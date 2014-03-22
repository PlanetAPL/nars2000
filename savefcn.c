//***************************************************************************
//  NARS2000 -- Save Function Routines
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
#include <stdio.h>
#include "headers.h"


#ifdef DEBUG
HGLOBAL hGlbRC1,
        hGlbRC2;
#endif

LPWCHAR lpLW_FcnHdr = $AFORESULT $IS L"{" $ALPHA L"}"             $DEL                $OMEGA,   // Function header
        lpLW_Op1Hdr = $AFORESULT $IS L"{" $ALPHA L"}" L"(" $LOPER $DELDEL        L")" $OMEGA,   // Monadic operator header
        lpLW_Op2Hdr = $AFORESULT $IS L"{" $ALPHA L"}" L"(" $LOPER $DELDEL $ROPER L")" $OMEGA;   // Dyadic operator header


//***************************************************************************
//  $SaveFunction
//
//  Attempt to save a function to the WS
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SaveFunction"
#else
#define APPEND_NAME
#endif

UBOOL SaveFunction
    (HWND hWndFE)                   // Function Editor window handle

{
    SF_FCNS SF_Fcns = {0};

    // Fill in common values
    SF_Fcns.bDisplayErr     = TRUE;             // Display Errors
////SF_Fcns.bAFO            = FALSE;            // Parsing an AFO (already zero from = {0})
    SF_Fcns.SF_LineLen      = SF_LineLenFE;     // Ptr to line length function
    SF_Fcns.SF_ReadLine     = SF_ReadLineFE;    // Ptr to read line function
    SF_Fcns.SF_NumLines     = SF_NumLinesFE;    // Ptr to get # lines function
    SF_Fcns.SF_CreationTime = SF_CreationTimeCom;// Ptr to get function creation time
    SF_Fcns.SF_LastModTime  = SF_LastModTimeCom;// Ptr to get function creation time
    SF_Fcns.SF_UndoBuffer   = SF_UndoBufferFE;  // Ptr to get function Undo Buffer global memory handle
////SF_Fcns.LclParams       = NULL;             // Already zero from = {0}

    // Call common routine
    return SaveFunctionCom (hWndFE,             // Function Edit window handle (not-[]FX only)
                           &SF_Fcns);           // Ptr to common values
} // End SaveFunction
#undef  APPEND_NAME


//***************************************************************************
//  $SF_LineLenAN
//
//  Return the length of a function text header/line
//    when saving an AFO
//***************************************************************************

UINT SF_LineLenAN
    (HWND        hWndEC,            // Edit Ctrl window handle (FE only)
     LPLW_PARAMS lpLW_Params,       // Ptr to common struc
     UINT        uLineNum)          // Function line # (0 = header)

{
    // If it's the header, ...
    if (uLineNum EQ 0)
    {
        // Split cases based upon the UDFO type
        switch (lpLW_Params->lpYYRht->lptkRhtBrace->tkData.tkDfnType)
        {
            case DFNTYPE_FCN:
                return lstrlenW (lpLW_FcnHdr);

            case DFNTYPE_OP1:
                return lstrlenW (lpLW_Op1Hdr);

            case DFNTYPE_OP2:
                return lstrlenW (lpLW_Op2Hdr);

            defstop
                return -1;
        } // End SWITCH
    } else
        // Position of Right Brace less that of the char to the right of the left brace
        return lpLW_Params->lpYYRht->lptkRhtBrace->tkCharIndex
            - (lpLW_Params->lpYYRht->lptkLftBrace->tkCharIndex + 1);
} // End SF_LineLenAN


//***************************************************************************
//  $SF_LineLenFE
//
//  Return the length of a function text header/line
//    when called from function editor
//***************************************************************************

UINT SF_LineLenFE
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPVOID    lpVoid,              // Ptr to common struc
     UINT      uLineNum)            // Function line # (0 = header)

{
    UINT uLinePos;

    // Get the char pos at the start of this line
    uLinePos = (UINT) SendMessageW (hWndEC, EM_LINEINDEX, uLineNum, 0);

    return (UINT) SendMessageW (hWndEC, EM_LINELENGTH, uLinePos, 0);
} // End SF_LineLenFE


//***************************************************************************
//  $SF_LineLenLW
//
//  Return the length of a function text header/line
//    when called from LoadWorkspace
//***************************************************************************

UINT SF_LineLenLW
    (HWND        hWndEC,            // Edit Ctrl window handle (FE only)
     LPLW_PARAMS lpLW_Params,       // Ptr to common struc
     UINT        uLineNum)          // Function line # (0 = header)

{
    // The caller requests the line length before it reads
    //   the line so it can allocate memory.

    // This means we must read the line first so we can get its length
    SF_ReadLineLW (hWndEC,                  // Edit Ctrl window handle (FE only)
                   lpLW_Params,             // Ptr to common struc
                   uLineNum,                // Function line #
                   lpLW_Params->lpwBuffer); // Ptr to header/line global memory
    // Return the length
    return lstrlenW (lpLW_Params->lpwBuffer);
} // End SF_LineLenLW


//***************************************************************************
//  $SF_LineLenM
//
//  Return the length of a function text header/line
//    when called from []FX with a simple char matrix arg
//***************************************************************************

UINT SF_LineLenM
    (HWND        hWndEC,            // Edit Ctrl window handle (FE only)
     LPFX_PARAMS lpFX_Params,       // Ptr to common struc
     UINT        uLineNum)          // Function line # (0 = header)

{
    LPAPLCHAR lpMemRht;             // Ptr to right arg global memory
    UINT      uRowOff,              // Offset to start of row
              uLineLen;             // Line length

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (lpFX_Params->hGlbRht);

    // Get the # cols in the matrix
    uLineLen = (UINT) lpFX_Params->aplColsRht;

    // Calc the offset to the appropriate row
    uRowOff = uLineNum * uLineLen;

    // Skip over the header to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, lpFX_Params->aplRankRht);

    // As this is a matrix and the header/function line might have
    //   been padded out beyond its normal length, delete trailing blanks
    for (; uLineLen; uLineLen--)
    if (lpMemRht[uRowOff + uLineLen - 1] NE L' ')
        break;

    // We no longer need this ptr
    MyGlobalUnlock (lpFX_Params->hGlbRht); lpMemRht = NULL;

    return uLineLen;
} // End SF_LineLenM


//***************************************************************************
//  $SF_LineLenN
//
//  Return the length of a function text header/line
//    when called from []FX with a nested arg
//***************************************************************************

UINT SF_LineLenN
    (HWND        hWndEC,            // Edit Ctrl window handle (FE only)
     LPFX_PARAMS lpFX_Params,       // Ptr to common struc
     UINT        uLineNum)          // Function line # (0 = header)

{
    LPAPLNESTED lpMemRht;           // Ptr to right arg global memory
    HGLOBAL     hGlbItmRht;         // Right arg item global memory handle
    APLNELM     aplNELMItmRht;      // Right arg item NELM

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (lpFX_Params->hGlbRht);

    // Skip over the header to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, lpFX_Params->aplRankRht);

    // Split cases based upon the right arg item ptr type
    switch (GetPtrTypeDir (lpMemRht[uLineNum]))
    {
        case PTRTYPE_STCONST:
            aplNELMItmRht = 1;      // Scalar is of length 1

            break;

        case PTRTYPE_HGLOBAL:
            // Get the global memory handle
            hGlbItmRht = lpMemRht[uLineNum];

            // Get the right arg item global attrs
            AttrsOfGlb (hGlbItmRht, NULL, &aplNELMItmRht, NULL, NULL);

            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (lpFX_Params->hGlbRht); lpMemRht = NULL;

    return (UINT) aplNELMItmRht;
} // End SF_LineLenN


//***************************************************************************
//  $SF_LineLenSV
//
//  Return the length of a function text header/line
//    when called from []FX with a simple char scalar or vector arg
//***************************************************************************

UINT SF_LineLenSV
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPVOID    lpVoid,              // Ptr to common struc
     UINT      uLineNum)            // Function line # (0 = header)

{
    // The function line is a single char
    return 1;
} // End SF_LineLenSV


//***************************************************************************
//  $SF_LineLenTF1
//
//  Return the length of a function text header/line
//    when called from []TF with a simple char matrix arg
//***************************************************************************

UINT SF_LineLenTF1
    (HWND         hWndEC,           // Edit Ctrl window handle (FE only)
     LPTF1_PARAMS lpTF1_Params,     // Ptr to common struc
     UINT         uLineNum)         // Function line # (0 = header)

{
    return (UINT) lpTF1_Params->aplColsRht;
} // End SF_LineLenTF1


//***************************************************************************
//  $SF_LineLenAA
//
//  Return the length of a function text header/line
//    when called from )INASCII with a file whose contents are a
//    simple char vector
//***************************************************************************

UINT SF_LineLenAA
    (HWND         hWndEC,           // Edit Ctrl window handle (FE only)
     LPAA_PARAMS  lpAA_Params,      // Ptr to common struc
     UINT         uLineNum)         // Function line # (0 = header)

{
    LPWCHAR lpw;

    // Get a ptr to the start of the function lines
    lpw = lpAA_Params->lpwStart;

    // Skip to the designated line #
    while (uLineNum)
    {
        // Skip over the line
        lpw += lstrlenW (lpw);

        // Skip over trailing CR/LFs zapped to zeros
        while (*lpw EQ WC_EOS)
            lpw++;
        // Count out one more line
        uLineNum--;
    } // End WHILE

    // Return the line length
    return lstrlenW (lpw);
} // End SF_LineLenAA


//***************************************************************************
//  $SF_ReadLineAN
//
//  Read in a header/function line
//    when saving an AFO
//***************************************************************************

void SF_ReadLineAN
    (HWND        hWndEC,            // Edit Ctrl window handle (FE only)
     LPLW_PARAMS lpLW_Params,       // Ptr to common struc
     UINT        uLineNum,          // Function line # (0 = header)
     LPAPLCHAR   lpMemLine)         // Ptr to header/line global memory

{
    // If it's the header, ...
    if (uLineNum EQ 0)
    {
        // Split cases based upon the UDFO type
        switch (lpLW_Params->lpYYRht->lptkRhtBrace->tkData.tkDfnType)
        {
            case DFNTYPE_FCN:
                lstrcpyW (lpMemLine, lpLW_FcnHdr);

                break;

            case DFNTYPE_OP1:
                lstrcpyW (lpMemLine, lpLW_Op1Hdr);

                break;

            case DFNTYPE_OP2:
                lstrcpyW (lpMemLine, lpLW_Op2Hdr);

                break;

            defstop
                break;
        } // End SWITCH
    } else
    {
        // If the line text is present, ...
        if (lpLW_Params->lpplLocalVars->lpwszLine)
            // Copy the line to the caller's memory
            //   with room for the trailing zero
            lstrcpynW (lpMemLine,
                      &lpLW_Params->lpplLocalVars->lpwszLine[lpLW_Params->lpYYRht->lptkLftBrace->tkCharIndex + 1],
                       lpLW_Params->lpYYRht->lptkRhtBrace->tkCharIndex
                     - lpLW_Params->lpYYRht->lptkLftBrace->tkCharIndex);
        else
        {
            LPMEMTXT_UNION lpMemTxtLine;

            // Lock the memory to get a ptr to it
            lpMemTxtLine = MyGlobalLock (lpLW_Params->lpplLocalVars->hGlbTxtLine);

            // Copy the line to the caller's memory
            //   with room for the trailing zero
            lstrcpynW (lpMemLine,
                      &(&lpMemTxtLine->C)[lpLW_Params->lpYYRht->lptkLftBrace->tkCharIndex + 1],
                       lpLW_Params->lpYYRht->lptkRhtBrace->tkCharIndex
                     - lpLW_Params->lpYYRht->lptkLftBrace->tkCharIndex);
            // We no longer need this ptr
            MyGlobalUnlock (lpLW_Params->lpplLocalVars->hGlbTxtLine); lpMemTxtLine = NULL;
        } // End IF/ELSE
    } // End IF/ELSE
} // End SF_ReadLineAN


