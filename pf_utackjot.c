//***************************************************************************
//  NARS2000 -- Primitive Function -- UpTackJot
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
#include <math.h>
#include "headers.h"


//***************************************************************************
//  $PrimFnUpTackJot_EM_YY
//
//  Primitive function for monadic and dyadic UpTackJot ("execute" and ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnUpTackJot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnUpTackJot_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_UPTACKJOT);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc APPEND_NAME_ARG);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonUpTackJot_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydUpTackJot_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimFnUpTackJot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnUpTackJot_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic UpTackJot
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnUpTackJot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnUpTackJot_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnUpTackJot_EM_YY,  // Ptr to primitive function routine
                                    lptkLftArg,             // Ptr to left arg token
                                    lptkFunc,               // Ptr to function token
                                    lptkRhtArg,             // Ptr to right arg token
                                    lptkAxis);              // Ptr to axis token (may be NULL)
} // End PrimProtoFnUpTackJot_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonUpTackJot_EM_YY
//
//  Primitive function for monadic UpTackJot ("execute")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonUpTackJot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonUpTackJot_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE aplTypeRht;            // Right arg storage type
    APLNELM  aplNELMRht;            // Right arg NELM
    APLRANK  aplRankRht;            // Right arg rank
    HGLOBAL  hGlbRht;               // Right arg global memory handle

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RIGHT RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RANK_EXIT;

    // Check for RIGHT DOMAIN ERROR
    if ((!IsSimpleChar (aplTypeRht)) && !IsEmpty (aplNELMRht))
        goto DOMAIN_EXIT;

    // Check for empty case
    if (IsEmpty (aplNELMRht))
        // Return PL_YYSTYPE NoValue entry
        return MakeNoValue_YY (lptkFunc);

    // Split cases based upon the right arg's token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
            {
                // Get the global memory handle
                hGlbRht = lptkRhtArg->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir_PTB (hGlbRht));

                break;          // Join common global code
            } // End IF

            // Handle the immediate case
            return PrimFnMonUpTackJotImm_EM_YY
                   (lptkRhtArg->tkData.tkSym->stData.stLongest, // Immediate value
                    lptkAxis,                                   // Ptr to axis token (may be NULL)
                    lptkFunc);                                  // Ptr to function token
        case TKT_VARIMMED:
            return PrimFnMonUpTackJotImm_EM_YY
                   (lptkRhtArg->tkData.tkLongest,               // Immediate value
                    lptkAxis,                                   // Ptr to axis token (may be NULL)
                    lptkFunc);                                  // Ptr to function token
        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (lptkRhtArg->tkData.tkGlbData));

            // Get the global memory handle
            hGlbRht = lptkRhtArg->tkData.tkGlbData;

            break;          // Join common global code

        defstop
            return NULL;
    } // End SWITCH

    return PrimFnMonUpTackJotGlb_EM_YY (hGlbRht,                // HGLOBAL
                                        lptkAxis,               // Ptr to axis token (may be NULL)
                                        lptkFunc);              // Ptr to function token
RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFnMonUpTackJot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonUpTackJotImm_EM_YY
//
//  Monadic UpTackJot ("execute") on an immediate value
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonUpTackJotImm_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonUpTackJotImm_EM_YY
    (APLLONGEST    aplLongest,      // Ptr to right arg value
     LPTOKEN       lptkAxis,        // Ptr to axis token (may be NULL)
     LPTOKEN       lptkFunc)        // Ptr to function token

