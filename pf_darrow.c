//***************************************************************************
//  NARS2000 -- Primitive Function -- DownArrow
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
//  $PrimFnDownArrow_EM_YY
//
//  Primitive function for monadic and dyadic DownArrow (ERROR and "drop")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDownArrow_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDownArrow_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_DOWNARROW);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc APPEND_NAME_ARG);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonDownArrow_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydDownArrow_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnDownArrow_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnDownArrow_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic DownArrow
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnDownArrow_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnDownArrow_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnDownArrow_EM_YY,  // Ptr to primitive function routine
                                    lptkLftArg,             // Ptr to left arg token
                                    lptkFunc,               // Ptr to function token
                                    lptkRhtArg,             // Ptr to right arg token
                                    lptkAxis);              // Ptr to axis token (may be NULL)
} // End PrimProtoFnDownArrow_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimIdentFnDownArrow_EM_YY
//
//  Generate an identity element for the primitive function dyadic DownArrow
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimIdentDownArrow_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimIdentFnDownArrow_EM_YY
    (LPTOKEN lptkRhtOrig,           // Ptr to original right arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYRes;           // Ptr to result
    APLNELM      aplNELMRes;        // Result NELM
    APLUINT      ByteRes;           // # bytes in the result
    HGLOBAL      hGlbRes;           // Result global memory handle
    LPVOID       lpMemRes;          // Ptr to left arg global memory

    // The right arg is the prototype item from
    //   the original empty arg.

    Assert (lptkRhtOrig NE NULL);
    Assert (lptkFunc    NE NULL);
    Assert (lptkRhtArg  NE NULL);

    // If there's an axis operator, ...
    if (lptkAxis)
    {
        APLRANK aplRankRht;         // Right arg rank

        // Get the attributes (Type, NELM, and Rank) of the right arg
        AttrsOfToken (lptkRhtArg, NULL, NULL, &aplRankRht, NULL);

        // Check the axis values, fill in # elements in axis
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           aplRankRht,      // All values less than this
                           FALSE,           // TRUE iff scalar or one-element vector only
                           FALSE,           // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // TRUE iff fractional values allowed
                           NULL,            // Return last axis value
                           NULL,            // Return # elements in axis vector
                           NULL))           // Return HGLOBAL with APLINT axis values
            goto ERROR_EXIT;

        // The (left) identity function for dyadic DownArrow
        //   (L {downarrow}[X] R) ("drop w/axis") is
        //   ({rho} X){rho} 0.

        // Get the attributes (Type, NELM, and Rank)
        //   of the right arg
        AttrsOfToken (lptkAxis, NULL, &aplNELMRes, NULL, NULL);
    } else
        // The (left) identity function for dyadic DownArrow
        //   (L {downarrow} R) ("drop") is
        //   ({rho} {rho} R){rho} 0.

        // Get the attributes (Type, NELM, and Rank)
        //   of the right arg
        AttrsOfToken (lptkRhtArg, NULL, NULL, &aplNELMRes, NULL);

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_BOOL, aplNELMRes, 1);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result.
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock000 (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_BOOL;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Save the dimension
    * (VarArrayBaseToDim (lpMemRes)) = aplNELMRes;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Allocate a new YYRes;
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
ERROR_EXIT:
    return NULL;
} // End PrimIdentFnDownArrow_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonDownArrow_EM_YY
//
//  Primitive function for monadic DownArrow (ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonDownArrow_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonDownArrow_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End PrimFnMonDownArrow_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydDownArrow_EM_YY
//
//  Primitive function for dyadic DownArrow ("drop")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydDownArrow_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydDownArrow_EM_YY
    (LPTOKEN lptkLftArg,                // Ptr to left arg token
     LPTOKEN lptkFunc,                  // Ptr to function token
     LPTOKEN lptkRhtArg,                // Ptr to right arg token
     LPTOKEN lptkAxis)                  // Ptr to axis token (may be NULL)

