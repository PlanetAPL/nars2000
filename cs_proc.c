//***************************************************************************
//  NARS2000 -- Control Structures Processing
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
//  $CS_ChangeTokenType
//
//  Change the token type of a token (cs_yyparse)
//***************************************************************************

void CS_ChangeTokenType
    (LPCSLOCALVARS lpcsLocalVars,       // Ptr to Ctrl Struc local vars
     LPTOKEN       lptkArg1,            // Ptr to 1st argument
     TOKEN_TYPES   TknType)             // New token type

{
    LPTOKEN_HEADER lpMemTknHdr;         // Ptr to tokenized line header global memory
    LPTOKEN        lpMemTknLine;        // Ptr to tokenized line global memory
    LPDFN_HEADER   lpMemDfnHdr;         // Ptr to user-defined function/operator global memory
    LPFCNLINE      lpFcnLines;          // Ptr to array of function line structs (FCNLINE[numFcnLines])
    HGLOBAL        hGlbTknHdr = NULL;   // Tokenized line header global memory handle

    // If this is execute or immexec, ...
    if (lpcsLocalVars->hGlbImmExec NE NULL)
    {
        Assert (lptkArg1->tkData.Orig.c.uLineNum EQ 1);

        // Get the execute/immexec global memory handle
        hGlbTknHdr = lpcsLocalVars->hGlbImmExec;

        // Lock the memory to get a ptr to it
        lpMemTknHdr = MyGlobalLockTkn (hGlbTknHdr);
    } else
    // It's a defined function/operator
    {
        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLockDfn (lpcsLocalVars->hGlbDfnHdr);

        // Get ptr to array of function line structs (FCNLINE[numFcnLines])
        lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

        Assert (lptkArg1->tkData.Orig.c.uLineNum > 0);

        // Get the ptr to the given line's tokenized header global memory
        lpMemTknHdr = (LPTOKEN_HEADER) ByteAddr (lpMemDfnHdr, lpFcnLines[lptkArg1->tkData.Orig.c.uLineNum - 1].offTknLine);
    } // End IF/ELSE

    // Skip over the header to the data
    lpMemTknLine = TokenBaseToStart (lpMemTknHdr);

    // Change the given token number's type
    lpMemTknLine[lptkArg1->tkData.Orig.c.uTknNum].tkFlags.TknType = TknType;

    // Change the value on the CS stack
    lptkArg1->tkFlags.TknType = TknType;

    if (hGlbTknHdr)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbTknHdr); lpMemTknLine = NULL;
    } // End IF

    // If this is NOT execute or immexec, ...
    if (lpcsLocalVars->hGlbImmExec EQ NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lpcsLocalVars->hGlbDfnHdr); lpMemDfnHdr = NULL;
    } // End IF
} // End CS_ChangeTokenType


//***************************************************************************
//  $CS_ChainTokens
//
//  Chain together two CS tokens (cs_yyparse)
//***************************************************************************

void CS_ChainTokens
    (LPCSLOCALVARS lpcsLocalVars,       // Ptr to Ctrl Struc local vars
     LPTOKEN_DATA  lptdArg1,            // Ptr to 1st argument TOKEN_DATA (Reading Orig.c)
     LPTOKEN       lptkArg2)            // Ptr to 2nd argument TOKEN      (Setting Next)

{
    LPTOKEN_HEADER lpMemTknHdr;         // Ptr to tokenized line header global memory
    LPTOKEN        lpMemTknLine;        // Ptr to tokenized line global memory
    LPDFN_HEADER   lpMemDfnHdr;         // Ptr to user-defined function/operator global memory
    LPFCNLINE      lpFcnLines;          // Ptr to array of function line structs (FCNLINE[numFcnLines])
    HGLOBAL        hGlbTknHdr = NULL;   // Tokenized line header global memory handle

    // If this is execute or immexec, ...
    if (lpcsLocalVars->hGlbImmExec NE NULL)
    {
        Assert (lptdArg1->Orig.c.uLineNum EQ 1
             && lptkArg2->tkData.Orig.c.uLineNum EQ 1);

        // Get the execute/immexec global memory handle
        hGlbTknHdr = lpcsLocalVars->hGlbImmExec;

        // Lock the memory to get a ptr to it
        lpMemTknHdr = MyGlobalLockTkn (hGlbTknHdr);
    } else
    // It's a defined function/operator
    {
        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLockDfn (lpcsLocalVars->hGlbDfnHdr);

        // Get ptr to array of function line structs (FCNLINE[numFcnLines])
        lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

        Assert (lptdArg1->Orig.c.uLineNum > 0);

        // Get the given line's tokenized global memory handle
        lpMemTknHdr = (LPTOKEN_HEADER) ByteAddr (lpMemDfnHdr, lpFcnLines[lptdArg1->Orig.c.uLineNum - 1].offTknLine);
    } // End IF/ELSE

    // Skip over the header to the data
    lpMemTknLine = TokenBaseToStart (lpMemTknHdr);

#ifdef DEBUG
    // Check debug level
    if (gDbgLvl >= gChnLvl)
        dprintfWL0 (L"!!Chaining %S (%u,%u) to %S (%u,%u) (%S#%d)",
                  GetTokenTypeName (lpMemTknLine[lptdArg1->Orig.c.uTknNum].tkFlags.TknType),
                  lptdArg1->Orig.c.uLineNum,
                  lptdArg1->Orig.c.uTknNum,
                  GetTokenTypeName (lptkArg2->tkFlags.TknType),
                  lptkArg2->tkData.Orig.c.uLineNum,
                  lptkArg2->tkData.Orig.c.uTknNum,
                  FNLN);
#endif

    // Point the 1st token to the 2nd token
    lpMemTknLine[lptdArg1->Orig.c.uTknNum].tkData.Next.uLineNum = lptkArg2->tkData.Orig.c.uLineNum;
    lpMemTknLine[lptdArg1->Orig.c.uTknNum].tkData.Next.uStmtNum = lptkArg2->tkData.Orig.c.uStmtNum;
    lpMemTknLine[lptdArg1->Orig.c.uTknNum].tkData.Next.uTknNum  = lptkArg2->tkData.Orig.c.uTknNum;

    // Change the values on the CS stack
    lptdArg1->Next.uLineNum = lptkArg2->tkData.Orig.c.uLineNum;
    lptdArg1->Next.uStmtNum = lptkArg2->tkData.Orig.c.uStmtNum;
    lptdArg1->Next.uTknNum  = lptkArg2->tkData.Orig.c.uTknNum;

    if (hGlbTknHdr)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbTknHdr); lpMemTknLine = NULL;
    } // End IF

    // If this is NOT execute or immexec, ...
    if (lpcsLocalVars->hGlbImmExec EQ NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lpcsLocalVars->hGlbDfnHdr); lpMemDfnHdr = NULL;
    } // End IF
} // End CS_ChainTokens


//***************************************************************************
//  $CS_LinkStmt
//
//  Link together a new CS stmt (cs_yyparse)
//***************************************************************************

void CS_LinkStmt
    (LPCSLOCALVARS lpcsLocalVars,       // Ptr to Ctrl Struc local vars
     LPCS_YYSTYPE  lpYYLval)            // Ptr to yylval

{
    Assert (lpYYLval->lptkCur[1].tkFlags.TknType EQ TKT_CS_IF2
         || lpYYLval->lptkCur[1].tkFlags.TknType EQ TKT_CS_FOR2
         || lpYYLval->lptkCur[1].tkFlags.TknType EQ TKT_CS_REPEAT2
         || lpYYLval->lptkCur[1].tkFlags.TknType EQ TKT_CS_SELECT2
         || lpYYLval->lptkCur[1].tkFlags.TknType EQ TKT_CS_WHILE2);

    // Save as ptr to previous last stmt
    lpYYLval->lptkCur[1].tkData.lptkCSLink = lpcsLocalVars->lptkCSLink;

    // Save as new last stmt ptr
    lpcsLocalVars->lptkCSLink = lpYYLval->lptkCur;

    // Set the next .uCLIndex value
    CS_SetCLIndex (lpYYLval);
} // End CS_LinkStmt


//***************************************************************************
//  $CS_UnlinkStmt
//
//  Unlink an old CS stmt (cs_yyparse)
//***************************************************************************

void CS_UnlinkStmt
    (LPCSLOCALVARS lpcsLocalVars,       // Ptr to Ctrl Struc Local vars
     LPCS_YYSTYPE  lpYYLval)            // Ptr to yylval

{
    // Unlink the stmt
    lpcsLocalVars->lptkCSLink = lpYYLval->lptkCur[1].tkData.lptkCSLink;
    lpYYLval->lptkCur[1].tkData.lptkCSLink = NULL;
} // End CS_UnlinkStmt


//***************************************************************************
//  $CS_SetCLIndex
//
//  Set the next .uCLIndex value (cs_yyparse)
//***************************************************************************

void CS_SetCLIndex
    (LPCS_YYSTYPE  lpYYLval)            // Ptr to yylval

{
    static UINT uCLIndex = 0;           // ContinueLeave index

    // Save the starting index common to this group
    lpYYLval->lptkCur->tkData.uCLIndex =
    lpYYLval->uCLIndex                 = ++uCLIndex;

    // The default value of .uCLIndex is zero, so we
    //   need to ensure that if the ++ wraps to zero
    //   we increment it again
    if (lpYYLval->lptkCur->tkData.uCLIndex EQ 0)
        lpYYLval->lptkCur->tkData.uCLIndex =
        lpYYLval->uCLIndex                 = ++uCLIndex;
} // End CS_SetCLIndex


//***************************************************************************
//  $CS_SetTokenCLIndex
//
//  Set the CLIndex of a given token to a given value (cs_yyparse)
//***************************************************************************

void CS_SetTokenCLIndex
    (LPCSLOCALVARS lpcsLocalVars,       // Ptr to Ctrl Struc Local vars
     LPTOKEN_DATA  lptdArg,             // Ptr to arg TOKEN_DATA
     UINT          uCLIndex)            // The CLIndex to use