//***************************************************************************
//  $SF_ReadLineFE
//
//  Read in a header/function line
//    when called from function editor
//***************************************************************************

void SF_ReadLineFE
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPVOID    lpVoid,              // Ptr to common struc
     UINT      uLineNum,            // Function line # (0 = header)
     LPAPLCHAR lpMemLine)           // Ptr to header/line global memory

{
    SendMessageW (hWndEC, EM_GETLINE, uLineNum, (LPARAM) lpMemLine);
} // End SF_ReadLineFE


//***************************************************************************
//  $SF_ReadLineLW
//
//  Read in a header/function line
//    when called from LoadWorkspace
//***************************************************************************

void SF_ReadLineLW
    (HWND        hWndEC,            // Edit Ctrl window handle (FE only)
     LPLW_PARAMS lpLW_Params,       // Ptr to common struc
     UINT        uLineNum,          // Function line # (0 = header)
     LPAPLCHAR   lpMemLine)         // Ptr to header/line global memory

{
    WCHAR   wszLineNum[8];          // Save area for formatted line #
    LPWCHAR lpwszProf;              // Ptr to profile string

    // Format the line #
    wsprintfW (wszLineNum, L"%d", uLineNum);

    // Get the one (and only) line
    lpwszProf =
      ProfileGetString (lpLW_Params->lpwSectName,   // Ptr to the section name
                        wszLineNum,                 // Ptr to the key name
                        L"",                        // Ptr to the default value
                        lpLW_Params->lpDict);       // Ptr to workspace dictionary
    // Copy the line to a local buffer
    // DO NOT USE lstrcpyW as it doesn't trigger a visible Page Fault
    CopyMemoryW (lpLW_Params->lpwBuffer, lpwszProf, lstrlenW (lpwszProf) + 1);

    // Convert the {name}s and other chars to UTF16_xxx
    (void) ConvertNameInPlace (lpLW_Params->lpwBuffer);

    // Copy the buffer to the caller's save area
    CopyMemoryW (lpMemLine,
                 lpLW_Params->lpwBuffer,
                 lstrlenW (lpLW_Params->lpwBuffer));
} // End SF_ReadLineLW


//***************************************************************************
//  $SF_ReadLineM
//
//  Read in a header/function line
//    when called from []FX with a simple char matrix arg
//***************************************************************************

void SF_ReadLineM
    (HWND        hWndEC,            // Edit Ctrl window handle (FE only)
     LPFX_PARAMS lpFX_Params,       // Ptr to common struc
     UINT        uLineNum,          // Function line # (0 = header)
     LPAPLCHAR   lpMemLine)         // Ptr to header/line global memory

{
    LPAPLCHAR lpMemRht;             // Ptr to right arg global memory
    UINT      uRowOff,              // Offset to start of row
              uLineLen;             // Line length

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (lpFX_Params->hGlbRht);

    // Skip over the header to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, lpFX_Params->aplRankRht);

    // Get the # cols in the matrix
    uLineLen = (UINT) lpFX_Params->aplColsRht;

    // Calc the offset to the appropriate row
    uRowOff = uLineNum * uLineLen;

    // As this is a matrix and the header/function line might have
    //   been padded out beyond its normal length, delete trailing blanks
    for (; uLineLen; uLineLen--)
    if (lpMemRht[uRowOff + uLineLen - 1] NE L' ')
        break;

    // Copy the line to the result
    for (; uLineLen; uLineLen--, uRowOff++)
        *lpMemLine++ = lpMemRht[uRowOff];

    // Ensure properly terminated
    *lpMemLine++ = WC_EOS;

    // We no longer need this ptr
    MyGlobalUnlock (lpFX_Params->hGlbRht); lpMemRht = NULL;
} // End SF_ReadLineM


//***************************************************************************
//  $SF_ReadLineN
//
//  Read in a header/function line
//    when called from []FX with a nested arg
//***************************************************************************

void SF_ReadLineN
    (HWND        hWndEC,            // Edit Ctrl window handle (FE only)
     LPFX_PARAMS lpFX_Params,       // Ptr to common struc
     UINT        uLineNum,          // Function line # (0 = header)
     LPAPLCHAR   lpMemLine)         // Ptr to header/line global memory

{
    LPAPLNESTED lpMemRht;           // Ptr to right arg global memory
    LPAPLCHAR   lpMemItmRht;        // Ptr to right arg item global memory
    HGLOBAL     hGlbItmRht;         // Right arg item global memory handle
    UINT        uLineLen;           // Line length

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (lpFX_Params->hGlbRht);

    // Skip over the header to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, lpFX_Params->aplRankRht);

    // Split cases based upon the right arg item ptr type
    switch (GetPtrTypeDir (lpMemRht[uLineNum]))
    {
        case PTRTYPE_STCONST:
            // Copy the single char to the result
            *lpMemLine++ = ((LPSYMENTRY*) lpMemRht)[uLineNum]->stData.stChar;
            *lpMemLine++ = WC_EOS;

            break;

        case PTRTYPE_HGLOBAL:
            // Get the global memory handle
            hGlbItmRht = lpMemRht[uLineNum];

            // Lock the memory to get a ptr to it
            lpMemItmRht = MyGlobalLock (hGlbItmRht);

            // Get the NELM (line length)
#define lpHeader        ((LPVARARRAY_HEADER) lpMemItmRht)
            uLineLen = (UINT) lpHeader->NELM;

            // Skip over the header to the data
            lpMemItmRht = VarArrayBaseToData (lpMemItmRht, lpHeader->Rank);
#undef  lpHeader

            // Copy the data to the result
            CopyMemoryW (lpMemLine, lpMemItmRht, uLineLen);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbItmRht); lpMemItmRht = NULL;

            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (lpFX_Params->hGlbRht); lpMemRht = NULL;
} // End SF_ReadLineN


//***************************************************************************
//  $SF_ReadLineSV
//
//  Read in a header/function line
//    when called from []FX with a simple char scalar or vector arg
//***************************************************************************

void SF_ReadLineSV
    (HWND        hWndEC,            // Edit Ctrl window handle (FE only)
     LPFX_PARAMS lpFX_Params,       // Ptr to common struc
     UINT        uLineNum,          // Function line # (0 = header)
     LPAPLCHAR   lpMemLine)         // Ptr to header/line global memory

{
    LPAPLCHAR lpMemRht;             // Ptr to right arg global memory

    // Check for immediate right arg
    if (uLineNum EQ 0 && lpFX_Params->hGlbRht EQ NULL)
    {
        // Copy the simple char to the result
        *lpMemLine++ = (APLCHAR) lpFX_Params->aplLongestRht;
        *lpMemLine++ = WC_EOS;
    } else
    {
        // Lock the memory to get a ptr to it
        lpMemRht = MyGlobalLock (lpFX_Params->hGlbRht);

        // Skip over the header to the data
        lpMemRht = VarArrayBaseToData (lpMemRht, lpFX_Params->aplRankRht);

        // Copy the simple char to the result
        *lpMemLine++ = lpMemRht[uLineNum];
        *lpMemLine++ = WC_EOS;

        // We no longer need this ptr
        MyGlobalUnlock (lpFX_Params->hGlbRht); lpMemRht = NULL;
    } // End IF/ELSE
} // End SF_ReadLineSV


//***************************************************************************
//  $SF_ReadLineTF1
//
//  Read in a header/function line
//    when called from []TF with a simple char matrix arg
//***************************************************************************

void SF_ReadLineTF1
    (HWND         hWndEC,           // Edit Ctrl window handle (FE only)
     LPTF1_PARAMS lpTF1_Params,     // Ptr to common struc
     UINT         uLineNum,         // Function line # (0 = header)
     LPAPLCHAR    lpMemLine)        // Ptr to header/line global memory

{
    // Copy the line to global memory
    CopyMemoryW (lpMemLine,
                 lpTF1_Params->lpMemRht + uLineNum * lpTF1_Params->aplColsRht,
                 (APLU3264) lpTF1_Params->aplColsRht);
} // End SF_ReadLineTF1


//***************************************************************************
//  $SF_ReadLineAA
//
//  Read in a header/function line
//    when called from )INASCII with a file whose contents are a
//    simple char vector
//***************************************************************************

void SF_ReadLineAA
    (HWND         hWndEC,           // Edit Ctrl window handle (FE only)
     LPAA_PARAMS  lpAA_Params,      // Ptr to common struc
     UINT         uLineNum,         // Function line # (0 = header)
     LPAPLCHAR    lpMemLine)        // Ptr to header/line global memory

{
    LPWCHAR lpw;

    // Get a ptr to the start of the function lines
    lpw = lpAA_Params->lpwStart;

    // Skip to the designated line #
    while (uLineNum)
    {
        // Skip over the line
        lpw += lstrlenW (lpw);

        // Skip over trailing CR/LFs zapped to zeros
        while (*lpw EQ WC_EOS)
            lpw++;
        // Count out one more line
        uLineNum--;
    } // End WHILE

    // Copy the line to global memory
    CopyMemoryW (lpMemLine,
                 lpw,
                 lstrlenW (lpw));
} // End SF_ReadLineAA


//***************************************************************************
//  $SF_NumLinesCom
//
//  Return the # lines in the function (excluding the header line)
//    when called from []FX with a simple char scalar or vector arg, or
//    when called from []FX with a nested arg
//***************************************************************************

UINT SF_NumLinesCom
    (HWND        hWndEC,            // Edit Ctrl window handle (FE only)
     LPFX_PARAMS lpFX_Params)       // Ptr to common struc

{
    // The # function lines
    return -1 +
      (UINT) lpFX_Params->aplColsRht;
} // End SF_NumLinesCom


//***************************************************************************
//  $SF_NumLinesAN
//
//  Return the # lines in the function (excluding the header line)
//    when saving an AFO
//***************************************************************************

UINT SF_NumLinesAN
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPVOID    lpVoid)              // Ptr to common struc

{
    return 1;
} // End SF_NumLinesAN


//***************************************************************************
//  $SF_NumLinesFE
//
//  Return the # lines in the function (excluding the header line)
//    when called from function editor
//***************************************************************************

UINT SF_NumLinesFE
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPVOID    lpVoid)              // Ptr to common struc

{
    return -1 +
      (UINT) SendMessageW (hWndEC, EM_GETLINECOUNT, 0, 0);
} // End SF_NumLinesFE


//***************************************************************************
//  $SF_NumLinesLW
//
//  Return the # lines in the function (excluding the header line)
//    when called from LoadWorkspace
//***************************************************************************

UINT SF_NumLinesLW
    (HWND        hWndEC,            // Edit Ctrl window handle (FE only)
     LPLW_PARAMS lpLW_Params)       // Ptr to common struc

