//***************************************************************************
//  NARS2000 -- Session Manager
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
#include "headers.h"


/*

The Session Manager (SM) window consists of an Edit Ctrl which
holds the APL statements of the session.

When the cursor moves to a line, the contents of the current line
are copied to <lpwCurLine>.

If the user edits the line:
    * The edit changes are saved in the Edit Ctrl.

In any case,
    * If the user hits Enter, the contents of the current
      line in the Edit Ctrl are copied to the last line
      in the Edit Ctrl, the contents of <lpwCurLine>
      replace the current line in the Edit Ctrl, and the
      last line in the Edit Ctrl is executed.

 */

////LPTOKEN lptkStackBase;          // Ptr to base of token stack used in parsing

// MDI WM_NCCREATE & WM_CREATE parameter passing convention
//
// When calling CreateMDIWindowW with an extra data parameter
//   of (say) &lpMemPTD, the window procedure receives the data
//   in the following struc:
//
//      typedef struct tagSM_CREATESTRUCTW
//      {
//          LPPERTABDATA lpMemPTD;
//      } UNALIGNED SM_CREATESTRUCTW, *LPSM_CREATESTRUCTW;
//
//   which is used as follows:
//
//      #define lpMDIcs     ((LPMDICREATESTRUCTW) (((LPCREATESTRUCTW) lParam)->lpCreateParams))
//      lpMemPTD = ((LPSM_CREATESTRUCTW) (lpMDIcs->lParam))->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));
//      #undef  lpMDIcs

APLCHAR wszQuadInput[] = WS_UTF16_QUAD L":";
#ifdef DEBUG
UBOOL gPrompt = FALSE;
#endif


//***************************************************************************
//  $SetAttrs
//
//  Set attributes for a DC
//***************************************************************************

void SetAttrs
    (HDC      hDC,              // Handle to the device context
     HFONT    hFont,            // Handle to the font (may be NULL)
     COLORREF crfg,             // Foreground text color
     COLORREF crbk)             // Background ...

{
    // Set the mapping mode
    SetMapMode (hDC, MM_TEXT);

    // Select the font into the DC
    if (hFont NE NULL)
        SelectObject (hDC, hFont);

    // Set the color of the foreground text
    SetTextColor (hDC, crfg);

    // Set the color of the background text
    SetBkColor   (hDC, crbk);
} // End SetAttrs


//***************************************************************************
//  $GetThreadSMEC
//
//  Get the hWndEC for the Session Manager from the current thread
//***************************************************************************

HWND GetThreadSMEC
    (void)

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Return the handle to the Edit Ctrl
    return (HWND) GetWindowLongPtrW (lpMemPTD->hWndSM, GWLSF_HWNDEC);
} // End GetThreadSMEC


//***************************************************************************
//  $AppendLine
//
//  Append lpwszLine to the history buffer
//***************************************************************************

void AppendLine
    (LPWCHAR lpwszLine,         // Ptr to the line to append
     UBOOL   bLineCont,         // TRUE iff this is a line continuation
     UBOOL   bEndingCRLF)       // TRUE iff this line should end with a CR/LF

{
    HWND hWndEC;                // Window handle to Edit Ctrl

    // Get hWndEC for the Session Manager from the current thread
    hWndEC = GetThreadSMEC ();

    // Move the text caret to the end of the buffer
    MoveCaretEOB (hWndEC);

    // Scroll the caret into view
    SendMessageW (hWndEC, EM_SCROLLCARET, 0, 0);

    dprintfWL9 (L"AppendLine: <%s> (%S#%d)", lpwszLine, FNLN);

    // Replace the selection (none) with the new line
    SendMessageW (hWndEC, EM_REPLACESEL, FALSE, (LPARAM) lpwszLine);

    // If requested, end the line
    if (bEndingCRLF)
        // Replace the selection (none) with WS_CRLF
        SendMessageW (hWndEC, EM_REPLACESEL, FALSE, (LPARAM) WS_CRLF);
#ifdef DEBUG
    // To aid in debugging, output the text immediately
    UpdateWindow (hWndEC);
#endif
    PERFMON
    PERFMONSHOW (NULL)
} // End AppendLine


//***************************************************************************
//  $ReplaceLine
//
//  Replace the current line in the history buffer
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ReplaceLine"
#else
#define APPEND_NAME
#endif

void ReplaceLine
    (HWND    hWndEC,            // Edit Ctrl window handle
     LPWCHAR lpwszLine,         // Ptr to incoming line text
     UINT    uLineNum)          // Line # to replace

{
    UINT uLinePos,
         uLineLen;

    // Get the line position of the given line
    uLinePos = (UINT) SendMessageW (hWndEC, EM_LINEINDEX, uLineNum, 0);

    // Get the length of the line
    uLineLen = (UINT) SendMessageW (hWndEC, EM_LINELENGTH, uLinePos, 0);

    // Set the selection to this line
    SendMessageW (hWndEC, EM_SETSEL, uLinePos, uLinePos + uLineLen);

    dprintfWL9 (L"ReplaceLine: %d:<%s> (%S#%d)", uLineNum, lpwszLine, FNLN);

    // Replace the selection with the given line
    SendMessageW (hWndEC, EM_REPLACESEL, FALSE, (LPARAM) lpwszLine);
} // End ReplaceLine
#undef  APPEND_NAME


//***************************************************************************
//  $ReplacelastLineCR
//
//  Replace the last line in the history buffer
//    and move the text caret to the next line
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ReplacelastLineCR"
#else
#define APPEND_NAME
#endif

void ReplaceLastLineCR
    (LPWCHAR lpwszLine)         // Ptr to incoming line text

{
    HWND hWndEC;                // Window handle to Edit Ctrl

    // Get hWndEC for the Session Manager from the current thread
    hWndEC = GetThreadSMEC ();

    // Move the text caret to the end of the buffer
    MoveCaretEOB (hWndEC);

    dprintfWL9 (L"ReplaceLastLineCR: <%s> (%S#%d)", lpwszLine, FNLN);

    // Replace the current (now last) line
    ReplaceLine (hWndEC, lpwszLine, NEG1U);

    // Move the text caret to the next line
    AppendLine (L"", FALSE, TRUE);
} // End ReplacelastLineCR
#undef  APPEND_NAME


//***************************************************************************
//  $ReplaceLastLineCRPmt
//
//  Replace the last line in the history buffer
//    move the text caret to the next line, and
//    display a prompt if this is not the active tab
//***************************************************************************

void ReplaceLastLineCRPmt
    (LPWCHAR lpwszLine)         // Ptr to incoming line text

{
    dprintfWL9 (L"ReplaceLastLineCRPmt: <%s> (%S#%d)", lpwszLine, FNLN);

    // Replace the last line
    ReplaceLastLineCR (lpwszLine);

    // If this is not the active tab, display a prompt
    if (!IsCurTabActive ())
        DisplayPrompt (GetThreadSMEC (), 8);
} // End ReplaceLastLineCRPmt


//***************************************************************************
//  $IzitLastLine
//
//  Return TRUE iff the cursor is on the last line
//***************************************************************************

UBOOL IzitLastLine
    (HWND hWndEC)           // Window handle of the Edit Ctrl

{
    UINT uLineNum,          // Current line #
         uLineEnd,          // Ending line of the block containing uLineNum
         uLineCnt;          // # lines in the Edit Ctrl

    // Get the current line # (origin-0)
    uLineNum = (DWORD) SendMessageW (hWndEC, EM_LINEFROMCHAR, (WPARAM) -1, 0);

    // Get the last line # in this block (origin-0)
    uLineEnd = GetBlockEndLine (hWndEC, uLineNum);

    // Get the # lines in the text
    uLineCnt = (UINT) SendMessageW (hWndEC, EM_GETLINECOUNT, 0, 0);

    return (uLineEnd EQ (uLineCnt - 1));
} // End IzitlastLine


//***************************************************************************
//  $WidthLC
//
//  Calculate the width of the Line Continuation marker
//***************************************************************************

UINT WidthLC
    (FONTENUM fontEnum)             // Font enum index (see FONTENUM)

{
    HDC   hDC;                      // Temporary DC
    RECT  rcScr = {0};              // Screen rectangle
    WCHAR wcLC[2];                  // Temp save area for LC marker
    HFONT hFontOld;                 // Old font handle

    // Get a device context
    hDC = MyGetDC (HWND_DESKTOP);

    // Save LC char as string
    wcLC[0] = uUserChar;
    wcLC[1] = WC_EOS;

    // Set the mapping mode
    SetMapMode (hDC, MM_TEXT);

    // Install the appropriate font
    hFontOld = SelectObject (hDC, GetFSIndFontHandle (fontEnum));

    // Calculate the width & height of the Line Continuation marker
    //   in screen coordinates
    DrawTextW (hDC,
               wcLC,
               1,
              &rcScr,
               0
             | DT_CALCRECT
             | DT_NOPREFIX
             | DT_NOCLIP);
    // Restore the old font
    SelectObject (hDC, hFontOld);

    // We no longer need this resource
    MyReleaseDC (HWND_DESKTOP, hDC); hDC = NULL;

    return rcScr.right - rcScr.left;
} // End WidthLC


//***************************************************************************
//  $DrawAllLineCont
//
//  Draw all line continuation markers
//***************************************************************************

void DrawAllLineCont
    (HWND hWndEC)                   // Edit Ctrl window handle

{
    HWND     hWndParent;            // Parent window handle
    UINT     uLineCnt,              // # lines in the Edit Ctrl
             uLineTop,              // # of topmost visible line
             uLeft;                 // Left margin in rcLC
    HDC      hDC;                   // Window DC
    RECT     rcLC;                  // Rectangle for the Line Continuation markers
    FONTENUM fontEnum;              // Font enum index
    HBRUSH   hBrush;                // Background brush

    // Get the handle to the parent window (hWndFE or hWndSM)
    hWndParent = GetParent (hWndEC);

    // If our parent is SM, ...
    if (IzitSM (hWndParent))
    {
        fontEnum = FONTENUM_SM;
        uLeft = 0;
    } else
    if (IzitFE (hWndParent))
    {
        fontEnum = FONTENUM_FE;
        uLeft = LftMarginsFE (hWndEC, FALSE);
#ifdef DEBUG
    } else
    {
        DbgBrk ();
#endif
    } // End IF/ELSE/...

    // Get the window DC
    hDC = MyGetDC (hWndEC);

    // Get the # lines in the text
    uLineCnt = (UINT) SendMessageW (hWndEC, EM_GETLINECOUNT, 0, 0);

    // Get the # of the topmost visible line
    uLineTop = (UINT) SendMessageW (hWndEC, EM_GETFIRSTVISIBLELINE, 0, 0);

    // Get the client rectangle
    GetClientRect (hWndEC, &rcLC);

    // Fill in the rest of the rectangle
    rcLC.left  = uLeft;
    rcLC.right = rcLC.left + uWidthLC[fontEnum];

    // If the background color is transparent, ...
    if (gSyntaxColorLC.crBack EQ DEF_SCN_TRANSPARENT)
    {
        // Get the background brush
        hBrush = (HBRUSH) GetClassLongPtrW (hWndEC, GCLP_HBRBACKGROUND);

        // Fill in the rectangle with the background color
        FillRect (hDC, &rcLC, hBrush);
    } else
    {
        // Create a brush with the background color
        hBrush = MyCreateSolidBrush (gSyntaxColorName[SC_LINECONT].syntClr.crBack);

        // Fill in the rectangle with the background color
        FillRect (hDC, &rcLC, hBrush);

        // We no longer need this resource
        MyDeleteObject (hBrush); hBrush = NULL;
    } // End IF/ELSE

    // We no longer need this resource
    MyReleaseDC (hWndEC, hDC); hDC = NULL;

    // Ensure topmost line has a preceding line
    uLineTop = max (uLineTop, 1);

    // Loop through the line #s
    for (; uLineTop < uLineCnt; uLineTop++)
    // If the preceding physical line continues to the current line, ...
    if (SendMessageW (hWndEC, MYEM_ISLINECONT, uLineTop - 1, 0) EQ TRUE)
        // Draw a Line Continuation marker
        DrawLineCont (hWndEC, uLineTop);
} // End DrawAllLineCont


//***************************************************************************
//  $DrawLineCont
//
//  Draw a line continuation marker
//***************************************************************************

void DrawLineCont
    (HWND  hWndEC,                  // Edit Ctrl window handle
     UINT  uLineNum)                // Line number

