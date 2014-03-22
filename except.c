//***************************************************************************
//  NARS2000 -- Exception Handling
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2014 Sudley Place Software

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
#pragma pack(push,4)
#include <dbghelp.h>
#pragma pack(pop)
#include "headers.h"
#include "startaddr.h"


typedef struct tagEXCEPT_NAMES
{
    char *ExceptName;
    UINT  ExceptCode;
} EXCEPT_NAMES, *LPEXCEPT_NAMES;

EXCEPT_NAMES ExceptNames[] =
{
 {"FLT_DIVIDE_BY_ZERO", EXCEPTION_FLT_DIVIDE_BY_ZERO},
 {"INT_DIVIDE_BY_ZERO", EXCEPTION_INT_DIVIDE_BY_ZERO},
 {"ACCESS_VIOLATION"  , EXCEPTION_ACCESS_VIOLATION  },
 {"SINGLE_STEP"       , EXCEPTION_SINGLE_STEP       },
 {"BREAKPOINT"        , EXCEPTION_BREAKPOINT        },
 {"LIMIT ERROR"       , EXCEPTION_LIMIT_ERROR       },
 {"STACK OVERFLOW"    , EXCEPTION_STACK_OVERFLOW    },
};

#define EXCEPT_NAMES_LENGTH         countof (ExceptNames)

// Save area for exception address if EXCEPTION_BREAKPOINT
APLU3264        gExceptAddr;            // Exception address
LPWCHAR         glpExceptionText;       // Ptr to Exception text
LPUCHAR         glpInvalidAddr;         // Ptr to invalid address
EXCEPTION_CODES gExceptionCode;

// Save area for crash information
CONTEXT gContextRecord;
#ifndef PROTO
  #ifdef _WIN64
    #define ADDR_MASK     0xFFFFFFFFFFFFF000
  EXCEPTION_RECORD64 gExceptionRecord;
  #elif defined (_WIN32)
    #define ADDR_MASK     0x00000000FFFFF000
  EXCEPTION_RECORD32 gExceptionRecord;
  #else
    #error Need code for this architecture.
  #endif
#endif

#define STACKWALK_MAX_NAMELEN       1024

typedef struct tagCallstackEntry
{
    DWORD64 offset;                                     // If 0, we have no valid entry
    CHAR    name[STACKWALK_MAX_NAMELEN];
    CHAR    undName[STACKWALK_MAX_NAMELEN];
    CHAR    undFullName[STACKWALK_MAX_NAMELEN];
    DWORD64 offsetFromSmybol;
    DWORD   offsetFromLine;
    DWORD   lineNumber;
    CHAR    lineFileName[STACKWALK_MAX_NAMELEN];
    DWORD   symType;
    LPCSTR  symTypeString;
    CHAR    moduleName[STACKWALK_MAX_NAMELEN];
    DWORD64 baseOfImage;
    CHAR    loadedImageName[STACKWALK_MAX_NAMELEN];
} CallstackEntry;

typedef enum CallstackEntryType
{
    firstEntry,
    nextEntry,
    lastEntry
};

typedef struct tagIMAGEHLP_MODULE64_V2
{
    DWORD    SizeOfStruct;          // Set to sizeof(IMAGEHLP_MODULE64)
    DWORD64  BaseOfImage;           // Base load address of module
    DWORD    ImageSize;             // Virtual size of the loaded module
    DWORD    TimeDateStamp;         // Date/time stamp from pe header
    DWORD    CheckSum;              // Checksum from the pe header
    DWORD    NumSyms;               // Number of symbols in the symbol table
    SYM_TYPE SymType;               // Type of symbols loaded
    CHAR     ModuleName[32];        // Module name
    CHAR     ImageName[256];        // Image name
    CHAR     LoadedImageName[256];  // Symbol file name
} IMAGEHLP_MODULE64_V2;


//***************************************************************************
//  $MyGetExceptionCode
//
//  Return the current ExceptionCode
//***************************************************************************

EXCEPTION_CODES MyGetExceptionCode
    (void)

{
    // Return the ExceptionCode
    return gExceptionCode;
} // End MyGetExceptionCode


//***************************************************************************
//  $MyGetExceptionStr
//
//  Return the current ExceptionCode as a string
//***************************************************************************

LPWSTR MyGetExceptionStr
    (void)