{
    LPDFN_HEADER   lpMemDfnHdr;         // Ptr to user-defined function/operator global memory
    LPFCNLINE      lpFcnLines;          // Ptr to array of function line structs (FCNLINE[numFcnLines])
    HGLOBAL        hGlbTknHdr = NULL;   // Tokenized line header global memory handle
    LPTOKEN_HEADER lpMemTknHdr;         // Ptr to tokenized line header global memory
    LPTOKEN        lpMemTknLine;        // Ptr to tokenized line global memory

    // If this is execute or immexec, ...
    if (lpcsLocalVars->hGlbImmExec NE NULL)
    {
        Assert (lptdArg->Orig.c.uLineNum EQ 1);

        // Get the execute/immexec global memory handle
        hGlbTknHdr = lpcsLocalVars->hGlbImmExec;

        // Lock the memory to get a ptr to it
        lpMemTknHdr = MyGlobalLockTkn (hGlbTknHdr);
    } else
    // It's a defined function/operator
    {
        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLockDfn (lpcsLocalVars->hGlbDfnHdr);

        // Get ptr to array of function line structs (FCNLINE[numFcnLines])
        lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

        Assert (lptdArg->Orig.c.uLineNum > 0);

        // Get the given line's tokenized global memory handle
        lpMemTknHdr = (LPTOKEN_HEADER) ByteAddr (lpMemDfnHdr, lpFcnLines[lptdArg->Orig.c.uLineNum - 1].offTknLine);
    } // End IF/ELSE

    // Skip over the header to the data
    lpMemTknLine = TokenBaseToStart (lpMemTknHdr);

    // Set the CLIndex
    lpMemTknLine[lptdArg->Orig.c.uTknNum].tkData.uCLIndex = uCLIndex;

    if (hGlbTknHdr)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbTknHdr); lpMemTknLine = NULL;
    } // End IF

    // If this is NOT execute or immexec, ...
    if (lpcsLocalVars->hGlbImmExec EQ NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lpcsLocalVars->hGlbDfnHdr); lpMemDfnHdr = NULL;
    } // End IF
} // End CS_SetTokenCLIndex


//***************************************************************************
//  Above this point all the routines are called from cs_yyparse.
//***************************************************************************
//  Below this point all the routines are called from pl_yyparse.
//***************************************************************************


//***************************************************************************
//  $CS_ASSERT_Stmt_EM
//
//  Process ASSERT stmt (pl_yyparse)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CS_ASSERT_Stmt_EM"
#else
#define APPEND_NAME
#endif

UBOOL CS_ASSERT_Stmt_EM
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPPL_YYSTYPE  lpYYRhtArg)          // Ptr to right arg

{
    APLSTYPE   aplTypeRht;              // Right arg storage type
    APLNELM    aplNELMRht;              // Right arg NELM
    APLRANK    aplRankRht;              // Right arg rank
    APLLONGEST aplLongestRht;           // Right arg longest if immediate
    UBOOL      bRet;                    // TRUE iff the result is valid

    // The arg to the ASSERT stmt must be a Boolean-valued scalar or
    //   one-element vector.

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (&lpYYRhtArg->tkToken, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RANK_EXIT;

    // Check for LENGTH ERROR
    if (!IsSingleton (aplNELMRht))
        goto LENGTH_EXIT;

    // Get and validate the first item in a token
    aplLongestRht = ValidateFirstItemToken (aplTypeRht, &lpYYRhtArg->tkToken, &bRet);

    // Check for DOMAIN ERROR
    if (!bRet
     || !IsBooleanValue (aplLongestRht))
         goto DOMAIN_EXIT;

    if (aplLongestRht EQ 0)
        ErrorMessageIndirectToken (ERRMSG_ASSERTION_ERROR APPEND_NAME,
                                  &lpYYRhtArg->tkToken);
    return (UBOOL) aplLongestRht;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                              &lpYYRhtArg->tkToken);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                              &lpYYRhtArg->tkToken);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYRhtArg->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    return FALSE;
} // End CS_ASSERT_Stmt_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CS_CASE_Stmt
//
//  Process CASE/CASELIST stmt (pl_yyparse)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CS_CASE_Stmt"
#else
#define APPEND_NAME
#endif

UBOOL CS_CASE_Stmt
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPPL_YYSTYPE  lpYYRhtArg)          // Ptr to right arg

{
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Copy the result
    lpMemPTD->YYCaseExec = *lpYYRhtArg;

    // If the result is not immediate, ...
    if (lpMemPTD->YYCaseExec.tkToken.tkFlags.TknType NE TKT_VARIMMED)
        lpMemPTD->YYCaseExec.tkToken.tkData.tkGlbData =
          CopySymGlbDirAsGlb (lpMemPTD->YYCaseExec.tkToken.tkData.tkGlbData);

    return TRUE;
} // End CS_CASE_Stmt
#undef  APPEND_NAME


//***************************************************************************
//  $CS_CONTINUE_Stmt
//
//  Process CONTINUE stmt (pl_yyparse)
//***************************************************************************

UBOOL CS_CONTINUE_Stmt
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN       lptkContinueArg)     // Ptr to CONTINUE arg

{
    // Tell the lexical analyzer to get the next token from
    //   the start of the stmt at the token pointed to by
    //   the CONTINUE stmt
    CS_SetNextStmtToStmtAt_NEXT (lpplLocalVars, &lptkContinueArg->tkData);

    // If the starting and ending stmts are not on the same line, ...
    if (lptkContinueArg->tkData.Next.uLineNum NE lpplLocalVars->uLineNum)
        // Tell the parser to stop executing this line
        lpplLocalVars->bStopExec = TRUE;

    // If we're not stopping execution, ...
    if (!lpplLocalVars->bStopExec)
        // We're restarting
        lpplLocalVars->bRestart = TRUE;

    return TRUE;
} // End CS_CONTINUE_Stmt


//***************************************************************************
//  $CS_CONTINUEIF_Stmt_EM
//
//  Process CONTINUEIF stmt (pl_yyparse)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CS_CONTINUEIF_Stmt_EM"
#else
#define APPEND_NAME
#endif

UBOOL CS_CONTINUEIF_Stmt_EM
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN       lptkContinueIfArg,   // Ptr to CONTINUEIF arg
     LPPL_YYSTYPE  lpYYRhtArg)          // Ptr to right arg

{
    APLSTYPE   aplTypeRht;              // Right arg storage type
    APLNELM    aplNELMRht;              // Right arg NELM
    APLRANK    aplRankRht;              // Right arg rank
    APLLONGEST aplLongestRht;           // Right arg longest if immediate
    UBOOL      bRet;                    // TRUE iff the result is valid

    // The arg to the CONTINUEIF stmt must be a Boolean-valued scalar or
    //   one-element vector.

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (&lpYYRhtArg->tkToken, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RANK_EXIT;

    // Check for LENGTH ERROR
    if (!IsSingleton (aplNELMRht))
        goto LENGTH_EXIT;

    // Get and validate the first item in a token
    aplLongestRht = ValidateFirstItemToken (aplTypeRht, &lpYYRhtArg->tkToken, &bRet);

    // Check for DOMAIN ERROR
    if (!bRet
     || !IsBooleanValue (aplLongestRht))
         goto DOMAIN_EXIT;

    if (aplLongestRht)
        CS_CONTINUE_Stmt (lpplLocalVars, lptkContinueIfArg);

    return TRUE;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                              &lpYYRhtArg->tkToken);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                              &lpYYRhtArg->tkToken);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYRhtArg->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    return FALSE;
} // End CS_CONTINUEIF_Stmt_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CS_ELSE_Stmt
//
//  Process ELSE stmt (pl_yyparse)
//***************************************************************************

UBOOL CS_ELSE_Stmt
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN       lptkElseArg)         // Ptr to ELSE arg

{
    // Entry into this function means that we dropped through from above
    //   and should branch to the matching ENDIF/ENDSELECT stmt

    // Tell the lexical analyzer to get the next token from
    //   the stmt after the token pointed to by the ELSE stmt
    CS_SetNextStmtToStmtAfter_NEXT (lpplLocalVars, &lptkElseArg->tkData);

#ifdef DEBUG
    {
        TOKEN_TYPES TknType;

        // Get the token type of the stmt pointed to by the ELSE stmt
        TknType = CS_GetTokenType_NEXT (lpplLocalVars, &lptkElseArg->tkData);
        Assert (TknType EQ TKT_CS_ENDIF
             || TknType EQ TKT_CS_ENDSELECT);
    }
#endif
    // If the starting and ending stmts are not on the same line, ...
    if (lptkElseArg->tkData.Next.uLineNum NE lpplLocalVars->uLineNum)
        // Tell the parser to stop executing this line
        lpplLocalVars->bStopExec = TRUE;

    return TRUE;
} // End CS_ELSE_Stmt


//***************************************************************************
//  $FindMatchingForStmt
//
//  Find the matching FORSTMT/FORLCLSTMT ptr
//***************************************************************************

LPFORSTMT FindMatchingForStmt
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     UINT          uCLIndex)            // Matching CL index

{
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPSIS_HEADER lpSISCur;              // Ptr to current SIS header
    LPFORSTMT    lpForStmtNext;         // Ptr to next available entry on FORSTMT stack

    // Get ptr to PerTabData global memory
    lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Copy ptr to current SI level
    lpSISCur = lpMemPTD->lpSISCur;

    // Check for uninitialized FORSTMT/FORLCLSTMT stack
    if (lpSISCur->lpForStmtBase EQ lpSISCur->lpForStmtNext)
        return NULL;

    // Get ptr to current entry on FORSTMT/FORLCLSTMT stack
    lpForStmtNext = &lpSISCur->lpForStmtNext[-1];

    // Check that this level on the FORSTMT/FORLCLSTMT stack matches
    //   this ENDFOR/ENDFORLCL stmt
    if (uCLIndex NE lpForStmtNext->uForStmtID)
        return NULL;

    return lpForStmtNext;
} // End FindMatchingForStmt


//***************************************************************************
//  $CS_ENDFOR_Stmt_EM
//
//  Process ENDFOR/ENDFORLCL stmt (pl_yyparse)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CS_ENDFOR_Stmt_EM"
#else
#define APPEND_NAME
#endif

UBOOL CS_ENDFOR_Stmt_EM
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN       lptkEndForArg,       // Ptr to ENDFOR arg
     UBOOL         bFORLCL)             // TRUE iff FORLCL

