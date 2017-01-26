//***************************************************************************
//  NARS2000 -- Primitive Function -- Epsilon Underbar
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


//***************************************************************************
//  $PrimFnEpsilonUnderbar_EM_YY
//
//  Primitive function for monadic and dyadic EpsilonUnderbar (ERROR and "find")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnEpsilonUnderbar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnEpsilonUnderbar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_EPSILONUNDERBAR);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc APPEND_NAME_ARG);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonEpsilonUnderbar_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydEpsilonUnderbar_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnEpsilonUnderbar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnEpsilonUnderbar_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic EpsilonUnderbar
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnEpsilonUnderbar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnEpsilonUnderbar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnEpsilonUnderbar_EM_YY,// Ptr to primitive function routine
                                    lptkLftArg,         // Ptr to left arg token
                                    lptkFunc,           // Ptr to function token
                                    lptkRhtArg,         // Ptr to right arg token
                                    lptkAxis);          // Ptr to axis token (may be NULL)
} // End PrimProtoFnEpsilonUnderbar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonEpsilonUnderbar_EM_YY
//
//  Primitive function for monadic EpsilonUnderbar (ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonEpsilonUnderbar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonEpsilonUnderbar_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End PrimFnMonEpsilonUnderbar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydEpsilonUnderbar_EM_YY
//
//  Primitive function for dyadic EpsilonUnderbar ("find")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydEpsilonUnderbar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydEpsilonUnderbar_EM_YY
    (LPTOKEN lptkLftArg,                    // Ptr to left arg token
     LPTOKEN lptkFunc,                      // Ptr to function token
     LPTOKEN lptkRhtArg,                    // Ptr to right arg token
     LPTOKEN lptkAxis)                      // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeLft,           // Left arg storage type
                      aplTypeRht,           // Right ...
                      aplTypeRes;           // Result ...
    APLNELM           aplNELMLft,           // Left arg NELM
                      aplNELMRht;           // Right ...
    APLRANK           aplRankLft,           // Left arg rank
                      aplRankRht;           // Right ...
    APLDIM            aplColsLft,           // Left arg  # cols
                      aplColsRht,           // Right ...
                      Dim1 = 1;             // Constant 1
    HGLOBAL           hGlbLft = NULL,       // Left arg global memory handle
                      hGlbRht = NULL,       // Right ...
                      hGlbRes = NULL,       // Result   ...
                      hGlbOdoRht = NULL,    // Right arg odometer global memory handle
                      hGlbWVecRht = NULL,   // ...       weighting vector global memory handle
                      hGlbDimDiff = NULL,   // Dimension difference global memory handle
                      hGlbDimTmp = NULL,    // Test odometer global memory handle
                      hGlbWVecTst = NULL,   // ...  weighting vector ...
                      hGlbOdoTst = NULL,    // ...  odometer         ...
                      hGlbKmpNext = NULL;   // KMP temp global memory handle
    LPVOID            lpMemLft = NULL,      // Ptr to left arg global memory
                      lpMemRht = NULL;      // ...    right ...
    LPAPLDIM          lpMemDimLft,          // Ptr to left arg dimensions
                      lpMemDimRht,          // ...    right ...
                      lpMemDimTmp = NULL;   // ...    temp     ...
    LPAPLBOOL         lpMemRes = NULL;      // Ptr to result   ...
    APLUINT           ByteRes,              // # bytes in the result
                      uRes;                 // Accumulator
    APLINT            iDim;                 // Loop counter
    LPAPLUINT         lpMemOdoRht = NULL,   // Ptr to right arg odometer global memory
                      lpMemWVecRht = NULL,  // ...    ...      weighting vector global memory
                      lpMemWVecTst = NULL,  // ...    test weighting vector global memory
                      lpMemOdoTst = NULL,   // ...         odometer global memory
                      lpMemDimDiff = NULL;  // ...    dimension difference global memory
    LPAPLINT          lpMemKmpNext = NULL;  // Ptr to KMP temp global memory
    APLLONGEST        aplLongestLft,        // Left arg immediate value
                      aplLongestRht;        // Right ...
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    APLFLOAT          fQuadCT;              // []CT
    UBOOL             bSome1s = FALSE,      // TRUE iff in empty case fill with some 1s
                      bEmpty;               // TRUE iff either arg is empty

    // Get the current value of []CT
    fQuadCT = GetQuadCT ();

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, &aplColsLft);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, &aplColsRht);

    //***************************************************************
    // Split off the case of scalar right arg as the code below
    //   doesn't handle it well
    //***************************************************************
    if (IsScalar (aplRankRht))
    {
        // Handle via {match}
        lpYYRes =
          PrimFnDydEqualUnderbar_EM_YY (lptkLftArg,     // Ptr to left arg token
                                        lptkFunc,       // Ptr to function token
                                        lptkRhtArg,     // Ptr to right arg token
                                        NULL);          // Ptr to axis token (may be NULL)
        goto NORMAL_EXIT;
    } // End IF

    // Get left and right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // If the right arg is not an immediate, ...
    if (lpMemRht)
    {
        //***************************************************************
        // Calculate space needed for the result
        //***************************************************************
        ByteRes = CalcArraySize (ARRAY_BOOL, aplNELMRht, aplRankRht);

        //***************************************************************
        // Check for overflow
        //***************************************************************
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;

        //***************************************************************
        // Now we can allocate the storage for the result
        //***************************************************************
        hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (hGlbRes EQ NULL)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemRes = MyGlobalLock000 (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = ARRAY_BOOL;
////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = aplNELMRht;
        lpHeader->Rank       = aplRankRht;
#undef  lpHeader

        // Fill in the result's dimension

        // Skip over the header to the dimensions
        lpMemRes = (LPAPLBOOL) VarArrayBaseToDim (lpMemRes);

        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemRht);

        // Copy the right arg dimensions to the result
        CopyMemory (lpMemRes, lpMemDimRht, (APLU3264) aplRankRht * sizeof (APLDIM));

        // Skip over the dimensions to the data
        lpMemRes = VarArrayDimToData (lpMemRes, aplRankRht);
        lpMemRht = VarArrayDimToData (lpMemDimRht, aplRankRht);
    } else
    {
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_BOOL;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkBoolean  =                // Filled in below
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

        // Fill in the result's dimension

        // Point to the right arg dimension
        lpMemDimRht = &Dim1;

        // Point to the result immediate value
        lpMemRes = &lpYYRes->tkToken.tkData.tkBoolean;

        // Point to the right arg immediate value
        lpMemRht = &aplLongestRht;
    } // End IF/ELSE

    if (lpMemLft)
    {
        // Skip over the header to the dimensions
        lpMemDimLft = VarArrayBaseToDim (lpMemLft);

        // Skip over the dimensions to the data
        lpMemLft = VarArrayDimToData (lpMemDimLft, aplRankLft);
    } else
    {
        // Point to the left arg dimension
        lpMemDimLft = &Dim1;

        // Point to the left arg immediate value
        lpMemLft = &aplLongestLft;
    } // End IF

    // Is either arg empty?
    bEmpty = (IsEmpty (aplNELMLft)
           || IsEmpty (aplNELMRht));

    // If the left arg rank is > right arg rank, or
    //    the left and right args are non-empty and different types (numeric vs. char), ...
    if (aplRankLft > aplRankRht
     || (IsNumeric (aplTypeLft) && IsSimpleChar (aplTypeRht) && !bEmpty)
     || (IsNumeric (aplTypeRht) && IsSimpleChar (aplTypeLft) && !bEmpty))
        goto NOMATCH;

    // If the left  arg is empty, or
    //    the right arg is empty, or
    if (bEmpty)
        goto SOME1s;

    // If the left arg rank is < the right arg rank, ...
    if (aplRankLft < aplRankRht)
    {
        // We need a left arg dimension vector which is the
        //   same length as that of the right arg so we can
        //   use it in a call to IncrOdometer.

        // Calculate space needed for the result
        ByteRes = aplRankRht * sizeof (APLUINT);

        // Check for overflow
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;

        //***************************************************************
        // Allocate space for the temporary left arg dimension vector
        //***************************************************************
        hGlbDimTmp = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (hGlbDimTmp EQ NULL)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemDimTmp = MyGlobalLock000 (hGlbDimTmp);

        // Fill in the leading padding (1s)
        for (uRes = 0; uRes < (aplRankRht - aplRankLft); uRes++)
            lpMemDimTmp[uRes] = 1;

        // Copy the actual dimensions
        CopyMemory (&lpMemDimTmp[uRes], lpMemDimLft, (APLU3264) (aplRankLft * sizeof (APLDIM)));
    } else
        // Use the actual left arg dimensions
        lpMemDimTmp = lpMemDimLft;

    //***************************************************************
    // Allocate space for the KMP intermediate array
    //***************************************************************
    hGlbKmpNext =
      DbgGlobalAlloc (GHND, (APLU3264) ((aplColsLft + 1 ) * sizeof (APLINT)));
    if (hGlbKmpNext EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemKmpNext = MyGlobalLock000 (hGlbKmpNext);

    // Calculate space needed for the result
    ByteRes = aplRankRht * sizeof (APLUINT);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    //***************************************************************
    // Allocate space for the dimension difference vector
    //***************************************************************
    hGlbDimDiff = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbDimDiff EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemDimDiff = MyGlobalLock000 (hGlbDimDiff);

    // Ensure the left arg dimensions are <= the right arg dimensions
    //   and save the difference for later use
    for (iDim = aplRankLft; iDim > 0; iDim--)
    if (0 > (APLINT) (lpMemDimDiff[aplRankRht - iDim] = (lpMemDimRht[aplRankRht - iDim] - lpMemDimLft[aplRankLft - iDim])))
        goto NOMATCH;

    // Fill in the leading entries (if any)
    for (iDim = aplRankRht - aplRankLft - 1; iDim >= 0; iDim--)
        lpMemDimDiff[iDim] = lpMemDimRht[iDim] - 1;

    // Calculate space needed for the result
    ByteRes = aplRankRht * sizeof (APLUINT);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    //***************************************************************
    // Allocate space for the right arg weighting vector which is
    //   {times}{backscan}{rho}R
    // Note we do not weigh the columns as they are looped though
    //   separately.
    //***************************************************************
    hGlbWVecRht = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbWVecRht EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemWVecRht = MyGlobalLock000 (hGlbWVecRht);

    // Loop through the dimensions of the right arg in reverse
    //   order {backscan} and compute the cumulative product
    //   (right arg weighting vector).
    // Note we use a signed index variable because we're
    //   walking backwards and the test against zero must be
    //   made as a signed variable.
    uRes = 1;
    for (iDim = aplRankRht - 1; iDim >= 0; iDim--)
    {
        lpMemWVecRht[iDim] = uRes;
        uRes *= lpMemDimRht[iDim];
    } // End FOR

    // Calculate space needed for the result
    ByteRes = aplRankRht * sizeof (APLUINT);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    //***************************************************************
    // Allocate space for the right arg odometer array, one value per
    //   dimension in the right arg.
    //***************************************************************
    hGlbOdoRht = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbOdoRht EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemOdoRht = MyGlobalLock000 (hGlbOdoRht);

    // Calculate space needed for the result
    ByteRes = aplRankRht * sizeof (APLUINT);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    //***************************************************************
    // Allocate space for the test arg weighting vector which is
    //   {times}{backscan}{rho}R
    // Note we do not weigh the columns as they are looped though
    //   separately.
    //***************************************************************
    hGlbWVecTst = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbWVecTst EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemWVecTst = MyGlobalLock000 (hGlbWVecTst);

    // Copy the right arg weighting vector
    CopyMemory (lpMemWVecTst, lpMemWVecRht, (APLU3264) ByteRes);

    // Calculate space needed for the result
    ByteRes = aplRankRht * sizeof (APLUINT);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    //***************************************************************
    // Allocate space for the test arg odometer array, one value per
    //   dimension in the right arg.
    //***************************************************************
    hGlbOdoTst = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbOdoTst EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemOdoTst = MyGlobalLock000 (hGlbOdoTst);

    // Split cases based upon the left & right arg storage types
    if (IsSimpleBool (aplTypeLft) && IsSimpleBool (aplTypeRht))
    {
        // Handle APLBOOL vs. APLBOOL
        if (!PrimFnDydEpsilonUnderbarBvB (lpMemRes,         // Ptr to result global memory data
                                          lpMemDimDiff,     // ...    dimension difference vector
                                          lpMemWVecTst,     // ...    test weighting vector
                                          lpMemOdoTst,      // ...    ...  odometer vector
                                          lpMemKmpNext,     // ...    KMP temp vector

                                          aplNELMLft,       // Left arg NELM
                                          aplColsLft,       // ...      # cols
                                          aplRankLft,       // ...      rank
                                          lpMemLft,         // Ptr to left arg global memory data
                                          lpMemDimTmp,      // ...             dimension vector

                                          aplNELMRht,       // Right arg NELM
                                          aplColsRht,       // ...       # cols
                                          aplRankRht,       // ...       rank
                                          lpMemRht,         // Ptr to right arg global memory data
                                          lpMemDimRht,      // ...              dimension vector
                                          lpMemWVecRht,     // ...              weighting vector
                                          lpMemOdoRht,      // ...              odometer vector

                                          fQuadCT,          // []CT
                                          lpbCtrlBreak,     // Ptr to Ctrl-Break flag
                                          lptkFunc))        // Ptr to function token
            goto ERROR_EXIT;
    } else
    if (IsSimpleBool (aplTypeLft) && IsSimpleFlt (aplTypeRht))
    {
        // Handle APLBOOL vs. APLFLOAT
        if (!PrimFnDydEpsilonUnderbarBvF (lpMemRes,         // Ptr to result global memory data
                                          lpMemDimDiff,     // ...    dimension difference vector
                                          lpMemWVecTst,     // ...    test weighting vector
                                          lpMemOdoTst,      // ...    ...  odometer vector
                                          lpMemKmpNext,     // ...    KMP temp vector

                                          aplNELMLft,       // Left arg NELM
                                          aplColsLft,       // ...      # cols
                                          aplRankLft,       // ...      rank
                                          lpMemLft,         // Ptr to left arg global memory data
                                          lpMemDimTmp,      // ...             dimension vector

                                          aplNELMRht,       // Right arg NELM
                                          aplColsRht,       // ...       # cols
                                          aplRankRht,       // ...       rank
                                          lpMemRht,         // Ptr to right arg global memory data
                                          lpMemDimRht,      // ...              dimension vector
                                          lpMemWVecRht,     // ...              weighting vector
                                          lpMemOdoRht,      // ...              odometer vector

                                          fQuadCT,          // []CT
                                          lpbCtrlBreak,     // Ptr to Ctrl-Break flag
                                          lptkFunc))        // Ptr to function token
            goto ERROR_EXIT;
    } else
    if (IsSimpleAPA (aplTypeLft) && IsSimpleFlt (aplTypeRht))
    {
        // Handle APLAPA vs. APLFLOAT
        if (!PrimFnDydEpsilonUnderbarAvF (lpMemRes,         // Ptr to result global memory data
                                          lpMemDimDiff,     // ...    dimension difference vector
                                          lpMemWVecTst,     // ...    test weighting vector
                                          lpMemOdoTst,      // ...    ...  odometer vector
                                          lpMemKmpNext,     // ...    KMP temp vector

                                          aplNELMLft,       // Left arg NELM
                                          aplColsLft,       // ...      # cols
                                          aplRankLft,       // ...      rank
                                          lpMemLft,         // Ptr to left arg global memory data
                                          lpMemDimTmp,      // ...             dimension vector

                                          aplNELMRht,       // Right arg NELM
                                          aplColsRht,       // ...       # cols
                                          aplRankRht,       // ...       rank
                                          lpMemRht,         // Ptr to right arg global memory data
                                          lpMemDimRht,      // ...              dimension vector
                                          lpMemWVecRht,     // ...              weighting vector
                                          lpMemOdoRht,      // ...              odometer vector

                                          fQuadCT,          // []CT
                                          lpbCtrlBreak,     // Ptr to Ctrl-Break flag
                                          lptkFunc))        // Ptr to function token
            goto ERROR_EXIT;
    } else
    if (IsSimpleInt (aplTypeLft) && IsSimpleInt (aplTypeRht))
    {
        // Handle APLINT/APLAPA vs. APLBOOL/APLINT/APLAPA
        if (!PrimFnDydEpsilonUnderbarIvI (lpMemRes,         // Ptr to result global memory data
                                          lpMemDimDiff,     // ...    dimension difference vector
                                          lpMemWVecTst,     // ...    test weighting vector
                                          lpMemOdoTst,      // ...    ...  odometer vector
                                          lpMemKmpNext,     // ...    KMP temp vector

                                          aplTypeLft,       // Left arg storage type
                                          aplNELMLft,       // ...      NELM
                                          aplColsLft,       // ...      # cols
                                          aplRankLft,       // ...      rank
                                          lpMemLft,         // Ptr to left arg global memory data
                                          lpMemDimTmp,      // ...             dimension vector

                                          aplTypeRht,       // Right arg storage type
                                          aplNELMRht,       // ...       NELM
                                          aplColsRht,       // ...       # cols
                                          aplRankRht,       // ...       rank
                                          lpMemRht,         // Ptr to right arg global memory data
                                          lpMemDimRht,      // ...              dimension vector
                                          lpMemWVecRht,     // ...              weighting vector
                                          lpMemOdoRht,      // ...              odometer vector

                                          fQuadCT,          // []CT
                                          lpbCtrlBreak,     // Ptr to Ctrl-Break flag
                                          lptkFunc))        // Ptr to function token
            goto ERROR_EXIT;
    } else
    if (IsSimpleNum (aplTypeLft) && IsSimpleNum (aplTypeRht))
    {
        // Handle APLFLOAT vs. APLBOOL/APLINT/APLAPA/APLFLOAT
        // Handle APLINT   vs. APLFLOAT
        if (!PrimFnDydEpsilonUnderbarNvN (lpMemRes,         // Ptr to result global memory data
                                          lpMemDimDiff,     // ...    dimension difference vector
                                          lpMemWVecTst,     // ...    test weighting vector
                                          lpMemOdoTst,      // ...    ...  odometer vector
                                          lpMemKmpNext,     // ...    KMP temp vector

                                          aplTypeLft,       // Left arg storage type
                                          aplNELMLft,       // ...      NELM
                                          aplColsLft,       // ...      # cols
                                          aplRankLft,       // ...      rank
                                          lpMemLft,         // Ptr to left arg global memory data
                                          lpMemDimTmp,      // ...             dimension vector

                                          aplTypeRht,       // Right arg storage type
                                          aplNELMRht,       // ...       NELM
                                          aplColsRht,       // ...       # cols
                                          aplRankRht,       // ...       rank
                                          lpMemRht,         // Ptr to right arg global memory data
                                          lpMemDimRht,      // ...              dimension vector
                                          lpMemWVecRht,     // ...              weighting vector
                                          lpMemOdoRht,      // ...              odometer vector

                                          fQuadCT,          // []CT
                                          lpbCtrlBreak,     // Ptr to Ctrl-Break flag
                                          lptkFunc))        // Ptr to function token
            goto ERROR_EXIT;
    } else
    if (IsSimpleChar (aplTypeLft) && IsSimpleChar (aplTypeRht))
    {
        // Handle APLCHAR vs. APLCHAR
        if (!PrimFnDydEpsilonUnderbarCvC (lpMemRes,         // Ptr to result global memory data
                                          lpMemDimDiff,     // ...    dimension difference vector
                                          lpMemWVecTst,     // ...    test weighting vector
                                          lpMemOdoTst,      // ...    ...  odometer vector
                                          lpMemKmpNext,     // ...    KMP temp vector

                                          aplNELMLft,       // Left arg NELM
                                          aplColsLft,       // ...      # cols
                                          aplRankLft,       // ...      rank
                                          lpMemLft,         // Ptr to left arg global memory data
                                          lpMemDimTmp,      // ...             dimension vector

                                          aplNELMRht,       // Right arg NELM
                                          aplColsRht,       // ...       # cols
                                          aplRankRht,       // ...      rank
                                          lpMemRht,         // Ptr to right arg global memory data
                                          lpMemDimRht,      // ...              dimension vector
                                          lpMemWVecRht,     // ...              weighting vector
                                          lpMemOdoRht,      // ...              odometer vector

                                          fQuadCT,          // []CT
                                          lpbCtrlBreak,     // Ptr to Ctrl-Break flag
                                          lptkFunc))        // Ptr to function token
            goto ERROR_EXIT;
    } else
    {
        // Handle all other combinations
        // APLHETERO/APLNESTED/APLRAT/APLVFP vs. anything
        // anything            vs. APLHETERO/APLNESTED/APLRAT/APLVFP
        if (!PrimFnDydEpsilonUnderbarOvO (lpMemRes,         // Ptr to result global memory data
                                          lpMemDimDiff,     // ...    dimension difference vector
                                          lpMemWVecTst,     // ...    test weighting vector
                                          lpMemOdoTst,      // ...    ...  odometer vector
                                          lpMemKmpNext,     // ...    KMP temp vector

                                          aplTypeLft,       // Left arg storage type
                                          aplNELMLft,       // ...      NELM
                                          aplColsLft,       // ...      # cols
                                          aplRankLft,       // ...      rank
                                          lpMemLft,         // Ptr to left arg global memory data
                                          lpMemDimTmp,      // ...             dimension vector

                                          aplTypeRht,       // Right arg storage type
                                          aplNELMRht,       // ...       NELM
                                          aplColsRht,       // ...       # cols
                                          aplRankRht,       // ...      rank
                                          lpMemRht,         // Ptr to right arg global memory data
                                          lpMemDimRht,      // ...              dimension vector
                                          lpMemWVecRht,     // ...              weighting vector
                                          lpMemOdoRht,      // ...              odometer vector

                                          fQuadCT,          // []CT
                                          lpbCtrlBreak,     // Ptr to Ctrl-Break flag
                                          lptkFunc))        // Ptr to function token
            goto ERROR_EXIT;
    } // End IF/ELSE/...

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // If the right arg is not immediate, ...
    if (lpMemRht)
    {
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } // End IF

    goto NORMAL_EXIT;

SOME1s:
    // Some of the elements in the result are 1s
    bSome1s = TRUE;
NOMATCH:
    // If the right arg is a global, ...
    if (lpMemRht)
    {
        // Unlock and free (and set to NULL) a global name and ptr
        UnlFreeGlbName (hGlbRes, lpMemRes);

        // If there was a YYAlloc, ...
        if (lpYYRes)
        {
            // YYFree it
            YYFree (lpYYRes); lpYYRes = NULL;
        } // End IF

        // If the result has some 1s, ...
        if (bSome1s)
        {
            HGLOBAL hGlbMFO;                // Magic function/operator global memory handle

            // Get the magic function/operator global memory handle
            hGlbMFO = GetMemPTD ()->hGlbMFO[MFOE_DydEpsUnderbar];

            //  Use an internal magic function/operator.
            return
              ExecuteMagicFunction_EM_YY (lptkLftArg,   // Ptr to left arg token
                                          lptkFunc,     // Ptr to function token
                                          NULL,         // Ptr to function strand
                                          lptkRhtArg,   // Ptr to right arg token
                                          NULL,         // Ptr to axis token
                                          hGlbMFO,      // Magic function/operator global memory handle
                                          NULL,         // Ptr to HSHTAB struc (may be NULL)
                                          LINENUM_ONE); // Starting line # type (see LINE_NUMS)
        } else
        {
            // Fill in result storage type
            aplTypeRes = ARRAY_APA;

            //***************************************************************
            // Calculate space needed for the result
            //***************************************************************
            ByteRes = CalcArraySize (aplTypeRes, aplNELMRht, aplRankRht);

            //***************************************************************
            // Check for overflow
            //***************************************************************
            if (ByteRes NE (APLU3264) ByteRes)
                goto WSFULL_EXIT;

            //***************************************************************
            // Now we can allocate the storage for the result
            //***************************************************************
            hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
            if (hGlbRes EQ NULL)
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpMemRes = MyGlobalLock000 (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
            // Fill in the header
            lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
            lpHeader->ArrType    = aplTypeRes;
////////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
            lpHeader->RefCnt     = 1;
            lpHeader->NELM       = aplNELMRht;
            lpHeader->Rank       = aplRankRht;
#undef  lpHeader

            // Skip over the header to the dimensions
            lpMemRes = (LPAPLBOOL) VarArrayBaseToDim (lpMemRes);

            // Copy the right arg dimensions to the result
            CopyMemory (lpMemRes, lpMemDimRht, (APLU3264) aplRankRht * sizeof (APLDIM));

            // Skip over the dimensions to the data
            lpMemRes = VarArrayDimToData (lpMemRes, aplRankRht);

            // Fill in the APA parms
////////////((LPAPLAPA) lpMemRes)->Off = 0;     // Already zero from GHND
////////////((LPAPLAPA) lpMemRes)->Mul = 0;     // ...

            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

            // Allocate a new YYRes
            lpYYRes = YYAlloc ();

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
            lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
        } // End IF/ELSE
    } else
        // Fill in the immediate result
        *((LPAPLBOOL) lpMemRes) = bSome1s;

    goto NORMAL_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkAxis);
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
    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbKmpNext, lpMemKmpNext);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbDimTmp, lpMemDimTmp);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbOdoTst, lpMemOdoTst);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbWVecTst, lpMemWVecTst);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbWVecRht, lpMemWVecRht);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbOdoRht, lpMemOdoRht);

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
} // End PrimFnDydEpsilonUnderbar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  Magic function/operator for dyadic helper function for the find function
//    on empty arguments
//***************************************************************************

