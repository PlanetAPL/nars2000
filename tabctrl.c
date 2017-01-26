//***************************************************************************
//  NARS2000 -- Tab Control Routines
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
#include "headers.h"
#include <gsl/gsl_rng.h>


// The width & height of an image as drawn on the tab
#define IMAGE_WIDTH     (IMAGE_TC_CX + 3)
#define IMAGE_HEIGHT    (IMAGE_TC_CY + 3)

// The left and right margins for a tab
#define TABMARGIN_LEFT      8
#define TABMARGIN_RIGHT     8

// Define the starting char for tab numbering
#define TABNUMBER_START     L'\x2460'

CNT_THREAD cntThread;           // Temporary storage for CreateNewTabInThread
CR_THREAD  crThread;            // Temporary storage for CreateResetInThread

// 1s mark the indices in use
UCHAR crIndices[32] = {0};      // This supports 256 (=32x8) open WSs

typedef struct tagTABCOLORS
{
    COLORREF bk,                        // Background
             fg,                        // Foreground
             hl;                        // Highlight
} TABCOLORS;

TABCOLORS crTab[] =
//                                Text          Text
//     Background              Foreground     Highlight
{
 {DEF_SCN_MOCCASIN          , DEF_SCN_BLACK, DEF_SCN_RED},
 {DEF_SCN_FUCHSIA           , DEF_SCN_BLACK, DEF_SCN_RED},
 {DEF_SCN_MEDIUMSLATEBLUE   , DEF_SCN_BLACK, DEF_SCN_RED},
 {DEF_SCN_DODGERBLUE        , DEF_SCN_BLACK, DEF_SCN_RED},
 {DEF_SCN_LIMEGREEN         , DEF_SCN_BLACK, DEF_SCN_RED},
 {DEF_SCN_GOLD              , DEF_SCN_BLACK, DEF_SCN_RED},
 {DEF_SCN_DARKKHAKI         , DEF_SCN_BLACK, DEF_SCN_RED},
 {DEF_SCN_BURLYWOOD         , DEF_SCN_BLACK, DEF_SCN_RED},
 {RGB (193,208,165)         , DEF_SCN_BLACK, DEF_SCN_RED},
 {DEF_SCN_SANDYBROWN        , DEF_SCN_BLACK, DEF_SCN_RED},
 {DEF_SCN_DARKGOLDENROD     , DEF_SCN_BLACK, DEF_SCN_RED},
 {DEF_SCN_MEDIUMSPRINGGREEN , DEF_SCN_BLACK, DEF_SCN_RED},
 {DEF_SCN_GREENYELLOW       , DEF_SCN_BLACK, DEF_SCN_RED},
 {DEF_SCN_DARKVIOLET        , DEF_SCN_BLACK, DEF_SCN_RED},
 {DEF_SCN_LAVENDER          , DEF_SCN_BLACK, DEF_SCN_RED},
 {DEF_SCN_HOTPINK           , DEF_SCN_BLACK, DEF_SCN_RED},
 {DEF_SCN_CRIMSON           , DEF_SCN_BLACK, DEF_SCN_RED},
};

#define NUM_CRTABS  countof (crTab)


//***************************************************************************
//  $EnumCallbackShowHide
//
//  Enum child proc for ShowChildWindows
//***************************************************************************

UBOOL CALLBACK EnumCallbackShowHide
    (HWND   hWnd,           // Handle to child window
     LPARAM lParam)         // Application-defined value

{
    // When an MDI child window is minimized, Windows creates two windows: an
    // icon and the icon title.  The parent of the icon title window is set to
    // the MDI client window, which confines the icon title to the MDI client
    // area.  The owner of the icon title is set to the MDI child window.
    if (GetWindow (hWnd, GW_OWNER))     // If it's an icon title window, ...
        return TRUE;                    // skip it, and continue enumerating

    // Show or hide the window
    ShowWindow (hWnd, *(LPUINT) &lParam);

    return TRUE;                        // Continue enumerating
} // End EnumCallbackShowHide


//***************************************************************************
//  $ShowHideChildWindows
//
//  Show or hide the given window and all its children
//***************************************************************************

void ShowHideChildWindows
    (HWND  hWndMC,      // Window handle to MDI Client
     UBOOL bShow)       // TRUE iff showing the window, FALSE if hiding it

{
    if (!hWndMC)
        return;

    // If we're showing, ...
    if (bShow)
    {
        HWND hWndAct;

        // Put the MDI Client window at the top of the Z-order
        SetWindowPos (hWndMC,
                      HWND_TOP,
                      0, 0, 0, 0,
                      SWP_NOMOVE
                    | SWP_NOSIZE);
        // Show/hide the MDI Client window
        ShowWindow (hWndMC, SW_SHOWNORMAL);

        // Loop through the child windows
        EnumChildWindows (hWndMC,
                          EnumCallbackShowHide,
                          SW_SHOWNORMAL);
        // Set the focus to the active window
        hWndAct = (HWND) SendMessageW (hWndMC, WM_MDIGETACTIVE, 0, 0);
        if (hWndAct)
        {
            SetFocus (hWndAct);
            SendMessageW ((HWND) GetWindowLongPtrW (hWndAct, GWLSF_HWNDEC), WM_SETCURSOR, 0, 0);
        } else
            DbgStop ();
    } else
    // We're hiding, ...
    {
        // Loop through the child windows
        EnumChildWindows (hWndMC,
                          EnumCallbackShowHide,
                          SW_HIDE);
        // Put the MDI Client window at the top (SHOW) or bottom (HIDE)
        //   of the Z-order
        SetWindowPos (hWndMC,
                      HWND_BOTTOM,
                      0, 0, 0, 0,
                      SWP_NOMOVE
                    | SWP_NOSIZE);

        // Show/hide the MDI Client window
        ShowWindow (hWndMC, SW_HIDE);
    } // End IF/ELSE
} // End ShowHideChildWindows


//***************************************************************************
//  $CreateNewTab
//
//  Create a new tab
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CreateNewTab"
#else
#define APPEND_NAME
#endif

UBOOL CreateNewTab
    (HWND    hWndParent,        // Window handle of the parent
     LPWCHAR lpwsz,             // Drive, Path, Filename, Ext of the workspace
     int     iTabIndex,         // Insert the new tab to the left of this one
     UBOOL   bExecLX)           // TRUE iff execute []LX after successful load

{
    HANDLE  hThread;            // Handle to the thread
    HGLOBAL hGlbDPFE = NULL;    // Workspace DPFE global memory handle
    LPWCHAR lpwszDPFE;          // Ptr to workspace DPFE global memory
    UINT    uLen;               // Length of workspace DPFE

    // Save the incoming Workspace DPFE in a global memory object
    //   and pass that handle to the Session Manager
    uLen = lstrlenW (lpwsz);

    // Allocate space for the workspace name ("+ 1" for trailing zero)
    // The storage for this handle is freed in <CreateNewTabInThread>.
    hGlbDPFE = DbgGlobalAlloc (GHND, (uLen + 1) * sizeof (lpwsz[0]));
    if (hGlbDPFE EQ NULL)
        return FALSE;

    // Lock the memory to get a ptr to it
    lpwszDPFE = MyGlobalLock100 (hGlbDPFE);

    // Copy the workspace name to global memory
    CopyMemoryW (lpwszDPFE, lpwsz, uLen);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbDPFE); lpwszDPFE = NULL;

    // Save args in struc to pass to thread func
    cntThread.hWndParent = hWndParent;
    cntThread.hGlbDPFE   = hGlbDPFE;        // Freed in sessman.c/WM_CREATE
    cntThread.iTabIndex  = iTabIndex;
    cntThread.bExecLX    = bExecLX;

    // Can't call Debugger as it hasn't been created as yet.
