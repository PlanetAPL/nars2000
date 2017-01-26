//***************************************************************************
//  NARS2000 -- Primitive Function -- UpArrow
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
//  $PrimFnUpArrow_EM_YY
//
//  Primitive function for monadic and dyadic UpArrow ("first" and "take")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnUpArrow_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnUpArrow_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_UPARROW);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc APPEND_NAME_ARG);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonUpArrow_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydUpArrow_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnUpArrow_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnUpArrow_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic UpArrow
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnUpArrow_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnUpArrow_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnUpArrow_EM_YY,// Ptr to primitive function routine
                                    lptkLftArg,         // Ptr to left arg token
                                    lptkFunc,           // Ptr to function token
                                    lptkRhtArg,         // Ptr to right arg token
                                    lptkAxis);          // Ptr to axis token (may be NULL)
} // End PrimProtoFnUpArrow_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimIdentFnUpArrow_EM_YY
//
//  Generate an identity element for the primitive function dyadic UpArrow
//***************************************************************************

LPPL_YYSTYPE PrimIdentFnUpArrow_EM_YY
    (LPTOKEN lptkRhtOrig,           // Ptr to original right arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYRes,           // Ptr to result
                 lpYYRes2;          // Ptr to secondary result
    TOKEN        tkFcn = {0};       // Function token

    // The right arg is the prototype item from
    //   the original empty arg.

    Assert (lptkRhtOrig NE NULL);
    Assert (lptkFunc    NE NULL);
    Assert (lptkRhtArg  NE NULL);

    // Setup a token with the {rho} function
    tkFcn.tkFlags.TknType   = TKT_FCNIMMED;
    tkFcn.tkFlags.ImmType   = IMMTYPE_PRIMFCN;
////tkFcn.tkFlags.NoDisplay = FALSE;           // Already zero from = {0}
    tkFcn.tkData.tkChar     = UTF16_RHO;
    tkFcn.tkCharIndex       = lptkFunc->tkCharIndex;

    // Compute {rho} R
    lpYYRes =
      PrimFnMonRho_EM_YY (&tkFcn,       // Ptr to function token
                           lptkRhtArg,  // Ptr to right arg token
                           NULL);       // Ptr to axis token (may be NULL)
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

        // The (left) identity function for dyadic UpArrow
        //   (L {uparrow}[X] R) ("take w/axis") is
        //   ({rho} R)[X].
        lpYYRes2 =
          ArrayIndexRef_EM_YY (&lpYYRes->tkToken,
                                lptkAxis);
        // Free the YYRes
        FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;

        return lpYYRes2;
    } // End IF

    // The (left) identity function for dyadic UpArrow
    //   (L {uparrow} R) ("take") is
    //   {rho} R.
    return lpYYRes;

ERROR_EXIT:
    // Free the YYRes
    FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;

    return NULL;
} // End PrimIdentFnUpArrow_EM_YY


//***************************************************************************
//  $PrimFnMonUpArrow_EM_YY
//
//  Primitive function for monadic UpArrow ("first")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonUpArrow_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonUpArrow_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLLONGEST   aplLongest;
    HGLOBAL      lpSymGlb;
    IMM_TYPES    immType;
    APLSTYPE     aplTypeRht;
    LPPL_YYSTYPE lpYYRes;

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, NULL, NULL, NULL);

    // Extract the first item (or the prototype) from the right arg
    GetFirstValueToken (lptkRhtArg,     // Ptr to right arg token
                        NULL,           // Ptr to integer result
                        NULL,           // Ptr to float ...
                        NULL,           // Ptr to WCHAR ...
                       &aplLongest,     // Ptr to longest ...
                       &lpSymGlb,       // Ptr to lpSym/Glb ...
                       &immType,        // Ptr to ...immediate type ...
                        NULL);          // Ptr to array type ...
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // If the first value is an array, ...
    if (lpSymGlb)
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = CopySymGlbNumDir_PTB (lpSymGlb, aplTypeRht, lptkFunc);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

        // Check for errors
        if (lpYYRes->tkToken.tkData.tkGlbData EQ NULL)
            goto ERROR_EXIT;

        // See if it fits into a lower (but not necessarily smaller) datatype
        TypeDemote (&lpYYRes->tkToken);
    } else
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = immType;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkLongest  = aplLongest;
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } // End IF/ELSE

    return lpYYRes;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