{
    LPFORSTMT lpForStmtNext;            // Ptr to next available entry on FORSTMT/FORLCLSTMT stack

    // Find the matching FORSTMT/FORLCLSTMT ptr
    lpForStmtNext =
      FindMatchingForStmt (lpplLocalVars,
                           lptkEndForArg->tkData.uCLIndex);
    // If it's not found, it's a spurious branch to :endfor
    if (lpForStmtNext EQ NULL)
        goto UNINIT_EXIT;

    // Check the index against the limit
    if (lpForStmtNext->uIndex < lpForStmtNext->uNELM)
    {
        // Get the next value from the IN array token into the Named Var token
        //   and increment the index
        if (!GetNextValueTokenIntoNamedVarToken_EM (&lpForStmtNext->tkForArr,
                                                     lpForStmtNext->uIndex++,
                                                    &lpForStmtNext->tkForI))
        {
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                       lptkEndForArg);
            return FALSE;
        } // End IF

        // Tell the lexical analyzer to get the next token from
        //   the stmt after the token pointed to by the ENDFOR/ENDFORLCL stmt
        CS_SetNextStmtToStmtAfter_NEXT (lpplLocalVars, &lptkEndForArg->tkData);

        // If the starting and ending stmts are not on the same line, ...
        if (lptkEndForArg->tkData.Next.uLineNum NE lpplLocalVars->uLineNum)
            // Tell the parser to stop executing this line
            lpplLocalVars->bStopExec = TRUE;
    } else
        // We're done with this FOR/FORLCL stmt
        CS_DoneFOR (lpplLocalVars, bFORLCL);

        // By not changing the line/token #, we fall through to the next stmt
UNINIT_EXIT:
    return TRUE;
} // End CS_ENDFOR_Stmt_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CS_DoneFOR
//
//  We're done with this FOR/FORLCL stmt-- free the global in the token (if any),
//    and delete this entry
//***************************************************************************

void CS_DoneFOR
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     UBOOL         bFORLCL)             // TRUE iff FORLCL

{
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPFORSTMT    lpForStmtNext;         // Ptr to next available entry on FORSTMT/FORLCLSTMT stack

    // Get ptr to PerTabData global memory
    lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Get ptr to current entry on FORSTMT/FORLCLSTMT stack
    //   and delete this entry
    lpForStmtNext = --lpMemPTD->lpSISCur->lpForStmtNext;

    // We're done -- free the global in the token (if any)
    FreeResultTkn (&lpForStmtNext->tkForArr);

    if (bFORLCL)
    {
        // Free the value of the :IN var
        FreeResultName (&lpForStmtNext->tkForI);

        // Restore the original :IN var
        *lpForStmtNext->tkForI.tkData.tkSym = lpForStmtNext->symForI;
    } // End IF

    // Zap the entry
    ZeroMemory (lpForStmtNext, sizeof (*lpForStmtNext));
} // End CS_DoneFOR


//***************************************************************************
//  $CS_ENDREPEAT_Stmt
//
//  Process ENDREPEAT stmt (pl_yyparse)
//***************************************************************************

UBOOL CS_ENDREPEAT_Stmt
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN       lptkEndRepeatArg)    // Ptr to ENDREPEAT arg

{
    // Tell the lexical analyzer to get the next token from
    //   the stmt after the token pointed to by the ENDREPEAT stmt
    CS_SetNextStmtToStmtAfter_NEXT (lpplLocalVars, &lptkEndRepeatArg->tkData);

    // If the starting and ending stmts are not on the same line, ...
    if (lptkEndRepeatArg->tkData.Next.uLineNum NE lpplLocalVars->uLineNum)
        // Tell the parser to stop executing this line
        lpplLocalVars->bStopExec = TRUE;

    return TRUE;
} // End CS_ENDREPEAT_Stmt


//***************************************************************************
//  $CS_ENDWHILE_Stmt
//
//  Process ENDWHILE stmt (pl_yyparse)
//***************************************************************************

UBOOL CS_ENDWHILE_Stmt
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN       lptkEndWhileArg)     // Ptr to ENDWHILE arg

{
    // Tell the lexical analyzer to get the next token from
    //   the start of the stmt at the token pointed to by
    //   the ENDWHILE stmt
    CS_SetNextStmtToStmtAt_NEXT (lpplLocalVars, &lptkEndWhileArg->tkData);

    // If the starting and ending stmts are not on the same line, ...
    if (lptkEndWhileArg->tkData.Next.uLineNum NE lpplLocalVars->uLineNum)
        // Tell the parser to stop executing this line
        lpplLocalVars->bStopExec = TRUE;

    // If we're not stopping execution, ...
    if (!lpplLocalVars->bStopExec)
        // We're restarting
        lpplLocalVars->bRestart = TRUE;

    return TRUE;
} // End CS_ENDWHILE_Stmt


//***************************************************************************
//  $CS_FOR_Stmt_EM
//
//  Process FOR/FORLCL stmt (pl_yyparse)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CS_FOR_Stmt_EM"
#else
#define APPEND_NAME
#endif

UBOOL CS_FOR_Stmt_EM
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPPL_YYSTYPE  lpYYForArg,          // Ptr to FOR arg
     LPPL_YYSTYPE  lpYYNameArg,         // Ptr to Name arg
     LPPL_YYSTYPE  lpYYRhtArg)          // Ptr to right arg

{
    UBOOL        bFORLCL;               // TRUE iff FORLCL
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPFORSTMT    lpForStmtNext;         // Ptr to next available entry on FORSTMT/FORLCLSTMT stack

    // Set the FORLCL flag
    bFORLCL = lpYYForArg->tkToken.tkFlags.TknType EQ TKT_CS_FORLCL;

    Assert (bFORLCL || lpYYForArg->tkToken.tkFlags.TknType EQ TKT_CS_FOR);

    // Find the matching FORSTMT/FORLCLSTMT ptr
    // If it's found, this is a branch to an active FOR/FORLCL stmt
    //   and we must terminate it before activating it again
    if (FindMatchingForStmt (lpplLocalVars,
                             lpYYForArg->tkToken.tkData.uCLIndex))
        // We're done with this FOR/FORLCL stmt
        CS_DoneFOR (lpplLocalVars, bFORLCL);

    // Get ptr to PerTabData global memory
    lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Save ptr to next available entry on FORSTMT/FORLCLSTMT stack
    //   and skip over this to the next entry
    lpForStmtNext = lpMemPTD->lpSISCur->lpForStmtNext++;

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (&lpYYRhtArg->tkToken, NULL, &lpForStmtNext->uNELM, NULL, NULL);

    // Save initial values on the FORSTMT/FORLCLSTMT stack
    lpForStmtNext->uIndex     = 0;
    lpForStmtNext->tkForI     = lpYYNameArg->tkToken;
    lpForStmtNext->tkForArr   = *CopyToken_EM (&lpYYRhtArg->tkToken, FALSE);
    lpForStmtNext->uForStmtID = lpYYForArg->tkToken.tkData.uCLIndex;

    if (bFORLCL)
    {
        // Make a copy of the STE to restore later
        lpForStmtNext->symForI = *lpYYNameArg->tkToken.tkData.tkSym;

        // Set the ptr to the previous entry to the STE in its shadow chain
        lpYYNameArg->tkToken.tkData.tkSym->stPrvEntry = &lpForStmtNext->symForI;

        // Set the STE to NoValue; otherwise the first assignment into the :IN var removes the old value
        lpYYNameArg->tkToken.tkFlags.TknType             = TKT_VARNAMED;
        lpYYNameArg->tkToken.tkFlags.ImmType             = IMMTYPE_ERROR;
        lpYYNameArg->tkToken.tkData.tkSym->stFlags.Value = FALSE;
    } else
        // Mark as not valid
        lpForStmtNext->symForI.stFlags.Inuse = FALSE;

    // Tell the lexical analyzer to get the next token from
    //   the start of the stmt at the token pointed to by
    //   the FOR/FORLCL stmt (the ENDFOR/ENDFORLCL stmt)
    CS_SetNextStmtToStmtAt_NEXT (lpplLocalVars, &lpYYForArg->tkToken.tkData);

    // If we're not stopping execution, ...
    if (!lpplLocalVars->bStopExec)
        // We're restarting
        lpplLocalVars->bRestart = TRUE;

    return TRUE;
} // End CS_FOR_Stmt_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CS_IF_Stmt_EM
//
//  Process IF/ELSEIF/ANDIF/ORIF/UNTIL/WHILE stmt (pl_yyparse)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CS_IF_Stmt"
#else
#define APPEND_NAME
#endif

UBOOL CS_IF_Stmt_EM
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN       lptkIfArg,           // Ptr to IF/... arg
     LPPL_YYSTYPE  lpYYRhtArg)          // Ptr to right arg

