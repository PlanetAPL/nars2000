//***************************************************************************
//  NARS2000 -- Primitive Function -- Iota
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

// Define the following var in order to grade the secondary
//   argument so as to catch duplicate values.
//   It's probably not worth the effort, so this option is
//     not recommended.
////#define GRADE2ND


//***************************************************************************
//  $PrimFnIota_EM_YY
//
//  Primitive function for monadic and dyadic iota ("index generator" and "index of")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnIota_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnIota_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_IOTA);

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
        return PrimFnMonIota_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydIota_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimFnIota_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnIota_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic Iota
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnIota_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnIota_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnIota_EM_YY,   // Ptr to primitive function routine
                                    lptkLftArg,         // Ptr to left arg token
                                    lptkFunc,           // Ptr to function token
                                    lptkRhtArg,         // Ptr to right arg token
                                    lptkAxis);          // Ptr to axis token (may be NULL)
} // End PrimProtoFnIota_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonIota_EM_YY
//
//  Primitive function for monadic iota ("index generator")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonIota_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonIota_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE      aplTypeRht,       // Right arg storage type
                  aplTypeRes;       // Result    ...
    APLNELM       aplNELMRht;       // Right arg NELM
    APLRANK       aplRankRht;       // Right arg rank
    HGLOBAL       hGlbRht,          // Right arg global memory handle
                  hGlbRes,          // Result    ...
                  lpSymGlbRht;      // Right arg as rational
    APLUINT       ByteRes;          // # bytes in the result
    APLINT        iRes,             // Loop counter
                  iLim;             // Loop limit
    LPVOID        lpMemRes;         // Ptr to result global memory
    UBOOL         bRet;             // TRUE iff result is valid
    APLLONGEST    aplLongestRht;    // Right arg iommediate value
    LPPL_YYSTYPE  lpYYRes = NULL;   // Ptr to the result
    APLBOOL       bQuadIO;          // []IO
    LPPERTABDATA  lpMemPTD;         // Ptr to PerTabData global memory
    LPPLLOCALVARS lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;     // Ptr to Ctrl-Break flag

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    //***************************************************************
    // Check the right argument for RANK, LENGTH, and DOMAIN ERRORs
    //***************************************************************
    if (IsMultiRank (aplRankRht))
        goto RANK_EXIT;

    if (!IsNumeric (aplTypeRht))
        goto DOMAIN_EXIT;

    // Handle length != 1 args via magic function/operator
    if (!IsSingleton (aplNELMRht))
        return PrimFnMonIotaVector_EM_YY (lptkFunc,     // Ptr to function token
                                          lptkRhtArg,   // Ptr to right arg token
                                          lptkAxis);    // Ptr to axis token (may be NULL)
    // Get right arg global ptrs
    aplLongestRht = GetGlbPtrs (lptkRhtArg, &hGlbRht);

    // If it's a global, get the first value
    if (hGlbRht)
        GetFirstValueGlb (hGlbRht,          // The global memory handle
                          NULL,             // Ptr to integer (or Boolean) (may be NULL)
                          NULL,             // ...    float (may be NULL)
                          NULL,             // ...    char (may be NULL)
                         &aplLongestRht,    // ...    longest (may be NULL)
                         &lpSymGlbRht,      // ...    LPSYMENTRY or HGLOBAL (may be NULL)
                          NULL,             // ...    immediate type (see IMM_TYPES) (may be NULL)
                          NULL,             // ...    array type -- ARRAY_TYPES (may be NULL)
                          FALSE);           // TRUE iff we should expand LPSYMENTRY into immediate value
    // The singleton value is in aplLongestRht or lpSymGlbRht

    // Split cases based upon the storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            // Set the result storage type
            aplTypeRes = ARRAY_APA;

            break;

        case ARRAY_RAT:
            // Attempt to convert the RAT to an APLINT using System []CT
            aplLongestRht = mpq_get_sctsx ((LPAPLRAT) lpSymGlbRht, &bRet);
            if (!bRet)
                goto DOMAIN_EXIT;

            // Set the result storage type
            aplTypeRes = ARRAY_RAT;

            break;

        case ARRAY_VFP:
            // Attempt to convert the VFP to an APLINT using System []CT
            aplLongestRht = mpfr_get_sctsx ((LPAPLVFP) lpSymGlbRht, &bRet);
            if (!bRet)
                goto DOMAIN_EXIT;

            // Set the result storage type
            aplTypeRes = ARRAY_VFP;

            break;

        case ARRAY_FLOAT:
            // Attempt to convert the float to an integer using System []CT
            aplLongestRht = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestRht, &bRet);
            if (!bRet)
                goto DOMAIN_EXIT;

            // Set the result storage type
            aplTypeRes = ARRAY_APA;

            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
        defstop
            break;
    } // End SWITCH

    // If negative indices are not allowed, ...
    if (!lpMemPTD->aplCurrentFEATURE[FEATURENDX_NEGINDICES])
    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
        case ARRAY_FLOAT:
            // If the value is negative, ...
            if (SIGN_APLLONGEST (aplLongestRht))
                goto DOMAIN_EXIT;
            break;

        case ARRAY_RAT:
            // If the value is negative, ...
            if (mpz_sgn (mpq_numref ((LPAPLRAT) lpSymGlbRht)) < 0)
                goto DOMAIN_EXIT;
            break;

        case ARRAY_VFP:
            // If the value is negative, ...
            if (mpfr_sgn            ((LPAPLVFP) lpSymGlbRht)  < 0)
                goto DOMAIN_EXIT;
            break;

        defstop
            break;
    } // End SWITCH

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, abs64 (aplLongestRht), 1);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for an APA
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
    lpHeader->PV0        = (bQuadIO EQ 0) && !SIGN_APLLONGEST(aplLongestRht);
    lpHeader->PV1        = (bQuadIO EQ 1) && !SIGN_APLLONGEST(aplLongestRht);
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = abs64 (aplLongestRht);
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Save the dimension in the result
    *VarArrayBaseToDim (lpMemRes) = abs64 (aplLongestRht);

    // Skip over the header and dimensions to the data (APLAPA struct)
    lpMemRes = VarArrayDataFmBase (lpMemRes);

    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        case ARRAY_APA:
            // Save the APA values
#define lpAPA       ((LPAPLAPA) lpMemRes)
            // Handle negative indices
            if (SIGN_APLINT (aplLongestRht))
                lpAPA->Off = bQuadIO + aplLongestRht;
            else
                lpAPA->Off = bQuadIO;

            lpAPA->Mul = 1;
#undef  lpAPA
            break;

        case ARRAY_RAT:
            // Save the loop limit
            iLim = (APLINT) (bQuadIO + abs64 (aplLongestRht));

            if (SIGN_APLINT (aplLongestRht))
                // Loop through the result
                for (iRes = bQuadIO; iRes < iLim; iRes++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Initialize and save the value
                    mpq_init_set_sx (((LPAPLRAT) lpMemRes)++, aplLongestRht + iRes, 1);
                } // End FOR
            else
                // Loop through the result
                for (iRes = bQuadIO; iRes < iLim; iRes++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Initialize and save the value
                    mpq_init_set_sx (((LPAPLRAT) lpMemRes)++,                 iRes, 1);
                } // End FOR
            break;

        case ARRAY_VFP:
            // Save the loop limit
            iLim = (APLINT) (bQuadIO + abs64 (aplLongestRht));

            if (SIGN_APLINT (aplLongestRht))
                // Loop through the result
                for (iRes = bQuadIO; iRes < iLim; iRes++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Initialize and save the value
                    mpfr_init_set_sx (((LPAPLVFP) lpMemRes)++, aplLongestRht + iRes, MPFR_RNDN);
                } // End FOR
            else
                // Loop through the result
                for (iRes = bQuadIO; iRes < iLim; iRes++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Initialize and save the value
                    mpfr_init_set_sx (((LPAPLVFP) lpMemRes)++,                 iRes, MPFR_RNDN);
                } // End FOR
            break;

        defstop
            break;
    } // End SWITCH


    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

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
NORMAL_EXIT:
    return lpYYRes;
} // End PrimFnMonIota_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonIotaVector_EM_YY
//
//  Monadic iota extended to length != 1 numeric arguments
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonIotaVector_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonIotaVector_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLNELM      aplNELMRht;        // Right arg NELM
    APLUINT      ByteRes;           // # bytes in the result
    LPVOID       lpMemRes;          // Ptr to result global memory
    HGLOBAL      hGlbRes,           // Result global memory handle
                 hGlbMFO;           // Magic function/operator ...
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result

    Assert (lptkAxis EQ NULL);

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, NULL, &aplNELMRht, NULL, NULL);

    // Handle length zero args directly
    if (IsEmpty (aplNELMRht))
    {
        // Calculate space needed for the result
        ByteRes = CalcArraySize (ARRAY_NESTED, 1, 0);

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
        lpHeader->ArrType    = ARRAY_NESTED;
////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////lpHeader->SysVar     = FALSE;       // Already zero from GHND
        lpHeader->PV0        =
        lpHeader->PV1        = FALSE;
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = 1;
        lpHeader->Rank       = 0;
#undef  lpHeader

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemRes);

        // Save the only element {zilde}
        *((LPAPLNESTED) lpMemRes) = MakePtrTypeGlb (hGlbZilde);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

        goto YYALLOC_EXIT;
    } else
    {
        // Get ptr to PerTabData global memory
        lpMemPTD = GetMemPTD ();

        // Get the magic function/operator global memory handle
        hGlbMFO = lpMemPTD->hGlbMFO[MFOE_MonIota];

        //  Return the matrix of indices
        //  Use an internal magic function/operator.
        return
          ExecuteMagicFunction_EM_YY (NULL,         // Ptr to left arg token
                                      lptkFunc,     // Ptr to function token
                                      NULL,         // Ptr to function strand
                                      lptkRhtArg,   // Ptr to right arg token
                                      lptkAxis,     // Ptr to axis token
                                      hGlbMFO,      // Magic function/operator global memory handle
                                      NULL,         // Ptr to HSHTAB struc (may be NULL)
                                      LINENUM_ONE); // Starting line # type (see LINE_NUMS)
    } // End IF/ELSE

YYALLOC_EXIT:
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    return lpYYRes;
} // End PrimFnMonIotaVector_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  Magic function/operator for Extended Monadic Iota
//
//  Extended Monadic Iota -- Index Generator For Arrays
//
//  Return an array of indices appropriate to an array of
//    the same shape as the right arg.
//
//  I believe this algorithm was first created by Carl M. Cheney.
//***************************************************************************

//***************************************************************************
//  Magic function/operator for extended dyadic iota
//
//  Extended dyadic iota
//
//  On rank > 1 left args, return an array of vector indices
//    such that A[A iota R] is R, assuming that all of R is in A.
//***************************************************************************

#include "mf_iota.h"


