//***************************************************************************
//  NARS2000 -- INI File Routines
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
#include <stdio.h>
#include <stdlib.h>
#include "headers.h"
////#include "bom.h"


// Section names
#define SECTNAME_GENERAL                L"General"
#define SECTNAME_SYSVARS                L"SysVars"
#define SECTNAME_FONTS                  L"Fonts"
#define SECTNAME_APLFONTS               L"APL Fonts"
#define SECTNAME_OPTIONS                L"Options"
#define SECTNAME_RANGELIMITS            L"RangeLimits"
#define SECTNAME_RESETVARS              L"ResetVars"
#define SECTNAME_SAMEFONTAS             L"SameFontAs"
#define SECTNAME_COLORS                 L"Colors"
#define SECTNAME_TOOLBARS               L"Toolbars"
#define SECTNAME_MIGRATION              L"Migration"
#define SECTNAME_RECENTFILES            L"RecentFiles"
#define SECTNAME_KEYBOARDS              L"Keyboards"
#define SECTNAME_KEYBPREFIX             L"KeybLayout-"
#define SECTNAME_LIBDIRS                L"LibDirs"

// Key names
#define KEYNAME_VERSION                 L"Version"
#define KEYNAME_SIZESTATE               L"SizeState"
#define KEYNAME_XPOS                    L"xPos"
#define KEYNAME_XSIZE                   L"xSize"
#define KEYNAME_YPOS                    L"yPos"
#define KEYNAME_YSIZE                   L"ySize"
#define KEYNAME_INITIALCAT              L"InitialCategory"
#define KEYNAME_COUNT                   L"Count"
#define KEYNAME_UNIBASE                 L"Unibase"
#define KEYNAME_CHAR                    L"Char"
#define KEYNAME_KEYBTCNUM               L"KeybTCNum"
#define KEYNAME_KEYBSTATE               L"KeybState"
#define KEYNAME_KEYBLAYOUTNAME          L"KeybLayoutName"
#define KEYNAME_KEYBSCANCODE            L"KSC"
#define KEYNAME_KEYBSCANCODE2B          L"KeybScanCode2B"
#define KEYNAME_KEYBEXTRAKEYROW3        L"KeybExtraKeyRow3"
#define KEYNAME_KEYBUSECXV              L"KeybUseCXV"
#define KEYNAME_KEYBUSEZY               L"KeybUseZY"
#define KEYNAME_KEYBUSESEQ              L"KeybUseSEQ"
#define KEYNAME_LOGFONTKB               L"LogFontKB"

#define KEYNAME_IC0LOG0                 L"IC0LOG0"

#define KEYNAME_QUADALX                 L"QuadALX"
#define KEYNAME_QUADCT                  L"QuadCT"
#define KEYNAME_QUADDT                  L"QuadDT"
#define KEYNAME_QUADELX                 L"QuadELX"
#define KEYNAME_QUADFC                  L"QuadFC"
#define KEYNAME_QUADFEATURE             L"QuadFEATURE"
#define KEYNAME_QUADFPC                 L"QuadFPC"
#define KEYNAME_QUADIC                  L"QuadIC"
#define KEYNAME_QUADIO                  L"QuadIO"
#define KEYNAME_QUADLX                  L"QuadLX"
#define KEYNAME_QUADMF                  L"QuadMF"
#define KEYNAME_QUADPP                  L"QuadPP"
#define KEYNAME_QUADPR                  L"QuadPR"
#define KEYNAME_QUADPW                  L"QuadPW"
#define KEYNAME_QUADRL                  L"QuadRL"
#define KEYNAME_QUADSA                  L"QuadSA"

#define KEYNAME_LOGFONTFE               L"LogFontFE"
#define KEYNAME_LOGFONTPR               L"LogFontPR"
#define KEYNAME_LOGFONTSM               L"LogFontSM"
#define KEYNAME_LOGFONTTC               L"LogFontTC"

#define KEYNAME_ADJUSTPW                L"AdjustPW"
#define KEYNAME_UNDERBARTOLOWERCASE     L"UnderbarToLowercase"
#define KEYNAME_NEWTABONCLEAR           L"NewTabOnClear"
#define KEYNAME_NEWTABONLOAD            L"NewTabOnload"
#define KEYNAME_USELOCALTIME            L"UseLocalTime"
#define KEYNAME_BACKUPONLOAD            L"BackupOnLoad"
#define KEYNAME_BACKUPONSAVE            L"BackupOnSave"
#define KEYNAME_NOCOPYRIGHTMSG          L"NoCopyrightMsg"
#define KEYNAME_DEFAULTPASTE            L"DefaultPaste"
#define KEYNAME_DEFAULTCOPY             L"DefaultCopy"
#define KEYNAME_SYNTCLRFCNS             L"SyntClrFcns"
#define KEYNAME_SYNTCLRSESS             L"SyntClrSess"
#define KEYNAME_SYNTCLRPRNT             L"SyntClrPrnt"
#define KEYNAME_CHECKGROUP              L"CheckGroup"
#define KEYNAME_INSSTATE                L"InsState"
#define KEYNAME_REVDBLCLK               L"RevDblClk"
#define KEYNAME_VIEWSTATUSBAR           L"ViewStatusBar"
#define KEYNAME_DEFDISPFCNLINENUMS      L"DefDispFcnLineNums"
#define KEYNAME_DISPMPSUF               L"DisplayMPSuffix"
#define KEYNAME_OUTPUTDEBUG             L"OutputDebug"

#define KEYNAME_SC_GLBNAME              L"GlbName"
#define KEYNAME_SC_LCLNAME              L"LclName"
#define KEYNAME_SC_LABEL                L"Label"
#define KEYNAME_SC_PRIMFCN              L"PrimFcn"
#define KEYNAME_SC_PRIMOP1              L"PrimOp1"
#define KEYNAME_SC_PRIMOP2              L"PrimOp2"
#define KEYNAME_SC_SYSFCN               L"SysFcn"
#define KEYNAME_SC_GLBSYSVAR            L"GlbSysVar"
#define KEYNAME_SC_LCLSYSVAR            L"LclSysVar"
#define KEYNAME_SC_CTRLSTRUC            L"CtrlStruc"
#define KEYNAME_SC_NUMCONST             L"NumConst"
#define KEYNAME_SC_CHRCONST             L"ChrConst"
#define KEYNAME_SC_PNSEP                L"PNSep"
#define KEYNAME_SC_COMMENT              L"Comment"
#define KEYNAME_SC_LINEDRAWING          L"LineDrawing"
#define KEYNAME_SC_FCNLINENUMS          L"FcnLineNums"
#define KEYNAME_SC_MATCHGRP1            L"MatchGrp1"
#define KEYNAME_SC_MATCHGRP2            L"MatchGrp2"
#define KEYNAME_SC_MATCHGRP3            L"MatchGrp3"
#define KEYNAME_SC_MATCHGRP4            L"MatchGrp4"
#define KEYNAME_SC_UNMATCHGRP           L"UnmatchGrp"
#define KEYNAME_SC_UNNESTED             L"UnnestedGrp"
#define KEYNAME_SC_UNK                  L"Unk"
#define KEYNAME_SC_LINECONT             L"LineContinuation"
#define KEYNAME_SC_WINTEXT              L"WinText"

#define KEYNAME_CUSTOMCOLORS            L"CustomColors"

#define KEYNAME_TOOLBAR_WS              L"Workspace"
#define KEYNAME_TOOLBAR_ED              L"Edit"
#define KEYNAME_TOOLBAR_FN              L"Objects"
#define KEYNAME_TOOLBAR_FW              L"SMFont"
#define KEYNAME_TOOLBAR_LW              L"Language"

#define KEYNAME_UPDFRQ                  L"UpdateFrequency"
#define KEYNAME_UPDCHK                  L"UpdateCheck"

// Format string for Keyboard Layout SCA Chars for each ScanCode
#define FMTSTR_KEYBCHARS        L"%04X, %04X, %04X, %04X, %04X, %04X, %04X, %04X"

// Format string for [Fonts] section LOGFONTW
#define FMTSTR_LOGFONT_INP      L"%d %d %d %d %d %d %d %d %d %d %d %d %d '%s'"
#define FMTSTR_LOGFONT_OUT      L"%d %d %d %d %d %d %d %d %d %d %d %d %d '%s'"

// Format string for [Colors] section
#define FMTSTR_SYNTAXCOLOR      L"%u, %u, %u"
#define FMTSTR_CUSTOMCOLORS     L"%u, %u, %u, %u, " \
                                L"%u, %u, %u, %u, " \
                                L"%u, %u, %u, %u, " \
                                L"%u, %u, %u, %u"

// Array of keynames for use in [Colors] section
LPWCHAR aColorKeyNames[] =
{KEYNAME_SC_GLBNAME     ,       // 00:  Global Name
 KEYNAME_SC_LCLNAME     ,       // 01:  Local  ...
 KEYNAME_SC_LABEL       ,       // 02:  Label
 KEYNAME_SC_PRIMFCN     ,       // 03:  Primitive Function
 KEYNAME_SC_PRIMOP1     ,       // 04:  Primitive Monadic Operator
 KEYNAME_SC_PRIMOP2     ,       // 05:  Primitive Dyadic Operator
 KEYNAME_SC_SYSFCN      ,       // 06:  System Function
 KEYNAME_SC_GLBSYSVAR   ,       // 07:  Global System Variable
 KEYNAME_SC_LCLSYSVAR   ,       // 08:  Local  ...
 KEYNAME_SC_CTRLSTRUC   ,       // 09:  Control Structure
 KEYNAME_SC_NUMCONST    ,       // 0A:  Numeric constant
 KEYNAME_SC_CHRCONST    ,       // 0B:  Character constant
 KEYNAME_SC_PNSEP       ,       // 0C:  Point notation separator
 KEYNAME_SC_COMMENT     ,       // 0D:  Comment
 KEYNAME_SC_LINEDRAWING ,       // 0E:  Line drawing chars
 KEYNAME_SC_FCNLINENUMS ,       // 0F:  Function line numbers
 KEYNAME_SC_MATCHGRP1   ,       // 10:  Matched Grouping Symbols [] () {}
 KEYNAME_SC_MATCHGRP2   ,       // 11:  Matched Grouping Symbols [] () {}
 KEYNAME_SC_MATCHGRP3   ,       // 12:  Matched Grouping Symbols [] () {}
 KEYNAME_SC_MATCHGRP4   ,       // 13:  Matched Grouping Symbols [] () {}
 KEYNAME_SC_UNMATCHGRP  ,       // 14:  Unmatched Grouping Symbols [] () {} ' "
 KEYNAME_SC_UNNESTED    ,       // 15:  Improperly Nested Grouping Symbols [] () {}
 KEYNAME_SC_UNK         ,       // 16:  Unknown symbol
 KEYNAME_SC_LINECONT    ,       // 17:  Line Continuation
 KEYNAME_SC_WINTEXT     ,       // 18:  Window text
};

// Array of keynames for use in [Toolbars] section
LPWCHAR aToolbarNames[] =
{KEYNAME_TOOLBAR_WS,            // Workspace
 KEYNAME_TOOLBAR_ED,            // Edit
 KEYNAME_TOOLBAR_FN,            // Objects
 KEYNAME_TOOLBAR_FW,            // SM Font
 KEYNAME_TOOLBAR_LW,            // Language
};

// Migration level struc
typedef struct tagMIGRATION
{
    UINT IC0LOG0:2,
                :31;
} MIGRATION, *LPMIGRATION;

// Default migration levels
MIGRATION uMigration = {0};

#define STRCNT_KEYBPREFIX       strcountof (SECTNAME_KEYBPREFIX)
#define STRCNT_KEYBSCANCODE     strcountof (KEYNAME_KEYBSCANCODE)

WCHAR wszSectName[STRCNT_KEYBPREFIX + 4],       // Room for prefix, some digits, and a terminating zero
      wszKeyName[STRCNT_KEYBSCANCODE + 2 + 1];  // Room for prefix, 2 hex digits, and a terminating zero


//***************************************************************************
//  $CreateAppDataDirs
//
//  Ensure the Application Data and workspaces
//    directories are present
//***************************************************************************

UBOOL CreateAppDataDirs
    (void)

{
    static WCHAR wszAppData[2 * MAX_PATH];
    DWORD        cchSize;
    WCHAR        wszTemp[1024];
    UINT         uNxt;

#define TEMPBUFLEN      countof (wszTemp)
#define WS_APPDATA          L"APPDATA"

    // If we didn't find it, ...
    if (0 EQ GetEnvironmentVariableW (WS_APPDATA, wszAppData, TEMPBUFLEN))
    {
        MBW (L"Unable to find " WS_APPDATA L" in the environment.");

        return FALSE;
    } // End IF

#undef  WS_APPDATA

    // Append the "\\NARS2000" part
    MyStrcatW (wszAppData, sizeof (wszAppData), WS_SLOPE WS_APPNAME);

    // Ensure the "...\\Application Data\\NARS2000" directory is present
    if (!CreateDirectoryW (wszAppData, NULL)
     && GetLastError () NE ERROR_ALREADY_EXISTS)
    {
        // Start with leading text
        strcpyW (wszTemp, L"CreateAppDataDirs (wszAppData):  ");
        uNxt = lstrlenW (wszTemp);

        // Format the error message
        FormatMessageW (FORMAT_MESSAGE_FROM_SYSTEM, // Source and processing options
                        NULL,                       // Pointer to  message source
                        GetLastError (),            // Requested message identifier
                        0,                          // Language identifier for requested message
                       &wszTemp[uNxt],              // Pointer to message buffer
                        TEMPBUFLEN - uNxt,          // Maximum size of message buffer
                        NULL);                      // Address of array of message inserts
#ifdef DEBUG
        DbgBrk ();          // #ifdef DEBUG
#endif
        MBW (wszTemp);

        return FALSE;
    } // End IF

    // Get the size so we can append to it
    cchSize = lstrlenW (wszAppData);

    // Now append the .ini file name to get lpwszIniFile
    strcpyW (&wszAppData[cchSize], WS_SLOPE WS_APPNAME L".ini");
    lpwszIniFile = wszAppData;

    // Get ptr to where the workspaces dir will be stored
    // The "+ 1" is to skip over the terminating zero
    lpwszWorkDir = &wszAppData[lstrlenW (lpwszIniFile) + 1];

    // Append the AppData directory part so we can form the workspaces dir
    // The "+ 1" is because strcpynW includes a terminating zero in the count
    strcpynW (lpwszWorkDir, lpwszIniFile, cchSize + 1);

    // Append the workspaces name
    strcatW (lpwszWorkDir, WS_SLOPE WS_WKSNAME);

    // Ensure the workspaces dir is present
    if (!CreateDirectoryW (lpwszWorkDir, NULL)
     && GetLastError () NE ERROR_ALREADY_EXISTS)
    {
        // Start with leading text
        strcpyW (wszTemp, L"CreateAppDataDirs (lpwszWorkDir):  ");
        uNxt = lstrlenW (wszTemp);

        // Format the error message
        FormatMessageW (FORMAT_MESSAGE_FROM_SYSTEM, // Source and processing options
                        NULL,                       // Pointer to  message source
                        GetLastError (),            // Requested message identifier
                        0,                          // Language identifier for requested message
                       &wszTemp[uNxt],              // Pointer to message buffer
                        TEMPBUFLEN - uNxt,          // Maximum size of message buffer
                        NULL);                      // Address of array of message inserts
#undef  TEMPBUFLEN
#ifdef DEBUG
        DbgBrk ();          // #ifdef DEBUG
#endif
        MBW (wszTemp);

        return FALSE;
    } // End IF

    // Ensure there's a trailing backslash
    strcatW (lpwszWorkDir, WS_SLOPE);

    return TRUE;
} // End CreateAppDataDirs


//***************************************************************************
//  $ReadAplFontNames
//
//  Read the [APL Fonts] names and fill a ComboBox with them
//***************************************************************************

void ReadAplFontNames
    (HWND hWndCB)               // Window handle to ComboBox

