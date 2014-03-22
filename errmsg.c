//***************************************************************************
//  NARS2000 -- Error Message Functions
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
#include "headers.h"


// Length of a Newline
#define NL_LEN      1


//***************************************************************************
//  $BreakMessage
//
//  Display a message from Ctrl-Break
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- BreakMessage"
#else
#define APPEND_NAME
#endif

void BreakMessage
    (HWND         hWndSM,       // SM window handle
     LPSIS_HEADER lpSISCur)     // Ptr to current SIS entry (may be NULL if none)

{
    LPAPLCHAR    lpMemName;     // Ptr to function name global memory
    APLNELM      aplNELMRes;    // Length of function name[line #]
    APLUINT      ByteRes;       // # bytes in the result
    LPAPLCHAR    lpMemRes;      // Ptr to result global memory
    HGLOBAL      hGlbRes;       // Result global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Save the event type
    SetEventTypeMessage (EVENTTYPE_BREAK, NULL, NULL);

    // If it's valid, ...
    if (lpSISCur)
    {
        // Mark as suspended
        lpSISCur->Suspended = TRUE;

        // Lock the memory to get a ptr to it
        lpMemName = MyGlobalLock (lpSISCur->hGlbFcnName);

        // Copy the leading text
        lstrcpyW (lpMemPTD->lpwszTemp, ERRMSG_ELLIPSIS WS_CR);

        // Calculate the length so far
        aplNELMRes = lstrlenW (lpMemPTD->lpwszTemp);

        // Format the name and line #
        aplNELMRes +=
          wsprintfW (&lpMemPTD->lpwszTemp[aplNELMRes],
                      L"%s[%d]",
                      lpMemName,
                      lpSISCur->CurLineNum);
        // We no longer need this ptr
        MyGlobalUnlock (lpSISCur->hGlbFcnName); lpMemName = NULL;

        // Save the ptr
        lpMemPTD->lpwszErrorMessage = lpMemPTD->lpwszTemp;
    } else
    {
        // Prepend an ellipsis
        lpMemPTD->lpwszErrorMessage = ERRMSG_ELLIPSIS;

        // Calculate length of error message text
        aplNELMRes = lstrlenW (lpMemPTD->lpwszErrorMessage);
    } // End IF/ELSE

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRes, 1);

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
////lpHeader->Perm       = FALSE;   // Already zero from GHND
////lpHeader->SysVar     = FALSE;   // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Fill in the dimension
    *VarArrayBaseToDim (lpMemRes) = aplNELMRes;

    // Skip over the header and dimension to the data
    lpMemRes = VarArrayDataFmBase (lpMemRes);

    // Copy the function name[line #] to the result
    CopyMemoryW (lpMemRes, lpMemPTD->lpwszErrorMessage, (APLU3264) aplNELMRes);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Free the old value
    FreeResultGlobalVar (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_DM]->stData.stGlbData); lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_DM]->stData.stGlbData = NULL;

    // Save the new value in the STE
    lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_DM]->stData.stGlbData = MakePtrTypeGlb (hGlbRes);

    return;

WSFULL_EXIT:
    MessageBoxW (hWndSM,
                 L"Unable to allocate space for " $QUAD_DM,
                 lpwszAppName,
                 MB_OK | MB_ICONWARNING | MB_APPLMODAL);
    return;
} // End BreakMessage
#undef  APPEND_NAME


//***************************************************************************
//  $AppendFcnNameLineNum
//
//  Append the (suspended) function name and line #.
//***************************************************************************

LPMEMTXT_UNION AppendFcnNameLineNum
    (LPPERTABDATA  lpMemPTD,            // Ptr to PerTabData global memory
     LPSIS_HEADER  lpSISCur,            // Ptr to current SIS header
     LPUINT        lpuNameLen)          // Ptr to length of function name[line #]

{
    LPAPLCHAR      lpMemName;           // Ptr to function name global memory
    LPDFN_HEADER   lpMemDfnHdr;         // Ptr to user-defined function/operator header global memory
    LPFCNLINE      lpFcnLines;          // Ptr to array function line structs (FCNLINE[numFcnLines])
    LPMEMTXT_UNION lpMemTxtLine;        // Ptr to text header/line global memory

    // Include the function name & line #

    // Lock the memory to get a ptr to it
    lpMemName = MyGlobalLock (lpSISCur->hGlbFcnName);

    // Format the name and line #
    *lpuNameLen =
      wsprintfW (lpMemPTD->lpwszTemp,
                 L"%s[%d] ",
                 lpMemName,
                 lpSISCur->CurLineNum);
    // We no longer need this ptr
    MyGlobalUnlock (lpSISCur->hGlbFcnName); lpMemName = NULL;

    // Lock the memory to get a ptr to it
    lpMemDfnHdr = MyGlobalLock (lpSISCur->hGlbDfnHdr);

    // Get a ptr to the line # in error
    if (lpSISCur->CurLineNum EQ 0)
        lpMemTxtLine = MyGlobalLock (lpMemDfnHdr->hGlbTxtHdr);
    else
    {
        // Get ptr to array of function line structs (FCNLINE[numFcnLines])
        lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

        // Get a ptr to the function line, converting to origin-0 from origin-1
        lpMemTxtLine = MyGlobalLock (lpFcnLines[lpSISCur->CurLineNum - 1].hGlbTxtLine);
    } // End IF/ELSE

    // We no longer need this ptr
    MyGlobalUnlock (lpSISCur->hGlbDfnHdr); lpMemDfnHdr = NULL;

    return lpMemTxtLine;
} // End AppendFcnNameLineNum