//***************************************************************************
//  $PrimFnDydIota_EM_YY
//
//  Primitive function for dyadic iota ("index of")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydIota_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydIota_EM_YY
    (LPTOKEN lptkLftArg,                // Ptr to left arg token
     LPTOKEN lptkFunc,                  // Ptr to function token
     LPTOKEN lptkRhtArg,                // Ptr to right arg token
     LPTOKEN lptkAxis)                  // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeLft,       // Left arg storage type
                      aplTypeRht;       // Right ...
    APLNELM           aplNELMLft,       // Left arg NELM
                      aplNELMRht;       // Right ...
    APLRANK           aplRankLft,       // Left arg rank
                      aplRankRht;       // Right ...
    HGLOBAL           hGlbLft = NULL,   // Left arg global memory handle
                      hGlbRht = NULL,   // Right ...
                      hGlbRes = NULL;   // Result   ...
    LPVARARRAY_HEADER lpHeaderLft;      // Ptr to left arg header
    LPVOID            lpMemLft = NULL,  // Ptr to left arg global memory
                      lpMemRht = NULL;  // Ptr to right ...
    LPAPLUINT         lpMemRes = NULL;  // Ptr to result   ...
    UBOOL             bQuadIO;          // []IO
    APLFLOAT          fQuadCT;          // []CT
    APLUINT           NotFound;         // Not found value
    APLUINT           ByteRes;          // # bytes in the result
    APLLONGEST        aplLongestLft,    // Left arg immediate value
                      aplLongestRht;    // Right ...
    LPPL_YYSTYPE      lpYYRes = NULL;   // Ptr to the result
    LPPLLOCALVARS     lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;     // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    Assert (lptkAxis EQ NULL);

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for extended dyadic iota
    if (!IsVector (aplRankLft))
    {
        HGLOBAL hGlbMFO;                // Magic function/operator global memory handle

        // Get the magic function/operator global memory handle
        hGlbMFO = GetMemPTD ()->hGlbMFO[MFOE_DydIota];

        //  Extend to aplRankLft > 1 args by returning
        //    an array of index vectors where the length
        //    of each vector is aplRankLft.
        //  Use an internal magic function/operator.
        return
          ExecuteMagicFunction_EM_YY (lptkLftArg,   // Ptr to left arg token
                                      lptkFunc,     // Ptr to function token
                                      NULL,         // Ptr to function strand
                                      lptkRhtArg,   // Ptr to right arg token
                                      lptkAxis,     // Ptr to axis token
                                      hGlbMFO,      // Magic function/operator global memory handle
                                      NULL,         // Ptr to HSHTAB struc (may be NULL)
                                      LINENUM_ONE); // Starting line # type (see LINE_NUMS)
    } // End IF

    // Get left and right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    //***************************************************************
    // Calculate space needed for the result
    //***************************************************************
    ByteRes = CalcArraySize (ARRAY_INT, aplNELMRht, aplRankRht);

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

    // Save ptr to left arg header
    lpHeaderLft = lpMemLft;

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_INT;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRht;
    lpHeader->Rank       = aplRankRht;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = (LPAPLUINT) VarArrayBaseToDim (lpMemRes);

    // Fill in the result's dimension
    if (lpMemRht)
    {
        // Skip over the header to the dimensions
        lpMemRht = VarArrayBaseToDim (lpMemRht);

        // Copy the left arg dimensions to the result
        CopyMemory (lpMemRes, lpMemRht, (APLU3264) aplRankRht * sizeof (APLDIM));

        // Skip over the dimensions to the data
        lpMemRes = VarArrayDimToData (lpMemRes, aplRankRht);
        lpMemRht = VarArrayDimToData (lpMemRht, aplRankRht);
    } else
        // Point to the right arg immediate value
        lpMemRht = &aplLongestRht;

    if (lpMemLft)
        // Skip over the header and dimensions to the data
        lpMemLft = VarArrayDataFmBase (lpMemLft);
    else
        // Point to the left arg immediate value
        lpMemLft = &aplLongestLft;

    // Get the current value of []IO & []CT
    bQuadIO = GetQuadIO ();
    fQuadCT = GetQuadCT ();

    // Calculate the NotFound value
    NotFound = bQuadIO + aplNELMLft;

    // If the left arg is non-empty, and
    //    the right arg is non-empty, and
    //    the left and right args are not different types (numeric vs. char), ...
    if (!(IsEmpty (aplNELMLft)
       || IsEmpty (aplNELMRht)
       || (IsNumeric (aplTypeLft) && IsSimpleChar (aplTypeRht))
       || (IsNumeric (aplTypeRht) && IsSimpleChar (aplTypeLft))))
    {
        // Split cases based upon the left & right arg storage types
        if (IsSimpleBool (aplTypeLft) && IsSimpleBool (aplTypeRht))
        {
            // Handle APLBOOL vs. APLBOOL
            if (!PrimFnDydIotaBvB (lpMemRes,        // Ptr to result global memory data
                                   aplNELMLft,      // Left arg NELM
                                   lpMemLft,        // Ptr to left arg global memory data
                                   aplNELMRht,      // Right arg NELM
                                   lpMemRht,        // Ptr to right arg global memory data
                                   bQuadIO,         // []IO
                                   NotFound,        // Not found value
                                   lpbCtrlBreak))   // Ptr to Ctrl-Break flag
                goto ERROR_EXIT;
        } else
        if (IsSimpleBool (aplTypeLft) && IsNumeric (aplTypeRht))
        {
            // Handle APLBOOL vs. APLINT/APLAPA/APLFLOAT/APLRAT/APLVFP
            if (!PrimFnDydIotaBvN (lpMemRes,        // Ptr to result global memory data
                                   aplNELMLft,      // Left arg NELM
                                   lpMemLft,        // Ptr to left arg global memory data
                                   aplTypeRht,      // Right arg storage type
                                   aplNELMRht,      // Right arg NELM
                                   lpMemRht,        // Ptr to right arg global memory data
                                   bQuadIO,         // []IO
                                   fQuadCT,         // []CT
                                   NotFound,        // Not found value
                                   lpbCtrlBreak))   // Ptr to Ctrl-Break flag
                goto ERROR_EXIT;
        } else
        if (IsPermVector (lpHeaderLft) && IsNumeric (aplTypeRht))
        {
            // Handle PV vs. APLBOOL/APLINT/APLAPA/APLFLOAT/APLRAT/APLVFP
            if (!PrimFnDydIotaPvN_EM (lpMemRes,     // Ptr to result global memory data
                                      lpHeaderLft,  // Ptr to left arg header
                                      aplTypeLft,   // Left arg storage type
                                      aplNELMLft,   // Left arg NELM
                                      lpMemLft,     // Ptr to left arg global memory data
                                      aplTypeRht,   // Right arg storage type
                                      aplNELMRht,   // Right arg NELM
                                      lpMemRht,     // Ptr to right arg global memory data
                                      bQuadIO,      // []IO
                                      fQuadCT,      // []CT
                                      NotFound,     // Not found value
                                      lpbCtrlBreak, // Ptr to Ctrl-Break flag
                                      lptkFunc))    // Ptr to function token
                goto ERROR_EXIT;
        } else
        if (IsSimpleAPA (aplTypeLft) && IsNumeric (aplTypeRht))
        {
            // Handle APLAPA vs. APLBOOL/APLINT/APLAPA/APLFLOAT/APLRAT/APLVFP
            if (!PrimFnDydIotaAvN_EM (lpMemRes,     // Ptr to result global memory data
                                      aplNELMLft,   // Left arg NELM
                                      lpMemLft,     // Ptr to left arg global memory data
                                      aplTypeRht,   // Right arg storage type
                                      aplNELMRht,   // Right arg NELM
                                      lpMemRht,     // Ptr to right arg global memory data
                                      bQuadIO,      // []IO
                                      fQuadCT,      // []CT
                                      NotFound,     // Not found value
                                      lpbCtrlBreak, // Ptr to Ctrl-Break flag
                                      lptkFunc))    // Ptr to function token
                goto ERROR_EXIT;
        } else
        if (IsSimpleInt (aplTypeLft) && IsNumeric (aplTypeRht))
        {
            // Handle APLINT vs. APLBOOL/APLINT/APLAPA/APLFLOAT/APLRAT/APLVFP
            if (!PrimFnDydIotaIvN_EM (lpMemRes,     // Ptr to result global memory data
                                      lptkLftArg,   // Ptr to left arg token
                                      aplTypeLft,   // Left arg storage type
                                      aplNELMLft,   // Left arg NELM
                                      aplRankLft,   // Left arg rank
                                      lpMemLft,     // Ptr to left arg global memory data
                                      lptkRhtArg,   // Ptr to right arg token
                                      aplTypeRht,   // Right arg storage type
                                      aplNELMRht,   // Right arg NELM
                                      aplRankRht,   // Right arg rank
                                      lpMemRht,     // Ptr to right arg global memory data
                                      bQuadIO,      // []IO
                                      fQuadCT,      // []CT
                                      NotFound,     // Not found value
                                      lpbCtrlBreak, // Ptr to Ctrl-Break flag
                                      lptkFunc))    // Ptr to function token
                goto ERROR_EXIT;
        } else
        if (IsSimpleNum (aplTypeLft) && IsNumeric (aplTypeRht))
        {
            // Handle APLFLOAT vs. APLBOOL/APLINT/APLAPA/APLFLOAT/APLRAT/APLVFP
            if (!PrimFnDydIotaFvN_EM (lpMemRes,     // Ptr to result global memory data
                                      lptkLftArg,   // Ptr to left arg token
                                      aplTypeLft,   // Left arg storage type
                                      aplNELMLft,   // Left arg NELM
                                      lpMemLft,     // Ptr to left arg global memory data
                                      lptkRhtArg,   // Ptr to right arg token
                                      aplTypeRht,   // Right arg storage type
                                      aplNELMRht,   // Right arg NELM
                                      aplRankRht,   // Right arg rank
                                      lpMemRht,     // Ptr to right arg global memory data
                                      bQuadIO,      // []IO
                                      fQuadCT,      // []CT
                                      NotFound,     // Not found value
                                      lpbCtrlBreak, // Ptr to Ctrl-Break flag
                                      lptkFunc))    // Ptr to function token
                goto ERROR_EXIT;
        } else
        if (IsRat (aplTypeLft) && IsNumeric (aplTypeRht))
        {
            // Handle APLRAT vs. APLBOOL/APLINT/APLAPA/APLFLOAT/APLRAT/APLVFP
            if (!PrimFnDydIotaRvN_EM (lpMemRes,     // Ptr to result global memory data
                                      lptkLftArg,   // Ptr to left arg token
                                      aplTypeLft,   // Left arg storage type
                                      aplNELMLft,   // Left arg NELM
                           (LPAPLRAT) lpMemLft,     // Ptr to left arg global memory data
                                      lptkRhtArg,   // Ptr to right arg token
                                      aplTypeRht,   // Right arg storage type
                                      aplNELMRht,   // Right arg NELM
                                      aplRankRht,   // Right arg rank
                                      lpMemRht,     // Ptr to right arg global memory data
                                      bQuadIO,      // []IO
                                      fQuadCT,      // []CT
                                      NotFound,     // Not found value
                                      lpbCtrlBreak, // Ptr to Ctrl-Break flag
                                      lptkFunc))    // Ptr to function token
                goto ERROR_EXIT;
        } else
        if (IsVfp (aplTypeLft) && IsNumeric (aplTypeRht))
        {
            // Handle APLVFP vs. APLBOOL/APLINT/APLAPA/APLFLOAT/APLRAT/APLVFP
            if (!PrimFnDydIotaVvN_EM (lpMemRes,     // Ptr to result global memory data
                                      lptkLftArg,   // Ptr to left arg token
                                      aplTypeLft,   // Left arg storage type
                                      aplNELMLft,   // Left arg NELM
                           (LPAPLVFP) lpMemLft,     // Ptr to left arg global memory data
                                      lptkRhtArg,   // Ptr to right arg token
                                      aplTypeRht,   // Right arg storage type
                                      aplNELMRht,   // Right arg NELM
                                      aplRankRht,   // Right arg rank
                                      lpMemRht,     // Ptr to right arg global memory data
                                      bQuadIO,      // []IO
                                      fQuadCT,      // []CT
                                      NotFound,     // Not found value
                                      lpbCtrlBreak, // Ptr to Ctrl-Break flag
                                      lptkFunc))    // Ptr to function token
                goto ERROR_EXIT;
        } else
        if (IsSimpleChar (aplTypeLft) && IsSimpleChar (aplTypeRht))
        {
            // Handle APLCHAR vs. APLCHAR
            if (!PrimFnDydIotaCvC_EM (lpMemRes,     // Ptr to result global memory data
                                      aplNELMLft,   // Left arg NELM
                                      lpMemLft,     // Ptr to left arg global memory data
                                      aplNELMRht,   // Right arg NELM
                                      lpMemRht,     // Ptr to right arg global memory data
                                      bQuadIO,      // []IO
                                      NotFound,     // Not found value
                                      lpbCtrlBreak, // Ptr to Ctrl-Break flag
                                      lptkFunc))    // Ptr to function token
                goto ERROR_EXIT;
        } else
        {
            // Handle all other combinations
            // APLHETERO/APLNESTED vs. anything
            // anything            vs. APLHETERO/APLNESTED
            if (!PrimFnDydIotaOther (lpMemRes,      // Ptr to result global memory data
                                     aplTypeLft,    // Left arg storage type
                                     aplNELMLft,    // Left arg NELM
                                     lpMemLft,      // Ptr to left arg global memory data
                                     aplTypeRht,    // Right arg storage type
                                     aplNELMRht,    // Right arg NELM
                                     lpMemRht,      // Ptr to right arg global memory data
                                     bQuadIO,       // []IO
                                     fQuadCT,       // []CT
                                     NotFound,      // Not found value
                                     lpbCtrlBreak,  // Ptr to Ctrl-Break flag
                                     lptkFunc))     // Ptr to function token
                goto ERROR_EXIT;
        } // End IF/ELSE/...
    } else
    {
        APLUINT uRht;           // Loop counter

        // Fill the result with NotFound
        for (uRht = 0; uRht < aplNELMRht; uRht++)
            *lpMemRes++ = NotFound;
    } // End IF/ELSE

    // Unlock the result global memory in case TypeDemote actually demotes
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken);

    goto NORMAL_EXIT;

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
} // End PrimFnDydIota_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydIotaBvB
//
//  Dyadic iota of APLBOOL vs. APLBOOL
//***************************************************************************

