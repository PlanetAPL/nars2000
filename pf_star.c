//***************************************************************************
//  NARS2000 -- Primitive Function -- Star
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
PRIMSPEC PrimSpecStar =
{
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecStarStorageTypeMon,
    NULL,   // &PrimFnMonStarAPA_EM, -- Can't happen w/Star

    NULL,   // &PrimFnMonStarBisB, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarBisI, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarBisF, -- Can't happen w/Star

////               IisB,     // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonStarIisI, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarIisF, -- Can't happen w/Star

////               FisB,     // Handled via type promotion (to FisI)
    &PrimFnMonStarFisI,
    &PrimFnMonStarFisF,

    NULL,   // &PrimFnMonStarRisR, -- Can't happen w/Star

////               VisR,     // Handled via type promotion (to VisV)
    &PrimFnMonStarVisV,

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecStarStorageTypeDyd,
    NULL,   // &PrimFnDydStarAPA_EM, -- Can't happen w/Star

    &PrimFnDydStarBisBvB,
    NULL,   // &PrimFnDydStarBisIvI, -- Can't happen w/Star
    NULL,   // &PrimFnDydStarBisFvF, -- Can't happen w/Star
    NULL,   // &PrimFnDydStarBisCvC, -- Can't happen w/Star

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    &PrimFnDydStarIisIvI,
    NULL,   // &PrimFnDydStarIisFvF, -- Can't happen w/Star

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    &PrimFnDydStarFisIvI,
    &PrimFnDydStarFisFvF,

    NULL,   // &PrimFnDydStarBisRvR, -- Can't happen w/Star
    &PrimFnDydStarRisRvR,

    NULL,   // &PrimFnDydStarBisVvV, -- Can't happen w/Star
////                 VisRvR     // Handled via type promotion (to VisVvV)
    &PrimFnDydStarVisVvV,

    NULL,   // &PrimFnMonStarB64isB64, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarB32isB32, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarB16isB16, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarB08isB08, -- Can't happen w/Star

    &PrimFnDydStarB64isB64vB64,
    &PrimFnDydStarB32isB32vB32,
    &PrimFnDydStarB16isB16vB16,
    &PrimFnDydStarB08isB08vB08,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecStar};
#endif


//***************************************************************************
//  $PrimFnStar_EM_YY
//
//  Primitive function for monadic and dyadic Star ("exponential" and "power")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnStar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnStar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (IsAPLCharStar (lptkFunc->tkData.tkChar));

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnStar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecStarStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecStarStorageTypeMon
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
        case ARRAY_APA:
            aplTypeRes = ARRAY_FLOAT;

            break;

        // Except that RAT becomes VFP
        case ARRAY_RAT:
            aplTypeRes = ARRAY_VFP;

            break;

        case ARRAY_FLOAT:
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
} // End PrimSpecStarStorageTypeMon


//***************************************************************************
//  $PrimFnMonStarFisI
//
//  Primitive scalar function monadic Star:  F {is} fn I
//***************************************************************************

APLFLOAT PrimFnMonStarFisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return exp ((APLFLOAT) aplIntegerRht);
} // End PrimFnMonStarFisI


//***************************************************************************
//  $PrimFnMonStarFisF
//
//  Primitive scalar function monadic Star:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonStarFisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    return exp (aplFloatRht);
} // End PrimFnMonStarFisF


//***************************************************************************
//  $PrimFnMonStarVisV
//
//  Primitive scalar function monadic Star:  V {is} fn V
//
//  See "http://www.jsoftware.com/jwiki/Essays/Extended%20Precision%20Functions"
//***************************************************************************

