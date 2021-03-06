//***************************************************************************
//  NARS2000 -- Immediate Execution
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


typedef struct tagWFSO          // Struct for WaitForSingleObject
{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    HANDLE       WaitHandle,    // WaitHandle from RegisterWaitForSingleObject
                 hSigaphore,    // Handle to signal next (may be NULL)
                 hThread;       // Thread handle
    HWND         hWndEC;        // Edit Ctrl window handle
} WFSO, *LPWFSO;

#define EXEC_QUAD_ELX_TXT   WS_UTF16_UPTACKJOT $QUAD_ELX
#define EXEC_QUAD_ELX_LEN   strcountof (EXEC_QUAD_ELX_TXT)

#define DISP_QUAD_DM_TXT                       $QUAD_DM
#define DISP_QUAD_DM_LEN    strcountof (DISP_QUAD_DM_TXT)


//***************************************************************************
//  $WaitForImmExecStmt
//
//  Wait callback for ImmExecStmt
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- WaitForImmExecStmt"
#else
#define APPEND_NAME
#endif

VOID CALLBACK WaitForImmExecStmt
    (LPVOID  lpParameter,           // Thread data
     BOOLEAN TimerOrWaitFired)      // Reason

{
    LPWFSO       lpMemWFSO;         // Ptr to WFSO global memory
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    EXIT_TYPES   exitType;          // Exit type (see EXIT_TYPES)
    UBOOL        bValid;            // TRUE iff <lpMemPTD> is valid
#ifdef DEBUG
    DWORD        dwRet;             // Return code from UnregisterWait
#endif

#define hGlbWFSO    ((HGLOBAL) lpParameter)

    // Lock the memory to get a ptr to it
    lpMemWFSO = MyGlobalLock (hGlbWFSO);

    // Save ptr to PerTabData global memory
    TlsSetValue (dwTlsPerTabData, lpMemWFSO->lpMemPTD);

    // Get ptr to PerTabData global memory
    lpMemPTD = lpMemWFSO->lpMemPTD; // Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));
    bValid = IsValidPtr (lpMemPTD, sizeof (lpMemPTD));

    if (bValid)
    {
        dprintfWL9 (L"~~WaitForImmExecStmt (%p)", lpMemWFSO->hThread);
    } // End IF -- MUST use braces as dprintfWLx is empty for non-DEBUG

    // Get the thread's exit code
    GetExitCodeThread (lpMemWFSO->hThread, (LPDWORD) &exitType);

    // If <lpMemPTD> is valid, ...
    if (bValid)
        // Save in global memory
        lpMemPTD->ImmExecExitType = exitType;

    // Cancel the wait operation
#ifdef DEBUG
    dwRet =
#endif
      UnregisterWait (lpMemWFSO->WaitHandle); lpMemWFSO->WaitHandle = NULL;
    Assert (dwRet EQ 0 || dwRet EQ ERROR_IO_PENDING);

    // If there's a next level up, ...
    if (lpMemWFSO->hSigaphore)
        // Signal the next level
        MyReleaseSemaphore (lpMemWFSO->hSigaphore, 1, NULL);
    else
    // If the current SIS level is not Quad input, ...
    if (lpMemPTD->lpSISCur EQ NULL
     || lpMemPTD->lpSISCur->DfnType NE DFNTYPE_QUAD)
        // Display the default prompt
        DisplayPrompt (lpMemWFSO->hWndEC, 10);

    // Free the MPFR cache
    mpfr_free_cache ();

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbWFSO, lpMemWFSO);
#undef  hGlbWFSO
} // End WaitForImmExecStmt
#undef  APPEND_NAME


//***************************************************************************
//  $GetBlockStartLine
//
//  Return the starting line of a block of continued lines
//***************************************************************************

UINT GetBlockStartLine
    (HWND hWndEC,                           // Handle of Edit Ctrl window
     UINT uLineNum)                         // Line #

{
    // While the preceding physical line continues to the current line, ...
    while (uLineNum NE 0
        && SendMessageW (hWndEC, MYEM_ISLINECONT, uLineNum - 1, 0) EQ TRUE)
        // Back off to previous line #
        uLineNum--;

    return uLineNum;
} // End GetBlockStartLine


//***************************************************************************
//  $GetBlockEndLine
//
//  Return the ending line of a block of continued lines
//***************************************************************************

UINT GetBlockEndLine
    (HWND hWndEC,                           // Handle of Edit Ctrl window
     UINT uLineNum)                         // Line #

{
    // While the current physical line continues to the next line, ...
    while (SendMessageW (hWndEC, MYEM_ISLINECONT, uLineNum, 0) EQ TRUE)
        // Skip to next line #
        uLineNum++;

    return uLineNum;
} // End GetBlockEndLine