////dprintfWL9 (L"--Starting thread in <CreateNewTab>.");

    // Create a new thread
    hThread = CreateThread (NULL,                   // No security attrs
                            0,                      // Use default stack size
                           &CreateNewTabInThread,   // Starting routine
                           &cntThread,              // Param to thread func
                            CREATE_SUSPENDED,       // Creation flag
                           &cntThread.dwThreadId);  // Returns thread id
    // Save the thread handle
    cntThread.hThread = hThread;

    if (hThread)
        ResumeThread (hThread);

    return (hThread NE NULL);
} // End CreateNewTab
#undef  APPEND_NAME


//***************************************************************************
//  $CalcClientRectMC
//
//  Calculate the client rectangle for the MDI client windows
//    in Master Frame client area coords
//***************************************************************************

void CalcClientRectMC
    (LPRECT lprc,               // Ptr to in/out rect
     UBOOL  bGetRect)           // TRUE iff we should fill in lprc

{
    // If we should fill in the rectangle, ...
    if (bGetRect)
        // Get the size and position of the parent window.
        GetClientRect (hWndMF, lprc);

    // Calculate the display rectangle, assuming the
    //   tab control is the size of the client area.
    TabCtrl_AdjustRect (hWndTC, FALSE, lprc);
} // End CalcClientRectMC


//***************************************************************************
//  $CreateNewTabInThread
//
//  Create a new tab within a thread
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CreateNewTabInThread"
#else
#define APPEND_NAME
#endif

UBOOL WINAPI CreateNewTabInThread
    (LPCNT_THREAD lpcntThread)

