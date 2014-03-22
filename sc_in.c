//***************************************************************************
//  NARS2000 -- System Command:  )IN
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2013 Sudley Place Software

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
#include <stdio.h>
#include <time.h>
#include "headers.h"
#include "transfer.h"
#include "debug.h"              // For xxx_TEMP_OPEN macros


//***************************************************************************
//  $CmdIn_EM
//
//  Execute the system command:  )IN filename[.ext]
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CmdIn_EM"
#else
#define APPEND_NAME
#endif

UBOOL CmdIn_EM
    (LPWCHAR lpwszTail)                     // Ptr to command line tail

{
    LPPERTABDATA  lpMemPTD;                 // Ptr to PerTabData global memory
    WCHAR         wszDrive[_MAX_DRIVE],
                  wszDir  [_MAX_DIR],
                  wszFname[_MAX_FNAME],
                  wszExt  [_MAX_EXT];
    SECURITY_ATTRIBUTES secAttr = {sizeof (SECURITY_ATTRIBUTES), NULL, TRUE};
    HANDLE        hAtfFile = NULL,          // Handle for .atf file
                  hAtfMap = NULL,           // Handle from CreateFileMappingW
                  hAtfView = NULL;          // Handle from MapViewOfFile
    LPUCHAR       lpAtfView;                // Ptr to file contents
    LPWCHAR       lpwszTemp,                // Ptr to temporary storage
                  lpwszOrigTemp,            // Original lpMemPTD->lpwszTemp
                  lpwszFormat;              // Ptr to format area
    DWORD         dwAtfFileSize;            // Byte size of .atf file
    WCHAR         wszTemp[1024];            // Temporary storage for message strings
#define wszTempLen      countof (wszTemp)
    UINT          uLen,                     // Length of output save area in WCHARs
                  uOldRecNo,                // Starting record # in file (for range display)
                  uRecNo = 0,               // Record # in file
                  uMaxSize;                 // Maximum size of lpwszTemp
    UBOOL         bRet = FALSE,             // TRUE iff the result is valid
                  bIsEBCDIC;                // TRUE iff the orignial .atf file is in EBCDIC format
    FILETIME      ftCreation;               // Creation timestamp
    HWND          hWndSM,                   // Session Manager Window handle
                  hWndEC;                   // Edit Ctrl    ...
    VARS_TEMP_OPEN

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get ptr to temporary storage & maximum size
    lpwszOrigTemp =
    lpwszTemp     = lpMemPTD->lpwszTemp;
    uMaxSize      = lpMemPTD->uTempMaxSize;
    hWndSM        = lpMemPTD->hWndSM;
    lpwszFormat   = lpMemPTD->lpwszFormat;

    // Get the Edit Ctrl window handle
    hWndEC = (HWND) (HANDLE_PTR) GetWindowLongPtrW (hWndSM, GWLSF_HWNDEC);

    // Check for empty string
    if (lpwszTail[0] EQ WC_EOS)
        goto INCORRECT_COMMAND_EXIT;

    // Get the command tail length
    uLen = lstrlenW (lpwszTail);

    // Check for leading and trailing double quotes
    if (lpwszTail[0       ] EQ WC_DQ
     && lpwszTail[uLen - 1] EQ WC_DQ)
    {
        // Zap the trailing DQ
        lpwszTail[--uLen] = WC_EOS;

        // Move the string down over the leading DQ
        CopyMemoryW (lpwszTail, &lpwszTail[1], uLen);
    } // End IF

    // Split out the drive and path from the module filename
    _wsplitpath (lpwszTail, wszDrive, wszDir, wszFname, wszExt);

    // If there's no extension, supply one
    if (wszExt[0] EQ WC_EOS)
        // Put it all back together
        _wmakepath  (lpwszTail, wszDrive, wszDir, wszFname, WS_ATFEXT);

    // See if we can open this file
    hAtfFile =
      CreateFileW (lpwszTail,                   // Pointer to name of the file
                   0
                 | GENERIC_READ                 // Access (read-write) mode
                 | GENERIC_WRITE
                   ,
                   FILE_SHARE_READ
                 | FILE_SHARE_WRITE,            // Share mode
                  &secAttr,                     // Pointer to security descriptor
                   OPEN_EXISTING,               // How to create
                   0                            // File attributes
                 | FILE_ATTRIBUTE_NORMAL,
                   NULL);                       // Handle to file with attributes to copy
    if (hAtfFile EQ INVALID_HANDLE_VALUE)
        goto FILENOTFOUND_EXIT;

    // Get the file size (it always fits in a DWORD)
    dwAtfFileSize = GetFileSize (hAtfFile, NULL);

    // Map the file
    hAtfMap =
      CreateFileMappingW (hAtfFile,             // Handle to file to map
                          NULL,                 // Optional security attributes
                          PAGE_WRITECOPY,       // Protection for mapping object
                          0,                    // High-order 32 bits of object size
                          dwAtfFileSize,        // Low-order 32 bits of object size
                          NULL);                // Name of file-mapping object
    if (hAtfMap EQ NULL)
    {
        lstrcpyW (wszTemp, L"Unable to create file mapping:  ");
        uLen = lstrlenW (wszTemp);

        FormatMessageW (FORMAT_MESSAGE_FROM_SYSTEM, // Source and processing options
                        NULL,                       // Pointer to  message source
                        GetLastError (),            // Requested message identifier
                        0,                          // Language identifier for requested message
                       &wszTemp[uLen],              // Pointer to message buffer
                        wszTempLen - uLen,          // Maximum size of message buffer
                        NULL);                      // Address of array of message inserts
        MBW (wszTemp);

        goto ERROR_EXIT;
    } // End IF

    // Get a view of the file
    hAtfView =
      MapViewOfFile (hAtfMap,                   // File-mapping object to map into address space
                     FILE_MAP_COPY,             // Access mode
                     0,                         // High-order 32 bits of file offset
                     0,                         // Low-order 32 bits of file offset
                     dwAtfFileSize);            // Number of bytes to map
    if (hAtfView EQ NULL)
    {
        lstrcpyW (wszTemp, L"Unable to map view of file:  ");
        uLen = lstrlenW (wszTemp);

        FormatMessageW (FORMAT_MESSAGE_FROM_SYSTEM, // Source and processing options
                        NULL,                       // Pointer to  message source
                        GetLastError (),            // Requested message identifier
                        0,                          // Language identifier for requested message
                       &wszTemp[uLen],              // Pointer to message buffer
                        wszTempLen - uLen,          // Maximum size of message buffer
                        NULL);                      // Address of array of message inserts
        MBW (wszTemp);

        goto ERROR_EXIT;
    } // End IF

    // lpAtfView is a WCHAR pointer to the file contents
    lpAtfView = hAtfView;

    // Determine the file character set
    bIsEBCDIC =((lpAtfView[0] EQ EBCDIC_BLANK) || (lpAtfView[0] EQ EBCDIC_X) || (lpAtfView[0] EQ EBCDIC_STAR));

    // If the file type is EBCDIC, translate it to ASCII
    if (bIsEBCDIC)
        CmdInEBCDIC2ASCII (lpAtfView, dwAtfFileSize);

    // Ensure that the file contents now look like ASCII
    Assert ((lpAtfView[0] EQ AC_BLANK) || (lpAtfView[0] EQ AC_X) || (lpAtfView[0] EQ AC_STAR));

    while (dwAtfFileSize)
    {
        // Save the starting record #
        uOldRecNo = uRecNo + 1;

        // Restore the original ptr
        lpMemPTD->lpwszTemp = lpwszOrigTemp;
        CHECK_TEMP_OPEN

        // Copy and translate the next record into lpwszTemp
        lpAtfView = CmdInCopyAndTranslate_EM (lpAtfView, &dwAtfFileSize, lpwszTemp, uMaxSize, &uLen, &ftCreation, &uRecNo, bIsEBCDIC);

        // Protect the text
        lpMemPTD->lpwszTemp += lstrlenW (lpMemPTD->lpwszTemp);
        EXIT_TEMP_OPEN

        if (!lpAtfView)
            goto ERROR_EXIT;

        // Process the record in lpwszTemp of length uLen

        // Split cases based upon the first char
        switch (lpwszTemp[0])
        {
            case L'A':
                // The transfer form of an array is as follows:
                //     Aarrayname{is}values
                //   or
                //     Aarrayname{is}shape{rho}values
                //   where 'shape' is the shape of the array and 'values' are the values

                // Point to the name, skipping over the type char
                if (!TransferInverseArr2_EM (&lpwszTemp[1], uLen - 1, uOldRecNo, uRecNo, NULL, &ftCreation, TRUE))
                    goto ERROR_EXIT;
                break;

            case L'F':
                // The transfer form of a function is as follows:
                //     Ffunctionname []FX 'header' 'line 1' ...
                //   or
                //     Ffunctionname attrs []FX 'header' 'line 1' ...
                //   where the attrs are optional

                // Point to the name, skipping over the type char
                if (!TransferInverseFcn2_EM (&lpwszTemp[1], uOldRecNo, uRecNo, NULL, &ftCreation, TRUE))
                    goto ERROR_EXIT;
                break;

            case L'C':
                // The migration transfer form of a character array is as follows:
                //     Cvarname rank shape values
                //   where 'rank' is a number, 'shape' is 'rank' numbers and 'values' are x/shape characters
                if (!TransferInverseChr1_EM (lpwszTemp, uOldRecNo, uRecNo, NULL, TRUE))
                    goto ERROR_EXIT;
                break;

            case L'N':
                // The migration transfer form of a numeric array is as follows:
                //     Nvarname rank shape values
                //   where 'rank' is a number, 'shape' is 'rank' numbers and 'values' are x/shape numbers
                if (!TransferInverseNum1_EM (lpwszTemp, uOldRecNo, uRecNo, NULL, TRUE))
                    goto ERROR_EXIT;
                break;

            defstop
                break;
        } // End SWITCH

        // If there's an undisplayed result, ...
        if (lpMemPTD->YYResExec.YYInuse)
        {
            // Free it
            FreeResult (&lpMemPTD->YYResExec);

            // We no longer need these values
            ZeroMemory (&lpMemPTD->YYResExec, sizeof (lpMemPTD->YYResExec));
        } // End IF
    } // End WHILE

    // Mark as sucessful
    bRet = TRUE;

    goto NORMAL_EXIT;

INCORRECT_COMMAND_EXIT:
    IncorrectCommand ();

    goto ERROR_EXIT;

FILENOTFOUND_EXIT:
    ReplaceLastLineCRPmt (ERRMSG_FILE_NOT_FOUND APPEND_NAME);

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (hAtfView)
    {
        UnmapViewOfFile (hAtfView); hAtfView = NULL;
    } // End IF

    if (hAtfMap)
    {
        CloseHandle (hAtfMap); hAtfMap = NULL;
    } // End IF

    if (hAtfFile)
    {
        CloseHandle (hAtfFile); hAtfFile = NULL;
    } // End IF

    // Restore the original ptr
    lpMemPTD->lpwszTemp = lpwszOrigTemp;

    return bRet;
} // End CmdIn_EM
#undef  APPEND_NAME