APLVFP PrimFnMonStarVisV
    (APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
#ifdef OWN_EXPLOG
    APLVFP   mpfRes  = {0},
             mpfTmp1 = {0},
             mpfTmp2 = {0},
             mpfLn2;
    APLINT   log2x;
    LPPERTABDATA   lpMemPTD;        // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();
#else
    APLVFP mpfRes = {0};
#endif
    // Check for special case:  ^-_
    if (IsMpfNegInfinity (&aplVfpRht))
    {
        // Initialize the result to zero
        mpfr_init0 (&mpfRes);

        return mpfRes;
    } // End IF
#ifdef OWN_EXPLOG
    // Initialize the result
    mpfr_init_copy (&mpfRes, &aplVfpRht);
    mpfr_init0 (&mpfTmp1);
    mpfr_init0 (&mpfTmp2);

    // First, represent exp (V) as (2 ^ m) * exp (V - m * log (2))
    //   and m = floor (0.5 + V/log (2)) which bounds
    //   |V - m * log(2) to be less than 0.5 * log (2)
////log2x = (APLINT) (floor (0.5 + (mpfr_get_d (&aplVfpRht) / log (2.0))));

    // Get the log of 2 ...
    mpfr_const_log2 (&mpfLn2, MPFR_RNDN);

    // Divide V by log of 2
    mpfr_div (&mpfTmp1, &aplVfpRht, &mpfLn2, MPFR_RNDN);

    // Add 0.5
    mpfr_set_d (&mpfTmp2, 0.5, MPFR_RNDN);
    mpfr_add   (&mpfTmp1, &mpfTmp1, &mpfTmp2, MPFR_RNDN);

    // Floor
    mpfr_floor (&mpfTmp1, &mpfTmp1);

    log2x = mpfr_get_si (&mpfTmp1);

    // Times m
    mpfr_mul_ui (&mpfTmp1, &mpfLn2, abs ((int) log2x), MPFR_RNDN);
    if (log2x < 0)
        mpfr_neg (&mpfTmp1, &mpfTmp1, MPFR_RNDN);
    // Subtract from V to yield V - m * log (2)
    mpfr_sub (&mpfRes, &mpfRes, &mpfTmp1, MPFR_RNDN);

    // Calculate the exp of mpfRes via the power series 4.2.1 in Abramowitz & Stegun
    Myf_clear (&mpfTmp2);
    mpfTmp2 = ExpVfp (mpfRes);

    // Copy to the result
    mpfr_copy (&mpfRes, &mpfTmp2);

    // Finally, convert the result back to normal
    //   by multiplying it by 2 ^ m.
    // Split cases based upon the signum of m
    switch (signumint (log2x))
    {
        case -1:
            mpfr_div_2exp (&mpfRes, &mpfRes, (int) -log2x);

            break;

        case  0:
            break;

        case  1:
            mpfr_mul_2exp (&mpfRes, &mpfRes, (int)  log2x);

            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this storage
    Myf_clear (&mpfTmp2);
    Myf_clear (&mpfLn2 );
    Myf_clear (&mpfTmp1);

    return mpfRes;
#else
#ifdef DEBUG
    mpfr_buildopt_tls_p ();
#endif
    // Convert the data to mpfr-format
    mpfr_init0 (&mpfRes);

    // Calculate the function
    mpfr_exp  (&mpfRes, &aplVfpRht, MPFR_RNDN);

    return mpfRes;
#endif
} // End PrimFnMonStarVisV

#ifdef OWN_EXPLOG
//***************************************************************************
//  $ExpVfp
//
//  Calculate the exp of a VFP where (|V) < 0.5 * log (2)
//
//  See "http://www.jsoftware.com/jwiki/Essays/Extended%20Precision%20Functions"
//***************************************************************************

APLVFP ExpVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes   = {0},      // VFP result
           mpfBase  = {0};      // ... base
    UINT   uRes;                // Loop counter

    // exp (z) = 1 + (z/!1) + ((z^2)/!2) + ((z^3)/!3) + ...

    // Initialize the result to 0
    mpfr_init_set_ui (&mpfRes, 0);

    // Initialize the base to 1
    mpfr_init_set_ui (&mpfBase, 1);

    // Loop through the # terms
    for (uRes = 0 ; uRes < nDigitsFPC; uRes++)
    {
        // Accumulate the base into the result
        mpfr_add (&mpfRes, &mpfRes, &mpfBase, MPFR_RNDN);

        // Multiply the base by z / (uRes + 1)
        mpfr_mul    (&mpfBase, &mpfBase, &aplVfpRht, MPFR_RNDN);
        mpfr_div_ui (&mpfBase, &mpfBase, uRes + 1, MPFR_RNDN);
    } // End FOR

    // We no longer need this storage
    Myf_clear (&mpfBase);

    return mpfRes;
} // End ExpVfp
#endif

