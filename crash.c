//***************************************************************************
//  NARS2000 -- Crash Handling
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
#include "headers.h"


// Save area for old CC ListBox window procedure address
WNDPROC lpfnOldCCListboxWndProc;


//***************************************************************************
//  $CC_Create
//
//  Perform window-specific initialization
//***************************************************************************

void CC_Create
    (HWND hWnd)

{
} // End CC_Create


//***************************************************************************
//  $CC_Delete
//
//  Perform window-specific uninitialization
//***************************************************************************

void CC_Delete
    (HWND hWnd)

{
} // End CC_Delete


//***************************************************************************
//  $CCWndProc
//
//  Message processing routine for the Crash window
//***************************************************************************

LRESULT APIENTRY CCWndProc
    (HWND   hWnd,       // Window handle
     UINT   message,    // Type of message
     WPARAM wParam,     // Additional information
     LPARAM lParam)     // ...

{

////LCLODSAPI ("CC: ", hWnd, message, wParam, lParam);
    switch (message)
    {
        case WM_CREATE:
            // Create an Edit Control to fit inside this window
            hWndCC_LB =
              CreateWindowExW (0L,                  // Extended styles
                               WC_LISTBOXW,         // Class name
                               L"Crash Control Listbox",// For debugging only
                               0
                             | WS_CHILD
                             | WS_VSCROLL
                             | WS_HSCROLL
                             | WS_CLIPCHILDREN
                             | LBS_NOINTEGRALHEIGHT
                             | LBS_EXTENDEDSEL
                             | LBS_MULTIPLESEL
                               ,                    // Styles
                               0,                   // X-position
                               0,                   // Y-...
                               CW_USEDEFAULT,       // Width
                               CW_USEDEFAULT,       // Height
                               hWnd,                // Parent window
                               (HMENU) IDWC_CC_LB,  // ID
                               _hInstance,          // Instance
                               0);                  // lParam
            // Subclass the List Box so we can handle
            //   WM_CONTEXTMENU
            lpfnOldCCListboxWndProc = (WNDPROC)
              SetWindowLongPtrW (hWndCC_LB,
                                 GWLP_WNDPROC,
                                 (APLU3264) (LONG_PTR) (WNDPROC) &LclCCListboxWndProc);
            // Initialize window-specific resources
            CC_Create (hWnd);

            break;

#define fwSizeType  wParam
#define nWidth      (LOWORD(lParam))
#define nHeight     (HIWORD(lParam))
        case WM_SIZE:               // fwSizeType = wParam;      // Resizing flag
                                    // nWidth = LOWORD(lParam);  // Width of client area
                                    // nHeight = HIWORD(lParam); // Height of client area
            if (fwSizeType NE SIZE_MINIMIZED)
                SetWindowPos (hWndCC_LB,        // Window handle to position
                              0,                // SWP_NOZORDER
                              0,                // X-position
                              0,                // Y-...
                              nWidth,           // Width
                              nHeight,          // Height
                              SWP_NOZORDER      // Flags
                            | SWP_SHOWWINDOW
                             );
            break;                  // Continue with next handler
#undef  nHeight
#undef  nWidth
#undef  fwSizeType

        case WM_SYSCOLORCHANGE:
        case WM_SETTINGCHANGE:
            // Uninitialize window-specific resources
            CC_Delete (hWnd);

            // Initialize window-specific resources
            CC_Create (hWnd);

            break;                  // Continue with next handler

        case WM_CONTEXTMENU:        // hwnd = (HWND) wParam;
                                    // xPos = LOSHORT (lParam); // Horizontal position of cursor
                                    // yPos = HISHORT (lParam); // Vertical position of cursor
        case LB_ADDSTRING:          // 0 = wParam;
                                    // lpsz = (LPCTSTR) lParam;
        case WM_SETFONT:            // hFont = (HFONT) wParam;
                                    // fRedraw = LOWORD (lParam);
        case WM_MOUSEWHEEL:
            // Pass these messages through to the Edit Control
            SendMessageW (hWndCC_LB, message, wParam, lParam);

            return FALSE;           // We handled the msg

        case MYWM_DISPMB:
            MBW (L"1.  Copy the data;  2.  When you press OK, the program will terminate.");

            return FALSE;           // We handled the msg

        case WM_CLOSE:
            DestroyWindow (hWnd);

            return FALSE;           // We handled the msg

        case WM_DESTROY:
            // Uninitialize window-specific resources
            CC_Delete (hWnd);

            // Tell the thread to quit, too
            PostQuitMessage (0);

            return FALSE;           // We handled the msg

        default:
            break;
    } // End SWITCH

////LCLODSAPI ("CCZ:", hWnd, message, wParam, lParam);
    return DefWindowProcW (hWnd, message, wParam, lParam);
} // End CCWndProc


//***************************************************************************
//  $LclCCListboxWndProc
//
//  Local window procedure for the Crash Control's LISTBOX
//***************************************************************************

LRESULT WINAPI LclCCListboxWndProc
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

////LCLODSAPI ("CCLB: ", hWnd, message, wParam, lParam);
    // Split cases
    switch (message)
    {
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

                    // We no longer need this resource
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

        case WM_CONTEXTMENU:                // hwnd = (HWND) wParam;
                                            // xPos = LOSHORT (lParam); // Horizontal position of cursor
                                            // yPos = HISHORT (lParam); // Vertical position of cursor
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
                         IDM_COPY,
                         L"&Copy");
            AppendMenuW (hMenu,                 // Handle
                         MF_ENABLED
                       | MF_STRING,             // Flags
                         IDM_SELECTALL,
                        L"Select &All");

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

            return FALSE;
    } // End SWITCH

////LCLODSAPI ("CCLBZ: ", hWnd, message, wParam, lParam);
    return CallWindowProcW (lpfnOldCCListboxWndProc,
                            hWnd,
                            message,
                            wParam,
                            lParam); // Pass on down the line
} // End LclCCListboxWndProc


//***************************************************************************
//  End of File: crash.c
//***************************************************************************