{
    static WCHAR wszTemp[256];

    // Split cases based upon the exception code
    switch (gExceptionCode)
    {
        case EXCEPTION_ACCESS_VIOLATION:
            return L"EXCEPTION_ACCESS_VIOLATION";

        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
            return L"EXCEPTION_ARRAY_BOUNDS_EXCEEDED";

        case EXCEPTION_BREAKPOINT:
            return L"EXCEPTION_BREAKPOINT";

        case EXCEPTION_CTRL_BREAK:
            return L"EXCEPTION_CTRL_BREAK";

        case EXCEPTION_DATATYPE_MISALIGNMENT:
            return L"EXCEPTION_DATATYPE_MISALIGNMENT";

        case EXCEPTION_DOMAIN_ERROR:
            return L"EXCEPTION_DOMAIN_ERROR";

        case EXCEPTION_FLT_DENORMAL_OPERAND:
            return L"EXCEPTION_FLT_DENORMAL_OPERAND";

        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            return L"EXCEPTION_FLT_DIVIDE_BY_ZERO";

        case EXCEPTION_FLT_INEXACT_RESULT:
            return L"EXCEPTION_FLT_INEXACT_RESULT";

        case EXCEPTION_FLT_INVALID_OPERATION:
            return L"EXCEPTION_FLT_INVALID_OPERATION";

        case EXCEPTION_FLT_OVERFLOW:
            return L"EXCEPTION_FLT_OVERFLOW";

        case EXCEPTION_FLT_STACK_CHECK:
            return L"EXCEPTION_FLT_STACK_CHECK";

        case EXCEPTION_FLT_UNDERFLOW:
            return L"EXCEPTION_FLT_UNDERFLOW";

        case EXCEPTION_GUARD_PAGE:
            return L"EXCEPTION_GUARD_PAGE";

        case EXCEPTION_ILLEGAL_INSTRUCTION:
            return L"EXCEPTION_ILLEGAL_INSTRUCTION";

        case EXCEPTION_IN_PAGE_ERROR:
            return L"EXCEPTION_IN_PAGE_ERROR";

        case EXCEPTION_INT_DIVIDE_BY_ZERO:
            return L"EXCEPTION_INT_DIVIDE_BY_ZERO";

        case EXCEPTION_INT_OVERFLOW:
            return L"EXCEPTION_INT_OVERFLOW";

        case EXCEPTION_INVALID_DISPOSITION:
            return L"EXCEPTION_INVALID_DISPOSITION";

        case EXCEPTION_INVALID_HANDLE:
            return L"EXCEPTION_INVALID_HANDLE";

        case EXCEPTION_LIMIT_ERROR:
            return L"EXCEPTION_LIMIT_ERROR";

        case EXCEPTION_NONCE_ERROR:
            return L"EXCEPTION_NONCE_ERROR";

        case EXCEPTION_PRIV_INSTRUCTION:
            return L"EXCEPTION_PRIV_INSTRUCTION";

        case EXCEPTION_RESULT_FLOAT:
            return L"EXCEPTION_RESULT_FLOAT";

        case EXCEPTION_SINGLE_STEP:
            return L"EXCEPTION_SINGLE_STEP";

        case EXCEPTION_STACK_OVERFLOW:
            return L"EXCEPTION_STACK_OVERFLOW";

        case EXCEPTION_SUCCESS:
            return L"EXCEPTION_SUCCESS";

        case STATUS_UNWIND_CONSOLIDATE:
            return L"STATUS_UNWIND_CONSOLIDATE";

        defstop
            wsprintfW (wszTemp,
                       L"*** Unknown Exception Code:  %u",
                       gExceptionCode);
            return wszTemp;
    } // End SWITCH
} // End MyGetExceptionStr


//***************************************************************************
//  $MySetExceptionCode
//
//  Set the current ExceptionCode
//***************************************************************************

void MySetExceptionCode
    (EXCEPTION_CODES ExceptionCode) // Exception code

{
    // Set the ExceptionCode
    gExceptionCode = ExceptionCode;
} // End MySetExceptionCode


//***************************************************************************
//  $CheckVirtAlloc
//
//  Check on virtual allocs
//***************************************************************************

long CheckVirtAlloc
    (LPEXCEPTION_POINTERS lpExcept,         // Ptr to exception & context records
     LPWCHAR              lpText)   // Ptr to text of exception handler

{
    LPUCHAR      lpInvalidAddr;     // Ptr to invalid address
    int          iRet;              // Return code

    // Handle access violations only
    if (lpExcept->ExceptionRecord->ExceptionCode EQ EXCEPTION_ACCESS_VIOLATION)
    {
        // Get the invalid address
#ifdef _WIN64
        lpInvalidAddr = (LPUCHAR) ((PEXCEPTION_RECORD64) lpExcept->ExceptionRecord)->ExceptionInformation[1];
#elif defined (_WIN32)
        lpInvalidAddr = (LPUCHAR) ((PEXCEPTION_RECORD32) lpExcept->ExceptionRecord)->ExceptionInformation[1];
#else
  #error Need code for this architecture.
#endif
        // Check on virtual allocs from <memVirtStr>
        iRet = CheckMemVirtStr (lpInvalidAddr);
        if (iRet)
            return iRet;

        // Check on virtual allocs in the <lpMemPTD->lpLstMVS> chain
        iRet = CheckPTDVirtStr (lpInvalidAddr);
        if (iRet)
            return iRet;
    } // End IF

    return EXCEPTION_CONTINUE_SEARCH;
} // End CheckVirtAlloc


//***************************************************************************
//  $CheckPTDVirtStr
//
//  Check on virtual allocs in the <lpMemPTD->lpLstMVS> chain
//***************************************************************************

int CheckPTDVirtStr
    (LPUCHAR lpInvalidAddr)         // Ptr to invalid address

