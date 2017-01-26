//***************************************************************************
//  NARS2000 -- System Function -- Quad NFNS
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
#include "accctrl.h"
#include "aclapi.h"
#include "sddl.h"


typedef DWORD (WINAPI *GETFPN) (HANDLE hFile,
                                LPWSTR lpszFilePath,
                                DWORD  cchFilePath,
                                DWORD  dwFlags);

#define BUFLEN          1024
#define CHAR8           BYTE
#define CHAR16          WORD
#define CHAR32          DWORD
#define FLT64           double

#define FLT64_MIN       DBL_MIN
#define FLT64_MAX       DBL_MAX

typedef union tagNFNS_BUFFER
{
    CHAR8    Tchar8 [8 * BUFLEN];   // Temp buffer
    INT8     Tint8  [8 * BUFLEN];   // ...
    CHAR16   Tchar16[4 * BUFLEN];   // ...
    INT16    Tint16 [4 * BUFLEN];   // ...
    CHAR32   Tchar32[2 * BUFLEN];   // ...
    INT32    Tint32 [2 * BUFLEN];   // ...
    INT64    Tint64 [1 * BUFLEN];   // ...
    APLFLOAT Tflt64 [1 * BUFLEN];   // ...
} NFNS_BUFFER, *LPNFNS_BUFFER;


ARRAY_TYPES propArrType[NPROP_LENGTH]
 = {
    ARRAY_NESTED ,      // 0 = DPFE
    ARRAY_INT    ,      // 1 = Type (see NFILETYPES)
    ARRAY_INT    ,      // 2 = Size in bytes
    ARRAY_NESTED ,      // 3 = Last modification time
    ARRAY_NESTED ,      // 4 = Owner user ID
    ARRAY_NESTED ,      // 5 = Owner name
    ARRAY_INT    ,      // 6 = Hidden or not
    ARRAY_NESTED ,      // 7 = Target of symbolic link
   };


//***************************************************************************
//  $SysFnNAPPEND_EM_YY
//
//  System function:  []NAPPEND -- Append data to an open native file
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnNAPPEND_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnNAPPEND_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonNAPPEND_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydNAPPEND_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnNAPPEND_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonNAPPEND_EM_YY
//
//  Monadic []NAPPEND -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonNAPPEND_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonNAPPEND_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End SysFnMonNAPPEND_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydNAPPEND_EM_YY
//
//  Dyadic []NAPPEND -- Append data to an open native file
//
//  Data []NAPPEND tn [DiskConv]
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydNAPPEND_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydNAPPEND_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeLft,           // Left arg storage type
                      aplTypeRht;           // Right ...
    APLNELM           aplNELMLft,           // Left arg NELM
                      aplNELMRht;           // Right ...
    APLRANK           aplRankLft,           // Left arg Rank
                      aplRankRht;           // Right ...
    APLLONGEST        aplLongestLft,        // Left arg longest if immediate
                      aplLongestRht;        // Right ...
    HGLOBAL           hGlbLft = NULL,       // Left arg global memory handle
                      hGlbRht = NULL;       // Right ...
    LPVARARRAY_HEADER lpMemHdrLft = NULL,   // Ptr to left arg header
                      lpMemHdrRht = NULL;   // ...    right    ...
    LPAPLCHAR         lpMemLft;             // Ptr to left arg global memory
    LPVOID            lpMemRht;             // Ptr to right ...
    DR_VAL            DiskConv;             // Disk format
    APLINT            TieNum,               // File tie number
                      aplFileOff,           // Starting offset of append
                      aplFileOut;           // # bytes written to disk
    UINT              uTie;                 // Offset of matching tie number entry
    LPPERTABDATA      lpMemPTD;             // Ptr to PerTabData global memory
    LPNFNSHDR         lpNfnsHdr = NULL;     // Ptr to NFNSHDR global memory
    LPNFNSDATA        lpNfnsMem;            // Ptr to aNfnsData
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to result
    OVERLAPPED        overLapped = {0};     // OVERLAPPED struc

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RIGHT RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Check for RIGHT LENGTH ERROR
    if (1 > aplNELMRht
     ||     aplNELMRht > 2)
        goto RIGHT_LENGTH_EXIT;

    // Check for LEFT DOMAIN ERROR
    if (!IsSimpleNH (aplTypeLft))
        goto LEFT_DOMAIN_EXIT;

    // Check for RIGHT DOMAIN ERROR
    if (!(IsNumeric (aplTypeRht)
       || IsNested  (aplTypeRht)))
        goto RIGHT_DOMAIN_EXIT;

    // Lock the memory to get a ptr to it
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // If the left arg is a global, ...
    if (hGlbLft NE NULL)
        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
    else
        lpMemLft = (LPAPLCHAR) &aplLongestLft;

    // If the right arg is a global, ...
    if (hGlbRht NE NULL)
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    else
        lpMemRht = &aplLongestRht;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Validate the tie number
    if (!NfnsArgTieNum (aplTypeRht, lpMemRht, aplNELMRht, 0, &TieNum, lptkFunc, lpMemPTD)
     ||  TieNum >= 0)
        goto RIGHT_DOMAIN_EXIT;

    // Check for duplicate tie number
    switch (IsDuplicateTieNum_EM (TieNum, lptkFunc, lpMemPTD, &uTie))
    {
        case -1:
            goto ERROR_EXIT;

        case FALSE:
            goto RIGHT_DOMAIN_EXIT;

        case TRUE:
            break;

        defstop
            break;
    } // End SWITCH

    // Lock the memory to get a ptr to it
    lpNfnsHdr = InitLockNfns (lptkFunc, lpMemPTD);

    // Point to the matching tie number entry
    lpNfnsMem = &lpNfnsHdr->aNfnsData[uTie];

    // Validate the conversion
    if (!NfnsArgConv   (aplTypeRht, lpMemRht, aplNELMRht, 1, TRUE, &DiskConv, lpNfnsMem->DiskConv, NULL, 0, NULL))
        goto RIGHT_DOMAIN_EXIT;

    // Initialize the OVERLAPPED struc with the file offset (-1 for append)
    overLapped.Offset     =
    overLapped.OffsetHigh = -1;

    // Create an event
    overLapped.hEvent =
      CreateEvent (NULL,                        // Ptr to security attributes (may be NULL)
                   TRUE,                        // TRUE iff manual-reset timer
                   FALSE,                       // TRUE if initial state is signalled
                   NULL);                       // Ptr to event name (may be NULL)
    if (overLapped.hEvent EQ NULL)
        goto EVENT_EXIT;

    // Set the file pointer to the end-of-file for appending
    aplFileOff = 0;
    LOAPLINT (aplFileOff) =
      SetFilePointer (lpNfnsMem->hFile,
                      LOAPLINT (aplFileOff),
                     &HIAPLINT (aplFileOff),
                      FILE_END);
    // Translate the data to DiskConv format and write it out
    if (!NfnsWriteData_EM (lpNfnsMem->hFile,        // File handle
                           aplTypeLft,              // Left arg storage type
                           aplNELMLft,              // Left arg NELM
                           lpMemLft,                // Ptr to left arg data
                           DiskConv,                // Disk conversion value (DR_xxx)
                          &aplFileOut,              // # bytes written to disk
                          &overLapped,              // Ptr to OVERLAPPED struc
                           lpMemPTD,                // Ptr to PerTabData global memory
                           lptkFunc))               // Ptr to function token
        goto ERROR_EXIT;

    // Skip past the bytes written out
    aplFileOff += aplFileOut;

    // Allocate a new YYRes;
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
    lpYYRes->tkToken.tkFlags.NoDisplay = TRUE;
    lpYYRes->tkToken.tkData.tkInteger  = aplFileOff;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

EVENT_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LIMIT_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (overLapped.hEvent)
    {
        // We no longer need this resource
        CloseHandle (overLapped.hEvent); overLapped.hEvent = NULL;
    } // End IF

    if (hGlbLft NE NULL && lpMemHdrLft NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;
    } // End IF

    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    if (lpNfnsHdr NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lpMemPTD->hGlbNfns); lpNfnsHdr = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnDydNAPPEND_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnNCREATE_EM_YY
//
//  System function:  []NCREATE -- Create and open a native file
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnNCREATE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnNCREATE_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonNCREATE_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydNCREATE_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnNCREATE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonNCREATE_EM_YY
//
//  Monadic []NCREATE -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonNCREATE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonNCREATE_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End SysFnMonNCREATE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydNCREATE_EM_YY
//
//  Dyadic []NCREATE -- Create and open a native file
//
//  FileName []NCREATE tn [Mode [DiskConv | (DiskConv WsConv)]]
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydNCREATE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydNCREATE_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Call common code
    return SysFnCreateTie_EM_YY (TRUE, lptkLftArg, FALSE, lptkFunc, lptkRhtArg, lptkAxis);
} // End SysFnDydNCREATE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnCreateTie_EM_YY
//
//  Common function to []NCREATE & []NTIE
//
//  FileName []NCREATE tn [Mode [DiskConv | (DiskConv WSConv)]]
//  FileName []NTIE    tn [Mode [DiskConv | (DiskConv WSConv)]]
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnCreateTie_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnCreateTie_EM_YY
    (UBOOL   bCreate,               // TRUE iff []NCREATE
     LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     UBOOL   bSpecial,              // TRUE iff the file might be special (Directory, Reparse point, ...)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeLft,           // Left arg storage type
                      aplTypeRht,           // Right ...
                      aplTypeWs;            // Workspace ...
    APLNELM           aplNELMLft,           // Left arg NELM
                      aplNELMRht;           // Right ...
    APLRANK           aplRankLft,           // Left arg rank
                      aplRankRht;           // Right ...
    HGLOBAL           hGlbLft = NULL,       // Left arg global memory handle
                      hGlbRht = NULL;       // Right ...
    LPVARARRAY_HEADER lpMemHdrLft = NULL,   // Ptr to left arg header
                      lpMemHdrRht = NULL;   // ...    right    ...
    LPAPLCHAR         lpMemLft;             // Ptr to left arg global memory
    LPVOID            lpMemRht;             // ...    right ...
    APLINT            TieNum;               // File tie number
    APLLONGEST        aplLongestLft,        // Left arg immediate value
                      aplLongestRht;        // Right ...
    DWORD             AccessMode,           // Access mode
                      ShareMode;            // Share mode
    DR_VAL            DiskConv,             // Disk format
                      WsConv;               // Workspace format
    HANDLE            hFile;                // File handle
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    LPPERTABDATA      lpMemPTD;             // Ptr to PerTabData global memory
    UINT              uFlags;               // File flags & attributes

    /* The left arg is a char scalar or vector naming the file.

       The right arg may be a scalar or one-, two-, or three-item vector.

       The first item of the right arg is tie number -- if it's zero, generate an
         automatic tie number, otherwise it must be negative (use this as the tie number).

       If present, the second item of the right arg is the mode in which the file
       is to be opened, e.g., read and/or write access.

       If present, the third item of the right arg is the conversion code(s) used
       by subsequent operations such as read, replace, and append.
    */

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for LEFT RANK ERROR
    if (IsMultiRank (aplRankLft))
        goto LEFT_RANK_EXIT;

    // Check for RIGHT RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Check for RIGHT LENGTH ERROR
    if (1 > aplNELMRht
     ||     aplNELMRht > 3)
        goto RIGHT_LENGTH_EXIT;

    // Check for LEFT DOMAIN ERROR
    if (!IsSimpleChar (aplTypeLft))
        goto LEFT_DOMAIN_EXIT;

    // Check for RIGHT DOMAIN ERROR
    if (!(IsNumeric (aplTypeRht)
       || IsNested  (aplTypeRht)))
        goto RIGHT_DOMAIN_EXIT;

    // Lock the memory to get a ptr to it
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // If the left arg is a global, ...
    if (hGlbLft NE NULL)
        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
    else
        lpMemLft = (LPAPLCHAR) &aplLongestLft;

    // If the right arg is a global, ...
    if (hGlbRht NE NULL)
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    else
        lpMemRht = &aplLongestRht;

    // Validate the tie number
    if (!NfnsArgTieNum (aplTypeRht, lpMemRht, aplNELMRht, 0, &TieNum, lptkFunc, lpMemPTD)
     ||  TieNum >= 0)
        goto RIGHT_DOMAIN_EXIT;

    // Check for duplicate tie number
    switch (IsDuplicateTieNum_EM (TieNum, lptkFunc, lpMemPTD, NULL))
    {
        case -1:
            goto ERROR_EXIT;

        case FALSE:
            break;

        case TRUE:
            goto RIGHT_DOMAIN_EXIT;

        defstop
            break;
    } // End SWITCH

    // Validate the mode
    if (!NfnsArgMode   (aplTypeRht, lpMemRht, aplNELMRht, 1, &AccessMode, OF_READWRITE, &ShareMode, OF_SHARE_COMPAT))
        goto RIGHT_DOMAIN_EXIT;

    // Validate the conversion
    if (!NfnsArgConv   (aplTypeRht, lpMemRht, aplNELMRht, 2, FALSE, &DiskConv, DR_CHAR8, &WsConv, DR_CHAR16, &aplTypeWs))
        goto RIGHT_DOMAIN_EXIT;
    // If this file might be special, ...
    if (bSpecial)
    {
        AccessMode = GENERIC_READ;
        ShareMode  = FILE_SHARE_READ;
        uFlags = 0
               | FILE_FLAG_OPEN_REPARSE_POINT
               | FILE_FLAG_BACKUP_SEMANTICS;
    } else
    {
        uFlags = 0
               | FILE_ATTRIBUTE_NORMAL
               | FILE_FLAG_OVERLAPPED;
    } // End IF/ELSE

    // Create the file
    hFile =
      CreateFileW (lpMemLft,                // Ptr to Drive, path, filename
                   AccessMode,              // Desired access
                   ShareMode,               // Shared access
                   NULL,                    // Security attributes
                   bCreate ? CREATE_ALWAYS
                           : OPEN_EXISTING, // Create/open flags
                   uFlags,                  // File attributes & flags
                   NULL);                   // Template file
    // If it failed, ...
    if (hFile EQ INVALID_HANDLE_VALUE)
    {
        // Format a system error message
        SysErrMsg_EM (GetLastError (), lptkLftArg);

        goto ERROR_EXIT;
    } else
    {
        // Save the file handle, etc. in local storage
        if (!NfnsSaveData_EM (hFile, hGlbLft, DiskConv, WsConv, aplTypeWs, TieNum, lptkFunc, lpMemPTD))
            goto ERROR_EXIT;

        // Allocate a new YYRes;
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
        lpYYRes->tkToken.tkFlags.NoDisplay = (aplLongestRht NE 0);  // Shy iff specifed tie number is explicit (non-zero)
        lpYYRes->tkToken.tkData.tkInteger  = TieNum;
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

        goto NORMAL_EXIT;
    } // End IF/ELSE

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (hGlbLft NE NULL && lpMemHdrLft NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;
    } // End IF

    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnCreateTie_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnNERASE_EM_YY
//
//  System function:  []NERASE -- Erase an open native file
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnNERASE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnNERASE_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonNERASE_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydNERASE_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnNERASE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonNERASE_EM_YY
//
//  Monadic []NERASE -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonNERASE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonNERASE_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End SysFnMonNERASE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydNERASE_EM_YY
//
//  Dyadic []NERASE -- Erase an open native file
//
//  FileName []NERASE tn
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydNERASE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydNERASE_EM_YY
    (LPTOKEN lptkLftArg,                    // Ptr to left arg token
     LPTOKEN lptkFunc,                      // Ptr to function token
     LPTOKEN lptkRhtArg,                    // Ptr to right arg token
     LPTOKEN lptkAxis)                      // Ptr to axis token (may be NULL)

{
    return
      NfnsEraseRenameResize_EM (lptkLftArg,
                                lptkFunc,
                                lptkRhtArg,
                                lptkAxis,
                                COM_NERASE,
                                NfnsErase);
} // End SysFnDydNERASE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnNINFO_EM_YY
//
//  System function:  []NINFO -- Return file information
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnNINFO_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnNINFO_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE                   aplTypeLft,              // Left arg storage type
                               aplTypeRht,              // Right ...
                               aplTypeRes;              // Result ...
    APLNELM                    aplNELMLft,              // Left arg NELM
                               aplNELMRht;              // Right ...
    APLRANK                    aplRankLft,              // Left arg Rank
                               aplRankRht;              // Right ...
    APLLONGEST                 aplLongestLft,           // Left arg longest if immediate
                               aplLongestRht;           // Right ...
    HGLOBAL                    hGlbLft = NULL,          // Left arg global memory handle
                               hGlbRht = NULL,          // Right ...
                               hGlbRes = NULL;          // Result global memory handle
    LPVARARRAY_HEADER          lpMemHdrLft = NULL,      // Ptr to left arg header
                               lpMemHdrRht = NULL,      // ...    right    ...
                               lpMemHdrRes = NULL;      // ...    result   ...
    LPVOID                     lpMemLft,                // Ptr to left arg global memory
                               lpMemRht;                // Ptr to right ...
    LPAPLNESTED                lpMemRes;                // Ptr to result global memory
    LPAPLDIM                   lpMemDimLft;             // Ptr to left arg dimensions
    LPPL_YYSTYPE               lpYYRes = NULL,          // Ptr to result
                               lpYYTieNum = NULL;       // Ptr to tie num if right arg is char vector
    APLINT                     TieNum;                  // Right arg as an integer
    UINT                       uLft,                    // Loop counter
                               uTie = -1;               // Offset of matching tie number entry
    LPPERTABDATA               lpMemPTD;                // Ptr to PerTabData global memory
    APLUINT                    ByteRes;                 // # bytes in the result
    LPNFNSHDR                  lpNfnsHdr = NULL;        // Ptr to NFNSHDR global memory
    LPNFNSDATA                 lpNfnsMem;               // Ptr to aNfnsData
    UBOOL                      bRet,                    // TRUE iff the result is valid
                               bWildcard = FALSE;       // TRUE iff the right arg contains a wildcard
    PSECURITY_DESCRIPTOR       lpSD = NULL;             // Ptr to Security Descriptor
    LPWCHAR                    lpAcctName = NULL,       // Ptr to buffer for the acct name for lpSidOwner
                               lpDomainName = NULL,     // Ptr to buffer for the domain name for lpSidOwner
                               lpStringSID = NULL;      // Ptr to String SID
    BY_HANDLE_FILE_INFORMATION fileInfo;                // File information by handle buffer
    DWORD                      dwFileAttributes;        // File attributes w/o spurious attrs
    HANDLE                     hFileLink = NULL;        // File handle
    HMODULE                    hModuleLink = NULL;      // Module handle for GetProcAddress
    WIN32_FIND_DATAW           win32FindData;           // Wildcard find data struc
    HANDLE                     hWildcard = NULL;        // Wildcard file handle
    NPROP_STR                  propStr[NPROP_LENGTH];   // NINFO property struc
    WCHAR                      wszDrive[_MAX_DRIVE],    // Drive info from command line
                               wszDir  [_MAX_DIR];      // Dir   ...

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // If we're called monadically, ...
    if (lptkLftArg EQ NULL)
        // Use the default left arg of an immediate 0
        lptkLftArg = (LPTOKEN) &tkZero;

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for LEFT RANK ERROR
    if (IsMultiRank (aplRankLft))
        goto LEFT_RANK_EXIT;

    // Check for LEFT DOMAIN ERROR
    if (!IsNumeric (aplTypeLft))
        goto LEFT_DOMAIN_EXIT;

    // Check for RIGHT RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Check for RIGHT LENGTH ERROR
    if (!IsSimpleChar (aplTypeRht)
     && !IsSingleton (aplTypeRht))
        goto RIGHT_LENGTH_EXIT;

    // Check for RIGHT DOMAIN ERROR
    if (!IsSimpleChar (aplTypeRht)
     && !IsNumeric (aplTypeRht))
        goto RIGHT_DOMAIN_EXIT;

    // Lock the memory to get a ptr to it
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // If the left arg is a global, ...
    if (hGlbLft NE NULL)
    {
        lpMemDimLft = VarArrayBaseToDim  (lpMemHdrLft);
        lpMemLft    = VarArrayDataFmBase (lpMemHdrLft);
    } else
    {
        lpMemDimLft = &aplNELMLft;
        lpMemLft    = (LPAPLINT) &aplLongestLft;
    } // End IF/ELSE

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // If the right arg is a global, ...
    if (hGlbRht NE NULL)
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    else
        lpMemRht = (LPAPLINT) &aplLongestRht;

    // If the right arg is not a char vector, ...
    if (!IsSimpleChar (aplTypeRht))
    {
        // Verify the right arg as a tie number
        if (!NfnsArgTieNum (aplTypeRht, lpMemRht, aplNELMRht, 0, &TieNum, lptkFunc, lpMemPTD)
         ||  TieNum > 0)        // N.B. ">" not ">=" as we don't allow 0
            goto RIGHT_DOMAIN_EXIT;
    } else
        // Check for wildcards
        bWildcard =
         (strchrW (lpMemRht, L'*') NE NULL
       || strchrW (lpMemRht, L'?') NE NULL);

    // Calculate the result storage type
    aplTypeRes = IsEmpty (aplNELMLft) ? ARRAY_BOOL : ARRAY_NESTED;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMLft, aplRankLft);

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
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMLft;
    lpHeader->Rank       = aplRankLft;
