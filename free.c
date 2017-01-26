//***************************************************************************
//  NARS2000 -- Free Routines
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
//  $FreeResultName
//
//  Free named result
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FreeResultName"
#else
#define APPEND_NAME
#endif

void FreeResultName
    (LPTOKEN lptkRes)

{
    Assert (IsTknNamed (lptkRes));

    FreeResultSub (lptkRes, TRUE, FALSE);

////// Clear the STE flags
////ZeroMemory (&lptkRes->tkData.tkSym->stFlags,
////     sizeof (lptkRes->tkData.tkSym->stFlags));
} // End FreeResultName
#undef  APPEND_NAME


//***************************************************************************
//  $FreeTopResult
//
//  Free just the top level of a result, not recursively
//***************************************************************************

void FreeTopResult
    (LPPL_YYSTYPE lpYYRes)              // Ptr to YYSTYPE to free

{
    HGLOBAL *lphGlb;                    // Ptr to global memory handle
    int     iRefCnt;                    // RefCnt

    // Free just the top level, not recursively

    // Get a ptr to the global memory handle
    lphGlb = GetPtrGlbDataToken (&lpYYRes->tkToken);

    if (lphGlb NE NULL
     && *lphGlb NE NULL)
    {
        // Decrement the RefCnt
        iRefCnt = ChangeRefCntDir_PTB (*lphGlb, -1);

        if (iRefCnt EQ 0)
        {
            // Free and zap it
            DbgGlobalFree (*lphGlb); *lphGlb = NULL;
        } // End IF
    } // End IF
} // End FreeTopResult


//***************************************************************************
//  $FreeResult
//
//  Free the HGLOBALs and LPSYMENTRYs in a result
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FreeResult"
#else
#define APPEND_NAME
#endif

void FreeResult
    (LPPL_YYSTYPE lpYYRes)              // Ptr to YYSTYPE to free

{
    if (lpYYRes)
    {
        // If there is a curried index, ...
        if (lpYYRes->lpplYYIdxCurry)
        {
            // YYFree and Free it recursively
            FreeResult (lpYYRes->lpplYYIdxCurry); YYFree (lpYYRes->lpplYYIdxCurry); lpYYRes->lpplYYIdxCurry = NULL;
        } // End IF

        // If there is a curried arg, ...
        if (lpYYRes->lpplYYArgCurry)
        {
            // YYFree and Free it recursively
            FreeResult (lpYYRes->lpplYYArgCurry); YYFree (lpYYRes->lpplYYArgCurry); lpYYRes->lpplYYArgCurry = NULL;
        } // End IF

        // If there is a curried fcn, ...
        if (lpYYRes->lpplYYFcnCurry)
        {
            // If the curried function is not an AFO, ...
            if (!IsTknAFO (&lpYYRes->lpplYYFcnCurry->tkToken))
                // Free it recursively
                FreeResult (lpYYRes->lpplYYFcnCurry);

            // YYFree it
            YYFree (lpYYRes->lpplYYFcnCurry); lpYYRes->lpplYYFcnCurry = NULL;
        } // End IF

        // If there is a curried right operand, ...
        if (lpYYRes->lpplYYOpRCurry)
        {
            // If the curried right operand is not an AFO, ...
            if (!IsTknAFO (&lpYYRes->lpplYYOpRCurry->tkToken))
                // Free it recursively
                FreeResult (lpYYRes->lpplYYOpRCurry);

            // YYFree it
            YYFree (lpYYRes->lpplYYOpRCurry); lpYYRes->lpplYYOpRCurry = NULL;
        } // End IF

        Assert (lpYYRes->TknCount <= 1);

        // If the object is in use, ...
        if (lpYYRes->YYInuse)
            // Free it
            FreeResultSub (&lpYYRes->tkToken, FALSE, FALSE);
        else
            Assert (YYCheckInuse (lpYYRes));
    } // End IF
} // End FreeResult
#undef  APPEND_NAME


//***************************************************************************
//  $FreeResultTkn
//
//  Free the HGLOBALs and LPSYMENTRYs in a result
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FreeResultTkn"
#else
#define APPEND_NAME
#endif

void FreeResultTkn
    (LPTOKEN lptkRes)

{
    FreeResultSub (lptkRes, FALSE, FALSE);
} // End FreeResultTkn
#undef  APPEND_NAME


//***************************************************************************
//  $FreeResultSub
//
//  Free the HGLOBALs and LPSYMENTRYs in a result
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FreeResultSub"
#else
#define APPEND_NAME
#endif

