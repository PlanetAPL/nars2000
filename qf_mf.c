//***************************************************************************
//  NARS2000 -- System Function -- Quad MF
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

typedef union tagLPEXTMONINFO_UNION
{
    EXTMONINFO_INT MI_Int[1];
    EXTMONINFO_FLT MI_Flt[1];
} *LPEXTMONINFO_UNION;


//***************************************************************************
//  $SysFnMF_EM_YY
//
//  System function:  []MF -- Monitor Function
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMF_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMF_EM_YY
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
        return SysFnMonMF_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydMF_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnMF_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonMF_EM_YY
//
//  Monadic []MF -- Monitor Function -- Return current values
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonMF_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonMF_EM_YY
    (LPTOKEN lptkFunc,                          // Ptr to function token
     LPTOKEN lptkRhtArg,                        // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)                          // Ptr to axis token (may be NULL)

{
    APLSTYPE           aplTypeRht,              // Right arg storage type
                       aplTypeRes;              // Result    ...
    APLNELM            aplNELMRht;              // Right arg NELM
    APLRANK            aplRankRht;              // Right arg rank
    APLLONGEST         aplLongestRht;           // Right arg as immediate
    HGLOBAL            hGlbRht = NULL,          // Right arg global memory handle
                       hGlbDfnHdr = NULL,       // Function  ...
                       hGlbMonInfo = NULL,      // MonInfo   ...
                       hGlbRes = NULL,          // Result    ...
                       lpSymGlbRht;             // Ptr to global immediate
    LPVARARRAY_HEADER  lpMemHdrRht = NULL,      // ...    right arg header
                       lpMemHdrResUnion = NULL; // Ptr to result union header
    LPAPLCHAR          lpMemRht;                // Ptr to right arg global memory
    LPEXTMONINFO_UNION lpMemResUnion;           // Ptr to result    ...           as integers
    LPDFN_HEADER       lpMemDfnHdr = NULL;      // Ptr to function header global memory
    LPINTMONINFO       lpMemMonInfo;            // Ptr to function line monitoring info
    STFLAGS            stFlags = {0};           // Symbol Table Flags used to limit the lookup
    LPSYMENTRY         lpSymEntry;              // Ptr to SYMENTRY if found
    APLUINT            ByteRes,                 // # bytes in the result
                       aplRowsRes,              // # rows in the result
                       aplColsRes;              // # cols ...
    LARGE_INTEGER      aplTickCnt;              // Current tick count
    UINT               uCnt;                    // Loop counter
    LPPL_YYSTYPE       lpYYRes = NULL;          // Ptr to the result
    APLFLOAT           aplScale;                // Scale factor if TIMERSOURCE_PC2MS

    // The right arg may be of three forms:
    //   1.  a scalar name  as in 'a'
    //   2.  a vector name  as in 1{rho}'a' (not 'a b c')
    //   3.  a scalar or one-element vector number as in -2, -1, or TIMERSOURCE_xxx

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RANK_EXIT;

    // Pick off numeric singleton cases
    if (IsNumeric (aplTypeRht)
     && IsSingleton (aplNELMRht))
    {
        APLINT   aplIntegerRht;             // Right arg integer
        APLFLOAT aplFloatRht;               // ...       float
        UBOOL    bRet = TRUE;               // TRUE iff the result is valid

        // Get the one and only value
        GetFirstValueToken (lptkRhtArg,     // Ptr to right arg token
                           &aplIntegerRht,  // Ptr to integer result
                           &aplFloatRht,    // Ptr to float ...
                            NULL,           // Ptr to WCHAR ...
                            NULL,           // Ptr to longest ...
                           &lpSymGlbRht,    // Ptr to lpSym/Glb ...
                            NULL,           // Ptr to ...immediate type ...
                            NULL);          // Ptr to array type ...
        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
                break;

            case ARRAY_FLOAT:
                // Attempt to convert the float to an integer using System []CT
                aplIntegerRht = FloatToAplint_SCT (aplFloatRht, &bRet);

                break;

            case ARRAY_RAT:
                // Attempt to convert the RAT to an integer using System []CT
                aplIntegerRht = mpq_get_sctsx ((LPAPLRAT) lpSymGlbRht, &bRet);

                break;

            case ARRAY_VFP:
                // Attempt to convert the VFP to an integer using System []CT
                aplIntegerRht = mpfr_get_sctsx ((LPAPLVFP) lpSymGlbRht, &bRet);

                break;

            defstop
                break;
        } // End SWITCH

        if (!bRet)
            goto DOMAIN_EXIT;

        return SysFnMonMF_Numeric_EM (aplIntegerRht, lptkFunc);
    } // End IF

    // Check for DOMAIN ERROR
    if (!IsCharOrEmpty (aplTypeRht, aplNELMRht))
        goto DOMAIN_EXIT;

    // Get right arg's global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // If the right arg is a global, ...
    if (hGlbRht NE NULL)
        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    else
        lpMemRht = (LPAPLCHAR) &aplLongestRht;

    // Skip over leading blanks
    while (aplNELMRht && IsWhiteW (lpMemRht[0]))
    {
        lpMemRht++;
        aplNELMRht--;
    } // End WHILE

    // Skip over trailing blanks
    while (aplNELMRht && IsWhiteW (lpMemRht[aplNELMRht - 1]))
        aplNELMRht--;

    // If it's not a valid name, ...
    if (!IsValidName (lpMemRht, (APLU3264) aplNELMRht))
        goto DOMAIN_EXIT;

    // Lookup the name in the symbol table
    // SymTabLookupName sets the .ObjName enum,
    //   and the .Inuse flag
    lpSymEntry =
      SymTabLookupNameLength (lpMemRht,
                   (APLU3264) aplNELMRht,
                             &stFlags);
    if (lpSymEntry)
    {
        // Check for user-defined function/operator
        if (lpSymEntry->stFlags.ObjName NE OBJNAME_USR          // User-defined function/operator
         || !IsNameTypeFnOp (lpSymEntry->stFlags.stNameType)    // ...
         || IzitSusPendent (lpSymEntry))                        // Suspended or pendent
            aplRowsRes = 0;
        else
        {
            // Get the global memory handle
            hGlbDfnHdr = lpSymEntry->stData.stGlbData;

            Assert (IsGlbTypeDfnDir_PTB (hGlbDfnHdr));

            // Lock the memory to get a ptr to it
            lpMemDfnHdr = MyGlobalLockDfn (hGlbDfnHdr);

            // Get the MonInfo global memory handle
            hGlbMonInfo = lpMemDfnHdr->hGlbMonInfo;

            // Get the # rows in the result
            if (hGlbMonInfo NE NULL)
                aplRowsRes = lpMemDfnHdr->numFcnLines + 1;
            else
                aplRowsRes = 0;
            // We no longer need this ptr
            MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
        } // End IF/ELSE
    } else
        aplRowsRes = 0;

    // If the result is empty, ...
    if (aplRowsRes EQ 0)
        aplTypeRes = ARRAY_BOOL;
    else
    // If the timer source is scaled, ...
    if (GetMemPTD ()->uQuadMF EQ TIMERSOURCE_PC2MS)
        aplTypeRes = ARRAY_FLOAT;
    else
        aplTypeRes = ARRAY_INT;

    // Calculate the result cols
    aplColsRes = (sizeof (lpMemResUnion->MI_Int[0]) / sizeof (lpMemResUnion->MI_Int[0].IncSubFns));

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplRowsRes * aplColsRes, 2);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;
    // Lock the memory to get a ptr to it
    lpMemHdrResUnion = MyGlobalLock000 (hGlbRes);