{
    int   iNumFonts,            // # APL Fonts
          iCnt;                 // Loop counter
    WCHAR wszTemp[1024],        // Temporary storage for string results
          wszCount[8];          // Save area for formatted APL Fonts counter

    // If there's an [APL Fonts] section, ...
    iNumFonts =
      GetPrivateProfileIntW (SECTNAME_APLFONTS,     // Ptr to the section name
                             KEYNAME_COUNT,         // Ptr to the key name
                             -1,                    // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // If there is no [APL Fonts] section, ...
    if (iNumFonts <= 0)
    {
        // Fill with the default APL fonts
        AppendAplFont (hWndCB, L"APL2 Unicode Italic"   );
        AppendAplFont (hWndCB, L"APL385 Unicode"        );
        AppendAplFont (hWndCB, L"APLX Upright"          );
        AppendAplFont (hWndCB, L"Courier APL2 Unicode"  );
        AppendAplFont (hWndCB, L"SImPL"                 );
        AppendAplFont (hWndCB, L"SImPL medium"          );
    } else
    // Loop through the Font Names
    for (iCnt = 0; iCnt < iNumFonts; iCnt++)
    {
        // Format the counter
        MySprintfW (wszCount,
                    sizeof (wszCount),
                   L"%d",
                    iCnt);
        // Read in the Font Name
        GetPrivateProfileStringW (SECTNAME_APLFONTS,    // Ptr to the section name
                                  wszCount,             // Ptr to the key name
                                  L"",                  // Ptr to the default value
                                  wszTemp,              // Ptr to the output buffer
                                  countof (wszTemp),    // Count of the output buffer
                                  lpwszIniFile);        // Ptr to the file name
        // If the name is present, ...
        if (wszTemp[0])
            // Append it to the ComboBox
            AppendAplFont (hWndCB, wszTemp);
    } // End IF/ELSE/FOR
} // End ReadAplFontNames


//***************************************************************************
//  $AppendAplFont
//
//  Append an APL font to a ComboBox, first ensuring that it's present
//***************************************************************************

void AppendAplFont
    (HWND    hWndCB,            // Window handle to ComboBox
     LPWCHAR lpwFontName)       // Ptr to APL font name

{
    HDC hDC;                    // Temporary Device Context

    // Initialize the global var
    bAPLFont = FALSE;

    hDC = MyGetDC (HWND_DESKTOP);
    SetAttrs (hDC, NULL, DEF_SCN_BLACK, DEF_SCN_WHITE);

    // Enumerate fonts to ensure our default font is installed
    EnumFontsW (hDC,
                lpwFontName,
                (FONTENUMPROCW) EnumCallbackFindAplFont,
       (LPARAM) lpwFontName);
    // Release the DC
    MyReleaseDC (HWND_DESKTOP, hDC); hDC = NULL;

    // If we found the font name, ...
    if (bAPLFont)
        // Add the Font Name to our list
        SendMessageW (hWndCB, CB_ADDSTRING, 0, (LPARAM) lpwFontName);
} // End AppendAplFont


//***************************************************************************
//  $WriteAplFontNames
//
//  Write out the APL Font Names from a given ComboBox
//    to the [APL Fonts] section
//***************************************************************************

void WriteAplFontNames
    (HWND hWndCB)               // Window handle to ComboBox

{
    UINT  uNumFonts,            // # APL Fonts
          uCnt;                 // Loop counter
    WCHAR wszTemp[1024],        // Temporary storage for string results
          wszCount[8];          // Save area for formatted APL Fonts counter

    // Clear the [APL Fonts] section
    WritePrivateProfileSectionW (SECTNAME_APLFONTS, // Ptr to the section name
                                 NULL,              // No data
                                 lpwszIniFile);     // Ptr to the file name
    // Get the # APL Fonts
    uNumFonts = (UINT)
      SendMessageW (hWndCB, CB_GETCOUNT, 0, 0);

    // Loop through the Font Names
    for (uCnt = 0; uCnt < uNumFonts; uCnt++)
    {
        // Get the next Font Name
        SendMessageW (hWndCB, CB_GETLBTEXT, uCnt, (LPARAM) wszTemp);

        // Format the counter
        MySprintfW (wszCount,
                    sizeof (wszCount),
                   L"%d",
                    uCnt);
        // Write out the Font Name
        WritePrivateProfileStringW (SECTNAME_APLFONTS,  // Ptr to the section name
                                    wszCount,           // Ptr to the key name
                                    wszTemp,            // Ptr to the key value
                                    lpwszIniFile);      // Ptr to the file name
    } // End FOR

    // Format the counter
    MySprintfW (wszCount,
                sizeof (wszCount),
               L"%d",
                uNumFonts);
    // Write out the count
    WritePrivateProfileStringW (SECTNAME_APLFONTS,  // Ptr to the section name
                                KEYNAME_COUNT,      // Ptr to the key name
                                wszCount,           // Ptr to the key value
                                lpwszIniFile);      // Ptr to the file name
} // End WriteAplFontNames


//***************************************************************************
//  $ReadIniFileGlb
//
//  Read in global-specific .ini file settings
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ReadIniFileGlb"
#else
#define APPEND_NAME
#endif

UBOOL ReadIniFileGlb
    (void)

{
    WCHAR         wszTemp[1024],                    // Temporary storage for string results
                  wszKey[8 + 1],                    // Room for a keyname
                 *lpwszTemp;                        // Temporary ptr into wszTemp
    UINT          uCnt,                             // Loop counter
                  uCnt2,                            // ...
                  uCol;                             // ...
    WCHAR       (*lpwszRecentFiles)[][_MAX_PATH];   // Ptr to list of recent files
    LPKEYBLAYOUTS lpKeybLayouts;                    // Ptr to keyboard layouts global memory
    LPWSZLIBDIRS  lpwszLibDirs;                     // Ptr to LibDirs
    WORD          hkl;                              // Default keyboard layout
    LPWCHAR       lpKeybLayout;                     // Ptr to default keyboard layout name

#define TEMPBUFLEN      countof (wszTemp)

    //***************************************************************
    // Read in the [LibDirs] section
    //***************************************************************

    // Get the # LibDirs
    uNumLibDirs =
      GetPrivateProfileIntW (SECTNAME_LIBDIRS,      // Ptr to the section name
                             KEYNAME_COUNT,         // Ptr to the key name
                             0,                     // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Allocate space for the LibDirs
    hGlbLibDirs =
      DbgGlobalAlloc (GHND, max (uNumLibDirs, 2) * _MAX_PATH * sizeof (WCHAR));
    if (hGlbLibDirs EQ NULL)
    {
        MessageBoxW (hWndMF, L"Unable to allocate enough memory for Library Directories", WS_APPNAME, MB_OK | MB_ICONSTOP);

        return FALSE;
    } // End IF

    // Lock the memory to get a ptr to it
    lpwszLibDirs = MyGlobalLock000 (hGlbLibDirs);

    // Check for no LibDirs
    if (uNumLibDirs EQ 0)
    {
        // Use the default count
        uNumLibDirs = 2;

        // Append the two default dirs
        strcpyW (lpwszLibDirs[0], L".");
        strcpyW (lpwszLibDirs[1], lpwszWorkDir);
    } else
    // Loop through the LibDirs
    for (uCnt = 0; uCnt < uNumLibDirs; uCnt++)
    {
        // Format the keyname
        MySprintfW (wszKey,
                    sizeof (wszKey),
                   L"%u",
                    uCnt);
        // Read in the next LibDir
        GetPrivateProfileStringW (SECTNAME_LIBDIRS,     // Ptr to the section name
                                  wszKey,               // Ptr to the key name
                                  L"",                  // Ptr to the default value
                                  lpwszLibDirs[uCnt],   // Ptr to the output buffer
                                  _MAX_PATH,            // Count of the output buffer
                                  lpwszIniFile);        // Ptr to the file name
    } // End IF/ELSE/FOR

    // We no longer need this ptr
    MyGlobalUnlock (hGlbLibDirs); lpwszLibDirs = NULL;

    //***************************************************************
    // Read in the [Fonts] section
    //***************************************************************

    // Read in the LOGFONTW strucs
    GetPrivateProfileLogFontW (SECTNAME_FONTS, KEYNAME_LOGFONTFE, &lfFE);
    GetPrivateProfileLogFontW (SECTNAME_FONTS, KEYNAME_LOGFONTPR, &lfPR);
    GetPrivateProfileLogFontW (SECTNAME_FONTS, KEYNAME_LOGFONTSM, &lfSM);
    GetPrivateProfileLogFontW (SECTNAME_FONTS, KEYNAME_LOGFONTTC, &lfTC);

    //***************************************************************
    // Read in the [SameFontAs] section
    //***************************************************************

    // Loop through the items of glbSameFontAs
    for (uCnt = 0; uCnt < FONTENUM_LENGTH; uCnt++)
    {
        // Format the keyname
        MySprintfW (wszTemp,
                    sizeof (wszTemp),
                   L"%u",
                    uCnt);
        // Read in the glbSameFontAs value
        glbSameFontAs[uCnt] =
          GetPrivateProfileIntW (SECTNAME_SAMEFONTAS,   // Ptr to the section name
                                 wszTemp,               // Ptr to the key name
                                 uCnt,                  // Default value if not found
                                 lpwszIniFile);         // Ptr to the file name
        // Overcome a bug in which the saved glbSameFontAs value may be
        //   larger than the corresponding index
        glbSameFontAs[uCnt] = min (glbSameFontAs[uCnt], (int) uCnt);
    } // End FOR

    //***************************************************************
    // Read in the [Options] section
    //***************************************************************

    // Read in bAdjustPW
    OptionFlags.bAdjustPW =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_ADJUSTPW,      // Ptr to the key name
                             DEF_ADJUSTPW,          // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bUnderbarToLowercase
    OptionFlags.bUnderbarToLowercase =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_UNDERBARTOLOWERCASE,// Ptr to the key name
                             DEF_UNDERBARTOLOWERCASE,// Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bNewTabOnClear
    OptionFlags.bNewTabOnClear =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_NEWTABONCLEAR, // Ptr to the key name
                             DEF_NEWTABONCLEAR,     // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bNewTabOnLoad
    OptionFlags.bNewTabOnLoad =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_NEWTABONLOAD,  // Ptr to the key name
                             DEF_NEWTABONLOAD,      // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bUseLocalTime
    OptionFlags.bUseLocalTime =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_USELOCALTIME,  // Ptr to the key name
                             DEF_USELOCALTIME,      // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bBackupOnLoad
    OptionFlags.bBackupOnLoad =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_BACKUPONLOAD,  // Ptr to the key name
                             DEF_BACKUPONLOAD,      // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bBackupOnSave
    OptionFlags.bBackupOnSave =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_BACKUPONSAVE,  // Ptr to the key name
                             DEF_BACKUPONSAVE,      // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bNoCopyrightMsg
    OptionFlags.bNoCopyrightMsg =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_NOCOPYRIGHTMSG,// Ptr to the key name
                             DEF_NOCOPYRIGHTMSG,    // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in uDefaultPaste
    OptionFlags.uDefaultPaste =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_DEFAULTPASTE,  // Ptr to the key name
                             DEF_DEFAULTPASTE,      // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in uDefaultCopy
    OptionFlags.uDefaultCopy =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_DEFAULTCOPY,   // Ptr to the key name
                             DEF_DEFAULTCOPY,       // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bSyntClrFcns
    OptionFlags.bSyntClrFcns =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_SYNTCLRFCNS,   // Ptr to the key name
                             DEF_SYNTCLRFCNS,       // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bSyntClrSess
    OptionFlags.bSyntClrSess =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_SYNTCLRSESS,   // Ptr to the key name
                             DEF_SYNTCLRSESS,       // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bSyntClrPrnt
    OptionFlags.bSyntClrPrnt =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_SYNTCLRPRNT,   // Ptr to the key name
                             DEF_SYNTCLRPRNT,       // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bCheckGroup
    OptionFlags.bCheckGroup =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_CHECKGROUP,    // Ptr to the key name
                             DEF_CHECKGROUP,        // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bInsState
    OptionFlags.bInsState =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_INSSTATE,      // Ptr to the key name
                             DEF_INSSTATE,          // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bRevDblClk
    OptionFlags.bRevDblClk =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_REVDBLCLK,     // Ptr to the key name
                             DEF_REVDBLCLK,         // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bViewStatusBar
    OptionFlags.bViewStatusBar =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_VIEWSTATUSBAR, // Ptr to the key name
                             DEF_VIEWSTATUSBAR,     // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bDefDispFcnLineNums
    OptionFlags.bDefDispFcnLineNums =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_DEFDISPFCNLINENUMS, // Ptr to the key name
                             DEF_DISPFCNLINENUMS,   // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bDispMPSuf
    OptionFlags.bDispMPSuf =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_DISPMPSUF,     // Ptr to the key name
                             DEF_DISPMPSUF,         // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bOutputDebug
    OptionFlags.bOutputDebug =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_OUTPUTDEBUG,   // Ptr to the key name
                             DEF_OUTPUTDEBUG,       // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in the Line Continuation char as an integer
    uUserChar =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_CHAR,          // Ptr to the key name
                             WC_LC,                 // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in the Unicode base for typed chars (10 or 16)
    uUserUnibase =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_UNIBASE,       // Ptr to the key name
                             16,                    // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Ensure the Unicode base is either 10 or 16
    if (uUserUnibase NE 10
     && uUserUnibase NE 16)
        uUserUnibase = 16;

    //***************************************************************
    // Read in the [SysVars] section -- default values for system
    //                                  variables in a CLEAR WS
    //***************************************************************

    // Read in []ALX
    hGlbQuadALX_CWS =
      GetPrivateProfileGlbCharW (SECTNAME_SYSVARS,  // Ptr to the section name
                                 KEYNAME_QUADALX,   // Ptr to the key name
                                 DEF_QUADALX_CWS,   // Ptr to default value
                                 DEF_QUADALX_GLB,   // HGLOBAL of the default value
                                 lpwszIniFile);     // Ptr to the file name
    // Read in []CT
    fQuadCT_CWS =
      GetPrivateProfileFloatW (SECTNAME_SYSVARS,    // Ptr to the section name
                               KEYNAME_QUADCT,      // Ptr to the key name
                               DEF_QUADCT_CWS,      // Ptr to the default value
                               lpwszIniFile);       // Ptr to the file name
    // Read in []DT
    GetPrivateProfileStringW (SECTNAME_SYSVARS,     // Ptr to the section name
                              KEYNAME_QUADDT,       // Ptr to the key name
                              DEF_QUADDT_CWS,       // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              TEMPBUFLEN,           // Count of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // Convert any {name}s to symbols
    ConvertNameInPlace (wszTemp);
    cQuadDT_CWS = wszTemp[0];

    // Read in []ELX
    hGlbQuadELX_CWS =
      GetPrivateProfileGlbCharW (SECTNAME_SYSVARS,  // Ptr to the section name
                                 KEYNAME_QUADELX,   // Ptr to the key name
                                 DEF_QUADELX_CWS,   // Ptr to default value
                                 DEF_QUADELX_GLB,   // HGLOBAL of the default value
                                 lpwszIniFile);     // Ptr to the file name
    // Read in []FC
    hGlbQuadFC_CWS =
      GetPrivateProfileGlbCharW (SECTNAME_SYSVARS,  // Ptr to the section name
                                 KEYNAME_QUADFC,    // Ptr to the key name
                                 DEF_QUADFC_CWS,    // Ptr to default value
                                 DEF_QUADFC_GLB,    // HGLOBAL of the default value
                                 lpwszIniFile);     // Ptr to the file name
    // Loop through the items of aplDefaultFEATURE
    for (uCnt = 0, lpwszTemp = wszTemp; uCnt < FEATURENDX_LENGTH; uCnt++)
        // Make the default wide-string form of aplDefaultFEATURE
        lpwszTemp +=
          wsprintfW (lpwszTemp,
                     L"%d ",
                     aplDefaultFEATURE[uCnt]);
    // Read in []FEATURE
    hGlbQuadFEATURE_CWS =
      GetPrivateProfileGlbIntW (SECTNAME_SYSVARS,       // Ptr to the section name
                                KEYNAME_QUADFEATURE,    // Ptr to the key name
                                wszTemp,                // Ptr to default value
                                DEF_QUADFEATURE_GLB,    // HGLOBAL of the default value
                                FEATURENDX_LENGTH,      // Length of the default integer vector
                                lpwszIniFile);          // Ptr to the file name
    // Read in []FPC
    uQuadFPC_CWS =
      GetPrivateProfileIntW (SECTNAME_SYSVARS,      // Ptr to the section name
                             KEYNAME_QUADFPC,       // Ptr to the key name
                             DEF_QUADFPC_CWS,       // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Loop through the items of aplDefaultIC
    for (uCnt = 0, lpwszTemp = wszTemp; uCnt < ICNDX_LENGTH; uCnt++)
        // Make the default wide-string form of aplDefaultIC
        lpwszTemp +=
          wsprintfW (lpwszTemp,
                     L"%d ",
                     aplDefaultIC[uCnt]);
    // Read in []IC
    hGlbQuadIC_CWS =
      GetPrivateProfileGlbIntW (SECTNAME_SYSVARS,   // Ptr to the section name
                                KEYNAME_QUADIC,     // Ptr to the key name
                                wszTemp,            // Ptr to default value
                                DEF_QUADIC_GLB,     // HGLOBAL of the default value
                                ICNDX_LENGTH,       // Length of the default integer vector
                                lpwszIniFile);      // Ptr to the file name
    // Read in uMigration.IC0LOG0
    uMigration.IC0LOG0 =
      GetPrivateProfileIntW (SECTNAME_MIGRATION,    // Ptr to the section name
                             KEYNAME_IC0LOG0,       // Ptr to the key name
                             0,                     // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // If it's unfixed, ...
    if (uMigration.IC0LOG0 EQ 0)
    {
        LPAPLINT lpMemQuadIC;

        // Lock the memory to get a ptr to it
        lpMemQuadIC = MyGlobalLockVar (hGlbQuadIC_CWS);

        // Skip over the header and dimensions to the data
        lpMemQuadIC = VarArrayDataFmBase (lpMemQuadIC);

        // Set the new value
        lpMemQuadIC[ICNDX_0LOG0] = aplDefaultIC[ICNDX_0LOG0];

        // We no longer need this ptr
        MyGlobalUnlock (hGlbQuadIC_CWS); lpMemQuadIC = NULL;

        // Increment the migration level for IC0LOG0
        uMigration.IC0LOG0++;
    } // End IF

    // Read in []IO
    bQuadIO_CWS = (APLBOOL)
      GetPrivateProfileIntW (SECTNAME_SYSVARS,      // Ptr to the section name
                             KEYNAME_QUADIO,        // Ptr to the key name
                             DEF_QUADIO_CWS,        // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in []LX
    hGlbQuadLX_CWS =
      GetPrivateProfileGlbCharW (SECTNAME_SYSVARS,  // Ptr to the section name
                                 KEYNAME_QUADLX,    // Ptr to the key name
                                 DEF_QUADLX_CWS,    // Ptr to default value
                                 DEF_QUADLX_GLB,    // HGLOBAL of the default value
                                 lpwszIniFile);     // Ptr to the file name
    // Read in []MF
    uQuadMF_CWS =
      GetPrivateProfileIntW (SECTNAME_SYSVARS,      // Ptr to the section name
                             KEYNAME_QUADMF,        // Ptr to the key name
                             DEF_QUADMF_CWS,        // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in []PP
    uQuadPP_CWS =
      GetPrivateProfileIntW (SECTNAME_SYSVARS,      // Ptr to the section name
                             KEYNAME_QUADPP,        // Ptr to the key name
                             DEF_QUADPP_CWS,        // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in []PW
    uQuadPW_CWS =
      GetPrivateProfileIntW (SECTNAME_SYSVARS,      // Ptr to the section name
                             KEYNAME_QUADPW,        // Ptr to the key name
                             DEF_QUADPW_CWS,        // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in []PR
    GetPrivateProfileStringW (SECTNAME_SYSVARS,     // Ptr to the section name
                              KEYNAME_QUADPR,       // Ptr to the key name
                              DEF_QUADPR_CWS,       // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              TEMPBUFLEN,           // Count of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // Convert any {name}s to symbols
    ConvertNameInPlace (wszTemp);

    // If the value is empty (''), ...
    if (wszTemp[0] EQ WC_EOS)
    {
        cQuadPR_CWS = CQUADPR_MT;
        hGlbQuadPR_CWS = hGlbV0Char;
    } else
        cQuadPR_CWS = wszTemp[0];

    // Read in []RL as a string as the IntW form doesn't support 64-bit integers
    GetPrivateProfileStringW (SECTNAME_SYSVARS,     // Ptr to the section name
                              KEYNAME_QUADRL,       // Ptr to the key name
                              DEF_QUADRL_CWS_WS,    // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              TEMPBUFLEN,           // Count of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // Convert to a number
    sscanfW (wszTemp, L"%I64u", &uQuadRL_CWS);

    // A past bug in Customize set []RL to zero if it exceeded INT_MAX, so we check for that here
    if (uQuadRL_CWS EQ 0)
        // Set to the default value
        uQuadRL_CWS = DEF_QUADRL_CWS;

    // Read in []SA index
    cQuadxSA_CWS = (APLCHAR)
      GetPrivateProfileIntW (SECTNAME_SYSVARS,      // Ptr to the section name
                             KEYNAME_QUADSA,        // Ptr to the key name
                             DEF_QUADxSA_CWS,       // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Set the correct value for []SA
    switch (cQuadxSA_CWS)
    {
        case 0:         // ""
            hGlbQuadSA_CWS = hGlbV0Char;
            break;

        case 1:         // "CLEAR"
            hGlbQuadSA_CWS = hGlbSAClear;
            break;

        case 2:         // "ERROR"
            hGlbQuadSA_CWS = hGlbSAError;
            break;

        case 3:         // "EXIT"
            hGlbQuadSA_CWS = hGlbSAExit;
            break;

        case 4:         // "OFF"
            hGlbQuadSA_CWS = hGlbSAOff;
            break;

        defstop
            break;
    } // End SWITCH

    //***************************************************************
    // Read in the [RangeLimits] section
    //***************************************************************

    // Read in bRangeLimit.CT
    bRangeLimit.CT =
      GetPrivateProfileIntW (SECTNAME_RANGELIMITS,  // Ptr to the section name
                             KEYNAME_QUADCT,        // Ptr to the key name
                             DEF_RANGELIMIT_CT,     // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bRangeLimit.FEATURE
    bRangeLimit.FEATURE =
      GetPrivateProfileIntW (SECTNAME_RANGELIMITS,  // Ptr to the section name
                             KEYNAME_QUADFEATURE,   // Ptr to the key name
                             DEF_RANGELIMIT_FEATURE,// Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bRangeLimit.FPC
    bRangeLimit.FPC =
      GetPrivateProfileIntW (SECTNAME_RANGELIMITS,  // Ptr to the section name
                             KEYNAME_QUADFPC,       // Ptr to the key name
                             DEF_RANGELIMIT_FPC,    // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bRangeLimit.IC
    bRangeLimit.IC =
      GetPrivateProfileIntW (SECTNAME_RANGELIMITS,  // Ptr to the section name
                             KEYNAME_QUADIC,        // Ptr to the key name
                             DEF_RANGELIMIT_IC,     // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bRangeLimit.IO
    bRangeLimit.IO =
      GetPrivateProfileIntW (SECTNAME_RANGELIMITS,  // Ptr to the section name
                             KEYNAME_QUADIO,        // Ptr to the key name
                             DEF_RANGELIMIT_IO,     // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bRangeLimit.PP
    bRangeLimit.PP =
      GetPrivateProfileIntW (SECTNAME_RANGELIMITS,  // Ptr to the section name
                             KEYNAME_QUADPP,        // Ptr to the key name
                             DEF_RANGELIMIT_PP,     // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bRangeLimit.PW
    bRangeLimit.PW =
      GetPrivateProfileIntW (SECTNAME_RANGELIMITS,  // Ptr to the section name
                             KEYNAME_QUADPW,        // Ptr to the key name
                             DEF_RANGELIMIT_PW,     // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bRangeLimit.RL
    bRangeLimit.RL =
      GetPrivateProfileIntW (SECTNAME_RANGELIMITS,  // Ptr to the section name
                             KEYNAME_QUADRL,        // Ptr to the key name
                             DEF_RANGELIMIT_RL,     // Default value if not found
                             lpwszIniFile);         // Ptr to the file name

    //***************************************************************
    // Read in the [ResetVars] section
    //***************************************************************

    // Read in bResetVars.CT
    bResetVars.CT =
      GetPrivateProfileIntW (SECTNAME_RESETVARS,    // Ptr to the section name
                             KEYNAME_QUADCT,        // Ptr to the key name
                             DEF_RESETVARS_CT,      // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bResetVars.DT
    bResetVars.DT =
      GetPrivateProfileIntW (SECTNAME_RESETVARS,    // Ptr to the section name
                             KEYNAME_QUADDT,        // Ptr to the key name
                             DEF_RESETVARS_DT,      // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bResetVars.FC
    bResetVars.FC =
      GetPrivateProfileIntW (SECTNAME_RESETVARS,    // Ptr to the section name
                             KEYNAME_QUADFC,        // Ptr to the key name
                             DEF_RESETVARS_FC,      // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bResetVars.FEATURE
    bResetVars.FEATURE =
      GetPrivateProfileIntW (SECTNAME_RESETVARS,    // Ptr to the section name
                             KEYNAME_QUADFEATURE,   // Ptr to the key name
                             DEF_RESETVARS_FEATURE, // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bResetVars.FPC
    bResetVars.FPC =
      GetPrivateProfileIntW (SECTNAME_RESETVARS,    // Ptr to the section name
                             KEYNAME_QUADFPC,       // Ptr to the key name
                             DEF_RESETVARS_FPC,     // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bResetVars.IC
    bResetVars.IC =
      GetPrivateProfileIntW (SECTNAME_RESETVARS,    // Ptr to the section name
                             KEYNAME_QUADIC,        // Ptr to the key name
                             DEF_RESETVARS_IC,      // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bResetVars.IO
    bResetVars.IO =
      GetPrivateProfileIntW (SECTNAME_RESETVARS,    // Ptr to the section name
                             KEYNAME_QUADIO,        // Ptr to the key name
                             DEF_RESETVARS_IO,      // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bResetVars.PP
    bResetVars.PP =
      GetPrivateProfileIntW (SECTNAME_RESETVARS,    // Ptr to the section name
                             KEYNAME_QUADPP,        // Ptr to the key name
                             DEF_RESETVARS_PP,      // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bResetVars.PW
    bResetVars.PW =
      GetPrivateProfileIntW (SECTNAME_RESETVARS,    // Ptr to the section name
                             KEYNAME_QUADPW,        // Ptr to the key name
                             DEF_RESETVARS_PW,      // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bResetVars.RL
    bResetVars.RL =
      GetPrivateProfileIntW (SECTNAME_RESETVARS,    // Ptr to the section name
                             KEYNAME_QUADRL,        // Ptr to the key name
                             DEF_RESETVARS_RL,      // Default value if not found
                             lpwszIniFile);         // Ptr to the file name

    //***************************************************************
    // Read in the [Colors] section
    //***************************************************************

    Assert (SC_LENGTH EQ countof (aColorKeyNames));

    // Loop through the color names
    for (uCnt = 0; uCnt < SC_LENGTH; uCnt++)
    {
        // Read in the string
        GetPrivateProfileStringW (SECTNAME_COLORS,      // Ptr to the section name
                                  aColorKeyNames[uCnt], // Ptr to the key name
                                  L"",                  // Ptr to the default value
                                  wszTemp,              // Ptr to the output buffer
                                  TEMPBUFLEN,           // Count of the output buffer
                                  lpwszIniFile);        // Ptr to the file name
        // Check for value
        if (wszTemp[0] NE WC_EOS)
            // Convert the numbers
            sscanfW (wszTemp,
                     FMTSTR_SYNTAXCOLOR,
                    &gSyntaxColorName[uCnt].syntClr.crFore,
                    &gSyntaxColorName[uCnt].syntClr.crBack,
                    &gSyntClrBGTrans [uCnt]);
    } // End FOR

    // Respecify the Window Background brush
    //   because the Window Background color has changed
    RedoWinBG ();

    // Read in the CustomColors
    GetPrivateProfileStringW (SECTNAME_COLORS,      // Ptr to the section name
                              KEYNAME_CUSTOMCOLORS, // Ptr to the key name
                              L"",                  // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              TEMPBUFLEN,           // Count of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // Check for value
    if (wszTemp[0] NE WC_EOS)
        // Convert the numbers
        sscanfW (wszTemp,
                 FMTSTR_CUSTOMCOLORS,
                &aCustomColors[ 0], &aCustomColors[ 1], &aCustomColors[ 2], &aCustomColors[ 3],
                &aCustomColors[ 4], &aCustomColors[ 5], &aCustomColors[ 6], &aCustomColors[ 7],
                &aCustomColors[ 8], &aCustomColors[ 9], &aCustomColors[10], &aCustomColors[11],
                &aCustomColors[12], &aCustomColors[13], &aCustomColors[14], &aCustomColors[15]);

    //***************************************************************
    // Read in the [Toolbars] section
    //***************************************************************

    // Loop through the Toolbars
    for (uCnt = 0; uCnt < countof (aRebarBands); uCnt++)
        // Read in the Toolbars setting
        aRebarBands[uCnt].bShowBand =
          GetPrivateProfileIntW (SECTNAME_TOOLBARS,     // Ptr to the section name
                                 aToolbarNames[uCnt],   // Ptr to the key name
                                 TRUE,                  // Default value if not found
                                 lpwszIniFile);         // Ptr to the file name

    //***************************************************************
    // Read in the [RecentFiles] section
    //***************************************************************

    // Get the # entries in the Recent Files list
    uNumRecentFiles =
      GetPrivateProfileIntW (SECTNAME_RECENTFILES,  // Ptr to the section name
                             KEYNAME_COUNT,         // Ptr to the key name
                             0,                     // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Allocate space for the Recent Files list
    hGlbRecentFiles =
      DbgGlobalAlloc (GHND, DEF_RECENTFILES * _MAX_PATH * sizeof (WCHAR));

    // Check for error
    if (hGlbRecentFiles EQ NULL)
        return FALSE;           // Stop the whole process

    // Lock the memory to get a ptr to it
    lpwszRecentFiles = MyGlobalLock000 (hGlbRecentFiles);

    // Loop through the Recent Files
    for (uCnt = 0; uCnt < uNumRecentFiles; uCnt++)
    {
        // Format the keyname
        MySprintfW (wszKey,
                    sizeof (wszKey),
                   L"%u",
                    uCnt);
        // Read in the next Recent File
        GetPrivateProfileStringW (SECTNAME_RECENTFILES,         // Ptr to the section name
                                  wszKey,                       // Ptr to the key name
                                  L"",                          // Ptr to the default value
                                  (*lpwszRecentFiles)[uCnt],    // Ptr to the output buffer
                                  _MAX_PATH,                    // Count of the output buffer
                                  lpwszIniFile);                // Ptr to the file name
    } // End FOR

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRecentFiles); lpwszRecentFiles = NULL;

    //***************************************************************
    // Read in the [Keyboards] section
    //***************************************************************

    // Get the # user-defined keyboards
    uGlbKeybLayoutUser =
      GetPrivateProfileIntW (SECTNAME_KEYBOARDS,    // Ptr to the section name
                             KEYNAME_COUNT,         // Ptr to the key name
                             0,                     // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Allocate space for the # built-in layouts + user-defined
    hGlbKeybLayouts =
      DbgGlobalAlloc (GHND, (uGlbKeybLayoutBI + uGlbKeybLayoutUser) * sizeof (KEYBLAYOUTS));
    if (hGlbKeybLayouts EQ NULL)
    {
        MessageBoxW (hWndMF, L"Unable to allocate enough memory for Keyboard Layouts", WS_APPNAME, MB_OK | MB_ICONSTOP);

        return FALSE;           // Stop the whole process
    } // End IF

    // Lock the memory to get a ptr to it
    lpKeybLayouts = MyGlobalLock000 (hGlbKeybLayouts);

    // Get the Unicode base for typed chars (10 or 16)
    uKeybUnibase =
      GetPrivateProfileIntW (SECTNAME_KEYBOARDS,    // Ptr to the section name
                             KEYNAME_UNIBASE,       // Ptr to the key name
                             16,                    // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Ensure the Unicode base is either 10 or 16
    if (uKeybUnibase NE 10
     && uKeybUnibase NE 16)
        uKeybUnibase = 16;

    // Get the last keyboard unicode character selected
    uKeybChar =
      GetPrivateProfileIntW (SECTNAME_KEYBOARDS,    // Ptr to the section name
                             KEYNAME_CHAR,          // Ptr to the key name
                             0,                     // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Get the index of the keyboard TabCtrl tab
    uKeybTCNum =
      GetPrivateProfileIntW (SECTNAME_KEYBOARDS,    // Ptr to the section name
                             KEYNAME_KEYBTCNUM,     // Ptr to the key name
                             10,                    // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Get the initial keyboard state (0=none
    //                                 1=           Alt
    //                                 2=      Ctrl
    //                                 3=      Ctrl+Alt
    //                                 4=Shift
    //                                 5=Shift     +Alt
    //                                 6=Shift+Ctrl
    //                                 7=Shift+Ctrl+Alt)
    uKeybState =
      GetPrivateProfileIntW (SECTNAME_KEYBOARDS,    // Ptr to the section name
                             KEYNAME_KEYBSTATE,     // Ptr to the key name
                             0,                     // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Get the default keyboard layout for the current thread
    // LANG_DANISH  == 0x06  SUBLANG_DANISH_DENMARK == 0x01
    // LANG_ENGLISH == 0x09  SUBLANG_ENGLISH_US     == 0x01
    // LANG_ENGLISH == 0x09  SUBLANG_ENGLISH_UK     == 0x02
    // LANG_FRENCH  == 0x0C  SUBLANG_FRENCH         -- 0x01
    hkl = LOWORD (GetKeyboardLayout (0));

    // Split cases based upon the primary language ID (LANG_xxx)
    switch (PRIMARYLANGID (hkl))
    {
        case LANG_DANISH:
            lpKeybLayout = KEYBLAYOUT_DK_CTL;

            break;

        case LANG_ENGLISH:
            // Split cases based upon the sublanguage ID (SUBLANG_xxx)
            switch (SUBLANGID (hkl))
            {
                case SUBLANG_ENGLISH_UK:
                    lpKeybLayout = KEYBLAYOUT_UK_ALT;

                    break;

                case SUBLANG_ENGLISH_US:
                    lpKeybLayout = KEYBLAYOUT_US_ALT;

                    break;

                default:
                    lpKeybLayout = KEYBLAYOUT_US_ALT;

                    break;
            } // End SWITCH

            break;

        case LANG_FRENCH:
            lpKeybLayout = KEYBLAYOUT_FR_ALT;

            break;

        default:
            lpKeybLayout = KEYBLAYOUT_US_ALT;

            break;
    } // End SWITCH

    // Get the active keyboard layout name
    GetPrivateProfileStringW (SECTNAME_KEYBOARDS,       // Ptr to the section name
                              KEYNAME_KEYBLAYOUTNAME,   // Ptr to the key name
                              lpKeybLayout,             // Ptr to the default value
                              wszGlbKeybLayoutAct,      // Ptr to the output buffer
                              countof (wszGlbKeybLayoutAct),// Count of the output buffer
                              lpwszIniFile);            // Ptr to the file name
    // Read in the LOGFONTW strucs
    GetPrivateProfileLogFontW (SECTNAME_KEYBOARDS, KEYNAME_LOGFONTKB, &lfKB);

    // Initialize the built-in keyboard CharCodes
    for (uCnt = 0; uCnt < uGlbKeybLayoutBI; uCnt++)
    {
        CopyMemory (lpKeybLayouts[uCnt].aCharCodes,
                    aKeybLayoutsBI[uCnt].lpCharCodes,
                    aKeybLayoutsBI[uCnt].uCharCodesLen * sizeof (CHARCODE));
        lpKeybLayouts[uCnt].uCharCodesLen          = aKeybLayoutsBI[uCnt].uCharCodesLen;
        strcpyW (lpKeybLayouts[uCnt].wszLayoutName, aKeybLayoutsBI[uCnt].wszLayoutName);
        lpKeybLayouts[uCnt].uScanCode2B_RowNum     = aKeybLayoutsBI[uCnt].uScanCode2B_RowNum;
        lpKeybLayouts[uCnt].bReadOnly              = TRUE;
        lpKeybLayouts[uCnt].bExtraKeyRow3          = aKeybLayoutsBI[uCnt].bExtraKeyRow3;
        lpKeybLayouts[uCnt].bUseCXV                = aKeybLayoutsBI[uCnt].bUseCXV;
        lpKeybLayouts[uCnt].bUseZY                 = aKeybLayoutsBI[uCnt].bUseZY;
        lpKeybLayouts[uCnt].bUseSEQ                = aKeybLayoutsBI[uCnt].bUseSEQ;
    } // End FOR

    // Read in the user-defined keyboard layouts
    for (uCnt = 0, uCnt2 = uGlbKeybLayoutBI;
         uCnt < uGlbKeybLayoutUser;
         uCnt++, uCnt2++)
    {
        WCHAR wszKeybChars[8 * 8 - 2 + 1];              // Room for a string formatted by FMTSTR_KEYBCHARS
        UINT wc[8],                                     // Save area for keyb chars as UINTs
             uLen;                                      // Length value

        // Format the section name
        MySprintfW (wszSectName,
                    sizeof (wszSectName),
                    SECTNAME_KEYBPREFIX L"%u",
                    uCnt);
        // Read in the layout name
        GetPrivateProfileStringW (wszSectName,              // Ptr to the section name
                                  KEYNAME_KEYBLAYOUTNAME,   // Ptr to the key name
                                  L"",                      // Ptr to the default value
                                  lpKeybLayouts[uCnt2].wszLayoutName, // Ptr to the output buffer
                                  KBLEN,                    // Count of the output buffer
                                  lpwszIniFile);            // Ptr to the file name
        // Read in the row # for scancode 2B
        lpKeybLayouts[uCnt2].uScanCode2B_RowNum =
          GetPrivateProfileIntW (wszSectName,               // Ptr to the section name
                                 KEYNAME_KEYBSCANCODE2B,    // Ptr to the key name
                                 0,                         // Default value if not found
                                 lpwszIniFile);             // Ptr to the file name
        // Read in the flag for extra key on row #3
        lpKeybLayouts[uCnt2].bExtraKeyRow3 =
          GetPrivateProfileIntW (wszSectName,               // Ptr to the section name
                                 KEYNAME_KEYBEXTRAKEYROW3,  // Ptr to the key name
                                 0,                         // Default value if not found
                                 lpwszIniFile);             // Ptr to the file name
        // Read in the flag for Ctrl-C, -X, -V
        lpKeybLayouts[uCnt2].bUseCXV =
          GetPrivateProfileIntW (wszSectName,               // Ptr to the section name
                                 KEYNAME_KEYBUSECXV,        // Ptr to the key name
                                 0,                         // Default value if not found
                                 lpwszIniFile);             // Ptr to the file name
        // Read in the flag for Ctrl-Z, -Y
        lpKeybLayouts[uCnt2].bUseZY =
          GetPrivateProfileIntW (wszSectName,               // Ptr to the section name
                                 KEYNAME_KEYBUSEZY,         // Ptr to the key name
                                 0,                         // Default value if not found
                                 lpwszIniFile);             // Ptr to the file name
        // Read in the flag for Ctrl-S, -E, -Q
        lpKeybLayouts[uCnt2].bUseSEQ =
          GetPrivateProfileIntW (wszSectName,               // Ptr to the section name
                                 KEYNAME_KEYBUSESEQ,        // Ptr to the key name
                                 0,                         // Default value if not found
                                 lpwszIniFile);             // Ptr to the file name
        // Read in the # scancodes in the layout
        uLen = lpKeybLayouts[uCnt2].uCharCodesLen =
          GetPrivateProfileIntW (wszSectName,               // Ptr to the section name
                                 KEYNAME_COUNT,             // Ptr to the key name
                                 0,                         // Default value if not found
                                 lpwszIniFile);             // Ptr to the file name
        // Loop through the scancodes
        for (uCol = 0; uCol < uLen; uCol++)
        {
            // Format the keyname
            MySprintfW (wszKeyName,
                        sizeof (wszKeyName),
                        KEYNAME_KEYBSCANCODE L"%02X",
                        uCol);
            // Read in the keyboard chars for this scancode
            GetPrivateProfileStringW (wszSectName,              // Ptr to the section name
                                      wszKeyName,               // Ptr to the key name
                                      L"",                      // Ptr to the default value
                                      wszKeybChars,             // Ptr to the output buffer
                                      countof (wszKeybChars),   // Count of the output buffer
                                      lpwszIniFile);            // Ptr to the file name
            // Convert the hex digits to integers
            sscanfW (wszKeybChars,
                     FMTSTR_KEYBCHARS,
                    &wc[0],
                    &wc[1],
                    &wc[2],
                    &wc[3],
                    &wc[4],
                    &wc[5],
                    &wc[6],
                    &wc[7]);
            // Save the values in aCharCodes
            lpKeybLayouts[uCnt2].aCharCodes[uCol].wc[0] = wc[0];
            lpKeybLayouts[uCnt2].aCharCodes[uCol].wc[1] = wc[1];
            lpKeybLayouts[uCnt2].aCharCodes[uCol].wc[2] = wc[2];
            lpKeybLayouts[uCnt2].aCharCodes[uCol].wc[3] = wc[3];
            lpKeybLayouts[uCnt2].aCharCodes[uCol].wc[4] = wc[4];
            lpKeybLayouts[uCnt2].aCharCodes[uCol].wc[5] = wc[5];
            lpKeybLayouts[uCnt2].aCharCodes[uCol].wc[6] = wc[6];
            lpKeybLayouts[uCnt2].aCharCodes[uCol].wc[7] = wc[7];
        } // End FOR
    } // End FOR

    // Accumulate into the total # keyboard layouts
    uGlbKeybLayoutCount = uGlbKeybLayoutBI + uGlbKeybLayoutUser;

    // Set the active keyboard layout #
    for (uGlbKeybLayoutNumAct = 0; uGlbKeybLayoutNumAct < uGlbKeybLayoutCount; uGlbKeybLayoutNumAct++)
    if (lstrcmpW (wszGlbKeybLayoutAct, lpKeybLayouts[uGlbKeybLayoutNumAct].wszLayoutName) EQ 0)
        break;

    Assert (uGlbKeybLayoutNumAct < uGlbKeybLayoutCount);
    if (uGlbKeybLayoutNumAct EQ uGlbKeybLayoutCount)
        uGlbKeybLayoutNumAct = 0;
    // Set the active keyboard layout
    aKeybLayoutAct = lpKeybLayouts[uGlbKeybLayoutNumAct];

    // Make that the visible keyboard layout name
    strcpyW (wszGlbKeybLayoutVis, wszGlbKeybLayoutAct);
    uGlbKeybLayoutNumVis = uGlbKeybLayoutNumAct;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbKeybLayouts); lpKeybLayouts = NULL;

#undef  TEMPBUFLEN

    return TRUE;
} // End ReadIniFileGlb
#undef  APPEND_NAME


//***************************************************************************
//  $DeleIniFileGlb
//
//  Delete the global vars created by <ReadIniFileGlb>
//***************************************************************************

void DeleIniFileGlb
    (void)

{
////FreeGlbName (hGlbQuadALX_CWS    ); hGlbQuadALX_CWS      = NULL; // Deleted in DelePermVars
////FreeGlbName (hGlbQuadELX_CWS    ); hGlbQuadELX_CWS      = NULL; // ...
//  FreeGlbName (hGlbQuadFC_CWS     ); hGlbQuadFC_CWS       = NULL;
//  FreeGlbName (hGlbQuadLX_CWS     ); hGlbQuadLX_CWS       = NULL;
//  FreeGlbName (hGlbQuadFEATURE_CWS); hGlbQuadFEATURE_CWS  = NULL;
//  FreeGlbName (hGlbQuadIC_CWS     ); hGlbQuadIC_CWS       = NULL;
    FreeGlbName (hGlbLibDirs        ); hGlbLibDirs          = NULL;
    FreeGlbName (hGlbRecentFiles    ); hGlbRecentFiles      = NULL;
} // End DeleIniFileGlb


//***************************************************************************
//  $RedoWinBG
//
//  Respecify the Window Background brush
//    because the Window Background color has changed
//***************************************************************************

void RedoWinBG
    (void)

{
    UINT uCnt;                      // Loop counter

    // Delete the window background brush
    if (ghBrushBG)
    {
        DeleteObject (ghBrushBG); ghBrushBG = NULL;
    } // End IF

    // Initialize the window background brush
    ghBrushBG = CreateSolidBrush (gSyntaxColorText.crBack);

    // Loop through the Syntax Colors
    for (uCnt = 0; uCnt < SC_LENGTH; uCnt++)
    // If the background is transparent, change it
    if (gSyntClrBGTrans[uCnt])
        gSyntaxColorName[uCnt].syntClr.crBack = gSyntaxColorText.crBack;
} // End RedoWinBG


//***************************************************************************
//  $GetPrivateProfileLogFontW
//
//  Get a LOGFONTW struc from a .ini file
//***************************************************************************

void GetPrivateProfileLogFontW
    (LPWCHAR    lpwSectName,                        // Ptr to the section name
     LPWCHAR    wszKeyName,                         // Ptr to the key name
     LPLOGFONTW lplfFont)                           // Ptr to the output buffer

{
    WCHAR wszTemp[1024];            // Temporary storage for string results

    // Read in the LOGFONTW struc as a string
    GetPrivateProfileStringW (lpwSectName,          // Ptr to the section name
                              wszKeyName,           // Ptr to the key name
                              L"",                  // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              countof (wszTemp) - 1,// Count of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // If the new value is present, ...
    if (wszTemp[0] NE WC_EOS)
    {
        int lfTmp[8];

        // Because many of the fields in a LOGFONT struc are BYTE wide,
        //   we use the following artifice of temporary INT wide fields
        //   so that sscanfW doesn't write into date beyond the BYTE.
        sscanfW (wszTemp,
                 FMTSTR_LOGFONT_INP,
                &lplfFont->lfHeight,
                &lplfFont->lfWidth,
                &lplfFont->lfEscapement,
                &lplfFont->lfOrientation,
                &lplfFont->lfWeight,
                &lfTmp[0],                  // &lplfFont->lfItalic,
                &lfTmp[1],                  // &lplfFont->lfUnderline,
                &lfTmp[2],                  // &lplfFont->lfStrikeOut,
                &lfTmp[3],                  // &lplfFont->lfCharSet,
                &lfTmp[4],                  // &lplfFont->lfOutPrecision,
                &lfTmp[5],                  // &lplfFont->lfClipPrecision,
                &lfTmp[6],                  // &lplfFont->lfQuality,
                &lfTmp[7],                  // &lplfFont->lfPitchAndFamily,
                &lplfFont->lfFaceName);
        // Copy the temp fields into the byte fields in lplfFont
        lplfFont->lfItalic          = (BYTE) lfTmp[0];
        lplfFont->lfUnderline       = (BYTE) lfTmp[1];
        lplfFont->lfStrikeOut       = (BYTE) lfTmp[2];
        lplfFont->lfCharSet         = (BYTE) lfTmp[3];
        lplfFont->lfOutPrecision    = (BYTE) lfTmp[4];
        lplfFont->lfClipPrecision   = (BYTE) lfTmp[5];
        lplfFont->lfQuality         = (BYTE) lfTmp[6];
        lplfFont->lfPitchAndFamily  = (BYTE) lfTmp[7];

        // If the facename contains an embedded blank, sscanfW misses
        //   the tail of the name so we do it over again here
        strcpyW (lplfFont->lfFaceName, strchrW (wszTemp, WC_SQ) + 1);
        *(strchrW (lplfFont->lfFaceName, WC_SQ)) = WC_EOS;
    } // End IF
} // End GetPrivateProfileLogFontW


//***************************************************************************
//  $GetPrivateProfileFloatW
//
//  Read in a float value from a .ini file
//***************************************************************************

APLFLOAT GetPrivateProfileFloatW
    (LPWCHAR  lpwSectName,                          // Ptr to the section name
     LPWCHAR  lpwKeyName,                           // Ptr to the key name
     APLFLOAT fDefVal,                              // Ptr to the default value
     LPWCHAR  lpwszIniFile)                         // Ptr to the file name

{
    WCHAR wszTemp[1024];                            // Temporary storage for string results
    char  szTemp[1024];                             // ...

    // Read in the floating point value as a string
    GetPrivateProfileStringW (lpwSectName,          // Ptr to the section name
                              lpwKeyName,           // Ptr to the key name
                              L"",                  // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              countof (wszTemp),    // Count of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // If the new value is present, ...
    if (wszTemp[0])
    {
        // Convert wide to single-byte so we can use MyStrtod
        //   which has no wide counterpart
        W2A (szTemp, wszTemp, sizeof (wszTemp));

        // Use David Gay's routines
        return MyStrtod (szTemp, NULL);
    } else
        return fDefVal;
} // End GetPrivateProfileFloatW


//***************************************************************************
//  $GetPrivateProfileGlbCharW
//
//  Read in a global char vector from a .ini file
//***************************************************************************

HGLOBAL GetPrivateProfileGlbCharW
    (LPWCHAR  lpwSectName,                          // Ptr to the section name
     LPWCHAR  lpwKeyName,                           // Ptr to the key name
     LPWCHAR  lpwDefVal,                            // Ptr to the default value
     HGLOBAL  hDefVal,                              // Default value global memory handle
     LPWCHAR  lpwszIniFile)                         // Ptr to the file name

{
    return
    GetPrivateProfileGlbComW (lpwSectName,          // Ptr to the section name
                              lpwKeyName,           // Ptr to the key name
                              lpwDefVal,            // Ptr to the default value
                              hDefVal,              // Default value global memory handle
                              ARRAY_CHAR,           // Result storage type
                              sizeof (APLCHAR),     // Size of each item in the result
                              lstrlenW (lpwDefVal), // Length of the default vector
                              lpwszIniFile);        // Ptr to the file name
} // End GetPrivateProfileGlbCharW


//***************************************************************************
//  $GetPrivateProfileGlbIntW
//
//  Read in a global integer vector from a .ini file
//***************************************************************************

HGLOBAL GetPrivateProfileGlbIntW
    (LPWCHAR  lpwSectName,                          // Ptr to the section name
     LPWCHAR  lpwKeyName,                           // Ptr to the key name
     LPWCHAR  lpwDefVal,                            // Ptr to the default value
     HGLOBAL  hDefVal,                              // Default value global memory handle
     UINT     uVecLen,                              // Length of the default integer vector
     LPWCHAR  lpwszIniFile)                         // Ptr to the file name

{
    return
    GetPrivateProfileGlbComW (lpwSectName,          // Ptr to the section name
                              lpwKeyName,           // Ptr to the key name
                              lpwDefVal,            // Ptr to the default value
                              hDefVal,              // Default value global memory handle
                              ARRAY_INT,            // Result storage type
                              sizeof (APLINT),      // Size of each item in the result
                              uVecLen,              // Length of the default vector
                              lpwszIniFile);        // Ptr to the file name
} // End GetPrivateProfileGlbIntW


//***************************************************************************
//  $GetPrivateProfileGlbComW
//
//  Read in a global integer or char vector from a .ini file
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- GetPrivateProfileGlbComW"
#else
#define APPEND_NAME
#endif

HGLOBAL GetPrivateProfileGlbComW
    (LPWCHAR  lpwSectName,                          // Ptr to the section name
     LPWCHAR  lpwKeyName,                           // Ptr to the key name
     LPWCHAR  lpDefVal,                             // Ptr to the default values
     HGLOBAL  hDefVal,                              // Default value global memory handle
     APLSTYPE aplTypeRes,                           // Result storage type
     UINT     aplSizeRes,                           // Size of each item in the result
     UINT     uVecLen,                              // Length of the default vector
     LPWCHAR  lpwszIniFile)                         // Ptr to the file name

{
    WCHAR             wszTemp[1024];                // Temporary storage for string results
    APLNELM           aplNELMRes,                   // Result NELM
                      aplNELMInp;                   // Input NELM
    APLUINT           ByteRes;                      // # bytes in the result
    HGLOBAL           hGlbRes,                      // Result global memory handle
                      hGlbChk;                      // Result from CheckGlobals
    LPVARARRAY_HEADER lpMemHdrRes = NULL;           // Ptr to result header
    LPVOID            lpMemRes,                     // Ptr to result global memory
                      lpMemInp;                     // Ptr to input global memory

    // Read in the global integer or char vector as a string
    GetPrivateProfileStringW (lpwSectName,          // Ptr to the section name
                              lpwKeyName,           // Ptr to the key name
                              L"\x0001",            // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              countof (wszTemp),    // Count of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // If the new value is present, ...
    if (wszTemp[0] NE L'\x0001')
    {
        // Catch short default values of []IC where we inserted a
        //   new value into the middle of the integer vector
        if (lstrcmpW (lpwKeyName, KEYNAME_QUADIC) EQ 0
         && (lstrcmpW (wszTemp, L"3 4 2 2 2 1 2 2 1 2 2 2 1")   EQ 0
          || lstrcmpW (wszTemp, L"3 4 2 2 2 1 2 2 1 2 2 2 2 1") EQ 0))
            // Use the new default value
            strcpyW (wszTemp, L"3 4 2 2 2 1 2 2 2 2 1 2 2 2 2 1");
        // Use the given (or substituted value)
        lpMemInp = wszTemp;
    } else
        // Use the default value
        lpMemInp = lpDefVal;

    // Find out how many elements are in the .ini file value
    aplNELMInp = ScanNELM (lpMemInp, aplTypeRes);
    aplNELMRes = max (aplNELMInp, uVecLen);

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, 1);

    // Allocate space for the data
    // Note, we can't use DbgGlobalAlloc here as the
    //   PTD has not been allocated as yet
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        return hGlbRes;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader    lpMemHdrRes
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Save the dimension
    *VarArrayBaseToDim (lpMemHdrRes) = aplNELMRes;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Copy and convert the values to the result
    CopyConvertDataOfType (lpMemRes,                                // Ptr to result global memory
                           0,                                       // Initial index into lpMemRes
                           aplTypeRes,                              // Result storage type
                           aplNELMInp,                              // Result NELM
                           lpMemInp,                                // Ptr to the input buffer
                           FALSE);                                  // TRUE iff the input buffer is integer
    // If the saved value is shorter than the default value, ...
    if (aplNELMInp < aplNELMRes)
    {
        LPVARARRAY_HEADER lpMemHdrDef = NULL;   // Ptr to default header
        LPVOID            lpMemDef;             // Ptr to default values

        // Lock the memory to get a ptr to it
        lpMemHdrDef = MyGlobalLockVar (hDefVal);

        // Skip over the header & dimensions to the data
        lpMemDef = VarArrayDataFmBase (lpMemHdrDef);

        // Copy and convert the values to the result
        CopyConvertDataOfType (lpMemRes,                            // Ptr to result global memory
                               aplNELMInp,                          // Initial index into lpMemRes
                               aplTypeRes,                          // Result storage type
                               aplNELMRes - aplNELMInp,             // Result NELM
                               lpMemDef,                            // Ptr to the input buffer
                               lpMemHdrDef->ArrType EQ ARRAY_INT);  // TRUE iff the input buffer is integer
        // We no longer need this ptr
        MyGlobalUnlock (hDefVal); lpMemHdrDef = NULL;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

    // Check to see if this value duplicates one of the already
    //   allocated permanent globals
    hGlbChk = CheckGlobals (hGlbRes);
    if (hGlbChk)
    {
        // We no longer need this storage
        FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;

        // Copy the global memory handle
        hGlbRes = CopySymGlbDirAsGlb (hGlbChk);
    } // End IF

    return hGlbRes;
} // End GetPrivateProfileGlbComW
#undef  APPEND_NAME


//***************************************************************************
//  $CopyConvertDataOfType
//
//  Copy and convert data of a given type to another location
//***************************************************************************

void CopyConvertDataOfType
    (LPVOID   lpMemRes,                             // Ptr to result global memory
     APLNELM  aplNELMIni,                           // Initial index into lpMemRes
     APLSTYPE aplTypeRes,                           // Result storage type
     APLNELM  aplNELMRes,                           // Result NELM
     LPWCHAR  lpwszTemp,                            // Ptr to the input buffer
     UBOOL    bInpInteger)                          // TRUE iff the input buffer is integer

{
    LPAPLINT  lpaplInteger;                         // Ptr to integer input
    APLUINT   uCnt;                                 // Loop counter
    WCHAR     wcTmp;                                // Temporary char

    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        case ARRAY_INT:
            lpaplInteger = (LPAPLINT) lpwszTemp;

            // Loop through the result elements
            for (uCnt = 0; uCnt < aplNELMRes; uCnt++)
            {
                // If the input buffer is integer, ...
                if (bInpInteger)
                {
                    ((LPAPLINT) lpMemRes)[aplNELMIni] = lpaplInteger[uCnt];
                } else
                {
                    // Convert the next #
                    sscanfW (lpwszTemp,
                             L"%I64d",
                            &((LPAPLINT) lpMemRes)[aplNELMIni]);
                    // Skip to the next field
                    lpwszTemp = SkipPastCharW (lpwszTemp, L' ');
                } // End IF/ELSE

                // Skip to next result location
                aplNELMIni++;
            } // End FOR

            break;

        case ARRAY_CHAR:
            // Loop through the result elements
            for (uCnt = 0; uCnt < aplNELMRes; uCnt++)
            {
                // Convert the single {name} or other char to UTF16_xxx
                if (L'{' EQ  *lpwszTemp)
                {
                    // Get the next char
                    wcTmp = SymbolNameToChar (lpwszTemp);

#define ZEROSTR     L"{\\x0000}"

                    // If there's a matching UTF16_xxx equivalent,
                    //   or the hex value is zero
                    if (wcTmp || strncmpW (lpwszTemp, ZEROSTR, strcountof (ZEROSTR)) EQ 0)
                    {
                        // Save in the result and skip over it
                        ((LPAPLCHAR) lpMemRes)[aplNELMIni++] = wcTmp;

                        // Skip to the next field
                        lpwszTemp = SkipPastCharW (lpwszTemp, L'}');
                    } else
                    {
                        // Copy source to destin up to and including the matching '}'
                        while (lpwszTemp[0] NE L'}')
                            // Save in the result and skip over it
                            ((LPAPLCHAR) lpMemRes)[aplNELMIni++] = *lpwszTemp++;

                        // Copy the '}'
                        ((LPAPLCHAR) lpMemRes)[aplNELMIni++] = *lpwszTemp++;
                    } // End IF/ELSE
#undef  ZEROSTR
                } else
                    // Save in the result and skip over it
                    ((LPAPLCHAR) lpMemRes)[aplNELMIni++] = *lpwszTemp++;
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH
} // End CopyConvertDataOfType


//***************************************************************************
//  $ScanNELM
//
//  Scan a string and return the # elements in it
//***************************************************************************

APLNELM ScanNELM
    (LPWCHAR  wszTemp,
     APLSTYPE aplTypeRes)

{
    APLNELM aplNELMRes;             // Result NELM
    APLINT  aplIntTemp;             // Temporary integer for sscanfW

    // Initialize the result NELM
    aplNELMRes = 0;

    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        case ARRAY_INT:
            // Loop through the arg counting one integer at a time
            while (*wszTemp)
            {
                if (EOF EQ sscanfW (wszTemp,
                                    L"%I64d",
                                   &aplIntTemp))
                    break;
                aplNELMRes++;
                wszTemp = SkipPastCharW (wszTemp, L' ');
            } // End WHILE

            return aplNELMRes;

        case ARRAY_CHAR:
            // Loop through the arg counting {name} as one char
            while (*wszTemp)
            {
                if (*wszTemp EQ L'{')
                    // Skip over the name
                    wszTemp = SkipPastCharW (wszTemp, L'}');
                else
                    // Skip to next char
                    wszTemp++;

                // Count in another char
                aplNELMRes++;
            } // End WHILE

            return aplNELMRes;

        defstop
            return (APLNELM) -1;
    } // End SWITCH
} // End ScanNELM


//***************************************************************************
//  $ReadIniFileWnd
//
//  Read in window-specific .ini file settings
//***************************************************************************

void ReadIniFileWnd
    (HWND hWnd)

{
    RECT  rcDtop;           // Rectangle for desktop
    POINT PosCtr;           // x- and y- positions
    WCHAR wszTemp[128];     // Temporary buffer
    UINT  uTmp[3];          // Temp UINTs for sscanfW

    // Read in the values from the [General] section

    // Setup the default values in case the .INI file is not present
    GetWindowRect (hWnd, &rcDtop);
    MFSize.cx = rcDtop.right  - rcDtop.left;
    MFSize.cy = rcDtop.bottom - rcDtop.top;

    // Save the window size for use in WM_MOVE messages
    // Note that this is the window size, not the client area size.
    MFSize.cx =
      GetPrivateProfileIntW (SECTNAME_GENERAL,      // Ptr to the section name
                             KEYNAME_XSIZE,         // Ptr to the key name
                             MFSize.cx,             // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    MFSize.cy =
      GetPrivateProfileIntW (SECTNAME_GENERAL,      // Ptr to the section name
                             KEYNAME_YSIZE,         // Ptr to the key name
                             MFSize.cy,             // Default value if not found
                             lpwszIniFile);         // Ptr to the file name

    // Setup the default values in case the .INI file is not present
    GetWindowRect (GetDesktopWindow(), &rcDtop);
    PosCtr.x = rcDtop.left + MFSize.cx / 2; // Center horizontally
    PosCtr.y = rcDtop.top  + MFSize.cy / 2; // Center vertically

    MFPosCtr.x =
      GetPrivateProfileIntW (SECTNAME_GENERAL,      // Ptr to the section name
                             KEYNAME_XPOS,          // Ptr to the key name
                             PosCtr.x,              // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    MFPosCtr.y =
      GetPrivateProfileIntW (SECTNAME_GENERAL,      // Ptr to the section name
                             KEYNAME_YPOS,          // Ptr to the key name
                             PosCtr.y,              // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    MFSizeState =
      GetPrivateProfileIntW (SECTNAME_GENERAL,      // Ptr to the section name
                             KEYNAME_SIZESTATE,     // Ptr to the key name
                             MFSizeState,           // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in the initial customize category
    gInitCustomizeCategory =
      GetPrivateProfileIntW (SECTNAME_GENERAL,      // Ptr to the section name
                             KEYNAME_INITIALCAT,    // Ptr to the key name
                             DEF_INIT_CATEGORY,     // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in the update frequency
    GetPrivateProfileStringW (SECTNAME_GENERAL,     // Ptr to the section name
                              KEYNAME_UPDFRQ,       // Ptr to the key name
                              DEF_UPDFRQ_STR,       // Ptr to the default value
                              gszUpdFrq,            // Ptr to the output buffer
                              countof (gszUpdFrq),  // Count of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // Save the corresponding index
    for (guUpdFrq = 0; guUpdFrq < countof (updFrq); guUpdFrq++)
    if (lstrcmpW (gszUpdFrq, updFrq[guUpdFrq].lpwsz) EQ 0)
        break;
    // Check for error
    if (guUpdFrq EQ countof (updFrq))
    {
        // Use the defaults
        strcpyW (gszUpdFrq, DEF_UPDFRQ_STR);
        guUpdFrq = DEF_UPDFRQ_NDX;
    } // End IF

    // Read in the update check date
    GetPrivateProfileStringW (SECTNAME_GENERAL,     // Ptr to the section name
                              KEYNAME_UPDCHK,       // Ptr to the key name
                              DEF_UPDCHK,           // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              countof (wszTemp),    // Count of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // Convert the date to SYSTEMTIME format
    sscanfW (wszTemp,
             FMTSTR_UPDCHK,
            &uTmp[0],           // &gstUpdChk.wYear,
            &uTmp[1],           // &gstUpdChk.wMonth,
            &uTmp[2]);          // &gstUpdChk.wDay,
    // Copy the temps to the SYSTEMTIME struc
    gstUpdChk.wYear     = (WORD)uTmp[0];
    gstUpdChk.wMonth    = (WORD)uTmp[1];
    gstUpdChk.wDay      = (WORD)uTmp[2];
} // End ReadIniFileWnd


//***************************************************************************
//  $SaveIniFile
//
//  Save window-specific values into an .ini file
//***************************************************************************

void SaveIniFile
    (void)

{
    WCHAR         wszTemp[1024],                    // Temporary storage
                  wszKey[8 + 1];                    // ...
    UINT          uCnt,                             // Loop counter
                  uCnt2,                            // ...
                  uCol,                             // ...
                  uLen,                             // Length value
                  uVal;                             // Temp var
    LPVOID        lpMemObj;                         // Ptr to object global memory
    LPAPLCHAR     lpaplChar;                        // Ptr to output save area
    APLNELM       aplNELMObj;                       // Object NELM
    WCHAR       (*lpwszRecentFiles)[][_MAX_PATH];   // Ptr to list of recent files
    LPKEYBLAYOUTS lpKeybLayouts;                    // Ptr to keyboard layouts global memory
    LPWSZLIBDIRS  lpwszLibDirs;                     // Ptr to LibDirs
    UINT          uTmp[3];                          // Temp UINTs for sscanfW

    //*********************************************************
    // Write out [LibDirs] section entries
    //*********************************************************

    // Format the # LibDirs
    MySprintfW (wszKey,
                sizeof (wszKey),
               L"%u",
                uNumLibDirs);
    // Write it out
    WritePrivateProfileStringW (SECTNAME_LIBDIRS,           // Ptr to the section name
                                KEYNAME_COUNT,              // Ptr to the key name
                                wszKey,                     // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    // Lock the memory to get a ptr to it
    lpwszLibDirs = MyGlobalLock (hGlbLibDirs);

    // Loop through the LibDirs
    for (uCnt = 0; uCnt < uNumLibDirs; uCnt++)
    {
        // Format the keyname
        MySprintfW (wszKey,
                    sizeof (wszKey),
                   L"%u",
                    uCnt);
        // Write it out
        WritePrivateProfileStringW (SECTNAME_LIBDIRS,       // Ptr to the section name
                                    wszKey,                 // Ptr to the key name
                                    lpwszLibDirs[uCnt],     // Ptr to the key value
                                    lpwszIniFile);          // Ptr to the file name
    } // End FOR

    // We no longer need this ptr
    MyGlobalUnlock (hGlbLibDirs); lpwszLibDirs = NULL;

    //*********************************************************
    // Write out [General] section entries
    //*********************************************************

    // Write out the version #
    WritePrivateProfileStringW (SECTNAME_GENERAL,           // Ptr to the section name
                                KEYNAME_VERSION,            // Ptr to the key name
                                L"0.01",                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    // Format the x-position
    MySprintfW (wszTemp,
                sizeof (wszTemp),
               L"%d",
                MFPosCtr.x);
    // Write out the x-position
    WritePrivateProfileStringW (SECTNAME_GENERAL,           // Ptr to the section name
                                KEYNAME_XPOS,               // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    // Format the x-size
    MySprintfW (wszTemp,
                sizeof (wszTemp),
               L"%d",
                MFSize.cx);
    // Write out the x-size
    WritePrivateProfileStringW (SECTNAME_GENERAL,           // Ptr to the section name
                                KEYNAME_XSIZE,              // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    // Format the y-position
    MySprintfW (wszTemp,
                sizeof (wszTemp),
               L"%d",
                MFPosCtr.y);
    // Write out the y-position
    WritePrivateProfileStringW (SECTNAME_GENERAL,           // Ptr to the section name
                                KEYNAME_YPOS,               // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    // Format the y-size
    MySprintfW (wszTemp,
                sizeof (wszTemp),
               L"%d",
                MFSize.cy);
    // Write out the y-size
    WritePrivateProfileStringW (SECTNAME_GENERAL,           // Ptr to the section name
                                KEYNAME_YSIZE,              // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    // Format the size state
    MySprintfW (wszTemp,
                sizeof (wszTemp),
               L"%d",
                MFSizeState);
    // Write out the size state
    WritePrivateProfileStringW (SECTNAME_GENERAL,           // Ptr to the section name
                                KEYNAME_SIZESTATE,          // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    // Format the initial category
    MySprintfW (wszTemp,
                sizeof (wszTemp),
               L"%d",
                gInitCustomizeCategory);
    // Write out the initial category
    WritePrivateProfileStringW (SECTNAME_GENERAL,           // Ptr to the section name
                                KEYNAME_INITIALCAT,         // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    // Write out the update frequency
    WritePrivateProfileStringW (SECTNAME_GENERAL,           // Ptr to the section name
                                KEYNAME_UPDFRQ,             // Ptr to the key name
                                gszUpdFrq,                  // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    // Copy the SYSTEMTIME struc to the temps
    (WORD)uTmp[0]    = gstUpdChk.wYear ;
    (WORD)uTmp[1]    = gstUpdChk.wMonth;
    (WORD)uTmp[2]    = gstUpdChk.wDay  ;

    // Format the update check date
    MySprintfW (wszTemp,
                sizeof (wszTemp),
                FMTSTR_UPDCHK,
                uTmp[0],
                uTmp[1],
                uTmp[2]);
    // Write out the update check date
    WritePrivateProfileStringW (SECTNAME_GENERAL,           // Ptr to the section name
                                KEYNAME_UPDCHK,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name

    //*********************************************************
    // Write out [Migration] section entries
    //*********************************************************

    //******************* IC0LOG0 *****************************
    wszTemp[0] = L'0' + uMigration.IC0LOG0;
    wszTemp[1] = WC_EOS;

    // Write out IC0LOG0
    WritePrivateProfileStringW (SECTNAME_MIGRATION,         // Ptr to the section name
                                KEYNAME_IC0LOG0,            // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name

    //*********************************************************
    // Write out [Fonts] section entries
    //*********************************************************

    // Write out the LOGFONTW struc for FE
    WritePrivateProfileLogfontW (SECTNAME_FONTS,            // Ptr to the section name
                                 KEYNAME_LOGFONTFE,         // Ptr to the key name
                                &lfFE,                      // Ptr to LOGFONTW
                                 lpwszIniFile);             // Ptr to the file name
    // Write out the LOGFONTW struc for PR
    WritePrivateProfileLogfontW (SECTNAME_FONTS,            // Ptr to the section name
                                 KEYNAME_LOGFONTPR,         // Ptr to the key name
                                &lfPR,                      // Ptr to LOGFONTW
                                 lpwszIniFile);             // Ptr to the file name
    // Write out the LOGFONTW struc for SM
    WritePrivateProfileLogfontW (SECTNAME_FONTS,            // Ptr to the section name
                                 KEYNAME_LOGFONTSM,         // Ptr to the key name
                                &lfSM,                      // Ptr to LOGFONTW
                                 lpwszIniFile);             // Ptr to the file name
    // Write out the LOGFONTW struc for TC
    WritePrivateProfileLogfontW (SECTNAME_FONTS,            // Ptr to the section name
                                 KEYNAME_LOGFONTTC,         // Ptr to the key name
                                &lfTC,                      // Ptr to LOGFONTW
                                 lpwszIniFile);             // Ptr to the file name
    //*********************************************************
    // Write out [SameFontAs] section entries
    //*********************************************************

    // Loop through the items of glbSameFontAs
    for (uCnt = 0; uCnt < FONTENUM_LENGTH; uCnt++)
    {
        // Format the keyname
        MySprintfW (wszKey,
                    sizeof (wszKey),
                   L"%u",
                    uCnt);
        // Format the glbSameFontAs value
        MySprintfW (wszTemp,
                    sizeof (wszTemp),
                   L"%u",
                    glbSameFontAs[uCnt]);
        WritePrivateProfileStringW (SECTNAME_SAMEFONTAS,    // Ptr to the section name
                                    wszKey,                 // Ptr to the key name
                                    wszTemp,                // Ptr to the SameFontAs value
                                    lpwszIniFile);          // Ptr to the file name
    } // End FOR

    //*********************************************************
    // Write out [Options] section entries
    //*********************************************************

    //******************* bAdjustPW ***************************
    // Format bAdjustPw
    wszTemp[0] = L'0' + OptionFlags.bAdjustPW;
    wszTemp[1] = WC_EOS;

    // Write out bAdjustPw
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_ADJUSTPW,           // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bUnderbarToLowercase ****************
    // Format bUnderbarToLowercase
    wszTemp[0] = L'0' + OptionFlags.bUnderbarToLowercase;
    wszTemp[1] = WC_EOS;

    // Write out bUnderbarToLowercase
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_UNDERBARTOLOWERCASE,// Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bNewTabOnClear **********************
    // Format bNewTabOnClear
    wszTemp[0] = L'0' + OptionFlags.bNewTabOnClear;
    wszTemp[1] = WC_EOS;

    // Write out bNewTabOnClear
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_NEWTABONCLEAR,      // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bNewTabOnLoad ***********************
    // Format bNewTabOnLoad
    wszTemp[0] = L'0' + OptionFlags.bNewTabOnLoad;
    wszTemp[1] = WC_EOS;

    // Write out bNewTabOnLoad
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_NEWTABONLOAD,       // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bUseLocalTime ***********************
    // Format bUseLocalTime
    wszTemp[0] = L'0' + OptionFlags.bUseLocalTime;
    wszTemp[1] = WC_EOS;

    // Write out bUseLocalTime
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_USELOCALTIME,       // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bBackupOnLoad ***********************
    // Format bBackupOnLoad
    wszTemp[0] = L'0' + OptionFlags.bBackupOnLoad;
    wszTemp[1] = WC_EOS;

    // Write out bBackupOnLoad
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_BACKUPONLOAD,       // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bBackupOnSave ***********************
    // Format bBackupOnSave
    wszTemp[0] = L'0' + OptionFlags.bBackupOnSave;
    wszTemp[1] = WC_EOS;

    // Write out bBackupOnSave
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_BACKUPONSAVE,       // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bNoCopyrightMsg *********************
    // Format bNoCopyrightMsg
    wszTemp[0] = L'0' + OptionFlags.bNoCopyrightMsg;
    wszTemp[1] = WC_EOS;

    // Write out bNoCopyrightMsg
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_NOCOPYRIGHTMSG,     // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* uDefaultPaste ***********************
    // Format uDefaultPaste
    MySprintfW (wszTemp,
                sizeof (wszTemp),
               L"%u",
                OptionFlags.uDefaultPaste);
    // Write out uDefaultPaste
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_DEFAULTPASTE,       // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* uDefaultCopy ************************
    // Format uDefaultCopy
    MySprintfW (wszTemp,
                sizeof (wszTemp),
               L"%u",
                OptionFlags.uDefaultCopy);
    // Write out uDefaultCopy
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_DEFAULTCOPY,        // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bSyntClrFcns ************************
    // Format bSyntClrFcns
    wszTemp[0] = L'0' + OptionFlags.bSyntClrFcns;
    wszTemp[1] = WC_EOS;

    // Write out bSyntClrFcns
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_SYNTCLRFCNS,        // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bSyntClrSess ************************
    // Format bSyntClrSess
    wszTemp[0] = L'0' + OptionFlags.bSyntClrSess;
    wszTemp[1] = WC_EOS;

    // Write out bSyntClrSess
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_SYNTCLRSESS,        // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bSyntClrPrnt ************************
    // Format bSyntClrPrnt
    wszTemp[0] = L'0' + OptionFlags.bSyntClrPrnt;
    wszTemp[1] = WC_EOS;

    // Write out bSyntClrPrnt
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_SYNTCLRPRNT,        // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bCheckGroup *************************
    // Format bCheckGroup
    wszTemp[0] = L'0' + OptionFlags.bCheckGroup;
    wszTemp[1] = WC_EOS;

    // Write out bCheckGroup
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_CHECKGROUP,         // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bInsState ***************************
    // Format bInsState
    wszTemp[0] = L'0' + OptionFlags.bInsState;
    wszTemp[1] = WC_EOS;

    // Write out bInsState
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_INSSTATE,           // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name

    //******************* bRevDblClk **************************
    // Format bRevDblClk
    wszTemp[0] = L'0' + OptionFlags.bRevDblClk;
    wszTemp[1] = WC_EOS;

    // Write out bRevDblClk
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_REVDBLCLK,          // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name

    //******************* bViewStatusBar **********************
    // Format bViewStatusBar
    wszTemp[0] = L'0' + OptionFlags.bViewStatusBar;
    wszTemp[1] = WC_EOS;

    // Write out bViewStatusBar
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_VIEWSTATUSBAR,      // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name

    //******************* bDefDispFcnLineNums *****************
    // Format bDefDispFcnLineNums
    wszTemp[0] = L'0' + OptionFlags.bDefDispFcnLineNums;
    wszTemp[1] = WC_EOS;

    // Write out bDefDispFcnLineNums
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_DEFDISPFCNLINENUMS, // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name

    //******************* bDispMPSuf **************************
    // Format bDispMPSuf
    wszTemp[0] = L'0' + OptionFlags.bDispMPSuf;
    wszTemp[1] = WC_EOS;

    // Write out bDispMPSuf
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_DISPMPSUF,          // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bOutputDebug ************************
    // Format bOutputDebug
    wszTemp[0] = L'0' + OptionFlags.bOutputDebug;
    wszTemp[1] = WC_EOS;

    // Write out bOutputDebug
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_OUTPUTDEBUG,        // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* uUserChar ***************************
    // Format the Line Continuation marker
    MySprintfW (wszTemp,
                sizeof (wszTemp),
               L"%u",
                uUserChar);
    // Write it out
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_CHAR,               // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* uUserUnibase ************************
    // Format the Line Continuation Unicode base
    MySprintfW (wszTemp,
                sizeof (wszTemp),
               L"%u",
                uUserUnibase);
    // Write it out
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_UNIBASE,            // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name

    //*********************************************************
    // Write out [SysVars] section entries
    //*********************************************************

    //************************ []ALX **************************
    // Write out []ALX
    WritePrivateProfileGlbCharW (SECTNAME_SYSVARS,
                                 KEYNAME_QUADALX,
                                 hGlbQuadALX_CWS,
                                 lpwszIniFile);
    //************************ []CT ***************************
    // Format []CT
    lpaplChar =
     FormatAplFltFC (wszTemp,                               // Ptr to output save area
                     fQuadCT_CWS,                           // The value to format
                     DEF_MAX_QUADPP_IEEE,                   // Precision to use
                     L'.',                                  // Char to use as decimal separator
                     L'-',                                  // Char to use as overbar
                     FLTDISPFMT_RAWFLT,                     // Float display format
                     FALSE);                                // TRUE iff we're to substitute text for infinity
    // Zap the trailing blank
    lpaplChar[-1] = WC_EOS;

    // Write out []CT
    WritePrivateProfileStringW (SECTNAME_SYSVARS,           // Ptr to the section name
                                KEYNAME_QUADCT,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //************************ []DT ***************************
    // Format []DT
    wszTemp[0] = WC_SQ;
    uLen = (UINT)
      ConvertWideToNameLength (&wszTemp[1], &cQuadDT_CWS, 1);
    wszTemp[uLen + 1] = WC_SQ;
    wszTemp[uLen + 2] = WC_EOS;

    // Write out []DT
    WritePrivateProfileStringW (SECTNAME_SYSVARS,           // Ptr to the section name
                                KEYNAME_QUADDT,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //************************ []ELX **************************
    // Write out []ELX
    WritePrivateProfileGlbCharW (SECTNAME_SYSVARS,
                                 KEYNAME_QUADELX,
                                 hGlbQuadELX_CWS,
                                 lpwszIniFile);
    //************************ []FC ***************************
    // Write out []FC
    WritePrivateProfileGlbCharW (SECTNAME_SYSVARS,
                                 KEYNAME_QUADFC,
                                 hGlbQuadFC_CWS,
                                 lpwszIniFile);
    //************************ []FEATURE **********************
    // Lock the memory to get a ptr to it
    lpMemObj = MyGlobalLockVar (hGlbQuadFEATURE_CWS);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemObj)
    // Get the # bytes
    aplNELMObj = lpHeader->NELM;
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMemObj = VarArrayDataFmBase (lpMemObj);

    // Format []FEATURE
    lpaplChar = wszTemp;
    while (aplNELMObj--)
        lpaplChar +=
          wsprintfW (lpaplChar,
                     L"%d ",
                    *((LPAPLINT) lpMemObj)++);
    // Zap the trailing blank
    lpaplChar[-1] = WC_EOS;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbQuadFEATURE_CWS); lpMemObj = NULL;

    // Write out []FEATURE
    WritePrivateProfileStringW (SECTNAME_SYSVARS,           // Ptr to the section name
                                KEYNAME_QUADFEATURE,        // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //************************ []FPC **************************
    // Format []FPC
    lpaplChar =
      FormatAplIntFC (wszTemp,                              // Ptr to output save area
                      uQuadFPC_CWS,                         // The value to format
                      L'-');                                // Char to use as overbar
    // Zap the trailing blank
    lpaplChar[-1] = WC_EOS;

    // Write out []FPC
    WritePrivateProfileStringW (SECTNAME_SYSVARS,           // Ptr to the section name
                                KEYNAME_QUADFPC,            // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //************************ []IC ***************************
    // Lock the memory to get a ptr to it
    lpMemObj = MyGlobalLockVar (hGlbQuadIC_CWS);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemObj)
    // Get the # bytes
    aplNELMObj = lpHeader->NELM;
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMemObj = VarArrayDataFmBase (lpMemObj);

    // Format []IC
    lpaplChar = wszTemp;
    while (aplNELMObj--)
        lpaplChar +=
          wsprintfW (lpaplChar,
                     L"%d ",
                    *((LPAPLINT) lpMemObj)++);
    // Zap the trailing blank
    lpaplChar[-1] = WC_EOS;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbQuadIC_CWS); lpMemObj = NULL;

    // Write out []IC
    WritePrivateProfileStringW (SECTNAME_SYSVARS,           // Ptr to the section name
                                KEYNAME_QUADIC,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //************************ []IO ***************************
    // Format []IO
    lpaplChar =
      FormatAplIntFC (wszTemp,                              // Ptr to output save area
                      bQuadIO_CWS,                          // The value to format
                      L'-');                                // Char to use as overbar
    // Zap the trailing blank
    lpaplChar[-1] = WC_EOS;

    // Write out []IO
    WritePrivateProfileStringW (SECTNAME_SYSVARS,           // Ptr to the section name
                                KEYNAME_QUADIO,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //************************ []LX ***************************
    // Write out []LX
    WritePrivateProfileGlbCharW (SECTNAME_SYSVARS,
                                 KEYNAME_QUADLX,
                                 hGlbQuadLX_CWS,
                                 lpwszIniFile);
    //************************ []MF ***************************
    // Format []MF
    lpaplChar =
      FormatAplIntFC (wszTemp,                              // Ptr to output save area
                      uQuadMF_CWS,                          // The value to format
                      L'-');                                // Char to use as overbar
    // Zap the trailing blank
    lpaplChar[-1] = WC_EOS;

    // Write out []MF
    WritePrivateProfileStringW (SECTNAME_SYSVARS,           // Ptr to the section name
                                KEYNAME_QUADMF,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //************************ []PP ***************************
    // Format []PP
    lpaplChar =
      FormatAplIntFC (wszTemp,                              // Ptr to output save area
                      uQuadPP_CWS,                          // The value to format
                      L'-');                                // Char to use as overbar
    // Zap the trailing blank
    lpaplChar[-1] = WC_EOS;

    // Write out []PP
    WritePrivateProfileStringW (SECTNAME_SYSVARS,           // Ptr to the section name
                                KEYNAME_QUADPP,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //************************ []PR ***************************
    // Format []PR
    if (cQuadPR_CWS EQ CQUADPR_MT)
    {
        wszTemp[0] =
        wszTemp[1] = WC_SQ;
        wszTemp[2] = WC_EOS;
    } else
    {
        wszTemp[0] = WC_SQ;
        uLen = (UINT)
          ConvertWideToNameLength (&wszTemp[1], &cQuadPR_CWS, 1);
        wszTemp[uLen + 1] = WC_SQ;
        wszTemp[uLen + 2] = WC_EOS;
    } // End IF/ELSE

    // Write out []PR
    WritePrivateProfileStringW (SECTNAME_SYSVARS,           // Ptr to the section name
                                KEYNAME_QUADPR,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //************************ []PW ***************************
    // Format []PW
    lpaplChar =
      FormatAplIntFC (wszTemp,                              // Ptr to output save area
                      uQuadPW_CWS,                          // The value to format
                      L'-');                                // Char to use as overbar
    // Zap the trailing blank
    lpaplChar[-1] = WC_EOS;

    // Write out []PW
    WritePrivateProfileStringW (SECTNAME_SYSVARS,           // Ptr to the section name
                                KEYNAME_QUADPW,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //************************ []RL ***************************
    // Format []RL
    lpaplChar =
      FormatAplIntFC (wszTemp,                              // Ptr to output save area
                      uQuadRL_CWS,                          // The value to format
                      L'-');                                // Char to use as overbar
    // Zap the trailing blank
    lpaplChar[-1] = WC_EOS;

    // Write out []RL
    WritePrivateProfileStringW (SECTNAME_SYSVARS,           // Ptr to the section name
                                KEYNAME_QUADRL,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //************************ []SA ***************************
    // Format []SA index
    wszTemp[0] = L'0' + cQuadxSA_CWS;
    wszTemp[1] = WC_EOS;

    // Write out []SA index
    WritePrivateProfileStringW (SECTNAME_SYSVARS,           // Ptr to the section name
                                KEYNAME_QUADSA,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //*********************************************************
    // Write out [RangeLimits] section entries
    //*********************************************************

    //******************* bRangeLimit.CT **********************
    // Format bRangeLimit.CT
    wszTemp[0] = L'0' + bRangeLimit.CT;
    wszTemp[1] = WC_EOS;

    // Write out bRangeLimit.CT
    WritePrivateProfileStringW (SECTNAME_RANGELIMITS,       // Ptr to the section name
                                KEYNAME_QUADCT,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bRangeLimit.FEATURE *****************
    // Format bRangeLimit.FEATURE
    wszTemp[0] = L'0' + bRangeLimit.FEATURE;
    wszTemp[1] = WC_EOS;

    // Write out bRangeLimit.FEATURE
    WritePrivateProfileStringW (SECTNAME_RANGELIMITS,       // Ptr to the section name
                                KEYNAME_QUADFEATURE,        // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bRangeLimit.FPC *********************
    // Format bRangeLimit.FPC
    wszTemp[0] = L'0' + bRangeLimit.FPC;
    wszTemp[1] = WC_EOS;

    // Write out bRangeLimit.FPC
    WritePrivateProfileStringW (SECTNAME_RANGELIMITS,       // Ptr to the section name
                                KEYNAME_QUADFPC,            // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bRangeLimit.IC **********************
    // Format bRangeLimit.IC
    wszTemp[0] = L'0' + bRangeLimit.IC;
    wszTemp[1] = WC_EOS;

    // Write out bRangeLimit.IC
    WritePrivateProfileStringW (SECTNAME_RANGELIMITS,       // Ptr to the section name
                                KEYNAME_QUADIC,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bRangeLimit.IO **********************
    // Format bRangeLimit.IO
    wszTemp[0] = L'0' + bRangeLimit.IO;
    wszTemp[1] = WC_EOS;

    // Write out bRangeLimit.IO
    WritePrivateProfileStringW (SECTNAME_RANGELIMITS,       // Ptr to the section name
                                KEYNAME_QUADIO,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bRangeLimit.PP **********************
    // Format bRangeLimit.PP
    wszTemp[0] = L'0' + bRangeLimit.PP;
    wszTemp[1] = WC_EOS;

    // Write out bRangeLimit.PP
    WritePrivateProfileStringW (SECTNAME_RANGELIMITS,       // Ptr to the section name
                                KEYNAME_QUADPP,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bRangeLimit.PW **********************
    // Format bRangeLimit.PW
    wszTemp[0] = L'0' + bRangeLimit.PW;
    wszTemp[1] = WC_EOS;

    // Write out bRangeLimit.PW
    WritePrivateProfileStringW (SECTNAME_RANGELIMITS,       // Ptr to the section name
                                KEYNAME_QUADPW,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bRangeLimit.RL **********************
    // Format bRangeLimit.RL
    wszTemp[0] = L'0' + bRangeLimit.RL;
    wszTemp[1] = WC_EOS;

    // Write out bRangeLimit.RL
    WritePrivateProfileStringW (SECTNAME_RANGELIMITS,       // Ptr to the section name
                                KEYNAME_QUADRL,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //*********************************************************
    // Write out [ResetVars] section entries
    //*********************************************************

    //****************** bResetVars.CT ************************
    // Format bResetVars.CT
    wszTemp[0] = L'0' + bResetVars.CT;
    wszTemp[1] = WC_EOS;

    // Write out bResetVars.CT
    WritePrivateProfileStringW (SECTNAME_RESETVARS,         // Ptr to the section name
                                KEYNAME_QUADCT,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** bResetVars.DT ************************
    // Format bResetVars.DT
    wszTemp[0] = L'0' + bResetVars.DT;
    wszTemp[1] = WC_EOS;

    // Write out bResetVars.DT
    WritePrivateProfileStringW (SECTNAME_RESETVARS,         // Ptr to the section name
                                KEYNAME_QUADDT,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** bResetVars.FC ************************
    // Format bResetVars.FC
    wszTemp[0] = L'0' + bResetVars.FC;
    wszTemp[1] = WC_EOS;

    // Write out bResetVars.FC
    WritePrivateProfileStringW (SECTNAME_RESETVARS,         // Ptr to the section name
                                KEYNAME_QUADFC,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** bResetVars.FEATURE *******************
    // Format bResetVars.FEATURE
    wszTemp[0] = L'0' + bResetVars.FEATURE;
    wszTemp[1] = WC_EOS;

    // Write out bResetVars.FEATURE
    WritePrivateProfileStringW (SECTNAME_RESETVARS,         // Ptr to the section name
                                KEYNAME_QUADFEATURE,        // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** bResetVars.FPC ***********************
    // Format bResetVars.FPC
    wszTemp[0] = L'0' + bResetVars.FPC;
    wszTemp[1] = WC_EOS;

    // Write out bResetVars.FPC
    WritePrivateProfileStringW (SECTNAME_RESETVARS,         // Ptr to the section name
                                KEYNAME_QUADFPC,            // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** bResetVars.IC ************************
    // Format bResetVars.IC
    wszTemp[0] = L'0' + bResetVars.IC;
    wszTemp[1] = WC_EOS;

    // Write out bResetVars.IC
    WritePrivateProfileStringW (SECTNAME_RESETVARS,         // Ptr to the section name
                                KEYNAME_QUADIC,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** bResetVars.IO ************************
    // Format bResetVars.IO
    wszTemp[0] = L'0' + bResetVars.IO;
    wszTemp[1] = WC_EOS;

    // Write out bResetVars.IO
    WritePrivateProfileStringW (SECTNAME_RESETVARS,         // Ptr to the section name
                                KEYNAME_QUADIO,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** bResetVars.PP ************************
    // Format bResetVars.PP
    wszTemp[0] = L'0' + bResetVars.PP;
    wszTemp[1] = WC_EOS;

    // Write out bResetVars.PP
    WritePrivateProfileStringW (SECTNAME_RESETVARS,         // Ptr to the section name
                                KEYNAME_QUADPP,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** bResetVars.PW ************************
    // Format bResetVars.PW
    wszTemp[0] = L'0' + bResetVars.PW;
    wszTemp[1] = WC_EOS;

    // Write out bResetVars.PW
    WritePrivateProfileStringW (SECTNAME_RESETVARS,         // Ptr to the section name
                                KEYNAME_QUADPW,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** bResetVars.RL ************************
    // Format bResetVars.RL
    wszTemp[0] = L'0' + bResetVars.RL;
    wszTemp[1] = WC_EOS;

    // Write out bResetVars.RL
    WritePrivateProfileStringW (SECTNAME_RESETVARS,         // Ptr to the section name
                                KEYNAME_QUADRL,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //*********************************************************
    // Write out [Colors] section entries
    //*********************************************************

    // Loop through the color names
    for (uCnt = 0; uCnt < SC_LENGTH; uCnt++)
    {
        // Format the color name value
         MySprintfW (wszTemp,
                     sizeof (wszTemp),
                     FMTSTR_SYNTAXCOLOR,
                     gSyntaxColorName[uCnt].syntClr.crFore,
                     gSyntaxColorName[uCnt].syntClr.crBack,
                     gSyntClrBGTrans [uCnt]);
        // Write out the entry
        WritePrivateProfileStringW (SECTNAME_COLORS,            // Ptr to the section name
                                    aColorKeyNames[uCnt],       // Ptr to the key name
                                    wszTemp,                    // Ptr to the key value
                                    lpwszIniFile);              // Ptr to the file name
    } // End FOR

    // Write out the CustomColors
     MySprintfW (wszTemp,
                 sizeof (wszTemp),
                 FMTSTR_CUSTOMCOLORS,
                 aCustomColors[ 0], aCustomColors[ 1], aCustomColors[ 2], aCustomColors[ 3],
                 aCustomColors[ 4], aCustomColors[ 5], aCustomColors[ 6], aCustomColors[ 7],
                 aCustomColors[ 8], aCustomColors[ 9], aCustomColors[10], aCustomColors[11],
                 aCustomColors[12], aCustomColors[13], aCustomColors[14], aCustomColors[15]);
    // Write out the entry
    WritePrivateProfileStringW (SECTNAME_COLORS,            // Ptr to the section name
                                KEYNAME_CUSTOMCOLORS,       // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //*********************************************************
    // Write out [Toolbars] section entries
    //*********************************************************

    // Loop through the Toolbars
    for (uCnt = 0; uCnt < countof (aRebarBands); uCnt++)
    {
        // Format .bShowBand
        wszTemp[0] = L'0' + aRebarBands[uCnt].bShowBand;
        wszTemp[1] = WC_EOS;

        // Write out the entry
        WritePrivateProfileStringW (SECTNAME_TOOLBARS,          // Ptr to the section name
                                    aToolbarNames[uCnt],        // Ptr to the key name
                                    wszTemp,                    // Ptr to the key value
                                    lpwszIniFile);              // Ptr to the file name
    } // End FOR

    //*********************************************************
    // Write out [RecentFiles] section entries
    //*********************************************************

    // Lock the memory to get a ptr to it
    lpwszRecentFiles = MyGlobalLock (hGlbRecentFiles);

    // Loop through the Recent Files
    for (uVal = uCnt = 0; uCnt < uNumRecentFiles; uCnt++)
    if ((*lpwszRecentFiles)[uCnt][0])
    {
        // Count in another valid Recent File
        uVal++;

        // Format the keyname
        MySprintfW (wszKey,
                    sizeof (wszKey),
                   L"%u",
                    uCnt);
        // Write out the current Recent File
        WritePrivateProfileStringW (SECTNAME_RECENTFILES,       // Ptr to the section name
                                    wszKey,                     // Ptr to the key name
                                  (*lpwszRecentFiles)[uCnt],    // Ptr to the key value
                                    lpwszIniFile);              // Ptr to the file name
    } // End FOR/IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRecentFiles); lpwszRecentFiles = NULL;

    // Format the # valid Recent Files
    MySprintfW (wszKey,
                sizeof (wszKey),
               L"%u",
                uVal);
    // Write it out
    WritePrivateProfileStringW (SECTNAME_RECENTFILES,       // Ptr to the section name
                                KEYNAME_COUNT,              // Ptr to the key name
                                wszKey,                     // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //*********************************************************
    // Write out [Keyboards] section entries
    //*********************************************************

    // Format the # user-defined keyboards
    MySprintfW (wszKey,
                sizeof (wszKey),
               L"%u",
                uGlbKeybLayoutUser);
    // Write it out
    WritePrivateProfileStringW (SECTNAME_KEYBOARDS,         // Ptr to the section name
                                KEYNAME_COUNT,              // Ptr to the key name
                                wszKey,                     // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    // Format the keyboard Unicode base
    MySprintfW (wszKey,
                sizeof (wszKey),
               L"%u",
                uKeybUnibase);
    // Write it out
    WritePrivateProfileStringW (SECTNAME_KEYBOARDS,         // Ptr to the section name
                                KEYNAME_UNIBASE,            // Ptr to the key name
                                wszKey,                     // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    // Format the last keyboard unicode char
    MySprintfW (wszKey,
                sizeof (wszKey),
               L"%u",
                uKeybChar);
    // Write it out
    WritePrivateProfileStringW (SECTNAME_KEYBOARDS,         // Ptr to the section name
                                KEYNAME_CHAR,               // Ptr to the key name
                                wszKey,                     // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    // Format the keyboard TabCtrl tab index
    MySprintfW (wszKey,
                sizeof (wszKey),
               L"%u",
                uKeybTCNum);
    // Write it out
    WritePrivateProfileStringW (SECTNAME_KEYBOARDS,         // Ptr to the section name
                                KEYNAME_KEYBTCNUM,          // Ptr to the key name
                                wszKey,                     // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    // Format the keyboard state
    MySprintfW (wszKey,
                sizeof (wszKey),
               L"%u",
                uKeybState);
    // Write it out
    WritePrivateProfileStringW (SECTNAME_KEYBOARDS,         // Ptr to the section name
                                KEYNAME_KEYBSTATE,          // Ptr to the key name
                                wszKey,                     // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    // Write out the active keyboard layout name
    WritePrivateProfileStringW (SECTNAME_KEYBOARDS,         // Ptr to the section name
                                KEYNAME_KEYBLAYOUTNAME,     // Ptr to the key name
                                wszGlbKeybLayoutAct,        // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    // Write out the LOGFONTW struc for KB
    WritePrivateProfileLogfontW (SECTNAME_KEYBOARDS,        // Ptr to the section name
                                 KEYNAME_LOGFONTKB,         // Ptr to the key name
                                &lfKB,                      // Ptr to LOGFONTW
                                 lpwszIniFile);             // Ptr to the file name
    // Lock the memory to get a ptr to it
    lpKeybLayouts = MyGlobalLock (hGlbKeybLayouts);

    // Loop through the user-defined keyboard layouts
    for (uCnt = 0, uCnt2 = uGlbKeybLayoutBI;
         uCnt < uGlbKeybLayoutUser;
         uCnt++, uCnt2++)
    {
        WCHAR wszKeybChars[8 * 8 - 2 + 1],              // Room for a string formatted by FMTSTR_KEYBCHARS
              wszCount[8 + 1];                          // Scancode count

        // Format the section name
        MySprintfW (wszSectName,
                    sizeof (wszSectName),
                    SECTNAME_KEYBPREFIX L"%u",
                    uCnt);
        // Write out the layout name
        WritePrivateProfileStringW (wszSectName,                // Ptr to the section name
                                    KEYNAME_KEYBLAYOUTNAME,     // Ptr to the key name
                                    lpKeybLayouts[uCnt2].wszLayoutName, // Ptr to the key value
                                    lpwszIniFile);              // Ptr to the file name
        // Format the row # for scancode 2B
        wszTemp[0] = L'0' + lpKeybLayouts[uCnt2].uScanCode2B_RowNum;
        wszTemp[1] = WC_EOS;

        // Write out the row # for scancode 2B
        WritePrivateProfileStringW (wszSectName,                // Ptr to the section name
                                    KEYNAME_KEYBSCANCODE2B,     // Ptr to the key name
                                    wszTemp,                    // Ptr to the key value
                                    lpwszIniFile);              // Ptr to the file name
        // Format the flag for extra key on row #3
        wszTemp[0] = L'0' + lpKeybLayouts[uCnt2].bExtraKeyRow3;
        wszTemp[1] = WC_EOS;

        // Write out the flag for extra key on row #3
        WritePrivateProfileStringW (wszSectName,                // Ptr to the section name
                                    KEYNAME_KEYBEXTRAKEYROW3,   // Ptr to the key name
                                    wszTemp,                    // Ptr to the key value
                                    lpwszIniFile);              // Ptr to the file name
        // Format the flag for Ctrl-C, -X, -V
        wszTemp[0] = L'0' + lpKeybLayouts[uCnt2].bUseCXV;
        wszTemp[1] = WC_EOS;

        // Write out the flag for Ctrl-C, -X, -V
        WritePrivateProfileStringW (wszSectName,                // Ptr to the section name
                                    KEYNAME_KEYBUSECXV,         // Ptr to the key name
                                    wszTemp,                    // Ptr to the key value
                                    lpwszIniFile);              // Ptr to the file name
        // Format the flag for Ctrl-Z, -Y
        wszTemp[0] = L'0' + lpKeybLayouts[uCnt2].bUseZY;
        wszTemp[1] = WC_EOS;

        // Write out the flag for Ctrl-Z, -Y
        WritePrivateProfileStringW (wszSectName,                // Ptr to the section name
                                    KEYNAME_KEYBUSEZY,          // Ptr to the key name
                                    wszTemp,                    // Ptr to the key value
                                    lpwszIniFile);              // Ptr to the file name
        // Format the flag for Ctrl-S, -E, -Q
        wszTemp[0] = L'0' + lpKeybLayouts[uCnt2].bUseSEQ;
        wszTemp[1] = WC_EOS;

        // Write out the flag for Ctrl-S, -E, -Q
        WritePrivateProfileStringW (wszSectName,                // Ptr to the section name
                                    KEYNAME_KEYBUSESEQ,         // Ptr to the key name
                                    wszTemp,                    // Ptr to the key value
                                    lpwszIniFile);              // Ptr to the file name
        // Get the # scancodes in this layout
        uLen = lpKeybLayouts[uCnt2].uCharCodesLen;

        // Format the # scancodes in this layout
        MySprintfW (wszCount,
                    sizeof (wszCount),
                   L"%u",
                    uLen);
        // Write out the # scancodes in this layout
        WritePrivateProfileStringW (wszSectName,                // Ptr to the section name
                                    KEYNAME_COUNT,              // Ptr to the key name
                                    wszCount,                   // Ptr to the key value
                                    lpwszIniFile);              // Ptr to the file name
        // Loop through the scancodes
        for (uCol = 0; uCol < uLen; uCol++)
        {
            // Format the keyname
            MySprintfW (wszKeyName,
                        sizeof (wszKeyName),
                        KEYNAME_KEYBSCANCODE L"%02X",
                        uCol);
            // Format the keyboard chars for this scancode
            MySprintfW (wszKeybChars,
                        sizeof (wszKeybChars),
                        FMTSTR_KEYBCHARS,
                        lpKeybLayouts[uCnt2].aCharCodes[uCol].wc[0],
                        lpKeybLayouts[uCnt2].aCharCodes[uCol].wc[1],
                        lpKeybLayouts[uCnt2].aCharCodes[uCol].wc[2],
                        lpKeybLayouts[uCnt2].aCharCodes[uCol].wc[3],
                        lpKeybLayouts[uCnt2].aCharCodes[uCol].wc[4],
                        lpKeybLayouts[uCnt2].aCharCodes[uCol].wc[5],
                        lpKeybLayouts[uCnt2].aCharCodes[uCol].wc[6],
                        lpKeybLayouts[uCnt2].aCharCodes[uCol].wc[7]);
            // Write out the keyb chars for this scancode
            WritePrivateProfileStringW (wszSectName,                // Ptr to the section name
                                        wszKeyName,                 // Ptr to the key name
                                        wszKeybChars,               // Ptr to the key value
                                        lpwszIniFile);              // Ptr to the file name
        } // End FOR
#undef  SECTNAME
    } // End FOR

    // Delete keyboard layouts above the last user-defined layout
    while (TRUE)
    {
        WCHAR wszLayoutName[KBLEN];                         // Space for a temporary layout name

        // Format the section name
        MySprintfW (wszSectName,
                    sizeof (wszSectName),
                    SECTNAME_KEYBPREFIX L"%u",
                    uCnt);
        // Read in the layout name
        GetPrivateProfileStringW (wszSectName,              // Ptr to the section name
                                  KEYNAME_KEYBLAYOUTNAME,   // Ptr to the key name
                                  L"",                      // Ptr to the default value
                                  wszLayoutName,            // Ptr to the output buffer
                                  KBLEN,                    // Count of the output buffer
                                  lpwszIniFile);            // Ptr to the file name
        // If the layout name is non-empty, ...
        if (wszLayoutName[0])
        {
            // Delete the section
            WritePrivateProfileSectionW (wszSectName,       // Ptr to the section name
                                         NULL,              // Delete the entire section
                                         lpwszIniFile);     // Ptr to the file name
            // Skip to the next potential keyboard layout
            uCnt++;
        } else
            break;
    } // End WHILE

    // We no longer need this ptr
    MyGlobalUnlock (hGlbKeybLayouts); lpKeybLayouts = NULL;
} // End SaveIniFile


//***************************************************************************
//  $WritePrivateProfileLogfontW
//
//  Write out a LOGFONTW struc to a .ini file
//***************************************************************************

void WritePrivateProfileLogfontW
    (LPWCHAR    lpwSectName,                // Ptr to the section name
     LPWCHAR    lpwKeyName,                 // Ptr to the key name
     LPLOGFONTW lplfFont,                   // Ptr to LOGFONTW
     LPWCHAR    lpwszIniFile)               // Ptr to file name

{
    WCHAR wszTemp[1024];                            // Temporary storage

    // Format the LOGFONTW struc entries
    MySprintfW (wszTemp,
                sizeof (wszTemp),
                FMTSTR_LOGFONT_OUT,
                lplfFont->lfHeight,
                lplfFont->lfWidth,
                lplfFont->lfEscapement,
                lplfFont->lfOrientation,
                lplfFont->lfWeight,
                lplfFont->lfItalic,
                lplfFont->lfUnderline,
                lplfFont->lfStrikeOut,
                lplfFont->lfCharSet,
                lplfFont->lfOutPrecision,
                lplfFont->lfClipPrecision,
                lplfFont->lfQuality,
                lplfFont->lfPitchAndFamily,
               &lplfFont->lfFaceName);
    // Write out the LOGFONTW struc
    WritePrivateProfileStringW (lpwSectName,        // Ptr to the section name
                                lpwKeyName,         // Ptr to the key name
                                wszTemp,            // Ptr to the key value
                                lpwszIniFile);      // Ptr to the file name
} // End WritePrivateProfileLogfontW


//***************************************************************************
//  $WritePrivateProfileGlbCharW
//
//  Write out a global char string to a .ini file
//***************************************************************************

void WritePrivateProfileGlbCharW
    (LPWCHAR lpwSectName,                   // Ptr to the section name
     LPWCHAR lpwKeyName,                    // Ptr to the key name
     HGLOBAL hGlbObj,                       // Object global memory handle
     LPWCHAR lpwszIniFile)                  // Ptr to file name

{
    LPAPLCHAR lpMemObj,                     // Ptr to object global memory
              lpaplChar;                    // Ptr to running output
    APLNELM   aplNELMObj;                   // Object NELM

    // Lock the memory to get a ptr to it
    lpMemObj = MyGlobalLockVar (hGlbObj);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemObj)
    // Get the NELM
    aplNELMObj = lpHeader->NELM;
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMemObj = VarArrayDataFmBase (lpMemObj);

    // Get starting ptr
    lpaplChar = lpwszGlbTemp;

    // Leading single quote
    *lpaplChar++ = WC_SQ;

    // Format the text as an ASCII string with non-ASCII chars
    //   represented as either {symbol} or {\xXXXX} where XXXX is
    //   a four-digit hex number.
    lpaplChar +=
      ConvertWideToNameLength (lpaplChar,       // Ptr to output save buffer
                               lpMemObj,        // Ptr to incoming chars
                    (APLU3264) aplNELMObj);     // # chars to convert
    // Trailing single quote
    *lpaplChar++ = WC_SQ;

    // Ensure properly terminated
    *lpaplChar   = WC_EOS;

    // Write out the global char vector
    WritePrivateProfileStringW (lpwSectName,        // Ptr to the section name
                                lpwKeyName,         // Ptr to the key name
                                lpwszGlbTemp,       // Ptr to the key value
                                lpwszIniFile);      // Ptr to the file name
    // We no longer need this ptr
    MyGlobalUnlock (hGlbObj); lpMemObj = NULL;
} // End WritePrivateProfileGlbCharW


//***************************************************************************
//  $ProfileInit_EM
//
//  Initialize a .ini profile
//***************************************************************************

LPDICTIONARY ProfileInit_EM
    (LPWSTR       lpwszDPFE,        // Ptr to workspace DPFE
     LPWCHAR     *lplpwErrMsg)      // Ptr to ptr to error message text

{
    LPDICTIONARY lpDict;            // Ptr to workspace dictionary

    // Initialize the parser dictionary
    lpDict = iniparser_init (lpwszDPFE);
    if (lpDict EQ NULL)
        *lplpwErrMsg = L"No room for Dictionary";
    else
        *lplpwErrMsg = NULL;

    return lpDict;
} // End ProfileInit_EM


//***************************************************************************
//  $ProfileLoad_EM
//
//  Load a .ini profile
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ProfileLoad_EM"
#else
#define APPEND_NAME
#endif

LPDICTIONARY ProfileLoad_EM
    (LPWSTR       lpwszDPFE,        // Ptr to workspace DPFE
     LPWCHAR     *lplpwErrMsg)      // Ptr to ptr to error message text

{
    FILE        *fStream;           // File ptr
    ERRCODES     errCode;           // Return error code
    UINT         lineno;            // Line # in error
    LPDICTIONARY lpDict;            // Ptr to workspace dictionary

    // Attempt to open the file for reading
    fStream = fopenW (lpwszDPFE, L"r");

    // If the file doesn't exist, ...
    if (fStream EQ NULL)
        return NULL;

    __try
    {
        // Load the private .ini file
        lpDict =
          iniparser_load (fStream, lpwszDPFE, &errCode, &lineno);

        // Close the file
        fclose (fStream);
    } __except (CheckVirtAlloc (GetExceptionInformation (),
                                L"ProfileLoad"))
    {
        // Display message for unhandled exception
        DisplayException ();
    } // End __try/__except

    // Check for error
    if (lpDict EQ NULL)
    {
        static WCHAR wszTemp[1024];

        // Point to where we're storing the error message text
        *lplpwErrMsg = &wszTemp[0];

        // Split cases based upon the error code
        switch (errCode)
        {
            case ERRCODE_BUFFER_OVERFLOW:
                MySprintfW (wszTemp,
                            sizeof (wszTemp),
                            ERRMSG_WS_NOT_LOADABLE
                           L":  Buffer overflow" APPEND_NAME);
                break;

            case ERRCODE_SYNTAX_ERROR:
                MySprintfW (wszTemp,
                            sizeof (wszTemp),
                            ERRMSG_WS_NOT_LOADABLE
                           L":  Syntax error in line %u" APPEND_NAME,
                            lineno);
                break;

            case ERRCODE_ALLOC_ERROR:
                MySprintfW (wszTemp,
                            sizeof (wszTemp),
                            ERRMSG_WS_NOT_LOADABLE
                           L":  Allocation in line %u" APPEND_NAME,
                            lineno);
                break;

            defstop
                break;
        } // End SWITCH
    } // End IF

    return lpDict;
} // End ProfileLoad_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ProfileGetString
//
//  Retrieve a string value from a .ini profile
//***************************************************************************

LPWSTR ProfileGetString
    (LPWSTR       lpwAppName,   // Section name containing the key name
     LPWSTR       lpwKeyName,   // Key name whose associated string is to be retrieved
     LPWSTR       lpwDefault,   // Ptr to default result if lpwKeyName not found
     LPDICTIONARY lpDict)       // Ptr to workspace dictionary

{
    WCHAR wszTemp[1024];        // Save area for longest "sectionname:keyname"

    // Merge the section and key names
    MySprintfW (wszTemp,
                sizeof (wszTemp),
               L"%s" SECTION_SEP_STR L"%s",
                lpwAppName,
                lpwKeyName);
    return iniparser_getstring (lpDict, wszTemp, lpwDefault, NULL);
} // End ProfileGetString


//***************************************************************************
//  $ProfileGetStringEx
//
//  Retrieve a string value from a .ini profile
//***************************************************************************

LPWSTR ProfileGetStringEx
    (LPWSTR       lpwAppName,   // Section name containing the key name
     LPWSTR       lpwKeyName,   // Key name whose associated string is to be retrieved
     LPWSTR       lpwDefault,   // Ptr to default result if lpwKeyName not found
     LPINT        lpIndex,      // Ptr to index on output (may be NULL)
     LPDICTIONARY lpDict)       // Ptr to workspace dictionary

{
    WCHAR wszTemp[1024];        // Save area for longest "sectionname:keyname"

    // Merge the section and key names
    MySprintfW (wszTemp,
                sizeof (wszTemp),
               L"%s" SECTION_SEP_STR L"%s",
                lpwAppName,
                lpwKeyName);
    return iniparser_getstring (lpDict, wszTemp, lpwDefault, lpIndex);
} // End ProfileGetStringEx


//***************************************************************************
//  $ProfileCopyString
//
//  Copy a string value from a .ini profile into an external buffer
//***************************************************************************

UBOOL ProfileCopyString
    (LPWSTR       lpwAppName,   // Section name containing the key name
     LPWSTR       lpwKeyName,   // Key name whose associated string is to be retrieved
     LPWSTR       lpwDefault,   // Ptr to default result if lpwKeyName not found
     LPWSTR       lpwBuffer,    // Ptr to output buffer
     DWORD        nSize,        // # bytes in the output buffer
     LPDICTIONARY lpDict)       // Ptr to workspace dictionary

{
    WCHAR  wszTemp[1024];       // Save area for longest "sectionname:keyname"
    LPWSTR lpwOutput;           // Ptr to string in memory

    // Merge the section and key names
    MySprintfW (wszTemp,
                sizeof (wszTemp),
               L"%s" SECTION_SEP_STR L"%s",
                lpwAppName,
                lpwKeyName);
    lpwOutput = iniparser_getstring (lpDict, wszTemp, lpwDefault, NULL);

    // If the section:keyname was found, ...
    if (lpwOutput NE NULL)
        // Copy the string to the output buffer
        return SUCCEEDED (StringCchCopyW (lpwBuffer,
                                          nSize / sizeof (WCHAR),
                                          lpwOutput));
    else
        return FALSE;
} // End ProfileCopyString


//***************************************************************************
//  $ProfileSetString
//
//  Set a string value into a .ini profile
//***************************************************************************

int ProfileSetString
    (LPWSTR       lpwAppName,   // Section name containing the key name
     LPWSTR       lpwKeyName,   // Key name whose associated string is to be set
     LPWSTR       lpwVal,       // Ptr to new value to associate with the Section:Keyname
     LPDICTIONARY lpDict)       // Ptr to workspace dictionary

{
    WCHAR wszTemp[1024];        // Save area for longest "sectionname:keyname"

#ifdef DEBUG
    // If the Keyname is present, ...
    if (lpwKeyName NE NULL)
    {
        int nSecs, i;

        // Get the # sections
        nSecs = iniparser_getnsec (lpDict);

        // Ensure the section name is present
        for (i = 0; i < nSecs; i++)
        if (lstrcmpiW (lpwAppName, iniparser_getsecname (lpDict, i)) EQ 0)
            break;
        // If we didn't terminate early (i.e., section not found), ...
        if (i EQ nSecs)
            DbgStop ();
    } // End IF
#endif
    // If the keyname is not NULL, ...
    if (lpwKeyName NE NULL)
        // Merge the section and key names
        MySprintfW (wszTemp,
                    sizeof (wszTemp),
                   L"%s" SECTION_SEP_STR L"%s",
                    lpwAppName,
                    lpwKeyName);
    else
        // Format the section name
        MySprintfW (wszTemp,
                    sizeof (wszTemp),
                   L"%s",
                    lpwAppName);
    return iniparser_set (lpDict, wszTemp, lpwVal);
} // End ProfileSetString


//***************************************************************************
//  $ProfileSetSection
//
//  Create a section in a .ini profile
//***************************************************************************

int ProfileSetSection
    (LPWSTR       lpwAppName,   // Section name
     LPDICTIONARY lpDict)       // Ptr to workspace dictionary

{
    return iniparser_set (lpDict, lpwAppName, NULL);
} // End ProfileSetSection


//***************************************************************************
//  $ProfileGetInt
//
//  Retrieve an integer value from a .ini profile
//***************************************************************************

int ProfileGetInt
    (LPWSTR       lpwAppName,   // Section name containing the key name
     LPWSTR       lpwKeyName,   // Key name whose associated string is to be retrieved
     int          iDefault,     // Default result if lpwKeyName not found
     LPDICTIONARY lpDict)       // Ptr to workspace dictionary

{
    WCHAR wszTemp[1024];        // Save area for longest "sectionname:keyname"

    // Merge the section and key names
    MySprintfW (wszTemp,
                sizeof (wszTemp),
               L"%s" SECTION_SEP_STR L"%s",
                lpwAppName,
                lpwKeyName);
    // Get the integer
    return iniparser_getint (lpDict, wszTemp, iDefault);
} // End ProfileGetInt


//***************************************************************************
//  $ProfileGetBoolean
//
//  Retrieve a Boolean value from a .ini profile
//***************************************************************************

UBOOL ProfileGetBoolean
    (LPWSTR       lpwAppName,   // Section name containing the key name
     LPWSTR       lpwKeyName,   // Key name whose associated string is to be retrieved
     UBOOL        bDefault,     // Default result if lpwKeyName not found
     LPDICTIONARY lpDict)       // Ptr to workspace dictionary

{
    WCHAR wszTemp[1024];        // Save area for longest "sectionname:keyname"

    // Merge the section and key names
    MySprintfW (wszTemp,
                sizeof (wszTemp),
               L"%s" SECTION_SEP_STR L"%s",
                lpwAppName,
                lpwKeyName);
    // Get the integer
    return iniparser_getboolean (lpDict, wszTemp, bDefault);
} // End ProfileGetBoolean


//***************************************************************************
//  $ProfileGetDouble
//
//  Retrieve a floating point value from a .ini profile
//***************************************************************************

APLFLOAT ProfileGetDouble
    (LPWSTR       lpwAppName,   // Section name containing the key name
     LPWSTR       lpwKeyName,   // Key name whose associated string is to be retrieved
     APLFLOAT     fDefault,     // Default result if lpwKeyName not found
     LPDICTIONARY lpDict)       // Ptr to workspace dictionary

{
    WCHAR wszTemp[1024];        // Save area for longest "sectionname:keyname"

    // Merge the section and key names
    MySprintfW (wszTemp,
                sizeof (wszTemp),
               L"%s" SECTION_SEP_STR L"%s",
                lpwAppName,
                lpwKeyName);
    // Get the integer
    return iniparser_getdouble (lpDict, wszTemp, fDefault);
} // End ProfileGetDouble


//***************************************************************************
//  $ProfileDelSection
//
//  Delete a section in a .ini profile
//***************************************************************************

void ProfileDelSection
    (LPWSTR       lpwAppName,   // Section name containing the key name
     LPDICTIONARY lpDict)       // Ptr to workspace dictionary

{
    WCHAR wszTemp[1024];        // Save area for longest "sectionname:keyname"

    // Format the section name with a trailing section separator
    MySprintfW (wszTemp,
                sizeof (wszTemp),
               L"%s" SECTION_SEP_STR,
                lpwAppName);
    // Delete the Section
    iniparser_delsection (lpDict, wszTemp);
} // End ProfileDelSection


//***************************************************************************
//  $ProfileWrite
//
//  Write out a .ini profile
//***************************************************************************

UBOOL ProfileWrite
    (LPDICTIONARY lpDict)       // Ptr to workspace dictionary

{
    WCHAR wszDrive[_MAX_DRIVE],
          wszDir  [_MAX_DIR],
          wszFname[_MAX_FNAME],
          wszExt  [_MAX_EXT],
          wszPath [_MAX_PATH];
    HANDLE hFile;               // File handle from CreateFileW
////DWORD  dwBytesOut;          // # bytes written out

    // Split out the drive and path from the module filename
    _wsplitpath (lpDict->lpwszDPFE, wszDrive, wszDir, wszFname, wszExt);

    // Form the destination path
    lstrcpyW (wszPath, wszDrive);
    lstrcatW (wszPath, wszDir  );

    // Append a backslash if not already present
    if (wszPath[lstrlenW (wszPath) - 1] NE '\\')
        lstrcatW (wszPath, L"\\"   );

    // Append the temp filename
    GetTempFileNameW (wszPath,                      // Temp path
                     L"",                           // Prefix string
                      0,                            // Unique ID (0 = none)
                      wszPath);                     // Output buffer
    // Create (or truncate the file)
    hFile =
      CreateFileW (wszPath,                         // lpwFileName
                   GENERIC_READ | GENERIC_WRITE,    // dwDesiredAccess
                   FILE_SHARE_READ,                 // dwShareMode
                   NULL,                            // lpSecurityAttributes
                   CREATE_ALWAYS,                   // dwCreationDistribution
                   FILE_ATTRIBUTE_NORMAL,           // dwFlagsAndAttributes
                   NULL);                           // hTemplateFile
    if (hFile EQ INVALID_HANDLE_VALUE)
    {
#ifdef DEBUG
        DWORD dwErrCode = GetLastError ();

        DbgBrk ();
#endif
        return FALSE;
    } // End IF

////// Write out the BOM for UTF-16
////WriteFile (hFile, UTF16LE_BOM, strcountof (UTF16LE_BOM), &dwBytesOut, NULL);
////
    // Write out the .ini file
    iniparser_dump_ini (lpDict, hFile);

    // Close it after creating the file
    CloseHandle (hFile); hFile = NULL;

    // Delete the old file
    DeleteFileW (lpDict->lpwszDPFE);

    // Rename the temp file
    MoveFileW (wszPath, lpDict->lpwszDPFE);

    return TRUE;
} // End ProfileWrite


//***************************************************************************
//  $ProfileUnload
//
//  Unload a .ini profile
//***************************************************************************

void ProfileUnload
    (LPDICTIONARY lpDict)       // Ptr to workspace dictionary

{
    // Free the dictionary
    iniparser_freedict (lpDict);
} // End ProfileUnload


//***************************************************************************
//  End of File: inifile.c
//***************************************************************************
