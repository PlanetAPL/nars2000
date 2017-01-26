//***************************************************************************
//  NARS2000 -- Performance Monitoring
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


#ifdef PERFMONON

typedef struct tagPERFMONDATA
{
    LPCHAR        lpFileName;       // Ptr to filename
    UINT          uLine;            // Line #
    LARGE_INTEGER liPerf;           // Performance data
} PERFMONDATA, *LPPERFMONDATA;


PERFMONDATA   PerfMonData[128];
UINT          uPerfMonData = 0;
HWND          hWndPM = NULL;
LARGE_INTEGER liStart = {0};
#endif


#ifdef PERFMONON
//***************************************************************************
//  PerfMonInit
//
//  Performance Monitor initialize
//***************************************************************************

void PerfMonInit
    (void)

{
    // Initialize the starting performance counter value
    QueryPerformanceCounter (&liStart);
} // End PerfMonInit
#endif


#ifdef PERFMONON
//***************************************************************************
//  PerfMonAccum
//
//  Performance Monitor accumulate
//***************************************************************************

void PerfMonAccum
    (LPCHAR lpFileName,     // Ptr to filename
     UINT   uLine)          // Line #

{
    // Ensure no overflow
    if (uPerfMonData EQ countof (PerfMonData))
        DbgBrk ();          // #ifdef PERFMONON

    // Save the performance data
    PerfMonData[uPerfMonData].lpFileName = lpFileName;
    PerfMonData[uPerfMonData].uLine      = uLine;
    QueryPerformanceCounter (&PerfMonData[uPerfMonData].liPerf);

    // Convert to origin-0
    PerfMonData[uPerfMonData].liPerf.QuadPart -= liStart.QuadPart;

    // Skip to the next slot
    uPerfMonData++;
} // End PerfMonAccum
#endif


#ifdef PERFMONON
//***************************************************************************
//  $PerfMonShow
//
//  Show Perfomance Monitoring data
//***************************************************************************

void PerfMonShow
    (void)

{
    WCHAR         wszTemp[1024];    // Temporary storage area
    HWND          hWndLB;           // ListBox window handle
    UINT          uCnt;             // Loop counter
    LARGE_INTEGER liLast = {0},     // Last value
                  liCurrent;        // Current # ticks
    LPWCHAR       lpwszMicro = L"\x03BC" L"s";

    // If the Perfmonance Monitoring window is not created, do so now
    if (hWndPM EQ NULL)
        hWndPM = PerfMonCreate ();

    // Get the window handle of the Listbox
    (HANDLE_PTR) hWndLB = GetWindowLongPtrW (hWndPM, GWLPM_HWNDLB);

    // Clear the ListBox
    SendMessageW (hWndLB, LB_RESETCONTENT, 0, 0);

    // Insert the PM lines into the ListBox
    for (uCnt = 0; uCnt < uPerfMonData; uCnt++)
    {
        // Accumulate into the total
        liCurrent.QuadPart = PerfMonData[uCnt].liPerf.QuadPart - liLast.QuadPart;

        // Format the string
        MySprintfW (wszTemp,
                    sizeof (wszTemp),
                   L"%-13S[%5d]: %s %s",
                    PerfMonData[uCnt].lpFileName,
                    PerfMonData[uCnt].uLine,
                    FormatWithSep ((1000000 * liCurrent.QuadPart) / liTicksPerSec.QuadPart, 13),
                    lpwszMicro);
        // Zap for the next time
        lpwszMicro = L"";

        // Save as new Last performance value
        liLast.QuadPart = PerfMonData[uCnt].liPerf.QuadPart;

        // Append the string
        SendMessageW (hWndLB, LB_ADDSTRING, 0, (LPARAM) wszTemp);
    } // End FOR

    liCurrent.QuadPart = PerfMonData[uPerfMonData - 1].liPerf.QuadPart;

    // Format the string
    MySprintfW (wszTemp,
                sizeof (wszTemp),
               L"%-13S %5s   %s %s",
               L"",
               L"",
                FormatWithSep ((1000000 * liCurrent.QuadPart) / liTicksPerSec.QuadPart, 13),
                lpwszMicro);
    // Append the string
    SendMessageW (hWndLB, LB_ADDSTRING, 0, (LPARAM) wszTemp);
} // End PerfMonShow
#endif


#ifdef PERFMONON
//***************************************************************************
//  $PerfMonCreate
//
//  $Create the Performance Monitoring window
//***************************************************************************

HWND PerfMonCreate
    (void)

{
    // Create the Performance Monitoring window
    hWndPM =
      CreateWindowExW (0L,                  // Extended styles
                       LPMWNDCLASS,         // Class
                       wszPMTitle,          // Window title (for debugging purposes only)
                       0
                     | WS_OVERLAPPEDWINDOW
                       ,                    // Styles
                       CW_USEDEFAULT, CW_USEDEFAULT,    // X- and Y-coord
                       CW_USEDEFAULT, CW_USEDEFAULT,    // X- and Y-size
                       hWndMF,              // Parent window
                       NULL,                // Menu
                       _hInstance,          // Instance
                       NULL);               // No extra data
    if (hWndPM EQ NULL)
        MB (pszNoCreatePMWnd);

    return hWndPM;
} // End PerfMonCreate
#endif


