//***************************************************************************
//  NARS2000 -- Anonymous Function/Operator Routines
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

// The following struc should have as many rows as AFOROW_LENGTH
//   and as many columns as AFOCOL_LENGTH.
AFOACTSTR fsaActTableAFO [][AFOCOL_LENGTH]
#ifndef PROTO
 =
{   // AFOROW_SOS       Start of stmt ('')
  {{AFOROW_EXIT      , NULL        , NULL        },   // Digit
   {AFOROW_ALPHA     , afoNameIni  , afoAlpha    },   // Alphabetic including Underbar, Delta & variants
   {AFOROW_EXIT      , NULL        , NULL        },   // Overbar
   {AFOROW_EXIT      , NULL        , NULL        },   // Assignment symbol
   {AFOROW_EXIT      , NULL        , NULL        },   // Left brace
   {AFOROW_EXIT      , NULL        , NULL        },   // Right ...
   {AFOROW_SOS       , NULL        , NULL        },   // White space (' ' or '\t')
   {AFOROW_EXIT      , NULL        , NULL        },   // Single quote symbol
   {AFOROW_EXIT      , NULL        , NULL        },   // Double ...
   {AFOROW_EXIT      , NULL        , NULL        },   // Comment ...
   {AFOROW_EXIT      , NULL        , NULL        },   // End-of-line
   {AFOROW_EXIT      , NULL        , NULL        },   // Unknown symbols
   {AFOROW_SOS       , NULL        , NULL        },   // CR
  },
    // AFOROW_ALPHA
  {{AFOROW_ALPHA     , NULL        , NULL        },   // Digit
   {AFOROW_ALPHA     , NULL        , NULL        },   // Alphabetic including Underbar, Delta & variants
   {AFOROW_ALPHA     , NULL        , NULL        },   // Overbar
   {AFOROW_ASSIGN    , afoNameEnd  , afoAssign   },   // Assignment arrow
   {AFOROW_EXIT      , NULL        , NULL        },   // Left brace
   {AFOROW_EXIT      , NULL        , NULL        },   // Right ...
   {AFOROW_SPACE     , afoNameEnd  , NULL        },   // White space (' ' or '\t')
   {AFOROW_EXIT      , NULL        , NULL        },   // Single quote symbol
   {AFOROW_EXIT      , NULL        , NULL        },   // Double ...
   {AFOROW_EXIT      , NULL        , NULL        },   // Comment ...
   {AFOROW_EXIT      , NULL        , NULL        },   // End-of-line
   {AFOROW_EXIT      , NULL        , NULL        },   // Unknown symbols
   {AFOROW_SPACE     , afoNameEnd  , NULL        },   // CR
  },
    // AFOROW_SPACE
  {{AFOROW_EXIT      , NULL        , NULL        },   // Digit
   {AFOROW_EXIT      , NULL        , NULL        },   // Alphabetic including Underbar, Delta & variants
   {AFOROW_EXIT      , NULL        , NULL        },   // Overbar
   {AFOROW_ASSIGN    , afoAssign   , NULL        },   // Assignment arrow
   {AFOROW_EXIT      , NULL        , NULL        },   // Left brace
   {AFOROW_EXIT      , NULL        , NULL        },   // Right ...
   {AFOROW_SPACE     , NULL        , NULL        },   // White space (' ' or '\t')
   {AFOROW_EXIT      , NULL        , NULL        },   // Single quote symbol
   {AFOROW_EXIT      , NULL        , NULL        },   // Double ...
   {AFOROW_EXIT      , NULL        , NULL        },   // Comment ...
   {AFOROW_EXIT      , NULL        , NULL        },   // End-of-line
   {AFOROW_EXIT      , NULL        , NULL        },   // Unknown symbols
   {AFOROW_SPACE     , NULL        , NULL        },   // CR
  },
    // AFOROW_ASSIGN
  {{AFOROW_EXIT      , NULL        , NULL        },   // Digit
   {AFOROW_EXIT      , NULL        , NULL        },   // Alphabetic including Underbar, Delta & variants
   {AFOROW_EXIT      , NULL        , NULL        },   // Overbar
   {AFOROW_EXIT      , NULL        , NULL        },   // Assignment arrow
   {AFOROW_LBR_INIT  , afoLbrInit  , NULL        },   // Left brace
   {AFOROW_EXIT      , NULL        , NULL        },   // Right ...
   {AFOROW_ASSIGN    , NULL        , NULL        },   // White space (' ' or '\t')
   {AFOROW_EXIT      , NULL        , NULL        },   // Single quote symbol
   {AFOROW_EXIT      , NULL        , NULL        },   // Double ...
   {AFOROW_EXIT      , NULL        , NULL        },   // Comment ...
   {AFOROW_EXIT      , NULL        , NULL        },   // End-of-line
   {AFOROW_EXIT      , NULL        , NULL        },   // Unknown symbols
   {AFOROW_ASSIGN    , NULL        , NULL        },   // CR
  },
    // AFOROW_LBR_INIT
  {{AFOROW_LBR_INIT  , NULL        , NULL        },   // Digit
   {AFOROW_LBR_INIT  , NULL        , NULL        },   // Alphabetic including Underbar, Delta & variants
   {AFOROW_LBR_INIT  , NULL        , NULL        },   // Overbar
   {AFOROW_LBR_INIT  , NULL        , NULL        },   // Assignment arrow
   {AFOROW_LBR_INIT  , afoLbrInit  , NULL        },   // Left brace
   {AFOROW_LBR_EXIT  , afoLbrExit  , NULL        },   // Right ...
   {AFOROW_LBR_INIT  , NULL        , NULL        },   // White space (' ' or '\t')
   {AFOROW_LBR_Q1    , NULL        , NULL        },   // Single quote symbol
   {AFOROW_LBR_Q2    , NULL        , NULL        },   // Double ...
   {AFOROW_LBR_INIT  , afoLbrLamp  , NULL        },   // Comment ...
   {AFOROW_EXIT      , NULL        , NULL        },   // End-of-line
   {AFOROW_LBR_INIT  , NULL        , NULL        },   // Unknown symbols
   {AFOROW_LBR_INIT  , afoLbrCR    , NULL        },   // CR
  },
    // AFOROW_LBR_EXIT
  {{AFOROW_LBR_INIT  , NULL        , NULL        },   // Digit
   {AFOROW_LBR_INIT  , NULL        , NULL        },   // Alphabetic including Underbar, Delta & variants
   {AFOROW_LBR_INIT  , NULL        , NULL        },   // Overbar
   {AFOROW_LBR_INIT  , NULL        , NULL        },   // Assignment arrow
   {AFOROW_LBR_INIT  , afoLbrInit  , NULL        },   // Left brace
   {AFOROW_LBR_EXIT  , afoLbrExit  , NULL        },   // Right ...
   {AFOROW_LBR_INIT  , NULL        , NULL        },   // White space (' ' or '\t')
   {AFOROW_LBR_Q1    , NULL        , NULL        },   // Single quote symbol
   {AFOROW_LBR_Q2    , NULL        , NULL        },   // Double ...
   {AFOROW_LBR_INIT  , afoLbrLamp  , NULL        },   // Comment ...
   {AFOROW_EXIT      , NULL        , NULL        },   // End-of-line
   {AFOROW_LBR_INIT  , NULL        , NULL        },   // Unknown symbols
   {AFOROW_LBR_INIT  , afoLbrCR    , NULL        },   // CR
  },
    // AFOROW_LBR_Q1
  {{AFOROW_LBR_Q1    , NULL        , NULL        },   // Digit
   {AFOROW_LBR_Q1    , NULL        , NULL        },   // Alphabetic including Underbar, Delta & variants
   {AFOROW_LBR_Q1    , NULL        , NULL        },   // Overbar
   {AFOROW_LBR_Q1    , NULL        , NULL        },   // Assignment arrow
   {AFOROW_LBR_Q1    , NULL        , NULL        },   // Left brace
   {AFOROW_LBR_Q1    , NULL        , NULL        },   // Right ...
   {AFOROW_LBR_Q1    , NULL        , NULL        },   // White space (' ' or '\t')
   {AFOROW_LBR_INIT  , NULL        , NULL        },   // Single quote symbol
   {AFOROW_LBR_Q1    , NULL        , NULL        },   // Double ...
   {AFOROW_LBR_Q1    , NULL        , NULL        },   // Comment ...
   {AFOROW_EXIT      , NULL        , NULL        },   // End-of-line
   {AFOROW_LBR_Q1    , NULL        , NULL        },   // Unknown symbols
   {AFOROW_LBR_Q1    , afoLbrCR    , NULL        },   // CR
  },
    // AFOROW_LBR_Q2
  {{AFOROW_LBR_Q2    , NULL        , NULL        },   // Digit
   {AFOROW_LBR_Q2    , NULL        , NULL        },   // Alphabetic including Underbar, Delta & variants
   {AFOROW_LBR_Q2    , NULL        , NULL        },   // Overbar
   {AFOROW_LBR_Q2    , NULL        , NULL        },   // Assignment arrow
   {AFOROW_LBR_Q2    , NULL        , NULL        },   // Left brace
   {AFOROW_LBR_Q2    , NULL        , NULL        },   // Right ...
   {AFOROW_LBR_Q2    , NULL        , NULL        },   // White space (' ' or '\t')
   {AFOROW_LBR_Q2    , NULL        , NULL        },   // Single quote symbol
   {AFOROW_LBR_INIT  , NULL        , NULL        },   // Double ...
   {AFOROW_LBR_Q2    , NULL        , NULL        },   // Comment ...
   {AFOROW_EXIT      , NULL        , NULL        },   // End-of-line
   {AFOROW_LBR_Q2    , NULL        , NULL        },   // Unknown symbols
   {AFOROW_LBR_Q2    , afoLbrCR    , NULL        },   // CR
  },
}
#endif
;


