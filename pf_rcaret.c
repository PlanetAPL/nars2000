//***************************************************************************
//  NARS2000 -- Primitive Function -- RightCaret
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
PRIMSPEC PrimSpecRightCaret = {
    // Monadic functions
    NULL,   // &PrimFnMon_EM, -- Can't happen with RightCaret
    NULL,   // &PrimSpecRightCaretStorageTypeMon, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretAPA_EM, -- Can't happen w/RightCaret

    NULL,   // &PrimFnMonRightCaretBisB, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretBisI, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretBisF, -- Can't happen w/RightCaret

////                    IisB,   // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonRightCaretIisI, -- Can't happend w/RightCaret
    NULL,   // &PrimFnMonRightCaretIisF, -- Can't happend w/RightCaret

////                    FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonRightCaretFisI, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretFisF, -- Can't happen w/RightCaret

    NULL,   // &PrimFnMonRightCaretRisR, -- Can't happen w/RightCaret

////               VisR,    // Handled via type promotion (to VisV)
    NULL,   // &PrimFnMonRightCaretVisV, -- Can't happen w/RightCaret

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecRightCaretStorageTypeDyd,
    NULL,   // &PrimFnDydRightCaretAPA_EM, -- Can't happen w/RightCaret

    &PrimFnDydRightCaretBisBvB,
    &PrimFnDydRightCaretBisIvI,
    &PrimFnDydRightCaretBisFvF,
    NULL,   // &PrimFnDydRightCaretBisCvC, -- Can't happen w/RightCaret

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydRightCaretIisIvI, -- Result Boolean, can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretIisFvF, -- Result Boolean, can't happen w/RightCaret

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydRightCaretFisIvI, -- Result Boolean, can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretFisFvF, -- Result Boolean, can't happen w/RightCaret

    &PrimFnDydRightCaretBisRvR,
    NULL,   // &PrimFnDydRightCaretRisRvR, -- Result Boolean, can't happen w/RightCaret

    &PrimFnDydRightCaretBisVvV,
////                 VisRvR     // Handled via type promotion (to VisVvV)
    NULL,   // &PrimFnDydRightCaretVisVvV, -- Result Boolean, can't happen w/RightCaret

    NULL,   // &PrimFnMonRightCaretB64isB64, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretB32isB32, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretB16isB16, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretB08isB08, -- Can't happen w/RightCaret

    &PrimFnDydRightCaretB64isB64vB64,
    &PrimFnDydRightCaretB32isB32vB32,
    &PrimFnDydRightCaretB16isB16vB16,
    &PrimFnDydRightCaretB08isB08vB08,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecRightCaret};
#endif


//***************************************************************************
//  $PrimFnRightCaret_EM_YY
//
//  Primitive function for monadic and dyadic RightCaret (ERROR and "less than")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnRightCaret_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnRightCaret_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_RIGHTCARET);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonRightCaret_EM_YY      (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnRightCaret_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonRightCaret_EM_YY
//
//  Primitive function for monadic RightCaret (ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonRightCaret_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonRightCaret_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End PrimFnMonRightCaret_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecRightCaretStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecRightCaretStorageTypeDyd
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
} // End PrimSpecRightCaretStorageTypeDyd


//***************************************************************************
//  $PrimFnDydRightCaretBisBvB
//
//  Primitive scalar function dyadic RightCaret:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydRightCaretBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanLft > aplBooleanRht;
} // End PrimFnDydRightCaretBisBvB


//***************************************************************************
//  $PrimFnDydRightCaretB64isB64vB64
//
//  Primitive scalar function dyadic RightCaret:  B64 {is} B64 fn B64
//***************************************************************************

APLB64 PrimFnDydRightCaretB64isB64vB64
    (APLB64     aplBooleanLft,
     APLB64     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanLft & ~aplBooleanRht;
} // End PrimFnDydRightCaretB64isB64vB64


//***************************************************************************
//  $PrimFnDydRightCaretB32isB32vB32
//
//  Primitive scalar function dyadic RightCaret:  B32 {is} B32 fn B32
//***************************************************************************

APLB32 PrimFnDydRightCaretB32isB32vB32
    (APLB32     aplBooleanLft,
     APLB32     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanLft & ~aplBooleanRht;
} // End PrimFnDydRightCaretB32isB32vB32


//***************************************************************************
//  $PrimFnDydRightCaretB16isB16vB16
//
//  Primitive scalar function dyadic RightCaret:  B16 {is} B16 fn B16
//***************************************************************************

APLB16 PrimFnDydRightCaretB16isB16vB16
    (APLB16     aplBooleanLft,
     APLB16     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanLft & ~aplBooleanRht;
} // End PrimFnDydRightCaretB16isB16vB16


//***************************************************************************
//  $PrimFnDydRightCaretB08isB08vB08
//
//  Primitive scalar function dyadic RightCaret:  B08 {is} B08 fn B08
//***************************************************************************

APLB08 PrimFnDydRightCaretB08isB08vB08
    (APLB08     aplBooleanLft,
     APLB08     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanLft & ~aplBooleanRht;
} // End PrimFnDydRightCaretB08isB08vB08


//***************************************************************************
//  $PrimFnDydRightCaretBisIvI
//
//  Primitive scalar function dyadic RightCaret:  B {is} I fn I
//***************************************************************************

APLBOOL PrimFnDydRightCaretBisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplIntegerLft > aplIntegerRht);
} // End PrimFnDydRightCaretBisIvI


//***************************************************************************
//  $PrimFnDydRightCaretBisFvF
//
//  Primitive scalar function dyadic RightCaret:  B {is} F fn F
//***************************************************************************

APLBOOL PrimFnDydRightCaretBisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // If the left and right arguments are tolerantly-equal, return 0
    if (PrimFnDydEqualBisFvF (aplFloatLft, aplFloatRht, lpPrimSpec))
        return 0;

    // Otherwise, return the natural result
    return (aplFloatLft > aplFloatRht);
} // End PrimFnDydRightCaretBisFvF


//***************************************************************************
//  $PrimFnDydRightCaretBisRvR
//
//  Primitive scalar function dyadic RightCaret:  B {is} R fn R
//***************************************************************************

APLBOOL PrimFnDydRightCaretBisRvR
    (APLRAT     aplRatLft,
     APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
#ifdef RAT_EXACT
    // Compare the two RATs
    return mpq_cmp    (&aplRatLft, &aplRatRht              ) > 0;
#else
    // Compare the two RATs relative to []CT
    return mpq_cmp_ct ( aplRatLft,  aplRatRht, GetQuadCT ()) > 0;
#endif
} // End PrimFnDydRightCaretBisRvR


//***************************************************************************
//  $PrimFnDydRightCaretBisVvV
//
//  Primitive scalar function dyadic RightCaret:  B {is} V fn V
//***************************************************************************

APLBOOL PrimFnDydRightCaretBisVvV
    (APLVFP     aplVfpLft,
     APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Compare the two VFPs relative to []CT
    return (mpfr_cmp_ct (aplVfpLft, aplVfpRht, GetQuadCT ()) > 0);
} // End PrimFnDydRightCaretBisVvV


//***************************************************************************
//  End of File: pf_rcaret.c
//***************************************************************************
