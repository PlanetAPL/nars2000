//***************************************************************************
//  NARS2000 -- Primitive Function -- Times
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
PRIMSPEC PrimSpecTimes =
{
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecTimesStorageTypeMon,
    NULL,   // &PrimFnMonTimesAPA_EM, -- Can't happen w/Times

    &PrimFnMonTimesBisB,
    NULL,   // &PrimFnMonTimesBisI, -- Can't happen w/Times
    NULL,   // &PrimFnMonTimesBisF, -- Can't happen w/Times

////               IisB,     // Handled via type promotion (to IisI)
    &PrimFnMonTimesIisI,
    &PrimFnMonTimesIisF,

////               FisB,     // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonTimesFisI, -- Can't happen w/Times
    NULL,   // &PrimFnMonTimesFisF, -- Can't happen w/Times

    &PrimFnMonTimesRisR,

////               VisR,     // Handled via type promotion (to VisV)
    &PrimFnMonTimesVisV,

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecTimesStorageTypeDyd,
    &PrimFnDydTimesAPA_EM,

    &PrimFnDydUpCaretBisBvB,
    NULL,   // &PrimFnDydTimesBisIvI, -- Can't happen w/Times
    NULL,   // &PrimFnDydTimesBisFvF, -- Can't happen w/Times
    NULL,   // &PrimFnDydTimesBisCvC, -- Can't happen w/Times

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    &PrimFnDydTimesIisIvI,
    NULL,   // &PrimFnDydTimesIisFvF, -- Can't happen w/Times

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    &PrimFnDydTimesFisIvI,
    &PrimFnDydTimesFisFvF,

    NULL,   // &PrimFnDydTimesBisRvR, -- Can't happen w/Times
    &PrimFnDydTimesRisRvR,

    NULL,   // &PrimFnDydTimesBisVvV, -- Can't happen w/Times
////                 VisRvR     // Handled via type promotion (to VisVvV)
    &PrimFnDydTimesVisVvV,

    NULL,   // &PrimFnMonTimesB64isB64, -- Can't happen w/Times
    NULL,   // &PrimFnMonTimesB32isB32, -- Can't happen w/Times
    NULL,   // &PrimFnMonTimesB16isB16, -- Can't happen w/Times
    NULL,   // &PrimFnMonTimesB08isB08, -- Can't happen w/Times

    &PrimFnDydUpCaretB64isB64vB64,
    &PrimFnDydUpCaretB32isB32vB32,
    &PrimFnDydUpCaretB16isB16vB16,
    &PrimFnDydUpCaretB08isB08vB08,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecTimes};
#endif


//***************************************************************************
//  $PrimFnTimes_EM_YY
//
//  Primitive function for monadic and dyadic Times ("signum" and "multiplication")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnTimes_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnTimes_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_TIMES);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnTimes_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecTimesStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecTimesStorageTypeMon
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

    // Except that FLOAT and APA become INT
    if (IsSimpleFlt (aplTypeRes)
     || IsSimpleAPA (aplTypeRes))
        return ARRAY_INT;

    return aplTypeRes;
} // End PrimSpecTimesStorageTypeMon


//***************************************************************************
//  $PrimFnMonTimesBisB
//
//  Primitive scalar function monadic Times:  B {is} fn B
//***************************************************************************

APLBOOL PrimFnMonTimesBisB
    (APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanRht;
} // End PrimFnMonTimesBisB


//***************************************************************************
//  $PrimFnMonTimesIisI
//
//  Primitive scalar function monadic Times:  I {is} fn I
//***************************************************************************

APLINT PrimFnMonTimesIisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return signumint (aplIntegerRht);
} // End PrimFnMonTimesIisI


//***************************************************************************
//  $PrimFnMonTimesIisF
//
//  Primitive scalar function monadic Times:  I {is} fn F
//***************************************************************************

APLINT PrimFnMonTimesIisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    return signumflt (aplFloatRht);
} // End PrimFnMonTimesIisF


//***************************************************************************
//  $PrimFnMonTimesRisR
//
//  Primitive scalar function monadic Times:  R {is} fn R
//***************************************************************************

APLRAT PrimFnMonTimesRisR
    (APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLRAT mpqRes = {0};

    mpq_init_set_si (&mpqRes, mpq_sgn (&aplRatRht), 1);

    return mpqRes;
} // End PrimFnMonTimesRisR


//***************************************************************************
//  $PrimFnMonTimesVisV
//
//  Primitive scalar function monadic Times:  V {is} fn V
//***************************************************************************

APLVFP PrimFnMonTimesVisV
    (APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLVFP mpfRes = {0};

    // If the arg is 0, ...
    if (IsMpf0 (&aplVfpRht))
        mpfr_init_set_si (&mpfRes, SIGN_APLVFP (&aplVfpRht) ? -1 : 0, MPFR_RNDN);
    else
        mpfr_init_set_si (&mpfRes,    mpfr_sgn (&aplVfpRht)         , MPFR_RNDN);

    return mpfRes;
} // End PrimFnMonTimesVisV


