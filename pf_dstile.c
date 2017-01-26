//***************************************************************************
//  NARS2000 -- Primitive Function -- DownStile
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
PRIMSPEC PrimSpecDownStile = {
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecDownStileStorageTypeMon,
    &PrimFnMonDownStileAPA_EM,

    &PrimFnMonDownStileBisB,
    NULL,   // &PrimFnMonDownStileBisI, -- Can't happen w/DownStile
    NULL,   // &PrimFnMonDownStileBisF, -- Can't happen w/DownStile

////                   IisB,   // Handled via type promotion (to IisI)
    &PrimFnMonDownStileIisI,
    &PrimFnMonDownStileIisF,

////                   FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonDownStileFisI, -- Can't happen w/DownStile
    &PrimFnMonDownStileFisF,

    &PrimFnMonDownStileRisR,

////               VisR,    // Handled via type promotion (to VisV)
    &PrimFnMonDownStileVisV,

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecDownStileStorageTypeDyd,
    NULL,   // &PrimFnDydDownStileAPA_EM, -- Can't happen w/DownStile

    &PrimFnDydUpCaretBisBvB,
    NULL,   // &PrimFnDydDownStileBisIvI, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileBisFvF, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileBisCvC, -- Can't happen w/DownStile

////                   IisBvB,  // Handled via type promotion (to IisIvI)
    &PrimFnDydDownStileIisIvI,
    NULL,   // &PrimFnDydDownStileIisFvF, -- Can't happen w/DownStile

////                   FisBvB,  // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydDownStileFisIvI, -- Can't happen w/DownStile
    &PrimFnDydDownStileFisFvF,

    NULL,   // &PrimFnDydDownStileBisRvR, -- Can't happen w/DownStile
    &PrimFnDydDownStileRisRvR,

    NULL,   // &PrimFnDydDownStileBisVvV, -- Can't happen w/DownStile
////                  VisRvR,   // Handled via type promotion (to VisVvV)
    &PrimFnDydDownStileVisVvV,

    NULL,   // &PrimFnMonDownStileB64isB64, -- Can't happen w/DownStile
    NULL,   // &PrimFnMonDownStileB32isB32, -- Can't happen w/DownStile
    NULL,   // &PrimFnMonDownStileB16isB16, -- Can't happen w/DownStile
    NULL,   // &PrimFnMonDownStileB08isB08, -- Can't happen w/DownStile

    &PrimFnDydUpCaretB64isB64vB64,
    &PrimFnDydUpCaretB32isB32vB32,
    &PrimFnDydUpCaretB16isB16vB16,
    &PrimFnDydUpCaretB08isB08vB08,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecDownStile};
#endif


//***************************************************************************
//  $PrimFnDownStile_EM_YY
//
//  Primitive function for monadic and dyadic DownStile ("floor" and "minimum")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDownStile_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDownStile_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_DOWNSTILE);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnDownStile_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecDownStileStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecDownStileStorageTypeMon
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
} // End PrimSpecDownStileStorageTypeMon


//***************************************************************************
//  $PrimFnMonDownStileBisB
//
//  Primitive scalar function monadic DownStile:  B {is} fn B
//***************************************************************************

APLBOOL PrimFnMonDownStileBisB
    (APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanRht;
} // End PrimFnMonDownStileBisB


//***************************************************************************
//  $PrimFnMonDownStileIisI
//
//  Primitive scalar function monadic DownStile:  I {is} fn I
//***************************************************************************

APLINT PrimFnMonDownStileIisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplIntegerRht;
} // End PrimFnMonDownStileIisI


//***************************************************************************
//  $PrimFnMonDownStileIisF
//
//  Primitive scalar function monadic DownStile:  I {is} fn F
//***************************************************************************

APLINT PrimFnMonDownStileIisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for PoM infinity and numbers whose
    //   absolute value is >= 2*53
    if (IsFltInfinity (aplFloatRht)
     || fabs (aplFloatRht) >= Float2Pow53)
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

    return (APLINT) PrimFnMonDownStileFisF (aplFloatRht, lpPrimSpec);
} // End PrimFnMonDownStileIisF


