//***************************************************************************
//  NARS2000 -- System Function -- Quad DR
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
#include "headers.h"


//***************************************************************************
//  $SysFnDR_EM_YY
//
//  System function:  []DR -- Data Representation
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDR_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDR_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Note that we don't test for TKT_LIST/ARRAY_LIST
    //   as this function processes that element.

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonDR_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydDR_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnDR_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonDR_EM_YY
//
//  Monadic []DR -- Display the Data Representation
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonDR_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonDR_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeRht;        // Right arg storage type
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
////lpYYRes->tkToken.tkData.tkInteger  =   (filled in below)
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, NULL, NULL, NULL);

    // Set the datatype
    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            lpYYRes->tkToken.tkData.tkInteger = DR_BOOL;

            break;

        case ARRAY_INT:
            lpYYRes->tkToken.tkData.tkInteger = DR_INT64;

            break;

        case ARRAY_FLOAT:
            lpYYRes->tkToken.tkData.tkInteger = DR_FLOAT;

            break;

        case ARRAY_CHAR:
            lpYYRes->tkToken.tkData.tkInteger = DR_CHAR16;

            break;

        case ARRAY_APA:
            lpYYRes->tkToken.tkData.tkInteger = DR_APA;

            break;

        case ARRAY_HETERO:
            lpYYRes->tkToken.tkData.tkInteger = DR_HETERO;

            break;

        case ARRAY_NESTED:
            lpYYRes->tkToken.tkData.tkInteger = DR_NESTED;

            break;

        case ARRAY_RAT:
            lpYYRes->tkToken.tkData.tkInteger = DR_RAT;

            break;

        case ARRAY_VFP:
            lpYYRes->tkToken.tkData.tkInteger = DR_VFP;

            break;

        defstop
            break;
    } // End SWITCH

    return lpYYRes;
} // End SysFnMonDR_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydDR_EM_YY
//
//  Dyadic []DR -- Convert To and From Data Representation
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydDR_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydDR_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeLft,        // Left arg storage type
                 aplTypeRht;        // Right ...
    APLNELM      aplNELMLft;        // Left arg NELM
    APLRANK      aplRankLft;        // Left arg rank
    HGLOBAL      hGlbRes = NULL;    // Result global memory handle
    APLINT       aplIntegerLft;     // Left arg as integer
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result
    UBOOL        bScalar = FALSE,   // TRUE iff the result is a simple scalar
                 bRet;              // TRUE iff the result is valid

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht,  NULL      ,  NULL      , NULL);

    // Check for LEFT RANK ERROR
    if (IsMultiRank (aplRankLft))
        goto LEFT_RANK_EXIT;

    // Check for LEFT LENGTH ERROR
    if (!IsSingleton (aplNELMLft))
        goto LEFT_LENGTH_EXIT;

    // Check for LEFT DOMAIN ERROR
    if (!IsSimpleGlbNum (aplTypeLft))
        goto LEFT_DOMAIN_EXIT;

    // Get and validate the first item in a token
    aplIntegerLft = ValidateFirstItemToken (aplTypeLft, lptkLftArg, &bRet);

    // Check for DOMAIN ERROR
    if (!bRet)
        goto LEFT_DOMAIN_EXIT;

    // Ensure the left arg is valid
    switch (aplIntegerLft)
    {
        case DR_SHOW:
            // Return a character vector representation
            return SysFnDR_Show_EM_YY (lptkRhtArg, lptkFunc);

        case DR_FLOAT2CHAR:
            if (IsSimpleNum (aplTypeRht))
                // Convert a float to its 16-digit hexadecimal character representation
                hGlbRes = SysFnDR_FloatToChar_EM (lptkRhtArg, lptkFunc);
            else
                // Convert a character representation of a float to a float
                hGlbRes = SysFnDR_CharToFloat_EM (lptkRhtArg, lptkFunc, &bScalar);
            break;

        case DR_INT2CHAR:
            if (IsSimpleNum (aplTypeRht))
                // Convert an integer to its 16-digit hexadecimal character representation
                hGlbRes = SysFnDR_IntToChar_EM (lptkRhtArg, lptkFunc);
            else
                // Convert a character representation of an integer to an integer
                hGlbRes = SysFnDR_CharToInt_EM (lptkRhtArg, lptkFunc, &bScalar);
            break;

        case DR_BOOL:
            return SysFnDR_Convert_EM_YY (ARRAY_BOOL,  lptkRhtArg, lptkFunc);

        case DR_CHAR16:
            return SysFnDR_Convert_EM_YY (ARRAY_CHAR,  lptkRhtArg, lptkFunc);

        case DR_INT64:
            return SysFnDR_Convert_EM_YY (ARRAY_INT,   lptkRhtArg, lptkFunc);

        case DR_FLOAT:
            return SysFnDR_Convert_EM_YY (ARRAY_FLOAT, lptkRhtArg, lptkFunc);

        case DR_APA:
            return SysFnDR_Convert_EM_YY (ARRAY_APA,   lptkRhtArg, lptkFunc);

        case DR_HETERO:
        case DR_NESTED:
        default:
            return PrimFnDomainError_EM (lptkFunc APPEND_NAME_ARG);

        case DR_CHAR8:
        case DR_CHAR32:
        case DR_INT8:
        case DR_INT16:
        case DR_INT32:
        case DR_COMPLEX_I:
        case DR_QUATERNIONS_I:
        case DR_OCTONIONS_I:
        case DR_COMPLEX_F:
        case DR_QUATERNIONS_F:
        case DR_OCTONIONS_F:
        case DR_COMPLEX_Q:
        case DR_QUATERNIONS_Q:
        case DR_OCTONIONS_Q:
        case DR_COMPLEX_V:
        case DR_QUATERNIONS_V:
        case DR_OCTONIONS_V:
        case DR_RAT:
        case DR_VFP:
            return PrimFnNonceError_EM (lptkFunc APPEND_NAME_ARG);
    } // End SWITCH

    if (hGlbRes EQ NULL)
        return NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // If the result is a simple scalar, ...
    if (bScalar)
    {
        IMM_TYPES  immTypeRes;
        APLLONGEST aplLongestRes;

        // Get the first value
        GetFirstValueGlb (hGlbRes,                          // The global memory handle
                          NULL,                             // Ptr to integer (or Boolean) (may be NULL)
                          NULL,                             // ...    float (may be NULL)
                          NULL,                             // ...    char (may be NULL)
                         &aplLongestRes,                    // ...    longest (may be NULL)
                          NULL,                             // ...    LPSYMENTRY or HGLOBAL (may be NULL)
                         &immTypeRes,                       // ...    immediate type (see IMM_TYPES) (may be NULL)
                          NULL,                             // ...    array type -- ARRAY_TYPES (may be NULL)
                          FALSE);                           // TRUE iff we should expand LPSYMENTRY into immediate value
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = immTypeRes;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkLongest  = aplLongestRes;
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } else
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } // End IF/ELSE

    return lpYYRes;

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    return NULL;