//***************************************************************************
//  $EraseAfoResult
//
//  Erase the $AFORESULT
//***************************************************************************

void EraseAfoResult
    (LPDFN_HEADER lpMemDfnHdr)          // Ptr to user-defined function/operator header global memory

{
    LPSYMENTRY     lpSymEntry;
    LPTOKEN_HEADER lpMemTknHdr;
    LPTOKEN        lpMemTknData;

    // Lock the memory to get a ptr to it
    lpMemTknHdr = MyGlobalLockTkn (lpMemDfnHdr->hGlbTknHdr);

    // Skip over the TOKEN_HEADER
    lpMemTknData = TokenBaseToStart (lpMemTknHdr);

    // Get an LPSYMENTRY for $AFORESULT
    lpSymEntry = lpMemTknData[1].tkData.tkSym;

    // If found, attempt to expunge the name
    if (lpSymEntry NE NULL)
        // Erase the current value of $AFORESULT
        ExpungeName (lpSymEntry, FALSE);

    // We no longer need this ptr
    MyGlobalUnlock (lpMemDfnHdr->hGlbTknHdr); lpMemTknHdr = NULL;
} // End EraseAfoResult


//***************************************************************************
//  $AfoReturn
//
//  Handle return after guard from an AFO
//***************************************************************************

