//***************************************************************************
//  NARS2000 -- Display Routines
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2014 Sudley Place Software

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
#define _USE_MATH_DEFINES
#include <math.h>
#include "headers.h"
#include "debug.h"              // For xxx_TEMP_OPEN macros

typedef enum tagDTOA_MODE
{
    DTOAMODE_SHORT_RND = 0,             // 0 = shortest string with rounding, e.g., 1e23
    DTOAMODE_SHORT_NORND,               // 1 = shortest string without rounding. e.g., 9.999999999999999e22
    DTOAMODE_SIGDIGS,                   // 2 = # significant digits
    DTOAMODE_FRACTDIGS,                 // 3 = # fractional digits (past decimal point)
} DTOAMODE;

WCHAR wcInf = UTF16_INFINITY;

// DTOA mode for the corresponding FLTDSPFMT_xxx value
DTOAMODE gDTOA_Mode[FLTDISPFMT_LENGTH] = {DTOAMODE_SIGDIGS,     // E :  2 = nDigits significant digits
                                          DTOAMODE_FRACTDIGS,   // F :  3 = nDigits past decimal point
                                          DTOAMODE_SIGDIGS,     // RI:  2 = nDigits significant digits
                                          DTOAMODE_SHORT_RND};  // RF:  0 = shortest string

typedef struct tagROWPTRS
{
    LPAPLCHAR lpNxtChar,        // Ptr to next entry of formatted data
              lpEndChar;        // ...    byte after row end of ...
} ROWPTRS, *LPROWPTRS;


//***************************************************************************
//  $ArrayDisplay_EM
//
//  Display an array
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- AssignDisplay_EM"
#else
#define APPEND_NAME
#endif

UBOOL ArrayDisplay_EM
    (LPTOKEN lptkRes,               // Ptr to value token
     UBOOL   bEndingCR,             // TRUE iff last line has CR
     LPUBOOL lpbCtrlBreak)          // Ptr to Ctrl-Break flag

{
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPAPLCHAR    lpaplChar;         // Ptr to output save area
    LPWCHAR      lpwszFormat;       // Ptr to formatting save area
    IMM_TYPES    immType;           // Type of immediate value
    APLCHAR      aplChar;           // Immediate value if character

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get ptr to formatting save area
    lpwszFormat = lpMemPTD->lpwszFormat;

    // Split cases based upon the token type
    switch (lptkRes->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRes->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // Check for NoValue
            if (IsSymNoValue (lptkRes->tkData.tkSym))
                return TRUE;

            // If it's not immediate, it's an array
            if (!lptkRes->tkData.tkSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir_PTB (lptkRes->tkData.tkSym->stData.stVoid));

                // Check for NoDisplay flag
                if (!lptkRes->tkFlags.NoDisplay)
                    return
                      DisplayGlbArr_EM (lptkRes->tkData.tkSym->stData.stGlbData,
                                        bEndingCR,          // TRUE iff last line has CR
                                        lpbCtrlBreak,       // Ptr to Ctrl-Break flag
                                        lptkRes);           // Ptr to function token
            } // End IF

            // Check for NoDisplay flag
            if (lptkRes->tkFlags.NoDisplay)
                return TRUE;

            // Handle the immediate case

            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRes->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // stData is immediate
            Assert (lptkRes->tkData.tkSym->stFlags.Imm);

            lpaplChar =
              FormatSymTabConst (lpwszFormat,
                                 lptkRes->tkData.tkSym);
            immType = lptkRes->tkData.tkSym->stFlags.ImmType;
            aplChar = lptkRes->tkData.tkSym->stData.stChar;

            break;

        case TKT_VARIMMED:  // The tkData is an immediate constant
            // Check for NoDisplay flag
            if (lptkRes->tkFlags.NoDisplay)
                return TRUE;

            lpaplChar =
              FormatImmed (lpwszFormat,
                           lptkRes->tkFlags.ImmType,
                          &lptkRes->tkData.tkLongest);
            immType = lptkRes->tkFlags.ImmType;
            aplChar = lptkRes->tkData.tkChar;

            break;

        case TKT_LISTPAR:   // The tkData is an HGLOBAL of an array of LPSYMENTRYs/HGLOBALs
            goto SYNTAX_EXIT;

        case TKT_VARARRAY:  // The tkData is an HGLOBAL of an array of LPSYMENTRYs/HGLOBALs
            // Check for NoDisplay flag
            if (lptkRes->tkFlags.NoDisplay)
                return TRUE;

            switch (GetPtrTypeDir (lptkRes->tkData.tkVoid))
            {
                case PTRTYPE_STCONST:
                    lpaplChar =
                      FormatSymTabConst (lpwszFormat,
                                         lptkRes->tkData.tkSym);
                    immType = lptkRes->tkData.tkSym->stFlags.ImmType;
                    aplChar = lptkRes->tkData.tkSym->stData.stChar;

                    break;

                case PTRTYPE_HGLOBAL:
                    return
                      DisplayGlbArr_EM (lptkRes->tkData.tkGlbData,
                                        bEndingCR,          // TRUE iff last line has CR
                                        lpbCtrlBreak,       // Ptr to Ctrl-Break flag
                                        lptkRes);           // Ptr to function token

                defstop
                    return FALSE;
            } // End SWITCH

            break;

////////case TKT_STRAND:    // The tkData is an HGLOBAL of a single HGLOBAL
        defstop
            return FALSE;
    } // End SWITCH

    // Delete the last blank in case it matters,
    //   and ensure properly terminated
    if (lpaplChar[-1] EQ L' ')
        *--lpaplChar = WC_EOS;
    else
        *lpaplChar = WC_EOS;

    // If the value is a character, ...
    if (IsImmChr (immType))
    // Split cases based upon the character value
    switch (aplChar)
    {
        case TCBEL:
            // Sound the alarum!
            MessageBeep (NEG1U);

            // Empty the line
            lpwszFormat[0] = WC_EOS;

            break;

        case TCHT:
            FillMemoryW (lpwszFormat, uTabStops, L' ');

            // Append a terminating zero
            lpwszFormat[uTabStops] = WC_EOS;

            break;

        case TCLF:
        case TCNL:
        default:
            // If the char is not printable, ...
            if (aplChar < L' ')
                // Empty the line
                lpwszFormat[0] = WC_EOS;
            break;
    } // End IF/SWITCH

    // Display the line
    AppendLine (lpwszFormat, FALSE, bEndingCR);

    return TRUE;

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkRes);
    return FALSE;
} // End ArrayDisplay_EM
#undef  APPEND_NAME


//***************************************************************************
//  $DisplayGlbArr_EM
//
//  Display a global array
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- DisplayGlbArr_EM"
#else
#define APPEND_NAME
#endif

UBOOL DisplayGlbArr_EM
    (HGLOBAL hGlb,                  // Global memory handle to display
     UBOOL   bEndingCR,             // TRUE iff last line has CR
     LPUBOOL lpbCtrlBreak,          // Ptr to Ctrl-Break flag
     LPTOKEN lptkFunc)              // Ptr to function token

{
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPVOID       lpMem = NULL;      // Ptr to incoming global memory
    LPAPLCHAR    lpaplCharIni = NULL,
                 lpaplChar,
                 lpaplCharStart,
                 lpwszNxt,          // Ptr to next char of input
                 lpwszEnd;          // ..     end  char of input
    APLSTYPE     aplType;           // Arg storage type
    APLNELM      aplNELM;           // Arg NELM
    APLRANK      aplRank;           // Arg rank
    LPAPLDIM     lpMemDim;          // Ptr to arg dimensions
    APLDIM       aplDimNCols,       // # cols
                 aplDimNRows,       // # rows
                 aplChrNCols,       // # cols for char arrays
                 aplDimCol,         // Col loop counter
                 aplLastDim;        // Length of the last dimension in the result
    APLNELM      aplNELMRes;        // Result NELM
    LPFMTHEADER  lpFmtHeader,       // Ptr to format header struc
                 lpFmtHeader2;      // ...
    LPFMTCOLSTR  lpFmtColStr;       // Ptr to format col struc
    APLUINT      uQuadPW;           // []PW
    UBOOL        bLineCont = FALSE, // TRUE iff this line is a continuation
                 bRawOut,           // TRUE iff using raw output
                 bRet = FALSE;      // TRUE iff the result is valid
    APLUINT      uOutLen;           // Output length for this line
    MEMVIRTSTR   lclMemVirtStr[1] = {0};// Room for one GuardAlloc
    LPWCHAR      lpwszFormat;       // Ptr to formatting save area
    jmp_buf      oldHeapFull;       // Save area for previous jmp_buf

    // Save the previous heapFull
    *oldHeapFull = *heapFull;

    // Set the heapFull for a longjmp
    if (setjmp (heapFull) NE 0)
        goto WSFULL_EXIT;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get ptr to formatting save area
    lpwszFormat = lpMemPTD->lpwszFormat;

    // Get the current value of []PW
    uQuadPW = GetQuadPW ();

    // Allocate space for the display
    lclMemVirtStr[0].lpText   = "lpaplCharIni in <DisplayGlbArr_EM>";
    lclMemVirtStr[0].IncrSize = DEF_DISPLAY_INCRNELM * sizeof (APLCHAR);
    lclMemVirtStr[0].MaxSize  = DEF_DISPLAY_MAXNELM  * sizeof (APLCHAR);
    lclMemVirtStr[0].IniAddr  = (LPUCHAR)
    lpaplCharIni =
      GuardAlloc (NULL,             // Any address
                  lclMemVirtStr[0].MaxSize,
                  MEM_RESERVE,
                  PAGE_READWRITE);
    if (!lclMemVirtStr[0].IniAddr)
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsgW (L"DisplayGlbArr_EM:  GuardAlloc for <lpaplCharIni> failed");

        goto ERROR_EXIT;    // Mark as failed
    } // End IF

    // Link this struc into the chain
    LinkMVS (&lclMemVirtStr[0]);

////// Commit the intial size
////VirtualAlloc (lclMemVirtStr[0].IniAddr,
////              DEF_DISPLAY_INITNELM * sizeof (APLCHAR),
////              MEM_COMMIT,
////              PAGE_READWRITE);
    __try
    {
#ifdef DEBUG
        // Fill lpaplCharIni with FFs so we can tell what we actually wrote
        FillMemory (lpaplCharIni, 1024, 0xFF);
#endif
        // Lock the memory to get a ptr to it
        lpMem = MyGlobalLock (hGlb);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)
        // Get the Array Type, NELM, and Rank
        aplType = lpHeader->ArrType;
        aplNELM = lpHeader->NELM;
        aplRank = lpHeader->Rank;
#undef  lpHeader

        // Skip over the header to the dimensions
        lpMemDim = VarArrayBaseToDim (lpMem);

        // Skip over the header and dimension to the data
        lpMem = VarArrayDataFmBase (lpMem);

        // Get the # columns
        if (IsScalar (aplRank))
            aplDimNCols = aplDimNRows = 1;
        else
        {
            aplDimNCols = lpMemDim[aplRank - 1];

            // If there are no columns and the rank is > 1, ignore this
            if (IsZeroDim (aplDimNCols)
             && IsMultiRank (aplRank))
                goto EMPTY_EXIT;

            // Get the # rows (across all planes)
            if (IsVector (aplRank))
                aplDimNRows = 1;
            else
                aplDimNRows = aplNELM / aplDimNCols;
        } // End IF/ELSE

        // Format char arrays as one col
        aplChrNCols = (IsSimpleChar (aplType)) ? 1 : aplDimNCols;

        // Create a new FMTHEADER
        lpFmtHeader2 = (LPFMTHEADER) lpaplCharIni;
        ZeroMemory (lpFmtHeader2, sizeof (FMTHEADER));
////////lpFmtHeader2->lpFmtHeadUp = NULL;               // Filled in by ZeroMemory
        lpFmtHeader = lpFmtHeader2;
#ifdef DEBUG
        lpFmtHeader->Sig.nature  = FMTHEADER_SIGNATURE;
