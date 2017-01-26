//***************************************************************************
//  NARS2000 -- Save Function Routines
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
#include "headers.h"
#include "debug.h"              // For xxx_TEMP_OPEN macros


#ifdef DEBUG
HGLOBAL hGlbRC1,
        hGlbRC2;
#endif

// The following <lpw...> vars must be in the same order as the <FCN_VALENCES> enum

//  Function headers of all valences
LPWCHAR lpwFcnValHdr[FCNVALENCE_LENGTH] =
{
    $AFORESULT $IS                              $DEL                       ,    // Function header, niladic
    $AFORESULT $IS                              $DEL                $OMEGA ,    // ...              monadic
    $AFORESULT $IS      $ALPHA                  $DEL                $OMEGA ,    // ...              dyadic
    $AFORESULT $IS L"{" $ALPHA L"}"             $DEL                $OMEGA ,    // ...              ambivalent
};

//  Monadic operator headers of all valences
LPWCHAR lpwOp1ValHdr[FCNVALENCE_LENGTH] =
{
    $AFORESULT $IS                  L"(" $LOPER $DELDEL        L")"        ,    // Monadic operator header, niladic derived function
    $AFORESULT $IS                  L"(" $LOPER $DELDEL        L")" $OMEGA ,    // ...                      monadic ...
    $AFORESULT $IS      $ALPHA      L"(" $LOPER $DELDEL        L")" $OMEGA ,    // ...                      dyadic ...
    $AFORESULT $IS L"{" $ALPHA L"}" L"(" $LOPER $DELDEL        L")" $OMEGA ,    // ...                      ambivalent
};

//  Dyadic operator headers of all valences
LPWCHAR lpwOp2ValHdr[FCNVALENCE_LENGTH] =
{
    $AFORESULT $IS                  L"(" $LOPER $DELDEL $ROPER L")"        ,    // Dyadic operator header, niladic derived function
    $AFORESULT $IS                  L"(" $LOPER $DELDEL $ROPER L")" $OMEGA ,    // ...                     monadic ...
    $AFORESULT $IS      $ALPHA      L"(" $LOPER $DELDEL $ROPER L")" $OMEGA ,    // ...                     dyadic ...
    $AFORESULT $IS L"{" $ALPHA L"}" L"(" $LOPER $DELDEL $ROPER L")" $OMEGA ,    // ...                     ambivalent ...
};




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
    SF_FCNS      SF_Fcns = {0};     // SaveFunction local vars
    HGLOBAL      hGlbDfnHdr;        // User-defined function/operator header global memory handle
    LPDFN_HEADER lpMemDfnHdr;       // Ptr to user-defined function/operator header global memory
    LW_PARAMS    LW_Params = {0};   // Local  ...

    // Get the previous function global memory handle (if any)
    hGlbDfnHdr = (HGLOBAL) GetWindowLongPtrW (hWndFE, GWLSF_HGLBDFNHDR);

    // If the handle is valid, ...
    if (hGlbDfnHdr NE NULL)
    {
        LPPERTABDATA lpMemPTD;                      // Ptr to PerTabData global memory
        LPWCHAR      lpwszTemp;                     // Ptr to temporary storage
        VARS_TEMP_OPEN

        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLockDfn (hGlbDfnHdr);

        // Save the AFO flag
        SF_Fcns.bAFO     = lpMemDfnHdr->bAFO;       // Parsing an AFO
        SF_Fcns.bMakeAFE = TRUE;                    // ...            from the FE

        // We no longer need this ptr
        MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;

        // Get ptr to PerTabData global memory
        lpMemPTD = GetMemPTD ();

        // Get ptr to temporary storage
        lpwszTemp = lpMemPTD->lpwszTemp;
        CHECK_TEMP_OPEN

        // Get this window's function name (if any)
        (void) GetFunctionName (hWndFE, lpwszTemp, &SF_Fcns.lpSymName);

        EXIT_TEMP_OPEN
    } // End IF

    // Fill in common values
    SF_Fcns.bDisplayErr     = TRUE;                 // Display Errors
////SF_Fcns.bAFO            = FALSE;                // Parsing an AFO (already zero from = {0})
    SF_Fcns.SF_LineLen      = SF_LineLenFE;         // Ptr to line length function
    SF_Fcns.SF_ReadLine     = SF_ReadLineFE;        // Ptr to read line function
    SF_Fcns.SF_IsLineCont   = SF_IsLineContFE;      // Ptr to Is Line Continued function
    SF_Fcns.SF_NumPhyLines  = SF_NumPhyLinesFE;     // Ptr to get # physical lines function
    SF_Fcns.SF_NumLogLines  = SF_NumLogLinesFE;     // Ptr to get # logical  ...
    SF_Fcns.SF_CreationTime = SF_CreationTimeCom;   // Ptr to get function creation time
    SF_Fcns.SF_LastModTime  = SF_LastModTimeCom;    // Ptr to get function creation time
    SF_Fcns.SF_UndoBuffer   = SF_UndoBufferFE;      // Ptr to get function Undo Buffer global memory handle
    SF_Fcns.LclParams       = &LW_Params;           // Ptr to local parameters in case it's an AFO
    SF_Fcns.sfTypes         = SFTYPES_FE;           // Caller type

    // Call common routine
    return SaveFunctionCom (hWndFE,                 // Function Edit window handle (not-[]FX only)
                           &SF_Fcns);               // Ptr to common values
} // End SaveFunction
#undef  APPEND_NAME


//***************************************************************************
//  $SF_IsLineContCom
//
//  Return TRUE iff the line is continued to the next line
//***************************************************************************

UINT SF_IsLineContCom
    (HWND        hWndEC,            // Edit Ctrl window handle (FE only)
     LPVOID      lpVoid,            // Ptr to common struc
     UINT        uLineNum)          // Function line # (0 = header)

{
    return FALSE;
} // End SF_IsLineContCom


//***************************************************************************
//  $SF_IsLineContFE
//
//  Return TRUE iff the line is continued to the next line
//    in a function editing window
//***************************************************************************

UINT SF_IsLineContFE
    (HWND        hWndEC,            // Edit Ctrl window handle (FE only)
     LPVOID      lpVoid,            // Ptr to common struc
     UINT        uLineNum)          // Function line # (0 = header)

{
    return SendMessageW (hWndEC, MYEM_ISLINECONT, uLineNum, 0) EQ TRUE;
} // End SF_IsLineContFE


//***************************************************************************
//  $SF_IsLineContSV
//
//  Return TRUE iff the line is continued to the next line
//    when called from []FX with a simple char scalar or vector arg
//***************************************************************************

UINT SF_IsLineContSV
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns,           // Ptr to common struc
     UINT      uLineNum)            // Function line # (0 = header)

{
    // If it's an AFO, ...
    if (lpSF_Fcns->bAFO)
    {
        LPFX_PARAMS     lpFX_Params;    // Ptr to common struc
        LPAFODETECT_STR lpafoDetectStr; // Ptr to AFODETECT_STR

        // Save local params
        lpFX_Params    = lpSF_Fcns->LclParams;
        lpafoDetectStr = lpFX_Params->lpafoDetectStr;

        return lpafoDetectStr->lpafoLineStr[uLineNum].bLineCont;
    } else
        return FALSE;
} // End SF_IsLineContSV


//***************************************************************************
//  $SF_LineLenAN
//
//  Return the length of a function text header/line
//    when saving an AFO
//***************************************************************************

UINT SF_LineLenAN
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns,           // Ptr to common struc
     UINT      uLineNum)            // Function line # (0 = header)

{
    LPLW_PARAMS lpLW_Params;        // Ptr to common struc

    // Save local params
    lpLW_Params = lpSF_Fcns->LclParams;

    // If it's the header, ...
    if (uLineNum EQ 0)
    {
        // Split cases based upon the UDFO type
        switch (lpLW_Params->lpYYRht->lptkRhtBrace->tkData.tkDfnType)
        {
            case DFNTYPE_FCN:
                return lstrlenW (lpwFcnValHdr[FCNVALENCE_AMB]);

            case DFNTYPE_OP1:
                return lstrlenW (lpwOp1ValHdr[FCNVALENCE_AMB]);

            case DFNTYPE_OP2:
                return lstrlenW (lpwOp2ValHdr[FCNVALENCE_AMB]);

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
     LPSF_FCNS lpSF_Fcns,           // Ptr to common struc
     UINT      uLineNum)            // Function line # (0 = header)

{
    // If the preceding physical line does not continue to the current line, ...
    if (uLineNum EQ 0
     || SendMessageW (hWndEC, MYEM_ISLINECONT, uLineNum - 1, 0) EQ FALSE)
        // Get the overall block length
        //   not including a terminating zero
        return GetBlockLength (hWndEC, uLineNum);
    else
        return 0;
} // End SF_LineLenFE


//***************************************************************************
//  $SF_LineLenLW
//
//  Return the length of a function text header/line
//    when called from LoadWorkspace
//***************************************************************************

UINT SF_LineLenLW
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns,           // Ptr to common struc
     UINT      uLineNum)            // Function line # (0 = header)

{
    LPLW_PARAMS lpLW_Params;        // Ptr to common struc

    // Save local params
    lpLW_Params = lpSF_Fcns->LclParams;

    // The caller requests the line length before it reads
    //   the line so it can allocate memory.

    // This means we must read the line first so we can get its length
    SF_ReadLineLW (hWndEC,                  // Edit Ctrl window handle (FE only)
                   lpSF_Fcns,               // Ptr to common struc
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
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns,           // Ptr to common struc
     UINT      uLineNum)            // Function line # (0 = header)

{
    LPVARARRAY_HEADER lpMemHdrRht = NULL;   // Ptr to right arg header
    LPAPLCHAR         lpMemRht;             // Ptr to right arg global memory
    UINT              uRowOff,              // Offset to start of row
                      uLineLen;             // Line length
    LPFX_PARAMS       lpFX_Params;          // Ptr to common struc

    // Save local params
    lpFX_Params = lpSF_Fcns->LclParams;

    // Lock the memory to get a ptr to it
    lpMemHdrRht = MyGlobalLockVar (lpFX_Params->hGlbRht);

    // Skip over the header to the data
    lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

    // Get the # cols in the matrix
    uLineLen = (UINT) lpFX_Params->aplColsRht;

    // Calc the offset to the appropriate row
    uRowOff = uLineNum * uLineLen;

    // Skip over the header to the data
    lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

    // As this is a matrix and the header/function line might have
    //   been padded out beyond its normal length, delete trailing blanks
    for (; uLineLen; uLineLen--)
    if (lpMemRht[uRowOff + uLineLen - 1] NE L' ')
        break;

    // We no longer need this ptr
    MyGlobalUnlock (lpFX_Params->hGlbRht); lpMemHdrRht = NULL;

    return uLineLen;
} // End SF_LineLenM


//***************************************************************************
//  $SF_LineLenN
//
//  Return the length of a function text header/line
//    when called from []FX with a nested arg
//***************************************************************************

UINT SF_LineLenN
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns,           // Ptr to common struc
     UINT      uLineNum)            // Function line # (0 = header)

{
    LPVARARRAY_HEADER lpMemHdrRht = NULL;   // Ptr to right arg header
    LPAPLNESTED       lpMemRht;             // Ptr to right arg global memory
    HGLOBAL           hGlbItmRht;           // Right arg item global memory handle
    APLNELM           aplNELMItmRht;        // Right arg item NELM
    LPFX_PARAMS       lpFX_Params;          // Ptr to common struc

    // Save local params
    lpFX_Params = lpSF_Fcns->LclParams;

    // Lock the memory to get a ptr to it
    lpMemHdrRht = MyGlobalLockVar (lpFX_Params->hGlbRht);

    // Skip over the header to the data
    lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

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
    MyGlobalUnlock (lpFX_Params->hGlbRht); lpMemHdrRht = NULL;

    return (UINT) aplNELMItmRht;
} // End SF_LineLenN


//***************************************************************************
//  $SF_LineLenAFO
//
//  Return the length of a function text header/line
//    as well as its offset in the line
//    when called from []FX with a simple char scalar or vector arg
//***************************************************************************

UINT SF_LineLenAFO
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns,           // Ptr to common struc
     UINT      uLineNum,            // Function line # (0 = header)
     LPUINT    lpuLineOff)          // Ptr to line offset (may be NULL)

