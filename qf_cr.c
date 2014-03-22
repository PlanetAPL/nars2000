//***************************************************************************
//  NARS2000 -- System Function -- Quad CR
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
#include "debug.h"              // For xxx_TEMP_OPEN macros


//***************************************************************************
//  $SysFnCR_EM_YY
//
//  System function:  []CR -- Canonical Representation
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnCR_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnCR_EM_YY
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
        goto AXIS_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonCR_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydCR_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);

AXIS_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnCR_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonCR_EM_YY
//
//  Monadic []CR -- Canonical Representation
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonCR_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonCR_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return SysFnCR_Common_EM_YY (2,                     // Result rank
                                 lptkFunc,              // Ptr to function token
                                 lptkRhtArg,            // Ptr to right arg token
                                 lptkAxis);             // Ptr to axis token (may be NULL)
} // End SysFnMon_CR_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnCR_Common_EM_YY
//
//  Monadic []CR -- Canonical Representation, for vector and matrix results
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnCR_Common_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnCR_Common_EM_YY
    (APLRANK aplRankRes,                // Result rank
     LPTOKEN lptkFunc,                  // Ptr to function token
     LPTOKEN lptkRhtArg,                // Ptr to right arg token
     LPTOKEN lptkAxis)                  // Ptr to axis token (may be NULL)