{
    return -1 +
      ProfileGetInt (lpLW_Params->lpwSectName,  // Ptr to the section name
                      KEYNAME_COUNT,            // Ptr to the key name
                      0,                        // Default value if not found
                      lpLW_Params->lpDict);     // Ptr to workspace dictionary
} // End SF_NumLinesLW


//***************************************************************************
//  $SF_NumLinesM
//
//  Return the # lines in the function (excluding the header line)
//    when called from []FX with a simple char matrix arg
//***************************************************************************

UINT SF_NumLinesM
    (HWND        hWndEC,            // Edit Ctrl window handle (FE only)
     LPFX_PARAMS lpFX_Params)       // Ptr to common struc

{
    // The # function lines
    return -1 +
      (UINT) lpFX_Params->aplRowsRht;
} // End SF_NumLinesM


//***************************************************************************
//  $SF_NumLinesTF1
//
//  Return the # lines in the function (excluding the header line)
//    when called from []TF with a simple char matrix arg
//***************************************************************************

UINT SF_NumLinesTF1
    (HWND         hWndEC,           // Edit Ctrl window handle (FE only)
     LPTF1_PARAMS lpTF1_Params)     // Ptr to common struc

{
    // The # function lines
    return -1 +
      (UINT) lpTF1_Params->aplRowsRht;
} // End SF_NumLinesTF1


//***************************************************************************
//  $SF_NumLinesAA
//
//  Return the # lines in the function (excluding the header line)
//    when called from )INASCII with a file whose contents are a
//    simple char vector
//***************************************************************************

UINT SF_NumLinesAA
    (HWND         hWndEC,           // Edit Ctrl window handle (FE only)
     LPAA_PARAMS  lpAA_Params)      // Ptr to common struc

{
    // The # function lines
    return -1 +
      (UINT) lpAA_Params->NumLines;
} // End SF_NumLinesAA


//***************************************************************************
//  $SF_CreationTimeCom
//
//  Return the creation time
//    when called from function editor, or
//    when called from []FX with a simple char scalar or vector arg, or
//    when called from []FX with a simple char matrix arg, or
//    when called from []FX with a nested arg
//    when called from []TF with a simple char matrix arg
//    when saving an AFO
//***************************************************************************

void SF_CreationTimeCom
    (LPLW_PARAMS lpLW_Params,       // Ptr to common struc
     SYSTEMTIME *lpSystemTime,      // Ptr to current system (UTC) time
     FILETIME   *lpftCreation)      // Ptr to output save area

{
    // Convert system time to file time and save as creation time
    SystemTimeToFileTime (lpSystemTime, lpftCreation);
} // End SF_CreationTimeCom


//***************************************************************************
//  $SF_CreationTimeLW
//
//  Return the creation time
//    when called from LoadWorkspace
//***************************************************************************

void SF_CreationTimeLW
    (LPLW_PARAMS lpLW_Params,       // Ptr to common struc
     SYSTEMTIME *lpSystemTime,      // Ptr to current system (UTC) time
     FILETIME   *lpftCreation)      // Ptr to output save area

{
    // Copy from parameters
    *lpftCreation = lpLW_Params->ftCreation;
} // End SF_CreationTimeLW


//***************************************************************************
//  $SF_LastModTimeCom
//
//  Return the last modification time
//    when called from function editor
//    when called from []FX with a simple char scalar or vector arg
//    when called from []FX with a simple char matrix arg
//    when called from []FX with a nested arg
//    when called from []TF with a simple char matrix arg
//    when saving an AFO
//***************************************************************************

void SF_LastModTimeCom
    (LPLW_PARAMS lpLW_Params,       // Ptr to common struc
     SYSTEMTIME *lpSystemTime,      // Ptr to current system (UTC) time
     FILETIME   *lpftLastMod)       // Ptr to output save area

{
    // Convert system time to file time and save as last modification time
    SystemTimeToFileTime (lpSystemTime, lpftLastMod);
} // End SF_LastModTimeCom


//***************************************************************************
//  $SF_LastModTimeLW
//
//  Return the last modification time
//    when called from LoadWorkspace
//***************************************************************************

void SF_LastModTimeLW
    (LPLW_PARAMS lpLW_Params,       // Ptr to common struc
     SYSTEMTIME *lpSystemTime,      // Ptr to current system (UTC) time
     FILETIME   *lpftLastMod)       // Ptr to output save area

{
    // Copy from parameters
    *lpftLastMod = lpLW_Params->ftLastMod;
} // End SF_LastModTimeLW


//***************************************************************************
//  $SF_UndoBufferCom
//
//  Return a ptr to the Undo buffer
//    when called from []FX with a simple char scalar or vector arg
//    when called from []FX with a simple char matrix arg
//    when called from []FX with a nested arg
//    when called from []TF with a simple char matrix arg
//    when saving an AFO
//***************************************************************************

HGLOBAL SF_UndoBufferCom
    (HWND        hWndEC,            // Edit Ctrl window handle (FE only)
     LPVOID      lpVoid)            // Ptr to common struc

{
    // No Undo Buffer for []FX created functions
    return NULL;
} // End SF_UndoBufferCom


//***************************************************************************
//  $SF_UndoBufferFE
//
//  Return a ptr to the Undo buffer
//    when called from function editor
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SF_UndoBufferFE"
#else
#define APPEND_NAME
#endif

HGLOBAL SF_UndoBufferFE
    (HWND        hWndEC,            // Edit Ctrl window handle (FE only)
     LPVOID      lpVoid)            // Ptr to common struc

{
    HWND         hWndFE;            // Function Editor window handle
    LPUNDO_BUF   lpUndoBeg,         // Ptr to start of Undo buffer
                 lpUndoLst;         // Ptr to end ...
    HGLOBAL      hGlbUndoBuff;      // Undo Buffer global memory handle

    // Get the Function Editor window handle
    hWndFE = GetParent (hWndEC);

    (HANDLE_PTR) lpUndoBeg = GetWindowLongPtrW (hWndFE, GWLSF_UNDO_BEG);
    if (lpUndoBeg)
    {
        LPUNDO_BUF lpMemUndo;       // Ptr to Undo Buffer global memory

        // Get the ptr to the last entry in the Undo Buffer
        (HANDLE_PTR) lpUndoLst = GetWindowLongPtrW (hWndFE, GWLSF_UNDO_LST);

        // Check for empty Undo buffer
        if (lpUndoLst EQ lpUndoBeg)
            hGlbUndoBuff = NULL;
        else
        {
            // Allocate storage for the Undo buffer
            hGlbUndoBuff = DbgGlobalAlloc (GHND, (lpUndoLst - lpUndoBeg) * sizeof (UNDO_BUF));
            if (!hGlbUndoBuff)
            {
                // Display the error message
                MessageBoxW (hWndEC,
                             L"Insufficient memory to save Undo buffer!!",
                             lpwszAppName,
                             MB_OK | MB_ICONWARNING | MB_APPLMODAL);
                SetFocus (GetParent (hWndEC));

                return NULL;
            } // End IF

            // Lock the memory to get a ptr to it
            lpMemUndo = MyGlobalLock (hGlbUndoBuff);

            // Copy the Undo Buffer to global memory
            CopyMemory (lpMemUndo, lpUndoBeg, (lpUndoLst - lpUndoBeg) * sizeof (UNDO_BUF));

            // We no longer need this ptr
            MyGlobalUnlock (hGlbUndoBuff); lpMemUndo = NULL;
        } // End IF/ELSE
    } // End IF

    return hGlbUndoBuff;
} // End SF_UndoBufferFE
#undef  APPEND_NAME


//***************************************************************************
//  $SF_UndoBufferLW
//
//  Return a ptr to the Undo buffer
//    when called from LoadWorkspace
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SF_UndoBufferLW"
#else
#define APPEND_NAME
#endif

HGLOBAL SF_UndoBufferLW
    (HWND        hWndEC,            // Edit Ctrl window handle (FE only)
     LPLW_PARAMS lpLW_Params)       // Ptr to common struc

