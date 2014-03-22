//***************************************************************************
//  NARS2000 -- Primitive Function -- DownTackJot
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
#include "headers.h"


typedef struct tagWID_PRC
{
    APLUINT uWid;                   // 00:  Actual width (if Auto, then this field is initially 0)
    APLINT  iPrc;                   // 04:  Actual precision (0 = none, >0 = decimal notation, <0 = E-notation)
    UINT    uMaxExp:30,             // 08:  3FFFFFFF:  If iPrc < 0, length of longest exponent including the E and {neg} (if any)
            Auto:1,                 //      40000000:  TRUE iff this width is automatic
            bAllChar:1;             //      80000000:  TRUE iff the column is all character
                                    // 0C:  Length
} WIDPRC, *LPWIDPRC;

// Macro to get the next char value taking into account heteros are preceded by storage type
#define GetHetChar(ptr,at)  ptr[IsSimpleHet (at)]


//***************************************************************************
//  $PrimFnDownTackJot_EM_YY
//
//  Primitive function for monadic and dyadic DownTackJot
//    ("default format" and "format by example")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDownTackJot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDownTackJot_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_DOWNTACKJOT);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc APPEND_NAME_ARG);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonDownTackJot_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydDownTackJot_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimFnDownTackJot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnDownTackJot_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic DownTackJot
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnDownTackJot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnDownTackJot_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnDownTackJot_EM_YY,// Ptr to primitive function routine
                                    lptkLftArg,             // Ptr to left arg token
                                    lptkFunc,               // Ptr to function token
                                    lptkRhtArg,             // Ptr to right arg token
                                    lptkAxis);              // Ptr to axis token (may be NULL)
} // End PrimProtoFnDownTackJot_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonDownTackJot_EM_YY
//
//  Primitive function for monadic DownTackJot ("default format")
//
//  Taken from the IBM APL2 documentation:
//
//  "The formal rules listed below for default formatting of nested arrays use
//   the function NOTCHAR.  NOTCHAR returns a 1 if its argument is not a simple
//   character array and a 0 otherwise.
//
//   For Z{is}{format}R, where R is a nested array:
//
//   * Z has a single left and right blank pad spaces
//   * Z has S intermediate blank spaces between horizontally adjacent items
//     A and B where:
//       S is the larger of ({rho}{rho}X) + NOTCHAR X
//       for both X =: A and B
//   * Z has LN intermediate blank lines between vertically adjacent items
//     C and D, where
//       LN is the 0{max}((({rho}{rho}C){max}{rho}{rho}D)-1
//   * If the rank of R is three or more, Z can contain blank lines for the
//     interdimensional spacing."
//   * Char scalar/vector items in columns containing numeric scalars are
//     right-justified; otherwise they are left-justified.
//
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonDownTackJot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonDownTackJot_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE      aplTypeRht;           // Right arg storage type
    APLNELM       aplNELMRht,           // Right arg NELM
                  aplNELMRes;           // Result    ...
    APLRANK       aplRankRht,           // Right arg rank
                  aplRankRes;           // Result    ...
    APLDIM        aplDimNCols,          // # columns
                  aplDimNRows,          // # rows
                  aplDimCol,            // Col loop counter
                  aplLastDim;           // Length of the last dimension in the result
    HGLOBAL       hGlbRht = NULL,       // Right arg global memory handle
                  hGlbRes = NULL;       // Result    ...
    LPVOID        lpMemRht = NULL,      // Ptr to right arg global memory
                  lpMemRes = NULL;      // Ptr to result    ...
    LPAPLDIM      lpMemDimRht = NULL;   // Ptr to right arg dimensions
    APLINT        aplIntegerRht;        // Right arg temporary integer
    APLFLOAT      aplFloatRht;          // ...                 float
    APLCHAR       aplCharRht;           // ...                 char
    LPFMTHEADER   lpFmtHeader,          // Ptr to format header struc
                  lpFmtHeader2;         // ...
    LPFMTCOLSTR   lpFmtColStr;          // Ptr to col struc
    LPAPLCHAR     lpaplChar,            // Ptr to output save area
                  lpaplCharStart;       // Ptr to start of output save area
    APLUINT       ByteRes;              // # bytes in the result
    UBOOL         bSimpleScalar;        // TRUE if right arg is a simple scalar
    LPPL_YYSTYPE  lpYYRes = NULL;       // Ptr to the result
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    LPPERTABDATA  lpMemPTD;             // Ptr to PerTabData global memory
    LPWCHAR       lpwszFormat;          // Ptr to formatting save area
    HGLOBAL       hGlbFormat = NULL;    // New lpwszFormat global memory handle

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get ptr to formatting save area
    lpwszFormat = lpMemPTD->lpwszFormat;

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the attributes (Type, NELM, and Rank) of the right args
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Get right arg's global ptrs
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    if (lpMemRht)
        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemRht);

    // Get the # rows & columns in the right arg
    if (IsScalar (aplRankRht))
        aplDimNCols = aplDimNRows = 1;
    else
    {
        aplDimNCols = lpMemDimRht[aplRankRht - 1];

        // Get the # rows (across all planes)
        if (IsVector (aplRankRht)
         || IsZeroDim (aplDimNCols))
            aplDimNRows = 1;
        else
            aplDimNRows = aplNELMRht / aplDimNCols;
    } // End IF/ELSE

    // Skip over the header and dimensions to the data
    if (lpMemRht)
        lpMemRht = VarArrayDataFmBase (lpMemRht);

RESTART_FORMAT:

__try
{
#ifdef DEBUG
    // Fill lpwszFormat with FFs so we can tell what we actually wrote
    FillMemory (lpwszFormat, 4096, 0xFF);
#endif

    // Create a new FMTHEADER
    lpFmtHeader2 = (LPFMTHEADER) lpwszFormat;
    ZeroMemory (lpFmtHeader2, sizeof (FMTHEADER));
    lpFmtHeader = lpFmtHeader2;
    lpFmtHeader->lpFmtHdrUp  = NULL;
#ifdef DEBUG
    lpFmtHeader->Sig.nature  = FMTHEADER_SIGNATURE;
#endif
////lpFmtHeader->lpFmtRowUp  = NULL;                // Already zero from ZeroMemory (No parent row struct)
////lpFmtHeader->lpFmtColUp  = NULL;                // ...                           ...       col ...
////lpFmtHeader->lpFmtRow1st =                      // Filled in below
////lpFmtHeader->lpFmtRowLst =                      // ...
////lpFmtHeader->lpFmtCol1st =                      // ...
    lpFmtHeader->uRealRows   = (UINT) aplDimNRows;  // # real rows from dimensions
////lpFmtHeader->uActRows    = 0;                   // Already zero from ZeroMemory (initial value)
    lpFmtHeader->uActCols    = (UINT) aplDimNCols;
////lpFmtHeader->uFmtRows    = 0;                   // Already zero from ZeroMemory (initial value)
////lpFmtHeader->uFmtInts    = 0;                   // ...                           ...
////lpFmtHeader->uFmtChrs    = 0;                   // ...                           ...
////lpFmtHeader->uFmtFrcs    = 0;                   // ...                           ...
////lpFmtHeader->uFmtTrBl    = 0;                   // ...                           ...
////lpFmtHeader->uMatRes     =                      // Filled in below

    // Create <aplDimNCols> FMTCOLSTRs
    lpFmtColStr = (LPFMTCOLSTR) (&lpFmtHeader[1]);

    // Create <aplDimNCols> FMTCOLSTRs in the output
    lpFmtHeader->lpFmtCol1st = lpFmtColStr;
    ZeroMemory (lpFmtColStr, (APLU3264) aplDimNCols * sizeof (FMTCOLSTR));
#ifdef DEBUG
    {
        APLDIM uCol;

        for (uCol = 0; uCol < aplDimNCols; uCol++)
            lpFmtColStr[uCol].Sig.nature = FMTCOLSTR_SIGNATURE;
    }
#endif
    // Skip over the FMTCOLSTRs to the next available position
    lpaplChar = lpaplCharStart = (LPAPLCHAR) &lpFmtColStr[aplDimNCols];

    // Save ptr to 1st and last child FMTROWSTR
    lpFmtHeader->lpFmtRow1st = lpFmtHeader->lpFmtRowLst =
      (IsZeroDim (aplDimNRows) ? NULL
                               : (LPFMTROWSTR) lpaplChar);
    // Loop through the array appending the formatted values (separated by WC_EOS)
    //   to the output vector, and accumulating the values in the appropriate
    //   FMTCOLSTR & FMTROWSTR entries.

    // If the right arg is a simple scalar,
    //   get its value
    bSimpleScalar = (IsScalar (aplRankRht) && IsSimpleNH (aplTypeRht));
    if (bSimpleScalar)
        GetFirstValueToken (lptkRhtArg,     // Ptr to right arg token
                           &aplIntegerRht,  // Ptr to integer result
                           &aplFloatRht,    // Ptr to float ...
                           &aplCharRht,     // Ptr to WCHAR ...
                            NULL,           // Ptr to longest ...
                            NULL,           // Ptr to lpSym/Glb ...
                            NULL,           // Ptr to ...immediate type ...
                            NULL);          // Ptr to array type ...
    // Split cases based upon the right arg's storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
////////////lpaplChar =
              CompileArrBool    (bSimpleScalar ? (LPAPLBOOL) &aplIntegerRht
                                               : (LPAPLBOOL)   lpMemRht,    // Ptr to right arg memory
                                 lpFmtHeader,                               // Ptr to parent header
                                 lpFmtColStr,                               // Ptr to vector of ColStrs
                                 lpaplChar,                                 // Ptr to compiled output
                                 aplDimNRows,                               // # rows
                                 aplDimNCols,                               // # cols
                                 aplRankRht,                                // Right arg rank
                                 lpMemDimRht,                               // Ptr to right arg dimensions
                                 TRUE);                                     // TRUE iff top level array
            break;

        case ARRAY_INT:
////////////lpaplChar =
              CompileArrInteger (bSimpleScalar ? &aplIntegerRht
                                               : (LPAPLINT)    lpMemRht,    // Ptr to right arg memory
                                 lpFmtHeader,                               // Ptr to parent header
                                 lpFmtColStr,                               // Ptr to vector of ColStrs
                                 lpaplChar,                                 // Ptr to compiled output
                                 aplDimNRows,                               // # rows
                                 aplDimNCols,                               // # cols
                                 aplRankRht,                                // Right arg rank
                                 lpMemDimRht,                               // Ptr to right arg dimensions
                                 TRUE);                                     // TRUE iff top level array
            break;

        case ARRAY_FLOAT:
////////////lpaplChar =
              CompileArrFloat   (bSimpleScalar ? &aplFloatRht
                                               : (LPAPLFLOAT)  lpMemRht,    // Ptr to right arg memory
                                 lpFmtHeader,                               // Ptr to parent header
                                 lpFmtColStr,                               // Ptr to vector of ColStrs
                                 lpaplChar,                                 // Ptr to compiled output
                                 aplDimNRows,                               // # rows
                                 aplDimNCols,                               // # cols
                                 aplRankRht,                                // Right arg rank
                                 lpMemDimRht,                               // Ptr to right arg dimensions
                                 TRUE);                                     // TRUE iff top level array
            break;

        case ARRAY_APA:
////////////lpaplChar =
              CompileArrAPA     ((LPAPLAPA)    lpMemRht,                    // Ptr to right arg memory
                                 lpFmtHeader,                               // Ptr to parent header
                                 lpFmtColStr,                               // Ptr to vector of ColStrs
                                 lpaplChar,                                 // Ptr to compiled output
                                 aplDimNRows,                               // # rows
                                 aplDimNCols,                               // # cols
                                 aplRankRht,                                // Right arg rank
                                 lpMemDimRht,                               // Ptr to right arg dimensions
                                 TRUE);                                     // TRUE iff top level array
            break;

        case ARRAY_CHAR:
            // Return the right arg

            // If the right arg is scalar,
            //   return it as an immediate
            if (IsScalar (aplRankRht))
            {
                // Allocate a new YYRes
                lpYYRes = YYAlloc ();

                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
                lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_CHAR;
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkChar     = aplCharRht;
                lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

                goto QUICK_EXIT;
            } else
            {
                // Make a copy of the right arg
                hGlbRes = CopySymGlbDirAsGlb (hGlbRht);

                goto NORMAL_EXIT;
            } // End IF/ELSE

        case ARRAY_HETERO:
////////////lpaplChar =
              CompileArrHetero  ((LPAPLHETERO) lpMemRht,        // Ptr to right arg memory
                                 lpFmtHeader,                   // Ptr to parent header
                                 lpFmtColStr,                   // Ptr to vector of ColStrs
                                 lpaplChar,                     // Ptr to compiled output
                                 aplDimNRows,                   // # rows
                                 aplDimNCols,                   // # cols
                                 aplRankRht,                    // Right arg rank
                                 lpMemDimRht,                   // Ptr to right arg dimensions
                                 TRUE,                          // TRUE iff top level array
                                 FALSE);                        // TRUE iff handle []TCLF specially
            break;

        case ARRAY_NESTED:
////////////lpaplChar =
              CompileArrNested  ((LPAPLNESTED) lpMemRht,        // Ptr to right arg memory
                                 lpFmtHeader,                   // Ptr to parent header
                                 lpFmtColStr,                   // Ptr to vector of ColStrs
                                 lpaplChar,                     // Ptr to compiled output
                                 aplDimNRows,                   // # rows
                                 aplDimNCols,                   // # cols
                                 aplRankRht,                    // Right arg rank
                                 lpMemDimRht,                   // Ptr to right arg dimensions
                                 TRUE);                         // TRUE iff top level array
            break;

        case ARRAY_RAT:
////////////lpaplChar =
              CompileArrRat     ((LPAPLRAT)    lpMemRht,        // Ptr to right arg memory
                                 lpFmtHeader,                   // Ptr to parent header
                                 lpFmtColStr,                   // Ptr to vector of ColStrs
                                 lpaplChar,                     // Ptr to compiled output
                                 aplDimNRows,                   // # rows
                                 aplDimNCols,                   // # cols
                                 aplRankRht,                    // Right arg rank
                                 lpMemDimRht,                   // Ptr to right arg dimensions
                                 TRUE);                         // TRUE iff top level array
            break;

        case ARRAY_VFP:
////////////lpaplChar =
              CompileArrVfp     ((LPAPLVFP)    lpMemRht,        // Ptr to right arg memory
                                 lpFmtHeader,                   // Ptr to parent header
                                 lpFmtColStr,                   // Ptr to vector of ColStrs
                                 lpaplChar,                     // Ptr to compiled output
                                 aplDimNRows,                   // # rows
                                 aplDimNCols,                   // # cols
                                 aplRankRht,                    // Right arg rank
                                 lpMemDimRht,                   // Ptr to right arg dimensions
                                 TRUE);                         // TRUE iff top level array
            break;

        defstop
            break;
    } // End SWITCH
} __except (CheckException (GetExceptionInformation (), L"PrimFnMonDownTackJot_EM_YY"))
{
    // Split cases based upon the exception code
    switch (MyGetExceptionCode ())
    {
        APLU3264 uNewSize;              // Temp new size
        LPWCHAR  lpwszFormat2;          // Temp ptr
        HGLOBAL  hGlbFormat2;           // Temp global memory handle

        case EXCEPTION_ACCESS_VIOLATION:
        case EXCEPTION_LIMIT_ERROR:
            // If this is our first time through, ...
            if (hGlbFormat EQ NULL)
            {
                LPMEMVIRTSTR lpLclMemVirtStr;       // Ptr to local MemVirtStr

                // Get a ptr to the PTDMEMVIRT_STR
                (HANDLE_PTR) lpLclMemVirtStr = GetWindowLongPtrW (lpMemPTD->hWndSM, GWLSF_LPMVS);

                uNewSize = lpLclMemVirtStr[PTDMEMVIRT_WSZFORMAT].MaxSize;
            } else
                uNewSize = MyGlobalSize (hGlbFormat);

            // Attempt to allocate more space in global memory rather than using lpMemPTD->lpwszFormat
            hGlbFormat2 =
              DbgGlobalAlloc (GHND, 2 * uNewSize);
            if (hGlbFormat2)
            {
                // Lock the memory to get a ptr to it
                lpwszFormat2 = MyGlobalLock (hGlbFormat2);

                // If there's an old save area, ...
                if (hGlbFormat)
                {
////////////////////// Lock the memory to get a ptr to it
////////////////////lpwszFormat = MyGlobalLock (hGlbFormat);
////////////////////
////////////////////// Copy the old data to the new memory
////////////////////CopyMemory (lpwszFormat2, lpwszFormat, uNewSize);
////////////////////
////////////////////// We no longer need this ptr
////////////////////MyGlobalUnlock (hGbFormat); lpwszFormat = NULL;

                    // We no longer need this resource
                    MyGlobalFree (hGlbFormat); hGlbFormat = NULL;
////////////////} else
////////////////{
////////////////    // Copy the old data to the new memory
////////////////    CopyMemory (lpwszFormat2, lpwszFormat, uNewSize);
                } // End IF/ELSE

                // Save the new handle & ptr
                hGlbFormat  = hGlbFormat2;
                lpwszFormat = lpwszFormat2;

                goto RESTART_FORMAT;
            } // End IF

            goto LIMIT_EXIT;

        defstop
            break;
    } // End SWITCH
} // End __try/__except

    // Propagate the row & col count up the line
    PropagateRowColCount (lpFmtHeader);

    // CHARs are handled above

    // For nested arrays, the rank of the result is one
    //   if all items are scalars or vectors, otherwise
    //   the rank is two.
    // For non-CHAR simple arrays, the rank of the result
    //   is the larger of the rank of the right arg and one.
    if (IsNested (aplTypeRht) || lpFmtHeader->uMatRes)
        aplRankRes = 1 + lpFmtHeader->uMatRes;
    else
        aplRankRes = max (aplRankRht, 1);

    // Add up the width of each column to get the
    //   # cols in the result
    for (aplLastDim = aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        aplLastDim += ExteriorColWidth (&lpFmtColStr[aplDimCol]);
    Assert (aplLastDim EQ ExteriorHdrWidth (lpFmtHeader));

    // Calculate the NELM of the result
    aplNELMRes = (lpFmtHeader->uFmtRows * ExteriorHdrWidth (lpFmtHeader));

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRes, aplRankRes);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    //***************************************************************
    // Now we can allocate the storage for the result
    //***************************************************************
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemRes);

    // If this is a nested array and the rank
    //   of the result is two, fill in the
    //   next-to-the-last dimension
    if ((IsNested (aplTypeRht) || lpFmtHeader->uMatRes)
     && IsMatrix (aplRankRes))
        // Fill in the # rows
        *((LPAPLDIM) lpMemRes)++ = lpFmtHeader->uFmtRows;
    else
    {
        // Fill in all but the last dimension
        if (lpMemDimRht && !IsScalar (aplRankRht))
        for (aplDimCol = 0; aplDimCol < (aplRankRht - 1); aplDimCol++)
            *((LPAPLDIM) lpMemRes)++ = *lpMemDimRht++;
    } // End IF

    // Fill in the last dimension
    *((LPAPLDIM) lpMemRes)++ = aplLastDim;

    // lpMemRes now points to the result's data
#ifdef PREFILL
    // Check for overflow
    if (aplNELMRes NE (APLU3264) aplNELMRes)
        goto WSFULL_EXIT;

    // Fill it with all blanks
    FillMemoryW (lpMemRes, (APLU3264) aplNELMRes, L' ');
#endif
    // Output the result at appropriate widths

    // Run through the right arg again processing the
    //   output stream into lpMemRes

    // Split cases based upon the right arg's storage type
#define lpwszOut    ((LPAPLCHAR) lpMemRes)
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_CHAR:
        case ARRAY_APA:
        case ARRAY_HETERO:
        case ARRAY_RAT:
        case ARRAY_VFP:
////////////lpaplChar =
              FormatArrSimple (lpFmtHeader,             // Ptr to FMTHEADER
                               lpFmtColStr,             // Ptr to vector of <aplDimNCols> FMTCOLSTRs
                               lpaplCharStart,          // Ptr to compiled input
                              &lpwszOut,                // Ptr to ptr to output string
                               lpFmtHeader->uActRows,   // # actual rows in this array
                               aplDimNCols,             // # cols in this array
                               aplLastDim,              // Length of last dim in result
                               aplRankRht,              // Rank of this array
                               lpMemDimRht,             // Ptr to this array's dimensions
                               aplTypeRht,              // Storage type of this array
                               TRUE,                    // TRUE iff skip to next row after this item
                               FALSE,                   // TRUE iff raw (not {thorn}) output
                               TRUE,                    // TRUE iff last line has CR
                               lpbCtrlBreak);           // Ptr to Ctrl-Break flag
            break;

        case ARRAY_NESTED:
////////////lpaplChar =
              FormatArrNested (lpFmtHeader,             // Ptr to FMTHEADER
                               lpMemRht,                // Ptr to raw input
                               lpFmtColStr,             // Ptr to vector of <aplDimNCols> FMTCOLSTRs
                               lpaplCharStart,          // Ptr to compiled input
                              &lpwszOut,                // Ptr to ptr to output string
                               lpFmtHeader->uActRows,   // # actual rows in this array
                               aplDimNCols,             // # cols ...
                               aplRankRht,              // Rank of this array
                               lpMemDimRht,             // Ptr to this array's dimensions
                               aplLastDim,              // Length of last dim in result
                               FALSE,                   // TRUE iff raw (not {thorn}) output
                               TRUE,                    // TRUE iff last line has CR
                               lpbCtrlBreak);           // Ptr to Ctrl-Break flag
            break;

        defstop
            break;
    } // End SWITCH
#undef  lpwszOut

    // Check for Ctrl-Break
    if (CheckCtrlBreak (*lpbCtrlBreak))
        goto ERROR_EXIT;
NORMAL_EXIT:
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto QUICK_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LIMIT_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LIMIT_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes)
    {
        if (lpMemRes)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
QUICK_EXIT:
    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    if (hGlbFormat && lpwszFormat)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbFormat); lpwszFormat = NULL;

        // We no longer need this resource
        MyGlobalFree (hGlbFormat); hGlbFormat = NULL;
    } // End IF

    return lpYYRes;
} // End PrimFnMonDownTackJot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $CompileArrBool
//
//  Compile an array of Booleans
//***************************************************************************

LPAPLCHAR CompileArrBool
    (LPAPLBOOL   lpMem,         // Ptr to data to format
     LPFMTHEADER lpFmtHeader,   // Ptr to parent FMTHEADER
     LPFMTCOLSTR lpFmtColStr,   // Ptr to vector of <aplDimNCols> FMTCOLSTRs
     LPAPLCHAR   lpaplChar,     // Ptr to next available format position
     APLDIM      aplDimNRows,   // # rows to format (actually it's x/ all but last dim)
     APLDIM      aplDimNCols,   // # cols to format
     APLRANK     aplRank,       // Rank of data to format
     LPAPLDIM    lpMemDim,      // Ptr to dimension vector
     UBOOL       bTopLevel)     // TRUE iff top level array

{
    UINT        uBitMask,
                uLen;
    APLDIM      aplDimCol,
                aplDimRow;
    LPFMTROWSTR lpFmtRowLcl = NULL;

    // Loop through the cols, setting the common widths
    for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
    {
        // Set column type
        lpFmtColStr[aplDimCol].colType = COLTYPE_NOTCHAR;

        // Include a leading blank if not 1st col
        uLen = (aplDimCol NE 0);

        // Max the current leading blanks with this
        lpFmtColStr[aplDimCol].uLdBlMax = max (lpFmtColStr[aplDimCol].uLdBlMax, uLen);

        // Calculate the length of the integer part
        uLen = 1;

        // Max the current integer width with this
        lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);

////////// Calculate the length of the fractional part
////////uLen = 0;
////////
////////// Max the current fractional width with this
////////lpFmtColStr[aplDimCol].uFrcs = max (lpFmtColStr[aplDimCol].uFrcs, uLen);
    } // End FOR

    // Initialize bit mask for all references to lpMem
    uBitMask = BIT0;

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
        ZeroMemory (lpFmtRowLcl, sizeof (lpFmtRowLcl[0]));
#ifdef DEBUG
        lpFmtRowLcl->Sig.nature  = FMTROWSTR_SIGNATURE;
        lpFmtRowLcl->lpFmtColUp  = lpFmtColStr;
#endif
////////lpFmtRowLcl->uAccWid     = 0;               // Initialize the accumulated width
        lpFmtRowLcl->uFmtRows    = 1;               // Initialize the count
////////lpFmtRowLcl->bRptCol     = FALSE;           // Initialize as not repeating a col
////////lpFmtRowLcl->bDone       = FALSE;           // Initialize as not done with output
        lpFmtRowLcl->bRealRow    = TRUE;            // Initialize as a real row
////////lpFmtRowLcl->bBlank      = FALSE;           // Initialize as not all blank
////////lpFmtRowLcl->uColOff     = 0;               // Initialize the col offset
////////lpFmtRowLcl->uItemCount  = 0;               // Initialize the # items to follow
////////lpFmtRowLcl->lpFmtRowNxt = NULL;            // Initialize the ptr
        lpFmtRowLcl->aplType     = ARRAY_BOOL;      // Initialize the storage type

        // Link into the row chain
        lpFmtHeader->lpFmtRowLst = lpFmtRowLcl;

        // Skip over the FMTROWSTR to the next available position
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        // Save the ptr to the 1st value
        lpFmtRowLcl->lpNxtChar   = lpaplChar;

        // Loop through the cols
        for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        {
            // Format the Boolean
            *lpaplChar++ = ((uBitMask & *lpMem) ? L'1' : L'0');

            // Append a string separator
            *lpaplChar++ = WC_EOS;

            // Count in another item
            lpFmtRowLcl->uItemCount++;

            // Shift over the bit mask
            uBitMask <<= 1;

            // Check for end-of-byte
            if (uBitMask EQ END_OF_BYTE)
            {
                uBitMask = BIT0;        // Start over
                lpMem++;                // Skip to next byte
            } // End IF
        } // End FOR

        // Save the ptr to the terminating zero
        lpFmtRowLcl->lpEndChar   = lpaplChar - 1;

        // Save as ptr to next row struc
        lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;

        // If this is not at the top level and not the last row,
        //   we need to count the interplanar spacing
        //   as blank rows in the row count
        if ((!bTopLevel)                        // Not top level
         && aplDimRow NE (aplDimNRows - 1))     // Not last row
            lpaplChar = CompileBlankRows (lpaplChar,        // Ptr to output buffer
                                          lpMemDim,         // Ptr to item dimensions
                                          0,                // Maximum row rank of next row
                                          aplRank,          // Item rank
                                          aplDimRow,        // Item row #
                                          lpFmtColStr);     // Ptr to item FMTCOLSTR
    } // End FOR

    // Loop through the cols, ...
    for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        // Set the interior width
        lpFmtColStr[aplDimCol].uIntWid = lpFmtColStr[aplDimCol].uInts;

    // Mark as last row struc
    if (lpFmtRowLcl)
        lpFmtRowLcl->lpFmtRowNxt = NULL;

    // Propagate the row & col count up the line
    PropagateRowColCount (lpFmtHeader);

    return lpaplChar;
} // End CompileArrBool


//***************************************************************************
//  $CompileArrInteger
//
//  Compile an array of integers
//***************************************************************************

LPAPLCHAR CompileArrInteger
    (LPAPLINT    lpMem,         // Ptr to data to format
     LPFMTHEADER lpFmtHeader,   // Ptr to parent FMTHEADER
     LPFMTCOLSTR lpFmtColStr,   // Ptr to vector of <aplDimNCols> FMTCOLSTRs
     LPAPLCHAR   lpaplChar,     // Ptr to next available format position
     APLDIM      aplDimNRows,   // # rows to format (actually it's x/ all but last dim)
     APLDIM      aplDimNCols,   // # cols to format
     APLRANK     aplRank,       // Rank of data to format
     LPAPLDIM    lpMemDim,      // Ptr to dimension vector
     UBOOL       bTopLevel)     // TRUE iff top level array

{
    UINT        uLen;               // Length of integer part
    APLDIM      aplDimCol,          // Loop counter
                aplDimRow;          // ...
    LPAPLCHAR   lpwszOut;           // Ptr to output buffer
    LPFMTROWSTR lpFmtRowLcl = NULL; // Ptr to local FMTROWSTR

    // Loop through the cols, setting the column type
    for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        // Set column type
        lpFmtColStr[aplDimCol].colType = COLTYPE_NOTCHAR;

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
        ZeroMemory (lpFmtRowLcl, sizeof (lpFmtRowLcl[0]));
#ifdef DEBUG
        lpFmtRowLcl->Sig.nature  = FMTROWSTR_SIGNATURE;
        lpFmtRowLcl->lpFmtColUp  = lpFmtColStr;
#endif
////////lpFmtRowLcl->uAccWid     = 0;               // Initialize the accumulated width
        lpFmtRowLcl->uFmtRows    = 1;               // Initialize the count
////////lpFmtRowLcl->bRptCol     = FALSE;           // Initialize as not repeating a col
////////lpFmtRowLcl->bDone       = FALSE;           // Initialize as not done with output
        lpFmtRowLcl->bRealRow    = TRUE;            // Initialize as a real row
////////lpFmtRowLcl->bBlank      = FALSE;           // Initialize as not all blank
////////lpFmtRowLcl->uColOff     = 0;               // Initialize the col offset
////////lpFmtRowLcl->uItemCount  = 0;               // Initialize the # items to follow
////////lpFmtRowLcl->lpFmtRowNxt = NULL;            // Initialize the ptr
        lpFmtRowLcl->aplType     = ARRAY_INT;       // Initialize the storage type

        // Link into the row chain
        lpFmtHeader->lpFmtRowLst = lpFmtRowLcl;

        // Skip over the FMTROWSTR to the next available position
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        // Save the ptr to the 1st value
        lpFmtRowLcl->lpNxtChar   = lpaplChar;

        // Loop through the cols
        for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        {
            // Format the integer
            lpaplChar =
              FormatAplint (lpwszOut = lpaplChar,
                            *lpMem++);
            // Zap the trailing blank
            lpaplChar[-1] = WC_EOS;

            // Count in another item
            lpFmtRowLcl->uItemCount++;

            // Include a leading blank if not 1st col
            uLen = (aplDimCol NE 0);

            // Max the current leading blanks with this
            lpFmtColStr[aplDimCol].uLdBlMax = max (lpFmtColStr[aplDimCol].uLdBlMax, uLen);

            // Calculate the length of the integer part
            uLen = (UINT) (lpaplChar - lpwszOut) - 1;

            // Max the current integer width with this
            lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);

////////////// Calculate the length of the fractional part
////////////uLen = 0;
////////////
////////////// Max the current fractional width with this
////////////lpFmtColStr[aplDimCol].uFrcs = max (lpFmtColStr[aplDimCol].uFrcs, uLen);
        } // End FOR

        // Save the ptr to the terminating zero
        lpFmtRowLcl->lpEndChar   = lpaplChar - 1;

        // Save as ptr to next row struc
        lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;

        // If this is not at the top level and not the last row,
        //   we need to count the interplanar spacing
        //   as blank rows in the row count
        if ((!bTopLevel)                        // Not top level
         && aplDimRow NE (aplDimNRows - 1))     // Not last row
            lpaplChar = CompileBlankRows (lpaplChar,        // Ptr to output buffer
                                          lpMemDim,         // Ptr to item dimensions
                                          0,                // Maximum row rank of next row
                                          aplRank,          // Item rank
                                          aplDimRow,        // Item row #
                                          lpFmtColStr);     // Ptr to item FMTCOLSTR
    } // End FOR

    // Loop through the cols, ...
    for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        // Set the interior width
        lpFmtColStr[aplDimCol].uIntWid = lpFmtColStr[aplDimCol].uInts;

    // Mark as last row struc
    if (lpFmtRowLcl)
        lpFmtRowLcl->lpFmtRowNxt = NULL;

    // Propagate the row & col count up the line
    PropagateRowColCount (lpFmtHeader);

    return lpaplChar;
} // End CompileArrInteger


//***************************************************************************
//  $CompileArrFloat
//
//  Compile an array of floats
//***************************************************************************

LPAPLCHAR CompileArrFloat
    (LPAPLFLOAT  lpMem,         // Ptr to data to format
     LPFMTHEADER lpFmtHeader,   // Ptr to parent FMTHEADER
     LPFMTCOLSTR lpFmtColStr,   // Ptr to vector of <aplDimNCols> FMTCOLSTRs
     LPAPLCHAR   lpaplChar,     // Ptr to next available format position
     APLDIM      aplDimNRows,   // # rows to format (actually it's x/ all but last dim)
     APLDIM      aplDimNCols,   // # cols to format
     APLRANK     aplRank,       // Rank of data to format
     LPAPLDIM    lpMemDim,      // Ptr to dimension vector
     UBOOL       bTopLevel)     // TRUE iff top level array

{
    UINT        uLen;
    APLDIM      aplDimCol,
                aplDimRow;
    LPAPLCHAR   lpwszOut;
    LPWCHAR     lpwsz;
    LPFMTROWSTR lpFmtRowLcl = NULL;

    // Loop through the cols, setting the column type
    for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        // Set column type
        lpFmtColStr[aplDimCol].colType = COLTYPE_NOTCHAR;

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
        ZeroMemory (lpFmtRowLcl, sizeof (lpFmtRowLcl[0]));
#ifdef DEBUG
        lpFmtRowLcl->Sig.nature  = FMTROWSTR_SIGNATURE;
        lpFmtRowLcl->lpFmtColUp  = lpFmtColStr;
#endif
////////lpFmtRowLcl->uAccWid     = 0;               // Initialize the accumulated width
        lpFmtRowLcl->uFmtRows    = 1;               // Initialize the count
////////lpFmtRowLcl->bRptCol     = FALSE;           // Initialize as not repeating a col
////////lpFmtRowLcl->bDone       = FALSE;           // Initialize as not done with output
        lpFmtRowLcl->bRealRow    = TRUE;            // Initialize as a real row
////////lpFmtRowLcl->bBlank      = FALSE;           // Initialize as not all blank
////////lpFmtRowLcl->uColOff     = 0;               // Initialize the col offset
////////lpFmtRowLcl->uItemCount  = 0;               // Initialize the # items to follow
////////lpFmtRowLcl->lpFmtRowNxt = NULL;            // Initialize the ptr
        lpFmtRowLcl->aplType     = ARRAY_FLOAT;     // Initialize the storage type

        // Link into the row chain
        lpFmtHeader->lpFmtRowLst = lpFmtRowLcl;

        // Skip over the FMTROWSTR to the next available position
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        // Save the ptr to the 1st value
        lpFmtRowLcl->lpNxtChar   = lpaplChar;

        // Loop through the cols
        for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        {
            // Format the float
            lpaplChar =
              FormatFloat (lpwszOut = lpaplChar,    // Ptr to output save area
                           *lpMem++,                // The floating point value
                           0);                      // Use default significant digits
            // Zap the trailing blank
            lpaplChar[-1] = WC_EOS;

            // Count in another item
            lpFmtRowLcl->uItemCount++;

            // Include a leading blank if not 1st col
            uLen = (aplDimCol NE 0);

            // Max the current leading blanks with this
            lpFmtColStr[aplDimCol].uLdBlMax = max (lpFmtColStr[aplDimCol].uLdBlMax, uLen);

            // Check for decimal point
            lpwsz = strchrW (lpwszOut, L'.');
            if (lpwsz)
            {
                // Calculate the length of the integer part
                uLen = (UINT) (lpwsz - lpwszOut);

                // Max the current integer width with this
                lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);

                // Calculate the length of the fractional part
                //   including the decimal point
                uLen = (UINT) (lpaplChar - lpwsz) - 1;

                // Max the current fractional width with this
                lpFmtColStr[aplDimCol].uFrcs = max (lpFmtColStr[aplDimCol].uFrcs, uLen);
            } else  // No decimal point
            {
                // Calculate the length of the integer part
                uLen = (UINT) (lpaplChar - lpwszOut) - 1;

                // Max the current integer width with this
                lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);

////////////////// Calculate the length of the fractional part
////////////////uLen = 0;
////////////////
////////////////// Max the current fractional width with this
////////////////lpFmtColStr[aplDimCol].uFrcs = max (lpFmtColStr[aplDimCol].uFrcs, uLen);
            } // End IF/ELSE
        } // End FOR

        // Save the ptr to the terminating zero
        lpFmtRowLcl->lpEndChar   = lpaplChar - 1;

        // Save as ptr to next row struc
        lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;

        // If this is not at the top level and not the last row,
        //   we need to count the interplanar spacing
        //   as blank rows in the row count
        if ((!bTopLevel)                        // Not top level
         && aplDimRow NE (aplDimNRows - 1))     // Not last row
            lpaplChar = CompileBlankRows (lpaplChar,        // Ptr to output buffer
                                          lpMemDim,         // Ptr to item dimensions
                                          0,                // Maximum row rank of next row
                                          aplRank,          // Item rank
                                          aplDimRow,        // Item row #
                                          lpFmtColStr);     // Ptr to item FMTCOLSTR
    } // End FOR

    // Loop through the cols, ...
    for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        // Set the interior width
        lpFmtColStr[aplDimCol].uIntWid = lpFmtColStr[aplDimCol].uInts
                                       + lpFmtColStr[aplDimCol].uFrcs;
    // Mark as last row struc
    if (lpFmtRowLcl)
        lpFmtRowLcl->lpFmtRowNxt = NULL;

    // Propagate the row & col count up the line
    PropagateRowColCount (lpFmtHeader);

    return lpaplChar;
} // End CompileArrFloat


//***************************************************************************
//  $CompileArrChar
//
//  Compile an array of chars
//***************************************************************************

LPAPLCHAR CompileArrChar
    (LPAPLCHAR   lpMem,         // Ptr to data to format
     LPFMTHEADER lpFmtHeader,   // Ptr to parent FMTHEADER
     LPFMTCOLSTR lpFmtColStr,   // Ptr to vector of one FMTCOLSTR (because it's char)
     LPAPLCHAR   lpaplChar,     // Ptr to next available format position
     APLDIM      aplDimNRows,   // # rows to format (actually it's x/ all but last dim)
     APLDIM      aplDimNCols,   // # cols to format
     APLRANK     aplRank,       // Rank of data to format
     LPAPLDIM    lpMemDim,      // Ptr to dimension vector
     UBOOL       bTopLevel)     // TRUE iff top level array

{
    APLDIM      aplDimRow,      // Loop counter
                aplDimCol;      // ...
    LPFMTROWSTR lpFmtRowLcl = NULL; // Ptr to local FMTROWSTR
    UINT        uCurPos;        // Current col position

    // Set column type
    lpFmtColStr[0].colType   = max (lpFmtColStr[0].colType, COLTYPE_ALLCHAR);

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
        ZeroMemory (lpFmtRowLcl, sizeof (lpFmtRowLcl[0]));
#ifdef DEBUG
        lpFmtRowLcl->Sig.nature  = FMTROWSTR_SIGNATURE;
        lpFmtRowLcl->lpFmtColUp  = lpFmtColStr;
#endif
////////lpFmtRowLcl->uAccWid     = 0;               // Initialize the accumulated width
        lpFmtRowLcl->uFmtRows    = 1;               // Initialize the count
////////lpFmtRowLcl->bRptCol     = FALSE;           // Initialize as not repeating a col
////////lpFmtRowLcl->bDone       = FALSE;           // Initialize as not done with output
        lpFmtRowLcl->bRealRow    = TRUE;            // Initialize as a real row
////////lpFmtRowLcl->bBlank      = FALSE;           // Initialize as not all blank
////////lpFmtRowLcl->uColOff     = 0;               // Initialize the col offset
////////lpFmtRowLcl->uItemCount  = 0;               // Initialize the # items to follow
////////lpFmtRowLcl->lpFmtRowNxt = NULL;            // Initialize the ptr
        lpFmtRowLcl->aplType     = ARRAY_CHAR;      // Initialize the storage type

        // Link into the row chain
        lpFmtHeader->lpFmtRowLst = lpFmtRowLcl;

        // Skip over the FMTROWSTR to the next available position
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        // Save the ptr to the 1st value
        lpFmtRowLcl->lpNxtChar   = lpaplChar;

        // Copy the data including a terminating zero
        //  and launder special chars
        for (aplDimCol = uCurPos = 0;
             aplDimCol < aplDimNCols;
             aplDimCol++)
        // Split cases based upon the char
        switch (lpMem[aplDimCol])
        {
            case TCNUL:         // []TCNUL -- Ignore this
                break;

            default:            // Insert a new char
                lpaplChar[uCurPos] = lpMem[aplDimCol];
                uCurPos++;

                break;
        } // End FOR/SWITCH

        // Skip over the current item width
        lpaplChar += uCurPos;

        // Skip over the right arg cols
        lpMem     += aplDimNCols;

        // Ensure properly terminated
        *lpaplChar++ = WC_EOS;

        // Count in another item
        lpFmtRowLcl->uItemCount++;

        // Max the current width with the width of this col
        lpFmtColStr[0].uChrs = max (lpFmtColStr[0].uChrs, uCurPos);

        // Save the ptr to the terminating zero
        lpFmtRowLcl->lpEndChar   = lpaplChar - 1;

        // Save as ptr to next row struc
        lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;

////    // If this is not at the top level and not the last row,
        // If this is not the last row,
        //   we need to count the interplanar spacing
        //   as blank rows in the row count
////    if ((!bTopLevel)                        // Not top level
////     && aplDimRow NE (aplDimNRows - 1))     // Not last row
        if (aplDimRow NE (aplDimNRows - 1))     // Not last row
            lpaplChar = CompileBlankRows (lpaplChar,        // Ptr to output buffer
                                          lpMemDim,         // Ptr to item dimensions
                                          0,                // Maximum row rank of next row
                                          aplRank,          // Item rank
                                          aplDimRow,        // Item row #
                                          lpFmtColStr);     // Ptr to item FMTCOLSTR
    } // End FOR

    // Set the interior width
    lpFmtColStr[0].uIntWid = lpFmtColStr[0].uChrs;

    // Mark as last row struc
    if (lpFmtRowLcl)
        lpFmtRowLcl->lpFmtRowNxt = NULL;

    // Propagate the row & col count up the line
    PropagateRowColCount (lpFmtHeader);

    return lpaplChar;
} // End CompileArrChar