{
    LPAPLCHAR lpwszCompLine;        // Ptr to the line to execute

    // Allocate space for the immediate value
    lpwszCompLine =
      MyVirtualAlloc (NULL,         // Any address (FIXED SIZE)
                      (1 + 1) * sizeof (WCHAR), // "+ 1" for the terminating zero
                      MEM_COMMIT | MEM_TOP_DOWN,
                      PAGE_READWRITE);
    if (!lpwszCompLine)
        goto WSFULL_EXIT;

    // Save the char in the line
    lpwszCompLine[0] = (APLCHAR) aplLongest;
    lpwszCompLine[1] = WC_EOS;

    return
      PrimFnMonUpTackJotCommon_EM_YY (lpwszCompLine,    // Ptr to text of line to execute
                                      1,                // Length of the line to execute
                                      TRUE,             // TRUE iff we should free lpwszCompLine
                                      TRUE,             // TRUE iff we should return a NoValue YYRes
                                      TRUE,             // TRUE iff we should act on errors
                                      NULL,             // Ptr to return EXITTYPE_xxx (may be NULL)
                                      lptkFunc);        // Ptr to function token
WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    return NULL;                // Mark as failed
} // End PrimFnMonUpTackJotImm_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonUpTackGlb_EM_YY
//
//  Monadic UpTackJot ("execute") on a global memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonUpTackJotGlb_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonUpTackJotGlb_EM_YY
    (HGLOBAL hGlbRht,               // Handle to right arg
     LPTOKEN lptkAxis,              // Ptr to axis token (may be NULL)
     LPTOKEN lptkFunc)              // Ptr to function token

{
    LPAPLCHAR lpwszCompLine;        // Ptr to the line to execute
    LPAPLCHAR lpMemRht;             // Ptr to right arg global memory
    APLNELM   aplNELMRht;           // Right arg NELM
    APLRANK   aplRankRht;           // Right arg rank

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRht)
    // Get the NELM and Rank
    aplNELMRht = lpHeader->NELM;
    aplRankRht = lpHeader->Rank;
#undef  lpHeader

    // Skip over the header and dimension
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    if (aplNELMRht NE (APLU3264) aplNELMRht)
        goto WSFULL_EXIT;

    // Allocate space for the global value
    lpwszCompLine =
      MyVirtualAlloc (NULL,         // Any address (FIXED SIZE)
                      (DWORD) (aplNELMRht + 1) * sizeof (APLCHAR),   // "+ 1" for the terminating zero
                      MEM_COMMIT | MEM_TOP_DOWN,
                      PAGE_READWRITE);
    if (!lpwszCompLine)
        goto WSFULL_EXIT;

    // Check for overflow
    if (aplNELMRht NE (APLU3264) aplNELMRht)
        goto WSFULL_EXIT;

    // Copy the chars into the line
    CopyMemoryW (lpwszCompLine, lpMemRht, (APLU3264) aplNELMRht);
    lpwszCompLine[aplNELMRht] = WC_EOS;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRht); lpMemRht = NULL;

    return
      PrimFnMonUpTackJotCommon_EM_YY (lpwszCompLine,    // Ptr to text of line to execute
                                      aplNELMRht,       // Length of the line to execute
                                      TRUE,             // TRUE iff we should free lpwszCompLine
                                      TRUE,             // TRUE iff we should return a NoValue YYRes
                                      TRUE,             // TRUE iff we should act on errors
                                      NULL,             // Ptr to return EXITTYPE_xxx (may be NULL)
                                      lptkFunc);        // Ptr to function token
WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    return NULL;                // Mark as failed
} // End PrimFnMonUpTackJotGlb_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonUpTackJotCommon_EM_YY
//
//  Common subroutine to the Imm and Glb cases
//***************************************************************************

LPPL_YYSTYPE PrimFnMonUpTackJotCommon_EM_YY
    (LPAPLCHAR   lpwszCompLine,     // Ptr to text of line to execute
     APLNELM     aplNELMComp,       // Length of the line to execute
     UBOOL       bFreeCompLine,     // TRUE iff we should free lpwszCompLine
     UBOOL       bMakeNoValue,      // TRUE iff we should return a NoValue YYRes
     UBOOL       bActOnErrors,      // TRUE iff we should act on errors
     LPEXITTYPES lpExitType,        // Ptr to return EXITTYPE_xxx (may be NULL)
     LPTOKEN     lptkFunc)          // Ptr to function token

