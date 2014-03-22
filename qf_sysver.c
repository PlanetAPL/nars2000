//***************************************************************************
//  NARS2000 -- System Function -- Quad SYSVER
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

LPWCHAR aDaysOfWeek[] = {L"Sun", L"Mon", L"Tue", L"Wed", L"Thu", L"Fri", L"Sat"};
LPWCHAR aMonths    [] = {L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun", L"Jul", L"Aug", L"Sep", L"Oct", L"Nov", L"Dec"};


//***************************************************************************
//  $SysFnSYSVER_EM_YY
//
//  System function:  []SYSVER -- System Version
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnSYSVER_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnSYSVER_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLUINT      ByteRes;           // # bytes in the result
    APLNELM      aplNELMRes;        // Result NELM
    HGLOBAL      hGlbRes;           // Result global memory handle
    LPAPLCHAR    lpMemRes,          // Ptr to result global memory
                 lpMemData;         // Ptr to result data
    LPAPLCHAR    lpw;               // Temporary ptr
    HANDLE       hFile;             // File handle from CreateFileW
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Define maximum length of []SYSVER
#ifdef DEBUG
  #define DEBUGSTR      L" (DEBUG)"
#else
  #define DEBUGSTR
#endif

#define SYSVER  L"000.000.0000.00799  Tue Jan 16 17:43:45 2007  Win/32" DEBUGSTR
#define SYSVER_NELM    strcountof (SYSVER)

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_CHAR, SYSVER_NELM, 1);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
////lpHeader->NELM       = SYSVER_NELM;     // Filled in below
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Fill in the dimension
////*VarArrayBaseToDim (lpMemRes) = SYSVER_NELM;    // Filled in below

    // Skip over the header and dimensions to the data
    lpw = lpMemData = VarArrayBaseToData (lpMemRes, 1);

    // Copy the application's File Version String
    lstrcpyW (lpw, wszFileVer);

    // Skip to the trailing zero
    lpw += lstrlenW (lpw);
    *lpw++ = L' ';      // Blank separators
    *lpw++ = L' ';

    // Open the executable file so we get its last write time
    hFile =
      CreateFileW (wszAppDPFE,              // lpwFileName
                   GENERIC_READ,            // dwDesiredAccess
                   FILE_SHARE_READ,         // dwShareMode
                   NULL,                    // lpSecurityAttributes
                   OPEN_EXISTING,           // dwCreationDistribution
                   FILE_ATTRIBUTE_NORMAL,   // dwFlagsAndAttributes
                   NULL);                   // hTemplateFile
    if (hFile NE INVALID_HANDLE_VALUE)
    {
        FILETIME   ftLastWrite;
        SYSTEMTIME systemTime;

        // Get the file's last write time
        // Note that the file's creation time need not be the
        //   same as the file's last write time.  I think the
        //   linker might sometimes rewrite the .exe file
        //   instead of recreating it.
        GetFileTime (hFile, NULL, NULL, &ftLastWrite);

        // We no longer need this handle
        CloseHandle (hFile); hFile = NULL;

        // Convert the file's last write time to system time
        FileTimeToSystemTime (&ftLastWrite, &systemTime);

        // Format the system time as
        //    "Wed Jan 02 02:03:55 1980"
        wsprintfW (lpw,
                   L"%s %s %02u %02u:%02u:%02u %u",
                   aDaysOfWeek[systemTime.wDayOfWeek],
                   aMonths[systemTime.wMonth - 1],
                   systemTime.wDay,
                   systemTime.wHour,
                   systemTime.wMinute,
                   systemTime.wSecond,
                   systemTime.wYear);
        // Skip to the trailing zero
        lpw += lstrlenW (lpw);
        *lpw++ = L' ';    // Blank separators
        *lpw++ = L' ';
    } // End IF

#ifdef _WIN64
  #define SYSTYPE     L"Win/64" DEBUGSTR
#elif defined (_WIN32)
  #define SYSTYPE     L"Win/32" DEBUGSTR
#else
  #error Need code for this architecture.
#endif

    // Copy to the result
    CopyMemoryW (lpw, SYSTYPE, strcountof (SYSTYPE));

    // Calculate the actual NELM
    aplNELMRes = lstrlenW (lpMemData);

    // Fill in actual length of []SYSVER
#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    lpHeader->NELM = aplNELMRes;
#undef  lpHeader

    // Fill in the dimension
    *VarArrayBaseToDim (lpMemRes) = aplNELMRes;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Make sure we didn't overwrite
    if (aplNELMRes < SYSVER_NELM)
    {
        // Calculate space needed for the result
        ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRes, 1);

////////// Check for overflow
////////if (ByteRes NE (APLU3264) ByteRes)
////////    goto WSFULL_EXIT;

        // Re-allocate the global downwards
        hGlbRes = MyGlobalReAlloc (hGlbRes, (APLU3264) ByteRes, GMEM_MOVEABLE);
    } else
    if (aplNELMRes > SYSVER_NELM)
        // We should never get here
        DbgStop ();

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; y    // Already zero from YYAlloc
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
} // End SysFnSYSVER_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_sysver.c
//***************************************************************************