#endif
////////lpFmtHeader->lpFmtRowUp  = NULL;                // Already zero from ZeroMemory
////////lpFmtHeader->lpFmtColUp  = NULL;                // ...
////////lpFmtHeader->lpFmtRow1st =                      // Filled in below
////////lpFmtHeader->lpFmtCol1st =                      // ...
        lpFmtHeader->uRealRows   = (UINT) aplDimNRows;  // # real rows from dimensions
////////lpFmtHeader->uActRows    = 0;                   // Already zero from ZeroMemory (initial value)
        lpFmtHeader->uActCols    = (UINT) aplChrNCols;
////////lpFmtHeader->uFmtRows    = 0;                   // Already zero from ZeroMemory (initial value)
////////lpFmtHeader->uFmtInts    = 0;                   // ...
////////lpFmtHeader->uFmtChrs    = 0;                   // ...
////////lpFmtHeader->uFmtFrcs    = 0;                   // ...
////////lpFmtHeader->uFmtTrBl    = 0;                   // ...
////////lpFmtHeader->uMatRes     = 0;                   // ...

        // Create <aplChrNCols> FMTCOLSTRs in the output
        lpFmtColStr = (LPFMTCOLSTR) (&lpFmtHeader[1]);
        lpFmtHeader->lpFmtCol1st = lpFmtColStr;
        Assert (aplChrNCols EQ (APLU3264) aplChrNCols);
        ZeroMemory (lpFmtColStr, (APLU3264) aplChrNCols * sizeof (FMTCOLSTR));
#ifdef DEBUG
        {
            APLDIM uCol;

            for (uCol = 0; uCol < aplChrNCols; uCol++)
                lpFmtColStr[uCol].Sig.nature = FMTCOLSTR_SIGNATURE;
        }
#endif
        // Skip over the FMTCOLSTRs
        lpaplCharStart = (LPAPLCHAR) (&lpFmtColStr[aplChrNCols]);

        // Save ptr to 1st child FMTROWSTR
        if (aplDimNRows)
            lpFmtHeader->lpFmtRow1st = (LPFMTROWSTR) lpaplCharStart;
        else
            lpFmtHeader->lpFmtRow1st = NULL;

        // Loop through the array appending the formatted values (separated by WC_EOS)
        //   to the output vector, and accumulating the values in the appropriate
        //   FMTCOLSTR & FMTROWSTR entries.

        // Split cases based upon the array's storage type
        switch (aplType)
        {
            case ARRAY_BOOL:
////////////////lpaplChar =
                  CompileArrBool    ((LPAPLBOOL)    lpMem,  // Ptr to right arg memory
                                     lpFmtHeader,           // Ptr to parent header
                                     lpFmtColStr,           // Ptr to vector of ColStrs
                                     lpaplCharStart,        // Ptr to compiled output
                                     aplDimNRows,           // # rows
                                     aplDimNCols,           // # cols
                                     aplRank,               // Right arg rank
                                     lpMemDim,              // Ptr to right arg dimensions
                                     TRUE);                 // TRUE iff top level array
                break;

            case ARRAY_INT:
////////////////lpaplChar =
                  CompileArrInteger ((LPAPLINT)    lpMem,   // Ptr to right arg memory
                                     lpFmtHeader,           // Ptr to parent header
                                     lpFmtColStr,           // Ptr to vector of ColStrs
                                     lpaplCharStart,        // Ptr to compiled output
                                     aplDimNRows,           // # rows
                                     aplDimNCols,           // # cols
                                     aplRank,               // Right arg rank
                                     lpMemDim,              // Ptr to right arg dimensions
                                     TRUE);                 // TRUE iff top level array
                break;

            case ARRAY_FLOAT:
////////////////lpaplChar =
                  CompileArrFloat   ((LPAPLFLOAT)  lpMem,   // Ptr to right arg memory
                                     lpFmtHeader,           // Ptr to parent header
                                     lpFmtColStr,           // Ptr to vector of ColStrs
                                     lpaplCharStart,        // Ptr to compiled output
                                     aplDimNRows,           // # rows
                                     aplDimNCols,           // # cols
                                     aplRank,               // Right arg rank
                                     lpMemDim,              // Ptr to right arg dimensions
                                     TRUE);                 // TRUE iff top level array
                break;

            case ARRAY_APA:
////////////////lpaplChar =
                  CompileArrAPA     ((LPAPLAPA)    lpMem,   // Ptr to right arg memory
                                     lpFmtHeader,           // Ptr to parent header
                                     lpFmtColStr,           // Ptr to vector of ColStrs
                                     lpaplCharStart,        // Ptr to compiled output
                                     aplDimNRows,           // # rows
                                     aplDimNCols,           // # cols
                                     aplRank,               // Right arg rank
                                     lpMemDim,              // Ptr to right arg dimensions
                                     TRUE);                 // TRUE iff top level array
                break;

            case ARRAY_CHAR:
////////////////lpaplChar =
                  CompileArrChar    ((LPAPLCHAR)   lpMem,   // Ptr to right arg memory
                                     lpFmtHeader,           // Ptr to parent header
                                     lpFmtColStr,           // Ptr to vector of ColStrs
                                     lpaplCharStart,        // Ptr to compiled output
                                     aplDimNRows,           // # rows
                                     aplDimNCols,           // # cols
                                     aplRank,               // Right arg rank
                                     lpMemDim,              // Ptr to right arg dimensions
                                     TRUE);                 // TRUE iff top level array
                break;

            case ARRAY_HETERO:
////////////////lpaplChar =
                  CompileArrHetero  ((LPAPLHETERO) lpMem,   // Ptr to right arg memory
                                     lpFmtHeader,           // Ptr to parent header
                                     lpFmtColStr,           // Ptr to vector of ColStrs
                                     lpaplCharStart,        // Ptr to compiled output
                                     aplDimNRows,           // # rows
                                     aplDimNCols,           // # cols
                                     aplRank,               // Right arg rank
                                     lpMemDim,              // Ptr to right arg dimensions
                                     TRUE,                  // TRUE iff top level array
                                     TRUE);                 // TRUE iff handle []TCLF specially
                break;

            case ARRAY_NESTED:
////////////////lpaplChar =
                  CompileArrNested  ((LPAPLNESTED) lpMem,   // Ptr to right arg memory
                                     lpFmtHeader,           // Ptr to parent header
                                     lpFmtColStr,           // Ptr to vector of ColStrs
                                     lpaplCharStart,        // Ptr to compiled output
                                     aplDimNRows,           // # rows
                                     aplDimNCols,           // # cols
                                     aplRank,               // Right arg rank
                                     lpMemDim,              // Ptr to right arg dimensions
                                     TRUE);                 // TRUE iff top level array
                break;

            case ARRAY_RAT:
////////////////lpaplChar =
                  CompileArrRat     ((LPAPLRAT)    lpMem,   // Ptr to right arg memory
                                     lpFmtHeader,           // Ptr to parent header
                                     lpFmtColStr,           // Ptr to vector of ColStrs
                                     lpaplCharStart,        // Ptr to compiled output
                                     aplDimNRows,           // # rows
                                     aplDimNCols,           // # cols
                                     aplRank,               // Right arg rank
                                     lpMemDim,              // Ptr to right arg dimensions
                                     TRUE);                 // TRUE iff top level array
                break;

            case ARRAY_VFP:
////////////////lpaplChar =
                  CompileArrVfp     ((LPAPLVFP)    lpMem,   // Ptr to right arg memory
                                     lpFmtHeader,           // Ptr to parent header
                                     lpFmtColStr,           // Ptr to vector of ColStrs
                                     lpaplCharStart,        // Ptr to compiled output
                                     aplDimNRows,           // # rows
                                     aplDimNCols,           // # cols
                                     aplRank,               // Right arg rank
                                     lpMemDim,              // Ptr to right arg dimensions
                                     TRUE);                 // TRUE iff top level array
                break;

            defstop
                break;
        } // End SWITCH

        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Propagate the row & col count up the line
        PropagateRowColCount (lpFmtHeader);

        // lpaplCharStart now contains the compiled version of the output

        // Add up the width of each column to get the
        //   # cols in the result
        for (aplLastDim = aplDimCol = 0; aplDimCol < aplChrNCols; aplDimCol++)
            aplLastDim += ExteriorColWidth (&lpFmtColStr[aplDimCol]);
        Assert (aplLastDim EQ ExteriorHdrWidth (lpFmtHeader));

        // Calculate the NELM of the result
        aplNELMRes = lpFmtHeader->uFmtRows * aplLastDim;

        // Get ptr to formatting save area
        lpaplChar = lpwszFormat;

#ifdef PREFILL
        // Fill the output area with all zeros
        // Ensure at least one value filled in case the char array is empty
        if (IsSimpleChar (aplType))
            ZeroMemory (lpaplChar, (APLU3264) max (aplNELMRes, 1) * sizeof (APLCHAR));
        else
        {
            // Check for WS FULL
            if (aplNELMRes NE (APLU3264) aplNELMRes)
                goto WSFULL_EXIT;

            // Fill the output area with all blanks
            FillMemoryW (lpaplChar, (APLU3264) aplNELMRes, L' ');
        } // End IF/ELSE
#endif
        // Run through the array again processing the
        //   output stream into lpaplChar

        // Calc when to use raw output
        bRawOut = (!IsNested (aplType) && !IsSimpleHet (aplType) && !IsSimpleChar (aplType));

        // Split cases based upon the array's storage type
        switch (aplType)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_FLOAT:
            case ARRAY_CHAR:
            case ARRAY_APA:
            case ARRAY_HETERO:
            case ARRAY_RAT:
            case ARRAY_VFP:
////////////////lpaplChar =
                  FormatArrSimple (lpFmtHeader,             // Ptr to FMTHEADER
                                   lpFmtColStr,             // Ptr to vector of <aplChrNCols> FMTCOLSTRs
                                   lpaplCharStart,          // Ptr to compiled input
                                  &lpaplChar,               // Ptr to output string
                                   lpFmtHeader->uActRows,   // # formatted rows in this array
                                   aplDimNCols,             // # formatted cols in this array
                                   aplLastDim,              // Length of last dim in result
                                   aplRank,                 // Rank of this array
                                   lpMemDim,                // Ptr to this array's dimensions
                                   aplType,                 // Storage type of this array
                                   TRUE,                    // TRUE iff skip to next row after this item
                                   bRawOut,                 // TRUE iff raw (not {thorn}) output
                                   bEndingCR,               // TRUE iff last line has CR
                                   lpbCtrlBreak);           // Ptr to Ctrl-Break flag
                break;

            case ARRAY_NESTED:
