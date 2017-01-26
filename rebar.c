//***************************************************************************
//  NARS2000 -- Rebar Control Routines
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
#include <commctrl.h>
#include "headers.h"

#ifndef USER_DEFAULT_SCREEN_DPI
  #define USER_DEFAULT_SCREEN_DPI 96
#endif

// Save area for old WS/ED/OW Toolbar window procedure address
WNDPROC lpfnOldWS_ED_OWToolbarWndProc;

// These window titles are for debugging purpose only
#ifdef DEBUG
WCHAR wszRBTitle[]         = WS_APPNAME L" Rebar Ctrl Window"                                WS_APPEND_DEBUG,
      wszTB_WSTitle[]      = WS_APPNAME L" Toolbar in Workspace Window in Rebar Ctrl Window" WS_APPEND_DEBUG,
      wszTB_EDTitle[]      = WS_APPNAME L" Toolbar in Edit Window in Rebar Ctrl Window"      WS_APPEND_DEBUG,
      wszTB_OWTitle[]      = WS_APPNAME L" Toolbar in Objects Window in Rebar Ctrl Window"   WS_APPEND_DEBUG,
      wszFW_RBTitle[]      = WS_APPNAME L" Font Window in Rebar Ctrl Window"                 WS_APPEND_DEBUG,
      wszLW_RBTitle[]      = WS_APPNAME L" Language Window in Rebar Ctrl Window"             WS_APPEND_DEBUG;
#else
  #define wszRBTitle        NULL
  #define wszTB_WSTitle     NULL
  #define wszTB_EDTitle     NULL
  #define wszTB_OWTitle     NULL
  #define wszFW_RBTitle     NULL
  #define wszLW_RBTitle     NULL
#endif

char pszNoCreateRBWnd[]    = "Unable to create Rebar Ctrl",
     pszNoCreateWS_RBWnd[] = "Unable to create Workspace Window in Rebar Ctrl",
     pszNoCreateED_RBWnd[] = "Unable to create Edit Window in Rebar Ctrl",
     pszNoCreateOW_RBWnd[] = "Unable to create Objects Window in Rebar Ctrl",
     pszNoCreateFW_RBWnd[] = "Unable to create Font Window in Rebar Ctrl",
     pszNoCreateLW_RBWnd[] = "Unable to create Language Window in Rebar Ctrl";

HBITMAP hBitmapWS,                  // Handle to WS bitmap strip
        hBitmapED,                  // ...       ED ...
        hBitmapOW;                  // ...       OW ...

#define IMAGE_FW_CY     30

#define  xComboFN       5
#define cxComboFN       160
#define cyComboFN       IMAGE_FW_CY
#define spComboFN       5

#define  xComboFS       xComboFN  + cxComboFN + spComboFN
#define cxComboFS       100
#define cyComboFS       IMAGE_FW_CY
#define spComboFS       5

#define cyEditUpDn      IMAGE_FW_CY - 6

#define  xEdit          xComboFS  + cxComboFS + spComboFS
#define cxEdit          50
#define cyEdit          cyEditUpDn
#define spEdit          0

#define  xUpDown        xEdit     + cxEdit    + spEdit
#define cxUpDown        10
#define cyUpDown        cyEditUpDn
#define spUpDown        0

#define cxFW_RB         xUpDown + cxUpDown

#define MAX_PTSIZE      72
#define MIN_PTSIZE       8

#define MAXTIPWIDTHINCHARS  70


//***************************************************************
// Workspace Toolbar
//***************************************************************

#define BUTTONSTYLE0    BTNS_BUTTON
#define BUTTONSTYLE1    BTNS_BUTTON
#define BUTTONSTYLE2    BTNS_DROPDOWN
#define BUTTONSTYLE3    BTNS_CHECK

enum tagIMAGEINDEX_WS
{
    IMAGEINDEX_WS_FILENEW = 0 ,     // 00:  Image index for FILENEW
    IMAGEINDEX_WS_FILEOPEN    ,     // 01:  ...             FILEOPEN
    IMAGEINDEX_WS_FILEXOPEN   ,     // 02:  ...             FILEXOPEN
    IMAGEINDEX_WS_FILECOPY    ,     // 03:  ...             FILECOPY
    IMAGEINDEX_WS_FILESAVE    ,     // 04:  ...             FILESAVE
    IMAGEINDEX_WS_FILESAVE_AS ,     // 05:  ...             FILESAVEAS
    IMAGEINDEX_WS_FILEPRINT   ,     // 06:  ...             FILEPRINT
    IMAGEINDEX_WS_FILEDROP    ,     // 07:  ...             FILEDROP
    IMAGEINDEX_WS_LENGTH            // 08:  Length
};

enum tagIMAGEINDEX_WS0
{
    IMAGEINDEX_WS_ARROW1 = 0  ,     // 00:  ...             Drop Down Arrow #1
    IMAGEINDEX_WS_ARROW2      ,     // 01:  ...             Drop Down Arrow #2
    IMAGEINDEX_WS_ARROW3      ,     // 02:  ...             Drop Down Arrow #3
    IMAGEINDEX_WS_LENGTH0           // 03:  Length
};

#define IDI_IMAGELIST_WS0       0
#define IDI_IMAGELIST_WS        1

// Initialize Workspace Window button info
TBBUTTON tbButtonsWS[IMAGEINDEX_WS_LENGTH + IMAGEINDEX_WS_LENGTH0] =
{
  {MAKELONG (IMAGEINDEX_WS_FILENEW    , IDI_IMAGELIST_WS ), IDM_NEW_WS    , TBSTATE_ENABLED, BUTTONSTYLE1, {0}, 0, (INT_PTR) L"Create a new workspace"                       },
  {MAKELONG (IMAGEINDEX_WS_FILEOPEN   , IDI_IMAGELIST_WS ), IDM_LOAD_WS   , TBSTATE_ENABLED, BUTTONSTYLE2, {0}, 0, (INT_PTR) L"Open an existing workspace from disk"         },
  {MAKELONG (IMAGEINDEX_WS_ARROW1     , IDI_IMAGELIST_WS0), 0             , TBSTATE_HIDDEN , BUTTONSTYLE0, {0}, 0, (INT_PTR) L"Select from a list of recent worksapces"      },
  {MAKELONG (IMAGEINDEX_WS_FILEXOPEN  , IDI_IMAGELIST_WS ), IDM_XLOAD_WS  , TBSTATE_ENABLED, BUTTONSTYLE2, {0}, 0, (INT_PTR) L"XOpen an existing workspace from disk"        },
  {MAKELONG (IMAGEINDEX_WS_ARROW2     , IDI_IMAGELIST_WS0), 0             , TBSTATE_HIDDEN , BUTTONSTYLE0, {0}, 0, (INT_PTR) L"Select from a list of recent worksapces"      },
  {MAKELONG (IMAGEINDEX_WS_FILECOPY   , IDI_IMAGELIST_WS ), IDM_COPY_WS   , TBSTATE_ENABLED, BUTTONSTYLE2, {0}, 0, (INT_PTR) L"Copy from disk into the current workspace"    },
  {MAKELONG (IMAGEINDEX_WS_ARROW3     , IDI_IMAGELIST_WS0), 0             , TBSTATE_HIDDEN , BUTTONSTYLE0, {0}, 0, (INT_PTR) L"Select from a list of recent worksapces"      },
  {MAKELONG (IMAGEINDEX_WS_FILESAVE   , IDI_IMAGELIST_WS ), IDM_SAVE_WS   , TBSTATE_ENABLED, BUTTONSTYLE1, {0}, 0, (INT_PTR) L"Save the current workspace to disk"           },
  {MAKELONG (IMAGEINDEX_WS_FILESAVE_AS, IDI_IMAGELIST_WS ), IDM_SAVE_AS_WS, TBSTATE_ENABLED, BUTTONSTYLE1, {0}, 0, (INT_PTR) L"Save the current workspace to disk\n"
                                                                                                                             L"under a different name"                       },
  {MAKELONG (IMAGEINDEX_WS_FILEPRINT  , IDI_IMAGELIST_WS ), IDM_PRINT_WS  , TBSTATE_ENABLED, BUTTONSTYLE1, {0}, 0, (INT_PTR) L"Print a selection or the current page\n"
                                                                                                                             L"from the current workspace"                   },
  {MAKELONG (IMAGEINDEX_WS_FILEDROP   , IDI_IMAGELIST_WS ), IDM_DROP_WS   , TBSTATE_ENABLED, BUTTONSTYLE1, {0}, 0, (INT_PTR) L"Delete an existing workspace from disk"       },
};

#define NUMBUTTONS_WS       IMAGEINDEX_WS_LENGTH
#define NUMBUTTONS_WS0      IMAGEINDEX_WS_LENGTH0


//***************************************************************
// Edit Toolbar
//***************************************************************

enum tagIMAGEINDEX_ED
{
    IMAGEINDEX_ED_EDITCUT  = 0  ,   // 00:  Image index for EDITCUT
    IMAGEINDEX_ED_EDITCOPY      ,   // 01:  ...             EDITCOPY
    IMAGEINDEX_ED_EDITPASTE     ,   // 02:  ...             EDITPASTE
    IMAGEINDEX_ED_EDITDELETE    ,   // 03:  ...             EDITDELETE
    IMAGEINDEX_ED_EDITUNDO      ,   // 04:  ...             EDITUNDO
    IMAGEINDEX_ED_EDITREDO      ,   // 05:  ...             EDITREDO
    IMAGEINDEX_ED_EDITNEW       ,   // 06:  ...             EDITNEW
    IMAGEINDEX_ED_EDITCUSTOMIZE ,   // 07:  ...             EDITCUSTOMIZE
    IMAGEINDEX_ED_LENGTH            // 08:  Length
};

enum tagIMAGEINDEX_ED0
{
    IMAGEINDEX_ED_ARROW1 = 0    ,   // 00:  ...             Drop Down Arrow #1
    IMAGEINDEX_ED_ARROW2        ,   // 01:  ...             Drop Down Arrow #2
    IMAGEINDEX_ED_ARROW3        ,   // 02:  ...             Drop Down Arrow #3
    IMAGEINDEX_ED_LENGTH0           // 03:  Length
};

#define IDI_IMAGELIST_ED0       0
#define IDI_IMAGELIST_ED        1

// Initialize Edit Window button info
TBBUTTON tbButtonsED[IMAGEINDEX_ED_LENGTH + IMAGEINDEX_ED_LENGTH0] =
{
  {MAKELONG (IMAGEINDEX_ED_EDITCUT       , IDI_IMAGELIST_ED ), IDM_CUT       ,  TBSTATE_ENABLED, BUTTONSTYLE2, {0}, 0, (INT_PTR) L"Cut selected text from the current workspace"     },
  {MAKELONG (IMAGEINDEX_ED_ARROW1        , IDI_IMAGELIST_ED0), 0             ,  TBSTATE_HIDDEN , BUTTONSTYLE0, {0}, 0, (INT_PTR) L"Select how the text is to be Copied"              },
  {MAKELONG (IMAGEINDEX_ED_EDITCOPY      , IDI_IMAGELIST_ED ), IDM_COPY      ,  TBSTATE_ENABLED, BUTTONSTYLE2, {0}, 0, (INT_PTR) L"Copy selected text from the current workspace"    },
  {MAKELONG (IMAGEINDEX_ED_ARROW2        , IDI_IMAGELIST_ED0), 0             ,  TBSTATE_HIDDEN , BUTTONSTYLE0, {0}, 0, (INT_PTR) L"Select how the text is to be Copied"              },
  {MAKELONG (IMAGEINDEX_ED_EDITPASTE     , IDI_IMAGELIST_ED ), IDM_PASTE     ,  TBSTATE_ENABLED, BUTTONSTYLE2, {0}, 0, (INT_PTR) L"Paste text into the current workspace"            },
  {MAKELONG (IMAGEINDEX_ED_ARROW3        , IDI_IMAGELIST_ED0), 0             ,  TBSTATE_HIDDEN , BUTTONSTYLE0, {0}, 0, (INT_PTR) L"Select how the text is to Pasted"                 },
  {MAKELONG (IMAGEINDEX_ED_EDITDELETE    , IDI_IMAGELIST_ED ), IDM_DELETE    ,  TBSTATE_ENABLED, BUTTONSTYLE1, {0}, 0, (INT_PTR) L"Delete selected text from the current workspace"  },
  {MAKELONG (IMAGEINDEX_ED_EDITUNDO      , IDI_IMAGELIST_ED ), IDM_UNDO      ,  TBSTATE_ENABLED, BUTTONSTYLE1, {0}, 0, (INT_PTR) L"Undo the last text change"                        },
  {MAKELONG (IMAGEINDEX_ED_EDITREDO      , IDI_IMAGELIST_ED ), IDM_REDO      ,  0              , BUTTONSTYLE1, {0}, 0, (INT_PTR) L"Redo the next text change"                        },
  {MAKELONG (IMAGEINDEX_ED_EDITNEW       , IDI_IMAGELIST_ED ), IDM_NEW_FCN   ,  TBSTATE_ENABLED, BUTTONSTYLE1, {0}, 0, (INT_PTR) L"Create a new function"                            },
  {MAKELONG (IMAGEINDEX_ED_EDITCUSTOMIZE , IDI_IMAGELIST_ED ), IDM_CUSTOMIZE ,  TBSTATE_ENABLED, BUTTONSTYLE1, {0}, 0, (INT_PTR) L"Customize Features and Appearance"                },
};

#define NUMBUTTONS_ED       IMAGEINDEX_ED_LENGTH
#define NUMBUTTONS_ED0      IMAGEINDEX_ED_LENGTH0


//***************************************************************
// Objects Toolbar
//***************************************************************

enum tagIMAGEINDEX_OW
{
    IMAGEINDEX_OW_OBJCLOSE    = 0,  // 00:  Image index for OBJCLOSE
    IMAGEINDEX_OW_OBJSAVE        ,  // 01:  ...             OBJSAVE
    IMAGEINDEX_OW_OBJSAVE_AS     ,  // 02:  ...             OBJSAVE_AS
    IMAGEINDEX_OW_OBJSAVECLOSE   ,  // 03:  ...             OBJSAVECLOSE
    IMAGEINDEX_OW_OBJTOGGLE_LNS  ,  // 04:  ...             OBJTOGGLE_LNS
    IMAGEINDEX_OW_LENGTH            // 05:  Length
};

enum tagIMAGEINDEX_OW0
{
    IMAGEINDEX_OW_LENGTH0 = 0       // 00:  Length
};

#define IDI_IMAGELIST_OW0       0
#define IDI_IMAGELIST_OW        1

// Initialize Objects Window button info
TBBUTTON tbButtonsOW[IMAGEINDEX_OW_LENGTH + IMAGEINDEX_OW_LENGTH0] =
{
  {MAKELONG (IMAGEINDEX_OW_OBJCLOSE     , IDI_IMAGELIST_OW ), IDM_CLOSE_FCN      ,  TBSTATE_ENABLED, BUTTONSTYLE1, {0}, 0, (INT_PTR) L"Close the function"                             },
  {MAKELONG (IMAGEINDEX_OW_OBJSAVE      , IDI_IMAGELIST_OW ), IDM_SAVE_FCN       ,  TBSTATE_ENABLED, BUTTONSTYLE1, {0}, 0, (INT_PTR) L"Save the function into the current workspace"   },
  {MAKELONG (IMAGEINDEX_OW_OBJSAVE_AS   , IDI_IMAGELIST_OW ), IDM_SAVE_AS_FCN    ,  TBSTATE_ENABLED, BUTTONSTYLE1, {0}, 0, (INT_PTR) L"Save the function into the current workspace\n"
                                                                                                                                     L"under a different name"                         },
  {MAKELONG (IMAGEINDEX_OW_OBJSAVECLOSE , IDI_IMAGELIST_OW ), IDM_SAVECLOSE_FCN  ,  TBSTATE_ENABLED, BUTTONSTYLE1, {0}, 0, (INT_PTR) L"Save the function into the current workspace\n"
                                                                                                                                     L"and close it"                                   },
  {MAKELONG (IMAGEINDEX_OW_OBJTOGGLE_LNS, IDI_IMAGELIST_OW ), IDM_TOGGLE_LNS_FCN ,  TBSTATE_ENABLED, BUTTONSTYLE3, {0}, 0, (INT_PTR) L"Toggle display of line numbers"                 },
};

#define NUMBUTTONS_OW       IMAGEINDEX_OW_LENGTH
#define NUMBUTTONS_OW0      IMAGEINDEX_OW_LENGTH0


//***************************************************************
//  Language Window
//***************************************************************

int nLangBands = 1;             // # Language bands (actually it's a height multiplier)


//***************************************************************
// Subclass window procedure
//***************************************************************

COMBOBOXINFO cbi = {sizeof (cbi)};      // ComboBoxInfo struc
WNDPROC lpfnOldFontsComboLboxWndProc;   // Save area for old FontNames ComboLbox window proc


//***************************************************************************
//  $CreateEntireRebarCtrl
//
//  Create the Rebar Ctrl and all its bands
//***************************************************************************

UBOOL CreateEntireRebarCtrl
    (HWND hWndParent)           // Parent window handle

{
    //***************************************************************
    // Create the Rebar control
    //***************************************************************
    hWndRB =
      CreateRebarCtrl (hWndParent);
    if (hWndRB EQ NULL)
    {
        MB (pszNoCreateRBWnd);
        return FALSE;
    } // End IF

    //***************************************************************
    // Create the Workspace Window in the Rebar Ctrl
    //***************************************************************
    hWndWS_RB =
      MakeWorkspaceWindow (hWndParent);
    if (hWndWS_RB EQ NULL)
    {
        MB (pszNoCreateWS_RBWnd);
        return FALSE;
    } // End IF

    //***************************************************************
    // Create the Edit Window in the Rebar Ctrl
    //***************************************************************
    hWndED_RB =
      MakeEditWindow (hWndParent);
    if (hWndED_RB EQ NULL)
    {
        MB (pszNoCreateED_RBWnd);
        return FALSE;
    } // End IF

    //***************************************************************
    // Create the Font Window in the Rebar Ctrl
    //***************************************************************
    hWndFW_RB =
      MakeFontWindow (hWndParent);
    if (hWndFW_RB EQ NULL)
    {
        MB (pszNoCreateFW_RBWnd);
        return FALSE;
    } // End IF

    //***************************************************************
    // Create the Objects Window in the Rebar Ctrl
    //***************************************************************
    hWndOW_RB =
      MakeObjectsWindow (hWndParent);
    if (hWndOW_RB EQ NULL)
    {
        MB (pszNoCreateOW_RBWnd);
        return FALSE;
    } // End IF

    //***************************************************************
    // Create the Language Window in the Rebar Ctrl
    //***************************************************************
    hWndLW_RB =
      MakeLanguageWindow (hWndParent);
    if (hWndLW_RB EQ NULL)
    {
        MB (pszNoCreateLW_RBWnd);
        return FALSE;
    } // End IF

    //***************************************************************
    // Initialize the Workspace Window band info
    //***************************************************************
    InitWorkspaceBand (-1);

    //***************************************************************
    // Initialize the Edit Window band info
    //***************************************************************
    InitEditBand (-1);

    //***************************************************************
    // Initialize the Font Window band info
    //***************************************************************
    InitFontBand (-1);

    //***************************************************************
    // Initialize the Objects Window band info
    //***************************************************************
    InitObjectsBand (-1);

    //***************************************************************
    // Initialize the Language Window band info
    //***************************************************************
    InitLanguageBand (-1);

    return TRUE;
} // End CreateEntireRebarCtrl


//***************************************************************************
//  $CreateRebarCtrl
//
//  Create the Rebar Ctrl
//***************************************************************************

HWND CreateRebarCtrl
    (HWND hWndParent)               // Parent window handle

{
    HWND hWndRes;                   // Result window handle

    hWndRes =
      CreateWindowExW (0L,                  // Extended styles
                       WC_REBARW,           // Class name
                       wszRBTitle,          // Window title (for debugging purposes only)
                       0                    // Styles
                     | WS_BORDER
                     | WS_CHILD
                     | WS_CLIPSIBLINGS
                     | WS_CLIPCHILDREN
                     | WS_VISIBLE

                     | CCS_NODIVIDER
////                 | CCS_NOPARENTALIGN

                     | RBS_BANDBORDERS
                     | RBS_DBLCLKTOGGLE
/////////////////////| RBS_REGISTERDROP     // Used in IE
                     | RBS_VARHEIGHT
///////              | RBS_AUTOSIZE
                       ,
                       0,                   // X-coord
                       0,                   // Y-coord
                       CW_USEDEFAULT,       // X-size
                       CW_USEDEFAULT,       // Y-size (to be sized later)
                       hWndParent,          // Parent window
               (HMENU) IDWC_RB,             // Window ID
                       _hInstance,          // Instance
                       NULL);               // No extra data
    return hWndRes;
} // End CreateRebarCtrl


