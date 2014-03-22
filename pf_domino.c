//***************************************************************************
//  NARS2000 -- Primitive Function -- Domino
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

#define __GSL_MATRIX_COMPLEX_LONG_DOUBLE_H__

#include "headers.h"
#include <gsl/gsl_vector_double.h>
#include <gsl/gsl_matrix_double.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_linalg.h>
#undef  inline
#define inline


//***************************************************************************
//  $PrimFnDomino_EM_YY
//
//  Primitive function for monadic and dyadic Domino ("matrix inverse" and "matrix divide")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDomino_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDomino_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_DOMINO);

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
        return PrimFnMonDomino_EM_YY             (lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydDomino_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimFnDomino_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnDomino_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic Domino
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnDomino_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnDomino_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnDomino_EM_YY, // Ptr to primitive function routine
                                    lptkLftArg,         // Ptr to left arg token
                                    lptkFunc,           // Ptr to function token
                                    lptkRhtArg,         // Ptr to right arg token
                                    lptkAxis);          // Ptr to axis token (may be NULL)
} // End PrimProtoFnDomino_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimIdentFnDomino_EM_YY
//
//  Generate an identity element for the primitive function dyadic Domino
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimIdentFnDomino_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimIdentFnDomino_EM_YY
    (LPTOKEN lptkRhtOrig,           // Ptr to original right arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeRht;        // Right arg storage type
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to identity element result
    HGLOBAL      hGlbRht = NULL,    // Right arg global memory handle
                 hGlbRes = NULL;    // Result    ...
    LPVOID       lpMemRht = NULL;   // Ptr to right arg global memory
    LPAPLBOOL    lpMemRes = NULL;   // Ptr to result    ...
    APLNELM      aplNELMRes;        // Result NELM
    APLRANK      aplRankRht;        // Right arg rank
    APLDIM       uNumRows,          // # rows in the right arg
                 uNumCols;          // # cols ...
    APLUINT      ByteRes,           // # bytes in the result
                 uRes;              // Loop counter

    // The right arg is the prototype item from
    //   the original empty arg.

    Assert (lptkRhtOrig NE NULL);
    Assert (lptkFunc    NE NULL);
    Assert (lptkRhtArg  NE NULL);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // The (right) identity function for dyadic Domino
    //   (L {domino} R) ("matrix divide") is
    //   ({iota}{first}{rho}R){jot}.={iota}{first}{rho}R.

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, NULL, &aplRankRht, NULL);

    // Check for RANK ERROR
    if (IsRank3P (aplRankRht))
        goto RANK_EXIT;

    // Get right arg's global ptrs
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Calculate the # rows & cols in the result
    switch (aplRankRht)
    {
        case 0:                 // 1x1 matrix
            uNumRows =
            uNumCols = 1;

            break;

        case 1:                 // 1-col matrix
            uNumRows = *VarArrayBaseToDim (lpMemRht);
            uNumCols = 1;

            break;

        case 2:
            uNumRows = (VarArrayBaseToDim (lpMemRht))[0];
            uNumCols = (VarArrayBaseToDim (lpMemRht))[1];

            break;

        defstop
            break;
    } // End SWITCH

    // Check for LENGTH ERROR
    if (IsMatrix (aplRankRht)
     && uNumRows < uNumCols)
        goto LENGTH_EXIT;

    // Check for DOMAIN ERROR
    if (!IsNumeric (aplTypeRht))
        goto DOMAIN_EXIT;

    // If the right arg is a scalar, the result is an immediate
    if (IsScalar (aplRankRht))
    {
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_BOOL;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkBoolean  = 1;
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

        goto NORMAL_EXIT;
    } // End IF

    //***************************************************************
    // From here on, the right arg (and the result) is a vector or matrix
    //***************************************************************

    // Calculate the result NELM
    aplNELMRes = uNumRows * uNumRows;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_BOOL, aplNELMRes, aplRankRht);

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
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_BOOL;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRht;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = (LPAPLBOOL) VarArrayBaseToDim (lpMemRes);

    // Save the dimension(s)
    // Split cases based upon the rank of the right arg (same as the result)
    switch (aplRankRht)
    {
        case 1:                                         // Vector
            *((LPAPLDIM) lpMemRes)++ = uNumRows;        // Length

            break;

        case 2:                                         // Matrix
            *((LPAPLDIM) lpMemRes)++ = uNumRows;        // # rows
            *((LPAPLDIM) lpMemRes)++ = uNumRows;        // # cols

            break;

        defstop
            break;
    } // End SWITCH

    //***************************************************************
    // lpMemRes now points to its data
    //***************************************************************

    // Loop through the result setting the major diagonal bits
    for (uRes = 0; uRes < aplNELMRes; uRes += (uNumRows + 1))
        lpMemRes[uRes >> LOG2NBIB] |= BIT0 << (MASKLOG2NBIB & (UINT) uRes);

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

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
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

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
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

    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return lpYYRes;
} // End PrimIdentFnDomino_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonDomino_EM_YY
//
//  Primitive function for monadic Domino ("matrix inverse")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonDomino_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonDomino_EM_YY
    (LPTOKEN lptkFunc,                  // Ptr to function token
     LPTOKEN lptkRhtArg,                // Ptr to right arg token
     LPTOKEN lptkAxis)                  // Ptr to axis token (may be NULL)