#undef  lpHeader

    // Copy the dimensions from the left arg
    CopyMemory (VarArrayBaseToDim (lpMemHdrRes),
                lpMemDimLft,
     (APLU3264) (aplRankLft * sizeof (APLDIM)));

    // lpMemRes now points to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Lock the memory to get a ptr to it
    lpNfnsHdr = InitLockNfns (lptkFunc, lpMemPTD);

    // If we're wildcarding, ...
    if (bWildcard)
    {
        // Initialize the WIN32_FIND_DATA struc
        hWildcard =
          FindFirstFileW (lpMemRht,
                         &win32FindData);
        // Check for error
        if (hWildcard EQ INVALID_HANDLE_VALUE)
            goto SYS_ERROR_EXIT;
        // Extract the Drive and Path from the command line
        _wsplitpath (lpMemRht, wszDrive, wszDir, NULL, NULL);

        // Clear the wildcard property struc
        ZeroMemory (propStr, sizeof (propStr));
    } // End IF

    // Loop through the wildcard files
    while (hWildcard NE INVALID_HANDLE_VALUE)
    {
        // If the right arg is a char vector, ...
        if (IsSimpleChar (aplTypeRht))
        {
            // If we're wildcarding, ...
            if (bWildcard)
            {
                WCHAR   wszDPFE[_MAX_PATH];                 // Save area for the DPFE
                HANDLE  hFile;                              // File handle
                DWORD   AccessMode = GENERIC_READ,          // Access mode
                        ShareMode  = FILE_SHARE_READ,       // Share mode
                        uFlags     = 0                      // Flags
                                   | FILE_FLAG_OPEN_REPARSE_POINT
                                   | FILE_FLAG_BACKUP_SEMANTICS;
                APLNELM aplNELMTmp;                         // NELM of filename
                HGLOBAL hGlbTmp;                            // Global memory handle
                LPVARARRAY_HEADER lpMemHdrTmp = NULL;       // Ptr to ...    header
                LPAPLCHAR         lpMemTmp;                 // ...    global memory

                // Put the Drive, Dir, and Filename.ext info together
                _wmakepath (wszDPFE, wszDrive, wszDir, win32FindData.cFileName, NULL);

                // Attempt to open the file
                hFile =
                  CreateFileW (wszDPFE,                     // Ptr to Drive, path, filename
                               AccessMode,                  // Desired access
                               ShareMode,                   // Shared access
                               NULL,                        // Security attributes
                               OPEN_EXISTING,               // Create/open flags
                               uFlags,                      // File attributes & flags
                               NULL);                       // Template file
                // If it failed, ...
                if (hFile EQ INVALID_HANDLE_VALUE)
                    goto NEXTFILE;
                else
                {
                    // Get the filename NELM
                    aplNELMTmp = lstrlenW (wszDPFE);

                    // Allocate memory for the filename as a global
                    // Calculate space needed for the result
                    ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMTmp, 1);

                    // Check for overflow
                    if (ByteRes NE (APLU3264) ByteRes)
                        goto WSFULL_EXIT;

                    // Allocate space for the result
                    hGlbTmp = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
                    if (hGlbRes EQ NULL)
                        goto WSFULL_EXIT;

                    // Lock the memory to get a ptr to it
                    lpMemHdrTmp = MyGlobalLock000 (hGlbTmp);

#define lpHeader    lpMemHdrTmp
                    // Fill in the header values
                    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
                    lpHeader->ArrType    = ARRAY_CHAR;
////////////////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////////////////lpHeader->SysVar     = FALSE;       // Already zero from GHND
                    lpHeader->RefCnt     = 1;
                    lpHeader->NELM       = aplNELMTmp;
                    lpHeader->Rank       = 1;
#undef  lpHeader

                    // Copy the dimensions from the left arg
                    *VarArrayBaseToDim (lpMemHdrTmp) = aplNELMTmp;

                    // lpMemRes now points to the data
                    lpMemTmp = VarArrayDataFmBase (lpMemHdrTmp);

                    // Copy the filename to global memory ("+ 1" for the terminating zero)
                    CopyMemoryW (lpMemTmp, wszDPFE, (APLU3264) (aplNELMTmp + 1));

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbTmp); lpMemHdrTmp = NULL;

                    // Get the next tie number
                    TieNum = GetNextTieNum_EM (lptkFunc, lpMemPTD);

                    // Save the file handle, etc. in local storage
                    if (!NfnsSaveData_EM (hFile, hGlbTmp, 0, 0, ARRAY_ERROR, TieNum, lptkFunc, lpMemPTD))
                        goto ERROR_EXIT;
                } // End IF/ELSE
            } else
            {
                // Attempt to tie it
                lpYYTieNum = SysFnCreateTie_EM_YY (FALSE, lptkRhtArg, TRUE, lptkFunc, (LPTOKEN) &tkZero, lptkAxis);

                // Check for error
                if (lpYYTieNum EQ NULL)
                    goto ERROR_EXIT;
                Assert (lpYYTieNum->tkToken.tkFlags.TknType EQ TKT_VARIMMED);
                Assert (lpYYTieNum->tkToken.tkFlags.ImmType EQ IMMTYPE_INT);

                // Save the tie number
                TieNum = lpYYTieNum->tkToken.tkData.tkInteger;
            } // End IF/ELSE
        } // End IF

        // Lookup the corresponding index into aNfnsData
        bRet = IsDuplicateTieNum_EM (TieNum, lptkFunc, lpMemPTD, &uTie);

        Assert (bRet);

        // Point to the matching tie number entry
        lpNfnsMem = &lpNfnsHdr->aNfnsData[uTie];

        // Get the file information
        if (!GetFileInformationByHandle (lpNfnsMem->hFile, &fileInfo))
            goto SYS_ERROR_EXIT;

        // Copy the file attributes and remove spurious ones
        dwFileAttributes = fileInfo.dwFileAttributes
                         & ~(0
                           | FILE_ATTRIBUTE_READONLY
                           | FILE_ATTRIBUTE_HIDDEN
                           | FILE_ATTRIBUTE_SYSTEM
                           | FILE_ATTRIBUTE_ARCHIVE
                           | FILE_ATTRIBUTE_NORMAL
                           | FILE_ATTRIBUTE_TEMPORARY
                           | FILE_ATTRIBUTE_SPARSE_FILE
                           | FILE_ATTRIBUTE_COMPRESSED
                           | FILE_ATTRIBUTE_OFFLINE
                           | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED
                           | FILE_ATTRIBUTE_ENCRYPTED
                           | FILE_ATTRIBUTE_INTEGRITY_STREAM
                           | FILE_ATTRIBUTE_VIRTUAL
                           | FILE_ATTRIBUTE_NO_SCRUB_DATA
                           | FILE_ATTRIBUTE_EA
                           );
        // Loop through the left arg
        for (uLft = 0; uLft < aplNELMLft; uLft++)
        {
            APLINT            aplIntegerLft,        // Integer value in left arg
                              aplIntegerRes,        // ...              result
                              aplFileSize;          // File size
            FILETIME          ftLocalTime;          // Local time
            SYSTEMTIME        systemTime;           // []TS format time
            HGLOBAL           hGlbTmp = NULL;       // Temporary
            LPVARARRAY_HEADER lpMemHdrTmp = NULL;   // Ptr to temp header
            LPAPLCHAR         lpMemTmp;             // Ptr to temp global memory
            APLNELM           aplNELMTmp;           // Temp NELM
            DWORD             dwRtnCode = 0;        // Return code
            PSID              lpSidOwner = NULL;    // Ptr to SID owner
            BOOL              bRtnBool = TRUE;      // TRUE iff the result of LookupAccountSid is valid
            DWORD             dwAcctName = 1,       // Buffer size for lpAcctname
                              dwDomainName = 1;     // Buffer size for lpDomainName
            SID_NAME_USE      eUse = SidTypeUnknown;// Ptr to var that receives a SID_NAME_USE value

            // Convert the next item to an integer
            aplIntegerLft =
              ConvertToInteger_SCT (aplTypeLft,     // Arg storage type
                                    lpMemLft,       // Ptr to global memory data
                                    uLft,           // Index # into lpMemLft
                                   &bRet);          // Ptr to TRUE iff the result is valid
            if (!bRet)
                goto LEFT_DOMAIN_EXIT;

            // Split cases based upon the left arg value
            switch (aplIntegerLft)
            {
                case NPROP_DPFE:                    // Name of the file (char vector)
                    // If we're wildcarding, ...
                    if (bWildcard)
                    {
                        LPAPLNESTED lpMemTmp;

                        // If this is the first time wildcarding for this property, ...
                        if (!NPropInitAlloc (&propStr[aplIntegerLft], propArrType[aplIntegerLft]))
                            goto WSFULL_EXIT;

                        // Check for enough room
                        if (!NPropCheckResize (&propStr[aplIntegerLft], propArrType[aplIntegerLft]))
                            goto WSFULL_EXIT;
                        // Lock the memory to get a ptr to it
                        lpMemHdrTmp = MyGlobalLockVar (propStr[aplIntegerLft].hWildGlb);

                        // Skip over the header and dimensions to the data
                        lpMemTmp = VarArrayDataFmBase (lpMemHdrTmp);

                        // Save the next item
                        lpMemTmp[propStr[aplIntegerLft].uWildCur++] = CopySymGlbDir_PTB (lpNfnsMem->hGlbFileName);

                        // We no longer need this ptr
                        MyGlobalUnlock (propStr[aplIntegerLft].hWildGlb); lpMemHdrTmp = NULL;
                    } else
                        // Save a copy of the filename
                        lpMemRes[uLft] = CopySymGlbDir_PTB (lpNfnsMem->hGlbFileName);

                    break;

                case NPROP_TYPE:                        // 1:  Type:  (numeric scalar) (see FILE_TYPES)
                                                        //     0 = Not known
                                                        //     1 = Directory
                                                        //     2 = Regular file
                                                        //     3 = Character device
                                                        //     4 = Symbolic link (only when Follow is 0)
                                                        //     5 = Block device
                                                        //     6 = FIFO (not Windows)
                                                        //     7 = Socket (not Windows)
                    // Test for File Attribute Constants
                    if (dwFileAttributes EQ 0)
                        aplIntegerRes = NFILETYPE_REG;
                    else
                    if (dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
                        aplIntegerRes = NFILETYPE_LINK;
                    else
                    if (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                        aplIntegerRes = NFILETYPE_DIR;
                    else
                    if (dwFileAttributes & FILE_ATTRIBUTE_DEVICE)
                    {
                        aplIntegerRes = NFILETYPE_CHRDEV;       // ***FIXME*** -- How to distinguish between the two devices??
                        aplIntegerRes = NFILETYPE_BLKDEV;       // ***FIXME*** -- ...
                    } else
                        aplIntegerRes = NFILETYPE_UNK;

                    // If we're wildcarding, ...
                    if (bWildcard)
                    {
                        LPAPLINT lpMemTmp;

                        // If this is the first time wildcarding for this property, ...
                        if (!NPropInitAlloc (&propStr[aplIntegerLft], propArrType[aplIntegerLft]))
                            goto WSFULL_EXIT;

                        // Check for enough room
                        if (!NPropCheckResize (&propStr[aplIntegerLft], propArrType[aplIntegerLft]))
                            goto WSFULL_EXIT;
                        // Lock the memory to get a ptr to it
                        lpMemHdrTmp = MyGlobalLockVar (propStr[aplIntegerLft].hWildGlb);

                        // Skip over the header and dimensions to the data
                        lpMemTmp = VarArrayDataFmBase (lpMemHdrTmp);

                        // Save the next item
                        lpMemTmp[propStr[aplIntegerLft].uWildCur++] = aplIntegerRes;

                        // We no longer need this ptr
                        MyGlobalUnlock (propStr[aplIntegerLft].hWildGlb); lpMemHdrTmp = NULL;
                    } else
                    {
                        // Save an LPSYMENTRY in the result
                        lpMemRes[uLft] =
                          MakeSymEntry_EM (IMMTYPE_INT,                 // Immediate type
                                          &aplIntegerRes,               // Ptr to immediate value
                                           lptkFunc);                   // Ptr to function token
                        // Check for error
                        if (lpMemRes[uLft] EQ NULL)
                            goto ERROR_EXIT;
                    } // End IF/ELSE

                    break;

                case NPROP_SIZE:                        // 2:  File size in bytes (numeric scalar)
                    // Get the file size
                    LOAPLINT (aplFileSize) = fileInfo.nFileSizeLow;
                    HIAPLINT (aplFileSize) = fileInfo.nFileSizeHigh;

                    // If we're wildcarding, ...
                    if (bWildcard)
                    {
                        LPAPLINT lpMemTmp;

                        // If this is the first time wildcarding for this property, ...
                        if (!NPropInitAlloc (&propStr[aplIntegerLft], propArrType[aplIntegerLft]))
                            goto WSFULL_EXIT;

                        // Check for enough room
                        if (!NPropCheckResize (&propStr[aplIntegerLft], propArrType[aplIntegerLft]))
                            goto WSFULL_EXIT;
                        // Lock the memory to get a ptr to it
                        lpMemHdrTmp = MyGlobalLockVar (propStr[aplIntegerLft].hWildGlb);

                        // Skip over the header and dimensions to the data
                        lpMemTmp = VarArrayDataFmBase (lpMemHdrTmp);

                        // Save the next item
                        lpMemTmp[propStr[aplIntegerLft].uWildCur++] = aplFileSize;

                        // We no longer need this ptr
                        MyGlobalUnlock (propStr[aplIntegerLft].hWildGlb); lpMemHdrTmp = NULL;
                    } else
                    {
                        // Save an LPSYMENTRY in the result
                        lpMemRes[uLft] =
                          MakeSymEntry_EM (IMMTYPE_INT,                 // Immediate type
                                          &aplFileSize,                 // Ptr to immediate value
                                           lptkFunc);                   // Ptr to function token
                        // Check for error
                        if (lpMemRes[uLft] EQ NULL)
                            goto ERROR_EXIT;
                    } // End IF/ELSE

                    break;

                case NPROP_LASTMOD:                     // 3:  Last modification time (sensitive to bUseLocalTime) (numeric vector)
                    // Get the file's last write time

                    // If we're to use local time (instead of GMT), ...
                    if (OptionFlags.bUseLocalTime)
                        // Convert the last mod time to local time
                        FileTimeToLocalFileTime (&fileInfo.ftLastWriteTime, &ftLocalTime);
                    else
                        // Copy last mod time as local Time
                        ftLocalTime = fileInfo.ftLastWriteTime;
                    // Convert the last mod time to system time so we can display it
                    FileTimeToSystemTime (&ftLocalTime, &systemTime);

                    // Allocate space for the timestamp
                    hGlbTmp = TimestampAllocate (&systemTime);

                    // Check for error
                    if (hGlbTmp EQ NULL)
                        goto WSFULL_EXIT;

                    // If we're wildcarding, ...
                    if (bWildcard)
                    {
                        LPAPLNESTED lpMemTmp;

                        // If this is the first time wildcarding for this property, ...
                        if (!NPropInitAlloc (&propStr[aplIntegerLft], propArrType[aplIntegerLft]))
                            goto WSFULL_EXIT;

                        // Check for enough room
                        if (!NPropCheckResize (&propStr[aplIntegerLft], propArrType[aplIntegerLft]))
                            goto WSFULL_EXIT;
                        // Lock the memory to get a ptr to it
                        lpMemHdrTmp = MyGlobalLockVar (propStr[aplIntegerLft].hWildGlb);

                        // Skip over the header and dimensions to the data
                        lpMemTmp = VarArrayDataFmBase (lpMemHdrTmp);

                        // Save the next item
                        lpMemTmp[propStr[aplIntegerLft].uWildCur++] = MakePtrTypeGlb (hGlbTmp);

                        // We no longer need this ptr
                        MyGlobalUnlock (propStr[aplIntegerLft].hWildGlb); lpMemHdrTmp = NULL;
                    } else
                        // Save in the result
                        lpMemRes[uLft] = MakePtrTypeGlb (hGlbTmp);

                    break;

                case NPROP_OWNER_ID:                    // 4:  Owner user id (char vector)
                    // Get the owner SID of the file
                    dwRtnCode =
                      GetSecurityInfo (lpNfnsMem->hFile,            // Ptr to file handle
                                       SE_FILE_OBJECT,              // Object type enum (see SE_OBJECT_TYPE)
                                       OWNER_SECURITY_INFORMATION,  // Security Information flags (see SECURITY_INFORMATION)
                                      &lpSidOwner,                  // Ptr to SID owner (only if OWNER_SECURITY_INFORMATION)
                                       NULL,                        // ...    SID group (only if GROUP_SECURITY_INFORMATION)
                                       NULL,                        // ...    DACL (only if DACL_SECURITY_INFORMATION)
                                       NULL,                        // ...    SACL (only if SACL_SECURITY_INFORMATION)
                                      &lpSD);                       // ...    Security Descriptor
                    // Check GetLastError for GetSecurityInfo error condition
                    if (dwRtnCode != ERROR_SUCCESS)
                        goto SYS_ERROR_EXIT;

                    // Get the SID string from the SID
                    if (!ConvertSidToStringSidW (lpSidOwner, &lpStringSID))
                        goto SYS_ERROR_EXIT;

                    // Calculate space for the String SID
                    aplNELMTmp = lstrlenW (lpStringSID);

                    // Allocate a character array for the string SID
                    hGlbTmp = AllocateGlobalArray (ARRAY_CHAR, aplNELMTmp, 1, &aplNELMTmp);
                    if (hGlbTmp EQ NULL)
                        goto WSFULL_EXIT;

                    // Lock the memory to get a ptr to it
                    lpMemHdrTmp = MyGlobalLockVar (hGlbTmp);

                    // Copy the item data to global memory
                    CopyMemory (VarArrayDataFmBase (lpMemHdrTmp),
                                lpStringSID,
                                (APLU3264) (aplNELMTmp * sizeof (lpStringSID[0])));
                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbTmp); lpMemHdrTmp = NULL;

                    // If we're wildcarding, ...
                    if (bWildcard)
                    {
                        LPAPLNESTED lpMemTmp;

                        // If this is the first time wildcarding for this property, ...
                        if (!NPropInitAlloc (&propStr[aplIntegerLft], propArrType[aplIntegerLft]))
                            goto WSFULL_EXIT;

                        // Check for enough room
                        if (!NPropCheckResize (&propStr[aplIntegerLft], propArrType[aplIntegerLft]))
                            goto WSFULL_EXIT;
                        // Lock the memory to get a ptr to it
                        lpMemHdrTmp = MyGlobalLockVar (propStr[aplIntegerLft].hWildGlb);

                        // Skip over the header and dimensions to the data
                        lpMemTmp = VarArrayDataFmBase (lpMemHdrTmp);

                        // Save the next item
                        lpMemTmp[propStr[aplIntegerLft].uWildCur++] = MakePtrTypeGlb (hGlbTmp);

                        // We no longer need this ptr
                        MyGlobalUnlock (propStr[aplIntegerLft].hWildGlb); lpMemHdrTmp = NULL;
                    } else
                        // Save in the result
                        lpMemRes[uLft] = MakePtrTypeGlb (hGlbTmp);

                    break;

                case NPROP_OWNER_NAME:                  // 5:  Owner name (char vector)
                    // Get the owner SID of the file
                    dwRtnCode =
                      GetSecurityInfo (lpNfnsMem->hFile,            // Ptr to file handle
                                       SE_FILE_OBJECT,              // Object type enum (see SE_OBJECT_TYPE)
                                       OWNER_SECURITY_INFORMATION,  // Security Information flags (see SECURITY_INFORMATION)
                                      &lpSidOwner,                  // Ptr to SID owner (only if OWNER_SECURITY_INFORMATION)
                                       NULL,                        // ...    SID group (only if GROUP_SECURITY_INFORMATION)
                                       NULL,                        // ...    DACL (only if DACL_SECURITY_INFORMATION)
                                       NULL,                        // ...    SACL (only if SACL_SECURITY_INFORMATION)
                                      &lpSD);                       // ...    Security Descriptor
                    // Check GetLastError for GetSecurityInfo error condition
                    if (dwRtnCode != ERROR_SUCCESS)
                        goto SYS_ERROR_EXIT;

                    // First call to LookupAccountSid to get the buffer sizes
////////////////////bRtnBool =
                      LookupAccountSidW (NULL,          // Target computer (NULL = Local computer)
                                         lpSidOwner,    // Ptr to SID owner
                                         lpAcctName,    // Ptr to buffer for the acct name for lpSidOwner
                              (LPDWORD) &dwAcctName,    // Ptr to buffer size for lpAcctname
                                         lpDomainName,  // Ptr to buffer for the domain name for lpSidOwner
                              (LPDWORD) &dwDomainName,  // Ptr to buffer size for lpDomainName
                                        &eUse);         // Ptr to var that receives a SID_NAME_USE value
                    // No need to check for error because it is supposed to fail

                    // Allocate buffers
                    lpAcctName   = MyGlobalAlloc (GPTR, dwAcctName   * sizeof (WCHAR));
                    lpDomainName = MyGlobalAlloc (GPTR, dwDomainName * sizeof (WCHAR));

                    // Check for error
                    if (lpAcctName   EQ NULL
                     || lpDomainName EQ NULL)
                        goto WSFULL_EXIT;

                    // Second call to LookupAccountSid to get the account name.
                    bRtnBool =
                      LookupAccountSidW (NULL,          // Name of local or remote computer
                                         lpSidOwner,    // Security identifier
                                         lpAcctName,    // Account name buffer
                              (LPDWORD) &dwAcctName,    // Size of account name buffer
                                         lpDomainName,  // Domain name
                              (LPDWORD) &dwDomainName,  // Size of domain name buffer
                                        &eUse);         // SID type
                    // Check for error
                    if (!bRtnBool)
                        goto SYS_ERROR_EXIT;

                    // Calculate space for the catenation of lpDomainName \ lpAcctName
                    aplNELMTmp = dwDomainName + 1 + dwAcctName;

                    // Calculate space needed for the result
                    ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMTmp, 1);

                    // Check for overflow
                    if (ByteRes NE (APLU3264) ByteRes)
                        goto WSFULL_EXIT;

                    // Allocate space for the result
                    hGlbTmp = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
                    if (hGlbTmp EQ NULL)
                        goto WSFULL_EXIT;

                    // Lock the memory to get a ptr to it
                    lpMemHdrTmp = MyGlobalLock000 (hGlbTmp);

#define lpHeader    lpMemHdrTmp
                    // Fill in the header
                    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
                    lpHeader->ArrType    = ARRAY_CHAR;
////////////////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////////////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
                    lpHeader->RefCnt     = 1;
                    lpHeader->NELM       = aplNELMTmp;
                    lpHeader->Rank       = 1;
#undef  lpHeader

                    // Fill in the dimension
                    *VarArrayBaseToDim (lpMemHdrTmp) = aplNELMTmp;

                    // Skip over the header and dimensions to the data
                    lpMemTmp = VarArrayDataFmBase (lpMemHdrTmp);

                    // Copy the Domain Name to the result
                    lstrcpyW (lpMemTmp, lpDomainName);

                    // Append a backslash
                    lstrcatW (lpMemTmp, L"\\");

                    // Catenate the Acct Name to the result
                    lstrcatW (lpMemTmp, lpAcctName);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbTmp); lpMemHdrTmp = NULL;

                    // If we're wildcarding, ...
                    if (bWildcard)
                    {
                        LPAPLNESTED lpMemTmp;

                        // If this is the first time wildcarding for this property, ...
                        if (!NPropInitAlloc (&propStr[aplIntegerLft], propArrType[aplIntegerLft]))
                            goto WSFULL_EXIT;

                        // Check for enough room
                        if (!NPropCheckResize (&propStr[aplIntegerLft], propArrType[aplIntegerLft]))
                            goto WSFULL_EXIT;
                        // Lock the memory to get a ptr to it
                        lpMemHdrTmp = MyGlobalLockVar (propStr[aplIntegerLft].hWildGlb);

                        // Skip over the header and dimensions to the data
                        lpMemTmp = VarArrayDataFmBase (lpMemHdrTmp);

                        // Save the next item
                        lpMemTmp[propStr[aplIntegerLft].uWildCur++] = MakePtrTypeGlb (hGlbTmp);

                        // We no longer need this ptr
                        MyGlobalUnlock (propStr[aplIntegerLft].hWildGlb); lpMemHdrTmp = NULL;
                    } else
                        // Save in the result
                        lpMemRes[uLft] = MakePtrTypeGlb (hGlbTmp);

                    break;

                case NPROP_HIDDEN:                      // 6:  Hidden flag (Boolean scalar)
                    // If we're wildcarding, ...
                    if (bWildcard)
                    {
                        LPAPLINT lpMemTmp;

                        // If this is the first time wildcarding for this property, ...
                        if (!NPropInitAlloc (&propStr[aplIntegerLft], propArrType[aplIntegerLft]))
                            goto WSFULL_EXIT;

                        // Check for enough room
                        if (!NPropCheckResize (&propStr[aplIntegerLft], propArrType[aplIntegerLft]))
                            goto WSFULL_EXIT;
                        // Lock the memory to get a ptr to it
                        lpMemHdrTmp = MyGlobalLockVar (propStr[aplIntegerLft].hWildGlb);

                        // Skip over the header and dimensions to the data
                        lpMemTmp = VarArrayDataFmBase (lpMemHdrTmp);

                        // Save the next item
                        lpMemTmp[propStr[aplIntegerLft].uWildCur++] = (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) NE 0;

                        // We no longer need this ptr
                        MyGlobalUnlock (propStr[aplIntegerLft].hWildGlb); lpMemHdrTmp = NULL;
                    } else
                    {
                        // If the file is Hidden, ...
                        //   (note we use fileInfo.dwFileAttributes as FILE_ATTRIBUTE_HIDDEN has been removed from dwFileAttributes)
                        if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
                            // Save an LPSYMENTRY in the result
                            lpMemRes[uLft] = lpMemPTD->lphtsGLB->steOne ;
                        else
                            // Save an LPSYMENTRY in the result
                            lpMemRes[uLft] = lpMemPTD->lphtsGLB->steZero;
                    } // End IF/ELSE

                    break;

                case NPROP_SYMBLINK:                    // 7:  Target of symbol link (when Type is 4)
                    if (dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
                    {
                        GETFPN    GetFPN;               // Ptr to GetFinalPathNameByHandleW
                        LPAPLCHAR lpMemTmp;             // Ptr to global memory data

                        // If we're wildcarding, ...
                        if (bWildcard)
                            // Copy the file handle
                            hFileLink = lpNfnsMem->hFile;
                        else
                        {
                            DWORD AccessMode,           // Access mode
                                  ShareMode,            // Share mode
                                  uFlags;               // File flags & attributes

                            // Set the modes and flags for opening a reparse point
                            AccessMode = GENERIC_READ;
                            ShareMode  = FILE_SHARE_READ;
                            uFlags     = 0
                                       | FILE_FLAG_BACKUP_SEMANTICS;

                            // Attempt to open the file as a symbolic link
                            hFileLink =
                              CreateFileW (lpMemRht,                // Ptr to Drive, path, filename
                                           AccessMode,              // Desired access
                                           ShareMode,               // Shared access
                                           NULL,                    // Security attributes
                                           OPEN_EXISTING,           // Create/open flags
                                           uFlags,                  // File attributes & flags
                                           NULL);                   // Template file
                        } // End IF/ELSE

                        // If it succeeded, ...
                        if (hFileLink NE INVALID_HANDLE_VALUE)
                        {
                            // Because symbolic links were introduced in Windows Vista and we support back to XP,
                            //   we have to resort to LoadLibrary/GetProcAddress to call <GetFinalPathNameByHandle>
                            hModuleLink = LoadLibrary ("kernel32.dll");

                            // Check for error
                            if (hModuleLink NE NULL)
                            {
                                // Get the procedure address
                                GetFPN = (GETFPN) GetProcAddress (hModuleLink, "GetFinalPathNameByHandleW");

                                // If it was found, ...
                                if (GetFPN NE NULL)
                                {
                                    // Calculate space for the symbolic link target
                                    //   (this count includes the terminating zero)
                                    aplNELMTmp =
                                      (*GetFPN) (hFileLink,             // File handle
                                                 NULL,                  // Ptr to buffer
                                                 0,                     // Sizeof buffer
                                                 VOLUME_NAME_DOS);      // Flags
                                    // Check for error
                                    if (aplNELMTmp EQ 0)
                                        goto SYS_ERROR_EXIT;

                                    // Delete the terminating zero from the count
                                    aplNELMTmp--;

                                    // Allocate a character array for the symbolic link target
                                    hGlbTmp = AllocateGlobalArray (ARRAY_CHAR, aplNELMTmp, 1, &aplNELMTmp);
                                    if (hGlbTmp EQ NULL)
                                        goto WSFULL_EXIT;

                                    // Lock the memory to get a ptr to it
                                    lpMemHdrTmp = MyGlobalLockVar (hGlbTmp);

                                    // Skip over the header & dimensions
                                    lpMemTmp = VarArrayDataFmBase (lpMemHdrTmp);

                                    // Get the symbolic link target
                                    if ((*GetFPN) (hFileLink,
                                                   lpMemTmp,
                                           (DWORD) (aplNELMTmp * sizeof (WCHAR)),
                                                   VOLUME_NAME_DOS) EQ 0)
                                    {
                                        // We no longer need this ptr
                                        MyGlobalUnlock (hGlbTmp); lpMemHdrTmp = NULL;

                                        // We no longer need this resource
                                        MyGlobalFree (hGlbTmp); hGlbTmp = NULL;

                                        goto SYS_ERROR_EXIT;
                                    } // End IF

#define UNC_PREFIX  L"\\\\?\\"
#define UNC_COUNT   strcountof (UNC_PREFIX)

                                    // If the name begins with "\\?\", ...
                                    if (strncmpW (lpMemTmp, UNC_PREFIX, UNC_COUNT) EQ 0)
                                    {
                                        // Copy down the DPFE over the UNC prefix ("1 +" for the terminating zero)
                                        CopyMemoryW (lpMemTmp, &lpMemTmp[UNC_COUNT], (APLU3264) (1 + aplNELMTmp - UNC_COUNT));

                                        // Reduce the NELM and dimension
                                        lpMemHdrTmp->NELM                  -= UNC_COUNT;
                                        (*VarArrayBaseToDim (lpMemHdrTmp)) -= UNC_COUNT;

                                        // N.B.:  The GlobalSize of this array is larger than
                                        //   the minimum size necessary to store it
                                    } // End IF

                                    // We no longer need this ptr
                                    MyGlobalUnlock (hGlbTmp); lpMemHdrTmp = NULL;

                                    // If we're wildcarding, ...
                                    if (bWildcard)
                                    {
                                        // Save hGlbTmp as the result
                                        if (!NfnsSaveGlb (hGlbTmp, &propStr[aplIntegerLft], aplIntegerLft))
                                            goto WSFULL_EXIT;
                                    } else
                                        // Save in the result
                                        lpMemRes[uLft] = MakePtrTypeGlb (hGlbTmp);
                                } else
                                // If we're wildcarding, ...
                                if (bWildcard)
                                {
                                    // Save '' as the result
                                    if (!NfnsSaveGlb (hGlbV0Char, &propStr[aplIntegerLft], aplIntegerLft))
                                        goto WSFULL_EXIT;
                                } else
                                    // Save '' in the result
                                    lpMemRes[uLft] = MakePtrTypeGlb (hGlbV0Char);
                            } else
                            // If we're wildcarding, ...
                            if (bWildcard)
                            {
                                // Save '' as the result
                                if (!NfnsSaveGlb (hGlbV0Char, &propStr[aplIntegerLft], aplIntegerLft))
                                    goto WSFULL_EXIT;
                            } else
                                // Error condition:  return ''
                               lpMemRes[uLft] = MakePtrTypeGlb (hGlbV0Char);
#undef  UNC_COUNT
#undef  UNC_PREFIX
                            // If we're not wildcarding, ...
                            if (!bWildcard)
                            {
                                // We no longer need this handle
                                CloseHandle (hFileLink); hFileLink = NULL;
                            } // End IF
                        } else
                        // If we're wildcarding, ...
                        if (bWildcard)
                        {
                            // Save '' as the result
                            if (!NfnsSaveGlb (hGlbV0Char, &propStr[aplIntegerLft], aplIntegerLft))
                                goto WSFULL_EXIT;
                        } else
                            // Not a Symbolic Link:  return ''
                            lpMemRes[uLft] = MakePtrTypeGlb (hGlbV0Char);
                    } else
                    // If we're wildcarding, ...
                    if (bWildcard)
                    {
                        // Save '' as the result
                        if (!NfnsSaveGlb (hGlbV0Char, &propStr[aplIntegerLft], aplIntegerLft))
                            goto WSFULL_EXIT;
                    } else
                        // Not a Symbolic Link:  return ''
                        lpMemRes[uLft] = MakePtrTypeGlb (hGlbV0Char);

                    break;

                default:
                    // If we're wildcarding, ...
                    if (bWildcard)
                    {
                        // Save {zilde} as the result
                        if (!NfnsSaveGlb (hGlbZilde, &propStr[aplIntegerLft], aplIntegerLft))
                            goto WSFULL_EXIT;
                    } else
                        // Unknown value:  return Zilde
                        lpMemRes[uLft] = MakePtrTypeGlb (hGlbZilde);

                    break;
            } // End SWITCH
        } // End FOR

        // If we're wildcarding, ...
        if (bWildcard)
        {
            Assert (hWildcard NE INVALID_HANDLE_VALUE);

            // Lookup the corresponding index into aNfnsData
            IsDuplicateTieNum_EM (TieNum, lptkFunc, lpMemPTD, &uTie);

            // Release the resources
            NfnsReleaseResources (lpNfnsHdr, lpNfnsMem, uTie); uTie = -1;
NEXTFILE:
            if (FindNextFileW (hWildcard, &win32FindData) EQ 0)
            {
                if (GetLastError () NE ERROR_NO_MORE_FILES)
                    goto SYS_ERROR_EXIT;

                // Tell the WHILE loop to stop
                hWildcard = INVALID_HANDLE_VALUE;

                // Loop through the result
                for (uLft = 0; uLft < aplNELMLft; uLft++)
                {
                    APLINT aplIndex;            // Loop counter

                    // Convert the next item to an integer
                    aplIndex =
                      ConvertToInteger_SCT (aplTypeLft,     // Arg storage type
                                            lpMemLft,       // Ptr to global memory data
                                            uLft,           // Index # into lpMemLft
                                           &bRet);          // Ptr to TRUE iff the result is valid
                    if (!bRet)
                        goto LEFT_DOMAIN_EXIT;

                    // Copy the global memory handles from propStr to lpMemRes
                    lpMemRes[uLft] = CopySymGlbDirAsGlb (propStr[aplIndex].hWildGlb);
                } // End FOR
            } // End IF
        } else
            // Tell the WHILE loop to stop
            hWildcard = INVALID_HANDLE_VALUE;
     } // End WHILE

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