void AfoReturn
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to LocalVars
     LPPL_YYSTYPE  lpYYRhtArg)          // Ptr to token

{
    LPDFN_HEADER lpMemDfnHdr;           // Ptr to user-defined function/operator header global memory

    // Lock the memory to get a ptr to it
    lpMemDfnHdr = MyGlobalLockDfn (lpplLocalVars->hGlbDfnHdr);

    // If the token has No Value, ...
    if (IsTokenNoValue (&lpYYRhtArg->tkToken))
        // Erase the $AFORESULT
        EraseAfoResult (lpMemDfnHdr);
    else
    {
        // Transfer the NoDisplay flag to the AFO header
        lpMemDfnHdr->bAfoNoDispRes = lpYYRhtArg->tkToken.tkFlags.NoDisplay;

        // Assign the value to $AFORESULT
        AfoDisplay_EM (&lpYYRhtArg->tkToken, FALSE, lpplLocalVars, lpplLocalVars->hGlbDfnHdr);
    } // End IF/ELSE

    // We no longer need this ptr
    MyGlobalUnlock (lpplLocalVars->hGlbDfnHdr); lpMemDfnHdr = NULL;
} // End AfoReturn


//***************************************************************************
//  $AfoGuard
//
//  Test an AFO guard stmt
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- AfoGuard"
#else
#define APPEND_NAME
#endif

UBOOL AfoGuard
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to LocalVars
     LPTOKEN       lptkRhtArg)          // Ptr to right argument token
{
    APLSTYPE   aplTypeRht;          // Right arg storage type
    APLNELM    aplNELMRht;          // Right arg NELM
    APLRANK    aplRankRht;          // Right arg rank
    UBOOL      bRet;                // TRUE iff the result is valid
    APLLONGEST aplLongestRht;       // Right arg value if immediate

    // Get the right arg (Type, NELM, Rank)
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RIGHT RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RANK_EXIT;
    // Check for LENGTH ERROR
    if (!IsSingleton (aplNELMRht))
        goto LENGTH_EXIT;
    // Check for DOMAIN ERROR
    if (!IsNumeric (aplTypeRht))
        goto DOMAIN_EXIT;

    // Get and validate the first item in a token
    aplLongestRht = ValidateFirstItemToken (aplTypeRht, lptkRhtArg, &bRet);

    // Check for DOMAIN ERROR
    if (!bRet
     || !IsBooleanValue (aplLongestRht))
         goto DOMAIN_EXIT;

    // If the guard is FALSE, ...
    if (aplLongestRht EQ 0)
    {
        // If we're at the EOL, ...
        if (lpplLocalVars->lptkNext[-1].tkFlags.TknType EQ TKT_EOL)
            goto SYNTAX_EXIT;

        Assert (lpplLocalVars->lptkNext[-1].tkFlags.TknType EQ TKT_EOS);

        // Skip over the current stmt to the last token of the next stmt
        // When pl_yylex gets the next token it decrements this ptr first
        //   so that the next token it processes is an EOS/EOL which causes
        //   it to skip the next stmt.
        lpplLocalVars->lptkNext += lpplLocalVars->lptkNext[-1].tkData.tkIndex;

        // Save a ptr to the EOS/EOL token
        lpplLocalVars->lptkEOS = lpplLocalVars->lptkNext - 1;
    } // End IF

    goto NORMAL_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

ERROR_EXIT:
    // Mark as unsuccessful
    bRet = FALSE;
NORMAL_EXIT:
    return bRet;
} // End AfoGuard
#undef  APPEND_NAME


//***************************************************************************
//  $MakeAfo_EM_YY
//
//  Make an AFO
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeAfo_EM_YY"
#else
#define APPEND_NAME
#endif