//***************************************************************************
//  $MakeWorkspaceWindow
//
//  Create the Workspace Window
//***************************************************************************

HWND MakeWorkspaceWindow
    (HWND hWndParent)               // Parent window handle

{
    HWND hWndRes;                   // Result window handle

    //***************************************************************
    // Create an image list
    //***************************************************************
    hImageListWS =
      ImageList_Create (IMAGE_WS_CX,    // Common width in pixels
                        IMAGE_WS_CY,    // ...    height ...
                        0
                      | ILC_COLOR32
                      | ILC_MASK,       // Flags
                        NUMBUTTONS_WS,  // Max # images
                        0);             // # images by which the list can grow
    if (!hImageListWS)
        return NULL;                    // Stop the whole process

    hWndRes =
      CreateWindowExW (0                    // Extended styles
                     | TBSTYLE_EX_DOUBLEBUFFER
                       ,
                       WC_TOOLBARW,         // Class name
                       wszTB_WSTitle,       // Window title (for debugging purposes only)
                       0                    // Styles
                     | WS_CHILD
                     | WS_CLIPSIBLINGS
                     | WS_CLIPCHILDREN
                     | WS_VISIBLE

                     | CCS_ADJUSTABLE
                     | CCS_NORESIZE         // Required when in Rebar Ctrl
                     | CCS_NOPARENTALIGN    // ...

                     | TBSTYLE_ALTDRAG
                     | TBSTYLE_WRAPABLE
                       ,
                       0, 0,                // X- and Y-coord
                       0, 0,                // X- and Y-size
                       hWndParent,          // Parent window
               (HMENU) IDWC_WS_RB,          // Window ID
                       _hInstance,          // Instance
                       NULL);               // No extra data
    if (hWndRes)
    {
        // Subclass the Toolbar so we can handle
        //   WM_MOUSEMOVE/LEAVE
        lpfnOldWS_ED_OWToolbarWndProc = (WNDPROC)
          SetWindowLongPtrW (hWndRes,
                             GWLP_WNDPROC,
                             (APLU3264) (LONG_PTR) (WNDPROC) &LclWS_ED_OWToolbarWndProc);
        // Tell the subclass procedure to initialize its vars
        SendMessageW (hWndRes, MYWM_INIT_EC, 0, (LPARAM) &tbButtonsWS);

        // Tell the Toolbar Ctrl the size of our struc
        SendMessageW (hWndRes, TB_BUTTONSTRUCTSIZE, sizeof (TBBUTTON), 0);

        // Enable multiple image lists
        SendMessageW (hWndRes, CCM_SETVERSION, 5, 0);

        //***************************************************************
        // Fill in the Toolbar Ctrl
        //***************************************************************

        // Load the bitmap strip
        hBitmapWS =
          MyLoadBitmap (_hInstance,                         // Bitmap instance
                        MAKEINTRESOURCE (IDB_WORKSPACE));   // Bitmap ID
        // Add it to the image list
        ImageList_Add (hImageListWS,                        // Handle to image list
                       hBitmapWS,                           // Bitmap handle
                       NULL);                               // No mask
        // Assign the image list to the Toolbar Ctrl
        SendMessageW (hWndRes, TB_SETIMAGELIST, IDI_IMAGELIST_WS , (LPARAM) hImageListWS);
        SendMessageW (hWndRes, TB_SETIMAGELIST, IDI_IMAGELIST_WS0, (LPARAM) hImageListWS);

        // Tell the Toolbar Ctrl to use the iString value as a Tooltip
        SendMessageW (hWndRes, TB_SETMAXTEXTROWS, 0, 0);

        // Tell the Toolbar Ctrl to use drop down arrows
        //   and use iString as tooltip
        SendMessageW (hWndRes, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_MIXEDBUTTONS);

        // Add buttons to the Toolbar Ctrl
        SendMessageW (hWndRes, TB_ADDBUTTONSW, NUMBUTTONS_WS + NUMBUTTONS_WS0, (LPARAM) &tbButtonsWS);

        // Show the window
        ShowWindow (hWndRes, SW_SHOWNORMAL);
    } // End IF
#ifdef DEBUG
    else
    {
        UINT uErr = GetLastError ();

        DbgBrk ();          // #ifdef DEBUG
    } // End IF/ELSE
#endif

    return hWndRes;
} // End MakeWorkspaceWindow


//***************************************************************************
//  $InitWorkspaceBand
//
//  Initialize the Workspace Band
//***************************************************************************

void InitWorkspaceBand
    (APLU3264 uBandPos)         // Insert the band in this position
                                // (-1 = at the end)

{
    REBARBANDINFOW rbBand;          // Rebar band info struc
    DWORD          dwBtnSize;       // Size of buttons in Toolbar
    UINT           uArrowWidth,     // Width of the arrow next to a dropdown button
                   uWidth;          // Width of the Toolbar client area
////UINT           uItem;           // Index of this band

    // Get the size of the buttons in the Toolbar in the Workspace Window
    dwBtnSize = (DWORD)
      SendMessageW (hWndWS_RB, TB_GETBUTTONSIZE, 0, 0);
#define dwBtnHeight     HIWORD (dwBtnSize)
#define dwBtnWidth      LOWORD (dwBtnSize)

    // Get the width of the dropdown arrow
    uArrowWidth = GetSystemMetrics (SM_CXVSCROLL);

    // Calculate the width of the Toolbar client area
    uWidth = NUMBUTTONS_WS * dwBtnWidth + NUMBUTTONS_WS0 * uArrowWidth;

    // Initialize the band info struc
    ZeroMemory (&rbBand, sizeof (rbBand));

    // Specify the attributes of the band
    rbBand.cbSize     = sizeof (REBARBANDINFOW);
    rbBand.fMask      = 0
                      | RBBIM_STYLE     // fStyle is valid
                      | RBBIM_TEXT      // lpText is valid
                      | RBBIM_CHILD     // hwndChild is valid
                      | RBBIM_CHILDSIZE // child size members are valid
                      | RBBIM_IDEALSIZE // cxIdeal is valid
                      | RBBIM_SIZE      // cx is valid
                      | RBBIM_ID        // wID is valid
                      ;
    rbBand.fStyle     = 0
                      | RBBS_CHILDEDGE
                      | RBBS_GRIPPERALWAYS
////                  | RBBS_USECHEVRON
                      ;
////rbBand.clrFore    =
////rbBand.clrBack    =
    rbBand.lpText     = L"WS";
////rbBand.cch        =
////rbBand.iImage     =
    rbBand.hwndChild  = hWndWS_RB;
    rbBand.cxMinChild = uWidth;
    rbBand.cyMinChild = 2 + dwBtnHeight + 2;    // Including top & bottom border
    rbBand.cxIdeal    = uWidth;
    rbBand.cx         = uWidth;
////rbBand.hbmBack    =
    rbBand.wID        = IDWC_WS_RB;
////rbBand.cyChild    =
////rbBand.cyMaxChild =
////rbBand.cyIntegral =
////rbBand.lParam     =
////rbBand.cxheader   =
#undef  dwBtnWidth
#undef  dwBtnHeight

    // Insert the Workspace Window band
    SendMessageW (hWndRB, RB_INSERTBANDW, (WPARAM) uBandPos, (LPARAM) &rbBand);

////// Get the zero-based index of the Workspace Window band
////uItem = SendMessageW (hWndRB, RB_IDTOINDEX, IDWC_WS_RB, 0);
////
////// Maximize this band to the size of cxIdeal
////SendMessageW (hWndRB, RB_MAXIMIZEBAND, uItem, TRUE);
} // End InitWorkspaceBand


//***************************************************************************
//  $MakeEditWindow
//
//  Create the Edit Window
//***************************************************************************

HWND MakeEditWindow
    (HWND hWndParent)               // Parent window handle

{
    HWND hWndRes;                   // Result window handle

    //***************************************************************
    // Create an image list
    //***************************************************************
    hImageListED =
      ImageList_Create (IMAGE_ED_CX,    // Common width in pixels
                        IMAGE_ED_CY,    // ...    height ...
                        0
                      | ILC_COLOR32
                      | ILC_MASK,       // Flags
                        NUMBUTTONS_ED,  // Max # images
                        0);             // # images by which the list can grow
    if (!hImageListED)
        return NULL;                    // Stop the whole process

    hWndRes =
      CreateWindowExW (0                    // Extended styles
                     | TBSTYLE_EX_DOUBLEBUFFER
                       ,
                       WC_TOOLBARW,         // Class name
                       wszTB_EDTitle,       // Window title (for debugging purposes only)
                       0                    // Styles
                     | WS_CHILD
                     | WS_CLIPSIBLINGS
                     | WS_CLIPCHILDREN
                     | WS_VISIBLE

                     | CCS_ADJUSTABLE
                     | CCS_NORESIZE         // Required when in Rebar Ctrl
                     | CCS_NOPARENTALIGN    // ...

                     | TBSTYLE_ALTDRAG
                     | TBSTYLE_WRAPABLE
                       ,
                       0, 0,                // X- and Y-coord
                       0, 0,                // X- and Y-size
                       hWndParent,          // Parent window
               (HMENU) IDWC_ED_RB,          // Window ID
                       _hInstance,          // Instance
                       NULL);               // No extra data
    if (hWndRes)
    {
        // Subclass the Toolbar so we can handle
        //   WM_MOUSEMOVE/LEAVE
        lpfnOldWS_ED_OWToolbarWndProc = (WNDPROC)
          SetWindowLongPtrW (hWndRes,
                             GWLP_WNDPROC,
                             (APLU3264) (LONG_PTR) (WNDPROC) &LclWS_ED_OWToolbarWndProc);
        // Tell the subclass procedure to initialize its vars
        SendMessageW (hWndRes, MYWM_INIT_EC, 0, (LPARAM) &tbButtonsED);

        // Tell the Toolbar Ctrl the size of our struc
        SendMessageW (hWndRes, TB_BUTTONSTRUCTSIZE, sizeof (TBBUTTON), 0);

        // Enable multiple image lists
        SendMessageW (hWndRes, CCM_SETVERSION, 5, 0);

        //***************************************************************
        // Fill in the Toolbar Ctrl
        //***************************************************************

        // Load the bitmap strip
        hBitmapED =
          MyLoadBitmap (_hInstance,                         // Bitmap instance
                        MAKEINTRESOURCE (IDB_EDIT));        // Bitmap ID
        // Add it to the image list
        ImageList_Add (hImageListED,                        // Handle to image list
                       hBitmapED,                           // Bitmap handle
                       NULL);                               // No mask
        // Assign the image list to the Toolbar Ctrl
        SendMessageW (hWndRes, TB_SETIMAGELIST, IDI_IMAGELIST_ED , (LPARAM) hImageListED);
        SendMessageW (hWndRes, TB_SETIMAGELIST, IDI_IMAGELIST_ED0, (LPARAM) hImageListED);

        // Tell the Toolbar Ctrl to use the iString value as a Tooltip
        SendMessageW (hWndRes, TB_SETMAXTEXTROWS, 0, 0);

        // Tell the Toolbar Ctrl to use drop down arrows
        //   and use iString as tooltip
        SendMessageW (hWndRes, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_MIXEDBUTTONS);

        // Add buttons to the Toolbar Ctrl
        SendMessageW (hWndRes, TB_ADDBUTTONSW, NUMBUTTONS_ED + NUMBUTTONS_ED0, (LPARAM) &tbButtonsED);

        // Show the window
        ShowWindow (hWndRes, SW_SHOWNORMAL);
    } // End IF
#ifdef DEBUG
    else
    {
        UINT uErr = GetLastError ();

        DbgBrk ();          // #ifdef DEBUG
    } // End IF/ELSE
#endif

    return hWndRes;
} // End MakeEditWindow


//***************************************************************************
//  $InitEditBand
//
//  Initialize the Edit Band
//***************************************************************************

void InitEditBand
    (APLU3264 uBandPos)         // Insert the band in this position
                                // (-1 = at the end)

{
    REBARBANDINFOW rbBand;          // Rebar band info struc
    DWORD          dwBtnSize;       // Size of buttons in Toolbar
    UINT           uArrowWidth,     // Width of the arrow next to a dropdown button
                   uWidth;          // Width of the Toolbar client area
////UINT           uItem;           // Index of this band

    // Get the size of the buttons in the Toolbar in the Edit Window
    dwBtnSize = (DWORD)
      SendMessageW (hWndED_RB, TB_GETBUTTONSIZE, 0, 0);
#define dwBtnHeight     HIWORD (dwBtnSize)
#define dwBtnWidth      LOWORD (dwBtnSize)

    // Get the width of the dropdown arrow
    uArrowWidth = GetSystemMetrics (SM_CXVSCROLL);

    // Calculate the width of the Toolbar client area
    uWidth = NUMBUTTONS_ED * dwBtnWidth + NUMBUTTONS_ED0 * uArrowWidth;

    // Initialize the band info struc
    ZeroMemory (&rbBand, sizeof (rbBand));

    // Specify the attributes of the band
    rbBand.cbSize     = sizeof (REBARBANDINFOW);
    rbBand.fMask      = 0
                      | RBBIM_STYLE     // fStyle is valid
                      | RBBIM_TEXT      // lpText is valid
                      | RBBIM_CHILD     // hwndChild is valid
                      | RBBIM_CHILDSIZE // child size members are valid
                      | RBBIM_IDEALSIZE // cxIdeal is valid
                      | RBBIM_SIZE      // cx is valid
                      | RBBIM_ID        // wID is valid
                      ;
    rbBand.fStyle     = 0
                      | RBBS_CHILDEDGE
                      | RBBS_GRIPPERALWAYS
////                  | RBBS_USECHEVRON
                      ;
////rbBand.clrFore    =
////rbBand.clrBack    =
    rbBand.lpText     = L"Edit";
////rbBand.cch        =
////rbBand.iImage     =
    rbBand.hwndChild  = hWndED_RB;
    rbBand.cxMinChild = uWidth;
    rbBand.cyMinChild = 2 + dwBtnHeight + 2;    // Including top & bottom border
    rbBand.cxIdeal    = uWidth;
    rbBand.cx         = uWidth;
////rbBand.hbmBack    =
    rbBand.wID        = IDWC_ED_RB;
////rbBand.cyChild    =
////rbBand.cyMaxChild =
////rbBand.cyIntegral =
////rbBand.lParam     =
////rbBand.cxheader   =
#undef  dwBtnWidth
#undef  dwBtnHeight

    // Insert the Edit Window band
    SendMessageW (hWndRB, RB_INSERTBANDW, (WPARAM) uBandPos, (LPARAM) &rbBand);

////// Get the zero-based index of the Edit Window band
////uItem = SendMessageW (hWndRB, RB_IDTOINDEX, IDWC_ED_RB, 0);
////
////// Maximize this band to the size of cxIdeal
////SendMessageW (hWndRB, RB_MAXIMIZEBAND, uItem, TRUE);
} // End InitEditBand


//***************************************************************************
//  $MakeObjectsWindow
//
//  Create the Objects Window in the Rebar Ctrl
//***************************************************************************

HWND MakeObjectsWindow
    (HWND hWndParent)               // Parent window handle

{
    HWND hWndRes;                   // Result window handle

    //***************************************************************
    // Create an image list
    //***************************************************************
    hImageListOW =
      ImageList_Create (IMAGE_OW_CX,    // Common width in pixels
                        IMAGE_OW_CY,    // ...    height ...
                        0
                      | ILC_COLOR32
                      | ILC_MASK,       // Flags
                        NUMBUTTONS_OW,  // Max # images
                        0);             // # images by which the list can grow
    if (!hImageListOW)
        return NULL;                    // Stop the whole process

    hWndRes =
      CreateWindowExW (0                    // Extended styles
                     | TBSTYLE_EX_DOUBLEBUFFER
                       ,
                       WC_TOOLBARW,         // Class name
                       wszTB_OWTitle,       // Window title (for debugging purposes only)
                       0                    // Styles
                     | WS_CHILD
                     | WS_CLIPSIBLINGS
                     | WS_CLIPCHILDREN
                     | WS_VISIBLE

                     | CCS_ADJUSTABLE
                     | CCS_NORESIZE         // Required when in Rebar Ctrl
                     | CCS_NOPARENTALIGN    // ...

                     | TBSTYLE_ALTDRAG
                     | TBSTYLE_WRAPABLE
                       ,
                       0, 0,                // X- and Y-coord
                       0, 0,                // X- and Y-size
                       hWndParent,          // Parent window
               (HMENU) IDWC_OW_RB,          // Window ID
                       _hInstance,          // Instance
                       NULL);               // No extra data
    if (hWndRes)
    {
        // Subclass the Toolbar so we can handle
        //   WM_MOUSEMOVE/LEAVE
        lpfnOldWS_ED_OWToolbarWndProc = (WNDPROC)
          SetWindowLongPtrW (hWndRes,
                             GWLP_WNDPROC,
                             (APLU3264) (LONG_PTR) (WNDPROC) &LclWS_ED_OWToolbarWndProc);
        // Tell the subclass procedure to initialize its vars
        SendMessageW (hWndRes, MYWM_INIT_EC, 0, (LPARAM) &tbButtonsOW);

        // Tell the Toolbar Ctrl the size of our struc
        SendMessageW (hWndRes, TB_BUTTONSTRUCTSIZE, sizeof (TBBUTTON), 0);

        // Enable multiple image lists
        SendMessageW (hWndRes, CCM_SETVERSION, 5, 0);

        //***************************************************************
        // Fill in the Toolbar Ctrl
        //***************************************************************

        // Load the bitmap strip
        hBitmapOW =
          MyLoadBitmap (_hInstance,                         // Bitmap instance
                        MAKEINTRESOURCE (IDB_OBJECTS));     // Bitmap ID
        // Add it to the image list
        ImageList_Add (hImageListOW,                        // Handle to image list
                       hBitmapOW,                           // Bitmap handle
                       NULL);                               // No mask
        // Assign the image list to the Toolbar Ctrl
        SendMessageW (hWndRes, TB_SETIMAGELIST, IDI_IMAGELIST_OW , (LPARAM) hImageListOW);
        SendMessageW (hWndRes, TB_SETIMAGELIST, IDI_IMAGELIST_OW0, (LPARAM) hImageListOW);

        // Tell the Toolbar Ctrl to use the iString value as a Tooltip
        SendMessageW (hWndRes, TB_SETMAXTEXTROWS, 0, 0);

        // Tell the Toolbar Ctrl to use drop down arrows
        //   and use iString as tooltip
        SendMessageW (hWndRes, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_MIXEDBUTTONS);

        // Add buttons to the Toolbar Ctrl
        SendMessageW (hWndRes, TB_ADDBUTTONSW, NUMBUTTONS_OW + NUMBUTTONS_OW0, (LPARAM) &tbButtonsOW);

        // Show the window
        ShowWindow (hWndRes, SW_SHOWNORMAL);
    } // End IF
#ifdef DEBUG
    else
    {
        UINT uErr = GetLastError ();

        DbgBrk ();          // #ifdef DEBUG
    } // End IF/ELSE
#endif

    return hWndRes;
} // End MakeObjectsWindow


//***************************************************************************
//  $InitObjectsBand
//
//  Initialize the Objects Band
//***************************************************************************

void InitObjectsBand
    (APLU3264 uBandPos)         // Insert the band in this position
                                // (-1 = at the end)

{
    REBARBANDINFOW rbBand;          // Rebar band info struc
    DWORD          dwBtnSize;       // Size of buttons in Toolbar
    UINT           uArrowWidth,     // Width of the arrow next to a dropdown button
                   uWidth;          // Width of the Toolbar client area
////UINT           uItem;           // Index of this band

    // Get the size of the buttons in the Toolbar in the Objects Window
    dwBtnSize = (DWORD)
      SendMessageW (hWndOW_RB, TB_GETBUTTONSIZE, 0, 0);
#define dwBtnHeight     HIWORD (dwBtnSize)
#define dwBtnWidth      LOWORD (dwBtnSize)

    // Get the width of the dropdown arrow
    uArrowWidth = GetSystemMetrics (SM_CXVSCROLL);

    // Calculate the width of the Toolbar client area
    uWidth = NUMBUTTONS_OW * dwBtnWidth + NUMBUTTONS_OW0 * uArrowWidth;

    // Initialize the band info struc
    ZeroMemory (&rbBand, sizeof (rbBand));

    // Specify the attributes of the band
    rbBand.cbSize     = sizeof (REBARBANDINFOW);
    rbBand.fMask      = 0
                      | RBBIM_STYLE     // fStyle is valid
                      | RBBIM_TEXT      // lpText is valid
                      | RBBIM_CHILD     // hwndChild is valid
                      | RBBIM_CHILDSIZE // child size members are valid
                      | RBBIM_IDEALSIZE // cxIdeal is valid
                      | RBBIM_SIZE      // cx is valid
                      | RBBIM_ID        // wID is valid
                      ;
    rbBand.fStyle     = 0
                      | RBBS_CHILDEDGE
                      | RBBS_GRIPPERALWAYS
////                  | RBBS_USECHEVRON
                      ;
////rbBand.clrFore    =
////rbBand.clrBack    =
    rbBand.lpText     = L"Objects";
////rbBand.cch        =
////rbBand.iImage     =
    rbBand.hwndChild  = hWndOW_RB;
    rbBand.cxMinChild = uWidth;
    rbBand.cyMinChild = 2 + dwBtnHeight + 2;    // Including top & bottom border
    rbBand.cxIdeal    = uWidth;
    rbBand.cx         = uWidth;
////rbBand.hbmBack    =
    rbBand.wID        = IDWC_OW_RB;
////rbBand.cyChild    =
////rbBand.cyMaxChild =
////rbBand.cyIntegral =
////rbBand.lParam     =
////rbBand.cxheader   =
#undef  dwBtnWidth
#undef  dwBtnHeight

    // Insert the Objects Window band
    SendMessageW (hWndRB, RB_INSERTBANDW, (WPARAM) uBandPos, (LPARAM) &rbBand);

////// Get the zero-based index of the Objects Window band
////uItem = SendMessageW (hWndRB, RB_IDTOINDEX, IDWC_OW_RB, 0);
////
////// Maximize this band to the size of cxIdeal
////SendMessageW (hWndRB, RB_MAXIMIZEBAND, uItem, TRUE);
} // End InitObjectsBand