UBOOL PrimFnDydIotaBvB
    (LPAPLUINT lpMemRes,            // Ptr to result global memory data
     APLNELM   aplNELMLft,          // Left arg NELM
     LPAPLBOOL lpMemLft,            // Ptr to left arg global memory data
     APLNELM   aplNELMRht,          // Right arg (and result) NELM
     LPAPLBOOL lpMemRht,            // Ptr to right arg global memory data
     UBOOL     bQuadIO,             // []IO
     APLUINT   NotFound,            // Not found value
     LPUBOOL   lpbCtrlBreak)        // Ptr to Ctrl-Break flag

{
    UBOOL   Found[2];               // TRUE iff there is at least one [0,1] in the right arg
    APLUINT Index[2],               // Index of the first [0,1] in the left arg
            BytesInLftBits,         // # bytes in the left arg data
            BytesInRhtBits,         // ...            right ...
            uBit,                   // The bit we found or are searching for
            uValid,                 // # valid bits in the arg
            uLft,                   // Loop counter
            uRht,                   // Loop counter
            uTmp;                   // Temporary
    UINT    uBitMaskRht;            // Bit mask for the right arg

    // The result is either all NotFounds           (if left arg is empty -- already ruled out), or
    //                      bQuadIO + Rht           (if left arg is all 1s), or
    //                      bQuadIO + 1 - Rht       (if left arg is all 0s), or
    //                      Index[bQuadIO + Rht]    (if left arg has at least one 0 and one 1)

    // Calculate the # bytes in the left arg (result) data
    BytesInLftBits = RoundUpBitsToBytes (aplNELMLft);

    // Calculate the # bytes in the right arg data
    BytesInRhtBits = RoundUpBitsToBytes (aplNELMRht);

    // Get the first bit from the left arg
    uBit = BIT0 & *(LPAPLBOOL) lpMemLft;

    // Save that information
    Found[uBit] = TRUE;
    Index[uBit] = bQuadIO;

    // Complement the value to get the bit we're searching for
    uBit = !uBit;

    // Search the left arg for uBit
    for (Found[uBit] = FALSE, Index[uBit] = bQuadIO, uLft = 0; uLft < (BytesInLftBits - 1); uLft++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Get the index of the next occurrence of uBit
        uTmp = FastBoolTrans[lpMemLft[uLft]][fbtFirst[uBit]];
        Index[uBit] += uTmp;

        // If the bit is found, ...
        if (uTmp < NBIB)
        {
            Found[uBit] = TRUE;

            break;
        } // End IF
    } // End FOR

    // If we didn't find uBit, check the last byte (may be short)
    if (!Found[uBit])
    {
        // Calculate the # valid bits in the last byte in the left arg
        uValid = (MASKLOG2NBIB & (aplNELMLft - 1)) + 1;

        // Get the index of the next occurrence of uBit
        uTmp = FastBoolTrans[lpMemLft[uLft] & ((BIT0 << uValid) - 1)][fbtFirst[uBit]];

        // If the bit is not found, ...
        if (uTmp EQ NBIB)
            // Use the NotFound value
            Index[uBit]  = NotFound;
        else
            Index[uBit] += uTmp;
    } // End IF

    // Initialize the bit mask
    uBitMaskRht = BIT0;

    // Loop through the right arg,
    //   saving in the result Index[0] for each 0 in the right arg, and
    //                        Index[1] for each 1 in the right arg
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Save the appropriate value in the result
        *lpMemRes++ = Index[(*lpMemRht & uBitMaskRht) NE 0];

        // Shift over the right bit mask
        uBitMaskRht <<= 1;

        // Check for end-of-byte
        if (uBitMaskRht EQ END_OF_BYTE)
        {
            uBitMaskRht = BIT0;     // Start over
            lpMemRht++;             // Skip to next byte
        } // End IF
    } // End FOR

    return TRUE;
ERROR_EXIT:
    return FALSE;
} // End PrimFnDydIotaBvB


//***************************************************************************
//  $PrimFnDydIotaBvN
//
//  Dyadic iota of APLBOOL vs. APLINT/APLAPA/APLFLOAT/APLRAT/APLVFP
//***************************************************************************

UBOOL PrimFnDydIotaBvN
    (LPAPLUINT lpMemRes,            // Ptr to result global memory data
     APLNELM   aplNELMLft,          // Left arg NELM
     LPAPLBOOL lpMemLft,            // Ptr to left arg global memory data
     APLSTYPE  aplTypeRht,          // Right arg storage type
     APLNELM   aplNELMRht,          // Right arg (and result) NELM
     LPAPLBOOL lpMemRht,            // Ptr to right arg global memory data
     UBOOL     bQuadIO,             // []IO
     APLFLOAT  fQuadCT,             // []CT
     APLUINT   NotFound,            // Not found value
     LPUBOOL   lpbCtrlBreak)        // Ptr to Ctrl-Break flag

{
    UBOOL    Found[2];              // TRUE iff there is at least one [0,1] in the right arg
    APLUINT  Index[2],              // Index of the first [0,1] in the left arg
             BytesInLftBits,        // # bytes in the left arg data
             uBit,                  // The bit we found or are searching for
             uValid,                // # valid bits in the arg
             uLft,                  // Loop counter
             uRht,                  // Loop counter
             uTmp;                  // Temporary
    APLINT   apaOff,                // APA offfset
             apaMul;                // ... multiplier
    APLFLOAT uFlt;                  // Temporary float
    UBOOL    bRet;                  // TRUE iff the result is valid

    // Calculate the # bytes in the left arg (result) data
    BytesInLftBits = RoundUpBitsToBytes (aplNELMLft);

    // Get the first bit from the left arg
    uBit = BIT0 & *(LPAPLBOOL) lpMemLft;

    // Save that information
    Found[uBit] = TRUE;
    Index[uBit] = bQuadIO;

    // Complent the value to get the bit we're searching for
    uBit = !uBit;

    // Search the left arg for uBit
    for (Found[uBit] = FALSE, Index[uBit] = bQuadIO, uLft = 0; uLft < (BytesInLftBits - 1); uLft++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Get the index of the next occurrence of uBit
        uTmp = FastBoolTrans[lpMemLft[uLft]][fbtFirst[uBit]];
        Index[uBit] += uTmp;

        // If the bit is found, ...
        if (uTmp < NBIB)
        {
            Found[uBit] = TRUE;

            break;
        } // End IF
    } // End FOR

    // If we didn't find uBit, check the last byte (may be short)
    if (!Found[uBit])
    {
        // Calculate the # valid bits in the last byte in the left arg
        uValid = (MASKLOG2NBIB & (aplNELMLft - 1)) + 1;

        // Get the index of the next occurrence of uBit
        uTmp = FastBoolTrans[lpMemLft[uLft] & ((BIT0 << uValid) - 1)][fbtFirst[uBit]];

        // If the bit is not found, ...
        if (uTmp EQ NBIB)
            // Use the NotFound value
            Index[uBit]  = NotFound;
        else
            Index[uBit] += uTmp;
    } // End IF

    // If the right arg is APA, get its parameters
    if (IsSimpleAPA (aplTypeRht))
    {
#define lpaplAPA        ((LPAPLAPA) lpMemRht)
        apaOff = lpaplAPA->Off;
        apaMul = lpaplAPA->Mul;
#undef  lpaplAPA
    } // End IF

    // Loop through the right arg,
    //   saving in the result Index[0] for each 0 in the right arg, and
    //                        Index[1] for each 1 in the right arg, and
    //                        NotFound for each non-Boolean ...
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_INT:
                // Get the next right arg value
                uTmp = *((LPAPLINT) lpMemRht)++;

                // Split cases based upon the right arg value
                if (IsBooleanValue (uTmp))
                    // Save the appropriate value in the result
                    *lpMemRes++ = Index[uTmp];
                else
                    // Save the appropriate value in the result
                    *lpMemRes++ = NotFound;
                break;

            case ARRAY_APA:
                // Get the next right arg value
                uTmp = apaOff + apaMul * uRht;

                // Split cases based upon the right arg value
                if (IsBooleanValue (uTmp))
                    // Save the appropriate value in the result
                    *lpMemRes++ = Index[uTmp];
                else
                    // Save the appropriate value in the result
                    *lpMemRes++ = NotFound;
                break;

            case ARRAY_FLOAT:
                // Get the next right arg value
                uFlt = *((LPAPLFLOAT) lpMemRht)++;

                if (uFlt EQ 0)
                    // Save the appropriate value in the result
                    *lpMemRes++ = Index[0];
                else
                if (CompareCT (uFlt, 1.0, fQuadCT, NULL))
                    // Save the appropriate value in the result
                    *lpMemRes++ = Index[1];
                else
                    // Save the appropriate value in the result
                    *lpMemRes++ = NotFound;
                break;

            case ARRAY_RAT:
                // Exact vs. exact doesn't use []CT
                // Attempt to convert the RAT to an APLINT
                uTmp = mpq_get_sx (&((LPAPLRAT) lpMemRht)[uRht], &bRet);
                if (bRet && IsBooleanValue (uTmp))
                    // Save the appropriate value in the result
                    *lpMemRes++ = Index[uTmp];
                else
                    // Save the appropriate value in the result
                    *lpMemRes++ = NotFound;
                break;

            case ARRAY_VFP:
                // Attempt to convert the VFP to an APLINT using []CT
                uTmp = mpfr_get_ctsx (&((LPAPLVFP) lpMemRht)[uRht], fQuadCT, &bRet);
                if (bRet && IsBooleanValue (uTmp))
                    // Save the appropriate value in the result
                    *lpMemRes++ = Index[uTmp];
                else
                    // Save the appropriate value in the result
                    *lpMemRes++ = NotFound;
                break;

            defstop
                break;
        } // End SWITCH
    } // End FOR

    return TRUE;
ERROR_EXIT:
    return FALSE;
} // End PrimFnDydIotaBvN


//***************************************************************************
//  $PrimFnDydIotaAvN_EM
//
//  Dyadic iota of APLAPA vs. APLBOOL/APLINT/APLAPA/APLFLOAT/APLRAT/APLVFP
//***************************************************************************

UBOOL PrimFnDydIotaAvN_EM
    (LPAPLUINT lpMemRes,                // Ptr to result global memory data
     APLNELM   aplNELMLft,              // Left arg NELM
     LPAPLAPA  lpMemLft,                // Ptr to left arg global memory data
     APLSTYPE  aplTypeRht,              // Right arg storage type
     APLNELM   aplNELMRht,              // Right arg NELM
     LPVOID    lpMemRht,                // Ptr to right arg global memory data
     UBOOL     bQuadIO,                 // []IO
     APLFLOAT  fQuadCT,             // []CT
     APLUINT   NotFound,                // Not found value
     LPUBOOL   lpbCtrlBreak,            // Ptr to Ctrl-Break flag
     LPTOKEN   lptkFunc)                // Ptr to function token