{
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPMEMVIRTSTR lpLstMVS;          // Ptr to last MEMVIRTSTR (NULL = none)
    LPUCHAR      lpIniAddr;         // Ptr to invalid address

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); // Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // If lpMemPTD isn't set, just exit
    if (lpMemPTD EQ NULL)
        return 0;

    // Get the ptr to the last MVS
    lpLstMVS = lpMemPTD->lpLstMVS;

    // Check for global VirtualAlloc memory that needs to be expanded
    while (lpLstMVS)
    {
        // Get the initial address
        lpIniAddr = lpLstMVS->IniAddr;

        // If it's within range for this VirtualAlloc address, ...
        if (lpIniAddr <= lpInvalidAddr
         &&              lpInvalidAddr < (lpIniAddr + lpLstMVS->MaxSize))
        {
            // Allocate more memory
            if (VirtualAlloc ((LPVOID) (ADDR_MASK & (HANDLE_PTR) lpInvalidAddr),
                              lpLstMVS->IncrSize,
                              MEM_COMMIT,
                              PAGE_READWRITE) NE NULL)
                return EXCEPTION_CONTINUE_EXECUTION;
            else
            // Can't allocate more memory??
            {
                MessageBoxW (hWndMF,
                             L"Not enough memory for <VirtualAlloc> in <CheckPTDVirtStr>",
                             lpwszAppName,
                             MB_OK | MB_ICONERROR);
                MySetExceptionCode (EXCEPTION_LIMIT_ERROR);

                return EXCEPTION_EXECUTE_HANDLER;
            } // End IF/ELSE
        } else
        {
            // Skip to the guard page address
            lpIniAddr += lpLstMVS->MaxSize;

            // Check for the guard page
            if (lpIniAddr <= lpInvalidAddr
             &&              lpInvalidAddr <  (lpIniAddr + PAGESIZE))
            {
                dprintfWL0 (L"Exceeded LIMIT of %08X @ %S", lpLstMVS->MaxSize, lpLstMVS->lpText);

                MySetExceptionCode (EXCEPTION_LIMIT_ERROR);

                return EXCEPTION_EXECUTE_HANDLER;
            } // End IF
        } // End IF/ELSE

        // Get the previous ptr in the chain
        lpLstMVS = lpLstMVS->lpPrvMVS;
    } // End FOR

    // Mark as no match
    return 0;
} // End CheckPTDVirtStr


//***************************************************************************
//  $CheckMemVirtStr
//
//  Check on virtual allocs from <memVirtStr>
//***************************************************************************

int CheckMemVirtStr
    (LPUCHAR lpInvalidAddr)         // Ptr to invalid address

{
    UINT    uMem;                   // Loop counter
    LPUCHAR lpIniAddr;              // Ptr to initial address

    // Check for global VirtualAlloc memory that needs to be expanded
    for (uMem = 0; uMem < uMemVirtCnt; uMem++)
    {
        // Get the initial address
        lpIniAddr = memVirtStr[uMem].IniAddr;

        // If it's within range for this VirtualAlloc address, ...
        if (lpIniAddr <= lpInvalidAddr
         &&              lpInvalidAddr < (lpIniAddr + memVirtStr[uMem].MaxSize))
        {
            // Allocate more memory
            if (VirtualAlloc (lpInvalidAddr,
                              memVirtStr[uMem].IncrSize,
                              MEM_COMMIT,
                              PAGE_READWRITE) NE NULL)
                return EXCEPTION_CONTINUE_EXECUTION;
            else
            // Can't allocate more memory??
            {
                MessageBoxW (hWndMF,
                             L"Not enough memory for <VirtualAlloc> in <CheckMemVirtStr>",
                             lpwszAppName,
                             MB_OK | MB_ICONERROR);
                MySetExceptionCode (EXCEPTION_LIMIT_ERROR);

                return EXCEPTION_EXECUTE_HANDLER;
            } // End IF/ELSE
        } else
        {
            // Skip to the guard page address
            lpIniAddr += memVirtStr[uMem].MaxSize;

            // Check for the guard page
            if (lpIniAddr <= lpInvalidAddr
             &&              lpInvalidAddr <  (lpIniAddr + PAGESIZE))
            {
                MySetExceptionCode (EXCEPTION_LIMIT_ERROR);

                return EXCEPTION_EXECUTE_HANDLER;
            } // End IF
        } // End IF/ELSE
    } // End FOR

    // Mark as no match
    return 0;
} // End CheckMemVirtStr


//***************************************************************************
//  $CheckException
//
//  Check on a structured exception
//***************************************************************************

long CheckException
    (LPEXCEPTION_POINTERS lpExcept,         // Ptr to exception information
     LPWCHAR              lpText)           // Ptr to text of exception handler

{
    int iRet;                   // Return code

    // Save in globals
    gContextRecord   = *lpExcept->ContextRecord;
#ifdef _WIN64
    gExceptionRecord = *(PEXCEPTION_RECORD64) lpExcept->ExceptionRecord;
#elif defined (_WIN32)
    gExceptionRecord = *(PEXCEPTION_RECORD32) lpExcept->ExceptionRecord;
#else
  #error Need code for this architecture.
#endif
    // Get the invalid address
    glpInvalidAddr = (LPUCHAR) gExceptionRecord.ExceptionInformation[1];    // Save as global

    // Save the exception code, address, and text for later use
    MySetExceptionCode (lpExcept->ExceptionRecord->ExceptionCode);      // ***DELETEME***
    glpExceptionText = lpText;

    // Split cases based upon the exception code
    switch (lpExcept->ExceptionRecord->ExceptionCode)
    {
        case EXCEPTION_ACCESS_VIOLATION:
            // Check on virtual allocs from <memVirtStr>
            iRet = CheckMemVirtStr (glpInvalidAddr);
            if (iRet)
                return iRet;

            // Check on virtual allocs in the <lpMemPTD->lpLstMVS> chain
            iRet = CheckPTDVirtStr (glpInvalidAddr);
            if (iRet)
                return iRet;

            // Fall through to common handler execution

////////case EXCEPTION_RESULT_BOOL:
////////case EXCEPTION_RESULT_INT:
        case EXCEPTION_RESULT_FLOAT:
        case EXCEPTION_RESULT_VFP:
        case EXCEPTION_RESULT_RAT:
        case EXCEPTION_DOMAIN_ERROR:
        case EXCEPTION_NONCE_ERROR:
        case EXCEPTION_LIMIT_ERROR:
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
        case EXCEPTION_SINGLE_STEP:
        case EXCEPTION_GUARD_PAGE:
        case EXCEPTION_STACK_OVERFLOW:
            return EXCEPTION_EXECUTE_HANDLER;

        case EXCEPTION_BREAKPOINT:
            // In this case, we need to know who called us,
            //   so we can report it to the end user

            // Save our return address for later use
#ifdef _WIN64
            gExceptAddr = lpExcept->ContextRecord->Rsp;
#elif defined (_WIN32)
            gExceptAddr = lpExcept->ContextRecord->Esp;
#else
  #error Need code for this architecture.
#endif
            return EXCEPTION_EXECUTE_HANDLER;

        case EXCEPTION_CTRL_BREAK:
            return EXCEPTION_CONTINUE_EXECUTION;

        default:
            return EXCEPTION_CONTINUE_SEARCH;
    } // End SWITCH
} // End CheckException