{
    int          iCurTabIndex = -1;     // Index of the current tab
    TC_ITEMW     tcItem = {0};          // TabCtrl item struc
    LPPERTABDATA lpMemPTD = NULL,       // Ptr to PerTabData global memory
                 lpMemPTDOld = NULL;    // Outgoing lpMemPTD
    UBOOL        bRet = FALSE;          // TR$UE iff the result is valid
    RECT         rc;                    // Rectangle for setting size of window
    CLIENTCREATESTRUCT ccs;             // For MDI Client window
    SM_CREATESTRUCTW csSM;              // For Session Manager window
    HANDLE       hThread;               // Handle to this thread
    HWND         hWndMC,                // Window handle of MDI Client
                 hWndParent,            // Window handle of the parent
                 hWndTmp;               // Temporary window handle
    int          iTabIndex;             // Insert the new tab to the left of this one
    MSG          Msg;                   // Message for GetMessageW loop
    int          nThreads;
    WCHAR        wszTemp[32];           // Temporary storage

    // Store the thread type ('TC')
    TlsSetValue (dwTlsType, TLSTYPE_TC);

    // Extract values from the arg struc
    hWndParent    = lpcntThread->hWndParent;
    csSM.hGlbDPFE = lpcntThread->hGlbDPFE;      // Freed in sessman.c/WM_CREATE
    iTabIndex     = lpcntThread->iTabIndex;
    csSM.bExecLX  = lpcntThread->bExecLX;
    hThread       = lpcntThread->hThread;

    if (gCurTabID NE -1)
    {
        // Get ptr to PerTabData global memory
        lpMemPTDOld = GetPerTabPtr (TranslateTabIDToIndex (gCurTabID)); Assert (IsValidPtr (lpMemPTDOld, sizeof (lpMemPTDOld)));

        // Hide the child windows of the outgoing tab
        ShowHideChildWindows (lpMemPTDOld->hWndMC, FALSE);
    } // End IF

    // Allocate PerTabData
    lpMemPTD =
      MyVirtualAlloc (NULL,                 // Any address (FIXED SIZE)
                      sizeof (PERTABDATA),
                      MEM_COMMIT | MEM_TOP_DOWN,
                      PAGE_READWRITE);
    if (lpMemPTD EQ NULL)
        goto ERROR_EXIT;    // Stop the whole process

    // Tell the Master Frame about the current lpMemPTD
    PostThreadMessage (dwMainThreadId, MYWM_LPMEMPTD, 0, (LPARAM) lpMemPTD);

    // Save ptr to PerTabData global memory
    TlsSetValue (dwTlsPerTabData, (LPVOID) lpMemPTD);

    // Initialize the item struc
    tcItem.mask       = TCIF_IMAGE | TCIF_PARAM;
////tcItem.pszText    =
////tcItem.cchTextMax =
    tcItem.iImage     = 0;
    tcItem.lParam     = (LPARAM) lpMemPTD;

    // Insert a new tab
    // The new tab is inserted to the left of the index value (iTabIndex)
    iCurTabIndex = (UINT) SendMessageW (hWndTC, TCM_INSERTITEMW, iTabIndex, (LPARAM) &tcItem);
    if (iCurTabIndex EQ -1)
    {
        MB (pszNoInsertTCTab);

        goto ERROR_EXIT;
    } // End IF

    Assert (lpMemPTD EQ GetPerTabPtr (iCurTabIndex));

    // Save the thread ID for when we close
    lpMemPTD->dwThreadId = lpcntThread->dwThreadId;

    // Save the next available color index and Tab ID
    lpMemPTD->CurTabID =
    gCurTabID          =
    lpMemPTD->crIndex  =
      GetNextTabColorIndex ();

    // Save the previous tab ID (if there is one)
    if (iTabIndex EQ 0)
        lpMemPTD->PrvTabID = gCurTabID;
    else
        lpMemPTD->PrvTabID = TranslateTabIndexToID (iTabIndex - 1);

    // Calculate the client rectangle for the MDI Client windows
    //   in Master Frame client area coords using the GetClientRect
    //   value.
    CalcClientRectMC (&rc, TRUE);

    // Fill in the CLIENTCREATESTRUCT for the MDI Client window
    ccs.hWindowMenu = GetSubMenu (GetMenu (hWndParent), IDMPOS_SM_WINDOW);
    ccs.idFirstChild = IDM_CHILDWINDOW;

    // Create the MDI client window
    lpMemPTD->hWndMC =
      CreateWindowExW (0,                   // Extended styles
                       WC_MDICLIENTW,       // Class name
                       wszMCTitle,          // Window title (for debugging purposes only)
                       0
//////               | MDIS_ALLCHILDSTYLES
                     | WS_CHILD
                     | WS_BORDER
                     | WS_CLIPCHILDREN
                       ,                    // Styles
                       rc.left,             // X-coordinate
                       rc.top,              // Y-...
                       rc.right  - rc.left, // Width
                       rc.bottom - rc.top,  // Height
                       hWndParent,          // Parent
                       (HMENU) IDWC_TC_MC,  // ID
                       _hInstance,          // hInstance
                      &ccs);                // lParam
    if (lpMemPTD->hWndMC EQ NULL)
    {
        MB (pszNoCreateMCWnd);

        goto ERROR_EXIT;
    } // End IF

    // Show and paint the window
    ShowWindow (lpMemPTD->hWndMC, SW_SHOWNORMAL);
    UpdateWindow (lpMemPTD->hWndMC);

    // Resize the Master Frame so as to size this MDI Client window
    PostMessageW (hWndMF, MYWM_RESIZE, 0, 0);
#ifdef DEBUG
    // Create the Debugger window first
    //   so it can be used by subsequent windows
    CreateDebuggerWindow (lpMemPTD);
#endif

    // Save hWndMC for use inside message loop
    //   so we can unlock the per-tab data memory
    hWndMC = lpMemPTD->hWndMC;

    // Create the Session Manager window
    hWndTmp =
      CreateMDIWindowW (LSMWNDCLASS,        // Class name
                        wszSMTitle,         // Window title
                        0
                      | WS_CLIPCHILDREN,    // Styles
                        CW_USEDEFAULT,      // X-pos
                        CW_USEDEFAULT,      // Y-pos
                        CW_USEDEFAULT,      // Height
                        CW_USEDEFAULT,      // Width
                        hWndMC,             // Parent
                        _hInstance,         // Instance
              (LPARAM) &csSM);              // Extra data
    // If there's an error, don't display a message
    //   because that is a normal occurrence if we
    //   fail )LOAD.
    if (hWndTmp EQ NULL)
        goto ERROR_EXIT;

    // Save as the SM window handle
    lpMemPTD->hWndSM = hWndTmp;

    // Get # current threads under the SM
    nThreads = 1 + HandleToULong (GetPropW (lpMemPTD->hWndSM, PROP_NTHREADS));

    // Format the next property name
    MySprintfW (wszTemp,
                sizeof (wszTemp),
               L"Thread%d",
                hThread);
    // Set the property to the current thread handle
    SetPropW (hWndParent, wszTemp, hThread);

    // Set the handle of the active window
    lpMemPTD->hWndActive = lpMemPTD->hWndSM;

    // Save the handle
    SetWindowLongPtrW (lpMemPTD->hWndSM, GWLSF_PERTAB, (APLU3264) (LONG_PTR) lpMemPTD);

    // Show the child windows of the incoming tab
    ShowHideChildWindows (lpMemPTD->hWndMC, TRUE);

    // Activate this tab
    TabCtrl_SetCurSel (hWndTC, iCurTabIndex);

    // Draw the tab with the text normal
    InvalidateRect (hWndTC, NULL, FALSE);

    // Tell the SM we're finished
    PostMessageW (lpMemPTD->hWndSM, MYWM_INIT_EC, 0, 0);

    __try
    {
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
    } __except (CheckException (GetExceptionInformation (), L"CreateNewTabInThread"))
    {
        // Display message for unhandled exception
        DisplayException ();
    } // End __try/__except
    // GetMessageW returned FALSE for a Quit message

    // Mark as successful
    bRet = TRUE;

    // Zap so we don't try to free it
    csSM.hGlbDPFE = NULL;

    goto NORMAL_EXIT;

ERROR_EXIT:
    // If there's a current tab index, ...
    if (iCurTabIndex NE -1)
    {
        // Delete the failed tab ctrl
        TabCtrl_DeleteItem (hWndTC, iCurTabIndex);

        // Restore the focus to the previous tab
        TabCtrl_SetCurFocus (hWndTC, iTabIndex - 1);

        // If there was an outgoing tab, ...
        if (lpMemPTDOld NE NULL)
            // Show the child windows of the outgoing tab
            ShowHideChildWindows (lpMemPTDOld->hWndMC, TRUE);
    } // End IF
NORMAL_EXIT:
    // Destroy any windows we might have created
    if (lpMemPTD NE NULL && lpMemPTD->hWndMC NE NULL)
    {
        if (lpMemPTD->hWndDB NE NULL)
        {
            SendMessageW (lpMemPTD->hWndMC, WM_MDIDESTROY, (WPARAM) (lpMemPTD->hWndDB), 0); lpMemPTD->hWndDB = NULL;
        } // End IF

        if (lpMemPTD->hWndSM NE NULL)
        {
            SendMessageW (lpMemPTD->hWndMC, WM_MDIDESTROY, (WPARAM) (lpMemPTD->hWndSM), 0); lpMemPTD->hWndSM = NULL;
        } // End IF

        DestroyWindow (lpMemPTD->hWndMC); lpMemPTD->hWndMC = NULL;
    } // End IF

    if (csSM.hGlbDPFE NE NULL)
    {
        // Free the storage for the workspace DPFE global memory
        DbgGlobalFree (csSM.hGlbDPFE); csSM.hGlbDPFE = NULL;
    } // End IF

    if (lpMemPTD NE NULL)
    {
        // Destroy the PerTabData vars
        DESTROY_PERTABVARS

        // We no longer need this storage
        MyVirtualFree (lpMemPTD, 0, MEM_RELEASE); lpMemPTD = NULL;
    } // End IF

    // Get ptr to PerTabData global memory
    lpMemPTD = GetPerTabPtr (TabCtrl_GetCurSel (hWndTC)); // Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Ensure it's a valid ptr
    if (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)))
        // Ensure the SM has the focus
        PostMessageW (lpMemPTD->hWndSM, MYWM_SETFOCUS, 0, 0);

    return bRet;
} // End CreateNewTabInThread
#undef  APPEND_NAME


//***************************************************************************
//  $GetPerTabPtr
//
//  Return ptr to PerTabData global memory
//***************************************************************************

LPPERTABDATA GetPerTabPtr
    (APLU3264 iTab)                     // Tab

{
    TC_ITEMW tcItem = {0};              // TabCtrl item struc

    // We want lParam only
    //   but in order to overcome a problem in WIN64,
    //   we need to ask for the image index, too.
    tcItem.mask       = TCIF_IMAGE | TCIF_PARAM;
////tcItem.mask       = TCIF_PARAM;
////tcItem.pszText    =
////tcItem.cchTextMax =
////tcItem.iImage     =
////tcItem.lParam     =

    // Get the lParam value (lpMemPTD)
    SendMessageW (hWndTC, TCM_GETITEMW, iTab, (LPARAM) &tcItem);

    // Return the ptr
    return (LPPERTABDATA) tcItem.lParam;
} // End GetPerTabPtr


//***************************************************************************
//  $LclTabCtrlWndProc
//
//  Local window procedure for the Tab Control
//***************************************************************************

LRESULT WINAPI LclTabCtrlWndProc
    (HWND   hWnd,       // Window handle
     UINT   message,    // Type of message
     WPARAM wParam,     // Additional information
     LPARAM lParam)     // ...