{
    LPFX_PARAMS       lpFX_Params;          // Ptr to common struc
    LPVARARRAY_HEADER lpMemHdrRht = NULL;   // Ptr to right arg header
    LPAPLCHAR         lpMemRht;             // Ptr to right arg global memory
    UINT              uLineLen,             // Length of the object
                      uLineOff = 0;         // Offset in the line of the object

    // Save local params
    lpFX_Params = lpSF_Fcns->LclParams;

    // Lock the memory to get a ptr to it
    lpMemHdrRht = MyGlobalLockVar (lpFX_Params->hGlbRht);

    // Skip over the header to the data
    lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

    // Split cases based upon the arg rank
    switch (lpFX_Params->aplRankRht)
    {
        case 0:
            Assert (!lpSF_Fcns->bAFO);

            // The function line is a single char
            uLineLen = 1;

            break;

        case 1:
        case 2:
            // If it's an AFO, ...
            if (lpSF_Fcns->bAFO)
            {
                // Split cases based upon the line #
                switch (uLineNum)
                {
                    case 0:
                        // Find the left arrow or space
                        // Return the index as the length of the function name
                        uLineLen = (UINT) strcspnW (lpMemRht, WS_UTF16_LEFTARROW L" ");

                        break;

                    case 1:
                        // Find the left brace
                        uLineOff = (UINT) strcspnW (lpMemRht, WS_UTF16_LEFTBRACE);

                        // If it's valid, ...
                        if (uLineOff NE 0)
                        {
                            Assert (lpMemRht[uLineOff] EQ UTF16_LEFTBRACE);

                            // Skip over the left brace
                            uLineOff++;

                            // Copy the NELM
                            uLineLen = (UINT) lpMemHdrRht->NELM;

                            // Back off trailing blanks
                            while (uLineLen NE 0 && isspaceW (lpMemRht[uLineLen - 1]))
                                uLineLen--;

                            Assert (lpMemRht[uLineLen - 1] EQ UTF16_RIGHTBRACE);

                            // Back up over the trailing right brace
                            uLineLen--;

                            // Subtract from the overall length and return it as the length of
                            //   the function line without the function name/left arrow
                            //   and without the surrounding braces
                            uLineLen = (UINT) (uLineLen - uLineOff);
                        } // End IF

                        break;

                    defstop
                        break;
                } // End SWITCH
            } else
                uLineLen = (UINT) lpFX_Params->aplColsRht;
            break;

        defstop
            uLineLen = -1;

            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (lpFX_Params->hGlbRht); lpMemHdrRht = NULL;

    // If the caller wants the offset, ...
    if (lpuLineOff NE NULL)
        *lpuLineOff = uLineOff;

    return uLineLen;
} // End SF_LineLenAFO


//***************************************************************************
//  $SF_LineLenSV
//
//  Return the length of a function text header/line
//    when called from []FX with a simple char scalar or vector arg
//***************************************************************************

UINT SF_LineLenSV
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns,           // Ptr to common struc
     UINT      uLineNum)            // Function line # (0 = header)

{
    // If it's an AFO, ...
    if (lpSF_Fcns->bAFO)
    {
        LPFX_PARAMS     lpFX_Params;    // Ptr to common struc
        LPAFODETECT_STR lpafoDetectStr; // Ptr to AFODETECT_STR
        UINT            uLineLen;       // The line length

        // Save local params
        lpFX_Params    = lpSF_Fcns->LclParams;
        lpafoDetectStr = lpFX_Params->lpafoDetectStr;

        // Get the precomputed line length
        uLineLen = lpFX_Params->lpafoDetectStr->lpafoLineStr[uLineNum].uLineLen;

        // Loop through contiguous continued physical lines
        while (lpafoDetectStr->lpafoLineStr[uLineNum++].bLineCont)
            // Add in the line length plus the count of WS_CRCRLF
            uLineLen += lpafoDetectStr->lpafoLineStr[uLineNum].uLineLen + strcountof (WS_CRCRLF);
        return uLineLen;
    } else
        return SF_LineLenAFO (hWndEC, lpSF_Fcns, uLineNum, NULL);
} // End SF_LineLenSV


//***************************************************************************
//  $SF_LineLenTF1
//
//  Return the length of a function text header/line
//    when called from []TF with a simple char matrix arg
//***************************************************************************

UINT SF_LineLenTF1
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns,           // Ptr to common struc
     UINT      uLineNum)            // Function line # (0 = header)

{
    LPTF1_PARAMS lpTF1_Params;      // Ptr to common struc

    // Save local params
    lpTF1_Params = lpSF_Fcns->LclParams;

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
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns,           // Ptr to common struc
     UINT      uLineNum)            // Function line # (0 = header)

{
    LPAA_PARAMS lpAA_Params;        // Ptr to common struc
    LPWCHAR     lpw;

    // Save local params
    lpAA_Params = lpSF_Fcns->LclParams;

    // Get a ptr to the start of the function lines
    lpw = lpAA_Params->lpwStart;

    // Skip to the designated line #
    while (uLineNum NE 0)
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
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns,           // Ptr to common struc
     UINT      uLineNum,            // Function line # (0 = header)
     LPAPLCHAR lpMemLine)           // Ptr to header/line global memory

