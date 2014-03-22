//***************************************************************************
//  NARS2000 -- StatusBar Routines
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
#include <math.h>
#include "headers.h"

int     glbStatusPartsWidth[SP_LENGTH] = {0};
int     glbStatusBorders[3];
WNDPROC lpfnOldStatusBarWndProc;            // Save area for old StatusBar Window procedure
LPWCHAR lpwszToolTips[SP_LENGTH] =
{
    L"Text message"                     ,       // 00:  Text message
    L"Execution time in hh:mm:ss:ms"    ,       // 01:  Execution timer
    L"Current line #"                   ,       // 02:  Line # (origin-0)
    L"Current character position"       ,       // 03:  Char # (origin-0
    L"State of INS key"                 ,       // 04:  Ins/Ovr state
    L"State of NUM lock key"            ,       // 05:  NumLock state
    L"State of CAPS lock key"           ,       // 06:  CapsLock state
};



//***************************************************************************
//  $MakeStatusWindow
//
//  Initial status window creation
//***************************************************************************

UBOOL MakeStatusWindow
    (HWND hWndParent)

{
    // Create the status window
    hWndStatus =
      CreateWindowExW (0L,                  // Extended styles
                       LCL_WC_STATUSBARW,   // Class name
                       wszStatusIdle,       // Initial text
                       0
                     | WS_CHILD
                     | WS_VISIBLE
                     | SBARS_SIZEGRIP
                       ,                    // Styles
                       0,                   // X-position
                       0,                   // Y-...
                       CW_USEDEFAULT,       // Width
                       CW_USEDEFAULT,       // Height
                       hWndParent,          // Parent window
               (HMENU) IDWC_MF_ST,          // Window ID
                       _hInstance,          // Instance
                       0);                  // lParam
    if (hWndStatus EQ NULL)
        return FALSE;

    // Get the width of the borders of the Status Window
    SendStatusMsg (SB_GETBORDERS, 0, (LPARAM) glbStatusBorders);

    return TRUE;
} // End MakeStatusWindow


//***************************************************************************
//  $SendStatusMsg
//
//  Send a message to the status window and show or hide the
//    window afterwards in case the message sent activated it.
//***************************************************************************

void SendStatusMsg
    (UINT   msg,
     WPARAM wParam,
     LPARAM lParam)

{
    // Send the message to the window
    SendMessageW (hWndStatus, msg, wParam, lParam);

    // Show or hide the status window in case the above message activated the window
    ShowWindow (hWndStatus, OptionFlags.bViewStatusBar ? SW_SHOWNORMAL : SW_HIDE);
} // End SendStatusMsg


//***************************************************************************
//  $SetStatusParts
//
//  Set the Status Parts and right edges
//***************************************************************************

void SetStatusParts
    (UINT uWidth)               // Width of the Client Area