{
    LPPERTABDATA   lpMemPTD;            // Ptr to PerTabData global memory
    LPWCHAR        lpwszTemp;           // Ptr to temporary storage
    APLSTYPE       aplTypeRht,          // Right arg storage type
                   aplTypeRes;          // Result    ...
    APLNELM        aplNELMRht,          // Right arg NELM
                   aplNELMRes;          // Result    ...
    APLRANK        aplRankRht;          // Right arg rank
    HGLOBAL        hGlbRht = NULL,      // Right arg global memory handle
                   hGlbRes = NULL,      // Result    ...
                   hGlbData = NULL,     // Data      ...
                   hGlbTxtLine;         // Line text ...
    LPAPLCHAR      lpMemRht = NULL;     // Ptr to right arg global memory
    LPVOID         lpMemRes = NULL,     // Ptr to result    ...
                   lpMemData = NULL;    // Ptr to function data ...
    LPMEMTXT_UNION lpMemTxtLine;        // Ptr to header/line text global memory
    UINT           uRes;                // Loop counter
    APLLONGEST     aplLongestRht;       // Right arg longest if immediate
    LPSYMENTRY     lpSymEntry;          // Ptr to SYMENTRY
    STFLAGS        stFlags = {0};       // STE flags
    LPPL_YYSTYPE   lpYYRes = NULL;      // Ptr to the result
    VARS_TEMP_OPEN

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get ptr to temporary storage
    lpwszTemp = lpMemPTD->lpwszTemp;
    CHECK_TEMP_OPEN

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
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // If the right arg is global, ...
    if (hGlbRht)
        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemRht);
    else
        // Point to the right arg data
        lpMemRht = (LPAPLCHAR) &aplLongestRht;

    // Delete leading blanks
    while (isspaceW (*lpMemRht))
    {
        lpMemRht++;
        aplNELMRht--;
    } // End WHILE

    // Delete trailing blanks
    while (aplNELMRht && isspaceW (lpMemRht[aplNELMRht - 1]))
        aplNELMRht--;

    // Lookup the name in the symbol table
    // SymTabLookupName sets the .ObjName enum,
    //   and the .Inuse flag
    lpSymEntry =
      SymTabLookupNameLength (lpMemRht,
                   (APLU3264) aplNELMRht,
                             &stFlags);
    // Mark the result type & rank
    aplTypeRes = ARRAY_CHAR;

    // If not found,
    //   or it's a System Name,
    //   or without a value,
    //   return empty vector or 0 x 0 char matrix
    if (!lpSymEntry
     ||  lpSymEntry->stFlags.ObjName EQ OBJNAME_SYS
     || !lpSymEntry->stFlags.Value)
        // Not the signature of anything we know
        // Return an empty nested char vector or a 0 x 0 char matrix
        hGlbRes = SysFnMonCR_ALLOC_EM (0, aplRankRes, NULL, lptkFunc);
    else
    // If it's immediate, ...
    if (lpSymEntry->stFlags.Imm)
        // Finish the job via subroutine
        hGlbRes = SysFnMonCR_ALLOC_EM (1, aplRankRes, &lpSymEntry->stData.stChar, lptkFunc);
    else
    {
        // Check for internal functions
        if (lpSymEntry->stFlags.FcnDir)
        {
            // Append the function name from the symbol table
            CopySteName (lpwszTemp, lpSymEntry, &aplNELMRes);

            // Finish the job via subroutine
            hGlbRes = SysFnMonCR_ALLOC_EM (aplNELMRes, aplRankRes, lpwszTemp, lptkFunc);
        } else
        {
            // Get the global memory ptr
            hGlbData = lpSymEntry->stData.stGlbData;

            // Lock the memory to get a ptr to it
            lpMemData = MyGlobalLock (hGlbData);

            // Split cases based upon the array type
            switch (GetSignatureMem (lpMemData))
            {
                case FCNARRAY_HEADER_SIGNATURE:
                    // Get the line text global memory handle
                    hGlbTxtLine = ((LPFCNARRAY_HEADER) lpMemData)->hGlbTxtLine;

                    // Lock the memory to get a ptr to it
                    lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

                    // Get the length of the line text
                    aplNELMRes = lpMemTxtLine->U;

                    // Copy the function line text to global memory
                    CopyMemoryW (lpwszTemp, &lpMemTxtLine->C, (APLU3264) aplNELMRes);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;

                    // Finish the job via subroutine
                    hGlbRes = SysFnMonCR_ALLOC_EM (aplNELMRes, aplRankRes, lpwszTemp, lptkFunc);

                    break;

                case DFN_HEADER_SIGNATURE:
                {
                    LPDFN_HEADER  lpMemDfnHdr;      // Ptr to user-defined function/operator header ...
                    LPFCNLINE     lpFcnLines;       // Ptr to array of function line structs (FCNLINE[numFcnLines])
                    UINT          uNumLines,        // # function lines
                                  uLine,            // Loop counter
                                  uMaxLineLen;      // Length of the longest line
                    APLUINT       ByteRes;          // # bytes in the result

                    // Get ptr to user-defined function/operator header
                    lpMemDfnHdr = (LPDFN_HEADER) lpMemData;

                    // If this is an AFO, ...
                    if (lpMemDfnHdr->bAFO)
                    {
                        // Get ptr to array of function line structs (FCNLINE[numFcnLines])
                        lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

                        // Get the line text global memory handle
                        hGlbTxtLine = lpFcnLines->hGlbTxtLine;

                        // Lock the memory to get a ptr to it
                        lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

                        // Get the length of the line text
                        aplNELMRes = lpMemTxtLine->U;

                        // Start with a leading left brace
                        lpwszTemp[0] = UTF16_LEFTBRACE;

                        // Copy the function line text to global memory
                        CopyMemoryW (&lpwszTemp[1], &lpMemTxtLine->C, (APLU3264) aplNELMRes);

                        // End with a trailing right brace
                        lpwszTemp[1 + aplNELMRes] = UTF16_RIGHTBRACE;

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;

                        // Finish the job via subroutine
                        hGlbRes = SysFnMonCR_ALLOC_EM (aplNELMRes + 2, aplRankRes, lpwszTemp, lptkFunc);
                    } else
                    {
                        // Lock the memory to get a ptr to it
                        lpMemTxtLine = MyGlobalLock (lpMemDfnHdr->hGlbTxtHdr);

                        // Get the length of the function header text
                        uMaxLineLen = lpMemTxtLine->U;

                        // We no longer need this ptr
                        MyGlobalUnlock (lpMemDfnHdr->hGlbTxtHdr); lpMemTxtLine = NULL;

                        // Get ptr to array of function line structs (FCNLINE[numFcnLines])
                        lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

                        // Get # function lines
                        uNumLines = lpMemDfnHdr->numFcnLines;

                        // If the result is a matrix, ...
                        if (IsMatrix (aplRankRes))
                        {
                            // Run through the function lines looking for the longest
                            for (uLine = 0; uLine < uNumLines; uLine++)
                            {
                                // Get the line text global memory handle
                                hGlbTxtLine = lpFcnLines->hGlbTxtLine;

                                if (hGlbTxtLine)
                                {
                                    // Lock the memory to get a ptr to it
                                    lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

                                    // Find the length of the longest line
                                    uMaxLineLen = max (uMaxLineLen, lpMemTxtLine->U);

                                    // We no longer need this ptr
                                    MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;
                                } // End IF

                                // Skip to the next struct
                                lpFcnLines++;
                            } // End FOR
                        } else
                            aplTypeRes = ARRAY_NESTED;

                        // Calculate the result NELM ("1 +" includes the header)
                        aplNELMRes = 1 + uNumLines;
                        if (IsMatrix (aplRankRes))
                            aplNELMRes *= uMaxLineLen;

                        // Calculate space needed for the result
                        ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

                        // Check for overflow
                        if (ByteRes NE (APLU3264) ByteRes)
                            goto WSFULL_EXIT;

                        // Allocate space for the result
                        hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
                        if (!hGlbRes)
                            goto WSFULL_EXIT;

                        // Lock the memory to get a ptr to it
                        lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
                        // Fill in the header
                        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
                        lpHeader->ArrType    = aplTypeRes;
////////////////////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////////////////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
                        lpHeader->RefCnt     = 1;
                        lpHeader->NELM       = aplNELMRes;
                        lpHeader->Rank       = aplRankRes;
#undef  lpHeader

                        // Save the dimensions in the result ("1 +" includes the header)
                        (VarArrayBaseToDim (lpMemRes))[0] = 1 + uNumLines;
                        if (IsMatrix (aplRankRes))
                            (VarArrayBaseToDim (lpMemRes))[1] = uMaxLineLen;

#define lpMemResChar    ((LPAPLCHAR) lpMemRes)
                        // Skip over the header and dimensions to the data
                        lpMemResChar = VarArrayDataFmBase (lpMemRes);

                        // If this is a nested result, fill it in PTR_REUSED
                        //   in case we fail along the way
                        if (IsVector (aplRankRes))
                        for (uRes = 0; uRes < aplNELMRes; uRes++)
                            ((LPAPLNESTED) lpMemResChar)[uRes] = PTR_REUSED;

                        // Copy the header to the result as either a row or as an allocated HGLOBAL
                        lpMemResChar = SysFnCR_Copy_EM (aplRankRes, lpMemResChar, lpMemDfnHdr->hGlbTxtHdr, uMaxLineLen, lptkFunc);
                        if (lpMemResChar EQ NULL)
                            goto ERROR_EXIT;

                        // Get ptr to array of function line structs (FCNLINE[numFcnLines])
                        lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

                        // Run through the function lines copying each line text to the result
                        for (uLine = 0; uLine < uNumLines; uLine++)
                        {
                            // Get the line text global memory handle
                            hGlbTxtLine = lpFcnLines->hGlbTxtLine;

                            if (hGlbTxtLine)
                            {
                                // Copy the line text to the result as either a row or as an allocated HGLOBAL
                                lpMemResChar = SysFnCR_Copy_EM (aplRankRes, lpMemResChar, hGlbTxtLine, uMaxLineLen, lptkFunc);
                                if (lpMemResChar EQ NULL)
                                    goto ERROR_EXIT;
                            } // End IF
#undef  lpMemResChar
                            // Skip to the next struct
                            lpFcnLines++;
                        } // End FOR
                    } // End IF/ELSE

                    break;
                } // End DFN_HEADER_SIGNATURE

                case VARARRAY_HEADER_SIGNATURE:
                    // Return an empty nested char vector or a 0 x 0 char matrix
                    hGlbRes = SysFnMonCR_ALLOC_EM (0, aplRankRes, NULL, lptkFunc);

                    break;

                defstop
                    break;
            } // End SWITCH

            // We no longer need this ptr
            MyGlobalUnlock (hGlbData); lpMemData = NULL;
        } // End IF/ELSE
    } // End IF/ELSE

    if (!hGlbRes)
        goto ERROR_EXIT;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

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
    if (hGlbData && lpMemData)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbData); lpMemData = NULL;
    } // End IF

    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    EXIT_TEMP_OPEN

    return lpYYRes;
} // End SysFnCR_Common_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnCR_Copy_EM
//
//  Subroutine to SysFnCR_Common_EM for copying a line to the result
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnCR_Copy_EM_YY"
#else
#define APPEND_NAME
#endif