//***************************************************************************
//  $CompareStartAddresses
//
//  Compare starting addresses so as to sort them
//***************************************************************************

UINT __cdecl CompareStartAddresses
    (const void *elem1,
     const void *elem2)

{
#define lpSALft     ((LPSTART_ADDRESSES) elem1)
#define lpSARht     ((LPSTART_ADDRESSES) elem2)

    return (UINT) (lpSALft->StartAddressAddr
                 - lpSARht->StartAddressAddr);
#undef  lpSARht
#undef  lpSALft
} // End CompareStartAddresses


//***************************************************************************
//  $IsGoodReadPtr
//
//  Return TRUE iff the given ptr is valid for reading a given # bytes
//***************************************************************************

UBOOL IsGoodReadPtr
    (LPBYTE lpReadPtr,
     DWORD  dwBytes)

{
    DWORD dwCnt;
    BYTE  dwRead;

    __try
    {
        for (dwCnt = 0; dwCnt < dwBytes; dwCnt++)
            dwRead += *lpReadPtr++;

        return TRUE;
    } __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return FALSE;
    } // End __try/__except
} // End IsGoodReadPtr


//***************************************************************************
//  $DisplayException
//
//  Display an exception code, address, etc.
//***************************************************************************

void DisplayException
    (void)

{
#ifdef DEBUG
    WCHAR        wszTemp[1024]; // Temp output save area
    int          exceptIndex;   // Exception index
    UINT         exceptCode,    // Exception code
                 uMem,          // Loop counter
                 uCnt,          // ...
                 SILevel;       // The current SI level
    APLU3264     nearAddress,   // Offset from closest address
                 nearIndex,     // Index into StartAddresses
                 nearAddress0,  // Offset from closest address
                 nearIndex0,    // Index into StartAddresses
                 nearAddress1,  // Offset from closest address
                 nearIndex1;    // Index into StartAddresses
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    LPWCHAR      exceptText;    // Ptr to exception text
    LPUCHAR      exceptAddr;    // Exception address
    APLU3264     regEBP,        // Stack trace ptr
                 regEIP;        // Instruction ptr
    LPSIS_HEADER lpSISCur;      // Ptr to current SIS header
    LPMEMVIRTSTR lpLstMVS;      // Ptr to last MEMVIRTSTR (NULL = none)

    // Sort the StartAddresses in ascending order by address
    qsort (StartAddresses,
           START_ADDRESSES_LENGTH,
           sizeof (StartAddresses[0]),
          &CompareStartAddresses);

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); // Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // If lpMemPTD isn't valid, just exit
    if (!IsValidPtr (lpMemPTD, sizeof (lpMemPTD)))
        return;

    // Get the saved exception code & address, & text
    exceptCode = gExceptionCode;
    exceptAddr = (LPUCHAR) gExceptionRecord.ExceptionAddress;
    exceptText = glpExceptionText;
#ifdef _WIN64
    regEBP     = gContextRecord.Rbp;
#elif defined (_WIN32)
    regEBP     = gContextRecord.Ebp;
#else
  #error Need code for this architecture.
#endif
    lpSISCur   = lpMemPTD->lpSISCur;
    lpLstMVS   = lpMemPTD->lpLstMVS;

    // If the exception is EXCEPTION_BREAKPOINT (from DbgStop),
    //   we need to display the return address as that's from
    //   where we were called.  Displaying DbgStop address is
    //   of no help
    if (exceptCode EQ EXCEPTION_BREAKPOINT)
        exceptAddr = *(LPUCHAR *) &gExceptAddr;

    // Find the address closest to and at or below the given address
    // If the address is not found, it could be that we're
    //   running under a debugger and the debugger has changed the
    //   starting address of the routine to a near JMP instruction,
    //   so try again with that assumption
    FindRoutineAddress (exceptAddr, &nearAddress0, &nearIndex0, FALSE);
    FindRoutineAddress (exceptAddr, &nearAddress1, &nearIndex1, TRUE);
    if (nearAddress0 < nearAddress1)
    {
        nearAddress = nearAddress0;
        nearIndex   = nearIndex0;
    } else
    {
        nearAddress = nearAddress1;
        nearIndex   = nearIndex1;
    } // End IF/ELSE

    for (exceptIndex = 0; exceptIndex < EXCEPT_NAMES_LENGTH; exceptIndex++)
    if (exceptCode EQ ExceptNames[exceptIndex].ExceptCode)
        break;

    ShowWindow (hWndCC, SW_SHOWNORMAL);
    UpdateWindow (hWndCC);