void FreeResultSub
    (LPTOKEN lptkRes,
     UBOOL   bFreeName,
     UBOOL   bNoFreeDfn)

{
    STFLAGS  stMaskFlags = {0};             // STE mask flags
    HGLOBAL *lphGlbData;                    // Ptr to temporary global memory handle

    // Split cases based upon the token type
    switch (lptkRes->tkFlags.TknType)
    {
        case TKT_FCNNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRes->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // Check for internal functions,
            //   or no value
            if (lptkRes->tkData.tkSym->stFlags.FcnDir
             || !lptkRes->tkData.tkSym->stFlags.Value)
                break;

            // If the LPSYMENTRY is not immediate, it must be an HGLOBAL
            if (!lptkRes->tkData.tkSym->stFlags.Imm)
            {
                // Get the global memory handle
                lphGlbData = &lptkRes->tkData.tkSym->stData.stGlbData;

                // Data is an valid HGLOBAL named primitive
                //   or user-defined function/operator
                Assert (IsGlbTypeFcnDir_PTB (*lphGlbData)
                     || IsGlbTypeDfnDir_PTB (*lphGlbData));

                // If we're not allowed to free a named UDFO and it's a named UDFO, ...
                if (bNoFreeDfn
                 && IsGlbTypeDfnDir_PTB (*lphGlbData))
                    break;

                if (FreeResultGlobalDFLV (*lphGlbData))
                {
#ifdef DEBUG_ZAP
                    dprintfWL9 (L"**Zapping in FreeResultSub: Token=%p, Value=%p (%S#%d)",
                                 lptkRes,
                                 ClrPtrTypeDir (lptkRes->tkData.tkSym->stData.stGlbData),
                                 FNLN);
#endif
                    *lphGlbData = NULL;
                } // End IF
            } // End IF

            // Is it time to free the name?
            if (bFreeName)
            {
                // Set the flags we'll leave alone
                stMaskFlags.Inuse       = TRUE;
                stMaskFlags.ObjName     =
                stMaskFlags.SysVarValid = NEG1U;

                // Clear the symbol table flags
                *(UINT *) &lptkRes->tkData.tkSym->stFlags &= *(UINT *) &stMaskFlags;
            } // End IF

            break;

        case TKT_VARNAMED:
        case TKT_OP1NAMED:
        case TKT_OP2NAMED:
        case TKT_OP3NAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRes->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // Check for internal functions,
            //   or no value
            if (lptkRes->tkData.tkSym->stFlags.FcnDir
             || !lptkRes->tkData.tkSym->stFlags.Value)
                break;

            // If the LPSYMENTRY is not immediate, it must be an HGLOBAL
            if (!lptkRes->tkData.tkSym->stFlags.Imm)
            {
                // Get the global memory ptr
                lphGlbData = &lptkRes->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable or function array
                //   or user-defined function/operator
                Assert (IsGlbTypeVarDir_PTB (*lphGlbData)
                     || IsGlbTypeFcnDir_PTB (*lphGlbData)
                     || IsGlbTypeDfnDir_PTB (*lphGlbData));

                // If we're not allowed to free a named UDFO and it's a named UDFO, ...
                if (bNoFreeDfn
                 && IsGlbTypeDfnDir_PTB (*lphGlbData))
                    break;

                // The call to FreeResult after ArrayDisplay_EM needs the
                //   following if-statement.
                if (FreeResultGlobalDFLV (*lphGlbData))
                {
#ifdef DEBUG_ZAP
                    dprintfWL9 (L"**Zapping in FreeResultSub: Token=%p, Value=%p (%S#%d)",
                                 lptkRes,
                                 ClrPtrTypeDir (*lphGlbData),
                                 FNLN);
#endif
                    *lphGlbData = NULL;
                } // End IF
            } // End IF

            // Is it time to free the name?
            if (bFreeName)
            {
                // Set the flags we'll leave alone
                stMaskFlags.Inuse       = TRUE;
                stMaskFlags.ObjName     =
                stMaskFlags.SysVarValid = NEG1U;

                // Clear the symbol table flags
                *(UINT *) &lptkRes->tkData.tkSym->stFlags &= *(UINT *) &stMaskFlags;
            } // End IF

            return;

        case TKT_VARIMMED:  // tkData is an immediate constant
        case TKT_AXISIMMED: // ...
        case TKT_LSTIMMED:  // ...
        case TKT_STRAND:    // Occurs if MakeStrand fails -- data freed by <FreeStrand>
        case TKT_LISTPAR:   // tkData is -1
        case TKT_LISTINT:   // tkData is -1
        case TKT_INPOUT:    // tkData is UTF16_xxx
            return;

        case TKT_VARARRAY:  // tkData contains an HGLOBAL of an array of LPSYMENTRYs and HGLOBALs
        case TKT_AXISARRAY: // ...
        case TKT_FCNARRAY:  // ...
        case TKT_CHRSTRAND: // ...
        case TKT_NUMSTRAND: // ...
        case TKT_NUMSCALAR: // ...
        case TKT_LSTARRAY:  // tkData contains an HGLOBAL of an array of LPTOKENs
        case TKT_LSTMULT:   // ...
        case TKT_FCNAFO:    // ...                           UDFO header
        case TKT_OP1AFO:    // ...                           ...
        case TKT_OP2AFO:    // ...                           ...
        case TKT_DELAFO:    // ...                           ...
            // Get the global memory handle
            lphGlbData = GetPtrGlbDataToken (lptkRes);

            // Check for valid handle
            if (lphGlbData && *lphGlbData)
            {
                // tkData is a valid HGLOBAL variable or function array or list
                Assert (IsGlbTypeVarDir_PTB (*lphGlbData)
                     || IsGlbTypeFcnDir_PTB (*lphGlbData)
                     || IsGlbTypeDfnDir_PTB (*lphGlbData)
                     || IsGlbTypeLstDir_PTB (*lphGlbData));

                if (FreeResultGlobalDFLV (*lphGlbData))
                {
#ifdef DEBUG_ZAP
                    dprintfWL9 (L"**Zapping in FreeResultSub: Token=%p, Value=%p (%S#%d)",
                                 lptkRes,
                                 ClrPtrTypeDir (*lphGlbData),
                                 FNLN);
#endif
                    *lphGlbData = NULL;
                } // End IF
            } // End IF

            return;

        case TKT_FCNIMMED:      // tkData is an immediate function
        case TKT_OP1IMMED:      // ...                    monadic operator
        case TKT_OP2IMMED:      // ...                    dyadic  ...
        case TKT_OP3IMMED:      // ...                    ambiguous ...
        case TKT_OPJOTDOT:      // ...                    {jot}{dot}
        case TKT_FILLJOT:       // ...                    fill {jot}
        case TKT_RIGHTBRACE:    // ...                    right brace in {}
        case TKT_SYNTERR:       // ...                    SYNTAX ERROR
        case TKT_EOL:           // ...                    EOL EXAMPLE:  {{alpha}{alpha} {omega}}{each}{zilde}
            return;

        case TKT_STRNAMED:  // tkData contains an HGLOBAL of a strand of names
            DbgGlobalFree (lptkRes->tkData.tkGlbData);

            return;

        defstop
            return;
    } // End SWITCH
} // End FreeResultSub
#undef  APPEND_NAME


