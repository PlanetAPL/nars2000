//***************************************************************************
//  NARS2000 -- Free Open Source Software APL Interpreter
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

#define COMPILE_MULTIMON_STUBS

#define STRICT
#define OEMRESOURCE         // To get OBM_CHECK define
#include <windows.h>
#include <windowsx.h>
#include <versionhelpers.h>

#define DEFINE_VARS
#define DEFINE_VALUES
#define DEFINE_ENUMS
#define MPIFNS
#include "headers.h"
#include "typemote.h"
#undef  MPIFNS
#undef  DEFINE_ENUMS
#undef  DEFINE_VALUES
#undef  DEFINE_VARS


CRITICAL_SECTION CSOPthread;            // Critical Section Object for pthread

#ifdef DEBUG
extern int Debug = 1;
#endif
WCHAR crsh_dll[] = L"CRASHRPT.DLL",
      crsh_version[32] = L" ***NOT LOADED***";


//************************** Data Area **************************************

typedef struct tagENUMSETFONTW
{
    LPWCHAR lpwClassName;
    HFONT   hFont;
} ENUMSETFONTW, *LPENUMSETFONTW;

typedef struct tagENUMPASSMSG
{
    UINT   message;
    WPARAM wParam;
    LPARAM lParam;
} ENUMPASSMSG, *LPENUMPASSMSG;

int  nMinState,                         // Minimized state as per WinMain
     iScrollSize;                       // Width of a vertical scrollbar
UBOOL fHelp = FALSE,                    // TRUE iff we displayed help
      bCommandLine = FALSE;             // ...      there is a filename on the command line

HMODULE user32_module;                  // Needed by WineHQ\EDITCTRL.C

HICON hIconMF_Large,    hIconMF_Small,      // Icon handles
      hIconSM_Large,    hIconSM_Small,
      hIconFE_Large,    hIconFE_Small,
      hIconME_Large,    hIconME_Small,
      hIconVE_Large,    hIconVE_Small,
      hIconCC_Large,    hIconCC_Small,
      hIconWC_Large,    hIconWC_Small,
      hIconAbout,
      hIconClose,
      hIconCustom;
#ifdef DEBUG
HICON hIconDB_Large, hIconDB_Small;
#endif
#ifdef PERFMONON
HICON hIconPM_Large, hIconPM_Small;
#endif

WCHAR wszMFTitle[]          = WS_APPNAME WS_BITSIZE WS_APPEND_DEBUG,                // Master frame window title
      wszCCTitle[]          = WS_APPNAME L" Crash Control Window" WS_APPEND_DEBUG,  // Crash Control window title
      wszTCTitle[]          = WS_APPNAME L" Tab Control Window" WS_APPEND_DEBUG;    // Tab Control ... (for debugging purposes only)

char pszNoRegMFWndClass[]   = "Unable to register window class <" MFWNDCLASS        ">.",
     pszNoRegSMWndClass[]   = "Unable to register window class <" SMWNDCLASS        ">.",
     pszNoRegFEWndClass[]   = "Unable to register window class <" FEWNDCLASS        ">.",
     pszNoRegECWndClass[]   = "Unable to register window class <" ECWNDCLASS        ">.",
     pszNoRegCCWndClass[]   = "Unable to register window class <" CCWNDCLASS        ">.",
     pszNoRegPBWndClass[]   = "Unable to register window class <" PBWNDCLASS        ">.",
     pszNoRegSBWndClass[]   = "Unable to register window class <" LCL_WC_STATUSBARA ">.",
     pszNoRegFW_RBWndClass[]= "Unable to register window class <" FW_RBWNDCLASS     ">.",
     pszNoRegLW_RBWndClass[]= "Unable to register window class <" LW_RBWNDCLASS     ">.";
#ifdef DEBUG
char pszNoRegDBWndClass[]   = "Unable to register window class <" DBWNDCLASS        ">.";
#endif
#ifdef PERFMONON
char pszNoRegPMWndClass[]   = "Unable to register window class <" PMWNDCLASS        ">.";
#endif

char pszNoCreateMFWnd[]     = "Unable to create Master Frame window",
     pszNoCreateTCWnd[]     = "Unable to create Tab Control window",
     pszNoCreateTTWnd[]     = "Unable to create Tooltip window",
     pszNoCreateSTWnd[]     = "Unable to create Status window",
     pszNoCreateCCWnd[]     = "Unable to create Crash Control window";

UBOOL bMainDestroy = FALSE;     // TRUE iff we're destroying the Main window

extern
WNDPROC lpfnOldStatusBarWndProc;            // Save area for old StatusBar Window procedure


//***************************************************************************
//  $EnumCallbackPassMsg
//
//  EnumChildWindows callback to pass a message to all child
//    windows of MDI Clients
//***************************************************************************

UBOOL CALLBACK EnumCallbackPassMsg
    (HWND   hWndMC,         // Handle to child window (MDI Client)
     LPARAM lParam)         // Application-defined value

{
    HWND hWndChild;

    // Forward this to all child windows of all MDI Clients
    for (hWndChild = GetWindow (hWndMC, GW_CHILD);
         hWndChild;
         hWndChild = GetWindow (hWndChild, GW_HWNDNEXT))
    {
        // When an MDI child window is minimized, Windows creates two windows: an
        // icon and the icon title.  The parent of the icon title window is set to
        // the MDI client window, which confines the icon title to the MDI client
        // area.  The owner of the icon title is set to the MDI child window.
        if (GetWindow (hWndChild, GW_OWNER) NE NULL) // If it's an icon title window, ...
            continue;                       // skip it, and continue enumerating

        PostMessageW (hWndChild, ((LPENUMPASSMSG) lParam)->message,
                                 ((LPENUMPASSMSG) lParam)->wParam,
                                 ((LPENUMPASSMSG) lParam)->lParam);
    } // End FOR

    return TRUE;        // Keep on truckin'
} // End EnumCallbackPassMsg


//***************************************************************************
//  $EnumCallbackDrawLineCont
//
//  EnumChildWindows callback to redraw the Line Continuation markers
//
//  lParam = unused
//***************************************************************************

UBOOL CALLBACK EnumCallbackDrawLineCont
    (HWND   hWnd,           // Handle to child window
     LPARAM lParam)         // Application-defined value

{
    // When an MDI child window is minimized, Windows creates two windows: an
    // icon and the icon title.  The parent of the icon title window is set to
    // the MDI client window, which confines the icon title to the MDI client
    // area.  The owner of the icon title is set to the MDI child window.
    if (GetWindow (hWnd, GW_OWNER) NE NULL)     // If it's an icon title window, ...
        return TRUE;                    // skip it, and continue enumerating

    // If the window is either SM or FE, ...
    if (IzitSM (hWnd) || IzitFE (hWnd))
    {
        HWND hWndEC;

        // Get the window handle to the Edit Ctrl
        hWndEC = (HWND) GetWindowLongPtrW (hWnd, GWLSF_HWNDEC);

        // If it's a SM window, ...
        if (IzitSM (hWnd))
            SetMarginsSM (hWndEC);
        else
            SetMarginsFE (hWndEC);

        // Tell the SM or FE window to redraw its Line Continuation markers
        DrawAllLineCont (hWndEC);
    } // End IF

    return TRUE;                        // Continue enumerating
} // End EnumCallbackDrawLineCont


//***************************************************************************
//  $EnumCallbackSetFontW
//
//  EnumChildWindows callback to set a window's font
//***************************************************************************

UBOOL CALLBACK EnumCallbackSetFontW
    (HWND   hWnd,           // Handle to child window
     LPARAM lParam)         // Application-defined value

{
    WCHAR wszTemp[32];

    // When an MDI child window is minimized, Windows creates two windows: an
    // icon and the icon title.  The parent of the icon title window is set to
    // the MDI client window, which confines the icon title to the MDI client
    // area.  The owner of the icon title is set to the MDI child window.
    if (GetWindow (hWnd, GW_OWNER) NE NULL)     // If it's an icon title window, ...
        return TRUE;                    // skip it, and continue enumerating

    // Get the window's class name
    GetClassNameW (hWnd, wszTemp, countof (wszTemp));

#define lpEnumSetFontW  ((LPENUMSETFONTW) lParam)

    // If this is the matching class name,
    //   set the new font in place and redraw.
    if (lstrcmpiW (lpEnumSetFontW->lpwClassName, wszTemp) EQ 0)
        SendMessageW (hWnd, WM_SETFONT, (WPARAM) lpEnumSetFontW->hFont, MAKELPARAM (TRUE, 0));

    return TRUE;        // Keep on truckin'
#undef  lpEnumSetFontW
} // End EnumCallbackSetFontW


//***************************************************************************
//  $EnumCallbackFindAplFont
//
//  EnumFonts callback function to find the default APL font
//***************************************************************************

int CALLBACK EnumCallbackFindAplFont
    (ENUMLOGFONTW  *lpELF,      // Ptr to ENUMLOGFONTW struc
     NEWTEXTMETRIC *lpNTM,      // Ptr to TEXTMETRIC struc
     int            iFontType,  // Font type
     LPARAM         lParam)     // Application-defined data
{
#ifdef DEBUG
    LPWCHAR lpwFontName = (LPWCHAR) lParam;
#else
  #define lpwFontName   ((LPWCHAR) lParam)
#endif
    // If the name does not match our name, ...
    if (lstrcmpiW (lpELF->elfFullName, lpwFontName) NE 0)
        // Continue enumerating
        return TRUE;

    // We found a match
    bAPLFont = TRUE;

    return FALSE;               // Stop enumerating
#ifndef DEBUG
  #undef  lpwFontName
#endif
} // End EnumCallbackFindAplFont


//***************************************************************************
//  $InitChooseFont
//
//  Initialize CHOOSEFONTW values
//***************************************************************************

void InitChooseFont
    (void)

{
    int iLogPixelsY,            // # vertical pixels per inch in the DC
        fontEnum;               // Loop counter

    // Get # vertical pixels per inch
    // N.B.:  Do not use Printer DC here as that calculation is done
    //        at printing time.  We need to use screen coordinates
    //        for display purposes and convert to printer coords
    //        only when printing.
    iLogPixelsY = GetLogPixelsY (NULL);

    // Loop through the fonts
    for (fontEnum = 0; fontEnum < FONTENUMX_LENGTH; fontEnum++)
    {
        // Zero the struc
        ZeroMemory (fontStruc[fontEnum].lpcf, sizeof (CHOOSEFONT));

        // Convert from point size to pixels
        //   unless already set in which case
        //   calculate the point size
        if (fontStruc[fontEnum].lplf->lfHeight EQ 0)
            fontStruc[fontEnum].lplf->lfHeight = -MulDiv (fontStruc[fontEnum].iDefPtSize, iLogPixelsY, 72);
        else
            fontStruc[fontEnum].iDefPtSize = MulDiv (-fontStruc[fontEnum].lplf->lfHeight, 72, iLogPixelsY);

        fontStruc[fontEnum].lpcf->lStructSize = sizeof (CHOOSEFONT);
////////fontStruc[fontEnum].lpcf->hwndOwner  =
////////fontStruc[fontEnum].lpcf->hDC =                     // Only w/CF_PRINTERFONTS
        fontStruc[fontEnum].lpcf->lpLogFont  = fontStruc[fontEnum].lplf;
        fontStruc[fontEnum].lpcf->iPointSize = fontStruc[fontEnum].iDefPtSize * 10; // Font point size in 1/10ths
        fontStruc[fontEnum].lpcf->Flags      = 0
                                             | CF_ENABLEHOOK
                                             | CF_FORCEFONTEXIST
                                             | CF_INITTOLOGFONTSTRUCT
                                             | CF_SCREENFONTS
                                               ;
////////fontStruc[fontEnum].lpcf->rgbColors  =              // Only w/CF_EFFECTS
        fontStruc[fontEnum].lpcf->lCustData  = (LPARAM) fontStruc[fontEnum].lpwTitle;
        fontStruc[fontEnum].lpcf->lpfnHook   = &MyChooseFontHook;
////////fontStruc[fontEnum].lpcf->lpTemplateName =          // Only w/CF_ENABLETEMPLATE
////////fontStruc[fontEnum].lpcf->hInstance  =              // Only w/CF_ENABLETEMPLATE
////////fontStruc[fontEnum].lpcf->lpszStyle  =              // Only w/CF_USESTYLE
////////fontStruc[fontEnum].lpcf->nFontType  =              // Output only
////////fontStruc[fontEnum].lpcf->nSizeMin   =              // Only w/CF_LIMITSIZE
////////fontStruc[fontEnum].lpcf->nSizeMax   =              // Only w/CF_LIMITSIZE
    } // End FOR
} // End InitChooseFont


//***************************************************************************
//  $MyChooseFontHook
//
//  Hook function for ChooseFontW
//***************************************************************************

UINT_PTR APIENTRY MyChooseFontHook
    (HWND   hDlg,           // Dialog handle
     UINT   message,        // Type of message
     WPARAM wParam,         // Additional information
     LPARAM lParam)         // ...

{
    // Split cases based upon the message
    switch (message)
    {
        case WM_INITDIALOG:             // hwndFocus = (HWND) wParam; // Handle of control to receive focus
                                        // lInitParam = lParam;       // Initialization parameter
            // If there's a window title, ...
            if (((LPCHOOSEFONT) lParam)->lCustData)
                // Set the window title
                SetWindowTextW (hDlg, (LPWCHAR) ((LPCHOOSEFONT) lParam)->lCustData);
            else
            {
                // Set the window title
                SetWindowTextW (hDlg, L"Keyboard Font");

                // Hide the size selection windows
                ShowWindow (GetDlgItem (hDlg, stc3), SW_HIDE);
                ShowWindow (GetDlgItem (hDlg, cmb3), SW_HIDE);
            } // End IF/ELSE

            // Subclass the Sample Text Static Ctrl
            //   so we can avoid overwriting our Sample Text
            (HANDLE_PTR) lpfnOldChooseFontSampleWndProc =
              SetWindowLongPtrW (GetDlgItem (hDlg, stc5),
                                 GWLP_WNDPROC,
                                 (APLU3264) (LONG_PTR) (WNDPROC) &LclChooseFontSampleWndProc);
            // Set the sample text
            SetWindowTextW (GetDlgItem (hDlg, stc5), SampleText);

            return FALSE;               // Pass msg to standard dialog

        case WM_COMMAND:            // wNotifyCode = HIWORD (wParam);   // Notification code
                                    // wID = LOWORD (wParam);           // Item, control, or accelerator identifier
                                    // hwndCtl = (HWND) lParam;         // Handle of control
        {
#ifdef DEBUG
            UINT idCtl   = GET_WM_COMMAND_ID   (wParam, lParam),
                 cmdCtl  = GET_WM_COMMAND_CMD  (wParam, lParam);
            HWND hwndCtl = GET_WM_COMMAND_HWND (wParam, lParam);
#else
  #define idCtl             GET_WM_COMMAND_ID   (wParam, lParam)
  #define cmdCtl            GET_WM_COMMAND_CMD  (wParam, lParam)
  #define hwndCtl           GET_WM_COMMAND_HWND (wParam, lParam)
#endif
            // If the user pressed one of our buttons, ...
            switch (idCtl)
            {
                case psh3:          // The Apply button id
                    // Get the current LOGFONTW struc
                    SendMessageW (hDlg, WM_CHOOSEFONT_GETLOGFONT, 0, (LPARAM) &lfKB);

                    // Set the font for the appropriate keyboard controls
                    //   from lfKB
                    SetKeybFont (NULL);

                    break;

                default:
                    break;
            } // End SWITCH

            return FALSE;               // Pass msg to standard dialog
#ifndef DEBUG
  #undef  hwndCtl
  #undef  cmdCtl
  #undef  idCtl
#endif
        } // End WM_COMMAND

        default:
            return FALSE;               // Pass msg to standard dialog
    } // End SWITCH
} // End MyChooseFontHook


//***************************************************************************
//  $LclChooseFontSampleWndProc
//
//  Local ChooseFont Sample Text subclass procedure
//***************************************************************************

LRESULT WINAPI LclChooseFontSampleWndProc
    (HWND   hWnd,       // Window handle
     UINT   message,    // Type of message
     WPARAM wParam,     // Additional information
     LPARAM lParam)     // ...

{
////LCLODSAPI ("LST: ", hWnd, message, wParam, lParam);

    // Split cases
    switch (message)
    {
        case WM_SETTEXT:
            // Set the sample text
            return
              CallWindowProcW (lpfnOldChooseFontSampleWndProc,
                               hWnd,
                               message,
                               wParam,
                      (LPARAM) SampleText);     // Pass on down the line
        default:
            break;
    } // End SWITCH

////LCLODSAPI ("LSTZ: ", hWnd, message, wParam, lParam);
    return
      CallWindowProcW (lpfnOldChooseFontSampleWndProc,
                       hWnd,
                       message,
                       wParam,
                       lParam);     // Pass on down the line
} // End LclChooseFontSampleWndProc


//***************************************************************************
//  $CreateNewFontCom
//
//  Subroutine to CreateNewFontXX to set various variables
//***************************************************************************

UBOOL CreateNewFontCom
    (HFONT        *lphFont,             // Ptr to in HFONT to create
     FONTENUM      fontEnum,            // Font enum index (-1 = none)
     LPLOGFONTW    lplf,                // Ptr to in/out LOGFONTW to set
     LPCHOOSEFONTW lpcf,                // Ptr to in CHOOSEFONTW with iPointSize
     LPTEXTMETRICW lptm,                // Ptr to in TEXTMETRICWs
     long         *lpcxAveChar,         // Ptr to out avg width (may be NULL)
     long         *lpcyAveChar)         // Ptr to out avg height (may be NULL)