////////////////lpaplChar =
                  FormatArrNested (lpFmtHeader,             // Ptr to FMTHEADER
                                   lpMem,                   // Ptr to raw input
                                   lpFmtColStr,             // Ptr to vector of <aplDimNCols> FMTCOLSTRs
                                   lpaplCharStart,          // Ptr to compiled input
                                  &lpaplChar,               // Ptr to ptr to output string
                                   lpFmtHeader->uActRows,   // # formatted rows in this array
                                   aplDimNCols,             // # formatted cols ...
                                   aplRank,                 // Rank of this array
                                   lpMemDim,                // Ptr to this array's dimensions
                                   aplLastDim,              // Length of last dim in result
                                   bRawOut,                 // TRUE iff raw (not {thorn}) output
                                   bEndingCR,               // TRUE iff last line has CR
                                   lpbCtrlBreak);           // Ptr to Ctrl-Break flag
                break;

            defstop
                break;
        } // End SWITCH

        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // If we didn't use raw output in the
        //   FormatArrxxx routines, do it now
        // The following code handles wrapping at []PW
        //   of groups of rows
        if (!bRawOut)
        {
            APLUINT     uColGrp,
                        uColLim;
            WCHAR       wcCur = L' ';       // The replaced WCHAR (start with anything non-zero)
            UINT        uFmtRow;            // Loop counter
            LPAPLCHAR   lpwsz;              // Temporary ptr
            LPWCHAR     lpwszTemp,          // Ptr to formatting temp area
                        lpwszOrigTemp;      // ...    original value ...
            LPROWPTRS   lpRowPtrs;
            VARS_TEMP_OPEN

            // Save the original ptr to restore later
            lpwszOrigTemp = lpMemPTD->lpwszTemp;
            CHECK_TEMP_OPEN
            EXIT_TEMP_OPEN

            // Reserve space for Nxt and End Char ptrs
            lpRowPtrs = (LPVOID) lpMemPTD->lpwszTemp;
            lpMemPTD->lpwszTemp += (sizeof (ROWPTRS) * lpFmtHeader->uFmtRows);

            // Save the data ptr for each row
            for (uFmtRow = 0,
                   lpwsz = lpwszFormat;
                 uFmtRow < lpFmtHeader->uFmtRows;
                 uFmtRow++)
            {
                lpRowPtrs[uFmtRow].lpNxtChar = lpwsz;
                lpwsz += aplLastDim;
                lpRowPtrs[uFmtRow].lpEndChar = lpwsz;
            } // End FOR

            // Get ptr to formatting temp area
            //   and protect by skipping over it
            //   taking into account tab stops
            lpwszTemp = lpMemPTD->lpwszTemp;
            lpMemPTD->lpwszTemp += (uTabStops * aplLastDim) + 1;    // "+ 1" for the terminating zero

            // Insert a terminating zero
            *lpaplChar = WC_EOS;

            // Calculate the # of cols per row group
            uColLim = (aplLastDim + uQuadPW - 1) / uQuadPW;

            // Loop through the groups of cols
            for (uColGrp = 0; uColGrp < uColLim; uColGrp++, bLineCont = TRUE)
            {
                // Loop through the formatted rows
                for (uFmtRow = 0;
                     uFmtRow < lpFmtHeader->uFmtRows;
                     uFmtRow++)
                {
                    APLUINT uColCur,            // ...
                            uTmp,               // Temporary
                            uMaxWidth,          // Maximum col width
                            uCurPos;            // Current col position

                    // ***FIXME*** -- this routine may split a number in half
                    //                because it doesn't know the difference
                    //                between numbers and characters

                    // Set start and end of input
                    lpwszNxt = lpRowPtrs[uFmtRow].lpNxtChar;
                    lpwszEnd = lpRowPtrs[uFmtRow].lpEndChar;

                    if (lpwszNxt >= lpwszEnd)
                        break;

                    if (uColGrp NE 0)
                    {
                        lstrcpyW (lpwszTemp, wszIndent);                // Copy the indent
                        uOutLen = uQuadPW - DEF_INDENT;                 // Maximum output length
                        uCurPos = DEF_INDENT;
                    } else
                    {
                        uOutLen = uQuadPW;                              // Maximum output length
                        uCurPos = 0;
                    } // End IF/ELSE

                    // Set the current maximum width
                    uMaxWidth = uCurPos;

                    // Loop through this part of the output line
                    for (uColCur = 0; (uColCur < uOutLen) && lpwszNxt < lpwszEnd; uColCur++)
                    {
                        // Get the next char
                        wcCur = *lpwszNxt++;

                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // If the char is not printable, ...
                        if (wcCur < L' ')
                            uColCur--;      // Count it out

                        // Split cases based upon the character
                        switch (wcCur)
                        {
                            case TCHT:          // []TCHT -- Move ahead to the next tab stop
                                // *Insert* enough blanks to get to the next tabstop
                                uTmp = uTabStops - (uCurPos % uTabStops);
                                MoveMemoryW (&lpwszTemp[uCurPos + uTmp], &lpwszTemp[uCurPos], (APLU3264) uTmp);
                                FillMemoryW (&lpwszTemp[uCurPos], (APLU3264) uTmp, L' ');

                                // Increase the maximum width and current position by the # inserted blanks
                                uMaxWidth += uTmp;
                                uCurPos   += uTmp;

                                break;

                            case TCBEL:         // []TCBEL -- Sound the alarum!
                                MessageBeep (NEG1U);

                                break;

                            case TCBS:          // []TCBS -- Backspace if there's room
                                if (uCurPos)
                                    uCurPos--;
                                break;

                            case TCNL:          // []TCNL -- Restart at the beginning of a new line
                                // Zap the temp buffer at the maximum width
                                lpwszTemp[uMaxWidth] = WC_EOS;

                                // Output the line up to this point w/NL
                                AppendLine (lpwszTemp, FALSE, TRUE);

                                // Reset the maximum width and current position
                                uMaxWidth =
                                uCurPos   =
                                uColCur   = 0;
                                uColCur--;              // Count it out
                                uOutLen = uQuadPW;      // Maximum output length

                                break;

                            case TCLF:          // []TCLF -- Start a new line
                                // Zap the temp buffer at the maximum width
                                lpwszTemp[uMaxWidth] = WC_EOS;

                                // Output the line up to this point w/NL
                                AppendLine (lpwszTemp, FALSE, TRUE);

                                // Output enough blanks to get back to the current position
                                FillMemoryW (lpwszTemp, (APLU3264) uCurPos, L' ');

                                break;

                            default:
                                // If the char is printable, ...
                                if (wcCur >= L' ')
                                {
                                    // Save the new char
                                    lpwszTemp[uCurPos++] = wcCur;
                                    uMaxWidth = max (uMaxWidth, uCurPos);
                                } // End IF/ELSE

                                break;
                        } // End SWITCH
                    } // End FOR

                    // Save ptr to next input
                    lpRowPtrs[uFmtRow].lpNxtChar = lpwszNxt;

                    // Zap the temp buffer at the maximum width
                    lpwszTemp[uMaxWidth] = WC_EOS;          // Zap it

                    // If we're at the last line, ...
                    if (uColGrp EQ (uColLim - 1)
                     && uFmtRow EQ (lpFmtHeader->uFmtRows - 1))
                        AppendLine (lpwszTemp, bLineCont, bEndingCR); // Display the line
                    else
                        AppendLine (lpwszTemp, bLineCont, TRUE);      // Display the line
                } // End FOR

                // If we're not at the last line, and
                //   the array is multirank, ...
                // ***FIXME*** -- Sometimes on matrices whose lines contain TCNLs at low []PW,
                //                we may display extra blank line(s) at the end because we
                //                use up uColGrps faster than does the FOR loop.
                if (uColGrp NE (uColLim - 1)
                 && IsMultiRank (aplRank))
                    // Display a blank separator line
                    AppendLine (L"", FALSE, TRUE);
            } // End FOR

            // Restore original ptr to formatting temp area
            lpMemPTD->lpwszTemp = lpwszOrigTemp;
        } // End IF

        // If this is an empty vector, make sure it skips a line
        if (IsEmpty (lpFmtHeader->uFmtIntWid)
         && IsVector (aplRank)
         && !bRawOut)
            AppendLine (L"", FALSE, TRUE);// Display the empty line
    } __except (CheckVirtAlloc (GetExceptionInformation (),
                                L"DisplayGlbArr_EM"))
    {
        // Split cases based upon the exception code
        switch (MyGetExceptionCode ())
        {
            case EXCEPTION_LIMIT_ERROR:
                goto LIMIT_EXIT;

            defstop
                break;
        } // End SWITCH
    } // End __try/__except
EMPTY_EXIT:
    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LIMIT_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LIMIT_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    // Restore previous heapFull
    *heapFull = *oldHeapFull;

    // If we allocated virtual storage, ...
    if (lclMemVirtStr[0].IniAddr)
    {
        // Free the virtual storage
        MyVirtualFree (lclMemVirtStr[0].IniAddr, 0, MEM_RELEASE); lclMemVirtStr[0].IniAddr = NULL;
        lpaplCharIni = NULL;

        // Unlink this struc from the chain
        UnlinkMVS (&lclMemVirtStr[0]);
    } // End IF

    if (hGlb && lpMem)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlb); lpMem = NULL;
    } // End IF

    return bRet;
} // End DisplayGlbArr_EM
#undef  APPEND_NAME


//***************************************************************************
//  $FormatImmed
//
//  Format an immediate constant
//***************************************************************************

LPAPLCHAR FormatImmed
    (LPWCHAR      lpaplChar,
     UINT         ImmType,
     LPAPLLONGEST lpaplLongest)

{
    LPVARARRAY_HEADER lpMemHdr;
    LPVOID            lpMemGlbNum;

    // Split cases based upon the immediate type
    switch (ImmType)
    {
        case IMMTYPE_BOOL:
            lpaplChar =
              FormatAplint (lpaplChar,                          // Ptr to output save area
                            BIT0 & *(LPAPLBOOL) lpaplLongest);  // The value to format
            break;

        case IMMTYPE_INT:
            lpaplChar =
              FormatAplint (lpaplChar,                          // Ptr to output save area
                            *(LPAPLINT) lpaplLongest);          // The value to format
            break;

        case IMMTYPE_CHAR:
            *lpaplChar++ = *(LPAPLCHAR) lpaplLongest;
            *lpaplChar++ = L' ';    // Append a blank to be deleted

            break;

        case IMMTYPE_FLOAT:
            lpaplChar =
              FormatFloat (lpaplChar,                 // Ptr to output save area
                           *(LPAPLFLOAT) lpaplLongest,// The value to format
                           0);                        // Use default significant digits
            break;

        case IMMTYPE_RAT:
            lpaplChar =
              FormatAplRat (lpaplChar,      // Ptr to output save area
                *(LPAPLRAT) VarArrayDataFmBase (ClrPtrTypeDir (lpaplLongest))); // The value to format
            break;

        case IMMTYPE_VFP:
            lpaplChar =
              FormatAplVfp (lpaplChar,      // Ptr to output save area
                *(LPAPLVFP) VarArrayDataFmBase (ClrPtrTypeDir (lpaplLongest)),  // The value to format
                            GetQuadPPV ()); // Use this many significant digits for VFP
            break;

        case IMMTYPE_ERROR:
            Assert (GetPtrTypeDir (lpaplLongest) EQ PTRTYPE_HGLOBAL);

            // Lock the memory to get a ptr to it
            lpMemHdr = MyGlobalLock (lpaplLongest);

            // Skip over the header and dimensions to the data
            lpMemGlbNum = VarArrayDataFmBase (lpMemHdr);

            // Split cases based upon the storage type
            switch (lpMemHdr->ArrType)
            {
                case ARRAY_RAT:
                    lpaplChar =
                      FormatAplRat (lpaplChar,      // Ptr to output save area
                        *(LPAPLRAT) lpMemGlbNum);   // The value to format
                    break;

                case ARRAY_VFP:
                    lpaplChar =
                      FormatAplVfp (lpaplChar,      // Ptr to output save area
                        *(LPAPLVFP) lpMemGlbNum,    // The value to format
                                    GetQuadPPV ()); // Use this many significant digits for VFP
                    break;

                defstop
                    break;
            } // End SWITCH

            // We no longer need this ptr
            MyGlobalUnlock (lpaplLongest); lpMemHdr = NULL; lpMemGlbNum = NULL;

            break;

        defstop
            break;
    } // End SWITCH

    return lpaplChar;
} // End FormatImmed


//***************************************************************************
//  $FormatImmedFC
//
//  Format an immediate constant using []FC
//***************************************************************************

LPAPLCHAR FormatImmedFC
    (LPWCHAR      lpaplChar,        // Ptr to input string
     UINT         ImmType,          // Immediate type
     LPAPLLONGEST lpaplLongest,     // Ptr to value to format
     APLUINT      nDigits,          // # significant digits
     APLCHAR      aplCharDecimal,   // Char to use as decimal separator
     APLCHAR      aplCharOverbar,   // Char to use as overbar
     FLTDISPFMT   fltDispFmt,       // Float display format
     UBOOL        bSubstInf)        // TRUE iff we're to substitute text for infinity