LEFT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkLftArg);
    return NULL;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
    return NULL;
} // End SysFnDydDR_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDR_Convert_EM_YY
//
//  Return the <aplTypeRes> representation of the right arg
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDR_Convert_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDR_Convert_EM_YY
    (APLSTYPE aplTypeRes,           // Result storage type
     LPTOKEN  lptkRhtArg,           // Ptr to right arg token
     LPTOKEN  lptkFunc)             // Ptr to function token

{
    HGLOBAL      hGlbRht = NULL,    // Right arg global memory handle
                 hGlbRes = NULL;    // Result    ...
    LPVOID       lpMemRht = NULL,   // Ptr to right arg global memory
                 lpMemRes = NULL;   // Ptr to result    ...
    APLSTYPE     aplTypeRht;        // Right arg storage type
    APLNELM      aplNELMRht,        // Right arg NELM
                 aplNELMRes;        // Result    ...
    APLRANK      aplRankRht,        // Right arg rank
                 aplRankRes;        // Result rank
    APLUINT      ByteRes,           // # bytes in the result
                 aplColsRht,        // Right arg # cols
                 aplColsRes,        // Result    # cols
                 uCnt;              // Loop counter
    APLINT       apaOffRes,         // Result APA offset
                 apaMulRes;         // ...        multiplier
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
    APLLONGEST   aplLongestRht;     // Right arg as immediate

    // The result is an array of the same shape as R with
    //   the last coordinate multiplied by the # bits per
    //   right arg element divided by the bits per result element.

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, &aplColsRht);

    // Get right arg's global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Check on APA to APA
    if (IsSimpleAPA (aplTypeRes) && IsSimpleAPA (aplTypeRht))
    {
        // Copy the right arg
        hGlbRes = CopySymGlbDirAsGlb (hGlbRht);

        goto YYALLOC_EXIT;
    } // End IF

    // Check on result APA
    if (IsSimpleAPA (aplTypeRes))
    {
        // Check on RIGHT RANK ERROR
        if (!IsVector (aplRankRht))
            goto RIGHT_RANK_EXIT;

        // Check on RIGHT LENGTH ERROR
        if ((aplNELMRht * SysFnDR_BPE (aplTypeRht)) < (3 * SysFnDR_BPE (aplTypeRes)))
            goto RIGHT_LENGTH_EXIT;
    } // End IF

    // Check for RIGHT DOMAIN ERROR
    if (!IsSimpleNH (aplTypeRht))
        goto RIGHT_DOMAIN_EXIT;

    // Handle empty right args & result
    if (IsEmpty (aplNELMRht))
        aplTypeRes = ARRAY_BOOL;

    // Calculate the result cols
    aplColsRes = SysFnDR_GetCols_EM (aplTypeRes, aplTypeRht, aplColsRht, lptkFunc);
    if (aplColsRes EQ NEG1U)
        goto ERROR_EXIT;

    // Check on result APA
    if (IsSimpleAPA (aplTypeRes))
    {
        APLUINT  uLim;
        LPAPLINT lpMemDataRht;

        // Skip over the header and dimensions to the data
        lpMemDataRht = VarArrayBaseToData (lpMemRht, aplRankRht);

        // Get the APA offset
        apaOffRes = ((LPAPLAPA) lpMemDataRht)->Off;

        // Get the APA multiplier
        apaMulRes = ((LPAPLAPA) lpMemDataRht)->Mul;

        // Calculate # integers in the right arg to process
        uLim = (aplNELMRht * SysFnDR_BPE (aplTypeRht)) / SysFnDR_BPE (aplTypeRes);

        // Get the APA NELM
        for (uCnt = 2, aplNELMRes = 1; uCnt < uLim; uCnt++)
        {
            APLINT iAccum;                  // Accumulator for result NELM
            UBOOL  bRet;                    // TRUE iff the result is valid

            // Get the next value as an integer
            iAccum = lpMemDataRht[uCnt];

            // Check for RIGHT DOMAIN ERROR
            if (iAccum < 0)
                goto RIGHT_DOMAIN_EXIT;

            // Save in NELM
            aplNELMRes = _imul64 (aplNELMRes, iAccum, &bRet);

            // Check for overflow
            if (!bRet)
                goto RIGHT_DOMAIN_EXIT;
        } // End FOR

        // Set the # result cols
        aplColsRes = aplNELMRes;
    } else
    // Check on right arg APA
    if (IsSimpleAPA (aplTypeRht))
    {
        // Get the # bits per element for the result
        aplNELMRes = (2 * SysFnDR_BPE (aplTypeRht)) / SysFnDR_BPE (aplTypeRes);

        // Count in the shape vector, too
        aplNELMRes += (aplRankRht * SysFnDR_BPE (aplTypeRht)) / SysFnDR_BPE (aplTypeRes);;

        // Set the # result cols
        aplColsRes = aplNELMRes;
    } else
    if (!IsZeroDim (aplColsRht))
        aplNELMRes = aplColsRes * (aplNELMRht / aplColsRht);
    else
        aplNELMRes = 0;

    // Calculate the result rank
    if (IsSimpleAPA (aplTypeRes))
        aplRankRes = ((aplNELMRht * SysFnDR_BPE (aplTypeRht)) / SysFnDR_BPE (aplTypeRes)) - 2;
    else
    if (IsSimpleAPA (aplTypeRht))
        aplRankRes = 1;
    else
        aplRankRes = max (aplRankRht, 1);

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

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
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // If the right arg is a global, ...
    if (hGlbRht)
    {
        LPAPLDIM lpMemDimRes;           // Ptr to result dimensions

        // Get the result dimension ptr
        lpMemDimRes = VarArrayBaseToDim (lpMemRes);

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

        // If the result is an APA, save the offset & multiplier
        if (IsSimpleAPA (aplTypeRes))
        {
            ((LPAPLAPA) lpMemRes)->Off = apaOffRes;
            ((LPAPLAPA) lpMemRes)->Mul = apaMulRes;

            // Skip over the header and dimensions to the data
            lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

            // Save the result shape vector
            CopyMemory (lpMemDimRes, &((LPAPLUINT) lpMemRht)[2], (APLU3264) BytesIn (ARRAY_INT, aplRankRes));
        } else
        {
            // Copy the right arg dimensions to the result unless APA
            if (!IsSimpleAPA (aplTypeRht))
                CopyMemory (lpMemDimRes,
                            VarArrayBaseToDim (lpMemRht),
                            (APLU3264) BytesIn (ARRAY_INT, aplRankRes - 1));
            // Save the last dimension
            lpMemDimRes[aplRankRes - 1] = aplColsRes;

            // If the right arg is an APA, save its shape in the result
            if (IsSimpleAPA (aplTypeRht))
                CopyMemory (&((LPAPLINT) lpMemRes)[2],
                             VarArrayBaseToDim (lpMemRht),
                             (APLU3264) BytesIn (ARRAY_INT, aplRankRht));
            // Skip over the header and dimensions to the data
            lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

            // Copy the right arg data to the result
            CopyMemory (lpMemRes, lpMemRht, (APLU3264) BytesIn (aplTypeRht, aplNELMRht));
        } // End IF/ELSE
    } else
    {
        // Save the last dimension
        *VarArrayBaseToDim (lpMemRes) = aplColsRes;

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

        // Copy the right arg to the result
        *((LPAPLLONGEST) lpMemRes) = aplLongestRht;
    } // End IF/ELSE
