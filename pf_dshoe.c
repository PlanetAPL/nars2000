//***************************************************************************
//  NARS2000 -- Primitive Function -- DownShoe
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
//  $PrimFnDownShoe_EM_YY
//
//  Primitive function for monadic and dyadic DownShoe ("unique" and "union")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDownShoe_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDownShoe_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_DOWNSHOE);

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
        return PrimFnMonDownShoe_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydDownShoe_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimFnDownShoe_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnDownShoe_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic DownShoe
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnDownShoe_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnDownShoe_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnDownShoe_EM_YY,   // Ptr to primitive function routine
                                    lptkLftArg,             // Ptr to left arg token
                                    lptkFunc,               // Ptr to function token
                                    lptkRhtArg,             // Ptr to right arg token
                                    lptkAxis);              // Ptr to axis token (may be NULL)
} // End PrimProtoFnDownShoe_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimIdentFnDownShoe_EM_YY
//
//  Generate an identity element for the primitive function dyadic DownShoe
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimIdentFnDownShoe_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimIdentFnDownShoe_EM_YY
    (LPTOKEN lptkRhtOrig,           // Ptr to original right arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLRANK      aplRankRht;        // Right arg rank
    LPPL_YYSTYPE lpYYRes;           // Ptr to result

    // The right arg is the prototype item from
    //   the original empty arg.

    Assert (lptkRhtOrig NE NULL);
    Assert (lptkFunc    NE NULL);
    Assert (lptkRhtArg  NE NULL);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // The (left/right) identity function for dyadic DownShoe
    //   (L {downshoe} R) ("union") is
    //   {zilde}.

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, NULL, NULL, &aplRankRht, NULL);

    // Check for RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto DOMAIN_EXIT;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbZilde);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimIdentFnDownShoe_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonDownShoe_EM_YY