{
    TC_HITTESTINFO tcHit;
    static UBOOL   bCaptured = FALSE;
    POINT          ptScr;
    HMENU          hMenu;
    UINT           uCloseState,
                   uOverTabState;
#if 0
    int            iTmpTabIndex;
#endif
    LPPERTABDATA   lpMemPTD;        // Ptr to PerTabData global memory

////LCLODSAPI ("TC: ", hWnd, message, wParam, lParam);
    // Split cases
    switch (message)
    {
////    case WM_ERASEBKGND:
////        // In order to reduce screen flicker, we handle erase background
////        // in the WM_PAINT message for the individual tabs
////        return TRUE;            // We erased the background
//// #if 0
////         case WM_MOUSEMOVE:          // fwKeys = wParam;         // Key flags
////                                     // xPos = LOSHORT (lParam); // Horizontal position of cursor in CA
////                                     // yPos = HISHORT (lParam); // Vertical position of cursor  in CA
////             if (!bCaptured)
////             {
////                 SetCapture (hWnd);
////                 bCaptured = TRUE;
////             } // End IF
////
////             // Save the client coordinates
////             tcHit.pt.x = LOSHORT (lParam);
////             tcHit.pt.y = HISHORT (lParam);
////
////             // Ask the Tab Control if we're over a tab
////             iTmpTabIndex = TabCtrl_HitTest (hWnd, &tcHit);
////
////             // Ensure we're over a tab
////             if (iTmpTabIndex EQ -1)
////             {
////                 SendMessageW (hWnd, WM_MOUSELEAVE, wParam, lParam);
////                 ReleaseCapture ();
////                 bCaptured = FALSE;
////
////                 break;
////             } // End IF
////
////             // If it's not the same tab, restore the state of the previous one
////             if (iTmpTabIndex NE gOverTabIndex)
////                 SendMessageW (hWnd, WM_MOUSELEAVE, wParam, lParam);
////
////             gOverTabIndex = iTmpTabIndex;
////
////             // Draw the tab with the text highlighted
////             SetTabTextState (gOverTabIndex, TRUE);
////             InvalidateRect (hWnd, NULL, FALSE);
////
////             break;
////
////         case WM_MOUSELEAVE:
////             // If the tab index is invalid, ignore this message
////             if (gOverTabIndex EQ -1)
////                 break;
////
////             // Draw the tab with the text normal
////             SetTabTextState (gOverTabIndex, FALSE);
////             InvalidateRect (hWnd, NULL, FALSE);
////
////             break;
//// #endif
        case WM_RBUTTONDOWN:                // fwKeys = wParam;         // Key flags
                                            // xPos = LOSHORT (lParam); // Horizontal position of cursor
                                            // yPos = HISHORT (lParam); // Vertical position of cursor
        {
            UBOOL bExecuting;               // TRUE iff we're waiting for an execution to complete

            if (bCaptured)
            {
                SendMessageW (hWnd, WM_MOUSELEAVE, wParam, lParam);
                ReleaseCapture ();
                bCaptured = FALSE;
            } // End IF

            // Save the client coordinates
            tcHit.pt.x = LOSHORT (lParam);
            tcHit.pt.y = HISHORT (lParam);

            // Ask the Tab Control if we're over a tab
            gOverTabIndex = TabCtrl_HitTest (hWnd, &tcHit);

            // Get the mouse position in screen coordinates
            GetCursorPos (&ptScr);

            // Create a popup menu
            hMenu = CreatePopupMenu ();

            // If we're not over a tab, disallow any actions
            //   which require a valid tab index
            uOverTabState = (gOverTabIndex EQ -1)
                          ? MF_GRAYED
                          : MF_ENABLED;

            // Get ptr to outgoing PerTabData global memory
            lpMemPTD = GetPerTabPtr (gOverTabIndex); // Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

            // If it's invalid, we're closing out the last tab, so ignore
            if (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)))
                // Get the executing state flag
                bExecuting = lpMemPTD->bExecuting;
            else
                bExecuting = FALSE;

            // If we're not over a tab or this is the last open tab,
            //   or this tab is executing,
            //   disallow closing the WS
            uCloseState = (gOverTabIndex EQ -1
                        || TabCtrl_GetItemCount (hWnd) EQ 1
                        || bExecuting)
                        ? MF_GRAYED
                        : MF_ENABLED;

            AppendMenuW (hMenu,                 // Handle
                         uOverTabState
                       | MF_STRING,             // Flags
                         IDM_NEW_WS,
                         L"&New WS");
            AppendMenuW (hMenu,                 // Handle
                         MF_GRAYED
                       | MF_STRING,             // Flags
                         IDM_DUP_WS,
                         L"&Duplicate WS");
            AppendMenuW (hMenu,                 // Handle
                         uCloseState
                       | MF_STRING,             // Flags
                         IDM_SAVECLOSE_WS,
                         L"&Save and Close WS");
            AppendMenuW (hMenu,                 // Handle
                         uCloseState
                       | MF_STRING,             // Flags
                         IDM_CLOSE_WS,
                         L"&Close WS");

            TrackPopupMenu (hMenu,              // Handle
                            0                   // Flags
                          | TPM_CENTERALIGN
                          | TPM_RIGHTBUTTON
                            ,
                            ptScr.x,            // x-position
                            ptScr.y,            // y-position
                            0,                  // Reserved (must be zero)
                            hWndMF,             // Handle of owner window
                            NULL);              // Dismissal area outside rectangle (none)
            // Free the menu resources
            DestroyMenu (hMenu);

            break;
        } // End WM_RBUTTONDOWN

        case WM_LBUTTONUP:                  // fwKeys = wParam;         // Key flags
                                            // xPos = LOSHORT (lParam); // Horizontal position of cursor
                                            // yPos = HISHORT (lParam); // vertical position of cursor
            // If the user clicked on the icon, close the tab
            if (ClickOnClose ())
                CloseTab (gOverTabIndex);
            else
            {
                // Save the client coordinates
                tcHit.pt.x = LOSHORT (lParam);
                tcHit.pt.y = HISHORT (lParam);

                // Ask the Tab Control if we're over a tab
                gOverTabIndex = TabCtrl_HitTest (hWnd, &tcHit);

                // If we're over a tab, ...
                if (gOverTabIndex NE -1)
                {
                    // Get ptr to outgoing PerTabData global memory
                    lpMemPTD = GetPerTabPtr (gOverTabIndex); // Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

                    // If it's valid, ...
                    if (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)))
                        // Tell the Master Frame about the current lpMemPTD
                        PostThreadMessage (dwMainThreadId, MYWM_LPMEMPTD, 0, (LPARAM) lpMemPTD);
                } // End IF
            } // End IF/ELSE

            break;

        case TCM_DELETEITEM:                    // itemID = (int) wParam;
                                                // 0 = lParam;
        {
            int     iNewTabIndex,               // Index of new tab (after deleting this one)
                    iDelTabIndex;               // Index of tab to delete
            LRESULT lResult;                    // Result from CallWindowProcW
            DWORD   dwThreadId;                 // Outgoing thread ID
            HWND    hWndFENxt;                  // Next Function Editing window handle

            // Save the tab index to delete
            iDelTabIndex = (int) wParam;

            // Get ptr to outgoing PerTabData global memory
            lpMemPTD = GetPerTabPtr (iDelTabIndex); // Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

            // If it's invalid, we're closing out the last tab, so just quit
            if (!IsValidPtr (lpMemPTD, sizeof (lpMemPTD)))
                break;

            // If this tab is still executing, ...
            if (lpMemPTD->bExecuting)
            {
                MBW (L"Can't close tab -- still executing.  First, use Ctrl-Break to stop execution.");

                // Ignore this action
                return FALSE;
            } // End IF

            // While there are open FE windows, ...
            while (hWndFENxt = lpMemPTD->hWndFENxt)
            {
                // If the user doesn't want the window to close, ...
                if (!CloseFunction (hWndFENxt))
                    // Ignore this action
                    return FALSE;

                Assert (hWndFENxt NE lpMemPTD->hWndFENxt);
            } // End WHILE

            // If gOverTabIndex is this tab or to the right of it, ...
            if (gOverTabIndex && gOverTabIndex >= iDelTabIndex)
                // Decrement it
                gOverTabIndex--;

            // Reset this tab's color index bit
            ResetTabColorIndex (lpMemPTD->crIndex);

            // Save the outgoing thread ID
            dwThreadId = lpMemPTD->dwThreadId;

            // The storage for lpMemPTD is freed in CreateNewTabInThread

            // Call the original window proc so it can delete
            //   the tab and we can get the new current selection
            lResult =
              CallWindowProcW (lpfnOldTabCtrlWndProc,
                               hWnd,
                               message,
                               wParam,
                               lParam); // Pass on down the line
#ifdef DEBUG
            // Zap the ptr
            lpMemPTD = NULL;
#endif
            // Set as new tab index
            iNewTabIndex = gOverTabIndex;

            // Select the new tab
            TabCtrl_SetCurSel (hWnd, iNewTabIndex);

            // Call common code to show/hide the tab windows
            TabCtrl_SelChange ();

            // Save as new tab ID
            gCurTabID = TranslateTabIndexToID (iNewTabIndex);

            // Continue only if the tab ID is valid
            if (gCurTabID NE -1)
            {
                // Get ptr to outgoing PerTabData global memory
                lpMemPTD = GetPerTabPtr (TranslateTabIDToIndex (gCurTabID)); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

                // Give the new tab the focus
                PostMessageW (lpMemPTD->hWndSM, MYWM_SETFOCUS, 0, 0);
            } // End IF

            // Tell the thread to quit
            PostThreadMessageW (dwThreadId, WM_QUIT, 0, 0);

            return lResult;
        } // End TCM_DELETEITEM
    } // End SWITCH

    return CallWindowProcW (lpfnOldTabCtrlWndProc,
                            hWnd,
                            message,
                            wParam,
                            lParam); // Pass on down the line
} // End LclTabCtrlWndProc