{
    APLSTYPE      aplTypeLft,           // Left arg storage type
                  aplTypeRht,           // Right ...
                  aplTypeRes;           // Result   ...
    APLNELM       aplNELMLft,           // Left arg NELM
                  aplNELMRht,           // Right ...
                  aplNELMRes,           // Result   ...
                  aplNELMAxis;          // Axis     ...
    APLRANK       aplRankLft,           // Left arg rank
                  aplRankRht,           // Right ...
                  aplRankRes;           // Result   ...
    HGLOBAL       hGlbLft = NULL,       // Left arg global memory handle
                  hGlbRht = NULL,       // Right ...
                  hGlbRes = NULL,       // Result   ...
                  hGlbAxis = NULL;      // Axis     ...
    LPVOID        lpMemLft = NULL,      // Ptr to left arg global memory
                  lpMemRht = NULL,      // Ptr to right ...
                  lpMemRes = NULL;      // Ptr to result   ...
    HGLOBAL       hGlbTmpLft = NULL,    // Left arg temporary values global memory handle
                  hGlbOdoRht = NULL,    // Right arg odometer ...
                  hGlbWVecRht = NULL;   // Right arg weighting vector ...
    APLLONGEST    aplLongestLft,        // Left arg immediate value
                  aplLongestRht;        // Right arg ...
    APLINT        apaOffRht,            // Right arg APA offset
                  apaMulRht,            // Right arg APA multiplier
                  iRht,                 // Loop counter
                  aplIntLft;            // Temporary left arg integer
    APLUINT       ByteRes,              // # bytes in the result
                  uRes,                 // Loop counter
                  uRht,                 // Loop counter
                  uOdo;                 // Loop counter
    LPAPLDIM      lpMemDimRht = NULL;   // Ptr to right arg dimensions
    LPAPLUINT     lpMemAxisHead = NULL, // Ptr to axis values, fleshed out by CheckAxis_EM
                  lpMemAxisTail = NULL, // Ptr to grade up of AxisHead
                  lpMemOdoRht = NULL,   // Ptr to right arg odometer global memory
                  lpMemWVecRht = NULL;  // Ptr to right arg weighting vector global memory
    LPAPLINT      lpMemTmpLft,          // Ptr to left arg temporary values global memory
                  lpMemLoHiRht;         // Ptr to right arg low/high bounds
    UINT          uBitMask;             // Bit mask for looping through Booleans
    LPPL_YYSTYPE  lpYYRes = NULL;       // Ptr to the result
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // If the right arg is scalar, the result rank is aplNELMLft,
    //   otherwise the result rank is aplRankRht
    if (IsScalar (aplRankRht))
        aplRankRes = aplNELMLft;
    else
        aplRankRes = aplRankRht;

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Check the axis values, fill in # elements in axis
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           aplRankRes,      // All values less than this
                           FALSE,           // TRUE iff scalar or one-element vector only
                           FALSE,           // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // TRUE iff fractional values allowed
                           NULL,            // Return last axis value
                          &aplNELMAxis,     // Return # elements in axis vector
                          &hGlbAxis))       // Return HGLOBAL with APLINT axis values
            return NULL;
        // Lock the memory to get a ptr to it
        lpMemAxisHead = MyGlobalLockInt (hGlbAxis);

        // Get pointer to the axis tail (where the [X] values are)
        lpMemAxisTail = &lpMemAxisHead[aplRankRes - aplNELMAxis];
    } else
        // No axis is the same as all axes
        aplNELMAxis = aplRankRes;

    // Check for RANK error
    if (IsMultiRank (aplRankLft))
        goto RANK_EXIT;

    // Check for LENGTH error
    if (aplNELMLft NE aplNELMAxis)
        goto LENGTH_EXIT;

    // Check for DOMAIN error
    if (!IsNumeric (aplTypeLft)
     && !IsEmpty (aplNELMLft))
        goto DOMAIN_EXIT;

    // Get left & right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Split off case of {zilde}{drop} SimpleScalar
    //               and      ''{drop} SimpleScalar
    if (IsSimple (aplTypeLft)
     && IsEmpty (aplNELMLft)
     && IsSimple (aplTypeRht))
    {
        // Allocate a new YYRes;
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = TranslateArrayTypeToImmType (aplTypeRht);
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkLongest  = aplLongestRht;
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

        goto NORMAL_EXIT;
    } // End IF

    // From here on, the result is a global

    // If the right arg is not immediate, ...
    if (lpMemRht)
    {
        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemRht);

        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemRht);
    } // End IF

    // lpMemRht now points to its data

    // Run through the left arg converting Booleans/floats
    //   to integers,  checking floats for near-integers
    //   calculating the result NELM as x/aplNELMLft, and
    //   saving the resulting normalized left arg in global
    //   memory as signed integers.
    hGlbTmpLft =
      PrimFnDydUpDownArrowLftGlbValid_EM (NULL,             // Ptr to common NELM
                                         &aplNELMRes,       // Ptr to result NELM
                                          aplRankRes,       // Result rank
                                          lpMemLft,         // Ptr to left arg global memory (may be NULL if immediate)
                                          aplTypeLft,       // Left arg storage type
                                          aplLongestLft,    // Left arg immediate value
                                          lpMemDimRht,      // Ptr to right arg global memory
                                          aplRankRht,       // Right arg rank
                                          lpMemAxisTail,    // Ptr to axis global memory (may be NULL if axis not present)
                                          TRUE,             // TRUE iff it's DownArrow
                                          lptkFunc);        // Ptr to function token
    if (hGlbTmpLft EQ NULL)
        goto ERROR_EXIT;

    // Lock the memory to get a ptr to it
    lpMemTmpLft = MyGlobalLockInt (hGlbTmpLft);

    // The storage type of the result is the same as that of the right arg
    //   unless the right arg is hetero and the result is a singleton, or
    //   the right arg is APA
    if (IsSimpleHet (aplTypeRht)
     && IsSingleton (aplNELMRes))
        aplTypeRes = TranslateImmTypeToArrayType ((*(LPAPLHETERO) lpMemRht)->stFlags.ImmType);
    else
    if (IsSimpleAPA (aplTypeRht))
    {
        // The (under-)drop of an APV is an APV
        if (IsVector (aplRankRht)
         && abs64 (lpMemTmpLft[0]) < (APLINT) aplNELMRht)
            aplTypeRes = aplTypeRht;
        else
            aplTypeRes = ARRAY_INT;
    } else
        aplTypeRes = aplTypeRht;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result.
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock000 (hGlbRes);

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

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemRes);

    // Skip over the header and dimensions to the data
    if (lpMemLft)
        lpMemLft = VarArrayDataFmBase (lpMemLft);
    else
        lpMemLft = &aplLongestLft;

    // Get ptr to low/high bounds
    lpMemLoHiRht = &lpMemTmpLft[aplRankRes];

    // Copy the left arg values to the result dimensions
    if (!IsScalar (aplRankRht))
    {
        for (uRes = 0; uRes < aplRankRes; uRes++)
        {
            aplIntLft = lpMemDimRht[uRes] - abs64 (lpMemTmpLft[uRes]);
            *((LPAPLDIM) lpMemRes)++ = max (0, aplIntLft);
        } // End FOR
    } else
    {
        for (uRes = 0; uRes < aplRankRes; uRes++)
        {
            aplIntLft = 1                 - abs64 (lpMemTmpLft[uRes]);
            *((LPAPLDIM) lpMemRes)++ = max (0, aplIntLft);
        } // End FOR
    } // End IF/ELSE

    // lpMemRes now points to its data

    // Handle empty array results
    if (IsEmpty (aplNELMRes))
    {
        // If the result is nested, copy the prototype from the right arg
        if (IsNested (aplTypeRes))
            *((LPAPLNESTED) lpMemRes) = CopySymGlbDir_PTB (*(LPAPLNESTED) lpMemRht);
        goto YYALLOC_EXIT;
    } // End IF

    // If the right arg is an APA, ...
    if (IsSimpleAPA (aplTypeRht))
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        // Get the APA parameters
        apaOffRht = lpAPA->Off;
        apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // Copy the values from the right arg to the result
    //   taking into account
    //   1.  scalar right arg extension
    //   2.  negative values in the left arg
    //   3.  axis operator

    // If the result is an APA, handle specially
    if (IsSimpleAPA (aplTypeRes))
    {
        // If the left arg is positive, ...
        if (lpMemTmpLft[0] > 0)
#define lpAPA           ((LPAPLAPA) lpMemRes)
            // Increase the offset by that amount times the multiplier
            lpAPA->Off = apaOffRht + apaMulRht * lpMemTmpLft[0];
        else
            lpAPA->Off = apaOffRht;
        lpAPA->Mul = apaMulRht;
#undef  lpAPA
        goto YYALLOC_EXIT;
    } // End IF

    // Calculate space needed for the result
    ByteRes = aplRankRes * sizeof (APLUINT);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    //***************************************************************
    // Allocate space for the right arg weighting vector which is
    //   {times}{backscan}1{drop}({rho}R),1
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
    if (lpMemDimRht)
    for (iRht = aplRankRes - 1; iRht >= 0; iRht--)
    {
        lpMemWVecRht[iRht] = uRes;
        uRes *= lpMemDimRht[iRht];
    } // End IF/FOR

    // Calculate space needed for the result
    ByteRes = aplRankRes * sizeof (APLUINT);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    //***************************************************************
    // Allocate space for the right arg odometer array, one value per dimension
    //   in the right arg.
    //***************************************************************
    hGlbOdoRht = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbOdoRht EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemOdoRht = MyGlobalLock000 (hGlbOdoRht);

    // Initialize the odometer array
    CopyMemory (lpMemOdoRht, lpMemLoHiRht, (APLU3264) aplRankRes * sizeof (APLUINT));

    // If the right arg is an immediate, ...
    if (lpMemRht EQ NULL)
        lpMemRht = &aplLongestRht;

    // Loop through the result/right arg copying values
    //   common to the two from the right arg to the result
    for (uRes = 0; uRes < aplNELMRes; uRes++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Use the index in lpMemOdoRht to calculate the
        //   corresponding index in lpMemRht from which
        //   the next value comes.
        for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
            uRht += lpMemOdoRht[uOdo] * lpMemWVecRht[uOdo];

        // Increment the odometer in lpMemOdoRht subject to
        //   the values in lpMemLoHiRht
        IncrLoHiOdometer (lpMemOdoRht, lpMemLoHiRht, aplRankRes);

        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
                uBitMask = BIT0 << (MASKLOG2NBIB & (UINT) uRht);

                // Copy element # uRht from the right arg to lpMemRes[uRes]
                ((LPAPLBOOL)   lpMemRes)[uRes >> LOG2NBIB] |=
                ((uBitMask & ((LPAPLBOOL) lpMemRht)[uRht >> LOG2NBIB]) ? TRUE : FALSE) << (MASKLOG2NBIB & (UINT) uRes);

                break;

            case ARRAY_INT:
                // Copy element # uRht from the right arg to lpMemRes[uRes]
                ((LPAPLINT)    lpMemRes)[uRes] = ((LPAPLINT) lpMemRht)[uRht];

                break;

            case ARRAY_FLOAT:
                // Copy element # uRht from the right arg to lpMemRes[uRes]
                ((LPAPLFLOAT)  lpMemRes)[uRes] = ((LPAPLFLOAT) lpMemRht)[uRht];

                break;

            case ARRAY_APA:
                // Copy element # uRht from the right arg to lpMemRes[uRes]
                ((LPAPLINT)    lpMemRes)[uRes] = apaOffRht + apaMulRht * uRht;

                break;

            case ARRAY_CHAR:
                // Copy element # uRht from the right arg to lpMemRes[uRes]
                ((LPAPLCHAR)   lpMemRes)[uRes] = ((LPAPLCHAR) lpMemRht)[uRht];

                break;

            case ARRAY_NESTED:
            case ARRAY_HETERO:
                // Copy element # uRht from the right arg to lpMemRes[uRes]
                ((LPAPLNESTED) lpMemRes)[uRes] = CopySymGlbDir_PTB (((LPAPLNESTED) lpMemRht)[uRht]);

                break;

            case ARRAY_RAT:
                // Copy element # uRht from the right arg to lpMemRes[uRes]
                mpq_init_set (&((LPAPLRAT) lpMemRes)[uRes], &((LPAPLRAT) lpMemRht)[uRht]);

                break;

            case ARRAY_VFP:
                // Copy element # uRht from the right arg to lpMemRes[uRes]
                mpfr_init_copy (&((LPAPLVFP) lpMemRes)[uRes], &((LPAPLVFP) lpMemRht)[uRht]);

                break;

            defstop
                break;
        } // End SWITCH
    } // End FOR