LPVOID SysFnCR_Copy_EM
    (APLRANK aplRankRes,                    // Result rank
     LPVOID  lpMemRes,                      // Ptr to result
     HGLOBAL hGlbTxt,                       // Header/line text global memory handle
     UINT    uMaxLineLen,                   // Maximum line length (valid only with IsMatrix (aplRankRes))
     LPTOKEN lptkFunc)                      // Ptr to function token

{
    LPMEMTXT_UNION lpMemTxtLine;        // Ptr to header/line text global memory
    UINT           uLineLen;            // Length of a text line
    HGLOBAL        hGlbCpy;             // Copy of header/line text global memory handle
    LPVOID         lpMemCpy;            // Ptr to header/line text global memory
    APLUINT        ByteRes;             // # bytes in the result

    // Lock the memory to get a ptr to it
    lpMemTxtLine = MyGlobalLock (hGlbTxt);

    // Split cases based upon the result rank (1 or 2)
    if (IsVector (aplRankRes))
    {
        // Calculate space needed for the result
        ByteRes = CalcArraySize (ARRAY_CHAR, lpMemTxtLine->U, 1);

        // Check for overflow
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;

        // Allocate space for the result.
        hGlbCpy = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (!hGlbCpy)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemCpy = MyGlobalLock (hGlbCpy);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemCpy)
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = ARRAY_CHAR;
////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////lpHeader->SysVar     = FALSE;       // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = lpMemTxtLine->U;
        lpHeader->Rank       = 1;
#undef  lpHeader

        // Save the dimension
        *VarArrayBaseToDim (lpMemCpy) = lpMemTxtLine->U;

        // Skip over the header and dimensions to the data
        lpMemCpy = VarArrayDataFmBase (lpMemCpy);

        // Copy the text
        CopyMemoryW (lpMemCpy, &lpMemTxtLine->C, lpMemTxtLine->U);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbCpy); lpMemCpy = NULL;

        // Save the HGLOBAL in the result and skip over it
        *((LPAPLNESTED) lpMemRes)++ = MakePtrTypeGlb (hGlbCpy);
    } else
    {
        // Get the length of the function header/line text
        uLineLen = lpMemTxtLine->U;
#define lpMemResChar        ((LPAPLCHAR) lpMemRes)
        // Copy the function header/line text to the result
        CopyMemoryW (lpMemResChar, &lpMemTxtLine->C, uLineLen);

        // Fill the remainder of the line with blanks
        // Could use FillMemoryW ??
        if (IsMatrix (aplRankRes))
        for (lpMemResChar += uLineLen;
             uLineLen < uMaxLineLen;
             uLineLen++)
            *lpMemResChar++ = L' ';
#undef  lpMemResChar
    } // End IF/ELSE

    goto NORMAL_EXIT;