//***************************************************************************
//  $CompileArrAPA
//
//  Compile an array of APA
//***************************************************************************

LPAPLCHAR CompileArrAPA
    (LPAPLAPA    lpAPA,         // Ptr to data to format
     LPFMTHEADER lpFmtHeader,   // Ptr to parent FMTHEADER
     LPFMTCOLSTR lpFmtColStr,   // Ptr to vector of <aplDimNCols> FMTCOLSTRs
     LPAPLCHAR   lpaplChar,     // Ptr to next available format position
     APLDIM      aplDimNRows,   // # rows to format (actually it's x/ all but last dim)
     APLDIM      aplDimNCols,   // # cols to format
     APLRANK     aplRank,       // Rank of data to format
     LPAPLDIM    lpMemDim,      // Ptr to dimension vector
     UBOOL       bTopLevel)     // TRUE iff top level array

{
    APLINT      apaOff,
                apaMul,
                apaAcc;
    UINT        uLen;
    APLDIM      aplDimCol,      // Loop counter
                aplDimRow;      // ...
    LPAPLCHAR   lpwszOut;
    LPFMTROWSTR lpFmtRowLcl = NULL;

    // Loop through the cols, setting the column type
    for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        // Set column type
        lpFmtColStr[aplDimCol].colType = COLTYPE_NOTCHAR;

    // Get the APA parameters
    apaOff = lpAPA->Off;
    apaMul = lpAPA->Mul;

    // Loop through the rows
    for (aplDimRow = apaAcc = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
        ZeroMemory (lpFmtRowLcl, sizeof (lpFmtRowLcl[0]));
#ifdef DEBUG
        lpFmtRowLcl->Sig.nature  = FMTROWSTR_SIGNATURE;
        lpFmtRowLcl->lpFmtColUp  = lpFmtColStr;
#endif
////////lpFmtRowLcl->uAccWid     = 0;               // Initialize the accumulated width
        lpFmtRowLcl->uFmtRows    = 1;               // Initialize the count
////////lpFmtRowLcl->bRptCol     = FALSE;           // Initialize as not repeating a col
////////lpFmtRowLcl->bDone       = FALSE;           // Initialize as not done with output
        lpFmtRowLcl->bRealRow    = TRUE;            // Initialize as a real row
////////lpFmtRowLcl->bBlank      = FALSE;           // Initialize as not all blank
////////lpFmtRowLcl->uColOff     = 0;               // Initialize the col offset
////////lpFmtRowLcl->uItemCount  = 0;               // Initialize the # items to follow
////////lpFmtRowLcl->lpFmtRowNxt = NULL;            // Initialize the ptr
        lpFmtRowLcl->aplType     = ARRAY_INT;       // Initialize the storage type

        // Link into the row chain
        lpFmtHeader->lpFmtRowLst = lpFmtRowLcl;

        // Skip over the FMTROWSTR to the next available position
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        // Save the ptr to the 1st value
        lpFmtRowLcl->lpNxtChar   = lpaplChar;

        // Loop through the cols
        for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++, apaAcc++)
        {
            // Format the integer
            lpaplChar =
              FormatAplint (lpwszOut = lpaplChar,
                            apaOff + apaMul * apaAcc);
            // Zap the trailing blank
            lpaplChar[-1] = WC_EOS;

            // Count in another item
            lpFmtRowLcl->uItemCount++;

            // Include a leading blank if not 1st col
            uLen = (aplDimCol NE 0);

            // Max the current leading blanks with this
            lpFmtColStr[aplDimCol].uLdBlMax = max (lpFmtColStr[aplDimCol].uLdBlMax, uLen);

            // Calculate the length of the integer part
            uLen = (UINT) (lpaplChar - lpwszOut) - 1;

            // Max the current integer width with this
            lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);

////////////// Calculate the length of the fractional part
////////////uLen = 0;
////////////
////////////// Max the current fractional width with this
////////////lpFmtColStr[aplDimCol].uFrcs = max (lpFmtColStr[aplDimCol].uFrcs, uLen);
        } // End FOR

        // Save the ptr to the terminating zero
        lpFmtRowLcl->lpEndChar   = lpaplChar - 1;

        // Save as ptr to next row struc
        lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;

        // If this is not at the top level and not the last row,
        //   we need to count the interplanar spacing
        //   as blank rows in the row count
        if ((!bTopLevel)                        // Not top level
         && aplDimRow NE (aplDimNRows - 1))     // Not last row
            lpaplChar = CompileBlankRows (lpaplChar,        // Ptr to output buffer
                                          lpMemDim,         // Ptr to item dimensions
                                          0,                // Maximum row rank of next row
                                          aplRank,          // Item rank
                                          aplDimRow,        // Item row #
                                          lpFmtColStr);     // Ptr to item FMTCOLSTR
    } // End FOR

    // Loop through the cols, ...
    for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        // Set the interior width
        lpFmtColStr[aplDimCol].uIntWid = lpFmtColStr[aplDimCol].uInts;

    // Mark as last row struc
    if (lpFmtRowLcl)
        lpFmtRowLcl->lpFmtRowNxt = NULL;

    // Propagate the row & col count up the line
    PropagateRowColCount (lpFmtHeader);

    return lpaplChar;
} // End CompileArrAPA


//***************************************************************************
//  $CompileArrHetero
//
//  Compile an array of LPSYMENTRYs
//***************************************************************************

LPAPLCHAR CompileArrHetero
    (LPAPLHETERO lpMem,         // Ptr to data to format
     LPFMTHEADER lpFmtHeader,   // Ptr to parent FMTHEADER
     LPFMTCOLSTR lpFmtColStr,   // Ptr to vector of <aplDimNCols> FMTCOLSTRs
     LPAPLCHAR   lpaplChar,     // Ptr to next available format position
     APLDIM      aplDimNRows,   // # rows to format (actually it's x/ all but last dim)
     APLDIM      aplDimNCols,   // # cols to format
     APLRANK     aplRank,       // Rank of data to format
     LPAPLDIM    lpMemDim,      // Ptr to dimension vector
     UBOOL       bTopLevel,     // TRUE iff top level array
     UBOOL       bTCLF)         // TRUE iff handle []TCLF specially

{
    UINT        uLen;           // Length of integer or fractional part
    APLDIM      aplDimCol,      // Loop counter
                aplDimRow,      // ...
                uCol;           // ...
    LPAPLCHAR   lpwszOut;       // Ptr to output buffer
    LPWCHAR     lpwsz;          // ...
    LPFMTROWSTR lpFmtRowLcl = NULL; // Ptr to current FMTROWSTR
    APLSTYPE    aplTypeItm;     // Item's array storage type
    IMM_TYPES   immTypeCur,     // Current immediate type
                immTypeLst;     // Last    ...

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        UINT uTCLF = 0;         // # []TCLFs in a given row

        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
        ZeroMemory (lpFmtRowLcl, sizeof (lpFmtRowLcl[0]));
#ifdef DEBUG
        lpFmtRowLcl->Sig.nature  = FMTROWSTR_SIGNATURE;
        lpFmtRowLcl->lpFmtColUp  = lpFmtColStr;
#endif
////////lpFmtRowLcl->uAccWid     = 0;               // Initialize the accumulated width
        lpFmtRowLcl->uFmtRows    = 1;               // Initialize the count
////////lpFmtRowLcl->bRptCol     = FALSE;           // Initialize as not repeating a col
////////lpFmtRowLcl->bDone       = FALSE;           // Initialize as not done with output
        lpFmtRowLcl->bRealRow    = TRUE;            // Initialize as a real row
////////lpFmtRowLcl->bBlank      = FALSE;           // Initialize as not all blank
////////lpFmtRowLcl->uColOff     = 0;               // Initialize the col offset
////////lpFmtRowLcl->uItemCount  = 0;               // Initialize the # items to follow
////////lpFmtRowLcl->lpFmtRowNxt = NULL;            // Initialize the ptr
        lpFmtRowLcl->aplType     = ARRAY_HETERO;    // Initialize the storage type

        // Link into the row chain
        lpFmtHeader->lpFmtRowLst = lpFmtRowLcl;

        // Skip over the FMTROWSTR to the next available position
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        // Save the ptr to the 1st value
        lpFmtRowLcl->lpNxtChar   = lpaplChar;

        // Note that it doesn't matter what we start off the row
        //   with as the last immediate type (immTypeLst) because
        //   the phrase "(aplDimCol NE 0) &&" causes any value to
        //   which we set it to be ignored.
////////immTypeLst = IMMTYPE_BOOL;

        // Loop through the cols
        for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        {
            // Split cases based upon the ptr type bits
            switch (GetPtrTypeInd (lpMem))
            {
                case PTRTYPE_STCONST:
#define lpSymEntry      ((LPAPLHETERO) lpMem)
                    // Save the immediate type
                    immTypeCur = (*lpSymEntry)->stFlags.ImmType;

                    break;

                case PTRTYPE_HGLOBAL:
                    // Get the attributes of the global memory handle
                    AttrsOfGlb (*(HGLOBAL *) lpMem, &aplTypeItm, NULL, NULL, NULL);

                    // Save the immediate type
                    immTypeCur = TranslateArrayTypeToImmType (aplTypeItm);

                    break;

                defstop
                    break;
            } // End SWITCH

            // Set column type
            lpFmtColStr[aplDimCol].colType =
              IsImmChr (immTypeCur) ? max (lpFmtColStr[aplDimCol].colType, COLTYPE_ALLCHAR)
                                    : COLTYPE_NOTCHAR;
            // Append the storage type
            *lpaplChar++ = TranslateImmTypeToArrayType (immTypeCur);

            lpaplChar =
              FormatImmed (lpwszOut = lpaplChar,
                           immTypeCur,
                           (GetPtrTypeDir (*lpSymEntry) EQ PTRTYPE_STCONST)
                          ? &(*lpSymEntry)->stData.stLongest
                          :   (LPVOID) *lpSymEntry);
            // Skip to next entry
            lpSymEntry++;
#undef  lpSymEntry

            // Zap the trailing blank
            lpaplChar[-1] = WC_EOS;

            // Count in another item
            lpFmtRowLcl->uItemCount++;

            // Include a leading blank if (not first col) and either last or current is not a char
            uLen = ((aplDimCol NE 0) && (!IsImmChr (immTypeLst) || !IsImmChr (immTypeCur)));

            // Max the current leading blanks with this
            lpFmtColStr[aplDimCol].uLdBlMax = max (lpFmtColStr[aplDimCol].uLdBlMax, uLen);

            // Check for decimal point, unless char
            if (!IsImmChr (immTypeCur)
             && (lpwsz = strchrW (lpwszOut, L'.')))
            {
                // Calculate the length of the integer part
                uLen = (UINT) (lpwsz - lpwszOut);

                // Max the current integer width with this
                lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);

                // Calculate the length of the fractional part
                uLen = (UINT) (lpaplChar - lpwsz) - 1;

                // Max the current fractional width with this
                lpFmtColStr[aplDimCol].uFrcs = max (lpFmtColStr[aplDimCol].uFrcs, uLen);
            } else  // No decimal point
            {
                // Calculate the length of the integer part
                // We use the following odd construction because
                //   if the previous SymEntry was a []TCLF,
                //   lpaplChar EQ lpwszOut
                uLen = (UINT) (lpaplChar - lpwszOut);
                uLen = max (uLen, 1) - 1;

                if (IsImmChr (immTypeCur))
                    // Max the current integer width with this
                    lpFmtColStr[aplDimCol].uChrs = max (lpFmtColStr[aplDimCol].uChrs, uLen);
                else
                    // Max the current integer width with this
                    lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);

////////////// Calculate the length of the fractional part
////////////uLen = 0;
////////////
////////////// Max the current fractional width with this
////////////lpFmtColStr[aplDimCol].uFrcs = max (lpFmtColStr[aplDimCol].uFrcs, uLen);
            } // End IF/ELSE

            // Save as last immediate type
            immTypeLst = immTypeCur;
        } // End FOR

        // Fill in trailing zeros for each []TCLF
        for (uCol = 0; uCol < uTCLF; uCol++)
        {
            *lpaplChar++ = ARRAY_CHAR;
            *lpaplChar++ = WC_EOS;
        } // End IF

        // Save the ptr to the terminating zero
        lpFmtRowLcl->lpEndChar   = lpaplChar - 1;

        // Save as ptr to next row struc
        lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;

        // If this is not at the top level and not the last row,
        //   we need to count the interplanar spacing
        //   as blank rows in the row count
        if ((!bTopLevel)                        // Not top level
         && aplDimRow NE (aplDimNRows - 1))     // Not last row
            lpaplChar = CompileBlankRows (lpaplChar,        // Ptr to output buffer
                                          lpMemDim,         // Ptr to item dimensions
                                          0,                // Maximum row rank of next row
                                          aplRank,          // Item rank
                                          aplDimRow,        // Item row #
                                          lpFmtColStr);     // Ptr to item FMTCOLSTR
    } // End FOR

    // Loop through the cols, ...
    for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        // Set the interior width
        lpFmtColStr[aplDimCol].uIntWid = max (lpFmtColStr[aplDimCol].uInts,
                                              lpFmtColStr[aplDimCol].uChrs)
                                       + lpFmtColStr[aplDimCol].uFrcs;
    // Mark as last row struc
    if (lpFmtRowLcl)
        lpFmtRowLcl->lpFmtRowNxt = NULL;

    // Propagate the row & col count up the line
    PropagateRowColCount (lpFmtHeader);

    return lpaplChar;
} // End CompileArrHetero


//***************************************************************************
//  $CompileArrNested
//
//  Compile an array of LPSYMENTRYs/NESTEDs
//***************************************************************************

LPAPLCHAR CompileArrNested
    (LPAPLNESTED lpMem,         // Ptr to data to format
     LPFMTHEADER lpFmtHeader,   // Ptr to parent FMTHEADER
     LPFMTCOLSTR lpFmtColStr,   // Ptr to vector of <aplDimNCols> FMTCOLSTRs
     LPAPLCHAR   lpaplChar,     // Ptr to next available format position
     APLDIM      aplDimNRows,   // # rows to format (actually it's x/ all but last dim)
     APLDIM      aplDimNCols,   // # cols to format
     APLRANK     aplRank,       // Rank of data to format
     LPAPLDIM    lpMemDim,      // Ptr to dimension vector
     UBOOL       bTopLevel)     // TRUE iff top level array

{
    APLDIM      aplDimCol,          // Loop counter
                aplDimRow;          // ...
    APLRANK     aplRowRankCur,      // Maximum rank across the current row
                aplRowRankNxt,      // ...                     next    ...
                aplRowRankMax;      // Maximum of aplRowRankCur and aplRowRankNxt
    LPFMTROWSTR lpFmtRowLcl = NULL; // Ptr to local FMTROWSTR
    UINT        NotCharPlusPrv,     // ({rho}{rho}L) + NOTCHAR L
                NotCharPlusCur;     // ...        R            R

    // Set the Matrix result bit if the arg is
    //   a matrix or higher rank array
    lpFmtHeader->uMatRes |= IsMultiRank (aplRank);

    // If there are any cols, ...
    if (aplDimNCols)
    {
        // Nested arrays always include one leading blank
        lpFmtColStr[0              ].uLdBlNst++;

        // Nested arrays always include one trailing blank
        lpFmtColStr[aplDimNCols - 1].uTrBlNst++;
    } // End IF

    // Initialize maximum rank across the current row
    aplRowRankCur = 0;

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
        ZeroMemory (lpFmtRowLcl, sizeof (lpFmtRowLcl[0]));
#ifdef DEBUG
        lpFmtRowLcl->Sig.nature  = FMTROWSTR_SIGNATURE;
        lpFmtRowLcl->lpFmtColUp  = lpFmtColStr;
#endif
////////lpFmtRowLcl->uAccWid     = 0;               // Initialize the accumulated width
        lpFmtRowLcl->uFmtRows    = 1;               // Initialize the count
////////lpFmtRowLcl->bRptCol     = FALSE;           // Initialize as not repeating a col
////////lpFmtRowLcl->bDone       = FALSE;           // Initialize as not done with output
        lpFmtRowLcl->bRealRow    = TRUE;            // Initialize as a real row
////////lpFmtRowLcl->bBlank      = FALSE;           // Initialize as not all blank
////////lpFmtRowLcl->uColOff     = 0;               // Initialize the col offset
////////lpFmtRowLcl->uItemCount  = 0;               // Initialize the # items to follow
////////lpFmtRowLcl->lpFmtRowNxt = NULL;            // Initialize the ptr
        lpFmtRowLcl->aplType     = ARRAY_NESTED;    // Initialize the storage type

        // Link into the row chain
        lpFmtHeader->lpFmtRowLst = lpFmtRowLcl;

        // Skip over the FMTROWSTR to the next available position
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        // Save the ptr to the 1st value
        lpFmtRowLcl->lpNxtChar   = lpaplChar;

        // Initialize NOTCHAR+ vars
        NotCharPlusPrv =
        NotCharPlusCur = NEG1U;

        // Loop through the cols
        for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++, ((LPAPLHETERO) lpMem)++)
        {
            // Split cases based upon the ptr type
            switch (GetPtrTypeInd (lpMem))
            {
                case PTRTYPE_STCONST:
                    lpaplChar =
                      CompileArrNestedCon ((LPAPLHETERO) lpMem,             // Ptr to data to format
                                           lpFmtHeader,                     // The parent FMTHEADER
                                           lpFmtRowLcl,                     // This row's FMTROWSTR
                                          &lpFmtColStr[aplDimCol],          // This col's FMTCOLSTR
                                          &NotCharPlusPrv,                  // Ptr to previous NOTCHAR+ var
                                          &NotCharPlusCur,                  // Ptr to current  ...
                                           lpaplChar,                       // Ptr to next available position
                                           aplDimCol EQ 0);                 // TRUE iff first (leftmost) col
                    break;

                case PTRTYPE_HGLOBAL:
                    lpaplChar =
                      CompileArrNestedGlb (ClrPtrTypeInd (lpMem),           // The HGLOBAL to format
                                           lpFmtHeader,                     // The parent FMTHEADER
                                           lpFmtRowLcl,                     // This row's FMTROWSTR
                                          &lpFmtColStr[aplDimCol],          // This col's FMTCOLSTR
                                          &NotCharPlusPrv,                  // Ptr to previous NOTCHAR+ var
                                          &NotCharPlusCur,                  // Ptr to current  ...
                                           lpaplChar,                       // Ptr to next available position
                                           aplDimCol EQ 0);                 // TRUE iff first (leftmost) col
                    break;

                defstop
                    break;
            } // End SWITCH

            // Save the NOTCHAR+ value for next time
            NotCharPlusPrv = NotCharPlusCur;
        } // End FOR

        // Save the ptr to the terminating zero
        lpFmtRowLcl->lpEndChar   = lpaplChar - 1;

        // Save as ptr to next row struc
        lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;

        // Initialize maximum rank across the next row
        aplRowRankNxt =  0;

        // If this is not the last row, ...
        if (aplDimRow NE (aplDimNRows - 1))
        // Run through the cols again for the next row finding the maximum rank
        //   so we can insert the proper # of intermediate blank lines
        for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        // Split cases based upon the ptr type
        switch (GetPtrTypeDir (((LPAPLHETERO) lpMem)[aplDimCol]))
        {
            LPVARARRAY_HEADER lpTmp;

            case PTRTYPE_STCONST:
////////////////// Use the larger rank
////////////////aplRowRankNxt = max (aplRowRankNxt, 0);

                break;

            case PTRTYPE_HGLOBAL:
                // Lock the memory to get a ptr to it
                lpTmp = MyGlobalLock (((LPAPLHETERO) lpMem)[aplDimCol]);

                // Use the larger rank
                aplRowRankNxt = max (aplRowRankNxt, lpTmp->Rank);

                // We no longer need this ptr
                MyGlobalUnlock (((LPAPLHETERO) lpMem)[aplDimCol]); lpTmp = NULL;

                break;

            defstop
                break;
        } // End IF/FOR/SWITCH

        // Use the larger rank
        aplRowRankMax = max (aplRowRankCur, aplRowRankNxt);

        // If this is not the last row,
        //   we need to count the interplanar spacing
        //   as blank rows in the row count
        if (aplDimRow NE (aplDimNRows - 1))         // Not last row
            lpaplChar = CompileBlankRows (lpaplChar,        // Ptr to output buffer
                                          lpMemDim,         // Ptr to item dimensions
                                          aplRowRankMax,    // Maximum rank across the current and next row
                                          aplRank,          // Item rank
                                          aplDimRow,        // Item row #
                                          lpFmtColStr);     // Ptr to item FMTCOLSTR
        // Save for the next row
        aplRowRankCur = aplRowRankNxt;
    } // End FOR

    // Mark as last row struc
    if (lpFmtRowLcl)
        lpFmtRowLcl->lpFmtRowNxt = NULL;

    // Propagate the row & col count up the line
    PropagateRowColCount (lpFmtHeader);

    return lpaplChar;
} // End CompileArrNested


