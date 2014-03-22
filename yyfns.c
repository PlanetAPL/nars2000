//***************************************************************************
//  NARS2000 -- PL_YYSTYPE Functions
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
//  $_YYAlloc
//
//  Allocate a new YYRes entry
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- _YYAlloc"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE _YYAlloc
#ifdef DEBUG
    (LPCHAR lpFileName,         // Ptr to filename where allocated
     UINT   uLineNum)           // Line # where allocated
#else
    (void)
#endif

{
    UINT         u;             // Loop counter
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYRes = NULL;// Ptr to the result

#ifdef DEBUG
    static UINT YYIndex = 0;
#endif

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Search for an empty YYRes slot,
    //   zero it,
    //   mark it as inuse,
    //   and return a ptr to it
    for (u = 0; u < lpMemPTD->numYYRes; u++)
    if (!lpMemPTD->lpYYRes[u].YYInuse)
    {
        // Set the return value
        lpYYRes = &lpMemPTD->lpYYRes[u];

        // Zero the memory
        ZeroMemory (lpYYRes, sizeof (lpYYRes[0]));

        // Mark as inuse
        lpYYRes->YYInuse = TRUE;
#ifdef DEBUG
        // Save the ptr to the filename where allocated
        lpYYRes->lpFileName = lpFileName;

        // Save the line # where allocated
        lpYYRes->uLineNum   = uLineNum;
#endif
        goto NORMAL_EXIT;
    } // End FOR/IF

    // If we get here, we ran out of indices
    lpYYRes = &lpMemPTD->lpYYRes[u++];
    lpMemPTD->numYYRes = u;

    // Zero the memory
    ZeroMemory (lpYYRes, sizeof (lpYYRes[0]));

    // Mark as inuse
    lpYYRes->YYInuse = TRUE;
NORMAL_EXIT:
#ifdef DEBUG
    lpYYRes->SILevel = lpMemPTD->SILevel;   // Save the SI Level
    lpYYRes->YYFlag = 0;  // Mark as a YYAlloc Index

    // Save unique number for debugging/tracking purposes
    lpYYRes->YYIndex = ++YYIndex;
#endif
    return lpYYRes;
} // End _YYAlloc
#undef  APPEND_NAME


//***************************************************************************
//  $YYCopy
//
//  Copy one PL_YYSTYPE to another
//    retaining the destination Inuse and DEBUG values
//***************************************************************************

void YYCopy
    (LPPL_YYSTYPE lpYYDst,
     LPPL_YYSTYPE lpYYSrc)

{
#ifdef DEBUG
    UINT   YYIndex,
           YYFlag,
           SILevel,
           uLineNum;
    LPCHAR lpFileName;
#endif

    Assert (lpYYDst->YYInuse);

#ifdef DEBUG
    // Save the old values
    YYIndex    = lpYYDst->YYIndex;
    YYFlag     = lpYYDst->YYFlag;
    SILevel    = lpYYDst->SILevel;
    lpFileName = lpYYDst->lpFileName;
    uLineNum   = lpYYDst->uLineNum;
#endif
    *lpYYDst = *lpYYSrc;                // Copy the PL_YYSTYPE
    lpYYDst->YYInuse = TRUE;            // Retain YYInuse flag
#ifdef DEBUG
    lpYYDst->YYIndex    = YYIndex;      // Retain YYIndex
    lpYYDst->YYFlag     = YYFlag;       // ...    YYFlag
    lpYYDst->SILevel    = SILevel;      // ...    SILevel
    lpYYDst->lpFileName = lpFileName;   // ...    ptr to filename
    lpYYDst->uLineNum   = uLineNum;     // ...    line #
#endif
} // End YYCopy


//***************************************************************************
//  $YYCopyFreeDst
//
//  Copy one PL_YYSTYPE to another
//    retaining the destination Inuse, Flag, and Index
//    and free the destination copy if it's not inuse.
//***************************************************************************

void YYCopyFreeDst
    (LPPL_YYSTYPE lpYYDst,
     LPPL_YYSTYPE lpYYSrc)

{
    if (lpYYDst->YYInuse)
        YYCopy (lpYYDst, lpYYSrc);
    else
    {
        lpYYDst->YYInuse = TRUE;    // Mark as in use for YYCopy
        YYCopy (lpYYDst, lpYYSrc);
        lpYYDst->YYInuse = FALSE;   // Mark as no longer in use
    } // End IF/ELSE
} // End YYCopyFreeDst


//***************************************************************************
//  $YYFree
//
//  Free a YYRes entry
//***************************************************************************

void YYFree
    (LPPL_YYSTYPE lpYYRes)      // Ptr to the YYRes entry

{
#ifdef DEBUG
    APLU3264     u;             // Index into lpMemPTD->YYRes
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    u = (APLU3264) (lpYYRes - lpMemPTD->lpYYRes);
    Assert (u < lpMemPTD->numYYRes);
    Assert (lpYYRes->YYInuse);
#endif
    ZeroMemory (lpYYRes, sizeof (lpYYRes[0]));
} // End YYFree


