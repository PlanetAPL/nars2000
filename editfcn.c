//***************************************************************************
//  NARS2000 -- Function Editor
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2016 Sudley Place Software

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

#define STRICT
#include <windows.h>
#include <windowsx.h>
#include "headers.h"
#include "unitranstab.h"
#include "scancodes.h"
#include "debug.h"              // For xxx_TEMP_OPEN macros

extern DWORD LclGetTabbedTextExtentW (HFONT, HFONT, HDC, LPWSTR, int, int, const LPINT);

// ToDo
/*
 * Redo
 * Move Undo Code into EDITCTRL.C
 * Allow external editor
 *

 */

#ifdef DEBUG
//#define DEBUG_WM_KEYDOWN
//#define DEBUG_WM_KEYUP
//#define DEBUG_WM_CHAR
#endif

extern UBOOL bMainDestroy;
extern TKACTSTR fsaActTableTK [][TKCOL_LENGTH];
char szCloseMessage[] = "You have changed the body of this function;"
                        " save the changes?";

// VK_CANCEL scan code for some keyboards
// The initial value is unimportant as it is overridden
//    by the first and all subsequent calls to VK_CANCEL.
UINT uCancelCode = 0x46;

// Define struct for passing parameters to WM_NCCREATE/WM_CREATE
//   for the Function Edit window
typedef struct tagFE_CREATESTRUCTW
{
    LPAPLCHAR lpwszLine;
    UINT      ErrCode;
} UNALIGNED FE_CREATESTRUCTW, *LPFE_CREATESTRUCTW;


typedef struct tagCLIPFMTS
{
    UINT    uFmtNum;                // Format #
    HGLOBAL hGlbFmt;                // Handle for this format #
} CLIPFMTS, *LPCLIPFMTS;


typedef enum tagFCNMEMVIRTENUM
{
    FCNMEMVIRT_UNDOBEG,                 // 00:  lpUndoBeg
    FCNMEMVIRT_LENGTH                   // 01:  # entries
} FCNMEMVIRTENUM;


//***************************************************************************
//  $CreateFcnWindow
//
//  Create a function window
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CreateFcnWindow"
#else
#define APPEND_NAME
#endif

UBOOL CreateFcnWindow
    (LPWCHAR lpwszLine)             // Ptr to text after {del}

{
    HWND             hWnd,              // Handle for Function Editor window
                     hWndNxt;           // Next window handle in doubly-linked list
                                        //   of FE window handles
    FE_CREATESTRUCTW feCreateStructW;   // CreateStruct for Function Editor window
    LPPERTABDATA     lpMemPTD;          // Ptr to PerTabData global memory
    UINT             uLen;              // Length of the function name

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Skip over the initial UTF16_DEL, if present
    if (lpwszLine[0] EQ UTF16_DEL)
        lpwszLine++;

    // Skip over leading blanks
    while (isspaceW (*lpwszLine))
        lpwszLine++;

    // Calculate the length of the function name
    uLen = (UINT) (SkipToCharW (lpwszLine, L';') - lpwszLine);

    // If there's a name, ...
    if (uLen NE 0)
    {
        // Check to see if this function name is already being edited
        // If so, switch to that window instead of opening a new one
        hWndNxt = lpMemPTD->hWndFENxt;

        while (hWndNxt NE NULL)
        {
            // Ask the window if this function name is theirs
            if (SendMessageW (hWndNxt, MYWM_CMPNAME, uLen, (LPARAM) (HANDLE_PTR) lpwszLine))
                break;
            // Get the next FE window handle in sequence
            hWndNxt = (HWND) GetWindowLongPtrW (hWndNxt, GWLFE_HWNDNXT);
        } // End WHILE

        // If we found a matching window, ...
        if (hWndNxt NE NULL)
        {
            // Activate the other window
            SendMessageW (lpMemPTD->hWndMC, WM_MDIACTIVATE, (WPARAM) hWndNxt, 0);

            return TRUE;
        } // End IF
    } // End IF

    // Save a ptr to the line
    feCreateStructW.lpwszLine = lpwszLine;

    // Initialize the return error code
    feCreateStructW.ErrCode   = 0;

    hWnd =
      CreateMDIWindowW (LFEWNDCLASS,        // Class name
                        NULL,               // Window title
                        0,                  // Styles
                        CW_USEDEFAULT,      // X-pos
                        CW_USEDEFAULT,      // Y-pos
                        CW_USEDEFAULT,      // Height
                        CW_USEDEFAULT,      // Width
                        lpMemPTD->hWndMC,   // Parent
                        _hInstance,         // Instance
              (LPARAM) &feCreateStructW);   // Extra data,
    // If it didn't succeed, ...
    if (hWnd EQ NULL)
    {
        switch (feCreateStructW.ErrCode)
        {
            case NAMETYPE_VAR:
                MB (pszNoEditVars);

                break;

            case NAMETYPE_FN12:
            case NAMETYPE_OP1:
            case NAMETYPE_OP2:
            case NAMETYPE_OP3:
            case NAMETYPE_TRN:
                MB (pszNoEditPrimFns);

                break;

            case NAMETYPE_UNK:
            case NAMETYPE_FN0:
                MB (pszNoCreateFEWnd);

                break;

            defstop
                break;
        } // End SWITCH

        return FALSE;
    } // End IF

    // Make the window visible; update its client area
    ShowWindow (hWnd, SW_SHOWNORMAL);
    UpdateWindow (hWnd);

    // Set the appropriate font in place
    SendMessageW (hWnd, WM_SETFONT, (WPARAM) GetFSIndFontHandle (FONTENUM_FE), MAKELPARAM (TRUE, 0));

    return TRUE;
} // End CreateFcnWindow
#undef  APPEND_NAME


//***************************************************************************
//  $LftMarginsFE
//
//  Return the left margin for a Function Editor window
//***************************************************************************

UINT LftMarginsFE
    (HWND  hWndEC,          // Window handle to the Edit Ctrl
     UBOOL bIncludeLC)      // TRUE iff we should include the Line Continuation markers

{
    UINT uLeft;             // Left margin
    HWND hWndParent;        // Window handle of parent

    // Get the window handle of the parent window
    hWndParent = GetParent (hWndEC);

    Assert (IzitFE (hWndParent));

    // Make room for Line Continuation markers
    uLeft = bIncludeLC ? uWidthLC[FONTENUM_FE] : 0;

    // If we're displaying function line #s, ...
    if (GetWindowLongW (hWndParent, GWLSF_FLN))
        // Make room for the function line numbers and
        uLeft += FCN_INDENT * GetFSIndAveCharSize (FONTENUM_FE)->cx;
    return uLeft;
} // End LftMarginsFE


//***************************************************************************
//  $SetMarginsFE
//
//  Set the margins for a Function Editor window
//***************************************************************************

void SetMarginsFE
    (HWND hWndEC)           // Window handle to the Edit Ctrl

{
    UINT uLeft;             // Left margin

    // Return the left margin
    uLeft = LftMarginsFE (hWndEC, TRUE);

    // Tell the Edit Ctrl about it
    SendMessageW (hWndEC, EM_SETMARGINS, EC_LEFTMARGIN, MAKELONG (uLeft, 0));
} // End SetMarginsFE


//***************************************************************************
//  $IzitAFE
//
//  Is this is an Edit Ctrl from an AFO with bMakeAfe
//***************************************************************************

UBOOL IzitAFE
    (HWND   hWndEC,                 // Window handle to Edit Ctrl
     UBOOL  bAllow)                 // TRUE iff we should allow the operation regardless of the line #

{
    HGLOBAL      hGlbDfnHdr;        // User-defined function/operator header global memory handle
    LPDFN_HEADER lpMemDfnHdr;       // Ptr to user-defined function/operator header global memory
    UBOOL        bAFO = FALSE;      // TRUE iff this is an AFO

    if (!IzitFE (GetParent (hWndEC)))
        return FALSE;

    // Get the previous function global memory handle (if any)
    hGlbDfnHdr = (HGLOBAL) GetWindowLongPtrW (GetParent (hWndEC), GWLSF_HGLBDFNHDR);

    // If the handle is valid, ...
    if (hGlbDfnHdr NE NULL && !bAllow)
    {
        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLockDfn (hGlbDfnHdr);

        // Save the flag
        bAFO = lpMemDfnHdr->bAFO;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
    } // End IF

    // If this is an AFO, ...
    if (bAFO && !bAllow)
    {
        UINT uLineNum;

        // Get the current line #
        uLineNum = (UINT) SendMessageW (hWndEC, EM_LINEFROMCHAR, (WPARAM) -1, 0);

        // If the selection is on the header (line 0), ...
        if (uLineNum EQ 0)
        {
            MessageBeep (MB_ICONWARNING);

            return TRUE;
        } // End IF
    } // End IF

    return FALSE;
} // End IzitAFE


//***************************************************************************
//  $ReplaceSel
//
//  Replace a selection in a FE Edit Ctrl
//***************************************************************************

void ReplaceSel
    (HWND   hWndEC,                 // Window handle to Edit Ctrl
     UBOOL  bAllow,                 // TRUE iff we should allow the operation regardless of the line #
     LPARAM lParam)                 // Ptr to the replacement text

{
    // Izit an AFO called with bMakeAFE?
    if (IzitAFE (hWndEC, bAllow))
        // Pass on the message as a NOP
        SendMessageW (hWndEC , EM_REPLACESEL, FALSE, (LPARAM) L"");
    else
        // Pass on the message
        SendMessageW (hWndEC , EM_REPLACESEL, FALSE, lParam);
} // End ReplaceSel


//***************************************************************************
//  $FE_Create
//
//  Perform window-specific initialization
//***************************************************************************

void FE_Create
    (HWND hWnd)

{
} // End FE_Create


//***************************************************************************
//  $FE_Delete
//
//  Perform window-specific uninitialization
//***************************************************************************

void FE_Delete
    (HWND hWnd)

{
} // End FE_Delete


//***************************************************************************
//  $FEWndProc
//
//  Message processing routine for the Function Editor window
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FEWndProc"
#else
#define APPEND_NAME
#endif

LRESULT APIENTRY FEWndProc
    (HWND   hWnd,                   // Window handle
     UINT   message,                // Type of message
     WPARAM wParam,                 // Additional information
     LPARAM lParam)                 // ...

{
    HWND         hWndEC,            // Handle of Edit Ctrl window
                 hWndNxt,           // Next window handle in doubly-linked list
                 hWndPrv;           // Previous ...
    LPUNDO_BUF   lpUndoBeg,         // Ptr to start of Undo Buffer
                 lpUndoNxt;         // ...    next available slot in the Undo Buffer
    LPMEMVIRTSTR lpLclMemVirtStr;   // Ptr to local MemVirtStr
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

////static DWORD aHelpIDs[] = {
////                           IDOK,             IDH_OK,
////                           IDCANCEL,         IDH_CANCEL,
////                           IDC_APPLY,        IDH_APPLY,
////                           IDC_HELP2,        IDH_HELP2,
////                           0,             0,
////                          };
////LCLODSAPI ("FE: ", hWnd, message, wParam, lParam);

    // Get the handle to the Edit Ctrl
    hWndEC = (HWND) GetWindowLongPtrW (hWnd, GWLSF_HWNDEC);

    switch (message)
    {
#define lpMDIcs     ((LPMDICREATESTRUCTW) ((*(LPCREATESTRUCTW *) &lParam)->lpCreateParams))
        case WM_NCCREATE:
            // Allocate room for FCNMEMVIRT_LENGTH MemVirtStrs
            //  (see FCNMEMVIRTENUM)
            lpLclMemVirtStr =
              MyVirtualAlloc (NULL,                     // Any address (FIXED SIZE)
                              FCNMEMVIRT_LENGTH * sizeof (MEMVIRTSTR),
                              MEM_COMMIT | MEM_TOP_DOWN,
                              PAGE_READWRITE);
            if (lpLclMemVirtStr EQ NULL)
            {
                // ***FIXME*** -- Display error msg
                DbgMsgW (L"FEWndProc/WM_NCCREATE:  MyVirtualAlloc for <lpLclMemVirtStr> failed");

                return -1;
            } // End IF

            // Save the ptr in window extra bytes
            SetWindowLongPtrW (hWnd, GWLSF_LPMVS, (APLU3264) (LONG_PTR) lpLclMemVirtStr);

            // Save the initial function line # display state in window extra bytes
            SetWindowLongW (hWnd, GWLSF_FLN, OptionFlags.bDefDispFcnLineNums);

            break;
#undef  lpMDIcs

#define lpMDIcs     ((LPMDICREATESTRUCTW) ((*(LPCREATESTRUCTW *) &lParam)->lpCreateParams))
        case WM_CREATE:             // lpcs = (LPCREATESTRUCTW) lParam; // Structure with creation data
        {
            LPSYMENTRY   lpSymName;         // Ptr to function name STE
            HGLOBAL      hGlbDfnHdr = NULL; // User-defined function/operator header global memory handle
            LPDFN_HEADER lpMemDfnHdr;       // Ptr to user-defined function/operator header global memory

            // Get ptr to PerTabData global memory
            lpMemPTD = GetMemPTD ();

            // Initialize variables
            cfFE.hwndOwner = hWnd;

            // Save the ptr to this window's menu positions
            SetPropW (hWnd, PROP_IDMPOSFN, (HANDLE) GetIDMPOS_FE);

            // See if there is an existing function
            lpSymName = ParseFunctionName (hWnd, (*(LPFE_CREATESTRUCTW *) &lpMDIcs->lParam)->lpwszLine);
            if (lpSymName NE NULL)
            {
                // If it has a value and is not a user-defined function/operator, ...
                if (lpSymName->stFlags.Value
                 && !lpSymName->stFlags.UsrDfn)
                {
                    ((LPFE_CREATESTRUCTW) (lpMDIcs->lParam))->ErrCode = lpSymName->stFlags.stNameType;

                    return -1;
                } else
                if (lpSymName->stData.stGlbData NE NULL)
                {
                    // Get the global memory handle
                    hGlbDfnHdr = lpSymName->stData.stGlbData;

                    // Lock the memory to get a ptr to it
                    lpMemDfnHdr = MyGlobalLockDfn (hGlbDfnHdr);
                } // End IF/ELSE
            } // End IF

            // Get the ptr to the local virtual memory struc
            (HANDLE_PTR) lpLclMemVirtStr = GetWindowLongPtrW (hWnd, GWLSF_LPMVS);

            // *************** Undo Buffer *****************************
            // _BEG is the (static) ptr to the beginning of the virtual memory.
            // _NXT is the (dynamic) ptr to the next available entry.
            //    Undo entries are between _NXT[-1] and _BEG, inclusive.
            // _LST is the (dynamic) ptr to the last available entry.
            //    Redo entries are between _NXT and _LST[-1], inclusive.

            // Allocate virtual memory for the Undo Buffer
            lpLclMemVirtStr[FCNMEMVIRT_UNDOBEG].lpText   = "lpUndoBeg in <FEWndProc>";
            lpLclMemVirtStr[FCNMEMVIRT_UNDOBEG].IncrSize = DEF_UNDOBUF_INCRNELM * sizeof (UNDO_BUF);
            lpLclMemVirtStr[FCNMEMVIRT_UNDOBEG].MaxSize  = DEF_UNDOBUF_MAXNELM  * sizeof (UNDO_BUF);
            lpLclMemVirtStr[FCNMEMVIRT_UNDOBEG].IniAddr  = (LPUCHAR)
            lpUndoBeg =
              GuardAlloc (NULL,             // Any address
                          lpLclMemVirtStr[FCNMEMVIRT_UNDOBEG].MaxSize,
                          MEM_RESERVE,
                          PAGE_READWRITE);
            if (lpLclMemVirtStr[FCNMEMVIRT_UNDOBEG].IniAddr EQ NULL)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsgW (L"FEWndProc/WM_CREATE:  VirtualAlloc for <lpUndoBeg> failed");

                return -1;
            } // End IF

            // Link this struc into the chain
            LinkMVS (&lpLclMemVirtStr[FCNMEMVIRT_UNDOBEG]);

            // Commit the intial size
            MyVirtualAlloc (lpLclMemVirtStr[FCNMEMVIRT_UNDOBEG].IniAddr,
                            DEF_UNDOBUF_INITNELM * sizeof (UNDO_BUF),
                            MEM_COMMIT,
                            PAGE_READWRITE);
            // Save in window extra bytes
            SetWindowLongPtrW (hWnd, GWLSF_UNDO_BEG, (APLU3264) (LONG_PTR) lpUndoBeg);
            SetWindowLongPtrW (hWnd, GWLSF_UNDO_NXT, (APLU3264) (LONG_PTR) lpUndoBeg);
            SetWindowLongPtrW (hWnd, GWLSF_UNDO_LST, (APLU3264) (LONG_PTR) lpUndoBeg);
////////////SetWindowLongW (hWnd, GWLSF_UNDO_GRP, 0);    // Already zero

            // Start with an initial action of nothing
            AppendUndo (hWnd,                       // FE Window handle
                        GWLSF_UNDO_NXT,             // Offset in hWnd extra bytes of lpUndoNxt
                        undoNone,                   // Action
                        0,                          // Beginning char position
                        0,                          // Ending    ...
                        UNDO_NOGROUP,               // Group index
                        0);                         // Character
            // Save incremented starting ptr in window extra bytes
            SetWindowLongPtrW (hWnd, GWLSF_UNDO_BEG, (APLU3264) (LONG_PTR) ++lpUndoBeg);

            // If there's a pre-existing function,
            //   and there's an Undo Buffer
            if (hGlbDfnHdr NE NULL
             && lpMemDfnHdr->hGlbUndoBuff)
            {
                LPUNDO_BUF lpMemUndo;       // Ptr to Undo Buffer global memory
                SIZE_T     uUndoSize;       // Size of Undo Buffer in bytes
                HGLOBAL    hGlbUndoBuff;

                // Get the handle
                hGlbUndoBuff = lpMemDfnHdr->hGlbUndoBuff;

                // Get the size in bytes
                uUndoSize = MyGlobalSize (hGlbUndoBuff);

                // Lock the memory to get a ptr to it
                lpMemUndo = MyGlobalLock (hGlbUndoBuff);

                __try
                {
                    // Copy the previous Undo Buffer contents
                    CopyMemory (lpUndoBeg, lpMemUndo, uUndoSize);
                } __except (CheckVirtAlloc (GetExceptionInformation (),
                                            L"FEWndProc/WM_CREATE"))
                {} // End __try/__except

                // We no longer need this ptr
                MyGlobalUnlock (hGlbUndoBuff); lpMemUndo = NULL;

                // Get the ptr to the next available entry
                lpUndoNxt = (LPUNDO_BUF) ByteAddr (&lpUndoBeg[1], uUndoSize);

                    // Save in window extra bytes
                SetWindowLongPtrW (hWnd, GWLSF_UNDO_NXT, (APLU3264) (LONG_PTR) lpUndoNxt);
                SetWindowLongPtrW (hWnd, GWLSF_UNDO_LST, (APLU3264) (LONG_PTR) lpUndoNxt);
            } // End IF

            // Create an Edit Ctrl within which we can edit
            hWndEC =
            CreateWindowExW (0L,                    // Extended styles
                             LECWNDCLASS,           // Class name
                             L"",                   // Initial text
                             0
                           | WS_CHILD
                           | WS_VSCROLL
                           | ES_MULTILINE
                           | ES_WANTRETURN
                           | ES_AUTOHSCROLL
                           | ES_AUTOVSCROLL
                             ,                      // Styles
                             0,                     // X-position
                             0,                     // Y-...
                             CW_USEDEFAULT,         // Width
                             CW_USEDEFAULT,         // Height
                             hWnd,                  // Parent window
                             (HMENU) IDWC_FE_EC,    // ID
                             _hInstance,            // Instance
                             0);                    // lParam

            if (hWndEC EQ NULL)
            {
                MB (pszNoCreateFEEditCtrl);

                return -1;          // Stop the whole process
            } // End IF

            // Save in window extra bytes
            SetWindowLongPtrW (hWnd, GWLSF_HWNDEC    , (APLU3264) (LONG_PTR) hWndEC);
            SetWindowLongPtrW (hWnd, GWLSF_HGLBDFNHDR, (APLU3264) (LONG_PTR) hGlbDfnHdr);

            // Set the paint hook
            SendMessageW (hWndEC, EM_SETPAINTHOOK, 0, (LPARAM) &LclECPaintHook);

            // Set the soft-break flag
            SendMessageW (hWndEC, EM_FMTLINES, TRUE, 0);

            // If there's a pre-existing function,
            //   read in its lines as the initial text
            if (hGlbDfnHdr NE NULL)
            {
                HGLOBAL        hGlbTxtLine;     // Line/header text global memory handle
                LPMEMTXT_UNION lpMemTxtLine;    // Ptr to header/line text global memory
                UINT           numFcnLines,     // # lines in the function
                               uLineNum;        // Line #
                LPFCNLINE      lpFcnLines;      // Ptr to array function line structs (FCNLINE[numFcnLines])

                // If this function is an AFO, ...
                if (lpMemDfnHdr->bAFO)
                {
                    HGLOBAL hGlbName;           // Name text global memory handle
                    LPWCHAR lpMemName;          // Ptr to the name's text

                    Assert (lpSymName NE NULL);

                    // Get the global memory handle of the AFO name
                    hGlbName = lpSymName->stHshEntry->htGlbName;

                    // Lock the memory to get a ptr to it
                    lpMemName = MyGlobalLockWsz (hGlbName);

                    // Append the text to the Edit Ctrl
                    ReplaceSel (hWndEC, TRUE, (LPARAM) lpMemName);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbName); lpMemName = NULL;
                } else
                {
                    // Get the function header text global memory handle
                    hGlbTxtLine = lpMemDfnHdr->hGlbTxtHdr;

                    // Lock the memory to get a ptr to it
                    lpMemTxtLine = MyGlobalLockTxt (hGlbTxtLine);

                    // Append the text to the Edit Ctrl
                    ReplaceSel (hWndEC, TRUE, (LPARAM) &lpMemTxtLine->C);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;
                } // End IF/ELSE

                // Get the # logical lines in the function
                numFcnLines = lpMemDfnHdr->numFcnLines;

                // Get ptr to array of function line structs (FCNLINE[numFcnLines])
                lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

                // Loop through the lines, appending the text to the Edit Ctrl
                for (uLineNum = 0; uLineNum < numFcnLines; uLineNum++)
                {
                    // Append a CRLF to the Edit Ctrl
                    ReplaceSel (hWndEC, TRUE, (LPARAM) WS_CRLF);

                    // Get the line text global memory handle
                    hGlbTxtLine = lpFcnLines->hGlbTxtLine;

                    // Lock the memory to get a ptr to it
                    lpMemTxtLine = MyGlobalLockTxt (hGlbTxtLine);

                    // Append the text to the Edit Ctrl
                    ReplaceSel (hWndEC, TRUE, (LPARAM) &lpMemTxtLine->C);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;

                    // Skip to the next struct
                    lpFcnLines++;
                } // End FOR
            } else
                // Set the initial text
                ReplaceSel (hWndEC,
                            TRUE,
                   (LPARAM) (((LPFE_CREATESTRUCTW) (lpMDIcs->lParam))->lpwszLine));
            // Mark as no changes so far
            SetWindowLongW (hWnd, GWLSF_CHANGED, FALSE);

            // If there's a pre-existing function, ...
            if (hGlbDfnHdr NE NULL)
            {
                // We no longer need this ptr
                MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
            } // End IF

            // Paint the window
            ShowWindow (hWndEC, SW_SHOWNORMAL);
            UpdateWindow (hWndEC);

            // Use Post here as we need to wait for the EC window
            //   to be drawn.
            PostMessageW (hWnd, MYWM_INIT_EC, 0, 0);

            // Link this FE window into the doubly-linked list of FE windows
            hWndNxt = lpMemPTD->hWndFENxt;

            // Save as our next window
            SetWindowLongPtrW (hWnd, GWLFE_HWNDNXT, (APLU3264) (LONG_PTR) hWndNxt);

            // Clear our prev window
            SetWindowLongPtrW (hWnd, GWLFE_HWNDPRV, (APLU3264) (LONG_PTR) NULL);

            // If there are other FE windows, ...
            if (hWndNxt NE NULL)
            {
                Assert ((HWND) GetWindowLongPtrW (hWndNxt, GWLFE_HWNDPRV) EQ NULL);

                // Save our window handle as the next's prev
                SetWindowLongPtrW (hWndNxt, GWLFE_HWNDPRV, (APLU3264) (LONG_PTR) hWnd);
            } // End IF

            // Save our window handle as the new next
            lpMemPTD->hWndFENxt = hWnd;

            break;
        } // End WM_CREATE
#undef  lpMDIcs

        case MYWM_INIT_EC:
        {
            UINT  uLineLen;     // Line length
            UBOOL bAFO;         // TRUE iff this window is an AFO

            // Write out the FE window title
            bAFO = SetFETitle (hWnd);

            // Draw the line #s
            DrawLineNumsFE (hWndEC);

            // Get the length of the function header block
            uLineLen = GetBlockLength (hWndEC, 0);

            // If it's an AFO, ...
            if (bAFO)
                // Plus the line terminator and the length of the first line
                uLineLen  = sizeof (WS_CRLF) + GetBlockLength (hWndEC, 1);
            // Set the caret to that character
            SendMessageW (hWndEC, EM_SETSEL, uLineLen, uLineLen);

            return FALSE;           // We handled the msg
        } // End MYWM_INIT_EC