//***************************************************************************
//  $ErrorMessageDirect
//
//  Signal an error message, directly
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ErrorMessageDirect"
#else
#define APPEND_NAME
#endif

void ErrorMessageDirect
    (LPWCHAR lpwszMsg,          // Ptr to error message text
     LPWCHAR lpwszLine,         // Ptr to the line which generated the error
     UINT    uCaret)            // Position of caret (origin-0)

{
    APLNELM        aplNELMRes;      // Result NELM
    APLUINT        ByteRes;         // # bytes in the result
    HGLOBAL        hGlbRes;         // Result global memory handle
    LPAPLCHAR      lpMemRes;        // Ptr to result global memory
    LPPERTABDATA   lpMemPTD;        // Ptr to PerTabData global memory
    LPSIS_HEADER   lpSISCur,        // Ptr to current SIS header
                   lpSISPrv;        // ...    previous ...
    UINT           uErrMsgLen,      // Error message length
                   uNameLen,        // Length of function name[line #]
                   uErrLinLen,      // Error line length
                   uCaretLen,       // Caret line length
                   uTailLen,        // Length of line tail
                   uMaxLen,         // Maximum length
                   uExecCnt;        // # execute levels
    LPMEMTXT_UNION lpMemTxtLine = NULL; // Ptr to text header/line global memory
    HGLOBAL       *lphGlbQuadEM;    // Ptr to active hGlbQuadEM (in either lpSISCur or lpMemPTD)

#define ERROR_CARET     UTF16_UPCARET   // UTF16_CIRCUMFLEX

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get ptr to current SI stack
    lpSISCur = lpMemPTD->lpSISCur;

    // Split cases based upon the DfnType
    if (lpSISCur)
    {
        // No signalling from here
        lpSISCur->hSigaphore = NULL;

        // Unwind through SI levels as appropriate
        while (lpSISCur && lpSISCur->Unwind)
            lpSISCur = lpSISCur->lpSISPrv;

        // If there's an SIS level, ...
        if (lpSISCur)
        // Split cases based upon the defined function type
        switch (lpSISCur->DfnType)
        {
            case DFNTYPE_OP1:
            case DFNTYPE_OP2:
            case DFNTYPE_FCN:
                // Include the function name & line #
                lpMemTxtLine =
                  AppendFcnNameLineNum (lpMemPTD, lpSISCur, &uNameLen);

                // Ptr to the text
                lpwszLine = &lpMemTxtLine->C;

                break;

            case DFNTYPE_EXEC:
            case DFNTYPE_ERRCTRL:
                // Unwind to non-execute level
                for (lpSISPrv = lpSISCur,
                       uExecCnt = 0;
                     lpSISPrv
                  && (lpSISPrv->DfnType EQ DFNTYPE_EXEC
                   || lpSISPrv->DfnType EQ DFNTYPE_ERRCTRL);
                     lpSISPrv = lpSISPrv->lpSISPrv,
                       uExecCnt++)
                    ;

                // If the preceding SI level is a UDFO, ...
                if (lpSISPrv
                 && (lpSISPrv->DfnType EQ DFNTYPE_OP1
                  || lpSISPrv->DfnType EQ DFNTYPE_OP2
                  || lpSISPrv->DfnType EQ DFNTYPE_FCN))
                {
                    // Include the function name & line #
                    lpMemTxtLine =
                      AppendFcnNameLineNum (lpMemPTD, lpSISPrv, &uNameLen);

                    // Zap trailing blank
                    lpMemPTD->lpwszTemp[lstrlenW (lpMemPTD->lpwszTemp) - 1] = WC_EOS;
                    uNameLen--;

                    // Count in appended execute symbols plus trailing blank
                    uNameLen += uExecCnt + 1;

                    // Include leading marker(s)
                    while (uExecCnt--)
                        lstrcatW (lpMemPTD->lpwszTemp, WS_UTF16_UPTACKJOT);
                    // ...and a trailing blank
                    lstrcatW (lpMemPTD->lpwszTemp, L" ");
                } else
                {
                    // Include a leading marker
                    lstrcpyW (lpMemPTD->lpwszTemp, WS_UTF16_UPTACKJOT L"     ");
                    uNameLen = 6;
                } // End IF/ELSE

                break;

            case DFNTYPE_IMM:       // No action
            case DFNTYPE_QUAD:      // ...
                uNameLen = 0;

                break;

            case DFNTYPE_QQUAD:
            defstop
                break;
        } // End IF/SWITCH
    } else
        uNameLen = 0;

    // Calculate the various lengths
    uErrMsgLen = lstrlenW (lpwszMsg);
    uErrLinLen = lstrlenW (lpwszLine);
    if (uCaret EQ NEG1U)
        uCaretLen = 0;
    else
        uCaretLen  = uNameLen + uCaret;

    // Calculate the length of the []DM vector
    aplNELMRes = uErrMsgLen
               + NL_LEN
               + uNameLen
               + uErrLinLen
               + ((uCaret EQ NEG1U) ? 0
                                    : NL_LEN
                                    + uCaretLen + 1);
    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRes, 1);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_DM_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_DM_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->Perm       = FALSE;   // Already zero from GHND