{
    HDC         hDC;
    int         lclStatusPartsRight[SP_LENGTH];
    STATUSPARTS iCnt;           // Loop counter
    SIZE        sText;
    HWND        hWndEC;         // Edit Ctrl window handle

    // Get a Client Area DC for the Status Window
    hDC = MyGetDC (hWndStatus);

    // Copy the Status Window font
    SelectObject (hDC, (HFONT) SendMessageW (hWndStatus, WM_GETFONT, 0, 0));

#define GRIPPER_WIDTH       20
#define SP_BORDER_EXTRA      5

    // Loop backwards through the SetParts enum
    for (iCnt = SP_LENGTH - 1; iCnt >= 0; iCnt--)
    // Split cases based upon the enum value
    switch (iCnt)
    {
        case SP_CAPS:
#define TEXT_CAPS           L"CAPS"
            // Get the width of the text
            GetTextExtentPoint32W (hDC,
                                   TEXT_CAPS,
                                   strcountof (TEXT_CAPS),
                                  &sText);
            // Set the parts width
            glbStatusPartsWidth[iCnt] = sText.cx + SP_BORDER_EXTRA;
#undef  TEXT_CAPS
            break;

        case SP_NUM:
#define TEXT_CAPS           L"NUM"
            // Get the width of the text
            GetTextExtentPoint32W (hDC,
                                   TEXT_CAPS,
                                   strcountof (TEXT_CAPS),
                                  &sText);
            // Set the parts width
            glbStatusPartsWidth[iCnt] = sText.cx + SP_BORDER_EXTRA;
#undef  TEXT_CAPS
            break;

        case SP_INS:
#define TEXT_CAPS           L"OVR"
            // Get the width of the text
            GetTextExtentPoint32W (hDC,
                                   TEXT_CAPS,
                                   strcountof (TEXT_CAPS),
                                  &sText);
            // Set the parts width
            glbStatusPartsWidth[iCnt] = sText.cx + SP_BORDER_EXTRA;
#undef  TEXT_CAPS
            break;

        case SP_CHARPOS:
#define TEXT_CAPS           L"999999"
            // Get the width of the text
            GetTextExtentPoint32W (hDC,
                                   TEXT_CAPS,
                                   strcountof (TEXT_CAPS),
                                  &sText);
            // Set the parts width
            glbStatusPartsWidth[iCnt] = sText.cx + SP_BORDER_EXTRA;
#undef  TEXT_CAPS
            break;

        case SP_LINEPOS:
#define TEXT_CAPS           L"999999"
            // Get the width of the text
            GetTextExtentPoint32W (hDC,
                                   TEXT_CAPS,
                                   strcountof (TEXT_CAPS),
                                  &sText);
            // Set the parts width
            glbStatusPartsWidth[iCnt] = sText.cx + SP_BORDER_EXTRA;
#undef  TEXT_CAPS
            break;

        case SP_TIMER:
#define TEXT_CAPS           L"99:99:99:999 ms"
            // Get the width of the text
            GetTextExtentPoint32W (hDC,
                                   TEXT_CAPS,
                                   strcountof (TEXT_CAPS),
                                  &sText);
            // Set the parts width
            glbStatusPartsWidth[iCnt] = sText.cx + SP_BORDER_EXTRA;
#undef  TEXT_CAPS
            break;

        case SP_TEXTMSG:
            glbStatusPartsWidth[iCnt] = 0;

            break;

        defstop
            break;
    } // End SWITCH

    // Back off by size of Gripper
    uWidth -= GRIPPER_WIDTH;

    // Adjust the Status Parts right edges
    for (iCnt = SP_LENGTH - 1; iCnt >= 0; iCnt--)
    if (glbStatusPartsWidth[iCnt] EQ 0)
        lclStatusPartsRight[iCnt] = uWidth;
    else
    {
        lclStatusPartsRight[iCnt] = uWidth;
        uWidth -= (glbStatusBorders[2] + abs (glbStatusPartsWidth[iCnt]));
    } // End FOR/IF/ELSE/...

    // Tell the status window about the new Status Parts right edge
    SendStatusMsg (SB_SETPARTS, SP_LENGTH, (LPARAM) lclStatusPartsRight);

    // We no longer need this resource
    MyReleaseDC (hWndStatus, hDC); hDC = NULL;

    // Get the active MDI Child window handle (if any)
    hWndEC  = GetActiveEC (hWndTC);
    if (hWndEC)
        // Tell the Status Window about the new positions
        SetStatusPos (hWndEC);
} // End SetStatusParts


//***************************************************************************
//  $SetStatusAll
//
//  Initialize all status msgs
//***************************************************************************

void SetStatusAll
    (void)

{
    HWND    hWndEC  = GetActiveEC (hWndTC);
    VKSTATE vkState = GetVkState (hWndEC);

    SetStatusIns   (vkState.Ins);
    SetStatusCaps  (GetKeyState (VK_CAPITAL) & BIT0);
    SetStatusNum   (GetKeyState (VK_NUMLOCK) & BIT0);
    SetStatusPos   (hWndEC);
    SetStatusTimer (0);
} // End SetStatusAll


//***************************************************************************
//  $SetStatusIns
//
//  Set the state of the Ins key in the Status Window
//***************************************************************************

void SetStatusIns
    (UBOOL bInsKey)             // TRUE iff the Ins key is ON

{
    SendStatusMsg (SB_SETTEXTW, SP_INS, (LPARAM) (bInsKey ? WS_HT L"INS" : WS_HT L"OVR"));
} // End SetStatusIns


//***************************************************************************
//  $SetStatusCaps
//
//  Set the state of the CapsLock key in the Status Window
//***************************************************************************

void SetStatusCaps
    (UBOOL bCapsKey)            // TRUE iff the CapsLock key is ON

{
    SendStatusMsg (SB_SETTEXTW, SP_CAPS, (LPARAM) (bCapsKey ? WS_HT L"CAPS" : L""));
} // End SetStatusCaps


//***************************************************************************
//  $SetStatusNum
//
//  Set the state of the NumLock key in the Status Window
//***************************************************************************

void SetStatusNum
    (UBOOL bNumKey)             // TRUE iff the NumLock key is ON