{
    HDC   hDCTmp;                       // Temporary DC
    HFONT hFontOld;                     // Previous font handle
    long  cyAveChar;                    // Average char height
    int   iLogPixelsY,                  // # vertical pixels per inch in the DC
          iOldMode;                     // Previous mapping mode

    // Delete the previous handle (if any)
    if (*lphFont NE NULL)
    {
        // Delete the font handle
        MyDeleteObject (*lphFont); *lphFont = NULL;
    } // End IF

    // Create the font
    *lphFont = MyCreateFontIndirectW (lplf);

    // Check for error
    if (*lphFont EQ NULL)
        return FALSE;

    // Get a new device context or use the given one
    hDCTmp = MyGetDC (HWND_DESKTOP);

    // Set the mapping mode
    iOldMode = SetMapMode (hDCTmp, MM_TEXT);

    // Get the # pixels per vertical inch
    iLogPixelsY = GetDeviceCaps (hDCTmp, LOGPIXELSY);

    // Select the newly created font
    hFontOld = SelectObject (hDCTmp, *lphFont);

    // Get the text metrics for this font
    GetTextMetricsW (hDCTmp, lptm);

    // Restore the old font
    SelectObject (hDCTmp, hFontOld);

    // Restore the mapping mode
    SetMapMode (hDCTmp, iOldMode);

    // Release the one we created
    MyReleaseDC (HWND_DESKTOP, hDCTmp); hDCTmp = NULL;

    // New height in pixels
    cyAveChar = MulDiv (lpcf->iPointSize, iLogPixelsY, 72 * 10);

    // New width (same aspect ratio as old)
    if (lpcxAveChar NE NULL)
        *lpcxAveChar = MulDiv (lptm->tmAveCharWidth, cyAveChar, -lplf->lfHeight);

    // New height
    lplf->lfHeight = -cyAveChar;

    if (lpcyAveChar NE NULL)
        *lpcyAveChar = lptm->tmHeight;

    // Now that we've calculated the correct height & width,
    //   delete the font and re-create it
    MyDeleteObject (*lphFont); *lphFont = NULL;

    // Re-create the font
    *lphFont = MyCreateFontIndirectW (lplf);

    // If this font is within normal bounds, ...
    if (fontEnum NE -1
     && fontEnum < FONTENUM_LENGTH)
    {
        // Determine the charsize of the Line Continuation Marker
        uWidthLC[fontEnum] = WidthLC (fontEnum);

        // If this is FONTENUM_SM, also create FB_SM
        if (fontEnum EQ glbSameFontAs[FONTENUM_SM])
        {
            // Set the point size to that of the parent font
            cfFB_SM.iPointSize = fontStruc[fontEnum].lpcf->iPointSize;

            if (CreateNewFontCom (&hFontFB_SM,
                                   FONTENUM_FB_SM,
                                  &lfFB_SM,
                                  &cfFB_SM,
                                  &tmFB_SM,
                                  &GetFSDirAveCharSize (FONTENUM_FB_SM)->cx,
                                  &GetFSDirAveCharSize (FONTENUM_FB_SM)->cy))
                // Recalculate the average char width
                GetFSDirAveCharSize (FONTENUM_FB_SM)->cx = RecalcAveCharWidth (hFontFB_SM);
            else
                OptionFlags.bOutputDebug = FALSE;
        } // End IF

        // If this is FONTENUM_FE, also create FB_FE
        if (fontEnum EQ glbSameFontAs[FONTENUM_FE])
        {
            // Set the point size to that of the parent font
            cfFB_FE.iPointSize = fontStruc[fontEnum].lpcf->iPointSize;

            if (CreateNewFontCom (&hFontFB_FE,
                                   FONTENUM_FB_FE,
                                  &lfFB_FE,
                                  &cfFB_FE,
                                  &tmFB_FE,
                                  &GetFSDirAveCharSize (FONTENUM_FB_FE)->cx,
                                  &GetFSDirAveCharSize (FONTENUM_FB_FE)->cy))
                // Recalculate the average char width
                GetFSDirAveCharSize (FONTENUM_FB_FE)->cx = RecalcAveCharWidth (hFontFB_FE);
            else
                OptionFlags.bOutputDebug = FALSE;
        } // End IF

        // If this is FONTENUM_PR, also create FB_PR_SM and FB_PR_FE
        if (fontEnum EQ glbSameFontAs[FONTENUM_PR])
        {
            // Set the point size to that of the parent font
            cfFB_PR_SM.iPointSize =
            cfFB_PR_FE.iPointSize = fontStruc[fontEnum].lpcf->iPointSize;

            // Create the font
            if (CreateNewFontCom (&hFontFB_PR_SM,
                                   FONTENUM_FB_PR_SM,
                                  &lfFB_PR_SM,
                                  &cfFB_PR_SM,
                                  &tmFB_PR_SM,
                                  &GetFSDirAveCharSize (FONTENUM_FB_PR_SM)->cx,
                                  &GetFSDirAveCharSize (FONTENUM_FB_PR_SM)->cy))
                // Recalculate the average char width
                GetFSDirAveCharSize (FONTENUM_FB_PR_SM)->cx = RecalcAveCharWidth (hFontFB_PR_SM);
            else
                OptionFlags.bOutputDebug = FALSE;

            // Create the font
            if (CreateNewFontCom (&hFontFB_PR_FE,
                                   FONTENUM_FB_PR_FE,
                                  &lfFB_PR_FE,
                                  &cfFB_PR_FE,
                                  &tmFB_PR_FE,
                                  &GetFSDirAveCharSize (FONTENUM_FB_PR_FE)->cx,
                                  &GetFSDirAveCharSize (FONTENUM_FB_PR_FE)->cy))
                // Recalculate the average char width
                GetFSDirAveCharSize (FONTENUM_FB_PR_FE)->cx = RecalcAveCharWidth (hFontFB_PR_FE);
            else
                OptionFlags.bOutputDebug = FALSE;
        } // End IF
    } // End IF

    return TRUE;
} // End CreateNewFontCom


//***************************************************************************
//  $CreateNewFontTC
//
//  Create a new font for the TC window.
//***************************************************************************

void CreateNewFontTC
    (UBOOL bApply)                      // TRUE iff we should apply the new font

{
    // Call common routine to set various variables
    CreateNewFontCom (&hFontTC,
                       FONTENUM_TC,
                      &lfTC,
                      &cfTC,
                      &tmTC,
                      &GetFSDirAveCharSize (FONTENUM_TC)->cx,
                      &GetFSDirAveCharSize (FONTENUM_TC)->cy);
    // If we are also applying the font, ...
    if (bApply)
        ApplyNewFontEnum (FONTENUM_TC);
} // End CreateNewFontTC


//***************************************************************************
//  $ApplyNewFontEnum
//
//  Apply a new font to any matching windows in <glbSameFontAs>
//***************************************************************************

void ApplyNewFontEnum
    (FONTENUM fontEnum)                 // FONTENUM_xxx value to match

{
    UINT uCnt;                          // Loop counter

    // Loop through glbSameFontAs to apply this font to similar windows
    for (uCnt = 0 ; uCnt < FONTENUM_LENGTH; uCnt++)
    if (glbSameFontAs[uCnt] EQ fontEnum)
        (*fontStruc[uCnt].lpApplyNewFont) (*fontStruc[fontEnum].lphFont);
} // End ApplyNewFontEnum


//***************************************************************************
//  $GetFSIndFontHandle
//
//  From FontStruc, get the indirect font handle taking into account
//    <glbSameFontAs>.
//***************************************************************************

HFONT GetFSIndFontHandle
    (FONTENUM fontEnum)                 // FONTENUM_xxx index

{
    return *fontStruc[glbSameFontAs[fontEnum]].lphFont;
} // End GetFSIndFontHandle


//***************************************************************************
//  $GetFSIndLogfontW
//
//  From FontStruc, get the indirect logfont taking into account
//    <glbSameFontAs>.
//***************************************************************************

LPLOGFONTW GetFSIndLogfontW
    (FONTENUM fontEnum)                 // FONTENUM_xxx index

{
    return fontStruc[glbSameFontAs[fontEnum]].lplf;
} // End GetFSIndLogfontW


//***************************************************************************
//  $GetFSIndAveCharSize
//
//  From FontStruc, get the indirect average char size taking into account
//    <glbSameFontAs>.
//***************************************************************************

LPSIZE GetFSIndAveCharSize
    (FONTENUM fontEnum)                 // FONTENUM_xxx index

{
    return &fontStruc[glbSameFontAs[fontEnum]].charSize;
} // End GetFSIndAveCharSize


//***************************************************************************
//  $GetFSDirAveCharSize
//
//  From FontStruc, get the direct average char size
//***************************************************************************

LPSIZE GetFSDirAveCharSize
    (FONTENUM fontEnum)                 // FONTENUM_xxx index

{
    return &fontStruc[fontEnum].charSize;
} // End GetFSDirAveCharSize


//***************************************************************************
//  $ApplyNewFontTC
//
//  Apply the TC font to the appropriate windows
//***************************************************************************

void ApplyNewFontTC
    (HFONT hFont)                   // Font handle to use

{
    // Tell the TC about the new font
    SendMessageW (hWndTC, WM_SETFONT, (WPARAM) hFont, MAKELPARAM (TRUE, 0));

    // Repaint the TC labels
    InvalidateRect (hWndTC, NULL, TRUE);
} // End ApplyNewFontTC


//***************************************************************************
//  $CreateNewFontCC
//
//  Create a new font for the CC window.
//***************************************************************************

void CreateNewFontCC
    (UBOOL bApply)                      // TRUE iff we should apply the new font

{
    // Call common routine to set various variables
    CreateNewFontCom (&hFontCC,
                       FONTENUM_CC,
                      &lfCC,
                      &cfCC,
                      &tmCC,
                      &GetFSDirAveCharSize (FONTENUM_CC)->cx,
                      &GetFSDirAveCharSize (FONTENUM_CC)->cy);
    // If we are also applying the font, ...
    if (bApply)
        ApplyNewFontEnum (FONTENUM_CC);
} // End CreateNewFontCC


//***************************************************************************
//  $ApplyNewFontCC
//
//  Apply the CC font to the appropriate windows
//***************************************************************************

void ApplyNewFontCC
    (HFONT hFont)                   // Font handle to use

{
    // Tell the CC about the new font
    SendMessageW (hWndCC, WM_SETFONT, (WPARAM) hFont, MAKELPARAM (TRUE, 0));

    // Repaint the CC text
    InvalidateRect (hWndCC, NULL, TRUE);
} // End ApplyNewFontCC


//***************************************************************************
//  $CreateNewFontLW
//
//  Create a new font for the LW window.
//***************************************************************************

void CreateNewFontLW
    (UBOOL bApply)                      // TRUE iff we should apply the new font

{
    // Call common routine to set various variables
    CreateNewFontCom (&hFontLW,
                       FONTENUM_LW,
                      &lfLW,
                      &cfLW,
                      &tmLW,
                      &GetFSDirAveCharSize (FONTENUM_LW)->cx,
                      &GetFSDirAveCharSize (FONTENUM_LW)->cy);
    // If we are also applying the font, ...
    if (bApply)
        ApplyNewFontEnum (FONTENUM_LW);
} // End CreateNewFontLW


//***************************************************************************
//  $ApplyNewFontLW
//
//  Apply the LW font to the Language Window
//***************************************************************************

void ApplyNewFontLW
    (HFONT hFont)                   // Font handle to use

{
    // Update the Language Bar
    InitLanguageBand (SendMessageW (hWndRB, RB_IDTOINDEX, IDWC_LW_RB, 0));
} // End ApplyNewFontLW


//***************************************************************************
//  $CreateNewFontSM
//
//  Create a new font for the SM windows.
//***************************************************************************

void CreateNewFontSM
    (UBOOL bApply)                      // TRUE iff we should apply the new font

{
    // Call common routine to set various variables
    CreateNewFontCom (&hFontSM,
                       FONTENUM_SM,
                      &lfSM,
                      &cfSM,
                      &tmSM,
                      &GetFSDirAveCharSize (FONTENUM_SM)->cx,
                      &GetFSDirAveCharSize (FONTENUM_SM)->cy);
    // If we're applying, ...
    if (bApply)
        // Re-calculate the average character width
        GetFSDirAveCharSize (FONTENUM_SM)->cx = RecalcAveCharWidth (hFontSM);

    // Change the font name in the ComboBox in the Font Window
    InitFontName ();

    // Change the font style in the ComboBox in the Font Window
    InitFontStyle ();

    // Change the point size in the Listbox/UpDown Controls in the Font Window
    InitPointSize ();

    // If we are also applying the font, ...
    if (bApply)
        ApplyNewFontEnum (FONTENUM_SM);
} // End CreateNewFontSM


//***************************************************************************
//  $RecalcAveCharWidth
//***************************************************************************

long RecalcAveCharWidth
    (HFONT hFont)

{
#define TXTLEN  128

    HDC   hDC;
    WCHAR wszTemp[TXTLEN];
    SIZE  sz;
    HFONT hFontOld;

    // The following code is necessary as the code in CreateNewFontCom
    //   doesn't always calculate the correct average char width.
    // ***FIXME*** I don't understand why, but this corrects it.

    // Get a DC for the Session Manager
    hDC = MyGetDC (HWND_DESKTOP);

    // Set the font
    hFontOld =
      SelectObject (hDC, hFont);

    // Set the mapping mode
    SetMapMode (hDC, MM_TEXT);

    // Fill the temp string with spaces
    FillMemoryW (wszTemp, TXTLEN, L' ');

    // Get the size in pixels of TXTLEN spaces
    GetTextExtentPoint32W (hDC, wszTemp, TXTLEN, &sz);

    // Restore the old HFONT
    SelectObject (hDC, hFontOld);

    // We no longer need this resource
    MyReleaseDC (HWND_DESKTOP, hDC); hDC = NULL;

    // Return as the "new" average char width of this font
    return sz.cx / TXTLEN;
#undef  TXTLEN
} // End RecalcAveCharWidth


//***************************************************************************
//  $InitFontName
//
//  Initialize the Font Name in the Font Window ComboBox
//***************************************************************************

void InitFontName
    (void)

{
    APLU3264 uItem;                 // Index of the current item

    // Find the default SM font
    uItem =
      SendMessageW (hWndCBFN_FW, CB_FINDSTRINGEXACT, (WPARAM) -1, (LPARAM) lfSM.lfFaceName);

    // If the default SM font is not found, ...
    if (uItem EQ CB_ERR)
        // Add it into the Listbox
        uItem =
          SendMessageW (hWndCBFN_FW, CB_ADDSTRING, 0, (LPARAM) lfSM.lfFaceName);
    // Set the current selection
    SendMessageW (hWndCBFN_FW, CB_SETCURSEL, uItem, 0);
} // End InitFontName


//***************************************************************************
//  $InitFontStyle
//
//  Initialize the Font Style in the Font Window ComboBox
//***************************************************************************

void InitFontStyle
    (void)

{
    UINT uItem;         // Index of the current item

    // Calculate the current selection
    //   (0 = Regular, 1 = Italic, 2 = Bold, 3 = Bold Italic)
    uItem = (lfSM.lfWeight >= FW_BOLD) * 2
          +  lfSM.lfItalic;

    // Change the font style in the ComboBox in the Font Window
    SendMessageW (hWndCBFS_FW, CB_SETCURSEL, uItem, 0);
} // End InitFontStyle


//***************************************************************************
//  $InitPointSize
//
//  Initialize the Point Size in the Font Window Listbox/UpDown Controls
//***************************************************************************

void InitPointSize
    (void)

{
    // Set the UpDown Ctrl position
    SendMessageW (hWndUD_FW, UDM_SETPOS32, 0, cfSM.iPointSize / 10);
} // End InitPointSize


//***************************************************************************
//  $ApplyNewFontSM
//
//  Apply the SM font to the appropriate windows
//***************************************************************************

void ApplyNewFontSM
    (HFONT hFont)                   // Font handle to use

{
    ENUMSETFONTW enumSetFontW;
#ifndef UNISCRIBE
    LOGFONTW     lfAlt;
    TEXTMETRIC   tmAlt;
#endif

    // Initialize the struct
    enumSetFontW.lpwClassName = LSMWNDCLASS;
    enumSetFontW.hFont        = hFont;

    // Refont the SM windows
    EnumChildWindows (hWndMF, &EnumCallbackSetFontW, (LPARAM) &enumSetFontW);

    // Refont the Language Window in the Rebar Ctrl
    InvalidateRect (hWndLW_RB, NULL, FALSE);

#ifndef UNISCRIBE
    // Copy the SM LOGFONTW & TEXTMETRICS
    lfAlt = lfSM;
    tmAlt = tmSM;

    // Change the font name
    strcpyW (lfAlt.lfFaceName, DEF_ASFONTNAME);

    // Re-specify the alternate SM font
    CreateNewFontCom (&hFontAlt,
                       -1,
                      &lfAlt,
                      &cfSM,
                      &tmAlt,
                       NULL,
                       NULL);
#endif
#ifdef DEBUG
    // Initialize the struct
    enumSetFontW.lpwClassName = LDBWNDCLASS;
    enumSetFontW.hFont        = hFont;

    // Refont the DB windows
    EnumChildWindows (hWndMF, &EnumCallbackSetFontW, (LPARAM) &enumSetFontW);
#endif
} // End ApplyNewFontSM


//***************************************************************************
//  $CreateNewFontPR
//
//  Create a new font for the Printer
//***************************************************************************

void CreateNewFontPR
    (UBOOL bApply)                      // TRUE iff we should apply the new font

{
    // Call common routine to set various variables
    CreateNewFontCom (&hFontPR,
                       FONTENUM_PR,
                      &lfPR,
                      &cfPR,
                      &tmPR,
                      &GetFSDirAveCharSize (FONTENUM_PR)->cx,
                      &GetFSDirAveCharSize (FONTENUM_PR)->cy);
    // If we are also applying the font, ...
    if (bApply)
        ApplyNewFontEnum (FONTENUM_PR);
} // End CreateNewFontPR


//***************************************************************************
//  $ApplyNewFontPR
//
//  Apply the PR font to the appropriate windows
//***************************************************************************

void ApplyNewFontPR
    (HFONT hFont)                   // Font handle to use

{
} // End ApplyNewFontPR


//***************************************************************************
//  $CreateNewFontFE
//
//  Create a new font for the FE windows.
//***************************************************************************

void CreateNewFontFE
    (UBOOL bApply)                      // TRUE iff we should apply the new font

{
    // Call common routine to set various variables
    CreateNewFontCom (&hFontFE,
                       FONTENUM_FE,
                      &lfFE,
                      &cfFE,
                      &tmFE,
                      &GetFSDirAveCharSize (FONTENUM_FE)->cx,
                      &GetFSDirAveCharSize (FONTENUM_FE)->cy);
    // If we are also applying the font, ...
    if (bApply)
        ApplyNewFontEnum (FONTENUM_FE);
} // End CreateNewFontFE


//***************************************************************************
//  $ApplyNewFontFE
//
//  Apply the FE font to the appropriate windows
//***************************************************************************

void ApplyNewFontFE
    (HFONT hFont)                   // Font handle to use

{
    ENUMSETFONTW enumSetFontW;

    // Initialize the struct
    enumSetFontW.lpwClassName = LFEWNDCLASS;
    enumSetFontW.hFont        = hFont;

    // Refont the FE windows
    EnumChildWindows (hWndMF, &EnumCallbackSetFontW, (LPARAM) &enumSetFontW);
} // End ApplyNewFontFE


//***************************************************************************
//  $CreateTooltip
//
//  Creates the Tooltip window and initializes it.
//***************************************************************************

HWND CreateTooltip
    (UBOOL bBalloon)            // TRUE iff we use balloon TTs

{
    HWND hWnd;

    // Create the Tooltip window
    hWnd =
      CreateWindowExW (0L,                  // Extended styles
                       WC_TOOLTIPSW,        // Class for MS Controls
                       NULL,                // Window title
                       0
                     | WS_POPUP

                     | TTS_NOANIMATE
                     | TTS_ALWAYSTIP
                     | (bBalloon ? TTS_BALLOON : TTS_NOFADE)
                     | TTS_NOPREFIX
                       ,                    // Styles
                       CW_USEDEFAULT,       // X-coord
                       CW_USEDEFAULT,       // Y-...
                       CW_USEDEFAULT,       // Width
                       CW_USEDEFAULT,       // Height
                       NULL,                // Parent window
                       NULL,                // Menu (should not have an ID)
                       _hInstance,          // Instance
                       NULL);               // No extra data
    if (hWnd EQ NULL)
        MB (pszNoCreateTTWnd);

    return hWnd;
} // End CreateTooltip


