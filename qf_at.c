//***************************************************************************
//  NARS2000 -- System Function -- Quad AT
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
//  $SysFnAT_EM_YY
//
//  System function:  []AT -- Attributes
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnAT_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnAT_EM_YY
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
        return SysFnMonAT_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydAT_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnAT_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonAT_EM_YY
//
//  Monadic []AT -- error
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonAT_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonAT_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End SysFnMonAT_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydAT_EM_YY
//
//  Dyadic []AT -- Attributes
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydAT_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydAT_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeLft,        // Left arg storage type
                 aplTypeRht;        // Right ...
    APLNELM      aplNELMRht,        // Right arg NELM
                 aplColsRht,        // ...       # cols
                 aplNELMRes,        // Result NELM
                 aplRowsRes,        // ...    # rows
                 aplColsRes;        // ...      cols
    APLRANK      aplRankLft,        // Left arg rank
                 aplRankRht,        // Right ...
                 aplRankRes;        // Result   ...
    HGLOBAL      hGlbLft,           // Left arg global memory handle
                 hGlbRht = NULL,    // Right ...
                 hGlbRes = NULL;    // Result   ...
    LPVOID       lpMemRht = NULL;   // Ptr to right arg global memory
    LPAPLCHAR    lpMemDataRht,      // Ptr to right arg char data
                 lpMemDataStart;    // Ptr to start of identifier
    LPAPLUINT    lpMemRes = NULL,   // Ptr to result global memory
                 lpMemDataRes;      // Ptr to result integer data
    APLLONGEST   aplLongestLft,     // Left arg immediate value
                 aplLongestRht;     // Right ...
    APLUINT      uRht,              // Loop counter
                 uCol,              // ...
                 ByteRes;           // # bytes in the result
    LPSYMENTRY   lpSymEntry;        // Ptr to SYMENTRY
    STFLAGS      stFlags;           // STE flags
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
    UBOOL        bRet;              // TRUE iff the result is valid

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, NULL,        &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, &aplColsRht);

    // Check for LEFT RANK ERROR
    if (!IsScalar (aplRankLft))
        goto LEFT_RANK_EXIT;

    // Check for RIGHT RANK ERROR
    if (IsRank3P (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Check for LEFT DOMAIN ERROR
    if (!IsNumeric (aplTypeLft))
        goto LEFT_DOMAIN_EXIT;

    // Get the left arg global handle & longest w/o locking it
    aplLongestLft = GetGlbPtrs (lptkLftArg, &hGlbLft);

    // If the left arg is a global, ...
    if (hGlbLft)
        // Set the ptr type bits
        hGlbLft = MakePtrTypeGlb (hGlbLft);

    // Split cases based upon the left arg storage type
    switch (aplTypeLft)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            bRet = TRUE;

            break;

        case ARRAY_FLOAT:
            // Attempt to convert the float to an integer using System []CT
            aplLongestLft = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestLft, &bRet);

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
    if (!bRet)
        goto LEFT_DOMAIN_EXIT;

    // Check for LEFT DOMAIN ERROR
    switch (aplLongestLft)
    {
        case 1:                     // Valences
            aplColsRes = 3;         // [1] = Explicit result (0 or 1)
                                    // [2] = Function valence (0, 1, or 2)
                                    // [3] = Operator valence (0, 1, or 2)
            break;

        case 2:                     // Fix time
            aplColsRes = 7;         // [1] = Year
                                    // [2] = Month
                                    // [3] = Day
                                    // [4] = Hour
                                    // [5] = Minute
                                    // [6] = Second
                                    // [7] = Millisecond
            break;

        case 3:                     // Execution properties
            aplColsRes = 4;         // [1] = Nondisplayable
                                    // [2] = Nonsuspendable
                                    // [3] = Ignores weak interrupts
                                    // [4] = Converts non-resource errorr to DOMAIN ERROR
            break;

        case 4:                     // Object size
            aplColsRes = 2;         // [1] = Header and Data
                                    // [2] = Data only
            break;

        default:
            goto LEFT_DOMAIN_EXIT;
    } // End SWITCH

    // Check for RIGHT DOMAIN ERROR
    if (!IsSimple (aplTypeRht)
     || ((!IsSimpleChar (aplTypeRht))
      && !IsEmpty (aplNELMRht)))
        goto RIGHT_DOMAIN_EXIT;

    // Get right arg global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Calculate the # identifiers in the argument
    //   allowing for vector and matrix with multiple names
    bRet =
      CalcNumIDs (aplNELMRht,       // Right arg NELM
                  aplRankRht,       // Right arg rank
                  aplLongestRht,    // Right arg longest
                  TRUE,             // TRUE iff we allow multiple names in a vector
                  lpMemRht,         // Ptr to right arg global memory
                 &aplRowsRes,       // Ptr to # right arg IDs
                 &aplColsRht);      // Ptr to # right arg cols (matrix only)
    // Note that if bRet EQ FALSE, aplRowsRes EQ 1

    // Calculate the result NELM and rank
    aplNELMRes = aplRowsRes * aplColsRes;
    aplRankRes = 1 + (aplRowsRes > 1);

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_INT, aplNELMRes, aplRankRes);

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
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_INT;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // If the result is a matrix, ...
    if (IsMatrix (aplRankRes))
    {
        // Fill in the dimensions
        (VarArrayBaseToDim (lpMemRes))[0] = aplRowsRes;
        (VarArrayBaseToDim (lpMemRes))[1] = aplColsRes;
    } else
    // The result is a vector
        // Fill in the dimensions
        (VarArrayBaseToDim (lpMemRes))[0] = aplColsRes;

    // If we failed in CalcNumIDs, quit now
    if (!bRet)
        goto YYALLOC_EXIT;

    // Skip over the header and dimensions to the data
    lpMemDataRes = VarArrayDataFmBase (lpMemRes);

    // Split cases based upon the right arg rank
    switch (aplRankRht)
    {
        case 0:
            // Lookup the name in the symbol table
            // SymTabLookupName sets the .ObjName enum,
            //   and the .Inuse flag
            ZeroMemory (&stFlags, sizeof (stFlags));
            lpSymEntry =
              SymTabLookupNameLength ((LPAPLCHAR) &aplLongestRht,
                                      1,
                                     &stFlags);
            // If found, ...
            if (lpSymEntry)
                lpMemDataRes =
                  AttributeCommon (lpMemDataRes,        // Ptr to result global memory
                                   lpSymEntry,          // Ptr to object SYMENTRY
                                   aplLongestLft);      // Function type
            break;

        case 1:
            // Skip over the header and dimensions to the data
            lpMemDataRht = VarArrayDataFmBase (lpMemRht);

            // Loop through the right arg looking for identifiers
            uRht = 0;
            while (TRUE)
            {
                // Skip over white space
                while (uRht < aplNELMRht && lpMemDataRht[uRht] EQ L' ')
                    uRht++;
                if (uRht < aplColsRht)
                {
                    // Save the starting ptr
                    lpMemDataStart = &lpMemDataRht[uRht];

                    // Skip over black space
                    while (uRht < aplNELMRht && lpMemDataRht[uRht] NE L' ')
                        uRht++;
                    // Lookup the name in the symbol table
                    // SymTabLookupName sets the .ObjName enum,
                    //   and the .Inuse flag
                    ZeroMemory (&stFlags, sizeof (stFlags));
                    lpSymEntry =
                      SymTabLookupNameLength (lpMemDataStart,
                                              (APLU3264) (&lpMemDataRht[uRht] - lpMemDataStart),
                                             &stFlags);
                    // If found, ...
                    if (lpSymEntry)
                        lpMemDataRes =
                          AttributeCommon (lpMemDataRes,        // Ptr to result global memory
                                           lpSymEntry,          // Ptr to object SYMENTRY
                                           aplLongestLft);      // Function type
                } else
                    break;
            } // End WHILE

            break;

        case 2:
            // Skip over the header and dimensions to the data
            lpMemDataRht = VarArrayDataFmBase (lpMemRht);

            for (uRht = 0; uRht < aplRowsRes; uRht++)
            {
                // Point to the start of the data
                lpMemDataStart = &lpMemDataRht[aplColsRht * uRht];

                // Skip over leading white space
                uCol = 0;
                while (uCol < aplColsRht && lpMemDataStart[uCol] EQ L' ')
                    uCol++;

                // Lookup the name in the symbol table
                // SymTabLookupName sets the .ObjName enum,
                //   and the .Inuse flag
                ZeroMemory (&stFlags, sizeof (stFlags));
                lpSymEntry =
                  SymTabLookupNameLength (&lpMemDataStart[uCol],
                                           (APLU3264) (aplColsRht - uCol),
                                          &stFlags);
                // If found, ...
                if (lpSymEntry)
                    lpMemDataRes =
                      AttributeCommon (lpMemDataRes,        // Ptr to result global memory
                                       lpSymEntry,          // Ptr to object SYMENTRY
                                       aplLongestLft);      // Function type
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH
YYALLOC_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = lpMemDataRes = NULL;

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
    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    // We no longer need this ptr
    if (hGlbRht && lpMemRht)
    {
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnDydAT_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $AttributeCommon
//
//  Common routine to call the appropriate Attribute routine
//***************************************************************************

LPAPLUINT AttributeCommon
    (LPAPLUINT  lpMemDataRes,               // Ptr to result global memory
     LPSYMENTRY lpSymEntry,                 // Ptr to object SYMENTRY
     APLLONGEST aplLongestLft)              // Function type

{
    // Split cases based upon the left arg value
    switch (aplLongestLft)
    {
        case 1:         // Valences
            // Save the valences
            return
              AttributeValences (lpMemDataRes,      // Ptr to result global memory
                                 lpSymEntry);       // Ptr to object SYMENTRY
        case 2:         // Fix time
            // Save the fix time
            return
              AttributeFixTime  (lpMemDataRes,      // Ptr to result global memory
                                 lpSymEntry);       // Ptr to object SYMENTRY
        case 3:
            // Save the execution properties
            return
              AttributeExecProp (lpMemDataRes,      // Ptr to result global memory
                                 lpSymEntry);       // Ptr to object SYMENTRY
        case 4:
            // Save the object size
            return
              AttributeObjSize  (lpMemDataRes,      // Ptr to result global memory
                                 lpSymEntry);       // Ptr to object SYMENTRY
        defstop
            return NULL;
    } // End SWITCH
} // End AttributeCommon


//***************************************************************************
//  $AttributeValences
//
//  Save the object's valences
//***************************************************************************

LPAPLUINT AttributeValences
    (LPAPLUINT  lpMemDataRes,               // Ptr to result global memory
     LPSYMENTRY lpSymEntry)                 // Ptr to object SYMENTRY

{
    HGLOBAL hGlbObj = NULL;                 // Object global memory handle
    LPVOID  lpMemObj = NULL;                // Ptr to object global memory

    // Split cases based upon the name type
    switch (lpSymEntry->stFlags.stNameType)
    {
        case NAMETYPE_UNK:
            *lpMemDataRes++ = 0;            // [1] = Explicit result (0 or 1)
            *lpMemDataRes++ = 0;            // [2] = Function valence (0, 1, or 2)
            *lpMemDataRes++ = 0;            // [3] = Operator valence (0, 1, or 2)

            break;

        case NAMETYPE_VAR:
            *lpMemDataRes++ = 1;            // [1] = Explicit result (0 or 1)
            *lpMemDataRes++ =               // [2] = Function valence (0, 1, or 2)
            *lpMemDataRes++ = 0;            // [3] = Operator valence (0, 1, or 2)

            break;

        case NAMETYPE_FN0:
        case NAMETYPE_FN12:
        case NAMETYPE_OP1:
        case NAMETYPE_OP2:
        case NAMETYPE_OP3:
            // If the object is immediate, ...
            if (lpSymEntry->stFlags.Imm)
            {
                // Split cases based upon the name type
                switch (lpSymEntry->stFlags.stNameType)
                {
                    case NAMETYPE_FN12:
                        *lpMemDataRes++ = 1;            // [1] = Explicit result (0 or 1)
                        *lpMemDataRes++ = 2;            // [2] = Function valence (0, 1, or 2)
                        *lpMemDataRes++ = 0;            // [3] = Operator valence (0, 1, or 2)

                        break;

                    case NAMETYPE_OP1:
                        *lpMemDataRes++ = 1;            // [1] = Explicit result (0 or 1)
                        *lpMemDataRes++ = 0;            // [2] = Function valence (0, 1, or 2)
                        *lpMemDataRes++ = 1;            // [3] = Operator valence (0, 1, or 2)

                        break;

                    case NAMETYPE_OP2:
                        *lpMemDataRes++ = 1;            // [1] = Explicit result (0 or 1)
                        *lpMemDataRes++ = 0;            // [2] = Function valence (0, 1, or 2)
                        *lpMemDataRes++ = 2;            // [3] = Operator valence (0, 1, or 2)

                        break;

                    case NAMETYPE_OP3:
                        *lpMemDataRes++ = 1;            // [1] = Explicit result (0 or 1)
                        *lpMemDataRes++ = 0;            // [2] = Function valence (0, 1, or 2)
                        *lpMemDataRes++ = 1;            // [3] = Operator valence (0, 1, or 2)

                        break;

                    case NAMETYPE_FN0:
                    defstop
                        break;
                } // End SWITCH
            } else
            // If it's a user-defined function/operator
            if (lpSymEntry->stFlags.UsrDfn)
            {
                // Get the user-defined function/operator global memory handle
                hGlbObj = lpSymEntry->stData.stGlbData;

                // Lock the memory to get a ptr to it
                lpMemObj = MyGlobalLock (hGlbObj);

#define lpHeader    ((LPDFN_HEADER) lpMemObj)
                *lpMemDataRes++ = lpHeader->numResultSTE > 0;   // [1] = Explicit result (0 or 1)
                *lpMemDataRes++ = (lpHeader->numLftArgSTE > 0)  // [2] = Function valence (0, 1, or 2)
                                + (lpHeader->numRhtArgSTE > 0);
                *lpMemDataRes++ = (lpHeader->steLftOpr NE 0)    // [3] = Operator valence (0, 1, or 2)
                                + (lpHeader->steRhtOpr NE 0);
#undef  lpHeader
            } else
            // If it's a direct function, ...
            if (lpSymEntry->stFlags.FcnDir)
            {
                *lpMemDataRes++ = 1;            // [1] = Explicit result (0 or 1)
                *lpMemDataRes++ = 2;            // [2] = Function valence (0, 1, or 2)
                *lpMemDataRes++ = 0;            // [3] = Operator valence (0, 1, or 2)
            } else
            // It's a function array
            {
                *lpMemDataRes++ = 1;            // [1] = Explicit result (0 or 1)
                *lpMemDataRes++ = 2;            // [2] = Function valence (0, 1, or 2)
                *lpMemDataRes++ = 0;            // [3] = Operator valence (0, 1, or 2)
            } // End IF/ELSE/...

            break;

        case NAMETYPE_TRN:
            *lpMemDataRes++ = 1;            // [1] = Explicit result (0 or 1)
            *lpMemDataRes++ = 2;            // [2] = Function valence (0, 1, or 2)
            *lpMemDataRes++ = 0;            // [3] = Operator valence (0, 1, or 2)

            break;

        case NAMETYPE_LST:
        case NAMETYPE_FILL1:
        case NAMETYPE_FILL2:
        defstop
            break;
    } // End SWITCH

    if (hGlbObj && lpMemObj)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbObj); lpMemObj = NULL;
    } // End IF

    return lpMemDataRes;
} // End AttributeValences


//***************************************************************************
//  $AttributeFixTime
//
//  Save the object's fix time
//***************************************************************************

LPAPLUINT AttributeFixTime
    (LPAPLUINT  lpMemDataRes,               // Ptr to result global memory
     LPSYMENTRY lpSymEntry)                 // Ptr to object SYMENTRY

{
    HGLOBAL    hGlbObj = NULL;              // Object global memory handle
    LPVOID     lpMemObj = NULL;             // Ptr to object global memory
    FILETIME   ftLastMod;                   // FILETIME of last modification
    SYSTEMTIME systemTime;                  // Current system (UTC) time

    // Split cases based upon the name type
    switch (lpSymEntry->stFlags.stNameType)
    {
        case NAMETYPE_UNK:
        case NAMETYPE_VAR:
            *lpMemDataRes++ = 0;            // [1] = Year
            *lpMemDataRes++ = 0;            // [2] = Month
            *lpMemDataRes++ = 0;            // [3] = Day
            *lpMemDataRes++ = 0;            // [4] = Hour
            *lpMemDataRes++ = 0;            // [5] = Minute
            *lpMemDataRes++ = 0;            // [6] = Second
            *lpMemDataRes++ = 0;            // [7] = Millisecond

            break;

        case NAMETYPE_FN0:
        case NAMETYPE_FN12:
        case NAMETYPE_OP1:
        case NAMETYPE_OP2:
        case NAMETYPE_OP3:
        case NAMETYPE_TRN:
            // If the object is not immediate, and
            //                  not a direct function, ...
            if (!lpSymEntry->stFlags.Imm
             && !lpSymEntry->stFlags.FcnDir)
            {
                // Get the user-defined function/operator global memory handle
                hGlbObj = lpSymEntry->stData.stGlbData;

                // Lock the memory to get a ptr to it
                lpMemObj = MyGlobalLock (hGlbObj);

                // If it's a user-defined function/operator
                if (lpSymEntry->stFlags.UsrDfn)
#define lpHeader    ((LPDFN_HEADER) lpMemObj)
                    ftLastMod = lpHeader->ftLastMod;
#undef  lpHeader
                else
                // It's a function array
#define lpHeader    ((LPFCNARRAY_HEADER) lpMemObj)
                    ftLastMod = lpHeader->ftLastMod;
#undef  lpHeader
                // Convert the last mod time to system time so we can display it
                FileTimeToSystemTime (&ftLastMod, &systemTime);

                *lpMemDataRes++ = systemTime.wYear;         // [1] = Year
                *lpMemDataRes++ = systemTime.wMonth;        // [2] = Month
                *lpMemDataRes++ = systemTime.wDay;          // [3] = Day
                *lpMemDataRes++ = systemTime.wHour;         // [4] = Hour
                *lpMemDataRes++ = systemTime.wMinute;       // [5] = Minute
                *lpMemDataRes++ = systemTime.wSecond;       // [6] = Second
                *lpMemDataRes++ = systemTime.wMilliseconds; // [7] = Millisecond
            } // End IF

            break;

        case NAMETYPE_LST:
        case NAMETYPE_FILL1:
        case NAMETYPE_FILL2:
        defstop
            break;
    } // End SWITCH

    if (hGlbObj && lpMemObj)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbObj); lpMemObj = NULL;
    } // End IF

    return lpMemDataRes;
} // End AttributeFixTime