//***************************************************************************
//  $PrimSpecTimesStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecTimesStorageTypeDyd
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

    // Special case multiplication with APA
    if (aplTypeRes EQ ARRAY_INT                            // Res = INT
     && (!IsSingleton (aplNELMLft) || !IsSingleton (aplNELMRht))    // Not both singletons
     && ((IsSingleton (aplNELMLft) && IsSimpleAPA (*lpaplTypeRht))   // Non-singleton is APA
      || (IsSingleton (aplNELMRht) && IsSimpleAPA (*lpaplTypeLft)))) // ...
        aplTypeRes = ARRAY_APA;

    return aplTypeRes;
} // End PrimSpecTimesStorageTypeDyd


//***************************************************************************
//  $PrimFnDydTimesIisIvI
//
//  Primitive scalar function dyadic Times:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydTimesIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for -0
    // If either arg is 0
    //   and the signs are different
    if (gAllowNeg0
     && (aplIntegerLft EQ 0
      || aplIntegerRht EQ 0)
     && (aplIntegerLft < 0) NE
        (aplIntegerRht < 0))
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

    return imul64_RE (aplIntegerLft, aplIntegerRht);
} // End PrimFnDydTimesIisIvI


//***************************************************************************
//  $PrimFnDydTimesFisIvI
//
//  Primitive scalar function dyadic Times:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydTimesFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    UBOOL  bRet;
    APLINT aplRes;

    // Check for -0
    // If either arg is 0
    //   and the signs are different
    if (gAllowNeg0
     && (aplIntegerLft EQ 0
      || aplIntegerRht EQ 0)
     && (aplIntegerLft < 0) NE
        (aplIntegerRht < 0))
        return -0.0;

    aplRes = imul64 (aplIntegerLft, aplIntegerRht, &bRet);
    if (bRet)
        return (APLFLOAT) aplRes;

    return ((APLFLOAT) aplIntegerLft) * (APLFLOAT) aplIntegerRht;
} // End PrimFnDydTimesFisIvI


//***************************************************************************
//  $PrimFnDydTimesFisFvF
//
//  Primitive scalar function dyadic Times:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydTimesFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for indeterminates:  0 {times} _  or  _ {times} 0
    if ((aplFloatLft EQ 0
      && IsFltPosInfinity (aplFloatRht))
     || (IsFltPosInfinity (aplFloatLft)
      && aplFloatRht EQ 0))
        return TranslateQuadICIndex (aplFloatLft,
                                     ICNDX_0MULPi,
                                     aplFloatRht,
                                     (aplFloatLft EQ 0) ? SIGN_APLFLOAT (aplFloatLft)
                                                        : SIGN_APLFLOAT (aplFloatRht));
    // Check for indeterminates:  0 {times} {neg}_  or  {neg}_ {times} 0
    if ((aplFloatLft EQ 0
      && IsFltNegInfinity (aplFloatRht))
     || (IsFltNegInfinity (aplFloatLft)
      && aplFloatRht EQ 0))
        return TranslateQuadICIndex (aplFloatLft,
                                     ICNDX_0MULNi,
                                     aplFloatRht,
                                     (aplFloatLft EQ 0) ? SIGN_APLFLOAT (aplFloatLft)
                                                        : SIGN_APLFLOAT (aplFloatRht));
    // Check for -0
    // If either arg is 0
    //   and the signs are different
    if (gAllowNeg0
     && (aplFloatLft EQ 0
      || aplFloatRht EQ 0)
     && (SIGN_APLFLOAT (aplFloatLft) NE
         SIGN_APLFLOAT (aplFloatRht)))
        return -0.0;

    return (aplFloatLft * aplFloatRht);
} // End PrimFnDydTimesFisFvF


//***************************************************************************
//  $PrimFnDydTimesRisRvR
//
//  Primitive scalar function dyadic Times:  R {is} R fn R
//***************************************************************************