SYS_ERROR_EXIT:
    SysErrMsg_EM (GetLastError (), lptkFunc);

    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF

        // We no longer need this resource
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF

    // If we're wildcarding, ...
    if (bWildcard
     && uTie NE -1
     && hWildcard NE INVALID_HANDLE_VALUE)
    {
        // Lookup the corresponding index into aNfnsData
        IsDuplicateTieNum_EM (TieNum, lptkFunc, lpMemPTD, &uTie);

        // Release the resources
        NfnsReleaseResources (lpNfnsHdr, lpNfnsMem, uTie);
    } // End IF
NORMAL_EXIT:
    if (bWildcard && hWildcard NE NULL)
    {
        // Close the find data handle
        FindClose (hWildcard); hWildcard = NULL;
    } // End IF

    if (lpYYTieNum NE NULL)
    {
        // Close the handle -- this also releases all locked regions
        CloseHandle (lpNfnsMem->hFile); lpNfnsMem->hFile = NULL;

        // Release resources
        NfnsReleaseResources (lpNfnsHdr, lpNfnsMem, uTie);

        // We no longer need this resource
        FreeResult (lpYYTieNum); YYFree (lpYYTieNum); lpYYTieNum = NULL;
    } // End IF

    if (bWildcard)
    {
        UINT uProp;                     // Loop counter

        // Loop through the wildcard property data
        for (uProp = 0; uProp < NPROP_LENGTH; uProp++)
        // If the global memory handle is valid, ...
        if (propStr[uProp].hWildGlb NE NULL)
        {
            // Note that the HGLOBALs in an ARRAY_NESTED item are at RefCnt EQ 1,
            //   so there's no need to free them separately outside <FreeResultGlobalVar>.

            // We no longer need this resource
            FreeResultGlobalVar (propStr[uProp].hWildGlb); propStr[uProp].hWildGlb = NULL;
        } // End FOR/IF
    } // End IF

    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    if (hGlbLft NE NULL && lpMemHdrLft NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;
    } // End IF

    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    if (lpNfnsHdr NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lpMemPTD->hGlbNfns); lpNfnsHdr = NULL;
    } // End IF

    if (lpSD NE NULL)
    {
        // We no longer need this ptr
        LocalFree (lpSD); lpSD = NULL;
    } // End IF

    if (lpAcctName NE NULL)
    {
        // We no longer need this ptr
        MyGlobalFree (lpAcctName); lpAcctName = NULL;
    } // End IF

    if (lpDomainName NE NULL)
    {
        // We no longer need this ptr
        MyGlobalFree (lpDomainName); lpDomainName = NULL;
    } // End IF

    if (lpStringSID NE NULL)
    {
        // We no longer need this ptr
        LocalFree (lpStringSID); lpStringSID = NULL;
    } // End IF

    if (!bWildcard && hFileLink NE NULL)
    {
        // We no longer need this handle
        CloseHandle (hFileLink); hFileLink = NULL;
    } // End IF

    if (hModuleLink NE NULL)
    {
        // We no longer need this handle
        FreeLibrary (hModuleLink); hModuleLink = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnNINFO_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $NfnsSaveGlb
//
//  Save a global memory handle in a wildcard result
//***************************************************************************

UBOOL NfnsSaveGlb
    (HGLOBAL     hGlbTmp,
     LPNPROP_STR lpPropStr,
     APLINT      aplIntegerLft)

{
    LPVARARRAY_HEADER lpMemHdrTmp = NULL;   // Ptr to temp header
    LPAPLNESTED       lpMemTmp;

    // If this is the first time wildcarding for this property, ...
    if (!NPropInitAlloc (lpPropStr, propArrType[aplIntegerLft]))
        return FALSE;

    // Check for enough room
    if (!NPropCheckResize (lpPropStr, propArrType[aplIntegerLft]))
        return FALSE;
    // Lock the memory to get a ptr to it
    lpMemHdrTmp = MyGlobalLockVar (lpPropStr->hWildGlb);

    // Skip over the header and dimensions to the data
    lpMemTmp = VarArrayDataFmBase (lpMemHdrTmp);

    // Save the next item
    lpMemTmp[lpPropStr->uWildCur++] = MakePtrTypeGlb (hGlbTmp);

    // We no longer need this ptr
    MyGlobalUnlock (lpPropStr->hWildGlb); lpMemHdrTmp = NULL;

    return TRUE;
} // End NfnsSaveGlb


//***************************************************************************
//  $NPropInitAlloc
//
//  Allocate initial space for NINFO wildcard properties
//***************************************************************************

UBOOL NPropInitAlloc
    (LPNPROP_STR lpPropStr,         // Ptr to the array of NPROP_STRs
     ARRAY_TYPES aplTypeRes)        // Array storage type

{
    APLUINT           ByteRes;              // # bytes in the result
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to the result header

    // If we've already made the initial allocation, ...
    if (lpPropStr->hWildGlb NE NULL)
        return TRUE;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, NPROP_INIT, 1);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        return FALSE;

    // Allocate space for the result
    lpPropStr->hWildGlb =
      MyGlobalAlloc (GHND, (APLU3264) ByteRes);

    // Check for error
    if (lpPropStr->hWildGlb EQ NULL)
        return FALSE;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLock000 (lpPropStr->hWildGlb);

#define lpHeader    lpMemHdrRes
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
////lpHeader->NELM       = 0;           // Already zero from GHND
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Save as the dimension
////*VarArrayBaseToDim (lpMemHdrRes) = 0;   // Already zero from GHND

    // We no longer need this ptr
    MyGlobalUnlock (lpPropStr->hWildGlb); lpMemHdrRes = NULL;

    // Set the current and maximum values
    lpPropStr->uWildCur = 0;
    lpPropStr->uWildMax = NPROP_INIT;

    return TRUE;
} // End NPropInitAlloc


//***************************************************************************
//  $NPropCheckResize
//
//  Check on whether or not we need to resize the current allocation for
//    a Wildcard property
//***************************************************************************

UBOOL NPropCheckResize
    (LPNPROP_STR lpPropStr,         // Ptr to the array of NPROP_STRs
     ARRAY_TYPES aplTypeRes)        // Array storage type

{
    LPVARARRAY_HEADER lpMemHdrNew = NULL;   // Ptr to the new result header

    // If there isn't room for one more entry, ...
    if (lpPropStr->uWildCur EQ lpPropStr->uWildMax)
    {
        APLUINT ByteRes;                    // # bytes in the result
        HGLOBAL hWildGlb;                   // Reallocated global memory handle

        // Calculate space needed for the result
        ByteRes = CalcArraySize (aplTypeRes, lpPropStr->uWildMax + NPROP_INCR, 1);

        // Check for overflow
        if (ByteRes NE (APLU3264) ByteRes)
            return FALSE;

        // ReAllocate space for the result,
        //   moving the old data to the new location, and
        //   freeing the old global memory
        hWildGlb =
          MyGlobalReAlloc (lpPropStr->hWildGlb, (APLU3264) ByteRes,  GHND);

        // Check for error
        if (hWildGlb EQ NULL)
        {
            LPVARARRAY_HEADER lpMemHdrOld = NULL;   // Ptr to the old result header

            // Allocate new memory
            hWildGlb =
              MyGlobalAlloc (GHND, (APLU3264) ByteRes);

            // Check for error
            if (hWildGlb EQ NULL)
                return FALSE;
            // Lock the old & new memory to get a ptr to it
            lpMemHdrOld = MyGlobalLockVar (lpPropStr->hWildGlb);
            lpMemHdrNew = MyGlobalLock000 (           hWildGlb);

            // Copy the old data to the new location
            CopyMemory (lpMemHdrNew, lpMemHdrOld, MyGlobalSize (lpPropStr->hWildGlb));

            // We no longer need this ptr
            MyGlobalUnlock (lpPropStr->hWildGlb); lpMemHdrOld = NULL;
        } // End IF

        // Save as new maximum
        lpPropStr->uWildMax += NPROP_INCR;

        // Save as new ptr
        lpPropStr->hWildGlb = hWildGlb;
    } // End IF

    // Lock the new memory to get a ptr to it
    lpMemHdrNew = MyGlobalLockVar (lpPropStr->hWildGlb);

    // Count in one more item
    lpMemHdrNew->NELM++;

    // Save as the dimension of the vector
    *VarArrayBaseToDim (lpMemHdrNew) = lpMemHdrNew->NELM;

    // We no longer need this ptr
    MyGlobalUnlock (lpPropStr->hWildGlb); lpMemHdrNew = NULL;

    return TRUE;
} // End NPropCheckResize


//***************************************************************************
//  $SysFnNLOCK_EM_YY
//
//  System function:  []NLOCK -- Lock/Unlock an open native file
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnNLOCK_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnNLOCK_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonNLOCK_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydNLOCK_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnNLOCK_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonNLOCK_EM_YY
//
//  Monadic []NLOCK -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonNLOCK_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonNLOCK_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End SysFnMonNLOCK_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydNLOCK_EM_YY
//
//  Dyadic []NLOCK -- Lock/Unlock an open native file
//
//  Type [Timeout] []NLOCK tn [Offset [Length]]
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydNLOCK_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydNLOCK_EM_YY
    (LPTOKEN lptkLftArg,                // Ptr to left arg token
     LPTOKEN lptkFunc,                  // Ptr to function token
     LPTOKEN lptkRhtArg,                // Ptr to right arg token
     LPTOKEN lptkAxis)                  // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeLft,           // Left arg storage type
                      aplTypeRht;           // Right ...
    APLNELM           aplNELMLft,           // Left arg NELM
                      aplNELMRht;           // Right ...
    APLRANK           aplRankLft,           // Left arg Rank
                      aplRankRht;           // Right ...
    APLLONGEST        aplLongestLft,        // Left arg longest if immediate
                      aplLongestRht;        // Right ...
    UINT              uTie;                 // Offset of matching tie number entry
    HGLOBAL           hGlbLft = NULL,       // Left arg global memory handle
                      hGlbRht = NULL;       // Right ...
    LPVARARRAY_HEADER lpMemHdrLft = NULL,   // Ptr to left arg header
                      lpMemHdrRht = NULL;   // ...    right    ...
    LPAPLINT          lpMemLft,             // Ptr to left arg global memory
                      lpMemRht;             // Ptr to right ...
    APLINT            TieNum,               // File tie number
                      LockLength,           // Lock length
                      LockType,             // Lock type
                      LockOffset;           // Lock offset
    LARGE_INTEGER     LockTimeout;          // Lock timeout
    LPPERTABDATA      lpMemPTD;             // Ptr to PerTabData global memory
    LPNFNSHDR         lpNfnsHdr = NULL;     // Ptr to NFNSHDR global memory
    LPNFNSDATA        lpNfnsMem;            // Ptr to aNfnsData
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    OVERLAPPED        overLapped = {0};     // OVERLAPPED struc
    DWORD             dwFlags = 0;          // LockFileEx flags

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for LEFT RANK ERROR
    if (IsMultiRank (aplRankLft))
        goto LEFT_RANK_EXIT;

    // Check for RIGHT RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Check for LEFT LENGTH ERROR
    if (aplNELMLft NE 1
     && aplNELMLft NE 2)
        goto LEFT_LENGTH_EXIT;

    // Check for RIGHT LENGTH ERROR
    if (1 > aplNELMRht
     ||     aplNELMRht > 3)
        goto RIGHT_LENGTH_EXIT;

    // Check for LEFT DOMAIN ERROR
    if (!IsNumeric (aplTypeLft))
        goto LEFT_DOMAIN_EXIT;

    // Check for RIGHT DOMAIN ERROR
    if (!IsNumeric (aplTypeRht))
        goto RIGHT_DOMAIN_EXIT;

    // Lock the memory to get a ptr to it
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // If the left arg is a global, ...
    if (hGlbLft NE NULL)
        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
    else
        lpMemLft = (LPAPLINT) &aplLongestLft;

    // If the right arg is a global, ...
    if (hGlbRht NE NULL)
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    else
        lpMemRht = (LPAPLINT) &aplLongestRht;

    // Validate the tie number
    if (!NfnsArgTieNum (aplTypeRht, lpMemRht, aplNELMRht, 0, &TieNum, lptkFunc, lpMemPTD)
     ||  TieNum >= 0)
        goto RIGHT_DOMAIN_EXIT;

    // Check for duplicate tie number
    switch (IsDuplicateTieNum_EM (TieNum, lptkFunc, lpMemPTD, &uTie))
    {
        case -1:
            goto ERROR_EXIT;

        case FALSE:
            goto RIGHT_DOMAIN_EXIT;

        case TRUE:
            break;

        defstop
            break;
    } // End SWITCH

    // Parse LockOffset from lpMemRht
    LockOffset  =  0;                   // Set default value
    if (!NfnsArgAplint (aplTypeRht,     // Arg storage type
                        lpMemRht,       // Ptr to global memory data
                        aplNELMRht,     // NELM of arg
                        1,              // Index # into lpMem
                       &LockOffset))    // Ptr to result APLINT
        goto RIGHT_DOMAIN_EXIT;

    // Lock the memory to get a ptr to it
    lpNfnsHdr = InitLockNfns (lptkFunc, lpMemPTD);

    // Point to the matching tie number entry
    lpNfnsMem = &lpNfnsHdr->aNfnsData[uTie];

    // Get the file size
    LOAPLINT (LockLength) =
      GetFileSize (lpNfnsMem->hFile,            // File handle
                  &HIAPLINT (LockLength));      // High-order file size
    // Less the offset to encompass the remainder of the file
    LockLength -= LockOffset;

    // Parse LockLength from lpMemRht
    if (!NfnsArgAplint (aplTypeRht,     // Arg storage type
                        lpMemRht,       // Ptr to global memory data
                        aplNELMRht,     // NELM of arg
                        2,              // Index # into lpMem
                       &LockLength))    // Ptr to result APLINT
        goto RIGHT_DOMAIN_EXIT;

    // Parse LockType from lpMemLft
    if (!NfnsArgAplint (aplTypeLft,     // Arg storage type
                        lpMemLft,       // Ptr to global memory data
                        aplNELMLft,     // NELM of arg
                        0,              // Index # into lpMem
                       &LockType))      // Ptr to result APLINT
        goto LEFT_DOMAIN_EXIT;

    // Parse LockTimeout from lpMemLft
    LockTimeout.QuadPart = -1;                  // Set default value
    if (!NfnsArgAplint (aplTypeLft,             // Arg storage type
                        lpMemLft,               // Ptr to global memory data
                        aplNELMLft,             // NELM of arg
                        1,                      // Index # into lpMem
                       &LockTimeout.QuadPart))  // Ptr to result APLINT
        goto RIGHT_DOMAIN_EXIT;

    // Initialize the OVERLAPPED struc with the lock offset
    overLapped.Offset     = LOAPLINT (LockOffset);
    overLapped.OffsetHigh = HIAPLINT (LockOffset);
////overLapped.hEvent     = NULL;                       // Set above to zero by = {0}

    switch (LockType)
    {
        case 0:             // Unlock
            if (!UnlockFileEx (lpNfnsMem->hFile,        // File handle
                               0,                       // Reserved
                               LOAPLINT (LockLength),   // # bytes to unlock, low dword
                               HIAPLINT (LockLength),   // ...                high ...
                              &overLapped))             // Ptr to OVERLAPPED struc
            {
                SysErrMsg_EM (GetLastError (), lptkFunc);

                goto ERROR_EXIT;
            } // End IF

            break;

        case 2:             // Write Lock
            dwFlags |= LOCKFILE_EXCLUSIVE_LOCK;

            // Fall through to common lock code
        case 1:             // Read Lock
            // If LockTimeout is valid, ...
            if (LockTimeout.QuadPart NE -1)
            {
                // Create an event
                overLapped.hEvent =
                  CreateEvent (NULL,                        // Ptr to security attributes (may be NULL)
                               TRUE,                        // TRUE iff manual-reset timer
                               FALSE,                       // TRUE if initial state is signalled
                               NULL);                       // Ptr to event name (may be NULL)
                if (overLapped.hEvent EQ NULL)
                    goto EVENT_EXIT;
            } // End IF

            if (!LockFileEx (lpNfnsMem->hFile,          // File handle
                             dwFlags,                   // Flags
                             0,                         // Reserved
                             LOAPLINT (LockLength),     // # bytes to lock, low dword
                             HIAPLINT (LockLength),     // ...              high ...
                            &overLapped))               // Ptr to OVERLAPPED struc
            {
                DWORD dwErr = GetLastError ();

                if (dwErr NE ERROR_IO_PENDING)
                {
                    SysErrMsg_EM (dwErr, lptkFunc);

                    goto ERROR_EXIT;
                } // End IF
            } // End IF

            // If LockTimeout is valid, ...
            if (LockTimeout.QuadPart NE -1)
            {
                DWORD dwRet;
                HANDLE hWaitEvent;

                // Convert from seconds to milliseconds
                LockTimeout.QuadPart *= 1000;   // Seconds to milliseconds

                // Check for overflow
                Assert (LockTimeout.QuadPart EQ (DWORD) LockTimeout.QuadPart);

                // Check for previous uncleared event
                Assert (lpMemPTD->hWaitEvent EQ NULL);

                // Save the wait event in case the user Ctrl-Breaks
                lpMemPTD->hWaitEvent = overLapped.hEvent;

                // Wait for the specified time
                dwRet = WaitForSingleObject (overLapped.hEvent, (DWORD) LockTimeout.QuadPart);

                // Save and clear the wait event
                //  so as to test for Ctrl-Break signalling the event
                hWaitEvent = lpMemPTD->hWaitEvent;
                lpMemPTD->hWaitEvent = NULL;

                // If the lock is not granted, ...
                if (hWaitEvent EQ NULL          // Ctrl-Break
                 || dwRet NE WAIT_OBJECT_0)     // Incomplete I/O
                    goto LOCKED_EXIT;
            } // End IF

            break;

        default:
            goto RIGHT_DOMAIN_EXIT;
    } // End SWITCH

    // The result is lptkRhtArg

    // Allocate a new YYRes;
    lpYYRes = YYAlloc ();

    // If the result is a global, ...
    if (hGlbRht NE NULL)
    {
        // Increment the right arg ref cnt so we can return it as the result
        hGlbRht = MakePtrTypeGlb (hGlbRht);

        // Increment the refcnt
        DbgIncrRefCntDir_PTB (hGlbRht); // []NLOCK with 2- or 3-element right arg

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay =                // Filled in below
        lpYYRes->tkToken.tkData.tkGlbData  = hGlbRht;
////////lpYYRes->tkToken.tkCharIndex       =                // Filled in below
    } else
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
////////lpYYRes->tkToken.tkFlags.NoDisplay =                // Filled in below
        lpYYRes->tkToken.tkData.tkInteger  = aplLongestRht;
////////lpYYRes->tkToken.tkCharIndex       =                // Filled in below
    } // End IF/ELSE

    lpYYRes->tkToken.tkFlags.NoDisplay = TRUE;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

LEFT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

EVENT_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LIMIT_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LOCKED_EXIT:
    ErrorMessageIndirectToken (ERRMSG_FILE_NOT_LOCKED APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (overLapped.hEvent NE NULL)
    {
        // We no longer need this resource
        CloseHandle (overLapped.hEvent); overLapped.hEvent = NULL;
    } // End IF

    if (hGlbLft NE NULL && lpMemHdrLft NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;
    } // End IF

    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    if (lpNfnsHdr NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lpMemPTD->hGlbNfns); lpNfnsHdr = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnDydNLOCK_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnNNAMES_EM_YY
//
//  System function:  []NNAMES -- Get the names of open native files
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnNNAMES_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnNNAMES_EM_YY
    (LPTOKEN lptkLftArg,                // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,                  // Ptr to function token
     LPTOKEN lptkRhtArg,                // Ptr to right arg token
     LPTOKEN lptkAxis)                  // Ptr to axis token (may be NULL)

{
    HGLOBAL           hGlbRes = NULL;           // Result global memory handle
    LPAPLINT          lpMemRes;                 // Ptr to result global memory
    LPVARARRAY_HEADER lpMemHdrFile = NULL,      // Ptr to filename header
                      lpMemHdrRes = NULL;       // ...    result   ...
    LPPERTABDATA      lpMemPTD;                 // Ptr to PerTabData global memory
    LPPL_YYSTYPE      lpYYRes = NULL;           // Ptr to the result
    LPNFNSHDR         lpNfnsHdr = NULL;         // Ptr to NFNSHDR global memory
    LPNFNSDATA        lpNfnsMem;                // Ptr to aNfnsData
    APLUINT           ByteRes;                  // # bytes in the result
    UINT              uCnt;                     // Loop counter
    APLUINT           uMaxLen,                  // Max length of the file names
                      uLen;                     // Length of a filename

    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Lock the memory to get a ptr to it
    lpNfnsHdr = InitLockNfns (lptkFunc, lpMemPTD);

    // Point to the first entry in use
    lpNfnsMem = &lpNfnsHdr->aNfnsData[lpNfnsHdr->offFirstInuse];

    // Loop through the in use tie numbers
    for (uMaxLen = uCnt = 0; uCnt < lpNfnsHdr->nTieNums; uCnt++)
    {
        // Lock the memory to get a ptr to it
        lpMemHdrFile = MyGlobalLockVar (lpNfnsMem->hGlbFileName);

        // Get the maximum length
        uMaxLen = max (uMaxLen, lpMemHdrFile->NELM);

        // We no longer need this ptr
        MyGlobalUnlock (lpNfnsMem->hGlbFileName); lpMemHdrFile = NULL;

        // Point to the next tie number in use
        lpNfnsMem = &lpNfnsHdr->aNfnsData[lpNfnsMem->offNextInuse];
    } // End FOR

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_CHAR, lpNfnsHdr->nTieNums * uMaxLen, 2);

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
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = lpNfnsHdr->nTieNums * uMaxLen;
    lpHeader->Rank       = 2;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemHdrRes);

    // Save the two dimensions
    *((LPAPLDIM) lpMemRes)++ = lpNfnsHdr->nTieNums;
    *((LPAPLDIM) lpMemRes)++ = uMaxLen;

    // lpMemRes now points to the data

    // Point to the first entry in use
    lpNfnsMem = &lpNfnsHdr->aNfnsData[lpNfnsHdr->offFirstInuse];

    // Loop through the tie numbers
    for (uCnt = 0; uCnt < lpNfnsHdr->nTieNums; uCnt++)
    {
        // Lock the memory to get a ptr to it
        lpMemHdrFile = MyGlobalLockVar (lpNfnsMem->hGlbFileName);

        // Get the filename length
        uLen = lpMemHdrFile->NELM;

        // Copy the filename to the result
        CopyMemoryW (lpMemRes, VarArrayDataFmBase (lpMemHdrFile), (APLU3264) uLen);

        // Fill in the tail with blanks
        FillMemoryW (&((LPAPLCHAR) lpMemRes)[uLen], (APLU3264) (uMaxLen - uLen), L' ');

        // We no longer need this ptr
        MyGlobalUnlock (lpNfnsMem->hGlbFileName); lpMemHdrFile = NULL;

        // Point to the next tie number in use
        lpNfnsMem = &lpNfnsHdr->aNfnsData[lpNfnsMem->offNextInuse];

        // Point to the next row in the result
        ((LPAPLCHAR) lpMemRes) += uMaxLen;
    } // End FOR

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbRes, lpMemHdrRes);
NORMAL_EXIT:
    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    if (lpNfnsHdr NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lpMemPTD->hGlbNfns); lpNfnsHdr = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnNNAMES_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnNNUMS_EM_YY
//
//  System function:  []NNUMS -- Get the tie numbers of open native files
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnNNUMS_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnNNUMS_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    HGLOBAL           hGlbRes = NULL;       // Result global memory handle
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // ...    result   ...
    LPAPLINT          lpMemRes;             // Ptr to result global memory
    LPPERTABDATA      lpMemPTD;             // Ptr to PerTabData global memory
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    LPNFNSHDR         lpNfnsHdr = NULL;     // Ptr to NFNSHDR global memory
    LPNFNSDATA        lpNfnsMem;            // Ptr to aNfnsData
    APLUINT           ByteRes;              // # bytes in the result
    UINT              uCnt;                 // Loop counter

    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Lock the memory to get a ptr to it
    lpNfnsHdr = InitLockNfns (lptkFunc, lpMemPTD);

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_INT, lpNfnsHdr->nTieNums, 1);

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
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_INT;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = lpNfnsHdr->nTieNums;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemHdrRes);

    // Save the dimension
    *((LPAPLDIM) lpMemRes)++ = lpNfnsHdr->nTieNums;

    // lpMemRes now points to the data

    // Point to the first entry in use
    lpNfnsMem = &lpNfnsHdr->aNfnsData[lpNfnsHdr->offFirstInuse];

    // Loop through the tie numbers
    for (uCnt = 0; uCnt < lpNfnsHdr->nTieNums; uCnt++)
    {
        // Copy the existing tie numbers to the global memory
        *lpMemRes++ = lpNfnsMem->iTieNumber;

        // Point to the next tie number in use
        lpNfnsMem = &lpNfnsHdr->aNfnsData[lpNfnsMem->offNextInuse];
    } // End FOR

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbRes, lpMemHdrRes);
NORMAL_EXIT:
    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    if (lpNfnsHdr NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lpMemPTD->hGlbNfns); lpNfnsHdr = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnNNUMS_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnNREAD_EM_YY