#define uLen        ((UINT) wParam)
#define lpwName     ((LPWCHAR) lParam)
        case MYWM_CMPNAME:          // uLen = (UINT) wParam
                                    // lpwName = (LPWCHAR) lParam
        {
            UINT     uNameLen;      // Function name length
            LPWCHAR  lpwszTemp;     // Ptr to temporary storage
            LRESULT  lResult;       // Result
            VARS_TEMP_OPEN

            // Get ptr to PerTabData global memory
            lpMemPTD = GetMemPTD ();

            // Get ptr to temporary storage
            lpwszTemp = lpMemPTD->lpwszTemp;
            CHECK_TEMP_OPEN

            // Get this window's function name (if any)
            uNameLen = GetFunctionName (hWnd, lpwszTemp, NULL);

            if (uNameLen NE 0 && uNameLen EQ uLen)
                lResult = (strncmpW (lpwszTemp, lpwName, uNameLen) EQ 0);
            else
                lResult = FALSE;

            EXIT_TEMP_OPEN

            return lResult;         // TRUE iff the names are equal
        } // End MYWM_CMPNAME
#undef  lpwName
#undef  uLen

        case MYWM_SAVE_FN:
            // Save the function (if well-formed)
            SaveFunction (hWnd);

            return FALSE;           // We handled the msg

        case MYWM_SAVECLOSE_FN:
            // Save the function (if well-formed)
            if (SaveFunction (hWnd))
                // Close the function (if allowed)
                CloseFunction (hWnd);

            return FALSE;           // We handled the msg

        case MYWM_CLOSE_FN:
            // Close the function (if allowed)
            CloseFunction (hWnd);

            return FALSE;           // We handled the msg

////////case MYWM_SAVE_AS_FN:
////////    // Save the function (if well-formed) under a different name
////////    SaveAsFunction (hWnd);
////////
////////    return FALSE;           // We handled the msg

        case WM_SYSCOLORCHANGE:
        case WM_SETTINGCHANGE:
            // Uninitialize window-specific resources
            FE_Delete (hWnd);

            // Initialize window-specific resources
            FE_Create (hWnd);

            break;                  // Continue with next handler

        case WM_CTLCOLOREDIT:       // hdcEdit = (HDC) wParam;   // handle of display context
                                    // hwndEdit = (HWND) lParam; // handle of static control
            // Ensure it's from our Edit Ctrl
            if (hWndEC EQ (HWND) lParam)
                return (LRESULT) ghBrushBG;
            else
                break;

#define lpnmEC  (*(LPNMEDITCTRL *) &lParam)
        case WM_NOTIFY:             // idCtrl = (int) wParam;
                                    // pnmh = (LPNMHDR) lParam;
            // Ensure it's from our Edit Ctrl
            if (lpnmEC->nmHdr.hwndFrom EQ hWndEC)
                // Pass on to LclEditCtrlWndProc
                SendMessageW (hWndEC, MYWM_NOTIFY, wParam, lParam);

            return FALSE;           // We handled the msg
#undef  lpnmEC

#define fwSizeType  wParam
#define nWidth      (LOWORD (lParam))
#define nHeight     (HIWORD (lParam))
        case WM_SIZE:               // fwSizeType = wParam;      // Resizing flag
                                    // nWidth = LOWORD(lParam);  // Width of client area
                                    // nHeight = HIWORD(lParam); // Height of client area
            if (fwSizeType NE SIZE_MINIMIZED)
                SetWindowPos (hWndEC,           // Window handle to position
                              0,                // SWP_NOZORDER
                              0,                // X-position
                              0,                // Y-...
                              nWidth,           // Width
                              nHeight,          // Height
                              SWP_NOZORDER      // Flags
                            | SWP_SHOWWINDOW
                             );
            break;                  // *MUST* pass on to DefMDIChildProcW
#undef  nHeight
#undef  nWidth
#undef  fwSizeType

        case WM_SETFONT:
            // Pass on to the Edit Ctrl
            SendMessageW (hWndEC, message, wParam, lParam);

            // Changing the font also means changing the size
            //   of the margins as the character width might change
            SetMarginsFE (hWndEC);

            return FALSE;           // We handled the msg

        case MYWM_SETFOCUS:
            // Set the focus to the Function Editor so the cursor displays
            SetFocus (hWnd);

            return FALSE;           // We handled the msg

        case WM_SETFOCUS:
            // Pass on to the Edit Ctrl
            SetFocus (hWndEC);

            // Draw the line #s
            DrawLineNumsFE (hWndEC);

            break;                  // *MUST* pass on to DefMDIChildProcW

        case WM_MDIACTIVATE:        // Activate/de-activate a child window
            // If we're being activated, ...
            if (GET_WM_MDIACTIVATE_FACTIVATE (hWnd, wParam, lParam))
            {
                ActivateMDIMenu (WINDOWCLASS_FE, hWnd);
                SetFocus (hWnd);
                SendMessageW (hWndEC, EM_SCROLLCARET, 0, 0);
            } // End IF

            break;                  // Continue with DefMDIChildProcW

        case WM_UNDO:
        case MYWM_REDO:
        case WM_COPY:
        case MYWM_COPY_APL:
        case WM_CUT:
        case WM_PASTE:
        case MYWM_PASTE_APL:
        case WM_CLEAR:
        case MYWM_SELECTALL:
            // Pass on to the Edit Ctrl
            SendMessageW (hWndEC, message, wParam, lParam);

            return FALSE;           // We handled the msg

#define wNotifyCode     (HIWORD (wParam))
#define wID             (LOWORD (wParam))
#define hWndCtrl        MakeGlbFromVal (lParam)
        case WM_COMMAND:            // wNotifyCode = HIWORD (wParam); // Notification code
                                    // wID = LOWORD (wParam);         // Item, control, or accelerator identifier
                                    // hwndCtrl = (HWND) lParam;      // Handle of control
            // This message should be from the Edit Ctrl
            Assert (wID      EQ IDWC_FE_EC);
            Assert (hWndCtrl EQ hWndEC);

            // Split cases based upon the notify code
            switch (wNotifyCode)
            {
                UBOOL bChanged;
                UINT  iLinePos,
                      iLineNum;

                case EN_CHANGE:                     // idEditCtrl = (int) LOWORD(wParam); // identifier of Edit Ctrl
                                                    // hwndEditCtrl = (HWND) lParam;      // handle of Edit Ctrl
                    // Split cases based upon the last key
                    switch (GetWindowLongW (hWnd, GWLSF_LASTKEY))
                    {
                        case VK_RETURN:
                        case VK_BACK:
                        case VK_DELETE:
                            // Draw the line #s
                            DrawLineNumsFE (hWndEC);

                            break;
                    } // End SWITCH

                    // Get the current changed flag
                    bChanged = GetWindowLongW (hWnd, GWLSF_CHANGED);

                    // The contents of the Edit Ctrl have changed,
                    // set the changed flag
                    SetWindowLongW (hWnd, GWLSF_CHANGED, TRUE);

                    // If the Changed flag has changed, ...
                    if (!bChanged)
                        // Write out the FE window title
                        SetFETitle (hWnd);

                    // Get the char index of the first char in the current line
                    iLinePos = (UINT) SendMessageW (hWndEC, EM_LINEINDEX, -1, 0);

                    // Get the line number that contains the specified position
                    iLineNum = (UINT) SendMessageW (hWndEC, EM_LINEFROMCHAR, iLinePos, 0);

                    // If the cursor is in block #0 (the function header),
                    //   the user might have changed the name of the function,
                    //   so we need to rewrite the Function Editor window title,
                    //   or if Syntax Coloring is in effect, the user might
                    //   have changed the localization status of a var,
                    //   so we need to repaint the whole window to reflect
                    //   the possible change in syntax colors.
                    if (0 EQ GetBlockStartLine (hWndEC, iLineNum))
                    {
                        // If the Changed flag didn't change, ...
                        if (bChanged)
                            // Write out the FE window title
                            SetFETitle (hWnd);

                        // If we're Syntax Coloring function lines, ...
                        if (OptionFlags.bSyntClrFcns)
                            InvalidateRect (hWndEC, NULL, FALSE);
                    } // End IF

                    break;

                case EN_MAXTEXT:    // idEditCtrl = (int) LOWORD(wParam); // Identifier of Edit Ctrl
                                    // hwndEditCtrl = (HWND) lParam;      // Handle of Edit Ctrl
                    // The Edit Ctrl has exceed its maximum # chars

                    // The default maximum is 32K, so we increase it by that amount
                    SendMessageW (hWndEC,
                                  EM_SETLIMITTEXT,
                                  SendMessageW (hWndEC, EM_GETLIMITTEXT, 0, 0) + 32*1024, 0);
                    break;

////            case EN_SETFOCUS:   // 0x0100
////            case EN_KILLFOCUS:  // 0x0200
////            case EN_CHANGE:     // 0x0300
////            case EN_UPDATE:     // 0x0400
////            case EN_ERRSPACE:   // 0x0500
////            case EN_MAXTEXT:    // 0x0501
////            case EN_HSCROLL:    // 0x0601
////            case EN_VSCROLL:    // 0x0602
                    break;
            } // End SWITCH

            break;
#undef  hWndCtrl
#undef  wID
#undef  wNotifyCode

        case WM_QUERYENDSESSION:
        case WM_CLOSE:
            // If it's OK to close the window, do so, else ignore
            if (!QueryCloseFE (hWnd))
                return FALSE;       // Do not close

            break;                  // Default action is to close

        case WM_DESTROY:
            // *************** FCNMEMVIRTENUM Entries ******************
            // Get the MemVirtStr ptr
            lpLclMemVirtStr = (LPMEMVIRTSTR) GetWindowLongPtrW (hWnd, GWLSF_LPMVS);
            if (lpLclMemVirtStr NE NULL)
            {
                UINT uCnt;                  // Loop counter

                // Loop through the entries
                for (uCnt = 0; uCnt < FCNMEMVIRT_LENGTH; uCnt++)
                    // Free the virtual memory and unlink it
                    FreeVirtUnlink (uCnt, lpLclMemVirtStr);

                // Free the virtual storage
                MyVirtualFree (lpLclMemVirtStr, 0, MEM_RELEASE); lpLclMemVirtStr = NULL;

                // Zap it in case we come through again
                SetWindowLongPtrW (hWnd, GWLSF_LPMVS, (APLU3264) (LONG_PTR) NULL);
            } // End IF

            // Unlink this window from the doubly-linked chain of FE windows
            hWndNxt = (HWND) GetWindowLongPtrW (hWnd, GWLFE_HWNDNXT);
            hWndPrv = (HWND) GetWindowLongPtrW (hWnd, GWLFE_HWNDPRV);

            if (hWndPrv NE NULL)
                SetWindowLongPtrW (hWndPrv, GWLFE_HWNDNXT, (APLU3264) (LONG_PTR) hWndNxt);
            if (hWndNxt NE NULL)
                SetWindowLongPtrW (hWndNxt, GWLFE_HWNDPRV, (APLU3264) (LONG_PTR) hWndPrv);

            // Get ptr to PerTabData global memory
            lpMemPTD = GetMemPTD ();

            // If we're the current next, ...
            if (lpMemPTD->hWndFENxt EQ hWnd)
            {
                Assert (hWndPrv EQ NULL);

                lpMemPTD->hWndFENxt = hWndNxt;
            } // End IF

            // Uninitialize window-specific resources
            FE_Delete (hWnd);

            break;
    } // End SWITCH

////LCLODSAPI ("FEZ:", hWnd, message, wParam, lParam);
    return DefMDIChildProcW (hWnd, message, wParam, lParam);
} // End FEWndProc
#undef  APPEND_NAME


//***************************************************************************
//  $GetFunctionName
//
//  Parse the function name from a FE window and return it
//***************************************************************************

UINT GetFunctionName
    (HWND        hWndFE,            // FE window handle
     LPWCHAR     lpwszTemp,         // Ptr to temporary storage
     LPSYMENTRY *lplpSymName)       // Ptr to ptr to SYMENTRY (may be NULL)

{
    HWND         hWndEC;            // Edit Ctrl window handle
    UINT         uLineLen,          // Line length
                 uNameLen;          // Function name length
    LPSYMENTRY   lpSymName;         // Ptr to function name STE

    // Get the handle to the Edit Ctrl
    (HANDLE_PTR) hWndEC = GetWindowLongPtrW (hWndFE, GWLSF_HWNDEC);

    // Get the overall block length
    //   not including a terminating zero
    uLineLen = GetBlockLength (hWndEC, 0);

    // Tell EM_GETLINE maximum # chars in the buffer
    // The output array is a temporary so we don't have to
    //   worry about overwriting outside the allocated buffer
    ((LPWORD) lpwszTemp)[0] = (WORD) (uLineLen * sizeof (WCHAR));

    // Copy a block of lines
    //   including a terminating zero if there's enough room
    CopyBlockLines (hWndEC, 0, lpwszTemp);

    // Ensure the line is properly terminated
    lpwszTemp[uLineLen] = WC_EOS;

    // Get the header line text
    lpSymName = ParseFunctionName (hWndFE, lpwszTemp);

    if (lpSymName NE NULL)
        // Append the function name from the symbol table
        //   and calculate its length
        uNameLen = (UINT) (CopySteName (lpwszTemp, lpSymName, NULL) - lpwszTemp);
    else
        // Mark as no function name
        uNameLen = 0;

    // If we're returning the SymEntry, ...
    if (lplpSymName NE NULL)
        *lplpSymName = lpSymName;

    return uNameLen;
} // End GetFunctionName


//***************************************************************************
//  $SetFETitle
//
//  Write out the FE window title including the function name (if any)
//***************************************************************************

UBOOL SetFETitle
    (HWND hWndFE)                   // FE window handle

{
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPWCHAR      lpwszTemp;         // Ptr to temporary storage
    UINT         uNameLen;          // Function name length
    UBOOL        bAFO = FALSE;      // TRUE iff we're editing an AFO
    HGLOBAL      hGlbDfnHdr;        // Global memory handle of the previous function
    VARS_TEMP_OPEN

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get ptr to temporary storage
    lpwszTemp = lpMemPTD->lpwszTemp;
    CHECK_TEMP_OPEN

    // Get the function name (if any)
    uNameLen = GetFunctionName (hWndFE, lpwszTemp, NULL);

    // Ensure properly terminated
    //   and increment the name length to the next position
    lpwszTemp[uNameLen++] = WC_EOS;

    // Get the handle of the previous function
    // If we're editing an AFO, then we can be doing that only
    //   if there was a previous function.
    hGlbDfnHdr = (HGLOBAL) GetWindowLongPtrW (hWndFE, GWLSF_HGLBDFNHDR);

    // If it's valid, ...
    if (hGlbDfnHdr NE NULL)
    {
        LPDFN_HEADER lpMemDfnHdr;       // Ptr to user-defined function/operator header global memory

        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLockDfn (hGlbDfnHdr);

        // Save the AFO flag
        bAFO = lpMemDfnHdr->bAFO;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
    } // End IF

    // Format the new title in the memory following the name
    wsprintfW (&lpwszTemp[uNameLen],
                bAFO ? wszFETitle2 : wszFETitle,
                lpwszTemp,
                L" *"[GetWindowLongW (hWndFE, GWLSF_CHANGED)]);
    // Rewrite the window title with the (new?) function name
    SetWindowTextW (hWndFE, &lpwszTemp[uNameLen]);

    EXIT_TEMP_OPEN

    return bAFO;
} // End SetFETitle


//***************************************************************************
//  $SyntaxColor
//
//  Syntax color a line of text
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SyntaxColor"
#else
#define APPEND_NAME
#endif

UBOOL SyntaxColor
    (LPAPLCHAR lpwszLine,           // Ptr to line of text
     UINT      uLen,                // Length of text to display
     LPCLRCOL  lpMemClr,            // Ptr to array of Syntax Colors/Column Indices
     HWND      hWndEC)              // Window handle of Edit Ctrl (parent is SM or FE)

{
    UBOOL        bRet = FALSE;      // TRUE iff the result is valid
    UINT         uChar,             // Loop counter
                 uCharIni;          // Initial counter
    TK_ACTION    scAction1_EM,      // Ptr to 1st action
                 scAction2_EM;      // ...    2nd ...
    TKLOCALVARS  tkLocalVars = {0}; // Local vars
    WCHAR        wchOrig;           // The original char
    TKCOLINDICES tkColInd;          // The TKCOL_* of the translated char
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    HGLOBAL      hSyntClr = NULL;   // Syntax Color global memory handle
    LPSCINDICES  lpSyntClr = NULL;  // Ptr to Syntax Color entries
    SF_FCNS      SF_Fcns = {0};     // Temp value in case lpSF_Fcns is null

////LCLODS ("Entering <SyntaxColor>\r\n");

    // Allocate room for <uLen> Syntax Color entries
    hSyntClr = DbgGlobalAlloc (GHND, uLen * sizeof (SCINDICES));
    if (hSyntClr EQ NULL)
        goto CRIT_EXIT;

    // Lock the memory to get a ptr to it
    lpSyntClr = MyGlobalLock000 (hSyntClr);

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Save local vars in struct which we pass to each FSA action routine
    tkLocalVars.State[2]         =
    tkLocalVars.State[1]         =
    tkLocalVars.State[0]         = TKROW_SOS;       // Initialize the FSA state
    tkLocalVars.lpwszOrig        = lpwszLine;       // Save ptr to start of input line
////tkLocalVars.CtrlStrucTknType = 0;               // No initial token type (already zero from = {0})
////tkLocalVars.CtrlStrucStrLen  = 0;               // ...
    tkLocalVars.lpMemClrIni      =
    tkLocalVars.lpMemClrNxt      = lpMemClr;        // Save ptr to array of Syntax Colors
    tkLocalVars.lpGrpSeqIni      =
    tkLocalVars.lpGrpSeqNxt      = lpSyntClr;
    tkLocalVars.PrevGroup        = NO_PREVIOUS_GROUPING_SYMBOL;
    tkLocalVars.NameInit         = NO_PREVIOUS_NAME;
    tkLocalVars.hWndEC           = hWndEC;
    tkLocalVars.uSyntClrLen      = uLen;            // # Syntax Color entries
////tkLocalVars.bMFO             = FALSE;           // Not a Magic Function/Operator (already zero from = {0})
    tkLocalVars.lpMemPTD         = lpMemPTD;        // Ptr to PerTabData global memory
    tkLocalVars.lpSF_Fcns        = &SF_Fcns;        // Ptr to common struc (may be NULL if unused)

    // Set initial limit for hGlbNum
    tkLocalVars.iNumLim = DEF_NUM_INITNELM;

    // Allocate storage for hGlbNum
    tkLocalVars.hGlbNum =
      DbgGlobalAlloc (GHND, tkLocalVars.iNumLim * sizeof (char));
    if (!tkLocalVars.hGlbNum)
        goto FREEGLB_EXIT;

    // Set initial limit for hGlbStr
    tkLocalVars.iStrLim = DEF_STR_INITNELM;

    // Allocate storage for hGlbStr
    tkLocalVars.hGlbStr =
      DbgGlobalAlloc (GHND, tkLocalVars.iStrLim * sizeof (APLCHAR));
    if (!tkLocalVars.hGlbStr)
        goto FREEGLB_EXIT;

    // Initialize the accumulation variables for the next constant
    InitAccumVars (&tkLocalVars);

    // Skip over leading blanks (more to reduce clutter
    //   in the debugging window)
    for (uChar = 0; uChar < uLen; uChar++)
    if (IsWhiteW (lpwszLine[uChar]))
    {
        // Save the column index
        tkLocalVars.lpMemClrNxt->colIndex = TKCOL_SPACE;

        // Save the color
        tkLocalVars.lpMemClrNxt++->syntClr =
          gSyntaxColorText;
    } else
        break;

    // Save initial char index to compare against
    //   in case we get a leading colon
    tkLocalVars.uCharStart =
    tkLocalVars.uCharIni   = uChar;
    tkLocalVars.uActLen    = uLen;
    uCharIni = 0;

    // Handle leading right parens as system commands
    //   and leading del as function editor
    //   and as such are not to be colored
    if (lpwszLine[uChar] EQ UTF16_RIGHTPAREN
     || lpwszLine[uChar] EQ UTF16_DEL)
        goto FREEGLB_EXIT;

    for (     ; uChar <= uLen; uChar++)
    {
        // Use a FSA to tokenize the line

        // Save current index (may be modified by an action)
        tkLocalVars.uChar = uChar;

        // Save pointer to current wch
        tkLocalVars.lpwszCur = &lpwszLine[uChar];

        // Strip out EOL check so we don't confuse a zero-value char with EOL
        if (uChar EQ uLen)
        {
            wchOrig = WC_EOS;
            tkColInd = TKCOL_EOL;
        } else
        {
            wchOrig = lpwszLine[uChar];
            tkColInd = CharTransTK (wchOrig, &tkLocalVars);
        } // End IF/ELSE

        // Save the TKCOL_xxx value
        tkLocalVars.colIndex = tkColInd;
        if (uChar < uLen)
            tkLocalVars.lpMemClrNxt->colIndex = tkColInd;

        // Get primary action and new state
        scAction1_EM = fsaActTableTK[tkLocalVars.State[0]][tkColInd].scAction1;
        scAction2_EM = fsaActTableTK[tkLocalVars.State[0]][tkColInd].scAction2;
        SetTokenStatesTK (&tkLocalVars, fsaActTableTK[tkLocalVars.State[0]][tkColInd].iNewState);

        // Check for primary action
        if (scAction1_EM
         && !(*scAction1_EM) (&tkLocalVars))
            goto ERROR_EXIT;

        // Check for secondary action
        if (scAction2_EM
         && !(*scAction2_EM) (&tkLocalVars))
            goto ERROR_EXIT;

        // Split cases based upon the return code
        switch (tkLocalVars.State[0])
        {
            case TKROW_NONCE:
                goto NONCE_EXIT;

            case TKROW_EXIT:
                goto NORMAL_EXIT;
        } // End SWITCH

        // Get next index (may have been modified by an action)
        uChar = tkLocalVars.uChar;
    } // End FOR

    // In rare circumstances we fall through to this point.
    // I think what is happening is that the mouse selects
    //   parts of more than one line and moves during the WM_PAINT
    //   message such that the line-to-be-colored changes.  I think.
    goto NORMAL_EXIT;

NORMAL_EXIT:
    // Mark as successful
    bRet = TRUE;
FREEGLB_EXIT:
NONCE_EXIT:
ERROR_EXIT:
    // Ensure numeric length has been reset
    Assert (tkLocalVars.iNumLen EQ 0);
    Assert ((uChar - uCharIni) EQ (UINT) (tkLocalVars.lpMemClrNxt - lpMemClr));

    // Free the global memory:  hGlbNum
    if (tkLocalVars.hGlbNum)
    {
        DbgGlobalFree (tkLocalVars.hGlbNum); tkLocalVars.hGlbNum = NULL;
    } // End IF

    // Free the global memory:  hGlbStr
    if (tkLocalVars.hGlbStr)
    {
        DbgGlobalFree (tkLocalVars.hGlbStr); tkLocalVars.hGlbStr = NULL;
    } // End IF

    // Free the global memory:  hSyntClr
    if (hSyntClr NE NULL)
    {
        if (lpSyntClr NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hSyntClr); lpSyntClr = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hSyntClr); hSyntClr = NULL;
    } // End IF
CRIT_EXIT:
////LCLODS ("Exiting  <SyntaxColor>\r\n");

    return bRet;
} // End SyntaxColor
#undef  APPEND_NAME


//***************************************************************************
//  $LclECPaintHook
//
//  Local Edit Ctrl paint hook
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- LclECPaintHook"
#else
#define APPEND_NAME
#endif

int LclECPaintHook
    (HWND    hWndEC,                        // Window handle of Edit Ctrl
     HDC     hDC,                           // The Device Context
     int     x,                             // The x-coordinate (Client Area)
     int     y,                             // ... y- ...
     LPWCHAR lpwsz,                         // Ptr to start of line
     UINT    uCol,                          // Starting column in the line
     UINT    uLen,                          // Length of text to display
     long    lFlags,                        // PRF_ flags
     int     line_height,                   // Line height in screen coordinates
     int     char_width,                    // Average char width in screen coordinates
     UBOOL   rev)                           // TRUE iff we're displaying in reverse mode