{
    APLINT       iRht,                  // Loop counter
                 apaOffLft,             // Left arg APA offset
                 apaMulLft,             // ...          multiplier
                 apaMinLft,             // ...          minimum value
                 apaMaxLft,             // ...          maximum ...
                 aplIntegerRht;         // Right arg integer
    UBOOL        bRet = FALSE;          // TRUE iff the result is valid

    // Get the left arg APA parameters
    apaOffLft = lpMemLft->Off;
    apaMulLft = lpMemLft->Mul;

    if (apaMulLft >= 0)
    {
        apaMinLft = apaOffLft;
        apaMaxLft = apaOffLft + (aplNELMLft - 1) * apaMulLft;
    } else
    {
        apaMinLft = apaOffLft + (aplNELMLft - 1) * apaMulLft;
        apaMaxLft = apaOffLft;
    } // End IF/ELSE

    // Loop through the right arg values and look'em up
    //   in the left arg
    for (iRht = 0; iRht < (APLINT) aplNELMRht; iRht++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
                // Get the next integer from the right arg
                aplIntegerRht =
                  GetNextInteger (lpMemRht,     // Ptr to global memory
                                  aplTypeRht,   // Storage type
                                  iRht);        // Index
                // Mark as an integer
                bRet = TRUE;

                break;

            case ARRAY_FLOAT:
                // Attempt to convert the float to an integer using []CT
                aplIntegerRht =
                  FloatToAplint_CT (*((LPAPLFLOAT) lpMemRht)++, fQuadCT, &bRet);

                break;

            case ARRAY_RAT:
                // Exact vs. exact doesn't use []CT
                // Attempt to convert the RAT to an APLINT
                aplIntegerRht = mpq_get_sx (((LPAPLRAT) lpMemRht)++, &bRet);

                break;

            case ARRAY_VFP:
                // Attempt to convert the VFP to an APLINT using []CT
                aplIntegerRht = mpfr_get_ctsx (((LPAPLVFP) lpMemRht)++, fQuadCT, &bRet);

                break;

            defstop
                break;
        } // End SWITCH

        // Determine if the right arg value is in the left arg
        if (bRet
         && apaMinLft <= aplIntegerRht
         &&              aplIntegerRht <= apaMaxLft
         && (0 EQ apaMulLft
          || 0 EQ (aplIntegerRht - apaOffLft) % apaMulLft))
            // Save in the result
            *lpMemRes++ = bQuadIO + (aplIntegerRht - apaOffLft) / apaMulLft;
        else
            // Save in the result
            *lpMemRes++ = NotFound;
    } // End FOR

    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
    return bRet;
} // End PrimFnDydIotaAvN_EM


//***************************************************************************
//  $PrimFnDydIotaIvN_EM
//
//  Dyadic iota of APLINT/APLAPA vs. APLBOOL/APLINT/APLAPA/APLFLOAT/APLRAT/APLVFP
//***************************************************************************

UBOOL PrimFnDydIotaIvN_EM
    (LPAPLUINT lpMemRes,                // Ptr to result global memory data
     LPTOKEN   lptkLftArg,              // Ptr to left arg token
     APLSTYPE  aplTypeLft,              // Left arg storage type
     APLNELM   aplNELMLft,              // Left arg NELM
     APLRANK   aplRankLft,              // Left arg rank
     LPVOID    lpMemLft,                // Ptr to left arg global memory data
     LPTOKEN   lptkRhtArg,              // Ptr to right arg token
     APLSTYPE  aplTypeRht,              // Right arg storage type
     APLNELM   aplNELMRht,              // Right arg NELM
     APLRANK   aplRankRht,              // Right arg rank
     LPVOID    lpMemRht,                // Ptr to right arg global memory data
     UBOOL     bQuadIO,                 // []IO
     APLFLOAT  fQuadCT,                 // []CT
     APLUINT   NotFound,                // Not found value
     LPUBOOL   lpbCtrlBreak,            // Ptr to Ctrl-Break flag
     LPTOKEN   lptkFunc)                // Ptr to function token

{
    TOKEN        tkFunc = {0};          // Grade-up function token
    HGLOBAL      hGlbGupLft = NULL;     // Left arg grade-up global memory handle
    LPAPLINT     lpMemGupLft = NULL;    // Ptr to left arg grade-up global memory
#ifdef GRADE2ND
    HGLOBAL      hGlbGupRht = NULL;     // Right arg grade-up global memory handle
    LPAPLINT     lpMemGupRht = NULL;    // Ptr to right grade-up global memory
    APLUINT      uLastVal;              // Last value saved in the result
    APLINT       aplIntegerLst;         // Last right arg integer
#endif
    APLINT       iLft,                  // Loop counter
                 iRht,                  // ...
                 iCnt,                  // ...
                 iLow,                  // Lowest index of a match
                 iMin,                  // Minimum index
                 iMax,                  // Maximum ...
                 aplIntegerLft,         // Left arg integer
                 aplIntegerRht = 0,     // Right arg integer
                 aplIntegerZero = 0;    // A zero in case the right arg is a scalar
    LPPL_YYSTYPE lpYYRes;               // Ptr to grade-up result
    UBOOL        bRet = FALSE;          // TRUE iff the result is valid

    // Setup the grade-up function token
    tkFunc.tkFlags.TknType   = TKT_FCNIMMED;
    tkFunc.tkFlags.ImmType   = IMMTYPE_PRIMFCN;
////tkFunc.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
    tkFunc.tkData.tkChar     = UTF16_DELTASTILE;
    tkFunc.tkCharIndex       = lptkFunc->tkCharIndex;

    // Set the current index origin to zero for convenience
    SetQuadIO (0);

    // Grade-up the left arg
    lpYYRes =
      PrimFnMonGradeCommon_EM_YY (&tkFunc,      // Ptr to function token
                                   lptkLftArg,  // Ptr to right arg token
                                   NULL,        // Ptr to axis token (may be NULL)
                                   TRUE);       // TRUE iff we're to treat the right arg as ravelled
    // Restore the index origin
    SetQuadIO (bQuadIO);

    if (lpYYRes EQ NULL)
        goto ERROR_EXIT;
    // Get the grade-up global memory handle
    hGlbGupLft = lpYYRes->tkToken.tkData.tkGlbData;

    // Free the YYRes
    YYFree (lpYYRes); lpYYRes = NULL;

    // Lock the memory to get a ptr to it
    lpMemGupLft = MyGlobalLock (hGlbGupLft);

    // Skip over the header and dimensions to the data
    lpMemGupLft = VarArrayDataFmBase (lpMemGupLft);
#ifdef GRADE2ND
    // If the right arg is not a scalar, ...
    if (!IsScalar (aplRankRht))
    {
        // Set the current index origin to zero for convenience
        SetQuadIO (0);

        // Grade-up the right arg
        lpYYRes =
          PrimFnMonGradeCommon_EM_YY (&tkFunc,      // Ptr to function token
                                       lptkRhtArg,  // Ptr to right arg token
                                       NULL,        // Ptr to axis token (may be NULL)
                                       TRUE);       // TRUE iff we're to treat the right arg as ravelled
        // Restore the index origin
        SetQuadIO (bQuadIO);

        if (lpYYRes EQ NULL)
            goto ERROR_EXIT;
        // Get the grade-up global memory handle
        hGlbGupRht = lpYYRes->tkToken.tkData.tkGlbData;

        // Free the YYRes
        YYFree (lpYYRes); lpYYRes = NULL;

        // Lock the memory to get a ptr to it
        lpMemGupRht = MyGlobalLock (hGlbGupRht);

        // Skip over the header and dimensions to the data
        lpMemGupRht = VarArrayDataFmBase (lpMemGupRht);
    } else
        // Handle the scalar case
        lpMemGupRht = &aplIntegerZero;
#endif
    // Loop through the right arg values and look'em up
    //   in the left arg
    for (iRht = 0; iRht < (APLINT) aplNELMRht; iRht++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;
#ifdef GRADE2ND
        // Save the right arg value as the last one
        aplIntegerLst = aplIntegerRht;

        // Get the result index
        iRes = lpMemGupRht[iRht];

        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
                // Get the next integer from the right arg
                aplIntegerRht =
                  GetNextInteger (lpMemRht,                 // Ptr to global memory
                                  aplTypeRht,               // Storage type
                                  iRes);                    // Index
                break;

            case ARRAY_FLOAT:
                // Attempt to convert the float to an integer using []CT
                aplIntegerRht = FloatToAplint_CT (((LPAPLFLOAT) lpMemRht)[iRes], fQuadCT, &bRet);
                if (!bRet)
                    goto NOMATCH;
                break;

            case ARRAY_RAT:
                // Exact vs. exact doesn't use []CT
                // Attempt to convert the RAT to an APLINT
                aplIntegerRht = mpq_get_sx (&((LPAPLRAT) lpMemRht)[iRes], &bRet);
                if (!bRet)
                    goto NOMATCH;
                break;

            case ARRAY_VFP:
                // Attempt to convert the VFP to an APLINT using []CT
                aplIntegerRht = mpfr_get_ctsx (&((LPAPLVFP) lpMemRht)[iRht], fQuadCT, &bRet);
                if (!bRet)
                    goto NOMATCH;
                break;

            defstop
                break;
        } // End SWITCH

        // Check for duplicate value from the right arg
        if (iRht
         && aplIntegerRht EQ aplIntegerLst)
        {
            // Save in the result
            lpMemRes[iRes] = uLastVal;

            continue;
        } // End IF
#else
        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
                // Get the next integer from the right arg
                aplIntegerRht =
                  GetNextInteger (lpMemRht,                 // Ptr to global memory
                                  aplTypeRht,               // Storage type
                                  iRht);                    // Index
                break;

            case ARRAY_FLOAT:
                // Attempt to convert the float to an integer using System []CT
                aplIntegerRht = FloatToAplint_CT (((LPAPLFLOAT) lpMemRht)[iRht], fQuadCT, &bRet);
                if (!bRet)
                    goto NOMATCH;
                break;

            case ARRAY_RAT:
                // Exact vs. exact doesn't use []CT
                // Attempt to convert the RAT to an APLINT
                aplIntegerRht = mpq_get_sx (&((LPAPLRAT) lpMemRht)[iRht], &bRet);
                if (!bRet)
                    goto NOMATCH;
                break;

            case ARRAY_VFP:
                // Attempt to convert the VFP to an APLINT using []CT
                aplIntegerRht = mpfr_get_ctsx (&((LPAPLVFP) lpMemRht)[iRht], fQuadCT, &bRet);
                if (!bRet)
                    goto NOMATCH;
                break;

            defstop
                break;
        } // End SWITCH
#endif
        // Initialize the left arg minimum and maximum indices
        iMin = 0;
        iMax = aplNELMLft - 1;

        // Lookup this value in the left arg (binary search)
        while (iMin <= iMax)
        {
            // Set the current index
            iLft = (iMin + iMax) / 2;

            // Get the next integer from the left arg
            aplIntegerLft =
              GetNextInteger (lpMemLft,             // Ptr to global memory
                              aplTypeLft,           // Storage type
                              lpMemGupLft[iLft]);   // Index
            // Check for a match
            if (aplIntegerLft > aplIntegerRht)
                iMax = iLft - 1;
            else
            if (aplIntegerLft < aplIntegerRht)
                iMin = iLft + 1;
            else
            {
                // Save the index of the match
                iLow = lpMemGupLft[iLft];

                // We found a match -- check earlier indices for a match
                //   so we always return the lowest index match.
                for (iCnt = iLft - 1; iCnt >= 0; iCnt--)
                {
                    // Get the next integer from the left arg
                    aplIntegerLft =
                      GetNextInteger (lpMemLft,             // Ptr to global memory
                                      aplTypeLft,           // Storage type
                                      lpMemGupLft[iCnt]);   // Index
                    if (aplIntegerLft NE aplIntegerRht)
                        break;
                    else
                        iLow = min (iLow, lpMemGupLft[iCnt]);
                } // End FOR
#ifdef GRADE2ND
                // Save in the result
                uLastVal       =
                lpMemRes[iRes] = bQuadIO + iLow;
#else
                // Save in the result
                lpMemRes[iRht] = bQuadIO + iLow;
#endif
                break;
            } // End IF/ELSE/...
        } // End WHILE

        // If we didn't find a match, ...
        if (iMin > iMax)