//***************************************************************************
//  $GetBlockLength
//
//  Return the length in WCHARs of a block including CR/CR/LFs
//
//  Note that this function does not include a terminating zero
//    in the returned length
//***************************************************************************

UINT GetBlockLength
    (HWND hWndEC,           // Handle of Edit Ctrl window
     UINT uLineNum)         // Starting line #

{
    UINT uLinePos,          // Char position of start of line
         uLineLen = 0;      // Line length

    // While the current physical line continues to the next line, ...
    while (SendMessageW (hWndEC, MYEM_ISLINECONT, uLineNum, 0) EQ TRUE)
    {
        // Get the position of the start of the line
        uLinePos = (UINT) SendMessageW (hWndEC, EM_LINEINDEX, uLineNum, 0);

        // Get the line length including the trailing WS_CRCRLF
        uLineLen += strcountof (WS_CRCRLF) + (UINT) SendMessageW (hWndEC, EM_LINELENGTH, uLinePos, 0);

        // Skip to the next line
        uLineNum++;
    } // End WHILE

    // Once more to get the length of the last (non-continued) line

    // Get the position of the start of the line
    uLinePos = (UINT) SendMessageW (hWndEC, EM_LINEINDEX, uLineNum, 0);

    // Get the line length
    uLineLen += (UINT) SendMessageW (hWndEC, EM_LINELENGTH, uLinePos, 0);

    return uLineLen;
} // End GetBlockLength


//***************************************************************************
//  $GetLogicalLineCountFE
//
//  Return the # logical lines in a function (excluding the header
//    which might itself take up more than one line)
//***************************************************************************

UINT GetLogicalLineCountFE
    (HWND    hWndEC)                        // Handle of FE Edit Ctrl window

{
    UINT    uLineNum = 0,                   // Line # counter
            uLineCnt,                       // # physical lines in the function
            uLineLog = 0;                   // # logical lines in the function

    Assert (IzitFE (GetParent (hWndEC)));

    // Get the # physical lines in the function including the header
    uLineCnt = (UINT) SendMessageW (hWndEC, EM_GETLINECOUNT, 0, 0);

    // Loop through the physical lines
    for (uLineNum = 0; uLineNum < uLineCnt; uLineNum++)
    // If the current physical line does not continue to the next line, ...
    if (SendMessageW (hWndEC, MYEM_ISLINECONT, uLineNum, 0) EQ FALSE)
        uLineLog++;

    return uLineLog - 1;
} // End GetLogicalLineCountFE


//***************************************************************************
//  $CopyBlockLines
//
//  Copy a block of lines
//
//  Note that this function copies a terminating zero if there's enough room
//    but it is not included in the returned length
//***************************************************************************

UINT CopyBlockLines
    (HWND    hWndEC,                        // Handle of Edit Ctrl window
     UINT    uLineNum,                      // Starting line #
     LPWCHAR lpwszLine)                     // Ptr to output buffer

{
    UINT uLineLen,                          // Line length
         uBlockLen = 0;                     // Block length

    Assert (IzitSM (GetParent (hWndEC)) || IzitFE (GetParent (hWndEC)));

    // While the current physical line continues to the next line, ...
    while (SendMessageW (hWndEC, MYEM_ISLINECONT, uLineNum, 0) EQ TRUE)
    {
        // Tell EM_GETLINE maximum # chars in the buffer
        // Because we allocated space for the terminating zero,
        //   we don't have to worry about overwriting the
        //   allocation limits of the buffer
        ((LPWORD) lpwszLine)[0] = (WORD) -1;

        // Get the contents of the line
        uLineLen = (UINT) SendMessageW (hWndEC, EM_GETLINE, uLineNum, (LPARAM) lpwszLine);

        // Skip to the next line ptr
        lpwszLine = &lpwszLine[uLineLen];

        // Append and skip over a Line Continuation marker
        lstrcpyW (lpwszLine, WS_CRCRLF); lpwszLine += strcountof (WS_CRCRLF);

        // Accumulate in result
        uBlockLen += uLineLen + strcountof (WS_CRCRLF);

        // Skip to the next line
        uLineNum++;
    } // End WHILE

    // Once more to get the contents of the last (non-continued) line

    // Tell EM_GETLINE maximum # chars in the buffer
    // Because we allocated space for the terminating zero,
    //   we don't have to worry about overwriting the
    //   allocation limits of the buffer
    ((LPWORD) lpwszLine)[0] = (WORD) -1;

    // Get the contents of the line
    uLineLen = (UINT) SendMessageW (hWndEC, EM_GETLINE, uLineNum, (LPARAM) lpwszLine);

    // Accumulate in result
    return uBlockLen + uLineLen;
} // End CopyBlockLines