{
    LPLW_PARAMS lpLW_Params;        // Ptr to common struc

    // Save local params
    lpLW_Params = lpSF_Fcns->LclParams;

    // If it's the header, ...
    if (uLineNum EQ 0)
    {
        // Split cases based upon the UDFO type
        switch (lpLW_Params->lpYYRht->lptkRhtBrace->tkData.tkDfnType)
        {
            case DFNTYPE_FCN:
                strcpyW (lpMemLine, lpwFcnValHdr[FCNVALENCE_AMB]);

                break;

            case DFNTYPE_OP1:
                strcpyW (lpMemLine, lpwOp1ValHdr[FCNVALENCE_AMB]);

                break;

            case DFNTYPE_OP2:
                strcpyW (lpMemLine, lpwOp2ValHdr[FCNVALENCE_AMB]);

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
            strcpynW (lpMemLine,
                     &lpLW_Params->lpplLocalVars->lpwszLine[lpLW_Params->lpYYRht->lptkLftBrace->tkCharIndex + 1],
                      lpLW_Params->lpYYRht->lptkRhtBrace->tkCharIndex
                    - lpLW_Params->lpYYRht->lptkLftBrace->tkCharIndex);
        else
        {
            LPMEMTXT_UNION lpMemTxtLine;

            // Lock the memory to get a ptr to it
            lpMemTxtLine = MyGlobalLockPad (lpLW_Params->lpplLocalVars->hGlbTxtLine);

            // Copy the line to the caller's memory
            //   with room for the trailing zero
            strcpynW (lpMemLine,
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
     LPSF_FCNS lpSF_Fcns,           // Ptr to common struc
     UINT      uLineNum,            // Function line # (0 = header)
     LPAPLCHAR lpMemLine)           // Ptr to header/line global memory

{
    // If the preceding physical line does not continue to the current line, ...
    if (uLineNum EQ 0
     || SendMessageW (hWndEC, MYEM_ISLINECONT, uLineNum - 1, 0) EQ FALSE)
        // Copy a block of lines
        //   including a terminating zero if there's enough room
        CopyBlockLines (hWndEC, uLineNum, lpMemLine);
    else
        lpMemLine[0] = WC_EOS;
} // End SF_ReadLineFE


//***************************************************************************
//  $SF_ReadLineLW
//
//  Read in a header/function line
//    when called from LoadWorkspace
//***************************************************************************

void SF_ReadLineLW
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns,           // Ptr to common struc
     UINT      uLineNum,            // Function line # (0 = header)
     LPAPLCHAR lpMemLine)           // Ptr to header/line global memory

{
    WCHAR       wszLineNum[8];      // Save area for formatted line #
    LPWCHAR     lpwszProf;          // Ptr to profile string
    LPLW_PARAMS lpLW_Params;        // Ptr to common struc

    // Save local params
    lpLW_Params = lpSF_Fcns->LclParams;

    // Format the line #
    MySprintfW (wszLineNum,
                sizeof (wszLineNum),
               L"%d",
                uLineNum);
    // Get the one (and only) line
    lpwszProf =
      ProfileGetString (lpLW_Params->lpwSectName,   // Ptr to the section name
                        wszLineNum,                 // Ptr to the key name
                        L"",                        // Ptr to the default value
                        lpLW_Params->lpDict);       // Ptr to workspace dictionary
    // Copy the line to a local buffer
    // DO NOT USE lstrcpyW as it doesn't trigger a visible Page Fault
    strcpyW (lpLW_Params->lpwBuffer, lpwszProf);

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
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns,           // Ptr to common struc
     UINT      uLineNum,            // Function line # (0 = header)
     LPAPLCHAR lpMemLine)           // Ptr to header/line global memory

{
    LPVARARRAY_HEADER lpMemHdrRht = NULL;   // Ptr to right arg header
    LPAPLCHAR         lpMemRht;             // Ptr to right arg global memory
    UINT              uRowOff,              // Offset to start of row
                      uLineLen;             // Line length
    LPFX_PARAMS       lpFX_Params;          // Ptr to common struc

    // Save local params
    lpFX_Params = lpSF_Fcns->LclParams;

    // Lock the memory to get a ptr to it
    lpMemHdrRht = MyGlobalLockVar (lpFX_Params->hGlbRht);

    // Skip over the header to the data
    lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

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
    lpMemLine[0] = WC_EOS;

    // We no longer need this ptr
    MyGlobalUnlock (lpFX_Params->hGlbRht); lpMemHdrRht = NULL;
} // End SF_ReadLineM


//***************************************************************************
//  $SF_ReadLineN
//
//  Read in a header/function line
//    when called from []FX with a nested arg
//***************************************************************************

void SF_ReadLineN
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns,           // Ptr to common struc
     UINT      uLineNum,            // Function line # (0 = header)
     LPAPLCHAR lpMemLine)           // Ptr to header/line global memory

{
    LPVARARRAY_HEADER lpMemHdrRht = NULL,       // Ptr to right arg header
                      lpMemHdrItmRht = NULL;    // Ptr to right arg header
    LPAPLNESTED       lpMemRht;                 // Ptr to right arg global memory
    LPAPLCHAR         lpMemItmRht;              // Ptr to right arg item global memory
    HGLOBAL           hGlbItmRht;               // Right arg item global memory handle
    UINT              uLineLen;                 // Line length
    LPFX_PARAMS       lpFX_Params;              // Ptr to common struc

    // Save local params
    lpFX_Params = lpSF_Fcns->LclParams;

    // Lock the memory to get a ptr to it
    lpMemHdrRht = MyGlobalLockVar (lpFX_Params->hGlbRht);

    // Skip over the header to the data
    lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

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
            lpMemHdrItmRht = MyGlobalLockVar (hGlbItmRht);

            // Get the NELM (line length)
#define lpHeader        lpMemHdrItmRht
            uLineLen = (UINT) lpHeader->NELM;

            // Skip over the header to the data
            lpMemItmRht = VarArrayDataFmBase (lpMemHdrItmRht);
#undef  lpHeader

            // Copy the data to the result
            CopyMemoryW (lpMemLine, lpMemItmRht, uLineLen);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbItmRht); lpMemHdrItmRht = NULL;

            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (lpFX_Params->hGlbRht); lpMemHdrRht = NULL;
} // End SF_ReadLineN


//***************************************************************************
//  $SF_ReadLineSV
//
//  Read in a header/function line
//    when called from []FX with a simple char scalar or vector arg
//***************************************************************************

void SF_ReadLineSV
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns,           // Ptr to common struc
     UINT      uLineNum,            // Function line # (0 = header)
     LPAPLCHAR lpMemLine)           // Ptr to header/line global memory

{
    LPVARARRAY_HEADER lpMemHdrRht = NULL;   // Ptr to right arg header
    LPAPLCHAR         lpMemRht;             // Ptr to right arg global memory
    UINT              uLineLen,             // The line length
                      uLineOff;             // The line offset
    LPFX_PARAMS       lpFX_Params;          // Ptr to common struc
    LPAFODETECT_STR   lpafoDetectStr;       // Ptr to AFODETECT_STR

    // Save local params
    lpFX_Params    = lpSF_Fcns->LclParams;
    lpafoDetectStr = lpFX_Params->lpafoDetectStr;

    // If it's an AFO, ...
    if (lpSF_Fcns->bAFO)
    {
        // Get the precomputed line length & offset
        uLineLen = SF_LineLenSV (hWndEC, lpSF_Fcns, uLineNum);
        uLineOff = lpafoDetectStr->lpafoLineStr[uLineNum].uLineOff;
    } else
        // Get the line length and offset
        uLineLen = SF_LineLenAFO (hWndEC, lpSF_Fcns, uLineNum, &uLineOff);

    // Check for immediate right arg
    if (uLineNum EQ 0 && lpFX_Params->hGlbRht EQ NULL)
    {
        // Copy the simple char to the result
        *lpMemLine++ = (APLCHAR) lpFX_Params->aplLongestRht;
        *lpMemLine   = WC_EOS;
    } else
    {
        // Lock the memory to get a ptr to it
        lpMemHdrRht = MyGlobalLockVar (lpFX_Params->hGlbRht);

        // Skip over the header to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

        // Copy the char vector to the result
        CopyMemoryW (lpMemLine, &lpMemRht[uLineOff], uLineLen);

        // Ensure properly terminated
        lpMemLine[uLineLen] = WC_EOS;

        // We no longer need this ptr
        MyGlobalUnlock (lpFX_Params->hGlbRht); lpMemHdrRht = NULL;
    } // End IF/ELSE
} // End SF_ReadLineSV


//***************************************************************************
//  $SF_ReadLineTF1
//
//  Read in a header/function line
//    when called from []TF with a simple char matrix arg
//***************************************************************************

void SF_ReadLineTF1
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns,           // Ptr to common struc
     UINT      uLineNum,            // Function line # (0 = header)
     LPAPLCHAR lpMemLine)           // Ptr to header/line global memory

{
    LPTF1_PARAMS lpTF1_Params;      // Ptr to common struc

    // Save local params
    lpTF1_Params = lpSF_Fcns->LclParams;

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
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns,           // Ptr to common struc
     UINT      uLineNum,            // Function line # (0 = header)
     LPAPLCHAR lpMemLine)           // Ptr to header/line global memory

{
    LPWCHAR     lpw;
    LPAA_PARAMS lpAA_Params;        // Ptr to common struc

    // Save local params
    lpAA_Params = lpSF_Fcns->LclParams;

    // Get a ptr to the start of the function lines
    lpw = lpAA_Params->lpwStart;

    // Skip to the designated line #
    while (uLineNum NE 0)
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
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns)           // Ptr to common struc

{
    LPFX_PARAMS lpFX_Params;        // Ptr to common struc

    // Save local params
    lpFX_Params = lpSF_Fcns->LclParams;

    // The # function lines
    return -1 +
      (UINT) lpFX_Params->aplRowsRht;
} // End SF_NumLinesCom


//***************************************************************************
//  $SF_NumLinesAN
//
//  Return the # lines in the function (excluding the header line)
//    when saving an AFO
//***************************************************************************

UINT SF_NumLinesAN
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns)           // Ptr to common struc

{
    return 1;
} // End SF_NumLinesAN


//***************************************************************************
//  $SF_NumPhyLinesFE
//
//  Return the # physical lines in the function (excluding the header line)
//    when called from function editor
//***************************************************************************

UINT SF_NumPhyLinesFE
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns)           // Ptr to common struc

{
    return -1 +
      (UINT) SendMessageW (hWndEC, EM_GETLINECOUNT, 0, 0);
} // End SF_NumPhyLinesFE


//***************************************************************************
//  $SF_NumLogLinesFE
//
//  Return the # logical lines in the function (excluding the header line)
//    when called from function editor
//***************************************************************************

UINT SF_NumLogLinesFE
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns)           // Ptr to common struc

{
    return GetLogicalLineCountFE (hWndEC);
} // End SF_NumLogLinesFE


//***************************************************************************
//  $SF_NumLinesLW
//
//  Return the # lines in the function (excluding the header line)
//    when called from LoadWorkspace
//***************************************************************************

UINT SF_NumLinesLW
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns)           // Ptr to common struc

{
    LPLW_PARAMS lpLW_Params;       // Ptr to common struc

    // Save local params
    lpLW_Params = lpSF_Fcns->LclParams;

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
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns)           // Ptr to common struc

{
    LPFX_PARAMS lpFX_Params;       // Ptr to common struc

    // Save local params
    lpFX_Params = lpSF_Fcns->LclParams;

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
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns)           // Ptr to common struc

{
    LPTF1_PARAMS lpTF1_Params;     // Ptr to common struc

    // Save local params
    lpTF1_Params = lpSF_Fcns->LclParams;

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
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns)           // Ptr to common struc

{
    LPAA_PARAMS lpAA_Params;        // Ptr to common struc

    // Save local params
    lpAA_Params = lpSF_Fcns->LclParams;

    // The # function lines
    return -1 +
      (UINT) lpAA_Params->NumLines;
} // End SF_NumLinesAA


//***************************************************************************
//  $SF_NumPhyLinesSV
//
//  Return the # physical lines in the function (excluding the header line)
//    when called from []FX with a simple char scalar or vector arg, or
//***************************************************************************

UINT SF_NumPhyLinesSV
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns)           // Ptr to common struc

