//***************************************************************************
//  NARS2000 -- System Function -- Quad TRACE & Quad STOP
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
#include "debug.h"              // For xxx_TEMP_OPEN macros


//***************************************************************************
//  $SysFnSTOP_EM_YY
//
//  System function:  []STOP -- Manage Stop Points
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnSTOP_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnSTOP_EM_YY
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
        return SysFnMonSTOP_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydSTOP_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnSTOP_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnTRACE_EM_YY
//
//  System function:  []TRACE -- Manage trace points
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnTRACE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnTRACE_EM_YY
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
        return SysFnMonTRACE_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydTRACE_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnTRACE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonSTOP_EM_YY
//
//  Monadic []STOP -- Return Stop Points
//***************************************************************************

LPPL_YYSTYPE SysFnMonSTOP_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return SysFnMonSTRACE_EM_YY (lptkFunc,      // Ptr to function token
                                 lptkRhtArg,    // Ptr to right arg token
                                 lptkAxis,      // Ptr to axis token (may be NULL)
                                 FALSE);        // TRUE iff this is a call to []TRACE
} // End SysFnMonSTOP_EM_YY


//***************************************************************************
//  $SysFnMonTRACE_EM_YY
//
//  Monadic []TRACE -- Return Trace Points
//***************************************************************************

LPPL_YYSTYPE SysFnMonTRACE_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return SysFnMonSTRACE_EM_YY (lptkFunc,      // Ptr to function token
                                 lptkRhtArg,    // Ptr to right arg token
                                 lptkAxis,      // Ptr to axis token (may be NULL)
                                 TRUE);         // TRUE iff this is a call to []TRACE
} // End SysFnMonTRACE_EM_YY


//***************************************************************************
//  $SysFnMonSTRACE_EM_YY
//
//  Monadic []STOP/[]TRACE -- Return Stop/Trace Points
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonSTRACE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonSTRACE_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis,              // Ptr to axis token (may be NULL)
     UBOOL   bTrace)                // TRUE iff this is a call to []TRACE

{
    APLSTYPE          aplTypeRht;           // Right arg storage type
    APLNELM           aplNELMRht,           // Right arg NELM
                      aplNELMRes;           // Result NELM
    APLRANK           aplRankRht;           // Right arg Rank
    APLLONGEST        aplLongestRht;        // Right arg longest if immediate
    HGLOBAL           hGlbRht = NULL,       // Right arg global memory handle
                      hGlbRes = NULL;       // Result    ...
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg header
                      lpMemHdrRes = NULL;   // ...    result    ...
    LPAPLCHAR         lpMemRht;             // Ptr to right arg data
    LPAPLINT          lpMemRes;             // ...    result    ...
    LPSYMENTRY        lpSymEntry;           // Ptr to SYMENTRY
    LPDFN_HEADER      lpMemDfnHdr = NULL;   // Ptr to DFN_HDR global memory
    STFLAGS           stFlags;              // STE flags
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    LPFCNLINE         lpFcnLines;           // Ptr to array of function line structs (FCNLINE[numFcnLines])
    UINT              uNumLines,            // # function lines
                      uLine;                // Loop counter
    APLUINT           ByteRes;              // # bytes in the result

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RANK_EXIT;

    // Check for DOMAIN ERROR
    if (!IsSimpleChar (aplTypeRht))
        goto DOMAIN_EXIT;

    // Get right arg's global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // Skip over the header and dimensions to the data
    if (lpMemHdrRht NE NULL)
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    else
        lpMemRht = (LPAPLCHAR) &aplLongestRht;

    // Lookup the name in the symbol table
    // SymTabLookupName sets the .ObjName enum,
    //   and the .Inuse flag
    ZeroMemory (&stFlags, sizeof (stFlags));
    lpSymEntry =
      SymTabLookupNameLength (lpMemRht,
                              lstrlenW (lpMemRht),
                             &stFlags);
    // If not found, ...
    if (lpSymEntry EQ NULL)
        goto DOMAIN_EXIT;

    // If it's not a user-defined function or MFO, ...
    if ((stFlags.ObjName NE OBJNAME_USR
      && stFlags.ObjName NE OBJNAME_MFO)
     || stFlags.stNameType EQ NAMETYPE_VAR)
        goto DOMAIN_EXIT;

    // Lock the memory to get a ptr to it
    lpMemDfnHdr = MyGlobalLockDfn (lpSymEntry->stData.stGlbData);

    // Get ptr to array of function line structs (FCNLINE[numFcnLines])
    lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

    // Get # function lines
    uNumLines = lpMemDfnHdr->numFcnLines;

    // Count in the header
    aplNELMRes =  ( bTrace  && lpMemDfnHdr->bTraceHdr)
              || ((~bTrace) && lpMemDfnHdr->bStopHdr);

    // Run through the function lines
    for (uLine = 0; uLine < uNumLines; uLine++)
    // Count the # active STOP/TRACE lines
    if (bTrace)
        aplNELMRes += lpFcnLines[uLine].bTrace;
    else
        aplNELMRes += lpFcnLines[uLine].bStop;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_INT, aplNELMRes, 1);

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
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_INT;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Fill in the dimension
    *VarArrayBaseToDim (lpMemHdrRes) = aplNELMRes;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Check on the STOP/TRACE of the header
    if ((  bTrace  && lpMemDfnHdr->bTraceHdr)
     || ((~bTrace) && lpMemDfnHdr->bStopHdr))
        *lpMemRes++ = 0;

    // Run through the function lines
    for (uLine = 0; uLine < uNumLines; uLine++)
    if ((  bTrace  && lpFcnLines[uLine].bTrace)
     || ((!bTrace) && lpFcnLines[uLine].bStop))
        *lpMemRes++ = uLine + 1;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // ...
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (lpMemDfnHdr NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lpSymEntry->stData.stGlbData); lpMemDfnHdr = NULL;
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

    return lpYYRes;
} // End SysFnMonSTRACE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydSTOP_EM_YY
//
//  Dyadic []STOP -- Set Stop Points
//***************************************************************************

