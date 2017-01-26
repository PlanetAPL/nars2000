//***************************************************************************
//  NARS2000 -- Primitive Function -- ColonBar
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


#ifndef PROTO
PRIMSPEC PrimSpecColonBar =
{
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecColonBarStorageTypeMon,
    NULL,   // &PrimFnMonColonBarAPA_EM, -- Can't happen w/ColonBar

    NULL,   // &PrimFnMonColonBarBisB, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarBisI, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarBisF, -- Can't happen w/ColonBar

////               IisB,     // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonColonBarIisI, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarIisF, -- Can't happen w/ColonBar

////               FisB,     // Handled via type promotion (to FisI)
    &PrimFnMonColonBarFisI,
    &PrimFnMonColonBarFisF,

    &PrimFnMonColonBarRisR,

////               VisR,    // Handled via type promotion (to VisV)
    &PrimFnMonColonBarVisV,

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecColonBarStorageTypeDyd,
    NULL,   // &PrimFnDydColonBarAPA_EM, -- Can't happen w/ColonBar

    &PrimFnDydUpCaretBisBvB,
    NULL,   // &PrimFnDydColonBarBisIvI, -- Can't happen w/ColonBar
    NULL,   // &PrimFnDydColonBarBisFvF, -- Can't happen w/ColonBar
    NULL,   // &PrimFnDydColonBarBisCvC, -- Can't happen w/ColonBar

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    &PrimFnDydColonBarIisIvI,
    NULL,   // &PrimFnDydColonBarIisFvF, -- Can't happen w/ColonBar

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    &PrimFnDydColonBarFisIvI,
    &PrimFnDydColonBarFisFvF,

    NULL,   // &PrimFnDydColonBarBisRvR, -- Can't happen w/ColonBar
    &PrimFnDydColonBarRisRvR,

    NULL,   // &PrimFnDydColonBarBisVvV, -- Can't happen w/ColonBar
////                  VisRvR,   // Handled via type promotion (to VisVvV)
    &PrimFnDydColonBarVisVvV,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecColonBar};
#endif


//***************************************************************************
//  $PrimFnColonBar_EM_YY
//
//  Primitive function for monadic and dyadic ColonBar ("reciprocal" and "division")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnColonBar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnColonBar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_COLONBAR);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnColonBar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecColonBarStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecColonBarStorageTypeMon
    (APLNELM    aplNELMRht,
     LPAPLSTYPE lpaplTypeRht,
     LPTOKEN    lptkFunc)

{
    APLSTYPE aplTypeRes;

    // In case the right arg is an empty char,
    //   change its type to BOOL
    if (IsCharEmpty (*lpaplTypeRht, aplNELMRht))
        *lpaplTypeRht = ARRAY_BOOL;

    // Weed out chars & heteros
    if (IsSimpleCH (*lpaplTypeRht))
        return ARRAY_ERROR;

    // The storage type of the result is
    //   the same as that of the right arg
    aplTypeRes = *lpaplTypeRht;

    // Except that BOOL, INT and APA become FLOAT
    if (IsSimpleInt (aplTypeRes))
        return ARRAY_FLOAT;

    return aplTypeRes;
} // End PrimSpecColonBarStorageTypeMon


//***************************************************************************
//  $PrimFnMonColonBarFisI
//
//  Primitive scalar function monadic ColonBar:  F {is} fn I
//***************************************************************************

APLFLOAT PrimFnMonColonBarFisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for indeterminates:  {div} 0
    if (aplIntegerRht EQ 0)
        return TranslateQuadICIndex (0,
                                     ICNDX_DIV0,
                          (APLFLOAT) aplIntegerRht,
                                     FALSE);
    // The FPU handles overflow and underflow for us
    return (1 / (APLFLOAT) aplIntegerRht);
} // End PrimFnMonColonBarFisI


//***************************************************************************
//  $PrimFnMonColonBarFisF
//
//  Primitive scalar function monadic ColonBar:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonColonBarFisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for indeterminates:  {div} 0
    if (aplFloatRht EQ 0)
        return TranslateQuadICIndex (0,
                                     ICNDX_DIV0,
                                     aplFloatRht,
                                     SIGN_APLFLOAT (aplFloatRht));
    // The FPU handles overflow and underflow for us
    return (1 / aplFloatRht);
} // End PrimFnMonColonBarFisF


//***************************************************************************
//  $PrimFnMonColonBarRisR
//
//  Primitive scalar function monadic ColonBar:  R {is} fn R
//***************************************************************************