#include "mf_epsund.h"


//***************************************************************************
//  Define macro for all routines
//***************************************************************************

#define GetNextBool(lpMem,typeMem,uRes)     (BIT0 & (((LPAPLBOOL) lpMem)[(uRes) >> LOG2NBIB] >> (MASKLOG2NBIB & (uRes))))
#define GetNextDir(lpMem,typeMem,uRes)       lpMem[uRes]
#define GetNextRAT(lpMem,typeMem,uRes)      ((LPAPLRAT) lpMem)[uRes]
#define GetNextVFP(lpMem,typeMem,uRes)      ((LPAPLVFP) lpMem)[uRes]
#define CompareInt(Lft,typeLft,Rht,typeRht) (Lft EQ Rht)
#define CompareFlt(Lft,typeLft,Rht,typeRht) (CompareCT   ( Lft,  Rht, fQuadCT, NULL))
#ifdef RAT_EXACT
#define CompareRAT(Lft,typeLft,Rht,typeRht) (mpq_cmp     (&Lft, &Rht         ) EQ 0)
#else
#define CompareRAT(Lft,typeLft,Rht,typeRht) (mpq_cmp_ct  ( Lft,  Rht, fQuadCT) EQ 0)
#endif
#define CompareVFP(Lft,typeLft,Rht,typeRht) (mpfr_cmp_ct ( Lft,  Rht, fQuadCT) EQ 0)
#define Compare
//  ***FIXME*** -- What to do about fuzzy comparisons not being transitive???