LPPL_YYSTYPE SysFnDydSTOP_EM_YY
    (LPTOKEN lptkLftArg,                    // Ptr to left arg token
     LPTOKEN lptkFunc,                      // Ptr to function token
     LPTOKEN lptkRhtArg,                    // Ptr to right arg token
     LPTOKEN lptkAxis)                      // Ptr to axis token (may be NULL)

{
    return SysFnDydSTRACE_EM_YY (lptkLftArg,    // Ptr to left arg token
                                 lptkFunc,      // Ptr to function token
                                 lptkRhtArg,    // Ptr to right arg token
                                 lptkAxis,      // Ptr to axis token (may be NULL)
                                 FALSE);        // TRUE iff this is a call to []TRACE
} // End SysFnDydSTOP_EM_YY


//***************************************************************************
//  $SysFnDydTRACE_EM_YY
//
//  Dyadic []TRACE -- Set Trace Points
//***************************************************************************

LPPL_YYSTYPE SysFnDydTRACE_EM_YY
    (LPTOKEN lptkLftArg,                    // Ptr to left arg token
     LPTOKEN lptkFunc,                      // Ptr to function token
     LPTOKEN lptkRhtArg,                    // Ptr to right arg token
     LPTOKEN lptkAxis)                      // Ptr to axis token (may be NULL)

{
    return SysFnDydSTRACE_EM_YY (lptkLftArg,    // Ptr to left arg token
                                 lptkFunc,      // Ptr to function token
                                 lptkRhtArg,    // Ptr to right arg token
                                 lptkAxis,      // Ptr to axis token (may be NULL)
                                 TRUE);         // TRUE iff this is a call to []TRACE
} // End SysFnDydTRACE_EM_YY