//***************************************************************************
//  $CreateChildWindows
//
//  Create the child windows under the parent window
//***************************************************************************

UBOOL CreateChildWindows
    (HWND hWndParent)           // Parent (Master Frame) window handle

{
    RECT rc;                    // Rectangle for setting size of window

    // Get the size and position of the parent window.
    GetClientRect (hWndParent, &rc);

    //***************************************************************
    // Create the Tooltip window first so that
    // the other windows can reference it.
    //***************************************************************
    hWndTT = CreateTooltip (TRUE);
    if (hWndTT EQ NULL)
        return FALSE;       // Stop the whole process

    //***************************************************************
    // Create the crash control window
    //***************************************************************
    hWndCC =
      CreateWindowExW (0L,                  // Extended styles
                       LCCWNDCLASS,         // Class
                       wszCCTitle,          // Window title (for debugging purposes only)
                       0
                     | WS_OVERLAPPEDWINDOW
                       ,                    // Styles
                       CW_USEDEFAULT, CW_USEDEFAULT,    // X- and Y-coord
                       CW_USEDEFAULT, CW_USEDEFAULT,    // X- and Y-size
                       hWndParent,          // Parent window
                       NULL,                // Menu
                       _hInstance,          // Instance
                       NULL);               // No extra data
    if (hWndCC EQ NULL)
    {
        MB (pszNoCreateCCWnd);
        return FALSE;
    } // End IF

    //***************************************************************
    // Create the Rebar control and all its bands
    //***************************************************************
    if (!CreateEntireRebarCtrl (hWndParent))
        return FALSE;

    //***************************************************************
    // Create the tab control window
    //***************************************************************
    hWndTC =
      CreateWindowExW (0L,                  // Extended styles
                       WC_TABCONTROLW,      // Class
                       wszTCTitle,          // Window title (for debugging purposes only)
                       0
                     | WS_CHILD
                     | WS_CLIPSIBLINGS
                     | WS_VISIBLE

/////////////////////| TCS_FOCUSNEVER
                     | TCS_OWNERDRAWFIXED
                     | TCS_TOOLTIPS
                       ,                    // Styles
                       rc.left,             // X-coord
                       rc.top,              // Y-coord
                       rc.right - rc.left,  // X-size
                       rc.bottom - rc.top,  // Y-size
                       hWndParent,          // Parent window
               (HMENU) IDWC_TC,             // Window ID
                       _hInstance,          // Instance
                       NULL);               // No extra data
    if (hWndTC EQ NULL)
    {
        MB (pszNoCreateTCWnd);
        return FALSE;
    } // End IF

    // Subclass the Tab Control so we can handle some of its messages
    (HANDLE_PTR) lpfnOldTabCtrlWndProc =
      SetWindowLongPtrW (hWndTC,
                         GWLP_WNDPROC,
                         (APLU3264) (HANDLE_PTR) (WNDPROC) &LclTabCtrlWndProc);
    // Show and paint the window
    ShowWindow (hWndTC, SW_SHOWNORMAL);
    UpdateWindow (hWndTC);

    // Save as the owner in the CHOOSEFONT struc
    cfTC.hwndOwner = hWndTC;

    //***************************************************************
    // Create the Status Window
    //***************************************************************
    if (!MakeStatusWindow (hWndParent))
    {
        MB (pszNoCreateSTWnd);
        return FALSE;
    } // End IF

    return TRUE;            // Tell 'em it worked
} // End CreateChildWindows


//***************************************************************************
//  $EnumCallbackRestoreAll
//
//  EnumChildWindows callback to restore all MDI Child windows
//
//  lParam = unused
//***************************************************************************

UBOOL CALLBACK EnumCallbackRestoreAll
    (HWND   hWnd,           // Handle to child window
     LPARAM lParam)         // Application-defined value

{
    HWND hWndMC;            // Window handle to MDI Client

    // Get the window handle to the MDI Client
    hWndMC = GetParent (hWnd);

    // When an MDI child window is minimized, Windows creates two windows: an
    // icon and the icon title.  The parent of the icon title window is set to
    // the MDI client window, which confines the icon title to the MDI client
    // area.  The owner of the icon title is set to the MDI child window.
    if (GetWindow (hWnd, GW_OWNER) NE NULL)     // If it's an icon title window, ...
        return TRUE;                    // skip it, and continue enumerating

    // Restore the window in case it's an icon
    SendMessageW (hWndMC, WM_MDIRESTORE, (WPARAM) hWnd, 0);

    return TRUE;                        // Continue enumerating
} // End EnumCallbackRestoreAll


//***************************************************************************
//  $MF_Create
//
//  Perform window-specific initialization
//***************************************************************************

void MF_Create
    (HWND hWnd)

{
    // Read the default scrollbar width
    iScrollSize = GetSystemMetrics (SM_CXVSCROLL);
} // End MF_Create


//***************************************************************************
//  $MF_Delete
//
//  Perform window-specific uninitialization
//***************************************************************************

void MF_Delete
    (HWND hWnd)

{
} // End MF_Delete


//***************************************************************************
//  $MFWndProc
//
//  Message processing routine for the Master Frame window
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MFWndProc"
#else
#define APPEND_NAME
#endif

LRESULT APIENTRY MFWndProc
    (HWND   hWnd,       // Window handle
     UINT   message,    // Type of message
     WPARAM wParam,     // Additional information
     LPARAM lParam)     // ...

