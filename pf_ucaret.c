//***************************************************************************
//  NARS2000 -- Primitive Function -- UpCaret
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


#ifndef PROTO
PRIMSPEC PrimSpecUpCaret = {
    // Monadic functions
    &PrimFnMonValenceError_EM,
    NULL,   // &PrimSpecUpCaretStorageTypeMon, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretAPA_EM, -- Can't happen w/UpCaret

    NULL,   // &PrimFnMonUpCaretBisB, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretBisI, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretBisF, -- Can't happen w/UpCaret

////                 IisB,   // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonUpCaretIisI, -- Can't happend w/UpCaret
    NULL,   // &PrimFnMonUpCaretIisF, -- Can't happend w/UpCaret

////                 FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonUpCaretFisI, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretFisF, -- Can't happen w/UpCaret

    NULL,   // &PrimFnMonUpCaretRisR, -- Can't happen w/UpCaret

////                 VisR,   // Handled via type promotion (to VisV)
    NULL,   // &PrimFnMonUpCaretVisV, -- Can't happen w/UpCaret

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecUpCaretStorageTypeDyd,
    NULL,   // &PrimFnDydUpCaretAPA_EM, -- Can't happen w/UpCaret

    &PrimFnDydUpCaretBisBvB,
    NULL,   // &PrimFnDydUpCaretBisIvI, -- Can't happen w/UpCaret
    NULL,   // &PrimFnDydUpCaretBisFvF, -- Can't happen w/UpCaret
    NULL,   // &PrimFnDydUpCaretBisCvC, -- Can't happen w/UpCaret

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    &PrimFnDydUpCaretIisIvI,
    NULL,   // &PrimFnDydUpCaretIisFvF, -- Can't happen w/UpCaret

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    &PrimFnDydUpCaretFisIvI,
    &PrimFnDydUpCaretFisFvF,

    NULL,   // &PrimFnDydUpCaretBisRvR, -- Can't happen w/UpCaret
    &PrimFnDydUpCaretRisRvR,

    NULL,   // &PrimFnDydUpCaretBisVvV, -- Can't happen w/UpCaret
////                 VisRvR     // Handled via type promotion (to VisVvV)
    &PrimFnDydUpCaretVisVvV,

    NULL,   // &PrimFnMonUpCaretB64isB64, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretB32isB32, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretB16isB16, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretB08isB08, -- Can't happen w/UpCaret

    &PrimFnDydUpCaretB64isB64vB64,
    &PrimFnDydUpCaretB32isB32vB32,
    &PrimFnDydUpCaretB16isB16vB16,
    &PrimFnDydUpCaretB08isB08vB08,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecUpCaret};
#endif


//***************************************************************************
//  $PrimFnUpCaret_EM_YY
//
//  Primitive function for monadic and dyadic UpCaret (ERROR and "and/LCM")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnUpCaret_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnUpCaret_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_UPCARET
         || lptkFunc->tkData.tkChar EQ UTF16_CIRCUMFLEX);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnUpCaret_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecUpCaretStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecUpCaretStorageTypeDyd
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

    return aplTypeRes;
} // End PrimSpecUpCaretStorageTypeDyd


//***************************************************************************
//  $lcmAplInt
//
//  LCM (Least Common Multiple) for aplIntegers
//***************************************************************************

APLINT lcmAplInt
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLINT aplTmp, aplLft, aplRht;

    // Ensure both arguments are non-negative
    aplLft = PrimFnMonStileIisI (aplIntegerLft, lpPrimSpec);
    aplRht = PrimFnMonStileIisI (aplIntegerRht, lpPrimSpec);

    // Calculate the GCD
    aplTmp = gcdAplInt (aplLft, aplRht, lpPrimSpec);
    if (aplTmp EQ 0)
        return aplTmp;

    aplTmp = aplLft * (aplRht / aplTmp);

    // The sign of the result is the sign of the left argument
    if (aplIntegerLft < 0)
        return -aplTmp;
    else
        return  aplTmp;
} // End lcmAplInt


//***************************************************************************
//  $lcmAplFloat
//
//  LCM (Least Common Multiple) for aplFloats
//***************************************************************************

