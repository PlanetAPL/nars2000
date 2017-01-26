//***************************************************************************
//  NARS2000 -- Primitive Function -- RightCaretUnderbar
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
PRIMSPEC PrimSpecRightCaretUnderbar = {
    // Monadic functions
    &PrimFnMonValenceError_EM,
    NULL,   // &PrimSpecRightCaretUnderbarStorageTypeMon, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarAPA_EM, -- Can't happen w/RightCaretUnderbar

    NULL,   // &PrimFnMonRightCaretUnderbarBisB, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarBisI, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarBisF, -- Can't happen w/RightCaretUnderbar

////                 IisB,   // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonRightCaretUnderbarIisI, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarIisF, -- Can't happen w/RightCaretUnderbar

////                 FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonRightCaretUnderbarFisI, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarFisF, -- Can't happen w/RightCaretUnderbar

    NULL,   // &PrimFnMonRightCaretUnderbarRisR, -- Can't happen w/RightCaretUnderbar

////                 VisR,   // Handled via type promotion (to VisV)
    NULL,   // &PrimFnMonRightCaretUnderbarVisV, -- Can't happen w/RightCaretUnderbar

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecRightCaretUnderbarStorageTypeDyd,
    NULL,   // &PrimFnDydRightCaretUnderbarAPA_EM, -- Can't happen w/RightCaretUnderbar

    &PrimFnDydRightCaretUnderbarBisBvB,
    &PrimFnDydRightCaretUnderbarBisIvI,
    &PrimFnDydRightCaretUnderbarBisFvF,
    NULL,   // &PrimFnDydRightCaretUnderbarBisCvC, -- Can't happen w/RightCaretUnderbar

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydRightCaretUnderbarIisIvI, -- Result Boolean, can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarIisFvF, -- Result Boolean, can't happen w/RightCaretUnderbar

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydRightCaretUnderbarFisIvI, -- Result Boolean, can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarFisFvF, -- Result Boolean, can't happen w/RightCaretUnderbar

    &PrimFnDydRightCaretUnderbarBisRvR,
    NULL,   // &PrimFnDydRightCaretUnderbarRisRvR, -- Result Boolean, can't happen w/RightCaretUnderbar

    &PrimFnDydRightCaretUnderbarBisVvV,
////                 VisRvR     // Handled via type promotion (to VisVvV)
    NULL,   // &PrimFnDydRightCaretUnderbarVisVvV, -- Result Boolean, can't happen w/RightCaretUnderbar

    NULL,   // &PrimFnMonRightCaretUnderbarB64isB64, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarB32isB32, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarB16isB16, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarB08isB08, -- Can't happen w/RightCaretUnderbar

    &PrimFnDydRightCaretUnderbarB64isB64vB64,
    &PrimFnDydRightCaretUnderbarB32isB32vB32,
    &PrimFnDydRightCaretUnderbarB16isB16vB16,
    &PrimFnDydRightCaretUnderbarB08isB08vB08,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecRightCaretUnderbar};
#endif


//***************************************************************************
//  $PrimFnRightCaretUnderbar_EM_YY
//
//  Primitive function for monadic and dyadic RightCaretUnderbar (ERROR and "not less")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnRightCaretUnderbar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnRightCaretUnderbar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (IsAPLCharNotLess (lptkFunc->tkData.tkChar));

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnRightCaretUnderbar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecRightCaretUnderbarStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecRightCaretUnderbarStorageTypeDyd
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

    // All numeric results are Boolean
    if (IsNumeric (aplTypeRes))
        aplTypeRes = ARRAY_BOOL;

    return aplTypeRes;
} // End PrimSpecRightCaretUnderbarStorageTypeDyd


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarBisBvB
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydRightCaretUnderbarBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft >= aplBooleanRht);
} // End PrimFnDydRightCaretUnderbarBisBvB


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarB64isB64vB64
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B64 {is} B64 fn B64
//***************************************************************************

APLB64 PrimFnDydRightCaretUnderbarB64isB64vB64
    (APLB64     aplBooleanLft,
     APLB64     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~((~aplBooleanLft) & aplBooleanRht);
} // End PrimFnDydRightCaretUnderbarB64isB64vB64


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarB32isB32vB32
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B32 {is} B32 fn B32
//***************************************************************************

APLB32 PrimFnDydRightCaretUnderbarB32isB32vB32
    (APLB32     aplBooleanLft,
     APLB32     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~((~aplBooleanLft) & aplBooleanRht);
} // End PrimFnDydRightCaretUnderbarB32isB32vB32


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarB16isB16vB16
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B16 {is} B16 fn B16
//***************************************************************************

APLB16 PrimFnDydRightCaretUnderbarB16isB16vB16
    (APLB16     aplBooleanLft,
     APLB16     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~((~aplBooleanLft) & aplBooleanRht);
} // End PrimFnDydRightCaretUnderbarB16isB16vB16


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarB08isB08vB08
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B08 {is} B08 fn B08
//***************************************************************************

APLB08 PrimFnDydRightCaretUnderbarB08isB08vB08
    (APLB08     aplBooleanLft,
     APLB08     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~((~aplBooleanLft) & aplBooleanRht);
} // End PrimFnDydRightCaretUnderbarB08isB08vB08


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarBisIvI
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B {is} I fn I
//***************************************************************************

APLBOOL PrimFnDydRightCaretUnderbarBisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplIntegerLft >= aplIntegerRht);
} // End PrimFnDydRightCaretUnderbarBisIvI


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarBisFvF
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B {is} F fn F
//***************************************************************************

APLBOOL PrimFnDydRightCaretUnderbarBisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // If the left and right arguments are tolerantly-equal, return 1
    if (PrimFnDydEqualBisFvF (aplFloatLft, aplFloatRht, lpPrimSpec))
        return 1;

    // Otherwise, return the natural result
    return (aplFloatLft >= aplFloatRht);
} // End PrimFnDydRightCaretUnderbarBisFvF


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarBisRvR
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B {is} R fn R
//***************************************************************************

APLBOOL PrimFnDydRightCaretUnderbarBisRvR
    (APLRAT     aplRatLft,
     APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
#ifdef RAT_EXACT
    // Compare the two RATs
    return mpq_cmp    (&aplRatLft, &aplRatRht              ) >= 0;
#else
    // Compare the two RATs relative to []CT
    return mpq_cmp_ct ( aplRatLft,  aplRatRht, GetQuadCT ()) >= 0;
#endif
} // End PrimFnDydRightCaretUnderbarBisRvR


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarBisVvV
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B {is} V fn V
//***************************************************************************

APLBOOL PrimFnDydRightCaretUnderbarBisVvV
    (APLVFP     aplVfpLft,
     APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Compare the two VFPs relative to []CT
    return (mpfr_cmp_ct (aplVfpLft, aplVfpRht, GetQuadCT ()) >= 0);
} // End PrimFnDydRightCaretUnderbarBisVvV


//***************************************************************************
//  End of File: pf_rcaretund.c
//***************************************************************************