YYALLOC_EXIT:
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
ERROR_EXIT:
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

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    return NULL;

RIGHT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkRhtArg);
    return NULL;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    return NULL;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End SysFnDR_Convert_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDR_BPE
//
//  Return the # bits per element for a given storage type
//***************************************************************************

UINT SysFnDR_BPE
    (APLSTYPE aplTypeRht)           // Right arg storage type

{
    static UINT uBPE[] = {BPE_VEC};

    return uBPE[aplTypeRht];
} // End SysFnDR_BPE


//***************************************************************************
//  $SysFnDR_GetCols_EM
//
//  Return the # result cols for a given right arg storage type & cols
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDR_GetCols_EM"
#else
#define APPEND_NAME
#endif

APLNELM SysFnDR_GetCols_EM
    (APLSTYPE aplTypeRes,               // Result storage type
     APLSTYPE aplTypeRht,               // Right arg storage type
     APLUINT  aplColsRht,               // Right arg # cols
     LPTOKEN  lptkFunc)                 // Ptr to function token

{
    UINT uBPERht,                       // # bits per element for the right arg
         uBPERes;                       // # bits per element for the result

    // Get the # bits per element for the right arg & result
    uBPERht = SysFnDR_BPE (aplTypeRht);
    uBPERes = SysFnDR_BPE (aplTypeRes);

    if (uBPERht NE 0 && uBPERes NE 0)
    {
        // Calculate # bits in the right arg cols
        aplColsRht *= uBPERht;

        // Ensure the right arg BPE is a multiple of the result BPE
        if (0 NE (aplColsRht % uBPERes))
            goto RIGHT_DOMAIN_EXIT;

        // Return # result cols
        return (aplColsRht / uBPERes);
    } else
        return 0;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkFunc);
    return NEG1U;
} // End SysFnDR_GetCols_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDR_Show_EM_YY
//
//  Return a character representation of the storage type
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDR_Show_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDR_Show_EM_YY
    (LPTOKEN  lptkRhtArg,
     LPTOKEN  lptkFunc)