//***************************************************************************
//  $AttributeExecProp
//
//  Save the object's execution properties
//***************************************************************************

LPAPLUINT AttributeExecProp
    (LPAPLUINT  lpMemDataRes,               // Ptr to result global memory
     LPSYMENTRY lpSymEntry)                 // Ptr to object SYMENTRY

{
    // Split cases based upon the name type
    switch (lpSymEntry->stFlags.stNameType)
    {
        case NAMETYPE_UNK:
        case NAMETYPE_VAR:
            *lpMemDataRes++ = 0;            // [1] = Nondisplayable
            *lpMemDataRes++ = 0;            // [2] = Nonsuspendable
            *lpMemDataRes++ = 0;            // [3] = Ignores weak interrupts
            *lpMemDataRes++ = 0;            // [4] = Converts non-resource errors to DOMAIN ERRORs

            break;

        case NAMETYPE_FN0:
        case NAMETYPE_FN12:
        case NAMETYPE_OP1:
        case NAMETYPE_OP2:
        case NAMETYPE_OP3:
        case NAMETYPE_TRN:
            // If the object is immediate, ...
            if (lpSymEntry->stFlags.Imm)
            {
                *lpMemDataRes++ = 1;            // [1] = Nondisplayable
                *lpMemDataRes++ = 1;            // [2] = Nonsuspendable
                *lpMemDataRes++ = 1;            // [3] = Ignores weak interrupts
                *lpMemDataRes++ = 0;            // [4] = Converts non-resource errors to DOMAIN ERRORs
            } else
            // If it's a user-defined function/operator
            if (lpSymEntry->stFlags.UsrDfn)
            {
                LPDFN_HEADER lpMemDfnHdr;

                // Lock the memory to get a ptr to it
                lpMemDfnHdr = MyGlobalLock (lpSymEntry->stData.stGlbData);

                // If it's a Magic Function/Operator,
                //   or AFO, ...
                if (lpSymEntry->stFlags.ObjName EQ OBJNAME_MFO
                 || lpMemDfnHdr->bAFO)
                {
                    *lpMemDataRes++ = 0;            // [1] = Nondisplayable
                    *lpMemDataRes++ = 1;            // [2] = Nonsuspendable
                    *lpMemDataRes++ = 1;            // [3] = Ignores weak interrupts
                    *lpMemDataRes++ = 0;            // [4] = Converts non-resource errors to DOMAIN ERRORs
                } else
                {
                    *lpMemDataRes++ = 0;            // [1] = Nondisplayable
                    *lpMemDataRes++ = 0;            // [2] = Nonsuspendable
                    *lpMemDataRes++ = 0;            // [3] = Ignores weak interrupts
                    *lpMemDataRes++ = 0;            // [4] = Converts non-resource errors to DOMAIN ERRORs
                } // End IF/ELSE

                // We no longer need this ptr
                MyGlobalUnlock (lpSymEntry->stData.stGlbData); lpMemDfnHdr = NULL;
            } else
            // If it's a direct function, ...
            if (lpSymEntry->stFlags.FcnDir)
            {
                *lpMemDataRes++ = 1;            // [1] = Nondisplayable
                *lpMemDataRes++ = 1;            // [2] = Nonsuspendable
                *lpMemDataRes++ = 1;            // [3] = Ignores weak interrupts
                *lpMemDataRes++ = 0;            // [4] = Converts non-resource errors to DOMAIN ERRORs
            } else
            // It's a function array
            {
                *lpMemDataRes++ = 0;            // [1] = Nondisplayable
                *lpMemDataRes++ = 1;            // [2] = Nonsuspendable
                *lpMemDataRes++ = 1;            // [3] = Ignores weak interrupts
                *lpMemDataRes++ = 0;            // [4] = Converts non-resource errors to DOMAIN ERRORs
            } // End IF/ELSE/...

            break;

        case NAMETYPE_LST:
        case NAMETYPE_FILL1:
        case NAMETYPE_FILL2:
        defstop
            break;
    } // End SWITCH

    return lpMemDataRes;
} // End AttributeExecProp