//***************************************************************************
//  $FreeGlobalStorage
//
//  Free the global storage in the symbol table for a given PerTabData struc
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FreeGlobalStorage"
#else
#define APPEND_NAME
#endif

void FreeGlobalStorage
    (LPPERTABDATA lpMemPTD)             // Ptr to PerTabData global memory

{
    LPHSHTABSTR lphtsPTD;               // Loop var
    LPSYMENTRY  lpSymEntry;             // ...
    LPHSHENTRY  lpHshEntry;             // ...

    // If there's a pending wait event, ...
    if (lpMemPTD->hWaitEvent)
    {
        SetEvent (lpMemPTD->hWaitEvent); lpMemPTD->hWaitEvent = NULL;
    } // End IF

    // Free global storage for Native File functions
    if (lpMemPTD->hGlbNfns)
    {
        LPNFNSHDR    lpNfnsHdr;         // Ptr to NFNSHDR global memory
        LPNFNSDATA   lpNfnsMem;         // Ptr to aNfnsData
        UINT         uCnt;              // Loop counter

        // Lock the memory to get a ptr to it
        lpNfnsHdr = MyGlobalLockNfn (lpMemPTD->hGlbNfns);

        // Point to the first entry in use
        lpNfnsMem = &lpNfnsHdr->aNfnsData[lpNfnsHdr->offFirstInuse];

        // Loop through the in use tie numbers
        for (uCnt = 0; uCnt < lpNfnsHdr->nTieNums; uCnt++)
        {
            // Release these resources
            NfnsReleaseOneResource (lpNfnsMem);

            // Point to the next tie number in use
            lpNfnsMem = &lpNfnsHdr->aNfnsData[lpNfnsMem->offNextInuse];
        } // End FOR

        // We no longer need this ptr
        MyGlobalUnlock (lpMemPTD->hGlbNfns); lpNfnsHdr = NULL;

        // We no longer need this storage
        DbgGlobalFree (lpMemPTD->hGlbNfns); lpMemPTD->hGlbNfns = NULL;
    } // End IF

    // Free global storage if the SymTab is valid
    if (lpMemPTD->lphtsPTD
     && lpMemPTD->lphtsPTD->lpSymTab)
    {
        // If there's something suspended, ...
        if (lpMemPTD->lpSISCur)
            // Create a new thread
            CreateResetThread (lpMemPTD);

        // Get a ptr to the HTS
        lphtsPTD = lpMemPTD->lphtsPTD;

        // Loop through the STEs
        for (lpSymEntry = lphtsPTD->lpSymTab;
             lpSymEntry < lphtsPTD->lpSymTabNext;
             lpSymEntry++)
        if (lpSymEntry->stFlags.Inuse
         && lpSymEntry->stFlags.Value
         && lpSymEntry->stFlags.Imm EQ FALSE)
        {
            HGLOBAL hGlbData;               // User-defined function/operator global memory handle

            // Get the global memory handle
            hGlbData = lpSymEntry->stData.stGlbData;

            // If it's a Magic Function/Operator, ...
            if (lpSymEntry->stFlags.ObjName EQ OBJNAME_MFO)
            {
                LPDFN_HEADER lpMemDfnHdr;       // Ptr to user-defined function/operator struc

                // If the handle is valid, ...
                if (hGlbData)
                {
                    // Lock the memory to get a ptr to it
                    lpMemDfnHdr = MyGlobalLockDfn (hGlbData);

                    // Free the globals in the struc
                    FreeResultGlobalDfnStruc (lpMemDfnHdr, TRUE);

                    // Free the HshTab & SymTab
                    FreeHshSymTabs (&lpMemDfnHdr->htsDFN, FALSE);

                    // Unlock and free (and set to NULL) a global name and ptr
                    UnlFreeGlbName (hGlbData, lpMemDfnHdr);
                }// End IF
            } else
            // Free all global fns and vars in the workspace
            if (hGlbData EQ NULL || FreeResultGlobalDFLV (hGlbData))
                // Erase the Symbol Table Entry
                //   even if it's a []var
                EraseSTE (lpSymEntry, TRUE);
        } // End FOR/IF

        // Loop through the HTEs to free the names
        for (lpHshEntry = lphtsPTD->lpHshTab;
             lpHshEntry NE &lphtsPTD->lpHshTab[lphtsPTD->iHshTabTotalNelm];
             lpHshEntry++)
        if (lpHshEntry->htFlags.Inuse
         && lpHshEntry->htGlbName NE NULL)
            DbgGlobalFree (lpHshEntry->htGlbName);

        // Zap the SymTab ptr so we don't re-execute this code
        lpMemPTD->lphtsPTD->lpSymTab = NULL;
    } // End IF

    // If it's valid, ...
    if (lpMemPTD->hGlbQuadEM)
    {
        // Free the old value of []EM
        FreeResultGlobalVar (lpMemPTD->hGlbQuadEM); lpMemPTD->hGlbQuadEM = NULL;
    } // End IF
} // End FreeGlobalStorage
#undef  APPEND_NAME