{
    APLSTYPE   aplTypeRht;              // Right arg storage type
    APLNELM    aplNELMRht;              // Right arg NELM
    APLRANK    aplRankRht;              // Right arg rank
    APLLONGEST aplLongestRht;           // Right arg longest if immediate
    UBOOL      bRet;                    // TRUE iff the result is valid
    TOKEN_DATA tdCur,                   // TOKEN_DATA of the IF/... stmt
               tdPrv;                   // ...               previous stmt
    TOKEN      tkNxt;                   // Next token

    // The arg to the IF/... stmt must be a Boolean-valued scalar or
    //   one-element vector.
    // If the stmt pointed to by the IF/... stmt is ORIF or ANDIF, process
    //   that sequence depending upon the value of the arg to the IF/... stmt.
    // Otherwise, if the value of the arg to the IF/... stmt is 1, continue
    //   execution with the next stmt; if the value is 0, look at the
    //   token pointed to by the IF/... stmt:
    // If that token is an ELSEIF, start execution with that stmt.
    // If that token is an ELSE, start execution with the stmt after that.
    // If that token is an ENDIF, start execution with the stmt after that.
    // If that token is an ENDWHILE, start execution with the stmt to which that points.

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (&lpYYRhtArg->tkToken, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RANK_EXIT;

    // Check for LENGTH ERROR
    if (!IsSingleton (aplNELMRht))
        goto LENGTH_EXIT;

    // Get and validate the first item in a token
    aplLongestRht = ValidateFirstItemToken (aplTypeRht, &lpYYRhtArg->tkToken, &bRet);

    // Check for DOMAIN ERROR
    if (!bRet
     || !IsBooleanValue (aplLongestRht))
         goto DOMAIN_EXIT;

    // Copy the token data
    tdCur = lptkIfArg->tkData;

    // Get the contents of the token pointed to by
    //   the IF/... token
    CS_GetToken_NEXT (lpplLocalVars, &tdCur, &tkNxt);

    // Split cases based upon the type of the token to which the IF/... stmt points
    switch (tkNxt.tkFlags.TknType)
    {
        case TKT_CS_ANDIF:
            //***************************************************************
            // tdCur/tkNxt are chained as in
            //   IF ...         ELSEIF ...      UNTIL ...   WHILE ...   ANDIF ...
            //   ANDIF ...      ANDIF ...       ANDIF ...   ANDIF ...   ANDIF ...
            //***************************************************************
#ifdef DEBUG
            {
                TOKEN_TYPES TknType;

                // Get the token type of the stmt which contains the tdCur token
                TknType = CS_GetTokenType_ORIG (lpplLocalVars, &tdCur);
                Assert (TknType EQ TKT_CS_IF
                     || TknType EQ TKT_CS_ELSEIF
                     || TknType EQ TKT_CS_UNTIL
                     || TknType EQ TKT_CS_WHILE
                     || TknType EQ TKT_CS_ANDIF
                       );
            }
#endif

            // If the IF/ELSEIF/UNTIL/WHILE/ANDIF arg is 0, ...
            if (aplLongestRht EQ 0)
            {
                // Execute the stmt pointed to by the IF/ELSEIF/UNTIL/WHILE or last ANDIF token

                // Loop through the ANDIF chain stopping at the first token
                //   in the chain which is not ANDIF
                while (tkNxt.tkFlags.TknType EQ TKT_CS_ANDIF)
                {
                    // Get the TOKEN_DATA from the ANDIF token
                    tdPrv = tkNxt.tkData;

                    // Get the contents of the token pointed to by
                    //   the ANDIF token
                    CS_GetToken_NEXT (lpplLocalVars, &tdPrv, &tkNxt);
                } // End WHILE

                // tdPrv contains the TOKEN_DATA of the IF/ELSEIF/UNTIL/WHILE or last ANDIF token
                // tkNxt contains the TOKEN pointed to by the tdPrv token

                Assert (tkNxt.tkFlags.TknType EQ TKT_CS_ELSE
                     || tkNxt.tkFlags.TknType EQ TKT_CS_ELSEIF
                     || tkNxt.tkFlags.TknType EQ TKT_CS_ENDIF
                     || tkNxt.tkFlags.TknType EQ TKT_CS_ENDWHILE
                     || tkNxt.tkFlags.TknType EQ TKT_CS_UNTIL
                     || tkNxt.tkFlags.TknType EQ TKT_CS_REPEAT
                     || tkNxt.tkFlags.TknType EQ TKT_CS_WHILE);

                // If the next token is has an argument (ELSEIF/UNTIL/WHILE), execute that stmt
                if (tkNxt.tkFlags.TknType EQ TKT_CS_ELSEIF
                 || tkNxt.tkFlags.TknType EQ TKT_CS_UNTIL
                 || tkNxt.tkFlags.TknType EQ TKT_CS_WHILE)
                    // Tell the lexical analyzer to get the next token from
                    //   the start of the stmt pointed to by the ANDIF token
                    //   (in this case the ELSEIF/UNTIL/WHILE token)
                    CS_SetNextStmtToStmtAt_NEXT (lpplLocalVars, &tdPrv);
                else
                    // Tell the lexical analyzer to get the next token from
                    //   the start of the stmt after the stmt pointed to by
                    //   the ANDIF token
                    CS_SetNextStmtToStmtAfter_NEXT (lpplLocalVars, &tdPrv);
            } else
            // Split cases based upon the tdCur token type
            switch (CS_GetTokenType_ORIG (lpplLocalVars, &tdCur))
            {
                case TKT_CS_UNTIL:
                case TKT_CS_ANDIF:
                    // Tell the lexical analyzer to get the next token from
                    //   the start of the stmt pointed to by the ANDIF token
                    CS_SetNextStmtToStmtAt_NEXT (lpplLocalVars, &tdCur);

                    break;

                default:
                    // Tell the lexical analyzer to get the next token from
                    //   the start of the stmt after the IF/ELSEIF token
                    CS_SetNextStmtToStmtAfter_ORIG (lpplLocalVars, &tdCur);

                    break;
            } // End SWITCH

            // If we're not stopping execution, ...
            if (!lpplLocalVars->bStopExec)
                // We're restarting
                lpplLocalVars->bRestart = TRUE;
            break;

        case TKT_CS_ORIF:
            //***************************************************************
            // tdCur/tkNxt are chained as in
            //   IF ...         ELSEIF ...      UNTIL ...   WHILE ...   ORIF ...
            //   ORIF ...       ORIF ...        ORIF ...    ORIF ...    ORIF ...
            //***************************************************************
#ifdef DEBUG
            {
                TOKEN_TYPES TknType;

                // Get the token type of the stmt which contains the tdCur token
                TknType = CS_GetTokenType_ORIG (lpplLocalVars, &tdCur);
                Assert (TknType EQ TKT_CS_IF
                     || TknType EQ TKT_CS_ELSEIF
                     || TknType EQ TKT_CS_UNTIL
                     || TknType EQ TKT_CS_WHILE
                     || TknType EQ TKT_CS_ORIF
                       );
            }
#endif
            // If the IF/ELSEIF/UNTIL/WHILE/ORIF arg is 1, ...
            if (aplLongestRht EQ 1)
            {
                // Execute the stmt after the IF/ELSEIF/UNTIL/WHILE or last ORIF token

                // Loop through the ORIF chain stopping at the first token
                //   in the chain which is not ORIF
                while (tkNxt.tkFlags.TknType EQ TKT_CS_ORIF)
                {
                    // Get the TOKEN_DATA from the ORIF token
                    tdPrv = tkNxt.tkData;

                    // Get the contents of the token pointed to by
                    //   the ORIF token
                    CS_GetToken_NEXT (lpplLocalVars, &tdPrv, &tkNxt);
                } // End WHILE

                Assert (tkNxt.tkFlags.TknType EQ TKT_CS_ELSE
                     || tkNxt.tkFlags.TknType EQ TKT_CS_ELSEIF
                     || tkNxt.tkFlags.TknType EQ TKT_CS_ENDIF
                     || tkNxt.tkFlags.TknType EQ TKT_CS_ENDWHILE
                     || tkNxt.tkFlags.TknType EQ TKT_CS_UNTIL
                     || tkNxt.tkFlags.TknType EQ TKT_CS_REPEAT
                     || tkNxt.tkFlags.TknType EQ TKT_CS_WHILE);

                // tdPrv contains the TOKEN_DATA of the IF/ELSEIF/UNTIL/WHILE or last ORIF token
                // tkNxt is unused

                // Tell the lexical analyzer to get the next token from
                //   the start of the stmt after the IF/ELSEIF/UNTIL/WHILE or last ORIF stmt
                CS_SetNextStmtToStmtAfter_ORIG (lpplLocalVars, &tdPrv);
            } else
            // Split cases based upon the tdCur token type
            switch (CS_GetTokenType_ORIG (lpplLocalVars, &tdCur))
            {
                case TKT_CS_UNTIL:
                case TKT_CS_ORIF:
                    // Tell the lexical analyzer to get the next token from
                    //   the start of the stmt pointed to by the UNTIL/ORIF token
                    CS_SetNextStmtToStmtAt_NEXT (lpplLocalVars, &tdCur);

                    break;

                default:
                    // Tell the lexical analyzer to get the next token from
                    //   the start of the stmt after the IF/ELSEIF token
                    CS_SetNextStmtToStmtAfter_ORIG (lpplLocalVars, &tdCur);
            } // End SWITCH

            // If we're not stopping execution, ...
            if (!lpplLocalVars->bStopExec)
                // We're restarting
                lpplLocalVars->bRestart = TRUE;
            break;

        case TKT_CS_ELSEIF:
        case TKT_CS_UNTIL:
            //***************************************************************
            // tdCur/tkNxt are chained as in
            //   IF ...         ELSEIF ...  ANDIF ...   ORIF ...    WHILE ...
            //   ELSEIF ...     ELSEIF ...  ELSEIF ...  ELSEIF ...  UNTIL ...
            //***************************************************************
#ifdef DEBUG
            {
                TOKEN_TYPES TknType;

                // Get the token type of the stmt which contains the tdCur token
                TknType = CS_GetTokenType_ORIG (lpplLocalVars, &tdCur);
                Assert (TknType EQ TKT_CS_IF
                     || TknType EQ TKT_CS_ELSEIF
                     || TknType EQ TKT_CS_ANDIF
                     || TknType EQ TKT_CS_ORIF
                     || TknType EQ TKT_CS_WHILE
                       );
            }
#endif
            // If the IF/ELSEIF/ANDIF/ORIF/WHILE arg is 0, ...
            if (aplLongestRht EQ 0)
            {
                // If tdCur is WHILE, ...
                if (CS_GetTokenType_ORIG (lpplLocalVars, &tdCur) EQ TKT_CS_WHILE)
                {
                    // Loop through the ANDIF/ORIF chain stopping at the first token
                    //   in the chain which is not ANDIF/ORIF
                    while (tkNxt.tkFlags.TknType EQ TKT_CS_UNTIL    // Needed for first time only, harmless later
                        || tkNxt.tkFlags.TknType EQ TKT_CS_ANDIF
                        || tkNxt.tkFlags.TknType EQ TKT_CS_ORIF)
                    {
                        // Get the TOKEN_DATA from the UNTIL/ANDIF/ORIF token
                        tdPrv = tkNxt.tkData;

                        // Get the contents of the token pointed to by
                        //   the UNTIL/ANDIF/ORIF token
                        CS_GetToken_NEXT (lpplLocalVars, &tdPrv, &tkNxt);
                    } // End WHILE

                    // tdPrv contains the TOKEN_DATA of the last UNTIL/ANDIF/ORIF token
                    // tkNxt is unused

                    // Tell the lexical analyzer to get the next token from
                    //   the start of the stmt after the last UNTIL/ANDIF/ORIF token
                    CS_SetNextStmtToStmtAfter_ORIG (lpplLocalVars, &tdPrv);
                } else
                    // Tell the lexical analyzer to get the next token from
                    //   the stmt at the token pointed to by the IF/ELSEIF/ANDIF/ORIF/WHILE stmt
                    CS_SetNextStmtToStmtAt_NEXT (lpplLocalVars, &tdCur);

                // If we're not stopping execution, ...
                if (!lpplLocalVars->bStopExec)
                    // We're restarting
                    lpplLocalVars->bRestart = TRUE;
            } // End IF

            break;

        case TKT_CS_ELSE:
        case TKT_CS_ENDIF:
        case TKT_CS_ENDWHILE:
            //***************************************************************
            // tdCur/tkNxt are chained as in
            //   IF ...         ELSEIF ...      ANDIF ...   ORIF ...
            //   ELSE           ELSE            ELSE        ELSE
            //
            //   IF ...         ELSEIF ...      ANDIF ...   ORIF ...
            //   ENDIF          ENDIF           ENDIF       ENDIF
            //
            //   WHILE ...
            //   ENDWHILE ...
            //***************************************************************
#ifdef DEBUG
            {
                TOKEN_TYPES TknType;

                // Get the token type of the stmt which contains the tdCur token
                TknType = CS_GetTokenType_ORIG (lpplLocalVars, &tdCur);
                Assert (TknType EQ TKT_CS_IF
                     || TknType EQ TKT_CS_ELSEIF
                     || TknType EQ TKT_CS_WHILE
                     || TknType EQ TKT_CS_ANDIF
                     || TknType EQ TKT_CS_ORIF
                       );
            }
#endif
            // If the IF/... arg is 0, ...
            if (aplLongestRht EQ 0)
            {
                // Tell the lexical analyzer to get the next token from
                //   the stmt after the token pointed to by the ELSE/ENDIF/ENDWHILE stmt
                CS_SetNextStmtToStmtAfter_NEXT (lpplLocalVars, &tdCur);

                // If we're not stopping execution, ...
                if (!lpplLocalVars->bStopExec)
                    // We're restarting
                    lpplLocalVars->bRestart = TRUE;
            } // End IF

            break;

        case TKT_CS_WHILE:
        case TKT_CS_REPEAT:
            //***************************************************************
            // tdCur/tkNxt are chained as in
            //   UNTIL ...      ANDIF ...       ORIF ...
            //   WHILE ...      WHILE ...       WHILE ...
            //
            //   UNTIL ...      ANDIF ...       ORIF ...
            //   REPEAT         REPEAT          REPEAT
            //***************************************************************
#ifdef DEBUG
            {
                TOKEN_TYPES TknType;

                // Get the token type of the stmt which contains the tdCur token
                TknType = CS_GetTokenType_ORIG (lpplLocalVars, &tdCur);
                Assert (TknType EQ TKT_CS_UNTIL
                     || TknType EQ TKT_CS_ANDIF
                     || TknType EQ TKT_CS_ORIF
                       );
            }
#endif
            // If the UNTIL/ANDIF/ORIF arg is 0, ...
            if (aplLongestRht EQ 0)
            {
                // Split cases based upon the tkNxt token type
                switch (tkNxt.tkFlags.TknType)
                {
                    case TKT_CS_WHILE:      // It has an argument, so we execute it (At_NEXT)
                        // Tell the lexical analyzer to get the next token from
                        //   the stmt at the token pointed to by the UNTIL/ANDIF/ORIF stmt
                        CS_SetNextStmtToStmtAt_NEXT (lpplLocalVars, &tdCur);

                        break;

                    case TKT_CS_REPEAT:     // It doesn't have an argument, so we execute
                                            //   the stmt after (After_NEXT)
                        // Tell the lexical analyzer to get the next token from
                        //   the stmt after the token pointed to by the UNTIL/ANDIF/ORIF stmt
                        CS_SetNextStmtToStmtAfter_NEXT (lpplLocalVars, &tdCur);

                        break;

                    defstop
                        break;
                } // End SWITCH

                // If we're not stopping execution, ...
                if (!lpplLocalVars->bStopExec)
                    // We're restarting
                    lpplLocalVars->bRestart = TRUE;
            } // End IF

            break;

        defstop
            break;
    } // End SWITCH

    return TRUE;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                              &lpYYRhtArg->tkToken);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                              &lpYYRhtArg->tkToken);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYRhtArg->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    return FALSE;
} // End CS_IF_Stmt_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CS_LEAVE_Stmt
//
//  Process LEAVE stmt (pl_yyparse)
//***************************************************************************