NOMATCH:
#ifdef GRADE2ND
            // Save in the result
            uLastVal       =
            lpMemRes[iRes] = NotFound;
#else
            // Save in the result
            lpMemRes[iRht] = NotFound;
#endif
    } // End FOR

    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
    if (hGlbGupLft && lpMemGupLft)
    {
        if (lpMemGupLft)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbGupLft); lpMemGupLft = NULL;
        } // End IF

        // We no longer need this resource
        FreeResultGlobalVar (hGlbGupLft); hGlbGupLft = NULL;
    } // End IF
#ifdef GRADE2ND
    if (hGlbGupRht && lpMemGupRht)
    {
        if (lpMemGupRht)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbGupRht); lpMemGupRht = NULL;
        } // End IF

        // We no longer need this resource
        FreeResultGlobalVar (hGlbGupRht); hGlbGupRht = NULL;
    } // End IF
#endif
    return bRet;
} // End PrimFnDydIotaIvN_EM


//***************************************************************************
//  $PrimFnDydIotaFvN_EM
//
//  Dyadic iota of APLFLOAT vs. APLBOOL/APLINT/APLAPA/APLFLOAT/APLRAT/APLVFP and
//***************************************************************************

UBOOL PrimFnDydIotaFvN_EM
    (LPAPLUINT lpMemRes,                // Ptr to result global memory data
     LPTOKEN   lptkLftArg,              // Ptr to left arg token
     APLSTYPE  aplTypeLft,              // Left arg storage type
     APLNELM   aplNELMLft,              // Left arg NELM
     LPVOID    lpMemLft,                // Ptr to left arg global memory data
     LPTOKEN   lptkRhtArg,              // Ptr to right arg token
     APLSTYPE  aplTypeRht,              // Right arg storage type
     APLNELM   aplNELMRht,              // Right arg NELM
     APLRANK   aplRankRht,              // Right arg rank
     LPVOID    lpMemRht,                // Ptr to right arg global memory data
     UBOOL     bQuadIO,                 // []IO
     APLFLOAT  fQuadCT,                 // []CT
     APLUINT   NotFound,                // Not found value
     LPUBOOL   lpbCtrlBreak,            // Ptr to Ctrl-Break flag
     LPTOKEN   lptkFunc)                // Ptr to function token

{
    TOKEN        tkFunc = {0};          // Grade-up function token
    HGLOBAL      hGlbGupLft = NULL;     // Left arg grade-up global memory handle
    LPAPLINT     lpMemGupLft = NULL;    // Ptr to left arg grade-up global memory
#ifdef GRADE2ND
    HGLOBAL      hGlbGupRht = NULL;     // Right arg grade-up global memory handle
    LPAPLINT     lpMemGupRht = NULL;    // Ptr to right arg grade-up global memory
    APLUINT      uLastVal;              // Last value saved in the result
    APLFLOAT     aplFloatLst;           // Last right arg float
#endif
    APLINT       iLft,                  // Loop counter
                 iRht,                  // ...
                 iCnt,                  // ...
                 iLow,                  // Lowest index of a match
                 iMin,                  // Minimum index
                 iMax;                  // Maximum ...
    APLFLOAT     aplFloatLft,           // Left arg float
                 aplFloatRht = 0;       // Right arg float
    APLINT       aplIntegerZero = 0;    // A zero in case the right arg is a scalar
    LPPL_YYSTYPE lpYYRes;               // Ptr to grade-up result
    UBOOL        bRet = FALSE,          // TRUE iff the result is valid
                 bComp;                 // TRUE iff the left and right floats are equal within []CT

    // Setup the grade-up function token
    tkFunc.tkFlags.TknType   = TKT_FCNIMMED;
    tkFunc.tkFlags.ImmType   = IMMTYPE_PRIMFCN;
////tkFunc.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
    tkFunc.tkData.tkChar     = UTF16_DELTASTILE;
    tkFunc.tkCharIndex       = lptkFunc->tkCharIndex;

    // Set the current index origin to zero for convenience
    SetQuadIO (0);

    // Grade-up the left arg
    lpYYRes =
      PrimFnMonGradeCommon_EM_YY (&tkFunc,      // Ptr to function token
                                   lptkLftArg,  // Ptr to right arg token
                                   NULL,        // Ptr to axis token (may be NULL)
                                   TRUE);       // TRUE iff we're to treat the right arg as ravelled
    // Restore the index origin
    SetQuadIO (bQuadIO);

    if (lpYYRes EQ NULL)
        goto ERROR_EXIT;
    // Get the grade-up global memory handle
    hGlbGupLft = lpYYRes->tkToken.tkData.tkGlbData;

    // Free the YYRes
    YYFree (lpYYRes); lpYYRes = NULL;

    // Lock the memory to get a ptr to it
    lpMemGupLft = MyGlobalLock (hGlbGupLft);

    // Skip over the header and dimensions to the data
    lpMemGupLft = VarArrayDataFmBase (lpMemGupLft);
#ifdef GRADE2ND
    // If the right arg is not a scalar, ...
    if (!IsScalar (aplRankRht))
    {
        // Set the current index origin to zero for convenience
        SetQuadIO (0);

        // Grade-up the right arg
        lpYYRes =
          PrimFnMonGradeCommon_EM_YY (&tkFunc,      // Ptr to function token
                                       lptkRhtArg,  // Ptr to right arg token
                                       NULL,        // Ptr to axis token (may be NULL)
                                       TRUE);       // TRUE iff we're to treat the right arg as ravelled
        // Restore the index origin
        SetQuadIO (bQuadIO);

        if (lpYYRes EQ NULL)
            goto ERROR_EXIT;
        // Get the grade-up global memory handle
        hGlbGupRht = lpYYRes->tkToken.tkData.tkGlbData;

        // Free the YYRes
        YYFree (lpYYRes); lpYYRes = NULL;

        // Lock the memory to get a ptr to it
        lpMemGupRht = MyGlobalLock (hGlbGupRht);

        // Skip over the header and dimensions to the data
        lpMemGupRht = VarArrayDataFmBase (lpMemGupRht);
    } else
        // Handle the scalar case
        lpMemGupRht = &aplIntegerZero;
#endif
    // Loop through the right arg values and look'em up
    //   in the left arg
    for (iRht = 0; iRht < (APLINT) aplNELMRht; iRht++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;
#ifdef GRADE2ND
        // Save the right arg value as the last one
        aplFloatLst = aplFloatRht;

        // Get the result index
        iRes = lpMemGupRht[iRht];

        // Get the next float from the right arg
        aplFloatRht =
          GetNextFloat (lpMemRht,                   // Ptr to global memory
                        aplTypeRht,                 // Storage type
                        iRes);                      // Index
        // Check for duplicate value from the right arg
        if (iRht
         && aplFloatRht EQ aplFloatLst)
        {
            // Save in the result
            lpMemRes[iRes] = uLastVal;

            continue;
        } // End IF
#else
        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
            case ARRAY_FLOAT:
                // Get the next float from the right arg
                aplFloatRht =
                  GetNextFloat (lpMemRht,                   // Ptr to global memory
                                aplTypeRht,                 // Storage type
                                iRht);                      // Index
                break;

            case ARRAY_RAT:
                // Convert the RAT to an APLFLOAT
                aplFloatRht = mpq_get_d (&((LPAPLRAT) lpMemRht)[iRht]);

                break;

            case ARRAY_VFP:
                // Convert the VFP to an APLFLOAT
                aplFloatRht = mpfr_get_d (&((LPAPLVFP) lpMemRht)[iRht], MPFR_RNDN);

                break;

            defstop
                break;
        } // End SWITCH
#endif
        // Initialize the left arg minimum and maximum indices
        iMin = 0;
        iMax = aplNELMLft - 1;

        // Lookup this value in the left arg (binary search)
        while (iMin <= iMax)
        {
            // Set the current index
            iLft = (iMin + iMax) / 2;

            // Get the next float from the left arg
            aplFloatLft =
              GetNextFloat (lpMemLft,               // Ptr to global memory
                            aplTypeLft,             // Storage type
                            lpMemGupLft[iLft]);     // Index
            // Compare 'em
            bComp = CompareCT (aplFloatLft, aplFloatRht, fQuadCT, NULL);

            // Check for a match
            if (bComp)
            {
                // Save the index of the match
                iLow = lpMemGupLft[iLft];

                // We found a match -- check earlier indices for a match
                //   so we always return the lowest index match.
                for (iCnt = iLft - 1; iCnt >= 0; iCnt--)
                {
                    // Get the next float from the left arg
                    aplFloatLft =
                      GetNextFloat (lpMemLft,           // Ptr to global memory
                                    aplTypeLft,         // Storage type
                                    lpMemGupLft[iCnt]); // Index
                    if (!CompareCT (aplFloatLft, aplFloatRht, fQuadCT, NULL))
                        break;
                    else
                        iLow = min (iLow, lpMemGupLft[iCnt]);
                } // End FOR

                // Check for later but smaller indices for a match
                //   so we always return the lowest index match.
                for (iCnt = iLft + 1; iCnt < (APLINT) aplNELMLft; iCnt++)
                {
                    // Get the next float from the left arg
                    aplFloatLft =
                      GetNextFloat (lpMemLft,           // Ptr to global memory
                                    aplTypeLft,         // Storage type
                                    lpMemGupLft[iCnt]); // Index
                    if (!CompareCT (aplFloatLft, aplFloatRht, fQuadCT, NULL))
                        break;
                    else
                        iLow = min (iLow, lpMemGupLft[iCnt]);
                } // End FOR
#ifdef GRADE2ND
                // Save in the result
                uLastVal       =
                lpMemRes[iRes] = bQuadIO + iLow;
#else
                // Save in the result
                lpMemRes[iRht] = bQuadIO + iLow;
#endif
                break;
            } else
            if (aplFloatLft > aplFloatRht)
                iMax = iLft - 1;
            else
                iMin = iLft + 1;
        } // End WHILE

        // If we didn't find a match, ...
        if (iMin > iMax)
#ifdef GRADE2ND
            // Save in the result
            uLastVal       =
            lpMemRes[iRes] = NotFound;
#else
            // Save in the result
            lpMemRes[iRht] = NotFound;
#endif
    } // End FOR

    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
    if (hGlbGupLft && lpMemGupLft)
    {
        if (lpMemGupLft)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbGupLft); lpMemGupLft = NULL;
        } // End IF

        // We no longer need this resource
        FreeResultGlobalVar (hGlbGupLft); hGlbGupLft = NULL;
    } // End IF
#ifdef GRADE2ND
    if (hGlbGupRht && lpMemGupRht)
    {
        if (lpMemGupRht)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbGupRht); lpMemGupRht = NULL;
        } // End IF

        // We no longer need this resource
        FreeResultGlobalVar (hGlbGupRht); hGlbGupRht = NULL;
    } // End IF
#endif
    return bRet;
} // End PrimFnDydIotaFvN_EM


//***************************************************************************
//  $PrimFnDydIotaPvN_EM
//
//  Dyadic iota of PV vs. APLBOOL/APLINT/APLAPA/APLFLOAT/APLRAT/APLVFP
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydIotaPvN_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydIotaPvN_EM
    (LPAPLUINT         lpMemRes,        // Ptr to result global memory data
     LPVARARRAY_HEADER lpHeaderLft,     // Ptr to left arg header
     APLSTYPE          aplTypeLft,      // Left arg storage type
     APLNELM           aplNELMLft,      // Left arg NELM
     LPAPLUINT         lpMemLft,        // Ptr to left arg global memory data
     APLSTYPE          aplTypeRht,      // Right arg storage type
     APLNELM           aplNELMRht,      // Right arg NELM
     LPVOID            lpMemRht,        // Ptr to right arg global memory data
     UBOOL             bQuadIO,         // []IO
     APLFLOAT          fQuadCT,         // []CT
     APLUINT           NotFound,        // Not found value
     LPUBOOL           lpbCtrlBreak,    // Ptr to Ctrl-Break flag
     LPTOKEN           lptkFunc)        // Ptr to function token

