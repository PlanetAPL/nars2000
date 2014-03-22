//***************************************************************************
//  NARS2000 -- Version Routines
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2014 Sudley Place Software

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
#include <stdlib.h>
#include <winver.h>
#include "headers.h"


WCHAR wszVarFileInfo[] = WS_SLOPE L"VarFileInfo" WS_SLOPE L"Translation",
      lpwszVersion[]   = WS_APPNAME L" (" WS_WINSTR L") # %s";

HWND    hWndStatic;                 // Handle to static control
WNDPROC lpfnOldStaticWndProc;       // Save area for old Static Control procedure
extern HICON hIconAbout;
extern char  ecm_version[];
extern WCHAR crsh_dll[];
extern WCHAR crsh_version[];


//***************************************************************************
//  $LclFileVersionStrW
//
//  Get file version string
//***************************************************************************

void LclFileVersionStrW
    (LPWSTR lpwszFileName,
     LPWSTR wszFileVer)

{
    DWORD  dwVerHandle;     // Version file handle
    DWORD  dwVerSize;       // Size of the VERSIONINFO struc
    DWORD  dwTrans;         // Translation value
    LPWSTR lpwVer, lpwBuf;
    HLOCAL hLoc = NULL;
    UINT   cb;
    WCHAR  wszTemp[128];

    wszTemp[0] = '\0';      // Ensure properly terminated in case we fail
    dwVerSize = GetFileVersionInfoSizeW (lpwszFileName, &dwVerHandle);

    if (dwVerSize EQ 0)     // If it didn't work, ...
        return;

    // Allocate space for the file version info
    hLoc = LocalAlloc (LPTR, (long) dwVerSize);
    lpwVer = (LPWSTR) hLoc;
    if (lpwVer EQ NULL)     // If it didn't work
        return;

    // Read in the file version info
    if (!GetFileVersionInfoW (lpwszFileName, dwVerHandle, dwVerSize, lpwVer))
        goto ERROR_EXIT1;

    // Get the translation string
    // Note that we must put the VarFileInfo string into read-write
    // memory because, although the prototype for VerQueryValue has
    // this string cast as LPCSTR, the Win 3.1x version of this API
    // call writes into the string (zapping the intermediate backslash
    // so it can lstrcmpi against various possible cases).
////if (!VerQueryValue (lpVer, "\\VarFileInfo\\Translation", &lpBuf, &cb))
    if (!VerQueryValueW (lpwVer, wszVarFileInfo, &lpwBuf, &cb))
        goto ERROR_EXIT2;

    // Extract the translation value (with the swapped words)
    dwTrans = *(DWORD FAR *)lpwBuf;
    wsprintfW (wszTemp,
               WS_SLOPE L"StringFileInfo" WS_SLOPE L"%08lX" WS_SLOPE L"FileVersion",
               MAKELONG (HIWORD (dwTrans), LOWORD (dwTrans)));
    if (!VerQueryValueW (lpwVer, wszTemp, &lpwBuf, &cb))
        goto ERROR_EXIT3;

    // Copy to local storage as we're about to free the memory
    lstrcpyW (wszFileVer, lpwBuf);

    goto NORMAL_EXIT;

ERROR_EXIT1:
#ifdef DEBUG
    MB ("Error from GetFileVersionInfoW");
#endif

    goto NORMAL_EXIT;

ERROR_EXIT2:
#ifdef DEBUG
    MB ("Error from VerQueryvalueW # 1");
#endif

    goto NORMAL_EXIT;

ERROR_EXIT3:
#ifdef DEBUG
    MB ("Error from VerQueryvalueW # 2");
#endif

    goto NORMAL_EXIT;

NORMAL_EXIT:
    if (hLoc)
    {
        // Free the local memory
        LocalFree (hLoc); hLoc = NULL;
    } // End IF
} // End LclFileVersionStrW


//***************************************************************************
//  $AboutDlgProc
//
//  Just yer standard About box.
//***************************************************************************

APLU3264 CALLBACK AboutDlgProc
    (HWND   hDlg,       // Window handle
     UINT   message,    // Type of message
     WPARAM wParam,     // Additional information
     LPARAM lParam)     // ...