#ifdef DEBUG
//***************************************************************************
//  $YYResIsEmpty
//
//  Ensure that YYRes has no Inuse entries
//***************************************************************************

UBOOL YYResIsEmpty
    (void)

{
    UINT         u;             // Loop counter
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    UBOOL        bRet = TRUE;   // TRUE iff result is valid

    CheckMemStat ();

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Loop through the YYRes entries looking for
    //   entries in use at this SI Level
    for (u = 0; u < lpMemPTD->numYYRes; u++)
    if (lpMemPTD->lpYYRes[u].YYInuse
     && lpMemPTD->SILevel EQ lpMemPTD->lpYYRes[u].SILevel)
    {
#ifdef DEBUG
        WCHAR wszTemp[1024];

        wsprintfW (wszTemp,
                   L"The YYRes at %p TknType %S index %04X SI(%d) was allocated at %S#%d",
                  &lpMemPTD->lpYYRes[u],
                   GetTokenTypeName (lpMemPTD->lpYYRes[u].tkToken.tkFlags.TknType),
                   lpMemPTD->lpYYRes[u].YYIndex,
                   lpMemPTD->lpYYRes[u].SILevel,
                   lpMemPTD->lpYYRes[u].lpFileName,
                   lpMemPTD->lpYYRes[u].uLineNum);
        DbgMsgW (wszTemp);
#endif
        bRet = FALSE;

        break;
    } // End FOR/IF

    return bRet;
} // End YYResIsEmpty
#endif


//***************************************************************************
//  $YYCountFcnStr
//
//  Count the # tokens in a function strand
//***************************************************************************

UINT YYCountFcnStr
    (LPPL_YYSTYPE lpYYArg)          // Ptr to function strand

{
    UINT              uCnt,         // Loop counter
                      uLen,         // # tokens at top level
                      TknCount;     // The result
    LPTOKEN           lpToken;      // Ptr to token
    TOKEN_TYPES       tknType;      // Token type
    HGLOBAL           hGlbFcn;      // Function array global memory handle
    LPFCNARRAY_HEADER lpMemHdrFcn;  // Ptr to function array header global memory

    // Get the token count at the top level in this function strand
    uLen = lpYYArg->TknCount;

    // Loop through the tokens associated with this symbol
    for (uCnt = TknCount = 0; uCnt < uLen; uCnt++)
    {
        // If the function is indirect, recurse
        if (lpYYArg[uCnt].YYIndirect)
            TknCount += YYCountFcnStr (lpYYArg[uCnt].lpYYFcnBase);
        else
        {
            // Get the function arg token
            lpToken = &lpYYArg[uCnt].tkToken;

            // Get the token type
            tknType = lpToken->tkFlags.TknType;

            // If it's named and not immediate and not an internal function, ...
            if (IsTknNamedFcnOpr (lpToken)
             && !lpToken->tkData.tkSym->stFlags.Imm     // not an immediate, and
             && !lpToken->tkData.tkSym->stFlags.UsrDfn  // not a user-defined function/operator, and
             && !lpToken->tkData.tkSym->stFlags.FcnDir) // not an internal function
            {
                // Get the global memory handle
                hGlbFcn = lpToken->tkData.tkSym->stData.stGlbData;

                // Lock the memory to get a ptr to it
                lpMemHdrFcn = MyGlobalLock (hGlbFcn);

                Assert (lpMemHdrFcn->Sig.nature EQ FCNARRAY_HEADER_SIGNATURE);

                // Accumulate tokens
                TknCount += lpMemHdrFcn->tknNELM;

                // We no longer need this ptr
                MyGlobalUnlock (hGlbFcn); lpMemHdrFcn = NULL;
            } else
            if (tknType EQ TKT_FCNARRAY)
                // Recurse down through this function array item
                TknCount += YYCountFcnGlb (GetGlbHandle (lpToken));
            else
                TknCount++;
        } // End IF/ELSE
    } // End FOR

    return TknCount;
} // End YYCountFcnStr


//***************************************************************************
//  $YYCountFcnGlb
//
//  Count the # tokens in a function HGLOBAL
//***************************************************************************

UINT YYCountFcnGlb
    (HGLOBAL hGlbFcn)                   // Global memory handle