{
    RECT         rcDtop;        // Rectangle for desktop
    HWND         hWndActive,    // Active window handle
                 hWndMC,        // MDI Client ...
                 hWndSM;        // Session Manager ...
    UINT         uCnt;          // Loop counter
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    static HWND  hWndTC_TT;     // Window handle for TT in TC
////static DWORD aHelpIDs[] = {
////                           IDOK,             IDH_OK,
////                           IDCANCEL,         IDH_CANCEL,
////                           IDC_APPLY,        IDH_APPLY,
////                           IDC_HELP2,        IDH_HELP2,
////                           0,             0,
////                          };

////LCLODSAPI ("MF: ", hWnd, message, wParam, lParam);
    switch (message)
    {
        case WM_NCCREATE:       // lpcs = (LPCREATESTRUCTW) lParam
            hWndMF = hWnd;

            hMenuSM = LoadMenu (_hInstance, MAKEINTRESOURCE (IDR_SMMENU));
            hMenuFE = LoadMenu (_hInstance, MAKEINTRESOURCE (IDR_FEMENU));
////////////hMenuME = LoadMenu (_hInstance, MAKEINTRESOURCE (IDR_MEMENU));
////////////hMenuVE = LoadMenu (_hInstance, MAKEINTRESOURCE (IDR_VEMENU));

            hMenuSMWindow = GetSubMenu (hMenuSM, IDMPOS_SM_WINDOW);
            hMenuFEWindow = GetSubMenu (hMenuFE, IDMPOS_FE_WINDOW);
////////////hMenuMEWindow = GetSubMenu (hMenuME, IDMPOS_ME_WINDOW);
////////////hMenuVEWindow = GetSubMenu (hMenuVE, IDMPOS_VE_WINDOW);

            break;                  // Continue with next handler

        case WM_CREATE:
            // Create the child windows
            if (CreateChildWindows (hWnd) EQ FALSE)
                return -1;          // Stop the whole process

            // Read in window-specific .ini file entries
            ReadIniFileWnd (hWnd);

            // *************** Bitmaps *********************************
            hBitmapCheck  = MyLoadBitmap (NULL, MAKEINTRESOURCE (OBM_CHECK));
            if (hBitmapCheck NE NULL)
                GetObjectW (hBitmapCheck, sizeof (BITMAP), (LPVOID) &bmCheck);

            // *************** Image Lists *****************************
            hImageListTC =
              ImageList_Create (IMAGE_TC_CX,    // Common width in pixels
                                IMAGE_TC_CY,    // ...    height ...
                                0
                              | ILC_COLOR32
                              | ILC_MASK,       // Flags
                                1,              // Max # images
                                0);             // # images by which the list can grow
            if (hImageListTC EQ NULL)
                return -1;          // Stop the whole process

            // Add the Close button icon to the image list
            ImageList_AddIcon (hImageListTC, hIconClose);

            // Assign the image list to the tab control
            SendMessageW (hWndTC, TCM_SETIMAGELIST, 0, (LPARAM) hImageListTC);

////        // Ensure the position is valid
////        if (MFPosCtr.x > rcDtop.right)  // If center is right of right, ...
////            MFPosCtr.x = rcDtop.right;
////        if (MFPosCtr.y > rcDtop.bottom) // If center is below the bottom, ...
////            MFPosCtr.y = rcDtop.bottom;
////
            // Check the last SizeState
            if (MFSizeState EQ SIZE_MAXIMIZED)
                nMinState = SW_MAXIMIZE;

            // Reposition the window to previous center & size
            MoveWindowPosSize (hWnd, MFPosCtr, MFSize);

            // Show and paint the window
            ShowWindow (hWnd, SW_SHOWNORMAL);
            UpdateWindow (hWnd);

            // Initialize window-specific resources
            MF_Create (hWnd);

            // Load a CLEAR WS or the workspace named on the command line
            if (!CreateNewTab (hWnd,
                               wszLoadFile,
                               TabCtrl_GetItemCount (hWndTC),
                               wszLoadFile[0] NE WC_EOS))
                return -1;          // Stop the whole process

            // *************** Fonts ***********************************
            // Create a new font for various windows
            CreateNewFontSM (TRUE);
            CreateNewFontFE (TRUE);
            CreateNewFontPR (TRUE);
            CreateNewFontCC (TRUE);
            CreateNewFontTC (TRUE);
            CreateNewFontLW (TRUE);

            // If we're to check for updates, ...
            if (guUpdFrq NE ENUM_UPDFRQ_NEVER)
            {
                SYSTEMTIME stCur;
                FILETIME   ftCur,
                           ftUpdChk;
                APLUINT    aplCur,
                           aplUpdChk;
                USPLIT     uSplit;

                // Get the current system date
                GetSystemDate (&stCur);

                // Convert the SYSTEMTIMEs to FILETIMEs
                SystemTimeToFileTime (&stCur,     &ftCur);
                SystemTimeToFileTime (&gstUpdChk, &ftUpdChk);

                // Convert the FILETIMEs to 64-bit integers
                uSplit.lo = ftCur.dwLowDateTime;
                uSplit.hi = ftCur.dwHighDateTime;
                aplCur    = uSplit.aplInt;

                uSplit.lo = ftUpdChk.dwLowDateTime;
                uSplit.hi = ftUpdChk.dwHighDateTime;
                aplUpdChk = uSplit.aplInt;

                // Izit time to check for updates?
                if (gstUpdChk.wYear EQ 0
                 || aplCur >= (aplUpdChk + updFrq[guUpdFrq].aplElap))
                    // Post a message to do the check
                    PostMessageW (hWnd, WM_TIMER, ID_TIMER_UPDCHK, 0);
                else
                    // Start the timer
                    SetUpdChkTimer ();
            } // End IF

            break;                  // Continue with next handler

        case WM_TIMER:                      // wTimerID = wParam            // Timer identifier
                                            // tmpc = (TIMERPROC *) lParam  // Ptr to timer callback
            if (wParam EQ ID_TIMER_UPDCHK)
                // Check for updates, but don't display the dialog unless there's a new version
                CheckForUpdates (FALSE, FALSE);

            return FALSE;           // We handled the msg

        case WM_SYSCOLORCHANGE:
        case WM_SETTINGCHANGE:
        {
            ENUMPASSMSG enumPassMsg;

            // Save parameters in struc
            enumPassMsg.message = message;
            enumPassMsg.wParam  = wParam;
            enumPassMsg.lParam  = lParam;

            // Enumerate all child windows of the Tab Control
            //   which should give us all MDI Client windows
            EnumChildWindows (hWndTC, &EnumCallbackPassMsg, (LPARAM) &enumPassMsg);

            // Tell the Tooltip window about it
            PostMessageW (hWndTT, message, wParam, lParam);

            // Uninitialize window-specific resources
            MF_Delete (hWnd);

            // Initialize window-specific resources
            MF_Create (hWnd);

            break;                  // Continue with next handler
        } // End WM_SYSCOLORCHANGE/WM_SETTINGCHANGE

        case MYWM_RESIZE:
        {
            RECT rc;

            // Get the size of our Client Area
            GetClientRect (hWnd, &rc);

            // Resize the Master Frame so as to recalculate the size of the
            //  child windows
            PostMessageW (hWnd, WM_SIZE, SIZE_RESTORED, MAKELPARAM (rc.right - rc.left, rc.bottom - rc.top));

            return FALSE;           // We handled the msg
        } // End MYWM_RESIZE

#define fwSizeType  wParam
#define nWidth      (LOWORD (lParam))
#define nHeight     (HIWORD (lParam))
        case WM_SIZE:               // fwSizeType = wParam;      // Resizing flag
                                    // nWidth = LOWORD(lParam);  // Width of client area
                                    // nHeight = HIWORD(lParam); // Height of client area
            if (fwSizeType EQ SIZE_MAXIMIZED
             || fwSizeType EQ SIZE_RESTORED)
            {
                SIZE S;
                HDWP hdwp;
                RECT rc,                    // Rectangle for new MF client area
                     rcRB;                  // ...           Rebar Ctrl
                UINT uHeightRB,             // Total height of Rebar ctrl
                     uTabCnt;               // # tabs in Tab Ctrl

                // Calculate the display rectangle, assuming the
                //   tab control is the size of the client area
                //   less the height of the Rebar Ctrl
                SetRect (&rc,
                          0, 0,
                          nWidth, nHeight);
                // Run through all tabs (and hence all MDI Client Windows)
                uTabCnt = TabCtrl_GetItemCount (hWndTC);

                // Start the defer process ("2 +" for Rebar Ctrl and Tab Ctrl)
                hdwp = BeginDeferWindowPos (2 + uTabCnt);

                // Get the outer dimensions of the Rebar Ctrl
                GetWindowRect (hWndRB, &rcRB);

                // Get the height of the Rebar Ctrl
                uHeightRB = rcRB.bottom - rcRB.top;

                // Size the Rebar control
                DeferWindowPos (hdwp,           // Handle to internal structure
                                hWndRB,         // Handle of window to position
                                NULL,           // Placement-order handle
                                rc.left,        // X-position
                                rc.top,         // Y-position
                                rc.right - rc.left, // X-size
                                uHeightRB,      // Y-size
                                0
                              | SWP_NOMOVE      // Ignore X- and Y-position
                              | SWP_NOZORDER    // Window-positioning flags
                              | SWP_SHOWWINDOW
                               );

                // Skip over the Rebar Ctrl
                rc.top += uHeightRB;

                // Adjust the rectangle top to overcome a bug in Wine's handling of Tab Ctrls
                rc.top--;
                rc.top--;

                // Size the tab control
                DeferWindowPos (hdwp,           // Handle to internal structure
                                hWndTC,         // Handle of window to position
                                NULL,           // Placement-order handle
                                rc.left,        // X-position
                                rc.top,         // Y-position
                                rc.right - rc.left, // X-size
                                rc.bottom - rc.top, // Y-size
                                0
                              | SWP_NOZORDER    // Window-positioning flags
                              | SWP_SHOWWINDOW
                               );
                //  Calculate the client rectangle for the MDI client windows
                //    in Master Frame client area coords using the incoming
                //   rectangle in <rc>.
                CalcClientRectMC (&rc, FALSE);

                // If the Status Bar is visible, ...
                if (OptionFlags.bViewStatusBar)
                {
                    RECT rcStatus;

                    // Get the window size of the Status Window
                    GetWindowRect (hWndStatus, &rcStatus);

                    // Reduce the bottom of the MDI Child window
                    //   by the height of the Status Window
                    rc.bottom -= (rcStatus.bottom - rcStatus.top);
                } // End IF

                // Loop through the tabs
                for (uCnt = 0; uCnt < uTabCnt; uCnt++)
                {
                    // Get the ptr to this tab's PerTabData global memory
                    lpMemPTD = GetPerTabPtr (uCnt);

                    // If it and the MDI Client's window handle are both valid, ...
                    if (lpMemPTD NE NULL && lpMemPTD->hWndMC NE NULL)
                        // Position and size the MDI Child window to fit the
                        // tab control's display area
                        DeferWindowPos (hdwp,               // Handle to internal structure
                                        lpMemPTD->hWndMC,   // Handle of window to position
                                        NULL,               // Placement-order handle
                                        rc.left,            // X-position
                                        rc.top,             // Y-position
                                        rc.right - rc.left, // X-size
                                        rc.bottom - rc.top, // Y-size
                                        SWP_NOZORDER);      // Window-positioning flags
                } // End FOR

                // Tell the status window about the new Status Parts right edge
                SetStatusParts (rc.right - rc.left);

                // Position and size the Status Window
                SendStatusMsg (WM_SIZE, wParam, lParam);

                // Paint the windows
                EndDeferWindowPos (hdwp);

                // Save the current Maximized or Normal state
                MFSizeState = (UINT) wParam;

                S.cx = LOWORD (lParam);
                S.cy = HIWORD (lParam);

                // Convert client area size to window size
                MFSize = ClientToWindowSize (hWnd, S);

                // Because we track the center position of the window,
                // we need to modify that as well.  Note that we needn't
                // specify lParam as our MYWM_MOVE code doesn't use it.
                PostMessageW (hWnd, MYWM_MOVE, 0, 0);

                // Normally we pass this message on to the MDI Child Window,
                //   so it can resize itself, however, in this case we're
                //   handling the hWndMC sizing.
                return FALSE;           // We handled the msg
            } // End IF

            break;                  // *MUST* pass on to DefFrameProcW
#undef  nHeight
#undef  nWidth
#undef  fwSizeType

        case MYWM_MOVE:
            if (!IsIconic (hWnd))   // If we're not minimized, otherwise
            {                       // the positions are of the icon
                GetWindowRect (hWnd, &rcDtop);  // Get current window positions
                MFPosCtr.x = rcDtop.left + MFSize.cx / 2; // Convert to center coords
                MFPosCtr.y = rcDtop.top  + MFSize.cy / 2; // ...
            } // End IF

            return FALSE;           // We handled the msg

        case WM_MOVE:
            PostMessageW (hWnd, MYWM_MOVE, 0, 0);

            break;                  // Continue with next handler

        case WM_NOTIFY:             // idCtrl = (int) wParam;
                                    // lpnmhdr = (LPNMHDR) lParam;
        {
#ifdef DEBUG
            LPNMHDR     lpnmhdr = (LPNMHDR)     lParam;
            LPNMTOOLBAR lpnmtb  = (LPNMTOOLBAR) lParam;
#else
  #define lpnmhdr   ((LPNMHDR) lParam)
  #define lpnmtb    ((LPNMTOOLBAR) lParam)
#endif
            // Split cases based upon the notification code
            switch (lpnmhdr->code)
            {
                APLI3264          nButtons,         // # buttons
                                  iCnt;             // Loop counter
                static APLI3264   nSavedButtons;    // # saved buttons
                static LPTBBUTTON lpSavedButtons;   // Ptr to saved button state

                //***************************************************************
                // Rebar Ctrl Notifications
                //***************************************************************
                case RBN_HEIGHTCHANGE:
                    // Resize the Master Frame so as to recalculate the size of the
                    //  child windows
                    PostMessageW (hWnd, MYWM_RESIZE, 0, 0);

                    break;

                //***************************************************************
                // Toolbar Ctrl Notifications
                //***************************************************************
                case TBN_BEGINADJUST:       // Save the initial state
                    // Get the current button count
                    nSavedButtons =
                      SendMessageW (lpnmtb->hdr.hwndFrom, TB_BUTTONCOUNT, 0, 0);

                    // Allocate memory to store initial state info
                    lpSavedButtons =
                      DbgGlobalAlloc (GPTR, nSavedButtons * sizeof (TBBUTTON));

                    // Save the current state
                    for (iCnt = 0; iCnt < nSavedButtons; iCnt++)
                        SendMessageW (lpnmtb->hdr.hwndFrom, TB_GETBUTTON, iCnt, (LPARAM) &lpSavedButtons[iCnt]);

                    return TRUE;

                case TBN_ENDADJUST:         // Free initial state memory
                    DbgGlobalFree (lpSavedButtons); lpSavedButtons = NULL;

                    return TRUE;

                case TBN_INITCUSTOMIZE:     // Tell the ctrl to hide the Help button
                    return TBNRF_HIDEHELP;

                case TBN_QUERYINSERT:       // wParam unused
                                            // lpnmtb = (LPNMTOOLBAR) lParam;
                    return TRUE;            // Return TRUE iff the button can be inserted
                                            //   be inserted
                case TBN_QUERYDELETE:       // wParam unused
                                            // lpnmtb = (LPNMTOOLBAR) lParam;
                    return TRUE;            // Return TRUE iff the button can be deleted

                case TBN_RESET:             // wParam unused
                                            // lpnmtb = (LPNMTOOLBAR) lParam;
                    // Split cases based upon the Window ID
                    switch (lpnmtb->hdr.idFrom)
                    {
                        case IDWC_WS_RB:
                        case IDWC_ED_RB:
                        case IDWC_OW_RB:
                            // Get the current button count
                            nButtons =
                              SendMessageW (lpnmtb->hdr.hwndFrom, TB_BUTTONCOUNT, 0, 0);

                            // Remove all of the existing buttons starting with the
                            // last and working down (because that's what MSDN does)
                            for (iCnt = nButtons - 1; iCnt >= 0; iCnt--)
                                SendMessageW (lpnmtb->hdr.hwndFrom, TB_DELETEBUTTON, iCnt, 0);

                            // Restore the saved buttons
                            SendMessageW (lpnmtb->hdr.hwndFrom, TB_ADDBUTTONS, nSavedButtons, (LPARAM) lpSavedButtons);

                            return TRUE;

                        default:
                            break;
                    } // End SWITCH

                    break;

                case TBN_DROPDOWN:          // wParam unused
                                            // lpnmtb = (LPNMTOOLBAR) lParam;
                    // Split cases based upon the Window ID
                    switch (lpnmtb->hdr.idFrom)
                    {
                        RECT      rc;                   // Button rectangle
                        HMENU     hMenu;                // Popup menu handle
                        TPMPARAMS tpm;                  // TrackPopupMenuEx data struc
                        HWND      hWndEC;               // EditCtrl window handle
                        DWORD     dwSelBeg,             // Selection beginning
                                  dwSelEnd;             // ...       end
                        UINT      mfState;              // Menu state
                        HGLOBAL   hGlbClip;             // Handle to clipboard data

                        case IDWC_WS_RB:
                        case IDWC_ED_RB:
                        case IDWC_OW_RB:
                            // Get the coordinates of the button
                            SendMessageW (lpnmtb->hdr.hwndFrom, TB_GETRECT, (WPARAM) lpnmtb->iItem, (LPARAM) &rc);

                            // Convert to screen coordinates
                            MapWindowPoints (lpnmtb->hdr.hwndFrom, HWND_DESKTOP, (LPPOINT) &rc, 2);

                            // Create a popup menu
                            hMenu = CreatePopupMenu ();

                            // Split cases based upon the item
                            switch (lpnmtb->iItem)
                            {
                                case IDM_LOAD_WS:
                                case IDM_XLOAD_WS:
                                case IDM_COPY_WS:
                                {
                                    WCHAR (*lpwszRecentFiles)[][_MAX_PATH]; // Ptr to list of recent files
                                    LPWCHAR lpwszWSID;                      // Ptr to current recent file
                                    UINT    uIDBase;                        // ID base (IDM_LOAD_BASE/IDM_XLOAD_BASE/IDM_COPY_BASE)

                                    // Get the ID base
                                    switch (lpnmtb->iItem)
                                    {
                                        case IDM_LOAD_WS:
                                            uIDBase = IDM_LOAD_BASE;

                                            break;

                                        case IDM_XLOAD_WS:
                                            uIDBase = IDM_XLOAD_BASE;

                                            break;

                                        case IDM_COPY_WS:
                                            uIDBase = IDM_COPY_BASE;

                                            break;

                                        defstop
                                            break;
                                    } // End SWITCH

                                    // Lock the memory to get a ptr to it
                                    lpwszRecentFiles = MyGlobalLock (hGlbRecentFiles);

                                    // Fill in the popup menu with a list of recently
                                    //   )LOADed/)XLOADed/)COPYed WSs
                                    for (uCnt = 0; uCnt < uNumRecentFiles; uCnt++)
                                    {
                                        // Get a ptr to the current recent file
                                        lpwszWSID = (*lpwszRecentFiles)[uCnt];

                                        if (lpwszWSID[0] EQ WC_EOS)
                                            break;

                                        // Append the file name, shortened if appropriate
                                        AppendMenuW (hMenu,                     // Handle
                                                     0                          // Flags
                                                   | MF_STRING
                                                   | MF_ENABLED,
                                                     uIDBase + uCnt,            // ID
                                                     ShortenWSID (lpwszWSID));  // Text
                                    } // End FOR

                                    // We no longer need this ptr
                                    MyGlobalUnlock (hGlbRecentFiles); lpwszRecentFiles = NULL;

                                    break;
                                } // End IDM_LOAD/XLOAD/COPY_WS

                                case IDM_CUT:
                                case IDM_COPY:
                                    // Get the active EditCtrl window handle
                                    hWndEC = GetActiveEC (hWndTC);

                                    // Determine whether or not the EditCtrl has a selection
                                    SendMessageW (hWndEC, EM_GETSEL, (WPARAM) &dwSelBeg, (LPARAM) &dwSelEnd);

                                    // Set the menu state depending upon whether or not there is a selection
                                    mfState = MF_STRING | ((dwSelBeg NE dwSelEnd) ? MF_ENABLED : MF_GRAYED);

                                    // Fill in the popup menu with a list of cut/copy choices
                                    AppendMenuW (hMenu,                 // Handle
                                                 mfState,               // Flags
                                                 IDM_COPY_APLWIN,       // ID
                                                 L"Copy APL+&Win");     // Text
                                    AppendMenuW (hMenu,                 // Handle
                                                 mfState,               // Flags
                                                 IDM_COPY_APL2,         // ID
                                                 L"Copy APL&2");        // Text
                                    AppendMenuW (hMenu,                 // Handle
                                                 mfState,               // Flags
                                                 IDM_COPY_DYALOG,       // ID
                                                 L"Copy &Dyalog");      // Text
                                    AppendMenuW (hMenu,                 // Handle
                                                 mfState,               // Flags
                                                 IDM_COPY_ISO,          // ID
                                                 L"Copy &ISO");         // Text
                                    AppendMenuW (hMenu,                 // Handle
                                                 mfState,               // Flags
                                                 IDM_COPY_PC3270,       // ID
                                                 L"Copy &PC3270");      // Text
                                    AppendMenuW (hMenu,                 // Handle
                                                 mfState,               // Flags
                                                 IDM_COPY_BRACES,       // ID
                                                 L"Copy &Braces");      // Text
                                    break;

                                case IDM_PASTE:
                                    // Open the clipboard so we can read from it
                                    OpenClipboard (NULL);

                                    // Get a handle to the clipboard data
                                    hGlbClip = GetClipboardData (CF_PRIVATEFIRST);
                                    if (hGlbClip EQ NULL)
                                        hGlbClip = GetClipboardData (CF_UNICODETEXT);

                                    // We're done with the clipboard
                                    CloseClipboard ();

                                    // Set the menu state depending upon whether or not the clipboard has data
                                    mfState = MF_STRING | (hGlbClip ? MF_ENABLED : MF_GRAYED);

                                    // Fill in the popup menu with a list of cut/copy choices
                                    AppendMenuW (hMenu,                 // Handle
                                                 mfState,               // Flags
                                                 IDM_PASTE_APLWIN,      // ID
                                                 L"Paste APL+&Win");    // Text
                                    AppendMenuW (hMenu,                 // Handle
                                                 mfState,               // Flags
                                                 IDM_PASTE_APL2,        // ID
                                                 L"Paste APL&2");       // Text
                                    AppendMenuW (hMenu,                 // Handle
                                                 mfState,               // Flags
                                                 IDM_PASTE_DYALOG,      // ID
                                                 L"Paste &Dyalog");     // Text
                                    AppendMenuW (hMenu,                 // Handle
                                                 mfState,               // Flags
                                                 IDM_PASTE_ISO,         // ID
                                                 L"Paste &ISO");        // Text
                                    AppendMenuW (hMenu,                 // Handle
                                                 mfState,               // Flags
                                                 IDM_PASTE_PC3270,      // ID
                                                 L"Paste &PC3270");     // Text
                                    AppendMenuW (hMenu,                 // Handle
                                                 mfState,               // Flags
                                                 IDM_PASTE_BRACES,      // ID
                                                 L"Paste &Braces");     // Text
                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            // Set up the popup menu
                            // Set rcExclude equal to the button rectangle so that if the toolbar
                            //   is too close to the bottom of the screen, the menu will appear above
                            //   the button rather than below it.
                            tpm.cbSize = sizeof (TPMPARAMS);
                            tpm.rcExclude = rc;

                            // Show the menu and wait for input
                            // If the user selects an item, its WM_COMMAND is sent
                            TrackPopupMenuEx (hMenu,                // Handle
                                              0                     // Flags
                                            | TPM_CENTERALIGN
                                            | TPM_RIGHTBUTTON
                                            | TPM_VERTICAL
                                              ,
                                              rc.left,              // X-position
                                              rc.bottom,            // Y-position
                                              hWnd,                 // Parent
                                             &tpm);                 // Ptr to extra parameters (exclude region)
                            // Free the menu resources
                            DestroyMenu (hMenu);

                            return FALSE;               // We handled the msg

                        default:
                            break;
                    } // End SWITCH

                    break;

                //***************************************************************
                // Tab Ctrl Notifications
                //***************************************************************
                case TCN_SELCHANGING:       // idTabCtl = (int) LOWORD(wParam);
                                            // lpnmhdr = (LPNMHDR) lParam;
                    DestroyCaret ();        // 'cause we just lost the focus

                    // If the user clicked on the close button,
                    //   disallow this change so as to avoid
                    //   screen flicker
                    if (ClickOnClose ())
                        return TRUE;        // Prevent the selection from changing

                    // Save the index of the outgoing tab
                    gLstTabID = TranslateTabIndexToID (TabCtrl_GetCurSel (hWndTC));

                    return FALSE;           // Allow the selection to change

                case TCN_SELCHANGE:         // idTabCtl = (int) LOWORD(wParam);
                                            // lpnmhdr = (LPNMHDR) lParam;
                    // Call common code to show/hide the tab windows
                    TabCtrl_SelChange ();

                    return FALSE;       // We handled the msg

                //***************************************************************
                // Tooltip Ctrl Notifications
                //***************************************************************
////////////////case TTN_NEEDTEXTW:
                case TTN_GETDISPINFOW:      // idCtl = (int) wParam;
                                            // lptdi = (LPNMTTDISPINFOW) lParam;
                {
                    static WCHAR TooltipText[_MAX_PATH];
                    LPAPLCHAR    lpMemWSID;     // Ptr to []WSID global memory
#ifdef DEBUG
                    LPNMTTDISPINFOW lptdi = (LPNMTTDISPINFOW) lParam;
#else
  #define lptdi     ((LPNMTTDISPINFOW) lParam)
#endif
                    // In a Tooltip from a Tab Ctrl, the lptdi->hdr.idFrom
                    //   is the Tab index starting at zero.
                    // In order to distinguish a Tooltip from our Tab Ctrl
                    //   from any other Tooltip we have the Tab Ctrl create
                    //   its own Tooltip Ctrl (TCS_TOOLTIPS) and check for
                    //   that window handle.

                    // Ensure this is from our Tab Ctrl
                    if (hWndTC_TT EQ lpnmhdr->hwndFrom)
                    {
                        // Get a ptr to the ws name
                        lpMemWSID = PointToWsName ((APLU3264) (HANDLE_PTR) lptdi->hdr.idFrom);

                        // Get ptr to PerTabData global memory
                        lpMemPTD = GetPerTabPtr ((APLU3264) (HANDLE_PTR) lptdi->hdr.idFrom); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));
#ifndef DEBUG
                        // Return a ptr to the stored tooltip text
                        strcpyW (TooltipText, lpMemWSID);
#else
                        // Return a ptr to the stored tooltip text
                        MySprintfW (TooltipText,
                                    sizeof (TooltipText),
                                   L"hWndMC=%p  hWndSM=%p  TabID=%u  TabIndex=%d: %s",
                                    lpMemPTD->hWndMC,
                                    lpMemPTD->hWndSM,
                                    TranslateTabIndexToID ((int) lptdi->hdr.idFrom),
                                    lptdi->hdr.idFrom,
                                    lpMemWSID);
#endif
                        // If the tab is still executing, say so
                        if (lpMemPTD->bExecuting)
                            MyStrcatW (TooltipText, sizeof (TooltipText), L" (RUNNING)");

                        // Return the ptr to the caller
                        lptdi->lpszText = TooltipText;
                    } else
                        // Clear the text
                        lptdi->lpszText = L"";

                    return FALSE;
#ifndef DEBUG
  #undef  lptdi
#endif
                } // End TTN_GETDISPINFOW

                case NM_TOOLTIPSCREATED:        // idCtl = (UINT) wparam;
                                                // lpnmttc = (LPNMTOOLTIPSCREATED) lParam;
                {
                    DWORD dwStyle;              // Styles for TT
#ifdef DEBUG
                    UINT                idCtl   = (UINT) wParam;
                    LPNMTOOLTIPSCREATED lpnmttc = (LPNMTOOLTIPSCREATED) lParam;
#else
  #define idCtl     ((UINT) wParam)
  #define lpnmttc   ((LPNMTOOLTIPSCREATED) lParam)
#endif
                    // Split cases based upon the Window ID
                    switch (idCtl)
                    {
                        case IDWC_WS_RB:
                        case IDWC_ED_RB:
                        case IDWC_OW_RB:
                        case IDWC_TC:
                            // Get and set the TT's styles
                            dwStyle = GetWindowLong (lpnmttc->hwndToolTips, GWL_STYLE);

                            // Include more styles
                            dwStyle |= 0
                                     | TTS_ALWAYSTIP
                                     | TTS_BALLOON
                                     | TTS_NOPREFIX
                                       ;
                            // Tell the TT about this change
                            SetWindowLong (lpnmttc->hwndToolTips, GWL_STYLE, dwStyle);

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;
#ifndef DEBUG
  #undef  idCtl
  #undef  lpnmttc
#endif
                } // End NM_TOOLTIPSCREATED

                default:
                    break;
            } // End SWITCH
#ifndef DEBUG
  #undef  lpnmtb
  #undef  lpnmhdr
#endif
            break;                  // Continue with next handler
        } // End WM_NOTIFY

#define lpdis   (*(LPDRAWITEMSTRUCT *) &lParam)
        case WM_DRAWITEM:           // idCtl = (UINT) wParam;             // control identifier
                                    // lpdis = (LPDRAWITEMSTRUCT) lParam; // item-drawing information
            // Ensure this message is for our tab control
            if (lpdis->CtlType EQ ODT_TAB)
            {
                // Split cases based upon the item action
                switch (lpdis->itemAction)
                {
                    case ODA_DRAWENTIRE:
                        // Draw the tab
                        DrawTab (lpdis->hDC,
                                 lpdis->itemID,
                                &lpdis->rcItem);
                        return TRUE;    // We processed this msg

////////////////////case ODA_FOCUS:     // These actions don't appear to occur with
////////////////////                    //   an owner-drawn Tab Ctrl
////////////////////case ODA_SELECT:    // ...
////////////////////    break;
                } // End SWITCH
            } // End IF

            break;
#undef  lpdis

////////case WM_HELP:
////////{
////////    LPHELPINFO lpHI;
////////
////////    lpHI = (LPHELPINFO) lParam;
////////
////////    if (lpHI->iContextType EQ HELPINFO_WINDOW)  // Must be for control
////////    {
////////        WinHelpW (lpHI->hItemHandle,
////////                  wszHlpDPFE,
////////                  HELP_WM_HELP,
////////                  (DWORD) (LPVOID) aHelpIDs);
////////        fHelp = TRUE;
////////    } // End IF
////////
////////    return FALSE;           // We handled the message
////////
////////} // End WM_HELP
////////
////////case WM_CONTEXTMENU:        // hwnd = (HWND) wParam;
////////                            // xPos = LOSHORT (lParam); // Horizontal position of cursor
////////                            // yPos = HISHORT (lParam); // Vertical position of cursor
////////    if (hWndTreeView NE (HWND) wParam)
////////    {
////////        WinHelpW ((HWND) wParam,
////////                  wszHlpDPFE,
////////                  HELP_CONTEXTMENU,
////////                  (DWORD) (LPVOID) aHelpIDs);
////////        fHelp = TRUE;
////////    } // End IF
////////    return FALSE;           // We handled the message

        case WM_COMMAND:            // wNotifyCode = HIWORD (wParam); // notification code
                                    // wID = LOWORD (wParam);         // item, control, or accelerator identifier
                                    // hwndCtrl = (HWND) lParam;      // handle of control
        {
            UINT uCnt;              // Temporary counter
#ifdef DEBUG
            UINT idCtl    = GET_WM_COMMAND_ID   (wParam, lParam);
            UINT cmdCtl   = GET_WM_COMMAND_CMD  (wParam, lParam);
            HWND hwndCtrl = GET_WM_COMMAND_HWND (wParam, lParam);
#else
  #define idCtl               GET_WM_COMMAND_ID   (wParam, lParam)
  #define cmdCtl              GET_WM_COMMAND_CMD  (wParam, lParam)
  #define hwndCtl             GET_WM_COMMAND_HWND (wParam, lParam)
#endif
            // Get the window handle of the currently active MDI Client
            hWndMC = GetActiveMC (hWndTC);

            // Get the handle of the active MDI window
            hWndActive = (HWND) SendMessageW (hWndMC, WM_MDIGETACTIVE, 0, 0);

            switch (idCtl)
            {
                HMENU hMenu;                // Menu handle for View/Toolbars/...
                GETIDMPOS_XX GetIDMPOS_xx;  // Ptr to GetIDMPOS_xx function

                case IDM_EXIT:
                    PostMessageW (hWnd, WM_CLOSE, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_STATUSBAR:
                    // Toggle the state
                    OptionFlags.bViewStatusBar = !OptionFlags.bViewStatusBar;

                    // Get the IDMPOS_xx translator function
                    GetIDMPOS_xx = (GETIDMPOS_XX) GetPropW (hWndActive, PROP_IDMPOSFN);
                    Assert (GetIDMPOS_xx NE NULL);

                    // Check/uncheck the View | Status Bar menu item as appropriate
                    hMenu = GetMenu (hWnd);
                    hMenu = GetSubMenu (hMenu, (*GetIDMPOS_xx) (IDMPOSNAME_VIEW));
                    CheckMenuItem (hMenu,
                                   idCtl,
                                   MF_BYCOMMAND
                                 | (OptionFlags.bViewStatusBar ? MF_CHECKED : MF_UNCHECKED)
                                  );
                    ShowWindow (hWndStatus, OptionFlags.bViewStatusBar ? SW_SHOWNORMAL : SW_HIDE);
                    InvalidateRect (hWnd, NULL, FALSE);

                    // Resize the Master Frame so as to show/hide the Status Window
                    PostMessageW (hWnd, MYWM_RESIZE, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_TOGGLE_LNS_FCN:
                    // Toggle the Fcn Line #s display value for the active (FE) window
                    SetWindowLongW (hWndActive, GWLSF_FLN, !GetWindowLongW (hWndActive, GWLSF_FLN));

                    // Set the current state of the Object Toolbar's Toggle Fcn Line #s button
                    SetLineNumState (hWndActive);

                    return FALSE;       // We handled the msg

                case IDM_TB_WS:
                case IDM_TB_ED:
                case IDM_TB_OW:
                case IDM_TB_FW:
                case IDM_TB_LW:
                    // Calculate zero-origin index
                    uCnt = idCtl - IDM_TB_FIRST;

                    // Toggle the Rebar bands state
                    aRebarBands[uCnt].bShowBand = !aRebarBands[uCnt].bShowBand;

                    // Get the IDMPOS_xx translator function
                    GetIDMPOS_xx = (GETIDMPOS_XX) GetPropW (hWndActive, PROP_IDMPOSFN);
                    Assert (GetIDMPOS_xx NE NULL);

                    // Get the View menu handle
                    hMenu = GetMenu (hWnd);
                    hMenu = GetSubMenu (hMenu, (*GetIDMPOS_xx) (IDMPOSNAME_VIEW));

                    // Set the corresponding Toolbars menu checkmark and
                    //   show/hide the corresponding Rebar band
                    SetToolbarsMenu (hMenu, uCnt);

                    return FALSE;       // We handled the msg

                case IDM_UNDO:
                    SendMessageW (hWndActive, WM_UNDO, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_REDO:
                    SendMessageW (hWndActive, MYWM_REDO, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_CUT:
                    SendMessageW (hWndActive, WM_CUT, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_COPY:
                    SendMessageW (hWndActive, WM_COPY, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_COPY_APLWIN:
                    SendMessageW (hWndActive, MYWM_COPY_APL, UNITRANS_APLWIN, 0);

                    return FALSE;       // We handled the msg

                case IDM_COPY_APL2:
                    SendMessageW (hWndActive, MYWM_COPY_APL, UNITRANS_APL2, 0);

                    return FALSE;       // We handled the msg

                case IDM_COPY_DYALOG:
                    SendMessageW (hWndActive, MYWM_COPY_APL, UNITRANS_DYALOG, 0);

                    return FALSE;       // We handled the msg

                case IDM_COPY_ISO:
                    SendMessageW (hWndActive, MYWM_COPY_APL, UNITRANS_ISO, 0);

                    return FALSE;       // We handled the msg

                case IDM_COPY_PC3270:
                    SendMessageW (hWndActive, MYWM_COPY_APL, UNITRANS_PC3270, 0);

                    return FALSE;       // We handled the msg

                case IDM_COPY_BRACES:
                    SendMessageW (hWndActive, MYWM_COPY_APL, UNITRANS_BRACES, 0);

                    return FALSE;       // We handled the msg

                case IDM_PASTE:
                    SendMessageW (hWndActive, WM_PASTE, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_PASTE_APLWIN:
                    SendMessageW (hWndActive, MYWM_PASTE_APL, UNITRANS_APLWIN, 0);

                    return FALSE;       // We handled the msg

                case IDM_PASTE_APL2:
                    SendMessageW (hWndActive, MYWM_PASTE_APL, UNITRANS_APL2, 0);

                    return FALSE;       // We handled the msg

                case IDM_PASTE_DYALOG:
                    SendMessageW (hWndActive, MYWM_PASTE_APL, UNITRANS_DYALOG, 0);

                    return FALSE;       // We handled the msg

                case IDM_PASTE_ISO:
                    SendMessageW (hWndActive, MYWM_PASTE_APL, UNITRANS_ISO, 0);

                    return FALSE;       // We handled the msg

                case IDM_PASTE_PC3270:
                    SendMessageW (hWndActive, MYWM_PASTE_APL, UNITRANS_PC3270, 0);

                    return FALSE;       // We handled the msg

                case IDM_PASTE_BRACES:
                    SendMessageW (hWndActive, MYWM_PASTE_APL, UNITRANS_BRACES, 0);

                    return FALSE;       // We handled the msg

                case IDM_DELETE:
                    SendMessageW (hWndActive, WM_CLEAR, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_SELECTALL:
                    SendMessageW (hWndActive, MYWM_SELECTALL, 0, (LPARAM) -1);

                    return FALSE;       // We handled the msg

                case IDM_TILE_HORZ:
                    PostMessageW (hWndMC, WM_MDITILE, MDITILE_HORIZONTAL, 0);

                    return FALSE;       // We handled the msg

                case IDM_TILE_VERT:
                    PostMessageW (hWndMC, WM_MDITILE, MDITILE_VERTICAL, 0);

                    return FALSE;       // We handled the msg

                case IDM_CASCADE:
                    // In this case, we don't care whether or not there
                    // are any child windows as we're restoring them all anyway.
                    EnumChildWindows (hWndMC, EnumCallbackRestoreAll, 0);

                    PostMessageW (hWndMC, WM_MDICASCADE, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_ARRANGE_ICONS:
                    PostMessageW (hWndMC, WM_MDIICONARRANGE, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_CUSTOMIZE:
                    // If not already open, ...
                    if (ghDlgCustomize EQ NULL)
                        // Display a dialog with the choices
////////////////////////ghDlgCustomize =        // Set in the WM_INITDIALOG handler
                          CreateDialogParamW (_hInstance,
                                               MAKEINTRESOURCEW (IDD_CUSTOMIZE),
                                               hWndMF,
                                    (DLGPROC) &CustomizeDlgProc,
                                               0);
                    return FALSE;       // We handled the msg

                case IDM_HELP_CONTENTS:
                    WinHelpW (hWnd, wszHlpDPFE, HELP_INDEX, 0);

                    return FALSE;       // We handled the msg

                case IDM_TUTORIALS:
                    // Fill in the URL
#define wszTemp     L"http://aplwiki.com/PrimersAndTutorials"

                    // In case we need COM, ...
                    CoInitializeEx (NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

                    // Ask the system to load the web page
                    ShellExecuteW (hWnd,            // Parent window handle
                                   NULL,            // Operation (NULL = use default verb)
                                   wszTemp,         // The file to "open"
                                   NULL,            // Parameters
                                   NULL,            // Ptr to directory
                                   SW_SHOWNORMAL);  // Show command
                    return FALSE;       // We handled the msg
#undef  wszTemp

                case IDM_DOCUMENTATION:
                    // Fill in the URL
#define wszTemp     L"http://wiki.nars2000.org/"

                    // In case we need COM, ...
                    CoInitializeEx (NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

                    // Ask the system to load the web page
                    ShellExecuteW (hWnd,            // Parent window handle
                                   NULL,            // Operation (NULL = use default verb)
                                   wszTemp,         // The file to "open"
                                   NULL,            // Parameters
                                   NULL,            // Ptr to directory
                                   SW_SHOWNORMAL);  // Show command
                    return FALSE;       // We handled the msg
#undef  wszTemp

                case IDM_ABOUT:
                    DialogBoxW (_hInstance,
                                MAKEINTRESOURCEW (IDD_ABOUT),
                                hWnd,
                     (DLGPROC) &AboutDlgProc);
                    return FALSE;       // We handled the msg

                case IDM_UPDATES:
                    // See if we have the latest version
                    CheckForUpdates (TRUE,
                                     (GetKeyState (VK_CONTROL) & BIT15) ? TRUE : FALSE);
                    return FALSE;       // We handled the msg

                // The following messages come from the Tab Control.
                // They are sent here because we need a window whose
                //   WM_COMMAND we control so as not to interfere with
                //   any other window's meaning for these menu items.
                case IDM_NEW_WS:
                    // Load a CLEAR WS
                    if (!CreateNewTab (hWnd,
                                       L"",
                                       (gOverTabIndex EQ -1) ? 999 : gOverTabIndex + 1,
                                       FALSE))
                        return -1;          // Stop the whole process

                    return FALSE;   // We handled the msg

                case IDM_COPY_WS:   // )COPY  a workspace
                case IDM_LOAD_WS:   // )LOAD  ...
                case IDM_XLOAD_WS:  // )XLOAD ...
                case IDM_DROP_WS:   // )DROP  ...
                case IDM_SAVE_AS_WS:// )SAVE  ...         under a different name
                {
#define OPENFILE_LEN        _MAX_PATH
#define INITDIR_LEN         _MAX_PATH
                    static OPENFILENAMEW ofn;       // OpenFileName struc
                    static WCHAR lpwszOpenFile[OPENFILE_LEN] = {WC_EOS};
                    static WCHAR lpwszInitDir [INITDIR_LEN]  = {WC_EOS};

                    // If we're )COPYing, check to see if the current workspace is executing
                    if (idCtl EQ IDM_COPY_WS)
                    {
                        // Get ptr to PerTabData global memory
                        lpMemPTD = GetMemPTD ();

                        // If we're executing, ...
                        if (lpMemPTD->bExecuting)
                        {
                            MessageBoxW (hWnd,
                                         L"Unable to )COPY into an executing workspace.",
                                         lpwszAppName,
                                         MB_OK | MB_ICONWARNING | MB_APPLMODAL);
                            return FALSE;   // We handled the msg
                        } // End IF
                    } // End IF

                    // If the OpenFile is not initialized as yet, ...
                    if (lpwszOpenFile[0] EQ WC_EOS)
                    {
                        lpwszOpenFile[0] = WC_DQ;
                        lpwszOpenFile[1] = WC_EOS;
                    } // End IF

                    // If the initial directory is not set as yet, ...
                    if (lpwszInitDir[0] EQ WC_EOS)
                        strcpyW (lpwszInitDir, lpwszWorkDir);

                    // If there's a trailing DQ, zap it
                    *SkipToCharW (&lpwszOpenFile[1], WC_DQ) = WC_EOS;

                    // Fill in the struc
                    ZeroMemory (&ofn, sizeof (ofn));
                    ofn.lStructSize       = sizeof (ofn);
                    ofn.hwndOwner         = hWnd;
////////////////////ofn.hInstance         = _hInstance;     // Used only w/OFN_ENABLETEMPLATEHANDLE or OFN_EMABLETEMPLATE
                    ofn.lpstrFilter       = L"Workspaces (*.ws.nars)\0" L"*.ws.nars\0"
                                            L"All files (*.*)\0"        L"*.*\0";
////////////////////ofn.lpstrCustomFilter = NULL;           // Already zero from ZeroMemory
////////////////////ofn.nMaxCustFilter    =                 // Used only if lpstrCustomFilter NE NULL
                    ofn.nFilterIndex      = 1;
                    ofn.lpstrFile         = &lpwszOpenFile[1];
                    ofn.nMaxFile          = OPENFILE_LEN;
                    ofn.lpstrFileTitle    = NULL;
////////////////////ofn.nMaxFileTitle     =                 // Set only if lpstrFileTitle NE NULL
                    ofn.lpstrInitialDir   = lpwszInitDir;
                    ofn.lpstrTitle        = (idCtl EQ IDM_LOAD_WS    ) ? L"Open A Workspace"
                                          : (idCtl EQ IDM_XLOAD_WS   ) ? L"XOpen A Workspace"
                                          : (idCtl EQ IDM_COPY_WS    ) ? L"Copy A Workspace"
                                          : (idCtl EQ IDM_DROP_WS    ) ? L"Delete A Workspace"
                                          : (idCtl EQ IDM_SAVE_AS_WS ) ? L"Save As A Workspace"
                                          :                              L"";
                    ofn.Flags             = 0
                                          | OFN_ENABLEHOOK
                                          | OFN_ENABLESIZING
                                          | OFN_EXPLORER
                                          | OFN_HIDEREADONLY
                                          | ((idCtl EQ IDM_SAVE_AS_WS) ? 0 : OFN_FILEMUSTEXIST)
                                            ;
////////////////////ofn.nFileOffset       =                 // Set on output
////////////////////ofn.nFileExtension    =                 // Set on output
                    ofn.lpstrDefExt       = WS_WKSEXT + 1;  // Skipping over the leading dot
                    ofn.lCustData         = (LPARAM) idCtl;
                    ofn.lpfnHook          = OFNHookProc;
////////////////////ofn.lpTemplateName    =                 // Used only w/OFN_ENABLETEMPLATE
////////////////////ofn.pvReserved        = NULL;           // Already zero from ZeroMemory
////////////////////ofn.dwReserved        = 0;              // Already zero from ZeroMemory
////////////////////ofn.FlagsEx           = 0;              // Already zero From ZeroMemory

                    // Display an OPENFILENAME dialog
                    if (GetOpenFileNameW (&ofn))
                    {
                        // Append a trailing DQ to match the leading one
                        strcatW (lpwszOpenFile, WS_DQ);

                        // Split cases based upon the operation
                        switch (idCtl)
                        {
                            case IDM_COPY_WS:
                                // )COPY the workspace
                                CmdCopy_EM (lpwszOpenFile);

                                break;

                            case IDM_LOAD_WS:
                            case IDM_XLOAD_WS:
                                // )LOAD or )XLOAD the workspace
                                CmdLoadCom_EM (lpwszOpenFile, idCtl EQ IDM_LOAD_WS);

                                break;

                            case IDM_DROP_WS:
                                // )DROP the workspace
                                CmdDrop_EM (lpwszOpenFile);

                                break;

                            case IDM_SAVE_AS_WS:
                                // Save the workspace
                                CmdSave_EM (lpwszOpenFile);

                                break;

                            defstop
                                break;
                        } // End SWITCH

                        // Call common prompt display code
                        PostMessageW (hWnd, MYWM_PROMPT, 0, 0);
                    } // End IF

                    return FALSE;   // We handled the msg
                } // End IDM_COPY_WS/...

                case IDM_PCOPY_WS:
#ifdef DEBUG
                    DbgBrk ();      // ***FINISHME*** -- IDM_PCOPY_WS
#endif



                    return FALSE;   // We handled the msg

                case IDM_SAVE_WS:
                    // Get ptr to PerTabData global memory
                    lpMemPTD = GetPerTabPtr (TabCtrl_GetCurSel (hWndTC)); // Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));
                    if (!IsValidPtr (lpMemPTD, sizeof (lpMemPTD)))
                        return FALSE;
                    // Get this tab's SM window handle
                    hWndSM = lpMemPTD->hWndSM;

                    // Tell the SM to save the ws
                    SendMessageW (hWndSM, MYWM_SAVE_WS, 0, (LPARAM) L"");

                    // Call common prompt display code
                    PostMessageW (hWnd, MYWM_PROMPT, 0, 0);

                    return FALSE;   // We handled the msg

                case IDM_DUP_WS:
#ifdef DEBUG
                    DbgBrk ();      // ***FINISHME*** -- IDM_DUP_WS
#endif



                    return FALSE;   // We handled the msg

                case IDM_PRINT_WS:
                {
                    PRINTDLGEXW pdex;       // Struc used when printing
                    HWND        hWndEC;     // Edit Ctrl window handle
                    DWORD       dwSelBeg,   // Selection beginning
                                dwSelEnd;   // ...       end
                    HRESULT     hResult;    // Result from PrintDlgExW

                    // Determine whether or not the Edit Ctrl has a selection
                    hWndEC  = GetActiveEC (hWndTC);
                    SendMessageW (hWndEC, EM_GETSEL, (WPARAM) &dwSelBeg, (LPARAM) &dwSelEnd);

                    // Fill in the PRINTDLGEXW struc
                    ZeroMemory (&pdex, sizeof (pdex));
                    pdex.lStructSize         = sizeof (pdex);
                    pdex.hwndOwner           = hWndEC;
////////////////////pdex.hDevMode            = NULL;    // Output only
////////////////////pdex.hDevNames           = NULL;    // Output only
////////////////////pdex.hDC                 = NULL;    // Output only
                    pdex.Flags               = 0
                                           | ((dwSelBeg EQ dwSelEnd) ? PD_CURRENTPAGE | PD_NOSELECTION
                                                                     : PD_SELECTION)
                                           | PD_RETURNDC
                                           | PD_NOPAGENUMS
                                           ;
////////////////////pdex.Flags2              = 0;       // Unused
////////////////////pdex.Exclusion Flags     = 0;       // Unused
////////////////////pdex.nPageRanges         = 0;       // Not with PD_NOPAGENUMS
////////////////////pdex.nMaxPageRanges      = 0;       // Not with PD_NOPAGENUMS
////////////////////pdex.lpPageRanges        = NULL;    // Not with PD_NOPAGENUMS
////////////////////pdex.nMinPage            =          // Not with PD_NOPAGENUMS
////////////////////pdex.nMaxPage            =          // Not with PD_NOPAGENUMS
                    pdex.nCopies             = 1;       // Input/output
////////////////////pdex.hInstance           = NULL;    // Used for PD_ENABLEPRINTTEMPLATE only
////////////////////pdex.lpPrintTemplateName = NULL;    // Unused
////////////////////pdex.lpCallback          = NULL;    // Unused
////////////////////pdex.nPropertyPages      = 0;       // Used for lphPropertyPages only
////////////////////pdex.lphPropertyPages    = NULL;    // Unused
                    pdex.nStartPage          = START_PAGE_GENERAL;
////////////////////pdex.dwResultAction      = 0;       // Output only

                    // Ask the user what to do
                    hResult = PrintDlgExW (&pdex);

                    // Split cases based upon the result code
                    if (hResult EQ S_OK)
                    switch (pdex.dwResultAction)
                    {
                        case PD_RESULT_APPLY:
#ifdef DEBUG
                            DbgBrk ();          // ***FINISHME*** -- PD_RESULT_APPLY
#endif

                            break;

                        case PD_RESULT_PRINT:
                        {
#ifdef DEBUG
                            DEVMODE  *lpDevMode;
                            DEVNAMES *lpDevNames;
#endif
                            DOCINFOW docInfo = {0};

////////////////////////////// If the user said print-to-file, get the filename
////////////////////////////if (pdex.Flags & PD_PRINTTOFILE)
////////////////////////////{
////////////////////////////    DbgBrk ();          // ***FINISHME*** -- Print-to-file
////////////////////////////
////////////////////////////
////////////////////////////
////////////////////////////} // End IF
#ifdef DEBUG
                            lpDevMode  = MyGlobalLockWsz (pdex.hDevMode);
                            lpDevNames = MyGlobalLockWsz (pdex.hDevNames);
#endif
                            // If the Edit Ctrl is from SM, ...
                            if (IzitSM (GetParent (hWndEC)))
                                // Tell 'em about the Fallback font
                                SendMessageW (hWndEC,
                                              EM_SETFALLBACKFONT,
                                              (WPARAM) (OptionFlags.bOutputDebug ? hFontFB_PR_SM : NULL),
                                              (LPARAM) (OptionFlags.bOutputDebug ? hFontFB_SM    : NULL));
                            else
                            // If the Edit Ctrl is from FE, ...
                            if (IzitFE (GetParent (hWndEC)))
                                // Tell 'em about the Fallback font
                                SendMessageW (hWndEC,
                                              EM_SETFALLBACKFONT,
                                              (WPARAM) (OptionFlags.bOutputDebug ? hFontFB_PR_FE : NULL),
                                              (LPARAM) (OptionFlags.bOutputDebug ? hFontFB_FE    : NULL));
#ifdef DEBUG
                            else
                                DbgBrk ();
#endif
                            // Setup the DOCINFO struc for the print job
                            docInfo.cbSize       = sizeof (docInfo);
                            docInfo.lpszDocName  = WS_APPNAME;
                            docInfo.lpszOutput   = NULL;        // ***FIXME*** -- Insert filename
////////////////////////////docInfo.lpszDatatype = NULL;
////////////////////////////docInfo.fwType       = 0;

                            // Start the print job
                            StartDocW (pdex.hDC, &docInfo);

                            // Tell the Edit Ctrl to print the client area
                            SendMessageW (hWndEC,
                                          WM_PRINTCLIENT,
                                          (WPARAM) pdex.hDC,
                                          PRF_PRINTCLIENT
                                        | ((pdex.Flags & PD_SELECTION) ? PRF_SELECTION
                                                                       : (pdex.Flags & PD_CURRENTPAGE) ? PRF_CURRENTPAGE
                                                                                                       : 0));
                            // End the print job
                            EndDoc (pdex.hDC);
#ifdef DEBUG
                            MyGlobalUnlock (pdex.hDevNames); lpDevNames = NULL;
                            MyGlobalUnlock (pdex.hDevMode);  lpDevMode  = NULL;
#endif
                            break;
                        } // End PD_RESULT_PRINT

                        case PD_RESULT_CANCEL:
                        case E_OUTOFMEMORY:
                        case E_INVALIDARG:
                        case E_POINTER:
                        case E_HANDLE:
                        case E_FAIL:
                            break;
                    } // End SWITCH

                    // Free allocated resources
                    if (pdex.hDC NE NULL)
                        DeleteDC (pdex.hDC);
                    if (pdex.hDevMode NE NULL)
                        GlobalFree (pdex.hDevMode);
                    if (pdex.hDevNames NE NULL)
                        GlobalFree (pdex.hDevNames);

                    return FALSE;   // We handled the msg
                } // End IDM_PRINT_WS

                // The following messages are signalled from right-click on a Tab
                case IDM_SAVECLOSE_WS:
                    if (SendMessageW (hWnd, WM_COMMAND, GET_WM_COMMAND_MPS (IDM_SAVE_WS, NULL, 0)))
                        // Close the tab
                        CloseTab (gOverTabIndex);

                    return FALSE;   // We handled the msg

                case IDM_CLOSE_WS:
                    // Close the tab
                    CloseTab (gOverTabIndex);

                    return FALSE;   // We handled the msg

                case IDM_NEW_FCN:
                    // Get ptr to PerTabData global memory
                    lpMemPTD = GetPerTabPtr (TabCtrl_GetCurSel (hWndTC)); // Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));
                    if (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)))
                        // Create a new function window
                        PostMessageW (lpMemPTD->hWndSM, MYWM_CREATEFCN, 0, 0);
                    return FALSE;   // We handled the msg

                case IDM_CLOSE_FCN:
                    // Tell the active window to handle it
                    PostMessageW (hWndActive, MYWM_CLOSE_FN, wParam, lParam);

                    return FALSE;   // We handled the msg

                // The following messages are signalled from the Objects Menu
                //   when a function window is open
                case IDM_SAVE_FCN:
                    // Tell the active window to handle it
                    PostMessageW (hWndActive, MYWM_SAVE_FN, wParam, lParam);

                    return FALSE;   // We handled the msg

                case IDM_SAVECLOSE_FCN:
                    // Tell the active window to handle it
                    PostMessageW (hWndActive, MYWM_SAVECLOSE_FN, wParam, lParam);

                    return FALSE;   // We handled the msg

////////////////case IDM_SAVE_FN_AS:            // ***FIXME*** -- Make this work??
////////////////    SaveAsFunction (hWndActive);
////////////////
////////////////    return FALSE;   // We handled the msg

                default:
                {
                    WCHAR (*lpwszRecentFiles)[][_MAX_PATH]; // Ptr to list of recent files

                    // Lock the memory to get a ptr to it
                    lpwszRecentFiles = MyGlobalLock (hGlbRecentFiles);

                    // Check for IDM_LOAD_BASE
                    if (IDM_LOAD_BASE  <= idCtl
                     &&                   idCtl < (IDM_LOAD_BASE + uNumRecentFiles))
                    {
                        // )LOAD the corresponding workspace
                        CmdLoadCom_EM ((*lpwszRecentFiles)[idCtl - IDM_LOAD_BASE], TRUE);
                    } else
                    // Check for IDM_XLOAD_BASE
                    if (IDM_XLOAD_BASE <= idCtl
                     &&                   idCtl < (IDM_XLOAD_BASE + uNumRecentFiles))
                    {
                        // )XLOAD the corresponding workspace
                        CmdLoadCom_EM ((*lpwszRecentFiles)[idCtl - IDM_XLOAD_BASE], FALSE);
                    } else
                    // Check for IDM_COPY_BASE
                    if (IDM_COPY_BASE  <= idCtl
                     &&                   idCtl < (IDM_COPY_BASE + uNumRecentFiles))
                    {
                        // )COPY the corresponding workspace
                        CmdCopy_EM    ((*lpwszRecentFiles)[idCtl - IDM_COPY_BASE]);
                    } // End IF/ELSE/...

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbRecentFiles); lpwszRecentFiles = NULL;
                } // End default
            } // End SWITCH

            break;                  // Continue with next handler ***MUST***
#ifndef DEBUG
  #undef  hwndCtl
  #undef  cmdCtl
  #undef  idCtl
#endif
        } // End WM_COMMAND

        case MYWM_PROMPT:
            // Get ptr to PerTabData global memory
            lpMemPTD = GetPerTabPtr (TabCtrl_GetCurSel (hWndTC)); // Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));
            if (!IsValidPtr (lpMemPTD, sizeof (lpMemPTD)))
                return FALSE;

            // Get this tab's SM window handle
            hWndSM = lpMemPTD->hWndSM;

            // If it's Quad input, and we're not resetting, ...
            if (lpMemPTD->lpSISCur NE NULL
             && lpMemPTD->lpSISCur->ResetFlag EQ RESETFLAG_NONE
             && lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QUAD)
                // Tell the SM to display the Quad Input Prompt
                PostMessageW (hWndSM, MYWM_QUOTEQUAD, FALSE, 100);
            else
            // If no SIS layer or not Quad input and not reset all, ...
            if (lpMemPTD->lpSISCur EQ NULL
             || (lpMemPTD->lpSISCur->DfnType NE DFNTYPE_QUAD
              && lpMemPTD->lpSISCur->ResetFlag NE RESETFLAG_ALL))
            {
                HWND hWndEC;

                // Get the handle to the Edit Ctrl
                hWndEC = (HWND) GetWindowLongPtrW (hWndSM, GWLSF_HWNDEC);

                // Display the default prompt
                DisplayPrompt (hWndEC, 9);
            } // End IF/ELSE/...

            return FALSE;           // We handled the msg

        case WM_ERASEBKGND:
            // In order to reduce screen flicker, we handle erase background
            // in the WM_PAINT message for the child windows.
            return TRUE;            // We erased the background

        case WM_QUERYENDSESSION:
        case WM_CLOSE:
            if (fHelp && wszHlpDPFE[0])  // If we used help and there's a valid .HLP file, ...
            {
                WinHelpW (hWnd, wszHlpDPFE, HELP_QUIT, 0); // Quit it
                fHelp = FALSE;
            } // End IF

            // If the Customize dialog box is still active, ...
            if (ghDlgCustomize NE NULL)
            {
                // Ask 'em to close
                SendMessageW (ghDlgCustomize, WM_CLOSE, 0, 0);

                // If the Customize dialog box is still active, ...
                if (ghDlgCustomize NE NULL)
                    return FALSE;
            } // End IF

            // If the Updates dialog box is still active, ...
            if (ghDlgUpdates NE NULL)
            {
                // Ask 'em to close
                SendMessageW (ghDlgUpdates, WM_CLOSE, 0, 0);

                // If the Updates dialog box is still active, ...
                if (ghDlgUpdates NE NULL)
                    return FALSE;
            } // End IF

            // Save .ini file variables
            SaveIniFile ();

            // Ask the child windows if it's OK to close
            if (EnumChildWindows (hWnd, EnumCallbackQueryClose, 0))
            {
#ifdef DEBUG
////            // Tell the debugger windows to unhook its hooks
////            EnumChildWindows (hWnd, EnumCallbackUnhookDebugger, 0);
#endif
                // Delete all the tabs
                TabCtrl_DeleteAllItems (hWndTC);

                // This also tells the child windows to close
                DestroyWindow (hWnd);

                if (message EQ WM_QUERYENDSESSION)
                    return TRUE;        // OK to terminate
                else
                    return FALSE;       // We handled the msg
            } else
                return FALSE;           // Not OK to terminate/we handled the msg

        case WM_DESTROY:
            // Mark as in the process of being destroyed
            bMainDestroy = TRUE;

            // If the Customize dialog box is still active, ...
            if (ghDlgCustomize NE NULL)
                // Destroy it
                DestroyWindow (ghDlgCustomize);
            // If the Updates dialog box is still active, ...
            if (ghDlgUpdates NE NULL)
                // Destroy it
                DestroyWindow (ghDlgUpdates);
            // If the Keyboard Layout global memory is present, ...
            if (hGlbKeybLayouts NE NULL)
            {
                DbgGlobalFree (hGlbKeybLayouts); hGlbKeybLayouts = NULL;
            } // End IF

            // Uninitialize window-specific resources
            MF_Delete (hWnd);

            // Destroy the fonts
            if (hFontTC NE NULL)
            {
                MyDeleteObject (hFontTC); hFontTC = NULL;
            } // End IF

            if (hFontLW NE NULL)
            {
                MyDeleteObject (hFontLW); hFontLW = NULL;
            } // End IF

            if (hFontSM NE NULL)
            {
                MyDeleteObject (hFontSM); hFontSM = NULL;
            } // End IF

            if (hFontFB_SM NE NULL)
            {
                MyDeleteObject (hFontFB_SM); hFontFB_SM = NULL;
            } // End IF

            if (hFontFB_FE NE NULL)
            {
                MyDeleteObject (hFontFB_FE); hFontFB_FE = NULL;
            } // End IF

            if (hFontFB_PR_SM NE NULL)
            {
                MyDeleteObject (hFontFB_PR_SM); hFontFB_PR_SM = NULL;
            } // End IF

            if (hFontFB_PR_FE NE NULL)
            {
                MyDeleteObject (hFontFB_PR_FE); hFontFB_PR_FE = NULL;
            } // End IF

            if (hFontCC NE NULL)
            {
                MyDeleteObject (hFontCC); hFontCC = NULL;
            } // End IF

            if (hFontFE NE NULL)
            {
                MyDeleteObject (hFontFE); hFontFE = NULL;
            } // End IF

            // Destroy the image lists
            if (hImageListOW NE NULL)
            {
                ImageList_Destroy (hImageListOW); hImageListOW = NULL;
            } // End IF

            if (hImageListED NE NULL)
            {
                ImageList_Destroy (hImageListED); hImageListED = NULL;
            } // End IF

            if (hImageListWS NE NULL)
            {
                ImageList_Destroy (hImageListWS); hImageListWS = NULL;
            } // End IF

            if (hImageListTC NE NULL)
            {
                ImageList_Destroy (hImageListTC); hImageListTC = NULL;
            } // End IF

            // *************** Bitmaps *********************************
            if (hBitmapCheck NE NULL)
            {
                MyDeleteObject (hBitmapCheck); hBitmapCheck = NULL;
            } // End IF

            DeleteImageBitmaps ();

            break;                  // Continue with default handler

        default:
            break;                  // Continue with default handler
    } // End SWITCH

    // Get the window handle of the currently active MDI Client
    if (message NE WM_NCDESTROY
     && message NE WM_CLOSE)
        hWndMC = GetActiveMC (hWndTC);
    else
        hWndMC = NULL;
////LCLODSAPI ("MFZ:", hWnd, message, wParam, lParam);
    return DefFrameProcW (hWnd, hWndMC, message, wParam, lParam);
} // End MFWndProc
#undef  APPEND_NAME


//***************************************************************************
//  $TabCtrl_SelChange
//
//  Handle Tab Ctrl notification TCN_SELCHANGE
//***************************************************************************

void TabCtrl_SelChange
    (void)

{
    int iCurTabIndex,       // Index of the current tab
        iLstTabIndex;       // Index of the previous tab

    // Get the index of the incoming tab
    iCurTabIndex = TabCtrl_GetCurSel (hWndTC);

    // Get the index of the previous tab
    iLstTabIndex = TranslateTabIDToIndex (gLstTabID);

    // Save the index of the incoming tab
    gCurTabID = TranslateTabIndexToID (iCurTabIndex);

    // Hide the child windows of the outgoing tab
    if (iLstTabIndex NE -1 && iLstTabIndex NE iCurTabIndex)
        ShowHideChildWindows (GetWndMC (iLstTabIndex), FALSE);
    // Show the child windows of the incoming tab
    if (iCurTabIndex NE -1)
        ShowHideChildWindows (GetWndMC (iCurTabIndex), TRUE);
} // End TabCtrl_SelChange


//***************************************************************************
//  $OFNHookProc
//
//  Hook procedure for GetOpenFileName
//***************************************************************************

UINT_PTR CALLBACK OFNHookProc
    (HWND   hDlg,       // Window handle
     UINT   message,    // Type of message
     WPARAM wParam,     // Additional information
     LPARAM lParam)     // ...

{
    // Split cases based upon the message
    switch (message)
    {
        static UINT     idCtl;
        LPOPENFILENAMEW lpofn;

        case WM_INITDIALOG:         // lpofn = (LPOPENFILENAMEW) lParam;
            // Address the arguments
            lpofn = (LPOPENFILENAMEW) lParam;

            // Save the idCtl in a static var
            idCtl = (UINT) lpofn->lCustData;

            break;

        case WM_NOTIFY:             // idCtrl = (int) wParam;
                                    // pnmh = (LPNMHDR) lParam;
            // Split cases based upon the notification code
#define lpnmh   (*(LPNMHDR *) &lParam)
            switch (lpnmh->code)
#undef  lpnmh
            {
                LPWCHAR lpwszBtnTitle;

                case CDN_INITDONE:
                    // Split cases based upon the message ID
                    switch (idCtl)
                    {
                        case IDM_LOAD_WS:
                            lpwszBtnTitle = L"Open";

                            break;

                        case IDM_XLOAD_WS:
                            lpwszBtnTitle = L"XOpen";

                            break;

                        case IDM_COPY_WS:
                            lpwszBtnTitle = L"Copy";

                            break;

                        case IDM_DROP_WS:
                            lpwszBtnTitle = L"Delete";

                            break;

                        case IDM_SAVE_AS_WS:
                            lpwszBtnTitle = L"Save As";

                            break;

                        default:
                            lpwszBtnTitle = L"Open";

                            break;
                    } // End SWITCH

                    // Set the text for the Open button
                    SendMessageW (GetParent (hDlg), CDM_SETCONTROLTEXT, IDOK, (LPARAM) lpwszBtnTitle);

                    break;

                default:
                    break;
            } // End SWITCH

            break;

        default:
            break;
    } // End SWITCH

    return FALSE;               // We didn't handle the message
} // End OFNHookProc


//***************************************************************************
//  $GetActiveEC
//
//  Get the Edit Ctrl window handle of the currently active MDI Client
//***************************************************************************

HWND GetActiveEC
    (HWND hWndTC)           // Window handle of Tab Control

{
    HWND hWndMC,
         hWndAct;

    // Get the active MDI Client window handle
    hWndMC  = GetActiveMC (hWndTC);
    if (hWndMC NE NULL)
    {
        // Get the active MDI Child window handle
        hWndAct = (HWND) SendMessageW (hWndMC, WM_MDIGETACTIVE, 0, 0);

        // Return the EditCtrl window handle
        return (HWND) GetWindowLongPtrW (hWndAct, GWLSF_HWNDEC);
    } else
        return NULL;
} // End GetActiveEC


//***************************************************************************
//  $GetActiveMC
//
//  Get the window handle of the currently active MDI Client
//***************************************************************************

HWND GetActiveMC
    (HWND hWndTC)           // Window handle of Tab Control

{
    int iCurTabIndex;       // Index of the current tab

    // If the Tab Control is not defined, quit
    if (hWndTC EQ NULL)
        return NULL;

    // Get the index of the currently selected tab
    iCurTabIndex = TabCtrl_GetCurSel (hWndTC);

    // If no tab selected (early in MFWndProc processing), quit
    if (iCurTabIndex EQ -1)
        return NULL;

    return GetWndMC (iCurTabIndex);
} // End GetActiveMC


//***************************************************************************
//  $GetWndMC
//
//  Get the window handle of the MDI Client of a given tab
//***************************************************************************

HWND GetWndMC
    (int iCurTab)                       // Index of the tab of interest (-1 = none)

{
    LPPERTABDATA lpMemPTD;              // Ptr to PertabData global memory

    // If the Tab Control is not defined, quit
    if (hWndTC EQ NULL)
        return NULL;

    // If no tab selected (early in MFWndProc processing), quit
    if (iCurTab EQ -1)
        return NULL;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetPerTabPtr (iCurTab); // Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Ensure it's a valid ptr
    if (!IsValidPtr (lpMemPTD, sizeof (lpMemPTD)))
        return NULL;

    // Return window handle of MDI Client
    return lpMemPTD->hWndMC;
} // End GetWndMC


//***************************************************************************
//  $EnumCallbackQueryClose
//
//  EnumChildWindows callback to query whether or not they can close
//***************************************************************************

UBOOL CALLBACK EnumCallbackQueryClose
    (HWND   hWnd,           // Handle to child window
     LPARAM lParam)         // Application-defined value

{
    // When an MDI child window is minimized, Windows creates two windows: an
    // icon and the icon title.  The parent of the icon title window is set to
    // the MDI client window, which confines the icon title to the MDI client
    // area.  The owner of the icon title is set to the MDI child window.
    if (GetWindow (hWnd, GW_OWNER) NE NULL)     // If it's an icon title window, ...
        return TRUE;                    // skip it, and continue enumerating

    return (UINT) SendMessageW (hWnd, WM_QUERYENDSESSION, 0, 0);
} // End EnumCallbackQueryClose


#ifdef DEBUG
//***************************************************************************
//  $IzitDB
//
//  Is the window DBWNDCLASS?
//***************************************************************************

UBOOL IzitDB
    (HWND hWnd)

{
    WCHAR wszClassName[32];

    GetClassNameW (hWnd, wszClassName, countof (wszClassName));

    return (lstrcmpW (wszClassName, LDBWNDCLASS) EQ 0);
} // End IzitDB
#endif


//***************************************************************************
//  $InitApplication
//
//  Initializes window data and registers window class
//***************************************************************************

UBOOL InitApplication
    (HANDLE hInstance)      // Current instance

{
    WNDCLASSEXW wcw = {sizeof (WNDCLASSEXW)};

    // Fill in Master Frame window class structure
    wcw.style           = CS_DBLCLKS;
    wcw.lpfnWndProc     = (WNDPROC) MFWndProc;
    wcw.cbClsExtra      = 0;
    wcw.cbWndExtra      = 0;
    wcw.hInstance       = hInstance;
    wcw.hIcon           = hIconMF_Large;
    wcw.hIconSm         = hIconMF_Small;
    wcw.hCursor         = LoadCursor (NULL, MAKEINTRESOURCE (IDC_ARROW));
    wcw.hbrBackground   = (HBRUSH) (COLOR_WINDOW + 1);
    wcw.lpszMenuName    = MAKEINTRESOURCEW (IDR_SMMENU);
    wcw.lpszClassName   = LMFWNDCLASS;

    // Register the Master Frame window class
    if (!RegisterClassExW (&wcw))
    {
        MB (pszNoRegMFWndClass);
        return FALSE;
    } // End IF

    // Fill in Session Manager window class structure
    wcw.style           = CS_DBLCLKS | CS_NOCLOSE;
    wcw.lpfnWndProc     = (WNDPROC) SMWndProc;
    wcw.cbClsExtra      = 0;
    wcw.cbWndExtra      = GWLSM_EXTRA;
    wcw.hInstance       = hInstance;
    wcw.hIcon           = hIconSM_Large;
    wcw.hIconSm         = hIconSM_Small;
    wcw.hCursor         = LoadCursor (NULL, MAKEINTRESOURCE (IDC_ARROW));
    wcw.hbrBackground   = GetStockObject (WHITE_BRUSH);
    wcw.lpszMenuName    = MAKEINTRESOURCEW (IDR_SMMENU);
    wcw.lpszClassName   = LSMWNDCLASS;

    // Register the Session Manager window class
    if (!RegisterClassExW (&wcw))
    {
        MB (pszNoRegSMWndClass);
        return FALSE;
    } // End IF

    // Fill in Function Editor window class structure
    wcw.style           = CS_DBLCLKS;
    wcw.lpfnWndProc     = (WNDPROC) FEWndProc;
    wcw.cbClsExtra      = 0;
    wcw.cbWndExtra      = GWLFE_EXTRA;
    wcw.hInstance       = hInstance;
    wcw.hIcon           = hIconFE_Large;
    wcw.hIconSm         = hIconFE_Small;
    wcw.hCursor         = LoadCursor (NULL, MAKEINTRESOURCE (IDC_ARROW));
    wcw.hbrBackground   = GetStockObject (WHITE_BRUSH);
    wcw.lpszMenuName    = MAKEINTRESOURCEW (IDR_FEMENU);
    wcw.lpszClassName   = LFEWNDCLASS;

    // Register the Function Editor window class
    if (!RegisterClassExW (&wcw))
    {
        MB (pszNoRegFEWndClass);
        return FALSE;
    } // End IF
#ifdef DEBUG
    // Fill in Debugger window class structure
    wcw.style           = CS_DBLCLKS;
    wcw.lpfnWndProc     = (WNDPROC) DBWndProc;
    wcw.cbClsExtra      = 0;
    wcw.cbWndExtra      = GWLDB_EXTRA;
    wcw.hInstance       = hInstance;
    wcw.hIcon           = hIconDB_Large;
    wcw.hIconSm         = hIconDB_Small;
    wcw.hCursor         = LoadCursor (NULL, MAKEINTRESOURCE (IDC_ARROW));
    wcw.hbrBackground   = GetStockObject (WHITE_BRUSH);
    wcw.lpszMenuName    = MAKEINTRESOURCEW (IDR_SMMENU);
    wcw.lpszClassName   = LDBWNDCLASS;

    // Register the Debugger window class
    if (!RegisterClassExW (&wcw))
    {
        MB (pszNoRegDBWndClass);
        return FALSE;
    } // End IF
#endif

    // Fill in Crash Control window class structure
    wcw.style           = CS_DBLCLKS;
    wcw.lpfnWndProc     = (WNDPROC) CCWndProc;
    wcw.cbClsExtra      = 0;
    wcw.cbWndExtra      = GWLCC_EXTRA;
    wcw.hInstance       = hInstance;
    wcw.hIcon           = hIconCC_Large;
    wcw.hIconSm         = hIconCC_Small;
    wcw.hCursor         = LoadCursor (NULL, MAKEINTRESOURCE (IDC_ARROW));
    wcw.hbrBackground   = GetStockObject (WHITE_BRUSH);
////wcw.lpszMenuName    = MAKEINTRESOURCEW (IDR_CCMENU);
    wcw.lpszMenuName    = NULL;
    wcw.lpszClassName   = LCCWNDCLASS;

    // Register the Crash Control window class
    if (!RegisterClassExW (&wcw))
    {
        MB (pszNoRegCCWndClass);
        return FALSE;
    } // End IF

    // Fill in Edit Ctrl window class structure
    wcw.style           = CS_DBLCLKS;
    wcw.lpfnWndProc     = (WNDPROC) LclEditCtrlWndProc;
    wcw.cbClsExtra      = 0;
    wcw.cbWndExtra      = GWLEC_EXTRA;
    wcw.hInstance       = hInstance;
    wcw.hIcon           = NULL;
    wcw.hIconSm         = NULL;
    wcw.hCursor         = LoadCursor (NULL, MAKEINTRESOURCE (IDC_IBEAM));
    wcw.hbrBackground   = (HBRUSH) (COLOR_WINDOW + 1);
    wcw.lpszMenuName    = NULL;
    wcw.lpszClassName   = LECWNDCLASS;

    // Register the Edit Ctrl window class
    if (!RegisterClassExW (&wcw))
    {
        MB (pszNoRegECWndClass);
        return FALSE;
    } // End IF

#ifdef PERFMONON
    // Fill in Performance Monitoring window class structure
    wcw.style           = CS_DBLCLKS;
    wcw.lpfnWndProc     = (WNDPROC) PMWndProc;
    wcw.cbClsExtra      = 0;
    wcw.cbWndExtra      = GWLPM_EXTRA;
    wcw.hInstance       = _hInstance;
    wcw.hIcon           = hIconPM_Large;
    wcw.hIconSm         = hIconPM_Small;
    wcw.hCursor         = LoadCursor (NULL, MAKEINTRESOURCE (IDC_ARROW));
    wcw.hbrBackground   = GetStockObject (WHITE_BRUSH);
////wcw.lpszMenuName    = MAKEINTRESOURCE (IDR_PMMENU);
    wcw.lpszMenuName    = NULL;
    wcw.lpszClassName   = LPMWNDCLASS;

    // Register the Performance Monitoring window class
    if (!RegisterClassExW (&wcw))
    {
        MB (pszNoRegPMWndClass);
        return FALSE;
    } // End IF
#endif

    // Fill in Font Window in Rebar Ctrl class structure
    wcw.style           = CS_DBLCLKS;
    wcw.lpfnWndProc     = (WNDPROC) FW_RBWndProc;
    wcw.cbClsExtra      = 0;
    wcw.cbWndExtra      = GWLFW_RB_EXTRA;
    wcw.hInstance       = _hInstance;
    wcw.hIcon           =
    wcw.hIconSm         = NULL;
    wcw.hCursor         = LoadCursor (NULL, MAKEINTRESOURCE (IDC_ARROW));
    wcw.hbrBackground   = (HBRUSH) (COLOR_BTNFACE + 1);
////wcw.lpszMenuName    = MAKEINTRESOURCE (IDR_PMMENU);
    wcw.lpszMenuName    = NULL;
    wcw.lpszClassName   = LFW_RBWNDCLASS;

    // Register the Font Window window class
    if (!RegisterClassExW (&wcw))
    {
        MB (pszNoRegFW_RBWndClass);
        return FALSE;
    } // End IF

    // Fill in Language Window in Rebar Ctrl class structure
    wcw.style           = CS_DBLCLKS;
    wcw.lpfnWndProc     = (WNDPROC) LW_RBWndProc;
    wcw.cbClsExtra      = 0;
    wcw.cbWndExtra      = GWLLW_RB_EXTRA;
    wcw.hInstance       = _hInstance;
    wcw.hIcon           =
    wcw.hIconSm         = NULL;
    wcw.hCursor         = LoadCursor (NULL, MAKEINTRESOURCE (IDC_ARROW));
    wcw.hbrBackground   = (HBRUSH) (COLOR_BTNFACE + 1);
////wcw.lpszMenuName    = MAKEINTRESOURCE (IDR_PMMENU);
    wcw.lpszMenuName    = NULL;
    wcw.lpszClassName   = LLW_RBWNDCLASS;

    // Register the Language Window window class
    if (!RegisterClassExW (&wcw))
    {
        MB (pszNoRegLW_RBWndClass);
        return FALSE;
    } // End IF

    // Fill in Progress Bar window class structure
    wcw.style           = CS_DBLCLKS;
    wcw.lpfnWndProc     = (WNDPROC) PBWndProc;
    wcw.cbClsExtra      = 0;
    wcw.cbWndExtra      = GWLPB_EXTRA;
    wcw.hInstance       = _hInstance;
    wcw.hIcon           =
    wcw.hIconSm         = NULL;
    wcw.hCursor         = LoadCursor (NULL, MAKEINTRESOURCE (IDC_ARROW));
    wcw.hbrBackground   = (HBRUSH) (COLOR_BTNFACE + 1);
////wcw.lpszMenuName    =
    wcw.lpszMenuName    = NULL;
    wcw.lpszClassName   = LPBWNDCLASS;

    // Register the Progress Bar Window window class
    if (!RegisterClassExW (&wcw))
    {
        MB (pszNoRegPBWndClass);
        return FALSE;
    } // End IF

    // Get the base class info prior to superclassing
    GetClassInfoExW (NULL,              // hInstance (NULL for system classes)
                     WC_STATUSBARW,     // Class name
                    &wcw);              // Ptr to WNDCLASSSEX struc
    // Save the address of the window procedure
    lpfnOldStatusBarWndProc = wcw.lpfnWndProc;

    // Fill in StatusBar Ctrl window class structure
////wcw.style           = 0;
    wcw.lpfnWndProc     = (WNDPROC) LclStatusBarWndProc;
////wcw.cbClsExtra      = 0;
////wcw.cbWndExtra      = 0;
    wcw.hInstance       = hInstance;
////wcw.hIcon           = NULL;
////wcw.hIconSm         = NULL;
////wcw.hCursor         = LoadCursor (NULL, MAKEINTRESOURCE (IDC_ARROW));
////wcw.hbrBackground   = (HBRUSH) (COLOR_BTNFACE + 1);
////wcw.lpszMenuName    = NULL;
    wcw.lpszClassName   = LCL_WC_STATUSBARW;

    // Register the StatusBar Ctrl window class
    if (!RegisterClassExW (&wcw))
    {
        MB (pszNoRegSBWndClass);
        return FALSE;
    } // End IF

    return TRUE;
} // End InitApplication


//***************************************************************************
//  $UninitApplication
//
//  Uninitialize application-specific resources
//***************************************************************************

void UninitApplication
    (HINSTANCE hInstance)

{
} // UninitApplication


//***************************************************************************
//  $InitInstance
//
//  Saves instance handle and creates main window
//***************************************************************************

UBOOL InitInstance
    (HANDLE hInstance)

{
    HDC hDC;                    // Temporary Device Context
    int i;                      // Loop counter

    // Save in global variable
    _hInstance = hInstance;

    // *************** Fonts ***********************************
    // Enumerate fonts to ensure our default font is installed
    hDC = MyGetDC (HWND_DESKTOP);
    SetAttrs (hDC, NULL, DEF_SCN_BLACK, DEF_SCN_WHITE);
    EnumFontsW (hDC,
                DEF_APLFONT_INTNAME,
                (FONTENUMPROCW) EnumCallbackFindAplFont,
       (LPARAM) DEF_APLFONT_INTNAME);
    // Release the DC
    MyReleaseDC (HWND_DESKTOP, hDC); hDC = NULL;

    // If we didn't find the font, ...
    if (!bAPLFont)
    {
        HKEY hKeyFont;

        // Ask the user if s/he wants to copy it to the Fonts folder
        if (MessageBoxW (hWndMF,
                         L"The default APL font <" DEF_APLFONT_FILE L"> is not installed and will be installed now.  Copy it to the Fonts folder, too?",
                         lpwszAppName,
                         MB_YESNO | MB_ICONQUESTION) EQ IDYES)
        {
            WCHAR wszTemp[_MAX_PATH];

            // Get the Windows directory (parent of the Fonts folder)
            GetWindowsDirectoryW (wszTemp, countof (wszTemp));
            MyStrcatW (wszTemp, sizeof (wszTemp), WS_SLOPE L"Fonts" WS_SLOPE DEF_APLFONT_FILE);

            // Copy the file to the Fonts folder without Overwrite
            if (!CopyFileW (wszFntDPFE, wszTemp, TRUE))
            {
                DWORD dwError = GetLastError ();
                WCHAR wszError[1024];

                // Split cases based upon the error code
                switch (dwError)
                {
                    case ERROR_FILE_EXISTS:
                        // Ask if we should overwrite it
                        if (MessageBoxW (hWndMF,
                                         L"The default APL font <" DEF_APLFONT_FILE L"> is already in the Fonts folder:  Overwrite it?",
                                         lpwszAppName,
                                         MB_YESNO | MB_ICONQUESTION) EQ IDYES)
                        {
                            // Copy the file to the Fonts folder with Overwrite
                            CopyFileW (wszFntDPFE, wszTemp, FALSE);

                            // Respecify the DPFE of the file to install
                            strcpyW  (wszFntDPFE, wszTemp);
                        } // End IF

                        break;

                    case ERROR_FILE_NOT_FOUND:
                        // Tell 'em to copy the font to the program folder
                        MessageBoxW (hWndMF,
                                     L"The default APL font <" DEF_APLFONT_FILE L"> is not in the same folder as " WS_APPNAME  L".exe:  Please copy it there and retry.",
                                     lpwszAppName,
                                     MB_OK | MB_ICONERROR);
                        return FALSE;

                    default:
                        // Find out why it failed
                        FormatMessageW (FORMAT_MESSAGE_FROM_SYSTEM, // Source and processing options
                                        NULL,                       // Pointer to  message source
                                        dwError,                    // Requested message identifier
                                        0,                          // Language identifier for requested message
                                        wszError,                   // Pointer to message buffer
                                        countof (wszError),         // Maximum size of message buffer
                                        NULL);                      // Address of array of message inserts
                        // Tell 'em why it failed
                        MessageBoxW (hWndMF,
                                     wszError,
                                     lpwszAppName,
                                     MB_OK | MB_ICONERROR);
                        return FALSE;
                } // End SWITCH
            } else
                // Respecify the DPFE of the file to install
                strcpyW  (wszFntDPFE, wszTemp);
        } // End IF

        // Install the font
        if (AddFontResourceW (wszFntDPFE) EQ 0)
        {
            MessageBoxW (hWndMF,
                         L"Unable to add the default APL font resource <" DEF_APLFONT_FILE L">.  Please copy it to the same folder as " WS_APPNAME L".exe and retry.",
                         lpwszAppName,
                         MB_OK | MB_ICONERROR);
            return FALSE;
        } // End IF

        // Make the font persistent across reboots
        if (RegCreateKeyW (HKEY_LOCAL_MACHINE,
                           L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts",
                          &hKeyFont)
            NE ERROR_SUCCESS)
        {
            MessageBoxW (hWndMF,
                         L"Unable to create the Registry key for the default APL font.",
                         lpwszAppName,
                         MB_OK | MB_ICONERROR);
            return FALSE;
        } // End IF

        // Set the key value
        if (RegSetValueExW (hKeyFont,                           // Handle of an open key
                            DEF_APLFONT_INTNAME L" (TrueType)", // Name of value to set
                            0,                                  // Reserved
                            REG_SZ,                             // Type of value
                   (LPBYTE) (DEF_APLFONT_FILE),                 // Ptr to value
                            sizeof (DEF_APLFONT_FILE))          // Byte size of value (including terminating zero)
            NE ERROR_SUCCESS)
        {
            MessageBoxW (hWndMF,
                         L"Unable to set the Registry key value for the default APL font.",
                         lpwszAppName,
                         MB_OK | MB_ICONERROR);
            return FALSE;
        } // End IF

        // We no longer need this resource
        RegCloseKey (hKeyFont); hKeyFont = NULL;

        // Tell other apps about it
        SendMessageW (HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
    } // End IF

    //***************************************************************
    // Allocate virtual memory for the WCHAR temporary storage
    //***************************************************************
    memVirtStr[MEMVIRT_WSZGLBTEMP].lpText   = "lpwszGlbTemp in <InitInstance>";
    memVirtStr[MEMVIRT_WSZGLBTEMP].IncrSize = DEF_WGLBTEMP_INCRNELM * sizeof (WCHAR);
    memVirtStr[MEMVIRT_WSZGLBTEMP].MaxSize  = DEF_WGLBTEMP_MAXNELM  * sizeof (WCHAR);
    memVirtStr[MEMVIRT_WSZGLBTEMP].IniAddr  = (LPUCHAR)
    lpwszGlbTemp =
      GuardAlloc (NULL,             // Any address
                  memVirtStr[MEMVIRT_WSZGLBTEMP].MaxSize,
                  MEM_RESERVE,      // memVirtStr
                  PAGE_READWRITE);
    if (memVirtStr[MEMVIRT_WSZGLBTEMP].IniAddr EQ NULL)
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsgW (L"InitInstance:  GuardAlloc for <lpwszGlbTemp> failed");

        return FALSE;       // Mark as failed
    } // End IF

    // Commit the intial size
    MyVirtualAlloc (memVirtStr[MEMVIRT_WSZGLBTEMP].IniAddr,
                    DEF_WGLBTEMP_INITNELM * sizeof (WCHAR),
                    MEM_COMMIT,
                    PAGE_READWRITE);

    //***************************************************************
    // Allocate virtual memory for the HshTab for {symbol} names & values
    //***************************************************************
    memVirtStr[MEMVIRT_GLBHSHTAB].lpText   = "htsGLB.lpHshTab in <InitInstance>";
    memVirtStr[MEMVIRT_GLBHSHTAB].IncrSize = DEF_GLBHSHTAB_INCRNELM * sizeof (htsGLB.lpHshTab[0]);
    memVirtStr[MEMVIRT_GLBHSHTAB].MaxSize  = DEF_GLBHSHTAB_MAXNELM  * sizeof (htsGLB.lpHshTab[0]);
    memVirtStr[MEMVIRT_GLBHSHTAB].IniAddr  = (LPUCHAR)
    htsGLB.lpHshTab =
      GuardAlloc (NULL,             // Any address
                  memVirtStr[MEMVIRT_GLBHSHTAB].MaxSize,
                  MEM_RESERVE,      // memVirtStr
                  PAGE_READWRITE);
    if (memVirtStr[MEMVIRT_GLBHSHTAB].IniAddr EQ NULL)
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsgW (L"InitInstance:  GuardAlloc for <lpGlbHshTab> failed");

        return FALSE;       // Mark as failed
    } // End IF

    // Commit the intial size
    MyVirtualAlloc (memVirtStr[MEMVIRT_GLBHSHTAB].IniAddr,
                    DEF_GLBHSHTAB_INITNELM * sizeof (htsGLB.lpHshTab[0]),
                    MEM_COMMIT,
                    PAGE_READWRITE);

    // Initialize the principal hash entry (1st one in each block).
    // This entry is never overwritten with an entry with a
    //   different hash value.
    for (i = 0; i < DEF_GLBHSHTAB_INITNELM; i += DEF_GLBHSHTAB_EPB)
        htsGLB.lpHshTab[i].htFlags.PrinHash = TRUE;

    // Initialize the next & prev same HTE values
    for (i = 0; i < DEF_GLBHSHTAB_INITNELM; i++)
    {
        htsGLB.lpHshTab[i].NextSameHash =
        htsGLB.lpHshTab[i].PrevSameHash = LPHSHENTRY_NONE;
    } // End FOR

    // Initialize the global HshTab values
    htsGLB.lphtsPrvSrch      =
    htsGLB.lphtsPrvMFO       = NULL;
    htsGLB.lpHshTabSplitNext = htsGLB.lpHshTab;
    htsGLB.iHshTabBaseNelm   = DEF_GLBHSHTAB_INITNELM;
    htsGLB.iHshTabTotalNelm  = DEF_GLBHSHTAB_INITNELM;
    htsGLB.iHshTabIncrFree   = DEF_GLBHSHTAB_INCRFREE;
    htsGLB.iHshTabIncrNelm   = DEF_GLBHSHTAB_INCRNELM;
    htsGLB.iHshTabEPB        = DEF_GLBHSHTAB_EPB;
    htsGLB.uHashMask         = DEF_GLBHSHTAB_HASHMASK;
    htsGLB.bGlbHshTab        = TRUE;
    htsGLB.lpSymTab          =
    htsGLB.lpSymTabNext      = NULL;
    htsGLB.iSymTabTotalNelm  = 0;

    // Read in the icons
    hIconMF_Large = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_MF_LARGE));
    hIconMF_Small = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_MF_SMALL));

    hIconSM_Large = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_SM_LARGE));
    hIconSM_Small = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_SM_SMALL));
#ifdef DEBUG
    hIconDB_Large = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_DB_LARGE));
    hIconDB_Small = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_DB_SMALL));