//
//  Primitive function for monadic DownShoe ("unique")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonDownShoe_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonDownShoe_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeRht,        // Right arg storage type
                 aplTypeRes;        // Result    ...
    APLNELM      aplNELMRht,        // Right arg NELM
                 aplNELMRes;        // Result    ...
    APLRANK      aplRankRht;        // Right arg rank
    APLLONGEST   aplLongestRht;     // Right arg immediate value
    APLUINT      ByteRes,           // # bytes in the result
                 uRht;              // Loop counter
    HGLOBAL      hGlbRht = NULL,    // Right arg global memory handle
                 hGlbRes = NULL,    // Result    ...
                 hGlbMFO,           // Magic function/operator global memory handle
                 hGlbGup = NULL,    // Grade-up global memory handle
                 hGlbTmp = NULL;    // Temporary ...
    LPAPLINT     lpMemGup = NULL,   // Ptr to grade-up global memory
                 lpMemTmp = NULL;   // ...    temporary ...
    LPVARARRAY_HEADER lpMemHdrRht;  // Ptr to right arg header
    LPVOID       lpMemRht = NULL,   // Ptr to right arg global memory
                 lpMemRes = NULL;   // Ptr to result    ...
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to result

    Assert (lptkAxis EQ NULL);

    // Get the right arg (Type, NELM, Rank)
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RIGHT RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RANK_EXIT;

    // Get right arg global ptr
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // If the right arg is not immediate, ...
    if (hGlbRht)
    {
        // Save a ptr to the right arg header
        lpMemHdrRht = lpMemRht;

        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemRht);
    } // End IF

    // If the right arg is a scalar or an APA with a zero multiplier, ...
    if (IsScalar (aplRankRht)
     || (IsSimpleAPA (aplTypeRht)
      && ((LPAPLAPA) lpMemRht)->Mul EQ 0))
    {
        // Return a one-element vector

        // If the right arg is an APA, the result is an integer
        if (IsSimpleAPA (aplTypeRht))
            aplTypeRes = ARRAY_INT;
        else
            aplTypeRes = aplTypeRht;

        // Calculate space needed for the result
        ByteRes = CalcArraySize (aplTypeRht, 1, 1);

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
////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
////////lpHeader->PV0        = FALSE;           // Already zero from GHND
////////lpHeader->PV1        = FALSE;           // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = 1;
        lpHeader->Rank       = 1;
#undef  lpHeader

        // Save the dimension in the result
        *VarArrayBaseToDim (lpMemRes) = 1;

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemRes);

        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
                *((LPAPLBOOL)  lpMemRes) = (APLBOOL) aplLongestRht;

                break;

            case ARRAY_INT:
            case ARRAY_APA:
                *((LPAPLINT)   lpMemRes) = aplLongestRht;

                break;

            case ARRAY_FLOAT:
                *((LPAPLFLOAT) lpMemRes) = *(LPAPLFLOAT) &aplLongestRht;

                break;

            case ARRAY_CHAR:
                *((LPAPLCHAR)  lpMemRes) = (APLCHAR) aplLongestRht;

                break;

            case ARRAY_RAT:
                mpq_init_set ((LPAPLRAT) lpMemRes, (LPAPLRAT) lpMemRht);

                break;

            case ARRAY_VFP:
                mpfr_init_set ((LPAPLVFP) lpMemRes, (LPAPLVFP) lpMemRht, MPFR_RNDN);

                break;

            case ARRAY_NESTED:
                *((LPAPLNESTED) lpMemRes) = CopySymGlbInd_PTB (lpMemRht);

                break;

            case ARRAY_HETERO:      // No such thing as a salar hetero array
            defstop
                break;
        } // End SWITCH

        goto YYALLOC_EXIT;
    } else
    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        {
            APLNELM aplNELMRnd;         // Right arg NELM rounded up to bits in byte
            APLBOOL aplComp,            // Comparison value (0x00 or 0xFF)
                    aplLastByte,        // Last byte
                    aplMask;            // Mask for testing the extra bits in the last byte

            // The first element of the right arg is in aplLongestRht

            // Get the # bytes in the right arg
            aplNELMRnd = RoundUpBitsToBytes (aplNELMRht);

            // Get the comparison value
            // If we're searching for a 1, compare using NE against 0x00;
            // ...                      0, ...                      0xFF
            aplComp = (aplLongestRht EQ 0) ? 0x00 : 0xFF;

            // Search for the complement of the first value
            for (uRht = 0; uRht < aplNELMRnd; uRht++)
            if (aplComp NE (aplLastByte = ((LPAPLBOOL) lpMemRht)[uRht]))
                break;

            // Initialize the result NELM
            aplNELMRes = (aplNELMRht NE 0);

            // Get the # extra bits (0 to 7)
            aplMask = (MASKLOG2NBIB & (UINT) aplNELMRht);

            // Check for the last byte
            if (uRht EQ (aplNELMRnd - 1)
             && aplMask)
            {
                // Get a mask to isolate the active bits in the last byte
                //   0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F
                aplMask = (BIT0 << aplMask) - 1;

                if ((aplComp & aplMask) NE (aplLastByte & aplMask))
                    aplNELMRes++;
            } else
            if (uRht < aplNELMRnd)
                aplNELMRes++;

            // If there's more than one element in the result, ...
            if (aplNELMRes > 1
             && aplLongestRht EQ 0)
                aplLongestRht ^= BIT1;

            // Calculate space needed for the result
            ByteRes = CalcArraySize (aplTypeRht, aplNELMRes, 1);

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
            lpHeader->ArrType    = ARRAY_BOOL;
////////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
////////////lpHeader->PV0        = FALSE;           // Already zero from GHND
////////////lpHeader->PV1        = FALSE;           // Already zero from GHND
            lpHeader->RefCnt     = 1;
            lpHeader->NELM       = aplNELMRes;
            lpHeader->Rank       = 1;
#undef  lpHeader

            // Save the dimension in the result
            *VarArrayBaseToDim (lpMemRes) = aplNELMRes;

            // Skip over the header and dimensions to the data
            lpMemRes = VarArrayDataFmBase (lpMemRes);

            if (aplNELMRes)
                *((LPAPLBOOL) lpMemRes) = (APLBOOL) aplLongestRht;
            break;
        } // End ARRAY_BOOL

        case ARRAY_INT:
            // If the right arg is a PermVec, return it
            if (IsPermVector (lpMemHdrRht))
            {
                hGlbRes = CopySymGlbDirAsGlb (hGlbRht);

                break;
            } // End IF

            // Otherwise fall through ...

        case ARRAY_FLOAT:
        case ARRAY_CHAR:
        case ARRAY_RAT:
        case ARRAY_VFP:
        {
            TOKEN    tkFunc = {0},      // Grade-up function token
                     tkRht  = {0};      // Right arg token
            APLBOOL  bQuadIO;           // []IO
            APLFLOAT fQuadCT;           // []CT
            APLINT   aplLastInt;        // The last int   we found
            APLFLOAT aplLastFlt;        // ...      float ...
            APLCHAR  aplLastChr;        // ...      char  ...
            APLRAT   aplLastRat;        // ...      RAT   ...
            APLVFP   aplLastVfp;        // ...      VFP   ...

            // Setup the grade-up function token
            tkFunc.tkFlags.TknType   = TKT_FCNIMMED;
            tkFunc.tkFlags.ImmType   = IMMTYPE_PRIMFCN;
////////////tkFunc.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
            tkFunc.tkData.tkChar     = UTF16_DELTASTILE;
            tkFunc.tkCharIndex       = lptkFunc->tkCharIndex;

            // Get the current value of []CT
            fQuadCT = GetQuadCT ();

            // Save the current index origin and set it to zero
            bQuadIO = GetQuadIO ();
            SetQuadIO (0);

            //***************************************************************
            // In order to find unique values, we grade-up the right arg and
            //   loop through the grade-up indices looking for different
            //   values in the right arg.
            //***************************************************************

            // Grade-up the right arg
            lpYYRes =
              PrimFnMonGradeCommon_EM_YY (&tkFunc,      // Ptr to function token
                                           lptkRhtArg,  // Ptr to right arg token
                                           NULL,        // Ptr to axis token (may be NULL)
                                           TRUE);       // TRUE iff we're to treat the right arg as ravelled
            // Restore the index origin
            SetQuadIO (bQuadIO);

            if (lpYYRes EQ NULL)
                goto ERROR_EXIT;
            // Get the grade-up global memory handle
            hGlbGup = lpYYRes->tkToken.tkData.tkGlbData;

            // Free the YYRes
            YYFree (lpYYRes); lpYYRes = NULL;

            // Lock the memory to get a ptr to it
            lpMemGup = MyGlobalLockVar (hGlbGup);

            // Skip over the header and dimensions to the data
            lpMemGup = VarArrayDataFmBase (lpMemGup);

            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_INT:
                    // Initialize the last value with the first one
                    if (aplNELMRht)
                        aplLastInt = ((LPAPLINT)   lpMemRht)[*lpMemGup];

                    // Trundle through the grade-up of the right arg
                    //   counting unique values and saving their indices
                    for (uRht = 1, aplNELMRes = (aplNELMRht > 0);
                         uRht < aplNELMRht;
                         uRht++)
                    // Check for a different (and thus unique) value
                    if (aplLastInt NE ((LPAPLINT) lpMemRht)[lpMemGup[uRht]])
                    {
                        // Save as the next unique value's index
                        lpMemGup[aplNELMRes++] = lpMemGup[uRht];

                        // Save as the next unique value
                        aplLastInt = ((LPAPLINT) lpMemRht)[lpMemGup[uRht]];
                    } // End FOR/IF

                    break;

                case ARRAY_FLOAT:
                    // Initialize the last value with the first one
                    if (aplNELMRht)
                        aplLastFlt = ((LPAPLFLOAT) lpMemRht)[*lpMemGup];

                    // Trundle through the grade-up of the right arg
                    //   counting unique values and saving their indices
                    for (uRht = 1, aplNELMRes = (aplNELMRht > 0);
                         uRht < aplNELMRht;
                         uRht++)
                    // Check for a different (and thus unique) value
                    if (!CompareCT (aplLastFlt, ((LPAPLFLOAT) lpMemRht)[lpMemGup[uRht]], fQuadCT, NULL))
                    {
                        // Save as the next unique value's index
                        lpMemGup[aplNELMRes++] = lpMemGup[uRht];

                        // Save as the next unique value
                        aplLastFlt = ((LPAPLFLOAT) lpMemRht)[lpMemGup[uRht]];
                    } // End FOR/IF

                    break;

                case ARRAY_CHAR:
                    // Initialize the last value with the first one
                    if (aplNELMRht)
                        aplLastChr = ((LPAPLCHAR)  lpMemRht)[*lpMemGup];

                    // Trundle through the grade-up of the right arg
                    //   counting unique values and saving their indices
                    for (uRht = 1, aplNELMRes = (aplNELMRht > 0);
                         uRht < aplNELMRht;
                         uRht++)
                    // Check for a different (and thus unique) value
                    if (aplLastChr NE ((LPAPLCHAR) lpMemRht)[lpMemGup[uRht]])
                    {
                        // Save as the next unique value's index
                        lpMemGup[aplNELMRes++] = lpMemGup[uRht];

                        // Save as the next unique value
                        aplLastChr = ((LPAPLCHAR) lpMemRht)[lpMemGup[uRht]];
                    } // End FOR/IF

                    break;

                case ARRAY_RAT:
                    // Initialize the last value with the first one
                    if (aplNELMRht)
                        aplLastRat = ((LPAPLRAT)   lpMemRht)[*lpMemGup];

                    // Trundle through the grade-up of the right arg
                    //   counting unique values and saving their indices
                    for (uRht = 1, aplNELMRes = (aplNELMRht > 0);
                         uRht < aplNELMRht;
                         uRht++)
                    // Check for a different (and thus unique) value
                    if (mpq_cmp (&aplLastRat, &((LPAPLRAT) lpMemRht)[lpMemGup[uRht]]) NE 0)
                    {
                        // Save as the next unique value's index
                        lpMemGup[aplNELMRes++] = lpMemGup[uRht];

                        // Save as the next unique value
                        aplLastRat = ((LPAPLRAT) lpMemRht)[lpMemGup[uRht]];
                    } // End FOR/IF

                    break;

                case ARRAY_VFP:
                    // Initialize the last value with the first one
                    if (aplNELMRht)
                        aplLastVfp = ((LPAPLVFP)   lpMemRht)[*lpMemGup];

                    // Trundle through the grade-up of the right arg
                    //   counting unique values and saving their indices
                    for (uRht = 1, aplNELMRes = (aplNELMRht > 0);
                         uRht < aplNELMRht;
                         uRht++)
                    // Check for a different (and thus unique) value
                    if (mpfr_cmp_ct (aplLastVfp, ((LPAPLVFP) lpMemRht)[lpMemGup[uRht]], fQuadCT) NE 0)
                    {
                        // Save as the next unique value's index
                        lpMemGup[aplNELMRes++] = lpMemGup[uRht];

                        // Save as the next unique value
                        aplLastVfp = ((LPAPLVFP) lpMemRht)[lpMemGup[uRht]];
                    } // End FOR/IF

                    break;

                defstop
                    break;
            } // End IF/SWITCH

            //***************************************************************
            // In order to preserve the order of the unique elements
            //   we need to grade-up the indices of the unique values so we
            //   can place them into the result in the same order as
            //   they occur in the right arg.
            //***************************************************************

            // Calculate space needed for a temp
            ByteRes = CalcArraySize (ARRAY_INT, aplNELMRes, 1);

            // Check for overflow
            if (ByteRes NE (APLU3264) ByteRes)
                goto WSFULL_EXIT;

            // Allocate space for the temp
            hGlbTmp = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
            if (hGlbTmp EQ NULL)
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpMemTmp = MyGlobalLock000 (hGlbTmp);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemTmp)
            // Fill in the header
            lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
            lpHeader->ArrType    = ARRAY_INT;
