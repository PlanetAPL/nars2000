//***************************************************************************
//  NARS2000 -- System Function -- Quad UCS
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
//  $SysFnUCS_EM_YY
//
//  System function:  []UCS -- Universal Character Set
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnUCS_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnUCS_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
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
        return SysFnMonUCS_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydUCS_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnUCS_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonUCS_EM_YY
//
//  Monadic []UCS -- Universal Character Set
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonUCS_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonUCS_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE       aplTypeRht,          // Right arg storage type
                   aplTypeRes,          // Result    ...
                   aplTypeHet;          // Hetero item ...
    APLNELM        aplNELMRht;          // Right arg NELM
    APLRANK        aplRankRht;          // Right arg rank
    HGLOBAL        hGlbRht = NULL,      // Right arg global memory handle
                   hGlbRes = NULL;      // Result    ...
    APLLONGEST     aplLongestRht;       // Right arg longest if immediate
    LPVOID         lpMemRht = NULL,     // Ptr to right arg global memory
                   lpMemRes = NULL;     // Ptr to result    ...
    LPPL_YYSTYPE   lpYYRes = NULL;      // Ptr to the result
    UBOOL          bRet;                // TRUE iff result if valid
    IMM_TYPES      immTypeRes;          // Result immediate type
    APLINT         apaOffRht,           // Right arg APA offset
                   apaMulRht;           // ...           multiplier
    APLUINT        ByteRes,             // # bytes in the result
                   uRht;                // Loop counter
    UINT           uBitMask;            // Bit mask for marching through Booleans
    APLHETERO      aplHeteroRht,        // Right arg value as APLHETERO
                   lpMemHeteroRht;      // Ptr to hetero item
    LPSYMENTRY     lpSymTmp;            // Ptr to temporary LPSYMENTRY

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for DOMAIN ERROR
    if (!IsSimpleGlbNum (aplTypeRht))
        goto DOMAIN_EXIT;

    // Get right arg's global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // If the right arg/result is a scalar, ...
    if (IsScalar (aplRankRht))
    {
        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_FLOAT:
                // Attempt to convert the float to an integer using System []CT
                aplLongestRht = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestRht, &bRet);
                if (!bRet)
                    goto DOMAIN_EXIT;

                // Fall through to common code

            case ARRAY_BOOL:
            case ARRAY_INT:
                // Check for out of range for Unicode
                //   as an APLUINT so we don't have to deal with negatives
                if (MAX_UNICODE < aplLongestRht)
                    goto DOMAIN_EXIT;

                // Check for out of range for UCS-2
                if (APLCHAR_SIZE <= aplLongestRht)
                    aplLongestRht = UTF16_REPLACEMENTCHAR;

                // Set the result immediate type
                immTypeRes = IMMTYPE_CHAR;

                break;

            case ARRAY_CHAR:
                // Set the result immediate type
                immTypeRes = IMMTYPE_INT;

                break;

            case ARRAY_RAT:
                aplLongestRht = mpq_get_sctsx ((LPAPLRAT) VarArrayDataFmBase (lpMemRht), &bRet);
                if (!bRet)
                    goto DOMAIN_EXIT;
                // Check for out of range for Unicode
                //   as an APLUINT so we don't have to deal with negatives
                if (MAX_UNICODE < aplLongestRht)
                    goto DOMAIN_EXIT;

                // Check for out of range for UCS-2
                if (APLCHAR_SIZE <= aplLongestRht)
                    aplLongestRht = UTF16_REPLACEMENTCHAR;

                // Set the result immediate type
                immTypeRes = IMMTYPE_CHAR;

                break;

            case ARRAY_VFP:
                aplLongestRht = mpfr_get_sctsx ((LPAPLVFP) VarArrayDataFmBase (lpMemRht), &bRet);
                if (!bRet)
                    goto DOMAIN_EXIT;
                // Check for out of range for Unicode
                //   as an APLUINT so we don't have to deal with negatives
                if (MAX_UNICODE < aplLongestRht)
                    goto DOMAIN_EXIT;

                // Check for out of range for UCS-2
                if (APLCHAR_SIZE <= aplLongestRht)
                    aplLongestRht = UTF16_REPLACEMENTCHAR;

                // Set the result immediate type
                immTypeRes = IMMTYPE_CHAR;

                break;

            defstop
                break;
        } // End SWITCH

        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = immTypeRes;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkLongest  = aplLongestRht;
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

        goto NORMAL_EXIT;
    } // End IF

    // From here on, the result is a global memory handle

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_APA:
        case ARRAY_RAT:
        case ARRAY_VFP:
            aplTypeRes = ARRAY_CHAR;

            break;

        case ARRAY_CHAR:
            aplTypeRes = ARRAY_INT;

            break;

        case ARRAY_HETERO:
            aplTypeRes = ARRAY_HETERO;

            break;

        defstop
            break;
    } // End SWITCH

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRht, aplRankRht);

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
    lpHeader->NELM       = aplNELMRht;
    lpHeader->Rank       = aplRankRht;
