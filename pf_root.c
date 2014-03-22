//***************************************************************************
//  NARS2000 -- Primitive Function -- Root
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
PRIMSPEC PrimSpecRoot = {
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecRootStorageTypeMon,
    NULL,   // &PrimFnMonRootAPA_EM, -- Can't happen w/Root

    &PrimFnMonRootBisB,
    NULL,   // &PrimFnMonRootBisI, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootBisF, -- Can't happen w/Root

////               IisB,   // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonRootIisI, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootIisF, -- Can't happen w/Root

////               FisB,   // Handled via type promotion (to FisI)
    &PrimFnMonRootFisI,
    &PrimFnMonRootFisF,

    NULL,   // &PrimFnMonRootRisR, -- Can't happen w/Root

////               VisR,   // Handled via type promotion (to VisV)
    &PrimFnMonRootVisV,

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecRootStorageTypeDyd,
    NULL,   // &PrimFnDydRootAPA_EM, -- Can't happen w/Root

    &PrimFnDydRootBisBvB,
    NULL,   // &PrimFnDydRootBisIvI, -- Can't happen w/Root
    NULL,   // &PrimFnDydRootBisFvF, -- Can't happen w/Root
    NULL,   // &PrimFnDydRootBisCvC, -- Can't happen w/Root

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    &PrimFnDydRootIisIvI,
    NULL,   // &PrimFnDydRootIisFvF, -- Can't happen w/Root

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    &PrimFnDydRootFisIvI,
    &PrimFnDydRootFisFvF,

    NULL,   // &PrimFnDydRootBisRvR, -- Can't happen w/Root
    NULL,   // &PrimFnDydRootRisRvR, -- Can't happen w/Root

    NULL,   // &PrimFnDydRootBisVvV, -- Can't happen w/Root
////                 VisRvR     // Handled via type promotion (to VisVvV)
    &PrimFnDydRootVisVvV,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecRoot};
#endif


//***************************************************************************
//  $PrimFnRoot_EM_YY
//
//  Primitive function for monadic and dyadic Root ("Square Root" and "Nth Root")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnRoot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnRoot_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_ROOT);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnRoot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecRootStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecRootStorageTypeMon
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

    // Except that APA and INT become FLOAT
    if (IsSimpleAPA (aplTypeRes)
     || IsSimpleInt (aplTypeRes))
        return ARRAY_FLOAT;

    // Except that RAT becomes VFP
    if (IsRat (aplTypeRes))
        return ARRAY_VFP;

    return aplTypeRes;
} // End PrimSpecRootStorageTypeMon


//***************************************************************************
//  $PrimFnMonRootBisB
//
//  Primitive scalar function monadic Root:  B {is} fn B
//***************************************************************************

APLBOOL PrimFnMonRootBisB
    (APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanRht;
} // End PrimFnMonRootBisB


//***************************************************************************
//  $PrimFnMonRootFisI
//
//  Primitive scalar function monadic Root:  F {is} fn I
//***************************************************************************

APLFLOAT PrimFnMonRootFisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for Complex result
    if (aplIntegerRht < 0)
        RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

    return sqrt ((APLFLOAT) aplIntegerRht);
} // End PrimFnMonRootFisI


//***************************************************************************
//  $PrimFnMonRootFisF
//
//  Primitive scalar function monadic Root:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonRootFisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for Complex result
    if (aplFloatRht < 0)
        RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

    return sqrt (aplFloatRht);
} // End PrimFnMonRootFisF


//***************************************************************************
//  $PrimFnMonRootVisV
//
//  Primitive scalar function monadic Root:  V {is} fn V
//***************************************************************************

APLVFP PrimFnMonRootVisV
    (APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLVFP mpfRes = {0};

    // Check for Complex result
    if (mpfr_cmp_ui (&aplVfpRht, 0) < 0)
        RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

    // Initialize the result
    mpfr_init0 (&mpfRes);

    // Extract the square root of a Variable FP
    mpfr_sqrt (&mpfRes, &aplVfpRht, MPFR_RNDN);

    return mpfRes;
} // End PrimFnMonRootVisV


