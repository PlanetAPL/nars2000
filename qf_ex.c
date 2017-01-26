//***************************************************************************
//  NARS2000 -- System Function -- Quad EX
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
//  $SysFnEX_EM_YY
//
//  System function:  []EX -- Expunge Name
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnEX_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnEX_EM_YY
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
        return SysFnMonEX_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydEX_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnEX_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonEX_EM_YY
//
//  Monadic []EX -- Expunge Name
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonEX_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonEX_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeRht;           // Right arg storage type
    APLNELM           aplNELMRht,           // Right arg NELM
                      aplNELMRes,           // Result NELM
                      aplNELMCol;           // Result column NELM
    APLRANK           aplRankRht;           // Right arg Rank
    APLLONGEST        aplLongestRht;        // Right arg longest if immediate
    HGLOBAL           hGlbRht = NULL,       // Right arg global memory handle
                      hGlbRes = NULL;       // Result    ...
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg header
                      lpMemHdrRes = NULL;   // ...    result    ...
    LPAPLCHAR         lpMemDataRht,         // Ptr to right arg char data
                      lpMemDataStart;       // Ptr to start of identifier
    LPAPLBOOL         lpMemDataRes;         // Ptr to result Boolean data
    APLUINT           uRht,                 // Loop counter
                      uCol,                 // ...
                      ByteRes;              // # bytes in the result
    LPSYMENTRY        lpSymEntry;           // Ptr to SYMENTRY
    STFLAGS           stFlags;              // STE flags
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    UBOOL             bRet = TRUE,          // TRUE iff result is valid
                      bAFO;                 // TRUE iff we're parsing an AFO
    UINT              uBitIndex;            // Bit index for looping through Boolean result
    APLU3264          uLen;                 // Name length

    // Determine if we're parsing an AFO
    bAFO = (SISAfo (GetMemPTD ()) NE NULL);

    // The right arg may be of three forms:
    //   1.  a scalar    name  as in 'a'
    //   2.  a vector of names as in 'a' or 'a b c'
    //   3.  a matrix of names as in 3 1{rho}'abc'

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RANK ERROR
    if (IsRank3P (aplRankRht))
        goto RANK_EXIT;

    // Check for DOMAIN ERROR
    if (!IsCharOrEmpty (aplTypeRht, aplNELMRht))
        goto DOMAIN_EXIT;

    // Get right arg's global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // Calculate the # identifiers in the argument
    //   allowing for vector and matrix with multiple names
    bRet =
      CalcNumIDs (aplNELMRht,       // Right arg NELM
                  aplRankRht,       // Right arg rank
                  aplLongestRht,    // Right arg longest
                  TRUE,             // TRUE iff we allow multiple names in a vector
                  lpMemHdrRht,      // Ptr to right arg global memory header
                 &aplNELMRes,       // Ptr to # right arg IDs
                 &aplNELMCol);      // Ptr to # right arg cols (matrix only)
    // Note that if bRet EQ FALSE, aplNELMRes EQ 1

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_BOOL, aplNELMRes, 1);

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
    lpHeader->ArrType    = ARRAY_BOOL;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Fill in the dimension
    *VarArrayBaseToDim (lpMemHdrRes) = aplNELMRes;

    // Skip over the header and dimensions to the data
    lpMemDataRes = VarArrayDataFmBase (lpMemHdrRes);

    // If we failed in CalcNumIDs, quit now
    if (!bRet)
        goto YYALLOC_EXIT;

    // Expunge each name

    // Initialize the bit index
    uBitIndex = 0;

    // Split cases based upon the right arg rank
    switch (aplRankRht)
    {
        case 0:
            // If we're not parsing an AFO,
            //   or this var is not a special AFO name, ...
            if (!bAFO
             || !IsAfoName ((LPAPLCHAR) &aplLongestRht, 1))
            {
                // Lookup the name in the symbol table
                // SymTabLookupName sets the .ObjName enum,
                //   and the .Inuse flag
                ZeroMemory (&stFlags, sizeof (stFlags));
                lpSymEntry = SymTabLookupNameLength ((LPAPLCHAR) &aplLongestRht,
                                                     1,
                                                    &stFlags);
                // If found, attempt to expunge the name
                // If not found, return a one if it's a valid name, zero otherwise
                if (lpSymEntry)
                    *lpMemDataRes |= (ExpungeName (lpSymEntry, FALSE)) << uBitIndex;
                else
                    *lpMemDataRes |= (IsValidName ((LPAPLCHAR) &aplLongestRht,
                                                   1)) << uBitIndex;
            } // End IF

            break;

        case 1:
            // Skip over the header and dimensions to the data
            lpMemDataRht = VarArrayDataFmBase (lpMemHdrRht);

            // Loop through the right arg looking for identifiers
            uRht = 0;
            while (TRUE)
            {
                // Skip over white space
                while (uRht < aplNELMRht && lpMemDataRht[uRht] EQ L' ')
                    uRht++;
                if (uRht < aplNELMRht)
                {
                    // Save the starting ptr
                    lpMemDataStart = &lpMemDataRht[uRht];

                    // Skip over black space
                    while (uRht < aplNELMRht && lpMemDataRht[uRht] NE L' ')
                        uRht++;

                    // Save the name length
                    uLen = (APLU3264) (&lpMemDataRht[uRht] - lpMemDataStart);

                    // If we're not parsing an AFO,
                    //   or this var is not a special AFO name, ...
                    if (!bAFO
                     || !IsAfoName (lpMemDataStart, uLen))
                    {
                        // Lookup the name in the symbol table
                        // SymTabLookupName sets the .ObjName enum,
                        //   and the .Inuse flag
                        ZeroMemory (&stFlags, sizeof (stFlags));
                        lpSymEntry = SymTabLookupNameLength (lpMemDataStart,
                                                            uLen,
                                                            &stFlags);
                        // If found, attempt to expunge the name
                        // If not found, return a one if it's a valid name, zero otherwise
                        if (lpSymEntry)
                            *lpMemDataRes |= (ExpungeName (lpSymEntry, FALSE)) << uBitIndex;
                        else
                            *lpMemDataRes |= (IsValidName (lpMemDataStart, uLen)) << uBitIndex;
                    } // End IF

                    // Check for end-of-byte
                    if (++uBitIndex EQ NBIB)
                    {
                        uBitIndex = 0;      // Start over
                        lpMemDataRes++;     // Skip to next byte
                    } // End IF
                } else
                    break;
            } // End WHILE

            break;

        case 2:
            // Skip over the header and dimensions to the data
            lpMemDataRht = VarArrayDataFmBase (lpMemHdrRht);

            for (uRht = 0; uRht < aplNELMRes; uRht++)
            {
                // Point to the start of the data
                lpMemDataStart = &lpMemDataRht[aplNELMCol * uRht];

                // Skip over leading white space
                uCol = 0;
                while (uCol < aplNELMCol && lpMemDataStart[uCol] EQ L' ')
                    uCol++;

                // Save the name length
                uLen = (APLU3264) (aplNELMCol - uCol);

                // If we're not parsing an AFO,
                //   or this var is not a special AFO name, ...
                if (!bAFO
                 || !IsAfoName (&lpMemDataStart[uCol], uLen))
                {
                    // Lookup the name in the symbol table
                    // SymTabLookupName sets the .ObjName enum,
                    //   and the .Inuse flag
                    ZeroMemory (&stFlags, sizeof (stFlags));
                    lpSymEntry = SymTabLookupNameLength (&lpMemDataStart[uCol],
                                                          uLen,
                                                         &stFlags);
                    // If found, attempt to expunge the name
                    // If not found, return a one if it's a valid name, zero otherwise
                    if (lpSymEntry)
                        *lpMemDataRes |= (ExpungeName (lpSymEntry, FALSE)) << uBitIndex;
                    else
                        *lpMemDataRes |= (IsValidName (lpMemDataStart, uLen)) << uBitIndex;
                } // End IF

                // Check for end-of-byte
                if (++uBitIndex EQ NBIB)
                {
                    uBitIndex = 0;      // Start over
                    lpMemDataRes++;     // Skip to next byte
                } // End IF
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH
YYALLOC_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
    lpYYRes->tkToken.tkFlags.NoDisplay = TRUE;
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
    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    // We no longer need this ptr
    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnMonEX_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ExpungeName
//
//  Expunge a given name and
//    return TRUE iff successful
//***************************************************************************

APLBOOL ExpungeName
    (LPSYMENTRY lpSymEntry,         // Ptr to the Symbol Table Entry
     UBOOL      bGlobalName)        // TRUE iff we should free the global value of []EM

{
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    HGLOBAL     *lphGlbQuadEM;      // Ptr to active hGlbQuadEM (in either lpSISCur or lpMemPTD)
    UBOOL        bQuadDM,           // TRUE iff the name is []DM
                 bQuadEM;           // ...                  []EM

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Check for eraseability
    if (!EraseableName (lpSymEntry, &bQuadDM, &bQuadEM))
        return FALSE;

    // If it's []DM, ...
    if (bQuadDM)
    {
        // Out with the old
        FreeResultGlobalVar (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_DM]->stData.stGlbData); lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_DM]->stData.stGlbData = NULL;

        // In with the new
        lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_DM]->stData.stGlbData = MakePtrTypeGlb (hGlbV0Char);
    } else
    // If it's []EM, ...
    if (bQuadEM)
    {
        // Get a ptr to either the global (in lpMemPTD)
        lphGlbQuadEM = bGlobalName ? &lpMemPTD->hGlbQuadEM
        //                           or the active hGlbQuadEM (in either lpSISCur or lpMemPTD)
                                   : GetPtrQuadEM (lpMemPTD);
        // Out wth the old
        FreeResultGlobalVar (*lphGlbQuadEM); *lphGlbQuadEM = NULL;

        // In with the new
        *lphGlbQuadEM = hGlbQuadEM_DEF;
    } else
    {
        // If the STE is not immediate and has a value, ...
        if (!lpSymEntry->stFlags.Imm
         && lpSymEntry->stFlags.Value)
        {
            // If the name is that of a UDFO/AFO, ...
            if (lpSymEntry->stFlags.UsrDfn)
            {
                // Enumerate all Function Editor windows
                 EnumChildWindows (hWndMF, &EnumCallbackExpPrevGlb, (LPARAM) (lpSymEntry->stData.stGlbData));
            } // End IF

            // Free the global memory handle
            FreeResultGlobalDFLV (lpSymEntry->stData.stGlbData);
        } // End IF

        // Erase the Symbol Table Entry
        //   unless it's a []var
        EraseSTE (lpSymEntry, FALSE);
    } // End IF/ELSE

    return TRUE;
} // End ExpungeName