//***************************************************************************
//  $CompileArrNestedCon
//
//  Compile a symbol table constant with a nested array
//***************************************************************************

LPAPLCHAR CompileArrNestedCon
    (LPAPLHETERO lpMem,             // Ptr to data to format
     LPFMTHEADER lpFmtHdrUp,        // The parent FMTHEADER
     LPFMTROWSTR lpFmtRowUp,        // This row's FMTROWSTR
     LPFMTCOLSTR lpFmtColUp,        // This col's FMTCOLSTR
     LPUINT      lpNotCharPlusPrv,  // Ptr to prevous NOTCHAR+ var
     LPUINT      lpNotCharPlusCur,  // Ptr to current ...
     LPAPLCHAR   lpaplChar,         // Ptr to next available position
     UBOOL       b1stCol)           // TRUE iff first (leftmost) col

{
    LPFMTHEADER lpFmtHeader;        // Ptr to format header struc
    LPFMTCOLSTR lpFmtColStr;        // This col's FMTCOLSTR
    IMM_TYPES   immTypeCon;         // Constant immediate type
    UINT        uMax;               // Maximum of NotCharPluses


#define aplDimNRows     1
#define aplDimNCols     1
#define aplRank         0

    // Create a new FMTHEADER
    lpFmtHeader = (LPFMTHEADER) lpaplChar;
    ZeroMemory (lpFmtHeader, sizeof (FMTHEADER));
    lpFmtHeader->lpFmtHdrUp  = lpFmtHdrUp;
#ifdef DEBUG
    lpFmtHeader->Sig.nature  = FMTHEADER_SIGNATURE;
#endif
    lpFmtHeader->lpFmtRowUp  = lpFmtRowUp;
    lpFmtHeader->lpFmtColUp  = lpFmtColUp;
////lpFmtHeader->lpFmtRow1st =                      // Filled in below
////lpFmtHeader->lpFmtRowLst =                      // ...
////lpFmtHeader->lpFmtCol1st =                      // ...
////lpFmtHeader->uRealRows   = 0;                   // Already zero from ZeroMemory (initial value)
////lpFmtHeader->uActRows    = 0;                   // ...
    lpFmtHeader->uActCols    = (UINT) aplDimNCols;
////lpFmtHeader->uFmtRows    = 0;                   // Already zero from ZeroMemory (initial value)
////lpFmtHeader->uFmtInts    = 0;                   // ...                           ...
////lpFmtHeader->uFmtChrs    = 0;                   // ...                           ...
////lpFmtHeader->uFmtFrcs    = 0;                   // ...                           ...
////lpFmtHeader->uFmtTrBl    = 0;                   // ...                           ...
    lpFmtHeader->uMatRes     = IsMultiRank (aplRank);

    // Skip over the FMTHEADER to the next available position
    lpFmtColStr = (LPFMTCOLSTR) (&lpFmtHeader[1]);

    // Create <aplDimNCols> FMTCOLSTRs in the output
    lpFmtHeader->lpFmtCol1st = lpFmtColStr;
    ZeroMemory (lpFmtColStr, (APLU3264) aplDimNCols * sizeof (FMTCOLSTR));
#ifdef DEBUG
    {
        APLDIM uCol;

        for (uCol = 0; uCol < aplDimNCols; uCol++)
            lpFmtColStr[uCol].Sig.nature = FMTCOLSTR_SIGNATURE;
    }
#endif
    // Save ptr to parent col
    lpFmtColStr->lpFmtColUp = lpFmtColUp;

    // Skip over the FMTCOLSTRs to the next available position
    lpaplChar = (LPAPLCHAR) (&lpFmtColStr[aplDimNCols]);

    // Save ptr to 1st and last child FMTROWSTR
    lpFmtHeader->lpFmtRow1st = lpFmtHeader->lpFmtRowLst =
      (IsZeroDim (aplDimNRows) ? NULL
                               : (LPFMTROWSTR) lpaplChar);
    // Set the immediate type
    immTypeCon = (*lpMem)->stFlags.ImmType;

    // Split cases based upon the STE immediate type
    switch (immTypeCon)
    {
        case IMMTYPE_BOOL:
            lpaplChar =
              CompileArrBool    (&(*lpMem)->stData.stBoolean,   // Ptr to right arg memory
                                  lpFmtHeader,                  // Ptr to parent header
                                  lpFmtColStr,                  // Ptr to vector of ColStrs
                                  lpaplChar,                    // Ptr to compiled output
                                  aplDimNRows,                  // # rows
                                  aplDimNCols,                  // # cols
                                  aplRank,                      // Right arg rank
                                  NULL,                         // Ptr to right arg dimensions
                                  FALSE);                       // TRUE iff top level array
            break;

        case IMMTYPE_INT:
            lpaplChar =
              CompileArrInteger (&(*lpMem)->stData.stInteger,   // Ptr to right arg memory
                                  lpFmtHeader,                  // Ptr to parent header
                                  lpFmtColStr,                  // Ptr to vector of ColStrs
                                  lpaplChar,                    // Ptr to compiled output
                                  aplDimNRows,                  // # rows
                                  aplDimNCols,                  // # cols
                                  aplRank,                      // Right arg rank
                                  NULL,                         // Ptr to right arg dimensions
                                  FALSE);                       // TRUE iff top level array
            break;

        case IMMTYPE_FLOAT:
            lpaplChar =
              CompileArrFloat   (&(*lpMem)->stData.stFloat,     // Ptr to right arg memory
                                  lpFmtHeader,                  // Ptr to parent header
                                  lpFmtColStr,                  // Ptr to vector of ColStrs
                                  lpaplChar,                    // Ptr to compiled output
                                  aplDimNRows,                  // # rows
                                  aplDimNCols,                  // # cols
                                  aplRank,                      // Right arg rank
                                  NULL,                         // Ptr to right arg dimensions
                                  FALSE);                       // TRUE iff top level array
            break;

        case IMMTYPE_CHAR:
            lpaplChar =
              CompileArrChar    (&(*lpMem)->stData.stChar,      // Ptr to right arg memory
                                  lpFmtHeader,                  // Ptr to parent header
                                  lpFmtColStr,                  // Ptr to vector of ColStrs
                                  lpaplChar,                    // Ptr to compiled output
                                  aplDimNRows,                  // # rows
                                  aplDimNCols,                  // # cols
                                  aplRank,                      // Right arg rank
                                  NULL,                         // Ptr to right arg dimensions
                                  FALSE);                       // TRUE iff top level array
            break;

        defstop
            break;
    } // End SWITCH

    // Set parent's NOTCHAR+ var
    *lpNotCharPlusCur = ((UINT) aplRank) + !IsImmChr (immTypeCon);

    // If not the first col, ...
    if (!b1stCol)
    {
        // Find maximum of NotCharPluses
        uMax = max (*lpNotCharPlusPrv, *lpNotCharPlusCur);

        // Insert intermediate blanks
        lpFmtColStr->uLdBlMax = max (lpFmtColStr->uLdBlMax, uMax);
    } // End IF

    // Pass this data to the next col up
    lpFmtColUp->uInts   = max (lpFmtColUp->uInts,   lpFmtColStr->uInts);
    lpFmtColUp->uChrs   = max (lpFmtColUp->uChrs,   lpFmtColStr->uChrs);
    lpFmtColUp->uFrcs   = max (lpFmtColUp->uFrcs,   lpFmtColStr->uFrcs);
    uMax = max (lpFmtColUp->uInts, lpFmtColUp->uChrs)
         + lpFmtColUp->uFrcs;
    lpFmtColUp->uIntWid = max (lpFmtColUp->uIntWid, uMax);

    // Propagate the row & col count up the line
    PropagateRowColCount (lpFmtHeader);

    return lpaplChar;

#undef  aplRank
#undef  aplDimNCols
#undef  aplDimNRows
} // End CompileArrNestedCon


//***************************************************************************
//  $CompileArrNestedGlb
//
//  Compile a global memory object within a nested array
//***************************************************************************

LPAPLCHAR CompileArrNestedGlb
    (HGLOBAL     hGlb,              // The HGLOBAL to format
     LPFMTHEADER lpFmtHdrUp,        // The parent FMTHEADER
     LPFMTROWSTR lpFmtRowUp,        // This row's FMTROWSTR
     LPFMTCOLSTR lpFmtColUp,        // This col's FMTCOLSTR
     LPUINT      lpNotCharPlusPrv,  // Ptr to prevous NOTCHAR+ var
     LPUINT      lpNotCharPlusCur,  // Ptr to current ...
     LPAPLCHAR   lpaplChar,         // Ptr to next available position
     UBOOL       b1stCol)           // TRUE iff first (leftmost) col

{
    LPFMTHEADER lpFmtHeader;        // Ptr to format header struc
    LPFMTCOLSTR lpFmtColStr;        // This col's FMTCOLSTR
    APLSTYPE    aplType;            // Arg storage type
    APLNELM     aplNELM;            // Arg NELM
    APLRANK     aplRank;            // Arg rank
    LPVOID      lpMem;
    LPAPLDIM    lpMemDim;           // Ptr to arg dimensions
    APLDIM      aplDimNRows,        // # rows
                aplDimNCols,        // # cols
                aplChrNCols;        // # cols for char arrays
    UBOOL       bSimpleScalar;
    APLINT      aplInteger;
    APLFLOAT    aplFloat;
    APLCHAR     aplChar;

    // Get the attributes (Type, NELM, Rank) of the global
    AttrsOfGlb (hGlb, &aplType, &aplNELM, &aplRank, NULL);

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlb);

    // Skip over the header to the dimensions
    lpMemDim = VarArrayBaseToDim (lpMem);

    // Get the # rows & columns in the right arg
    if (IsScalar (aplRank))
        aplDimNCols = aplDimNRows = 1;
    else
    {
        aplDimNCols = lpMemDim[aplRank - 1];

        // Get the # rows (across all planes)
        if (IsVector (aplRank)
         || IsZeroDim (aplDimNCols))
            aplDimNRows = 1;
        else
            aplDimNRows = aplNELM / aplDimNCols;
    } // End IF/ELSE

    // Skip over the header and dimensions to the data
    lpMem = VarArrayDataFmBase (lpMem);

    // Format char arrays as one col
    aplChrNCols = (IsSimpleChar (aplType)) ? 1 : aplDimNCols;

    // Create a new FMTHEADER
    lpFmtHeader = (LPFMTHEADER) lpaplChar;
    ZeroMemory (lpFmtHeader, sizeof (FMTHEADER));
    lpFmtHeader->lpFmtHdrUp  = lpFmtHdrUp;
#ifdef DEBUG
    lpFmtHeader->Sig.nature  = FMTHEADER_SIGNATURE;
#endif
    lpFmtHeader->lpFmtRowUp  = lpFmtRowUp;
    lpFmtHeader->lpFmtColUp  = lpFmtColUp;
////lpFmtHeader->lpFmtRow1st =                      // Filled in below
////lpFmtHeader->lpFmtRowLst =                      // ...
////lpFmtHeader->lpFmtCol1st =                      // ...
////lpFmtHeader->uRealRows   = 0;                   // Already zero from ZeroMemory (initial value)
////lpFmtHeader->uActRows    = 0;                   // ...
    lpFmtHeader->uActCols    = (UINT) aplChrNCols;
////lpFmtHeader->uFmtRows    = 0;                   // Already zero from ZeroMemory (initial value)
////lpFmtHeader->uFmtInts    = 0;                   // ...                           ...
////lpFmtHeader->uFmtChrs    = 0;                   // ...                           ...
////lpFmtHeader->uFmtFrcs    = 0;                   // ...                           ...
////lpFmtHeader->uFmtTrBl    = 0;                   // ...                           ...
    lpFmtHeader->uMatRes     = IsMultiRank (aplRank);

    // Create <aplChrNCols> FMTCOLSTRs in the output
    lpFmtColStr = (LPFMTCOLSTR) (&lpFmtHeader[1]);

    // Create <aplChrNCols> FMTCOLSTRs in the output
    lpFmtHeader->lpFmtCol1st = lpFmtColStr;
    ZeroMemory (lpFmtColStr, (APLU3264) aplChrNCols * sizeof (FMTCOLSTR));
#ifdef DEBUG
    {
        APLDIM uCol;

        for (uCol = 0; uCol < aplChrNCols; uCol++)
            lpFmtColStr[uCol].Sig.nature = FMTCOLSTR_SIGNATURE;
    }
#endif
    // Save ptr to parent col
    lpFmtColStr->lpFmtColUp = lpFmtColUp;

    // Skip over the FMTCOLSTRs to the next available position
    lpaplChar = (LPAPLCHAR) (&lpFmtColStr[aplChrNCols]);

    // Save ptr to 1st and last child FMTROWSTR
    lpFmtHeader->lpFmtRow1st = lpFmtHeader->lpFmtRowLst =
      (IsZeroDim (aplDimNRows) ? NULL
                               : (LPFMTROWSTR) lpaplChar);
    // Loop through the array appending the formatted values (separated by WC_EOS)
    //   to the output vector, and accumulating the values in the appropriate
    //   FMTCOLSTR & FMTROWSTR entries.

    // Is the arg a simple scalar?
    bSimpleScalar = IsSimpleScalar (aplType, aplRank);

    // If the right arg is a simple scalar,
    //   get its value
    if (bSimpleScalar)
        GetFirstValueGlb (hGlb,                 // The global memory handle
                         &aplInteger,           // Ptr to integer result
                         &aplFloat,             // Ptr to float ...
                         &aplChar,              // Ptr to WCHAR ...
                          NULL,                 // Ptr to longest ...
                          NULL,                 // Ptr to lpSym/Glb ...
                          NULL,                 // Ptr to ...immediate type ...
                          NULL,                 // Ptr to array type ...
                          TRUE);                // TRUE iff we should recurse through LPSYMENTRYs
    // Split cases based upon the right arg's storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
            lpaplChar =
              CompileArrBool    (bSimpleScalar ? (LPAPLBOOL) &aplInteger
                                               : (LPAPLBOOL)  lpMem,    // Ptr to right arg memory
                                 lpFmtHeader,                           // Ptr to parent header
                                 lpFmtColStr,                           // Ptr to vector of ColStrs
                                 lpaplChar,                             // Ptr to compiled output
                                 aplDimNRows,                           // # rows
                                 aplDimNCols,                           // # cols
                                 aplRank,                               // Right arg rank
                                 lpMemDim,                              // Ptr to right arg dimensions
                                 FALSE);                                // TRUE iff top level array
            break;

        case ARRAY_INT:
            lpaplChar =
              CompileArrInteger (bSimpleScalar ? &aplInteger
                                               : (LPAPLINT)   lpMem,    // Ptr to right arg memory
                                 lpFmtHeader,                           // Ptr to parent header
                                 lpFmtColStr,                           // Ptr to vector of ColStrs
                                 lpaplChar,                             // Ptr to compiled output
                                 aplDimNRows,                           // # rows
                                 aplDimNCols,                           // # cols
                                 aplRank,                               // Right arg rank
                                 lpMemDim,                              // Ptr to right arg dimensions
                                 FALSE);                                // TRUE iff top level array
            break;

        case ARRAY_FLOAT:
            lpaplChar =
              CompileArrFloat   (bSimpleScalar ? &aplFloat
                                               : (LPAPLFLOAT) lpMem,    // Ptr to right arg memory
                                 lpFmtHeader,                           // Ptr to parent header
                                 lpFmtColStr,                           // Ptr to vector of ColStrs
                                 lpaplChar,                             // Ptr to compiled output
                                 aplDimNRows,                           // # rows
                                 aplDimNCols,                           // # cols
                                 aplRank,                               // Right arg rank
                                 lpMemDim,                              // Ptr to right arg dimensions
                                 FALSE);                                // TRUE iff top level array
            break;

        case ARRAY_CHAR:
            lpaplChar =
              CompileArrChar    (bSimpleScalar ? &aplChar
                                               : (LPAPLCHAR)  lpMem,    // Ptr to right arg memory
                                 lpFmtHeader,                           // Ptr to parent header
                                 lpFmtColStr,                           // Ptr to vector of ColStrs
                                 lpaplChar,                             // Ptr to compiled output
                                 aplDimNRows,                           // # rows
                                 aplDimNCols,                           // # cols
                                 aplRank,                               // Right arg rank
                                 lpMemDim,                              // Ptr to right arg dimensions
                                 FALSE);                                // TRUE iff top level array
            break;

        case ARRAY_APA:
            lpaplChar =
              CompileArrAPA     ((LPAPLAPA)    lpMem,                   // Ptr to right arg memory
                                 lpFmtHeader,                           // Ptr to parent header
                                 lpFmtColStr,                           // Ptr to vector of ColStrs
                                 lpaplChar,                             // Ptr to compiled output
                                 aplDimNRows,                           // # rows
                                 aplDimNCols,                           // # cols
                                 aplRank,                               // Right arg rank
                                 lpMemDim,                              // Ptr to right arg dimensions
                                 FALSE);                                // TRUE iff top level array
            break;

        case ARRAY_HETERO:
            lpaplChar =
              CompileArrHetero  ((LPAPLHETERO) lpMem,                   // Ptr to right arg memory
                                 lpFmtHeader,                           // Ptr to parent header
                                 lpFmtColStr,                           // Ptr to vector of ColStrs
                                 lpaplChar,                             // Ptr to compiled output
                                 aplDimNRows,                           // # rows
                                 aplDimNCols,                           // # cols
                                 aplRank,                               // Right arg rank
                                 lpMemDim,                              // Ptr to right arg dimensions
                                 FALSE,                                 // TRUE iff top level array
                                 TRUE);                                 // TRUE iff handle []TCLF specially
            break;

        case ARRAY_NESTED:
            lpaplChar =
              CompileArrNested  ((LPAPLNESTED) lpMem,                   // Ptr to right arg memory
                                 lpFmtHeader,                           // Ptr to parent header
                                 lpFmtColStr,                           // Ptr to vector of ColStrs
                                 lpaplChar,                             // Ptr to compiled output
                                 aplDimNRows,                           // # rows
                                 aplDimNCols,                           // # cols
                                 aplRank,                               // Right arg rank
                                 lpMemDim,                              // Ptr to right arg dimensions
                                 FALSE);                                // TRUE iff top level array
            break;

        case ARRAY_RAT:
            lpaplChar =
              CompileArrRat     ((LPAPLRAT)    lpMem,                   // Ptr to right arg memory
                                 lpFmtHeader,                           // Ptr to parent header
                                 lpFmtColStr,                           // Ptr to vector of ColStrs
                                 lpaplChar,                             // Ptr to compiled output
                                 aplDimNRows,                           // # rows
                                 aplDimNCols,                           // # cols
                                 aplRank,                               // Right arg rank
                                 lpMemDim,                              // Ptr to right arg dimensions
                                 FALSE);                                // TRUE iff top level array
            break;

        case ARRAY_VFP:
            lpaplChar =
              CompileArrVfp     ((LPAPLVFP)    lpMem,                   // Ptr to right arg memory
                                 lpFmtHeader,                           // Ptr to parent header
                                 lpFmtColStr,                           // Ptr to vector of ColStrs
                                 lpaplChar,                             // Ptr to compiled output
                                 aplDimNRows,                           // # rows
                                 aplDimNCols,                           // # cols
                                 aplRank,                               // Right arg rank
                                 lpMemDim,                              // Ptr to right arg dimensions
                                 FALSE);                                // TRUE iff top level array
            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlb); lpMem = NULL;

    // Set parent's NOTCHAR+ var
    *lpNotCharPlusCur = ((UINT) aplRank) + !IsSimpleChar (aplType);

    // If not the first col, ...
    if (!b1stCol)
    {
        UINT uMax;              // Maximum of NotCharPluses

        // Find maximum of NotCharPluses
        uMax = max (*lpNotCharPlusPrv, *lpNotCharPlusCur);

        // Insert intermediate blanks
        lpFmtColStr->uLdBlMax = max (lpFmtColStr->uLdBlMax, uMax);
    } // End IF

    // Propagate the row & col count up the line
    PropagateRowColCount (lpFmtHeader);

    return lpaplChar;
} // End CompileArrNestedGlb