{
    WCHAR wc;

    // Split cases based upon the immediate type
    switch (ImmType)
    {
        case IMMTYPE_BOOL:
            lpaplChar =
              FormatAplintFC (lpaplChar,                        // Ptr to output save area
                              BIT0 & *(LPAPLBOOL) lpaplLongest, // The value to format
                              aplCharOverbar);                  // Char to use as overbar
            break;

        case IMMTYPE_INT:
            lpaplChar =
              FormatAplintFC (lpaplChar,                        // Ptr to output save area
                              *(LPAPLINT) lpaplLongest,         // The value to format
                              aplCharOverbar);                  // Char to use as overbar
            break;

        case IMMTYPE_CHAR:
            // Get the char
            wc = *(LPAPLCHAR) lpaplLongest;

            // Test for Terminal Control chars
            switch (wc)
            {
                case TCBEL:     // Bel
                    MessageBeep (NEG1U);    // Sound the alarum!

                    // Fall through to common code

                case TCESC:     // Esc
                case TCFF:      // FF
                case TCNUL:     // NUL
                case TCBS:      // BS
                case TCNL:      // NL
                    *lpaplChar++ = L' ';    // Append a blank to be deleted

                    break;          // Can't go any farther left

                case TCHT:      // HT
                    // We're always at the (virtual) left margin,
                    //   so insert enough blanks for a TAB
                    lpaplChar = FillMemoryW (lpaplChar, uTabStops + 1, L' ');
///////////////////*lpaplChar++ = L' ';     // Append a blank to be deleted

                    break;

                case TCLF:      // LF       // Handled during raw output
                    *lpaplChar++ = wc;      // Append it
                    *lpaplChar++ = L' ';    // Append a blank to be deleted

                    break;

                default:
                    if (wc >= 32)           // If none of the above but printable,
                        *lpaplChar++ = wc;  //   append it

                    *lpaplChar++ = L' ';    // Append a blank to be deleted

                    break;
            } // End SWITCH

            break;

        case IMMTYPE_FLOAT:
            lpaplChar =
              FormatFloatFC (lpaplChar,                 // Ptr to output save area
                             *(LPAPLFLOAT) lpaplLongest,// The value to format
                             nDigits,                   // # significant digits
                             aplCharDecimal,            // Char to use as decimal separator
                             aplCharOverbar,            // Char to use as overbar
                             fltDispFmt,                // Float display format
                             bSubstInf);                // TRUE iff we're to substitute text for infinity
            break;

        defstop
            break;
    } // End SWITCH

    return lpaplChar;
} // End FormatImmedFC


//***************************************************************************
//  $FormatAplint
//
//  Format an APLINT
//***************************************************************************

LPAPLCHAR FormatAplint
    (LPAPLCHAR lpaplChar,           // Ptr to output save area
     APLINT    aplInt)              // Integer to format

{
    return FormatAplintFC (lpaplChar,       // Ptr to output save area
                           aplInt,          // The value to format
                           UTF16_OVERBAR);  // Char to use as overbar
} // End FormatAplint


//***************************************************************************
//  $FormatAplintFC
//
//  Format an APLINT using []FC
//***************************************************************************

LPAPLCHAR FormatAplintFC
    (LPAPLCHAR lpaplChar,           // Ptr to output save area
     APLINT    aplInt,              // Integer to format
     APLCHAR   aplCharOverbar)      // Char to use as overbar

{

#define MAXLEN  32

    WCHAR wszTemp[MAXLEN + 1];
    int   i;
    UINT  u;

    // Check the sign bit -- if set, save and make positive
    if (aplInt < 0)
    {
        // Save the overbar
        *lpaplChar++ = aplCharOverbar;

        // If it's the minimum integer, ...
        if (aplInt EQ MIN_APLINT)
        {
            // Copy the constant (skipping over the sign WCHAR) to the result
            lstrcpyW (lpaplChar, &MIN_APLINT_STR[1]);

            // Skip over it
            lpaplChar += lstrlenW (lpaplChar);

            goto NORMAL_EXIT;
        } // End IF

        // Now it's safe to negate the integer
        aplInt = -aplInt;
    } // End IF

    // Format the number one digit at a time
    //   in reverse order into lpaplChar
    i = MAXLEN - 1;

    // Execute at least once so zero prints
    do
    {
        u = (UINT) (aplInt % 10);
        wszTemp[i--] = u + L'0';
        aplInt = aplInt / 10;
    } while (aplInt);

    // Copy the digits to the result
    for (i++; i < MAXLEN; i++)
        *lpaplChar++ = wszTemp[i];
NORMAL_EXIT:
    // Include a separator
    *lpaplChar++ = L' ';

    return lpaplChar;
} // End FormatAplintFC


//***************************************************************************
//  $FormatAplRat
//
//  Format an APLRAT
//***************************************************************************

LPAPLCHAR FormatAplRat
    (LPAPLCHAR lpaplChar,           // Ptr to output save area
     APLRAT    aplRat)              // The value to format

{
    return FormatAplRatFC (lpaplChar,       // Ptr to output save area
                           aplRat,          // The value to format
                           UTF16_OVERBAR,   // Char to use as overbar
                           DEF_RATSEP,      // Char to use as rational separator
                           FALSE);          // TRUE iff we're to substitute text for infinity
} // End FormatAplRat


//***************************************************************************
//  $FormatAplRatFC
//
//  Format an APLRAT using []FC
//***************************************************************************

LPAPLCHAR FormatAplRatFC
    (LPAPLCHAR lpaplChar,           // Ptr to output save area
     APLRAT    aplRat,              // The value to format
     APLCHAR   aplCharOverbar,      // Char to use as overbar
     APLCHAR   aplCharRatSep,       // Char to use as rational separator
     UBOOL     bSubstInf)           // TRUE iff we're to substitute text for infinity

{
    APLINT    iLen,                 // String length
              iRes;                 // Loop counter
    LPAPLCHAR lpw;                  // Ptr to string
    UBOOL     bNeg;                 // TRUE iff the number is negative
    LPCHAR    lpRawFmt;             // Ptr to raw formatted #
    APLUINT   uTotSize;             // Total size of formatted #

    lpRawFmt = (LPCHAR) lpaplChar;

    // If the number is not +/- infinity, ...
    if (!mpq_inf_p (&aplRat))
    {
        // If the denominator is NE 1, ...
        if (mpz_cmp_ui (mpq_denref (&aplRat), 1) NE 0)
            // Canonicalize the arg
            mpq_canonicalize (&aplRat);

        // Calculate the needed size, where both "1 +"s include the sign,
        //   the first "+ 1" includes the 'r' separator and the second
        //   one the terminating zero
        uTotSize = 1 + mpz_sizeinbase (mpq_numref (&aplRat), 10) + 1 +
                   1 + mpz_sizeinbase (mpq_denref (&aplRat), 10) + 1;
        // Double as we convert in place
        uTotSize = 2 * uTotSize;

        if (uTotSize NE (APLU3264) uTotSize)
            uTotSize = UINT_MAX;

        // Test the output save area limits
        ZeroMemory (lpRawFmt, (APLU3264) uTotSize);
    } // End IF

    // Format the num/den
    mpq_get_str (lpRawFmt, 10, &aplRat);

    // Convert a leading minus to aplCharOverbar
    bNeg = (lpRawFmt[0] EQ '-');

    // Get the char length
    iLen = lstrlen (lpRawFmt);

    // Convert the char to WCHAR
    for (iRes = iLen; iRes >= 0; iRes--)
        lpaplChar[iRes] = lpRawFmt[iRes];

    // Convert a leading minus to aplCharOverbar
    if (bNeg)
    {
        *lpaplChar++ = aplCharOverbar;
        iLen--;
    } // End IF

    // Check for Infinity
    if (lpaplChar[0] EQ DEF_POSINFINITY_CHAR)
    {
        // If we're to substitute, ...
        if (bSubstInf)
            lpaplChar +=
              ConvertWideToNameLength (lpaplChar,   // Ptr to output save buffer
                                      &wcInf,       // Ptr to incoming chars
                                       1);          // # chars to convert
        else
            *lpaplChar++ = UTF16_INFINITY;
    } else
    {
        // Convert '/' (if any) to aplCharRatSep
        lpw = strchrW (lpaplChar, L'/');
        if (lpw)
            *lpw = aplCharRatSep;

        // Skip over the formatted number
        lpaplChar += iLen;
    } // End IF/ELSE

    // Include a separator
    *lpaplChar++ = L' ';

    return lpaplChar;
} // End FormatAplRatFC


//***************************************************************************
//  $FormatFloat
//
//  Format a floating point number
//***************************************************************************

LPAPLCHAR FormatFloat
    (LPWCHAR  lpaplChar,        // Ptr to output save area
     APLFLOAT aplFloat,         // The value to format
     APLUINT  nDigits)          // Raw or E-format:  # significant digits
                                // F-format:  # digits to right of decimal sep

{
    return FormatFloatFC (lpaplChar,                // Ptr to output save area
                          aplFloat,                 // The value to format
                          nDigits,                  // # significant digits (0 = default)
                          L'.',                     // Char to use as decimal separator
                          UTF16_OVERBAR,            // Char to use as overbar
                          FLTDISPFMT_RAWFLT,        // Float display format
                          FALSE);                   // TRUE iff we're to substitute text for infinity
} // End FormatFloat


//***************************************************************************
//  $FormatFloatFC
//
//  Format a APLFLOAT using []FC
//***************************************************************************

LPAPLCHAR FormatFloatFC
    (LPWCHAR    lpaplChar,          // Ptr to output save area
     APLFLOAT   aplFloat,           // The value to format
     APLINT     nDigits,            // Raw or E-format:  # significant digits
                                    // F-format:  # digits to right of decimal sep
     APLCHAR    aplCharDecimal,     // Char to use as decimal separator
     APLCHAR    aplCharOverbar,     // Char to use as overbar
     FLTDISPFMT fltDispFmt,         // Float display format (see FLTDISPFMT)
     UBOOL      bSubstInf)          // TRUE iff we're to substitute text for infinity