//***************************************************************************
//  $EnumCallbackExpPrevGlb
//
//  EnumChildWindows callback to expunge a matching previous FE window
//   global memory handle
//***************************************************************************

UBOOL CALLBACK EnumCallbackExpPrevGlb
    (HWND   hWnd,           // Handle to child window
     LPARAM lParam)         // Application-defined value

{
    // When an MDI child window is minimized, Windows creates two windows: an
    // icon and the icon title.  The parent of the icon title window is set to
    // the MDI client window, which confines the icon title to the MDI client
    // area.  The owner of the icon title is set to the MDI child window.
    if (GetWindow (hWnd, GW_OWNER))     // If it's an icon title window, ...
        return TRUE;                    // skip it, and continue enumerating

    // If it's a Function Editor window, ...
    if (IzitFE (hWnd))
    {
        HGLOBAL hGlbDfnHdr;             // User-defined function/operator header global memory handle

        // Get the previous function global memory handle (if any)
        hGlbDfnHdr = (HGLOBAL) GetWindowLongPtrW (hWnd, GWLSF_HGLBDFNHDR);

        Assert (GetPtrTypeDir (MakeGlbFromVal (lParam)) EQ PTRTYPE_HGLOBAL);

        // If they match, ...
        if (ClrPtrTypeDir (hGlbDfnHdr) EQ ClrPtrTypeDir (MakeGlbFromVal (lParam)))
            // Zap the previous global memory handle
            SetWindowLongPtrW (hWnd, GWLSF_HGLBDFNHDR, (HANDLE_PTR) NULL);
    } // End IF

    return TRUE;        // Keep on truckin'
} // End EnumCallbackExpPrevGlb


