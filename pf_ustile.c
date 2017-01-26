//***************************************************************************
//  NARS2000 -- Primitive Function -- UpStile
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


#ifndef PROTO
PRIMSPEC PrimSpecUpStile = {
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecUpStileStorageTypeMon,
    &PrimFnMonUpStileAPA_EM,

    &PrimFnMonUpStileBisB,
    NULL,   // &PrimFnMonUpStileBisI, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileBisF, -- Can't happen w/UpStile

////                 IisB,   // Handled via type promotion (to IisI)
    &PrimFnMonUpStileIisI,
    &PrimFnMonUpStileIisF,

////                 FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonUpStileFisI, -- Can't happen w/UpStile
    &PrimFnMonUpStileFisF,

    &PrimFnMonUpStileRisR,

////               VisR,    // Handled via type promotion (to VisV)
    &PrimFnMonUpStileVisV,

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecUpStileStorageTypeDyd,
    NULL,   // &PrimFnDydUpStileAPA_EM, -- Can't happen w/UpStile

    &PrimFnDydDownCaretBisBvB,
    NULL,   // &PrimFnDydUpStileBisIvI, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileBisFvF, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileBisCvC, -- Can't happen w/UpStile

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    &PrimFnDydUpStileIisIvI,
    NULL,   // &PrimFnDydUpStileIisFvF, -- Can't happen w/UpStile

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydUpStileFisIvI, -- Can't happen w/UpStile
    &PrimFnDydUpStileFisFvF,

    NULL,   // &PrimFnDydUpStileBisRvR, -- Can't happen w/UpStile
    &PrimFnDydUpStileRisRvR,

    NULL,   // &PrimFnDydUpStileBisVvV, -- Can't happen w/UpStile
////                  VisRvR,   // Handled via type promotion (to VisVvV)
    &PrimFnDydUpStileVisVvV,

    NULL,   // &PrimFnMonUpStileB64isB64, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileB32isB32, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileB16isB16, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileB08isB08, -- Can't happen w/UpStile

    &PrimFnDydDownCaretB64isB64vB64,
    &PrimFnDydDownCaretB32isB32vB32,
    &PrimFnDydDownCaretB16isB16vB16,
    &PrimFnDydDownCaretB08isB08vB08,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecUpStile};
#endif


//***************************************************************************
//  $PrimFnUpStile_EM_YY
//
//  Primitive function for monadic and dyadic UpStile ("ceiling" and "maximum")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnUpStile_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnUpStile_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_UPSTILE);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnUpStile_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecUpStileStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecUpStileStorageTypeMon
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
        // Except FLOAT goes to INT
        // IisF promotes to FisF as necessary.
        case ARRAY_FLOAT:
            aplTypeRes = ARRAY_FLOAT;

            break;

        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
        case ARRAY_RAT:
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
} // End PrimSpecUpStileStorageTypeMon


//***************************************************************************
//  $PrimFnMonUpStileBisB
//
//  Primitive scalar function monadic UpStile:  B {is} fn B
//***************************************************************************

APLBOOL PrimFnMonUpStileBisB
    (APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanRht;
} // End PrimFnMonUpStileBisB


//***************************************************************************
//  $PrimFnMonUpStileIisI
//
//  Primitive scalar function monadic UpStile:  I {is} fn I
//***************************************************************************

APLINT PrimFnMonUpStileIisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplIntegerRht;
} // End PrimFnMonUpStileIisI


//***************************************************************************
//  $PrimFnMonUpStileIisF
//
//  Primitive scalar function monadic UpStile:  I {is} fn F
//***************************************************************************

APLINT PrimFnMonUpStileIisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for PoM infinity and numbers whose
    //   absolute value is >= 2*53
    if (IsFltInfinity (aplFloatRht)
     || fabs (aplFloatRht) >= Float2Pow53)
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

    // Use the code in DownStile
    return (APLINT) -PrimFnMonDownStileFisF (-aplFloatRht, lpPrimSpec);
} // End PrimFnMonUpStileIisF


//***************************************************************************
//  $PrimFnMonUpStileFisF
//
//  Primitive scalar function monadic UpStile:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonUpStileFisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Use the code in DownStile
    return -PrimFnMonDownStileFisF (-aplFloatRht, lpPrimSpec);
} // End PrimFnMonUpStileFisF


//***************************************************************************
//  $PrimFnMonUpStileRisR
//
//  Primitive scalar function monadic UpStile:  R {is} fn R
//***************************************************************************