{
    APLSTYPE      aplTypeRht,           // Right arg storage type
                  aplTypeRes;           // Result    ...
    APLNELM       aplNELMRht,           // Right arg NELM
                  aplNELMRes;           // Result    ...
    APLRANK       aplRankRht,           // Right arg rank
                  aplRankRes;           // Result    ...
    HGLOBAL       hGlbRht = NULL,       // Right arg global memory handle
                  hGlbRes = NULL,       // Result    ...
                  hGlbTmp = NULL,       // Temporary ...
                  lpSymGlbRht;          // Ptr to right arg global numeric
    LPVOID        lpMemRht = NULL,      // Ptr to right arg global memory
                  lpMemRes = NULL,      // Ptr to result    ...
                  lpMemTmp = NULL;      // Ptr to temporary ...
    APLUINT       ByteRes;              // # bytes in the result
    APLDIM        uNumRows,             // # rows in the right arg
                  uNumCols,             // # cols ...
                  uRow,                 // Loop counter
                  uCol;                 // ...
    APLINT        apaOffRht,            // Right arg APA offset
                  apaMulRht;            // ...           multiplier
    APLFLOAT      aplFloatRht;          // Right arg temporary float
    LPPL_YYSTYPE  lpYYRes = NULL;       // Ptr to the result
    UINT          uBitMask;             // Bit mask for marching through Booleans
    gsl_matrix   *lpGslMatrixU = NULL,
                 *lpGslMatrixV = NULL;
    gsl_vector   *lpGslVectorS = NULL,
                 *lpGslVectorW = NULL,
                 *lpGslVectorI = NULL;
    int           ErrCode;
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Get right arg's global ptrs
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Check for RANK ERROR
    if (IsRank3P (aplRankRht))
        goto RANK_EXIT;

    // Calculate the # rows & cols in the result
    switch (aplRankRht)
    {
        case 0:                 // 1x1 matrix
            uNumRows =
            uNumCols = 1;
            GetFirstValueToken (lptkRhtArg,     // Ptr to right arg token
                                NULL,           // Ptr to integer result
                               &aplFloatRht,    // Ptr to float ...
                                NULL,           // Ptr to WCHAR ...
                                NULL,           // Ptr to longest ...
                               &lpSymGlbRht,    // Ptr to lpSym/Glb ...
                                NULL,           // Ptr to ...immediate type ...
                                NULL);          // Ptr to array type ...
            break;

        case 1:                 // 1-col matrix
            uNumRows = *VarArrayBaseToDim (lpMemRht);
            uNumCols = 1;

            break;

        case 2:
            uNumRows = (VarArrayBaseToDim (lpMemRht))[0];
            uNumCols = (VarArrayBaseToDim (lpMemRht))[1];

            break;

        defstop
            break;
    } // End SWITCH

    // Check for LENGTH ERROR
    if (IsMatrix (aplRankRht)
     && uNumRows < uNumCols)
        goto LENGTH_EXIT;

    // Check for DOMAIN ERROR
    if (!IsNumeric (aplTypeRht))
        goto DOMAIN_EXIT;

    // Calculate the result storage type
    if (IsSimpleNum (aplTypeRht))
        aplTypeRes = ARRAY_FLOAT;
    else
        aplTypeRes = aplTypeRht;

    // If the right arg is a scalar, the result is an immediate or scalar global numeric, ...
    if (IsScalar (aplRankRht))
    {
        IMM_TYPES immTypeRes;
        APLRAT    aplRatRht = {0};
        APLVFP    aplVfpRht = {0};

        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   =            // Filled in below
////////lpYYRes->tkToken.tkFlags.ImmType   =            // Filled in below
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;     // Already zero from YYAlloc
////////lpYYRes->tkToken.tkData.tkFloat    =            // Filled in below
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
            case ARRAY_FLOAT:
                // Set the immediate type
                immTypeRes = IMMTYPE_FLOAT;

                // If the right arg is zero, the result is zero
                //   as per SVD
                if (aplFloatRht NE 0)
                    // Invert it
                    aplFloatRht = 1.0 / aplFloatRht;

                // Save in the result
                lpYYRes->tkToken.tkFlags.ImmType   = immTypeRes;
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
                lpYYRes->tkToken.tkData.tkFloat    = aplFloatRht;

                break;

            case ARRAY_RAT:
                // Set the immediate type
                immTypeRes = IMMTYPE_RAT;

                // Copy the value
                mpq_init_set (&aplRatRht, (LPAPLRAT) lpSymGlbRht);

                // If the right arg is zero, the result is zero
                //   as per SVD
                if (!IsMpq0 (&aplRatRht))
                    // Invert it
                    mpq_inv (&aplRatRht, &aplRatRht);

                // Save in the result
                lpYYRes->tkToken.tkFlags.ImmType   = immTypeRes;
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
                lpYYRes->tkToken.tkData.tkGlbData  =
                  MakeGlbEntry_EM (aplTypeRes,
                                  &aplRatRht,
                                   FALSE,
                                   lptkFunc);
                // Check for errors
                if (!lpYYRes->tkToken.tkData.tkGlbData)
                    goto WSFULL_EXIT;
                break;

            case ARRAY_VFP:
                // Set the immediate type
                immTypeRes = IMMTYPE_VFP;

                // Copy the value
                mpfr_init_copy (&aplVfpRht, (LPAPLVFP) lpSymGlbRht);

                // If the right arg is zero, the result is zero
                //   as per SVD
                if (!IsMpf0 (&aplVfpRht))
                    // Invert it
                    mpfr_inv (&aplVfpRht, &aplVfpRht, MPFR_RNDN);

                // Save in the result
                lpYYRes->tkToken.tkFlags.ImmType   = immTypeRes;
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
                lpYYRes->tkToken.tkData.tkGlbData  =
                  MakeGlbEntry_EM (aplTypeRes,
                                  &aplVfpRht,
                                   FALSE,
                                   lptkFunc);
                // Check for errors
                if (!lpYYRes->tkToken.tkData.tkGlbData)
                    goto WSFULL_EXIT;
                break;

            defstop
                break;
        } // End SWITCH

        goto NORMAL_EXIT;
    } // End IF

    //***************************************************************
    // From here on, the right arg (and the result) is a vector or matrix
    //***************************************************************

    // If the right arg is an overdetermined global numeric array, ...
    if (uNumRows > uNumCols
     && IsGlbNum (aplTypeRht))
    {
        HGLOBAL      hGlbMFO;           // Magic function/operator ...
        LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

        // Get ptr to PerTabData global memory
        lpMemPTD = GetMemPTD ();

        // Get the magic function/operator global memory handle
        hGlbMFO = lpMemPTD->hGlbMFO[MFOE_MonDomino];

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
    } // End IF

    // The rank of the result is the same as
    //   that of the right arg
    aplRankRes = aplRankRht;

    // Save the NELM of the result
    Assert (aplNELMRht EQ (uNumRows * uNumCols));
