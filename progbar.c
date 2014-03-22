//***************************************************************************
//  NARS2000 -- Progress Bar Routines
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2012 Sudley Place Software

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


typedef struct tagPBWD
{
    RECT rcClient;      // Client rectangle
    UINT uCurPos,       // Current position
         uState,        // Current state (PBST_xxx)
         uMinVal,       // Minimum value of the range
         uMaxVal,       // Maximum ...
         uStep;         // Step value
    HWND hBuddy;        // Buddy window handle
} PBWD, *LPPBWD;

typedef struct tagCOLORBLEND
{
    COLORREF topBlend[6],
             topLine,
             midBands[4],
             botLine;
} COLORBLEND, *LPCOLORBLEND;

COLORBLEND pbGreen
= {{RGB (214,248,211),      // topBlend[0]
    RGB (198,255,199),      // ...     [1]
    RGB (188,255,195),      // ...     [2]
    RGB (182,238,191),      // ...     [3]
    RGB (171,233,184),      // ...     [4]
    RGB (139,249,160)},     // ...     [5]
    RGB ( 17,199, 52),      // topLine
   {RGB (  0,215, 32),      // midBands[0]
    RGB (  0,216, 34),      // ...     [1]
    RGB (  6,207, 43),      // ...     [2]
    RGB (  5,207, 43)},     // ...     [3]
    RGB ( 53,209, 73)       // botLine
  },
           pbRed
= {{RGB (244,210,211),      // topBlend[0]
    RGB (255,201,202),      // ...     [1]
    RGB (255,188,185),      // ...     [2]
    RGB (245,165,164),      // ...     [3]
    RGB (244,166,162),      // ...     [4]
    RGB (244,150,148)},     // ...     [5]
    RGB (196,  6,  6),      // topLine
   {RGB (219,  0,  0),      // midBands[0]
    RGB (216,  0,  0),      // ...     [1]
    RGB (206,  2,  3),      // ...     [2]
    RGB (207,  1,  3)},     // ...     [3]
    RGB (211, 19, 18)       // botLine
  },
           pbYellow
= {{RGB (254,250,221),      // topBlend[0]
    RGB (254,253,205),      // ...     [1]
    RGB (254,253,189),      // ...     [2]
    RGB (253,251,164),      // ...     [3]
    RGB (255,252,139),      // ...     [4]
    RGB (255,254,111)},     // ...     [5]
    RGB (205,199, 27),      // topLine
   {RGB (225,219,  7),      // midBands[0]
    RGB (225,219,  8),      // ...     [1]
    RGB (229,220, 12),      // ...     [2]
    RGB (229,220, 16)},     // ...     [3]
    RGB (243,235, 75)       // botLine
  };


//***************************************************************************
//  $PB_Create
//
//  Perform window-specific initialization
//***************************************************************************

void PB_Create
    (HWND hWnd)

{
} // End PB_Create


//***************************************************************************
//  $PB_Delete
//
//  Perform window-specific uninitialization
//***************************************************************************

void PB_Delete
    (HWND hWnd)

{
} // End PB_Delete


//***************************************************************************
//  $PBWndProc
//
//  Message processing routine for the Progress Bar window
//***************************************************************************

LRESULT APIENTRY PBWndProc
    (HWND   hWnd,       // Window handle
     UINT   message,    // Type of message
     WPARAM wParam,     // Additional information
     LPARAM lParam)     // ...