{
    if (IsInfinity (aplFloat))
    {
        if (aplFloat < 0)
            *lpaplChar++ = aplCharOverbar;

        // If we're to substitute, ...
        if (bSubstInf)
            lpaplChar +=
              ConvertWideToNameLength (lpaplChar,       // Ptr to output save buffer
                                      &wcInf,           // Ptr to incoming chars
                                        1);             // # chars to convert
        else
            *lpaplChar++ = wcInf;           // Char for infinity
    } else
    if (aplFloat EQ 0)
    {
        // Split cases based upon the float display format
        switch (fltDispFmt)
        {
            case FLTDISPFMT_E:          // 0E0 or 0.0E0 or 0.0---0E0
                if (nDigits EQ 1)
                {
                    // Fill in the result of "0E0"
                    *lpaplChar++ = L'0';
                    *lpaplChar++ = DEF_EXPONENT_UC;
                    *lpaplChar++ = L'0';
                } else
                if (nDigits > 1)
                {
                    // Start with "0."
                    *lpaplChar++ = L'0';
                    nDigits--;
                    *lpaplChar++ = aplCharDecimal;

                    // While there are more digits, ...
                    while (nDigits > 0)
                    {
                        // Fill with leading zeros
                        *lpaplChar++ = L'0';
                        nDigits--;
                    } // End WHILE

                    // End with "E0"
                    *lpaplChar++ = DEF_EXPONENT_UC;
                    *lpaplChar++ = L'0';
                } else
                    DbgStop ();         // We should never get here
                break;

            case FLTDISPFMT_F:
                // Start with a "0"
                *lpaplChar++ = L'0';

                // If there are any significant digits, ...
                if (nDigits > 0)
                {
                    // Append a decimal point
                    *lpaplChar++ = aplCharDecimal;

                    // While there are more significant digits, ...
                    while (nDigits > 0)
                    {
                        // Append another zero
                        *lpaplChar++ = L'0';
                        nDigits--;
                    } // End WHILE
                } // End IF

                break;

            case FLTDISPFMT_RAWINT:
            case FLTDISPFMT_RAWFLT:
                // Display signed zero so that 6413 []DR {neg}64{take}1
                //   displays with a sign
                if (SIGN_APLFLOAT (aplFloat))
                    *lpaplChar++ = aplCharOverbar;
                // The result is "0"
                *lpaplChar++ = L'0';

                break;

            defstop
                break;
        } // End SWITCH
    } else
    // Non-zero
    {
        DTOAMODE dtoaMode;              // DTOA mode corresponding to fltDispFmt
        LPCHAR   s, s0;                 // Ptr to output from dtoa
        int      decpt,                 // Exponent from dtoa
                 sign;                  // TRUE iff the number is negative
        APLINT   iSigDig;               // # significant digits
        UBOOL    bPowerOfTwo;           // TRUE iff the # is a power of two
        LPWCHAR  lpaplCharIni;          // Initial ptr to output save area

        // Get the corresponding DTOA mode
        // DTOAMODE_SHORT_RND = 0,              // 0 = shortest string with rounding, e.g., 1e23
        // DTOAMODE_SHORT_NORND,                // 1 = shortest string without rounding. e.g., 9.999999999999999e22
        // DTOAMODE_SIGDIGS,                    // 2 = # significant digits
        // DTOAMODE_FRACTDIGS,                  // 3 = # fractional digits (past decimal point)

        // Save the initial ptr to the output save area
        lpaplCharIni = lpaplChar;

        // If this is raw float formatting, ...
        if (nDigits EQ 0 && fltDispFmt EQ FLTDISPFMT_RAWFLT)
            // Use the current value of []PP
            nDigits  = GetQuadPP ();
        // Get the corresponding DTOAMODE_xxx
        dtoaMode = gDTOA_Mode[fltDispFmt];

        // If this is RAWINT, ...
        if (fltDispFmt EQ FLTDISPFMT_RAWINT)
        {
            APLFLOATUNION aplFloatUnion;        // Temporary union

            // Save the FP value in the union so we can pick it apart
            aplFloatUnion.aplFloat = aplFloat;

            // Determine if the value is a power of two
            bPowerOfTwo = (aplFloatUnion.aplFloatStr.uMantissa EQ 0);

            nDigits = 0;                    // No digits (integral)
            dtoaMode = DTOAMODE_FRACTDIGS;  // 3 = nDigits past decimal point

            // Round aplFloat to the nearest integer
            //   taking into account the sign (which floor doesn't)
            if (aplFloat >= 0)
                aplFloat =  floor (0.5 + aplFloat);
            else
                aplFloat = -floor (0.5 - aplFloat);
            // Handle signed zero
            if (aplFloat EQ 0)
                aplFloat = 0;
        } // End IF

        // Convert aplFloat to an ASCII string
        // Use David Gay's routines
        s = s0 = dtoa (aplFloat, dtoaMode, (UINT) nDigits, &decpt, &sign, NULL);

        // If we're displaying short round, ensure that we display no more digits
        //   than nDigits.
        if (fltDispFmt EQ FLTDISPFMT_RAWFLT
         && dtoaMode EQ DTOAMODE_SHORT_RND
         && nDigits NE 0
         && nDigits < lstrlen (s))
        {
            // Free the temporary storage
            freedtoa (s0);

            // Try again with SIGDIGS
            dtoaMode = DTOAMODE_SIGDIGS;        // Display up to []PP digits
            s = s0 = dtoa (aplFloat, dtoaMode, (UINT) nDigits, &decpt, &sign, NULL);
        } // End IF

        // Handle the sign
        if (sign)
            *lpaplChar++ = aplCharOverbar;

        // Split cases based upon the float display format
        switch (fltDispFmt)
        {
            case FLTDISPFMT_E:
                // Format the number with exactly nDigits significant digits
                lpaplChar =
                  FormatExpFmt (lpaplChar,          // Ptr to output save area
                         (int) -nDigits,            // # significant digits
                                s,                  // Ptr to raw formatted number
                                decpt,              // Exponent
                                DEF_MAX_QUADPP64,   // Maximum # significant digits
                                aplCharDecimal,     // Char to use as decimal separator
                                aplCharOverbar);    // Char to use as overbar
                break;

            case FLTDISPFMT_F:
                // Format the number with no more than nDigits
                //   to the right of the decimal point

                // Get the maximum # significant digits
                iSigDig = DEF_MAX_QUADPP64;

                // Handle numbers between 0 and 1
                if (decpt <= 0)
                {
                    // Start with "0."
                    *lpaplChar++ = L'0';
                    *lpaplChar++ = aplCharDecimal;

                    // Then a bunch of 0s
                    while (nDigits > 0
                        && decpt < 0)
                    {
                        *lpaplChar++ = L'0';
                        nDigits--;
                        decpt++;
                    } // End WHILE

                    // If there are no more significant digits
                    //   and the sign is negative, ...
                    if (nDigits EQ 0
                     && sign)
                        // Move the number down over the sign so we don't return {neg}0
                        CopyMemoryW (lpaplCharIni, &lpaplCharIni[1], lpaplChar-- - lpaplCharIni);
                    else
                        // Copy the remaining digits (or underflow chars) to the result
                        //   converting from one-byte ASCII to two-byte UTF16
                        while (nDigits > 0
                            && *s)
                        {
                            *lpaplChar++ = (iSigDig > 0) ? *s++ : DEF_UNDERFLOW;
                            nDigits--;
                            iSigDig--;
                        } // End WHILE

                        // End with zeros or underflow chars for nDigits
                        while (nDigits > 0)
                        {
                            *lpaplChar++ = (iSigDig > 0) ? L'0' : DEF_UNDERFLOW;
                            nDigits--;
                            iSigDig--;
                        } // End WHILE
                } else
                {
                    // Copy no more than decpt digits to the result
                    //   converting from one-byte ASCII to two-byte UTF16
                    while (min (decpt, iSigDig) > 0
                        && *s)
                    {
                        *lpaplChar++ = *s++;
                        decpt--;
                        iSigDig--;
                    } // End WHILE

                    // If there are still more digits in the exponent, ...
                    if (decpt > 0)
                    {
                        // Fill with trailing zeros or underflow chars
                        while (decpt > 0)
                        {
                            *lpaplChar++ = (iSigDig > 0) ? L'0' : DEF_UNDERFLOW;
                            decpt--;
                            iSigDig--;
                        } // End WHILE

                        // Next, the decimal point
                        *lpaplChar++ = aplCharDecimal;

                        // End with zeros or underflow chars for nDigits
                        while (nDigits > 0)
                        {
                            *lpaplChar++ = (iSigDig > 0) ? L'0' : DEF_UNDERFLOW;
                            nDigits--;
                            iSigDig--;
                        } // End WHILE
                    } else
                    {
                        // Next, the decimal point
                        *lpaplChar++ = aplCharDecimal;

                        // Fill with trailing digits or underflow chars
                        while (nDigits > 0
                            && *s)
                        {
                            *lpaplChar++ = (iSigDig > 0) ? *s++ : DEF_UNDERFLOW;
                            nDigits--;
                            iSigDig--;
                        } // End WHILE

                        // End with zeros or underflow chars for nDigits
                        while (nDigits > 0)
                        {
                            *lpaplChar++ = (iSigDig > 0) ? L'0' : DEF_UNDERFLOW;
                            nDigits--;
                            iSigDig--;
                        } // End WHILE
                    } // End IF/ELSE
                } // End IF/ELSE

                break;

            case FLTDISPFMT_RAWINT:
                // If the number is a power of two, ...
                if (bPowerOfTwo)
                    // Copy the remaining digits to the result
                    //   converting from one-byte ASCII to two-byte UTF16
                    while (*s)
                        *lpaplChar++ = *s++;
                else
                {
                    APLINT nSigDig;             // # significant digits

                    // Copy no more than nDigits

                    // No more than DEF_MAX_QUADPP64 digits
                    if (nDigits)
                        nSigDig = min (nDigits, DEF_MAX_QUADPP64);
                    else
                    {
                        nSigDig = DEF_MAX_QUADPP64;
                        nDigits = decpt;
                    } // End IF/ELSE

                    // Loop through the digits
                    while (nSigDig > 0
                        && decpt > 0
                        && *s)
                    {
                        // Copy the remaining digits to the result
                        //   converting from one-byte ASCII to two-byte UTF16
                        *lpaplChar++ = *s++;
                        nSigDig--;
                        nDigits--;
                        decpt--;
                    } // End WHILE

                    // If there are more digits in the exponent, ...
                    while (nSigDig > 0
                        && decpt > 0)
                    {
                        // Fill with trailing zeros
                        *lpaplChar++ = L'0';
                        nSigDig--;
                        nDigits--;
                        decpt--;
                    } // End WHILE

                    // If there are more digits in the result, ...
                    while (nDigits > 0)
                    {
                        // Fill with trailing underflow chars
                        *lpaplChar++ = DEF_UNDERFLOW;
                        nDigits--;
                    } // End WHILE
                } // End IF/ELSE

                break;

            case FLTDISPFMT_RAWFLT:
                // Format the number wth no more than nDigits significant digits

                // If we're formatting as default, ...
                if (nDigits EQ 0)
                    // Get the default precision
                    nDigits = GetQuadPP ();

                // If (decpt > nDigits) or (decpt < -5), switch to E-format
                if (decpt > nDigits
                 || decpt < -5)
                    // Format the number with no more than nDigits significant digits
                    lpaplChar =
                      FormatExpFmt (lpaplChar,          // Ptr to output save area
                              (int) nDigits,            // # significant digits
                                    s,                  // Ptr to raw formatted number
                                    decpt,              // Exponent
                                    DEF_MAX_QUADPP64,   // Maximum # significant digits
                                    aplCharDecimal,     // Char to use as decimal separator
                                    aplCharOverbar);    // Char to use as overbar
                else
                // Handle numbers between 0 and 1
                if (decpt <= 0)
                {
                    // Start with "0."
                    *lpaplChar++ = L'0';
                    *lpaplChar++ = aplCharDecimal;

                    // Then a bunch of 0s
                    while (decpt < 0)
                    {
                        *lpaplChar++ = L'0';
                        decpt++;
                    } // End WHILE

                    // Copy the remaining digits to the result
                    //   converting from one-byte ASCII to two-byte UTF16
                    while (*s)
                        *lpaplChar++ = *s++;
                } else
                {
                    // Copy no more than nDigits digits to the result
                    //   converting from one-byte ASCII to two-byte UTF16
                    while (nDigits > 0
                        && decpt > 0
                        && *s)
                    {
                        *lpaplChar++ = *s++;
                        nDigits--;
                        decpt--;
                    } // End WHILE

                    // If there are still more digits in the exponent, ...
                    if (decpt > 0)
                        // Fill with trailing zeros
                        while (nDigits > 0
                            && decpt > 0)
                        {
                            *lpaplChar++ = L'0';
                            nDigits--;
                            decpt--;
                        } // End WHILE
                    else
                    {
                        // If there are more digits in the result, ...
                        if (nDigits > 0 && *s)
                        {
                            // Next, the decimal point
                            *lpaplChar++ = aplCharDecimal;

                            // Fill with trailing digits
                            while (nDigits > 0 && *s)
                            {
                                *lpaplChar++ = *s++;
                                nDigits--;
                            } // End WHILE
                        } // End IF
                    } // End IF/ELSE
                } // End IF/ELSE

                break;

            defstop
                break;
        } // End SWITCH

        // Free the temporary storage
        freedtoa (s0);
    } // End IF/ELSE/...

    // Append a separator
    *lpaplChar++ = L' ';

    return lpaplChar;
} // End FormatFloatFC


//***************************************************************************
//  $FormatExpFmt
//
//  Format a number in exponential format
//***************************************************************************

LPAPLCHAR FormatExpFmt
    (LPAPLCHAR lpaplChar,                   // Ptr to output save area
     int       nDigits,                     // # significant digits
     LPCHAR    s,                           // Ptr to raw formatted number
     int       decpt,                       // Exponent
     APLINT    iSigDig,                     // # significant digits
     APLCHAR   aplCharDecimal,              // Char to use as decimal separator
     APLCHAR   aplCharOverbar)              // Char to use as overbar