{
    LPWCHAR    lpMemUndoTxt;        // Ptr to Undo Buffer in text format
    WCHAR      wcAction;            // Temporary character
    LPUNDO_BUF lpMemUndoBin;        // Ptr to Undo Buffer in binary format
    UINT       uUndoCount,          // # entries in the Undo Buffer
               uFields;             // # fields parsed by sscanfW
    HGLOBAL    hGlbUndoBuff;        // Undo Buffer global memory handle

    // Get parameters
    lpMemUndoTxt = lpLW_Params->lpMemUndoTxt;

    // Check for not present
    if (lpMemUndoTxt EQ NULL)
        return NULL;

    // Parse the # entries, and skip over it
    sscanfW (lpMemUndoTxt, L"%d", &uUndoCount);
    lpMemUndoTxt = SkipBlackW (lpMemUndoTxt);

    // Check for empty buffer
    if (*lpMemUndoTxt EQ 0)
        return NULL;

    // Skip over the separating blank
    Assert (*lpMemUndoTxt EQ L' '); lpMemUndoTxt++;

    // Allocate storage for the Undo buffer
    hGlbUndoBuff = DbgGlobalAlloc (GHND, uUndoCount * sizeof (UNDO_BUF));
    if (!hGlbUndoBuff)
    {
        // Display the error message
        MessageBoxW (hWndEC,
                     L"Insufficient memory to save Undo buffer!!",
                     lpwszAppName,
                     MB_OK | MB_ICONWARNING | MB_APPLMODAL);
        SetFocus (GetParent (hWndEC));

        return NULL;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemUndoBin = MyGlobalLock (hGlbUndoBuff);

    // Parse the Undo Buffer entries
    while (*lpMemUndoTxt)
    {
        // Convert the Undo Buffer text to Undo Buffer format

        // Parse the Action, CharPosBeg, CharPosEnd, Group
        uFields =
          sscanfW (lpMemUndoTxt,
                   L"%c %d %d %d '%c'",
                  &wcAction,
                  &lpMemUndoBin->CharPosBeg,
                  &lpMemUndoBin->CharPosEnd,
                  &lpMemUndoBin->Group,
                  &lpMemUndoBin->Char);
        // Convert the Action code to binary
        switch (wcAction)
        {
            case L'N':
                // Save the Action
                lpMemUndoBin->Action = undoNone;

                break;

            case L'I':
                // Save the Action
                lpMemUndoBin->Action = undoIns;

                break;

            case L'R':
                // Save the Action
                lpMemUndoBin->Action = undoRep;

                break;

            case L'D':
                // Save the Action
                lpMemUndoBin->Action = undoDel;

                break;

            case L'S':
                // Save the Action
                lpMemUndoBin->Action = undoSel;

                break;

            case L'T':
                // Save the Action
                lpMemUndoBin->Action = undoInsToggle;

                break;

            defstop
                break;
        } // End SWITCH

        // Split cases based upon the # fields scanned
        switch (uFields)
        {
            case 3:
                Assert (lpMemUndoBin->Action EQ undoSel);

                break;

            case 4:
                Assert (lpMemUndoBin->Action NE undoIns
                     || lpMemUndoBin->Action NE undoRep);
                break;

            case 5:
                // Parse the Char if Ins or Rep
                Assert (lpMemUndoBin->Action EQ undoIns
                     || lpMemUndoBin->Action EQ undoRep);
                if (lpMemUndoBin->Char EQ L'{')
                {
                    // Skip to the left brace char
                    lpMemUndoTxt = SkipToCharW (lpMemUndoTxt, L'{');

                    // Parse the {name}
                    // Because we created this name, we can expect it to be found
                    //   and so don't need to check for zero result
                    lpMemUndoBin->Char = SymbolNameToChar (lpMemUndoTxt);
                } else
                if (lpMemUndoBin->Char EQ WC_SLOPE)
                {
                    // Skip over the slope char
                    lpMemUndoTxt = SkipPastCharW (lpMemUndoTxt, WC_SLOPE);

                    // Split cases based upon the next char
                    switch (*lpMemUndoTxt)
                    {
                        case L'n':
                            lpMemUndoBin->Char = WC_LF;

                            break;

                        case L'r':
                            lpMemUndoBin->Char = WC_CR;

                            break;

                        case WC_SQ:
                            lpMemUndoBin->Char = WC_SQ;

                            break;

                        case WC_SLOPE:
                            lpMemUndoBin->Char = WC_SLOPE;

                            break;

                        defstop
                            break;
                    } // End SWITCH
                } // End IF/ELSE/...

                break;

            defstop
                break;
        } // End SWITCH

        // Skip over the separating comma
        lpMemUndoTxt = SkipPastCharW (lpMemUndoTxt, L',');

        if (*lpMemUndoTxt)
        {
            // Skip over the separating blank
            Assert (*lpMemUndoTxt EQ L' '); lpMemUndoTxt++;
        } // End IF

        // Skip to next entry
        lpMemUndoBin++;
    } // End WHILE

    // We no longer need this ptr
    MyGlobalUnlock (hGlbUndoBuff); lpMemUndoBin = NULL;

    return hGlbUndoBuff;
} // End SF_UndoBufferLW
#undef  APPEND_NAME


//***************************************************************************
//  $SaveFunctionCom
//
//  Attempt to save a function to the WS.
//  Subroutine to SaveFunction; also called directly from []FX.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SaveFunctionCom"
#else
#define APPEND_NAME
#endif

UBOOL SaveFunctionCom
    (HWND      hWndFE,                      // Function Editor window handle (FE only, NULL otherwise)
     LPSF_FCNS lpSF_Fcns)                   // Ptr to common struc

{
    HWND           hWndEC = NULL;           // Edit Ctrl Window handle (FE only)
    UINT           uLineLen;                // Line length
    HGLOBAL        hGlbTxtHdr = NULL,       // Header text global memory handle
                   hGlbTknHdr = NULL,       // Tokenized header text ...
                   hGlbDfnHdr = NULL;       // User-defined function/operator header ...
    LPDFN_HEADER   lpMemDfnHdr = NULL;      // Ptr to user-defined function/operator header ...
    LPMEMTXT_UNION lpMemTxtLine;            // Ptr to header/line text global memory
    FHLOCALVARS    fhLocalVars = {0};       // Function Header local vars
    HGLOBAL        hGlbOldDfn = NULL;       // Old Dfn global memory handle
    LPPERTABDATA   lpMemPTD;                // Ptr to PerTabData global memory
    WCHAR          wszTemp[1024];           // Save area for error message text
    MEMVIRTSTR     lclMemVirtStr[1] = {0};  // Room for one GuardAlloc
    LPTOKEN        lptkCSBeg;               // Ptr to next token on the CS stack

    // Fill in common values
    lpSF_Fcns->bRet     = FALSE;
    lpSF_Fcns->uErrLine = NEG1U;

    Assert ((hWndFE EQ NULL) ? TRUE : IzitFE (hWndFE));

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Save the ptr to the next token on the CS stack
    //   as our beginning
    lptkCSBeg = lpMemPTD->lptkCSNxt;

    // If we're saving an AFO, ...
    if (lpSF_Fcns->bAFO)
    {
        // Allocate a global HshTab
        if (!AllocHshTab (NULL,                     // Ptr to this entry in MemVirtStr (may be NULL if global allocation)
                         &((LPLW_PARAMS) lpSF_Fcns->LclParams)->htsDFN, // Ptr to HshTab Struc
                          DEF_AFO_HSHTAB_NBLKS,     // # blocks in this HshTab
                          DEF_AFO_HSHTAB_INCRNELM,  // # HTEs by which to resize when low
                          DEF_AFO_HSHTAB_MAXNELM))  // Maximum # HTEs
            goto WSFULL_EXIT;

        if (!AllocSymTab (NULL,                     // Ptr to this entry in MemVirtStr (may be NULL if global allocation)
                          lpMemPTD,                 // Ptr to PerTabData global memory
                         &((LPLW_PARAMS) lpSF_Fcns->LclParams)->htsDFN, // Ptr to HshTab Struc
                          DEF_AFO_SYMTAB_INITNELM,  // Initial # STEs in SymTab
                          DEF_AFO_SYMTAB_INCRNELM,  // # STEs by which to resize when low
                          DEF_AFO_SYMTAB_MAXNELM))  // Maximum # STEs
            goto WSFULL_EXIT;

        // Mark as global Hsh & Sym tabs
        ((LPLW_PARAMS) lpSF_Fcns->LclParams)->htsDFN.bGlbHshSymTabs = TRUE;

        // Put the new one into effect so that symbol table
        //   references are made in the new tables
        lpSF_Fcns->lpHTS = &((LPLW_PARAMS) lpSF_Fcns->LclParams)->htsDFN;

        // Append all system names to the local SymTab
        SymTabAppendAllSysNames_EM (&((LPLW_PARAMS) lpSF_Fcns->LclParams)->htsDFN);

        // Assign default values to the system vars
        _AssignDefaultSysVars (lpMemPTD, &((LPLW_PARAMS) lpSF_Fcns->LclParams)->htsDFN);

        // Link the current HshTab & SymTabs to the new
        // This link is what provides static scoping
        ((LPLW_PARAMS) lpSF_Fcns->LclParams)->htsDFN.lphtsPrvSrch = lpMemPTD->lphtsPTD;
    } // End IF

    // Get the handle to the Edit Ctrl
    if (hWndFE)
        (HANDLE_PTR) hWndEC = GetWindowLongPtrW (hWndFE, GWLSF_HWNDEC);

    // Get the length of the header line
    uLineLen = (*lpSF_Fcns->SF_LineLen) (hWndEC, lpSF_Fcns->LclParams, 0);

    // Allocate space for the text
    //   (the "sizeof (uLineLen)" is for the leading line length
    //    and the "+ 1" is for the terminating zero)
    // Note, we can't use DbgGlobalAlloc here as we
    //   might have been called from the Master Frame
    //   via a system command, in which case there is
    //   no PTD for that thread.
    hGlbTxtHdr =
      MyGlobalAlloc (GHND, sizeof (lpMemTxtLine->U) + (uLineLen + 1) * sizeof (lpMemTxtLine->C));
    if (!hGlbTxtHdr)
    {
        // Mark the line in error
        lpSF_Fcns->uErrLine = 0;

        if (hWndFE)
        {
            // Display the error message
            MessageBoxW (hWndEC,
                         L"Insufficient memory to save the function header text!!",
                         lpwszAppName,
                         MB_OK | MB_ICONWARNING | MB_APPLMODAL);
            SetFocus (GetParent (hWndEC));
        } else
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                       lpSF_Fcns->lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // The following test isn't an optimzation, but avoids
    //   overwriting the allocation limits of the buffer
    //   when filling in the leading WORD with uLineLen
    //   on the call to EM_GETLINE.

    // If the header is non-empty, ...
    if (uLineLen)
    {
        // Lock the memory to get a ptr to it
        lpMemTxtLine = MyGlobalLock (hGlbTxtHdr);

        // Save the line length
        lpMemTxtLine->U = uLineLen;

        // Tell EM_GETLINE maximum # chars in the buffer
        lpMemTxtLine->W = (WORD) uLineLen;

        // Read in the line text
        (*lpSF_Fcns->SF_ReadLine) (hWndEC, lpSF_Fcns->LclParams, 0, &lpMemTxtLine->C);

        // Tokenize the function header
        hGlbTknHdr =
          Tokenize_EM (&lpMemTxtLine->C,        // The line to tokenize (not necessarily zero-terminated)
                        uLineLen,               // NELM of lpwszLine
                        hWndEC,                 // Window handle for Edit Ctrl (may be NULL if lpErrHandFn is NULL)
                        0,                      // Function line # (0 = header)
                       &ErrorHandler,           // Ptr to error handling function (may be NULL)
                        lpSF_Fcns,              // Ptr to common struc (may be NULL if unused)
                        FALSE);                 // TRUE iff we're tokenizing a Magic Function/Operator
        // We no longer need this ptr
        MyGlobalUnlock (hGlbTxtHdr); lpMemTxtLine = NULL;
    } // End IF

    if (!hGlbTknHdr)
    {
        // Mark the line in error
        lpSF_Fcns->uErrLine = 0;

        if (hWndFE)
        {
            // Format the error message
            wsprintfW (wszTemp,
                       ERRMSG_SYNTAX_ERROR_IN_FUNCTION_HEADER APPEND_NAME,
                       lpMemPTD->uCaret);
            // Display the error message
            MessageBoxW (hWndEC,
                        wszTemp,
                        lpwszAppName,
                        MB_OK | MB_ICONWARNING | MB_APPLMODAL);
            SetFocus (GetParent (hWndEC));
        } // End IF

        goto ERROR_EXIT;
    } // End IF

    // Fill in fhLocalvars
    fhLocalVars.bAFO = lpSF_Fcns->bAFO;

    // Allocate virtual memory for the Variable Strand accumulator
    lclMemVirtStr[0].lpText   = "fhLocalvars.lpYYStrandStart in <SaveFunctionCom>";
    lclMemVirtStr[0].IncrSize = DEF_STRAND_INCRNELM * sizeof (PL_YYSTYPE);
    lclMemVirtStr[0].MaxSize  = DEF_STRAND_MAXNELM  * sizeof (PL_YYSTYPE);
    lclMemVirtStr[0].IniAddr  = (LPUCHAR)
    fhLocalVars.lpYYStrandStart =
      GuardAlloc (NULL,             // Any address
                  lclMemVirtStr[0].MaxSize,
                  MEM_RESERVE,
                  PAGE_READWRITE);
    if (!lclMemVirtStr[0].IniAddr)
    {
        if (hWndFE)
        {
            // Display the error message
            MessageBoxW (hWndEC,
                         L"Insufficient memory to save the function header strand stack!!",
                         lpwszAppName,
                         MB_OK | MB_ICONWARNING | MB_APPLMODAL);
            SetFocus (GetParent (hWndEC));
        } else
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                       lpSF_Fcns->lptkFunc);
        goto ERROR_EXIT;        // Mark as failed
    } // End IF

    // Link this struc into the chain
    LinkMVS (&lclMemVirtStr[0]);

    // Commit the intial size
    MyVirtualAlloc (lclMemVirtStr[0].IniAddr,
                    DEF_STRAND_INITNELM * sizeof (PL_YYSTYPE),
                    MEM_COMMIT,
                    PAGE_READWRITE);
    // Parse the function header
    if (ParseFcnHeader (hWndEC, hGlbTknHdr, &fhLocalVars, lpSF_Fcns->bDisplayErr))
    {
        UINT         uLineNum,          // Current line # in the Edit Ctrl
                     uOffset,           // Cumulative offset
                     numResultSTE,      // # result STEs (may be zero)
                     numLftArgSTE,      // # left arg ...
                     numRhtArgSTE,      // # right ...
                     numLocalsSTE,      // # locals ...
                     numFcnLines,       // # lines in the function
                     numSTE;            // Loop counter
        LPFCNLINE    lpFcnLines;        // Ptr to array of function line structs (FCNLINE[numFcnLines])
        LPSYMENTRY   lpSymName = NULL,  // Ptr to SYMENTRY for the function name
                    *lplpSymDfnHdr;     // Ptr to LPSYMENTRYs at end of user-defined function/operator header
        SYSTEMTIME   systemTime;        // Current system (UTC) time
        FILETIME     ftCreation;        // Creation time
        CSLOCALVARS  csLocalVars = {0}; // CS local vars

        // Check on invalid function name (e.g. empty function header/body)
        if (!fhLocalVars.lpYYFcnName)
        {
            // Mark the line in error
            lpSF_Fcns->uErrLine = 0;

            goto ERROR_EXIT;
        } // End IF

        // Get the current system (UTC) time
        GetSystemTime (&systemTime);

        // If this is not an AFO, ...
        if (!fhLocalVars.bAFO)
        {
            // Check to see if this function is already in global memory
            lpSymName = fhLocalVars.lpYYFcnName->tkToken.tkData.tkSym;

            // Get the old Dfn global memory handle
            hGlbOldDfn = lpSymName->stData.stGlbData;
        } // End IF

        // If it's already in memory, get its creation time
        //   and then free it
        if (hGlbOldDfn)
        {
            LPSIS_HEADER lpSISCur;

            // Get a ptr to the current SI stack
            lpSISCur = lpMemPTD->lpSISCur;

            // Check for already on the SI stack
            for (;
                 lpSISCur;
                 lpSISCur = lpSISCur->lpSISPrv)
            if (lpSISCur->hGlbDfnHdr EQ hGlbOldDfn)
            {
                if (hWndFE)
                {
                    // Display the error message
                    MessageBoxW (hWndEC,
                                 L"SI Damage in pending function:  changes to this function NOT saved",
                                 lpwszAppName,
                                 MB_OK | MB_ICONWARNING | MB_APPLMODAL);
                    SetFocus (GetParent (hWndEC));
                } else
                    ErrorMessageIndirectToken (ERRMSG_SI_DAMAGE APPEND_NAME,
                                               lpSF_Fcns->lptkFunc);
                goto ERROR_EXIT;
            } // End FOR/IF

            // Ensure it's a UDFO
            if (IsGlbTypeDfnDir_PTB (MakePtrTypeGlb (hGlbOldDfn)))
            {
                // Lock the memory to get a ptr to it
                lpMemDfnHdr = MyGlobalLock (hGlbOldDfn);

                // Get the creation time
                ftCreation = lpMemDfnHdr->ftCreation;

                // We no longer need this ptr
                MyGlobalUnlock (hGlbOldDfn); lpMemDfnHdr = NULL;
            } else
            {
                if (hWndFE)
                {
                    // Format the error message
                    wsprintfW (wszTemp,
                               L"New object name is <%s>, not a user-defined function/operator:  changes to this function NOT saved",
                               lpwNameTypeStr[lpSymName->stFlags.stNameType]);
                    // Display the error message
                    MessageBoxW (hWndEC,
                                 wszTemp,
                                 lpwszAppName,
                                 MB_OK | MB_ICONWARNING | MB_APPLMODAL);
                    SetFocus (GetParent (hWndEC));
                } else
                    ErrorMessageIndirectToken (ERRMSG_NOT_SAVED_FILE_ERROR APPEND_NAME,
                                               lpSF_Fcns->lptkFunc);
                goto ERROR_EXIT;
            } // End IF/ELSE
        } else
            (*lpSF_Fcns->SF_CreationTime) (lpSF_Fcns->LclParams, &systemTime, &ftCreation);

        // Get # extra result STEs
        if (fhLocalVars.lpYYResult)
            // Save in global memory
            numResultSTE = fhLocalVars.lpYYResult->uStrandLen;
        else
            numResultSTE = 0;

        // Get # extra left arg STEs
        if (fhLocalVars.lpYYLftArg)
            // Save in global memory
            numLftArgSTE = fhLocalVars.lpYYLftArg->uStrandLen;
        else
            numLftArgSTE = 0;

        // Get # extra right arg STEs
        if (fhLocalVars.lpYYRhtArg)
            // Save in global memory
            numRhtArgSTE = fhLocalVars.lpYYRhtArg->uStrandLen;
        else
            numRhtArgSTE = 0;

        // Get # locals STEs
        if (fhLocalVars.lpYYLocals)
            // Save in global memory
            numLocalsSTE = fhLocalVars.lpYYLocals->uStrandLen;
        else
            numLocalsSTE = 0;

        // Get # lines in the function (excluding the header)
        numFcnLines = (*lpSF_Fcns->SF_NumLines) (hWndEC, lpSF_Fcns->LclParams);

        // Get size of tokenization of all lines (excluding the header)
        for (uOffset = uLineNum = 0; uLineNum < numFcnLines; uLineNum++)
            // Size a function line
            if (SaveFunctionLine (lpSF_Fcns, NULL, NULL, uLineNum, NULL, hWndEC, hWndFE, &uOffset) EQ -1)
                goto ERROR_EXIT;
        // Restore the ptr to the next token on the CS stack
        //   because we advanced it during the above sizing
        lpMemPTD->lptkCSNxt = lptkCSBeg;

        // If we're saving an AFO, ...
        if (lpSF_Fcns->bAFO)
        {
            Assert (numLocalsSTE EQ 0);

            // Count in the # locals found during sizing
            numLocalsSTE += lpSF_Fcns->numLocalsSTE;
        } // End IF

        // Allocate global memory for the function header
        lpSF_Fcns->hGlbDfnHdr =
        hGlbDfnHdr =
          DbgGlobalAlloc (GHND, sizeof (DFN_HEADER)
                              + sizeof (LPSYMENTRY) * (numResultSTE
                                                     + numLftArgSTE
                                                     + numRhtArgSTE
                                                     + numLocalsSTE)
                              + sizeof (FCNLINE) * numFcnLines
                              + uOffset);
        if (!hGlbDfnHdr)
        {
            if (hWndFE)
            {
                // Display the error message
                MessageBoxW (hWndEC,
                             L"Insufficient memory to save the function header!!",
                             lpwszAppName,
                             MB_OK | MB_ICONWARNING | MB_APPLMODAL);
                SetFocus (GetParent (hWndEC));
            } else
                ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                           lpSF_Fcns->lptkFunc);
            goto ERROR_EXIT;
        } // End IF
#ifdef DEBUG
////    // If it's named <times>, ...
////    if (lstrcmpW ((*lpSymName->stHshEntry->lplpaplChar6)->aplChar, L"times") EQ 0)
////        hGlbRC1 = hGlbDfnHdr;
////    // If it's named <from>, ...
////    if (lstrcmpW ((*lpSymName->stHshEntry->lplpaplChar6)->aplChar, L"rle4" ) EQ 0)
////        hGlbRC2 = hGlbDfnHdr;
#endif
        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLock (hGlbDfnHdr);

        // Save numbers in global memory
        lpMemDfnHdr->numResultSTE = numResultSTE;
        lpMemDfnHdr->numLftArgSTE = numLftArgSTE;
        lpMemDfnHdr->numRhtArgSTE = numRhtArgSTE;
        lpMemDfnHdr->numLocalsSTE = numLocalsSTE;

        // Save the static parts of the function into global memory
        lpMemDfnHdr->Sig.nature   = DFN_HEADER_SIGNATURE;
        lpMemDfnHdr->Version      = 0;
        lpMemDfnHdr->DfnType      = fhLocalVars.DfnType;
        lpMemDfnHdr->FcnValence   = fhLocalVars.FcnValence;
        lpMemDfnHdr->DfnAxis      = fhLocalVars.DfnAxis;
        lpMemDfnHdr->NoDispRes    = fhLocalVars.NoDispRes;
        lpMemDfnHdr->ListRes      = fhLocalVars.ListRes;
        lpMemDfnHdr->ListLft      = fhLocalVars.ListLft;
        lpMemDfnHdr->ListRht      = fhLocalVars.ListRht;
        lpMemDfnHdr->bAFO         = fhLocalVars.bAFO;
        lpMemDfnHdr->RefCnt       = 1;
        lpMemDfnHdr->numFcnLines  = numFcnLines;
        lpMemDfnHdr->steLftOpr    = fhLocalVars.lpYYLftOpr
                                  ? fhLocalVars.lpYYLftOpr ->tkToken.tkData.tkSym
                                  : NULL;
        lpMemDfnHdr->steFcnName   = fhLocalVars.lpYYFcnName->tkToken.tkData.tkSym;
        lpMemDfnHdr->steAxisOpr   = fhLocalVars.lpYYAxisOpr
                                  ? fhLocalVars.lpYYAxisOpr->tkToken.tkData.tkSym
                                  : NULL;
        lpMemDfnHdr->steRhtOpr    = fhLocalVars.lpYYRhtOpr
                                  ? fhLocalVars.lpYYRhtOpr ->tkToken.tkData.tkSym
                                  : NULL;
        lpMemDfnHdr->hGlbTxtHdr   = hGlbTxtHdr;
        lpMemDfnHdr->hGlbTknHdr   = hGlbTknHdr;
////////lpMemDfnHdr->hGlbMonInfo  = NULL;           // Already zero from GHND

        // Save creation time
        lpMemDfnHdr->ftCreation = ftCreation;

        // Get the function's last modification time
        (*lpSF_Fcns->SF_LastModTime) (lpSF_Fcns->LclParams, &systemTime, &lpMemDfnHdr->ftLastMod);

        // Get the ptr to the start of the Undo Buffer
        lpMemDfnHdr->hGlbUndoBuff = (*lpSF_Fcns->SF_UndoBuffer) (hWndEC, lpSF_Fcns->LclParams);

        // Save the dynamic parts of the function into global memory

        // Initialize cumulative offset
        uOffset = sizeof (DFN_HEADER);

        // Initialize ptr to ptr to SYMENTRYs at end of header
        lplpSymDfnHdr = (LPAPLHETERO) ByteAddr (lpMemDfnHdr, uOffset);

        // If there's a result, ...
        if (fhLocalVars.lpYYResult)
        {
            // Save the current offset from lpMemDfnHdr
            lpMemDfnHdr->offResultSTE = uOffset;

            // Copy STEs to global memory
            for (numSTE = 0; numSTE < numResultSTE; numSTE++)
            {
                *lplpSymDfnHdr++ = fhLocalVars.lpYYResult[numSTE].tkToken.tkData.tkSym;
                uOffset += sizeof (LPSYMENTRY);
            } // End FOR
        } else
            lpMemDfnHdr->offResultSTE = 0;

        // If there's a left arg, ...
        if (fhLocalVars.lpYYLftArg)
        {
            // Save the current offset from lpMemDfnHdr
            lpMemDfnHdr->offLftArgSTE = uOffset;

            // Copy STEs to global memory
            for (numSTE = 0; numSTE < numLftArgSTE; numSTE++)
            {
                *lplpSymDfnHdr++ = fhLocalVars.lpYYLftArg[numSTE].tkToken.tkData.tkSym;
                uOffset += sizeof (LPSYMENTRY);
            } // End FOR
        } else
            lpMemDfnHdr->offLftArgSTE = 0;

        // If there's a right arg, ...
        if (fhLocalVars.lpYYRhtArg)
        {
            // Save the current offset from lpMemDfnHdr
            lpMemDfnHdr->offRhtArgSTE = uOffset;

            // Copy STEs to global memory
            for (numSTE = 0; numSTE < numRhtArgSTE; numSTE++)
            {
                *lplpSymDfnHdr++ = fhLocalVars.lpYYRhtArg[numSTE].tkToken.tkData.tkSym;
                uOffset += sizeof (LPSYMENTRY);
            } // End FOR
        } else
            lpMemDfnHdr->offRhtArgSTE = 0;

        // If there are any locals, ...
        if (fhLocalVars.lpYYLocals)
        {
            // Save the current offset from lpMemDfnHdr
            lpMemDfnHdr->offLocalsSTE = uOffset;

            // Copy STEs to global memory
            for (numSTE = 0; numSTE < numLocalsSTE; numSTE++)
            {
                *lplpSymDfnHdr++ = fhLocalVars.lpYYLocals[numSTE].tkToken.tkData.tkSym;
                uOffset += sizeof (LPSYMENTRY);
            } // End FOR
        } else
        if (numLocalsSTE)
        {
            Assert (lpSF_Fcns->bAFO);
            Assert (lpMemDfnHdr->offLocalsSTE EQ 0);

            // Save the current offset from lpMemDfnHdr
            lpMemDfnHdr->offLocalsSTE = uOffset;

            // Save a ptr to the save area for local STEs
            lpSF_Fcns->lplpLocalSTEs = lplpSymDfnHdr;

            // Make room for the AFO STEs
            lplpSymDfnHdr += numLocalsSTE;
            uOffset       += numLocalsSTE * sizeof (LPSYMENTRY);
        } else
            lpMemDfnHdr->offLocalsSTE = 0;

        // Save offset to start of function line structs
        lpMemDfnHdr->offFcnLines = uOffset;

        // Save offset to tokenized lines
        lpMemDfnHdr->offTknLines = uOffset + numFcnLines * sizeof (FCNLINE);

        // Get ptr to array of function line structs (FCNLINE[numFcnLines])
        lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

        // Initialize the offset of where to start saving the tokenized lines
        uOffset = lpMemDfnHdr->offTknLines;

        // Loop through the lines
        for (uLineNum = 0; uLineNum < numFcnLines; uLineNum++)
        {
            // Save a function line
            uLineLen =
              SaveFunctionLine (lpSF_Fcns, NULL, lpMemDfnHdr, uLineNum, lpFcnLines, hWndEC, hWndFE, &uOffset);
            if (uLineLen EQ -1)
                goto ERROR_EXIT;

            // If tokenization failed, ...
            if (!lpFcnLines->offTknLine)
            {
                if (hWndFE)
                {
                    // Format the error message
                    wsprintfW (wszTemp,
                               L"SYNTAX ERROR on line # %d, position %d -- function not saved",
                               uLineNum + 1,
                               lpMemPTD->uCaret);
                    // Display the error message
                    MessageBoxW (hWndEC,
                                 wszTemp,
                                 lpwszAppName,
                                 MB_OK | MB_ICONWARNING | MB_APPLMODAL);
                    SetFocus (GetParent (hWndEC));
                } else
                    // Save the line # in error (origin-0)
                    lpSF_Fcns->uErrLine = uLineNum + 1;

                goto ERROR_EXIT;
            } // End IF

            // Transfer Stop & Trace info
            lpFcnLines->bStop  =
            lpFcnLines->bTrace = FALSE;     // ***FIXME*** -- transfer from orig fn

            // Skip to the next struct
            lpFcnLines++;
        } // End FOR

        // Fill in the CS local vars struc
        csLocalVars.hWndEC      = hWndEC;
        csLocalVars.lpMemPTD    = lpMemPTD;
        csLocalVars.lptkCSBeg   =
        csLocalVars.lptkCSNxt   = lptkCSBeg;
        csLocalVars.lptkCSLink  = NULL;
        csLocalVars.hGlbDfnHdr  = hGlbDfnHdr;
        csLocalVars.hGlbImmExec = NULL;

        // Parse the tokens on the CS stack
        if (!ParseCtrlStruc_EM (&csLocalVars))
        {
            if (hWndFE)
            {
                // Format the error message
                wsprintfW (wszTemp,
                           L"%s on line # %d, statement #%d, position %d -- function not saved",
                           csLocalVars.lpwszErrMsg,
                           csLocalVars.tkCSErr.tkData.Orig.c.uLineNum,
                           csLocalVars.tkCSErr.tkData.Orig.c.uStmtNum + 1,
                           lpMemPTD->uCaret);
                // Display the error message
                MessageBoxW (hWndEC,
                             wszTemp,
                             lpwszAppName,
                             MB_OK | MB_ICONWARNING | MB_APPLMODAL);
                SetFocus (GetParent (hWndEC));
            } else
                // Save the line # in error (origin-0)
                lpSF_Fcns->uErrLine = uLineNum + 1;

            goto ERROR_EXIT;
        } // End IF

        // Save flag if Ctrl Strucs in AFOs
        lpMemDfnHdr->bAfoCtrlStruc = (lpSF_Fcns->bAFO
                                    && csLocalVars.bMainStmt);
        // Check for line labels ([]ID, etc.)
        if (!GetLabelNums (lpMemDfnHdr, hWndEC, hWndFE NE NULL, lpSF_Fcns))
            goto ERROR_EXIT;

        // If there was a previous function, ...
        if (hGlbOldDfn)
        {
            // Free it
            FreeResultGlobalDfn (hGlbOldDfn); hGlbOldDfn = NULL;
        } // End IF

        // If we're parsing an AFO, ...
        if (lpSF_Fcns->bAFO)
        {
            // Save the new HTS in global memory
            lpMemDfnHdr->htsDFN = ((LPLW_PARAMS) lpSF_Fcns->LclParams)->htsDFN;

            // Set the function/operator valence based upon the Set/RefAlpha bits
            switch (lpSF_Fcns->bSetAlpha * 2 + lpSF_Fcns->bRefAlpha)
            {
                case 2 * 0 + 0:         // !bSetAlpha   !bRefAlpha
                    if (lpSF_Fcns->bRefOmega)
                    {
                        // Monadic function/operator
                        lpMemDfnHdr->FcnValence   = FCNVALENCE_MON;
                        lpMemDfnHdr->numLftArgSTE = 0;
                    } else
                    {
                        // Niladic function/operator
                        lpMemDfnHdr->FcnValence   = FCNVALENCE_NIL;
                        lpMemDfnHdr->numLftArgSTE =
                        lpMemDfnHdr->numRhtArgSTE = 0;
                    } // End IF/ELSE

                    break;

                case 2 * 0 + 1:         // !bSetAlpha    bRefAlpha
                    // Dyadic function/operator
                    lpMemDfnHdr->FcnValence = FCNVALENCE_DYD;

                    break;

                case 2 * 1 + 0:         //  bSetAlpha   !bRefAlpha
                defstop
                    // Can't happen

                    break;

                case 2 * 1 + 1:         //  bSetAlpha    bRefAlpha
                    // Ambivalent function/operator (already set)
                    Assert (lpMemDfnHdr->FcnValence EQ FCNVALENCE_AMB);

////////////////////lpMemDfnHdr->FcnValence = FCNVALENCE_AMB;

                    break;
            } // End SWITCH
        } else
        {
            // Save the global memory handle in the STE
            lpSymName->stData.stGlbData = MakePtrTypeGlb (hGlbDfnHdr);

            // Mark as valued and user-defined function/operator
            lpSymName->stFlags.Value  =
            lpSymName->stFlags.UsrDfn = TRUE;

            // Copy the "Accepts Axis Operator" flag
            lpSymName->stFlags.DfnAxis = lpMemDfnHdr->DfnAxis;

            // Set the object name
            lpSymName->stFlags.ObjName = OBJNAME_USR;

            // Mark as with the proper type and valence

            // Split cases based upon the function type
            switch (lpMemDfnHdr->DfnType)
            {
                case DFNTYPE_OP1:   // Monadic operator
                    lpSymName->stFlags.stNameType = NAMETYPE_OP1;

                    break;

                case DFNTYPE_OP2:   // Dyadic operator
                    lpSymName->stFlags.stNameType = NAMETYPE_OP2;

                    break;

                case DFNTYPE_FCN:   // Function
                    // Split cases based upon the function valence
                    switch (lpMemDfnHdr->FcnValence)
                    {
                        case FCNVALENCE_NIL:    // Niladic function
                            lpSymName->stFlags.stNameType = NAMETYPE_FN0;

                            break;

                        case FCNVALENCE_MON:    // Monadic function
                        case FCNVALENCE_DYD:    // Dyadic function
                        case FCNVALENCE_AMB:    // Ambivalent function
                            lpSymName->stFlags.stNameType = NAMETYPE_FN12;

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case DFNTYPE_UNK:   // Unknown
                defstop
                    break;
            } // End SWITCH
        } // End IF/ELSE

        // If the caller is the Function Editor, ...
        if (hWndFE)
        {
            // Mark as unchanged since the last save
            SetWindowLongW (hWndFE, GWLSF_CHANGED, FALSE);

            // Write out the FE window title to unmark the Changed flag
            SetFETitle (hWndFE);
        } // End IF

        // We no longer need this ptr
        MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;

        lpSF_Fcns->bRet      = TRUE;
        lpSF_Fcns->lpSymName = lpSymName;

        goto NORMAL_EXIT;
    } else
    {
        // Copy the error message up the line
        lstrcpyW (lpSF_Fcns->wszErrMsg, fhLocalVars.wszErrMsg);

        // Copy the error line # up the line
        lpSF_Fcns->uErrLine = 0;
    } // End IF/ELSE

    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lpSF_Fcns->lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hWndFE)
        // Ensure the FE window redraws the caret
        SetFocus (hWndFE);

    if (hGlbDfnHdr)
    {
        if (lpMemDfnHdr)
        {
            // Mark whether or not to save the function name STE flags
            lpMemDfnHdr->SaveSTEFlags = (hGlbOldDfn NE NULL);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalDfn (hGlbDfnHdr); hGlbDfnHdr = NULL;
        hGlbTknHdr = hGlbTxtHdr = NULL;
    } // End IF

    if (hGlbTknHdr)
    {
        LPTOKEN_HEADER lpMemTknHdr;         // Ptr to tokenized line header global memory

        // Lock the memory to get a ptr to it
        lpMemTknHdr = MyGlobalLock (hGlbTknHdr);

        // Free the tokens
        Untokenize (lpMemTknHdr);

        // We no Longer need this ptr
        MyGlobalUnlock (hGlbTknHdr); lpMemTknHdr = NULL;

        // We no longer need this storage
        DbgGlobalFree (hGlbTknHdr); hGlbTknHdr = NULL;
    } // End IF

    if (hGlbTxtHdr)
    {
        // We no longer need this storage
        MyGlobalFree (hGlbTxtHdr); hGlbTxtHdr = NULL;
    } // End IF
NORMAL_EXIT:
    // Restore the ptr to the next token on the CS stack
    lpMemPTD->lptkCSNxt = lptkCSBeg;

    // If we allocated virtual storage, ...
    if (lclMemVirtStr[0].IniAddr)
    {
        // Free the virtual storage
        MyVirtualFree (lclMemVirtStr[0].IniAddr, 0, MEM_RELEASE); lclMemVirtStr[0].IniAddr = NULL;
        fhLocalVars.lpYYStrandStart = NULL;

        // Unlink this entry from the chain
        UnlinkMVS (&lclMemVirtStr[0]);
    } // End IF

    return lpSF_Fcns->bRet;
} // End SaveFunctionCom
#undef  APPEND_NAME


//***************************************************************************
//  $SaveFunctionLine
//
//  Save one function line or
//    size one function line
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SaveFunctionLine"
#else
#define APPEND_NAME
#endif

UINT SaveFunctionLine
    (LPSF_FCNS      lpSF_Fcns,              // Ptr to common struc (may be NULL if unused)
     LPMAGIC_FCNOPR lpMagicFcnOpr,          // Ptr to magic function/operator struc (is not NULL if Magic Functions)
     LPDFN_HEADER   lpMemDfnHdr,            // Ptr to user-defined function/operator header (may be NULL if sizing)
     UINT           uLineNum,               // Current line # in the Edit Ctrl
     LPFCNLINE      lpFcnLines,             // Ptr to array of function line structs (FCNLINE[numFcnLines]) (may be NULL if sizing)
     HWND           hWndEC,                 // Edit Ctrl Window handle (FE only)
     HWND           hWndFE,                 // Function Editor window handle (FE only, NULL otherwise)
     LPUINT         lpOffNextTknLine)       // Ptr to offset of next tokenized line

{
    UINT           uLineLen,                // Line length
                   uTknSize;                // Token size
    HGLOBAL        hGlbTxtLine,             // Line text global memory handle
                   hGlbTknHdr;              // Tokenized line text global memory handle
    LPTOKEN_HEADER lpMemTknHdr;             // Ptr to tokenized line header global memory
    LPMEMTXT_UNION lpMemTxtLine;            // Ptr to header/line text global memory
    LPAPLCHAR      lpwszLine;               // Ptr to function line text
    WCHAR          wszTemp[1024];           // Save area for error message text
    LPPERTABDATA   lpMemPTD;                // Ptr to PerTabData global memory
    APLU3264       uLen;                    // Temporary var

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // If it's a Magic Function, ...
    if (lpMagicFcnOpr)
        // Get the line length of the line
        uLineLen = lstrlenW (lpMagicFcnOpr->Body[uLineNum]);
    else
        // Get the length of the function line
        uLineLen = (*lpSF_Fcns->SF_LineLen) (hWndEC, lpSF_Fcns->LclParams, uLineNum + 1);

    // Calculate extra WCHARs in case we need to surround the line with braces
    uLen = 2 * (lpSF_Fcns && lpSF_Fcns->bAFO && !lpSF_Fcns->bMakeAFO);

    // Allocate global memory to hold this text
    // The "sizeof (lpMemTxtLine->U) + " is for the leading length
    //   and the "+ 1" is for the terminating zero
    //   and the "+ uLen" is for the extra WCHARs in case we need them for AFOs
    //   as well as to handle GlobalLock's aversion to locking
    //   zero-length arrays
    hGlbTxtLine = DbgGlobalAlloc (GHND, sizeof (lpMemTxtLine->U) + (uLineLen + 1 + uLen) * sizeof (APLCHAR));
    if (!hGlbTxtLine)
    {
        if (hWndFE)
        {
            // Display the error message
            MessageBoxW (hWndEC,
                         L"Insufficient memory to save a function line!!",
                         lpwszAppName,
                         MB_OK | MB_ICONWARNING | MB_APPLMODAL);
            SetFocus (GetParent (hWndEC));

            goto ERROR_EXIT;
        } else
            goto WSFULL_EXIT;
    } // End IF

    // If we're not sizing, ...
    if (lpFcnLines)
        // Save the global memory handle
        lpFcnLines->hGlbTxtLine = hGlbTxtLine;

    // Lock the memory to get a ptr to it
    lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

    // Save the line length
    lpMemTxtLine->U = uLineLen;

    // The following test isn't an optimzation, but avoids
    //   overwriting the allocation limits of the buffer
    //   when filling in the leading WORD with uLineLen
    //   on the call to EM_GETLINE.

    // If the line is non-empty, ...
    if (uLineLen)
    {
        // Tell EM_GETLINE maximum # chars in the buffer
        lpMemTxtLine->W = (WORD) uLineLen;

        // Point to the start of the line
        lpwszLine = &lpMemTxtLine->C;

        // If it's a Magic Function, ...
        if (lpMagicFcnOpr)
            // Copy the line text to global memory
            CopyMemoryW (lpwszLine, lpMagicFcnOpr->Body[uLineNum], uLineLen);
        else
        {
            // Read in the line text
            (*lpSF_Fcns->SF_ReadLine) (hWndEC, lpSF_Fcns->LclParams, uLineNum + 1, lpwszLine);

            // If we're about to tokenize an AFO and we're not called from MakeAFO, ...
            if (lpSF_Fcns->bAFO
             && !lpSF_Fcns->bMakeAFO)
            {
                // Make room for a leading left brace
                MoveMemory (&lpwszLine[1], lpwszLine, (uLineLen + 1) * sizeof (lpwszLine[0]));

                // Insert a leading left brace
                lpwszLine[0           ] = UTF16_LEFTBRACE;

                // Append a trailing right brace & EOS
                lpwszLine[uLineLen + 1] = UTF16_RIGHTBRACE;
////////////////lpwszLine[uLineLen + 2] = WC_EOS;       // Already zero from GHND

                // Tokenize the line so as to determine operator valence (if it's an operator)
                //   and whether or not the (derived) function is niladic
                hGlbTknHdr =
                  Tokenize_EM (lpwszLine,               // The line to tokenize (not necessarily zero-terminated)
                               uLineLen + 2,            // NELM of lpwszLine (including surrounding braces)
                               hWndEC,                  // Window handle for Edit Ctrl (may be NULL if lpErrHandFn is NULL)
                               uLineNum + 1,            // Function line # (0 = header)
                              &ErrorHandler,            // Ptr to error handling function (may be NULL)
                               lpSF_Fcns,               // Ptr to common struc (may be NULL if unused)
                               lpMagicFcnOpr
                            || (lpSF_Fcns
                             && lpSF_Fcns->bMFO)
                            || (lpMemDfnHdr
                             && lpMemDfnHdr->bMFO));    // TRUE iff we're tokenizing a Magic Function/Operator
                // We no longer need this storage
                MyGlobalFree (hGlbTknHdr); hGlbTknHdr = NULL;

                // Delete the surrounding braces to bring it back to where it was
                CopyMemory (lpwszLine, &lpwszLine[1], uLineLen * sizeof (lpwszLine[0]));

                // Terminate the line
                lpwszLine[uLineLen] = WC_EOS;
            } // End IF
        } // End IF/ELSE
    } else
        lpwszLine = L"";

    // Tokenize the line
    hGlbTknHdr =
      Tokenize_EM (lpwszLine,               // The line to tokenize (not necessarily zero-terminated)
                   uLineLen,                // NELM of lpwszLine
                   hWndEC,                  // Window handle for Edit Ctrl (may be NULL if lpErrHandFn is NULL)
                   uLineNum + 1,            // Function line # (0 = header)
                  &ErrorHandler,            // Ptr to error handling function (may be NULL)
                   lpSF_Fcns,               // Ptr to common struc (may be NULL if unused)
                   lpMagicFcnOpr
                || (lpSF_Fcns
                 && lpSF_Fcns->bMFO)
                || (lpMemDfnHdr
                 && lpMemDfnHdr->bMFO));    // TRUE iff we're tokenizing a Magic Function/Operator
    // We no longer need this ptr
    MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;

    // If we're sizing, ...
    if (!lpFcnLines)
    {
        // We no longer need this storage
        MyGlobalFree (hGlbTxtLine); hGlbTxtLine = NULL;
    } // End IF

    // If tokenization failed, ...
    if (!hGlbTknHdr)
    {
        // Mark the line in error
        lpSF_Fcns->uErrLine = uLineNum + 1;

        if (hWndFE)
        {
            // Format the error message
            wsprintfW (wszTemp,
                       ERRMSG_SYNTAX_ERROR_IN_FUNCTION_LINE APPEND_NAME,
                       uLineNum + 1,
                       lpMemPTD->uCaret);
            // Display the error message
            MessageBoxW (hWndEC,
                        wszTemp,
                        lpwszAppName,
                        MB_OK | MB_ICONWARNING | MB_APPLMODAL);
            SetFocus (GetParent (hWndEC));
        } // End IF

        goto ERROR_EXIT;
    } // End IF

    // If we're not sizing, ...
    if (lpFcnLines)
        // Check the line for empty
        lpFcnLines->bEmpty =
          IsLineEmpty (hGlbTknHdr);

    // Save the token size
    uTknSize = (UINT) MyGlobalSize (hGlbTknHdr);

    // Lock the memory to get a ptr to it
    lpMemTknHdr = MyGlobalLock (hGlbTknHdr);

    // If we're not sizing, ...
    if (lpFcnLines)
    {
        // Copy the tokens to the end of the function header
        CopyMemory (ByteAddr (lpMemDfnHdr, *lpOffNextTknLine), lpMemTknHdr, uTknSize);

        // Save the offset
        lpFcnLines->offTknLine = *lpOffNextTknLine;
    } else
        // Free the storage in the tokens
        Untokenize (lpMemTknHdr);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbTknHdr); lpMemTknHdr = NULL;

    // We no longer need this storage
    MyGlobalFree (hGlbTknHdr); hGlbTknHdr = NULL;

    // Account for these tokens
    *lpOffNextTknLine += uTknSize;

    return uLineLen;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lpSF_Fcns->lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    return -1;
} // End SaveFunctionLine
#undef  APPEND_NAME


