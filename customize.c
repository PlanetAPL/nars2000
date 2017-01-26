//***************************************************************************
//  NARS2000 -- Customize Dialog Box
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

/* To Do

* Finish Tab Colors
*

*/

#define STRICT
#define OEMRESOURCE                     // Necessary to get OBM checkboxes
#define CINTERFACE                      // To define the C (not C++) interface
#define COBJMACROS                      // To define the C macros for IMalloc, etc.
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <uxtheme.h>                    // Theme specific stuff
#include <vssym32.h>                    // ...
#include <shlobj.h>                     // For SHBrowseForFolder

#include "headers.h"

extern HICON hIconCustom;
WNDPROC lpfnOldKeybEditCtrlWndProc,     // Save area for old EditCtrl window proc
        lpfnOldFontsStaticTextWndProc;  // ...               Static Text ...
HWND hWndListBox;                       // Dialog ListBox window handle

// Local copy of gSyntaxColorName[].syntClr
SYNTAXCOLORS lclSyntaxColors[SC_LENGTH];

char pszNoCreateWCWnd[]  = "Unable to create Web Color Names window";
WCHAR wszCancelMessage[] = L"You have made changes to the Customize settings.  Save the changes?";

UBOOL gbCallbackBN = TRUE,
      gbCallbackEC = TRUE;


//***************************************************************************
//  []IC-specific Information
//***************************************************************************

#define NEG     WS_UTF16_OVERBAR
#define INF     WS_UTF16_INFINITY
#define NINF    NEG INF
#define R_NE_FLOOR_R    L"R" WS_UTF16_NOTEQUAL WS_UTF16_DOWNSTILE L"R"
#define PoM     WS_UTF16_PLUS_OR_MINUS

// []IC Index Names -- these must be in the same order as the IC_INDICES enum.
LPWCHAR icIndexNames[ICNDX_LENGTH]
= {WS_UTF16_COLONBAR L"0"                                                             , // 00:    {div} 0 and L {div} 0 for L != 0
   WS_UTF16_CIRCLESTAR L"0"                                                           , // 01:    {log} 0
   L"!N for integer N<0"                                                              , // 02:      !N for integer N < 0
   L"0" WS_UTF16_TIMES  INF                                                           , // 03:  0 {times} _
   L"0" WS_UTF16_TIMES NINF                                                           , // 04:  0 {times} {neg}_
   L"0" WS_UTF16_COLONBAR L"0"                                                        , // 05:  0 {div} 0
   INF WS_UTF16_COLONBAR INF L" (same sign)"                                          , // 06:  _ {div} _   (same sign)
   INF WS_UTF16_COLONBAR INF L" (diff sign)"                                          , // 07:  _ {div} _   (different sign)
   INF L"-"              INF L" (and related)"                                        , // 08:  _ - _ or _ + -_ or ...
   L"L|"   INF L"  for L>0"                                                           , // 09:  L   |    _  for L > 0
   L"L|"  NINF L" for L>0"                                                            , // 0A:  L   |   -_  for L > 0
   L"L|"   INF L"  for L<0"                                                           , // 0B:  L   |    _  for L < 0
   L"L|"  NINF L" for L<0"                                                            , // 0C:  L   |   -_  for L < 0
   NINF L"|R"                                                                         , // 0D:  -_  |   R   for R > 0
    INF L"|R"                                                                         , // 0E:   _  |   R   for R < 0
   L"0*0"                                                                             , // 0F:  0   *   0
   L"L*" INF L" for L" WS_UTF16_LEFTCARETUNDERBAR WS_UTF16_OVERBAR L"1"               , // 10:  L   *   _   for L <= -1
   L"0*"  INF                                                                         , // 11:  0   *   +_
   L"0*" NINF                                                                         , // 12:  0   *   -_
   PoM INF L"*0"                                                                      , // 13:  PoM_  *   0
   L"L * R for L<0 and " R_NE_FLOOR_R                                                 , // 14:  L   *   R   for L < 0 and R != floor (R)
   L"0" WS_UTF16_CIRCLESTAR L"0"                                                      , // 15:  0 {log} 0
   L"0" WS_UTF16_CIRCLESTAR L"1"                                                      , // 16:  0 {log} 1
   L"1" WS_UTF16_CIRCLESTAR L"0"                                                      , // 17:  1 {log} 0
   L"1" WS_UTF16_CIRCLESTAR L"1"                                                      , // 18:  1 {log} 1
   L"0" WS_UTF16_DOWNCARET PoM INF L"  or  " PoM INF WS_UTF16_DOWNCARET L"0"          , // 19:  0 {gcd} PoM_  or  PoM_ {gcd} 0
   L"0" WS_UTF16_UPCARET   PoM INF L"  or  " PoM INF WS_UTF16_UPCARET   L"0"          , // 1A:  0 {lcm} PoM_  or  PoM_ {lcm} 0
   L"0" WS_UTF16_CIRCLESTAR L"R (R" WS_UTF16_NOTEQUAL L"0 or 1)"                      , // 1B:  0 {log} N (N NE 0 or 1)
   L"L*" NINF L" for " NEG L"1 " WS_UTF16_LEFTCARETUNDERBAR L" L < 0"                 , // 1C:  L   *  -_   for -1 <= L < 0
  };

// []IC Index Values -- these must be in the same order as the IC_VALUES enum.
LPWCHAR icIndexValues[ICVAL_LENGTH]
= {WS_UTF16_OVERBAR L"1"    ,
   L"0"                     ,
   L"1"                     ,
   L"DOMAIN ERROR"          ,
   INF                      ,
   NINF                     ,
   L"L"                     ,
   L"R"                     ,
  };
#undef  NINF
#undef  INF


//***************************************************************************
//  Keyboard-specific Information
//***************************************************************************

#define KEYB_UNICODE_LIMIT          5   // Maximum # WCHARs allowed in the Keyboard Unicode Edit Ctrl
                                        //   to accommodate 16bFFFF = 65535 in decimal
                                        // Change to 6 when we support UCS-32
HFONT hFontKB = NULL;                   // Font handle for the keyboard TabCtrl and Keycaps

#define KKC_CY          4               // # rows in Keyb Keycaps

// Array of beginning IDs for each Keycap row
UINT aKKC_IDC_BEG[]
 = {IDC_KEYB_BN_KC_00,
    IDC_KEYB_BN_KC_10,
    IDC_KEYB_BN_KC_20,
    IDC_KEYB_BN_KC_30,
   };

// Array of ending IDs for each Keycap row
UINT aKKC_IDC_END[]
 = {IDC_KEYB_BN_KC_0C,
    IDC_KEYB_BN_KC_1C,
    IDC_KEYB_BN_KC_2B,
    IDC_KEYB_BN_KC_3A,
   };

// Scan codes for the Keycaps, row by row
UINT aKKC_SC_R0[] = {0x29, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D};
UINT aKKC_SC_R1[] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x2B};
UINT aKKC_SC_R2[] = {0x1E, 0x1F, 0x20 ,0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x2B};
UINT aKKC_SC_R3[] = {0x56, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35};

typedef struct tagKKC_SC
{
    UINT *aSC;
    UINT uLen;
} KKC_SC, *LPKKC_SC;

// Array of Keyboard Keycap scancodes one for each row
//   along with their length
KKC_SC aKKC_SC[]
 = {{aKKC_SC_R0, countof (aKKC_SC_R0)},
    {aKKC_SC_R1, countof (aKKC_SC_R1)},
    {aKKC_SC_R2, countof (aKKC_SC_R2)},
    {aKKC_SC_R3, countof (aKKC_SC_R3)},
   };

// Array of beginning IDs for each TabCtrl row
UINT aKTC_IDC_BEG[]
 = {IDC_KEYB_BN_TC_00,
    IDC_KEYB_BN_TC_10,
    IDC_KEYB_BN_TC_20,
    IDC_KEYB_BN_TC_30,
    IDC_KEYB_BN_TC_40,
    IDC_KEYB_BN_TC_50,
    IDC_KEYB_BN_TC_60,
    IDC_KEYB_BN_TC_70,
    IDC_KEYB_BN_TC_80,
    IDC_KEYB_BN_TC_90,
    IDC_KEYB_BN_TC_A0,
    IDC_KEYB_BN_TC_B0,
    IDC_KEYB_BN_TC_C0,
   };

#define NOCONTROL           -1

#define KTC_TXT_CX      6           // # cols in KTC text
#define KTC_TXT_CY     13           // # rows ...

WCHAR awKTC_APL[KTC_TXT_CY][KTC_TXT_CX]
= {{UTF16_UPCARET       , UTF16_DOWNCARET       , UTF16_UPCARETTILDE    , UTF16_DOWNCARETTILDE   , UTF16_TILDE            , UTF16_ZILDE             },
   {UTF16_DELTASTILE    , UTF16_DELSTILE        , UTF16_DELTA           , UTF16_DEL              , UTF16_DELTAUNDERBAR    , UTF16_DIERESISDEL       },
   {UTF16_QUAD          , UTF16_QUOTEQUAD       , UTF16_SQUAD           , UTF16_DOMINO           , UTF16_COLONBAR         , UTF16_TIMES             },
   {UTF16_UPARROW       , UTF16_DOWNARROW       , UTF16_UPDOWNARROWS    , UTF16_LEFTARROW        , UTF16_RIGHTARROW       , UTF16_LEFTRIGHTARROWS   },
   {UTF16_UPTACK        , UTF16_DOWNTACK        , UTF16_LEFTTACK        , UTF16_RIGHTTACK        , UTF16_UPTACKJOT        , UTF16_DOWNTACKJOT       },
   {UTF16_JOT           , UTF16_DIERESISJOT     , UTF16_DIERESISDOWNTACK, UTF16_DIERESISSTAR     , UTF16_DIERESIS         , UTF16_VARIANT           },
   {UTF16_UPSTILE       , UTF16_DOWNSTILE       , UTF16_STILE           , UTF16_STILETILDE       , UTF16_DOWNSHOESTILE    , UTF16_PI                },
   {UTF16_LEFTSHOE      , UTF16_RIGHTSHOE       , UTF16_LEFTSHOEUNDERBAR, UTF16_RIGHTSHOEUNDERBAR, UTF16_UPSHOE           , UTF16_DOWNSHOE          },
   {UTF16_IOTA          , UTF16_IOTAUNDERBAR    , UTF16_EPSILON         , UTF16_EPSILONUNDERBAR  , UTF16_RHO              , UTF16_ROOT              },
   {UTF16_CIRCLE        , UTF16_CIRCLESTILE     , UTF16_CIRCLEBAR       , UTF16_CIRCLESLOPE      , UTF16_CIRCLESTAR       , UTF16_DIERESISCIRCLE    },
   {UTF16_SLASH         , UTF16_SLASHBAR        , UTF16_SLOPE           , UTF16_SLOPEBAR         , UTF16_NOTEQUAL         , UTF16_SECTION           },
   {UTF16_EQUALUNDERBAR , UTF16_NOTEQUALUNDERBAR, UTF16_ALPHA           , UTF16_OMEGA            , UTF16_LEFTCARETUNDERBAR, UTF16_RIGHTCARETUNDERBAR},
   {UTF16_DIAMOND       , UTF16_COMMABAR        , UTF16_OVERBAR         , UTF16_LAMP             , UTF16_INFINITY         , UTF16_CIRCLEMIDDLEDOT   },
  };

WCHAR awKTC_ABC[KTC_TXT_CY][KTC_TXT_CX]
= {{L'a', L'b', L'c', L'd', L'e', L'f'},
   {L'g', L'h', L'i', L'j', L'k', L'l'},
   {L'm', L'n', L'o', L'p', L'q', L'r'},
   {L's', L't', L'u', L'v', L'w', L'x'},
   {L'y', L'z',   0 ,   0 ,   0 ,   0 },
   {  0 ,   0 ,   0 ,   0 ,   0 ,   0 },
   {L'A', L'B', L'C', L'D', L'E', L'F'},
   {L'G', L'H', L'I', L'J', L'K', L'L'},
   {L'M', L'N', L'O', L'P', L'Q', L'R'},
   {L'S', L'T', L'U', L'V', L'W', L'X'},
   {L'Y', L'Z',   0 ,   0 ,   0 ,   0 },
   {  0 ,   0 ,   0 ,   0 ,   0 ,   0 },
  };

WCHAR awKTC_123[KTC_TXT_CY][KTC_TXT_CX]
= {{L'0', L'1', L'2', L'3', L'4', L'5'},
   {L'6', L'7', L'8', L'9', L'0',   0 },
   {  0 ,   0 ,   0 ,   0 ,   0 ,   0 },
   {L'`', L'~', L'!', L'@', L'#', L'$'},
   {L'%', L'^', L'&', L'*', L'_', L'='},
   {L'|', L'?',   0 ,   0 ,   0 ,   0 },
   {  0 ,   0 ,   0 ,   0 ,   0 ,   0 },
   {L'(', L')', L'[', L']', L'{', L'}'},
   {L'+', L'-', L'/', L'\\',L'<', L'>'},
   {L';', L':', L',', L'.', L'\'',L'"'},
   {  0 ,   0 ,   0 ,   0 ,   0 ,   0 },
   {  0 ,   0 ,   0 ,   0 ,   0 ,   0 },
  };

WCHAR awKTC_BOX[KTC_TXT_CY][KTC_TXT_CX]
= {{UTF16_LDC_LT_UL     , UTF16_LDC_LT_HORZ_D , UTF16_LDC_LT_UR     , 0 , UTF16_LDC_LT_HORZ , 0},
   {UTF16_LDC_LT_VERT_R , UTF16_LDC_LT_CROSS  , UTF16_LDC_LT_VERT_L , 0 , UTF16_LDC_LT_VERT , 0},
   {UTF16_LDC_LT_LL     , UTF16_LDC_LT_HORZ_U , UTF16_LDC_LT_LR     , 0 ,                 0 , 0},
   {                  0 ,                   0 ,                   0 , 0 ,                 0 , 0},
   {UTF16_LDC_DB_UL     , UTF16_LDC_DB_HORZ_D , UTF16_LDC_DB_UR     , 0 , UTF16_LDC_DB_HORZ , 0},
   {UTF16_LDC_DB_VERT_R , UTF16_LDC_DB_CROSS  , UTF16_LDC_DB_VERT_L , 0 , UTF16_LDC_DB_VERT , 0},
   {UTF16_LDC_DB_LL     , UTF16_LDC_DB_HORZ_U , UTF16_LDC_DB_LR     , 0 ,                 0 , 0},
   {                  0 ,                   0 ,                   0 , 0 ,                 0 , 0},
   {L'\x2580'           , L'\x2584'           ,                   0 , 0 ,                 0 , 0},
   {L'\x2588'           , L'\x258C'           , L'\x2590'           , 0 ,                 0 , 0},
   {L'\x2591'           , L'\x2592'           , L'\x2593'           , 0 ,                 0 , 0},
   {                  0 ,                   0 ,                   0 , 0 ,                 0 , 0},
  };


WCHAR awKTC_NLS[KTC_TXT_CY][KTC_TXT_CX]
= {{L'\x00E0', L'\x00E1', L'\x00E2', L'\x00E3', L'\x00E4', L'\x00E5'},    // a
   {L'\x00C0', L'\x00C1', L'\x00C2', L'\x00C3', L'\x00C4', L'\x00C5'},    // A
   {        0, L'\x0107', L'\x0109', L'\x010B', L'\x00E7', L'\x010D'},    // c
   {        0, L'\x0106', L'\x0108', L'\x010A', L'\x00C7', L'\x010C'},    // C
   {L'\x00E8', L'\x00E9', L'\x00EA',         0, L'\x00EB', L'\x00F8'},    // e
   {L'\x00C8', L'\x00C9', L'\x00CA',         0, L'\x00CB', L'\x00F1'},    // E
   {L'\x00EC', L'\x00ED', L'\x00EE', L'\x0129', L'\x00EF', L'\x00D1'},    // i
   {L'\x00CC', L'\x00CD', L'\x00CE', L'\x0128', L'\x00CF', L'\x00DF'},    // I
   {L'\x00F2', L'\x00F3', L'\x00F4', L'\x00F5', L'\x00F6',         0},    // o
   {L'\x00D2', L'\x00D3', L'\x00D4', L'\x00D5', L'\x00D6',         0},    // O
   {L'\x00F9', L'\x00FA', L'\x00FB', L'\x0169', L'\x00FC',         0},    // u
   {L'\x00D9', L'\x00DA', L'\x00DB', L'\x0168', L'\x00DC',         0},    // U
   {L'\x00A3', L'\x20AC', L'\x20A7', L'\x00A5', L'\x00BF', L'\x00A1'},    // Currency
  };

typedef struct tagKTC               // Keyb TabCtrl
{
    LPWSTR  lpwName;                // Ptr to Tab name
    WCHAR   (*lpwText)[KTC_TXT_CX]; // Ptr to TC page
    UINT    idLastHighlight;        // IDC_xxx of last highted button (NOCONTROL = none)
} KTC, *LPKTC;

KTC aKTC[]
= {{L"APL", awKTC_APL, NOCONTROL},
   {L"abc", awKTC_ABC, NOCONTROL},
   {L"123", awKTC_123, NOCONTROL},
   {L"Box", awKTC_BOX, NOCONTROL},
   {L"NLS", awKTC_NLS, NOCONTROL},
  };

LPKEYBLAYOUTS lpGlbKeybLayouts = NULL,  // Ptr to array of global Keyboard Layouts
              lpLclKeybLayouts = NULL;  // ...             local  ...
KEYBLAYOUTS aKeybLayoutOrig;            // Copy of the original keyboard layout
HGLOBAL hLclKeybLayouts;                // Local keyb layouts global memory handle

UINT uLclKeybLayoutNumAct,              // # of local active keyboard layout
     uLclKeybLayoutNumVis,              // # of local visible ...
     uLclKeybLayoutCount,               // Total # local keyboard layouts (built-in + user-defined)
     uLclKeybLayoutUser,                // # local user-defined keyboard layouts
     uLclKeybChar,                      // Local last selected unicode value
     uLclUserChar;                      // Local Line Continuation marker

WCHAR wszLclKeybLayoutAct[KBLEN],       // Active local keyboard layout name
      wszLclKeybLayoutVis[KBLEN];       // Visible .....

typedef struct tagNEWKEYBDLG
{
    LPWCHAR lpwKeybLayoutName;          // Ptr to input/output keyboard layout name
    HWND    hWndProp;                   // Property page window handle
} NEWKEYBDLG, *LPNEWKEYBDLG;

// Define message # used to set a color
#define MYWM_SETCOLOR           (WM_APP + 0)

// Define default pt size for the CLEARWS Values font
#define DEF_CWSPTSIZE       11


//***************************************************************************
//  Theme-specific Information
//***************************************************************************

typedef HRESULT (__stdcall *PFNCLOSETHEMEDATA)                  (HTHEME hTheme);
typedef HRESULT (__stdcall *PFNDRAWTHEMEBACKGROUND)             (HTHEME hTheme,
                                                                 HDC hdc,
                                                                 int iPartId,
                                                                 int iStateId,
                                                                 const RECT *pRect,
                                                                 const RECT *pClipRect);
typedef HTHEME  (__stdcall *PFNOPENTHEMEDATA)                   (HWND hwnd,
                                                                 LPCWSTR pszClassList);
typedef HRESULT (__stdcall *PFNDRAWTHEMETEXT)                   (HTHEME hTheme,
                                                                 HDC hdc,
                                                                 int iPartId,
                                                                 int iStateId,
                                                                 LPCWSTR pszText,
                                                                 int iCharCount,
                                                                 DWORD dwTextFlags,
                                                                 DWORD dwTextFlags2,
                                                                 const RECT *pRect);
typedef HRESULT (__stdcall *PFNGETTHEMEBACKGROUNDCONTENTRECT)   (HTHEME hTheme,
                                                                 HDC hdc,
                                                                 int iPartId,
                                                                 int iStateId,
                                                                 const RECT *pBoundingRect,
                                                                 RECT *pContentRect);
PFNOPENTHEMEDATA                 zOpenThemeData                 = NULL;
PFNDRAWTHEMEBACKGROUND           zDrawThemeBackground           = NULL;
PFNCLOSETHEMEDATA                zCloseThemeData                = NULL;
////PFNDRAWTHEMETEXT                 zDrawThemeText                 = NULL;
////PFNGETTHEMEBACKGROUNDCONTENTRECT zGetThemeBackgroundContentRect = NULL;

HMODULE hModThemes = NULL;          // Module handle for UXTHEME.DLL
UBOOL bThemeLibLoaded = FALSE,      // TRUE iff the theme library successfully loaded
      bThemedKeybs = FALSE,         // TRUE iff the default theme for Keybs is active
      bMouseOverButton = FALSE;     // ***FIXME*** -- Separate flags for each Keycap???


//***************************************************************************
//  $CustomizeDlgProc
//
//  Modeless dialog box to allow the user to customize the program
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CustomizeDlgProc"
#else
#define APPEND_NAME
#endif

INT_PTR CALLBACK CustomizeDlgProc
    (HWND   hWnd,       // Window handle
     UINT   message,    // Type of message
     WPARAM wParam,     // Additional information
     LPARAM lParam)     // ...

{
    static HFONT        hFontCWS = NULL;        // Font for CLEARWS Values
    static HWND         hWndGroupBox,           // Dialog GroupBox window handle
                        hWndDirsComboBox,       // Directories ComboBox window handle
                        hWndKeybComboBox,       // Keyboard Layout ComboBox window handle
                        hWndKeycapC,            // Keyboard letter C window handle
                        hWndKeycapX,            // ...             X ...
                        hWndKeycapV,            // ...             V ...
                        hWndKeycapZ,            // ...             Z ...
                        hWndKeycapY,            // ...             Y ...
                        hWndKeycapS,            // ...             S ...
                        hWndKeycapE,            // ...             E ...
                        hWndKeycapQ,            // ...             Q ...
                        hWndLast,               // Last (outgoing) ...
                        hDlg,                   // Dialog          ...
                        hWndApply,              // Apply button    ...
                        hWndFEATURE_CB1,        // []FEATURE ComboBox #1 ...
                        hWndFEATURE_CB2,        // []FEATURE ...      #2 ...
                        hWndIC_CB1,             // []IC ComboBox #1 ...
                        hWndIC_CB2;             // []IC ...      #2 ...
           UINT         uSel,                   // Index of the current selection
                        uShow,                  // Show/hide flag
                        uCol,                   // Loop counter
                        uValue,                 // Temporary value
                        idLHL,                  // ID of LastHighlight
                        uStyle,                 // Window Style
                        uScanCode,              // Scan code
                        uCnt;                   // Loop counter
           FONTENUM     enumFont;               // Font enumerator
           APLU3264     uKeybSize;              // Size of local/global keyboard layouts
           HANDLE_PTR   uIDD;                   // Item data:  IDD_xxx or HWND
           RECT         rc;                     // Temporary rectangle
           LPAPLCHAR    lpMemChr,               // Ptr to global memory
                        lpaplChar;              // ...
           LPAPLINT     lpMemInt;               // ...
           APLNELM      aplNELM;                // NELM of object
    static POINT        ptGroupBox;             // X- & Y- coordinates (in pixels) of the upper left corner of the GroupBox
    static SIZE         szGroupBox;             // Size (in pixels) of the GroupBox
    static FONTENUM     lclSameFontAs[FONTENUM_LENGTH];

    static UINT         FontsRadio1[] = {IDC_FONTS_RADIO1A},
                        FontsRadio2[] = {IDC_FONTS_RADIO2A, IDC_FONTS_RADIO2B},
                        FontsRadio3[] = {IDC_FONTS_RADIO3A, IDC_FONTS_RADIO3B, IDC_FONTS_RADIO3C},
                        FontsRadio4[] = {IDC_FONTS_RADIO4A, IDC_FONTS_RADIO4B, IDC_FONTS_RADIO4C, IDC_FONTS_RADIO4D},
                        FontsRadio5[] = {IDC_FONTS_RADIO5A, IDC_FONTS_RADIO5B, IDC_FONTS_RADIO5C, IDC_FONTS_RADIO5D, IDC_FONTS_RADIO5E},
                        FontsRadio6[] = {IDC_FONTS_RADIO6A, IDC_FONTS_RADIO6B, IDC_FONTS_RADIO6C, IDC_FONTS_RADIO6D, IDC_FONTS_RADIO6E, IDC_FONTS_RADIO6F},
                        FontsRadio7[] = {IDC_FONTS_RADIO7A, IDC_FONTS_RADIO7B, IDC_FONTS_RADIO7C, IDC_FONTS_RADIO7D, IDC_FONTS_RADIO7E, IDC_FONTS_RADIO7F, IDC_FONTS_RADIO7G},
                        FontsRadio8[] = {IDC_FONTS_RADIO8A, IDC_FONTS_RADIO8B, IDC_FONTS_RADIO8C, IDC_FONTS_RADIO8D, IDC_FONTS_RADIO8E, IDC_FONTS_RADIO8F, IDC_FONTS_RADIO8G, IDC_FONTS_RADIO8H}
                        ;
    static LPUINT       FontsRadioPtr[] = {&FontsRadio1[0],
                                           &FontsRadio2[0],
                                           &FontsRadio3[0],
                                           &FontsRadio4[0],
                                           &FontsRadio5[0],
                                           &FontsRadio6[0],
                                           &FontsRadio7[0],
                                           &FontsRadio8[0],
                                          };
    static UINT         ResetRadioCT      [] = {IDC_RESET_CT_RADIO1      , IDC_RESET_CT_RADIO2      },
                        ResetRadioDT      [] = {IDC_RESET_DT_RADIO1      , IDC_RESET_DT_RADIO2      },
                        ResetRadioFC      [] = {IDC_RESET_FC_RADIO1      , IDC_RESET_FC_RADIO2      },
                        ResetRadioFEATURE [] = {IDC_RESET_FEATURE_RADIO1 , IDC_RESET_FEATURE_RADIO2 },
                        ResetRadioFPC     [] = {IDC_RESET_FPC_RADIO1     , IDC_RESET_FPC_RADIO2     },
                        ResetRadioIC      [] = {IDC_RESET_IC_RADIO1      , IDC_RESET_IC_RADIO2      },
                        ResetRadioIO      [] = {IDC_RESET_IO_RADIO1      , IDC_RESET_IO_RADIO2      },
                        ResetRadioPP      [] = {IDC_RESET_PP_RADIO1      , IDC_RESET_PP_RADIO2      },
                        ResetRadioPW      [] = {IDC_RESET_PW_RADIO1      , IDC_RESET_PW_RADIO2      },
                        ResetRadioRL      [] = {IDC_RESET_RL_RADIO1      , IDC_RESET_RL_RADIO2      };
    static LPUINT       ResetRadioPtr[] = {&ResetRadioCT      [0],
                                           &ResetRadioDT      [0],
                                           &ResetRadioFC      [0],
                                           &ResetRadioFEATURE [0],
                                           &ResetRadioFPC     [0],
                                           &ResetRadioIC      [0],
                                           &ResetRadioIO      [0],
                                           &ResetRadioPP      [0],
                                           &ResetRadioPW      [0],
                                           &ResetRadioRL      [0],
                                          };
           HWND         hWndProp,               // Property page window handle
                        hWndProp1,              // ...
                        hWndProp2,              // ...
                        hWndFont,               // Font property page ChooseFontW button window handle
                        hWndKTC,                // Keyboard TabCtrl window handle
                        hWnd_UD;                // []FPC/[]IO/[]PP/[]PW UpDown Control or its Buddy window handle
           LOGFONTW     lf_ST,                  // Static text LOGFONTW
                        lf_CWS;                 // CLEARWS Values ...
    static HFONT        hFontNorm_ST = NULL,    // Normal static text font handle
                        hFontBold_ST = NULL;    // Bold   ...
    static APLINT       featValues[FEATURENDX_LENGTH]; // []FEATURE local values
    static APLINT       icValues[ICNDX_LENGTH]; // []IC local values
    static RESET_VARS   lclResetVars;           // Local copy of bResetVars
           WCHAR        wszTemp[128];           // Temporary WCHAR storage
           char         szTemp[128];            // Temporary char storage
    static WCHAR        wszText[2] = {L'\0'};
    static CHOOSECOLORW cc = {0};               // Struct for ChooseColorW
    static UBOOL        gbFore;                 // TRUE iff the button is FOREGROUND
    static UINT         guIndex;                // Index of the button
    static LPWCHAR      lpwszQuadDTNames[] = {DEF_QUADDT_NAMES};
    static   WCHAR        wszQuadDTAllow[] = {DEF_QUADDT_ALLOW};

    static COLORNAMES scMenuItems[] =
    {
        {DEF_SCN_BLACK    , L"Black"      },    // 00
        {DEF_SCN_WHITE    , L"White"      },    // 01
        {DEF_SCN_GREEN    , L"Green"      },    // 02
        {DEF_SCN_MAROON   , L"Maroon"     },    // 03
        {DEF_SCN_OLIVE    , L"Olive"      },    // 04
        {DEF_SCN_NAVY     , L"Navy"       },    // 05
        {DEF_SCN_PURPLE   , L"Purple"     },    // 06
        {DEF_SCN_GRAY     , L"Gray"       },    // 07
        {DEF_SCN_YELLOW   , L"Yellow"     },    // 08
        {DEF_SCN_LIME     , L"Lime"       },    // 09
        {DEF_SCN_AQUA     , L"Aqua"       },    // 0A
        {DEF_SCN_FUCHSIA  , L"Fuchsia"    },    // 0B
        {DEF_SCN_SILVER   , L"Silver"     },    // 0C
        {DEF_SCN_RED      , L"Red"        },    // 0D
        {DEF_SCN_BLUE     , L"Blue"       },    // 0E
        {DEF_SCN_TEAL     , L"Teal"       },    // 0F
        {DEF_SCN_BLACK    , L"Custom"     },    // 10
        {DEF_SCN_BLACK    , L"Web Colors" },    // 11
    };

#define MI_CUSTOM       (countof (scMenuItems) - 2)
#define MI_WEBCOLORS    (countof (scMenuItems) - 1)

    typedef struct tagUNITRANS_STR
    {
        LPWCHAR lpwChar;            // Ptr to text
        UBOOL   bValidCopy,         // TRUE iff this entry is valid for Copy
                bValidPaste;        // ...                              Paste
        UINT    IncTransCopy,       // Incoming Translation for Copy
                IncTransPaste,      // ...                      Paste
                OutTransCopy,       // Outgoing Translation for Copy
                OutTransPaste;      // ...                      Paste
    } UNITRANS_STR;

    // The following var must be in the same order as UNITRANS_xxx
    static UNITRANS_STR unitransStr[] = {{L"APL+Win",             TRUE,   TRUE , 0, 0, 0, 0},
                                         {L"ISO Standard PDF",    TRUE,   TRUE , 1, 1, 1, 1},
                                         {L"APL2",                TRUE,   TRUE , 2, 2, 2, 2},
                                         {L"Dyalog",              TRUE,   TRUE , 3, 3, 3, 3},
                                         {L"PC/3270",             TRUE,   TRUE , 4, 4, 4, 4},
                                         {L"Normal (Unicode)",    FALSE,  TRUE , 0, 5, 6, 5},
                                         {L"NARS (Unicode)",      TRUE,   FALSE, 5, 0, 0, 0},
                                         {L"{braces}",            TRUE,   TRUE , 6, 6, 7, 7},
                                        };
#define UNITRANS_STR_LENGTH      countof (unitransStr)

    // The following text is too long for a resource file (limit 256 chars), so we define it here
    //   and save it into the dialog upon initialization of the appropriate Property Page
    static LPWCHAR    lpwFontsText = L"There are several Categories of fonts (Session Manager, etc.).  "
                                     L"Click the Change button to the left of a Category Name to change the font "
                                     L"for that Category and all Categories tied to it.  Click a radio button to the right of "
                                     L"a Category Name to tie the font for that Category with the Category at the top of that column.  "
                                     L"The rightmost radio button in a row unties that Category from all others.  "
                                     L"Buttons are visible only if that choice is allowed.",
                      lpwResetText = L"When one of the following System Variables is assigned a simple empty vector, "
                                     L"the system assigns to it a value which is either the workspace CLEAR WS "
                                     L"value (User-controlled) or the System Default value (Fixed).  This dialog "
                                     L"allows you to choose which value is used.",
                      lpwDirsText  = L"The Library Directories are used when searching for workspaces (*.ws.nars) or "
                                     L"transfer files (*.atf).  To include a new directory in the list, click on "
                                     L"Browse and select a directory.  To delete a directory from the list, select "
                                     L"it and then click on Delete."
                                     ;

    static TOOLINFOW tti = {sizeof (tti)};  // For Tooltip Ctrl
    static CHOOSEFONTW cfKB = {0};

    WCHAR        wszStr[KEYB_UNICODE_LIMIT + 1];
    LPWCHAR      lpwszStr;              // Temporary string ptr

    //***************************************************************************
    // LibDirs-specific data
    //***************************************************************************

    LPWSZLIBDIRS lpwszLibDirs;          // Ptr to LibDirs
    static BROWSEINFOW bi = {0};        // Browse info for Dirs
    static LPITEMIDLIST pidl;
    static WCHAR wszBrowsePath[_MAX_PATH];
    static COMBOBOXINFO cbi = {sizeof (cbi)};

    static HTHEME hThemeKeybs = NULL;   // Handle to default theme for Keybs

#define MYWM_INITDIALOG                 (WM_APP + 100)
#define MYWM_ENABLECHOOSEFONT           (WM_APP + 101)

////LCLODSAPI ("CD: ", hWnd, message, wParam, lParam);
    // Split cases based upon the message #
    switch (message)
    {
        case WM_INITDIALOG:         // hwndFocus = (HWND) wParam; // handle of control to receive focus
                                    // lInitParam = lParam;       // initialization parameter
            // If it's the first time through, ...
            if (lParam EQ 0)
            {
                // Because we use this DlgProc as the procedure for the
                //   Property Pages, too, when we're called by a Property
                //   Page, the incoming window handle is that of the
                //   Property Page, not the original dialog.  By saving the
                //   original window handle in the static var hDlg, we
                //   always know the dialog handle.
                hDlg = hWnd;

                // Save the dialog handle for use in <ApplyNewFontSM> in case
                //   the font used for CLEAR WS values (hFontSM) changes so we
                //   can apply the changes to the Edit Ctrls used there.
                ghDlgCustomize = hDlg;

                // Initialize for themes
                InitThemes ();

                // Change the icon to our own
                SendMessageW (hDlg, WM_SETICON, ICON_BIG, (LPARAM) (HANDLE_PTR) hIconCustom);

                // Mark as no last window handle
                hWndLast = NULL;

                // Mark as no previous Bold Static Text font
                hFontBold_ST = NULL;

                // Save the window handle for the Apply button for later use
                hWndApply = GetDlgItem (hDlg, IDC_APPLY);

                // Get the IDC_CUST_LB window handle
                hWndListBox = GetDlgItem (hDlg, IDC_CUST_LB);

                // Loop through the selections
                for (uCnt = 0; uCnt < custStrucLen; uCnt++)
                {
                    // Fill the ListBox with items and item data
                    uSel = (UINT) SendMessageW (hWndListBox, LB_ADDSTRING, 0, (LPARAM) custStruc[uCnt].lpwTitle);
                    SendMessageW (hWndListBox, LB_SETITEMDATA, uSel, custStruc[uCnt].uIDD);

                    // Mark as not initialized
                    custStruc[uCnt].bInitialized = FALSE;
                } // End FOR

                // Get the IDC_GROUPBOX window handle
                hWndGroupBox = GetDlgItem (hDlg, IDC_GROUPBOX);

                // Get the coordinates of the groupbox in screen coordinates
                GetWindowRect (hWndGroupBox, &rc);

                // Copy to POINT & SIZE structs
                ptGroupBox.x  = rc.left;
                ptGroupBox.y  = rc.top;
                szGroupBox.cx = rc.right  - rc.left;
                szGroupBox.cy = rc.bottom - rc.top;

                // Convert these coordinates from screen-relative to dialog box-relative
                ScreenToClient (hDlg, &ptGroupBox);

                // Reposition the dialog to the center of the screen
                // ***FIXME*** -- Should we remember where it was the last time
                //                and reposition it to there?
                CenterWindow (hDlg);

                // Tell the ListBox to change the current selection
                SendMessageW (hWndListBox, LB_SETCURSEL, gInitCustomizeCategory, 0);

                // Tell the dialog the selection changed
                SendMessageW (hDlg, WM_COMMAND, GET_WM_COMMAND_MPS (IDC_CUST_LB, NULL, LBN_SELCHANGE));
            } // End IF

            return DLG_MSGDEFFOCUS;     // Use the focus in wParam, DWLP_MSGRESULT is ignored

        case MYWM_INITDIALOG:
            // Set the group box text
            SetDlgItemTextW (hDlg, IDC_GROUPBOX, custStruc[wParam].lpwTitle);

            // If the struc hasn't been initialized as yet, ...
            if (!custStruc[wParam].bInitialized)
            {
                UBOOL bApply;                           // TRUE iff the Apply button is enabled

                // Mark it as initialized for next time
                custStruc[wParam].bInitialized = TRUE;

                // Get the associated item data
                hWndProp = (HWND)
                  SendMessageW (hWndListBox, LB_GETITEMDATA, wParam, 0);

                // The techniques used to initialize the Property Pages
                //   sometimes cause the Apply button to be enabled.
                // To avoid enabling it spuriously, we save its state
                //   here and restore that state after the Property Page
                //   is initialized.
                bApply = IsWindowEnabled (hWndApply);

                // Initialize each separate property page
                // Split cases based upon the IDD_xxx value
                switch (custStruc[wParam].uIDD)
                {
                    case IDD_PROPPAGE_CLEARWS_VALUES:                   // MYWM_INITDIALOG
                    {
                        int iLogPixelsY;                    // # vertical pixels per inch in the DC

                        // Initialize the CLEAR WS values

                        // Get # vertical pixels per inch
                        iLogPixelsY = GetLogPixelsY (NULL);

                        // Get the current SM LOGFONTW struc and set its size
                        GetObjectW (hFontSM, sizeof (lf_CWS), &lf_CWS);

                        lf_CWS.lfHeight = -MulDiv (DEF_CWSPTSIZE, iLogPixelsY, 72);

                        // Create a new HFONT from the changed LOGFONTW
                        hFontCWS = MyCreateFontIndirectW (&lf_CWS);

                        // Set the font for each Edit Ctrl or ComboBox
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_ALX_EC      ), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontCWS, MAKELPARAM (FALSE, 0));  // []ALX
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_CT_EC       ), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontCWS, MAKELPARAM (FALSE, 0));  // []CT
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_DT_CB       ), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontCWS, MAKELPARAM (FALSE, 0));
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_ELX_EC      ), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontCWS, MAKELPARAM (FALSE, 0));  // []ELX
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_FC_EC       ), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontCWS, MAKELPARAM (FALSE, 0));  // []FC
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_FEATURE_CB1 ), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontCWS, MAKELPARAM (FALSE, 0));  // []FEATURE
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_FEATURE_CB2 ), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontCWS, MAKELPARAM (FALSE, 0));  // []FEATURE
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_FPC_EC      ), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontCWS, MAKELPARAM (FALSE, 0));  // []FPC
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_IC_CB1      ), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontCWS, MAKELPARAM (FALSE, 0));  // []IC
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_IC_CB2      ), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontCWS, MAKELPARAM (FALSE, 0));  // []IC
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_IO_EC       ), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontCWS, MAKELPARAM (FALSE, 0));  // []IO
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_LX_EC       ), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontCWS, MAKELPARAM (FALSE, 0));  // []LX
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_MF_CB       ), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontCWS, MAKELPARAM (FALSE, 0));  // []MF
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_PP_EC       ), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontCWS, MAKELPARAM (FALSE, 0));  // []PP
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_PW_EC       ), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontCWS, MAKELPARAM (FALSE, 0));  // []PW
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_RL_EC       ), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontCWS, MAKELPARAM (FALSE, 0));  // []RL

                        // Save the normal font handle
                        hFontNorm_ST = (HFONT) SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_ALX_LEN), WM_GETFONT, 0, 0);

                        // Get the current LOGFONTW struc and make it bold
                        GetObjectW (hFontNorm_ST, sizeof (lf_ST), &lf_ST);

                        // Set the new weight
                        lf_ST.lfWeight = FW_BOLD;

                        // Create a new HFONT from the changed LOGFONTW
                        hFontBold_ST = MyCreateFontIndirectW (&lf_ST);

                        //***************************************************************
                        // []ALX -- CLEAR WS Values
                        //***************************************************************

                        // Lock the memory to get a ptr to it
                        lpMemChr = MyGlobalLockVar (hGlbQuadALX_CWS);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemChr)
                        // Get the NELM
                        aplNELM = lpHeader->NELM;
