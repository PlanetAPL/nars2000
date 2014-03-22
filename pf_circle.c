//***************************************************************************
//  NARS2000 -- Primitive Function -- Circle
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
    if (IsEmpty (aplNELMRht) && IsSimpleChar (*lpaplTypeRht))
        *lpaplTypeRht = ARRAY_BOOL;

    if (IsSimpleChar (*lpaplTypeRht)
     || *lpaplTypeRht EQ ARRAY_LIST)
        return ARRAY_ERROR;

    // The storage type of the result is
    //   the same as that of the right arg
    aplTypeRes = *lpaplTypeRht;

    // Except all simple numerics become FLOAT
    if (IsSimpleNum (aplTypeRes))
        aplTypeRes = ARRAY_FLOAT;
    else
    // Except RAT becomes VFP
    if (IsRat (aplTypeRes))
        aplTypeRes = ARRAY_VFP;

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
#ifdef _WIN64
    return PrimFnMonCircleFisF ((APLFLOAT) aplIntegerRht, lpPrimSpec);
#elif defined (_WIN32)
    APLFLOAT aplTmp;

    // The following code is slightly more accurate
    //   than multiplying by a decimal constant
    //   as it uses the 80-bit representation of pi
    //   but it isn't very portable
    _asm
    {
        fldpi;
        fild    aplIntegerRht;
        fmulp   st(1),st(0);
        fstp    aplTmp;
    }
    return aplTmp;
#else
  #error Need code for this architecture.
#endif
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
#ifdef _WIN64
    return aplFloatRht * 3.1415926535897932384626433832795028;
#elif defined (_WIN32)
    APLFLOAT aplTmp;

    // The following code is slightly more accurate
    //   than multiplying by a decimal constant
    //   as it uses the 80-bit representation of pi
    //   but it isn't very portable
    _asm
    {
        fldpi;
        fmul    aplFloatRht;
        fstp    aplTmp;
    }
    return aplTmp;
#else
  #error Need code for this architecture.
#endif
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

    // Initialize the result
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
    if (IsEmpty (aplNELMLft) && IsSimpleChar (*lpaplTypeLft))
        *lpaplTypeLft = ARRAY_BOOL;

    // In case the right arg is an empty char,
    //   change its type to BOOL
    if (IsEmpty (aplNELMRht) && IsSimpleChar (*lpaplTypeRht))
        *lpaplTypeRht = ARRAY_BOOL;

    // Calculate the storage type of the result
    aplTypeRes = StorageType (*lpaplTypeLft, lptkFunc, *lpaplTypeRht);

    // Except all simple numerics become FLOAT
    if (IsSimpleNum (aplTypeRes))
        aplTypeRes = ARRAY_FLOAT;
    else
    // Except RAT becomes VFP
    if (IsRat (aplTypeRes))
        aplTypeRes = ARRAY_VFP;

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
#ifndef _WIN64
    APLFLOAT aplFloatTmp;
