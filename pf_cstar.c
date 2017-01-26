//***************************************************************************
//  NARS2000 -- Primitive Function -- CircleStar
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

////#define OWN_EXPLOG

#define STRICT
#include <windows.h>
#include <math.h>
#include "headers.h"


#ifndef PROTO
PRIMSPEC PrimSpecCircleStar =
{
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecCircleStarStorageTypeMon,
    NULL,   // &PrimFnMonCircleStarAPA_EM, -- Can't happen w/CircleStar

    NULL,   // &PrimFnMonCircleStarBisB, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarBisI, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarBisF, -- Can't happen w/CircleStar

////               IisB,     // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonCircleStarIisI, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarIisF, -- Can't happen w/CircleStar

////               FisB,     // Handled via type promotion (to FisI)
    &PrimFnMonCircleStarFisI,
    &PrimFnMonCircleStarFisF,

    NULL,   // &PrimFnMonCircleStarRisR, -- Can't happen w/CircleStar

////               VisR,    // Handled via type promotion (to VisV)
    &PrimFnMonCircleStarVisV,

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecCircleStarStorageTypeDyd,
    NULL,   // &PrimFnDydCircleStarAPA_EM, -- Can't happen w/CircleStar

    &PrimFnDydRightCaretUnderbarBisBvB,
    NULL,   // &PrimFnDydCircleStarBisIvI, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarBisFvF, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarBisCvC, -- Can't happen w/CircleStar

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydCircleStarIisIvI, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarIisFvF, -- Can't happen w/CircleStar

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    &PrimFnDydCircleStarFisIvI,
    &PrimFnDydCircleStarFisFvF,

    NULL,   // &PrimFnDydCircleStarBisRvR, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarRisRvR, -- Can't happen w/CircleStar

    NULL,   // &PrimFnDydCircleStarBisVvV, -- Can't happen w/CircleStar
////                 VisRvR,    // Handled via type promotion (to VisVvV)
    &PrimFnDydCircleStarVisVvV,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecCircleStar};
#endif

IC_INDICES icndxLog[2][2] =
{{ICNDX_0LOG0, ICNDX_0LOG1},
 {ICNDX_1LOG0, ICNDX_1LOG1}};


//***************************************************************************
//  $PrimFnCircleStar_EM_YY
//
//  Primitive function for monadic and dyadic CircleStar
//     ("natural logarithm" and "logarithm")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnCircleStar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnCircleStar_EM_YY
    (LPTOKEN       lptkLftArg,      // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN       lptkFunc,        // Ptr to function token
     LPTOKEN       lptkRhtArg,      // Ptr to right arg token
     LPTOKEN       lptkAxis)        // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_CIRCLESTAR);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnCircleStar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecCircleStarStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecCircleStarStorageTypeMon
    (APLNELM    aplNELMRht,
     LPAPLSTYPE lpaplTypeRht,
     LPTOKEN    lptkFunc)

{
    APLSTYPE aplTypeRes;

    // In case the right arg is an empty char,
    //   change its type to BOOL
    if (IsCharEmpty (*lpaplTypeRht, aplNELMRht))
        *lpaplTypeRht = ARRAY_BOOL;

    // The storage type of the result is
    //   the same as that of the right arg
    aplTypeRes = *lpaplTypeRht;

    // Split cases based upon the storage type
    switch (aplTypeRes)
    {
        // Except that BOOL, INT and APA become FLOAT
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_APA:
            aplTypeRes = ARRAY_FLOAT;

            break;

        // Except that RAT becomes VFP
        case ARRAY_RAT:
            aplTypeRes = ARRAY_VFP;

            break;

        case ARRAY_VFP:
        case ARRAY_NESTED:
            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
            aplTypeRes = ARRAY_ERROR;

            break;

        defstop
            break;
    } // End SWITCH

    return aplTypeRes;
} // End PrimSpecCircleStarStorageTypeMon


//***************************************************************************
//  $PrimFnMonCircleStarFisI
//
//  Primitive scalar function monadic CircleStar:  F {is} fn I
//***************************************************************************

APLFLOAT PrimFnMonCircleStarFisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for Complex result
    if (SIGN_APLFLOAT (aplIntegerRht))
        RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

    // Check for indeterminates:  {log} 0
    if (aplIntegerRht EQ 0)
        return TranslateQuadICIndex (0,
                                     ICNDX_LOG0,
                          (APLFLOAT) aplIntegerRht,
                                     FALSE);
    return log ((APLFLOAT) aplIntegerRht);
} // End PrimFnMonCircleStarFisI


//***************************************************************************
//  $PrimFnMonCircleStarFisF
//
//  Primitive scalar function monadic CircleStar:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonCircleStarFisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for Complex result
    if (aplFloatRht < 0)        // Not SIGN_APLFLOAT as that'll catch -0 whose log is indeterminate
        RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

    // Check for indeterminates:  {log} 0
    if (aplFloatRht EQ 0)
        return TranslateQuadICIndex (0,
                                     ICNDX_LOG0,
                                     aplFloatRht,
                                     FALSE);
    // Check for special cases:  {log} _
    if (IsFltPosInfinity (aplFloatRht))
        return fltPosInfinity;

    // Check for special cases:  {log} -_
    if (IsFltNegInfinity (aplFloatRht))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    return log (aplFloatRht);
} // End PrimFnMonCircleStarFisF


//***************************************************************************
//  $PrimFnMonCircleStarVisV
//
//  Primitive scalar function monadic CircleStar:  V {is} fn V
//
//  See "http://www.jsoftware.com/jwiki/Essays/Extended%20Precision%20Functions"
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonCircleStarVisV"
#else
#define APPEND_NAME
#endif

APLVFP PrimFnMonCircleStarVisV
    (APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
#if OWN_EXPLOG
    APLMPI mpzTmp = {0};
    APLVFP mpfRes = {0},
           mpfTmp = {0},
           mpfLn2;
    signed long log2x;
    int    log2xSign;
#else
    APLVFP  mpfRes  = {0};
#endif
    // Check for Complex result
    if (mpfr_sgn (&aplVfpRht) < 0)  // Not SIGN_APLVFP as that'll catch -0 whose log is indeterminate
        RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

    // Check for indeterminates:  {log} 0
    if (IsMpf0 (&aplVfpRht))
        return *mpfr_QuadICValue (&aplVfpRht,       // No left arg
                                   ICNDX_LOG0,
                                  &aplVfpRht,
                                  &mpfRes,
                                   FALSE);
    // Check for special cases:  {log} _
    if (IsMpfPosInfinity (&aplVfpRht))
        return mpfPosInfinity;

    // Check for special cases:  {log} -_
    if (IsMpfNegInfinity (&aplVfpRht))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

#if OWN_EXPLOG
    // Initialize the result
    mpfr_init_copy (&mpfRes, &aplVfpRht);

    // First, represent V as r * 2 ^ m where m is an integer
    //   and r is between 1/sqrt (2) and sqrt (2).
    // This means that log (V) = (log (r)) + m * log (2)
////log2x = (int) (floor (0.5 + (log (mpfr_get_d (&aplVfpRht)) / log (2.0))));

    // Initialize temps to 0/1 and 0
    mpz_init (&mpzTmp);
    mpfr_init0 (&mpfTmp);

    if (mpfr_cmp (&mpfRes, &mpfSqrt2) > 0)
    {
        mpfr_div (&mpfTmp, &mpfRes, &mpfSqrt2, MPFR_RNDN);
        mpfr_floor (&mpfTmp, &mpfTmp);
        mpz_set_fr (&mpzTmp, &mpfTmp);
        log2x = (int) mpz_sizeinbase (&mpzTmp, 2);
        mpfr_div_2exp (&mpfRes, &mpfRes, log2x, MPFR_RNDN);
        log2xSign =  1;
    } else
    if (mpfr_cmp (&mpfInvSqrt2, &mpfRes) > 0)
    {
        mpfr_div (&mpfTmp, &mpfInvSqrt2, &mpfRes, MPFR_RNDN);
        mpfr_floor (&mpfTmp, &mpfTmp);
        mpz_set_fr (&mpzTmp, &mpfTmp);
        log2x = (int) mpz_sizeinbase (&mpzTmp, 2);
        mpfr_mul_2exp (&mpfRes, &mpfRes, log2x, MPFR_RNDN);
        log2xSign = -1;
    } else
        log2x = log2xSign = 0;

    Myf_clear (&mpfTmp);
    Myz_clear (&mpzTmp);

    // Check the scale as the above calculation of log2x can be off by one
    while (mpfr_cmp (&mpfRes, &mpfSqrt2) > 0)
    {
        mpfr_div_ui (&mpfRes, &mpfRes, 2);
        log2x++;
    } // End WHILE

    while (mpfr_cmp (&mpfInvSqrt2, &mpfRes) > 0)
    {
        mpfr_mul_ui (&mpfRes, &mpfRes, 2, MPFR_RNDN);
        log2x++;
    } // End WHILE

    // Calculate the log of mpfRes via the power series 4.1.27 in Abramowitz & Stegun
    mpfTmp = LogVfp (mpfRes);

    // Copy to the result
    mpfr_copy (&mpfRes, &mpfTmp);

    // Calculate the log of 2 ...
    mpfr_set_ui (&mpfTmp, 2);
    mpfLn2 = LogVfp (mpfTmp);

    // Finally, convert the result back to normal
    mpfr_mul_ui (&mpfTmp, &mpfLn2, log2x);
    if (log2xSign < 0)
        mpfr_neg (&mpfTmp, &mpfTmp);
    mpfr_add    (&mpfRes, &mpfRes, &mpfTmp, MPFR_RNDN);

    // We no longer need this storage
    Myf_clear (&mpfTmp);
    Myf_clear (&mpfLn2);

    return mpfRes;
#else
    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Calculate the function
    mpfr_log   (&mpfRes, &aplVfpRht, MPFR_RNDN);

    return mpfRes;
#endif
} // End PrimFnMonCircleStarVisV
#undef  APPEND_NAME