{
    HGLOBAL hGlbWD;     // Window data global memory handle
    LPPBWD  lpMemWD;    // Ptr to per-window data global memory
    UINT    uCurPos,    // Current position
            uState,     // ...     state
            uStep;      // ...     step
    HWND    hBuddy;     // Buddy window handle
    DWORD   dwRange;    // Current range

////LCLODSAPI ("PB: ", hWnd, message, wParam, lParam);
    switch (message)
    {
        case WM_CREATE:
            // Allocate global memory for the per window data
            hGlbWD = GlobalAlloc (GHND, sizeof (PBWD));
            if (!hGlbWD)
                return -1;          // Fail the create

            // Save it for later use
            SetWindowLongPtrW (hWnd, GWLPB_HGLB, (HANDLE_PTR) hGlbWD);

            // Lock the memory to get a ptr to it
            lpMemWD = GlobalLock (hGlbWD);

            // Initialize the struct
            lpMemWD->uCurPos = 0;
            lpMemWD->uState  = PBST_NORMAL;
            lpMemWD->uMinVal = 0;
            lpMemWD->uMaxVal = 100;
            lpMemWD->uStep   = 10;
            lpMemWD->hBuddy  = NULL;

            // We no longer need this resource
            GlobalUnlock (hGlbWD); lpMemWD = NULL;

            // Initialize window-specific resources
            PB_Create (hWnd);

            break;

        case PBM_GETSTEP:
            // Get the per-window data global memory handle
            (HANDLE_PTR) hGlbWD = GetWindowLongPtrW (hWnd, GWLPB_HGLB);

            // Lock the memory to get a ptr to it
            lpMemWD = GlobalLock (hGlbWD);

            // Return the current step
            uStep = lpMemWD->uState;

            // We no longer need this resource
            GlobalUnlock (hGlbWD); lpMemWD = NULL;

            return uStep;           // Return the current step

        case PBM_SETSTEP:           // uStep = (UINT) wParam;
            // Get the per-window data global memory handle
            (HANDLE_PTR) hGlbWD = GetWindowLongPtrW (hWnd, GWLPB_HGLB);

            // Lock the memory to get a ptr to it
            lpMemWD = GlobalLock (hGlbWD);

            // Return the old current step
            uStep = lpMemWD->uCurPos;

            // Save as the current step
            lpMemWD->uStep = (UINT) wParam;

            // We no longer need this resource
            GlobalUnlock (hGlbWD); lpMemWD = NULL;

            // Redraw the window
            InvalidateRect (hWnd, NULL, FALSE);

            return uStep;           // Return the previous step

        case PBM_GETPOS:
            // Get the per-window data global memory handle
            (HANDLE_PTR) hGlbWD = GetWindowLongPtrW (hWnd, GWLPB_HGLB);

            // Lock the memory to get a ptr to it
            lpMemWD = GlobalLock (hGlbWD);

            // Return the current position
            uCurPos = lpMemWD->uCurPos;

            // We no longer need this resource
            GlobalUnlock (hGlbWD); lpMemWD = NULL;

            return uCurPos;         // Return the current position

        case PBM_SETPOS:            // uCurPos = (UINT) wParam;
            // Get the per-window data global memory handle
            (HANDLE_PTR) hGlbWD = GetWindowLongPtrW (hWnd, GWLPB_HGLB);

            // Lock the memory to get a ptr to it
            lpMemWD = GlobalLock (hGlbWD);

            // Return the old current position
            uCurPos = lpMemWD->uCurPos;

            // Save as the current position
            lpMemWD->uCurPos = (UINT) wParam;

            // We no longer need this resource
            GlobalUnlock (hGlbWD); lpMemWD = NULL;

            // Redraw the window
            InvalidateRect (hWnd, NULL, FALSE);

            return uCurPos;         // Return the previous position

        case PBM_GETSTATE:
            // Get the per-window data global memory handle
            (HANDLE_PTR) hGlbWD = GetWindowLongPtrW (hWnd, GWLPB_HGLB);

            // Lock the memory to get a ptr to it
            lpMemWD = GlobalLock (hGlbWD);

            // Return the current state
            uState = lpMemWD->uState;

            // We no longer need this resource
            GlobalUnlock (hGlbWD); lpMemWD = NULL;

            return uState;          // Return the current state

        case PBM_SETSTATE:          // uState = (UINT) wParam
            // Get the per-window data global memory handle
            (HANDLE_PTR) hGlbWD = GetWindowLongPtrW (hWnd, GWLPB_HGLB);

            // Lock the memory to get a ptr to it
            lpMemWD = GlobalLock (hGlbWD);

            // Return the old current state
            uState = lpMemWD->uState;

            // Save as the current state
            lpMemWD->uState = (UINT) wParam;

            // We no longer need this resource
            GlobalUnlock (hGlbWD); lpMemWD = NULL;

            // Redraw the window
            InvalidateRect (hWnd, NULL, FALSE);

            return uState;          // Return the previous state

        case PBM_GETBUDDY:
            // Get the per-window data global memory handle
            (HANDLE_PTR) hGlbWD = GetWindowLongPtrW (hWnd, GWLPB_HGLB);

            // Lock the memory to get a ptr to it
            lpMemWD = GlobalLock (hGlbWD);

            // Return the current handle
            hBuddy = lpMemWD->hBuddy;

            // We no longer need this resource
            GlobalUnlock (hGlbWD); lpMemWD = NULL;

            return (LRESULT) hBuddy;    // Return the current buddy window handle

        case PBM_SETBUDDY:          // hWnd = (HWND) wParam
            // Get the per-window data global memory handle
            (HANDLE_PTR) hGlbWD = GetWindowLongPtrW (hWnd, GWLPB_HGLB);

            // Lock the memory to get a ptr to it
            lpMemWD = GlobalLock (hGlbWD);

            // Return the old current handle
            hBuddy = lpMemWD->hBuddy;

            // Save as the current buddy window handle
            lpMemWD->hBuddy = (HWND) wParam;

            // We no longer need this resource
            GlobalUnlock (hGlbWD); lpMemWD = NULL;

            return (LRESULT) hBuddy;    // Return the previous buddy window handle

        case PBM_SETRANGE32:        // uMinVal = (UINT) wParam;
                                    // uMaxVal = (UINT) lParam;
            // Get the per-window data global memory handle
            (HANDLE_PTR) hGlbWD = GetWindowLongPtrW (hWnd, GWLPB_HGLB);

            // Lock the memory to get a ptr to it
            lpMemWD = GlobalLock (hGlbWD);

            // Return the old current range
            dwRange = (DWORD) MAKELONG (lpMemWD->uMinVal, lpMemWD->uMaxVal);

            // Save the new range
            lpMemWD->uMinVal = (UINT) wParam;
            lpMemWD->uMaxVal = (UINT) lParam;

            // We no longer need this resource
            GlobalUnlock (hGlbWD); lpMemWD = NULL;

            // Redraw the window
            InvalidateRect (hWnd, NULL, FALSE);

            return dwRange;         // Return the previous range

        case WM_ERASEBKGND:
            // In order to reduce screen flicker, we handle erase background
            // in the WM_PAINT message for the child windows.
            return TRUE;            // We erased the background

        case WM_PAINT:              // hdc = (HDC) wParam; // the device context to draw in
        {
            RECT        rcUpdate,   // Update rectangle
                        rcClient;   // Client ...
            PAINTSTRUCT ps;         // Paint struct
            COLORBLEND  cbType;     // Background color blend

            // Get the update rectangle
            if (!GetUpdateRect (hWnd, &rcUpdate, FALSE))
                return FALSE;       // We handled the msg

            // If there's no incoming HDC, ...
            if (((HDC) wParam) EQ NULL)
                // Layout the drop cloths
                BeginPaint (hWnd, &ps);
            else
                ps.hdc = (HDC) wParam;
            // Get the per-window data global memory handle
            (HANDLE_PTR) hGlbWD = GetWindowLongPtrW (hWnd, GWLPB_HGLB);

            // Lock the memory to get a ptr to it
            lpMemWD = GlobalLock (hGlbWD);

            // Get the client rect
            GetClientRect (hWnd, &rcClient);

            // If the min != max and min > max, ...
            if (lpMemWD->uMinVal < lpMemWD->uMaxVal)
            {
                // Split cases based upon the PB state
                switch (lpMemWD->uState)
                {
                    case PBST_ERROR:
                        // Set the color
                        cbType = pbRed;

                        break;

                    case PBST_PAUSED:
                        // Set the color
                        cbType = pbYellow;

                        break;

                    case PBST_NORMAL:
                    default:
                        // Set the color
                        cbType = pbGreen;

                        break;
                } // End SWITCH

                // Make a copy of the client rect
                CopyRect (&rcUpdate, &rcClient);

                // Scale the current position to the client rectangle
                // And set as left half of the PB
                rcUpdate.right = rcClient.left
                               + ((rcClient.right - rcClient.left)
                                * (lpMemWD->uCurPos - lpMemWD->uMinVal))
                                 / (lpMemWD->uMaxVal - lpMemWD->uMinVal);
                // Fill in the left half of the PB
                //   with a color blend
                FillProgressBarBackground (ps.hdc, &rcUpdate, &cbType);

                // Make rectangle for right half of PB
                rcUpdate.left  = rcUpdate.right;
                rcUpdate.right = rcClient.right;

                // Fill in the right half of the PB
                //   with a background brush
                FillRect (ps.hdc, &rcUpdate, GetSysColorBrush (COLOR_BTNFACE));
            } // End IF

            // If there's a buddy window handle, ...
            if (lpMemWD->hBuddy)
            {
                WCHAR wszTemp[8];

                uCurPos = lpMemWD->uCurPos - lpMemWD->uMinVal;
                uCurPos = (100 * uCurPos + 50) / lpMemWD->uMaxVal;

                // Format the precentage
                wsprintfW (wszTemp, L"%u%%", uCurPos);

                // Send the new percentage to the buddy window
                SetWindowTextW (lpMemWD->hBuddy, wszTemp);
            } // End IF

            // We no longer need this resource
            GlobalUnlock (hGlbWD); lpMemWD = NULL;

            // If there's no incoming HDC, ...
            if (((HDC) wParam) EQ NULL)
                // Break out the turpentine
                EndPaint (hWnd, &ps);
            return FALSE;           // We handled the msg
        } // End WM_PAINT

        case WM_DESTROY:
            // Uninitialize window-specific resources
            PB_Delete (hWnd);

            // Get the per-window data global memory handle
            (HANDLE_PTR) hGlbWD = GetWindowLongPtrW (hWnd, GWLPB_HGLB);

            // We no longer need this storage
            GlobalFree (hGlbWD); hGlbWD = NULL; SetWindowLongPtrW (hWnd, GWLPB_HGLB, (HANDLE_PTR) hGlbWD);

            return FALSE;           // We handled the msg

        default:
            break;
    } // End SWITCH

////LCLODSAPI ("PBZ:", hWnd, message, wParam, lParam);
    return DefWindowProcW (hWnd, message, wParam, lParam);
} // End PBWndProc