{
    APLSTYPE          aplTypeRht;       // Right arg storage type
    APLNELM           aplNELMRht;       // ...       NELM
    APLRANK           aplRankRht;       // ...       rank
    APLUINT           ByteRes,          // # bytes in the result
                      uRht;             // Loop counter
    HGLOBAL           hGlbRht,          // Right arg global memory handle
                      hGlbRes;          // Result    ...
    LPVARARRAY_HEADER lpHeader;         // Ptr to right arg array header
    LPVOID            lpMemRes;         // Ptr to result global memory
    LPPL_YYSTYPE      lpYYRes;          // Ptr to the result
    APLNELM           aplNELMRes;       // Result NELM
    APLCHAR           wszTemp[512];     // Temporary APLCHARs
    UBOOL             bInit = FALSE;    // TRUE iff array properties have been initialized

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            wsprintfW (wszTemp,
                      L"Boolean (%d):  1 bit per element",
                       DR_BOOL);
            break;

        case ARRAY_INT:
            wsprintfW (wszTemp,
                      L"Integer (%d):  64 bits per element",
                       DR_INT64);
            break;

        case ARRAY_FLOAT:
            wsprintfW (wszTemp,
                      L"Floating Point (%d):  64 bits per element",
                       DR_FLOAT);
            break;

        case ARRAY_CHAR:
            wsprintfW (wszTemp,
                      L"Character (%d):  16 bits per element",
                       DR_CHAR16);
            break;

        case ARRAY_APA:
            wsprintfW (wszTemp,
                      L"Arithmetic Progression Array (%d):  64 bit offset + 64 bit multiplier",
                      DR_APA);
            break;

        case ARRAY_NESTED:
            wsprintfW (wszTemp,
                      L"Nested Array (%d):  PTR bits per element",
                      DR_NESTED);
            break;

        case ARRAY_HETERO:
            wsprintfW (wszTemp,
                      L"Heterogeneous Array (%d):  PTR bits per element",
                      DR_HETERO);
            break;

        case ARRAY_RAT:
            wsprintfW (wszTemp,
                      L"Rational (%d):  arbitrary precision numerator and denominator",
                      DR_RAT);
            break;

        case ARRAY_VFP:
            wsprintfW (wszTemp,
                      L"Variable Floating Point (%d):  variable precision mantissa, 32-bit exponent",
                      DR_VFP);
            break;

        defstop
            break;
    } // End SWITCH

    // Get right arg's global ptr
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpHeader);

    // Append array properties
    if (IsPermVector0 (lpHeader))
    {
        if (!bInit)
        {
            lstrcatW (wszTemp, L" --");
            bInit = TRUE;
        } // End IF

        lstrcatW (wszTemp, L" " AP_PV0);
    } // End IF

    if (IsPermVector1 (lpHeader))
    {
        if (!bInit)
        {
            lstrcatW (wszTemp, L" --");
            bInit = TRUE;
        } // End IF

        lstrcatW (wszTemp, L" " AP_PV1);
    } // End IF

    if (IsAll2s (lpHeader))
    {
        if (!bInit)
        {
            lstrcatW (wszTemp, L" --");
            bInit = TRUE;
        } // End IF

        lstrcatW (wszTemp, L" " AP_ALL2S);
    } // End IF

    if (IsVfp (aplTypeRht))
    {
        LPAPLVFP lpaplVfp;          // Ptr to VFP value
        APLUINT  uCommPrec;         // Ptr to common VFP array precision (0 if none) (may be NULL)

        // If the array is all the same precision, ...

        // Skip over the header and dimensions to the data
        lpaplVfp = VarArrayBaseToData (lpHeader, aplRankRht);

        // Get the initial precision
        uCommPrec = mpfr_get_prec (lpaplVfp++);

        for (uRht = 1; uRht < aplNELMRht; uRht++)
        if (uCommPrec NE mpfr_get_prec (lpaplVfp++))
        {
            // Mark as none
            uCommPrec = 0;

            break;
        } // End FOR

        if (!bInit)
        {
            lstrcatW (wszTemp, L" --");
            bInit = TRUE;
        } // End IF

        // If there's a common precision, ...
        if (uCommPrec NE 0)
        {
            lstrcatW (wszTemp, L" " AP_FPC);
            wsprintfW (&wszTemp[lstrlenW (wszTemp)],
                        L"%I64u ",
                        uCommPrec);
        } else
            lstrcatW (wszTemp, L" " AP_FPC L"-Mixed");
    } // End IF

    // If the ptr is valid, ...
    if (hGlbRht && lpHeader)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpHeader = NULL;
    } // End IF

    // Get the result NELM
    aplNELMRes = lstrlenW (wszTemp);

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRes, 1);

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
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Save the dimension in the result
    *VarArrayBaseToDim (lpMemRes) = aplNELMRes;

    // Point to the data (APLAPA struct)
    lpMemRes = VarArrayBaseToData (lpMemRes, 1);

    // Copy the text to the result
    CopyMemory (lpMemRes, wszTemp, (APLU3264) BytesIn (ARRAY_CHAR, aplNELMRes));

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End SysFnDR_Show_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDR_FloatToChar_EM
//
//  Convert a float to its 16-digit hexadecimal character representation
//***************************************************************************