////////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
////////////lpHeader->PV0        = FALSE;           // Already zero from GHND
////////////lpHeader->PV1        = FALSE;           // Already zero from GHND
            lpHeader->RefCnt     = 1;
            lpHeader->NELM       = aplNELMRes;
            lpHeader->Rank       = 1;
#undef  lpHeader
            // Save the dimension in the result
            *VarArrayBaseToDim (lpMemTmp) = aplNELMRes;

            // Skip over the header and dimensions to the data
            lpMemTmp = VarArrayDataFmBase (lpMemTmp);

            // Copy the data from lpMemGup to lpMemTmp
            CopyMemory (lpMemTmp, lpMemGup, (APLU3264) aplNELMRes * sizeof (APLINT));

            // We no longer need this ptr
            MyGlobalUnlock (hGlbGup); lpMemGup = NULL;

            // We no longer need this storage
            FreeResultGlobalVar (hGlbGup); hGlbGup = NULL;

            // Setup the right arg token
            tkRht.tkFlags.TknType   = TKT_VARARRAY;
////////////tkRht.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from = {0}
////////////tkRht.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
            tkRht.tkData.tkGlbData  = MakePtrTypeGlb (hGlbTmp);
            tkRht.tkCharIndex       = lptkFunc->tkCharIndex;

            // Save the current index origin and set it to zero
