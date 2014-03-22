//***************************************************************************
//  NARS2000 -- My Routines For Debugging
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2013 Sudley Place Software

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
#include <stdio.h>
#include "headers.h"


CDB_THREAD cdbThread;               // Temporary global
WNDPROC    lpfnOldListboxWndProc;   // Save area for old Listbox window proc

#define BKCOLOR_REFCNT_NE1      DEF_SCN_RED
#define FGCOLOR_REFCNT_NE1      DEF_SCN_WHITE

#ifdef DEBUG
//***************************************************************************
//  $_CheckCtrlBreak
//
//  Check for Ctrl-Break, thus providing a place to put
//    a breakpoint to catch when it is first checked.
//***************************************************************************

UBOOL _CheckCtrlBreak
    (UBOOL bCtrlBreak)

{
    if (bCtrlBreak)
        return TRUE;
    else
        return FALSE;
} // End _CheckCtrlBreak
#endif


////#ifndef DEBUG
//////***************************************************************************
//////  $AssertPrint
//////***************************************************************************
////
////void AssertPrint
////    (LPCHAR lpExp,              // Ptr to expression string
////     LPCHAR lpFileName,         // Ptr to filename where allocated
////     UINT   uLineNum)           // Line # where allocated
////
////{
////    char szTemp[1024];
////
////    wsprintf (szTemp,
////              "ASSERTION FAILURE:  Expression <%s> failed in file <%s> line #%d, continuing anyway...",
////              lpExp,
////              lpFileName,
////              uLineNum);
////
////    MB (szTemp);
////} // End AssertPrint
////#endif


#ifdef DEBUG
//***************************************************************************
//  $CreateDebuggerWindow
//
//  Create a debugger window
//***************************************************************************

void CreateDebuggerWindow
    (LPPERTABDATA lpMemPTD)         // Ptr to PerTabData global memory

{
    DWORD dwThreadId;       // Thread ID

    // Pass parameters to thread
    cdbThread.lpMemPTD = lpMemPTD;

    // Create the thread
    CreateThread (NULL,                         // No security attrs
                  0,                            // Use default stack size
                 &CreateDebuggerInThread,       // Starting routine
                 &cdbThread,                    // Param to thread func
                  0,                            // Creation flag
                 &dwThreadId);                  // Returns thread id
} // End CreateDebuggerWindow
#endif


#ifdef DEBUG
//***************************************************************************
//  $CreateDebuggerInThread
//
//  Create debugger window in thread
//***************************************************************************

UBOOL WINAPI CreateDebuggerInThread
    (LPCDB_THREAD lpcdbThread)

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    HWND         hWndMC,        // MDI Client window handle
                 hWndDB;        // Debugger window handle
    MSG          Msg;           // Message for GetMessageW loop

    __try
    {
        // Save the thread type ('DB')
        TlsSetValue (dwTlsType, TLSTYPE_DB);

        // Extract values from the arg struc
        lpMemPTD = lpcdbThread->lpMemPTD;

        // Save ptr to PerTabData global memory
        TlsSetValue (dwTlsPerTabData, lpMemPTD);

        // Get the MDI Client window handle
        hWndMC = lpMemPTD->hWndMC;

        // Create the debugger window
        hWndDB =
          CreateMDIWindowW (LDBWNDCLASS,        // Class name
                            wszDBTitle,         // Window title
                            0
                          | WS_CLIPCHILDREN,    // Styles
                            CW_USEDEFAULT,      // X-pos
                            CW_USEDEFAULT,      // Y-pos
                            CW_USEDEFAULT,      // Height
                            CW_USEDEFAULT,      // Width
                            hWndMC,             // Parent
                            _hInstance,         // Instance
                            0);                 // Extra data
        // Save in PerTabData
        lpMemPTD->hWndDB = hWndDB;

        // If it didn't succeed, ...
        if (hWndDB EQ NULL)
            MB (pszNoCreateDBWnd);
        else
        {
            // Show and paint the window
            ShowWindow (hWndDB, SW_SHOWNORMAL);
            UpdateWindow (hWndDB);

            // Make sure we can communicate between windows
            AttachThreadInput (GetCurrentThreadId (), dwMainThreadId, TRUE);

            // Tell the SM we're active
            PostMessageW (lpMemPTD->hWndSM, WM_PARENTNOTIFY, MAKEWPARAM (WM_CREATE, IDWC_SM_DB), (LPARAM) hWndDB);

            // Main message loop
            while (GetMessageW (&Msg, NULL, 0, 0))
            {
                // Handle MDI messages and accelerators
                if (!TranslateMDISysAccel (hWndMC, &Msg)
                 && ((!hAccel) || !TranslateAcceleratorW (hWndMF, hAccel, &Msg)))
                {
                    TranslateMessage (&Msg);
                    DispatchMessageW (&Msg);
                } // End IF
            } // End WHILE

            // GetMessageW returned FALSE for a Quit message
        } // End IF
    } __except (CheckException (GetExceptionInformation (), L"CreateDebuggerInThread"))
    {
        // Display message for unhandled exception
        DisplayException ();
    } // End __try/__except

    return 0;
} // End CreateDebuggerInThread
#endif