#define FINDMAC(preKmp,aplTypeKmp,GetNextValLft,aplTypeLft,GetNextValRht,aplTypeRht,CompareVal,LblSuf)  \
    APLINT    i,                        /* Loop counter             */                                  \
              j,                        /* ...                      */                                  \
              k,                        /* ...                      */                                  \
              aplColsRmL;               /* aplColsRht - aplColsLft  */                                  \
    APLUINT   uLft,                     /* Loop counter             */                                  \
              uRht,                     /* ...                      */                                  \
              uTst,                     /* ...                      */                                  \
              uOdo;                     /* ...                      */                                  \
                                                                                                        \
    /* Initializing */                                                                                  \
    uRht = 0;                                                                                           \
    aplColsRmL = aplColsRht - aplColsLft;                                                               \
                                                                                                        \
    /* Preprocessing the first row in the left arg */                                                   \
    preKmp (lpMemLft, aplTypeKmp, aplColsLft, lpMemKmpNext);                                            \
                                                                                                        \
    while (TRUE)                                                                                        \
    {                                                                                                   \
        /* Initializing */                                                                              \
        i = j = -1;                                                                                     \
        k = j - i;                                                                                      \
                                                                                                        \
        /* Searching through the cols */                                                                \
        while (k <= aplColsRmL)                                                                         \
        {                                                                                               \
            while (i > -1 && !CompareVal (GetNextValLft (lpMemLft, aplTypeLft, i),        aplTypeLft,   \
                                          GetNextValRht (lpMemRht, aplTypeRht, uRht + j), aplTypeRht))  \
                i = lpMemKmpNext[i];                                                                    \
            i++;                                                                                        \
            j++;                                                                                        \
                                                                                                        \
            /* Calculate the offset within the right arg of the putative match */                       \
            k = j - i;                                                                                  \
                                                                                                        \
            if (i >= (APLINT) aplColsLft)                                                               \
            {                                                                                           \
                /* If there are any uncompared elements in the left arg, ... */                         \
                if (aplColsLft < aplNELMLft                                                             \
                 || (IsMultiRank (aplRankLft) && aplColsLft EQ 1))                                      \
                {                                                                                       \
                    /* Use the same odometer as the current match */                                    \
                    CopyMemory (lpMemOdoTst, lpMemOdoRht, (APLU3264) (aplRankRht * sizeof (APLUINT)));  \
                                                                                                        \
                    /* Skip over the first row in the right arg (already checked) */                    \
                    lpMemOdoTst[aplRankRht - 2]++;                                                      \
                                                                                                        \
                    /* Check the remaining elements between the two args                */              \
                    /*   skipping over the first row in the left arg (already checked)  */              \
                    for (uLft = aplColsLft; uLft < aplNELMLft; uLft++)                                  \
                    {                                                                                   \
                        /* Use the index in lpMemOdoTst to calculate the            */                  \
                        /*   corresponding index in lpMemRht.                       */                  \
                        for (uTst = uOdo = 0; uOdo < aplRankRht; uOdo++)                                \
                            uTst += lpMemOdoTst[uOdo] * lpMemWVecRht[uOdo];                             \
                                                                                                        \
                        /* Increment the odometer in lpTmpOdoRht subject to         */                  \
                        /*   the values in lpMemDimLft                              */                  \
                        IncrOdometer (lpMemOdoTst, lpMemDimLft, NULL, aplRankRht);                      \
                                                                                                        \
                        if (!CompareVal (GetNextValLft (lpMemLft, aplTypeLft, uLft),     aplTypeLft,    \
                                         GetNextValRht (lpMemRht, aplTypeRht, uTst + k), aplTypeRht))   \
                            goto NOMATCH##LblSuf;                                                       \
                    } /* End FOR */                                                                     \
                } /* End IF */                                                                          \
                                                                                                        \
                /* Include the leading offset */                                                        \
                k += uRht;                                                                              \
                                                                                                        \
                /* Mark as a match */                                                                   \
                lpMemRes[k >> LOG2NBIB] |= 1 << (MASKLOG2NBIB & k);                                     \
NOMATCH##LblSuf:                                                                                        \
                /* Skip over the match */                                                               \
                i = lpMemKmpNext[i];                                                                    \
                                                                                                        \
                /* Recalculate */                                                                       \
                k = j - i;                                                                              \
            } /* End IF */                                                                              \
        } /* End WHILE */                                                                               \
                                                                                                        \
        while (TRUE)                                                                                    \
        {                                                                                               \
            /* Increment the odometer in lpMemOdoRht subject to         */                              \
            /*   the values in lpMemDimRht, skipping the cols           */                              \
            /*   (thus using aplRankRht - 1 instead of aplRankRht).     */                              \
            if (IncrOdometer (lpMemOdoRht, lpMemDimRht, NULL, aplRankRht - 1))                          \
                goto ALLDONE##LblSuf;                                                                   \
                                                                                                        \
            for (uRht = 0; uRht < (aplRankRht - 1); uRht++)                                             \
            if (lpMemDimDiff[uRht] < lpMemOdoRht[uRht])                                                 \
                break;          /* Out of the FOR loop */                                               \
            if (uRht EQ (aplRankRht - 1))                                                               \
                break;          /* Out of the WHILE loop */                                             \
        } /* End WHILE */                                                                               \
                                                                                                        \
        /* Use the index in lpMemOdoRht to calculate the */                                             \
        /*   corresponding index in lpMemRht.            */                                             \
        for (uRht = uOdo = 0; uOdo < (aplRankRht - 1); uOdo++)                                          \
            uRht += lpMemOdoRht[uOdo] * lpMemWVecRht[uOdo];                                             \
    } /* End WHILE */                                                                                   \