//***************************************************************************
//  $EraseSTE
//
//  Erase a Symbol Table Entry
//***************************************************************************

void EraseSTE
    (LPSYMENTRY lpSymEntry,         // Ptr to the STE
     UBOOL      bEraseSysVar)       // TRUE iff we can erase []vars

{
    // If the entry is a system name and we're allowed to erase them,
    //   or it's not a system var, mark it as empty (e.g., VALUE ERROR)
    if (bEraseSysVar
     || lpSymEntry->stFlags.ObjName NE OBJNAME_SYS)
    {
        STFLAGS stFlagsMT = {0};        // STE flags for empty entry

        // Fill in mask flag values for erased entry
        stFlagsMT.Inuse   = TRUE;       // Retain Inuse flag
        stFlagsMT.ObjName = NEG1U;      // ...    ObjName setting

        // Clear the STE flags & data
        *(UINT *) &lpSymEntry->stFlags &= *(UINT *) &stFlagsMT;
        lpSymEntry->stData.stLongest = 0;
    } // End IF
} // End EraseSTE


//***************************************************************************
//  $EraseableName
//
//  Return TRUE iff the name is erasable
//***************************************************************************

APLBOOL EraseableName
    (LPSYMENTRY lpSymEntry,     // Ptr to SYMENTRY of name
     LPUBOOL    lpbQuadDM,      // Ptr to return flag of TRUE iff the name is []DM
     LPUBOOL    lpbQuadEM)      // ...                                        []EM