{
    APLUINT   uLft,                     // Loop counter
              uRht;                     // ...
    APLINT    aplIntegerLft,            // Left arg integer
              aplIntegerRht,            // Right arg integer
              ByteRes;                  // # bytes in the result
    APLFLOAT  aplFloatRht;              // Right arg float
    HGLOBAL   hGlbInv = NULL;           // Inverse indices global memory handle
    LPAPLUINT lpMemInv = NULL;          // Ptr to inverse indices global memory
    UBOOL     bRet = FALSE;             // TRUE iff the result is valid

    // Calculate space needed for the result
    ByteRes = aplNELMLft * sizeof (APLUINT);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate a temporary array to hold the inverse indices
    hGlbInv = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbInv)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemInv = MyGlobalLock (hGlbInv);

    // Loop through the left arg converting it to
    //   origin-0 inverse indices
    for (uLft = 0; uLft < aplNELMLft; uLft++)
    {
        // Get the next integer
        aplIntegerLft =
          GetNextInteger (lpMemLft,
                          aplTypeLft,
                          uLft);
        // Save as origin-0 inverse index
        lpMemInv[aplIntegerLft - lpHeaderLft->PV1] = uLft + bQuadIO;
    } // End FOR

    // Loop through the right arg looking up each value
    //   in the inverse indices and saving the answer
    //   in the result
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
                // Get the next integer
                aplIntegerRht =
                  GetNextInteger (lpMemRht,
                                  aplTypeRht,
                                  uRht);
                // Convert to same origin as the inverse indices
                aplIntegerRht = _isub64 (aplIntegerRht,  lpHeaderLft->PV1, &bRet);

                break;

            case ARRAY_FLOAT:
                // Get the next float
                aplFloatRht = *((LPAPLFLOAT) lpMemRht)++;

                // Attempt to convert the float to an integer using []CT
                aplIntegerRht = FloatToAplint_CT (aplFloatRht, fQuadCT, &bRet);

                // If it succeeded, ...
                if (bRet)
                    // Convert to same origin as the inverse indices
                    aplIntegerRht = _isub64 (aplIntegerRht,  lpHeaderLft->PV1, &bRet);
                break;

            case ARRAY_RAT:
                // Exact vs. exact doesn't use []CT
                // Attempt to convert the RAT to an APLINT
                aplIntegerRht = mpq_get_sx (((LPAPLRAT) lpMemRht)++, &bRet);

                // If it succeeded, ...
                if (bRet)
                    // Convert to same origin as the inverse indices
                    aplIntegerRht = _isub64 (aplIntegerRht,  lpHeaderLft->PV1, &bRet);
                break;

            case ARRAY_VFP:
                // Attempt to convert the VFP to an APLINT using []CT
                aplIntegerRht = mpfr_get_ctsx (((LPAPLVFP) lpMemRht)++, fQuadCT, &bRet);

                // If it succeeded, ...
                if (bRet)
                    // Convert to same origin as the inverse indices
                    aplIntegerRht = _isub64 (aplIntegerRht,  lpHeaderLft->PV1, &bRet);
                break;

            defstop
                break;
        } // End SWITCH

        // If there's no overflow, and it's within range, ...
        if (bRet
         && (0 <= aplIntegerRht
          &&      aplIntegerRht <= ((APLINT) aplNELMLft - 1)))
            // Lookup in the inverse indices
            *lpMemRes++ = lpMemInv[aplIntegerRht];
        else
            // It's out of range
            *lpMemRes++ = NotFound;
    } // End FOR

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (hGlbInv)
    {
        if (lpMemInv)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbInv); lpMemInv = NULL;
        } // End IF

        // We no longer need this resource
        DbgGlobalFree (hGlbInv); hGlbInv = NULL;
    } // End IF

    return bRet;
} // End PrimFnDydIotaPvN_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydIotaCvC_EM
//
//  Dyadic iota of APLCHAR vs. APLCHAR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydIotaCvC_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydIotaCvC_EM
    (LPAPLUINT lpMemRes,            // Ptr to result global memory data
     APLNELM   aplNELMLft,          // Left arg (and result) NELM
     LPAPLCHAR lpMemLft,            // Ptr to left arg global memory data
     APLNELM   aplNELMRht,          // Right arg NELM
     LPAPLCHAR lpMemRht,            // Ptr to right arg global memory data
     UBOOL     bQuadIO,             // []IO
     APLUINT   NotFound,            // Not found value
     LPUBOOL   lpbCtrlBreak,        // Ptr to Ctrl-Break flag
     LPTOKEN   lptkFunc)            // Ptr to function token

{
    LPAPLUINT lpMemTT = NULL;       // Translate table global memory handle
    UINT      ByteTT;               // # bytes in the Translate Table
    APLUINT   uRht;                 // Loop counter
    APLINT    iLft;                 // ...
    UBOOL     bRet = FALSE;         // TRUE iff the result is valid

    // Calculate # bytes in the TT at one APLUINT per 16-bit index (APLCHAR)
    ByteTT = APLCHAR_SIZE * sizeof (APLUINT);

    // Allocate space for a ByteTT Translate Table
    // Note that this allocation is GMEM_FIXED
    //   because we'll use it quickly and then free it.
    lpMemTT = MyGlobalAlloc (GMEM_FIXED, ByteTT);
    if (!lpMemTT)
        goto WSFULL_EXIT;

    // Trundle through the TT setting each value to NotFound
    for (uRht = 0; uRht < APLCHAR_SIZE; uRht++)
        lpMemTT[uRht] = NotFound;

    // Trundle through the left arg backwards marking the TT
    for (iLft = aplNELMLft - 1, lpMemLft += iLft;
         iLft >= 0;
         iLft--)
        lpMemTT[*lpMemLft--] = iLft + bQuadIO;

    // Trundle through the right arg looking for the chars
    //   in the TT and setting the result value
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Save the index from the TT in the result
        *lpMemRes++ = lpMemTT[*lpMemRht++];
    } // End FOR

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (lpMemTT)
    {
        // We no longer need this storage
        MyGlobalFree (lpMemTT); lpMemTT = NULL;
    } // End IF

    return bRet;
} // End PrimFnDydIotaCvC_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydIotaRvN_EM
//
//  Dyadic iota of APLRAT vs. APLBOOL/APLINT/APLAPA/APLFLOAT/APLRAT/APLVFP
//***************************************************************************

UBOOL PrimFnDydIotaRvN_EM
    (LPAPLUINT lpMemRes,                // Ptr to result global memory data
     LPTOKEN   lptkLftArg,              // Ptr to left arg token
     APLSTYPE  aplTypeLft,              // Left arg storage type
     APLNELM   aplNELMLft,              // Left arg NELM
     LPAPLRAT  lpMemLft,                // Ptr to left arg global memory data
     LPTOKEN   lptkRhtArg,              // Ptr to right arg token
     APLSTYPE  aplTypeRht,              // Right arg storage type
     APLNELM   aplNELMRht,              // Right arg NELM
     APLRANK   aplRankRht,              // Right arg rank
     LPVOID    lpMemRht,                // Ptr to right arg global memory data
     UBOOL     bQuadIO,                 // []IO
     APLFLOAT  fQuadCT,                 // []CT
     APLUINT   NotFound,                // Not found value
     LPUBOOL   lpbCtrlBreak,            // Ptr to Ctrl-Break flag
     LPTOKEN   lptkFunc)                // Ptr to function token

{
    TOKEN        tkFunc = {0};          // Grade-up function token
    HGLOBAL      hGlbGupLft = NULL;     // Left arg grade-up global memory handle
    LPAPLINT     lpMemGupLft = NULL;    // Ptr to left arg grade-up global memory
#ifdef GRADE2ND
    HGLOBAL      hGlbGupRht = NULL;     // Right arg grade-up global memory handle
    LPAPLINT     lpMemGupRht = NULL;    // Ptr to right grade-up global memory
    APLUINT      uLastVal;              // Last value saved in the result
    APLINT       aplIntegerLst,         // Last right arg integer
                 aplIntegerZero = 0;    // A zero in case the right arg is a scalar
#endif
    APLINT       iLft,                  // Loop counter
                 iRht,                  // ...
                 iCnt,                  // ...
                 iLow,                  // Lowest index of a match
                 iMin,                  // Minimum index
                 iMax;                  // Maximum ...
    LPPL_YYSTYPE lpYYRes;               // Ptr to grade-up result
    UBOOL        bRet = FALSE,          // TRUE iff the result is valid
                 bUseCT;                // TRUE iff we need to use []CT when comparing
    APLRAT       aplRatLft = {0},       // Left arg item as rational
                 aplRatRht = {0};       // Right ...

    // Initialize the temps
    mpq_init (&aplRatLft);
    mpq_init (&aplRatRht);

    // Setup the grade-up function token
    tkFunc.tkFlags.TknType   = TKT_FCNIMMED;
    tkFunc.tkFlags.ImmType   = IMMTYPE_PRIMFCN;
////tkFunc.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
    tkFunc.tkData.tkChar     = UTF16_DELTASTILE;
    tkFunc.tkCharIndex       = lptkFunc->tkCharIndex;

    // Set the current index origin to zero for convenience
    SetQuadIO (0);

    // Grade-up the left arg
    lpYYRes =
      PrimFnMonGradeCommon_EM_YY (&tkFunc,      // Ptr to function token
                                   lptkLftArg,  // Ptr to right arg token
                                   NULL,        // Ptr to axis token (may be NULL)
                                   TRUE);       // TRUE iff we're to treat the right arg as ravelled
    // Restore the index origin
    SetQuadIO (bQuadIO);

    if (lpYYRes EQ NULL)
        goto ERROR_EXIT;
    // Get the grade-up global memory handle
    hGlbGupLft = lpYYRes->tkToken.tkData.tkGlbData;

    // Free the YYRes
    YYFree (lpYYRes); lpYYRes = NULL;

    // Lock the memory to get a ptr to it
    lpMemGupLft = MyGlobalLock (hGlbGupLft);

    // Skip over the header and dimensions to the data
    lpMemGupLft = VarArrayDataFmBase (lpMemGupLft);
#ifdef GRADE2ND
    // If the right arg is not a scalar, ...
    if (!IsScalar (aplRankRht))
    {
        // Set the current index origin to zero for convenience
        SetQuadIO (0);

        // Grade-up the right arg
        lpYYRes =
          PrimFnMonGradeCommon_EM_YY (&tkFunc,      // Ptr to function token
                                       lptkRhtArg,  // Ptr to right arg token
                                       NULL,        // Ptr to axis token (may be NULL)
                                       TRUE);       // TRUE iff we're to treat the right arg as ravelled
        // Restore the index origin
        SetQuadIO (bQuadIO);

        if (lpYYRes EQ NULL)
            goto ERROR_EXIT;
        // Get the grade-up global memory handle
        hGlbGupRht = lpYYRes->tkToken.tkData.tkGlbData;

        // Free the YYRes
        YYFree (lpYYRes); lpYYRes = NULL;

        // Lock the memory to get a ptr to it
        lpMemGupRht = MyGlobalLock (hGlbGupRht);

        // Skip over the header and dimensions to the data
        lpMemGupRht = VarArrayDataFmBase (lpMemGupRht);
    } else
        // Handle the scalar case
        lpMemGupRht = &aplIntegerZero;
#endif
    // Loop through the right arg values and look'em up
    //   in the left arg
    for (iRht = 0; iRht < (APLINT) aplNELMRht; iRht++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;
#ifdef GRADE2ND
        // Save the right arg value as the last one
        aplIntegerLst = aplIntegerRht;

        // Get the result index
        iRes = lpMemGupRht[iRht];

        // Get the next integer from the right arg
        aplIntegerRht =
          GetNextInteger (lpMemRht,                 // Ptr to global memory
                          aplTypeRht,               // Storage type
                          iRes);                    // Index
        // Check for duplicate value from the right arg
        if (iRht
         && aplIntegerRht EQ aplIntegerLst)
        {
            // Save in the result
            lpMemRes[iRes] = uLastVal;

            continue;
        } // End IF
#else
        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
                // Copy the INT to a RAT
                mpq_set_sx (&aplRatRht,
                             GetNextInteger (lpMemRht,      // Ptr to global memory
                                             aplTypeRht,    // Storage type
                                             iRht),         // Index
                             1);
                break;

            case ARRAY_FLOAT:
                // Copy the FLOAT to a RAT
                mpq_set_d  (&aplRatRht, ((LPAPLFLOAT) lpMemRht)[iRht]);

                break;

            case ARRAY_RAT:
                // Copy the RAT
                mpq_set    (&aplRatRht, &((LPAPLRAT) lpMemRht)[iRht]);

                break;

            case ARRAY_VFP:
                // Copy the VFP to a RAT
                mpq_set_fr (&aplRatRht, &((LPAPLVFP) lpMemRht)[iRht]);

                break;

            defstop
                break;
        } // End SWITCH
#endif
        // Initialize the left arg minimum and maximum indices
        iMin = 0;
        iMax = aplNELMLft - 1;

        // Lookup this value in the left arg (binary search)
        while (iMin <= iMax)
        {
            // Set the current index
            iLft = (iMin + iMax) / 2;

            // Get the next rational from the left arg
            mpq_set (&aplRatLft, &lpMemLft[lpMemGupLft[iLft]]);

            // Should we use []CT when comparing?
            bUseCT = IsVfp (aplTypeRht) || IsSimpleFlt (aplTypeRht);

            // Split cases based upon the signum of the comparison
            switch (signum (bUseCT ? mpq_cmp_ct ( aplRatLft,  aplRatRht, fQuadCT)
                                   : mpq_cmp    (&aplRatLft, &aplRatRht         )))
            {
                case  1:
                    iMax = iLft - 1;

                    continue;

                case  0:
                    break;

                case -1:
                    iMin = iLft + 1;

                    continue;

                defstop
                    break;
            } // End SWITCH

            // Save the index of the match
            iLow = lpMemGupLft[iLft];

            // We found a match -- check earlier indices for a match
            //   so we always return the lowest index match.
            for (iCnt = iLft - 1; iCnt >= 0; iCnt--)
            {
                // Get the next rational from the left arg
                mpq_set (&aplRatLft, &lpMemLft[lpMemGupLft[iCnt]]);

                // If we should use []CT when comparing, ...
                if ((bUseCT ? mpq_cmp_ct ( aplRatLft,  aplRatRht, fQuadCT)
                            : mpq_cmp    (&aplRatLft, &aplRatRht         )) NE 0)
                    break;
                else
                    iLow = min (iLow, lpMemGupLft[iCnt]);
            } // End FOR

            // If we should use []CT when comparing, ...
            if (bUseCT)
            // Check for later but smaller indices for a match
            //   so we always return the lowest index match.
            for (iCnt = iLft + 1; iCnt < (APLINT) aplNELMLft; iCnt++)
            {
                // Get the next rational from the left arg
                mpq_set (&aplRatLft, &lpMemLft[lpMemGupLft[iCnt]]);

                if (mpq_cmp_ct (aplRatLft, aplRatRht, fQuadCT) NE 0)
                    break;
                else
                    iLow = min (iLow, lpMemGupLft[iCnt]);
            } // End IF/FOR
#ifdef GRADE2ND
            // Save in the result
            uLastVal       =
            lpMemRes[iRes] = bQuadIO + iLow;
#else
            // Save in the result
            lpMemRes[iRht] = bQuadIO + iLow;
#endif
            break;
        } // End WHILE

        // If we didn't find a match, ...
        if (iMin > iMax)
#ifdef GRADE2ND
            // Save in the result
            uLastVal       =
            lpMemRes[iRes] = NotFound;
#else
            // Save in the result
            lpMemRes[iRht] = NotFound;
#endif
    } // End FOR

    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
    if (hGlbGupLft && lpMemGupLft)
    {
        if (lpMemGupLft)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbGupLft); lpMemGupLft = NULL;
        } // End IF

        // We no longer need this resource
        FreeResultGlobalVar (hGlbGupLft); hGlbGupLft = NULL;
    } // End IF