//***************************************************************************
//  $FreeResultGlobalDFLV
//
//  Free a global defined function, variable, function array, or list
//***************************************************************************

UBOOL FreeResultGlobalDFLV
    (HGLOBAL hGlbData)

{
    // Split cases based upon the signature
    switch (GetSignatureGlb (hGlbData))
    {
        case DFN_HEADER_SIGNATURE:
            return FreeResultGlobalDfn (hGlbData);

        case FCNARRAY_HEADER_SIGNATURE:
            return FreeResultGlobalFcn (hGlbData);

        case LSTARRAY_HEADER_SIGNATURE:
            return FreeResultGlobalLst (hGlbData);

        case VARARRAY_HEADER_SIGNATURE:
            return FreeResultGlobalVar (hGlbData);

        defstop
            return FALSE;
    } // End SWITCH
} // End FreeResultGlobalDFLV


//***************************************************************************
//  $FreeResultGlobalLst
//
//  Free a global list, recursively
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FreeResultGlobalLst"
#else
#define APPEND_NAME
#endif

UBOOL FreeResultGlobalLst
    (HGLOBAL hGlbData)

{
    LPAPLLIST lpMemLst;         // Ptr to list global memory
    APLNELM   aplNELM,          // List NELM
              uLst;             // Loop counter

    // Data is an valid HGLOBAL variable array
    Assert (IsGlbTypeLstDir_PTB (MakePtrTypeGlb (hGlbData)));

    // Lock the memory to get a ptr to it
    lpMemLst = MyGlobalLockLst (hGlbData);

#define lpHeader    ((LPLSTARRAY_HEADER) lpMemLst)
    // Get the NELM
    aplNELM = lpHeader->NELM;
#undef  lpHeader

    // Skip opver the header to the data
    lpMemLst = LstArrayBaseToData (lpMemLst);

    // Loop through the array of LISTs
    for (uLst = 0; uLst < aplNELM; uLst++, lpMemLst++)
    {
        Assert (lpMemLst->tkFlags.TknType EQ TKT_VARIMMED
             || lpMemLst->tkFlags.TknType EQ TKT_VARARRAY
             || lpMemLst->tkFlags.TknType EQ TKT_LISTSEP);

        // If it's a global var, ...
        if (lpMemLst->tkFlags.TknType EQ TKT_VARARRAY)
        {
            if (FreeResultGlobalVar (lpMemLst->tkData.tkGlbData))
            {
#ifdef DEBUG_ZAP
                dprintfWL9 (L"**Zapping in FreeResultGlobalLst: Global=%p, Value=%p (%S#%d)",
                             hGlbData,
                             ClrPtrTypeDir (lpMemLst->tkData.tkGlbData),
                             FNLN);
#endif
                // Zap the APLLIST
                ZeroMemory (lpMemLst, sizeof (lpMemLst[0]));
            } // End IF
        } // End IF
    } // End FOR

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbData, lpMemLst);

    return TRUE;
} // End FreeResultGlobalLst
#undef  APPEND_NAME