#undef  lpHeader
                        // Skip over the header and dimension to the data
                        lpMemChr = VarArrayDataFmBase (lpMemChr);

                        // Copy to temp string so we can zero-terminate it
                        CopyMemoryW (lpwszGlbTemp, lpMemChr, (APLU3264) aplNELM);

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbQuadALX_CWS); lpMemChr = NULL;

                        // Ensure properly terminated
                        lpwszGlbTemp[aplNELM] = WC_EOS;

                        // Set the text
                        SetDlgItemTextW (hWndProp, IDC_CLEARWS_ALX_EC, lpwszGlbTemp);

                        //***************************************************************
                        // []CT -- CLEAR WS Values
                        //***************************************************************

                        // Format the value
                        lpMemChr =
                          FormatAplFltFC (lpwszGlbTemp,                 // Ptr to output save area
                                          fQuadCT_CWS,                  // The value to format
                                          DEF_MAX_QUADPP_IEEE,          // Precision to use
                                          L'.',                         // Char to use as decimal separator
                                          UTF16_OVERBAR,                // Char to use as overbar
                                          FLTDISPFMT_RAWFLT,            // Float display format
                                          FALSE);                       // TRUE iff we're to substitute text for infinity
                        // Zap the trailing blank
                        lpMemChr[-1] = WC_EOS;

                        // Set the text
                        SetDlgItemTextW (hWndProp, IDC_CLEARWS_CT_EC,  lpwszGlbTemp);

                        //***************************************************************
                        // []DT -- CLEAR WS Values
                        //***************************************************************

                        // Loop through the []DT names
                        for (uCnt = 0; uCnt < strcountof (wszQuadDTAllow); uCnt++)
                            // Insert the []DT names
                            SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_DT_CB), CB_ADDSTRING, 0, (LPARAM) (HANDLE_PTR) lpwszQuadDTNames[uCnt]);

                        // Get the index of the current value
                        uCnt = (UINT) (strchrW (wszQuadDTAllow, cQuadDT_CWS) - wszQuadDTAllow);

                        // Select the current value
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_DT_CB), CB_SETCURSEL, uCnt, 0);

                        //***************************************************************
                        // []ELX -- CLEAR WS Values
                        //***************************************************************

                        // Lock the memory to get a ptr to it
                        lpMemChr = MyGlobalLockVar (hGlbQuadELX_CWS);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemChr)
                        // Get the NELM
                        aplNELM = lpHeader->NELM;
#undef  lpHeader
                        // Skip over the header and dimension to the data
                        lpMemChr = VarArrayDataFmBase (lpMemChr);

                        // Copy to temp string so we can zero-terminate it
                        CopyMemoryW (lpwszGlbTemp, lpMemChr, (APLU3264) aplNELM);

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbQuadELX_CWS); lpMemChr = NULL;

                        // Ensure properly terminated
                        lpwszGlbTemp[aplNELM] = WC_EOS;

                        // Set the text
                        SetDlgItemTextW (hWndProp, IDC_CLEARWS_ELX_EC, lpwszGlbTemp);

                        //***************************************************************
                        // []FC -- CLEAR WS Values
                        //***************************************************************

                        // Lock the memory to get a ptr to it
                        lpMemChr = MyGlobalLockVar (hGlbQuadFC_CWS);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemChr)
                        // Get the NELM
                        aplNELM = lpHeader->NELM;
#undef  lpHeader
                        // Skip over the header and dimension to the data
                        lpMemChr = VarArrayDataFmBase (lpMemChr);

                        // Copy to temp string so we can zero-terminate it
                        CopyMemoryW (lpwszGlbTemp, lpMemChr, (APLU3264) aplNELM);

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbQuadFC_CWS); lpMemChr = NULL;

                        // Ensure properly terminated
                        lpwszGlbTemp[aplNELM] = WC_EOS;

                        // Set the text
                        SetDlgItemTextW (hWndProp, IDC_CLEARWS_FC_EC,  lpwszGlbTemp);

                        //***************************************************************
                        // []FEATURE -- CLEAR WS Values
                        //***************************************************************

                        // Get the window handle to the ComboBox of index names & values
                        hWndFEATURE_CB1 = GetDlgItem (hWndProp, IDC_CLEARWS_FEATURE_CB1);
                        hWndFEATURE_CB2 = GetDlgItem (hWndProp, IDC_CLEARWS_FEATURE_CB2);

                        // Insert the []FEATURE index names
                        for (uCnt = 0; uCnt < FEATURENDX_LENGTH; uCnt++)
                            SendMessageW (hWndFEATURE_CB1, CB_ADDSTRING, 0, (LPARAM) (HANDLE_PTR) featNames[uCnt].lpwszFeatureName);

                        // Insert the []FEATURE index values
                        for (uCnt = 0; uCnt < FEATUREVAL_LENGTH; uCnt++)
                            SendMessageW (hWndFEATURE_CB2, CB_ADDSTRING, 0, (LPARAM) (HANDLE_PTR) (uCnt ? L"TRUE" : L"FALSE"));

                        // Lock the memory to get a ptr to it
                        lpMemInt = MyGlobalLockVar (hGlbQuadFEATURE_CWS);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemInt)
                        // Get the NELM
                        aplNELM = lpHeader->NELM;
#undef  lpHeader
                        // Skip over the header and dimension to the data
                        lpMemInt = VarArrayDataFmBase (lpMemInt);

                        // Initialize the []FEATURE local values
                        for (uCnt = 0; uCnt < FEATURENDX_LENGTH; uCnt++)
                        if (uCnt < aplNELM)
                            featValues[uCnt] = lpMemInt[uCnt];
                        else
                            featValues[uCnt] = aplDefaultFEATURE[uCnt];

                        // Set the current selection to the first name
                        //   and its corresponding value
                        SendMessageW (hWndFEATURE_CB1, CB_SETCURSEL, 0, 0);
                        SendMessageW (hWndFEATURE_CB2, CB_SETCURSEL, (APLU3264) (featValues[0] - FEATUREVAL_MINVAL), 0);

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbQuadFEATURE_CWS); lpMemInt = NULL;

                        //***************************************************************
                        // []FPC -- CLEAR WS Values
                        //***************************************************************

                        // Get the []FPC UpDown Control window handle
                        hWnd_UD = GetDlgItem (hWndProp, IDC_CLEARWS_FPC_UD);

                        // Set the range
                        SendMessageW (hWnd_UD, UDM_SETRANGE32, DEF_MIN_QUADFPC, DEF_MAX_QUADFPC);

                        // Set the initial position
                        SendMessageW (hWnd_UD, UDM_SETPOS32, 0, (LPARAM) uQuadFPC_CWS);

                        //***************************************************************
                        // []IC -- CLEAR WS Values
                        //***************************************************************

                        // Get the window handle to the ComboBox of index names & values
                        hWndIC_CB1 = GetDlgItem (hWndProp, IDC_CLEARWS_IC_CB1);
                        hWndIC_CB2 = GetDlgItem (hWndProp, IDC_CLEARWS_IC_CB2);

                        // Insert the []IC index names
                        for (uCnt = 0; uCnt < ICNDX_LENGTH; uCnt++)
                            SendMessageW (hWndIC_CB1, CB_ADDSTRING, 0, (LPARAM) (HANDLE_PTR) icIndexNames[uCnt]);

                        // Insert the []IC index values
                        for (uCnt = 0; uCnt < ICVAL_LENGTH; uCnt++)
                            SendMessageW (hWndIC_CB2, CB_ADDSTRING, 0, (LPARAM) (HANDLE_PTR) icIndexValues[uCnt]);

                        // Lock the memory to get a ptr to it
                        lpMemInt = MyGlobalLockVar (hGlbQuadIC_CWS);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemInt)
                        // Get the NELM
                        aplNELM = lpHeader->NELM;
#undef  lpHeader
                        // Skip over the header and dimension to the data
                        lpMemInt = VarArrayDataFmBase (lpMemInt);

                        // Initialize the []IC local values
                        for (uCnt = 0; uCnt < ICNDX_LENGTH; uCnt++)
                        if (uCnt < aplNELM)
                            icValues[uCnt] = lpMemInt[uCnt];
                        else
                            icValues[uCnt] = aplDefaultIC[uCnt];

                        // Set the current selection to the first name
                        //   and its corresponding value
                        SendMessageW (hWndIC_CB1, CB_SETCURSEL, 0, 0);
                        SendMessageW (hWndIC_CB2, CB_SETCURSEL, (APLU3264) (icValues[0] - ICVAL_MINVAL), 0);

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbQuadIC_CWS); lpMemInt = NULL;

                        //***************************************************************
                        // []IO -- CLEAR WS Values
                        //***************************************************************

                        // Get the []IO UpDown Control window handle
                        hWnd_UD = GetDlgItem (hWndProp, IDC_CLEARWS_IO_UD);

                        // Set the range
                        SendMessageW (hWnd_UD, UDM_SETRANGE32, DEF_MIN_QUADIO,  DEF_MAX_QUADIO );

                        // Set the initial position
                        SendMessageW (hWnd_UD, UDM_SETPOS32, 0, bQuadIO_CWS);

                        //***************************************************************
                        // []LX -- CLEAR WS Values
                        //***************************************************************

                        // Lock the memory to get a ptr to it
                        lpMemChr = MyGlobalLockVar (hGlbQuadLX_CWS);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemChr)
                        // Get the NELM
                        aplNELM = lpHeader->NELM;
#undef  lpHeader
                        // Skip over the header and dimension to the data
                        lpMemChr = VarArrayDataFmBase (lpMemChr);

                        // Copy to temp string so we can zero-terminate it
                        CopyMemoryW (lpwszGlbTemp, lpMemChr, (APLU3264) aplNELM);

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbQuadLX_CWS); lpMemChr = NULL;

                        // Ensure properly terminated
                        lpwszGlbTemp[aplNELM] = WC_EOS;

                        // Set the text
                        SetDlgItemTextW (hWndProp, IDC_CLEARWS_LX_EC,  lpwszGlbTemp);

                        //***************************************************************
                        // []MF -- CLEAR WS Values
                        //***************************************************************

                        // Format the value
                        lpaplChar =
                          FormatAplInt (lpwszGlbTemp,                   // Ptr to output save area
                                        uQuadMF_CWS);                   // The value to format
                        // Zap the trailing blank
                        lpaplChar[-1] = WC_EOS;

                        // Get the window handle for the Monitor Function combo box
                        hWndProp1 = GetDlgItem (hWndProp, IDC_CLEARWS_MF_CB);

                        // Append the possible values
                        SendMessageW (hWndProp1, CB_ADDSTRING, 0, (LPARAM) L"Millisecond Timer");
                        SendMessageW (hWndProp1, CB_ADDSTRING, 0, (LPARAM) L"High Resolution Timer");
                        SendMessageW (hWndProp1, CB_ADDSTRING, 0, (LPARAM) L"High Resolution Timer, scaled to ms");

                        // Set the current selection to the CLEAR WS value
                        SendMessageW (hWndProp1, CB_SETCURSEL, (APLU3264) uQuadMF_CWS - 1, 0);

                        //***************************************************************
                        // []PP -- CLEAR WS Values
                        //***************************************************************

                        // Get the []PP UpDown Control window handle
                        hWnd_UD = GetDlgItem (hWndProp, IDC_CLEARWS_PP_UD);

                        // Set the range
                        SendMessageW (hWnd_UD, UDM_SETRANGE32, DEF_MIN_QUADPP,  DEF_MAX_QUADPP_UDM);

                        // Set the initial position
                        SendMessageW (hWnd_UD, UDM_SETPOS32, 0, (LPARAM) uQuadPP_CWS);

                        //***************************************************************
                        // []PW -- CLEAR WS Values
                        //***************************************************************

                        // Get the []PW UpDown Control window handle
                        hWnd_UD = GetDlgItem (hWndProp, IDC_CLEARWS_PW_UD);

                        // Set the range
                        SendMessageW (hWnd_UD, UDM_SETRANGE32, DEF_MIN_QUADPW,  DEF_MAX_QUADPW );

                        // Set the initial position
                        SendMessageW (hWnd_UD, UDM_SETPOS32, 0, (LPARAM) uQuadPW_CWS);

                        //***************************************************************
                        // []RL -- CLEAR WS Values
                        //***************************************************************

                        // Format the value
                        lpaplChar =
                          FormatAplInt (lpwszGlbTemp,                   // Ptr to output save area
                                        uQuadRL_CWS);                   // The value to format
                        // Zap the trailing blank
                        lpaplChar[-1] = WC_EOS;

                        // Set the text
                        SetDlgItemTextW (hWndProp, IDC_CLEARWS_RL_EC,  lpwszGlbTemp);

                        break;
                    } // End IDD_PROPPAGE_CLEARWS_VALUES

                    case IDD_PROPPAGE_DIRS:                             // MYWM_INITDIALOG
                    {
                        HFONT    hFont;
                        LOGFONTW lf;

                        // Get the ComboBox window handle
                        hWndDirsComboBox = GetDlgItem (hWndProp, IDC_DIRS_CB_LIBDIRS);

                        // Get the ComboBox info
                        SendMessageW (hWndDirsComboBox, CB_GETCOMBOBOXINFO, 0, (LPARAM) &cbi);

                        // Lock the memory to get a ptr to it
                        lpwszLibDirs = MyGlobalLock (hGlbLibDirs);

                        // Populate the Combobox
                        for (uCnt = 0; uCnt < uNumLibDirs; uCnt++)
                            SendMessageW (hWndDirsComboBox, CB_ADDSTRING, 0, (LPARAM) lpwszLibDirs[uCnt]);

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbLibDirs); lpwszLibDirs = NULL;

                        // Set the text for the top instructions
                        SendMessageW (GetDlgItem (hWndProp, IDC_DIRS_LT1), WM_SETTEXT, 0, (LPARAM) lpwDirsText);

                        // Change the font for the "Library Directories" to bold

                        // Get the font used for the text
                        hFont = (HFONT)
                          SendMessageW (GetDlgItem (hWndProp, IDC_DIRS_LT2), WM_GETFONT, 0, 0);

                        // Get the current LOGFONTW struc and make it bold
                        GetObjectW (hFont, sizeof (lf), &lf);

                        // Set the new weight
                        lf.lfWeight = FW_BOLD;

                        // Create a new HFONT from the changed LOGFONTW
                        hFont = MyCreateFontIndirectW (&lf);

                        // Save as the new font for the text
                        SendMessageW (GetDlgItem (hWndProp, IDC_DIRS_LT2), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFont, MAKELPARAM (TRUE, 0));

                        // We no longer need this handle
                        DeleteObject (hFont); hFont = NULL;

                        break;
                    } // End IDD_PROPPAGE_DIRS

                    case IDD_PROPPAGE_FONTS:                            // MYWM_INITDIALOG
                        // Initialize the instructions
                        SetWindowTextW (GetDlgItem (hWndProp, IDC_FONTS_TEXT), lpwFontsText);

                        // Initialize
                        lpfnOldFontsStaticTextWndProc = NULL;

                        // Initialize the structs
                        for (uCnt = 0; uCnt < FONTENUM_LENGTH; uCnt++)
                        {
                            // Copy the global SameFontAs value to the local
                            lclSameFontAs[uCnt]      = glbSameFontAs[uCnt];

                            // Copy the global CHOOSEFONTW data to the local
                            fontStruc[uCnt].cfLcl    = *fontStruc[uCnt].lpcf;

                            // Clear the changed and applied flags
                            fontStruc[uCnt].bChanged =
                            fontStruc[uCnt].bApplied = FALSE;

                            // Set the owner to hWndProp so we can't close this dialog with ChooseFontW open
                            fontStruc[uCnt].cfLcl.hwndOwner = hWndProp;

                            // Set the radio button initial states
                            SendMessageW (GetDlgItem (hWndProp, FontsRadioPtr[uCnt][glbSameFontAs[uCnt]]), BM_SETCHECK, TRUE, 0);

                            // Simulate the user clicking the radio button
                            SendMessageW (hDlg, WM_COMMAND, GET_WM_COMMAND_MPS (FontsRadioPtr[uCnt][glbSameFontAs[uCnt]], NULL, BN_CLICKED));

                            // Add a Tooltip for the Fonts button
                            // Fill in constant fields
                            tti.uFlags   = 0
                                         | TTF_IDISHWND
                                         | TTF_SUBCLASS
                                           ;
                            tti.hwnd     = hWndProp;
                            tti.uId      = (INT_PTR) GetDlgItem (hWndProp, IDC_FONTS1 + uCnt);
////////////////////////////tti.rect     =                      // Not used with TTF_IDISHWND
////////////////////////////tti.hinst    =                      // Not used except with string resources
                            tti.lpszText = LPSTR_TEXTCALLBACKW;
////////////////////////////tti.lParam   =                      // Not used by this code

                            // Register a tooltip for the Fonts button
                            SendMessageW (hWndTT, TTM_ADDTOOLW, 0, (LPARAM) &tti);

                            // Add a Tooltip for the Fonts static text
                            // Fill in constant fields
                            // Note we don't use TTF_SUBCLASS here because it doesn't work on Static Controls.
                            // Instead, we subclass the control ourselves and pass certain mouse events to the TT
                            //   via TTM_RELAYEVENT.
                            tti.uFlags   = 0
                                         | TTF_IDISHWND
                                           ;
                            tti.hwnd     = hWndProp;
                            tti.uId      = (INT_PTR) GetDlgItem (hWndProp, IDC_FONTS_LT1 + uCnt);
////////////////////////////tti.rect     =                      // Not used with TTF_IDISHWND
////////////////////////////tti.hinst    =                      // Not used except with string resources
                            tti.lpszText = LPSTR_TEXTCALLBACKW;
////////////////////////////tti.lParam   =                      // Not used by this code

                            // Register a tooltip for the Fonts static text
                            SendMessageW (hWndTT, TTM_ADDTOOLW, 0, (LPARAM) &tti);

                            // Subclass the Fonts static text so we can display a Tooltip
                            if (lpfnOldFontsStaticTextWndProc)
                                  SetWindowLongPtrW (GetDlgItem (hWndProp, IDC_FONTS_LT1 + uCnt),
                                                     GWLP_WNDPROC,
                                                     (APLU3264) (LONG_PTR) (WNDPROC) &LclFontsStaticTextWndProc);
                            else
                                (HANDLE_PTR) lpfnOldFontsStaticTextWndProc =
                                  SetWindowLongPtrW (GetDlgItem (hWndProp, IDC_FONTS_LT1 + uCnt),
                                                     GWLP_WNDPROC,
                                                     (APLU3264) (LONG_PTR) (WNDPROC) &LclFontsStaticTextWndProc);
                        } // End FOR

                        break;

                    case IDD_PROPPAGE_KEYBS:                            // MYWM_INITDIALOG
                    {
                        TCITEMW  tcItem = {0};              // TabCtrl item struc
                        RECT     rcKTC;                     // Client rect
                        UINT     uCnt;                      // Loop counter
                        NMHDR    nmhdr = {0};               // NMHDR for TCN_SELCHANGE
                        HFONT    hFontTmp;                  // HFONT for one of the TabCtrl pushboxes
                        LOGFONTW lfTmp;                     // LOGFONTW ...
                        int      iLogPixelsY;               // # vertical pixels per inch in the DC

                        // Get # vertical pixels per inch
                        iLogPixelsY = GetLogPixelsY (NULL);

                        // Make a copy of the active keyboard layout so we
                        //   may resore it in case the user abandon changes
                        aKeybLayoutOrig = aKeybLayoutAct;

                        // Lock the memory to get a ptr to it
                        lpGlbKeybLayouts = MyGlobalLock (hGlbKeybLayouts);

                        // Get its size
                        uKeybSize = MyGlobalSize (hGlbKeybLayouts);

                        // Make a local copy of the keyboard layouts
                        hLclKeybLayouts = DbgGlobalAlloc (GHND, uKeybSize);
                        if (!hLclKeybLayouts)
                        {
                            MessageBoxW (hWndProp, L"Unable to allocate memory for a local copy of the keyboard layouts.", lpwszAppName, MB_OK | MB_ICONERROR);

                            // We're done here
                            DestroyWindow (hDlg);
                        } // End IF

                        // Lock the memory to get a ptr to it
                        lpLclKeybLayouts = MyGlobalLock000 (hLclKeybLayouts);

                        // Copy the global layouts to local memory
                        CopyMemory (lpLclKeybLayouts, lpGlbKeybLayouts, uKeybSize);

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbKeybLayouts); lpGlbKeybLayouts = NULL;

                        //***************************************************************
                        //  Keyboard TabCtrl
                        //***************************************************************

                        // Initialize the Tab Ctrl item struct
                        tcItem.mask       = TCIF_TEXT | TCIF_PARAM;
////////////////////////tcItem.pszText    =                         // Set below
////////////////////////tcItem.cchTextMax =                         // Not used with TCM_INSERTITEM
////////////////////////tcItem.iImage     =                         // Used with TCIF_IMAGE only
                        tcItem.lParam     = 0;

                        // Get the TabCtrl window handle
                        hWndKTC = GetDlgItem (hWndProp, IDC_KEYB_TC);

                        // Initialize the Tab Ctrl items
                        for (uCnt = 0; uCnt < countof (aKTC); uCnt++)
                        {
                            tcItem.pszText = aKTC[uCnt].lpwName;
                            tcItem.lParam  = (LPARAM) aKTC[uCnt].lpwText;
                            SendMessageW (hWndKTC, TCM_INSERTITEMW, uCnt, (LPARAM) &tcItem);
                        } // End FOR

                        // Get the TabCtrl size
                        GetClientRect (hWndKTC, &rcKTC);

                        // Size each tab to one-third the TC width ("- 2" is to account for margins)
                        SendMessageW (hWndKTC, TCM_SETITEMSIZE, 0, MAKELPARAM (rcKTC.right / 3 - 2, 0));

                        // Set the current selection
                        TabCtrl_SetCurSel (hWndKTC, uKeybTCNum);

                        // Fill in the NMHDR struc
////////////////////////nmhdr.hwndFrom =
////////////////////////nmhdr.idFrom   =
                        nmhdr.code     = TCN_SELCHANGE;

                        // Tell the control to set the current text
                        SendMessageW (hDlg, WM_NOTIFY, 0, (LPARAM) &nmhdr);

                        //***************************************************************
                        //  Keyboard CheckBoxes
                        //***************************************************************

                        CheckDlgButton (hWndProp, IDC_KEYB_XB_ALT  , uKeybState & KS_ALT);
                        CheckDlgButton (hWndProp, IDC_KEYB_XB_CTRL , uKeybState & KS_CTRL);
                        CheckDlgButton (hWndProp, IDC_KEYB_XB_SHIFT, uKeybState & KS_SHIFT);

                        //***************************************************************
                        //  Keyboard Unicode Ctrls
                        //***************************************************************

                        // Tell the control about the Unicode bases
                        CheckDlgButton (hWndProp, IDC_KEYB_RB_DEC, uKeybUnibase EQ 10);
                        CheckDlgButton (hWndProp, IDC_KEYB_RB_HEX, uKeybUnibase EQ 16);

                        // Limit the text in the Unicode Edit Ctrls
                        SendMessageW (GetDlgItem (hWndProp, IDC_KEYB_EC_UNICODE), EM_SETLIMITTEXT, KEYB_UNICODE_LIMIT, 0);

                        // Save the last selected unicode values
                        uLclKeybChar = uKeybChar;

                        // Convert the last selected keyboard unicode character to a string
                        MySprintfW (wszStr,
                                    sizeof (wszStr),
                                    (uKeybUnibase EQ 10) ? L"%u" : L"%05X",   // The "5" here is KEYB_UNICODE_LIMIT
                                    uLclKeybChar);
                        // Initialize the value in the Unicode Edit Ctrl
                        SetWindowTextW (GetDlgItem (hWndProp, IDC_KEYB_EC_UNICODE), wszStr);

                        // Display the new value
                        DisplayKeybTCValue (hWndProp, -1, TRUE );

                        // Subclass the Unicode Edit Ctrl
                        //   so we can launder input
                        (HANDLE_PTR) lpfnOldKeybEditCtrlWndProc =
                          SetWindowLongPtrW (GetDlgItem (hWndProp, IDC_KEYB_EC_UNICODE),
                                             GWLP_WNDPROC,
                                             (APLU3264) (LONG_PTR) (WNDPROC) &LclKeybEditCtrlWndProc);

                        //***************************************************************
                        //  Keyboard Fonts
                        //***************************************************************

                        // Save the normal font handle
                        hFontTmp = (HFONT) SendMessageW (GetDlgItem (hWndProp, IDC_KEYB_BN_TC_00), WM_GETFONT, 0, 0);

                        // Get the current LOGFONTW struct and make it bold
                        GetObjectW (hFontTmp, sizeof (lfTmp), &lfTmp);

                        // Set the height
                        lfKB.lfHeight = -MulDiv (DEF_KBPTSIZE - 2, iLogPixelsY, 72);

                        // Set the font for the appropriate keyboard controls
                        //   from lfKB
                        SetKeybFont (hWndProp);

                        // Initialize the CHOOSEFONTW struc for IDC_KEYB_BN_FONT
                        cfKB.lStructSize    = sizeof (cfKB);
                        cfKB.hwndOwner      = hWndProp;