#define NewMsg(a)   SendMessageW (hWndCC_LB, LB_ADDSTRING, 0, (LPARAM) (a)); UpdateWindow (hWndCC_LB)

    NewMsg (L"COPY THIS TEXT TO AN EMAIL MESSAGE"                    );
    NewMsg (L"----------------------------------------------------"  );
    NewMsg (L"Use Right-click:  Select All, and"                     );
    NewMsg (L"    Right-click:  Copy"                                );
    NewMsg (L"   to copy the entire text to the clipboard."          );
    NewMsg (L"----------------------------------------------------"  );
    NewMsg (L"Post the text on the Forum <http://forum.nars2000.org>");
    NewMsg (L"   in the Bug Reports section along with a detailed"   );
    NewMsg (L"   statement of what you were doing just prior to the" );
    NewMsg (L"   crash."                                             );
    NewMsg (L"Also, if at all possible, it would be great if you"    );
    NewMsg (L"   could send along a copy of the last saved workspace");
    NewMsg (L"   (the one with an extension of .save.bak.ws.nars)."  );
    NewMsg (L"----------------- Copy Below Here ------------------"  );

    // Display the version # of the executable
    wsprintfW (wszTemp,
               WS_APPNAME L" -- Version %s (%s)" WS_APPEND_DEBUG,
               wszFileVer,
#ifdef _WIN64
               L"Win64"
#elif defined (_WIN32)
               L"Win32"
#else
  #error Need code for this architecture.
#endif
               );
    NewMsg (wszTemp);

    // Display the exception code
    wsprintfW (wszTemp,
               L"Exception code = %08X (%S)",
               exceptCode,
               (exceptIndex EQ EXCEPT_NAMES_LENGTH)
                 ? "Exception Unknown"
                 : ExceptNames[exceptIndex].ExceptName);
    NewMsg (L"");
    NewMsg (wszTemp);

    wsprintfW (wszTemp,
               L"   at %p (%S + %p)",
               exceptAddr,
               StartAddresses[nearIndex].StartAddressName,
               nearAddress);
    NewMsg (wszTemp);

    wsprintfW (wszTemp,
               L"   from %s",
               exceptText);
    NewMsg (wszTemp);

    // Display the registers
    NewMsg (L"");
    NewMsg (L"== REGISTERS ==");
    wsprintfW (wszTemp,
#ifdef _WIN64
               L"RAX = %p RBX = %p RCX = %p RDX = %p RIP = %p",
               gContextRecord.Rax,
               gContextRecord.Rbx,
               gContextRecord.Rcx,
               gContextRecord.Rdx,
               gContextRecord.Rip
#elif defined (_WIN32)
               L"EAX = %p EBX = %p ECX = %p EDX = %p EIP = %p",
               gContextRecord.Eax,
               gContextRecord.Ebx,
               gContextRecord.Ecx,
               gContextRecord.Edx,
               gContextRecord.Eip
#else
  #error Need code for this architecture.
#endif
               );
    NewMsg (wszTemp);

    wsprintfW (wszTemp,
#ifdef _WIN64
               L"RSI = %p RDI = %p RBP = %p RSP = %p EFL = %08X",
               gContextRecord.Rsi,
               gContextRecord.Rdi,
               gContextRecord.Rbp,
               gContextRecord.Rsp,
#elif defined (_WIN32)
               L"ESI = %p EDI = %p EBP = %p ESP = %p EFL = %08X",
               gContextRecord.Esi,
               gContextRecord.Edi,
               gContextRecord.Ebp,
               gContextRecord.Esp,
#else
  #error Need code for this architecture.
#endif
               gContextRecord.EFlags);
    NewMsg (wszTemp);

    wsprintfW (wszTemp,
#ifdef _WIN64
               L"CS = %04X DS = %04X ES = %04X FS = %04X GS = %04X SS = %04X CR2 = %p",
#elif defined (_WIN32)
               L"CS = %04X DS = %04X ES = %04X FS = %04X GS = %04X SS = %04X CR2 = %p",
#else
  #error Need code for this architecture.
#endif
               gContextRecord.SegCs,
               gContextRecord.SegDs,
               gContextRecord.SegEs,
               gContextRecord.SegFs,
               gContextRecord.SegGs,
               gContextRecord.SegSs,
               gExceptionRecord.ExceptionInformation[1]);
    NewMsg (wszTemp);

    // Display the instruction stream
    NewMsg (L"");
    NewMsg (L"== INSTRUCTIONS ==");

    // Get the instruction pointer
#ifdef _WIN64
    regEIP = gContextRecord.Rip;
#elif defined (_WIN32)
    regEIP = gContextRecord.Eip;
#else
  #error Need code for this architecture.
#endif

    // Start instruction display three rows before the actual fault instruction
    regEIP -= 3 * 16;

    if (IsGoodReadPtr (*(LPUCHAR *) &regEIP, 48))
    {
        for (uCnt = 0; uCnt < 7; uCnt++, regEIP += 16)
        {
            wsprintfW (wszTemp,
                       L"%p: ",
                       regEIP);
            for (uMem = 0; uMem < 16; uMem++)
                wsprintfW (&wszTemp[lstrlenW (wszTemp)],
                           L" %02X",
                           *(LPBYTE) (regEIP + uMem));
            NewMsg (wszTemp);
        } // End FOR
    } // End IF

    // Display the backtrace
    NewMsg (L"");
    NewMsg (L"== BACKTRACE ==");

    // Do a stack walk
    DoStackWalk (&gContextRecord);

    // Display the virtual memory ranges
    NewMsg (L"");
    NewMsg (L"== MEMVIRTSTR ==");
#ifdef _WIN64
    NewMsg (L"     IniAddr     IncrSize  MaxSize GuardPage");