ALLDONE##LblSuf:                                                                                        \
    return TRUE;


//***************************************************************************
//  $GetNextSN
//
//  Get the next element from a simple hetero/nested array
//***************************************************************************

APLLONGEST GetNextSN
    (LPVOID   lpMem,
     APLSTYPE typeMem,
     APLINT   uRes)

{
    // Split cases based upon the array storage type
    switch (typeMem)
    {
        case ARRAY_BOOL:
            return (APLLONGEST) GetNextBool (lpMem, typeMem, uRes);

        case ARRAY_INT:
            return (APLLONGEST) ((LPAPLINT) lpMem)[uRes];

        case ARRAY_FLOAT:
            return (APLLONGEST) ((LPAPLLONGEST) lpMem)[uRes];

        case ARRAY_CHAR:
            return (APLLONGEST) ((LPAPLCHAR) lpMem)[uRes];

        case ARRAY_APA:
            return (APLLONGEST) (((LPAPLAPA) lpMem)->Off + ((LPAPLAPA) lpMem)->Mul * uRes);

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            return (UINT_PTR) ((LPAPLHETERO) lpMem)[uRes];

        defstop
            return 0;
    } // End SWITCH
} // End GetNextSN


//***************************************************************************
//  $PrimFnDydEpsilonUnderbarBvB
//
//  Dyadic EpsilonUnderbar of APLBOOL vs. APLBOOL
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydEpsilonUnderbarBvB"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydEpsilonUnderbarBvB
    (LPAPLBOOL lpMemRes,                // Ptr to result global memory data
     LPAPLUINT lpMemDimDiff,            // ...    dimension difference vector
     LPAPLUINT lpMemWVecTst,            // ...    test weighting vector
     LPAPLUINT lpMemOdoTst,             // ...    ...  odometer vector
     LPAPLINT  lpMemKmpNext,            // ...    KMP intermediate vector

     APLNELM   aplNELMLft,              // Left arg NELM
     APLDIM    aplColsLft,              // ...      # cols
     APLRANK   aplRankLft,              // ...      rank
     LPAPLBOOL lpMemLft,                // Ptr to left arg global memory data
     LPAPLDIM  lpMemDimLft,             // ...             dimension vector

     APLNELM   aplNELMRht,              // Right arg NELM
     APLDIM    aplColsRht,              // ...       # cols
     APLRANK   aplRankRht,              // ...       rank
     LPAPLBOOL lpMemRht,                // Ptr to right arg global memory data
     LPAPLDIM  lpMemDimRht,             // ...              dimension vector
     LPAPLUINT lpMemWVecRht,            // ...              weighting vector
     LPAPLUINT lpMemOdoRht,             // ...              odometer vector

     APLFLOAT  fQuadCT,                 // []CT
     LPUBOOL   lpbCtrlBreak,            // Ptr to Ctrl-Break flag
     LPTOKEN   lptkFunc)                // Ptr to function token