{
    UINT              TknCount = 0;     // The result
    LPFCNARRAY_HEADER lpMemHdrFcn;      // Ptr to function array header global memory
    LPPL_YYSTYPE      lpMemFcn;         // Ptr to function array global memory

    // Lock the memory to get a ptr to it
    lpMemHdrFcn = MyGlobalLock (hGlbFcn);

    // Split cases based upon the signature
    switch (lpMemHdrFcn->Sig.nature)
    {
        case DFN_HEADER_SIGNATURE:
        case VARARRAY_HEADER_SIGNATURE:
            // Count in one UDFO/var
            TknCount++;

            break;

        case FCNARRAY_HEADER_SIGNATURE:
            // If this function array is a Train, ...
            if (lpMemHdrFcn->fnNameType EQ NAMETYPE_TRN)
                // Count in one token
                TknCount++;
            else
            {
                UINT uCnt2,             // Loop counter
                     uLen2;             // # tokens at top level

                // Get the token count
                uLen2 = lpMemHdrFcn->tknNELM;

                // Skip over the header to the data
                lpMemFcn = FcnArrayBaseToData (lpMemHdrFcn);

                // Loop through the function array
                for (uCnt2 = 0; uCnt2 < uLen2; uCnt2++, lpMemFcn++)
                // Split off immediates so as not to double count TKT_OP1IMMED/INDEX_OPTRAINs
                if (IsTknImmed (&lpMemFcn->tkToken))
                    TknCount++;
                else
                    // Recurse down through this function array item
                    TknCount += YYCountFcnGlb (GetGlbHandle (&lpMemFcn->tkToken));
            } // End IF/ELSE

            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbFcn); lpMemHdrFcn = NULL; lpMemFcn = NULL;

    return TknCount;
} // End YYCountFcnGlb


//***************************************************************************
//  $YYIsFcnStrAxis
//
//  Check for an axis operator in a function strand
//***************************************************************************

UBOOL YYIsFcnStrAxis
    (LPPL_YYSTYPE lpYYArg)          // Ptr to function strand