#ifdef GRADE2ND
    if (hGlbGupRht && lpMemGupRht)
    {
        if (lpMemGupRht)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbGupRht); lpMemGupRht = NULL;
        } // End IF

        // We no longer need this resource
        FreeResultGlobalVar (hGlbGupRht); hGlbGupRht = NULL;
    } // End IF
#endif
    // We no longer need this storage
    Myq_clear (&aplRatRht);
    Myq_clear (&aplRatLft);

    return bRet;
} // End PrimFnDydIotaRvN_EM


//***************************************************************************
//  $PrimFnDydIotaVvN_EM
//
//  Dyadic iota of APLVFP vs. APLBOOL/APLINT/APLAPA/APLFLOAT/APLRAT/APLVFP
//***************************************************************************

UBOOL PrimFnDydIotaVvN_EM
    (LPAPLUINT lpMemRes,                // Ptr to result global memory data
     LPTOKEN   lptkLftArg,              // Ptr to left arg token
     APLSTYPE  aplTypeLft,              // Left arg storage type
     APLNELM   aplNELMLft,              // Left arg NELM
     LPAPLVFP  lpMemLft,                // Ptr to left arg global memory data
     LPTOKEN   lptkRhtArg,              // Ptr to right arg token
     APLSTYPE  aplTypeRht,              // Right arg storage type
     APLNELM   aplNELMRht,              // Right arg NELM
     APLRANK   aplRankRht,              // Right arg rank
     LPVOID    lpMemRht,                // Ptr to right arg global memory data
     UBOOL     bQuadIO,                 // []IO
     APLFLOAT  fQuadCT,                 // []CT
     APLUINT   NotFound,                // Not found value
     LPUBOOL   lpbCtrlBreak,            // Ptr to Ctrl-Break flag
     LPTOKEN   lptkFunc)                // Ptr to function token

{
    TOKEN        tkFunc = {0};          // Grade-up function token
    HGLOBAL      hGlbGupLft = NULL;     // Left arg grade-up global memory handle
    LPAPLINT     lpMemGupLft = NULL;    // Ptr to left arg grade-up global memory
#ifdef GRADE2ND
    HGLOBAL      hGlbGupRht = NULL;     // Right arg grade-up global memory handle
    LPAPLINT     lpMemGupRht = NULL;    // Ptr to right grade-up global memory
    APLUINT      uLastVal;              // Last value saved in the result
    APLINT       aplIntegerLst,         // Last right arg integer
                 aplIntegerZero = 0;    // A zero in case the right arg is a scalar
#endif
    APLINT       iLft,                  // Loop counter
                 iRht,                  // ...
                 iCnt,                  // ...
                 iLow,                  // Lowest index of a match
                 iMin,                  // Minimum index
                 iMax;                  // Maximum ...
    LPPL_YYSTYPE lpYYRes;               // Ptr to grade-up result
    UBOOL        bRet = FALSE;          // TRUE iff the result is valid
    APLVFP       aplVfpLft = {0},       // Left arg item as VFP
                 aplVfpRht = {0};       // Right ...

    // Initialize the temps
    mpfr_init0 (&aplVfpLft);
    mpfr_init0 (&aplVfpRht);

    // Setup the grade-up function token
    tkFunc.tkFlags.TknType   = TKT_FCNIMMED;
    tkFunc.tkFlags.ImmType   = IMMTYPE_PRIMFCN;
////tkFunc.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
    tkFunc.tkData.tkChar     = UTF16_DELTASTILE;
    tkFunc.tkCharIndex       = lptkFunc->tkCharIndex;

    // Set the current index origin to zero for convenience
    SetQuadIO (0);

    // Grade-up the left arg
    lpYYRes =
      PrimFnMonGradeCommon_EM_YY (&tkFunc,      // Ptr to function token
                                   lptkLftArg,  // Ptr to right arg token
                                   NULL,        // Ptr to axis token (may be NULL)
                                   TRUE);       // TRUE iff we're to treat the right arg as ravelled
    // Restore the index origin
    SetQuadIO (bQuadIO);

    if (lpYYRes EQ NULL)
        goto ERROR_EXIT;
    // Get the grade-up global memory handle
    hGlbGupLft = lpYYRes->tkToken.tkData.tkGlbData;

    // Free the YYRes
    YYFree (lpYYRes); lpYYRes = NULL;

    // Lock the memory to get a ptr to it
    lpMemGupLft = MyGlobalLock (hGlbGupLft);

    // Skip over the header and dimensions to the data
    lpMemGupLft = VarArrayDataFmBase (lpMemGupLft);
#ifdef GRADE2ND
    // If the right arg is not a scalar, ...
    if (!IsScalar (aplRankRht))
    {
        // Set the current index origin to zero for convenience
        SetQuadIO (0);

        // Grade-up the right arg
        lpYYRes =
          PrimFnMonGradeCommon_EM_YY (&tkFunc,      // Ptr to function token
                                       lptkRhtArg,  // Ptr to right arg token
                                       NULL,        // Ptr to axis token (may be NULL)
                                       TRUE);       // TRUE iff we're to treat the right arg as ravelled
        // Restore the index origin
        SetQuadIO (bQuadIO);

        if (lpYYRes EQ NULL)
            goto ERROR_EXIT;
        // Get the grade-up global memory handle
        hGlbGupRht = lpYYRes->tkToken.tkData.tkGlbData;

        // Free the YYRes
        YYFree (lpYYRes); lpYYRes = NULL;

        // Lock the memory to get a ptr to it
        lpMemGupRht = MyGlobalLock (hGlbGupRht);

        // Skip over the header and dimensions to the data
        lpMemGupRht = VarArrayDataFmBase (lpMemGupRht);
    } else
        // Handle the scalar case
        lpMemGupRht = &aplIntegerZero;
#endif
    // Loop through the right arg values and look'em up
    //   in the left arg
    for (iRht = 0; iRht < (APLINT) aplNELMRht; iRht++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;
#ifdef GRADE2ND
        // Save the right arg value as the last one
        aplIntegerLst = aplIntegerRht;

        // Get the result index
        iRes = lpMemGupRht[iRht];

        // Get the next integer from the right arg
        aplIntegerRht =
          GetNextInteger (lpMemRht,                 // Ptr to global memory
                          aplTypeRht,               // Storage type
                          iRes);                    // Index
        // Check for duplicate value from the right arg
        if (iRht
         && aplIntegerRht EQ aplIntegerLst)
        {
            // Save in the result
            lpMemRes[iRes] = uLastVal;

            continue;
        } // End IF
#else
        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
                // Copy the INT to a VFP
                mpfr_set_sx (&aplVfpRht,
                              GetNextInteger (lpMemRht,     // Ptr to global memory
                                              aplTypeRht,   // Storage type
                                              iRht),        // Index
                              MPFR_RNDN);                   // Rounding mode
                break;

            case ARRAY_FLOAT:
                // Copy the FLOAT to a VFP
                mpfr_set_d  (&aplVfpRht, ((LPAPLFLOAT) lpMemRht)[iRht], MPFR_RNDN);

                break;

            case ARRAY_RAT:
                // Copy the RAT to a VFP
                mpfr_set_q (&aplVfpRht, &((LPAPLRAT) lpMemRht)[iRht], MPFR_RNDN);

                break;

            case ARRAY_VFP:
                // Copy the VFP
                mpfr_copy  (&aplVfpRht, &((LPAPLVFP) lpMemRht)[iRht]);

                break;

            defstop
                break;
        } // End SWITCH
#endif
        // Initialize the left arg minimum and maximum indices
        iMin = 0;
        iMax = aplNELMLft - 1;

        // Lookup this value in the left arg (binary search)
        while (iMin <= iMax)
        {
            // Set the current index
            iLft = (iMin + iMax) / 2;

            // Get the next VFP from the left arg
            mpfr_copy (&aplVfpLft, &lpMemLft[lpMemGupLft[iLft]]);

            // Split cases based upon the signum of the comparison
            switch (signum (mpfr_cmp_ct (aplVfpLft, aplVfpRht, fQuadCT)))
            {
                case  1:
                    iMax = iLft - 1;

                    continue;

                case  0:
                    break;

                case -1:
                    iMin = iLft + 1;

                    continue;

                defstop
                    break;
            } // End SWITCH

            // Save the index of the match
            iLow = lpMemGupLft[iLft];

            // We found a match -- check earlier indices for a match
            //   so we always return the lowest index match.
            for (iCnt = iLft - 1; iCnt >= 0; iCnt--)
            {
                // Get the next VFP from the left arg
                mpfr_copy (&aplVfpLft, &lpMemLft[lpMemGupLft[iCnt]]);

                if (mpfr_cmp_ct (aplVfpLft, aplVfpRht, fQuadCT) NE 0)
                    break;
                else
                    iLow = min (iLow, lpMemGupLft[iCnt]);
            } // End FOR

            // Check for later but smaller indices for a match
            //   so we always return the lowest index match.
            for (iCnt = iLft + 1; iCnt < (APLINT) aplNELMLft; iCnt++)
            {
                // Get the next VFP from the left arg
                mpfr_copy (&aplVfpLft, &lpMemLft[lpMemGupLft[iCnt]]);

                if (mpfr_cmp_ct (aplVfpLft, aplVfpRht, fQuadCT) NE 0)
                    break;
                else
                    iLow = min (iLow, lpMemGupLft[iCnt]);
            } // End FOR
#ifdef GRADE2ND
            // Save in the result
            uLastVal       =
            lpMemRes[iRes] = bQuadIO + iLow;
#else
            // Save in the result
            lpMemRes[iRht] = bQuadIO + iLow;
#endif
            break;
        } // End WHILE

        // If we didn't find a match, ...
        if (iMin > iMax)
#ifdef GRADE2ND
            // Save in the result
            uLastVal       =
            lpMemRes[iRes] = NotFound;
#else
            // Save in the result
            lpMemRes[iRht] = NotFound;
#endif
    } // End FOR

    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
    if (hGlbGupLft && lpMemGupLft)
    {
        if (lpMemGupLft)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbGupLft); lpMemGupLft = NULL;
        } // End IF

        // We no longer need this resource
        FreeResultGlobalVar (hGlbGupLft); hGlbGupLft = NULL;
    } // End IF
