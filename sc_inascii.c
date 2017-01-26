//***************************************************************************
//  NARS2000 -- System Command:  )INASCII
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
#include <stdio.h>
#include <time.h>
#include "headers.h"


#define LEADING_TEXT        L"FUNCTIONS COPIED:"


//***************************************************************************
//  $CmdInAscii_EM
//
//  Execute the system command:  )INASCII D:\path\to\filename.ext
//    with support for wildcards
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CmdInAscii_EM"
#else
#define APPEND_NAME
#endif

UBOOL CmdInAscii_EM
    (LPWCHAR lpwszTail)                     // Ptr to command line tail

{
    UBOOL            bRet = FALSE,          // TRUE iff the result is valid
                     bLast;                 // TRUE iff the last call to InAsciiFile_EM was successful
    WIN32_FIND_DATAW findData = {0};        // Struct used with FindXXXX functions
    HANDLE           hFile;                 // File handle
    WCHAR            wszDrive[_MAX_DRIVE],  // Drive info from command line
                     wszDir  [_MAX_DIR];    // Dir   ...
    UINT             uFcnCount;             // # functions converted

    // Ensure there's something on the command line
    if (lpwszTail[0] EQ WC_EOS)
        goto INCORRECT_COMMAND_EXIT;

    // Initialize the struc
    findData.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;

    // Extract the Drive and Path from the command line
    _wsplitpath (lpwszTail, wszDrive, wszDir, NULL, NULL);

    // Initialize success string
    strcpyW (lpwszGlbTemp, LEADING_TEXT);

    // Call on first file
    hFile = FindFirstFileW (lpwszTail, &findData);

    if (hFile NE INVALID_HANDLE_VALUE)
    {
        // Initialize # functions converted
        uFcnCount = 0;

        do
        {
            // Process the file
            bRet |= bLast =
              InAsciiFile_EM (wszDrive,                     // Drive info from command line
                              wszDir,                       // Dir   ...
                              findData.cFileName,           // Filename.ext
                              lpwszGlbTemp,                 // Ptr to string to which function named is appended
                              lstrlenW (LEADING_TEXT));     // Length of leading text in success string
            // Count in another conversion
            uFcnCount++;
        } while  (FindNextFileW (hFile, &findData));

        // Close it up to free resources
        FindClose (hFile);

        // Display the success string if there were
        //   functions and at least one was successful
        if (uFcnCount && bLast)
            ReplaceLastLineCRPmt (lpwszGlbTemp);
    } else
        goto FILENOTFOUND_EXIT;

    goto NORMAL_EXIT;

FILENOTFOUND_EXIT:
    ReplaceLastLineCRPmt (ERRMSG_FILE_NOT_FOUND APPEND_NAME);

    goto ERROR_EXIT;

INCORRECT_COMMAND_EXIT:
    ReplaceLastLineCRPmt (ERRMSG_INCORRECT_COMMAND APPEND_NAME);

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    return bRet;
} // End CmdInAscii_EM
#undef  APPEND_NAME


//***************************************************************************
//  $InAsciiFile_EM
//
//  Process a single ASCII2APL file
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- InAsciiFile_EM"
#else
#define APPEND_NAME
#endif

UBOOL InAsciiFile_EM
    (LPWCHAR wszDrive,                              // Drive info from command line
     LPWCHAR wszDir,                                // Dir   ...
     LPWCHAR lpwszFileName,                         // Filename.ext
     LPWCHAR lpwszTemp,                             // Ptr to string to which function named is appended
     UINT    uLeadingText)                          // Length of leading text in success string