//***************************************************************************
//  $CompileArrRat
//
//  Compile an array of Rationals
//***************************************************************************

LPAPLCHAR CompileArrRat
    (LPAPLRAT    lpMem,         // Ptr to data to format
     LPFMTHEADER lpFmtHeader,   // Ptr to parent FMTHEADER
     LPFMTCOLSTR lpFmtColStr,   // Ptr to vector of <aplDimNCols> FMTCOLSTRs
     LPAPLCHAR   lpaplChar,     // Ptr to next available format position
     APLDIM      aplDimNRows,   // # rows to format (actually it's x/ all but last dim)
     APLDIM      aplDimNCols,   // # cols to format
     APLRANK     aplRank,       // Rank of data to format
     LPAPLDIM    lpMemDim,      // Ptr to dimension vector
     UBOOL       bTopLevel)     // TRUE iff top level array

{
    UINT        uLen;               // Length of integer part
    APLDIM      aplDimCol,          // Loop counter
                aplDimRow;          // ...
    LPAPLCHAR   lpwszOut;           // Ptr to output buffer
    LPWCHAR     lpwsz;
    LPFMTROWSTR lpFmtRowLcl = NULL; // Ptr to local FMTROWSTR

    // Loop through the cols, setting the column type
    for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        // Set column type
        lpFmtColStr[aplDimCol].colType = COLTYPE_NOTCHAR;

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
        ZeroMemory (lpFmtRowLcl, sizeof (lpFmtRowLcl[0]));
#ifdef DEBUG
        lpFmtRowLcl->Sig.nature  = FMTROWSTR_SIGNATURE;
        lpFmtRowLcl->lpFmtColUp  = lpFmtColStr;
#endif
////////lpFmtRowLcl->uAccWid     = 0;               // Initialize the accumulated width
        lpFmtRowLcl->uFmtRows    = 1;               // Initialize the count
////////lpFmtRowLcl->bRptCol     = FALSE;           // Initialize as not repeating a col
////////lpFmtRowLcl->bDone       = FALSE;           // Initialize as not done with output
        lpFmtRowLcl->bRealRow    = TRUE;            // Initialize as a real row
////////lpFmtRowLcl->bBlank      = FALSE;           // Initialize as not all blank
////////lpFmtRowLcl->uColOff     = 0;               // Initialize the col offset
////////lpFmtRowLcl->uItemCount  = 0;               // Initialize the # items to follow
////////lpFmtRowLcl->lpFmtRowNxt = NULL;            // Initialize the ptr
        lpFmtRowLcl->aplType     = ARRAY_RAT;       // Initialize the storage type

        // Link into the row chain
        lpFmtHeader->lpFmtRowLst = lpFmtRowLcl;

        // Skip over the FMTROWSTR to the next available position
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        // Save the ptr to the 1st value
        lpFmtRowLcl->lpNxtChar   = lpaplChar;

        // Loop through the cols
        for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        {
            // Format the Rational
            lpaplChar =
              FormatAplRat (lpwszOut = lpaplChar,   // Ptr to output save area
                           *lpMem++);               // The value to format
            // Zap the trailing blank
            lpaplChar[-1] = WC_EOS;

            // Count in another item
            lpFmtRowLcl->uItemCount++;

            // Include a leading blank if not 1st col
            uLen = (aplDimCol NE 0);

            // Max the current leading blanks with this
            lpFmtColStr[aplDimCol].uLdBlMax = max (lpFmtColStr[aplDimCol].uLdBlMax, uLen);

            // Check for rational separator
            lpwsz = strchrW (lpwszOut, DEF_RATSEP);
            if (lpwsz)
            {
                // Calculate the length of the numerator
                uLen = (UINT) (lpwsz - lpwszOut);

                // Max the current integer width with this
                lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);

                // Calculate the length of the denominator
                //   including the rational separator
                uLen = (UINT) (lpaplChar - lpwsz) - 1;

                // Max the current fractional width with this
                lpFmtColStr[aplDimCol].uFrcs = max (lpFmtColStr[aplDimCol].uFrcs, uLen);
            } else  // No rational separator
            {
                // Calculate the length of the numerator
                uLen = (UINT) (lpaplChar - lpwszOut) - 1;

                // Max the current integer width with this
                lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);

////////////////// Calculate the length of the denominator
////////////////uLen = 0;
////////////////
////////////////// Max the current fractional width with this
////////////////lpFmtColStr[aplDimCol].uFrcs = max (lpFmtColStr[aplDimCol].uFrcs, uLen);
            } // End IF/ELSE

////////////// Include a trailing blank if nested and last col
////////////uLen = 0;
////////////
////////////// Max the current trailing blanks with this
////////////lpFmtColStr[aplDimCol].uTrBl = max (lpFmtColStr[aplDimCol].uTrBl, uLen);
        } // End FOR

        // Save the ptr to the terminating zero
        lpFmtRowLcl->lpEndChar   = lpaplChar - 1;

        // Save as ptr to next row struc
        lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;

        // If this is not at the top level and not the last row,
        //   we need to count the interplanar spacing
        //   as blank rows in the row count
        if ((!bTopLevel)                        // Not top level
         && aplDimRow NE (aplDimNRows - 1))     // Not last row
            lpaplChar = CompileBlankRows (lpaplChar,        // Ptr to output buffer
                                          lpMemDim,         // Ptr to item dimensions
                                          0,                // Maximum row rank of next row
                                          aplRank,          // Item rank
                                          aplDimRow,        // Item row #
                                          lpFmtColStr);     // Ptr to item FMTCOLSTR
    } // End FOR

    // Loop through the cols, ...
    for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        // Set the interior width
        lpFmtColStr[aplDimCol].uIntWid = lpFmtColStr[aplDimCol].uInts
                                       + lpFmtColStr[aplDimCol].uFrcs;
    // Mark as last row struc
    if (lpFmtRowLcl)
        lpFmtRowLcl->lpFmtRowNxt = NULL;

    // Propagate the row & col count up the line
    PropagateRowColCount (lpFmtHeader);

    return lpaplChar;
} // End CompileArrRat


//***************************************************************************
//  $CompileArrVfp
//
//  Compile an array of VFPs
//***************************************************************************

LPAPLCHAR CompileArrVfp
    (LPAPLVFP    lpMem,         // Ptr to data to format
     LPFMTHEADER lpFmtHeader,   // Ptr to parent FMTHEADER
     LPFMTCOLSTR lpFmtColStr,   // Ptr to vector of <aplDimNCols> FMTCOLSTRs
     LPAPLCHAR   lpaplChar,     // Ptr to next available format position
     APLDIM      aplDimNRows,   // # rows to format (actually it's x/ all but last dim)
     APLDIM      aplDimNCols,   // # cols to format
     APLRANK     aplRank,       // Rank of data to format
     LPAPLDIM    lpMemDim,      // Ptr to dimension vector
     UBOOL       bTopLevel)     // TRUE iff top level array

{
    UINT        uLen;
    APLDIM      aplDimCol,
                aplDimRow;
    LPAPLCHAR   lpwszOut;
    LPWCHAR     lpwsz;
    LPFMTROWSTR lpFmtRowLcl = NULL;
    APLUINT     uQuadPP;        // []PP save area

    // Get the current value of []PP for VFP
    uQuadPP  = GetQuadPPV ();

    // Loop through the cols, setting the column type
    for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        // Set column type
        lpFmtColStr[aplDimCol].colType = COLTYPE_NOTCHAR;

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
        ZeroMemory (lpFmtRowLcl, sizeof (lpFmtRowLcl[0]));
#ifdef DEBUG
        lpFmtRowLcl->Sig.nature  = FMTROWSTR_SIGNATURE;
        lpFmtRowLcl->lpFmtColUp  = lpFmtColStr;
#endif
////////lpFmtRowLcl->uAccWid     = 0;               // Initialize the accumulated width
        lpFmtRowLcl->uFmtRows    = 1;               // Initialize the count
////////lpFmtRowLcl->bRptCol     = FALSE;           // Initialize as not repeating a col
////////lpFmtRowLcl->bDone       = FALSE;           // Initialize as not done with output
        lpFmtRowLcl->bRealRow    = TRUE;            // Initialize as a real row
////////lpFmtRowLcl->bBlank      = FALSE;           // Initialize as not all blank
////////lpFmtRowLcl->uColOff     = 0;               // Initialize the col offset
////////lpFmtRowLcl->uItemCount  = 0;               // Initialize the # items to follow
////////lpFmtRowLcl->lpFmtRowNxt = NULL;            // Initialize the ptr
        lpFmtRowLcl->aplType     = ARRAY_VFP;       // Initialize the storage type

        // Link into the row chain
        lpFmtHeader->lpFmtRowLst = lpFmtRowLcl;

        // Skip over the FMTROWSTR to the next available position
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        // Save the ptr to the 1st value
        lpFmtRowLcl->lpNxtChar   = lpaplChar;

        // Loop through the cols
        for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        {
            // Format the VFP
            lpaplChar =
              FormatAplVfp (lpwszOut = lpaplChar,   // Ptr to output save area
                           *lpMem++,                // The value for format
                            uQuadPP);               // Use this many significant digits
            // Zap the trailing blank
            lpaplChar[-1] = WC_EOS;

            // Count in another item
            lpFmtRowLcl->uItemCount++;

            // Include a leading blank if not 1st col
            uLen = (aplDimCol NE 0);

            // Max the current leading blanks with this
            lpFmtColStr[aplDimCol].uLdBlMax = max (lpFmtColStr[aplDimCol].uLdBlMax, uLen);

            // Check for decimal point
            lpwsz = strchrW (lpwszOut, L'.');
            if (lpwsz)
            {
                // Calculate the length of the integer part
                uLen = (UINT) (lpwsz - lpwszOut);

                // Max the current integer width with this
                lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);

                // Calculate the length of the fractional part
                //   including the decimal point
                uLen = (UINT) (lpaplChar - lpwsz) - 1;

                // Max the current fractional width with this
                lpFmtColStr[aplDimCol].uFrcs = max (lpFmtColStr[aplDimCol].uFrcs, uLen);
            } else  // No decimal point
            {
                // Calculate the length of the integer part
                uLen = (UINT) (lpaplChar - lpwszOut) - 1;

                // Max the current integer width with this
                lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);

////////////////// Calculate the length of the fractional part
////////////////uLen = 0;
////////////////
////////////////// Max the current fractional width with this
////////////////lpFmtColStr[aplDimCol].uFrcs = max (lpFmtColStr[aplDimCol].uFrcs, uLen);
            } // End IF/ELSE

////////////// Include a trailing blank if nested and last col
////////////uLen = 0;
////////////
////////////// Max the current trailing blanks with this
////////////lpFmtColStr[aplDimCol].uTrBl = max (lpFmtColStr[aplDimCol].uTrBl, uLen);
        } // End FOR

        // Save the ptr to the terminating zero
        lpFmtRowLcl->lpEndChar   = lpaplChar - 1;

        // Save as ptr to next row struc
        lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;

        // If this is not at the top level and not the last row,
        //   we need to count the interplanar spacing
        //   as blank rows in the row count
        if ((!bTopLevel)                        // Not top level
         && aplDimRow NE (aplDimNRows - 1))     // Not last row
            lpaplChar = CompileBlankRows (lpaplChar,        // Ptr to output buffer
                                          lpMemDim,         // Ptr to item dimensions
                                          0,                // Maximum row rank of next row
                                          aplRank,          // Item rank
                                          aplDimRow,        // Item row #
                                          lpFmtColStr);     // Ptr to item FMTCOLSTR
    } // End FOR

    // Loop through the cols, ...
    for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        // Set the interior width
        lpFmtColStr[aplDimCol].uIntWid = lpFmtColStr[aplDimCol].uInts
                                       + lpFmtColStr[aplDimCol].uFrcs;
    // Mark as last row struc
    if (lpFmtRowLcl)
        lpFmtRowLcl->lpFmtRowNxt = NULL;

    // Propagate the row & col count up the line
    PropagateRowColCount (lpFmtHeader);

    return lpaplChar;
} // End CompileArrVfp


//***************************************************************************
//  $InteriorColWidth
//
//  Calculate the interior width of a col
//***************************************************************************

UINT InteriorColWidth
    (LPFMTCOLSTR lpFmtColStr)

{
    return lpFmtColStr->uIntWid;
} // End InteriorColWidth


//***************************************************************************
//  $InteriorColWidthExplicit
//
//  Calculate the interior width of a col
//    using explicit values as uIntWid might not have been set as yet.
//***************************************************************************

UINT InteriorColWidthExplicit
    (LPFMTCOLSTR lpFmtColStr)

{
    return max (lpFmtColStr->uInts, lpFmtColStr->uChrs) + lpFmtColStr->uFrcs;
} // End InteriorColWidthExplicit


//***************************************************************************
//  $LftExteriorColWidth
//
//  Calculate the left exterior width of a col
//***************************************************************************

UINT LftExteriorColWidth
    (LPFMTCOLSTR lpFmtColStr)

{
    // Return leading blanks
    return lpFmtColStr->uLdBlNst + lpFmtColStr->uLdBlMax;
} // End LftExteriorColWidth


//***************************************************************************
//  $RhtExteriorColWidth
//
//  Calculate the right exterior width of a col
//***************************************************************************

UINT RhtExteriorColWidth
    (LPFMTCOLSTR lpFmtColStr)

{
    // Return interior width + trailing blanks
    return InteriorColWidth (lpFmtColStr) + lpFmtColStr->uTrBlNst;
} // End RhtExteriorColWidth


//***************************************************************************
//  $ExteriorColWidth
//
//  Calculate the exterior width of a col
//***************************************************************************

UINT ExteriorColWidth
    (LPFMTCOLSTR lpFmtColStr)

{
    // Return Left plus Right exterior widths
    return LftExteriorColWidth (lpFmtColStr) + RhtExteriorColWidth (lpFmtColStr);
} // End ExteriorColWidth


//***************************************************************************
//  $InteriorHdrWidth
//
//  Calculate the interior width of a header
//***************************************************************************

UINT InteriorHdrWidth
    (LPFMTHEADER lpFmtHeader)

{
    return lpFmtHeader->uFmtIntWid;
} // End InteriorHdrWidth


//***************************************************************************
//  $ExteriorHdrWidth
//
//  Calculate the exterior width of a header
//***************************************************************************

UINT ExteriorHdrWidth
    (LPFMTHEADER lpFmtHeader)

{
    // Return leading blanks + interior width + trailing blanks
    return lpFmtHeader->uFmtLdBl + InteriorHdrWidth (lpFmtHeader) + lpFmtHeader->uFmtTrBl;
} // End ExteriorHdrWidth


//***************************************************************************
//  $PropagateRowColCount
//
//  Propagate the row & col count up the line
//***************************************************************************

void PropagateRowColCount
    (LPFMTHEADER lpFmtHeader)       // Ptr to FMTHEADER struc

{
    UINT        uActRows,           // Accumulator for actual    rows
                uFmtRows,           // ...             formatted rows
                uIntWid,            // ...             interior width
                uActCols,           // # active cols
                uCol;               // Loop counter
    LPFMTROWSTR lpFmtRowLcl;        // Ptr to local FMTROWSTR
    LPFMTCOLSTR lpFmtColLcl;        // Ptr to local FMTCOLSTR

    // Count the # actual & formatted rows in this block
    for (lpFmtRowLcl = lpFmtHeader->lpFmtRow1st,
           uActRows = uFmtRows = 0;
         lpFmtRowLcl NE NULL;
         lpFmtRowLcl = lpFmtRowLcl->lpFmtRowNxt,
           uActRows++)
        uFmtRows += lpFmtRowLcl->uFmtRows;

    // Save in head struc
    if (lpFmtHeader->lpFmtRowUp)
        lpFmtHeader->lpFmtRowUp->uFmtRows = max (lpFmtHeader->lpFmtRowUp->uFmtRows, uFmtRows);

    lpFmtHeader->uActRows = uActRows;
    lpFmtHeader->uFmtRows = max (lpFmtHeader->uFmtRows, uFmtRows);

    uActCols = lpFmtHeader->uActCols;
    lpFmtColLcl = lpFmtHeader->lpFmtCol1st;

    // Count the col width in this block
    for (uIntWid = uCol = 0;
         uCol < uActCols;
         uCol++, lpFmtColLcl++)
        uIntWid += lpFmtColLcl->uLdBlMax
                 + lpFmtColLcl->uLdBlNst
                 + lpFmtColLcl->uIntWid
                 + lpFmtColLcl->uTrBlNst;
    // Save in col struc
    if (lpFmtHeader->lpFmtColUp)
    {
        COLTYPES colType;

        lpFmtColLcl = lpFmtHeader->lpFmtCol1st;

        // If there's only one col, and
        //   there are leading NCP blanks, and
        //   the cols are at the same depth, ...
        if (uActCols EQ 1
         && lpFmtColLcl->uLdBlMax)
        {
            UINT uLdBlMax;

            uLdBlMax = max (lpFmtColLcl->lpFmtColUp->uLdBlMax, lpFmtColLcl->uLdBlMax);

            // Transfer them upstairs
            lpFmtColLcl->lpFmtColUp->uLdBlMax = uLdBlMax;

            // Account for them in the upstairs interior width
            lpFmtColLcl->lpFmtColUp->uIntWid -= (uLdBlMax - lpFmtColLcl->uLdBlMax);

            // Remove from the interior width
            uIntWid -= lpFmtColLcl->uLdBlMax;

            // Clear them locally
            lpFmtColLcl->uLdBlMax = 0;
        } // End IF

        lpFmtHeader->lpFmtColUp->uIntWid = max (lpFmtHeader->lpFmtColUp->uIntWid, uIntWid);

        // Loop through all columns in this block
        for (uCol = 0,
               colType = COLTYPE_UNK,
               lpFmtColLcl = lpFmtHeader->lpFmtCol1st;
             uCol < lpFmtHeader->uActCols;
             uCol++, lpFmtColLcl++)
            // Calculate the common column type
            colType = max (lpFmtColLcl->colType, colType);

        // Save in next column up
        lpFmtHeader->lpFmtColUp->colType =
          max (lpFmtHeader->lpFmtColUp->colType, colType);
    } // End IF

    // Save in head struc
    lpFmtHeader->uFmtIntWid = max (lpFmtHeader->uFmtIntWid, uIntWid);

    // Propagate the uMatRes bit
    if (lpFmtHeader->lpFmtHdrUp)
        lpFmtHeader->lpFmtHdrUp->uMatRes |= lpFmtHeader->uMatRes;

    // Recurse
    if (lpFmtHeader->lpFmtHdrUp)
        PropagateRowColCount (lpFmtHeader->lpFmtHdrUp);
} // End PropagateRowColCount


//***************************************************************************
//  $CompileBlankRows
//
//  Compile blank rows to account for interdimensional spacing
//***************************************************************************

LPAPLCHAR CompileBlankRows
    (LPAPLCHAR   lpaplChar,             // Ptr to output buffer
     LPAPLDIM    lpMemDim,              // Ptr to item dimensions
     APLRANK     aplRowRank,            // Maximum row rank of next row
     APLRANK     aplRank,               // Item rank
     APLDIM      aplDimRow,             // Item row #
     LPFMTCOLSTR lpFmtColStr)           // Ptr to item FMTCOLSTR

{
    APLDIM      aplDimAcc,              // Dimension product accumulator
                aplDimCol;              // Loop counter

    // Initialize the product accumulator
    aplDimAcc = 1;

    // Loop through all but the last dimension
    for (aplDimCol = 0; aplDimCol < (aplRank - 1); aplDimCol++)
    {
        aplDimAcc *= lpMemDim[(aplRank - 2) - aplDimCol];
        if ((aplDimRow + 1) % aplDimAcc)
            break;

        // Insert a row of blanks
        lpaplChar =
          InsertBlankRow (lpaplChar,    // Ptr to output buffer
                          lpFmtColStr,  // Ptr to item FMTCOLSTR
                          1);           // # rows to insert
    } // End FOR

    // If the maximum row rank is > 1, ...
    if (IsMultiRank (aplRowRank))
        // Insert <aplRowRank - 1> row(s) of blanks
        lpaplChar =
          InsertBlankRow (lpaplChar,        // Ptr to output buffer
                          lpFmtColStr,      // Ptr to item FMTCOLSTR
                          aplRowRank - 1);  // # rows to insert
    return lpaplChar;
} // End CompileBlankRows


//***************************************************************************
//  $InsertBlankRow
//
//  Insert a row of blanks
//***************************************************************************

LPAPLCHAR InsertBlankRow
    (LPAPLCHAR   lpaplChar,             // Ptr to output buffer
     LPFMTCOLSTR lpFmtColStr,           // Ptr to item FMTCOLSTR
     APLUINT     aplNumRows)            // # rows to insert