{
    static HFONT     hFont = NULL;
    static TOOLINFOW tti = {sizeof (tti)};
    static WCHAR     wszLclAppDPFE[_MAX_PATH + 1024];

    // Split cases
    switch (message)
    {
        case WM_INITDIALOG:
        {
            WCHAR wszTemp[512];

            // Change the icon to our own
            SendMessageW (hDlg, WM_SETICON, ICON_BIG, (LPARAM) (HANDLE_PTR) hIconAbout);

            // Format the version #
            wsprintfW (wszTemp, lpwszVersion, wszFileVer);

            // Write out the version string
            SetDlgItemTextW (hDlg, IDC_VERSION, wszTemp);

            // Copy the MPIR prefix to the text
            lstrcpyW (wszTemp, L"MPIR #");

            // Append the MPIR version #
            wsprintfW (&wszTemp[lstrlenW (wszTemp)], L"%S\n", mpir_version);

            // Copy the GMP prefix to the text
            lstrcatW (wszTemp, L"GMP #");

            // Append the GMP version #
            wsprintfW (&wszTemp[lstrlenW (wszTemp)], L"%S\n", gmp_version);

            // Copy the MPFR prefix to the text
            lstrcatW (wszTemp, L"MPFR #");

            // Append the MPFR version #
            wsprintfW (&wszTemp[lstrlenW (wszTemp)], L"%S\n", mpfr_get_version ());

            // Copy the ECM prefix to the text
            lstrcatW (wszTemp, L"ECM #");

            // Append the ECM version #
            wsprintfW (&wszTemp[lstrlenW (wszTemp)], L"%S\n", ecm_version);

            // Copy the COMCTL32.DLL prefix to the text
            lstrcatW (wszTemp, L"COMCTL32.DLL #");

            // Append the COMCTL32.DLL version #
            wsprintfW (&wszTemp[lstrlenW (wszTemp)], L"%s\n", wszComctl32FileVer);

            // Copy the CRSHHNDL.DLL prefix to the text
            lstrcatW (wszTemp, crsh_dll);
            lstrcatW (wszTemp, L" #");

            // Append the CRSHHNDL.DLL version #
            wsprintfW (&wszTemp[lstrlenW (wszTemp)], L"%s\n", crsh_version);

            // Write out the secondary version string
            SetDlgItemTextW (hDlg, IDC_VERSION2, wszTemp);

            CenterWindow (hDlg);    // Reposition the window to the center of the screen

            // Get the IDC_LINK window handle
            hWndStatic = GetDlgItem (hDlg, IDC_LINK);

            // Subclass the IDC_LINK control
            //   so we can handle WM_LBUTTONDOWN
            (HANDLE_PTR) lpfnOldStaticWndProc =
              SetWindowLongPtrW (hWndStatic,
                                 GWLP_WNDPROC,
                                 (APLU3264) (LONG_PTR) (WNDPROC) &LclStaticWndProc);
            // If we haven't done this before, ...
            if (wszLclAppDPFE[0] EQ WC_EOS)
            {
#define TT_PREFIX   L"Loaded from:  "
                // Copy the prefix to the text
                lstrcpyW (wszLclAppDPFE, TT_PREFIX);

                // Append the source DPFE
                lstrcatW (wszLclAppDPFE, wszAppDPFE);
#undef  TT_PREFIX
            } // End IF

            // Fill in TOOLINFOW fields
            tti.uFlags   = 0
                         | TTF_IDISHWND
                         | TTF_SUBCLASS
                           ;
            tti.hwnd     = hDlg;
            tti.uId      = (APLU3264) (HANDLE_PTR) GetDlgItem (hDlg, IDC_LOADEDFROM);
////////////tti.rect     =                      // Not used with TTF_IDISHWND
////////////tti.hinst    =                      // Not used except with string resources
            tti.lpszText = wszLclAppDPFE;
////////////tti.lParam   =                      // Not used by this code

            // Register a tooltip for the Icon
            SendMessageW (hWndTT,
                          TTM_ADDTOOLW,
                          0,
                          (LPARAM) (LPTOOLINFOW) &tti);
            return DLG_MSGDEFFOCUS; // Use the focus in wParam, DWLP_MSGRESULT is ignored
        } // End WM_INITDIALOG

        case WM_CTLCOLORSTATIC:     // hdc = (HDC) wParam;   // Handle of display context
                                    // hwnd = (HWND) lParam; // Handle of static control
#define hDC     ((HDC)  wParam)
#define hWnd    ((HWND) lParam)

            // We handle IDC_LINK static window only
            if (hWnd EQ hWndStatic)
            {
                LOGFONTW lf;

                // Set the text color
                SetTextColor (hDC, DEF_SCN_BLUE);

                // Set the background text color
                SetBkColor (hDC, GetSysColor (COLOR_BTNFACE));

                // Get the font handle
                hFont = (HFONT) SendMessageW (hWnd, WM_GETFONT, 0, 0);

                // Get the LOGFONTW structure for the font
                GetObjectW (hFont, sizeof (lf), &lf);

                // Change to an underlined font
                lf.lfUnderline = TRUE;

                // Create a new font, now underlined
                hFont = MyCreateFontIndirectW (&lf);

                // Select it into the DC
                SelectObject (hDC, hFont);

                // Return handle of brush for background, DWLP_MSGRESULT is ignored
                return (APLU3264) (HANDLE_PTR) GetSysColorBrush (COLOR_BTNFACE);
            } else
                break;
#undef  hWnd
#undef  hDC

        case WM_CLOSE:
            // If it's still around, delete the font we created
            if (hFont)
            {
                MyDeleteObject (hFont); hFont = NULL;
            } // End IF

            // Restore the old WndProc
            SetWindowLongPtrW (hWndStatic,
                               GWLP_WNDPROC,
                               (APLU3264) (LONG_PTR) (WNDPROC) lpfnOldStaticWndProc);
            lpfnOldStaticWndProc = NULL;

            // Unregister the tooltip for the Icon
            SendMessageW (hWndTT,
                          TTM_DELTOOLW,
                          0,
                          (LPARAM) (LPTOOLINFOW) &tti);
            EndDialog (hDlg, TRUE); // Quit this dialog

            DlgMsgDone (hDlg);      // We handled the msg

#define idCtl               GET_WM_COMMAND_ID   (wParam, lParam)
#define cmdCtl              GET_WM_COMMAND_CMD  (wParam, lParam)
#define hwndCtl             GET_WM_COMMAND_HWND (wParam, lParam)
        case WM_COMMAND:
            // If the user pressed one of our buttons, ...
            switch (idCtl)
            {
                case IDOK:
                    PostMessageW (hDlg, WM_CLOSE, 0, 0);

                    DlgMsgDone (hDlg);      // We handled the msg
            } // End switch (wParam)

            break;
#undef  hwndCtl
#undef  cmdCtl
#undef  idCtl
    } // End SWITCH

    DlgMsgPass (hDlg);      // We didn't handle the msg
} // End AboutDlgProc


