//***************************************************************************
//  NARS2000 -- Primitive Function -- Stile
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
PRIMSPEC PrimSpecStile = {
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecStileStorageTypeMon,
    NULL,   // &PrimFnMonStileAPA_EM, -- Can't happen w/Stile

    &PrimFnMonStileBisB,
    NULL,   // &PrimFnMonStileBisI, -- Can't happen w/Stile
    NULL,   // &PrimFnMonStileBisF, -- Can't happen w/Stile

////               IisB,   // Handled via type promotion (to IisI)
    &PrimFnMonStileIisI,
    NULL,   // &PrimFnMonStileIisF, -- Can't happen w/Stile

////               FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonStileFisI, -- Can't happen w/Stile
    &PrimFnMonStileFisF,

    &PrimFnMonStileRisR,

////               VisR,   // Handled via type promotion (to VisV)
    &PrimFnMonStileVisV,

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecStileStorageTypeDyd,
    NULL,   // &PrimFnDydStileAPA_EM, -- Can't happen w/Stile

    &PrimFnDydLeftCaretBisBvB,
    NULL,   // &PrimFnDydStileBisIvI, -- Can't happen w/Stile
    NULL,   // &PrimFnDydStileBisFvF, -- Can't happen w/Stile
    NULL,   // &PrimFnDydStileBisCvC, -- Can't happen w/Stile

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    &PrimFnDydStileIisIvI,
    NULL,   // &PrimFnDydStileIisFvF, -- Can't happen w/Stile

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydStileFisIvI, -- Can't happen w/Stile
    &PrimFnDydStileFisFvF,

    NULL,   // &PrimFnDydStileBisRvR, -- Can't happen w/Stile
    &PrimFnDydStileRisRvR,

    NULL,   // &PrimFnDydStileBisVvV, -- Can't happen w/Stile
////                 VisRvR     // Handled via type promotion (to VisVvV)
    &PrimFnDydStileVisVvV,

    NULL,   // &PrimFnMonStileB64isB64, -- Can't happen w/Stile
    NULL,   // &PrimFnMonStileB32isB32, -- Can't happen w/Stile
    NULL,   // &PrimFnMonStileB16isB16, -- Can't happen w/Stile
    NULL,   // &PrimFnMonStileB08isB08, -- Can't happen w/Stile

    &PrimFnDydLeftCaretB64isB64vB64,
    &PrimFnDydLeftCaretB32isB32vB32,
    &PrimFnDydLeftCaretB16isB16vB16,
    &PrimFnDydLeftCaretB08isB08vB08,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecStile};
#endif


//***************************************************************************
//  $PrimFnStile_EM_YY
//
//  Primitive function for monadic and dyadic Stile ("absolute value" and "modulus")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnStile_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnStile_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (IsAPLCharStile (lptkFunc->tkData.tkChar));

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnStile_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecStileStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecStileStorageTypeMon
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

    // Except APA becomes INT
    if (IsSimpleAPA (aplTypeRes))
        aplTypeRes = ARRAY_INT;

    return aplTypeRes;
} // End PrimSpecStileStorageTypeMon


//***************************************************************************
//  $PrimFnMonStileBisB
//
//  Primitive scalar function monadic Stile:  B {is} fn B
//***************************************************************************

APLBOOL PrimFnMonStileBisB
    (APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanRht;
} // End PrimFnMonStileBisB


//***************************************************************************
//  $PrimFnMonStileIisI
//
//  Primitive scalar function monadic Stile:  I {is} fn I
//***************************************************************************

APLINT PrimFnMonStileIisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    if (aplIntegerRht < 0)
        return -aplIntegerRht;
    else
        return  aplIntegerRht;
} // End PrimFnMonStileIisI


//***************************************************************************
//  $PrimFnMonStileFisF
//
//  Primitive scalar function monadic Stile:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonStileFisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    return fabs (aplFloatRht);
} // End PrimFnMonStileFisF


//***************************************************************************
//  $PrimFnMonStileRisR
//
//  Primitive scalar function monadic Stile:  R {is} fn R
//***************************************************************************

APLRAT PrimFnMonStileRisR
    (APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLRAT mpqRes = {0};

    // Initialize the result
    mpq_init (&mpqRes);

    // Calculate the absolute value of a Rational
    mpq_abs (&mpqRes, &aplRatRht);

    return mpqRes;
} // End PrimFnMonStileRisR