//***************************************************************************
//  $PrimSpecRootStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecRootStorageTypeDyd
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

    // Except that RAT becomes VFP
    if (IsRat (aplTypeRes))
        return ARRAY_VFP;

    return aplTypeRes;
} // End PrimSpecRootStorageTypeDyd


//***************************************************************************
//  $PrimFnDydRootBisBvB
//
//  Primitive scalar function dyadic Root:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydRootBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplFloatRes;

    // Check for indeterminates:  0 {root} 0 <==> 0 * _
    if (aplBooleanLft EQ 0
     && aplBooleanRht EQ 0)
    {
        // See what the []IC oracle has to say
        aplFloatRes = TranslateQuadICIndex ((APLFLOAT) aplBooleanLft,
                                            ICNDX_0EXPPi,
                                            (APLFLOAT) aplBooleanRht);
        // If the result is Boolean
        if (aplFloatRes EQ 0)
            return 0;
        else
        if (aplFloatRes EQ 1)
            return 1;
        else
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } // End IF

    // Check for special case:  1 {root} R <==> R * 1 <==> R
    if (aplBooleanLft EQ 1)
        return aplBooleanRht;
    else
        // It's 0 {root} 1 <==> 1 * _ <==> 1
        return 1;
} // End PrimFnDydRootBisBvB


//***************************************************************************
//  $PrimFnDydRootIisIvI
//
//  Primitive scalar function dyadic Root:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydRootIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplFloatRes;

    // Check for special case:  1 {root} R <==> R * 1 <==> R
    if (aplIntegerLft EQ 1)
        return aplIntegerRht;

    // Check for indeterminates and special cases:  0 {root} R
    if (aplIntegerLft EQ 0)
    {
        // Check for indeterminate:  0 {root} R where R <= -1
        if (aplIntegerRht <= -1)
            aplFloatRes = TranslateQuadICIndex ((APLFLOAT) aplIntegerLft,
                                                ICNDX_NEXPPi,
                                                (APLFLOAT) aplIntegerRht);
        else
////////// Check for special case:  0 {root} R where -1 < R < 0 <==> 0
////////if (-1 < aplIntegerRht
//////// &&      aplIntegerRht < 0)
////////    return 0;
////////else
        // Check for indeterminate:  0 {root} 0 <==> 0 * _
        if (aplIntegerRht EQ 0)
            aplFloatRes = TranslateQuadICIndex ((APLFLOAT) aplIntegerLft,
                                                ICNDX_0EXPPi,
                                                (APLFLOAT) aplIntegerRht);
        else
////////// Check for special case:  0 {root} R where 0 < R < 1
////////if (0 < aplIntegerRht
//////// &&     aplIntegerRht < 1)
////////    return 0;
////////else
////////// Check for special case:  0 {root} R where R > 1 <==> R * _ <==> _
////////if (aplIntegerRht > 1)
////////    RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
////////else
            aplFloatRes = 2;        // Any non-Boolean value

        // If the result is Boolean
        if (aplFloatRes EQ 0)
            return 0;
        else
        if (aplFloatRes EQ 1)
            return 1;
        else
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } // End IF

    // Otherwise, let the FisFvF code handle it
    RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

    return 0;           // To keep the compiler happy
} // End PrimFnDydRootIisIvI