//***************************************************************************
//  $ImmExecLine
//
//  Execute a line (sys/user command, fn defn, etc.)
//    in immediate execution mode
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ImmExecLine"
#else
#define APPEND_NAME
#endif

void ImmExecLine
    (HWND hWndEC,                           // Handle of Edit Ctrl window
     UINT uLineNum)                         // Line #

{
    LPPERTABDATA lpMemPTD;                  // Ptr to PerTabData global memory
    LPWCHAR      lpwszCompLine,             // Ptr to complete line
                 lpwszLine;                 // Ptr to line following leading blanks
    UINT         uLineBeg,                  // Line # of start of continuation
                 uLineLen = 0;              // Line length
    SYSCMDS_ENUM sysCmdEnum = SYSCMD_None;  // Type of system command (if any)

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the line # of the start of a block of a Line Continuations
    uLineBeg = GetBlockStartLine (hWndEC, uLineNum);

    // Get the overall block length
    //   not including a terminating zero
    uLineLen = GetBlockLength (hWndEC, uLineBeg);

    // Allocate virtual memory for the line (along with its continuations)
    lpwszCompLine =
      MyVirtualAlloc (NULL,             // Any address (FIXED SIZE)
                      (uLineLen + 1) * sizeof (lpwszCompLine[0]),   // "+ 1" for the terminating zero
                      MEM_COMMIT | MEM_TOP_DOWN,
                      PAGE_READWRITE);
    if (lpwszCompLine EQ NULL)
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsgW (L"ImmExecLine:  VirtualAlloc for <lpwszCompLine> failed");

        return;                 // Mark as failed
    } // End IF

    // Copy a block of lines
    //   including a terminating zero if there's enough room
    CopyBlockLines (hWndEC, uLineBeg, lpwszCompLine);

    // Ensure properly terminated
    lpwszCompLine[uLineLen] = WC_EOS;

    // Strip off leading blanks
    for (lpwszLine = lpwszCompLine;
         lpwszLine[0] && lpwszLine[0] EQ L' ';
         lpwszLine++)
    {}

    // Split cases based upon the first non-blank char
    switch (lpwszLine[0])
    {
        case L')':          // System commands
            // Execute the command (ignore the result)
            sysCmdEnum =
              ExecSysCmd (lpwszLine, hWndEC);

            // If it's Quad input, and we're not resetting, ...
            if (lpMemPTD->lpSISCur
             && lpMemPTD->lpSISCur->ResetFlag EQ RESETFLAG_NONE
             && lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QUAD)
                // Tell the SM to display the Quad Input Prompt
                PostMessageW (lpMemPTD->hWndSM, MYWM_QUOTEQUAD, FALSE, 100);
            break;

        case L']':          // User commands
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
            // ***FIXME***
            DbgMsgW (L"User command");
#endif
            AppendLine (ERRMSG_NONCE_ERROR, FALSE, TRUE);

            // If it's Quad input, ...
            if (lpMemPTD->lpSISCur
             && lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QUAD)
                // Tell the SM to display the Quad Input Prompt
                PostMessageW (lpMemPTD->hWndSM, MYWM_QUOTEQUAD, FALSE, 101);
            break;

        case UTF16_DEL:     // Function definition
            // Create the Function Editor Window (ignore the result)
            CreateFcnWindow (lpwszLine);

            // Fall through to empty line case

        case WC_EOS:        // Empty line
            // If it's Quad input, ...
            if (lpMemPTD->lpSISCur
             && lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QUAD)
                // Tell the SM to display the Quad Input Prompt
                PostMessageW (lpMemPTD->hWndSM, MYWM_QUOTEQUAD, FALSE, 102);
            break;

        default:
            // Execute the statement
            ImmExecStmt (lpwszCompLine,             // Ptr to line to execute
                         lstrlenW (lpwszCompLine),  // NELM of lpwszCompLine
                         TRUE,                      // TRUE iff we should VirtualFree lpwszCompLine on completion
                         hWndEC,                    // Edit Ctrl window handle
                         TRUE);                     // TRUE iff errors are acted upon
            return;
    } // End SWITCH

    // If no SIS layer or not Quad input and not reset all, ...
    if (sysCmdEnum NE SYSCMD_RESET
     && (lpMemPTD->lpSISCur EQ NULL
      || (lpMemPTD->lpSISCur->DfnType NE DFNTYPE_QUAD
       && lpMemPTD->lpSISCur->ResetFlag NE RESETFLAG_ALL)))
        // Display the default prompt
        DisplayPrompt (hWndEC, 4);

    // Free the virtual memory for the complete line
    MyVirtualFree (lpwszCompLine, 0, MEM_RELEASE); lpwszCompLine = NULL;
} // End ImmExecLine
#undef  APPEND_NAME


