//***************************************************************************
//  NARS2000 -- Primitive Function -- QuoteDot
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

#define __GSL_MATRIX_COMPLEX_LONG_DOUBLE_H__

#include "headers.h"
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_sf_result.h>
#include <gsl/gsl_errno.h>


#ifndef PROTO
PRIMSPEC PrimSpecQuoteDot =
{
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecQuoteDotStorageTypeMon,
    NULL,   // &PrimFnMonQuoteDotAPA_EM, -- Can't happen w/QuoteDot

    &PrimFnMonQuoteDotBisB,
    NULL,   // &PrimFnMonQuoteDotBisI, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotBisF, -- Can't happen w/QuoteDot

////              IisB,     // Handled via type promotion (to IisI)
    &PrimFnMonQuoteDotIisI,
    NULL,   // &PrimFnMonQuoteDotIisF, -- Can't happen w/QuoteDot

////              FisB,     // Handled via type promotion (to FisI)
    &PrimFnMonQuoteDotFisI,
    &PrimFnMonQuoteDotFisF,

    &PrimFnMonQuoteDotRisR,

////              VisR,     // Handled via type promotion (to VisV)
    &PrimFnMonQuoteDotVisV,

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecQuoteDotStorageTypeDyd,
    NULL,   // &PrimFnDydQuoteDotAPA_EM, -- Can't happen w/QuoteDot

    &PrimFnDydQuoteDotBisBvB,
    NULL,   // &PrimFnDydQuoteDotBisIvI, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnDydQuoteDotBisFvF, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnDydQuoteDotBisCvC, -- Can't happen w/QuoteDot

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    &PrimFnDydQuoteDotIisIvI,
    NULL,   // &PrimFnDydQuoteDotIisFvF, -- Can't happen w/QuoteDot

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    &PrimFnDydQuoteDotFisIvI,
    &PrimFnDydQuoteDotFisFvF,

    NULL,   // &PrimFnDydQuoteDotBisRvR, -- Can't happen w/QuoteDot
    &PrimFnDydQuoteDotRisRvR,

    NULL,   // &PrimFnDydQuoteDotBisVvV, -- Can't happen w/QuoteDot
////                 VisRvR     // Handled via type promotion (to VisVvV)
    &PrimFnDydQuoteDotVisVvV,

    NULL,   // &PrimFnMonQuoteDotB64isB64, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotB32isB32, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotB16isB16, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotB08isB08, -- Can't happen w/QuoteDot

    &PrimFnDydLeftCaretUnderbarB64isB64vB64,
    &PrimFnDydLeftCaretUnderbarB32isB32vB32,
    &PrimFnDydLeftCaretUnderbarB16isB16vB16,
    &PrimFnDydLeftCaretUnderbarB08isB08vB08,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecQuoteDot};
#endif


//***************************************************************************
//  $PrimFnQuoteDot_EM_YY
//
//  Primitive function for monadic and dyadic QuoteDot ("factorial" and "binomial")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnQuoteDot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnQuoteDot_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_QUOTEDOT);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnQuoteDot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecQuoteDotStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecQuoteDotStorageTypeMon
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
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_RAT:
        case ARRAY_VFP:
        case ARRAY_NESTED:
            break;

        // Except that APAs are converted to INTs
        case ARRAY_APA:
            aplTypeRes = ARRAY_INT;

            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
            aplTypeRes = ARRAY_ERROR;

            break;

        defstop
            break;
    } // End SWITCH

    return aplTypeRes;
} // End PrimSpecQuoteDotStorageTypeMon


//***************************************************************************
//  $PrimFnMonQuoteDotBisB
//
//  Primitive scalar function monadic QuoteDot:  B {is} fn B
//***************************************************************************

APLBOOL PrimFnMonQuoteDotBisB
    (APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (APLBOOL) 1;
} // End PrimFnMonQuoteDotBisB


//***************************************************************************
//  $PrimFnMonQuoteDotIisI
//
//  Primitive scalar function monadic QuoteDot:  I {is} fn I
//***************************************************************************

APLINT PrimFnMonQuoteDotIisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplFloatRes;
    static APLINT factorial[] =
    {
                    1,      //  0
                    1,      //  1
                    2,      //  2
                    6,      //  3
                   24,      //  4
                  120,      //  5
                  720,      //  6
                 5040,      //  7
                40320,      //  8
               362880,      //  9
              3628800,      // 10
             39916800,      // 11
            479001600,      // 12
           6227020800,      // 13
          87178291200,      // 14
        1307674368000,      // 15
       20922789888000,      // 16
      355687428096000,      // 17
     6402373705728000,      // 18
   121645100408832000,      // 19
  2432902008176640000,      // 20
    };

    // Check for indeterminates:  !N for integer N < 0
    if (aplIntegerRht < 0)
    {
        // Get the result as float
        aplFloatRes =
          TranslateQuadICIndex (0,
                                ICNDX_QDOTn,
                     (APLFLOAT) aplIntegerRht,
                                FALSE);
        // If it's infinite, ...
        if (IsFltInfinity (aplFloatRes))
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
        else
            return (APLINT) aplFloatRes;
    } // End IF

    // Check for results too large to express as integers
    if (aplIntegerRht >= countof (factorial))
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

    // Lookup the result
    return factorial[aplIntegerRht];
} // End PrimFnMonQuoteDotIisI


//***************************************************************************
//  $PrimFnMonQuoteDotFisI
//
//  Primitive scalar function monadic QuoteDot:  F {is} fn I
//***************************************************************************

APLFLOAT PrimFnMonQuoteDotFisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return PrimFnMonQuoteDotFisF ((APLFLOAT) aplIntegerRht, lpPrimSpec);
} // End PrimFnMonQuoteDotFisI


//***************************************************************************
//  $PrimFnMonQuoteDotFisF
//
//  Primitive scalar function monadic QuoteDot:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonQuoteDotFisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    int           iRet;
    gsl_sf_result gsr = {0};

    // Check for indeterminates:  !N for integer N < 0
    if (aplFloatRht < 0)
    {
        if (aplFloatRht EQ floor (aplFloatRht)
         || aplFloatRht EQ ceil  (aplFloatRht))
            return TranslateQuadICIndex (0,
                                         ICNDX_QDOTn,
                                         aplFloatRht,
                                         FALSE);
    } // End IF

    // Check for PosInfinity
    if (IsFltPosInfinity (aplFloatRht))
        return fltPosInfinity;

    // Check for too large for GSL
    if ((aplFloatRht + 1) > GSL_SF_GAMMA_XMAX)
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    // Use the GNU Scientific Library Gamma function
    iRet = gsl_sf_gamma_e (aplFloatRht + 1, &gsr);

    // Check the return code
    switch (iRet)
    {
        case GSL_SUCCESS:
            break;

        case GSL_FAILURE:
        case GSL_EDOM:
        case GSL_ERANGE:
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

        defstop
            break;
    } // End SWITCH

    return gsr.val;
} // End PrimFnMonQuoteDotFisF


//***************************************************************************
//  $PrimFnMonQuoteDotRisR
//
//  Primitive scalar function monadic QuoteDot:  R {is} fn R
//***************************************************************************

APLRAT PrimFnMonQuoteDotRisR
    (APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLRAT mpqRes = {0};
    UINT   uRht;

    // Check for indeterminates:  !N for integer N < 0
    if (mpq_integer_p (&aplRatRht)
     && mpq_cmp_ui (&aplRatRht, 0, 1) < 0)
        return *mpq_QuadICValue (&aplRatRht,        // No left arg
                                  ICNDX_QDOTn,
                                 &aplRatRht,
                                 &mpqRes,
                                  FALSE);
    // Check for PosInfinity
    if (IsMpqPosInfinity (&aplRatRht))
        return mpqPosInfinity;

    // If the denominator is 1,
    //   and the numerator fts in a UINT, ...
    if (mpq_integer_p (&aplRatRht)
     && mpz_fits_slong_p (mpq_numref (&aplRatRht)) NE 0)
    {
        // Initialize the result to 0/1
        mpq_init (&mpqRes);

        // Extract the numerator
        uRht = (UINT) mpz_get_si (mpq_numref (&aplRatRht));

        // Compute the factorial
        mpz_fac_ui (mpq_numref (&mpqRes), uRht);
    } else
        RaiseException (EXCEPTION_RESULT_VFP, 0, 0, NULL);

    return mpqRes;
} // End PrimFnMonQuoteDotRisR


//***************************************************************************
//  $PrimFnMonQuoteDotVisV
//
//  Primitive scalar function monadic QuoteDot:  V {is} fn V
//***************************************************************************