{
    LPPL_YYSTYPE   lpYYRes;         // Ptr to the result
    LPPERTABDATA   lpMemPTD;        // Ptr to PerTabData global memory
    HWND           hWndEC;          // Edit Ctrl window handle
    EXIT_TYPES     exitType;        // Exit type from CSPLParse

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get hWndEC for the Session Manager from the current thread
    hWndEC = GetThreadSMEC ();

    // Call common function which calls ParseCtrlStruc & ParseLine
    exitType =
      PrimFnMonUpTackJotCSPLParse (hWndEC,          // Edit Ctrl window handle
                                   lpMemPTD,        // Ptr to PerTabData global memory
                                   lpwszCompLine,   // Ptr to text of line to execute
                                   aplNELMComp,     // Length of the line to execute
                                   bActOnErrors,    // TRUE iff we should act on errors
                                   FALSE,           // TRUE iff we're to skip the depth check
                                   lptkFunc);       // Ptr to function token
    // Split cases based upon the exit type
    switch (exitType)
    {
        case EXITTYPE_DISPLAY:
        case EXITTYPE_NODISPLAY:
            if (lpMemPTD->YYResExec.YYInuse)
            {
                // Allocate a new YYRes
                lpYYRes = YYAlloc ();

                // Copy the result
                *lpYYRes = lpMemPTD->YYResExec;
                lpYYRes->tkToken.tkFlags.NoDisplay |= (exitType EQ EXITTYPE_NODISPLAY);
                lpYYRes->tkToken.tkCharIndex = lptkFunc->tkCharIndex;
                ZeroMemory (&lpMemPTD->YYResExec, sizeof (lpMemPTD->YYResExec));

                break;
            } // End IF

            // Fall through to NoValue code

        case EXITTYPE_GOTO_ZILDE:
        case EXITTYPE_GOTO_LINE:
        case EXITTYPE_RESET_ONE:
        case EXITTYPE_RESET_ONE_INIT:
        case EXITTYPE_RESET_ALL:
        case EXITTYPE_NONE:
        case EXITTYPE_NOVALUE:
        case EXITTYPE_QUADERROR_INIT:
        case EXITTYPE_QUADERROR_EXEC:
            // If we should return a NoValue YYRes, ...
            if (bMakeNoValue)
            {
                // Make a PL_YYSTYPE NoValue entry
                lpYYRes = MakeNoValue_YY (lptkFunc);

                break;
            } // End IF

            // Fall through to error code

        case EXITTYPE_STOP:
        case EXITTYPE_ERROR:
        case EXITTYPE_STACK_FULL:
        case EXITTYPE_RETURNxLX:
            // Mark as in error
            lpYYRes = NULL;

            // Set the error caret location
            ErrorMessageSetToken (lptkFunc);

            break;

        defstop
            break;
    } // End SWITCH

    // Free the virtual memory for the complete line
    if (bFreeCompLine)
    {
        MyVirtualFree (lpwszCompLine, 0, MEM_RELEASE); lpwszCompLine = NULL;
    } // End IF

    // If the caller wants the EXITTYPE_xxx, ...
    if (lpExitType)
        *lpExitType = exitType;

    return lpYYRes;
} // PrimFnMonUpTackJotCommon_EM_YY


//***************************************************************************
//  $PrimFnMonUpTackJotCSPLParse
//
//  Monadic UpTackJot ("execute") CS & PL parser
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonUpTackJotCSPLParse"
#else
#define APPEND_NAME
#endif

EXIT_TYPES WINAPI PrimFnMonUpTackJotCSPLParse
    (HWND         hWndEC,               // Edit Ctrl window handle
     LPPERTABDATA lpMemPTD,             // Ptr to PerTabData global memory
     LPAPLCHAR    lpwszCompLine,        // Ptr to text of line to execute
     APLNELM      aplNELMComp,          // Length of the line to execute
     UBOOL        bActOnErrors,         // TRUE iff we should act on errors
     UBOOL        bNoDepthCheck,        // TRUE iff we're to skip the depth check
     LPTOKEN      lptkFunc)             // Ptr to function token