////////////////////////cfKB.hDC            =                           // Only w/CF_PRINTERFONTS
                        cfKB.lpLogFont      = &lfKB;
                        cfKB.iPointSize     = DEF_KBPTSIZE * 10;
                        cfKB.Flags          = 0
                                            | CF_APPLY
                                            | CF_ENABLEHOOK
                                            | CF_FORCEFONTEXIST
                                            | CF_INITTOLOGFONTSTRUCT
                                            | CF_SCREENFONTS
                                              ;
////////////////////////cfKB.rgbColors      =                           // Only w/CF_EFFECTS
                        cfKB.lCustData      = (LPARAM) NULL;            // Signal to use "Keyboard Font"
                        cfKB.lpfnHook       = &MyChooseFontHook;
////////////////////////cfKB.lpTemplateName =                           // Only w/CF_ENABLETEMPLATE
////////////////////////cfKB.hInstance      =                           // Only w/CF_ENABLETEMPLATE
////////////////////////cfKB.lpszStyle      =                           // Only w/CF_USESTYLE
////////////////////////cfKB.nFontType      =                           // Output only
////////////////////////cfKB.nSizeMin       =                           // Only w/CF_LIMITSIZE
////////////////////////cfKB.nSizeMax       =                           // Only w/CF_LIMITSIZE

                        // Add a Tooltip to the Font button
                        // Fill in constant fields
                        tti.uFlags   = 0
                                     | TTF_IDISHWND
                                     | TTF_SUBCLASS
                                       ;
                        tti.hwnd     = hWndProp;
                        tti.uId      = (INT_PTR) GetDlgItem (hWndProp, IDC_KEYB_BN_FONT);
////////////////////////tti.rect     =                      // Not used with TTF_IDISHWND
////////////////////////tti.hinst    =                      // Not used except with string resources
                        tti.lpszText = LPSTR_TEXTCALLBACKW;
////////////////////////tti.lParam   =                      // Not used by this code

                        // Register a tooltip for the Keyboard Fonts button
                        SendMessageW (hWndTT, TTM_ADDTOOLW, 0, (LPARAM) &tti);

                        //***************************************************************
                        //  Keyboard Keycaps
                        //***************************************************************

                        // If the theme library is loaded, ...
                        if (bThemeLibLoaded)
                        {
                            // Open the default theme for Keybs
                            hThemeKeybs = zOpenThemeData (GetDlgItem (hWndProp, IDC_KEYB_BN_KC_00), WC_BUTTONW);

                            // Mark whether or not it succeeded
                            bThemedKeybs = (hThemeKeybs NE NULL);
                        } // End IF

                        //***************************************************************
                        //  Keyboard Layouts
                        //***************************************************************

                        // Save the Keyboard Layout ComboBox window handle
                        hWndKeybComboBox = GetDlgItem (hWndProp, IDC_KEYB_CB_LAYOUT);

                        // Make a local copy of the global values
                        uLclKeybLayoutNumAct = uGlbKeybLayoutNumAct;
                        uLclKeybLayoutNumVis = uGlbKeybLayoutNumVis;
                        uLclKeybLayoutCount  = uGlbKeybLayoutCount ;
                        uLclKeybLayoutUser   = uGlbKeybLayoutUser  ;
                        MyStrcpyW (wszLclKeybLayoutAct, sizeof (wszLclKeybLayoutAct), wszGlbKeybLayoutAct);
                        MyStrcpyW (wszLclKeybLayoutVis, sizeof (wszLclKeybLayoutVis), wszGlbKeybLayoutVis);

                        // Loop through the Keyboard Layouts
                        for (uCnt = 0; uCnt < uLclKeybLayoutCount; uCnt++)
                            // Add the layout names
                            SendMessageW (hWndKeybComboBox, CB_ADDSTRING, 0, (LPARAM) lpLclKeybLayouts[uCnt].wszLayoutName);

                        // Get the CB index of the visible selection
                        uCnt = (UINT)
                          SendMessageW (hWndKeybComboBox, CB_FINDSTRING, -1, (LPARAM) wszLclKeybLayoutVis);

                        // Select the visible one
                        SendMessageW (hWndKeybComboBox, CB_SETCURSEL, uCnt, 0);

                        // Display it
                        DispKeybLayout (hWndProp);

                        // Add a Tooltip to the Combobox
                        // Fill in constant fields
                        tti.uFlags   = 0
                                     | TTF_IDISHWND
                                     | TTF_SUBCLASS
                                       ;
                        tti.hwnd     = hWndProp;
                        tti.uId      = (INT_PTR) hWndKeybComboBox;
////////////////////////tti.rect     =                      // Not used with TTF_IDISHWND
////////////////////////tti.hinst    =                      // Not used except with string resources
                        tti.lpszText = LPSTR_TEXTCALLBACKW;
////////////////////////tti.lParam   =                      // Not used by this code

                        // Set the maximum tip width
                        SetMaxTipWidth (hWndTT);

                        // Register a tooltip for the Keyboard Layout Combobox
                        SendMessageW (hWndTT, TTM_ADDTOOLW, 0, (LPARAM) &tti);
#ifdef DEBUG
                        // Add a tooltip for each of the keycaps
                        for (uCnt = 0; uCnt < KKC_CY; uCnt++)
                        for (uCol = aKKC_IDC_BEG[uCnt]; uCol <= aKKC_IDC_END[uCnt]; uCol++)
                        {
                            tti.uId    = (INT_PTR) (GetDlgItem (hWndProp, uCol));
                            tti.lParam = uCol;
                            SendMessageW (hWndTT, TTM_ADDTOOLW, 0, (LPARAM) &tti);
                        } // End FOR/FOR

                        // Mark as not using tti.lParam
                        tti.lParam = 0;
#else
                        // Add a Tooltip for the special Keycaps
                        tti.uId      = (INT_PTR) (hWndKeycapC = GetDlgItem (hWndProp, KeybScanCodeToID (KeybCharToScanCode (L'c', KS_NONE))));
                        SendMessageW (hWndTT, TTM_ADDTOOLW, 0, (LPARAM) &tti);
                        tti.uId      = (INT_PTR) (hWndKeycapX = GetDlgItem (hWndProp, KeybScanCodeToID (KeybCharToScanCode (L'x', KS_NONE))));
                        SendMessageW (hWndTT, TTM_ADDTOOLW, 0, (LPARAM) &tti);
                        tti.uId      = (INT_PTR) (hWndKeycapV = GetDlgItem (hWndProp, KeybScanCodeToID (KeybCharToScanCode (L'v', KS_NONE))));
                        SendMessageW (hWndTT, TTM_ADDTOOLW, 0, (LPARAM) &tti);

                        tti.uId      = (INT_PTR) (hWndKeycapZ = GetDlgItem (hWndProp, KeybScanCodeToID (KeybCharToScanCode (L'z', KS_NONE))));
                        SendMessageW (hWndTT, TTM_ADDTOOLW, 0, (LPARAM) &tti);
                        tti.uId      = (INT_PTR) (hWndKeycapY = GetDlgItem (hWndProp, KeybScanCodeToID (KeybCharToScanCode (L'y', KS_NONE))));
                        SendMessageW (hWndTT, TTM_ADDTOOLW, 0, (LPARAM) &tti);

                        tti.uId      = (INT_PTR) (hWndKeycapS = GetDlgItem (hWndProp, KeybScanCodeToID (KeybCharToScanCode (L's', KS_NONE))));
                        SendMessageW (hWndTT, TTM_ADDTOOLW, 0, (LPARAM) &tti);
                        tti.uId      = (INT_PTR) (hWndKeycapE = GetDlgItem (hWndProp, KeybScanCodeToID (KeybCharToScanCode (L'e', KS_NONE))));
                        SendMessageW (hWndTT, TTM_ADDTOOLW, 0, (LPARAM) &tti);
                        tti.uId      = (INT_PTR) (hWndKeycapQ = GetDlgItem (hWndProp, KeybScanCodeToID (KeybCharToScanCode (L'q', KS_NONE))));
                        SendMessageW (hWndTT, TTM_ADDTOOLW, 0, (LPARAM) &tti);
#endif
                        break;
                    } // End IDD_PROPPAGE_KEYBS

                    case IDD_PROPPAGE_RANGE_LIMITS:                     // MYWM_INITDIALOG
                        // Check the boxes of Range Limited system vars
                        CheckDlgButton (hWndProp, IDC_RANGE_XB_CT      , bRangeLimit.CT      );
                        CheckDlgButton (hWndProp, IDC_RANGE_XB_FEATURE , bRangeLimit.FEATURE );
                        CheckDlgButton (hWndProp, IDC_RANGE_XB_IC      , bRangeLimit.IC      );
                        CheckDlgButton (hWndProp, IDC_RANGE_XB_IO      , bRangeLimit.IO      );
                        CheckDlgButton (hWndProp, IDC_RANGE_XB_PP      , bRangeLimit.PP      );
                        CheckDlgButton (hWndProp, IDC_RANGE_XB_PW      , bRangeLimit.PW      );
                        CheckDlgButton (hWndProp, IDC_RANGE_XB_RL      , bRangeLimit.RL      );

                        break;

                    case IDD_PROPPAGE_SYNTAX_COLORING:                  // MYWM_INITDIALOG
                        // Copy the global Foreground and Background colors to the local var
                        for (uCnt = 0; uCnt < SC_LENGTH; uCnt++)
                            lclSyntaxColors[uCnt] = gSyntaxColorName[uCnt].syntClr;

                        // Fill in constant fields
                        tti.uFlags   = 0
                                     | TTF_IDISHWND
                                     | TTF_SUBCLASS
                                       ;
                        tti.hwnd     = hDlg;
////////////////////////tti.uId      =                      // Filled in below
////////////////////////tti.rect     =                      // Not used with TTF_IDISHWND
////////////////////////tti.hinst    =                      // Not used except with string resources
                        tti.lpszText = LPSTR_TEXTCALLBACKW;
////////////////////////tti.lParam   =                      // Not used by this code

                        // Loop through the Foreground and Background buttons
                        for (uCnt = 0; uCnt < SC_LENGTH; uCnt++)
                        {
                            // Write out the static text
                            SendDlgItemMessageW (hWndProp,
                                                 IDC_SYNTCLR_LT1 + uCnt,
                                                 WM_SETTEXT,
                                                 0,
                                                 (LPARAM) (HANDLE_PTR) gSyntaxColorName[uCnt].lpwSCName);
                            // Fill in the dynamic field
                            tti.uId = (INT_PTR)
                              GetDlgItem (hWndProp, IDC_SYNTCLR_BN_FGCLR1 + uCnt);

                            // Register a tooltip for the Syntax Coloring Foreground button
                            SendMessageW (hWndTT,
                                          TTM_ADDTOOLW,
                                          0,
                                          (LPARAM) (LPTOOLINFOW) &tti);
                            // Fill in the dynamic field
                            tti.uId = (INT_PTR)
                              GetDlgItem (hWndProp, IDC_SYNTCLR_BN_BGCLR1 + uCnt);

                            // Register a tooltip for the Syntax Coloring Background button
                            SendMessageW (hWndTT,
                                          TTM_ADDTOOLW,
                                          0,
                                          (LPARAM) (LPTOOLINFOW) &tti);
                        } // End FOR

                        // The color buttons are initialized in WM_DRAWITEM

                        // Check the "Background Transparent" boxes
                        //   and show/hide the Background button window
                        for (uCnt = 0; uCnt < SC_LENGTH; uCnt++)
                        {
                            // Get the show/hide state
                            uShow = gSyntClrBGTrans[uCnt] ? SW_HIDE : SW_SHOWNORMAL;

                            // Show/hide the Background Color button
                            ShowWindow (GetDlgItem (hWndProp, IDC_SYNTCLR_BN_BGCLR1 + uCnt), uShow);

                            // The Window text label forces the background to be displayed,
                            //   hence there is no checkbox to disable it
                            if (uCnt NE SC_WINTEXT)
                                // Set the "Background Transparent" button initial states
                                CheckDlgButton (hWndProp, IDC_SYNTCLR_XB_TRANS1 + uCnt, gSyntClrBGTrans[uCnt]);
                        } // End FOR

                        // Check the "Enable ... Coloring" boxes
                        CheckDlgButton (hWndProp, IDC_SYNTCLR_XB_CLRFCNS, OptionFlags.bSyntClrFcns);
                        CheckDlgButton (hWndProp, IDC_SYNTCLR_XB_CLRSESS, OptionFlags.bSyntClrSess);
                        CheckDlgButton (hWndProp, IDC_SYNTCLR_XB_CLRPRNT, OptionFlags.bSyntClrPrnt);

                        // Fill in the static members of the CHOOSECOLORW struc
                        cc.lStructSize    = sizeof (cc);
                        cc.hwndOwner      = hWndProp;
////////////////////////cc.hInstance      = NULL;                   // Already zero from = {0}
////////////////////////cc.rgbResult      =                         // Filled in by the WM_COMMAND handler
                        cc.lpCustColors   = aCustomColors;
                        cc.Flags          = CC_RGBINIT | CC_FULLOPEN;
////////////////////////cc.lCustData      = 0;                      // Already zero from = {0}
////////////////////////cc.lpfnHook       = NULL;                   // Already zero from = {0}
////////////////////////cc.lpfnHook       = NULL;                   // Already zero from = {0}
////////////////////////cc.lpTemplateName = NULL;                   // Already zero from = {0}

                        break;

                    case IDD_PROPPAGE_SYSTEM_VAR_RESET:                 // MYWM_INITDIALOG
                        // Initialize the instructions
                        SetWindowTextW (GetDlgItem (hWndProp, IDC_RESET_TEXT), lpwResetText);

                        // Initialize the local copy of bResetVars
                        lclResetVars = bResetVars;

                        // Set the radio button initial states
                        SendMessageW (GetDlgItem (hWndProp, ResetRadioPtr[0][lclResetVars.CT     ]), BM_SETCHECK, TRUE, 0);
                        SendMessageW (GetDlgItem (hWndProp, ResetRadioPtr[1][lclResetVars.DT     ]), BM_SETCHECK, TRUE, 0);
                        SendMessageW (GetDlgItem (hWndProp, ResetRadioPtr[2][lclResetVars.FC     ]), BM_SETCHECK, TRUE, 0);
                        SendMessageW (GetDlgItem (hWndProp, ResetRadioPtr[3][lclResetVars.FEATURE]), BM_SETCHECK, TRUE, 0);
                        SendMessageW (GetDlgItem (hWndProp, ResetRadioPtr[4][lclResetVars.FPC    ]), BM_SETCHECK, TRUE, 0);
                        SendMessageW (GetDlgItem (hWndProp, ResetRadioPtr[5][lclResetVars.IC     ]), BM_SETCHECK, TRUE, 0);
                        SendMessageW (GetDlgItem (hWndProp, ResetRadioPtr[6][lclResetVars.IO     ]), BM_SETCHECK, TRUE, 0);
                        SendMessageW (GetDlgItem (hWndProp, ResetRadioPtr[7][lclResetVars.PP     ]), BM_SETCHECK, TRUE, 0);
                        SendMessageW (GetDlgItem (hWndProp, ResetRadioPtr[8][lclResetVars.PW     ]), BM_SETCHECK, TRUE, 0);
                        SendMessageW (GetDlgItem (hWndProp, ResetRadioPtr[9][lclResetVars.RL     ]), BM_SETCHECK, TRUE, 0);

                        break;

////////////////////case IDD_PROPPAGE_TAB_COLORS:                       // MYWM_INITDIALOG
////////////////////    DbgBrk ();      // ***FINISHME***
////////////////////
////////////////////
////////////////////
////////////////////
////////////////////
////////////////////
////////////////////    break;

                    case IDD_PROPPAGE_USER_PREFS:                       // MYWM_INITDIALOG
                        // Check the boxes of User Preferences
                        CheckDlgButton (hWndProp, IDC_USER_PREFS_XB_ADJUSTPW           , OptionFlags.bAdjustPW           );
                        CheckDlgButton (hWndProp, IDC_USER_PREFS_XB_UNDERBARTOLOWERCASE, OptionFlags.bUnderbarToLowercase);
                        CheckDlgButton (hWndProp, IDC_USER_PREFS_XB_NEWTABONCLEAR      , OptionFlags.bNewTabOnClear      );
                        CheckDlgButton (hWndProp, IDC_USER_PREFS_XB_NEWTABONLOAD       , OptionFlags.bNewTabOnLoad       );
                        CheckDlgButton (hWndProp, IDC_USER_PREFS_XB_USELOCALTIME       , OptionFlags.bUseLocalTime       );
                        CheckDlgButton (hWndProp, IDC_USER_PREFS_XB_BACKUPONLOAD       , OptionFlags.bBackupOnLoad       );
                        CheckDlgButton (hWndProp, IDC_USER_PREFS_XB_BACKUPONSAVE       , OptionFlags.bBackupOnSave       );
                        CheckDlgButton (hWndProp, IDC_USER_PREFS_XB_NOCOPYRIGHTMSG     , OptionFlags.bNoCopyrightMsg     );
                        CheckDlgButton (hWndProp, IDC_USER_PREFS_XB_CHECKGROUP         , OptionFlags.bCheckGroup         );
                        CheckDlgButton (hWndProp, IDC_USER_PREFS_XB_INSSTATE           , OptionFlags.bInsState           );
                        CheckDlgButton (hWndProp, IDC_USER_PREFS_XB_REVDBLCLK          , OptionFlags.bRevDblClk          );
                        CheckDlgButton (hWndProp, IDC_USER_PREFS_XB_DEFDISPFCNLINENUMS , OptionFlags.bDefDispFcnLineNums );
                        CheckDlgButton (hWndProp, IDC_USER_PREFS_XB_DISPMPSUF          , OptionFlags.bDispMPSuf          );
                        CheckDlgButton (hWndProp, IDC_USER_PREFS_XB_OUTPUTDEBUG        , OptionFlags.bOutputDebug        );

                        // If any of the Fallback fonts are not available, ...
                        if (hFontFB_SM    EQ NULL
                         || hFontFB_FE    EQ NULL
                         || hFontFB_PR_SM EQ NULL
                         || hFontFB_PR_FE EQ NULL)
                            // Grey out the checkbox
                            EnableWindow (GetDlgItem (hWndProp, IDC_USER_PREFS_XB_OUTPUTDEBUG), FALSE);

                        // Tell the control about the Unicode bases
                        CheckDlgButton (hWndProp, IDC_USER_PREFS_RB_DEC, uUserUnibase EQ 10);
                        CheckDlgButton (hWndProp, IDC_USER_PREFS_RB_HEX, uUserUnibase EQ 16);

                        // Limit the text in the Unicode Edit Ctrls
                        SendMessageW (GetDlgItem (hWndProp, IDC_USER_PREFS_EC_UNICODE), EM_SETLIMITTEXT, KEYB_UNICODE_LIMIT, 0);
                        SendMessageW (GetDlgItem (hWndProp, IDC_USER_PREFS_BN_UNICODE), EM_SETLIMITTEXT,                  1, 0);

                        // Save the last selected unicode values
                        uLclUserChar = uUserChar;

////////////////////////// Convert the Line Continuation Marker to a string
////////////////////////MySprintfW (wszStr,
////////////////////////            sizeof (wszStr),
////////////////////////            (uUserUnibase EQ 10) ? L"%u" : L"%05X",   // The "5" here is KEYB_UNICODE_LIMIT
////////////////////////            uLclUserChar);
                        // Initialize the value in the Unicode Edit Ctrl
////////////////////////SetWindowTextW (GetDlgItem (hWndProp, IDC_USER_PREFS_EC_UNICODE), wszStr);
                        SetWindowTextW (GetDlgItem (hWndProp, IDC_USER_PREFS_BN_UNICODE), (LPWCHAR) &uLclUserChar);

                        // Display the new value
                        DisplayKeybTCValue (hWndProp, -1, FALSE);

                        (HANDLE_PTR) lpfnOldKeybEditCtrlWndProc =
                          SetWindowLongPtrW (GetDlgItem (hWndProp, IDC_USER_PREFS_EC_UNICODE),
                                             GWLP_WNDPROC,
                                             (APLU3264) (LONG_PTR) (WNDPROC) &LclKeybEditCtrlWndProc);
                        // ***FIXME*** -- Make these work so we don't have to gray out the choices
                        {
                            CheckDlgButton (hWndProp, IDC_USER_PREFS_XB_NEWTABONCLEAR      , TRUE                            );
                            CheckDlgButton (hWndProp, IDC_USER_PREFS_XB_NEWTABONLOAD       , TRUE                            );

                            EnableWindow (GetDlgItem (hWndProp, IDC_USER_PREFS_XB_NEWTABONCLEAR), FALSE);
                            EnableWindow (GetDlgItem (hWndProp, IDC_USER_PREFS_XB_NEWTABONLOAD ), FALSE);
                        } // End ***FIXME***

                        // Get the window handle for the Paste & Copy combo boxes
                        hWndProp1 = GetDlgItem (hWndProp, IDC_USER_PREFS_CB_DEFAULTPASTE);
                        hWndProp2 = GetDlgItem (hWndProp, IDC_USER_PREFS_CB_DEFAULTCOPY);

                        // Loop through the Unitrans Copy/Paste options
                        for (uCnt = 0; uCnt < UNITRANS_STR_LENGTH; uCnt++)
                        {
                            if (unitransStr[uCnt].bValidPaste)
                                SendMessageW (hWndProp1, CB_ADDSTRING, 0, (LPARAM) unitransStr[uCnt].lpwChar);
                            if (unitransStr[uCnt].bValidCopy)
                                SendMessageW (hWndProp2, CB_ADDSTRING, 0, (LPARAM) unitransStr[uCnt].lpwChar);
                        } // End FOR

                        // Set the current selection to the user preference value
                        SendMessageW (hWndProp1, CB_SETCURSEL, unitransStr[OptionFlags.uDefaultPaste].IncTransPaste, 0);
                        SendMessageW (hWndProp2, CB_SETCURSEL, unitransStr[OptionFlags.uDefaultCopy ].IncTransCopy , 0);

                        // Get the window handle for the Update Frequency combo box
                        hWndProp1 = GetDlgItem (hWndProp, IDC_USER_PREFS_CB_UPDFRQ);

                        // Populate the Update Frequency ComboBox
                        for (uCnt = 0; uCnt < countof (updFrq); uCnt++)
                            SendMessageW (hWndProp1, CB_ADDSTRING, 0, (LPARAM) updFrq[uCnt].lpwsz);

                        // Set the current selection to the user preference value
                        SendMessageW (hWndProp1, CB_SELECTSTRING, -1, (LPARAM) gszUpdFrq);

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Ensure that the OK button is still the default push button
                uStyle = GetWindowLong (GetDlgItem (hDlg, IDOK), GWL_STYLE);
                SendMessageW (GetDlgItem (hDlg, IDOK), BM_SETSTYLE, uStyle | BS_DEFPUSHBUTTON, TRUE);

                // Restore the Apply button enable/disable state
                EnableWindow (hWndApply, bApply);
            } // End IF

            // Return dialog result
            DlgMsgDone (hDlg);              // We handled the msg

#define ctrlId      ((UINT) wParam)
#define bEnable     ((UBOOL) lParam)
        case MYWM_ENABLECHOOSEFONT:                     // ctrlId  = (UINT) wParam;
                                                        // bEnable = (UBOOL) lParam;
            // Get the associated item data (window handle of the Property Page)
            hWndProp = (HWND)
              SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_FONTS - IDD_PROPPAGE_START, 0);

            // Get the SW_xxx flag
            uShow = bEnable ? SW_SHOWNORMAL : SW_HIDE;

            // Show/Hide the radio buttons under the endpoint
            for (uSel = ctrlId - IDC_FONTS1, uCnt = uSel + 1; uCnt < FONTENUM_LENGTH; uCnt++)
                ShowWindow (GetDlgItem (hWndProp, FontsRadioPtr[uCnt][uSel]), uShow);

            // Get the window handle of the ChooseFontW button
            hWndFont = GetDlgItem (hWndProp, ctrlId);

            // Show/Hide the ChooseFontW button
            ShowWindow (hWndFont, uShow);

            // Enable/disable the corresponding ChooseFontW button
            EnableWindow (hWndFont, bEnable);

            // Loop through the ending radio buttons to see if we should
            //   show/hide the radio buttons to the left
            for (uCnt = 1; uCnt < FONTENUM_LENGTH; uCnt++)
            {
                // If this ending button is checked, ...
                if (IsDlgButtonChecked (hWndProp, FontsRadioPtr[uCnt][uCnt]))
                {
                    // If there are checked radio buttons underneath, ...
                    for (uSel = uCnt + 1; uSel < FONTENUM_LENGTH; uSel++)
                    if (IsDlgButtonChecked (hWndProp, FontsRadioPtr[uSel][uCnt]))
                    {
                        // Loop through the radio buttons to the left of this ending button
                        for (uSel = 0; uSel < uCnt; uSel++)
                            // Hide the radio buttons to the left of this ending button
                            ShowWindow (GetDlgItem (hWndProp, FontsRadioPtr[uCnt][uSel]), SW_HIDE);
                        break;
                    } // End FOR
                } else
                    uSel = FONTENUM_LENGTH;

                // If there were no checked radio buttons underneath, ...
                if (uSel EQ FONTENUM_LENGTH)
                {
                    // Loop through the radio buttons to the left of this ending button
                    for (uSel = 0; uSel < uCnt; uSel++)
                    // If the ending button for this column is checked, ...
                    if (IsDlgButtonChecked (hWndProp, FontsRadioPtr[uSel][uSel]))
                        // Show the radio button to the left of this ending button
                        ShowWindow (GetDlgItem (hWndProp, FontsRadioPtr[uCnt][uSel]), SW_SHOWNORMAL);
                } // End IF
            } // End FOR

            // Enable the Apply button
            EnableWindow (hWndApply, TRUE);

            // Return dialog result
            DlgMsgDone (hDlg);              // We handled the msg

#undef  bEnable
#undef  ctrlId

        case WM_MEASUREITEM:        // idCtl = (UINT) wParam;                // control identifier
        {                           // lpmis = (LPMEASUREITEMSTRUCT) lParam; // item-size information
#ifdef DEBUG
            UINT idCtl = (UINT) wParam;
            LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT) lParam;
#else
  #define idCtl       ((UINT) wParam)
  #define lpmis       ((LPMEASUREITEMSTRUCT) lParam)
#endif
            // Check to see if this is one of our menu items
            if (IDC_SYNTCLR_MI1 <= lpmis->itemID
             &&                    lpmis->itemID <= IDC_SYNTCLR_MI_LAST)
            {
                HDC  hDCClient;                         // Client Area DC
                SIZE sMenuItem;                         // Size of menu text
                UINT itemIndex;                         // Menu item index

                Assert (lpmis->CtlType EQ ODT_MENU);

                // Get the item index
                itemIndex = lpmis->itemID - IDC_SYNTCLR_MI1;

#define SYNTCLR_MENU_MARGIN_TOP      5
#define SYNTCLR_MENU_MARGIN_BOTTOM   5
#define SYNTCLR_MENU_MARGIN_LEFT     5
#define SYNTCLR_MENU_MARGIN_RIGHT    5
#define SYNTCLR_MENU_MARK_WIDTH     15
#define SYNTCLR_MENU_MARK_HEIGHT    10
#define SYNTCLR_MENU_SWATCH_WIDTH   25
#define SYNTCLR_MENU_SWATCH_HEIGHT  10
#define SYNTCLR_MENU_SWATCH_SEP      5          // Distance in pixels on either side of the swatch

                // Get a DC for the Client Area
                hDCClient = MyGetDC (hDlg);

                // Tell the DC to use this font
                SelectObject (hDCClient, GetStockObject (DEFAULT_GUI_FONT));

                // Get the size in pixels of the string
                GetTextExtentPoint32W (hDCClient,
                                       scMenuItems[itemIndex].lpwName,
                                       lstrlenW (scMenuItems[itemIndex].lpwName),
                                      &sMenuItem);
                // We no longer need this resource
                MyReleaseDC (hDlg, hDCClient); hDCClient = NULL;

                // Set the item width
                lpmis->itemWidth  = SYNTCLR_MENU_MARGIN_LEFT
                                  + SYNTCLR_MENU_MARK_WIDTH
                                  + SYNTCLR_MENU_SWATCH_SEP
                                  + SYNTCLR_MENU_SWATCH_WIDTH
                                  + SYNTCLR_MENU_SWATCH_SEP
                                  + sMenuItem.cx
                                  + SYNTCLR_MENU_MARGIN_RIGHT;

                // Set the item height
                lpmis->itemHeight = max (sMenuItem.cy, SYNTCLR_MENU_MARGIN_TOP
                                                     + SYNTCLR_MENU_SWATCH_HEIGHT
                                                     + SYNTCLR_MENU_MARGIN_BOTTOM);
                // Return dialog result
                DlgMsgDone (hDlg);              // We handled the msg
////////////} else
////////////// See if this is our Keyboard Layout ComboBox
////////////if (idCtl EQ IDC_KEYB_CB_LAYOUT)
////////////{
////////////    // Nothing to do
            } // End IF/ELSE

            break;                  // We didn't handle the msg
#ifndef DEBUG
  #undef  lpmis
  #undef  idCtl