YYALLOC_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Allocate a new YYRes;
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken);

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

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbWVecRht, lpMemWVecRht);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbOdoRht, lpMemOdoRht);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbAxis, lpMemAxisHead);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbTmpLft, lpMemTmpLft);

    return lpYYRes;
} // End PrimFnDownArrow_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $IncrHiLoOdometer
//
//  Increment the odometer in lpMemOdo subject to the
//    limits in lpMemLoHi of length <aplRank>.
//***************************************************************************

void IncrLoHiOdometer
    (LPAPLUINT lpMemOdo,    // Ptr to the values to increment
     LPAPLDIM  lpMemLoHi,   // Ptr to the limits on each dimension
     APLRANK   aplRank)     // # elements to which each of the
                            //   ptrs point

{
    LPAPLDIM lpMemLo,       // Ptr to low bounds of LoHi
             lpMemHi;       // Ptr to high ...
    APLINT iOdo;            // Loop counter

    // Get the separate ptrs
    lpMemLo =  lpMemLoHi;
    lpMemHi = &lpMemLoHi[aplRank];

    // Loop through the odometer values from right to left
    // Note we use a signed index variable because we're
    //   walking backwards and the test against zero must be
    //   made as a signed variable.
    for (iOdo = aplRank - 1; iOdo >= 0; iOdo--)
    if (++lpMemOdo[iOdo] EQ lpMemHi[iOdo])
        lpMemOdo[iOdo] = lpMemLo[iOdo];
    else
        break;
} // End IncrLoHiOdometer