////aplNELMRes = uNumRows * uNumCols;
    aplNELMRes = aplNELMRht;

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

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = (aplNELMRes EQ 0) ? ARRAY_BOOL : aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Fill in the dimensions
    if (IsVector (aplRankRes))
        *VarArrayBaseToDim (lpMemRes) = uNumRows;
    else
    {
        // Reverse dimensions from right arg
        (VarArrayBaseToDim (lpMemRes))[0] = uNumCols;
        (VarArrayBaseToDim (lpMemRes))[1] = uNumRows;
    } // End IF/ELSE

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayDataFmBase (lpMemRht);
    lpMemRes = VarArrayDataFmBase (lpMemRes);

    //***************************************************************
    // lpMemRes and lpMemRht now point to their data
    //***************************************************************

    // Check for no rows as gsl_linalg_SV_decomp doesn't handle it well
    if (uNumRows EQ 0)
        goto YYALLOC_EXIT;

    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        case ARRAY_FLOAT:
            // No aborting on error!
            gsl_set_error_handler_off ();

            // Allocate space for the GSL matrices
            Assert (uNumRows EQ (APLU3264) uNumRows);
            Assert (uNumCols EQ (APLU3264) uNumCols);

            lpGslMatrixU = gsl_matrix_alloc  ((APLU3264) uNumRows, (APLU3264) uNumCols);    // M x N
            lpGslMatrixV = gsl_matrix_alloc  ((APLU3264) uNumCols, (APLU3264) uNumCols);    // N x N
            lpGslVectorS = gsl_vector_alloc  ((APLU3264) uNumCols);                         // N
            lpGslVectorW = gsl_vector_alloc  ((APLU3264) uNumCols);                         // N

            // Check the return codes for the above allocations
            if (GSL_ENOMEM EQ (HANDLE_PTR) lpGslMatrixU
             || GSL_ENOMEM EQ (HANDLE_PTR) lpGslMatrixV
             || GSL_ENOMEM EQ (HANDLE_PTR) lpGslVectorS
             || GSL_ENOMEM EQ (HANDLE_PTR) lpGslVectorW)
                goto WSFULL_EXIT;

            // Copy the right arg to the GSL matrix U
            // Split cases based upon the right arg's rank
            switch (aplRankRht)
            {
////////////////case 0:         // Scalar
////////////////    lpGslMatrixU->data[0] = aplFloatRht;
////////////////
////////////////    break;
////////////////
                case 1:         // Vector
                case 2:         // Matrix
                    // Split cases based upon the right arg storage type
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:
                            uBitMask = BIT0;

                            for (uCol = 0; uCol < aplNELMRht; uCol++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                lpGslMatrixU->data[uCol] = (uBitMask & *(LPAPLBOOL) lpMemRht) ? TRUE : FALSE;

                                // Shift over the bit mask
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
                            for (uCol = 0; uCol < aplNELMRht; uCol++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                lpGslMatrixU->data[uCol] = (APLFLOAT) ((LPAPLINT) lpMemRht)[uCol];
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:
                            // Calculate space needed for the result
                            ByteRes = aplNELMRes * sizeof (APLFLOAT);

                            // Check for overflow
                            if (ByteRes NE (APLU3264) ByteRes)
                                goto WSFULL_EXIT;

                            Assert (sizeof (double) EQ sizeof (APLFLOAT));
                            CopyMemory (lpGslMatrixU->data, lpMemRht, (APLU3264) ByteRes);
////////////////////////////for (uCol = 0; uCol < aplNELMRht; uCol++)
////////////////////////////{
////////////////////////////    // Check for Ctrl-Break
////////////////////////////    if (CheckCtrlBreak (*lpbCtrlBreak))
////////////////////////////        goto ERROR_EXIT;
////////////////////////////
////////////////////////////    lpGslMatrixU->data[uCol] = ((LPAPLFLOAT) lpMemRht)[uCol];
////////////////////////////} // End FOR

                            break;

                        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemRht)
                            // Get the APA parameters
                            apaOffRht = lpAPA->Off;
                            apaMulRht = lpAPA->Mul;
#undef  lpAPA
                            for (uCol = 0; uCol < aplNELMRht; uCol++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                lpGslMatrixU->data[uCol] = (APLFLOAT) (APLINT) (apaOffRht + apaMulRht * uCol);
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                defstop
                    break;
            } // End SWITCH

            // Calculate the SVD (Singular Value Decomposition)
            //   with the result in U and S (S is the diagonal of the matrix)
            ErrCode =
              gsl_linalg_SV_decomp (lpGslMatrixU,               // M x N
                                    lpGslMatrixV,               // N x N
                                    lpGslVectorS,               // N
                                    lpGslVectorW);              // N
            // Check the error code
            if (ErrCode NE GSL_SUCCESS)
                goto DOMAIN_EXIT;

            // Solve the system Ax = b
            //   where x is the result, and b is the MxM identity matrix

            // Allocate and zero a working vector (for the identity matrix)
            lpGslVectorI = gsl_vector_alloc  ((APLU3264) uNumRows); // M

            // Check the return code for the above allocation
            if (GSL_ENOMEM EQ (HANDLE_PTR) lpGslVectorI)
                goto WSFULL_EXIT;

            gsl_vector_set_zero (lpGslVectorI);

            // The solution rows are now L[;I] +.x V +.x (1/S) +.x U*
            //   where L is an identity matrix
            for (uRow = 0; uRow < uNumRows; uRow++)             // 0 to M-1
            {
                // If we're not at the first row, clear the previous value
                if (uRow NE 0)
                    lpGslVectorI->data[uRow - 1] = 0;

                // Set the next value
                lpGslVectorI->data[uRow] = 1;

                // Call GSL function to solve for this column
                ErrCode =
                  gsl_linalg_SV_solve (lpGslMatrixU,            // M x N
                                       lpGslMatrixV,            // N x N
                                       lpGslVectorS,            // N
                                       lpGslVectorI,            // M
                                       lpGslVectorW);           // N
                // Check the error code
                if (ErrCode NE GSL_SUCCESS)
                    goto DOMAIN_EXIT;

#define lpMemData   ((LPAPLFLOAT) lpMemRes)

                // Copy the values from W to the result
                for (uCol = 0; uCol < uNumCols; uCol++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    lpMemData[uCol * uNumRows + uRow] = lpGslVectorW->data[uCol];
                } // End FOR

#undef  lpMemData
            } // End FOR
            break;

        case ARRAY_RAT:
            // Loop through the rows and cols
            for (uRow = 0; uRow < uNumRows; uRow++)
            for (uCol = 0; uCol < uNumCols; uCol++)
                // Initialize the matrix
                mpq_init_set (&((LPAPLRAT) lpMemRes)[uRow * uNumCols + uCol],
                              &((LPAPLRAT) lpMemRht)[uRow * uNumCols + uCol]);

            // Use Gauss-Jordan elimination to invert the matrix
            if (!GaussJordan ((LPAPLRAT) lpMemRes, uNumRows, uNumCols, NULL, 0, lpbCtrlBreak))
                goto DOMAIN_EXIT;

            break;

        case ARRAY_VFP:
            // Allocate a temp array to hold the VFPs when converted to RATs
            hGlbTmp = DbgGlobalAlloc (GHND, (APLU3264) (uNumRows * uNumCols * sizeof (APLRAT)));
            if (!hGlbTmp)
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpMemTmp = MyGlobalLock (hGlbTmp);

            // Loop through the rows and cols
            for (uRow = 0; uRow < uNumRows; uRow++)
            for (uCol = 0; uCol < uNumCols; uCol++)
            {
                // Initialize the entry
                mpq_init   (&((LPAPLRAT) lpMemTmp)[uRow * uNumRows + uCol]);

                // Copy and convert the entry
                mpq_set_fr (&((LPAPLRAT) lpMemTmp)[uRow * uNumRows + uCol],
                            &((LPAPLVFP) lpMemRht)[uRow * uNumCols + uCol]);
            } // End FOR/FOR

            // Use Gauss-Jordan elimination to invert the matrix
            if (!GaussJordan ((LPAPLRAT) lpMemTmp, uNumRows, uNumCols, NULL, 0, lpbCtrlBreak))
                goto DOMAIN_EXIT;

            // Copy to the result and convert back to VFPs
            for (uRow = 0; uRow < uNumRows; uRow++)
            for (uCol = 0; uCol < uNumCols; uCol++)
            {
                // Initialize the entry
                mpfr_init0 (&((LPAPLVFP) lpMemRes)[uRow * uNumRows + uCol]);

                // Copy and convert the entry
                mpfr_set_q (&((LPAPLVFP) lpMemRes)[uRow * uNumRows + uCol],
                            &((LPAPLRAT) lpMemTmp)[uRow * uNumRows + uCol],
                            MPFR_RNDN);
            } // End FOR/FOR

            break;

        defstop
            break;
    } // End SWITCH
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

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
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
    if (lpGslVectorI)
    {
        // We no longer need this storage and ptr
        gsl_vector_free (lpGslVectorI); lpGslVectorI = NULL;
    } // End IF

    if (lpGslVectorW)
    {
        // We no longer need this storage and ptr
        gsl_vector_free (lpGslVectorW); lpGslVectorW = NULL;
    } // End IF

    if (lpGslVectorS)
    {
        // We no longer need this storage and ptr
        gsl_vector_free (lpGslVectorS); lpGslVectorS = NULL;
    } // End IF

    if (lpGslMatrixV)
    {
        // We no longer need this storage and ptr
        gsl_matrix_free (lpGslMatrixV); lpGslMatrixV = NULL;
    } // End IF

    if (lpGslMatrixU)
    {
        // We no longer need this storage and ptr
        gsl_matrix_free (lpGslMatrixU); lpGslMatrixU = NULL;
    } // End IF

    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    if (hGlbTmp)
    {
        if (lpMemTmp)
        {
            // Loop through the rows and cols
            for (uRow = 0; uRow < uNumRows; uRow++)
            for (uCol = 0; uCol < uNumCols; uCol++)
                Myq_clear (&((LPAPLRAT) lpMemTmp)[uRow * uNumRows + uCol]);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbTmp); lpMemTmp = NULL;
        } // End IF

        // We no longer need this storage
        MyGlobalFree (hGlbTmp); hGlbTmp = NULL;
    } // End IF

    return lpYYRes;
} // End PrimFnMonDomino_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  Magic function/operator for Monadic Domino for Global Numerics
//
//  Return the pseudo-inverse of an array of global numerics.
//***************************************************************************
//***************************************************************************
//  Magic function/operator for Dyadic Domino for Global Numerics
//
//  Return the pseudo-inverse of an array of global numerics.
//***************************************************************************