#endif
        } // End WM_MEASUREITEM

        case WM_DRAWITEM:           // idCtl = (UINT) wParam;             // Control identifier
        {                           // lpdis = (LPDRAWITEMSTRUCT) lParam; // Item-drawing information
#ifdef DEBUG
            UINT             idCtl = (UINT) wParam;
            LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;
#else
  #define idCtl       ((UINT) wParam)
  #define lpdis       ((LPDRAWITEMSTRUCT) lParam)
#endif
            //***************************************************************
            // SYNTAX COLORING -- WM_DRAWITEM
            //***************************************************************

            // Check to see if this is one of our Syntax Coloring Foreground/Background Color buttons
            if ((IDC_SYNTCLR_BN_FGCLR1 <= idCtl && idCtl <= IDC_SYNTCLR_BN_FGCLR_LAST)
             || (IDC_SYNTCLR_BN_BGCLR1 <= idCtl && idCtl <= IDC_SYNTCLR_BN_BGCLR_LAST))
            {
                UBOOL bFore;                // TRUE iff Foreground color button

                Assert (lpdis->CtlType EQ ODT_BUTTON);

                // Determine if this is Foreground or Background
                bFore = (IDC_SYNTCLR_BN_FGCLR1 <= idCtl
                      &&                          idCtl <= IDC_SYNTCLR_BN_FGCLR_LAST);

                // Split cases based upon the item action
                switch (lpdis->itemAction)
                {
                    case ODA_DRAWENTIRE:
                    case ODA_SELECT:
                        // Draw the edge, fill with the color
                        FillSyntaxColor (lpdis, bFore ? lclSyntaxColors[idCtl - IDC_SYNTCLR_BN_FGCLR1].crFore
                                                      : lclSyntaxColors[idCtl - IDC_SYNTCLR_BN_BGCLR1].crBack);
                        break;

                    case ODA_FOCUS:     // Ignore changes in focus
                        break;
                } // End SWITCH
            } else
            // Check to see if this is one of our Syntax Coloring menu items
            if (IDC_SYNTCLR_MI1 <= lpdis->itemID && lpdis->itemID <= IDC_SYNTCLR_MI_LAST)
            {
                RECT     rcSwatch,              // Swatch rectangle
                         rcText,                // Text   ...
                         rcMark;                // Mark   ...
                HBRUSH   hBrush;                // BG/Swatch brush
                UINT     itemIndex;             // Menu item index
                COLORREF clrBack,               // Color of background
                         clrText;               // ...      text

                Assert (lpdis->CtlType EQ ODT_MENU);

                // Get the associated item data (window handle of the Property Page)
                hWndProp = (HWND)
                  SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_FONTS - IDD_PROPPAGE_START, 0);

                // Get the item index
                itemIndex = lpdis->itemID - IDC_SYNTCLR_MI1;

                // Copy the drawing rectangle
                rcSwatch = lpdis->rcItem;

                // Narrow the drawing rectangle for the swatch
                rcSwatch.left  += SYNTCLR_MENU_MARGIN_LEFT
                               +  SYNTCLR_MENU_MARK_WIDTH
                               +  SYNTCLR_MENU_SWATCH_SEP;
                rcSwatch.right  = rcSwatch.left + SYNTCLR_MENU_SWATCH_WIDTH;
                rcSwatch.top   += SYNTCLR_MENU_MARGIN_TOP;
                rcSwatch.bottom = rcSwatch.top + SYNTCLR_MENU_SWATCH_HEIGHT;

                // Split cases based upon the item action
                switch (lpdis->itemAction)
                {
                    case ODA_DRAWENTIRE:
                    case ODA_SELECT:
                        // If we're selected, ...
                        if (lpdis->itemState & ODS_SELECTED)
                        {
                            // Get a brush for the background
                            hBrush = MyCreateSolidBrush (GetSysColor (COLOR_HIGHLIGHT));

                            // Fill the client area background
                            FillRect (lpdis->hDC, &lpdis->rcItem, hBrush);

                            // We no longer need this resource
                            MyDeleteObject (hBrush); hBrush = NULL;

                            // Set the Foreground and Background colors
                            clrBack = SetBkColor   (lpdis->hDC, GetSysColor (COLOR_HIGHLIGHT    ));
                            clrText = SetTextColor (lpdis->hDC, GetSysColor (COLOR_HIGHLIGHTTEXT));
                        } else
                        {
                            // Get a brush for the background
                            hBrush = MyCreateSolidBrush (GetBkColor (lpdis->hDC));

                            // Fill the client area background
                            FillRect (lpdis->hDC, &lpdis->rcItem, hBrush);

                            // We no longer need this resource
                            MyDeleteObject (hBrush); hBrush = NULL;
                        } // End IF/ELSE

                        // If this is the selected color, ...
                        if (lpdis->itemData)
                        {
                            HDC     hDCMem;
                            HBITMAP hBitmapOld;

                            // Copy the drawing rectangle
                            rcMark = lpdis->rcItem;

                            // Narrow the drawing rectangle
                            rcMark.left += SYNTCLR_MENU_MARGIN_LEFT;
                            rcMark.right = rcMark.left + SYNTCLR_MENU_MARK_WIDTH;

                            // Get a Client Area DC for the bitmap
                            hDCMem = MyCreateCompatibleDC (lpdis->hDC);

                            // Select it into the DC
                            hBitmapOld = SelectObject (hDCMem, hBitmapCheck);

                            // Draw a bitmap
                            BitBlt (lpdis->hDC,
                                    rcMark.left,
                                    rcMark.top,
                                    bmCheck.bmWidth,
                                    bmCheck.bmHeight,
                                    hDCMem,
                                    0,
                                    0,
                                    SRCCOPY);
                            // Restore the previous bitmap
                            SelectObject (hDCMem, hBitmapOld);

                            // We no longer need this resource
                            MyDeleteDC (hDCMem);
                        } // End IF

                        // If this is not a special item, ...
                        if (itemIndex NE MI_CUSTOM
                         && itemIndex NE MI_WEBCOLORS)
                        {
                            // Get a brush for the swatch color
                            hBrush = MyCreateSolidBrush (scMenuItems[itemIndex].clrRef);

                            // Draw the color swatch
                            FillRect (lpdis->hDC, &rcSwatch, hBrush);

                            // We no longer need this resource
                            MyDeleteObject (hBrush); hBrush = NULL;

                            // Draw a border around the swatch
                            FrameRect (lpdis->hDC, &rcSwatch, GetStockObject (BLACK_BRUSH));
                        } // End IF

                        // Copy the drawing rectangle
                        rcText = lpdis->rcItem;

                        // Move over the rectangle for the text
                        rcText.left = rcSwatch.right + SYNTCLR_MENU_SWATCH_SEP;

                        // Draw the text
                        DrawTextW (lpdis->hDC,
                                   scMenuItems[itemIndex].lpwName,
                                   lstrlenW (scMenuItems[itemIndex].lpwName),
                                  &rcText,
                                   DT_VCENTER | DT_SINGLELINE);
                        // If we're selected, ...
                        if (lpdis->itemState & ODS_SELECTED)
                        {
                            // Reset the Foreground and Background colors
                             SetBkColor   (lpdis->hDC, clrBack);
                             SetTextColor (lpdis->hDC, clrText);
                        } // End IF

                        break;

                    case ODA_FOCUS:     // Ignore changes in focus
                        break;
                } // End SWITCH
            } else
            //***************************************************************
            // KEYBOARDS -- WM_DRAWITEM
            //***************************************************************

            // Check to see if this is one of our Keyboard Keycap buttons
            if (IDC_KEYB_BN_KC_00 <= idCtl
             &&                      idCtl <= IDC_KEYB_BN_KC_LAST)
            {
                UINT oldBkMode;

                Assert (lpdis->CtlType EQ ODT_BUTTON);

                // Get the associated item data (window handle of the Property Page)
                hWndProp = (HWND)
                  SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_KEYBS - IDD_PROPPAGE_START, 0);

                // Set the text background mode and save the old one
                oldBkMode = SetBkMode (lpdis->hDC, TRANSPARENT);

                // Paint the button border & background
                if (bThemedKeybs)
                    // Draw the theme background
                    zDrawThemeBackground (hThemeKeybs, lpdis->hDC, BP_PUSHBUTTON, PBS_NORMAL, &lpdis->rcItem, NULL);
                else
                    // Draw a border around the char
                    DrawEdge (lpdis->hDC, &lpdis->rcItem, EDGE_RAISED, BF_RECT | BF_SOFT | BF_FLAT | BF_MONO);

                // Get the row of the keycap
                for (uCnt = 0; uCnt < KKC_CY; uCnt++)
                if ((UINT) idCtl <= aKKC_IDC_END[uCnt])
                    break;

                // Get the scan code
                uScanCode = aKKC_SC[uCnt].aSC[idCtl - aKKC_IDC_BEG[uCnt]];

                // If the ScanCode is within range of our table, and
                //   the keystate is Ctrl, and
                //   we're using Ctrl-C, -X, -V, -Z, -Y
                //   and this ScanCode is one of those, ...
                if (uScanCode < lpLclKeybLayouts[uLclKeybLayoutNumVis].uCharCodesLen
                 && uKeybState EQ KS_CTRL
                 && ((lpLclKeybLayouts[uLclKeybLayoutNumVis].bUseCXV
                   && (uScanCode EQ KeybCharToScanCode (L'c', KS_NONE)
                    || uScanCode EQ KeybCharToScanCode (L'x', KS_NONE)
                    || uScanCode EQ KeybCharToScanCode (L'v', KS_NONE)))
                  || (lpLclKeybLayouts[uLclKeybLayoutNumVis].bUseZY
                   && (uScanCode EQ KeybCharToScanCode (L'z', KS_NONE)
                    || uScanCode EQ KeybCharToScanCode (L'y', KS_NONE)))
                  || (lpLclKeybLayouts[uLclKeybLayoutNumVis].bUseSEQ
                   && (uScanCode EQ KeybCharToScanCode (L's', KS_NONE)
                    || uScanCode EQ KeybCharToScanCode (L'e', KS_NONE)
                    || uScanCode EQ KeybCharToScanCode (L'q', KS_NONE)))))
                {
                    HPEN   hPen  , hOldPen;
                    HBRUSH         hOldBrush;
                    RECT   rc;

                    // Create a pen with which to draw the ellipse
                    hPen = MyCreatePen (PS_SOLID, 4, DEF_SCN_RED);
                    hOldPen = SelectObject (lpdis->hDC, hPen);

                    // Create a transparent brush with which to fill the ellipse
                    hOldBrush = SelectObject (lpdis->hDC, GetStockObject (NULL_BRUSH));

                    // Copy the rectangle
                    rc = lpdis->rcItem;

                    // Move the rectangle in a bit
                    rc.left   += 4;
                    rc.top    += 4;
                    rc.right  -= 4;
                    rc.bottom -= 4;

                    // Draw the ellipse
                    Ellipse (lpdis->hDC,
                             rc.left,
                             rc.top,
                             rc.right,
                             rc.bottom);
                    // Draw the line
                    MoveToEx (lpdis->hDC,
                              rc.left + 4,
                              rc.top  + 4,
                              NULL);
                    LineTo (lpdis->hDC,
                            rc.right  - 4,
                            rc.bottom - 4);
                    // Restore the old objects
                    SelectObject (lpdis->hDC, hOldBrush);
                    SelectObject (lpdis->hDC, hOldPen);

                    // Delete the object
                    MyDeleteObject (hPen); hPen = NULL;
                } else
                {
                    // If the ScanCode is within range of our table, ...
                    if (uScanCode < lpLclKeybLayouts[uLclKeybLayoutNumVis].uCharCodesLen)
                    {
                        // Get the char to draw
                        wszText[0] = lpLclKeybLayouts[uLclKeybLayoutNumVis].aCharCodes[uScanCode].wc[uKeybState];

                        // If it's empty, ...
                        if (wszText[0] EQ L'\0')
                            // Draw a blank
                            wszText[0] = L' ';
                    } else
                        // Draw a blank
                        wszText[0] = L' ';

                    // Draw the text (one char)
                    DrawTextW (lpdis->hDC,
                               wszText,
                               1,
                              &lpdis->rcItem,
                               DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
                } // End IF

                // Restore the old text background mode
                SetBkMode (lpdis->hDC, oldBkMode);
            } else
            // Check to see if this is one of our Keyboard TabCtrl buttons
            if (IDC_KEYB_BN_TC_00 <= idCtl
             &&                      idCtl <= IDC_KEYB_BN_TC_LAST)
            {
                Assert (lpdis->CtlType EQ ODT_BUTTON);

                // Get the associated item data (window handle of the Property Page)
                hWndProp = (HWND)
                  SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_KEYBS - IDD_PROPPAGE_START, 0);

                // Draw the button
                DrawButton (hWndListBox, hWndProp, lpdis, idCtl);
            } else
            // Check to see if this is our Keyboard Unicode button
            if (idCtl EQ IDC_KEYB_BN_UNICODE)
            {
                Assert (lpdis->CtlType EQ ODT_BUTTON);

                // Get the associated item data (window handle of the Property Page)
                hWndProp = (HWND)
                  SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_KEYBS - IDD_PROPPAGE_START, 0);

                // Draw the button
                DrawButton (hWndListBox, hWndProp, lpdis, idCtl);

                // Draw a border around the char
                DrawEdge (lpdis->hDC, &lpdis->rcItem, EDGE_SUNKEN, BF_RECT);
            } else
            // Check to see if this is our Keyboard Layout ComboBox
            if (idCtl EQ IDC_KEYB_CB_LAYOUT)
            {
                COLORREF clrBack,                   // Color of background
                         clrText;                   // ...      text
                WCHAR    wszTmpKeybLayoutName[KBLEN];  // Save area for keyboard layout name
                HBITMAP  hBitmapOBM;                // OBM bitmap handle
                BITMAP   bmOBM;                     // OBM bitmap info
                RECT     rcItem,                    // The item rectangle
                         rcOBM;                     // The OBM rectangle

                // Split cases based upon the item action
                switch (lpdis->itemAction)
                {
                    case ODA_DRAWENTIRE:
                    case ODA_SELECT:
                        // Get the text
                        SendMessageW (lpdis->hwndItem, CB_GETLBTEXT, lpdis->itemID, (LPARAM) wszTmpKeybLayoutName);

                        // Load the OBM check bitmap
                        hBitmapOBM = LoadBitmapA (NULL, MAKEINTRESOURCEA (OBM_CHECK));

                        // Get the width and height of the bitmap
                        GetObjectA (hBitmapOBM, sizeof (BITMAP), &bmOBM);

                        // Copy the item rectangle so we can change it
                        rcItem = lpdis->rcItem;

                        // Set the OBM rectangle for the checkmark
                        rcOBM.left   = rcItem.left;
                        rcOBM.top    = rcItem.top + ((rcItem.bottom - rcItem.top) - bmOBM.bmHeight) / 2;
                        rcOBM.right  = rcOBM.left + bmOBM.bmWidth;
                        rcOBM.bottom = rcOBM.top  + bmOBM.bmHeight;

                        // Move the left edge of the rectangle over to accommodate the checkmark
                        rcItem.left += bmOBM.bmWidth - 2;

                        // If this is the active layout, ...
                        if (lstrcmpW (wszTmpKeybLayoutName, wszLclKeybLayoutAct) EQ 0)
                            // Draw the checkmark
                            DrawBitmap (lpdis->hDC,
                                        hBitmapOBM,
                                        rcOBM.left,
                                        rcOBM.top);
                        // Find this layout's #
                        for (uCnt = 0; uCnt < uLclKeybLayoutCount; uCnt++)
                        // See if this is the visible keyboard layout
                        if (lstrcmpW (wszTmpKeybLayoutName, lpLclKeybLayouts[uCnt].wszLayoutName) EQ 0)
                            break;
                        // Set the OBM rectangle for the bullet
#define BULLET_WIDTH        6
#define BULLET_HEIGHT       6
                        rcOBM.left   = rcItem.left;
                        rcOBM.top    = rcItem.top + ((rcItem.bottom - rcItem.top) - BULLET_HEIGHT) / 2;
                        rcOBM.right  = rcOBM.left + BULLET_WIDTH;
                        rcOBM.bottom = rcOBM.top  + BULLET_HEIGHT;

                        // If this layout is read-only, ...
                        if (lpLclKeybLayouts[uCnt].bReadOnly)
                        {
                            HPEN   hPen  , hOldPen;
                            HBRUSH         hOldBrush;

                            // Create a pen with which to draw the ellipse
                            hPen = MyCreatePen (PS_SOLID, 1, DEF_SCN_BLACK);
                            hOldPen = SelectObject (lpdis->hDC, hPen);

                            // Create a black brush with which to fill the ellipse
                            hOldBrush = SelectObject (lpdis->hDC, GetStockObject (BLACK_BRUSH));

                            // Draw the bullet
                            Ellipse (lpdis->hDC,
                                     rcOBM.left,
                                     rcOBM.top,
                                     rcOBM.right,
                                     rcOBM.bottom);
                            // Restore the old objects
                            SelectObject (lpdis->hDC, hOldBrush);
                            SelectObject (lpdis->hDC, hOldPen);

                            // Delete the object
                            MyDeleteObject (hPen); hPen = NULL;
                        } // End IF

                        // Move the left edge of the rectangle over to accommodate the bullet
                        rcItem.left += 2 * BULLET_WIDTH;

                        // If we're selected, ...
                        if (lpdis->itemState & ODS_SELECTED)
                        {
                            // Set the Foreground and Background colors
                            clrBack = SetBkColor   (lpdis->hDC, GetSysColor (COLOR_HIGHLIGHT    ));
                            clrText = SetTextColor (lpdis->hDC, GetSysColor (COLOR_HIGHLIGHTTEXT));
                        } // End IF

                        // Draw the text
                        DrawTextW (lpdis->hDC,
                                   wszTmpKeybLayoutName,
                                   -1,
                                  &rcItem,
                                   DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
                        // If we're selected, ...
                        if (lpdis->itemState & ODS_SELECTED)
                        {
                            // Reset the Foreground and Background colors
                             SetBkColor   (lpdis->hDC, clrBack);
                             SetTextColor (lpdis->hDC, clrText);
                        } // End IF

                        // We no longer need this resource
                        DeleteObject (hBitmapOBM); hBitmapOBM = NULL;

                        break;

                    case ODA_FOCUS:     // Ignore changes in focus
                        break;
                } // End SWITCH
            } // End IF/ELSE/...

            // Return dialog result
            DlgMsgDone (hDlg);              // We handled the msg

#ifndef DEBUG
  #undef  lpdis
  #undef  idCtl
#endif
        } // End WM_DRAWITEM

        case WM_THEMECHANGED:
            // If the theme library is loaded, ...
            if (bThemeLibLoaded)
            {
                // If the active theme for Keybs is loaded, ...
                if (hThemeKeybs NE NULL)
                {
                    // Close the theme for Keybs
                    zCloseThemeData (hThemeKeybs); hThemeKeybs = NULL;
                } // End IF

                // Close the theme library
                FinThemes ();
            } // End IF

            // Initialize for themes
            InitThemes ();

            // If the theme library is loaded, ...
            if (bThemeLibLoaded)
            {
                // Get the associated item data (window handle of the Property Page)
                hWndProp = (HWND)
                  SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_KEYBS - IDD_PROPPAGE_START, 0);

                // Open the default theme for Keybs
                hThemeKeybs = zOpenThemeData (GetDlgItem (hWndProp, IDC_KEYB_BN_KC_00), WC_BUTTONW);

                // Mark as whether or not the active theme for Keybs is loaded
                bThemedKeybs = (hThemeKeybs NE NULL);
            } // End IF

            // Return dialog result
            DlgMsgDone (hDlg);              // We handled the msg

        case WM_NOTIFY:             // idCtl = (int) wParam;
        {                           // pnmh = (LPNMHDR) lParam;
#ifdef DEBUG
            APLI3264 idCtl = (APLI3264) wParam;
            LPNMHDR  lpnmh = (LPNMHDR) lParam;
#else
  #define idCtl   ((APLI3264) wParam)
  #define lpnmh   ((LPNMHDR) lParam)
#endif
            // Split cases based upon the notification code
            switch (lpnmh->code)
            {
                //***************************************************************
                // Tooltip Ctrl Notifications
                //***************************************************************
////////////////case TTN_NEEDTEXTW:
                case TTN_GETDISPINFOW:  // idCtl = (int) wParam;
                                        // lpttt = (LPTOOLTIPTEXTW) lParam;
                {
                    static WCHAR    TooltipText [_MAX_PATH];        // Save area for Tooltip text returned to caller
                           UBOOL    bFore;                          // TRUE iff Foreground color for Syntax Coloring
                           COLORREF clrRef;                         // Foreground or Background color for Syntax Coloring
                           int      iLogPixelsY;                    // # vertical pixels per inch in the DC
#ifdef DEBUG
                           LPTOOLTIPTEXTW lpttt = (LPTOOLTIPTEXTW) lParam;
#else
  #define lpttt   ((LPTOOLTIPTEXTW) lParam)
#endif
                    // Check to see if this is our Keyboard Layout Combobox
                    if (idCtl EQ (INT_PTR) hWndKeybComboBox)
                        // Return the a ptr to the text to the caller
                        lpttt->lpszText = L"A checkmark indicates the Active layout\nA diamond indicates a built-in (Read-only) layout";
                    else
#ifndef DEBUG
                    // Check to see if this is our Keycap letter C
                    if (idCtl EQ (INT_PTR) hWndKeycapC)
                    {
                        // If the keyboard state is displaying Ctrl-keys only
                        //   and this layout reserves that keystroke for the system, ...
                        if (uKeybState EQ KS_CTRL
                         && lpLclKeybLayouts[uLclKeybLayoutNumVis].bUseCXV)
                            lpttt->lpszText = L"Reserved for system use; click on \"Use Ctrl-Ins, ...\" to change";
                    } else
                    // Check to see if this is our Keycap letter X
                    if (idCtl EQ (INT_PTR) hWndKeycapX)
                    {
                        // If the keyboard state is displaying Ctrl-keys only
                        //   and this layout reserves that keystroke for the system, ...
                        if (uKeybState EQ KS_CTRL
                         && lpLclKeybLayouts[uLclKeybLayoutNumVis].bUseCXV)
                            lpttt->lpszText = L"Reserved for system use; click on \"Use Ctrl-Ins, ...\" to change";
                    } else
                    // Check to see if this is our Keycap letter V
                    if (idCtl EQ (INT_PTR) hWndKeycapV)
                    {
                        // If the keyboard state is displaying Ctrl-keys only
                        //   and this layout reserves that keystroke for the system, ...
                        if (uKeybState EQ KS_CTRL
                         && lpLclKeybLayouts[uLclKeybLayoutNumVis].bUseCXV)
                            lpttt->lpszText = L"Reserved for system use; click on \"Use Ctrl-Ins, ...\" to change";
                    } else
                    // Check to see if this is our Keycap letter Z
                    if (idCtl EQ (INT_PTR) hWndKeycapZ)
                    {
                        // If the keyboard state is displaying Ctrl-keys only
                        //   and this layout reserves that keystroke for the system, ...
                        if (uKeybState EQ KS_CTRL
                         && lpLclKeybLayouts[uLclKeybLayoutNumVis].bUseZY)
                            lpttt->lpszText = L"Reserved for system use; click on \"No keyboard shortcuts for Undo, Redo\" to change";
                    } else
                    // Check to see if this is our Keycap letter Y
                    if (idCtl EQ (INT_PTR) hWndKeycapY)
                    {
                        // If the keyboard state is displaying Ctrl-keys only
                        //   and this layout reserves that keystroke for the system, ...
                        if (uKeybState EQ KS_CTRL
                         && lpLclKeybLayouts[uLclKeybLayoutNumVis].bUseZY)
                            lpttt->lpszText = L"Reserved for system use; click on \"No keyboard shortcuts for Undo, Redo\" to change";
                    } else
                    // Check to see if this is our Keycap letter S
                    if (idCtl EQ (INT_PTR) hWndKeycapS)
                    {
                        // If the keyboard state is displaying Ctrl-keys only
                        //   and this layout reserves that keystroke for the system, ...
                        if (uKeybState EQ KS_CTRL
                         && lpLclKeybLayouts[uLclKeybLayoutNumVis].bUseSEQ)
                            lpttt->lpszText = L"Reserved for system use; click on \"No keyboard shortcuts for Function Editing commands\" to change";
                    } else
                    // Check to see if this is our Keycap letter E
                    if (idCtl EQ (INT_PTR) hWndKeycapE)
                    {
                        // If the keyboard state is displaying Ctrl-keys only
                        //   and this layout reserves that keystroke for the system, ...
                        if (uKeybState EQ KS_CTRL
                         && lpLclKeybLayouts[uLclKeybLayoutNumVis].bUseSEQ)
                            lpttt->lpszText = L"Reserved for system use; click on \"No keyboard shortcuts for Function Editing commands\" to change";
                    } else
                    // Check to see if this is our Keycap letter Q
                    if (idCtl EQ (INT_PTR) hWndKeycapQ)
                    {
                        // If the keyboard state is displaying Ctrl-keys only
                        //   and this layout reserves that keystroke for the system, ...
                        if (uKeybState EQ KS_CTRL
                         && lpLclKeybLayouts[uLclKeybLayoutNumVis].bUseSEQ)
                            lpttt->lpszText = L"Reserved for system use; click on \"No keyboard shortcuts for Function Editing commands\" to change";
                    } else
#endif
                    {
#ifdef DEBUG
                        // Check to see if this is one of our Keycaps
                        if (lpttt->lParam)
                        {
                            // Find the row and col of the ID in lpttt->lParam
                            for (uCnt = 0; uCnt < KKC_CY; uCnt++)
                            for (uCol = aKKC_IDC_BEG[uCnt]; uCol <= aKKC_IDC_END[uCnt]; uCol++)
                            if (lpttt->lParam EQ uCol)
                            {
                                MySprintfW (TooltipText,
                                            sizeof (TooltipText),
                                           L"Scan code 0x%02X",
                                            aKKC_SC[uCnt].aSC[uCol - aKKC_IDC_BEG[uCnt]]);
                                lpttt->lpszText = TooltipText;

                                // Return dialog result
                                DlgMsgDone (hDlg);              // We handled the msg
                            } // End FOR/FOR/IF
                        } // End IF
#endif
                        // In case we used TTF_IDISHWND when adding the Tooltip, get the Ctrl ID
                        idCtl = GetDlgCtrlID ((HWND) idCtl);

                        // Check to see if this is one of our Font labels
                        if ((IDC_FONTS1    <= idCtl && idCtl <= IDC_FONTS_LAST)
                         || (IDC_FONTS_LT1 <= idCtl && idCtl <= IDC_FONTS_LT_LAST)
                         ||                            idCtl EQ IDC_KEYB_BN_FONT)
                        {
                            LOGFONTW lf;

                            // Convert the idCtl into an index
                            if (IDC_FONTS1    <= idCtl && idCtl <= IDC_FONTS_LAST)
                            {
                                idCtl -= IDC_FONTS1;

                                // Get a ptr to the current LOGFONTW struc
                                lf = *fontStruc[lclSameFontAs[idCtl]].lpcf->lpLogFont;
                            } else
                            if (IDC_FONTS_LT1 <= idCtl && idCtl <= IDC_FONTS_LT_LAST)
                            {
                                idCtl -= IDC_FONTS_LT1;

                                // Get a ptr to the current LOGFONTW struc
                                lf = *fontStruc[lclSameFontAs[idCtl]].lpcf->lpLogFont;
                            } else
                                // Get the LOGFONTW structure for the font
                                GetObjectW (hFontKB, sizeof (lf), &lf);

                            // Get # vertical pixels per inch
                            // N.B.:  Do not use a Printer DC here as that calculation is done
                            //        at printing time.  We need to use screen coordinates
                            //        for display purposes and convert to printer coords
                            //        only when printing.
////////////////////////////iLogPixelsY = GetLogPixelsY (hDC);
                            iLogPixelsY = GetLogPixelsY (NULL);

                            // Format the relevant data
                            MySprintfW (TooltipText,
                                        sizeof (TooltipText),
                                        strchrW (lf.lfFaceName, L' ') ? L"\"%s\"%s" : L"%s%s",
                                        lf.lfFaceName,
                                        GetFontWeightW (lf.lfWeight));
                            // If the font is Italic, ...
                            if (lf.lfItalic)
                                MyStrcatW (TooltipText, sizeof (TooltipText), L" Italic");

                            // If the font is Underlined, ...
                            if (lf.lfUnderline)
                                MyStrcatW (TooltipText, sizeof (TooltipText), L" Underline");

                            // If the font is Strike Out, ...
                            if (lf.lfStrikeOut)
                                MyStrcatW (TooltipText, sizeof (TooltipText), L" StrikeOut");

                            // If the font's height is negative, ...
                            if (lf.lfHeight < 0)
                                MySprintfW (&TooltipText[lstrlenW (TooltipText)],
                                             sizeof (TooltipText) - (lstrlenW (TooltipText) * sizeof (TooltipText[0])),
                                            L" %u point",
                                             -MulDiv (lf.lfHeight, 72, iLogPixelsY));
                            // Return the ptr to the caller
                            lpttt->lpszText = TooltipText;
                        } else
                        {
                            // Check to see if this is one of our Syntax Coloring Foreground/Background Color buttons
                            if (IDC_SYNTCLR_BN_FGCLR1 <= idCtl && idCtl <= IDC_SYNTCLR_BN_FGCLR_LAST)
                            {
                                // Convert to an index
                                idCtl -= IDC_SYNTCLR_BN_FGCLR1;

                                // Mark as foreground
                                bFore = TRUE;

                                // Get the foreground color
                                clrRef = lclSyntaxColors[idCtl].crFore;
                            } else
                            // If it's a Syntax Coloring background ID, ...
                            if (IDC_SYNTCLR_BN_BGCLR1 <= idCtl && idCtl <= IDC_SYNTCLR_BN_BGCLR_LAST)
                            {
                                // Convert to an index
                                idCtl -= IDC_SYNTCLR_BN_BGCLR1;

                                // Mark as background
                                bFore = FALSE;

                                // Get the background color
                                clrRef = lclSyntaxColors[idCtl].crBack;
                            } // End IF/ELSE

                            // Initialize the tooltip text
                            TooltipText[0] = WC_EOS;

                            // Loop through the color names to see if there's a match
                            for (uCnt = 0; uCnt < uColorNames; uCnt++)
                            if (clrRef EQ aColorNames[uCnt].clrRef)
                            {
                                MySprintfW (TooltipText,
                                            sizeof (TooltipText),
                                           L"%s: ",
                                            aColorNames[uCnt].lpwName);
                                break;
                            } // End FOR/IF

                            // Return a ptr to the stored tooltip text
                            MySprintfW (&TooltipText[lstrlenW (TooltipText)],
                                         sizeof (TooltipText) - (lstrlenW (TooltipText) * sizeof (TooltipText[0])),
                                        L"%u, %u, %u (#%02X%02X%02X)",
                                         GetRValue (clrRef), GetGValue (clrRef), GetBValue (clrRef),
                                         GetRValue (clrRef), GetGValue (clrRef), GetBValue (clrRef));
                            // Return the ptr to the caller
                            lpttt->lpszText = TooltipText;
                        } // End IF/ELSE
                    } // End IF/ELSE
#ifndef DEBUG
  #undef  lpttt
#endif
                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg
                } // End TTN_GETDISPINFOW

                //***************************************************************
                // Tab Ctrl Notifications
                //***************************************************************
                case TCN_SELCHANGE:             // lpnmh = (LPNMHDR) lParam
                {
                    WCHAR (*lpwText)[KTC_TXT_CX];

                    // Get the handle of the Keyboards Property Page
                    hWndProp = (HWND)
                      SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_KEYBS - IDD_PROPPAGE_START, 0);

                    // Get the TabCtrl window handle
                    hWndKTC = GetDlgItem (hWndProp, IDC_KEYB_TC);

                    // Clear the highlight from the last button
                    KeybHighlight (hWndProp, NOCONTROL);

                    // Get the current selection
                    uKeybTCNum = TabCtrl_GetCurSel (hWndKTC);

                    // Get the ptr to this tab's text array
                    lpwText = aKTC[uKeybTCNum].lpwText;

                    // Loop through the TabCtrl text and change it to that
                    //   of the incoming tab
                    for (uCnt = 0; uCnt < KTC_TXT_CY; uCnt++)
                    for (uCol = 0; uCol < KTC_TXT_CX; uCol++)
                    {
                        // Save the char in a string
                        wszText[0] = lpwText[uCnt][uCol];

                        // Tell the window about its char
                        SendMessageW (GetDlgItem (hWndProp, aKTC_IDC_BEG[uCnt] + uCol), WM_SETTEXT, 0, (LPARAM) wszText);
                    } // End FOR/FOR

                    // Save the last highlight ID
                    idLHL = aKTC[uKeybTCNum].idLastHighlight;

                    // Display the new value
                    DisplayKeybTCValue (hWndProp, aKTC[uKeybTCNum].idLastHighlight, TRUE);

                    // Restore the last highlight ID
                    aKTC[uKeybTCNum].idLastHighlight = idLHL;

                    // Highlight the current button
                    KeybHighlight (hWndProp, aKTC[uKeybTCNum].idLastHighlight);

                    // Return dialog result
                    DlgReturn (hDlg, FALSE);    // Allow the selection to change
                } // End TCN_SELCHANGE

                default:
                    break;
            } // End SWITCH

            break;
#ifndef DEBUG
  #undef  idCtl
  #undef  lpnmh