//***************************************************************************
//  $IsLineEmpty
//
//  Determine whether or not a line of tokens is empty
//***************************************************************************

UBOOL IsLineEmpty
    (HGLOBAL hGlbTknHdr)                // Token line header global memory handle

{
    LPTOKEN lptkLine;                   // Ptr to line of tokens
    UBOOL   bRet = FALSE;               // TRUE iff the line is empty

    // Handle tokenize error
    if (hGlbTknHdr EQ NULL)
        return TRUE;

    // Lock the memory to get a ptr to it
    lptkLine = MyGlobalLock (hGlbTknHdr);

    // Skip over the TOKEN_HEADER
    lptkLine = TokenBaseToStart (lptkLine);

    // Handle labeled lines

    // If a single stmt line is labeled, ...
    if (lptkLine[0].tkFlags.TknType EQ TKT_EOL
     && lptkLine[1].tkFlags.TknType EQ TKT_VARNAMED
     && lptkLine[2].tkFlags.TknType EQ TKT_LABELSEP
     && lptkLine[3].tkFlags.TknType EQ TKT_SOS)
        // Mark as an empty line
        bRet = TRUE;

    // If a multiple stmt line is labeled, ...
    if (lptkLine[0].tkFlags.TknType EQ TKT_EOS
     && lptkLine[1].tkFlags.TknType EQ TKT_VARNAMED
     && lptkLine[2].tkFlags.TknType EQ TKT_LABELSEP
     && lptkLine[3].tkFlags.TknType EQ TKT_SOS)
        // Skip over the labeled stmt quadruple
        lptkLine += 4;

    // Handle unlabeled lines

    // Loop through the tokens looking for anything other than
    //   Zero or more pairs of (TKT_EOS, TKT_SOS)
    //   ending with the pair  (TKT_EOL, TKT_SOS)
    while (lptkLine[0].tkFlags.TknType EQ TKT_EOS
        && lptkLine[1].tkFlags.TknType EQ TKT_SOS)
        // Skip over the pair
        lptkLine += 2;

    if (lptkLine[0].tkFlags.TknType EQ TKT_EOL
     && lptkLine[1].tkFlags.TknType EQ TKT_SOS)
        // Mark as an empty line
        bRet = TRUE;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbTknHdr); lptkLine = NULL;

    return bRet;
} // End IsLineEmpty