{
    RECT             rcScr,                 // Rect for actual width/height in screen coordinates
                     rcAct;                 // ...                             printer/screen ...
    HFONT            hFontFB_NP,            // HFONT for Fallback font for NOT printing either SM or FE (whichever is active)
                     hFontFB_PR,            // ...                     for printing     either ...
                     hFontOld,              // Original font for restore in case we're Output Debugging
                     hFontCur,              // Current  ...
                     hFontNP,               // HFONT for non-printing
                     // Use the following two fonts (when printing or NOT printing)
                     hFontScrRG,            // HFONT for Regular chars -- in screen coordinates (if printing)
                     hFontScrFB;            // ...       Fallback chars   ...
    HGLOBAL          hGlbClr = NULL;        // Syntax Color global memory handle
    LPCLRCOL         lpMemClrIni = NULL;    // Ptr to Syntax Colors/Column Indices global memory
    long             cxAveChar,             // Width of average char in given screen
                     cxAveWidth,            //                       for a temp
                     cxAveFB;               // ...                   for hFontFB_SF or hFontFB_PR
    UINT             uCnt;                  // Loop counter
    WCHAR            wcCur;                 // Current char
#ifndef UNISCRIBE
    LPPERTABDATA     lpMemPTD;              // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();
#endif
    if (IzitSM (GetParent (hWndEC)))
    {
        // Setup Fallback fonts for printing and non-printing
        hFontFB_PR = hFontFB_PR_SM;
        hFontFB_NP = hFontFB_SM;
        hFontNP    = hFontSM;
    } else
    if (IzitFE (GetParent (hWndEC)))
    {
        // Setup Fallback fonts for printing and non-printing
        hFontFB_PR = hFontFB_PR_FE;
        hFontFB_NP = hFontFB_FE;
        hFontNP    = hFontFE;
#ifdef DEBUG
    } else
    {
        DbgBrk ();
#endif
    } // End IF/ELSE/...

    // If we're printing, ...
    if (lFlags & PRF_PRINTCLIENT)
    {
        LOGFONTW lfPR,                  // LOGFONT for PR
                 lfFB_PR;               // ...         FB_PR

        // Get the LOGFONTW structure for the font
        GetObjectW (hFontPR   , sizeof (lfPR)   , &lfPR   );
        GetObjectW (hFontFB_PR, sizeof (lfFB_PR), &lfFB_PR);

        // Convert the font from screen coords to printer coords
        lfPR   .lfHeight = MulDiv (lfPR   .lfHeight, GetLogPixelsY (hDC), GetLogPixelsY (NULL));
        lfFB_PR.lfHeight = MulDiv (lfFB_PR.lfHeight, GetLogPixelsY (hDC), GetLogPixelsY (NULL));

        // Use the following two fonts when printing
        // Make a font of it
        hFontScrRG = MyCreateFontIndirectW (&lfPR   );
        hFontScrFB = MyCreateFontIndirectW (&lfFB_PR);
    } else
    {
        // Use the following two fonts when NOT printing
        hFontScrRG = hFontNP;
        hFontScrFB = hFontFB_NP;
    } // End IF/ELSE

    // If we're not displaying in reverse color, ...
    if (!rev)
    {
        // If we're to Syntax Color the line, ...
        if ((IzitSM (GetParent (hWndEC)) && OptionFlags.bSyntClrSess)
         || (IzitFE (GetParent (hWndEC)) && OptionFlags.bSyntClrFcns))
        {
            // If we're printing a session, ...
            if (!((IzitSM (GetParent (hWndEC)) && OptionFlags.bSyntClrSess)
               && (lFlags & PRF_PRINTCLIENT)
               && !OptionFlags.bSyntClrPrnt))
            {
                // To do this, we use a FSA to parse the line from the start
                //   through the last char to display

                // Allocate space for the colors
                hGlbClr = DbgGlobalAlloc (GHND, (uCol + uLen) * sizeof (lpMemClrIni[0]));
                if (hGlbClr NE NULL)
                {
                    // Lock the memory to get a ptr to it
                    lpMemClrIni = MyGlobalLock000 (hGlbClr);

                    // Syntax color the line
                    if (!SyntaxColor (lpwsz, uCol + uLen, lpMemClrIni, hWndEC))
                    {
                        // Unlock and free (and set to NULL) a global name and ptr
                        UnlFreeGlbName (hGlbClr, lpMemClrIni);
                    } // End IF

                    // Set the Window text colors
                    SetTextColor (hDC, gSyntaxColorText.crFore);
                    SetBkColor   (hDC, gSyntaxColorText.crBack);
                } // End IF
            } // End IF
        } else
        {
            // Set the Window text colors
            SetTextColor (hDC, gSyntaxColorText.crFore);
            SetBkColor   (hDC, gSyntaxColorText.crBack);
        } // End IF/ELSE
    } // End IF

    // Set the coordinates
    rcScr.top    = y;
    rcScr.left   = x;
    rcScr.right  =
    rcScr.bottom = 0;

    // Save the current font in case we are Output Debugging
    hFontOld =
    hFontCur = GetCurrentObject (hDC, OBJ_FONT);

    // If we're Output Debugging, ...
    if (OptionFlags.bOutputDebug)
    {
        // Zero the temp coordinates
        SetRectEmpty (&rcAct);

        // Start off rcScr.right at the left
        rcScr.right = rcScr.left;

        // Loop through the chars
        for (uCnt = 0; uCnt < uLen; uCnt++)
        {
            // Get the char
            wcCur = lpwsz[uCnt];

            // If the char is a Fallback char and
            //   we're Output Debugging, ...
            if (IsOutDbg (wcCur)
             && OptionFlags.bOutputDebug
             && hFontScrFB NE NULL)
            {
                // If it's a replacement for WC_EOS, ...
                if (wcCur EQ UTF16_REPLACEMENT0000)
                    wcCur = WC_EOS;
                // Shift the value into the Fallback font range
                wcCur += L' ';

                // Select the Fallback font
                SelectObject (hDC, hFontScrFB);

                // Calculate the width & height of the char
                //   in screen coordinates
                DrawTextW (hDC,
                          &wcCur,
                           1,
                          &rcAct,
                           0
                         | DT_CALCRECT
                         | DT_NOPREFIX
                         | DT_NOCLIP);
                // Restore the current font
                SelectObject (hDC, hFontCur);
            } else
                // Calculate the width & height of the char
                //   in screen coordinates
                DrawTextW (hDC,
                          &wcCur,
                           1,
                          &rcAct,
                           0
                         | DT_CALCRECT
                         | DT_NOPREFIX
                         | DT_NOCLIP);
            // Accumulate as the screen coordinates
            rcScr.right += rcAct.right - rcAct.left;
            rcScr.bottom = max (rcScr.bottom, rcAct.bottom);
        } // End FOR
    } else
        // Calculate the width & height of the line
        //   in screen coordinates
        DrawTextW (hDC,
                  &lpwsz[uCol],
                   uLen,
                  &rcScr,
                   0
                 | DT_CALCRECT
                 | DT_NOPREFIX
                 | DT_NOCLIP);
    // Copy the screen coordinates
    //   in case we're not printing, or
    //   if we are, to initialize the struc
    rcAct = rcScr;

    // If we're printing, ...
    if (lFlags & PRF_PRINTCLIENT)
    {
        TEXTMETRICW tmPR;               // TEXTMETRICWs for the Printer Font
        DWORD       dwTmp;

        // Respecify the current font
        hFontCur = hFontScrRG;

        // Use the printer font
        SelectObject (hDC, hFontScrRG);

        // Get the text metrics for this font
        GetTextMetricsW (hDC, &tmPR);

        // Respecify the horizontal & vertical positions in printer coordinates
        rcAct.top  = MulDiv (tmPR.tmHeight      , rcAct.top , line_height);
        rcAct.left = MulDiv (tmPR.tmAveCharWidth, rcAct.left, char_width );

        // Calculate the width & height of the line
        //   in printer coordinates
        dwTmp = LclGetTabbedTextExtentW (hFontScrRG,    // HFONT for regular chars
                                         hFontScrFB,    // ...       Fallback ...
                                         hDC,           // Device context
                                        &lpwsz[uCol],   // Ptr to string
                                         uLen,          // Length of string
                                         0, NULL);      // No tabs
        // Get the actual character width (rounded up)
        rcAct.right = rcAct.left + LOWORD (dwTmp);
        cxAveChar = ((uLen - 1) + (rcAct.right - rcAct.left)) / uLen;
    } else
        // Get the actual character width (rounded up)
        cxAveChar = ((uLen - 1) + (rcScr.right - rcScr.left)) / uLen;

    // Recalculate the average char width of the Fallback font
    cxAveFB = RecalcAveCharWidth (hFontScrFB);

#ifndef UNISCRIBE
    // On some systems when the alternate font isn't the same
    //   width as the SM font, the width calculated by DT_CALCRECT
    //   is too small, so we recalculate it here
    rcScr.right = rcScr.left + cxAveChar * uLen;
    rcAct.right = rcAct.left + cxAveChar * uLen;

    if (lpMemPTD->lpFontLink EQ NULL
     || FAILED (DrawTextFL (lpMemPTD->lpFontLink, hDC, &rcAct, lpwsz, uLen, cxAveChar)))
        OneDrawTextW (hDC, &rcAct, &lpwsz, uLen, cxAveChar);
#else
    {
        UINT     uAlignPrev;                // Previous value for TextAlign
        COLORREF clrBackDef;                // Default background color
        LPWCHAR  lpwCur;                    // Ptr to current character
          WCHAR    wcCur;                   // The current char

        // Get the default background color for this DC
        clrBackDef = GetBkColor (hDC);

        // Set the initial position
        MoveToEx (hDC,
                  rcAct.left,
                  rcAct.top,
                  NULL);
        // Set the TextAlign state
        uAlignPrev = GetTextAlign (hDC);
        uAlignPrev = SetTextAlign (hDC, TA_UPDATECP | uAlignPrev);

        // Get a ptr to the current char
        lpwCur = &lpwsz[uCol];
        wcCur = *lpwCur;

        // Loop through the characters
        while (uLen--)
        {
            // If we're syntax coloring, ...
            if (hGlbClr NE NULL)
            {
                // Set the foreground color
                SetTextColor (hDC, lpMemClrIni[uCol].syntClr.crFore);

                // Set the background color
                if (lpMemClrIni[uCol].syntClr.crBack EQ DEF_SCN_TRANSPARENT)
                    SetBkColor (hDC, clrBackDef);
                else
                    SetBkColor (hDC, lpMemClrIni[uCol].syntClr.crBack);
            } // End IF

            // If the char is a Fallback char and
            //   we're Output Debugging, ...
            if (IsOutDbg (wcCur)
             && OptionFlags.bOutputDebug
             && hFontScrFB NE NULL)
            {
                // If it's a replacement for WC_EOS, ...
                if (wcCur EQ UTF16_REPLACEMENT0000)
                    wcCur = WC_EOS;
                // Shift the value into the Fallback font range
                wcCur += L' ';

                // Put the Fallback font into effect
                SelectObject (hDC, hFontScrFB);

                // Save as the average char width
                cxAveWidth = cxAveFB;
            } else
                // Save as the average char width
                cxAveWidth = cxAveChar;

            // Draw the line for real
            DrawTextW (hDC,
                      &wcCur,
                       1,
                      &rcAct,
                       0
                     | DT_SINGLELINE
                     | DT_NOPREFIX
                     | DT_NOCLIP);
            // Skip to the next char
            lpwCur = CharNextW (lpwCur);
            wcCur = *lpwCur;
            uCol++;
            rcAct.left  += cxAveWidth;
            rcAct.right  = rcAct.left + cxAveWidth;

            // Restore the current font
            SelectObject (hDC, hFontCur);
        } // End WHILE

        // Restore TextAlign state
        SetTextAlign (hDC, uAlignPrev);

        // Restore the current font
        SelectObject (hDC, hFontCur);
    } // End Nothing
#endif
    // If we're printing, ...
    if (lFlags & PRF_PRINTCLIENT)
    {
        SelectObject (hDC, hFontOld);

        DeleteObject (hFontScrRG); hFontScrRG = NULL;
        DeleteObject (hFontScrFB); hFontScrFB = NULL;
    } // End IF

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbClr, lpMemClrIni);

    // Calculate the result
    return MAKELONG (rcScr.right - rcScr.left, rcScr.bottom - rcScr.top);
} // End LclECPaintHook
#undef  APPEND_NAME


#ifndef UNISCRIBE
//***************************************************************************
//  $DrawTextFL
//
//  DrawText with font linking as seen in
//  http://blogs.msdn.com/oldnewthing/archive/2004/07/16/185261.aspx
//***************************************************************************

int DrawTextFL
    (IMLangFontLink *pfl,
     HDC             hdc,
     LPRECT          lprc,
     LPCWSTR         lpsz,
     long            cch,
     long            cxAveChar)

{
    HRESULT hr;
    HFONT   hfOrig = (HFONT) GetCurrentObject (hdc, OBJ_FONT);
    DWORD   dwFontCodePages = 0;

    hr = IMLangFontLink_GetFontCodePages (pfl, hdc, hfOrig, &dwFontCodePages);
    if (SUCCEEDED (hr))
    {
        while (cch > 0)
        if (FAILED (hr = DrawTextFLsub (pfl, hdc, dwFontCodePages, hfOrig, lprc, cxAveChar, &lpsz, &cch)))
            break;

        if (FAILED (hr))
        {
            //  We started outputting characters so we have to finish.
            //  Do the rest without font linking since we have no choice.
            OneDrawTextW (hdc, lprc, &lpsz, cch, cxAveChar);

            hr = S_FALSE;
        } // End IF
    } // End IF

    return hr;
} // End DrawTextFL
#endif


#ifndef UNISCRIBE
//***************************************************************************
//  $DrawTextFLsub
//
//  Subroutine to DrawTextFL
//***************************************************************************

HRESULT DrawTextFLsub
    (IMLangFontLink *pfl,
     HDC             hdc,
     DWORD           dwFontCodePages,
     HFONT           hfOrig,
     LPRECT          lprc,
     long            cxAveChar,
     LPCWSTR        *lplpsz,
     int            *lpcch)

{
    HRESULT hr;
    long    cchActual;
    HFONT   hfLinked;
    DWORD   dwActualCodePages;
    int     cchCnt,
            cch;

    hr = IMLangFontLink_GetStrCodePages (pfl, *lplpsz, *lpcch, dwFontCodePages, &dwActualCodePages, &cchActual);
    if (FAILED (hr))
        return hr;

    if (dwActualCodePages & dwFontCodePages)
        OneDrawTextW (hdc, lprc, lplpsz, cchActual, cxAveChar);
    else
    if (FAILED (hr = IMLangFontLink_MapFont (pfl, hdc, dwActualCodePages, 0, &hfLinked)))
    {
        // If the actual code pages are non-zero and cchActual > 1,
        //   run through the chars one-by-one
        if (dwActualCodePages && cchActual > 1)
        {
            for (cchCnt = 0; cchCnt < cchActual; cchCnt++, (*lpcch)--)
            {
                cch = 1;

                if (FAILED (hr = DrawTextFLsub (pfl, hdc, dwFontCodePages, hfOrig, lprc, cxAveChar, lplpsz, &cch)))
                    return hr;
            } // End FOR

            return S_OK;
        } else
        // If the char is {infinity} or A-Z underbar, ...
        if (**lplpsz EQ UTF16_INFINITY
         || (UTF16_A_ <= **lplpsz
          &&             **lplpsz <= UTF16_Z_))
            // Use the alternate font
            SelectObject (hdc, hFontAlt);

        // Output the char
        OneDrawTextW (hdc, lprc, lplpsz, cchActual, cxAveChar);

        // Restore the original font (even if we didn't change it out)
        SelectObject (hdc, hfOrig);

        // Set the return code to OK even though we failed to find a linked font
        hr = S_OK;
    } else
    {
        // Use the linked font
        SelectObject (hdc, hfLinked);
        OneDrawTextW (hdc, lprc, lplpsz, cchActual, cxAveChar);
        SelectObject (hdc, hfOrig);

        // Release the linked font and mappings
        IMLangFontLink_ReleaseFont (pfl, hfLinked);
        IMLangFontLink_ResetFontMapping (pfl);
    } // End IF/ELSE

    // Update the string count
    (*lpcch) -= cchActual;

    return hr;
} // End DrawTextFLsub
#endif


#ifndef UNISCRIBE
//***************************************************************************
//  $OneDrawTextW
//
//  Output one chunk all with the same font
//***************************************************************************

void OneDrawTextW
    (HDC      hdc,
     LPRECT   lprc,
     LPCWSTR *lplpsz,
     long     cchActual,
     long     cxAveChar)

{
    int  cch;

    for (cch = 0; cch < cchActual; cch++)
    {
        DrawTextW (hdc,
                  (*lplpsz)++,
                   1,
                   lprc,
                   0
                 | DT_SINGLELINE
                 | DT_NOPREFIX
                 | DT_NOCLIP);
        // Update the horizontal position
        lprc->left += cxAveChar;
    } // End FOR
} // End OneDrawTextW
#endif


//***************************************************************************
//  $GetIDMPOS_SM
//
//  Translate menu enums into positions for SM
//***************************************************************************

UINT GetIDMPOS_SM
    (ALLMENUPOS menuEnum)

{
    // Split cases based upon the menu enum
    switch (menuEnum)
    {
        case IDMPOSNAME_FILE:
            return IDMPOS_SM_FILE;

        case IDMPOSNAME_EDIT:
            return IDMPOS_SM_EDIT;

        case IDMPOSNAME_VIEW:
            return IDMPOS_SM_VIEW;

        case IDMPOSNAME_WINDOW:
            return IDMPOS_SM_WINDOW;

        case IDMPOSNAME_HELP:
            return IDMPOS_SM_HELP;

        defstop
            return -1;
    } // End SWITCH
} // End GetIDMPOS_SM


//***************************************************************************
//  $GetIDMPOS_FE
//
//  Translate menu enums into positions for FE
//***************************************************************************

UINT GetIDMPOS_FE
    (ALLMENUPOS menuEnum)

{
    // Split cases based upon the menu enum
    switch (menuEnum)
    {
        case IDMPOSNAME_FILE:
            return IDMPOS_FE_FILE;

        case IDMPOSNAME_EDIT:
            return IDMPOS_FE_EDIT;

        case IDMPOSNAME_VIEW:
            return IDMPOS_FE_VIEW;

        case IDMPOSNAME_WINDOW:
            return IDMPOS_FE_WINDOW;

        case IDMPOSNAME_OBJECTS:
            return IDMPOS_FE_OBJECTS;

        case IDMPOSNAME_HELP:
            return IDMPOS_FE_HELP;

        defstop
            return -1;
    } // End SWITCH
} // End GetIDMPOS_FE


//***************************************************************************
//  $IzitNameChar
//
//  Return TRUE iff the given APLCHAR is valid in a variable/function/operator name
//***************************************************************************

UBOOL IzitNameChar
    (APLCHAR aplChar)

{
    return IsValid2ndCharInName (aplChar)
        || aplChar EQ UTF16_ALPHA           // Only as one and only char
        || aplChar EQ UTF16_OMEGA           // ...
        || IsSysName (&aplChar);            // Valid as 1st char only
} // End IzitNameChar


//***************************************************************************
//  $GetVkState
//
//  Return the current vkState
//***************************************************************************

VKSTATE GetVkState
    (HWND hWndEC)                           // Edit Ctrl window handle

{
    long lvkState;                          // Temporary var for vkState

    lvkState = GetWindowLongW (hWndEC, GWLEC_VKSTATE);
    return *(LPVKSTATE) &lvkState;
} // End GetVkState


//***************************************************************************
//  $LclEditCtrlWndProc
//
//  Local window procedure for the Edit Ctrl (superclass)
//***************************************************************************

LRESULT WINAPI LclEditCtrlWndProc
    (HWND   hWnd,                   // Window handle
     UINT   message,                // Type of message
     WPARAM wParam,                 // Additional information
     LPARAM lParam)                 // ...