//***************************************************************************
//  $PrimSpecStarStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecStarStorageTypeDyd
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
} // End PrimSpecStarStorageTypeDyd


//***************************************************************************
//  $PrimFnDydStarBisBvB
//
//  Primitive scalar function dyadic Star:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydStarBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplFloatRes;

    // Check for indeterminates:  0 * 0
    if (aplBooleanLft EQ 0
     && aplBooleanRht EQ 0)
    {
        // See what the []IC oracle has to say
        aplFloatRes =
          TranslateQuadICIndex ((APLFLOAT) aplBooleanLft,
                                           ICNDX_0EXP0,
                                (APLFLOAT) aplBooleanRht,
                                           FALSE);
        // If the result is Boolean
        if (aplFloatRes EQ 0)
            return 0;
        else
        if (aplFloatRes EQ 1)
            return 1;
        else
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } // End IF

    return (aplBooleanLft >= aplBooleanRht);
} // End PrimFnDydStarBisBvB


//***************************************************************************
//  $PrimFnDydStarB64isB64vB64
//
//  Primitive scalar function dyadic Star:  B64 {is} B64 fn B64
//***************************************************************************

APLB64 PrimFnDydStarB64isB64vB64
    (APLB64     aplBooleanLft,
     APLB64     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplFloatRes;

    // Check for indeterminates:  0 * 0
    if (aplBooleanLft EQ 0
     && aplBooleanRht EQ 0)
    {
        // See what the []IC oracle has to say
        aplFloatRes =
          TranslateQuadICIndex ((APLFLOAT) aplBooleanLft,
                                           ICNDX_0EXP0,
                                (APLFLOAT) aplBooleanRht,
                                           FALSE);
        // If the result is Boolean
        if (aplFloatRes EQ 0)
            return 0x0000000000000000;
        else
        if (aplFloatRes EQ 1)
            return 0xFFFFFFFFFFFFFFFF;
        else
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } // End IF

    return ~((~aplBooleanLft) & aplBooleanRht);
} // End PrimFnDydStarB64isB64vB64


//***************************************************************************
//  $PrimFnDydStarB32isB32vB32
//
//  Primitive scalar function dyadic Star:  B32 {is} B32 fn B32
//***************************************************************************

APLB32 PrimFnDydStarB32isB32vB32
    (APLB32     aplBooleanLft,
     APLB32     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplFloatRes;

    // Check for indeterminates:  0 * 0
    if (aplBooleanLft EQ 0
     && aplBooleanRht EQ 0)
    {
        // See what the []IC oracle has to say
        aplFloatRes =
          TranslateQuadICIndex ((APLFLOAT) aplBooleanLft,
                                           ICNDX_0EXP0,
                                (APLFLOAT) aplBooleanRht,
                                           FALSE);
        // If the result is Boolean
        if (aplFloatRes EQ 0)
            return 0x00000000;
        else
        if (aplFloatRes EQ 1)
            return 0xFFFFFFFF;
        else
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } // End IF

    return ~((~aplBooleanLft) & aplBooleanRht);
} // End PrimFnDydStarB32isB32vB32


//***************************************************************************
//  $PrimFnDydStarB16isB16vB16
//
//  Primitive scalar function dyadic Star:  B16 {is} B16 fn B16
//***************************************************************************