//***************************************************************************
//  $FreeResultGlobalVar
//
//  Free a global variable, recursively
//***************************************************************************

UBOOL FreeResultGlobalVar
    (HGLOBAL hGlbData)          // Global memory handle to free

{
    return FreeResultGlobalVarSub (hGlbData, TRUE);
} // End FreeResultGlobalVar


//***************************************************************************
//  $FreeResultGlobalIncompleteVar
//
//  Free a global variable, recursively
//***************************************************************************

UBOOL FreeResultGlobalIncompleteVar
    (HGLOBAL hGlbData)          // Global memory handle to free

{
    return FreeResultGlobalVarSub (hGlbData, FALSE);
} // End FreeResultGlobalIncompleteVar


//***************************************************************************
//  $FreeResultGlobalVarSub
//
//  Subroutine to FreeResultGlobalVar/FreeResultGlobalIncompleteVar
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FreeResultGlobalVarSub"
#else
#define APPEND_NAME
#endif

UBOOL FreeResultGlobalVarSub
    (HGLOBAL hGlbData,          // Global memory handle to free
     UBOOL   bReqComplete)      // TRUE iff we require a complete var

{
    LPVOID    lpMem;            // Ptr to the array global memory
    APLRANK   aplRank;          // The array rank
    APLSTYPE  aplType;          // The array storage type (see ARRAY_TYPES)
    APLNELM   aplNELM;          // The array NELM
    UINT      u,                // Loop counter
              RefCnt;           // The array reference count
    UBOOL     bRet;             // TRUE iff the result is valid

    // Data is an valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir_PTB (MakePtrTypeGlb (hGlbData)));

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLockVar (hGlbData);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)
    // If the var is not permanent, ...
    if (lpHeader->PermNdx EQ PERMNDX_NONE)
    {
        // Get the Type, RefCnt, NELM, and Rank
        aplType = lpHeader->ArrType;
        RefCnt  = lpHeader->RefCnt;
        aplNELM = lpHeader->NELM;
        aplRank = lpHeader->Rank;
#undef  lpHeader

        // Ensure non-zero
        Assert (RefCnt > 0);

        // Decrement
        RefCnt =
          DbgDecrRefCntDir_PTB (MakePtrTypeGlb (hGlbData));

        // If the RefCnt is zero, free the globals
        if (RefCnt EQ 0)
        // Split cases based upon the array type
        switch (aplType)
        {
            case ARRAY_BOOL:    // Nothing to do
            case ARRAY_INT:     // ...
            case ARRAY_CHAR:    // ...
            case ARRAY_FLOAT:   // ...
            case ARRAY_APA:     // ...
                break;

            case ARRAY_NESTED:  // Free the LPSYMENTRYs and/or HGLOBALs
                // Take into account nested prototypes
                aplNELM = max (aplNELM, 1);

                // Fall through to common handling

            case ARRAY_HETERO:  // Free the LPSYMENTRYs and/or HGLOBALs
                // Point to the array data (LPSYMENTRYs and/or HGLOBALs)
                lpMem = VarArrayDataFmBase (lpMem);

                // Loop through the LPSYMENTRYs and/or HGLOBALs
                for (u = 0; u < aplNELM; u++, ((LPAPLNESTED) lpMem)++)
                {
                    // Check for invalid ptrs
                    if (lpMem)
                    // Check for required complete vars
                    if (bReqComplete || *(LPAPLNESTED) lpMem)
                    switch (GetPtrTypeInd (lpMem))
                    {
                        case PTRTYPE_STCONST:
                            break;

                        case PTRTYPE_HGLOBAL:
                            if (FreeResultGlobalVarSub (ClrPtrTypeInd (lpMem), bReqComplete))
                            {
#ifdef DEBUG_ZAP
                                dprintfWL9 (L"**Zapping in FreeResultGlobalVar: Global=%p, Value=%p (%S#%d)",
                                             hGlbData,
                                             ClrPtrTypeInd (lpMem),
                                             FNLN);
#endif
                                *((LPVOID *) lpMem) = NULL;
                            } // End IF

                            break;

                        defstop
                            break;
                    } // End IF/SWITCH
                } // End FOR

                break;

            case ARRAY_RAT:
                // Point to the array data (APLRATs)
                lpMem = VarArrayDataFmBase (lpMem);

                // Loop through the APLRATs
                for (u = 0; u < aplNELM; u++, ((LPAPLRAT) lpMem)++)
                    Myq_clear ((LPAPLRAT) lpMem);

                break;

            case ARRAY_VFP:
                // Point to the array data (APLVFPs)
                lpMem = VarArrayDataFmBase (lpMem);

                // Loop through the APLVFPs
                for (u = 0; u < aplNELM; u++, ((LPAPLVFP) lpMem)++)
                    Myf_clear ((LPAPLVFP) lpMem);

                break;

            defstop
                break;
        } // End IF/SWITCH
    } else
        RefCnt = 1;     // Any non-zero value to prevent erasure

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMem = NULL;

    // If the RefCnt is zero, free the global
    bRet = (RefCnt EQ 0);

    if (bRet)
    {
        // Unlock and free (and set to NULL) a global name and ptr
        UnlFreeGlbName (hGlbData, lpMem);
    } // End IF

    return bRet;
} // End FreeResultGlobalVarSub
#undef  APPEND_NAME