void MakeAfo_EM_YY
    (LPPL_YYSTYPE  lpYYRht,             // Ptr to right brace token
     LPPLLOCALVARS lpplLocalVars)       // Ptr to LocalVars
{
    SF_FCNS      SF_Fcns = {0};         // Common struc for SaveFunctionCom
    LW_PARAMS    LW_Params = {0};       // Local  ...
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    HGLOBAL      hGlbDfnHdr;            // AFO Global memory handle

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Fill in common values
    SF_Fcns.bDisplayErr     = FALSE;                // Display Errors
    SF_Fcns.bAFO            = TRUE;                 // Parsing an AFO
    SF_Fcns.bMakeAFO        = TRUE;                 // Called from MakeAFO
////SF_Fcns.bSetAlpha       = FALSE;                // AFO assigns {alpha}
////SF_Fcns.bRefAlpha       = FALSE;                // AFO references {alpha}
////SF_Fcns.bRefOmega       = FALSE;                // AFO references {omega}
    SF_Fcns.bMFO            = lpplLocalVars->bMFO;  // TRUE iff this is an MFO
////SF_Fcns.bRet            =                       // Filled in by SaveFunctionCom
////SF_Fcns.uErrLine        =                       // ...
////SF_Fcns.lpSymName       =                       // ...
    SF_Fcns.lptkFunc        = NULL;                 // Ptr to function token ***FIXME*** -- Used on error
    SF_Fcns.SF_LineLen      = SF_LineLenAN;         // Ptr to line length function
    SF_Fcns.SF_ReadLine     = SF_ReadLineAN;        // Ptr to read line function
    SF_Fcns.SF_IsLineCont   = SF_IsLineContAN;      // Ptr to Is Line Continued function
    SF_Fcns.SF_NumPhyLines  = SF_NumPhyLinesAN;     // Ptr to get # physical lines function
    SF_Fcns.SF_NumLogLines  = SF_NumLogLinesAN;     // Ptr to get # logical  ...
    SF_Fcns.SF_CreationTime = SF_CreationTimeCom;   // Ptr to get function creation time
    SF_Fcns.SF_LastModTime  = SF_LastModTimeCom;    // Ptr to get function last modification time
    SF_Fcns.SF_UndoBuffer   = SF_UndoBufferCom;     // Ptr to get function Undo Buffer global memory handle
////SF_Fcns.hGlbDfnHdr      = NULL;                 // UDFO global memory handle        (Already zero from = {0})
////SF_Fcns.numLocalsSTE    = 0;                    // # locals                         (Already zero from = {0})
////SF_Fcns.lplpLocalSTEs   = NULL;                 // Ptr to save area for local STEs  (Already zero from = {0})
    SF_Fcns.LclParams       = &LW_Params;           // Ptr to local parameters
    SF_Fcns.sfTypes         = SFTYPES_AFO;          // Caller type

    // Fill in local values
    LW_Params.lpYYRht       = lpYYRht;              // Ptr to right brace token
    LW_Params.lpplLocalVars = lpplLocalVars;        // Ptr to Local Vars

    Assert (lpYYRht->lptkLftBrace[1].tkFlags.TknType EQ TKT_GLBDFN);

    // Get the AFO global memory handle (may be NULL if not already saved)
    hGlbDfnHdr = lpYYRht->lptkLftBrace[1].tkData.tkGlbData;

    // Call common routine unless we've already saved this AFO
    if (hGlbDfnHdr NE NULL
     || SaveFunctionCom (NULL, &SF_Fcns))
    {
        // ***FIXME*** -- Should we rebase tkCharIndexes now?

        // If we've not already saved this AFO, ...
        if (hGlbDfnHdr EQ NULL)
        {
            // Get the one created by <SaveFunctionCom>
            hGlbDfnHdr = MakePtrTypeGlb (SF_Fcns.hGlbDfnHdr);

            // Set the AFO global memory handle
            lpYYRht->lptkLftBrace[1].tkData.tkGlbData = hGlbDfnHdr;
        } else
            // Convert to a PTRTYPE_HGLOBAL
            hGlbDfnHdr = MakePtrTypeGlb (hGlbDfnHdr);

        goto NORMAL_EXIT;
    } else
    {
        // If the error line # is NEG1U, there is an error message, so return NULL.
        if (SF_Fcns.uErrLine EQ NEG1U)
            goto ERROR_EXIT;
        else
            goto SYNTAX_EXIT;
    } // End IF/ELSE

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lpYYRht->lptkLftBrace);
    goto ERROR_EXIT;

ERROR_EXIT:
    // Free the HshTab & SymTab
    FreeHshSymTabs (&SF_Fcns.htsDFN, FALSE);
NORMAL_EXIT:
    return;
} // End MakeAfo_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $AfoDisplay_EM
//
//  Handle display in an AFO
//***************************************************************************

UBOOL AfoDisplay_EM
    (LPTOKEN       lptkSrc,             // Ptr to source token
     UBOOL         bNoDisplay,          // TRUE iff value is not to be displayed
     LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     HGLOBAL       hGlbDfnHdr)          // AFO DfnHdr global memory handle

{
    LPDFN_HEADER   lpMemDfnHdr;
    LPTOKEN_HEADER lpMemTknHdr;
    LPTOKEN        lpMemTknData;
    UBOOL          bRet;

    // Lock the memory to get a ptr to it
    lpMemDfnHdr = MyGlobalLockDfn (hGlbDfnHdr);

    Assert (lpMemDfnHdr NE NULL);

    // Transfer the NoDisplay flag from the incoming token
    lpMemDfnHdr->bAfoNoDispRes = (lptkSrc->tkFlags.NoDisplay || bNoDisplay);

    // Lock the memory to get a ptr to it
    lpMemTknHdr = MyGlobalLockTkn (lpMemDfnHdr->hGlbTknHdr);

    // Skip over the TOKEN_HEADER
    lpMemTknData = TokenBaseToStart (lpMemTknHdr);

    // Assign the result to $AFORESULT
    // Note that <AssignName_EM> sets the <NoDisplay> flag in the source token
    bRet = AssignName_EM (&lpMemTknData[1],
                           lptkSrc);
    // Transfer the NoDisplay flag
    bNoDisplay                       |=
    lpMemTknData[1].tkFlags.NoDisplay = lpMemDfnHdr->bAfoNoDispRes;

    // We no longer need these ptrs
    MyGlobalUnlock (lpMemDfnHdr->hGlbTknHdr);   lpMemTknHdr = NULL;
    MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;

    // If we succeeded,
    //   and we're displaying, ...
    if (bRet
     && !bNoDisplay)
    {
        // Tell the parser to stop executing this line
        lpplLocalVars->bAfoValue =
        lpplLocalVars->bStopExec = TRUE;

        // and to display it
        lpplLocalVars->ExitType = EXITTYPE_DISPLAY;
    } // End IF

    return bRet;
} // End AfoDisplay_EM


//***************************************************************************
//  $SISAfo
//
//  Return TRUE iff we're at a non-EXEC AFO
//***************************************************************************