APLFLOAT lcmAplFloat
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplTmp, aplLft, aplRht;

    // Ensure both arguments are non-negative
    aplLft = PrimFnMonStileFisF (aplFloatLft, lpPrimSpec);
    aplRht = PrimFnMonStileFisF (aplFloatRht, lpPrimSpec);

    // Calculate the GCD
    aplTmp = gcdAplFloat (aplLft, aplRht, lpPrimSpec);
    if (fabs (aplTmp) < SYS_CT)
        return aplTmp;

    aplTmp = aplLft * (aplRht / aplTmp);

    // The sign of the result is the sign of the left argument
    if (aplFloatLft < 0)
        return -aplTmp;
    else
        return  aplTmp;
} // End lcmAplFloat


//***************************************************************************
//  $lcmAplRat
//
//  LCM (Least Common Multiple) for aplRats
//***************************************************************************

APLRAT lcmAplRat
    (APLRAT     aplRatLft,
     APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLRAT aplTmp,
           aplLft,
           aplRht;

    // Ensure both arguments are non-negative
    aplLft = PrimFnMonStileRisR (aplRatLft, lpPrimSpec);
    aplRht = PrimFnMonStileRisR (aplRatRht, lpPrimSpec);

    // Calculate the GCD
    aplTmp = gcdAplRat (aplLft, aplRht, lpPrimSpec);
    if (!IsMpq0 (&aplTmp))
    {
////////aplTmp = aplLft * (aplRht / aplTmp);

        mpq_div (&aplTmp, &aplRht, &aplTmp);
        mpq_mul (&aplTmp, &aplLft, &aplTmp);

        // The sign of the result is the sign of the left argument
        if (mpq_sgn (&aplRatLft) < 0)
            mpq_neg (&aplTmp, &aplTmp);
    } // End IF

    return aplTmp;
} // End lcmAplRat


//***************************************************************************
//  $lcmAplVfp
//
//  LCM (Least Common Multiple) for aplVfps
//***************************************************************************

APLVFP lcmAplVfp
    (APLVFP     aplVfpLft,
     APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLVFP aplTmp,
           aplLft,
           aplRht;

    // Ensure both arguments are non-negative
    aplLft = PrimFnMonStileVisV (aplVfpLft, lpPrimSpec);
    aplRht = PrimFnMonStileVisV (aplVfpRht, lpPrimSpec);

    // Calculate the GCD
    aplTmp = gcdAplVfp (aplLft, aplRht, lpPrimSpec);
    if (!IsMpf0 (&aplTmp))
    {
////////aplTmp = aplLft * (aplRht / aplTmp);

        mpfr_div (&aplTmp, &aplRht, &aplTmp, MPFR_RNDN);
        mpfr_mul (&aplTmp, &aplLft, &aplTmp, MPFR_RNDN);

        // The sign of the result is the sign of the left argument
        if (mpfr_sgn (&aplVfpLft) < 0)
            mpfr_neg0 (&aplTmp, &aplTmp, MPFR_RNDN);
    } // End IF

    return aplTmp;
} // End lcmAplVfp


//***************************************************************************
//  $PrimFnDydUpCaretBisBvB
//
//  Primitive scalar function dyadic UpCaret:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydUpCaretBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretBisBvB


//***************************************************************************
//  $PrimFnDydUpCaretB64isB64vB64
//
//  Primitive scalar function dyadic UpCaret:  B64 {is} B64 fn B64
//***************************************************************************