#ifdef OWN_EXPLOG
//***************************************************************************
//  $LogVfp
//
//  Calculate the natural log of a VFP where
//       V is between 1/sqrt (2) and sqrt (2)
//
//  See "http://www.jsoftware.com/jwiki/Essays/Extended%20Precision%20Functions"
//***************************************************************************

APLVFP LogVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes   = {0},      // MPFR result
           mpfTmp1  = {0},      // ...  temp
           mpfTmp2  = {0},      // ...
           mpfBase  = {0};      // ...  base
    UINT   uRes;                // Loop counter

    // Initialize the temps to 0
    mpfr_init0 (&mpfRes);
    mpfr_init0 (&mpfTmp1);
    mpfr_init0 (&mpfTmp2);
    mpfr_init0 (&mpfBase);

    // ln z = 2 * [(1/1)*(z-1)/(z+1) + (1/3)*((z-1)/z+1))^3 + (1/5)*...]

    // Calculate Base:  (z-1)/(z+1)
    mpfr_sub_ui (&mpfTmp1, &aplVfpRht, 1);
    mpfr_add_ui (&mpfTmp2, &aplVfpRht, 1);
    mpfr_div    (&mpfBase, &mpfTmp1, &mpfTmp2);

    // Calculate the multiplier:  Base^2
    mpfr_mul    (&mpfTmp2, &mpfBase, &mpfBase);

    // Loop through the # terms
    for (uRes = 0 ; uRes < nDigitsFPC; uRes++)
    {
        // Divide the base by (2 * uRes) + 1
        mpfr_div_ui (&mpfTmp1, &mpfBase, 2 * uRes + 1);

        // Accumulate into the result
        mpfr_add (&mpfRes, &mpfRes, &mpfTmp1);

        // Multiply the base by the multiplier
        mpfr_mul (&mpfBase, &mpfBase, &mpfTmp2);
    } // End FOR

    // Multiply by the final 2
    mpfr_mul_ui (&mpfRes, &mpfRes, 2);

    // We no longer need this storage
    Myf_clear (&mpfBase);
    Myf_clear (&mpfTmp2);
    Myf_clear (&mpfTmp1);

    return mpfRes;
} // End LogfVfp
#endif