HGLOBAL SISAfo
    (LPPERTABDATA lpMemPTD)

{
    LPSIS_HEADER lpSISCur;          // Ptr to current SIS header

    // Get a ptr to the current SIS header
    lpSISCur = lpMemPTD->lpSISCur;

    // Peel back to the first non-Exec layer
    while (lpSISCur && lpSISCur->DfnType EQ DFNTYPE_EXEC)
       lpSISCur = lpSISCur->lpSISPrv;

    // If it's a FCN/OP1/OP2, ...
    if (lpSISCur NE NULL
     && (lpSISCur->DfnType EQ DFNTYPE_OP1
      || lpSISCur->DfnType EQ DFNTYPE_OP2
      || lpSISCur->DfnType EQ DFNTYPE_FCN)
     && lpSISCur->bAFO)
        return lpSISCur->hGlbDfnHdr;
    else
        return NULL;
} // End SISAfo


//***************************************************************************
//  $IsAfoName
//
//  Return TRUE iff the arg is a valid AFO special name
//***************************************************************************

UBOOL IsAfoName
    (LPAPLCHAR lpwName,     // Ptr to name (not zero-terminated)
     APLU3264  uLen)        // Length of name

{
    WCHAR wszTemp[3];

    // Check the length
    if (uLen > 2)
        return FALSE;

    // Copy the name to a temp and terminate it
    wszTemp[0]    = lpwName[0];
    wszTemp[1]    = lpwName[1];
    wszTemp[uLen] = WC_EOS;

    // Handle aliases of {alpha} and {omega}
    ConvertAfoAlias (&wszTemp[0]);
    ConvertAfoAlias (&wszTemp[1]);

    // If the name ends with a blank, ...
    if (IsWhiteW (wszTemp[1]))
        // Zap it
        wszTemp[1] = WC_EOS;

    return (lstrcmpW (wszTemp, WS_UTF16_ALPHA  ) EQ 0
         || lstrcmpW (wszTemp, WS_UTF16_DEL    ) EQ 0
         || lstrcmpW (wszTemp, WS_UTF16_OMEGA  ) EQ 0
         || lstrcmpW (wszTemp, WS_UTF16_LFTOPER) EQ 0
         || lstrcmpW (wszTemp, WS_UTF16_DELDEL ) EQ 0
         || lstrcmpW (wszTemp, WS_UTF16_RHTOPER) EQ 0);
} // End IsAfoName


//***************************************************************************
//  $ConvertAfoAlias
//
//  Handle aliases of {alpha} and {omega}
//***************************************************************************

void ConvertAfoAlias
    (LPAPLCHAR lpwChar)

{
    if (lpwChar[0] EQ UTF16_ALPHA2)
        lpwChar[0] = UTF16_ALPHA;
    else
    if (lpwChar[0] EQ UTF16_OMEGA2)
        lpwChar[0] = UTF16_OMEGA;
} // End ConvertAfoAlias


#ifdef DEBUG
//***************************************************************************
//  $InitFsaTabAFO
//
//  Ensure the AFO FSA table has been properly setup
//***************************************************************************

void InitFsaTabAFO
    (void)

{
    // Ensure we calculated the lengths properly
    Assert (sizeof (fsaActTableAFO) EQ (AFOCOL_LENGTH * sizeof (AFOACTSTR) * AFOROW_LENGTH));
} // End InitFsaTabAFO
#endif


//***************************************************************************
//  $AfoDetectResize
//
//  Resize the HGLOBAL for one more entry
//***************************************************************************

UBOOL AfoDetectResize
    (LPAFODETECT_STR lpafoDetectStr)

{
    // If there is no more room, ...
    if (lpafoDetectStr->uLineStrCnt EQ lpafoDetectStr->uLineStrNxt)
    {
        HGLOBAL hGlbLineStr;

        // Unlock the global memory handle so we may resize it
        GlobalUnlock (lpafoDetectStr->hGlbLineStr);

        // Reallocate the AFOLINE_STR
        //   moving the old data to the new location, and
        //   freeing the old global memory
        hGlbLineStr =
          GlobalReAlloc (lpafoDetectStr->hGlbLineStr,
                          (lpafoDetectStr->uLineStrCnt + AFOLINESTR_INCR) * sizeof (AFOLINE_STR),
                          GMEM_MOVEABLE);
        // Check for error
        if (hGlbLineStr EQ NULL)
            return FALSE;

        // Save (possibly new) handle
        lpafoDetectStr->hGlbLineStr  = hGlbLineStr;
        lpafoDetectStr->lpafoLineStr = GlobalLock (hGlbLineStr);

        return TRUE;
    } else
        return TRUE;
} // End AfoDetectResize


//***************************************************************************
//  $afoNameIni
//
//  We encountered the first char of the function name
//***************************************************************************

UBOOL afoNameIni
    (LPAFOLOCALVARS lpafoLocalVars)     // Ptr to AfoDetect local vars

{
    LPAFODETECT_STR lpafoDetectStr = lpafoLocalVars->lpafoDetectStr;

    // Save the line offset
    lpafoDetectStr->uLastLineOff = lpafoLocalVars->uChar;

    return TRUE;
} // End afoNameIni


//***************************************************************************
//  $afoNameEnd
//
//  We encountered the last char of the function name
//***************************************************************************

UBOOL afoNameEnd
    (LPAFOLOCALVARS lpafoLocalVars)     // Ptr to AfoDetect local vars