//***************************************************************************
//  $CreateResetThread
//
//  Create a Reset thread
//***************************************************************************

void CreateResetThread
    (LPPERTABDATA lpMemPTD)         // Ptr to PerTabData global memory

{
    // Create a new thread
    crThread.hThread =
      CreateThread (NULL,                   // No security attrs
                    0,                      // Use default stack size
                   &CreateResetInThread,    // Starting routine
                   &crThread,               // Param to thread func
                    CREATE_SUSPENDED,       // Creation flag
                   &crThread.dwThreadId);   // Returns thread id
    // Save the thread struc values
    crThread.lpMemPTD = lpMemPTD;

    if (crThread.hThread)
        ResumeThread (crThread.hThread);
} // End CreateResetThread


//***************************************************************************
//  $CreateResetInThread
//
//  Create a )RESET within a thread
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CreateResetInThread"
#else
#define APPEND_NAME
#endif

UBOOL WINAPI CreateResetInThread
    (LPCR_THREAD lpcrThread)

{
////HANDLE       hThread;           // Handle to this thread
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

    // Extract values from the arg struc
////hThread  = lpcrThread->hThread;
    lpMemPTD = lpcrThread->lpMemPTD;

    // Reset the SI stack

    // If the current thread's lpMemPTD is NULL, ...
    if (TlsGetValue (dwTlsPerTabData) EQ NULL)
        TlsSetValue (dwTlsPerTabData, lpMemPTD);

    // Create a semaphore for ourselves
    lpMemPTD->hExitphore =
      MyCreateSemaphoreW (NULL,         // No security attrs
                          0,            // Initial count (non-signalled)
                          64*1024,      // Maximum count
                          NULL);        // No name
    // Call )RESET
    CmdReset_EM (L"");

    // Wait for the semaphore to trigger
    MyWaitForSemaphore (lpMemPTD->hExitphore,   // Ptr to handle to wait for
                        INFINITE,               // Timeout value in milliseconds
                       L"CreateResetInThread"); // Caller identification
    // Close the semaphore handle as it isn't used anymore
    MyCloseSemaphore (lpMemPTD->hExitphore); lpMemPTD->hExitphore = NULL;

    return TRUE;
} // End CreateResetInThread
#undef  APPEND_NAME


//***************************************************************************
//  $ClickOnClose
//
//  Did the user click on a close button?
//***************************************************************************

UBOOL ClickOnClose
    (void)

{
    TC_HITTESTINFO  tcHit;
    RECT            rcTab;

    // Get the mouse position in screen coordinates
    GetCursorPos (&tcHit.pt);

    // Convert from screen to window coordinates
    ScreenToClient (hWndTC, &tcHit.pt);

    // Ask the Tab Control if we're over a tab
    gOverTabIndex = TabCtrl_HitTest (hWndTC, &tcHit);

    // If we're not over a tab, quit
    if (gOverTabIndex EQ -1)
        return FALSE;

    // If there's only one tab, don't check the close button
    if (TabCtrl_GetItemCount (hWndTC) EQ 1)
        return FALSE;

    // Get the item's bounding rectangle
    TabCtrl_GetItemRect (hWndTC, gOverTabIndex, &rcTab);

    // Remove the border from the rectangle
    AdjustTabRect (&rcTab, gOverTabIndex, FALSE);

    // Transform the bounding rectangle into an image rectangle
    GetImageRect (&rcTab);

    // If the point is on the button, return so
    return PtInRect (&rcTab, tcHit.pt);
} // End ClickOnClose


//***************************************************************************
//  $CloseTab
//
//  Close a given tab
//***************************************************************************

UBOOL CloseTab
    (int iTabIndex)             // Tab index

{
    // Close the tab
    return (TabCtrl_DeleteItem (hWndTC, iTabIndex) NE -1);
} // End CloseTab


//***************************************************************************
//  $SetTabTextState
//
//  Set a given tab's text state to highlight or normal
//***************************************************************************

void SetTabTextState
    (int  iCurTab,      // Index # of current tab
     UBOOL bHighlight)  // TRUE iff to be drawn as highlighted

{
    LPPERTABDATA lpMemPTD;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetPerTabPtr (iCurTab); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Save the text state
    lpMemPTD->bTabTextState = bHighlight;
} // End SetTabTextState


//***************************************************************************
//  $FillTabBackground
//
//  Fill in a tab's background
//***************************************************************************

void FillTabBackground
    (HDC      hDC,
     LPRECT   lpRect,
     COLORREF crTop,
     COLORREF crBot)

{
    HBRUSH   hBrush;
    int      i, nBands;
    RECT     rcBand;
    COLORREF crBand;
    float    fRStep, fGStep, fBStep;

    // Calculate the # bands
    nBands = lpRect->bottom - lpRect->top;

    fRStep = ((float) (GetRValue (crBot) - GetRValue (crTop))) / (float) nBands;
    fGStep = ((float) (GetGValue (crBot) - GetGValue (crTop))) / (float) nBands;
    fBStep = ((float) (GetBValue (crBot) - GetBValue (crTop))) / (float) nBands;

    // Fill the rectangle with one band
    SetRect (&rcBand,
              lpRect->left,
              nBands + lpRect->top - 1,
              lpRect->right + 1,
              nBands + lpRect->top);
    // Loop through all bands
    for (i = 0; i < nBands; i++)
    {
        crBand = RGB ((int) (((float) GetRValue (crBot)) - fRStep * i),
                      (int) (((float) GetGValue (crBot)) - fGStep * i),
                      (int) (((float) GetBValue (crBot)) - fBStep * i));
        hBrush = MyCreateSolidBrush (crBand);
        FillRect (hDC, &rcBand, hBrush);
        MyDeleteObject (hBrush); hBrush = NULL;

        // Back off by one band
        rcBand.top--;
        rcBand.bottom--;
    } // End FOR
} // End FillTabBackground


//***************************************************************************
//  $GetNextTabColorIndex
//
//  Get the next available tab color index.
//***************************************************************************

int GetNextTabColorIndex
    (void)

{
    int i, j;

    // Search for the next available color index
    for (i = 0; i < countof (crIndices); i++)
        // If not all in use
        if (crIndices[i] NE 0xFF)
            // Search for the available bit within this byte
            for (j = 0; j < NBIB; j++)
                // If it's this bit, ...
                if (!(crIndices[i] & (BIT0 << j)))
                {
                    crIndices[i] |= BIT0 << j;  // Set the bit
                    return j + i * NBIB;        // Return its index
                } // End IF
    return -1;
} // End GetNextTabColorIndex


