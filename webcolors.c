//***************************************************************************
//  NARS2000 -- Web Color Names Window
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2011 Sudley Place Software

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

#define MYWM_CLOSE          (WM_APP +  0)

extern HICON hIconWC_Large;


//***************************************************************************
//  $WebColorsDlgProc
//
//  Message processing routine for the Web Color Names dialog
//***************************************************************************

APLU3264 CALLBACK WebColorsDlgProc
    (HWND   hDlg,       // Window handle
     UINT   message,    // Type of message
     WPARAM wParam,     // Additional information
     LPARAM lParam)     // ...

{
    static TOOLINFOW       tti = {sizeof (tti)};    // Tooltip Info struc
    static SYNTAXCOLORNAME scnMatch;                // Incoming color to match
           UINT            idCtl;                   // ID of the control
           WCHAR           wszTemp[256];            // Temporary storage for IDC_WEBCLR_REP text
           UINT            uCnt;                    // Loop counter
__try
{
    // Split cases
    switch (message)
    {
#define lInitParam      ((LPSYNTAXCOLORNAME) lParam)
        case WM_INITDIALOG:                 // hwndFocus = (HWND) wParam; // Handle of control to receive focus
                                            // lInitParam = lParam;       // Initialization parameter
        {
            LPWCHAR lpwName = NULL;         // Ptr to Web Color Name (if any)

            // Save the incoming values
            scnMatch = *lInitParam;

            // Change the icon to our own
            SendMessageW (hDlg, WM_SETICON, ICON_BIG, (LPARAM) (HANDLE_PTR) hIconWC_Large);

            // Fill in constant fields
            tti.uFlags   = 0
                         | TTF_IDISHWND
                         | TTF_SUBCLASS
                           ;
            tti.hwnd     = hDlg;
////////////tti.uId      =                      // Filled in below
////////////tti.hinst    =                      // Not used except with string resources
            tti.lpszText = LPSTR_TEXTCALLBACKW;
////////////tti.rect     =                      // Not used with TTF_IDISHWND
////////////tti.lParam   =                      // Not used by this code

            // Loop through the Web Color Names buttons
            for (uCnt = 0; uCnt < uColorNames; uCnt++)
            {
                // If the incoming color matches this one, ...
                if (scnMatch.syntClr.crFore EQ aColorNames[uCnt].clrRef)
                    // Save a ptr to the name
                    lpwName = aColorNames[uCnt].lpwName;

                // Fill in the Web Color Names text
                SendDlgItemMessageW (hDlg,
                                     IDC_WEBCLR_LT001 + uCnt,
                                     WM_SETTEXT,
                                     0,
                                     (APLU3264) (HANDLE_PTR) aColorNames[uCnt].lpwName);
                // Fill in the dynamic field
                tti.uId = (APLU3264) (HANDLE_PTR) GetDlgItem (hDlg, IDC_WEBCLR_BN001 + uCnt);

                // Register a tooltip for the Web Color Names button
                SendMessageW (hWndTT,
                              TTM_ADDTOOLW,
                              0,
                              (LPARAM) (LPTOOLINFOW) &tti);
            } // End FOR

            // The color buttons are initialized in WM_DRAWITEM
            // The Tooltip text is filled in by WM_NOTIFY/TTN_GETDISPINFOW

            // If we didn't match the name (some non-Web Color), ...
            if (lpwName EQ NULL)
                wsprintfW (wszTemp,
                           L" The color you are replacing is %u, %u, %u (#%02X%02X%02X) for category \"%s\".",
                           GetRValue (scnMatch.syntClr.crFore), GetGValue (scnMatch.syntClr.crFore), GetBValue (scnMatch.syntClr.crFore),
                           GetRValue (scnMatch.syntClr.crFore), GetGValue (scnMatch.syntClr.crFore), GetBValue (scnMatch.syntClr.crFore),
                           scnMatch.lpwSCName);
            else
                wsprintfW (wszTemp,
                           L" The color you are replacing is \"%s\" for category \"%s\".",
                           lpwName,
                           scnMatch.lpwSCName);
            // Append common text
            lstrcatW (wszTemp, WS_LF L" Click on a new color to replace and exit, or click on Cancel to exit without replacing.");

            // Insert the text about what color we're replacing
            SendDlgItemMessageW (hDlg,
                                 IDC_WEBCLR_LT_REP,
                                 WM_SETTEXT,
                                 0,
                                 (APLU3264) (HANDLE_PTR) wszTemp);
            // Reposition the window to the center of the screen
            CenterWindow (hDlg);

            return DLG_MSGDEFFOCUS; // Use the focus in wParam
        } // End WM_INITDIALOG
#undef  lInitParam

#define hdcStatic       ((HDC) wParam)
#define hwndStatic      ((HWND) lParam)
        case WM_CTLCOLORSTATIC:             // hdcStatic = (HDC) wParam;   // handle of display context
                                            // hwndStatic = (HWND) lParam; // handle of static control
            // Get the control's ID
            idCtl = GetDlgCtrlID (hwndStatic);

            // If it's a Web Color Name static text ID, ...
            if (IDC_WEBCLR_LT001 <= idCtl
             &&                     idCtl <= IDC_WEBCLR_LT_LAST)
            {
                // If this matches the incoming color ref, ...
                if (scnMatch.syntClr.crFore EQ aColorNames[idCtl - IDC_WEBCLR_LT001].clrRef)
                {
                    // Change the text color to a highlight
                    SetTextColor (hdcStatic, DEF_SCN_WHITE);

                    // and the background color, too.
                    SetBkColor (hdcStatic, DEF_SCN_NAVY);

                    // Return a NULL brush so the normal background is used, DWLP_MSGRESULT is ignored
                    return (APLU3264) GetStockObject (NULL_BRUSH);
                } // End IF
            } else
            // Check to see if this is our OwnerDraw static text control
            if (IDC_WEBCLR_LT_REP EQ idCtl)
                // Return the brush handle, DWLP_MSGRESULT is ignored
                return (APLU3264) GetStockObject (WHITE_BRUSH);
            break;
#undef  hwndStatic
#undef  hdcStatic

        case WM_NOTIFY:             // idCtrl = (int) wParam;
                                    // pnmh = (LPNMHDR) lParam;
#define lpnmh   (*(LPNMHDR *) &lParam)

            // Split cases based upon the notification code
            switch (lpnmh->code)
            {
////////////////case TTN_NEEDTEXTW:
                case TTN_GETDISPINFOW:  // idCtl = (int) wParam;
                                        // lpttt = (LPTOOLTIPTEXTW) lParam;
                {
                    static WCHAR    TooltipText[_MAX_PATH];
                           COLORREF clrRef;

#define lpttt   (*(LPTOOLTIPTEXTW *) &lParam)

                    // Get the ID of the calling window
                    idCtl = GetDlgCtrlID ((HWND) lpttt->hdr.idFrom);

                    // If it's a Web Color Name button ID, ...
                    if (IDC_WEBCLR_BN001 <= idCtl && idCtl <= IDC_WEBCLR_BN_LAST)
                    {
                        // Convert to an index
                        idCtl -= IDC_WEBCLR_BN001;

                        // Get the color
                        clrRef = aColorNames[idCtl].clrRef;

                        // Format the tooltip text
                        wsprintfW (TooltipText,
                                   L"%s %u, %u, %u (#%02X%02X%02X)",
                                   aColorNames[idCtl].lpwName,
                                   GetRValue (clrRef), GetGValue (clrRef), GetBValue (clrRef),
                                   GetRValue (clrRef), GetGValue (clrRef), GetBValue (clrRef));
                        // Return the ptr to the caller
                        lpttt->lpszText = TooltipText;
#undef  lpttt
                        DlgMsgDone (hDlg);  // We handled the msg
                    } // End IF

                    break;
                } // End TTN_GETDISPINFOW

                default:
                    break;
            } // End SWITCH

            break;

#define idCtl       ((UINT) wParam)
#define lpdis       ((LPDRAWITEMSTRUCT) lParam)
        case WM_DRAWITEM:           // idCtl = (UINT) wParam;             // Control identifier
                                    // lpdis = (LPDRAWITEMSTRUCT) lParam; // Item-drawing information
            // Check to see if this is one of our Web Color Names buttons
            if (IDC_WEBCLR_BN001 <= idCtl && idCtl <= IDC_WEBCLR_BN_LAST)
            {
                Assert (lpdis->CtlType EQ ODT_BUTTON);

                // Split cases based upon the item action
                switch (lpdis->itemAction)
                {
                    case ODA_DRAWENTIRE:
                    case ODA_SELECT:
                        // Draw the edge, fill with the color
                        FillSyntaxColor (lpdis, aColorNames[idCtl - IDC_WEBCLR_BN001].clrRef);

                        break;

                    case ODA_FOCUS:     // Ignore changes in focus
                        break;
                } // End SWITCH
            } else
            // Check to see if this is our OwnerDraw static text control
            if (IDC_WEBCLR_LT_REP EQ idCtl)
            {
                Assert (lpdis->CtlType EQ ODT_STATIC);

                // Split cases based upon the item action
                switch (lpdis->itemAction)
                {
                    case ODA_DRAWENTIRE:
                    case ODA_SELECT:
                        // Get the text from the static control
                        SendDlgItemMessageW (hDlg,
                                             IDC_WEBCLR_LT_REP,
                                             WM_GETTEXT,
                                             countof (wszTemp),
                                             (APLU3264) (HANDLE_PTR) wszTemp);
                        // Fill in the background
                        FillRect (lpdis->hDC, &lpdis->rcItem, GetStockObject (WHITE_BRUSH));

                        // Draw the text
                        DrawTextW (lpdis->hDC,
                                   wszTemp,
                                   lstrlenW (wszTemp),
                                  &lpdis->rcItem,
                                   0);
                        break;

                    case ODA_FOCUS:     // Ignore changes in focus
                        break;
                } // End SWITCH
            } // End IF/ELSE

            DlgMsgDone (hDlg);      // We handled the msg
#undef  lpdis
#undef  idCtl

        case WM_CLOSE:
            PostMessageW (hDlg, MYWM_CLOSE, FALSE, 0);

            DlgMsgDone (hDlg);      // We handled the msg

        case MYWM_CLOSE:                    // bSuccess = (UBOOL) wParam
                                            // clrRef = (COLORREF) lParam
            // Loop through the Web Color Names buttons
            for (uCnt = 0; uCnt < uColorNames; uCnt++)
            {
                // Fill in the dynamic field
                tti.uId = (APLU3264) (HANDLE_PTR) GetDlgItem (hDlg, IDC_WEBCLR_BN001 + uCnt);

                // Unregister a tooltip for the Web Color Names button
                SendMessageW (hWndTT,
                              TTM_DELTOOLW,
                              0,
                              (LPARAM) (LPTOOLINFOW) &tti);
            } // End FOR
            // Quit this dialog
            EndDialog (hDlg, ((UBOOL) lParam) ? (COLORREF) lParam : -1);

            DlgMsgDone (hDlg);      // We handled the msg

#define idCtl               GET_WM_COMMAND_ID   (wParam, lParam)
#define cmdCtl              GET_WM_COMMAND_CMD  (wParam, lParam)
#define hwndCtl             GET_WM_COMMAND_HWND (wParam, lParam)
        case WM_COMMAND:            // wNotifyCode = HIWORD(wParam); // Notification code
                                    // wID = LOWORD(wParam);         // Item, control, or accelerator identifier
                                    // hwndCtl = (HWND) lParam;      // Handle of control
            // If the user pressed one of our buttons, ...
            switch (idCtl)
            {
                case IDCANCEL:
                    PostMessageW (hDlg, MYWM_CLOSE, FALSE, 0);

                    DlgMsgDone (hDlg);      // We handled the msg

                default:
                    // Check to see if this is one of our Web Color Names buttons
                    if (IDC_WEBCLR_BN001 <= idCtl
                     &&                     idCtl <= IDC_WEBCLR_BN_LAST)
                    {
                        // Tell 'em to quit and use this color
                        PostMessageW (hDlg, MYWM_CLOSE, TRUE, aColorNames[idCtl - IDC_WEBCLR_BN001].clrRef);

                        DlgMsgDone (hDlg);      // We handled the msg
                    } // End IF

                    break;
            } // End SWITCH

            break;
#undef  hwndCtl
#undef  cmdCtl
#undef  idCtl
    } // End SWITCH

} __except (CheckException (GetExceptionInformation (), L"WebColorsDlgProc"))
{
    // Display message for unhandled exception
    DisplayException ();
} // End __try/__except

    DlgMsgPass (hDlg);      // We didn't handle the msg
} // End WebColorsDlgProc


//***************************************************************************
//  End of File: webcolors.c
//***************************************************************************