{
    HWND         hWndParent;        // Parent window handle
    HDC          hDC;               // Screen DC
    FONTENUM     fontEnum;          // Font enum index
    SIZE         charSize;          // cx & cy of the average char in the font
    UINT         uLineTop,          // # of topmost visible line
                 uLeft;             // Left margin in rcLC
    RECT         rcLC;              // Rectangle for the Line Continuation char
    HFONT        hFontOld;          // Handle to the old font

    // Get the handle to the parent window (hWndFE or hWndSM)
    hWndParent = GetParent (hWndEC);

    // If our parent is SM, ...
    if (IzitSM (hWndParent))
    {
        fontEnum = FONTENUM_SM;
        uLeft    = 0;
    } else
    if (IzitFE (hWndParent))
    {
        fontEnum = FONTENUM_FE;
        uLeft    = LftMarginsFE (hWndEC, FALSE);
#ifdef DEBUG
    } else
    {
        DbgBrk ();
#endif
    } // End IF/ELSE/...

    // Get a DC for the Edit Ctrl
    hDC = MyGetDC (hWndEC);

    // Put the fontEnum font into effect
    hFontOld = SelectObject (hDC, GetFSIndFontHandle (fontEnum));

    // Get the font average char size
    charSize = *GetFSIndAveCharSize (fontEnum);

    // Get the # of the topmost visible line
    uLineTop = (UINT) SendMessageW (hWndEC, EM_GETFIRSTVISIBLELINE, 0, 0);

    // Fill in the top/left of the RECT
    rcLC.top  = (uLineNum - uLineTop) * charSize.cy;
    rcLC.left = uLeft + uWidthLC[fontEnum];

    // Handle via subroutine
    DrawLineContSub (hWndEC, hDC, rcLC.left, rcLC.top, fontEnum EQ FONTENUM_PR);

    // Restore the old font
    SelectObject (hDC, hFontOld);

    // We no longer need this resource
    MyReleaseDC (hWndEC, hDC); hDC = NULL;
} // End DrawLineCont


//***************************************************************************
//  $DrawLineContSub
//
//  Subroutine to DrawLineCont
//***************************************************************************

void DrawLineContSub
    (HWND  hWndEC,              // Edit Ctrl window handle
     HDC   hDC,                 // Device context
     int   x,                   // Left side of RECT (NOT including the LC marker)
     int   y,                   // Top  ...
     UBOOL bPrinting)           // TRUE iff we're printing

{
    SIZE     charSize;          // cx & cy of the average char in the font
    RECT     rcLC;              // Rectangle for the Line Continuation char
    UBOOL    bSyntClr;          // TRUE iff we're Syntax Coloring
    FONTENUM fontEnum;          // FONTENUM index
    HFONT    hFontOld;          // Old font (to be resored)
    UINT     uWidth;            // Width of the LC marker
    int      mmOld;             // Old mapping mode
    SIZE     GLP_DC,            // GetLogPixels[X|Y] (hDC)
             GLP_NULL;          // GetLogPixels[X|Y] (NULL)
    HFONT    hFontCur;          // Current font

    // Set the mapping mode
    mmOld = SetMapMode (hDC, MM_TEXT);

    //If we're printing, ...
    if (bPrinting)
    {
        LOGFONTW    lfPR;       // LOGFONT for PR
        TEXTMETRICW tmPR;       // TEXTMETRICWs for the Printer Font

        fontEnum = FONTENUM_PR;
        bSyntClr = OptionFlags.bSyntClrPrnt;

        // Get the font average char size
        charSize = *GetFSIndAveCharSize (fontEnum);

        // Get the width of the LC marker
        uWidth = uWidthLC[fontEnum];

        // Get the current font for PR
        hFontCur = *fontStruc[glbSameFontAs[fontEnum]].lphFont;

        // Get the LOGFONTW structure for the font
        GetObjectW (hFontCur, sizeof (lfPR), &lfPR);

        // Get the log pixels for DC and NULL
////    GLP_DC.cx   = GetLogPixelsX (hDC);
        GLP_DC.cy   = GetLogPixelsY (hDC);
////    GLP_NULL.cx = GetLogPixelsX (NULL);
        GLP_NULL.cy = GetLogPixelsY (NULL);

        // Respecify the horizontal & vertical positions in printer coordinates
        lfPR.lfHeight = MulDiv (lfPR.lfHeight, GLP_DC.cy, GLP_NULL.cy);

        // Make a font of it
        hFontCur = MyCreateFontIndirectW (&lfPR);

        // Install the current font
        hFontOld = SelectObject (hDC, hFontCur);

        // Get the text metrics for this font
        GetTextMetricsW (hDC, &tmPR);

        // Convert the x & y & cx & cy values from screen coords to printer coords
        x             = MulDiv (x            , tmPR.tmAveCharWidth, charSize.cx);
        y             = MulDiv (y            , tmPR.tmHeight      , charSize.cy);
        uWidth        = MulDiv (uWidth       , tmPR.tmAveCharWidth, charSize.cx);

        charSize.cx   = MulDiv (charSize.cx  , tmPR.tmAveCharWidth, charSize.cx);
        charSize.cy   = MulDiv (charSize.cy  , tmPR.tmHeight      , charSize.cy);
    } else
    {
        // If our parent is SM, ...
        if (IzitSM (GetParent (hWndEC)))
        {
            fontEnum = FONTENUM_SM;
            bSyntClr = OptionFlags.bSyntClrSess;
        } else
        if (IzitFE (GetParent (hWndEC)))
        {
            fontEnum = FONTENUM_FE;
            bSyntClr = OptionFlags.bSyntClrFcns;
#ifdef DEBUG
        } else
        {
            DbgBrk ();
            bSyntClr = FALSE;
#endif
        } // End IF/ELSE/...

        // Get the font average char size
        charSize = *GetFSIndAveCharSize (fontEnum);

        // Get the width of the LC marker
        uWidth = uWidthLC[fontEnum];

        // Get the current font
        hFontCur = *fontStruc[glbSameFontAs[fontEnum]].lphFont;

        // Install the current font
        hFontOld = SelectObject (hDC, hFontCur);
    } // End IF/ELSE

    // Fill in the RECT
    rcLC.top    = y;
    rcLC.left   = x - uWidth;
    rcLC.right  = x;
    rcLC.bottom = rcLC.top  + charSize.cy;

    // If we're Syntax Coloring, ...
    if (bSyntClr)
    {
        // Set the foreground color
        SetTextColor (hDC, gSyntaxColorLC.crFore);

        // If the background color is not transparent, ...
        if (gSyntaxColorLC.crBack NE DEF_SCN_TRANSPARENT)
            // Set it
            SetBkColor (hDC, gSyntaxColorLC.crBack);
    } // End IF

    // Draw the Line Continuation marker
    DrawTextW (hDC,
    (LPWCHAR) &uUserChar,
               1,
              &rcLC,
               0
             | DT_SINGLELINE
             | DT_NOPREFIX
             | DT_VCENTER);
    // Restore the old font
    SelectObject (hDC, hFontOld);

    // Restore the old mapping mode
    SetMapMode (hDC, mmOld);

    // If we're printing, ...
    if (bPrinting)
    {
        // Delete the font we created
        DeleteObject (hFontCur); hFontCur = NULL;
    } // End IF
} // End DrawLineContSub


//***************************************************************************
//  $LftMarginsSM
//
//  Return the left margin for a Session Manager window
//***************************************************************************

UINT LftMarginsSM
    (void)

{
    // Make room for Line Continuation markers
    return uWidthLC[FONTENUM_SM];
} // End LftMarginsSM


//***************************************************************************
//  $SetMarginsSM
//
//  Set the margins for a Session Manager window
//***************************************************************************

void SetMarginsSM
    (HWND hWndEC)           // Window handle to the Edit Ctrl

{
    UINT uLeft;             // Left margin

    // Calculate the left margin for the SM
    uLeft = LftMarginsSM ();

    // Tell the Edit Ctrl about it
    SendMessageW (hWndEC, EM_SETMARGINS, EC_LEFTMARGIN, MAKELONG (uLeft, 0));
} // End SetMarginsSM


//***************************************************************************
//  $MoveCaretEOB
//
//  Move the text caret in an Edit Ctrl to the end of the buffer
//***************************************************************************

void MoveCaretEOB
    (HWND  hWndEC)          // Window handle of Edit Ctrl

{
    UINT uLineCnt,
         uLinePos,
         uLineLen,
         uCharPos;

    // Get the # lines in the text
    uLineCnt = (UINT) SendMessageW (hWndEC, EM_GETLINECOUNT, 0, 0);

    // Get the initial char pos of the last line (origin-0)
    uLinePos = (UINT) SendMessageW (hWndEC, EM_LINEINDEX, uLineCnt - 1, 0);

    // Get the length of the last line
    uLineLen = (UINT) SendMessageW (hWndEC, EM_LINELENGTH, uLinePos, 0);

    // Add to get char pos
    uCharPos = uLinePos + uLineLen;

    // Set the caret to the End-of-Buffer
    SendMessageW (hWndEC, EM_SETSEL, uCharPos, uCharPos);
} // End MoveCaretEOB


//***************************************************************************
//  $DisplayPrompt
//
//  Display the usual six-space prompt
//***************************************************************************

void DisplayPrompt
    (HWND     hWndEC,       // Window handle of the Edit Ctrl
     APLU3264 uCaller)      // ***DEBUG***

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

#ifdef DEBUG
    if (gPrompt)
        dprintfWL0 (L"~~DisplayPrompt (%d)", uCaller);
    else
        dprintfWL9 (L"~~DisplayPrompt (%d)", uCaller);
#endif

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Mark as no longer executing
    SetExecuting (lpMemPTD, FALSE);

    // Set the cursor to indicate the new state
    ForceSendCursorMsg (hWndEC, FALSE);

    // Check for exiting semaphore
    if (lpMemPTD->hExitphore NE NULL)
    {
        // Start executing at the Tab Delete code
        MyReleaseSemaphore (lpMemPTD->hExitphore, 1, NULL);

        return;
    } // End IF

    // Move the text caret to the end of the buffer
    MoveCaretEOB (hWndEC);

    // Display the indent
    AppendLine (wszIndent, FALSE, FALSE);

    PERFMON
////PERFMONSHOW (NULL)
} // End DisplayPrompt


//***************************************************************************
//  $GetLineLength
//
//  Return the line length of a given line #
//***************************************************************************

UINT GetLineLength
    (HWND     hWndEC,           // Edit Ctrl window handle
     APLU3264 uLineNum)         // The line #

{
    UINT uLinePos;              // Char position of start of line

    // Get the position of the start of the line
    uLinePos = (UINT) SendMessageW (hWndEC, EM_LINEINDEX, uLineNum, 0);

    // Get the line length
    return (UINT) SendMessageW (hWndEC, EM_LINELENGTH, uLinePos, 0);
} // GetLineLength


//***************************************************************************
//  $FormatQQuadInput
//
//  Format QQ input and save in global memory
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FormatQQuadInput"
#else
#define APPEND_NAME
#endif

void FormatQQuadInput
    (UINT         uLineNum,     // Line #
     HWND         hWndEC,       // Handle of Edit Ctrl window
     LPPERTABDATA lpMemPTD)     // Ptr to PerTabData global memory

