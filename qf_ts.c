//***************************************************************************
//  NARS2000 -- System Function -- Quad TS
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
//  $SysFnTS_EM_YY
//
//  System function:  []TS -- Time Stamp
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnTS_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnTS_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result
    HGLOBAL      hGlbRes = NULL;    // Result global memory handle

    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Allocate space for the timestamp
    hGlbRes = TimestampAllocate (NULL);

    // Check for error
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

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
} // End SysFnTS_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $TimestampAllocate
//
//  Allocate space for a timestamp and fil it in if requested
//***************************************************************************

HGLOBAL TimestampAllocate
    (LPSYSTEMTIME lpSystemTime)         // Ptr to incoming systemTime (may be null)

{
    SYSTEMTIME   SystemTime;        // Struct for the system time
    APLUINT      ByteRes;           // # bytes in the result
    HGLOBAL      hGlbRes = NULL;    // Result global memory handle
    LPAPLINT     lpMemRes;          // Ptr to result global memory

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_INT, 7, 1);

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
    lpHeader->ArrType    = ARRAY_INT;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = 7;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Fill in the dimension
    *VarArrayBaseToDim (lpMemRes) = 7;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemRes);

    // If we're to use the current time, ...
    if (lpSystemTime EQ NULL)
    {
        // Get the current time
        if (OptionFlags.bUseLocalTime)
            GetLocalTime  (&SystemTime);
        else
            GetSystemTime (&SystemTime);
        // Point to it
        lpSystemTime = &SystemTime;
    } // End IF

    // Save the timestamp in the result
    lpMemRes[0] = lpSystemTime->wYear;
    lpMemRes[1] = lpSystemTime->wMonth;
    lpMemRes[2] = lpSystemTime->wDay;
    lpMemRes[3] = lpSystemTime->wHour;
    lpMemRes[4] = lpSystemTime->wMinute;
    lpMemRes[5] = lpSystemTime->wSecond;
    lpMemRes[6] = lpSystemTime->wMilliseconds;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
WSFULL_EXIT:
    return hGlbRes;
} // End TimestampAllocate


//***************************************************************************
//  End of File: qf_ts.c
//***************************************************************************
