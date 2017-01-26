//***************************************************************************
//  NARS2000 -- Primitive Function -- Bar
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
PRIMSPEC PrimSpecBar =
{
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecBarStorageTypeMon,
    &PrimFnMonBarAPA_EM,

    NULL,   // &PrimFnMonBarBisB, -- Can't happen w/Bar
    NULL,   // &PrimFnMonBarBisI, -- Can't happen w/Bar
    NULL,   // &PrimFnMonBarBisF, -- Can't happen w/Bar

////               IisB,        // Handled via type promotion (to IisI)
    &PrimFnMonBarIisI,
    NULL,   // &PrimFnMonBarIisF, -- Can't happen w/Bar

////               FisB,        // Handled via type promotion (to FisI)
    &PrimFnMonBarFisI,
    &PrimFnMonBarFisF,

    &PrimFnMonBarRisR,

////               VisR,        // Handled via type promotion (to VisV)
    &PrimFnMonBarVisV,

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecBarStorageTypeDyd,
    &PrimFnDydBarAPA_EM,

    NULL,   // &PrimFnDydBarBisBvB, -- Can't happen w/Bar
    NULL,   // &PrimFnDydBarBisIvI, -- Can't happen w/Bar
    NULL,   // &PrimFnDydBarBisFvF, -- Can't happen w/Bar
    NULL,   // &PrimFnDydBarBisCvC, -- Can't happen w/Bar

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    &PrimFnDydBarIisIvI,
    NULL,   // &PrimFnDydBarIisFvF, -- Can't happen w/Bar

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    &PrimFnDydBarFisIvI,
    &PrimFnDydBarFisFvF,

    NULL,   // &PrimFnDydBarBisRvR, -- Can't happen w/Bar
    &PrimFnDydBarRisRvR,

    NULL,   // &PrimFnDydBarBisVvV, -- Can't happen w/Bar
////                 VisRvR     // Handled via type promotion (to VisVvV)
    &PrimFnDydBarVisVvV,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecBar};
#endif


//***************************************************************************
//  $PrimFnBar_EM_YY
//
//  Primitive function for monadic and dyadic Bar ("negation" and "subtraction")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnBar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnBar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (IsAPLCharBar (lptkFunc->tkData.tkChar));

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnBar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecBarStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecBarStorageTypeMon
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

    // No Boolean result for negation
    if (IsSimpleBool (aplTypeRes))
        aplTypeRes = ARRAY_INT;

    return aplTypeRes;
} // End PrimSpecBarStorageTypeMon


//***************************************************************************
//  $PrimFnMonBarIisI
//
//  Primitive scalar function monadic Bar:  I {is} fn I
//***************************************************************************

APLINT PrimFnMonBarIisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for overflow or -0
    if (aplIntegerRht EQ MIN_APLINT
     || (gAllowNeg0 && aplIntegerRht EQ 0))
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

    return -aplIntegerRht;
} // End PrimFnMonBarIisI


//***************************************************************************
//  $PrimFnMonBarFisI
//
//  Primitive scalar function monadic Bar:  F {is} fn I
//***************************************************************************

APLFLOAT PrimFnMonBarFisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return PrimFnMonBarFisF ((APLFLOAT) aplIntegerRht, lpPrimSpec);
} // End PrimFnMonBarFisI


//***************************************************************************
//  $PrimFnMonBarFisF
//
//  Primitive scalar function monadic Bar:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonBarFisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    if (!gAllowNeg0
     && aplFloatRht EQ 0)
        return 0.0;
    else
        return -aplFloatRht;
} // End PrimFnMonBarFisF


//***************************************************************************
//  $PrimFnMonBarRisR
//
//  Primitive scalar function monadic Bar:  R {is} fn R
//***************************************************************************

APLRAT PrimFnMonBarRisR
    (APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLRAT mpqRes = {0};

    // Check for -0
    if (gAllowNeg0
     && IsMpq0 (&aplRatRht))
        RaiseException (EXCEPTION_RESULT_VFP, 0, 0, NULL);

    // Initialize the result to 0/1
    mpq_init (&mpqRes);

    // Negate the Rational
    mpq_neg (&mpqRes, &aplRatRht);

    return mpqRes;
} // End PrimFnMonBarRisR


//***************************************************************************
//  $PrimFnMonBarVisV
//
//  Primitive scalar function monadic Bar:  V {is} fn V
//***************************************************************************

