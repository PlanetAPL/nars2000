//***************************************************************************
//  NARS2000 -- Parser Grammar Functions for executable lines
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
//  $WaitForInput
//
//  Wait for either Quad or Quote-quad input
//***************************************************************************

LPPL_YYSTYPE WaitForInput
    (HWND    hWndSM,                // Window handle to Session Manager
     UBOOL   bQuoteQuad,            // TRUE iff Quote-Quad input (FALSE if Quad input)
     LPTOKEN lptkFunc)              // Ptr to function token

{
    HANDLE         hSemaphore;      // Semaphore handle
    LPPL_YYSTYPE   lpYYRes;         // Ptr to the result
    LPPERTABDATA   lpMemPTD;        // Ptr to PerTabData global memory
    UINT           uLinePos,        // Char position of start of line
                   uCharPos;        // Current char position
    HWND           hWndEC;          // Edit Ctrl window handle

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Create a semaphore
    hSemaphore =
      MyCreateSemaphoreW (NULL,         // No security attrs
                          0,            // Initial count (non-signalled)
                          64*1024,      // Maximum count
                          NULL);        // No name
    // Fill in the SIS header for Quote-Quad/Quad Input Mode
    FillSISNxt (lpMemPTD,               // Ptr to PerTabData global memory
                hSemaphore,             // Semaphore handle
                bQuoteQuad ? DFNTYPE_QQUAD: DFNTYPE_QUAD, // DfnType
                FCNVALENCE_NIL,         // FcnValence
                TRUE,                   // Suspended
                TRUE,                   // Restartable
                TRUE);                  // LinkIntoChain
    // Save a ptr to the function token
    lpMemPTD->lpSISCur->lptkFunc = lptkFunc;

    // Get the Edit Ctrl window handle
    (HANDLE_PTR) hWndEC = GetWindowLongPtrW (hWndSM, GWLSF_HWNDEC);

    // Get the char position of the caret
    uCharPos = GetCurCharPos (hWndEC);

    // Get the position of the start of the line
    uLinePos = (UINT) SendMessageW (hWndEC, EM_LINEINDEX, NEG1U, 0);

    // Save the Quote-Quad input prompt length
    lpMemPTD->lpSISCur->QQPromptLen = uCharPos - uLinePos;

    // Tell the Session Manager to display the appropriate prompt
    PostMessageW (hWndSM, MYWM_QUOTEQUAD, bQuoteQuad, 14);

    // Wait for the semaphore to trigger
    MyWaitForSemaphore (hSemaphore,         // Handle to wait for
                        INFINITE,           // Timeout value in milliseconds
                       L"WaitForInput");    // Ptr to caller identification
    // Close the semaphore handle as it is no longer needed
    MyCloseSemaphore (hSemaphore); hSemaphore = NULL;

    // Unlocalize the STEs on the innermost level
    //   and strip off one level
    UnlocalizeSTEs (NULL);

    // If we're resetting, ...
    if (lpMemPTD->lpSISCur
     && lpMemPTD->lpSISCur->ResetFlag NE RESETFLAG_NONE)
        lpYYRes = NULL;
    else
    // If there's no result from the expression, ...
    if (lpMemPTD->YYResExec.tkToken.tkFlags.TknType EQ 0)
        // Make a PL_YYSTYPE NoValue entry
        lpYYRes = MakeNoValue_YY (lptkFunc);
    else
    {
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Copy the result
        *lpYYRes = lpMemPTD->YYResExec;
    } // End IF/ELSE

    // We no longer need these values
    ZeroMemory (&lpMemPTD->YYResExec, sizeof (lpMemPTD->YYResExec));

    return lpYYRes;
} // End WaitForInput


//***************************************************************************
//  $ArrExprCheckCaller
//
//  Tuck away the result from Execute/Quad.
//***************************************************************************

void ArrExprCheckCaller
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to local vars
     LPSIS_HEADER  lpSISCur,            // Ptr to current SIS header
     LPPL_YYSTYPE  lpYYArg,             // Ptr to ArrExpr
     UBOOL         bNoDisplay)          // TRUE iff value is not to be displayed