{
    UBOOL             bRet;         // TRUE iff the result is valid
    HGLOBAL           hGlbFcn;      // Function array global memory handle
    LPFCNARRAY_HEADER lpMemHdrFcn;  // Ptr to function array header global memory
    LPPL_YYSTYPE      lpMemFcn;     // Ptr to function array global memory

    /*
        The only cases we need to consider are as follows:

            Token           Type        Imm/Glb     TknCount

        1.  TKT_FCNIMMED    /           Imm             2
            TKT_AXISIMMED   [2]         Imm             1

        2.  TKT_FCNNAMED    /           Imm             2
            TKT_AXISIMMED   [2]         Imm             1

        3.  TKT_FCNARRAY    /[2]        Glb             1

        4.  TKT_FCNNAMED    /[2]        Glb             1

     */

    // SPlit cases based upon the token type
    switch (lpYYArg->tkToken.tkFlags.TknType)
    {
        case TKT_FCNIMMED:
            // Is there another token?
            if (lpYYArg->TknCount > 1)
                // Is the next token an axis value?
                return (lpYYArg[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
                     || lpYYArg[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY);
            else
                return FALSE;

        case TKT_FCNNAMED:
            // If the named op is immediate, ...
            if (lpYYArg->tkToken.tkData.tkSym->stFlags.Imm)
            {
                // Is there another token?
                if (lpYYArg->TknCount > 1)
                    // Is the next token an axis value?
                    return (lpYYArg[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
                         || lpYYArg[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY);
                else
                    return FALSE;
            } // End IF

            // Fall through to common global case

        case TKT_FCNARRAY:
            // Get the global memory handle
            hGlbFcn = lpYYArg->tkToken.tkData.tkSym->stData.stGlbData;

            // Lock the memory to get a ptr to it
            lpMemHdrFcn = MyGlobalLock (hGlbFcn);

            Assert (lpMemHdrFcn->tknNELM > 1);

            // Skip over the function array header
            lpMemFcn = FcnArrayBaseToData (lpMemHdrFcn);

            // Is the next token an axis value?
            bRet = (((LPPL_YYSTYPE) lpMemFcn)[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
                 || ((LPPL_YYSTYPE) lpMemFcn)[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbFcn); lpMemHdrFcn = NULL; lpMemFcn = NULL;

            return bRet;

        defstop
            return FALSE;
    } // End SWITCH
} // End YYIsFcnStrAxis


//***************************************************************************
//  $YYCopyFcn
//
//  Copy one or more PL_YYSTYPE functions to a memory object
//***************************************************************************

LPPL_YYSTYPE YYCopyFcn
    (LPPL_YYSTYPE  lpYYMem,             // Ptr to result memory object
     LPPL_YYSTYPE  lpYYArg,             // Ptr to function arg
     LPPL_YYSTYPE *lplpYYBase,          // Ptr to ptr to YY base address
     LPINT         lpTknCount)          // Ptr to resulting token count

{
    int               i,                    // Loop counter
                      iLen,                 // Token count in this function strand
                      TknCount,             // # tokens added for this element of the function strand
                      TotalTknCount = 0;    // Total token count
    PL_YYSTYPE        YYFcn = {0};          // Temporary YYSTYPE
    HGLOBAL           hGlbFcn;              // Function array global memory handle
    LPTOKEN           lpToken;              // Ptr to temporary token
    LPPL_YYSTYPE      lpYYMem0,             // Ptr to temporary YYSTYPE
                      lpYYMem1,             // ...
                      lpYYCopy;             // ...
    UBOOL             bYYFcn,               // TRUE iff we're using YYFcn
                      bGlbTkn;              // TRUE iff we're using lpToken

    // Get the token count in this function strand
    iLen = lpYYArg->TknCount;

    // We need to modify the function count in the first token,
    //   so save its address in the array.
    lpYYMem0 = lpYYMem;

    // Loop through the tokens associated with this symbol
    for (i = 0; i < iLen; i++)
    {
#ifdef DEBUG
        LPPL_YYSTYPE lpYYArgI;

        lpYYArgI = &lpYYArg[i];
#endif
        Assert (YYCheckInuse (&lpYYArg[i]));

        // If the function base is valid, ...
        if (lpYYArg[i].lpYYFcnBase)
            // Calculate the earlier function base
            *lplpYYBase = min (*lplpYYBase, lpYYArg[i].lpYYFcnBase);

        // If the function is indirect, recurse
        if (lpYYArg[i].YYIndirect)
        {
            // Save the starting point so we can save the token count
            lpYYMem1 = lpYYMem;

            TknCount = 0;   // Initialize as it is incremented in YYCopyFcn
            lpYYMem = YYCopyFcn (lpYYMem, lpYYArg[i].lpYYFcnBase, lplpYYBase, &TknCount);

            Assert (TknCount EQ (lpYYMem - lpYYMem1));

            // Set the proper token count
            lpYYMem1->TknCount = TknCount;

            Assert (YYCheckInuse (&lpYYArg[i]));
        } else
        {
            // Get the function arg token
            lpToken = &lpYYArg[i].tkToken;

            // Assume it's a single token
            TknCount = 1;

            // Assume we're not using lpToken
            bGlbTkn = FALSE;

            // Special case for named functions/operators
            if (lpToken->tkFlags.TknType EQ TKT_FCNNAMED
             || lpToken->tkFlags.TknType EQ TKT_OP1NAMED
             || lpToken->tkFlags.TknType EQ TKT_OP2NAMED
             || lpToken->tkFlags.TknType EQ TKT_OP3NAMED)
            {
                // Mark as using YYFcn
                bYYFcn = TRUE;

                // tkData is an LPSYMENTRY
                Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

                // If it's an immediate function/operator, copy it directly
                if (lpToken->tkData.tkSym->stFlags.Imm)
                {
                    YYFcn.tkToken.tkFlags.TknType   = TranslateImmTypeToTknType (lpToken->tkData.tkSym->stFlags.ImmType);
                    YYFcn.tkToken.tkFlags.ImmType   = lpToken->tkData.tkSym->stFlags.ImmType;
////////////////////YYFcn.tkToken.tkFlags.NoDisplay = FALSE;    // Already zero from = {0}
                    YYFcn.tkToken.tkData.tkLongest  = 0;        // Keep the extraneous data clear
                    YYFcn.tkToken.tkData.tkChar     = lpToken->tkData.tkSym->stData.stChar;
                    YYFcn.tkToken.tkCharIndex       = lpToken->tkCharIndex;
                    YYFcn.TknCount                  = TknCount;
////////////////////YYFcn.YYInuse                   = FALSE;    // (Factored out below)
////////////////////YYFcn.YYIndirect                = FALSE;    // Already zero from = {0}
                    YYFcn.YYCopyArray               = lpYYArg[i].YYCopyArray;
////////////////////YYFcn.YYAvail                   = 0;        // Already zero from = {0}
////////////////////YYFcn.YYIndex                   = 0;        // (Factored out below)
////////////////////YYFcn.YYFlag                    = 0;        // Already zero from = {0}
////////////////////YYFcn.lpYYFcnBase               = NULL;     // Already zero from = {0}
                    YYFcn.lpYYStrandBase            = lpYYArg[i].lpYYStrandBase;
                } else
                // If it's an internal function, ...
                if (lpToken->tkData.tkSym->stFlags.FcnDir)
                    // Copy the argument
                    YYFcn = lpYYArg[i];
                else
                {
                    // Get the global memory handle or function address if direct
                    hGlbFcn = lpToken->tkData.tkSym->stData.stGlbData;

                    // stData is a valid HGLOBAL function array
                    //   or user-defined function/operator
                    Assert (IsGlbTypeFcnDir_PTB (hGlbFcn)
                         || IsGlbTypeDfnDir_PTB (hGlbFcn));

                    // Mark as using lpToken
                    bGlbTkn = TRUE;

                    // Mark as not using YYFcn
                    bYYFcn = FALSE;
                } // End IF/ELSE/...

                // If we used YYFcn (and thus didn't already copy the function to lpYYMem)
                if (bYYFcn)
                {
                    YYFcn.YYInuse  = FALSE;
#ifdef DEBUG
                    YYFcn.YYIndex  = NEG1U;
#endif
                    // Copy the destination YYInuse flag in case it's
                    //   in the YYRes allocated region
                    YYFcn.YYInuse  = lpYYMem->YYInuse;
                    YYFcn.TknCount = TknCount;

                    // Copy to the destination
                    *lpYYMem++ = YYFcn;
                    Assert (YYCheckInuse (&lpYYMem[-1]));
                } // End IF
            } else
            if (lpToken->tkFlags.TknType EQ TKT_FCNARRAY)
            {
                // tkData is an HGLOBAL
                Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_HGLOBAL);

                // Get the global memory handle or function address if direct
                hGlbFcn = lpToken->tkData.tkGlbData;

                // stData is a valid HGLOBAL function array
                //   or user-defined function/operator
                Assert (IsGlbTypeFcnDir_PTB (hGlbFcn)
                     || IsGlbTypeDfnDir_PTB (hGlbFcn));

                // Mark as using lpToken
                bGlbTkn = TRUE;
            } else
            {
                // If the token type is a var or axis array, ...
                if (lpToken->tkFlags.TknType EQ TKT_VARARRAY
                 || lpToken->tkFlags.TknType EQ TKT_AXISARRAY)
                    // Don't increment the refcnt as that was done
                    //   when the var array was created in MakeVarStrand
                    lpYYCopy = CopyPL_YYSTYPE_YY (&lpYYArg[i]);
                else
                    lpYYCopy = CopyPL_YYSTYPE_EM_YY (&lpYYArg[i], FALSE);
                if (lpYYMem->YYInuse)
                    YYCopy (lpYYMem++, lpYYCopy);
                else
                    YYCopyFreeDst (lpYYMem++, lpYYCopy);
                Assert (YYCheckInuse (&lpYYMem[-1]));
                YYFree (lpYYCopy); lpYYCopy = NULL;
            } // End IF/ELSE

            // If we used lpToken, ...
            if (bGlbTkn)
            {
                // Initialize as it is incremented in YYCopyGlbFcn_PTB
                TknCount = 0;

                lpYYMem = YYCopyGlbFcn_PTB (lpYYMem, lpToken, &lpYYArg[i], &TknCount);
            } // End IF
        } // End IF/ELSE

        // If this array is YYCopyArray'ed, ...
        if (lpYYArg[i].YYCopyArray)
            YYFreeArray (&lpYYArg[i]);

        // Accumulate into the total token count
        TotalTknCount += TknCount;
    } // End FOR

    // Save the total function count in the first token
    lpYYMem0->TknCount = TotalTknCount;

    // Return as the overall total
    *lpTknCount += TotalTknCount;

    Assert (YYCheckInuse (lpYYMem));

    return lpYYMem;
} // End YYCopyFcn


//***************************************************************************
//  $YYCopyGlbFcn_PTB
//
//  Copy an HGLOBAL to a memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- YYCopyGlbFcn_PTB"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE YYCopyGlbFcn_PTB
    (LPPL_YYSTYPE  lpYYMem,             // Ptr to result memory object
     LPTOKEN       lpToken,             // Ptr to function token
     LPPL_YYSTYPE  lpYYArgI,            // Ptr to function arg
     LPINT         lpTknCount)          // Ptr to resulting token count

{
    HGLOBAL           hGlbFcn;          // Function global memory handle
    PL_YYSTYPE        YYFcn = {0};      // Temporary YYSTYPE
    UBOOL             bYYFcn;           // TRUE iff we're using YYFcn
    LPFCNARRAY_HEADER lpMemHdrFcn;      // Ptr to function array header global memory
    LPPL_YYSTYPE      lpMemFcn;         // Ptr to function array global memory
    int               TknCount = 1;     // # tokens added for this element of the function strand

    // Get the global memory handle
    hGlbFcn = MakePtrTypeGlb (GetGlbHandle (lpToken));

    // Split cases based upon the function signature
    switch (GetSignatureGlb_PTB (hGlbFcn))
    {
        case DFN_HEADER_SIGNATURE:
            // Increment the reference count in global memory
            DbgIncrRefCntDir_PTB (hGlbFcn);

            // Fill in the token
            YYFcn.tkToken.tkFlags.TknType   = TKT_FCNARRAY;
////////////YYFcn.tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;    // Already zero from = {0}
////////////YYFcn.tkToken.tkFlags.NoDisplay = FALSE;            // Already zero from = {0}
            YYFcn.tkToken.tkData.tkGlbData  = hGlbFcn;
            YYFcn.tkToken.tkCharIndex       = lpYYArgI->tkToken.tkCharIndex;
            YYFcn.TknCount                  = TknCount;
////////////YYFcn.YYInuse                   = FALSE;            // (Factored out below)
////////////YYFcn.YYIndirect                = FALSE;            // Already zero from = {0}
////////////YYFcn.YYCopyArray               = 0;                // Already zero from = {0}
////////////YYFcn.YYAvail                   = 0;                // Already zero from = {0}
////////////YYFcn.YYIndex                   = 0;                // (Factored out below)
////////////YYFcn.YYFlag                    = 0;                // Already zero from = {0}
////////////YYFcn.lpYYFcnBase               = NULL;             // Already zero from = {0}
            YYFcn.lpYYStrandBase            = lpYYArgI->lpYYStrandBase;

            // Mark as using YYFcn
            bYYFcn = TRUE;

            break;

        case FCNARRAY_HEADER_SIGNATURE:
        {
            UINT uLen,              // # tokens in the array
                 uCnt;              // Loop counter

            Assert (GetPtrTypeDir (hGlbFcn) EQ PTRTYPE_HGLOBAL);

            // In case this is a Train with SRCIFlag set, ...
            DbgIncrRefCntDir_PTB (hGlbFcn);

            // Lock the memory to get a ptr to it
            lpMemHdrFcn = MyGlobalLock (hGlbFcn);

            // Get the token count
            uLen = lpMemHdrFcn->tknNELM;

            // Skip over the header to the data
            lpMemFcn = FcnArrayBaseToData (lpMemHdrFcn);

            // If this function array is a Train, ...
            if (lpMemHdrFcn->fnNameType EQ NAMETYPE_TRN)
            {
                // Count in one token
                TknCount = 1;

                // Count in another use
                DbgIncrRefCntDir_PTB (MakePtrTypeGlb (hGlbFcn));

                // Fill in the token
                YYFcn.tkToken.tkFlags.TknType   = TKT_FCNARRAY;
////////////////YYFcn.tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;    // Already zero from = {0}
////////////////YYFcn.tkToken.tkFlags.NoDisplay = FALSE;            // Already zero from = {0}
                YYFcn.tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbFcn);
                YYFcn.tkToken.tkCharIndex       = lpYYArgI->tkToken.tkCharIndex;
                YYFcn.TknCount                  = TknCount;
////////////////YYFcn.YYInuse                   = FALSE;            // (Factored out below)
////////////////YYFcn.YYIndirect                = FALSE;            // Already zero from = {0}
////////////////YYFcn.YYCopyArray               = 0;                // Already zero from = {0}
////////////////YYFcn.YYAvail                   = 0;                // Already zero from = {0}
////////////////YYFcn.YYIndex                   = 0;                // (Factored out below)
////////////////YYFcn.YYFlag                    = 0;                // Already zero from = {0}
////////////////YYFcn.lpYYFcnBase               = NULL;             // Already zero from = {0}
                YYFcn.lpYYStrandBase            = lpYYArgI->lpYYStrandBase;

                // Mark as using YYFcn
                bYYFcn = TRUE;
            } else
            {
                // Initialize the token count
                TknCount = 0;

                // Loop through the function array
                for (uCnt = 0; uCnt < uLen; uCnt++, lpMemFcn++)
                // Split off immediates so as not to double count TKT_OP1IMMED/INDEX_OPTRAINs
                if (IsTknImmed (&lpMemFcn->tkToken))
                {
                    // Copy the item to the result
                    *lpYYMem++ = *lpMemFcn;
                    TknCount++;
                } else
                    // Recurse down through this function array item
                    lpYYMem = YYCopyGlbFcn_PTB (lpYYMem, &lpMemFcn->tkToken, lpYYArgI, &TknCount);
                // Mark as not using YYFcn
                bYYFcn = FALSE;
            } // End IF/ELSE

            // We no longer need this ptr
            MyGlobalUnlock (hGlbFcn); lpMemHdrFcn = NULL; lpMemFcn = NULL;

            // In case this is a Train with SRCIFlag set, ...
            FreeResultGlobalDFLV (hGlbFcn);

            break;
        } // End FCNARRAY_HEADER_SIGNATURE

        case VARARRAY_HEADER_SIGNATURE:
            // Increment the reference count in global memory
            DbgIncrRefCntDir_PTB (hGlbFcn);

            // Split cases based upon the token type
            switch (lpToken->tkFlags.TknType)
            {
                case TKT_VARNAMED:
                    // Fill in the token
                    YYFcn.tkToken.tkFlags.TknType   = lpToken->tkFlags.TknType;
                    YYFcn.tkToken.tkData.tkSym      = lpToken->tkData.tkSym;

                    break;

                case TKT_CHRSTRAND:
                case TKT_NUMSTRAND:
                case TKT_NUMSCALAR:
                case TKT_VARARRAY:
                case TKT_AXISARRAY:
                    // Fill in the token
                    YYFcn.tkToken.tkFlags.TknType   = lpToken->tkFlags.TknType;
                    YYFcn.tkToken.tkData.tkGlbData  = hGlbFcn;

                    break;

                defstop
                    break;
            } // End SWITCH

            // Fill in the token
////////////YYFcn.tkToken.tkFlags.TknType   =                   // Done in above SWITCH stmt
////////////YYFcn.tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;    // Already zero from = {0}
////////////YYFcn.tkToken.tkFlags.NoDisplay = FALSE;            // Already zero from = {0}
////////////YYFcn.tkToken.tkData.tkGlbData  =                   // Done in above SWITCH stmt
            YYFcn.tkToken.tkCharIndex       = lpYYArgI->tkToken.tkCharIndex;
            YYFcn.TknCount                  = TknCount;
////////////YYFcn.YYInuse                   = FALSE;            // (Factored out below)
////////////YYFcn.YYIndirect                = FALSE;            // Already zero from = {0}
////////////YYFcn.YYCopyArray               = 0;                // Already zero from = {0}
////////////YYFcn.YYAvail                   = 0;                // Already zero from = {0}
////////////YYFcn.YYIndex                   = 0;                // (Factored out below)
////////////YYFcn.YYFlag                    = 0;                // Already zero from = {0}
////////////YYFcn.lpYYFcnBase               = NULL;             // Already zero from = {0}
            YYFcn.lpYYStrandBase            = lpYYArgI->lpYYStrandBase;

            // Mark as using YYFcn
            bYYFcn = TRUE;

            break;

        defstop
            break;
    } // End SWITCH

    // If we used YYFcn (and thus didn't already copy the function to lpYYMem)
    if (bYYFcn)
    {
        YYFcn.YYInuse  = FALSE;
#ifdef DEBUG
        YYFcn.YYIndex  = NEG1U;
#endif
        // Copy the destination YYInuse flag in case it's
        //   in the YYRes allocated region
        YYFcn.YYInuse  = lpYYMem->YYInuse;
        YYFcn.TknCount = TknCount;

        // Copy to the destination
        *lpYYMem++ = YYFcn;
        Assert (YYCheckInuse (&lpYYMem[-1]));
    } // End IF

    // Return as the overall total
    *lpTknCount += TknCount;

    return lpYYMem;
} // End YYCopyGlbFcn_PTB
#undef  APPEND_NAME


//***************************************************************************
//  $YYFreeArray
//
//  Free YYCopyArray'ed elements recursively
//***************************************************************************

void YYFreeArray
    (LPPL_YYSTYPE lpYYArg)

{
    UINT    uLen,                   // Token count in this function strand
            uCnt;                   // Loop counter
    HGLOBAL hGlbFcn;                // Function array global memory handle

    // Get the token count in this function strand
    uLen = lpYYArg->TknCount;

    // Loop through the elements
    for (uCnt = 0; uCnt < uLen; uCnt++)
    {
        // If the function is indirect, recurse
        if (lpYYArg[uCnt].YYIndirect)
            YYFreeArray (lpYYArg[uCnt].lpYYFcnBase);
        else
        if (lpYYArg[uCnt].tkToken.tkFlags.TknType EQ TKT_FCNARRAY)
        {
            // Get the global memory handle or function address if direct
            hGlbFcn = lpYYArg[uCnt].tkToken.tkData.tkGlbData;

            // If it hasn't already been erased, ...
            if (!PtrReusedDir (hGlbFcn))
            {
                // tkData is an HGLOBAL
                Assert (GetPtrTypeDir (hGlbFcn) EQ PTRTYPE_HGLOBAL);

                // stData is a valid HGLOBAL function array
                //   or user-defined function/operator
                Assert (IsGlbTypeFcnDir_PTB (hGlbFcn)
                     || IsGlbTypeDfnDir_PTB (hGlbFcn));

                // Recurse through the function array
                YYFreeGlbFcn (hGlbFcn);
            } // End IF
        } // End IF

        if (lpYYArg[uCnt].YYCopyArray
         && !lpYYArg[uCnt].YYIndirect)
            // Free the storage
            FreeResult (&lpYYArg[uCnt]);
    } // End FOR
} // End YYFreeArray


//***************************************************************************
//  $YYFreeGlbFcn
//
//  Free YYCopyArray'ed elements recursively
//***************************************************************************

void YYFreeGlbFcn
    (HGLOBAL hGlbFcn)                   // Function global memory handle

{
    UINT              uLen,             // # tokens in the array
                      uCnt;             // Loop counter
    LPFCNARRAY_HEADER lpMemHdrFcn;      // Ptr to function array header global memory
    LPPL_YYSTYPE      lpMemFcn;         // Ptr to function array global memory

    // Split cases based upon the function signature
    switch (GetSignatureGlb_PTB (hGlbFcn))
    {
        case DFN_HEADER_SIGNATURE:
        case VARARRAY_HEADER_SIGNATURE:
            break;

        case FCNARRAY_HEADER_SIGNATURE:
            // Lock the memory to get a ptr to it
            lpMemHdrFcn = MyGlobalLock (hGlbFcn);

            // Get the token count
            uLen = lpMemHdrFcn->tknNELM;

            // Skip over the header to the data
            lpMemFcn = FcnArrayBaseToData (lpMemHdrFcn);

            // Loop through the function array
            for (uCnt = 0; uCnt < uLen; uCnt++, lpMemFcn++)
            {
                // If the token is named, ...
                //   and it's a direct function, ...
                if (IsTknNamed (&lpMemFcn->tkToken)
                 && lpMemFcn->tkToken.tkData.tkSym->stFlags.FcnDir)
                    continue;

                // Split off immediates so as not to double count TKT_OP1IMMED/INDEX_OPTRAINs
                if (!IsTknImmed (&lpMemFcn->tkToken))
                    // Recurse down through this function array item
                    YYFreeGlbFcn (lpMemFcn->tkToken.tkData.tkGlbData);

                if (lpMemFcn->YYCopyArray
                 && !lpMemFcn->YYIndirect)
                    // Free the storage
                    FreeResult (lpMemFcn);
            } // End FOR

            // We no longer need this ptr
            MyGlobalUnlock (hGlbFcn); lpMemHdrFcn = NULL; lpMemFcn = NULL;

            break;

        defstop
            break;
    } // End SWITCH
} // End YYFreeGlbFcn


#if 0
//***************************************************************************
//  $IncrFcnTkn
//
//  Increment reference counts in a function array
//***************************************************************************

void IncrFcnTkn
    (LPTOKEN lptkSrc)                   // Ptr to source token

{
    HGLOBAL           hGlbFcn;          // Source global memory handle
    LPFCNARRAY_HEADER lpMemHdrFcn;      // Ptr to function array header global memory

    // Get the global memory handle
    hGlbFcn = lptkSrc->tkData.tkGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrFcn = MyGlobalLock (hGlbFcn);

    // Increment function array reference counts
    IncrFcnMem (lpMemHdrFcn);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbFcn); lpMemHdrFcn = NULL;
} // End IncrFcnTkn
#endif


#if 0
//***************************************************************************
//  $IncrFcnMem
//
//  Increment reference counts in a function array
//***************************************************************************

void IncrFcnMem
    (LPVOID lpMemSrc)                   // Ptr to source array

{
    UINT         TknCount,              // # tokens in the function array
                 uCnt;                  // Loop counter
    LPPL_YYSTYPE lpMemFcn;              // Ptr to function array global memory
    HGLOBAL      hGlbItm;               // Item global memory handle
    LPVOID       lpMemItm;              // Ptr to item global memory

    // Split cases based upon the array signature
    switch (GetSignatureMem (lpMemSrc))
    {
        case FCNARRAY_HEADER_SIGNATURE:
            // Get the token count
            TknCount = ((LPFCNARRAY_HEADER) lpMemSrc)->tknNELM;

            // Skip over the header to the data
            lpMemFcn = FcnArrayBaseToData (lpMemSrc);

            // Loop through the source function array
            for (uCnt = 0; uCnt < TknCount; uCnt++, lpMemFcn++)
            // Split cases based upon the item token type
            switch (lpMemFcn->tkToken.tkFlags.TknType)
            {
                case TKT_FCNARRAY:
                case TKT_VARARRAY:
                case TKT_CHRSTRAND:
                case TKT_NUMSTRAND:
                case TKT_NUMSCALAR:
                    // Get the item global memory handle
                    hGlbItm = lpMemFcn->tkToken.tkData.tkGlbData;

                    // Increment the reference count
                    DbgIncrRefCntDir_PTB (hGlbItm);

                    // Lock the memory to get a ptr to it
                    lpMemItm = MyGlobalLock (hGlbItm);

                    // If the item is a UDFO, avoid double increment
                    if (GetSignatureMem (lpMemItm) NE DFN_HEADER_SIGNATURE)
                        // Recurse in case there are more functions
                        IncrFcnMem (lpMemItm);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbItm); lpMemItm = NULL;

                    break;

                case TKT_AXISIMMED:
                case TKT_VARIMMED:
                case TKT_FCNIMMED:
                case TKT_OP1IMMED:
                case TKT_OP2IMMED:
                case TKT_OP3IMMED:
                case TKT_OPJOTDOT:
                case TKT_FILLJOT:
                    break;

                case TKT_FCNNAMED:
                    // Get the item global memory handle
                    hGlbItm = lpMemFcn->tkToken.tkData.tkSym->stData.stGlbData;

                    // Increment the reference count
                    DbgIncrRefCntDir_PTB (hGlbItm);

                    // Lock the memory to get a ptr to it
                    lpMemItm = MyGlobalLock (hGlbItm);

                    // If the item is a UDFO, avoid double increment
                    if (GetSignatureMem (lpMemItm) NE DFN_HEADER_SIGNATURE)
                        // Recurse in case there are more functions
                        IncrFcnMem (lpMemItm);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbItm); lpMemItm = NULL;

                    break;

                defstop
                    break;
            } // End FOR/SWITCH

            break;

        case DFN_HEADER_SIGNATURE:
            // Increment the reference count
            DbgIncrRefCntDir_PTB (MakePtrTypeGlb (MyGlobalHandle (lpMemSrc)));

            break;

        case VARARRAY_HEADER_SIGNATURE:
            break;

        defstop
            break;
    } // End SWITCH
} // End IncrFcnMem
#endif


#ifdef DEBUG
//***************************************************************************
//  $YYCheckInuse
//
//  Debug routine to check YYInuse flag
//***************************************************************************

UBOOL YYCheckInuse
    (LPPL_YYSTYPE lpYYRes)

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // If it's within the allocated range, ...
    if (lpMemPTD->lpYYRes <= lpYYRes
     &&                      lpYYRes < &lpMemPTD->lpYYRes[lpMemPTD->numYYRes])
        return lpYYRes->YYInuse;
    else
        return TRUE;
} // End YYCheckInuse
#endif


//***************************************************************************
//  End of File: yyfns.c
//***************************************************************************