{

    VKSTATE      vkState;                   // Virtual key state (Shift, Alt, Ctrl)
    HWND         hWndParent;                // Handle of parent (SM/FE) window
    LRESULT      lResult;                   // Result from calling original handler
    LPUNDO_BUF   lpUndoNxt,                 // Ptr to next available slot in the Undo Buffer
                 lpUndoBeg;                 // ...    first          ...
    UINT         uCharPosBeg,               // Pos of the beginning char
                 uCharPosEnd,               // ...        ending    ...
                 uCharPos,                  // ...    a character position
                 uLinePos,                  // Char position of start of line
                 uLineNum,                  // Line #
                 uSpaces,                   // # spaces to insert
                 uTmp,                      // Temporary var
                 uGroupIndex;               // Group index
    WCHAR        wChar[DEF_TABS + 1],       // Array of blanks for converting tabs to spaces
                 uChar;                     // Loop counter
    WCHAR        wchCode[2] = {L'\0'};      // Character code
    UINT         uState;                    // Keyboard state (Shift- Ctrl- Alt-keys)
    UBOOL        bSelection,                // TRUE iff there's a selection
                 bDrawLineNums = FALSE;     // TRUE iff the ending code should draw the
                                            //   line #s after calling the original handler
    HANDLE       hGlbClip;                  // Handle to the clipboard
    LPWCHAR      lpMemClip;                 // Memory ptr
    UBOOL        ksShft,                    // TRUE iff VK_CONTROL is pressed (either Ctrl- key)
                 ksCtrl,                    // ...      VK_SHIFT   ...                Shift-...
                 ksMenu;                    // ...      VK_MENU    ...                Alt-  ...
    LPPERTABDATA lpMemPTD;                  // Ptr to PerTabData global memory
    LPWCHAR      lpwszFormat;               // Ptr to formatting save area
    static WNDPROC lpfnOldEditCtrlWndProc = // Ptr to preceding Edit Ctrl window procedure
                &EditWndProcW;
    LPWCHAR      lpwszTemp;                 // Ptr to temporary storage
    static UINT  uAltNum = 0;               // Accumulator for Alt-nnn (NumPad only)

    // If the thread is MF, ...
    if (TLSTYPE_MF EQ TlsGetValue (dwTlsType))
    {
        // Mark as from MF
        lpMemPTD = NULL;
        lpwszTemp = NULL;
////////LCLODSAPI ("EC: ", hWnd, message, wParam, lParam);
    } else
    {
        // Get ptr to PerTabData global memory
        lpMemPTD = GetMemPTD ();

        // Get ptr to temporary storage
        lpwszTemp = lpMemPTD->lpwszTemp;
    } // End IF/ELSE

////LCLODSAPI ("EC: ", hWnd, message, wParam, lParam);
    // Split cases
    switch (message)
    {
        case WM_NCCREATE:               // 0 = (int) wParam
                                        // lpcs = (LPCREATESTRUCTW) lParam
            // Initialize the key state
            ZeroMemory (&vkState, sizeof (vkState));
            vkState.Ins = OptionFlags.bInsState;

            // If our parent is not MF, ...
            if (lpMemPTD NE NULL)
                // Tell the Status Window about this
                SetStatusIns (vkState.Ins);

            // Save in window extra bytes
            SetWindowLongW (hWnd, GWLEC_VKSTATE, *(long *) &vkState);

            break;

        case WM_CREATE:
            // In order to avoid displaying a spurious Context Menu, set the
            //   double-click tick count window property to the current tick count

            // In case we get a WM_RBUTTONUP message at startup because the WM_RBUTTONUP
            //   message from the tail of a right double-click is sent to this window
            //   instead of the Edit Ctrl window which initiated the right double-click
            //   due to window overlap, we need a way to recognize it and discard it so
            //   W doesn't think to generate another WM_CONTEXTMENU message.
            SetPropW (hWnd, L"DBLCLK.TICKCOUNT", ULongToHandle (GetTickCount ()));

            PERFMON

            break;

        case WM_GETDLGCODE:
            // Get the code from the Edit Ctrl
            lResult = CallWindowProcW (lpfnOldEditCtrlWndProc,
                                       hWnd,
                                       message,
                                       wParam,
                                       lParam);     // Pass on down the line
            // Include the keys we need to process APL symbols
            return lResult | DLGC_WANTALLKEYS | DLGC_WANTMESSAGE;

        case WM_SETCURSOR:          // hwnd = (HWND) wParam;       // handle of window with cursor
                                    // nHittest = LOWORD(lParam);  // hit-test code
                                    // wMouseMsg = HIWORD(lParam); // mouse-message identifier
            // If not from MF, and
            //    this is an Edit Ctrl for SM, and
            //    we set the cursor, ...
            if (lpMemPTD NE NULL
             && IzitSM (GetParent (hWnd))
             && LclSetCursor (hWnd,
                              lpMemPTD,
                              LOWORD (lParam)))
                return TRUE;                // We handled the msg

            break;

        // In the case of right double click, the sequence of messages is
        //   WM_RBUTTONDOWN
        //   WM_RBUTTONUP
        //   WM_CONTEXTMENU
        //   WM_RBUTTONDBLCLK
        //   WM_RBUTTONUP
        //
        // If the user double clicked, we set a timer in the WM_CONTEXTMENU
        //   message handler and wait for it to expire (real WM_CONTEXTMENU)
        //   or WM_RBUTTONDBLCLK occurs in which it's a double click.  Note
        //   the last WM_RBUTTONUP.  If we pass this message on, W will
        //   send another WM_CONTEXTMENU.  To avoid this, we save the current
        //   tick count at the end of WM_RBUTTONDBLCLK.  If WM_RBUTTONUP
        //   occurs too soon, we ignore that message.

        case WM_CONTEXTMENU:                // hwnd = (HWND) wParam;
                                            // xPos = LOSHORT (lParam); // horizontal position of cursor in Screen coords
                                            // yPos = HISHORT (lParam); // vertical   ...
            // Set a timer waiting for WM_RBUTTONDBLCLK or expiration
            SetTimer (hWnd, ID_TIMER_EDIT, GetDoubleClickTime () / 2, NULL);

            // Save the context menu's wParam & lParam so we
            //   can retrieve them in WM_TIMER.  The values
            //   in lParam are in screen coordinates and are
            //   translated for multiple monitor support which
            //   saves me the trouble.
            SetPropW (hWnd, L"TIMER.WPARAM", (LPVOID) wParam);
            SetPropW (hWnd, L"TIMER.LPARAM", (LPVOID) lParam);

            return FALSE;

        case WM_TIMER:              // wTimerID = wParam            // Timer identifier
                                    // tmpc = (TIMERPROC *) lParam  // Ptr to timer callback
            if (wParam EQ ID_TIMER_EDIT)
            {
                // The timer expired, so it's a real WM_CONTEXTMENU
                KillTimer (hWnd, ID_TIMER_EDIT);

                return CallWindowProcW (lpfnOldEditCtrlWndProc,
                                        hWnd,
                                        WM_CONTEXTMENU,
                                        (WPARAM) GetPropW (hWnd, L"TIMER.WPARAM"),
                                        (LPARAM) GetPropW (hWnd, L"TIMER.LPARAM"));
            } // End IF

            break;

        case WM_LBUTTONDBLCLK:      // fwKeys = wParam;         // key flags
                                    // xPos = LOSHORT(lParam);  // horizontal position of cursor in CA coords
                                    // yPos = HISHORT(lParam);  // vertical   ...
            // If we're reversing left- and right-double-click, ...
            if (OptionFlags.bRevDblClk)
            {
                POINT pt;

                // Save this message's wParam & lParam so we
                //   can retrieve them in MYWM_RBUTTONDBLCLK.  The values
                //   in lParam are in client coordinates and must be
                //   translated to screen coords to be compatible with
                //   lParam from WM_CONTEXTMENU.
                pt.x = LOSHORT (lParam);
                pt.y = HISHORT (lParam);
                ClientToScreen (hWnd, &pt);
                SetPropW (hWnd, L"TIMER.WPARAM", (LPVOID) wParam);
                SetPropW (hWnd, L"TIMER.LPARAM", (LPVOID) MAKELPARAM (pt.x, pt.y));

                return SendMessageW (hWnd, MYWM_RBUTTONDBLCLK, wParam, lParam);
            } else
                break;

        case WM_RBUTTONDBLCLK:      // fwKeys = wParam;         // key flags
                                    // xPos = LOSHORT(lParam);  // horizontal position of cursor
                                    // yPos = HISHORT(lParam);  // vertical position of cursor
            // It's a right double click, so cancel the timer
            KillTimer (hWnd, ID_TIMER_EDIT);

            // If we're reversing left- and right-double-click, ...
            if (OptionFlags.bRevDblClk)
            {
                // Pass on to the Edit Ctrl as a left-button down, double-click, up
                CallWindowProcW (lpfnOldEditCtrlWndProc,
                                 hWnd,
                                 WM_LBUTTONDOWN,
                                 wParam,
                                 lParam);     // Pass on down the line
                CallWindowProcW (lpfnOldEditCtrlWndProc,
                                 hWnd,
                                 WM_LBUTTONDBLCLK,
                                 wParam,
                                 lParam);     // Pass on down the line
                CallWindowProcW (lpfnOldEditCtrlWndProc,
                                 hWnd,
                                 WM_LBUTTONUP,
                                 wParam,
                                 lParam);     // Pass on down the line
                // In case we get a WM_RBUTTONUP message shortly after this one,
                //   we need a way to recognize it and discard it so W doesn't
                //   think to generate another WM_CONTEXTMENU message.
                SetPropW (hWnd, L"DBLCLK.TICKCOUNT", ULongToHandle (GetTickCount ()));

                return FALSE;
            } // End IF

            // Fall through to local right-double-click

        case MYWM_RBUTTONDBLCLK:    // fwKeys = wParam;         // key flags
                                    // xPos = LOSHORT(lParam);  // horizontal position of cursor
                                    // yPos = HISHORT(lParam);  // vertical position of cursor
        {
            UINT       xPos, yPos;      // x- and y-screen coordinates
            LPSYMENTRY lpSymEntry;      // Ptr to the SYMENTRY under the name

            // If from MF, pass on this message
            if (lpMemPTD EQ NULL)
                break;

            // Handle a right double click

            // Get the lParam from the original WM_CONTEXTMENU/WM_LBUTTONDBLCLK message
            yPos = HandleToULong (GetPropW (hWnd, L"TIMER.LPARAM"));

            xPos = (short) LOWORD (yPos);
            yPos = (short) HIWORD (yPos);

            // If there's an identifier underneath this double click,
            //   attempt to edit it as a function/variable
            lpSymEntry = (LPSYMENTRY) SendMessageW (hWnd, MYWM_IZITNAME, xPos, yPos);
            if (lpSymEntry NE NULL
             && (lpSymEntry->stFlags.ObjName EQ OBJNAME_NONE
              || lpSymEntry->stFlags.ObjName EQ OBJNAME_USR))
            {
                LPAPLCHAR lpMemName;

                // If the name is that of a user defined function/operator
                //   or it is undefined, edit it as a function
                switch (lpSymEntry->stFlags.stNameType)
                {
                    case NAMETYPE_UNK:
                    case NAMETYPE_FN0:
                    case NAMETYPE_FN12:
                    case NAMETYPE_OP1:
                    case NAMETYPE_OP2:
                    case NAMETYPE_OP3:
                    case NAMETYPE_TRN:
                        // Lock the memory to get a ptr to it
                        lpMemName = MyGlobalLockWsz (lpSymEntry->stHshEntry->htGlbName);

                        // Get ptr to formatting save area
                        lpwszFormat = lpMemPTD->lpwszFormat;

                        // Copy the name (and its trailing zero) to temporary storage
                        //   which won't go away when we unlock the name's global
                        //   memory handle
                        strcpyW (lpwszFormat, lpMemName);

                        // We no longer need this ptr
                        MyGlobalUnlock (lpSymEntry->stHshEntry->htGlbName); lpMemName = NULL;

                        // Open a Function Editor window
                        CreateFcnWindow (lpwszFormat);

                        break;

                    case NAMETYPE_LST:      // Ignore this name type
                    case NAMETYPE_VAR:      // ...
                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End IF

            // In case we get a WM_RBUTTONUP message shortly after this one,
            //   we need a way to recognize it and discard it so W doesn't
            //   think to generate another WM_CONTEXTMENU message.
            SetPropW (hWnd, L"DBLCLK.TICKCOUNT", ULongToHandle (GetTickCount ()));

            break;
        } // End WM_RBUTTONDBLCLK

        case WM_RBUTTONUP:          // fwKeys = wParam;         // key flags
                                    // xPos = LOSHORT(lParam);  // horizontal position of cursor
                                    // yPos = HISHORT(lParam);  // vertical position of cursor
            // If from MF, pass on this message
            if (lpMemPTD EQ NULL)
                break;

            // If this message occurs closely on the heels of a WM_RBUTTONDBLCLK,
            //  discard it so W doesn't generate another WM_CONTEXTMENU message.
            if (GetDoubleClickTime () > (GetTickCount () - HandleToULong (GetPropW (hWnd, L"DBLCLK.TICKCOUNT"))))
                return FALSE;           // Discard this message
            break;

        case MYWM_IZITNAME:         // xPos = (UINT) wParam
                                    // yPos = (UINT) lParam
        {
            POINT pt;
            LONG  lCharPos,
                  lLinePos,
                  lLineLen;
            VARS_TEMP_OPEN

            // This message is sent whenever the user right clicks
            //   inside a Function Edit window and the Edit Ctrl
            //   wants to know whether or not to enable the
            //   Localize/Unlocalize menu items, or it occurs
            //   inside a Session Manager window and the user wants
            //   to edit the named object.

            // Translate the xPos & yPos mouse screen coordinates into
            //   client coordinates in the Edit Ctrl window
            pt.x = (LONG) wParam;
            pt.y = (LONG) lParam;
            ScreenToClient (hWnd, &pt);

            // Translate the xPos & yPos client coordinates into
            //   char positions in the text
            CharFromPos (hWnd, SendMessageW (hWnd, EM_CHARFROMPOS, 0, MAKELPARAM (pt.x, pt.y)), &uLineNum, &lCharPos);

            // Get the length of the line with this char
            lLineLen = (LONG) SendMessageW (hWnd, EM_LINELENGTH, lCharPos, 0);

            // Get the index of the first character in the line
            lLinePos = (LONG) SendMessageW (hWnd, EM_LINEINDEX, uLineNum, 0);

            // Convert lCharPos from origin-0 to origin-Line
            lCharPos -= lLinePos;

            // Tell EM_GETLINE maximum # chars in the buffer
            // The output array is a temporary so we don't have to
            //   worry about overwriting outside the allocated buffer
            CHECK_TEMP_OPEN
            ((LPWORD) lpwszTemp)[0] = (WORD) lLineLen;

            // Get the contents of the line
            SendMessageW (hWnd, EM_GETLINE, uLineNum, (LPARAM) lpwszTemp);

            // Ensure the name is properly terminated
            lpwszTemp[lLineLen] = WC_EOS;

            // Check the chars at and to the right of the specified char pos
            for (uCharPosEnd = lCharPos;
                IzitNameChar (lpwszTemp[uCharPosEnd]);
                uCharPosEnd++)
                ;

            // Ensure the name is properly terminated
            lpwszTemp[uCharPosEnd] = WC_EOS;

            // Check the chars to the left of the specified char pos
            for (uCharPosBeg = lCharPos;
                 uCharPosBeg && IzitNameChar (lpwszTemp[uCharPosBeg - 1]);
                 uCharPosBeg--)
                ;

            // The name spans [uCharPosBeg, uCharPosEnd)
            // Check the whole name now
            if (IsValidName (&lpwszTemp[uCharPosBeg], (UINT) (uCharPosEnd - uCharPosBeg))
             && !IsSysName (&lpwszTemp[uCharPosBeg]))
            {
                STFLAGS    stFlags = {0};       // Symbol Table Flags used to limit the lookup
                LPSYMENTRY lpSymEntry;          // Ptr to SYMENTRY if found

                // Lookup the name in the symbol table
                lpSymEntry = SymTabLookupNameLength (&lpwszTemp[uCharPosBeg],
                                                      (UINT) (uCharPosEnd - uCharPosBeg),
                                                     &stFlags);
                if (lpSymEntry NE NULL)
                    lResult = (LRESULT) lpSymEntry;
                else
                    // Name not found -- append it
                    // Lookup in or append to the symbol table
                    lResult = (LRESULT) SymTabAppendName_EM (&lpwszTemp[uCharPosBeg], NULL);
            } else
                lResult = 0;

            EXIT_TEMP_OPEN

            return lResult;
        } // End MYWM_IZITNAME

#define fwSizeType  wParam
#define nWidth      (LOWORD (lParam))
#define nHeight     (HIWORD (lParam))
        case WM_SIZE:               // fwSizeType = wParam;      // Resizing flag
                                    // nWidth = LOWORD(lParam);  // Width of client area
                                    // nHeight = HIWORD(lParam); // Height of client area
            // Repaint the client area
            InvalidateRect (hWnd, NULL, TRUE);

            // If from MF, pass on this message
            if (lpMemPTD EQ NULL)
                break;

            if (IzitSM (GetParent (hWnd))   // If Edit Ctrl for SM,
             && nWidth NE 0)                // and width is non-zero
                // Change []PW to track the new width
                RespecifyNewQuadPW (hWnd, nWidth);

            PERFMON
////////////PERFMONSHOW (NULL)

            break;
#undef  nHeight
#undef  nWidth
#undef  fwSizeType

        case WM_KEYDOWN:            // nVirtKey = (int) wParam;         // Virtual-key code
        {                           // lKeyData = *(LPKEYDATA) &lParam; // Key data
#ifdef DEBUG
            int     nVirtKey    = (int) wParam;
            KEYDATA keyData     = *(LPKEYDATA) &lParam;
#else
  #define nVirtKey      ((int) wParam)
  #define keyData       (*(LPKEYDATA) &lParam)
#endif
            // Get the key states
            ksShft  = (GetKeyState (VK_SHIFT)   & BIT15) ? TRUE : FALSE;
            ksCtrl  = (GetKeyState (VK_CONTROL) & BIT15) ? TRUE : FALSE;
            ksMenu  = (GetKeyState (VK_MENU )   & BIT15) ? TRUE : FALSE;

            // If the virtual key is alphabetic,
            //   and the Alt key is up,
            //   and the Ctrl key is up,
            //   and the CapsLock key is set, ...
            if ('A' <= nVirtKey
             &&        nVirtKey <= 'Z'
             && !ksMenu
             && !ksCtrl
             && (GetKeyState (VK_CAPITAL) & BIT0))
                // Toggle the shift state
                ksShft ^= TRUE;

            // Handle Numlock key
            if (GetKeyState (VK_NUMLOCK) & BIT0)
            {
                if (VK_NUMPAD0 <= nVirtKey
                 &&               nVirtKey <= VK_NUMPAD9)
                {
                    // Convert to origin-0
                    uTmp = nVirtKey - VK_NUMPAD0;

                    // Convert to scancode for '1' to '9', '0'
                    keyData.scanCode = (uTmp EQ 0) ? SCANCODE_0 : (SCANCODE_1 + uTmp - 1);

                    // Convert to virtkey  for '0' to '9'
                    nVirtKey += '0' - VK_NUMPAD0;
                } else
                if (nVirtKey EQ VK_DECIMAL)
                {
                    keyData.scanCode = SCANCODE_DECIMAL;
                    nVirtKey         = '.';
                } // End IF/ELSE
            } // End IF

#ifdef DEBUG_WM_KEYDOWN
            dprintfWL0 (L"WM_KEYDOWN:     nVirtKey = %02X(%c), ScanCode = %02X, SCM = %u%u%u",
                        nVirtKey,
                        nVirtKey,
                        keyData.scanCode,
                        ksShft, ksCtrl, ksMenu);
#endif
            // Get the handle of the parent window
            hWndParent = GetParent (hWnd);

            // If our parent is not MF, ...
            if (lpMemPTD NE NULL)
                // Save the key in the parent window's extra bytes
                SetWindowLongW (hWndParent, GWLSF_LASTKEY, nVirtKey);

            // This message handles special keys that don't
            //   produce a WM_CHAR, i.e. non-printable keys,
            //   Backspace, and Delete.

            // If the scanCode is within range of our table, ...
            if (keyData.scanCode < aKeybLayoutAct.uCharCodesLen)
            {
                // Set the keyb state
                uState = (ksShft << 2) | (ksCtrl << 1) | (ksMenu << 0);

                // Get the corresponding character
                wchCode[0] = aKeybLayoutAct.aCharCodes[keyData.scanCode].wc[uState];

                // Check for valid character
                if (wchCode[0])
                {
                    // If this control allows numbers only, ...
                    if (ES_NUMBER & GetWindowLongW (hWnd, GWL_STYLE))
                    {
                        if (L'0' > wchCode[0]
                         ||        wchCode[0] > L'9')
                            break;
                    } // End IF

                    // Insert/replace the char string
                    InsRepCharStr (hWnd, wchCode, lpMemPTD EQ NULL);

                    return FALSE;       // We handled the msg
                } // End IF
            } // End IF

            // If we're inside an AFE,
            //   and it's a key to ignore, ...
            // Note we test for the key first so as to avoid spurious beeps
            //   if we're in an AFE but the keystroke is allowed
            if (( nVirtKey EQ VK_DELETE
              ||  nVirtKey EQ VK_RETURN
              ||  nVirtKey EQ VK_TAB
              ||  nVirtKey EQ VK_INSERT
                )
             && IzitAFE (hWnd, FALSE))
                return FALSE;       // We handled the msg

            // Process the virtual key
            switch (nVirtKey)
            {
#ifdef DEBUG
                case VK_F1:
                case VK_F2:
                case VK_F3:
                case VK_F4:
                case VK_F5:
                case VK_F6:
                case VK_F7:
////////////////case VK_F8:
                case VK_F9:
                case VK_F10:
                case VK_F11:
                case VK_F12:
                    // If our parent is not MF, ...
                    if (lpMemPTD NE NULL)
                        PostMessageW (lpMemPTD->hWndSM, MYWM_KEYDOWN, wParam, lParam);

                    return FALSE;

                case VK_F8:             // Display the Undo Buffer
                    // If our parent is not MF, ...
                    if (lpMemPTD NE NULL)
                        DisplayUndo (hWnd); // Display the Undo Buffer

                    return FALSE;
#endif
                case VK_UP:
                case VK_DOWN:
                    // If our parent is MF, ...
                    if (lpMemPTD EQ NULL)
                        break;

                    if (IzitSM (hWndParent))
                    {
                        // Get the current line #
                        uLineNum = (DWORD) SendMessageW (hWnd, EM_LINEFROMCHAR, (WPARAM) -1, 0);

                        PostMessageW (hWndParent, MYWM_KEYDOWN, wParam, uLineNum);
                    } // End IF

                    break;

                case VK_CANCEL:
                {
                    KEYDATA lKeyData = *(LPKEYDATA) &lParam;

                    // Save the VK_CANCEL scan code in memory
                    //   for later use to distinuish from Ctrl-C
                    //   so it doesn't also generate a WM_COPY
                    uCancelCode = lKeyData.scanCode;

                    // If our parent is not MF, ...
                    if (lpMemPTD NE NULL)
                        PostMessageW (hWndParent, MYWM_KEYDOWN, VK_CANCEL, 0);
                    return FALSE;       // We handled the msg
                } // End VK_CANCEL

                case VK_INSERT:         // Insert
                    // Ins      toggles the key state
                    // Shft-Ins WM_PASTEs
                    // Ctrl-Ins WM_COPYs

                    // If either VK_SHIFT or VK_CONTROL is pressed,
                    //   ignore this keystroke
                    if (ksCtrl || ksShft)
                        break;

                    // Get the current vkState
                    vkState = GetVkState (hWnd);

                    // Toggle the insert state
                    vkState.Ins = !vkState.Ins;

                    // If our parent is not MF, ...
                    if (lpMemPTD NE NULL)
                        // Tell the Status Window about it
                        SetStatusIns (vkState.Ins);

                    // Save in window extra bytes
                    SetWindowLongW (hWnd, GWLEC_VKSTATE, *(long *) &vkState);

                    // If our parent is MF, ...
                    if (lpMemPTD EQ NULL)
                        break;

                    // Get the char position of the caret
                    uCharPos = GetCurCharPos (hWnd);

                    // Save Undo entry
                    AppendUndo (hWndParent,                 // SM/FE Window handle
                                GWLSF_UNDO_NXT,             // Offset in hWnd extra bytes of lpUndoNxt
                                undoInsToggle,              // Action
                                uCharPos,                   // Beginning char position
                                uCharPos + 1,               // Ending    ...
                                UNDO_NOGROUP,               // Group index
                                0);                         // Character
                    break;

                case VK_RETURN:
                {
                    RECT rcFmtEC,       // Formatting rectangle for the Edit Ctrl
                         rcPaint;

                    // If our parent is MF, ...
                    if (lpMemPTD EQ NULL)
                        break;

                    // If our parent is SM,
                    // pass on to caller and return
                    if (IzitSM (hWndParent))
                    {
                        // Get the current line #
                        uLineNum = (DWORD) SendMessageW (hWnd, EM_LINEFROMCHAR, (WPARAM) -1, 0);

                        PostMessageW (hWndParent, MYWM_KEYDOWN, wParam, uLineNum);

                        return FALSE;
                    } // End IF

                    // Get the char position of the caret
                    uCharPos = GetCurCharPos (hWnd);

                    // Undo deletes it
                    AppendUndo (hWndParent,                 // FE Window handle
                                GWLSF_UNDO_NXT,             // Offset in hWnd extra bytes of lpUndoNxt
                                undoDel,                    // Action
                                uCharPos,                   // Beginning char position
                                uCharPos + 2 + ksShft,      // Ending    ...
                                UNDO_NOGROUP,               // Group index
                                0);                         // Character
                    // Get the formatting rectangle
                    SendMessageW (hWnd, EM_GETRECT, 0, (LPARAM) &rcFmtEC);

                    // Get the client rectangle
                    GetClientRect (hWnd, &rcPaint);

                    // Remove the line #s column
                    rcPaint.left = rcFmtEC.left;

                    // Get the line # of this char
                    uLineNum = (DWORD) SendMessageW (hWnd, EM_LINEFROMCHAR, uCharPos, 0);

                    // Less the # of the topmost visible line
                    uLineNum -= (DWORD) SendMessageW (hWnd, EM_GETFIRSTVISIBLELINE, 0, 0);

                    rcPaint.left = 0;       // Include the left margins
                    rcPaint.top = (DWORD) uLineNum * GetFSIndAveCharSize (FONTENUM_FE)->cy;
                    rcPaint.bottom = rcPaint.top + GetFSIndAveCharSize (FONTENUM_FE)->cy;

                    // Invalidate this line
                    InvalidateRect (hWnd, &rcPaint, FALSE);

                    break;
                } // End VK_RETURN

                case VK_TAB:
                    // Insert a tab -- convert into insert N spaces

                    // If our parent is MF, ...
                    if (lpMemPTD EQ NULL)
                        break;

                    // Get the indices of the selected text (if any)
                    SendMessageW (hWnd, EM_GETSEL, (WPARAM) &uCharPosBeg, (LPARAM) &uCharPosEnd);

                    // Get the line # of this char
                    uLineNum = (DWORD) SendMessageW (hWnd, EM_LINEFROMCHAR, uCharPosBeg, 0);

                    // Get the char position of the start of the current line
                    uLinePos = (DWORD) SendMessageW (hWnd, EM_LINEINDEX, uLineNum, 0);

                    // Get the # spaces to insert
                    uCharPos = uCharPosBeg - uLinePos;
                    uSpaces = ((uCharPos / DEF_TABS) * DEF_TABS + DEF_TABS) - uCharPos;

                    Assert (uSpaces <= DEF_TABS);

                    for (uChar = 0; uChar < uSpaces; uChar++)
                        wChar[uChar] = L' ';

                    wChar[uChar] = WC_EOS;

                    // Insert/replace the char string
                    InsRepCharStr (hWnd, wChar, lpMemPTD EQ NULL);

                    return FALSE;       // We handled the msg
            } // End SWITCH

            // We need to pass this message on to the next handler
            //   so WM_CHAR & WM_SYSCHAR can process it.
            break;
#ifndef DEBUG
  #undef  keyData
  #undef  nVirtKey
#endif
        } // End WM_KEYDOWN

        case WM_KEYUP:              // nVirtKey = (int) wParam;         // Virtual-key code
        {                           // lKeyData = *(LPKEYDATA) &lParam; // Key data
#ifdef DEBUG
            int     nVirtKey    = (int) wParam;
            KEYDATA keyData     = *(LPKEYDATA) &lParam;
#else
  #define nVirtKey      ((int) wParam)
  #define keyData       (*(LPKEYDATA) &lParam)
#endif
            keyData = keyData;          // Respecify to avoid compiler error (unreferenced var)

#ifdef DEBUG_WM_KEYUP
            dprintfWL0 (L"WM_KEYUP:       nVirtKey = %02X(%c), ScanCode = %02X, SCMA = %u%u%u%u", nVirtKey, nVirtKey, keyData.scanCode);
#endif
            // Process the virtual key
            switch (nVirtKey)
            {
                case VK_MENU:
                    if (uAltNum)
                    {
                        // Insert/replace the corresponding Unicode char
                        InsRepCharStr (hWnd, (LPWCHAR) &uAltNum, lpMemPTD EQ NULL);

                        // Clear the number
                        uAltNum = 0;

                        return FALSE;           // We handled the msg
                    } // End IF

                    break;

                case VK_CAPITAL:
                    // Tell the Status Window about it
                    SetStatusCaps (GetKeyState (VK_CAPITAL) & BIT0);

                    break;

                case VK_NUMLOCK:
                    // Tell the Status Window about it
                    SetStatusNum (GetKeyState (VK_NUMLOCK) & BIT0);

                    break;

                case VK_UP:
                case VK_DOWN:
                case VK_LEFT:
                case VK_RIGHT:
                case VK_PRIOR:
                case VK_NEXT:
                case VK_HOME:
                case VK_END:
                case VK_BACK:
                case VK_TAB:
                case VK_RETURN:
                    // Tell the Status Window about the new positions
                    SetStatusPos (hWnd);

                    break;
            } // End SWITCH

            break;
#ifndef DEBUG
  #undef  keyData
  #undef  nVirtKey
#endif
        } // End WM_KEYUP

        case WM_SYSKEYDOWN:         // nVirtKey = (int) wParam;         // Virtual-key code
        {                           // lKeyData = *(LPKEYDATA) &lParam; // Key data
#ifdef DEBUG
            int     nVirtKey    = (int) wParam;
            KEYDATA keyData     = *(LPKEYDATA) &lParam;
#else
  #define nVirtKey      ((int) wParam)
  #define keyData       (*(LPKEYDATA) &lParam)
#endif
            // Get the key states
            ksShft  = (GetKeyState (VK_SHIFT)   & BIT15) ? TRUE : FALSE;
            ksCtrl  = (GetKeyState (VK_CONTROL) & BIT15) ? TRUE : FALSE;
            ksMenu  = (GetKeyState (VK_MENU )   & BIT15) ? TRUE : FALSE;

            // If the virtual key is alphabetic,
            //   and the Alt key is up,
            //   and the Ctrl key is up,
            //   and the CapsLock key is set, ...
            if ('A' <= nVirtKey
             &&        nVirtKey <= 'Z'
             && !ksMenu
             && !ksCtrl
             && (GetKeyState (VK_CAPITAL) & BIT0))
                // Toggle the shift state
                ksShft ^= TRUE;

            // Handle Numlock key
            if (GetKeyState (VK_NUMLOCK) & BIT0)
            {
                if (VK_NUMPAD0 <= nVirtKey
                 &&               nVirtKey <= VK_NUMPAD9)
                {
                    // Convert to origin-0
                    uTmp = nVirtKey - VK_NUMPAD0;

                    // Convert to scancode for '1' to '9', '0'
                    keyData.scanCode = (uTmp EQ 0) ? SCANCODE_0 : (SCANCODE_1 + uTmp - 1);

                    // Convert to virtkey  for '0' to '9'
                    nVirtKey += '0' - VK_NUMPAD0;
                } else
                if (nVirtKey EQ VK_DECIMAL)
                {
                    keyData.scanCode = SCANCODE_DECIMAL;
                    nVirtKey         = '.';
                } // End IF/ELSE
            } // End IF

#ifdef DEBUG_WM_KEYDOWN
            dprintfWL0 (L"WM_SYSKEYDOWN:  nVirtKey = %02X(%c), ScanCode = %02X, SCM = %u%u%u",
                        nVirtKey,
                        nVirtKey,
                        keyData.scanCode,
                        ksShft, ksCtrl, ksMenu);
#endif
            // Ensure the scanCode is within range of our table
            if (keyData.scanCode < aKeybLayoutAct.uCharCodesLen)
            {
                // Set the keyb state
                uState = (ksShft << 2) | (ksCtrl << 1) | (ksMenu << 0);

                // Get the corresponding character
                wchCode[0] = aKeybLayoutAct.aCharCodes[keyData.scanCode].wc[uState];

                // Check for valid character
                if (wchCode[0])
                {
                    // If this control allows numbers only, ...
                    if (ES_NUMBER & GetWindowLongW (hWnd, GWL_STYLE))
                    {
                        if (L'0' > wchCode[0]
                         ||        wchCode[0] > L'9')
                            break;
                    } // End IF

                    // Insert/replace the char string
                    InsRepCharStr (hWnd, wchCode, lpMemPTD EQ NULL);

                    return FALSE;       // We handled the msg
                } // End IF
            } // End IF

            // Process the virtual key
            switch (nVirtKey)
            {
                static UINT AltTrans[] =
                {
                    5,  // VK_CLEAR       0x0C
                   -1,  // VK_RETURN      0x0D
                   -1,  //                0x0E
                   -1,  //                0x0F
                   -1,  // VK_SHIFT       0x10
                   -1,  // VK_CONTROL     0x11
                   -1,  // VK_MENU        0x12
                   -1,  // VK_PAUSE       0x13
                   -1,  // VK_CAPITAL     0x14
                   -1,  // VK_KANA        0x15
                   -1,  //                0x16
                   -1,  // VK_JUNJA       0x17
                   -1,  // VK_FINAL       0x18
                   -1,  // VK_KANJI       0x19
                   -1,  //                0x1A
                   -1,  // VK_ESCAPE      0x1B
                   -1,  // VK_CONVERT     0x1C
                   -1,  // VK_NONCONVERT  0x1D
                   -1,  // VK_ACCEPT      0x1E
                   -1,  // VK_MODECHANGE  0x1F
                   -1,  // VK_SPACE       0x20
                    9,  // VK_PRIOR       0x21
                    3,  // VK_NEXT        0x22
                    1,  // VK_END         0x23
                    7,  // VK_HOME        0x24
                    4,  // VK_LEFT        0x25
                    8,  // VK_UP          0x26
                    6,  // VK_RIGHT       0x27
                    2,  // VK_DOWN        0x28
                   -1,  // VK_SELECT      0x29
                   -1,  // VK_PRINT       0x2A
                   -1,  // VK_EXECUTE     0x2B
                   -1,  // VK_SNAPSHOT    0x2C
                    0,  // VK_INSERT      0x2D
                };

#define AltAccum(a)     uAltNum *= 10; uAltNum += (a)   // Accumulate the digit

                case VK_NUMPAD0:    // 0
                case VK_NUMPAD1:    // 1
                case VK_NUMPAD2:    // 2
                case VK_NUMPAD3:    // 3
                case VK_NUMPAD4:    // 4
                case VK_NUMPAD5:    // 5
                case VK_NUMPAD6:    // 6
                case VK_NUMPAD7:    // 7
                case VK_NUMPAD8:    // 8
                case VK_NUMPAD9:    // 9
                    // Accumulate the digit
                    AltAccum (nVirtKey - VK_NUMPAD0);

                    return FALSE;           // We handled the msg

                case VK_INSERT:     // 0
                case VK_END:        // 1
                case VK_DOWN:       // 2
                case VK_NEXT:       // 3
                case VK_LEFT:       // 4
                case VK_CLEAR:      // 5
                case VK_RIGHT:      // 6
                case VK_HOME:       // 7
                case VK_UP:         // 8
                case VK_PRIOR:      // 9
                    // Accumulate the digit
                    AltAccum (AltTrans[nVirtKey - VK_CLEAR]);

                    return FALSE;           // We handled the msg
#undef  AltAccum
                default:
                    break;
            } // End SWITCH

            break;
#ifndef DEBUG
  #undef  keyData
  #undef  nVirtKey
#endif
        } // End WM_SYSKEYDOWN

////         case WM_SYSKEYUP:           // nVirtKey = (int) wParam;         // Virtual-key code
////         {                           // lKeyData = *(LPKEYDATA) &lParam; // Key data
//// #ifdef DEBUG
////             int     nVirtKey    = (int) wParam;
////             KEYDATA keyData     = *(LPKEYDATA) &lParam;
//// #else
////   #define nVirtKey      ((int) wParam)
////   #define keyData       (*(LPKEYDATA) &lParam)
//// #endif
////             // Get the key states
////             ksShft  = (GetKeyState (VK_SHIFT)   & BIT15) ? TRUE : FALSE;
////             ksCtrl  = (GetKeyState (VK_CONTROL) & BIT15) ? TRUE : FALSE;
////             ksMenu  = (GetKeyState (VK_MENU )   & BIT15) ? TRUE : FALSE;
////
////             // If the virtual key is alphabetic,
////             //   and the Alt key is up,
////             //   and the Ctrl key is up,
////             //   and the CapsLock key is set, ...
////             if ('A' <= nVirtKey
////              &&        nVirtKey <= 'Z'
////              && !ksMenu
////              && !ksCtrl
////              && (GetKeyState (VK_CAPITAL) & BIT0))
////                 // Toggle the shift state
////                 ksShft ^= TRUE;
////
////            // Handle Numlock key
////            if (GetKeyState (VK_NUMLOCK) & BIT0
////             && VK_NUMPAD0 <= nVirtKey
////             &&               nVirtKey <= VK_NUMPAD9)
////            {
////                UINT uTmp;
////
////                // Convert to origin-0
////                uTmp = nVirtKey - VK_NUMPAD0;
////
////                // Convert to scancode for '1' to '9', '0'
////                keyData.scanCode = (uTmp EQ 0) ? SCANCODE_0 : (SCANCODE_1 + uTmp - 1);
////
////                // Convert to virtkey  for '0' to '9'
////                nVirtKey += '0' - VK_NUMPAD0;
////            } // End IF
////
//// #ifdef DEBUG_WM_KEYUP
////             dprintfWL0 (L"WM_SYSKEYUP:    nVirtKey = %02X(%c), ScanCode = %02X, SCM = %u%u%u%u",
////                         nVirtKey,
////                         nVirtKey,
////                         keyData.scanCode,
////                         ksShft, ksCtrl, ksMenu);
//// #endif
////             break;
//// #ifndef DEBUG
////   #undef  keyData
////   #undef  nVirtKey
//// #endif
////         } // End WM_SYSKEYUP

        case WM_CHAR:               // wchCharCode = (TCHAR) wParam;    // character code
        {                           // lKeyData = *(LPKEYDATA) &lParam; // Key data
#ifdef DEBUG
            WCHAR   wchCharCode = (WCHAR) wParam;
            KEYDATA keyData     = *(LPKEYDATA) &lParam;
#else
  #define wchCharCode   ((WCHAR) wParam)
  #define keyData       (*(LPKEYDATA) &lParam)
#endif
            // If the transition state is set, then this char
            //   seems to come from the secondary char generated
            //   by Alt-nnnn
            if (keyData.transitionState)
                return FALSE;

            // Get the key states
            // Note that the shift state (L- and R-shift and CapsLock)
            //   has already been taken into consideration, so there's
            //   no need to obtain the shift state., and if the Alt key
            //   is down, then WM_SYSCHAR is called, not WM_CHAR.
            ksShft  = (GetKeyState (VK_SHIFT)   & BIT15) ? TRUE : FALSE;
            ksCtrl  = (GetKeyState (VK_CONTROL) & BIT15) ? TRUE : FALSE;
////////////ksMenu  = (GetKeyState (VK_MENU )   & BIT15) ? TRUE : FALSE;

#ifdef DEBUG_WM_CHAR
            ksMenu  = (GetKeyState (VK_MENU )   & BIT15) ? TRUE : FALSE;
            dprintfWL0 (L"WM_CHAR:      CharCode = %02X(%c), ScanCode = %02X, SCM = %u%u%u",
                         wchCharCode,
                         wchCharCode,
                         keyData.scanCode,
                         ksShft, ksCtrl, ksMenu);
#endif
            // Check for Tab
            if (wchCharCode EQ WC_HT)
            {
                // If it's from MF, ...
                if (lpMemPTD EQ NULL)
                    // Let the caller handle it
                    break;
                else
                    // Ignore Tab as we handled it in WM_KEYDOWN
                    return FALSE;       // We handled the msg
            } // End IF

            // Check for Return
            if (wchCharCode EQ WC_CR        // It's CR
             && IzitSM (GetParent (hWnd)))  // Parent is SM
            {
                // If not Shift-CR and not Ctrl-CR and it's on the last line, move the caret to the EOL (EOB)
                if (!ksShft
                 && !ksCtrl
                 && IzitLastLine (hWnd))
                    MoveCaretEOB (hWnd);        // Move to the End-of-Buffer
                else
                // Otherwise, let the SM handle copying and restoring the lines
                    return FALSE;
            } // End IF

            // Check for Ctrl-C (Copy)
            if (aKeybLayoutAct.bUseCXV
             && ksCtrl
             && wchCharCode EQ 03)
            {
                KEYDATA lKeyData = *(LPKEYDATA) &lParam;

                // If it's NOT from a VK_CANCEL key, ...
                if (uCancelCode NE lKeyData.scanCode)
                    // Post to ourselves a request to Copy
                    PostMessageW (hWnd, WM_COPY, 0, 0);

                return FALSE;       // We handled the msg
            } // End IF

            // Check for Ctrl-X (Cut)
            if (aKeybLayoutAct.bUseCXV
             && ksCtrl
             && wchCharCode EQ 24)
            {
                // Post to ourselves a request to Cut
                PostMessageW (hWnd, WM_CUT, 0, 0);

                return FALSE;       // We handled the msg
            } // End IF

            // Check for Ctrl-V (Paste)
            if (aKeybLayoutAct.bUseCXV
             && ksCtrl
             && wchCharCode EQ 22)
            {
                // Post to ourselves a request to paste
                PostMessageW (hWnd, WM_PASTE, 0, 0);

                return FALSE;       // We handled the msg
            } // End IF

            // Check for Ctrl-Z (Redo)
            if (aKeybLayoutAct.bUseZY
             && ksCtrl
             && wchCharCode EQ 26)
            {
                // Post to ourselves a request to Undo
                PostMessageW (hWnd, WM_UNDO, 0, 0);

                return FALSE;       // We handled the msg
            } // End IF

            // Check for Ctrl-Y (Redo)
            if (aKeybLayoutAct.bUseZY
             && ksCtrl
             && wchCharCode EQ 25)
            {
                // Post to ourselves a request to Redo
                PostMessageW (hWnd, WM_REDO, 0, 0);

                return FALSE;       // We handled the msg
            } // End IF

            // Check for Ctrl-S (Save)
            if (aKeybLayoutAct.bUseSEQ
             && wchCharCode EQ 19
             && ksCtrl
             && IzitFE (GetParent (hWnd)))  // Parent is FE
            {
                // Post a request to ourselves to save the function
                PostMessageW (hWnd, MYWM_SAVE_FN, wParam, lParam);

                return FALSE;       // We handled the msg
            } // End IF

            // Check for Ctrl-E (Save and End)
            if (aKeybLayoutAct.bUseSEQ
             && wchCharCode EQ 5
             && ksCtrl
             && IzitFE (GetParent (hWnd)))  // Parent is FE
            {
                // Post a request to ourselves to save & close the function
                PostMessageW (hWnd, MYWM_SAVECLOSE_FN, wParam, lParam);

                return FALSE;       // We handled the msg
            } // End IF

            // Check for Ctrl-Q (Close)
            if (aKeybLayoutAct.bUseSEQ
             && wchCharCode EQ 17
             && ksCtrl
             && IzitFE (GetParent (hWnd)))  // Parent is FE
            {
                // Post a request to ourselves to close the function
                PostMessageW (hWnd, MYWM_CLOSE_FN, wParam, lParam);

                return FALSE;       // We handled the msg
            } // End IF

////////////// Check for Ctrl-A (SaveAs)        // ***FIXME*** -- Make this work??
////////////if (wchCharCode EQ 1
//////////// && ksCtrl
//////////// && IzitFE (GetParent (hWnd)))  // Parent is FE
////////////{
////////////    // Post a request to ourselves to save the function under a different name
////////////    PostMessageW (hWnd, MYWM_SAVE_AS_FN, wParam, lParam);
////////////
////////////    return FALSE;       // We handled the msg
////////////} // End IF

            // Ensure we're not editing an AFO,
            //   and this is not the function header line 0)

            // If it's an AFE, ...
            if (IzitAFE (hWnd, FALSE))
            {
                // Check for chars we need to ignore (BS, LF, CR)
                if (!ksCtrl
                 && (wchCharCode EQ WC_BS
                  || wchCharCode EQ WC_LF
                  || wchCharCode EQ WC_CR))
                    return FALSE;       // We handled the msg
            } else
            // Check for chars we need to pass on (BS, LF, CR)
            if (!ksCtrl
             && (wchCharCode EQ WC_BS
              || wchCharCode EQ WC_LF
              || wchCharCode EQ WC_CR))
                break;

            return FALSE;           // We handled the msg
        } // End WM_CHAR
#ifndef DEBUG
  #undef  keyData
  #undef  wchCharCode
#endif

        case WM_SYSCHAR:            // wchCharCode = (TCHAR) wParam;    // character code
        {                           // lKeyData = *(LPKEYDATA) &lParam; // Key data
#ifdef DEBUG
            WCHAR   wchCharCode = (WCHAR) wParam;
            KEYDATA keyData     = *(LPKEYDATA) &lParam;
#else
  #define wchCharCode   ((WCHAR) wParam)
  #define keyData       (*(LPKEYDATA) &lParam)
#endif
            wchCharCode = wchCharCode;  // Respecify to avoid compiler error (unreferenced var)
            keyData = keyData;

#ifdef DEBUG_WM_CHAR
            dprintfWL0 (L"WM_SYSCHAR:   CharCode = %02X(%c), ScanCode = %02X", wchCharCode, wchCharCode, keyData.scanCode);
#endif
            return FALSE;           // We handled the msg
#ifndef DEBUG
  #undef  keyData
  #undef  wchCharCode
#endif
        } // End WM_SYSCHAR

        case WM_UNDO:               // 0 = wParam
                                    // 0 = lParam
            // If from MF, pass on this message
            if (lpMemPTD EQ NULL)
                break;

            // Get the handle of the parent window
            hWndParent = GetParent (hWnd);

            // Get the ptrs to our Undo Buffer
            lpUndoBeg = (LPUNDO_BUF) GetWindowLongPtrW (hWndParent, GWLSF_UNDO_BEG);
            lpUndoNxt = (LPUNDO_BUF) GetWindowLongPtrW (hWndParent, GWLSF_UNDO_NXT);

            // If there's more to Undo, ...
            if (lpUndoBeg < lpUndoNxt)
            do
            {
                // Split cases based upon the previous Undo Action
                switch ((--lpUndoNxt)->Action)
                {
                    case undoNone:
                        lpUndoNxt--;

                        break;

                    case undoIns:
                        // Get the character position in the Undo Entry
                        uCharPosBeg = lpUndoNxt->CharPosBeg;

                        // Get the corresponding line #
                        uLineNum = (DWORD) SendMessageW (hWnd, EM_LINEFROMCHAR, uCharPosBeg, 0);

                        // Unset the selection to this char position
                        SendMessageW (hWnd, EM_SETSEL, uCharPosBeg, uCharPosBeg);

                        // Put the character into a string
                        wChar[0] = lpUndoNxt->Char;
                        wChar[1] = WC_EOS;

                        // Insert the selection
                        ReplaceSel (hWnd, FALSE, (LPARAM) &wChar);

                        // Move to the line
                        MoveToLine (uLineNum, lpMemPTD, hWnd);

                        break;

                    case undoRep:
                        // Get the character position in the Undo Entry
                        uCharPosBeg = lpUndoNxt->CharPosBeg;

                        // Get the corresponding line #
                        uLineNum = (DWORD) SendMessageW (hWnd, EM_LINEFROMCHAR, uCharPosBeg, 0);

                        // Set the selection to this char position
                        SendMessageW (hWnd, EM_SETSEL, uCharPosBeg, uCharPosBeg + 1);

                        // Put the character into a string
                        wChar[0] = lpUndoNxt->Char;
                        wChar[1] = WC_EOS;

                        // Replace the selection
                        ReplaceSel (hWnd, FALSE, (LPARAM) &wChar);

                        // Move to the line
                        MoveToLine (uLineNum, lpMemPTD, hWnd);

                        break;

                    case undoDel:
                        // Get the character indices in the Undo Entry
                        uCharPosBeg = lpUndoNxt->CharPosBeg;
                        uCharPosEnd = lpUndoNxt->CharPosEnd;

                        // Get the corresponding line #
                        uLineNum = (DWORD) SendMessageW (hWnd, EM_LINEFROMCHAR, uCharPosBeg, 0);

                        // Set the selection to these chars
                        SendMessageW (hWnd, EM_SETSEL, uCharPosBeg, uCharPosEnd);

                        // Delete the selection
                        ReplaceSel (hWnd, FALSE, (LPARAM) L"");

                        // Move to the line
                        MoveToLine (uLineNum, lpMemPTD, hWnd);

                        break;

                    case undoSel:
                        // Get the character indices in the Undo Entry
                        uCharPosBeg = lpUndoNxt->CharPosBeg;
                        uCharPosEnd = lpUndoNxt->CharPosEnd;

                        // Get the corresponding line #
                        uLineNum = (DWORD) SendMessageW (hWnd, EM_LINEFROMCHAR, uCharPosBeg, 0);

                        // Set the selection to these chars
                        SendMessageW (hWnd, EM_SETSEL, uCharPosBeg, uCharPosEnd);

                        // Move to the line
                        MoveToLine (uLineNum, lpMemPTD, hWnd);

                        break;

                    case undoInsToggle:
                        // Get the current vkState
                        vkState = GetVkState (hWnd);

                        // Toggle the insert state
                        vkState.Ins = !vkState.Ins;

                        // Tell the Status Window about it
                        SetStatusIns (vkState.Ins);

                        // Save in window extra bytes
                        SetWindowLongW (hWnd, GWLEC_VKSTATE, *(long *) &vkState);

                        break;

                    defstop
                        break;
                } // End SWITCH
            } while (lpUndoNxt->Group NE UNDO_NOGROUP
                  && lpUndoNxt->Group EQ lpUndoNxt[-1].Group);

            // Save the new Undo Next ptr
            SetWindowLongPtrW (hWndParent, GWLSF_UNDO_NXT, (APLU3264) (LONG_PTR) lpUndoNxt);

            // Draw the line #s
            DrawLineNumsFE (hWnd);

            return TRUE;

        case EM_CANUNDO:
            // If from MF, pass on this message
            if (lpMemPTD EQ NULL)
                break;

            // Get the ptrs to the next available slot in our Undo Buffer
            lpUndoNxt = (LPUNDO_BUF) GetWindowLongPtrW (hWnd, GWLSF_UNDO_NXT);
            lpUndoBeg = (LPUNDO_BUF) GetWindowLongPtrW (hWnd, GWLSF_UNDO_BEG);

            return (lpUndoBeg NE lpUndoNxt);

        case WM_REDO:
////////////DbgBrk ();              // ***FINISHME*** -- Make Redo work??






            break;

        case WM_CUT:                // 0 = wParam
                                    // 0 = lParam
        case WM_CLEAR:              // 0 = wParam
                                    // 0 = lParam
            // Delete selected chars and (if WM_CUT) copy to clipboard

            // If it's an AFE ...
            if (IzitAFE (hWnd, FALSE))
                return FALSE;       // We handled the message

            // If from MF, pass on this message
            if (lpMemPTD EQ NULL)
                break;

            // Get the indices of the selected text (if any)
            SendMessageW (hWnd, EM_GETSEL, (WPARAM) &uCharPosBeg, (LPARAM) &uCharPosEnd);

            // If there's no selected text, ignore this msg
            if (uCharPosBeg EQ uCharPosEnd)
                break;

            // Get the handle of the parent window
            hWndParent = GetParent (hWnd);

            // Undo inserts the selected chars

            // Get the next group index, and save it back
            uGroupIndex = 1 + GetWindowLongW (hWndParent, GWLSF_UNDO_GRP);
            SetWindowLongW (hWndParent, GWLSF_UNDO_GRP, (UINT) uGroupIndex);

            // Append an Undo action to set the selection
            AppendUndo (hWndParent,                     // SM/FE Window handle
                        GWLSF_UNDO_NXT,                 // Offset in hWnd extra bytes of lpUndoNxt
                        undoSel,                        // Action
                        uCharPosBeg,                    // Beginning char position
                        uCharPosEnd,                    // Ending    ...
                        uGroupIndex,                    // Group index
                        0);                             // Character
            // Loop through the selected chars (if any)
            for (uCharPos = uCharPosBeg; uCharPos < uCharPosEnd; uCharPos++)
                AppendUndo (hWndParent,                     // SM/FE Window handle
                            GWLSF_UNDO_NXT,                 // Offset in hWnd extra bytes of lpUndoNxt
                            undoIns,                        // Action
                            uCharPosBeg,                    // Beginning char position
                            0,                              // Ending    ...
                            uGroupIndex,                    // Group index
                            GetCharValue (hWnd, uCharPos)); // Character
            // Tell the ending code to draw the line #s afterwards
            bDrawLineNums = TRUE;

            break;

        case WM_PASTE:              // bPasteAPL = (BOOL) wParam
                                    // 0 = lParam
            // If it's an AFE ...
            if (IzitAFE (hWnd, FALSE))
                return FALSE;       // We handled the message

            // If we've not already laundered the input, do so now
            if (wParam)
                // Run through the "Normal" processing
                PasteAPLChars_EM (hWnd, UNITRANS_NORMAL);
            else
                // Insert text from the clipboard, deleting selected text (if any)
                PasteAPLChars_EM (hWnd, OptionFlags.uDefaultPaste);

            // If from MF, pass on this message
            if (lpMemPTD EQ NULL)
                break;

            // Undo deletes the inserted (pasted) chars
            //   and insert the deleted chars (if any)

            // Get the handle of the parent window
            hWndParent = GetParent (hWnd);

            // Get the indices of the selected text (if any)
            SendMessageW (hWnd, EM_GETSEL, (WPARAM) &uCharPosBeg, (LPARAM) &uCharPosEnd);

            // Note if there's a selection
            bSelection = uCharPosBeg NE uCharPosEnd;

            // Get the next group index, and save it back
            uGroupIndex = 1 + GetWindowLongW (hWndParent, GWLSF_UNDO_GRP);
            SetWindowLongW (hWndParent, GWLSF_UNDO_GRP, (UINT) uGroupIndex);

            // Append an Undo action to set the selection if there was one before
            if (bSelection)
                AppendUndo (hWndParent,                     // SM/FE Window handle
                            GWLSF_UNDO_NXT,                 // Offset in hWnd extra bytes of lpUndoNxt
                            undoSel,                        // Action
                            uCharPosBeg,                    // Beginning char position
                            uCharPosEnd,                    // Ending    ...
                            uGroupIndex,                    // Group index
                            0);                             // Character
            // Append the undo action (insert deleted (selected) chars)
            // Loop through the selected chars (if any)
            for (uCharPos = uCharPosBeg; uCharPos < uCharPosEnd; uCharPos++)
                AppendUndo (hWndParent,                     // SM/FE Window handle
                            GWLSF_UNDO_NXT,                 // Offset in hWnd extra bytes of lpUndoNxt
                            undoIns,                        // Action
                            uCharPosBeg,                    // Beginning char position
                            0,                              // Ending    ...
                            uGroupIndex,                    // Group index
                            GetCharValue (hWnd, uCharPos)); // Character
            // Open the clipboard so we can read from it
            OpenClipboard (hWnd);

            // Get a handle to the clipboard data
            hGlbClip = GetClipboardData (CF_PRIVATEFIRST);
            if (hGlbClip EQ NULL)
                hGlbClip = GetClipboardData (CF_UNICODETEXT);

            // If there's data in the clipboard, ...
            if (hGlbClip NE NULL)
            {
                // Lock the memory to get a ptr to it
                // Note we can't use MyGlobalLock/Unlock as the lock count
                //   is not modified for a clipboard (non-owned) handle
                lpMemClip = GlobalLock (hGlbClip); Assert (lpMemClip NE NULL);

                // The ending position is the beginning position plus the string length
                uCharPosEnd = uCharPosBeg + lstrlenW (lpMemClip);

                // We no longer need this ptr
                GlobalUnlock (hGlbClip); lpMemClip = NULL;

                // Append the undo action (delete inserted (pasted) chars)
                AppendUndo (hWndParent,                     // SM/FE Window handle
                            GWLSF_UNDO_NXT,                 // Offset in hWnd extra bytes of lpUndoNxt
                            undoDel,                        // Action
                            uCharPosBeg,                    // Beginning char position
                            uCharPosEnd,                    // Ending    ...
                            uGroupIndex,                    // Group index
                            0);                             // Character
            } // End IF

            // We're done with the clipboard and its handle
            CloseClipboard (); hGlbClip = NULL;

            // Tell the ending code to draw the line #s afterwards
            bDrawLineNums = TRUE;

            break;

        case MYWM_PASTE_APL:
            PasteAPLChars_EM (hWnd, (UINT) wParam);
            PostMessageW (hWnd, WM_PASTE, TRUE, 0);

            return FALSE;           // We handled the msg

        case WM_COPY:               // bCopyAPL = (BOOL) wParam
                                    // 0 = lParam
            lResult = CallWindowProcW (lpfnOldEditCtrlWndProc,
                                       hWnd,
                                       WM_COPY,
                                       0,
                                       lParam);     // Pass on down the line
            // If we've not already laundered the input, do so now
            if (wParam EQ 0)
                // Copy text to the clipboard
                CopyAPLChars_EM (hWnd, OptionFlags.uDefaultCopy);
            return lResult;         // We handled the msg

        case MYWM_COPY_APL:
            // Let the Edit Ctrl do its thing
            lResult = CallWindowProcW (lpfnOldEditCtrlWndProc,
                                       hWnd,
                                       WM_COPY,
                                       0,
                                       lParam);     // Pass on down the line
            CopyAPLChars_EM (hWnd, (UINT) wParam);

            return lResult;         // We handled the msg

        case MYWM_SAVE_FN:
        case MYWM_SAVECLOSE_FN:
        case MYWM_CLOSE_FN:
        case MYWM_SAVE_AS_FN:
            PostMessageW (GetParent (hWnd), message, wParam, lParam);

            return FALSE;           // We handled the msg

        case MYWM_LOCALIZE:
        case MYWM_UNLOCALIZE:
        {
            LPSYMENTRY lpSymEntry;      // Ptr to the SYMENTRY under the name
            LPWCHAR    lpwGlbName;      // Ptr to SymEntry's name's global memory
            UINT       xPos,            // Horizontal position of cursor in screen coords
                       yPos,            // Vertical   ...
                       uPos,            // Position of name in header or position of 1st semicolon
                       uLen;            // Length of the name to localize/unlocalize

            // Get the cursor position when right clicked
            xPos = LOSHORT ((HANDLE_PTR) GetPropW (hWnd, L"TIMER.LPARAM"));
            yPos = HISHORT ((HANDLE_PTR) GetPropW (hWnd, L"TIMER.LPARAM"));

            // Get the corresponding STE
            lpSymEntry = (LPSYMENTRY) SendMessageW (hWnd, MYWM_IZITNAME, xPos, yPos);
            if (lpSymEntry NE NULL && lpwszTemp NE NULL)
            {
                VARS_TEMP_OPEN
                CHECK_TEMP_OPEN

                // Lock the memory to get a ptr to it
                lpwGlbName = MyGlobalLockWsz (lpSymEntry->stHshEntry->htGlbName);

                // Get the name length
                uLen = lstrlenW (lpwGlbName);

                // If the name is already local, ...
                if (IsLocalName (lpwGlbName, uLen, hWnd, lpwszTemp, &uPos))
                {
                    // If we're unlocalizing, ...
                    if (message EQ MYWM_UNLOCALIZE)
                    {
                        // Move to the left over leading white space
                        while (IsWhiteW (lpwszTemp[uPos - 1]))
                        {
                            uPos--;
                            uLen++;
                        } // End WHILE

                        Assert (lpwszTemp[uPos - 1] EQ L';');

                        // Move to the left to the leading semicolon
                        uPos--;
                        uLen++;

                        // Select the name
                        SendMessageW (hWnd, EM_SETSEL, uPos, uPos + uLen);

                        // Delete it
                        ReplaceSel (hWnd, FALSE, (LPARAM) L"");

                        // Repaint the Edit Ctrl window so the name changes color
                        InvalidateRect (hWnd, NULL, FALSE);
                    } // End IF
                } else
                {
                    // If we're localizing, ...
                    if (message EQ MYWM_LOCALIZE)
                    {
                        // Select the insertion point
                        SendMessageW (hWnd, EM_SETSEL, uPos, uPos);

                        // Insert the leading semicolon
                        ReplaceSel (hWnd, FALSE, (LPARAM) L";");

                        // Select the insertion point
                        SendMessageW (hWnd, EM_SETSEL, uPos + 1, uPos + 1);

                        // Insert the name
                        ReplaceSel (hWnd, FALSE, (LPARAM) lpwGlbName);

                        // Repaint the Edit Ctrl window so the name changes color
                        InvalidateRect (hWnd, NULL, FALSE);
                    } // End IF
                } // End IF/ELSE

                // We no longer need this resource
                MyGlobalUnlock (lpSymEntry->stHshEntry->htGlbName); lpwGlbName = NULL;

                EXIT_TEMP_OPEN
            } // End IF

            return FALSE;           // We handled the msg
        } // End MYWM_(UN)LOCALIZE

        case WM_SETFONT:
        {
            HBITMAP hBitmap;        // Handle of the replace caret bitmap
            USHORT  bits[1024];     // We need (cxAveCharXX x cyAveCharXX) / 8 bytes
            UINT    fontEnum,       // FONTENUM_xx value
                    uLen,           // # words in each row of the text caret (1+(sizeChar.cx-1)/16)
                    uCnt;           // Loop counter
            HWND    hWndParent;     // Handle of parent window
            HFONT   hFontFB,        // HFONT for Fallback font for either SM or FE (whichever is active)
                    hFontFB_PR;     // ...                     for printing for either ...
            SIZE    sizeChar;       // cx & cy of the average char

            // Get the caret replace bitmap handle (if any)
            hBitmap = (HBITMAP) GetWindowLongPtrW (hWnd, GWLEC_HBITMAP);

            // If it's defined, ...
            if (hBitmap NE NULL)
            {
                // Delete any existing replace caret bitmap
                DeleteObject (hBitmap); hBitmap = NULL;
            } // End IF

            // Get the parent window handle
            hWndParent = GetParent (hWnd);

            // Use the appropriate FONTENUM_xx value
            if (IzitSM (hWndParent))
            {
                fontEnum = FONTENUM_SM;

                // Setup Fallback fonts for printing and non-printing
                hFontFB_PR = hFontFB_PR_SM;
                hFontFB    = hFontFB_SM;
            } else
            if (IzitFE (hWndParent)
             || IzitDialog (hWndParent))
            {
                fontEnum = FONTENUM_FE;

                // Setup Fallback fonts for printing and non-printing
                hFontFB_PR = hFontFB_PR_FE;
                hFontFB    = hFontFB_FE;
#ifdef DEBUG
            } else
            {
                DbgStop ();             // #ifdef DEBUG
#endif
            } // End IF/ELSE/...

            // Get the caret width and height
            sizeChar = *GetFSIndAveCharSize (fontEnum);

            // Set the bits
            ZeroMemory (bits, sizeof (bits));

            // Calculate the # words in each row of the text caret,
            //  typically this value ranges from 1 to 5.
            uLen = 1 + ((sizeChar.cx - 1) / 16);

            // Loop through the text caret width
            for (uCnt = 0; uCnt < uLen; uCnt++)
            {
                // First row
                bits[(sizeChar.cy - 2) * uLen + uCnt] = -1;

                // Second row
                bits[(sizeChar.cy - 1) * uLen + uCnt] = -1;
            } // End FOR

            // Create a bitmap for replace mode
            hBitmap = CreateBitmap (sizeChar.cx,    // Bitmap width, in pixels
                                    sizeChar.cy,    // ...    height, ...
                                    1,              // # color planes used by device
                                    1,              // # bits requried to identify a color
                                   &bits);          // Ptr to array containing color data
            // Save the handle
            SetWindowLongPtrW (hWnd, GWLEC_HBITMAP, (HANDLE_PTR) hBitmap);

            // If Edit Ctrl for SM, ...
            if (IzitSM (hWndParent))
                // Change []PW to track the new width
                RespecifyNewQuadPW (hWnd, 0);

            // If we're Output debugging, ...
            if (OptionFlags.bOutputDebug)
                // Tell the EC about our Fallback font
                SendMessageW (hWnd, EM_SETFALLBACKFONT, (WPARAM) hFontFB_PR, (LPARAM) hFontFB);

            break;                  // Pass on to the Edit Ctrl
        } // End WM_SETFONT

#define lpnmEC  (*(LPNMEDITCTRL *) &lParam)
        case MYWM_NOTIFY:
            // Split cases based upon the notification code
            switch (lpnmEC->nmHdr.code)
            {
                HBITMAP hBitmap;        // Caret bitmap for replace mode

                case EN_KILLFOCUS:
                    // Delete the replace mode bitmap (if any)
                    hBitmap = (HBITMAP) GetWindowLongPtrW (hWnd, GWLEC_HBITMAP);

                    // If it's valid
                    if (hBitmap NE NULL)
                    {
                        // Delete it
                        DeleteObject (hBitmap); hBitmap = NULL;

                        // Save the handle
                        SetWindowLongPtrW (hWnd, GWLEC_HBITMAP, (HANDLE_PTR) hBitmap);
                    } // End IF

                    break;

                case NM_LAST:
                    // Get the current vkState
                    vkState = GetVkState (hWnd);

                    // If it's insert mode, ...
                    if (vkState.Ins)
                        // Mark as no bitmap
                        lpnmEC->hBitMap = NULL;
                    else
                    // It's replace mode
                        // Return the handle
                        lpnmEC->hBitMap = (HBITMAP) GetWindowLongPtrW (hWnd, GWLEC_HBITMAP);

                    break;

                defstop
                    break;
            } // End SWITCH

            return FALSE;           // We handled the msg
#undef  lpnmEC

#define wNotifyCode     (HIWORD (wParam))
#define wID             (LOWORD (wParam))
#define hWndCtrl        ((HWND) lParam)
        case WM_COMMAND:            // wNotifyCode = HIWORD (wParam); // Notification code
                                    // wID = LOWORD (wParam);         // Item, control, or accelerator identifier
                                    // hwndCtrl = (HWND) lParam;      // Handle of control
            // Split cases based upon the ID
            switch (wID)
            {
                case IDM_UNDO:
                    SendMessageW (hWnd, WM_UNDO, 0, 0);

                    return FALSE;   // We handled the msg

                case IDM_REDO:
                    SendMessageW (hWnd, MYWM_REDO, 0, 0);

                    return FALSE;   // We handled the msg

                case IDM_CUT:
                    SendMessageW (hWnd, WM_CUT, 0, 0);

                    return FALSE;   // We handled the msg

                case IDM_COPY:
                    SendMessageW (hWnd, WM_COPY, 0, 0);

                    return FALSE;   // We handled the msg

                case IDM_COPY_APLWIN:
                    SendMessageW (hWnd, MYWM_COPY_APL, UNITRANS_APLWIN, 0);

                    return FALSE;   // We handled the msg

                case IDM_COPY_APL2:
                    SendMessageW (hWnd, MYWM_COPY_APL, UNITRANS_APL2, 0);

                    return FALSE;   // We handled the msg

                case IDM_COPY_DYALOG:
                    SendMessageW (hWnd, MYWM_COPY_APL, UNITRANS_DYALOG, 0);

                    return FALSE;   // We handled the msg

                case IDM_COPY_ISO:
                    SendMessageW (hWnd, MYWM_COPY_APL, UNITRANS_ISO, 0);

                    return FALSE;   // We handled the msg

                case IDM_COPY_PC3270:
                    SendMessageW (hWnd, MYWM_COPY_APL, UNITRANS_PC3270, 0);

                    return FALSE;   // We handled the msg

                case IDM_COPY_BRACES:
                    SendMessageW (hWnd, MYWM_COPY_APL, UNITRANS_BRACES, 0);

                    return FALSE;   // We handled the msg

                case IDM_PASTE:
                    SendMessageW (hWnd, WM_PASTE, FALSE, 0);

                    return FALSE;   // We handled the msg

                case IDM_PASTE_APLWIN:
                    SendMessageW (hWnd, MYWM_PASTE_APL, UNITRANS_APLWIN, 0);

                    return FALSE;   // We handled the msg

                case IDM_PASTE_APL2:
                    SendMessageW (hWnd, MYWM_PASTE_APL, UNITRANS_APL2, 0);

                    return FALSE;   // We handled the msg

                case IDM_PASTE_DYALOG:
                    SendMessageW (hWnd, MYWM_PASTE_APL, UNITRANS_DYALOG, 0);

                    return FALSE;   // We handled the msg

                case IDM_PASTE_ISO:
                    SendMessageW (hWnd, MYWM_PASTE_APL, UNITRANS_ISO, 0);

                    return FALSE;   // We handled the msg

                case IDM_PASTE_PC3270:
                    SendMessageW (hWnd, MYWM_PASTE_APL, UNITRANS_PC3270, 0);

                    return FALSE;   // We handled the msg

                case IDM_PASTE_BRACES:
                    SendMessageW (hWnd, MYWM_PASTE_APL, UNITRANS_BRACES, 0);

                    return FALSE;   // We handled the msg

                case IDM_DELETE:
                    SendMessageW (hWnd, WM_CLEAR, 0, 0);

                    return FALSE;   // We handled the msg

                case IDM_SELECTALL:
                    SendMessageW (hWnd, MYWM_SELECTALL, 0, (LPARAM) -1);

                    return FALSE;   // We handled the msg

                case IDM_LOCALIZE:
                    // Pass on the message
                    SendMessageW (hWnd, MYWM_LOCALIZE, 0, 0);

                    return FALSE;   // We handled the msg

                case IDM_UNLOCALIZE:
                    // Pass on the message
                    SendMessageW (hWnd, MYWM_UNLOCALIZE, 0, 0);

                    return FALSE;   // We handled the msg
            } // End SWITCH

            break;

        case WM_PAINT:              // hdc = (HDC) wParam; // the device context to draw in
        {
            RECT rcUpdate;          // Update rectangle

            // If from MF, pass on this message
            if (lpMemPTD EQ NULL)
                break;

            // If this tab isn't active, ignore the msg
            if (!IsCurTabActive ())
            {
                // If there's no incoming HDC, ...
                if (((HDC) wParam) EQ NULL)
                {
                    PAINTSTRUCT ps;

                    // Tell 'em we're done
                    BeginPaint (hWnd, &ps);
                    EndPaint   (hWnd, &ps);
                } // End IF

                return FALSE;       // We ignored the msg
            } // End IF

            // Get the update rectangle
            if (!GetUpdateRect (hWnd, &rcUpdate, FALSE))
                return FALSE;       // We handled the msg

            // If the rectangle doesn't already include the left edge, ...
            if (rcUpdate.left NE 0)
            {
                // Ensure the rectangle starts at the beginning of the line
                //   so Syntax Coloring can change the color of a preceding char
                rcUpdate.left = 0;

                // Add this rectangle to the window's update region
                InvalidateRect (hWnd, &rcUpdate, FALSE);
            } // End IF

            // Tell the ending code to draw the line #s afterwards
            bDrawLineNums = TRUE;

            break;
        } // End WM_PAINT

        case WM_SETFOCUS:           // hwndLoseFocus = (HWND) wParam; // handle of window losing focus
        case WM_LBUTTONDOWN:        // fwKeys = wParam;         // key flags
                                    // xPos = LOSHORT(lParam);  // horizontal position of cursor
                                    // yPos = HISHORT(lParam);  // vertical position of cursor
            // Let the Edit Ctrl move the caret
            lResult = CallWindowProcW (lpfnOldEditCtrlWndProc,
                                       hWnd,
                                       message,
                                       wParam,
                                       lParam);     // Pass on down the line
            // Repaint the status window
            InvalidateRect (hWndStatus, NULL, FALSE);

            // Tell the Status Window about the new positions
            SetStatusPos (hWnd);

            return FALSE;           // We handled the msg

        case WM_DESTROY:
        {
            HBITMAP hBitmap;        // Handle of the replace caret bitmap

            // Get the replace caret bitmap handle (if any)
            hBitmap = (HBITMAP) GetWindowLongPtrW (hWnd, GWLEC_HBITMAP);

            // Delete any replace caret bitmap
            if (hBitmap NE NULL)
            {
                // Delete it
                DeleteObject (hBitmap); hBitmap = NULL;

                // Save the handle
                SetWindowLongPtrW (hWnd, GWLEC_HBITMAP, (HANDLE_PTR) hBitmap);
            } // End IF

            break;
        } // End WM_DESTROY
    } // End SWITCH

////LCLODSAPI ("FEC:", hWnd, message, wParam, lParam);
    lResult = CallWindowProcW (lpfnOldEditCtrlWndProc,
                               hWnd,
                               message,
                               wParam,
                               lParam);     // Pass on down the line
    if (bDrawLineNums)
    {
        // Draw the line #s
        DrawLineNumsFE (hWnd);

        // Draw all Line Continuation markers
        DrawAllLineCont (hWnd);
    } // End IF

    return lResult;
} // End LclEditCtrlWndProc


//***************************************************************************
//  $RespecifyNewQuadPW
//
//  If allowed, respecify a new []PW based on the new SM font
//***************************************************************************

void RespecifyNewQuadPW
    (HWND hWndEC,                   // EC window handle
     int  nWidth)                   // Window width in pixels (0 = calculate it)

{
    APLUINT aplInteger;

    // If we are requested to do so, ...
    if (OptionFlags.bAdjustPW)
    {
        // If the incoming width is to be calculated here, ...
        if (nWidth EQ 0)
        {
            RECT rc;

            // Get the EC window's client rectangle
            GetClientRect (hWndEC, &rc);

            // Calculate the client area width
            nWidth = rc.right - rc.left;
        } // End IF

        // Less the left margin for SM
        nWidth -= LftMarginsSM ();

        // Calculate the floor of the new width in chars
        aplInteger = nWidth / GetFSIndAveCharSize (FONTENUM_SM)->cx;

        // Validate the incoming value
        if (ValidateIntegerTest (&aplInteger,       // Ptr to the integer to test
                                  DEF_MIN_QUADPW,   // Low range value (inclusive)
                                  DEF_MAX_QUADPW,   // High ...
                                  bRangeLimit.PW))  // TRUE iff we're range limiting
        {
            LPPERTABDATA lpMemPTD;                  // Ptr to PerTabData global memory

            // Get ptr to PerTabData global memory
            lpMemPTD = GetMemPTD ();

            // If the lphtsPTD is valid, ...
            if (lpMemPTD->lphtsPTD)
                // Save as new []PW
                lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_PW]->stData.stInteger = aplInteger;
#ifdef DEBUG
            else
                DbgStop ();         // #ifdef DEBUG
#endif
        } // End IF
    } // End IF
} // End RespecifyNewQuadPW


//***************************************************************************
//  $LclSetCursor
//
//  Set cursor to idle or running
//***************************************************************************

UBOOL LclSetCursor
    (HWND         hWndEC,               // Edit Ctrl window handle
     LPPERTABDATA lpMemPTD,             // Ptr to PerTabData global memory
     UINT         hitTest)              // Hit-test code

{
    HCURSOR hCursorNew;                 // New cursor
    LPWCHAR lpwszStatusMsg;             // Ptr to new Status Window message

    // If we're executing, ...
    if (lpMemPTD->bExecuting)
    {
        // Set the new cursor and message ptr
        hCursorNew     = hCursorWait;
        lpwszStatusMsg = wszStatusRunning;
    } else
    {
        // Set the new cursor and message ptr
        hCursorNew     = hCursorIdle;
        lpwszStatusMsg = wszStatusIdle;
    } // End IF/ELSE

    // In order to reduce Status Window flicker,
    //   compare the statusmsg-to-be with the existing one
    if (((HANDLE) lpwszStatusMsg) NE GetPropW (hWndStatus, PROP_STATUSMSG))
        // Set the status message
        SetStatusMsg (lpwszStatusMsg);

    // Set a new cursor to indicate our new state
    SetClassLongPtrW (hWndEC, GCLP_HCURSOR, (HANDLE_PTR) hCursorNew);

    // If the mouse is in the client area, ...
    if (hitTest EQ HTCLIENT)
        // Set the new cursor
        SetCursor (hCursorNew);

    // Return whether or not the mouse is in the client area, ...
    return (hitTest EQ HTCLIENT);
} // End LclSetCursor


//***************************************************************************
//  $ForceSendCursorMsg
//
//  Send WM_SETCURSOR message to the Edit Ctrl window
//    with a forced executing value
//***************************************************************************

void ForceSendCursorMsg
    (HWND  hWndEC,              // Edit Ctrl window handle
     UBOOL bExecuting)          // TRUE iff we're executing

{
    POINT        ptCursor;      // Position of the mouse cursor
    UINT         hitTest;       // Hit test for the mouse cursor
    LPPERTABDATA lpMemPTD;      // Ptr to this window's PerTabData
    UBOOL        bOldExecuting; // TRUE iff the old value was executing

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Indicate the new execution state
    bOldExecuting = lpMemPTD->bExecuting; SetExecuting (lpMemPTD, bExecuting);

    // Get the cursor position in screen coords
    GetCursorPos (&ptCursor);

    // If we're not in the process of destroying the Main window, ...
    // This is necessary sometimes for shutdown
    if (!bMainDestroy)
    {
        // Get the hit test value relative to the Edit Ctrl window
        hitTest = (UINT) SendMessageW (hWndEC, WM_NCHITTEST, 0, MAKELPARAM (ptCursor.x, ptCursor.y));

        // Set the cursor to indicate that we're executing
        SendMessageW (hWndEC, WM_SETCURSOR, (WPARAM) hWndEC, MAKELPARAM (hitTest, WM_MOUSEMOVE));
    } // End IF

    // Restore the previous executing state
    SetExecuting (lpMemPTD, bOldExecuting);
} // End ForceSendCursorMsg


//***************************************************************************
//  $CopyGlbMemory
//
//  Copy global memory from one handle to another
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CopyGlbMemory"
#else
#define APPEND_NAME
#endif

HGLOBAL CopyGlbMemory
    (HGLOBAL hGlbSrc,       // Global memory handle to copy
     UBOOL   bMyFns)        // TRUE iff we're to MyXxx functions

{
    HGLOBAL hGlbDst;        // The result global memory handle
    SIZE_T  dwSize;         // The size of the source
    LPVOID  lpMemSrc,       // Ptr to source global memory
            lpMemDst;       // Ptr to result ...

    // Check for bad handle
    if (!IsValidHandle (hGlbSrc))
        return NULL;

    // Get the size of the global memory object
    dwSize = MyGlobalSize (hGlbSrc);

    if (bMyFns)
    {
        // Allocate space for the result
        hGlbDst = DbgGlobalAlloc (GHND | GMEM_DDESHARE, dwSize);

        if (hGlbDst NE NULL)
        {
            // Lock both memory blocks
            lpMemDst = MyGlobalLock000 (hGlbDst);
            lpMemSrc = MyGlobalLock    (hGlbSrc);

            // Copy source to destin
            CopyMemory (lpMemDst, lpMemSrc, dwSize);

            // We no longer need these ptrs
            MyGlobalUnlock (hGlbDst); lpMemDst = NULL;
            MyGlobalUnlock (hGlbSrc); lpMemSrc = NULL;
        } // End IF
    } else
    {
        // Note we do not use MyGlobalAlloc or DbgGlobalAlloc here as the global memory handle
        //   is to be placed onto the clipboard at which point the system
        //   will own the handle

        // Allocate space for the result
        hGlbDst =    GlobalAlloc (GHND | GMEM_DDESHARE, dwSize);

        if (hGlbDst NE NULL)
        {
            // Lock both memory blocks
            lpMemDst = GlobalLock (hGlbDst);
            lpMemSrc = GlobalLock (hGlbSrc);

            // Copy source to destin
            CopyMemory (lpMemDst, lpMemSrc, dwSize);

            // We no longer need these ptrs
            GlobalUnlock (hGlbDst); lpMemDst = NULL;
            GlobalUnlock (hGlbSrc); lpMemSrc = NULL;
        } // End IF
    } // End IF/ELSE

    return hGlbDst;
} // End CopyGlbMemory
#undef  APPEND_NAME


//***************************************************************************
//  $CopyAPLChars_EM
//
//  Copy APL chars to another APL system
//***************************************************************************

void CopyAPLChars_EM
    (HWND      hWndEC,              // Window handle of the Edit Ctrl
     UNI_TRANS uIndex)              // UNI_TRANS index

{
    UINT         numChars;          // # chars on the clipboard
    HGLOBAL      hGlbClip = NULL,   // Clipboard global memory handle
                 hGlbText = NULL;   // Clipboard UNICODETEXT global memory handle
    LPVOID       lpMemClip = NULL;  // Ptr to clipboard global memory
    LPWCHAR      lpMemText = NULL;  // Ptr to clipboard UNICODETEXT global memory
    UINT         uTran,             // Loop counter
                 uText,             // Loop counter
                 uCount;            // # formats on the clipboard
    UBOOL        bUnicode;          // TRUE iff the clipboard contains Unicdoe chars
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPWCHAR      lpwszTemp;         // Ptr to temporary storage
    VARS_TEMP_OPEN

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get ptr to temp save area
    lpwszTemp = lpMemPTD->lpwszTemp;
    CHECK_TEMP_OPEN

    // Open the clipboard so we can write to it
    OpenClipboard (hWndEC);

    // Get the # formats on the clipboard
    uCount = CountClipboardFormats ();

    // If there are no formats, ...
    if (uCount EQ 0)
        goto NORMAL_EXIT;

    // Get a handle to the clipboard data for CF_UNICODETEXT
    hGlbClip = GetClipboardData (CF_UNICODETEXT);
    if (hGlbClip EQ NULL)
    {
        // Get a handle to the clipboard data for CF_TEXT
        hGlbClip = GetClipboardData (CF_TEXT);
        if (hGlbClip EQ NULL)
            goto NORMAL_EXIT;

        // Get the # chars
        numChars = (UINT) MyGlobalSize (hGlbClip);

        // Mark as not Unicode chars
        bUnicode = FALSE;
    } else
    {
        // Get the # chars
        numChars = (UINT) MyGlobalSize (hGlbClip) / sizeof (WCHAR);

        // Mark as Unicode chars
        bUnicode = TRUE;
    } // End IF/ELSE

    // Split off braces case
    if (uIndex EQ UNITRANS_BRACES)
    {
        // Lock the memory to get a ptr to it
        lpMemClip = GlobalLock (hGlbClip); Assert (lpMemClip NE NULL);

        if (bUnicode)
            // Make a copy of the clipboard data
            CopyMemoryW (lpwszGlbTemp, lpMemClip, numChars);
        else
        for (uText = 0; uText < numChars; uText++)
            lpwszGlbTemp[uText] = (WCHAR) ((LPUCHAR) lpMemClip)[uText];

        // We no longer need this ptr
        GlobalUnlock (hGlbClip); lpMemClip = NULL;

        // In case the last char is WC_EOS, ...
        while (numChars && lpwszGlbTemp[numChars - 1] EQ WC_EOS)
            numChars--;

        // Format the text as an ASCII string with non-ASCII chars
        //   represented as either {symbol} or {\xXXXX} where XXXX is
        //   a four-digit hex number.
        ConvertWideToNameLength (lpwszTemp,     // Ptr to output save buffer
                                 lpwszGlbTemp,  // Ptr to incoming chars
                                 numChars);     // # chars to convert
        // Get the new length in WCHARs
        numChars = lstrlenW (lpwszTemp);

        // Allocate space for that many Unicode chars
        // Note that we can't use MyGlobalAlloc or DbgGlobalAlloc
        //   because after we pass this handle to the clipboard
        //   we won't own it anymore.
        // "+ 1" for the trailing zero
        hGlbText = GlobalAlloc (GHND | GMEM_DDESHARE, (numChars + 1 ) * sizeof (WCHAR));
        if (hGlbText EQ NULL)
        {
            MessageBox (hWndMF,
                        "Unable to allocate memory for the copy of CF_UNICODETEXT format",
                        lpszAppName,
                        MB_OK | MB_ICONWARNING | MB_APPLMODAL);
            goto ERROR_EXIT;
        } // End IF

        // Lock the memory to get a ptr to it
        // Note we can't use MyGlobalLock/Unlock as the lock count
        //   is not modified for a clipboard (non-owned) handle
        lpMemText = GlobalLock (hGlbText); Assert (lpMemText NE NULL);

        // Copy the converted data
        CopyMemoryW (lpMemText, lpwszTemp, numChars);
    } else
    {
        // Allocate space for that many Unicode chars
        // Note that we can't use MyGlobalAlloc or DbgGlobalAlloc
        //   because after we pass this handle to the clipboard
        //   we won't own it anymore.
        hGlbText = GlobalAlloc (GHND | GMEM_DDESHARE, numChars * sizeof (WCHAR));
        if (hGlbText EQ NULL)
        {
            MessageBox (hWndMF,
                        "Unable to allocate memory for the copy of CF_UNICODETEXT format",
                        lpszAppName,
                        MB_OK | MB_ICONWARNING | MB_APPLMODAL);
            goto ERROR_EXIT;
        } // End IF

        // Lock the memory to get a ptr to it
        // Note we can't use MyGlobalLock/Unlock as the lock count
        //   is not modified for a clipboard (non-owned) handle
        lpMemClip = GlobalLock (hGlbClip); Assert (lpMemClip NE NULL);
        lpMemText = GlobalLock (hGlbText); Assert (lpMemText NE NULL);

        if (bUnicode)
            // Make a copy of the clipboard data
            CopyMemoryW (lpMemText, lpMemClip, numChars);
        else
        for (uText = 0; uText < numChars; uText++)
            lpMemText[uText] = (WCHAR) ((LPUCHAR) lpMemClip)[uText];

        // We no longer need this ptr
        GlobalUnlock (hGlbClip); lpMemClip = NULL;

        // Translate the NARS charset to the other APL charset
        for (uText = 0; uText < numChars; uText++, lpMemText++)
        if (*lpMemText)
        {
            for (uTran = 0; uTran < UNITRANS_NROWS; uTran++)
            if (*lpMemText EQ uniTransTab[uTran][UNITRANS_NARS])
            {
                WCHAR wcTmp;

                // Translate the external char to the given format
                wcTmp = uniTransTab[uTran][uIndex];

                // If the translated char is not the same as the original, ...
                if (wcTmp NE SAME)
                {
                    // If the translation is not from NARS and
                    //   the char is UTF16_A_ through UTF16_Z_,
                    //   optionally translate it to 'a-z'
                    if (OptionFlags.bUnderbarToLowercase
                     && uTran NE UNITRANS_NORMAL
                     && UTF16_A_ <= wcTmp
                     &&             wcTmp <= UTF16_Z_)
                        wcTmp = L'a' + (wcTmp - UTF16_A_);
                    *lpMemText = wcTmp;
                } // End IF

                break;      // out of the innermost FOR loop
            } // End FOR/IF/...
        } // End FOR/IF
    } // End IF/ELSE

    // We no longer need this ptr
    GlobalUnlock (hGlbText); lpMemText = NULL;

    // Empty the clipboard
    EmptyClipboard (); hGlbClip = NULL;

    // Place the changed data onto the clipboard
    SetClipboardData (CF_UNICODETEXT, hGlbText); hGlbText = NULL;
ERROR_EXIT:
NORMAL_EXIT:
    // We're done with the clipboard and its handle
    CloseClipboard ();

    if (hGlbText NE NULL && lpMemText NE NULL)
    {
        // We no longer need this ptr
        GlobalUnlock (hGlbText); lpMemText = NULL;
    } // End IF

    if (hGlbClip NE NULL)
    {
        if (lpMemClip NE NULL)
        {
            // We no longer need this ptr
            GlobalUnlock (hGlbClip); lpMemClip = NULL;
        } // End IF

        // We're done with the clipboard
        CloseClipboard ();
    } // End IF

    EXIT_TEMP_OPEN
} // End CopyAPLChars_EM


//***************************************************************************
//  $PasteAPLChars_EM
//
//  Paste APL chars from another APL system
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PasteAPLChars_EM"
#else
#define APPEND_NAME
#endif

void PasteAPLChars_EM
    (HWND      hWndEC,              // Window handle of the Edit Ctrl
     UNI_TRANS uIndex)              // UNI_TRANS index

{
    SIZE_T     dwSize;
    HGLOBAL    hGlbFmts = NULL,     // Clipboard formats global memory handle
               hGlbClip = NULL,
               hGlbText = NULL;
    LPCLIPFMTS lpMemFmts = NULL;    // Ptr to clipboard format global memory
    LPWCHAR    lpMemClip = NULL,    // Ptr to clipboard global memory
               lpMemText = NULL;    // Ptr to local copy global memory
    UINT       uTran,               // Loop counter
               uText,               // Loop counter
               uFmt,                // Loop counter
               uFmtNum,             // Clipbaord format #
               uCount;              // # formats on the clipboard

    // Open the clipboard so we can read from it
    OpenClipboard (hWndEC);

    // Get the # formats on the clipboard
    uCount = CountClipboardFormats ();

    // If there are no formats, ...
    if (uCount EQ 0)
        goto NORMAL_EXIT;

    // Allocate memory to hold the format # and the matching handle
    hGlbFmts = DbgGlobalAlloc (GHND, uCount * sizeof (CLIPFMTS));
    if (hGlbFmts EQ NULL)
    {
        MessageBox (hWndMF,
                    "Unable to allocate memory for the clipboard formats",
                    lpszAppName,
                    MB_OK | MB_ICONWARNING | MB_APPLMODAL);
        goto ERROR_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemFmts = MyGlobalLock000 (hGlbFmts);

    // Enumerate the clipboard formats and save the format # and handle
    for (uFmtNum = uFmt = 0; uFmt < uCount; uFmt++)
    {
        uFmtNum = EnumClipboardFormats (uFmtNum);
        if (uFmtNum EQ CF_TEXT
         || uFmtNum EQ CF_OEMTEXT
         || uFmtNum EQ CF_UNICODETEXT
         || uFmtNum EQ CF_DSPTEXT)
        {
            // Save the format #
            lpMemFmts[uFmt].uFmtNum = uFmtNum;

            // Get a handle to the clipboard data
            //   and make a copy of the data
            lpMemFmts[uFmt].hGlbFmt =
              CopyGlbMemory (GetClipboardData (uFmtNum), FALSE);
        } // End IF
    } // End FOR

    // Get a handle to the clipboard data for CF_PRIVATEFIRST
    hGlbClip = GetClipboardData (CF_PRIVATEFIRST);
    if (hGlbClip EQ NULL)
        // Get a handle to the clipboard data for CF_UNICODETEXT
        hGlbClip = GetClipboardData (CF_UNICODETEXT);

    if (hGlbClip NE NULL)
    {
        // Get the clipboard memory size
        dwSize = MyGlobalSize (hGlbClip);

        // Split off braces case
        if (uIndex EQ UNITRANS_BRACES)
        {
            // Lock the memory to get a ptr to it
            // Note we can't use MyGlobalLock/Unlock as the lock count
            //   is not modified for a clipboard (non-owned) handle
            lpMemClip = GlobalLock (hGlbClip); Assert (lpMemClip NE NULL);

            // Convert the {name}s and other chars to UTF16_xxx
            (void) ConvertNameInPlace (lpMemClip);

            // Get the size in bytes
            // "+ 1" for the trailing zero
            dwSize = (lstrlenW (lpMemClip) + 1) * sizeof (WCHAR);

            // Allocate space for the new object
            // Note that we can't use MyGlobalAlloc or DbgGlobalAlloc
            //   because after we pass this handle to the clipboard
            //   we won't own it anymore.
            hGlbText = GlobalAlloc (GHND | GMEM_DDESHARE, dwSize);
            if (hGlbText EQ NULL)
            {
                MessageBox (hWndMF,
                            "Unable to allocate memory for the copy of CF_UNICODETEXT/CF_PRIVATEFIRST format",
                            lpszAppName,
                            MB_OK | MB_ICONWARNING | MB_APPLMODAL);
                goto ERROR_EXIT;
            } // End IF

            // Lock the memory to get a ptr to it
            lpMemText = GlobalLock (hGlbText); Assert (lpMemText NE NULL);

            // Make a copy of the clipboard data
            CopyMemory (lpMemText, lpMemClip, dwSize);

            // We no longer need this ptr
            GlobalUnlock (hGlbClip); lpMemClip = NULL;
        } else
        {
            UINT uLen;

            // Allocate space for the new object
            // Note that we can't use MyGlobalAlloc or DbgGlobalAlloc
            //   because after we pass this handle to the clipboard
            //   we won't own it anymore.
            hGlbText = GlobalAlloc (GHND | GMEM_DDESHARE, dwSize);
            if (hGlbText EQ NULL)
            {
                MessageBox (hWndMF,
                            "Unable to allocate memory for the copy of CF_UNICODETEXT/CF_PRIVATEFIRST format",
                            lpszAppName,
                            MB_OK | MB_ICONWARNING | MB_APPLMODAL);
                goto ERROR_EXIT;
            } // End IF

            // Lock the memory to get a ptr to it
            // Note we can't use MyGlobalLock/Unlock as the lock count
            //   is not modified for a clipboard (non-owned) handle
            lpMemClip = GlobalLock (hGlbClip); Assert (lpMemClip NE NULL);
            lpMemText = GlobalLock (hGlbText); Assert (lpMemText NE NULL);

            // Make a copy of the clipboard data
            CopyMemory (lpMemText, lpMemClip, dwSize);

            // We no longer need this ptr
            GlobalUnlock (hGlbClip); lpMemClip = NULL;

            // Get the string length
            uLen = lstrlenW (lpMemText);

            // If the parent of the Edit Ctrl is SM and the last character
            //   in the string is CR/LF, delete it so the cursor appears
            //   on the last line.  The user can then press Enter to
            //   execute the expression.
            if (IzitSM (GetParent (hWndEC)))
            while (uLen
                && (lpMemText[uLen - 1] EQ WC_LF
                 || lpMemText[uLen - 1] EQ WC_CR))
            {
                // Back off by one character
                uLen--;

                // Delete the CR/LF
                lpMemText[uLen] = WC_EOS;
            } // End IF/WHILE

            // Translate the other APL charset to the NARS charset
            for (uText = 0; uText < uLen; uText++, lpMemText++)
            if (*lpMemText)
            {
                for (uTran = 0; uTran < UNITRANS_NROWS; uTran++)
                if (*lpMemText EQ uniTransTab[uTran][uIndex])
                {
                    WCHAR wcTmp;

                    // Translate the external char to NARS
                    wcTmp = uniTransTab[uTran][UNITRANS_NARS];

                    // If the translation is not from NARS and
                    //   the char is UTF16_A_ through UTF16_Z_,
                    //   optionally translate it to 'a-z'
                    if (OptionFlags.bUnderbarToLowercase
                     && uTran NE UNITRANS_NORMAL
                     && UTF16_A_ <= wcTmp
                     &&             wcTmp <= UTF16_Z_)
                        wcTmp = L'a' + (wcTmp - UTF16_A_);
                    *lpMemText = wcTmp;

                    break;      // out of the innermost FOR loop
                } // End FOR/IF/...
            } // End FOR/IF
        } // End IF/ELSE

        // We no longer need this ptr
        GlobalUnlock (hGlbText); lpMemText = NULL;

        // Empty the clipboard
        EmptyClipboard (); hGlbClip = NULL;

        // Place all the other formats on the clipboard first
        for (uFmt = 0; uFmt < uCount; uFmt++)
        if (lpMemFmts[uFmt].uFmtNum NE CF_PRIVATEFIRST
         && lpMemFmts[uFmt].hGlbFmt NE NULL)
            SetClipboardData (lpMemFmts[uFmt].uFmtNum, lpMemFmts[uFmt].hGlbFmt);

        // Place the changed data onto the clipboard
        SetClipboardData (CF_PRIVATEFIRST, hGlbText); hGlbText = NULL;
    } // End IF

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbFmts, lpMemFmts);
ERROR_EXIT:
NORMAL_EXIT:
    // We're done with the clipboard and its handle
    CloseClipboard (); hGlbClip = NULL;

    if (hGlbText NE NULL && lpMemText NE NULL)
    {
        // We no longer need this ptr
        GlobalUnlock (hGlbText); lpMemText = NULL;
    } // End IF

    if (hGlbClip NE NULL)
    {
        if (lpMemClip NE NULL)
        {
            // We no longer need this ptr
            GlobalUnlock (hGlbClip); lpMemClip = NULL;
        } // End IF

        // We're done with the clipboard
        CloseClipboard ();
    } // End IF

    if (hGlbFmts NE NULL)
    {
        // Loop through the formats freeing them as appropriate
        for (uFmt = 0; uFmt < uCount; uFmt++)
        switch (lpMemFmts[uFmt].uFmtNum)
        {
            case 0:             // Ignore empty slots
                break;

            case CF_DSPENHMETAFILE:
            case CF_DSPMETAFILEPICT:
            case CF_ENHMETAFILE:
            case CF_METAFILEPICT:
                DeleteMetaFile (lpMemFmts[uFmt].hGlbFmt); lpMemFmts[uFmt].hGlbFmt = NULL;

                break;

            case CF_BITMAP:
            case CF_DSPBITMAP:
            case CF_PALETTE:
                MyDeleteObject (lpMemFmts[uFmt].hGlbFmt); lpMemFmts[uFmt].hGlbFmt = NULL;

                break;

            case CF_DIB:
            case CF_DSPTEXT:
            case CF_OEMTEXT:
            case CF_TEXT:
            case CF_UNICODETEXT:
            default:
                GlobalFree (lpMemFmts[uFmt].hGlbFmt);

                break;
        } // End FOR/SWITCH

        // Unlock and free (and set to NULL) a global name and ptr
        UnlFreeGlbName (hGlbFmts, lpMemFmts);
    } // End IF
} // End PasteAPLChars_EM
#undef  APPEND_NAME


//***************************************************************************
//  $IzitEOB
//
//  Return TRUE iff the caret is at the End-of-Buffer
//***************************************************************************

UBOOL IzitEOB
    (HWND hWnd)         // Window handle of the Edit Ctrl

{
    UINT uCharPos,
         uLineCnt,
         uLinePos,
         uLineLen;

    // Get the char position of the caret (origin-0)
    uCharPos = GetCurCharPos (hWnd);

    // Get the # lines in the text
    uLineCnt = (UINT) SendMessageW (hWnd, EM_GETLINECOUNT, 0, 0);

    // Get the initial char pos of the last line (origin-0)
    uLinePos = (UINT) SendMessageW (hWnd, EM_LINEINDEX, uLineCnt - 1, 0);

    // Get the length of the last line
    uLineLen = (UINT) SendMessageW (hWnd, EM_LINELENGTH, uLinePos, 0);

    return (uCharPos >= (uLinePos + uLineLen));
} // End IzitEOB


//***************************************************************************
//  $GetCurCharPos
//
//  Return the character position of the caret
//***************************************************************************

UINT GetCurCharPos
    (HWND hWndEC)       // Window handle of the Edit Ctrl

{
    UINT uCharPosBeg,
         uCharPosEnd;

    // Get the indices of the selected text (if any)
    SendMessageW (hWndEC, EM_GETSEL, (WPARAM) &uCharPosBeg, (LPARAM) &uCharPosEnd);

    return uCharPosEnd;
} // End GetCurCharPos


//***************************************************************************
//  $GetCharValue
//
//  Return the character under the caret or a given position
//***************************************************************************

WCHAR GetCharValue
    (HWND     hWndEC,               // Window handle of the Edit Ctrl
     UINT     uCharPos)             // Char position (-1 = under the caret)

{
    POINT    ptCaret;               // The caret position
    UINT     uLineNum,              // The line #
             uLinePos,              // The line position (start of line)
             uLineLen,              // The line length
             uLineOff;              // The line offset

    if (uCharPos EQ -1)
    {
        // Get the caret position in client coords
        GetCaretPos (&ptCaret);

        // Get the position of the char under the caret
        //   and the line #
        CharFromPos (hWndEC, SendMessageW (hWndEC, EM_CHARFROMPOS, 0, MAKELPARAM (ptCaret.x, ptCaret.y)), &uLineNum, &uCharPos);
    } else
        // Get the line # of the char position
        uLineNum = (DWORD) SendMessageW (hWndEC, EM_LINEFROMCHAR, uCharPos, 0);

    // Get the char position of the start of the current line
    uLinePos = (DWORD) SendMessageW (hWndEC, EM_LINEINDEX, uLineNum, 0);

    // Get the length of the line
    uLineLen = (UINT) SendMessageW (hWndEC, EM_LINELENGTH, uLinePos, 0);

    // If the char position is at the end of the line, return WC_CR;
    //   if it's beyond the end of the line, return WC_LF
    uLineOff = uCharPos - uLinePos;
    if (uLineOff >= uLineLen)
        return (uLineOff EQ uLineLen) ? WC_CR : WC_LF;

    // Tell EM_GETLINE maximum # chars in the buffer
    // The output array is a temporary so we don't have to
    //   worry about overwriting outside the allocated buffer
    ((LPWORD) lpwszGlbTemp)[0] = (WORD) uLineLen;

    // Get the current line
    SendMessageW (hWndEC, EM_GETLINE, uLineNum, (LPARAM) lpwszGlbTemp);

    // Finally, return the char under the caret
    return lpwszGlbTemp[uCharPos - uLinePos];
} // End GetCharValue


//***************************************************************************
//  $AppendUndo
//
//  Append a new Undo entry
//***************************************************************************

void AppendUndo
    (HWND      hWnd,            // SM/FE Window handle
     UINT      GWLxx_UNDO_NXT,  // Offset in hWnd extra bytes of lpUndoNxt
     UNDO_ACTS Action,          // Action to take
     UINT      CharPosBeg,      // Beginning character position, -1 = caret
     UINT      CharPosEnd,      // Ending    ...
     UINT      Group,           // Group index, 0 = no group
     WCHAR     Char)            // Character, 0 = none

{
    LPUNDO_BUF lpUndoNxt;   // Ptr to next available slot in the Undo Buffer

    Assert (IzitSM (hWnd) || IzitFE (hWnd));

    // Get the ptr to the next available slot in our Undo Buffer
    lpUndoNxt = (LPUNDO_BUF) GetWindowLongPtrW (hWnd, GWLxx_UNDO_NXT);

RESTART_UNDO:

    __try
    {
        // Save the undo entry
        lpUndoNxt->Action     = Action;
        lpUndoNxt->CharPosBeg = (UINT) CharPosBeg;
        lpUndoNxt->CharPosEnd = (UINT) CharPosEnd;
        lpUndoNxt->Group      = (UINT) Group;
        lpUndoNxt->Char       = Char;
    } __except (CheckVirtAlloc (GetExceptionInformation (),
                                L"AppendUndo"))
    {
        // Split cases based upon the exception code
        switch (MyGetExceptionCode ())
        {
            LPUNDO_BUF lpUndoBeg;           // Ptr to start of Undo Buffer

            case EXCEPTION_LIMIT_ERROR:
                lpUndoBeg = (LPUNDO_BUF) GetWindowLongPtrW (hWnd, GWLSF_UNDO_BEG);

                // Delete the first entry
                MoveMemory (lpUndoBeg, &lpUndoBeg[1], (lpUndoNxt - lpUndoBeg - 1) * sizeof (UNDO_BUF));

                goto RESTART_UNDO;

            default:
                // Display message for unhandled exception
                DisplayException ();

                break;
        } // End SWITCH
    } // End __try/__except

    // Skip over this entry
    lpUndoNxt++;

    // Save the incremented ptr in window extra bytes
    SetWindowLongPtrW (hWnd, GWLSF_UNDO_NXT, (APLU3264) (LONG_PTR) lpUndoNxt);
    SetWindowLongPtrW (hWnd, GWLSF_UNDO_LST, (APLU3264) (LONG_PTR) lpUndoNxt);
} // End AppendUndo


//***************************************************************************
//  $InsRepCharStr
//
//  Insert or replace a char string in an Edit Ctrl
//***************************************************************************

void InsRepCharStr
    (HWND    hWnd,          // EC window handle
     LPWCHAR lpwch,         // The incoming char string
     UBOOL   bParentMF)     // TRUE iff the window's parent is MF

{
    VKSTATE vkState;        // Virtual key state (Shift, Alt, Ctrl)
    HWND    hWndParent;     // Handle of parent (SM/FE) window
    UINT    uCharPosBeg,
            uCharPosEnd,
            uCharPos,
            uLineLen,
            uLinePos,
            uGroupIndex,    // Group index
            uStrLen;        // Incoming string length
    UBOOL   bSelection;     // TRUE iff there is a selection

    // Ignore invalid window handles
    //   (e.g., click in Debug Window and then Language Bar
    //    with no valid active EC)
    if (hWnd EQ NULL)
        return;

    Assert (IzitEC (hWnd));

    // Get the handle of the parent window (SM/FE)
    hWndParent = GetParent (hWnd);

    // Get the current vkState
    vkState = GetVkState (hWnd);

    // Get the indices of the selected text (if any)
    SendMessageW (hWnd, EM_GETSEL, (WPARAM) &uCharPosBeg, (LPARAM) &uCharPosEnd);

    // Note if there's a selection
    bSelection = uCharPosBeg NE uCharPosEnd;

    // Get the incoming string length
    uStrLen = lstrlenW (lpwch);

    // If there's no selection, and we're in Replace mode,
    //   set the selection to the current character
    if (!bSelection && !vkState.Ins)
    {
        // Get the length of the line with the given char position
        uLineLen = (UINT) SendMessageW (hWnd, EM_LINELENGTH, uCharPosBeg, 0);

        // Get the char position of the start of the current line
        uLinePos = (UINT) SendMessageW (hWnd, EM_LINEINDEX, (WPARAM) -1, 0);

        // Get the next group index, and save it back
        uGroupIndex = 1 + GetWindowLongW (hWndParent, GWLSF_UNDO_GRP);
        SetWindowLongW (hWndParent, GWLSF_UNDO_GRP, uGroupIndex);

        // Calculate the ending position of the replacement
        uCharPosEnd = uCharPosBeg + uStrLen;

        // Set the selection to a corresponding length char string so we replace
        SendMessageW (hWnd, EM_SETSEL, uCharPosBeg, min (uCharPosEnd, uLineLen + uLinePos));

        // If our parent is not MF, ...
        if (!bParentMF)
        {
            // Loop through the incoming char string
            for (; uCharPosBeg < uCharPosEnd; uCharPosBeg++)
            // If we're at the end of the line, insert rather than replace
            if (uCharPosBeg >= (uLineLen + uLinePos))
            {
                // Insert a char

                // Undo deletes the inserted char
                AppendUndo (hWndParent,                         // SM/FE Window handle
                            GWLSF_UNDO_NXT,                     // Offset in hWnd extra bytes of lpUndoNxt
                            undoDel,                            // Action
                            uCharPosBeg,                        // Beginning char position
                            uCharPosBeg + 1,                    // Ending    ...
                            uGroupIndex,                        // Group index
                            0);                                 // Character
            } else
            {
                // Replace a char string

                // Undo replaces it with the existing char
                AppendUndo (hWndParent,                         // SM/FE Window handle
                            GWLSF_UNDO_NXT,                     // Offset in hWnd extra bytes of lpUndoNxt
                            undoRep,                            // Action
                            uCharPosBeg,                        // Beginning char position
                            0,                                  // Ending    ...
                            uGroupIndex,                        // Group index
                            GetCharValue (hWnd, uCharPosBeg));  // Outgoing char
            } // End FOR/IF/ELSE
        } // End IF
    } else
    {
        // If our parent is not MF, ...
        if (!bParentMF)
        {
            // Insert a char string, deleting selected chars (if any)

            // Undo inserts the deleted (selected) chars (if any)

            // Get the next group index, and save it back
            uGroupIndex = 1 + GetWindowLongW (hWndParent, GWLSF_UNDO_GRP);
            SetWindowLongW (hWndParent, GWLSF_UNDO_GRP, uGroupIndex);

            // Append an Undo action to set the selection if there was one before
            if (bSelection)
                AppendUndo (hWndParent,                     // SM/FE Window handle
                            GWLSF_UNDO_NXT,                 // Offset in hWnd extra bytes of lpUndoNxt
                            undoSel,                        // Action
                            uCharPosBeg,                    // Beginning char position
                            uCharPosEnd,                    // Ending    ...
                            uGroupIndex,                    // Group index
                            0);                             // Character
            // Loop through the selected chars (if any)
            for (uCharPos = uCharPosBeg; uCharPos < uCharPosEnd; uCharPos++)
                AppendUndo (hWndParent,                     // SM/FE Window handle
                            GWLSF_UNDO_NXT,                 // Offset in hWnd extra bytes of lpUndoNxt
                            undoIns,                        // Action
                            uCharPosBeg,                    // Beginning char position
                            0,                              // Ending    ...
                            uGroupIndex,                    // Group index
                            GetCharValue (hWnd, uCharPos)); // Character
            // Undo deletes the inserted char string
                AppendUndo (hWndParent,                     // SM/FE Window handle
                            GWLSF_UNDO_NXT,                 // Offset in hWnd extra bytes of lpUndoNxt
                            undoDel,                        // Action
                            uCharPosBeg,                    // Beginning char position
                            uCharPosBeg + uStrLen,          // Ending    ...
                            uGroupIndex,                    // Group index
                            0);                             // Character
        } // End IF
    } // End IF/ELSE

    // Insert/replace the char string into the text
    ReplaceSel (hWnd, (WPARAM) FALSE, (LPARAM) lpwch);

    // If our parent is not MF, ...
    if (!bParentMF)
        // Tell the Status Window about the new positions
        SetStatusPos (hWnd);
} // End InsRepCharStr


//***************************************************************************
//  $IzitFE
//
//  Is the window FEWNDCLASS?
//***************************************************************************

UBOOL IzitFE
    (HWND hWnd)

{
    WCHAR wszClassName[32];

    GetClassNameW (hWnd, wszClassName, strcountof (wszClassName));

    return (lstrcmpW (wszClassName, LFEWNDCLASS) EQ 0);
} // End IzitFE


//***************************************************************************
//  $IzitSM
//
//  Is the window SMWNDCLASS?
//***************************************************************************

UBOOL IzitSM
    (HWND hWnd)

{
    WCHAR wszClassName[32];

    GetClassNameW (hWnd, wszClassName, strcountof (wszClassName));

    return (lstrcmpW (wszClassName, LSMWNDCLASS) EQ 0);
} // End IzitSM


//***************************************************************************
//  $IzitMC
//
//  Is the window WC_MDICLIENTW?
//***************************************************************************

UBOOL IzitMC
    (HWND hWnd)

{
    WCHAR wszClassName[32];

    GetClassNameW (hWnd, wszClassName, strcountof (wszClassName));

    return (lstrcmpW (wszClassName, WC_MDICLIENTW) EQ 0);
} // End IzitMC


//***************************************************************************
//  $IzitEC
//
//  Is the window ECWNDCLASS?
//***************************************************************************

UBOOL IzitEC
    (HWND hWnd)

{
    WCHAR wszClassName[32];

    GetClassNameW (hWnd, wszClassName, strcountof (wszClassName));

    return (lstrcmpW (wszClassName, LECWNDCLASS) EQ 0);
} // End IzitEC


//***************************************************************************
//  $IzitDialog
//
//  Is the window Dialog Class?
//***************************************************************************

UBOOL IzitDialog
    (HWND hWnd)

{
    WCHAR wszClassName[32];

    GetClassNameW (hWnd, wszClassName, strcountof (wszClassName));

    return (lstrcmpW (wszClassName, WC_DIALOGW) EQ 0);
} // End IzitDialog


//***************************************************************************
//  $DrawLineNumsFE
//
//  Draw the line #s in an FE window
//***************************************************************************

void DrawLineNumsFE
    (HWND hWndEC)           // Edit Ctrl window handle

{
    HWND    hWndParent;     // Window handle of the parent (i.e. hWndFE)
    HDC     hDC,            // Device context
            hDCMem;         // Handle to memory device context
    HBITMAP hBitmap,        // Handle to compatible bitmap
            hBitmapOld;     // Handle to memory DC old bitmap
    HFONT   hFontOld;       // Handle to old font
    RECT    rcPaint,        // Paint rectangle
            rcItem,         // Rectangle for use when double buffering
            rcClient;       // Client area
    UINT    uLen,           // Length of string
            uLineCnt,       // # lines in the Edit Ctrl
            uLineTop,       // # of topmost visible line
            uCnt,           // Counter
            uCntLC,         // Line Continuation counter
            uLineNum,       // Line #
            line_height;    // The line height
    WCHAR   wszLineNum[FCN_INDENT + 1];  // Line # (e.g. L"[0000]\0"
    HBRUSH  hBrush;         // Brush for background color

    // Get the handle to the parent window (hWndFE)
    hWndParent = GetParent (hWndEC);

    // Ensure this is the function editor
    if (!IzitFE (hWndParent))
        return;

    // If we're not displaying function line #s, ...
    if (!GetWindowLongW (hWndParent, GWLSF_FLN))
        return;

    // Get the client rectangle
    GetClientRect (hWndEC, &rcClient);

    // Set the line number rectangle using the EC left margin as our right side
    rcItem = rcClient;

    // Do not include the width for the Line Continuation markers as they are drawn separately
    rcItem.right  = LftMarginsFE (hWndEC, FALSE);

    // Get a device context
    hDC = MyGetDC (hWndEC);

    // Create a compatible memory DC and bitmap
    hDCMem  = MyCreateCompatibleDC     (hDC);
    hBitmap = MyCreateCompatibleBitmap (hDC,
                                        rcItem.right,
                                        rcItem.bottom);
    hBitmapOld = SelectObject (hDCMem, hBitmap);
    hFontOld   = SelectObject (hDCMem, GetCurrentObject (hDC, OBJ_FONT));

    // Set our DC attributes
    SetAttrs (hDCMem,
              GetFSIndFontHandle (FONTENUM_FE),
              gSyntaxColorName[SC_FCNLINENUMS].syntClr.crFore,
              gSyntaxColorName[SC_FCNLINENUMS].syntClr.crBack);
    // Create a brush for the function line numbers background
    hBrush = MyCreateSolidBrush (gSyntaxColorName[SC_FCNLINENUMS].syntClr.crBack);

    // Fill in the background
    FillRect (hDCMem, &rcItem, hBrush);

    // We no longer need this resource
    MyDeleteObject (hBrush); hBrush = NULL;

    // Get the # lines in the text
    uLineCnt = (UINT) SendMessageW (hWndEC, EM_GETLINECOUNT, 0, 0);

    // Get the # of the topmost visible line
    uLineTop = (UINT) SendMessageW (hWndEC, EM_GETFIRSTVISIBLELINE, 0, 0);

    // Get the line height
    line_height = (UINT) SendMessageW (hWndEC, MYWM_LINE_HEIGHT, 0, 0);

    // Less the top index
    uLineCnt -= uLineTop;

    // Loop through the line #s
    for (uCnt = uCntLC = 0; uCnt < uLineCnt; uCnt++)
    {
        // Calculate the current line #
        uLineNum = uCnt + uLineTop;

        // If the preceding physical line does not continue to the current line, ...
        if (uLineNum EQ 0
         || SendMessageW (hWndEC, MYEM_ISLINECONT, uLineNum - 1, 0) EQ FALSE)
        {
            // Format the line #
            MySprintfW (wszLineNum,
                        sizeof (wszLineNum),
                       L"[%d]",
                        uLineNum - uCntLC);
            uLen = lstrlenW (wszLineNum);
        } else
        {
            uLen = 0;
            uCntLC++;
        } // End IF/ELSE

        // Pad out to FCN_INDENT chars
        for (; uLen < FCN_INDENT; uLen++)
            wszLineNum[uLen] = L' ';
        wszLineNum[uLen] = WC_EOS;

        // Calculate the rectangle size of the line #s
        SetRectEmpty (&rcPaint);
        DrawTextW (hDCMem,
                   wszLineNum,
                   uLen,
                  &rcPaint,
                   0
                 | DT_CALCRECT
                 | DT_NOPREFIX);
        // Move the rectangle down
        rcPaint.top    += uCnt * line_height;
        rcPaint.bottom += uCnt * line_height;

        // Draw the line #s
        DrawTextW (hDCMem,
                   wszLineNum,
                   uLen,
                  &rcPaint,
                   0
                 | DT_LEFT
                 | DT_NOPREFIX);
    } // End FOR

    // Because we might have deleted some lines whose
    //   #s we need to clear, and we have disabled the
    //   WM_ERASEBKGND message (so as to reduce screen flicker),
    //   we need to fill the bottom of the page with blanks

    // Set to the same top as the next line #
    rcClient.top = rcPaint.top + GetFSIndAveCharSize (FONTENUM_FE)->cy;

    // Create a brush for the function line numbers background
    hBrush = MyCreateSolidBrush (gSyntaxColorName[SC_FCNLINENUMS].syntClr.crBack);

    // Pour on the white out
    FillRect (hDCMem, &rcClient, hBrush);

    // We no longer need this resource
    MyDeleteObject (hBrush); hBrush = NULL;

    // Copy the memory DC to the screen DC
    BitBlt (hDC,
            rcItem.left,
            rcItem.top,
            rcItem.right,
            rcItem.bottom,
            hDCMem,
            rcItem.left,
            rcItem.top,
            SRCCOPY);
    // Restore the old resources
    SelectObject (hDCMem, hBitmapOld);
    SelectObject (hDCMem, hFontOld);

    // We no longer need these resources
    MyDeleteObject (hBitmap); hBitmap = NULL;
    MyDeleteDC (hDCMem); hDCMem = NULL;

    // We no longer need this DC
    MyReleaseDC (hWndEC, hDC); hDC = NULL;
} // End DrawLineNumsFE


//***************************************************************************
//  $QueryCloseFE
//
//  Return a Boolean result on whether we can close
//***************************************************************************

UBOOL QueryCloseFE
    (HWND hWndFE)           // Handle for FE window

{
    // If the text hasn't changed, continue with close
    if (!GetWindowLongW (hWndFE, GWLSF_CHANGED))
        return TRUE;

    // Ask the user what to do
    switch (MessageBox (hWndMF, szCloseMessage, lpszAppName, MB_YESNOCANCEL | MB_ICONQUESTION))
    {
        case IDYES:         // Save the function
            return SaveFunction (hWndFE);

        case IDNO:          // Do not save the function
            return TRUE;

        case IDCANCEL:      // Not OK to close
        default:            // Problem with MessageBox
            return FALSE;
    } // End SWITCH
} // End QueryClose


//***************************************************************************
//  $ErrorHandler
//
//  Error handler for calls to Tokenize_EM when parsing
//    a function header and its lines.
//***************************************************************************

void ErrorHandler
    (LPWCHAR lpwszMsg,          // Ptr to error message text
     LPWCHAR lpwszLine,         // Ptr to the line which generated the error
     UINT    uCaret)            // Position of caret (origin-0)

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Save in global for later reference
    lpMemPTD->lpwszErrorMessage = lpwszMsg;
    lpMemPTD->uCaret = uCaret;
} // End ErrorHandler