//***************************************************************************
//  $TransferInverseArr2_EM
//
//  Transfer form inverse of a Type-2 array
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- TransferInverseArr2_EM"
#else
#define APPEND_NAME
#endif

UBOOL TransferInverseArr2_EM
    (LPWCHAR    lpwName,                    // Ptr to incoming data
     APLNELM    aplNELM,                    // NELM of lpwName
     UINT       uOldRecNo,                  // Starting record # from .atf file
     UINT       uRecNo,                     // Ending   ...
     LPTOKEN    lptkFunc,                   // Ptr to function token (may be NULL if called from )IN)
     FILETIME  *lpftCreation,               // Ptr to timestamp (if any) (may be NULL)
     UBOOL      bSysCmd)                    // TRUE iff called from a system command

{
    LPPERTABDATA  lpMemPTD;                 // Ptr to PerTabData global memory
    LPWCHAR       lpwNameEnd = NULL,        // Ptr to end of name
                  lpwszFormat;              // Ptr to format area
    WCHAR         wch;                      // Temporary character
    EXIT_TYPES    exitType;                 // Exit type from ImmExecStmt
    HWND          hWndSM,                   // Session Manager Window handle
                  hWndEC;                   // Edit Ctrl    ...
    STFLAGS       stFlags;                  // ST flags for name lookup
    LPSYMENTRY    lpSymEntry;               // Ptr to SYMENTRY for name lookup
    UBOOL         bRet = FALSE;             // TRUE iff result is valid

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get window handle of the Session Manager
    hWndSM = lpMemPTD->hWndSM;

    // Get ptr to temporary format save area
    lpwszFormat = lpMemPTD->lpwszFormat;

    // Get the Edit Ctrl window handle
    hWndEC = (HWND) (HANDLE_PTR) GetWindowLongPtrW (hWndSM, GWLSF_HWNDEC);

    // Search for the left arrow which marks the end of the name
    lpwNameEnd = SkipToCharW (lpwName, UTF16_LEFTARROW);
    if (!lpwNameEnd)
    {
        if (!lptkFunc)
        {
            // Format the error message
            wsprintfW (lpwszFormat,
                       L"Missing left arrow in .atf file, lines %u-%u (origin-1), records starts with %.20s.",
                       uOldRecNo,
                       uRecNo,
                       lpwName);
            // Display the error message
            AppendLine (lpwszFormat, FALSE, TRUE);
        } // End IF

        goto ERROR_EXIT;
    } // End IF

    // Ensure the name is properly terminated
    wch = *lpwNameEnd;
    *lpwNameEnd = WC_EOS;

    // Set the flags for what we're looking up/appending
    ZeroMemory (&stFlags, sizeof (stFlags));
    stFlags.Inuse   = TRUE;

    // If it's a system name, ...
    if (IsSysName (lpwName))
    {
        // Convert the name to lowercase
        CharLowerBuffW (lpwName, (UINT) (lpwNameEnd - lpwName));

        // Tell 'em we're looking for system names
        stFlags.ObjName = OBJNAME_SYS;
    } else
        // Tell 'em we're looking for user names
        stFlags.ObjName = OBJNAME_USR;

    // Look up the name
    lpSymEntry = SymTabLookupName (lpwName, &stFlags);
    if (!lpSymEntry)
    {
        // If it's a system name and it's not found, then we don't support it
        if (IsSysName (lpwName))
            goto INVALIDSYSNAME_EXIT;
        lpSymEntry = SymTabAppendNewName_EM (lpwName, &stFlags);
        if (!lpSymEntry)
            goto STFULL_EXIT;

        // Set the object name and name type value for this new entry
        lpSymEntry->stFlags.ObjName    = OBJNAME_USR;
        lpSymEntry->stFlags.stNameType = NAMETYPE_VAR;
    } else
    // If this is a system command, we change the global values only
    if (bSysCmd)
    while (lpSymEntry->stPrvEntry)
        lpSymEntry = lpSymEntry->stPrvEntry;

    // If this isn't []DM, ...
    if (lstrcmpiW (lpwName, $QUAD_DM) NE 0)
    {
        // Restore the zapped char
        *lpwNameEnd = wch;

        // Execute the statement starting with lpwName
        exitType =
          PrimFnMonUpTackJotCSPLParse (hWndEC,              // Edit Ctrl window handle
                                       lpMemPTD,            // Ptr to PerTabData global memory
                                       lpwName,             // Ptr to text of line to execute
                                       lstrlenW (lpwName),  // Length of the line to execute
                                       TRUE,                // TRUE iff we should act on errors
                                       FALSE,               // TRUE iff we're to skip the depth check
                                       NULL);               // Ptr to function token
        // Split cases based upon the exit type
        switch (exitType)
        {
            case EXITTYPE_DISPLAY:
            case EXITTYPE_NODISPLAY:
                // If the Execute/Quad result is present, free it
                if (lpMemPTD->YYResExec.tkToken.tkFlags.TknType)
                {
                    // Free it
                    FreeResult (&lpMemPTD->YYResExec);

                    // We no longer need these values
                    ZeroMemory (&lpMemPTD->YYResExec, sizeof (lpMemPTD->YYResExec));
                } // End IF

                break;

            case EXITTYPE_ERROR:
            case EXITTYPE_STACK_FULL:
                if (!lptkFunc)
                {
                    // Display the leading part of the error message
                    AppendLine (lpMemPTD->lpwszErrorMessage, FALSE, TRUE);

                    // Ensure the name is properly terminated
                    *lpwNameEnd = WC_EOS;

                    // Format the trailing part of the error message
                    wsprintfW (lpwszFormat,
                               L"  Error in .atf file, lines %u-%u (origin-1), type-2 variable:  %s.",
                               uOldRecNo,
                               uRecNo,
                               lpwName);
                    // Display the trailing part of the error message
                    AppendLine (lpwszFormat, TRUE, TRUE);
                } // End IF

                goto ERROR_EXIT;

            defstop
                break;
        } // End SWITCH
    } // End IF

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

INVALIDSYSNAME_EXIT:
    if (!lptkFunc)
    {
        // Format the error message
        wsprintfW (lpwszFormat,
                   L"INVALID " WS_UTF16_QUAD L"NAME in .atf file, lines %u-%u (origin-1), record starts with %.20s.",
                   uOldRecNo,
                   uRecNo,
                   lpwName);
        // Display the error message
        AppendLine (lpwszFormat, FALSE, TRUE);
    } // End IF

    goto ERROR_EXIT;

STFULL_EXIT:
    if (!lptkFunc)
        AppendLine (ERRMSG_SYMBOL_TABLE_FULL APPEND_NAME, FALSE, TRUE);

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    // Restore the char at the name end
    if (lpwNameEnd)
        *lpwNameEnd = wch;

    return bRet;
} // End TransInverseArr2_EM
#undef  APPEND_NAME