UBOOL CS_LEAVE_Stmt
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN       lptkLeaveArg)        // Ptr to LEAVE arg

{
     TOKEN_TYPES TknType;               // Token type

    // Tell the lexical analyzer to get the next token from
    //   the stmt after the token pointed to by the LEAVE stmt
    CS_SetNextStmtToStmtAfter_NEXT (lpplLocalVars, &lptkLeaveArg->tkData);

    // If the starting and ending stmts are not on the same line, ...
    if (lptkLeaveArg->tkData.Next.uLineNum NE lpplLocalVars->uLineNum)
        // Tell the parser to stop executing this line
        lpplLocalVars->bStopExec = TRUE;

    // If we're not stopping execution, ...
    if (!lpplLocalVars->bStopExec)
        // We're restarting
        lpplLocalVars->bRestart = TRUE;

    // Get the token type of the stmt pointed to by the LEAVE stmt
    TknType = CS_GetTokenType_NEXT (lpplLocalVars, &lptkLeaveArg->tkData);

    // If the stmt pointed to by the LEAVE stmt is ENDFOR/ENDFORLCL, ...
    if (TknType EQ TKT_CS_ENDFOR
     || TknType EQ TKT_CS_ENDFORLCL)
        // We're done with this FOR/FORLCL stmt
        CS_DoneFOR (lpplLocalVars, TknType EQ TKT_CS_ENDFORLCL);

    return TRUE;
} // End CS_LEAVE_Stmt


//***************************************************************************
//  $CS_LEAVEIF_Stmt_EM
//
//  Process LEAVEIF stmt (pl_yyparse)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CS_LEAVEIF_Stmt_EM"
#else
#define APPEND_NAME
#endif

UBOOL CS_LEAVEIF_Stmt_EM
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN       lptkLeaveIfArg,      // Ptr to LEAVEIF arg
     LPPL_YYSTYPE  lpYYRhtArg)          // Ptr to right arg

{
    APLSTYPE   aplTypeRht;              // Right arg storage type
    APLNELM    aplNELMRht;              // Right arg NELM
    APLRANK    aplRankRht;              // Right arg rank
    APLLONGEST aplLongestRht;           // Right arg longest if immediate
    UBOOL      bRet;                    // TRUE iff the result is valid

    // The arg to the LEAVEIF stmt must be a Boolean-valued scalar or
    //   one-element vector.

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (&lpYYRhtArg->tkToken, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RANK_EXIT;

    // Check for LENGTH ERROR
    if (!IsSingleton (aplNELMRht))
        goto LENGTH_EXIT;

    // Get and validate the first item in a token
    aplLongestRht = ValidateFirstItemToken (aplTypeRht, &lpYYRhtArg->tkToken, &bRet);

    // Check for DOMAIN ERROR
    if (!bRet
     || !IsBooleanValue (aplLongestRht))
         goto DOMAIN_EXIT;

    if (aplLongestRht)
        CS_LEAVE_Stmt (lpplLocalVars, lptkLeaveIfArg);

    return TRUE;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                              &lpYYRhtArg->tkToken);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                              &lpYYRhtArg->tkToken);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYRhtArg->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    return FALSE;
} // End CS_LEAVEIF_Stmt
#undef  APPEND_NAME


//***************************************************************************
//  $CS_SELECT_Stmt_EM
//
//  Process SELECT stmt (pl_yyparse)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CS_SELECT_Stmt_EM"
#else
#define APPEND_NAME
#endif

UBOOL CS_SELECT_Stmt_EM
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN       lptkSelectArg,       // Ptr to SELECT arg
     LPPL_YYSTYPE  lpYYRhtArg)          // Ptr to right arg

