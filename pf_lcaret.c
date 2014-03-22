//***************************************************************************
//  NARS2000 -- Primitive Function -- LeftCaret
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2012 Sudley Place Software

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
PRIMSPEC PrimSpecLeftCaret = {
    // Monadic functions
    &PrimFnMonValenceError_EM,
    NULL,   // &PrimSpecLeftCaretStorageTypeMon, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretAPA_EM, -- Can't happen w/LeftCaret

    NULL,   // &PrimFnMonLeftCaretBisB, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretBisI, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretBisF, -- Can't happen w/LeftCaret

////                   IisB,   // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonLeftCaretIisI, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretIisF, -- Can't happen w/LeftCaret

////                   FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonLeftCaretFisI, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretFisF, -- Can't happen w/LeftCaret

    NULL,   // &PrimFnMonLeftCaretRisR, -- Can't happen w/LeftCaret

////               VisR,    // Handled via type promotion (to VisV)
    NULL,   // &PrimFnMonLeftCaretVisV, -- Can't happen w/LeftCaret

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecLeftCaretStorageTypeDyd,
    NULL,   // &PrimFnDydLeftCaretAPA_EM, -- Can't happen w/LeftCaret

    &PrimFnDydLeftCaretBisBvB,
    &PrimFnDydLeftCaretBisIvI,
    &PrimFnDydLeftCaretBisFvF,
    NULL,   // &PrimFnDydLeftCaretBisCvC, -- Can't happen w/LeftCaret

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydLeftCaretIisIvI, -- Result Boolean, can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretIisFvF, -- Result Boolean, can't happen w/LeftCaret

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydLeftCaretFisIvI, -- Result Boolean, can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretFisFvF, -- Result Boolean, can't happen w/LeftCaret

    &PrimFnDydLeftCaretBisRvR,
    NULL,   // &PrimFnDydLeftCaretRisRvR, -- Result Boolean, can't happen w/LeftCaret

    &PrimFnDydLeftCaretBisVvV,
////                 VisRvR     // Handled via type promotion (to VisVvV)
    NULL,   // &PrimFnDydLeftCaretVisVvV, -- Result Boolean, can't happen w/LeftCaret

    NULL,   // &PrimFnMonLeftCaretB64isB64, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretB32isB32, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretB16isB16, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretB08isB08, -- Can't happen w/LeftCaret

    &PrimFnDydLeftCaretB64isB64vB64,
    &PrimFnDydLeftCaretB32isB32vB32,
    &PrimFnDydLeftCaretB16isB16vB16,
    &PrimFnDydLeftCaretB08isB08vB08,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecLeftCaret};
#endif


//***************************************************************************
//  $PrimFnLeftCaret_EM_YY
//
//  Primitive function for monadic and dyadic LeftCaret (ERROR and "greater than")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnLeftCaret_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnLeftCaret_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_LEFTCARET);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnLeftCaret_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecLeftCaretStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecLeftCaretStorageTypeDyd
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
} // End PrimSpecLeftCaretStorageTypeDyd


//***************************************************************************
//  $PrimFnDydLeftCaretBisBvB
//
//  Primitive scalar function dyadic LeftCaret:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydLeftCaretBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanLft < aplBooleanRht;
} // End PrimFnDydLeftCaretBisBvB


//***************************************************************************
//  $PrimFnDydLeftCaretB64isB64vB64
//
//  Primitive scalar function dyadic LeftCaret:  B64 {is} B64 fn B64
//***************************************************************************

APLB64 PrimFnDydLeftCaretB64isB64vB64
    (APLB64     aplBooleanLft,
     APLB64     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (~aplBooleanLft) & aplBooleanRht;
} // End PrimFnDydLeftCaretB64isB64vB64


//***************************************************************************
//  $PrimFnDydLeftCaretB32isB32vB32
//
//  Primitive scalar function dyadic LeftCaret:  B32 {is} B32 fn B32
//***************************************************************************

APLB32 PrimFnDydLeftCaretB32isB32vB32
    (APLB32     aplBooleanLft,
     APLB32     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (~aplBooleanLft) & aplBooleanRht;
} // End PrimFnDydLeftCaretB32isB32vB32


//***************************************************************************
//  $PrimFnDydLeftCaretB16isB16vB16
//
//  Primitive scalar function dyadic LeftCaret:  B16 {is} B16 fn B16
//***************************************************************************

APLB16 PrimFnDydLeftCaretB16isB16vB16
    (APLB16     aplBooleanLft,
     APLB16     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (~aplBooleanLft) & aplBooleanRht;
} // End PrimFnDydLeftCaretB16isB16vB16


//***************************************************************************
//  $PrimFnDydLeftCaretB08isB08vB08
//
//  Primitive scalar function dyadic LeftCaret:  B08 {is} B08 fn B08
//***************************************************************************

APLB08 PrimFnDydLeftCaretB08isB08vB08
    (APLB08     aplBooleanLft,
     APLB08     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (~aplBooleanLft) & aplBooleanRht;
} // End PrimFnDydLeftCaretB08isB08vB08


//***************************************************************************
//  $PrimFnDydLeftCaretBisIvI
//
//  Primitive scalar function dyadic LeftCaret:  B {is} I fn I
//***************************************************************************

APLBOOL PrimFnDydLeftCaretBisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplIntegerLft < aplIntegerRht);
} // End PrimFnDydLeftCaretBisIvI


//***************************************************************************
//  $PrimFnDydLeftCaretBisFvF
//
//  Primitive scalar function dyadic LeftCaret:  B {is} F fn F
//***************************************************************************

APLBOOL PrimFnDydLeftCaretBisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // If the left and right arguments are tolerantly-equal, return 0
    if (PrimFnDydEqualBisFvF (aplFloatLft, aplFloatRht, lpPrimSpec))
        return 0;

    // Otherwise, return the natural result
    return (aplFloatLft < aplFloatRht);
} // End PrimFnDydLeftCaretBisFvF


//***************************************************************************
//  $PrimFnDydLeftCaretBisRvR
//
//  Primitive scalar function dyadic LeftCaret:  B {is} R fn R
//***************************************************************************

APLBOOL PrimFnDydLeftCaretBisRvR
    (APLRAT     aplRatLft,
     APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
#ifdef RAT_EXACT
    // Compare the two RATs
    return mpq_cmp    (&aplRatLft, &aplRatRht              ) < 0;
#else
    // Compare the two RATs relative to []CT
    return mpq_cmp_ct ( aplRatLft,  aplRatRht, GetQuadCT ()) < 0;
#endif
} // End PrimFnDydLeftCaretBisRvR


//***************************************************************************
//  $PrimFnDydLeftCaretBisVvV
//
//  Primitive scalar function dyadic LeftCaret:  B {is} V fn V
//***************************************************************************

APLBOOL PrimFnDydLeftCaretBisVvV
    (APLVFP     aplVfpLft,
     APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Compare the two VFPs relative to []CT
    return (mpfr_cmp_ct (aplVfpLft, aplVfpRht, GetQuadCT ()) < 0);
} // End PrimFnDydLeftCaretBisVvV


//***************************************************************************
//  End of File: pf_lcaret.c
//***************************************************************************