//***************************************************************************
//  $TransferInverseFcn2_EM
//
//  Transfer form inverse of a Type-2 function
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- TransferInverseFcn2_EM"
#else
#define APPEND_NAME
#endif

UBOOL TransferInverseFcn2_EM
    (LPWCHAR    lpwName,                    // Ptr to incoming data
     UINT       uOldRecNo,                  // Starting record # from .atf file
     UINT       uRecNo,                     // Ending   ...
     LPTOKEN    lptkFunc,                   // Ptr to function token (may be NULL if called from )IN)
     FILETIME  *lpftCreation,               // Ptr to timestamp (if any) (may be NULL)
     UBOOL      bSysCmd)                    // TRUE iff called from a system command

{
    LPPERTABDATA  lpMemPTD;                 // Ptr to PerTabData global memory
    LPWCHAR       lpwNameEnd = NULL,        // Ptr to end of name
                  lpwData,                  // Ptr to data ...
                  lpwszFormat;              // Ptr to format area
    EXIT_TYPES    exitType;                 // Exit type from ImmExecStmt
    HWND          hWndSM,                   // Session Manager Window handle
                  hWndEC;                   // Edit Ctrl    ...
    WCHAR         wch;                      // Temporary char
    UBOOL         bRet = FALSE;             // TRUE iff the result is valid

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get window handle of the Session Manager
    hWndSM = lpMemPTD->hWndSM;

    // Get ptr to temporary format save area
    lpwszFormat = lpMemPTD->lpwszFormat;

    // Get the Edit Ctrl window handle
    hWndEC = (HWND) (HANDLE_PTR) GetWindowLongPtrW (hWndSM, GWLSF_HWNDEC);

    if (!lptkFunc)
    {
        LPWCHAR lpwQ1,
                lpwQ2;

        lpwQ1 = SkipToCharW (lpwName, UTF16_QUAD);
        lpwQ2 = SkipToCharW (lpwName, UTF16_QUAD2);

        // Point to the leading Quad of []FX
        lpwData = min (lpwQ1, lpwQ2);

        // Preceded with a left arrow so the result is not displayed
        *--lpwData = UTF16_LEFTARROW;
    } else
        lpwData = lpwName;

    // ***FIXME*** -- Ensure we change the global not local value
    // ***FIXME*** -- Ensure we don't change []DM/[]ES

    // Execute the statement starting with lpwData
    exitType =
      PrimFnMonUpTackJotCSPLParse (hWndEC,              // Edit Ctrl window handle
                                   lpMemPTD,            // Ptr to PerTabData global memory
                                   lpwData,             // Ptr to text of line to execute
                                   lstrlenW (lpwData),  // Length of the line to execute
                                   TRUE,                // TRUE iff we should act on errors
                                   FALSE,               // TRUE iff we're to skip the depth check
                                   NULL);               // Ptr to function token
    Assert (exitType EQ EXITTYPE_NODISPLAY
         || exitType EQ EXITTYPE_NOVALUE
         || exitType EQ EXITTYPE_ERROR);
    // Copy the ptr to the name end
    //   and skip over it
    lpwNameEnd = lpwData++;

    // Ensure the name is properly terminated
    wch = *lpwNameEnd;
    *lpwNameEnd = WC_EOS;

    // Check for errors
    if (exitType EQ EXITTYPE_ERROR
     || lpMemPTD->uErrLine NE NEG1U)
    {
        if (!lptkFunc)
        {
            // Display the leading part of the error message
            AppendLine (lpMemPTD->lpwszErrorMessage, FALSE, TRUE);

            // Format the trailing part of the error message
            if (lpMemPTD->uErrLine EQ NEG1U)
                wsprintfW (lpwszFormat,
                           L"  Error in .atf file, lines %u-%u (origin-1), type-2 function:  %s.",
                           uOldRecNo,
                           uRecNo,
                           lpwName);
            else
                wsprintfW (lpwszFormat,
                           L"  Error in .atf file, lines %u-%u (origin-1), type-2 function:  %s, line # %d.",
                           uOldRecNo,
                           uRecNo,
                           lpwName,
                           lpMemPTD->uErrLine + 1);
            // Display the trailing part of the error message
            AppendLine (lpwszFormat, TRUE, TRUE);
        } // End IF

        goto ERROR_EXIT;
    } else
    // If there was a creation time, ...
    if (lpftCreation
     && (lpftCreation->dwLowDateTime NE 0
      || lpftCreation->dwHighDateTime NE 0))
    {
        STFLAGS       stFlags;                  // ST flags for name lookup
        LPSYMENTRY    lpSymEntry;               // Ptr to SYMENTRY for name lookup

        // Set the flags for what we're looking up/appending
        ZeroMemory (&stFlags, sizeof (stFlags));
        stFlags.Inuse   = TRUE;

        // If it's a system name, ...
        if (IsSysName (lpwName))
        {
            // Convert the name to lowercase
            CharLowerBuffW (lpwName, (UINT) (lpwData - lpwName));

            // Tell 'em we're looking for system names
            stFlags.ObjName = OBJNAME_SYS;
        } else
            // Tell 'em we're looking for user names
            stFlags.ObjName = OBJNAME_USR;

        // Look up the name
        lpSymEntry = SymTabLookupName (lpwName, &stFlags);
        if (lpSymEntry)
        {
            LPDFN_HEADER lpMemDfnHdr;   // Ptr to user-defined function/operator header ...
            HGLOBAL       hGlbDfnHdr;   // User-defined function/operator header ...

            // ***FIXME*** -- Ensure we change the global not local value

            // Get the function's global memory handle
            hGlbDfnHdr = lpSymEntry->stData.stGlbData;

            Assert (IsGlbTypeDfnDir_PTB (hGlbDfnHdr));

            // Lock the memory to get a ptr to it
            lpMemDfnHdr = MyGlobalLock (hGlbDfnHdr);

            // Save creation time
            lpMemDfnHdr->ftCreation = *lpftCreation;

            // We no longer need this ptr
            MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
        } // End IF
    } // End IF/ELSE

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    // Restore the char at the name end
    if (lpwNameEnd)
        *lpwNameEnd = wch;

    return bRet;
} // End TransferInverseFcn2_EM
#undef  APPEND_NAME


