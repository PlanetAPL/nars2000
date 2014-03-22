//***************************************************************************
//  NARS2000 -- Goto Functions
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
#include <limits.h>
#include "headers.h"


//***************************************************************************
//  $GotoLine_EM
//
//  Handle {goto} LineNum
//
//  Return EXITTYPE_GOTO_ZILDE if we're going to {zilde}
//         EXITTYPE_GOTO_LINE  if we're going to a valid line num
//         EXITTYPE_ERROR if error
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- GotoLine_EM"
#else
#define APPEND_NAME
#endif

EXIT_TYPES GotoLine_EM
    (LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkFunc)              // Ptr to function token

{
    LPPERTABDATA   lpMemPTD;        // Ptr to PerTabData global memory
    EXIT_TYPES     exitType;        // Return value
    APLSTYPE       aplTypeRht;      // Right arg storage type
    APLNELM        aplNELMRht;      // ...       NELM
    APLRANK        aplRankRht;      // ...       rank
    IMM_TYPES      immType;         // Right arg first value immediate type
    APLINT         aplIntegerRht;   // First value as integer
    APLFLOAT       aplFloatRht;     // ...            float
    LPSIS_HEADER   lpSISCur;        // Ptr to current SIS header
    TOKEN_TYPES    TknType;         // Target token type
    LPTOKEN_HEADER lpMemTknHdr;     // Ptr to tokenized line header
    LPTOKEN        lpMemTknLine;    // Ptr to tokenized line global memory
    LPPLLOCALVARS  lpplLocalVars;   // Ptr to PL local vars
    TOKEN          tkNxt;           // Token of next stmt
    UINT           uTknNum = 0;     // Starting token #
    UBOOL          bExecEC,         // TRUE iff we're executing under []EC
                   bRet;            // TRUE iff the result is valid
    HGLOBAL        lpSymGlb;        // Ptr to global numeric value
    LPDFN_HEADER   lpMemDfnHdr;     // Ptr to user-defined function/operator header ...

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RANK_EXIT;

    // If the right arg is not empty, ...
    if (!IsEmpty (aplNELMRht))
    {
        // Get the first value
        GetFirstValueToken (lptkRhtArg,     // Ptr to right arg token
                           &aplIntegerRht,  // Ptr to integer result
                           &aplFloatRht,    // Ptr to float ...
                            NULL,           // Ptr to WCHAR ...
                            NULL,           // Ptr to longest ...
                           &lpSymGlb,       // Ptr to lpSym/Glb ...
                           &immType,        // Ptr to ...immediate type ...
                            NULL);          // Ptr to array type ...
        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
                break;

            case ARRAY_HETERO:
            case ARRAY_NESTED:
                // Split cases based upon the immediate storage type
                switch (immType)
                {
                    case IMMTYPE_BOOL:
                    case IMMTYPE_INT:
                        break;

                    case IMMTYPE_RAT:
                        Assert (GetPtrTypeDir (lpSymGlb) EQ PTRTYPE_HGLOBAL);

                        // Attempt to convert the RAT to an integer using System []CT
                        aplIntegerRht = GetNextRatIntGlb (lpSymGlb, 0, &bRet);
                        if (!bRet)
                            goto DOMAIN_EXIT;
                        break;

                    case IMMTYPE_VFP:
                        Assert (GetPtrTypeDir (lpSymGlb) EQ PTRTYPE_HGLOBAL);

                        // Attempt to convert the VFP to an integer using System []CT
                        aplIntegerRht = GetNextVfpIntGlb (lpSymGlb, 0, &bRet);
                        if (!bRet)
                            goto DOMAIN_EXIT;
                        break;

                    case IMMTYPE_FLOAT:
                        // Attempt to convert the float to an integer using System []CT
                        aplIntegerRht = FloatToAplint_SCT (aplFloatRht, &bRet);
                        if (!bRet)
                            goto DOMAIN_EXIT;
                        break;

                    case IMMTYPE_CHAR:
                    case IMMTYPE_ERROR:
                        goto DOMAIN_EXIT;

                    defstop
                        break;
                } // End SWITCH

                break;

            case ARRAY_FLOAT:
                // Attempt to convert the float to an integer using System []CT
                aplIntegerRht = FloatToAplint_SCT (aplFloatRht, &bRet);
                if (!bRet)
                    goto DOMAIN_EXIT;
                break;

            case ARRAY_RAT:
                // Attempt to convert the RAT to an integer using System []CT
                aplIntegerRht = mpq_get_sctsx ((LPAPLRAT) lpSymGlb, &bRet);
                if (!bRet)
                    goto DOMAIN_EXIT;
                break;

            case ARRAY_VFP:
                // Attempt to convert the VFP to an integer using System []CT
                aplIntegerRht = mpfr_get_sctsx ((LPAPLVFP) lpSymGlb, &bRet);
                if (!bRet)
                    goto DOMAIN_EXIT;
                break;

            case ARRAY_CHAR:
                goto DOMAIN_EXIT;

            defstop
                break;
        } // End SWITCH
    } // End IF

    // Copy ptr to current SI level
    lpSISCur = lpMemPTD->lpSISCur;

    // Check for not restartable SI level
    if (lpSISCur->DfnType EQ DFNTYPE_IMM    // This level is Immediate
     && lpSISCur->lpSISPrv                  // There is a previous level
     && !lpSISCur->lpSISPrv->Restartable)   // and it's not restartable
        goto NORESTART_EXIT;

    // Save the {goto} target if we're executing under []EC
    bExecEC = SaveGotoTarget (lpMemPTD, lptkRhtArg);

    // If the right arg is empty, ...
    if (IsEmpty (aplNELMRht))
    {
        exitType = EXITTYPE_GOTO_ZILDE;

        goto NORMAL_EXIT;
    } // End IF

    // Ensure line # is within range
    if (aplIntegerRht < 0
     || aplIntegerRht > UINT_MAX)
        aplIntegerRht = -1;

    // Peel back to non-ImmExec, non-Execute, non-Quad layer
    while (lpSISCur
        && (lpSISCur->DfnType EQ DFNTYPE_IMM
         || lpSISCur->DfnType EQ DFNTYPE_QUAD
         || lpSISCur->DfnType EQ DFNTYPE_EXEC))
        lpSISCur = lpSISCur->lpSISPrv;

    // If we're at a UDFO layer, ...
    if (lpSISCur NE NULL)
        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLock (lpSISCur->hGlbDfnHdr);

    if (lpSISCur NE NULL
     && !bExecEC
     && aplIntegerRht > 0
     && aplIntegerRht <= lpMemDfnHdr->numFcnLines)
    {
        UINT         uStmtLen,          // Statement length (in TOKENs)
                     TokenCnt;          // Token count in this line
        LPFCNLINE    lpFcnLines;        // Ptr to array of function line structs (FCNLINE[numFcnLines])

        // Get ptr to array of function line structs (FCNLINE[numFcnLines])
        lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

        // Get a ptr to the corresponding tokenized line's global memory
        lpMemTknHdr = (LPTOKEN_HEADER) ByteAddr (lpMemDfnHdr, lpFcnLines[aplIntegerRht - 1].offTknLine);

        // Get the token count
        TokenCnt = lpMemTknHdr->TokenCnt;

        // Skip over the header to the data
        lpMemTknLine = TokenBaseToStart (lpMemTknHdr);

        Assert (lpMemTknLine->tkFlags.TknType EQ TKT_EOS
             || lpMemTknLine->tkFlags.TknType EQ TKT_EOL);
        // Get the EOS/EOL token length and skip over it
        uStmtLen = lpMemTknLine->tkData.tkIndex;
        lpMemTknLine++; TokenCnt--;

        // Check for and skip over line label
        if (TokenCnt > 1
         && lpMemTknLine[1].tkFlags.TknType EQ TKT_LABELSEP)
        {
            lpMemTknLine += 2; TokenCnt -= 2;
        } // End IF

        // Get the next token and its type
        tkNxt = *lpMemTknLine;
        TknType = tkNxt.tkFlags.TknType;

        // Check for branch to non-restartable Control Structure tokens:
        //   CASE, CASELIST (both covered by SKIPCASE)
        if (TknType EQ TKT_CS_SKIPCASE)
            goto DESTIN_EXIT;

        // Get a ptr to the PL local vars
        lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

        // If the target is an ELSE stmt, skip to the next stmt
        //   and execute that one
        if (TknType EQ TKT_CS_ELSE)
            // Skip over the ELSE stmt
            uTknNum = uStmtLen;
        else
        // If the target is a FOR/FORLCL stmt, ensure that the identifier
        //  on the FORSTMT stack is the same as the one in the EndFor
        //  token.
        if (TknType EQ TKT_CS_ENDFOR
         || TknType EQ TKT_CS_ENDFORLCL)
        {
            // If there's no previous entry on the FORSTMT stack, ...
            if (lpSISCur EQ NULL
             || lpSISCur->lpForStmtBase EQ lpSISCur->lpForStmtNext)
                goto DESTIN_EXIT;

            // Compare the current FOR/FORLCL stmt identifier from the FORSTMT stack
            //   with the CLIndex in the ENDFOR/ENDFORLCL stmt -- if they are unequal,
            //   then this ENDFOR/ENDFORLCL stmt is not restartable (we're starting in
            //   the middle of a FOR/FORLCL stmt which has not been initialized
            //   or is out of nested sequence).
            if (lpSISCur->lpForStmtNext[-1].uForStmtID
             NE tkNxt.tkData.uCLIndex)
                goto DESTIN_EXIT;
        } // End IF/ELSE/IF
    } // End IF

    // If we're at a UDFO layer, ...
    if (lpSISCur NE NULL)
        // We no longer need this ptr
        MyGlobalUnlock (lpSISCur->hGlbDfnHdr); lpMemDfnHdr = NULL;

    // Save the exit type
    exitType = EXITTYPE_GOTO_LINE;

    // If we're not executing under []EC, ...
    if (!bExecEC)
    // Split cases based upon the function type
    switch (lpMemPTD->lpSISCur->DfnType)
    {
        case DFNTYPE_IMM:       // Restart execution in a suspended function
            // If there's a suspended function, ...
            if (lpMemPTD->lpSISCur->lpSISPrv)
            {
                // Save as the next line & token #s
                lpMemPTD->lpSISCur->lpSISPrv->NxtLineNum = (UINT) aplIntegerRht;
                lpMemPTD->lpSISCur->lpSISPrv->NxtTknNum  = uTknNum;

                // Mark as no longer suspended
                lpMemPTD->lpSISCur->lpSISPrv->Suspended = FALSE;
            } // End IF

            break;

        case DFNTYPE_OP1:
        case DFNTYPE_OP2:
        case DFNTYPE_FCN:       // Jump to a new line #
            // If there's a suspended function, ...
            if (lpMemPTD->lpSISCur)
            {
                // Save as the next line & token #s
                lpMemPTD->lpSISCur->NxtLineNum = (UINT) aplIntegerRht;
                lpMemPTD->lpSISCur->NxtTknNum  = uTknNum;

                // Mark as no longer suspended
                lpMemPTD->lpSISCur->Suspended = FALSE;
            } // End IF

            break;

        case DFNTYPE_EXEC:
        case DFNTYPE_QUAD:
            // Peel back to the first non-Imm/Exec layer
            //   starting with the previous SIS header
            lpSISCur = GetSISLayer (lpMemPTD->lpSISCur->lpSISPrv);

            // If there's a suspended user-defined function/operator, ...
            if (lpSISCur
             && (lpSISCur->DfnType EQ DFNTYPE_OP1
              || lpSISCur->DfnType EQ DFNTYPE_OP2
              || lpSISCur->DfnType EQ DFNTYPE_FCN))
            {
                // Save as the next line & token #s
                lpSISCur->NxtLineNum = (UINT) aplIntegerRht;
                lpSISCur->NxtTknNum  = uTknNum;

                // Mark as no longer suspended
                lpSISCur->Suspended = FALSE;

                // Save the suspended function's semaphore as the one to signal
                lpMemPTD->lpSISCur->hSigaphore = lpSISCur->hSemaphore;
            } // End IF

            break;

        defstop
            break;
    } // End IF/SWITCH

    goto NORMAL_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