#ifdef DEBUG
//***************************************************************************
//  $DB_Create
//
//  Perform window-specific initialization
//***************************************************************************

void DB_Create
    (HWND hWnd)

{
} // End DB_Create
#endif


#ifdef DEBUG
//***************************************************************************
//  $DB_Delete
//
//  Perform window-specific uninitialization
//***************************************************************************

void DB_Delete
    (HWND hWnd)

{
} // End DB_Delete
#endif


#ifdef DEBUG
//***************************************************************************
//  $DBWndProc
//
//  Message processing routine for the Debugger window
//***************************************************************************

LRESULT APIENTRY DBWndProc
    (HWND   hWnd,       // Window handle
     UINT   message,    // Type of message
     WPARAM wParam,     // Additional information
     LPARAM lParam)     // ...

{
    char         szTemp[1204];
    WCHAR        wszTemp[1024];
    int          iLineNum,
                 iIndex,
                 iHeight;
    RECT         rcClient;
    HWND         hWndLB;            // ListBox window handle
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LRESULT      lResult = FALSE;   // Result from DefMDIChildProcW

#define PROP_LINENUM    L"iLineNum"

    // Get the window handle of the Listbox
    (HANDLE_PTR) hWndLB = GetWindowLongPtrW (hWnd, GWLDB_HWNDLB);

////LCLODSAPI ("DB: ", hWnd, message, wParam, lParam);
    switch (message)
    {
        case WM_CREATE:
            iLineNum = 0;
            SetPropW (hWnd, PROP_LINENUM, ULongToHandle (iLineNum));

            // Create a listbox to fit inside this window
            hWndLB =
              CreateWindowW (WC_LISTBOXW,           // Class name
                             L"Debugger Listbox",   // For debugging only
                             0
                           | WS_CHILD
                           | WS_VSCROLL
                           | WS_HSCROLL
                           | WS_CLIPCHILDREN
                           | LBS_NOINTEGRALHEIGHT
                           | LBS_EXTENDEDSEL
                           | LBS_MULTIPLESEL
                           | LBS_OWNERDRAWVARIABLE
                           | LBS_HASSTRINGS
                             ,                      // Styles
                             0,                     // X-position
                             0,                     // Y-...
                             CW_USEDEFAULT,         // Width
                             CW_USEDEFAULT,         // Height
                             hWnd,                  // Parent window
                             (HMENU) IDWC_DB_LB,    // ID
                             _hInstance,            // Instance
                             0);                    // lParam
            // Save for later use
            SetWindowLongPtrW (hWnd, GWLDB_HWNDLB, (APLU3264) (LONG_PTR) hWndLB);

            // Show the windows
            ShowWindow (hWndLB, SW_SHOWNORMAL);
            ShowWindow (hWnd,   SW_SHOWNORMAL);

            // Get ptr to PerTabData global memory
            lpMemPTD = GetMemPTD ();

            // Subclass the List Box so we can pass
            //   certain WM_KEYDOWN messages to the
            //   session manager.
            (HANDLE_PTR) lpfnOldListboxWndProc =
              SetWindowLongPtrW (hWndLB,
                                 GWLP_WNDPROC,
                                 (APLU3264) (LONG_PTR) (WNDPROC) &LclListboxWndProc);
            // Initialize window-specific resources
            DB_Create (hWnd);

            // Use Post here as we need to wait for the LB window
            //   to be drawn.
            PostMessageW (hWnd, MYWM_INIT_DB, 0, 0);

////////////goto NORMAL_EXIT;       // We handled the msg
            break;

        case MYWM_INIT_DB:
            // Tell the Listbox Control about its font
            SendMessageW (hWndLB, WM_SETFONT, (WPARAM) hFontSM, MAKELPARAM (TRUE, 0));

            goto NORMAL_EXIT;       // We handled the msg

        case WM_SETFONT:            // hFont = (HFONT) wParam;
                                    // fRedraw = LOWORD (lParam);
        case WM_MOUSEWHEEL:
            // Pass these messages through to the ListBox
            SendMessageW (hWndLB, message, wParam, lParam);

            goto NORMAL_EXIT;       // We handled the msg

        case WM_SYSCOLORCHANGE:
        case WM_SETTINGCHANGE:
            // Uninitialize window-specific resources
            DB_Delete (hWnd);

            // Initialize window-specific resources
            DB_Create (hWnd);

            break;                  // Continue with next handler

#define fwSizeType  wParam
#define nWidth      (LOWORD(lParam))
#define nHeight     (HIWORD(lParam))
        case WM_SIZE:               // fwSizeType = wParam;      // Resizing flag
                                    // nWidth = LOWORD(lParam);  // Width of client area
                                    // nHeight = HIWORD(lParam); // Height of client area
            if (fwSizeType NE SIZE_MINIMIZED)
                SetWindowPos (hWndLB,           // Window handle to position
                              0,                // SWP_NOZORDER
                              0,                // X-position
                              0,                // Y-...
                              nWidth,           // Width
                              nHeight,          // Height
                              SWP_NOZORDER      // Flags
                            | SWP_SHOWWINDOW
                             );
            break;                  // Continue with next handler ***MUST***
#undef  nHeight
#undef  nWidth
#undef  fwSizeType

        case WM_MEASUREITEM:
        {
            LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT) lParam;

            // Set the height of the list box items
            //   less a little so as to cram more into the ListBox
            lpmis->itemHeight = GetFSDirAveCharSize (FONTENUM_SM)->cy - 2;

            goto NORMAL_EXIT;       // We handled the msg
        } // End WM_MEASUREITEM

        case WM_ERASEBKGND:
            return TRUE;                // Tell 'em we erased the background

        case WM_DRAWITEM:
        {
            LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam; // Ptr to DRAWITEMSTRUCT
            WCHAR            wszText[1024];     // Save area for text
            int              iOffset,           // 1 if the text starts with UTF16_REFCNT_NE1, 0 otherwise
                             bkMode;            // Background mode
            COLORREF         bkColor,           // Background color
                             fgColor;           // Foreground ...
            RECT             rcClient,          // Rectangle for Listbox's client area
                             rcItem;            // Rectangle for use when double buffering
            HBRUSH           hBrush;            // Handle to brush for painting the background

            // Get the corresponding line
            SendMessageW (hWndLB, LB_GETTEXT, lpdis->itemID, (LPARAM) wszText);

            bkColor = GetBkColor   (lpdis->hDC);
            fgColor = GetTextColor (lpdis->hDC);
            bkMode  = GetBkMode    (lpdis->hDC);

            // Check for leading UTF16_REFCNT_NE1
            iOffset = (wszText[0] EQ UTF16_REFCNT_NE1);

            // Copy the orginal rectangle
            rcItem = lpdis->rcItem;

            // Get the Listbox's client area
            GetClientRect (hWndLB, &rcClient);

            // Split cases based upon the item action
            switch (lpdis->itemAction)
            {
// Define the following name to use a memory DC in WM_PAINT
//   for drawing operations so as to reduce screen flicker.
// Otherwise, use a memory DC inside the WM_DRAWITEM message.
#define   USEMEMDC_WM_PAINT

#ifndef USEMEMDC_WM_PAINT
                HDC     hDCMem;             // Handle to memory device context
                HBITMAP hBitmap,            // Handle to compatible bitmap
                        hBitmapOld;         // Handle to memory DC old bitmap
                HFONT   hFontOld;           // Handle to old font
#else
  #define hDCMem        (lpdis->hDC)
#endif
                case ODA_DRAWENTIRE:
                case ODA_SELECT:
#ifndef USEMEMDC_WM_PAINT
                    // Move the rectangle to the upper left corner
////////////////////rcItem.right  -= rcItem.left;       // Overridden below
                    rcItem.bottom -= rcItem.top;
                    rcItem.top     =
                    rcItem.left    = 0;

                    // Set the width to that of the client area
                    //   so we can fill out the entire line with FillRect
                    rcItem.right   = rcClient.right - rcClient.left;

                    // Create a compatible memory DC and bitmap
                    hDCMem  = MyCreateCompatibleDC     (lpdis->hDC);
                    hBitmap = MyCreateCompatibleBitmap (lpdis->hDC,
                                                        rcItem.right,
                                                        rcItem.bottom);
                    hBitmapOld = SelectObject (hDCMem, hBitmap);
#endif
                    // If the string is to be drawn as selected, ...
                    if (lpdis->itemState & ODS_SELECTED)
                    {
                        // Set the memory DC attributes
                        SetBkColor   (hDCMem, GetSysColor (COLOR_HIGHLIGHT));
                        SetTextColor (hDCMem, GetSysColor (COLOR_HIGHLIGHTTEXT));
                        SetBkMode    (hDCMem, OPAQUE);
                    } else
                    // If the string is to be drawn with xxCOLOR_REFCNT_NE1, ...
                    if (iOffset)
                    {
                        // Set the memory DC attributes
                        SetBkColor   (hDCMem, BKCOLOR_REFCNT_NE1);
                        SetTextColor (hDCMem, FGCOLOR_REFCNT_NE1);
                        SetBkMode    (hDCMem, OPAQUE);
                    } else
                    {
                        // Set the memory DC attributes
                        SetBkColor   (hDCMem, bkColor);
                        SetTextColor (hDCMem, fgColor);
                        SetBkMode    (hDCMem, bkMode);
                    } // End IF/ELSE/...
#ifndef USEMEMDC_WM_PAINT
                    hFontOld = SelectObject (hDCMem, GetCurrentObject (lpdis->hDC, OBJ_FONT));
#endif
                    // Create a background color brush
                    hBrush = MyCreateSolidBrush (GetBkColor (lpdis->hDC));

                    // Handle WM_ERASEBKGND here by filling in
                    //   the memory DC with the background brush
                    FillRect (hDCMem, &rcItem, hBrush);

                    // We no longer need this resource
                    MyDeleteObject (hBrush); hBrush = NULL;

                    // Draw the text into the memory DC
                    DrawTextW (hDCMem,
                              &wszText[iOffset],
                               lstrlenW (&wszText[iOffset]),
                              &rcItem,
                               DT_VCENTER | DT_SINGLELINE);
#ifndef USEMEMDC_WM_PAINT
                    // Copy the memory DC to the screen DC
                    BitBlt (lpdis->hDC,
                            lpdis->rcItem.left,
                            lpdis->rcItem.top,
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
#else
  #undef  hDCMem
#endif
                    // If this is the last item in the Listbox, ...
                    if (lpdis->itemID EQ (UINT) (SendMessageW (hWndLB, LB_GETCOUNT, 0, 0) - 1))
                    {
                        // Set the unpainted rectangle at the bottom of the client area
                        rcItem.top    = lpdis->rcItem.bottom;
                        rcItem.bottom = rcClient.bottom;

                        // Create a background color brush
                        hBrush = MyCreateSolidBrush (bkColor);

                        FillRect (lpdis->hDC, &rcItem, hBrush);

                        // We no longer need this resource
                        MyDeleteObject (hBrush); hBrush = NULL;
                    } // End IF

                    break;

                case ODA_FOCUS:     // Ignore changes in focus
                    break;

                default:
                    break;
            } // End SWITCH

            // Restore the original attributes
            SetBkColor   (lpdis->hDC, bkColor);
            SetTextColor (lpdis->hDC, fgColor);
            SetBkMode    (lpdis->hDC, bkMode);

            // Set the result to indciate we processed the message
            lResult = TRUE;

            goto NORMAL_EXIT;       // We handled the msg
        } // End WM_DRAWITEM

        case MYWM_DBGMSGA:          // Single-char message
            iLineNum = HandleToUlong (GetPropW (hWnd, PROP_LINENUM));

            // Format the string with a preceding line #
            wsprintfA (szTemp,
                       "%5d:  %s",
                       ++iLineNum,
                       *(LPCHAR *) &lParam);
            // Convert the string from A to W
            A2W (wszTemp, szTemp, sizeof (wszTemp) - 1);

            SetPropW (hWnd, PROP_LINENUM, ULongToHandle (iLineNum));

            // Add the string to the ListBox
            iIndex = (UINT) SendMessageW (hWndLB, LB_ADDSTRING, 0, (LPARAM) wszTemp);

            // Scroll this item into view
            SendMessageW (hWnd, MYWM_DBGMSG_SCROLL, iIndex, 0);

            goto NORMAL_EXIT;       // We handled the msg

        case MYWM_DBGMSGW:          // Double-char message
            iLineNum = HandleToULong (GetPropW (hWnd, PROP_LINENUM));

            // Determine whether or not this line is in xxCOLOR_REFCNT_NE1
            iIndex = (UTF16_REFCNT_NE1 EQ (*(LPWCHAR *) &lParam)[0]);

            // Format the string with a preceding line #
            wsprintfW (wszTemp,
                       L"%s%5d:  %s",
                       iIndex ? WS_UTF16_REFCNT_NE1 : L"",
                       ++iLineNum,
                       &(*(LPWCHAR *) &lParam)[iIndex]);

            SetPropW (hWnd, PROP_LINENUM, ULongToHandle (iLineNum));

            // Add the string to the ListBox
            iIndex = (UINT) SendMessageW (hWndLB, LB_ADDSTRING, 0, (LPARAM) wszTemp);

            // Scroll this item into view
            SendMessageW (hWnd, MYWM_DBGMSG_SCROLL, iIndex, 0);

            goto NORMAL_EXIT;       // We handled the msg

        case MYWM_DBGMSG_SCROLL:    // Scroll item into view
            iIndex = (int) wParam;  // Get the item #

            // Check for last line #
            if (iIndex EQ -1)
                iIndex = (UINT) SendMessageW (hWndLB, LB_GETCOUNT, 0, 0);

            // Ensure the item just added is visible
            GetClientRect (hWndLB, &rcClient);

            // Common item height
            iHeight = (UINT) SendMessageW (hWndLB, LB_GETITEMHEIGHT, 0, 0);

            // Less # whole items we can display
            iIndex -= (rcClient.bottom / iHeight) - 1;

            SendMessageW (hWndLB, LB_SETTOPINDEX, max (0, iIndex), 0);

            UpdateWindow (hWnd);    // Redraw the screen now

            goto NORMAL_EXIT;       // We handled the msg

        case MYWM_DBGMSG_CLR:
            // Start over again
            SendMessageW (hWndLB, LB_RESETCONTENT, 0, 0);
            iLineNum = 0;
            SetPropW (hWnd, PROP_LINENUM, ULongToHandle (iLineNum));

            UpdateWindow (hWnd);    // Redraw the screen now

            goto NORMAL_EXIT;       // We handled the msg

        case WM_CLOSE:
            // Remove saved window properties
            RemovePropW (hWnd, PROP_LINENUM);

            DestroyWindow (hWnd);

            goto NORMAL_EXIT;       // We handled the msg

        case WM_DESTROY:
            // Uninitialize window-specific resources
            DB_Delete (hWnd);

            // Tell the thread to quit, too
            PostQuitMessage (0);

            goto NORMAL_EXIT;       // We handled the msg
    } // End SWITCH

////LCLODSAPI ("DBY:", hWnd, message, wParam, lParam);
    lResult =
      DefMDIChildProcW (hWnd, message, wParam, lParam);
NORMAL_EXIT:
////LCLODSAPI ("DBZ:", hWnd, message, wParam, lParam);

    return lResult;
} // End DBWndProc
#endif


