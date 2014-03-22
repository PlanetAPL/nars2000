//***************************************************************************
//  NARS2000 -- System Function -- Quad NL
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
#include "nameclass.h"

// List of valid nameclasses
UCHAR ucNameClassValid [] =
{0,             //  0 = Available name
 1,             //  1 = User label
 1,             //  2 = User variable
 1,             //  3 = User-defined function   (any valence:  0, 1, or 2)
 1,             //  4 = User-defined operator   (either valence:  1 or 2)
 1,             //  5 = System variable
 1,             //  6 = System function         (any valence:  0, 1, or 2)
 0,             //  7 = (Unused)
 0,             //  8 = (Unused)
 0,             //  9 = (Unused)
 0,             // 10 = (Unused)
 0,             // 11 = (Unused)
 0,             // 12 = (Unused)
 0,             // 13 = (Unused)
 0,             // 14 = (Unused)
 0,             // 15 = (Unused)
 0,             // 16 = (Unused)
 0,             // 17 = (Unused)
 0,             // 18 = (Unused)
 0,             // 19 = (Unused)
 0,             // 20 = (Unused)
 1,             // 21 = System label
 0,             // 22 = (Unused)
 1,             // 23 = Magic function          (any valence:  0, 1, or 2)
 1,             // 24 = Magic operator          (either valence:  1 or 2)
};


//***************************************************************************
//  $SysFnNL_EM_YY
//
//  System function:  []NL -- Name List
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnNL_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnNL_EM_YY
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
        return SysFnMonNL_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydNL_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnNL_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonNL_EM_YY
//
//  Monadic []NL -- Name List (by Class)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonNL_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonNL_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return SysFnDydNL_EM_YY (NULL,          // Ptr to left arg token
                             lptkFunc,      // Ptr to function token
                             lptkRhtArg,    // Ptr to right arg token
                             lptkAxis);     // Ptr to axis token (may be NULL)
} // End SysFnMonNL_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydNL_EM_YY
//
//  Dyadic []NL -- Name List (by Alphabet and Class)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydNL_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydNL_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeLft,        // Right arg storage type
                 aplTypeRht;        // Right arg storage type
    APLNELM      aplNELMLft,        // Right arg NELM
                 aplNELMRht;        // Right arg NELM
    APLRANK      aplRankLft,        // Right arg rank
                 aplRankRht;        // Right arg rank
    HGLOBAL      hGlbLft = NULL,    // Left arg global memory handle
                 hGlbRht = NULL,    // Right ...
                 hGlbRes = NULL;    // Result    ...
    LPVOID       lpMemRht = NULL;   // Ptr to right arg global memory
    LPAPLCHAR    lpMemLft = NULL,   // Ptr to left   ...
                 lpMemRes = NULL;   // Ptr to result    ...
    APLUINT      uLft,              // Loop counter
                 uRht,              // Loop counter
                 ByteRes;           // # bytes in the result
    APLLONGEST   aplLongestLft,     // Left arg immediate value
                 aplLongestRht;     // Right ...
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
    APLNELM      uNameLen;          // Length of the current name
    APLUINT      nameClasses = 0;   // Bit flags for each nameclass 1 through (NAMECLASS_LENp1 - 1)
    UINT         uMaxNameLen = 0,   // Length of longest name
                 uSymCnt,           // Count of # matching STEs
                 uSymNum;           // Loop counter
    UBOOL        bRet;              // TRUE iff result is valid
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPSYMENTRY   lpSymEntry;        // Ptr to current SYMENTRY
    LPAPLCHAR    lpMemName;         // Ptr to name global memory
    LPSYMENTRY  *lpSymSort;         // Ptr to LPSYMENTRYs for sorting

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // If there's a left arg, ...
    if (lptkLftArg)
    {
        // Get the attributes (Type, NELM, and Rank)
        //   of the left arg
        AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);

        // Check for LEFT RANK ERROR
        if (IsMultiRank (aplRankLft))
            goto LEFT_RANK_EXIT;

        // Check for LEFT DOMAIN ERROR
        if (!IsSimple (aplTypeLft)
         || (!IsSimpleChar (aplTypeLft)
          && !IsEmpty (aplNELMLft)))
            goto LEFT_DOMAIN_EXIT;

        // Get left arg global ptr
        aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);

        // If the left arg is immediate, point to its one char
        if (hGlbLft EQ NULL)
            lpMemLft = &(APLCHAR) aplLongestLft;
        else
            // Skip over the header and dimensions to the data
            lpMemLft = VarArrayDataFmBase (lpMemLft);