#include "mf_domino.h"


//***************************************************************************
//  $PrimFnDydDomino_EM_YY
//
//  Primitive function for dyadic Domino ("matrix divide")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydDomino_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydDomino_EM_YY
    (LPTOKEN lptkLftArg,                // Ptr to left arg token
     LPTOKEN lptkFunc,                  // Ptr to function token
     LPTOKEN lptkRhtArg,                // Ptr to right arg token
     LPTOKEN lptkAxis)                  // Ptr to axis token (may be NULL)

{
    APLSTYPE      aplTypeLft,           // Left arg storage type
                  aplTypeRht,           // Right ...
                  aplTypeRes;           // Result    ...
    APLNELM       aplNELMLft,           // Left arg NELM
                  aplNELMRht,           // Right ...
                  aplNELMRes;           // Result   ...
    APLRANK       aplRankLft,           // Left arg rank
                  aplRankRht,           // Right ...
                  aplRankRes;           // Result   ...
    HGLOBAL       hGlbLft = NULL,       // Left arg global memory handle
                  hGlbRht = NULL,       // Right ...
                  hGlbRes = NULL,       // Result   ...
                  hGlbTmpLft = NULL,    // Temporary left ...
                  hGlbTmpRht = NULL,    // ...       right ...
                  lpSymGlbLft,          // Ptr to left arg global numeric
                  lpSymGlbRht;          // Ptr to right ...
    LPVOID        lpMemLft = NULL,      // Ptr to left arg global memory
                  lpMemRht = NULL,      // Ptr to right ...
                  lpMemRes = NULL;      // Ptr to result   ...
    LPAPLRAT      lpMemTmpLft = NULL,   // Ptr to temporary left arg global memory
                  lpMemTmpRht = NULL;   // ...             right ...
    APLUINT       ByteRes;              // # bytes in the result
    APLDIM        uNumRowsLft,          //
                  uNumColsLft,          //
                  uNumRowsRht,          //
                  uNumColsRht,          //
                  uNumRowsRes,          //
                  uNumColsRes,          //
                  uRow,                 //
                  uCol;                 //
    APLINT        apaOffLft,            // Left arg APA offset
                  apaMulLft,            // ...           multiplier
                  apaOffRht,            // Right arg APA offset
                  apaMulRht;            // ...           multiplier
    APLFLOAT      aplFloatLft,          //
                  aplFloatRht,          //
                  aplFloatRes;          //
    UBOOL         bRet = TRUE;          // TRUE iff result is valid
    LPPL_YYSTYPE  lpYYRes = NULL;       // Ptr to the result
    UINT          uBitMask;             // Bit mask for marching through Booleans
    gsl_matrix   *lpGslMatrixU = NULL,
                 *lpGslMatrixV = NULL;
    gsl_vector   *lpGslVectorS = NULL,
                 *lpGslVectorW = NULL,
                 *lpGslVectorX = NULL,
                 *lpGslVectorB = NULL;
    int           ErrCode;
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    APLRAT        aplRatTmp = {0};      // Temporary RAT

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Get left and right arg's global ptrs
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Check for RANK ERROR
    if (IsRank3P (aplRankLft) || IsRank3P (aplRankRht))
        goto RANK_EXIT;

    // Calculate the # rows & cols in the right arg
    switch (aplRankRht)
    {
        case 0:                 // 1x1 matrix
            uNumRowsRht =
            uNumColsRht = 1;
            GetFirstValueToken (lptkRhtArg,     // Ptr to right arg token
                                NULL,           // Ptr to integer result
                               &aplFloatRht,    // Ptr to float ...
                                NULL,           // Ptr to WCHAR ...
                                NULL,           // Ptr to longest ...
                               &lpSymGlbRht,    // Ptr to lpSym/Glb ...
                                NULL,           // Ptr to ...immediate type ...
                                NULL);          // Ptr to array type ...
            break;

        case 1:                 // 1-col matrix
            uNumRowsRht = *VarArrayBaseToDim (lpMemRht);
            uNumColsRht = 1;

            break;

        case 2:
            uNumRowsRht = (VarArrayBaseToDim (lpMemRht))[0];
            uNumColsRht = (VarArrayBaseToDim (lpMemRht))[1];

            break;

        defstop
            break;
    } // End SWITCH

    // Calculate the # rows & cols in the left arg
    switch (aplRankLft)
    {
        case 0:                 // 1x1 matrix
            uNumRowsLft =
            uNumColsLft = 1;
            GetFirstValueToken (lptkLftArg,     // Ptr to left arg token
                                NULL,           // Ptr to integer result
                               &aplFloatLft,    // Ptr to float ...
                                NULL,           // Ptr to WCHAR ...
                                NULL,           // Ptr to longest ...
                               &lpSymGlbLft,    // Ptr to lpSym/Glb ...
                                NULL,           // Ptr to ...immediate type ...
                                NULL);          // Ptr to array type ...
            break;

        case 1:                 // 1-col matrix
            uNumRowsLft = *VarArrayBaseToDim (lpMemLft);
            uNumColsLft = 1;

            break;

        case 2:
            uNumRowsLft = (VarArrayBaseToDim (lpMemLft))[0];
            uNumColsLft = (VarArrayBaseToDim (lpMemLft))[1];

            break;

        defstop
            break;
    } // End SWITCH

    // Check for LENGTH ERROR
    if (uNumRowsRht <  uNumColsRht
     || uNumRowsLft NE uNumRowsRht)
        goto LENGTH_EXIT;

    // Check for DOMAIN ERROR
    if (!IsNumeric (aplTypeLft)
     || !IsNumeric (aplTypeRht))
        goto DOMAIN_EXIT;

    // Save the # rows & cols in the result
    uNumRowsRes = uNumColsRht;
    uNumColsRes = uNumColsLft;

    // Save the rank of the result
    aplRankRes = aplRankLft + aplRankRht;
    aplRankRes = max (aplRankRes, 2) - 2;

    // Save the NELM of the result
    aplNELMRes = uNumRowsRes * uNumColsRes;

    // Calculate the result storage type
    aplTypeRes = aTypePromote[aplTypeLft][aplTypeRht];

    // If the result is simple, ...
    if (IsSimpleNum (aplTypeRes))
        // Make it a float
        aplTypeRes = ARRAY_FLOAT;

    // If the args are overdetermined global numeric arrays, ...
    if (uNumRowsRht > uNumColsRht
     && (IsGlbNum (aplTypeLft)
      || IsGlbNum (aplTypeRht)))
    {
        HGLOBAL      hGlbMFO;           // Magic function/operator ...
        LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

        // Get ptr to PerTabData global memory
        lpMemPTD = GetMemPTD ();

        // Get the magic function/operator global memory handle
        hGlbMFO = lpMemPTD->hGlbMFO[MFOE_DydDomino];

        //  Return the matrix of indices
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
    } else
    {
        // Calculate space needed for the result
        if (!IsScalar (aplRankRes)
         || IsGlbNum (aplTypeRes))
        {
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

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
            // Fill in the header values
            lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
            lpHeader->ArrType    = (aplNELMRes EQ 0) ? ARRAY_BOOL : aplTypeRes;
////////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////////lpHeader->SysVar     = FALSE;       // Already zero from GHND
            lpHeader->RefCnt     = 1;
            lpHeader->NELM       = aplNELMRes;
            lpHeader->Rank       = aplRankRes;
#undef  lpHeader

            // Fill in the dimensions
            if (IsVector (aplRankRes))
                *VarArrayBaseToDim (lpMemRes) = uNumRowsRes;
            else
            if (IsMatrix (aplRankRes))
            {
                (VarArrayBaseToDim (lpMemRes))[0] = uNumRowsRes;
                (VarArrayBaseToDim (lpMemRes))[1] = uNumColsRes;
            } // End IF/ELSE
        } // End IF

        // Skip over the header and dimensions to the data
        if (!IsScalar (aplRankLft))
            lpMemLft = VarArrayDataFmBase (lpMemLft);
        if (!IsScalar (aplRankRht))
            lpMemRht = VarArrayDataFmBase (lpMemRht);
        if (!IsScalar (aplRankRes) || IsGlbNum (aplTypeRes))
            lpMemRes = VarArrayDataFmBase (lpMemRes);

        // Check for no rows as gsl_linalg_SV_decomp doesn't handle it well
        if (uNumRowsRht EQ 0)
            goto YYALLOC_EXIT;

        // Split cases based upon the result storage type
        switch (aplTypeRes)
        {
            case ARRAY_FLOAT:
                // No aborting on error!
                gsl_set_error_handler_off ();

                // Ensure no overflow
                Assert (uNumRowsLft EQ (APLU3264) uNumRowsLft);
                Assert (uNumRowsRht EQ (APLU3264) uNumRowsRht);

                // Allocate space for the GSL matrices
                lpGslMatrixU = gsl_matrix_alloc  ((APLU3264) uNumRowsRht, (APLU3264) uNumColsRht);  // M x N
                lpGslMatrixV = gsl_matrix_alloc  ((APLU3264) uNumColsRht, (APLU3264) uNumColsRht);  // N x N
                lpGslVectorS = gsl_vector_alloc  ((APLU3264) uNumColsRht);                      // N
                lpGslVectorW = gsl_vector_alloc  ((APLU3264) uNumColsRht);                      // N
                lpGslVectorX = gsl_vector_alloc  ((APLU3264) uNumColsRht);                      // N
                lpGslVectorB = gsl_vector_alloc  ((APLU3264) uNumRowsRht);                      // M

                // Check the return codes for the above allocations
                if (GSL_ENOMEM EQ (HANDLE_PTR) lpGslMatrixU
                 || GSL_ENOMEM EQ (HANDLE_PTR) lpGslMatrixV
                 || GSL_ENOMEM EQ (HANDLE_PTR) lpGslVectorS
                 || GSL_ENOMEM EQ (HANDLE_PTR) lpGslVectorW
                 || GSL_ENOMEM EQ (HANDLE_PTR) lpGslVectorX)
                    goto WSFULL_EXIT;

                // Copy the right arg to the GSL matrix U
                // Split cases based upon the right arg's rank
                switch (aplRankRht)
                {
                    case 0:         // Scalar
                        lpGslMatrixU->data[0] = aplFloatRht;

                        break;

                    case 1:         // Vector
                    case 2:         // Matrix
                        // Split cases based upon the right arg storage type
                        switch (aplTypeRht)
                        {
                            case ARRAY_BOOL:
                                uBitMask = BIT0;

                                for (uCol = 0; uCol < aplNELMRht; uCol++)
                                {
                                    // Check for Ctrl-Break
                                    if (CheckCtrlBreak (*lpbCtrlBreak))
                                        goto ERROR_EXIT;

                                    lpGslMatrixU->data[uCol] = (uBitMask & *(LPAPLBOOL) lpMemRht) ? TRUE : FALSE;

                                    // Shift over the bit mask
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
                                for (uCol = 0; uCol < aplNELMRht; uCol++)
                                {
                                    // Check for Ctrl-Break
                                    if (CheckCtrlBreak (*lpbCtrlBreak))
                                        goto ERROR_EXIT;

                                    lpGslMatrixU->data[uCol] = (APLFLOAT) ((LPAPLINT) lpMemRht)[uCol];
                                } // End FOR

                                break;

                            case ARRAY_FLOAT:
                                // Calculate space needed for the result
                                ByteRes = aplNELMRht * sizeof (APLFLOAT);

                                // Check for overflow
                                if (ByteRes NE (APLU3264) ByteRes)
                                    goto WSFULL_EXIT;

                                Assert (sizeof (double) EQ sizeof (APLFLOAT));
                                CopyMemory (lpGslMatrixU->data, lpMemRht, (APLU3264) ByteRes);
////////////////////////////////for (uCol = 0; uCol < aplNELMRht; uCol++)
////////////////////////////////{
////////////////////////////////    // Check for Ctrl-Break
////////////////////////////////    if (CheckCtrlBreak (*lpbCtrlBreak))
////////////////////////////////        goto ERROR_EXIT;
////////////////////////////////
////////////////////////////////    lpGslMatrixU->data[uCol] = ((LPAPLFLOAT) lpMemRht)[uCol];
////////////////////////////////} // End FOR

                                break;

                            case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemRht)
                                // Get the APA parameters
                                apaOffRht = lpAPA->Off;
                                apaMulRht = lpAPA->Mul;
#undef  lpAPA
                                for (uCol = 0; uCol < aplNELMRht; uCol++)
                                {
                                    // Check for Ctrl-Break
                                    if (CheckCtrlBreak (*lpbCtrlBreak))
                                        goto ERROR_EXIT;

                                    lpGslMatrixU->data[uCol] = (APLFLOAT) (APLINT) (apaOffRht + apaMulRht * uCol);
                                } // End FOR

                                break;

                            defstop
                                break;
                        } // End SWITCH

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Calculate the SVD (Singular Value Decomposition)
                //   with the result in U and S (S is the diagonal of the matrix)
                ErrCode =
                  gsl_linalg_SV_decomp (lpGslMatrixU,               // M x N
                                        lpGslMatrixV,               // N x N
                                        lpGslVectorS,               // N
                                        lpGslVectorW);              // N
                // Check the error code
                if (ErrCode NE GSL_SUCCESS)
                    goto DOMAIN_EXIT;

                // Free the GSL work vector
                gsl_vector_free (lpGslVectorW); lpGslVectorW = NULL;

                // The solution rows are now L[;I] +.x V +.x (1/S) +.x U*
                for (uCol = 0; uCol < uNumColsLft; uCol++)
                {
                    // Copy the next column from L to B
                    if (IsScalar (aplRankLft))
                        lpGslVectorB->data[0] = aplFloatLft;
                    else
                    // Split cases based upon the left arg's storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:
                            uBitMask = BIT0;

                            Assert (uNumRowsLft EQ lpGslVectorB->size);
                            for (uRow = 0; uRow < uNumRowsLft; uRow++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                lpGslVectorB->data[uRow] = (uBitMask & *(LPAPLBOOL) lpMemLft) ? TRUE : FALSE;

                                // Shift over the bit mask
                                uBitMask <<= 1;

                                // Check for end-of-byte
                                if (uBitMask EQ END_OF_BYTE)
                                {
                                    uBitMask = BIT0;            // Start over
                                    ((LPAPLBOOL) lpMemLft)++;   // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:
                            Assert (uNumRowsLft EQ lpGslVectorB->size);
                            for (uRow = 0; uRow < uNumRowsLft; uRow++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                lpGslVectorB->data[uRow] = (APLFLOAT) ((LPAPLINT) lpMemLft)[uRow * uNumColsLft + uCol];
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:
                            Assert (uNumRowsLft EQ lpGslVectorB->size);
                            for (uRow = 0; uRow < uNumRowsLft; uRow++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                lpGslVectorB->data[uRow] = ((LPAPLFLOAT) lpMemLft)[uRow * uNumColsLft + uCol];
                            } // End FOR

                            break;

                        case ARRAY_APA:
                            Assert (uNumRowsLft EQ lpGslVectorB->size);
#define lpAPA       ((LPAPLAPA) lpMemLft)
                            // Get the APA parameters
                            apaOffLft = lpAPA->Off;
                            apaMulLft = lpAPA->Mul;
#undef  lpAPA
                            for (uRow = 0; uRow < uNumRowsLft; uRow++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                lpGslVectorB->data[uRow] = (APLFLOAT) (APLINT) (apaOffLft + apaMulLft * (uRow * uNumColsLft + uCol));
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    // Call GSL function to solve for this column
                    ErrCode =
                      gsl_linalg_SV_solve (lpGslMatrixU,            // M x N
                                           lpGslMatrixV,            // N x N
                                           lpGslVectorS,            // N
                                           lpGslVectorB,            // M
                                           lpGslVectorX);           // N
                    // Check the error code
                    if (ErrCode NE GSL_SUCCESS)
                        goto DOMAIN_EXIT;

#define lpMemData   ((LPAPLFLOAT) lpMemRes)

                    // Copy X to the next column in the result
                    Assert (uNumRowsRes EQ lpGslVectorX->size);
                    if (IsScalar (aplRankRes))
                        aplFloatRes = lpGslVectorX->data[0];
                    else
                    for (uRow = 0; uRow < uNumRowsRes; uRow++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        lpMemData[uRow * uNumColsRes + uCol] = lpGslVectorX->data[uRow];
                    } // End IF/ELSE/...

#undef  lpMemData
                } // End FOR

                break;

            case ARRAY_RAT:
            case ARRAY_VFP:
                // Ensure no overflow
                Assert (uNumRowsLft EQ (APLU3264) uNumRowsLft);
                Assert (uNumRowsRht EQ (APLU3264) uNumRowsRht);

                // Initialize the temp
                mpq_init (&aplRatTmp);

                // Create the temporary left and right APLRAT matrices
                hGlbTmpLft = DbgGlobalAlloc (GHND, (APLU3264) (uNumRowsLft * uNumColsLft * sizeof (APLRAT)));
                hGlbTmpRht = DbgGlobalAlloc (GHND, (APLU3264) (uNumRowsRht * uNumColsRht * sizeof (APLRAT)));

                if (hGlbTmpLft EQ NULL
                 || hGlbTmpRht EQ NULL)
                    goto WSFULL_EXIT;

                // Lock the memory to get ptrs to 'em
                lpMemTmpLft = MyGlobalLock (hGlbTmpLft);
                lpMemTmpRht = MyGlobalLock (hGlbTmpRht);

                // Loop through the left arg rows and cols
                for (uRow = 0; uRow < uNumRowsLft; uRow++)
                for (uCol = 0; uCol < uNumColsLft; uCol++)
                {
                    // Split cases based upon the left arg storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:
                        case ARRAY_INT:
                        case ARRAY_APA:
                            // Convert the BOOL/INT/APA to a RAT
                            mpq_set_sx (&aplRatTmp, GetNextInteger (lpMemLft, aplTypeLft, uRow * uNumColsLft + uCol), 1);

                            break;

                        case ARRAY_FLOAT:
                            // Convert the FLOAT to a RAT
                            mpq_set_d  (&aplRatTmp, ((LPAPLFLOAT) lpMemLft)[uRow * uNumColsLft + uCol]);

                            break;

                        case ARRAY_RAT:
                            // Copy the RAT
                            mpq_set    (&aplRatTmp, &((LPAPLRAT ) lpMemLft)[uRow * uNumColsLft + uCol]);

                            break;

                        case ARRAY_VFP:
                            // Convert the VFP to a RAT
                            mpq_set_fr (&aplRatTmp, &((LPAPLVFP ) lpMemLft)[uRow * uNumColsLft + uCol]);

                            break;

                        case ARRAY_CHAR:
                        case ARRAY_HETERO:
                        case ARRAY_NESTED:
                        defstop
                            break;
                    } // End SWITCH

                    // Copy to the matrix entry
                    mpq_init_set (&lpMemTmpLft[uRow * uNumColsLft + uCol], &aplRatTmp);
                } // End FOR/FOR

                // Loop through the right arg rows and cols
                for (uRow = 0; uRow < uNumRowsRht; uRow++)
                for (uCol = 0; uCol < uNumColsRht; uCol++)
                {
                    // Split cases based upon the right arg storage type
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:
                        case ARRAY_INT:
                        case ARRAY_APA:
                            // Convert the BOOL/INT/APA to a RAT
                            mpq_set_sx (&aplRatTmp, GetNextInteger (lpMemRht, aplTypeRht, uRow * uNumColsRht + uCol), 1);

                            break;

                        case ARRAY_FLOAT:
                            // Convert the FLOAT to a RAT
                            mpq_set_d  (&aplRatTmp, ((LPAPLFLOAT) lpMemRht)[uRow * uNumColsRht + uCol]);

                            break;

                        case ARRAY_RAT:
                            // Copy the RAT
                            mpq_set    (&aplRatTmp, &((LPAPLRAT ) lpMemRht)[uRow * uNumColsRht + uCol]);

                            break;

                        case ARRAY_VFP:
                            // Convert the VFP to a RAT
                            mpq_set_fr (&aplRatTmp, &((LPAPLVFP ) lpMemRht)[uRow * uNumColsRht + uCol]);

                            break;

                        case ARRAY_CHAR:
                        case ARRAY_HETERO:
                        case ARRAY_NESTED:
                        defstop
                            break;
                    } // End SWITCH

                    // Copy to the matrix entry
                    mpq_init_set (&lpMemTmpRht[uRow * uNumColsRht + uCol], &aplRatTmp);
                } // End FOR/FOR

                // Solve the equation
                bRet = GaussJordan (lpMemTmpRht, uNumRowsRht, uNumColsRht, lpMemTmpLft, uNumColsLft, lpbCtrlBreak);

                // If it succeeded, ...
                if (bRet)
                {
                    // Loop through the result rows and cols
                    for (uRow = 0; uRow < uNumRowsRes; uRow++)
                    for (uCol = 0; uCol < uNumColsRes; uCol++)
                    // Split cases based upon the result storage type
                    switch (aplTypeRes)
                    {
                        case ARRAY_RAT:
                            // Initialize and copy the inverted elements to the result
                            mpq_init_set   (&((LPAPLRAT) lpMemRes)[uRow * uNumColsRes + uCol],
                                            &lpMemTmpRht          [uRow * uNumColsRes + uCol]);
                            break;

                        case ARRAY_VFP:
                            // Initialize and convert the RAT to a VFP
                            mpfr_init_set_q (&((LPAPLVFP) lpMemRes)[uRow * uNumColsRes + uCol],
                                             &lpMemTmpRht          [uRow * uNumColsRes + uCol],
                                             MPFR_RNDN);
                            break;

                        defstop
                            break;
                    } // End FOR/FOR/SWITCH
                } else
                    goto DOMAIN_EXIT;
                break;

            defstop
                break;
        } // End SWITCH
    } // End IF/ELSE
YYALLOC_EXIT:
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    if (IsScalar (aplRankRes))
    {
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_FLOAT;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkFloat    = aplFloatRes;
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } else
    {
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } // End IF/ELSE

    goto NORMAL_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
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
    // Mark as in error
    bRet = FALSE;
NORMAL_EXIT:
    if (lpGslVectorB)
    {
        // We no longer need this storage and ptr
        gsl_vector_free (lpGslVectorB); lpGslVectorB = NULL;
    } // End IF

    if (lpGslVectorX)
    {
        // We no longer need this storage and ptr
        gsl_vector_free (lpGslVectorX); lpGslVectorX = NULL;
    } // End IF

    if (lpGslVectorW)
    {
        // We no longer need this storage and ptr
        gsl_vector_free (lpGslVectorW); lpGslVectorW = NULL;
    } // End IF

    if (lpGslVectorS)
    {
        // We no longer need this storage and ptr
        gsl_vector_free (lpGslVectorS); lpGslVectorS = NULL;
    } // End IF

    if (lpGslMatrixV)
    {
        // We no longer need this storage and ptr
        gsl_matrix_free (lpGslMatrixV); lpGslMatrixV = NULL;
    } // End IF

    if (lpGslMatrixU)
    {
        // We no longer need this storage and ptr
        gsl_matrix_free (lpGslMatrixU); lpGslMatrixU = NULL;
    } // End IF

    if (hGlbRes)
    {
        if (lpMemRes)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
        } // End IF

        if (!bRet)
        {
            // We no longer need this storage
            FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
        } // End IF
    } // End IF

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

    if (hGlbTmpLft)
    {
        if (lpMemTmpLft)
        {
            // Loop through the rows and cols
            for (uRow = 0; uRow < uNumRowsLft; uRow++)
            for (uCol = 0; uCol < uNumColsLft; uCol++)
                Myq_clear (&lpMemTmpLft[uRow * uNumColsLft + uCol]);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbTmpLft); lpMemTmpLft = NULL;
        } // End IF

        // We no longer need this storage
        MyGlobalFree (hGlbTmpLft); hGlbTmpLft = NULL;
    } // End IF

    if (hGlbTmpRht)
    {
        if (lpMemTmpRht)
        {
            // Loop through the rows and cols
            for (uRow = 0; uRow < uNumRowsRht; uRow++)
            for (uCol = 0; uCol < uNumColsRht; uCol++)
                Myq_clear (&lpMemTmpRht[uRow * uNumColsRht + uCol]);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbTmpRht); lpMemTmpRht = NULL;
        } // End IF

        // We no longer need this storage
        MyGlobalFree (hGlbTmpRht); hGlbTmpRht = NULL;
    } // End IF

    // We no longer need this storage
    Myq_clear (&aplRatTmp);

    return lpYYRes;
} // End PrimFnDydDomino_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $GaussJordan
//
//  Perform Gauss-Jordan elimination on a square rational matrix
//   or between left and right matrices.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- GaussJordan"
#else
#define APPEND_NAME
#endif