//***************************************************************************
//  $ImmExecStmt
//
//  Execute a statement
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ImmExecStmt"
#else
#define APPEND_NAME
#endif

EXIT_TYPES ImmExecStmt
    (LPWCHAR lpwszCompLine,     // Ptr to line to execute
     APLNELM aplNELM,           // NELM of lpwszCompLine
     UBOOL   bFreeLine,         // TRUE iff we should VirtualFree lpwszCompLine on completion
     HWND    hWndEC,            // Edit Ctrl window handle
     UBOOL   bActOnErrors)      // TRUE iff errors are acted upon

{
    HANDLE     hThread;         // Thread handle
    DWORD      dwThreadId;      // The thread ID #
    HGLOBAL    hGlbWFSO;        // WFSO global memory handle
    LPWFSO     lpMemWFSO;       // Ptr to WFSO global memory
    EXIT_TYPES exitType;        // Exit code from thread
    static IE_THREAD ieThread;  // Temporary global

    // Allocate memory for the WaitForSingleObject struct
    hGlbWFSO = DbgGlobalAlloc (GHND, sizeof (WFSO));

    // Save args in struc to pass to thread func
    ieThread.hWndEC         = hWndEC;
    ieThread.lpMemPTD       = GetMemPTD ();
    ieThread.lpwszCompLine  = lpwszCompLine;
    ieThread.aplNELM        = aplNELM;
    ieThread.hGlbWFSO       = hGlbWFSO;
    ieThread.bFreeLine      = bFreeLine;
    ieThread.bActOnErrors   = bActOnErrors;

    // Lock the memory to get a ptr to it
    lpMemWFSO = MyGlobalLock000 (hGlbWFSO);

    // Fill in the struct
    lpMemWFSO->lpMemPTD = ieThread.lpMemPTD;
    lpMemWFSO->hWndEC   = hWndEC;

    // Create a new thread
    hThread = CreateThread (NULL,                   // No security attrs
                            0,                      // Use default stack size
                           &ImmExecStmtInThread,    // Starting routine
                           &ieThread,               // Param to thread func
                            CREATE_SUSPENDED,       // Creation flag
                           &dwThreadId);            // Returns thread id
    // Save the thread handle
    lpMemWFSO->hThread = hThread;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbWFSO); lpMemWFSO = NULL;

    // Check for LIMIT ERROR
    if (hThread EQ NULL)
        goto LIMIT_EXIT;

    dprintfWL9 (L"~~RegisterWaitForSingleObject (%p) (%S#%d)", hThread, FNLN);

    // Lock the memory to get a ptr to it
    lpMemWFSO = MyGlobalLock (hGlbWFSO);

    // Tell W to callback when this thread terminates
    RegisterWaitForSingleObject (&lpMemWFSO->WaitHandle,// Return wait handle
                                  hThread,              // Handle to wait on
                                 &WaitForImmExecStmt,   // Callback function
                                  hGlbWFSO,             // Callback function parameter
                                  INFINITE,             // Wait time in milliseconds
                                  WT_EXECUTEONLYONCE);  // Options
    // We no longer need this ptr
    MyGlobalUnlock (hGlbWFSO); lpMemWFSO = NULL;

    // Start 'er up
    ResumeThread (hThread);

    exitType = EXITTYPE_NONE;

    goto NORMAL_EXIT;

LIMIT_EXIT:
    ErrorMessageIndirect (ERRMSG_LIMIT_ERROR APPEND_NAME);

    exitType = EXITTYPE_ERROR;

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    return exitType;
} // End ImmExecStmt
#undef  APPEND_NAME


//***************************************************************************
//  $InitPerThreadVars
//
//  Initialize per-thread vars
//***************************************************************************

void InitPerThreadVars
    (LPPERTABDATA lpMemPTD)                 // Ptr to PerTabData global memory

{
    // Set the MPFR default precision
    mpfr_set_default_prec ((mpfr_prec_t) lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_FPC]->stData.stInteger);

    // Set the MPFR default rounding mode
    mpfr_set_default_rounding_mode (MPFR_RNDN);
} // InitPerThreadVars


//***************************************************************************
//  $ImmExecStmtInThread
//
//  Execute a line (sys/user command, fn defn, etc.)
//    in immediate execution mode
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ImmExecStmtInThread"
#else
#define APPEND_NAME
#endif

DWORD WINAPI ImmExecStmtInThread
    (LPIE_THREAD lpieThread)            // Ptr to IE_THREAD struc