APLB16 PrimFnDydStarB16isB16vB16
    (APLB16     aplBooleanLft,
     APLB16     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplFloatRes;

    // Check for indeterminates:  0 * 0
    if (aplBooleanLft EQ 0
     && aplBooleanRht EQ 0)
    {
        // See what the []IC oracle has to say
        aplFloatRes =
          TranslateQuadICIndex ((APLFLOAT) aplBooleanLft,
                                           ICNDX_0EXP0,
                                (APLFLOAT) aplBooleanRht,
                                           FALSE);
        // If the result is Boolean
        if (aplFloatRes EQ 0)
            return 0x0000;
        else
        if (aplFloatRes EQ 1)
            return 0xFFFF;
        else
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } // End IF

    return ~((~aplBooleanLft) & aplBooleanRht);
} // End PrimFnDydStarB16isB16vB16


//***************************************************************************
//  $PrimFnDydStarB08isB08vB08
//
//  Primitive scalar function dyadic Star:  B08 {is} B08 fn B08
//***************************************************************************

APLB08 PrimFnDydStarB08isB08vB08
    (APLB08     aplBooleanLft,
     APLB08     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplFloatRes;

    // Check for indeterminates:  0 * 0
    if (aplBooleanLft EQ 0
     && aplBooleanRht EQ 0)
    {
        // See what the []IC oracle has to say
        aplFloatRes =
          TranslateQuadICIndex ((APLFLOAT) aplBooleanLft,
                                           ICNDX_0EXP0,
                                (APLFLOAT) aplBooleanRht,
                                           FALSE);
        // If the result is Boolean
        if (aplFloatRes EQ 0)
            return 0x00;
        else
        if (aplFloatRes EQ 1)
            return 0xFF;
        else
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } // End IF

    return ~((~aplBooleanLft) & aplBooleanRht);
} // End PrimFnDydStarB08isB08vB08


//***************************************************************************
//  $PrimFnDydStarIisIvI
//
//  Primitive scalar function dyadic Star:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydStarIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLINT aplIntegerRes;

    // Check for indeterminates:  0 * 0
    if (aplIntegerLft EQ 0
     && aplIntegerRht EQ 0)
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

    // Check for special cases:  (|1) * R
    if (abs64 (aplIntegerLft) EQ 1)
    {
        // If the right arg is odd, return the left arg (1 or -1)
        if (abs64 (aplIntegerRht) % 2)
            return aplIntegerLft;
        else
        // Otherwise return 1.
            return 1;
    } // End IF

    // Check for special cases:  L * R for R < 0 or R > 62
    if (aplIntegerRht < 0
     || aplIntegerRht > 62)
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

    // Initialize with identity element for multiplication
    aplIntegerRes = 1;
    while (aplIntegerRht--)
        aplIntegerRes = imul64_RE (aplIntegerRes, aplIntegerLft);

    return aplIntegerRes;
} // End PrimFnDydStarIisIvI


//***************************************************************************
//  $PrimFnDydStarFisIvI
//
//  Primitive scalar function dyadic Star:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydStarFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for indeterminates:  0 * 0
    if (aplIntegerLft EQ 0
     && aplIntegerRht EQ 0)
        return TranslateQuadICIndex ((APLFLOAT) aplIntegerLft,
                                                ICNDX_0EXP0,
                                     (APLFLOAT) aplIntegerRht,
                                                FALSE);
    return pow ((APLFLOAT) aplIntegerLft, (APLFLOAT) aplIntegerRht);
} // End PrimFnDydStarFisIvI