HGLOBAL SysFnDR_FloatToChar_EM
    (LPTOKEN lptkRhtArg,
     LPTOKEN lptkFunc)

{
    return SysFnDR_IntFloatToChar_EM (lptkRhtArg,
                                      lptkFunc,
                                     &SysFnDR_ConvertFloatToChar);
} // End SysFnDr_FloatToChar_EM


//***************************************************************************
//  $SysFnDR_ConvertFloatToChar
//
//  Convert a float to APLCHARs.
//***************************************************************************

void SysFnDR_ConvertFloatToChar
    (LPAPLCHAR  lpMemRes,               // Ptr to result global memory
     APLSTYPE   aplTypeRht,             // Right arg storage type
     APLLONGEST aplLongestRht,          // Right arg immediate value
     LPUBOOL    lpbRet)                 // Ptr to TRUE iff the result is valid

{
    APLFLOAT aplFloatRht;               // Temporary float

    if (IsSimpleFlt (aplTypeRht))
        // Copy the float
        aplFloatRht = *(LPAPLFLOAT) &aplLongestRht;
    else
        // If the value is integer, convert it to float
        aplFloatRht = (APLFLOAT) (APLINT) aplLongestRht;

    // Convert a floating point number to APLCHARs
    IntFloatToAplchar (lpMemRes, (LPAPLLONGEST) &aplFloatRht);
} // End SysFnDR_ConvertFloatToChar