#endif
        } // End WM_NOTIFY

        case WM_COMMAND:            // wNotifyCode = HIWORD(wParam); // Notification code
                                    // wID = LOWORD(wParam);         // Item, control, or accelerator identifier
                                    // hwndCtl = (HWND) lParam;      // Handle of control
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
                case IDOK:
                    // Apply changes by simulating a click of the Apply button
                    SendMessageW (hDlg, WM_COMMAND, GET_WM_COMMAND_MPS (IDC_APPLY, NULL, BN_CLICKED));

                    // We're done here
                    DestroyWindow (hDlg);

                    // Return dialog result
                    DlgMsgDone (hDlg);          // We handled the msg

                case IDCANCEL:
                    // Complain if abandoning changes
                    if (IsWindowEnabled (hWndApply))
                    // Split cases based upon the user response
                    switch (MessageBoxW (hWndMF, wszCancelMessage, WS_APPNAME, MB_YESNOCANCEL | MB_ICONWARNING))
                    {
                        case IDYES:
                            // Apply changes by simulating a click of the Apply button
                            SendMessageW (hDlg, WM_COMMAND, GET_WM_COMMAND_MPS (IDC_APPLY, NULL, BN_CLICKED));

                            break;

                        case IDNO:
                            // If the keyboard layout is active, ...
                            if (lpLclKeybLayouts NE NULL)
                                // Restore the original keyboard layout
                                aKeybLayoutAct = aKeybLayoutOrig;
                            break;

                        case IDCANCEL:
                            // Return dialog result
                            DlgMsgDone (hDlg);              // We handled the msg

                        defstop
                            break;
                    } // End IF/SWITCH

                    // We're done here
                    DestroyWindow (hDlg);

                    // Return dialog result
                    DlgMsgDone (hDlg);          // We handled the msg

                case IDC_APPLY:
                {
                    // Apply changes

                    //***************************************************************
                    // CLEAR WS VALUES -- Apply
                    //***************************************************************

                    // Get the Property Page index
                    uCnt = IDD_PROPPAGE_CLEARWS_VALUES - IDD_PROPPAGE_START;
                    if (custStruc[uCnt].bInitialized)
                    {
                        LPWCHAR wp;

                        // Get the associated item data (window handle of the Property Page)
                        hWndProp = (HWND)
                          SendMessageW (hWndListBox, LB_GETITEMDATA, uCnt, 0);

                        //***************************************************************
                        // []ALX
                        //***************************************************************

                        // Read in and save the new []ALX value
                        GetClearWsChrValue (hWndProp, IDC_CLEARWS_ALX_EC, &hGlbQuadALX_CWS);

                        //***************************************************************
                        // []CT
                        //***************************************************************

                        // Tell the Edit Ctrl how big our buffer is
                        lpwszGlbTemp[0] = 1024;

                        // Get the []CT text
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_CT_EC), EM_GETLINE, 0, (LPARAM) lpwszGlbTemp);

                        // Save as current ptr
                        wp = lpwszGlbTemp;

                        // Convert overbar to high minus
                        while (wp = strchrW (wp, UTF16_OVERBAR))
                            *wp++ = L'-';

                        // Convert the format string to ASCII
                        W2A (szTemp, lpwszGlbTemp, sizeof (szTemp));

                        // Convert this text to a
                        //   using David Gay's routines
                        fQuadCT_CWS = MyStrtod (szTemp, NULL);

                        //***************************************************************
                        // []DT
                        //***************************************************************

                        // Get the index of the currently selected []DT name
                        uCnt = (UINT)
                          SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_DT_CB), CB_GETCURSEL, 0, 0);

                        // Save in global
                        cQuadDT_CWS = wszQuadDTAllow[uCnt];

                        //***************************************************************
                        // []ELX
                        //***************************************************************

                        // Read in and save the new []ELX value
                        GetClearWsChrValue (hWndProp, IDC_CLEARWS_ELX_EC, &hGlbQuadELX_CWS);

                        //***************************************************************
                        // []FC
                        //***************************************************************

                        // Read in and save the new []FC value
                        GetClearWsChrValue (hWndProp, IDC_CLEARWS_FC_EC, &hGlbQuadFC_CWS);

                        //***************************************************************
                        // []FEATURE
                        //***************************************************************

                        // Save the new []FEATURE value
                        GetClearWsComValue (ARRAY_INT, FEATURENDX_LENGTH, &hGlbQuadFEATURE_CWS, featValues, sizeof (featValues[0]));

                        //***************************************************************
                        // []FPC
                        //***************************************************************
                        uQuadFPC_CWS =
                          GetDlgItemInt (hWndProp, IDC_CLEARWS_FPC_EC, NULL, FALSE);

                        //***************************************************************
                        // []IC
                        //***************************************************************

                        // Save the new []IC value
                        GetClearWsComValue (ARRAY_INT, ICNDX_LENGTH, &hGlbQuadIC_CWS, icValues, sizeof (icValues[0]));

                        //***************************************************************
                        // []IO
                        //***************************************************************
                        bQuadIO_CWS =
                          GetDlgItemInt (hWndProp, IDC_CLEARWS_IO_EC, NULL, FALSE);

                        //***************************************************************
                        // []LX
                        //***************************************************************

                        // Read in and save the new []LX value
                        GetClearWsChrValue (hWndProp, IDC_CLEARWS_LX_EC, &hGlbQuadLX_CWS);

                        //***************************************************************
                        // []MF
                        //***************************************************************
                        uQuadMF_CWS =
                          1 + SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_MF_CB), CB_GETCURSEL, 0, 0);

                        //***************************************************************
                        // []PP
                        //***************************************************************
                        uQuadPP_CWS =
                          GetDlgItemInt (hWndProp, IDC_CLEARWS_PP_EC, NULL, FALSE);

                        //***************************************************************
                        // []PW
                        //***************************************************************
                        uQuadPW_CWS =
                          GetDlgItemInt (hWndProp, IDC_CLEARWS_PW_EC, NULL, FALSE);

                        //***************************************************************
                        // []RL
                        //***************************************************************
                        uQuadRL_CWS =
                          GetDlgItemInt64 (hWndProp, IDC_CLEARWS_RL_EC, NULL, FALSE);
                    } // End IF


                    //***************************************************************
                    // DIRECTORIES -- Apply
                    //***************************************************************

                    // Get the Property Page index
                    uCnt = IDD_PROPPAGE_DIRS - IDD_PROPPAGE_START;
                    if (custStruc[uCnt].bInitialized)
                    {
                        HGLOBAL hGlb;

                        // Get the associated item data (window handle of the Property Page)
                        hWndProp = (HWND)
                          SendMessageW (hWndListBox, LB_GETITEMDATA, uCnt, 0);
                        // Get the # entries in the Dirs ComboBox
                        uCnt = (UINT)
                          SendMessageW (hWndDirsComboBox, CB_GETCOUNT, 0, 0);

                        // Allocate space for the LibDirs
                        hGlb =
                          DbgGlobalAlloc (GHND, uCnt * _MAX_PATH * sizeof (WCHAR));

                        if (hGlb EQ NULL)
                            MessageBoxW (hWndMF, L"Unable to allocate enough memory for Library Directories", WS_APPNAME, MB_OK | MB_ICONSTOP);
                        else
                        {
                            // Lock the memory to get a ptr to it
                            lpwszLibDirs = MyGlobalLock000 (hGlb);

                            // Save the count
                            uNumLibDirs = uCnt;

                            // Loop through the entries
                            for (uCnt = 0; uCnt < uNumLibDirs; uCnt++)
                                // Get the next string
                                SendMessageW (hWndDirsComboBox, CB_GETLBTEXT, uCnt, (LPARAM) lpwszLibDirs[uCnt]);
                            // We no longer need this ptr
                            MyGlobalUnlock (hGlb); lpwszLibDirs = NULL;

                            // Free the old memory
                            DbgGlobalFree (hGlbLibDirs); hGlbLibDirs = NULL;

                            // Save the global memory handle
                            hGlbLibDirs = hGlb;
                        } // End IF/ELSE
                    } // End IF


                    //***************************************************************
                    // FONTS -- Apply
                    //***************************************************************

                    // Get the Property Page index
                    uCnt = IDD_PROPPAGE_FONTS - IDD_PROPPAGE_START;
                    if (custStruc[uCnt].bInitialized)
                    {
                        // Get the associated item data (window handle of the Property Page)
                        hWndProp = (HWND)
                          SendMessageW (hWndListBox, LB_GETITEMDATA, uCnt, 0);

                        // Loop through the fonts
                        for (uCnt = 0; uCnt < FONTENUM_LENGTH; uCnt++)
                        {
                            // If the font is the same as another, or
                            //   its SameFontAs state changed, ...
                            if (lclSameFontAs[uCnt] NE (FONTENUM) uCnt
                             || glbSameFontAs[uCnt] NE lclSameFontAs[uCnt])
                            {
                                // If its SameFontAs state changed, ...
                                if (glbSameFontAs[uCnt] NE lclSameFontAs[uCnt])
                                {
                                    // Copy the local SameFontAs value to the global
                                    glbSameFontAs[uCnt] = lclSameFontAs[uCnt];

                                    // Mark as should be applied
                                    fontStruc[glbSameFontAs[uCnt]].bApplied = TRUE;
                                } // End IF
                            } else
                            // If the font changed, ...
                            if (fontStruc[uCnt].bChanged)
                            {
                                HWND hwndOwner;

                                // Because we changed the hwndOwner in cfLcl,
                                //   we must not copy it back to the global
                                hwndOwner = fontStruc[uCnt].lpcf->hwndOwner;

                                // Copy the local CHOOSEFONTW value to the global
                                *fontStruc[uCnt].lpcf = fontStruc[uCnt].cfLcl;

                                // Restore the original value
                                fontStruc[uCnt].lpcf->hwndOwner = hwndOwner;

                                // Call the CreateNewFontXX for this font
                                //   but don't apply it as yet as other
                                //   windows might depend upon it.
                                (*fontStruc[uCnt].lpCreateNewFont) (FALSE);

                                // Mark as should be applied
                                fontStruc[uCnt].bApplied = TRUE;
                            } // End IF/ELSE/...
                        } // End FOR

                        // Loop through the fonts again, this time applying them
                        for (uCnt = 0; uCnt < FONTENUM_LENGTH; uCnt++)
                        // If the font should be applied, ...
                        if (fontStruc[uCnt].bApplied)
                        {
                            // Clear the bit field for next time
                            fontStruc[uCnt].bApplied = FALSE;

                            // Call the appropriate ApplyNewFontXX for this fontEnum
                            ApplyNewFontEnum (uCnt);

                            // If this is the Tab Ctrl font, ...
                            if (uCnt EQ FONTENUM_TC)
                            {
                                RECT rc;                // Rectangle for the MDI Client windows

                                // Calculate the client rectangle for the MDI Client windows
                                //   in Master Frame client area coords using the GetClientRect
                                //   value.
                                CalcClientRectMC (&rc, TRUE);

                                // Resize each of the MDI Client windows
                                EnumChildWindows (hWndMF, EnumCallbackResizeMC, (LPARAM) &rc);
                            } // End IF
                        } // End IF
                    } // End IF


                    //***************************************************************
                    // KEYBOARDS -- Apply
                    //***************************************************************

                    // Get the Property Page index
                    uCnt = IDD_PROPPAGE_KEYBS - IDD_PROPPAGE_START;
                    if (custStruc[uCnt].bInitialized)
                    {
                        // Delete the global copy
                        DbgGlobalFree (hGlbKeybLayouts); hGlbKeybLayouts = NULL;

                        // Get the size of the local copy
                        uKeybSize = MyGlobalSize (hLclKeybLayouts);

                        // Allocate space for another copy of the local layouts
                        hGlbKeybLayouts = DbgGlobalAlloc (GHND, uKeybSize);
                        if (hGlbKeybLayouts EQ NULL)
                        {
                            MessageBoxW (hWndProp, L"Unable to allocate memory for a local copy of the keyboard layouts.", lpwszAppName, MB_OK | MB_ICONERROR);

                            // We're done here
                            DestroyWindow (hDlg);
                        } // End IF

                        // Lock the memory to get a ptr to it
                        lpGlbKeybLayouts = MyGlobalLock000 (hGlbKeybLayouts);

                        // Copy the local values to the global copy
                        CopyMemory (lpGlbKeybLayouts, lpLclKeybLayouts, uKeybSize);

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbKeybLayouts); lpGlbKeybLayouts = NULL;

                        // Save the current keyboard layout as the active copy
                        aKeybLayoutAct = lpLclKeybLayouts[uLclKeybLayoutNumAct];

                        // Save the current keyboard layout in our local copy
                        //   to restore on Cancel
                        aKeybLayoutOrig = aKeybLayoutAct;

                        // Copy the local values to the globals
                        uGlbKeybLayoutNumAct = uLclKeybLayoutNumAct;
                        uGlbKeybLayoutNumVis = uLclKeybLayoutNumVis;
                        uGlbKeybLayoutCount  = uLclKeybLayoutCount ;
                        uGlbKeybLayoutUser   = uLclKeybLayoutUser  ;
                        MyStrcpyW (wszGlbKeybLayoutAct, sizeof (wszGlbKeybLayoutAct), wszLclKeybLayoutAct);
                        MyStrcpyW (wszGlbKeybLayoutVis, sizeof (wszGlbKeybLayoutVis), wszLclKeybLayoutVis);

                        // Copy the last selected unicode values
                        uKeybChar = uLclKeybChar;
                    } // End IF


                    //***************************************************************
                    // RANGE LIMITED VARS -- Apply
                    //***************************************************************

                    // Get the Property Page index
                    uCnt = IDD_PROPPAGE_RANGE_LIMITS - IDD_PROPPAGE_START;
                    if (custStruc[uCnt].bInitialized)
                    {
                        // Get the associated item data (window handle of the Property Page)
                        hWndProp = (HWND)
                          SendMessageW (hWndListBox, LB_GETITEMDATA, uCnt, 0);

                        bRangeLimit.CT      = IsDlgButtonChecked (hWndProp, IDC_RANGE_XB_CT      );
                        bRangeLimit.FEATURE = IsDlgButtonChecked (hWndProp, IDC_RANGE_XB_FEATURE );
                        bRangeLimit.IC      = IsDlgButtonChecked (hWndProp, IDC_RANGE_XB_IC      );
                        bRangeLimit.IO      = IsDlgButtonChecked (hWndProp, IDC_RANGE_XB_IO      );
                        bRangeLimit.PP      = IsDlgButtonChecked (hWndProp, IDC_RANGE_XB_PP      );
                        bRangeLimit.PW      = IsDlgButtonChecked (hWndProp, IDC_RANGE_XB_PW      );
                        bRangeLimit.RL      = IsDlgButtonChecked (hWndProp, IDC_RANGE_XB_RL      );
                    } // End IF


                    //***************************************************************
                    // SYNTAX COLORING -- Apply
                    //***************************************************************

                    // Get the Property Page index
                    uCnt = IDD_PROPPAGE_SYNTAX_COLORING - IDD_PROPPAGE_START;
                    if (custStruc[uCnt].bInitialized)
                    {
                        UBOOL bWinBGDiff;           // TRUE iff the Window background color is changing

                        // Get the associated item data (window handle of the Property Page)
                        hWndProp = (HWND)
                          SendMessageW (hWndListBox, LB_GETITEMDATA, uCnt, 0);

                        // Determine if the Window background changes
                        bWinBGDiff =
                          (gSyntaxColorText.crBack NE lclSyntaxColors[SC_WINTEXT].crBack);

                        // Loop through the Syntax Colors
                        for (uCnt = 0; uCnt < SC_LENGTH; uCnt++)
                        {
                            // Copy the state of the "Background Transparent" checkboxes to the global var
                            gSyntClrBGTrans[uCnt] = IsDlgButtonChecked (hWndProp, IDC_SYNTCLR_XB_TRANS1 + uCnt);

                            // Copy the local Foreground/Background Colors to the global var
                            gSyntaxColorName[uCnt].syntClr = lclSyntaxColors[uCnt];

                            // If the background is transparent, ...
                            if (gSyntClrBGTrans[uCnt])
                                // Change it to the Windows text background
                                gSyntaxColorName[uCnt].syntClr.crBack = gSyntaxColorName[SC_WINTEXT].syntClr.crBack;
                        } // End IF

                        // Copy the state of the "Enable ... Coloring" checkboxes to the OptionFlags
                        OptionFlags.bSyntClrFcns = IsDlgButtonChecked (hWndProp, IDC_SYNTCLR_XB_CLRFCNS);
                        OptionFlags.bSyntClrSess = IsDlgButtonChecked (hWndProp, IDC_SYNTCLR_XB_CLRSESS);
                        OptionFlags.bSyntClrPrnt = IsDlgButtonChecked (hWndProp, IDC_SYNTCLR_XB_CLRPRNT);

                        // If the Window background color changed, ...
                        if (bWinBGDiff)
                            // Respecify the Window Background brush
                            //   because the Window Background color has changed
                            RedoWinBG ();

                        // Repaint the current Session Manager as well
                        //   as any open Function Editor sessions
                        EnumChildWindows (hWndMF, &EnumCallbackRepaint, bWinBGDiff);
                    } // End IF


                    //***************************************************************
                    // SYSTEM VAR RESET -- Apply
                    //***************************************************************

                    // Get the Property Page index
                    uCnt = IDD_PROPPAGE_SYSTEM_VAR_RESET - IDD_PROPPAGE_START;
                    if (custStruc[uCnt].bInitialized)
                    {
                        // Get the associated item data (window handle of the Property Page)
                        hWndProp = (HWND)
                          SendMessageW (hWndListBox, LB_GETITEMDATA, uCnt, 0);

                        bResetVars.CT      = IsDlgButtonChecked (hWndProp, IDC_RESET_CT_RADIO2      );
                        bResetVars.DT      = IsDlgButtonChecked (hWndProp, IDC_RESET_DT_RADIO2      );
                        bResetVars.FC      = IsDlgButtonChecked (hWndProp, IDC_RESET_FC_RADIO2      );
                        bResetVars.FEATURE = IsDlgButtonChecked (hWndProp, IDC_RESET_FEATURE_RADIO2 );
                        bResetVars.FPC     = IsDlgButtonChecked (hWndProp, IDC_RESET_FPC_RADIO2     );
                        bResetVars.IC      = IsDlgButtonChecked (hWndProp, IDC_RESET_IC_RADIO2      );
                        bResetVars.IO      = IsDlgButtonChecked (hWndProp, IDC_RESET_IO_RADIO2      );
                        bResetVars.PP      = IsDlgButtonChecked (hWndProp, IDC_RESET_PP_RADIO2      );
                        bResetVars.PW      = IsDlgButtonChecked (hWndProp, IDC_RESET_PW_RADIO2      );
                        bResetVars.RL      = IsDlgButtonChecked (hWndProp, IDC_RESET_RL_RADIO2      );
                    } // End IF


                    //***************************************************************
                    // TAB COLORS -- Apply
                    //***************************************************************

////////////////////// Get the Property Page index
////////////////////uCnt = IDD_PROPPAGE_TAB_COLORS - IDD_PROPPAGE_START;
////////////////////if (custStruc[uCnt].bInitialized)
////////////////////{
////////////////////    // Get the associated item data (window handle of the Property Page)
////////////////////    hWndProp = (HWND)
////////////////////      SendMessageW (hWndListBox, LB_GETITEMDATA, uCnt, 0);
////////////////////
////////////////////    DbgBrk ();      // ***FINISHME***
////////////////////
////////////////////
////////////////////
////////////////////
////////////////////
////////////////////} // End IF


                    //***************************************************************
                    // USER PREFERENCES -- Apply
                    //***************************************************************

                    // Get the Property Page index
                    uCnt = IDD_PROPPAGE_USER_PREFS - IDD_PROPPAGE_START;
                    if (custStruc[uCnt].bInitialized)
                    {
                        // Get the associated item data (window handle of the Property Page)
                        hWndProp = (HWND)
                          SendMessageW (hWndListBox, LB_GETITEMDATA, uCnt, 0);

                        OptionFlags.bAdjustPW            = IsDlgButtonChecked (hWndProp, IDC_USER_PREFS_XB_ADJUSTPW           );
                        OptionFlags.bUnderbarToLowercase = IsDlgButtonChecked (hWndProp, IDC_USER_PREFS_XB_UNDERBARTOLOWERCASE);
                        OptionFlags.bNewTabOnClear       = IsDlgButtonChecked (hWndProp, IDC_USER_PREFS_XB_NEWTABONCLEAR      );
                        OptionFlags.bNewTabOnLoad        = IsDlgButtonChecked (hWndProp, IDC_USER_PREFS_XB_NEWTABONLOAD       );
                        OptionFlags.bUseLocalTime        = IsDlgButtonChecked (hWndProp, IDC_USER_PREFS_XB_USELOCALTIME       );
                        OptionFlags.bBackupOnLoad        = IsDlgButtonChecked (hWndProp, IDC_USER_PREFS_XB_BACKUPONLOAD       );
                        OptionFlags.bBackupOnSave        = IsDlgButtonChecked (hWndProp, IDC_USER_PREFS_XB_BACKUPONSAVE       );
                        OptionFlags.bNoCopyrightMsg      = IsDlgButtonChecked (hWndProp, IDC_USER_PREFS_XB_NOCOPYRIGHTMSG     );
                        OptionFlags.bCheckGroup          = IsDlgButtonChecked (hWndProp, IDC_USER_PREFS_XB_CHECKGROUP         );
                        OptionFlags.bInsState            = IsDlgButtonChecked (hWndProp, IDC_USER_PREFS_XB_INSSTATE           );
                        OptionFlags.bRevDblClk           = IsDlgButtonChecked (hWndProp, IDC_USER_PREFS_XB_REVDBLCLK          );
                        OptionFlags.bDefDispFcnLineNums  = IsDlgButtonChecked (hWndProp, IDC_USER_PREFS_XB_DEFDISPFCNLINENUMS );
                        OptionFlags.bDispMPSuf           = IsDlgButtonChecked (hWndProp, IDC_USER_PREFS_XB_DISPMPSUF          );
                        OptionFlags.bOutputDebug         = IsDlgButtonChecked (hWndProp, IDC_USER_PREFS_XB_OUTPUTDEBUG        );

                        // Get the window handle for the Paste & Copy combo boxes
                        hWndProp1 = GetDlgItem (hWndProp, IDC_USER_PREFS_CB_DEFAULTPASTE);
                        hWndProp2 = GetDlgItem (hWndProp, IDC_USER_PREFS_CB_DEFAULTCOPY);

                        // Get the current selection of the user preference value
                        OptionFlags.uDefaultPaste = unitransStr[SendMessageW (hWndProp1, CB_GETCURSEL, 0, 0)].OutTransPaste;
                        OptionFlags.uDefaultCopy  = unitransStr[SendMessageW (hWndProp2, CB_GETCURSEL, 0, 0)].OutTransCopy ;

                        // Get the window handle for the Update Frequency combo box
                        hWndProp1 = GetDlgItem (hWndProp, IDC_USER_PREFS_CB_UPDFRQ);

                        // Get the current selection of the user preference value
                        guUpdFrq = (UINT) SendMessageW (hWndProp1, CB_GETCURSEL, 0, 0);

                        // Save the update frequency text string
                        MyStrcpyW (gszUpdFrq, sizeof (gszUpdFrq), updFrq[guUpdFrq].lpwsz);

                        // Copy the last selected unicode values
                        uUserChar = uLclUserChar;

                        // Loop through all of the font categories
                        for (enumFont = 0; enumFont < FONTENUM_LENGTH; enumFont++)
                            // Recalculate the width of the Line Continuation marker
                            //   in each font category
                            uWidthLC[enumFont] = WidthLC (enumFont);

                        // Redraw the Line Continuations markers in SM and FE windows
                        EnumChildWindows (hWndMF, &EnumCallbackDrawLineCont, 0);

                        // Respecify the Fallback font
                        EnumChildWindows (hWndMF, &EnumCallbackFallbackFont, 0);
                    } // End IF

                    // Disable the Apply button
                    EnableWindow (hWndApply, FALSE);

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg
                } // End IDC_APPLY

                case IDC_CUST_LB:
                    // Split cases based upon the notification code
                    switch (cmdCtl)
                    {
                        case LBN_SELCHANGE:
                            // Get the index of the current selection
                            uSel = (UINT) SendMessageW (hWndListBox, LB_GETCURSEL, 0, 0);

                            // Save for next time
                            gInitCustomizeCategory = uSel;

                            // Get the associated item data (if any)
                            uIDD = SendMessageW (hWndListBox, LB_GETITEMDATA, uSel, 0);

                            // If it's an IDD_xxx value, ...
                            if (HIWORD (LODWORD (uIDD)) EQ 0)
                            {
                                hWndProp =
                                  CreateDialogParamW (_hInstance,
                                                      MAKEINTRESOURCEW (uIDD),
                                                      hDlg,
                                           (DLGPROC) &CustomizeDlgProc,
                                                      TRUE);        // Anything non-zero so as to
                                                                    //   distinguish it from the
                                                                    //   initial entry
                                // Save the window handle as the item data
                                SendMessageW (hWndListBox, LB_SETITEMDATA, uSel, (LPARAM) hWndProp);

                                // Tell the dialog to initialize the new page
                                SendMessageW (hDlg, MYWM_INITDIALOG, uSel, 0);
                            } else
                            {
                                // Get the Property Page window handle
                                hWndProp = MakeGlbFromVal (uIDD);

                                // Set the group box text
                                SetDlgItemTextW (hDlg, IDC_GROUPBOX, custStruc[uSel].lpwTitle);
                            } // End IF/ELSE

                            // If there's an outgoing window, ...
                            if (hWndLast NE NULL)
                            {
                                // Position the outgoing window on the bottom
                                SetWindowPos (hWndLast,
                                              HWND_BOTTOM,
                                              0,
                                              0,
                                              0,
                                              0,
                                              SWP_HIDEWINDOW
                                            | SWP_NOACTIVATE
                                            | SWP_NOMOVE
                                            | SWP_NOSIZE);
                                EnableWindow (hWndLast, FALSE);
                            } // End IF

                            // Save incoming window handle as the last
                            hWndLast = hWndProp;

                            // Position the incoming window on the top
                            SetWindowPos (hWndLast,
                                          HWND_TOP,
                                          ptGroupBox.x,
                                          ptGroupBox.y,
                                          szGroupBox.cx,
                                          szGroupBox.cy,
                                          SWP_SHOWWINDOW);
                            EnableWindow (hWndLast, TRUE);

                            break;

                        default:
                            break;
                    } // End SWITCH

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg


                //***************************************************************
                // CLEAR WS VALUES -- WM_COMMAND
                //***************************************************************

                case IDC_CLEARWS_ALX_EC:
                case IDC_CLEARWS_ELX_EC:
                case IDC_CLEARWS_FC_EC:
                case IDC_CLEARWS_LX_EC:
                    // We care about EN_CHANGE only
                    if (EN_CHANGE EQ cmdCtl)
                    {
                        UINT    uID_EC;
                        UBOOL   bFC;
                        HWND    hWnd_EC,
                                hWnd_ST;

                        // Get the associated item data (window handle of the Property Page)
                        hWndProp = (HWND)
                          SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_CLEARWS_VALUES - IDD_PROPPAGE_START, 0);

                        // Get the Edit Ctrl ID
                        uID_EC = idCtl;

                        // Get the Edit Ctrl & Static Text window handles
                        hWnd_EC = GetDlgItem (hWndProp, uID_EC    );
                        hWnd_ST = GetDlgItem (hWndProp, uID_EC + 1);

                        // Get the text length and format it into the static text on the right
                        uSel = (UINT) SendMessageW (hWnd_EC, EM_LINELENGTH, 0, 0);

                        // If this is the []FC Edit Ctrl and the length is not the expected value, ...
                        bFC = ((uID_EC EQ IDC_CLEARWS_FC_EC) && (uSel NE DEF_QUADFC_CWS_LEN));

                        // Format the text length
                        MySprintfW (wszTemp,
                                    sizeof (wszTemp),
                                   L"Character Vector (%u)",
                                    uSel);
                        // Tell the Static Text control about the new font
                        SendMessageW (hWnd_ST,
                                      WM_SETFONT,
                                      (WPARAM) (bFC ? hFontBold_ST : hFontNorm_ST),
                                      MAKELPARAM (FALSE, 0));
                        // Display the message and text length
                        SetWindowTextW (hWnd_ST, wszTemp);

                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    } // End IF

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_CLEARWS_CT_EC:
                    // Split cases based upon the command
                    switch (cmdCtl)
                    {
                        APLFLOAT fQuadVar;

                        case EN_CHANGE:
                            // Enable the Apply button
                            EnableWindow (hWndApply, TRUE);

                            break;

                        case EN_KILLFOCUS:
                            // If we're leaving this control, validate the number
                            //   and complain if not valid.

                            // Get the associated item data (window handle of the Property Page)
                            hWndProp = (HWND)
                              SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_CLEARWS_VALUES - IDD_PROPPAGE_START, 0);

                            // Tell the Edit Ctrl how big our buffer is
                            lpwszGlbTemp[0] = 1024;

                            // Get the text
                            SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_CT_EC), EM_GETLINE, 0, (LPARAM) lpwszGlbTemp);

                            if (lpwszGlbTemp[0])
                            {
                                LPWCHAR wp;

                                // Ensure empty
                                wszTemp[0] = WC_EOS;

                                // Save as current ptr
                                wp = lpwszGlbTemp;

                                // Convert overbar to high minus
                                while (wp = strchrW (wp, UTF16_OVERBAR))
                                    *wp++ = L'-';

                                // Scan the value and one additional field to check for extra chars
                                sscanfW (lpwszGlbTemp, L"%le%1ws", &fQuadVar, wszTemp);

                                // Ensure non-negative and no extra chars
                                if (fQuadVar >= 0
                                 && wszTemp[0] EQ WC_EOS)
                                    break;
                            } // End IF

                            // Display the error message
                            MessageBoxW (hWndProp, L"This Field is not a correctly formed non-negative number.  Please fix it before changing focus.", lpwszAppName, MB_OK | MB_ICONERROR);

                            // Return the focus to the Edit Ctrl
                            SetFocus (GetDlgItem (hWndProp, IDC_CLEARWS_CT_EC));

                            break;

                        default:
                            break;
                    } // End SWITCH

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_CLEARWS_DT_CB:
                    // We care about CBN_SELCHANGE only
                    if (CBN_SELCHANGE EQ cmdCtl)
                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_CLEARWS_FEATURE_CB1:
                    // We care about CBN_SELCHANGE only
                    if (CBN_SELCHANGE EQ cmdCtl)
                    {
                        // Get the changed index
                        uSel = (UINT)
                          SendMessageW (hWndFEATURE_CB1, CB_GETCURSEL, 0, 0);

                        // Display the corresponding selection
                        SendMessageW (hWndFEATURE_CB2, CB_SETCURSEL, (APLU3264) (featValues[uSel] - FEATUREVAL_MINVAL), 0);

                        // Note that the Apply button is enabled (below) only when
                        //   changing the value associated with this index.
                    } // End IF

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_CLEARWS_FEATURE_CB2:
                    // We care about CBN_SELCHANGE only
                    if (CBN_SELCHANGE EQ cmdCtl)
                    {
                        // Get the current index
                        uSel = (UINT)
                          SendMessageW (hWndFEATURE_CB1, CB_GETCURSEL, 0, 0);

                        // Save as current value
                        featValues[uSel] = ((APLINT) SendMessageW (hWndFEATURE_CB2, CB_GETCURSEL, 0, 0)) + FEATUREVAL_MINVAL;

                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    } // End IF

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_CLEARWS_IC_CB1:
                    // We care about CBN_SELCHANGE only
                    if (CBN_SELCHANGE EQ cmdCtl)
                    {
                        // Get the changed index
                        uSel = (UINT)
                          SendMessageW (hWndIC_CB1, CB_GETCURSEL, 0, 0);

                        // Display the corresponding selection
                        SendMessageW (hWndIC_CB2, CB_SETCURSEL, (APLU3264) (icValues[uSel] - ICVAL_MINVAL), 0);

                        // Note that the Apply button is enabled (below) only when
                        //   changing the value associated with this index.
                    } // End IF

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_CLEARWS_IC_CB2:
                    // We care about CBN_SELCHANGE only
                    if (CBN_SELCHANGE EQ cmdCtl)
                    {
                        // Get the current index
                        uSel = (UINT)
                          SendMessageW (hWndIC_CB1, CB_GETCURSEL, 0, 0);

                        // Save as current value
                        icValues[uSel] = ((APLINT) SendMessageW (hWndIC_CB2, CB_GETCURSEL, 0, 0)) + ICVAL_MINVAL;

                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    } // End IF

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_CLEARWS_MF_CB:
                    // We care about CBN_SELCHANGE only
                    if (CBN_SELCHANGE EQ cmdCtl)
                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg


                //***************************************************************
                // DIRECTORIES -- WM_COMMAND
                //***************************************************************

                case IDC_DIRS_BN_BROWSE:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ cmdCtl)
                    {
                        // Get the associated item data (window handle of the Property Page)
                        hWndProp = (HWND)
                          SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_DIRS - IDD_PROPPAGE_START, 0);

                        // Fill in BROWSEINFO parameters
                        bi.hwndOwner      = hWndProp;               // Owner window handle
                        bi.pidlRoot       = NULL;                   // Allow root at Desktop
                        bi.pszDisplayName = wszBrowsePath;          // Return result here
////////////////////////bi.lpszTitle      =                         // No title
                        bi.ulFlags        = 0                       // Flags
                                          | BIF_EDITBOX
                                          | BIF_VALIDATE
                                          | BIF_NEWDIALOGSTYLE
                                          ;
                        bi.lpfn           = DirsBrowseCallbackProc; // Callback