{
    UINT         uLineLen;      // Line length
    APLUINT      ByteRes;       // # bytes in the result
    HGLOBAL      hGlbRes;       // Result global memory handle
    LPAPLCHAR    lpMemRes;      // Ptr to result global memory
    LPPL_YYSTYPE lpYYRes;       // Ptr to the result

    // Get the line length of a given line #
    uLineLen = GetLineLength (hWndEC, uLineNum);

    // Calculate space needed for the result
    // N.B.:  max is needed because, in order to get the line,
    //        we need to tell EM_GETLINE the buffer size which
    //        takes up one APLCHAR (WORD) at the start of the buffer.
    ByteRes = CalcArraySize (ARRAY_CHAR, max (uLineLen, 1), 1);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock000 (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = uLineLen;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Save the dimension in the result
    *VarArrayBaseToDim (lpMemRes) = uLineLen;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemRes);

    // Tell EM_GETLINE maximum # chars in the buffer
    // Because we allocated space for max (uLineLen, 1)
    //   chars, we don't have to worry about overwriting
    //   the allocation limits of the buffer
    ((LPWORD) lpMemRes)[0] = (WORD) uLineLen;

    // Get the contents of the line
    SendMessageW (hWndEC, EM_GETLINE, uLineNum, (LPARAM) lpMemRes);

    // Replace leading Prompt Replacement chars
    if (lpMemPTD->cQuadPR NE CQUADPR_MT)
    {
        UINT QQPromptLen,   // Length of QQ prompt
             u;             // Loop counter

        // Get the length of the QQ prompt
        QQPromptLen = lpMemPTD->lpSISCur->QQPromptLen;

        // ***FIXME*** -- we're supposed to save the actual prompt
        //                and compare it with the chars after the
        //                user has responded to the request for
        //                Quote-Quad input.

        // Replace all prompt chars
        for (u = 0; u < QQPromptLen; u++)
            lpMemRes[u] = lpMemPTD->cQuadPR;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
    lpYYRes->tkToken.tkFlags.NoDisplay = TRUE;
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lpMemPTD->lpSISCur->lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lpMemPTD->lpSISCur->lptkFunc);
    // Make a PL_YYSTYPE NoValue entry
    lpYYRes = MakeNoValue_YY (lpMemPTD->lpSISCur->lptkFunc);
NORMAL_EXIT:
    // Save the result in PerTabData
    lpMemPTD->YYResExec = *lpYYRes;

    // Free the YYRes we allocated
    YYFree (lpYYRes); lpYYRes = NULL;

    Assert (lpMemPTD->lpSISCur->hSemaphore NE NULL);

    // Signal WaitForInput that we have a result
    MyReleaseSemaphore (lpMemPTD->lpSISCur->hSemaphore, 1, NULL);

    // Release our time slice so the released thread can act
    Sleep (0);
} // End FormatQQuadInput
#undef  APPEND_NAME


//***************************************************************************
//  $SM_Create
//
//  Perform window-specific initialization
//***************************************************************************

void SM_Create
    (HWND hWnd)

{
} // End SM_Create


//***************************************************************************
//  $SM_Delete
//
//  Perform window-specific uninitialization
//***************************************************************************

void SM_Delete
    (HWND hWnd)

{
} // End SM_Delete


//***************************************************************************
//  $SMWndProc
//
//  Message processing routine for the Session Manager window
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SMWndProc"
#else
#define APPEND_NAME
#endif

LRESULT APIENTRY SMWndProc
    (HWND   hWnd,       // Window handle
     UINT   message,    // Type of message
     WPARAM wParam,     // Additional information
     LPARAM lParam)     // ...