//***************************************************************************
//  $FreeResultGlobalFcn
//
//  Free a global function
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FreeResultGlobalFcn"
#else
#define APPEND_NAME
#endif

UBOOL FreeResultGlobalFcn
    (HGLOBAL hGlbData)

{
    LPFCNARRAY_HEADER lpMemHdr;     // Ptr to the function array's global memory
    UINT              tknNELM,      // The # tokens in the function array
                      u,            // Loop counter
                      RefCnt;       // Reference count
    UBOOL             bRet;         // TRUE iff result is valid
    LPPL_YYSTYPE      lpYYToken;    // Ptr to function array token
    HGLOBAL          *lphGlbLcl,    // Ptr to global memory handle
                      hGlbTxtLine;  // Line text global memory handle

    // Data is an valid HGLOBAL function array
    Assert (IsGlbTypeFcnDir_PTB (MakePtrTypeGlb (hGlbData)));

    // Lock the memory to get a ptr to it
    lpMemHdr = MyGlobalLockFcn (hGlbData);

    // Get the RefCnt, NELM, and line text handle
    RefCnt      = lpMemHdr->RefCnt;
    tknNELM     = lpMemHdr->tknNELM;
    hGlbTxtLine = lpMemHdr->hGlbTxtLine;

    // Ensure non-zero
    Assert (RefCnt > 0);

    // If the pre-decremented RefCnt is one, free the globals
    if (RefCnt EQ 1)
    {
        // Decrement
        RefCnt =
          ChangeRefCntDir_PTB (MakePtrTypeGlb (hGlbData), -1);

        // Free the line text
        if (hGlbTxtLine)
        {
            DbgGlobalFree (hGlbTxtLine); hGlbTxtLine = NULL;
        } // End IF

        // Skip over the header to the data (PL_YYSTYPEs)
        lpYYToken = FcnArrayBaseToData (lpMemHdr);

        // Loop through the PL_YYSTYPEs
        for (u = 0; u < tknNELM; u++, lpYYToken++)
        {
            // N.B.:  As this is a function array, all curried FCN/OPR/ARGs have been resolved
            //   and are no longer present in the items
            Assert (lpYYToken->lpplYYIdxCurry EQ NULL);
            Assert (lpYYToken->lpplYYArgCurry EQ NULL);
            Assert (lpYYToken->lpplYYFcnCurry EQ NULL);
            Assert (lpYYToken->lpplYYOpRCurry EQ NULL);

            // Split cases based upon the token type
            switch (lpYYToken->tkToken.tkFlags.TknType)
            {
                case TKT_FCNIMMED:      // Nothing to do
                case TKT_VARIMMED:      // ...
                case TKT_OP1IMMED:      // ...
                case TKT_OP2IMMED:      // ...
                case TKT_OP3IMMED:      // ...
                case TKT_AXISIMMED:     // ...
                case TKT_OPJOTDOT:      // ...
                case TKT_FILLJOT:       // ...
                    break;              // Ignore immediates

                case TKT_FCNNAMED:      // Free the named function array
                case TKT_OP1NAMED:      // ...
                case TKT_OP2NAMED:      // ...
                case TKT_OP3NAMED:      // ...
                    // tkData is an LPSYMENTRY
                    Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

                    // Check for internal functions
                    if (lpYYToken->tkToken.tkData.tkSym->stFlags.FcnDir)
                        break;          // Ignore internal functions

                    // Get the global memory handle
                    lphGlbLcl = &lpYYToken->tkToken.tkData.tkSym->stData.stGlbData;

                    Assert (*lphGlbLcl NE NULL);

                    // tkData is a valid HGLOBAL function array
                    //   or user-defined function/operator
                    Assert (IsGlbTypeFcnDir_PTB (*lphGlbLcl)
                         || IsGlbTypeDfnDir_PTB (*lphGlbLcl));

                    // Free the function array or user-defined function/operator
                    if (FreeResultGlobalDFLV (*lphGlbLcl))
                    {
#ifdef DEBUG_ZAP
                        dprintfWL9 (L"**Zapping in FreeResultGlobalFcn: Global=%p, Value=%p (%S#%d)",
                                     hGlbData,
                                    *lphGlbLcl,
                                     FNLN);
#endif
                        *lphGlbLcl = NULL;
                    } // End IF

                    break;

                case TKT_FCNARRAY:      // Free the function array
                case TKT_FCNAFO:
                case TKT_OP1AFO:
                case TKT_OP2AFO:
                case TKT_DELAFO:
                    // Get the global memory handle
                    lphGlbLcl = GetPtrGlbDataToken (&lpYYToken->tkToken);

                    Assert (*lphGlbLcl NE NULL);

                    // tkData is a valid HGLOBAL function array
                    //   or user-defined function/operator
                    Assert (IsGlbTypeFcnDir_PTB (*lphGlbLcl)
                         || IsGlbTypeDfnDir_PTB (*lphGlbLcl));

                    // Free the function array or user-defined function/operator
                    if (FreeResultGlobalDFLV (*lphGlbLcl))
                    {
#ifdef DEBUG_ZAP
                        dprintfWL9 (L"**Zapping in FreeResultGlobalFcn: Global=%p, Value=%p (%S#%d)",
                                     hGlbData,
                                    *lphGlbLcl,
                                     FNLN);
#endif
                        *lphGlbLcl = NULL;
                    } // End IF

                    break;

                case TKT_VARARRAY:      // Free the var array (strand arg to dyadic op)
                case TKT_AXISARRAY:     // Free the axis array
                case TKT_CHRSTRAND:     // Free the character strand
                case TKT_NUMSTRAND:     // Free the numeric strand
                case TKT_NUMSCALAR:     // Free the numeric scalar
                    // Get the global handle
                    lphGlbLcl = &lpYYToken->tkToken.tkData.tkGlbData;

                    Assert (*lphGlbLcl NE NULL);

                    // tkData is a valid HGLOBAL variable array
                    Assert (IsGlbTypeVarDir_PTB (*lphGlbLcl));

                    // Free the global variable
                    if (FreeResultGlobalVar (*lphGlbLcl))
                    {
#ifdef DEBUG_ZAP
                        dprintfWL9 (L"**Zapping in FreeResultGlobalFcn: Global=%p, Value=%p (%S#%d)",
                                     hGlbData,
                                    *lphGlbLcl,
                                     FNLN);
#endif
                        *lphGlbLcl = NULL;
                    } // End IF

                    break;

                case TKT_VARNAMED:
                    // tkData is an LPSYMENTRY
                    Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

                    // If it's not an immediate, ...
                    if (!lpYYToken->tkToken.tkData.tkSym->stFlags.Imm)
                    {
                        // Get the global handle
                        lphGlbLcl = &lpYYToken->tkToken.tkData.tkSym->stData.stGlbData;

                        Assert (*lphGlbLcl NE NULL);

                        // stData is a valid HGLOBAL variable array
                        Assert (IsGlbTypeVarDir_PTB (*lphGlbLcl));

                        // Free the global variable
                        if (FreeResultGlobalVar (*lphGlbLcl))
                        {
#ifdef DEBUG_ZAP
                            dprintfWL9 (L"**Zapping in FreeResultGlobalFcn: Global=%p, Value=%p (%S#%d)",
                                         hGlbData,
                                        *lphGlbLcl,
                                         FNLN);
#endif
                            *lphGlbLcl = NULL;
                        } // End IF
                    } // End IF

                    break;

                defstop
                    break;
            } // End SWITCH
        } // End FOR
    } else
        // Decrement the RefCnt of all items
        RefCnt =
          DbgDecrRefCntFcnArray (hGlbData);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMemHdr = NULL;

    // If the RefCnt is zero, free the global
    bRet = (RefCnt EQ 0);

    if (bRet)
    {
        // We no longer need this storage
        DbgGlobalFree (hGlbData); hGlbData = NULL;
    } // End IF

    return bRet;
} // End FreeResultGlobalFcn
#undef  APPEND_NAME