//***************************************************************************
//  $PrimSpecCircleStarStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecCircleStarStorageTypeDyd
    (APLNELM    aplNELMLft,
     LPAPLSTYPE lpaplTypeLft,
     LPTOKEN    lptkFunc,
     APLNELM    aplNELMRht,
     LPAPLSTYPE lpaplTypeRht)

{
    APLSTYPE aplTypeRes;

    // In case the left arg is an empty char,
    //   change its type to BOOL
    if (IsCharEmpty (*lpaplTypeLft, aplNELMLft))
        *lpaplTypeLft = ARRAY_BOOL;

    // In case the right arg is an empty char,
    //   change its type to BOOL
    if (IsCharEmpty (*lpaplTypeRht, aplNELMRht))
        *lpaplTypeRht = ARRAY_BOOL;

    // If both arguments are simple numeric,
    //   the result is FLOAT
    if (IsSimpleNum (*lpaplTypeLft)
     && IsSimpleNum (*lpaplTypeRht))
        aplTypeRes = ARRAY_FLOAT;
    else
        // Calculate the storage type of the result
        aplTypeRes = StorageType (*lpaplTypeLft, lptkFunc, *lpaplTypeRht);

    // Split cases based upon the storage type
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_APA:
        case ARRAY_VFP:
        case ARRAY_NESTED:
            break;

        // Except that RAT becomes VFP
        case ARRAY_RAT:
            aplTypeRes = ARRAY_VFP;

            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
            aplTypeRes = ARRAY_ERROR;

            break;

        defstop
            break;
    } // End SWITCH

    return aplTypeRes;
} // End PrimSpecCircleStarStorageTypeDyd


//***************************************************************************
//  $PrimFnDydCircleStarFisIvI
//
//  Primitive scalar function dyadic CircleStar:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydCircleStarFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for indeterminates:  B {log} B
    if (IsBooleanValue (aplIntegerLft)
     && IsBooleanValue (aplIntegerRht))
        return TranslateQuadICIndex ((APLFLOAT) aplIntegerLft,
                                                icndxLog[aplIntegerLft][aplIntegerRht],
                                     (APLFLOAT) aplIntegerRht,
                                                FALSE);
    // Check for indeterminates:  0 {log} N (N != 0 or 1)
    if (aplIntegerLft EQ 0)
        return TranslateQuadICIndex ((APLFLOAT) aplIntegerLft,
                                                ICNDX_0LOGN,
                                     (APLFLOAT) aplIntegerRht,
                                                FALSE);
    // Check for Complex result
    if (aplIntegerLft < 0)
        RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

    // The EAS says "If A and B are equal, return one."
    if (aplIntegerLft EQ aplIntegerRht)
        return 1;

    // Calculate the log
    // Note that the native IEEE code correctly handles infinite results
    //   such as 5{log}0
    return log ((APLFLOAT) aplIntegerRht) / log ((APLFLOAT) aplIntegerLft);
} // End PrimFnDydCircleStarFisIvI


//***************************************************************************
//  $PrimFnDydCircleStarFisFvF
//
//  Primitive scalar function dyadic CircleStar:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydCircleStarFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for special cases:  0 {log} PoM_
    // Check for special cases:  PoM_ {log} 0
    // Check for special cases:  PoM_ {log} PoM_
    if ((aplFloatLft EQ 0 && IsFltInfinity (aplFloatRht))
     || (IsFltInfinity (aplFloatLft) && aplFloatRht EQ 0)
     || (IsFltInfinity (aplFloatLft) && IsFltInfinity (aplFloatRht)))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    // Check for indeterminates:  B {log} B
    if (IsBooleanValue (aplFloatLft)
     && IsBooleanValue (aplFloatRht))
        return TranslateQuadICIndex (aplFloatLft,
                                     icndxLog[(UINT) aplFloatLft][(UINT) aplFloatRht],
                                     aplFloatRht,
                                     FALSE);
    // Check for indeterminates:  0 {log} N  (N != 0 or 1)
    if (aplFloatLft EQ 0.0)
        return TranslateQuadICIndex (aplFloatLft,
                                     ICNDX_0LOGN,
                                     aplFloatRht,
                                     FALSE);
    // Check for Complex result
    if (SIGN_APLFLOAT (aplFloatLft))
        RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

    // The EAS says "If A and B are equal, return one."
    if (aplFloatLft EQ aplFloatRht)
        return 1;

    // Calculate the log
    // Note that the native IEEE code correctly handles infinite results
    //   such as 5{log}0  and  0.5{log}0
    return log (aplFloatRht) / log (aplFloatLft);
} // End PrimFnDydCircleStarFisFvF