#ifdef DEBUG
//***************************************************************************
//  $LclListboxWndProc
//
//  Local window procedure for the debugger's LISTBOX
//    to pass on certain WM_KEYDOWN messages to the session manager.
//***************************************************************************

LRESULT WINAPI LclListboxWndProc
    (HWND   hWnd,       // Window handle
     UINT   message,    // Type of message
     WPARAM wParam,     // Additional information
     LPARAM lParam)     // ...

{
    HMENU        hMenu;
    int          iSelCnt,
                 mfState,
                 i,
                 iTotalBytes;
    POINT        ptScr;
    HGLOBAL      hGlbInd,
                 hGlbSel;
    LPINT        lpInd;
    LPWCHAR      lpSel,
                 p;
    LRESULT      lResult;
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

////LCLODSAPI ("LLB: ", hWnd, message, wParam, lParam);

    // Split cases
    switch (message)
    {
#ifdef USEMEMDC_WM_PAINT
#define hDC     ((HDC) wParam)
        case WM_PAINT:              // hDC = (HDC) wParam
        {
            RECT    rcUpdate;           // Update rectangle
            HDC     hDCMem;             // Handle to memory device context
            HBITMAP hBitmap,            // Handle to compatible bitmap
                    hBitmapOld;         // Handle to memory DC old bitmap

            // If there's an incoming DC, pass the message on
            if (hDC)
                break;

            // Get the update rectangle; if none, exit
            if (!GetUpdateRect (hWnd, &rcUpdate, FALSE))
                break;

            // Allocate a DC for painting the client area
            hDC = MyGetDC (hWnd);

            // Set the bounding rectangle
            SetBoundsRect (hDC, &rcUpdate, DCB_SET);

            // Create a compatible memory DC and bitmap
            hDCMem  = MyCreateCompatibleDC     (hDC);
            hBitmap = MyCreateCompatibleBitmap (hDC,
                                                rcUpdate.right,
                                                rcUpdate.bottom);
            hBitmapOld = SelectObject (hDCMem, hBitmap);

            // Pass on down the line
            lResult =
              CallWindowProcW (lpfnOldListboxWndProc,
                               hWnd,
                               message,
                      (WPARAM) hDCMem,
                               lParam);
            // Copy the memory DC to the screen DC
            BitBlt (hDC,
                    rcUpdate.left,
                    rcUpdate.top,
                    rcUpdate.right,
                    rcUpdate.bottom,
                    hDCMem,
                    rcUpdate.left,
                    rcUpdate.top,
                    SRCCOPY);
            // Validate the window
            ValidateRect (hWnd, &rcUpdate);

            // Restore the old resources
            SelectObject (hDCMem, hBitmapOld);

            // We no longer need these resources
            MyDeleteObject (hBitmap); hBitmap = NULL;
            MyDeleteDC (hDCMem); hDCMem = NULL;
            MyReleaseDC (hWnd, hDC); hDC = NULL;

            return FALSE;               // We handled the msg
        } // End WM_PAINT
#undef  hDC
#endif
        case WM_ERASEBKGND:
            return TRUE;                // Tell 'em we erased the background

#define idCtl               GET_WM_COMMAND_ID   (wParam, lParam)
#define cmdCtl              GET_WM_COMMAND_CMD  (wParam, lParam)
#define hwndCtl             GET_WM_COMMAND_HWND (wParam, lParam)
        case WM_COMMAND:
            switch (idCtl)
            {
                case IDM_COPY:
                    // Get the # selected items
                    iSelCnt = (UINT) SendMessageW (hWnd, LB_GETSELCOUNT, 0, 0);

                    // Allocate space for that many indices
                    hGlbInd = GlobalAlloc (GHND, iSelCnt * sizeof (int));

                    // Lock the memory to get a ptr to it
                    lpInd = GlobalLock (hGlbInd);

                    // Populate the array
                    SendMessageW (hWnd, LB_GETSELITEMS, iSelCnt, (LPARAM) lpInd);

                    // Loop through the selected items and calculate
                    //   the storage requirement for the collection
                    for (iTotalBytes = i = 0; i < iSelCnt; i++)
                        // The "EOL_LEN +" is for the AC_CR and AC_LF at the end of each line
                        iTotalBytes += sizeof (WCHAR) * (EOL_LEN + (UINT) SendMessageW (hWnd, LB_GETTEXTLEN, lpInd[i], 0));

                    // Allocate storage for the entire collection
                    hGlbSel = GlobalAlloc (GHND | GMEM_DDESHARE, iTotalBytes);

                    // Lock the memory to get a ptr to it
                    lpSel = GlobalLock (hGlbSel);

                    // Copy the text to the array, separated by a newline
                    for (p = lpSel, i = 0; i < iSelCnt; i++)
                    {
                        p += (UINT) SendMessageW (hWnd, LB_GETTEXT, lpInd[i], (LPARAM) p);
                        *p++ = AC_CR;
                        *p++ = AC_LF;
                    } // End FOR

                    // We no longer need this ptr
                    GlobalUnlock (hGlbSel); lpSel = NULL;

                    // We no longer need this ptr
                    GlobalUnlock (hGlbInd); lpInd = NULL;

                    // Free the memory
                    GlobalFree (hGlbInd); hGlbInd = NULL;

                    // Prepare to put the data onto the clipboard
                    OpenClipboard (hWnd);
                    EmptyClipboard ();

                    // Put the data onto the clipboard
                    SetClipboardData (CF_UNICODETEXT,
                                      hGlbSel);

                    // We're finished with the clipboard
                    CloseClipboard ();

////////////////////// Free the memory -- NO, the system now owns hGlbSel
////////////////////GlobalFree (hGlbSel); hGlbSel = NULL;

                    break;

                case IDM_DELETE:
                    // Delete selected items
                    while (TRUE)
                    {
                        UINT uNumItems,         // # selected items
                             uSelItems[128];    // Indices of selected items to delete

                        // Get the indices of the selected items
                        uNumItems = (UINT)
                          SendMessageW (hWnd,
                                        LB_GETSELITEMS,
                                        countof (uSelItems),
                              (LPARAM) &uSelItems);
                        // If there are any items still selected, ...
                        if (uNumItems)
                            // Loop through them all
                            while (uNumItems--)
                                // Delete this item
                                SendMessageW (hWnd,
                                              LB_DELETESTRING,
                                              uSelItems[uNumItems],
                                              0);
                        else
                            break;
                    } // End WHILE

                    break;

                case IDM_DELETEALL:
                    // Delete all items
                    DbgClr ();

                    break;

                case IDM_SELECTALL:
                    // Select all items
                    if (LB_ERR EQ SendMessageW (hWnd,
                                               LB_SELITEMRANGE,
                                               TRUE,
                                               MAKELPARAM (0, -1)))
                        DbgStop ();     // We should never get here
                    break;
            } // End SWITCH

            break;
#undef  hwndCtl
#undef  cmdCtl
#undef  idCtl

        case WM_RBUTTONDOWN:                // fwKeys = wParam;         // key flags
                                            // xPos = LOSHORT (lParam); // horizontal position of cursor
                                            // yPos = HISHORT (lParam); // vertical position of cursor
            // Ensure there are items selected
            iSelCnt = (UINT) SendMessageW (hWnd, LB_GETSELCOUNT, 0, 0);

            mfState = (iSelCnt EQ 0) ? MF_GRAYED : MF_ENABLED;

            // Get the mouse position in screen coordinates
            GetCursorPos (&ptScr);

            // Create a popup menu
            hMenu = CreatePopupMenu ();

            AppendMenuW (hMenu,                 // Handle
                         mfState
                       | MF_STRING,             // Flags
                         IDM_COPY,              // ID
                         L"&Copy");             // Text
            AppendMenuW (hMenu,                 // Handle
                         mfState
                       | MF_STRING,             // Flags
                         IDM_DELETE,            // ID
                         L"&Delete");           // Text
            AppendMenuW (hMenu,                 // Handle
                         MF_ENABLED
                       | MF_STRING,             // Flags
                         IDM_DELETEALL,         // ID
                         L"Delet&e All");       // Text
            AppendMenuW (hMenu,                 // Handle
                         MF_ENABLED
                       | MF_STRING,             // Flags
                         IDM_SELECTALL,         // ID
                         L"Selec&t All");       // Text

            TrackPopupMenu (hMenu,              // Handle
                            0                   // Flags
                          | TPM_CENTERALIGN
                          | TPM_LEFTBUTTON
                          | TPM_RIGHTBUTTON
                            ,
                            ptScr.x,            // x-position
                            ptScr.y,            // y-position
                            0,                  // Reserved (must be zero)
                            hWnd,               // Handle of owner window
                            NULL);              // Dismissal area outside rectangle (none)

            // Free the menu resources
            DestroyMenu (hMenu);

            break;

        case WM_KEYDOWN:            // nVirtKey = (int) wParam;     // virtual-key code
                                    // lKeyData = lParam;           // Key data
#define nVirtKey ((int) wParam)
        {
            // Process the virtual key
            switch (nVirtKey)
            {
                case VK_F1:
                case VK_F2:
                case VK_F3:
                case VK_F4:
                case VK_F5:
                case VK_F6:
                case VK_F7:
                case VK_F8:
                case VK_F9:
                case VK_F10:
                case VK_F11:
                case VK_F12:
                    // Get ptr to PerTabData global memory
                    lpMemPTD = GetMemPTD ();

                    PostMessageW (lpMemPTD->hWndSM, message, wParam, lParam);

                    return FALSE;   // We handled the msg
            } // End SWITCH

            break;
        } // End WM_KEYDOWN
#undef  nVirtKey
    } // End SWITCH

////LCLODSAPI ("LLBZ: ", hWnd, message, wParam, lParam);
    lResult = CallWindowProcW (lpfnOldListboxWndProc,
                               hWnd,
                               message,
                               wParam,
                               lParam); // Pass on down the line
    return lResult;
} // End LclListboxWndProc
#endif