APLRAT PrimFnDydTimesRisRvR
    (APLRAT     aplRatLft,
     APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLRAT mpqRes = {0};

    // Check for indeterminates:  0 {times} _  or  _ {times} 0
    if ((IsMpq0 (&aplRatLft)
      && IsMpqPosInfinity (&aplRatRht))
     || (IsMpqPosInfinity (&aplRatLft)
      && IsMpq0 (&aplRatRht)))
        return *mpq_QuadICValue (&aplRatLft,
                                  ICNDX_0MULPi,
                                 &aplRatRht,
                                 &mpqRes,
                                  IsMpq0 (&aplRatLft) ? (mpq_sgn (&aplRatLft) EQ -1)
                                                      : (mpq_sgn (&aplRatRht) EQ -1));
    // Check for indeterminates:  0 {times} {neg}_  or  {neg}_ {times} 0
    if ((IsMpq0 (&aplRatLft)
      && IsMpqNegInfinity (&aplRatRht))
     || (IsMpqNegInfinity (&aplRatLft)
      && IsMpq0 (&aplRatRht)))
        return *mpq_QuadICValue (&aplRatLft,
                                  ICNDX_0MULNi,
                                 &aplRatRht,
                                 &mpqRes,
                                  IsMpq0 (&aplRatLft) ? (mpq_sgn (&aplRatLft) EQ -1)
                                                      : (mpq_sgn (&aplRatRht) EQ -1));
    else
    // Check for -0
    // If either arg is 0
    //   and the signs are different
    if (gAllowNeg0
     && (IsMpq0 (&aplRatLft)
      || IsMpq0 (&aplRatRht))
     && (mpq_sgn (&aplRatLft) < 0) NE
        (mpq_sgn (&aplRatRht) < 0))
        RaiseException (EXCEPTION_RESULT_VFP, 0, 0, NULL);
    else
    {
        // Initalize the result to 0/1
        mpq_init (&mpqRes);

        // Multiply two Rationals
        mpq_mul (&mpqRes, &aplRatLft, &aplRatRht);
    } // End IF/ELSE/...

    return mpqRes;
} // End PrimFnDydTimesRisRvR


//***************************************************************************
//  $PrimFnDydTimesVisVvV
//
//  Primitive scalar function dyadic Times:  V {is} V fn V
//***************************************************************************

APLVFP PrimFnDydTimesVisVvV
    (APLVFP     aplVfpLft,
     APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLVFP mpfRes = {0};

    // Check for indeterminates:  0 {times} _  or  _ {times} 0
    if ((IsMpf0 (&aplVfpLft)
      && IsMpfPosInfinity (&aplVfpRht))
     || (IsMpfPosInfinity (&aplVfpLft)
      && IsMpf0 (&aplVfpRht)))
        return *mpfr_QuadICValue (&aplVfpLft,
                                   ICNDX_0MULPi,
                                  &aplVfpRht,
                                  &mpfRes,
                                   IsMpf0 (&aplVfpLft) ? SIGN_APLVFP (&aplVfpLft)
                                                       : SIGN_APLVFP (&aplVfpRht));
    // Check for indeterminates:  0 {times} {neg}_  or  {neg}_ {times} 0
    if ((IsMpf0 (&aplVfpLft)
      && IsMpfNegInfinity (&aplVfpRht))
     || (IsMpfNegInfinity (&aplVfpLft)
      && IsMpf0 (&aplVfpRht)))
        return *mpfr_QuadICValue (&aplVfpLft,
                                   ICNDX_0MULNi,
                                  &aplVfpRht,
                                  &mpfRes,
                                   IsMpf0 (&aplVfpLft) ? SIGN_APLVFP (&aplVfpLft)
                                                       : SIGN_APLVFP (&aplVfpRht));
    if (gAllowNeg0
     && (IsMpf0 (&aplVfpLft)
      || IsMpf0 (&aplVfpRht))
     && (mpfr_sgn (&aplVfpLft) < 0) NE
        (mpfr_sgn (&aplVfpRht) < 0))
        // Set the result to -0
        mpfr_init_set_d (&mpfRes, -0, MPFR_RNDN);
    else
    {
        // Initalize the result to 0
        mpfr_init0 (&mpfRes);

        // Multiply two Variable FPs
        mpfr_mul (&mpfRes, &aplVfpLft, &aplVfpRht, MPFR_RNDN);
    } // End IF/ELSE

    return mpfRes;
} // End PrimFnDydTimesVisVvV


//***************************************************************************
//  $PrimFnDydTimesAPA_EM
//
//  Dyadic Times, result is APA
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydTimesAPA_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydTimesAPA_EM
    (LPPL_YYSTYPE lpYYRes,          // Ptr to result token (may be NULL)

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
    APLNELM           aplNELMRes;           // Result NELM
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
    // Also, in case we're doing (1 1 1{rho}1) fn {iota}4
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

#define lpHeader    lpMemHdrRes
    aplNELMRes = lpHeader->NELM;
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

#define lpAPA       ((LPAPLAPA) lpMemRes)

    // Check for negative integer {times} an APA that spans 0
    if (gAllowNeg0
     && aplInteger < 0
     && (lpAPA->Off EQ 0
      || (lpAPA->Off + aplNELMRes * lpAPA->Mul) EQ 0
      || signumint (lpAPA->Off) NE signumint (lpAPA->Off + aplNELMRes * lpAPA->Mul)
        )
       )
    {
        // We no longer need this ptr
        MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;

        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } // End IF

    // Multiply the singleton's value to the result offset and multiplier
    lpAPA->Off *= aplInteger;
    lpAPA->Mul *= aplInteger;

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
} // End PrimFnDydTimesAPA_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_times.c
//***************************************************************************