////////// Validate the chars in the left arg
////////for (uLft = 0; uLft < aplNELMLft; uLft++)
////////if (!IzitNameChar (lpMemLft[uLft]))
////////    goto LEFT_DOMAIN_EXIT;
    } // End IF

    // Check for RIGHT RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Check for RIGHT DOMAIN ERROR
    if (!IsNumeric (aplTypeRht))
        goto RIGHT_DOMAIN_EXIT;

    // Get right arg global ptr
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // If the right arg is an HGLOBAL, ...
    if (hGlbRht)
    {
        // Set the ptr type bits
        hGlbRht = MakePtrTypeGlb (hGlbRht);

        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemRht);
    } else
        lpMemRht = &aplLongestRht;

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            // Loop through the right arg elements
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Get the next value as an integer
                aplLongestRht = GetNextInteger (lpMemRht, aplTypeRht, uRht);

                if (aplLongestRht >= NAMECLASS_LENp1
                 || !ucNameClassValid[aplLongestRht])
                    goto RIGHT_DOMAIN_EXIT;

                // Mark as nameclass aplLongestRht
                nameClasses |= (APLINT) (BIT0 << (UINT) aplLongestRht);
            } // End FOR

            break;

        case ARRAY_FLOAT:
            // Loop through the right arg elements
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Get the next value
                // Attempt to convert the float to an integer using System []CT
                aplLongestRht = FloatToAplint_SCT (((LPAPLFLOAT) lpMemRht)[uRht], &bRet);
                if (!bRet
                 || aplLongestRht >= NAMECLASS_LENp1
                 || !ucNameClassValid[aplLongestRht])
                    goto RIGHT_DOMAIN_EXIT;

                // Mark as nameclass aplLongestRht
                nameClasses |= (APLINT) (BIT0 << (UINT) aplLongestRht);
            } // End FOR

            break;

        case ARRAY_RAT:
            // Loop through the right arg elements
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Attempt to convert the RAT to an integer using System []CT
                aplLongestRht = GetNextRatIntGlb (hGlbRht, uRht, &bRet);

                if (!bRet
                 || aplLongestRht >= NAMECLASS_LENp1
                 || !ucNameClassValid[aplLongestRht])
                    goto RIGHT_DOMAIN_EXIT;

                // Mark as nameclass aplLongestRht
                nameClasses |= (APLINT) (BIT0 << (UINT) aplLongestRht);
            } // End FOR

            break;

        case ARRAY_VFP:
            // Loop through the right arg elements
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Attempt to convert the RAT to an integer using System []CT
                aplLongestRht = GetNextVfpIntGlb (hGlbRht, uRht, &bRet);

                if (!bRet
                 || aplLongestRht >= NAMECLASS_LENp1
                 || !ucNameClassValid[aplLongestRht])
                    goto RIGHT_DOMAIN_EXIT;

                // Mark as nameclass aplLongestRht
                nameClasses |= (APLINT) (BIT0 << (UINT) aplLongestRht);
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH

    // Initialize the LPSYMENTRY sort array
    lpSymSort = (LPSYMENTRY *) lpMemPTD->lpwszTemp;

    // Loop through the symbol table looking for STEs
    //   with one of the right arg name classes
    for (lpSymEntry = lpMemPTD->lphtsPTD->lpSymTab, uSymCnt = 0;
         lpSymEntry < lpMemPTD->lphtsPTD->lpSymTabNext;
         lpSymEntry++)
    if (lpSymEntry->stFlags.Inuse                                       // It's in use
     && nameClasses & (APLINT) (BIT0 << CalcNameClass (lpSymEntry)))    // It's in one of the specified name classes
    {
        // Lock the memory to get a ptr to it
        lpMemName = MyGlobalLock (lpSymEntry->stHshEntry->htGlbName);

        // If there's a left arg, ensure the first char of the name
        //   is in the left arg
        if (lptkLftArg)
        {
            for (uLft = 0; uLft < aplNELMLft; uLft++)
            if (lpMemName[0] EQ lpMemLft[uLft])
                break;

            // If there's no match, continue looking
            if (uLft EQ aplNELMLft)
                continue;
        } // End IF

        // Get the name length
        uNameLen = lstrlenW (lpMemName);

        // Find the longest name
        uMaxNameLen = max (uMaxNameLen, (UINT) uNameLen);

        // We no longer need this ptr
        MyGlobalUnlock (lpSymEntry->stHshEntry->htGlbName); lpMemName = NULL;

        // Save the LPSYMENTRY ptr for later use
        lpSymSort[uSymCnt] = lpSymEntry;

        // Count in another matching name
        uSymCnt++;
    } // End FOR/IF

    // Sort the HGLOBALs
    ShellSort (lpSymSort, uSymCnt, CmpLPSYMENTRY);

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_CHAR, uSymCnt * uMaxNameLen, 2);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    //***************************************************************
    // Now we can allocate the storage for the result
    //***************************************************************
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = uSymCnt * uMaxNameLen;
    lpHeader->Rank       = 2;
#undef  lpHeader

    // Fill in the result's dimensions
    (VarArrayBaseToDim (lpMemRes))[0] = uSymCnt;
    (VarArrayBaseToDim (lpMemRes))[1] = uMaxNameLen;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemRes);

    // Loop through the sorted STEs copying the names to the result
    for (uSymNum = 0; uSymNum < uSymCnt; uSymNum++)
    {
        // Copy the STE name to the result
        lpMemRes = CopySteName (lpMemRes, lpSymSort[uSymNum], &uNameLen);

        // Fill in the tail of the name with blanks
        // Could use FillMemoryW ??
        for (uRht = uNameLen; uRht < uMaxNameLen; uRht++)
            *lpMemRes++ = L' ';
    } // End FOR

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
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
    if (hGlbLft && lpMemLft)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

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

    return lpYYRes;
} // End SysFnDydNL_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_nl.c
//***************************************************************************