//***************************************************************************
//  $LclWS_ED_OWToolbarWndProc
//
//  Local window procedure for the Workspace/Edit/Objects Window TOOLBAR
//***************************************************************************


LRESULT WINAPI LclWS_ED_OWToolbarWndProc
    (HWND   hWnd,       // Window handle
     UINT   message,    // Type of message
     WPARAM wParam,     // Additional information
     LPARAM lParam)     // ...

{
    POINT            pt;                    // Mouse coords
    APLI3264         iCnt;                  // Index of non-separator item (may be negative)
    static UBOOL     bTrackMouse = FALSE;   // TRUE iff tracking the mouse
    TOOLINFOW        tti;                   // For Tooltip Ctrl tracking
#define NOLASTCNT    0x3FFFFFFF
    static APLI3264  iLastCnt = NOLASTCNT;  // Last valid index
    TBBUTTON         tbbi;                  // For Toolbar Button Info
    UINT             uArrowWidth;           // Width of the arrow next to a dropdown button
    static HWND      hWndTT;                // Tooltip window handle

////LCLODSAPI ("OWTB: ", hWnd, message, wParam, lParam);
    // Split cases based upon the message
    switch (message)
    {
        case MYWM_INIT_EC:          // 0 = wParam
        {                           // lptbButton = (LPTBBUTTON) lParam;
#ifdef DEBUG
            LPTBBUTTON lptbButton = (LPTBBUTTON) lParam;
#else
  #define lptbButton    ((LPTBBUTTON) lParam)
#endif
            // If the Tooltip window has not been created as yet, ...
            if (hWndTT EQ NULL)
            {
                // Create it
                hWndTT = CreateTooltip (TRUE);
                if (hWndTT EQ NULL)
                    return -1;      // Mark as in error
            } // End IF

            // Set the maximum tip width
            SetMaxTipWidth (hWndTT);

            // Fill in the TOOLINFOW size based upon the matching COMCTL32.DLL version #
            ZeroMemory (&tti, sizeof (tti));
            if (fComctl32FileVer >= 6)
                tti.cbSize = sizeof (tti);
            else
                tti.cbSize = TTTOOLINFOW_V2_SIZE;

            // Fill in the TOOLINFOW struc
            tti.uFlags   = 0
                         | TTF_IDISHWND
                         | TTF_TRACK
                         ;
            tti.hwnd     = hWnd;
            tti.uId      = (UINT_PTR) hWnd;
////////////tti.rect     =                      // Not used with TTF_IDISHWND
////////////tti.hinst    =                      // Not used except with string resources
            tti.lpszText = LPSTR_TEXTCALLBACKW;
            tti.lParam   = lParam;

            // Attach the Tooltip window to it
            SendMessageW (hWndTT, TTM_ADDTOOLW, 0, (LPARAM) &tti);

            return FALSE;               // We handled the msg
#ifndef DEBUG
  #undef  lptbButton
#endif
        } // End MYWM_INIT_EC

        case WM_SETFONT:
        {
#ifdef DEBUG
            HFONT hFontTT = (HFONT) wParam; // Tooltip font handle
#else
  #define hFontTT   ((HFONT) wParam)
#endif
            // Tell the Tooltip window to use the same font
            SendMessageW (hWndTT, WM_SETFONT, (WPARAM) hFontTT, MAKELPARAM (TRUE, 0));

            // Set the maximum tip width
            SetMaxTipWidth (hWndTT);

            break;
#ifndef DEBUG
  #undef  hFontTT
#endif
        } // End WM_SETFONT

        case WM_NOTIFY:             // idCtrl = (int) wParam;
        {                           // pnmh = (LPNMHDR) lParam;
#ifdef DEBUG
            LPNMHDR         lpnmhdr = (LPNMHDR) lParam;
            LPNMTTDISPINFOW lpnmtdi = (LPNMTTDISPINFOW) lParam;
            WPARAM          idCtl   = wParam;
#else
  #define lpnmhdr         ((LPNMHDR) lParam)
  #define lpnmtdi         ((LPNMTTDISPINFOW) lParam)
  #define idCtl           ((WPARAM) wParam)
#endif
            // Split cases based upon the code
            switch (lpnmhdr->code)
            {
////////////////case TTN_NEEDTEXTW:
                case TTN_GETDISPINFOW:

                    // If there's a last index, ...
                    if (iLastCnt NE NOLASTCNT)
                    {
                        // Fill in the TOOLINFOW size based upon the matching COMCTL32.DLL version #
                        ZeroMemory (&tti, sizeof (tti));
                        if (fComctl32FileVer >= 6)
                            tti.cbSize = sizeof (tti);
                        else
                            tti.cbSize = TTTOOLINFOW_V2_SIZE;

                        // Initialize the Tooltip Info struct
                        tti.hwnd = hWnd;
                        tti.uId  = (UINT_PTR) hWnd;

                        // Get the tooltip info
                        SendMessageW (hWndTT, TTM_GETTOOLINFOW, 0, (LPARAM) &tti);

                        // Set the Tooltip text
                        lpnmtdi->lpszText = (LPWSTR) ((LPTBBUTTON) tti.lParam)[iLastCnt].iString;
                    } // End IF

                    break;

                default:
                    break;
            } // End SWITCH

            return FALSE;       // We handled the msg
#ifndef DEBUG
  #undef  idCtl
  #undef  lpnmtdi
  #undef  lpnmhdr
#endif
        } // End WM_NOTIFY

        case WM_MOUSEMOVE:          // fwKeys = wParam;        // key flags
                                    // xPos = LOWORD(lParam);  // horizontal position of cursor (CA)
                                    // yPos = HIWORD(lParam);  // vertical position of cursor (CA)
            // Save the mouse position as a point
            pt.x = GET_X_LPARAM (lParam);
            pt.y = GET_Y_LPARAM (lParam);

            // Ask the Toolbar where we are
            // If the return value is >= 0, it is the button # the mouse is over
            iCnt = (APLI3264)
              SendMessageW (hWnd, TB_HITTEST, 0, (LPARAM) &pt);

            // If the index is over an item, ...
            if (iCnt >= 0)
            {
                RECT rcHit;         // This item's bounding rectangle

                // Fill in the TOOLINFOW size based upon the matching COMCTL32.DLL version #
                ZeroMemory (&tti, sizeof (tti));
                if (fComctl32FileVer >= 6)
                    tti.cbSize = sizeof (tti);
                else
                    tti.cbSize = TTTOOLINFOW_V2_SIZE;
                // Initialize the struct
                tti.hwnd = hWnd;
                tti.uId  = (UINT_PTR) hWnd;

                // Get this item's bounding rectangle
                SendMessageW (hWnd, TB_GETITEMRECT, iCnt, (LPARAM) &rcHit);

                // Get the Toolbar button info
                SendMessageW (hWnd, TB_GETBUTTON, iCnt, (LPARAM) &tbbi);

                // If this a BTNS_DROPDOWN button, ...
                if (tbbi.fsStyle & BTNS_DROPDOWN)
                {
                    // Get the width of the dropdown arrow
                    uArrowWidth = GetSystemMetrics (SM_CXVSCROLL);

                    // If the mouse is over the dropdown arrow
                    if (pt.x >= (int) (rcHit.right - uArrowWidth))
                        // Skip to the dropdown item #
                        iCnt++;

                    // If this index is different from the last and we're tracking the mouse, ...
                    if (iCnt NE iLastCnt
                     && bTrackMouse)
                    {
                        // We need to toggle the Tooltip tracking state off/on to tell the
                        //   Tooltip Ctrl to resend TTN_GETDISPINFOW.

                        // Tell the Tooltip Ctrl to stop tracking
                        SendMessageW (hWndTT, TTM_TRACKACTIVATE, FALSE, (LPARAM) &tti);

                        // Clear tracking
                        PostMessageW (hWnd, MYWM_MOUSELEAVE, wParam, lParam);

                        // Mark as no longer tracking
                        bTrackMouse = FALSE;
                    } // End IF
                } // End IF

                // If this index is different from the last, ...
                if (iCnt NE iLastCnt)
                {
                    // Save the new last index
                    iLastCnt = iCnt;

                    // In case we are now over a BTNS_DROPDOWN button

                    // Set the Tooltip balloon to the bottom right-hand corner of the
                    //   Hit rectangle so that it always appears in a constant place
                    pt.x = rcHit.right;
                    pt.y = rcHit.bottom;

                    // Convert the coords for the Tooltip
                    ClientToScreen (hWnd, &pt);

                    // Tell the Tooltip Ctrl the new position
                    SendMessageW (hWndTT, TTM_TRACKPOSITION, 0, MAKELONG (pt.x, pt.y));
                } // End IF

                // If not already tracking, ...
                if (!bTrackMouse)
                {
                    // Tell the Tooltip Ctrl to track
                    SendMessageW (hWndTT, TTM_TRACKACTIVATE, TRUE, (LPARAM) &tti);

                    // Mark as tracking
                    bTrackMouse = TRUE;
                } // End IF
            } else
            {
                // The mouse is outside our window
                // Clear tracking
                PostMessageW (hWnd, MYWM_MOUSELEAVE, wParam, lParam);

                // Mark as no longer tracking
                bTrackMouse = FALSE;
            } // End IF

            break;                  // Pass on to the next handler

        case WM_LBUTTONUP:          // fwKeys = wParam;        // key flags
                                    // xPos = LOWORD(lParam);  // horizontal position of cursor (CA)
                                    // yPos = HIWORD(lParam);  // vertical position of cursor (CA)
            // This message is here to avoid the TT from remaining
            //   on the screen if the user clicks on a Toolbar button

            // If we're tracking the mouse, ...
            if (bTrackMouse)
                // Clear tracking
                PostMessageW (hWnd, MYWM_MOUSELEAVE, wParam, lParam);
            break;

        case WM_MOUSELEAVE:         // fwKeys = wParam;        // key flags
                                    // xPos = LOWORD(lParam);  // horizontal position of cursor (CA)
                                    // yPos = HIWORD(lParam);  // vertical position of cursor (CA)
            // Call common code to clear tracking
            SendMessageW (hWnd, MYWM_MOUSELEAVE, wParam, lParam);

            break;                  // Pass on to the next handler

        case MYWM_MOUSELEAVE:       // fwKeys = wParam;        // key flags
                                    // xPos = LOWORD(lParam);  // horizontal position of cursor (CA)
                                    // yPos = HIWORD(lParam);  // vertical position of cursor (CA)
            // Fill in the TOOLINFOW size based upon the matching COMCTL32.DLL version #
            ZeroMemory (&tti, sizeof (tti));
            if (fComctl32FileVer >= 6)
                tti.cbSize = sizeof (tti);
            else
                tti.cbSize = TTTOOLINFOW_V2_SIZE;

            // Initialize the struct
            tti.hwnd = hWnd;
            tti.uId  = (UINT_PTR) hWnd;

            // Tell the Tooltip Ctrl to stop tracking
            SendMessageW (hWndTT, TTM_TRACKACTIVATE, FALSE, (LPARAM) &tti);

            // Mark as no longer valid
            iLastCnt = NOLASTCNT;

            // Mark as no longer tracking
            bTrackMouse = FALSE;

            return FALSE;               // We handled the msg

        case WM_DESTROY:
            // If the Tooltip window is still present, ...
            if (hWndTT)
            {
                // Destroy it
                DestroyWindow (hWndTT); hWndTT = NULL;
            } // End IF

            break;

        default:
            break;
    } // End SWITCH

////LCLODSAPI ("OWTBZ:", hWnd, message, wParam, lParam);
    return CallWindowProcW (lpfnOldWS_ED_OWToolbarWndProc,
                            hWnd,
                            message,
                            wParam,
                            lParam); // Pass on down the line
} // End LclWS_ED_OWToolbarWndProc


//***************************************************************************
//  $SetMaxTipWidth
//
//  Set the maximum Tooltip width
//***************************************************************************

void SetMaxTipWidth
    (HWND  hWndTT)                  // Tooltip window handle
{
    HFONT       hFontTT,            // Tooltip font handle
                hFontOld;           // Old DC font handle (if any)
    HDC         hDCTmp;             // Temporary Device Context handle
    int         iOldMode;           // Old mapping mode
    TEXTMETRICW tm;                 // Textmetrics for the current font

    // Get the current font for the Tooltip window
    hFontTT = (HFONT) SendMessageW (hWndTT, WM_GETFONT, 0, 0);

    // Get a new device context
    hDCTmp = MyGetDC (hWndTT);

    // Set the mapping mode
    iOldMode = SetMapMode (hDCTmp, MM_TEXT);

    // Select the newly created font
    hFontOld = SelectObject (hDCTmp, hFontTT);

    // Get the text metrics for this font
    GetTextMetricsW (hDCTmp, &tm);

    // Restore the old font
    SelectObject (hDCTmp, hFontOld);

    // Restore the mapping mode
    SetMapMode (hDCTmp, iOldMode);

    // Release the one we created
    MyReleaseDC (hWndTT, hDCTmp); hDCTmp = NULL;

    // Set the maximum Tooltip width in pixels
    SendMessageW (hWndTT, TTM_SETMAXTIPWIDTH, 0, tm.tmAveCharWidth * MAXTIPWIDTHINCHARS);
} // End SetMaxTipWidth


//***************************************************************************
//  $DeleteImageBitmaps
//
//  Delete the image bitmaps for the Workspace Window in the Rebar Ctrl
//***************************************************************************

void DeleteImageBitmaps
    (void)

{
    MyDeleteObject (hBitmapOW); hBitmapOW = NULL;
    MyDeleteObject (hBitmapED); hBitmapED = NULL;
    MyDeleteObject (hBitmapWS); hBitmapWS = NULL;
} // End DeleteImageBitmaps


//***************************************************************************
//  $MakeFontWindow
//
//  Create the Font Window in the Rebar Ctrl
//***************************************************************************

HWND MakeFontWindow
    (HWND hWndParent)               // Parent window handle

{
    HWND hWndRes;                   // Result window handle

    hWndRes =
      CreateWindowExW (0,                   // Extended styles
                       LFW_RBWNDCLASS,      // Class
                       wszFW_RBTitle,       // Window title (for debugging purposes only)
                       0                    // Styles
                     | WS_CHILD
                     | WS_CLIPSIBLINGS
                     | WS_CLIPCHILDREN
                     | WS_VISIBLE
                       ,
                       0, 0,                // X- and Y-coord
                       CW_USEDEFAULT,       // X-size
                       CW_USEDEFAULT,       // Y-size
                       hWndParent,          // Parent window
               (HMENU) IDWC_FW_RB,          // Window ID
                       _hInstance,          // Instance
                       NULL);               // No extra data
    if (hWndRes)
        // Show the window
        ShowWindow (hWndRes, SW_SHOWNORMAL);
#ifdef DEBUG
    else
    {
        UINT uErr = GetLastError ();

        DbgBrk ();          // #ifdef DEBUG
    } // End IF/ELSE
#endif

    return hWndRes;
} // End MakeFontWindow


//***************************************************************************
//  $FW_RB_Create
//
//  Perform window-specific initialization
//***************************************************************************

void FW_RB_Create
    (HWND hWnd)

{
} // End FW_RB_Create


//***************************************************************************
//  $FW_RB_Delete
//
//  Perform window-specific uninitialization
//***************************************************************************

void FW_RB_Delete
    (HWND hWnd)

{
} // End FW_RB_Delete


//***************************************************************************
//  $FW_RBWndProc
//
//  Message processing routine for the Font Window in Rebar Ctrl window
//***************************************************************************

LRESULT APIENTRY FW_RBWndProc
    (HWND   hWnd,       // Window handle
     UINT   message,    // Type of message
     WPARAM wParam,     // Additional information
     LPARAM lParam)     // ...