#ifdef DEBUG
//***************************************************************************
//  $DbgMsg
//
//  Display a SBCS debug message
//***************************************************************************

void DbgMsg
    (LPCHAR szTemp)

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); // Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Ensure it's a valid ptr
    if (!IsValidPtr (lpMemPTD, sizeof (lpMemPTD)))
        return;

    if (lpMemPTD->hWndDB)
    {
        WCHAR wszTemp[1024];        // Temporary storage for the message in wide form

        // Convert the string from A to W
        A2W (wszTemp, szTemp, sizeof (wszTemp) - 1);

        DbgMsgW (wszTemp);
    } // End IF
} // End DbgMsg
#endif


#ifdef DEBUG
//***************************************************************************
//  $DbgMsgW
//
//  Display a DBCS debug message
//***************************************************************************

void DbgMsgW
    (LPWCHAR wszTemp)

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    HWND         hWndDB;        // Debugger window handle

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); // Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Ensure it's a valid ptr
    if (!IsValidPtr (lpMemPTD, sizeof (lpMemPTD)))
        return;

    // Get the debugger window handle
    hWndDB = lpMemPTD->hWndDB;

    if (hWndDB)
        SendMessageW (hWndDB, MYWM_DBGMSGW, 0, (LPARAM) wszTemp);
} // End DbgMsgW
#endif