//***************************************************************************
//  $TransferInverseChr1_EM
//
//  Transfer form inverse of a Type-1 character array
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- TransferInverseChr1_EM"
#else
#define APPEND_NAME
#endif

UBOOL TransferInverseChr1_EM
    (LPWCHAR lpwszTemp,                     // Ptr to incoming data
     UINT    uOldRecNo,                     // Starting record # from .atf file
     UINT    uRecNo,                        // Ending   ...
     LPTOKEN lptkFunc,                      // Ptr to function token (may be NULL if called from )IN)
     UBOOL   bSysCmd)                       // TRUE iff called from a system command

{
    LPPERTABDATA  lpMemPTD;                 // Ptr to PerTabData global memory
    LPWCHAR       lpwName,                  // Ptr to name portion of array
                  lpwNameEnd = NULL,        // Ptr to end of name
                  lpwData,                  // Ptr to data ...
                  lpwTemp,                  // Ptr to temporary
                  lpwszFormat;              // Ptr to format area
    APLUINT       ByteRes;                  // # bytes in the result
    HGLOBAL       hGlbRes;                  // Result global memory handle
    LPVOID        lpMemRes;                 // Ptr to result global memory
    UBOOL         bRet = FALSE;             // TRUE iff result is valid
    STFLAGS       stFlags;                  // ST flags for name lookup
    LPSYMENTRY    lpSymEntry;               // Ptr to SYMENTRY for name lookup
    APLRANK       aplRankRes;               // Result rank
    APLNELM       aplNELMRes,               // ...    NELM
                  aplTemp;                  // Temporary value
    UINT          uCnt;                     // Loop counter
    WCHAR         wch;                      // Temporary char

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get ptr to temporary format save area
    lpwszFormat = lpMemPTD->lpwszFormat;

    // Point to the name, skipping over the type char
    lpwName = &lpwszTemp[1];

    // Search for the blank which marks the end of the name
    lpwData = SkipToCharW (lpwName, L' ');
    if (!lpwData)
    {
        if (!lptkFunc)
        {
            // Format the error message
            wsprintfW (lpwszFormat,
                       L"Missing blank in .atf file, lines %u-%u (origin-1), record starts with %.20s.",
                       uOldRecNo,
                       uRecNo,
                       lpwszTemp);
            // Display the error message
            AppendLine (lpwszFormat, FALSE, TRUE);
        } // End IF

        goto ERROR_EXIT;
    } // End IF

    // Copy the ptr to the name end
    //   and skip over it
    lpwNameEnd = lpwData++;

    // Ensure the name is properly terminated
    wch = *lpwNameEnd;
    *lpwNameEnd = WC_EOS;

    // Set the flags for what we're looking up/appending
    ZeroMemory (&stFlags, sizeof (stFlags));
    stFlags.Inuse   = TRUE;

    // If it's a system name, ...
    if (IsSysName (lpwName))
    {
        // Convert the name to lowercase
        CharLowerBuffW (lpwName, (UINT) (lpwData - lpwName));

        // Tell 'em we're looking for system names
        stFlags.ObjName = OBJNAME_SYS;
    } else
        // Tell 'em we're looking for user names
        stFlags.ObjName = OBJNAME_USR;

    // Look up the name
    lpSymEntry = SymTabLookupName (lpwName, &stFlags);
    if (!lpSymEntry)
    {
        // If it's a system name and it's not found, then we don't support it
        if (IsSysName (lpwName))
            goto INVALIDSYSNAME_EXIT;
        lpSymEntry = SymTabAppendNewName_EM (lpwName, &stFlags);
        if (!lpSymEntry)
            goto STFULL_EXIT;

        // Set the object name and name type value for this new entry
        lpSymEntry->stFlags.ObjName    = OBJNAME_USR;
        lpSymEntry->stFlags.stNameType = NAMETYPE_VAR;
    } else
    // If this is a system command, we change the global values only
    if (bSysCmd)
    while (lpSymEntry->stPrvEntry)
        lpSymEntry = lpSymEntry->stPrvEntry;

    // Get the rank
    sscanfW (lpwData, L"%I64u", &aplRankRes);

    // Skip past the rank
    lpwData = SkipPastCharW (lpwData, L' ');

    // Save the ptr for later use
    lpwTemp = lpwData;

    // Calculate the NELM
    aplNELMRes = 1;

    // Loop through the shape vector
    for (uCnt = 0; uCnt < aplRankRes; uCnt++)
    {
        // Get the next shape value
        sscanfW (lpwData, L"%I64u", &aplTemp);

        // Accumulate into the NELM
        aplNELMRes *= aplTemp;

        // Skip past the shape value
        lpwData = SkipPastCharW (lpwData, L' ');
    } // End FOR

    // If the result is a scalar, ...
    if (IsScalar (aplRankRes))
    {
        // Set the new value
        lpSymEntry->stFlags.Imm     =
        lpSymEntry->stFlags.Value   = TRUE;
        lpSymEntry->stFlags.ImmType = IMMTYPE_CHAR;
        lpSymEntry->stData.stChar   = *lpwData;
    } else
    {
        // Calculate space needed for the result
        ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRes, aplRankRes);

        // Check for overflow
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;

        // Now we can allocate the storage for the result
        hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (!hGlbRes)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = ARRAY_CHAR;
////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = aplNELMRes;
        lpHeader->Rank       = aplRankRes;
#undef  lpHeader

        // Skip over the header to the dimensions
        lpMemRes = VarArrayBaseToDim (lpMemRes);

        // Save the dimensions

        // Loop through the shape vector
        for (uCnt = 0; uCnt < aplRankRes; uCnt++)
        {
            // Get the next shape value
            sscanfW (lpwTemp, L"%I64u", ((LPAPLDIM) lpMemRes)++);

            // Skip past the shape value
            lpwTemp = SkipPastCharW (lpwTemp, L' ');
        } // End FOR

        // Copy the data to the result
        CopyMemoryW (lpMemRes, lpwData, (APLU3264) aplNELMRes);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

        // If the old value is a global, free it
        if (lpSymEntry->stFlags.Value && !lpSymEntry->stFlags.Imm)
            FreeResultGlobalVar (lpSymEntry->stData.stGlbData);
        // Save the global in the STE
        lpSymEntry->stFlags.Imm   = FALSE;
        lpSymEntry->stFlags.Value = TRUE;
        lpSymEntry->stData.stGlbData = MakePtrTypeGlb (hGlbRes);
    } // End IF/ELSE

    // Set the name type for this new entry
    lpSymEntry->stFlags.stNameType = NAMETYPE_VAR;

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