{
    UBOOL     bExact;                       // TRUE iff to be formatted with exactly
                                            // nDigits significant digits

    // Check for exactly nDigits significant digits
    bExact = (nDigits < 0);

    // If so, make positive
    if (bExact)
        nDigits = -nDigits;

    // If the number is negative, ...
    if (s[0] EQ '-')
    {
        // Copy it to the result
        *lpaplChar++ = aplCharOverbar;

        // Skip over it
        s++;
    } // End IF

    // If there's only one significant digit, ...
    if (nDigits EQ 1)
    {
        WCHAR wch;

        // Copy the first digit
        wch = *s;

        // If there's a next digit and we should round up, ...
        if (s[1] && s[1] >= L'5')
        {
            // If the first digit is not at the top, ...
            if (wch NE L'9')
                // Round up
                wch++;
            else
                // Otherwise, wrap to '1'
                wch = L'1';
        } // End IF

        // Save the one and only digit
        *lpaplChar++ = wch;
    } else
    {
        // Copy the first digit
        *lpaplChar++ = *s++;
        nDigits--;
        iSigDig--;

        // If there are more digits or we're being exact
        if (*s || bExact)
            // Then the decimal separator
            *lpaplChar++ = aplCharDecimal;

        // If there are more digits, ...
        if (*s)
        // Then the next nDigits digits
        while (nDigits > 0
            && *s)
        {
            *lpaplChar++ = (iSigDig > 0) ? *s++ : DEF_UNDERFLOW;
            nDigits--;
            iSigDig--;
        } // End IF/WHILE

        // If we're being exact, ...
        if (bExact)
        // While there are more digits, ...
        while (nDigits > 0)
        {
            // Fill with trailing zeros or underflow chars
            *lpaplChar++ = (iSigDig > 0) ? L'0' : DEF_UNDERFLOW;
            nDigits--;
            iSigDig--;
        } // End WHILE
    } // End IF/ELSE

    // Then the exponent separator
    *lpaplChar++ = DEF_EXPONENT_UC;

    // Finally the exponent
    lpaplChar =
      FormatAplintFC (lpaplChar,        // Ptr to output save area
                      decpt - 1,        // The value to format
                      aplCharOverbar);  // Char to use as overbar
    // Ensure properly terminated
    *--lpaplChar = WC_EOS;

    return lpaplChar;
} // End FormatExpFmt


//***************************************************************************
//  $FormatAplVfp
//
//  Format a variable-precision floating point number
//***************************************************************************

LPAPLCHAR FormatAplVfp
    (LPWCHAR  lpaplChar,        // Ptr to output save area
     APLVFP   aplVfp,           // The value to format
     APLUINT  nDigits)          // # significant digits (0 = all)

{
    return FormatAplVfpFC (lpaplChar,               // Ptr to output save area
                           aplVfp,                  // The value to format
                           nDigits,                 // # significant digits (0 = all)
                           UTF16_DOT,               // Char to use as decimal separator
                           UTF16_OVERBAR,           // Char to use as overbar
                           FALSE,                   // TRUE iff nDigits is # fractional digits
                           FALSE,                   // TRUE iff we're to substitute text for infinity
                           FALSE);                  // TRUE iff we're to precede the display with (FPCnnn)
} // End FormatAplVfp


//***************************************************************************
//  $FormatAplVfpFC
//
//  Format a variable FP using []FC
//***************************************************************************

LPAPLCHAR FormatAplVfpFC
    (LPWCHAR    lpaplChar,          // Ptr to output save area
     APLVFP     aplVfp,             // The value to format
     APLINT     nDigits,            // # significant digits (0 = all), or
                                    // # fractional digits (if bFractDigs)
                                    // If negative, use E-format
     APLCHAR    aplCharDecimal,     // Char to use as decimal separator
     APLCHAR    aplCharOverbar,     // Char to use as overbar
     UBOOL      bFractDigs,         // TRUE iff nDigits is # fractional digits
     UBOOL      bSubstInf,          // TRUE iff we're to substitute text for infinity
     UBOOL      bPrecFPC)           // TRUE iff we're to precede the display with (FPCnnn)

{
    mp_exp_t  expptr;               // Ptr to exponent shift
    APLINT    iLen,                 // String length
              iDiff,                // ...           difference between maximum digits and requested # digits
              iRes;                 // Loop counter
    UBOOL     bNeg;                 // TRUE iff the number is negative
    APLINT    iDecPt = 0,           // Position of the decimal point (0 = none)
              iDelDigits,           // # trailing digits to delete if bFractDigs
              iUnderflow;           // # trailing underflow digits
    int       iSigDigs,             // # significant digits for <mpfr_get_str>
              iIntDigs,             // # integer digits (may be > iPrcDigs)
              iFrcDigs,             // # fractional digits (may be > iPrcDigs) (excluding 'Enn')
              iPrcDigs;             // # significant digits in the precision
    LPCHAR    lpRawFmt;             // Ptr to raw formatted #

    // If we're to precede the display with (FPCnnn), ...
    if (bPrecFPC)
    {
        // Append leading marker
        *lpaplChar++ = L'(';

        lpaplChar =
          FormatAplint (lpaplChar,                  // Ptr to output save area
                        mpfr_get_prec (&aplVfp));   // The value to format
        // Append trailing marker overwriting the trailing space
        lpaplChar[-1] = L')';
    } // End IF

    // Izit an infinity?
    if (IsMpfInfinity (&aplVfp))
    {
        // Izit negative?
        if (mpfr_sgn (&aplVfp) < 0)
            *lpaplChar++ = aplCharOverbar;
        *lpaplChar++ = UTF16_INFINITY;
    } else
    {
        lpRawFmt = (LPCHAR) lpaplChar;

        // Get # significant digits
        iSigDigs = abs ((UINT) nDigits);

        // Format the VFP at maximum # digits
        mpfr_get_str (lpRawFmt,             // Ptr to output save area
                     &expptr,               // Ptr to exponent save area
                      10,                   // Base of number system
                      0,                    // # significant digits (0 = all)
                     &aplVfp,               // Ptr to VFP number
                      MPFR_RNDN);           // Rounding mode
        // Get the char length
        iDiff = lstrlen (lpRawFmt);

        // Check for negative
        bNeg = (lpRawFmt[0] EQ '-');

        // Get the # digits in the precision
        iPrcDigs = (int) iDiff - bNeg;

        // nDigits < 0 ==> !bFractDigs
        // nDigits >= 0 &&  bFractDigs ==> 'Fxx.yy' []FMT ...
        // nDigits >= 0 && !bFractDigs ==> 'Ixx'    []FMT ... or FormatAplVfp () or dyadic DownTackJot

        // If abs (number) >= 1
        if (expptr > 0)
        {
            iIntDigs = expptr;

            // If we're not displaying in E-format, ...
            if (nDigits >= 0)
            {
                // If nDigits is # fractional digits, ...
                if (bFractDigs)
                    iFrcDigs = (int) nDigits;
                else
                    iFrcDigs = max (iPrcDigs - iIntDigs, 0);
            } // End IF/ELSE
        } else
        // abs (number) < 1
        // 0.0000nnnnnnnnnnnnnn
        {
            // The integer digit is 0
            iIntDigs = 1;

            // If we're not displaying in E-format, ...
            if (nDigits >= 0)
            {
                // If nDigits is # fractional digits, ...
                if (bFractDigs)
                    iFrcDigs = (int) nDigits;
                else
                    iFrcDigs = (int) nDigits - expptr;
            } // End IF/ELSE
        } // End IF/ELSE

        // If we're not displaying in E-format, ...
        if (nDigits >= 0)
        {
            // If abs (number) >= 1,
            //   and nDigits is # fractional digits, ...
            if (expptr > 0
             && bFractDigs)
                // Add in the # integer digits
                iSigDigs += expptr;

            // Ask for no more digits than are in the precision
            iSigDigs = min (iSigDigs, iPrcDigs);
        } else
        {
            Assert (!bFractDigs);
        } // End IF/ELSE

        // If # significant digits NE # precision digits, ...
        if (iSigDigs NE iPrcDigs)
            // Format the VFP
            mpfr_get_str (lpRawFmt,                     // Ptr to output save area
                         &expptr,                       // Ptr to exponent save area
                          10,                           // Base of number system
                          iSigDigs,                     // # significant digits (0 = all)
                         &aplVfp,                       // Ptr to VFP number
                          MPFR_RNDN);                   // Rounding mode
        // Get the char length
        iLen = lstrlen (lpRawFmt);

        // Delete trailing zeros as <mpf_get_str> does
        for (iRes = iLen - 1; iRes >= (bNeg + max (expptr, 0)); iRes--)
        if (lpRawFmt[iRes] EQ '0')
        {
            lpRawFmt[iRes] = AC_EOS;
            iLen--;
        } else
            break;

        // If we're not displaying in E-format, ...
        if (nDigits >= 0)
            // Get the difference in char lengths
            iDiff -= iLen;

        // If displaying fractional digits, ...
        if (bFractDigs)
        {
            UINT uDig;                  // # significant digits based on the precision

            // Convert the precision to the # digits it represents
            //   via the formula  1 + floor (log10 (2^P))
            //                  = 1 + floor (P x log10 (2))
            //   where log10 (2) = (ln (2)) / (ln (10))
            //                   = M_LN2 / M_LN10
            uDig = 1 + (UINT) floor (mpfr_get_prec (&aplVfp) * M_LN2 / M_LN10);

            // Calculate the # trailing underflow digits
            iUnderflow = abs64 (nDigits) + max (expptr, 0) - max (uDig, (APLUINT) iLen);
            iUnderflow = max (iUnderflow, 0);
        } else
            iUnderflow = 0;

        // If the number is zero, ...
        if (iLen EQ 0)
        {
            // "Format" the number
            *lpaplChar++ = L'0';

            // If we're formatting in E-format, ...
            if (nDigits < 0)
            {
                *lpaplChar++ = aplCharDecimal;

                lpaplChar = FillMemoryW (lpaplChar, (APLU3264) ((-nDigits) - 1), L'0');

                *lpaplChar++ = DEF_EXPONENT_UC;
                *lpaplChar++ = L'0';
            } else
            // If displaying fractional digits, ...
            if (bFractDigs)
            {
                *lpaplChar++ = aplCharDecimal;

                lpaplChar = FillMemoryW (lpaplChar, (APLU3264)    nDigits      , L'0');
            } // End IF
        } else
        // If we're formatting in E-format, ...
        if (nDigits < 0)
        {
            // Copy the raw # to a point to the right of itself so we can format it in place
            CopyMemory (&lpRawFmt[6 + 2 * (iLen + 1)], lpRawFmt, (APLU3264) (iLen + 1));
            lpRawFmt += 6 + 2 * (iLen + 1);

            // Format the number with exactly nDigits significant digits
            lpaplChar =
              FormatExpFmt (lpaplChar,          // Ptr to output save area
                      (int) nDigits,            // # significant digits
                            lpRawFmt,           // Ptr to raw formatted number
                            expptr,             // Exponent
                            DEF_MAX_QUADPPVFP,  // Maximum # significant digits
                            aplCharDecimal,     // Char to use as decimal separator
                            aplCharOverbar);    // Char to use as overbar
        } else
        {
            // Convert the char to WCHAR
            for (iRes = iLen - 1; iRes >= 0; iRes--)
                lpaplChar[iRes] = lpRawFmt[iRes];

            // Check for Infinity
            if (lpaplChar[bNeg] EQ DEF_POSINFINITY_CHAR)
            {
                if (bSubstInf)
                {
                    lstrcpyW (&lpaplChar[bNeg], LTEXT_INFINITY);
                    iLen = lstrlenW (lpaplChar);
                } else
                    lpaplChar[bNeg] = UTF16_INFINITY;
            } else
            // If the number is in (-1, 1), ...
            if (expptr <= 0)
            {
                // Get the absolute value of the exponent
                expptr = -expptr;

                // If displaying fractional digits, ...
                if (bFractDigs)
                    iDelDigits = expptr;
                else
                    iDelDigits = 0;

                // Make room for "0." and leading zeros
                // If bFractDigs, nDigits is the # fractional digits to display
                //   so we must delete trailing expptr chars as
                //   the format string above produces nDigits
                //   significant digits
                MoveMemoryW (&lpaplChar[bNeg + 2 + expptr], &lpaplChar[bNeg], (APLU3264) (iLen - bNeg));

                // Fill in the leading "0."
                lpaplChar[bNeg + 0] = L'0';
                lpaplChar[iDecPt = bNeg + 1] = aplCharDecimal;

                // Fill in the leading zeros
                FillMemoryW (&lpaplChar[bNeg + 2], expptr, L'0');

                // If displaying fractional digits, ...
                if (bFractDigs)
                {
                    if (nDigits > ((iLen - bNeg) - expptr))
                        // Fill in the trailing zeros
                        FillMemoryW (&lpaplChar[iLen + 2], (APLU3264) (nDigits - ((iLen - bNeg) - expptr)), L'0');

                    //       Neg  "0."  Digits
                    iLen  = bNeg + 2 + nDigits;
                } else
                    //            "0." Leading
                    iLen +=        2 + expptr;
            } else
            // If the # significant digits is smaller than the exponent, ...
            if (iLen < (bNeg + expptr))
            {
                // Fill in trailing underscores up to but not including the decimal point
                FillMemoryW (&lpaplChar[iLen], (APLU3264) (bNeg + expptr - iLen), DEF_UNDERFLOW);
                iLen = bNeg + expptr;
            } else
            // If the # significant digits is larger than the exponent, ...
            if (iLen > (bNeg + expptr))
            {
                // Make room for the decimal point
                MoveMemoryW (&lpaplChar[bNeg + 1 + expptr], &lpaplChar[bNeg + expptr], (APLU3264) (iLen - (bNeg + expptr)));

                // Insert the decimal point
                lpaplChar[iDecPt = bNeg + expptr] = aplCharDecimal;
                iLen++;
            } // End IF/ELSE

            // If displaying fractional digits, ...
            if (bFractDigs)
            {
                if (iDecPt EQ 0)
                    lpaplChar[iDecPt = iLen++] = aplCharDecimal;

                // If the # digits after the decimal point is less than nDigits, ...
                if (nDigits > (iLen - iDecPt))
                {
                    // Fill with trailing zeros
                    FillMemoryW (&lpaplChar[iLen], (APLU3264) (nDigits - (iLen - (iDecPt + 1))), L'0');

                    //           Decpt    Digits
                    iLen =  1 + iDecPt + nDigits;
                } // End IF

                // Fill in the trailing underflow digits, possibly overwriting the decimal point
                if (iUnderflow)
                {
                    FillMemoryW (&lpaplChar[iLen - iUnderflow], (APLU3264) iUnderflow, DEF_UNDERFLOW);

                    // Restore the decimal point
                    lpaplChar[iDecPt] = aplCharDecimal;
                } // End IF
            } // End IF

            // If the number is negative, ...
            if (bNeg)
                lpaplChar[0] = aplCharOverbar;

            // Skip over the formatted number
            lpaplChar += iLen;
        } // End IF/ELSE/...
    } // End IF/ELSE

    // Include a separator
    *lpaplChar++ = L' ';

    return lpaplChar;
} // End FormatAplVfpFC