{
    FINDMAC (preKmpB, ARRAY_BOOL, GetNextBool, ARRAY_BOOL, GetNextBool, ARRAY_BOOL, CompareInt, BvB)
} // End PrimFnDydEpsilonUnderbarBvB
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydEpsilonUnderbarBvF
//
//  Dyadic EpsilonUnderbar of APLBOOL vs. APLFLOAT
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydEpsilonUnderbarBvF"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydEpsilonUnderbarBvF
    (LPAPLBOOL lpMemRes,                // Ptr to result global memory data
     LPAPLUINT lpMemDimDiff,            // ...    dimension difference vector
     LPAPLUINT lpMemWVecTst,            // ...    test weighting vector
     LPAPLUINT lpMemOdoTst,             // ...    ...  odometer vector
     LPAPLINT  lpMemKmpNext,            // ...    KMP intermediate vector

     APLNELM   aplNELMLft,              // Left arg NELM
     APLDIM    aplColsLft,              // ...      # cols
     APLRANK   aplRankLft,              // ...      rank
     LPAPLBOOL lpMemLft,                // Ptr to left arg global memory data
     LPAPLDIM  lpMemDimLft,             // ...             dimension vector

     APLNELM   aplNELMRht,              // ...       NELM
     APLDIM    aplColsRht,              // ...       # cols
     APLRANK   aplRankRht,              // ...       rank
     LPAPLBOOL lpMemRht,                // Ptr to right arg global memory data
     LPAPLDIM  lpMemDimRht,             // ...              dimension vector
     LPAPLUINT lpMemWVecRht,            // ...              weighting vector
     LPAPLUINT lpMemOdoRht,             // ...              odometer vector

     APLFLOAT  fQuadCT,                 // []CT
     LPUBOOL   lpbCtrlBreak,            // Ptr to Ctrl-Break flag
     LPTOKEN   lptkFunc)                // Ptr to function token

{
    FINDMAC (preKmpB, ARRAY_BOOL, GetNextBool, ARRAY_BOOL, GetNextFloat, ARRAY_FLOAT, CompareFlt, BvF)
} // End PrimFnDydEpsilonUnderbarBvF
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydEpsilonUnderbarAvF
//
//  Dyadic EpsilonUnderbar of APLAPA vs. APLFLOAT
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydEpsilonUnderbarAvF"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydEpsilonUnderbarAvF
    (LPAPLBOOL lpMemRes,                // Ptr to result global memory data
     LPAPLUINT lpMemDimDiff,            // ...    dimension difference vector
     LPAPLUINT lpMemWVecTst,            // ...    test weighting vector
     LPAPLUINT lpMemOdoTst,             // ...    ...  odometer vector
     LPAPLINT  lpMemKmpNext,            // ...    KMP intermediate vector

     APLNELM   aplNELMLft,              // Left arg NELM
     APLDIM    aplColsLft,              // ...      # cols
     APLRANK   aplRankLft,              // ...      rank
     LPVOID    lpMemLft,                // Ptr to left arg global memory data
     LPAPLDIM  lpMemDimLft,             // ...             dimension vector

     APLNELM   aplNELMRht,              // Right arg NELM
     APLDIM    aplColsRht,              // ...       # cols
     APLRANK   aplRankRht,              // ...       rank
     LPAPLAPA  lpMemRht,                // Ptr to right arg global memory data
     LPAPLDIM  lpMemDimRht,             // ...              dimension vector
     LPAPLUINT lpMemWVecRht,            // ...              weighting vector
     LPAPLUINT lpMemOdoRht,             // ...              odometer vector

     APLFLOAT  fQuadCT,                 // []CT
     LPUBOOL   lpbCtrlBreak,            // Ptr to Ctrl-Break flag
     LPTOKEN   lptkFunc)                // Ptr to function token

{
    FINDMAC (preKmpA, ARRAY_APA, GetNextFloat, ARRAY_APA, GetNextFloat, ARRAY_FLOAT, CompareFlt, AvF)
} // End PrimFnDydEpsilonUnderbarAvF
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydEpsilonUnderbarIvI
//
//  Dyadic EpsilonUnderbar of APLINT/APLAPA vs. APLBOOL/APLINT/APLAPA
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydEpsilonUnderbarIvI"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydEpsilonUnderbarIvI
    (LPAPLBOOL lpMemRes,                // Ptr to result global memory data
     LPAPLUINT lpMemDimDiff,            // ...    dimension difference vector
     LPAPLUINT lpMemWVecTst,            // ...    test weighting vector
     LPAPLUINT lpMemOdoTst,             // ...    ...  odometer vector
     LPAPLINT  lpMemKmpNext,            // ...    KMP intermediate vector

     APLSTYPE  aplTypeLft,              // Left arg storage type
     APLNELM   aplNELMLft,              // ...      NELM
     APLDIM    aplColsLft,              // ...      # cols
     APLRANK   aplRankLft,              // ...      rank
     LPVOID    lpMemLft,                // Ptr to left arg global memory data
     LPAPLDIM  lpMemDimLft,             // ...             dimension vector

     APLSTYPE  aplTypeRht,              // Right arg storage type
     APLNELM   aplNELMRht,              // Right arg NELM
     APLDIM    aplColsRht,              // ...       # cols
     APLRANK   aplRankRht,              // ...       rank
     LPVOID    lpMemRht,                // Ptr to right arg global memory data
     LPAPLDIM  lpMemDimRht,             // ...              dimension vector
     LPAPLUINT lpMemWVecRht,            // ...              weighting vector
     LPAPLUINT lpMemOdoRht,             // ...              odometer vector

     APLFLOAT  fQuadCT,                 // []CT
     LPUBOOL   lpbCtrlBreak,            // Ptr to Ctrl-Break flag
     LPTOKEN   lptkFunc)                // Ptr to function token

{
    FINDMAC (preKmpO, aplTypeLft, GetNextInteger, aplTypeLft, GetNextInteger, aplTypeRht, CompareInt, IvI)
} // End PrimFnDydEpsilonUnderbarIvI
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydEpsilonUnderbarNvN
//
//  Dyadic EpsilonUnderbar of APLBOOL/APLINT/APLAPA/APLFLOAT vs. APLFLOAT and
//                            APLFLOAT vs. APLINT
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydEpsilonUnderbarNvN"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydEpsilonUnderbarNvN
    (LPAPLBOOL lpMemRes,                // Ptr to result global memory data
     LPAPLUINT lpMemDimDiff,            // ...    dimension difference vector
     LPAPLUINT lpMemWVecTst,            // ...    test weighting vector
     LPAPLUINT lpMemOdoTst,             // ...    ...  odometer vector
     LPAPLINT  lpMemKmpNext,            // ...    KMP intermediate vector

     APLSTYPE  aplTypeLft,              // Left arg storage type
     APLNELM   aplNELMLft,              // ...      NELM
     APLDIM    aplColsLft,              // ...      # cols
     APLRANK   aplRankLft,              // ...      rank
     LPVOID    lpMemLft,                // Ptr to left arg global memory data
     LPAPLDIM  lpMemDimLft,             // ...             dimension vector

     APLSTYPE  aplTypeRht,              // Ptr to right arg storage type
     APLNELM   aplNELMRht,              // Right arg NELM
     APLDIM    aplColsRht,              // ...       # cols
     APLRANK   aplRankRht,              // ...       rank
     LPVOID    lpMemRht,                // Ptr to right arg global memory data
     LPAPLDIM  lpMemDimRht,             // ...              dimension vector
     LPAPLUINT lpMemWVecRht,            // ...              weighting vector
     LPAPLUINT lpMemOdoRht,             // ...              odometer vector

     APLFLOAT  fQuadCT,                 // []CT
     LPUBOOL   lpbCtrlBreak,            // Ptr to Ctrl-Break flag
     LPTOKEN   lptkFunc)                // Ptr to function token

{
    FINDMAC (preKmpO, aplTypeLft, GetNextFloat, aplTypeLft, GetNextFloat, aplTypeRht, CompareFlt, NvN)
} // End PrimFnDydEpsilonUnderbarNvN
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydEpsilonUnderbarCvC
//
//  Dyadic EpsilonUnderbar of APLCHAR vs. APLCHAR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydEpsilonUnderbarCvC"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydEpsilonUnderbarCvC
    (LPAPLBOOL lpMemRes,                // Ptr to result global memory data
     LPAPLUINT lpMemDimDiff,            // ...    dimension difference vector
     LPAPLUINT lpMemWVecTst,            // ...    test weighting vector
     LPAPLUINT lpMemOdoTst,             // ...    ...  odometer vector
     LPAPLINT  lpMemKmpNext,            // ...    KMP intermediate vector

     APLNELM   aplNELMLft,              // Left arg NELM
     APLDIM    aplColsLft,              // ...      # cols
     APLRANK   aplRankLft,              // ...      rank
     LPAPLCHAR lpMemLft,                // Ptr to left arg global memory data
     LPAPLDIM  lpMemDimLft,             // ...             dimension vector

     APLNELM   aplNELMRht,              // Right arg NELM
     APLDIM    aplColsRht,              // ...       # cols
     APLRANK   aplRankRht,              // ...       rank
     LPAPLCHAR lpMemRht,                // Ptr to right arg global memory data
     LPAPLDIM  lpMemDimRht,             // ...              dimension vector
     LPAPLUINT lpMemWVecRht,            // ...              weighting vector
     LPAPLUINT lpMemOdoRht,             // ...              odometer vector

     APLFLOAT  fQuadCT,                 // []CT
     LPUBOOL   lpbCtrlBreak,            // Ptr to Ctrl-Break flag
     LPTOKEN   lptkFunc)                // Ptr to function token