//***************************************************************************
//  $GetTabColorIndex
//
//  Get a given tab's color index
//***************************************************************************

int GetTabColorIndex
    (APLU3264 iTab)                     // Tab index

{
    LPPERTABDATA lpMemPTD;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetPerTabPtr (iTab); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Return the color index
    return lpMemPTD->crIndex;
} // End GetTabColorIndex


//***************************************************************************
//  $ResetTabColorIndex
//
//  Reset a given tab's color index bit
//***************************************************************************

void ResetTabColorIndex
    (int crIndex)                       // Color index

{
    // Reset this tab's color index bit
    crIndices[crIndex >> LOG2NBIB] &= ~(BIT0 << (crIndex % NBIB));
} // End ResetTabColorIndex


//***************************************************************************
//  $AdjustTabRect
//
//  Adjust a tab's bounding rectangle to avoid writing
//    over (or under) the border.
//***************************************************************************

void AdjustTabRect
    (LPRECT lpRect,         // Ptr to rectangle to adjust
     int    iTabIndex,      // Tab index
     UBOOL  bForce)         // TRUE iff we're to force adjustment

{
    // If this is the current tab, adjust by one pixel
    if (bForce || iTabIndex EQ TabCtrl_GetCurSel (hWndTC))
    {
        lpRect->left   += 1;
        lpRect->top    += 1;
        lpRect->right  -= 1;
        lpRect->bottom -= 1;
    } // End IF
} // End AdjustTabRect


//***************************************************************************
//  $DrawTab
//
//  Draw the tab
//***************************************************************************

void DrawTab
    (HDC      hDC,                  // Handle to device context
     int      iCurTab,              // Index of current tab
     LPRECT   lpRect)               // Ptr to surrounding rectangle

{
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPAPLCHAR    lpMemWSID;         // Ptr to []WSID global memory
    int          crIndex;           // Index into crTab for this tab
#ifdef DRAWTABNUM
    int          tabNum;            // Tab # (origin-1)
    HBRUSH       hBrushOrig;        // Original brush from the DC so we can restore it
    HPEN         hPenOrig,          // ...      pen   ...
                 hPenEllipse;       // Actual pen for the ellipse boundary
    WCHAR        wszTabNum[3 + 1];  // Format area for tab # (including terminating zero)
    RECT         rcTabNum;          // Rectangle for tab #
#endif
    COLORREF     crfg,              // Foreground color
                 crbk;              // Background color
#if 0
    HDC          hDCMem;            // Handle to memory device context
    HBITMAP      hBitmap,           // Handle to bitmap
                 hBitmapOld;        // Handle to old bitmap
#endif

    // Get ptr to PerTabData global memory
    lpMemPTD = GetPerTabPtr (iCurTab); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Get the tab's color index
    crIndex = lpMemPTD->crIndex % NUM_CRTABS;

    // Get the tab's background color
    crbk = crTab[crIndex].bk;

    // Get the tab text's foreground color
    if (lpMemPTD->bTabTextState)
        crfg = crTab[crIndex].hl;
    else
        crfg = crTab[crIndex].fg;

    SetAttrs (hDC, GetFSIndFontHandle (FONTENUM_TC), crfg, crbk);

#if 0
    // Create a compatible DC and bitmap
    hDCMem = MyCreateCompatibleDC (hDC);
    hBitmap = MyCreateCompatibleBitmap (hDC,
                                        lpRect->right,
                                        lpRect->bottom);
    hBitmapOld = SelectObject (hDCMem, hBitmap);
    SetAttrs (hDCMem, GetFSIndFontHandle (FONTENUM_TC), crfg, crbk);
#endif

#define hDCMem  hDC

    // Remove the border from the rectangle
    AdjustTabRect (lpRect, iCurTab, FALSE);

    // Fill the background of the tab
    FillTabBackground (hDCMem, lpRect, crbk, DEF_SCN_WHITE);

    // Draw transparently so the background shows through
    SetBkMode (hDCMem, TRANSPARENT);

    // Get a ptr to the ws name
    lpMemWSID = PointToWsName (iCurTab);

    // Account for the tab left margin
    lpRect->left  += TABMARGIN_LEFT;
    lpRect->right += TABMARGIN_LEFT;

#ifdef DRAWTABNUM
    // The following code draws tab #s inside an appropriately
    //   sized ellipse.  It superseded the code that relied
    //   upon Font Fallback (although I didn't know it at the
    //   time) using the TABNUMER_START characters from 1 to 20
    //   within an ellipse.  When I tried the program on a system
    //   without Font Fallback (Uniscribe) installed, I then
    //   realized the dependency, but didn't know how to enable
    //   Font Fallback on each machine.  I then coded all this
    //   stuff and got it working.  Then I found out how to
    //   enable Font Fallback (which I prefer), so I commented
    //   out all this code.

    // Get the tab # (origin-1)
    tabNum = 1 + lpMemWSID[0] - TABNUMBER_START;

    // Format the tab # as text
    MySprintfW (wszTabNum,
                sizeof (wszTabNum),
               L"%d",
                tabNum);
    // Create the bounding rectangle for the tab #
    rcTabNum.top    = lpRect->top + 1;
    rcTabNum.left   = lpRect->left;
    rcTabNum.right  =
    rcTabNum.bottom = 0;

    // Calculate the size of the bounding rectangle
    DrawTextW (hDCMem,
               wszTabNum,
               lstrlenW (wszTabNum),
              &rcTabNum,
               0
             | DT_SINGLELINE
             | DT_CENTER
             | DT_VCENTER
             | DT_NOPREFIX
             | DT_NOCLIP
             | DT_CALCRECT
              );
    // Set the bottom to the actual bottom
    rcTabNum.bottom = lpRect->bottom - 1;

#define PEN_WIDTH   2

    // Account for the pen width when drawing the ellipse
    rcTabNum.left   -= PEN_WIDTH;
    rcTabNum.top    -= PEN_WIDTH;
    rcTabNum.right  += PEN_WIDTH;
////rcTabNum.bottom += PEN_WIDTH;       // Already set

    // Save the existing brush so we can replace it with a white brush
    //   to provide a contrasting background
    hBrushOrig = GetCurrentObject (hDC, OBJ_BRUSH);
    SelectObject (hDC, GetStockObject (WHITE_BRUSH));

    // Save the existing pen so we can set one with the proper attributes
    hPenOrig = GetCurrentObject (hDC, OBJ_PEN);
    hPenEllipse = MyCreatePen (PS_INSIDEFRAME, PEN_WIDTH, DEF_SCN_BLACK);
    SelectObject (hDC, hPenEllipse);

    // Draw the ellipse
    Ellipse (hDCMem,
             rcTabNum.left,
             rcTabNum.top,
             rcTabNum.right,
             rcTabNum.bottom);
    // Restore the original brush & pen
    SelectObject (hDC, hBrushOrig);
    SelectObject (hDC, hPenOrig);

    // Delete the resources
    MyDeleteObject (hPenEllipse); hPenEllipse = NULL;

    // Account for the pen width when drawing the tab #
    rcTabNum.left   += PEN_WIDTH;
    rcTabNum.top    += PEN_WIDTH;
    rcTabNum.right  -= PEN_WIDTH;
////rcTabNum.bottom -= PEN_WIDTH;       // Already set
#undef  PEN_WIDTH

    // Draw the tab # inside the ellipse
    DrawTextW (hDCMem,
               wszTabNum,
               lstrlenW (wszTabNum),
              &rcTabNum,
               0
             | DT_SINGLELINE
             | DT_CENTER
             | DT_VCENTER
             | DT_NOPREFIX
             | DT_NOCLIP
              );
    // Move the bounding rectangle over past the ellipse
    lpRect->left += rcTabNum.right - rcTabNum.left;
#endif
    // Draw the text
    DrawTextW (hDCMem,
#ifdef DRAWTABNUM
              &lpMemWSID[1],
               lstrlenW (&lpMemWSID[1]),
#else
              &lpMemWSID[0],
               lstrlenW (&lpMemWSID[0]),
#endif
               lpRect,
               0
             | DT_SINGLELINE
             | DT_VCENTER
             | DT_NOPREFIX
             | DT_NOCLIP
              );
    // Restore the original values
    lpRect->right -= TABMARGIN_LEFT;
    lpRect->left  -= TABMARGIN_LEFT;

    // If there's only one tab, don't draw the close button
    if (TabCtrl_GetItemCount (hWndTC) NE 1)
    {
        RECT rcImage;

        // Copy the original rectangle
        rcImage = *lpRect;

        // Get the image rectangle
        GetImageRect (&rcImage);

        // Draw transparently
        ImageList_SetBkColor (hImageListTC, CLR_NONE);

        // Draw the image
        ImageList_Draw (hImageListTC,       // Handle to the image list
                        0,                  // Index of the image to draw
                        hDCMem,             // Handle of the DC
                        rcImage.left,       // X-coordinate
                        rcImage.top,        // Y-...
                        0                   // Styles
                      | ILD_TRANSPARENT
                        );
    } // End IF

#if 0
    // Copy the memory DC to the screen DC
    BitBlt (hDC,
            lpRect->left,
            lpRect->top,
            lpRect->right  - lpRect->left,
            lpRect->bottom - lpRect->top,
            hDCMem,
            lpRect->left,
            lpRect->top,
            SRCCOPY);
    // Restore the old resources
    SelectObject (hDCMem, hBitmapOld);

    // We no longer need these resources
    MyDeleteObject (hBitmap); hBitmap = NULL;
    MyDeleteDC (hDCMem); hDCMem = NULL;
#endif
} // End DrawTab