//***************************************************************************
//  $PrimFnDydStarFisFvF
//
//  Primitive scalar function dyadic Star:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydStarFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplFloatRes;

    // Check for indeterminates:  0 * 0
    if (aplFloatLft EQ 0
     && aplFloatRht EQ 0)
        return TranslateQuadICIndex (aplFloatLft,
                                     ICNDX_0EXP0,
                                     aplFloatRht,
                                     FALSE);
    // Check for indeterminates:  0 * +_
    if (aplFloatLft EQ 0
     && IsFltPosInfinity (aplFloatRht))
        return TranslateQuadICIndex (aplFloatLft,
                                     ICNDX_0EXPPi,
                                     aplFloatRht,
                                     FALSE);
    // Check for indeterminates:  0 * -_
    if (aplFloatLft EQ 0
     && IsFltNegInfinity (aplFloatRht))
        return TranslateQuadICIndex (aplFloatLft,
                                     ICNDX_0EXPNi,
                                     aplFloatRht,
                                     FALSE);
    // Check for indeterminates:  L * _ for L <= -1
    if (aplFloatLft <= -1
     && IsFltPosInfinity (aplFloatRht))
        return TranslateQuadICIndex (aplFloatLft,
                                     ICNDX_NEXPPi,
                                     aplFloatRht,
                                     FALSE);
    // Check for indeterminates:  L * -_ for -1 <= L < 0
    if (IsFltNegInfinity (aplFloatRht)
     && aplFloatLft >= -1
     && aplFloatLft <   0)
        return TranslateQuadICIndex (aplFloatLft,
                                     ICNDX_N1to0EXPNi,
                                     aplFloatRht,
                                     FALSE);
////// Check for indeterminates:  L * R for L < 0 and R not an integer
////if (aplFloatLft < 0
//// && aplFloatRht NE floor (aplFloatRht))
////////return TranslateQuadICIndex (aplFloatLft,
////////                             ICNDX_NegEXPFrc,
////////                             aplFloatRht,
////////                             FALSE);
    // Check for complex result
    if (SIGN_APLFLOAT (aplFloatLft)
     && aplFloatRht NE floor (aplFloatRht))
        RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

    // Check for special cases:  _ * 0 and -_ * 0
    if (IsFltInfinity (aplFloatLft)
     && aplFloatRht EQ 0)
        return TranslateQuadICIndex (aplFloatLft,
                                     ICNDX_InfEXP0,
                                     aplFloatRht,
                                     FALSE);
    // Check for special cases:  1 * _ and 1 * -_
    if (aplFloatLft EQ 1
     && IsFltInfinity (aplFloatRht))
        return aplFloatLft;

    // Calculate the power
    aplFloatRes = pow (aplFloatLft, aplFloatRht);

    return aplFloatRes;
} // End PrimFnDydStarFisFvF


//***************************************************************************
//  $PrimFnDydStarRisRvR
//
//  Primitive scalar function dyadic Star:  R {is} R fn R
//***************************************************************************