{
    LPAFODETECT_STR lpafoDetectStr = lpafoLocalVars->lpafoDetectStr;

    // Make room for one more entry
    if (!AfoDetectResize (lpafoDetectStr))
        goto WSFULL_EXIT;

    // Save the last line length and offset
    lpafoDetectStr->lpafoLineStr[lpafoDetectStr->uLineStrNxt].uLineOff = lpafoDetectStr->uLastLineOff;
    lpafoDetectStr->lpafoLineStr[lpafoDetectStr->uLineStrNxt].uLineLen = lpafoLocalVars->uChar - lpafoDetectStr->uLastLineOff;
    lpafoDetectStr->uLineStrNxt++;

    return TRUE;

WSFULL_EXIT:
    // Set new state (we're done)
    lpafoLocalVars->State = AFOROW_WSFULL;

    return FALSE;
} // End afoNameEnd


//***************************************************************************
//  $afoAlpha
//
//  We encountered an alphabetic char
//***************************************************************************

UBOOL afoAlpha
    (LPAFOLOCALVARS lpafoLocalVars)     // Ptr to AfoDetect local vars

{
    // Mark as encountered
    lpafoLocalVars->bLbrAlpha = TRUE;

    return TRUE;
} // End afoAlpha


//***************************************************************************
//  $afoAssign
//
//  We encountered an assignment symbol
//***************************************************************************

UBOOL afoAssign
    (LPAFOLOCALVARS lpafoLocalVars)     // Ptr to AfoDetect local vars

{
    // Mark as encountered
    lpafoLocalVars->bLbrAssign = TRUE;

    return TRUE;
} // End afoAssign


//***************************************************************************
//  $afoLbrInit
//
//  We encountered another left brace
//***************************************************************************

UBOOL afoLbrInit
    (LPAFOLOCALVARS lpafoLocalVars)     // Ptr to AfoDetect local vars

{
    LPAFODETECT_STR lpafoDetectStr = lpafoLocalVars->lpafoDetectStr;

    // If it's the first time, ...
    if (lpafoLocalVars->lbrCount EQ 0)
        // Save as the start of the first line (skipping over the left brace)
        lpafoDetectStr->uLastLineOff = lpafoLocalVars->uChar + 1;

    // Mark as encountered
    lpafoLocalVars->bLbrInit = TRUE;

    // Count in another
    lpafoLocalVars->lbrCount++;

    return TRUE;
} // End afoLbrInit


//***************************************************************************
//  $afoLbrExit
//
//  We encountered another right brace
//***************************************************************************

UBOOL afoLbrExit
    (LPAFOLOCALVARS lpafoLocalVars)     // Ptr to AfoDetect local vars

{
    LPAFODETECT_STR lpafoDetectStr = lpafoLocalVars->lpafoDetectStr;

    // Count out another
    lpafoLocalVars->lbrCount--;

    // If we're back to zero, ...
    if (lpafoLocalVars->lbrCount EQ 0)
    {
        // Set new state (we're done)
        lpafoLocalVars->State = AFOROW_EXIT;

        // Make room for one more entry
        if (!AfoDetectResize (lpafoDetectStr))
            goto WSFULL_EXIT;

        // Save the last line length and offset
        lpafoDetectStr->lpafoLineStr[lpafoDetectStr->uLineStrNxt].uLineOff = lpafoDetectStr->uLastLineOff;
        lpafoDetectStr->lpafoLineStr[lpafoDetectStr->uLineStrNxt].uLineLen = lpafoLocalVars->uChar - lpafoDetectStr->uLastLineOff;
        lpafoDetectStr->uLineStrNxt++;
    } // End IF

    return TRUE;

WSFULL_EXIT:
    // Set new state (we're done)
    lpafoLocalVars->State = AFOROW_WSFULL;

    return FALSE;
} // End afoLbrExit


//***************************************************************************
//  $afoLbrCR
//
//  We encountered another CR
//***************************************************************************

UBOOL afoLbrCR
    (LPAFOLOCALVARS lpafoLocalVars)     // Ptr to AfoDetect local vars

{
    LPAFODETECT_STR lpafoDetectStr = lpafoLocalVars->lpafoDetectStr;

    // If we're at the top level, ...
    if (lpafoLocalVars->lbrCount EQ 1)
        // Make room for one more entry
        if (!AfoDetectResize (lpafoDetectStr))
            goto WSFULL_EXIT;

    // Save the line length and offset
    lpafoDetectStr->lpafoLineStr[lpafoDetectStr->uLineStrNxt].uLineOff = lpafoDetectStr->uLastLineOff;
    lpafoDetectStr->lpafoLineStr[lpafoDetectStr->uLineStrNxt].uLineLen = lpafoLocalVars->uChar - lpafoDetectStr->uLastLineOff;

    // If it's a CR/LF, ...
    if (strncmpW (&lpafoLocalVars->lpwszOrig[lpafoLocalVars->uChar], WS_CRLF, strcountof (WS_CRLF)) EQ 0)
    {
        // Skip over it (less one as the loop increments uChar)
        lpafoLocalVars->uChar += strcountof (WS_CRLF) - 1;

        // If we're not at the top level, ...
        if (lpafoLocalVars->lbrCount NE 1)
            // Ignore this CR
            return TRUE;

        // Count it in
        lpafoDetectStr->numPhyLines++;
        lpafoDetectStr->numLogLines++;

        // Mark as NOT continued onto the next physical line
        lpafoDetectStr->lpafoLineStr[lpafoDetectStr->uLineStrNxt].bLineCont = FALSE;
    } else
    // If it's a CR/CR/LF, ...
    if (strncmpW (&lpafoLocalVars->lpwszOrig[lpafoLocalVars->uChar], WS_CRCRLF, strcountof (WS_CRCRLF)) EQ 0)
    {
        // Skip over it (less one as the loop increments uChar)
        lpafoLocalVars->uChar += strcountof (WS_CRCRLF) - 1;

        // If we're not at the top level, ...
        if (lpafoLocalVars->lbrCount NE 1)
            // Ignore this CR
            return TRUE;

        // Count it in
        lpafoDetectStr->numPhyLines++;

        // Mark as continued onto the next physical line
        lpafoDetectStr->lpafoLineStr[lpafoDetectStr->uLineStrNxt].bLineCont = TRUE;
    } // End IF/ELSE
#ifdef DEBUG
    else
        DbgBrk ();
#endif
    // Skip to the next AFOLINE_STR
    lpafoDetectStr->uLineStrNxt++;

    // Save as the last line offset ("+ 1" for the loop increment accounted for above)
    lpafoDetectStr->uLastLineOff = lpafoLocalVars->uChar + 1;

    return TRUE;

WSFULL_EXIT:
    // Set new state (we're done)
    lpafoLocalVars->State = AFOROW_WSFULL;

    return FALSE;
} // End afoLbrCR