{
    HWND         hWndEC;                // Window handle to Edit Ctrl
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPWCHAR      lpwCurLine;            // Ptr to current line global memory
    LPUNDO_BUF   lpUndoBeg;             // Ptr to start of Undo Buffer
    static UBOOL bLoadMsgDisp = FALSE;  // TRUE iff )LOAD message has been displayed
    LPMEMVIRTSTR lpLclMemVirtStr;       // Ptr to local MemVirtStr

    // Get the handle to the Edit Ctrl
    (HANDLE_PTR) hWndEC = GetWindowLongPtrW (hWnd, GWLSF_HWNDEC);

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

////LCLODSAPI ("SM: ", hWnd, message, wParam, lParam);
    switch (message)
    {
#define lpMDIcs     ((LPMDICREATESTRUCTW) ((*(LPCREATESTRUCTW *) &lParam)->lpCreateParams))
        case WM_NCCREATE:               // 0 = (int) wParam
                                        // lpcs = (LPCREATESTRUCTW) lParam
            PERFMON
#ifndef UNISCRIBE
            // Initialize the OLE libraries
            CoInitialize (NULL);

            // Get FontLink ptr
            CoCreateInstance (&CLSID_CMultiLanguage,
                               NULL,
                               CLSCTX_ALL,
                              &IID_IMLangFontLink,
                               (void **) &lpMemPTD->lpFontLink);
#endif
            // Save the window handle
            lpMemPTD->hWndSM = hWnd;

            INIT_PERTABVARS

            // Allocate room for PTDMEMVIRT_LENGTH MemVirtStrs
            //  (see PTDMEMVIRTENUM)
            lpLclMemVirtStr =
              MyVirtualAlloc (NULL,                 // Any address (FIXED SIZE)
                              PTDMEMVIRT_LENGTH * sizeof (MEMVIRTSTR),
                              MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN,
                              PAGE_READWRITE);
            if (lpLclMemVirtStr EQ NULL)
            {
                // ***FIXME*** -- Display error msg
                MBW (L"SMWndProc/WM_NCCREATE:  MyVirtualAlloc for <lpLclMemVirtStr> failed");

                goto WM_NCCREATE_FAIL;
            } // End IF

            // Allocate room for PTDMEMVIRT_LFTSTK
            lpLclMemVirtStr[PTDMEMVIRT_LFTSTK].lpText   = "lpMemPTD->lpplLftStk in <SMWndProc>";
            lpLclMemVirtStr[PTDMEMVIRT_LFTSTK].IncrSize = DEF_LFTSTK_INCRNELM * sizeof (lpMemPTD->lpplLftStk[0]);
            lpLclMemVirtStr[PTDMEMVIRT_LFTSTK].MaxSize  = DEF_LFTSTK_MAXNELM  * sizeof (lpMemPTD->lpplLftStk[0]);
            lpLclMemVirtStr[PTDMEMVIRT_LFTSTK].IniAddr  = (LPVOID)
            lpMemPTD->lpplLftStk =
            lpMemPTD->lpplOrgLftStk =
              GuardAlloc (NULL,             // Any address
                          lpLclMemVirtStr[PTDMEMVIRT_LFTSTK].MaxSize,
                          MEM_RESERVE,
                          PAGE_READWRITE);
            if (!lpLclMemVirtStr[PTDMEMVIRT_LFTSTK].IniAddr)
            {
                // ***FIXME*** -- WS FULL before we got started???
                MBW (L"SMWndProc/WM_CREATE:  GuardAlloc for <lpplLftStk> failed");

                goto WM_CREATE_FAIL;    // Mark as failed
            } // End IF

            // Link this struc into the chain
            LinkMVS (&lpLclMemVirtStr[PTDMEMVIRT_LFTSTK]);

            // Commit the intial size
            MyVirtualAlloc (lpLclMemVirtStr[PTDMEMVIRT_LFTSTK].IniAddr,
                            DEF_LFTSTK_INITNELM * sizeof (lpMemPTD->lpplLftStk[0]),
                            MEM_COMMIT,
                            PAGE_READWRITE);

            // Allocate room for PTDMEMVIRT_RHTSTK
            lpLclMemVirtStr[PTDMEMVIRT_RHTSTK].lpText   = "lpMemPTD->lpplRhtStk in <SMWndProc>";
            lpLclMemVirtStr[PTDMEMVIRT_RHTSTK].IncrSize = DEF_RHTSTK_INCRNELM * sizeof (lpMemPTD->lpplRhtStk[0]);
            lpLclMemVirtStr[PTDMEMVIRT_RHTSTK].MaxSize  = DEF_RHTSTK_MAXNELM  * sizeof (lpMemPTD->lpplRhtStk[0]);
            lpLclMemVirtStr[PTDMEMVIRT_RHTSTK].IniAddr  = (LPVOID)
            lpMemPTD->lpplRhtStk =
            lpMemPTD->lpplOrgRhtStk =
              GuardAlloc (NULL,             // Any address
                          lpLclMemVirtStr[PTDMEMVIRT_RHTSTK].MaxSize,
                          MEM_RESERVE,
                          PAGE_READWRITE);
            if (!lpLclMemVirtStr[PTDMEMVIRT_RHTSTK].IniAddr)
            {
                // ***FIXME*** -- WS FULL before we got started???
                MBW (L"SMWndProc/WM_CREATE:  GuardAlloc for <lpplRhtStk> failed");

                goto WM_CREATE_FAIL;    // Mark as failed
            } // End IF

            // Link this struc into the chain
            LinkMVS (&lpLclMemVirtStr[PTDMEMVIRT_RHTSTK]);

            // Commit the intial size
            MyVirtualAlloc (lpLclMemVirtStr[PTDMEMVIRT_RHTSTK].IniAddr,
                            DEF_RHTSTK_INITNELM * sizeof (lpMemPTD->lpplRhtStk[0]),
                            MEM_COMMIT,
                            PAGE_READWRITE);

            // Save in window extra bytes
            SetWindowLongPtrW (hWnd, GWLSF_LPMVS, (APLU3264) (LONG_PTR) lpLclMemVirtStr);

            PERFMON

            break;                  // Continue with next handler
WM_NCCREATE_FAIL:
            // Send a constant message to the previous tab
            SendMessageLastTab (ERRMSG_TABS_FULL APPEND_NAME, lpMemPTD);

            return -1;              // Mark as failed
#undef  lpMDIcs

#define lpMDIcs     ((LPMDICREATESTRUCTW) ((*(LPCREATESTRUCTW *) &lParam)->lpCreateParams))
        case WM_CREATE:             // 0 = (int) wParam
                                    // lpcs = (LPCREATESTRUCTW) lParam
        {
            LRESULT lResult = -1;       // The result (assume we failed)
            UBOOL   bRet;               // TRUE iff the result is valid

            PERFMON

            // Initialize # threads
            SetPropW (hWnd, PROP_NTHREADS, 0);

            // Save the ptr to this window's menu positions
            SetPropW (hWnd, PROP_IDMPOSFN, (HANDLE) GetIDMPOS_SM);

            // Initialize variables
            cfSM.hwndOwner = hWnd;

            // Initialize window-specific resources
            SM_Create (hWnd);

            // Get the ptr to the local virtual memory struc
            (HANDLE_PTR) lpLclMemVirtStr = GetWindowLongPtrW (hWnd, GWLSF_LPMVS);

            // *************** []ERROR/[]ES ****************************

            // Allocate virtual memory for the []ERROR/[]ES buffer
            lpLclMemVirtStr[PTDMEMVIRT_QUADERROR].lpText   = "lpMemPTD->lpwszQuadErrorMsg in <SMWndProc>";
            lpLclMemVirtStr[PTDMEMVIRT_QUADERROR].IncrSize = DEF_QUADERROR_INCRNELM * sizeof (lpMemPTD->lpwszQuadErrorMsg[0]);
            lpLclMemVirtStr[PTDMEMVIRT_QUADERROR].MaxSize  = DEF_QUADERROR_MAXNELM  * sizeof (lpMemPTD->lpwszQuadErrorMsg[0]);
            lpLclMemVirtStr[PTDMEMVIRT_QUADERROR].IniAddr  = (LPVOID)
            lpMemPTD->lpwszQuadErrorMsg =
              GuardAlloc (NULL,             // Any address
                          lpLclMemVirtStr[PTDMEMVIRT_QUADERROR].MaxSize,
                          MEM_RESERVE,
                          PAGE_READWRITE);
            if (lpLclMemVirtStr[PTDMEMVIRT_QUADERROR].IniAddr EQ NULL)
            {
                // ***FIXME*** -- WS FULL before we got started???
                MBW (L"SMWndProc/WM_CREATE:  GuardAlloc for <lpwszQuadErrorMsg> failed");

                goto WM_CREATE_FAIL;    // Mark as failed
            } // End IF

            // Link this struc into the chain
            LinkMVS (&lpLclMemVirtStr[PTDMEMVIRT_QUADERROR]);

            // Commit the intial size
            MyVirtualAlloc (lpLclMemVirtStr[PTDMEMVIRT_QUADERROR].IniAddr,
                            DEF_QUADERROR_INITNELM * sizeof (lpMemPTD->lpwszQuadErrorMsg[0]),
                            MEM_COMMIT,
                            PAGE_READWRITE);
            // *************** Undo Buffer *****************************
            // _BEG is the (static) ptr to the beginning of the virtual memory.
            // _NXT is the (dynamic) ptr to the next available entry.
            //    Undo entries are between _NXT[-1] and _BEG, inclusive.
            // _LST is the (dynamic) ptr to the last available entry.
            //    Redo entries are between _NXT and _LST[-1], inclusive.
            // *********************************************************

            // Allocate virtual memory for the Undo Buffer
            lpLclMemVirtStr[PTDMEMVIRT_UNDOBEG].lpText   = "lpUndoBeg in <SMWndProc>";
            lpLclMemVirtStr[PTDMEMVIRT_UNDOBEG].IncrSize = DEF_UNDOBUF_INCRNELM * sizeof (UNDO_BUF);
            lpLclMemVirtStr[PTDMEMVIRT_UNDOBEG].MaxSize  = DEF_UNDOBUF_MAXNELM  * sizeof (UNDO_BUF);
            lpLclMemVirtStr[PTDMEMVIRT_UNDOBEG].IniAddr  = (LPVOID)
            lpUndoBeg =
              GuardAlloc (NULL,             // Any address
                          lpLclMemVirtStr[PTDMEMVIRT_UNDOBEG].MaxSize,
                          MEM_RESERVE,
                          PAGE_READWRITE);
            if (lpLclMemVirtStr[PTDMEMVIRT_UNDOBEG].IniAddr EQ NULL)
            {
                // ***FIXME*** -- WS FULL before we got started???
                MBW (L"SMWndProc/WM_CREATE:  GuardAlloc for <lpUndoBeg> failed");

                goto WM_CREATE_FAIL;    // Mark as failed
            } // End IF

            // Link this struc into the chain
            LinkMVS (&lpLclMemVirtStr[PTDMEMVIRT_UNDOBEG]);

            // Commit the intial size
            MyVirtualAlloc (lpLclMemVirtStr[PTDMEMVIRT_UNDOBEG].IniAddr,
                            DEF_UNDOBUF_INITNELM * sizeof (lpUndoBeg[0]),
                            MEM_COMMIT,
                            PAGE_READWRITE);
            // Save in window extra bytes
            SetWindowLongPtrW (hWnd, GWLSF_UNDO_BEG, (APLU3264) (LONG_PTR) lpUndoBeg);
            SetWindowLongPtrW (hWnd, GWLSF_UNDO_NXT, (APLU3264) (LONG_PTR) lpUndoBeg);
            SetWindowLongPtrW (hWnd, GWLSF_UNDO_LST, (APLU3264) (LONG_PTR) lpUndoBeg);
////////////SetWindowLongW (hWnd, GWLSF_UNDO_GRP, 0);    // Already zero

            // Start with an initial action of nothing
            AppendUndo (hWnd,                       // SM Window handle
                        GWLSF_UNDO_NXT,             // Offset in hWnd extra bytes of lpUndoNxt
                        undoNone,                   // Action
                        0,                          // Beginning char position
                        0,                          // Ending    ...
                        UNDO_NOGROUP,               // Group index
                        0);                         // Character
            // Save incremented starting ptr in window extra bytes
            SetWindowLongPtrW (hWnd, GWLSF_UNDO_BEG, (APLU3264) (LONG_PTR) ++lpUndoBeg);

            // *************** lphtsPTD ********************************

            // Allocate virtual memory for the HSHTABSTR
            lpLclMemVirtStr[PTDMEMVIRT_HTSPTD].lpText   = "lpMemPTD->lphtsPTD in <SMWndProc>";
            lpLclMemVirtStr[PTDMEMVIRT_HTSPTD].IncrSize = 0;
            lpLclMemVirtStr[PTDMEMVIRT_HTSPTD].MaxSize  = sizeof (HSHTABSTR);
            lpLclMemVirtStr[PTDMEMVIRT_HTSPTD].IniAddr  = (LPVOID)
            lpMemPTD->lphtsPTD =
            lpMemPTD->lphtsGLB =
              GuardAlloc (NULL,             // Any address
                          lpLclMemVirtStr[PTDMEMVIRT_HTSPTD].MaxSize,
                          MEM_COMMIT | MEM_RESERVE,
                          PAGE_READWRITE);
            if (lpLclMemVirtStr[PTDMEMVIRT_HTSPTD].IniAddr EQ NULL)
            {
                // ***FIXME*** -- WS FULL before we got started???
                MBW (L"SMWndProc/WM_CREATE:  GuardAlloc for <lphtsPTD> failed");

                goto WM_CREATE_FAIL;    // Mark as failed
            } // End IF

            // Link this struc into the chain
            LinkMVS (&lpLclMemVirtStr[PTDMEMVIRT_HTSPTD]);

            // *************** lphtsPTD->lpHshTab **********************

            lpLclMemVirtStr[PTDMEMVIRT_HSHTAB].lpText = "lpMemPTD->lphtsPTD->lpHshTab in <SMWndProc>";

            // Allocate virtual memory for the hash table
            bRet = AllocHshTab (&lpLclMemVirtStr[PTDMEMVIRT_HSHTAB],    // Ptr to this entry in MemVirtStr
                                 lpMemPTD->lphtsPTD,                    // Ptr to this HSHTABSTR
                                 DEF_HSHTAB_NBLKS,                      // Initial # blocks in HshTab
                                 DEF_HSHTAB_INCRNELM,                   // # HTEs by which to resize when low
                                 gHshTabSize);                          // Maximum # HTEs
            if (!bRet)
            {
                // ***FIXME*** -- WS FULL before we got started???
                MBW (L"SMWndProc/WM_CREATE:  AllocHshTab for <lpMemPTD->lpHshTab> failed");

                goto WM_CREATE_FAIL;    // Mark as failed
            } // End IF

            // *************** lphtsPTD->lpSymTab **********************

            lpLclMemVirtStr[PTDMEMVIRT_SYMTAB].lpText = "lpMemPTD->lphtsPTD->lpSymTab in <SMWndProc>";

            // Allocate virtual memory for the symbol table
            bRet = AllocSymTab (&lpLclMemVirtStr[PTDMEMVIRT_SYMTAB],    // Ptr to this entry in MemVirtStr
                                 lpMemPTD,                              // Ptr to PerTabData global memory
                                 lpMemPTD->lphtsPTD,                    // Ptr to this HSHTABSTR
                                 DEF_SYMTAB_INITNELM,                   // Initial # STEs in SymTab
                                 DEF_SYMTAB_INCRNELM,                   // # STEs by which to resize when low
                                 gSymTabSize);                          // Maximum # STEs
            if (!bRet)
            {
                // ***FIXME*** -- WS FULL before we got started???
                MBW (L"SMWndProc/WM_CREATE:  AllocSymTab for <lpMemPTD->lpSymTab> failed");

                goto WM_CREATE_FAIL;    // Mark as failed
            } // End IF

            // *************** State Indicator Stack *******************

            // Allocate virtual memory for the State Indicator Stack
            lpLclMemVirtStr[PTDMEMVIRT_SIS].lpText   = "lpMemPTD->lpSISBeg in <SMWndProc>";
            lpLclMemVirtStr[PTDMEMVIRT_SIS].IncrSize = DEF_SIS_INCRNELM * sizeof (SYMENTRY);
            lpLclMemVirtStr[PTDMEMVIRT_SIS].MaxSize  = DEF_SIS_MAXNELM  * sizeof (SYMENTRY);
            lpLclMemVirtStr[PTDMEMVIRT_SIS].IniAddr  = (LPVOID)
            lpMemPTD->lpSISBeg = lpMemPTD->lpSISNxt =
              GuardAlloc (NULL,             // Any address
                          lpLclMemVirtStr[PTDMEMVIRT_SIS].MaxSize,
                          MEM_RESERVE,
                          PAGE_READWRITE);
            if (lpLclMemVirtStr[PTDMEMVIRT_SIS].IniAddr EQ NULL)
            {
                // ***FIXME*** -- WS FULL before we got started???
                MBW (L"SMWndProc/WM_CREATE:  GuardAlloc for <lpMemPTD->lpSISBeg> failed");

                goto WM_CREATE_FAIL;    // Mark as failed
            } // End IF

            // Link this struc into the chain
            LinkMVS (&lpLclMemVirtStr[PTDMEMVIRT_SIS]);

            // Commit the intial size
            MyVirtualAlloc (lpLclMemVirtStr[PTDMEMVIRT_SIS].IniAddr,
                            DEF_SIS_INITNELM * sizeof (SYMENTRY),
                            MEM_COMMIT,
                            PAGE_READWRITE);

            // *************** Control Structure Stack *****************

            // Allocate virtual memory for the Control Structure Stack
            lpLclMemVirtStr[PTDMEMVIRT_CS].lpText   = "lpMemPTD->lptkCSIni in <SMWndProc>";
            lpLclMemVirtStr[PTDMEMVIRT_CS].IncrSize = DEF_CS_INCRNELM * sizeof (TOKEN);
            lpLclMemVirtStr[PTDMEMVIRT_CS].MaxSize  = DEF_CS_MAXNELM  * sizeof (TOKEN);
            lpLclMemVirtStr[PTDMEMVIRT_CS].IniAddr  = (LPVOID)
            lpMemPTD->lptkCSIni = lpMemPTD->lptkCSNxt =
              GuardAlloc (NULL,             // Any address
                          lpLclMemVirtStr[PTDMEMVIRT_CS].MaxSize,
                          MEM_RESERVE,
                          PAGE_READWRITE);
            if (lpLclMemVirtStr[PTDMEMVIRT_CS].IniAddr EQ NULL)
            {
                // ***FIXME*** -- WS FULL before we got started???
                MBW (L"SMWndProc/WM_CREATE:  GuardAlloc for <lpMemPTD->lptkCSIni> failed");

                goto WM_CREATE_FAIL;    // Mark as failed
            } // End IF

            // Link this struc into the chain
            LinkMVS (&lpLclMemVirtStr[PTDMEMVIRT_CS]);

            // Commit the intial size
            MyVirtualAlloc (lpLclMemVirtStr[PTDMEMVIRT_CS].IniAddr,
                            DEF_CS_INITNELM * sizeof (TOKEN),
                            MEM_COMMIT,
                            PAGE_READWRITE);

            // *************** YYRes Buffer ****************************

            // Allocate virtual memory for the YYRes buffer
            lpLclMemVirtStr[PTDMEMVIRT_YYRES].lpText   = "lpMemPTD->lpYYRes in <SMWndProc>";
            lpLclMemVirtStr[PTDMEMVIRT_YYRES].IncrSize = DEF_YYRES_INCRNELM * sizeof (PL_YYSTYPE);
            lpLclMemVirtStr[PTDMEMVIRT_YYRES].MaxSize  = DEF_YYRES_MAXNELM  * sizeof (PL_YYSTYPE);
            lpLclMemVirtStr[PTDMEMVIRT_YYRES].IniAddr  = (LPVOID)
            lpMemPTD->lpYYRes =
              GuardAlloc (NULL,             // Any address
                          lpLclMemVirtStr[PTDMEMVIRT_YYRES].MaxSize,
                          MEM_RESERVE,
                          PAGE_READWRITE);
            if (lpLclMemVirtStr[PTDMEMVIRT_YYRES].IniAddr EQ NULL)
            {
                // ***FIXME*** -- WS FULL before we got started???
                MBW (L"SMWndProc/WM_CREATE:  GuardAlloc for <lpMemPTD->lpYYRes> failed");

                goto WM_CREATE_FAIL;    // Mark as failed
            } // End IF

            // Link this struc into the chain
            LinkMVS (&lpLclMemVirtStr[PTDMEMVIRT_YYRES]);

            // Commit the intial size
            MyVirtualAlloc (lpLclMemVirtStr[PTDMEMVIRT_YYRES].IniAddr,
                            DEF_YYRES_INITNELM * sizeof (PL_YYSTYPE),
                            MEM_COMMIT,
                            PAGE_READWRITE);

            //***************************************************************
            // *************** Strand Accumulators *********************
            //***************************************************************

            //***************************************************************
            // Allocate virtual memory for the VAR strand accumulator
            //***************************************************************
            lpLclMemVirtStr[PTDMEMVIRT_STRAND_VAR].lpText   = "lpMemPTD->lpStrand[STRAND_VAR] in <SMWndProc>";
            lpLclMemVirtStr[PTDMEMVIRT_STRAND_VAR].IncrSize = DEF_STRAND_INCRNELM * sizeof (PL_YYSTYPE);
            lpLclMemVirtStr[PTDMEMVIRT_STRAND_VAR].MaxSize  = DEF_STRAND_MAXNELM  * sizeof (PL_YYSTYPE);
            lpLclMemVirtStr[PTDMEMVIRT_STRAND_VAR].IniAddr  = (LPVOID)
            lpMemPTD->lpStrand[STRAND_VAR] =
              GuardAlloc (NULL,             // Any address
                          lpLclMemVirtStr[PTDMEMVIRT_STRAND_VAR].MaxSize,
                          MEM_RESERVE,
                          PAGE_READWRITE);
            if (lpLclMemVirtStr[PTDMEMVIRT_STRAND_VAR].IniAddr EQ NULL)
            {
                // ***FIXME*** -- WS FULL before we got started???
                MBW (L"SMWndProc/WM_CREATE:  GuardAlloc for <lpMemPTD->lpStrand[STRAND_VAR]> failed");

                return FALSE;           // Mark as failed
            } // End IF

            // Link this struc into the chain
            LinkMVS (&lpLclMemVirtStr[PTDMEMVIRT_STRAND_VAR]);

            // Commit the intial size
            MyVirtualAlloc (lpLclMemVirtStr[PTDMEMVIRT_STRAND_VAR].IniAddr,
                            DEF_STRAND_INITNELM * sizeof (PL_YYSTYPE),
                            MEM_COMMIT,
                            PAGE_READWRITE);
            //***************************************************************
            // Allocate virtual memory for the FCN strand accumulator
            //***************************************************************
            lpLclMemVirtStr[PTDMEMVIRT_STRAND_FCN].lpText   = "lpMemPTD->lpStrand[STRAND_FCN] in <SMWndProc>";
            lpLclMemVirtStr[PTDMEMVIRT_STRAND_FCN].IncrSize = DEF_STRAND_INCRNELM * sizeof (PL_YYSTYPE);
            lpLclMemVirtStr[PTDMEMVIRT_STRAND_FCN].MaxSize  = DEF_STRAND_MAXNELM  * sizeof (PL_YYSTYPE);
            lpLclMemVirtStr[PTDMEMVIRT_STRAND_FCN].IniAddr  = (LPVOID)
            lpMemPTD->lpStrand[STRAND_FCN] =
              GuardAlloc (NULL,             // Any address
                          lpLclMemVirtStr[PTDMEMVIRT_STRAND_FCN].MaxSize,
                          MEM_RESERVE,
                          PAGE_READWRITE);
            if (lpLclMemVirtStr[PTDMEMVIRT_STRAND_FCN].IniAddr EQ NULL)
            {
                // ***FIXME*** -- WS FULL before we got started???
                MBW (L"SMWndProc/WM_CREATE:  GuardAlloc for <lpMemPTD->lpStrand[STRAND_FCN]> failed");

                return FALSE;           // Mark as failed
            } // End IF

            // Link this struc into the chain
            LinkMVS (&lpLclMemVirtStr[PTDMEMVIRT_STRAND_FCN]);

            // Commit the intial size
            MyVirtualAlloc (lpLclMemVirtStr[PTDMEMVIRT_STRAND_FCN].IniAddr,
                            DEF_STRAND_INITNELM * sizeof (PL_YYSTYPE),
                            MEM_COMMIT,
                            PAGE_READWRITE);

            //***************************************************************
            // Allocate virtual memory for the LST strand accumulator
            //***************************************************************
            lpLclMemVirtStr[PTDMEMVIRT_STRAND_LST].lpText   = "lpMemPTD->lpStrand[STRAND_LST] in <SMWndProc>";
            lpLclMemVirtStr[PTDMEMVIRT_STRAND_LST].IncrSize = DEF_STRAND_INCRNELM * sizeof (PL_YYSTYPE);
            lpLclMemVirtStr[PTDMEMVIRT_STRAND_LST].MaxSize  = DEF_STRAND_MAXNELM  * sizeof (PL_YYSTYPE);
            lpLclMemVirtStr[PTDMEMVIRT_STRAND_LST].IniAddr  = (LPVOID)
            lpMemPTD->lpStrand[STRAND_LST] =
              GuardAlloc (NULL,             // Any address
                          lpLclMemVirtStr[PTDMEMVIRT_STRAND_LST].MaxSize,
                          MEM_RESERVE,
                          PAGE_READWRITE);
            if (lpLclMemVirtStr[PTDMEMVIRT_STRAND_LST].IniAddr EQ NULL)
            {
                // ***FIXME*** -- WS FULL before we got started???
                MBW (L"SMWndProc/WM_CREATE:  GuardAlloc for <lpMemPTD->lpStrand[STRAND_LST]> failed");

                return FALSE;           // Mark as failed
            } // End IF

            // Link this struc into the chain
            LinkMVS (&lpLclMemVirtStr[PTDMEMVIRT_STRAND_LST]);

            // Commit the intial size
            MyVirtualAlloc (lpLclMemVirtStr[PTDMEMVIRT_STRAND_LST].IniAddr,
                            DEF_STRAND_INITNELM * sizeof (PL_YYSTYPE),
                            MEM_COMMIT,
                            PAGE_READWRITE);

            //***************************************************************
            // Allocate virtual memory for the NAM strand accumulator
            //***************************************************************
            lpLclMemVirtStr[PTDMEMVIRT_STRAND_NAM].lpText   = "lpMemPTD->lpStrand[STRAND_NAM] in <SMWndProc>";
            lpLclMemVirtStr[PTDMEMVIRT_STRAND_NAM].IncrSize = DEF_STRAND_INCRNELM * sizeof (PL_YYSTYPE);
            lpLclMemVirtStr[PTDMEMVIRT_STRAND_NAM].MaxSize  = DEF_STRAND_MAXNELM  * sizeof (PL_YYSTYPE);
            lpLclMemVirtStr[PTDMEMVIRT_STRAND_NAM].IniAddr  = (LPVOID)
            lpMemPTD->lpStrand[STRAND_NAM] =
              GuardAlloc (NULL,             // Any address
                          lpLclMemVirtStr[PTDMEMVIRT_STRAND_NAM].MaxSize,
                          MEM_RESERVE,
                          PAGE_READWRITE);
            if (lpLclMemVirtStr[PTDMEMVIRT_STRAND_NAM].IniAddr EQ NULL)
            {
                // ***FIXME*** -- WS FULL before we got started???
                MBW (L"SMWndProc/WM_CREATE:  GuardAlloc for <lpMemPTD->lpStrand[STRAND_NAM]> failed");

                return FALSE;           // Mark as failed
            } // End IF

            // Link this struc into the chain
            LinkMVS (&lpLclMemVirtStr[PTDMEMVIRT_STRAND_NAM]);

            // Commit the intial size
            MyVirtualAlloc (lpLclMemVirtStr[PTDMEMVIRT_STRAND_NAM].IniAddr,
                            DEF_STRAND_INITNELM * sizeof (PL_YYSTYPE),
                            MEM_COMMIT,
                            PAGE_READWRITE);

            // *************** lpwszFormat *****************************

            // Allocate virtual memory for the WCHAR Formatting storage
            lpLclMemVirtStr[PTDMEMVIRT_WSZFORMAT].lpText   = "lpMemPTD->lpwszFormat in <SMWndProc>";
            lpLclMemVirtStr[PTDMEMVIRT_WSZFORMAT].IncrSize = DEF_WFORMAT_INCRNELM * sizeof (WCHAR);
            lpLclMemVirtStr[PTDMEMVIRT_WSZFORMAT].MaxSize  = DEF_WFORMAT_MAXNELM  * sizeof (WCHAR);
            lpLclMemVirtStr[PTDMEMVIRT_WSZFORMAT].IniAddr  = (LPVOID)
            lpMemPTD->lpwszFormat =
              GuardAlloc (NULL,             // Any address
                          lpLclMemVirtStr[PTDMEMVIRT_WSZFORMAT].MaxSize,
                          MEM_RESERVE,
                          PAGE_READWRITE);
            if (lpLclMemVirtStr[PTDMEMVIRT_WSZFORMAT].IniAddr EQ NULL)
            {
                // ***FIXME*** -- WS FULL before we got started???
                MBW (L"SMWndProc/WM_CREATE:  GuardAlloc for <lpwszFormat> failed");

                return FALSE;           // Mark as failed
            } // End IF

            // Link this struc into the chain
            LinkMVS (&lpLclMemVirtStr[PTDMEMVIRT_WSZFORMAT]);

            // Commit the intial size
            MyVirtualAlloc (lpLclMemVirtStr[PTDMEMVIRT_WSZFORMAT].IniAddr,
                            DEF_WFORMAT_INITNELM * sizeof (WCHAR),
                            MEM_COMMIT,
                            PAGE_READWRITE);

            // *************** lpwszTemp *******************************

            // Allocate virtual memory for the WCHAR Formatting storage
            lpLclMemVirtStr[PTDMEMVIRT_WSZTEMP].lpText   = "lpMemPTD->lpwszTemp in <SMWndProc>";
            lpLclMemVirtStr[PTDMEMVIRT_WSZTEMP].IncrSize = DEF_WPTDTEMP_INCRNELM * sizeof (WCHAR);
            lpLclMemVirtStr[PTDMEMVIRT_WSZTEMP].MaxSize  = DEF_WPTDTEMP_MAXNELM  * sizeof (WCHAR);
            lpLclMemVirtStr[PTDMEMVIRT_WSZTEMP].IniAddr  = (LPVOID)
            lpMemPTD->lpwszBaseTemp =
            lpMemPTD->lpwszTemp =
              GuardAlloc (NULL,             // Any address
                          lpLclMemVirtStr[PTDMEMVIRT_WSZTEMP].MaxSize,
                          MEM_RESERVE,
                          PAGE_READWRITE);
            // Save the maximum size
            lpMemPTD->iTempMaxSize = lpLclMemVirtStr[PTDMEMVIRT_WSZTEMP].MaxSize;

            if (lpLclMemVirtStr[PTDMEMVIRT_WSZTEMP].IniAddr EQ NULL)
            {
                // ***FIXME*** -- WS FULL before we got started???
                MBW (L"SMWndProc/WM_CREATE:  GuardAlloc for <lpwszTemp> failed");

                return FALSE;           // Mark as failed
            } // End IF

            // Link this struc into the chain
            LinkMVS (&lpLclMemVirtStr[PTDMEMVIRT_WSZTEMP]);

            // Commit the intial size
            MyVirtualAlloc (lpLclMemVirtStr[PTDMEMVIRT_WSZTEMP].IniAddr,
                            DEF_WPTDTEMP_INITNELM * sizeof (WCHAR),
                            MEM_COMMIT,
                            PAGE_READWRITE);

            // *************** lpForStmt *******************************

            // Allocate virtual memory for the WCHAR Formatting storage
            lpLclMemVirtStr[PTDMEMVIRT_FORSTMT].lpText   = "lpMemPTD->lpForStmt in <SMWndProc>";
            lpLclMemVirtStr[PTDMEMVIRT_FORSTMT].IncrSize = DEF_FORSTMT_INCRNELM * sizeof (FORSTMT);
            lpLclMemVirtStr[PTDMEMVIRT_FORSTMT].MaxSize  = DEF_FORSTMT_MAXNELM  * sizeof (FORSTMT);
            lpLclMemVirtStr[PTDMEMVIRT_FORSTMT].IniAddr  = (LPVOID)
            lpMemPTD->lpForStmtBase =
              GuardAlloc (NULL,             // Any address
                          lpLclMemVirtStr[PTDMEMVIRT_FORSTMT].MaxSize,
                          MEM_RESERVE,
                          PAGE_READWRITE);
            if (lpLclMemVirtStr[PTDMEMVIRT_FORSTMT].IniAddr EQ NULL)
            {
                // ***FIXME*** -- WS FULL before we got started???
                MBW (L"SMWndProc/WM_CREATE:  GuardAlloc for <lpForStmtBase> failed");

                return FALSE;           // Mark as failed
            } // End IF

            // Link this struc into the chain
            LinkMVS (&lpLclMemVirtStr[PTDMEMVIRT_FORSTMT]);

            // Commit the intial size
            MyVirtualAlloc (lpLclMemVirtStr[PTDMEMVIRT_FORSTMT].IniAddr,
                            DEF_FORSTMT_INITNELM * sizeof (FORSTMT),
                            MEM_COMMIT,
                            PAGE_READWRITE);

            // *************** System Names ****************************

            // Initialize all system names (functions and variables) as reserved
            if (!InitSystemNames_EM ())
            {
                MBW (L"WM_CREATE:  InitSystemNames_EM failed");

                goto WM_CREATE_FAIL;    // Mark as failed
            } // End IF

            // *************** System Vars *****************************

            // Initialize all system vars
            InitSysVars ();

            // *************** Edit Ctrl ****************************
            // Create an Edit Ctrl within which we can enter expressions
            hWndEC =
              CreateWindowExW (0L,                  // Extended styles
                               LECWNDCLASS,         // Class name
                               NULL,                // Initial text
                               0
                             | WS_CHILD
                             | WS_CLIPCHILDREN
                             | WS_VSCROLL
                             | ES_MULTILINE
                             | ES_WANTRETURN
                             | ES_AUTOHSCROLL
                             | ES_AUTOVSCROLL
                               ,                    // Styles
                               0,                   // X-position
                               0,                   // Y-...
                               CW_USEDEFAULT,       // Width
                               CW_USEDEFAULT,       // Height
                               hWnd,                // Parent window
                               (HMENU) IDWC_SM_EC,  // ID
                               _hInstance,          // Instance
                               0);                  // lParam
            if (hWndEC EQ NULL)
            {
                MB (pszNoCreateSMEditCtrl);

                goto WM_CREATE_FAIL;    // Mark as failed
            } // End IF

            // Save in window extra bytes
            SetWindowLongPtrW (hWnd, GWLSF_HWNDEC, (APLU3264) (LONG_PTR) hWndEC);

            // Set the paint hook
            SendMessageW (hWndEC, EM_SETPAINTHOOK, 0, (LPARAM) &LclECPaintHook);

            // Save the Window Background brush
            SetClassLongPtrW (hWndEC, GCLP_HBRBACKGROUND, (HANDLE_PTR) ghBrushBG);

            // Set the margins for a Session Manager window
            SetMarginsSM (hWndEC);

            // Set the soft-break flag
            SendMessageW (hWndEC, EM_FMTLINES, TRUE, 0);

            // Paint the window
            ShowWindow (hWndEC, SW_SHOWNORMAL);
            UpdateWindow (hWndEC);

            // *************** Magic Functions/Operators ***************

            // Initialize all magic functions/operators
            if (!InitMagicFunctions (lpMemPTD, hWndEC, lpLclMemVirtStr, PTDMEMVIRT_MFO1, PTDMEMVIRT_LENGTH))
            {
                MBW (L"WM_CREATE:  InitMagicFunctions failed");

                goto WM_CREATE_FAIL_UNHOOK;
            } // End IF

            // ***FIXME*** -- For some reason, the
            //                background of the MC window
            //                doesn't get painted, so I'm
            //                using this kludge for now.
            InvalidateRect (lpMemPTD->hWndMC, NULL, TRUE);

            // Save the bExecLX flag
            lpMemPTD->bExecLX = (*(LPSM_CREATESTRUCTW *) &lpMDIcs->lParam)->bExecLX;

            // Display the )LOAD message once and only once
            if (!bLoadMsgDisp && !OptionFlags.bNoCopyrightMsg)
            {
                // Display leading Copyright text and disclaimer
                AppendLine (WS_APPNAME L" " LVER_COPYRIGHT1_STR L".", FALSE, TRUE);
                AppendLine (L"This program comes with ABSOLUTELY NO WARRANTY; for details visit"            ,    TRUE , TRUE);
                AppendLine (L"  http://www.nars2000.org/License.html, or click on Help > About."            ,    TRUE , TRUE);
                AppendLine (L"This is free software, and you are welcome to redistribute it"                ,    TRUE , TRUE);
                AppendLine (L"  under certain conditions; visit the above link for details."                ,    TRUE , TRUE);
                AppendLine (L"To suppress this message, select Edit | Customize from the main"              ,    TRUE , TRUE);
                AppendLine (L"  menu, then in the User Preferences category, check the box marked"          ,    TRUE , TRUE);
                AppendLine (L"  \"Suppress copyright message at startup\"."                                 ,    TRUE , TRUE);

                // Mark as displayed so we don't do it again
                bLoadMsgDisp = TRUE;
            } // End IF

            // Attempt to load the workspace
            if (!LoadWorkspace_EM ((*(LPSM_CREATESTRUCTW *) &lpMDIcs->lParam)->hGlbDPFE, hWndEC))
            {
                // If we're loading a workspace from the command line, we can't afford to fail
                if (wszLoadFile[0])
                {
                    if (!LoadWorkspace_EM (NULL, hWndEC))   // Attempt to load a CLEAR WS
                        goto LOAD_WORKSPACE_FAIL;           // If that fails, give up
                } else
                    goto LOAD_WORKSPACE_FAIL;           // If that fails, give up
            } // End IF

            // Zap so the above test doesn't succeed again
            wszLoadFile[0] = WC_EOS;

            // Tell the window to finish initialization
            PostMessageW (hWnd, MYWM_INIT_EC, 0, 0);

            lResult = FALSE;        // We handled the msg

            goto NORMAL_EXIT;       // Join common code

WM_CREATE_FAIL_UNHOOK:
WM_CREATE_FAIL:
            // Send a constant message to the previous tab
            SendMessageLastTab (ERRMSG_TABS_FULL APPEND_NAME, lpMemPTD);
LOAD_WORKSPACE_FAIL:
            // Free global storage
            FreeGlobalStorage (lpMemPTD);
NORMAL_EXIT:
            // Free the workspace global
            DbgGlobalFree ((*(LPSM_CREATESTRUCTW *) &lpMDIcs->lParam)->hGlbDPFE); (*(LPSM_CREATESTRUCTW *) &lpMDIcs->lParam)->hGlbDPFE = NULL;

            PERFMON
////////////PERFMONSHOW (NULL)

            return lResult;         // Mark as failed
        } // End WM_CREATE
#undef  lpMDIcs

#define fwEvent     (LOWORD (wParam))
#define idChild     (HIWORD (wParam))
        case WM_PARENTNOTIFY:       // fwEvent = LOWORD(wParam);  // Event flags
                                    // idChild = HIWORD(wParam);  // Identifier of child window
                                    // lValue = lParam;           // Child handle, or cursor coordinates
            // Check for WM_CREATE from the Edit Ctrl/Debugger
            switch (fwEvent)
            {
                case WM_CREATE:
                    // Split cases based upon the child ID
                    switch (idChild)
                    {
                        case IDWC_SM_EC:
                            PostMessageW (hWnd, MYWM_INIT_EC, 0, 0);

                            break;
                    } // End SWITCH

                    break;

                case WM_LBUTTONDOWN:        // fwKeys = wParam;         // key flags
                                            // xPos = LOSHORT(lParam);  // horizontal position of cursor
                                            // yPos = HISHORT(lParam);  // vertical position of cursor
                {
                    LONG lCharPos,              // Character position
                         lLineNum;              // Line #

                    // Ask the Edit Ctrl what char is under the mouse cursor
                    CharFromPos (hWndEC, SendMessageW (hWndEC, EM_CHARFROMPOS, 0, lParam), &lLineNum, &lCharPos);

                    // If it's valid, ...
                    if (lCharPos NE -1)
                        // Move to that line
                        MoveToLine (lLineNum,
                                    lpMemPTD,
                                    hWndEC);
                    break;
                } // End WM_LBUTTONDOWN

                default:
                    break;
            } // End SWITCH

            return FALSE;           // We handled the msg
#undef  idChild
#undef  fwEvent

#ifdef DEBUG
        case MYWM_INIT_SMDB:
            // If the Debugger window handle is active, ...
            if (lpMemPTD->hWndDB NE NULL)
                PostMessageW (hWnd, MYWM_KEYDOWN, VK_F9, 0);
            else
                PostMessageW (hWnd, MYWM_INIT_SMDB, 0, 0);
            break;
#endif
        case MYWM_INIT_EC:
            // Wait for the third receipt of this message
            //   so we are sure everything is initialized
            switch ((APLU3264) (LONG_PTR) GetPropW (hWnd, L"INIT_EC"))
            {
                case 0:
                    SetPropW (hWnd, L"INIT_EC", (HANDLE) 1);

                    return FALSE;

                case 1:
                    SetPropW (hWnd, L"INIT_EC", (HANDLE) 2);

                    return FALSE;

                case 2:
                    RemovePropW (hWnd, L"INIT_EC");

                    PERFMON
////////////////////PERFMONSHOW (NULL)

                    break;

                defstop
                    break;
            } // End SWITCH

            // Initialize the precision-specific VFP constants
            InitVfpPrecision (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_FPC]->stData.stInteger);

            // Initialize PerTabData vars
            InitPTDVars (lpMemPTD);

            // Make sure we can communicate between windows
            AttachThreadInput (GetCurrentThreadId (), dwMainThreadId, TRUE);

            // Resize the Master Frame so as to show/hide the Status Window
            PostMessageW (hWndMF, MYWM_RESIZE, 0, 0);

            // Tell the Edit Ctrl about its font
            SendMessageW (hWndEC, WM_SETFONT, (WPARAM) GetFSIndFontHandle (FONTENUM_SM), MAKELPARAM (TRUE, 0));
#ifdef DEBUG
            PostMessageW (hWnd, MYWM_INIT_SMDB, 0, 0);
#endif
            // If requested, execute []LX
            if (lpMemPTD->bExecLX)
            {
                // Reset the flag
                lpMemPTD->bExecLX = FALSE;

#define EXECUTE_QUADLX  WS_UTF16_UPTACKJOT $QUAD_LX

                // Execute the statement in immediate execution mode
                ImmExecStmt (EXECUTE_QUADLX,                // Ptr to line to execute
                             strcountof (EXECUTE_QUADLX),   // NELM of line to execute
                             FALSE,                         // TRUE iff free the line on completion
                             hWndEC,                        // Edit Ctrl window handle
                             TRUE);                         // TRUE iff errors are acted upon
            } else
            {
                // If the SI level is for Quad Input
                if (lpMemPTD->lpSISCur NE NULL
                 && lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QUAD)
                    PostMessageW (hWnd, MYWM_QUOTEQUAD, FALSE, 105);
                else
                    // Display the default prompt
                    DisplayPrompt (hWndEC, 1);

                // Set the default cursor
                ForceSendCursorMsg (hWndEC, FALSE);
            } // End IF/ELSE

            // Ensure the SM has the focus
            SetFocus (hWnd);

            // Set all statusbar states
            SetStatusAll ();

            // Repaint the status window
            InvalidateRect (hWndStatus, NULL, TRUE);

            // Tell the Status Window about the new positions
            SetStatusPos (hWndEC);

            return FALSE;           // We handled the msg

        case MYWM_QUOTEQUAD:        // bQuoteQuad = (BOOL) wParam
                                    // TRUE iff Quote-Quad input, FALSE if Quad input
                                    // iPromptIndex = (int) lParam;
            if (!wParam)
            {
                AppendLine (wszQuadInput, FALSE, TRUE);
                DisplayPrompt (hWndEC, lParam);
            } // End IF

            return FALSE;           // We handled the msg

        case MYWM_ERRMSG:           // lpwErrMsg = (LPWCHAR) lParam;
            // Display the error message, replacing the current
            //   line as it might have a prompt on it
            ReplaceLastLineCRPmt (*(LPWCHAR *) &lParam);

            return FALSE;           // We handled the msg

        case MYWM_SAVE_WS:          // 0 = wParam;
                                    // lpwWSID = (LPWCHAR) lParam;
            // Save the workspace
            return CmdSave_EM (*(LPWCHAR *) &lParam);

#define fwSizeType  wParam
#define nWidth      (LOWORD (lParam))
#define nHeight     (HIWORD (lParam))
        case WM_SIZE:               // fwSizeType = wParam;      // Resizing flag
                                    // nWidth = LOWORD(lParam);  // Width of client area
                                    // nHeight = HIWORD(lParam); // Height of client area
            if (fwSizeType NE SIZE_MINIMIZED)
                SetWindowPos (hWndEC,           // Window handle to position
                              0,                // SWP_NOZORDER
                              0,                // X-position
                              0,                // Y-...
                              nWidth,           // Width
                              nHeight,          // Height
                              SWP_NOZORDER      // Flags
                            | SWP_SHOWWINDOW
                             );
            break;                  // *MUST* pass on to DefMDIChildProc
#undef  nHeight
#undef  nWidth
#undef  fwSizeType

        case WM_SETFONT:            // hFont = (HFONT) wParam;
                                    // fRedraw = LOWORD (lParam);
            // Changing the font also means changing the size
            //   of the margins as the character width might change
            SetMarginsSM (hWndEC);

            // Fall through to common code

        case WM_KILLFOCUS:          // hwndGainFocus = (HWND) wParam; // handle of window gaining focus
            // Pass these messages through to the EditCtrl
            SendMessageW (hWndEC, message, wParam, lParam);

            break;

        case WM_SETFOCUS:           // hwndLoseFocus = (HWND) wParam; // handle of window losing focus
            // Pass on to the edit ctrl
            SetFocus (hWndEC);

            break;                  // *MUST* pass on to DefMDIChildProc

        case WM_MDIACTIVATE:        // Activate/de-activate a child window
            // If we're being activated, ...
            if (GET_WM_MDIACTIVATE_FACTIVATE (hWnd, wParam, lParam))
            {
                ActivateMDIMenu (WINDOWCLASS_SM, hWnd);
                SetFocus (hWnd);
                SendMessageW (hWndEC, EM_SCROLLCARET, 0, 0);
            } // End IF

            break;                  // Continue with DefMDIChildProc

        case MYWM_SETFOCUS:
            // Set the focus to the Session Manager so the cursor displays
            SetFocus (hWnd);

            return FALSE;           // We handled the msg

        case WM_UNDO:
        case MYWM_REDO:
        case WM_COPY:
        case MYWM_COPY_APL:
        case WM_CUT:
        case WM_PASTE:
        case MYWM_PASTE_APL:
        case WM_CLEAR:
        case MYWM_SELECTALL:
            // Pass on to the Edit Ctrl
            SendMessageW (hWndEC, message, wParam, lParam);

            return FALSE;           // We handled the msg

        case MYWM_CREATEFCN:
            // If there's an active program, ignore this message
            if (!(lpMemPTD->lpSISCur NE NULL
              && !lpMemPTD->lpSISCur->bSuspended))
                CreateFcnWindow (L"");
            return FALSE;           // We handled the msg

#define nVirtKey    ((int) wParam)
#define uLineNum    ((UINT) lParam)
        case MYWM_KEYDOWN:          // nVirtKey = (int) wParam;     // Virtual-key code
                                    // uLineNum = lParam;           // Line #
                                    // lKeyData = lParam;           // Key data
        {
            UINT uLineLen,
                 uLineCnt;
            UBOOL bRet,
                  ksCtrl,
                  ksShift;

            // Set the shift-key state
            ksCtrl  = (GetKeyState (VK_CONTROL) & BIT15) ? TRUE : FALSE;
            ksShift = (GetKeyState (VK_SHIFT  ) & BIT15) ? TRUE : FALSE;

            // Special cases for SM windows:
            //   * Up/Dn arrows:
            //     * Cache original line before it's changed
            //
            //   * CR:
            //     * Restore original line from cached copy
            //     * Pass changed line to parent for execution
            //
            //   * Shift-CR
            //     * Insert a soft-break (line continuation)

            switch (nVirtKey)
            {
                case VK_CANCEL:
                {
                    EnterCriticalSection (&CSOPL);

                    // If there's a current SIS level, ...
                    if (lpMemPTD->lpPLCur NE NULL)
                    {
                        HANDLE hWaitEvent = lpMemPTD->hWaitEvent;

                        // If there's a delay active, ...
                        if (lpMemPTD->hSemaDelay NE NULL)
                            // Release the semaphore
                            MyReleaseSemaphore (lpMemPTD->hSemaDelay, 1, NULL);
                        else
                        // If there's a wait event active, ...
                        if (hWaitEvent NE NULL)
                        {
                            // Clear the event handle
                            lpMemPTD->hWaitEvent = NULL;

                            // Signal the event
                            SetEvent (hWaitEvent);
                        } else
                            lpMemPTD->lpPLCur->bCtrlBreak = TRUE;
                    } // End IF

                    LeaveCriticalSection (&CSOPL);

                    break;
                } // End VK_CANCEL

                case VK_RETURN:
                    // If there's an active program, ignore this key
                    bRet = (lpMemPTD->lpSISCur NE NULL
                        && !lpMemPTD->lpSISCur->bSuspended);
                    if (!bRet)
                    {
                        // If we're not on the last line,
                        //   copy it and append it to the buffer
                        if (ksShift || ksCtrl || !IzitLastLine (hWndEC))
                        {
                            UINT    uLastNum;
                            LPWCHAR lpwTmpLine;

                            // If we're inserting a soft line-break, ...
                            if (ksShift)
                            {
                                POINT ptCaret;                  // The caret position
                                UINT  uCharPos;                 // The line position (start of line)

                                // Get the char index (from the beginning of the edit text) given
                                //   a point in screen coordinates

                                // Get the caret position in client coords
                                GetCaretPos (&ptCaret);

                                // Get the position of the char under the caret
                                CharFromPos (hWndEC, SendMessageW (hWndEC, EM_CHARFROMPOS, 0, MAKELPARAM (ptCaret.x, ptCaret.y)), NULL, &uCharPos);

                                // Select the insertion point
                                SendMessageW (hWndEC, EM_SETSEL, uCharPos, uCharPos);

                                // Split the line at the current position as a Line Continuation
                                SendMessageW (hWndEC, EM_REPLACESEL, FALSE, (LPARAM) WS_CRCRLF);
                            } else
                            {
                                // If we're not on the last line, ...
                                if (!IzitLastLine (hWndEC))
                                {
                                    UINT uLineBeg;

                                    // Get the line # of the start of a block of a Line Continuations
                                    uLineBeg = GetBlockStartLine (hWndEC, uLineNum);

                                    // Get the overall block length
                                    //   not including a terminating zero
                                    uLineLen = GetBlockLength (hWndEC, uLineBeg);

                                    // Allocate space for the line including the terminating CRLF and zero
                                    lpwTmpLine =
                                      DbgGlobalAlloc (GPTR, (uLineLen + strcountof (WS_CRLF) + 1)   // "+ 1" for the terminating zero
                                                           * sizeof (lpwTmpLine[0]));

                                    // Check for error
                                    if (lpwTmpLine EQ NULL)
                                    {
                                        MBW (L"MYWM_KEYDOWN/VK_RETURN:  Unable to allocate memory for block of lines in the Session Manager");

                                        return FALSE;
                                    } // End IF

                                    // Copy a block of lines
                                    //   including a terminating zero if there's enough room
                                    CopyBlockLines (hWndEC, uLineBeg, lpwTmpLine);

                                    Assert (lpwTmpLine[uLineLen] EQ WC_EOS);

////////////////////////////////////// Ensure properly terminated
////////////////////////////////////lpwTmpLine[uLineLen] = WC_EOS;      // Unnecessary as GPTR fills with zeros

                                    // If it's not Ctrl-CR, ...
                                    if (!ksCtrl)
                                        // Append CRLF
                                        strcatW (lpwTmpLine, WS_CRLF);

                                    // Move the text caret to the end of the buffer
                                    MoveCaretEOB (hWndEC);

                                    // Get the # of the last line
                                    uLastNum = (UINT) SendMessageW (hWndEC, EM_LINEFROMCHAR, (WPARAM) -1, 0);

                                    // Replace the last line in the buffer
                                    ReplaceLine (hWndEC, lpwTmpLine, uLastNum);

                                    // We no longer need this storage
                                    DbgGlobalFree (lpwTmpLine); lpwTmpLine = NULL;
                                } // End IF

                                Assert (lpMemPTD->hGlbCurLine NE NULL);

                                // If the current line is valid, ...
                                // ***FIXME*** -- happens at top of buffer in timing glitch
                                //                when the CR is handled before hGlbCurLine is saved
                                if (lpMemPTD->hGlbCurLine NE NULL)
                                {
                                    // Lock the memory to get a ptr to it
                                    lpwCurLine = MyGlobalLockWsz (lpMemPTD->hGlbCurLine);

                                    // Restore the original of the current line
                                    ReplaceLine (hWndEC, lpwCurLine, uLineNum);

                                    // We no longer need this ptr
                                    MyGlobalUnlock (lpMemPTD->hGlbCurLine); lpwCurLine = NULL;
                                } // End IF

                                // Move the text caret to the end of the buffer
                                MoveCaretEOB (hWndEC);

                                // Get the current line #
                                uLineNum = uLastNum;
                            } // End IF/ELSE
                        } // End IF
                    } // End IF

                    // If we're in Quote-Quad input, ...
                    if (lpMemPTD->lpSISCur NE NULL
                     && lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QQUAD)
                        // Format QQ input and save in global memory
                        FormatQQuadInput (uLineNum, hWndEC, lpMemPTD);
                    else
                    // Execute the line if no other program is active
                    //   and not a soft line-break,
                    //   and not copy w/o execute
                    if (!bRet && !ksShift && !ksCtrl)
                        ImmExecLine (hWndEC, uLineNum);
                    break;

                case VK_UP:
                    // If the next line is out of range, exit
                    if (uLineNum < 1)
                        break;

                    // Set (new) current line
                    MoveToLine (--uLineNum, lpMemPTD, hWndEC);

                    break;

                case VK_DOWN:
                    // Get the # lines in the Edit Ctrl
                    uLineCnt = (UINT) SendMessageW (hWndEC, EM_GETLINECOUNT, 0, 0);

                    // If the next line is out of range, exit
                    if (uLineCnt <= (uLineNum + 1))
                        break;

                    // Set (new) current line
                    MoveToLine (++uLineNum, lpMemPTD, hWndEC);

                    break;
#ifdef DEBUG
                case VK_F1:             // No action defined as yet
                case VK_F7:             // ...
////////////////case VK_F8:             // Handled in EDITFCN.C as <DisplayUndo>
                case VK_F10:            // Not generated
                    return FALSE;
#endif
#ifdef DEBUG
                case VK_F2:             // Display hash table entries
                {
                    LPSYMENTRY lpSymEntry;
                    STFLAGS    stFlags = {0};
                    static WCHAR wFcn[32] = L"test";
                    extern HGLOBAL hGlbRC1,
                                   hGlbRC2;
                    DbgBrk ();          // #ifdef DEBUG

                    // Set the flags for what we're looking up
                    stFlags.Inuse   = TRUE;
                    stFlags.ObjName = OBJNAME_USR;

                    // Look up the function name
                    lpSymEntry =
                      SymTabLookupName (wFcn, &stFlags);
                    if (lpSymEntry NE NULL)
                    {
                        // Save so as to monitor RefCnt changes
                        hGlbRC1 = ClrPtrTypeDir (lpSymEntry->stData.stGlbData);
                    } // End IF

////                DisplayHshTab (lpMemPTD->lphtsPTD);
////                DisplayHshTab (&htsGLB);

                    return FALSE;
                } // VK_F2
#endif
////#ifdef DEBUG
////                case VK_F3:             // Display current token entries
////                    DisplayTokens (ghGlbToken);
////
////                    return FALSE;
////#endif
#ifdef DEBUG
                case VK_F4:             // Display symbol table entries
                                        //   with non-zero reference counts
                    DbgBrk ();          // #ifdef DEBUG
                    // If it's Shift-, then display all
                    if (GetKeyState (VK_SHIFT) & BIT15)
                        DisplaySymTab (lpMemPTD->lphtsPTD, TRUE);
                    else
                        DisplaySymTab (lpMemPTD->lphtsPTD, FALSE);

                    return FALSE;
#endif
#ifdef DEBUG
                case VK_F5:             // Display outstanding global memory objects
                    // If it's Shift-, then display all
                    if (GetKeyState (VK_SHIFT) & BIT15)
                        DisplayGlobals (2);
                    else
                    // If it's Ctl-, then display non-sysvars
                    if (GetKeyState (VK_CONTROL) & BIT15)
                        DisplayGlobals (1);
                    else
                    // Otherwise, then display non-permanent non-sysvars
                        DisplayGlobals (0);

                    return FALSE;
#endif
#ifdef DEBUG
                case VK_F6:             // Display outstanding heap objects
                    DisplayHeap ();

                    return FALSE;
#endif
#ifdef DEBUG
                case VK_F9:             // Resize Debugger and Session Manager windows
                {
                    RECT rc;
                    int  nWidthMC,  nHeightMC,
                         nHeightDB, nHeightSM;
                    HWND hWndMC;

                    // Get the window handle to the MDI Client (our parent)
                    hWndMC = GetParent (hWnd);

                    // Get its client rectangle
                    GetClientRect (hWndMC, &rc);

                    // Calculate its width & height
                    nWidthMC  = rc.right  - rc.left;
                    nHeightMC = rc.bottom - rc.top;

                    // Calculate the height of the DB & SM windows
                    nHeightDB = nHeightMC / 3;
                    nHeightSM = nHeightMC - nHeightDB;

                    // Resize the Debugger window
                    //   to the top of the client area
                    SetWindowPos (lpMemPTD->hWndDB, // Window handle to position
                                  0,                // SWP_NOZORDER
                                  0,                // X-position
                                  0,                // Y-...
                                  nWidthMC,         // Width
                                  nHeightDB,        // Height
                                  SWP_NOZORDER      // Flags
                                | SWP_SHOWWINDOW
                                 );
                    // Resize the Session Manager window
                    //   to the bottom of the client area
                    SetWindowPos (lpMemPTD->hWndSM, // Window handle to position
                                  0,                // SWP_NOZORDER
                                  0,                // X-position
                                  nHeightDB,        // Y-...
                                  nWidthMC,         // Width
                                  nHeightSM,        // Height
                                  SWP_NOZORDER      // Flags
                                | SWP_SHOWWINDOW
                                 );
                    // Tell the debugger window to scroll the last line into view
                    SendMessageW (lpMemPTD->hWndDB, MYWM_DBGMSG_SCROLL, (WPARAM) -1, 0);

                    return FALSE;
                } // End VK_F9
#endif
#ifdef DEBUG
                case VK_F11:            // DbgBrk ()
                {
                    LPSYMENTRY   lpSym = NULL;

                    // Get ptr to PerTabData global memory
                    lpMemPTD = GetMemPTD ();

                    // Signal a breakpoint to invoke the debugger
                    DbgBrk ();      // #ifdef DEBUG

                    HeapCompact (GetProcessHeap (), 0);

                    CheckMemStat ();

                    return FALSE;

                    gFcnLvl = 0;

                    return FALSE;
                } // End VK_F11
#endif
#ifdef DEBUG
                case VK_F12:            // Clear the debugging display
                    // If it's Shift-, then toggle gDbgLvl between 0 and 9
                    if (GetKeyState (VK_SHIFT) & BIT15)
                        gDbgLvl = (gDbgLvl EQ 0) ? 9 : 0;
                    else
                        // Clear the debugger listbox
                        DbgClr ();

                    return FALSE;
#endif
#ifdef DEBUG
                defstop
#else
                default:
                    Beep (1000,        // Frequency in Hz (37 to 32,767)
                          1000);         // Duration in milliseconds
#endif
                    break;
            } // End SWITCH

            return FALSE;           // We handled the msg
        } // End MYWM_KEYDOWN
#undef  uLineNum
#undef  nVirtKey

        case WM_SYSCOLORCHANGE:
        case WM_SETTINGCHANGE:
            // Uninitialize window-specific resources
            SM_Delete (hWnd);

            // Initialize window-specific resources
            SM_Create (hWnd);

            return FALSE;           // We handled the msg

        case WM_CTLCOLOREDIT:
            // Ensure it's from our Edit Ctrl
            if (hWndEC EQ (HWND) lParam)
                return (LRESULT) ghBrushBG;
            else
                break;

#define lpnmEC  (*(LPNMEDITCTRL *) &lParam)
        case WM_NOTIFY:             // idCtrl = (int) wParam;
                                    // pnmh = (LPNMHDR) lParam;
            // Ensure it's from our Edit Ctrl
            if (lpnmEC->nmHdr.hwndFrom EQ hWndEC)
                // Pass on to LclEditCtrlWndProc
                SendMessageW (hWndEC, MYWM_NOTIFY, wParam, lParam);

            return FALSE;           // We handled the msg
#undef  lpnmEC

#define wNotifyCode     (HIWORD (wParam))
#define wID             (LOWORD (wParam))
#define hWndCtrl        MakeGlbFromVal (lParam)
        case WM_COMMAND:            // wNotifyCode = HIWORD (wParam); // Notification code
                                    // wID = LOWORD (wParam);         // Item, control, or accelerator identifier
                                    // hwndCtrl = (HWND) lParam;      // Handle of control
            // This message should be from the Edit Ctrl
            Assert (wID      EQ IDWC_FE_EC
                 || wID      EQ IDWC_SM_EC);
            Assert (hWndCtrl EQ hWndEC || hWndEC EQ 0);

            // Split cases based upon the notify code
            switch (wNotifyCode)
            {
                case EN_CHANGE:                     // idEditCtrl = (int) LOWORD(wParam); // Identifier of Edit Ctrl
                                                    // hwndEditCtrl = (HWND) lParam;      // Handle of Edit Ctrl
                    // The contents of the Edit Ctrl have changed,
                    // set the changed flag
                    SetWindowLongW (hWnd, GWLSF_CHANGED, TRUE);

                    break;

                case EN_MAXTEXT:    // idEditCtrl = (int) LOWORD(wParam); // Identifier of Edit Ctrl
                                    // hwndEditCtrl = (HWND) lParam;      // Handle of Edit Ctrl
                    // The Edit Ctrl has exceed its maximum # chars

                    // The default maximum is 32K, so we increase it by that amount
                    Assert (hWndEC NE 0);
                    SendMessageW (hWndEC,
                                  EM_SETLIMITTEXT,
                                  (UINT) SendMessageW (hWndEC, EM_GETLIMITTEXT, 0, 0) + 32*1024, 0);
                    break;

////            case EN_SETFOCUS:   // 0x0100
////            case EN_KILLFOCUS:  // 0x0200
////            case EN_CHANGE:     // 0x0300
////            case EN_UPDATE:     // 0x0400
////            case EN_ERRSPACE:   // 0x0500
////            case EN_MAXTEXT:    // 0x0501
////            case EN_HSCROLL:    // 0x0601
////            case EN_VSCROLL:    // 0x0602
////                break;
            } // End IF

            break;
#undef  hWndCtrl
#undef  wID
#undef  wNotifyCode

        case WM_TIMER:                      // wTimerID = wParam            // Timer identifier
                                            // tmpc = (TIMERPROC *) lParam  // Ptr to timer callback
            if (wParam EQ ID_TIMER_EXEC)
                // Update the status timer
                UpdateStatusTimer (lpMemPTD);

            return FALSE;           // We handled the msg

        case WM_CLOSE:
            // Because the SM window doesn't close unless the
            //   MF window is closing, we ignore this message.
            // We close when the MF window calls DestroyWindow.

            return FALSE;           // We handled the msg

        case WM_DESTROY:
            // Delete all system vars
            DeleSysVars (lpMemPTD->lphtsPTD);

            // Free global storage
            FreeGlobalStorage (lpMemPTD);
#ifdef DEBUG
            // If the debugger is still active, close it
            if (lpMemPTD->hWndDB NE NULL)
                SendMessageW (lpMemPTD->hWndMC, WM_MDIDESTROY, (WPARAM) lpMemPTD->hWndDB, 0);
#endif
////////////// *************** lptkStackBase ***************************
////////////if (lpMemPTD->lptkStackBase NE NULL)
////////////{
////////////    MyVirtualFree (lpMemPTD->lptkStackBase, 0, MEM_RELEASE); lpMemPTD->lptkStackBase = NULL;
////////////} // End IF

            // *************** hGlbCurLine *****************************
            if (lpMemPTD->hGlbCurLine NE NULL)
            {
                DbgGlobalFree (lpMemPTD->hGlbCurLine); lpMemPTD->hGlbCurLine = NULL;
            } // End IF

            // *************** PTDMEMVIRTENUM Entries ******************
            // Get the MemVirtStr ptr
            (HANDLE_PTR) lpLclMemVirtStr = GetWindowLongPtrW (hWnd, GWLSF_LPMVS);
            if (lpLclMemVirtStr NE NULL)
            {
                UINT uCnt;                  // Loop counter

                // If it's valid, ...
                if (lpMemPTD->lphtsPTD NE NULL)
                {
                    // Free the hash & sym tabs
                    FreeHshSymTabs (lpMemPTD->lphtsPTD, TRUE); lpMemPTD->lphtsPTD = NULL;
                } // End IF

                // Zap the ptrs in lpMemPTD
                lpMemPTD->lpwszQuadErrorMsg    = NULL;
////////////////lpUndoBeg                      = NULL;
////////////////lpMemPTD->lphtsPTD->lpHshTab   = NULL;      // Zero'ed in FreeHshSymTabs
////////////////lpMemPTD->lphtsPTD->lpSymTab   = NULL;      // ...
////////////////lpMemPTD->lphtsPTD             = NULL;      // Zero'ed above
                lpMemPTD->lpSISBeg             = NULL;
                lpMemPTD->lptkCSIni            = NULL;
                lpMemPTD->lpYYRes              = NULL;
                lpMemPTD->lpStrand[STRAND_VAR] = NULL;
                lpMemPTD->lpStrand[STRAND_FCN] = NULL;
                lpMemPTD->lpStrand[STRAND_LST] = NULL;
                lpMemPTD->lpStrand[STRAND_NAM] = NULL;
                lpMemPTD->lpwszFormat          = NULL;
                lpMemPTD->lpwszBaseTemp        = NULL;

                // Loop through the entries
                for (uCnt = 0; uCnt < PTDMEMVIRT_LENGTH; uCnt++)
                    // Free the virtual memory and unlink it
                    FreeVirtUnlink (uCnt, lpLclMemVirtStr);

                // Free the virtual storage
                MyVirtualFree (lpLclMemVirtStr, 0, MEM_RELEASE); lpLclMemVirtStr = NULL;

                // Zap it in case we come through again
                SetWindowLongPtrW (hWnd, GWLSF_LPMVS, (APLU3264) (LONG_PTR) NULL);
            } // End IF

            // Uninitialize window-specific resources
            SM_Delete (hWnd);

#ifndef UNISCRIBE
            // Release the FontLink ptr
            if (lpMemPTD->lpFontLink NE NULL)
            {
                IMLangFontLink_Release (lpMemPTD->lpFontLink); lpMemPTD->lpFontLink = NULL;
            } // End IF

            // Uninitialize the OLE libraries
            CoUninitialize ();
#endif
            break;
    } // End SWITCH