//
//  System function:  []NREAD -- Read data from an open native file
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnNREAD_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnNREAD_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonNREAD_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydNREAD_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnNREAD_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonNREAD_EM_YY
//
//  Monadic []NREAD -- Read data from an open native file
//
//  []NREAD tn [DiskConv          [NELM [FileOffset]]]
//  []NREAD tn [(DiskConv WsConv) [NELM [FileOffset]]]
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonNREAD_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonNREAD_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeRht,           // Right arg storage type
                      aplTypeWs;            // Workspace ...
    APLNELM           aplNELMRht,           // Right arg NELM
                      aplNELMRes;           // Result ...
    APLRANK           aplRankRht;           // Right arg Rank
    APLLONGEST        aplLongestRht;        // Right arg longest if immediate
    HGLOBAL           hGlbRht = NULL,       // Right arg global memory handle
                      hGlbRes = NULL;       // Result ...
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg header
                      lpMemHdrRes = NULL;   // ...    result    ...
    LPVOID            lpMemRht;             // Ptr to right arg global memory
    LPAPLINT          lpMemRes;             // Ptr to result global memory data
    APLINT            TieNum,               // File tie number
                      aplFileSize;          // File size
    DR_VAL            DiskConv,             // Disk format
                      WsConv;               // Workspace format
    APLUINT           ByteRes,              // # bytes in the result
                      aplOff = 0,           // Temporary offset
                      aplFileOff;           // Starting offset of read
    UINT              uTie;                 // Offset of matching tie number entry
    APLNELM           uCnt,                 // Loop counter
                      uMin;                 // Loop limit
    UBOOL             bFileOffset;          // TRUE iff the file offset is specified
    DWORD             dwRead,               // # bytes to read
                      dwRet,                // Return code from WaitForSingleObject
                      dwErr;                // GetLastError code
    LPPERTABDATA      lpMemPTD;             // Ptr to PerTabData global memory
    LPNFNSHDR         lpNfnsHdr = NULL;     // Ptr to NFNSHDR global memory
    LPNFNSDATA        lpNfnsMem;            // Ptr to aNfnsData
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    NFNS_BUFFER       NfnsBuff;             // Temporary buffer
    OVERLAPPED        overLapped = {0};     // OVERLAPPED struc
    HANDLE            hWaitEvent;           // Wait event handle

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RIGHT RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Check for RIGHT DOMAIN ERROR
    if (!(IsNumeric (aplTypeRht)
       || IsNested  (aplTypeRht)))
        goto RIGHT_DOMAIN_EXIT;

    // Check for RIGHT LENGTH ERROR
    if (1 > aplNELMRht
     ||     aplNELMRht > 4)
        goto RIGHT_LENGTH_EXIT;

    // Is the file offset specified?
    bFileOffset = (aplNELMRht > 3);

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get right arg's global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // If the right arg is a global, ...
    if (hGlbRht NE NULL)
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    else
        lpMemRht = &aplLongestRht;

    // Validate the tie number
    if (!NfnsArgTieNum (aplTypeRht, lpMemRht, aplNELMRht, 0, &TieNum, lptkFunc, lpMemPTD))
        goto RIGHT_DOMAIN_EXIT;

    // Check for duplicate tie number
    switch (IsDuplicateTieNum_EM (TieNum, lptkFunc, lpMemPTD, &uTie))
    {
        case -1:
            goto ERROR_EXIT;

        case FALSE:
            goto RIGHT_DOMAIN_EXIT;

        case TRUE:
            break;

        defstop
            break;
    } // End SWITCH

    // Lock the memory to get a ptr to it
    lpNfnsHdr = InitLockNfns (lptkFunc, lpMemPTD);

    // Point to the matching tie number entry
    lpNfnsMem = &lpNfnsHdr->aNfnsData[uTie];

    // Validate the conversion
    if (!NfnsArgConv   (aplTypeRht, lpMemRht, aplNELMRht, 1, FALSE, &DiskConv, lpNfnsMem->DiskConv, &WsConv, lpNfnsMem->WsConv, &aplTypeWs))
        goto RIGHT_DOMAIN_EXIT;

    // Get the file size
    LOAPLINT (aplFileSize) =
      GetFileSize (lpNfnsMem->hFile,            // File handle
                  &HIAPLINT (aplFileSize));     // High-order file size

    // Check for error
    if (LOAPLINT (aplFileSize) EQ INVALID_FILE_SIZE
     && GetLastError () NE NO_ERROR)
    {
        // Format a system error message
        SysErrMsg_EM (GetLastError (), lptkRhtArg);

        goto ERROR_EXIT;
    } // End IF

    // If there's a file offset, ...
    if (bFileOffset)
    {
        // Parse FileOffset from lpMemRht
        if (!NfnsArgAplint (aplTypeRht,     // Arg storage type
                            lpMemRht,       // Ptr to global memory data
                            aplNELMRht,     // NELM of arg
                            3,              // Index # into lpMem
                           &aplFileOff))    // Ptr to result APLINT
            goto RIGHT_DOMAIN_EXIT;
        // If the file offset is the current offset, ...
        if (aplFileOff EQ -1)
            bFileOffset = FALSE;
        else
        // Set the file pointer to the value just parsed
        LOAPLINT (aplFileOff) =
          SetFilePointer (lpNfnsMem->hFile,
                          LOAPLINT (aplFileOff),
                         &HIAPLINT (aplFileOff),
                          FILE_BEGIN);
    } // End IF

    // If there's no file offset, ...
    if (!bFileOffset)
    {
        // Initialize the file offset
        aplFileOff = 0;

        // Set the file pointer to the current file offset
        LOAPLINT (aplFileOff) =
          SetFilePointer (lpNfnsMem->hFile,
                          LOAPLINT (aplFileOff),
                         &HIAPLINT (aplFileOff),
                          FILE_CURRENT);
    } // End IF/ELSE

    // Initialize the OVERLAPPED struc with the file offset
    overLapped.Offset     = LOAPLINT (aplFileOff);
    overLapped.OffsetHigh = HIAPLINT (aplFileOff);

    // Create an event
    overLapped.hEvent =
      CreateEvent (NULL,                        // Ptr to security attributes (may be NULL)
                   TRUE,                        // TRUE iff manual-reset timer
                   FALSE,                       // TRUE if initial state is signalled
                   NULL);                       // Ptr to event name (may be NULL)
    if (overLapped.hEvent EQ NULL)
        goto EVENT_EXIT;

    // Calculate the default result NELM
    // Split cases based upon the disk conversion code
    switch (DiskConv)
    {
        case DR_BOOL:
            aplNELMRes = 8 * (aplFileSize - aplFileOff);

            break;

        case DR_CHAR8:
        case DR_INT8:
            aplNELMRes = ((aplFileSize - aplFileOff) + (1 - 1)) / 1;

            break;

        case DR_CHAR16:
        case DR_INT16:
            aplNELMRes = ((aplFileSize - aplFileOff) + (2 - 1)) / 2;

            break;

        case DR_CHAR32:
        case DR_INT32:
            aplNELMRes = ((aplFileSize - aplFileOff) + (4 - 1)) / 4;

            break;

        case DR_INT64:
        case DR_FLOAT:
            aplNELMRes = ((aplFileSize - aplFileOff) + (8 - 1)) / 8;

            break;

        defstop
            break;
    } // End SWITCH

    // Parse NELM from lpMemRht
    if (!NfnsArgAplint (aplTypeRht,     // Arg storage type
                        lpMemRht,       // Ptr to global memory data
                        aplNELMRht,     // NELM of arg
                        2,              // Index # into lpMem
                       &aplNELMRes))    // Ptr to result APLINT
        goto RIGHT_DOMAIN_EXIT;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeWs, aplNELMRes, 1);

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
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeWs;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemHdrRes);

    // Save the dimension
    *((LPAPLDIM) lpMemRes)++ = aplNELMRes;

    // Read and translate the data from the file

#define LIMIT_READ_IN(DstCast,SrcBuff,minval,maxval,SrcType,GetNext,ArrType)                    \
                    /* Loop through the chunks of the arg */                                    \
                    while (aplNELMRes)                                                          \
                    {                                                                           \
                        SrcType aplTemp;                                                        \
                                                                                                \
                        /* Get # source elements to handle in this chunk */                     \
                        uMin = min (countof (NfnsBuff.SrcBuff), aplNELMRes);                    \
                                                                                                \
                        /* Calculate the # bytes to read in */                                  \
                        dwRead = (DWORD) (uMin * sizeof (NfnsBuff.SrcBuff[0]));                 \
                                                                                                \
                        /* Read in the next chunk of data */                                    \
                        if (!ReadFile (lpNfnsMem->hFile,    /* File Handle             */       \
                                       NfnsBuff.SrcBuff,    /* Ptr to buffer           */       \
                                       dwRead,              /* # bytes to read         */       \
                                       NULL,                /* # bytes read            */       \
                                      &overLapped))         /* Ptr to OVERLAPPED struc */       \
                        {                                                                       \
                            /* Get the last error code */                                       \
                            dwErr = GetLastError ();                                            \
                                                                                                \
                            /* If not still doing asynchronous I/O, ... */                      \
                            if (dwErr NE ERROR_IO_PENDING)                                      \
                                goto SYS_ERROR_EXIT;                                            \
                        } /* End IF */                                                          \
                                                                                                \
                        /* Check for previous uncleared event */                                \
                        Assert (lpMemPTD->hWaitEvent EQ NULL);                                  \
                                                                                                \
                        /* Save the wait event in case the user Ctrl-Breaks */                  \
                        lpMemPTD->hWaitEvent = overLapped.hEvent;                               \
                                                                                                \
                        /* Wait for the I/O complete */                                         \
                        dwRet = WaitForSingleObject (overLapped.hEvent, INFINITE);              \
                                                                                                \
                        /* Save and clear the wait event                                        \
                            so as to test for Ctrl-Break signalling the event */                \
                        hWaitEvent = lpMemPTD->hWaitEvent;                                      \
                        lpMemPTD->hWaitEvent = NULL;                                            \
                                                                                                \
                        /* If the read is incomplete, ... */                                    \
                        if (hWaitEvent EQ NULL          /* Ctrl-Break     */                    \
                         || dwRet NE WAIT_OBJECT_0)     /* Incomplete I/O */                    \
                            goto INCOMPLETE_EXIT;                                               \
                                                                                                \
                        /* Get # bytes read */                                                  \
                        GetOverlappedResult (lpNfnsMem->hFile,  /* File handle              */  \
                                            &overLapped,        /* Ptr to OVERLAPPED struc  */  \
                                            &dwRead,            /* Ptr to # bytes read      */  \
                                             TRUE);             /* TRUE iff the function waits  \
                                                                     for I/O completion     */  \
                        /* Loop through the arg */                                              \
                        for (uCnt = 0; uCnt < uMin; uCnt++)                                     \
                        {                                                                       \
                            /* Get the source value */                                          \
                            aplTemp = GetNext (NfnsBuff.SrcBuff, ArrType, uCnt);                \
                                                                                                \
                            /* If it's within range, ... */                                     \
                            if (minval <= aplTemp                                               \
                             &&           aplTemp <= maxval)                                    \
                                *((DstCast *) lpMemRes)++ = (DstCast) aplTemp;                  \
                            else                                                                \
                                goto DOMAIN_EXIT;                                               \
                        } /* End FOR */                                                         \
                                                                                                \
                        /* Remove from NELM */                                                  \
                        aplNELMRes -= uMin;                                                     \
                     /* aplOff     += uMin; */                                                  \
                                                                                                \
                        /* Increment the offset in overLapped for next read */                  \
                        aplFileOff += dwRead;                                                   \
                        overLapped.Offset     = LOAPLINT (aplFileOff);                          \
                        overLapped.OffsetHigh = HIAPLINT (aplFileOff);                          \
                    } /* End WHILE */

#define READ_IN_PROMOTE_FROM_BOOL(DstCast)                                                      \
                    /* Loop through the chunks of the arg */                                    \
                    while (aplNELMRes)                                                          \
                    {                                                                           \
                        APLINT aplTemp;                                                         \
                                                                                                \
                        /* Get # source elements to handle in this chunk */                     \
                        uMin = min (8 * countof (NfnsBuff.Tchar8), aplNELMRes);                 \
                                                                                                \
                        /* Calculate the # bytes to read in */                                  \
                        dwRead = (DWORD) RoundUpBitsToBytes (uMin);                             \
                                                                                                \
                        /* Read in the next chunk of data */                                    \
                        if (!ReadFile (lpNfnsMem->hFile,    /* File Handle             */       \
                                       NfnsBuff.Tchar8,     /* Ptr to buffer           */       \
                                       dwRead,              /* # bytes to read         */       \
                                       NULL,                /* # bytes read            */       \
                                      &overLapped))         /* Ptr to OVERLAPPED struc */       \
                        {                                                                       \
                            /* Get the last error code */                                       \
                            dwErr = GetLastError ();                                            \
                                                                                                \
                            /* If not still doing asynchronous I/O, ... */                      \
                            if (dwErr NE ERROR_IO_PENDING)                                      \
                                goto SYS_ERROR_EXIT;                                            \
                        } /* End IF */                                                          \
                                                                                                \
                        /* Check for previous uncleared event */                                \
                        Assert (lpMemPTD->hWaitEvent EQ NULL);                                  \
                                                                                                \
                        /* Save the wait event in case the user Ctrl-Breaks */                  \
                        lpMemPTD->hWaitEvent = overLapped.hEvent;                               \
                                                                                                \
                        /* Wait for the I/O complete */                                         \
                        dwRet = WaitForSingleObject (overLapped.hEvent, INFINITE);              \
                                                                                                \
                        /* Save and clear the wait event                                        \
                            so as to test for Ctrl-Break signalling the event */                \
                        hWaitEvent = lpMemPTD->hWaitEvent;                                      \
                        lpMemPTD->hWaitEvent = NULL;                                            \
                                                                                                \
                        /* If the read is incomplete, ... */                                    \
                        if (hWaitEvent EQ NULL         /* Ctrl-Break     */                     \
                         || dwRet NE WAIT_OBJECT_0)    /* Incomplete I/O */                     \
                            goto INCOMPLETE_EXIT;                                               \
                                                                                                \
                        /* Get # bytes read */                                                  \
                        GetOverlappedResult (lpNfnsMem->hFile,  /* File handle              */  \
                                            &overLapped,        /* Ptr to OVERLAPPED struc  */  \
                                            &dwRead,            /* Ptr to # bytes read      */  \
                                             TRUE);             /* TRUE iff the function waits  \
                                                                     for I/O completion     */  \
                        /* Loop through the bytes in the arg */                                 \
                        for (uCnt = 0; uCnt < dwRead; uCnt++)                                   \
                            /* Reverse the bytes */                                             \
                            NfnsBuff.Tchar8[uCnt] =                                             \
                              FastBoolTrans[NfnsBuff.Tchar8[uCnt]][FBT_REVERSE];                \
                                                                                                \
                        /* Loop through the arg */                                              \
                        for (uCnt = 0; uCnt < uMin; uCnt++)                                     \
                        {                                                                       \
                            /* Get the source value */                                          \
                            aplTemp = GetNextInteger (NfnsBuff.Tchar8, ARRAY_BOOL, uCnt);       \
                                                                                                \
                            /* If it's within range, ... */                                     \
                            if (IsBooleanValue (aplTemp))                                       \
                                *((DstCast *) lpMemRes)++ = (DstCast) aplTemp;                  \
                            else                                                                \
                                goto DOMAIN_EXIT;                                               \
                        } /* End FOR */                                                         \
                                                                                                \
                        /* Remove from NELM */                                                  \
                        aplNELMRes -= uMin;                                                     \
                     /* aplOff     += uMin; */                                                  \
                                                                                                \
                        /* Increment the offset in overLapped for next read */                  \
                        aplFileOff += dwRead;                                                   \
                        overLapped.Offset     = LOAPLINT (aplFileOff);                          \
                        overLapped.OffsetHigh = HIAPLINT (aplFileOff);                          \
                    } /* End WHILE */

