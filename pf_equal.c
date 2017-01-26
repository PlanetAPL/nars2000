//***************************************************************************
//  NARS2000 -- Primitive Function -- Equal
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
PRIMSPEC PrimSpecEqual = {
    // Monadic functions
    &PrimFnMonValenceError_EM,
    NULL,   // &PrimSpecEqualStorageTypeMon, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualAPA_EM, -- Can't happen w/Equal

    NULL,   // &PrimFnMonEqualBisB, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualBisI, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualBisF, -- Can't happen w/Equal

////               IisB,   // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonEqualIisI, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualIisF, -- Can't happen w/Equal

////               FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonEqualFisI, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualFisF, -- Can't happen w/Equal

    NULL,   // &PrimFnMonEqualRisR, -- Can't happen w/Equal

////                          VisR      // Handled via type promotion (to VisV)
    NULL,   // &PrimFnMonEqualVisV, -- Can't happen w/Equal

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecEqualStorageTypeDyd,
    NULL,   // &PrimFnDydEqualAPA_EM, -- Can't happen w/Equal

    &PrimFnDydEqualBisBvB,
    &PrimFnDydEqualBisIvI,
    &PrimFnDydEqualBisFvF,
    &PrimFnDydEqualBisCvC,

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydEqualIisIvI, -- Result Boolean, can't happen w/Equal
    NULL,   // &PrimFnDydEqualIisFvF, -- Result Boolean, can't happen w/Equal

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydEqualFisIvI, -- Result Boolean, can't happen w/Equal
    NULL,   // &PrimFnDydEqualFisFvF, -- Result Boolean, can't happen w/Equal

    &PrimFnDydEqualBisRvR,
    NULL,   // &PrimFnDydEqualRisRvR, -- Result Boolean, can't happen w/Equal

    &PrimFnDydEqualBisVvV,
////                  VisRvR,   // Handled via type promotion (to VisVvV)
    NULL,   // &PrimFnDydUpEqualVisVvV, -- Result Boolean, can't happen w/Equal

    NULL,   // &PrimFnMonEqualB64isB64, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualB32isB32, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualB16isB16, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualB08isB08, -- Can't happen w/Equal

    &PrimFnDydEqualB64isB64vB64,
    &PrimFnDydEqualB32isB32vB32,
    &PrimFnDydEqualB16isB16vB16,
    &PrimFnDydEqualB08isB08vB08,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecEqual};
#endif


//***************************************************************************
//  $PrimFnEqual_EM_YY
//
//  Primitive function for monadic and dyadic Equal (ERROR and "equal")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnEqual_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnEqual_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnEqual_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecEqualStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecEqualStorageTypeDyd
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
} // End PrimSpecEqualStorageTypeDyd


//***************************************************************************
//  $PrimFnDydEqualBisBvB
//
//  Primitive scalar function dyadic Equal:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydEqualBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft EQ aplBooleanRht);
} // End PrimFnDydEqualBisBvB


//***************************************************************************
//  $PrimFnDydEqualB64isB64vB64
//
//  Primitive scalar function dyadic Equal:  B64 {is} B64 fn B64
//***************************************************************************

APLB64 PrimFnDydEqualB64isB64vB64
    (APLB64     aplBooleanLft,
     APLB64     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft ^ aplBooleanRht);
} // End PrimFnDydEqualB64isB64vB64


//***************************************************************************
//  $PrimFnDydEqualB32isB32vB32
//
//  Primitive scalar function dyadic Equal:  B32 {is} B32 fn B32
//***************************************************************************

APLB32 PrimFnDydEqualB32isB32vB32
    (APLB32     aplBooleanLft,
     APLB32     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft ^ aplBooleanRht);
} // End PrimFnDydEqualB32isB32vB32


//***************************************************************************
//  $PrimFnDydEqualB16isB16vB16
//
//  Primitive scalar function dyadic Equal:  B16 {is} B16 fn B16
//***************************************************************************

APLB16 PrimFnDydEqualB16isB16vB16
    (APLB16     aplBooleanLft,
     APLB16     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft ^ aplBooleanRht);
} // End PrimFnDydEqualB16isB16vB16


//***************************************************************************
//  $PrimFnDydEqualB08isB08vB08
//
//  Primitive scalar function dyadic Equal:  B08 {is} B08 fn B08
//***************************************************************************

APLB08 PrimFnDydEqualB08isB08vB08
    (APLB08     aplBooleanLft,
     APLB08     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft ^ aplBooleanRht);
} // End PrimFnDydEqualB08isB08vB08


//***************************************************************************
//  $PrimFnDydEqualBisIvI
//
//  Primitive scalar function dyadic Equal:  B {is} I fn I
//***************************************************************************

APLBOOL PrimFnDydEqualBisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplIntegerLft EQ aplIntegerRht);
} // End PrimFnDydEqualBisIvI


//***************************************************************************
//  $PrimFnDydEqualBisFvF
//
//  Primitive scalar function dyadic Equal:  B {is} F fn F
//***************************************************************************

APLBOOL PrimFnDydEqualBisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Compare the two floats relative to []CT
    return CompareCT (aplFloatLft, aplFloatRht, GetQuadCT (), lpPrimSpec);
} // End PrimFnDydEqualBisFvF


//***************************************************************************
//  $PrimFnDydEqualBisCvC
//
//  Primitive scalar function dyadic Equal:  B {is} C fn C
//***************************************************************************

APLBOOL PrimFnDydEqualBisCvC
    (APLCHAR    aplCharLft,
     APLCHAR    aplCharRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplCharLft EQ aplCharRht);
} // End PrimFnDydEqualBisCvC


//***************************************************************************
//  $PrimFnDydEqualRisRvR
//
//  Primitive scalar function dyadic Equal:  B {is} R fn R
//***************************************************************************

APLBOOL PrimFnDydEqualBisRvR
    (APLRAT     aplRatLft,
     APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
#ifdef RAT_EXACT
    // Compare the two RATs
    return mpq_cmp    (&aplRatLft, &aplRatRht              ) EQ 0;
#else
    // Compare the two RATs relative to []CT
    return mpq_cmp_ct ( aplRatLft,  aplRatRht, GetQuadCT ()) EQ 0;
#endif
} // End PrimFnDydEqualBisRvR


//***************************************************************************
//  $PrimFnDydEqualBisVvV
//
//  Primitive scalar function dyadic Equal:  B {is} V fn V
//***************************************************************************

APLBOOL PrimFnDydEqualBisVvV
    (APLVFP     aplVfpLft,
     APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Compare the two VFPs relative to []CT
    return (mpfr_cmp_ct (aplVfpLft, aplVfpRht, GetQuadCT ()) EQ 0);
} // End PrimFnDydEqualBisVvV


//***************************************************************************
//  End of File: pf_equal.c
//***************************************************************************