{
           TOOLINFOW tti;                       // For Tooltip Ctrl
           RECT      rc;                        // Temporary rectangle
    static HWND      hWndTT,                    // Tooltip window handle
                     hWndTT_CBL;                // ...                   for ComboLbox
    static HFONT     hFontCBFN = NULL;          // Font Name ComboBox font handle

////LCLODSAPI ("FW_RB: ", hWnd, message, wParam, lParam);
    switch (message)
    {
        case WM_CREATE:
            // Create a Tooltip window
            hWndTT = CreateTooltip (TRUE);

            // Check for errors
            if (hWndTT EQ NULL)
                return -1;                      // Stop the whole process

            // Create a Tooltip window for the ComboLbox
            hWndTT_CBL = CreateTooltip (FALSE);

            // Check for errors
            if (hWndTT_CBL EQ NULL)
                return -1;                      // Stop the whole process

            // Fill in the TOOLINFOW size based upon the matching COMCTL32.DLL version #
            ZeroMemory (&tti, sizeof (tti));
            if (fComctl32FileVer >= 6)
                tti.cbSize = sizeof (tti);
            else
                tti.cbSize = TTTOOLINFOW_V2_SIZE;

            //***************************************************************
            // Create a ComboBox Ctrl for the font name
            //***************************************************************
            hWndCBFN_FW =
              CreateWindowExW (0L,                  // Extended styles
                               WC_COMBOBOXW,
                               L"Font Window Font Name ComboBox",// For debugging only
                               0                    // Styles
                             | WS_CHILD
                             | WS_BORDER
                             | WS_VISIBLE

                             | CBS_DROPDOWNLIST
                             | CBS_SORT
                             | CBS_AUTOHSCROLL
                             | CBS_OWNERDRAWFIXED
                             | CBS_HASSTRINGS
                               ,
                               xComboFN,            // X-position
                               0,                   // Y-...
                               cxComboFN,           // Width
                               cyComboFN,           // Height
                               hWnd,                // Parent window
                       (HMENU) IDWC_CBFN_FW,        // Window ID
                               _hInstance,          // Instance
                               0);                  // lParam
            // Fill in the TOOLINFOW struc
            tti.uFlags   = 0
                         | TTF_IDISHWND
                         | TTF_SUBCLASS
                         ;
            tti.hwnd     = hWndCBFN_FW;
            tti.uId      = (UINT_PTR) hWndCBFN_FW;
////////////tti.rect     =                      // Not used with TTF_IDISHWND
////////////tti.hinst    =                      // Not used except with string resources
            tti.lpszText = L"Session Font Name";
////////////tti.lParam   =                      // Not used by this code

            // Attach the Tooltip window to it
            SendMessageW (hWndTT, TTM_ADDTOOLW, 0, (LPARAM) &tti);

            // Get the ComboBox info
            SendMessageW (hWndCBFN_FW, CB_GETCOMBOBOXINFO, 0, (LPARAM) &cbi);

            Assert (hWndCBFN_FW EQ cbi.hwndCombo);

            // Save the TT window handle as a window property
            SetPropW (cbi.hwndList, L"hWndTT", hWndTT_CBL);

            // Get the default font for the Font name ComboBox
            hFontCBFN = GetComboBoxFont (hWnd, hFontCBFN, hWndTT_CBL);

            // Subclass the Fonts ComboLbox so we can display a Tooltip
            (HANDLE_PTR) lpfnOldFontsComboLboxWndProc =
              SetWindowLongPtrW (cbi.hwndList,
                                 GWLP_WNDPROC,
                                 (APLU3264) (LONG_PTR) (WNDPROC) &LclFontsComboLboxWndProc);
            // Fill the ComboBox with Font Names
            ReadAplFontNames (hWndCBFN_FW);

            // Change the font name in the ComboBox in the Font Window
            InitFontName ();

            //***************************************************************
            // Create a ComboBox Ctrl for the font style
            //***************************************************************
            hWndCBFS_FW =
              CreateWindowExW (0L,                  // Extended styles
                               WC_COMBOBOXW,
                               L"Font Window Font Style ComboBox",// For debugging only
                               0                    // Styles
                             | WS_CHILD
                             | WS_BORDER
                             | WS_VISIBLE

                             | CBS_DROPDOWNLIST
                               ,
                               xComboFS,            // X-position
                               0,                   // Y-...
                               cxComboFS,           // Width
                               cyComboFS,           // Height
                               hWnd,                // Parent window
                       (HMENU) IDWC_CBFS_FW,        // Window ID
                               _hInstance,          // Instance
                               0);                  // lParam
            // Fill in the TOOLINFOW struc
            tti.uFlags   = 0
                         | TTF_IDISHWND
                         | TTF_SUBCLASS
                         ;
            tti.hwnd     = hWndCBFS_FW;
            tti.uId      = (UINT_PTR) hWndCBFS_FW;
////////////tti.rect     =                      // Not used with TTF_IDISHWND
////////////tti.hinst    =                      // Not used except with string resources
            tti.lpszText = L"Session Font Style";
////////////tti.lParam   =                      // Not used by this code

            // Attach the Tooltip window to it
            SendMessageW (hWndTT, TTM_ADDTOOLW, 0, (LPARAM) &tti);

            // Fill the ComboBox with Font Styles
            SendMessageW (hWndCBFS_FW, CB_ADDSTRING, 0, (LPARAM) L"Regular");
            SendMessageW (hWndCBFS_FW, CB_ADDSTRING, 0, (LPARAM) L"Italic");
            SendMessageW (hWndCBFS_FW, CB_ADDSTRING, 0, (LPARAM) L"Bold");
            SendMessageW (hWndCBFS_FW, CB_ADDSTRING, 0, (LPARAM) L"Bold Italic");

            // Change the font style in the ComboBox in the Font Window
            InitFontStyle ();

            // Get the outer dimensions of the ComboBox
            GetWindowRect (hWndCBFS_FW, &rc);

            //***************************************************************
            // Create an Edit Ctrl for the font size
            //***************************************************************
            hWndEC_FW =
              CreateWindowExW (0L,                  // Extended styles
                               WC_EDITW,
                               L"Font Window Font Size Edit Ctrl",// For debugging only
                               0                    // Styles
                             | WS_CHILD
                             | WS_BORDER
                             | WS_VISIBLE

                             | ES_NUMBER
                             | ES_CENTER
                             | ES_READONLY
                               ,
                               xEdit,               // X-position
                               0,                   // Y-...
                               cxEdit,              // Width
                               rc.bottom - rc.top,  // Height
                               hWnd,                // Parent window
                       (HMENU) IDWC_EC_FW,          // Window ID
                               _hInstance,          // Instance
                               0);                  // lParam
            // Fill in the TOOLINFOW struc
            tti.uFlags   = 0
                         | TTF_IDISHWND
                         | TTF_SUBCLASS
                         ;
            tti.hwnd     = hWndEC_FW;
            tti.uId      = (UINT_PTR) hWndEC_FW;
////////////tti.rect     =                      // Not used with TTF_IDISHWND
////////////tti.hinst    =                      // Not used except with string resources
            tti.lpszText = L"Session Font Size";
////////////tti.lParam   =                      // Not used by this code

            // Attach the Tooltip window to it
            SendMessageW (hWndTT, TTM_ADDTOOLW, 0, (LPARAM) &tti);

            // Change the point size in the Listbox in the Font Window
            InitPointSize ();

            //***************************************************************
            // Create an UpDown Ctrl for the font size
            //***************************************************************
            hWndUD_FW =
              CreateWindowExW (0L,                  // Extended styles
                               WC_UPDOWNW,          // Class name
                               L"Font Window UpDown Ctrl",  // For debugging only
                               0                    // Styles
                             | WS_CHILD
                             | WS_BORDER
                             | WS_VISIBLE

                             | UDS_AUTOBUDDY
                             | UDS_SETBUDDYINT
                             | UDS_ARROWKEYS
                             | UDS_ALIGNRIGHT
                             | UDS_HOTTRACK
                               ,
                               xUpDown,             // X-position
                               0,                   // Y-...
                               cxUpDown,            // Width
                               rc.bottom - rc.top,  // Height
                               hWnd,                // Parent window
                       (HMENU) IDWC_UD_FW,          // Window ID
                               _hInstance,          // Instance
                               0);                  // lParam
            // Fill in the TOOLINFOW struc
            tti.uFlags   = 0
                         | TTF_IDISHWND
                         | TTF_SUBCLASS
                         ;
            tti.hwnd     = hWndUD_FW;
            tti.uId      = (UINT_PTR) hWndUD_FW;
////////////tti.rect     =                      // Not used with TTF_IDISHWND
////////////tti.hinst    =                      // Not used except with string resources
            tti.lpszText = L"Session Font Size Spinner";
////////////tti.lParam   =                      // Not used by this code

            // Attach the Tooltip window to it
            SendMessageW (hWndTT, TTM_ADDTOOLW, 0, (LPARAM) &tti);

            // Set the range of point sizes and initial value
            SendMessageW (hWndUD_FW, UDM_SETRANGE32, MIN_PTSIZE, MAX_PTSIZE);
            SendMessageW (hWndUD_FW, UDM_SETPOS32,   0         , cfSM.iPointSize / 10);

            // Initialize window-specific resources
            FW_RB_Create (hWnd);

            break;

        case WM_DRAWITEM:           // idCtl = (UINT) wParam;             // Control identifier
        {                           // lpdis = (LPDRAWITEMSTRUCT) lParam; // Item-drawing information
#ifdef DEBUG
            UINT             idCtl = (UINT) wParam;
            LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;
#else
  #define idCtl       ((UINT) wParam)
  #define lpdis       ((LPDRAWITEMSTRUCT) lParam)
#endif
            // Split cases based upon the id
            switch (idCtl)
            {
                case IDWC_CBFN_FW:
                    Assert (lpdis->CtlType EQ ODT_COMBOBOX);

                    // Split cases based upon the item action
                    switch (lpdis->itemAction)
                    {
                        WCHAR wszTemp[1024];
                        UINT  uLen;

                        case ODA_DRAWENTIRE:
                        case ODA_SELECT:
                            // Get the text of the item to display
                            uLen = (UINT)
                              SendMessageW (hWndCBFN_FW, CB_GETLBTEXT, lpdis->itemID, (LPARAM) &wszTemp);

                            Assert (uLen < countof (wszTemp));

                            // Draw the text
                            DrawTextW (lpdis->hDC,
                                       wszTemp,
                                       uLen,
                                      &lpdis->rcItem,
                                       0
                                     | DT_SINGLELINE
                                     | DT_NOPREFIX
                                      );
                            break;

                        case ODA_FOCUS:     // Ignore changes in focus
                            break;
                    } // End SWITCH

                    break;

                defstop
                    break;
            } // End SWITCH

            break;
#ifndef DEBUG
  #undef  lpdis
  #undef  idCtl
#endif
        } // End WM_DRAWITEM

        case WM_SYSCOLORCHANGE:
        case WM_SETTINGCHANGE:
            // Uninitialize window-specific resources
            FW_RB_Delete (hWnd);

            // Initialize window-specific resources
            FW_RB_Create (hWnd);

            // Get the default font for the Font name ComboBox
            hFontCBFN = GetComboBoxFont (hWnd, hFontCBFN, hWndTT_CBL);

            break;                  // Continue with next handler

        case WM_COMMAND:            // wNotifyCode = HIWORD (wParam); // notification code
                                    // wID = LOWORD (wParam);         // item, control, or accelerator identifier
                                    // hwndCtrl = (HWND) lParam;      // handle of control
        {
#ifdef DEBUG
            UINT idCtl    = GET_WM_COMMAND_ID   (wParam, lParam);
            UINT cmdCtl   = GET_WM_COMMAND_CMD  (wParam, lParam);
            HWND hwndCtrl = GET_WM_COMMAND_HWND (wParam, lParam);
#else
  #define idCtl               GET_WM_COMMAND_ID   (wParam, lParam)
  #define cmdCtl              GET_WM_COMMAND_CMD  (wParam, lParam)
  #define hwndCtl             GET_WM_COMMAND_HWND (wParam, lParam)
#endif
            // Split cases based upon the ID
            switch (idCtl)
            {
                case IDWC_CBFN_FW:
                case IDWC_CBFS_FW:
                    // Split cases based upon the code
                    switch (cmdCtl)
                    {
                        case CBN_SELENDOK:
                            // Handle in common message routine
                            PostMessageW (hWnd, MYWM_NEWFONT, 0, 0);

                            break;

                        default:
                            break;
                    } // End SWITCH

                    break;

                case IDWC_EC_FW:
                    // Split cases based upon the code
                    switch (cmdCtl)
                    {
                        case EN_CHANGE:
                            // Handle in common message routine
                            PostMessageW (hWnd, MYWM_NEWFONT, 0, 0);

                            break;

                        default:
                            break;
                    } // End SWITCH

                    break;

                default:
                    break;
            } // End SWITCH

            return FALSE;           // We handled the msg
#ifndef DEBUG
  #undef  hwndCtl
  #undef  cmdCtl
  #undef  idCtl
#endif
        } // End WM_COMMAND

        case MYWM_NEWFONT:
        {
            APLU3264 uCurSel,           // Index of the current ComboBox selection
                     uStyle;            // Current style

            // Get the index of current selection in the Font Name ComboBox
            uCurSel =
              SendMessageW (hWndCBFN_FW, CB_GETCURSEL, 0, 0);

            // Get the current selection text from the Font Name ComboBox
            SendMessageW (hWndCBFN_FW, CB_GETLBTEXT, uCurSel, (LPARAM) &lfSM.lfFaceName);

            // Get and save the current point size
            cfSM.iPointSize = (int)
              SendMessageW (hWndUD_FW, UDM_GETPOS, 0, 0) * 10;

            // Get the current style
            //   (0 = Regular, 1 = Italic, 2 = Bold, 3 = Bold Italic)
            uStyle =
              SendMessageW (hWndCBFS_FW, CB_GETCURSEL, 0, 0);

            // Save in the LOGFONTW struc
            lfSM.lfWeight = (uStyle >= 2)   ? FW_BOLD : FW_REGULAR;
            lfSM.lfItalic = (uStyle & BIT0) ? TRUE : FALSE;

            // Create and apply the new font
            CreateNewFontSM (TRUE);

            return FALSE;           // We handled the msg
        } // End MYWM_NEWFONT

        case WM_CLOSE:
            DestroyWindow (hWnd);

            return FALSE;           // We handled the msg

        case WM_DESTROY:
            // Fill in the TOOLINFOW size based upon the matching COMCTL32.DLL version #
            ZeroMemory (&tti, sizeof (tti));
            if (fComctl32FileVer >= 6)
                tti.cbSize = sizeof (tti);
            else
                tti.cbSize = TTTOOLINFOW_V2_SIZE;

            // Unregister the Tooltip for the Font Name ComboBox
            tti.hwnd     = hWndCBFN_FW;
            tti.uId      = (UINT_PTR) hWndCBFN_FW;
            SendMessageW (hWndTT,
                          TTM_DELTOOLW,
                          0,
                          (LPARAM) &tti);
            // Unregister the Tooltip for the Font Style ComboBox
            tti.hwnd     = hWndCBFS_FW;
            tti.uId      = (UINT_PTR) hWndCBFS_FW;
            SendMessageW (hWndTT,
                          TTM_DELTOOLW,
                          0,
                          (LPARAM) &tti);
            // Unregister the Tooltip for the Font Size Edit Ctrl
            tti.hwnd     = hWndEC_FW;
            tti.uId      = (UINT_PTR) hWndEC_FW;
            SendMessageW (hWndTT,
                          TTM_DELTOOLW,
                          0,
                          (LPARAM) &tti);
            // Unregister the Tooltip for the Font Size UpDown Ctrl
            tti.hwnd     = hWndUD_FW;
            tti.uId      = (UINT_PTR) hWndUD_FW;
            SendMessageW (hWndTT,
                          TTM_DELTOOLW,
                          0,
                          (LPARAM) &tti);
            // Write out the APL Font Names to the .ini file
            WriteAplFontNames (hWndCBFN_FW);

            // Uninitialize window-specific resources
            FW_RB_Delete (hWnd);

            // If the Tooltip window is still present, ...
            if (hWndTT)
            {
                // Destroy it
                DestroyWindow (hWndTT); hWndTT = NULL;
            } // End IF

            // If the Tooltip window is still present, ...
            if (hWndTT_CBL)
            {
                // Destroy it
                DestroyWindow (hWndTT_CBL); hWndTT_CBL = NULL;
            } // End IF

            // If the Tooltip font is still present, ...
            if (hFontCBFN)
            {
                // We no longer need this resource
                DeleteObject (hFontCBFN); hFontCBFN = NULL;
            } // End IF

            return FALSE;           // We handled the msg

        default:
            break;
    } // End SWITCH

////LCLODSAPI ("FW_RBZ:", hWnd, message, wParam, lParam);
    return DefWindowProcW (hWnd, message, wParam, lParam);
} // End FW_RBWndProc


//***************************************************************************
//  $GetComboBoxFont
//
//  Get the ComboBox font
//***************************************************************************

HFONT GetComboBoxFont
    (HWND  hWnd,                            // ComboBox window handle
     HFONT hFont,                           // Incoming font (may be NULL)
     HWND  hWndTT_CBL)                      // Tooltip window handle for Font Window ComboBox

{
    HDC       hDC;                          // Client DC for font sizing
    LOGFONTW  lf;                           // LOGFONT for font sizing
////NONCLIENTMETRICSW ncMetrics = {sizeof (NONCLIENTMETRICSW)};
    int       iLogPixelsX;               // # horizontal pixels per inch in the DC

    // If the incoming font is valid, ...
    if (hFont)
    {
        // We no longer need this resource
        DeleteObject (hFont); hFont = NULL;
    } // End IF

    // ANSI_FIXED_POINT
    // ANSI_VAR_FONT
    // DEVICE_DEFAULT_FONT
    // DEFAULT_GUI_FONT
    // OEM_FIXED_POINT
    // SYSTEM_FONT
    // SYSTEM_FIXED_POINT
    hFont = GetStockObject (SYSTEM_FONT);

    // Convert the font into a LOGFONT
    GetObjectW (hFont, sizeof (lf), &lf);

    // Get a DC for the window's client area
    hDC = MyGetDC (hWnd);

    // Get the # horizontal pixels per inch in the DC
    iLogPixelsX = GetDeviceCaps (hDC, LOGPIXELSX);

    // We no longer need this resource
    MyReleaseDC (hWnd, hDC); hDC = NULL;

////// Get the non-client metrics for the system including LOGFONTs for default fonts
////SystemParametersInfoW (SPI_GETNONCLIENTMETRICS, ncMetrics.cbSize, &ncMetrics, 0);

    // Copy the LOGFONTW struc
    // (on Windows 7 Pro, these are all Segoe UI, not the font used for a ComboBox)
////lf = ncMetrics.lfCaptionFont  ;
////lf = ncMetrics.lfSmCaptionFont;
////lf = ncMetrics.lfMenuFont     ;
////lf = ncMetrics.lfStatusFont   ;
////lf = ncMetrics.lfMessageFont  ;

    // Scale the width & height accordingly
    lf.lfHeight = MulDiv (lf.lfHeight, iLogPixelsX, USER_DEFAULT_SCREEN_DPI);
    lf.lfWidth  = MulDiv (lf.lfWidth , iLogPixelsX, USER_DEFAULT_SCREEN_DPI);

    // Make a font from the LOGFONTW
    hFont = CreateFontIndirectW (&lf);

    // Tell the Tooltip Ctrl to use it as well
    SendMessageW (hWndTT_CBL, WM_SETFONT, (WPARAM) hFont, MAKELPARAM (TRUE, 0));

    // Tell the ComboBox Ctrl to use it as well
    SendMessageW (cbi.hwndCombo, WM_SETFONT, (WPARAM) hFont, MAKELPARAM (TRUE, 0));

    return hFont;
} // End GetComboBoxFont


//***************************************************************************
//  $LclFontsComboLboxWndProc
//
//  Local font name ComboLbox subclass procedure
//***************************************************************************

LRESULT WINAPI LclFontsComboLboxWndProc
    (HWND   hWnd,                           // Window handle
     UINT   message,                        // Type of message
     WPARAM wParam,                         // Additional information
     LPARAM lParam)                         // ...