//***************************************************************************
//  $AttributeObjSize
//
//  Save the object's size
//***************************************************************************

LPAPLUINT AttributeObjSize
    (LPAPLUINT  lpMemDataRes,               // Ptr to result global memory
     LPSYMENTRY lpSymEntry)                 // Ptr to object SYMENTRY

{
    APLINT aplObjSize,                      // The object's size (header and data)
           aplDataSize;                     // The object's data size

    // Initialize the value
    aplDataSize = 0;

    // Get the object's size
    aplObjSize = CalcSymEntrySize (lpSymEntry, &aplDataSize);

    // Save it in the result
    *lpMemDataRes++ = aplObjSize;           // [1] Header and data
    *lpMemDataRes++ = aplDataSize;          // [2] Data

    return lpMemDataRes;
} // End AttributeObjSize


//***************************************************************************
//  $CalcSymEntrySize
//
//  Calculate the size of a SYMENTRY
//***************************************************************************

APLINT CalcSymEntrySize
    (LPSYMENTRY lpSymEntry,         // Ptr to the SYMENTRY
     LPAPLINT   lpDataSize)         // Ptr to save area for object data size (may be NULL)
                                    // If this value is not NULL, it is assumed to have
                                    //   been initialized so we may add to it to allow
                                    //   it to be called serially.