{
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // Mark as not in error
    lpplLocalVars->bRet = TRUE;

    // Get ptr to PerTabData global memory
    lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // If it's NoValue, ...
    if (IsTokenNoValue (&lpYYArg->tkToken))
        // Copy the result
        lpYYRes = lpYYArg;
    else
    // If it's []DM, ...
    if (IzitQuadDM (&lpYYArg->tkToken))
    {
        // This DEBUG stmt probably never is triggered because
        //    pl_yylex converts all unassigned named vars to temps
        //    hence IzitQuadDM always returns FALSE.
#ifdef DEBUG
        DbgStop ();             // ***Probably never executed***
#endif
        // Alloc a new YYRes
        lpYYRes = YYAlloc ();

        // Copy the token
        lpYYRes->tkToken = lpYYArg->tkToken;

        // Change it to a unnamed var
        lpYYRes->tkToken.tkFlags.TknType  = TKT_VARARRAY;
        lpYYRes->tkToken.tkData.tkGlbData = lpYYRes->tkToken.tkData.tkSym->stData.stGlbData;

        // Increment the refcnt
        DbgIncrRefCntTkn (&lpYYRes->tkToken);   // EXAMPLE:  ***Probably never executed***
    } else
        // Copy the function array incrementing the RefCnt
        lpYYRes = CopyPL_YYSTYPE_EM_YY (lpYYArg, FALSE);

    // Set/clear the NoDisplay flag
    lpYYRes->tkToken.tkFlags.NoDisplay = bNoDisplay;

    // If the Execute/Quad result is present,
    //   and the token is not named or it has a value, ...
    if (lpMemPTD->YYResExec.tkToken.tkFlags.TknType NE TKT_UNUSED
     && (!IsTknNamed (&lpMemPTD->YYResExec.tkToken)
      || !IsSymNoValue (lpMemPTD->YYResExec.tkToken.tkData.tkSym)))
    {
        HGLOBAL hGlbDfnHdr;

        // If we're parsing an AFO, ...
        if (hGlbDfnHdr = SISAfo (lpMemPTD))
            lpplLocalVars->bStopExec =
            lpplLocalVars->bRet =
              AfoDisplay_EM (&lpMemPTD->YYResExec.tkToken, bNoDisplay, lpplLocalVars, hGlbDfnHdr);
        else
            lpplLocalVars->bRet =
              ArrayDisplay_EM (&lpMemPTD->YYResExec.tkToken, TRUE, &lpplLocalVars->bCtrlBreak);
    } // End IF

    // Save the Execute/Quad result
    //   unless the current line starts with a "sink"
    //   or there's a error control parent active
    if (lpplLocalVars->lptkEOS EQ NULL
     || lpplLocalVars->lptkEOS[1].tkFlags.TknType NE TKT_ASSIGN
     || lpMemPTD->lpSISCur->lpSISErrCtrl NE NULL)
        lpMemPTD->YYResExec = *lpYYRes;

    // If it's not NoValue,
    if (!IsTokenNoValue (&lpYYArg->tkToken))
    {
        // Free the YYRes (but not the storage)
        YYFree (lpYYRes); lpYYRes = NULL;
    } // End IF
} // End ArrExprCheckCaller


