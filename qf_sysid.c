//***************************************************************************
//  NARS2000 -- System Function -- Quad SYSID
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
//  $SysFnSYSID_EM_YY
//
//  System function:  []SYSID -- System Identifier
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnSYSID_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnSYSID_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLUINT           ByteRes;              // # bytes in the result
    HGLOBAL           hGlbRes;              // Result global memory handle
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to result header
    LPVOID            lpMemRes;             // Ptr to result global memory
    LPPL_YYSTYPE      lpYYRes;              // Ptr to the result

    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

#define SYSID           WS_APPNAME
#define SYSID_NELM      strcountof (SYSID)

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_CHAR, SYSID_NELM, 1);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader    lpMemHdrRes
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = SYSID_NELM;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Fill in the dimension
    *VarArrayBaseToDim (lpMemHdrRes) = SYSID_NELM;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Copy the SYSID to the result
    CopyMemoryW (lpMemRes, SYSID, SYSID_NELM);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAllocGlb (hGlbRes, lptkFunc->tkCharIndex);

    return lpYYRes;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End SysFnSYSID_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_sysid.c
//***************************************************************************