{
    HGLOBAL        hGlbTknHdr;          // Tokenized line header global memory handle
    HANDLE         hSigaphore = NULL;   // Semaphore handle to signal (NULL if none)
    EXIT_TYPES     exitType;            // Return code from ParseLine
    LPTOKEN        lptkCSBeg;           // Ptr to next token on the CS stack
    CSLOCALVARS    csLocalVars = {0};   // CS local vars
    LPTOKEN_HEADER lpMemTknHdr;         // Ptr to tokenized line header global memory

    // Save the ptr to the next token on the CS stack
    //   as our beginning
    lptkCSBeg = lpMemPTD->lptkCSNxt;

    // Tokenize, parse, and untokenize the line

    // Tokenize the line
    hGlbTknHdr =
      Tokenize_EM (lpwszCompLine,               // The line to tokenize (not necessarily zero-terminated)
                   aplNELMComp,                 // Length of the line to execute
                   hWndEC,                      // Window handle for Edit Ctrl (may be NULL if lpErrHandFn is NULL)
                   1,                           // Function line # (0 = header)
                  &ErrorMessageDirect,          // Ptr to error handling function (may be NULL)
                   NULL,                        // Ptr to common struc (may be NULL if unused)
                   FALSE);                      // TRUE iff we're tokenizing a Magic Function/Operator
    // If it's invalid, ...
    if (hGlbTknHdr EQ NULL)
    {
        exitType = EXITTYPE_ERROR;              // Mark as failed Tokenize_EM

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
        static WCHAR wszTemp[1024];

        // Format the error message
        wsprintfW (wszTemp,
                   L"%s -- Line %d statement %d",
                   csLocalVars.lpwszErrMsg,
                   csLocalVars.tkCSErr.tkData.Orig.c.uLineNum,
                   csLocalVars.tkCSErr.tkData.Orig.c.uStmtNum + 1);
        // Save the error message
        ErrorMessageIndirectToken (wszTemp, lptkFunc);

        exitType = EXITTYPE_ERROR;              // Mark as failed ParseCtrlStruc

        goto ERROR_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemTknHdr = MyGlobalLock (hGlbTknHdr);

    // Fill the SIS struc, execute the line via ParseLine, and cleanup
    exitType =
      PrimFnMonUpTackJotPLParse (lpMemPTD,          // Ptr to PerTabData global memory
                                 lpMemTknHdr,       // Ptr to tokenized line header global memory
                                 NULL,              // Text      ...
                                 lpwszCompLine,     // Ptr to text of line to execute
                                &hSigaphore,        // Ptr to semaphore handle to signal
                                 1,                 // Function line # (1 for execute or immexec)
                                 0,                 // Starting token # in the above function line
                                 bActOnErrors,      // TRUE iff we should act on errors
                                 FALSE,             // TRUE iff executing only one stmt
                                 bNoDepthCheck);    // TRUE iff we're to skip the depth check
    // Untokenize the temporary line and free its memory
    Untokenize (lpMemTknHdr);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbTknHdr); lpMemTknHdr = NULL;

    // We no longer need this storage
    MyGlobalFree (hGlbTknHdr); hGlbTknHdr = NULL;
ERROR_EXIT:
    // Restore the ptr to the next token on the CS stack
    lpMemPTD->lptkCSNxt = lptkCSBeg;

    // If there's a semaphore to signal, ...
    if (hSigaphore)
    {
        dprintfWL9 (L"~~Releasing semaphore:  %p (%S#%d)", hSigaphore, FNLN);

        MyReleaseSemaphore (hSigaphore, 1, NULL);

        // Release our time slice so the released thread can act
        Sleep (0);
    } // End IF

    return exitType;
} // End PrimFnMonUpTackJotCSPLParse
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonUpTackJotPLParse
//
//  Fill the SIS struc, execute the line via ParseLine, and cleanup
//***************************************************************************