{
    FINDMAC (preKmpC, ARRAY_CHAR, GetNextDir, ARRAY_CHAR, GetNextDir, ARRAY_CHAR, CompareInt, CvC)
} // End PrimFnDydEpsilonUnderbarCvC
#undef  APPEND_NAME


//***************************************************************************
//  $preKmpA
//
//  Preprocessing phase for Knuth-Morris-Pratt
//    for APLAPAs
//***************************************************************************

void preKmpA
    (LPAPLAPA    x,
     APLSTYPE    aplTypeKmp,
     APLINT      m,
     APLINT      kmpNext[])
{
    APLINT i, j;

    i = 0;
    j = kmpNext[0] = -1;

    while (i < m)
    {
        while (j > -1 && GetNextInteger (x, ARRAY_APA, i) NE GetNextInteger (x, ARRAY_APA, j))
            j = kmpNext[j];
        i++;
        j++;

        if ((i < m) && (GetNextInteger (x, ARRAY_APA, i) EQ GetNextInteger (x, ARRAY_APA, j)))
            kmpNext[i] = kmpNext[j];
        else
            kmpNext[i] = j;
   } // End WHILE
} // End preKmpA


//***************************************************************************
//  $preKmpB
//
//  Preprocessing phase for Knuth-Morris-Pratt
//    for APLBOOLs
//***************************************************************************

void preKmpB
    (LPAPLBOOL   x,
     APLSTYPE    aplTypeKmp,
     APLINT      m,
     APLINT      kmpNext[])
{
    APLINT i, j;

    i = 0;
    j = kmpNext[0] = -1;

    while (i < m)
    {
        while (j > -1 && GetNextInteger (x, ARRAY_BOOL, i) NE GetNextInteger (x, ARRAY_BOOL, j))
            j = kmpNext[j];
        i++;
        j++;

        if ((i < m) && (GetNextInteger (x, ARRAY_BOOL, i) EQ GetNextInteger (x, ARRAY_BOOL, j)))
            kmpNext[i] = kmpNext[j];
        else
            kmpNext[i] = j;
   } // End WHILE
} // End preKmpB


//***************************************************************************
//  $preKmpC
//
//  Preprocessing phase for Knuth-Morris-Pratt
//    for APLCHARs
//***************************************************************************

void preKmpC
    (LPAPLCHAR   x,
     APLSTYPE    aplTypeKmp,
     APLINT      m,
     APLINT      kmpNext[])
{
    APLINT i, j;

    i = 0;
    j = kmpNext[0] = -1;

    while (i < m)
    {
        while (j > -1 && x[i] NE x[j])
            j = kmpNext[j];
        i++;
        j++;

        if ((i < m) && (x[i] EQ x[j]))
            kmpNext[i] = kmpNext[j];
        else
            kmpNext[i] = j;
   } // End WHILE
} // End preKmpC


//***************************************************************************
//  $preKmpF
//
//  Preprocessing phase for Knuth-Morris-Pratt
//    for APLFLOATs
//
//  ***FXME*** -- What to do about fuzzy comparisons not being transitive???
//***************************************************************************

void preKmpF
    (LPAPLFLOAT  x,
     APLSTYPE    aplTypeKmp,
     APLINT      m,
     APLINT      kmpNext[])
{
    APLINT i, j;

    i = 0;
    j = kmpNext[0] = -1;

    while (i < m)
    {
        while (j > -1 && x[i] NE x[j])
            j = kmpNext[j];
        i++;
        j++;

        if ((i < m) && (x[i] EQ x[j]))
            kmpNext[i] = kmpNext[j];
        else
            kmpNext[i] = j;
   } // End WHILE
} // End preKmpF


//***************************************************************************
//  $preKmpI
//
//  Preprocessing phase for Knuth-Morris-Pratt
//    for APLINTs
//***************************************************************************

void preKmpI
    (LPAPLINT    x,
     APLSTYPE    aplTypeKmp,
     APLINT      m,
     APLINT      kmpNext[])
{
    APLINT i, j;

    i = 0;
    j = kmpNext[0] = -1;

    while (i < m)
    {
        while (j > -1 && x[i] NE x[j])
            j = kmpNext[j];
        i++;
        j++;

        if ((i < m) && (x[i] EQ x[j]))
            kmpNext[i] = kmpNext[j];
        else
            kmpNext[i] = j;
   } // End WHILE
} // End preKmpI


//***************************************************************************
//  $preKmpR
//
//  Preprocessing phase for Knuth-Morris-Pratt
//    for APLRATs
//***************************************************************************

void preKmpR
    (LPAPLRAT    x,
     APLSTYPE    aplTypeKmp,
     APLINT      m,
     APLINT      kmpNext[])
{
    APLINT i, j;

    i = 0;
    j = kmpNext[0] = -1;

    while (i < m)
    {
        while (j > -1 && mpq_cmp (&x[i], &x[j]) NE 0)
            j = kmpNext[j];
        i++;
        j++;

        if ((i < m) && mpq_cmp (&x[i], &x[j]) EQ 0)
            kmpNext[i] = kmpNext[j];
        else
            kmpNext[i] = j;
   } // End WHILE
} // End preKmpR


//***************************************************************************
//  $preKmpV
//
//  Preprocessing phase for Knuth-Morris-Pratt
//    for APLVFPs
//***************************************************************************

void preKmpV
    (LPAPLVFP    x,
     APLSTYPE    aplTypeKmp,
     APLINT      m,
     APLINT      kmpNext[])
{
    APLINT i, j;

    i = 0;
    j = kmpNext[0] = -1;

    while (i < m)
    {
        while (j > -1 && mpfr_cmp (&x[i], &x[j]) NE 0)
            j = kmpNext[j];
        i++;
        j++;

        if ((i < m) && mpfr_cmp (&x[i], &x[j]) EQ 0)
            kmpNext[i] = kmpNext[j];
        else
            kmpNext[i] = j;
   } // End WHILE
} // End preKmpV


//***************************************************************************
//  $preKmpHN
//
//  Preprocessing phase for Knuth-Morris-Pratt
//    for APLHETERO/APLNESTEDs
//***************************************************************************

void preKmpHN
    (LPAPLNESTED x,
     APLSTYPE    aplTypeKmp,
     APLINT      m,
     APLINT      kmpNext[])
{
    APLINT i, j;

    i = 0;
    j = kmpNext[0] = -1;

    while (i < m)
    {
        while (j > -1 && !CompareNested (x[i], aplTypeKmp, x[j], aplTypeKmp))
            j = kmpNext[j];
        i++;
        j++;

        if ((i < m) && CompareNested (x[i], aplTypeKmp, x[j], aplTypeKmp))
            kmpNext[i] = kmpNext[j];
        else
            kmpNext[i] = j;
   } // End WHILE
} // End preKmpHN


//***************************************************************************
//  $CompareNested
//
//  Compare two nested items
//***************************************************************************

UBOOL CompareNested
    (APLHETERO x,               // Left arg, possibly simple as per typeX
     APLSTYPE  typeX,           // Left arg storage type
     APLHETERO y,               // Right arg, possibly simple as per typeY
     APLSTYPE  typeY)           // Right arg storage type

{
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // If the left arg is simple non-hetero and the right arg is hetero/nested, ...
    if (IsSimpleNH (typeX) && !IsSimpleNH (typeY))
    {
        // If the hetero/nested arg is not a STE, ...
        if (GetPtrTypeDir (y) NE PTRTYPE_STCONST)
            return FALSE;
        return
          PrimFnDydEqualUnderbarSimpleUnord (&x,                   typeX,                                            1, 0,
                                             &y->stData.stLongest, TranslateImmTypeToArrayType (y->stFlags.ImmType), 1, 0,
                                              FALSE, lpbCtrlBreak);
    } // End IF

    // If the right arg is simple non-hetero and the left arg is hetero/nested, ...
    if (IsSimpleNH (typeY) && !IsSimpleNH (typeX))
    {
        // If the hetero/nested arg is not a STE, ...
        if (GetPtrTypeDir (x) NE PTRTYPE_STCONST)
            return FALSE;
        return
          PrimFnDydEqualUnderbarSimpleUnord (&y,                   typeY,                                            1, 0,
                                             &x->stData.stLongest, TranslateImmTypeToArrayType (x->stFlags.ImmType), 1, 0,
                                              FALSE, lpbCtrlBreak);
    } // End IF

    return
      PrimFnDydEqualUnderbarNested (&x, ARRAY_NESTED, 1, 0,
                                    &y, ARRAY_NESTED, 1, 0,
                                     FALSE, lpbCtrlBreak);
} // End CompareNested


//***************************************************************************
//  $CompareSNvSN
//
//  Compare two nested items
//***************************************************************************

