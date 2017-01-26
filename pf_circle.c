//***************************************************************************
//  NARS2000 -- Primitive Function -- Circle
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
#include <gsl\gsl_sys.h>


#ifndef PROTO
PRIMSPEC PrimSpecCircle = {
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecCircleStorageTypeMon,
    NULL,   // &PrimFnMonCircleAPA_EM, -- Can't happen w/Circle

    NULL,   // &PrimFnMonCircleBisB, -- Can't happen w/Circle
    NULL,   // &PrimFnMonCircleBisI, -- Can't happen w/Circle
    NULL,   // &PrimFnMonCircleBisF, -- Can't happen w/Circle

////                IisB,   // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonCircleIisI, -- Can't happen w/Circle
    NULL,   // &PrimFnMonCircleIisF, -- Can't happen w/Circle

////                FisB,   // Handled via type promotion (to FisI)
    &PrimFnMonCircleFisI,
    &PrimFnMonCircleFisF,

    NULL,   // &PrimFnMonCircleRisR, -- Can't happen w/Circle

////                VisR,   // Handled via type promotion (to VisV)
    &PrimFnMonCircleVisV,

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecCircleStorageTypeDyd,
    NULL,   // &PrimFnDydCircleAPA_EM, -- Can't happen w/Circle

    NULL,   // &PrimFnDydCircleBisBvB, -- Can't happen w/Circle
    NULL,   // &PrimFnDydCircleBisIvI, -- Can't happen w/Circle
    NULL,   // &PrimFnDydCircleBisFvF, -- Can't happen w/Circle
    NULL,   // &PrimFnDydCircleBisCvC, -- Can't happen w/Circle

////                IisBvB,     // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydCircleIisIvI, -- Can't happen w/Circle
    NULL,   // &PrimFnDydCircleIisFvF, -- Can't happen w/Circle

////                FisBvB,     // Handled via type promotion (to FisIvI)
    &PrimFnDydCircleFisIvI,
    &PrimFnDydCircleFisFvF,

    NULL,   // &PrimFnDydCircleBisRvR, -- Can't happen w/Circle
    NULL,   // &PrimFnDydCircleRisRvR, -- Can't happen w/Circle

    NULL,   // &PrimFnDydCircleBisVvV, -- Can't happen w/Circle
////                VisRvR,     // Handled via type promotion (to VisVvV)
    &PrimFnDydCircleVisVvV,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecCircle};
#endif


//***************************************************************************
//  $PrimFnCircle_EM_YY
//
//  Primitive function for monadic and dyadic Circle ("pi times" and "circle functions")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnCircle_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnCircle_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (IsAPLCharCircle (lptkFunc->tkData.tkChar));

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnCircle_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecCircleStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecCircleStorageTypeMon
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
        // Except all simple numerics become FLOAT
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_APA:
            aplTypeRes = ARRAY_FLOAT;

            break;

        // Except RAT becomes VFP
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
} // End PrimSpecCircleStorageTypeMon


//***************************************************************************
//  $PrimFnMonCircleFisI
//
//  Primitive scalar function monadic Circle:  F {is} fn I
//***************************************************************************

APLFLOAT PrimFnMonCircleFisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplTmp;

    // Call assembler function
    iAsmMulPiInt (&aplTmp, &aplIntegerRht);

    return aplTmp;
} // End PrimFnMonCircleFisI


//***************************************************************************
//  $PrimFnMonCircleFisF
//
//  Primitive scalar function monadic Circle:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonCircleFisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplTmp;

    // Call assembler function
    iAsmMulPiFlt (&aplTmp, &aplFloatRht);

    return aplTmp;
} // End PrimFnMonCircleFisF


//***************************************************************************
//  $PrimFnMonCircleVisV
//
//  Primitive scalar function monadic Circle:  V {is} fn V
//***************************************************************************

APLVFP PrimFnMonCircleVisV
    (APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLVFP mpfRes = {0};

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    mpfr_mul (&mpfRes, &aplVfpRht, &GetMemPTD ()->mpfrPi, MPFR_RNDN);

    return mpfRes;
} // End PrimFnMonCircleVisV