#undef  lpHeader

    // Copy the dimensions to the result
    CopyMemory (VarArrayBaseToDim (lpMemRes),
                VarArrayBaseToDim (lpMemRht),
                (APLU3264) aplRankRht * sizeof (APLDIM));
    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayDataFmBase (lpMemRht);
    lpMemRes = VarArrayDataFmBase (lpMemRes);

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            uBitMask = BIT0;

            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                *((LPAPLCHAR) lpMemRes)++ =
                  (APLCHAR) ((uBitMask & *(LPAPLBOOL) lpMemRht) ? TRUE : FALSE);

                // Shift over the right arg bit mask
                uBitMask <<= 1;

                // Check for end-of-byte
                if (uBitMask EQ END_OF_BYTE)
                {
                    uBitMask = BIT0;            // Start over
                    ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                } // End IF
            } // End FOR

            break;

        case ARRAY_INT:
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Get the next element
                aplLongestRht = *((LPAPLUINT) lpMemRht)++;

                // Check for out of range for Unicode
                //   as an APLUINT so we don't have to deal with negatives
                if (MAX_UNICODE < aplLongestRht)
                    goto DOMAIN_EXIT;

                // Check for out of range for UCS-2
                if (APLCHAR_SIZE <= aplLongestRht)
                    *((LPAPLCHAR) lpMemRes)++ = UTF16_REPLACEMENTCHAR;
                else
                    *((LPAPLCHAR) lpMemRes)++ = (APLCHAR) aplLongestRht;
            } // End IF

            break;

        case ARRAY_FLOAT:
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Attempt to convert the float to an integer using System []CT
                aplLongestRht = FloatToAplint_SCT (*((LPAPLFLOAT) lpMemRht)++, &bRet);
                if (!bRet)
                    goto DOMAIN_EXIT;

                // Check for out of range for Unicode
                //   as an APLUINT so we don't have to deal with negatives
                if (MAX_UNICODE < aplLongestRht)
                    goto DOMAIN_EXIT;

                // Check for out of range for UCS-2
                if (APLCHAR_SIZE <= aplLongestRht)
                    *((LPAPLCHAR) lpMemRes)++ = UTF16_REPLACEMENTCHAR;
                else
                    *((LPAPLCHAR) lpMemRes)++ = (APLCHAR) aplLongestRht;
            } // End FOR

            break;

        case ARRAY_CHAR:
            for (uRht = 0; uRht < aplNELMRht; uRht++)
                *((LPAPLINT) lpMemRes)++ = *((LPAPLCHAR) lpMemRht)++;
            break;

        case ARRAY_APA:
#define lpAPA           ((LPAPLAPA) lpMemRht)
            // Get the APA parameters
            apaOffRht = lpAPA->Off;
            apaMulRht = lpAPA->Mul;