{
    LPFX_PARAMS lpFX_Params;        // Ptr to common struc

    // Save local params
    lpFX_Params = lpSF_Fcns->LclParams;

    // If it's an AFO, ...
    if (lpSF_Fcns->bAFO)
        // The # physical function lines
        return lpFX_Params->lpafoDetectStr->numPhyLines;
    else
        // The # physical function lines
        return (UINT) lpFX_Params->aplRowsRht - 1;
} // End SF_NumPhyLinesSV


//***************************************************************************
//  $SF_NumLogLinesSV
//
//  Return the # logical lines in the function (excluding the header line)
//    when called from []FX with a simple char scalar or vector arg, or
//***************************************************************************

UINT SF_NumLogLinesSV
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns)           // Ptr to common struc

{
    LPFX_PARAMS lpFX_Params;        // Ptr to common struc

    // Save local params
    lpFX_Params = lpSF_Fcns->LclParams;

    // If it's an AFO, ...
    if (lpSF_Fcns->bAFO)
        // The # logical function lines
        return lpFX_Params->lpafoDetectStr->numLogLines;
    else
        // The # logical function lines
        return (UINT) lpFX_Params->aplRowsRht - 1;
} // End SF_NumLogLinesSV


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
    (LPSF_FCNS   lpSF_Fcns,         // Ptr to common struc
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
    (LPSF_FCNS   lpSF_Fcns,         // Ptr to common struc
     SYSTEMTIME *lpSystemTime,      // Ptr to current system (UTC) time
     FILETIME   *lpftCreation)      // Ptr to output save area

{
    LPLW_PARAMS lpLW_Params;        // Ptr to common struc

    // Save local params
    lpLW_Params = lpSF_Fcns->LclParams;

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
    (LPSF_FCNS   lpSF_Fcns,         // Ptr to common struc
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
    (LPSF_FCNS   lpSF_Fcns,         // Ptr to common struc
     SYSTEMTIME *lpSystemTime,      // Ptr to current system (UTC) time
     FILETIME   *lpftLastMod)       // Ptr to output save area

{
    LPLW_PARAMS lpLW_Params;        // Ptr to common struc

    // Save local params
    lpLW_Params = lpSF_Fcns->LclParams;

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
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns)           // Ptr to common struc

{
    // If there's a UDFO global handle, ...
    if (lpSF_Fcns->hGlbDfnHdr NE NULL)
    {
        LPDFN_HEADER lpMemDfnHdr;

        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLockDfn (lpSF_Fcns->hGlbDfnHdr);

        Assert (lpMemDfnHdr->Sig.nature EQ DFN_HEADER_SIGNATURE);


        // We no longer need this ptr
        MyGlobalUnlock (lpSF_Fcns->hGlbDfnHdr); lpMemDfnHdr = NULL;

        return NULL;
    } else
        // No Undo Buffer
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
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns)           // Ptr to common struc

{
    HWND         hWndFE;            // Function Editor window handle
    LPUNDO_BUF   lpUndoBeg,         // Ptr to start of Undo buffer
                 lpUndoLst;         // Ptr to end ...
    HGLOBAL      hGlbUndoBuff;      // Undo Buffer global memory handle

    // Get the Function Editor window handle
    hWndFE = GetParent (hWndEC);

    (HANDLE_PTR) lpUndoBeg = GetWindowLongPtrW (hWndFE, GWLSF_UNDO_BEG);
    if (lpUndoBeg NE NULL)
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
            if (hGlbUndoBuff EQ NULL)
            {
                // Display the error message
                MessageBoxW (hWndMF,
                             L"Insufficient memory to save Undo buffer!!",
                             lpwszAppName,
                             MB_OK | MB_ICONWARNING | MB_APPLMODAL);
                SetFocus (GetParent (hWndEC));

                return NULL;
            } // End IF

            // Lock the memory to get a ptr to it
            lpMemUndo = MyGlobalLock000 (hGlbUndoBuff);

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
    (HWND      hWndEC,              // Edit Ctrl window handle (FE only)
     LPSF_FCNS lpSF_Fcns)           // Ptr to common struc

{
    LPWCHAR     lpMemUndoTxt;       // Ptr to Undo Buffer in text format
    WCHAR       wcAction;           // Temporary character
    LPUNDO_BUF  lpMemUndoBin;       // Ptr to Undo Buffer in binary format
    UINT        uUndoCount,         // # entries in the Undo Buffer
                uFields;            // # fields parsed by sscanfW
    HGLOBAL     hGlbUndoBuff;       // Undo Buffer global memory handle
    LPLW_PARAMS lpLW_Params;        // Ptr to common struc

    // Save local params
    lpLW_Params = lpSF_Fcns->LclParams;

    // Get parameters
    lpMemUndoTxt = lpLW_Params->lpMemUndoTxt;

    // Check for not present
    if (lpMemUndoTxt EQ NULL)
        return NULL;

    // Parse the # entries, and skip over it
    sscanfW (lpMemUndoTxt, L"%d", &uUndoCount);
    lpMemUndoTxt = SkipBlackW (lpMemUndoTxt);

    // Check for empty buffer
    if (*lpMemUndoTxt EQ '\0')
        return NULL;

    // Skip over the separating blank
    Assert (*lpMemUndoTxt EQ L' '); lpMemUndoTxt++;

    // Allocate storage for the Undo buffer
    hGlbUndoBuff = DbgGlobalAlloc (GHND, uUndoCount * sizeof (UNDO_BUF));
    if (hGlbUndoBuff EQ NULL)
    {
        // Display the error message
        MessageBoxW (hWndMF,
                     L"Insufficient memory to save Undo buffer!!",
                     lpwszAppName,
                     MB_OK | MB_ICONWARNING | MB_APPLMODAL);
        SetFocus (GetParent (hWndEC));

        return NULL;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemUndoBin = MyGlobalLock000 (hGlbUndoBuff);

    // Parse the Undo Buffer entries
    while (*lpMemUndoTxt NE '\0')
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

        if (*lpMemUndoTxt NE '\0')
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
//  $ConstructFcnHdr
//
//  Construct the function header choosing from the above lpLW_xxx lines
//***************************************************************************

LPWCHAR ConstructFcnHdr
    (LPSF_FCNS lpSF_Fcns)           // Ptr to common struc

{
    LPWCHAR *p,                     // Ptr to ptr to LPWCHAR
             q;                     // The result

    // The choices for a function header split into two independent groups:
    //   Fcn vs. Op1 vs. Op2 vs. Op3  and
    //   Nil vs. Mon vs. Dyd vs. Amb

    // If it's a dyadic operator, ...
    if (lpSF_Fcns->bRefRhtOper)
        p = lpwOp2ValHdr;
    else
    if (lpSF_Fcns->bRefLftOper)
        p = lpwOp1ValHdr;
    else
        p = lpwFcnValHdr;

    // Decide amongst the valences
    if (lpSF_Fcns->bSetAlpha)
        q = p[FCNVALENCE_AMB];
    else
    if (lpSF_Fcns->bRefAlpha)
        q = p[FCNVALENCE_DYD];
    else
    if (lpSF_Fcns->bRefOmega)
        q = p[FCNVALENCE_MON];
    else
        q = p[FCNVALENCE_NIL];

    return q;
} // End ConstructFcnHdr


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
    UINT           uLineLen,                // Line length
                   uLineNum,                // Current line # in the Edit Ctrl (0 = header)
                   uOffset,                 // Cumulative offset
                   numPhyLines,             // # physical lines in the function
                   numLogLines;             // # logical  ...
    HGLOBAL        hGlbTxtHdr = NULL,       // Header text global memory handle
                   hGlbTknHdr = NULL,       // Tokenized header text ...
                   hGlbDfnHdr = NULL;       // User-defined function/operator header ...
    LPWCHAR        lpwUdfoHdr;              // Ptr to function header text
    LPDFN_HEADER   lpMemDfnHdr = NULL;      // Ptr to user-defined function/operator header ...
    LPMEMTXT_UNION lpMemTxtLine;            // Ptr to header/line text global memory
    FHLOCALVARS    fhLocalVars = {0};       // Function Header local vars
    HGLOBAL        hGlbOldDfn = NULL;       // Old Dfn global memory handle
    LPPERTABDATA   lpMemPTD;                // Ptr to PerTabData global memory
    WCHAR          wszTemp[1024];           // Save area for error message text
    MEMVIRTSTR     lclMemVirtStr[1] = {0};  // Room for one GuardAlloc
    LPTOKEN        lptkCSBeg;               // Ptr to next token on the CS stack

    Assert (lpSF_Fcns->sfTypes NE SFTYPES_UNK);

    // Fill in common values
    lpSF_Fcns->bRet = FALSE;
    lpSF_Fcns->uErrLine = NEG1U;

    Assert ((hWndFE EQ NULL) || IzitFE (hWndFE));

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
                         &lpSF_Fcns->htsDFN,        // Ptr to HshTab Struc
                          DEF_AFO_HSHTAB_NBLKS,     // # blocks in this HshTab
                          DEF_AFO_HSHTAB_INCRNELM,  // # HTEs by which to resize when low
                          DEF_AFO_HSHTAB_MAXNELM))  // Maximum # HTEs
            goto WSFULL_EXIT;

        if (!AllocSymTab (NULL,                     // Ptr to this entry in MemVirtStr (may be NULL if global allocation)
                          lpMemPTD,                 // Ptr to PerTabData global memory
                         &lpSF_Fcns->htsDFN,        // Ptr to HshTab Struc
                          DEF_AFO_SYMTAB_INITNELM,  // Initial # STEs in SymTab
                          DEF_AFO_SYMTAB_INCRNELM,  // # STEs by which to resize when low
                          DEF_AFO_SYMTAB_MAXNELM))  // Maximum # STEs
            goto WSFULL_EXIT;

        // Mark as global Hsh & Sym tabs
        lpSF_Fcns->htsDFN.bGlbHshSymTabs = TRUE;

        // Put the new one into effect so that symbol table
        //   references are made in the new tables
        lpSF_Fcns->lpHTS = &lpSF_Fcns->htsDFN;

        // Append all system names to the local SymTab
        SymTabAppendAllSysNames_EM (lpSF_Fcns->lpHTS);

        // Assign default values to the system vars
        AssignDefaultHTSSysVars (lpMemPTD, lpSF_Fcns->lpHTS);

        // Link the current HshTab & SymTabs to the new
        // This link is what provides static scoping
        lpSF_Fcns->htsDFN.lphtsPrvSrch = lpMemPTD->lphtsPTD;
    } // End IF

    // Get the handle to the Edit Ctrl
    if (hWndFE NE NULL)
        (HANDLE_PTR) hWndEC = GetWindowLongPtrW (hWndFE, GWLSF_HWNDEC);

    // If we're parsing an AFO from MakeAFE or SysFnMonFX_EM, ...
    if (lpSF_Fcns->bAFO && (lpSF_Fcns->bMakeAFE || lpSF_Fcns->bMakeAFX))
    {
        // Get # physical & logical lines in the function (excluding the header)
        numPhyLines = (*lpSF_Fcns->SF_NumPhyLines) (hWndEC, lpSF_Fcns);
        numLogLines = (*lpSF_Fcns->SF_NumLogLines) (hWndEC, lpSF_Fcns);

        // Get size of tokenization of all lines (excluding the header)
        for (uOffset = 0, uLineNum = 1; uLineNum <= numPhyLines; uLineNum++)
            // If the preceding physical line is not continued to the current line, ...
            if (!(*lpSF_Fcns->SF_IsLineCont) (hWndEC, lpSF_Fcns, uLineNum - 1))
            // Size a function line
            if (SaveFunctionLine (lpSF_Fcns, NULL, NULL, uLineNum, NULL, hWndEC, hWndFE, &uOffset) EQ -1)
                goto ERROR_EXIT;
        // Restore the ptr to the next token on the CS stack
        //   because we advanced it during the above sizing
        lpMemPTD->lptkCSNxt = lptkCSBeg;

        // Clear the # locals as we incremented it during the above sizing
        lpSF_Fcns->numLocalsSTE = 0;

        // Based upon the information gathered from the above sizing (actually from the tokenization)
        //   we now know enough to construct the correct header
        lpwUdfoHdr = ConstructFcnHdr (lpSF_Fcns);

        // Get its length
        uLineLen = lstrlenW (lpwUdfoHdr);
    } else
        // Get the length of the header line
        uLineLen = (*lpSF_Fcns->SF_LineLen) (hWndEC, lpSF_Fcns, 0);

    // Allocate space for the text
    //   (the "sizeof (lpMemTxtLine->U)" is for the leading line length
    //    and the "+ 1" is for the terminating zero)
    // Note, we can't use DbgGlobalAlloc here as we
    //   might have been called from the Master Frame
    //   via a system command, in which case there is
    //   no PTD for that thread.
    hGlbTxtHdr =
      DbgGlobalAlloc (GHND, sizeof (lpMemTxtLine->U) + (uLineLen + 1) * sizeof (lpMemTxtLine->C));
    if (hGlbTxtHdr EQ NULL)
    {
        // Mark the line in error
        lpSF_Fcns->uErrLine = 0;

        if (hWndFE NE NULL)
        {
            // Display the error message
            MessageBoxW (hWndMF,
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
    if (uLineLen NE 0)
    {
        // Lock the memory to get a ptr to it
        lpMemTxtLine = MyGlobalLock000 (hGlbTxtHdr);    // ->U not assigned as yet

        // Save the line length
        lpMemTxtLine->U = uLineLen;

        // Tell EM_GETLINE maximum # chars in the buffer
        lpMemTxtLine->W = (WORD) uLineLen;

        // If we're NOT parsing an AFO from MakeAFE or SysFnMonFX_EM, ...
        if (!(lpSF_Fcns->bAFO && (lpSF_Fcns->bMakeAFE || lpSF_Fcns->bMakeAFX)))
        {
            // Read in the line text
            (*lpSF_Fcns->SF_ReadLine) (hWndEC, lpSF_Fcns, 0, &lpMemTxtLine->C);

            // Point to the function header text
            lpwUdfoHdr = &lpMemTxtLine->C;
        } else
            // Copy the AFO header to the global memory
            CopyMemoryW (&lpMemTxtLine->C, lpwUdfoHdr, lstrlenW (lpwUdfoHdr));

        // Tokenize the function header
        hGlbTknHdr =
          Tokenize_EM (lpwUdfoHdr,          // The line to tokenize (not necessarily zero-terminated)
                       uLineLen,            // NELM of lpwszLine
                       hWndEC,              // Window handle for Edit Ctrl (may be NULL if lpErrHandFn is NULL)
                       0,                   // Function line # (0 = header)
                      &ErrorHandler,        // Ptr to error handling function (may be NULL)
                       lpSF_Fcns,           // Ptr to common struc (may be NULL if unused)
                       FALSE);              // TRUE iff we're tokenizing a Magic Function/Operator
        // We no longer need this ptr
        MyGlobalUnlock (hGlbTxtHdr); lpMemTxtLine = NULL;
    } // End IF

    // If tokenization failed, ...
    if (hGlbTknHdr EQ NULL)
    {
        // Mark the line in error
        lpSF_Fcns->uErrLine = 0;

        if (hWndFE NE NULL)
        {
            // Format the error message
            MySprintfW (wszTemp,
                        sizeof (wszTemp),
                        ERRMSG_SYNTAX_ERROR_IN_FUNCTION_HEADER APPEND_NAME,
                        lpMemPTD->uCaret);
            // Display the error message
            MessageBoxW (hWndMF,
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
    if (lclMemVirtStr[0].IniAddr EQ NULL)
    {
        if (hWndFE NE NULL)
        {
            // Display the error message
            MessageBoxW (hWndMF,
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
        UINT         numResultSTE,      // # result STEs (may be zero)
                     numLftArgSTE,      // # left arg ...
                     numRhtArgSTE,      // # right ...
                     numLocalsSTE,      // # locals ...
                     numSTE;            // Loop counter
        LPFCNLINE    lpFcnLines;        // Ptr to array of function line structs (FCNLINE[numLogLines])
        LPSYMENTRY   lpSymName = NULL,  // Ptr to SYMENTRY for the function name
                    *lplpSymDfnHdr;     // Ptr to LPSYMENTRYs at end of user-defined function/operator header
        SYSTEMTIME   systemTime;        // Current system (UTC) time
        FILETIME     ftCreation;        // Creation time
        CSLOCALVARS  csLocalVars = {0}; // CS local vars

        // Check on invalid function name (e.g. empty function header/body)
        if (fhLocalVars.lpYYFcnName EQ NULL)
        {
            // Mark the line in error
            lpSF_Fcns->uErrLine = 0;

            goto ERROR_EXIT;
        } // End IF

        // Only from FE (handles localization already), ...
        if (lpSF_Fcns->sfTypes EQ SFTYPES_FE)
            // Find the next )SI in which this name is localized
            FindSILocalizedName (&fhLocalVars.lpYYFcnName->tkToken.tkData.tkSym,
                                  lpMemPTD,
                                  hGlbTknHdr,
                                  fhLocalVars.offFcnName);
        // Get the current system (UTC) time
        GetSystemTime (&systemTime);

        // If we don't already have the SymName, ...
        if (lpSF_Fcns->lpSymName EQ NULL)
            // Get the one from ParseFcnHeader
            lpSymName = fhLocalVars.lpYYFcnName->tkToken.tkData.tkSym;
        else
        {
            // Use the one from the FE window
            lpSymName = lpSF_Fcns->lpSymName;

            // Save in fh Local Vars
            fhLocalVars.lpYYFcnName->tkToken.tkData.tkSym = lpSymName;
        } // End IF/ELSE

        // If this is not an AFO, ...
        //    or it is and we're called from <SaveFunction> to save an AFO
        //    or it is and we're called from <SysFnMonFX_EM> to save an AFO
        if (!fhLocalVars.bAFO || lpSF_Fcns->bMakeAFE || lpSF_Fcns->bMakeAFX)
            // Get the old Dfn global memory handle
            hGlbOldDfn = lpSymName->stData.stGlbData;

        // If it's already in memory, get its creation time
        //   and then free it
        if (hGlbOldDfn NE NULL)
        {
            LPSIS_HEADER lpSISCur;

            // Get a ptr to the current SI stack
            lpSISCur = lpMemPTD->lpSISCur;

            // Check for already on the SI stack
            for (;
                 lpSISCur NE NULL;
                 lpSISCur = lpSISCur->lpSISPrv)
            if (lpSISCur->hGlbDfnHdr NE NULL
             && ClrPtrTypeDir (lpSISCur->hGlbDfnHdr) EQ ClrPtrTypeDir (hGlbOldDfn))
            {
                if (hWndFE NE NULL)
                {
                    // Display the error message
                    MessageBoxW (hWndMF,
                                 L"SI Damage in pending function:  changes to this function NOT saved",
                                 lpwszAppName,
                                 MB_OK | MB_ICONWARNING | MB_APPLMODAL);
                    SetFocus (GetParent (hWndEC));
                } else
                    ErrorMessageIndirectToken (ERRMSG_SI_DAMAGE APPEND_NAME,
                                               lpSF_Fcns->lptkFunc);
                goto ERROR_EXIT;
            } // End FOR/IF

            // If it's a UDFO, ...
            if (IsGlbTypeDfnDir_PTB (MakePtrTypeGlb (hGlbOldDfn)))
            {
                // Lock the memory to get a ptr to it
                lpMemDfnHdr = MyGlobalLockDfn (hGlbOldDfn);

                // Get the creation time
                ftCreation = lpMemDfnHdr->ftCreation;

                // We no longer need this ptr
                MyGlobalUnlock (hGlbOldDfn); lpMemDfnHdr = NULL;
            } else
            // If it's a function array, ...
            if (IsGlbTypeFcnDir_PTB (MakePtrTypeGlb (hGlbOldDfn)))
            {
                LPFCNARRAY_HEADER lpMemHdrFcn;  // Ptr to FCNARRAY header

                // Lock the memory to get a ptr to it
                lpMemHdrFcn = MyGlobalLockDfn (hGlbOldDfn);

                // Get the creation time
                ftCreation = lpMemHdrFcn->ftCreation;

                // We no longer need this ptr
                MyGlobalUnlock (hGlbOldDfn); lpMemHdrFcn = NULL;
            } else
            {
                if (hWndFE NE NULL)
                {
                    // Format the error message
                    MySprintfW (wszTemp,
                                sizeof (wszTemp),
                               L"New object name is <%s>, not a user-defined function/operator:  changes to this function NOT saved",
                                lpwNameTypeStr[lpSymName->stFlags.stNameType]);
                    // Display the error message
                    MessageBoxW (hWndMF,
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
            (*lpSF_Fcns->SF_CreationTime) (lpSF_Fcns, &systemTime, &ftCreation);

        // Get # extra result STEs
        if (fhLocalVars.lpYYResult NE NULL)
            // Save in global memory
            numResultSTE = fhLocalVars.lpYYResult->uStrandLen;
        else
            numResultSTE = 0;

        // Get # extra left arg STEs
        if (fhLocalVars.lpYYLftArg NE NULL)
            // Save in global memory
            numLftArgSTE = fhLocalVars.lpYYLftArg->uStrandLen;
        else
            numLftArgSTE = 0;

        // Get # extra right arg STEs
        if (fhLocalVars.lpYYRhtArg NE NULL)
            // Save in global memory
            numRhtArgSTE = fhLocalVars.lpYYRhtArg->uStrandLen;
        else
            numRhtArgSTE = 0;

        // Get # locals STEs
        if (fhLocalVars.lpYYLocals NE NULL)
            // Save in global memory
            numLocalsSTE = fhLocalVars.lpYYLocals->uStrandLen;
        else
            numLocalsSTE = 0;

        // Get # physical & logical lines in the function (excluding the header)
        numPhyLines = (*lpSF_Fcns->SF_NumPhyLines) (hWndEC, lpSF_Fcns);
        numLogLines = (*lpSF_Fcns->SF_NumLogLines) (hWndEC, lpSF_Fcns);

        // Get size of tokenization of all lines (excluding the header)
        for (uOffset = 0, uLineNum = 1; uLineNum <= numPhyLines; uLineNum++)
            // If the preceding physical line is not continued to the current line, ...
            if (!(*lpSF_Fcns->SF_IsLineCont) (hWndEC, lpSF_Fcns, uLineNum - 1))
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
                              + sizeof (FCNLINE) * numLogLines
                              + uOffset);
        if (hGlbDfnHdr EQ NULL)
        {
            if (hWndFE NE NULL)
            {
                // Display the error message
                MessageBoxW (hWndMF,
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
        lpMemDfnHdr = MyGlobalLock000 (hGlbDfnHdr);

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
        lpMemDfnHdr->bLclRL       = fhLocalVars.bLclRL;
        lpMemDfnHdr->RefCnt       = 1;
        lpMemDfnHdr->numFcnLines  = numLogLines;
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
        (*lpSF_Fcns->SF_LastModTime) (lpSF_Fcns, &systemTime, &lpMemDfnHdr->ftLastMod);

        // Get the ptr to the start of the Undo Buffer
        lpMemDfnHdr->hGlbUndoBuff = (*lpSF_Fcns->SF_UndoBuffer) (hWndEC, lpSF_Fcns);

        // Save the dynamic parts of the function into global memory

        // Initialize cumulative offset
        uOffset = sizeof (DFN_HEADER);

        // Initialize ptr to ptr to SYMENTRYs at end of header
        lplpSymDfnHdr = (LPAPLHETERO) ByteAddr (lpMemDfnHdr, uOffset);

        // If there's a result, ...
        if (fhLocalVars.lpYYResult NE NULL)
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
        if (fhLocalVars.lpYYLftArg NE NULL)
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
        if (fhLocalVars.lpYYRhtArg NE NULL)
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
        if (fhLocalVars.lpYYLocals NE NULL)
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
        if (numLocalsSTE NE 0)
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
        lpMemDfnHdr->offTknLines = uOffset + numLogLines * sizeof (FCNLINE);

        // Get ptr to array of function line structs (FCNLINE[numLogLines])
        lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

        // Initialize the offset of where to start saving the tokenized lines
        uOffset = lpMemDfnHdr->offTknLines;

        // Loop through the lines (excluding the header)
        for (uLineNum = 1; uLineNum <= numPhyLines; uLineNum++)
        // If the preceding physical line is not continued to the current line, ...
        if (!(*lpSF_Fcns->SF_IsLineCont) (hWndEC, lpSF_Fcns, uLineNum - 1))
        {
            // Save a function line
            uLineLen =
              SaveFunctionLine (lpSF_Fcns, NULL, lpMemDfnHdr, uLineNum, lpFcnLines, hWndEC, hWndFE, &uOffset);
            if (uLineLen EQ -1)
                goto ERROR_EXIT;

            // If tokenization failed, ...
            if (lpFcnLines->offTknLine EQ 0)
            {
                if (hWndFE NE NULL)
                {
                    // Format the error message
                    MySprintfW (wszTemp,
                                sizeof (wszTemp),
                               L"SYNTAX ERROR on line # %d, position %d -- function not saved",
                                uLineNum,
                                lpMemPTD->uCaret);
                    // Display the error message
                    MessageBoxW (hWndMF,
                                 wszTemp,
                                 lpwszAppName,
                                 MB_OK | MB_ICONWARNING | MB_APPLMODAL);
                    SetFocus (GetParent (hWndEC));
                } else
                    // Save the line # in error (origin-0)
                    lpSF_Fcns->uErrLine = uLineNum;

                goto ERROR_EXIT;
            } // End IF

            // Transfer Stop & Trace info
            lpFcnLines->bStop  =
            lpFcnLines->bTrace = FALSE;     // ***FIXME*** -- transfer from orig fn

            // Skip to the next struct
            lpFcnLines++;
        } // End FOR/IF

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
            if (hWndFE NE NULL)
            {
                // Format the error message
                MySprintfW (wszTemp,
                            sizeof (wszTemp),
                           L"%s on line # %d, statement #%d, position %d -- function not saved",
                            csLocalVars.lpwszErrMsg,
                            csLocalVars.tkCSErr.tkData.Orig.c.uLineNum,
                            csLocalVars.tkCSErr.tkData.Orig.c.uStmtNum + 1,
                            lpMemPTD->uCaret);
                // Display the error message
                MessageBoxW (hWndMF,
                             wszTemp,
                             lpwszAppName,
                             MB_OK | MB_ICONWARNING | MB_APPLMODAL);
                SetFocus (GetParent (hWndEC));
            } else
                // Save the line # in error (origin-0)
                lpSF_Fcns->uErrLine = csLocalVars.tkCSErr.tkData.Orig.c.uLineNum;

            goto ERROR_EXIT;
        } // End IF

        // Save flag if Ctrl Strucs in AFOs
        lpMemDfnHdr->bAfoCtrlStruc = (lpSF_Fcns->bAFO
                                   && csLocalVars.bMainStmt);
        // Check for line labels ([]ID, etc.)
        if (!GetLabelNums (lpMemDfnHdr, hWndEC, hWndFE NE NULL, lpSF_Fcns))
            goto ERROR_EXIT;

        // If there was a previous function, ...
        if (hGlbOldDfn NE NULL)
        {
            // Zap the previous global memory handle
            SetWindowLongPtrW (hWndFE, GWLSF_HGLBDFNHDR, (HANDLE_PTR) NULL);

            // Free it
            FreeResultGlobalDFLV (hGlbOldDfn); hGlbOldDfn = NULL;
        } // End IF

        // If we're parsing an AFO, ...
        if (lpSF_Fcns->bAFO)
        {
            // Save the []RL{is} setting
            lpMemDfnHdr->bLclRL |= lpSF_Fcns->bLclRL;

            // Save the new HTS in global memory
            lpMemDfnHdr->htsDFN = lpSF_Fcns->htsDFN;

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
        } // End IF

        // If this is not an AFO, ...
        //    or it is and we're called from <SaveFunction> to save an AFO
        //    or it is and we're called from <SysFnMonFX_EM> to save an AFO
        if (!lpSF_Fcns->bAFO || lpSF_Fcns->bMakeAFE || lpSF_Fcns->bMakeAFX)
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
        } // End IF

        // If the caller is the Function Editor, ...
        if (hWndFE NE NULL)
        {
            // Mark as unchanged since the last save
            SetWindowLongW (hWndFE, GWLSF_CHANGED, FALSE);

            // Write out the FE window title to unmark the Changed flag
            SetFETitle (hWndFE);
        } // End IF

        // We no longer need this ptr
        MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;

        lpSF_Fcns->bRet = TRUE;
        lpSF_Fcns->lpSymName = lpSymName;

        goto NORMAL_EXIT;
    } else
    {
        // Copy the error message up the line
        strcpyW (lpSF_Fcns->wszErrMsg, fhLocalVars.wszErrMsg);

        // Copy the error line # up the line
        lpSF_Fcns->uErrLine = 0;
    } // End IF/ELSE

    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lpSF_Fcns->lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hWndFE NE NULL)
        // Ensure the FE window redraws the caret
        SetFocus (hWndFE);

    if (hGlbDfnHdr NE NULL)
    {
        if (lpMemDfnHdr NE NULL)
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

    if (hGlbTknHdr NE NULL)
    {
        LPTOKEN_HEADER lpMemTknHdr;         // Ptr to tokenized line header global memory

        // Lock the memory to get a ptr to it
        lpMemTknHdr = MyGlobalLockTkn (hGlbTknHdr);

        // Free the tokens
        Untokenize (lpMemTknHdr);

        // Unlock and free (and set to NULL) a global name and ptr
        UnlFreeGlbName (hGlbTknHdr, lpMemTknHdr);
    } // End IF

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbTxtHdr, lpMemTxtLine);
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
     UINT           uLineNum,               // Current line # in the Edit Ctrl (0 = header)
     LPFCNLINE      lpFcnLines,             // Ptr to array of function line structs (FCNLINE[numLogLines]) (may be NULL if sizing)
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
    if (lpMagicFcnOpr NE NULL)
        // Get the line length of the line
        uLineLen = lstrlenW (lpMagicFcnOpr->Body[uLineNum - 1]);
    else
        // Get the length of the function line
        uLineLen = (*lpSF_Fcns->SF_LineLen) (hWndEC, lpSF_Fcns, uLineNum);

    // Calculate extra WCHARs in case we need to surround the line with braces
    uLen = 2 * (lpSF_Fcns && lpSF_Fcns->bAFO && !lpSF_Fcns->bMakeAFO);

    // Allocate global memory to hold this text
    // The "sizeof (lpMemTxtLine->U) + " is for the leading length
    //   and the "+ 1" is for the terminating zero
    //   and the "+ uLen" is for the extra WCHARs in case we need them for AFOs
    //   as well as to handle GlobalLock's aversion to locking
    //   zero-length arrays
    hGlbTxtLine = DbgGlobalAlloc (GHND, sizeof (lpMemTxtLine->U) + (uLineLen + 1 + uLen) * sizeof (APLCHAR));
    if (hGlbTxtLine EQ NULL)
    {
        if (hWndFE NE NULL)
        {
            // Display the error message
            MessageBoxW (hWndMF,
                         L"Insufficient memory to save a function line!!",
                         lpwszAppName,
                         MB_OK | MB_ICONWARNING | MB_APPLMODAL);
            SetFocus (GetParent (hWndEC));

            goto ERROR_EXIT;
        } else
            goto WSFULL_EXIT;
    } // End IF

    // If we're not sizing, ...
    if (lpFcnLines NE NULL)
        // Save the global memory handle
        lpFcnLines->hGlbTxtLine = hGlbTxtLine;

    // Lock the memory to get a ptr to it
    lpMemTxtLine = MyGlobalLock000 (hGlbTxtLine);   // ->U not assigned as yet

    // Save the line length
    lpMemTxtLine->U = uLineLen;

    // The following test isn't an optimzation, but avoids
    //   overwriting the allocation limits of the buffer
    //   when filling in the leading WORD with uLineLen
    //   on the call to EM_GETLINE.

    // If the line is non-empty, ...
    if (uLineLen NE 0)
    {
        // Tell EM_GETLINE maximum # chars in the buffer
        lpMemTxtLine->W = (WORD) uLineLen;

        // Point to the start of the line
        lpwszLine = &lpMemTxtLine->C;

        // If it's a Magic Function, ...
        if (lpMagicFcnOpr NE NULL)
            // Copy the line text to global memory
            CopyMemoryW (lpwszLine, lpMagicFcnOpr->Body[uLineNum - 1], uLineLen);
        else
        {
            // Read in the line text
            (*lpSF_Fcns->SF_ReadLine) (hWndEC, lpSF_Fcns, uLineNum, lpwszLine);

            // If we're about to tokenize an AFO
            //   and we're not called from MakeAFO
////        //   and we're not called from MakeAFE, ...
////        //   and we're not called from MakeAFX, ...
            if (lpSF_Fcns->bAFO
             && !lpSF_Fcns->bMakeAFO)
////         && !lpSF_Fcns->bMakeAFE
////         && !lpSF_Fcns->bMakeAFX)
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
                               uLineNum,                // Function line # (0 = header)
                              &ErrorHandler,            // Ptr to error handling function (may be NULL)
                               lpSF_Fcns,               // Ptr to common struc (may be NULL if unused)
                              (lpMagicFcnOpr NE NULL)
                            || (lpSF_Fcns
                             && lpSF_Fcns->bMFO)
                            || (lpMemDfnHdr
                             && lpMemDfnHdr->bMFO));    // TRUE iff we're tokenizing a Magic Function/Operator
                // We no longer need this storage
                DbgGlobalFree (hGlbTknHdr); hGlbTknHdr = NULL;

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
                   uLineNum,                // Function line # (0 = header)
                  &ErrorHandler,            // Ptr to error handling function (may be NULL)
                   lpSF_Fcns,               // Ptr to common struc (may be NULL if unused)
                  (lpMagicFcnOpr NE NULL)
                || (lpSF_Fcns
                 && lpSF_Fcns->bMFO)
                || (lpMemDfnHdr
                 && lpMemDfnHdr->bMFO));    // TRUE iff we're tokenizing a Magic Function/Operator
    // We no longer need this ptr
    MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;

    // If we're sizing, ...
    if (lpFcnLines EQ NULL)
    {
        // We no longer need this storage
        DbgGlobalFree (hGlbTxtLine); hGlbTxtLine = NULL;
    } // End IF

    // If tokenization failed, ...
    if (hGlbTknHdr EQ NULL)
    {
        // If it's valid, ...
        if (lpSF_Fcns NE NULL)
            // Mark the line in error
            lpSF_Fcns->uErrLine = uLineNum;

        if (hWndFE NE NULL)
        {
            // Format the error message
            MySprintfW (wszTemp,
                        sizeof (wszTemp),
                        ERRMSG_SYNTAX_ERROR_IN_FUNCTION_LINE APPEND_NAME,
                        uLineNum,
                        lpMemPTD->uCaret);
            // Display the error message
            MessageBoxW (hWndMF,
                        wszTemp,
                        lpwszAppName,
                        MB_OK | MB_ICONWARNING | MB_APPLMODAL);
            SetFocus (GetParent (hWndEC));
        } // End IF

        goto ERROR_EXIT;
    } // End IF

    // If we're not sizing, ...
    if (lpFcnLines NE NULL)
        // Check the line for empty
        lpFcnLines->bEmpty =
          IsLineEmpty (hGlbTknHdr);

    // Save the token size
    uTknSize = (UINT) MyGlobalSize (hGlbTknHdr);

    // Lock the memory to get a ptr to it
    lpMemTknHdr = MyGlobalLockTkn (hGlbTknHdr);

    // If we're not sizing, ...
    if (lpFcnLines NE NULL)
    {
        // Copy the tokens to the end of the function header
        CopyMemory (ByteAddr (lpMemDfnHdr, *lpOffNextTknLine), lpMemTknHdr, uTknSize);

        // Save the offset
        lpFcnLines->offTknLine = *lpOffNextTknLine;
    } else
        // Free the storage in the tokens
        Untokenize (lpMemTknHdr);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbTknHdr, lpMemTknHdr);

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
//  $FindSILocalizedName
//***************************************************************************

void FindSILocalizedName
    (LPSYMENTRY  *lplpSym,              // Ptr to ptr to SYMENTRY of function name
     LPPERTABDATA lpMemPTD,             // Ptr to PerTabData global memory
     HGLOBAL      hGlbTknHdr,           // Token header global memory handle (may be NULL)
     UINT         offFcnName)           // Offset in tokens in hGlbTknHdr of the function name

{
    LPSIS_HEADER lpSISCur;              // Ptr to current SIS_HEADER srtuc
    LPWCHAR      lpwFcnName,            // Ptr to the function's character name
                 lpwSymName;            // ...        SYMENTRY's ...
    UBOOL        bFound = FALSE;        // TRUE iff we found the local name
    LPSYMENTRY   lpSymEntry;            // Ptr to next localized LPSYMENTRY on the SIS

    // Lock the name to get a ptr to it
    lpwFcnName = MyGlobalLockWsz ((*lplpSym)->stHshEntry->htGlbName);

    // Loop backwards through the SI levels
    for (lpSISCur = lpMemPTD->lpSISCur;
         lpSISCur && !bFound;
         lpSISCur = lpSISCur->lpSISPrv)
    {
        // Split cases based upon the caller's function type
        switch (lpSISCur->DfnType)
        {
            case DFNTYPE_OP1:
            case DFNTYPE_OP2:
            case DFNTYPE_FCN:
            {
                UINT numSymEntries,         // # LPSYMENTRYs localized on the stack
                     numSym;                // Loop counter

                // Get # LPSYMENTRYs on the stack
                numSymEntries = lpSISCur->numSymEntries;

                // Point to the localized LPSYMENTRYs
                lpSymEntry = (LPSYMENTRY) ByteAddr (lpSISCur, sizeof (SIS_HEADER));

                // Loop through the # LPSYMENTRYs
                for (numSym = 0; numSym < numSymEntries; numSym++, lpSymEntry++)
                {
                    // Lock the name to get a ptr to it
                    lpwSymName = MyGlobalLockWsz (lpSymEntry->stHshEntry->htGlbName);

                    // Compare the STE names
                    if (lstrcmpW (lpwFcnName, lpwSymName) EQ 0)
                    {
                        // Save the LPSYMENTRY into the header tokens and incoming SYMENTRY
                        SaveSymEntry (lplpSym, hGlbTknHdr, offFcnName, lpSymEntry);

                        // Mark as found
                        bFound = TRUE;

                        break;
                    } // End IF

                    // We no longer need this ptr
                    MyGlobalUnlock (lpSymEntry->stHshEntry->htGlbName); lpwSymName = NULL;
                } // End FOR

                break;
            } // End DFN_OP1/OP2/FCN

            case DFNTYPE_IMM:
            case DFNTYPE_EXEC:
            case DFNTYPE_QUAD:
                break;

            case DFNTYPE_UNK:
            defstop
                break;
        } // End SWITCH
    } // End FOR

    // If not found, ...
    if (!bFound)
    {
        LPHSHTABSTR lphtsPTD,
                    lphtsOld;

        // Find the outer HTS
        for (lphtsOld = lphtsPTD = lpMemPTD->lphtsPTD;
             lphtsPTD NE NULL;
             lphtsOld = lphtsPTD,
               lphtsPTD = lphtsPTD->lphtsPrvSrch)
             ;
        // If it's valid, ...
        if (lphtsOld NE NULL)
        {
            // Lookup in or append to the symbol table
            lpSymEntry =
              SymTabHTSAppendName_EM (lpwFcnName,       // Ptr to name
                                      NULL,             // Ptr to incoming stFlags (may be NULL)
                                      FALSE,            // TRUE iff the name is to be local to the given HTS
                                      lphtsOld);        // Ptr to HshTab struc (may be NULL)
            // If it's valid, ...
            if (lpSymEntry NE NULL)
                // Save the LPSYMENTRY into the header tokens and incoming SYMENTRY
                SaveSymEntry (lplpSym, hGlbTknHdr, offFcnName, lpSymEntry);
        } // End IF
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock ((*lplpSym)->stHshEntry->htGlbName); lpwFcnName = NULL;
} // FindSILocalizedName


//***************************************************************************
//  $SaveSymEntry
//***************************************************************************

void SaveSymEntry
    (LPSYMENTRY *lplpSym,               // Ptr to ptr to SYMENTRY of function name
     HGLOBAL     hGlbTknHdr,            // Token header global memory handle (may be NULL)
     UINT        offFcnName,            // Offset in tokens in hGlbTknHdr of the function name
     LPSYMENTRY  lpSymEntry)            // Ptr to new LPSYMENTRY

{
    // If it's valid, ...
    if (hGlbTknHdr NE NULL)
    {
        LPTOKEN_HEADER lpMemTknHdr;     // Ptr to token header
        LPTOKEN        lpMemTknLine;    // Ptr to line of tokens

        // Lock the memory to get a ptr to it
        lpMemTknHdr = MyGlobalLockTkn (hGlbTknHdr);

        // Skip over the token header
        lpMemTknLine = TokenBaseToStart (lpMemTknHdr);

        Assert (IsTknNamed (&lpMemTknLine[offFcnName]));

        // Change the LPSYMENTRY in the token in hGlbTknHdr
        lpMemTknLine[offFcnName].tkData.tkSym = lpSymEntry;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbTknHdr); lpMemTknLine = NULL;
    } // End IF

    // Store as the local SymEntry
    (*lplpSym) = lpSymEntry;
} // End SaveSymEntry


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
    lptkLine = MyGlobalLockTkn (hGlbTknHdr);

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

#ifdef DEBUG
#define APPEND_NAME     L" -- GetLabelNums"
#else
#define APPEND_NAME
#endif

UBOOL GetLabelNums
    (LPDFN_HEADER  lpMemDfnHdr,         // Ptr to user-defined function/operator header
     HWND          hWndEC,              // Edit Ctrl window handle (FE only)
     UBOOL         bDispErrMsg,         // TRUE iff we may display error messages
     LPSF_FCNS     lpSF_Fcns)           // Ptr to common struc (may be NULL)

{
    UINT           numLogLines,         // # logical lines in the function
                   uDupLineNum1,        // Line # of duplicate label (origin-1)
                   uCnt,                // Loop counter
                   uLineNum1;           // Line # (origin-1)
    LPLBLENTRY    *lplpLblEntry = NULL, // Ptr to array of LPLBLENTRYs of labeled lines
                   lpLblEntry;          // Ptr to an individual LBLENTRY
    LPFCNLINE      lpFcnLines,          // Ptr to array of function line structs (FCNLINE[numLogLines])
                   lpLstLabel;          // Ptr to the last labeled line
    LPTOKEN_HEADER lptkHdr;             // Ptr to header of tokenized line
    LPTOKEN        lptkLine;            // Ptr to tokenized line
    UBOOL          bRet;                // TRUE iff the result is valid
    HGLOBAL        hGlbName;            // Name's global memory handle

    // Get # logical lines in the function
    numLogLines = lpMemDfnHdr->numFcnLines;

    // Allocate room for <numLogLines> of <LPSYMENTRY>s for the line labels
    //   so we can sort the labels and find duplicates
    lplpLblEntry = DbgGlobalAlloc (GPTR, numLogLines * (sizeof (LBLENTRY) + sizeof (LPVOID)));
    if (lplpLblEntry EQ NULL)
        goto WSFULL_EXIT;

    // Save ptr to start of LBLENTRYs
    lpLblEntry = (LPLBLENTRY) &lplpLblEntry[numLogLines];

    // Get ptr to array of function line structs (FCNLINE[numLogLines])
    lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

    // Save as ptr to the last labeled line
    lpLstLabel = NULL;
    lpMemDfnHdr->numLblLines = 0;

    // Loop through the function lines
    for (uLineNum1 = 1; uLineNum1 <= numLogLines; uLineNum1++)
    if (lpFcnLines->offTknLine NE 0)
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
            // If the first token is a name
            //   and the next one is a label separator,
            // OR
            // This is an AFO, and
            // If the first token is a NOP
            //   and the next token is a sys name
            //   and the next one is a label separator, ...
            if ((lptkLine[1].tkFlags.TknType EQ TKT_VARNAMED
              && lptkLine[2].tkFlags.TknType EQ TKT_LABELSEP)
             || (lpMemDfnHdr->bAFO
              && lptkLine[1].tkFlags.TknType EQ TKT_NOP
              && IsTknSysName (&lptkLine[2], TRUE)
              && lptkLine[3].tkFlags.TknType EQ TKT_LABELSEP))
            {
                LPAPLCHAR lpMemName;    // Ptr to the name
                UINT      uCnt;

                // Identify the named token
                if (IsTknNamed (&lptkLine[1]))
                    uCnt = 1;
                else
                if (IsTknNamed (&lptkLine[2]))
                    uCnt = 2;
#ifdef DEBUG
                else
                    DbgBrk ();
#endif
                // stData is an LPSYMENTRY
                Assert (GetPtrTypeDir (lptkLine[uCnt].tkData.tkSym) EQ PTRTYPE_STCONST);

                // If the stHshEntry is valid, ...
                //   (might not be if the System Label is mispelled)
                if (lptkLine[uCnt].tkData.tkSym->stHshEntry NE NULL)
                {
                    // Get the Name's global memory handle
                    hGlbName = lptkLine[uCnt].tkData.tkSym->stHshEntry->htGlbName;

                    // Lock the memory to get a ptr to it
                    lpMemName = MyGlobalLockWsz (hGlbName);

                    if (lstrcmpiW (lpMemName, $QUAD_ID ) EQ 0)
                    {
                        if (uDupLineNum1 = lpMemDfnHdr->nSysLblId  NE 0)
                            goto SYSDUP_EXIT;
                        // Save line # in origin-1
                        lpMemDfnHdr->nSysLblId  = uLineNum1;

                        // Mark as a System Label
                        lpFcnLines->bSysLbl = TRUE;
                    } else
                    if (lstrcmpiW (lpMemName, $QUAD_INV) EQ 0)
                    {
                        if (uDupLineNum1 = lpMemDfnHdr->nSysLblInv NE 0)
                            goto SYSDUP_EXIT;
                        // Save line # in origin-1
                        lpMemDfnHdr->nSysLblInv = uLineNum1;

                        // Mark as a System Label
                        lpFcnLines->bSysLbl = TRUE;
                    } else
                    if (lstrcmpiW (lpMemName, $QUAD_MS ) EQ 0)
                    {
                        if (uDupLineNum1 = lpMemDfnHdr->nSysLblMs  NE 0)
                            goto SYSDUP_EXIT;
                        // Save line # in origin-1
                        lpMemDfnHdr->nSysLblMs  = uLineNum1;

                        // Mark as a System Label
                        lpFcnLines->bSysLbl = TRUE;
                    } else
                    if (lstrcmpiW (lpMemName, $QUAD_PRO) EQ 0)
                    {
                        if (uDupLineNum1 = lpMemDfnHdr->nSysLblPro NE 0)
                            goto SYSDUP_EXIT;
                        // Save line # in origin-1
                        lpMemDfnHdr->nSysLblPro = uLineNum1;

                        // Mark as a System Label
                        lpFcnLines->bSysLbl = TRUE;
                    } // End IF/ELSE/...

                    // If there's a previous label, ...
                    if (lpLstLabel NE NULL)
                        // Save the line # of the next labeled line
                        lpLstLabel->numNxtLabel1 = uLineNum1;
                    else
                        // Save the line # of the first labeled line
                        lpMemDfnHdr->num1stLabel1 = uLineNum1;
                    // Save as the ptr to the last labeled line
                    lpLstLabel = lpFcnLines;

                    // Create a ptr to the LBLENTRY
                    lplpLblEntry[lpMemDfnHdr->numLblLines] = lpLblEntry++;

                    // Save the LBLENTRY of the label
                    lplpLblEntry[lpMemDfnHdr->numLblLines]->lpSymEntry = lptkLine[1].tkData.tkSym;
                    lplpLblEntry[lpMemDfnHdr->numLblLines]->uLineNum1  = uLineNum1;

                    // Mark as a labeled line
                    lpLstLabel->bLabel = TRUE;

                    // Count in another labeled line
                    lpMemDfnHdr->numLblLines++;

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
    if (lpSF_Fcns NE NULL)
        // Save the line # (origin-0)
        lpSF_Fcns->uErrLine = uDupLineNum1 - 1;

    goto ERROR_EXIT;

ERROR_EXIT:
    if (bDispErrMsg && hWndEC NE NULL)
        SetFocus (GetParent (hWndEC));

    // Mark as in error
    bRet = FALSE;
NORMAL_EXIT:
    if (lplpLblEntry NE NULL)
    {
        // We no longer need this storage
        DbgGlobalFree (lplpLblEntry); lplpLblEntry = NULL;
    } // End IF

    return bRet;
} // End GetLabelNums
#undef  APPEND_NAME


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
    lpMem = MyGlobalLockWsz (hGlbName);

    // Lock the memory to get a ptr to it
    lpMemName = MyGlobalLockWsz (lpMemDfnHdr->steFcnName->stHshEntry->htGlbName);

    // Format the error message
    MySprintfW (wszTemp,
                sizeof (wszTemp),
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