#endif
#ifdef PERFMONON
    hIconPM_Large = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_PM_LARGE));
    hIconPM_Small = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_PM_SMALL));
#endif
    hIconFE_Large = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_FE_LARGE));
    hIconFE_Small = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_FE_SMALL));

    hIconME_Large = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_ME_LARGE));
    hIconME_Small = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_ME_SMALL));

    hIconVE_Large = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_VE_LARGE));
    hIconVE_Small = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_VE_SMALL));

    hIconCC_Large = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_CC_LARGE));
    hIconCC_Small = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_CC_SMALL));

    hIconWC_Large = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_WC_LARGE));
    hIconWC_Small = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_WC_SMALL));

    hIconAbout    = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_ABOUT   ));
    hIconClose    = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_CLOSE   ));
    hIconCustom   = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_CUSTOM  ));

////// Get keyboard accelerators
//////   Not used with Customize Dialog for keyboards but left in to
//////     to see how it's done.
////hAccel = LoadAccelerators (hInstance, MAKEINTRESOURCE (IDR_ACCEL));

    // Initialize the cursors
    hCursorWait = LoadCursor (NULL, MAKEINTRESOURCE (IDC_WAIT));
    hCursorIdle = LoadCursor (NULL, MAKEINTRESOURCE (IDC_ARROW));

    // Initialize the window background brush
    ghBrushBG = CreateSolidBrush (gSyntaxColorText.crBack);

    return TRUE;
} // End InitInstance