{
    HANDLE         hSigaphore = NULL;   // Semaphore handle to signal (NULL if none)
    LPWCHAR        lpwszCompLine;       // Ptr to complete line
    APLNELM        aplNELM;             // NELM of lpwszCompLine
    HGLOBAL        hGlbTknHdr,          // Handle of tokenized line header
                   hGlbWFSO;            // WaitForSingleObject callback global memory handle
    HWND           hWndEC,              // Handle of Edit Ctrl window
                   hWndSM;              // ...       Session Manager ...
    LPPERTABDATA   lpMemPTD;            // Ptr to this window's PerTabData
    RESET_FLAGS    resetFlag;           // Reset flag (see RESET_FLAGS)
    UBOOL          bFreeLine,           // TRUE iff we should VirtualFree lpszCompLine on completion
                   bResetAll = FALSE,   // TRUE iff )RESET about to finish
                   bActOnErrors;        // TRUE iff errors are acted upon
    EXIT_TYPES     exitType;            // Return code from ParseLine
    LPWFSO         lpMemWFSO;           // Ptr to WFSO global memory
    LPSIS_HEADER   lpSISPrv;            // Ptr to previous SIS header
    LPTOKEN        lptkCSBeg;           // Ptr to next token on the CS stack
    CSLOCALVARS    csLocalVars = {0};   // CS local vars
    LPTOKEN_HEADER lpMemTknHdr;         // Ptr to tokenized line header global memory

    __try
    {
        // Save the thread type ('IE')
        TlsSetValue (dwTlsType, TLSTYPE_IE);

        // Extract values from the arg struc
        hWndEC         = lpieThread->hWndEC;
        lpMemPTD       = lpieThread->lpMemPTD;
        lpwszCompLine  = lpieThread->lpwszCompLine;
        aplNELM        = lpieThread->aplNELM;
        hGlbWFSO       = lpieThread->hGlbWFSO;
        bFreeLine      = lpieThread->bFreeLine;
        bActOnErrors   = lpieThread->bActOnErrors;

        // Save ptr to PerTabData global memory
        TlsSetValue (dwTlsPerTabData, lpMemPTD);

        // Initialize per-thread vars
        InitPerThreadVars (lpMemPTD);

        dprintfWL9 (L"--Starting thread in <ImmExecStmtInThread>.");

        // Get the window handle of the Session Manager
        hWndSM = GetParent (hWndEC);

        // Initialize the Reset Flag & Exit Type
        resetFlag = RESETFLAG_NONE;
        exitType  = EXITTYPE_NONE;

        // Save the ptr to the next token on the CS stack
        lptkCSBeg = lpMemPTD->lptkCSNxt;

        // Fill in the SIS header for Immediate Execution Mode
        FillSISNxt (lpMemPTD,               // Ptr to PerTabData global memory
                    NULL,                   // Semaphore handle
                    DFNTYPE_IMM,            // DfnType
                    FCNVALENCE_IMM,         // FcnValence
                    FALSE,                  // Suspended
                    TRUE,                   // Restartable
                    TRUE);                  // LinkIntoChain
        // Tokenize, parse, and untokenize the line

        // Tokenize the line
        hGlbTknHdr =
          Tokenize_EM (lpwszCompLine,       // The line to tokenize (not necessarily zero-terminated)
                       aplNELM,             // NELM of lpwszLine
                       hWndEC,              // Window handle for Edit Ctrl (may be NULL if lpErrHandFn is NULL)
                       1,                   // Function line # (0 = header)
                      &ErrorMessageDirect,  // Ptr to error handling function (may be NULL)
                       NULL,                // Ptr to common struc (may be NULL if unused)
                       FALSE);              // TRUE iff we're tokenizing a Magic Function/Operator
        // If it's invalid, ...
        if (hGlbTknHdr EQ NULL)
        {
            // If we should act on this error, ...
            exitType = bActOnErrors ? ActOnError (hWndSM, lpMemPTD) : EXITTYPE_ERROR;

            goto ERROR_EXIT;
        } // End IF

        // Fill in the CS local vars struc
        csLocalVars.hWndEC      = hWndEC;
        csLocalVars.lpMemPTD    = lpMemPTD;
        csLocalVars.lptkCSBeg   =
        csLocalVars.lptkCSNxt   = lptkCSBeg;
        csLocalVars.lptkCSLink  = NULL;
        csLocalVars.hGlbDfnHdr  = NULL;
        csLocalVars.hGlbImmExec = hGlbTknHdr;

        // Parse the tokens on the CS stack
        if (!ParseCtrlStruc_EM (&csLocalVars))
        {
            WCHAR wszTemp[1024];

            // Format the error message
            MySprintfW (wszTemp,
                        sizeof (wszTemp),
                       L"%s -- Line %d statement %d",
                        csLocalVars.lpwszErrMsg,
                        csLocalVars.tkCSErr.tkData.Orig.c.uLineNum,
                        csLocalVars.tkCSErr.tkData.Orig.c.uStmtNum + 1);
            // Create []DM & []EM
            ErrorMessageDirect (wszTemp,                            // Ptr to error message text
                                lpwszCompLine,                      // Ptr to the line which generated the error
                                csLocalVars.tkCSErr.tkCharIndex);   // Position of caret (origin-0)
            // If we should act on this error, ...
            exitType = bActOnErrors ? ActOnError (hWndSM, lpMemPTD) : EXITTYPE_ERROR;

            goto UNTOKENIZE_EXIT;
        } // End IF

        // If we're called from LoadWorkspaceGlobal_EM
        //  and we can't send a message to another thread.
        // Set the cursor to indicate the new state
        ForceSendCursorMsg (hWndEC, TRUE);

        // Lock the memory to get a ptr to it
        lpMemTknHdr = MyGlobalLockTkn (hGlbTknHdr);

        // Execute the line
        exitType =
          ParseLine (hWndSM,                // Session Manager window handle
                     lpMemTknHdr,           // Ptr to tokenized line header global memory
                     NULL,                  // Text of tokenized line global mamory handle
                     lpwszCompLine,         // Ptr to the complete line
                     lpMemPTD,              // Ptr to PerTabData global memory
                     1,                     // Function line #  (1 for execute or immexec)
                     0,                     // Starting token # in the above function line
                     FALSE,                 // TRUE iff we're tracing this line
                     NULL,                  // User-defined function/operator global memory handle (NULL = execute/immexec)
                     NULL,                  // Ptr to function token used for AFO function name
                     bActOnErrors,          // TRUE iff errors are acted upon
                     FALSE,                 // TRUE iff executing only one stmt
                     FALSE);                // TRUE iff we're to skip the depth check
        // The matching <ForceSendCursorMsg (hWndEC, FALSE)> is
        //   in <DisplayPrompt>.

        // We no longer need this ptr
        MyGlobalUnlock (hGlbTknHdr); lpMemTknHdr = NULL;

        // Start with the preceding layer (if any)
        lpSISPrv = lpMemPTD->lpSISCur->lpSISPrv;

        // Skip over SI levels of ImmExec
        while (lpSISPrv
            && lpSISPrv->DfnType EQ DFNTYPE_IMM)
            lpSISPrv = lpSISPrv->lpSISPrv;

        // Split cases based upon the exit type
        switch (exitType)
        {
            case EXITTYPE_STOP:
            case EXITTYPE_QUADERROR_INIT:
                // If there are no more SI layers, ...
                if (lpSISPrv EQ NULL)
                {
                    // ***FIXME*** -- Set the tkCharIndex for the error???

                    // Create []DM & []EM
                    ErrorMessageDirect (lpMemPTD->lpwszErrorMessage,    // Ptr to error message text
                                        lpwszCompLine,                  // Ptr to the line which generated the error
                                        lpMemPTD->tkErrorCharIndex);    // Position of caret (origin-0)
                    if (bActOnErrors)
                    {
                        HWND hWndEC;

                        // Get the Edit Ctrl window handle
                        hWndEC = (HWND) (HANDLE_PTR) GetWindowLongPtrW (hWndSM, GWLSF_HWNDEC);

                        // If it's STOP, ...
                        if (exitType EQ EXITTYPE_STOP)
                        {
                            UBOOL bCtrlBreak = FALSE;               // Pseudo Ctrl-Break

                            // Display []DM
                            DisplayGlbArr_EM (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_DM]->stData.stGlbData,   // Global memory handle to display
                                              TRUE,                                                         // TRUE iff last line has CR
                                             &bCtrlBreak,                                                   // Ptr to Ctrl-Break flag
                                              NULL);                                                        // Ptr to function token
                        } else
                        {
                            // Execute []ELX
                            exitType =
                              PrimFnMonUpTackJotCSPLParse (hWndEC,              // Edit Ctrl window handle
                                                           lpMemPTD,            // Ptr to PerTabData global memory
                                                           EXEC_QUAD_ELX_TXT,   // Ptr to text of line to execute
                                                           EXEC_QUAD_ELX_LEN,   // Length of the line to execute
                                                           TRUE,                // TRUE iff we should act on errors
                                                           FALSE,               // TRUE iff we're to skip the depth check
                                                           DFNTYPE_EXEC,        // DfnType for FillSISNxt
                                                           NULL);               // Ptr to function token
                        } // End IF/ELSE
                    } // End IF

                    // Set the reset flag
                    lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_NONE;

                    exitType = EXITTYPE_NONE;

                    break;
                } // End IF

                // If the previous layer in the SI stack is a
                //   User-Defined Function/Operator or Quad Input,
                //   signal it to handle this action
                if (lpSISPrv
                 && (lpSISPrv->DfnType EQ DFNTYPE_OP1
                  || lpSISPrv->DfnType EQ DFNTYPE_OP2
                  || lpSISPrv->DfnType EQ DFNTYPE_FCN
                  || lpSISPrv->DfnType EQ DFNTYPE_QUAD))
                {
                    // Lock the memory to get a ptr to it
                    lpMemWFSO = MyGlobalLock (hGlbWFSO);

                    Assert (lpSISPrv->hSemaphore NE NULL);

                    // Tell the wait handler to signal this layer
                    hSigaphore = lpMemWFSO->hSigaphore = lpSISPrv->hSemaphore;

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbWFSO); lpMemWFSO = NULL;
                } // End IF

                break;

            case EXITTYPE_QUADERROR_EXEC:
                if (bActOnErrors)
                    // Execute []ELX
                    exitType =
                      PrimFnMonUpTackJotCSPLParse ((HWND) (HANDLE_PTR) GetWindowLongPtrW (hWndSM, GWLSF_HWNDEC), // Edit Ctrl window handle
                                                   lpMemPTD,                                // Ptr to PerTabData global memory
                                                   EXEC_QUAD_ELX_TXT,                       // Ptr to text of line to execute
                                                   EXEC_QUAD_ELX_LEN,                       // Length of the line to execute
                                                   TRUE,                                    // TRUE iff we should act on errors
                                                   FALSE,                                   // TRUE iff we're to skip the depth check
                                                   DFNTYPE_EXEC,                            // DfnType for FillSISNxt
                                                   NULL);                                   // Ptr to function token
                // Set the reset flag
                lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_NONE;

                exitType = EXITTYPE_NONE;

                break;

            case EXITTYPE_GOTO_ZILDE:   // Nothing more to do with these types
            case EXITTYPE_NONE:         // ...
            case EXITTYPE_ERROR:        // ...
            case EXITTYPE_STACK_FULL:   // ...
            case EXITTYPE_RETURNxLX:    // ...
                break;

            case EXITTYPE_NODISPLAY:    // Signal previous SI layer's semaphore if it's Quad input
            case EXITTYPE_DISPLAY:      // ...
            case EXITTYPE_NOVALUE:      // ...
                // If the previous layer in the SI stack is Quad input, ...
                if (lpSISPrv
                 && lpSISPrv->DfnType EQ DFNTYPE_QUAD)
                {
                    // If there's no return value, ...
                    if (lpMemPTD->YYResExec.tkToken.tkFlags.TknType EQ 0
                     || (lpMemPTD->YYResExec.tkToken.tkFlags.TknType EQ TKT_VARNAMED
                      && lpMemPTD->YYResExec.tkToken.tkData.tkSym->stFlags.Value EQ FALSE))
                        // Tell SM to display the Quad Prompt
                        PostMessageW (hWndSM, MYWM_QUOTEQUAD, FALSE, 104);
                    else
                    // else, signal it to receive this value
                    {
                        // Lock the memory to get a ptr to it
                        lpMemWFSO = MyGlobalLock (hGlbWFSO);

                        Assert (lpSISPrv->hSemaphore NE NULL);

                        // Tell the wait handler to signal this layer
                        hSigaphore = lpMemWFSO->hSigaphore = lpSISPrv->hSemaphore;

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbWFSO); lpMemWFSO = NULL;
                    } // End IF/ELSE
                } // End IF

                break;

            case EXITTYPE_RESET_ONE:        // Stop at this level
                exitType = EXITTYPE_NOVALUE;
                lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_NONE;

                break;

            case EXITTYPE_RESET_ONE_INIT:   // Change to EXITTYPE_RESET_ONE
                exitType = EXITTYPE_RESET_ONE;
                lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_ONE;

                // Fall through to common code

            case EXITTYPE_RESET_ALL:        // Continue resetting if more layers
                // If there are no more SI layers, ...
                if (lpSISPrv EQ NULL
                 && bActOnErrors)
                {
                    // Set the ResetAll flag for later use
                    bResetAll = TRUE;

                    break;
                } // End IF

                // Fall through to common code

            case EXITTYPE_GOTO_LINE:        // Signal previous SI layer's semaphore if it's user-defined function/operator
                // If the previous layer in the SI stack is a
                //   User-Defined Function/Operator or Quad Input,
                //   signal it to handle this action
                if (lpSISPrv
                 && (lpSISPrv->DfnType EQ DFNTYPE_OP1
                  || lpSISPrv->DfnType EQ DFNTYPE_OP2
                  || lpSISPrv->DfnType EQ DFNTYPE_FCN
                  || lpSISPrv->DfnType EQ DFNTYPE_QUAD))
                {
                    // Lock the memory to get a ptr to it
                    lpMemWFSO = MyGlobalLock (hGlbWFSO);

                    Assert (lpSISPrv->hSemaphore NE NULL);

                    // Tell the wait handler to signal this layer
                    hSigaphore = lpMemWFSO->hSigaphore = lpSISPrv->hSemaphore;

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbWFSO); lpMemWFSO = NULL;
                } // End IF

                break;

            defstop
                break;
        } // End SWITCH

        // Get the reset flag
        resetFlag = lpMemPTD->lpSISCur->ResetFlag;
