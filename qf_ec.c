//***************************************************************************
//  NARS2000 -- System Function -- Quad EC
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
#include "qf_ec.h"


//***************************************************************************
//  $SysFnEC_EM_YY
//
//  System function:  []EC -- Execute Controlled
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnEC_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnEC_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token  (should be NULL)
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
        return SysFnMonEC_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydEC_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnEC_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonEC_EM_YY
//
//  Monadic []EC -- Execute Controlled
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonEC_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonEC_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeRht;        // Right arg storage type
    APLRANK      aplRankRht;        // Right arg rank
    APLNELM      aplNELMRht;        // Right arg NELM
    HGLOBAL      hGlbRht = NULL,    // Right arg global memory handle
                 hGlbRes = NULL,    // Result    ...
                 hGlbQuadDM,        // []DM      ...
                 hGlbTmp;           // Temporary ...
    LPAPLCHAR    lpMemRht = NULL;   // Ptr to right arg global memory
    LPAPLNESTED  lpMemRes = NULL;   // Ptr to result    ...
    APLLONGEST   aplLongestRht,     // Right arg immediate value
                 aplLongestRC;      // Return code
    LPPL_YYSTYPE lpYYRes = NULL,    // Ptr to result
                 lpYYRes2 = NULL;   // Ptr to secondary result
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    APLUINT      ByteRes;           // # bytes in the result
#ifdef DEBUG
    EC_RETCODES  retCode;           // Return code for []EC[0]
#endif
    EXIT_TYPES   exitType;          // Exit type (see EXIT_TYPES)

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Save the current value of []DM
    hGlbQuadDM = lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_DM]->stData.stGlbData;

    // Set it to a known (permanent) value
    lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_DM]->stData.stGlbData = MakePtrTypeGlb (hGlbV0Char);

    // Create an SIS layer to stop unwinding through this level
    // Fill in the SIS header for Quad-EC
    FillSISNxt (lpMemPTD,               // Ptr to PerTabData global memory
                NULL,                   // Semaphore handle
                DFNTYPE_ERRCTRL,        // DfnType
                FCNVALENCE_MON,         // FcnValence
                FALSE,                  // Suspended
                TRUE,                   // Restartable
                TRUE);                  // LinkIntoChain
    // Fill in the non-default SIS header entries
    lpMemPTD->lpSISCur->ItsEC = TRUE;

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RIGHT RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Check for RIGHT DOMAIN ERROR
    if (!IsSimpleChar (aplTypeRht))
        goto RIGHT_DOMAIN_EXIT;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_NESTED, 3, 1);

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
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_NESTED;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = 3;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = (LPAPLNESTED) VarArrayBaseToDim (lpMemRes);

    // Save the dimension
    *((LPAPLDIM) lpMemRes)++ = 3;

    // lpMemRes now points to its data

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // Get the right arg's global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // If it's global, ...
    if (hGlbRht)
        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);
    else
        // Point to the immediate value
        lpMemRht = (LPAPLCHAR) &aplLongestRht;

    // Execute the right arg
    lpYYRes2 =
      PrimFnMonUpTackJotCommon_EM_YY (lpMemRht,     // Ptr to text of line to execute
                                      aplNELMRht,   // Length of the line to execute
                                      FALSE,        // TRUE iff we should free lpwszCompLine
                                      TRUE,         // TRUE iff we should return a NoValue YYRes
                                      FALSE,        // TRUE iff we should act on errors
                                     &exitType,     // Ptr to return EXITTYPE_xxx (may be NULL)
                                      lptkFunc);    // Ptr to function token
    // Get the return code
#ifdef DEBUG
    retCode =