INVALIDSYSNAME_EXIT:
    if (!lptkFunc)
    {
        // Format the error message
        wsprintfW (lpwszFormat,
                   L"INVALID " WS_UTF16_QUAD L"NAME in .atf file, lines %u-%u (origin-1), record starts with %.20s.",
                   uOldRecNo,
                   uRecNo,
                   lpwszTemp);
        // Display the error message
        AppendLine (lpwszFormat, FALSE, TRUE);
    } // End IF

    goto ERROR_EXIT;

STFULL_EXIT:
    if (!lptkFunc)
        AppendLine (ERRMSG_SYMBOL_TABLE_FULL APPEND_NAME, FALSE, TRUE);

    goto ERROR_EXIT;

WSFULL_EXIT:
    if (!lptkFunc)
        AppendLine (ERRMSG_WS_FULL APPEND_NAME, FALSE, TRUE);

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    // Restore the char at the name end
    if (lpwNameEnd)
        *lpwNameEnd = wch;

    return bRet;
} // End TransferInverseChr1_EM
#undef  APPEND_NAME


//***************************************************************************
//  $TransferInverseNum1_EM
//
//  Transfer form inverse of a Type-1 numeric array
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- TransferInverseNum1_EM"
#else
#define APPEND_NAME
#endif

UBOOL TransferInverseNum1_EM
    (LPWCHAR lpwszTemp,                     // Ptr to incoming data
     UINT    uOldRecNo,                     // Starting record # from .atf file
     UINT    uRecNo,                        // Ending   ...
     LPTOKEN lptkFunc,                      // Ptr to function token (may be NULL if called from )IN)
     UBOOL   bSysCmd)                       // TRUE iff called from a system command