//***************************************************************************
//  $SysFnDR_IntToChar_EM
//
//  Convert an integer its 16-digit hexadecimal character representation
//***************************************************************************

HGLOBAL SysFnDR_IntToChar_EM
    (LPTOKEN lptkRhtArg,
     LPTOKEN lptkFunc)

{
    return SysFnDR_IntFloatToChar_EM (lptkRhtArg,
                                      lptkFunc,
                                     &SysFnDR_ConvertIntToChar);
} // End SysFnDr_IntToChar_EM


//***************************************************************************
//  $SysFnDR_ConvertIntToChar
//
//  Convert a float to APLCHARs.
//***************************************************************************

void SysFnDR_ConvertIntToChar
    (LPAPLCHAR  lpMemRes,               // Ptr to result global memory
     APLSTYPE   aplTypeRht,             // Right arg storage type
     APLLONGEST aplLongestRht,          // Right arg immediate value
     LPUBOOL    lpbRet)                 // Ptr to TRUE iff the result is valid

{
    APLINT aplIntRht;                   // Temporary integer

    if (IsSimpleFlt (aplTypeRht))
    {
        // If the value is float, convert it to integer
        aplIntRht = (APLINT) *(LPAPLFLOAT) &aplLongestRht;

        if (((APLFLOAT) aplIntRht) NE *(LPAPLFLOAT) &aplLongestRht)
        {
            *lpbRet = FALSE;

            return;
        } // End IF
    } else
        // Copy the integer
        aplIntRht = aplLongestRht;

    // Convert an integer to APLCHARs
    IntFloatToAplchar (lpMemRes, &aplIntRht);

    *lpbRet = TRUE;
} // End SysFnDR_ConvertIntToChar


//***************************************************************************
//  $SysFnDR_IntFloatToChar_EM
//
//  Convert an integer/float to its 16-digit hexadecimal character representation
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDR_IntFloatToChar_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL SysFnDR_IntFloatToChar_EM
    (LPTOKEN         lptkRhtArg,        // Ptr to right argtoken
     LPTOKEN         lptkFunc,          // Ptr to error function
     LPCONVERTTOCHAR lpConvertToChar)   // Ptr to ConvertToChar function