//***************************************************************************
//  $UninitInstance
//
//  Uninitialize instance-specific resources
//***************************************************************************

void UninitInstance
    (HINSTANCE hInstance)

{
    UINT uCnt;              // Loop counter

    // Delete the window background brush
    if (ghBrushBG NE NULL)
    {
        DeleteObject (ghBrushBG); ghBrushBG = NULL;
    } // End IF


    // *************** Temporary Storage ***********************
    for (uCnt = 0; uCnt < MEMVIRT_LENGTH; uCnt++)
    if (memVirtStr[uCnt].IniAddr NE NULL)
    {
        // Free the virtual storage
        MyVirtualFree (memVirtStr[uCnt].IniAddr, 0, MEM_RELEASE);
    } // End IF

    // No need to destroy either the cursors or icons
} // UninitInstance


//***************************************************************************
//  $TestCmdLine
//
//  Test the command line for a keyword
//***************************************************************************

UBOOL TestCmdLine
    (LPCHAR  *lplpCmdLine,      // Ptr to ptr to command line
     LPCHAR  lpKeyWord,         // Ptr to keyword
     UINT    uMul,              // Multiplier
     LPUBOOL lpbDone,           // Ptr to bDone var
     LPUBOOL lpbRet,            // Ptr to bRet  var
     size_t *lpOutput)          // Ptr to output save area