//***************************************************************************
//  $ParseFunctionName
//
//  Parse a function header and return the LPSYMENTRY
//    of the function name (if any)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ParseFunctionName"
#else
#define APPEND_NAME
#endif

LPSYMENTRY ParseFunctionName
    (HWND      hWndFE,                  // Window handle to Function Editor
     LPAPLCHAR lpaplChar)               // Ptr to header text

{
    HWND        hWndEC;                 // Window handle to Edit Ctrl
    HGLOBAL     hGlbTknHdr = NULL;      // Tokenized header global memory handle
    FHLOCALVARS fhLocalVars = {0};      // Function Header vars
    LPSYMENTRY  lpSymName = NULL;       // Ptr to SYMENTRY for the function name
    MEMVIRTSTR  lclMemVirtStr[1] = {0}; // Room for one GuardAlloc

    Assert (IzitFE (hWndFE));

    // If it's []Z, ...
    if (lstrcmpiW ($AFORESULT, lpaplChar) EQ 0)
        // Handle specially as Tokenize_EM treats []Z as no value
        //   which causes ParseFcnHeader to crash
        return lpSymName;

    // Get the handle to the Edit Ctrl
    hWndEC = (HWND) GetWindowLongPtrW (hWndFE, GWLSF_HWNDEC);

    // Tokenize the line
    hGlbTknHdr =
      Tokenize_EM (lpaplChar,               // The line to tokenize
                   lstrlenW (lpaplChar),    // The length of the above line
                   NULL,                    // Window handle for Edit Ctrl (may be NULL if lpErrHandFn is NULL)
                   0,                       // Function line # (0 = header)
                   NULL,                    // Ptr to error handling function (may be NULL)
                   NULL,                    // Ptr to common struc (may be NULL if unused)
                   FALSE);                  // TRUE iff we're tokenizing a Magic Function/Operator
    if (hGlbTknHdr EQ NULL)
        goto ERROR_EXIT;

    // Allocate virtual memory for the Variable Strand accumulator
    lclMemVirtStr[0].lpText   = "fhLocalvars.lpYYStrandStart in <ParseFunctionName>";
    lclMemVirtStr[0].IncrSize = DEF_STRAND_INCRNELM * sizeof (PL_YYSTYPE);
    lclMemVirtStr[0].MaxSize  = DEF_STRAND_MAXNELM  * sizeof (PL_YYSTYPE);
    lclMemVirtStr[0].IniAddr  = (LPUCHAR)
    fhLocalVars.lpYYStrandStart =
      GuardAlloc (NULL,       // Any address
                  lclMemVirtStr[0].MaxSize,
                  MEM_RESERVE,
                  PAGE_READWRITE);
    if (!lclMemVirtStr[0].IniAddr)
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsgW (L"ParseFunctionName:  VirtualAlloc for <fhLocalVars.lpYYStrandStart> failed");

        goto ERROR_EXIT;        // Mark as failed
    } // End IF

    // Link this struc into the chain
    LinkMVS (&lclMemVirtStr[0]);

    // Commit the intial size
    MyVirtualAlloc (lclMemVirtStr[0].IniAddr,
                    DEF_STRAND_INITNELM * sizeof (PL_YYSTYPE),
                    MEM_COMMIT,
                    PAGE_READWRITE);
    // Mark as parsing the function name (generate fewer errors)
    fhLocalVars.ParseFcnName = TRUE;

    // Parse the function header
    if (ParseFcnHeader (hWndEC, hGlbTknHdr, &fhLocalVars, FALSE)
     && fhLocalVars.lpYYFcnName)
        // Get the Name's symbol table entry
        lpSymName = fhLocalVars.lpYYFcnName->tkToken.tkData.tkSym;
    goto NORMAL_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    // If we allocated virtual storage, ...
    if (lclMemVirtStr[0].IniAddr)
    {
        // Free the virtual storage
        MyVirtualFree (lclMemVirtStr[0].IniAddr, 0, MEM_RELEASE); lclMemVirtStr[0].IniAddr = NULL;
        fhLocalVars.lpYYStrandStart = NULL;

        // Unlink this entry from the chain
        UnlinkMVS (&lclMemVirtStr[0]);
    } // End IF

    if (hGlbTknHdr NE NULL)
    {
        LPTOKEN_HEADER lpMemTknHdr;

        // Lock the memory to get a ptr to it
        lpMemTknHdr = MyGlobalLockTkn (hGlbTknHdr);

        // Free the tokens
        Untokenize (lpMemTknHdr);

        // Unlock and free (and set to NULL) a global name and ptr
        UnlFreeGlbName (hGlbTknHdr, lpMemTknHdr);
    } // End IF

    return lpSymName;
} // End ParseFunctionName
#undef  APPEND_NAME