{
    static HWND      hWndTT = NULL;         // Tooltip window handle
           RECT      rcHit;                 // This item's bounding rectangle
    static WCHAR     wszTemp[1024];         // Save area for selection text
    static int       iCurSel;               // Index of current selection
    static int       iLstSel = LB_ERR;
           TOOLINFOW tti;                   // For Tooltip Ctrl
    static UBOOL     bTrackMouse = FALSE;   // TRUE iff tracking the mouse
    static TRACKMOUSEEVENT tme = {sizeof (TRACKMOUSEEVENT)};

////LCLODSAPI ("LFLB: ", hWnd, message, wParam, lParam);

    if (hWndTT EQ NULL)
        // Get the Tooltip window handle
        hWndTT = (HWND) GetPropW (hWnd, L"hWndTT");

    Assert (hWndTT NE NULL);
    Assert (hWnd EQ cbi.hwndList);

    // Split cases based upon the message
    switch (message)
    {
        case WM_MOUSEMOVE:          // fwKeys = wParam;        // key flags
                                    // xPos = LOWORD(lParam);  // horizontal position of cursor (CA)
                                    // yPos = HIWORD(lParam);  // vertical position of cursor (CA)
        {
#ifdef DEBUG
            int xPos = LOSHORT (lParam),
                yPos = HISHORT (lParam);
#else
  #define xPos  (LOSHORT (lParam))
  #define yPos  (HISHORT (lParam))
#endif

            // Fill in the TOOLINFOW size based upon the matching COMCTL32.DLL version #
            ZeroMemory (&tti, sizeof (tti));
            if (fComctl32FileVer >= 6)
                tti.cbSize = sizeof (tti);
            else
                tti.cbSize = TTTOOLINFOW_V2_SIZE;

            // Initialize the struct
            tti.hwnd = hWnd;
            tti.uId  = (UINT_PTR) hWnd;

            // Get the current ListBox selection index
            iCurSel = LOSHORT (SendMessageW (hWnd, LB_ITEMFROMPOINT, 0, MAKELONG (xPos, yPos)));

            // If this index is different from the last and we're tracking the mouse, ...
            if (iCurSel NE iLstSel
             && bTrackMouse)
            {
                // We need to toggle the Tooltip tracking state off/on to tell the
                //   Tooltip Ctrl to resend TTN_GETDISPINFOW.

                // Clear tracking and capture
                SendMessageW (hWnd, WM_MOUSELEAVE, 0, 0);
            } // End IF

            // If the index is valid, ...
            if (iCurSel NE LB_ERR)
            {
                // If we're not already tracking the mouse, ...
                if (!bTrackMouse)
                {
                    // Fill in the TOOLINFOW struc
                    tti.uFlags   = 0
                                 | TTF_IDISHWND
                                 | TTF_TRACK
                                 | TTF_ABSOLUTE
                                 ;
////////////////////tti.hwnd     = cbi.hwndList;        // Initialized above
////////////////////tti.uId      = (UINT_PTR) cbi.hwndList; // ...
////////////////////tti.rect     =                      // Not used with TTF_IDISHWND
////////////////////tti.hinst    =                      // Not used except with string resources
////                tti.lpszText = LPSTR_TEXTCALLBACKW;
                    tti.lpszText = L" ";                            // MUST be non-empty
////////////////////tti.lParam   =                      // Not used by this code

                    // Activate the Tooltip Ctrl
                    SendMessageW (hWndTT, TTM_ACTIVATE, TRUE, 0);

                    // Attach the Tooltip window to it
                    SendMessageW (hWndTT, TTM_ADDTOOLW, 0, (LPARAM) &tti);

                    // Fill in the struc
                    tme.hwndTrack = hWnd;
                    tme.dwFlags   = TME_LEAVE;

                    // Ask to be notified when the mouse leaves the ListBox
                    TrackMouseEvent (&tme);

                    // Activate tracking the Tooltip
                    SendMessageW (hWndTT, TTM_TRACKACTIVATE, TRUE, (LPARAM) &tti);

                    // Mark as tracking
                    bTrackMouse = TRUE;
                } // End IF

                // If this index is different from the last, ...
                if (iCurSel NE iLstSel)
                {
                    RECT  rcLbox,                   // Client area of Lbox
                          rcTT = {0, 0, 0, 0};      // Tooltip drawing rectangle
                    HDC   hDC;                      // Client DC for Lbox
                    HFONT hFont,                    // Font handle for TT
                          hFontOld;                 // Old font for DC

                    // Activate the Tooltip Ctrl
                    SendMessageW (hWndTT, TTM_ACTIVATE, TRUE, 0);

                    // Point to the buffer
                    tti.lpszText = wszTemp;

                    // Get the current tooltip info
                    SendMessageW (hWndTT, TTM_GETTOOLINFOW, 0, (LPARAM) &tti);

                    // Save the new last index
                    iLstSel = iCurSel;

                    // Get the selection text
                    SendMessageW (hWnd, LB_GETTEXT, iCurSel, (LPARAM) wszTemp);

                    // Get the width of the ComboLbox client area
                    GetClientRect (hWnd, &rcLbox);

                    // Get a DC for the window's client area
                    hDC = MyGetDC (hWnd);

                    // Get the font for the tooltip
                    hFont = (HFONT)
                      SendMessageW (hWndTT, WM_GETFONT, 0, 0);

                    // Put it into effect
                    hFontOld = SelectObject (hDC, hFont);

                    // Get the length (in pixels) of the tooltip text
                    DrawTextW (hDC,
                               wszTemp,
                               -1,
                              &rcTT,
                               0
                             | DT_CALCRECT
                             | DT_NOPREFIX
                             | DT_SINGLELINE
                               );
                    // Put the old font back into effect
                    SelectObject (hDC, hFontOld);

                    // We no longer need this resource
                    MyReleaseDC (hWnd, hDC); hDC = NULL;

                    // If the tooltip text is longer than the Lbox client area, ...
                    if (rcTT.right > rcLbox.right)
                    {
                        // Tell the Tooltip Ctrl about it
////////////////////////SendMessageW (hWndTT, TTM_UPDATETIPTEXTW, 0, (LPARAM) &tti);
                        SendMessageW (hWndTT, TTM_SETTOOLINFOW, 0, (LPARAM) &tti);

                        // Get the item's bounding rectangle
                        SendMessageW (hWnd, LB_GETITEMRECT, iCurSel, (LPARAM) &rcHit);

                        // Adjust the ListBox rectangle to match the Tooltip Ctrl
                        SendMessageW (hWndTT, TTM_ADJUSTRECT, TRUE, (LPARAM) &rcHit);

                        // Convert coordinates
                        ClientToScreen (hWnd, (LPPOINT) &rcHit);

                        // Tell the Tooltip Ctrl the new position
                        SendMessageW (hWndTT, TTM_TRACKPOSITION, 0, MAKELONG (rcHit.left, rcHit.top));
                    } else
                        // De-activate the Tooltip Ctrl
                        SendMessageW (hWndTT, TTM_ACTIVATE, FALSE, 0);
                } // End IF
            } else
            {
                // The mouse is outside our window
                // Clear tracking and capture
                PostMessageW (hWnd, WM_MOUSELEAVE, 0, 0);

                // Mark as no longer tracking
                bTrackMouse = FALSE;
            } // End IF

            break;
#ifndef DEBUG
  #undef  yPos
  #undef  xPos
#endif
        } // End WM_MOUSEMOVE

        case WM_MOUSELEAVE:         // fwKeys = wParam;        // key flags
                                    // xPos = LOWORD(lParam);  // horizontal position of cursor (CA)
                                    // yPos = HIWORD(lParam);  // vertical position of cursor (CA)
            // The mouse is outside our window and
            //   we no longer need this resource

            // Fill in the TOOLINFOW size based upon the matching COMCTL32.DLL version #
            ZeroMemory (&tti, sizeof (tti));
            if (fComctl32FileVer >= 6)
                tti.cbSize = sizeof (tti);
            else
                tti.cbSize = TTTOOLINFOW_V2_SIZE;

            // Fill in the struc
            tti.hwnd = hWnd;
            tti.uId  = (UINT_PTR) hWnd;

            // Tell the Tooltip Ctrl to stop tracking
            SendMessageW (hWndTT, TTM_TRACKACTIVATE, FALSE, (LPARAM) &tti);

            // Unregister the Tooltip for the Font Name ComboLbox
            SendMessageW (hWndTT,
                          TTM_DELTOOLW,
                          0,
                          (LPARAM) &tti);
            // Fill in the struc
            tme.hwndTrack = hWnd;
            tme.dwFlags   = TME_CANCEL
                          | TME_LEAVE;
            // Cancel tracking
            TrackMouseEvent (&tme);

            // Mark as no longer valid
            iLstSel = LB_ERR;

            // Mark as no longer tracking
            bTrackMouse = FALSE;

            break;

        case WM_NOTIFY:             // idCtrl = (int) wParam;
        {                           // pnmh = (LPNMHDR) lParam;
#ifdef DEBUG
            LPNMHDR         lpnmhdr = (LPNMHDR) lParam;
            LPNMTTDISPINFOW lpnmtdi = (LPNMTTDISPINFOW) lParam;
            WPARAM          idCtl   = wParam;
#else
  #define lpnmhdr         ((LPNMHDR) lParam)
  #define lpnmtdi         ((LPNMTTDISPINFOW) lParam)
  #define idCtl           ((WPARAM) wParam)
#endif
            // Split cases based upon the code
            switch (lpnmhdr->code)
            {
                case TTN_SHOW:
////                // Reposition the Tooltip Ctrl on top of the ComboLbox
////                SetWindowPos (hWndTT,
////                              HWND_TOP,
////                              0, 0, 0, 0,
////                              0
////                            | SWP_NOSIZE
////                            | SWP_NOMOVE
////                            | SWP_NOACTIVATE
////                             );
                    // Reposition the ComboLbox not to be topmost
                    SetWindowPos (hWnd,
                                  HWND_NOTOPMOST,
                                  0, 0, 0, 0,
                                  0
                                | SWP_NOSIZE
                                | SWP_NOMOVE
                                | SWP_NOACTIVATE
                                 );
                    return FALSE;       // Use default positioning
////////////////////return TRUE;        // We positioned the TT

                default:
                    break;
            } // End SWITCH

            break;
#ifndef DEBUG
  #undef  idCtl
  #undef  lpnmtdi
  #undef  lpnmhdr
#endif
        } // End WM_NOTIFY

        default:
            break;
    } // End SWITCH

////LCLODSAPI ("LFLBZ: ", hWnd, message, wParam, lParam);
    return
      CallWindowProcW (lpfnOldFontsComboLboxWndProc,
                       hWnd,
                       message,
                       wParam,
                       lParam);     // Pass on down the line
} // End LclFontsComboLboxWndProc


//***************************************************************************
//  $InitFontBand
//
//  Initialize the Font Band
//***************************************************************************

void InitFontBand
    (APLU3264 uBandPos)         // Insert the band in this position
                                // (-1 = at the end)

{
    REBARBANDINFOW rbBand;      // Rebar band info struc
    RECT           rcCB;        // Rectangle for ComboBox in Font Window
////UINT           uItem;       // Index of this band

    // Get the outside dimensions of the ComboBox in the Font Window
    GetWindowRect (hWndCBFN_FW, &rcCB);

    // Initialize the band info struc
    ZeroMemory (&rbBand, sizeof (rbBand));

    // Specify the attributes of the band
    rbBand.cbSize     = sizeof (REBARBANDINFOW);
    rbBand.fMask      = 0
                      | RBBIM_STYLE     // fStyle is valid
                      | RBBIM_TEXT      // lpText is valid
                      | RBBIM_CHILD     // hwndChild is valid
                      | RBBIM_CHILDSIZE // child size members are valid
                      | RBBIM_IDEALSIZE // cxIdeal is valid
                      | RBBIM_SIZE      // cx is valid
                      | RBBIM_ID        // wID is valid
                      ;
    rbBand.fStyle     = 0
                      | RBBS_CHILDEDGE
                      | RBBS_GRIPPERALWAYS
////                  | RBBS_USECHEVRON
                      ;
////rbBand.clrFore    =
////rbBand.clrBack    =
    rbBand.lpText     = L"SM Font";
////rbBand.cch        =
////rbBand.iImage     =
    rbBand.hwndChild  = hWndFW_RB;
    rbBand.cxMinChild = cxFW_RB;
    rbBand.cyMinChild = rcCB.bottom - rcCB.top;
    rbBand.cxIdeal    = cxFW_RB;
    rbBand.cx         = cxFW_RB;
////rbBand.hbmBack    =
    rbBand.wID        = IDWC_FW_RB;
////rbBand.cyChild    =
////rbBand.cyMaxChild =
////rbBand.cyIntegral =
////rbBand.lParam     =
////rbBand.cxheader   =

    // Insert the Font Window band
    SendMessageW (hWndRB, RB_INSERTBANDW, (WPARAM) uBandPos, (LPARAM) &rbBand);

////// Get the zero-based index of the Font Window band
////uItem = SendMessageW (hWndRB, RB_IDTOINDEX, IDWC_FW_RB, 0);
////
////// Maximize this band to the size of cxIdeal
////SendMessageW (hWndRB, RB_MAXIMIZEBAND, uItem, TRUE);
} // End InitFontBand


//***************************************************************************
//  $MakeLanguageWindow
//
//  Create the Language Window in the Rebar Ctrl
//***************************************************************************

HWND MakeLanguageWindow
    (HWND hWndParent)               // Parent window handle

{
    HWND hWndRes;                   // Result window handle

    hWndRes =
      CreateWindowExW (0,                   // Extended styles
                       LLW_RBWNDCLASS,      // Class
                       wszLW_RBTitle,       // Window title (for debugging purposes only)
                       0                    // Styles
                     | WS_CHILD
                     | WS_CLIPSIBLINGS
                     | WS_CLIPCHILDREN
                     | WS_VISIBLE
/////////////////////| WS_BORDER
                       ,
                       0, 0,                // X- and Y-coord
                       CW_USEDEFAULT,       // X-size
                       CW_USEDEFAULT,       // Y-size
                       hWndParent,          // Parent window
               (HMENU) IDWC_LW_RB,          // Window ID
                       _hInstance,          // Instance
                       NULL);               // No extra data
    if (hWndRes)
        // Show the window
        ShowWindow (hWndRes, SW_SHOWNORMAL);
#ifdef DEBUG
    else
    {
        UINT uErr = GetLastError ();

        DbgBrk ();          // #ifdef DEBUG
    } // End IF/ELSE
#endif

    return hWndRes;
} // End MakeLanguageWindow


//***************************************************************************
//  $LW_RB_Create
//
//  Perform window-specific initialization
//***************************************************************************

void LW_RB_Create
    (HWND hWnd)

{
} // End LW_RB_Create


//***************************************************************************
//  $LW_RB_Delete
//
//  Perform window-specific uninitialization
//***************************************************************************

void LW_RB_Delete
    (HWND hWnd)

{
} // End LW_RB_Delete


//***************************************************************************
//  $LW_RBWndProc
//
//  Message processing routine for the Language Window in Rebar Ctrl window
//***************************************************************************

LRESULT APIENTRY LW_RBWndProc
    (HWND   hWnd,       // Window handle
     UINT   message,    // Type of message
     WPARAM wParam,     // Additional information
     LPARAM lParam)     // ...