#ifdef DEBUG
//***************************************************************************
//  $DbgClear
//
//  Clear the debug window
//***************************************************************************

void DbgClr
    (void)

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    if (lpMemPTD->hWndDB)
        PostMessageW (lpMemPTD->hWndDB, MYWM_DBGMSG_CLR, 0, 0);
} // End DbgClr
#endif


#ifdef DEBUG
//***************************************************************************
//  $oprintfW
//
//  Display a message a la printfW to ODS
//***************************************************************************

int oprintfW
    (LPWCHAR lpwszFmt,
     ...)

{
    va_list  vl;
    APLU3264 i1, i2, i3, i4, i5, i6, i7, i8;
    int      iRet;
    WCHAR    wszTemp[1024];


    // We hope that no one calls us with more than
    //   eight arguments.
    // Note we must grab them separately this way
    //   as using va_arg in the argument list to
    //   wsprintf pushes the arguments in reverse
    //   order.
    va_start (vl, lpwszFmt);

    i1 = va_arg (vl, APLU3264);
    i2 = va_arg (vl, APLU3264);
    i3 = va_arg (vl, APLU3264);
    i4 = va_arg (vl, APLU3264);
    i5 = va_arg (vl, APLU3264);
    i6 = va_arg (vl, APLU3264);
    i7 = va_arg (vl, APLU3264);
    i8 = va_arg (vl, APLU3264);

    va_end (vl);

    iRet = wsprintfW (wszTemp,
                      lpwszFmt,
                      i1, i2, i3, i4, i5, i6, i7, i8);
    OutputDebugStringW (wszTemp);

    return iRet;
} // End oprintfW
#endif