//***************************************************************************
//  $PrimSpecCircleStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecCircleStorageTypeDyd
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

    // Calculate the storage type of the result
    aplTypeRes = StorageType (*lpaplTypeLft, lptkFunc, *lpaplTypeRht);

    // Split cases based upon the storage type
    switch (aplTypeRes)
    {
        // Except all simple numerics become FLOAT
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_APA:
            aplTypeRes = ARRAY_FLOAT;

            break;

        // Except RAT becomes VFP
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
} // End PrimSpecCircleStorageTypeDyd


//***************************************************************************
//  $PrimFnDydCircleFisIvI
//
//  Primitive scalar function dyadic Circle:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydCircleFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplFloatTmp;

    // Split cases based upon the value of the integer left argument
    switch (aplIntegerLft)
    {
        case  4:        // (1 + R * 2) * 0.5
            // Call assembler function
            iAsmCircle4Int (&aplFloatTmp, &aplIntegerRht);

            // Check for NaN
            if (!_isnan (aplFloatTmp))
                return aplFloatTmp;
            break;

        case  3:        // tan (R)
            // Call assembler function
            iAsmCircle3Int (&aplFloatTmp, &aplIntegerRht);

            // Check for NaN
            if (!_isnan (aplFloatTmp))
                return aplFloatTmp;
            break;

        case  2:        // cos (R)
            // Call assembler function
            iAsmCircle2Int (&aplFloatTmp, &aplIntegerRht);

            // Check for NaN
            if (!_isnan (aplFloatTmp))
                return aplFloatTmp;
            break;

        case  1:        // sin (R)
            // Call assembler function
            iAsmCircle1Int (&aplFloatTmp, &aplIntegerRht);

            // Check for NaN
            if (!_isnan (aplFloatTmp))
                return aplFloatTmp;
            break;

        case  0:        // (1 - R * 2) * 0.5
            // Check for Complex result
            if (abs64 (aplIntegerRht) > 1)
                RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

            // Call assembler function
            iAsmCircle0Int (&aplFloatTmp, &aplIntegerRht);

            // Check for NaN
            if (!_isnan (aplFloatTmp))
                return aplFloatTmp;
            break;

        case -4:        // R x (1 - R * -2) * 0.5   a.k.a. ((-1) + R * 2) * 0.5
            // Check for Complex result
            if (abs64 (aplIntegerRht) < 1)
                RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

            // Call assembler function
            iAsmCircleN4Int (&aplFloatTmp, &aplIntegerRht);

            // Check for NaN
            if (!_isnan (aplFloatTmp))
                return aplFloatTmp;
            break;

        default:
            return PrimFnDydCircleFisFvF ((APLFLOAT) aplIntegerLft,
                                          (APLFLOAT) aplIntegerRht,
                                          lpPrimSpec);
    } // End SWITCH

    RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    return 0;   // To keep the compiler happy
} // End PrimFnDydCircleFisIvI