{
typedef struct tagLANGCHARS
{
    LPWCHAR lpwc;           // The char to draw (0 = separator)
    RECT    rcHit;          // Hit test rectangle
    LPWCHAR lpwszTitle,     // Ptr to Title text
            lpwszTipText;   // Ptr to Tooltip text
} LANGCHARS, *LPLANGCHARS;

#define LANGCHARS_SEPARATOR     {NULL, {0}},

    static LANGCHARS langChars[] =
    {
        {WS_UTF16_LEFTARROW         , {0}, L"LeftArrow",
                                           L"Sink:     " WS_UTF16_LEFTARROW L"R\n"
                                           L"Assign:  L" WS_UTF16_LEFTARROW L"R\n\n"
                                           L"Keyboard:  Alt-'['\n\n"
                                           L"Unicode:  0x2190 or 8592"},
        {WS_UTF16_RIGHTARROW        , {0}, L"RightArrow",
                                           L"Goto:  " WS_UTF16_RIGHTARROW L"R\n\n"
                                           L"Keyboard:  Alt-']'\n\n"
                                           L"Unicode:  0x2192 or 8594"},
        LANGCHARS_SEPARATOR
        {WS_UTF16_PLUS              , {0}, L"Plus",
                                           L"Conjugate:  +R (when Complex Numbers are implemented)\n"
                                           L"Addition:  L+R  or  L+[X] R\n\n"
                                           L"Keyboard:  '+' or Shift-'='\n\n"
                                           L"Unicode:  0x002B or 43"},
        {WS_UTF16_BAR               , {0}, L"Bar",
                                           L"Negation:      -R\n"
                                           L"Subtraction:  L-R  or  L-[X] R\n\n"
                                           L"Keyboard:  '-'\n\n"
                                           L"Unicode:  0x002D or 45"},
        {WS_UTF16_TIMES             , {0}, L"Times",
                                           L"Signum:           " WS_UTF16_TIMES L"R\n"
                                           L"Multiplication:  L" WS_UTF16_TIMES L"R  or  L" WS_UTF16_TIMES L"[X] R\n\n"
                                           L"Keyboard:  Alt-'-'\n\n"
                                           L"Unicode:  0x00D7 or 215"},
        {WS_UTF16_COLONBAR          , {0}, L"ColonBar",
                                           L"Reciprocal:  " WS_UTF16_COLONBAR L"R\n"
                                           L"Division:   L" WS_UTF16_COLONBAR L"R  or  L" WS_UTF16_COLONBAR L"[X] R\n\n"
                                           L"Keyboard:  Alt-'='\n\n"
                                           L"Unicode:  0x00F7 or 247"},
        {WS_UTF16_STAR              , {0}, L"Star",
                                           L"Power of e:   *R\n"
                                           L"Power of L:  L*R  or  L*[X] R\n\n"
                                           L"Keyboard:  '*' or Shift-'8'\n\n"
                                           L"Unicode:  0x002A or 42"},
        {WS_UTF16_CIRCLESTAR        , {0}, L"CircleStar",
                                           L"Log base e:   " WS_UTF16_CIRCLESTAR L"R\n"
                                           L"Log base L:  L" WS_UTF16_CIRCLESTAR L"R  or  L" WS_UTF16_CIRCLESTAR L"[X] R\n\n"
                                           L"Keyboard:  Alt-'*'\n\n"
                                           L"Unicode:  0x235F or 9055"},
        {WS_UTF16_DOMINO            , {0}, L"Domino",
                                           L"Matrix Inverse:    " WS_UTF16_DOMINO L"R\n"
                                           L"Matrix Division:  L" WS_UTF16_DOMINO L"R\n\n"
                                           L"Keyboard:  Alt-'+' or Alt-Shift-'='\n\n"
                                           L"Unicode:  0x2339 or 9017"},
        {WS_UTF16_CIRCLE            , {0}, L"Circle",
                                           L"Pi Times:            " WS_UTF16_CIRCLE L"R\n"
                                           L"Circle Functions:  " WS_UTF16_OVERBAR L"7" WS_UTF16_CIRCLE L"R " WS_UTF16_LEFTRIGHTARROWS L" atanh (R)\n"
                                           L"                   " WS_UTF16_OVERBAR L"6" WS_UTF16_CIRCLE L"R " WS_UTF16_LEFTRIGHTARROWS L" acosh (R)\n"
                                           L"                   " WS_UTF16_OVERBAR L"5" WS_UTF16_CIRCLE L"R " WS_UTF16_LEFTRIGHTARROWS L" asinh (R)\n"
                                           L"                   " WS_UTF16_OVERBAR L"4" WS_UTF16_CIRCLE L"R " WS_UTF16_LEFTRIGHTARROWS L" R" WS_UTF16_TIMES WS_UTF16_ROOT L"(1-R*" WS_UTF16_OVERBAR L"2)\n"
                                           L"                   " WS_UTF16_OVERBAR L"3" WS_UTF16_CIRCLE L"R " WS_UTF16_LEFTRIGHTARROWS L" atan  (R)\n"
                                           L"                   " WS_UTF16_OVERBAR L"2" WS_UTF16_CIRCLE L"R " WS_UTF16_LEFTRIGHTARROWS L" acos  (R)\n"
                                           L"                   " WS_UTF16_OVERBAR L"1" WS_UTF16_CIRCLE L"R " WS_UTF16_LEFTRIGHTARROWS L" asin  (R)\n"
                                           L"                    0" WS_UTF16_CIRCLE L"R " WS_UTF16_LEFTRIGHTARROWS L" " WS_UTF16_ROOT L"(1-R*2)\n"
                                           L"                    1" WS_UTF16_CIRCLE L"R " WS_UTF16_LEFTRIGHTARROWS L"  sin  (R)\n"
                                           L"                    2" WS_UTF16_CIRCLE L"R " WS_UTF16_LEFTRIGHTARROWS L"  cos  (R)\n"
                                           L"                    3" WS_UTF16_CIRCLE L"R " WS_UTF16_LEFTRIGHTARROWS L"  tan  (R)\n"
                                           L"                    4" WS_UTF16_CIRCLE L"R " WS_UTF16_LEFTRIGHTARROWS L" " WS_UTF16_ROOT L"(1+R*2)\n"
                                           L"                    5" WS_UTF16_CIRCLE L"R " WS_UTF16_LEFTRIGHTARROWS L"  sinh (R)\n"
                                           L"                    6" WS_UTF16_CIRCLE L"R " WS_UTF16_LEFTRIGHTARROWS L"  cosh (R)\n"
                                           L"                    7" WS_UTF16_CIRCLE L"R " WS_UTF16_LEFTRIGHTARROWS L"  tanh (R)\n"
                                           L"Also accepts axis operator as in L" WS_UTF16_CIRCLE L"[X] R\n\n"
                                           L"Keyboard:  Alt-'o'\n\n"
                                           L"Unicode:  0x25CB or 9675"},
        {WS_UTF16_QUOTEDOT          , {0}, L"QuoteDot",
                                           L"Factorial:      !R\n"
                                           L"Combinations:  L!R  or  L![X] R\n\n"
                                           L"Keyboard:  '!' or Shift-'1'\n\n"
                                           L"Unicode:  0x0021 or 33"},
        {WS_UTF16_QUERY             , {0}, L"Query",
                                           L"Roll:   ?R\n"
                                           L"Deal:  L?R\n\n"
                                           L"Keyboard:  '?' or Shift-'/' or Alt-'q'\n\n"
                                           L"Unicode:  0x003F or 63"},
        {WS_UTF16_ROOT              , {0}, L"Root",
                                           L"Square Root:  " WS_UTF16_ROOT L"R\n"
                                           L"Nth Root:    L" WS_UTF16_ROOT L"R\n\n"
                                           L"Keyboard:  Alt-'R' or Alt-Shift-'r'\n\n"
                                           L"Unicode:  0x221A or 8730"},
        LANGCHARS_SEPARATOR
        {WS_UTF16_STILE             , {0}, L"Stile",
                                           L"Magnitude:  |R\n"
                                           L"Modulus:   L|R  or  L|[X] R\n\n"
                                           L"Keyboard:  '|' or Shift-'\\'\n\n"
                                           L"Unicode:  0x2223 or 8739"},
        {WS_UTF16_UPSTILE           , {0}, L"UpStile",
                                           L"Ceiling:   " WS_UTF16_UPSTILE L"R\n"
                                           L"Maximum:  L" WS_UTF16_UPSTILE L"R  or  L" WS_UTF16_UPSTILE L"[X] R\n\n"
                                           L"Keyboard:  Alt-'s'\n\n"
                                           L"Unicode:  0x2308 or 8968"},
        {WS_UTF16_DOWNSTILE         , {0}, L"DownStile",
                                           L"Floor:     " WS_UTF16_DOWNSTILE L"R\n"
                                           L"Minimum:  L" WS_UTF16_DOWNSTILE L"R  or  L" WS_UTF16_DOWNSTILE L"[X] R\n\n"
                                           L"Keyboard:  Alt-'d'\n\n"
                                           L"Unicode:  0x230A or 8970"},
        {WS_UTF16_UPTACK            , {0}, L"UpTack",
                                           L"Base value:  L" WS_UTF16_UPTACK L"R\n\n"
                                           L"Keyboard:  Alt-'b'\n\n"
                                           L"Unicode:  0x22A5 or 8869"},
        {WS_UTF16_DOWNTACK          , {0}, L"DownTack",
                                           L"Type:             " WS_UTF16_DOWNTACK L"R\n"
                                           L"Representation:  L" WS_UTF16_DOWNTACK L"R\n\n"
                                           L"Keyboard:  Alt-'n'\n\n"
                                           L"Unicode:  0x22A4 or 8868"},
        {WS_UTF16_LEFTTACK          , {0}, L"LeftTack",
                                           L"Identity:  " WS_UTF16_LEFTTACK L"R\n"
                                           L"Lev:      L" WS_UTF16_LEFTTACK L"R\n\n"
                                           L"Keyboard:  Alt-'|' or Alt-Shift-'\\'\n\n"
                                           L"Unicode:  0x22A3 or 8867"},
        {WS_UTF16_RIGHTTACK         , {0}, L"RightTack",
                                           L"Identity:  " WS_UTF16_RIGHTTACK L"R\n"
                                           L"Dex:      L" WS_UTF16_RIGHTTACK L"R\n\n"
                                           L"Keyboard:  Alt-'\\'\n\n"
                                           L"Unicode:  0x22A2 or 8866"},
        LANGCHARS_SEPARATOR
        {WS_UTF16_EQUAL             , {0}, L"Equal",
                                           L"Equal To:  L=R  or  L=[X] R\n\n"
                                           L"Keyboard:  '=' or Alt-'5'\n\n"
                                           L"Unicode:  0x003D or 61"},
        {WS_UTF16_NOTEQUAL          , {0}, L"NotEqual",
                                           L"Not Equal To:  L" WS_UTF16_NOTEQUAL L"R  or  L" WS_UTF16_NOTEQUAL L"[X] R\n\n"
                                           L"Keyboard:  Alt-'8'\n\n"
                                           L"Unicode:  0x2260 or 8800"},
        {WS_UTF16_LEFTCARETUNDERBAR , {0}, L"LeftCaretUnderbar",
                                           L"Less Than or Equal To:  L" WS_UTF16_LEFTCARETUNDERBAR L"R  or  L" WS_UTF16_LEFTCARETUNDERBAR L"[X] R\n\n"
                                           L"Keyboard:  Alt-'4'\n\n"
                                           L"Unicode:  0x2264 or 8804"},
        {WS_UTF16_LEFTCARET         , {0}, L"LeftCaret",
                                           L"Less Than:  L" WS_UTF16_LEFTCARET L"R  or  L" WS_UTF16_LEFTCARET L"[X] R\n\n"
                                           L"Keyboard:  '<' or Shift-',' or Alt-'6'\n\n"
                                           L"Unicode:  0x003C or 60"},
        {WS_UTF16_RIGHTCARET        , {0}, L"RightCaret",
                                           L"Greater Than:  L" WS_UTF16_RIGHTCARET L"R  or  L" WS_UTF16_RIGHTCARET L"[X] R\n\n"
                                           L"Keyboard:  '>' or Shift-'.' or Alt-'7'\n\n"
                                           L"Unicode:  0x003E or 62"},
        {WS_UTF16_RIGHTCARETUNDERBAR, {0}, L"RightCaretUnderbar",
                                           L"Greater Than or Equal To:  L" WS_UTF16_RIGHTCARETUNDERBAR L"R  or  L" WS_UTF16_RIGHTCARETUNDERBAR L"[X] R\n\n"
                                           L"Keyboard:  Alt-'6'\n\n"
                                           L"Unicode:  0x2265 or 8805"},
        {WS_UTF16_EQUALUNDERBAR     , {0}, L"EqualUnderbar",
                                           L"Depth:                   " WS_UTF16_EQUALUNDERBAR L"R\n"
                                           L"Match:                  L" WS_UTF16_EQUALUNDERBAR L"R\n"
                                           L"Multiset (Identical):   L" WS_UTF16_EQUALUNDERBAR WS_UTF16_DOWNSHOESTILE L"R\n\n"
                                           L"Keyboard:  Alt-'!'  or  Alt-Shift-'1'\n\n"
                                           L"Unicode:  0x2261 or 8801"},
        {WS_UTF16_NOTEQUALUNDERBAR  , {0}, L"NotEqualUnderbar",
                                           L"Tally:                       " WS_UTF16_NOTEQUALUNDERBAR L"R\n"
                                           L"Mismatch:                   L" WS_UTF16_NOTEQUALUNDERBAR L"R\n"
                                           L"Multiset (Not Identical):   L" WS_UTF16_NOTEQUALUNDERBAR WS_UTF16_DOWNSHOESTILE L"R\n\n"
                                           L"Keyboard:  Alt-'@'  or  Alt-Shift-'2'\n\n"
                                           L"Unicode:  0x2262 or 8802"},
        LANGCHARS_SEPARATOR
        {WS_UTF16_DOWNCARET         , {0}, L"DownCaret",
                                           L"Or (Booleans):  L" WS_UTF16_DOWNCARET L"R  or  L" WS_UTF16_DOWNCARET L"[X] R\n"
                                           L"GCD (Integer/Floats):\n\n"
                                           L"Keyboard:  Alt-'9'\n\n"
                                           L"Unicode:  0x2228 or 8744"},
        {WS_UTF16_UPCARET           , {0}, L"UpCaret",
                                           L"And (Booleans):  L" WS_UTF16_UPCARET L"R  or  L" WS_UTF16_UPCARET L"[X] R\n"
                                           L"LCM (Integer/Floats):\n\n"
                                           L"Keyboard:  Alt-'0'\n\n"
                                           L"Unicode:  0x2227 or 8743"},
        {WS_UTF16_DOWNCARETTILDE    , {0}, L"DownCaretTilde",
                                           L"Nor:  L" WS_UTF16_DOWNCARETTILDE L"R  or  L" WS_UTF16_DOWNCARETTILDE L"[X] R\n\n"
                                           L"Keyboard:  Alt-'(' or Alt-Shift-'9'\n\n"
                                           L"Unicode:  0x2371 or 9073"},
        {WS_UTF16_UPCARETTILDE      , {0}, L"UpCaretTilde",
                                           L"Nand:  L" WS_UTF16_UPCARETTILDE L"R  or  L" WS_UTF16_UPCARETTILDE L"[X] R\n\n"
                                           L"Keyboard:  Alt-')' or Alt-Shift-'0'\n\n"
                                           L"Unicode:  0x2372 or 9074"},
        LANGCHARS_SEPARATOR
        {WS_UTF16_UPARROW           , {0}, L"UpArrow",
                                           L"First:  " WS_UTF16_UPARROW L"R\n"
                                           L"Take:  L" WS_UTF16_UPARROW L"R  or  L" WS_UTF16_UPARROW L"[X] R\n\n"
                                           L"Keyboard:  Alt-'y'\n\n"
                                           L"Unicode:  0x2191 or 8593"},
        {WS_UTF16_DOWNARROW         , {0}, L"DownArrow",
                                           L"Drop:  L" WS_UTF16_DOWNARROW L"R  or  L" WS_UTF16_DOWNARROW L"[X] R\n\n"
                                           L"Keyboard:  Alt-'u'\n\n"
                                           L"Unicode:  0x2193 or 8595"},
        {WS_UTF16_LEFTSHOE          , {0}, L"LeftShoe",
                                           L"Enclose:               " WS_UTF16_LEFTSHOE L"R  or   " WS_UTF16_LEFTSHOE L"[X] R\n"
                                           L"Partitioned Enclose:  L" WS_UTF16_LEFTSHOE L"R  or  L" WS_UTF16_LEFTSHOE L"[X] R\n\n"
                                           L"Keyboard:  Alt-'z'\n\n"
                                           L"Unicode:  0x2282 or 8834"},
        {WS_UTF16_RIGHTSHOE         , {0}, L"RightShoe",
                                           L"Disclose:  " WS_UTF16_RIGHTSHOE L"R  or  " WS_UTF16_RIGHTSHOE L"[X] R\n"
                                           L"Pick:     L" WS_UTF16_RIGHTSHOE L"R\n\n"
                                           L"Keyboard:  Alt-'x'\n\n"
                                           L"Unicode:  0x2283 or 8835"},
        {WS_UTF16_SQUAD             , {0}, L"Squad",
                                           L"Index:  L" WS_UTF16_SQUAD L"R  or  L" WS_UTF16_SQUAD L"[X] R\n\n"
                                           L"Keyboard:  Alt-'L' or Alt-Shift-'l'\n\n"
                                           L"Unicode:  0x2337 or 9015"},
        {WS_UTF16_DELTASTILE        , {0}, L"DeltaStile",
                                           L"Grade Up:            " WS_UTF16_DELTASTILE L"R\n"
                                           L"Grade Up Collated:  L" WS_UTF16_DELTASTILE L"R\n\n"
                                           L"Keyboard:  Alt-'$' or Alt-Shift-'4'\n\n"
                                           L"Unicode:  0x234B or 9035"},
        {WS_UTF16_DELSTILE          , {0}, L"DelStile",
                                           L"Grade Down:            " WS_UTF16_DELSTILE L"R\n"
                                           L"Grade Down Collated:  L" WS_UTF16_DELSTILE L"R\n\n"
                                           L"Keyboard:  Alt-'#' or Alt-Shift-'3'\n\n"
                                           L"Unicode:  0x2352 or 9042"},
        LANGCHARS_SEPARATOR
        {WS_UTF16_IOTA              , {0}, L"Iota",
                                           L"Index Generator:       " WS_UTF16_IOTA L"R\n"
                                           L"Index Of:             L" WS_UTF16_IOTA L"R\n"
                                           L"Multiset (Index Of):  L" WS_UTF16_IOTA WS_UTF16_DOWNSHOESTILE L"R\n\n"
                                           L"Keyboard:  Alt-'i'\n\n"
                                           L"Unicode:  0x2373 or 9075"},
        {WS_UTF16_EPSILON           , {0}, L"Epsilon",
                                           L"Enlist:                 " WS_UTF16_EPSILON L"R\n"
                                           L"Member Of:             L" WS_UTF16_EPSILON L"R\n"
                                           L"Multiset (Member Of):  L" WS_UTF16_EPSILON WS_UTF16_DOWNSHOESTILE L"R\n\n"
                                           L"Keyboard:  Alt-'e'\n\n"
                                           L"Unicode:  0x220A or 8714"},
        {WS_UTF16_IOTAUNDERBAR      , {0}, L"IotaUnderbar",
                                           L"ArrayLookup:  L" WS_UTF16_IOTAUNDERBAR L"R\n"
                                           L"Indices:       " WS_UTF16_IOTAUNDERBAR L"R\n\n"
                                           L"Keyboard:  Alt-'I' or Alt-Shift-'i'\n\n"
                                           L"Unicode:  0x2378 or 9080"},
        {WS_UTF16_EPSILONUNDERBAR   , {0}, L"EpsilonUnderbar",
                                           L"Find:  L" WS_UTF16_EPSILONUNDERBAR L"R\n\n"
                                           L"Keyboard:  Alt-'E' or Alt-Shift-'e'\n\n"
                                           L"Unicode:  0x2377 or 9079"},
        {WS_UTF16_DOWNSHOE          , {0}, L"DownShoe",
                                           L"Unique:             " WS_UTF16_DOWNSHOE L"R\n"
                                           L"Union:             L" WS_UTF16_DOWNSHOE L"R\n"
                                           L"Multiset (Union):  L" WS_UTF16_DOWNSHOE WS_UTF16_DOWNSHOESTILE L"R\n\n"
                                           L"Keyboard:  Alt-'v'\n\n"
                                           L"Unicode:  0x222A or 8746"},
        {WS_UTF16_UPSHOE            , {0}, L"UpShoe",
                                           L"Intersection:             L" WS_UTF16_UPSHOE L"R\n"
                                           L"Multiset (Intersection):  L" WS_UTF16_UPSHOE WS_UTF16_DOWNSHOESTILE L"R\n\n"
                                           L"Keyboard:  Alt-'c'\n\n"
                                           L"Unicode:  0x2229 or 8745"},
        {WS_UTF16_LEFTSHOEUNDERBAR  , {0}, L"LeftShoeUnderbar",
                                           L"Subset:             L" WS_UTF16_LEFTSHOEUNDERBAR L"R\n"
                                           L"Multiset (Subset):  L" WS_UTF16_LEFTSHOEUNDERBAR WS_UTF16_DOWNSHOESTILE L"R\n\n"
                                           L"Keyboard:  Alt-'Z'\n\n"
                                           L"Unicode:  0x2286 or 8838"},
        {WS_UTF16_RIGHTSHOEUNDERBAR , {0}, L"RightShoeUnderbar",
                                           L"Superset:             L" WS_UTF16_RIGHTSHOEUNDERBAR L"R\n"
                                           L"Multiset (Superset):  L" WS_UTF16_RIGHTSHOEUNDERBAR WS_UTF16_DOWNSHOESTILE L"R\n\n"
                                           L"Keyboard:  Alt-'X'\n\n"
                                           L"Unicode:  0x2287 or 8839"},
        {WS_UTF16_TILDE             , {0}, L"Tilde",
                                           L"Not:                                " WS_UTF16_TILDE L"R\n"
                                           L"Without:                           L" WS_UTF16_TILDE L"R\n"
                                           L"Multiset (Asymmetric Difference):  L" WS_UTF16_TILDE WS_UTF16_DOWNSHOESTILE L"R\n\n"
                                           L"Keyboard:  '" WS_UTF16_TILDE L"' or Shift-'`' or Alt-'t'\n\n"
                                           L"Unicode:  0x223C or 8764"},
        {WS_UTF16_SECTION           , {0}, L"Section",
                                           L"Symmetric Difference:             L" WS_UTF16_SECTION L"R\n"
                                           L"Multiset (Symmetric Difference):  L" WS_UTF16_SECTION WS_UTF16_DOWNSHOESTILE L"R\n\n"
                                           L"Keyboard:  Alt-'S' or Alt-Shift-'s'\n\n"
                                           L"Unicode:  0x00A7 or 167"},
        {WS_UTF16_PI                , {0}, L"Pi",
                                           L"Prime Factors:            " WS_UTF16_PI L"R\n"
                                           L"Rth Prime:              " WS_UTF16_OVERBAR L"2" WS_UTF16_PI L"R\n"
                                           L"Previous Prime:         " WS_UTF16_OVERBAR L"1" WS_UTF16_PI L"R\n"
                                           L"Primality Test:          0" WS_UTF16_PI L"R\n"
                                           L"Next Prime:              1" WS_UTF16_PI L"R\n"
                                           L"Number of Primes " WS_UTF16_LEFTCARETUNDERBAR L"R:     2" WS_UTF16_PI L"R\n"
                                           L"Divisor Count Function: 10" WS_UTF16_PI L"R\n"
                                           L"Divisor Sum Function:   11" WS_UTF16_PI L"R\n"
                                           L"M" L"\x00F6" L"bius Function:        12" WS_UTF16_PI L"R\n"
                                           L"Totient Function:       13" WS_UTF16_PI L"R\n\n"
                                           L"Keyboard:  Alt-'p'\n\n"
                                           L"Unicode:  0x03C0 or 960"},
        {L".."                      , {0}, L"Dot Dot",
                                           L"Sequence:  L..R\n\n"
                                           L"Keyboard:  '..'\n\n"
                                           L"Unicode:  0x002E 0x002E or 46 46"},
        LANGCHARS_SEPARATOR
        {WS_UTF16_COMMA             , {0}, L"Comma",
                                           L"Ravel:      ,R  or   ,[X] R\n"
                                           L"Catenate:  L,R  or  L,[X] R  where X is integral\n"
                                           L"Laminate:           L,[X] R  where X is fractional\n\n"
                                           L"Keyboard:  ','\n\n"
                                           L"Unicode:  0x002C or 44"},
        {WS_UTF16_COMMABAR          , {0}, L"CommaBar",
                                           L"Table:                 " WS_UTF16_COMMABAR L"R\n"
                                           L"Catenate First Axis:  L" WS_UTF16_COMMABAR L"R  or  L" WS_UTF16_COMMABAR L"[X] R\n\n"
                                           L"Keyboard:  Alt-'" WS_UTF16_TILDE L"' or Alt-Shift-'`'\n\n"
                                           L"Unicode:  0x236A or 9066"},
        {WS_UTF16_RHO               , {0}, L"Rho",
                                           L"Shape:     " WS_UTF16_RHO L"R\n"
                                           L"Reshape:  L" WS_UTF16_RHO L"R\n\n"
                                           L"Keyboard:  Alt-'r'\n\n"
                                           L"Unicode:  0x2374 or 9076"},
        {WS_UTF16_CIRCLESTILE       , {0}, L"CircleStile",
                                           L"Reverse:   " WS_UTF16_CIRCLESTILE L"R  or   " WS_UTF16_CIRCLESTILE L"[X] R\n"
                                           L"Rotate:   L" WS_UTF16_CIRCLESTILE L"R  or  L" WS_UTF16_CIRCLESTILE L"[X] R\n\n"
                                           L"Keyboard:  Alt-'%' or Alt-Shift-'5'\n\n"
                                           L"Unicode:  0x233D or 9021"},
        {WS_UTF16_CIRCLEBAR         , {0}, L"CircleBar",
                                           L"Reverse First Axis:  " WS_UTF16_CIRCLEBAR L"R  or   " WS_UTF16_CIRCLEBAR L"[X] R\n"
                                           L"Rotate First Axis:  L" WS_UTF16_CIRCLEBAR L"R  or  L" WS_UTF16_CIRCLEBAR L"[X] R\n\n"
                                           L"Keyboard:  Alt-'&' or Alt-Shift-'7'\n\n"
                                           L"Unicode:  0x2296 or 8854"},
        {WS_UTF16_CIRCLESLOPE       , {0}, L"CircleSlope",
                                           L"Reverse All Coords:  " WS_UTF16_CIRCLESLOPE L"R\n"
                                           L"Transpose:          L" WS_UTF16_CIRCLESLOPE L"R\n\n"
                                           L"Keyboard:  Alt-'^' or Alt-Shift-'6'\n\n"
                                           L"Unicode:  0x2349 or 9033"},
        LANGCHARS_SEPARATOR
        {WS_UTF16_SLASH             , {0}, L"Slash",
                                           L"Replicate:                 L/R  or    L/[X] R\n"
                                           L"Reduce Operator:           f/R  or    f/[X] R\n"
                                           L"N-wise Reduce Operator:  L f/R  or  L f/[X] R\n\n"
                                           L"Keyboard:  '/'\n\n"
                                           L"Unicode:  0x002F or 47"},
        {WS_UTF16_SLOPE             , {0}, L"Slope",
                                           L"Expand:         L\\R  or  L\\[X] R\n"
                                           L"Scan Operator:  f\\R  or  f\\[X] R\n\n"
                                           L"Keyboard:  '\\'\n\n"
                                           L"Unicode:  0x005C or 92"},
        {WS_UTF16_SLASHBAR          , {0}, L"SlashBar",
                                           L"Replicate First Axis:               L" WS_UTF16_SLASHBAR L"R  or    L" WS_UTF16_SLASHBAR L"[X] R\n"
                                           L"Reduce Operator First Axis:         f" WS_UTF16_SLASHBAR L"R  or    f" WS_UTF16_SLASHBAR L"[X] R\n"
                                           L"N-wise Reduce Operator First Axis:  f" WS_UTF16_SLASHBAR L"R  or  L f" WS_UTF16_SLASHBAR L"[X] R\n\n"
                                           L"Keyboard:  Alt-'/'\n\n"
                                           L"Unicode:  0x233F or 9023"},
        {WS_UTF16_SLOPEBAR          , {0}, L"SlopeBar",
                                           L"Expand First Axis:         L" WS_UTF16_SLOPEBAR L"R  or  L" WS_UTF16_SLOPEBAR L"[X] R\n"
                                           L"Scan Operator First Axis:  f" WS_UTF16_SLOPEBAR L"R  or  f" WS_UTF16_SLOPEBAR L"[X] R\n\n"
                                           L"Keyboard:  Alt-'.'\n\n"
                                           L"Unicode:  0x2340 or 9024"},
        {WS_UTF16_CIRCLEMIDDLEDOT   , {0}, L"CircleMiddleDot",
                                           L"Null Operator:    f" WS_UTF16_CIRCLEMIDDLEDOT L" R\n"
                                           L"                L f" WS_UTF16_CIRCLEMIDDLEDOT L" R\n\n"
                                           L"Keyboard:  Alt-'?' or Alt-Shift-'/'\n\n"
                                           L"Unicode:  0x2299 or 8857"},
        {WS_UTF16_DIERESIS          , {0}, L"Dieresis",
                                           L"Each Operator:  f" WS_UTF16_DIERESIS L" R\n"
                                           L"              L f" WS_UTF16_DIERESIS L" R\n"
                                           L"              L f" WS_UTF16_DIERESIS L"[X] R\n\n"
                                           L"Keyboard:  Alt-'1'\n\n"
                                           L"Unicode:  0x00A8 or 168"},
        {WS_UTF16_DIERESISTILDE     , {0}, L"DieresisTilde",
                                           L"Duplicate Operator:  f" WS_UTF16_DIERESISTILDE L" R\n"
                                           L"Commute Operator:  L f" WS_UTF16_DIERESISTILDE L" R\n\n"
                                           L"Keyboard:  Alt-'T' or Alt-Shift-'t'\n\n"
                                           L"Unicode:  0x2368 or 9064"},
        {WS_UTF16_DIERESISJOT       , {0}, L"DieresisJot",
                                           L"Rank Operator:  (f" WS_UTF16_DIERESISJOT L"Y) R\n"
                                           L"                (f" WS_UTF16_DIERESISJOT L"[X] Y) R\n"
                                           L"              L (f" WS_UTF16_DIERESISJOT L"Y) R\n"
                                           L"              L (f" WS_UTF16_DIERESISJOT L"[X] Y) R\n\n"
                                           L"Keyboard:  Alt-'J' or Alt-Shift-'j'\n\n"
                                           L"Unicode:  0x2364 or 9060"},
////////{WS_UTF16_DIERESISSTAR      , {0}, L""},       // Uncomment when implemented
////////{WS_UTF16_DIERESISDOT       , {0}, L""},       // ...
////////{WS_UTF16_DIERESISDEL       , {0}, L""},       // ...
        {WS_UTF16_DIERESISDOWNTACK  , {0}, L"DieresisDownTack",
                                           L"Convolution Operator:  L f" WS_UTF16_DIERESISDOWNTACK L"g R\n\n"
                                           L"Keyboard:  Alt-'N' or Alt-shift-'n'\n\n"
                                           L"Unicode:  0x2361 or 9057"},
        {WS_UTF16_DIERESISCIRCLE    , {0}, L"DieresisCircle",
                                           L"Composition Operator:  L f" WS_UTF16_DIERESISCIRCLE L"g R\n\n"
                                           L"Keyboard:  Alt-'O' or Alt-shift-'o'\n\n"
                                           L"Unicode:  0x2365 or 9061"},
        {WS_UTF16_DOWNSHOESTILE     , {0}, L"Multiset",
                                           L"Multiset Operator:  f" WS_UTF16_DOWNSHOESTILE L" R\n"
                                           L"                  L f" WS_UTF16_DOWNSHOESTILE L" R\n"
                                           L"Keyboard:  Alt-'m'\n\n"
                                           L"Unicode:  0x2366 or 9062"},
        {WS_UTF16_DOT               , {0}, L"Dot",
                                           L"Inner Product Operator:  L f.g R\n"
                                           L"Determinant Operator:      f.g R\n"
                                           L"Outer Product Operator:  L " WS_UTF16_JOT L".g R\n"
                                           L"Sequence:  L..R\n"
                                           L"Decimal notation separator:  1.23\n\n"
                                           L"Keyboard:  '.'\n\n"
                                           L"Unicode:  0x002E or 46"},
        {WS_UTF16_JOT               , {0}, L"Jot",
                                           L"Compose Operator:  f" WS_UTF16_JOT L"g  R\n"
                                           L"                  (a" WS_UTF16_JOT L"g) R\n"
                                           L"                  (f" WS_UTF16_JOT L"b) R\n"
                                           L"                 L f" WS_UTF16_JOT L"g  R\n\n"
                                           L"Keyboard:  Alt-'j'\n\n"
                                           L"Unicode:  0x2218 or 8728"},
        {WS_UTF16_VARIANT           , {0}, L"QuadColon",
                                           L"Variant Operator:  (f" WS_UTF16_VARIANT L"b) R\n"
                                           L"                 L (f" WS_UTF16_VARIANT L"b) R\n\n"
                                           L"Keyboard:  Alt-'_'\n\n"
                                           L"Unicode:  0x2360 or 9056"},
        LANGCHARS_SEPARATOR
        {WS_UTF16_QUOTEQUAD         , {0}, L"QuoteQuad",
                                           L"Character input from the keyboard:  A" WS_UTF16_LEFTARROW WS_UTF16_QUOTEQUAD L"\n"
                                           L"Character output to the screen:     "  WS_UTF16_QUOTEQUAD WS_UTF16_LEFTARROW L"A\n\n"
                                           L"Keyboard:  Alt-'{' or Alt-Shift-'['\n\n"
                                           L"Unicode:  0x235E or 9054"},
        {WS_UTF16_QUAD              , {0}, L"Quad",
                                           L"Used as the first character in system names\n\n"
                                           L"Evaluated Input:  A" WS_UTF16_LEFTARROW WS_UTF16_QUAD L"\n"
                                           L"Evaluated Output: " WS_UTF16_QUAD WS_UTF16_LEFTARROW L"A\n\n"
                                           L"Keyboard:  Alt-'l'\n\n"
                                           L"Unicode:  0x2395 or 9109"},
        {WS_UTF16_UPTACKJOT         , {0}, L"UpTackJot",
                                           L"Execute:  " WS_UTF16_UPTACKJOT L"R\n\n"
                                           L"Keyboard:  Alt-';'\n\n"
                                           L"Unicode:  0x234E or 9038"},
        {WS_UTF16_DOWNTACKJOT       , {0}, L"DownTackJot",
                                           L"Format:                    " WS_UTF16_DOWNTACKJOT L"R\n"
                                           L"Format By Specification:  L" WS_UTF16_DOWNTACKJOT L"R\n\n"
                                           L"Keyboard:  Alt-\"'\"\n\n"
                                           L"Unicode:  0x2355 or 9045"},
        LANGCHARS_SEPARATOR
        {WS_UTF16_DIAMOND           , {0}, L"Diamond",
                                           L"Used as a statement separator\n\n"
                                           L"stmt1 " WS_UTF16_DIAMOND L" stmt2 " WS_UTF16_DIAMOND L" stmt3 " WS_UTF16_DIAMOND L" ...\n\n"
                                           L"Keyboard:  Alt-'`'\n\n"
                                           L"Unicode:  0x22C4 or 8900"},
        {WS_UTF16_LAMP              , {0}, L"Lamp",
                                           L"Used as the start of a comment\n\n"
                                           L"expression " WS_UTF16_LAMP L" Comment which is not evaluated\n\n"
                                           L"Keyboard:  Alt-','\n\n"
                                           L"Unicode:  0x235D or 9053"},
        {WS_UTF16_DELTA             , {0}, L"Delta",
                                           L"Used as the first or subsequent character in names\n\n"
                                           L"Keyboard:  Alt-'h'\n\n"
                                           L"Unicode:  0x2206 or 8710"},
        {WS_UTF16_DELTAUNDERBAR     , {0}, L"DeltaUnderbar",
                                           L"Used as the first or subsequent character in names\n\n"
                                           L"Keyboard:  Alt-'H' or Alt-Shift-'h'\n\n"
                                           L"Unicode:  0x2359 or 9049"},
        {WS_UTF16_UNDERBAR          , {0}, L"Underbar",
                                           L"Used as the first or subsequent character in names such as _type\n\n"
                                           L"Keyboard:  '_' or Shift-'-'\n\n"
                                           L"Unicode:  0x005F or 95"},
        LANGCHARS_SEPARATOR
        {WS_UTF16_ALPHA             , {0}, L"Alpha",
                                           L"Used as an identifier name outside anonymous functions/operators"
                                           L" and as the identifier name for the (optional) left argument inside"
                                           L" an anonymous function/operator\n\n"
                                           L"Keyboard:  Alt-'a'\n\n"
                                           L"Unicode:  0x237A or 9082"},
        {WS_UTF16_DEL               , {0}, L"Del",
                                           L"Used to open function definition mode in immediate execution mode"
                                           L" and to refer to the enclosing function inside a user or"
                                           L" anonymous function/operator\n\n"
                                           WS_UTF16_DEL L"foo\n\n"
                                           L"Keyboard:  Alt-'g'\n\n"
                                           L"Unicode:  0x2207 or 8711"},
        {WS_UTF16_OMEGA             , {0}, L"Omega",
                                           L"Used as an identifier name outside anonymous functions/operators"
                                           L" and as the identifier name for the right argument inside"
                                           L" an anonymous function/operator\n\n"
                                           L"Keyboard:  Alt-'w'\n\n"
                                           L"Unicode:  0x2375 or 9077"},
        {WS_UTF16_LFTOPER           , {0}, L"Alpha Alpha",
                                           L"Used as an identifier name for the left operand inside"
                                           L" an anonymous operator\n\n"
                                           L"Keyboard:  Alt-'a' Alt-'a'\n\n"
                                           L"Unicode:  0x237A 0x237A or 9082 9082"},
        {WS_UTF16_DELDEL            , {0}, L"Del Del",
                                           L"Used as the identifier name for the operator inside"
                                           L" an anonymous operator\n\n"
                                           WS_UTF16_DEL L"foo\n\n"
                                           L"Keyboard:  Alt-'g'\n\n"
                                           L"Unicode:  0x2207 or 8711"},
        {WS_UTF16_RHTOPER           , {0}, L"Omega Omega",
                                           L"Used as an identifier name for the right operand inside\n"
                                           L"  an anonymous operator\n\n"
                                           L"Keyboard:  Alt-'w' Alt-'w'\n\n"
                                           L"Unicode:  0x2375 0x2375 or 9077 9077"},
        LANGCHARS_SEPARATOR
        {WS_UTF16_OVERBAR           , {0}, L"Overbar",
                                           L"When it immediately precedes a number, it indicates that the number is negative.  "
                                           L"It also may be used as the second or subsequent character in names such as a" WS_UTF16_OVERBAR L"3.\n\n"
                                           L"Keyboard:  Alt-'2'\n\n"
                                           L"Unicode:  0x00AF or 175"},
        {WS_UTF16_ZILDE             , {0}, L"Zilde",
                                           L"Empty numeric vector as opposed to ''\n"
                                           L"which is an empty character vector\n\n"
                                           L"Keyboard:  Alt-'}' or Alt-Shift-']'\n\n"
                                           L"Unicode:  0x236C or 9068"},
        {WS_UTF16_INFINITY          , {0}, L"Infinity",
                                           L" "             WS_UTF16_INFINITY L" is the largest (floating point) number\n"
                                           WS_UTF16_OVERBAR WS_UTF16_INFINITY L" is the smallest (floating point) number\n"
                                           WS_UTF16_INFINITY L"r1 is an infinite rational number\n\n"
                                           L"Keyboard:  Alt-'f'\n\n"
                                           L"Unicode:  0x221E or 8734"},
        LANGCHARS_SEPARATOR
        {L"b"                       , {0}, L"Base Point Notation",
                                           L"This infix notation make it easy to enter numeric constants in an abritrary base as in 16bFFFF to represent 16" WS_UTF16_UPTACK L"15 15 15 15 or 65535.  "
                                           L"The number to the left of the b is the base of the number system for the characters to the right of the b."
                                           },
        {L"e"                       , {0}, L"Exponential Point Notation",
                                           L"This infix notation allows you to enter numeric constants by specifying a signed multiplier and a signed base 10 exponent "
                                           L"as in 1.23e" WS_UTF16_OVERBAR L"3 to represent 1.23" WS_UTF16_TIMES L"10*" WS_UTF16_OVERBAR L"3."
                                           },
        {L"g"                       , {0}, L"Gamma Point Notation",
                                           L"This infix notation allows you to enter numeric constants of the form M" WS_UTF16_TIMES WS_UTF16_GAMMA L"*E "
                                           L"as in 0.5g1 for " WS_UTF16_GAMMA WS_UTF16_COLONBAR L"2, or combining this with Rational point notation, 1r3g1 for " WS_UTF16_GAMMA WS_UTF16_COLONBAR L"3,"
                                           L" where " WS_UTF16_GAMMA L" is Euler-Mascheroni's Constant (0.5772156649015329...)."
                                           },
        {L"p"                       , {0}, L"Pi Point Notation",
                                           L"This infix notation allows you to enter numeric constants of the form M" WS_UTF16_TIMES L"(" WS_UTF16_CIRCLE L"1)*E "
                                           L"as in 0.5p1 for " WS_UTF16_PI WS_UTF16_COLONBAR L"2, or combining this with Rational point notation, 1r3p1 for " WS_UTF16_PI WS_UTF16_COLONBAR L"3,"
                                           L" where " WS_UTF16_PI L" is Archimedes' Constant (3.141592653589793...)."
                                           },
        {DEF_RATSEP_WS              , {0}, L"Rational Point Notation",
                                           L"This infix notation allows you to enter Rational numbers as in 1r3 to represent 1" WS_UTF16_COLONBAR L"3, or, if the denominator is 1, using the suffix x as in 123x.  "
                                           L"Such constants are of infinite precision, limited only by the available workspace."
                                           },
        {DEF_VFPSEP_WS              , {0}, L"Variable-precision Floating Point Notation",
                                           L"This suffix notation allows you to enter VFP numbers as in 1.3v to represent 1.3 or 12v to represent 12 where both have "
                                           L"a precision controlled by the current value of " WS_UTF16_QUAD L"FPC, as opposed to the 53-bit precision of IEEE-754 floating point numbers."
                                           },
        {L"x"                       , {0}, L"Euler Point Notation",
                                           L"This infix notation allows you to enter numeric constants of the form M" WS_UTF16_TIMES L"(*1)*E "
                                           L"as in 3x2 for three times e squared where e is Euler's Number (2.718281828459045...).\r\n\r\n"
                                           L"As a suffix notation, \"x\" following an integer constant such as " WS_UTF16_OVERBAR L"123x indicates "
                                           L"that the constant is a rational integer identical to " WS_UTF16_OVERBAR L"123r1."
                                           },
    };

#define LANGCHARS_LENGTH        countof (langChars)
#define LANGCHARS_SEPWIDTH      8
#define LANGCHARS_LEFTMARGIN    5
#define LANGCHARS_RIGHTMARGIN   5
#define LANGCHARS_RIGHTPAD      2

#define NOLASTCHAR              -1

    UINT              uCnt;                     // Loop counter
    POINT             pt;                       // Mouse coords
    static UINT       uLastCnt = NOLASTCHAR;    // Index of previously outlined char
    HDC               hDCMem;                   // Memory DC
    HBITMAP           hBitmap,                  // Compatible bitmap
                      hBitmapOld;               // Old bitmap
    HFONT             hFontTmp,                 // Temporary font
                      hFontOld;                 // Old font
    RECT              rcChar;                   // Rectangle around largest char
    static UBOOL      bTrackMouse = FALSE;      // TRUE iff tracking the mouse
           TOOLINFOW  tti;                      // ...
    static HWND       hWndTT;                   // Tooltip window handle

////LCLODSAPI ("LW_RB: ", hWnd, message, wParam, lParam);
    switch (message)
    {
        case WM_CREATE:
            // Create a Tooltip window
            hWndTT = CreateTooltip (TRUE);

            // Check for errors
            if (hWndTT EQ NULL)
                return -1;                      // Stop the whole process

            // Set the Tooltip font for the Language window
            SetTTFontLW (hWndTT);

            // Fill in the TOOLINFOW struc
            InitToolInfoW (hWnd, &tti);

            // Initialize window-specific resources
            LW_RB_Create (hWnd);

            // Attach the Tooltip window to it
            SendMessageW (hWndTT, TTM_ADDTOOLW, 0, (LPARAM) &tti);

            break;

#define fwSizeType  wParam
#define nWidth      (LOWORD (lParam))
#define nHeight     (HIWORD (lParam))
        case WM_SIZE:               // fwSizeType = wParam;      // Resizing flag
                                    // nWidth = LOWORD(lParam);  // Width of client area
                                    // nHeight = HIWORD(lParam); // Height of client area
            if (fwSizeType NE SIZE_MINIMIZED)
            {
                RECT  rcWindow;                 // Window rectangle
                UINT  uWidth;                   // Char width
                int   nBands;                   // Expected # bands
                DWORD dwBtnSize;                // Button size

                // Get the actual window size
                GetWindowRect (hWnd, &rcWindow);

                // Calculate the available window width
                uWidth = rcWindow.right - rcWindow.left;

                if (uWidth NE 0)
                {
                    // Calculate the horizontal & vertical span across APL chars
                    dwBtnSize = (DWORD)
                      SendMessageW (hWnd, MYWM_GETCLIENTSIZE, 0, 0);

                    // Calculate the expected # Language Bands
                    nBands = ((GET_X_LPARAM (dwBtnSize) - LANGCHARS_LEFTMARGIN) + uWidth - 1)/ uWidth;

                    // If the band count changes, ...
                    if (nLangBands NE nBands)
                    {
                        // Save as new count
                        nLangBands = nBands;

                        // Update the Language Bar
                        InitLanguageBand (SendMessageW (hWndRB, RB_IDTOINDEX, IDWC_LW_RB, 0));

                        // Invalidate the window
                        InvalidateRect (hWnd, NULL, FALSE);
                    } // End IF
                } // End IF
            } // End IF

            break;                  // *MUST* pass on to DefMDIChildProcW
#undef  nHeight
#undef  nWidth
#undef  fwSizeType

        case WM_SYSCOLORCHANGE:
        case WM_SETTINGCHANGE:
            // Uninitialize window-specific resources
            LW_RB_Delete (hWnd);

            // Initialize window-specific resources
            LW_RB_Create (hWnd);

            break;                  // Continue with next handler

        case WM_NOTIFY:             // idCtrl = (int) wParam;
        {                           // pnmh = (LPNMHDR) lParam;
#ifdef DEBUG
            LPNMHDR            lpnmhdr = (LPNMHDR) lParam;
            LPNMTTDISPINFOW    lpnmtdi = (LPNMTTDISPINFOW) lParam;
            WPARAM             idCtl   = wParam;
#else
  #define lpnmhdr         ((LPNMHDR) lParam)
  #define lpnmtdi         ((LPNMTTDISPINFOW) lParam)
  #define idCtl           ((WPARAM) wParam)
#endif
            // Split cases based upon the code
            switch (lpnmhdr->code)
            {
////////////////case TTN_NEEDTEXTW:
                case TTN_GETDISPINFOW:
                    // If there's a last outlined char, ...
                    if (uLastCnt NE NOLASTCHAR)
                    {
                        static WCHAR wszText[2048];
                               UINT  uLen;

                        // Create the tooltip title
                        MySprintfW (wszText,
                                    sizeof (wszText),
                                   L"%s (%s)\n",
                                    langChars[uLastCnt].lpwszTitle,
                                    langChars[uLastCnt].lpwc);
                        // Get the title length
                        uLen = lstrlenW (wszText);

                        // Append enough underbars to underline the text
                        //   "- 1" to omit the trailing L'\n'
                        FillMemoryW (&wszText[uLen], uLen - 1, UTF16_LDC_DB_HORZ);

                        // Append the tooltip text
                        MySprintfW (&wszText[uLen + uLen - 1],
                                     sizeof (wszText) - (lstrlenW (wszText) * sizeof (wszText[0])),
                                    L"\n%s",
                                     langChars[uLastCnt].lpwszTipText);
                        // Display the tooltip for the last outlined char
                        lpnmtdi->lpszText = wszText;
                    } // End IF

                    break;

                default:
                    break;
            } // End SWITCH

            return FALSE;
#ifndef DEBUG
  #undef  idCtl
  #undef  lpnmtdi
  #undef  lpnmhdr
#endif
        } // End WM_NOTIFY

        case WM_GETFONT:
            // Get the font handle we want to use
            return (LRESULT) GetFSIndFontHandle (FONTENUM_LW);

        case WM_SETFONT:
            // Set the Tooltip font for the Language window
            SetTTFontLW (hWndTT);

            return FALSE;           // We handled the msg

        case MYWM_CLEARCHAR:
            // If there is a prevously outlined char, ...
            if (uLastCnt NE NOLASTCHAR)
            {
                // Get a DC to write into
                hDCMem = MyGetDC (hWnd);

                // Create a bitmap big enough
                hBitmap = MyCreateCompatibleBitmap (hDCMem,
                                                    128,        // Width -- any big enough value
                                                    128);       // Height ...
                hBitmapOld = SelectObject (hDCMem, hBitmap);

                // Set the background color so when we paint the
                //   char, the old highlight border is cleared.
                SetBkColor (hDCMem, GetSysColor (COLOR_BTNFACE));

                // Make a temporary font from the one for the Language Bar
                hFontTmp = MyCreateFontIndirectW (GetFSIndLogfontW (FONTENUM_LW));

                // Put it into effect
                hFontOld = SelectObject (hDCMem, hFontTmp);

                // Get the surrounding rectangle
                rcChar = langChars[uLastCnt].rcHit;

                // Draw the char
                DrawTextW (hDCMem,
                           langChars[uLastCnt].lpwc,
                           lstrlenW (langChars[uLastCnt].lpwc),
                          &rcChar,
                           0
                         | DT_NOPREFIX
                         | DT_SINGLELINE
                           );
                // Back off one to the left so we
                //   overlap left and right borders
                rcChar.left--;

                // Draw a normal border around the char
                DrawEdge (hDCMem,       // Handle to DC
                         &rcChar,       // Ptr to rectangle
                          0             // Type of inner and outer edge
                        | EDGE_RAISED
                          ,
                          0             // Type of border
                        | BF_FLAT
                        | BF_MONO
                        | BF_RECT
                          );
                // Restore the old resources
                SelectObject (hDCMem, hBitmapOld);
                SelectObject (hDCMem, hFontOld);

                // We no longer need these resources
                MyDeleteObject (hFontTmp); hFontTmp = NULL;
                MyDeleteObject (hBitmap); hBitmap = NULL;
                MyReleaseDC (hWnd, hDCMem); hDCMem = NULL;

                // Unmark as previously outlined
                uLastCnt = NOLASTCHAR;
            } // End IF

            return FALSE;           // We handled the msg

        case WM_MOUSEMOVE:          // fwKeys = wParam;        // key flags
                                    // xPos = LOWORD(lParam);  // horizontal position of cursor (CA)
                                    // yPos = HIWORD(lParam);  // vertical position of cursor (CA)
            // Save the mouse position as a point
            pt.x = GET_X_LPARAM (lParam);
            pt.y = GET_Y_LPARAM (lParam);

            // Loop through the characters looking for the
            //   rectangle that encompasses this point
            for (uCnt = 0; uCnt < LANGCHARS_LENGTH; uCnt++)
            if (PtInRect (&langChars[uCnt].rcHit, pt))
            {
                // If this character index is different from the last, ...
                if (uLastCnt NE uCnt)
                {
                    // Clear the old highlight (if any)
                    SendMessageW (hWnd, MYWM_CLEARCHAR, 0, 0);

                    // Save index to last char
                    uLastCnt = uCnt;

                    // Set the Tooltip balloon to 3/4 down from the top to bottom borders of the
                    //   Hit rectangle so that it always appears in a constant place
                    pt.x = (langChars[uCnt].rcHit.left + langChars[uCnt].rcHit.right) / 2;
                    pt.y = (langChars[uCnt].rcHit.top + 3 * langChars[uCnt].rcHit.bottom) / 4;

                    // Convert the coords for the Tooltip
                    ClientToScreen (hWnd, &pt);

                    // Tell the Tooltip Ctrl the new position
                    SendMessageW (hWndTT, TTM_TRACKPOSITION, 0, MAKELONG (pt.x, pt.y));

                    // Get the surrounding rectangle
                    rcChar = langChars[uCnt].rcHit;

                    // Back off one to the left so we
                    //   overlap left and right borders
                    rcChar.left--;

                    // Move the border in by one pixel
                    rcChar.left++;
                    rcChar.top++;
                    rcChar.right--;
                    rcChar.bottom--;

                    // Get a DC to write into
                    hDCMem = MyGetDC (hWnd);

                    // Create a bitmap big enough
                    hBitmap = MyCreateCompatibleBitmap (hDCMem,
                                                        128,        // Width -- any big enough value
                                                        128);       // Height ...
                    hBitmapOld = SelectObject (hDCMem, hBitmap);

                    // Set the background color so when we paint the
                    //   char, the old highlight border is cleared.
                    SetBkColor (hDCMem, GetSysColor (COLOR_BTNFACE));

                    // Make a temporary font from the one for the Language Bar
                    hFontTmp = MyCreateFontIndirectW (GetFSIndLogfontW (FONTENUM_LW));

                    // Put it into effect
                    hFontOld = SelectObject (hDCMem, hFontTmp);

                    // Draw a highlight border around the char
                    DrawEdge (hDCMem,       // Handle to DC
                             &rcChar,       // Ptr to rectangle
                              0             // Type of inner and outer edge
                            | EDGE_RAISED
                              ,
                              0             // Type of border
                            | BF_FLAT
                            | BF_MONO
                            | BF_RECT
                              );
                    // Restore the old resources
                    SelectObject (hDCMem, hBitmapOld);
                    SelectObject (hDCMem, hFontOld);

                    // We no longer need these resources
                    MyDeleteObject (hFontTmp); hFontTmp = NULL;
                    MyDeleteObject (hBitmap); hBitmap = NULL;
                    MyReleaseDC (hWnd, hDCMem); hDCMem = NULL;
                } // End IF

                // If not already tracking, ...
                if (!bTrackMouse)
                {
                    // Fill in the TOOLINFOW struc
                    InitToolInfoW (hWnd, &tti);

                    // Capture the mouse
                    SetCapture (hWnd);

                    // Tell the Tooltip Ctrl to track
                    SendMessageW (hWndTT, TTM_TRACKACTIVATE, TRUE, (LPARAM) &tti);

                    // Mark as tracking
                    bTrackMouse = TRUE;
                } // End IF

                break;
            } // End FOR/IF

            // If we didn't find a match, ...
            if (uCnt EQ LANGCHARS_LENGTH)
            {
                // The mouse is outside our window
                // Clear tracking and capture
                PostMessageW (hWnd, WM_MOUSELEAVE, 0, 0);

                // Mark as no longer tracking
                bTrackMouse = FALSE;
            } // End IF

            return FALSE;           // We handled the msg

        case WM_MOUSELEAVE:         // fwKeys = wParam;        // key flags
                                    // xPos = LOWORD(lParam);  // horizontal position of cursor (CA)
                                    // yPos = HIWORD(lParam);  // vertical position of cursor (CA)
            // The mouse is outside our window and
            //   we no longer need this resource
            ReleaseCapture ();

            // Fill in the TOOLINFOW struc
            InitToolInfoW (hWnd, &tti);

            // Tell the Tooltip Ctrl to stop tracking
            SendMessageW (hWndTT, TTM_TRACKACTIVATE, FALSE, (LPARAM) &tti);

            // Mark as no longer tracking
            bTrackMouse = FALSE;

            // Clear the old highlight (if any)
            SendMessageW (hWnd, MYWM_CLEARCHAR, 0, 0);

            return FALSE;           // We handled the msg

        case WM_LBUTTONUP:          // fwKeys = wParam;        // key flags
                                    // xPos = LOWORD(lParam);  // horizontal position of cursor (CA)
                                    // yPos = HIWORD(lParam);  // vertical position of cursor (CA)
            // Save the mouse position as a point
            pt.x = GET_X_LPARAM (lParam);
            pt.y = GET_Y_LPARAM (lParam);

            // Loop through the characters looking for the
            //   rectangle that encompasses this point
            for (uCnt = 0; uCnt < LANGCHARS_LENGTH; uCnt++)
            if (PtInRect (&langChars[uCnt].rcHit, pt))
            {
                HWND  hWndEC;       // Edit Ctrl window handle

                // Get the handle to the Edit Ctrl
                hWndEC  = GetActiveEC (hWndTC);

                // Insert that char into the corresponding Edit Ctrl
                InsRepCharStr (hWndEC, langChars[uCnt].lpwc, FALSE);

                break;              // Seek no more
            } // End FOR/IF

            return FALSE;           // We handled the msg

        case MYWM_GETCLIENTSIZE:
        {
            UINT uWidth,            // Width of the client area
                 uHeight;           // Height ...

            // Get a DC to write into
            hDCMem = MyGetDC (hWnd);

            // Create a bitmap big enough
            hBitmap = MyCreateCompatibleBitmap (hDCMem,
                                                128,        // Width -- any big enough value
                                                128);       // Height ...
            hBitmapOld = SelectObject (hDCMem, hBitmap);

            // Make a temporary font from the one for the Language Bar
            hFontTmp = MyCreateFontIndirectW (GetFSIndLogfontW (FONTENUM_LW));

            // Put it into effect
            hFontOld = SelectObject (hDCMem, hFontTmp);

            // Initialize the width and height
            uWidth  = LANGCHARS_LEFTMARGIN;
            uHeight = 0;

            // Loop through the chars
            for (uCnt = 0; uCnt < LANGCHARS_LENGTH; uCnt++)
            {
                if (langChars[uCnt].lpwc)
                {
                    RECT rcTemp;            // Temporary rectangle

                    // Clear the initial rectangle
                    SetRectEmpty (&rcTemp);

                    // Get the size of the surrounding rectangle
                    DrawTextW (hDCMem,
                               langChars[uCnt].lpwc,
                               lstrlenW (langChars[uCnt].lpwc),
                              &rcTemp,
                               0
                             | DT_CALCRECT
                             | DT_NOPREFIX
                             | DT_SINGLELINE
                               );
                    // Calculate the char width and height
                    //   "- 1" to account for overlapping left and right borders
                    uWidth += (rcTemp.right - rcTemp.left) + LANGCHARS_RIGHTPAD - 1;
                    uHeight = max (uHeight, (UINT) (rcTemp.bottom - rcTemp.top));
                } else
                    // It's a separator
                    uWidth += LANGCHARS_SEPWIDTH;
            } // End FOR

            // Make room for a right margin and
            // "1 +" to include the rightmost border
            uWidth += 1 + LANGCHARS_RIGHTMARGIN;

            // Restore the old resources
            SelectObject (hDCMem, hBitmapOld);
            SelectObject (hDCMem, hFontOld);

            // We no longer need these resources
            MyDeleteObject (hFontTmp); hFontTmp = NULL;
            MyDeleteObject (hBitmap); hBitmap = NULL;
            MyReleaseDC (hWnd, hDCMem); hDCMem = NULL;

            // Return the height and width of the client area
            return MAKELPARAM (uWidth, uHeight);
        } // End MYWM_GETCLIENTSIZE

        case WM_PAINT:
        {
            RECT        rcUpdate,           // Update rectangle
                        rcClient,           // Client rectangle
                        rcChar;             // Rectangle around largest char
            HBRUSH      hBrush;             // Background brush handle
            HDC         hDCMem;             // Memory DC
            HBITMAP     hBitmap,            // Compatible bitmap
                        hBitmapOld;         // Old bitmap
            HFONT       hFontOld;           // Old font
            PAINTSTRUCT ps;                 // Paint struc
            UINT        uWidth,             // Char width
                        uHeight;            // Line height
            DWORD       dwBtnSize;          // Button size

            // Get the update rectangle
            if (!GetUpdateRect (hWnd, &rcUpdate, FALSE))
                return FALSE;

            // Layout the drop cloths
            BeginPaint (hWnd, &ps);

            // Create a compatible DC and bitmap
            hDCMem = MyCreateCompatibleDC (ps.hdc);
            hBitmap = MyCreateCompatibleBitmap (ps.hdc,
                                                rcUpdate.right,
                                                rcUpdate.bottom);
            hBitmapOld = SelectObject (hDCMem, hBitmap);

            // Copy various attributes from the screen DC to the memory DC
            SetBkMode    (hDCMem, TRANSPARENT          );
            SetBkColor   (hDCMem, GetBkColor   (ps.hdc));
            SetTextColor (hDCMem, GetTextColor (ps.hdc));

            // Make a temporary font from the one for the Language Bar
            hFontTmp = MyCreateFontIndirectW (GetFSIndLogfontW (FONTENUM_LW));

            // Put it into effect
            hFontOld = SelectObject (hDCMem, hFontTmp);

            // Get the background brush
            hBrush = (HBRUSH) GetClassLongPtr (hWnd, GCLP_HBRBACKGROUND);

            // Handle WM_ERASEBKGND here by filling in the client area
            //   with the class background brush
            FillRect (hDCMem, &rcUpdate, hBrush);

            // Calculate the client rectangle
            GetClientRect (hWnd, &rcClient);

            // Calculate the horizontal & vertical span across APL chars
            dwBtnSize = (DWORD)
              SendMessageW (hWnd, MYWM_GETCLIENTSIZE, 0, 0);

            // Set the character rectangle
            rcChar.left   = LANGCHARS_LEFTMARGIN;
            rcChar.top    = 0;
            rcChar.bottom = GET_Y_LPARAM (dwBtnSize);
////////////rcChar.right  = GET_X_LPARAM (dwBtnSize);   // Filled in below

            // Paint the window

            // Loop through the chars
            for (uCnt = 0; uCnt < LANGCHARS_LENGTH; uCnt++)
            {
                if (langChars[uCnt].lpwc)
                {
                    RECT rcTemp;            // Temporary rectangle

                    // Clear the initial rectangle
                    SetRectEmpty (&rcTemp);

                    // Get the size of the surrounding rectangle
                    DrawTextW (hDCMem,
                               langChars[uCnt].lpwc,
                               lstrlenW (langChars[uCnt].lpwc),
                              &rcTemp,
                               0
                             | DT_CALCRECT
                             | DT_NOPREFIX
                             | DT_SINGLELINE
                               );
                    // Calculate the char width
                    uWidth = (rcTemp.right - rcTemp.left) + LANGCHARS_RIGHTPAD;

                    // If the new width exceeds the window width, ...
                    if ((rcChar.left + uWidth) > (UINT) rcClient.right)
                    {
                        uHeight = rcChar.bottom - rcChar.top;

////////////////////////rcChar.right  -= rcChar.left;
                        rcChar.left    = LANGCHARS_LEFTMARGIN;
                        rcChar.top     = rcChar.bottom;
                        rcChar.bottom += uHeight;
                    } // End IF

                    // Set the right edge to the width of the char
                    rcChar.right = rcChar.left + uWidth;

                    // Draw the char
                    DrawTextW (hDCMem,
                               langChars[uCnt].lpwc,
                               lstrlenW (langChars[uCnt].lpwc),
                              &rcChar,
                               0
                             | DT_NOPREFIX
                             | DT_SINGLELINE
                               );
                    // Save the rectangle for the hit test
                    langChars[uCnt].rcHit = rcChar;

                    // Back off one to the left so we
                    //   overlap left and right borders
                    rcChar.left--;

                    // Draw a normal border around the char
                    DrawEdge (hDCMem,       // Handle to DC
                             &rcChar,       // Ptr to rectangle
                              0             // Type of inner and outer edge
                            | EDGE_RAISED
                              ,
                              0             // Type of border
                            | BF_FLAT
                            | BF_MONO
                            | BF_RECT
                              );
                    // Skip over this char
                    rcChar.left += uWidth;
                } else
                    // It's a separator
                    rcChar.left += LANGCHARS_SEPWIDTH;
            } // End FOR

            // Copy the memory DC to the screen DC
            BitBlt (ps.hdc,
                    rcUpdate.left,
                    rcUpdate.top,
                    rcUpdate.right,
                    rcUpdate.bottom,
                    hDCMem,
                    rcUpdate.left,
                    rcUpdate.top,
                    SRCCOPY);
            // Restore the old resources
            SelectObject (hDCMem, hBitmapOld);
            SelectObject (hDCMem, hFontOld);

            // We no longer need these resources
            MyDeleteObject (hFontTmp); hFontTmp = NULL;
            MyDeleteObject (hBitmap); hBitmap = NULL;
            MyDeleteDC (hDCMem); hDCMem = NULL;

            // Break out the turpentine
            EndPaint (hWnd, &ps);

            return FALSE;           // We handled the msg
        } // End WM_PAINT

        case WM_ERASEBKGND:
            // In order to reduce screen flicker, we handle erase background
            // in the WM_PAINT message for the child windows.
            return TRUE;            // We erased the background

        case WM_CLOSE:
            DestroyWindow (hWnd);

            return FALSE;           // We handled the msg

        case WM_DESTROY:
            // Fill in the TOOLINFOW struc
            InitToolInfoW (hWnd, &tti);

            // Unregister the Tooltip for the Language Window
            SendMessageW (hWndTT,
                          TTM_DELTOOLW,
                          0,
                          (LPARAM) &tti);
            // Uninitialize window-specific resources
            LW_RB_Delete (hWnd);

            // If the Tooltip window is still present, ...
            if (hWndTT)
            {
                // Destroy it
                DestroyWindow (hWndTT); hWndTT = NULL;
            } // End IF

            return FALSE;           // We handled the msg

        default:
            break;
    } // End SWITCH

////LCLODSAPI ("LW_RBZ:", hWnd, message, wParam, lParam);
    return DefWindowProcW (hWnd, message, wParam, lParam);
} // End LW_RBWndProc