//***************************************************************************
//  $PrimFnMonDownStileFisF
//
//  Primitive scalar function monadic DownStile:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonDownStileFisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplFloor,
             aplCeil,
             aplNear;

    // Check for PoM infinity
    if (IsFltInfinity (aplFloatRht))
        return aplFloatRht;

    // Get the exact floor and ceiling
    aplFloor = floor (aplFloatRht);
    aplCeil  = ceil  (aplFloatRht);

    // Calculate the integer nearest the right arg

    // Split cases based upon the signum of the difference between
    //   (the number and its floor) and (the ceiling and the number)
    switch (signumflt ((aplFloatRht - aplFloor)
                     - (aplCeil     - aplFloatRht)))
    {
        case  1:
            aplNear = aplCeil;

            break;

        case  0:
            // They are equal, so use the one with the larger absolute value
            aplNear = ((fabs (aplFloor) > fabs (aplCeil)) ? aplFloor
                                                          : aplCeil);
            break;

        case -1:
            aplNear = aplFloor;

            break;

        defstop
            break;
    } // End SWITCH

    // If Near is < Rht, return Near
    if (aplNear < aplFloatRht)
        return aplNear;

    // If Near is non-zero, and
    //   Rht is tolerantly-equal to Near,
    //   return Near; otherwise, return Near - 1
    if (aplNear)
    {
        if (PrimFnDydEqualBisFvF (aplFloatRht,
                                  (APLFLOAT) aplNear,
                                  NULL))
            return aplNear;
        else
            return aplNear - 1;
    } // End IF

    // aplNear is zero

    // If Rht is between (-[]CT) and 0 (inclusive),
    //   return 0; otherwise, return -1
    if ((-GetQuadCT ()) <= aplFloatRht
     &&                    aplFloatRht <= 0)
        return 0;
    else
        return -1;
} // End PrimFnMonDownStileFisF


//***************************************************************************
//  $PrimFnMonDownStileRisR
//
//  Primitive scalar function monadic DownStile:  R {is} fn R
//***************************************************************************

APLRAT PrimFnMonDownStileRisR
    (APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLRAT mpqRes   = {0},
           mpqFloor = {0},
           mpqCeil  = {0},
           mpqTmp1  = {0},
           mpqTmp2  = {0},
           mpqNear  = {0};

    // Check for PoM infinity
    if (IsMpqInfinity (&aplRatRht))
        // Copy to the result
        mpq_init_set  (&mpqRes, &aplRatRht);
    else
    {
        // Initialize the temps
        mpq_init (&mpqRes);
        mpq_init (&mpqFloor);
        mpq_init (&mpqCeil );
        mpq_init (&mpqTmp1);
        mpq_init (&mpqTmp2);
        mpq_init (&mpqNear);

        // Get the exact floor and ceiling
        mpq_floor (&mpqFloor, &aplRatRht);
        mpq_ceil  (&mpqCeil , &aplRatRht);

        // Calculate the integer nearest the right arg

        mpq_sub (&mpqTmp1, &aplRatRht, &mpqFloor);
        mpq_sub (&mpqTmp2, &mpqCeil  , &aplRatRht);

        // Split cases based upon the signum of the difference between
        //   (the number and its floor) and (the ceiling and the number)
        switch (signumint (mpq_cmp (&mpqTmp1, &mpqTmp2)))
        {
            case  1:
                mpq_set (&mpqNear, &mpqCeil);

                break;

            case  0:
                mpq_abs (&mpqTmp1, &mpqFloor);
                mpq_abs (&mpqTmp2, &mpqFloor);

                // They are equal, so use the one with the larger absolute value
                mpq_set (&mpqNear, ((mpq_cmp (&mpqTmp1, &mpqTmp2) > 0) ? &mpqFloor
                                                                       : &mpqCeil));
                break;

            case -1:
                mpq_set (&mpqNear, &mpqFloor);

                break;

            defstop
                break;
        } // End SWITCH

        // If Near is < Rht, return Near
        if (mpq_cmp (&mpqNear, &aplRatRht) < 0)
            mpq_set (&mpqRes, &mpqNear);
        else
        {
            // If Near is non-zero, and
            //   Rht is tolerantly-equal to Near,
            //   return Near; otherwise, return Near - 1
            if (mpq_sgn (&mpqNear) NE 0)
            {
                mpq_set (&mpqRes, &mpqNear);

                if (!PrimFnDydEqualBisRvR (aplRatRht,
                                           mpqNear,
                                           NULL))
                    mpq_sub_ui (&mpqRes, &mpqRes, 1, 1);
            } else
            {
                // mpfNear is zero

                // Get -[]CT as a VFP
                mpq_set_d (&mpqTmp1, -GetQuadCT ());

                // If Rht is between (-[]CT) and 0 (inclusive),
                //   return 0; otherwise, return -1
                if (mpq_cmp (&mpqTmp1, &aplRatRht) <= 0
                 && mpq_sgn (&aplRatRht)           <= 0)
                    mpq_set_si (&mpqRes,  0, 1);
                else
                    mpq_set_si (&mpqRes, -1, 1);
            } // End IF/ELSE
        } // End IF/ELSE

        // We no longer need this storage
        Myq_clear (&mpqNear);
        Myq_clear (&mpqTmp2);
        Myq_clear (&mpqTmp1);
        Myq_clear (&mpqCeil);
        Myq_clear (&mpqFloor);
    } // End IF/ELSE

    return mpqRes;
} // End PrimFnMonDownStileRisR