/// //***************************************************************************
/// //  $CheckSelSpec_EM
/// //
/// //  Check on Selective Specification of the first (and only) name
/// //***************************************************************************
///
/// #ifdef DEBUG
/// #define APPEND_NAME     L" -- CheckSelSpec_EM"
/// #else
/// #define APPEND_NAME
/// #endif
///
/// UBOOL CheckSelSpec_EM
///     (LPPLLOCALVARS lpplLocalVars,       // Ptr to local vars
///      LPTOKEN       lptkNameVars)        // Ptr to NameVars token
///
/// {
///     HGLOBAL hGlbNameVars;               // NameVars global memory handle
///     LPTOKEN lpMemNameVars;              // Ptr to NameVars global memory
///     UBOOL   bRet;                       // TRUE iff the result is valid
///     UBOOL   bSelSpec = FALSE;           // TRUE iff Selective Specification is in error ***DEBUG***
///     UINT    uCharIndex;                 // Char index
///
///     // If it's Selective Specification, and
///     //   we have yet to see the first name, ...
///     if (lpplLocalVars->bSelSpec && lpplLocalVars->bIniSpec)
///     {
///         // Clear the flag
///         lpplLocalVars->bIniSpec = FALSE;
///
///         // Get the global memory handle
///         hGlbNameVars = lptkNameVars->tkData.tkGlbData;
///
///         Assert (IsGlbTypeNamDir_PTB (hGlbNameVars));
///
///         // Get the global memory handle
///         hGlbNameVars = lptkNameVars->tkData.tkGlbData;
///
///         // Lock the memory to get a ptr to it
///         lpMemNameVars = MyGlobalLockVnm (hGlbNameVars);
///
///         // Ensure there is only one name
///         bRet = (((LPVARNAMED_HEADER) lpMemNameVars)->NELM EQ 1);
///         if (bRet)
///         {
///             LPPL_YYSTYPE lpYYRes,       // Ptr to result
///                          lpYYRes2;      // ...
///
///             // Ensure the SelectSpec is properly marked
///             bRet = IsSelectSpec (lptkNameVars);
///             bSelSpec = !bRet;
///             if (bRet)
///             {
///                 // Skip over the header to the data
///                 lpMemNameVars = VarNamedBaseToData (lpMemNameVars);
///
///                 // Save the token
///                 lpplLocalVars->tkSelSpec = *lpMemNameVars;
///
///                 // Change the one (and only) token to an APA
///                 //   which maps the indices of the array
///                 lpYYRes2 =
///                   PrimFnMonRho_EM_YY (&lpplLocalVars->tkSelSpec,
///                                       &lpplLocalVars->tkSelSpec,
///                                        NULL);
///                 if (lpYYRes2)
///                 {
///                     lpYYRes =
///                       PrimFnMonIotaVector_EM_YY (&lpplLocalVars->tkSelSpec, // Ptr to function token
///                                                  &lpYYRes2->tkToken,        // Ptr to right arg token
///                                                   NULL);                    // Ptr to axis token (may be NULL)
///                     FreeResult (lpYYRes2); YYFree (lpYYRes2); lpYYRes2 = NULL;
///
///                     if (lpYYRes)
///                     {
///                         LPVARARRAY_HEADER lpMemData;
///
///                         // Save the char index
///                         uCharIndex = lptkNameVars->tkCharIndex;
///
///                         // Lock the memory to get a ptr to it
///                         lpMemData = MyGlobalLockVar (lpYYRes->tkToken.tkData.tkGlbData);
///
///                         // Mark as a Selective Specification array
///                         lpMemData->bSelSpec = TRUE;
///
///                         // We no longer need this ptr
///                         MyGlobalUnlock (lpYYRes->tkToken.tkData.tkGlbData); lpMemData = NULL;
///
///                         // Fill in the result token
///                         ZeroMemory (lptkNameVars, sizeof (lptkNameVars[0]));
///                         lptkNameVars->tkFlags.TknType   = TKT_VARARRAY;
/// ////////////////////////lptkNameVars->tkFlags.ImmType   = IMMTYPE_ERROR;    // Already zero from ZeroMemory
/// ////////////////////////lptkNameVars->tkFlags.NoDisplay = FALSE;            // Already zero from ZeroMemory
///                         lptkNameVars->tkData.tkGlbData  = lpYYRes->tkToken.tkData.tkGlbData;
///                         lptkNameVars->tkCharIndex       = uCharIndex;
///
///                         YYFree (lpYYRes); lpYYRes = NULL;
///                     } else
///                         // Mark as unsuccessful
///                         bRet = FALSE;
///                 } else
///                     // Mark as unsuccessful
///                     bRet = FALSE;
///             } // End IF
///         } // End IF
///
///         if (!bRet)
///         {
///             if (!(CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR))
///                 ErrorMessageIndirectToken (bSelSpec ? L"SelSpec ERROR" APPEND_NAME          // ***DEBUG***
///                                                     : ERRMSG_SYNTAX_ERROR APPEND_NAME,
///                                            lptkNameVars);
///             lpplLocalVars->ExitType = EXITTYPE_ERROR;
///         } // End IF
///
///         // We no longer need this ptr
///         MyGlobalUnlock (hGlbNameVars); lpMemNameVars = NULL;
///
///         return bRet;
///     } else
///         return TRUE;
///
///     return FALSE;
/// } // End CheckSelSpec_EM
/// #undef  APPEND_NAME
///
///
/// //***************************************************************************
/// //  $IsSelectSpec
/// //
/// //  Determine whether or not the arg is a Select Specification array
/// //***************************************************************************
///
/// UBOOL IsSelectSpec
///     (LPTOKEN lptkArg)               // Ptr to arg token
///
/// {
///     UBOOL bRet = FALSE;             // The result
///
///     // If the arg is not immediate, ...
///     if (lptkArg->tkFlags.TknType EQ TKT_VARARRAY)
///     {
///         HGLOBAL           hGlbArg;
///         LPVARARRAY_HEADER lpMemHdr;
///
///         // Get the global memory handle
///         hGlbArg = lptkArg->tkData.tkGlbData;
///
///         // Lock the memory to get a ptr to it
///         lpMemHdr = MyGlobalLockVar (hGlbArg);
///
///         // Get the Selective Specification array bit
///         bRet = lpMemHdr->bSelSpec;
///
///         // We no longer need this ptr
///         MyGlobalUnlock (hGlbArg); lpMemHdr = NULL;
///     } // End IF
///
///     return bRet;
/// } // End IsSelectSpec