//***************************************************************************
//  $SysFnDydSTRACE_EM_YY
//
//  Dyadic []STOP/[]TRACE -- Set Stop/Trace Points
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydSTRACE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydSTRACE_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis,              // Ptr to axis token (may be NULL)
     UBOOL   bTrace)                // TRUE iff this is a call to []TRACE

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
    LPVOID            lpMemLft;             // Ptr to left arg data
    LPAPLCHAR         lpMemRht;             // ...    right ...
    LPSYMENTRY        lpSymEntry;           // Ptr to SYMENTRY
    LPDFN_HEADER      lpMemDfnHdr = NULL;   // Ptr to DFN_HDR global memory
    STFLAGS           stFlags;              // STE flags
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    LPFCNLINE         lpFcnLines;           // Ptr to array of function line structs (FCNLINE[numFcnLines])
    APLINT            iNumLines,            // # function lines
                      iLine;                // Line #
    APLUINT           uLft;                 // Loop counter
    UBOOL             bRet;                 // TRUE iff the conversion to integer is successful

    // Get the attributes (Type, NELM, and Rank)
    //   of the left arg
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);

    // Check for RANK ERROR
    if (IsMultiRank (aplRankLft))
        goto LEFT_RANK_EXIT;

    // Check for DOMAIN ERROR
    if (!IsNumeric (aplTypeLft)
     && !IsCharEmpty (aplTypeLft, aplNELMLft))
        goto LEFT_DOMAIN_EXIT;

    // Get left arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);

    // Skip over the header and dimensions to the data
    if (lpMemHdrLft NE NULL)
        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
    else
        lpMemLft = (LPAPLCHAR) &aplLongestLft;

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Check for DOMAIN ERROR
    if (!IsSimpleChar (aplTypeRht))
        goto RIGHT_DOMAIN_EXIT;

    // Get right arg's global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // Skip over the header and dimensions to the data
    if (lpMemHdrRht NE NULL)
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    else
        lpMemRht = (LPAPLCHAR) &aplLongestRht;

    // Lookup the name in the symbol table
    // SymTabLookupName sets the .ObjName enum,
    //   and the .Inuse flag
    ZeroMemory (&stFlags, sizeof (stFlags));
    lpSymEntry =
      SymTabLookupNameLength (lpMemRht,
                              lstrlenW (lpMemRht),
                             &stFlags);
    // If not found, ...
    if (lpSymEntry EQ NULL)
        goto RIGHT_DOMAIN_EXIT;

    // If it's not a user-defined function or MFO, ...
    if ((stFlags.ObjName NE OBJNAME_USR
      && stFlags.ObjName NE OBJNAME_MFO)
     || stFlags.stNameType EQ NAMETYPE_VAR)
        goto RIGHT_DOMAIN_EXIT;

    // Lock the memory to get a ptr to it
    lpMemDfnHdr = MyGlobalLockDfn (lpSymEntry->stData.stGlbData);

    // Get ptr to array of function line structs (FCNLINE[numFcnLines])
    lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

    // Get # function lines
    iNumLines = lpMemDfnHdr->numFcnLines;

    // Clear all previous STOP or TRACE flags
    if (bTrace)
        // Clear the header flag
        lpMemDfnHdr->bTraceHdr = FALSE;
    else
        // Clear the header flag
        lpMemDfnHdr->bStopHdr  = FALSE;

    // Loop through all of the function lines
    for (iLine = 0; iLine < iNumLines; iLine++)
    if (bTrace)
        // Clear the line flag
        lpFcnLines[iLine].bTrace = FALSE;
    else
        // Clear the line flag
        lpFcnLines[iLine].bStop  = FALSE;

    // Loop through the left arg
    for (uLft = 0; uLft < aplNELMLft; uLft++)
    {
        // Get the next value as an integer
        iLine = ConvertToInteger_SCT (aplTypeLft,   // Argument storage type
                                      lpMemLft,     // ...      global memory handle
                                      uLft,         // Index into <lpMemLft>
                                     &bRet);        // Ptr to TRUE iff the result is valid
        if (bRet
         && 0 <= iLine
         &&      iLine <= iNumLines)
        {
            // Check for the header
            if (iLine EQ 0)
            {
                if (bTrace)
                    lpMemDfnHdr->bTraceHdr = TRUE;
                else
                    lpMemDfnHdr->bStopHdr  = TRUE;
            } else
            if (bTrace)
                lpFcnLines[iLine - 1].bTrace = TRUE;
            else
                lpFcnLines[iLine - 1].bStop  = TRUE;
        } // End IF
    } // End FOR

    // Return the vector of set lines
    lpYYRes =
      SysFnMonSTRACE_EM_YY (lptkFunc,              // Ptr to function token
                            lptkRhtArg,            // Ptr to right arg token
                            lptkAxis,              // Ptr to axis token (may be NULL)
                            bTrace);               // TRUE iff this is a call to []TRACE
    // Check for error
    if (lpYYRes EQ NULL)
        goto ERROR_EXIT;

    // Disable display of returned values
    lpYYRes->tkToken.tkFlags.NoDisplay = TRUE;

    goto NORMAL_EXIT;

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (lpMemDfnHdr NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lpSymEntry->stData.stGlbData); lpMemDfnHdr = NULL;
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

    return lpYYRes;
} // End SysFnDydSTRACE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $TraceLine
//
//  Display the trace of a function line
//***************************************************************************

