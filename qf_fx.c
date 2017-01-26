//***************************************************************************
//  NARS2000 -- System Function -- Quad FX
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
//  $SysFnFX_EM_YY
//
//  System function:  []FX -- Function Fix
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnFX_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnFX_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
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
        return SysFnMonFX_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydFX_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnFX_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonFX_EM_YY
//
//  Monadic []FX -- Function Fix
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonFX_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonFX_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    LPPERTABDATA      lpMemPTD;                 // Ptr to PerTabData global memory
    HGLOBAL           hGlbRes = NULL;           // Result global memory handle
    LPVOID            lpMemRes;                 // Ptr to result global memory
    APLSTYPE          aplTypeRht,               // Right arg storage type
                      aplTypeItmRht;            // Right arg item storage type
    APLNELM           aplNELMRht;               // Right arg NELM
    APLRANK           aplRankItmRht;            // Right arg item rank
    APLUINT           uRht;                     // Loop counter
    HGLOBAL           hGlbItmRht;               // Right arg item global memory handle
    LPVARARRAY_HEADER lpMemHdrRht = NULL,       // Ptr to right arg header
                      lpMemHdrItmRht = NULL,    // ...    right item arg ...
                      lpMemHdrRes = NULL;       // ...    result    ...
    LPAPLCHAR         lpMemRht;                 // Ptr to right arg global memory
    LPPL_YYSTYPE      lpYYRes = NULL;           // Ptr to the result
    APLLONGEST        aplLongestItmRht;         // Right arg item immediate value
    IMM_TYPES         immTypeItmRht;            // Right arg item immediate type
    SF_FCNS           SF_Fcns = {0};            // Common struc for SaveFunctionCom
    FX_PARAMS         FX_Params = {0};          // Local struc for  ...
    AFODETECT_STR     afoDetectStr = {0};       // Local struc for AfoDetect

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // In case we're called by )IN, zap the error line #
    lpMemPTD->uErrLine = NEG1U;

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &FX_Params.aplRankRht, &FX_Params.aplColsRht);

    // Check for empty right arg
    if (IsEmpty (aplNELMRht))
        goto RIGHT_DOMAIN_EXIT;

    // Get right arg's global ptrs
    FX_Params.aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &FX_Params.hGlbRht, &lpMemHdrRht);

    // If it's an immediate, ...
    if (lpMemHdrRht EQ NULL)
        // Point to the data
        lpMemRht = (LPAPLCHAR) &FX_Params.aplLongestRht;
    else
        // Skip over the header to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

    // Allocate the initial AFOLINE_STR
    afoDetectStr.hGlbLineStr =
      GlobalAlloc (GHND, AFOLINESTR_INIT * sizeof (AFOLINE_STR));

    // Check for error
    if (afoDetectStr.hGlbLineStr EQ NULL)
        goto WSFULL_EXIT;
    // Save in local params
    afoDetectStr.uLineStrCnt  = AFOLINESTR_INIT;
    afoDetectStr.lpafoLineStr = GlobalLock (afoDetectStr.hGlbLineStr);

    // Fill in common values
    SF_Fcns.bDisplayErr = FALSE;            // DO NOT Display Errors