//***************************************************************************
//  $PrimFnDydCircleFisFvF
//
//  Primitive scalar function dyadic Circle:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydCircleFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    UINT     bRet = TRUE;
    APLINT   aplLft;
    APLFLOAT aplFloatTmp;
    UBOOL    bNonce = FALSE;            // True iff the error is NONCE

    // Attempt to convert the float to an integer using System []CT
    aplLft = FloatToAplint_SCT (aplFloatLft, &bRet);

    if (bRet)
    // Split cases based upon the value of the (now) integer left argument
    switch (aplLft)
    {
        case  7:        // tanh (R)
            return tanh (aplFloatRht);

        case  6:        // cosh (R)
            return cosh (aplFloatRht);

        case  5:        // sinh (R)
            return sinh (aplFloatRht);

        case  4:        // (1 + R * 2) * 0.5
            // Get absolute value to avoid duplication in comparison and result
            aplFloatTmp = fabs (aplFloatRht);

            // Values at or above this limit when squared
            //   return infinity, so we return the arg unchanged.
            // Also, 2*26 = 67108864 is the smallest positive number such that
            //   a=sqrt (1 + a * 2)
            if (aplFloatTmp >= 1.3407807929942596E154)
                return aplFloatTmp;
            else
            {
                // Call assembler function
                iAsmCircle4Flt (&aplFloatTmp, &aplFloatRht);

                // Check for NaN
                if (!_isnan (aplFloatTmp))
                    return aplFloatTmp;
                break;
            } // End IF/ELSE

        case  3:        // tan (R)
            // Call assembler function
            iAsmCircle3Flt (&aplFloatTmp, &aplFloatRht);

            // Check for NaN
            if (!_isnan (aplFloatTmp))
                return aplFloatTmp;
            else
                break;

        case  2:        // cos (R)
            // Call assembler function
            iAsmCircle2Flt (&aplFloatTmp, &aplFloatRht);

            // Check for NaN
            if (!_isnan (aplFloatTmp))
                return aplFloatTmp;
            else
                break;

        case  1:        // sin (R)
            // Call assembler function
            iAsmCircle1Flt (&aplFloatTmp, &aplFloatRht);

            // Check for NaN
            if (!_isnan (aplFloatTmp))
                return aplFloatTmp;
            else
                break;

        case  0:        // (1 - R * 2) * 0.5
            // Check for Complex result
            if (bNonce = fabs (aplFloatRht) > 1)
                break;

            // Call assembler function
            iAsmCircle0Flt (&aplFloatTmp, &aplFloatRht);

            // Check for NaN
            if (!_isnan (aplFloatTmp))
                return aplFloatTmp;
            else
                break;

        case -1:        // asin (R)
            // Check for Complex result
            if (bNonce = fabs (aplFloatRht) > 1)
                break;

            return asin (aplFloatRht);

        case -2:        // acos (R)
            // Check for Complex result
            if (bNonce = fabs (aplFloatRht) > 1)
                break;

            return acos (aplFloatRht);

        case -3:        // atan (R)
            return atan (aplFloatRht);

        case -4:        // R x (1 - R * -2) * 0.5   a.k.a. ((-1) + R * 2) * 0.5
            // Check for Complex result
            if (bNonce = fabs (aplFloatRht) < 1)
                break;

            // Call assembler function
            iAsmCircleN4Flt (&aplFloatTmp, &aplFloatRht);

            // Check for NaN
            if (bNonce = _isnan (aplFloatTmp))
                break;
            return aplFloatTmp;

        case -5:        // asinh (R)
                        // ln (R + sqrt (1 + R * 2))
            aplFloatTmp = gsl_asinh (aplFloatRht);

            // Check for NaN
            if (bNonce = _isnan (aplFloatTmp))
                break;
            return aplFloatTmp;

        case -6:        // acosh (R)
                        // 2 x ln (sqrt ((R + 1) x 0.5) + sqrt ((R - 1) x 0.5))
            // Check for Complex result
            if (aplFloatRht < 1)
                RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

            aplFloatTmp = gsl_acosh (aplFloatRht);

            // Check for NaN
            if (bNonce = _isnan (aplFloatTmp))
                break;
            return aplFloatTmp;

        case -7:        // atanh (R)
                        // 0.5 x (ln (1 + R) - ln (1 - R))
            // Check for Complex result
            if (bNonce = fabs (aplFloatRht) > 1)
                break;

            aplFloatTmp = gsl_atanh (aplFloatRht);

            // Check for NaN
            if (bNonce = _isnan (aplFloatTmp))
                break;
            return aplFloatTmp;

        default:
            break;
    } // End SWITCH

    if (bNonce)
        RaiseException (EXCEPTION_NONCE_ERROR , 0, 0, NULL);
    else
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    return 0;   // To keep the compiler happy
} // End PrimFnDydCircleFisFvF


//***************************************************************************
//  $PrimFnDydCircleVisVvV
//
//  Primitive scalar function dyadic Circle:  V {is} V fn V
//***************************************************************************

