//***************************************************************************
//  NARS2000 -- Primitive Function -- LeftCaretUnderbar
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
PRIMSPEC PrimSpecLeftCaretUnderbar = {
    // Monadic functions
    &PrimFnMonValenceError_EM,
    NULL,   // &PrimSpecLeftCaretUnderbarStorageTypeMon, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarAPA_EM, -- Can't happen w/LeftCaretUnderbar

    NULL,   // &PrimFnMonLeftCaretUnderbarBisB, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarBisI, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarBisF, -- Can't happen w/LeftCaretUnderbar

////                 IisB,   // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonLeftCaretUnderbarIisI, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarIisF, -- Can't happen w/LeftCaretUnderbar

////                 FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonLeftCaretUnderbarFisI, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarFisF, -- Can't happen w/LeftCaretUnderbar

    NULL,   // &PrimFnMonLeftCaretUnderbarRisR, -- Can't happen w/LeftCaretUnderbar

////               VisR,    // Handled via type promotion (to VisV)
    NULL,   // &PrimFnMonLeftCaretUnderbarVisV, -- Can't happen w/LeftCaretUnderbar

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecLeftCaretUnderbarStorageTypeDyd,
    NULL,   // &PrimFnDydLeftCaretUnderbarAPA_EM, -- Can't happen w/LeftCaretUnderbar

    &PrimFnDydLeftCaretUnderbarBisBvB,
    &PrimFnDydLeftCaretUnderbarBisIvI,
    &PrimFnDydLeftCaretUnderbarBisFvF,
    NULL,   // &PrimFnDydLeftCaretUnderbarBisCvC, -- Can't happen w/LeftCaretUnderbar

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydLeftCaretUnderbarIisIvI, -- Result Boolean, can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarIisFvF, -- Result Boolean, can't happen w/LeftCaretUnderbar

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydLeftCaretUnderbarFisIvI, -- Result Boolean, can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarFisFvF, -- Result Boolean, can't happen w/LeftCaretUnderbar

    &PrimFnDydLeftCaretUnderbarBisRvR,
    NULL,   // &PrimFnDydLeftCaretUnderbarRisRvR, -- Result Boolean, can't happen w/LeftCaretUnderbar

    &PrimFnDydLeftCaretUnderbarBisVvV,
////                 VisRvR     // Handled via type promotion (to VisVvV)
    NULL,   // &PrimFnDydLeftCaretUnderbarVisVvV, -- Result Boolean, can't happen w/LeftCaretUnderbar

    NULL,   // &PrimFnMonLeftCaretUnderbarB64isB64, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarB32isB32, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarB16isB16, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarB08isB08, -- Can't happen w/LeftCaretUnderbar

    &PrimFnDydLeftCaretUnderbarB64isB64vB64,
    &PrimFnDydLeftCaretUnderbarB32isB32vB32,
    &PrimFnDydLeftCaretUnderbarB16isB16vB16,
    &PrimFnDydLeftCaretUnderbarB08isB08vB08,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecLeftCaretUnderbar};
#endif


//***************************************************************************
//  $PrimFnLeftCaretUnderbar_EM_YY
//
//  Primitive function for monadic and dyadic LeftCaretUnderbar (ERROR and "less than or equal")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnLeftCaretUnderbar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnLeftCaretUnderbar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (IsAPLCharNotMore (lptkFunc->tkData.tkChar));

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnLeftCaretUnderbar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecLeftCaretUnderbarStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecLeftCaretUnderbarStorageTypeDyd
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

    // All numeric results are Boolean
    if (IsNumeric (aplTypeRes))
        aplTypeRes = ARRAY_BOOL;

    return aplTypeRes;
} // End PrimSpecLeftCaretUnderbarStorageTypeDyd


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarBisBvB
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydLeftCaretUnderbarBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft <= aplBooleanRht);
} // End PrimFnDydLeftCaretUnderbarBisBvB


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarB64isB64vB64
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B64 {is} B64 fn B64
//***************************************************************************

APLB64 PrimFnDydLeftCaretUnderbarB64isB64vB64
    (APLB64     aplBooleanLft,
     APLB64     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft & ~aplBooleanRht);
} // End PrimFnDydLeftCaretUnderbarB64isB64vB64


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarB32isB32vB32
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B32 {is} B32 fn B32
//***************************************************************************

APLB32 PrimFnDydLeftCaretUnderbarB32isB32vB32
    (APLB32     aplBooleanLft,
     APLB32     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft & ~aplBooleanRht);
} // End PrimFnDydLeftCaretUnderbarB32isB32vB32


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarB16isB16vB16
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B16 {is} B16 fn B16
//***************************************************************************

APLB16 PrimFnDydLeftCaretUnderbarB16isB16vB16
    (APLB16     aplBooleanLft,
     APLB16     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft & ~aplBooleanRht);
} // End PrimFnDydLeftCaretUnderbarB16isB16vB16


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarB08isB08vB08
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B08 {is} B08 fn B08
//***************************************************************************

APLB08 PrimFnDydLeftCaretUnderbarB08isB08vB08
    (APLB08     aplBooleanLft,
     APLB08     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft & ~aplBooleanRht);
} // End PrimFnDydLeftCaretUnderbarB08isB08vB08


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarBisIvI
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B {is} I fn I
//***************************************************************************

APLBOOL PrimFnDydLeftCaretUnderbarBisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplIntegerLft <= aplIntegerRht);
} // End PrimFnDydLeftCaretUnderbarBisIvI


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarBisFvF
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B {is} F fn F
//***************************************************************************

APLBOOL PrimFnDydLeftCaretUnderbarBisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // If the left and right arguments are tolerantly-equal, return 1
    if (PrimFnDydEqualBisFvF (aplFloatLft, aplFloatRht, lpPrimSpec))
        return 1;

    // Otherwise, return the natural result
    return (aplFloatLft <= aplFloatRht);
} // End PrimFnDydLeftCaretUnderbarBisFvF


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarBisRvR
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B {is} R fn R
//***************************************************************************

APLBOOL PrimFnDydLeftCaretUnderbarBisRvR
    (APLRAT     aplRatLft,
     APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
#ifdef RAT_EXACT
    // Compare the two RATs
    return mpq_cmp    (&aplRatLft, &aplRatRht              ) <= 0;
#else
    // Compare the two RATs relative to []CT
    return mpq_cmp_ct ( aplRatLft,  aplRatRht, GetQuadCT ()) <= 0;
#endif
} // End PrimFnDydLeftCaretUnderbarBisRvR


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarBisVvV
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B {is} V fn V
//***************************************************************************

APLBOOL PrimFnDydLeftCaretUnderbarBisVvV
    (APLVFP     aplVfpLft,
     APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Compare the two VFPs relative to []CT
    return (mpfr_cmp_ct (aplVfpLft, aplVfpRht, GetQuadCT ()) <= 0);
} // End PrimFnDydLeftCaretUnderbarBisVvV


//***************************************************************************
//  End of File: pf_lcaretund.c
//***************************************************************************