//// //***************************************************************************
//// //  $SaveAsFunction
//// //
//// //  Attempt to save a function to the WS using a different name
//// //***************************************************************************
////
//// UBOOL SaveAsFunction
////     (HWND hWndFE)
////
//// {
////     DbgBrk ();          // ***FINISHME*** -- Save As Function
////
////     Assert (IzitFE (hWndFE));
////
////
////
////
////
////
////
////     return FALSE;
//// } // End SaveAsFunction


//***************************************************************************
//  $CloseFunction
//
//  Attempt to close a function
//***************************************************************************

UBOOL CloseFunction
    (HWND hWndFE)               // The window handle of the function to close

{
    UBOOL bRet;                 // TRUE iff we're to close

    Assert (IzitFE (hWndFE));

    // If it's OK to close the window, do so, else ignore
    bRet = QueryCloseFE (hWndFE);
    if (bRet)
    {
        Assert (GetParent (hWndFE) EQ GetMemPTD ()->hWndMC);

        // Close the Function Editor window
        SendMessageW (GetParent (hWndFE), WM_MDIDESTROY, (WPARAM) hWndFE, 0);
    } // End IF

    return bRet;
} // End CloseFunction


//***************************************************************************
//  $ShowBand
//
//  Show/hide a Rebar band
//***************************************************************************