{
    LPFMTROWSTR lpFmtRowLcl;            // Ptr to new row struc

    while (aplNumRows--)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
        ZeroMemory (lpFmtRowLcl, sizeof (lpFmtRowLcl[0]));
#ifdef DEBUG
        lpFmtRowLcl->Sig.nature  = FMTROWSTR_SIGNATURE;
        lpFmtRowLcl->lpFmtColUp  = lpFmtColStr;
#endif
////////lpFmtRowLcl->uAccWid     = 0;               // Initialize the accumulated width
        lpFmtRowLcl->uFmtRows    = 1;               // Initialize the count
////////lpFmtRowLcl->bRptCol     = FALSE;           // Initialize as not repeating a col
////////lpFmtRowLcl->bDone       = FALSE;           // Initialize as not done with output
////////lpFmtRowLcl->bRealRow    = FALSE;           // Initialize as not a real row
        lpFmtRowLcl->bBlank      = TRUE;            // Initialize as a blank row
////////lpFmtRowLcl->uColOff     = 0;               // Initialize the col offset
////////lpFmtRowLcl->uItemCount  = 0;               // Initialize the # items to follow
////////lpFmtRowLcl->lpFmtRowNxt = NULL;            // Initialize the ptr (filled in below)

        // Skip over the FMTROWSTR to the next available position
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        // Save as ptr to next row struc
        lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;

        // Save the ptr to the 1st value
        lpFmtRowLcl->lpNxtChar   = lpaplChar;

        // Save the ptr to the terminating zero
        lpFmtRowLcl->lpEndChar   = lpaplChar - 1;
    } // End WHILE

    return lpaplChar;
} // End InsertBlankRow


//***************************************************************************
//  $AppendBlankRows
//
//  Append blank rows via AppendLine
//***************************************************************************

void AppendBlankRows
    (APLRANK  aplRank,          // Arg rank
     APLDIM   aplDimRow,        // Arg row count
     LPAPLDIM lpMemDim,         // Ptr to arg dimensions
     LPUBOOL  lpbCtrlBreak)     // Ptr to Ctrl-Break flag

{
    APLDIM aplDimAcc,           // Dimension product accumulator
           aplDimCol;           // Loop counter

    // Initialize the product accumulator
    aplDimAcc = 1;

    // Loop through all but the last dimension
    for (aplDimCol = 0; aplDimCol < (aplRank - 1); aplDimCol++)
    {
        aplDimAcc *= lpMemDim[(aplRank - 2) - aplDimCol];
        if ((aplDimRow - 1) % aplDimAcc)
            break;

        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            return;

        AppendLine (L"", FALSE, TRUE);
    } // End FOR
} // End AppendBlankRows


//***************************************************************************
//  $FormatArrSimple
//
//  Format a simple array (this includes hetero) from its compiled form
//***************************************************************************

LPAPLCHAR FormatArrSimple
    (LPFMTHEADER lpFmtHeader,       // Ptr to FMTHEADER
     LPFMTCOLSTR lpFmtColStr,       // Ptr to vector of <aplChrNCols> FMTCOLSTRs
     LPAPLCHAR   lpaplChar2,        // Ptr to compiled input
     LPAPLCHAR  *lplpwszOut,        // Ptr to ptr to output string
     APLDIM      aplDimNRows,       // # formatted rows in this array
     APLDIM      aplDimNCols,       // # formatted cols ...
     APLDIM      aplLastDim,        // Length of the last dimension in the result
     APLRANK     aplRank,           // Rank of this array
     LPAPLDIM    lpMemDim,          // Ptr to this array's dimensions (NULL for scalar)
     APLSTYPE    aplType,           // Storage type of this array
     UBOOL       bNextRow,          // TRUE iff skip to next row after this item
     UBOOL       bRawOutput,        // TRUE iff raw (not {thorn}) output
     UBOOL       bEndingCR,         // TRUE iff last line has CR
     LPUBOOL     lpbCtrlBreak)      // Ptr to Ctrl-Break flag

{
    APLDIM      aplDimRow,          // Loop counter
                aplDimCol,          // Loop counter
                aplChrNCols,        // # cols for char arrays
                aplRealNRows,       // # real rows
                aplRealRow;         // Loop counter
    APLUINT     uActLen,            // Actual length
                uLead,              // # leading blanks
                uColTmp,            // Column temp
                uColIndent = 0,     // Column indent
                uCmpWid,            // Compiled width
                uColBeg,            // Beginning col to format
                uColLim,            // Ending + 1...
                uColOff;            // Current row's col offset
    LPWCHAR     lpwszOut,           // Ptr to local output string
                lpwszOutStart;      // Ptr to starting output string
    LPFMTROWSTR lpFmtRowStr,        // Ptr to this item's FMTROWSTR
                lpFmt1stRowStr;     // ...    the  1st    ...
    LPAPLCHAR   lpaplChar = lpaplChar2; // Ptr to moving input string
    APLUINT     uQuadPW;            // []PW
    WCHAR       wcSep;              // Decimal or rational separator
    UBOOL       bRptCol,            // TRUE iff we're to repeat a col
                bMoreCols;          // TRUE iff we're to process more cols of data in a row
    APLSTYPE    aplItmType;         // Item type if array type is hetero

    // Get the current value of []PW
    uQuadPW = GetQuadPW ();

    // Initialize local output string ptr
    lpwszOut = *lplpwszOut;

    // Initialize the # real rows
    aplRealNRows = lpFmtHeader->uRealRows;

    // Format char arrays as one col
    aplChrNCols = (IsSimpleChar (aplType)) ? 1 : aplDimNCols;

    // Set the decimal or rational separator

    // Split cases based upon the array storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
        case ARRAY_FLOAT:
        case ARRAY_VFP:
        case ARRAY_HETERO:
            wcSep = L'.';

            break;

        case ARRAY_CHAR:
            wcSep = UTF16_REPLACEMENTCHAR;

            break;

        case ARRAY_RAT:
            wcSep = DEF_RATSEP;

            break;

        defstop
            break;
    } // End SWITCH

    // Initialize the starting col
    uColBeg = 0;
    bRptCol = bMoreCols = FALSE;
    aplItmType = aplType;

    // Point to the FMTROWSTR
    lpFmt1stRowStr = (LPFMTROWSTR) lpaplChar;

    /* The following code is designed to display a multi-row array keeping the rows
       together displaying as many cols as fit within []PW followed by a blank line
       and more cols, etc.  The idea came from Larry Breed sometime in the early
       1980s (personal email communication 10/20/2012).
     */

    // Display cols until done
    while (TRUE)
    {
        // If we're doing raw (not {thorn}) output, ...
        if (bRawOutput)
        {
            UINT uColWidth = 0;

            // If the array is simple char, ...
            if (IsSimpleChar (aplType))
            {
                uColBeg = 0;
                uColLim = aplChrNCols;
            } else
            // Scan the column strucs for the # cols to output this time
            for (uColLim = uColBeg; uColLim < aplChrNCols; uColLim++)
            {
                uColWidth += ExteriorColWidth (&lpFmtColStr[uColLim]);
                if (uColWidth > (uQuadPW - uColIndent))
                    break;
            } // End FOR

            // Ensure at least one col processed
            //   e.g. when the first element exceeds []PW
            if (uColBeg EQ uColLim)
                uColLim = uColBeg + (lpFmtHeader->uActCols NE 0);

            // Format the cols from uColBeg up to but not including uColLim, all rows
        } else
            uColLim = aplChrNCols;

        // Point to the 1st FMTROWSTR
        lpFmtRowStr = lpFmt1stRowStr;

        // Loop through the formatted rows
        for (aplDimRow = aplRealRow = 0;
             aplDimRow < aplDimNRows;
             aplDimRow++)
        {
            UBOOL bRealRow;         // TRUE iff this is a real row

            // Validate the FMTROWSTR
            Assert (lpFmtRowStr->Sig.nature EQ FMTROWSTR_SIGNATURE);

            // It's a simple array
            Assert (lpFmtRowStr->uFmtRows EQ 1);

            // Accumulate the # real rows so far
            bRealRow = lpFmtRowStr->bRealRow;
            aplRealRow += bRealRow;

            // Save this row's col offset
            uColOff = lpFmtRowStr->uColOff;

            // Save starting output string ptr
            lpwszOutStart = lpwszOut;

            // Point to next entry
            lpaplChar = lpFmtRowStr->lpNxtChar;

            // Handle non-blank rows
            //   and non-empty cols
            if (!lpFmtRowStr->bBlank
             && !lpFmtRowStr->bDone
             && !IsZeroDim (uColLim))
            {
                if (GetHetChar (lpaplChar, aplType) NE WC_EOS)
                {
                    // If this row's col offset is non-zero, ...
                    if (uColOff)
                        // Fill with leading blanks
                        lpwszOut = FillMemoryW (lpwszOut, (APLU3264) uColOff, L' ');

                    // Loop through the cols
                    for (aplDimCol = uColBeg; aplDimCol < uColLim; aplDimCol++)
                    {
                        LPWCHAR lpw,
                                lpwDec,
                                lpwExp;
                        APLUINT uAlign;         // # blanks to align decimal points and exponents

                        // If hetero (hence, storage type precedes value), ...
                        if (IsSimpleHet (aplType))
                            // Save and skip over the item type
                            aplItmType = *lpaplChar++;

                        uCmpWid = InteriorColWidth (&lpFmtColStr[aplDimCol]);   // Compiled width
                        uActLen = lstrlenW (lpaplChar);                         // Actual length

                        Assert (uActLen <= (uCmpWid - lpFmtRowStr->uAccWid));

                        // Check for fractional part unless char
                        //   or we're repeating the item in this col
                        if (IsSimpleChar (aplType)
                         || lpFmtRowStr->bRptCol)
                            // Left-justify
                            uAlign = 0;
                        else
                        if (IsSimpleChar (aplItmType))
                            // Right-justify
                            uAlign = max (uCmpWid, 1) - 1;
                        else
                        {
                            lpwDec = SkipToCharW (lpaplChar, wcSep);
                            lpwExp = SkipToCharW (lpaplChar, DEF_EXPONENT_UC);

                            // Use the earlier value
                            lpw = min (lpwDec, lpwExp);

                            // Align the decimal points and the exponents (if no decimal point)
                            uColTmp = (UINT) (lpw - lpaplChar) + lpFmtColStr[aplDimCol].uFrcs;

                            // # leading blanks to align decimal points
                            // These blanks are already accounted for in uCmpWid
                            uAlign = uCmpWid - max (uActLen, uColTmp);
                        } // End IF/ELSE

                        // Plus leading blanks
                        uLead = lpFmtColStr[aplDimCol].uLdBlMax + uAlign;

                        if (IsSimpleChar (aplItmType))
                            // Calculate # leading blanks
                            uColTmp = uColIndent + uLead;
                        else
                            // Calculate # leading blanks
                            //            2nd or subseq    first entry in the row
                            //              row group
                            uColTmp = ((lpFmtRowStr->bRptCol || ((uColBeg > 0) && (aplDimCol EQ uColBeg))) ? uColIndent
                                                                                                           : 0) + uLead;
                        // Fill with leading blanks
                        lpwszOut = FillMemoryW (lpwszOut, (APLU3264) uColTmp, L' ');

                        // If this is raw output, ...
                        if (bRawOutput)
                        {
                            APLUINT uTmp;

                            // If the next row is not a real row, ...
                            if (lpFmtRowStr->lpFmtRowNxt
                             && lpFmtRowStr->lpFmtRowNxt->bRealRow EQ 0)
                                uTmp = uActLen;
                            else
                                uTmp = uCmpWid - uAlign - lpFmtRowStr->uAccWid;

                            // If we're to wrap this item, ...
                            if (uQuadPW < (uTmp + (UINT) (lpwszOut - lpwszOutStart)))
                            {
                                uActLen = min (uActLen, uQuadPW - uColTmp);
                                bRptCol = lpFmtRowStr->bRptCol = TRUE;
                            } // End IF
                        } // End IF

                        // Copy the next value
                        CopyMemoryW (lpwszOut, lpaplChar, (APLU3264) uActLen);
                        lpwszOut += uActLen;    // Skip over the formatted string

                        // If not simple char, ...
                        if (!IsSimpleChar (aplItmType))
                        {
                            // Include alignment blanks
                            uColTmp = lpFmtRowStr->uAccWid + uActLen + uAlign;
#ifdef PREFILL
                            // Skip over trailing blanks
                            Assert (uColTmp <= uCmpWid);
                            lpwszOut += (uCmpWid - uColTmp);
#else
                            // Fill with trailing blanks
                            lpwszOut = FillMemoryW (lpwszOut, uCmpWid - uColTmp, L' ');
#endif
                        } else
                            lpwszOut += lpFmtColStr->uTrBlNst;

                        // Skip over the # chars used
                        lpaplChar += uActLen;

                        // If we're not repeating this col, ...
                        if (!bRptCol)
                        {
                            Assert (*lpaplChar EQ WC_EOS);

                            // Skip over terminating zero
                            lpaplChar++;
                        } else
                            // Accumulate the width
                            lpFmtRowStr->uAccWid += (UINT) uActLen;

                        // If the next row is not a real row, ...
                        if (lpFmtRowStr->lpFmtRowNxt
                         && lpFmtRowStr->lpFmtRowNxt->bRealRow EQ 0)
                            // Accumulate the width
                            lpFmtRowStr->lpFmtRowNxt->uAccWid = lpFmtRowStr->uAccWid + (UINT) uActLen;
                    } // End FOR

                    // Save for the next time through
                    lpFmtRowStr->lpNxtChar = lpaplChar;

                    // Mark if more cols of data to process in this row
                    bMoreCols |= (lpaplChar < lpFmtRowStr->lpEndChar);
                } else
                {
                    if (IsSimpleHet (aplType))
                        *lpaplChar++;       // Skip over the storage type
                    *lpaplChar++;           // Skip over the terminating zero
                } // End IF/ELSE
            } // End IF

            // If not raw output, ...
            if (!bRawOutput)
            {
                // If blank row or not last row or last row and requested to do so, ...
                if (lpFmtRowStr->bBlank
                 || (aplDimRow NE (aplDimNRows - 1))
                 || (bNextRow && aplDimRow EQ (aplDimNRows - 1)))
                    // Skip to the start of the next row
                    lpwszOut = lpwszOutStart + aplLastDim;
            } else
            {
                // Handle blank lines between planes
                if (bRealRow && aplRealRow NE 1)        // Real row and not first row
                    AppendBlankRows (aplRank, aplRealRow, lpMemDim, lpbCtrlBreak);
                // Ensure properly terminated
                *lpwszOut = WC_EOS;

                // Check for Ctrl-Break
                if (CheckCtrlBreak (*lpbCtrlBreak))
                    return NULL;

                // If we're not already done with output for this row, ...
                if (!lpFmtRowStr->bDone)
                    // Output the line
                    AppendLine (lpwszOutStart, FALSE, bEndingCR || aplDimRow NE (aplDimNRows - 1));

                // Reset the line start
                lpwszOut = *lplpwszOut;

                // Fill the output area with all blanks
                uColTmp = (UINT) aplLastDim - (UINT) (*lplpwszOut - lpwszOutStart);
                FillMemoryW (lpwszOut, (APLU3264) uColTmp, L' ');

                // Are we done with this row?
                if (IsSimpleChar (aplItmType)
                 && lpaplChar >= lpFmtRowStr->lpEndChar)
                    lpFmtRowStr->bDone = TRUE;
            } // End IF

            // Point to the next FMTROWSTR
            lpFmtRowStr = lpFmtRowStr->lpFmtRowNxt;
        } // End FOR

        // If we're doing raw (not {thorn}) output,and
        //   (there is more data in this item, or
        //    there are more cols of data in this row)
        if (bRawOutput
         && (bRptCol
          || bMoreCols))
        {
            // If the array is multirank, ...
            if (IsMultiRank (aplRank))
                // Display a blank separator line
                AppendLine (L"", FALSE, TRUE);

            // If we're not finished with this col, ...
            if (bRptCol)
            {
                // Process the last item again
                uColBeg = uColLim - 1;

                // Clear the repeat col flag
                bRptCol = FALSE;
            } else
            {
                // Start with the last limit
                uColBeg = uColLim;

                // Point to the 1st FMTROWSTR
                lpFmtRowStr = lpFmt1stRowStr;

                // Loop through the rows, ...
                for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
                {
                    // Clear the repeat col flag
                    lpFmtRowStr->bRptCol = FALSE;

                    // If we're not repeating col, ...
                    if (!lpFmtRowStr->bRptCol
                     && *lpFmtRowStr->lpNxtChar EQ WC_EOS)
                        // Skip past the terminating zero
                        lpFmtRowStr->lpNxtChar++;

                    // Clear the accumulated width
                    lpFmtRowStr->uAccWid = 0;

                    // Point to the next FMTROWSTR
                    lpFmtRowStr = lpFmtRowStr->lpFmtRowNxt;
                } // End FOR
            } // End IF/ELSE

            // Set the column indent for the second and subsequent groups of lines
            uColIndent = DEF_INDENT;

            // Clear the more cols flag
            bMoreCols = FALSE;
        } else
            break;
    } // End WHILE

    // Return the output string ptr
    *lplpwszOut = max (lpwszOut, *lplpwszOut);

    return lpaplChar;
} // End FormatArrSimple


//***************************************************************************
//  $FormatArrNested
//
//  Format a nested array from its compiled form
//***************************************************************************

LPAPLCHAR FormatArrNested
    (LPFMTHEADER lpFmtHeader,       // Ptr to FMTHEADER
     LPVOID      lpMem,             // Ptr to raw input
     LPFMTCOLSTR lpFmtColStr,       // Ptr to vector of <aplDimNCols> FMTCOLSTRs
     LPAPLCHAR   lpaplChar,         // Ptr to compiled input
     LPAPLCHAR  *lplpwszOut,        // Ptr to ptr to output string
     APLDIM      aplDimNRows,       // # formatted rows in this array
     APLDIM      aplDimNCols,       // # formatted cols ...
     APLRANK     aplRank,           // Rank of this array
     LPAPLDIM    lpMemDim,          // Ptr to this array's dimensions (NULL for scalar)
     APLDIM      aplLastDim,        // Length of the last dimension in the result
     UBOOL       bRawOutput,        // TRUE iff raw (not {thorn}) output
     UBOOL       bEndingCR,         // TRUE iff last line has CR
     LPUBOOL     lpbCtrlBreak)      // Ptr to Ctrl-Break flag

{
    LPFMTROWSTR lpFmtRowStr;        // Ptr to current FMTROWSTR
    APLDIM      aplDimRow,          // Loop counter
                aplDimCol;          // ...
    LPWCHAR     lpwszOut,           // Ptr to local output string
                lpwszOutStart;      // Ptr to starting position for each row
    APLSTYPE    aplType;            // STE storage type
    UINT        uPrvWid;            // Sum of the widths of previous columns
    APLUINT     uColBeg,            // Beginning col to format
                uQuadPW;            // []PW

    // All calls to this function are cooked (not raw)
    Assert (!bRawOutput);

    // Get the current value of []PW
    uQuadPW = GetQuadPW ();

    // Initialize local output string ptr
    lpwszOut = *lplpwszOut;

    // Initialize the starting col
    uColBeg = 0;

    // Point to the FMTROWSTR
    lpFmtRowStr = (LPFMTROWSTR) lpaplChar;

    // Loop through the formatted rows
    for (aplDimRow = 0;
         aplDimRow < aplDimNRows;
         aplDimRow++)
    {
        // Validate the FMTROWSTR
        Assert (lpFmtRowStr->Sig.nature EQ FMTROWSTR_SIGNATURE);

        // Save starting output string ptr
        lpwszOutStart = lpwszOut;

        // Point to next entry
        lpaplChar = lpFmtRowStr->lpNxtChar;

        // Handle non-blank rows
        if (!lpFmtRowStr->bBlank
         && !lpFmtRowStr->bDone)    // N.B. lpaplChar does not point to data, hence
                                    //      *lpaplChar NE WC_EOS is inappropriate
        {
            // Loop through the cols
            for (uPrvWid = 0, aplDimCol = uColBeg;
                 aplDimCol < aplDimNCols;
                 aplDimCol++, ((LPAPLHETERO) lpMem)++)
            {
                // Start with the Left exterior width of the current col
                uPrvWid += LftExteriorColWidth (&lpFmtColStr[aplDimCol]);

                // If there's a previous col, ...
                if (aplDimCol > 0)
                    // Plus its Right exterior width
                    uPrvWid += RhtExteriorColWidth (&lpFmtColStr[aplDimCol - 1]);

                // Offset lpwszOut from the start by the width of previous cols
                lpwszOut = &lpwszOutStart[uPrvWid];

                // Split cases based upon the ptr type
                switch (GetPtrTypeInd (lpMem))
                {
                    case PTRTYPE_STCONST:
                        // Point to the FMTHEADER
                        lpFmtHeader = (LPFMTHEADER) lpaplChar;

                        // Validate the FMTHEADER
                        Assert (lpFmtHeader->Sig.nature EQ FMTHEADER_SIGNATURE);

                        // Get the array storage type
                        aplType = TranslateImmTypeToArrayType ((*(LPAPLHETERO) lpMem)->stFlags.ImmType);

                        lpaplChar =
                          FormatArrNestedCon (aplType,                  // Storage type of this array
                                              lpaplChar,                // Ptr to compiled input
                                             &lpwszOut,                 // Ptr to ptr to output string
                                             &lpFmtColStr[aplDimCol],   // Ptr to the FMTCOLSTR for this column
                                              aplLastDim,               // Length of the last dimension in the result
                                              lpbCtrlBreak);            // Ptr to Ctrl-Break flag
                        break;

                    case PTRTYPE_HGLOBAL:
                        lpaplChar =
                          FormatArrNestedGlb (ClrPtrTypeInd (lpMem),    // The global memory handle whose contents are to be formatted
                                              lpaplChar,                // Ptr to compiled input
                                             &lpwszOut,                 // Ptr to ptr to output string
                                             &lpFmtColStr[aplDimCol],   // Ptr to the FMTCOLSTR for this column
                                              aplLastDim,               // Length of the last dimension in the result
                                              lpbCtrlBreak);            // Ptr to Ctrl-Break flag
                        break;

                    defstop
                        break;
                } // End SWITCH

                // Check for Ctrl-Break
                if (CheckCtrlBreak (*lpbCtrlBreak))
                    return NULL;

                // Return the output string ptr
                *lplpwszOut = max (lpwszOut, *lplpwszOut);
            } // End FOR
#ifdef PREFILL
            // Skip to the start of the next row
            //   if we're not at the last row
            if (aplLastDim && aplDimRow NE (aplDimNRows - 1))
                lpwszOut = lpwszOutStart
                         + aplLastDim
                         * (((aplLastDim - 1)
                           + *lplpwszOut
                           - lpwszOutStart)
                          / aplLastDim);
            // Use the larger
            *lplpwszOut = max (lpwszOut, *lplpwszOut);
#else
            // Skip over trailing blanks
            lpwszOut = FillMemoryW (lpwszOut, lpFmtHeader->uFmtTrBl, L' ');
#endif
            // Save for the next time through
            lpFmtRowStr->lpNxtChar = lpaplChar;
        } else
            // Skip over the blanks to the next row
            *lplpwszOut += aplLastDim;

        // Reset local ptr
        lpwszOut = *lplpwszOut;

        // Point to the next FMTROWSTR
        lpFmtRowStr = lpFmtRowStr->lpFmtRowNxt;
    } // End FOR

    // Return the output string ptr
    *lplpwszOut = max (lpwszOut, *lplpwszOut);

    return lpaplChar;
} // End FormatArrNested