//***************************************************************************
//  $PrimFnMonStileVisV
//
//  Primitive scalar function monadic Stile:  V {is} fn V
//***************************************************************************

APLVFP PrimFnMonStileVisV
    (APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLVFP mpfRes = {0};

    // Initialize the result
    mpfr_init0 (&mpfRes);

    // Calculate the absolute value of a Variable FP
    mpfr_abs (&mpfRes, &aplVfpRht, MPFR_RNDN);

    return mpfRes;
} // End PrimFnMonStileVisV


//***************************************************************************
//  $PrimSpecStileStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecStileStorageTypeDyd
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
} // End PrimSpecStileStorageTypeDyd


//***************************************************************************
//  $PrimFnDydStileIisIvI
//
//  Primitive scalar function dyadic Stile:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydStileIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLINT aplTmp, aplLft, aplRht;

    // Handle zero modulus or argument
    if (aplIntegerLft EQ 0
     || aplIntegerRht EQ 0)
        return aplIntegerRht;

    // Ensure both arguments are non-negative
    aplLft = PrimFnMonStileIisI (aplIntegerLft, lpPrimSpec);
    aplRht = PrimFnMonStileIisI (aplIntegerRht, lpPrimSpec);

    aplTmp = aplRht % aplLft;

    // If the arguments are of opposite sign
    //   and the result so far is non-zero,
    //   replace the result with its complement
    //   in the modulus.
    if ((aplIntegerLft > 0) NE (aplIntegerRht > 0)
     && aplTmp NE 0)
        aplTmp = aplLft - aplTmp;

    // The sign of the result is the sign of the left arg
    if (aplIntegerLft < 0)
        return -aplTmp;
    else
        return  aplTmp;
} // End PrimFnDydStileIisIvI


//***************************************************************************
//  $PrimFnDydStileFisFvF
//
//  Primitive scalar function dyadic Stile:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydStileFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplTmp, aplLft, aplRht,
             fQuadCT;       // []CT
    UINT     bRet = TRUE;

    // Handle zero modulus or argument
    if (aplFloatLft EQ 0
     || aplFloatRht EQ 0)
        // Return the right arg
        return aplFloatRht;
    else
    // If the right arg is negative infinity, ...
    if (IsNegInfinity (aplFloatRht))
    {
        // If the left arg is positive, ...
        if (aplFloatLft > 0)
            return TranslateQuadICIndex (aplFloatLft,
                                         ICNDX_PosMODNi,
                                         aplFloatRht);
        // If the left arg is negative, ...
        if (aplFloatLft < 0)
            return TranslateQuadICIndex (aplFloatLft,
                                         ICNDX_NegMODNi,
                                         aplFloatRht);
    } else
    // If the right arg is positive infinity, ...
    if (IsPosInfinity (aplFloatRht))
    {
        // If the left arg is positive, ...
        if (aplFloatLft > 0)
            return TranslateQuadICIndex (aplFloatLft,
                                         ICNDX_PosMODPi,
                                         aplFloatRht);
        // If the left arg is negative, ...
        if (aplFloatLft < 0)
            return TranslateQuadICIndex (aplFloatLft,
                                         ICNDX_NegMODPi,
                                         aplFloatRht);
    } else
    // If the left arg is negative infinity, ...
    if (IsNegInfinity (aplFloatLft))
    {
        // If the right arg is positive, ...
        if (aplFloatRht > 0)
            return TranslateQuadICIndex (aplFloatLft,
                                         ICNDX_NiMODPos,
                                         aplFloatRht);
        // Return the right arg
        return aplFloatRht;
    } else
    // If the left arg is positive infinity, ...
    if (IsNegInfinity (aplFloatLft))
    {
        // If the right arg is negative, ...
        if (aplFloatRht < 0)
            return TranslateQuadICIndex (aplFloatLft,
                                         ICNDX_PiMODNeg,
                                         aplFloatRht);
        // Return the right arg
        return aplFloatRht;
    } // End IF

    // Ensure both arguments are non-negative
    aplLft = PrimFnMonStileFisF (aplFloatLft, lpPrimSpec);
    aplRht = PrimFnMonStileFisF (aplFloatRht, lpPrimSpec);

    // Get the current value of []CT
    fQuadCT = GetQuadCT ();

    // Calculate right divided-by left
    aplTmp = aplRht / aplLft;

    // If Rht divided-by Lft is near an integer within CT
    //   return 0.
    if (_CompareCT (aplTmp, floor (aplTmp), fQuadCT, NULL, TRUE))
        return 0;

    if (_CompareCT (aplTmp, ceil  (aplTmp), fQuadCT, NULL, TRUE))
        return 0;

    // Calculate the modulus
    aplTmp = fmod (aplRht, aplLft);

    // Due to differences in fmod and APL's mod as to how they treat signed args, ...
    // If the arguments are of opposite sign
    //   and the result so far is non-zero,
    //   replace the result with its complement
    //   in the modulus.
    if (SIGN_APLFLOAT (aplFloatLft) NE SIGN_APLFLOAT (aplFloatRht)
     && aplTmp NE 0)
        aplTmp = aplLft - aplTmp;

    // The sign of the result is the sign of the left arg
    if (aplFloatLft < 0)
        return -aplTmp;
    else
        return  aplTmp;
} // End PrimFnDydStileFisFvF