//***************************************************************************
//  $PrimFnDydRootFisIvI
//
//  Primitive scalar function dyadic Root:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydRootFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplFloatRes;

    // Check for indeterminates:  0 * _
    if (aplIntegerLft EQ 0
     && aplIntegerRht EQ 0)
        return TranslateQuadICIndex ((APLFLOAT) aplIntegerLft,
                                     ICNDX_0EXPPi,
                                     (APLFLOAT) aplIntegerRht);

    // Check for special case:  1 {root} R <==> R * 1 <==> R
    if (aplIntegerLft EQ 1)
        return (APLFLOAT) aplIntegerRht;

    // Check for indeterminates and special cases:  0 {root} R
    if (aplIntegerLft EQ 0)
    {
        // Check for indeterminate:  0 {root} R where R <= -1
        if (aplIntegerRht <= -1)
            return TranslateQuadICIndex ((APLFLOAT) aplIntegerLft,
                                         ICNDX_NEXPPi,
                                         (APLFLOAT) aplIntegerRht);

////////// Check for special case:  0 {root} R where -1 < R < 0 <==> 0
////////if (-1 < aplIntegerRht
//////// &&      aplIntegerRht < 0)
////////    return 0;

        // Check for indeterminate:  0 {root} 0 <==> 0 * _
        if (aplIntegerRht EQ 0)
            return TranslateQuadICIndex ((APLFLOAT) aplIntegerLft,
                                         ICNDX_0EXPPi,
                                         (APLFLOAT) aplIntegerRht);

////////// Check for special case:  0 {root} R where 0 < R < 1
////////if (0 < aplIntegerRht
//////// &&     aplIntegerRht < 1)
////////    return 0;

        // Check for special case:  0 {root} R where R > 1 <==> R * _ <==> _
        if (aplIntegerRht > 1)
            return PosInfinity;
    } // End IF

////// Check for indeterminate:  PoM_ {root} 0 <==> 0 * 0
////if (IsInfinity (aplIntegerLft)
//// && aplIntegerRht EQ 0)
////    return TranslateQuadICIndex ((APLFLOAT) aplIntegerLft,
////                                 ICNDX_0EXP0,
////                                 (APLFLOAT) aplIntegerRht);
    // Check for Complex result
    if (aplIntegerRht < 0)
        RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

    // Calculate the root
    aplFloatRes = pow ((APLFLOAT) aplIntegerRht, 1 / (APLFLOAT) aplIntegerLft);

    return aplFloatRes;
} // End PrimFnDydRootFisIvI


//***************************************************************************
//  $PrimFnDydRootFisFvF
//
//  Primitive scalar function dyadic Root:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydRootFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplFloatRes;

    // Check for indeterminates:  0 * _
    if (aplFloatLft EQ 0
     && aplFloatRht EQ 0)
        return TranslateQuadICIndex (aplFloatLft,
                                     ICNDX_0EXPPi,
                                     aplFloatRht);
    // Check for special case:  1 {root} R <==> R * 1 <==> R
    if (aplFloatLft EQ 1)
        return aplFloatRht;

    // Check for indeterminates and special cases:  0 {root} R
    if (aplFloatLft EQ 0)
    {
        // Check for indeterminate:  0 {root} R where R <= -1
        if (aplFloatRht <= -1)
            return TranslateQuadICIndex (aplFloatLft,
                                         ICNDX_NEXPPi,
                                         aplFloatRht);
        // Check for special case:  0 {root} R where -1 < R < 0 <==> 0
        if (-1 < aplFloatRht
         &&      aplFloatRht < 0)
            return 0;

        // Check for indeterminate:  0 {root} 0 <==> 0 * _
        if (aplFloatRht EQ 0)
            return TranslateQuadICIndex (aplFloatLft,
                                         ICNDX_0EXPPi,
                                         aplFloatRht);
        // Check for special case:  0 {root} R where 0 < R < 1
        if (0 < aplFloatRht
         &&     aplFloatRht < 1)
            return 0;

        // Check for special case:  0 {root} R where R > 1 <==> R * _ <==> _
        if (aplFloatRht > 1)
            return PosInfinity;
    } // End IF

    // Check for indeterminate:  PoM_ {root} 0 <==> 0 * 0
    if (IsInfinity (aplFloatLft)
     && aplFloatRht EQ 0)
        return TranslateQuadICIndex (aplFloatLft,
                                     ICNDX_0EXP0,
                                     aplFloatRht);
    // Check for Complex result
    if (aplFloatRht < 0)
        RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

    // Calculate the root
    aplFloatRes = pow (aplFloatRht, 1 / aplFloatLft);

    return aplFloatRes;
} // End PrimFnDydRootFisFvF