//***************************************************************************
//  $GetLabelNums
//
//  Calculate the line #s of the line labels
//***************************************************************************

UBOOL GetLabelNums
    (LPDFN_HEADER  lpMemDfnHdr,         // Ptr to user-defined function/operator header
     HWND          hWndEC,              // Edit Ctrl window handle (FE only)
     UBOOL         bDispErrMsg,         // TRUE iff we may display error messages
     LPSF_FCNS     lpSF_Fcns)           // Ptr to common struc (may be NULL)

{
    UINT           numFcnLines,         // # lines in the function
                   uDupLineNum1,        // Line # of duplicate label (origin-1)
                   uCnt,                // Loop counter
                   uLineNum0;           // Line # (origin-0)
    LPLBLENTRY    *lplpLblEntry = NULL, // Ptr to array of LPLBLENTRYs of labeled lines
                   lpLblEntry;          // Ptr to an individual LBLENTRY
    LPFCNLINE      lpFcnLines,          // Ptr to array of function line structs (FCNLINE[numFcnLines])
                   lpLstLabel;          // Ptr to the last labeled line
    LPTOKEN_HEADER lptkHdr;             // Ptr to header of tokenized line
    LPTOKEN        lptkLine;            // Ptr to tokenized line
    UBOOL          bRet;                // TRUE iff the result is valid
    HGLOBAL        hGlbName;            // Name's global memory handle

    // Get # lines in the function
    numFcnLines = lpMemDfnHdr->numFcnLines;

    // Allocate room for <numFcnLines> of <LPSYMENTRY>s for the line labels
    //   so we can sort the labels and find duplicates
    lplpLblEntry = DbgGlobalAlloc (GPTR, numFcnLines * (sizeof (LBLENTRY) + sizeof (LPVOID)));
    if (lplpLblEntry EQ NULL)
        goto WSFULL_EXIT;

    // Save ptr to start of LBLENTRYs
    lpLblEntry = (LPLBLENTRY) &lplpLblEntry[numFcnLines];

    // Get ptr to array of function line structs (FCNLINE[numFcnLines])
    lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

    // Save as ptr to the last labeled line
    lpLstLabel = NULL;
    lpMemDfnHdr->numLblLines = 0;

    // Loop through the function lines
    for (uLineNum0 = 0; uLineNum0 < numFcnLines; uLineNum0++)
    if (lpFcnLines->offTknLine)
    {
        UINT numTokens;     // # tokens in the line

        // Get a ptr to the token header
        lptkHdr = (LPTOKEN_HEADER) ByteAddr (lpMemDfnHdr, lpFcnLines->offTknLine);

        // Get the # tokens in the line
        numTokens = lptkHdr->TokenCnt;

        // Get ptr to the tokens in the line
        lptkLine = TokenBaseToStart (lptkHdr);

        Assert (lptkLine[0].tkFlags.TknType EQ TKT_EOL
             || lptkLine[0].tkFlags.TknType EQ TKT_EOS);

        // If there are at least three tokens, ...
        //   (TKT_EOL, TKT_VARNAMED, TKT_LABELSEP)
        if (numTokens >= 3)
        {
            if (lptkLine[2].tkFlags.TknType EQ TKT_LABELSEP
             && lptkLine[1].tkFlags.TknType EQ TKT_VARNAMED)
            {
                LPAPLCHAR lpMemName;    // Ptr to the name

                // stData is an LPSYMENTRY
                Assert (GetPtrTypeDir (lptkLine[1].tkData.tkSym) EQ PTRTYPE_STCONST);

                // If the stHshEntry is valid, ...
                //   (might not be if the System Label is mispelled)
                if (lptkLine[1].tkData.tkSym->stHshEntry)
                {
                    // Get the Name's global memory handle
                    hGlbName = lptkLine[1].tkData.tkSym->stHshEntry->htGlbName;

                    // Lock the memory to get a ptr to it
                    lpMemName = MyGlobalLock (hGlbName);

                    if (lstrcmpiW (lpMemName, $QUAD_ID ) EQ 0)
                    {
                        if (uDupLineNum1 = lpMemDfnHdr->nSysLblId )
                            goto SYSDUP_EXIT;
                        // Save line # in origin-1
                        lpMemDfnHdr->nSysLblId  = uLineNum0 + 1;
                    } else
                    if (lstrcmpiW (lpMemName, $QUAD_INV) EQ 0)
                    {
                        if (uDupLineNum1 = lpMemDfnHdr->nSysLblInv)
                            goto SYSDUP_EXIT;
                        // Save line # in origin-1
                        lpMemDfnHdr->nSysLblInv = uLineNum0 + 1;
                    } else
                    if (lstrcmpiW (lpMemName, $QUAD_MS ) EQ 0)
                    {
                        if (uDupLineNum1 = lpMemDfnHdr->nSysLblMs )
                            goto SYSDUP_EXIT;
                        // Save line # in origin-1
                        lpMemDfnHdr->nSysLblMs  = uLineNum0 + 1;
                    } else
                    if (lstrcmpiW (lpMemName, $QUAD_PRO) EQ 0)
                    {
                        if (uDupLineNum1 = lpMemDfnHdr->nSysLblPro)
                            goto SYSDUP_EXIT;
                        // Save line # in origin-1
                        lpMemDfnHdr->nSysLblPro = uLineNum0 + 1;
                    } else
                    if (lstrcmpiW (lpMemName, $QUAD_SGL) EQ 0)
                    {
                        if (uDupLineNum1 = lpMemDfnHdr->nSysLblSgl)
                            goto SYSDUP_EXIT;
                        // Save line # in origin-1
                        lpMemDfnHdr->nSysLblSgl = uLineNum0 + 1;
                    } else
                    {
                        // If there's a previous label, ...
                        if (lpLstLabel)
                            // Save the line # of the next labeled line
                            lpLstLabel->numNxtLabel1 = uLineNum0 + 1;
                        else
                            // Save the line # of the first labeled line
                            lpMemDfnHdr->num1stLabel1 = uLineNum0 + 1;
                        // Save as the ptr to the last labeled line
                        lpLstLabel = lpFcnLines;

                        // Create a ptr to the LBLENTRY
                        lplpLblEntry[lpMemDfnHdr->numLblLines] = lpLblEntry++;

                        // Save the LBLENTRY of the label
                        lplpLblEntry[lpMemDfnHdr->numLblLines]->lpSymEntry = lptkLine[1].tkData.tkSym;
                        lplpLblEntry[lpMemDfnHdr->numLblLines]->uLineNum1  = uLineNum0 + 1;

                        // Mark as a labeled line
                        lpLstLabel->bLabel = TRUE;

                        // Count in another labeled line
                        lpMemDfnHdr->numLblLines++;
                    } // End IF/ELSE/...

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbName); lpMemName = NULL;
                } // End IF
            } // End IF
        } // End IF

        // Skip to the next struct
        lpFcnLines++;
    } // End FOR/IF

    // Check for duplicate labels

    // If there is more than one label, ...
    if (lpMemDfnHdr->numLblLines > 1)
    {
        // Sort the array of LPSYMENTRYs of labels
        ShellSort (lplpLblEntry, lpMemDfnHdr->numLblLines, CmpLPLBLENTRY);

        // Loop through the LPSYMENTRYs looking for duplicates
        for (uCnt = 0; uCnt < (lpMemDfnHdr->numLblLines - 1); uCnt++)
        if (CmpLPLBLENTRY (lplpLblEntry[uCnt], lplpLblEntry[uCnt + 1]) EQ 0)
            // Call common error function
            ErrLabelNums (lplpLblEntry[uCnt]->lpSymEntry->stHshEntry->htGlbName,
                          lplpLblEntry[uCnt]->uLineNum1,
                          lpMemDfnHdr,
                          bDispErrMsg);
    } // End IF

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    if (bDispErrMsg)
        // Display the error message
        MessageBoxW (hWndMF,
                     L"Unable to allocate space to test for duplicate labels -- function not saved",
                     lpwszAppName,
                     MB_OK | MB_ICONWARNING | MB_APPLMODAL);
    goto ERROR_EXIT;