//***************************************************************************
//  $FormatSymTabConst
//
//  Format a symbol table constant
//***************************************************************************

LPAPLCHAR FormatSymTabConst
    (LPWCHAR    lpaplChar,
     LPSYMENTRY lpSymEntry)

{
    Assert (lpSymEntry->stFlags.Imm);

    return FormatImmed (lpaplChar,
                        lpSymEntry->stFlags.ImmType,
                       &lpSymEntry->stData.stLongest);
} // End FormatSymTabConst


//***************************************************************************
//  $DisplayTransferVar2
//
//  Create the Type 2 transfer form of a variable
//***************************************************************************

LPWCHAR DisplayTransferVar2
    (LPWCHAR    lpwszTemp,                  // Ptr to output save area
     LPSYMENTRY lpSymEntry)                 // Ptr to SYMENTRY of the var to display

{
    LPWCHAR lpwszOrig = lpwszTemp;          // Save the original ptr

    // Copy the var's name
    lpwszTemp =
      CopySteName (lpwszTemp,               // Ptr to result global memory
                   lpSymEntry,              // Ptr to function symbol table entry
                   NULL);                   // Ptr to name length (may be NULL)
    // If it's a System Var, ...
    if (lpSymEntry->stFlags.ObjName EQ OBJNAME_SYS)
        // Convert to upper case as some .atf interpreters don't handle lowercase well
        CharUpperBuffW (lpwszOrig, (DWORD) (lpwszTemp - lpwszOrig));

    // Append a left arrow
    *lpwszTemp++ = UTF16_LEFTARROW;

    // If the var is an immediate, ...
    if (lpSymEntry->stFlags.Imm)
        // Display the immediate value
        lpwszTemp =
          DisplayTransferImm2 (lpwszTemp,   // Ptr to output save area
                               lpSymEntry); // Ptr to SYMENTRY of the immediate to display
    else
        // Display the global memory value
        lpwszTemp =
          DisplayTransferGlb2 (lpwszTemp,                       // Ptr to output save area
                               lpSymEntry->stData.stGlbData,    // Arg global memory handle
                               0,                               // Outer NELM (if not at top level)
                               0,                               // Outer rank (if not at top level)
                               TRUE);                           // TRUE iff this call is at the top level
    // Delete the last blank in case it matters,
    //   and ensure properly terminated
    if (lpwszTemp[-1] EQ L' ')
        *--lpwszTemp = WC_EOS;
    else
        *lpwszTemp = WC_EOS;

    return lpwszTemp;
} // End DisplayTransferVar2


//***************************************************************************
//  $DisplayTransferChr2
//
//  Create the Type 2 transfer form of a character scalar or vector
//***************************************************************************

LPWCHAR DisplayTransferChr2
    (LPWCHAR    lpwszTemp,                  // Ptr to output save area
     LPAPLCHAR  lpMemArg,                   // Ptr to character scalar/vector
     APLNELM    aplNELMArg,                 // Arg NELM
     APLRANK    aplRankArg)                 // Arg rank

{
    APLUINT uCnt;                           // Loop counter

    // Start with a leading quote
    *lpwszTemp++ = WC_SQ;

    // Loop through the elements
    for (uCnt = 0; uCnt < aplNELMArg; uCnt++, ((LPAPLCHAR) lpMemArg)++)
    // If the data is a single quote, ...
    if ((*(LPAPLCHAR) lpMemArg) EQ WC_SQ)
    {
        // The data is ''
        *lpwszTemp++ = WC_SQ;
        *lpwszTemp++ = WC_SQ;
    } else
        // ***FIXME*** -- Use []UCS to handle chars outside APL2's charset
        *lpwszTemp++ = *(LPAPLCHAR) lpMemArg;

    // End with a trailing quote
    *lpwszTemp++ = WC_SQ;

    // and a trailing blank
    *lpwszTemp++ = L' ';

    return lpwszTemp;
} // End DisplayTransferChr2


//***************************************************************************
//  $DisplayTransferImm2
//
//  Create the Type 2 transfer form of an immediate
//***************************************************************************

LPWCHAR DisplayTransferImm2
    (LPWCHAR    lpwszTemp,                  // Ptr to output save area
     LPSYMENTRY lpSymEntry)                 // Ptr to SYMENTRY of the immediate to display

{
    // If the immediate is a char, ...
    if (IsImmChr (lpSymEntry->stFlags.ImmType))
        // Display the char immediate
        lpwszTemp =
          DisplayTransferChr2 (lpwszTemp,   // Ptr to output save area
                              &lpSymEntry->stData.stChar,   // Ptr to character scalar/vector
                               1,           // Arg NELM
                               0);          // Arg rank
    else
        lpwszTemp =
          FormatImmedFC (lpwszTemp,                         // Ptr to input string
                         lpSymEntry->stFlags.ImmType,       // Immediate type
                        &lpSymEntry->stData.stLongest,      // Ptr to value to format
                         DEF_MAX_QUADPP64,                  // # significant digits
                         UTF16_DOT,                         // Char to use as decimal separator
                         UTF16_OVERBAR,                     // Char to use as overbar
                         FLTDISPFMT_RAWFLT,                 // Float display format
                         FALSE);                            // TRUE iff we're to substitute text for infinity
    return lpwszTemp;
} // End DisplayTransferImm2


//***************************************************************************
//  $DisplayTransferGlb2
//
//  Create the Type 2 transfer form of a global
//***************************************************************************

LPWCHAR DisplayTransferGlb2
    (LPWCHAR    lpwszTemp,                  // Ptr to output save area
     HGLOBAL    hGlbArg,                    // Arg global memory handle
     APLNELM    aplNELMOut,                 // Outer NELM (if not at top level)
     APLRANK    aplRankOut,                 // Outer rank (if not at top level)
     UBOOL      bTopLevel)                  // TRUE iff this call is at the top level

{
    LPVOID   lpMemArg;                      // Ptr to arg item global memory
    APLSTYPE aplTypeArg;                    // Arg item storage type
    APLNELM  aplNELMArg,                    // Arg item NELM
             aplNELMNst;                    // Arg item NELM if nested
    APLRANK  aplRankArg;                    // Arg item rank
    APLUINT  uCnt;                          // Loop counter
    UBOOL    bNeedParens = FALSE;           // TRUE iff this level needs surrounding parens

    // Lock the memory to get a ptr to it
    lpMemArg = MyGlobalLock (hGlbArg);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemArg)
    // Get the Array Type, NELM, and Rank
    aplTypeArg = lpHeader->ArrType;
    aplNELMArg = lpHeader->NELM;
    aplRankArg = lpHeader->Rank;
#undef  lpHeader

    // If not at the top level, ...
    if (!bTopLevel)
    {
        // ***FIXME*** -- If the char vector has a non-APL2 char, use []UCS
        // If the inner item is not a non-singleton char vector,
        //   and the outer item is not a scalar,
        //   and the outer item is not empty, we need parens
        bNeedParens = (!(IsSimpleChar (aplTypeArg) && IsVector (aplRankArg) && !IsSingleton (aplNELMArg))
                    && !IsScalar (aplRankOut)
                    && !IsEmpty (aplNELMOut));
        if (bNeedParens)
            *lpwszTemp++ = L'(';

        // If the outer shell is a scalar or empty, append an {enclose} symbol
        if (IsScalar (aplRankOut)
         || IsEmpty (aplNELMOut))
            *lpwszTemp++ = UTF16_LEFTSHOE;
    } // End IF

#define lpHeader        ((LPVARARRAY_HEADER) lpMemArg)
    // Get the storage type, NELM & Rank
    aplTypeArg = lpHeader->ArrType;
    aplNELMArg = lpHeader->NELM;
    aplRankArg = lpHeader->Rank;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemArg = VarArrayBaseToDim (lpMemArg);

    // If the array is a matrix
    //   or a non-scalar singleton,
    //   or an empty nested array, ...
    if (IsMatrix (aplRankArg)
     || (IsSingleton (aplNELMArg) && !IsScalar (aplRankArg))
     || (IsEmpty (aplNELMArg) && IsNested (aplTypeArg)))
    {
        // Format & save the shape
        for (uCnt = 0; uCnt < aplRankArg; uCnt++)
            lpwszTemp =
              FormatAplintFC (lpwszTemp,                // Ptr to output save area
                              *((LPAPLDIM) lpMemArg)++, // The value to format
                              UTF16_OVERBAR);           // Char to use as overbar
        // lpMemArg now points to the data

        // If the array is not a scalar, ...
        if (!IsScalar (aplRankArg))
            // Append a {rho}, zapping the trailing blank
            lpwszTemp[-1] = UTF16_RHO;
    } else
        // Skip over the dimension(s)
        ((LPAPLDIM) lpMemArg) += aplRankArg;

    // Split cases based upon the arg storage type
    switch (aplTypeArg)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
            // Loop through the elements
            for (uCnt = 0; uCnt < aplNELMArg; uCnt++)
                lpwszTemp =
                  FormatAplintFC (lpwszTemp,        // Ptr to output save area
                                  GetNextInteger (lpMemArg, aplTypeArg, uCnt),  // The value to format
                                  UTF16_OVERBAR);   // Char to use as overbar
            break;

        case ARRAY_APA:
            // APAs are displayed as Off-Mulx[]IO-{iota}Len
            //   unless it's {zilde} in which case we use 0{rho}0

