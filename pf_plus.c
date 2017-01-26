//***************************************************************************
//  NARS2000 -- Primitive Function -- Plus
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
PRIMSPEC PrimSpecPlus =
{
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecPlusStorageTypeMon,
    &PrimFnMonPlusAPA_EM,

    &PrimFnMonPlusBisB,
    NULL,   // &PrimFnMonPlusBisI, -- Can't happen w/Plus
    NULL,   // &PrimFnMonPlusBisF, -- Can't happen w/Plus

////              IisB,     // Handled via type promotion (to IisI)
    &PrimFnMonPlusIisI,
    NULL,   // &PrimFnMonPlusIisF, -- Can't happen w/Plus

////              FisB,     // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonPlusFisI, -- Can't happen w/Plus
    &PrimFnMonPlusFisF,

    &PrimFnMonPlusRisR,

////               VisR,    // Handled via type promotion (to VisV)
    &PrimFnMonPlusVisV,

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecPlusStorageTypeDyd,
    &PrimFnDydPlusAPA_EM,

    NULL,   // &PrimFnDydPlusBisBvB, -- Can't happen w/Plus
    NULL,   // &PrimFnDydPlusBisIvI, -- Can't happen w/Plus
    NULL,   // &PrimFnDydPlusBisFvF, -- Can't happen w/Plus
    NULL,   // &PrimFnDydPlusBisCvC, -- Can't happen w/Plus

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    &PrimFnDydPlusIisIvI,
    NULL,   // &PrimFnDydPlusIisFvF, -- Can't happen w/Plus

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    &PrimFnDydPlusFisIvI,
    &PrimFnDydPlusFisFvF,

    NULL,   // &PrimFnDydPlusBisRvR, -- Can't happen w/Plus
    &PrimFnDydPlusRisRvR,

    NULL,   // &PrimFnDydPlusBisVvV, -- Can't happen w/Plus
////                 VisRvR     // Handled via type promotion (to VisVvV)
    &PrimFnDydPlusVisVvV,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecPlus};
#endif


//***************************************************************************
//  $PrimFnPlus_EM_YY
//
//  Primitive function for monadic and dyadic Plus ("conjugate" and "addition")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnPlus_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnPlus_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_PLUS);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnPlus_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecPlusStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecPlusStorageTypeMon
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

    return aplTypeRes;
} // End PrimSpecPlusStorageTypeMon


//***************************************************************************
//  $PrimFnMonPlusBisB
//
//  Primitive scalar function monadic Plus:  B {is} fn B
//***************************************************************************

APLBOOL PrimFnMonPlusBisB
    (APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanRht;
} // End PrimFnMonPlusBisB


//***************************************************************************
//  $PrimFnMonPlusIisI
//
//  Primitive scalar function monadic Plus:  I {is} fn I
//***************************************************************************

APLINT PrimFnMonPlusIisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplIntegerRht;
} // End PrimFnMonPlusIisI


//***************************************************************************
//  $PrimFnMonPlusFisF
//
//  Primitive scalar function monadic Plus:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonPlusFisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplFloatRht;
} // End PrimFnMonPlusFisF


//***************************************************************************
//  $PrimFnMonPlusRisR
//
//  Primitive scalar function monadic Plus:  R {is} fn R
//***************************************************************************

APLRAT PrimFnMonPlusRisR
    (APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLRAT mpqRes = {0};

    // Copy the right arg to the result
    mpq_init_set (&mpqRes, &aplRatRht);

    return mpqRes;
} // End PrimFnMonPlusRisR


//***************************************************************************
//  $PrimFnMonPlusVisV
//
//  Primitive scalar function monadic Plus:  V {is} fn V
//***************************************************************************

APLVFP PrimFnMonPlusVisV
    (APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLVFP mpfRes = {0};

    // Copy the right arg to the result
    mpfr_init_copy (&mpfRes, &aplVfpRht);

    return mpfRes;
} // End PrimFnMonPlusVisV


//***************************************************************************
//  $PrimFnMonPlusAPA_EM
//
//  Monadic plus, result is APA
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonPlusAPA_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnMonPlusAPA_EM
    (LPPL_YYSTYPE lpYYRes,          // The result token (may be NULL)
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
} // End PrimFnMonPlusAPA_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecPlusStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecPlusStorageTypeDyd
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

    // No Boolean results for addition
    if (IsSimpleBool (aplTypeRes))
        aplTypeRes = ARRAY_INT;

    // Special case addition with APA
    if (aplTypeRes EQ ARRAY_INT                             // Res = INT
     && (!IsSingleton (aplNELMLft) || !IsSingleton (aplNELMRht))    // Not both singletons
     && ((IsSingleton (aplNELMLft) && IsSimpleAPA (*lpaplTypeRht))  // Non-singleton is APA
      || (IsSingleton (aplNELMRht) && IsSimpleAPA (*lpaplTypeLft))))// ...
        aplTypeRes = ARRAY_APA;

    return aplTypeRes;
} // End PrimSpecPlusStorageTypeDyd


//***************************************************************************
//  $PrimFnDydPlusIisIvI
//
//  Primitive scalar function dyadic Plus:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydPlusIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Add the two integers and signal overflow exception in <iadd64_RE>
    return iadd64_RE (aplIntegerLft, aplIntegerRht);
} // End PrimFnDydPlusIisIvI


//***************************************************************************
//  $PrimFnDydPlusFisIvI
//
//  Primitive scalar function dyadic Plus:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydPlusFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ((APLFLOAT) (APLINT) aplIntegerLft)
         + ((APLFLOAT) (APLINT) aplIntegerRht);
} // End PrimFnDydPlusFisIvI