#elif defined (_WIN32)
    NewMsg (L" IniAddr IncrSize  MaxSize GuardPage");
#else
  #error Need code for this architecture.
#endif

    // Check for global VirtualAlloc memory that needs to be expanded
    for (uMem = 0; uMem < uMemVirtCnt; uMem++)
    {
        wsprintfW (wszTemp,
                   L"%p %08X %08X %p %S",
                   memVirtStr[uMem].IniAddr,
                   memVirtStr[uMem].IncrSize,
                   memVirtStr[uMem].MaxSize,
                   memVirtStr[uMem].IniAddr + memVirtStr[uMem].MaxSize,
                   memVirtStr[uMem].lpText
                  );
        NewMsg (wszTemp);
    } // End FOR

    // Display the local virtual memory ranges
    NewMsg (L"");
    NewMsg (L"== LCLMEMVIRTSTR ==");
#ifdef _WIN64
    NewMsg (L"     IniAddr     IncrSize  MaxSize GuardPage");
#elif defined (_WIN32)
    NewMsg (L" IniAddr IncrSize  MaxSize GuardPage");
#else
  #error Need code for this architecture.
#endif

    while (lpLstMVS)
    {
        wsprintfW (wszTemp,
                   L"%p %08X %08X %p %S",
                   lpLstMVS->IniAddr,
                   lpLstMVS->IncrSize,
                   lpLstMVS->MaxSize,
                   lpLstMVS->IniAddr + lpLstMVS->MaxSize,
                   lpLstMVS->lpText
                  );
        NewMsg (wszTemp);

        // Get the previous ptr in the chain
        lpLstMVS = lpLstMVS->lpPrvMVS;
    } // End WHILE

    // Display the SI stack
    NewMsg (L"");
    NewMsg (L"== SI STACK ==");

    // Loop backwards through the SI levels
    for (SILevel = 0;
         lpSISCur;
         lpSISCur = lpSISCur->lpSISPrv, SILevel++)
    {
        LPAPLCHAR lpMemName;            // Ptr to function name global memory

        // Split cases based upon the caller's function type
        switch (lpSISCur->DfnType)
        {
            case DFNTYPE_IMM:
#ifdef DEBUG
                NewMsg (WS_UTF16_IOTA);
#endif
                break;

            case DFNTYPE_OP1:
            case DFNTYPE_OP2:
            case DFNTYPE_FCN:
                // Lock the memory to get a ptr to it
                lpMemName = MyGlobalLock (lpSISCur->hGlbFcnName);

                // Format the Name, Line #, and Suspension marker
                wsprintfW (wszTemp,
                           L"%s[%d] %c",
                           lpMemName,
                           lpSISCur->CurLineNum,
                           " *"[lpSISCur->Suspended]);
                // We no longer need this ptr
                MyGlobalUnlock (lpSISCur->hGlbFcnName); lpMemName = NULL;

                // Display the function name & line #
                NewMsg (wszTemp);

                break;

            case DFNTYPE_EXEC:
                NewMsg (WS_UTF16_UPTACKJOT);

                break;

            case DFNTYPE_QUAD:
                NewMsg (WS_UTF16_QUAD);

                break;

            case DFNTYPE_UNK:
            default:
                NewMsg (L"***UNKNOWN***");

                break;
        } // End SWITCH
    } // End FOR

    // Tell the Crash Control window to display a MessageBox
    SendMessageW (hWndCC, MYWM_DISPMB, 0, 0);

#undef  NewMsg

    exit (exceptCode);
#endif
} // End DisplayException


//***************************************************************************
//  $DoStackWalk
//
//  Display the stack backtrace
//***************************************************************************

void DoStackWalk
    (LPCONTEXT lpContextRecord)

