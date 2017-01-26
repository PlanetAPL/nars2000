//***************************************************************************
//  NARS2000 -- System Function -- Quad TC
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


//***************************************************************************
//  $SysFnTC_EM_YY
//
//  System function:  []TC -- Terminal Control
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnTC_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnTC_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLUINT      ByteRes;           // # bytes in the result
    HGLOBAL      hGlbRes;           // Result global memory handle
    LPVOID       lpMemRes;          // Ptr to result global memory
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_CHAR, 3, 1);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock000 (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = 3;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Fill in the dimension
    *VarArrayBaseToDim (lpMemRes) = 3;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemRes);

#define lpMemData   ((LPAPLCHAR) lpMemRes)
    lpMemData[0] = WC_BS;   // Backspace
    lpMemData[1] = WC_CR;   // Newline (a.k.a. CR)
    lpMemData[2] = WC_LF;   // Linefeed
#undef  lpMemData

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End SysFnTC_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnTCCom_EM_YY
//
//  System function:  []TCxxx -- Terminal Control, Common Routine
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnTCCOM_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnTCCom_EM_YY
    (WCHAR   wc,                    // Char to return
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_CHAR;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkChar     = wc;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnTCCom_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnTCBEL_EM_YY
//
//  System function:  []TCBEL -- Terminal Control, Bell
//***************************************************************************

LPPL_YYSTYPE SysFnTCBEL_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    return SysFnTCCom_EM_YY (WC_BEL, lptkFunc, lptkAxis);
} // End SysFnTCBEL_EM_YY


//***************************************************************************
//  $SysFnTCBS_EM_YY
//
//  System function:  []TCBS -- Terminal Control, Backspace
//***************************************************************************

LPPL_YYSTYPE SysFnTCBS_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    return SysFnTCCom_EM_YY (WC_BS, lptkFunc, lptkAxis);
} // End SysFnTCBS_EM_YY


//***************************************************************************
//  $SysFnTCESC_EM_YY
//
//  System function:  []TCESC -- Terminal Control, Escape
//***************************************************************************

LPPL_YYSTYPE SysFnTCESC_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    return SysFnTCCom_EM_YY (WC_ESC, lptkFunc, lptkAxis);
} // End SysFnTCESC_EM_YY


//***************************************************************************
//  $SysFnTCFF_EM_YY
//
//  System function:  []TCFF -- Terminal Control, Form Feed
//***************************************************************************

LPPL_YYSTYPE SysFnTCFF_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    return SysFnTCCom_EM_YY (WC_FF, lptkFunc, lptkAxis);
} // End SysFnTCFF_EM_YY


//***************************************************************************
//  $SysFnTCHT_EM_YY
//
//  System function:  []TCHT -- Terminal Control, Horizontal Tab
//***************************************************************************

LPPL_YYSTYPE SysFnTCHT_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    return SysFnTCCom_EM_YY (WC_HT, lptkFunc, lptkAxis);
} // End SysFnTCHT_EM_YY


//***************************************************************************
//  $SysFnTCLF_EM_YY
//
//  System function:  []TCLF -- Terminal Control, Linefeed
//***************************************************************************

LPPL_YYSTYPE SysFnTCLF_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    return SysFnTCCom_EM_YY (WC_LF, lptkFunc, lptkAxis);
} // End SysFnTCLF_EM_YY


//***************************************************************************
//  $SysFnTCNL_EM_YY
//
//  System function:  []TCNL -- Terminal Control, Newline (a.k.a. CR)
//***************************************************************************

LPPL_YYSTYPE SysFnTCNL_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    return SysFnTCCom_EM_YY (WC_CR, lptkFunc, lptkAxis);
} // End SysFnTCNL_EM_YY


//***************************************************************************
//  $SysFnTCNUL_EM_YY
//
//  System function:  []TCNUL -- Terminal Control, Nul
//***************************************************************************

LPPL_YYSTYPE SysFnTCNUL_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    return SysFnTCCom_EM_YY (WC_EOS, lptkFunc, lptkAxis);
} // End SysFnTCNUL_EM_YY


//***************************************************************************
//  End of File: qf_tc.c
//***************************************************************************