{
    SendStatusMsg (SB_SETTEXTW, SP_NUM, (LPARAM) (bNumKey ? WS_HT L"NUM" : L""));
} // End SetStatusNum


//***************************************************************************
//  $SetStatusPos
//
//  Set the state of the Line and char positions in the Status Window
//***************************************************************************

void SetStatusPos
    (HWND hWndEC)               // Edit Ctrl window handle

{
    APLU3264 uCharPos,              // Character position (origin-0), initially from start
                                    //   of buffer then eventually from the start of the line
             uLineNum,              // Line # (origin-0)
             uLinePos;              // Line position from start of buffer
    WCHAR    szTemp[32];            // Temp format save area

    // Get the indices of the selected text (if any)
    SendMessageW (hWndEC, EM_GETSEL, (WPARAM) &uCharPos, 0);

    // Get the line # of this char
    uLineNum = (APLU3264) SendMessageW (hWndEC, EM_LINEFROMCHAR, uCharPos, 0);

    // Get the char position of the start of the current line
    uLinePos = (APLU3264) SendMessageW (hWndEC, EM_LINEINDEX, uLineNum, 0);

    // Get the character position from the start of the line
    uCharPos -= uLinePos;

    // Format the line #
    wsprintfW (szTemp,
               L"%u",
               uLineNum);
    SendStatusMsg (SB_SETTEXTW, SP_LINEPOS, (LPARAM) szTemp);

    // Format the char #
    wsprintfW (szTemp,
               L"%u",
               uCharPos);
    SendStatusMsg (SB_SETTEXTW, SP_CHARPOS, (LPARAM) szTemp);
} // End SetStatusPos


//***************************************************************************
//  $SetStatusTimer
//
//  Set the execution timer value in the Status Window
//***************************************************************************

void SetStatusTimer
    (APLINT aplTicks)           // # milliseconds

{
    static WCHAR wszTemp[32];   // Temp storage
    UINT  hrs,                  // Hours
          mins,                 // Minutes
          secs,                 // Seconds
          ms;                   // Milliseconds

    // Split the value into hrs:min:sec:ms
    ms   = aplTicks % 1000; aplTicks = (aplTicks - ms  ) / 1000;
    secs = aplTicks %   60; aplTicks = (aplTicks - secs) /   60;
    mins = aplTicks %   60; aplTicks = (aplTicks - mins) /   60;
    hrs  = (UINT) aplTicks;

    // Format the number
    if (hrs)
        wsprintfW (wszTemp,
                //   00:00:99:999 ms
                   L"%u:%02u:%02u:%03u ms",
                   hrs, mins, secs, ms);
    else
    if (mins)
        wsprintfW (wszTemp,
                //   00:00:99:999 ms
                   L"   %2u:%02u:%03u ms",
                        mins, secs, ms);
    else
    if (secs)
        wsprintfW (wszTemp,
                //   00:00:99:999 ms
                   L"      %2u:%03u ms",
                              secs, ms);
    else
        wsprintfW (wszTemp,
                //   00:00:00:999 ms
                   L"         %3u ms",
                                    ms);
    // Send the message to the window
    SendMessageW (hWndStatus, SB_SETTEXTW, SP_TIMER, (LPARAM) wszTemp);
} // End SetStatusTimer


//***************************************************************************
//  $UpdateStatusTimer
//
//  Update the status timer
//***************************************************************************

void UpdateStatusTimer
    (LPPERTABDATA lpMemPTD)     // Ptr to PerTabData global memory

{
    LARGE_INTEGER liTickCnt,            // Current tick count
                  liTicksPerSec;        // # ticks per second
    APLFLOAT      aplScale;             // Scale factor

    // Get current tick count
    QueryPerformanceCounter (&liTickCnt);

    // Get # ticks per second
    QueryPerformanceFrequency (&liTicksPerSec);

    // Calculate the scale factor for milliseconds
    aplScale = 1000.0 / (APLFLOAT) (APLINT) liTicksPerSec.QuadPart;

    // Convert the tick count into milliseconds
    SetStatusTimer ((APLINT) floor (aplScale * (APLFLOAT) (liTickCnt.QuadPart - lpMemPTD->liTickCnt.QuadPart)));
} // End UpdateStatusTimer


//***************************************************************************
//  $SetStatusMsg
//
//  Set the Status Window message
//***************************************************************************

void SetStatusMsg
    (LPWCHAR lpwszMsg)          // Ptr to Status Window msg