#ifdef DEBUG
//***************************************************************************
//  $dprintfWL0
//
//  Display a debug message a la printfW
//***************************************************************************

int dprintfWL0
    (LPWCHAR lpwszFmt,
     ...)

{
    va_list  vl;
    APLU3264 i1, i2, i3, i4, i5, i6, i7, i8;
    int      iRet;
    WCHAR    wszTemp[1024];

    // We hope that no one calls us with more than
    //   eight arguments.
    // Note we must grab them separately this way
    //   as using va_arg in the argument list to
    //   wsprintf pushes the arguments in reverse
    //   order.
    va_start (vl, lpwszFmt);

    i1 = va_arg (vl, APLU3264);
    i2 = va_arg (vl, APLU3264);
    i3 = va_arg (vl, APLU3264);
    i4 = va_arg (vl, APLU3264);
    i5 = va_arg (vl, APLU3264);
    i6 = va_arg (vl, APLU3264);
    i7 = va_arg (vl, APLU3264);
    i8 = va_arg (vl, APLU3264);

    va_end (vl);

    iRet = wsprintfW (wszTemp,
                      lpwszFmt,
                      i1, i2, i3, i4, i5, i6, i7, i8);
    DbgMsgW (wszTemp);

    return iRet;
} // End dprintfWL0
#endif