////lpHeader->SysVar     = FALSE;   // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Fill in the dimension
    *VarArrayBaseToDim (lpMemRes) = aplNELMRes;

    // Skip over the header and dimension to the data
    lpMemRes = VarArrayDataFmBase (lpMemRes);

    // Copy the error message to the result
    CopyMemoryW (lpMemRes, lpwszMsg, uErrMsgLen);
    lpMemRes += uErrMsgLen;

    // Copy a line terminator to the result
    *lpMemRes++ = WC_CR;

    // Copy the function name[line #] to the result
    CopyMemoryW (lpMemRes, lpMemPTD->lpwszTemp, uNameLen);
    lpMemRes += uNameLen;

    // Copy the function line to the result
    CopyMemoryW (lpMemRes, lpwszLine, uErrLinLen);
    lpMemRes += uErrLinLen;

    // If the caret is not -1, display a caret
    if (uCaret NE NEG1U)
    {
        UINT uLen;  // Length

        // Close the last line
        *lpMemRes++ = WC_CR;

        // Get the # leading blanks
        uLen = uCaret + uNameLen;

        // Append the caret
        lpMemRes = FillMemoryW (lpMemRes, uLen, L' ');
       *lpMemRes++ = ERROR_CARET;
///////*lpMemRes++ = WC_EOS;                // Already zero from (GHND)
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Free the old value
    FreeResultGlobalVar (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_DM]->stData.stGlbData); lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_DM]->stData.stGlbData = NULL;

    // Save the new value in the PTD
    lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_DM]->stData.stGlbData = MakePtrTypeGlb (hGlbRes); hGlbRes = NULL;

    //***************************************************************
    // Also create the corresponding value for []EM
    //***************************************************************

    // Get a ptr to the active hGlbQuadEM (in either lpSISCur or lpMemPTD)
    lphGlbQuadEM = GetPtrQuadEM (lpMemPTD);

    // Calculate the maximum length
    uMaxLen = max (uErrMsgLen, uNameLen + uErrLinLen);
    uMaxLen = max (uMaxLen, uCaretLen + 1);

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_CHAR, 3 * uMaxLen, 2);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EM_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EM_EXIT;
    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->Perm       = FALSE;           // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = 3 * uMaxLen;
    lpHeader->Rank       = 2;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = (LPAPLCHAR) VarArrayBaseToDim (lpMemRes);

    // Fill in the result's dimensions
    *((LPAPLDIM) lpMemRes)++ = 3;
    *((LPAPLDIM) lpMemRes)++ = uMaxLen;

    // lpMemRes now points to the data

    // Copy the error message text to the result
    CopyMemoryW (lpMemRes, lpwszMsg, uErrMsgLen);
    lpMemRes += uErrMsgLen;
    uTailLen = uMaxLen - uErrMsgLen;
    lpMemRes = FillMemoryW (lpMemRes, uTailLen, L' ');

    // Copy the function name[line #] to the result
    CopyMemoryW (lpMemRes, lpMemPTD->lpwszTemp, uNameLen);
    lpMemRes += uNameLen;

    // Copy the function line to the result
    CopyMemoryW (lpMemRes, lpwszLine, uErrLinLen);
    lpMemRes += uErrLinLen;
    uTailLen = uMaxLen - (uNameLen + uErrLinLen);
    lpMemRes = FillMemoryW (lpMemRes, uTailLen, L' ');

    // Copy the caret line to the result
    if (uCaret NE NEG1U)
    {
        lpMemRes = FillMemoryW (lpMemRes, uCaretLen, L' ');
       *lpMemRes++ = ERROR_CARET;
        uTailLen = uMaxLen - (uCaretLen + 1);
        lpMemRes = FillMemoryW (lpMemRes, uTailLen, L' ');
    } else
        lpMemRes = FillMemoryW (lpMemRes, uMaxLen, L' ');

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Free the old value of []EM
    FreeResultGlobalVar (*lphGlbQuadEM); *lphGlbQuadEM = NULL;

    // Save the global in the current SIS header
    *lphGlbQuadEM = MakePtrTypeGlb (hGlbRes);

    if (lpMemTxtLine)
    {
        // We no longer need this ptr
        MyGlobalUnlock (MyGlobalHandle (lpMemTxtLine));
    } // End IF

    return;