//***************************************************************************
//  $PointToWsName
//
//  Skip over the path name to the workspace name
//***************************************************************************

LPAPLCHAR PointToWsName
    (APLU3264 iTabIndex)                // Tab index

{
    HGLOBAL      hGlbWSID;              // []WSID global memory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPAPLCHAR    lpMemWSID;             // Ptr to []WSID global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetPerTabPtr (iTabIndex); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Initialize with default value
    strcpyW (lpwszGlbTemp, L"  CLEAR WS");

    // If the []WSID STE has been setup, ...
    if (lpMemPTD->lphtsPTD
     && lpMemPTD->lphtsPTD->lpSymQuad
     && lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_WSID])
    {
        APLNELM aplNELMWSID;    // []WSID NELM
        APLRANK aplRankWSID;    // []WSID rank

        // Get the []WSID global memory handle
        hGlbWSID = lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_WSID]->stData.stGlbData;

        // If the []WSID STE has been setup, ...
        if (hGlbWSID)
        {
            // Lock the memory to get a ptr to it
            lpMemWSID = MyGlobalLockVar (hGlbWSID);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemWSID)
            // Get the NELM and rank
            aplNELMWSID = lpHeader->NELM;
            aplRankWSID = lpHeader->Rank;
#undef  lpHeader

            // If []WSID is non-empty, ...
            if (aplNELMWSID)
            {
                LPAPLCHAR p, q;             // Temporary ptrs

                // Skip over the header to the data
                lpMemWSID = VarArrayDataFmBase (lpMemWSID);

                // Skip over the path
                q = lpMemWSID;
                while (p = strchrW (q, WC_SLOPE))
                    q = p + 1;

                // Copy to global temporary storage
                strcpynW (&lpwszGlbTemp[2], q, (UINT) ((lpMemWSID + aplNELMWSID + 1) - q));

                // Include the separator
                lpwszGlbTemp[1] = L' ';
            } // End IF
        } // End IF
    } else
        // Mark as invalid
        hGlbWSID = NULL;

    if (hGlbWSID)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbWSID); lpMemWSID = NULL;
    } // End IF

    // Prepend the tab index
    lpwszGlbTemp[0] = TABNUMBER_START + GetTabColorIndex (iTabIndex);

    return lpwszGlbTemp;
} // End PointToWsName


//***************************************************************************
//  $GetImageRect
//
//  Get the bounding rectangle for an image right-adjusted horizontally
//    and centered vertically inside the given rectangle.
//***************************************************************************

void GetImageRect
    (LPRECT lpRect)

{
    lpRect->left   = lpRect->right - IMAGE_WIDTH;
    lpRect->top    = lpRect->top
                   + (lpRect->bottom - lpRect->top - IMAGE_HEIGHT) / 2,
    lpRect->bottom = lpRect->top + IMAGE_HEIGHT;
} // End GetImageRect


//***************************************************************************
//  $NewTabName
//
//  Tell the tab ctrl about a new workspace name
//***************************************************************************

void NewTabName
    (LPAPLCHAR lpMemWSID)       // Ptr to new workspace name (may be NULL)

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    int          iCurTabIndex;  // Index of the current tab
    TC_ITEMW     tcItem = {0};  // TabCtrl item struc

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the tab index
    iCurTabIndex = TranslateTabIDToIndex (lpMemPTD->CurTabID);

    // If the arg is a WSID, ...
    if (lpMemWSID)
    {
        LPAPLCHAR p;            // Temp ptr

        // Skip over the path
        while (p = strchrW (lpMemWSID, WC_SLOPE))
            lpMemWSID = p + 1;
    } else
        lpMemWSID = PointToWsName (iCurTabIndex);

    // Initialize the item struc
    tcItem.mask       = TCIF_TEXT;
    tcItem.pszText    = lpMemWSID;
////tcItem.cchTextMax =
////tcItem.iImage     =
////tcItem.lParam     =

    // Tell the Tab Ctrl about the new name
    SendMessageW (hWndTC, TCM_SETITEMW, iCurTabIndex, (LPARAM) &tcItem);
} // End NewTabName


//***************************************************************************
//  $IsCurTabActive
//
//  Return TRUE iff the current tab is the active tab
//***************************************************************************

UBOOL IsCurTabActive
    (void)

{
    // Compare lpMemPTD from the Tab Index and from the thread
    return (GetPerTabPtr (TranslateTabIDToIndex (gCurTabID))
                       EQ TlsGetValue (dwTlsPerTabData));
} // End IsCurTabActive


//***************************************************************************
//  End of File: tabctrl.c
//***************************************************************************