#define READ_IN_DEMOTE_TO_BOOL(DstType,GetNext,ArrType,SrcType)                                 \
                    /* Loop through the chunks of the arg */                                    \
                    while (aplNELMRes)                                                          \
                    {                                                                           \
                        BYTE    uBitCount = 0;                                                  \
                        SrcType aplTemp;                                                        \
                                                                                                \
                        /* Get # source elements to handle in this chunk */                     \
                        uMin = min (countof (NfnsBuff.DstType), aplNELMRes);                    \
                                                                                                \
                        /* Calculate the # bytes to read in */                                  \
                        dwRead = (DWORD) (uMin * sizeof (SrcType));                             \
                                                                                                \
                        /* Read in the next chunk of data */                                    \
                        if (!ReadFile (lpNfnsMem->hFile,    /* File Handle             */       \
                                       NfnsBuff.DstType,    /* Ptr to buffer           */       \
                                       dwRead,              /* # bytes to read         */       \
                                       NULL,                /* # bytes read            */       \
                                      &overLapped))         /* Ptr to OVERLAPPED struc */       \
                        {                                                                       \
                            /* Get the last error code */                                       \
                            dwErr = GetLastError ();                                            \
                                                                                                \
                            /* If not still doing asynchronous I/O, ... */                      \
                            if (dwErr NE ERROR_IO_PENDING)                                      \
                                goto SYS_ERROR_EXIT;                                            \
                        } /* End IF */                                                          \
                                                                                                \
                        /* Check for previous uncleared event */                                \
                        Assert (lpMemPTD->hWaitEvent EQ NULL);                                  \
                                                                                                \
                        /* Save the wait event in case the user Ctrl-Breaks */                  \
                        lpMemPTD->hWaitEvent = overLapped.hEvent;                               \
                                                                                                \
                        /* Wait for the I/O complete */                                         \
                        dwRet = WaitForSingleObject (overLapped.hEvent, INFINITE);              \
                                                                                                \
                        /* Save and clear the wait event                                        \
                            so as to test for Ctrl-Break signalling the event */                \
                        hWaitEvent = lpMemPTD->hWaitEvent;                                      \
                        lpMemPTD->hWaitEvent = NULL;                                            \
                                                                                                \
                        /* If the read is incomplete, ... */                                    \
                        if (hWaitEvent EQ NULL         /* Ctrl-Break     */                     \
                         || dwRet NE WAIT_OBJECT_0)    /* Incomplete I/O */                     \
                            goto INCOMPLETE_EXIT;                                               \
                                                                                                \
                        /* Get # bytes read */                                                  \
                        GetOverlappedResult (lpNfnsMem->hFile,  /* File handle              */  \
                                            &overLapped,        /* Ptr to OVERLAPPED struc  */  \
                                            &dwRead,            /* Ptr to # bytes read      */  \
                                             TRUE);             /* TRUE iff the function waits  \
                                                                     for I/O completion     */  \
                        /* Loop through the arg */                                              \
                        for (uCnt = 0; uCnt < uMin; uCnt++)                                     \
                        {                                                                       \
                            /* Get the source value */                                          \
                            aplTemp = GetNext (NfnsBuff.DstType, ArrType, uCnt);                \
                                                                                                \
                            /* If it's within range, ... */                                     \
                            if (IsBooleanValue (aplTemp))                                       \
                                ((LPAPLBOOL) lpMemRes)[(aplOff + uCnt) >> LOG2NBIB] |=          \
                                  (aplTemp EQ 1) << (uBitCount++ % NBIB);                       \
                            else                                                                \
                                goto DOMAIN_EXIT;                                               \
                        } /* End FOR */                                                         \
                                                                                                \
                        /* Remove from NELM */                                                  \
                        aplNELMRes -= uMin;                                                     \
                        aplOff     += uMin;                                                     \
                                                                                                \
                        /* Increment the offset in overLapped for next read */                  \
                        aplFileOff += dwRead;                                                   \
                        overLapped.Offset     = LOAPLINT (aplFileOff);                          \
                        overLapped.OffsetHigh = HIAPLINT (aplFileOff);                          \
                    } /* End WHILE */

    // Split cases based upon the destination storage format
    switch (aplTypeWs)
    {
        case ARRAY_BOOL:                // Read -- Dest = BOOL
            // Split cases based upon the source storage value
            switch (DiskConv)
            {
                case DR_BOOL:           // Read -- Dest = BOOL, Src = BOOL
                    // Calculate the # bytes to read
                    uMin = RoundUpBitsToBytes (aplNELMRes);
                    dwRead = (DWORD) uMin;

                    // Read in the entire amount
                    if (!ReadFile (lpNfnsMem->hFile,    /* File Handle             */
                                   lpMemRes,            /* Ptr to buffer           */
                                   dwRead,              /* # bytes to read         */
                                   NULL,                /* # bytes read            */
                                  &overLapped))         /* Ptr to OVERLAPPED struc */
                    {
                        /* Get the last error code */
                        dwErr = GetLastError ();

                        /* If not still doing asynchronous I/O, ... */
                        if (dwErr NE ERROR_IO_PENDING)
                            goto SYS_ERROR_EXIT;
                    } /* End IF */

                    /* Check for previous uncleared event */
                    Assert (lpMemPTD->hWaitEvent EQ NULL);

                    /* Save the wait event in case the user Ctrl-Breaks */
                    lpMemPTD->hWaitEvent = overLapped.hEvent;

                    /* Wait for the I/O complete */
                    dwRet = WaitForSingleObject (overLapped.hEvent, INFINITE);

                    /* Save and clear the wait event
                        so as to test for Ctrl-Break signalling the event */
                    hWaitEvent = lpMemPTD->hWaitEvent;
                    lpMemPTD->hWaitEvent = NULL;

                    /* If the read is incomplete, ... */
                    if (hWaitEvent EQ NULL          /* Ctrl-Break     */
                     || dwRet NE WAIT_OBJECT_0)     /* Incomplete I/O */
                        goto INCOMPLETE_EXIT;

                    /* Get # bytes read */
                    GetOverlappedResult (lpNfnsMem->hFile,  /* File handle                  */
                                        &overLapped,        /* Ptr to OVERLAPPED struc      */
                                        &dwRead,            /* Ptr to # bytes read          */
                                         TRUE);             /* TRUE iff the function waits
                                                                 for I/O completion         */
                    // Loop through the result reversing the bits in each byte
                    for (uCnt = 0; uCnt < uMin; uCnt++)
                        *((LPAPLBOOL) lpMemRes)++ = FastBoolTrans[*(LPAPLBOOL) lpMemRes][FBT_REVERSE];
                    break;

                case DR_CHAR8:          // Read -- Dest = BOOL, Src = CHAR8
                    READ_IN_DEMOTE_TO_BOOL
                      (Tchar8,          // Destination type (NfnsBuff.DstType)
                       GetNextChar8,    // Source GetNext routine
                       0,               // ...    array type (ignored in this case)
                       CHAR8)           // ...    type
                    break;

                case DR_CHAR16:         // Read -- Dest = BOOL, Src = CHAR16
                    READ_IN_DEMOTE_TO_BOOL
                      (Tchar16,         // Destination type (NfnsBuff.DstType)
                       GetNextChar16,   // Source GetNext routine
                       0,               // ...    array type (ignored in this case)
                       CHAR16)          // ...    type
                    break;

                case DR_CHAR32:         // Read -- Dest = BOOL, Src = CHAR32
                    READ_IN_DEMOTE_TO_BOOL
                      (Tchar32,         // Destination type (NfnsBuff.DstType)
                       GetNextChar32,   // Source GetNext routine
                       0,               // ...    array type (ignored in this case)
                       CHAR32)          // ...    type
                    break;

                case DR_INT8:           // Read -- Dest = BOOL, Src = INT8
                    READ_IN_DEMOTE_TO_BOOL
                      (Tint8,           // Destination type (NfnsBuff.DstType)
                       GetNextInt8,     // Source GetNext routine
                       0,               // ...    array type (ignored in this case)
                       char)            // ...    type
                    break;

                case DR_INT16:          // Read -- Dest = BOOL, Src = INT16
                    READ_IN_DEMOTE_TO_BOOL
                      (Tint16,          // Destination type (NfnsBuff.DstType)
                       GetNextInt16,    // Source GetNext routine
                       0,               // ...    array type (ignored in this case)
                       short)           // ...    type
                    break;

                case DR_INT32:          // Read -- Dest = BOOL, Src = INT32
                    READ_IN_DEMOTE_TO_BOOL
                      (Tint32,          // Destination type (NfnsBuff.DstType)
                       GetNextInt32,    // Source GetNext routine
                       0,               // ...    array type (ignored in this case)
                       int)             // ...    type
                    break;

                case DR_INT64:          // Read -- Dest = BOOL, Src = INT64
                    READ_IN_DEMOTE_TO_BOOL
                      (Tint64,          // Destination type (NfnsBuff.DstType)
                       GetNextInt64,    // Source GetNext routine
                       0,               // ...    array type (ignored in this case)
                       INT64)           // ...    type
                    break;

                case DR_FLOAT:          // Read -- Dest = BOOL, Src = FLOAT
                    READ_IN_DEMOTE_TO_BOOL
                      (Tflt64,          // Destination type (NfnsBuff.DstType)
                       GetNextFlt64,    // Source GetNext routine
                       0,               // ...    array type (ignored in this case)
                       FLT64)           // ...    type
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_INT:                 // Read -- Dest = INT
            // Split cases based upon the source storage value
            switch (DiskConv)
            {
                case DR_BOOL:           // Read -- Dest = INT, Src = BOOL
                    READ_IN_PROMOTE_FROM_BOOL (APLINT)      // Destination cast

                    break;

                case DR_CHAR8:          // Read -- Dest = INT, Src = CHAR8
                    LIMIT_READ_IN (APLINT,          // Destination Cast
                                   Tchar8,          // Source buffer (NfnsBuff.DstType)
                                   0,               // ...    minimum
                                   UINT8_MAX,       // ...    maximum
                                   UCHAR,           // ...    type
                                   GetNextChar8,    // ...    GetNext routine
                                   0)               // ...    array type (ignored in this case)
                    break;

                case DR_CHAR16:         // Read -- Dest = INT, Src = CHAR16
                    LIMIT_READ_IN (APLINT,          // Destination Cast
                                   Tchar16,         // Source buffer (NfnsBuff.DstType)
                                   0,               // ...    minimum
                                   UINT16_MAX,      // ...    maximum
                                   WCHAR,           // ...    type
                                   GetNextChar16,   // ...    GetNext routine
                                   0)               // ...    array type (ignored in this case)
                    break;

                case DR_CHAR32:         // Read -- Dest = INT, Src = CHAR32
                    LIMIT_READ_IN (APLINT,          // Destination Cast
                                   Tchar32,         // Source buffer (NfnsBuff.DstType)
                                   0,               // ...    minimum
                                   UINT32_MAX,      // ...    maximum
                                   UINT32,          // ...    type
                                   GetNextChar32,   // ...    GetNext routine
                                   0)               // ...    array type (ignored in this case)
                    break;

                case DR_INT8:           // Read -- Dest = INT, Src = INT8
                    LIMIT_READ_IN (APLINT,          // Destination Cast
                                   Tint8,           // Source buffer (NfnsBuff.DstType)
                                   INT8_MIN,        // ...    minimum
                                   INT8_MAX,        // ...    maximum
                                   INT8,            // ...    type
                                   GetNextInt8,     // ...    GetNext routine
                                   0)               // ...    array type (ignored in this case)
                    break;

                case DR_INT16:          // Read -- Dest = INT, Src = INT16
                    LIMIT_READ_IN (APLINT,          // Destination Cast
                                   Tint16,          // Source buffer (NfnsBuff.DstType)
                                   INT16_MIN,       // ...    minimum
                                   INT16_MAX,       // ...    maximum
                                   INT16,           // ...    type
                                   GetNextInt16,    // ...    GetNext routine
                                   0)               // ...    array type (ignored in this case)
                    break;

                case DR_INT32:          // Read -- Dest = INT, Src = INT32
                    LIMIT_READ_IN (APLINT,          // Destination Cast
                                   Tint32,          // Source buffer (NfnsBuff.DstType)
                                   INT32_MIN,       // ...    minimum
                                   INT32_MAX,       // ...    maximum
                                   INT32,           // ...    type
                                   GetNextInt32,    // ...    GetNext routine
                                   0)               // ...    array type (ignored in this case)
                    break;

                case DR_INT64:          // Read -- Dest = INT, Src = INT64
                    // Calculate the # bytes to read
                    dwRead = (DWORD) (aplNELMRes * sizeof (APLINT));

                    // Read in the entire amount
                    if (!ReadFile (lpNfnsMem->hFile,    /* File Handle             */
                                   lpMemRes,            /* Ptr to buffer           */
                                   dwRead,              /* # bytes to read         */
                                   NULL,                /* # bytes read            */
                                  &overLapped))         /* Ptr to OVERLAPPED struc */
                    {
                        /* Get the last error code */
                        dwErr = GetLastError ();

                        /* If not still doing asynchronous I/O, ... */
                        if (dwErr NE ERROR_IO_PENDING)
                            goto SYS_ERROR_EXIT;
                    } /* End IF */

                    /* Check for previous uncleared event */
                    Assert (lpMemPTD->hWaitEvent EQ NULL);

                    /* Save the wait event in case the user Ctrl-Breaks */
                    lpMemPTD->hWaitEvent = overLapped.hEvent;

                    /* Wait for the I/O complete */
                    dwRet = WaitForSingleObject (overLapped.hEvent, INFINITE);

                    /* Save and clear the wait event
                        so as to test for Ctrl-Break signalling the event */
                    hWaitEvent = lpMemPTD->hWaitEvent;
                    lpMemPTD->hWaitEvent = NULL;

                    /* If the read is incomplete, ... */
                    if (hWaitEvent EQ NULL          /* Ctrl-Break     */
                     || dwRet NE WAIT_OBJECT_0)     /* Incomplete I/O */
                        goto INCOMPLETE_EXIT;

                    /* Get # bytes read */
                    GetOverlappedResult (lpNfnsMem->hFile,  /* File handle                  */
                                        &overLapped,        /* Ptr to OVERLAPPED struc      */
                                        &dwRead,            /* Ptr to # bytes read          */
                                         TRUE);             /* TRUE iff the function waits
                                                                 for I/O completion         */
                    break;

                case DR_FLOAT:          // Read -- Dest = INT, Src = FLOAT
                    LIMIT_READ_IN (APLINT,          // Destination Cast
                                   Tflt64,          // Source buffer (NfnsBuff.DstType)
                                   FLT64_MIN,       // ...    minimum
                                   FLT64_MAX,       // ...    maximum
                                   FLT64,           // ...    type
                                   GetNextFlt64,    // ...    GetNext routine
                                   0)               // ...    array type (ignored in this case)
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_CHAR:                // Read -- Dest = CHAR
            // Split cases based upon the source storage value
            switch (DiskConv)
            {
                case DR_BOOL:           // Read -- Dest = CHAR, Src = BOOL
                    READ_IN_PROMOTE_FROM_BOOL (APLCHAR)     // Destination Cast

                    break;

                case DR_CHAR8:          // Read -- Dest = CHAR, Src = CHAR8
                    LIMIT_READ_IN (APLCHAR,         // Destination Cast
                                   Tchar8,          // Source buffer (NfnsBuff.DstType)
                                   0,               // ...    minimum
                                   UINT8_MAX,       // ...    maximum
                                   UINT8,           // ...    type
                                   GetNextChar8,    // ...    GetNext routine
                                   0)               // ...    array type (ignored in this case)
                    break;

                case DR_INT8:           // Read -- Dest = CHAR, Src = INT8
                    LIMIT_READ_IN (APLCHAR,         // Destination Cast
                                   Tint8,           // Source buffer (NfnsBuff.DstType)
                                   INT8_MIN,        // ...    minimum
                                   INT8_MAX,        // ...    maximum
                                   INT8,            // ...    type
                                   GetNextInt8,     // ...    GetNext routine
                                   0)               // ...    array type (ignored in this case)
                    break;

                case DR_CHAR16:         // Read -- Dest = CHAR, Src = CHAR16
                case DR_INT16:          // Read -- Dest = CHAR, Src = INT16
                    // Calculate the # bytes to read
                    dwRead = (DWORD) (aplNELMRes * sizeof (APLCHAR));

                    // Read in the entire amount
                    if (!ReadFile (lpNfnsMem->hFile,    /* File Handle             */
                                   lpMemRes,            /* Ptr to buffer           */
                                   dwRead,              /* # bytes to read         */
                                   NULL,                /* # bytes read            */
                                  &overLapped))         /* Ptr to OVERLAPPED struc */
                    {
                        /* Get the last error code */
                        dwErr = GetLastError ();

                        /* If not still doing asynchronous I/O, ... */
                        if (dwErr NE ERROR_IO_PENDING)
                            goto SYS_ERROR_EXIT;
                    } /* End IF */

                    /* Check for previous uncleared event */
                    Assert (lpMemPTD->hWaitEvent EQ NULL);

                    /* Save the wait event in case the user Ctrl-Breaks */
                    lpMemPTD->hWaitEvent = overLapped.hEvent;

                    /* Wait for the I/O complete */
                    dwRet = WaitForSingleObject (overLapped.hEvent, INFINITE);

                    /* Save and clear the wait event
                        so as to test for Ctrl-Break signalling the event */
                    hWaitEvent = lpMemPTD->hWaitEvent;
                    lpMemPTD->hWaitEvent = NULL;

                    /* If the read is incomplete, ... */
                    if (hWaitEvent EQ NULL          /* Ctrl-Break     */
                     || dwRet NE WAIT_OBJECT_0)     /* Incomplete I/O */
                        goto INCOMPLETE_EXIT;

                    /* Get # bytes read */
                    GetOverlappedResult (lpNfnsMem->hFile,  /* File handle                  */
                                        &overLapped,        /* Ptr to OVERLAPPED struc      */
                                        &dwRead,            /* Ptr to # bytes read          */
                                         TRUE);             /* TRUE iff the function waits
                                                                 for I/O completion         */
                    break;

                case DR_CHAR32:         // Read -- Dest = CHAR, Src = CHAR32
                    LIMIT_READ_IN (APLCHAR,         // Destination Cast
                                   Tchar32,         // Source buffer (NfnsBuff.DstType)
                                   0,               // ...    minimum
                                   UINT32_MAX,      // ...    maximum
                                   UINT32,          // ...    type
                                   GetNextChar32,   // ...    GetNext routine
                                   0)               // ...    array type (ignored in this case)
                    break;

                case DR_INT32:          // Read -- Dest = CHAR, Src = INT32
                    LIMIT_READ_IN (APLCHAR,         // Destination Cast
                                   Tint32,          // Source buffer (NfnsBuff.DstType)
                                   INT32_MIN,       // ...    minimum
                                   INT32_MAX,       // ...    maximum
                                   INT32,           // ...    type
                                   GetNextInt32,    // ...    GetNext routine
                                   0)               // ...    array type (ignored in this case)
                    break;

                case DR_INT64:          // Read -- Dest = CHAR, Src = INT64
                    LIMIT_READ_IN (APLCHAR,         // Destination Cast
                                   Tint64,          // Source buffer (NfnsBuff.DstType)
                                   0,               // ...    minimum
                                   UINT16_MAX,      // ...    maximum
                                   APLINT,          // ...    type
                                   GetNextInt64,    // ...    GetNext routine
                                   0)               // ...    array type (ignored in this case)
                    break;

                case DR_FLOAT:          // Read -- Dest = CHAR, Src = FLOAT
                    LIMIT_READ_IN (APLCHAR,         // Destination Cast
                                   Tflt64,          // Source buffer (NfnsBuff.DstType)
                                   FLT64_MIN,       // ...    minimum
                                   FLT64_MAX,       // ...    maximum
                                   FLT64,           // ...    type
                                   GetNextFlt64,    // ...    GetNext routine
                                   0)               // ...    array type (ignored in this case)
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_FLOAT:               // Read -- Dest = FLOAT
            // Split cases based upon the source storage value
            switch (DiskConv)
            {
                case DR_BOOL:           // Read -- Dest = FLOAT, Src = BOOL
                    READ_IN_PROMOTE_FROM_BOOL (FLT64)       // Destination Cast

                    break;

                case DR_CHAR8:          // Read -- Dest = FLOAT, Src = CHAR8
                    goto RIGHT_DOMAIN_EXIT;

////////////////////LIMIT_READ_IN (FLT64,           // Destination Cast
////////////////////               Tchar8,          // Source buffer (NfnsBuff.DstType)
////////////////////               0,               // ...    minimum
////////////////////               UINT8_MAX,       // ...    maximum
////////////////////               APLINT,          // ...    type
////////////////////               GetNextChar8,    // ...    GetNext routine
////////////////////               0)               // ...    array type (ignored in this case)
                    break;

                case DR_CHAR16:         // Read -- Dest = FLOAT, Src = CHAR16
                    goto RIGHT_DOMAIN_EXIT;

////////////////////LIMIT_READ_IN (FLT64,           // Destination Cast
////////////////////               Tchar16,         // Source buffer (NfnsBuff.DstType)
////////////////////               0,               // ...    minimum
////////////////////               UINT16_MAX,      // ...    maximum
////////////////////               APLINT,          // ...    type
////////////////////               GetNextChar16,   // ...    GetNext routine
////////////////////               0)               // ...    array type (ignored in this case)
                    break;

                case DR_CHAR32:         // Read -- Dest = FLOAT, Src = CHAR32
                    goto RIGHT_DOMAIN_EXIT;

////////////////////LIMIT_READ_IN (FLT64,           // Destination Cast
////////////////////               Tchar32,         // Source buffer (NfnsBuff.DstType)
////////////////////               0,               // ...    minimum
////////////////////               UINT32_MAX,      // ...    maximum
////////////////////               APLINT,          // ...    type
////////////////////               GetNextChar32,   // ...    GetNext routine
////////////////////               0)               // ...    array type (ignored in this case)
                    break;

                case DR_INT8:           // Read -- Dest = FLOAT, Src = INT8
                    goto RIGHT_DOMAIN_EXIT;

////////////////////LIMIT_READ_IN (FLT64,           // Destination Cast
////////////////////               Tint8,           // Source buffer (NfnsBuff.DstType)
////////////////////               INT8_MIN,        // ...    minimum
////////////////////               INT8_MAX,        // ...    maximum
////////////////////               APLINT,          // ...    type
////////////////////               GetNextInt8,     // ...    GetNext routine
////////////////////               0)               // ...    array type (ignored in this case)
                    break;

                case DR_INT16:          // Read -- Dest = FLOAT, Src = INT16
                    goto RIGHT_DOMAIN_EXIT;

////////////////////LIMIT_READ_IN (FLT64,           // Destination Cast
////////////////////               Tint16,          // Source buffer (NfnsBuff.DstType)
////////////////////               INT16_MIN,       // ...    minimum
////////////////////               INT16_MAX,       // ...    maximum
////////////////////               APLINT,          // ...    type
////////////////////               GetNextInt16,    // ...    GetNext routine
////////////////////               0)               // ...    array type (ignored in this case)
                    break;

                case DR_INT32:          // Read -- Dest = FLOAT, Src = INT32
                    goto RIGHT_DOMAIN_EXIT;

////////////////////LIMIT_READ_IN (FLT64,           // Destination Cast
////////////////////               Tint32,          // Source buffer (NfnsBuff.DstType)
////////////////////               INT16_MIN,       // ...    minimum
////////////////////               INT16_MAX,       // ...    maximum
////////////////////               APLINT,          // ...    type
////////////////////               GetNextInt16,    // ...    GetNext routine
////////////////////               0)               // ...    array type (ignored in this case)
                    break;

                case DR_INT64:          // Read -- Dest = FLOAT, Src = INT64
                    goto RIGHT_DOMAIN_EXIT;

                case DR_FLOAT:          // Read -- Dest = FLOAT, Src = FLOAT
                    // Calculate the # bytes to read
                    dwRead = (DWORD) (aplNELMRes * sizeof (FLT64));

                    // Read in the entire amount
                    if (!ReadFile (lpNfnsMem->hFile,    /* File Handle             */
                                   lpMemRes,            /* Ptr to buffer           */
                                   dwRead,              /* # bytes to read         */
                                   NULL,                /* # bytes read            */
                                  &overLapped))         /* Ptr to OVERLAPPED struc */
                    {
                        /* Get the last error code */
                        dwErr = GetLastError ();

                        /* If not still doing asynchronous I/O, ... */
                        if (dwErr NE ERROR_IO_PENDING)
                            goto SYS_ERROR_EXIT;
                    } /* End IF */

                    /* Check for previous uncleared event */
                    Assert (lpMemPTD->hWaitEvent EQ NULL);

                    /* Save the wait event in case the user Ctrl-Breaks */
                    lpMemPTD->hWaitEvent = overLapped.hEvent;

                    /* Wait for the I/O complete */
                    dwRet = WaitForSingleObject (overLapped.hEvent, INFINITE);

                    /* Save and clear the wait event
                        so as to test for Ctrl-Break signalling the event */
                    hWaitEvent = lpMemPTD->hWaitEvent;
                    lpMemPTD->hWaitEvent = NULL;

                    /* If the read is incomplete, ... */
                    if (hWaitEvent EQ NULL          /* Ctrl-Break     */
                     || dwRet NE WAIT_OBJECT_0)     /* Incomplete I/O */
                        goto INCOMPLETE_EXIT;

                    /* Get # bytes read */
                    GetOverlappedResult (lpNfnsMem->hFile,  /* File handle                  */
                                        &overLapped,        /* Ptr to OVERLAPPED struc      */
                                        &dwRead,            /* Ptr to # bytes read          */
                                         TRUE);             /* TRUE iff the function waits
                                                                 for I/O completion         */
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        defstop
            break;
    } // End SWITCH

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

EVENT_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LIMIT_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

INCOMPLETE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_INCOMPLETE_IO APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

SYS_ERROR_EXIT:
    SysErrMsg_EM (GetLastError (), lptkFunc);

    goto ERROR_EXIT;

ERROR_EXIT:
    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbRes, lpMemHdrRes);
NORMAL_EXIT:
    if (overLapped.hEvent NE NULL)
    {
        // We no longer need this resource
        CloseHandle (overLapped.hEvent); overLapped.hEvent = NULL;
    } // End IF

    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    if (lpNfnsHdr NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lpMemPTD->hGlbNfns); lpNfnsHdr = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnMonNREAD_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydNREAD_EM_YY
//
//  Dyadic []NREAD -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydNREAD_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydNREAD_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End SysFnDydNREAD_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnNRENAME_EM_YY
//
//  System function:  []NRENAME -- Rename an open native file
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnNRENAME_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnNRENAME_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonNRENAME_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydNRENAME_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnNRENAME_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonNRENAME_EM_YY
//
//  Monadic []NRENAME -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonNRENAME_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonNRENAME_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End SysFnMonNRENAME_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydNRENAME_EM_YY
//
//  Dyadic []NRENAME -- Rename an open native file
//
//  FileName []NRENAME tn
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydNRENAME_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydNRENAME_EM_YY
    (LPTOKEN lptkLftArg,                    // Ptr to left arg token
     LPTOKEN lptkFunc,                      // Ptr to function token
     LPTOKEN lptkRhtArg,                    // Ptr to right arg token
     LPTOKEN lptkAxis)                      // Ptr to axis token (may be NULL)

{
    return
      NfnsEraseRenameResize_EM (lptkLftArg,
                                lptkFunc,
                                lptkRhtArg,
                                lptkAxis,
                                COM_NRENAME,
                                NfnsRename);
} // End SysFnDydNRENAME_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnNREPLACE_EM_YY
//
//  System function:  []NREPLACE -- Replace data in an open native file
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnNREPLACE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnNREPLACE_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonNREPLACE_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydNREPLACE_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnNREPLACE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonNREPLACE_EM_YY
//
//  Monadic []NREPLACE -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonNREPLACE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonNREPLACE_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End SysFnMonNREPLACE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydNREPLACE_EM_YY
//
//  Dyadic []NREPLACE -- Replace data in an open native file
//
//  Data []NREPLACE tn [DiskConv [Start]]
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydNREPLACE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydNREPLACE_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeLft,           // Left arg storage type
                      aplTypeRht;           // Right ...
    APLNELM           aplNELMLft,           // Left arg NELM
                      aplNELMRht;           // Right ...
    APLRANK           aplRankLft,           // Left arg Rank
                      aplRankRht;           // Right ...
    APLLONGEST        aplLongestLft,        // Left arg longest if immediate
                      aplLongestRht;        // Right ...
    HGLOBAL           hGlbLft = NULL,       // Left arg global memory handle
                      hGlbRht = NULL;       // Right ...
    LPVARARRAY_HEADER lpMemHdrLft = NULL,   // Ptr to left arg header
                      lpMemHdrRht = NULL;   // ...    right ...
    LPAPLCHAR         lpMemLft;             // Ptr to left arg global memory
    LPVOID            lpMemRht;             // Ptr to right ...
    APLINT            TieNum,               // File tie number
                      aplFileOff,           // Starting offset of read
                      aplFileOut;           // # bytes written to disk
    DR_VAL            DiskConv;             // Disk format
    UINT              uTie;                 // Offset of matching tie number entry
    UBOOL             bFileOffset;          // TRUE iff the file offset is specified
    LPPERTABDATA      lpMemPTD;             // Ptr to PerTabData global memory
    LPNFNSHDR         lpNfnsHdr = NULL;     // Ptr to NFNSHDR global memory
    LPNFNSDATA        lpNfnsMem;            // Ptr to aNfnsData
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to result
    OVERLAPPED        overLapped = {0};     // OVERLAPPED struc

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RIGHT RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Check for RIGHT LENGTH ERROR
    if (1 > aplNELMRht
     ||     aplNELMRht > 3)
        goto RIGHT_LENGTH_EXIT;

    // Is the file offset specified?
    bFileOffset = (aplNELMRht > 2);

    // Check for LEFT DOMAIN ERROR
    if (!IsSimpleNH (aplTypeLft))
        goto LEFT_DOMAIN_EXIT;

    // Check for RIGHT DOMAIN ERROR
    if (!(IsNumeric (aplTypeRht)
       || IsNested  (aplTypeRht)))
        goto RIGHT_DOMAIN_EXIT;

    // Lock the memory to get a ptr to it
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // If the left arg is a global, ...
    if (hGlbLft NE NULL)
        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
    else
        lpMemLft = (LPAPLCHAR) &aplLongestLft;

    // If the right arg is a global, ...
    if (hGlbRht NE NULL)
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    else
        lpMemRht = &aplLongestRht;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Validate the tie number
    if (!NfnsArgTieNum (aplTypeRht, lpMemRht, aplNELMRht, 0, &TieNum, lptkFunc, lpMemPTD)
     ||  TieNum >= 0)
        goto RIGHT_DOMAIN_EXIT;

    // Check for duplicate tie number
    switch (IsDuplicateTieNum_EM (TieNum, lptkFunc, lpMemPTD, &uTie))
    {
        case -1:
            goto ERROR_EXIT;

        case FALSE:
            goto RIGHT_DOMAIN_EXIT;

        case TRUE:
            break;

        defstop
            break;
    } // End SWITCH

    // Lock the memory to get a ptr to it
    lpNfnsHdr = InitLockNfns (lptkFunc, lpMemPTD);

    // Point to the matching tie number entry
    lpNfnsMem = &lpNfnsHdr->aNfnsData[uTie];

    // Validate the conversion
    if (!NfnsArgConv   (aplTypeRht, lpMemRht, aplNELMRht, 1, TRUE, &DiskConv, lpNfnsMem->DiskConv, NULL, 0, NULL))
        goto RIGHT_DOMAIN_EXIT;

    // If there's a file offset, ...
    if (bFileOffset)
    {
        // Parse FileOffset from lpMemRht
        if (!NfnsArgAplint (aplTypeRht,     // Arg storage type
                            lpMemRht,       // Ptr to global memory data
                            aplNELMRht,     // NELM of arg
                            2,              // Index # into lpMem
                           &aplFileOff))    // Ptr to result APLINT
            goto RIGHT_DOMAIN_EXIT;
        // If the file offset is the current offset, ...
        if (aplFileOff EQ -1)
            bFileOffset = FALSE;
        else
        // Set the file pointer to the value just parsed
        LOAPLINT (aplFileOff) =
          SetFilePointer (lpNfnsMem->hFile,
                          LOAPLINT (aplFileOff),
                         &HIAPLINT (aplFileOff),
                          FILE_BEGIN);
    } // End IF

    // If there's no file offset, ...
    if (!bFileOffset)
    {
        // Initialize the file offset
        aplFileOff = 0;

        // Set the file pointer to the current file offset
        LOAPLINT (aplFileOff) =
          SetFilePointer (lpNfnsMem->hFile,
                          LOAPLINT (aplFileOff),
                         &HIAPLINT (aplFileOff),
                          FILE_CURRENT);
    } // End IF/ELSE

    // Save the file offset in the OVERLAPPED struc
    overLapped.Offset     = LOAPLINT (aplFileOff);
    overLapped.OffsetHigh = HIAPLINT (aplFileOff);

    // Create an event
    overLapped.hEvent =
      CreateEvent (NULL,                        // Ptr to security attributes (may be NULL)
                   TRUE,                        // TRUE iff manual-reset timer
                   FALSE,                       // TRUE if initial state is signalled
                   NULL);                       // Ptr to event name (may be NULL)
    if (overLapped.hEvent EQ NULL)
        goto EVENT_EXIT;

    // Translate the data to DiskConv format and write it out
    if (!NfnsWriteData_EM (lpNfnsMem->hFile,        // File handle
                           aplTypeLft,              // Left arg storage type
                           aplNELMLft,              // Left arg NELM
                           lpMemLft,                // Ptr to left arg data
                           DiskConv,                // Disk conversion value (DR_xxx)
                          &aplFileOut,              // # bytes written to disk
                          &overLapped,              // Ptr to OVERLAPPED struc
                           lpMemPTD,                // Ptr to PerTabData global memory
                           lptkFunc))               // Ptr to function token
        goto ERROR_EXIT;

    // Skip past the bytes written out
    aplFileOff += aplFileOut;

    // Allocate a new YYRes;
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
    lpYYRes->tkToken.tkFlags.NoDisplay = TRUE;
    lpYYRes->tkToken.tkData.tkInteger  = aplFileOff;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

EVENT_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LIMIT_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (overLapped.hEvent NE NULL)
    {
        // We no longer need this resource
        CloseHandle (overLapped.hEvent); overLapped.hEvent = NULL;
    } // End IF

    if (hGlbLft NE NULL && lpMemHdrLft NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;
    } // End IF

    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    if (lpNfnsHdr NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lpMemPTD->hGlbNfns); lpNfnsHdr = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnDydNREPLACE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnNRESIZE_EM_YY
//
//  System function:  []NRESIZE -- Resize an open native file
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnNRESIZE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnNRESIZE_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonNRESIZE_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydNRESIZE_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnNRESIZE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonNRESIZE_EM_YY
//
//  Monadic []NRESIZE -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonNRESIZE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonNRESIZE_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End SysFnMonNRESIZE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydNRESIZE_EM_YY
//
//  Dyadic []NRESIZE -- Resize an open native file
//
//  NewSize  []NRESIZE tn
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydNRESIZE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydNRESIZE_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return
      NfnsEraseRenameResize_EM (lptkLftArg,
                                lptkFunc,
                                lptkRhtArg,
                                lptkAxis,
                                COM_NRESIZE,
                                NfnsResize);
} // End SysFnDydNRESIZE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnNSIZE_EM_YY
//
//  System function:  []NSIZE -- Get the size of an open native file
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnNSIZE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnNSIZE_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonNSIZE_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydNSIZE_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnNSIZE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonNSIZE_EM_YY
//
//  Monadic []NSIZE -- Get the size of an open native file
//
//  []NSIZE tn
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonNSIZE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonNSIZE_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeRht;           // Right arg storage type
    APLNELM           aplNELMRht;           // Right arg NELM
    APLRANK           aplRankRht;           // Right arg Rank
    APLLONGEST        aplLongestRht;        // Right arg longest if immediate
    HGLOBAL           hGlbRht = NULL;       // Right arg global memory handle
    LPVARARRAY_HEADER lpMemHdrRht = NULL;   // Ptr to right arg header
    LPVOID            lpMemRht;             // Ptr to right arg global memory
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    LPPERTABDATA      lpMemPTD;             // Ptr to PerTabData global memory
    APLINT            TieNum,               // File tie number
                      aplFileSize;          // File size
    UINT              uTie;                 // Offset of matching tie number entry
    LPNFNSHDR         lpNfnsHdr = NULL;     // Ptr to NFNSHDR global memory
    LPNFNSDATA        lpNfnsMem;            // Ptr to aNfnsData

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RIGHT RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Check for RIGHT LENGTH ERROR
    if (!IsSingleton (aplNELMRht))
        goto RIGHT_LENGTH_EXIT;

    // Check for RIGHT DOMAIN ERROR
    if (!IsNumeric (aplTypeRht))
        goto RIGHT_DOMAIN_EXIT;

    // Lock the memory to get a ptr to it
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // If the right arg is a global, ...
    if (hGlbRht NE NULL)
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    else
        lpMemRht = &aplLongestRht;

    // Validate the tie number
    if (!NfnsArgTieNum (aplTypeRht, lpMemRht, aplNELMRht, 0, &TieNum, lptkFunc, lpMemPTD)
     ||  TieNum >= 0)
        goto RIGHT_DOMAIN_EXIT;

    // Check for duplicate tie number
    switch (IsDuplicateTieNum_EM (TieNum, lptkFunc, lpMemPTD, &uTie))
    {
        case -1:
            goto ERROR_EXIT;

        case FALSE:
            goto RIGHT_DOMAIN_EXIT;

        case TRUE:
            break;

        defstop
            break;
    } // End SWITCH

    // Lock the memory to get a ptr to it
    lpNfnsHdr = InitLockNfns (lptkFunc, lpMemPTD);

    // Point to the matching tie number entry
    lpNfnsMem = &lpNfnsHdr->aNfnsData[uTie];

    // Get the file size
    LOAPLINT (aplFileSize) =
      GetFileSize (lpNfnsMem->hFile,            // File handle
                  &HIAPLINT (aplFileSize));     // High-order file size

    // Check for error
    if (LOAPLINT (aplFileSize) EQ INVALID_FILE_SIZE
     && GetLastError () NE NO_ERROR)
    {
        // Format a system error message
        SysErrMsg_EM (GetLastError (), lptkRhtArg);

        goto ERROR_EXIT;
    } // End IF

    // Allocate a new YYRes;
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkInteger  = aplFileSize;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    if (lpNfnsHdr NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lpMemPTD->hGlbNfns); lpNfnsHdr = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnMonNSIZE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydNSIZE_EM_YY
//
//  Dyadic []NSIZE -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydNSIZE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydNSIZE_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End SysFnDydNSIZE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnNTIE_EM_YY
//
//  System function:  []NTIE -- Open a native file
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnNTIE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnNTIE_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonNTIE_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydNTIE_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnNTIE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonNTIE_EM_YY
//
//  Monadic []NTIE -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonNTIE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonNTIE_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End SysFnMonNTIE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydNTIE_EM_YY
//
//  Dyadic []NTIE -- Open a native file
//
//  FileName []NTIE    tn [Mode [DiskConv | (DiskConv WsConv)]]
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydNTIE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydNTIE_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Call common code
    return SysFnCreateTie_EM_YY (FALSE, lptkLftArg, FALSE, lptkFunc, lptkRhtArg, lptkAxis);
} // End SysFnDydNTIE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnNUNTIE_EM_YY
//
//  System function:  []NUNTIE -- Close one or more open native files
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnNUNTIE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnNUNTIE_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonNUNTIE_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydNUNTIE_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnNUNTIE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonNUNTIE_EM_YY
//
//  Monadic []NUNTIE -- Close one or more open native files
//
//  []NUNTIE tn [tn ...]
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonNUNTIE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonNUNTIE_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeRht;           // Right arg storage type
    APLNELM           aplNELMRht;           // Right arg NELM
    APLRANK           aplRankRht;           // Right arg Rank
    APLLONGEST        aplLongestRht;        // Right arg longest if immediate
    HGLOBAL           hGlbRht = NULL,       // Right arg global memory handle
                      hGlbRes = NULL;       // Result ...
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg header
                      lpMemHdrRes = NULL;   // ...    result    ...
    LPVOID            lpMemRht;             // Ptr to right arg global memory
    LPAPLINT          lpMemRes;             // Ptr to result global memory data
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    LPPERTABDATA      lpMemPTD;             // Ptr to PerTabData global memory
    APLINT            TieNum,               // File tie number
                      nTieNums = 0;         // # tie numbers in the result
    APLUINT           ByteRes;              // # bytes in the result
    UINT              uCnt;                 // Loop counter
    UINT              uRht;                 // ...
    LPNFNSHDR         lpNfnsHdr = NULL;     // Ptr to NFNSHDR global memory
    LPNFNSDATA        lpNfnsMem;            // Ptr to aNfnsData

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RIGHT RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Check for RIGHT DOMAIN ERROR
    if (!IsNumeric (aplTypeRht))
        goto RIGHT_DOMAIN_EXIT;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get right arg's global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // If the right arg is a global, ...
    if (hGlbRht NE NULL)
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    else
        lpMemRht = &aplLongestRht;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_INT, aplNELMRht, 1);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLock000 (hGlbRes);

    // Fill in the header values
    lpMemHdrRes->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpMemHdrRes->ArrType    = ARRAY_INT;