void TraceLine
    (LPPERTABDATA  lpMemPTD,                // Ptr to PerTabData global memory
     LPPLLOCALVARS lpplLocalVars,           // Ptr to ParseLine local vars
     LPTOKEN       lptkFunc,                // Ptr to function token used for AFO function name
     LPPL_YYSTYPE  lpplYYCurObj)            // Ptr to the current token object (may be NULL)

{
    LPSIS_HEADER  lpSISCur;                 // Ptr to current SI Stack Header
    int           iStmtNum;                 // Stmt # (origin-1 if after PARSELINE_DONE)
    static WCHAR  wcDiamond [2] = {UTF16_DIAMOND},
                  wcLftArrow[2] = {UTF16_LEFTARROW},
                  wcRhtArrow[2] = {UTF16_RIGHTARROW};
    HGLOBAL       hGlbItm = NULL,           // Item global memory handle
                  hGlbRes = NULL;           // Result ...

    // Get the stmt # (origin-0)
    iStmtNum = lpplLocalVars->uStmtNum;

    // Get ptr to current SI stack
    lpSISCur = lpMemPTD->lpSISCur;

    // if the caller is neither Execute nor Quad, ...
    if (!(lpSISCur
       && (lpSISCur->DfnType EQ DFNTYPE_EXEC
        || (lpSISCur->DfnType EQ DFNTYPE_IMM
         && lpSISCur->lpSISPrv NE NULL
         && (lpSISCur->lpSISPrv->DfnType EQ DFNTYPE_EXEC
          || lpSISCur->lpSISPrv->DfnType EQ DFNTYPE_QUAD)
            )
           )
          )
        )
    {
        // Display the function name and line #

        // If the function name ptr is valid, ...
        if (lpSISCur->hGlbFcnName)
        {
            HGLOBAL      htGlbName;     // Function name global memory handle
            LPAPLCHAR    lpMemName;     // Ptr to function name global memory
            LPPL_YYSTYPE lpYYCurObj;    // Ptr to value to display
            LPDFN_HEADER lpMemDfnHdr;   // Ptr to DFN header
            VARS_TEMP_OPEN

            // If the function is an AFO, ...
            if (lpSISCur->bAFO && lptkFunc NE NULL)
            {
                Assert (!IsTknImmed (lptkFunc));

                // Split cases based upon the ptr type bits
                switch (GetPtrTypeDir (lptkFunc->tkData.tkVoid))
                {
                    case PTRTYPE_STCONST:
                        // Get the function name global memory handle
                        htGlbName = lptkFunc->tkData.tkSym->stHshEntry->htGlbName;

                        break;

                    case PTRTYPE_HGLOBAL:
                        Assert (IsGlbTypeDfnDir_PTB (lptkFunc->tkData.tkGlbData));

                        // Lock the memory to get a ptr to it
                        lpMemDfnHdr = MyGlobalLockDfn (lptkFunc->tkData.tkGlbData);

                        // Get the function name global memory handle
                        htGlbName = lpMemDfnHdr->steFcnName->stHshEntry->htGlbName;

                        // We no longer need this ptr
                        MyGlobalUnlock (lptkFunc->tkData.tkGlbData); lpMemDfnHdr = NULL;

                        break;

                    defstop
                        break;
                } // End SWITCH
            } else
                // Get the function name global memory handle
                htGlbName = lpSISCur->hGlbFcnName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLockWsz (htGlbName);

            CHECK_TEMP_OPEN

            // Format the name and line #
////////////*lpuNameLen =
              wsprintfW (lpMemPTD->lpwszTemp,
                         L"%s[%d]",
                         lpMemName,
                         lpSISCur->CurLineNum);
            // We no longer need this ptr
            MyGlobalUnlock (htGlbName); lpMemName = NULL;

            if (lpplYYCurObj EQ NULL)
                lpYYCurObj = &lpMemPTD->YYResExec;
            else
                lpYYCurObj = lpplYYCurObj;

            // Append a diamond for each stmt
            while (--iStmtNum)
                strcatW (lpMemPTD->lpwszTemp, wcDiamond);

            // If the value is sinked, ...
            if (lpYYCurObj->tkToken.tkFlags.NoDisplay)
                strcatW (lpMemPTD->lpwszTemp, wcLftArrow);
            else
            if (lpplLocalVars->ExitType EQ EXITTYPE_GOTO_LINE)
                strcatW (lpMemPTD->lpwszTemp, wcRhtArrow);

            // Note that although we're not finished with TEMP_OPEN, we must
            //   exit from it here as we can't carry the state across into
            //   <DisplayTraceResult>
            EXIT_TEMP_OPEN

            // If the arg is valid, ...
            if (lpYYCurObj->tkToken.tkFlags.TknType NE TKT_UNUSED)
            {
                // Display the trace result
                if (DisplayTraceResult (lpMemPTD->lpwszTemp,        // Ptr to function name/line #
                                       &lpYYCurObj->tkToken,        // Ptr to result token (may be NoValue)
                                        lpMemPTD,                   // Ptr to PerTabData global memory
                                       &lpplLocalVars->bCtrlBreak)) // Ptr to Ctrl-Break flag
                    goto NORMAL_EXIT;
            } // End IF

            // Prepend a blank so as to line up with other traces
            AppendLine (L" ", FALSE, FALSE);

            // Display the trace of the function name and line #
            AppendLine (lpMemPTD->lpwszTemp, FALSE, TRUE);
        } // End IF
    } // End IF
NORMAL_EXIT:
    // If we're to free the YYResExec, ...
    if (lpplLocalVars->bTraceFree)
    {
        LPPL_YYSTYPE lpYYVar;           // Ptr to a temp

        // Free (unnamed) the current object -- not allocated by YYAlloc so not to be YYFreed
        FreeTempResult (&lpMemPTD->YYResExec);

        // Make a PL_YYSTYPE NoValue entry
        lpYYVar =
          MakeNoValue_YY (&lpMemPTD->YYResExec.tkToken);
        lpMemPTD->YYResExec = *lpYYVar;
        YYFree (lpYYVar); lpYYVar = NULL;

        // Clear the flag
        lpplLocalVars->bTraceFree = FALSE;
    } // End IF
} // End TraceLine