#define lpAPA           ((LPAPLAPA) lpMemArg)
            // If it's {zilde}, ...
            if (IsEmpty (aplNELMArg))
            {
                *lpwszTemp++ = L'0';
                *lpwszTemp++ = UTF16_RHO;
                *lpwszTemp++ = L'0';
            } else
            {
                // Append the offset
                lpwszTemp =
                  FormatAplintFC (lpwszTemp,            // Ptr to output save area
                                  lpAPA->Off,           // The value to format
                                  UTF16_OVERBAR);       // Char to use as overbar
                // Append a minus sign, zapping the trailing blank
                lpwszTemp[-1] = L'-';

                // Append the multiplier
                lpwszTemp =
                  FormatAplintFC (lpwszTemp,            // Ptr to output save area
                                  lpAPA->Mul,           // The value to format
                                  UTF16_OVERBAR);       // Char to use as overbar
#undef  lpAPA
                // Append {times}[]IO-{iota}, zapping the trailing blank
                lstrcpyW (&lpwszTemp[-1], WS_UTF16_TIMES $QUAD_IO L"-" WS_UTF16_IOTA);
                lpwszTemp = &lpwszTemp[lstrlenW (lpwszTemp)];

                // Format the length
                lpwszTemp =
                  FormatAplintFC (lpwszTemp,            // Ptr to output save area
                                  aplNELMArg,           // The value to format
                                  UTF16_OVERBAR);       // Char to use as overbar
            } // End IF/ELSE

            break;

        case ARRAY_FLOAT:
            // Loop through the elements
            for (uCnt = 0; uCnt < aplNELMArg; uCnt++)
                lpwszTemp =
                  FormatFloatFC (lpwszTemp,             // Ptr to output save area
                                 *((LPAPLFLOAT) lpMemArg)++, // Ptr to float value
                                 DEF_MAX_QUADPP64,      // # significant digits
                                 L'.',                  // Char to use as decimal separator
                                 UTF16_OVERBAR,         // Char to use as overbar
                                 FLTDISPFMT_RAWFLT,     // Float display format
                                 FALSE);                // TRUE iff we're to substitute text for infinity
            break;

        case ARRAY_CHAR:
            // Display the char vector
            lpwszTemp =
              DisplayTransferChr2 (lpwszTemp,       // Ptr to output save area
                                   lpMemArg,        // Ptr to character scalar/vector
                                   aplNELMArg,      // Arg NELM
                                   1);              // Arg rank
            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // If it's an empty nested array, ...
            if (IsEmpty (aplNELMArg) && IsNested (aplTypeArg))
                aplNELMNst = 1;
            else
                aplNELMNst = aplNELMArg;

            // Loop through the elements
            for (uCnt = 0; uCnt < aplNELMNst; uCnt++, ((LPAPLHETERO) lpMemArg)++)
            {
                // Split cases based upon the ptr type
                switch (GetPtrTypeInd (lpMemArg))
                {
                    case PTRTYPE_STCONST:
                        // Display the immediate value
                        lpwszTemp =
                          DisplayTransferImm2 (lpwszTemp,                   // Ptr to output save area
                                               *(LPAPLHETERO) lpMemArg);    // Ptr to SYMENTRY of the immediate to display
                        break;

                    case PTRTYPE_HGLOBAL:
                        // Display the global memory value
                        lpwszTemp =
                          DisplayTransferGlb2 (lpwszTemp,               // Ptr to output save area
                                               *(LPAPLHETERO) lpMemArg, // Arg global memory handle
                                               aplNELMArg,              // Outer NELM (if not at top level)
                                               aplRankArg,              // Outer rank (if not at top level)
                                               FALSE);                  // TRUE iff this call is at the top level
                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH

    // Delete the last blank in case it matters,
    //   and ensure properly terminated
    if (lpwszTemp[-1] EQ L' '
     && (bNeedParens || lpwszTemp[-2] NE WC_SQ))
        *--lpwszTemp = WC_EOS;
    else
        *lpwszTemp = WC_EOS;

    // If we need parens, ...
    if (bNeedParens)
        *lpwszTemp++ = L')';

    // We no longer this ptr
    MyGlobalUnlock (hGlbArg); lpMemArg = NULL;

    return lpwszTemp;
} // End DisplayTansferGlb2


//***************************************************************************
//  $DisplayTransferFcn2
//
//  Create the Type 2 transfer form of a function
//***************************************************************************

LPWCHAR DisplayTransferFcn2
    (LPWCHAR    lpwszTemp,                  // Ptr to output save area
     LPSYMENTRY lpSymEntry,                 // Ptr to SYMENTRY of the var to display
     FILETIME  *lpftCreation)               // Ptr to output save area for creation time stamp (may be NULL)

{
    HGLOBAL           hGlbDfnHdr = NULL;        // User-defined function operator global memory handle
    LPDFN_HEADER      lpMemDfnHdr = NULL;       // Ptr to user-defined function/operator global memory
    LPMEMTXT_UNION    lpMemTxtLine;             // Ptr to header/line text global memory
    HGLOBAL           hGlbTxtLine;              // Line text global memory handle
    LPFCNLINE         lpFcnLines;               // Ptr to array of function line structs (FCNLINE[numFcnLines])
    UINT              uNumLines,                // # function lines
                      uLine;                    // Loop counter

    // If the function is user-defined, ...
    if (lpSymEntry->stFlags.UsrDfn)
    {
        // Append "[]FX "
        lstrcpyW (lpwszTemp, $QUAD_FX L" ");
        lpwszTemp = &lpwszTemp[lstrlenW (lpwszTemp)];

        // Get the user-defined function/operator global memory handle
        hGlbDfnHdr = lpSymEntry->stData.stGlbData;

        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLock (hGlbDfnHdr);

        // Save creation time if requested
        if (lpftCreation)
            *lpftCreation = lpMemDfnHdr->ftCreation;

        // Lock the memory to get a ptr to it
        lpMemTxtLine = MyGlobalLock (lpMemDfnHdr->hGlbTxtHdr);

        // Display the function header
        lpwszTemp =
          DisplayTransferChr2 (lpwszTemp,
                              &lpMemTxtLine->C,
                               lpMemTxtLine->U,
                               1);
        // We no longer need this ptr
        MyGlobalUnlock (lpMemDfnHdr->hGlbTxtHdr); lpMemTxtLine = NULL;

        // Get ptr to array of function line structs (FCNLINE[numFcnLines])
        lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

        // Get # function lines
        uNumLines = lpMemDfnHdr->numFcnLines;

        // Run through the function lines looking for the longest
        for (uLine = 0; uLine < uNumLines; uLine++)
        {
            // Get the line text global memory handle
            hGlbTxtLine = lpFcnLines->hGlbTxtLine;

            if (hGlbTxtLine)
            {
                // Lock the memory to get a ptr to it
                lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

                // Display the function line
                lpwszTemp =
                  DisplayTransferChr2 (lpwszTemp,
                                      &lpMemTxtLine->C,
                                       lpMemTxtLine->U,
                                       1);
                // We no longer need this ptr
                MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;
            } // End IF

            // Skip to the next struct
            lpFcnLines++;
        } // End FOR

        // We no longer need this ptr
        MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
    } else
    {
        // Copy the var's name
        lpwszTemp =
          CopySteName (lpwszTemp,               // Ptr to result global memory
                       lpSymEntry,              // Ptr to function symbol table entry
                       NULL);                   // Ptr to name length (may be NULL)
        // Append a left arrow
        *lpwszTemp++ = UTF16_LEFTARROW;

        // If the function is immediate, ...
        if (lpSymEntry->stFlags.Imm)
            // Append the function/operator
            *lpwszTemp++ = lpSymEntry->stData.stChar;
        else
            // Append the function strand
            lpwszTemp =
              DisplayFcnGlb (lpwszTemp,
                             lpSymEntry->stData.stGlbData,
                             FALSE,
                             NULL,
                             NULL,
                             NULL,
                             NULL);
    } // End IF/ELSE

    // Delete the last blank in case it matters,
    //   and ensure properly terminated
    if (lpwszTemp[-1] EQ L' ')
        *--lpwszTemp = WC_EOS;
    else
        *lpwszTemp = WC_EOS;

    return lpwszTemp;
} // End DisplayTransferFcn2


//***************************************************************************
//  $GetQuadFCValue
//
//  Return a specified value from []FC
//***************************************************************************

APLCHAR GetQuadFCValue
    (UINT uIndex)

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    HGLOBAL      hGlbQuadFC;    // []FC global memory handle
    LPAPLCHAR    lpMemQuadFC;   // Ptr to []FC global memory
    APLNELM      aplNELMQuadFC; // []FC NELM
    APLRANK      aplRankQuadFC; // []FC rank
    APLCHAR      aplCharQuadFC; // []FC[uIndex]

    Assert (uIndex < FCNDX_LENGTH);

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the []FC global memory handle
    hGlbQuadFC = lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_FC]->stData.stGlbData;

    // Lock the memory to get a ptr to it
    lpMemQuadFC = MyGlobalLock (hGlbQuadFC);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemQuadFC)
    // Get the array NELM and rank
    aplNELMQuadFC = lpHeader->NELM;
    aplRankQuadFC = lpHeader->Rank;
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMemQuadFC = VarArrayDataFmBase (lpMemQuadFC);

    // Check for short []FC
    if (uIndex >= aplNELMQuadFC)
        aplCharQuadFC = (DEF_QUADFC_CWS)[uIndex];
    else
        aplCharQuadFC = lpMemQuadFC[uIndex];

    // We no longer need this ptr
    MyGlobalUnlock (hGlbQuadFC); lpMemQuadFC = NULL;

    return aplCharQuadFC;
} // End GetQuadFCValue


//***************************************************************************
//  $GetQuadICValue
//
//  Return a specified value from []IC
//***************************************************************************

APLINT GetQuadICValue
    (IC_INDICES uIndex)

{
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    HGLOBAL      hGlbQuadIC;        // []IC global memory handle
    LPAPLINT     lpMemQuadIC;       // Ptr to []IC global memory
    APLSTYPE     aplTypeQuadIC;     // []IC storage type
    APLNELM      aplNELMQuadIC;     // []IC NELM
    APLRANK      aplRankQuadIC;     // []IC rank
    APLINT       aplIntQuadIC;      // []IC[uIndex]

    Assert (uIndex < ICNDX_LENGTH);

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the []IC global memory handle
    hGlbQuadIC = lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_IC]->stData.stGlbData;

    // Lock the memory to get a ptr to it
    lpMemQuadIC = MyGlobalLock (hGlbQuadIC);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemQuadIC)
    // Get the array Type, NELM, and rank
    aplTypeQuadIC = lpHeader->ArrType;
    aplNELMQuadIC = lpHeader->NELM;
    aplRankQuadIC = lpHeader->Rank;
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMemQuadIC = VarArrayDataFmBase (lpMemQuadIC);

    // Check for short []IC
    if (uIndex >= aplNELMQuadIC)
        aplIntQuadIC = aplDefaultIC[uIndex];
    else
        // Get next item from global memory ([]IC can be BOOL, INT, or APA only)
        GetNextItemMem (lpMemQuadIC,        // Ptr to item global memory data
                        aplTypeQuadIC,      // Item storage type
                        uIndex,             // Index into item
                        NULL,               // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                       &aplIntQuadIC);      // Ptr to result immediate value (may be NULL)
    // We no longer need this ptr
    MyGlobalUnlock (hGlbQuadIC); lpMemQuadIC = NULL;

    return aplIntQuadIC;
} // End GetQuadICValue


//***************************************************************************
//  End of File: display.c
//***************************************************************************