#endif
    aplLongestRC = TranslateExitTypeToReturnCode (exitType);

    // Create []DM & []EM
    ErrorMessageDirect (lpMemPTD->lpwszErrorMessage,    // Ptr to error message text
                        lpMemRht,                       // Ptr to the line which generated the error
                        lpMemPTD->tkErrorCharIndex);    // Position of caret (origin-0)
    // Save the return code
    lpMemRes[0] =
      MakeSymEntry_EM (IMMTYPE_INT,     // Immediate type
                      &aplLongestRC,    // Ptr to immediate value
                       lptkFunc);       // Ptr to function token
    // Allocate space for []ET
    hGlbTmp = AllocateET_EM (lpMemPTD, lptkFunc);
    if (hGlbTmp EQ NULL)
        goto ERROR_EXIT;

    // Save the value of []ET
    lpMemRes[1] = MakePtrTypeGlb (hGlbTmp);

    // If it succeeded, ...
    if (lpYYRes2)
    {
        // Check for NoValue
        if (IsTokenNoValue (&lpYYRes2->tkToken))
        {
            // Free the YYRes (but not the storage)
            YYFree (lpYYRes2); lpYYRes2 = NULL;

            goto VALUE_EXIT;
        } // End IF

        // Split cases based upon the token type
        switch (lpYYRes2->tkToken.tkFlags.TknType)
        {
            case TKT_VARIMMED:
                // Save the immediate result
                lpMemRes[2] =
                  MakeSymEntry_EM (lpYYRes2->tkToken.tkFlags.ImmType,   // Immediate type
                                  &lpYYRes2->tkToken.tkData.tkLongest,  // Ptr to immediate value
                                   lptkFunc);                           // Ptr to function token
                // Check for error
                if (lpMemRes[2] EQ NULL)
                    goto ERROR_EXIT;
                break;

            case TKT_VARARRAY:
                // Save the global result
                lpMemRes[2] =
                  CopySymGlbDir_PTB (lpYYRes2->tkToken.tkData.tkGlbData);

                break;

            case TKT_VARNAMED:
                // stData is an LPSYMENTRY
                Assert (GetPtrTypeDir (lpYYRes2->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

                // If the value is immediate, ...
                if (IsTknImmed (&lpYYRes2->tkToken))
                {
                    // Save the immediate result
                    lpMemRes[2] =
                      MakeSymEntry_EM (lpYYRes2->tkToken.tkData.tkSym->stFlags.ImmType,     // Immediate type
                                      &lpYYRes2->tkToken.tkData.tkSym->stData.stLongest,    // Ptr to immediate value
                                       lptkFunc);                                           // Ptr to function token
                    // Check for error
                    if (lpMemRes[2] EQ NULL)
                        goto ERROR_EXIT;
                } else
                    // Save the global result
                    lpMemRes[2] =
                      CopySymGlbDir_PTB (lpYYRes2->tkToken.tkData.tkSym->stData.stGlbData);
                break;

            defstop
                break;
        } // End SWITCH

        // Free the YYRes (but not the storage)
        YYFree (lpYYRes2); lpYYRes2 = NULL;
    } else
    {
VALUE_EXIT:
        // There was an error

        // Split cases based upon the return code
        switch (aplLongestRC)
        {
            case EC_RETCODE_ERROR:
                // Save the value of []EM
                lpMemRes[2] =
                  CopySymGlbDirAsGlb (lpMemPTD->lpSISCur->hGlbQuadEM);

                break;

            case EC_RETCODE_NOVALUE:
            case EC_RETCODE_RESET_ONE:
                // Save hGlbGoto (0 0{rho}0)
                lpMemRes[2] =
                  CopySymGlbDirAsGlb (hGlbQuadEC2_DEF);

                break;

            case EC_RETCODE_GOTO_LINE:
                // Save the {goto} target
                // Note there's no need to increment the reference count as
                //   that was done in <SaveGotoTarget>.
                lpMemRes[2] = lpMemPTD->lpSISCur->lpSymGlbGoto;

                break;

            case EC_RETCODE_DISPLAY:
            case EC_RETCODE_NODISPLAY:
            defstop
                break;
        } // End SWITCH
    } // End IF/ELSE

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
    if (hGlbRes)
    {
        if (lpMemRes)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    // Unlocalize the STEs on the innermost level
    //   and strip off one level
    UnlocalizeSTEs ();

    // Free the current value of []DM
    FreeResultGlobalVar (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_DM]->stData.stGlbData);

    // Restore the original value of []DM
    lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_DM]->stData.stGlbData = hGlbQuadDM;

    return lpYYRes;
} // End SysFnMonEC_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $AllocateET_EM
//
//  Allocate space for []ET
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- AllocateET_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL AllocateET_EM
    (LPPERTABDATA lpMemPTD,         // Ptr to PerTabData global memory
     LPTOKEN      lptkFunc)         // Ptr to function token

{
    APLUINT      ByteRes;           // # bytes in the result
    HGLOBAL      hGlbTmp;           // Temporary global memory handle
    LPAPLUINT    lpMemTmp;          // Ptr to temporary ...

    // Calculate space needed for the []ET
    ByteRes = CalcArraySize (ARRAY_INT, 2, 1);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    hGlbTmp = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbTmp)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemTmp = MyGlobalLock (hGlbTmp);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemTmp)
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_INT;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = 2;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemTmp = VarArrayBaseToDim (lpMemTmp);

    // Save the dimension
    *((LPAPLDIM) lpMemTmp)++ = 2;

    // lpMemTmp now points to its data
    lpMemTmp[0] = ET_MAJOR (lpMemPTD->lpSISCur->EventType);
    lpMemTmp[1] = ET_MINOR (lpMemPTD->lpSISCur->EventType);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbTmp); lpMemTmp = NULL;

    return hGlbTmp;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End AllocateET_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydEC_EM_YY
//
//  Dyadic []EC -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydEC_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydEC_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End SysFnDydEC_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_ec.c
//***************************************************************************