{
    SendStatusMsg (SB_SETTEXTW, SP_TEXTMSG, (LPARAM) lpwszMsg);
    SetPropW (hWndStatus, PROP_STATUSMSG, (HANDLE) lpwszMsg);
} // End SetStatusMsg


//***************************************************************************
//  $SetExecuting
//
//  Set/Reset execution status
//***************************************************************************

void SetExecuting
    (LPPERTABDATA lpMemPTD,     // Ptr to PerTabData global memory
     UBOOL        bExecuting)   // TRUE iff we're executing

{
    // If old & new the states are different, ...
    if (lpMemPTD->bExecuting NE bExecuting)
    {
        // If the new state is ON, ...
        if (bExecuting)
        {
            // Clear the timer status msg to 0
            SetStatusTimer (0);

            // Get current tick count
            QueryPerformanceCounter (&lpMemPTD->liTickCnt);

            // Start a timer for once per second
            SetTimer (lpMemPTD->hWndSM, ID_TIMER_EXEC, 1000, NULL);
        } else
        {
            // Update the status timer
            UpdateStatusTimer (lpMemPTD);

            // Clear the timer
            KillTimer (lpMemPTD->hWndSM, ID_TIMER_EXEC);
        } // End IF/ELSE

        // Set the current state
        lpMemPTD->bExecuting = bExecuting;
    } // End IF
} // End SetExecuting


//***************************************************************************
//  $LclStatusBarWndProc
//
//  Subclassed StatusBar window procedure
//***************************************************************************

LRESULT WINAPI LclStatusBarWndProc
    (HWND   hWnd,       // Window handle
     UINT   message,    // Type of message
     WPARAM wParam,     // Additional information
     LPARAM lParam)     // ...

