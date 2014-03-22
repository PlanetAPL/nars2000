//***************************************************************************
//  NARS2000 -- Matrix Editor
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2009 Sudley Place Software

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


//***************************************************************************
//  $MEWndProc
//
//  Message processing routine for the Matrix Editor window
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MEWndProc"
#else
#define APPEND_NAME
#endif

LRESULT APIENTRY MEWndProc
    (HWND   hWnd,       // Window handle
     UINT   message,    // Type of message
     WPARAM wParam,     // Additional information
     LPARAM lParam)     // ...

{
////static DWORD aHelpIDs[] = {
////                           IDOK,             IDH_OK,
////                           IDCANCEL,         IDH_CANCEL,
////                           IDC_APPLY,        IDH_APPLY,
////                           IDC_HELP2,        IDH_HELP2,
////                           0,             0,
////                          };
////LCLODSAPI ("ME: ", hWnd, message, wParam, lParam);
    switch (message)
    {
        case WM_CREATE:
            DbgBrk ();


            break;



    } // End SWITCH

////LCLODSAPI ("MEZ:", hWnd, message, wParam, lParam);
    return DefMDIChildProcW (hWnd, message, wParam, lParam);
} // End MEWndProc
#undef  APPEND_NAME


//***************************************************************************
//  End of File: editmat.c
//***************************************************************************