//***************************************************************************
//  $PrimFnMonDownStileVisV
//
//  Primitive scalar function monadic DownStile:  V {is} fn V
//***************************************************************************

APLVFP PrimFnMonDownStileVisV
    (APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLVFP mpfRes   = {0},
           mpfFloor = {0},
           mpfCeil  = {0},
           mpfTmp1  = {0},
           mpfTmp2  = {0},
           mpfNear  = {0};

    // Check for PoM infinity
    if (IsMpfInfinity (&aplVfpRht))
        // Copy to the result
        mpfr_init_copy (&mpfRes, &aplVfpRht);
    else
    {
#ifdef DEBUG
////    WCHAR wszTemp[512];
#endif
        // Initialize the temps
        mpfr_init0 (&mpfRes);
        mpfr_init0 (&mpfFloor);
        mpfr_init0 (&mpfCeil );
        mpfr_init0 (&mpfTmp1);
        mpfr_init0 (&mpfTmp2);
        mpfr_init0 (&mpfNear);

        // Get the exact floor and ceiling
        mpfr_floor (&mpfFloor, &aplVfpRht);
        mpfr_ceil  (&mpfCeil , &aplVfpRht);

        // Calculate the integer nearest the right arg

        mpfr_sub (&mpfTmp1, &aplVfpRht, &mpfFloor, MPFR_RNDN);
        mpfr_sub (&mpfTmp2, &mpfCeil  , &aplVfpRht, MPFR_RNDN);

        // Split cases based upon the signum of the difference between
        //   (the number and its floor) and (the ceiling and the number)
        switch (signumint (mpfr_cmp (&mpfTmp1, &mpfTmp2)))
        {
            case  1:
                mpfr_set (&mpfNear, &mpfCeil, MPFR_RNDN);

                break;

            case  0:
                mpfr_abs (&mpfTmp1, &mpfFloor, MPFR_RNDN);
                mpfr_abs (&mpfTmp2, &mpfFloor, MPFR_RNDN);

                // They are equal, so use the one with the larger absolute value
                mpfr_set (&mpfNear,
                           ((mpfr_cmp (&mpfTmp1, &mpfTmp2) > 0) ? &mpfFloor
                                                                : &mpfCeil),
                           MPFR_RNDN);
                break;

            case -1:
                mpfr_set (&mpfNear, &mpfFloor, MPFR_RNDN);

                break;

            defstop
                break;
        } // End SWITCH

#ifdef DEBUG
////    strcpyW (wszTemp, L"Floor: "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], mpfFloor, 0) = WC_EOS; DbgMsgW (wszTemp);
////    strcpyW (wszTemp, L"Near:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], mpfNear , 0) = WC_EOS; DbgMsgW (wszTemp);
////    strcpyW (wszTemp, L"Ceil:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], mpfCeil , 0) = WC_EOS; DbgMsgW (wszTemp);
#endif

        // If Near is < Rht, return Near
        if (mpfr_cmp (&mpfNear, &aplVfpRht) < 0)
            mpfr_set (&mpfRes, &mpfNear, MPFR_RNDN);
        else
        {
            // If Near is non-zero, and
            //   Rht is tolerantly-equal to Near,
            //   return Near; otherwise, return Near - 1
            if (mpfr_sgn (&mpfNear) NE 0)
            {
                mpfr_set (&mpfRes, &mpfNear, MPFR_RNDN);

                if (!PrimFnDydEqualBisVvV (aplVfpRht,
                                           mpfNear,
                                           NULL))
                    mpfr_sub_ui (&mpfRes, &mpfRes, 1, MPFR_RNDN);
            } else
            {
                // aplNear is zero

                // Get -[]CT as a VFP
                mpfr_set_d (&mpfTmp1, -GetQuadCT (), MPFR_RNDN);

                // If Rht is between (-[]CT) and 0 (inclusive),
                //   return 0; otherwise, return -1
                if (mpfr_cmp (&mpfTmp1, &aplVfpRht) <= 0
                 && mpfr_sgn (&aplVfpRht)           <= 0)
                    mpfr_set_si (&mpfRes,  0, MPFR_RNDN);
                else
                    mpfr_set_si (&mpfRes, -1, MPFR_RNDN);
            } // End IF/ELSE
        } // End IF/ELSE

        // We no longer need this storage
        Myf_clear (&mpfNear);
        Myf_clear (&mpfTmp2);
        Myf_clear (&mpfTmp1);
        Myf_clear (&mpfCeil);
        Myf_clear (&mpfFloor);
    } // End IF/ELSE

    return mpfRes;
} // End PrimFnMonDownStileVisV


