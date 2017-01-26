//***************************************************************************
//  NARS2000 -- Primitive Function -- DownCaretTilde
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
PRIMSPEC PrimSpecDownCaretTilde = {
    // Monadic functions
    &PrimFnMonValenceError_EM,
    NULL,   // &PrimSpecDownCaretTildeStorageTypeMon, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeAPA_EM, -- Can't happen w/DownCaretTilde

    NULL,   // &PrimFnMonDownCaretTildeBisB, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeBisI, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeBisF, -- Can't happen w/DownCaretTilde

////                        IisB,   // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonDownCaretTildeIisI, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeIisF, -- Can't happen w/DownCaretTilde

////                        FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonDownCaretTildeFisI, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeFisF, -- Can't happen w/DownCaretTilde

    NULL,   // &PrimFnMonDownCaretTildeRisR, -- Can't happen w/DownCaretTilde

////               VisR,    // Handled via type promotion (to VisV)
    NULL,   // &PrimFnMonDownCaretTildeVisV, -- Can't happen w/DownCaretTilde

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecDownCaretTildeStorageTypeDyd,
    NULL,   // &PrimFnDydDownCaretTildeAPA_EM, -- Can't happen w/DownCaretTilde

    &PrimFnDydDownCaretTildeBisBvB,
    &PrimFnDydDownCaretTildeBisIvI,
    &PrimFnDydDownCaretTildeBisFvF,
    NULL,   // &PrimFnDydDownCaretTildeBisCvC, -- Can't happen w/DownCaretTilde

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydDownCaretTildeIisIvI, -- Result Boolean, can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeIisFvF, -- Result Boolean, can't happen w/DownCaretTilde

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydDownCaretTildeFisIvI, -- Result Boolean, can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeFisFvF, -- Result Boolean, can't happen w/DownCaretTilde

    &PrimFnDydDownCaretTildeBisRvR,
////                 VisRvR     // Handled via type promotion (to VisVvV)
    NULL,   // &PrimFnDydDownCaretTildeRisRvR, -- Result Boolean, can't happen w/DownCaretTilde

    &PrimFnDydDownCaretTildeBisVvV,
    NULL,   // &PrimFnDydDownCaretTildeVisVvV, -- Result Boolean, can't happen w/DownCaretTilde

    NULL,   // &PrimFnMonDownCaretTildeB64isB64, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeB32isB32, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeB16isB16, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeB08isB08, -- Can't happen w/DownCaretTilde

    &PrimFnDydDownCaretTildeB64isB64vB64,
    &PrimFnDydDownCaretTildeB32isB32vB32,
    &PrimFnDydDownCaretTildeB16isB16vB16,
    &PrimFnDydDownCaretTildeB08isB08vB08,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecDownCaretTilde};
#endif


//***************************************************************************
//  $PrimFnDownCaretTilde_EM_YY
//
//  Primitive function for monadic and dyadic DownCaretTilde (ERROR and "nor")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDownCaretTilde_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDownCaretTilde_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (IsAPLCharDownCaretTilde (lptkFunc->tkData.tkChar));

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnDownCaretTilde_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecDownCaretTildeStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecDownCaretTildeStorageTypeDyd
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
} // End PrimSpecDownCaretTildeStorageTypeDyd


//***************************************************************************
//  $PrimFnDydDownCaretTildeBisBvB
//
//  Primitive scalar function dyadic DownCaretTilde:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydDownCaretTildeBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return !(aplBooleanLft | aplBooleanRht);
} // End PrimFnDydDownCaretTildeBisBvB


//***************************************************************************
//  $PrimFnDydDownCaretTildeB64isB64vB64
//
//  Primitive scalar function dyadic DownCaretTilde:  B64 {is} B64 fn B64
//***************************************************************************

APLB64 PrimFnDydDownCaretTildeB64isB64vB64
    (APLB64     aplBooleanLft,
     APLB64     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft | aplBooleanRht);
} // End PrimFnDydDownCaretTildeB64isB64vB64


//***************************************************************************
//  $PrimFnDydDownCaretTildeB32isB32vB32
//
//  Primitive scalar function dyadic DownCaretTilde:  B32 {is} B32 fn B32
//***************************************************************************

APLB32 PrimFnDydDownCaretTildeB32isB32vB32
    (APLB32     aplBooleanLft,
     APLB32     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft | aplBooleanRht);
} // End PrimFnDydDownCaretTildeB32isB32vB32


//***************************************************************************
//  $PrimFnDydDownCaretTildeB16isB16vB16
//
//  Primitive scalar function dyadic DownCaretTilde:  B16 {is} B16 fn B16
//***************************************************************************

APLB16 PrimFnDydDownCaretTildeB16isB16vB16
    (APLB16     aplBooleanLft,
     APLB16     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft | aplBooleanRht);
} // End PrimFnDydDownCaretTildeB16isB16vB16


//***************************************************************************
//  $PrimFnDydDownCaretTildeB08isB08vB08
//
//  Primitive scalar function dyadic DownCaretTilde:  B08 {is} B08 fn B08
//***************************************************************************

APLB08 PrimFnDydDownCaretTildeB08isB08vB08
    (APLB08     aplBooleanLft,
     APLB08     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft | aplBooleanRht);
} // End PrimFnDydDownCaretTildeB08isB08vB08


//***************************************************************************
//  $PrimFnDydDownCaretTildeBisIvI
//
//  Primitive scalar function dyadic DownCaretTilde:  B {is} I fn I
//***************************************************************************

APLBOOL PrimFnDydDownCaretTildeBisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    if (!IsBooleanValue (aplIntegerLft)
     || !IsBooleanValue (aplIntegerRht))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
    return !(((APLBOOL) aplIntegerLft) | (APLBOOL) aplIntegerRht);
} // End PrimFnDydDownCaretTildeBisIvI


//***************************************************************************
//  $PrimFnDydDownCaretTildeBisFvF
//
//  Primitive scalar function dyadic DownCaretTilde:  B {is} F fn F
//***************************************************************************

APLBOOL PrimFnDydDownCaretTildeBisFvF
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

    return !(((APLBOOL) aplLft) | (APLBOOL) aplRht);
} // End PrimFnDydDownCaretTildeBisFvF


//***************************************************************************
//  $PrimFnDydDownCaretTildeBisRvR
//
//  Primitive scalar function dyadic DownCaretTilde:  B {is} R fn R
//***************************************************************************

APLBOOL PrimFnDydDownCaretTildeBisRvR
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

    return !(aplBooleanLft | aplBooleanRht);
} // End PrimFnDydDownCaretTildeBisRvR


//***************************************************************************
//  $PrimFnDydDownCaretTildeBisVvV
//
//  Primitive scalar function dyadic DownCaretTilde:  B {is} V fn V
//***************************************************************************

APLBOOL PrimFnDydDownCaretTildeBisVvV
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

    return !(aplBooleanLft | aplBooleanRht);
} // End PrimFnDydDownCaretTildeBisVvV


//***************************************************************************
//  End of File: pf_ndcaret.c
//***************************************************************************