////////////bQuadIO = GetQuadIO ();     // Already read
            SetQuadIO (0);

            // Grade-up the graded-up right arg
            lpYYRes =
              PrimFnMonGradeCommon_EM_YY (&tkFunc,      // Ptr to function token
                                          &tkRht,       // Ptr to right arg token
                                           NULL,        // Ptr to axis token (may be NULL)
                                           TRUE);       // TRUE iff we're to treat the right arg as ravelled
            // Restore the index origin
            SetQuadIO (bQuadIO);

            if (lpYYRes EQ NULL)
                goto ERROR_EXIT;
            // Get the grade-up global memory handle
            hGlbGup = lpYYRes->tkToken.tkData.tkGlbData;

            // Free the YYRes
            YYFree (lpYYRes); lpYYRes = NULL;

            // Lock the memory to get a ptr to it
            lpMemGup = MyGlobalLockVar (hGlbGup);

            // Skip over the header and dimensions to the data
            lpMemGup = VarArrayDataFmBase (lpMemGup);

            // Calculate space needed for the result
            ByteRes = CalcArraySize (aplTypeRht, aplNELMRes, 1);

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
            lpHeader->ArrType    = aplTypeRht;
////////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
////////////lpHeader->PV0        = FALSE;           // Already zero from GHND
////////////lpHeader->PV1        = FALSE;           // Already zero from GHND
            lpHeader->RefCnt     = 1;
            lpHeader->NELM       = aplNELMRes;
            lpHeader->Rank       = 1;