////lpMemHdrRes->PermNdx    = PERMNDX_NONE; // Already zero from GHND
////lpMemHdrRes->SysVar     = FALSE;        // Already zero from GHND
    lpMemHdrRes->RefCnt     = 1;
    lpMemHdrRes->NELM       = 0;
    lpMemHdrRes->Rank       = 1;

    // Skip over the header and dimension
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Lock the memory to get a ptr to it
    lpNfnsHdr = InitLockNfns (lptkFunc, lpMemPTD);

    // Loop through the right arg
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    {
        // Validate the tie number
        if (!NfnsArgTieNum (aplTypeRht, lpMemRht, aplNELMRht, uRht, &TieNum, lptkFunc, lpMemPTD))
            goto RIGHT_DOMAIN_EXIT;

        // Point to the first entry in use
        lpNfnsMem = &lpNfnsHdr->aNfnsData[lpNfnsHdr->offFirstInuse];

        // Loop through the tie numbers
        for (uCnt = 0; uCnt < lpNfnsHdr->nTieNums; uCnt++)
        if (TieNum EQ lpNfnsMem->iTieNumber)
            break;
        else
            // Point to the next tie number in use
            lpNfnsMem = &lpNfnsHdr->aNfnsData[lpNfnsMem->offNextInuse];

        // Check for error
        if (uCnt < lpNfnsHdr->nTieNums)
        {
            // Save this tie # in the result
            *lpMemRes++ = TieNum;
            nTieNums++;

            // Close the handle -- this also releases all locked regions
            CloseHandle (lpNfnsMem->hFile); lpNfnsMem->hFile = NULL;

            // Release resources
            NfnsReleaseResources (lpNfnsHdr, lpNfnsMem, (UINT) (lpNfnsMem - &lpNfnsHdr->aNfnsData[0]));
        } // End IF
    } // End FOR

    // Save the result NELM and dimension
    lpMemHdrRes->NELM                =
    *VarArrayBaseToDim (lpMemHdrRes) = nTieNums;

    // If some tie numbers were invalid, ...
    if (nTieNums NE aplNELMRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

        // Calculate space needed for the result
        ByteRes = CalcArraySize (ARRAY_INT, nTieNums, 1);

        // Reallocate the result down if necessary
        hGlbRes =
          MyGlobalReAlloc (hGlbRes, (APLU3264) ByteRes, GMEM_MOVEABLE);
    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
    lpYYRes->tkToken.tkFlags.NoDisplay = TRUE;
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbRes, lpMemHdrRes);
NORMAL_EXIT:
    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    if (lpNfnsHdr NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lpMemPTD->hGlbNfns); lpNfnsHdr = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnMonNUNTIE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydNUNTIE_EM_YY
//
//  Dyadic []NUNTIE -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydNUNTIE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydNUNTIE_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End SysFnDydNUNTIE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $NfnsReleaseOneResource
//
//  Release one resource
//***************************************************************************

void NfnsReleaseOneResource
    (LPNFNSDATA lpNfnsMem)          // Ptr to aNfnsData

{
    // If it's still valid, ...
    if (lpNfnsMem->hFile NE NULL)
    {
        // Close the handle -- this also releases all locked regions
        CloseHandle (lpNfnsMem->hFile); lpNfnsMem->hFile = NULL;
    } // End IF

    // If it's still valid, free the filename global memory handle
    if (lpNfnsMem->hGlbFileName NE NULL
     && FreeResultGlobalVar (lpNfnsMem->hGlbFileName))
        lpNfnsMem->hGlbFileName = NULL;
} // End NfnsReleaseOneResource


//***************************************************************************
//  $NfnsReleaseResources
//
//  Release resources associated with a file
//***************************************************************************

void NfnsReleaseResources
    (LPNFNSHDR  lpNfnsHdr,          // Ptr to NFNSHDR global memory
     LPNFNSDATA lpNfnsMem,          // Ptr to aNfnsData
     UINT       uTie)               // Offset of matching tie number entry

{
    UINT       uOff;                // Loop counter
    LPNFNSDATA lpNfnsTmp;           // Ptr to aNfnsData

    // Release these resources
    NfnsReleaseOneResource (lpNfnsMem);

    // Point to the first Inuse entry
    lpNfnsTmp = &lpNfnsHdr->aNfnsData[lpNfnsHdr->offFirstInuse];

    // Walk the Inuse chain and remove this entry
    for (uOff = 0; uOff < lpNfnsHdr->nTieNums; uOff++)
    if (lpNfnsTmp->offNextInuse EQ uTie)
    {
        // Remove this entry from the Inuse chain
        lpNfnsTmp->offNextInuse = lpNfnsMem->offNextInuse;

        break;
    } else
        // Point to the next Inuse entry
        lpNfnsTmp = &lpNfnsHdr->aNfnsData[lpNfnsTmp->offNextInuse];

    // If this entry is at the start of the Inuse chain, ...
    if (lpNfnsHdr->offFirstInuse EQ uTie)
        // Remove it from the start of the Inuse chain
        lpNfnsHdr->offFirstInuse = lpNfnsMem->offNextInuse;

    // Delete the tie number entry
    lpNfnsMem->iTieNumber   = 0;
    lpNfnsMem->offNextInuse = -1;
    lpNfnsMem->offNextFree  = lpNfnsHdr->offFirstFree;

    // Append to the start of the free chain
    lpNfnsHdr->offFirstFree = uTie;

    // Count it out
    lpNfnsHdr->nTieNums--;
} // End NfnsReleaseResources


//***************************************************************************
//  $NfnsArgTieNum
//
//  Parse a tie number from an arg
//***************************************************************************

UBOOL NfnsArgTieNum
    (APLSTYPE     aplType,              // Arg storage type
     LPVOID       lpMem,                // Ptr to global memory data
     APLNELM      aplNELM,              // NELM of arg
     UINT         uIndex,               // Index # into lpMem
     LPAPLINT     lpTieNum,             // Ptr to result tie num
     LPTOKEN      lptkFunc,             // Ptr to function token
     LPPERTABDATA lpMemPTD)             // Ptr to PerTabData global memory

{
    UBOOL bRet = FALSE;                 // TRUE iff the result is valid

    // If the index is valid, ...
    if (uIndex < aplNELM)
        bRet =
          NfnsArgAplint (aplType,       // Arg storage type
                         lpMem,         // Ptr to global memory data
                         aplNELM,       // NELM of arg
                         uIndex,        // Index # into lpMem
                         lpTieNum);     // Ptr to result APLINT
    // If the result is valid,
    //   and the tie number is zero, ...
    if (bRet && *lpTieNum EQ 0)
        *lpTieNum = GetNextTieNum_EM (lptkFunc, lpMemPTD);

    return bRet;
} // End NfnsArgTieNum


//***************************************************************************
//  $NfnsArgAplint
//
//  Parse an APLINT from an arg
//***************************************************************************

UBOOL NfnsArgAplint
    (APLSTYPE     aplType,              // Arg storage type
     LPVOID       lpMem,                // Ptr to global memory data
     APLNELM      aplNELM,              // NELM of arg
     UINT         uIndex,               // Index # into lpMem
     LPAPLINT     lpAplint)             // Ptr to result

{
    UBOOL bRet = TRUE;                  // TRUE iff the result is valid

    // If the index is in range, ...
    if (uIndex < aplNELM)
        // Parse an APLINT
        *lpAplint =
          ConvertToInteger_SCT (aplType,    // Arg storage type
                                lpMem,      // Ptr to global memory data
                                uIndex,     // Index # into lpMem
                               &bRet);      // Ptr to TRUE iff the result is valid
    return bRet;
} // End NfnsArgAplint


//***************************************************************************
//  $NfnsArgMode
//
//  Parse a mode value from an arg
//***************************************************************************

UBOOL NfnsArgMode
    (APLSTYPE aplType,              // Arg storage type
     LPVOID   lpMem,                // Ptr to global memory data
     APLNELM  aplNELM,              // NELM of arg
     UINT     uIndex,               // Index # into lpMem
     LPDWORD  lpAccessMode,         // Ptr to result access mode
     DWORD    defAccessMode,        // Default access mode
     LPDWORD  lpShareMode,          // Ptr to result share mode
     DWORD    defShareMode)         // Default share mode

{
    UBOOL  bRet = TRUE;             // TRUE iff the result is valid
    APLINT aplMode;                 // Combined access & share modes

    // Set the default modes
    aplMode = defAccessMode + defShareMode;

    // If the index is in range, ...
    if (uIndex < aplNELM)
        bRet = NfnsArgAplint (aplType,
                              lpMem,
                              aplNELM,
                              uIndex,
                             &aplMode);
    // If the result is valid, ...
    if (bRet)
    // Split the combined mode into access mode
    switch (aplMode & 0x0F)
    {
        case OF_READ:
            *lpAccessMode = GENERIC_READ;

            break;

        case OF_WRITE:
            *lpAccessMode = GENERIC_WRITE;

            break;

        case OF_READWRITE:
            *lpAccessMode = GENERIC_READ
                          | GENERIC_WRITE;
            break;

        default:
            bRet = FALSE;

            break;
    } // End IF/SWITCH

    // If the result is valid, ...
    if (bRet)
    // Split the combined mode into share mode
    switch (aplMode & 0xF0)
    {
        case OF_SHARE_EXCLUSIVE:
            *lpShareMode = 0;

            break;

        case OF_SHARE_DENY_WRITE:
            *lpShareMode = FILE_SHARE_READ;

            break;

        case OF_SHARE_DENY_READ:
            *lpShareMode = FILE_SHARE_WRITE;

            break;

        case OF_SHARE_COMPAT:
        case OF_SHARE_DENY_NONE:
            *lpShareMode = FILE_SHARE_READ
                         | FILE_SHARE_WRITE;
            break;

        default:
            bRet = FALSE;

            break;
    } // End IF/SWITCH

    return bRet;
} // End NfnsArgMode


//***************************************************************************
//  $NfnsArgConv
//
//  Parse a conversion value from an arg
//***************************************************************************

UBOOL NfnsArgConv
    (APLSTYPE   aplType,            // Arg storage type
     LPVOID     lpMem,              // Ptr to global memory data
     APLNELM    aplNELM,            // NELM of arg
     UINT       uIndex,             // Index # into lpMem
     UBOOL      bOnly1,             // TRUE iff only one conversion value is allowed
     LPDR_VAL   lpDiskConv,         // Ptr to result disk conversion
     DR_VAL     defDiskConv,        // Default disk oncversion
     LPDR_VAL   lpWsConv,           // Ptr to result workspace conversion
     DR_VAL     defWsConv,          // Default workspace conversion
     LPAPLSTYPE lpaplTypeWs)        // Ptr to workspace storage type

{
    UBOOL             bRet = TRUE;  // TRUE iff the result is valid
    LPSYMENTRY        lpSymGlb;     // Ptr to SYMENTRY or HGLOBAL
    LPVARARRAY_HEADER lpMemHdr = NULL;      // Ptr to the global numeric item header

    // Initialize with default values
    *lpDiskConv = defDiskConv;
    if (!bOnly1)
        *lpWsConv   = defWsConv;

    // If the index is in range, ...
    if (uIndex < aplNELM)
    switch (aplType)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            *lpDiskConv = GetNextInteger (lpMem, aplType, uIndex);

            break;

        case ARRAY_FLOAT:
            // Attempt to convert the float to an integer using System []CT
            *lpDiskConv = FloatToAplint_SCT (*(LPAPLFLOAT) lpMem, &bRet);

            break;

        case ARRAY_RAT:
            // Attempt to convert the RAT to an integer using System []CT
            *lpDiskConv = mpq_get_sctsx (&((LPAPLRAT) lpMem)[uIndex], &bRet);

            break;

        case ARRAY_VFP:
            // Attempt to convert the VFP to an integer using System []CT
            *lpDiskConv = mpfr_get_sctsx (&((LPAPLVFP) lpMem)[uIndex], &bRet);

            break;

        case ARRAY_NESTED:
            // Get the global handle
            lpSymGlb = ((LPAPLNESTED) lpMem)[uIndex];

            // Split cases based upon the ptr type bits
            switch (GetPtrTypeDir (lpSymGlb))
            {
                case PTRTYPE_STCONST:
                    // The item must be an immediate
                    bRet = lpSymGlb->stFlags.Imm;

                    if (bRet)
                    // Split cases based upon the immediate type
                    switch (lpSymGlb->stFlags.ImmType)
                    {
                        case IMMTYPE_BOOL:
                        case IMMTYPE_INT:
                            // Get the integer value
                            *lpDiskConv = lpSymGlb->stData.stInteger;

                            break;

                        case IMMTYPE_FLOAT:
                            // Attempt to convert the float to an integer using System []CT
                            *lpDiskConv = FloatToAplint_SCT (lpSymGlb->stData.stFloat, &bRet);

                            break;

                        case IMMTYPE_CHAR:
                            bRet = FALSE;

                            break;

                        defstop
                            break;
                    } // End IF/SWITCH

                    break;

                case PTRTYPE_HGLOBAL:
                    // Lock the mmory to get a ptr to it
                    lpMemHdr = MyGlobalLockVar (lpSymGlb);

                    bRet = (IsSimpleChar (lpMemHdr->ArrType)
                         && IsVector (lpMemHdr->Rank))
                        || (IsNested (lpMemHdr->ArrType)
                         && IsVector (lpMemHdr->Rank)
                         && lpMemHdr->NELM <= 2)
                        || (IsNumeric (lpMemHdr->ArrType)
                         && (IsScalar (lpMemHdr->Rank)
                          || (IsVector (lpMemHdr->Rank)
                           && lpMemHdr->NELM <= 2)));

                    // If it's valid, ...
                    if (bRet)
                    // Split cases based upon the storage type
                    switch (lpMemHdr->ArrType)
                    {
                        case ARRAY_BOOL:
                        case ARRAY_INT:
                        case ARRAY_APA:
                            // Split cases based upon the NELM
                            switch (lpMemHdr->NELM)
                            {
                                case 2:
                                    if (bOnly1)
                                        bRet = FALSE;
                                    else
                                        *lpWsConv   = GetNextInteger (VarArrayDataFmBase (lpMemHdr), lpMemHdr->ArrType, 1);

                                    // Fall through to singleton case

                                case 1:
                                    *lpDiskConv = GetNextInteger (VarArrayDataFmBase (lpMemHdr), lpMemHdr->ArrType, 0);

                                    break;

                                case 0:
                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        case ARRAY_FLOAT:
                            // Split cases based upon the NELM
                            switch (lpMemHdr->NELM)
                            {
                                case 2:
                                    if (bOnly1)
                                        bRet = FALSE;
                                    else
                                        // Attempt to convert the float to an integer using System []CT
                                        *lpWsConv   = FloatToAplint_SCT (((LPAPLFLOAT) VarArrayDataFmBase (lpMemHdr))[1], &bRet);

                                    // Fall through to singleton case

                                case 1:
                                    *lpDiskConv = FloatToAplint_SCT (((LPAPLFLOAT) VarArrayDataFmBase (lpMemHdr))[0], &bRet);

                                    break;

                                case 0:
                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        case ARRAY_RAT:
                            // Split cases based upon the NELM
                            switch (lpMemHdr->NELM)
                            {
                                case 2:
                                    if (bOnly1)
                                        bRet = FALSE;
                                    else
                                        // Attempt to convert the RAT to an integer using System []CT
                                        *lpWsConv   = mpq_get_sctsx (&((LPAPLRAT) VarArrayDataFmBase (lpMemHdr))[1], &bRet);

                                    // Fall through to singleton case

                                case 1:
                                    // Attempt to convert the RAT to an integer using System []CT
                                    *lpDiskConv = mpq_get_sctsx (&((LPAPLRAT) VarArrayDataFmBase (lpMemHdr))[0], &bRet);

                                    break;

                                case 0:
                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        case ARRAY_VFP:
                            // Split cases based upon the NELM
                            switch (lpMemHdr->NELM)
                            {
                                case 2:
                                    if (bOnly1)
                                        bRet = FALSE;
                                    else
                                        // Attempt to convert the VFP to an integer using System []CT
                                        *lpWsConv   = mpfr_get_sctsx (&((LPAPLVFP) VarArrayDataFmBase (lpMemHdr))[1], &bRet);

                                    // Fall through to singleton case

                                case 1:
                                    // Attempt to convert the VFP to an integer using System []CT
                                    *lpDiskConv = mpfr_get_sctsx (&((LPAPLVFP) VarArrayDataFmBase (lpMemHdr))[0], &bRet);

                                    break;

                                case 0:
                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        case ARRAY_CHAR:
                            // Attempt to convert the item to a DR_xxx enum
                            *lpDiskConv = NestedCharVec (lpSymGlb, *lpDiskConv);

                            break;

                        case ARRAY_NESTED:
                            // Split cases based upon the NELM
                            switch (lpMemHdr->NELM)
                            {
                                case 2:
                                    if (bOnly1)
                                        bRet = FALSE;
                                    else
                                        // Attempt to convert the item to a DR_xxx enum
                                        *lpWsConv = NestedCharVec (((HGLOBAL *) VarArrayDataFmBase (lpMemHdr))[1], *lpDiskConv);

                                    // Fall through to singleton case

                                case 1:
                                    // Attempt to convert the item to a DR_xxx enum
                                    *lpDiskConv = NestedCharVec (((HGLOBAL *) VarArrayDataFmBase (lpMemHdr))[0], *lpDiskConv);

                                    break;

                                case 0:
                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        defstop
                            break;
                    } // End IF/SWITCH

                    // We no longer need this ptr
                    MyGlobalUnlock (lpSymGlb); lpMemHdr = NULL;

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        defstop
            break;
    } // End IF/SWITCH

    // If the result is valid, ...
    if (bRet)
    // Validate the two conversion values
    switch (*lpDiskConv)
    {
        case DR_BOOL:
        case DR_CHAR8:
        case DR_CHAR16:
        case DR_CHAR32:
        case DR_INT8:
        case DR_INT16:
        case DR_INT32:
        case DR_INT64:
        case DR_FLOAT:
            break;

        default:
            bRet = FALSE;

            break;
    } // End SWITCH

    // If the result is valid, ...
    if (bRet && !bOnly1)
    switch (*lpWsConv)
    {
        case DR_BOOL:
            *lpaplTypeWs = ARRAY_BOOL;

            break;

        case DR_CHAR16:
            *lpaplTypeWs = ARRAY_CHAR;

            break;

        case DR_INT64:
            *lpaplTypeWs = ARRAY_INT;

            break;

        case DR_FLOAT:
            *lpaplTypeWs = ARRAY_FLOAT;

            break;

        default:
            bRet = FALSE;

            break;
    } // End SWITCH

    return bRet;
} // End NfnsArgConv


//***************************************************************************
//  $NestedCharVec
//
//  Attempt to convert a char vector to a DR_xxx Conversion code
//***************************************************************************

DR_VAL NestedCharVec
    (HGLOBAL hGlb,                      // Global memory handle
     DR_VAL  drDef)                     // Default conversion value

{
    DR_VAL            drRet = 0;        // Return value
    UINT              uCnt;             // Loop counter
    LPVARARRAY_HEADER lpMemHdr = NULL;      // Ptr to global memory header
    LPAPLCHAR         lpMemChar;        // Ptr to global memory data

typedef struct tagCONV_CODES
{
    LPAPLCHAR  wszCode;                 // Ptr to conversion name
    DR_VAL     drVal;                   // Matching conversion code
} CONV_CODES, *LPCONV_CODES;

    static CONV_CODES convCodes[] =
    {{L"bool"   , DR_BOOL   },
     {L"char8"  , DR_CHAR8  },
     {L"char16" , DR_CHAR16 },
     {L"char32" , DR_CHAR32 },
     {L"int8"   , DR_INT8   },
     {L"int16"  , DR_INT16  },
     {L"int32"  , DR_INT32  },
     {L"int64"  , DR_INT64  },
     {L"flt64"  , DR_FLOAT  },
     {L"double" , DR_FLOAT  },
    };

    // Lock the memory to get a ptr to it
    lpMemHdr = MyGlobalLockVar (hGlb);

    // Check the rank
    if (!IsVector (lpMemHdr->Rank))
    {
        // Set the return value
        drRet = 0;

        goto NORMAL_EXIT;
    } // End IF

    // Check the storage type
    if (!IsSimpleChar (lpMemHdr->ArrType))
    {
        // Set the return value
        drRet = 0;

        goto NORMAL_EXIT;
    } // End IF

    // Check the NELM
    if (IsEmpty (lpMemHdr->NELM))
    {
        // Set the return value
        drRet = drDef;

        goto NORMAL_EXIT;
    } // End IF

    // Skip over the header and dimensions
    lpMemChar = VarArrayDataFmBase (lpMemHdr);

    // Loop through the possibilities
    for (uCnt = 0; uCnt < countof (convCodes); uCnt++)
    if (lstrcmpiW (lpMemChar, convCodes[uCnt].wszCode) EQ 0)
    {
        drRet = convCodes[uCnt].drVal;

        break;
    } // End FOR/IF
NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlb); lpMemHdr = NULL;

    return drRet;
} // End NestedCharVec


//***************************************************************************
//  $GetNextTieNum_EM
//
//  Get the next available native file tie number
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- GetNextTieNum_EM"
#else
#define APPEND_NAME
#endif

APLINT GetNextTieNum_EM
    (LPTOKEN      lptkFunc,             // Ptr to function token
     LPPERTABDATA lpMemPTD)             // Ptr to PerTabData global memory

{
    HGLOBAL    hGlbTie;                 // Tie # global memory handle
    LPAPLINT   lpMemTie;                // Ptr to tie # memory
    APLINT     TieNum;                  // The result
    LPNFNSHDR  lpNfnsHdr;               // Ptr to NFNSHDR global memory
    LPNFNSDATA lpNfnsMem;               // Ptr to aNfnsData
    UINT       uCnt;                    // Loop counter

    // Lock the memory to get a ptr to it
    lpNfnsHdr = InitLockNfns (lptkFunc, lpMemPTD);

    // If there are no tie numbers as yet, ...
    if (lpNfnsHdr->nTieNums EQ 0)
        TieNum = -1;
    else
    {
        // Point to the first entry in use
        lpNfnsMem = &lpNfnsHdr->aNfnsData[lpNfnsHdr->offFirstInuse];

        // Allocate memory to hold the existing tie numbers so we may sort them
        hGlbTie = DbgGlobalAlloc (GHND, lpNfnsHdr->nTieNums * sizeof (TieNum));
        if (hGlbTie NE NULL)
        {
            // Lock the memory to get a ptr to it
            lpMemTie = MyGlobalLock000 (hGlbTie);

            // Loop through the tie numbers
            for (uCnt = 0; uCnt < lpNfnsHdr->nTieNums; uCnt++)
            {
                // Copy the existing tie numbers to the global memory
                lpMemTie[uCnt] = lpNfnsMem->iTieNumber;

                // Point to the next tie number in use
                lpNfnsMem = &lpNfnsHdr->aNfnsData[lpNfnsMem->offNextInuse];
            } // End FOR

            // Sort the tie numbers
            ShellSortInt64 (lpMemTie, lpNfnsHdr->nTieNums, CmpAPLINT_DESC);

            // Loop through the tie numbers looking for an empty slot
            for (uCnt = 0, TieNum = -1; uCnt < lpNfnsHdr->nTieNums; uCnt++, TieNum--)
            if (TieNum NE lpMemTie[uCnt])
                break;

            // Unlock and free (and set to NULL) a global name and ptr
            UnlFreeGlbName (hGlbTie, lpMemTie);
        } else
        {
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                       lptkFunc);
            TieNum = 0;
        } // End IF/ELSE
    } // End IF/ELSE

    // We no longer need this ptr
    MyGlobalUnlock (lpMemPTD->hGlbNfns); lpNfnsHdr = NULL;

    return TieNum;
} // End GetNextTieNum_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CmpAPLINT_DESC
//
//  Compare two APLINTs being sorted desendingly
//***************************************************************************

int CmpAPLINT_DESC
    (APLINT aplIntLft,
     APLINT aplIntRht)

{
    // Compare the two numbers
    return signumint (aplIntRht - aplIntLft);
} // End CmpAPLINT_DESC


//***************************************************************************
//  $IsDuplciateTieNum_EM
//
//  Determines if the given tie number is already in use
//***************************************************************************

UBOOL IsDuplicateTieNum_EM
    (APLINT       TieNum,               // Tie # to check
     LPTOKEN      lptkFunc,             // Ptr to function token
     LPPERTABDATA lpMemPTD,             // Ptr to PerTabData global memory
     LPUINT       lpoff)                // Ptr to offset of matching entry

{
    UINT       uCnt;                    // Loop counter
    UBOOL      bRet;                    // TRUE iff the tie number is a duplicate
    LPNFNSHDR  lpNfnsHdr;               // Ptr to NFNSHDR global memory
    LPNFNSDATA lpNfnsMem;               // Ptr to aNfnsData

    // If it hasn't been initialized  as yet, ...
    if (lpMemPTD->hGlbNfns EQ NULL
     && !InitGlbNfns_EM (lptkFunc, lpMemPTD))
        return -1;

    // Lock the memory to get a ptr to it
    lpNfnsHdr = InitLockNfns (lptkFunc, lpMemPTD);

    // Point to the first entry in use
    lpNfnsMem = &lpNfnsHdr->aNfnsData[lpNfnsHdr->offFirstInuse];

    // Loop through the tie numbers
    for (uCnt = 0; uCnt < lpNfnsHdr->nTieNums; uCnt++)
    if (TieNum EQ lpNfnsMem->iTieNumber)
        break;
    else
        // Point to the next tie number in use
        lpNfnsMem = &lpNfnsHdr->aNfnsData[lpNfnsMem->offNextInuse];

    // Mark as a duplicate or not
    bRet = (uCnt < lpNfnsHdr->nTieNums);

    // Return result if requested
    if (lpoff NE NULL)
        *lpoff = (UINT) (lpNfnsMem - &lpNfnsHdr->aNfnsData[0]);

    // We no longer need this ptr
    MyGlobalUnlock (lpMemPTD->hGlbNfns); lpNfnsHdr = NULL;

    return bRet;
} // End IsDuplicateTieNum_EM


//***************************************************************************
//  $InitGlbNfns_EM
//
//  Initialize the lpMemPTD->hGlbNfns struc
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- InitGlbNfns_EM"
#else
#define APPEND_NAME
#endif

UBOOL InitGlbNfns_EM
    (LPTOKEN      lptkFunc,             // Ptr to function token
     LPPERTABDATA lpMemPTD)             // Ptr to PerTabData global memory

{
    Assert (lpMemPTD->hGlbNfns EQ NULL);

    // Allocate the initial entry
    lpMemPTD->hGlbNfns =
      DbgGlobalAlloc (GHND, sizeof (NFNSHDR) + DEF_NFNS_INIT * sizeof (NFNSDATA));

    if (lpMemPTD->hGlbNfns NE NULL)
    {
        LPNFNSHDR  lpNfnsHdr;           // Ptr to NFNSHDR global memory
        LPNFNSDATA lpNfnsMem;           // Ptr to aNfnsData
        UINT       uCnt;                // Loop counter

        // Lock the memory to get a ptr to it
        lpNfnsHdr = InitLockNfns (lptkFunc, lpMemPTD);

        // Initialize the struc
        lpNfnsHdr->Sig.nature    = NFNS_HEADER_SIGNATURE;
////////lpNfnsHdr->nTieNums      = 0;               // Already zero from GHND
        lpNfnsHdr->nMax          = DEF_NFNS_INIT;
////////lpNfnsHdr->offFirstFree  = 0;               // Already zero from GHND
        lpNfnsHdr->offFirstInuse = -1;
        lpNfnsMem = &lpNfnsHdr->aNfnsData[0];

        // Loop through the free & inuse entries
        for (uCnt = 0; uCnt < (DEF_NFNS_INIT - 1); uCnt++)
        {
            // Zap the value
            lpNfnsMem  ->offNextInuse = -1;

            // Form a linked list
            lpNfnsMem++->offNextFree  = uCnt + 1;
        } // End FOR

        // Initialize the last entry
        lpNfnsMem->offNextFree = -1;

        // We no longer need this ptr
        MyGlobalUnlock (lpMemPTD->hGlbNfns); lpNfnsHdr = NULL;

        return TRUE;
    } else
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return FALSE;
    } // End IF/ELSE
} // End InitGlbNfns_EM
#undef  APPEND_NAME


//***************************************************************************
//  $IncrGlbNfns
//
//  Reallocate the NFNS struc
//***************************************************************************

UBOOL IncrGlbNfns
    (LPNFNSHDR   *lplpNfnsHdr,      // Ptr to NFNSHDR global memory
     LPTOKEN      lptkFunc,         // Ptr to function token
     LPPERTABDATA lpMemPTD)         // Ptr to PerTabData global memory

{
    APLUINT    ByteRes;             // # bytes in the result
    HGLOBAL    hGlbNfns;            // New NFNS data struc global memory handle
    LPNFNSDATA lpNfnsMem;           // Ptr to aNfnsData
    UINT       uCnt;                // Loop counter

    // Calculate the new size
    ByteRes = sizeof (NFNSHDR) + ((*lplpNfnsHdr)->nMax + DEF_NFNS_INCR) * sizeof (NFNSDATA);

    // We no longer need this ptr
    MyGlobalUnlock (lpMemPTD->hGlbNfns); *lplpNfnsHdr = NULL;

    // Reallocate the struc
    //   moving the old data to the new location, and
    //   freeing the old global memory
    hGlbNfns =
      MyGlobalReAlloc (lpMemPTD->hGlbNfns, (APLU3264) ByteRes, GHND);
    if (hGlbNfns EQ NULL)
    {
        // Lock the original memory to get a ptr to it
        *lplpNfnsHdr = MyGlobalLockNfn (lpMemPTD->hGlbNfns);

        return FALSE;
    } // End IF

    // Save the new global memory handle
    lpMemPTD->hGlbNfns = hGlbNfns;

    // Lock the memory to get a ptr to it
    *lplpNfnsHdr = InitLockNfns (lptkFunc, lpMemPTD);

    // Append the new entries to the free list
    (*lplpNfnsHdr)->offFirstFree = (*lplpNfnsHdr)->nMax;

    // Point to the first free entry
    lpNfnsMem = &(*lplpNfnsHdr)->aNfnsData[(*lplpNfnsHdr)->nMax];

    // Loop through the free & inuse entries
    for (uCnt = 0; uCnt < (DEF_NFNS_INCR - 1); uCnt++)
    {
        // Zap the value
        lpNfnsMem  ->offNextInuse = -1;

        // Form a linked list
        lpNfnsMem++->offNextFree  = (*lplpNfnsHdr)->nMax + uCnt + 1;
    } // End FOR

    // Initialize the last entry
    lpNfnsMem->offNextFree = -1;

    // Add in the incremental size
    (*lplpNfnsHdr)->nMax += DEF_NFNS_INCR;

    // Mark as successful
    return TRUE;
} // End IncrGlbNfns


//***************************************************************************
//  $NfnsSaveData_EM
//
//  Save data for a newly created/opened native file
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- NfnsSaveData_EM"
#else
#define APPEND_NAME
#endif

UBOOL NfnsSaveData_EM
    (HANDLE       hFile,            // File handle
     HGLOBAL      hGlbFileName,     // Filename global memory handle
     DR_VAL       DiskConv,         // Disk format
     DR_VAL       WsConv,           // Workspace format
     APLSTYPE     aplTypeWs,        // Workspace storage type (if ARRAY_ERROR, don't increment the RefCnt)
     APLINT       TieNum,           // Tie #
     LPTOKEN      lptkFunc,         // Ptr to function token
     LPPERTABDATA lpMemPTD)         // Ptr to PerTabData global memory

{
    UBOOL      bRet = TRUE;         // TRUE iff the result is valid
    LPNFNSHDR  lpNfnsHdr;           // Ptr to NFNSHDR global memory
    LPNFNSDATA lpNfnsMem;           // Ptr to aNfnsData
    UINT       uTie;                // Offset of matching tie number entry

    // Lock the memory to get a ptr to it
    lpNfnsHdr = InitLockNfns (lptkFunc, lpMemPTD);

    // If we don't have room for one more tied file, ...
    if (lpNfnsHdr->nTieNums EQ lpNfnsHdr->nMax
     && !IncrGlbNfns (&lpNfnsHdr, lptkFunc, lpMemPTD))
        goto WSFULL_EXIT;
    else
    {
        // Get the index of the first free entry
        uTie = lpNfnsHdr->offFirstFree;

        // Point to the first free entry
        lpNfnsMem = &lpNfnsHdr->aNfnsData[uTie];

        // Save the new data
        lpNfnsMem->iTieNumber    = TieNum;
        lpNfnsMem->DiskConv      = DiskConv;
        lpNfnsMem->WsConv        = WsConv;
        lpNfnsMem->aplTypeWs     = aplTypeWs;
        lpNfnsMem->hFile         = hFile;
        lpNfnsMem->hGlbFileName  = MakePtrTypeGlb (hGlbFileName);

        // If requested to do so, ...
        if (aplTypeWs NE ARRAY_ERROR)
            // Increment the refcnt
            DbgIncrRefCntDir_PTB (lpNfnsMem->hGlbFileName); // EXAMPLE:  open same file twice or more

        // Manage the linked list
        lpNfnsHdr->offFirstFree  = lpNfnsMem->offNextFree;
        lpNfnsMem->offNextFree   = -1;

        // Append this entry to the Inuse list
        lpNfnsMem->offNextInuse  = lpNfnsHdr->offFirstInuse;
        lpNfnsHdr->offFirstInuse = uTie;

        // Count another tie number in use
        lpNfnsHdr->nTieNums++;

        goto NORMAL_EXIT;
    } // End IF/ELSE

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    // Mark as in error
    bRet = FALSE;
NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (lpMemPTD->hGlbNfns); lpNfnsHdr = NULL;

    return bRet;
} // End NfnsSaveData_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SysErrMsg_EM
//
//  Format a system error message
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysErrMsg_EM"
#else
#define APPEND_NAME
#endif

void SysErrMsg_EM
    (DWORD   dwLastError,           // GetLastError ()
     LPTOKEN lptkFunc)              // Ptr to function token

{
    static WCHAR wszTemp[256];      // ***FIXME*** -- Not thread-safe
    UINT         uLen;              // Length of message

    // Split cases based upon the last error
    switch (dwLastError)
    {
        case ERROR_FILE_NOT_FOUND:
            ErrorMessageIndirectToken (ERRMSG_FILE_NOT_FOUND APPEND_NAME,
                                       lptkFunc);
            break;

        case ERROR_PATH_NOT_FOUND:
            ErrorMessageIndirectToken (ERRMSG_PATH_NOT_FOUND APPEND_NAME,
                                       lptkFunc);
            break;

        case ERROR_ACCESS_DENIED:
            ErrorMessageIndirectToken (ERRMSG_FILE_ACCESS_ERROR APPEND_NAME,
                                       lptkFunc);
            break;

        case ERROR_SHARING_VIOLATION:
            ErrorMessageIndirectToken (ERRMSG_FILE_SHARE_ERROR APPEND_NAME,
                                       lptkFunc);
            break;

        default:
            FormatMessageW (FORMAT_MESSAGE_FROM_SYSTEM, // Source and processing options
                            NULL,                       // Pointer to message source
                            dwLastError,                // Requested message identifier
                            0,                          // Language identifier for requested message
                            wszTemp,                    // Pointer to message buffer
                            countof (wszTemp),          // Maximum size of message buffer
                            NULL);                      // Address of array of message inserts
            // Get the message length
            uLen = lstrlenW (wszTemp);

            // If the message ends with "\r\n"
            if (uLen >= 2
             && wszTemp[uLen - 2] EQ WC_CR
             && wszTemp[uLen - 1] EQ WC_LF)
                // Strip off the trailing "\r\n"
                wszTemp[uLen - 2] = WC_EOS;

            ErrorMessageIndirectToken (wszTemp,
                                       lptkFunc);
            break;
    } // End SWITCH
} // End SysErrMsg_EM
#undef  APPEND_NAME


//***************************************************************************
//  $NfnsEraseRenameResize_EM
//
//  Common subroutine to []NERASE, []NRENAME, and []NRESIZE
//
//  FileName []NERASE  tn
//  FileName []NRENAME tn
//  NewSize  []NRESIZE tn
//
//  When called for []NERASE/[]NRENAME, the left arg is a char scalar/vector, the right arg is a tie #.
//  ...             []NRESIZE         , ...               numeric singleton , ...
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- NfnsEraseRenameResize_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE NfnsEraseRenameResize_EM
    (LPTOKEN   lptkLftArg,
     LPTOKEN   lptkFunc,
     LPTOKEN   lptkRhtArg,
     LPTOKEN   lptkAxis,
     NFNS_NERR eNERR,
     UBOOL   (*NfnsCom) (LPNFNSHDR, LPNFNSDATA, LPAPLCHAR, LPAPLCHAR))

{
    APLSTYPE          aplTypeLft,           // Left arg storage type
                      aplTypeRht;           // Right ...
    APLNELM           aplNELMLft,           // Left arg NELM
                      aplNELMRht;           // Right ...
    APLRANK           aplRankLft,           // Left arg rank
                      aplRankRht;           // Right ...
    HGLOBAL           hGlbLft = NULL,       // Left arg global memory handle
                      hGlbRht = NULL;       // Right ...
    LPVARARRAY_HEADER lpMemHdrLft = NULL,   // Ptr to left arg header
                      lpMemHdrRht = NULL;   // ...    right    ...
    LPAPLCHAR         lpMemLft;             // Ptr to left arg global memory
    LPVOID            lpMemRht,             // ...    right ...
                      lpMemCom;             // ...    temp
    APLINT            TieNum,               // File tie number
                      aplNewSize;           // New size from []NRESIZE
    APLLONGEST        aplLongestLft,        // Left arg immediate value
                      aplLongestRht;        // Right ...
    UINT              uTie;                 // Offset of matching tie number entry
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    LPNFNSHDR         lpNfnsHdr = NULL;     // Ptr to NFNSHDR global memory
    LPNFNSDATA        lpNfnsMem;            // Ptr to aNfnsData
    LPPERTABDATA      lpMemPTD;             // Ptr to PerTabData global memory
    LPVARARRAY_HEADER lpMemHdrFile = NULL;  // Ptr to filename header
    LPAPLCHAR         lpMemFile;            // Ptr to filename memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for LEFT RANK ERROR
    if (IsMultiRank (aplRankLft))
        goto LEFT_RANK_EXIT;

    // Check for RIGHT RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // If it's []NRESIZE, ...
    if (eNERR EQ COM_NRESIZE)
    {
        // Check for LEFT LENGTH ERROR
        if (!IsSingleton (aplNELMLft))
            goto LEFT_LENGTH_EXIT;
    } // End IF

    // Check for RIGHT LENGTH ERROR
    if (!IsSingleton (aplNELMRht))
        goto RIGHT_LENGTH_EXIT;

    // If it's []NRESIZE, ...
    if (eNERR EQ COM_NRESIZE)
    {
        // Check for LEFT DOMAIN ERROR
        if (!IsNumeric (aplTypeLft))
            goto LEFT_DOMAIN_EXIT;
    } else
        // Check for LEFT DOMAIN ERROR
        if (!IsSimpleChar (aplTypeLft))
            goto LEFT_DOMAIN_EXIT;

    // Check for RIGHT DOMAIN ERROR
    if (!IsNumeric (aplTypeRht))
        goto RIGHT_DOMAIN_EXIT;

    // Lock the memory to get a ptr to it
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // If the left arg is a global, ...
    if (hGlbLft NE NULL)
        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
    else
        lpMemLft = (LPAPLCHAR) &aplLongestLft;

    // If the right arg is a global, ...
    if (hGlbRht NE NULL)
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    else
        lpMemRht = &aplLongestRht;

    // Validate the tie number
    if (!NfnsArgTieNum (aplTypeRht, lpMemRht, aplNELMRht, 0, &TieNum, lptkFunc, lpMemPTD)
     ||  TieNum >= 0)
        goto RIGHT_DOMAIN_EXIT;

    // If it's []NRESIZE, ...
    if (eNERR EQ COM_NRESIZE)
    {
        // Parse an APLINT from lpMemLft
        if (!NfnsArgAplint (aplTypeLft,   // Arg storage type
                            lpMemLft,     // Ptr to global memory data
                            aplNELMLft,   // NELM of arg
                            0,            // Index # into lpMem
                           &aplNewSize))  // Ptr to result APLINT
            goto LEFT_DOMAIN_EXIT;
        // Point to the result
        lpMemCom = &aplNewSize;
    } else
        lpMemCom = lpMemLft;

    // Check for duplicate tie number
    switch (IsDuplicateTieNum_EM (TieNum, lptkFunc, lpMemPTD, &uTie))
    {
        case -1:
            goto ERROR_EXIT;

        case FALSE:
            goto RIGHT_DOMAIN_EXIT;

        case TRUE:
            break;
    } // End SWITCH

    // Lock the memory to get a ptr to it
    lpNfnsHdr = InitLockNfns (lptkFunc, lpMemPTD);

    // Point to the matching tie number entry
    lpNfnsMem = &lpNfnsHdr->aNfnsData[uTie];

    // Lock the memory to get a ptr to it
    lpMemHdrFile = MyGlobalLockVar (lpNfnsMem->hGlbFileName);

    // Skip over the header and dimensions
    lpMemFile = VarArrayDataFmBase (lpMemHdrFile);

    // If it's []NERASE, ...
    if (eNERR EQ COM_NERASE
     && lstrcmpiW (lpMemLft, lpMemFile) NE 0)
        goto LEFT_DOMAIN_EXIT;

    // Erase/Rename/Resize the file
    if (!(*NfnsCom) (lpNfnsHdr, lpNfnsMem, lpMemFile, lpMemCom))
    {
        // Format a system error message
        SysErrMsg_EM (GetLastError (), lptkLftArg);

        goto ERROR_EXIT;
    } // End IF

    // Allocate a new YYRes;
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
    lpYYRes->tkToken.tkFlags.NoDisplay = TRUE;
    lpYYRes->tkToken.tkData.tkInteger  = TieNum;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

LEFT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (hGlbLft NE NULL && lpMemHdrLft NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;
    } // End IF

    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    if (lpNfnsMem->hGlbFileName && lpMemHdrFile)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lpNfnsMem->hGlbFileName); lpMemHdrFile = NULL;
    } // End IF

    if (lpMemPTD->hGlbNfns NE NULL && lpNfnsHdr NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lpMemPTD->hGlbNfns); lpNfnsHdr = NULL;
    } // End IF

    return lpYYRes;
} // End NfnsEraseRenameResize_EM
#undef  APPEND_NAME