APLVFP PrimFnMonQuoteDotVisV
    (APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLMPI mpzRes = {0};
    APLVFP mpfRes = {0};

    // Check for indeterminates:  !N for integer N < 0
    if (mpfr_integer_p (&aplVfpRht)
     && mpfr_cmp_ui (&aplVfpRht, 0) < 0)
        return *mpfr_QuadICValue (&aplVfpRht,       // No left arg
                                   ICNDX_QDOTn,
                                  &aplVfpRht,
                                  &mpfRes,
                                   FALSE);
    // Check for PosInfinity
    if (IsMpfPosInfinity (&aplVfpRht))
        return mpfPosInfinity;

    // If the arg is an integer,
    //   and it fits in a ULONG, ...
    if (mpfr_integer_p (&aplVfpRht)
     && mpfr_fits_uint_p (&aplVfpRht, MPFR_RNDN))
    {
        mpz_init   (&mpzRes);
        mpfr_init0 (&mpfRes);

        mpz_fac_ui (&mpzRes, mpfr_get_ui (&aplVfpRht, MPFR_RNDN));
        mpfr_set_z (&mpfRes, &mpzRes, MPFR_RNDN);

        Myz_clear (&mpzRes);
    } else
    {
        // Initialize the result
        mpfr_init_set (&mpfRes, &aplVfpRht, MPFR_RNDN);
        mpfr_add_ui   (&mpfRes, &mpfRes, 1, MPFR_RNDN);

        // Let MPFR handle it
        mpfr_gamma (&mpfRes, &mpfRes, MPFR_RNDN);
#ifdef DEBUG
        mpfr_free_cache ();
#endif
    } // End IF/ELSE

    return mpfRes;
} // End PrimFnMonQuoteDotVisV


//***************************************************************************
//  $PrimSpecQuoteDotStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecQuoteDotStorageTypeDyd
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

    return aplTypeRes;
} // End PrimSpecQuoteDotStorageTypeDyd


//***************************************************************************
//  $PrimFnDydQuoteDotBisBvB
//
//  Primitive scalar function dyadic QuoteDot:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydQuoteDotBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft <= aplBooleanRht);
} // End PrimFnDydQuoteDotBisBvB


//***************************************************************************
//  $PrimFnDydQuoteDotIisIvI
//
//  Primitive scalar function dyadic QuoteDot:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydQuoteDotIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLINT aplIntegerTmp,
           aplIntegerRes,
           aplIntegerInd;

#define L   aplIntegerLft
#define R   aplIntegerRht
#define T   aplIntegerTmp
#define Z   aplIntegerRes
#define I   aplIntegerInd

    // Determine whether or not L, R, or L - R is a negative integer
    // From ISO-IEC 13751 Extended APL Standard, p. 90
    switch (4*(L < 0) + 2*(R < 0) + 1*(R < L))
    {
        //    L     R   R-L
        case 4*0 + 2*0 + 0:     // (!R)/((!L)*!R-L)
            // WLOG, change L so that L <= (R/2)
            //   using the reflection rule for
            //   binomial coefficients.
            if (L > (R / 2))
                L = R - L;

            // Compute the result as:
            //      T = R-L;
            //      Z = Prod (I, 1, L, (I+T) / I)
            for (T = R - L, Z = 1, I = 1; I <= L; I++)
                Z = (imul64_RE (Z, I + T)) / I;
            return Z;

        case 4*0 + 2*0 + 1*1:   // 0
            return 0;

        case 4*0 + 2*1 + 1*0:   // DOMAIN ERROR
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

        case 4*0 + 2*1 + 1*1:   // ((-1)*L)*L!(L-(R+1))
            return ((L%2) ? -1: 1) * PrimFnDydQuoteDotIisIvI (L, L-(R+1), NULL);

        case 4*1 + 2*0 + 1*0:   // 0
            return 0;

        case 4*1 + 2*0 + 1*1:   // (Can't happen)
            return -1;

        case 4*1 + 2*1 + 1*0:   // ((-1)*R-L)*(-R+1)!|L+1
            return (((R-L)%2) ? -1: 1) * PrimFnDydQuoteDotIisIvI (-(R+1), -(L+1), NULL);

        case 4*1 + 2*1 + 1*1:   // 0
            return 0;

        defstop
            return -1;
    } // End SWITCH

#undef  I
#undef  Z
#undef  T
#undef  R
#undef  L
} // End PrimFnDydQuoteDotIisIvI


//***************************************************************************
//  $PrimFnDydQuoteDotFisIvI
//
//  Primitive scalar function dyadic QuoteDot:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydQuoteDotFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplFloatTmp,
             aplFloatRes,
             aplFloatInd;
    APLINT   aplIntegerTmp,
             aplIntegerRes,
             aplIntegerInd,
             aplIntegerTmp2;
    UBOOL    bRet = TRUE;       // Assume the result is valid

    // FYI:  The only way this function can be called is
    //   from an exception raised from IisIvI above.