#ifdef PERFMONON
//***************************************************************************
//  $FormatWithSep
//
//  Perform window-specific initialization
//***************************************************************************

LPWCHAR FormatWithSep
    (APLUINT aplInteger,
     int     iLen)

{
    static WCHAR wszTemp[256];  // Temporary storage
    int          iCnt,          // Loop counter
                 iSep;          // # commas already introduced

    // Ensure properly terminated
    wszTemp[iLen] = WC_EOS;

    // Loop through the digits
    for (iSep = 0, iCnt = iLen - 1; iCnt >= 0; iCnt--)
    {
        if (aplInteger NE 0
         && iCnt < (iLen - 1)
         && 1 EQ ((iLen - iCnt - iSep) % 3))
        {
            wszTemp[iCnt--] = L',';
            iSep++;
        } // End IF

        wszTemp[iCnt] = L'0' + (UINT) (aplInteger % 10);
        aplInteger /= 10;
    } // End FOR

    // Blank leading blanks
    for (iCnt = 0; iCnt < (iLen - 1); iCnt++)
    if (wszTemp[iCnt] EQ L'0')
        wszTemp[iCnt] = L' ';
    else
        break;

    return wszTemp;
} // End FormatWithSep
#endif


#ifdef PERFMONON
//***************************************************************************
//  $PM_Create
//
//  Perform window-specific initialization
//***************************************************************************

void PM_Create
    (HWND hWnd)

{
} // End PM_Create
#endif


#ifdef PERFMONON
//***************************************************************************
//  $PM_Delete
//
//  Perform window-specific uninitialization
//***************************************************************************

void PM_Delete
    (HWND hWnd)

{
} // End PM_Delete
#endif


#ifdef PERFMONON
//***************************************************************************
//  $PMWndProc
//
//  Message processing routine for the Performance Monitoring window
//***************************************************************************

LRESULT APIENTRY PMWndProc
    (HWND   hWnd,       // Window handle
     UINT   message,    // Type of message
     WPARAM wParam,     // Additional information
     LPARAM lParam)     // ...

{
    HWND         hWndLB;            // ListBox window handle
    LRESULT      lResult = FALSE;   // Result from DefMDIChildProcW

    // Get the window handle of the Listbox
    (HANDLE_PTR) hWndLB = GetWindowLongPtrW (hWnd, GWLPM_HWNDLB);

////LCLODSAPI ("PM: ", hWnd, message, wParam, lParam);
    switch (message)
    {
        case WM_CREATE:
            // Create a listbox to fit inside this window
            hWndLB =
              CreateWindowW (WC_LISTBOXW,           // Class name
                             L"Performance Listbox",   // For debugging only
                             0
                           | WS_CHILD
                           | WS_VSCROLL
                           | WS_HSCROLL
                           | WS_CLIPCHILDREN
                           | LBS_NOINTEGRALHEIGHT
                           | LBS_EXTENDEDSEL
                           | LBS_MULTIPLESEL
                             ,                      // Styles
                             0,                     // X-position
                             0,                     // Y-...
                             CW_USEDEFAULT,         // Width
                             CW_USEDEFAULT,         // Height
                             hWnd,                  // Parent window
                             (HMENU) IDWC_PM_LB,    // ID
                             _hInstance,            // Instance
                             0);                    // lParam
            // Save for later use
            SetWindowLongPtrW (hWnd, GWLDB_HWNDLB, (APLU3264) (LONG_PTR) hWndLB);

            // Set the font
            SendMessageW (hWndLB, WM_SETFONT, (WPARAM) hFontSM, MAKELPARAM (FALSE, 0));

            // Show the windows
            ShowWindow (hWndLB, SW_SHOWNORMAL);
            ShowWindow (hWnd,   SW_SHOWNORMAL);

            // Initialize window-specific resources
            PM_Create (hWnd);

            break;

        case WM_SETFONT:            // hFont = (HFONT) wParam;
                                    // fRedraw = LOWORD (lParam);
        case WM_MOUSEWHEEL:
            // Pass these messages through to the ListBox
            SendMessageW (hWndLB, message, wParam, lParam);

            goto NORMAL_EXIT;       // We handled the msg

        case WM_SYSCOLORCHANGE:
        case WM_SETTINGCHANGE:
            // Uninitialize window-specific resources
            PM_Delete (hWnd);

            // Initialize window-specific resources
            PM_Create (hWnd);

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

        case WM_CLOSE:
            DestroyWindow (hWnd);

            goto NORMAL_EXIT;       // We handled the msg

        case WM_DESTROY:
            // Uninitialize window-specific resources
            PM_Delete (hWnd);

            // Mark as closed
            hWndPM = NULL;

            goto NORMAL_EXIT;       // We handled the msg
    } // End SWITCH

////LCLODSAPI ("PMY:", hWnd, message, wParam, lParam);
    lResult =
      DefWindowProcW (hWnd, message, wParam, lParam);
NORMAL_EXIT:
////LCLODSAPI ("PMZ:", hWnd, message, wParam, lParam);

    return lResult;
} // End PMWndProc
#endif


//***************************************************************************
//  End of File: perfmon.c
//***************************************************************************