UBOOL GaussJordan
    (LPAPLRAT lpMemRes,             // Ptr to right arg/result as APLRAT matrix
     APLDIM   uNumRows,             // # rows in the left/right arg/result
     APLDIM   uNumColsRht,          // # cols ...
     LPAPLRAT lpMemLft,             // Ptr to left arg as APLRAT matrix (may be NULL)
     APLDIM   uNumColsLft,          // # cols in the left arg
     LPUBOOL  lpbCtrlBreak)         // Ptr to Ctrl-Break flag

{
    UBOOL      bRet = FALSE;        // TRUE iff the result is valid
    APLDIM     uRow,                // Loop counter
               uCol,                // ...
               uCol2,               // ...
               uInd,                // ...
               uRowInd,             // ...
               uNumInds = 0;        // # indices
    HGLOBAL    hGlbAux = NULL;      // Global memory handle for temp auxiliary matrix
    HGLOBAL    hGlbInd = NULL;      // Global memory handle for temp index vector
    LPAPLRAT   lpMemAux = NULL;     // Ptr to auxiliary matrix
    LPAPLDIM   lpMemInd = NULL;     // Ptr to index vector global memory data
    APLRAT     mpqTmp = {0},        // Temporary
               mpqCof = {0};        // ...

    // Initialize the temps
    mpq_init (&mpqTmp);
    mpq_init (&mpqCof);

    if (lpMemLft EQ NULL)
        uNumColsLft = uNumColsRht;

    // Allocate temp storage for the auxiliary matrix
    hGlbAux = DbgGlobalAlloc (GHND, (APLU3264) (uNumRows * uNumColsLft * sizeof (APLRAT)));
    if (!hGlbAux)
        goto ERROR_EXIT;

    // Lock the memory to get a ptr to it
    lpMemAux = MyGlobalLock (hGlbAux);

    // Loop through the rows and cols
    for (uRow = 0; uRow < uNumRows; uRow++)
    for (uCol = 0; uCol < uNumColsLft; uCol++)
    if (lpMemLft EQ NULL)
        // Populate the auxiliary matrix as an identity matrix
        mpq_init_set_ui (&lpMemAux[uRow * uNumColsLft + uCol], uRow EQ uCol, 1);
    else
        // Copy the left arg to the auxiliary matrix
        mpq_init_set    (&lpMemAux[uRow * uNumColsLft + uCol], &lpMemLft[uRow * uNumColsLft + uCol]);

    // Allocate temp storage for the index vector
    hGlbInd = DbgGlobalAlloc (GHND, (APLU3264) (uNumRows * sizeof (APLDIM)));
    if (!hGlbInd)
        goto ERROR_EXIT;

    // Lock the memory to get a ptr to it
    lpMemInd = MyGlobalLock (hGlbInd);

    // Populate the index vector with {iota}uNumRows
    for (uRow = 0; uRow < uNumRows; uRow++)
        lpMemInd[uRow] = uRow;

    // Loop through the cols of lpMemRes
    for (uCol = 0; uCol < uNumColsRht; uCol++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Loop through the rows of lpMemRes[uNumInds{drop}lpMemInd;]
        //   looking for the next row with a non-zero coefficient in col uCol
        for (uRowInd = uNumInds; uRowInd < uNumRows; uRowInd++)
        if (mpz_cmp_ui (mpq_numref (&lpMemRes[lpMemInd[uRowInd] * uNumColsRht + uCol]), 0) NE 0)
            break;
        if (uRowInd EQ uNumRows)
            goto ERROR_EXIT;

        //  Copy the actual row #
        uRow = lpMemInd[uRowInd];

        // Get the coefficient to normalize to 1
        mpq_set (&mpqTmp, &lpMemRes[uRow * uNumColsRht + uCol]);

        // Normalize row uRow so that the coefficient in lpMemRes[uRow;uCol] is 1
        // Run through lpMemRes
        for (uCol2 = uCol + 1; uCol2 < uNumColsRht; uCol2++)
            mpq_div (&lpMemRes[uRow * uNumColsRht + uCol2],
                     &lpMemRes[uRow * uNumColsRht + uCol2],
                     &mpqTmp);
        // Run through lpMemAux
        for (uCol2 = 0; uCol2 < uNumColsLft; uCol2++)
            mpq_div (&lpMemAux[uRow * uNumColsLft + uCol2],
                     &lpMemAux[uRow * uNumColsLft + uCol2],
                     &mpqTmp);
        // Subtract row uRow from all other rows so that the entry in col uCol is 0
        for (uInd = 0; uInd < uNumColsRht; uInd++)
        if (uInd NE uRow)
        {
            // Check for Ctrl-Break
            if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

            // Get the coefficient to zero
            mpq_set (&mpqCof, &lpMemRes[uInd * uNumColsRht + uCol]);

            // Run through lpMemRes
            for (uCol2 = uCol; uCol2 < uNumColsRht; uCol2++)
            {
                mpq_mul (&mpqTmp,
                         &lpMemRes[uRow * uNumColsRht + uCol2],
                         &mpqCof);
                mpq_sub (&lpMemRes[uInd * uNumColsRht + uCol2],
                         &lpMemRes[uInd * uNumColsRht + uCol2],
                         &mpqTmp);
            } // End FOR

            // Run through lpMemAux
            for (uCol2 = 0; uCol2 < uNumColsLft; uCol2++)
            {
                mpq_mul (&mpqTmp,
                         &lpMemAux[uRow * uNumColsLft + uCol2],
                         &mpqCof);
                mpq_sub (&lpMemAux[uInd * uNumColsLft + uCol2],
                         &lpMemAux[uInd * uNumColsLft + uCol2],
                         &mpqTmp);
            } // End FOR
        } // End FOR/IF

        // Swap with the next available entry
        lpMemInd[uRowInd] = lpMemInd[uNumInds];

        // Mark this row as used
        lpMemInd[uNumInds++] = uRow;
    } // End FOR

    // Copy and reorder the rows of lpMemAux to lpMemRes[lpMemInd;]
    // Note we use uNumColsLft instead of uNumColsRht when indexing lpMemRes
    //   because the result is stored as a submatrix inside lpMemRes in
    //   case uNumColsLft < uNumColsRht.
    for (uRow = 0; uRow < uNumRows; uRow++)
    for (uCol = 0; uCol < uNumColsLft; uCol++)
        mpq_set (&lpMemRes[         uRow  * uNumColsLft + uCol],
                 &lpMemAux[lpMemInd[uRow] * uNumColsLft + uCol]);
    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    // If we allocated memory for the index vector, ...
    if (hGlbInd)
    {
        // If it's still locked, ...
        if (lpMemInd)
        {
            // Loop through the items, freeing each RAT
            for (uRow = 0; uRow < uNumRows; uRow++)
            for (uCol = 0; uCol < uNumColsLft; uCol++)
                Myq_clear (&lpMemAux[uRow * uNumColsLft + uCol]);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbInd); lpMemInd = NULL;
        } // End IF

        // We no longer need this storage
        MyGlobalFree (hGlbInd); hGlbInd = NULL;
    } // End IF

    // If we allocated memory for the index vector, ...
    if (hGlbAux)
    {
        // If it's still locked, ...
        if (lpMemAux)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbAux); lpMemAux = NULL;
        } // End IF

        // We no longer need this storage
        MyGlobalFree (hGlbAux); hGlbAux = NULL;
    } // End IF

    // We no longer need this storage
    Myq_clear (&mpqCof);
    Myq_clear (&mpqTmp);

    return bRet;
} // End GaussJordan
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_domino.c
//***************************************************************************