{
    APLNELM        aplNELMCL;           // CASELIST NELM
    TOKEN_DATA     tdNxt,               // TOKEN_DATA of the next stmt
                   tdSOS;               // ...               SOS  ...
    TOKEN          tkNxt;               // TOKEN of next stmt
    LPTOKEN_HEADER lpMemTknHdr;         // Ptr to tokenized line header global memory
    HGLOBAL        hGlbTxtLine;         // Text of function line global memory handle
    LPPERTABDATA   lpMemPTD;            // Ptr to PerTabData global memory
    LPPL_YYSTYPE   lpYYTmp;             // Ptr to right arg
    UBOOL          bCmp;                // TRUE iff the comparison is TRUE
    UINT           offTknHdr;           // Offset from dfn header to tokenized line header

    // Get ptr to PerTabData global memory
    lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Copy the token data
    tkNxt.tkData = lptkSelectArg->tkData;

    // Run through the chain of SELECT/CASE/CASELIST stmts looking for a match
    while (TRUE)
    {
        // Copy the token data
        tdNxt = tkNxt.tkData;

        // Get the contents of the token pointed to by
        //   the SELECT/CASE/CASELIST token
        CS_GetTokenGlb_NEXT (lpplLocalVars, &tdNxt, &tkNxt, &lpMemTknHdr, &offTknHdr, &hGlbTxtLine);

        // Split cases based upon the token type
        switch (tkNxt.tkFlags.TknType)
        {
            EXIT_TYPES   exitType;
            HGLOBAL      hGlbDfnHdr;                // User-defined function/operator ...
            LPDFN_HEADER lpMemDfnHdr;               // Ptr to user-defined function/operator global memory

            case TKT_CS_CASE:
            case TKT_CS_CASELIST:
                // Copy token data to modify
                tdSOS = tkNxt.tkData;

                // Copy the .Orig struc to .Next
                CS_CopyOrigToNext (&tdSOS);

                // Point to the SOS token
                CS_PointToSOSToken_NEXT (lpplLocalVars, &tdSOS);

                // If the offset from dfn header to tokenized line header is valid, ...
                if (offTknHdr)
                {
                    // Get the corresponding user-defined function/operator global memory handle
                    hGlbDfnHdr = GetDfnHdrHandle (lpplLocalVars);

                    // Lock the memory to get a ptr to it
                    lpMemDfnHdr = MyGlobalLockDfn (hGlbDfnHdr);

                    // Get the given line's tokenized global memory handle
                    lpMemTknHdr = (LPTOKEN_HEADER) ByteAddr (lpMemDfnHdr, offTknHdr);
                } // End IF

                // Execute the stmt and compare the result with lpYYRhtArg

                // Fill the SIS struc, execute the line via ParseLine, and cleanup
                exitType =
                  PrimFnMonUpTackJotPLParse (lpMemPTD,              // Ptr to PerTabData global memory
                                             lpMemTknHdr,           // Ptr to tokenized line header global memory
                                             hGlbTxtLine,           // Text of function line global memory handle
                                             NULL,                  // Ptr to text of line to execute
                                             NULL,                  // Ptr to Semaphore handle to signal (NULL if none)
                                             tdSOS.Next.uLineNum,   // Function line #
                                             tdSOS.Next.uTknNum,    // Starting token # in the above function line
                                             TRUE,                  // TRUE iff we should act on errors
                                             TRUE,                  // TRUE iff executing only one stmt
                                             lpMemPTD->lpSISCur->DfnType,   // DfnType for FillSISNxt
                                             FALSE);                // TRUE iff we're to skip the depth check
                // If the offset from dfn header to tokenized line header is valid, ...
                if (offTknHdr)
                {
                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
                } // End IF

                // Split cases based upon the exit type
                switch (exitType)
                {
                    case EXITTYPE_NONE:
                    case EXITTYPE_DISPLAY:
                        // Match the SELECT arg against the CASE arg

                        // If it's CASE, ...
                        if (tkNxt.tkFlags.TknType EQ TKT_CS_CASE)
                        {
                            // Use match to determine equality
                            lpYYTmp =
                              PrimFnDydEqualUnderbar_EM_YY (&lpMemPTD->YYCaseExec.tkToken,  // Ptr to left arg token
                                                            &lpYYRhtArg->tkToken,           // Ptr to function token
                                                            &lpYYRhtArg->tkToken,           // Ptr to right arg token
                                                             NULL);                         // Ptr to axis token (may be NULL)
                            // Save the result of the comparison
                            bCmp = lpYYTmp->tkToken.tkData.tkBoolean;

                            // Free the temporary result
                            YYFree (lpYYTmp); lpYYTmp = NULL;
                        } else
                        // It's CASELIST
                        {
                            TOKEN tkCL = {0};       // CASELIST token for each element
                            UINT  uCnt;             // Loop counter

                            // Get the attributes (Type, NELM, and Rank)
                            //   of the CASELIST arg
                            AttrsOfToken (&lpMemPTD->YYCaseExec.tkToken, NULL, &aplNELMCL, NULL, NULL);

                            // Loop through the elements of the CASELIST arg
                            for (uCnt = 0, bCmp = FALSE; (!bCmp) && uCnt < aplNELMCL; uCnt++)
                            {
                                // Get the next value from the CASELIST result token into a token
                                GetNextValueTokenIntoToken (&lpMemPTD->YYCaseExec.tkToken,      // Ptr to the arg token
                                                             uCnt,                              // Index to use
                                                            &tkCL);                             // Ptr to the result token
                                // Use match to determine equality
                                lpYYTmp =
                                  PrimFnDydEqualUnderbar_EM_YY (&tkCL,                          // Ptr to left arg token
                                                                &lpYYRhtArg->tkToken,           // Ptr to function token
                                                                &lpYYRhtArg->tkToken,           // Ptr to right arg token
                                                                 NULL);                         // Ptr to axis token (may be NULL)
                                // Save the result of the comparison
                                bCmp = lpYYTmp->tkToken.tkData.tkBoolean;

                                // Free the temporary result
                                YYFree (lpYYTmp); lpYYTmp = NULL;
                            } // End FOR
                        } // End IF/ELSE

                        // Free the CASE/CASELIST result
                        FreeResult (&lpMemPTD->YYCaseExec); ZeroMemory (&lpMemPTD->YYCaseExec, sizeof (lpMemPTD->YYCaseExec));

                        // If the two are equal, ...
                        if (bCmp)
                        {
                            // Tell the lexical analyzer to get the next token from
                            //   the stmt after the token pointed to by the CASE stmt
                            CS_SetNextStmtToStmtAfter_NEXT (lpplLocalVars, &tdNxt);

                            return TRUE;
                        } // End IF

                        break;

                    case EXITTYPE_ERROR:
                    case EXITTYPE_STACK_FULL:
                        return FALSE;

                    case EXITTYPE_GOTO_ZILDE:
                    case EXITTYPE_GOTO_LINE:
                    case EXITTYPE_RESET_ONE:
                    case EXITTYPE_RESET_ONE_INIT:
                    case EXITTYPE_RESET_ALL:
                    case EXITTYPE_QUADERROR_INIT:
                    case EXITTYPE_QUADERROR_EXEC:
                    case EXITTYPE_STOP:
                    case EXITTYPE_NOVALUE:
                    case EXITTYPE_NODISPLAY:
                    defstop
                        return FALSE;
                } // End SWITCH

                break;

            case TKT_CS_ELSE:
            case TKT_CS_ENDSELECT:
                // Tell the lexical analyzer to get the next token from
                //   the stmt after the token pointed to by the ELSE stmt
                CS_SetNextStmtToStmtAfter_NEXT (lpplLocalVars, &tdNxt);

                // If we're not stopping execution, ...
                if (!lpplLocalVars->bStopExec)
                    // We're restarting
                    lpplLocalVars->bRestart = TRUE;
                return TRUE;

            defstop
                break;
        } // End SWITCH
    } // End WHILE

    return FALSE;
} // End CS_SELECT_Stmt_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CS_SKIPCASE_Stmt
//
//  Process SKIPCASE stmt (pl_yyparse)
//***************************************************************************

UBOOL CS_SKIPCASE_Stmt
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN       lptkSkipCaseArg)     // Ptr to SKIPCASE arg

{
    TOKEN      tkNxt;                   // Next token
    TOKEN_DATA tdNxt;                   // Next TOKEN_DATA

    // Get the contents of the (CASE/CASELIST) token pointed to by
    //   the SKIPCASE token
    CS_GetToken_NEXT (lpplLocalVars, &lptkSkipCaseArg->tkData, &tkNxt);

    Assert (tkNxt.tkFlags.TknType EQ TKT_CS_CASE
         || tkNxt.tkFlags.TknType EQ TKT_CS_CASELIST);

    do
    {
        // Get the TOKEN_DATA from the previous token
        tdNxt = tkNxt.tkData;

        // Get the contents of the token pointed to by
        //   the previous token
        CS_GetToken_NEXT (lpplLocalVars, &tdNxt, &tkNxt);
    } while (tkNxt.tkFlags.TknType EQ TKT_CS_CASE
          || tkNxt.tkFlags.TknType EQ TKT_CS_CASELIST
          || tkNxt.tkFlags.TknType EQ TKT_CS_ELSE);

    // The following commented out code is a bandaid covering the
    //   fact that the CS parsing code doesn't handle certain cases
    //   such as an :if ... :end stmt in a :case stmt.
////Assert (tkNxt.tkFlags.TknType EQ TKT_CS_ENDSELECT);

    // Tell the lexical analyzer to get the next token from
    //   the stmt after the token pointed to by the ENDSELECT stmt
    CS_SetNextStmtToStmtAfter_NEXT (lpplLocalVars, &tdNxt);

    // If the starting and ending stmts are not on the same line, ...
    if (tkNxt.tkData.Next.uLineNum NE lpplLocalVars->uLineNum)
        // Tell the parser to stop executing this line
        lpplLocalVars->bStopExec = TRUE;

    return TRUE;
} // End CS_SKIPCASE_Stmt


//***************************************************************************
//  $CS_SKIPEND_Stmt
//
//  Process SKIPEND stmt (pl_yyparse)
//***************************************************************************

UBOOL CS_SKIPEND_Stmt
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN       lptkSkipEndArg)      // Ptr to SKIPEND arg

{
    TOKEN      tkNxt;                   // Next token
    TOKEN_DATA tdNxt;                   // Next TOKEN_DATA

    // Get the contents of the (ELSEIF) token pointed to by
    //   the SKIPEND token
    CS_GetToken_NEXT (lpplLocalVars, &lptkSkipEndArg->tkData, &tkNxt);

    Assert (tkNxt.tkFlags.TknType EQ TKT_CS_ELSEIF);

    do
    {
        // Get the TOKEN_DATA from the previous token
        tdNxt = tkNxt.tkData;

        // Get the contents of the token pointed to by
        //   the previous token
        CS_GetToken_NEXT (lpplLocalVars, &tdNxt, &tkNxt);
    } while (tkNxt.tkFlags.TknType EQ TKT_CS_ANDIF
          || tkNxt.tkFlags.TknType EQ TKT_CS_ELSEIF
          || tkNxt.tkFlags.TknType EQ TKT_CS_ELSE
          || tkNxt.tkFlags.TknType EQ TKT_CS_ORIF);

    Assert (tkNxt.tkFlags.TknType EQ TKT_CS_ELSE
         || tkNxt.tkFlags.TknType EQ TKT_CS_ELSEIF
         || tkNxt.tkFlags.TknType EQ TKT_CS_ENDIF);

    // If the next token is ELSEIF, execute that stmt
    if (tkNxt.tkFlags.TknType EQ TKT_CS_ELSEIF)
        // Tell the lexical analyzer to get the next token from
        //   the stmt at the token pointed to by the ELSEIF stmt
        CS_SetNextStmtToStmtAt_NEXT (lpplLocalVars, &tdNxt);
    else
        // Tell the lexical analyzer to get the next token from
        //   the stmt after the token pointed to by the ELSE/ENDIF stmt
        CS_SetNextStmtToStmtAfter_NEXT (lpplLocalVars, &tdNxt);

    // If the starting and ending stmts are not on the same line, ...
    if (tkNxt.tkData.Next.uLineNum NE lpplLocalVars->uLineNum)
        // Tell the parser to stop executing this line
        lpplLocalVars->bStopExec = TRUE;

    return TRUE;
} // End CS_SKIPEND_Stmt