APLRAT PrimFnDydStarRisRvR
    (APLRAT     aplRatLft,
     APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLRAT  mpqRes = {0};
    mpir_ui uRht;

    // Check for indeterminates:  0 * 0
    if (IsMpq0 (&aplRatLft)
     && IsMpq0 (&aplRatRht))
        return *mpq_QuadICValue (&aplRatLft,
                                  ICNDX_0EXP0,
                                 &aplRatRht,
                                 &mpqRes,
                                  FALSE);
    // Check for indeterminates:  0 * +_
    if (IsMpq0 (&aplRatLft)
     && mpq_cmp (&aplRatRht, &mpqPosInfinity) EQ 0)
        return *mpq_QuadICValue (&aplRatLft,
                                  ICNDX_0EXPPi,
                                 &aplRatRht,
                                 &mpqRes,
                                  FALSE);
    // Check for indeterminates:  0 * -_
    if (IsMpq0 (&aplRatLft)
     && mpq_cmp (&aplRatRht, &mpqNegInfinity) EQ 0)
        return *mpq_QuadICValue (&aplRatLft,
                                  ICNDX_0EXPNi,
                                 &aplRatRht,
                                 &mpqRes,
                                  FALSE);
    // Check for indeterminates:  L * _ for L <= -1
    if (mpq_cmp_si (&aplRatLft, -1, 1) <= 0
     && mpq_cmp (&aplRatRht, &mpqPosInfinity) EQ 0)
        return *mpq_QuadICValue (&aplRatLft,
                                  ICNDX_NEXPPi,
                                 &aplRatRht,
                                 &mpqRes,
                                  FALSE);
    // Check for indeterminates:  L * -_ for -1 <= L < 0
    if (mpq_cmp    (&aplRatRht, &mpqNegInfinity) EQ 0
     && mpq_cmp_si (&aplRatLft, -1, 1) >= 0
     && mpq_cmp_si (&aplRatLft,  0, 1) <  0)
        return *mpq_QuadICValue (&aplRatLft,
                                  ICNDX_N1to0EXPNi,
                                 &aplRatRht,
                                 &mpqRes,
                                  FALSE);
////// Check for indeterminates:  L * R for L < 0 and R not an integer
////if (mpq_sgn (&aplRatLft) < 0
//// && !mpq_integer_p (&aplRatRht))
////    return *mpq_QuadICValue (&aplRatLft,
////                              ICNDX_NegEXPFrc,
////                             &aplRatRht,
////                             &mpqRes,
////                              FALSE);
    // Check for complex result
    if (mpq_sgn (&aplRatLft) < 0
     && !mpq_integer_p (&aplRatRht))
        RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

    // Check for special cases:  _ * 0 and -_ * 0
    if (mpq_inf_p (&aplRatLft)
     && IsMpq0 (&aplRatRht))
        return *mpq_QuadICValue (&aplRatLft,
                                  ICNDX_InfEXP0,
                                 &aplRatRht,
                                 &mpqRes,
                                  FALSE);
    // Check for special cases:  1 * _ and 1 * -_
    if (IsMpq1 (&aplRatLft)
     && mpq_inf_p (&aplRatRht))
    {
        mpq_init_set (&mpqRes, &aplRatLft);

        return mpqRes;
    } // End IF

    // If the exponent's denominator is 1,
    //   and the exponent's numerator fits in an UINT, ...
    if (IsMpz1 (mpq_denref (&aplRatRht))
     && mpz_fits_slong_p (mpq_numref (&aplRatRht)) NE 0)
    {
        // Initialize the base
        mpq_init_set (&mpqRes, &aplRatLft);

        // Extract the exponent
        uRht = mpz_get_ui (mpq_numref (&aplRatRht));

        // Compute the powers
        mpz_pow_ui (mpq_numref (&mpqRes), mpq_numref (&mpqRes), uRht);
        mpz_pow_ui (mpq_denref (&mpqRes), mpq_denref (&mpqRes), uRht);

        // If the right arg is negative, ...
        if (mpq_sgn (&aplRatRht) < 0)
            // Invert it
            mpq_inv (&mpqRes, &mpqRes);
    } else
    // If the base is negative, ...
    if (mpq_sgn (&aplRatLft) < 0)
    {
        // If the denominator is even, ...
        if (mpz_even_p (mpq_denref (&aplRatRht)))
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
//      else
//      // If the numerator is even (and the denominator is odd), ...
//      if (mpz_even_p (mpq_numref (&aplRatRht)))
//          // Blowup to VFP positive:  (|L)*R
//          RaiseException (EXCEPTION_RESULT_VFPPOS, 0, 0, NULL);
//      else
//      // The numerator and denominator are both odd
//          // Blowup to VFP negative:  -(|L)*R
//          RaiseException (EXCEPTION_RESULT_VFPNEG, 0, 0, NULL);
    } else
        // Otherwise, blowup to VFP
        RaiseException (EXCEPTION_RESULT_VFP, 0, 0, NULL);

    return mpqRes;
} // End PrimFnDydStarRisRvR


//***************************************************************************
//  $PrimFnDydStarVisVvV
//
//  Primitive scalar function dyadic Star:  V {is} V fn V
//***************************************************************************