{
    size_t iSpn;
    UINT   iVal;

    // Find the trailing char
    iSpn = strcspn (*lplpCmdLine, "= ");

    // If it matches, ...
    if (strncmpi (*lplpCmdLine, lpKeyWord, iSpn) EQ 0)
    {
        // Skip over the keyword
        *lplpCmdLine += lstrlen (lpKeyWord);

        // Skip over leading space
        *lplpCmdLine = SkipWhite (*lplpCmdLine);

        Assert ((*lplpCmdLine)[0] EQ '=');

        // If it's not the correct separator, ...
        if ((*lplpCmdLine)[0] NE '=')
        {
            *lpbDone = TRUE;
            *lpbRet = FALSE;
            MBW (L"Invalid command line argument -- missing '=' separator.");

            return FALSE;
        } else
        {
            // Skip over the separator
            (*lplpCmdLine)++;

////////////// Skip over leading space
////////////*lplpCmdLine = SkipWhite (*lplpCmdLine);    // atoi does this

            iVal = abs (atoi (*lplpCmdLine));
            if (iVal NE 0)
                *lpOutput = iVal * uMul;
             // Skip over the digits
             *lplpCmdLine += strspn (*lplpCmdLine, " 0123456789");

            return TRUE;
        } // End IF/ELSE
    } // End IF

    return FALSE;
} // End TestCmdLine