//***************************************************************************
//  $CS_GetToken_ORIG
//
//  Get the contents of a given token (pl_yyparse)
//***************************************************************************

void CS_GetToken_ORIG
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN_DATA  lptdArg,             // Ptr to arg TOKEN_DATA
     LPTOKEN       lptkRes)             // Ptr to result token

{
    // Call common routine with Orig.c arg
    CS_GetToken_COM (lpplLocalVars, &lptdArg->Orig.c, lptkRes, NULL, NULL, NULL);
} // End CS_GetToken_ORIG


//***************************************************************************
//  $CS_GetToken_NEXT
//
//  Get the contents of the token pointed to by
//    a given token (pl_yyparse)
//***************************************************************************

void CS_GetToken_NEXT
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN_DATA  lptdArg,             // Ptr to arg TOKEN_DATA
     LPTOKEN       lptkRes)             // Ptr to result token

{
    // Call common routine with Next arg
    CS_GetToken_COM (lpplLocalVars, &lptdArg->Next, lptkRes, NULL, NULL, NULL);
} // End CS_GetToken_NEXT


//***************************************************************************
//  $CS_GetTokenGlb_NEXT
//
//  Get the contents of the token pointed to by
//    a given token (pl_yyparse)
//***************************************************************************

void CS_GetTokenGlb_NEXT
    (LPPLLOCALVARS   lpplLocalVars,     // Ptr to PL local vars
     LPTOKEN_DATA    lptdArg,           // Ptr to arg TOKEN_DATA
     LPTOKEN         lptkRes,           // Ptr to result token
     LPTOKEN_HEADER *lplpMemTknHdr,     // Ptr to ptr to result tokenized line header global memory (may be NULL)
     LPUINT          lpOffTknHdr,       // Ptr to offset from dfn header to tokenized line header
                                        //   (may be zero if *lplpMemTknHdr is not NULL)
     HGLOBAL        *lphGlbTxtLine)     // Ptr to result text global memory handle

{
    // Call common routine with Next arg
    CS_GetToken_COM (lpplLocalVars, &lptdArg->Next, lptkRes, lplpMemTknHdr, lpOffTknHdr, lphGlbTxtLine);
} // End CS_GetTokenGlb_NEXT


//***************************************************************************
//  $CS_GetToken_COM
//
//  Get the contents of the token (pl_yyparse)
//***************************************************************************

void CS_GetToken_COM
    (LPPLLOCALVARS   lpplLocalVars,     // Ptr to PL local vars
     LPCLOCATION     lpLoc,             // Ptr to arg CLOCATION
     LPTOKEN         lptkRes,           // Ptr to result token
     LPTOKEN_HEADER *lplpMemTknHdr,     // Ptr to ptr to result tokenized line header global memory (may be NULL)
     LPUINT          lpOffTknHdr,       // Ptr to offset from dfn header to tokenized line header
                                        //   (may be zero if *lplpMemTknHdr is not NULL)
     HGLOBAL        *lphGlbTxtLine)     // Ptr to result text global memory handle

{
    LPDFN_HEADER   lpMemDfnHdr;         // Ptr to user-defined function/operator global memory
    LPFCNLINE      lpFcnLines;          // Ptr to array of function line structs (FCNLINE[numFcnLines])
    HGLOBAL        hGlbTknHdr = NULL,   // Tokenized line header global memory handle
                   hGlbTxtLine,         // Text line      ...
                   hGlbDfnHdr;          // User-defined function/operator ...
    LPTOKEN_HEADER lpMemTknHdr;         // Ptr to tokenized line header global memory
    LPTOKEN        lpMemTknLine;        // Ptr to tokenized line global memory
    UINT           offTknHdr;           // Offset from dfn header to tokenized line header

    Assert (lpLoc->uLineNum NE 0);

    // If the stmts are on the same line, ...
    if (lpLoc->uLineNum EQ lpplLocalVars->uLineNum)
    {
        // Get the token
        *lptkRes = lpplLocalVars->lptkStart[lpLoc->uTknNum];

        // Copy the ptr to the tokenized line's header global memory
        lpMemTknHdr = lpplLocalVars->lpMemTknHdr;

        // If the caller wants the offset to the tokenized line header, ...
        if (lpOffTknHdr)
            // Zap the offset as we have a legit lpMemTknHdr
            *lpOffTknHdr = 0;

        // Copy the text line's global memory handle
        hGlbTxtLine = lpplLocalVars->hGlbTxtLine;
    } else
    {
        // Get the corresponding user-defined function/operator global memory handle
        hGlbDfnHdr = GetDfnHdrHandle (lpplLocalVars);

        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLockDfn (hGlbDfnHdr);

        // Get ptr to array of function line structs (FCNLINE[numFcnLines])
        lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

        Assert (((SHORT) lpLoc->uLineNum) > 0);

        // Save the offset to the tokenized line header
        offTknHdr = lpFcnLines[lpLoc->uLineNum - 1].offTknLine;

        // If the caller wants the offset to the tokenized line header, ...
        if (lpOffTknHdr)
            *lpOffTknHdr = offTknHdr;

        // Get the given line's tokenized global memory handle
        lpMemTknHdr = (LPTOKEN_HEADER) ByteAddr (lpMemDfnHdr, offTknHdr);

        // Copy the text line's global memory handle
        hGlbTxtLine = lpFcnLines[lpLoc->uLineNum - 1].hGlbTxtLine;

        // Skip over the header to the data
        lpMemTknLine = TokenBaseToStart (lpMemTknHdr);

        // Get the given token
        *lptkRes = lpMemTknLine[lpLoc->uTknNum];
    } // End IF/ELSE

    // If the caller wants the tokenized line's global memory handle, ...
    if (lplpMemTknHdr)
        *lplpMemTknHdr = lpMemTknHdr;

    // If the caller wants the text line's global memory handle, ...
    if (lphGlbTxtLine)
        *lphGlbTxtLine = hGlbTxtLine;

    // If the stmts are NOT on the same line, ...
    if (lpLoc->uLineNum NE lpplLocalVars->uLineNum)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
    } // End IF
} // End CS_GetToken_COM


//***************************************************************************
//  $GetDfnHdrHandle
//
//  Get the corresponding user-defined function/operator global memory handle
//***************************************************************************

HGLOBAL GetDfnHdrHandle
    (LPPLLOCALVARS lpplLocalVars)       // Ptr to PL local vars

{
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPSIS_HEADER lpSISCur;              // Ptr to current SIS header

    // If the one in lpplLocalVars is valid, use that
    if (lpplLocalVars->hGlbDfnHdr)
        return lpplLocalVars->hGlbDfnHdr;

    // Get ptr to PerTabData global memory
    lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Copy ptr to current SI level
    lpSISCur = lpMemPTD->lpSISCur;

    // Peel back to non-ImmExec, non-Execute, non-Quad layer
    while (lpSISCur
        && (lpSISCur->DfnType EQ DFNTYPE_IMM
         || lpSISCur->DfnType EQ DFNTYPE_QUAD
         || lpSISCur->DfnType EQ DFNTYPE_EXEC))
        lpSISCur = lpSISCur->lpSISPrv;

    return lpSISCur->hGlbDfnHdr;
} // End GetDfnHdrHandle


//***************************************************************************
//  $CS_CopyOrigToNext
//
//  Copy the .Orig struc to the .Next struc
//***************************************************************************

void CS_CopyOrigToNext
    (LPTOKEN_DATA lptdArg)              // Ptr to arg TOKEN_DATA

{
    // Copy original values so we can change them
    lptdArg->Next.uLineNum = lptdArg->Orig.c.uLineNum;
    lptdArg->Next.uStmtNum = lptdArg->Orig.c.uStmtNum;
    lptdArg->Next.uTknNum  = lptdArg->Orig.c.uTknNum;
} // End CS_CopyOrigToNext


//***************************************************************************
//  $CS_PointToSOSToken_NEXT
//
//  Set the next token # of the SOS token of the stmt which contains
//    a given token.
//***************************************************************************

void CS_PointToSOSToken_NEXT
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN_DATA  lptdArg)             // Ptr to arg TOKEN_DATA

{
    Assert (lptdArg->Next.uTknNum NE 0);

    // The Arg token is at the start of the stmt, so backing up
    //   by one points to the preceding EOS/EOL stmt which contains the
    //   stmt length.
    lptdArg->Next.uTknNum--;

    // Skip to the end of the Arg1 stmt less one to address the SOS token
    lptdArg->Next.uTknNum += CS_GetEOSTokenLen_NEXT (lpplLocalVars, lptdArg) - 1;

    Assert (CS_GetTokenType_NEXT (lpplLocalVars, lptdArg) EQ TKT_SOS);
} // End CS_PointToSOSToken_NEXT


//// //***************************************************************************
//// //  $CS_SetNextStmtToStmtAt_ORIG
//// //
//// //  Set the next token to the beginning of the stmt
//// //    at the given token (pl_yyparse)
//// //***************************************************************************
////
//// void CS_SetNextStmtToStmtAt_ORIG
////     (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
////      LPTOKEN_DATA  lptdArg)             // Ptr to arg TOKEN_DATA
////
//// {
////     TOKEN_DATA tdArg;                   // Next TOKEN_DATA
////
////     Assert (lptdArg->Orig.c.uLineNum NE 0);
////
////     // Copy the token contents to modify
////     tdArg = *lptdArg;
////
////     // Copy the .Orig struc to .Next
////     CS_CopyOrigToNext (&tdArg);
////
////     // Point to the SOS token
////     CS_PointToSOSToken_NEXT (lpplLocalVars, &tdArg);
////
////     // Tell the lexical analyzer to get the next token from
////     //   the preceding EOS token
////     CS_SetNextToken_NEXT (lpplLocalVars, &tdArg);
//// } // End CS_SetNextStmtToStmtAt_ORIG


//***************************************************************************
//  $CS_SetNextStmtToStmtAt_NEXT
//
//  Set the next token to the beginning of the stmt
//    at the token pointed to by the given token (pl_yyparse)
//***************************************************************************

void CS_SetNextStmtToStmtAt_NEXT
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN_DATA  lptdArg)             // Ptr to arg TOKEN_DATA

