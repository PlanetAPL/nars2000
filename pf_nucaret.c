//***************************************************************************
//  NARS2000 -- Primitive Function -- UpCaretTilde
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
PRIMSPEC PrimSpecUpCaretTilde = {
    // Monadic functions
    &PrimFnMonValenceError_EM,
    NULL,   // &PrimSpecUpCaretTildeStorageTypeMon, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeAPA_EM, -- Can't happen w/UpCaretTilde

    NULL,   // &PrimFnMonUpCaretTildeBisB, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeBisI, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeBisF, -- Can't happen w/UpCaretTilde

////                      IisB,   // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonUpCaretTildeIisI, -- Can't happend w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeIisF, -- Can't happend w/UpCaretTilde

////                      FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonUpCaretTildeFisI, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeFisF, -- Can't happen w/UpCaretTilde

    NULL,   // &PrimFnMonUpCaretTildeRisR, -- Can't happen w/UpCaretTilde

////               VisR,    // Handled via type promotion (to VisV)
    NULL,   // &PrimFnMonUpCaretTildeVisV, -- Can't happen w/UpCaretTilde

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecUpCaretTildeStorageTypeDyd,
    NULL,   // &PrimFnDydUpCaretTildeAPA_EM, -- Can't happen w/UpCaretTilde

    &PrimFnDydUpCaretTildeBisBvB,
    &PrimFnDydUpCaretTildeBisIvI,
    &PrimFnDydUpCaretTildeBisFvF,
    NULL,   // &PrimFnDydUpCaretTildeBisCvC, -- Can't happen w/UpCaretTilde

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydUpCaretTildeIisIvI, -- Result Boolean, can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeIisFvF, -- Result Boolean, can't happen w/UpCaretTilde

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydUpCaretTildeFisIvI, -- Result Boolean, can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeFisFvF, -- Result Boolean, can't happen w/UpCaretTilde

    &PrimFnDydUpCaretTildeBisRvR,
    NULL,   // &PrimFnDydUpCaretTildeRisRvR, -- Result Boolean, can't happen w/UpCaretTilde

    &PrimFnDydUpCaretTildeBisVvV,
////                 VisRvR     // Handled via type promotion (to VisVvV)
    NULL,   // &PrimFnDydUpCaretTildeVisVvV, -- Result Boolean, can't happen w/UpCaretTilde

    NULL,   // &PrimFnMonUpCaretTildeB64isB64, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeB32isB32, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeB16isB16, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeB08isB08, -- Can't happen w/UpCaretTilde

    &PrimFnDydUpCaretTildeB64isB64vB64,
    &PrimFnDydUpCaretTildeB32isB32vB32,
    &PrimFnDydUpCaretTildeB16isB16vB16,
    &PrimFnDydUpCaretTildeB08isB08vB08,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecUpCaretTilde};
#endif


//***************************************************************************
//  $PrimFnUpCaretTilde_EM_YY
//
//  Primitive function for monadic and dyadic UpCaretTilde (ERROR and "nand")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnUpCaretTilde_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnUpCaretTilde_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (IsAPLCharUpCaretTilde (lptkFunc->tkData.tkChar));

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnUpCaretTilde_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecUpCaretTildeStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecUpCaretTildeStorageTypeDyd
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
} // End PrimSpecUpCaretTildeStorageTypeDyd


//***************************************************************************
//  $PrimFnDydUpCaretTildeBisBvB
//
//  Primitive scalar function dyadic UpCaretTilde:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydUpCaretTildeBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return !(aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretTildeBisBvB


//***************************************************************************
//  $PrimFnDydUpCaretTildeB64isB64vB64
//
//  Primitive scalar function dyadic UpCaretTilde:  B64 {is} B64 fn B64
//***************************************************************************