////SF_Fcns.bRet        =                   // Filled in by SaveFunctionCom
////SF_Fcns.uErrLine    =                   // ...
////SF_Fcns.lpSymName   =                   // ...
    SF_Fcns.lptkFunc    = lptkFunc;         // Ptr to function token
    SF_Fcns.bMakeAFX    =
    SF_Fcns.bAFO        = (lpMemRht NE NULL) && AfoDetect (lpMemRht, &afoDetectStr);
    SF_Fcns.LclParams   = &FX_Params;
    SF_Fcns.sfTypes     = SFTYPES_FX;       // Caller type

    // Copy to local params
    FX_Params.lpafoDetectStr = &afoDetectStr;

    // Check for AFO function name
    if (!CheckAfoFcnName (&SF_Fcns, lpMemRht))
        goto SYMTAB_FULL_EXIT;

    // Check for RIGHT RANK/DOMAIN ERRORs
    switch (FX_Params.aplRankRht)
    {
        case 0:                     // Right arg scalar
        case 1:                     // ...       vector
            // Fill in common values
            FX_Params.aplRowsRht = 1;

            if (IsNested (aplTypeRht))
            {
                // Fill in common values
                FX_Params.aplRowsRht = aplNELMRht;

                // Ensure that each item is a char scalar/vector
                for (uRht = 0; uRht < aplNELMRht; uRht++)
                {
                    // Get the next value from the right arg
                    GetNextValueMem (lpMemRht,              // Ptr to right arg global memory
                                     aplTypeRht,            // Right arg storage type
                                     aplNELMRht,            // Right arg NELM
                                     uRht,                  // Right arg index
                                    &hGlbItmRht,            // Right arg item LPSYMENTRY or HGLOBAL (may be NULL)
                                    &aplLongestItmRht,      // Ptr to right arg immediate value
                                    &immTypeItmRht);        // Ptr to right arg immediate type
                    // If the right arg item is an array, ...
                    if (hGlbItmRht NE NULL)
                    {
                        // Get the right arg item global attrs
                        AttrsOfGlb (hGlbItmRht, &aplTypeItmRht, NULL, &aplRankItmRht, NULL);

                        // Ensure the item is a scalar/vector
                        if (IsMultiRank (aplRankItmRht))
                            goto RIGHT_RANK_EXIT;

                        // If this is the function header, ensure that it's not all blank
                        if (uRht EQ 0)
                        {
                            LPVARARRAY_HEADER lpMemHdtrItmRht = NULL;   // Ptr to item header
                            LPAPLCHAR         lpMemItmRht;              // Ptr to right arg item global memory
                            APLNELM           aplNELMItmRht;            // Right arg item NELM

                            // Lock the memory to get a ptr to it
                            lpMemHdrItmRht = MyGlobalLockVar (hGlbItmRht);

                            // Get the array NELM
#define lpHeader        lpMemHdrItmRht
                            aplNELMItmRht = lpHeader->NELM;

                            // Skip over the header to the data
                            lpMemItmRht = VarArrayDataFmBase (lpMemHdrItmRht);
#undef  lpHeader
                            // Check for all blanks
                            for (; aplNELMItmRht; aplNELMItmRht--)
                            if (lpMemItmRht[aplNELMItmRht - 1] NE L' ')
                                break;

                            // We no longer need this ptr
                            MyGlobalUnlock (hGlbItmRht); lpMemHdrItmRht = NULL;

                            if (IsEmpty (aplNELMItmRht))
                                goto RIGHT_DOMAIN_EXIT;
                        } // End IF

                        // Ensure the item is simple char
                        if (!IsSimpleChar (aplTypeItmRht))
                            goto RIGHT_DOMAIN_EXIT;
                    } else
                    {
                        // The right arg item is an immediate scalar

                        // If this is the function header, ensure that it's not empty
                        if (uRht EQ 0
                         && ((APLCHAR) FX_Params.aplLongestRht) EQ L' ')
                            goto RIGHT_DOMAIN_EXIT;

                        // Ensure the item is simple char
                        if (!IsImmChr (immTypeItmRht))
                            goto RIGHT_DOMAIN_EXIT;
                    } // End IF/ELSE
                } // End FOR

                // We no longer need this ptr
                MyGlobalUnlock (FX_Params.hGlbRht); lpMemHdrRht = NULL;

                // Fill in common values
                SF_Fcns.SF_LineLen      = SF_LineLenN;      // Ptr to line length function
                SF_Fcns.SF_ReadLine     = SF_ReadLineN;     // Ptr to read line function
                SF_Fcns.SF_IsLineCont   = SF_IsLineContN;   // Ptr to Is Line Continued function
                SF_Fcns.SF_NumPhyLines  = SF_NumPhyLinesN;  // Ptr to get # physical lines function
                SF_Fcns.SF_NumLogLines  = SF_NumLogLinesN;  // Ptr to get # logical  ...
                SF_Fcns.SF_CreationTime = SF_CreationTimeN; // Ptr to get function creation time
                SF_Fcns.SF_LastModTime  = SF_LastModTimeN;  // Ptr to get function last modification time
                SF_Fcns.SF_UndoBuffer   = SF_UndoBufferN;   // Ptr to get function Undo Buffer global memory handle
            } else
            if (IsSimpleChar (aplTypeRht))
            {
                // Simple character scalar or vector

                // Check for DOMAIN ERROR
                if (IsEmpty (aplNELMRht))
                    goto RIGHT_DOMAIN_EXIT;

                // Fill in common values
                SF_Fcns.SF_LineLen      = SF_LineLenSV;     // Ptr to line length function
                SF_Fcns.SF_ReadLine     = SF_ReadLineSV;    // Ptr to read line function
                SF_Fcns.SF_IsLineCont   = SF_IsLineContSV;  // Ptr to Is Line Continued function
                SF_Fcns.SF_NumPhyLines  = SF_NumPhyLinesSV; // Ptr to get # physical lines function
                SF_Fcns.SF_NumLogLines  = SF_NumLogLinesSV; // Ptr to get # logical  ...
                SF_Fcns.SF_CreationTime = SF_CreationTimeSV;// Ptr to get function creation time
                SF_Fcns.SF_LastModTime  = SF_LastModTimeSV; // Ptr to get function last modification time
                SF_Fcns.SF_UndoBuffer   = SF_UndoBufferSV;  // Ptr to get function Undo Buffer global memory handle
            } else
                goto RIGHT_DOMAIN_EXIT;

            break;

        case 2:                     // Right arg matrix
            if (!IsSimpleChar (aplTypeRht))
                goto RIGHT_DOMAIN_EXIT;

            // Fill in common values
            if (FX_Params.aplColsRht NE 0)
                FX_Params.aplRowsRht = aplNELMRht / FX_Params.aplColsRht;
            // If it's an AFO, ...
            if (SF_Fcns.bAFO)
            {
                SF_Fcns.SF_LineLen      = SF_LineLenSV;         // Ptr to line length function
                SF_Fcns.SF_ReadLine     = SF_ReadLineSV;        // Ptr to read line function
                SF_Fcns.SF_NumPhyLines  = SF_NumPhyLinesSV;     // Ptr to # physical lines function
                SF_Fcns.SF_NumLogLines  = SF_NumLogLinesSV;     // Ptr to # logical  ...
                SF_Fcns.SF_IsLineCont   = SF_IsLineContSV;      // Ptr to Is Line Continued function
            } else
            {
                SF_Fcns.SF_LineLen      = SF_LineLenM;          // Ptr to line length function
                SF_Fcns.SF_ReadLine     = SF_ReadLineM;         // Ptr to read line function
                SF_Fcns.SF_NumPhyLines  = SF_NumPhyLinesM;      // Ptr to # physical lines function
                SF_Fcns.SF_NumLogLines  = SF_NumLogLinesM;      // Ptr to # logical  ...
                SF_Fcns.SF_IsLineCont   = SF_IsLineContM;       // Ptr to Is Line Continued function
            } // End IF/ELSE

            SF_Fcns.SF_CreationTime = SF_CreationTimeM;     // Ptr to get function creation time
            SF_Fcns.SF_LastModTime  = SF_LastModTimeM;      // Ptr to get function last modification time
            SF_Fcns.SF_UndoBuffer   = SF_UndoBufferM;       // Ptr to get function Undo Buffer global memory handle

            break;

        default:                    // Right arg rank > 2
            goto RIGHT_RANK_EXIT;
    } // End SWITCH

    // Call common routine
    if (SaveFunctionCom (NULL, &SF_Fcns))
    {
        HGLOBAL   htGlbName;        // Function name global memory handle
        LPAPLCHAR lpMemName;        // Ptr to function name global memory
        UINT      uNameLen;         // Function name length
        APLUINT   ByteRes;          // # bytes in the result

        // The function fix succeeded -- return the function name as the result

        // Get the function name global memory handle
        htGlbName = SF_Fcns.lpSymName->stHshEntry->htGlbName;

        // Lock the memory to get a ptr to it
        lpMemName = MyGlobalLockWsz (htGlbName);

        // Get the length
        uNameLen = lstrlenW (lpMemName);

        //***************************************************************
        // Calculate space needed for the result
        //***************************************************************
        ByteRes = CalcArraySize (ARRAY_CHAR, uNameLen, 1);

        //***************************************************************
        // Check for overflow
        //***************************************************************
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;

        //***************************************************************
        // Now we can allocate the storage for the result
        //***************************************************************
        hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (hGlbRes EQ NULL)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader        lpMemHdrRes
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = ARRAY_CHAR;
////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = uNameLen;
        lpHeader->Rank       = 1;
#undef  lpHeader

        // Fill in the result's dimension
        *VarArrayBaseToDim (lpMemHdrRes) = uNameLen;

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

        // Copy the function name to the result
        CopyMemoryW (lpMemRes, lpMemName, uNameLen);

        // We no longer need this ptr
        MyGlobalUnlock (htGlbName); lpMemName = NULL;

        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
        lpYYRes->tkToken.tkFlags.NoDisplay = TRUE;
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } else
    {
        // The function fix failed.

        // If the error line # is NEG1U, there is an error message, so return NULL.
        if (SF_Fcns.uErrLine EQ NEG1U)
            goto ERROR_EXIT;

        // Otherwise, return the error line # as an integer scalar (origin-sensitive)

        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkInteger  = GetQuadIO () + SF_Fcns.uErrLine;
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

        // In case we're called by )IN, save the error line # for later use
        lpMemPTD->uErrLine = SF_Fcns.uErrLine;
    } // End IF/ELSE

    goto NORMAL_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