//***************************************************************************
//  $FormatArrNestedCon
//
//  Format a nested array constant from its compiled form
//***************************************************************************

LPAPLCHAR FormatArrNestedCon
    (APLSTYPE    aplType,       // Storage type of this array
     LPAPLCHAR   lpaplChar,     // Ptr to compiled input
     LPAPLCHAR  *lplpwszOut,    // Ptr to ptr to output string
     LPFMTCOLSTR lpFmtColStr,   // Ptr to the FMTCOLSTR for this column
     APLDIM      aplLastDim,    // Length of the last dimension in the result
     LPUBOOL     lpbCtrlBreak)  // Ptr to Ctrl-Break flag

{
    APLDIM      aplDimNRows,    // # formatted rows
                aplDimNCols;    // # formatted cols
    LPFMTHEADER lpFmtHeader;    // Ptr to this item's FMTHEADER
    LPFMTCOLSTR lpFmtColLcl;    // Ptr to this item's FMTCOLSTR

    // Point to the FMTHEADER
    lpFmtHeader = (LPFMTHEADER) lpaplChar;

    // Validate the FMTHEADER
    Assert (lpFmtHeader->Sig.nature EQ FMTHEADER_SIGNATURE);

    // Get the # actual rows and cols
    aplDimNCols = lpFmtHeader->uActCols;
    aplDimNRows = lpFmtHeader->uActRows;

    // Skip over the FMTHEADER  to the next available position
    lpaplChar = (LPAPLCHAR) &lpFmtHeader[1];

    // Point to the FMTCOLSTRs
    lpFmtColLcl = (LPFMTCOLSTR) lpaplChar;

    // Skip over the FMTCOLSTRs to the next available position
    lpaplChar = (LPAPLCHAR) &lpFmtColLcl[aplDimNCols];

#ifdef DEBUG
    // Validate the FMTCOLSTRs
    {
        APLDIM uCol;

        for (uCol = 0; uCol < aplDimNCols; uCol++)
            Assert (lpFmtColLcl[uCol].Sig.nature EQ FMTCOLSTR_SIGNATURE);
    }
#endif
    // Right justify this constant in its field
    if (IsSimpleChar (aplType))
    {
        // If this is not an all char col, ...
        if (lpFmtColStr->colType NE COLTYPE_ALLCHAR)
            // Right justify it
            lpFmtColLcl->uLdBlMax += InteriorColWidth (lpFmtColStr) - 1;
        // Copy # trailing blanks for alignment
        lpFmtColLcl->uTrBlNst = lpFmtColStr->uTrBlNst;
    } else
    {
        LPFMTROWSTR lpFmtRowStr;        // Ptr to this item's FMTROWSTR
        LPAPLCHAR   lpaplChar2;         // Ptr to compiled input
        UINT        uCmpWid,            // Compiled width
                    uActLen,            // Actual length
                    uTmp;               // Temporary

        // Point to the FMTROWSTR
        lpFmtRowStr = (LPFMTROWSTR) lpaplChar;

        // Validate the FMTROWSTR
        Assert (lpFmtRowStr->Sig.nature EQ FMTROWSTR_SIGNATURE);

        // Skip over the FMTROWSTR to the next available position
        lpaplChar2 = (LPAPLCHAR) &lpFmtRowStr[1];

        uCmpWid = InteriorColWidth (lpFmtColStr);   // Compiled width
        uActLen = lstrlenW (lpaplChar2);            // Actual length

        Assert (uActLen <= uCmpWid);

        // Get length of integer part
        uTmp = (UINT) (SkipToCharW (lpaplChar2, L'.') - lpaplChar2);

        // Add in length of longest fraction in this
        //   column (including decimal point)
        uTmp += lpFmtColStr->uFrcs;

        // Calculate size of leading blanks in order to
        //   line up the decimal points
        uTmp = uCmpWid - max (uActLen, uTmp);

        // Save the extra width as leading blanks,
        //   in effect right-justifying this item in the col
        lpFmtColLcl->uLdBlMax = max (lpFmtColLcl->uLdBlMax, uTmp);
    } // End IF

    return
      FormatArrSimple (lpFmtHeader,         // Ptr to FMTHEADER
                       lpFmtColLcl,         // Ptr to vector of aplDimNCols FMTCOLSTRs
                       lpaplChar,           // Ptr to compiled input
                       lplpwszOut,          // Ptr to ptr to output string
                       aplDimNRows,         // # formatted rows in this array
                       aplDimNCols,         // # formatted cols in this array
                       aplLastDim,          // Length of last dim in result
                       0,                   // Rank of this array
                       NULL,                // Ptr to this array's dimensions
                       aplType,             // Storage type of this array
                       FALSE,               // TRUE iff skip to next row after this item
                       FALSE,               // TRUE iff raw output
                       TRUE,                // TRUE iff last line has CR
                       lpbCtrlBreak);       // Ptr to Ctrl-Break flag
} // End FormatArrNestedCon


//***************************************************************************
//  $FormatArrNestedGlb
//
//  Format a nested array global memory object from its compiled form
//***************************************************************************

LPAPLCHAR FormatArrNestedGlb
    (HGLOBAL     hGlb,          // The global memory handle whose contents are to be formatted
     LPAPLCHAR   lpaplChar,     // Ptr to compiled input
     LPAPLCHAR  *lplpwszOut,    // Ptr to ptr to output string
     LPFMTCOLSTR lpFmtColStr,   // Ptr to the FMTCOLSTR for this column
     APLDIM      aplLastDim,    // Length of the last dimension in the result
     LPUBOOL     lpbCtrlBreak)  // Ptr to Ctrl-Break flag

{
    APLSTYPE    aplType;        // This item's storage type
    APLRANK     aplRank;        // This item's rank
    LPVOID      lpMem;          // Ptr to this item's global memory
    LPAPLDIM    lpMemDim;       // Ptr to this item's dimensions
    APLDIM      aplDimNRows,    // # formatted rows
                aplDimNCols,    // # formatted cols
                uCol;           // Loop counter
    LPFMTHEADER lpFmtHeader;    // Ptr to this item's FMTHEADER
    LPFMTCOLSTR lpFmtColLcl;    // Ptr to this item's FMTCOLSTRs

    // Get the attributes (Type, NELM, Rank) of the global
    AttrsOfGlb (hGlb, &aplType, NULL, &aplRank, NULL);

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlb);

    // Skip over the header to the dimensions
    lpMemDim = VarArrayBaseToDim (lpMem);

    // Skip over the header and dimensions to the data
    lpMem = VarArrayDataFmBase (lpMem);

    // Point to the FMTHEADER
    lpFmtHeader = (LPFMTHEADER) lpaplChar;

    // Validate the FMTHEADER
    Assert (lpFmtHeader->Sig.nature EQ FMTHEADER_SIGNATURE);

    // Get the # actual rows and cols
    aplDimNCols = lpFmtHeader->uActCols;
    aplDimNRows = lpFmtHeader->uActRows;

    // Skip over the FMTHEADER to the next available position
    lpaplChar = (LPAPLCHAR) &lpFmtHeader[1];

    // Point to the FMTCOLSTRs
    lpFmtColLcl = (LPFMTCOLSTR) lpaplChar;

#ifdef DEBUG
    // Validate the FMTCOLSTRs
    for (uCol = 0; uCol < aplDimNCols; uCol++)
        Assert (lpFmtColLcl[uCol].Sig.nature EQ FMTCOLSTR_SIGNATURE);
#endif
    // If it's a simple char array, ...
    if (IsSimpleChar (aplType))
        // Copy # trailing blanks for alignment
        lpFmtColLcl->uTrBlNst = lpFmtColStr->uTrBlNst;
    // Skip over the FMTCOLSTRs to the next available position
    lpaplChar = (LPAPLCHAR) &lpFmtColLcl[aplDimNCols];

    // Distribute this column's FMTCOLSTR (in lpFmtColStr)
    //   amongst the FMTCOLSTRs for this item (in lpFmtColLcl)
    if (!IsZeroDim (aplDimNCols))
    {
        UINT uLclWid,           // Local column width
             uGlbWid;           // Global ...

        // Calculate the global interior width
        uGlbWid = InteriorColWidth (lpFmtColStr);

        // Calculate the local exterior width
        for (uLclWid = 0, uCol = 0; uCol < aplDimNCols; uCol++)
            uLclWid += ExteriorColWidth (&lpFmtColLcl[uCol]);

        // Less the leading and trailing blanks to get interior width
        if (aplDimNCols)
            uLclWid -= lpFmtColLcl[0              ].uLdBlNst
                     + lpFmtColLcl[0              ].uLdBlMax
                     + lpFmtColLcl[aplDimNCols - 1].uTrBlNst;

        Assert (uLclWid <= uGlbWid);

        // If the parent column is not all char, and
        //   the current item   is char, ...
        if (lpFmtColStr->colType EQ COLTYPE_NOTCHAR
         && IsSimpleChar (aplType))
            // Save the extra width as leading blanks,
            //   in effect right-justifying this item in the col
            lpFmtColLcl->uLdBlMax += (uGlbWid - uLclWid);
    } // End IF

    // Split cases based upon the right arg's storage type
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
            lpaplChar =
              FormatArrSimple (lpFmtHeader,     // Ptr to FMTHEADER
                               lpFmtColLcl,     // Ptr to vector of <aplDimNCols> FMTCOLSTRs
                               lpaplChar,       // Ptr to compiled input
                               lplpwszOut,      // Ptr to output string
                               aplDimNRows,     // # rows in this array
                               aplDimNCols,     // # cols in this array
                               aplLastDim,      // Length of last dim in result
                               aplRank,         // Rank of this array
                               lpMemDim,        // Ptr to this array's dimensions
                               aplType,         // Storage type of this array
                               FALSE,           // TRUE iff skip to next row after this item
                               FALSE,           // TRUE iff raw output
                               TRUE,            // TRUE iff last line has CR
                               lpbCtrlBreak);   // Ptr to Ctrl-Break flag
            break;

        case ARRAY_NESTED:
            lpaplChar =
              FormatArrNested (lpFmtHeader,     // Ptr to FMTHEADER
                               lpMem,           // Ptr to raw input
                               lpFmtColLcl,     // Ptr to vector of <aplDimNCols> FMTCOLSTRs
                               lpaplChar,       // Ptr to compiled input
                               lplpwszOut,      // Ptr to ptr to output string
                               aplDimNRows,     // # rows in this array
                               aplDimNCols,     // # cols ...
                               aplRank,         // Rank of this array
                               lpMemDim,        // Ptr to this array's dimensions
                               aplLastDim,      // Length of last dim in result
                               FALSE,           // TRUE iff raw (not {thorn}) output
                               TRUE,            // TRUE iff last line has CR
                               lpbCtrlBreak);   // Ptr to Ctrl-Break flag
            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlb); lpMem = NULL;

    return lpaplChar;
} // End FormatArrNestedGlb


//***************************************************************************
//  $PrimFnDydDownTackJot_EM_YY
//
//  Primitive function for dyadic DownTackJot
//     ("format by specification"/"format by example")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydDownTackJot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydDownTackJot_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeLft,        // Left arg storage type
                 aplTypeRht,        // Right ...
                 aplTypeSubRht;     // Right arg item ...
    APLNELM      aplNELMLft,        // Left arg NELM
                 aplNELMRht,        // Right ...
                 aplNELMRes;        // Result   ...
    APLRANK      aplRankLft,        // Left arg rank
                 aplRankRht,        // Right ...
                 aplRankSubRht,     // Right arg item ...
                 aplRankRes;        // Result   ...
    APLDIM       aplDimNCols,       // # columns
                 aplDimNRows,       // # rows
                 aplDimCol,         // Loop counter
                 aplDimRow,         // ...
                 aplColsRht;        // Right arg # cols
    HGLOBAL      hGlbLft = NULL,    // Left arg global memory handle
                 hGlbRht = NULL,    // Right ...
                 hGlbRes = NULL,    // Result   ...
                 hGlbWidPrc = NULL, // Left arg WIDPRC struct ...
                 hGlbItmRht = NULL, // Right arg item ...
                 lpSymGlbLft;       // Ptr to left arg as global numeric
    LPVOID       lpMemLft = NULL,   // Ptr to left arg global memory
                 lpMemRht = NULL;   // Ptr to right ...
    LPAPLCHAR    lpMemRes = NULL;   // Ptr to result   ...
    LPAPLDIM     lpMemDimRht = NULL;// Ptr to right arg dimensions
    APLLONGEST   aplLongestLft,     // Left arg immediate value
                 aplLongestRht;     // Right ...
    LPWIDPRC     lpMemWidPrc = NULL;// Ptr to left arg WIDPRC struc
    LPAPLCHAR    lpaplChar,         // Ptr to next available format position
                 lpaplCharIni,      // Ptr to initial format position
                 lpaplCharExp;      // Ptr to 'E' in E-format number
    APLINT       apaOffLft,         // Left arg APA offset
                 apaMulLft,         // ...          multiplier
                 aplIntegerLft,     // Left arg temporary integer
                 iPrc;              // Precision ...
    APLUINT      ByteRes,           // # bytes in the result
                 uDim,              // Loop counter
                 uRht,              // Loop counter
                 uPar,              // Parity indicator
                 uTotWid,           // Total width for this col
                 uAccWid,           // Accumulated width for this col
                 uWid;              // Width for this col
    IMM_TYPES    immTypeRht;        // Right arg element immediate type
    APLCHAR      aplCharDecimal,    // []FC[FCNDX_DECIMAL_SEP]
                 aplCharOverflow,   // []FC[FCNDX_OVERFLOW_FILL]
                 aplCharOverbar;    // []FC[FCNDX_OVERBAR]
    UINT         uBitMask,          // Bit mask for looping through Booleans
                 uMaxExp,           // Width of the maximum exponent (including the 'E')
                 uOldMaxExp,        // Old ...
                 uLen;              // Length of formatted number
    UBOOL        bRet = TRUE,       // TRUE iff result is valid
                 bAllChar,          // TRUE iff this column is all character
                 Auto;              // TRUE iff the col is automatic width
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to result
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPWCHAR      lpwszFormat;       // Ptr to formatting save area
    APLVFP       aplVfpItm = {0};   // Temp VFP number

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get ptr to formatting save area
    lpwszFormat = lpMemPTD->lpwszFormat;

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, &aplColsRht);

    // Check for LEFT RANK ERROR
    if (IsMultiRank (aplRankLft))
        goto RANK_EXIT;

    // Check for "format by example"
    if (IsSimpleChar (aplTypeLft))
        return PrimFnDydDownTackJotFBE_EM_YY (lptkLftArg,   // Ptr to left arg token
                                              lptkFunc,     // Ptr to function token
                                              lptkRhtArg,   // Ptr to right arg token
                                              lptkAxis);    // Ptr to axis token (may be NULL)
    // Continue with "format by specification"

    // Check for LEFT LENGTH ERROR
    if (!IsSingleton (aplNELMLft)           // Single number
     && aplNELMLft NE 2                     // Pair of numbers
     && (0 NE (aplNELMLft % 2)              // Pairs of numbers
      || ((aplNELMLft / 2) NE aplColsRht))) // One pair per right arg col
        goto LENGTH_EXIT;

    // Check for LEFT DOMAIN ERROR
    if (!IsNumeric (aplTypeLft))
        goto DOMAIN_EXIT;

    // Get left and right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Calculate space needed for the result
    ByteRes = aplColsRht * sizeof (WIDPRC);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate temp storage for the normalized left arg
    hGlbWidPrc = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbWidPrc)
        goto WSFULL_EXIT;

    // Copy left arg to temp storage (as WIDPRCs),
    //   and check the left arg for valid values

    // Lock the memory to get a ptr to it
    lpMemWidPrc = MyGlobalLock (hGlbWidPrc);

    if (hGlbLft)
        // Skip over the header to the data
        lpMemLft = VarArrayDataFmBase (lpMemLft);

    // Handle singleton left arg
    if (IsSingleton (aplNELMLft))
    {
        // Get the first value from the token
        GetFirstValueToken (lptkLftArg,         // Ptr to the token
                            NULL,               // Ptr to the integer (or Boolean) (may be NULL)
                            NULL,               // ...        float (may be NULL)
                            NULL,               // ...        char (may be NULL)
                           &aplLongestLft,      // ...        longest (may be NULL)
                           &lpSymGlbLft,        // ...        LPSYMENTRY or HGLOBAL (may be NULL)
                            NULL,               // ...        immediate type (see IMM_TYPES) (may be NULL)
                            NULL);              // ...        array type:  ARRAY_TYPES (may be NULL)
        // Split cases based upon the left arg storage type
        switch (aplTypeLft)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
                break;

            case ARRAY_FLOAT:
                // Attempt to convert the float to an integer using System []CT
                aplLongestLft = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestLft, &bRet);
                if (!bRet)
                    goto DOMAIN_EXIT;
                break;

            case ARRAY_RAT:
                // Attempt to convert the RAT to an integer using System []CT
                aplLongestLft = mpq_get_sctsx ((LPAPLRAT) lpSymGlbLft, &bRet);
                if (!bRet)
                    goto DOMAIN_EXIT;
                break;

            case ARRAY_VFP:
                // Attempt to convert the VFP to an integer using System []CT
                aplLongestLft = mpfr_get_sctsx ((LPAPLVFP) lpSymGlbLft, &bRet);
                if (!bRet)
                    goto DOMAIN_EXIT;
                break;

            case ARRAY_CHAR:
            case ARRAY_HETERO:
            case ARRAY_NESTED:
                break;

            defstop
                break;
        } // End SWITCH

        // Save as actual precision
        lpMemWidPrc[0].iPrc = aplLongestLft;
        lpMemWidPrc[0].Auto = TRUE;
    } else
    // Ensure that the first value in each pair is non-negative
    // Split cases based upon the left arg's storage type
    switch (aplTypeLft)
    {
        case ARRAY_BOOL:
            uBitMask = BIT0;

            for (uDim = uPar = 0; uDim < aplNELMLft; uDim++, uPar = 1 - uPar)
            {
                aplIntegerLft = (uBitMask & *(LPAPLBOOL) lpMemLft) ? TRUE : FALSE;
////////////////if ((!uPar) && aplIntegerLft < 0)   // Not needed on Booleans
////////////////    goto DOMAIN_EXIT;
                if (!uPar)
                    lpMemWidPrc  ->uWid = aplIntegerLft;
                else
                    lpMemWidPrc++->iPrc = aplIntegerLft;

                // Shift over the left bit mask
                uBitMask <<= 1;

                // Check for end-of-byte
                if (uBitMask EQ END_OF_BYTE)
                {
                    uBitMask = BIT0;            // Start over
                    ((LPAPLBOOL) lpMemLft)++;   // Skip to next byte
                } // End IF
            } // End FOR

            break;

        case ARRAY_INT:
            for (uDim = uPar = 0; uDim < aplNELMLft; uDim++, uPar = 1 - uPar)
            {
                aplIntegerLft = *((LPAPLINT) lpMemLft)++;
                if ((!uPar) && aplIntegerLft < 0)
                    goto DOMAIN_EXIT;
                if (!uPar)
                    lpMemWidPrc  ->uWid = aplIntegerLft;
                else
                    lpMemWidPrc++->iPrc = aplIntegerLft;
            } // End FOR

            break;

        case ARRAY_FLOAT:
            for (uDim = uPar = 0; uDim < aplNELMLft; uDim++, uPar = 1 - uPar)
            {
                // Attempt to convert the float to an integer using System []CT
                aplIntegerLft = FloatToAplint_SCT (*((LPAPLFLOAT) lpMemLft)++, &bRet);
                if ((!bRet) || ((!uPar) && aplIntegerLft < 0))
                    goto DOMAIN_EXIT;
                if (!uPar)
                    lpMemWidPrc  ->uWid = aplIntegerLft;
                else
                    lpMemWidPrc++->iPrc = aplIntegerLft;
            } // End FOR

            break;

        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemLft)
            // Get the APA parameters
            apaOffLft = lpAPA->Off;
            apaMulLft = lpAPA->Mul;