UBOOL CompareSNvSN
    (APLLONGEST x,               // Left arg, possibly simple as per typeX
     APLSTYPE   typeX,           // Left arg storage type
     APLLONGEST y,               // Right arg, possibly simple as per typeY
     APLSTYPE   typeY)           // Right arg storage type

{
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // If the left arg is simple non-hetero and the right arg is hetero/nested, ...
    if (IsSimpleNH (typeX) && !IsSimpleNH (typeY))
    {
        // If the hetero/nested arg is not a STE, ...
        if (GetPtrTypeDir ((APLHETERO) y) NE PTRTYPE_STCONST)
            return FALSE;
        return
          PrimFnDydEqualUnderbarSimpleUnord (&x,                                 typeX,                                                          1, 0,
                                             &((APLHETERO) y)->stData.stLongest, TranslateImmTypeToArrayType (((APLHETERO) y)->stFlags.ImmType), 1, 0,
                                              FALSE, lpbCtrlBreak);
    } // End IF

    // If the right arg is simple non-hetero and the left arg is hetero/nested, ...
    if (IsSimpleNH (typeY) && !IsSimpleNH (typeX))
    {
        // If the hetero/nested arg is not a STE, ...
        if (GetPtrTypeDir ((APLHETERO) x) NE PTRTYPE_STCONST)
            return FALSE;
        return
          PrimFnDydEqualUnderbarSimpleUnord (&y,                                 typeY,                                                          1, 0,
                                             &((APLHETERO) x)->stData.stLongest, TranslateImmTypeToArrayType (((APLHETERO) x)->stFlags.ImmType), 1, 0,
                                              FALSE, lpbCtrlBreak);
    } // End IF

    return
      PrimFnDydEqualUnderbarNested (&x, ARRAY_NESTED, 1, 0,
                                    &y, ARRAY_NESTED, 1, 0,
                                     FALSE, lpbCtrlBreak);
} // End CompareSNvSN


//***************************************************************************
//  $preKmpO
//
//  Preprocessing phase for Knuth-Morris-Pratt
//    for others
//***************************************************************************

void preKmpO
    (LPVOID     x,
     APLSTYPE   aplTypeKmp,
     APLINT     m,
     APLINT     kmpNext[])
{
    // Split cases based upon the KMP array storage type
    switch (aplTypeKmp)
    {
        case ARRAY_BOOL:
            preKmpB (x, aplTypeKmp, m, kmpNext);

            break;

        case ARRAY_INT:
            preKmpI (x, aplTypeKmp, m, kmpNext);

            break;

        case ARRAY_FLOAT:
            preKmpF (x, aplTypeKmp, m, kmpNext);

            break;

        case ARRAY_APA:
            preKmpA (x, aplTypeKmp, m, kmpNext);

            break;

        case ARRAY_CHAR:
            preKmpC (x, aplTypeKmp, m, kmpNext);

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            preKmpHN (x, aplTypeKmp, m, kmpNext);

            break;

        case ARRAY_RAT:
            preKmpR (x, aplTypeKmp, m, kmpNext);

            break;

        case ARRAY_VFP:
            preKmpV (x, aplTypeKmp, m, kmpNext);

            break;

        defstop
            break;
    } // End SWITCH
} // End preKmpO


//***************************************************************************
//  Boyer-Moore
//***************************************************************************

#if 0
void preBmBc(char *x, int m, int bmBc[]) {
   int i;

   for (i = 0; i < ASIZE; ++i)
      bmBc[i] = m;
   for (i = 0; i < m - 1; ++i)
      bmBc[x[i]] = m - i - 1;
}


void suffixes(char *x, int m, int *suff) {
   int f, g, i;

   suff[m - 1] = m;
   g = m - 1;
   for (i = m - 2; i >= 0; --i) {
      if (i > g && suff[i + m - 1 - f] < i - g)
         suff[i] = suff[i + m - 1 - f];
      else {
         if (i < g)
            g = i;
         f = i;
         while (g >= 0 && x[g] == x[g + m - 1 - f])
            --g;
         suff[i] = f - g;
      }
   }
}


void preBmGs(char *x, int m, int bmGs[]) {
   int i, j, suff[XSIZE];

   suffixes(x, m, suff);

   for (i = 0; i < m; ++i)
      bmGs[i] = m;
   j = 0;
   for (i = m - 1; i >= 0; --i)
      if (suff[i] == i + 1)
         for (; j < m - 1 - i; ++j)
            if (bmGs[j] == m)
               bmGs[j] = m - 1 - i;
   for (i = 0; i <= m - 2; ++i)
      bmGs[m - 1 - suff[i]] = m - 1 - i;
}


void BM(char *x, int m, char *y, int n) {
   int i, j, bmGs[XSIZE], bmBc[ASIZE];

   /* Preprocessing */
   preBmGs(x, m, bmGs);
   preBmBc(x, m, bmBc);

   /* Searching */
   j = 0;
   while (j <= n - m) {
      for (i = m - 1; i >= 0 && x[i] == y[i + j]; --i);
      if (i < 0) {
         OUTPUT(j);
         j += bmGs[0];
      }
      else
         j += MAX(bmGs[i], bmBc[y[i + j]] - m + 1 + i);
   }
}
#endif


//***************************************************************************
//  Knuth-Morris-Pratt
//***************************************************************************

#if 0
void preKmp
    (char *x,
     int m,
     int kmpNext[])
{
    int i, j;

    i = 0;
    j = kmpNext[0] = -1;

    while (i < m)
    {
        while (j > -1 && x[i] != x[j])
            j = kmpNext[j];
        i++;
        j++;
        if (x[i] == x[j])
            kmpNext[i] = kmpNext[j];
        else
            kmpNext[i] = j;
   }
}


void KMP
    (char *x,
     int m,
     char *y,
     int n)
{
    int i,
        j,
        kmpNext[XSIZE];

    /* Preprocessing */
    preKmp(x, m, kmpNext);

    /* Searching */
    i = j = -1;
    while (j+m-i <= n)
    {
        while (i > -1 && x[i] != y[j])
            i = kmpNext[i];
        i++;
        j++;
        if (i >= m)
        {
            OUTPUT(j - i);
            i = kmpNext[i];
        }
    }
}
#endif


//***************************************************************************
//  $CompareRATvVFP
//
//  Compare a RAT and a VFP
//***************************************************************************

UBOOL CompareRATvVFP
    (APLRAT    aplLft,          // Left arg, possibly simple as per typeLft
     APLSTYPE  typeX,           // Left arg storage type
     APLVFP    aplRht,          // Right arg, possibly simple as per typeRht
     APLSTYPE  typeY)           // Right arg storage type

{
    APLVFP mpfLft = {0};        // Temporary VFP
    UBOOL  bRet;                // TRUE iff the result is valid

    // Convert the RAT to a VFP
    mpfr_init_set_q (&mpfLft, &aplLft, MPFR_RNDN);

    // Compare 'em
    bRet = (mpfr_cmp_ct (mpfLft, aplRht, GetQuadCT ()) EQ 0);

    // We no longer need this storage
    Myf_clear (&mpfLft);

    return bRet;
} // End CompareRATvVFP


//***************************************************************************
//  $CompareRATvSN
//
//  Compare a RAT and a simple/nested
//***************************************************************************

UBOOL CompareRATvSN
    (APLRAT      aplLft,        // Left arg, possibly simple as per typeLft
     APLSTYPE    typeLft,       // Left arg storage type
     APLLONGEST  aplRht,        // Right arg, possibly simple as per typeRht
     APLSTYPE    typeRht)       // Right arg storage type

{
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // If the right arg is APA, ...
    if (IsSimpleAPA (typeRht))
        // Set to INT as aplRht is the current value in the APA
        typeRht = ARRAY_INT;

    // If the right arg is simple or global numeric, ...
    // Note that the types are switched to be in ARRAY_TYPES order
    //   expected by EqualUnderbar.
    if (IsSimpleGlbNum (typeRht))
        return
          PrimFnDydEqualUnderbarSimpleUnord (&aplRht, typeRht, 1, 0,
                                             &aplLft, typeLft, 1, 0,
                                              FALSE,  lpbCtrlBreak);
    else
        return
          PrimFnDydEqualUnderbarNested (&aplRht, typeRht     , 1, 0,
                                        &aplLft, typeLft     , 1, 0,
                                         FALSE,  lpbCtrlBreak);
} // End CompareRATvSN


//***************************************************************************
//  $CompareVFPvRAT
//
//  Compare a VFP and a RAT
//***************************************************************************

UBOOL CompareVFPvRAT
    (APLVFP    aplLft,          // Left arg, possibly simple as per typeLft
     APLSTYPE  typeX,           // Left arg storage type
     APLRAT    aplRht,          // Right arg, possibly simple as per typeRht
     APLSTYPE  typeY)           // Right arg storage type

{
    APLVFP mpfRht = {0};        // Temporary VFP
    UBOOL  bRet;                // TRUE iff the result is valid

    // Convert the RAT to a VFP
    mpfr_init_set_q (&mpfRht, &aplRht, MPFR_RNDN);

    // Compare 'em
    bRet = (mpfr_cmp_ct (aplLft, mpfRht, GetQuadCT ()) EQ 0);

    // We no longer need this storage
    Myf_clear (&mpfRht);

    return bRet;
} // End CompareVFPvRAT