//***************************************************************************
//  $TraceExit
//
//  Trace the value on function exit
//***************************************************************************

void TraceExit
    (LPPL_YYSTYPE lpYYRes,              // Ptr to the result (may be NoValue)
     LPDFN_HEADER lpMemDfnHdr,          // Ptr to user-defined function/operator header
     LPTOKEN      lptkFunc,             // Ptr to function token used for AFO function name
     LPPERTABDATA lpMemPTD)             // Ptr to PerTabData global memory

{
    LPAPLCHAR    lpMemName;             // Ptr to function name global memory
    static UBOOL bCtrlBreak = FALSE;    // Ctrl-Break flag
    HGLOBAL      htGlbName;             // Function name global memory handle
    VARS_TEMP_OPEN

    // If the function is an AFO, ...
    if (lpMemDfnHdr->bAFO && lptkFunc NE NULL)
    {
        Assert (!IsTknImmed (lptkFunc) && GetPtrTypeDir (lptkFunc->tkData.tkVoid) EQ PTRTYPE_STCONST);

        // Get the function name global memory handle
        htGlbName = lptkFunc->tkData.tkSym->stHshEntry->htGlbName;
    } else
        // Get the function name global memory handle
        htGlbName = lpMemDfnHdr->steFcnName->stHshEntry->htGlbName;

    // Lock the memory to get a ptr to it
    lpMemName = MyGlobalLockWsz (htGlbName);

    CHECK_TEMP_OPEN

    // Copy the name and line #
    strcpyW (lpMemPTD->lpwszTemp, lpMemName);
    strcatW (lpMemPTD->lpwszTemp, L"[0] ");

    // We no longer need this ptr
    MyGlobalUnlock (htGlbName); lpMemName = NULL;

    // Note that although we're not finished with TEMP_OPEN, we must
    //   exit from it here as we can't carry the state across into
    //   <DisplayTraceResult>
    EXIT_TEMP_OPEN

    // Display the trace result
    DisplayTraceResult (lpMemPTD->lpwszTemp,    // Ptr to function name/line #
                       &lpYYRes->tkToken,       // Ptr to result token (may be NoValue)
                        lpMemPTD,               // Ptr to PerTabData global memory
                       &bCtrlBreak);            // Ptr to Ctrl-Break flag
} // End TraceExit