UNTOKENIZE_EXIT:
        // Lock the memory to get a ptr to it
        lpMemTknHdr = MyGlobalLockTkn (hGlbTknHdr);

        // Free the tokens
        Untokenize (lpMemTknHdr);

        // Unlock and free (and set to NULL) a global name and ptr
        UnlFreeGlbName (hGlbTknHdr, lpMemTknHdr);
ERROR_EXIT:
        // Unlocalize the STEs on the innermost level
        //   and strip off one level
        UnlocalizeSTEs (NULL);

        dprintfWL9 (L"--Ending   thread in <ImmExecStmtInThread>.");

        // Restore the ptr to the next token on the CS stack
        lpMemPTD->lptkCSNxt = lptkCSBeg;

        // If we should free the virtual memory for the complete line, ...
        if (bFreeLine)
        {
            MyVirtualFree (lpwszCompLine, 0, MEM_RELEASE); lpwszCompLine = NULL;
        } // End IF

        // If there's an hExitphore pending from a )RESET, release it
        if (bResetAll && lpMemPTD->hExitphore)
            // Start executing at the Tab Delete code
            MyReleaseSemaphore (lpMemPTD->hExitphore, 1, NULL);

        return exitType;
    } __except (CheckException (GetExceptionInformation (), L"ImmExecStmtInThread"))
    {
        // Display message for unhandled exception
        DisplayException ();

        return EXITTYPE_ERROR;      // To keep the compiler happy
    } // End __try/__except
} // End ImmExecStmtInThread
#undef  APPEND_NAME