{
    STACKFRAME64         stackFrame = {0};
    CONTEXT              context;
    HANDLE               hProcess,
                         hThread;
////CallstackEntry       csEntry;
////IMAGEHLP_SYMBOL64   *pSym = NULL;
////IMAGEHLP_MODULE64_V2 Module;
////IMAGEHLP_LINE64      Line;
    LPBYTE               caller;            // Ptr to caller in stack trace
    APLU3264             nearAddress,       // Offset from closest address
                         nearIndex,         // Index into StartAddresses
                         nearAddress0,      // Offset from closest address
                         nearIndex0,        // Index into StartAddresses
                         nearAddress1,      // Offset from closest address
                         nearIndex1;        // Index into StartAddresses
    WCHAR                wszTemp[1024];     // Temp output save area
////char                 szAppDPFE[_MAX_PATH],
////                     szDir  [_MAX_DIR],
////                     szDrive[_MAX_DRIVE],
////                     szSymPath[_MAX_PATH];
////PSYMBOL_INFO         lpSymInfo;         // Ptr to ...

    // Initialize the handles
    hProcess = GetCurrentProcess ();
    hThread  = GetCurrentThread ();

////// Allocate space for the symbol name struc
////pSym = (IMAGEHLP_SYMBOL64 *) malloc (sizeof (IMAGEHLP_SYMBOL64) + STACKWALK_MAX_NAMELEN);
////if (!pSym)
////    goto CLEANUP;       // Not enough memory...
////memset (pSym, 0, sizeof (IMAGEHLP_SYMBOL64) + STACKWALK_MAX_NAMELEN);
////pSym->SizeOfStruct = sizeof (IMAGEHLP_SYMBOL64);
////pSym->MaxNameLength = STACKWALK_MAX_NAMELEN;

////// Allocate space for the symbol info struc
////lpSymInfo = (PSYMBOL_INFO) malloc (sizeof (SYMBOL_INFO) + STACKWALK_MAX_NAMELEN);
////if (!lpSymInfo)
////    goto CLEANUP;       // Not enough memory...
////memset (lpSymInfo, 0, sizeof (SYMBOL_INFO) + STACKWALK_MAX_NAMELEN);
////lpSymInfo->SizeOfStruct = sizeof (SYMBOL_INFO);
////lpSymInfo->MaxNameLen   = STACKWALK_MAX_NAMELEN;

////memset (&Line, 0, sizeof (Line));
////Line.SizeOfStruct = sizeof (Line);

////memset (&Module, 0, sizeof (Module));
////Module.SizeOfStruct = sizeof (Module);

////if (GetModuleFileNameA (_hInstance, szAppDPFE, sizeof (szSymPath)))
////{
////    // Split out the drive and path from the module filename
////    _splitpath (szAppDPFE, szDrive, szDir, NULL, NULL);
////
////    // Create the .HLP file name
////    _makepath  (szSymPath, szDrive, szDir, NULL, NULL);
////} else
////    szSymPath[0] = '\0';
////
////// Set the symbol options
////SymSetOptions (SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
////
////// Initialize the symbols
////SymInitialize (hProcess, szSymPath, TRUE);

    // Copy the outer ContextRecord
    context = *lpContextRecord;

    // Initialize these fields before the first call to StackWalk64
#ifdef _WIN64
    stackFrame.AddrPC.Offset     = lpContextRecord->Rip;    // Starting instruction address
    stackFrame.AddrFrame.Offset  = lpContextRecord->Rbp;    // Starting frame address
    stackFrame.AddrStack.Offset  = lpContextRecord->Rsp;    // Starting stack address
#elif defined (_WIN32)
    stackFrame.AddrPC.Offset     = lpContextRecord->Eip;    // Starting instruction address
    stackFrame.AddrFrame.Offset  = lpContextRecord->Ebp;    // Starting frame address
    stackFrame.AddrStack.Offset  = lpContextRecord->Esp;    // Starting stack address
#else
  #error Need code for this architecture.
#endif
    stackFrame.AddrPC.Segment    = 0;                       // Used for 16-bit addressing only
    stackFrame.AddrPC.Mode       = AddrModeFlat;            // Use flat mode addressing
    stackFrame.AddrFrame.Segment = 0;                       // Used for 16-bit addressing only
    stackFrame.AddrFrame.Mode    = AddrModeFlat;            // Use flat mode addressing
    stackFrame.AddrStack.Segment = 0;                       // Used for 16-bit addressing only
    stackFrame.AddrStack.Mode    = AddrModeFlat;            // Use flat mode addressing

    // Walk the stack
    while (StackWalk64 (
#ifdef _WIN64
                        IMAGE_FILE_MACHINE_AMD64,   // Machine architecture type
#elif defined (_WIN32)
                        IMAGE_FILE_MACHINE_I386,    // Machine architecture type
#else
  #error Need code for this architecture.
#endif
                        hProcess,                   // Process handle
                        hThread,                    // Thread handle
                       &stackFrame,                 // Ptr to stack frame (input/output)
                       &context,                    // Ptr to context struc (output)
                        NULL,                       // Ptr to ReadMemoryRoutine (may be NULL)
                       &SymFunctionTableAccess64,   // Ptr to function table access routine
                       &SymGetModuleBase64,         // Ptr to get module base routine
                        NULL))                      // Ptr to translate address routine for 16-bit addresses
    {
        // Check for infinite loop
        if (stackFrame.AddrPC.Offset EQ stackFrame.AddrReturn.Offset)
            break;

        // Check for valid instruction offset
        if (stackFrame.AddrPC.Offset EQ 0)
            break;

////    // Initialize the CallstackEntry struc
////    csEntry.offset             = stackFrame.AddrPC.Offset;
////    csEntry.name[0]            = 0;
////    csEntry.undName[0]         = 0;
////    csEntry.undFullName[0]     = 0;
////    csEntry.offsetFromSmybol   = 0;
////    csEntry.offsetFromLine     = 0;
////    csEntry.lineFileName[0]    = 0;
////    csEntry.lineNumber         = 0;
////    csEntry.loadedImageName[0] = 0;
////    csEntry.moduleName[0]      = 0;

////    DbgBrk ();

////    // Get the symbol name
////    if (SymFromAddr (hProcess, stackFrame.AddrPC.Offset, &csEntry.offsetFromSmybol, lpSymInfo))
////    {
////        DbgBrk ();
////
////
////
////    } // End IF

////    // Get the symbol name
////    if (SymGetSymFromAddr64 (hProcess, stackFrame.AddrPC.Offset, &csEntry.offsetFromSmybol, pSym))
////    {
////        DbgBrk ();
////
////        // Copy to temporary storage
////        lstrcpy (csEntry.name, pSym->Name);
////
////        // Undecorate the symbol name
////        UnDecorateSymbolName (pSym->Name, csEntry.undName,     STACKWALK_MAX_NAMELEN, UNDNAME_NAME_ONLY);
////        UnDecorateSymbolName (pSym->Name, csEntry.undFullName, STACKWALK_MAX_NAMELEN, UNDNAME_COMPLETE);
////    } // End IF

////    GetLastError ();

        caller = (LPBYTE) stackFrame.AddrPC.Offset;

        // Format the address
        FindRoutineAddress (caller, &nearAddress0, &nearIndex0, FALSE);
        FindRoutineAddress (caller, &nearAddress1, &nearIndex1, TRUE);
        if (nearAddress0 < nearAddress1)
        {
            nearAddress = nearAddress0;
            nearIndex   = nearIndex0;
        } else
        {
            nearAddress = nearAddress1;
            nearIndex   = nearIndex1;
        } // End IF/ELSE

        // If the address is out of bounds, just display the address
        if (nearAddress > 0x00100000)
            // Format the addresses
            wsprintfW (wszTemp,
                       L"%p -- EBP = %p",
                       caller,
                       stackFrame.AddrFrame.Offset);
        else
            // Format the addresses
            wsprintfW (wszTemp,
                       L"%p (%S + %p) -- EBP = %p",
                       caller,
                       StartAddresses[nearIndex].StartAddressName,
                       nearAddress,
                       stackFrame.AddrFrame.Offset);
#define NewMsg(a)   SendMessageW (hWndCC_LB, LB_ADDSTRING, 0, (LPARAM) (a)); UpdateWindow (hWndCC_LB)
        NewMsg (wszTemp);
#undef  NewMsg
    } // End WHILE
////CLEANUP:
////if (lpSymInfo)
////    free (lpSymInfo);
////if (pSym)
////    free (pSym);
} // End DoStackWalk