////////////////////////bi.lParams        =                         // Used with .lpfn only
////////////////////////bi.iImage         =                         // Returns index of image associated with selected folder

                        // Initialize the ptr to the ID list
                        pidl = SHBrowseForFolderW (&bi);

                        // If it's valid, ...
                        if (pidl)
                        {
                            IMalloc *iMalloc = 0;

                            // Get the name of the folder and save it in the ComboBox's Edit Ctrl
                            SHGetPathFromIDListW (pidl, wszBrowsePath);

                            // Save the path into the ComboBox's Edit Ctrl
                            SendMessageW (cbi.hwndItem, WM_SETTEXT, 0, (LPARAM) wszBrowsePath);

                            // Free memory used
                            if (SUCCEEDED (SHGetMalloc (&iMalloc)))
                            {
                                IMalloc_Free (iMalloc, pidl); pidl = NULL;
                                IMalloc_Release (iMalloc); iMalloc = NULL;
                            } // End IF

                            // Check for error
                            if (wszBrowsePath[0] EQ WC_EOS)
                                // Sound the alarum!
                                MessageBeep (MB_ICONERROR);
                            else
                            {
                                // Check for duplicates
                                uCnt = (UINT)
                                  SendMessageW (hWndDirsComboBox, CB_FINDSTRINGEXACT, -1, (LPARAM) wszBrowsePath);
                                if (uCnt NE CB_ERR)
                                    // Sound the alarum!
                                    MessageBeep (MB_ICONERROR);
                                else
                                {
                                    // Save the path in the ComboBox
                                    SendMessageW (hWndDirsComboBox, CB_ADDSTRING, 0, (LPARAM) wszBrowsePath);

                                    // Select it into the Edit Ctrl
                                    SendMessageW (hWndDirsComboBox, CB_SELECTSTRING, -1, (LPARAM) wszBrowsePath);
                                } // End IF/ELSE
                            } // End IF/ELSE

                            // Enable the Apply button
                            EnableWindow (hWndApply, TRUE);
                        } // End IF
                    } // End IF

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                 case IDC_DIRS_BN_DEL:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ cmdCtl)
                    {
                        // Get the associated item data (window handle of the Property Page)
                        hWndProp = (HWND)
                          SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_DIRS - IDD_PROPPAGE_START, 0);

                        // Get the path from the ComboBox's Edit Ctrl
                        SendMessageW (cbi.hwndItem, WM_GETTEXT, countof (wszBrowsePath), (LPARAM) wszBrowsePath);

                        // Get the index of the string in the ComboBox Edit Ctrl
                        uCnt = (UINT)
                          SendMessageW (hWndDirsComboBox, CB_FINDSTRINGEXACT, -1, (LPARAM) wszBrowsePath);
                        // Check for error
                        if (uCnt EQ CB_ERR)
                            // Sound the alarum!
                            MessageBeep (MB_ICONERROR);
                        else
                        {
                            // Delete the string from the ComboBox
                            SendMessageW (hWndDirsComboBox, CB_DELETESTRING, uCnt, 0);

                            // Remove it from the Edit Ctrl
                            SendMessageW (hWndDirsComboBox, CB_SETCURSEL, -1, 0);
                        } // End IF/ELSE

                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    } // End IF

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg


                //***************************************************************
                // FONTS -- WM_COMMAND
                //***************************************************************

                case IDC_FONTS_RADIO2A:
                    // Set the local SameFontAs value
                    lclSameFontAs[1] = idCtl - IDC_FONTS_RADIO2A;

                    // Disable the corresponding ChooseFontW button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONTS2, FALSE);

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_FONTS_RADIO3A:
                case IDC_FONTS_RADIO3B:
                    // Set the local SameFontAs value
                    lclSameFontAs[2] = idCtl - IDC_FONTS_RADIO3A;

                    // Disable the corresponding ChooseFontW button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONTS3, FALSE);

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_FONTS_RADIO4A:
                case IDC_FONTS_RADIO4B:
                case IDC_FONTS_RADIO4C:
                    // Set the local SameFontAs value
                    lclSameFontAs[3] = idCtl - IDC_FONTS_RADIO4A;

                    // Disable the corresponding ChooseFontW button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONTS4, FALSE);

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_FONTS_RADIO5A:
                case IDC_FONTS_RADIO5B:
                case IDC_FONTS_RADIO5C:
                case IDC_FONTS_RADIO5D:
                    // Set the local SameFontAs value
                    lclSameFontAs[4] = idCtl - IDC_FONTS_RADIO5A;

                    // Disable the corresponding ChooseFontW button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONTS5, FALSE);

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_FONTS_RADIO6A:
                case IDC_FONTS_RADIO6B:
                case IDC_FONTS_RADIO6C:
                case IDC_FONTS_RADIO6D:
                case IDC_FONTS_RADIO6E:
                    // Set the local SameFontAs value
                    lclSameFontAs[5] = idCtl - IDC_FONTS_RADIO6A;

                    // Disable the corresponding ChooseFontW button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONTS6, FALSE);

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_FONTS_RADIO7A:
                case IDC_FONTS_RADIO7B:
                case IDC_FONTS_RADIO7C:
                case IDC_FONTS_RADIO7D:
                case IDC_FONTS_RADIO7E:
                case IDC_FONTS_RADIO7F:
                    // Set the local SameFontAs value
                    lclSameFontAs[6] = idCtl - IDC_FONTS_RADIO7A;

                    // Disable the corresponding ChooseFontW button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONTS7, FALSE);

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_FONTS_RADIO8A:
                case IDC_FONTS_RADIO8B:
                case IDC_FONTS_RADIO8C:
                case IDC_FONTS_RADIO8D:
                case IDC_FONTS_RADIO8E:
                case IDC_FONTS_RADIO8F:
                case IDC_FONTS_RADIO8G:
                    // Set the local SameFontAs value
                    lclSameFontAs[7] = idCtl - IDC_FONTS_RADIO8A;

                    // Disable the corresponding ChooseFontW button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONTS8, FALSE);

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_FONTS_RADIO1A:
                    // Set the local SameFontAs value
                    lclSameFontAs[0] = idCtl - IDC_FONTS_RADIO1A;

                    // Enable the corresponding ChooseFontW button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONTS1, TRUE);

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_FONTS_RADIO2B:
                    // Set the local SameFontAs value
                    lclSameFontAs[1] = idCtl - IDC_FONTS_RADIO2A;

                    // Enable the corresponding ChooseFontW button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONTS2, TRUE);

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_FONTS_RADIO3C:
                    // Set the local SameFontAs value
                    lclSameFontAs[2] = idCtl - IDC_FONTS_RADIO3A;

                    // Enable the corresponding ChooseFontW button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONTS3, TRUE);

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_FONTS_RADIO4D:
                    // Set the local SameFontAs value
                    lclSameFontAs[3] = idCtl - IDC_FONTS_RADIO4A;

                    // Enable the corresponding ChooseFontW button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONTS4, TRUE);

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_FONTS_RADIO5E:
                    // Set the local SameFontAs value
                    lclSameFontAs[4] = idCtl - IDC_FONTS_RADIO5A;

                    // Enable the corresponding ChooseFontW button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONTS5, TRUE);

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_FONTS_RADIO6F:
                    // Set the local SameFontAs value
                    lclSameFontAs[5] = idCtl - IDC_FONTS_RADIO6A;

                    // Enable the corresponding ChooseFontW button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONTS6, TRUE);

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_FONTS_RADIO7G:
                    // Set the local SameFontAs value
                    lclSameFontAs[6] = idCtl - IDC_FONTS_RADIO7A;

                    // Enable the corresponding ChooseFontW button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONTS7, TRUE);

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_FONTS_RADIO8H:
                    // Set the local SameFontAs value
                    lclSameFontAs[7] = idCtl - IDC_FONTS_RADIO8A;

                    // Enable the corresponding ChooseFontW button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONTS8, TRUE);

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_FONTS1:
                case IDC_FONTS2:
                case IDC_FONTS3:
                case IDC_FONTS4:
                case IDC_FONTS5:
                case IDC_FONTS6:
                case IDC_FONTS7:
                case IDC_FONTS8:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ cmdCtl)
                    {
                        // Get the associated item data (window handle of the Property Page)
                        hWndProp = (HWND)
                          SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_FONTS - IDD_PROPPAGE_START, 0);

                        // Get the index of the font
                        uCnt = idCtl - IDC_FONTS1;

                        // Ask the user to choose a font, and keep track of
                        //   whether or not a font was chosen
                        // If the font changed, ...
                        if (ChooseFontW (&fontStruc[uCnt].cfLcl))
                        {
                            // Mark as changed
                            fontStruc[uCnt].bChanged = TRUE;

                            // Enable the Apply button
                            EnableWindow (hWndApply, TRUE);
                        } // End IF
                    } // End IF

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg


                //***************************************************************
                // RANGE LIMITED VARS -- WM_COMMAND
                //***************************************************************

                case IDC_RANGE_XB_CT:
                case IDC_RANGE_XB_IO:
                case IDC_RANGE_XB_IC:
                case IDC_RANGE_XB_PP:
                case IDC_RANGE_XB_PW:
                case IDC_RANGE_XB_RL:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ cmdCtl)
                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg


                //***************************************************************
                // SYNTAX COLORING -- WM_COMMAND
                //***************************************************************

                case IDC_SYNTCLR_XB_CLRFCNS:
                case IDC_SYNTCLR_XB_CLRSESS:
                case IDC_SYNTCLR_XB_CLRPRNT:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ cmdCtl)
                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg


                //***************************************************************
                // KEYBOARDS -- WM_COMMAND
                //***************************************************************

                case IDC_KEYB_EC_UNICODE:
                    // Split cases based upon the command
                    switch (cmdCtl)
                    {
                        case EN_CHANGE:
                            // Get the associated item data (window handle of the Property Page)
                            hWndProp = (HWND)
                              SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_KEYBS - IDD_PROPPAGE_START, 0);

                            // Get the text
                            SendMessageW (GetDlgItem (hWndProp, IDC_KEYB_EC_UNICODE), WM_GETTEXT, KEYB_UNICODE_LIMIT + 1, (LPARAM) wszStr);

                            // Convert the text to a binary value using base uKeybUnibase
                            lpwszStr = &wszStr[0];
                            uValue = 0;
                            while (*lpwszStr)
                            {
                                // Shift over the value
                                uValue *= uKeybUnibase;

                                if (L'0' <= *lpwszStr && *lpwszStr <= L'9')
                                    uValue += *lpwszStr - L'0';
                                else
                                    uValue += 10 + tolowerW (*lpwszStr) - L'a';

                                // Skip to the next value
                                lpwszStr++;
                            } // End WHILE

                            // Save the value locally
                            uLclKeybChar = uValue;

                            // Display the value
                            wszText[0] = (WCHAR) uValue;
                            SendMessageW (GetDlgItem (hWndProp, IDC_KEYB_BN_UNICODE), WM_SETTEXT, 0, (LPARAM) wszText);

                            // Clear the highlight from the last button
                            KeybHighlight (hWndProp, NOCONTROL);

                            // Clear the control ID
                            aKTC[uKeybTCNum].idLastHighlight = NOCONTROL;

                            break;
                    } // End SWITCH

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_KEYB_RB_DEC:
                case IDC_KEYB_RB_HEX:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ cmdCtl)
                    {
                        UINT uNewBase;

                        // Calculate the new base
                        uNewBase = (idCtl EQ IDC_KEYB_RB_DEC) ? 10 : 16;

                        // If it's different, ...
                        if (uKeybUnibase NE uNewBase)
                        {
                            // Set the new base
                            uKeybUnibase = uNewBase;

                            // Get the associated item data (window handle of the Property Page)
                            hWndProp = (HWND)
                              SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_KEYBS - IDD_PROPPAGE_START, 0);

                            // Save the last highlight ID
                            idLHL = aKTC[uKeybTCNum].idLastHighlight;

                            // Display the new value
                            DisplayKeybTCValue (hWndProp, -1, TRUE);

                            // Restore the last highlight ID
                            aKTC[uKeybTCNum].idLastHighlight = idLHL;

                            // Highlight the last button
                            KeybHighlight (hWndProp, aKTC[uKeybTCNum].idLastHighlight);
                        } // End IF
                    } // End IF

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_KEYB_BN_MAKEACT:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ cmdCtl)
                    {
                        // Get the associated item data (window handle of the Property Page)
                        hWndProp = (HWND)
                          SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_KEYBS - IDD_PROPPAGE_START, 0);

                        // Set the active keyboard to the current selection
                        uLclKeybLayoutNumAct = uLclKeybLayoutNumVis;

                        // Save the layout name
                        MyStrcpyW (wszLclKeybLayoutAct, sizeof (wszLclKeybLayoutAct), wszLclKeybLayoutVis);

                        // Save the active keyboard layout
                        aKeybLayoutAct = lpLclKeybLayouts[uLclKeybLayoutNumAct];

                        // Change the show/hide state of the button and text
                        ShowWindow (GetDlgItem (hWndProp, IDC_KEYB_BN_MAKEACT), SW_HIDE);
                        ShowWindow (GetDlgItem (hWndProp, IDC_KEYB_LT_ISACT  ), SW_SHOW);

                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    } // End IF

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_KEYB_BN_COPY:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ cmdCtl)
                    {
                        NEWKEYBDLG newKeybDlg;

                        // Get the associated item data (window handle of the Property Page)
                        hWndProp = (HWND)
                          SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_KEYBS - IDD_PROPPAGE_START, 0);

                        // Fill in the parameters passed to the NewKeybDlgProc
                        newKeybDlg.lpwKeybLayoutName = wszLclKeybLayoutVis;
                        newKeybDlg.hWndProp          = hWndProp;

                        // Ask the user to type in a name for the new layout
                        if (DialogBoxParamW (_hInstance,
                                             MAKEINTRESOURCEW (IDD_NEWKEYB),
                                             hWndProp,
                                  (DLGPROC) &NewKeybDlgProc,
                                   (LPARAM) &newKeybDlg))
                        {
                            HGLOBAL hGlbReAlloc;

                            // We no longer need this ptr
                            MyGlobalUnlock (hLclKeybLayouts); lpLclKeybLayouts = NULL;

                            // Reallocate up the struc
                            //   moving the old data to the new location, and
                            //   freeing the old global memory
                            hGlbReAlloc =
                              MyGlobalReAlloc (hLclKeybLayouts, (uLclKeybLayoutCount + 1) * sizeof (KEYBLAYOUTS), GHND);

                            // If it failed, ...
                            if (hGlbReAlloc EQ NULL)
                            {
                                MessageBoxW (hDlg, L"Unable to allocate more memory for the new Keyboard Layout", WS_APPNAME, MB_OK | MB_ICONWARNING);

                                // Lock the memory to get a ptr to it
                                lpLclKeybLayouts = MyGlobalLock (hLclKeybLayouts);
                            } else
                            {
                                // Save the (new) handle
                                hLclKeybLayouts = hGlbReAlloc;

                                // Lock the memory to get a ptr to it
                                lpLclKeybLayouts = MyGlobalLock (hLclKeybLayouts);

                                // Copy the selected keyboard layout to the new slot at the end of the keyboard layouts
                                CopyMemory ((LPBYTE) &lpLclKeybLayouts[uLclKeybLayoutCount],
                                            (LPBYTE) &lpLclKeybLayouts[uLclKeybLayoutNumVis],
                                            sizeof (KEYBLAYOUTS));
                                // Make it the visible layout
                                uLclKeybLayoutNumVis = uLclKeybLayoutCount;

                                // Make it writable
                                lpLclKeybLayouts[uLclKeybLayoutNumVis].bReadOnly = FALSE;

                                // Copy the new layout name
                                strcpyW (lpLclKeybLayouts[uLclKeybLayoutNumVis].wszLayoutName, wszLclKeybLayoutVis);

                                // Add it to the ComboBox
                                SendMessageW (hWndKeybComboBox, CB_ADDSTRING, 0, (LPARAM) wszLclKeybLayoutVis);

                                // Count it in
                                uLclKeybLayoutCount++;
                                uLclKeybLayoutUser++;

                                // Get the CB index of the visible layout name
                                uCnt = (UINT)
                                  SendMessageW (hWndKeybComboBox, CB_FINDSTRING, -1, (LPARAM) wszLclKeybLayoutVis);

                                // Select it
                                SendMessageW (hWndKeybComboBox, CB_SETCURSEL, uCnt, 0);

                                // Display it
                                DispKeybLayout (hWndProp);

                                // Enable the Apply button
                                EnableWindow (hWndApply, TRUE);
                            } // End IF
                        } // End IF
                    } // End IF

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_KEYB_BN_DEL:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ cmdCtl)
                    {
                        // Get the associated item data (window handle of the Property Page)
                        hWndProp = (HWND)
                          SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_KEYBS - IDD_PROPPAGE_START, 0);

                        // If the visible selection is the active one, ...
                        if (uLclKeybLayoutNumVis EQ uLclKeybLayoutNumAct)
                            MessageBoxW (hDlg, L"Unable to delete the active Keyboard Layout -- make another layout active before deleting this one", WS_APPNAME, MB_OK | MB_ICONWARNING);
                        else
                        // Ask for confirmation from the user
                        if (MessageBoxW (hDlg, L"Are you sure you want to delete this Keyboard Layout?", WS_APPNAME, MB_YESNO | MB_ICONWARNING) EQ IDYES)
                        {
                            // Get the CB index of the visible selection
                            uCnt = (UINT)
                              SendMessageW (hWndKeybComboBox, CB_FINDSTRING, -1, (LPARAM) wszLclKeybLayoutVis);

                            // Delete it
                            SendMessageW (hWndKeybComboBox, CB_DELETESTRING, uCnt, 0);

                            // Count it out
                            uLclKeybLayoutCount--;
                            uLclKeybLayoutUser--;

                            // If the deleted keyb layout was the last one, ...
                            if (uLclKeybLayoutNumVis EQ uLclKeybLayoutCount)
                                uLclKeybLayoutNumVis--;
                            else
                                // Copy down the keyb layouts above this one
                                CopyMemory ((LPBYTE) &lpLclKeybLayouts[uLclKeybLayoutNumVis],
                                            (LPBYTE) &lpLclKeybLayouts[uLclKeybLayoutNumVis + 1],
                                            (uLclKeybLayoutCount - uLclKeybLayoutNumVis) * sizeof (KEYBLAYOUTS));
                            // We no longer need this ptr
                            MyGlobalUnlock (hLclKeybLayouts); lpLclKeybLayouts = NULL;

                            // Reallocate down the struc
                            MyGlobalReAlloc (hLclKeybLayouts, uLclKeybLayoutCount * sizeof (KEYBLAYOUTS), GMEM_MOVEABLE);

                            // Lock the memory to get a ptr to it
                            lpLclKeybLayouts = MyGlobalLock (hLclKeybLayouts);

                            // Set the visible keyb layout name
                            MyStrcpyW (wszLclKeybLayoutVis, sizeof (wszLclKeybLayoutVis), lpLclKeybLayouts[uLclKeybLayoutNumVis].wszLayoutName);

                            // Get the CB index of the visible layout name
                            uCnt = (UINT)
                              SendMessageW (hWndKeybComboBox, CB_FINDSTRING, -1, (LPARAM) wszLclKeybLayoutVis);

                            // Select it
                            SendMessageW (hWndKeybComboBox, CB_SETCURSEL, uCnt, 0);

                            // Display it
                            DispKeybLayout (hWndProp);

                            // Enable the Apply button
                            EnableWindow (hWndApply, TRUE);
                        } // End IF
                    } // End IF

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_KEYB_BN_FONT:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ cmdCtl)
                    {
                        // Ask the user to choose a font, and keep track of
                        //   whether or not a font was chosen
                        // If the font changed, ...
                        if (ChooseFontW (&cfKB))
                            // Set the font for the appropriate keyboard controls
                            //   from lfKB
                            SetKeybFont (NULL);
                    } // End IF

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_KEYB_XB_ALT:
                case IDC_KEYB_XB_CTRL:
                case IDC_KEYB_XB_SHIFT:
                {
                    static UINT aKS[] = {KS_ALT, KS_CTRL, KS_SHIFT};
                           UINT BITx;

                    // Get the associated item data (window handle of the Property Page)
                    hWndProp = (HWND)
                      SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_KEYBS - IDD_PROPPAGE_START, 0);

                    // Set the mask bit
                    BITx = aKS[idCtl - IDC_KEYB_XB_ALT];

                    // If it's checked, ...
                    if (IsDlgButtonChecked (hWndProp, idCtl))
                        // Set the bit
                        uKeybState |= BITx;
                    else
                        // Clear the bit
                        uKeybState &= ~BITx;

                    // Invalidate the KeyCap buttons
                    for (uCnt = 0; uCnt < KKC_CY; uCnt++)
                    for (uCol = aKKC_IDC_BEG[uCnt]; uCol <= aKKC_IDC_END[uCnt]; uCol++)
                        InvalidateRect (GetDlgItem (hWndProp, uCol), NULL, TRUE);

                    // Display or hide the keyboard Ctrl keycaps
                    DispKeybCtrlKeycaps (hWndProp);

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg
                } // End IDC_KEYB_XB_xxx

                case IDC_KEYB_RB_CLIP0:
                case IDC_KEYB_RB_CLIP1:
                    // Get the associated item data (window handle of the Property Page)
                    hWndProp = (HWND)
                      SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_KEYBS - IDD_PROPPAGE_START, 0);

                    // Set or clear the bit
                    lpLclKeybLayouts[uLclKeybLayoutNumVis].bUseCXV = IsDlgButtonChecked (hWndProp, IDC_KEYB_RB_CLIP0);

                    // If it's now set, ...
                    if (lpLclKeybLayouts[uLclKeybLayoutNumVis].bUseCXV)
                    {
                        lpLclKeybLayouts[uLclKeybLayoutNumVis].aCharCodes[KeybCharToScanCode (L'c', KS_NONE)].wc[KS_CTRL] =
                        lpLclKeybLayouts[uLclKeybLayoutNumVis].aCharCodes[KeybCharToScanCode (L'x', KS_NONE)].wc[KS_CTRL] =
                        lpLclKeybLayouts[uLclKeybLayoutNumVis].aCharCodes[KeybCharToScanCode (L'v', KS_NONE)].wc[KS_CTRL] = 0;
                    } // End IF

                    // Display or hide the keyboard Ctrl keycaps
                    DispKeybCtrlKeycaps (hWndProp);

                    // Enable the Apply button
                    EnableWindow (hWndApply, TRUE);

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_KEYB_RB_UNDO0:
                case IDC_KEYB_RB_UNDO1:
                    // Get the associated item data (window handle of the Property Page)
                    hWndProp = (HWND)
                      SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_KEYBS - IDD_PROPPAGE_START, 0);

                    // Set or clear the bit
                    lpLclKeybLayouts[uLclKeybLayoutNumVis].bUseZY = IsDlgButtonChecked (hWndProp, IDC_KEYB_RB_UNDO0);

                    // If it's now set, ...
                    if (lpLclKeybLayouts[uLclKeybLayoutNumVis].bUseZY)
                    {
                        lpLclKeybLayouts[uLclKeybLayoutNumVis].aCharCodes[KeybCharToScanCode (L'z', KS_NONE)].wc[KS_CTRL] =
                        lpLclKeybLayouts[uLclKeybLayoutNumVis].aCharCodes[KeybCharToScanCode (L'y', KS_NONE)].wc[KS_CTRL] = 0;
                    } // End IF

                    // Display or hide the keyboard Ctrl keycaps
                    DispKeybCtrlKeycaps (hWndProp);

                    // Enable the Apply button
                    EnableWindow (hWndApply, TRUE);

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_KEYB_RB_FNED0:
                case IDC_KEYB_RB_FNED1:
                    // Get the associated item data (window handle of the Property Page)
                    hWndProp = (HWND)
                      SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_KEYBS - IDD_PROPPAGE_START, 0);

                    // Set or clear the bit
                    lpLclKeybLayouts[uLclKeybLayoutNumVis].bUseSEQ = IsDlgButtonChecked (hWndProp, IDC_KEYB_RB_FNED0);

                    // If it's now set, ...
                    if (lpLclKeybLayouts[uLclKeybLayoutNumVis].bUseSEQ)
                    {
                        lpLclKeybLayouts[uLclKeybLayoutNumVis].aCharCodes[KeybCharToScanCode (L's', KS_NONE)].wc[KS_CTRL] =
                        lpLclKeybLayouts[uLclKeybLayoutNumVis].aCharCodes[KeybCharToScanCode (L'e', KS_NONE)].wc[KS_CTRL] =
                        lpLclKeybLayouts[uLclKeybLayoutNumVis].aCharCodes[KeybCharToScanCode (L'q', KS_NONE)].wc[KS_CTRL] = 0;
                    } // End IF

                    // Display or hide the keyboard Ctrl keycaps
                    DispKeybCtrlKeycaps (hWndProp);

                    // Enable the Apply button
                    EnableWindow (hWndApply, TRUE);

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                //***************************************************************
                // USER PREFERENCES -- WM_COMMAND
                //***************************************************************

                case IDC_USER_PREFS_BN_UNICODE:
                    // Split cases based upon the command
                    switch (cmdCtl)
                    {
                        UBOOL bCallbackBN;

                        case EN_CHANGE:
                            // Get the associated item data (window handle of the Property Page)
                            hWndProp = (HWND)
                              SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_USER_PREFS - IDD_PROPPAGE_START, 0);

                            // Get the character
                            SendMessageW (GetDlgItem (hWndProp, IDC_USER_PREFS_BN_UNICODE), WM_GETTEXT, 1 + 1, (LPARAM) wszStr);

                            // Save for later use
                            uLclUserChar = wszStr[0];

                            // If we're to callback to EC, ...
                            if (gbCallbackEC)
                            {
                                // Convert the Line Continuation Marker to a string
                                MySprintfW (wszStr,
                                            sizeof (wszStr),
                                            (uUserUnibase EQ 10) ? L"%u" : L"%05X",   // The "5" here is KEYB_UNICODE_LIMIT
                                            uLclUserChar);
                                // Tell EC_UNICODE not to callback to us
                                 bCallbackBN = gbCallbackBN;
                                gbCallbackBN = FALSE;

                                // Initialize the value in the Unicode Edit Ctrl
                                SetWindowTextW (GetDlgItem (hWndProp, IDC_USER_PREFS_EC_UNICODE), wszStr);

                                // Restore the old value
                                gbCallbackBN = bCallbackBN;
                            } // End IF

                            // Enable the Apply button
                            EnableWindow (hWndApply, TRUE);

                            break;
                    } // End SWITCH

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_USER_PREFS_EC_UNICODE:
                    // Split cases based upon the command
                    switch (cmdCtl)
                    {
                        UBOOL bCallbackEC;

                        case EN_CHANGE:
                            // Get the associated item data (window handle of the Property Page)
                            hWndProp = (HWND)
                              SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_USER_PREFS - IDD_PROPPAGE_START, 0);

                            // Get the text
                            SendMessageW (GetDlgItem (hWndProp, IDC_USER_PREFS_EC_UNICODE), WM_GETTEXT, KEYB_UNICODE_LIMIT + 1, (LPARAM) wszStr);

                            // Convert the text to a binary value using base uUserUnibase
                            lpwszStr = &wszStr[0];
                            uValue = 0;
                            while (*lpwszStr)
                            {
                                // Shift over the value
                                uValue *= uUserUnibase;

                                if (L'0' <= *lpwszStr && *lpwszStr <= L'9')
                                    uValue += *lpwszStr - L'0';
                                else
                                    uValue += 10 + tolowerW (*lpwszStr) - L'a';

                                // Skip to the next value
                                lpwszStr++;
                            } // End WHILE

                            // Save the value locally
                            uLclUserChar = uValue;

                            // If we're to callback to BN, ...
                            if (gbCallbackBN)
                            {
                                // Tell BN_UNICODE not to callback to us
                                 bCallbackEC = gbCallbackEC;
                                gbCallbackEC = FALSE;

                                // Display the value
                                wszText[0] = (WCHAR) uValue;
                                SendMessageW (GetDlgItem (hWndProp, IDC_USER_PREFS_BN_UNICODE), WM_SETTEXT, 0, (LPARAM) wszText);

                                // Restore the old value
                                gbCallbackEC = bCallbackEC;
                            } // End IF

                            // Enable the Apply button
                            EnableWindow (hWndApply, TRUE);

                            break;
                    } // End SWITCH

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_USER_PREFS_RB_DEC:
                case IDC_USER_PREFS_RB_HEX:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ cmdCtl)
                    {
                        UINT uNewBase;

                        // Calculate the new base
                        uNewBase = (idCtl EQ IDC_USER_PREFS_RB_DEC) ? 10 : 16;

                        // If it's different, ...
                        if (uUserUnibase NE uNewBase)
                        {
                            // Set the new base
                            uUserUnibase = uNewBase;

                            // Get the associated item data (window handle of the Property Page)
                            hWndProp = (HWND)
                              SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_USER_PREFS - IDD_PROPPAGE_START, 0);

                            // Display the new value
                            DisplayKeybTCValue (hWndProp, -1, FALSE);
                        } // End IF
                    } // End IF

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                default:
                    //***************************************************************
                    // SYNTAX COLORING -- WM_COMMAND
                    //***************************************************************
                    // Check to see if this is one of our Syntax Coloring Transparent checkboxes
                    if (IDC_SYNTCLR_XB_TRANS1 <= idCtl
                     &&                          idCtl <= IDC_SYNTCLR_XB_TRANS_LAST
                      && BN_CLICKED EQ cmdCtl)
                    {
                        // We care about BN_CLICKED only
                        if (BN_CLICKED EQ cmdCtl)
                        {
                            // Get the associated item data (window handle of the Property Page)
                            hWndProp = (HWND)
                              SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_SYNTAX_COLORING - IDD_PROPPAGE_START, 0);

                            // Get the index of the Background color button
                            uCnt = idCtl - IDC_SYNTCLR_XB_TRANS1;

                            // Get the show/hide state
                            uShow = IsDlgButtonChecked (hWndProp, IDC_SYNTCLR_XB_TRANS1 + uCnt) ? SW_HIDE : SW_SHOWNORMAL;

                            // Show/hide the Background color button as appropriate
                            ShowWindow (GetDlgItem (hWndProp, IDC_SYNTCLR_BN_BGCLR1 + uCnt), uShow);

                            // Enable the Apply button
                            EnableWindow (hWndApply, TRUE);
                        } // End IF

                        // Return dialog result
                        DlgMsgDone (hDlg);              // We handled the msg
                    } else
                    // Check to see if this is one of our Syntax Coloring Foreground/Background Color buttons
                    if (((IDC_SYNTCLR_BN_FGCLR1 <= idCtl
                       &&                          idCtl <= IDC_SYNTCLR_BN_FGCLR_LAST)
                      || (IDC_SYNTCLR_BN_BGCLR1 <= idCtl
                       &&                          idCtl <= IDC_SYNTCLR_BN_BGCLR_LAST))
                      && BN_CLICKED EQ cmdCtl)
                    {
                        HMENU         hMenu;                // Handle to popup menu
                        MENUITEMINFOW mii = {0};            // Menu item info for popup menu
                        COLORREF      clrCmp;               // Color to compare with the scMenuItems color
                        POINT         ptScr;                // Cursor point in screen coords
                        UINT          uRetCmd;              // Return cmd from TrackPopupMenu

                        // Get the associated item data (window handle of the Property Page)
                        hWndProp = (HWND)
                          SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_SYNTAX_COLORING - IDD_PROPPAGE_START, 0);

                        // Determine if this is Foreground or Background
                        gbFore = (IDC_SYNTCLR_BN_FGCLR1 <= idCtl
                               &&                          idCtl <= IDC_SYNTCLR_BN_FGCLR_LAST);

                        // Get the index of the Foreground or Background color button
                        guIndex = idCtl - (gbFore ? IDC_SYNTCLR_BN_FGCLR1 : IDC_SYNTCLR_BN_BGCLR1);

                        // Prepare to display a popup menu with the color choices
                        hMenu = CreatePopupMenu ();
                        if (!hMenu)
                        {
                            MBW (L"Unable to create popup menu");

                            // Return dialog result
                            DlgMsgDone (hDlg);              // We handled the msg
                        } // End IF

                        // Fill in static info
                        mii.cbSize        = sizeof (mii);
                        mii.fMask         = MIIM_DATA | MIIM_ID | MIIM_STATE | MIIM_TYPE;
                        mii.fType         = MFT_OWNERDRAW;
                        mii.fState        = 0;
                        mii.hSubMenu      = NULL;
                        mii.hbmpChecked   = NULL;
                        mii.hbmpUnchecked = NULL;
                        mii.dwTypeData    = 0;
                        mii.cch           = 0;

                        // Get the color with which to compare
                        clrCmp = gbFore ? lclSyntaxColors[guIndex].crFore
                                        : lclSyntaxColors[guIndex].crBack;

                        // Loop through the basic colors
                        for (uCnt = 0; uCnt < countof (scMenuItems); uCnt++)
                        {
                            // Fill in the dynamic info
                            mii.wID           = IDC_SYNTCLR_MI1 + uCnt;
                            mii.dwItemData    = (clrCmp EQ scMenuItems[uCnt].clrRef);

                            // Insert the menu item
                            InsertMenuItemW (hMenu, uCnt, TRUE, &mii);
                        } // End FOR

                        // Get the mouse position in screen coordinates
                        GetCursorPos (&ptScr);

                        // Tell the static text that this is the one
                        SendDlgItemMessageW (hWndProp,
                                             (gbFore ? IDC_SYNTCLR_LT_FGMRK1 : IDC_SYNTCLR_LT_BGMRK1) + guIndex,
                                             STM_SETIMAGE,
                                             IMAGE_BITMAP,
                                             (LPARAM) (HANDLE) hBitmapCheck);
                        // Display the popup menu
                        uRetCmd =
                          TrackPopupMenu (hMenu,            // Handle
                                          0                 // Flags
                                        | TPM_CENTERALIGN
                                        | TPM_LEFTBUTTON
                                        | TPM_RIGHTBUTTON
                                        | TPM_NONOTIFY
                                        | TPM_RETURNCMD
                                          ,
                                          ptScr.x,          // x-position
                                          ptScr.y,          // y-position
                                          0,                // Reserved (must be zero)
                                          hDlg,             // Handle of owner window
                                          NULL);            // Dismissal area outside rectangle (none)
                        // We no longer need this resource
                        DestroyMenu (hMenu); hMenu = NULL;

                        // Get the colorref
                        cc.rgbResult = gbFore ? lclSyntaxColors[guIndex].crFore : lclSyntaxColors[guIndex].crBack;

                        // Split cases based upon the return cmd
                        switch (uRetCmd)
                        {
                            case IDC_SYNTCLR_MI_CUSTOM:
                                // Fill in the dynamic members of the CHOOSECOLORW struc
////////////////////////////////cc.lStructSize    =                         // Already filled in during initialization
////////////////////////////////cc.hwndOwner      =                         // Already filled in during initialization
////////////////////////////////cc.hInstance      = NULL;                   // Already zero from = {0}
////////////////////////////////cc.rgbResult      =                         // Already filled in above
////////////////////////////////cc.lpCustColors   =                         // Already filled in during initialization
////////////////////////////////cc.Flags          =                         // Already filled in during initialization
////////////////////////////////cc.lCustData      = 0;                      // Already zero from = {0}
////////////////////////////////cc.lpfnHook       =                         // Already filled in during initialization
////////////////////////////////cc.lpTemplateName = NULL;                   // Already zero from = {0}

                                // If the user presses OK, ...
                                if (ChooseColorW (&cc))
                                    // Handle in separate message
                                    SendMessageW (hDlg,
                                                  MYWM_SETCOLOR,
                                                  MAKEWPARAM (guIndex, gbFore),
                                                  cc.rgbResult);
                                // Tell the static text to disappear again
                                SendDlgItemMessageW (hWndProp,
                                                     (gbFore ? IDC_SYNTCLR_LT_FGMRK1 : IDC_SYNTCLR_LT_BGMRK1) + guIndex,
                                                     STM_SETIMAGE,
                                                     IMAGE_BITMAP,
                                                     0);
                                break;

                            case IDC_SYNTCLR_MI_WEBCOLORS:
                            {
                                SYNTAXCOLORNAME scnParams;

                                // Initialize the parameters we're passing
                                scnParams.syntClr.crFore = cc.rgbResult;
                                scnParams.lpwSCName      = gSyntaxColorName[guIndex].lpwSCName;

                                // Display a separate dialog with the Web Color Names
                                cc.rgbResult = (COLORREF)
                                  DialogBoxParamW (_hInstance,
                                                   MAKEINTRESOURCEW (IDD_WEBCOLORS),
                                                   hWnd,
                                        (DLGPROC) &WebColorsDlgProc,
                           (LPARAM) (HANDLE_PTR)  &scnParams);
                                if (-1 NE cc.rgbResult)
                                    // Handle in separate message
                                    SendMessageW (hDlg,
                                                  MYWM_SETCOLOR,
                                                  MAKEWPARAM (guIndex, gbFore),
                                                  cc.rgbResult);
                                // Tell the static text to disappear again
                                SendDlgItemMessageW (hWndProp,
                                                     (gbFore ? IDC_SYNTCLR_LT_FGMRK1 : IDC_SYNTCLR_LT_BGMRK1) + guIndex,
                                                     STM_SETIMAGE,
                                                     IMAGE_BITMAP,
                                                     0);
                                break;
                            } // End IDC_SYNTCLR_MI_WEBCOLORS

                            default:                // IDC_SYNTCLR_MI1 to the end-1
                                // Handle in separate message
                                SendMessageW (hDlg,
                                              MYWM_SETCOLOR,
                                              MAKEWPARAM (guIndex, gbFore),
                                              scMenuItems[uRetCmd - IDC_SYNTCLR_MI1].clrRef);

                                // Fall through to remove the mark

                            case 0:
                                // Tell the static text to disappear again
                                SendDlgItemMessageW (hWndProp,
                                                     (gbFore ? IDC_SYNTCLR_LT_FGMRK1 : IDC_SYNTCLR_LT_BGMRK1) + guIndex,
                                                     STM_SETIMAGE,
                                                     IMAGE_BITMAP,
                                                     0);
                                break;
                        } // End SWITCH

                        // Return dialog result
                        DlgMsgDone (hDlg);              // We handled the msg
                    } else
                    //***************************************************************
                    // KEYBOARDS -- WM_COMMAND
                    //***************************************************************
                    if (IDC_KEYB_BN_TC_00 <= idCtl
                     &&                      idCtl <= IDC_KEYB_BN_TC_LAST
                     && BN_CLICKED EQ cmdCtl)
                    {
                        WCHAR wszAmp[3] = {L'&', L'&', L'\0'};

                        // Get the associated item data (window handle of the Property Page)
                        hWndProp = (HWND)
                          SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_KEYBS - IDD_PROPPAGE_START, 0);

                        // Clear the highlight from the last button
                        KeybHighlight (hWndProp, NOCONTROL);

                        // In case the value is an ampersand, ...
                        SendMessageW (GetDlgItem (hWndProp, idCtl), WM_GETTEXT, KEYB_UNICODE_LIMIT + 1, (LPARAM) wszText);
                        if (wszText[0] EQ L'&')
                            // Change the text to double the ampersand so it displays as a single ampersand
                            SendMessageW (GetDlgItem (hWndProp, idCtl), WM_SETTEXT, 0, (LPARAM) wszAmp);

                        // Display the new value
                        DisplayKeybTCValue (hWndProp, idCtl, TRUE);

                        // Save as the new ID
                        aKTC[uKeybTCNum].idLastHighlight = idCtl;

                        // Highlight this button
                        KeybHighlight (hWndProp, aKTC[uKeybTCNum].idLastHighlight);

                        // Return dialog result
                        DlgMsgDone (hDlg);              // We handled the msg
                    } else
                    if (IDC_KEYB_BN_KC_00 <= idCtl
                     &&                      idCtl <= IDC_KEYB_BN_KC_LAST
                     && BN_CLICKED EQ cmdCtl)
                    {
                        // Get the associated item data (window handle of the Property Page)
                        hWndProp = (HWND)
                          SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_KEYBS - IDD_PROPPAGE_START, 0);

                        // If the Keyboard Layout is writable, ...
                        if (!lpLclKeybLayouts[uLclKeybLayoutNumVis].bReadOnly)
                        {
                            // Copy the value in the Unicode button to the corresponding position in aCharCodes

                            // Get the value to copy
                            SendMessageW (GetDlgItem (hWndProp, IDC_KEYB_BN_UNICODE), WM_GETTEXT, KEYB_UNICODE_LIMIT + 1, (LPARAM) wszText);

                            // Get the row of the keycap
                            for (uCnt = 0; uCnt < KKC_CY; uCnt++)
                            if ((UINT) idCtl <= aKKC_IDC_END[uCnt])
                                break;

                            // Get the scan code
                            uScanCode = aKKC_SC[uCnt].aSC[idCtl - aKKC_IDC_BEG[uCnt]];

                            // If the ScanCode is within range of our table, and
                            //   the keystate is Ctrl, and
                            //   we're using Ctrl-C, -X, -V, -Z, -Y
                            //   and this ScanCode is one of those, ...
                            if (uScanCode < lpLclKeybLayouts[uLclKeybLayoutNumVis].uCharCodesLen
                             && uKeybState EQ KS_CTRL
                             && ((lpLclKeybLayouts[uLclKeybLayoutNumVis].bUseCXV
                               && (uScanCode EQ KeybCharToScanCode (L'c', KS_NONE)
                                || uScanCode EQ KeybCharToScanCode (L'x', KS_NONE)
                                || uScanCode EQ KeybCharToScanCode (L'v', KS_NONE)))
                              || (lpLclKeybLayouts[uLclKeybLayoutNumVis].bUseZY
                               && (uScanCode EQ KeybCharToScanCode (L'z', KS_NONE)
                                || uScanCode EQ KeybCharToScanCode (L'y', KS_NONE)))
                              || (lpLclKeybLayouts[uLclKeybLayoutNumVis].bUseSEQ
                               && (uScanCode EQ KeybCharToScanCode (L's', KS_NONE)
                                || uScanCode EQ KeybCharToScanCode (L'e', KS_NONE)
                                || uScanCode EQ KeybCharToScanCode (L'q', KS_NONE)))))
                            {
                                // Sound the alarum!
                                MessageBeep (MB_ICONERROR);
                            } else
                            {
                                // Save the new value
                                lpLclKeybLayouts[uLclKeybLayoutNumVis].aCharCodes[uScanCode].wc[uKeybState] = wszText[0];

                                // Enable the Apply button
                                EnableWindow (hWndApply, TRUE);

                                // Display the new value
                                InvalidateRect (GetDlgItem (hWndProp, idCtl), NULL, TRUE);
                            } // End IF
                        } else
                        {
                            // Complain to the user
                            MessageBoxW (hDlg,
                                         L"This Keyboard Layout is Read-Only.  Copy this layout to a writable one with the \"Copy\" button before changing it.",
                                         WS_APPNAME,
                                         MB_OK
                                       | MB_ICONWARNING);
                            // Redraw the Keycap
                            InvalidateRect (GetDlgItem (hWndProp, idCtl), NULL, TRUE);
                        } // End IF/ELSE

                        // Return dialog result
                        DlgMsgDone (hDlg);              // We handled the msg
                    } // End IF/ELSE/...

                    break;

                case IDC_KEYB_CB_LAYOUT:
                    // We care about CBN_SELCHANGE only
                    if (CBN_SELCHANGE EQ cmdCtl)
                    {
                        // Get the associated item data (window handle of the Property Page)
                        hWndProp = (HWND)
                          SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_KEYBS - IDD_PROPPAGE_START, 0);

                        // Get the CB index of the visible selection
                        uSel = (UINT)
                          SendMessageW (hWndKeybComboBox, CB_GETCURSEL, 0, 0);

                        // Get the name of the visible selection
                        SendMessageW (hWndKeybComboBox, CB_GETLBTEXT, uSel, (LPARAM) wszLclKeybLayoutVis);

                        // Loop through the Keyboard Layouts
                        for (uLclKeybLayoutNumVis = -1, uCnt = 0; uCnt < uLclKeybLayoutCount; uCnt++)
                        // See if this is the visible keyboard layout
                        if (lstrcmpW (wszLclKeybLayoutVis, lpLclKeybLayouts[uCnt].wszLayoutName) EQ 0)
                        {
                            uLclKeybLayoutNumVis = uCnt;
                            break;
                        } // End IF

                        // If not found, ...
                        if (uLclKeybLayoutNumVis EQ -1)
                            uLclKeybLayoutNumVis = uLclKeybLayoutNumAct;

                        // Display the new Keyboard Layout selection
                        DispKeybLayout (hWndProp);

                        // Return dialog result
                        DlgMsgDone (hDlg);              // We handled the msg
                    } // End IF

                    break;

                case IDC_SYNTCLR_BN_RESTORE:
                    // Get the associated item data (window handle of the Property Page)
                    hWndProp = (HWND)
                      SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_SYNTAX_COLORING - IDD_PROPPAGE_START, 0);

                    // Restore the default colors
                    for (uCnt = 0; uCnt < SC_LENGTH; uCnt++)
                    {
                        // Set the "Background Transparent" button default states
                        CheckDlgButton (hWndProp, IDC_SYNTCLR_XB_TRANS1 + uCnt, defSyntClrBGTrans[uCnt]);

                        // Get the show/hide state
                        uShow = defSyntClrBGTrans[uCnt] ? SW_HIDE : SW_SHOWNORMAL;

                        // Show/hide the Background color button as appropriate
                        ShowWindow (GetDlgItem (hWndProp, IDC_SYNTCLR_BN_BGCLR1 + uCnt), uShow);

                        // Restore the default Foreground and Background colors
                        lclSyntaxColors[uCnt] = defSyntaxColors[uCnt];

                        // Repaint the Foreground button
                        InvalidateRect (GetDlgItem (hWndProp, IDC_SYNTCLR_BN_FGCLR1 + uCnt),
                                        NULL, FALSE);
                        // Repaint the Background button
                        InvalidateRect (GetDlgItem (hWndProp, IDC_SYNTCLR_BN_BGCLR1 + uCnt),
                                        NULL, FALSE);
                    } // End FOR

                    // Restore default checkbox states
                    CheckDlgButton (hWndProp, IDC_SYNTCLR_XB_CLRFCNS, DEF_SYNTCLRFCNS);
                    CheckDlgButton (hWndProp, IDC_SYNTCLR_XB_CLRSESS, DEF_SYNTCLRSESS);
                    CheckDlgButton (hWndProp, IDC_SYNTCLR_XB_CLRPRNT, DEF_SYNTCLRPRNT);

                    // Enable the Apply button
                    EnableWindow (hWndApply, TRUE);

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg


                //***************************************************************
                // SYSTEM VAR RESET -- WM_COMMAND
                //***************************************************************

                case IDC_RESET_CT_RADIO1:
                case IDC_RESET_CT_RADIO2:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ cmdCtl)
                    {
                        // Save the new setting in our local copy
                        lclResetVars.CT = (IDC_RESET_CT_RADIO2 EQ idCtl);

                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    } // End IF

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_RESET_DT_RADIO1:
                case IDC_RESET_DT_RADIO2:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ cmdCtl)
                    {
                        // Save the new setting in our local copy
                        lclResetVars.DT = (IDC_RESET_DT_RADIO2 EQ idCtl);

                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    } // End IF

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_RESET_FC_RADIO1:
                case IDC_RESET_FC_RADIO2:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ cmdCtl)
                    {
                        // Save the new setting in our local copy
                        lclResetVars.FC = (IDC_RESET_FC_RADIO2 EQ idCtl);

                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    } // End IF

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_RESET_FEATURE_RADIO1:
                case IDC_RESET_FEATURE_RADIO2:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ cmdCtl)
                    {
                        // Save the new setting in our local copy
                        lclResetVars.FEATURE = (IDC_RESET_FEATURE_RADIO2 EQ idCtl);

                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    } // End IF

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_RESET_FPC_RADIO1:
                case IDC_RESET_FPC_RADIO2:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ cmdCtl)
                    {
                        // Save the new setting in our local copy
                        lclResetVars.FPC = (IDC_RESET_FPC_RADIO2 EQ idCtl);

                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    } // End IF

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_RESET_IC_RADIO1:
                case IDC_RESET_IC_RADIO2:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ cmdCtl)
                    {
                        // Save the new setting in our local copy
                        lclResetVars.IC = (IDC_RESET_IC_RADIO2 EQ idCtl);

                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    } // End IF

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_RESET_IO_RADIO1:
                case IDC_RESET_IO_RADIO2:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ cmdCtl)
                    {
                        // Save the new setting in our local copy
                        lclResetVars.IO = (IDC_RESET_IO_RADIO2 EQ idCtl);

                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    } // End IF

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_RESET_PP_RADIO1:
                case IDC_RESET_PP_RADIO2:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ cmdCtl)
                    {
                        // Save the new setting in our local copy
                        lclResetVars.PP = (IDC_RESET_PP_RADIO2 EQ idCtl);

                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    } // End IF

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_RESET_PW_RADIO1:
                case IDC_RESET_PW_RADIO2:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ cmdCtl)
                    {
                        // Save the new setting in our local copy
                        lclResetVars.PW = (IDC_RESET_PW_RADIO2 EQ idCtl);

                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    } // End IF

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_RESET_RL_RADIO1:
                case IDC_RESET_RL_RADIO2:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ cmdCtl)
                    {
                        // Save the new setting in our local copy
                        lclResetVars.RL = (IDC_RESET_RL_RADIO2 EQ idCtl);

                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    } // End IF

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg


                //***************************************************************
                // TAB COLORS -- WM_COMMAND
                //***************************************************************







                //***************************************************************
                // USER PREFERENCES -- WM_COMMAND
                //***************************************************************

                case IDC_USER_PREFS_XB_ADJUSTPW:
                case IDC_USER_PREFS_XB_UNDERBARTOLOWERCASE:
                case IDC_USER_PREFS_XB_NEWTABONCLEAR:
                case IDC_USER_PREFS_XB_NEWTABONLOAD:
                case IDC_USER_PREFS_XB_USELOCALTIME:
                case IDC_USER_PREFS_XB_BACKUPONLOAD:
                case IDC_USER_PREFS_XB_BACKUPONSAVE:
                case IDC_USER_PREFS_XB_NOCOPYRIGHTMSG:
                case IDC_USER_PREFS_XB_CHECKGROUP:
                case IDC_USER_PREFS_XB_INSSTATE:
                case IDC_USER_PREFS_XB_REVDBLCLK:
                case IDC_USER_PREFS_XB_DEFDISPFCNLINENUMS:
                case IDC_USER_PREFS_XB_DISPMPSUF:
                case IDC_USER_PREFS_XB_OUTPUTDEBUG:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ cmdCtl)
                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDC_USER_PREFS_CB_DEFAULTPASTE:
                case IDC_USER_PREFS_CB_DEFAULTCOPY:
                case IDC_USER_PREFS_CB_UPDFRQ:
                    // We care about CBN_SELCHANGE only
                    if (CBN_SELCHANGE EQ cmdCtl)
                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg
            } // End SWITCH

            break;