{
    static RECT    rcParts[SP_LENGTH];
    static int     iParts = -1;             // Index of current part
    TOOLINFOW      tti;                     // For Tooltip Ctrl tracking
    int            iCnt;                    // Loop counter
    LRESULT        lResult;                 // The WndProc result
    POINT          ptCursor;                // Position of the mouse cursor
    static HWND    hWndTT;                  // Tooltip window handle
    MSG            msg;                     // Used to hold message info for TTM_RELAYEVENT

////LCLODSAPI ("SB: ", hWnd, message, wParam, lParam);

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
////tti.rect     =                      // Not used with TTF_IDISHWND
////tti.hinst    =                      // Not used except with string resources
    tti.lpszText = LPSTR_TEXTCALLBACKW;
    tti.lParam   = 0;

    // Split cases
    switch (message)
    {
        case WM_CREATE:
            // If the Tooltip window has not been created as yet, ...
            if (hWndTT EQ NULL)
            {
                // Create it
                hWndTT = CreateTooltip (TRUE);
                if (hWndTT EQ NULL)
                    return -1;      // Mark as in error
            } // End IF

            break;                  // Continue with the next handler

        case WM_SIZE:
            // Send the message to the baseclass so that the rectangles
            //   are properly sized
            lResult =
              CallWindowProcW (lpfnOldStatusBarWndProc,
                               hWnd,
                               message,
                               wParam,
                               lParam);     // Pass on down the line
            // Loop through the parts filling in the rectangle coords
            for (iCnt = 0; iCnt < SP_LENGTH; iCnt++)
                // Get the rectangle of the part
                CallWindowProcW (lpfnOldStatusBarWndProc,   // Ptr to old WndProc
                                 hWnd,                      // Window handle
                                 SB_GETRECT,                // Message
                                 iCnt,                      // Index of part
                       (LPARAM) &rcParts[iCnt]);            // Ptr to receiving rectangle
            return lResult;         // Return the baseclass result

        case WM_NOTIFY:             // idCtl = (int) wParam;
                                    // pnmh = (LPNMHDR) lParam;
        {
#ifdef DEBUG
            LPNMHDR lpnmh = *(LPNMHDR *) &lParam;
#else
#define lpnmh   (*(LPNMHDR *) &lParam)
#endif
            // Split cases based upon the notification code
            switch (lpnmh->code)
            {
////////////////case TTN_NEEDTEXTW:
                case TTN_GETDISPINFOW:  // idCtl    = (int) wParam;
                                        // lpnmttdi = (LPNMTTDISPINFOW) lParam;
                {
#ifdef DEBUG
                    LPNMTTDISPINFOW lpnmttdi = *(LPNMTTDISPINFOW *) &lParam;
#else
#define lpnmttdi    (*(LPNMTTDISPINFOW *) &lParam)
#endif
                    // Get the mouse position in screen coords
                    GetCursorPos (&ptCursor);

                    // Convert from screen to client coordinates
                    ScreenToClient (hWnd, &ptCursor);

                    // Loop through the parts rectangles looking for a hit
                    for (iCnt = 0; iCnt < SP_LENGTH; iCnt++)
                    if (PtInRect (&rcParts[iCnt], ptCursor))
                    {
                        // Save the current parts index
                        iParts = iCnt;

                        // Return the ptr to the caller
                        lpnmttdi->lpszText = lpwszToolTips[iCnt];

                        break;
                    } // End IF
#ifndef DEBUG
#undef  lpnmttdi
#endif
                    return FALSE;           // We handled the message
                } // End TTN_GETDISPINFOW

                case TTN_POP:               // lpnmh = (LPNMHDR) lParam;
                    // Save the current state
                    iCnt = iParts;

                    // Mark as inactive
                    iParts = -1;

                    // If we were active, ...
                    if (iCnt NE -1)
                        // Detach the Tooltip window from it
                        SendMessageW (hWndTT, TTM_DELTOOLW, 0, (LPARAM) &tti);

                    return FALSE;           // We handled the message

                default:
                    break;
            } // End SWITCH

            break;
        } // End WM_NOTIFY

        case WM_MOUSEMOVE:          // fwKeys = wParam;         // key flags
                                    // xPos = LOSHORT(lParam);  // horizontal position of cursor (CA)
                                    // yPos = HISHORT(lParam);  // vertical position of cursor (CA)
            // Save in a POINT struc
            ptCursor.x = LOSHORT (lParam);
            ptCursor.y = HISHORT (lParam);

            // If we're inactive, ...
            if (iParts EQ -1)
            {
                // Loop through the parts rectangles looking for a hit
                for (iCnt = 0; iCnt < SP_LENGTH; iCnt++)
                if (PtInRect (&rcParts[iCnt], ptCursor))
                {
                    // Save the current parts index
                    iParts = iCnt;

                    // If it's not the SP_TEXTMSG field, ...
                    if (iParts NE SP_TEXTMSG)
                    {
                        // Set the horizontal coord to the midpoint of the field
                        msg.pt.x = (rcParts[iParts].left + rcParts[iParts].right) / 2;

                        // Set the vertical coord to the top
                        msg.pt.y = rcParts[iParts].top;

                        // Convert from client to screen coordinates
                        ClientToScreen (hWnd, &msg.pt);
                    } else
                        // Get the mouse position in screen coords
                        GetCursorPos (&msg.pt);

                    // Fill in the MSG struc
                    msg.hwnd    = hWnd;
                    msg.message = message;
                    msg.wParam  = wParam;
                    msg.lParam  = lParam;
                    msg.time    = GetMessageTime ();
////////////////////msg.pt.x    =                       // Already filled in
////////////////////msg.pt.y    =                       // ...

                    // Attach the Tooltip window to it
                    SendMessageW (hWndTT, TTM_ADDTOOLW, 0, (LPARAM) &tti);

                    // Activate tracking
                    SendMessageW (hWndTT, TTM_TRACKACTIVATE, TRUE, (LPARAM) &tti);

                    // Position the tooltip
                    SendMessageW (hWndTT, TTM_TRACKPOSITION, 0, MAKELPARAM (msg.pt.x, msg.pt.y));

                    break;
                } // End IF
            } else
            if (!PtInRect (&rcParts[iParts], ptCursor)) // If this point is outside the current rectangle, ...
            {
                // Mark as inactive
                iParts = -1;

                // De-activate tracking
                SendMessageW (hWndTT, TTM_TRACKACTIVATE, FALSE, (LPARAM) &tti);

                // Pop the balloon
                SendMessageW (hWndTT, TTM_POP, 0, 0);

                // Detach the Tooltip window from it
                SendMessageW (hWndTT, TTM_DELTOOLW, 0, (LPARAM) &tti);
            } // End IF/ELSE/...

            break;

        case WM_DESTROY:
            // If the Tooltip window is still present, ...
            if (hWndTT)
            {
                // Destroy it
                DestroyWindow (hWndTT); hWndTT = NULL;
            } // End IF

            break;

        default:
            break;                  // Continue with the next handler
    } // End SWITCH

////LCLODSAPI ("SBZ: ", hWnd, message, wParam, lParam);
    return
      CallWindowProcW (lpfnOldStatusBarWndProc,
                       hWnd,
                       message,
                       wParam,
                       lParam);     // Pass on down the line
} // End LclStatusBarWndProc


//***************************************************************************
//  End of File: statusbar.c
//***************************************************************************