{
    LPPERTABDATA  lpMemPTD;                 // Ptr to PerTabData global memory
    LPWCHAR       lpwName,                  // Ptr to name portion of array
                  lpwNameEnd = NULL,        // Ptr to end of name
                  lpwData,                  // Ptr to data ...
                  lpwTemp,                  // Ptr to temporary
                  lpwszFormat;              // Ptr to format area
    UBOOL         bRet = FALSE;             // TRUE iff result is valid
    STFLAGS       stFlags;                  // ST flags for name lookup
    LPSYMENTRY    lpSymEntry;               // Ptr to SYMENTRY for name lookup
    APLRANK       aplRankRes;               // Result rank
    APLNELM       aplNELMRes,               // ...    NELM
                  aplTemp;                  // Temporary value
    APLU3264      uLen,                     // Length of output save area in WCHARs
                  uCnt;                     // Loop counter
    EXIT_TYPES    exitType;                 // Exit type from ImmExecStmt
    HWND          hWndSM,                   // Session Manager Window handle
                  hWndEC;                   // Edit Ctrl    ...
    WCHAR         wch;                      // Temporary char

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get ptr to temporary format save area
    lpwszFormat = lpMemPTD->lpwszFormat;
    hWndSM      = lpMemPTD->hWndSM;

    // Get the Edit Ctrl window handle
    hWndEC = (HWND) (HANDLE_PTR) GetWindowLongPtrW (hWndSM, GWLSF_HWNDEC);

    // Point to the name, skipping over the type char
    lpwName = &lpwszTemp[1];

    // Search for the blank which marks the end of the name
    lpwData = SkipToCharW (lpwName, L' ');
    if (!lpwData)
    {
        if (!lptkFunc)
        {
            // Format the error message
            wsprintfW (lpwszFormat,
                       L"Missing blank in .atf file, lines %u-%u (origin-1), record starts with %.20s.",
                       uOldRecNo,
                       uRecNo,
                       lpwszTemp);
            // Display the error message
            AppendLine (lpwszFormat, FALSE, TRUE);
        } // End IF

        goto ERROR_EXIT;
    } // End IF

    // Copy the ptr to the name end
    lpwNameEnd = lpwData;

    // Ensure the name is properly terminated
    wch = *lpwNameEnd;
    *lpwNameEnd = WC_EOS;

    // Set the flags for what we're looking up/appending
    ZeroMemory (&stFlags, sizeof (stFlags));
    stFlags.Inuse   = TRUE;

    // If it's a system name, ...
    if (IsSysName (lpwName))
    {
        // Convert the name to lowercase
        CharLowerBuffW (lpwName, (UINT) (lpwData - lpwName));

        // Tell 'em we're looking for system names
        stFlags.ObjName = OBJNAME_SYS;
    } else
        // Tell 'em we're looking for user names
        stFlags.ObjName = OBJNAME_USR;

    // Look up the name
    lpSymEntry = SymTabLookupName (lpwName, &stFlags);
    if (!lpSymEntry)
    {
        // If it's a system name and it's not found, then we don't support it
        if (IsSysName (lpwName))
            goto INVALIDSYSNAME_EXIT;
        lpSymEntry = SymTabAppendNewName_EM (lpwName, &stFlags);
        if (!lpSymEntry)
            goto STFULL_EXIT;

        // Set the object name and name type value for this new entry
        lpSymEntry->stFlags.ObjName    = OBJNAME_USR;
        lpSymEntry->stFlags.stNameType = NAMETYPE_VAR;
    } else
    // If this is a system command, we change the global values only
    if (bSysCmd)
    while (lpSymEntry->stPrvEntry)
        lpSymEntry = lpSymEntry->stPrvEntry;

    // Insert an assignment arrow
    //   and skip over it
    *lpwData++ = UTF16_LEFTARROW;

    // Get the rank
    sscanfW (lpwData, L"%I64u", &aplRankRes);

    // Save the ptr to the rank
    lpwTemp = lpwData;

    // Skip past the rank
    lpwData = SkipPastCharW (lpwData, L' ');

    // Get the # WCHARs in the rank
    uLen = (APLU3264) (lpwData - lpwTemp);

    // Fill the rank with blanks
    for (uCnt = 0; uCnt < uLen; uCnt++)
        lpwTemp[uCnt] = L' ';

    // Calculate the NELM
    aplNELMRes = 1;

    // Loop through the shape vector
    for (uCnt = 0; uCnt < aplRankRes; uCnt++)
    {
        // Get the next shape value
        sscanfW (lpwData, L"%I64u", &aplTemp);

        // Accumulate into the NELM
        aplNELMRes *= aplTemp;

        // Skip past the shape value
        lpwData = SkipPastCharW (lpwData, L' ');
    } // End FOR

    // If the array is empty, ...
    if (IsEmpty (aplNELMRes))
    {
        // If the old value is a global, free it
        if (lpSymEntry->stFlags.Value && !lpSymEntry->stFlags.Imm)
            FreeResultGlobalVar (lpSymEntry->stData.stGlbData);
        // Set the new flags & value
        lpSymEntry->stFlags.Imm      =
        lpSymEntry->stFlags.Value    = FALSE;
        lpSymEntry->stData.stGlbData = MakePtrTypeGlb (hGlbZilde);
    } else
    {
        // If non-scalar, insert a reshape symbol after the shape vector
        if (!IsScalar (aplRankRes))
            lpwData[-1] = UTF16_RHO;

        // ***FIXME*** -- Ensure we change the global not local value
        // ***FIXME*** -- Ensure we don't change []DM/[]ES

        // Execute the statement starting with lpwName
        exitType =
          PrimFnMonUpTackJotCSPLParse (hWndEC,              // Edit Ctrl window handle
                                       lpMemPTD,            // Ptr to PerTabData global memory
                                       lpwName,             // Ptr to text of line to execute
                                       lstrlenW (lpwName),  // Length of the line to execute
                                       TRUE,                // TRUE iff we should act on errors
                                       FALSE,               // TRUE iff we're to skip the depth check
                                       NULL);               // Ptr to function token
        Assert (exitType EQ EXITTYPE_NODISPLAY
             || exitType EQ EXITTYPE_ERROR);
        if (exitType EQ EXITTYPE_ERROR)
            goto IMMEXEC_EXIT;
    } // End IF/ELSE

    // Set the name type for this new entry
    lpSymEntry->stFlags.stNameType = NAMETYPE_VAR;

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

INVALIDSYSNAME_EXIT:
    if (!lptkFunc)
    {
        // Format the error message
        wsprintfW (lpwszFormat,
                   L"INVALID " WS_UTF16_QUAD L"NAME in .atf file, lines %u-%u (origin-1), record starts with %.20s.",
                   uOldRecNo,
                   uRecNo,
                   lpwszTemp);
        // Display the error message
        AppendLine (lpwszFormat, FALSE, TRUE);
    } // End IF

    goto ERROR_EXIT;

STFULL_EXIT:
    if (!lptkFunc)
        AppendLine (ERRMSG_SYMBOL_TABLE_FULL APPEND_NAME, FALSE, TRUE);

    goto ERROR_EXIT;

IMMEXEC_EXIT:
    if (!lptkFunc)
    {
        // Display the leading part of the error message
        AppendLine (lpMemPTD->lpwszErrorMessage, FALSE, TRUE);

        // Ensure the name is properly terminated
        *lpwNameEnd = WC_EOS;

        // Format the trailing part of the error message
        wsprintfW (lpwszFormat,
                   L"  Error in .atf file, lines %u-%u (origin-1), type-1 variable:  %s.",
                   uOldRecNo,
                   uRecNo,
                   lpwName);
        // Display the trailing part of the error message
        AppendLine (lpwszFormat, TRUE, TRUE);
    } // End IF

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    // Restore the char at the name end
    if (lpwNameEnd)
        *lpwNameEnd = wch;

    return bRet;
} // End TransferInverseNum1_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CmdInCopyAndTranslate_EM
//
//  Copy the (perhaps several) records from the input to
//    temporary storage for later processsing as a single object.
//***************************************************************************