//***************************************************************************
//  $InitToolInfoW
//
//  Initialize the TOOLINFOW struc
//***************************************************************************

void InitToolInfoW
    (HWND        hWnd,                  // Callback window handle
     LPTOOLINFOW lptti)                 // Ptr to TOOLINFOW struc

{
    // Fill in the TOOLINFOW size based upon the matching COMCTL32.DLL version #
    ZeroMemory (lptti, sizeof (*lptti));
    if (fComctl32FileVer >= 6)
        lptti->cbSize = sizeof (*lptti);
    else
        lptti->cbSize = TTTOOLINFOW_V2_SIZE;

    // Fill in the TOOLINFOW struc
    lptti->uFlags   = 0
                    | TTF_IDISHWND
                    | TTF_TRACK
                    ;
    lptti->hwnd     = hWnd;
    lptti->uId      = (UINT_PTR) hWnd;
////lptti->rect     =                       // Not used with TTF_IDISHWND
////lptti->hinst    =                       // Not used except with string resources
    lptti->lpszText = LPSTR_TEXTCALLBACKW;
////lptti->lParam   =                       // Not used by this code
} // End InitToolInfoW


//***************************************************************************
//  $SetTTFontLW
//
//  Set the Tooltip font for the Language Window
//***************************************************************************