//***************************************************************************
//  $FreeResultGlobalDfn
//
//  Free a global user-defined function/operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FreeResultGlobalDfn"
#else
#define APPEND_NAME
#endif

UBOOL FreeResultGlobalDfn
    (HGLOBAL hGlbData)              // Incoming global memory handle

{
    LPDFN_HEADER lpMemDfnHdr;       // Ptr to user-defined function/operator header global memory
    UINT         RefCnt;            // Reference count
    UBOOL        bRet;              // TRUE iff result is valid

    // Data is an valid HGLOBAL user-defined function/operator
    Assert (IsGlbTypeDfnDir_PTB (MakePtrTypeGlb (hGlbData)));

    // Lock the memory to get a ptr to it
    lpMemDfnHdr = MyGlobalLockDfn (hGlbData);

    // Get the reference count
    RefCnt = lpMemDfnHdr->RefCnt;

    // Ensure non-zero
    Assert (RefCnt > 0);

    // If it's not a MFO, decrement the reference count
    if (!lpMemDfnHdr->bMFO)
    {
        // Decrement the reference count
        RefCnt =
          DbgDecrRefCntDir_PTB (MakePtrTypeGlb (hGlbData));

        // If the RefCnt is zero, free the global
        bRet = (RefCnt EQ 0);

        if (bRet)
        {
            // Free the globals in the struc
            FreeResultGlobalDfnStruc (lpMemDfnHdr, TRUE);

            // Free the HshTab & SymTab
            FreeHshSymTabs (&lpMemDfnHdr->htsDFN, FALSE);
        } // End IF
    } else
        // Don't free the global storage
        bRet = FALSE;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMemDfnHdr = NULL;

    if (bRet)
    {
#ifdef DEBUG_ZAP
        dprintfWL0 (L"**Freeing in FreeResultGlobalDfn: %p (%S#%d)",
                     ClrPtrTypeDir (hGlbData),
                     FNLN);
#endif
        // We no longer need this storage
        DbgGlobalFree (hGlbData); hGlbData = NULL;
    } // End IF

    return bRet;
} // End FreeResultGlobalDfn
#undef  APPEND_NAME