APLRAT PrimFnMonColonBarRisR
    (APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLRAT mpqRes = {0};

    // Check for indeterminates:  {div} 0
    if (IsMpq0 (&aplRatRht))
        return *mpq_QuadICValue (&aplRatRht,        // No left arg
                                  ICNDX_DIV0,
                                 &aplRatRht,
                                 &mpqRes,
                                  FALSE);
    // Check for special case:  {div}{neg}{inf}x
    if (IsMpqInfinity (&aplRatRht)
     && mpq_sgn (&aplRatRht) < 0)
        RaiseException (EXCEPTION_RESULT_VFP, 0, 0, NULL);
    else
    {
        // Initialize the result
        mpq_init (&mpqRes);

        // Invert the Rational
        mpq_inv (&mpqRes, &aplRatRht);
    } // End IF/ELSE

    return mpqRes;
} // End PrimFnMonColonBarRisR


//***************************************************************************
//  $PrimFnMonColonBarVisV
//
//  Primitive scalar function monadic ColonBar:  V {is} fn V
//***************************************************************************

APLVFP PrimFnMonColonBarVisV
    (APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLVFP mpfRes = {0};

    // Check for indeterminates:  {div} 0
    if (IsMpf0 (&aplVfpRht))
        return *mpfr_QuadICValue (&aplVfpRht,       // No left arg
                                   ICNDX_DIV0,
                                  &aplVfpRht,
                                  &mpfRes,
                                   SIGN_APLVFP (&aplVfpRht));
    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Invert the Variable FP
    mpfr_inv (&mpfRes, &aplVfpRht, MPFR_RNDN);

    return mpfRes;
} // End PrimFnMonColonBarVisV


//***************************************************************************
//  $PrimSpecColonBarStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecColonBarStorageTypeDyd
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

    // Except that BOOL, INT, and APA become INT
    // If a result doesn't fit in an INT, blow up to FLOAT.
    if (IsSimpleInt (aplTypeRes))
        return ARRAY_INT;

    return aplTypeRes;
} // End PrimSpecColonBarStorageTypeDyd


//***************************************************************************
//  $PrimFnDydColonBarIisIvI
//
//  Primitive scalar function dyadic ColonBar:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydColonBarIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLINT aplIntegerRes;

    // Check for indeterminates:  0 {div} 0
    if (aplIntegerLft EQ 0
     || aplIntegerRht EQ 0)
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

    // Try integer division first
    aplIntegerRes = aplIntegerLft / aplIntegerRht;

    // See if the result is integral
    if (aplIntegerLft NE (aplIntegerRes * aplIntegerRht))
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

    return aplIntegerRes;
} // End PrimFnDydColonBarIisIvI


//***************************************************************************
//  $PrimFnDydColonBarFisIvI
//
//  Primitive scalar function dyadic ColonBar:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydColonBarFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for indeterminates:  0 {div} 0
    if (aplIntegerLft EQ 0
     && aplIntegerRht EQ 0)
        return TranslateQuadICIndex ((APLFLOAT) aplIntegerLft,
                                                ICNDX_0DIV0,
                                     (APLFLOAT) aplIntegerRht,
                                                FALSE);
    // Check for indeterminates:  L {div} 0
    if (aplIntegerRht EQ 0)
        return TranslateQuadICIndex ((APLFLOAT) aplIntegerLft,
                                                ICNDX_DIV0,
                                     (APLFLOAT) aplIntegerRht,
                                                gAllowNeg0
                                             && (aplIntegerLft < 0) NE
                                                (aplIntegerRht < 0));
    // The FPU handles overflow and underflow for us
    return (((APLFLOAT) aplIntegerLft) / (APLFLOAT) aplIntegerRht);
} // End PrimFnDydColonBarFisIvI


//***************************************************************************
//  $PrimFnDydColonBarFisFvF
//
//  Primitive scalar function dyadic ColonBar:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydColonBarFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for indeterminates:  0 {div} 0
    if (aplFloatLft EQ 0
     && aplFloatRht EQ 0)
        return TranslateQuadICIndex (aplFloatLft,
                                     ICNDX_0DIV0,
                                     aplFloatRht,
                                     SIGN_APLFLOAT (aplFloatLft) NE SIGN_APLFLOAT (aplFloatRht));
    // Check for indeterminates:  L {div} 0
    if (aplFloatRht EQ 0)
        return TranslateQuadICIndex (aplFloatLft,
                                     ICNDX_DIV0,
                                     aplFloatRht,
                                     SIGN_APLFLOAT (aplFloatLft) NE SIGN_APLFLOAT (aplFloatRht));
    // Check for indeterminates:  _ {div} _ (same or different signs)
    if (IsFltInfinity (aplFloatLft)
     && IsFltInfinity (aplFloatRht))
    {
        if (SIGN_APLFLOAT (aplFloatLft) EQ SIGN_APLFLOAT (aplFloatRht))
            return TranslateQuadICIndex (aplFloatLft,
                                         ICNDX_PiDIVPi,
                                         aplFloatRht,
                                         FALSE);
        else
            return TranslateQuadICIndex (aplFloatLft,
                                         ICNDX_NiDIVPi,
                                         aplFloatRht,
                                         FALSE);
    } // End IF

    // The FPU handles overflow and underflow for us
    return (aplFloatLft / aplFloatRht);
} // End PrimFnDydColonBarFisFvF