void SetTTFontLW
    (HWND  hWndTT)                      // Tooltip window handle

{
    HFONT hFont;

    // Set the maximum Tooltip width in pixels
    SendMessageW (hWndTT, TTM_SETMAXTIPWIDTH, 0, GetFSIndAveCharSize (FONTENUM_LW)->cx * MAXTIPWIDTHINCHARS);

    // Get the font handle we want to use
    hFont = GetFSIndFontHandle (FONTENUM_LW);

    // If it's valid, ...
    if (hFont)
        // Use the LW font to display the Tooltip
        SendMessageW (hWndTT, WM_SETFONT, (WPARAM) hFont, MAKELPARAM (TRUE, 0));
} // End SetTTFontLW


//***************************************************************************
//  $InitLanguageBand
//
//  Initialize the Language Band
//***************************************************************************

void InitLanguageBand
    (APLU3264 uBandPos)         // Insert the band in this position
                                // (-1 = at the end)

{
    REBARBANDINFOW rbBand;      // Rebar band info struc
    DWORD          dwBtnSize;   // Size of buttons in Toolbar
    UINT           cxLW_RB,     // Width of Language Window
                   cyLW_RB;     // Height ...
////UINT           uItem;       // Index of this band

    // Calculate the size of the client area
    dwBtnSize = (DWORD)
      SendMessageW (hWndLW_RB, MYWM_GETCLIENTSIZE, 0, 0);
    cxLW_RB = GET_X_LPARAM (dwBtnSize);
    cyLW_RB = GET_Y_LPARAM (dwBtnSize) * nLangBands;

    // Initialize the band info struc
    ZeroMemory (&rbBand, sizeof (rbBand));

    // Specify the attributes of the band
    rbBand.cbSize     = sizeof (REBARBANDINFOW);
    rbBand.fMask      = 0
                      | RBBIM_STYLE     // fStyle is valid
                      | RBBIM_TEXT      // lpText is valid
                      | RBBIM_CHILD     // hwndChild is valid
                      | RBBIM_CHILDSIZE // child size members are valid
                      | RBBIM_IDEALSIZE // cxIdeal is valid
                      | RBBIM_SIZE      // cx is valid
                      | RBBIM_ID        // wID is valid
                      ;
    rbBand.fStyle     = 0
                      | RBBS_CHILDEDGE
                      | RBBS_GRIPPERALWAYS
                      | RBBS_BREAK
////                  | RBBS_USECHEVRON
                      ;
////rbBand.clrFore    =
////rbBand.clrBack    =
    rbBand.lpText     = L"Lang";
////rbBand.cch        =
////rbBand.iImage     =
    rbBand.hwndChild  = hWndLW_RB;
    rbBand.cxMinChild = 700;            // ***FIXME*** -- calculate the correct minimum width (say, up to and including Grade Down)
    rbBand.cyMinChild = cyLW_RB;
    rbBand.cxIdeal    = cxLW_RB;
    rbBand.cx         = cxLW_RB;
////rbBand.hbmBack    =
    rbBand.wID        = IDWC_LW_RB;
////rbBand.cyChild    =
////rbBand.cyMaxChild =
////rbBand.cyIntegral =
////rbBand.lParam     =
////rbBand.cxheader   =

    // If the band is new, ...
    if (uBandPos EQ -1)
        // Insert the Language Window band
        SendMessageW (hWndRB, RB_INSERTBANDW, (WPARAM) uBandPos, (LPARAM) &rbBand);
    else
        // Reset the info for the Language Window band
        SendMessageW (hWndRB, RB_SETBANDINFOW, (WPARAM) uBandPos, (LPARAM) &rbBand);

////// Get the zero-based index of the Language Window band
////uItem = SendMessageW (hWndRB, RB_IDTOINDEX, IDWC_LW_RB, 0);
////
////// Maximize this band to the size of cxIdeal
////SendMessageW (hWndRB, RB_MAXIMIZEBAND, uItem, TRUE);

    // (Re)set the Language Window font
    SendMessageW (hWndLW_RB, WM_SETFONT, (WPARAM) GetFSIndFontHandle (FONTENUM_LW), MAKELPARAM (TRUE, 0));
} // End InitLanguageBand


//***************************************************************************
//  End of File: rebar.c
//***************************************************************************