//***************************************************************************
//  $ParseCommandLine
//
//  Parse any command line
//***************************************************************************

UBOOL ParseCommandLine
    (LPSTR lpCmdLine)

{
    WCHAR  wszTempDPFE[1024];       // Temporary buffer
    UBOOL  bRet = TRUE;             // TRUE iff the result is valid

    // If there's a command line, ...
    if (lpCmdLine NE NULL && lstrlen (lpCmdLine) NE 0)
    {
        LPCHAR p;                   // Temporary ptr
        UBOOL  bDone = FALSE;       // TRUE iff we're bDone

        // Skip over leading space
        p = SkipWhite (lpCmdLine);

        // Loop until bDone
        while (!bDone)
        // Split cases based upon the leading char
        switch (*p)
        {
            case '/':           // Check for common switch chars
            case '-':           // ...
                // Skip over the separator
                p++;

                // Test for various keywords
                if (TestCmdLine (&p,
                                 "symtabsize",
                                 SYMTABSIZE_MUL,
                                &bDone,
                                &bRet,
                                &gSymTabSize))
                    break;
                if (TestCmdLine (&p,
                                 "hshtabsize",
                                 HSHTABSIZE_MUL,
                                &bDone,
                                &bRet,
                                &gHshTabSize))
                    break;

                bDone = TRUE;
                bRet = FALSE;
                MBW (L"Unknown command line argument.");

                break;

            case AC_EOS:
                bDone = TRUE;

                break;

            default:
                // Copy to temporary buffer
                A2W (wszTempDPFE, p, sizeof (wszTempDPFE));

                // Convert the []WSID workspace name into a canonical form (without WS_WKSEXT)
                MakeWorkspaceNameCanonical (wszLoadFile, wszTempDPFE, lpwszWorkDir);

                // Append the common workspace extension
                MyStrcatW (wszLoadFile, sizeof (wszLoadFile), WS_WKSEXT);

                bDone = TRUE;

                break;
        } // End WHILE/SWITCH

        // Mark as present
        bCommandLine = TRUE;
    } // End IF

    return bRet;
} // ParseCommandLine


//***************************************************************************
//  $WinMain
//
//  Start the process
//***************************************************************************

int PASCAL WinMain
    (HINSTANCE   hInstance,
     HINSTANCE   hPrevInstance,     // Zero under Win32
     LPSTR       lpCmdLine,
     int         nCmdShow)

{
    MSG                  Msg;           // Message for GetMessageW loop
    UINT                 uCnt;          // Loop counter
    INITCOMMONCONTROLSEX icex;          // Common control class struc

#ifndef DEBUG
    // Instantiate the Crash Server
    if (CrashServer ())
        // Set the CRASHRPT.DLL version #
        LclFileVersionStrW (crsh_dll, crsh_version, sizeof (crsh_version));
    else
    {
        WCHAR wszTemp[512];

        MySprintfW (wszTemp,                                // Ptr to buffer
                    sizeof (wszTemp),                       // Size of buffer (in bytes)
                   L"The file <%s> or one of its dependencies was not found."
                   L"  These files are part of the normal distribution of NARS2000, and should be in the same directory as the .exe file."
                   L"  Without them, if the program encounters a problem, no record of the error will be created."
                   L"  Are you sure you want to continue?", // Ptr to format string
                    crsh_dll);                              // The variable list
        if (MessageBoxW (hWndMF, wszTemp, WS_APPNAME, MB_OKCANCEL) EQ IDCANCEL)
            return -1;
    } // End IF/ELSE
#endif
    // Ensure that the system meets our minimum requirements (WinXP = 5.1)
    if (!IsWindowsXPSP3OrGreater ())
    {
        MessageBoxW (hWndMF,
                    L"Sorry, but the minimum OS version for " WS_APPNAME L" is WinXP SP3.",
                     lpwszAppName,
                     MB_OK | MB_ICONERROR);
        return -1;
    } // End IF

    // Initialize common controls.
    icex.dwSize = sizeof (INITCOMMONCONTROLSEX);
    icex.dwICC   = 0
                 | ICC_BAR_CLASSES
                 | ICC_COOL_CLASSES
                 | ICC_STANDARD_CLASSES
                 | ICC_TAB_CLASSES
                 | ICC_UPDOWN_CLASS
                 ;
    InitCommonControlsEx (&icex);
    InitCommonControls ();

#ifdef DEBUG
////InitQualitasDebugControls (hInstance);
#endif

#ifdef PERFMONON
    MessageBeep (NEG1U);
#endif
    SetCursor (LoadCursor (NULL, MAKEINTRESOURCE (IDC_APPSTARTING)));

    PERFMONINIT (NULL)

    // Copy initial Syntax Colors as default
    for (uCnt = 0; uCnt < SC_LENGTH; uCnt++)
    {
        defSyntaxColors  [uCnt] = gSyntaxColorName[uCnt].syntClr;
        defSyntClrBGTrans[uCnt] = gSyntClrBGTrans [uCnt];
    } // End IF

    // This is needed by Wine's EDITCTRL.C
    user32_module = hInstance;

////PERFMON

    // Construct file name(s) based upon where the module is on disk
    GetModuleFileNames (hInstance);

////PERFMON

    // Ensure the Application Data and workspaces
    //   directories are present
    if (!CreateAppDataDirs ())
        goto EXIT1;

////PERFMON

    // Save initial state
    nMinState = nCmdShow;

    // Allocate TLS indices
    dwTlsType        = TlsAlloc ();     // Thread type ('MF', 'TC', 'PL', etc.)
    dwTlsPlLocalVars = TlsAlloc ();     // lpplLocalVars (for 'PL' only)
    dwTlsFhLocalVars = TlsAlloc ();     // lpfhLocalVars (for 'PL' or 'SM' only)
    dwTlsPerTabData  = TlsAlloc ();     // lpMemPTD      (for 'PL' or 'SM' only)

    // Save the thread type ('MF')
    TlsSetValue (dwTlsType, TLSTYPE_MF);

////PERFMON

    // If there's a command line, parse it
    if (!ParseCommandLine (lpCmdLine))
        goto EXIT1;

////PERFMON

    // Perform initializations that apply to a specific instance
    if (!InitInstance (hInstance))
        goto EXIT2;

////PERFMON

    // Register the window class
    if (!InitApplication (hInstance))
        goto EXIT3;

////PERFMON

    // Allocate Critical Section objects
    //   for use in dtoa.c (2),
    //              parseLine, and
    //              HshTabFrisk.
    InitializeCriticalSection (&CSO0);
    InitializeCriticalSection (&CSO1);
#ifdef DEBUG
    InitializeCriticalSection (&CSOFrisk);
#endif
#if RESDEBUG
    InitializeCriticalSection (&CSORsrc);
#endif
    InitializeCriticalSection (&CSOPL);
    InitializeCriticalSection (&CSOTokenize);
    InitializeCriticalSection (&CSOHshTab);
    InitializeCriticalSection (&CSOPthread);

    // Mark as CSO defined
    bCSO = TRUE;

////PERFMON

    // Create various permanent variables
    MakePermVars ();

////PERFMON

    // Initialize all {symbol} names & values
    if (!InitSymbolNamesValues ())
        goto EXIT4;

////PERFMON

    // Read in global .ini file values
    if (!ReadIniFileGlb ())
        goto EXIT4;

////PERFMON

    // Initialize ChooseFontW arguments here
    //   so its settings will be present
    //   for the second and subsequent time
    //   the common dialog is called.
    InitChooseFont ();

////PERFMON

    // Initialize global numeric constants
    InitGlbNumConstants ();

    // Initialize tables for Primitive Fns, Operators, etc.
    InitPrimTabs ();

////PERFMON

#ifdef DEBUG
    InitFsaTabs ();
#endif

    // Get and save the current Thread Id
    dwMainThreadId = GetCurrentThreadId ();

////PERFMON

    //***************************************************************
    // Create the Master Frame window
    //***************************************************************
    hWndMF =
      CreateWindowExW (0L,                              // Extended styles
                       LMFWNDCLASS,                     // Class
                       wszMFTitle,                      // Title
                       0
                     | WS_OVERLAPPEDWINDOW
                       ,                                // Styles
                       CW_USEDEFAULT, CW_USEDEFAULT,    // X- and Y-coord
                       CW_USEDEFAULT, CW_USEDEFAULT,    // X- and Y-size
                       NULL,                            // Parent window
                       NULL,                            // Menu
                       _hInstance,                      // Instance
                       NULL);                           // No extra data
    if (hWndMF EQ NULL)
    {
        MB (pszNoCreateMFWnd);
        goto EXIT5;
    } // End IF

////PERFMON

////PERFMONSHOW (NULL)

#ifdef DEBUG
    __try
    {
#endif
        // Main message loop
        while (GetMessageW (&Msg, NULL, 0, 0))
        {
            HWND hWndMC;        // MDI Client window handle

            // Get the window handle of the currently active MDI Client
            hWndMC = GetActiveMC (hWndTC);

            // Pick off set of current lpMemPTD
            if (Msg.message EQ MYWM_LPMEMPTD)
                // Save ptr to PerTabData global memory
                TlsSetValue (dwTlsPerTabData, (LPVOID) (LPPERTABDATA) Msg.lParam);
            else
            {
                // Handle MDI messages and accelerators
                if (!TranslateMDISysAccel (hWndMC, &Msg)
                 && ((!hAccel) || !TranslateAcceleratorW (hWndMF, hAccel, &Msg)))
                {
                    // Check for DialogBox messages
                    if ((ghDlgCustomize EQ NULL || !IsDialogMessage (ghDlgCustomize, &Msg))
                     && (ghDlgUpdates   EQ NULL || !IsDialogMessage (ghDlgUpdates  , &Msg)))
                    {
                        TranslateMessage (&Msg);
                        DispatchMessageW (&Msg);
                    } // End IF/IF
                } // End IF/IF
            } // End IF/ELSE
        } // End WHILE
#ifdef DEBUG
    } __except (CheckException (GetExceptionInformation (), L"WinMain"))
    {
        // Display message for unhandled exception
        DisplayException ();
    } // End __try/__except
#endif

    // GetMessageW returned FALSE for a Quit message
EXIT5:
    // Uninitialize global numeric constants
    UninitGlbNumConstants ();
EXIT4:
    // Mark as all CSO deleted
    bCSO = FALSE;

    // Delete globals created by <ReadIniFileGlb>
    DeleIniFileGlb ();

    // Delete globals created by <MakePermVars>.
    DelePermVars ();

    DeleteCriticalSection (&CSOPthread);
    DeleteCriticalSection (&CSOHshTab);
    DeleteCriticalSection (&CSOTokenize);
    DeleteCriticalSection (&CSOPL);
#if RESDEBUG
    DeleteCriticalSection (&CSORsrc);
#endif
#ifdef DEBUG
    DeleteCriticalSection (&CSOFrisk);
#endif
    DeleteCriticalSection (&CSO1);
    DeleteCriticalSection (&CSO0);
EXIT3:
    UninitApplication (hInstance);
EXIT2:
    UninitInstance (hInstance);
EXIT1:
    return (int) Msg.wParam;
} // End WinMain


//***************************************************************************
//  $pthread_mutex_lock
//***************************************************************************

void pthread_mutex_lock
    (LPVOID mutex)

{
    EnterCriticalSection (&CSOPthread);
} // End pthread_mutex_lock


//***************************************************************************
//  $pthread_mutex_unlock
//***************************************************************************

void pthread_mutex_unlock
    (LPVOID mutex)

{
    LeaveCriticalSection (&CSOPthread);
} // End pthread_mutex_unlock


//***************************************************************************
//  End of File: main.c
//***************************************************************************