APLRAT PrimFnMonUpStileRisR
    (APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLRAT mpqRes = {0},
           mpqTmp = {0};

    // Initialize the temp to 0/1
    mpq_init (&mpqTmp);

    // Negate the temp to use with DownStile
    mpq_neg (&mpqTmp, &aplRatRht);

    // Use the code in DownStile
    mpqRes = PrimFnMonDownStileRisR (mpqTmp, NULL);

    // Negate the result after calling RisR
    mpq_neg (&mpqRes, &mpqRes);

    // We no longer need this storage
    Myq_clear (&mpqTmp);

    return mpqRes;
} // End PrimFnMonUpStileRisR


//***************************************************************************
//  $PrimFnMonUpStileVisV
//
//  Primitive scalar function monadic UpStile:  V {is} fn V
//***************************************************************************

APLVFP PrimFnMonUpStileVisV
    (APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLVFP mpfRes = {0},
           mpfTmp = {0};

    // Initialize the temp to 0
    mpfr_init0 (&mpfTmp);

    // Negate the temp to use with DownStile
    mpfr_neg (&mpfTmp, &aplVfpRht, MPFR_RNDN);

    // Use the code in DownStile
    mpfRes = PrimFnMonDownStileVisV (mpfTmp, NULL);

    // Negate the result after calling VisV
    mpfr_neg (&mpfRes, &mpfRes, MPFR_RNDN);

    // We no longer need this storage
    Myf_clear (&mpfTmp);

    return mpfRes;
} // End PrimFnMonUpStileVisV


//***************************************************************************
//  $PrimFnMonUpStileAPA_EM
//
//  Monadic UpStile, result is APA
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonUpStileAPA_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnMonUpStileAPA_EM
    (LPPL_YYSTYPE lpYYRes,          // Ptr to result token (may be NULL)

     LPTOKEN      lptkFunc,         // Ptr to function token

     HGLOBAL      hGlbRht,          // Right arg handle
     HGLOBAL     *lphGlbRes,        // Ptr to result handle

     APLRANK      aplRankRht,       // Right arg rank

     LPPRIMSPEC   lpPrimSpec)       // Ptr to local PRIMSPEC

{
    DBGENTER;

    // Axis may be anything

    // Copy the HGLOBAL to the result
    *lphGlbRes = CopySymGlbDirAsGlb (hGlbRht);

    // Fill in the result token
    if (lpYYRes)
    {
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = hGlbRht;
    } // End IF

    DBGLEAVE;

    return TRUE;
} // End PrimFnMonUpStileAPA_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecUpStileStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecUpStileStorageTypeDyd
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
} // End PrimSpecUpStileStorageTypeDyd


//***************************************************************************
//  $PrimFnDydUpStileIisIvI
//
//  Primitive scalar function dyadic UpStile:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydUpStileIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return max (aplIntegerLft, aplIntegerRht);
} // End PrimFnDydUpStileIisIvI


//***************************************************************************
//  $PrimFnDydUpStileFisFvF
//
//  Primitive scalar function dyadic UpStile:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydUpStileFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    return max (aplFloatLft, aplFloatRht);
} // End PrimFnDydUpStileFisFvF


//***************************************************************************
//  $PrimFnDydUpStileRisRvR
//
//  Primitive scalar function dyadic UpStile:  R {is} R fn R
//***************************************************************************

APLRAT PrimFnDydUpStileRisRvR
    (APLRAT     aplRatLft,
     APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLRAT mpqRes = {0};

    // Compare the two Rationals
    if (mpq_cmp (&aplRatLft, &aplRatRht) > 0)
        mpq_init_set (&mpqRes, &aplRatLft);
    else
        mpq_init_set (&mpqRes, &aplRatRht);

    return mpqRes;
} // End PrimFnDydUpStileRisRvR


//***************************************************************************
//  $PrimFnDydUpStileVisVvV
//
//  Primitive scalar function dyadic UpStile:  V {is} V fn V
//***************************************************************************

APLVFP PrimFnDydUpStileVisVvV
    (APLVFP     aplVfpLft,
     APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLVFP mpfRes = {0};

    // Compare the two Variable FPs
    if (mpfr_cmp (&aplVfpLft, &aplVfpRht) > 0)
        mpfr_init_copy (&mpfRes, &aplVfpLft);
    else
        mpfr_init_copy (&mpfRes, &aplVfpRht);

    return mpfRes;
} // End PrimFnDydUpStileVisVvV


//***************************************************************************
//  End of File: pf_ustile.c
//***************************************************************************