WSFULL_DM_EXIT:
    MessageBoxW (hWndMF,
                 L"Unable to allocate space for " $QUAD_DM,
                 lpwszAppName,
                 MB_OK | MB_ICONWARNING | MB_APPLMODAL);
    return;
WSFULL_EM_EXIT:
    // WS FULL, so leave it alone
    *lphGlbQuadEM = hGlbQuadEM_DEF;

    MessageBoxW (hWndMF,
                 L"Unable to allocate space for " $QUAD_EM,
                 lpwszAppName,
                 MB_OK | MB_ICONWARNING | MB_APPLMODAL);
    return;
} // End ErrorMessageDirect
#undef  ERROR_CARET
#undef  APPEND_NAME


//***************************************************************************
//  $GetPtrQuadEM
//
//  Return a ptr to the active hGlbQuadEM in either lpSISCur or lpMemPTD
//***************************************************************************

HGLOBAL *GetPtrQuadEM
    (LPPERTABDATA lpMemPTD)         // Ptr to PerTabData global memory

{
    LPSIS_HEADER lpSISCur;          // Ptr to current SIS header

    // Get ptr to current SI stack
    lpSISCur = lpMemPTD->lpSISCur;

    // If there's a []EA/[]EC parent in control, ...
    if (lpSISCur
     && lpSISCur->lpSISErrCtrl NE NULL)
        // Get ptr to current []EA/[]EC parent of the current SI stack
        lpSISCur = lpSISCur->lpSISErrCtrl;
    else
        while (lpSISCur
            && lpSISCur->DfnType NE DFNTYPE_FCN
            && lpSISCur->DfnType NE DFNTYPE_OP1
            && lpSISCur->DfnType NE DFNTYPE_OP2
            && lpSISCur->DfnType NE DFNTYPE_ERRCTRL)
            lpSISCur = lpSISCur->lpSISPrv;

    // If there's an SIS level, ...
    if (lpSISCur)
        return &lpSISCur->hGlbQuadEM;
    else
        return &lpMemPTD->hGlbQuadEM;
} // End GetPtrQuadEM


//***************************************************************************
//  $ErrorMessageIndirect
//
//  Signal an error message, indirectly
//***************************************************************************

void ErrorMessageIndirect
    (LPWCHAR lpwszMsg)

{
    // Save in global for later reference
    GetMemPTD ()->lpwszErrorMessage = lpwszMsg;
} // End ErrorMessageIndirect


//***************************************************************************
//  $ErrorMessageIndirectToken
//
//  Signal an error message, indirectly
//    and set the error token
//***************************************************************************

void ErrorMessageIndirectToken
    (LPAPLCHAR lpwszMsg,            // Ptr to error message
     LPTOKEN   lptkError)           // Ptr to function token

{
    // Set the error message
    ErrorMessageIndirect (lpwszMsg);

    // Set the error token
    ErrorMessageSetToken (lptkError);
} // End ErrorMessageIndirectToken


//***************************************************************************
//  $ErrorMessageSetToken
//
//  Set the error token for an error message
//***************************************************************************

void ErrorMessageSetToken
    (LPTOKEN lptkError)             // Ptr to error token (may be NULL)

{
    // Set the error char index
    ErrorMessageSetCharIndex ((lptkError NE NULL) ? lptkError->tkCharIndex
                                                  : NEG1U);
} // End ErrorMessageSetToken


//***************************************************************************
//  $ErrorMessageSetCharIndex
//
//  Set the error char index for an error message
//***************************************************************************

void ErrorMessageSetCharIndex
    (int tkCharIndex)               // Error char index

{
    LPPLLOCALVARS lpplLocalVars;    // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsPlLocalVars);

    // Set the error char index if we're inside ParseLine
    if (lpplLocalVars)
        lpplLocalVars->tkErrorCharIndex = tkCharIndex;
} // End ErrorMessageSetCharIndex


//***************************************************************************
//  End of File: errmsg.c
//***************************************************************************