{
    TOKEN_DATA tdArg;                   // Next TOKEN_DATA

    Assert (lptdArg->Next.uLineNum NE 0);

    // Copy the token contents to modify
    tdArg = *lptdArg;

    // Point to the SOS token
    CS_PointToSOSToken_NEXT (lpplLocalVars, &tdArg);

    // Tell the lexical analyzer to get the next token from
    //   the preceding EOS token
    CS_SetNextToken_NEXT (lpplLocalVars, &tdArg);
} // End CS_SetNextStmtToStmtAt_NEXT


//***************************************************************************
//  $CS_SetNextStmtToStmtAfter_ORIG
//
//  Set the next token to the stmt after
//    a given token (pl_yyparse)
//***************************************************************************

void CS_SetNextStmtToStmtAfter_ORIG
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN_DATA  lptdArg)             // Ptr to arg TOKEN_DATA

{
    TOKEN       tkCur;                  // Current TOKEN
    TOKEN_DATA  tdCur;                  // Current TOKEN_DATA

    Assert (lptdArg->Orig.c.uLineNum NE 0);

    // The lptdArg token is at the start of the stmt, i.e.
    //   one token after EOS/EOL/LABELSEP.

    // Copy the target address (Line & Token #s)
    tdCur = *lptdArg;

    // Back up to the preceding EOS/EOL/LABELSEP
    Assert (lptdArg->Orig.c.uTknNum NE 0);
    tdCur.Orig.d.uTknNum--;

#ifdef DEBUG
    {
        TOKEN_TYPES TknType;

        // Get the token type of the stmt pointed to by the tdCur stmt
        TknType = CS_GetTokenType_ORIG (lpplLocalVars, &tdCur);
        Assert (TknType EQ TKT_EOS
             || TknType EQ TKT_EOL
             || TknType EQ TKT_LABELSEP);
    }
#endif

    // Get the contents of the token tdCur
    CS_GetToken_ORIG (lpplLocalVars, &tdCur, &tkCur);

    // Split cases based upon the pointed to token type
    switch (tkCur.tkFlags.TknType)
    {
        case TKT_EOL:
        case TKT_EOS:
            break;

        case TKT_LABELSEP:
            // Back up to the preceding EOS/EOL
            tdCur.Orig.d.uTknNum -= 2;

            // Get the contents of the token tdCur
            CS_GetToken_ORIG (lpplLocalVars, &tdCur, &tkCur);

            break;

        defstop
            break;
    } // End SWITCH

    // Split cases based upon the token type
    switch (tkCur.tkFlags.TknType)
    {
        case TKT_EOL:
            // Skip to the next line #, token #0
            tdCur.Orig.d.uLineNum++;
            tdCur.Orig.d.uTknNum = 0;

            break;

        case TKT_EOS:
            // Skip over this stmt
            tdCur.Orig.d.uTknNum += tkCur.tkData.tkIndex;

            // Get the contents of the token tdCur
            CS_GetToken_ORIG (lpplLocalVars, &tdCur, &tkCur);

            // Skip over this stmt, backing off to the SOS
            tdCur.Orig.d.uTknNum += tkCur.tkData.tkIndex - 1;

            // We're restarting
            lpplLocalVars->bRestart = TRUE;

            break;

        defstop
            break;
    } // End SWITCH

    // Tell the lexical analyzer to get the next token from tdCur
    CS_SetNextToken_ORIG (lpplLocalVars, &tdCur);
} // End CS_SetNextStmtToStmtAfter_ORIG


//***************************************************************************
//  $CS_SetNextStmtToStmtAfter_NEXT
//
//  Set the next token to the stmt after
//    where a given token points (pl_yyparse)
//***************************************************************************

void CS_SetNextStmtToStmtAfter_NEXT
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN_DATA  lptdArg)             // Ptr to arg TOKEN_DATA

{
    TOKEN       tkNxt;                  // Next TOKEN
    TOKEN_DATA  tdNxt;                  // Next TOKEN_DATA

    Assert (lptdArg->Next.uLineNum NE 0);

    // The lptdArg token is at the start of the stmt, i.e.
    //   one token after EOS/EOL/LABELSEP.

    // Copy the target address (Line & Token #s)
    tdNxt = *lptdArg;

    // Back up to the preceding EOS/EOL/LABELSEP
    Assert (lptdArg->Next.uTknNum NE 0);
    tdNxt.Next.uTknNum--;

#ifdef DEBUG
    {
        TOKEN_TYPES TknType;

        // Get the token type of the stmt pointed to by the tdNxt stmt
        TknType = CS_GetTokenType_NEXT (lpplLocalVars, &tdNxt);
        Assert (TknType EQ TKT_EOS
             || TknType EQ TKT_EOL
             || TknType EQ TKT_LABELSEP);
    }
#endif

    // Get the contents of the token pointed to by tdNxt
    CS_GetToken_NEXT (lpplLocalVars, &tdNxt, &tkNxt);

    // Split cases based upon the pointed to token type
    switch (tkNxt.tkFlags.TknType)
    {
        case TKT_EOL:
        case TKT_EOS:
            break;

        case TKT_LABELSEP:
            // Back up to the preceding EOS/EOL
            tdNxt.Next.uTknNum -= 2;

            // Get the contents of the token pointed to by tdNxt
            CS_GetToken_NEXT (lpplLocalVars, &tdNxt, &tkNxt);

            break;

        defstop
            break;
    } // End SWITCH

    // Split cases based upon the token type
    switch (tkNxt.tkFlags.TknType)
    {
        case TKT_EOL:
            // Skip to the next line #, token #0
            tdNxt.Next.uLineNum++;
            tdNxt.Next.uTknNum = 0;

            break;

        case TKT_EOS:
            // Skip over this stmt
            tdNxt.Next.uTknNum += tkNxt.tkData.tkIndex;

            // Get the contents of the token pointed to by tdNxt
            CS_GetToken_NEXT (lpplLocalVars, &tdNxt, &tkNxt);

            // Skip over this stmt, backing off to the SOS
            tdNxt.Next.uTknNum += tkNxt.tkData.tkIndex - 1;

            // We're restarting
            lpplLocalVars->bRestart = TRUE;

            break;

        defstop
            break;
    } // End SWITCH

    // Tell the lexical analyzer to get the next token from
    //   the stmt pointed to by tdNxt
    CS_SetNextToken_NEXT (lpplLocalVars, &tdNxt);
} // End CS_SetNextStmtToStmtAfter_NEXT


//***************************************************************************
//  $CS_SetNextToken_ORIG
//
//  Set the next token to a given token (pl_yyparse)
//***************************************************************************

void CS_SetNextToken_ORIG
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN_DATA  lptdArg)             // Ptr to arg TOKEN_DATA

{
    // Call common routine with Orig.c arg
    CS_SetNextToken_COM (lpplLocalVars, &lptdArg->Orig.c);
} // End CS_SetNextToken_ORIG


//***************************************************************************
//  $CS_SetNextToken_NEXT
//
//  Set the next token to where a given token points (pl_yyparse)
//***************************************************************************

void CS_SetNextToken_NEXT
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN_DATA  lptdArg)             // Ptr to arg TOKEN_DATA

{
    // Call common routine with Next arg
    CS_SetNextToken_COM (lpplLocalVars, &lptdArg->Next);
} // End CS_SetNextToken_NEXT


//***************************************************************************
//  $CS_SetNextToken_COM
//
//  Set the next token (pl_yyparse)
//***************************************************************************

void CS_SetNextToken_COM
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPCLOCATION   lpLoc)               // Ptr to arg CLOCATION

{
    Assert (lpLoc->uLineNum NE 0);

    // If the stmts are on the same line, ...
    if (lpLoc->uLineNum EQ lpplLocalVars->uLineNum)
        // Set the next token to parse
        lpplLocalVars->lptkNext =
          &lpplLocalVars->lptkStart[lpLoc->uTknNum];
    else
    {
        LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory

        // Get ptr to PerTabData global memory
        lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

        // Save as the next line & token #
        lpMemPTD->lpSISCur->NxtLineNum = lpLoc->uLineNum;
        lpMemPTD->lpSISCur->NxtTknNum  = lpLoc->uTknNum;

        // Tell the parser to stop executing this line
        lpplLocalVars->bStopExec = TRUE;
    } // End IF/ELSE
} // End CS_SetNextToken_COM


//***************************************************************************
//  $CS_GetTokenType_ORIG
//
//  Return the token type of a given token identified by line and token #
//    (pl_yyparse)
//***************************************************************************

TOKEN_TYPES CS_GetTokenType_ORIG
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN_DATA  lptdArg)             // Ptr to arg TOKEN_DATA

{
    TOKEN tkNxt;                        // Next token

    // Get the contents of the token lptdArg
    CS_GetToken_ORIG (lpplLocalVars, lptdArg, &tkNxt);

    return tkNxt.tkFlags.TknType;
} // End CS_GetTokenType_ORIG


//***************************************************************************
//  $CS_GetTokenType_NEXT
//
//  Return the token type of the token pointed to by a given token
//    identified by line and token #
//    (pl_yyparse)
//***************************************************************************

TOKEN_TYPES CS_GetTokenType_NEXT
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN_DATA  lptdArg)             // Ptr to arg TOKEN_DATA

{
    TOKEN tkNxt;                        // Next token

    Assert (lptdArg->Next.uLineNum NE 0);

    // Get the contents of the token pointed to by lptdArg
    CS_GetToken_NEXT (lpplLocalVars, lptdArg, &tkNxt);

    return tkNxt.tkFlags.TknType;
} // End CS_GetTokenType_NEXT


//***************************************************************************
//  $CS_GetEOSTokenLen_NEXT
//
//  Return the length of the EOS token which precedes a given token
//    (pl_yyparse)
//***************************************************************************

UINT CS_GetEOSTokenLen_NEXT
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN_DATA  lptdArg)             // Ptr to arg TOKEN_DATA

{
    TOKEN tkNxt;                        // Next token

    Assert (lptdArg->Next.uLineNum NE 0);

    // Get the contents of the token
    CS_GetToken_NEXT (lpplLocalVars, lptdArg, &tkNxt);

    Assert (tkNxt.tkFlags.TknType EQ TKT_EOS
         || tkNxt.tkFlags.TknType EQ TKT_EOL);

    return tkNxt.tkData.tkIndex;
} // End CS_GetEOSTokenLen_NEXT


//***************************************************************************
//  End of File: cs_proc.c
//***************************************************************************