//***************************************************************************
//  $FindRoutineAddress
//
//  Find the address of the routine at or below a given address
//***************************************************************************

void FindRoutineAddress
    (LPUCHAR     exceptAddr,        // Exception address
     APLU3264   *lpNearAddress,     // Ptr to offset from closest address
     APLU3264   *lpNearIndex,       // Ptr to index into StartAddresses
     UBOOL       bDebugger)         // TRUE iff test for running under debugger

{
    UINT    i;                      // Loop counter
    LPUCHAR startAddr;              // Ptr to starting addr

    // Find the address closest to and at or below the given address
    for (i = 0,
           *lpNearAddress = (INT_PTR) -1,
           *lpNearIndex   = START_ADDRESSES_LENGTH;
         i < START_ADDRESSES_LENGTH;
         i++)
    {
        // Get the starting address
        startAddr = StartAddresses[i].StartAddressAddr;

        // If we're testing for debugger and it's a near JMP (E9 xx xx xx xx), ...
        if (bDebugger
         && IsGoodReadPtr (startAddr, 5)
         && *startAddr EQ 0xE9)
            // Skip over the instruction ("5 +") and add in the
            //   value of the near offset arg to the JMP
            startAddr += 5 + *(LPUINT) &startAddr[1];

        // Check against the actual address
        if (exceptAddr >= startAddr
         && ((UINT_PTR) (exceptAddr - startAddr)) < *lpNearAddress)
        {
            *lpNearAddress = (UINT_PTR) (exceptAddr - startAddr);
            *lpNearIndex   = i;
        } // End IF
    } // End FOR
} // End FindRoutineAddress


//***************************************************************************
//  $GuardAlloc
//
//  VirtualAlloc with a guard page
//***************************************************************************

LPVOID GuardAlloc
    (LPVOID lpvAddress,         // Address of region to reserve or commit
     DWORD  cbSize,             // Size of region
     DWORD  fdwAllocationType,  // Type of allocation
     DWORD  fdwProtect)         // Type of access protection

{
    LPVOID lpRetAddr;           // Return address

    // Round up the allocation size to a page boundary
    cbSize = PAGESIZE * ((cbSize + PAGESIZE - 1) / PAGESIZE);

    lpRetAddr =
      MyVirtualAlloc (lpvAddress,
                      cbSize + PAGESIZE,    // Size of region with an extra guard page
                      fdwAllocationType,
                      fdwProtect);
    return lpRetAddr;
} // End GuardAlloc


//***************************************************************************
//  $LinkMVS
//
//  Link an entry into the MemVirtStr chain
//***************************************************************************

void LinkMVS
    (LPMEMVIRTSTR lpCurMVS)         // Ptr to current MVS entry

{
    LPPERTABDATA    lpMemPTD;       // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // There is no next entry for us
    lpCurMVS->lpNxtMVS = NULL;

    // Our previous entry is the last entry
    lpCurMVS->lpPrvMVS = lpMemPTD->lpLstMVS;

    // If there's a previous entry, ...
    if (lpCurMVS->lpPrvMVS)
        // Its next entry is us
        lpCurMVS->lpPrvMVS->lpNxtMVS = lpCurMVS;

    // We are the new last entry
    lpMemPTD->lpLstMVS = lpCurMVS;
} // End LinkMVS


//***************************************************************************
//  $UnlinkMVS
//
//  Unlink an entry from the MemVirtStr chain
//***************************************************************************

void UnlinkMVS
    (LPMEMVIRTSTR lpCurMVS)         // Ptr to current MVS entry

{
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // If there's a previous entry, ...
    if (lpCurMVS->lpPrvMVS)
        // Unlink us from it
        lpCurMVS->lpPrvMVS->lpNxtMVS = lpCurMVS->lpNxtMVS;

    // If there's a next entry, ...
    if (lpCurMVS->lpNxtMVS)
        // Unlink us from it
        lpCurMVS->lpNxtMVS->lpPrvMVS = lpCurMVS->lpPrvMVS;

    // If we're the last entry, ...
    if (lpMemPTD->lpLstMVS EQ lpCurMVS)
        // Set the new last entry as our previous entry
        lpMemPTD->lpLstMVS = lpCurMVS->lpPrvMVS;
} // End UnlinkMVS


//***************************************************************************
//  End of File: except.c
//***************************************************************************