//***************************************************************************
//  $PrimFnDydColonBarRisRvR
//
//  Primitive scalar function dyadic ColonBar:  R {is} R fn R
//***************************************************************************

APLRAT PrimFnDydColonBarRisRvR
    (APLRAT     aplRatLft,
     APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLRAT mpqRes = {0};

    // Check for indeterminates:  0 {div} 0
    if (IsMpq0 (&aplRatLft)
     && IsMpq0 (&aplRatRht))
        return *mpq_QuadICValue (&aplRatLft,
                                  ICNDX_0DIV0,
                                 &aplRatRht,
                                 &mpqRes,
                                  FALSE);
    // Check for indeterminates:  L {div} 0
    if (IsMpq0 (&aplRatRht))
        return *mpq_QuadICValue (&aplRatLft,
                                  ICNDX_DIV0,
                                 &aplRatRht,
                                 &mpqRes,
                                  FALSE);
    // Check for indeterminates:  _ {div} _ (same or different signs)
    if (mpq_inf_p (&aplRatLft)
     && mpq_inf_p (&aplRatRht))
    {
        if (mpq_sgn (&aplRatLft) EQ mpq_sgn (&aplRatRht))
            return *mpq_QuadICValue (&aplRatLft,
                                      ICNDX_PiDIVPi,
                                     &aplRatRht,
                                     &mpqRes,
                                      FALSE);
        else
            return *mpq_QuadICValue (&aplRatLft,
                                      ICNDX_NiDIVPi,
                                     &aplRatRht,
                                     &mpqRes,
                                      FALSE);
    } // End IF

    // Initialize the result
    mpq_init (&mpqRes);

    // Divide the Rationals
    mpq_div (&mpqRes, &aplRatLft, &aplRatRht);

    return mpqRes;
} // End PrimFnDydColonBarRisRvR


//***************************************************************************
//  $PrimFnDydColonBarVisVvV
//
//  Primitive scalar function dyadic ColonBar:  V {is} V fn V
//***************************************************************************

//#define DEBUG_FMT

APLVFP PrimFnDydColonBarVisVvV
    (APLVFP     aplVfpLft,
     APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLVFP mpfRes = {0};
#ifdef DEBUG_FMT
    WCHAR wszTemp[512];
#endif

    // Check for indeterminates:  0 {div} 0
    if (IsMpf0 (&aplVfpLft)
     && IsMpf0 (&aplVfpRht))
        return *mpfr_QuadICValue (&aplVfpLft,
                                   ICNDX_0DIV0,
                                  &aplVfpRht,
                                  &mpfRes,
                                   SIGN_APLVFP (&aplVfpLft) NE
                                   SIGN_APLVFP (&aplVfpRht));
    // Check for indeterminates:  L {div} 0
    if (IsMpf0 (&aplVfpRht))
        return *mpfr_QuadICValue (&aplVfpLft,
                                   ICNDX_DIV0,
                                  &aplVfpRht,
                                  &mpfRes,
                                   SIGN_APLVFP (&aplVfpLft) NE
                                   SIGN_APLVFP (&aplVfpRht));
    // Check for indeterminates:  _ {div} _ (same or different signs)
    if (mpfr_inf_p (&aplVfpLft)
     && mpfr_inf_p (&aplVfpRht))
    {
        if (mpfr_sgn (&aplVfpLft) EQ mpfr_sgn (&aplVfpRht))
            return *mpfr_QuadICValue (&aplVfpLft,
                                       ICNDX_PiDIVPi,
                                      &aplVfpRht,
                                      &mpfRes,
                                       FALSE);
        else
            return *mpfr_QuadICValue (&aplVfpLft,
                                       ICNDX_NiDIVPi,
                                      &aplVfpRht,
                                      &mpfRes,
                                       FALSE);
    } // End IF

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Divide the VFPs
    mpfr_div (&mpfRes, &aplVfpLft, &aplVfpRht, MPFR_RNDN);

#ifdef DEBUG_FMT
    strcpyW (wszTemp, L"L:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], aplVfpLft, 0) = WC_EOS; DbgMsgW (wszTemp);
    strcpyW (wszTemp, L"R:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], aplVfpRht, 0) = WC_EOS; DbgMsgW (wszTemp);
    strcpyW (wszTemp, L"Z:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], mpfRes   , 0) = WC_EOS; DbgMsgW (wszTemp);
#endif

    return mpfRes;
} // End PrimFnDydColonBarVisVvV


//***************************************************************************
//  End of File: pf_colonbar.c
//***************************************************************************