//***************************************************************************
//  $PrimFnDydCircleStarVisVvV
//
//  Primitive scalar function dyadic CircleStar:  V {is} V fn V
//***************************************************************************

APLVFP PrimFnDydCircleStarVisVvV
    (APLVFP     aplVfpLft,
     APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLVFP mpfRes = {0};

    // Check for special cases:  0 {log} PoM_
    // Check for special cases:  PoM_ {log} 0
    // Check for special cases:  PoM_ {log} PoM_
    if ((IsMpf0 (&aplVfpLft) && mpfr_inf_p (&aplVfpRht))
     || (mpfr_inf_p (&aplVfpLft) && IsMpf0 (&aplVfpRht))
     || (mpfr_inf_p (&aplVfpLft) && mpfr_inf_p (&aplVfpRht)))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    // Check for indeterminates:  B {log} B
    if (IsBooleanVfp (&aplVfpLft)
     && IsBooleanVfp (&aplVfpRht))
        return *mpfr_QuadICValue (&aplVfpLft,
                                   icndxLog[IsMpf1 (&aplVfpLft)][IsMpf1(&aplVfpRht)],
                                  &aplVfpRht,
                                  &mpfRes,
                                   FALSE);
    // Check for indeterminates:  0 {log} N  (N != 0 or 1)
    if (mpfr_zero_p (&aplVfpLft))
        return *mpfr_QuadICValue (&aplVfpLft,
                                   ICNDX_0LOGN,
                                  &aplVfpRht,
                                  &mpfRes,
                                   FALSE);
    // Check for Complex result
    if (SIGN_APLVFP (&aplVfpLft))
        RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // The EAS says "If A and B are equal, return one."
    if (mpfr_cmp (&aplVfpLft,  &aplVfpRht) EQ 0)
        mpfr_set_ui (&mpfRes, 1, MPFR_RNDN);
    else
    {
        APLVFP mpfLft,
               mpfRht;
#ifdef DEBUG
////    WCHAR  wszTemp[512];
#endif

        // Calculate log (aplVfpRht) / log (aplVfpLft)
        // Note that the MPFR code correctly handles infinite results
        //   such as 5{log}0  and  0.5{log}0
        mpfLft = PrimFnMonCircleStarVisV (aplVfpLft, lpPrimSpec);
#ifdef DEBUG
////    strcpyW (wszTemp, L"Lft: "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], mpfLft, 0) = WC_EOS; DbgMsgW (wszTemp);
#endif
        mpfRht = PrimFnMonCircleStarVisV (aplVfpRht, lpPrimSpec);
#ifdef DEBUG
////    strcpyW (wszTemp, L"Rht: "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], mpfRht, 0) = WC_EOS; DbgMsgW (wszTemp);
#endif
        mpfr_div (&mpfRes, &mpfRht, &mpfLft, MPFR_RNDN);
#ifdef DEBUG
////    strcpyW (wszTemp, L"Res: "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], mpfRes, 0) = WC_EOS; DbgMsgW (wszTemp);
#endif

        // We no longer need this storage
        Myf_clear (&mpfRht);
        Myf_clear (&mpfLft);
    } // End IF

    return mpfRes;
} // End PrimFnDydCircleStarVisVvV


//***************************************************************************
//  End of File: pf_cstar.c
//***************************************************************************