//***************************************************************************
//  $LclStaticWndProc
//
//  Local window procedure for the About Box
//  static control to handle left clicks.
//***************************************************************************

LRESULT WINAPI LclStaticWndProc
    (HWND   hWnd,       // Window handle
     UINT   message,    // Type of message
     WPARAM wParam,     // Additional information
     LPARAM lParam)     // ...

{
    // Split cases
    switch (message)
    {
        case WM_LBUTTONDOWN:        // fwKeys = wParam;         // key flags
                                    // xPos = LOSHORT(lParam);  // horizontal position of cursor
                                    // yPos = HISHORT(lParam);  // vertical position of cursor
            // Check for the IDC_LINK static window
            if (hWnd EQ hWndStatic)
            {
                WCHAR wszStaticText[128];       // Text of static control

                // The user clicked on our static text --
                //   make it behave like a hyperlink

                // Get the static text so we can pass it to ShellExecute
                GetWindowTextW (hWndStatic,
                                wszStaticText,
                                countof (wszStaticText) - 1);
                // Call the shell to invoke the browser
                ShellExecuteW (hWnd,            // Handle to parent window
                               L"open",         // Operation to perform
                               wszStaticText,   // Object on which to perform operation
                               NULL,            // Executable file parameters
                               NULL,            // Default directory
                               SW_SHOWNORMAL    // How window is to shown when opened
                             );
                return FALSE;           // We handled the msg
            } // End IF

            break;

        case WM_SETCURSOR:          // hwnd = (HWND) wParam;       // handle of window with cursor
                                    // nHittest = LOWORD(lParam);  // hit-test code
                                    // wMouseMsg = HIWORD(lParam); // mouse-message identifier
            // Check for the IDC_LINK static window
            if (hWnd EQ hWndStatic)
            {
                // Set a new cursor to indicate that
                //   this is a hyperlink.
                SetCursor (LoadCursor (NULL, IDC_HAND));

                return FALSE;           // We handled the msg
            } // End IF

            break;
    } // End SWITCH

    return CallWindowProcW (lpfnOldStaticWndProc,
                            hWnd,
                            message,
                            wParam,
                            lParam);    // Pass on down the line
} // End LclStaticWndProc


//***************************************************************************
//  End of File: ver.c
//***************************************************************************