APLVFP PrimFnDydStarVisVvV
    (APLVFP     aplVfpLft,
     APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLVFP mpfRes = {0},
           mpfTmp = {0};

    // Check for indeterminates:  0 * 0
    if (IsMpf0 (&aplVfpLft)
     && IsMpf0 (&aplVfpRht))
        return *mpfr_QuadICValue (&aplVfpLft,
                                   ICNDX_0EXP0,
                                  &aplVfpRht,
                                  &mpfRes,
                                   FALSE);
    // Check for indeterminates:  0 * +_
    if (IsMpf0 (&aplVfpLft)
     && mpfr_cmp (&aplVfpRht, &mpfPosInfinity) EQ 0)
        return *mpfr_QuadICValue (&aplVfpLft,
                                   ICNDX_0EXPPi,
                                  &aplVfpRht,
                                  &mpfRes,
                                   FALSE);
    // Check for indeterminates:  0 * -_
    if (IsMpf0 (&aplVfpLft)
     && mpfr_cmp (&aplVfpRht, &mpfNegInfinity) EQ 0)
        return *mpfr_QuadICValue (&aplVfpLft,
                                   ICNDX_0EXPNi,
                                  &aplVfpRht,
                                  &mpfRes,
                                   FALSE);
    // Check for indeterminates:  L * _ for L <= -1
    if (mpfr_cmp_si (&aplVfpLft, -1) <= 0
     && mpfr_cmp (&aplVfpRht, &mpfPosInfinity) EQ 0)
        return *mpfr_QuadICValue (&aplVfpLft,
                                   ICNDX_NEXPPi,
                                  &aplVfpRht,
                                  &mpfRes,
                                   FALSE);
    // Check for indeterminates:  L * -_ for -1 <= L < 0
    if (mpfr_cmp    (&aplVfpRht, &mpfNegInfinity) EQ 0
     && mpfr_cmp_si (&aplVfpLft, -1) >= 0
     && mpfr_cmp_si (&aplVfpLft,  0) <  0)
        return *mpfr_QuadICValue (&aplVfpLft,
                                   ICNDX_N1to0EXPNi,
                                  &aplVfpRht,
                                  &mpfRes,
                                   FALSE);
    // Check for indeterminates:  L * R for L < 0 and R not an integer
////if (mpfr_sgn (&aplVfpLft) < 0
//// && !mpfr_integer_p (&aplVfpRht))
////    return *mpfr_QuadICValue (&aplVfpLft,
////                               ICNDX_NegEXPFrc,
////                              &aplVfpRht,
////                              &mpfRes,
////                               FALSE);
    // Check for complex result
    if (mpfr_sgn (&aplVfpLft) < 0
     && !mpfr_integer_p (&aplVfpRht))
        RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

    // Check for special cases:  _ * 0 and -_ * 0
    if (mpfr_inf_p (&aplVfpLft)
     && IsMpf0 (&aplVfpRht))
        return *mpfr_QuadICValue (&aplVfpLft,
                                   ICNDX_InfEXP0,
                                  &aplVfpRht,
                                  &mpfRes,
                                   FALSE);
    // Check for special cases:  1 * _ and 1 * -_
    if (IsMpf1 (&aplVfpLft)
     && mpfr_inf_p (&aplVfpRht))
        mpfr_init_copy (&mpfRes, &aplVfpLft);
    else
    // If the exponent is an integer, ...
    if (mpfr_integer_p (&aplVfpRht))
    {
        mpz_t mpzTmp = {0};

        // Initialize the result and temp to 0
        mpfr_init0  (&mpfRes);
        mpz_init   ( mpzTmp);

        // Extract the exponent as MPZ
        mpfr_get_z ( mpzTmp, &aplVfpRht, MPFR_RNDN);

        // Compute the power
        mpfr_pow_z (&mpfRes, &aplVfpLft, mpzTmp, MPFR_RNDN);

        // We no longer need this storage
        Myz_clear  ( mpzTmp);
    } else
    {
        // General exp a^z = exp (z * ln (a))

        // Calculate ln (a)
        mpfTmp = PrimFnMonCircleStarVisV (aplVfpLft, lpPrimSpec);

        // Multiply by the exponent
        mpfr_mul (&mpfTmp, &mpfTmp, &aplVfpRht, MPFR_RNDN);

        // Calculate the exp of that
        mpfRes = PrimFnMonStarVisV (mpfTmp, lpPrimSpec);

        // We no longer need this storage
        Myf_clear (&mpfTmp);
    } // End IF/ELSE

    return mpfRes;
} // End PrimFnDydStarVisVvV


//***************************************************************************
//  End of File: pf_star.c
//***************************************************************************