{
    APLUINT aplSize = 0;            // The result

    // If it's an immediate (any type) or an internal function, ...
    if (lpSymEntry->stFlags.Imm
     || lpSymEntry->stFlags.FcnDir)
    {
        aplSize = sizeof (SYMENTRY);

        // If the caller wants the data size, ...
        if (lpDataSize)
        switch (lpSymEntry->stFlags.ImmType)
        {
            case IMMTYPE_BOOL:
                *lpDataSize += sizeof (APLBOOL);

                break;

            case IMMTYPE_INT:
                *lpDataSize += sizeof (APLINT);

                break;

            case IMMTYPE_FLOAT:
                *lpDataSize += sizeof (APLFLOAT);

                break;

            case IMMTYPE_CHAR:
                *lpDataSize += sizeof (APLCHAR);

                break;

            defstop
                break;
        } // End SWITCH
    } else
    // If it has no value, ...
    if (!lpSymEntry->stFlags.Value)
        aplSize = 0;
    else
    // If it is a user variable, ...
    if (IsNameTypeVar (lpSymEntry->stFlags.stNameType))
    {
        // Start with the size of the SYMENTRY
        aplSize = sizeof (SYMENTRY);

        // Recurse through the array returning the total size
        aplSize += CalcGlbVarSize (lpSymEntry->stData.stGlbData, lpDataSize);
    } else
    // If it is a user function/operator, ...
    if (IsNameTypeFnOp (lpSymEntry->stFlags.stNameType))
    {
        HGLOBAL      hGlbDfnHdr;        // User-defined function/operator header global memory handle
        LPDFN_HEADER lpMemDfnHdr;       // Ptr to user-defined function/operator header ...
        LPFCNLINE    lpFcnLines;        // Ptr to array of function line structs (FCNLINE[numFcnLines])
        UINT         uNumFcnLines,      // # function lines
                     uLine;             // Loop counter
        APLINT       aplGlbSize;        // Global size temp

        // Get the global memory handle
        hGlbDfnHdr = lpSymEntry->stData.stGlbData;

        // stData is a valid HGLOBAL function array or user-defined function/operator
        Assert (IsGlbTypeFcnDir_PTB (hGlbDfnHdr)
             || IsGlbTypeDfnDir_PTB (hGlbDfnHdr));

        // Split cases based upon the user-defined function/operator bit
        if (lpSymEntry->stFlags.UsrDfn)
        {
            // Lock the memory to get a ptr to it
            lpMemDfnHdr = MyGlobalLock (hGlbDfnHdr);

            // Get # function lines
            uNumFcnLines = lpMemDfnHdr->numFcnLines;

            // Start with the size of the DFN_HEADER
            aplSize =   sizeof (DFN_HEADER)
                      + sizeof (LPSYMENTRY) * (lpMemDfnHdr->numResultSTE
                                             + lpMemDfnHdr->numLftArgSTE
                                             + lpMemDfnHdr->numRhtArgSTE
                                             + lpMemDfnHdr->numLocalsSTE)
                      + sizeof (FCNLINE) * uNumFcnLines;
            if (lpMemDfnHdr->hGlbMonInfo)
                aplSize += MyGlobalSize (lpMemDfnHdr->hGlbMonInfo);

            // Get ptr to array of function line structs (FCNLINE[numFcnLines])
            lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

            // If the caller wants the data size, ...
            if (lpDataSize)
                *lpDataSize += uNumFcnLines * sizeof (FCNLINE);

            // Initialize
            aplGlbSize = 0;

            // Loop through the function lines
            for (uLine = 0; uLine < uNumFcnLines; uLine++)
            {
                if (lpFcnLines->hGlbTxtLine)
                    aplGlbSize += MyGlobalSize (lpFcnLines->hGlbTxtLine);

                if (lpFcnLines->offTknLine)
                {
                    LPTOKEN_HEADER lpTokenHdr;      // Ptr to token header

                    // Get a ptr to the TOKEN_HEADER
                    lpTokenHdr = (LPTOKEN_HEADER) ByteAddr (lpMemDfnHdr, lpFcnLines->offTknLine);

                    // Add in the token header and tokens all in bytes
                    aplGlbSize += sizeof (TOKEN_HEADER) + lpTokenHdr->TokenCnt * sizeof (TOKEN);
                } // End IF

                // Skip to the next struct
                lpFcnLines++;
            } // End FOR

            // Add in the size of the function header text
            if (lpMemDfnHdr->hGlbTxtHdr)
                aplGlbSize += MyGlobalSize (lpMemDfnHdr->hGlbTxtHdr);

            // Add in the size of the function header tokenized
            if (lpMemDfnHdr->hGlbTknHdr)
                aplGlbSize += MyGlobalSize (lpMemDfnHdr->hGlbTknHdr);

            // Include in the object size
            aplSize += aplGlbSize;

            // If the caller wants the data size, ...
            if (lpDataSize)
                *lpDataSize += aplGlbSize;

            // Add in the size of the function Undo buffer
            if (lpMemDfnHdr->hGlbUndoBuff)
                aplSize += MyGlobalSize (lpMemDfnHdr->hGlbUndoBuff);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
        } else
        // Otherwise, it's a function array
            // Start with the size of the SYMENTRY
            aplSize = sizeof (SYMENTRY)
                    + CalcGlbFcnSize (lpSymEntry->stData.stGlbData, lpDataSize);
    } else
    // Otherwise, its size is zero
        aplSize = 0;

    return aplSize;
} // End CalcSymEntrySize