APLVFP PrimFnDydCircleVisVvV
    (APLVFP     aplVfpLft,
     APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLVFP mpfRes = {0};
    APLINT aplLft;
    UBOOL  bNonce = FALSE;              // True iff the error is NONCE

    // Ensure the left arg is valid
    if (mpfr_integer_p (&aplVfpLft)
     && mpfr_cmp_si (&aplVfpLft, -7) >= 0
     && mpfr_cmp_si (&aplVfpLft,  7) <= 0)
    {
        // Get the left arg as an int
        aplLft = mpfr_get_si (&aplVfpLft, MPFR_RNDN);

        // Split cases based upon the value of the (now) integer left argument
        switch (aplLft)
        {
            case  7:        // tanh (R)
                return tanhVfp (aplVfpRht);

            case  6:        // cosh (R)
                return coshVfp (aplVfpRht);

            case  5:        // sinh (R)
                return sinhVfp (aplVfpRht);

            case  4:        // (1 + R * 2) * 0.5
                // Initialize the result to 0
                mpfr_init0  (&mpfRes);
                mpfr_mul    (&mpfRes, &aplVfpRht, &aplVfpRht, MPFR_RNDN);  // R * 2
                mpfr_add_si (&mpfRes, &mpfRes   , 1         , MPFR_RNDN);  // 1 + R * 2
                mpfr_sqrt   (&mpfRes, &mpfRes               , MPFR_RNDN);  // (1 + R * 2) * 0.5

                return mpfRes;      // sqrt (1 + pow (aplVfpRht, 2));

            case  3:        // tan (R)
                return tanVfp (aplVfpRht);

            case  2:        // cos (R)
                return cosVfp (aplVfpRht);

            case  1:        // sin (R)
                return sinVfp (aplVfpRht);

            case  0:        // (1 - R * 2) * 0.5
                // Check for Complex result
                if (mpfr_cmp_si (&aplVfpRht,  1) > 0
                 || mpfr_cmp_si (&aplVfpRht, -1) < 0)
                    RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

                // Initialize the result to 0
                mpfr_init0  (&mpfRes);
                mpfr_mul    (&mpfRes, &aplVfpRht, &aplVfpRht, MPFR_RNDN);  // R * 2
                mpfr_ui_sub (&mpfRes, 1         , &mpfRes   , MPFR_RNDN);  // 1 - R * 2
                mpfr_sqrt   (&mpfRes,             &mpfRes   , MPFR_RNDN);  // (1 - R * 2) * 0.5

                return mpfRes;      // sqrt (1 - pow (aplVfpRht, 2));

            case -1:        // asin (R)
                // Check for Complex result
                if (mpfr_cmp_si (&aplVfpRht,  1) > 0
                 || mpfr_cmp_si (&aplVfpRht, -1) < 0)
                    RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

                return asinVfp (aplVfpRht);

            case -2:        // acos (R)
                // Check for Complex result
                if (mpfr_cmp_si (&aplVfpRht,  1) > 0
                 || mpfr_cmp_si (&aplVfpRht, -1) < 0)
                    RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

                return acosVfp (aplVfpRht);

            case -3:        // atan (R)
                return atanVfp (aplVfpRht);

            case -4:        // R x (1 - R * -2) * 0.5   a.k.a. ((-1) + R * 2) * 0.5
                // Check for Complex result
                if (mpfr_cmp_si (&aplVfpRht,  1) < 0
                 && mpfr_cmp_si (&aplVfpRht, -1) > 0)
                    RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

                // Initialize the result to 0
                mpfr_init0  (&mpfRes);
                mpfr_mul    (&mpfRes, &aplVfpRht, &aplVfpRht, MPFR_RNDN);   // R * 2
                mpfr_sub_si (&mpfRes, &mpfRes   , 1         , MPFR_RNDN);   // (-1) + R * 2
                mpfr_sqrt   (&mpfRes, &mpfRes               , MPFR_RNDN);   // ((-1) + R * 2) * 0.5

                // Check for NaN
                if (bNonce = mpfr_nan_p (&mpfRes))
                    break;

                return mpfRes;      // sqrt ((-1) + pow (aplVfpRht, 2));

            case -5:        // asinh (R)
                            // ln (R + sqrt (1 + R * 2))
                return asinhVfp (aplVfpRht);

            case -6:        // acosh (R)
                            // 2 x ln (sqrt ((R + 1) x 0.5) + sqrt ((R - 1) x 0.5))
                // Check for Complex result
                if (mpfr_cmp_si (&aplVfpRht, 1) < 0)
                    RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

                return acoshVfp (aplVfpRht);

            case -7:        // atanh (R)
                            // 0.5 x (ln (1 + R) - ln (1 - R))
                // Check for Complex result
                if (mpfr_cmp_si (&aplVfpRht,  1) > 0
                 || mpfr_cmp_si (&aplVfpRht, -1) < 0)
                    RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

                return atanhVfp (aplVfpRht);

            default:
                break;
        } // End SWITCH

        Myf_clear (&mpfRes);
    } // End IF

    if (bNonce)
        RaiseException (EXCEPTION_NONCE_ERROR , 0, 0, NULL);
    else
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    return mpfRes;      // To keep the compiler happy
} // End PrimFnDydCircleVisVvV