//***************************************************************************
//  $ActOnError
//
//  Act on an error in immmediate execution mode
//***************************************************************************

EXIT_TYPES ActOnError
    (HWND         hWndSM,               // Session Manager window handle
     LPPERTABDATA lpMemPTD)             // Ptr to PerTabData global memory

{
    EXIT_TYPES exitType;                // Exit type
    UBOOL      bFALSE = FALSE;

    // Execute []ELX
    exitType =
      PrimFnMonUpTackJotCSPLParse ((HWND) (HANDLE_PTR) GetWindowLongPtrW (hWndSM, GWLSF_HWNDEC), // Edit Ctrl window handle
                                   lpMemPTD,                                // Ptr to PerTabData global memory
                                   EXEC_QUAD_ELX_TXT,                       // Ptr to text of line to execute
                                   EXEC_QUAD_ELX_LEN,                       // Length of the line to execute
                                   TRUE,                                    // TRUE iff we should act on errors
                                   FALSE,                                   // TRUE iff we're to skip the depth check
                                   DFNTYPE_EXEC,                            // DfnType for FillSISNxt
                                   NULL);                                   // Ptr to function token
    // Split cases based upon the exit type
    switch (exitType)
    {
        case EXITTYPE_GOTO_LINE:        // Pass on to caller
        case EXITTYPE_RESET_ALL:        // ...
        case EXITTYPE_RESET_ONE:        // ...
        case EXITTYPE_RESET_ONE_INIT:   // ...
        case EXITTYPE_STACK_FULL:       // ...
            break;

        case EXITTYPE_NODISPLAY:        // Display the result (if any)
        case EXITTYPE_DISPLAY:          // ...
            // If the Execute/Quad result is present, display it
            if (lpMemPTD->YYResExec.tkToken.tkFlags.TknType NE TKT_UNUSED)
            {
                // Display the result
                ArrayDisplay_EM (&lpMemPTD->YYResExec.tkToken, TRUE, &bFALSE);

                // Free it
                FreeResult (&lpMemPTD->YYResExec);

                // Zap the token type
                lpMemPTD->YYResExec.tkToken.tkFlags.TknType = 0;
            } // End IF

            // Fall through to common code

        case EXITTYPE_ERROR:            // Mark as in error (from the error in Tokenize_EM)
        case EXITTYPE_NOVALUE:          // ...
        case EXITTYPE_GOTO_ZILDE:       // ...
            exitType = EXITTYPE_ERROR;

            break;

        case EXITTYPE_NONE:
        defstop
            break;
    } // End SWITCH

    return exitType;
} // End ActOnError


//***************************************************************************
//  End of File: immexec.c
//***************************************************************************