//***************************************************************************
//  $afoLbrLamp
//
//  We encountered a comment symbol
//***************************************************************************

UBOOL afoLbrLamp
    (LPAFOLOCALVARS lpafoLocalVars)     // Ptr to AfoDetect local vars

{
    // Skip over everything up to but not including the next CR or EOS
    while (lpafoLocalVars->lpwszOrig[lpafoLocalVars->uChar + 1] NE WC_EOS
        && lpafoLocalVars->lpwszOrig[lpafoLocalVars->uChar + 1] NE WC_CR)
        lpafoLocalVars->uChar++;
    // If we ended on an EOS, ...
    if (lpafoLocalVars->lpwszOrig[lpafoLocalVars->uChar] EQ WC_EOS)
        // Set new state (we're done)
        lpafoLocalVars->State = AFOROW_EXIT;

    return TRUE;
} // End afoLbrLamp


//***************************************************************************
//  $AfoDetect
//
//  Detect an AFO by analyzing its structure
//
//  The structure of an AFO is of teh form
//
//  f {is} {leftbrace}...{rightbrace}\0
//
//  where 'f' is the function name {is} is a left arrow
//    and ... is any text (possibly with embedded AFOs)
//    but always with matching and properly nested braces.
//  Spaces are allowed around the left arrow and after the
//    trailing right brace.
//***************************************************************************

UBOOL AfoDetect
    (LPWCHAR         lpwszLine,         // Ptr to the AFO text
     LPAFODETECT_STR lpafoDetectStr)    // Ptr to AFO Detect struc

{
    APLNELM       aplNELM;              // NELM of lpwszLine
    UINT          uChar;                // Loop counter
    WCHAR         wchOrig;              // The original char
    AFOCOLINDICES colIndex;             // The translated char for tokenization as a AFOCOL_*** value
    AFO_ACTION    afoAction1_EM,        // Ptr to primary action routine
                  afoAction2_EM;        // ...    secondary ...
    AFOLOCALVARS  afoLocalVars = {0};   // Local vars
    UBOOL         bRet;                 // TRUE iff the input is an AFO

    // Get the length
    aplNELM = lstrlenW (lpwszLine);

    // Save local vars in struct which we pass to each FSA action routine
    afoLocalVars.State           = AFOROW_SOS;      // Initial state (Start-Of-Statement)
    afoLocalVars.lpwszOrig       = lpwszLine;       // Save ptr to start of input line
////afoLocalVars.lbrCount        = 0;               // Already zero from = {0}
    afoLocalVars.lpafoDetectStr  = lpafoDetectStr;  // Ptr to AFODETECT_STR

    // Initialize values
    lpafoDetectStr->uLastLineOff = 0;               // Initialize the last line offset
    lpafoDetectStr->numPhyLines  =                  // Initialize the counts
    lpafoDetectStr->numLogLines  = 1;               // ...

    // Skip over leading blanks (more to reduce clutter
    //   in the debugging window)
    for (uChar = 0; uChar < aplNELM; uChar++)
    if (!IsWhiteW (lpwszLine[uChar]))
        break;

    while (uChar <= aplNELM)
    {
        // Use a FSA to tokenize the line

        // Save current index (may be modified by an action)
        afoLocalVars.uChar = uChar;

        /* The FSA works as follows:

           1.  Get the next WCHAR from the input;
           2.  Translate it into a AFOCOL_*** index;
           3.  Use the current state as a row index and the
               above index as a column index into fsaActTableAFO,
               take the appropriate action.
           4.  Repeat until EOL or an error occurs.
         */

        if (uChar EQ aplNELM)
            wchOrig = WC_EOS;
        else
            wchOrig = lpwszLine[uChar];

        // Strip out EOL check so we don't confuse a zero-value char with EOL
        if (uChar EQ aplNELM)
            colIndex = AFOCOL_EOL;
        else
            colIndex = CharTransAFO (wchOrig);

        // Get the action function (if any) and new state
        afoAction1_EM      = fsaActTableAFO[afoLocalVars.State][colIndex].afoAction1;
        afoAction2_EM      = fsaActTableAFO[afoLocalVars.State][colIndex].afoAction2;
        afoLocalVars.State = fsaActTableAFO[afoLocalVars.State][colIndex].iNewState;

        // Check for primary action
        if (afoAction1_EM NE NULL
         && !(*afoAction1_EM) (&afoLocalVars))
            goto ERROR_EXIT;

        // Check for secondary action
        if (afoAction2_EM NE NULL
         && !(*afoAction2_EM) (&afoLocalVars))
            goto ERROR_EXIT;

        // Get next index (may have been modified by an action)
        uChar = afoLocalVars.uChar + 1;

        // Split cases based upon the return code
        switch (afoLocalVars.State)
        {
            case AFOROW_EXIT:
                // Check the trailing chars (if any)
                for (     ; uChar <= aplNELM; uChar++)
                // If the char is not a space, CR, or LF ...
                if (!isspaceW (lpwszLine[uChar])
                 && lpwszLine[uChar] NE WC_CR
                 && lpwszLine[uChar] NE WC_LF)
                    break;

                // Test for matched and properly nested grouping symbols, and
                //   proper trailing chars, and
                //   all bases touched
                bRet = (afoLocalVars.lbrCount EQ 0
                     && (lpwszLine[uChar] EQ WC_EOS
                      || lpwszLine[uChar] EQ UTF16_LAMP)
                     && afoLocalVars.bLbrAlpha
                     && afoLocalVars.bLbrAssign
                     && afoLocalVars.bLbrInit);
                if (bRet)
                    goto NORMAL_EXIT;
                else
                    goto ERROR_EXIT;

            case AFOROW_WSFULL:
                goto WSFULL_EXIT;

            default:
                break;
        } // End SWITCH
    } // End WHILE

    // We should never get here as we process the
    //   trailing zero in the input line which should
    //   exit from one of the actions with AFOROW_EXIT.
    DbgStop ();

WSFULL_EXIT:
    DbgBrk ();              // ***FINISHME***







ERROR_EXIT:
NORMAL_EXIT:
    return bRet;
} // End AfoDetect