//***************************************************************************
//  $GetEOSIndex
//
//  Return the index of the EOS stmt for a char index
//***************************************************************************

UINT GetEOSIndex
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to local vars
     int           tkCharIndex)         // Character index of array to display/free

{
    UINT uCnt,                          // Loop counter
         uLen;                          // # tokens in the stmt

    // Loop through the tokens looking for this stmt's char index
    for (uCnt = 0; uCnt < lpplLocalVars->uTokenCnt; uCnt += uLen)
    {
        Assert (lpplLocalVars->lptkStart[uCnt].tkFlags.TknType EQ TKT_EOL
             || lpplLocalVars->lptkStart[uCnt].tkFlags.TknType EQ TKT_EOS);

        // Get the # tokens in this stmt
        uLen = lpplLocalVars->lptkStart[uCnt].tkData.tkIndex;

        Assert (lpplLocalVars->lptkStart[uCnt + uLen - 1].tkFlags.TknType EQ TKT_SOS);

        if (lpplLocalVars->lptkStart[uCnt].tkCharIndex <= tkCharIndex
         &&                                               tkCharIndex < lpplLocalVars->lptkStart[uCnt + uLen - 1].tkCharIndex)
            break;
    } // End FOR

    // If we're still within the line, ...
    if (uCnt < lpplLocalVars->uTokenCnt)
        return uCnt;
    else
        return NEG1U;
} // End GetEOSIndex


//***************************************************************************
//  $IsLastStmt
//
//  Return TRUE iff the current stmt is the last one on the line
//  We need this to determine whether or not to print or return a result
//    from within an Execute or Quad stmt.
//***************************************************************************

UBOOL IsLastStmt
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to local vars
     int           tkCharIndex)         // Character index of array to display/free

{
    UINT uIndex;                        // Index of the corresponding EOS stmt

    // Get the index of the EOS stmt
    uIndex = GetEOSIndex (lpplLocalVars, tkCharIndex);

    // If we're still within the line, ...
    if (uIndex NE NEG1U)
        return (lpplLocalVars->uTokenCnt EQ (uIndex + lpplLocalVars->lptkStart[uIndex].tkData.tkIndex));
    else
        return TRUE;
} // End IsLastStmt


//***************************************************************************
//  $SrchSISForDfn
//
//  Search up the SIS chain looking for a UDFO
//***************************************************************************

LPSIS_HEADER SrchSISForDfn
    (LPPERTABDATA lpMemPTD)         // Ptr to PerTabData global memory

{
    LPSIS_HEADER lpSISCur;          // Ptr to current SIS layer

    // Search up the SIS chain to see what this is
    for (lpSISCur = lpMemPTD->lpSISCur;
         lpSISCur;
         lpSISCur = lpSISCur->lpSISPrv)
    // Split case based upon the function type
    switch (lpSISCur->DfnType)
    {
        case DFNTYPE_FCN:
        case DFNTYPE_OP1:
        case DFNTYPE_OP2:
            return lpSISCur;

        case DFNTYPE_IMM:
        case DFNTYPE_EXEC:
        case DFNTYPE_QUAD:
        case DFNTYPE_QQUAD:
        case DFNTYPE_ERRCTRL:
            break;

        defstop
            break;
    } // End FOR/SWITCH

    return NULL;
} // End SrchSISForDfn


//***************************************************************************
//  End of File: pl_proc.c
//***************************************************************************