{
    APLSTYPE   aplTypeRht;              // Right arg storage type
    APLNELM    aplNELMRht;              // Right arg NELM
    APLRANK    aplRankRht;              // Right arg rank
    APLUINT    ByteRes,                 // # bytes in the result
               uRes;                    // Result loop counter
    HGLOBAL    hGlbRht,                 // Right arg global memory handle
               hGlbRes = NULL;          // Result    ...
    LPVOID     lpMemRht = NULL,         // Ptr to right arg global memory
               lpMemRes = NULL;         // Ptr to result    ...
    APLLONGEST aplLongestRht;           // Immediate value
    UBOOL      bRet = TRUE;             // TRUE iff the result is valid

    // Get the attributes (Type, NELM, and Rank)
    //   of the right args
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Ensure the right arg is simple numeric
    if (!IsSimpleNum (aplTypeRht))
        goto RIGHT_DOMAIN_EXIT;

    // Convert the FP argument to displayable chars

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRht * 16, aplRankRht + 1);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result.
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRht * 16;
    lpHeader->Rank       = aplRankRht + 1;
#undef  lpHeader

    // Get right arg's global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemRes);
    if (lpMemRht)
    {
        // Skip over the header to the dimensions
        lpMemRht = VarArrayBaseToDim (lpMemRht);

        //***************************************************************
        // Copy the dimensions from the right arg
        //   to the result's dimension
        //***************************************************************
        for (uRes = 0; uRes < aplRankRht; uRes++)
            *((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemRht)++;
    } // End IF

    // The last dimension is 16
    *((LPAPLDIM) lpMemRes)++ = 16;

    // lpMemRes now points to the result's data

    // If the right arg is not an immediate, ...
    if (lpMemRht)
    {
        // Loop through the right arg converting it to the result
        for (uRes = 0; bRet && uRes < aplNELMRht; uRes++, ((LPAPLCHAR) lpMemRes += 16))
        {
            // Get the next value from the right arg
            GetNextValueMem (lpMemRht,      // Ptr to item global memory data
                             aplTypeRht,    // Item storage type
                             uRes,          // Index into item
                             NULL,          // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                            &aplLongestRht, // Ptr to result immediate value (may be NULL)
                             NULL);         // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
            // Convert to APLCHARs
            (*lpConvertToChar) (lpMemRes, aplTypeRht, aplLongestRht, &bRet);
        } // End FOR
    } else
    // The right arg is an immediate
        // Convert to APLCHARs
        (*lpConvertToChar) (lpMemRes, aplTypeRht, aplLongestRht, &bRet);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Check for error
    if (bRet)
        goto NORMAL_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
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
    // We no longer need this ptr
    if (lpMemRht)
    {
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return hGlbRes;
} // End SysFnDR_IntFloatToChar_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDR_CharToFloat_EM
//
//  Convert a 16-digit hexadecimal character representation of a float to a float
//***************************************************************************

HGLOBAL SysFnDR_CharToFloat_EM
    (LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkFunc,              // Ptr to error function token
     LPUBOOL lpbScalar)             // Ptr to TRUE iff result is a simple scalar

{
    return SysFnDR_CharToIntFloat_EM (lptkRhtArg,
                                      lptkFunc,
                                      ARRAY_FLOAT,
                                      lpbScalar);
} // End SysFnDR_CharToFloat_EM


//***************************************************************************
//  $SysFnDR_CharToInt_EM
//
//  Convert a 16-digit hexadecimal character representation of a float to a float
//***************************************************************************

HGLOBAL SysFnDR_CharToInt_EM
    (LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkFunc,              // Ptr to error function token
     LPUBOOL lpbScalar)             // Ptr to TRUE iff result is a simple scalar

{
    return SysFnDR_CharToIntFloat_EM (lptkRhtArg,
                                      lptkFunc,
                                      ARRAY_INT,
                                      lpbScalar);
} // End SysFnDR_CharToInt_EM


//***************************************************************************
//  $SysFnDR_CharToIntFloat_EM
//
//  Convert a 16-digit hexadecimal character representation of
//    an integer/float to a integer/float
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDR_CharToIntFloat_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL SysFnDR_CharToIntFloat_EM
    (LPTOKEN  lptkRhtArg,                   // Ptr to right arg token
     LPTOKEN  lptkFunc,                     // Ptr to error function token
     APLSTYPE aplTypeRes,                   // Result storage type
     LPUBOOL  lpbScalar)                    // Ptr to TRUE iff result is a simple scalar

{
    APLSTYPE   aplTypeRht;                  // Right arg storage type
    APLNELM    aplNELMRht,                  // Right arg NELM
               aplNELMRes,                  // Result    ...
               aplColsRht;                  // Right arg # cols
    APLRANK    aplRankRht,                  // Right arg rank
               aplRankRes;                  // Result    ...
    APLUINT    ByteRes,                     // # bytes in the result
               uRht,                        // Loop counter
               uRes;                        // Loop counter
    APLCHAR    aplChar;                     // Temporary char
    HGLOBAL    hGlbRht = NULL,              // Right arg global memory handle
               hGlbRes = NULL;              // Result    ...
    LPAPLCHAR  lpMemRht = NULL,             // Ptr to right arg global memory
               lpMemDataRht;                // Ptr to right arg global memory
    LPVOID     lpMemRes = NULL;             // Ptr to result    ...
    APLLONGEST aplLongestRht;               // Right arg accumulator

    // Get the attributes (Type, NELM, and Rank)
    //   of the right args
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, &aplColsRht);

    // Ensure the right arg has 16 cols (which implies that aplRankRht > 0)
    if (aplColsRht NE 16)
        goto RIGHT_LENGTH_EXIT;

    // Ensure the right arg is simple char
    if (!IsSimpleChar (aplTypeRht))
        goto RIGHT_DOMAIN_EXIT;

    // Get right arg's global ptrs
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);
    Assert (hGlbRht NE NULL);

    // Skip over the header to the data
    lpMemDataRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    // Ensure the right arg is all hexadecimal digits
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    {
        // Get the next char
        aplChar = lpMemDataRht[uRht];

        // Check it for hexadecimality
        if (!((L'0' <= aplChar
            &&         aplChar <= L'9')
           || (L'a' <= aplChar
            &&         aplChar <= L'z')
           || (L'A' <= aplChar
            &&         aplChar <= L'Z')))
            goto RIGHT_DOMAIN_EXIT;
    } // End FOR

    // Get the result NELM and rank
    aplNELMRes = aplNELMRht / 16;
    aplRankRes = aplRankRht - 1;

    // Tell the caller is the result is a simple scalar
    *lpbScalar = IsScalar (aplRankRes);

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_FLOAT, aplNELMRes, aplRankRes);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result.
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Skip over the header to the dimensions
    (LPAPLDIM) lpMemRes = VarArrayBaseToDim (lpMemRes);

    // Skip over the header to the dimensions
    (LPAPLDIM) lpMemRht = VarArrayBaseToDim (lpMemRht);

    //***************************************************************
    // Copy the dimensions from the right arg (except for the last one)
    //   to the result's dimension
    //***************************************************************
    for (uRes = 0; uRes < aplRankRes; uRes++)
        *((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemRht)++;

    // Skip over the right arg last dimension
    Assert (16 EQ *(LPAPLDIM) lpMemRht); ((LPAPLDIM) lpMemRht)++;

    // Convert the char representation of an integer/float to an integer/float

    // Loop through the right arg converting it to the result
    for (uRes = 0; uRes < aplNELMRes; uRes++, lpMemRht += 16)
    {
        // Initialize accumulator
        aplLongestRht = 0;

        // Convert hex chars to hex
        for (uRht = 0; uRht < 16; uRht++)
        {
            // Get the next char
            aplChar = lpMemRht[uRht];

            // Convert the char to a hexadecimal digit
            if (L'0' <= aplChar
              &&        aplChar <= L'9')
                aplChar -= L'0';
            else
            if (L'a' <= aplChar
             &&         aplChar <= L'z')
                aplChar -= L'a' - 10;
            else
            if (L'A' <= aplChar
              &&        aplChar <= L'Z')
                aplChar -= L'A' - 10;
            else
                // We should never get here
                DbgStop ();

            // Shift and accumulate
            aplLongestRht = (aplLongestRht << 4) | aplChar;
        } // End FOR

        // Save in the result
        if (aplTypeRes EQ ARRAY_FLOAT)
            *((LPAPLFLOAT) lpMemRes)++ = *(LPAPLFLOAT) &aplLongestRht;
        else
            *((LPAPLINT)   lpMemRes)++ =                aplLongestRht;
    } // End FOR

    goto NORMAL_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

RIGHT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
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

    return hGlbRes;
} // End SysFnDR_CharToIntFloat_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_dr.c
//***************************************************************************