//***************************************************************************
//  $PrimFnDydStileRisRvR
//
//  Primitive scalar function dyadic Stile:  R {is} R fn R
//***************************************************************************

APLRAT PrimFnDydStileRisRvR
    (APLRAT     aplRatLft,
     APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLRAT mpqRes = {0};

    // Handle zero modulus or argument
    if (IsMpq0 (&aplRatLft)
     || IsMpq0 (&aplRatRht))
        // Return the right arg
        mpq_init_set (&mpqRes, &aplRatRht);
    else
    // If the right arg is negative infinity, ...
    if (IsMpqNegInfinity (&aplRatRht))
    {
        // If the left arg is positive, ...
        if (mpq_sgn (&aplRatLft) > 0)
            return *mpq_QuadICValue (&aplRatLft,
                                      ICNDX_PosMODNi,
                                     &aplRatRht,
                                     &mpqRes);
        // If the left arg is negative, ...
        if (mpq_sgn (&aplRatLft) < 0)
            return *mpq_QuadICValue (&aplRatLft,
                                      ICNDX_NegMODNi,
                                     &aplRatRht,
                                     &mpqRes);
    } else
    // If the right arg is positive infinity, ...
    if (IsMpqPosInfinity (&aplRatRht))
    {
        // If the left arg is positive, ...
        if (mpq_sgn (&aplRatLft) > 0)
            return *mpq_QuadICValue (&aplRatLft,
                                      ICNDX_PosMODPi,
                                     &aplRatRht,
                                     &mpqRes);
        // If the left arg is negative, ...
        if (mpq_sgn (&aplRatLft) < 0)
            return *mpq_QuadICValue (&aplRatLft,
                                      ICNDX_NegMODPi,
                                     &aplRatRht,
                                     &mpqRes);
    } else
    // If the left arg is negative infinity, ...
    if (IsMpqNegInfinity (&aplRatLft))
    {
        // If the right arg is positive, ...
        if (mpq_sgn (&aplRatRht) > 0)
            return *mpq_QuadICValue (&aplRatLft,
                                      ICNDX_NiMODPos,
                                     &aplRatRht,
                                     &mpqRes);
        // Initialize the result with the right arg
        mpq_init_set (&mpqRes, &aplRatRht);
    } else
    // If the left arg is positive infinity, ...
    if (IsMpqPosInfinity (&aplRatLft))
    {
        // If the right arg is negative, ...
        if (mpq_sgn (&aplRatRht) < 0)
            return *mpq_QuadICValue (&aplRatLft,
                                      ICNDX_PiMODNeg,
                                     &aplRatRht,
                                     &mpqRes);
        // Initialize the result with the right arg
        mpq_init_set (&mpqRes, &aplRatRht);
    } else
    {
#ifdef RAT_EXACT
        // Initialize the result to 0/1
        mpq_init (&mpqRes);

        // Calculate the residue
        mpq_mod (&mpqRes, &aplRatRht, &aplRatLft);
#else
        APLRAT   aplLft = {0},
                 aplRht = {0},
                 aplTmp = {0};
        APLFLOAT fQuadCT;

        // Initialize the temps & result
        mpq_init (&aplLft);
        mpq_init (&aplRht);
        mpq_init (&aplTmp);
        mpq_init (&mpqRes);

        // Ensure both arguments are non-negative
        mpq_abs (&aplLft, &aplRatLft);
        mpq_abs (&aplRht, &aplRatRht);

        // Get the current value of []CT
        fQuadCT = GetQuadCT ();

        // Calculate right divided-by left
        mpq_div (&aplTmp, &aplRht, &aplLft);

        // If Rht divided-by Lft is near an integer within CT
        //   return 0.
        mpq_floor (&aplLft, &aplTmp);
        mpq_ceil  (&aplRht, &aplTmp);
        if (mpq_cmp_ct (aplTmp, aplLft, fQuadCT) NE 0
         && mpq_cmp_ct (aplTmp, aplRht, fQuadCT) NE 0)
            // Calculate the residue
            mpq_mod (&mpqRes, &aplRatRht, &aplRatLft);

        // We no longer need this storage
        Myq_clear (&aplTmp);
        Myq_clear (&aplRht);
        Myq_clear (&aplLft);
#endif
    } // End IF/ELSE/...

    return mpqRes;
} // End PrimFnDydStileRisRvR