#undef  lpAPA
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Get the next element
                aplLongestRht = apaOffRht + apaMulRht * uRht;

                // Check for out of range for Unicode
                //   as an APLUINT so we don't have to deal with negatives
                if (MAX_UNICODE < aplLongestRht)
                    goto DOMAIN_EXIT;

                // Check for out of range for UCS-2
                if (APLCHAR_SIZE <= aplLongestRht)
                    *((LPAPLCHAR) lpMemRes)++ = UTF16_REPLACEMENTCHAR;
                else
                    *((LPAPLCHAR) lpMemRes)++ = (APLCHAR) aplLongestRht;
            } // End IF

            break;

        case ARRAY_HETERO:
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                aplHeteroRht  = *((LPAPLHETERO) lpMemRht)++;

                // Split cases based upon the ptr type bits
                switch (GetPtrTypeDir (aplHeteroRht))
                {
                    case PTRTYPE_STCONST:
                        aplLongestRht = aplHeteroRht->stData.stLongest;

                        // Split cases based upon the immediate type
                        switch (aplHeteroRht->stFlags.ImmType)
                        {
                            case IMMTYPE_BOOL:
                                *((LPAPLHETERO) lpMemRes)++ =
                                lpSymTmp =
                                  SymTabAppendChar_EM    ((APLBOOL) aplLongestRht, TRUE);
                                if (!lpSymTmp)
                                    goto ERROR_EXIT;
                                break;

                            case IMMTYPE_FLOAT:
                                // Attempt to convert the float to an integer using System []CT
                                aplLongestRht = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestRht, &bRet);
                                if (!bRet)
                                    goto DOMAIN_EXIT;

                                // Fall through to common code

                            case IMMTYPE_INT:
                                // Check for out of range for Unicode
                                //   as an APLUINT so we don't have to deal with negatives
                                if (MAX_UNICODE < aplLongestRht)
                                    goto DOMAIN_EXIT;

                                // Check for out of range for UCS-2
                                if (APLCHAR_SIZE <= aplLongestRht)
                                    aplLongestRht = UTF16_REPLACEMENTCHAR;

                                *((LPAPLHETERO) lpMemRes)++ =
                                lpSymTmp =
                                  SymTabAppendChar_EM    ((APLCHAR) aplLongestRht, TRUE);
                                if (!lpSymTmp)
                                    goto ERROR_EXIT;
                                break;

                            case IMMTYPE_CHAR:
                                *((LPAPLHETERO) lpMemRes)++ =
                                lpSymTmp =
                                  SymTabAppendInteger_EM ((APLCHAR) aplLongestRht, TRUE);
                                if (!lpSymTmp)
                                    goto ERROR_EXIT;
                                break;

                            defstop
                                break;
                        } // End SWITCH

                        break;

                    case PTRTYPE_HGLOBAL:
                        // Lock the memory to get a ptr to it
                        lpMemHeteroRht = MyGlobalLock (aplHeteroRht);

                        // Get the array type
                        aplTypeHet = ((LPVARARRAY_HEADER) lpMemHeteroRht)->ArrType;

                        // Skip over the header and dimensions to the data
                        lpMemHeteroRht = VarArrayDataFmBase (lpMemHeteroRht);

                        // Split cases based upon the array storage type
                        switch (aplTypeHet)
                        {
                            case ARRAY_RAT:
                                aplLongestRht = mpq_get_sctsx ((LPAPLRAT) lpMemHeteroRht, &bRet);

                                break;

                            case ARRAY_VFP:
                                aplLongestRht = mpfr_get_sctsx ((LPAPLVFP) lpMemHeteroRht, &bRet);

                                break;

                            defstop
                                break;
                        } // End SWITCH

                        if (!bRet)
                            goto DOMAIN_EXIT;

                        // Check for out of range for Unicode
                        //   as an APLUINT so we don't have to deal with negatives
                        if (MAX_UNICODE < aplLongestRht)
                            goto DOMAIN_EXIT;

                        // Check for out of range for UCS-2
                        if (APLCHAR_SIZE <= aplLongestRht)
                            aplLongestRht = UTF16_REPLACEMENTCHAR;

                        *((LPAPLHETERO) lpMemRes)++ =
                        lpSymTmp =
                          SymTabAppendChar_EM    ((APLCHAR) aplLongestRht, TRUE);
                        if (!lpSymTmp)
                            goto ERROR_EXIT;

                        // We no longer need this ptr
                        MyGlobalUnlock (aplHeteroRht); lpMemHeteroRht = NULL;

                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR

            break;

        case ARRAY_RAT:
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Attempt to convert the RAT to an integer using System []CT
                aplLongestRht = mpq_get_sctsx (((LPAPLRAT) lpMemRht)++, &bRet);
                if (!bRet)
                    goto DOMAIN_EXIT;

                // Check for out of range for Unicode
                //   as an APLUINT so we don't have to deal with negatives
                if (MAX_UNICODE < aplLongestRht)
                    goto DOMAIN_EXIT;

                // Check for out of range for UCS-2
                if (APLCHAR_SIZE <= aplLongestRht)
                    *((LPAPLCHAR) lpMemRes)++ = UTF16_REPLACEMENTCHAR;
                else
                    *((LPAPLCHAR) lpMemRes)++ = (APLCHAR) aplLongestRht;
            } // End FOR

            break;

        case ARRAY_VFP:
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Attempt to convert the VFP to an integer using System []CT
                aplLongestRht = mpfr_get_sctsx (((LPAPLVFP) lpMemRht)++, &bRet);
                if (!bRet)
                    goto DOMAIN_EXIT;

                // Check for out of range for Unicode
                //   as an APLUINT so we don't have to deal with negatives
                if (MAX_UNICODE < aplLongestRht)
                    goto DOMAIN_EXIT;

                // Check for out of range for UCS-2
                if (APLCHAR_SIZE <= aplLongestRht)
                    *((LPAPLCHAR) lpMemRes)++ = UTF16_REPLACEMENTCHAR;
                else
                    *((LPAPLCHAR) lpMemRes)++ = (APLCHAR) aplLongestRht;
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH

    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
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
    // We no longer need this ptr
    if (hGlbRes && lpMemRes)
    {
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    // We no longer need this ptr
    if (hGlbRht && lpMemRht)
    {
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return lpYYRes;
} // End SysnMonUCS_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydUCS_EM_YY
//
//  Dyadic []UCS -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydUCS_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydUCS_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End SysFnDydUCS_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_ucs.c
//***************************************************************************