APLVFP PrimFnMonBarVisV
    (APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLVFP mpfRes = {0};

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Check for -0
    if (gAllowNeg0
     || !IsMpf0 (&aplVfpRht))
        // Negate the VFP
        mpfr_neg (&mpfRes, &aplVfpRht, MPFR_RNDN);

    return mpfRes;
} // End PrimFnMonBarVisV


//***************************************************************************
//  $PrimFnMonBarAPA_EM
//
//  Monadic Bar, result is APA
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonBarAPA_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnMonBarAPA_EM
    (LPPL_YYSTYPE  lpYYRes,         // Ptr to the result (may be NULL)
     LPTOKEN       lptkFunc,        // Ptr to function token
     HGLOBAL       hGlbRht,         // Handle to right arg
     HGLOBAL      *lphGlbRes,       // Ptr to handle to result
     APLRANK       aplRankRht,      // Rank of the right arg
     LPPRIMSPEC    lpPrimSpec)      // Ptr to local PRIMSPEC

{
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to result header
    LPVOID            lpMemRes;             // Ptr to result global memory
    APLNELM           aplNELMRes;           // Result NELM
    APLRANK           aplRankRes;           // Result rank
    UBOOL             bRet = FALSE;         // TRUE iff the result is valid


    // Axis may be anything

    *lphGlbRes = CopyArray_EM (hGlbRht, lptkFunc);
    if (!*lphGlbRes)
        goto ERROR_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);

#define lpHeader    lpMemHdrRes
    aplNELMRes = lpHeader->NELM;
    aplRankRes = lpHeader->Rank;
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Check for un-negatable integer
#define lpAPA       ((LPAPLAPA) lpMemRes)
    if (lpAPA->Off EQ MIN_APLINT
     || (lpAPA->Off + aplNELMRes * lpAPA->Mul) EQ MIN_APLINT
     || lpAPA->Off EQ 0
     || (lpAPA->Off + aplNELMRes * lpAPA->Mul) EQ 0
     || signumint (lpAPA->Off) NE signumint (lpAPA->Off + aplNELMRes * lpAPA->Mul)
       )
    {
        // We no longer need this ptr
        MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;

        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } // End IF

    // Negating the offset and multiplier negates the APA
    lpAPA->Off = -lpAPA->Off;
    lpAPA->Mul = -lpAPA->Mul;
#undef  lpAPA

    // We no longer need this ptr
    MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;

    // Fill in the result token
    if (lpYYRes NE NULL)
    {
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (*lphGlbRes);
    } // End IF

    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:

    return bRet;
} // End PrimFnMonBarAPA_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecBarStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecBarStorageTypeDyd
    (APLNELM    aplNELMLft,
     LPAPLSTYPE lpaplTypeLft,
     LPTOKEN    lptkFunc,
     APLNELM    aplNELMRht,
     LPAPLSTYPE lpaplTypeRht)

{
    APLSTYPE aplTypeRes;            // Result storage type

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

    // No Boolean results for subtraction
    if (IsSimpleBool (aplTypeRes))
        aplTypeRes = ARRAY_INT;

    // Special case subtraction with APA
    if (aplTypeRes EQ ARRAY_INT                            // Res = INT
     && (!IsSingleton (aplNELMLft) || !IsSingleton (aplNELMRht))    // Not both singletons
     && ((IsSingleton (aplNELMLft) && IsSimpleAPA (*lpaplTypeRht))  // Non-singleton is APA
      || (IsSingleton (aplNELMRht) && IsSimpleAPA (*lpaplTypeLft))))// ...
        aplTypeRes = ARRAY_APA;

    return aplTypeRes;
} // End PrimSpecBarStorageTypeDyd


//***************************************************************************
//  $PrimFnDydBarIisIvI
//
//  Primitive scalar function dyadic Bar:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydBarIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Subtract the two integers and signal overflow execption in <isub64_RE>
    return isub64_RE (aplIntegerLft, aplIntegerRht);
} // End PrimFnDydBarIisIvI


//***************************************************************************
//  $PrimFnDydBarFisIvI
//
//  Primitive scalar function dyadic Bar:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydBarFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ((APLFLOAT) (APLINT) aplIntegerLft)
         - ((APLFLOAT) (APLINT) aplIntegerRht);
} // End PrimFnDydBarFisIvI