EXIT_TYPES PrimFnMonUpTackJotPLParse
    (LPPERTABDATA   lpMemPTD,           // Ptr to PerTabData global memory
     LPTOKEN_HEADER lpMemTknHdr,        // Ptr to tokenized line header global memory
     HGLOBAL        hGlbTxtLine,        // Text of tokenized line global memory handle
     LPAPLCHAR      lpwszCompLine,      // Ptr to text of line to execute
     HANDLE        *lphSigaphore,       // Ptr to Semaphore handle to signal (NULL if none)
     UINT           uLineNum,           // Function line #
     UINT           uTknNum,            // Starting token # in the above function line
     UBOOL          bActOnErrors,       // TRUE iff we should act on errors
     UBOOL          bExec1Stmt,         // TRUE iff executing only one stmt
     UBOOL          bNoDepthCheck)      // TRUE iff we're to skip the depth check

{
    EXIT_TYPES    exitType;             // Return code from ParseLine
    LPPLLOCALVARS lpplLocalVars;        // Ptr to local plLocalVars

    // Fill in the SIS header for Execute
    FillSISNxt (lpMemPTD,               // Ptr to PerTabData global memory
                NULL,                   // Semaphore handle
                DFNTYPE_EXEC,           // DfnType
                FCNVALENCE_MON,         // FcnValence
                FALSE,                  // Suspended
                TRUE,                   // Restartable
                TRUE);                  // LinkIntoChain
    // Execute the line
    exitType =
      ParseLine (lpMemPTD->hWndSM,      // Session Manager window handle
                 lpMemTknHdr,           // Ptr to tokenized line header global memory
                 hGlbTxtLine,           // Text of tokenized line global memory handle
                 lpwszCompLine,         // Ptr to the complete line
                 lpMemPTD,              // Ptr to PerTabData global memory
                 uLineNum,              // Function line # (1 for execute or immexec)
                 uTknNum,               // Starting token # in the above function line
                 NULL,                  // User-defined function/operator global memory handle (NULL = execute/immexec)
                 bActOnErrors,          // TRUE iff errors are acted upon
                 bExec1Stmt,            // TRUE iff executing only one stmt
                 bNoDepthCheck);        // TRUE iff we're to skip the depth check
    // Get this thread's LocalVars ptr
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars); // Assert (lpplLocalVars NE NULL);

    if (lpplLocalVars)
        // Save the exit type in the plLocalVars
        lpplLocalVars->ExitType = exitType;

    // If there's no result, ...
    if (!lpMemPTD->YYResExec.YYInuse)
    // Split cases based upon the exit type
    switch (exitType)
    {
        case EXITTYPE_NODISPLAY:
            // Mark as no value
            exitType = EXITTYPE_NOVALUE;

            break;

        case EXITTYPE_NONE:
        case EXITTYPE_GOTO_ZILDE:
        case EXITTYPE_GOTO_LINE:
        case EXITTYPE_RESET_ONE:
        case EXITTYPE_RESET_ONE_INIT:
        case EXITTYPE_RESET_ALL:
        case EXITTYPE_QUADERROR_INIT:
        case EXITTYPE_QUADERROR_EXEC:
        case EXITTYPE_ERROR:
        case EXITTYPE_STACK_FULL:
        case EXITTYPE_STOP:
        case EXITTYPE_DISPLAY:
        case EXITTYPE_NOVALUE:
        case EXITTYPE_RETURNxLX:
            break;

        defstop
            break;
    } // End IF/SWITCH

    // Save the semaphore handle to signal after Unlocalize (may be NULL if none)
    if (lphSigaphore)
        *lphSigaphore = lpMemPTD->lpSISCur->hSigaphore;
    lpMemPTD->lpSISCur->hSigaphore = NULL;

    // Unlocalize the STEs on the innermost level
    //   and strip off one level
    UnlocalizeSTEs ();

    // If this hSigaphore is not for this level, pass it on up the line
    if (lphSigaphore)
        *lphSigaphore = PassSigaphore (lpMemPTD, *lphSigaphore);

    return exitType;
} // End PrimFnMonUpTackJotPLParse


//***************************************************************************
//  $PrimFnDydUpTackJot_EM_YY
//
//  Primitive function for dyadic UpTackJot (ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydUpTackJot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydUpTackJot_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End PrimFnDydUpTackJot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_utackjot.c
//***************************************************************************
