//***************************************************************************
//  NARS2000 -- Anonymous Function/Operator Routines
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
#include "headers.h"


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
    lpMemDfnHdr = MyGlobalLock (lpplLocalVars->hGlbDfnHdr);

    // If the token has No Value, ...
    if (IsTokenNoValue (&lpYYRhtArg->tkToken))
    {
        LPSYMENTRY     lpSymEntry;
        LPTOKEN_HEADER lpMemTknHdr;
        LPTOKEN        lpMemTknData;

        // Lock the memory to get a ptr to it
        lpMemTknHdr = MyGlobalLock (lpMemDfnHdr->hGlbTknHdr);

        // Skip over the TOKEN_HEADER
        lpMemTknData = TokenBaseToStart (lpMemTknHdr);

        // Get an LPSYMENTRY for $AFORESULT
        lpSymEntry = lpMemTknData[1].tkData.tkSym;

        // If found, attempt to expunge the name
        if (lpSymEntry)
            // Erase the current value of $AFORESULT
            ExpungeName (lpSymEntry, FALSE);

        // We no longer need this ptr
        MyGlobalUnlock (lpMemDfnHdr->hGlbTknHdr);   lpMemTknHdr = NULL;
    } else
    {
        // Transfer the NoDisplay flag to the AFO header
        lpMemDfnHdr->bAfoNoDispRes = lpYYRhtArg->tkToken.tkFlags.NoDisplay;

        // Assign the value to $AFORESULT
        AfoDisplay_EM (&lpYYRhtArg->tkToken, FALSE, lpplLocalVars, lpplLocalVars->hGlbDfnHdr);

        FreeResult (lpYYRhtArg);
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
    if (!aplLongestRht)
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

LPPL_YYSTYPE MakeAfo_EM_YY
    (LPPL_YYSTYPE  lpYYLft,             // Ptr to left brace token
     LPPL_YYSTYPE  lpYYRht,             // Ptr to right ...
     LPPLLOCALVARS lpplLocalVars)       // Ptr to LocalVars
{
    LPPL_YYSTYPE lpYYRes = NULL;        // The result
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
    SF_Fcns.SF_NumLines     = SF_NumLinesAN;        // Ptr to get # lines function
    SF_Fcns.SF_CreationTime = SF_CreationTimeCom;   // Ptr to get function creation time
    SF_Fcns.SF_LastModTime  = SF_LastModTimeCom;    // Ptr to get function last modification time
    SF_Fcns.SF_UndoBuffer   = SF_UndoBufferCom;     // Ptr to get function Undo Buffer global memory handle
////SF_Fcns.hGlbDfnHdr      = NULL;                 // UDFO global memory handle        (Already zero from = {0})
////SF_Fcns.numLocalsSTE    = 0;                    // # locals                         (Already zero from = {0})
////SF_Fcns.lplpLocalSTEs   = NULL;                 // Ptr to save area for local STEs  (Already zero from = {0})
    SF_Fcns.LclParams       = &LW_Params;           // Ptr to local parameters

    // Fill in local values
    LW_Params.lpYYRht       = lpYYRht;              // Ptr to right brace YYSTYPE
////LW_Params.htsDFN        =                       // Already zero from = {0}
    LW_Params.lpplLocalVars = lpplLocalVars;        // Ptr to Local Vars

    Assert (lpYYRht->lptkLftBrace[1].tkFlags.TknType EQ TKT_GLBDFN);

    // Get the AFO global memory handle (may be NULL if not already saved)
    hGlbDfnHdr = lpYYRht->lptkLftBrace[1].tkData.tkGlbData;

    // Call common routine unless we've already saved this AFO
    if (hGlbDfnHdr NE NULL
     || SaveFunctionCom (NULL, &SF_Fcns))
    {
        // ***FIXME*** -- Should we rebase tkCharIndexes now?

        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token type
        // Split cases based upon the DFNTYPE_xxx
        switch (lpYYRht->lptkRhtBrace->tkData.tkDfnType)
        {
            case DFNTYPE_FCN:
                lpYYRes->tkToken.tkFlags.TknType = TKT_FCNAFO;

                break;

            case DFNTYPE_OP1:
                lpYYRes->tkToken.tkFlags.TknType = TKT_OP1AFO;

                break;

            case DFNTYPE_OP2:
                lpYYRes->tkToken.tkFlags.TknType = TKT_OP2AFO;

                break;

            defstop
                break;
        } // End SWITCH

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

        // Fill in the rest of the result token
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = hGlbDfnHdr;
        lpYYRes->tkToken.tkCharIndex       = lpYYLft->tkToken.tkCharIndex;

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
                              &lpYYLft->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    // Free the Hshtab & SymTab
    FreeHshSymTabs (&LW_Params.htsDFN, FALSE);
NORMAL_EXIT:
    return lpYYRes;
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
    lpMemDfnHdr = MyGlobalLock (hGlbDfnHdr);

    Assert (lpMemDfnHdr NE NULL);

    // Lock the memory to get a ptr to it
    lpMemTknHdr = MyGlobalLock (lpMemDfnHdr->hGlbTknHdr);

    // Skip over the TOKEN_HEADER
    lpMemTknData = TokenBaseToStart (lpMemTknHdr);

    // Assign the result to $AFORESULT
    bRet = AssignName_EM (&lpMemTknData[1],
                           lptkSrc);
    // Transfer the NoDisplay flag from the incoming token
    lpMemDfnHdr->bAfoNoDispRes        =
    lpMemTknData[1].tkFlags.NoDisplay = (lptkSrc->tkFlags.NoDisplay || bNoDisplay);

    // We no longer need these ptrs
    MyGlobalUnlock (lpMemDfnHdr->hGlbTknHdr);   lpMemTknHdr = NULL;
    MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;

    // If we succeeded,
    //   and we're displaying, ...
    if (bRet
     && !lptkSrc->tkFlags.NoDisplay)
    {
        // Tell the parser to stop executing this line
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
    if (lpSISCur
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


//***************************************************************************
//  End of File: afofns.c
//***************************************************************************