//***************************************************************************
//  $NfnsErase
//
//  Subroutine to NfnsEraseRenameResize_EM to erase a file
//***************************************************************************

UBOOL NfnsErase
    (LPNFNSHDR  lpNfnsHdr,          // Ptr to NFNSHDR global memory
     LPNFNSDATA lpNfnsMem,          // Ptr to aNfnsData
     LPAPLCHAR  lpMemFile,          // Ptr to existing filename global memory data
     LPAPLCHAR  lpMemLft)           // Ptr to left arg global memory data (or new size if []NRESIZE)

{
    UBOOL uRes;                     // The result

    // Close the handle -- this also releases all locked regions
    CloseHandle (lpNfnsMem->hFile); lpNfnsMem->hFile = NULL;

    // Delete the file
    uRes = DeleteFileW (lpMemFile);

    // Release resources
    NfnsReleaseResources (lpNfnsHdr, lpNfnsMem, (UINT) (lpNfnsMem - &lpNfnsHdr->aNfnsData[0]));

    return uRes;
} // End NfnsErase


//***************************************************************************
//  $NfnsRename
//
//  Subroutine to NfnsEraseRenameResize_EM to rename a file
//***************************************************************************

UBOOL NfnsRename
    (LPNFNSHDR  lpNfnsHdr,          // Ptr to NFNSHDR global memory
     LPNFNSDATA lpNfnsMem,          // Ptr to aNfnsData
     LPAPLCHAR  lpMemFile,          // Ptr to existing filename global memory data
     LPAPLCHAR  lpMemLft)           // Ptr to left arg global memory data (or new size if []NRESIZE)

{
    return MoveFileW (lpMemFile, lpMemLft);
} // End NfnsRename


//***************************************************************************
//  $NfnsResize
//
//  Subroutine to NfnsEraseRenameResize_EM to resize a file
//***************************************************************************

UBOOL NfnsResize
    (LPNFNSHDR  lpNfnsHdr,          // Ptr to NFNSHDR global memory
     LPNFNSDATA lpNfnsMem,          // Ptr to aNfnsData
     LPAPLCHAR  lpMemFile,          // Ptr to existing filename global memory data
     LPAPLCHAR  lpMemLft)           // Ptr to left arg global memory data (or new size if []NRESIZE)

{
    DWORD  dwPtrLow;                // Temp result from SetFilePointer
    APLINT aplNewSize;              // New file size for []NRESIZE

    // Get the new size
    aplNewSize = *(LPAPLINT) lpMemLft;

    // Attempt to set the file pointer
    dwPtrLow =
      SetFilePointer (lpNfnsMem->hFile,
                      LOAPLINT (aplNewSize),
                     &HIAPLINT (aplNewSize),
                      FILE_BEGIN);
    // Check for error
    if (dwPtrLow EQ INVALID_SET_FILE_POINTER
     && GetLastError () NE NO_ERROR)
        return FALSE;

    // Otherwise, truncate/extend the file
    return SetEndOfFile (lpNfnsMem->hFile);
} // End NfnsResize


//***************************************************************************
//  $NfnsWriteData_EM
//
//  Translate and write out data
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- NfnsWriteData_EM"
#else
#define APPEND_NAME
#endif

UBOOL NfnsWriteData_EM
    (HANDLE       hFile,                // File handle
     APLSTYPE     aplType,              // Source storage type
     APLNELM      aplNELM,              // ...    NELM
     LPVOID       lpMem,                // Ptr to source data
     DR_VAL       DiskConv,             // Disk conversion value (DR_xxx)
     LPAPLINT     lpaplFileOut,         // # bytes written to disk
     LPOVERLAPPED lpOverLapped,         // Ptr to overlapped struct
     LPPERTABDATA lpMemPTD,             // Ptr to PerTabData global memory
     LPTOKEN      lptkFunc)             // Ptr to function token