void ShowBand
    (UINT  uCnt,            // Index into aRebarBands
     UBOOL bShow)           // TRUE iff we're to show this band

{
    UINT uItem;                     // Index of this band

    // Convert the Window ID to an index
    uItem = (UINT)
      SendMessageW (hWndRB, RB_IDTOINDEX, aRebarBands[uCnt].uWindowID, 0);

    // Show/Hide the specified Toolbar
    SendMessageW (hWndRB, RB_SHOWBAND, uItem, bShow);
} // End ShowBand


//***************************************************************************
//  $SetToolbarsMenu
//
//  Set/reset the corresponding Toolbars menu checkmark and
//    show/hide the corresponding Rebar band
//***************************************************************************

void SetToolbarsMenu
    (HMENU hMenu,
     UINT  uCnt)

{
    // Check/uncheck the Toolbars menu item
    CheckMenuItem (hMenu,
                   IDM_TB_FIRST + uCnt,
                   MF_BYCOMMAND
                 | MF_ENABLED
                 | (aRebarBands[uCnt].bShowBand ? MF_CHECKED : MF_UNCHECKED)
                  );
    // Show/hide the band
    ShowBand (uCnt, aRebarBands[uCnt].bShowBand);
} // End SetToolbarsMenu


//***************************************************************************
//  $SetLineNumState
//
//  Set the state of the Toggle Function Line #s button
//***************************************************************************