{
    UBOOL               bRet = FALSE;               // TRUE iff the result is valid
    HANDLE              hA2AFile = NULL,            // Handle for ASCII2APL file
                        hA2AMap = NULL,             // Handle from CreateFileMappingW
                        hA2AView = NULL;            // Handle from MapViewOfFile
    DWORD               dwA2AFileSize;              // Byte size of ASCII2APL file
    LPWCHAR             lpwA2AView = NULL,          // Ptr to global memory
                        lpwLoop;                    // Temporary ptr
    UINT                uLen;                       // Length of output save area in WCHARs
    WCHAR               wszDPFE[1024],              // Temporary storage for DPFE
                        wszTemp[1024];              // Temporary storage for message strings
#define wszTempLen      countof (wszTemp)
    SF_FCNS             SF_Fcns = {0};              // Common struc for SaveFunctionCom
    AA_PARAMS           AA_Params = {0};            // Local struc for  ...

    // Put the Drive, Dir, and Filename.ext info together
    _wmakepath (wszDPFE, wszDrive, wszDir, lpwszFileName, NULL);

    // See if we can open this file
    hA2AFile =
      CreateFileW (wszDPFE,                         // Pointer to Drive, Path, Filename, Ext
                   0
                 | GENERIC_READ                     // Access (read-only) mode
                   ,
                   FILE_SHARE_READ
                 | FILE_SHARE_WRITE,                // Share mode
                   NULL,                            // Pointer to security descriptor
                   OPEN_EXISTING,                   // How to create
                   0                                // File attributes
                 | FILE_ATTRIBUTE_NORMAL,
                   NULL);                           // Handle to file with attributes to copy
    if (hA2AFile EQ INVALID_HANDLE_VALUE)
        goto FILENOTFOUND_EXIT;

    // Get the file size (it always fits in a DWORD)
    dwA2AFileSize = GetFileSize (hA2AFile, NULL);

    // Map the file
    hA2AMap =
      CreateFileMappingW (hA2AFile,                 // Handle to file to map
                          NULL,                     // Optional security attributes
                          PAGE_READONLY,            // Protection for mapping object
                          0,                        // High-order 32 bits of object size
                          dwA2AFileSize,            // Low-order 32 bits of object size
                          NULL);                    // Name of file-mapping object
    if (hA2AMap EQ NULL)
    {
        strcpyW (wszTemp, L"Unable to create file mapping:  ");
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
    hA2AView =
      MapViewOfFile (hA2AMap,                       // File-mapping object to map into address space
                     FILE_MAP_READ,                 // Access mode
                     0,                             // High-order 32 bits of file offset
                     0,                             // Low-order 32 bits of file offset
                     dwA2AFileSize);                // Number of bytes to map
    if (hA2AView EQ NULL)
    {
        strcpyW (wszTemp, L"Unable to map view of file:  ");
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

    // Allocate storage for the file contents so as
    //   to convert it from chars to WCHARs
    lpwA2AView =
      MyVirtualAlloc (NULL,                             // Any address
                      dwA2AFileSize * sizeof (APLCHAR), // Maximum size
                      MEM_COMMIT | MEM_TOP_DOWN,
                      PAGE_READWRITE);
    if (lpwA2AView EQ NULL)
        goto WSFULL_EXIT;
    // Copy data from char to WCHAR
    //   converting WIF-specific chars as we go
    for (uLen = 0; uLen < dwA2AFileSize; uLen++)
#define lpView      ((LPUCHAR) hA2AView)
    switch (lpView[uLen])
    {
        case '#':
            if (uLen && lpView[uLen - 1] NE L'{')
                lpwA2AView[uLen] = UTF16_QUAD;
            else
                lpwA2AView[uLen] = lpView[uLen];
            break;

        case '@':
            if (uLen && lpView[uLen - 1] NE L'{')
                lpwA2AView[uLen] = UTF16_LAMP;
            else
                lpwA2AView[uLen] = lpView[uLen];
            break;

        case 0x1A:
            lpwA2AView[uLen] = WC_CR;

            break;

        default:
            lpwA2AView[uLen] = lpView[uLen];

            break;
    } // End FOR/SWITCH
#undef  lpView

    // Count the # lines (including the header),
    //   replace the CR,LF sequence with a zero, and
    //   replace text with  UTF16_xxx equivalents.
    for (lpwLoop = lpwA2AView, uLen = dwA2AFileSize, AA_Params.NumLines = 0;
         uLen;
         )
    {
        LPWCHAR lpw, lpw2;

        lpw = strpbrkW (lpwLoop, WS_CRLF);
        if (lpw)
        {
            // Account for the line length
            uLen -= (UINT) (lpw - lpwLoop);

            // Zap the CR/LF
            *lpw++ = WC_EOS; uLen --;

            // Strip off trailing CR/LFs
            while (uLen && (*lpw EQ WC_CR || *lpw EQ WC_LF))
            {
                *lpw++ = WC_EOS; uLen--;
            } // End WHILE

            // Convert {name}s to UTF16_xxx equivalents
            (void) ConvertNameInPlace (lpwLoop);

            // Point to the end of the converted {name}s
            lpw2 = &lpwLoop[lstrlenW (lpwLoop)];

            // Zero the extra trailing chars
            ZeroMemory (lpw2, (lpw - lpw2) * sizeof (APLCHAR));

            // Count in another line
            AA_Params.NumLines++;

            // Process next line
            lpwLoop = lpw;
        } else
            // We should never get here
            DbgStop ();
    } // End FOR

    // Check for dynamic functions
    lpwLoop = strpbrkW (lpwA2AView, WS_UTF16_LAMP L"{");
    if (lpwLoop && lpwLoop[0] EQ L'{')
        goto DYNAMIC_EXIT;

    // Fill in common values
////SF_Fcns.bRet      =                             // Filled in by SaveFunctionCom
////SF_Fcns.uErrLine  =                             // ...
////SF_Fcns.lpSymName =                             // ...
////SF_Fcns.lptkFunc  =                             // Ptr to function token (none)

    // Setup

    // Fill in common values
    SF_Fcns.bDisplayErr     = FALSE;                // DO NOT Display Errors
    SF_Fcns.SF_LineLen      = SF_LineLenAA;         // Ptr to line length function
    SF_Fcns.SF_ReadLine     = SF_ReadLineAA;        // Ptr to read line function
    SF_Fcns.SF_IsLineCont   = SF_IsLineContAA;      // Ptr to Is Line Continued function
    SF_Fcns.SF_NumPhyLines  = SF_NumPhyLinesAA;     // Ptr to get # physical lines function
    SF_Fcns.SF_NumLogLines  = SF_NumLogLinesAA;     // Ptr to get # logical  ...
    SF_Fcns.SF_CreationTime = SF_CreationTimeAA;    // Ptr to get function creation time
    SF_Fcns.SF_LastModTime  = SF_LastModTimeAA;     // Ptr to get function last modification time
    SF_Fcns.SF_UndoBuffer   = SF_UndoBufferAA;      // Ptr to get function Undo Buffer global memory handle
    SF_Fcns.sfTypes         = SFTYPES_AA;           // Caller type

    // Fill in ASCII2APL-specific values
    AA_Params.lpwStart = lpwA2AView;

    // Save ptr to local parameters
    SF_Fcns.LclParams = &AA_Params;

    // Call common routine
    if (SaveFunctionCom (NULL, &SF_Fcns))
    {
        HGLOBAL   htGlbName;        // Function name global memory handle
        LPAPLCHAR lpMemName;        // Ptr to function name global memory
        UINT      uNameLen;         // Length of the function name

        // The function fix succeeded -- append the function name
        //   to the list of functions copied

        // Get the function name global memory handle
        htGlbName = SF_Fcns.lpSymName->stHshEntry->htGlbName;

        // Lock the memory to get a ptr to it
        lpMemName = MyGlobalLockWsz (htGlbName);

        // Get the name length plus the leading blanks
        uNameLen = 2 + lstrlenW (lpMemName);

        // Get the current length
        uLen = lstrlenW (lpwszTemp);

        // Check for overflow with []PW
        if ((uLen + uNameLen) > GetQuadPW ())
        {
            // Display the text so far
            ReplaceLastLineCRPmt (lpwszTemp);

            // Fill with leading blanks for alignment
            FillMemoryW (lpwszTemp, uLeadingText, L' ');

            // Ensure properly terminated
            lpwszTemp[uLeadingText] = WC_EOS;
        } // End IF

        // Append the separator and function name
        strcatW (lpwszTemp, L"  ");
        strcatW (lpwszTemp, lpMemName);

        // We no longer need this ptr
        MyGlobalUnlock (htGlbName); lpMemName = NULL;
    } else
    {
        // The function fix failed.

        // Display the text so far
        ReplaceLastLineCRPmt (lpwszTemp);

        // Initialize success string
        strcpyW (lpwszTemp, LEADING_TEXT);

        // If the error line # is NEG1U, there is an error message, so display it
        if (SF_Fcns.uErrLine EQ NEG1U)
            MySprintfW (wszTemp,
                        sizeof (wszTemp),
                       L"FUNCTION IN <%s> NOT DEFINED:  %s",
                        lpwszFileName,
                        SF_Fcns.wszErrMsg);
        else
            // Otherwise, display the error line # as an integer scalar (origin-sensitive)
            MySprintfW (wszTemp,
                        sizeof (wszTemp),
                       L"FUNCTION IN <%s> NOT DEFINED:  ERROR ON LINE %u",
                        lpwszFileName,
                        SF_Fcns.uErrLine);
        // Display the line
        ReplaceLastLineCRPmt (wszTemp);

        goto ERROR_EXIT;
    } // End IF/ELSE
DYNAMIC_EXIT:
    // Mark as sucessful
    bRet = TRUE;

    goto NORMAL_EXIT;

FILENOTFOUND_EXIT:
    ReplaceLastLineCRPmt (ERRMSG_FILE_NOT_FOUND APPEND_NAME);

    goto ERROR_EXIT;

WSFULL_EXIT:
    ReplaceLastLineCRPmt (ERRMSG_WS_FULL APPEND_NAME);

    goto ERROR_EXIT;

ERROR_EXIT:
    // Mark as unsuccessful
    bRet = FALSE;
NORMAL_EXIT:
    if (lpwA2AView)
    {
        MyVirtualFree (lpwA2AView, 0, MEM_RELEASE); lpwA2AView = NULL;
    } // End IF

    if (hA2AView)
    {
        UnmapViewOfFile (hA2AView); hA2AView = NULL;
    } // End IF

    if (hA2AMap)
    {
        CloseHandle (hA2AMap); hA2AMap = NULL;
    } // End IF

    if (hA2AFile)
    {
        CloseHandle (hA2AFile); hA2AFile = NULL;
    } // End IF

    return bRet;
} // End InAsciiFile_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: sc_inascii.c
//***************************************************************************