{
    UBOOL       bRet = TRUE;            // TRUE iff the result is valid
    APLUINT     uCnt;                   // Loop counter
    APLINT      aplFileOff;             // Starting offset of write
    DWORD       dwWritten,              // # bytes to write out
                dwRet,                  // Return code from WaitForSingleObject
                dwErr;                  // GetLastError code
    APLNELM     uMin,                   // Minimum of size and NELM
                aplOff = 0;             // Offset from start
    UINT        uBitMask = BIT0;        // Bit mask for looping through Booleans
    NFNS_BUFFER NfnsBuff;               // Temporary buffer
    HANDLE      hWaitEvent;             // Wait event handle

    // Get the initial write offset (possibly -1)
    LOAPLINT (aplFileOff) = lpOverLapped->Offset;
    HIAPLINT (aplFileOff) = lpOverLapped->OffsetHigh;

    // Initialize the # bytes written to disk
    *lpaplFileOut = 0;

    // Split cases based upon the source storage format
    switch (aplType)
    {
        case ARRAY_BOOL:                // Write -- Src = BOOL
            // Split cases based upon the DiskConv value
            switch (DiskConv)
            {
                case DR_BOOL:           // Write -- Src = BOOL, Dest = BOOL
                    /* Loop through the chunks of the arg */
                    while (aplNELM)
                    {
                        APLUINT uBytes;

                        /* Get # bits to handle in this chunk */
                        uMin = min (8 * countof (NfnsBuff.Tchar8), aplNELM);
                        uBytes = RoundUpBitsToBytes (uMin);

                        // Copy bytes to NfnsBuff
                        CopyMemory (NfnsBuff.Tchar8, &((LPAPLBOOL) lpMem)[RoundUpBitsToBytes (aplOff)], (APLU3264) uBytes);

                        /* Loop through the arg reversing the bits in each byte */
                        for (uCnt = 0; uCnt < uBytes; uCnt++)
                            NfnsBuff.Tchar8[uCnt] = FastBoolTrans[NfnsBuff.Tchar8[uCnt]][FBT_REVERSE];

                        /* Remove from NELM */
                        aplNELM -= uMin;
                        aplOff  += uMin;

                        // Calculate the # bytes to write out
                        dwWritten = (DWORD) uBytes;

                        // Write out the data
                        if (!WriteFile (hFile,              // File handle
                                        NfnsBuff.Tchar8,    // Data to write out
                                        dwWritten,          // # bytes to write out
                                        NULL,               // Ptr to # bytes written
                                        lpOverLapped))      // Ptr to OVERLAPPED struc
                        {
                            // Get the last error code
                            dwErr = GetLastError ();

                            // If not still doing asynchronous I/O, ...
                            if (dwErr NE ERROR_IO_PENDING)
                                goto SYS_ERROR_EXIT;
                        } // End IF

                        // Check for previous uncleared event
                        Assert (lpMemPTD->hWaitEvent EQ NULL);

                        // Save the wait event in case the user Ctrl-Breaks
                        lpMemPTD->hWaitEvent = lpOverLapped->hEvent;

                        // Wait for the specified time
                        dwRet = WaitForSingleObject (lpOverLapped->hEvent, INFINITE);

                        // Save and clear the wait event
                        //  so as to test for Ctrl-Break signalling the event
                        hWaitEvent = lpMemPTD->hWaitEvent;
                        lpMemPTD->hWaitEvent = NULL;

                        // If the write is incomplete, ...
                        if (hWaitEvent EQ NULL          /* Ctrl-Break     */
                         || dwRet NE WAIT_OBJECT_0)     /* Incomplete I/O */
                            goto INCOMPLETE_EXIT;

                        // Get # bytes written
                        GetOverlappedResult (hFile,         // File handle
                                             lpOverLapped,  // Ptr to OVERLAPPED struc
                                            &dwWritten,     // Ptr to # bytes written
                                             TRUE);         // TRUE iff the function waits
                                                            //   for I/O completion
                        // Accumulate the # bytes written to disk
                        *lpaplFileOut += dwWritten;

                        /* If it's not append, ... */
                        if (aplFileOff NE -1)
                        {
                            /* Increment the offset in overLapped for next write */
                            aplFileOff += dwWritten;
                            lpOverLapped->Offset     = LOAPLINT (aplFileOff);
                            lpOverLapped->OffsetHigh = HIAPLINT (aplFileOff);
                        } /* End IF */
                    } // End WHILE

                    break;

#define PROMOTE_FROM_BOOL_WRITE_OUT(a)                                                              \
                    /* Loop through the chunks of the arg */                                        \
                    while (aplNELM)                                                                 \
                    {                                                                               \
                        /* Get # source elements to handle in this chunk */                         \
                        uMin = min (countof (NfnsBuff.a), aplNELM);                                 \
                                                                                                    \
                        /* Loop through the arg */                                                  \
                        for (uCnt = 0; uCnt < uMin; uCnt++)                                         \
                        {                                                                           \
                            NfnsBuff.a[uCnt] = (uBitMask & *(LPAPLBOOL) lpMem) ? TRUE : FALSE;      \
                                                                                                    \
                            /* Shift over the bit mask */                                           \
                            uBitMask <<= 1;                                                         \
                                                                                                    \
                            /* Check for end-of-byte */                                             \
                            if (uBitMask EQ END_OF_BYTE)                                            \
                            {                                                                       \
                                uBitMask = BIT0;            /* Start over */                        \
                                ((LPAPLBOOL) lpMem)++;      /* Skip to next byte */                 \
                            } /* End IF */                                                          \
                        } /* End FOR */                                                             \
                                                                                                    \
                        /* Remove from NELM */                                                      \
                        aplNELM -= uMin;                                                            \
                     /* aplOff  += uMin; */                                                         \
                                                                                                    \
                        /* Calculate the # bytes to write out */                                    \
                        dwWritten = (DWORD) (uMin * sizeof (NfnsBuff.a[0]));                        \
                                                                                                    \
                        /* Write out the data */                                                    \
                        if (!WriteFile (hFile,              /* File handle             */           \
                                        NfnsBuff.a,         /* Data to write out       */           \
                                        dwWritten,          /* # bytes to write out    */           \
                                        NULL,               /* Ptr to # bytes written  */           \
                                        lpOverLapped))      /* Ptr to OVERLAPPED struc */           \
                        {                                                                           \
                            /* Get the last error code */                                           \
                            dwErr = GetLastError ();                                                \
                                                                                                    \
                            /* If not still doing asynchronous I/O, ... */                          \
                            if (dwErr NE ERROR_IO_PENDING)                                          \
                                goto SYS_ERROR_EXIT;                                                \
                        } /* End IF */                                                              \
                                                                                                    \
                        /* Check for previous uncleared event */                                    \
                        Assert (lpMemPTD->hWaitEvent EQ NULL);                                      \
                                                                                                    \
                        /* Save the wait event in case the user Ctrl-Breaks */                      \
                        lpMemPTD->hWaitEvent = lpOverLapped->hEvent;                                \
                                                                                                    \
                        /* Wait for the specified time */                                           \
                        dwRet = WaitForSingleObject (lpOverLapped->hEvent, INFINITE);               \
                                                                                                    \
                        /* Save and clear the wait event                                            \
                            so as to test for Ctrl-Break signalling the event */                    \
                        hWaitEvent = lpMemPTD->hWaitEvent;                                          \
                        lpMemPTD->hWaitEvent = NULL;                                                \
                                                                                                    \
                        /* If the write is incomplete, ... */                                       \
                        if (hWaitEvent EQ NULL          /* Ctrl-Break     */                        \
                         || dwRet NE WAIT_OBJECT_0)     /* Incomplete I/O */                        \
                            goto INCOMPLETE_EXIT;                                                   \
                                                                                                    \
                        /* Get # bytes written */                                                   \
                        GetOverlappedResult (hFile,         /* File handle                */        \
                                             lpOverLapped,  /* Ptr to OVERLAPPED struc    */        \
                                            &dwWritten,     /* Ptr to # bytes written     */        \
                                             TRUE);         /* TRUE iff the function waits          \
                                                                 for I/O completion       */        \
                        /* Accumulate the # bytes written to disk */                                \
                        *lpaplFileOut += dwWritten;                                                 \
                                                                                                    \
                        /* If it's not append, ... */                                               \
                        if (aplFileOff NE -1)                                                       \
                        {                                                                           \
                            /* Increment the offset in overLapped for next write */                 \
                            aplFileOff += dwWritten;                                                \
                            lpOverLapped->Offset     = LOAPLINT (aplFileOff);                       \
                            lpOverLapped->OffsetHigh = HIAPLINT (aplFileOff);                       \
                        } /* End IF */                                                              \
                    } /* End WHILE */

                case DR_CHAR8:          // Write -- Src = BOOL, Dest = CHAR8
                    PROMOTE_FROM_BOOL_WRITE_OUT (Tchar8)

                    break;

                case DR_CHAR16:         // Write -- Src = BOOL, Dest = CHAR16
                    PROMOTE_FROM_BOOL_WRITE_OUT (Tchar16)

                    break;

                case DR_CHAR32:         // Write -- Src = BOOL, Dest = CHAR32
                    PROMOTE_FROM_BOOL_WRITE_OUT (Tchar32)

                    break;

                case DR_INT8:           // Write -- Src = BOOL, Dest = INT8
                    PROMOTE_FROM_BOOL_WRITE_OUT (Tint8)

                    break;

                case DR_INT16:          // Write -- Src = BOOL, Dest = INT16
                    PROMOTE_FROM_BOOL_WRITE_OUT (Tint16)

                    break;

                case DR_INT32:          // Write -- Src = BOOL, Dest = INT32
                    PROMOTE_FROM_BOOL_WRITE_OUT (Tint32)

                    break;

                case DR_INT64:          // Write -- Src = BOOL, Dest = INT64
                    PROMOTE_FROM_BOOL_WRITE_OUT (Tint64)

                    break;

                case DR_FLOAT:          // Write -- Src = BOOL, Dest = FLOAT
                    PROMOTE_FROM_BOOL_WRITE_OUT (Tflt64)

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

#define LIMIT_WRITE_OUT(DstType,DstCast,minval,maxval,GetNext,SrcType)                      \
                    /* Loop through the chunks of the arg */                                \
                    while (aplNELM)                                                         \
                    {                                                                       \
                        SrcType aplTemp;                                                    \
                                                                                            \
                        /* Get # source elements to handle in this chunk */                 \
                        uMin = min (countof (NfnsBuff.DstType), aplNELM);                   \
                                                                                            \
                        /* Loop through the arg */                                          \
                        for (uCnt = 0; uCnt < uMin; uCnt++)                                 \
                        {                                                                   \
                            /* Get the source value */                                      \
                            aplTemp = GetNext (lpMem, aplType, aplOff + uCnt);              \
                                                                                            \
                            /* If it's within range, ... */                                 \
                            if (minval <= aplTemp                                           \
                             &&           aplTemp <= maxval)                                \
                                NfnsBuff.DstType[uCnt] = (DstCast) aplTemp;                 \
                            else                                                            \
                                goto DOMAIN_EXIT;                                           \
                        } /* End FOR */                                                     \
                                                                                            \
                        /* Remove from NELM */                                              \
                        aplNELM -= uMin;                                                    \
                        aplOff  += uMin;                                                    \
                                                                                            \
                        /* Calculate the # bytes to write out */                            \
                        dwWritten = (DWORD) (uMin * sizeof (NfnsBuff.DstType[0]));          \
                                                                                            \
                        /* Write out the data */                                            \
                        if (!WriteFile (hFile,              /* File handle             */   \
                                        NfnsBuff.DstType,   /* Data to write out       */   \
                                        dwWritten,          /* # bytes to write out    */   \
                                        NULL,               /* Ptr to # bytes written  */   \
                                        lpOverLapped))      /* Ptr to OVERLAPPED struc */   \
                        {                                                                   \
                            /* Get the last error code */                                   \
                            dwErr = GetLastError ();                                        \
                                                                                            \
                            /* If not still doing asynchronous I/O, ... */                  \
                            if (dwErr NE ERROR_IO_PENDING)                                  \
                                goto SYS_ERROR_EXIT;                                        \
                        } /* End IF */                                                      \
                                                                                            \
                        /* Check for previous uncleared event */                            \
                        Assert (lpMemPTD->hWaitEvent EQ NULL);                              \
                                                                                            \
                        /* Save the wait event in case the user Ctrl-Breaks */              \
                        lpMemPTD->hWaitEvent = lpOverLapped->hEvent;                        \
                                                                                            \
                        /* Wait for the specified time */                                   \
                        dwRet = WaitForSingleObject (lpOverLapped->hEvent, INFINITE);       \
                                                                                            \
                        /* Save and clear the wait event                                    \
                            so as to test for Ctrl-Break signalling the event */            \
                        hWaitEvent = lpMemPTD->hWaitEvent;                                  \
                        lpMemPTD->hWaitEvent = NULL;                                        \
                                                                                            \
                        /* If the write is incomplete, ... */                               \
                        if (hWaitEvent EQ NULL          /* Ctrl-Break     */                \
                         || dwRet NE WAIT_OBJECT_0)     /* Incomplete I/O */                \
                            goto INCOMPLETE_EXIT;                                           \
                                                                                            \
                        /* Get # bytes written */                                           \
                        GetOverlappedResult (hFile,         /* File handle                */\
                                             lpOverLapped,  /* Ptr to OVERLAPPED struc    */\
                                            &dwWritten,     /* Ptr to # bytes written     */\
                                             TRUE);         /* TRUE iff the function waits  \
                                                                 for I/O completion       */\
                        /* Accumulate the # bytes written to disk */                        \
                        *lpaplFileOut += dwWritten;                                         \
                                                                                            \
                        /* If it's not append, ... */                                       \
                        if (aplFileOff NE -1)                                               \
                        {                                                                   \
                            /* Increment the offset in overLapped for next write */         \
                            aplFileOff += dwWritten;                                        \
                            lpOverLapped->Offset     = LOAPLINT (aplFileOff);               \
                            lpOverLapped->OffsetHigh = HIAPLINT (aplFileOff);               \
                        } /* End IF */                                                      \
                    } /* End WHILE */

#define DEMOTE_TO_BOOL_WRITE_OUT(GetNext,SrcType)                                           \
                    /* Loop through the chunks of the arg */                                \
                    while (aplNELM)                                                         \
                    {                                                                       \
                        BYTE    uBitCount = 0;                                              \
                        SrcType aplTemp;                                                    \
                                                                                            \
                        /* Get # source elements to handle in this chunk */                 \
                        uMin = min (countof (NfnsBuff.Tchar8), aplNELM);                    \
                                                                                            \
                        /* Loop through the arg */                                          \
                        for (uCnt = 0; uCnt < uMin; uCnt++)                                 \
                        {                                                                   \
                            /* Get the source value */                                      \
                            aplTemp = GetNext (lpMem, aplType, aplOff + uCnt);              \
                                                                                            \
                            /* If it's within range, ... */                                 \
                            if (IsBooleanValue (aplTemp))                                   \
                                NfnsBuff.Tchar8[uCnt >> LOG2NBIB] |=                        \
                                  (BYTE) ((aplTemp EQ 1) << (uBitCount++ % NBIB));          \
                            else                                                            \
                                goto DOMAIN_EXIT;                                           \
                        } /* End FOR */                                                     \
                                                                                            \
                        /* Remove from NELM */                                              \
                        aplNELM -= uMin;                                                    \
                        aplOff  += uMin;                                                    \
                                                                                            \
                        /* Calculate the # bytes to write out */                            \
                        dwWritten = (DWORD) RoundUpBitsToBytes (uMin);                      \
                                                                                            \
                        /* Write out the data */                                            \
                        if (!WriteFile (hFile,              /* File handle             */   \
                                        NfnsBuff.Tchar8,    /* Data to write out       */   \
                                        dwWritten,          /* # bytes to write out    */   \
                                        NULL,               /* Ptr to # bytes written  */   \
                                        lpOverLapped))      /* Ptr to OVERLAPPED struc */   \
                        {                                                                   \
                            /* Get the last error code */                                   \
                            dwErr = GetLastError ();                                        \
                                                                                            \
                            /* If not still doing asynchronous I/O, ... */                  \
                            if (dwErr NE ERROR_IO_PENDING)                                  \
                                goto SYS_ERROR_EXIT;                                        \
                        } /* End IF */                                                      \
                                                                                            \
                        /* Check for previous uncleared event */                            \
                        Assert (lpMemPTD->hWaitEvent EQ NULL);                              \
                                                                                            \
                        /* Save the wait event in case the user Ctrl-Breaks */              \
                        lpMemPTD->hWaitEvent = lpOverLapped->hEvent;                        \
                                                                                            \
                        /* Wait for the specified time */                                   \
                        dwRet = WaitForSingleObject (lpOverLapped->hEvent, INFINITE);       \
                                                                                            \
                        /* Save and clear the wait event                                    \
                            so as to test for Ctrl-Break signalling the event */            \
                        hWaitEvent = lpMemPTD->hWaitEvent;                                  \
                        lpMemPTD->hWaitEvent = NULL;                                        \
                                                                                            \
                        /* If the write is incomplete, ... */                               \
                        if (hWaitEvent EQ NULL          /* Ctrl-Break     */                \
                         || dwRet NE WAIT_OBJECT_0)     /* Incomplete I/O */                \
                            goto INCOMPLETE_EXIT;                                           \
                                                                                            \
                        /* Get # bytes written */                                           \
                        GetOverlappedResult (hFile,         /* File handle                */\
                                             lpOverLapped,  /* Ptr to OVERLAPPED struc    */\
                                            &dwWritten,     /* Ptr to # bytes written     */\
                                             TRUE);         /* TRUE iff the function waits  \
                                                                 for I/O completion       */\
                        /* Accumulate the # bytes written to disk */                        \
                        *lpaplFileOut += dwWritten;                                         \
                                                                                            \
                        /* If it's not append, ... */                                       \
                        if (aplFileOff NE -1)                                               \
                        {                                                                   \
                            /* Increment the offset in overLapped for next write */         \
                            aplFileOff += dwWritten;                                        \
                            lpOverLapped->Offset     = LOAPLINT (aplFileOff);               \
                            lpOverLapped->OffsetHigh = HIAPLINT (aplFileOff);               \
                        } /* End IF */                                                      \
                    } /* End WHILE */

        case ARRAY_INT:                 // Write -- Src = INT
        case ARRAY_APA:                 // Write -- Src = APA
            // Split cases based upon the DiskConv value
            switch (DiskConv)
            {
                case DR_BOOL:           // Write -- Src = INT/APA, Dest = BOOL
                    DEMOTE_TO_BOOL_WRITE_OUT (GetNextInteger, APLINT)

                    break;

                case DR_CHAR8:          // Write -- Src = INT/APA, Dest = CHAR8
                    LIMIT_WRITE_OUT (Tchar8,            // Destination type (NfnsBuff.DstType)
                                     CHAR8,             // ...         cast
                                     0,                 // ...         minimum
                                     UINT8_MAX,         // ...         maximum
                                     GetNextInteger,    // Source GetNext routine
                                     APLINT)            // ...    type
                    break;

                case DR_CHAR16:         // Write -- Src = INT/APA, Dest = CHAR16
                    LIMIT_WRITE_OUT (Tchar16,           // Destination type (NfnsBuff.DstType)
                                     CHAR16,            // ...         cast
                                     0,                 // ...         minimum
                                     UINT16_MAX,        // ...         maximum
                                     GetNextInteger,    // Source GetNext routine
                                     APLUINT)           // ...    type
                    break;

                case DR_CHAR32:         // Write -- Src = INT/APA, Dest = CHAR32
                    LIMIT_WRITE_OUT (Tchar32,           // Destination type (NfnsBuff.DstType)
                                     CHAR32,            // ...         cast
                                     0,                 // ...         minimum
                                     UINT32_MAX,        // ...         maximum
                                     GetNextInteger,    // Source GetNext routine
                                     APLUINT)           // ...    type
                    break;

                case DR_INT8:           // Write -- Src = INT/APA, Dest = INT8
                    LIMIT_WRITE_OUT (Tint8,             // Destination type (NfnsBuff.DstType)
                                     INT8,              // ...         cast
                                     INT8_MIN,          // ...         minimum
                                     INT8_MAX,          // ...         maximum
                                     GetNextInteger,    // Source GetNext routine
                                     APLINT)            // ...    type
                    break;

                case DR_INT16:          // Write -- Src = INT/APA, Dest = INT16
                    LIMIT_WRITE_OUT (Tint16,            // Destination type (NfnsBuff.DstType)
                                     INT16,             // ...         cast
                                     INT16_MIN,         // ...         minimum
                                     INT16_MAX,         // ...         maximum
                                     GetNextInteger,    // Source GetNext routine
                                     APLINT)            // ...    type
                    break;

                case DR_INT32:          // Write -- Src = INT/APA, Dest = INT32
                    LIMIT_WRITE_OUT (Tint32,            // Destination type (NfnsBuff.DstType)
                                     INT32,             // ...         cast
                                     INT32_MIN,         // ...         minimum
                                     INT32_MAX,         // ...         maximum
                                     GetNextInteger,    // Source GetNext routine
                                     APLINT)            // ...    type
                    break;

                case DR_INT64:          // Write -- Src = INT/APA, Dest = INT64
                    // If it's an APA, ...
                    if (IsSimpleAPA (aplType))
                    {
                        LIMIT_WRITE_OUT (Tint64,            // Destination type (NfnsBuff.DstType)
                                         INT64,             // ...         cast
                                         INT64_MIN,         // ...         minimum
                                         INT64_MAX,         // ...         maximum
                                         GetNextInteger,    // Source GetNext routine
                                         APLINT)            // ...    type
                    } else
                    {
                        // Calculate the # bytes to write out
                        dwWritten = (DWORD) aplNELM * sizeof (APLINT);

                        // Write out the data
                        if (!WriteFile (hFile,          // File handle
                                        lpMem,          // Data to write out
                                        dwWritten,      // # bytes to write out
                                        NULL,           // Ptr to # bytes written
                                        lpOverLapped))  // Ptr to OVERLAPPED struc
                        {
                            /* Get the last error code */
                            dwErr = GetLastError ();

                            /* If not still doing asynchronous I/O, ... */
                            if (dwErr NE ERROR_IO_PENDING)
                                goto SYS_ERROR_EXIT;
                        } /* End IF */

                        /* Check for previous uncleared event */
                        Assert (lpMemPTD->hWaitEvent EQ NULL);

                        /* Save the wait event in case the user Ctrl-Breaks */
                        lpMemPTD->hWaitEvent = lpOverLapped->hEvent;

                        /* Wait for the specified time */
                        dwRet = WaitForSingleObject (lpOverLapped->hEvent, INFINITE);

                        /* Save and clear the wait event
                            so as to test for Ctrl-Break signalling the event */
                        hWaitEvent = lpMemPTD->hWaitEvent;
                        lpMemPTD->hWaitEvent = NULL;

                        /* If the write is incomplete, ... */
                        if (hWaitEvent EQ NULL          /* Ctrl-Break     */
                         || dwRet NE WAIT_OBJECT_0)     /* Incomplete I/O */
                            goto INCOMPLETE_EXIT;

                        // Get # bytes written
                        GetOverlappedResult (hFile,         // File handle
                                             lpOverLapped,  // Ptr to OVERLAPPED struc
                                            &dwWritten,     // Ptr to # bytes written
                                             TRUE);         // TRUE iff the function waits
                                                            //   for I/O completion
                    } // End IF/ELSE

                    // Accumulate the # bytes written to disk
                    *lpaplFileOut += dwWritten;

                    break;

                case DR_FLOAT:          // Write -- Src = INT/APA, Dest = FLOAT
                    LIMIT_WRITE_OUT (Tflt64,            // Destination type (NfnsBuff.DstType)
                                     FLT64,             // ...         cast
                                     FLT64_MIN,         // ...         minimum
                                     FLT64_MAX,         // ...         maximum
                                     GetNextInteger,    // Source GetNext routine
                                     APLINT)            // ...    type
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_CHAR:                // Write -- Src = CHAR
            // Split cases based upon the DiskConv value
            switch (DiskConv)
            {
                case DR_BOOL:           // Write -- Src = CHAR, Dest = BOOL
                    DEMOTE_TO_BOOL_WRITE_OUT (GetNextChar16,  APLCHAR)

                    break;

                case DR_CHAR8:          // Write -- Src = CHAR, Dest = CHAR8
                    LIMIT_WRITE_OUT (Tchar8,            // Destination type (NfnsBuff.DstType)
                                     CHAR8,             // ...         cast
                                     0,                 // ...         minimum
                                     UINT8_MAX,         // ...         maximum
                                     GetNextChar16,     // Source GetNext routine
                                     APLCHAR)           // ...    type
                    break;

                case DR_CHAR16:         // Write -- Src = CHAR, Dest = CHAR16
                    // Calculate the # bytes to write out
                    dwWritten = (DWORD) aplNELM * sizeof (APLCHAR);

                    // Write out the data
                    if (!WriteFile (hFile,          // File handle
                                    lpMem,          // Data to write out
                                    dwWritten,      // # bytes to write out
                                    NULL,           // Ptr to # bytes written
                                    lpOverLapped))  // Ptr to OVERLAPPED struc
                    {
                        /* Get the last error code */
                        dwErr = GetLastError ();

                        /* If not still doing asynchronous I/O, ... */
                        if (dwErr NE ERROR_IO_PENDING)
                            goto SYS_ERROR_EXIT;
                    } /* End IF */

                    /* Check for previous uncleared event */
                    Assert (lpMemPTD->hWaitEvent EQ NULL);

                    /* Save the wait event in case the user Ctrl-Breaks */
                    lpMemPTD->hWaitEvent = lpOverLapped->hEvent;

                    /* Wait for the specified time */
                    dwRet = WaitForSingleObject (lpOverLapped->hEvent, INFINITE);

                    /* Save and clear the wait event
                        so as to test for Ctrl-Break signalling the event */
                    hWaitEvent = lpMemPTD->hWaitEvent;
                    lpMemPTD->hWaitEvent = NULL;

                    /* If the write is incomplete, ... */
                    if (hWaitEvent EQ NULL          /* Ctrl-Break     */
                     || dwRet NE WAIT_OBJECT_0)     /* Incomplete I/O */
                        goto INCOMPLETE_EXIT;

                    // Get # bytes written
                    GetOverlappedResult (hFile,         // File handle
                                         lpOverLapped,  // Ptr to OVERLAPPED struc
                                        &dwWritten,     // Ptr to # bytes written
                                         TRUE);         // TRUE iff the function waits
                                                        //   for I/O completion
                    // Accumulate the # bytes written to disk
                    *lpaplFileOut += dwWritten;

                    break;

                case DR_CHAR32:         // Write -- Src = CHAR, Dest = CHAR32
                    LIMIT_WRITE_OUT (Tchar32,           // Destination type (NfnsBuff.DstType)
                                     CHAR32,            // ...         cast
                                     0,                 // ...         minimum
                                     UINT32_MAX,        // ...         maximum
                                     GetNextChar16,     // Source GetNext routine
                                     APLCHAR)           // ...    type
                    break;

                case DR_INT8:           // Write -- Src = CHAR, Dest = INT8
                    LIMIT_WRITE_OUT (Tint8,             // Destination type (NfnsBuff.DstType)
                                     INT8,              // ...         cast
                                     INT8_MIN,          // ...         minimum
                                     INT8_MAX,          // ...         maximum
                                     GetNextChar16,     // Source GetNext routine
                                     APLCHAR)           // ...    type
                    break;

                case DR_INT16:          // Write -- Src = CHAR, Dest = INT16
                    LIMIT_WRITE_OUT (Tint16,            // Destination type (NfnsBuff.DstType)
                                     INT16,             // ...         cast
                                     INT16_MIN,         // ...         minimum
                                     INT16_MAX,         // ...         maximum
                                     GetNextChar16,     // Source GetNext routine
                                     APLCHAR)           // ...    type
                    break;

                case DR_INT32:          // Write -- Src = CHAR, Dest = INT32
                    LIMIT_WRITE_OUT (Tint32,            // Destination type (NfnsBuff.DstType)
                                     INT32,             // ...         cast
                                     INT32_MIN,         // ...         minimum
                                     INT32_MAX,         // ...         maximum
                                     GetNextChar16,     // Source GetNext routine
                                     APLCHAR)           // ...    type
                    break;

                case DR_INT64:          // Write -- Src = CHAR, Dest = INT64
                    LIMIT_WRITE_OUT (Tint64,            // Destination type (NfnsBuff.DstType)
                                     INT64,             // ...         cast
                                     INT64_MIN,         // ...         minimum
                                     INT64_MAX,         // ...         maximum
                                     GetNextChar16,     // Source GetNext routine
                                     APLCHAR)           // ...    type
                    break;

                case DR_FLOAT:          // Write -- Src = CHAR, Dest = FLOAT
                    LIMIT_WRITE_OUT (Tflt64,            // Destination type (NfnsBuff.DstType)
                                     FLT64,             // ...         cast
                                     FLT64_MIN,         // ...         minimum
                                     FLT64_MAX,         // ...         maximum
                                     GetNextChar16,     // Source GetNext routine
                                     APLCHAR)           // ...    type
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_FLOAT:               // Write -- Src = FLOAT
            // Split cases based upon the DiskConv value
            switch (DiskConv)
            {
                case DR_BOOL:           // Write -- Src = FLOAT, Dest = BOOL
                    goto DOMAIN_EXIT;

////////////////////DEMOTE_TO_BOOL_WRITE_OUT (GetNextFloat,   FLT64)

                    break;

                case DR_CHAR8:          // Write -- Src = FLOAT, Dest = CHAR8
                    goto DOMAIN_EXIT;

////////////////////LIMIT_WRITE_OUT (Tchar8,            // Destination type (NfnsBuff.DstType)
////////////////////                 CHAR8,             // ...         cast
////////////////////                 0,                 // ...         minimum
////////////////////                 UINT8_MAX,         // ...         maximum
////////////////////                 GetNextFlt64,      // Source GetNext routine
////////////////////                 FLT64)             // ...    type
                    break;

                case DR_CHAR16:         // Write -- Src = FLOAT, Dest = CHAR16
                    goto DOMAIN_EXIT;

////////////////////LIMIT_WRITE_OUT (Tchar16,           // Destination type (NfnsBuff.DstType)
////////////////////                 CHAR16,            // ...         cast
////////////////////                 0,                 // ...         minimum
////////////////////                 UINT16_MAX,        // ...         maximum
////////////////////                 GetNextFlt64,      // Source GetNext routine
////////////////////                 FLT64)             // ...    type
                    break;

                case DR_CHAR32:         // Write -- Src = FLOAT, Dest = CHAR32
                    goto DOMAIN_EXIT;

////////////////////LIMIT_WRITE_OUT (Tchar32,           // Destination type (NfnsBuff.DstType)
////////////////////                 CHAR32,            // ...         cast
////////////////////                 0,                 // ...         minimum
////////////////////                 UINT32_MAX,        // ...         maximum
////////////////////                 GetNextFlt64,      // Source GetNext routine
////////////////////                 FLT64)             // ...    type
                    break;

                case DR_INT8:           // Write -- Src = FLOAT, Dest = INT8
                    goto DOMAIN_EXIT;

////////////////////LIMIT_WRITE_OUT (Tint8,             // Destination type (NfnsBuff.DstType)
////////////////////                 INT8,              // ...         cast
////////////////////                 INT8_MIN,          // ...         minimum
////////////////////                 INT8_MAX,          // ...         maximum
////////////////////                 GetNextFlt64,      // Source GetNext routine
////////////////////                 FLT64)             // ...    type
                    break;

                case DR_INT16:          // Write -- Src = FLOAT, Dest = INT16
                    goto DOMAIN_EXIT;

////////////////////LIMIT_WRITE_OUT (Tint16,            // Destination type (NfnsBuff.DstType)
////////////////////                 INT16,             // ...         cast
////////////////////                 INT16_MIN,         // ...         minimum
////////////////////                 INT16_MAX,         // ...         maximum
////////////////////                 GetNextFlt64,      // Source GetNext routine
////////////////////                 FLT64)             // ...    type
                    break;

                case DR_INT32:          // Write -- Src = FLOAT, Dest = INT32
                    goto DOMAIN_EXIT;

////////////////////LIMIT_WRITE_OUT (Tint32,            // Destination type (NfnsBuff.DstType)
////////////////////                 INT32,             // ...         cast
////////////////////                 INT32_MIN,         // ...         minimum
////////////////////                 INT32_MAX,         // ...         maximum
////////////////////                 GetNextFlt64,      // Source GetNext routine
////////////////////                 FLT64)             // ...    type
                    break;

                case DR_INT64:          // Write -- Src = FLOAT, Dest = INT64
                    goto DOMAIN_EXIT;

////////////////////LIMIT_WRITE_OUT (Tint64,            // Destination type (NfnsBuff.DstType)
////////////////////                 INT64,             // ...         cast
////////////////////                 INT64_MIN,         // ...         minimum
////////////////////                 INT64_MAX,         // ...         maximum
////////////////////                 GetNextFlt64,      // Source GetNext routine
////////////////////                 FLT64)             // ...    type
                    break;

                case DR_FLOAT:          // Write -- Src = FLOAT, Dest = FLOAT
                    // Calculate the # bytes to write out
                    dwWritten = (DWORD) aplNELM * sizeof (FLT64);

                    // Write out the data
                    if (!WriteFile (hFile,          // File handle
                                    lpMem,          // Data to write out
                                    dwWritten,      // # bytes to write out
                                    NULL,           // Ptr to # bytes written
                                    lpOverLapped))  // Ptr to OVERLAPPED struc
                    {
                        /* Get the last error code */
                        dwErr = GetLastError ();

                        /* If not still doing asynchronous I/O, ... */
                        if (dwErr NE ERROR_IO_PENDING)
                            goto SYS_ERROR_EXIT;
                    } /* End IF */

                    /* Check for previous uncleared event */
                    Assert (lpMemPTD->hWaitEvent EQ NULL);

                    /* Save the wait event in case the user Ctrl-Breaks */
                    lpMemPTD->hWaitEvent = lpOverLapped->hEvent;

                    /* Wait for the specified time */
                    dwRet = WaitForSingleObject (lpOverLapped->hEvent, INFINITE);

                    /* Save and clear the wait event
                        so as to test for Ctrl-Break signalling the event */
                    hWaitEvent = lpMemPTD->hWaitEvent;
                    lpMemPTD->hWaitEvent = NULL;

                    /* If the write is incomplete, ... */
                    if (hWaitEvent EQ NULL          /* Ctrl-Break     */
                     || dwRet NE WAIT_OBJECT_0)     /* Incomplete I/O */
                        goto INCOMPLETE_EXIT;

                    // Get # bytes written
                    GetOverlappedResult (hFile,         // File handle
                                         lpOverLapped,  // Ptr to OVERLAPPED struc
                                        &dwWritten,     // Ptr to # bytes written
                                         TRUE);         // TRUE iff the function waits
                                                        //   for I/O completion
                    // Accumulate the # bytes written to disk
                    *lpaplFileOut += dwWritten;

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        defstop
            break;
    } // End SWITCH

    goto NORMAL_EXIT;

SYS_ERROR_EXIT:
    SysErrMsg_EM (GetLastError (), lptkFunc);

    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

INCOMPLETE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_INCOMPLETE_IO APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    // Mark as in error
    bRet = FALSE;
NORMAL_EXIT:
    return bRet;
} // End NfnsWriteData_EM
#undef  APPEND_NAME


//***************************************************************************
//  $InitLockNfns
//
//  Initialize and lock lpMemPTD->hGlbNfns
//***************************************************************************

LPVOID InitLockNfns
    (LPTOKEN      lptkFunc,             // Ptr to function token
     LPPERTABDATA lpMemPTD)             // Ptr to PerTabData global memory

{
    // Has it been initialized as yet?
    if (lpMemPTD->hGlbNfns EQ NULL
     && !InitGlbNfns_EM (lptkFunc, lpMemPTD))
        return NULL;

    return MyGlobalLockNfn (lpMemPTD->hGlbNfns);
} // End InitLockNfns


//***************************************************************************
//  End of File: qf_nfns.c
//***************************************************************************