APLB64 PrimFnDydUpCaretB64isB64vB64
    (APLB64     aplBooleanLft,
     APLB64     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretB64isB64vB64


//***************************************************************************
//  $PrimFnDydUpCaretB32isB32vB32
//
//  Primitive scalar function dyadic UpCaret:  B32 {is} B32 fn B32
//***************************************************************************

APLB32 PrimFnDydUpCaretB32isB32vB32
    (APLB32     aplBooleanLft,
     APLB32     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretB32isB32vB32


//***************************************************************************
//  $PrimFnDydUpCaretB16isB16vB16
//
//  Primitive scalar function dyadic UpCaret:  B16 {is} B16 fn B16
//***************************************************************************

APLB16 PrimFnDydUpCaretB16isB16vB16
    (APLB16     aplBooleanLft,
     APLB16     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretB16isB16vB16


//***************************************************************************
//  $PrimFnDydUpCaretB08isB08vB08
//
//  Primitive scalar function dyadic UpCaret:  B08 {is} B08 fn B08
//***************************************************************************

APLB08 PrimFnDydUpCaretB08isB08vB08
    (APLB08     aplBooleanLft,
     APLB08     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretB08isB08vB08


//***************************************************************************
//  $PrimFnDydUpCaretIisIvI
//
//  Primitive scalar function dyadic UpCaret:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydUpCaretIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return lcmAplInt (aplIntegerLft, aplIntegerRht, lpPrimSpec);
} // End PrimFnDydUpCaretIisIvI


//***************************************************************************
//  $PrimFnDydUpCaretFisIvI
//
//  Primitive scalar function dyadic UpCaret:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydUpCaretFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (APLFLOAT) lcmAplInt (aplIntegerLft, aplIntegerRht, lpPrimSpec);
} // End PrimFnDydUpCaretFisIvI


//***************************************************************************
//  $PrimFnDydUpCaretFisFvF
//
//  Primitive scalar function dyadic UpCaret:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydUpCaretFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for indeterminates:  lcm (PoM_, 0)  or  lcm (0, PoM_)
    if ((IsInfinity (aplFloatLft) && (aplFloatRht EQ 0))
     || (IsInfinity (aplFloatRht) && (aplFloatLft EQ 0)))
        return TranslateQuadICIndex (aplFloatLft,
                                     ICNDX_0LCMInf,
                                     aplFloatRht);
    // Check for special cases:  lcm (PoM_, N)  or  lcm (N, PoM_)
    if (IsInfinity (aplFloatLft)
     || IsInfinity (aplFloatRht))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    return lcmAplFloat (aplFloatLft, aplFloatRht, lpPrimSpec);
} // End PrimFnDydUpCaretFisFvF


//***************************************************************************
//  $PrimFnDydUpCaretRisRvR
//
//  Primitive scalar function dyadic UpCaret:  R {is} fn R fn R
//***************************************************************************

APLRAT PrimFnDydUpCaretRisRvR
    (APLRAT     aplRatLft,
     APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLRAT aplTmp = {0};

    // Check for indeterminates:  lcm (PoM_, 0)  or  lcm (0, PoM_)
    if ((mpq_inf_p (&aplRatLft) && IsMpq0 (&aplRatRht))
     || (mpq_inf_p (&aplRatRht) && IsMpq0 (&aplRatLft)))
        return *mpq_QuadICValue (&aplRatLft,
                                  ICNDX_0LCMInf,
                                 &aplRatRht,
                                 &aplTmp);
    // Check for special cases:  lcm (PoM_, N)  or  lcm (N, PoM_)
    if (mpq_inf_p (&aplRatLft)
     || mpq_inf_p (&aplRatRht))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    // If the denominators are both 1, ...
    if (IsMpz1 (mpq_denref (&aplRatLft))
     && IsMpz1 (mpq_denref (&aplRatRht)))
    {
        APLRAT mpqRes = {0};

        // Initialize the result
        mpq_init (&mpqRes);

        // Calculate the LCM on the numerators
        mpz_lcm (mpq_numref (&mpqRes),
                 mpq_numref (&aplRatLft),
                 mpq_numref (&aplRatRht));
        // The sign of the result is the sign of the left arg
        if (mpz_sgn (mpq_numref (&aplRatLft)) EQ -1)
            mpz_neg (mpq_numref (&mpqRes), mpq_numref (&mpqRes));
        // Set the denominator to 1
        mpz_set_ui (mpq_denref (&mpqRes), 1);

        return mpqRes;
    } else
        return lcmAplRat (aplRatLft, aplRatRht, lpPrimSpec);
} // End PrimFnDydUpCaretRisRvR


//***************************************************************************
//  $PrimFnDydUpCaretVisVvV
//
//  Primitive scalar function dyadic UpCaret:  V {is} fn V fn V
//***************************************************************************

APLVFP PrimFnDydUpCaretVisVvV
    (APLVFP     aplVfpLft,
     APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLVFP aplTmp = {0};

    // Check for indeterminates:  lcm (PoM_, 0)  or  lcm (0, PoM_)
    if ((mpfr_inf_p (&aplVfpLft) && IsMpf0 (&aplVfpRht))
     || (mpfr_inf_p (&aplVfpRht) && IsMpf0 (&aplVfpLft)))
        return *mpfr_QuadICValue (&aplVfpLft,
                                   ICNDX_0LCMInf,
                                  &aplVfpRht,
                                  &aplTmp);
    // Check for special cases:  lcm (PoM_, N)  or  lcm (N, PoM_)
    if (mpfr_inf_p (&aplVfpLft)
     || mpfr_inf_p (&aplVfpRht))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    return lcmAplVfp (aplVfpLft, aplVfpRht, lpPrimSpec);
} // End PrimFnDydUpCaretVisVvV


//***************************************************************************
//  End of File: pf_ucaret.c
//***************************************************************************