#ifndef DEBUG
  #undef  hwndCtl
  #undef  cmdCtl
  #undef  idCtl
#endif
        } // End WM_COMMAND

#define uIndex          LOWORD (wParam)
#define bFore           HIWORD (wParam)
#define clrRef          ((COLORREF) lParam)
        case MYWM_SETCOLOR:                 // uIndex = LOWORD (wParam);    // Index of button
                                            // bFore  = HIWORD (wParam);    // TRUE iff Foreground
                                            // clrRef = (COLORREF) lParam;  // Color to set
            // Get the associated item data (window handle of the Property Page)
            hWndProp = (HWND)
              SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_SYNTAX_COLORING - IDD_PROPPAGE_START, 0);

            // Save the color in a local var
            if (bFore)
                lclSyntaxColors[uIndex].crFore = clrRef;
            else
                lclSyntaxColors[uIndex].crBack = clrRef;

            // Repaint the button
            InvalidateRect (GetDlgItem (hWndProp,
                                        (bFore ? IDC_SYNTCLR_BN_FGCLR1 : IDC_SYNTCLR_BN_BGCLR1) + uIndex),
                            NULL, FALSE);
            // Enable the Apply button
            EnableWindow (hWndApply, TRUE);

            // Return dialog result
            DlgMsgDone (hDlg);              // We handled the msg
#undef  clrRef
#undef  bFore
#undef  uIndex

        case WM_QUERYENDSESSION:
        case WM_CLOSE:
            // Simulate a click of the Cancel button
            SendMessageW (hDlg, WM_COMMAND, GET_WM_COMMAND_MPS (IDCANCEL, NULL, BN_CLICKED));

            // Return dialog result
            DlgMsgDone (hDlg);              // We handled the msg

        case WM_DESTROY:
            // Set the index of the Syntax Coloring Property Page
            wParam = IDD_PROPPAGE_SYNTAX_COLORING - IDD_PROPPAGE_START;

            // If the struc has been initialized, ...
            if (custStruc[wParam].bInitialized)
            {
                // Get the handle of the Syntax Coloring Property Page
                hWndProp = (HWND)
                  SendMessageW (hWndListBox, LB_GETITEMDATA, wParam, 0);

                // Loop through the Foreground and Background buttons
                for (uCnt = 0; uCnt < SC_LENGTH; uCnt++)
                {
                    // Fill in the dynamic text field
                    tti.uId = (INT_PTR)
                      GetDlgItem (hWndProp, IDC_SYNTCLR_BN_FGCLR1 + uCnt);

                    // Unregister a tooltip for the Syntax Coloring Foreground button
                    SendMessageW (hWndTT,
                                  TTM_DELTOOLW,
                                  0,
                                  (LPARAM) (LPTOOLINFOW) &tti);
                    // Fill in the dynamic field
                    tti.uId = (INT_PTR)
                      GetDlgItem (hWndProp, IDC_SYNTCLR_BN_BGCLR1 + uCnt);

                    // Unregister a tooltip for the Syntax Coloring Background button
                    SendMessageW (hWndTT,
                                  TTM_DELTOOLW,
                                  0,
                                  (LPARAM) (LPTOOLINFOW) &tti);
                } // End FOR

                // Clear for next time
                custStruc[wParam].bInitialized = FALSE;
            } // End IF

            // Set the index of the Keyboards Property Page
            wParam = IDD_PROPPAGE_KEYBS - IDD_PROPPAGE_START;

            // If the struc has been initialized, ...
            if (custStruc[wParam].bInitialized)
            {
                // If it's still locked, ...
                if (lpLclKeybLayouts NE NULL)
                {
                    // We no longer need this ptr
                    MyGlobalUnlock (hLclKeybLayouts); lpLclKeybLayouts = NULL;
                } // End IF

                // If it's still valid, ...
                if (hLclKeybLayouts NE NULL)
                {
                    // Delete the local copy
                    DbgGlobalFree (hLclKeybLayouts); hLclKeybLayouts = NULL;
                } // End IF

                // If it's still valid, ...
                if (hFontKB NE NULL)
                {
                    MyDeleteObject (hFontKB); hFontKB = NULL;
                } // End IF

                // Clear for next time
                custStruc[wParam].bInitialized = FALSE;
            } // End IF

            // Set the index of the CLEARWS Values Property Page
            wParam = IDD_PROPPAGE_CLEARWS_VALUES - IDD_PROPPAGE_START;

            // If the struc has been initialized, ...
            if (custStruc[wParam].bInitialized)
            {
                // If it's still valid, ...
                if (hFontCWS NE NULL)
                {
                    MyDeleteObject (hFontCWS); hFontCWS = NULL;
                } // End IF

                // Clear for next time
                custStruc[wParam].bInitialized = FALSE;
            } // End IF

            // If the theme library is loaded, ...
            if (bThemeLibLoaded)
            {
                // If the active theme for Keybs is loaded, ...
                if (hThemeKeybs NE NULL)
                {
                    // Close the theme for Keybs
                    zCloseThemeData (hThemeKeybs); hThemeKeybs = NULL;
                } // End IF

                // Close the theme library
                FinThemes ();
            } // End IF

            // Free allocated resources
            if (hFontBold_ST NE NULL)
            {
                MyDeleteObject (hFontBold_ST); hFontBold_ST = NULL;
            } // End IF

            // Mark as no longer active
            ghDlgCustomize = NULL;

            // Return dialog result
            DlgMsgDone (hDlg);          // We handled the msg
    } // End SWITCH

    // Return dialog result
    DlgMsgPass (hDlg);      // We didn't handle the msg
} // End CustomizeDlgProc
#undef  APPEND_NAME


//***************************************************************************
//  $LclKeybEditCtrlWndProc
//
//  Local window procedure for the Customize's EDITCTRL
//***************************************************************************

LRESULT WINAPI LclKeybEditCtrlWndProc
    (HWND   hWnd,       // Window handle
     UINT   message,    // Type of message
     WPARAM wParam,     // Additional information
     LPARAM lParam)     // ...

{
////LCLODSAPI ("LEC: ", hWnd, message, wParam, lParam);

    // Split cases
    switch (message)
    {
        case WM_CHAR:               // wchCharCode = (TCHAR) wParam; // character code
                                    // lKeyData = lParam;           // Key data
        {
#ifdef DEBUG
            WCHAR   wchCharCode = (WCHAR) wParam;
////        KEYDATA keyData     = *(LPKEYDATA) &lParam;
#else
  #define wchCharCode   ((WCHAR) wParam)
  #define keyData       (*(LPKEYDATA) &lParam)
#endif
            UINT uUnibase;

            Assert (gInitCustomizeCategory EQ CAT_KEYBS
                 || gInitCustomizeCategory EQ CAT_USER_PREFS);

            // Get the corresponding Unibase
            uUnibase = (gInitCustomizeCategory EQ CAT_KEYBS) ? uKeybUnibase : uUserUnibase;

            // Ensure that the incoming char is one we allow for this base
            if (wchCharCode EQ WC_BS
             || (L'0' <= wchCharCode && wchCharCode <= L'9'
              || (uUnibase EQ 16
               && L'a' <= tolowerW (wchCharCode) && tolowerW (wchCharCode) <= L'f')))
                break;

            // Otherwise, sound the alarm
            MessageBeep (NEG1U);

            return FALSE;       // We handled the msg
        } // End WM_CHAR
#ifndef DEBUG
  #undef  keyData
  #undef  wchCharCode
#endif
    } // End SWITCH

////LCLODSAPI ("LECZ: ", hWnd, message, wParam, lParam);
    return CallWindowProcW (lpfnOldKeybEditCtrlWndProc,
                            hWnd,
                            message,
                            wParam,
                            lParam);    // Pass on down the line
} // End LclKeybEditCtrlWndProc


//***************************************************************************
//  $LclFontsStaticTextWndProc
//
//  Local ChooseFont Sample Text subclass procedure
//***************************************************************************

LRESULT WINAPI LclFontsStaticTextWndProc
    (HWND   hWnd,       // Window handle
     UINT   message,    // Type of message
     WPARAM wParam,     // Additional information
     LPARAM lParam)     // ...

{
    MSG   msg;          // Used to hold message info for TTM_RELAYEVENT
    DWORD dwMsgPos;     // Cursor position of the last message

////LCLODSAPI ("LFST: ", hWnd, message, wParam, lParam);

    // Split cases based upon the message
    switch (message)
    {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MOUSEMOVE:
            // Get the cursor position of the last message
            dwMsgPos = GetMessagePos ();

            // Fill in the MSG struc
            msg.hwnd    = hWnd;
            msg.message = message;
            msg.wParam  = wParam;
            msg.lParam  = lParam;
            msg.time    = GetMessageTime ();
            msg.pt.x    = GET_X_LPARAM (dwMsgPos);
            msg.pt.y    = GET_Y_LPARAM (dwMsgPos);

            // Relay this message to the Tooltip control
            SendMessageW (hWndTT, TTM_RELAYEVENT, 0, (LPARAM) &msg);

            break;

        default:
            break;
    } // End SWITCH

////LCLODSAPI ("LFSTZ: ", hWnd, message, wParam, lParam);
    return
      CallWindowProcW (lpfnOldFontsStaticTextWndProc,
                       hWnd,
                       message,
                       wParam,
                       lParam);     // Pass on down the line
} // End LclFontsStaticTextWndProc


//***************************************************************************
//  $DrawButton
//
//  Draw a button in the Customize DlgProc
//***************************************************************************

void DrawButton
    (HWND             hWndListBox,          // Main ListBox window handle
     HWND             hWndProp,             // Property Page window handle
     LPDRAWITEMSTRUCT lpdis,                // Ptr to DRAWITEMSTRUCT
     UINT             idCtl)                // IDC_xxx of the char to draw