//***************************************************************************
//  $CharTransAFO
//
//  Translate a character in preparation for detecting an AFO
//***************************************************************************

AFOCOLINDICES CharTransAFO
    (WCHAR wchOrig)                     // Char to translate into a AFOCOL_xxx index

{
    // Split cases
    switch (wchOrig)
    {
        case L'0':
        case L'1':
        case L'2':
        case L'3':
        case L'4':
        case L'5':
        case L'6':
        case L'7':
        case L'8':
        case L'9':
            return AFOCOL_DIGIT;

        case L'a':
        case L'b':
        case L'c':
        case L'd':
        case L'e':
        case L'f':
        case L'g':
        case L'h':
        case L'i':
        case L'j':
        case L'k':
        case L'l':
        case L'm':
        case L'n':
        case L'o':
        case L'p':
        case L'q':
        case L'r':
        case L's':
        case L't':
        case L'u':
        case L'v':
        case L'w':
        case L'x':
        case L'y':
        case L'z':

        case L'A':
        case L'B':
        case L'C':
        case L'D':
        case L'E':
        case L'F':
        case L'G':
        case L'H':
        case L'I':
        case L'J':
        case L'K':
        case L'L':
        case L'M':
        case L'N':
        case L'O':
        case L'P':
        case L'Q':
        case L'R':
        case L'S':
        case L'T':
        case L'U':
        case L'V':
        case L'W':
        case L'X':
        case L'Y':
        case L'Z':

        case UTF16_A_:                  // Alphabet underbar
        case UTF16_B_:
        case UTF16_C_:
        case UTF16_D_:
        case UTF16_E_:
        case UTF16_F_:
        case UTF16_G_:
        case UTF16_H_:
        case UTF16_I_:
        case UTF16_J_:
        case UTF16_K_:
        case UTF16_L_:
        case UTF16_M_:
        case UTF16_N_:
        case UTF16_O_:
        case UTF16_P_:
        case UTF16_Q_:
        case UTF16_R_:
        case UTF16_S_:
        case UTF16_T_:
        case UTF16_U_:
        case UTF16_V_:
        case UTF16_W_:
        case UTF16_X_:
        case UTF16_Y_:
        case UTF16_Z_:

        case UTF16_DELTA:               // Alt-'h' - delta
        case UTF16_DELTAUNDERBAR:       // Alt-'H' - delta-underbar
        case L'_':
            return AFOCOL_ALPHA;

        case UTF16_OVERBAR:             // High minus
            return AFOCOL_OVERBAR;

        case UTF16_LEFTARROW:           // Left arrow
            return AFOCOL_ASSIGN;

        case UTF16_LEFTBRACE:           // Left brace
            return AFOCOL_LEFTBRACE;

        case UTF16_RIGHTBRACE:          // Right brace
            return AFOCOL_RIGHTBRACE;

        case L' ':
        case WC_HT:
            return AFOCOL_SPACE;

        case UTF16_APOSTROPHE:          // Single quote
            return AFOCOL_QUOTE1;

        case UTF16_DOUBLEQUOTE:         // Double quote
            return AFOCOL_QUOTE2;

        case UTF16_LAMP:                // Comment
            return AFOCOL_LAMP;

        case WC_CR:
            return AFOCOL_CR;

        case WC_EOS:
            return AFOCOL_EOL;

        default:
            return AFOCOL_UNK;
    } // End SWITCH
} // End CharTransAFO


//***************************************************************************
//  End of File: afofns.c
//***************************************************************************