LPUCHAR CmdInCopyAndTranslate_EM
    (LPUCHAR    lpAtfView,                  // Ptr to incoming data
     LPDWORD    lpdwAtfFileSize,            // Ptr to file size
     LPWCHAR    lpwszTemp,                  // Ptr to output save area
     UINT       uMaxSize,                   // Size of output save area in bytes
     LPUINT     lpuLen,                     // Ptr to length of record in output save area (in WCHARs)
     FILETIME  *lpftCreation,               // Ptr to timestamp (if any)
     LPUINT     lpuRecNo,                   // Ptr to record count so far
     UBOOL      bIsEBCDIC)                  // TRUE iff the orignial .atf file was in EBCDIC format

{
    SYSTEMTIME systemTime;                  // Save area for timestamp
    UBOOL      bContinue = TRUE;            // TRUE iff the WHILE loop should continue
    UINT       uCnt,                        // Loop counter
               uOldRecNo;                   // Starting record # in file (for range display)
    APLUINT    uLen;                        // Actual length of record
    LPWCHAR    lpwTranslate;                // Ptr to either APL2_ASCIItoNARS or APL2_EBCDICtoNARS

    // Initialize the FILETIME struc
    lpftCreation->dwLowDateTime =
    lpftCreation->dwHighDateTime = 0;

    // Initialize the total length
    *lpuLen = 0;

    // Save the starting record #
    uOldRecNo = 1 + *lpuRecNo;

    // Set the translate table
    lpwTranslate = (bIsEBCDIC ? APL2_EBCDICtoNARS : APL2_ASCIItoNARS);

    // Loop through the records
    while (bContinue && *lpdwAtfFileSize)
    {
        // Split cases based upon the first char in the record
        switch (lpAtfView[0])
        {
            case AC_STAR:
                // This record is a comment

                // Check for a timestamp
                if (lpAtfView[1] EQ AC_LEFTPAREN)
                {
                    sscanf (&lpAtfView[2],
                             "%hd %hd %hd %hd %hd %hd %hd",
                            &systemTime.wYear,
                            &systemTime.wMonth,
                            &systemTime.wDay,
                            &systemTime.wHour,
                            &systemTime.wMinute,
                            &systemTime.wSecond,
                            &systemTime.wMilliseconds);
                    // Convert it to a format we use to store timestamps
                    SystemTimeToFileTime (&systemTime, lpftCreation);
                } // End IF

                // Skip to the next record
                lpAtfView += REC_LEN;
                (*lpuRecNo)++;
                *lpdwAtfFileSize -= REC_LEN;

                break;

            case AC_CR:
                // Skip over CR or CRLF
                if (lpAtfView[1] EQ AC_LF)
                {
                    lpAtfView += 2;
                    *lpdwAtfFileSize -= 2;
                } else
                {
                    lpAtfView++;
                    *lpdwAtfFileSize--;
                } // End IF/ELSE

                break;

            case AC_X:
            case AC_BLANK:
                // Copy and widen the input to the output save area
                //   and translate chars from APL2 to NARS
                for (uCnt = 0; uCnt < INT_LEN; uCnt++)
                    lpwszTemp[uCnt] = lpwTranslate[lpAtfView[uCnt + 1]];

                //Ensure properly terminated
                lpwszTemp[INT_LEN] = WC_EOS;

                // If this is the last record, ...
                if (lpAtfView[0] EQ AC_X)
                {
                    LPWCHAR lpwTmp;

                    // Point to the next char in output save area
                    lpwTmp = &lpwszTemp[INT_LEN];

                    // Get the actual length
                    uLen = lpwTmp - lpwszTemp;

                    // Mark as last record
                    bContinue = FALSE;
                } else
                    uLen = INT_LEN;

                // Count it into the length
                *lpuLen   += (UINT) uLen;
                lpwszTemp += (UINT) uLen;

                // Skip to the next record
                lpAtfView += REC_LEN;
                (*lpuRecNo)++;
                *lpdwAtfFileSize -= REC_LEN;

                // If this is the last record, check for trailing CR/LF
                if (!bContinue && *lpdwAtfFileSize && lpAtfView[0] EQ AC_CR)
                {
                    // Skip over CR or CRLF
                    if (lpAtfView[1] EQ AC_LF)
                    {
                        lpAtfView += 2;
                        *lpdwAtfFileSize -= 2;
                    } else
                    {
                        lpAtfView++;
                        *lpdwAtfFileSize--;
                    } // End IF/ELSE
                } // End IF

                break;

            default:
            {
                WCHAR wszTemp[80];

                wsprintfW (wszTemp,
                           L"Invalid record in .atf file, lines %u-%u (origin-1) starting with %.20s.",
                           uOldRecNo,
                           1 + *lpuRecNo,
                           lpAtfView);

                // Display the error message
                AppendLine (wszTemp, FALSE, TRUE);

                return NULL;
            } // End default
        } // End SWITCH
    } // End WHILE

    return lpAtfView;
} // End CmdInCopyAndTranslate_EM