//***************************************************************************
//  $tanhVfp:   7
//***************************************************************************

APLVFP tanhVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes = {0};

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Calculate the function
    mpfr_tanh (&mpfRes, &aplVfpRht, MPFR_RNDN);

    return mpfRes;
} // End tanhVfp


//***************************************************************************
//  $coshVfp:   6
//***************************************************************************

APLVFP coshVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes = {0};

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Calculate the function
    mpfr_cosh (&mpfRes, &aplVfpRht, MPFR_RNDN);

    return mpfRes;
} // End coshVfp


//***************************************************************************
//  $sinhVfp:   5
//***************************************************************************

APLVFP sinhVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes = {0};

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Calculate the function
    mpfr_sinh (&mpfRes, &aplVfpRht, MPFR_RNDN);

    return mpfRes;
} // End sinhVfp


//***************************************************************************
//  $tanVfp:    3
//***************************************************************************

APLVFP tanVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes = {0};

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Calculate the function
    mpfr_tan  (&mpfRes, &aplVfpRht, MPFR_RNDN);

    return mpfRes;
} // End tanVfp


//***************************************************************************
//  $cosVfp:    2
//***************************************************************************

APLVFP cosVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes = {0};

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Calculate the function
    mpfr_cos  (&mpfRes, &aplVfpRht, MPFR_RNDN);

    return mpfRes;
} // End cosVfp


//***************************************************************************
//  $sinVfp:    1
//***************************************************************************

APLVFP sinVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes = {0};

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Calculate the function
    mpfr_sin  (&mpfRes, &aplVfpRht, MPFR_RNDN);

    return mpfRes;
} // End sinVfp


//***************************************************************************
//  $asinVfp:  -1
//***************************************************************************

APLVFP asinVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes = {0};

    // Check for special cases:  R < -1  or  R > 1
    if (mpfr_cmp_si (&aplVfpRht, -1) < 0
     || mpfr_cmp_si (&aplVfpRht,  1) > 0)
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Calculate the function
    mpfr_asin (&mpfRes, &aplVfpRht, MPFR_RNDN);

    return mpfRes;
} // End asinVfp


//***************************************************************************
//  $acosVfp:  -2
//***************************************************************************

APLVFP acosVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes = {0};

    // Check for special cases:  R < -1  or  R > 1
    if (mpfr_cmp_si (&aplVfpRht, -1) < 0
     || mpfr_cmp_si (&aplVfpRht,  1) > 0)
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Calculate the function
    mpfr_acos (&mpfRes, &aplVfpRht, MPFR_RNDN);

    return mpfRes;
} // End acosVfp


//***************************************************************************
//  $atanVfp:  -3
//***************************************************************************

APLVFP atanVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes = {0};

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Calculate the function
    mpfr_atan (&mpfRes, &aplVfpRht, MPFR_RNDN);

    return mpfRes;
} // End atanVfp


//***************************************************************************
//  $asinhVfp:  -5
//***************************************************************************

APLVFP asinhVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes = {0};

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Calculate the function
    mpfr_asinh (&mpfRes, &aplVfpRht, MPFR_RNDN);

    // Check for a NaN
    if (mpfr_nan_p (&mpfRes))
    {
        mpfr_clear (&mpfRes);

        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
    } // End IF

    return mpfRes;
} // End asinhVfp


//***************************************************************************
//  $acoshVfp:  -6
//***************************************************************************

APLVFP acoshVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes = {0};

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Calculate the function
    mpfr_acosh (&mpfRes, &aplVfpRht, MPFR_RNDN);

    // Check for a NaN
    if (mpfr_nan_p (&mpfRes))
    {
        mpfr_clear (&mpfRes);

        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
    } // End IF

    return mpfRes;
} // End acoshVfp


//***************************************************************************
//  $atanhVfp:  -7
//***************************************************************************

APLVFP atanhVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes = {0};

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Calculate the function
    mpfr_atanh (&mpfRes, &aplVfpRht, MPFR_RNDN);

    // Check for a NaN
    if (mpfr_nan_p (&mpfRes))
    {
        mpfr_clear (&mpfRes);

        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
    } // End IF

    return mpfRes;
} // End atanhVfp


//***************************************************************************
//  End of File: pf_circle.c
//***************************************************************************