//***************************************************************************
//  $DisplayTraceResult
//
//  Display a trace result whether from a line or exit trace
//***************************************************************************

UBOOL DisplayTraceResult
    (LPWCHAR      lpwszFcnLine,     // Ptr to function name/line #
     LPTOKEN      lptkRes,          // Ptr to result token (may be NoValue)
     LPPERTABDATA lpMemPTD,         // Ptr to PerTabData global memory
     LPUBOOL      lpbCtrlBreak)     // Ptr to Ctrl-Break flag

{
    HGLOBAL           hGlbItm = NULL,       // Item global memory handle
                      hGlbRes = NULL;       // Result ...
    APLNELM           aplNELMItm,           // Item NELM
                      aplNELMRes;           // Result NELM
    LPVARARRAY_HEADER lpMemHdrItm = NULL,   // Ptr to item global memory data
                      lpMemHdrRes = NULL;   // ...    result ...
    LPAPLNESTED       lpMemRes;             // Ptr to global memory data
    UBOOL             bRet = FALSE;         // TRUE iff the result is valid
    VARS_TEMP_OPEN

    CHECK_TEMP_OPEN

    // Calculate the item NELM ("+ 1" for the trailing zero
    aplNELMItm = lstrlenW (lpwszFcnLine) + 1;

    // Calculate the result NELM
    aplNELMRes = 2;

    // Allocate a character array for the function name and line #
    hGlbItm = AllocateGlobalArray (ARRAY_CHAR, aplNELMItm, 1, &aplNELMItm);
    if (hGlbItm EQ NULL)
        goto WSFULL_EXIT;

    // Allocate a character array for the function name and line #
    hGlbRes = AllocateGlobalArray (ARRAY_NESTED, aplNELMRes, 1, &aplNELMRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrItm = MyGlobalLockVar (hGlbItm);

    // Copy the item data to global memory
    CopyMemory (VarArrayDataFmBase (lpMemHdrItm),
                lpwszFcnLine,
                (APLU3264) (aplNELMItm * sizeof (lpwszFcnLine[0])));
    // We no longer need this ptr
    MyGlobalUnlock (hGlbItm); lpMemHdrItm = NULL;

    EXIT_TEMP_OPEN

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (hGlbRes);

    // Skip over the header and dimension to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Save the first item
    lpMemRes[0] = MakePtrTypeGlb (hGlbItm);

    // If it's not a var or is NoValue, ...
    if (!IsTknTypeVar (lptkRes->tkFlags.TknType)
     || IsTokenNoValue (lptkRes))
        // Save the second item
        lpMemRes[1] = MakePtrTypeGlb (hGlbZilde);
    else
    // If it's immediate, ...
    if (IsTknImmed (lptkRes))
        // Save the second item
        lpMemRes[1] = MakeSymEntry_EM (lptkRes->tkFlags.ImmType,
                                      &lptkRes->tkData.tkLongest,
                                       lptkRes);
    else
        // Save the second item
        lpMemRes[1] = CopySymGlbDir_PTB (lptkRes->tkData.tkGlbData);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

    // Display the line
    DisplayGlbArr_EM (hGlbRes, TRUE, lpbCtrlBreak, lptkRes);

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    EXIT_TEMP_OPEN
NORMAL_EXIT:
    if (hGlbRes NE NULL)
    {
        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } else
    if (hGlbItm NE NULL)
    {
        // We no longer need this storage
        DbgGlobalFree (hGlbItm); hGlbItm = NULL;
    } // End IF

    return bRet;
} // End DisplayTraceResult


//***************************************************************************
//  End of File: qf_strace.c
//***************************************************************************