#define LI  aplIntegerLft
#define RI  aplIntegerRht
#define TI  aplIntegerTmp
#define TI2 aplIntegerTmp2
#define ZI  aplIntegerRes
#define II  aplIntegerInd

#define LF  aplFloatLft
#define RF  aplFloatRht
#define TF  aplFloatTmp
#define ZF  aplFloatRes
#define IF  aplFloatInd

    // Determine whether or not LI, RI, or LI - RI is a negative integer
    // From ISO-IEC 13751 Extended APL Standard, p. 90
    switch (4*(LI < 0) + 2*(RI < 0) + 1*(RI < LI))
    {
        //    LI   RI  RI-LI
        case 4*0 + 2*0 + 0:     // (!RI)/((!LI)*!RI-LI)
            // WLOG, change LI so that LI <= (RI/2)
            //   using the reflection rule for
            //   binomial coefficients.
            if (LI > (RI / 2))
                LI = RI - LI;

            // Compute the result as:
            //      TI = RI-LI;
            //      ZI = Prod (II, 1, LI, (II+TI) / II)
            for (TI = RI - LI, ZI = 1, II = 1;
                 II <= LI;
                 II++)
            {
                TI2 = imul64 (ZI, II + TI, &bRet);
                if (bRet)
                    ZI = TI2 / II;
                else
                {
                    for (ZF = (APLFLOAT) ZI, TF = (APLFLOAT) TI, IF = (APLFLOAT) II;
                         II <= LI;
                         II++, IF++)
                    {
                        ZF = (ZF * (IF + TF)) / IF;
                        if (IsFltInfinity (ZF))
                            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
                    } // End FOR

                    return ZF;
                } // End IF/ELSE
            } // End FOR

            return (APLFLOAT) ZI;

        case 4*0 + 2*0 + 1*1:   // 0
            return 0;

        case 4*0 + 2*1 + 1*0:   // DOMAIN ERROR
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

        case 4*0 + 2*1 + 1*1:   // ((-1)*LI)*LI!(LI-(RI+1))
            return ((LI%2) ? -1: 1) * PrimFnDydQuoteDotFisIvI (LI, LI-(RI+1), NULL);

        case 4*1 + 2*0 + 1*0:   // 0
            return 0;

        case 4*1 + 2*0 + 1*1:   // (Can't happen)
            return -1;

        case 4*1 + 2*1 + 1*0:   // ((-1)*RI-LI)*(-RI+1)!|LI+1
            return (((RI-LI)%2) ? -1: 1) * PrimFnDydQuoteDotFisIvI (-(RI+1), -(LI+1), NULL);

        case 4*1 + 2*1 + 1*1:   // 0
            return 0;

        defstop
            return -1;
    } // End SWITCH

#undef  IF
#undef  ZF
#undef  TF
#undef  RF
#undef  LF

#undef  II
#undef  ZI
#undef  TI2
#undef  TI
#undef  RI
#undef  LI
} // End PrimFnDydQuoteDotFisIvI


//***************************************************************************
//  $PrimFnDydQuoteDotFisFvF
//
//  Primitive scalar function dyadic QuoteDot:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydQuoteDotFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplFloatTmp,
             aplFloatRes,
             aplFloatInd;

#define LF  aplFloatLft
#define RF  aplFloatRht
#define TF  aplFloatTmp
#define ZF  aplFloatRes
#define IF  aplFloatInd

    // If LF and RF are exact integers
    if ((LF EQ floor (LF)
      || LF EQ ceil  (LF))
     && (RF EQ floor (RF)
      || RF EQ ceil  (RF)))
    {
        // Determine whether or not LF, RF, or LF - RF is a negative integer
        // From ISO-IEC 13751 Extended APL Standard, p. 90
        switch (4*(LF < 0) + 2*(RF < 0) + 1*(RF < LF))
        {
            //    LF   RF  RF-LF
            case 4*0 + 2*0 + 0:     // (!RF)/((!LF)*!RF-LF)
                // WLOG, change LF so that LF <= (RF/2)
                //   using the reflection rule for
                //   binomial coefficients.
                if (LF > (RF / 2))
                    LF = RF - LF;

                // Compute the result as:
                //      TF = RF-LF;
                //      ZF = Prod (IF, 1, LF, (IF+TF) / IF)
                for (TF = RF - LF, ZF = 1, IF = 1;
                     IF <= LF;
                     IF++)
                    ZF = (ZF * (IF + TF)) / IF;

                return ZF;

            case 4*0 + 2*0 + 1*1:   // 0
                return 0;

            case 4*0 + 2*1 + 1*0:   // DOMAIN ERROR
                RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            case 4*0 + 2*1 + 1*1:   // ((-1)*LF)*LF!(LF-(RF+1))
                return (fmod (LF, 2) ? -1: 1) * PrimFnDydQuoteDotFisFvF (LF, LF-(RF+1), NULL);

            case 4*1 + 2*0 + 1*0:   // 0
                return 0;

            case 4*1 + 2*0 + 1*1:   // (Can't happen)
                return -1;

            case 4*1 + 2*1 + 1*0:   // ((-1)*RI-LI)*(-RI+1)!|LI+1
                return (fmod (RF-LF, 2) ? -1: 1) * PrimFnDydQuoteDotFisFvF (-(RF+1), -(LF+1), NULL);

            case 4*1 + 2*1 + 1*1:   // 0
                return 0;

            defstop
                return -1;
        } // End SWITCH
    } else
    {
        // Z = (!R) / (!L) * !R-L
        return
                PrimFnMonQuoteDotFisF (RF, NULL)
             / (PrimFnMonQuoteDotFisF (LF, NULL)
              * PrimFnMonQuoteDotFisF (RF - LF, NULL));
    } // End IF/ELSE