APLB64 PrimFnDydUpCaretTildeB64isB64vB64
    (APLB64     aplBooleanLft,
     APLB64     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretTildeB64isB64vB64


//***************************************************************************
//  $PrimFnDydUpCaretTildeB32isB32vB32
//
//  Primitive scalar function dyadic UpCaretTilde:  B32 {is} B32 fn B32
//***************************************************************************

APLB32 PrimFnDydUpCaretTildeB32isB32vB32
    (APLB32     aplBooleanLft,
     APLB32     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretTildeB32isB32vB32


//***************************************************************************
//  $PrimFnDydUpCaretTildeB16isB16vB16
//
//  Primitive scalar function dyadic UpCaretTilde:  B16 {is} B16 fn B16
//***************************************************************************

APLB16 PrimFnDydUpCaretTildeB16isB16vB16
    (APLB16     aplBooleanLft,
     APLB16     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretTildeB16isB16vB16


//***************************************************************************
//  $PrimFnDydUpCaretTildeB08isB08vB08
//
//  Primitive scalar function dyadic UpCaretTilde:  B08 {is} B08 fn B08
//***************************************************************************

APLB08 PrimFnDydUpCaretTildeB08isB08vB08
    (APLB08     aplBooleanLft,
     APLB08     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretTildeB08isB08vB08


//***************************************************************************
//  $PrimFnDydUpCaretTildeIisIvI
//
//  Primitive scalar function dyadic UpCaretTilde:  B {is} I fn I
//***************************************************************************

APLBOOL PrimFnDydUpCaretTildeBisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    if (!IsBooleanValue (aplIntegerLft)
     || !IsBooleanValue (aplIntegerRht))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
    return !(((APLBOOL) aplIntegerLft) & (APLBOOL) aplIntegerRht);
} // End PrimFnDydUpCaretTildeBisIvI


//***************************************************************************
//  $PrimFnDydUpCaretTildeBisFvF
//
//  Primitive scalar function dyadic UpCaretTilde:  B {is} F fn F
//***************************************************************************

APLBOOL PrimFnDydUpCaretTildeBisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLINT aplLft,
           aplRht;
    UBOOL  bRet = TRUE;

    // Attempt to convert the float to an integer using System []CT
    aplLft = FloatToAplint_SCT (aplFloatLft, &bRet);
    if (bRet)
        // Attempt to convert the float to an integer using System []CT
        aplRht = FloatToAplint_SCT (aplFloatRht, &bRet);
    if (!bRet
     || !IsBooleanValue (aplLft)
     || !IsBooleanValue (aplRht))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    return !(((APLBOOL) aplLft) & (APLBOOL) aplRht);
} // End PrimFnDydUpCaretTildeBisFvF


//***************************************************************************
//  $PrimFnDydUpCaretTildeBisRvR
//
//  Primitive scalar function dyadic UpCaretTilde:  B {is} R fn R
//***************************************************************************

APLBOOL PrimFnDydUpCaretTildeBisRvR
    (APLRAT     aplRatLft,
     APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLBOOL aplBooleanLft,
            aplBooleanRht;

    if ((!IsMpq0 (&aplRatLft)
      && !IsMpq1 (&aplRatLft))
     || (!IsMpq0 (&aplRatRht)
      && !IsMpq1 (&aplRatRht)))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
    aplBooleanLft = IsMpq1 (&aplRatLft);
    aplBooleanRht = IsMpq1 (&aplRatRht);

    return !(aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretTildeBisRvR


//***************************************************************************
//  $PrimFnDydUpCaretTildeBisVvV
//
//  Primitive scalar function dyadic UpCaretTilde:  B {is} V fn V
//***************************************************************************

APLBOOL PrimFnDydUpCaretTildeBisVvV
    (APLVFP     aplVfpLft,
     APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLBOOL aplBooleanLft,
            aplBooleanRht;

    if ((!IsMpf0 (&aplVfpLft)
      && !IsMpf1 (&aplVfpLft))
     || (!IsMpf0 (&aplVfpRht)
      && !IsMpf1 (&aplVfpRht)))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
    aplBooleanLft = IsMpf1 (&aplVfpLft);
    aplBooleanRht = IsMpf1 (&aplVfpRht);

    return !(aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretTildeBisVvV


//***************************************************************************
//  End of File: pf_nucaret.c
//***************************************************************************