//***************************************************************************
//  $PrimFnDydUpDownArrowLftGlbValid_EM
//
//  Dyadic Up/DownArrow left argument validation on a global memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydUpDownArrowLftGlbValid_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL PrimFnDydUpDownArrowLftGlbValid_EM
    (LPAPLNELM  lpaplNELMCom,           // Ptr to common NELM (may be NULL if DownArrow)
     LPAPLNELM  lpaplNELMRes,           // Ptr to result NELM
     APLRANK    aplRankRes,             // Result rank
     LPVOID     lpMemLft,               // Ptr to left arg global memory (may be NULL if immediate)
     APLSTYPE   aplTypeLft,             // Left arg storage type
     APLLONGEST aplLongestLft,          // Left arg immediate value
     LPAPLDIM   lpMemDimRht,            // Ptr to right arg dimensions global memory
     APLRANK    aplRankRht,             // Right arg rank
     LPAPLUINT  lpMemAxisTail,          // Ptr to grade up of AxisHead (may be NULL if axis not present)
     UBOOL      bDownArrow,             // TRUE iff it's DownArrow
     LPTOKEN    lptkFunc)               // Ptr to function token

{
    LPVOID   lpDataLft;                 // Ptr to left arg data
    APLNELM  aplNELMLft;                // Left arg NELM
    APLRANK  aplRankLft;                // Left arg rank
    UBOOL    bRet = TRUE;               // TRUE iff result is valid
    APLUINT  uDim,                      // Loop counter
             uRes,                      // Loop counter
             ByteRes;                   // # bytes in the result
    UINT     uBitMask;                  // Bit mask for looping through Booleans
    APLINT   aplIntTmp,                 // Temporary integer
             aplIntLft;                 // Temporary left arg integer
    HGLOBAL  hGlbTmpLft;                // Left arg temporary values global memory handle
    LPAPLINT lpMemTmpLft,               // Ptr to left arg temporary values global memory
             lpMemTmpLoRht,             // ...    right arg low bound
             lpMemTmpHiRht,             // ...              high bound
             lpMemTmpLoRes,             // ...    result    low bound
             lpMemTmpHiRes;             // ...              high bound

    // If the left arg is not immediate, ...
    if (lpMemLft)
    {
#define lpHeader        ((LPVARARRAY_HEADER) lpMemLft)
////////aplTypeLft = lpHeader->ArrType;         // Already passed as argument
        aplNELMLft = lpHeader->NELM;
        aplRankLft = lpHeader->Rank;
#undef  lpHeader
        // Point to the left arg data
        lpDataLft = VarArrayDataFmBase (lpMemLft);
    } else
    {
        aplNELMLft = 1;
        aplRankLft = 0;

        // Point to the left arg data
        lpDataLft = &aplLongestLft;
    } // End IF/ELSE

    // Calculate space for a normalized left arg
    ByteRes = (5 - bDownArrow * 2) * aplRankRes * sizeof (APLINT);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        return NULL;

    // Allocate space for a normalized left arg
    // Three copies for DownArrow (TmpLft, LoRht, HiRht),
    //   five for UpArrow (TmpLft, LoRht, HiRht, LoRes, HiRes)
    hGlbTmpLft = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbTmpLft EQ NULL)
        return NULL;

    // Lock the memory to get a ptr to it
    lpMemTmpLft  = MyGlobalLock000 (hGlbTmpLft);
    lpMemTmpLoRht = &lpMemTmpLft   [aplRankRes];
    lpMemTmpHiRht = &lpMemTmpLoRht [aplRankRes];
    if (!bDownArrow)
    {
        lpMemTmpLoRes = &lpMemTmpHiRht[aplRankRes];
        lpMemTmpHiRes = &lpMemTmpLoRes[aplRankRes];
    } // End IF

    // If there's an axis operator, and
    //   the right arg is not scalar, and
    //   it's UpArrow
    if (lpMemAxisTail
     && !IsScalar (aplRankRht)
     && !bDownArrow)
        // Copy the right arg dimensions to the normalized left arg
        //   some of which will be overwritten by the actual left arg,
        //   the rest of which are the elided dimensions and hence
        //   are unchanged from the right arg to the result.
        CopyMemory (lpMemTmpLft, lpMemDimRht, (APLU3264) aplRankRht * sizeof (APLDIM));

    // Check for LEFT DOMAIN ERROR and fill in lpMemTmpLft
    // Split cases based upon the left arg's storage type
    switch (aplTypeLft)
    {
        case ARRAY_BOOL:
            uBitMask = BIT0;

            for (uDim = 0; bRet && uDim < aplNELMLft; uDim++)
            {
                // Get the left arg value
                aplIntLft = (uBitMask & *(LPAPLBOOL) lpDataLft) ? TRUE : FALSE;

                // If there's an axis, ...
                if (lpMemAxisTail)
                    uRes = lpMemAxisTail[uDim];
                else
                    uRes = uDim;

                // Save in the normalized global memory
                lpMemTmpLft[uRes] = aplIntLft;

                // Shift over the left bit mask
                uBitMask <<= 1;

                // Check for end-of-byte
                if (uBitMask EQ END_OF_BYTE)
                {
                    uBitMask = BIT0;            // Start over
                    ((LPAPLBOOL) lpDataLft)++;  // Skip to next byte
                } // End IF
            } // End FOR

            break;

        case ARRAY_INT:
            for (uDim = 0; bRet && uDim < aplNELMLft; uDim++)
            {
                // Get the left arg value
                aplIntLft = ((LPAPLINT) lpDataLft)[uDim];

                // If there's an axis, ...
                if (lpMemAxisTail)
                    uRes = lpMemAxisTail[uDim];
                else
                    uRes = uDim;

                // Save in the normalized global memory
                lpMemTmpLft[uRes] = aplIntLft;
            } // End FOR

            break;

        case ARRAY_FLOAT:
            for (uDim = 0; bRet && uDim < aplNELMLft; uDim++)
            {
                // Attempt to convert the float to an integer using System []CT
                aplIntLft = FloatToAplint_SCT (((LPAPLFLOAT) lpDataLft)[uDim], &bRet);
                if (bRet)
                {
                    // If there's an axis, ...
                    if (lpMemAxisTail)
                        uRes = lpMemAxisTail[uDim];
                    else
                        uRes = uDim;

                    // Save in the normalized global memory
                    lpMemTmpLft[uRes] = aplIntLft;
                } // End IF/ELSE
            } // End FOR

            break;

        case ARRAY_APA:
        {
            APLINT  apaOffLft,
                    apaMulLft;

#define lpAPA       ((LPAPLAPA) lpDataLft)
            // Get the APA parameters
            apaOffLft = lpAPA->Off;
            apaMulLft = lpAPA->Mul;
#undef  lpAPA
            for (uDim = 0; bRet && uDim < aplNELMLft; uDim++)
            {
                aplIntLft = apaOffLft + apaMulLft * uDim;
                if (aplIntLft > MAX_APLNELM)
                    bRet = FALSE;
                else
                {
                    // If there's an axis, ...
                    if (lpMemAxisTail)
                        uRes = lpMemAxisTail[uDim];
                    else
                        uRes = uDim;

                    // Save in the normalized global memory
                    lpMemTmpLft[uRes] = aplIntLft;
                } // End IF/ELSE
            } // End FOR

            break;
        } // End ARRAY_APA

        case ARRAY_CHAR:
        case ARRAY_NESTED:          // We could check for promotion to simple numeric
        case ARRAY_HETERO:          // ...
            bRet = FALSE;

            break;

        case ARRAY_RAT:
            for (uDim = 0; bRet && uDim < aplNELMLft; uDim++)
            {
                // Attempt to convert the RAT to an integer using System []CT
                aplIntLft = mpq_get_sctsx (&((LPAPLRAT) lpDataLft)[uDim], &bRet);
                if (bRet)
                {
                    // If there's an axis, ...
                    if (lpMemAxisTail)
                        uRes = lpMemAxisTail[uDim];
                    else
                        uRes = uDim;

                    // Save in the normalized global memory
                    lpMemTmpLft[uRes] = aplIntLft;
                } // End IF/ELSE
            } // End FOR

            break;

        case ARRAY_VFP:
            for (uDim = 0; bRet && uDim < aplNELMLft; uDim++)
            {
                // Attempt to convert the VFP to an integer using System []CT
                aplIntLft = mpfr_get_sctsx (&((LPAPLVFP) lpDataLft)[uDim], &bRet);
                if (bRet)
                {
                    // If there's an axis, ...
                    if (lpMemAxisTail)
                        uRes = lpMemAxisTail[uDim];
                    else
                        uRes = uDim;

                    // Save in the normalized global memory
                    lpMemTmpLft[uRes] = aplIntLft;
                } // End IF/ELSE
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH

    if (bRet)
    {
        // Initialize with identity element for multiply
        *lpaplNELMRes = 1;
        if (lpaplNELMCom)
            *lpaplNELMCom = 1;

        // Loop through the normalized left arg
        //   and fill in *lpaplNELMRes, lpMemTmpLo, and lpMemTmpHi
        for (uDim = 0; bRet && uDim < aplRankRes; uDim++)
        {
            // Get the next value
            aplIntLft = abs64 (lpMemTmpLft[uDim]);
            if (!IsScalar (aplRankRht))
                aplIntTmp = lpMemDimRht[uDim];
            else
                aplIntTmp = 1;
            // If it's DownArrow, ...
            if (bDownArrow)
            {
                // If lpMemTmpLft[uDim] is negative, ...
                if (SIGN_APLNELM (lpMemTmpLft[uDim]))
                {
                    // The lower bound is ...
                    *lpMemTmpLoRht++ = 0;                               // Inclusive

                    // The upper bound is ...
                    *lpMemTmpHiRht++ = max (0, aplIntTmp - aplIntLft);  // Exclusive
                } else
                {
                    // The lower bound is ...
                    *lpMemTmpLoRht++ = aplIntLft;                       // Inclusive

                    // The upper bound is ...
                    *lpMemTmpHiRht++ = aplIntTmp;                       // Exclusive
                } // End IF/ELSE

                // Calculate the remaining dimension length
                if (!IsScalar (aplRankRht))
                    aplIntLft = aplIntTmp - aplIntLft;
                else
                    aplIntLft = 1 - aplIntLft;

                // The dimension is always >= 0 (handle overdrop)
                aplIntLft = max (0, aplIntLft);
            } else
            // It's UpArrow
            {
                // Calculate the common NELM
                *lpaplNELMCom *= min (aplIntLft, aplIntTmp);

                // If lpMemTmpLft[uDim] is negative, ...
                if (SIGN_APLNELM (lpMemTmpLft[uDim]))
                {
                    // The lower bound is ...
                    *lpMemTmpLoRht++ = max (0, aplIntTmp - aplIntLft);  // Inclusive

                    // The upper bound is ...
                    *lpMemTmpHiRht++ = aplIntTmp;                       // Exclusive

                    // The lower bound is ...
                    *lpMemTmpLoRes++ = max (0, aplIntLft - aplIntTmp);  // Inclusive

                    // The upper bound is ...
                    *lpMemTmpHiRes++ = aplIntLft;                       // Exclusive
                } else
                {
                    // The lower bound is ...
                    *lpMemTmpLoRht++ = 0;                               // Inclusive

                    // The upper bound is ...
                    *lpMemTmpHiRht++ = min (aplIntLft, aplIntTmp);      // Exclusive

                    // The lower bound is ...
                    *lpMemTmpLoRes++ = 0;

                    // The upper bound is ...
                    *lpMemTmpHiRes++ = min (aplIntTmp, aplIntLft);      // Exclusive
                } // End IF/ELSE
            } // End IF/ELSE

            // Multiply the two numbers as APLINTs so we can check for overflow
            aplIntTmp = imul64 (*lpaplNELMRes, aplIntLft, &bRet);

            // Ensure the value fits into a dimension
            bRet = bRet && (aplIntTmp <= MAX_APLDIM);

            // Save back
            *lpaplNELMRes = aplIntTmp;
        } // End FOR
    } // End IF

    // If error, it's a DOMAIN ERROR
    if (!bRet)
    {
        // Unlock and free (and set to NULL) a global name and ptr
        UnlFreeGlbName (hGlbTmpLft, lpMemTmpLft);

        goto DOMAIN_EXIT;
    } else
    if (lpMemTmpLft)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbTmpLft); lpMemTmpLft = NULL;
    } // End IF

    return hGlbTmpLft;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFnDydUpDownArrowLftGlbValid_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_darrow.c
//***************************************************************************