//***************************************************************************
//  $FillProgressBarBackground
//
//  Fill in the Progress Bar background
//***************************************************************************

void FillProgressBarBackground
    (HDC          hDC,      // DC handle
     LPRECT       lpRect,   // Ptr to fill rectangle
     LPCOLORBLEND lpcbType) // Ptr to color blend

{
    HBRUSH hBrush;          // Brush handle
    int    i,               // Loop counter
           nBands;          // # middle bands
    RECT   rcBand;          // Band rectangle

    // Fill the rectangle with one band
    SetRect (&rcBand,
              lpRect->left,
              lpRect->top,
              lpRect->right + 1,
              lpRect->top + 1);
    // Loop through the top blend bands
    for (i = 0; i < countof (lpcbType->topBlend); i++)
    {
        hBrush = MyCreateSolidBrush (lpcbType->topBlend[i]);
        FillRect (hDC, &rcBand, hBrush);
        MyDeleteObject (hBrush); hBrush = NULL;

        // Increment by one band
        rcBand.top++;
        rcBand.bottom++;
    } // End FOR

    // Fill in the top line
    hBrush = MyCreateSolidBrush (lpcbType->topLine);
    FillRect (hDC, &rcBand, hBrush);
    MyDeleteObject (hBrush); hBrush = NULL;

    // Increment by one band
    rcBand.top++;
    rcBand.bottom++;

    // Calculate the # middle Light & Dark bands
    nBands = (lpRect->bottom - lpRect->top)
           - (countof (lpcbType->topBlend)
            + 1                             // countof (lpcbType->topLine)
            + 1);                           // countof (lpcbType->botLine)

    // Loop through the middle Light & Dark bands
    for (i = 0; i < nBands; i++)
    {
        hBrush = MyCreateSolidBrush (lpcbType->midBands[i % countof (lpcbType->midBands)]);
        FillRect (hDC, &rcBand, hBrush);
        MyDeleteObject (hBrush); hBrush = NULL;

        // Increment by one band
        rcBand.top++;
        rcBand.bottom++;
    } // End FOR

    // Fill in the bottom line
    hBrush = MyCreateSolidBrush (lpcbType->botLine);
    FillRect (hDC, &rcBand, hBrush);
    MyDeleteObject (hBrush); hBrush = NULL;

////// Increment by one band
////rcBand.top++;
////rcBand.bottom++;
} // End FillProgressBarBackground


//***************************************************************************
//  End of File: progressbar.c
//***************************************************************************