{
    RECT   rcItem;                          // Rectangle to draw within
    HBRUSH hBrush;                          // background brush to use
    WCHAR  wszText[2] = {L'\0'};            // Char to draw

    // Get the current background color of the ListBox
    hBrush = (HBRUSH) GetClassLongPtr (hWndListBox, GCLP_HBRBACKGROUND);

    // Get the char to draw
    SendMessageW (GetDlgItem (hWndProp, idCtl), WM_GETTEXT, KEYB_UNICODE_LIMIT + 1, (LPARAM) wszText);

    // If it's empty, ...
    if (wszText[0] EQ L'\0')
        // Draw a blank
        wszText[0] = L' ';

    // Copy the drawing rectangle
    rcItem = lpdis->rcItem;

    // Split cases based upon the item action
    switch (lpdis->itemAction)
    {
        case ODA_DRAWENTIRE:
            // Fill the client area background
            FillRect (lpdis->hDC, &rcItem, hBrush);

            // Set the text background mode
            SetBkMode (lpdis->hDC, TRANSPARENT);

            // Draw the text (one char)
            DrawTextW (lpdis->hDC,
                       wszText,
                       1,
                      &rcItem,
                       DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
            break;

        case ODA_FOCUS:     // Ignore changes in focus
            break;

        case ODA_SELECT:
            // Fill the client area background
            FillRect (lpdis->hDC, &rcItem, hBrush);

            // If the button is selected, ...
            if (lpdis->itemState & ODS_SELECTED)
            {
                // Offset it by one pixel
                rcItem.left  ++;
                rcItem.top   ++;
                rcItem.right ++;
                rcItem.bottom++;
            } // End IF

            // Set the text background mode
            SetBkMode (lpdis->hDC, TRANSPARENT);

            // Draw the text (one char)
            DrawTextW (lpdis->hDC,
                       wszText,
                       1,
                      &rcItem,
                       DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
            // Draw the edge as Sunken (if selected) or Raised (if normal)
            DrawEdge (lpdis->hDC,
                     &rcItem,
                      (lpdis->itemState & ODS_SELECTED) ? EDGE_SUNKEN : EDGE_RAISED,
                      BF_ADJUST | BF_RECT);
            break;
    } // End SWITCH
} // End DrawButton


//***************************************************************************
//  $DisplayKeybTCValue
//
//  Display a value from the keyboard tabCtrl as a Unicode value
//***************************************************************************

void DisplayKeybTCValue
    (HWND  hWndProp,                // Property Window handle
     UINT  idCtl,                   // Control ID (= -1 if no change)
     UBOOL bCC)                     // TRUE iff Curent Char (CC) instead of Line Continuation (LC)

{
    WCHAR   wszText[2] = {L'\0'},
            wszStr[KEYB_UNICODE_LIMIT + 1];
    LPWCHAR lpwszStr;               // Temporary string ptr
    UINT    uValue,                 // Temporary value
            uCnt,                   // Loop counter
            uIDC_XXXX_BN_UNICODE = bCC ? IDC_KEYB_BN_UNICODE : IDC_USER_PREFS_BN_UNICODE,
            uKeybUnibaseXX      = bCC ? uKeybUnibase       : uUserUnibase            ,
            uIDC_XXXX_EC_UNICODE = bCC ? IDC_KEYB_EC_UNICODE : IDC_USER_PREFS_EC_UNICODE;

    // If the value has changed, ...
    if (idCtl NE -1)
    {
        // Get the value to convert
        SendMessageW (GetDlgItem (hWndProp, idCtl), WM_GETTEXT, KEYB_UNICODE_LIMIT + 1, (LPARAM) wszText);

        // Display the character on the Unicode button
        SendMessageW (GetDlgItem (hWndProp, uIDC_XXXX_BN_UNICODE), WM_SETTEXT, 0, (LPARAM) wszText);
    } else
        // Get the value to convert
        SendMessageW (GetDlgItem (hWndProp, uIDC_XXXX_BN_UNICODE), WM_GETTEXT, KEYB_UNICODE_LIMIT + 1, (LPARAM) wszText);

    // Convert the code to a string in the Unicode base
    lpwszStr = &wszStr[KEYB_UNICODE_LIMIT];
    *lpwszStr = WC_EOS;
    uValue = wszText[0];
    uCnt = 0;       // Initialize the # digits
    while (uValue)
    {
        // Format the digit
        *--lpwszStr = L"0123456789ABCDEF"[uValue % uKeybUnibaseXX];

        // Strip it off
        uValue /= uKeybUnibaseXX;

        // Count it in
        uCnt++;
    } // End WHILE

    // If this is hexadecimal, ...
    if (uKeybUnibaseXX EQ 16)
        // Pad with leading zeros
        while (uCnt++ < KEYB_UNICODE_LIMIT)
            *--lpwszStr = L'0';

    // Select the entire text in the Edit Ctrl
    SendMessageW (GetDlgItem (hWndProp, uIDC_XXXX_EC_UNICODE), EM_SETSEL, 0, -1);

    // Insert the string into the Edit Ctrl
    SendMessageW (GetDlgItem (hWndProp, uIDC_XXXX_EC_UNICODE), EM_REPLACESEL, FALSE, (LPARAM) lpwszStr);
} // End DisplayKeybTCValue


//***************************************************************************
//  $KeybHighlight
//
//  Add or remove the highlight from a Keyboard TabCtrl button
//***************************************************************************

void KeybHighlight
    (HWND hWndProp,                         // Keyboard Property Page window handle
     UINT idNewCtl)                         // ID of control to highlight (or NOCONTROL to remove)

{
    WCHAR wszText[2] = {L'\0'};
    UINT  uStyle;                           // Window Style
    HWND  hWndCtl;                          // Window handle to the control

    // If we're to highlight, ...
    if (idNewCtl NE NOCONTROL)
    {
        // Get the window handle
        hWndCtl = GetDlgItem (hWndProp, idNewCtl);

        // Get the current style
        uStyle = GetWindowLong (hWndCtl, GWL_STYLE);

        // Highlight this button
        SendMessageW (hWndCtl, BM_SETSTYLE, uStyle & ~BS_OWNERDRAW, TRUE);
    } else
    // Clear the highlight from the last button
    if (aKTC[uKeybTCNum].idLastHighlight NE NOCONTROL)
    {
        // Get the window handle
        hWndCtl = GetDlgItem (hWndProp, aKTC[uKeybTCNum].idLastHighlight);

        // Get the current style
        uStyle = GetWindowLong (hWndCtl, GWL_STYLE);

        // In case the value is an ampersand, ...
        SendMessageW (hWndCtl, WM_GETTEXT, KEYB_UNICODE_LIMIT + 1, (LPARAM) wszText);
        SendMessageW (hWndCtl, WM_SETTEXT, 0, (LPARAM) wszText);

        // Set back to the normal style
        SendMessageW (hWndCtl, BM_SETSTYLE, uStyle | BS_OWNERDRAW, TRUE);
    } // End IF/ELSE
} // End KeybHighlight


//***************************************************************************
//  $DispKeybLayout
//
//  Display a newly selected keyboard layout
//***************************************************************************

void DispKeybLayout
    (HWND hWndProp)                         // Keyboard Property Page window handle

{
    UINT uCnt,                              // Loop counter
         uCol;                              // ..

    // Invalidate the Keycap buttons
    for (uCnt = 0; uCnt < KKC_CY; uCnt++)
    for (uCol = aKKC_IDC_BEG[uCnt]; uCol <= aKKC_IDC_END[uCnt]; uCol++)
        InvalidateRect (GetDlgItem (hWndProp, uCol), NULL, TRUE);

    // Change the show/hide state of the button and text
    ShowWindow (GetDlgItem (hWndProp, IDC_KEYB_BN_MAKEACT), (uLclKeybLayoutNumAct EQ uLclKeybLayoutNumVis) ? SW_HIDE : SW_SHOW);
    ShowWindow (GetDlgItem (hWndProp, IDC_KEYB_LT_ISACT  ), (uLclKeybLayoutNumAct EQ uLclKeybLayoutNumVis) ? SW_SHOW : SW_HIDE);

    // Show/hide Keycap buttons
    ShowWindow (GetDlgItem (hWndProp, IDC_KEYB_BN_KC_1C),
                (lpLclKeybLayouts[uLclKeybLayoutNumVis].uScanCode2B_RowNum EQ 1) ? SW_SHOW : SW_HIDE);
    ShowWindow (GetDlgItem (hWndProp, IDC_KEYB_BN_KC_2B),
                (lpLclKeybLayouts[uLclKeybLayoutNumVis].uScanCode2B_RowNum EQ 2) ? SW_SHOW : SW_HIDE);
    ShowWindow (GetDlgItem (hWndProp, IDC_KEYB_BN_KC_30),
                 lpLclKeybLayouts[uLclKeybLayoutNumVis].bExtraKeyRow3            ? SW_SHOW : SW_HIDE);

    // Check/uncheck CXV, ZY, and SEQ boxes
    CheckDlgButton (hWndProp, IDC_KEYB_RB_CLIP0,  lpLclKeybLayouts[uLclKeybLayoutNumVis].bUseCXV);
    CheckDlgButton (hWndProp, IDC_KEYB_RB_CLIP1, !lpLclKeybLayouts[uLclKeybLayoutNumVis].bUseCXV);
    CheckDlgButton (hWndProp, IDC_KEYB_RB_UNDO0,  lpLclKeybLayouts[uLclKeybLayoutNumVis].bUseZY );
    CheckDlgButton (hWndProp, IDC_KEYB_RB_UNDO1, !lpLclKeybLayouts[uLclKeybLayoutNumVis].bUseZY );
    CheckDlgButton (hWndProp, IDC_KEYB_RB_FNED0,  lpLclKeybLayouts[uLclKeybLayoutNumVis].bUseSEQ);
    CheckDlgButton (hWndProp, IDC_KEYB_RB_FNED1, !lpLclKeybLayouts[uLclKeybLayoutNumVis].bUseSEQ);

    // Display or hide the keyboard Ctrl keycaps
    DispKeybCtrlKeycaps (hWndProp);

    // Enable/disable the Delete... button
    EnableWindow (GetDlgItem (hWndProp, IDC_KEYB_BN_DEL), !lpLclKeybLayouts[uLclKeybLayoutNumVis].bReadOnly);
} // End DispKeybLayout


//***************************************************************************
//  $DispKeybCtrlKeycaps
//
//  Display or hide the CXV, ZY, and SEQ keycaps
//***************************************************************************

void DispKeybCtrlKeycaps
    (HWND hWndProp)                 // Keyboard Property Page window handle

{
    InvalidateRect (GetDlgItem (hWndProp, KeybScanCodeToID (KeybCharToScanCode (L'c', KS_NONE))), NULL, TRUE);
    InvalidateRect (GetDlgItem (hWndProp, KeybScanCodeToID (KeybCharToScanCode (L'x', KS_NONE))), NULL, TRUE);
    InvalidateRect (GetDlgItem (hWndProp, KeybScanCodeToID (KeybCharToScanCode (L'v', KS_NONE))), NULL, TRUE);

    InvalidateRect (GetDlgItem (hWndProp, KeybScanCodeToID (KeybCharToScanCode (L'z', KS_NONE))), NULL, TRUE);
    InvalidateRect (GetDlgItem (hWndProp, KeybScanCodeToID (KeybCharToScanCode (L'y', KS_NONE))), NULL, TRUE);

    InvalidateRect (GetDlgItem (hWndProp, KeybScanCodeToID (KeybCharToScanCode (L's', KS_NONE))), NULL, TRUE);
    InvalidateRect (GetDlgItem (hWndProp, KeybScanCodeToID (KeybCharToScanCode (L'e', KS_NONE))), NULL, TRUE);
    InvalidateRect (GetDlgItem (hWndProp, KeybScanCodeToID (KeybCharToScanCode (L'q', KS_NONE))), NULL, TRUE);
} // End DispKeybCtrlKeycaps


//***************************************************************************
//  $KeybScanCodeToID
//
//  Find the ID of a given scancode
//***************************************************************************

UINT KeybScanCodeToID
    (UINT uScanCode)                // The scancode to find

{
    UINT uCnt,                      // Loop counter
         uCol;                      // ...

    // Loop through the rows
    for (uCnt = 0; uCnt < KKC_CY; uCnt++)
    for (uCol = 0; uCol < aKKC_SC[uCnt].uLen; uCol++)
    if (aKKC_SC[uCnt].aSC[uCol] EQ uScanCode)
        return aKKC_IDC_BEG[uCnt] + uCol;

    return 0;
} // End KeybScanCodeToID


//***************************************************************************
//  $KeybCharToScanCode
//
//  Find the scancode of a given char and state
//***************************************************************************

UINT KeybCharToScanCode
    (WCHAR wc,                      // The char to find
     UINT  uState)                  // The keyboard state (combination of KS_SHIFT, KS_CTRL, and KS_ALT)

{
    UINT uCnt,                      // Loop counter
         uLen;                      // # scancodes in this layout

    // Get the # scancodes in the current layout
    uLen = lpLclKeybLayouts[uLclKeybLayoutNumVis].uCharCodesLen;

    // Loop through the chars
    for (uCnt = 0; uCnt < uLen; uCnt++)
    if (lpLclKeybLayouts[uLclKeybLayoutNumVis].aCharCodes[uCnt].wc[uState] EQ wc)
        return uCnt;

    return 0;
} // End KeybCharToScanCode


//***************************************************************************
//  $SetKeybFont
//
//  Set the font for keyboard controls
//***************************************************************************

void SetKeybFont
    (HWND hWndProp)                     // Property Page window handle (amy be NULL)

{
    UINT uCnt,                          // Loop counter
         uCol;                          // ...

    // If the Property Page window handle has not been specified, ...
    if (hWndProp EQ NULL)
        // Get the associated item data (window handle of the Property Page)
        hWndProp = (HWND)
          SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_KEYBS - IDD_PROPPAGE_START, 0);

    // If there is an existing font, delete it
    if (hFontKB NE NULL)
    {
        MyDeleteObject (hFontKB) ; hFontKB = NULL;
    } // End IF

    // Create the font for the keyboard TabCtrl
    hFontKB = MyCreateFontIndirectW (&lfKB);

    // Set the font for the keycap pushbuttons and redraw them
    for (uCnt = 0; uCnt < KKC_CY; uCnt++)
    for (uCol = aKKC_IDC_BEG[uCnt]; uCol <= aKKC_IDC_END[uCnt]; uCol++)
        SendMessageW (GetDlgItem (hWndProp, uCol), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontKB, MAKELPARAM (TRUE, 0));

    // Set the font for the TabCtrl pushbuttons and redraw them
    for (uCnt = 0; uCnt < KTC_TXT_CY; uCnt++)
    for (uCol = 0; uCol < KTC_TXT_CX; uCol++)
        SendMessageW (GetDlgItem (hWndProp, aKTC_IDC_BEG[uCnt] + uCol), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontKB, MAKELPARAM (TRUE, 0));

    // Set the font for the Unicode button and redraw it
    SendMessageW (GetDlgItem (hWndProp, IDC_KEYB_BN_UNICODE), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontKB, MAKELPARAM (TRUE, 0));
} // End SetKeybFont


//***************************************************************************
//  $NewKeybDlgProc
//
//  Ask the user to type in the name for the keyboard layout
//***************************************************************************

UBOOL CALLBACK NewKeybDlgProc
    (HWND   hDlg,       // Window handle
     UINT   message,    // Type of message
     WPARAM wParam,     // Additional information
     LPARAM lParam)     // ...

{
    static NEWKEYBDLG newKeybDlg;
    static WCHAR      wszTmpKeybLayoutName[KBLEN];
           LPWCHAR    lpwTemp;
           UINT       uLen;

    // Split cases
    switch (message)
    {
        case WM_INITDIALOG:     // hWndFocus = (HWND) wParam
                                // lpNewKeybDlg = (LPNEWKEYBDLG) lParam
            // Save the incoming data
            newKeybDlg = *(LPNEWKEYBDLG) lParam;

            // Copy the name for later use
            MyStrcpyW (wszTmpKeybLayoutName, sizeof (wszTmpKeybLayoutName), newKeybDlg.lpwKeybLayoutName);

            // Limit the text in the Edit Ctrl
            SendMessageW (GetDlgItem (hDlg, IDC_NEWKEYB_EC), EM_SETLIMITTEXT, countof (wszTmpKeybLayoutName), 0);

            // Seed the Edit Ctrl with this name
            SetWindowTextW (GetDlgItem (hDlg, IDC_NEWKEYB_EC), wszTmpKeybLayoutName);

            // Return the focus to the Edit Ctrl
            SetFocus (GetDlgItem (hDlg, IDC_NEWKEYB_EC));

            return DLG_MSGNODEFFOCUS;               // We set the focus

        case WM_CLOSE:
            EndDialog (hDlg, FALSE);                // Fail this dialog

#define idCtl               GET_WM_COMMAND_ID   (wParam, lParam)
#define cmdCtl              GET_WM_COMMAND_CMD  (wParam, lParam)
#define hwndCtl             GET_WM_COMMAND_HWND (wParam, lParam)
        case WM_COMMAND:
            // If the user pressed one of our buttons, ...
            switch (idCtl)
            {
                case IDOK:
                    // Get the text from the Edit Ctrl
                    GetWindowTextW (GetDlgItem (hDlg, IDC_NEWKEYB_EC), wszTmpKeybLayoutName, countof (wszTmpKeybLayoutName));

                    // Copy to a writable ptr
                    lpwTemp = &wszTmpKeybLayoutName[0];

                    // Strip off leading blanks
                    while (isspaceW (*lpwTemp))
                        lpwTemp++;
                    // Get the string length
                    uLen = lstrlenW (lpwTemp);

                    // Strip off trailing blanks
                    while (uLen && isspaceW (lpwTemp[uLen - 1]))
                        uLen--;
                    // Zap it
                    lpwTemp[uLen] = WC_EOS;

                    // If there's anything remaining, ...
                    if (uLen)
                    {
                        // Lookup this string in the caller's ComboBox
                        uLen = (UINT)
                          SendMessageW (GetDlgItem (newKeybDlg.hWndProp, IDC_KEYB_CB_LAYOUT), CB_FINDSTRING, -1, (LPARAM) lpwTemp);

                        // If not found, ...
                        if (uLen EQ CB_ERR)
                        {
                            // Copy the string to the caller's buffer
                            strcpyW (newKeybDlg.lpwKeybLayoutName, lpwTemp);

                            EndDialog (hDlg, TRUE);         // Quit this dialog
                        } else
                            MessageBoxW (hDlg, L"Duplicate Keyboard Layout Name -- Please try again.", WS_APPNAME, MB_OK | MB_ICONWARNING);
                    } else
                        MessageBoxW (hDlg, L"Empty Keyboard Layout Name -- Please try again.", WS_APPNAME, MB_OK | MB_ICONWARNING);

                    // Return the focus to the Edit Ctrl
                    SetFocus (GetDlgItem (hDlg, IDC_NEWKEYB_EC));

                    // Return dialog result
                    DlgMsgDone (hDlg);              // We handled the msg

                case IDCANCEL:
                    EndDialog (hDlg, FALSE);        // Fail this dialog
            } // End SWITCH

            break;
#undef  hwndCtl
#undef  cmdCtl
#undef  idCtl
    } // End SWITCH

    // Return dialog result
    DlgMsgPass (hDlg);              // We didn't handled the msg
} // End NewKeybDlgProc


//***************************************************************************
//  $EnumCallbackResizeMC
//
//  EnumChildWindows callback to resize all MC windows
//***************************************************************************

UBOOL CALLBACK EnumCallbackResizeMC
    (HWND   hWnd,           // Handle to child window
     LPARAM lParam)         // Application-defined value

{
    // If it's a MDI Client window, ...
    if (IzitMC (hWnd))
    {
#ifdef DEBUG
        LPRECT lprc = (LPRECT) lParam;      // Window rectangle for MDI Client in MF client area coords
#else
  #define lprc    ((LPRECT) lParam)         // Window rectangle for MDI Client in MF client area coords
#endif
        // Resize the MDI Client window to match the new MF size
        SetWindowPos (hWnd,                         // Window handle to position
                      NULL,                         // Placement order handle (ignored due to SWP_NOZORDER)
                      lprc->left,                   // X-position
                      lprc->top,                    // Y-...
                      lprc->right  - lprc->left,    // Width
                      lprc->bottom - lprc->top,     // Height
                      0                             // Flags
                    | SWP_NOACTIVATE
                    | SWP_NOZORDER
                    | SWP_SHOWWINDOW
                     );
#ifndef DEBUG
  #undef  lprc
#endif
    } // End IF

    return TRUE;        // Keep on truckin'
} // End EnumCallbackResizeMC


//***************************************************************************
//  $EnumCallbackRepaint
//
//  EnumChildWindows callback to repaint all SM and FE windows
//***************************************************************************

UBOOL CALLBACK EnumCallbackRepaint
    (HWND   hWnd,           // Handle to child window
     LPARAM lParam)         // Application-defined value

{
    // When an MDI child window is minimized, Windows creates two windows: an
    // icon and the icon title.  The parent of the icon title window is set to
    // the MDI client window, which confines the icon title to the MDI client
    // area.  The owner of the icon title is set to the MDI child window.
    if (GetWindow (hWnd, GW_OWNER))     // If it's an icon title window, ...
        return TRUE;                    // skip it, and continue enumerating

    // If it's a Session Manager or Function Editor window, ...
    if (IzitSM (hWnd)
     || IzitFE (hWnd))
    {
        HWND hWndEC;

        // Get the handle to the Edit Ctrl
        hWndEC = (HWND) GetWindowLongPtrW (hWnd, GWLSF_HWNDEC);

#define bWinBGDiff  ((UBOOL) lParam)

        // If the Window background color changed, ...
        if (bWinBGDiff)
            SetClassLongPtrW (hWndEC, GCLP_HBRBACKGROUND, (HANDLE_PTR) ghBrushBG);

#undef  bWinBGDiff

        // Invalidate the Client Area
        InvalidateRect (hWndEC, NULL, FALSE);

        // Update it
        UpdateWindow (hWndEC);
    } // End IF

    return TRUE;        // Keep on truckin'
} // End EnumCallbackRepaint


//***************************************************************************
//  $EnumCallbackFallbackFont
//
//  EnumChildWindows callback to respecify the Fallback font for all EC windows
//***************************************************************************

UBOOL CALLBACK EnumCallbackFallbackFont
    (HWND   hWnd,           // Handle to child window
     LPARAM lParam)         // Application-defined value

{
    UBOOL bSM, bFE;

    // When an MDI child window is minimized, Windows creates two windows: an
    // icon and the icon title.  The parent of the icon title window is set to
    // the MDI client window, which confines the icon title to the MDI client
    // area.  The owner of the icon title is set to the MDI child window.
    if (GetWindow (hWnd, GW_OWNER))     // If it's an icon title window, ...
        return TRUE;                    // skip it, and continue enumerating

    // Determine the type of parent
    bSM = IzitSM (hWnd);
    bFE = IzitFE (hWnd);

    // If it's a Session Manager or Function Editor window, ...
    if (bSM || bFE)
    {
        HWND hWndEC;
        HFONT hFontFB_PR,
              hFontFB_SF;

        // Get the handle to the Edit Ctrl
        hWndEC = (HWND) GetWindowLongPtrW (hWnd, GWLSF_HWNDEC);

        // If it's SM, ...
        if (bSM)
        {
            hFontFB_PR = hFontFB_PR_SM;
            hFontFB_SF = hFontFB_SM;
        } else
        {
            hFontFB_PR = hFontFB_PR_FE;
            hFontFB_SF = hFontFB_FE;
        } // End IF/ELSE

        // Tell 'em about the Fallback font
        SendMessageW (hWndEC,
                      EM_SETFALLBACKFONT,
                      (WPARAM) (OptionFlags.bOutputDebug ? hFontFB_PR : NULL),
                      (LPARAM) (OptionFlags.bOutputDebug ? hFontFB_SF : NULL));
        // Invalidate the Client Area
        InvalidateRect (hWndEC, NULL, FALSE);

        // Update it
        UpdateWindow (hWndEC);
    } // End IF

    return TRUE;        // Keep on truckin'
} // End EnumCallbackFallbackFont


//***************************************************************************
//  $FillSyntaxColor
//
//  Fill in a button with a Syntax Color
//***************************************************************************

void FillSyntaxColor
    (LPDRAWITEMSTRUCT lpdis,
     COLORREF         clrRef)

{
    RECT   rcItem;
    UINT   uEdge;
    HBRUSH hBrush;

    // Copy the item rectangle to fill in the Client Area
    rcItem = lpdis->rcItem;

    // Flood the Client Area with the default background
    hBrush = MyCreateSolidBrush (GetBkColor (lpdis->hDC));

    // Fill the client area background
    FillRect (lpdis->hDC, &rcItem, hBrush);

    // We no longer need this resource
    MyDeleteObject (hBrush); hBrush = NULL;

    // Get the edge type
    uEdge = (lpdis->itemState & ODS_SELECTED) ? EDGE_SUNKEN : EDGE_RAISED;

    // If the button is selected, offset it by one pixel
    if (lpdis->itemState & ODS_SELECTED)
    {
        rcItem.left  ++;
        rcItem.top   ++;
        rcItem.right ++;
        rcItem.bottom++;
    } // End IF

    // Draw the edge as Sunken (if selected) or Raised (if normal)
    DrawEdge (lpdis->hDC,
             &rcItem,
              uEdge,
              BF_ADJUST | BF_RECT);
    // Indent the rectangle to narrow the Client Area
    rcItem.top    += 2;
    rcItem.right  -= 2;
    rcItem.bottom -= 2;
    rcItem.left   += 2;

    // Get the corresponding Foreground/Background Color
    hBrush = MyCreateSolidBrush (clrRef);

    // Color the client area
    FillRect (lpdis->hDC, &rcItem, hBrush);

    // We no longer need this resource
    MyDeleteObject (hBrush); hBrush = NULL;
} // End FillSyntaxColor


//***************************************************************************
//  $GetClearWsChrValue
//
//  Read in and save a CLEAR WS value
//***************************************************************************

void GetClearWsChrValue
    (HWND     hWndProp,                 // Dialog property page window handle
     UINT     uID,                      // Edit Ctrl ID
     HGLOBAL *lphGlbVal)                // Ptr to global memory handle

{
    UINT      uLen;                     // Temporary length
    APLNELM   aplNELMVal;               // NELM of object
    LPAPLCHAR lpMemChr;                 // Ptr to global memory

    // Tell the Edit Ctrl how big our buffer is
    lpwszGlbTemp[0] = 1024;

    // Get the text
    SendMessageW (GetDlgItem (hWndProp, uID), EM_GETLINE, 0, (LPARAM) lpwszGlbTemp);

    // Get the text length
    uLen = lstrlenW (lpwszGlbTemp);

    // Get the existing array attributes
    AttrsOfGlb (*lphGlbVal, NULL, &aplNELMVal, NULL, NULL);

    // If the lengths are the same and the reference count is 1, ...
    if (aplNELMVal EQ uLen
     && GetRefCntGlb (*lphGlbVal) EQ 1)
    {
        // Lock the memory to get a ptr to it
        lpMemChr = MyGlobalLockVar (*lphGlbVal);

        // Skip over the header and dimensions to the data
        lpMemChr = VarArrayDataFmBase (lpMemChr);

        // Copy the values
        CopyMemoryW (lpMemChr, lpwszGlbTemp, uLen);

        // We no longer need this ptr
        MyGlobalUnlock (*lphGlbVal); lpMemChr = NULL;
    } else
        GetClearWsComValue (ARRAY_CHAR, uLen, lphGlbVal, lpwszGlbTemp, sizeof (APLCHAR));
} // End GetClearWsChrValue


//***************************************************************************
//  $GetClearWsComValue
//
//  Common routine to get a CLEARWS value
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- GetClearWsComValue"
#else
#define APPEND_NAME
#endif

void GetClearWsComValue
    (APLSTYPE aplTypeCom,               // Common storage type
     UINT     uLen,                     // Common length
     HGLOBAL *lphGlbRes,                // Ptr to incoming/outgoing global memory handle
     LPVOID   lpwszGlbTemp,             // Ptr to common values
     UINT     uSizeOf)                  // Size of each of the common values

{
    HGLOBAL hGlbCom;                    // Common global memory handle
    LPVOID  lpMemCom;                   // Ptr to common global memory
    APLUINT ByteRes;                    // # bytes in the result

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeCom, uLen, 1);

    // Allocate space for the data
    // Note, we can't use DbgGlobalAlloc here as the
    //   PTD has not been allocated as yet
    hGlbCom = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbCom NE NULL)
    {
        // Free the current value
        FreeResultGlobalVar (*lphGlbRes); *lphGlbRes = NULL;

        // Save the new global memory handle
        *lphGlbRes = hGlbCom;

        // Lock the memory to get a ptr to it
        lpMemCom = MyGlobalLock000 (hGlbCom);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemCom)
        // Fill in the header values
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = aplTypeCom;
////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////lpHeader->SysVar     = FALSE;       // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = uLen;
        lpHeader->Rank       = 1;
#undef  lpHeader
        // Save the dimension
        *VarArrayBaseToDim (lpMemCom) = uLen;

        // Skip over the header and dimensions to the data
        lpMemCom = VarArrayDataFmBase (lpMemCom);

        // Copy the values to the result
        CopyMemory (lpMemCom, lpwszGlbTemp, uLen * uSizeOf);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbCom); lpMemCom = NULL;
    } else
    {
        // ***FIXME***

#ifdef DEBUG
        DbgStop ();         // #ifdef DEBUG
#endif
    } // End IF/ELSE
} // End GetClearWsComValue
#undef  APPEND_NAME


//***************************************************************************
//  $InitThemes
//
//  Initialize for themes
//***************************************************************************

void InitThemes
    (void)

{
    // Attempt to load the library
    hModThemes = LoadLibrary ("UXTHEME.DLL");

    // If it succeeded, ...
    if (hModThemes NE NULL)
    {
        // Get the address of various routines we need
        zOpenThemeData                 = (PFNOPENTHEMEDATA)                 GetProcAddress (hModThemes, "OpenThemeData");
        zDrawThemeBackground           = (PFNDRAWTHEMEBACKGROUND)           GetProcAddress (hModThemes, "DrawThemeBackground");
        zCloseThemeData                = (PFNCLOSETHEMEDATA)                GetProcAddress (hModThemes, "CloseThemeData");
////    zDrawThemeText                 = (PFNDRAWTHEMETEXT)                 GetProcAddress (hModThemes, "DrawThemeText");
////    zGetThemeBackgroundContentRect = (PFNGETTHEMEBACKGROUNDCONTENTRECT) GetProcAddress (hModThemes, "GetThemeBackgroundContentRect");

        //If they all loaded, ...
        if (zOpenThemeData
         && zDrawThemeBackground
         && zCloseThemeData
////     && zDrawThemeText
////     && zGetThemeBackgroundContentRect
           )
            // Set the global flag
            bThemeLibLoaded = TRUE;
        else
        {
            // Free the module, zap the handle
            FreeLibrary (hModThemes); hModThemes = NULL;
        } // End IF/ELSE
    } // End IF
} // End InitThemes


//***************************************************************************
//  $FinThemes
//
//  Close out themes
//***************************************************************************

void FinThemes
    (void)

{
    // Free the library, zap the handle
    FreeLibrary (hModThemes); hModThemes = NULL;

    // Clear the flags
    bThemeLibLoaded =
    bThemedKeybs    = FALSE;
} // End FinThemes


//***************************************************************************
//  $GetFontWeightW
//
//  Return a ptr to a string describing the given font weight
//***************************************************************************

LPWSTR GetFontWeightW
    (LONG lfWeight)

{
    // Split cases based upon the font weight
    switch (lfWeight)
    {
        case FW_DONTCARE:
            return L" Don't Care";

        case FW_THIN:
            return L" Thin";

        case FW_EXTRALIGHT:
            return L" Extra Light";

////////case FW_ULTRALIGHT:             // Duplicate of FW_EXTRALIGHT
////////    return L" Ultra Light";

        case FW_LIGHT:
            return L" Light";

        case FW_NORMAL:
            return L" Normal";

////////case FW_REGULAR:                // Duplicate of FW_NORMAL
////////    return L" Regular";

        case FW_MEDIUM:
            return L" Medium";

        case FW_SEMIBOLD:
            return L" Semi Bold";

////////case FW_DEMIBOLD:               // Duplicate of FW_SEMIBOLD
////////    return L" Demi Bold";

        case FW_BOLD:
            return L" Bold";

        case FW_EXTRABOLD:
            return L" Extra Bold";

////////case FW_ULTRABOLD:              // Duplicate of FW_EXTRABOLD
////////    return L" Ultra Bold";

        case FW_HEAVY:
            return L" Heavy";

////////case FW_BLACK:                  // Duplicate of FW_HEAVY
////////    return L" Black";

        default:
            return L"";
    } // End SWITCH
} // End GetFontWeightW


//***************************************************************************
//  $GetLogPixelsX
//
//  Return the current iLogPixelsX
//***************************************************************************

int GetLogPixelsX
    (HDC hDC)                           // Device Context to use (may be NULL)

{
    int iLogPixelsX;                    // # horizontal pixels per inch in the DC

    if (hDC NE NULL)
        // Get the # pixels per horizontal inch
        iLogPixelsX = GetDeviceCaps (hDC, LOGPIXELSX);
    else
    {
        // Get a new device context
        hDC = MyGetDC (HWND_DESKTOP);

        // Get the # pixels per horizontal inch
        iLogPixelsX = GetDeviceCaps (hDC, LOGPIXELSX);

        // Release the one we just created
        MyReleaseDC (HWND_DESKTOP, hDC); hDC = NULL;
    } // End IF/ELSE

    return iLogPixelsX;
} // End GetLogPixelsX


//***************************************************************************
//  $GetLogPixelsY
//
//  Return the current iLogPixelsY
//***************************************************************************

int GetLogPixelsY
    (HDC hDC)                           // Device Context to use (may be NULL)

{
    int iLogPixelsY;                    // # vertical pixels per inch in the DC

    if (hDC NE NULL)
        // Get the # pixels per vertical inch
        iLogPixelsY = GetDeviceCaps (hDC, LOGPIXELSY);
    else
    {
        // Get a new device context
        hDC = MyGetDC (HWND_DESKTOP);

        // Get the # pixels per vertical inch
        iLogPixelsY = GetDeviceCaps (hDC, LOGPIXELSY);

        // Release the one we just created
        MyReleaseDC (HWND_DESKTOP, hDC); hDC = NULL;
    } // End IF/ELSE

    return iLogPixelsY;
} // End GetLogPixelsY


//***************************************************************************
//  $DirsBrowseCallbackProc
//
//  Callback proc for Dirs browse function
//***************************************************************************

int CALLBACK DirsBrowseCallbackProc
    (HWND   hWnd,           // Window handle
     UINT   uMsg,           // Type of message (BFFM_xxx)
     LPARAM lParam,         // Additional information (based upon uMsg)
     LPARAM lpData)         // ...                    (from BROWSEINFOW.lParam)

{
    RECT  rc;
    WCHAR wszTemp[_MAX_PATH + 256];
    HWND  hWndProp = NULL;
////static IUnknown    *iUnknown;
////static LPITEMIDLIST pidl;
////static IMalloc     *iMalloc;
////       WCHAR        wszBrowsePath[_MAX_PATH];

    // Split cases based upon the message
    switch (uMsg)
    {
        case BFFM_INITIALIZED:      // lParam = NULL
            // Get the screen coords of the browse dialog
            GetWindowRect (hWnd, &rc);

            // Reposition it to the top of the screen
            //   because it often displays with the bottom
            //   of the dialog below the bottom of the screen.
            SetWindowPos (hWnd,                         // Window handle to position
                          NULL,                         // Placement order handle (ignored due to SWP_NOZORDER)
                          rc.left,                      // X-position
                          0,                            // Y-...
                          0,                            // Width (ignored due to SWP_NOSIZE)
                          0,                            // Height ...
                          0                             // Flags
                        | SWP_NOZORDER
                        | SWP_NOSIZE
                         );
            break;

        case BFFM_IUNKNOWN:         // lParam = ptr to an IUnknown interface
////////////iUnknown = (IUnknown *) lParam;
////////////iUnknown->lpVtbl->QueryInterface ();
////////////iUnknown->lpVtbl->AddRef         ();
////////////iUnknown->lpVtbl->Release        ();

            break;

        case BFFM_SELCHANGED:       // lParam = PIDL identifying the newly selected item
////////////pidl = (LPITEMIDLIST) lParam;
////////////
////////////// Get the name of the folder
////////////SHGetPathFromIDListW (pidl, wszBrowsePath);
////////////
////////////// Free memory used
////////////if (SUCCEEDED (SHGetMalloc (&iMalloc)))
////////////{
////////////    IMalloc_Free (iMalloc, pidl); pidl = NULL;
////////////    IMalloc_Release (iMalloc); iMalloc = NULL;
////////////} // End IF

            break;

        case BFFM_VALIDATEFAILEDW:  // lParam = ptr to string containing the invalid name
            // Construct the error message
            MySprintfW (wszTemp,
                        sizeof (wszTemp),
                       L"The directory \"%s\" is not present.  Use \"Make New Folder\" to create it.",
                        lParam);
            MessageBoxW (hWndProp, wszTemp, lpwszAppName, MB_OK | MB_ICONERROR);

            return TRUE;            // Keep the dialog up

        case BFFM_VALIDATEFAILEDA:  // lParam = ptr to string containing the invalid name
        defstop
            break;
    } // End SWITCH

    return FALSE;
} // End DirsBrowseCallbackProc


//***************************************************************************
//  End of File: customize.c
//***************************************************************************