SYSDUP_EXIT:
    // Call common error function
    ErrLabelNums (hGlbName,
                  uDupLineNum1,
                  lpMemDfnHdr,
                  bDispErrMsg);
    // If we can pass on the line # in error, ...
    if (lpSF_Fcns)
        // Save the line # (origin-0)
        lpSF_Fcns->uErrLine = uDupLineNum1 - 1;

    goto ERROR_EXIT;

ERROR_EXIT:
    if (bDispErrMsg && hWndEC NE NULL)
        SetFocus (GetParent (hWndEC));

    // Mark as in error
    bRet = FALSE;
NORMAL_EXIT:
    if (lplpLblEntry)
    {
        // We no longer need this storage
        MyGlobalFree (lplpLblEntry); lplpLblEntry = NULL;
    } // End IF

    return bRet;
} // End GetLabelNums


//***************************************************************************
//  $ErrLabelNums
//
//  Display common error message from <GetLabelNums>
//***************************************************************************

void ErrLabelNums
    (HGLOBAL      hGlbName,         // Line label name's global memory handle
     UINT         uLineNum1,        // Line # in error (origin-1)
     LPDFN_HEADER lpMemDfnHdr,      // Ptr to user-defined function/operator header
     UBOOL        bDispErrMsg)      // TRUE iff we may display error messages

{
    WCHAR     wszTemp[1024];        // Save area for error message text
    LPAPLCHAR lpMem,                // Ptr to LPSYMENTRY global memory name
              lpMemName;            // Ptr to function name

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbName);

    // Lock the memory to get a ptr to it
    lpMemName = MyGlobalLock (lpMemDfnHdr->steFcnName->stHshEntry->htGlbName);

    // Format the error message
    wsprintfW (wszTemp,
               L"Duplicate label <%s> in <%s> on line # %d",
               lpMem,
               lpMemName,
               uLineNum1);
    // We no longer need these ptrs
    MyGlobalUnlock (lpMemDfnHdr->steFcnName->stHshEntry->htGlbName); lpMemName = NULL;
    MyGlobalUnlock (hGlbName); lpMem = NULL;

    // If we may display error messages, ...
    if (bDispErrMsg)
        // Display the error message
        MessageBoxW (hWndMF,
                     wszTemp,
                     lpwszAppName,
                     MB_OK | MB_ICONWARNING | MB_APPLMODAL);
    else
        // Display the message in the session
        AppendLine (wszTemp, FALSE, TRUE);
} // ErrLabelNums


//***************************************************************************
//  End of File: savefcn.c
//***************************************************************************