//***************************************************************************
//  $CmdInEBCDIC2ASCII
//
//  Translate the file contents from EBCDIC to ASCII
//***************************************************************************

void CmdInEBCDIC2ASCII
    (LPUCHAR lpAtfView,                     // Ptr to incoming data
     DWORD   dwAtfFileSize)                 // Length of incoming data

{
static unsigned char EBCDICtoASCII[256] =
{
//     x0      x1      x2      x3      x4      x5      x6      x7      x8      x9      xA      xB      xC      xD      xE      xF
    '\x00', '\x01', '\x02', '\x03', '\x05', '\x09', '\x15', '\x18', '\xDF', '\x19', '\xB0', '\x0B', '\x0C', '\x1F', '\x0E', '\x0F', // 0x
    '\x10', '\x11', '\x12', '\x13', '\x1A', '\x0D', '\x08', '\xDD', '\xDB', '\xDE', '\xB3', '\xBF', '\xDA', '\x1D', '\xC0', '\xD9', // 1x
    '\xB1', '\xA0', '\x1C', '\xA2', '\xA3', '\x0A', '\x17', '\x1B', '\xDC', '\xA8', '\xB2', '\xAD', '\xC5', '\xC4', '\x06', '\x07', // 2x
    '\xBA', '\xB9', '\x16', '\xBB', '\xBC', '\x1E', '\xAB', '\x04', '\xC9', '\xCA', '\xCE', '\xC2', '\x14', '\xC3', '\xC1', '\xB4', // 3x
    '\x20', '\x7F', '\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87', '\x9B', '\x2E', '\x3C', '\x28', '\x2B', '\xCB', // 4x
    '\x26', '\x88', '\x89', '\x8A', '\x8B', '\x8C', '\x8D', '\x8E', '\x8F', '\x93', '\xCC', '\x24', '\x2A', '\x29', '\x3B', '\xAA', // 5x
    '\x2D', '\x2F', '\x94', '\x95', '\x96', '\x97', '\x99', '\x9A', '\x9C', '\x9E', '\xA4', '\x2C', '\x25', '\x5F', '\x3E', '\x3F', // 6x
    '\xD8', '\x5E', '\xFE', '\xD5', '\xD0', '\xD1', '\xD6', '\xD7', '\xEB', '\x60', '\x3A', '\x23', '\x40', '\x27', '\x3D', '\x22', // 7x
    '\x7E', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', '\x68', '\x69', '\xC6', '\xC7', '\xF3', '\xA9', '\xBE', '\xB8', // 8x
    '\x90', '\x6A', '\x6B', '\x6C', '\x6D', '\x6E', '\x6F', '\x70', '\x71', '\x72', '\xE3', '\xE2', '\xCD', '\xEA', '\xA5', '\xBD', // 9x
    '\xFD', '\xA1', '\x73', '\x74', '\x75', '\x76', '\x77', '\x78', '\x79', '\x7A', '\xEF', '\xAC', '\x9D', '\x5B', '\xF2', '\xF8', // Ax
    '\xE0', '\xEE', '\xEC', '\xE6', '\xF9', '\xA7', '\xF5', '\x5C', '\xF6', '\xC8', '\xB7', '\xB6', '\x98', '\x5D', '\xF4', '\x7C', // Bx
    '\x7B', '\x41', '\x42', '\x43', '\x44', '\x45', '\x46', '\x47', '\x48', '\x49', '\xE5', '\xE7', '\xD3', '\xE8', '\xD4', '\xED', // Cx
    '\x7D', '\x4A', '\x4B', '\x4C', '\x4D', '\x4E', '\x4F', '\x50', '\x51', '\x52', '\x9F', '\x21', '\xFC', '\xFB', '\x91', '\xE4', // Dx
    '\xA6', '\xCF', '\x53', '\x54', '\x55', '\x56', '\x57', '\x58', '\x59', '\x5A', '\xF0', '\xF1', '\xD2', '\xE9', '\x92', '\xAE', // Ex
    '\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37', '\x38', '\x39', '\xE1', '\xFA', '\xF7', '\xB5', '\xAF', '\xFF', // Fx
};

    while (dwAtfFileSize--)
        *lpAtfView++ = EBCDICtoASCII[*lpAtfView];
} // End CmdInEBCDIC2ASCII


//***************************************************************************
//  End of File: sc_in.c
//***************************************************************************