#undef  IF
#undef  ZF
#undef  TF
#undef  RF
#undef  LF
} // End PrimFnDydQuoteDotFisFvF


//***************************************************************************
//  $PrimFnDydQuoteDotRisRvR
//
//  Primitive scalar function dyadic QuoteDot:  R {is} R fn R
//***************************************************************************

APLRAT PrimFnDydQuoteDotRisRvR
    (APLRAT     aplRatLft,
     APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLRAT mpqRes = {0};
    UINT   uLft;

    // Initialize the result to 0/1
    mpq_init (&mpqRes);

    // Using the identity A!B <=> (B-A)!B and because the function
    //   mpz_bin_ui requires that the left argument must fit in a
    //   UINT, we calculate the alternate left argument in case it
    //   fits in a UINT.
    mpq_sub (&mpqRes, &aplRatRht, &aplRatLft);

    // Use the smaller of the two for the left arg
    if (mpq_cmp (&mpqRes, &aplRatLft) > 0)
        mpq_set (&mpqRes, &aplRatLft);

    // If both denominators are 1,
    //   and the left numerator fits in a UINT, ...
    if (mpq_integer_p (&aplRatLft)
     && mpq_integer_p (&aplRatRht)
     && mpz_fits_slong_p (mpq_numref (&mpqRes)) NE 0)
    {
        // Extract the numerator of the left argument
        uLft = (UINT) mpz_get_si (mpq_numref (&mpqRes));

        // Compute the binomial coefficient
        mpz_bin_ui (mpq_numref (&mpqRes), mpq_numref (&aplRatRht), uLft);
    } else
    {
        Myq_clear (&mpqRes);

        RaiseException (EXCEPTION_RESULT_VFP, 0, 0, NULL);
    } // End IF/ELSE

    return mpqRes;
} // End PrimFnDydQuoteDotRisRvR


//***************************************************************************
//  $PrimFnDydQuoteDotVisVvV
//
//  Primitive scalar function dyadic QuoteDot:  V {is} V fn V
//***************************************************************************

APLVFP PrimFnDydQuoteDotVisVvV
    (APLVFP     aplVfpLft,
     APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLVFP mpfRes,
           aplLft,
           aplRht,
           aplTmp = {0};

    // Z = (!R) / (!L) * !R-L
    aplLft = PrimFnMonQuoteDotVisV (aplVfpLft, NULL);           // !L
    aplRht = PrimFnMonQuoteDotVisV (aplVfpRht, NULL);           // !R

    mpfr_init_copy (&aplTmp, &aplVfpRht);                       // R
    mpfr_sub       (&aplTmp, &aplTmp, &aplVfpLft, MPFR_RNDN);   // R-L
    mpfRes = PrimFnMonQuoteDotVisV (aplTmp, NULL);              // !R-L

    mpfr_mul (&mpfRes, &aplLft, &mpfRes         , MPFR_RNDN);   // (!L) * !R-L
    mpfr_div (&mpfRes, &aplRht, &mpfRes         , MPFR_RNDN);   // (!R) / ((!L) * !R-L)

    // We no longer need this storage
    Myf_clear (&aplRht);
    Myf_clear (&aplLft);
    Myf_clear (&aplTmp);

    return mpfRes;
} // End PrimFnDydQuoteDotVisVvV


//***************************************************************************
//  End of File: pf_qdot.c
//***************************************************************************