#ifdef GRADE2ND
    if (hGlbGupRht && lpMemGupRht)
    {
        if (lpMemGupRht)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbGupRht); lpMemGupRht = NULL;
        } // End IF

        // We no longer need this resource
        FreeResultGlobalVar (hGlbGupRht); hGlbGupRht = NULL;
    } // End IF
#endif
    // We no longer need this storage
    Myf_clear (&aplVfpRht);
    Myf_clear (&aplVfpLft);

    return bRet;
} // End PrimFnDydIotaVvN_EM


//***************************************************************************
//  $PrimFnDydIotaOther
//
//  Dyadic iota between all other arg combinations
//***************************************************************************

UBOOL PrimFnDydIotaOther
    (LPAPLUINT lpMemRes,            // Ptr to result global memory data
     APLSTYPE  aplTypeLft,          // Left arg storage type
     APLNELM   aplNELMLft,          // Left arg NELM
     LPVOID    lpMemLft,            // Ptr to left arg global memory data
     APLSTYPE  aplTypeRht,          // Right arg storage type
     APLNELM   aplNELMRht,          // Right arg NELM
     LPVOID    lpMemRht,            // Ptr to right arg global memory data
     UBOOL     bQuadIO,             // []IO
     APLFLOAT  fQuadCT,                 // []CT
     APLUINT   NotFound,            // Not found value
     LPUBOOL   lpbCtrlBreak,        // Ptr to Ctrl-Break flag
     LPTOKEN   lptkFunc)            // Ptr to function token

{
    HGLOBAL      hGlbSubLft,        // Left arg item global memory handle
                 hGlbSubRht;        // Right ...
    APLLONGEST   aplLongestSubLft,  // Left arg item immediate value
                 aplLongestSubRht;  // Right ...
    IMM_TYPES    immTypeSubLft,     // Left arg item immediate type
                 immTypeSubRht;     // Right ...
    APLUINT      uLft,              // Loop counter
                 uRht;              // ...
    LPPL_YYSTYPE lpYYTmp;           // Ptr to the temporary result
    UBOOL        bCmp;              // TRUE iff the comparison is TRUE
    TOKEN        tkFunc = {0};      // Function token

    // This leaves:  Left vs.  Right
    //               BIAF vs.     HN
    //               HN   vs. BIAFHN

    // Setup the Match function token
    tkFunc.tkFlags.TknType   = TKT_FCNIMMED;
    tkFunc.tkFlags.ImmType   = IMMTYPE_PRIMFCN;
////tkFunc.tkFlags.NoDisplay = FALSE;           // Already zero from = {0}
    tkFunc.tkData.tkChar     = UTF16_EQUALUNDERBAR;
    tkFunc.tkCharIndex       = lptkFunc->tkCharIndex;

    // Loop through the right arg
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    {
        TOKEN tkSubRht = {0};       // Right arg item token

        // Get the next value from the right arg
        GetNextValueMem (lpMemRht,              // Ptr to right arg global memory
                         aplTypeRht,            // Right arg storage type
                         uRht,                  // Right arg index
                        &hGlbSubRht,            // Right arg item LPSYMENTRY or HGLOBAL (may be NULL)
                        &aplLongestSubRht,      // Ptr to right arg immediate value
                        &immTypeSubRht);        // Ptr to right arg immediate type
        // Fill in the right arg item token
        tkSubRht.tkFlags.TknType   = TKT_VARARRAY;
////////tkSubRht.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from = {0}
////////tkSubRht.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
        tkSubRht.tkData.tkGlbData  = hGlbSubRht;
        tkSubRht.tkCharIndex       = NEG1U;

        // Loop through the left arg
        for (uLft = 0; uLft < aplNELMLft; uLft++)
        {
            // Check for Ctrl-Break
            if (CheckCtrlBreak (*lpbCtrlBreak))
                goto ERROR_EXIT;

            // Get the next value from the left arg
            GetNextValueMem (lpMemLft,              // Ptr to left arg global memory
                             aplTypeLft,            // Left arg storage type
                             uLft,                  // Left arg index
                            &hGlbSubLft,            // Left arg item LPSYMENTRY or HGLOBAL (may be NULL)
                            &aplLongestSubLft,      // Ptr to left arg immediate value
                            &immTypeSubLft);        // Ptr to left arg immediate type
            // If both items are globals, ...
            if ((hGlbSubLft NE NULL) && (hGlbSubRht NE NULL))
            {
                TOKEN tkSubLft = {0};       // Left arg item token

                // Fill in the left arg item token
                tkSubLft.tkFlags.TknType   = TKT_VARARRAY;
////////////////tkSubLft.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from = {0}
////////////////tkSubLft.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
                tkSubLft.tkData.tkGlbData  = hGlbSubLft;
                tkSubLft.tkCharIndex       = NEG1U;

                // Use match to determine equality
                lpYYTmp =
                  PrimFnDydEqualUnderbar_EM_YY (&tkSubLft,      // Ptr to left arg token
                                                &tkFunc,        // Ptr to function token
                                                &tkSubRht,      // Ptr to right arg token
                                                 NULL);         // Ptr to axis token (may be NULL)
                // Save the result of the comparison
                bCmp = lpYYTmp->tkToken.tkData.tkBoolean;

                // Free the temporary result
                YYFree (lpYYTmp); lpYYTmp = NULL;

                if (bCmp)
                    goto SET_RESULT_VALUE;
            } else
            // If both items are simple, ...
            if ((hGlbSubLft EQ NULL) && (hGlbSubRht EQ NULL))
            {
                // If both items are APLCHARs, ...
                if (IsImmChr (immTypeSubLft) && IsImmChr (immTypeSubRht))
                {
                    // Compare the APLCHARs
                    if (((APLCHAR) aplLongestSubLft) EQ (APLCHAR) aplLongestSubRht)
                        goto SET_RESULT_VALUE;
                } else
                // If both items are numeric, ...
                if (IsImmNum (immTypeSubLft) && IsImmNum (immTypeSubRht))
                {
                    // This leaves BIF vs. BIF

                    // Split cases based upon the left arg item immediate type
                    switch (immTypeSubLft)
                    {
                        case IMMTYPE_BOOL:              // Lft = BOOL
                            // Split cases based upon the right arg item immediate type
                            switch (immTypeSubRht)
                            {
                                case IMMTYPE_BOOL:      // Lft = BOOL, Rht = BOOL
                                    if (((APLBOOL) aplLongestSubLft) EQ (APLBOOL) aplLongestSubRht)
                                        goto SET_RESULT_VALUE;
                                    break;

                                case IMMTYPE_INT:       // Lft = BOOL, Rht = INT
                                    if (((APLBOOL) aplLongestSubLft) EQ (APLINT)  aplLongestSubRht)
                                        goto SET_RESULT_VALUE;
                                    break;

                                case IMMTYPE_FLOAT:     // Lft = BOOL, Rht = FLOAT
                                    if (CompareCT ((APLBOOL) aplLongestSubLft, *(LPAPLFLOAT) &aplLongestSubRht, fQuadCT, NULL))
                                        goto SET_RESULT_VALUE;
                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        case IMMTYPE_INT:               // Lft = INT
                            // Split cases based upon the right arg item immediate type
                            switch (immTypeSubRht)
                            {
                                case IMMTYPE_BOOL:      // Lft = INT, Rht = BOOL
                                    if (((APLINT)  aplLongestSubLft) EQ (APLBOOL) aplLongestSubRht)
                                        goto SET_RESULT_VALUE;
                                    break;

                                case IMMTYPE_INT:       // Lft = INT, Rht = INT
                                    if (((APLINT)  aplLongestSubLft) EQ (APLINT)  aplLongestSubRht)
                                        goto SET_RESULT_VALUE;
                                    break;

                                case IMMTYPE_FLOAT:     // Lft = INT, Rht = FLOAT
                                    if (CompareCT ((APLFLOAT) (APLINT)  aplLongestSubLft, *(LPAPLFLOAT) &aplLongestSubRht, fQuadCT, NULL))
                                        goto SET_RESULT_VALUE;
                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        case IMMTYPE_FLOAT:             // Lft = FLOAT
                            // Split cases based upon the right arg item immediate type
                            switch (immTypeSubRht)
                            {
                                case IMMTYPE_BOOL:      // Lft = FLOAT, Rht = BOOL
                                    if (CompareCT (*(LPAPLFLOAT) &aplLongestSubLft, (APLBOOL) aplLongestSubRht, fQuadCT, NULL))
                                        goto SET_RESULT_VALUE;
                                    break;

                                case IMMTYPE_INT:       // Lft = FLOAT, Rht = INT
                                    if (CompareCT (*(LPAPLFLOAT) &aplLongestSubLft, (APLFLOAT) (APLINT) aplLongestSubRht, fQuadCT, NULL))
                                        goto SET_RESULT_VALUE;
                                    break;

                                case IMMTYPE_FLOAT:     // Lft = FLOAT, Rht = FLOAT
                                    if (CompareCT (*(LPAPLFLOAT) &aplLongestSubLft, *(LPAPLFLOAT) &aplLongestSubRht, fQuadCT, NULL))
                                        goto SET_RESULT_VALUE;
                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        defstop
                            break;
                    } // End SWITCH
                } // End IF/ELSE/...
            } // End IF/ELSE/...
        } // End FOR

        // We didn't find the it, so
        //   set the result value to NotFound
        *lpMemRes ++ = NotFound;

        continue;

SET_RESULT_VALUE:
        // Set the result value
        *lpMemRes++ = uLft + bQuadIO;
    } // End FOR

    return TRUE;
ERROR_EXIT:
    return FALSE;
} // End PrimFnDydIotaOther


//***************************************************************************
//  End of File: pf_iota.c
//***************************************************************************
