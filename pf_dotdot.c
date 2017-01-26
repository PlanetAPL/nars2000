//***************************************************************************
//  NARS2000 -- Primitive Function -- DotDot
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
#include <math.h>
#include "headers.h"


//***************************************************************************
//  $PrimFnDotDot_EM_YY
//
//  Primitive function for monadic and dyadic DotDot (ERROR and "sequence")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDotDot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDotDot_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_DOT);

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
        return PrimFnMonDotDot_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydDotDot_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimFnDotDot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnDotDot_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic DotDot
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnDotDot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnDotDot_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnDotDot_EM_YY,     // Ptr to primitive function routine
                                    lptkLftArg,             // Ptr to left arg token
                                    lptkFunc,               // Ptr to function token
                                    lptkRhtArg,             // Ptr to right arg token
                                    lptkAxis);              // Ptr to axis token (may be NULL)
} // End PrimProtoFnDotDot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonDotDot_EM_YY
//
//  Primitive function for monadic DotDot (ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonDotDot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonDotDot_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End PrimFnMonDotDot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydDotDot_EM_YY
//
//  Primitive function for dyadic DotDot ("sequence")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydDotDot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydDotDot_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeLft,           // Left arg storage type
                      aplTypeRht,           // Right ...
                      aplTypeRes;           // Result ...
    APLNELM           aplNELMLft,           // Left arg NELM
                      aplNELMRht,           // Right ...
                      aplNELMRes;           // Result ...
    APLRANK           aplRankLft,           // Left arg rank
                      aplRankRht;           // Right ...
    APLLONGEST        aplLongestLft,        // left arg immediate value
                      aplLongestRht;        // Right ...
    APLUINT           uCnt;                 // Loop counter
    APLRAT            aplRatTmp = {0};      // Temporary RAT
    APLVFP            aplVfpTmp = {0};      // Temporary VFP
    APLUINT           ByteRes;              // # bytes in the result
    HGLOBAL           hGlbLft = NULL,       // Left arg global memory handle
                      hGlbRht = NULL,       // Right ...
                      hGlbRes = NULL;       // Result ...
    LPVARARRAY_HEADER lpMemHdrLft = NULL,   // Ptr to left arg header
                      lpMemHdrRht = NULL,   // ...    right ...
                      lpMemHdrRes = NULL;   // ...    result   ...
    LPVOID            lpMemLft,             // Ptr to left arg global memory
                      lpMemRht,             // ...    right ...
                      lpMemRes;             // ...    result ...
    UBOOL             bRet = TRUE;          // TRUE iff the result is valid
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    ALLTYPES          atLft = {0},          // Left arg as ALLTYPES
                      atStp = {0},          // Step  ...
                      atRht = {0};          // Right ...

    // 2..7   == 2 3 4 5 6 7
    // 2 2..7 == 2 4 6

    // Get the attributes (Type)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Save as result storage type (must do before any errors)
    aplTypeRes = aTypePromote[aplTypeLft][aplTypeRht];

    // Check for RANK ERROR
    if (IsMultiRank (aplRankLft))
        goto LEFT_RANK_EXIT;

    if (IsMultiRank (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Check for LENGTH ERROR
    if (aplNELMLft NE 1
     && aplNELMLft NE 2)
        goto LEFT_LENGTH_EXIT;

    if (!IsSingleton (aplNELMRht))
        goto RIGHT_LENGTH_EXIT;

    // If either arg is nested, ...
    if (IsNested (aplTypeLft)
     || IsNested (aplTypeRht))
        return PrimFnDydDotDotNested_EM_YY (lptkLftArg, // Ptr to right arg token
                                            lptkFunc,   // Ptr to function token
                                            lptkRhtArg, // Ptr to right arg token
                                            lptkAxis);  // Ptr to axis token (may be NULL)
    // Get Left & right arg global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // If the left arg is a global, ...
    if (hGlbLft NE NULL)
    {
        // Lock the memory to get a ptr to it
        lpMemHdrLft = MyGlobalLockVar (hGlbLft);

        // Skip over the header and dimension
        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
    } else
        lpMemLft = &aplLongestLft;

    // If the right arg is a global, ...
    if (hGlbRht NE NULL)
    {
        // Lock the memory to get a ptr to it
        lpMemHdrRht = MyGlobalLockVar (hGlbRht);

        // Skip over the header and dimension
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    } else
        lpMemRht = &aplLongestRht;

    // Initialize temps if necessary
    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            aplTypeRes = ARRAY_APA;

            break;

        case ARRAY_FLOAT:
        case ARRAY_RAT:
        case ARRAY_VFP:
            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
        case ARRAY_ERROR:
            goto LEFT_DOMAIN_EXIT;

        defstop
            break;
    } // End SWITCH

    // Promote the left arg to the result type
    (*aTypeActConvert[aplTypeLft][(aplTypeRes EQ ARRAY_APA) ? ARRAY_INT : aplTypeRes])(lpMemLft, 0, &atLft);

    // If there's a step, ...
    if (aplNELMLft EQ 2)
        // Promote the step (in the left arg) to the result type
        (*aTypeActConvert[aplTypeLft][(aplTypeRes EQ ARRAY_APA) ? ARRAY_INT : aplTypeRes])(lpMemLft, 1, &atStp);
    else
        // Split cases based upon the result storage type
        switch (aplTypeRes)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
                atStp.aplInteger = 1;

                break;

            case ARRAY_FLOAT:
                atStp.aplFloat = 1;

                break;

            case ARRAY_RAT:
                mpq_init_set_ui (&atStp.aplRat, 1, 1);

                break;

            case ARRAY_VFP:
                mpfr_init_set_ui (&atStp.aplVfp, 1, MPFR_RNDN);

                break;

            defstop
                break;
        } // End SWITCH

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
        case ARRAY_FLOAT:
        case ARRAY_RAT:
        case ARRAY_VFP:
            // Promote the right arg to the result type
            (*aTypeActConvert[aplTypeRht][(aplTypeRes EQ ARRAY_APA) ? ARRAY_INT : aplTypeRes])(lpMemRht, 0, &atRht);

            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            goto RIGHT_DOMAIN_EXIT;

        defstop
            break;
    } // End SWITCH

    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        case ARRAY_APA:
            // Check for step 0
            if (atStp.aplInteger EQ 0)
            {
                if (atLft.aplInteger NE atRht.aplInteger)
                    goto LEFT_DOMAIN_EXIT;
                atStp.aplInteger = 1;
            } // End IF

            // Set the sign of the step
            if (atLft.aplInteger <= atRht.aplInteger)
                atStp.aplInteger =  abs64 (atStp.aplInteger);
            else
                atStp.aplInteger = -abs64 (atStp.aplInteger);
            // The NELM of the result is
            aplNELMRes = 1 + abs64 ((atRht.aplInteger - atLft.aplInteger) / atStp.aplInteger);

            break;

        case ARRAY_FLOAT:
            // Check for step 0
            if (atStp.aplFloat EQ 0)
            {
                if (atLft.aplFloat NE atRht.aplFloat)
                    goto LEFT_DOMAIN_EXIT;
                atStp.aplFloat = 1;
            } // End IF

            // Set the sign of the step
            if (atLft.aplFloat <= atRht.aplFloat)
                atStp.aplFloat =  fabs (atStp.aplFloat);
            else
                atStp.aplFloat = -fabs (atStp.aplFloat);
            // The NELM of the result is
            aplNELMRes = 1 + (APLINT) fabs ((atRht.aplFloat - atLft.aplFloat) / atStp.aplFloat);

            break;

        case ARRAY_RAT:
            // Check for step 0
            if (mpq_cmp_ui (&atStp.aplRat, 0, 1) EQ 0)
            {
                if (mpq_cmp (&atLft.aplRat, &atRht.aplRat) NE 0)
                    goto LEFT_DOMAIN_EXIT;
                mpq_set_ui (&atStp.aplRat, 1, 1);
            } // End IF

            // Set the sign of the step
            if (mpq_cmp (&atLft.aplRat, &atRht.aplRat) <= 0)
                mpq_abs (&atStp.aplRat, &atStp.aplRat);
            else
            {
                mpq_abs (&atStp.aplRat, &atStp.aplRat);
                mpq_neg (&atStp.aplRat, &atStp.aplRat);
            } // End IF/ELSE

            // Initialize to 0/1
            mpq_init (&aplRatTmp);

            // The NELM of the result is
            mpq_sub (&aplRatTmp, &atRht.aplRat, &atLft.aplRat);
            mpq_div (&aplRatTmp, &aplRatTmp, &atStp.aplRat);
            mpq_abs (&aplRatTmp, &aplRatTmp);
            aplNELMRes = 1 + mpq_get_sctsx (&aplRatTmp, NULL);      // Rounding to nearest integer within SysCT

            break;

        case ARRAY_VFP:
            // Check for step 0
            if (mpfr_cmp_ui (&atStp.aplVfp, 0) EQ 0)
            {
                if (mpfr_cmp (&atLft.aplVfp, &atRht.aplVfp) NE 0)
                    goto LEFT_DOMAIN_EXIT;
                mpfr_set_ui (&atStp.aplVfp, 1, MPFR_RNDN);
            } // End IF

            // Set the sign of the step
            if (mpfr_cmp (&atLft.aplVfp, &atRht.aplVfp) <= 0)
                mpfr_abs (&atStp.aplVfp, &atStp.aplVfp, MPFR_RNDN);
            else
            {
                mpfr_abs (&atStp.aplVfp, &atStp.aplVfp, MPFR_RNDN);
                mpfr_neg (&atStp.aplVfp, &atStp.aplVfp, MPFR_RNDN);
            } // End IF/ELSE

            // Initialize to 0
            mpfr_init0 (&aplVfpTmp);

            // The NELM of the result is
            mpfr_sub (&aplVfpTmp, &atRht.aplVfp, &atLft.aplVfp, MPFR_RNDN);
            mpfr_div (&aplVfpTmp, &aplVfpTmp, &atStp.aplVfp, MPFR_RNDN);
            mpfr_abs (&aplVfpTmp, &aplVfpTmp, MPFR_RNDN);
            aplNELMRes = 1 + mpfr_get_sctsx (&aplVfpTmp, NULL);     // Rounding to nearest integer within SysCT

            break;

        defstop
            break;
    } // End SWITCH

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, 1);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Now we can allocate the storage for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader        lpMemHdrRes
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Fill in the result's dimension
    *VarArrayBaseToDim (lpMemHdrRes) = aplNELMRes;

    // Skip over the header and dimension
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        case ARRAY_APA:
            // Save the APA offset and multiplier
            ((LPAPLAPA) lpMemRes)->Off = atLft.aplInteger;
            ((LPAPLAPA) lpMemRes)->Mul = atStp.aplInteger;

            break;

        case ARRAY_FLOAT:
            // Loop through the result items
            for (uCnt = 0; uCnt < aplNELMRes; uCnt++)
                *((LPAPLFLOAT) lpMemRes)++ = atLft.aplFloat + uCnt * atStp.aplFloat;

            break;

        case ARRAY_RAT:
            // Loop through the result items
            for (uCnt = 0; uCnt < aplNELMRes; uCnt++)
            {
                mpq_init_set (((LPAPLRAT) lpMemRes)++, &atLft.aplRat);
                mpq_add (&atLft.aplRat, &atLft.aplRat, &atStp.aplRat);
            } // End FOR

            break;

        case ARRAY_VFP:
            // Loop through the result items
            for (uCnt = 0; uCnt < aplNELMRes; uCnt++)
            {
                mpfr_init_set (((LPAPLVFP) lpMemRes)++, &atLft.aplVfp, MPFR_RNDN);
                mpfr_add (&atLft.aplVfp, &atLft.aplVfp, &atStp.aplVfp, MPFR_RNDN);
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH

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

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

LEFT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemHdrRes  = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes  = NULL;
    } // End IF

    if (hGlbLft NE NULL && lpMemHdrLft NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemHdrLft  = NULL;
    } // End IF

    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht  = NULL;
    } // End IF

    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
        case ARRAY_FLOAT:
        case ARRAY_CHAR:            // Error condition
        case ARRAY_HETERO:          // ...
        case ARRAY_NESTED:          // ...
            break;

        case ARRAY_RAT:
            // We no longer need these vars
            Myq_clear (&aplRatTmp);
            Myq_clear (&atRht.aplRat);
            Myq_clear (&atStp.aplRat);
            Myq_clear (&atLft.aplRat);

            break;

        case ARRAY_VFP:
            // We no longer need these vars
            Myf_clear (&aplVfpTmp);
            Myf_clear (&atRht.aplVfp);
            Myf_clear (&atStp.aplVfp);
            Myf_clear (&atLft.aplVfp);

            break;

        defstop
            break;
    } // End SWITCH

    return lpYYRes;
} // End PrimFnDydDotDot_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydDotDotNested_EM_YY
//
//  Dyadic DotDot extended to nested arrays
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydDotDotNested_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydDotDotNested_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (must be NULL)

{
    HGLOBAL hGlbMFO;                // Magic function/operator global memory handle

    Assert (lptkAxis EQ NULL);

    // Get the magic function/operator global memory handle
    hGlbMFO = GetMemPTD ()->hGlbMFO[MFOE_DydDotDot];

    //  Use an internal magic function/operator.
    return
      ExecuteMagicFunction_EM_YY (lptkLftArg,   // Ptr to left arg token
                                  lptkFunc,     // Ptr to function token
                                  NULL,         // Ptr to function strand
                                  lptkRhtArg,   // Ptr to right arg token
                                  lptkAxis,     // Ptr to axis token
                                  hGlbMFO,      // Magic function/operator global memory handle
                                  NULL,         // Ptr to HSHTAB struc (may be NULL)
                                  LINENUM_ONE); // Starting line # type (see LINE_NUMS)
} // End PrimFnDydDotDotNested_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  Magic function/operator for Extended Dyadic DotDot
//
//  Extended Dyadic DotDot -- Sequence Generator For Nested Arrays
//
//  This algorithm is a generalization of 2..5 proposed by John Scholes.
//***************************************************************************

#include "mf_dotdot.h"


//***************************************************************************
//  End of File: pf_dotdot.c
//***************************************************************************
