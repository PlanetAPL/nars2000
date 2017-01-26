//***************************************************************************
//  NARS2000 -- Primitive Function -- NotEqual
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
PRIMSPEC PrimSpecNotEqual = {
    // Monadic functions
    &PrimFnMonValenceError_EM,
    NULL,   // &PrimSpecNotEqualStorageTypeMon, -- Can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualAPA_EM, -- Can't happen w/NotEqual

    NULL,   // &PrimFnMonNotEqualBisB, -- Can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualBisI, -- Can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualBisF, -- Can't happen w/NotEqual

////&PrimFnMonNotEqualIisB,   // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonNotEqualIisI, -- Can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualIisF, -- Can't happen w/NotEqual

////&PrimFnMonNotEqualFisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonNotEqualFisI, -- Can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualFisF, -- Can't happen w/NotEqual

    NULL,   // &PrimFnMonNotEqualRisR, -- Can't happen w/NotEqual

////                          VisR      // Handled via type promotion (to VisV)
    NULL,   // &PrimFnMonNotEqualVisV, -- Can't happen w/NotEqual

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecNotEqualStorageTypeDyd,
    NULL,   // &PrimFnDydNotEqualAPA_EM, -- Can't happen w/NotEqual

    &PrimFnDydNotEqualBisBvB,
    &PrimFnDydNotEqualBisIvI,
    &PrimFnDydNotEqualBisFvF,
    &PrimFnDydNotEqualBisCvC,

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydNotEqualIisIvI, -- Result Boolean, can't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualIisFvF, -- Result Boolean, can't happen w/NotEqual

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydNotEqualFisIvI, -- Result Boolean, can't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualFisFvF, -- Result Boolean, cCan't happen w/NotEqual

    &PrimFnDydNotEqualBisRvR,
    NULL,   // &PrimFnDydNotEqualRisRvR, -- Result Boolean, can't happen w/NotEqual

    &PrimFnDydNotEqualBisVvV,
////                  VisRvR,   // Handled via type promotion (to VisVvV)
    NULL,   // &PrimFnDydNotEqualVisVvV, -- Result Boolean, can't happen w/NotEqual

    NULL,   // &PrimFnMonNotEqualB64isB64, -- Can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualB32isB32, -- Can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualB16isB16, -- Can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualB08isB08, -- Can't happen w/NotEqual

    &PrimFnDydNotEqualB64isB64vB64,
    &PrimFnDydNotEqualB32isB32vB32,
    &PrimFnDydNotEqualB16isB16vB16,
    &PrimFnDydNotEqualB08isB08vB08,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecNotEqual};
#endif


//***************************************************************************
//  $PrimFnNotEqual_EM_YY
//
//  Primitive function for monadic and dyadic NotEqual (ERROR and "NotEqual")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnNotEqual_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnNotEqual_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnNotEqual_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecNotEqualStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecNotEqualStorageTypeDyd
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
} // End PrimSpecNotEqualStorageTypeDyd


//***************************************************************************
//  $PrimFnDydNotEqualBisBvB
//
//  Primitive scalar function dyadic NotEqual:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydNotEqualBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft NE aplBooleanRht);
} // End PrimFnDydNotEqualBisBvB


//***************************************************************************
//  $PrimFnDydNotEqualB64isB64vB64
//
//  Primitive scalar function dyadic NotEqual:  B64 {is} B64 fn B64
//***************************************************************************

APLB64 PrimFnDydNotEqualB64isB64vB64
    (APLB64     aplBooleanLft,
     APLB64     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft ^ aplBooleanRht);
} // End PrimFnDydNotEqualB64isB64vB64


//***************************************************************************
//  $PrimFnDydNotEqualB32isB32vB32
//
//  Primitive scalar function dyadic NotEqual:  B32 {is} B32 fn B32
//***************************************************************************

APLB32 PrimFnDydNotEqualB32isB32vB32
    (APLB32     aplBooleanLft,
     APLB32     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft ^ aplBooleanRht);
} // End PrimFnDydNotEqualB32isB32vB32


//***************************************************************************
//  $PrimFnDydNotEqualB16isB16vB16
//
//  Primitive scalar function dyadic NotEqual:  B16 {is} B16 fn B16
//***************************************************************************

APLB16 PrimFnDydNotEqualB16isB16vB16
    (APLB16     aplBooleanLft,
     APLB16     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft ^ aplBooleanRht);
} // End PrimFnDydNotEqualB16isB16vB16


//***************************************************************************
//  $PrimFnDydNotEqualB08isB08vB08
//
//  Primitive scalar function dyadic NotEqual:  B08 {is} B08 fn B08
//***************************************************************************

APLB08 PrimFnDydNotEqualB08isB08vB08
    (APLB08     aplBooleanLft,
     APLB08     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft ^ aplBooleanRht);
} // End PrimFnDydNotEqualB08isB08vB08


//***************************************************************************
//  $PrimFnDydNotEqualBisIvI
//
//  Primitive scalar function dyadic NotEqual:  B {is} I fn I
//***************************************************************************

APLBOOL PrimFnDydNotEqualBisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplIntegerLft NE aplIntegerRht);
} // End PrimFnDydNotEqualBisIvI


//***************************************************************************
//  $PrimFnDydNotEqualBisFvF
//
//  Primitive scalar function dyadic NotEqual:  B {is} F fn F
//***************************************************************************

APLBOOL PrimFnDydNotEqualBisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Compare the two floats relative to []CT
    return !CompareCT (aplFloatLft, aplFloatRht, GetQuadCT (), lpPrimSpec);
} // End PrimFnDydNotEqualBisFvF


//***************************************************************************
//  $PrimFnDydNotEqualBisCvC
//
//  Primitive scalar function dyadic NotEqual:  B {is} C fn C
//***************************************************************************

APLBOOL PrimFnDydNotEqualBisCvC
    (APLCHAR    aplCharLft,
     APLCHAR    aplCharRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplCharLft NE aplCharRht);
} // End PrimFnDydNotEqualBisCvC


//***************************************************************************
//  $PrimFnDydNotEqualRisRvR
//
//  Primitive scalar function dyadic NotEqual:  B {is} R fn R
//***************************************************************************

APLBOOL PrimFnDydNotEqualBisRvR
    (APLRAT     aplRatLft,
     APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
#ifdef RAT_EXACT
    // Compare the two RATs
    return mpq_cmp    (&aplRatLft, &aplRatRht              ) NE 0;
#else
    // Compare the two RATs relative to []CT
    return mpq_cmp_ct ( aplRatLft,  aplRatRht, GetQuadCT ()) NE 0;
#endif
} // End PrimFnDydNotEqualBisRvR


//***************************************************************************
//  $PrimFnDydNotEqualBisVvV
//
//  Primitive scalar function dyadic NotEqual:  B {is} V fn V
//***************************************************************************

APLBOOL PrimFnDydNotEqualBisVvV
    (APLVFP     aplVfpLft,
     APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Compare the two VFPs relative to []CT
    return (mpfr_cmp_ct (aplVfpLft, aplVfpRht, GetQuadCT ()) NE 0);
} // End PrimFnDydNotEqualBisVvV


//***************************************************************************
//  End of File: pf_nequal.c
//***************************************************************************