void SetLineNumState
    (HWND hWndFE)               // Handle of FE window

{
    UINT uFlags;                // Current flags of the Toggle Fcn Line #s button
    HWND hWndEC;                // Handle of Edit Ctrl window

    Assert (IzitFE (hWndFE));

    // Get the current flags of the Object Toolbar's Toggle Fcn Line #s button
    uFlags = (UINT)
      SendMessageW (hWndOW_RB, TB_GETSTATE, IDM_TOGGLE_LNS_FCN, 0);

    // Set the flags of the Objects Toolbar's Toggle Fcn Line #s button
    if (GetWindowLongW (hWndFE, GWLSF_FLN))
        uFlags |=  TBSTATE_CHECKED;
    else
        uFlags &= ~TBSTATE_CHECKED;

    // Tell the Objects Toolbar about the new state
    SendMessageW (hWndOW_RB, TB_SETSTATE, IDM_TOGGLE_LNS_FCN, (LPARAM) uFlags);

    // Get the corresponding Edit Ctrl window handle
    (HANDLE_PTR) hWndEC = GetWindowLongPtrW (hWndFE, GWLSF_HWNDEC);

    // Set the margins for a Function Editor window
    SetMarginsFE (hWndEC);
} // End SetLineNumState


//***************************************************************************
//  $ActivateMDIMenu
//
//  Activate an MDI window menu
//***************************************************************************

void ActivateMDIMenu
    (WINDOWCLASS  wndClass,         // SM or FE window class
     HWND         hWnd)             // hWndSM or hWndFE

{
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    HMENU        hMenuFrame,        // SM or FE menu handle
                 hMenu;             // Window/View/toolbar/ ... menu handle
    GETIDMPOS_XX GetIDMPOS_xx;      // Ptr to IDMPOS_xxx function
    UINT         uCnt;              // Loop counter

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Split cases based upon the window class
    switch (wndClass)
    {
        case WINDOWCLASS_SM:
            hMenuFrame = hMenuSM;
            GetIDMPOS_xx = GetIDMPOS_SM;

            break;

        case WINDOWCLASS_FE:
            hMenuFrame = hMenuFE;
            GetIDMPOS_xx = GetIDMPOS_FE;

            // Set the current state of the Object Toolbar's Toggle Fcn Line #s button
            SetLineNumState (hWnd);

            break;

        defstop
            break;
    } // End SWITCH

    // Get the Window menu handle
    hMenu = GetSubMenu (hMenuFrame, (*GetIDMPOS_xx) (IDMPOSNAME_WINDOW));

    SendMessageW (lpMemPTD->hWndMC,
                  WM_MDISETMENU,
                  GET_WM_MDISETMENU_MPS (hMenuFrame, hMenu));
    DrawMenuBar (hWndMF);

    // Get the View menu handle
    hMenu = GetSubMenu (hMenuFrame, (*GetIDMPOS_xx) (IDMPOSNAME_VIEW));

    // Check/uncheck the View | Status Bar menu item as appropriate
    CheckMenuItem (hMenu, IDM_STATUSBAR, MF_BYCOMMAND | (OptionFlags.bViewStatusBar ? MF_CHECKED : MF_UNCHECKED));

    // Loop through the Toolbars
    for (uCnt = 0; uCnt < countof (aRebarBands); uCnt++)
    // Split cases based upon the window class
    switch (wndClass)
    {
        case WINDOWCLASS_SM:
            // If this Toolbar applies to SM, ...
            if (aRebarBands[uCnt].bApplyToSM)
                // Set/reset the corresponding Toolbars menu checkmark and
                //   show/hide the corresponding Rebar band
                SetToolbarsMenu (hMenu, uCnt);
            else
                // Hide the band
                ShowBand (uCnt, FALSE);
            break;

        case WINDOWCLASS_FE:
            // If this Toolbar applies to FE, ...
            if (aRebarBands[uCnt].bApplyToFE)
                // Set/reset the corresponding Toolbars menu checkmark and
                //   show/hide the corresponding Rebar band
                SetToolbarsMenu (hMenu, uCnt);
            else
                // Hide the band
                ShowBand (uCnt, FALSE);
            break;

        defstop
            break;
    } // End FOR/SWITCH
} // End ActivateMDIMenu


//***************************************************************************
//  End of File: editfcn.c
//***************************************************************************