{
    HGLOBAL   htGlbName;        // Name global memory handle
    LPAPLCHAR lpMemName;        // Ptr to name global memory
    APLBOOL   bRet;             // TRUE iff eraseable name

    // Initialize the return value for []DM & []EM
    *lpbQuadDM =
    *lpbQuadEM = FALSE;

    // Split cases based upon the Name Type
    switch (lpSymEntry->stFlags.stNameType)
    {
        case NAMETYPE_UNK:
        case NAMETYPE_VAR:
        case NAMETYPE_FN0:
        case NAMETYPE_FN12:
        case NAMETYPE_OP1:
        case NAMETYPE_OP2:
        case NAMETYPE_TRN:
            // If the name is suspended or pendent, it's not eraseable
            if (IzitSusPendent (lpSymEntry))
                return FALSE;

            // Get the name global memory handle
            htGlbName = lpSymEntry->stHshEntry->htGlbName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLockWsz (htGlbName);

            // Izit a valid name?
            bRet = IsValidName (lpMemName, lstrlenW (lpMemName));

            // If it's a valid name, ...
            if (bRet)
            {
                UBOOL bQuadZ;

                // Save flag of whether or not the name is []DM
                *lpbQuadDM = lstrcmpiW (lpMemName, $QUAD_DM) EQ 0;

                // Save flag of whether or not the name is []EM
                *lpbQuadEM = lstrcmpiW (lpMemName, $QUAD_EM) EQ 0;

                // Save flag of whether or not the name is []Z
                   bQuadZ  = lstrcmpiW (lpMemName, $AFORESULT) EQ 0;

                // Not if it's a system name
                //   but []DM, []EM, and []Z are ok
                bRet = (*lpbQuadDM
                     || *lpbQuadEM
                     ||    bQuadZ
                     || !IsSysName (lpMemName));
            } // End IF

            // We no longer need this ptr
            MyGlobalUnlock (htGlbName); lpMemName = NULL;

            return bRet;

////////case NAMETYPE_LST:
        defstop
            return FALSE;
    } // End SWITCH
} // End EraseableName


//***************************************************************************
//  $IzitSusPendent
//
//  Return a one if the name is that of a suspended or
//    pendent defined function/operator
//***************************************************************************

APLBOOL IzitSusPendent
    (LPSYMENTRY lpSymEntry)

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    APLBOOL      bRet = FALSE;  // TRUE iff name is suspended or pendent
    LPSIS_HEADER lpSISCur;      // Ptr to current SIS layer
    HGLOBAL      htGlbName;     // Name global memory handle
    LPAPLCHAR    lpMemName,     // Ptr to name global memory
                 lpFcnName;     // Ptr to function name global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get a ptr to the innermost SIS layer
    lpSISCur = lpMemPTD->lpSISCur;

    // Get the name global memory handle
    htGlbName = lpSymEntry->stHshEntry->htGlbName;

    // Lock the memory to get a ptr to it
    lpMemName = MyGlobalLockWsz (htGlbName);

    while (lpSISCur && !bRet)
    {
        // Split cases based upon the function type
        switch (lpSISCur->DfnType)
        {
            case DFNTYPE_IMM:
            case DFNTYPE_EXEC:
            case DFNTYPE_QUAD:
            case DFNTYPE_ERRCTRL:
                break;

            case DFNTYPE_OP1:
            case DFNTYPE_OP2:
            case DFNTYPE_FCN:
                // Lock the memory to get a ptr to it
                lpFcnName = MyGlobalLockWsz (lpSISCur->hGlbFcnName);

                // Compare the names
                bRet = (lstrcmpW (lpMemName, lpFcnName) EQ 0);

                // We no longer need this ptr
                MyGlobalUnlock (lpSISCur->hGlbFcnName); lpFcnName = NULL;

                break;

            case DFNTYPE_QQUAD:
            case DFNTYPE_UNK:
            defstop
                break;
        } // End SWITCH

        // Skip to the previous SIS layer
        lpSISCur = lpSISCur->lpSISPrv;
    } // End WHILE

    // We no longer need this ptr
    MyGlobalUnlock (htGlbName); lpMemName = NULL;

    return bRet;
} // End IzitSusPendent


//***************************************************************************
//  $SysFnDydEX_EM_YY
//
//  Dyadic []EX -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydEX_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydEX_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token

     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End SysFnDydEX_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_ex.c
//***************************************************************************