#ifdef DEBUG
//***************************************************************************
//  $dprintfWL9
//
//  Display a debug message a la printfW
//***************************************************************************

int dprintfWL9
    (LPWCHAR lpwszFmt,
     ...)

{
    va_list  vl;
    APLU3264 i1, i2, i3, i4, i5, i6, i7, i8;
    int      iRet;
    WCHAR    wszTemp[1024];

    if (gDbgLvl < 9)
        return 0;

    // We hope that no one calls us with more than
    //   eight arguments.
    // Note we must grab them separately this way
    //   as using va_arg in the argument list to
    //   wsprintf pushes the arguments in reverse
    //   order.
    va_start (vl, lpwszFmt);

    i1 = va_arg (vl, APLU3264);
    i2 = va_arg (vl, APLU3264);
    i3 = va_arg (vl, APLU3264);
    i4 = va_arg (vl, APLU3264);
    i5 = va_arg (vl, APLU3264);
    i6 = va_arg (vl, APLU3264);
    i7 = va_arg (vl, APLU3264);
    i8 = va_arg (vl, APLU3264);

    va_end (vl);

    iRet = wsprintfW (wszTemp,
                      lpwszFmt,
                      i1, i2, i3, i4, i5, i6, i7, i8);
    DbgMsgW (wszTemp);

    return iRet;
} // End dprintfWL9
#endif


#ifdef DEBUG
//***************************************************************************
//  $DbgGlobalAllocSub
//
//  Debug version of MyGlobalAlloc
//***************************************************************************

HGLOBAL DbgGlobalAllocSub
    (UINT     uFlags,
     APLU3264 ByteRes,
     LPWCHAR  lpwFmtStr,
     LPSTR    lpFileName,
     UINT     uLineNum)

{
    HGLOBAL hGlbRes;

    hGlbRes = MyGlobalAlloc (uFlags, ByteRes);

    if (hGlbRes)
        dprintfWL9 (lpwFmtStr, hGlbRes, lpFileName, uLineNum);

    return hGlbRes;
} // End DbgGlobalAllocSub
#endif


//***************************************************************************
//  $FileNameOnly
//
//  Extract the filename only from __FILE__
//***************************************************************************

LPCHAR FileNameOnly
    (LPCHAR lpFileName)

{
    LPCHAR p, q;

    p = lpFileName;

    for (p = lpFileName;
         q = strchr (p, AC_SLOPE);
         p = q + 1)
        ;
    return p;
} // End FileNameOnly


//***************************************************************************
//  End of File: debug.c
//***************************************************************************