////LCLODSAPI ("SMZ:", hWnd, message, wParam, lParam);
    return DefMDIChildProcW (hWnd, message, wParam, lParam);
} // End SMWndProc
#undef  APPEND_NAME


//***************************************************************************
//  $MoveToLine
//
//  Common routine when moving the text cursor to a new line
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MoveToLine"
#else
#define APPEND_NAME
#endif

void MoveToLine
    (APLU3264     uLineNum,             // The given line #
     LPPERTABDATA lpMemPTD,             // Ptr to PerTabData global memory
     HWND         hWndEC)               // Edit Ctrl window handle

{
    UINT    uLineLen;                   // Line length
    LPWCHAR lpwCurLine;                 // Ptr to current line global memory

    // Get the length of the (new) current line
    uLineLen = GetLineLength (hWndEC, uLineNum);

    // If there's a previous current line global memory handle,
    //   free it
    if (lpMemPTD->hGlbCurLine NE NULL)
    {
        DbgGlobalFree (lpMemPTD->hGlbCurLine); lpMemPTD->hGlbCurLine = NULL;
    } // End IF

    // Allocate space for the line including a terminating zero
    lpMemPTD->hGlbCurLine =
      DbgGlobalAlloc (GHND, (uLineLen + 1) * sizeof (lpwCurLine[0]));

    // Check for error
    if (lpMemPTD->hGlbCurLine EQ NULL)
    {
        MBW (L"MoveToLine:  Unable to allocate memory for a new line in the Session Manager");

        _exit (-1);
    } // End IF

    // Lock the memory to get a ptr to it
    lpwCurLine = MyGlobalLockInt (lpMemPTD->hGlbCurLine);   // Might be only 1 byte

    // Tell EM_GETLINE maximum # chars in the buffer
    ((LPWORD) lpwCurLine)[0] = uLineLen;

    // Save the (new) current line
    SendMessageW (hWndEC, EM_GETLINE, uLineNum, (LPARAM) lpwCurLine);

    // We no longer need this ptr
    MyGlobalUnlock (lpMemPTD->hGlbCurLine); lpwCurLine = NULL;

    // Reset the changed line flag
    SetWindowLongW (lpMemPTD->hWndSM, GWLSF_CHANGED, FALSE);
} // End MoveToLine
#undef  APPEND_NAME