//***************************************************************************
//  $FreeResultGlobalDfnStruc
//
//  Free the globals in the DFN_HEADER struc
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FreeResultGlobalDfnStruc"
#else
#define APPEND_NAME
#endif

void FreeResultGlobalDfnStruc
    (LPDFN_HEADER lpMemDfnHdr,      // Ptr to DFN_HEADER struc
     UBOOL        bUntokenize)      // TRUE iff we can call Untokenize on the function lines

{
    UINT      numFcnLines;          // # lines in the function
    LPFCNLINE lpFcnLines;           // Ptr to the array of structs (FCNLINE[numFcnLine])

    // Should we clear the STE flags?
    if (!lpMemDfnHdr->SaveSTEFlags)
    {
        STFLAGS stMaskFlags = {0};

        // Set the flags we'll leave alone
        stMaskFlags.Inuse       = TRUE;
        stMaskFlags.ObjName     =
        stMaskFlags.SysVarValid = NEG1U;

        // Clear the symbol table flags for the function name
        *(UINT *) &lpMemDfnHdr->steFcnName->stFlags &= *(UINT *) &stMaskFlags;

        // Clear the flag for next time
        lpMemDfnHdr->SaveSTEFlags = FALSE;
    } // End IF

    // Check the static HGLOBALs
    if (lpMemDfnHdr->hGlbTxtHdr)
    {
        // We no longer need this storage
        DbgGlobalFree (lpMemDfnHdr->hGlbTxtHdr); lpMemDfnHdr->hGlbTxtHdr = NULL;
    } // End IF

    if (lpMemDfnHdr->hGlbTknHdr)
    {
        // We no longer need this storage
        DbgGlobalFree (lpMemDfnHdr->hGlbTknHdr); lpMemDfnHdr->hGlbTknHdr = NULL;
    } // End IF

    if (lpMemDfnHdr->hGlbUndoBuff)
    {
        // We no longer need this storage
        DbgGlobalFree (lpMemDfnHdr->hGlbUndoBuff); lpMemDfnHdr->hGlbUndoBuff = NULL;
    } // End IF

    if (lpMemDfnHdr->hGlbMonInfo)
    {
        // We no longer need this storage
        DbgGlobalFree (lpMemDfnHdr->hGlbMonInfo); lpMemDfnHdr->hGlbMonInfo = NULL;
    } // End IF

    // Get the # lines in the function
    numFcnLines = lpMemDfnHdr->numFcnLines;

    // Get a ptr to the start of the function line structs (FCNLINE[numFcnLines])
    lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

    // Loop through the lines
    while (numFcnLines--)
    {
        if (bUntokenize
         && lpFcnLines->offTknLine)
        {
            // Free the tokens
            Untokenize ((LPTOKEN_HEADER) ByteAddr (lpMemDfnHdr, lpFcnLines->offTknLine)); lpFcnLines->offTknLine = 0;
        } //End IF

        if (lpFcnLines->hGlbTxtLine)
        {
            // We no longer need this storage
            DbgGlobalFree (lpFcnLines->hGlbTxtLine); lpFcnLines->hGlbTxtLine = NULL;
        } // End IF

        // Skip to the next struct
        lpFcnLines++;
    } // End WHILE
} // End FreeResultGlobalDfnStruc
#undef  APPEND_NAME