#define lpHeader        lpMemHdrResUnion
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplRowsRes * aplColsRes;
    lpHeader->Rank       = 2;
#undef  lpHeader

    // Fill in the dimensions
    (VarArrayBaseToDim (lpMemHdrResUnion))[0] = aplRowsRes;
    (VarArrayBaseToDim (lpMemHdrResUnion))[1] = aplColsRes;

    // If the result is non-empty, ...
    if (aplRowsRes NE 0)
    {
        // Skip over the header and dimensions to the data
        lpMemResUnion = VarArrayDataFmBase (lpMemHdrResUnion);

        // Lock the memory to get a ptr to it
        lpMemMonInfo = MyGlobalLock (hGlbMonInfo);

        if (IsSimpleInt (aplTypeRes))
            // Copy the function counter
            lpMemResUnion->MI_Int[0].Count = lpMemMonInfo[0].Count;
        else
        {
            // Copy the function counter
            lpMemResUnion->MI_Flt[0].Count = lpMemMonInfo[0].Count;

            aplScale = 1000.0 / (APLFLOAT) (APLINT) liTicksPerSec.QuadPart;
        } // End IF/ELSE

        // Get the current tick count
        GetPerformanceCount (&aplTickCnt);

        // Copy the data to the result
        for (uCnt = 1; uCnt < aplRowsRes; uCnt++)
        if (IsSimpleInt (aplTypeRes))
        {
            lpMemResUnion->MI_Int[uCnt].IncSubFns = lpMemMonInfo[uCnt].IncSubFns;
////////////lpMemResUnion->MI_Int[uCnt].ExcSubFns = lpMemMonInfo[uCnt].ExcSubFns;
            lpMemResUnion->MI_Int[uCnt].Count     = lpMemMonInfo[uCnt].Count;

            if (lpMemMonInfo[uCnt].IncActive)
                lpMemResUnion->MI_Int[uCnt].IncSubFns += aplTickCnt.QuadPart;

////////////if (lpMemMonInfo[uCnt].ExcActive)
////////////    lpMemResUnion->MI_Int[uCnt]->ExcSubFns += aplTickCnt.QuadPart;

            // Add into overall sum
            lpMemResUnion->MI_Int[0].IncSubFns += lpMemResUnion->MI_Int[uCnt].IncSubFns;
        } else
        {
            lpMemResUnion->MI_Flt[uCnt].IncSubFns = ((APLFLOAT) lpMemMonInfo[uCnt].IncSubFns) * aplScale;
////////////lpMemResUnion->MI_Flt[uCnt].ExcSubFns = ((APLFLOAT) lpMemMonInfo[uCnt].ExcSubFns) * aplScale;
            lpMemResUnion->MI_Flt[uCnt].Count     =  (APLFLOAT) lpMemMonInfo[uCnt].Count;

            if (lpMemMonInfo[uCnt].IncActive)
                lpMemResUnion->MI_Flt[uCnt].IncSubFns += aplTickCnt.QuadPart * aplScale;

////////////if (lpMemMonInfo[uCnt].ExcActive)
////////////    lpMemResUnion->MI_Flt[uCnt].ExcSubFns += aplTickCnt.QuadPart * aplScale;

            // Add into overall sum
            lpMemResUnion->MI_Flt[0].IncSubFns += lpMemResUnion->MI_Flt[uCnt].IncSubFns;
        } // End FOR/IF

        // We no longer need this ptr
        MyGlobalUnlock (hGlbMonInfo); lpMemMonInfo = NULL;
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
        if (lpMemHdrResUnion NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemHdrResUnion = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    if (hGlbRes NE NULL && lpMemHdrResUnion NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrResUnion = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnMonMF_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonMF_Numeric_EM
//
//  Subroutine to SysFnMonMF_EM to handle numeric singleton args
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonMF_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonMF_Numeric_EM
    (APLINT  aplIntegerRht,                 // Right arg numeric value
     LPTOKEN lptkFunc)                      // Ptr to function token

{
    LPPL_YYSTYPE  lpYYRes = NULL;           // Ptr to the result
    APLINT        aplIntegerRes;            // Result as an immediate integer
    APLFLOAT      aplFloatRes;              // ...                    float
    LPPERTABDATA  lpMemPTD;                 // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Split cases based upon the numeric value
    switch (aplIntegerRht)
    {
        case -2:            // Return counter currently in use
            aplIntegerRes = lpMemPTD->uQuadMF;

            goto INTEGER_EXIT;

        case -1:            // Return current tick frequency in milliseconds
            // Split cases based upon the []MF timer source
            switch (lpMemPTD->uQuadMF)
            {
                case TIMERSOURCE_MS:
                    // Get # milliseconds per tick
                    aplFloatRes = 1.0;

                    break;

                case TIMERSOURCE_PC:
                case TIMERSOURCE_PC2MS:
                    // Convert to milliseconds per tick
                    aplFloatRes = 1000.0 / (APLFLOAT) (APLINT) liTicksPerSec.QuadPart;

                    break;

                defstop
                    break;
            } // End SWITCH

            goto FLOAT_EXIT;

        case  TIMERSOURCE_MS:           // Set new timer source
        case  TIMERSOURCE_PC:           // ...
        case  TIMERSOURCE_PC2MS:        // ...
            // Save the previous value
            aplIntegerRes = lpMemPTD->uQuadMF;

            // Set to timer source #1 (millisecond timer), or
            //                     #2 (performance timer), or
            //                     #3 (performance timer, scaled to milliseconds)
            lpMemPTD->uQuadMF = aplIntegerRht;

            goto INTEGER_EXIT;

        default:
            goto DOMAIN_EXIT;
    } // End SWITCH

INTEGER_EXIT:
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkInteger  = aplIntegerRes;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

FLOAT_EXIT:
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_FLOAT;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkFloat    = aplFloatRes;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    return lpYYRes;
} // End SysFnMonMF_Numeric_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydMF_EM_YY
//
//  Dyadic []MF -- Monitor Function -- Toggle setting
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydMF_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydMF_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeLft,           // Left  arg storage type
                      aplTypeRht;           // Right ...
    APLNELM           aplNELMLft,           // Left  arg NELM
                      aplNELMRht,           // Right ...
                      aplNELMCol,           // ...       col NELM
                      aplNELMRes;           // Result    ...
    APLRANK           aplRankLft,           // Left  arg rank
                      aplRankRht;           // Right ...
    APLLONGEST        aplLongestLft,        // Left  arg as immediate
                      aplLongestRht;        // Right ...
    HGLOBAL           hGlbLft = NULL,       // Left  arg global memory handle
                      hGlbRht = NULL,       // Right ...
                      hGlbRes = NULL;       // Result    ...
    LPVARARRAY_HEADER lpMemHdrLft = NULL,   // Ptr to left arg header
                      lpMemHdrRht = NULL,   // ...    right ...
                      lpMemHdrRes = NULL;   // ...    result   ...
    LPAPLCHAR         lpMemRht,             // Ptr to right ...
                      lpMemDataStart;       // Ptr to start of identifier
    LPAPLBOOL         lpMemRes;             // Ptr to result    ...
    APLUINT           ByteRes,              // # bytes in the result
                      uRht,                 // Loop counter
                      uCol;                 // Loop counter
    LPSYMENTRY        lpSymEntry;           // Ptr to SYMENTRY
    STFLAGS           stFlags;              // STE flags
    UBOOL             bRet = TRUE;          // TRUE iff the result is valid
    UINT              uBitIndex;            // Bit index for looping through Booleans
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result

    // The right arg may be of three forms:
    //   1.  a scalar    name  as in 'a'
    //   2.  a vector of names as in 'a' or 'a b c'
    //   3.  a matrix of names as in 3 1{rho}'abc'

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for LEFT RANK ERROR
    if (IsMultiRank (aplRankLft))
        goto LEFT_RANK_EXIT;

    // Check for RIGHT RANK ERROR
    if (IsRank3P (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Check for LEFT LENGTH ERROR
    if (IsMultiNELM (aplNELMLft))
        goto LEFT_LENGTH_EXIT;

    // Check for LEFT DOMAIN ERROR
    if (!IsNumeric (aplTypeLft))
        goto LEFT_DOMAIN_EXIT;

    // Check for RIGHT DOMAIN ERROR
    if (!IsCharOrEmpty (aplTypeRht, aplNELMRht))
        goto RIGHT_DOMAIN_EXIT;

    // Get left & right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // Check the left arg
    // Split cases based upon the left arg storage type
    switch (aplTypeLft)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            break;

        case ARRAY_FLOAT:
            // Attempt to convert the float to an integer using System []CT
            aplLongestLft = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestLft,
                                              &bRet);
            break;

        case ARRAY_RAT:
            // Attempt to convert the RAT to an integer using System []CT
            aplLongestLft = GetNextRatIntGlb (MakePtrTypeGlb (hGlbLft), 0, &bRet);

            break;

        case ARRAY_VFP:
            // Attempt to convert the VFP to an integer using System []CT
            aplLongestLft = GetNextVfpIntGlb (MakePtrTypeGlb (hGlbLft), 0, &bRet);

            break;

        defstop
            break;
    } // End SWITCH

    if (!bRet)
        goto LEFT_DOMAIN_EXIT;

    // Check for LEFT DOMAIN ERROR
    if (!IsBooleanValue (aplLongestLft))
        goto LEFT_DOMAIN_EXIT;

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
    if (!bRet)
        goto RIGHT_DOMAIN_EXIT;

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

#define lpHeader        lpMemHdrRes
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_BOOL;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Fill in the dimension
    (VarArrayBaseToDim (lpMemHdrRes))[0] = aplNELMRes;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Initialize the bit index
    uBitIndex = 0;

    // Split cases based upon the right arg rank
    switch (aplRankRht)
    {
        case 0:
            // Lookup the name in the symbol table
            // SymTabLookupName sets the .ObjName enum,
            //   and the .Inuse flag
            ZeroMemory (&stFlags, sizeof (stFlags));
            lpSymEntry = SymTabLookupNameLength ((LPAPLCHAR) &aplLongestRht,
                                                 1,
                                                &stFlags);
            // If found and a valid name, ...
            if (lpSymEntry
             && (!IsSysName ((LPAPLCHAR) &aplLongestRht))
             && IsValidName ((LPAPLCHAR) &aplLongestRht, 1))
                *lpMemRes |= ToggleMonInfo (lpSymEntry, (UBOOL) aplLongestLft) << uBitIndex;

////////////// Check for end-of-byte
////////////if (++uBitIndex EQ NBIB)
////////////{
////////////    uBitIndex = 0;              // Start over
////////////    lpMemRes++;                 // Skip to next byte
////////////} // End IF

            break;

        case 1:
            // Skip over the header and dimensions to the data
            lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

            // Loop through the right arg looking for identifiers
            uRht = 0;
            while (TRUE)
            {
                // Skip over white space
                while (uRht < aplNELMRht && lpMemRht[uRht] EQ L' ')
                    uRht++;
                if (uRht < aplNELMRht)
                {
                    // Save the starting ptr
                    lpMemDataStart = &lpMemRht[uRht];

                    // Skip over black space
                    while (uRht < aplNELMRht && lpMemRht[uRht] NE L' ')
                        uRht++;
                    // Lookup the name in the symbol table
                    // SymTabLookupName sets the .ObjName enum,
                    //   and the .Inuse flag
                    ZeroMemory (&stFlags, sizeof (stFlags));
                    lpSymEntry = SymTabLookupNameLength (lpMemDataStart,
                                              (APLU3264) (&lpMemRht[uRht] - lpMemDataStart),
                                                        &stFlags);
                    // If found and a valid name, ...
                    if (lpSymEntry
                     && (!IsSysName (lpMemDataStart))
                     && IsValidName (lpMemDataStart, (APLU3264) (&lpMemRht[uRht] - lpMemDataStart)))
                        *lpMemRes |= ToggleMonInfo (lpSymEntry, (UBOOL) aplLongestLft) << uBitIndex;

                    // Check for end-of-byte
                    if (++uBitIndex EQ NBIB)
                    {
                        uBitIndex = 0;              // Start over
                        lpMemRes++;                 // Skip to next byte
                    } // End IF
                } else
                    break;
            } // End WHILE

            break;

        case 2:
            // Skip over the header and dimensions to the data
            lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

            for (uRht = 0; uRht < aplNELMRes; uRht++)
            {
                // Point to the start of the data
                lpMemDataStart = &lpMemRht[aplNELMCol * uRht];

                // Skip over leading white space
                uCol = 0;
                while (uCol < aplNELMCol && lpMemDataStart[uCol] EQ L' ')
                    uCol++;

                // Lookup the name in the symbol table
                // SymTabLookupName sets the .ObjName enum,
                //   and the .Inuse flag
                ZeroMemory (&stFlags, sizeof (stFlags));
                lpSymEntry = SymTabLookupNameLength (&lpMemDataStart[uCol],
                                          (APLU3264) (aplNELMCol - uCol),
                                                     &stFlags);
                // If found and a valid name, ...
                if (lpSymEntry
                 && (!IsSysName (&lpMemDataStart[uCol]))
                 && IsValidName (&lpMemDataStart[uCol], (APLU3264) (aplNELMCol - uCol)))
                    *lpMemRes |= ToggleMonInfo (lpSymEntry, (UBOOL) aplLongestLft) << uBitIndex;

                // Check for end-of-byte
                if (++uBitIndex EQ NBIB)
                {
                    uBitIndex = 0;              // Start over
                    lpMemRes++;                 // Skip to next byte
                } // End IF
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
    lpYYRes->tkToken.tkFlags.NoDisplay = TRUE;
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

LEFT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkLftArg);
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

    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnDydMF_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ToggleMonInfo
//
//  Toggle (ON/OFF) the function line monitoring state of a function
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ToggleMonInfo"
#else
#define APPEND_NAME
#endif

UBOOL ToggleMonInfo
    (LPSYMENTRY lpSymEntry,             // Ptr to STE of the function
     UBOOL      bMonOn)                 // New MonInfo bit setting

{
    HGLOBAL      hGlbDfnHdr,            // Function global memory handle
                 hGlbMonInfo;           // MonInfo  ...
    LPDFN_HEADER lpMemDfnHdr;           // Ptr to function header global memory
    LPINTMONINFO lpMemMonInfo;          // Ptr to function line monitoring info
    UBOOL        bRet = FALSE;          // TRUE iff the result is valid
    UINT         uLineCnt;              // # function lines including the header

    // Check for user-defined function/operator
    if ((lpSymEntry->stFlags.ObjName NE OBJNAME_USR
      && lpSymEntry->stFlags.ObjName NE OBJNAME_MFO)
     || !IsNameTypeFnOp (lpSymEntry->stFlags.stNameType))
        return FALSE;

    // Get the global memory handle
    hGlbDfnHdr = lpSymEntry->stData.stGlbData;

    Assert (IsGlbTypeDfnDir_PTB (hGlbDfnHdr));

    // Lock the memory to get a ptr to it
    lpMemDfnHdr = MyGlobalLockDfn (hGlbDfnHdr);

    // Get # lines including the header
    uLineCnt = lpMemDfnHdr->numFcnLines + 1;

    // Set/reset the bit
    lpMemDfnHdr->MonOn = bMonOn;

    // Get the MonInfo global memory handle
    hGlbMonInfo = lpMemDfnHdr->hGlbMonInfo;

    // If we're enabling monitoring, zero the info (if already allocated),
    //   or allocate new memory (if not already allocated)
    if (bMonOn)
    {
        if (hGlbMonInfo NE NULL)
        {
            // Lock the memory to get a ptr to it
            lpMemMonInfo = MyGlobalLock (hGlbMonInfo);

            // Zap the memory
            ZeroMemory (lpMemMonInfo, uLineCnt * sizeof lpMemMonInfo[0]);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbMonInfo); lpMemMonInfo = NULL;
        } else
        {
            // Allocate new memory
            hGlbMonInfo = DbgGlobalAlloc (GHND, uLineCnt * sizeof lpMemMonInfo[0]);
            if (hGlbMonInfo NE NULL)
                lpMemDfnHdr->hGlbMonInfo = hGlbMonInfo;
            else
                goto ERROR_EXIT;
        } // End IF/ELSE
    } else
    // If we're disabling monitoring, free the global memory handle
    if (hGlbMonInfo NE NULL)
    {
        DbgGlobalFree (hGlbMonInfo); lpMemDfnHdr->hGlbMonInfo = hGlbMonInfo = NULL;
    } // End IF/ELSE/...

    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;

    return bRet;
} // End ToggleMonInfo
#undef  APPEND_NAME


//***************************************************************************
//  $StartStopMonInfo
//
//  Start/stop monitoring a given function line
//***************************************************************************

void StartStopMonInfo
    (LPDFN_HEADER lpMemDfnHdr,          // Ptr to user-defined function/operator header
     UINT         uLineNum,             // Function line # (origin-1)
     UBOOL        bStartMon)            // TRUE iff we're starting

{
    HGLOBAL       hGlbMonInfo;          // MonInfo global memory handle
    LPINTMONINFO  lpMemMonInfo;         // Ptr to function line monitoring info
    LARGE_INTEGER aplTickCnt;           // Current tick count

    // Get the MonInfo global memory handle
    hGlbMonInfo = lpMemDfnHdr->hGlbMonInfo;

    // Lock the memory to get a ptr to it
    lpMemMonInfo = MyGlobalLock (hGlbMonInfo);

    Assert (lpMemMonInfo[uLineNum].IncActive NE bStartMon);
    Assert (lpMemMonInfo[uLineNum].ExcActive NE bStartMon);

    // Get the current tick count
    GetPerformanceCount (&aplTickCnt);

    // Mark as active/inactive
    lpMemMonInfo[uLineNum].IncActive =
    lpMemMonInfo[uLineNum].ExcActive = bStartMon;

    // If we're starting, subtract the count
    if (bStartMon)
        aplTickCnt.QuadPart = -aplTickCnt.QuadPart;

    // Start/stop the count
    lpMemMonInfo[uLineNum].IncSubFns += aplTickCnt.QuadPart;
////lpMemMonInfo[uLineNum].ExcSubFns += aplTickCnt.QuadPart;

    // Increment the line count
    lpMemMonInfo[uLineNum].Count += bStartMon;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbMonInfo); lpMemMonInfo = NULL;
} // End StartStopMonInfo


//***************************************************************************
//  $GetPerformanceCount
//
//  Return the performance count depending upon the timer source
//***************************************************************************

void GetPerformanceCount
    (LARGE_INTEGER *lpaplTickCnt)           // Ptr to current tick count

{
    // Split cases based upon the timer source
    switch (GetMemPTD ()->uQuadMF)
    {
        case TIMERSOURCE_MS:    // Millisecond counter
            // Get current tick count
            lpaplTickCnt->QuadPart = GetTickCount ();

            break;

        case TIMERSOURCE_PC:    // QueryPerformance counter
        case TIMERSOURCE_PC2MS: // QueryPerformance counter, scaled to ms
            // Get current tick count
            QueryPerformanceCounter (lpaplTickCnt);

            break;

        defstop
            break;
    } // End SWITCH
} // End GetPerformanceCount


//***************************************************************************
//  End of File: qf_mf.c
//***************************************************************************