#endif
    // I coded these by hand because they are all easy,
    //   however, to be completely accurate, the sin, cos, and tan
    //   functions might need to have their arguments reduced to
    //   be in range.

    // Split cases based upon the value of the integer left argument
    switch (aplIntegerLft)
    {
        case  4:        // (1 + R * 2) * 0.5
#ifdef _WIN64
            return sqrt ((APLFLOAT) (1 + imul64 (aplIntegerRht, aplIntegerRht)));
#elif defined (_WIN32)
            _asm
            {                           //  ST0     ST1
                fild    aplIntegerRht;  //  Rht
                fild    aplIntegerRht;  //  Rht     Rht
                fmulp   st(1),st(0);    //  Rht*Rht
                fld1;                   //  1       Rht*Rht
                faddp   st(1),st(0);    //  1+Rht*Rht
                fsqrt;                  //  (1+Rht*Rht)*0.5
                fstp    aplFloatTmp;
            }

            // Check for NaN
            if (!_isnan (aplFloatTmp))
                return aplFloatTmp;
            break;
#else
  #error Need code for this architecture.
#endif

        case  3:        // tan (R)
#ifdef _WIN64
            return tan ((APLFLOAT) aplIntegerRht);
#elif defined (_WIN32)
            _asm
            {                           //  ST0     ST1
                fild    aplIntegerRht;  //  Rht
                fsincos;                //  cos (Rht) sin(Rht)
                fdivp   st(1),st(0);    //  tan (Rht)
                fstp    aplFloatTmp;
            }

            // Check for NaN
            if (!_isnan (aplFloatTmp))
                return aplFloatTmp;
            break;
#else
  #error Need code for this architecture.
#endif

        case  2:        // cos (R)
#ifdef _WIN64
            return cos ((APLFLOAT) aplIntegerRht);
#elif defined (_WIN32)
            _asm
            {                           //  ST0     ST1
                fild    aplIntegerRht;  //  Rht
                fcos;                   //  cos (Rht)
                fstp    aplFloatTmp;
            }

            // Check for NaN
            if (!_isnan (aplFloatTmp))
                return aplFloatTmp;
            break;
#else
  #error Need code for this architecture.
#endif

        case  1:        // sin (R)
#ifdef _WIN64
            return sin ((APLFLOAT) aplIntegerRht);
#elif defined (_WIN32)
            _asm
            {                           //  ST0     ST1
                fild    aplIntegerRht;  //  Rht
                fsin;                   //  sin (Rht)
                fstp    aplFloatTmp;
            }

            // Check for NaN
            if (!_isnan (aplFloatTmp))
                return aplFloatTmp;
            break;
#else
  #error Need code for this architecture.
#endif

        case  0:        // (1 - R * 2) * 0.5
#ifdef _WIN64
            return sqrt ((APLFLOAT) (1 - imul64 (aplIntegerRht, aplIntegerRht)));
#elif defined (_WIN32)
            _asm
            {                           //  ST0     ST1
                fild    aplIntegerRht;  //  Rht
                fild    aplIntegerRht;  //  Rht     Rht
                fmulp   st(1),st(0);    //  Rht*Rht
                fchs;                   //  -Rht*Rht
                fld1;                   //  1       -Rht*Rht
                faddp   st(1),st(0);    //  1-Rht*Rht
                fsqrt;                  //  (1-Rht*Rht)*0.5
                fstp    aplFloatTmp;
            }

            // Check for NaN
            if (!_isnan (aplFloatTmp))
                return aplFloatTmp;
            break;
#else
  #error Need code for this architecture.
#endif

        case -4:        // R x (1 - R * -2) * 0.5 a.k.a. ((-1) + R * 2) * 0.5
#ifdef _WIN64
            return sqrt ((APLFLOAT) (imul64 (aplIntegerRht, aplIntegerRht) - 1));
#elif defined (_WIN32)
            _asm
            {                           //  ST0     ST1
                fild    aplIntegerRht;  //  Rht
                fild    aplIntegerRht;  //  Rht     Rht
                fmulp   st(1),st(0);    //  Rht*Rht
                fld1;                   //  1       Rht*Rht
                fchs;                   //  -1      Rht*Rht
                faddp   st(1),st(0);    //  Rht*Rht-1
                fsqrt;                  //  (Rht*Rht-1)*0.5
                fstp    aplFloatTmp;
            }

            // Check for NaN
            if (!_isnan (aplFloatTmp))
                return aplFloatTmp;
            break;
#else
  #error Need code for this architecture.
#endif
        default:
            return PrimFnDydCircleFisFvF ((APLFLOAT) aplIntegerLft,
                                          (APLFLOAT) aplIntegerRht,
                                          lpPrimSpec);
    } // End SWITCH

    RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
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
            return sqrt (1 + pow (aplFloatRht, 2));

        case  3:        // tan (R)
            if (IsInfinity (aplFloatRht))
                break;

            return tan (aplFloatRht);

        case  2:        // cos (R)
            if (IsInfinity (aplFloatRht))
                break;

            return cos (aplFloatRht);

        case  1:        // sin (R)
            if (IsInfinity (aplFloatRht))
                break;

            return sin (aplFloatRht);

        case  0:        // (1 - R * 2) * 0.5
            if (IsInfinity (aplFloatRht))
                break;

            return sqrt (1 - pow (aplFloatRht, 2));

        case -1:        // asin (R)
            // Check for Complex result
            if (fabs (aplFloatRht) > 1)
                RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

            return asin (aplFloatRht);

        case -2:        // acos (R)
            // Check for Complex result
            if (fabs (aplFloatRht) > 1)
                RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

            return acos (aplFloatRht);

        case -3:        // atan (R)
            return atan (aplFloatRht);

        case -4:        // R x (1 - R * -2) * 0.5
            // Check for Complex result
            if (fabs (aplFloatRht) < 1
             && aplFloatRht NE 0)
                RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

            return aplFloatRht * sqrt (1 - pow (aplFloatRht, -2));

        case -5:        // asinh (R)
                        // ln (R + sqrt (1 + R * 2))
            aplFloatTmp = gsl_asinh (aplFloatRht);

            // Check for NaN
            if (_isnan (aplFloatTmp))
                break;
            return aplFloatTmp;

        case -6:        // acosh (R)
                        // 2 x ln (sqrt ((R + 1) x 0.5) + sqrt ((R - 1) x 0.5))
            // Check for Complex result
            if (aplFloatRht < 1)
                RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

            aplFloatTmp = gsl_acosh (aplFloatRht);

            // Check for NaN
            if (_isnan (aplFloatTmp))
                break;
            return aplFloatTmp;

        case -7:        // atanh (R)
                        // 0.5 x (ln (1 + R) - ln (1 - R))
            if (IsInfinity (aplFloatRht))
                break;

            // Check for Complex result
            if (fabs (aplFloatRht) > 1)
                RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

            aplFloatTmp = gsl_atanh (aplFloatRht);

            // Check for NaN
            if (_isnan (aplFloatTmp))
                break;
            return aplFloatTmp;

        default:
            break;
    } // End SWITCH

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
                mpfr_init0 (&mpfRes);
                mpfr_mul (&mpfRes, &aplVfpRht, &aplVfpRht, MPFR_RNDN);  // R * 2
                mpfr_add_ui (&mpfRes, &mpfRes, 1         , MPFR_RNDN);  // 1 + R * 2
                mpfr_sqrt (&mpfRes, &mpfRes              , MPFR_RNDN);  // (1 + R * 2) * 0.5

                return mpfRes;      // sqrt (1 + pow (aplVfpRht, 2));

            case  3:        // tan (R)
                return tanVfp (aplVfpRht);

            case  2:        // cos (R)
                return cosVfp (aplVfpRht);

            case  1:        // sin (R)
                return sinVfp (aplVfpRht);

            case  0:        // (1 - R * 2) * 0.5
                mpfr_init0 (&mpfRes);
                mpfr_mul (&mpfRes, &aplVfpRht, &aplVfpRht, MPFR_RNDN);  // R * 2
                mpfr_ui_sub (&mpfRes, 1, &mpfRes         , MPFR_RNDN);  // 1 - R * 2
                mpfr_sqrt (&mpfRes, &mpfRes              , MPFR_RNDN);  // (1 - R * 2) * 0.5

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

            case -4:        // R x (1 - R * -2) * 0.5
                // Check for Complex result
                if (mpfr_cmp_si (&aplVfpRht,  1) < 0
                 && mpfr_cmp_si (&aplVfpRht, -1) < 0
                 && !mpfr_zero_p (&aplVfpRht))
                    RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

                mpfr_init0 (&mpfRes);
                mpfr_mul (&mpfRes, &aplVfpRht, &aplVfpRht, MPFR_RNDN);  // R * 2
                mpfr_ui_div (&mpfRes, 1, &mpfRes         , MPFR_RNDN);          // R * -2
                mpfr_ui_sub (&mpfRes, 1, &mpfRes         , MPFR_RNDN);          // 1 - R * -2
                mpfr_sqrt (&mpfRes, &mpfRes              , MPFR_RNDN);               // (1 - R * -2) * 0.5
                mpfr_mul (&mpfRes, &mpfRes, &aplVfpRht   , MPFR_RNDN);    // R x (1 - R * -2) * 0.5

                return mpfRes;      // aplVfpRht * sqrt (1 - pow (aplVfpRht, -2));

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
    } // End IF

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

    // Initialize the result
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

    // Initialize the result
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

    // Initialize the result
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

    // Initialize the result
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

    // Initialize the result
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

    // Initialize the result
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

    // Initialize the result
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

    // Initialize the result
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

    // Initialize the result
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

    // Initialize the result
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

    // Initialize the result
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

    // Initialize the result
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