SYMTAB_FULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYMBOL_TABLE_FULL APPEND_NAME,
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
    if (FX_Params.hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (FX_Params.hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    if (afoDetectStr.hGlbLineStr NE NULL)
    {
        if (afoDetectStr.lpafoLineStr NE NULL)
        {
            // We no longer need this ptr
            GlobalUnlock (afoDetectStr.hGlbLineStr); afoDetectStr.lpafoLineStr = NULL;
        } // End IF

        // We no longer need this storage
        GlobalFree (afoDetectStr.hGlbLineStr); afoDetectStr.hGlbLineStr = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnMon_FX_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydFX_EM_YY
//
//  Dyadic []FX -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydFX_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydFX_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End SysFnDydFX_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $CheckAfoFcnName
//
//  Check for an AFO function name, returning its LPSYMENTRY
//***************************************************************************

UBOOL CheckAfoFcnName
    (LPSF_FCNS lpSF_Fcns,           // Ptr to Save Function local vars
     LPAPLCHAR lpMemRht)            // Ptr to right arg global memory

{
    UBOOL bRet = TRUE;              // TRUE iff successful

    // If we're fixing an AFO, ...
    if (lpSF_Fcns->bAFO)
    {
        UINT    uLineLen,           // Line length
                uLineOff;           // ...  offset
        STFLAGS stFlags = {0};      // STE flags
        LPFX_PARAMS       lpFX_Params;          // Ptr to common struc
        LPAFODETECT_STR   lpafoDetectStr;       // Ptr to AFODETECT_STR

        // Save local params
        lpFX_Params    = lpSF_Fcns->LclParams;
        lpafoDetectStr = lpFX_Params->lpafoDetectStr;

        // Get the precomputed line length & offset
        uLineLen = SF_LineLenSV (NULL, lpSF_Fcns, 0);
        uLineOff = lpafoDetectStr->lpafoLineStr[0].uLineOff;

        // Lookup the name in the symbol table
        // SymTabLookupName sets the .ObjName enum,
        //   and the .Inuse flag
        lpSF_Fcns->lpSymName =
          SymTabLookupNameLength (&lpMemRht[uLineOff],       // Ptr to value
                                   uLineLen,
                                  &stFlags);
        // If the name isn't already in the SymTab, ...
        if (lpSF_Fcns->lpSymName EQ NULL)
        {
            WCHAR wcZap;

            // Save and zap the ending char
            wcZap = lpMemRht[uLineOff + uLineLen];
                    lpMemRht[uLineOff + uLineLen] = WC_EOS;

            // Append the name to get a new LPSYMENTRY
            lpSF_Fcns->lpSymName =
              SymTabAppendName_EM (&lpMemRht[uLineOff],
                                   &stFlags);
            // Restore the zapped char
            lpMemRht[uLineOff + uLineLen] = wcZap;

            if (lpSF_Fcns->lpSymName EQ NULL)
                // Mark as not successful
                bRet = FALSE;
        } // End IF
    } // End IF

    return bRet;
} // End CheckAfoFcnName


//***************************************************************************
//  End of File: qf_fx.c
//***************************************************************************