//***************************************************************************
//  $FreeRelinkHshTab
//
//  Free a <lpLclMemVirtStr> entry, unlink it, and relink in the new old
//***************************************************************************

void FreeRelinkHshTab
    (LPHSHENTRY lpNewHshTab,            // Ptr to the new HshTab
     LPHSHENTRY lpOldHshTab)            // ...        old ...

{
    HWND         hWndSM;                // Current SM window handle
    LPMEMVIRTSTR lpLclMemVirtStr;       // Ptr to local MemVirtStr

    // Get the current SM window handle
    hWndSM = GetMemPTD ()->hWndSM;

    // Get the ptr to the local virtual memory struc
    (HANDLE_PTR) lpLclMemVirtStr = GetWindowLongPtrW (hWndSM, GWLSF_LPMVS);

    // Free the virtual memory and unlink it
    FreeVirtUnlink (PTDMEMVIRT_HSHTAB, lpLclMemVirtStr);

    // Respecify the new Initial Address
    lpLclMemVirtStr[PTDMEMVIRT_HSHTAB].IniAddr = (LPVOID) lpNewHshTab;

    // Link in the new HshTab
    LinkMVS (&lpLclMemVirtStr[PTDMEMVIRT_HSHTAB]);
} // End FreeRelinkHshTab


//***************************************************************************
//  End of File: sessman.c
//***************************************************************************