WSFULL_EXIT:
    // Mark as in error
    lpMemRes = NULL;

    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbTxt); lpMemTxtLine = NULL;

    return lpMemRes;
} // End SysFnCR_Copy_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonCR_ALLOC_EM
//
//  Subroutine to SysFnCR_Common_EM for allocating and copying to the result
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonCR_ALLOC_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL SysFnMonCR_ALLOC_EM
    (APLNELM   aplNELMRes,      // Result NELM
     APLRANK   aplRankRes,      // ...    rank
     LPAPLCHAR lpw,             // Ptr to result text
     LPTOKEN   lptkFunc)        // Ptr to function token

{
    APLSTYPE   aplTypeRes;      // Result storage type
    HGLOBAL    hGlbRes;         // Result global memory handle
    LPVOID     lpMemRes;        // Ptr to result    ...
    APLUINT    ByteRes;         // # bytes in the result

    // If the result is an empty vector, ...
    if (IsEmpty (aplNELMRes)
     && IsVector (aplRankRes))
        // The result type is nested
        aplTypeRes = ARRAY_NESTED;
    else
        // The result type is simple char
        aplTypeRes = ARRAY_CHAR;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // If it's a non-empty matrix, the first
    //   dimension is 1
    if (IsMatrix (aplRankRes)
     && !IsEmpty (aplNELMRes))
    {
        // Save the dimension in the result
        (VarArrayBaseToDim (lpMemRes))[0] = 1;

        // Save the dimension in the result
        (VarArrayBaseToDim (lpMemRes))[1] = aplNELMRes;
    } else
        // Save the dimension in the result
        *VarArrayBaseToDim (lpMemRes) = aplNELMRes;

    // Skip over the header and dimensions to the result
    lpMemRes = VarArrayDataFmBase (lpMemRes);

    // Ensure the source is valid
    if (lpw)
        // Copy the function text to the result
        CopyMemoryW (lpMemRes, lpw, (APLU3264) aplNELMRes);
    else
    if (IsNested (aplTypeRes))
        // Fill in the result prototype
        *((LPAPLNESTED) lpMemRes) = MakePtrTypeGlb (hGlbV0Char);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    return hGlbRes;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End SysFnMonCR_ALLOC_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CopySteName
//
//  Copy a STE name
//***************************************************************************

LPAPLCHAR CopySteName
    (LPAPLCHAR  lpMemRes,       // Ptr to result global memory
     LPSYMENTRY lpSymEntry,     // Ptr to function symbol table entry
     LPAPLNELM  lpaplNELM)      // Ptr to name length (may be NULL)

{
    LPAPLCHAR lpMemName;        // Ptr to function name global memory
    UINT      uNameLen;         // Length of STE name

    Assert (IsValidPtr    (lpSymEntry                       , sizeof (lpSymEntry)            ));
    Assert (IsValidPtr    (lpSymEntry->stHshEntry           , sizeof (lpSymEntry->stHshEntry)));
    Assert (IsValidHandle (lpSymEntry->stHshEntry->htGlbName                                 ));

    // Lock the memory to get a ptr to it
    lpMemName = MyGlobalLock (lpSymEntry->stHshEntry->htGlbName);

    // Get the name length
    uNameLen = lstrlenW (lpMemName);

    // Copy the name to the output area
    CopyMemoryW (lpMemRes, lpMemName, uNameLen);

    // Skip over the name
    lpMemRes += uNameLen;

    // Return the name length, if requested
    if (lpaplNELM)
        *lpaplNELM = uNameLen;

    // We no longer need this ptr
    MyGlobalUnlock (lpSymEntry->stHshEntry->htGlbName); lpMemName = NULL;

    return lpMemRes;
} // End CopySteName


//***************************************************************************
//  $ConvSteName
//
//  Copy and convert a STE name
//***************************************************************************

LPAPLCHAR ConvSteName
    (LPAPLCHAR  lpMemRes,       // Ptr to result global memory
     LPSYMENTRY lpSymEntry,     // Ptr to function symbol table entry
     LPAPLNELM  lpaplNELM)      // Ptr to name length (may be NULL)

{
    LPAPLCHAR lpMemName,        // Ptr to function name global memory
              lpMemEnd;         // Ptr to end of the converted name
    UINT      uNameLen;         // Length of STE name

    Assert (IsValidPtr    (lpSymEntry                       , sizeof (lpSymEntry)            ));
    Assert (IsValidPtr    (lpSymEntry->stHshEntry           , sizeof (lpSymEntry->stHshEntry)));
    Assert (IsValidHandle (lpSymEntry->stHshEntry->htGlbName                                 ));

    // Lock the memory to get a ptr to it
    lpMemName = MyGlobalLock (lpSymEntry->stHshEntry->htGlbName);

    // Get the name length
    uNameLen = lstrlenW (lpMemName);

    // Convert the name to {symbol} form
    //   and copy to the output area
    lpMemEnd = lpMemRes;
    lpMemEnd +=
      ConvertWideToNameLength (lpMemRes,    // Ptr to output save buffer
                               lpMemName,   // Ptr to incoming chars
                               uNameLen);   // # chars to convert
    // Return the name length, if requested
    if (lpaplNELM)
        *lpaplNELM = lstrlenW (lpMemRes);

    // We no longer need this ptr
    MyGlobalUnlock (lpSymEntry->stHshEntry->htGlbName); lpMemName = NULL;

    return lpMemEnd;
} // End ConvSteName


//***************************************************************************
//  $SysFnDydCR_EM_YY
//
//  Dyadic []CR -- Canonical Representation (matrix or vector)
//                 1 = return as vector
//                 2 = return as matrix
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydCR_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydCR_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE   aplTypeLft;          // Left arg storage type
    APLNELM    aplNELMLft;          // Left arg NELM
    APLRANK    aplRankLft;          // Left arg rank
    HGLOBAL    hGlbLft = NULL;      // Left arg global memory handle
    APLLONGEST aplLongestLft;       // Left arg immediate value
    UBOOL      bRet = TRUE;         // TRUE iff result is valid

    // Get the attributes (Type, NELM, and Rank)
    //   of the left arg
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);

    // Check for LEFT RANK ERROR
    if (IsMultiRank (aplRankLft))
        goto LEFT_RANK_EXIT;

    // Check for LEFT LENGTH ERROR
    if (!IsSingleton (aplNELMLft))
        goto LEFT_LENGTH_EXIT;

    // Check for LEFT DOMAIN ERROR
    if (!IsNumeric (aplTypeLft))
        goto LEFT_DOMAIN_EXIT;

    // Get left arg's global ptrs
    aplLongestLft = GetGlbPtrs (lptkLftArg, &hGlbLft);

    // If the left arg is a global, ...
    if (hGlbLft)
        // Set the ptr type bits
        hGlbLft = MakePtrTypeGlb (hGlbLft);

    //Split cases based upon the left arg storage type
    switch (aplTypeLft)
    {
        case ARRAY_FLOAT:
            // Attempt to convert the float to an integer using System []CT
            aplLongestLft = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestLft, &bRet);

            break;

        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            bRet = TRUE;

            break;

        case ARRAY_RAT:
            // Attempt to convert the RAT to an integer using System []CT
            aplLongestLft = GetNextRatIntGlb (hGlbLft, 0, &bRet);

            break;

        case ARRAY_VFP:
            // Attempt to convert the VFP to an integer using System []CT
            aplLongestLft = GetNextVfpIntGlb (hGlbLft, 0, &bRet);

            break;

        defstop
            break;
    } // End SWITCH

    // Check for LEFT DOMAIN ERROR
    if (!bRet
     || (aplLongestLft NE 1
      && aplLongestLft NE 2))
        goto LEFT_DOMAIN_EXIT;

    // Call common routine
    return SysFnCR_Common_EM_YY (aplLongestLft,         // Result rank
                                 lptkFunc,              // Ptr to function token
                                 lptkRhtArg,            // Ptr to right arg token
                                 lptkAxis);             // Ptr to axis token (may be NULL)
LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    return NULL;

LEFT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkLftArg);
    return NULL;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
    return NULL;
} // End SysFnDydCR_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_cr.c
//***************************************************************************