#undef  lpHeader
            // Save the dimension in the result
            *VarArrayBaseToDim (lpMemRes) = aplNELMRes;

            // Skip over the header and dimensions to the data
            lpMemRes = VarArrayDataFmBase (lpMemRes);

            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_INT:
                    // Trundle through the right arg via the grade-up
                    //   copying elements to the result
                    for (uRht = 0; uRht < aplNELMRes; uRht++)
                        *((LPAPLINT)   lpMemRes)++ = ((LPAPLINT)   lpMemRht)[lpMemTmp[*lpMemGup++]];
                    break;

                case ARRAY_FLOAT:
                    // Trundle through the right arg via the grade-up
                    //   copying elements to the result
                    for (uRht = 0; uRht < aplNELMRes; uRht++)
                        *((LPAPLFLOAT) lpMemRes)++ = ((LPAPLFLOAT) lpMemRht)[lpMemTmp[*lpMemGup++]];
                    break;

                case ARRAY_CHAR:
                    // Trundle through the right arg via the grade-up
                    //   copying elements to the result
                    for (uRht = 0; uRht < aplNELMRes; uRht++)
                        *((LPAPLCHAR)  lpMemRes)++ = ((LPAPLCHAR)  lpMemRht)[lpMemTmp[*lpMemGup++]];
                    break;

                case ARRAY_RAT:
                    // Trundle through the right arg via the grade-up
                    //   copying elements to the result
                    for (uRht = 0; uRht < aplNELMRes; uRht++)
                        mpq_init_set (((LPAPLRAT)   lpMemRes)++, &((LPAPLRAT)   lpMemRht)[lpMemTmp[*lpMemGup++]]);
                    break;

                case ARRAY_VFP:
                    // Trundle through the right arg via the grade-up
                    //   copying elements to the result
                    for (uRht = 0; uRht < aplNELMRes; uRht++)
                        mpfr_init_copy (((LPAPLVFP)   lpMemRes)++, &((LPAPLVFP)   lpMemRht)[lpMemTmp[*lpMemGup++]]);
                    break;

                defstop
                    break;
            } // End SWITCH

            // We no longer need this ptr
            MyGlobalUnlock (hGlbTmp); lpMemTmp = NULL;

            break;
        } // End ARRAY_INT/ARRAY_FLOAT/ARRAY_CHAR

        case ARRAY_APA:
            // The APA multiplier is non-zero, so return the right arg
            hGlbRes = CopySymGlbDirAsGlb (hGlbRht);

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Get ptr to PerTabData global memory
            lpMemPTD = GetMemPTD ();

            // Get the magic function/operator global memory handle
            hGlbMFO = lpMemPTD->hGlbMFO[MFOE_MonDnShoe];

            //  Return the unique elements in the right arg.
            //  Use an internal magic function/operator.
            lpYYRes =
              ExecuteMagicFunction_EM_YY (NULL,         // Ptr to left arg token
                                          lptkFunc,     // Ptr to function token
                                          NULL,         // Ptr to function strand
                                          lptkRhtArg,   // Ptr to right arg token
                                          lptkAxis,     // Ptr to axis token
                                          hGlbMFO,      // Magic function/operator global memory handle
                                          NULL,         // Ptr to HSHTAB struc (may be NULL)
                                          LINENUM_ONE); // Starting line # type (see LINE_NUMS)
            break;

        defstop
            break;
    } // End IF/ELSE/SWITCH

    // If there's no result global memory handle, ...
    //   (we executed a magic function/operator)
    if (hGlbRes EQ NULL)
        goto NORMAL_EXIT;

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
    if (hGlbTmp)
    {
        if (lpMemTmp)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbTmp); lpMemTmp = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalVar (hGlbTmp); hGlbTmp = NULL;
    } // End IF

    if (hGlbGup)
    {
        if (lpMemGup)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbGup); lpMemGup = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalVar (hGlbGup); hGlbGup = NULL;
    } // End IF

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

    return lpYYRes;
} // End PrimFnMonDownShoe_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  Magic function/operator for monadic DownShoe
//
//  Monadic DownShoe -- Unique
//
//  On scalar or vector right args, return the unique values
//***************************************************************************