//***************************************************************************
//  $PrimFnDydStileVisVvV
//
//  Primitive scalar function dyadic Stile:  V {is} V fn V
//***************************************************************************

APLVFP PrimFnDydStileVisVvV
    (APLVFP     aplVfpLft,
     APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLVFP mpfRes = {0};

    // Handle zero modulus or argument
    if (IsMpf0 (&aplVfpLft)
     || IsMpf0 (&aplVfpRht))
        // Return the right arg
        mpfr_init_copy (&mpfRes, &aplVfpRht);
    else
    // If the right arg is negative infinity, ...
    if (IsMpfNegInfinity (&aplVfpRht))
    {
        // If the left arg is positive, ...
        if (mpfr_sgn (&aplVfpLft) > 0)
            return *mpfr_QuadICValue (&aplVfpLft,
                                       ICNDX_PosMODNi,
                                      &aplVfpRht,
                                      &mpfRes);
        // If the left arg is negative, ...
        if (mpfr_sgn (&aplVfpLft) < 0)
            return *mpfr_QuadICValue (&aplVfpLft,
                                       ICNDX_NegMODNi,
                                      &aplVfpRht,
                                      &mpfRes);
    } else
    // If the right arg is positive infinity, ...
    if (IsMpfPosInfinity (&aplVfpRht))
    {
        // If the left arg is positive, ...
        if (mpfr_sgn (&aplVfpLft) > 0)
            return *mpfr_QuadICValue (&aplVfpLft,
                                       ICNDX_PosMODPi,
                                      &aplVfpRht,
                                      &mpfRes);
        // If the left arg is negative, ...
        if (mpfr_sgn (&aplVfpLft) < 0)
            return *mpfr_QuadICValue (&aplVfpLft,
                                       ICNDX_NegMODPi,
                                      &aplVfpRht,
                                      &mpfRes);
    } else
    // If the left arg is negative infinity, ...
    if (IsMpfNegInfinity (&aplVfpLft))
    {
        // If the right arg is positive, ...
        if (mpfr_sgn (&aplVfpRht) > 0)
            return *mpfr_QuadICValue (&aplVfpLft,
                                       ICNDX_NiMODPos,
                                      &aplVfpRht,
                                      &mpfRes);
        // Initialize the result with the right arg
        mpfr_init_copy (&mpfRes, &aplVfpRht);
    } else
    // If the left arg is positive infinity, ...
    if (IsMpfPosInfinity (&aplVfpLft))
    {
        // If the right arg is negative, ...
        if (mpfr_sgn (&aplVfpRht) < 0)
            return *mpfr_QuadICValue (&aplVfpLft,
                                       ICNDX_PiMODNeg,
                                      &aplVfpRht,
                                      &mpfRes);
        // Initialize the result with the right arg
        mpfr_init_copy (&mpfRes, &aplVfpRht);
    } else
    {
        // Initialize the result
        mpfr_init0 (&mpfRes);

        // Calculate the residue
        mpfr_mod (&mpfRes, &aplVfpRht, &aplVfpLft);
    } // End IF/ELSE/...

    return mpfRes;
} // End PrimFnDydStileVisVvV


//***************************************************************************
//  End of File: pf_stile.c
//***************************************************************************