//***************************************************************************
//  $PrimFnDydBarFisFvF
//
//  Primitive scalar function dyadic Bar:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydBarFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for indeterminates:  _ - _  or  -_ - -_

    // If the args are both infinite and of the same signs, ...
    if (IsFltInfinity (aplFloatLft)
     && IsFltInfinity (aplFloatRht)
     && SIGN_APLFLOAT (aplFloatLft) EQ SIGN_APLFLOAT (aplFloatRht))
        return TranslateQuadICIndex (aplFloatLft,
                                     ICNDX_InfSUBInf,
                                     aplFloatRht,
                                     FALSE);
    return (aplFloatLft - aplFloatRht);
} // End PrimFnDydBarFisFvF


//***************************************************************************
//  $PrimFnDydBarRisRvR
//
//  Primitive scalar function dyadic Bar:  R {is} R fn R
//***************************************************************************

APLRAT PrimFnDydBarRisRvR
    (APLRAT     aplRatLft,
     APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLRAT mpqRes = {0};

    // Check for indeterminates:  _ - _  or  -_ - -_

    // If the args are both infinite and of the same signs, ...
    if (mpq_inf_p (&aplRatLft)
     && mpq_inf_p (&aplRatRht)
     && mpq_sgn (&aplRatLft) EQ mpq_sgn (&aplRatRht))
        return *mpq_QuadICValue (&aplRatRht,        // No left arg
                                  ICNDX_InfSUBInf,
                                 &aplRatRht,
                                 &mpqRes,
                                  FALSE);
    // Initialize the result to 0/1
    mpq_init (&mpqRes);

    // Subtract the Rationals
    mpq_sub (&mpqRes, &aplRatLft, &aplRatRht);

    return mpqRes;
} // End PrimFnDydBarRisRvR


//***************************************************************************
//  $PrimFnDydBarVisVvV
//
//  Primitive scalar function dyadic Bar:  V {is} V fn V
//***************************************************************************

APLVFP PrimFnDydBarVisVvV
    (APLVFP     aplVfpLft,
     APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLVFP mpfRes = {0};

    // Check for indeterminates:  _ - _  or  -_ - -_

    // If the args are both infinite and of the same signs, ...
    if (mpfr_inf_p (&aplVfpLft)
     && mpfr_inf_p (&aplVfpRht)
     && mpfr_sgn (&aplVfpLft) EQ mpfr_sgn (&aplVfpRht))
        return *mpfr_QuadICValue (&aplVfpRht,       // No left arg
                                   ICNDX_InfSUBInf,
                                  &aplVfpRht,
                                  &mpfRes,
                                   FALSE);
    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Subtract the VFPs
    mpfr_sub (&mpfRes, &aplVfpLft, &aplVfpRht, MPFR_RNDN);

    return mpfRes;
} // End PrimFnDydBarVisVvV


//***************************************************************************
//  $PrimFnDydBarAPA_EM
//
//  Dyadic Bar, result is APA
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydBarAPA_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydBarAPA_EM
    (LPPL_YYSTYPE lpYYRes,          // Ptr to the result (may be NULL)

     LPTOKEN      lptkFunc,         // Ptr to function token

     HGLOBAL      hGlbLft,          // HGLOBAL of left arg (may be NULL if simple)
     HGLOBAL      hGlbRht,          // ...        right ...
     HGLOBAL     *lphGlbRes,        // Ptr to HGLOBAL of the result

     APLRANK      aplRankLft,       // Rank of the left arg
     APLRANK      aplRankRht,       // ...         right ...

     APLNELM      aplNELMLft,       // Only one of these NELMs is 1
     APLNELM      aplNELMRht,       // ...

     APLINT       aplInteger,       // The integer from the simple side
     LPPRIMSPEC   lpPrimSpec)       // Ptr to PRIMSPEC

{
    APLRANK           aplRankRes;           // Result rank
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to result header
    LPAPLAPA          lpMemRes;             // Ptr to result global memory
    UBOOL             bRet = FALSE;         // TRUE iff the result is valid


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
    lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

#define lpAPA       lpMemRes

    if (!IsSingleton (aplNELMLft))
        lpAPA->Off -= aplInteger;
    else
    {
        lpAPA->Off = aplInteger - lpAPA->Off;
        lpAPA->Mul = -lpAPA->Mul;
    } // End IF

#undef  lpAPA

    // We no longer need this ptr
    MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;

    // Fill in the result token
    if (lpYYRes NE NULL)
    {
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (*lphGlbRes);
    } // End IF

    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:

    return bRet;
} // End PrimFnDydBarAPA_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_bar.c
//***************************************************************************
