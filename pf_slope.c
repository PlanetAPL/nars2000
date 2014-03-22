//***************************************************************************
//  NARS2000 -- Primitive Function -- Slope
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
#include "headers.h"


//***************************************************************************
//  $PrimFnSlope_EM_YY
//
//  Primitive function for monadic and dyadic Slope (ERROR and "expand")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnSlope_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnSlope_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_SLOPE
         || lptkFunc->tkData.tkChar EQ UTF16_SLOPEBAR);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc APPEND_NAME_ARG);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonSlope_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydSlope_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnSlope_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnSlope_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic Slope
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnSlope_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnSlope_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnSlope_EM_YY,  // Ptr to primitive function routine
                                    lptkLftArg,         // Ptr to left arg token
                                    lptkFunc,           // Ptr to function token
                                    lptkRhtArg,         // Ptr to right arg token
                                    lptkAxis);          // Ptr to axis token (may be NULL)
} // End PrimProtoFnSlope_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonSlope_EM_YY
//
//  Primitive function for monadic Slope (ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonSlope_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonSlope_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End PrimFnMonSlope_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydSlope_EM_YY
//
//  Primitive function for dyadic Slope ("expand")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSlope_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydSlope_EM_YY
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
                  aplNELMRes;           // Result ...
    APLRANK       aplRankLft,           // Left arg rank
                  aplRankRht,           // Right ...
                  aplRankRes;           // Result   ...
    HGLOBAL       hGlbLft = NULL,       // Left arg global memory handle
                  hGlbRht = NULL,       // Right ...
                  hGlbRes = NULL,       // Result   ...
                  hGlbRep = NULL;       // Replicate ...
    LPAPLDIM      lpMemDimRht = NULL;   // Ptr to right arg dimensions
    LPVOID        lpMemLft = NULL,      // Ptr to left arg global memory
                  lpMemRht = NULL,      // Ptr to right ...
                  lpMemRes = NULL;      // Ptr to result   ...
    LPAPLINT      lpMemRep = NULL;      // Ptr to replicate ...
    UBOOL         bRet;                 // TRUE iff result is valid
    APLUINT       aplAxis,              // The (one and only) axis value
                  ByteRes,              // # bytes in the result
                  uLo,                  //
                  uDimLo,               //
                  uAx,                  //
                  uDimAxRht,            //
                  uDimLftSum,           //
                  uHi,                  //
                  uDimHi,               //
                  uDim,                 //
                  uDimRht,              //
                  uDimRes,              //
                  uRes,                 //
                  uRht,                 //
                  uAcc,                 //
                  uLen;                 //
    APLINT        aplIntegerLft,        //
                  aplIntegerRep,        //
                  apaOff,               //
                  apaMul;               //
    APLFLOAT      aplFloatLft,          //
                  aplFloatRep;          //
    APLCHAR       aplCharRep;           //
    APLNESTED     aplNestLft,           // Ptr to left arg global numeric
                  aplNestRep,           //
                  aplNestProto = NULL;  //
    LPAPLRAT      aplRatRep;            // Temporary RAT
    LPAPLVFP      aplVfpRep;            // ...       VFP
    LPPL_YYSTYPE  lpYYRes = NULL;       // Ptr to the result
    UINT          uBitMask,             //
                  uBitIndex;            //
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    LPVARARRAY_HEADER lpMemHdrRht;      // Ptr to right arg header

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // The rank of the result is the same as that of the right arg
    //   with scalar right arg promoted to a vector
    aplRankRes = max (aplRankRht, 1);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Expand allows a single integer axis value only
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           aplRankRes,      // All values less than this
                           TRUE,            // TRUE iff scalar or one-element vector only
                           FALSE,           // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // Return TRUE iff fractional values present
                          &aplAxis,         // Return last axis value
                           NULL,            // Return # elements in axis vector
                           NULL))           // Return HGLOBAL with APLINT axis values
            return NULL;
    } else
    {
        // No axis specified:
        //   if Slope, use last dimension
        if (lptkFunc->tkData.tkChar EQ UTF16_SLOPE)
            aplAxis = aplRankRes - 1;
        else
        {
            Assert (lptkFunc->tkData.tkChar EQ UTF16_SLOPEBAR);

            // Otherwise, it's SlopeBar on the first dimension
            aplAxis = 0;
        } // End IF/ELSE
    } // End IF/ELSE

    // Get left and right arg's global ptrs
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Check for LEFT RANK ERROR
    if (IsMultiRank (aplRankLft))
        goto LEFT_RANK_EXIT;

    // Check for LEFT DOMAIN ERROR
    if (!IsNumeric (aplTypeLft)
     && !(IsSimpleChar (aplTypeLft) && IsEmpty (aplNELMLft)))
        goto LEFT_DOMAIN_EXIT;

    // Save a ptr to the right arg header
    lpMemHdrRht = lpMemRht;

    // If the right arg is a global, ...
    if (lpMemRht)
        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemRht);

    // Calculate product of dimensions before, at, and after the axis dimension
    if (IsSingleton (aplNELMRht))
        uDimLo = uDimAxRht = uDimHi = 1;
    else
    {
        // Calculate the product of the right arg's dimensions below the axis dimension
        uDimLo = 1;
        for (uDim = 0; uDim < aplAxis; uDim++)
            uDimLo *= lpMemDimRht[uDim];

        // Get the length of the axis dimension
        uDimAxRht = lpMemDimRht[uDim++];

        // Calculate the product of the right arg's dimensions above the axis dimension
        uDimHi = 1;
        for (; uDim < aplRankRht; uDim++)
            uDimHi *= lpMemDimRht[uDim];
    } // End IF/ELSE

    // If the left arg is singleton, get its value
    if (IsSingleton (aplNELMLft))
    {
        // Get the integer or float value
        GetFirstValueToken (lptkLftArg,     // Ptr to left arg token
                           &aplIntegerLft,  // Ptr to integer result
                           &aplFloatLft,    // Ptr to float ...
                            NULL,           // Ptr to WCHAR ...
                            NULL,           // Ptr to longest ...
                           &aplNestLft,     // Ptr to lpSym/Glb ...
                            NULL,           // Ptr to ...immediate type ...
                            NULL);          // Ptr to array type ...
        // Attempt to convert FLOAT left arg
        if (IsSimpleFlt (aplTypeLft))
        {
            // Attempt to convert the float to an integer using System []CT
            aplIntegerLft = FloatToAplint_SCT (aplFloatLft, &bRet);
            if (!bRet)
                goto LEFT_DOMAIN_EXIT;
        } // End IF

        // Check the singleton arg
        if (!IsBooleanValue (aplIntegerLft))
            goto LEFT_DOMAIN_EXIT;

        // Save as the sum of the values in the left arg
        uDimLftSum = aplIntegerLft;
    } else
    // Non-singleton left arg
    {
        // Copy left arg to temp storage (all APLINTs),
        //   check the left arg for valid values, and
        //   accumulate the sum (as uDimLftSum).

        // Calculate space needed for the normalized left arg
        ByteRes = aplNELMLft * sizeof (APLINT);

        // Check for overflow
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;

        // Allocate temp storage for the normalized left arg
        hGlbRep = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (!hGlbRep)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        //   and check for empty (char) case
        if (!IsEmpty (aplNELMLft))
            lpMemRep = MyGlobalLock (hGlbRep);

        // Skip over the header to the data
        lpMemLft = VarArrayDataFmBase (lpMemLft);

        // Initialize the result axis dimension accumulator
        uDimLftSum = 0;

        // Split cases based upon the left arg's storage type
        switch (aplTypeLft)
        {
            case ARRAY_BOOL:
                uBitMask = BIT0;

                for (uDim = 0; uDim < aplNELMLft; uDim++)
                {
                    aplIntegerLft = (uBitMask & *(LPAPLBOOL) lpMemLft) ? TRUE : FALSE;
                    uDimLftSum += aplIntegerLft;
                    *lpMemRep++ = aplIntegerLft;

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
                for (uDim = 0; uDim < aplNELMLft; uDim++)
                {
                    aplIntegerLft = *((LPAPLINT) lpMemLft)++;
                    if (!IsBooleanValue (aplIntegerLft))
                        goto LEFT_DOMAIN_EXIT;
                    uDimLftSum += aplIntegerLft;

                    *lpMemRep++ = aplIntegerLft;
                } // End IF

                break;

            case ARRAY_FLOAT:
                for (uDim = 0; uDim < aplNELMLft; uDim++)
                {
                    // Attempt to convert the float to an integer using System []CT
                    aplIntegerLft = FloatToAplint_SCT (*((LPAPLFLOAT) lpMemLft)++, &bRet);
                    if (!bRet || !IsBooleanValue (aplIntegerLft))
                        goto LEFT_DOMAIN_EXIT;
                    uDimLftSum += aplIntegerLft;

                    *lpMemRep++ = aplIntegerLft;
                } // End FOR

                break;

            case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemLft)
                // Get the APA parameters
                apaOff = lpAPA->Off;
                apaMul = lpAPA->Mul;
#undef  lpAPA
                // Check the first and last values
                if (!IsBooleanValue (apaOff)
                 || !IsBooleanValue (apaOff + apaMul * (aplNELMLft - 1)))
                    goto LEFT_DOMAIN_EXIT;

                for (uDim = 0; uDim < aplNELMLft; uDim++)
                {
                    aplIntegerLft = apaOff + apaMul * uDim;
                    uDimLftSum += aplIntegerLft;
                    *lpMemRep++ = aplIntegerLft;
                } // End FOR

                break;

            case ARRAY_CHAR:
                break;

            case ARRAY_RAT:
                for (uDim = 0; uDim < aplNELMLft; uDim++)
                {
                    // Attempt to convert the RAT to an INT
                    aplIntegerLft = mpq_get_sctsx (((LPAPLRAT) lpMemLft)++, &bRet);
                    if (!bRet || !IsBooleanValue (aplIntegerLft))
                        goto LEFT_DOMAIN_EXIT;
                    uDimLftSum += aplIntegerLft;

                    *lpMemRep++ = aplIntegerLft;
                } // End IF

                break;

            case ARRAY_VFP:
                for (uDim = 0; uDim < aplNELMLft; uDim++)
                {
                    // Attempt to convert the VFP to an INT
                    aplIntegerLft = mpfr_get_sctsx (((LPAPLVFP) lpMemLft)++, &bRet);
                    if (!bRet || !IsBooleanValue (aplIntegerLft))
                        goto LEFT_DOMAIN_EXIT;
                    uDimLftSum += aplIntegerLft;

                    *lpMemRep++ = aplIntegerLft;
                } // End IF

                break;

            defstop
                break;
        } // End SWITCH

        // Restore lpMemRep to the start of the block
        //   and check for empty (char) case
        if (!IsEmpty (aplNELMLft))
        {
            MyGlobalUnlock (hGlbRep); lpMemRep = NULL;
            lpMemRep = MyGlobalLock (hGlbRep);
        } // End IF

        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

    // Check for LENGTH ERROR
    // Singleton right arg matches everything
    if (!IsSingleton (aplNELMRht)
     && uDimLftSum NE lpMemDimRht[aplAxis])
        goto LENGTH_EXIT;

    // Map APA right arg to INT result
    if (IsSimpleAPA (aplTypeRht))
        aplTypeRes = ARRAY_INT;
    else
        aplTypeRes = aplTypeRht;

    // Calculate the NELM of the result
    aplNELMRes = uDimLo * aplNELMLft * uDimHi;

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

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->bSelSpec   = lpMemHdrRht && lpMemHdrRht->bSelSpec;
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Copy the dimensions from the right arg to the result
    if (lpMemRht)
        CopyMemory (VarArrayBaseToDim (lpMemRes),
                    VarArrayBaseToDim (lpMemRht),
                    (APLU3264) aplRankRht * sizeof (APLDIM));

    // Fill in the axis dimension
    (VarArrayBaseToDim (lpMemRes))[aplAxis] = aplNELMLft;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemRes);
    if (lpMemRht)
        lpMemRht = VarArrayDataFmBase (lpMemRht);

    // Handle empty nested array results (prototypes)
    if (IsEmpty (aplNELMRht) && IsNested (aplTypeRht))
    {
        APLNELM aplNELM;

        // Empty nested results always have at least one element (the prototype)
        aplNELM = max (aplNELMRes, 1);

        for (uRes = 0; uRes < aplNELM; uRes++)
            *((LPAPLNESTED) lpMemRes)++ = CopySymGlbInd_PTB (lpMemRht);

        goto PROTO_EXIT;
    } // End IF

    if (IsSingleton (aplNELMLft))
        uLen = aplIntegerLft;
    // If the right arg is a singleton, get its value
    if (IsSingleton (aplNELMRht))
    {
        GetFirstValueToken (lptkRhtArg,     // Ptr to right arg token
                           &aplIntegerRep,  // Ptr to integer result
                           &aplFloatRep,    // Ptr to float ...
                           &aplCharRep,     // Ptr to WCHAR ...
                            NULL,           // Ptr to longest ...
                           &aplNestRep,     // Ptr to lpSym/Glb ...
                            NULL,           // Ptr to ...immediate type ...
                            NULL);          // Ptr to array type ...
        aplRatRep = (LPAPLRAT) aplNestRep;
        aplVfpRep = (LPAPLVFP) aplNestRep;
    } // End IF

    // Copy the data to the result

    // Split cases based upon the right arg's storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht  + uHi;
                uDimRes = uLo * uDimHi * aplNELMLft + uHi;
                for (uAcc = uAx = 0; uAx < aplNELMLft; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    if (!IsSingleton (aplNELMLft))
                        uLen = lpMemRep[uAx];
                    if (uLen)
                    {
                        uRes = uDimRes + uAx * uDimHi;
                        uBitIndex = (UINT) (uRes % NBIB);

                        if (!IsSingleton (aplNELMRht))
                        {
                            uRht = uDimRht + uAcc++ * uDimHi;
                            uBitMask  = BIT0 << (UINT) (uRht % NBIB);
                            aplIntegerRep = (uBitMask & ((LPAPLBOOL) lpMemRht)[uRht >> LOG2NBIB]) ? TRUE : FALSE;
                        } // End IF/ELSE

                        ((LPAPLBOOL) lpMemRes)[uRes >> LOG2NBIB] |= aplIntegerRep << uBitIndex;
////////////////////} else
////////////////////{
////////////////////    ((LPAPLBOOL) lpMemRes)[uRes >> LOG2NBIB] |= FALSE;  // Already filled in by GHND
                    } // End IF/ELSE
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_INT:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht  + uHi;
                uDimRes = uLo * uDimHi * aplNELMLft + uHi;
                for (uAcc = uAx = 0; uAx < aplNELMLft; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    if (!IsSingleton (aplNELMLft))
                        uLen = lpMemRep[uAx];
                    if (uLen)
                    {
                        if (!IsSingleton (aplNELMRht))
                            aplIntegerRep = ((LPAPLINT) lpMemRht)[uDimRht + uAcc++ * uDimHi];
                        ((LPAPLINT) lpMemRes)[uDimRes + uAx * uDimHi] = aplIntegerRep;
////////////////////} else
////////////////////{
////////////////////    ((LPAPLINT) lpMemRes)[uDimRes + uAx * uDimHi] = 0;  // Already filled in by GHND
                    } // End IF/ELSE
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_FLOAT:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht  + uHi;
                uDimRes = uLo * uDimHi * aplNELMLft + uHi;
                for (uAcc = uAx = 0; uAx < aplNELMLft; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    if (!IsSingleton (aplNELMLft))
                        uLen = lpMemRep[uAx];
                    if (uLen)
                    {
                        if (!IsSingleton (aplNELMRht))
                            aplFloatRep = ((LPAPLFLOAT) lpMemRht)[uDimRht + uAcc++ * uDimHi];
                        ((LPAPLFLOAT) lpMemRes)[uDimRes + uAx * uDimHi] = aplFloatRep;
////////////////////} else
////////////////////{
////////////////////    ((LPAPLFLOAT) lpMemRes)[uDimRes + uAx * uDimHi] = 0;    // Already filled in by GHND
                    } // End IF/ELSE
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_CHAR:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht  + uHi;
                uDimRes = uLo * uDimHi * aplNELMLft + uHi;
                for (uAcc = uAx = 0; uAx < aplNELMLft; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    if (!IsSingleton (aplNELMLft))
                        uLen = lpMemRep[uAx];
                    if (uLen)
                    {
                        if (!IsSingleton (aplNELMRht))
                            aplCharRep = ((LPAPLCHAR) lpMemRht)[uDimRht + uAcc++ * uDimHi];
                        ((LPAPLCHAR) lpMemRes)[uDimRes + uAx * uDimHi] = aplCharRep;
                    } else
                        ((LPAPLCHAR) lpMemRes)[uDimRes + uAx * uDimHi] = L' ';
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemRht)
            // Get the APA parameters
            apaOff = lpAPA->Off;
            apaMul = lpAPA->Mul;
#undef  lpAPA
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht  + uHi;
                uDimRes = uLo * uDimHi * aplNELMLft + uHi;
                for (uAcc = uAx = 0; uAx < aplNELMLft; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    if (!IsSingleton (aplNELMLft))
                        uLen = lpMemRep[uAx];
                    if (uLen)
                    {
                        if (!IsSingleton (aplNELMRht))
                            aplIntegerRep = apaOff + apaMul * (uDimRht + uAcc++ * uDimHi);
                        ((LPAPLINT) lpMemRes)[uDimRes + uAx * uDimHi] = aplIntegerRep;
////////////////////} else
////////////////////{
////////////////////    ((LPAPLINT) lpMemRes)[uDimRes + uAx * uDimHi] = 0;  // Already filled in by GHND
                    } // End IF/ELSE
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            aplNestProto =
              MakeMonPrototype_EM_PTB (*(LPAPLNESTED) lpMemRht, // Proto arg handle
                                       lptkFunc,                // Ptr to function token
                                       MP_CHARS);               // CHARs allowed
            if (!aplNestProto)
                goto ERROR_EXIT;

            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht  + uHi;
                uDimRes = uLo * uDimHi * aplNELMLft + uHi;
                for (uAcc = uAx = 0; uAx < aplNELMLft; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    if (!IsSingleton (aplNELMLft))
                        uLen = lpMemRep[uAx];
                    if (uLen)
                    {
                        if (!IsSingleton (aplNELMRht))
                            aplNestRep = ((LPAPLNESTED) lpMemRht)[uDimRht + uAcc++ * uDimHi];

                        ((LPAPLNESTED) lpMemRes)[uDimRes + uAx * uDimHi] = CopySymGlbDir_PTB (aplNestRep);
                    } else
                        ((LPAPLNESTED) lpMemRes)[uDimRes + uAx * uDimHi] = CopySymGlbDir_PTB (aplNestProto);
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_RAT:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht  + uHi;
                uDimRes = uLo * uDimHi * aplNELMLft + uHi;
                for (uAcc = uAx = 0; uAx < aplNELMLft; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    if (!IsSingleton (aplNELMLft))
                        uLen = lpMemRep[uAx];
                    if (uLen)
                    {
                        if (!IsSingleton (aplNELMRht))
                            aplRatRep = &((LPAPLRAT) lpMemRht)[uDimRht + uAcc++ * uDimHi];
                        mpq_init_set (&((LPAPLRAT) lpMemRes)[uDimRes + uAx * uDimHi], aplRatRep);
                    } else
                        mpq_init (&((LPAPLRAT) lpMemRes)[uDimRes + uAx * uDimHi]);
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_VFP:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht  + uHi;
                uDimRes = uLo * uDimHi * aplNELMLft + uHi;
                for (uAcc = uAx = 0; uAx < aplNELMLft; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    if (!IsSingleton (aplNELMLft))
                        uLen = lpMemRep[uAx];
                    if (uLen)
                    {
                        if (!IsSingleton (aplNELMRht))
                            aplVfpRep = &((LPAPLVFP) lpMemRht)[uDimRht + uAcc++ * uDimHi];
                        mpfr_init_copy (&((LPAPLVFP) lpMemRes)[uDimRes + uAx    * uDimHi], aplVfpRep);
                    } else
                        mpfr_init0 (&((LPAPLVFP) lpMemRes)[uDimRes + uAx * uDimHi]);
                } // End FOR
            } // End FOR/FOR

            break;

        defstop
            break;
    } // End SWITCH
PROTO_EXIT:
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
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
    // If the nested prototype is defined,
    //   and the prototype is a global memory handle, ...
    if (aplNestProto NE NULL
     && GetPtrTypeDir (aplNestProto) EQ PTRTYPE_HGLOBAL)
    {
        // We no longer need this storage
        FreeResultGlobalVar (aplNestProto); aplNestProto = NULL;
    } // End IF

    if (hGlbRep)
    {
        if (lpMemRep)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRep); lpMemRep = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbRep); hGlbRep = NULL;
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
} // End PrimFnDydSlope_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_slope.c
//***************************************************************************