//***************************************************************************
//  $CalcGlbVarSize
//
//  Calculate the size of a global memory variable
//***************************************************************************

APLUINT CalcGlbVarSize
    (HGLOBAL  hGlbData,             // Global memory handle
     LPAPLINT lpDataSize)           // Ptr to save area for object data size (may be NULL)
                                    // If this value is not NULL, it is assumed to have
                                    //   been initialized so we may add to it to allow
                                    //   it to be called serially.

{
    APLUINT     aplSize = 0;        // The result
    LPAPLNESTED lpMemData;          // Ptr to the global memory
    LPAPLRAT    lpMemRat;           // Ptr to Rationals ...
    APLSTYPE    aplType;            // Data storage type
    APLNELM     aplNELM;            // Data NELM
    APLRANK     aplRank;            // Data rank
    APLUINT     uData;              // Loop counter

    // stData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir_PTB (hGlbData));

    aplSize += MyGlobalSize (hGlbData);

    // Lock the memory to get a ptr to it
    lpMemData = MyGlobalLock (hGlbData);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemData)
    // Get the Array Type, NELM, and Rank
    aplType = lpHeader->ArrType;
    aplNELM = lpHeader->NELM;
    aplRank = lpHeader->Rank;
#undef  lpHeader

    // If the caller wants the data size, ...
    if (lpDataSize)
        *lpDataSize += aplSize - (sizeof (VARARRAY_HEADER) + aplRank * sizeof (APLDIM));

    // If the array is simple, that's all
    if (IsSimple (aplType))
        goto NORMAL_EXIT;

    // Skip over the header and dimensions to the data
    lpMemData = VarArrayDataFmBase (lpMemData);

    // Split cases based upon the array type
    switch (aplType)
    {
        case ARRAY_NESTED:
        case ARRAY_HETERO:
            // Loop through the array adding the sizes
            for (uData = 0; uData < aplNELM; uData++)
            // Split cases based upon the pointer type
            switch (GetPtrTypeDir (lpMemData[uData]))
            {
                case PTRTYPE_STCONST:
                    aplSize += sizeof (SYMENTRY);

                    break;

                case PTRTYPE_HGLOBAL:
                    aplSize += CalcGlbVarSize (lpMemData[uData], lpDataSize);

                    break;

                defstop
                    break;
            } // End FOR/SWITCH

            break;

        case ARRAY_RAT:
            // Copy as ptr to Rationals
            lpMemRat = (LPAPLRAT) lpMemData;

            // Loop through the array adding the sizes
            for (uData = 0; uData < aplNELM; uData++, lpMemRat++)
                aplSize += sizeof (APLRAT)                      // Size of the header
                        +  lpMemRat->_mp_num._mp_alloc          // # numerator limbs
                        * sizeof (*lpMemRat->_mp_num._mp_d)     // ...times the size of each limb
                        +  lpMemRat->_mp_den._mp_alloc          // # denominator limbs
                        * sizeof (*lpMemRat->_mp_den._mp_d);    // ...times the size of each limb
            break;

        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_CHAR:
        case ARRAY_FLOAT:
        case ARRAY_APA:
        defstop
            break;
    } // End SWITCH
NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMemData = NULL;

    return aplSize;
} // End CalcGlbVarSize


//***************************************************************************
//  $CalcGlbFcnSize
//
//  Calculate the size of a global memory function array
//***************************************************************************

APLUINT CalcGlbFcnSize
    (HGLOBAL  hGlbData,             // Global memory handle
     LPAPLINT lpDataSize)           // Ptr to save area for object data size (may be NULL)
                                    // If this value is not NULL, it is assumed to have
                                    //   been initialized so we may add to it to allow
                                    //   it to be called serially.

{
    APLUINT           aplSize = 0;  // The result
    HGLOBAL           hGlbTxtLine;  // Line text global memory handle
    LPFCNARRAY_HEADER lpMemData;    // Ptr to the global memory

    // stData is a valid HGLOBAL function array
    Assert (IsGlbTypeFcnDir_PTB (hGlbData));

    aplSize += MyGlobalSize (hGlbData);

    // Lock the memory to get a ptr to it
    lpMemData = MyGlobalLock (hGlbData);

#define lpHeader        ((LPFCNARRAY_HEADER) lpMemData)
    // Get the text ptr
    hGlbTxtLine = lpHeader->hGlbTxtLine;
#undef  lpHeader

    // If there's a line text global memory handle, ...
    if (hGlbTxtLine)
        aplSize += MyGlobalSize (hGlbTxtLine);

    // If the caller wants the data size, ...
    if (lpDataSize)
        *lpDataSize += aplSize - sizeof (FCNARRAY_HEADER);

    return aplSize;
} // End CalcGlbFcnSize


//***************************************************************************
//  End of File: qf_at.c
//***************************************************************************
