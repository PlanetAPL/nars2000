//***************************************************************************
//  NARS2000 -- System Function -- Quad AV
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2010 Sudley Place Software

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
//  $SysFnAV_EM_YY
//
//  System function:  []AV -- Atomic Vector
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnAV_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnAV_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbQuadAV);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnAV_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $MakeQuadAV
//
//  Create the global memory value for []AV
//***************************************************************************

void MakeQuadAV
    (void)

{
    LPVARARRAY_HEADER lpHeader;     // Ptr to array header
    LPAPLCHAR         lpMemRes;     // Ptr to result global memory
    APLUINT           ByteRes;      // # bytes in the result
    UINT              uRes;         // Loop counter

#define QUADAV_LEN      APLCHAR_SIZE

    // Calculate # bytes in the result
    ByteRes = CalcArraySize (ARRAY_CHAR, QUADAV_LEN, 1);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
    {
        DbgStop ();         // We should never get here
    } // End IF

    // Create []AV
    hGlbQuadAV = MyGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbQuadAV)
    {
        DbgStop ();         // We should never get here
    } // End IF

    // Lock the memory to get a ptr to it
    lpHeader = MyGlobalLock (hGlbQuadAV);

    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
    lpHeader->PermNdx    = PERMNDX_QUADAV;  // So we don't free it
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
////lpHeader->RefCnt     = 0;               // Ignore as this is perm
    lpHeader->NELM       = QUADAV_LEN;
    lpHeader->Rank       = 1;

    // Fill in the dimension
    *VarArrayBaseToDim (lpHeader) = QUADAV_LEN;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpHeader, 1);

    // Fill in the result with 0-0xFFFF
    for (uRes = 0; uRes < QUADAV_LEN;  uRes++)
        lpMemRes[uRes] = uRes;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbQuadAV); lpMemRes = NULL;
} // End MakeQuadAV


//***************************************************************************
//  End of File: qf_av.c
//***************************************************************************