//***************************************************************************
//  $PrimFnDydRootVisVvV
//
//  Primitive scalar function dyadic Root:  V {is} V fn V
//***************************************************************************

APLVFP PrimFnDydRootVisVvV
    (APLVFP     aplVfpLft,
     APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLVFP mpfRes = {0},
           mpfTmp;

    // Check for indeterminates:  0 * _
    if (IsMpf0 (&aplVfpLft)
     && IsMpf0 (&aplVfpRht))
        return *mpfr_QuadICValue (&aplVfpLft,
                                   ICNDX_0EXPPi,
                                  &aplVfpRht,
                                  &mpfRes);
    // Check for special case:  1 {root} R <==> R * 1 <==> R
    if (IsMpf1 (&aplVfpLft))
    {
        mpfr_init_copy (&mpfRes, &aplVfpRht);
        return mpfRes;
    } // End IF

    // Check for indeterminates and special cases:  0 {root} R
    if (IsMpf0 (&aplVfpLft))
    {
        // Check for indeterminate:  0 {root} R where R <= -1
        if (mpfr_cmp_si (&aplVfpRht, -1) <= 0)  // R <= -1
            return *mpfr_QuadICValue (&aplVfpLft,
                                       ICNDX_NEXPPi,
                                      &aplVfpRht,
                                      &mpfRes);
        // Check for special case:  0 {root} R where -1 < R < 0 <==> 0
        if (mpfr_si_cmp (-1, &aplVfpRht   ) < 0 // -1 < R
         && mpfr_cmp_si (    &aplVfpRht, 0) < 0)    //      R < 0)
        {
            mpfr_init0 (&mpfRes);
            return mpfRes;
        } // End IF

        // Check for indeterminate:  0 {root} 0 <==> 0 * _
        if (IsMpf0 (&aplVfpRht))
            return *mpfr_QuadICValue (&aplVfpLft,
                                       ICNDX_0EXPPi,
                                      &aplVfpRht,
                                      &mpfRes);
        // Check for special case:  0 {root} R where 0 < R < 1
        if (mpfr_ui_cmp (0, &aplVfpRht   ) < 0  // 0 < R
         && mpfr_cmp_ui (   &aplVfpRht, 1) < 0) //     R < 1
        {
            mpfr_init0 (&mpfRes);
            return mpfRes;
        } // End IF

        // Check for special case:  0 {root} R where R > 1 <==> R * _ <==> _
        if (mpfr_cmp_ui (&aplVfpRht, 1) > 0)    // R > 1
            return mpfPosInfinity;
    } // End IF

    // Check for indeterminate:  PoM_ {root} 0 <==> 0 * 0
    if (mpfr_inf_p (&aplVfpLft)
     && IsMpf0 (&aplVfpRht))
        return *mpfr_QuadICValue (&aplVfpLft,
                                   ICNDX_0EXP0,
                                  &aplVfpRht,
                                  &mpfRes);
    // Check for Complex result
    if (mpfr_cmp_ui (&aplVfpRht, 0) < 0)        // R < 0
        RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

    // Nth root (V) = exp (ln (a) / N)

    // Calculate ln (a)
    mpfTmp = PrimFnMonCircleStarVisV (aplVfpRht, NULL);

    // Divide by N
    mpfr_div (&mpfTmp, &mpfTmp, &aplVfpLft, MPFR_RNDN);

    // Exp that
    mpfRes = PrimFnMonStarVisV (mpfTmp, NULL);

    // We no longer need this storage
    Myf_clear (&mpfTmp);

    return mpfRes;
} // End PrimFnDydRootVisVvV


//***************************************************************************
//  End of File: pf_root.c
//***************************************************************************