//***************************************************************************
//  $CompareVFPvSN
//
//  Compare a VFP and a simple/nested
//***************************************************************************

UBOOL CompareVFPvSN
    (APLVFP      aplLft,        // Left arg, possibly simple as per typeLft
     APLSTYPE    typeLft,       // Left arg storage type
     APLLONGEST  aplRht,        // Right arg, possibly simple as per typeRht
     APLSTYPE    typeRht)       // Right arg storage type

{
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // If the right arg is APA, ...
    if (IsSimpleAPA (typeRht))
        // Set to INT as aplRht is the current value in the APA
        typeRht = ARRAY_INT;

    // If the right arg is simple or global numeric, ...
    // Note that the types are switched to be in ARRAY_TYPES order
    //   expected by EqualUnderbar.
    if (IsSimpleGlbNum (typeRht))
        return
          PrimFnDydEqualUnderbarSimpleUnord (&aplRht, typeRht, 1, 0,
                                             &aplLft, typeLft, 1, 0,
                                              FALSE,  lpbCtrlBreak);
    else
        return
          PrimFnDydEqualUnderbarNested (&aplRht, typeRht     , 1, 0,
                                        &aplLft, typeLft     , 1, 0,
                                         FALSE, lpbCtrlBreak);
} // End CompareVFPvSN


//***************************************************************************
//  $CompareSNvRAT
//
//  Compare a simple/nested and a RAT
//***************************************************************************

UBOOL CompareSNvRAT
    (APLLONGEST  aplLft,        // Left arg, possibly simple as per typeLft
     APLSTYPE    typeLft,       // Left arg storage type
     APLRAT      aplRht,        // Right arg, possibly simple as per typeRht
     APLSTYPE    typeRht)       // Right arg storage type

{
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // If the left arg is APA, ...
    if (IsSimpleAPA (typeLft))
        // Set to INT as aplLft is the current value in the APA
        typeLft = ARRAY_INT;

    // If the left arg is simple or global numeric, ...
    // Note that the types are switched to be in ARRAY_TYPES order
    //   expected by EqualUnderbar.
    if (IsSimpleGlbNum (typeLft))
        return
          PrimFnDydEqualUnderbarSimpleUnord (&aplRht, typeRht, 1, 0,
                                             &aplLft, typeLft, 1, 0,
                                              FALSE,  lpbCtrlBreak);
    else
        return
          PrimFnDydEqualUnderbarNested (&aplRht, typeRht     , 1, 0,
                                        &aplLft, typeLft     , 1, 0,
                                         FALSE, lpbCtrlBreak);
} // End CompareSNvRAT


//***************************************************************************
//  $CompareSNvVFP
//
//  Compare a simple/nested and a VFP
//***************************************************************************

UBOOL CompareSNvVFP
    (APLLONGEST  aplLft,        // Left arg, possibly simple as per typeLft
     APLSTYPE    typeLft,       // Left arg storage type
     APLVFP      aplRht,        // Right arg, possibly simple as per typeRht
     APLSTYPE    typeRht)       // Right arg storage type

{
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // If the left arg is APA, ...
    if (IsSimpleAPA (typeLft))
        // Set to INT as aplLft is the current value in the APA
        typeLft = ARRAY_INT;

    // If the left arg is simple or global numeric, ...
    // Note that the types are switched to be in ARRAY_TYPES order
    //   expected by EqualUnderbar.
    if (IsSimpleGlbNum (typeLft))
        return
          PrimFnDydEqualUnderbarSimpleUnord (&aplRht, typeRht, 1, 0,
                                             &aplLft, typeLft, 1, 0,
                                              FALSE,  lpbCtrlBreak);
    else
        return
          PrimFnDydEqualUnderbarNested (&aplRht, typeRht     , 1, 0,
                                        &aplLft, typeLft     , 1, 0,
                                         FALSE, lpbCtrlBreak);
} // End CompareSNvVFP


//***************************************************************************
//  $PrimFnDydEpsilonUnderbarOvO
//
//  Dyadic EpsilonUnderbar between all other arg combinations
//          APLHETERO/APLNESTED vs. anything
//          anything            vs. APLHETERO/APLNESTED
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydEpsilonUnderbarOvO"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydEpsilonUnderbarOvO
    (LPAPLBOOL   lpMemRes,              // Ptr to result global memory data
     LPAPLUINT   lpMemDimDiff,          // ...    dimension difference vector
     LPAPLUINT   lpMemWVecTst,          // ...    test weighting vector
     LPAPLUINT   lpMemOdoTst,           // ...    ...  odometer vector
     LPAPLINT    lpMemKmpNext,          // ...    KMP intermediate vector

     APLSTYPE    aplTypeLft,            // Left arg storage type
     APLNELM     aplNELMLft,            // Left arg NELM
     APLDIM      aplColsLft,            // ...      # cols
     APLRANK     aplRankLft,            // ...      rank
     LPAPLNESTED lpMemLft,              // Ptr to left arg global memory data
     LPAPLDIM    lpMemDimLft,           // ...             dimension vector

     APLSTYPE    aplTypeRht,            // Right arg storage type
     APLNELM     aplNELMRht,            // Right arg NELM
     APLDIM      aplColsRht,            // ...       # cols
     APLRANK     aplRankRht,            // ...       rank
     LPAPLNESTED lpMemRht,              // Ptr to right arg global memory data
     LPAPLDIM    lpMemDimRht,           // ...              dimension vector
     LPAPLUINT   lpMemWVecRht,          // ...              weighting vector
     LPAPLUINT   lpMemOdoRht,           // ...              odometer vector

     APLFLOAT    fQuadCT,               // []CT
     LPUBOOL     lpbCtrlBreak,          // Ptr to Ctrl-Break flag
     LPTOKEN     lptkFunc)              // Ptr to function token

{
    UINT uLft,
         uRht;

    // Is the left arg RAT, VFP, or Other?
    uLft = (IsRat (aplTypeLft) ? 0
                               : (IsVfp (aplTypeLft) ? 1
                                                     : 2));
    // Is the right arg RAT, VFP, or Other?
    uRht = (IsRat (aplTypeRht) ? 0
                               : (IsVfp (aplTypeRht) ? 1
                                                     : 2));
    // Split cases based upon the left vs. right arg storage types
    switch (3 * uLft + 1 * uRht)
    {
        //      RAT     RAT
        case 3 * 0 + 1 * 0:
        {
            FINDMAC (preKmpO, aplTypeLft, GetNextRAT, aplTypeLft, GetNextRAT, aplTypeRht, CompareRAT    , RvR)
        } // End RAT vs. RAT

        //      RAT     VFP
        case 3 * 0 + 1 * 1:
        {
            FINDMAC (preKmpO, aplTypeLft, GetNextRAT, aplTypeLft, GetNextVFP, aplTypeRht, CompareRATvVFP, RvV)
        } // End RAT vs. VFP

        //      RAT     Oth
        case 3 * 0 + 1 * 2:
        {
            FINDMAC (preKmpO, aplTypeLft, GetNextRAT, aplTypeLft, GetNextSN , aplTypeRht, CompareRATvSN , RvO)
        } // End RAT vs. Oth

        //      VFP     RAT
        case 3 * 1 + 1 * 0:
        {
            FINDMAC (preKmpO, aplTypeLft, GetNextVFP, aplTypeLft, GetNextRAT, aplTypeRht, CompareVFPvRAT, VvR)
        } // End VFP vs. RAT

        //      VFP     VFP
        case 3 * 1 + 1 * 1:
        {
            FINDMAC (preKmpO, aplTypeLft, GetNextVFP, aplTypeLft, GetNextVFP, aplTypeRht, CompareVFP    , VvV)
        } // End VFP vs. VFP

        //      VFP     Oth
        case 3 * 1 + 1 * 2:
        {
            FINDMAC (preKmpO, aplTypeLft, GetNextVFP, aplTypeLft, GetNextSN , aplTypeRht, CompareVFPvSN , VvO)
        } // End VFP vs. Oth

        //      Oth     RAT
        case 3 * 2 + 1 * 0:
        {
            FINDMAC (preKmpO, aplTypeLft, GetNextSN , aplTypeLft, GetNextRAT, aplTypeRht, CompareSNvRAT , OvR)
        } // End Oth vs. RAT

        //      Oth     VFP
        case 3 * 2 + 1 * 1:
        {
            FINDMAC (preKmpO, aplTypeLft, GetNextSN , aplTypeLft, GetNextVFP, aplTypeRht, CompareSNvVFP , OvV)
        } // End Oth vs. VFP

        //      Oth     Oth
        case 3 * 2 + 1 * 2:
        {
            FINDMAC (preKmpO, aplTypeLft, GetNextSN , aplTypeLft, GetNextSN , aplTypeRht, CompareSNvSN  , OvO)
        } // End Oth vs. Oth

        defstop
            return FALSE;
    } // End SWITCH
} // End PrimFnDydEpsilonUnderbarOvO
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_epsund.c
//***************************************************************************