//***************************************************************************
//  $PrimFnDydPlusFisFvF
//
//  Primitive scalar function dyadic Plus:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydPlusFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for indeterminates:  _ + -_  or  -_ + _

    // If the args are both infinite and of opposite signs, ...
    if (IsFltInfinity (aplFloatLft)
     && IsFltInfinity (aplFloatRht)
     && SIGN_APLFLOAT (aplFloatLft) NE SIGN_APLFLOAT (aplFloatRht))
        return TranslateQuadICIndex (aplFloatLft,
                                     ICNDX_InfSUBInf,
                                     aplFloatRht,
                                     FALSE);
    return (aplFloatLft + aplFloatRht);
} // End PrimFnDydPlusFisFvF


//***************************************************************************
//  $PrimFnDydPlusRisRvR
//
//  Primitive scalar function dyadic Plus:  R {is} R fn R
//***************************************************************************

APLRAT PrimFnDydPlusRisRvR
    (APLRAT     aplRatLft,
     APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLRAT mpqRes = {0};

    // Check for indeterminates:  _ + -_  or  -_ + _

    // If the args are both infinite and of opposite signs, ...
    if (mpq_inf_p (&aplRatLft)
     && mpq_inf_p (&aplRatRht)
     && mpq_sgn (&aplRatLft) NE mpq_sgn (&aplRatRht))
        return *mpq_QuadICValue (&aplRatRht,        // No left arg
                                  ICNDX_InfSUBInf,
                                 &aplRatRht,
                                 &mpqRes,
                                  FALSE);
    // Initialize the result to 0/1
    mpq_init (&mpqRes);

    // Add the two Rationals
    mpq_add (&mpqRes, &aplRatLft, &aplRatRht);

    return mpqRes;
} // End PrimFnDydPlusRisRvR


//***************************************************************************
//  $PrimFnDydPlusVisVvV
//
//  Primitive scalar function dyadic Plus:  V {is} V fn V
//***************************************************************************

APLVFP PrimFnDydPlusVisVvV
    (APLVFP     aplVfpLft,
     APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLVFP mpfRes = {0};

    // Check for indeterminates:  _ + -_  or  -_ + _

    // If the args are both infinite and of opposite signs, ...
    if (mpfr_inf_p (&aplVfpLft)
     && mpfr_inf_p (&aplVfpRht)
     && mpfr_sgn (&aplVfpLft) NE mpfr_sgn (&aplVfpRht))
        return *mpfr_QuadICValue (&aplVfpRht,       // No left arg
                                   ICNDX_InfSUBInf,
                                  &aplVfpRht,
                                  &mpfRes,
                                   FALSE);
    // Initialize the result to NaN
    mpfr_init (&mpfRes);

    // Add the VFPs
    mpfr_add (&mpfRes, &aplVfpLft, &aplVfpRht, MPFR_RNDN);

    return mpfRes;
} // End PrimFnDydPlusVisVvV


//***************************************************************************
//  $PrimFnDydPlusAPA_EM
//
//  Dyadic plus, result is APA
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydPlusAPA_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydPlusAPA_EM
    (LPPL_YYSTYPE lpYYRes,          // The result token (may be NULL)

     LPTOKEN      lptkFunc,         // Ptr to function token

     HGLOBAL      hGlbLft,          // HGLOBAL of left arg (may be NULL if simple)
     HGLOBAL      hGlbRht,          // ...        right ...
     HGLOBAL     *lphGlbRes,        // Ptr to result handle

     APLRANK      aplRankLft,       // Left arg rank
     APLRANK      aplRankRht,       // Right ...

     APLNELM      aplNELMLft,       // Left arg NELM
     APLNELM      aplNELMRht,       // Right ...

     APLINT       aplInteger,       // The integer from the simple side
     LPPRIMSPEC   lpPrimSpec)       // Ptr to local PRIMSPEC

{
    APLRANK aplRankRes;             // Result rank
    LPVOID  lpMemRes;               // Ptr to result global memory
    UBOOL   bRet = FALSE;           // TRUE iff the result is valid

    DBGENTER;

    //***************************************************************
    // The result is an APA, one of the args is a simple singleton,
    //   the other an APA
    //***************************************************************

    // Axis may be anything

    // Get a ptr to each arg's header
    //   and the value of the singleton
    // Also, in case we're doing (1 1 1{rho}1)+{iota}4
    //   set the rank of the result to the rank of
    //   the APA.  The rank passed into this function
    //   is the larger of the left and right ranks,
    //   which for the above example, is wrong.

    // Split cases based upon the arg's NELM
    //   (one of the arg's must be a singleton)
    if (!IsSingleton (aplNELMLft))
    {
        *lphGlbRes = CopyArray_EM (hGlbLft, lptkFunc);
        aplRankRes = aplRankLft;
    } else
    if (!IsSingleton (aplNELMRht))
    {
        *lphGlbRes = CopyArray_EM (hGlbRht, lptkFunc);
        aplRankRes = aplRankRht;
    } else
        DbgStop ();     // We should never get here

    if (!*lphGlbRes)
        goto ERROR_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLockVar (*lphGlbRes);

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemRes);

#define lpAPA       ((LPAPLAPA) lpMemRes)

    // Add in the singleton's value to the result offset
    lpAPA->Off += aplInteger;

#undef  lpAPA

    // We no longer need this ptr
    MyGlobalUnlock (*lphGlbRes); lpMemRes = NULL;

    // Fill in the result token
    if (lpYYRes)
    {
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (*lphGlbRes);
    } // End IF

    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
    DBGLEAVE;

    return bRet;
} // End PrimFnDydPlusAPA_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_plus.c
//***************************************************************************