#undef  lpAPA
            for (uDim = uPar = 0; uDim < aplNELMLft; uDim++, uPar = 1 - uPar)
            {
                aplIntegerLft = apaOffLft + apaMulLft * uDim;
                if ((!uPar) && aplIntegerLft < 0)
                    goto DOMAIN_EXIT;
                if (!uPar)
                    lpMemWidPrc  ->uWid = aplIntegerLft;
                else
                    lpMemWidPrc++->iPrc = aplIntegerLft;
            } // End FOR

            break;

        case ARRAY_RAT:
            for (uDim = uPar = 0; uDim < aplNELMLft; uDim++, uPar = 1 - uPar)
            {
                // Attempt to convert the RAT to an integer using System []CT
                aplIntegerLft = mpq_get_sctsx (((LPAPLRAT) lpMemLft)++, &bRet);
                if ((!bRet) || ((!uPar) && aplIntegerLft < 0))
                    goto DOMAIN_EXIT;
                if (!uPar)
                    lpMemWidPrc  ->uWid = aplIntegerLft;
                else
                    lpMemWidPrc++->iPrc = aplIntegerLft;
            } // End FOR

            break;

        case ARRAY_VFP:
            for (uDim = uPar = 0; uDim < aplNELMLft; uDim++, uPar = 1 - uPar)
            {
                // Attempt to convert the VFP to an integer using System []CT
                aplIntegerLft = mpfr_get_sctsx (((LPAPLVFP) lpMemLft)++, &bRet);
                if ((!bRet) || ((!uPar) && aplIntegerLft < 0))
                    goto DOMAIN_EXIT;
                if (!uPar)
                    lpMemWidPrc  ->uWid = aplIntegerLft;
                else
                    lpMemWidPrc++->iPrc = aplIntegerLft;
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH

    if (hGlbLft && lpMemLft)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

    // Restore lpMemWidPrc to the start of the block
    MyGlobalUnlock (hGlbWidPrc); lpMemWidPrc = NULL;
    lpMemWidPrc = MyGlobalLock (hGlbWidPrc);

    // If there's only one pair and more than one right arg col,
    //    spread the pair through lpMemWidPrc
    if (aplNELMLft <= 2
     && aplColsRht > 1)
    {
        // Get the corresponding attributes for this col
        uWid = lpMemWidPrc[0].uWid;
        iPrc = lpMemWidPrc[0].iPrc;

        for (uRht = 1; uRht < aplColsRht; uRht++)
        {
            // Save in temporary left arg
            lpMemWidPrc[uRht].uWid = uWid;
            lpMemWidPrc[uRht].iPrc = iPrc;
        } // End FOR
    } // End IF

    // Set the Auto bit if the width is zero
    for (uRht = 0; uRht < aplColsRht; uRht++)
        lpMemWidPrc[uRht].Auto = (lpMemWidPrc[uRht].uWid EQ 0);

    if (lpMemRht)
        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemRht);

    // Get the # rows & columns in the right arg
    if (IsScalar (aplRankRht))
        aplDimNCols = aplDimNRows = 1;
    else
    {
        aplDimNCols = lpMemDimRht[aplRankRht - 1];

        // Get the # rows (across all planes)
        if (IsVector (aplRankRht)
         || IsZeroDim (aplDimNCols))
            aplDimNRows = 1;
        else
            aplDimNRows = aplNELMRht / aplDimNCols;
    } // End IF/ELSE

    if (lpMemRht)
        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemRht);

    // Validate the right arg as either simple (including heterogeneous) or
    //   nested consisting of numeric scalars and/or
    //   simple character scalars or vectors
    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_CHAR:
        case ARRAY_APA:
        case ARRAY_RAT:
        case ARRAY_VFP:
            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Loop through the right arg
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Split cases based upon the right arg item ptr type
                switch (GetPtrTypeDir (((LPAPLNESTED) lpMemRht)[uRht]))
                {
                    case PTRTYPE_STCONST:
                        break;

                    case PTRTYPE_HGLOBAL:
                        // Get the attributes of the global memory handle
                        AttrsOfGlb (((LPAPLNESTED) lpMemRht)[uRht],
                                   &aplTypeSubRht,
                                    NULL,
                                   &aplRankSubRht,
                                    NULL);
                        // Check for RANK ERROR
                        switch (aplTypeSubRht)
                        {
                            case ARRAY_BOOL:
                            case ARRAY_INT:
                            case ARRAY_FLOAT:
                            case ARRAY_APA:
                            case ARRAY_RAT:
                            case ARRAY_VFP:
                                // Numeric scalars only
                                if (!IsScalar (aplRankSubRht))
                                    goto DOMAIN_EXIT;
                                break;

                            case ARRAY_CHAR:
                                // Character scalars or vectors only
                                if (IsMultiRank (aplRankSubRht))
                                    goto DOMAIN_EXIT;
                                break;

                            case ARRAY_HETERO:
                            case ARRAY_NESTED:
                                goto DOMAIN_EXIT;

                                break;

                            defstop
                                break;
                        } // End SWITCH

                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH

    // The result rank is at least a vector
    aplRankRes = max (aplRankRht, 1);

    // Initialize the output save area ptr
    lpaplChar = lpwszFormat;

    // Get the []FC values we need
    aplCharDecimal  = GetQuadFCValue (FCNDX_DECIMAL_SEP);
    aplCharOverflow = GetQuadFCValue (FCNDX_OVERFLOW_FILL);
    aplCharOverbar  = GetQuadFCValue (FCNDX_OVERBAR);

__try
{
    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
        case ARRAY_FLOAT:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
        case ARRAY_RAT:
        case ARRAY_VFP:
            // Loop through the right arg col by col
            //   formatting the values into lpwszFormat
            //   and accumulating the widths
            for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
            {
                // Get the corresponding attributes for this col
                Auto     = (BOOL) lpMemWidPrc[aplDimCol].Auto;
                uWid     =        lpMemWidPrc[aplDimCol].uWid;
                iPrc     =        lpMemWidPrc[aplDimCol].iPrc;
                bAllChar = TRUE;

                // Loop through all rows (and across planes)
                for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
                {
                    // Save the initial position
                    lpaplCharIni = lpaplChar;

                    // Get the next value from the right arg
                    if (lpMemRht)
                        GetNextValueMem (lpMemRht,                              // Ptr to right arg global memory data
                                         aplTypeRht,                            // Right arg  storage type
                                         aplDimCol + aplDimRow * aplDimNCols,   // Index into right arg
                                        &hGlbItmRht,                            // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                                        &aplLongestRht,                         // Ptr to result immediate value (may be NULL)
                                        &immTypeRht);                           // Ptr to result immediate type (may be NULL)
                    else
                        immTypeRht = TranslateArrayTypeToImmType (aplTypeRht);

                    // If the item is a global numeric, ...
                    if (IsGlbNum (aplTypeRht))
                    {
                        // No longer all character
                        bAllChar = FALSE;

                        // hGlbItmRht is actually a ptr to the global numeric data
                        // Split cases based upon the item storage type
                        switch (aplTypeRht)
                        {
                            case ARRAY_RAT:
                                // Convert the RAT to a VFP
                                mpfr_init_set_q (&aplVfpItm, (LPAPLRAT) hGlbItmRht, MPFR_RNDN);

                                lpaplChar =
                                  FormatAplVfpFC (lpaplChar,                // Ptr to output save area
                                                  aplVfpItm,                // The value to format
                                                  iPrc,                     // # significant/fractional digits (0 = all)
                                                  aplCharDecimal,           // Char to use as decimal separator
                                                  aplCharOverbar,           // Char to use as overbar
                                                  iPrc >= 0,                // TRUE iff nDigits is # fractional digits
                                                  FALSE,                    // TRUE iff we're to substitute text for infinity
                                                  FALSE);                   // TRUE iff we're to precede the display with (FPCnnn)
                                // We no longer need this storage
                                Myf_clear (&aplVfpItm);

                                break;

                            case ARRAY_VFP:
                                lpaplChar =
                                  FormatAplVfpFC (lpaplChar,                // Ptr to output save area
                                                 *(LPAPLVFP) hGlbItmRht,    // The value to format
                                                  iPrc,                     // # significant/fractional digits (0 = all)
                                                  aplCharDecimal,           // Char to use as decimal separator
                                                  aplCharOverbar,           // Char to use as overbar
                                                  iPrc >= 0,                // TRUE iff nDigits is # fractional digits
                                                  FALSE,                    // TRUE iff we're to substitute text for infinity
                                                  FALSE);                   // TRUE iff we're to precede the display with (FPCnnn)
                                break;

                            defstop
                                break;
                        } // End SWITCH

                        // Ensure it's properly terminated
                        lpaplChar[-1] = WC_EOS;
                    } else
                    // If the item is an HGLOBAL, ...
                    if (hGlbItmRht)
                    {
                        LPAPLCHAR lpMemItmRht;
                        APLNELM   aplNELMItmRht;
                        APLRANK   aplRankItmRht;

                        // The item must be a char vector

                        // Lock the memory to get a ptr to it
                        lpMemItmRht = MyGlobalLock (hGlbItmRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemItmRht)
                        // Get the array parameters
                        aplNELMItmRht = lpHeader->NELM;
                        aplRankItmRht = lpHeader->Rank;
#undef  lpHeader
                        // Skip over the header to the data
                        lpMemItmRht = VarArrayDataFmBase (lpMemItmRht);

                        // Copy the data to the format area
                        CopyMemoryW (lpaplChar, lpMemItmRht, (APLU3264) aplNELMItmRht);

                        // Skip over the copied data
                        lpaplChar += aplNELMItmRht;

                        // Ensure it's properly terminated
                        *lpaplChar++ = WC_EOS;

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbItmRht); lpMemItmRht = NULL;
                    } else
                    // Split cases based upon the immediate type
                    switch (immTypeRht)
                    {
                        case IMMTYPE_BOOL:
                        case IMMTYPE_INT:
                            // No longer all character
                            bAllChar = FALSE;

                            // Split cases based upon the sign of iPrc
                            if (iPrc >= 0)
                            {
                                // Format the number
                                lpaplChar =
                                  FormatAplintFC (lpaplChar,        // Ptr to output save area
                                                  aplLongestRht,    // The value to format
                                                  aplCharOverbar);  // Char to use as overbar
                                // Zap the trailing blank
                                lpaplChar[-1] = WC_EOS;

                                if (iPrc)
                                {
                                    // Append decimal separator followed by iPrc 0s
                                    lpaplChar[-1] = aplCharDecimal;
                                    lpaplChar = FillMemoryW (lpaplChar, (APLU3264) iPrc, L'0');
                                    *lpaplChar++ = WC_EOS;
                                } // End IF
                            } else
////////////////////////////if (iPrc < 0)
                            {
                                // Format the number
                                lpaplChar =
                                  FormatFloatFC (lpaplChar,                         // Ptr to output save area
                                                 (APLFLOAT) (APLINT) aplLongestRht, // The value to format
                                                 -iPrc,                             // Precision to use
                                                 aplCharDecimal,                    // Char to use as decimal separator
                                                 aplCharOverbar,                    // Char to use as overbar
                                                 FLTDISPFMT_E,                      // Float display format
                                                 FALSE);                            // TRUE iff we're to substitute text for infinity
                                // Zap the trailing blank
                                lpaplChar[-1] = WC_EOS;
                            } // End IF/ELSE

                            break;

                        case IMMTYPE_FLOAT:
                            // No longer all character
                            bAllChar = FALSE;

                            // Split cases based upon the sign of iPrc
                            if (iPrc > 0)
                                // Format the number
                                lpaplChar =
                                  FormatFloatFC (lpaplChar,                             // Ptr to output save area
                                                *(LPAPLFLOAT) &aplLongestRht,           // The value to format
                                                 iPrc,                                  // Precision for F-format, significant digits for E-format
                                                 aplCharDecimal,                        // Char to use as decimal separator
                                                 aplCharOverbar,                        // Char to use as overbar
                                                 FLTDISPFMT_F,                          // Float display format
                                                 FALSE);                                // TRUE iff we're to substitute text for infinity
                            else
                            if (iPrc EQ 0)
                                // Format the number
                                lpaplChar =
                                  FormatFloatFC (lpaplChar,                             // Ptr to output save area
                                                *(LPAPLFLOAT) &aplLongestRht,           // The value to format
                                                 DBL_MAX_10_EXP + 2,                    // Precision for F-format, significant digits for E-format
                                                 aplCharDecimal,                        // Char to use as decimal separator
                                                 aplCharOverbar,                        // Char to use as overbar
                                                 FLTDISPFMT_RAWINT,                     // Float display format
                                                 FALSE);                                // TRUE iff we're to substitute text for infinity
                            else
////////////////////////////if (iPrc < 0)
                                // Format the number
                                lpaplChar =
                                  FormatFloatFC (lpaplChar,                             // Ptr to output save area
                                                *(LPAPLFLOAT) &aplLongestRht,           // The value to format
                                                 -iPrc,                                 // Precision for F-format, significant digits for E-format
                                                 aplCharDecimal,                        // Char to use as decimal separator
                                                 aplCharOverbar,                        // Char to use as overbar
                                                 FLTDISPFMT_E,                          // Float display format
                                                 FALSE);                                // TRUE iff we're to substitute text for infinity
                            // Zap the trailing blank
                            lpaplChar[-1] = WC_EOS;

                            break;

                        case IMMTYPE_CHAR:
                            *lpaplChar++ = (APLCHAR) aplLongestRht;
                            *lpaplChar++ = WC_EOS;

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    // Get the formatted length
                    uLen = (UINT) (lpaplChar - lpaplCharIni) - 1;

                    // If we're using E-format, ...
                    if (iPrc < 0)
                    {
                        // Find the length of the exponent (counting the 'E')
                        uMaxExp = uLen - (UINT) ((SkipToCharW (lpaplCharIni, DEF_EXPONENT_UC) - lpaplCharIni));

                        // Save the old value in case we overflow
                        uOldMaxExp = lpMemWidPrc[aplDimCol].uMaxExp;

                        // Save the larger
                        lpMemWidPrc[aplDimCol].uMaxExp = max (uMaxExp, uOldMaxExp);
                    } // End IF

                    // Check for automatic width
                    if (Auto)
                        lpMemWidPrc[aplDimCol].uWid = uWid = max (uWid, uLen);
                    else
                    // Check for width overflow
                    if (uWid < uLen)
                    {
                        // If the overflow char is the default, ...
                        if (aplCharOverflow EQ DEF_QUADFC_OVERFLOW)
                            goto DOMAIN_EXIT;

                        // Restore the previous max exponent field as this
                        //   one has overflowed
                        if (iPrc < 0)
                            lpMemWidPrc[aplDimCol].uMaxExp = uOldMaxExp;;

                        // Fill with the overflow char
                        lpaplChar = FillMemoryW (lpaplCharIni, (APLU3264) uWid, aplCharOverflow);

                        // Ensure properly terminated
                        *lpaplChar++ = WC_EOS;
                    } // End IF/ELSE/...
                } // End FOR

                // Save AllChar value
                lpMemWidPrc[aplDimCol].bAllChar = bAllChar;
            } // End FOR

            break;

        case ARRAY_CHAR:
            // Pass the right arg on as the result
            hGlbRes = CopySymGlbDirAsGlb (hGlbRht);

            goto YYALLOC_EXIT;

        defstop
            break;
    } // End SWITCH
} __except (CheckVirtAlloc (GetExceptionInformation (), L"PrimFnDydDownTackJot_EM_YY"))
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

    // Calculate the total width, including extra column for auto width
    for (uRht = uTotWid = 0; uRht < aplColsRht; uRht++)
        uTotWid += lpMemWidPrc[uRht].Auto + lpMemWidPrc[uRht].uWid;

    // Calculate the result NELM
    aplNELMRes = aplDimNRows * uTotWid;

    // Calculate the space needed for the result
    ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRes, aplRankRes);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Skip over the header to the dimensions
    (LPVOID) lpMemRes = VarArrayBaseToDim (lpMemRes);

    // Fill in the dimensions
    CopyMemory (lpMemRes, lpMemDimRht, (APLU3264) (aplRankRes - 1) * sizeof (APLDIM));
    ((LPAPLDIM) lpMemRes)[aplRankRes - 1] = uTotWid;

    // Skip over the dimensions to the data
    lpMemRes = VarArrayDimToData (lpMemRes, aplRankRes);

    // Initialize the output save area ptr
    lpaplChar = lpwszFormat;

    // Fill in the data

    // Fill the result with blanks
    FillMemoryW (lpMemRes, (APLU3264) aplNELMRes, L' ');

    // Loop through the formatted data and copy it to the result
    for (aplDimCol = uAccWid = 0; aplDimCol < aplDimNCols; aplDimCol++, uAccWid += Auto + uWid)
    {
        // Get the corresponding attributes for this col
        Auto     = (BOOL) lpMemWidPrc[aplDimCol].Auto;
        uWid     =        lpMemWidPrc[aplDimCol].uWid;
        iPrc     =        lpMemWidPrc[aplDimCol].iPrc;
        bAllChar =        lpMemWidPrc[aplDimCol].bAllChar;

        // Loop through all rows (and across planes)
        for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
        {
            // Get the string length
            uLen = lstrlenW (lpaplChar);

            // If the col is all character data, ...
            if (bAllChar)
                // Left-adjust the char vector in the col
                uMaxExp = (UINT) uWid - uLen;
            else
            // If this is E-format, ...
            if (iPrc < 0)
            {
                // Find the 'E'
                lpaplCharExp = SkipToCharW (lpaplChar, DEF_EXPONENT_UC);

                // If there's an 'E' (not overflowed)
                if (lpaplCharExp[0]
                 && lpaplCharExp NE lpaplChar)      // In case the overflow char is 'E'
                {
                    // Find the length of the exponent (counting the 'E')
                    uMaxExp = uLen - (UINT) (lpaplCharExp - lpaplChar);

                    // Calculate the difference between the longest and the current length
                    uMaxExp = lpMemWidPrc[aplDimCol].uMaxExp - uMaxExp;
                } else
                    uMaxExp = 0;
            } else
                uMaxExp = 0;

            // Copy the next formatted value to the result,
            //   right-justifying it in the process
            CopyMemoryW (&lpMemRes[aplDimRow * uTotWid + uAccWid + Auto + (uWid - uLen) - uMaxExp],
                          lpaplChar,
                          uLen);
            // Skip over the formatted value and the trailing zero
            lpaplChar += uLen + 1;
        } // End FOR
    } // End FOR
YYALLOC_EXIT:
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LIMIT_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LIMIT_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes)
    {
        if (lpMemRes)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbWidPrc)
    {
        if (lpMemWidPrc)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbWidPrc); lpMemWidPrc = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbWidPrc); hGlbWidPrc = NULL;
    } // End IF

    if (hGlbLft && lpMemLft)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    return lpYYRes;
} // End PrimFnDydDownTackJot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydDownTackJotFBE_EM_YY
//
//  Primitive function for dyadic DownTackJot ("format by example")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydDownTackJotFBE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydDownTackJotFBE_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnNonceError_EM (lptkFunc APPEND_NAME_ARG);

    DbgBrk ();          // ***FINISHME*** -- PrimFnDydDownTackJot_EM_YY ("format by example")






} // End PrimFnDydDownTackJotFBE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_dtackjot.c
//***************************************************************************