//***************************************************************************
//  $IzitQuadDM
//
//  Is the name of a token []DM?
//***************************************************************************

UBOOL IzitQuadDM
    (LPTOKEN lptkName)          // Ptr to the token

{
    HGLOBAL    htGlbName;       // Name global memory handle
    LPAPLCHAR  lpMemName;       // Ptr to name global memory
    APLBOOL    bRet;            // TRUE iff eraseable name

    if (IsTknNamed (lptkName))
    {
        // Get the name global memory handle
        htGlbName = lptkName->tkData.tkSym->stHshEntry->htGlbName;

        // Lock the memory to get a ptr to it
        lpMemName = MyGlobalLockWsz (htGlbName);

        // Save flag of whether or not the name is []DM
        bRet = lstrcmpiW (lpMemName, $QUAD_DM) EQ 0;

        // We no longer need this ptr
        MyGlobalUnlock (htGlbName); lpMemName = NULL;
    } else
        bRet = FALSE;

    return bRet;
} // End IzitQuadDM


//***************************************************************************
//  $FreeTempResult
//
//  Free temporary (unnamed) result
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FreeTempResult"
#else
#define APPEND_NAME
#endif

void FreeTempResult
    (LPPL_YYSTYPE lpYYRes)

{
    Assert (lpYYRes->TknCount <= 1);

    // If it's not named, ...
    if (!IsTknNamed (&lpYYRes->tkToken))
        FreeResultSub (&lpYYRes->tkToken, FALSE, FALSE);
#ifdef DEBUG
    else
        nop ();
#endif
} // End FreeTempResult
#undef  APPEND_NAME


//***************************************************************************
//  End of File: free.c
//***************************************************************************