NORESTART_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NOTRESTARTABLE APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

DESTIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DESTINATION_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    // Mark as in error
    exitType = EXITTYPE_ERROR;
NORMAL_EXIT:
    return exitType;
} // End GotoLine_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SaveGotoTarget
//
//  Save the {goto} target if we're executing under []EC
//***************************************************************************

UBOOL SaveGotoTarget
    (LPPERTABDATA lpMemPTD,     // Ptr to PerTabData global memory
     LPTOKEN      lptkGoto)     // Ptr to {goto} target token

{
    LPSIS_HEADER lpSISCur;          // Ptr to current SIS header

    // Get ptr to current SIS header
    lpSISCur = lpMemPTD->lpSISCur;

    while (lpSISCur
        && lpSISCur->DfnType NE DFNTYPE_FCN
        && lpSISCur->DfnType NE DFNTYPE_OP1
        && lpSISCur->DfnType NE DFNTYPE_OP2
        && lpSISCur->DfnType NE DFNTYPE_ERRCTRL)
        lpSISCur = lpSISCur->lpSISPrv;

    // If there's an SIS level, ...
    //    and we're executing under []EA/[]EC, ...
    if (lpSISCur
     && lpSISCur->DfnType EQ DFNTYPE_ERRCTRL)
    {
        // If we're executing under []EC but not []EA, ...
        if (lpSISCur->ItsEC)
        // Split cases based upon the token type
        switch (lptkGoto->tkFlags.TknType)
        {
            case TKT_VARIMMED:
                // Copy the {goto} target as an LPSYMENTRY
                lpSISCur->lpSymGlbGoto =
                  MakeSymEntry_EM (lptkGoto->tkFlags.ImmType,   // Immediate type
                                  &lptkGoto->tkData.tkLongest,  // Ptr to immediate value
                                   lptkGoto);                   // Ptr to function token
                break;

            case TKT_VARARRAY:
                // Copy the {goto} target as an HGLOBAL
                lpSISCur->lpSymGlbGoto =
                  CopySymGlbDir_PTB (lptkGoto->tkData.tkGlbData);
                break;

            defstop
                break;
        } // End IF/SWITCH

        // Mark as executing under []EA/[]EC
        return TRUE;
    } // End IF

    // Mark as not executing under []EA/[]EC
    return FALSE;
} // End SaveGotoTarget


//***************************************************************************
//  End of File: goto.c
//***************************************************************************