//***************************************************************************
//  $PrimFnMonDownStileAPA_EM
//
//  Monadic downstile, result is APA
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonDownStileAPA_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnMonDownStileAPA_EM
    (LPPL_YYSTYPE lpYYRes,          // Ptr to the result (may be NULL)
     LPTOKEN      lptkFunc,         // Ptr to function token
     HGLOBAL      hGlbRht,          // Right arg handle
     HGLOBAL     *lphGlbRes,        // Ptr to result handle
     APLRANK      aplRankRht,       // Rank fo the right arg
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
} // End PrimFnMonDownStileAPA_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecDownStileStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecDownStileStorageTypeDyd
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
} // End PrimSpecDownStileStorageTypeDyd


//***************************************************************************
//  $PrimFnDydDownStileIisIvI
//
//  Primitive scalar function dyadic DownStile:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydDownStileIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return min (aplIntegerLft, aplIntegerRht);
} // End PrimFnDydDownStileIisIvI


//***************************************************************************
//  $PrimFnDydDownStileFisFvF
//
//  Primitive scalar function dyadic DownStile:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydDownStileFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    return min (aplFloatLft, aplFloatRht);
} // End PrimFnDydDownStileFisFvF


//***************************************************************************
//  $PrimFnDydDownStileRisRvR
//
//  Primitive scalar function dyadic DownStile:  R {is} R fn R
//***************************************************************************

APLRAT PrimFnDydDownStileRisRvR
    (APLRAT     aplRatLft,
     APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLRAT mpqRes = {0};

    // Compare the two Rationals
    if (mpq_cmp (&aplRatLft, &aplRatRht) < 0)
        mpq_init_set (&mpqRes, &aplRatLft);
    else
        mpq_init_set (&mpqRes, &aplRatRht);

    return mpqRes;
} // End PrimFnDydDownStileRisRvR


//***************************************************************************
//  $PrimFnDydDownStileVisVvV
//
//  Primitive scalar function dyadic DownStile:  V {is} V fn V
//***************************************************************************

APLVFP PrimFnDydDownStileVisVvV
    (APLVFP     aplVfpLft,
     APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLVFP mpfRes = {0};

    // Compare the two Variable FPs
    if (mpfr_cmp (&aplVfpLft, &aplVfpRht) < 0)
        mpfr_init_copy (&mpfRes, &aplVfpLft);
    else
        mpfr_init_copy (&mpfRes, &aplVfpRht);

    return mpfRes;
} // End PrimFnDydDownStileVisVvV


//***************************************************************************
//  End of File: pf_dstile.c
//***************************************************************************