ERROR_EXIT:
    return NULL;
} // End PrimFnMonUpArrow_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydUpArrow_EM_YY
//
//  Primitive function for dyadic UpArrow ("take")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydUpArrow_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydUpArrow_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE      aplTypeLft,           // Left arg storage type
                  aplTypeRht,           // Right ...
                  aplTypeRes;           // Result   ...
    APLNELM       aplNELMLft,           // Left arg NELM
                  aplNELMRht,           // Right ...
                  aplNELMRes,           // Result   ...
                  aplNELMCom,           // Common   ...
                  aplNELMAxis;          // Axis     ...
    APLRANK       aplRankLft,           // Left arg rank
                  aplRankRht,           // Right ...
                  aplRankRes;           // Result   ...
    HGLOBAL       hGlbLft = NULL,       // Left arg global memory handle
                  hGlbRht = NULL,       // Right ...
                  hGlbRes = NULL,       // Result   ...
                  hGlbAxis = NULL,      // Axis     ...
                  hGlbTmpLft = NULL,    // Left arg temporary values global memory handle
                  hGlbOdoRht = NULL,    // Right arg odometer ...
                  hGlbOdoRes = NULL,    // Result    ...
                  hGlbWVecRht = NULL,   // Right arg weighting vector ...
                  hGlbWVecRes = NULL;   // Result    ...
    LPVOID        lpMemLft = NULL,      // Ptr to left arg global memory
                  lpMemRht = NULL,      // Ptr to right ...
                  lpMemRes = NULL;      // Ptr to result   ...
    APLLONGEST    aplLongestLft,        // Left arg immediate value
                  aplLongestRht;        // Right arg ...
    APLINT        apaOffRht,            // Right arg APA offset
                  apaMulRht,            // Right arg APA multiplier
                  iRht;                 // Loop counter
    APLUINT       ByteRes,              // # bytes in the result
                  uRes,                 // Loop counter
                  uRhtWVec,             // Loop counter
                  uResWVec,             // Loop counter
                  uOdo;                 // Loop counter
    LPAPLDIM      lpMemDimRht = NULL,   // Ptr to right arg dimensions
                  lpMemDimRes = NULL;   // Ptr to result    ...
    LPAPLUINT     lpMemAxisHead,        // Ptr to axis values, fleshed out by CheckAxis_EM
                  lpMemAxisTail = NULL, // Ptr to grade up of AxisHead
                  lpMemOdoRht = NULL,   // Ptr to right arg odometer global memory
                  lpMemOdoRes = NULL,   // Ptr to result    ...
                  lpMemWVecRht = NULL,  // Ptr to right arg weighting vector global memory
                  lpMemWVecRes = NULL;  // Ptr to result    ...
    LPAPLINT      lpMemTmpLft,          // Ptr to left arg temporary values global memory
                  lpMemLoHiRht,         // Ptr to right arg low/high bounds
                  lpMemLoHiRes;         // Ptr to result    ...
    UINT          uBitMask;             // Bit mask for looping through Booleans
    LPPL_YYSTYPE  lpYYRes = NULL;       // Ptr to the result
    APLNESTED     aplProtoGlb;          // Right arg prototype if nested
    APLHETERO     aplProtoSym;          // Right arg prototype if hetero
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    LPPERTABDATA  lpMemPTD;             // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

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

    // Check for LEFT RANK error
    if (IsMultiRank (aplRankLft))
        goto LEFT_RANK_EXIT;

    // Check for LEFT LENGTH error
    if (aplNELMLft NE aplNELMAxis)
        goto LEFT_LENGTH_EXIT;

    // Check for LEFT DOMAIN error
    if (!IsSimpleGlbNum (aplTypeLft)
     && !IsCharEmpty (aplTypeLft, aplNELMLft))
        goto LEFT_DOMAIN_EXIT;

    // Get left & right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Split off case of {zilde}{take} SimpleScalar
    //               and      ''{take} SimpleScalar
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
      PrimFnDydUpDownArrowLftGlbValid_EM (&aplNELMCom,      // Ptr to common NELM
                                          &aplNELMRes,      // Ptr to result NELM
                                           aplRankRes,      // Result rank
                                           lpMemLft,        // Ptr to left arg global memory (may be NULL if immediate)
                                           aplTypeLft,      // Left arg storage type
                                           aplLongestLft,   // Left arg immediate value
                                           lpMemDimRht,     // Ptr to right arg global memory
                                           aplRankRht,      // Right arg rank
                                           lpMemAxisTail,   // Ptr to grade up of AxisHead (may be NULL if axis not present)
                                           FALSE,           // TRUE iff it's DownArrow
                                           lptkFunc);       // Ptr to function token
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
        // The (under-)take of an APV is an APV
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
    lpMemDimRes = lpMemRes = VarArrayBaseToDim (lpMemRes);

    // Skip over the header and dimensions to the data
    if (lpMemLft)
        lpMemLft = VarArrayDataFmBase (lpMemLft);
    else
        lpMemLft = &aplLongestLft;

    // Get ptr to the right arg low/high bounds
    lpMemLoHiRht = &lpMemTmpLft [aplRankRes];

    // Get ptr to the result    low/high bounds
    lpMemLoHiRes = &lpMemLoHiRht[aplRankRes * 2];

    // Copy the left arg values to the result dimensions
    for (uRes = 0; uRes < aplRankRes; uRes++)
        *((LPAPLDIM) lpMemRes)++ = abs64 (lpMemTmpLft[uRes]);

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
    //   3.  prototypes from overtake.
    //   4.  axis operator

    // If the result is an APA, handle specially
    if (IsSimpleAPA (aplTypeRes))
    {
        // If the left arg is negative, ...
        if (lpMemTmpLft[0] < 0)
#define lpAPA           ((LPAPLAPA) lpMemRes)
            // Increase the offset by the NELM-complement of that amount times the multiplier
            lpAPA->Off = apaOffRht + apaMulRht * (aplNELMRht + lpMemTmpLft[0]);
        else
            lpAPA->Off = apaOffRht;
        lpAPA->Mul = apaMulRht;
#undef  lpAPA
        goto YYALLOC_EXIT;
    } // End IF

    // If the result is simple char, and
    //   the common NELM is less than the result NELM
    //   fill the result with the prototype of the right arg
    if (IsSimpleChar (aplTypeRes)
     && aplNELMCom < aplNELMRes)
    for (uRes = 0; uRes < aplNELMRes; uRes++)
        ((LPAPLCHAR) lpMemRes)[uRes] = L' ';

    // Calculate space needed for the weighting vector
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

    // Calculate space needed for the odometer array
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

    // Initialize the right arg odometer array
    CopyMemory (lpMemOdoRht, lpMemLoHiRht, (APLU3264) aplRankRes * sizeof (APLUINT));

    // Calculate space needed for the weighting vector
    ByteRes = aplRankRes * sizeof (APLUINT);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    //***************************************************************
    // Allocate space for the result weighting vector which is
    //   {times}{backscan}1{drop}({rho}Z),1
    //***************************************************************
    hGlbWVecRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbWVecRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemWVecRes = MyGlobalLock000 (hGlbWVecRes);

    // Loop through the dimensions of the result in reverse
    //   order {backscan} and compute the cumulative product
    //   (result weighting vector).
    // Note we use a signed index variable because we're
    //   walking backwards and the test against zero must be
    //   made as a signed variable.
    uRes = 1;
    for (iRht = aplRankRes - 1; iRht >= 0; iRht--)
    {
        lpMemWVecRes[iRht] = uRes;
        uRes *= lpMemDimRes[iRht];
    } // End IF/FOR

    // Calculate space needed for the odometer array
    ByteRes = aplRankRes * sizeof (APLUINT);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    //***************************************************************
    // Allocate space for the result odometer array, one value per dimension
    //   in the result.
    //***************************************************************
    hGlbOdoRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbOdoRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemOdoRes = MyGlobalLock000 (hGlbOdoRes);

    // Initialize the result odometer array
    CopyMemory (lpMemOdoRes, lpMemLoHiRes, (APLU3264) aplRankRes * sizeof (APLUINT));

    // If the right arg is an immediate, ...
    if (lpMemRht EQ NULL)
        lpMemRht = &aplLongestRht;

    // Loop through the result/right arg copying values
    //   common to the two from the right arg to the result
    for (uRes = 0; uRes < aplNELMCom; uRes++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Use the index in lpMemOdoRht to calculate the
        //   corresponding index in lpMemRht from which
        //   the next value comes.
        for (uRhtWVec = uOdo = 0; uOdo < aplRankRht; uOdo++)
            uRhtWVec += lpMemOdoRht[uOdo] * lpMemWVecRht[uOdo];

        // Increment the odometer in lpMemOdoRht subject to
        //   the values in lpMemLoHiRht
        IncrLoHiOdometer (lpMemOdoRht, lpMemLoHiRht, aplRankRes);

        // Use the index in lpMemOdoRes to calculate the
        //   corresponding index in lpMemRes to which
        //   the next value goes.
        for (uResWVec = uOdo = 0; uOdo < aplRankRes; uOdo++)
            uResWVec += lpMemOdoRes[uOdo] * lpMemWVecRes[uOdo];

        // Increment the odometer in lpMemOdoRes subject to
        //   the values in lpMemLoHiRes
        IncrLoHiOdometer (lpMemOdoRes, lpMemLoHiRes, aplRankRes);

        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
                uBitMask = BIT0 << (MASKLOG2NBIB & (UINT) uRhtWVec);

                // Copy element # uRhtWVec from the right arg to lpMemRes]
                ((LPAPLBOOL)   lpMemRes)[uResWVec >> LOG2NBIB] |=
                ((uBitMask & ((LPAPLBOOL) lpMemRht)[uRhtWVec >> LOG2NBIB]) ? TRUE : FALSE) << (MASKLOG2NBIB & (UINT) uResWVec);

                break;

            case ARRAY_INT:
                // Copy element # uRhtWVec from the right arg to lpMemRes
                ((LPAPLINT)    lpMemRes)[uResWVec] = ((LPAPLINT) lpMemRht)[uRhtWVec];

                break;

            case ARRAY_FLOAT:
                // Copy element # uRhtWVec from the right arg to lpMemRes
                ((LPAPLFLOAT)  lpMemRes)[uResWVec] = ((LPAPLFLOAT) lpMemRht)[uRhtWVec];

                break;

            case ARRAY_APA:
                // Copy element # uRhtWVec from the right arg to lpMemRes
                ((LPAPLINT)    lpMemRes)[uResWVec] = apaOffRht + apaMulRht * uRhtWVec;;

                break;

            case ARRAY_CHAR:
                // Copy element # uRhtWVec from the right arg to lpMemRes
                ((LPAPLCHAR)   lpMemRes)[uResWVec] = ((LPAPLCHAR) lpMemRht)[uRhtWVec];

                break;

            case ARRAY_NESTED:
            case ARRAY_HETERO:
                // Copy element # uRhtWVec from the right arg to lpMemRes
                ((LPAPLNESTED) lpMemRes)[uResWVec] = CopySymGlbDir_PTB (((LPAPLNESTED) lpMemRht)[uRhtWVec]);

                break;

            case ARRAY_RAT:
                // Copy element # uRhtWVec from the right arg to lpMemRes
                mpq_init_set (&((LPAPLRAT) lpMemRes)[uResWVec],
                              &((LPAPLRAT) lpMemRht)[uRhtWVec]);
                break;

            case ARRAY_VFP:
                // Copy element # uRhtWVec from the right arg to lpMemRes
                mpfr_init_copy (&((LPAPLVFP) lpMemRes)[uResWVec],
                                &((LPAPLVFP) lpMemRht)[uRhtWVec]);
                break;

            defstop
                break;
        } // End SWITCH
    } // End FOR

    // Fill in the prototype elements

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_CHAR:
        case ARRAY_APA:
            break;

        case ARRAY_HETERO:
            // Get the first element
            aplProtoSym = *(LPAPLHETERO) lpMemRht;

            // Split cases based upon the immediate type
            switch (aplProtoSym->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                case IMMTYPE_INT:
                case IMMTYPE_FLOAT:
                    // Get the appropriate prototype
                    aplProtoSym = lpMemPTD->lphtsGLB->steZero;

                    break;

                case IMMTYPE_CHAR:
                    // Get the appropriate prototype
                    aplProtoSym = lpMemPTD->lphtsGLB->steBlank;

                    break;

                defstop
                    break;
            } // End SWITCH

            // Loop through the result filling in prototype values
            for (uRes = 0; uRes < aplNELMRes; uRes++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (*lpbCtrlBreak))
                    goto ERROR_EXIT;

                if (((LPAPLNESTED) lpMemRes)[uRes] EQ NULL)
                    ((LPAPLHETERO) lpMemRes)[uRes] = aplProtoSym;
            } // End FOR

            break;

        case ARRAY_NESTED:
            // Get the right arg prototype
            aplProtoGlb =
              MakeMonPrototype_EM_PTB (*(LPAPLNESTED) lpMemRht, // Proto arg handle
                                       lptkFunc,                // Ptr to function token
                                       MP_CHARS);               // CHARS allowed
            if (!aplProtoGlb)
                goto ERROR_EXIT;

            // Loop through the result filling in prototype values
            for (uRes = 0; uRes < aplNELMRes; uRes++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (*lpbCtrlBreak))
                    goto ERROR_EXIT;

                if (((LPAPLNESTED) lpMemRes)[uRes] EQ NULL)
                    ((LPAPLNESTED) lpMemRes)[uRes] = CopySymGlbDir_PTB (aplProtoGlb);
            } // End FOR

            // If the prototype is a global memory handle, ...
            if (GetPtrTypeDir (aplProtoGlb) EQ PTRTYPE_HGLOBAL)
            {
                // We no longer need this storage
                FreeResultGlobalVar (aplProtoGlb); aplProtoGlb = NULL;
            } // End IF/ELSE

            break;

        case ARRAY_RAT:
            // Loop through the result filling in prototype values
            for (uRes = 0; uRes < aplNELMRes; uRes++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (*lpbCtrlBreak))
                    goto ERROR_EXIT;

                if (IsMpqNULL (&((LPAPLRAT) lpMemRes)[uRes]))
                    // Initialize the result to 0/1
                    mpq_init (&((LPAPLRAT) lpMemRes)[uRes]);
            } // End FOR

            break;

        case ARRAY_VFP:
            // Loop through the result filling in prototype values
            for (uRes = 0; uRes < aplNELMRes; uRes++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (*lpbCtrlBreak))
                    goto ERROR_EXIT;

                if (IsMpfNULL (&((LPAPLVFP) lpMemRes)[uRes]))
                    // Initialize the result to 0
                    mpfr_init0 (&((LPAPLVFP) lpMemRes)[uRes]);
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH
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

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LEFT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
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
    UnlFreeGlbName (hGlbWVecRes, lpMemWVecRes);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbOdoRes, lpMemOdoRes);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbWVecRht, lpMemWVecRht);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbOdoRht, lpMemOdoRht);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbAxis, lpMemAxisHead);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbTmpLft, lpMemTmpLft);

    return lpYYRes;
} // End PrimFnUpArrow_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_uarrow.c
//***************************************************************************