//***************************************************************************
//  Magic function for dyadic DownShoe
//
//  Dyadic Down Shoe -- Union
//
//  Return the elements in L or in R.
//***************************************************************************

#include "mf_dshoe.h"


//***************************************************************************
//  $PrimFnDydDownShoe_EM_YY
//
//  Primitive function for dyadic DownShoe (Union)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydDownShoe_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydDownShoe_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLRANK      aplRankLft,        // Left arg rank
                 aplRankRht;        // Right ...
    HGLOBAL      hGlbMFO;           // Magic function/operator global memory handle
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc APPEND_NAME_ARG);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, NULL, NULL, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, NULL, NULL, &aplRankRht, NULL);

    // Check for LEFT/RIGHT RANK ERRORs
    if (IsMultiRank (aplRankLft))
        goto LEFT_RANK_EXIT;

    if (IsMultiRank (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the magic function/operator global memory handle
    hGlbMFO = lpMemPTD->hGlbMFO[MFOE_DydDnShoe];

    //  Return the elements in L or in R.
    //  Use an internal magic function.
    lpYYRes =
      ExecuteMagicFunction_EM_YY (lptkLftArg,   // Ptr to left arg token
                                  lptkFunc,     // Ptr to function token
                                  NULL,         // Ptr to function strand
                                  lptkRhtArg,   // Ptr to right arg token
                                  lptkAxis,     // Ptr to axis token
                                  hGlbMFO,      // Magic function/operator global memory handle
                                  NULL,         // Ptr to HSHTAB struc (may be NULL)
                                  LINENUM_ONE); // Starting line # type (see LINE_NUMS)
    goto NORMAL_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    return NULL;

ERROR_EXIT:
NORMAL_EXIT:
    return lpYYRes;
} // End PrimFnDydDownShoe_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_dshoe.c
//***************************************************************************
