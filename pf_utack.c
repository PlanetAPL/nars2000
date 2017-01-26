//***************************************************************************
//  NARS2000 -- Primitive Function -- UpTack
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
//  $PrimFnUpTack_EM_YY
//
//  Primitive function for monadic and dyadic UpTack (ERROR and "decode/base value")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnUpTack_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnUpTack_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_UPTACK);

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
        return PrimFnMonUpTack_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydUpTack_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimFnUpTack_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnUpTack_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic UpTack
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnUpTack_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnUpTack_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnUpTack_EM_YY,     // Ptr to primitive function routine
                                    lptkLftArg,             // Ptr to left arg token
                                    lptkFunc,               // Ptr to function token
                                    lptkRhtArg,             // Ptr to right arg token
                                    lptkAxis);              // Ptr to axis token (may be NULL)
} // End PrimProtoFnUpTack_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonUpTack_EM_YY
//
//  Primitive function for monadic UpTack (ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonUpTack_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonUpTack_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End PrimFnMonUpTack_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydUpTack_EM_YY
//
//  Primitive function for dyadic UpTack ("decode/base value")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydUpTack_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydUpTack_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE      aplTypeLft,       // Left arg storage type
                  aplTypeRht,       // Right ...
                  aplTypeRes;       // Result   ...
    APLNELM       aplNELMLft,       // Left arg NELM
                  aplNELMRht,       // Right ...
                  aplNELMRes;       // Result   ...
    APLRANK       aplRankLft,       // Left arg rank
                  aplRankRht,       // Right ...
                  aplRankRes;       // Result   ...
    APLDIM        aplColsLft,       // Left arg last dim
                  aplRestLft,       // Left arg product of remaining dims
                  aplFrstRht,       // Right arg 1st dim
                  aplRestRht,       // Right arg product of remaining dims
                  aplInnrMax;       // Larger of inner dimensions
    APLLONGEST    aplLongestLft,    // Left arg immediate value
                  aplLongestRht;    // Right ...
    HGLOBAL       hGlbLft = NULL,   // Left arg global memory handle
                  hGlbRht = NULL,   // Right ...
                  hGlbRes = NULL,   // Result   ...
                  lpSymGlbLft,      // Ptr to left arg global numeric
                  lpSymGlbRht;      // ...    right ...
    LPVOID        lpMemLft = NULL,  // Ptr to left arg global memory
                  lpMemRht = NULL,  // Ptr to right ...
                  lpMemRes = NULL;  // Ptr to result   ...
    LPAPLDIM      lpMemDimLft,      // Ptr to left arg dimensions
                  lpMemDimRht,      // Ptr to right ...
                  lpMemDimRes;      // Ptr to result   ...
    APLUINT       ByteRes,          // # bytes in the result
                  uRes,             // Loop counter
                  uOutLft,          // Loop counter
                  uOutRht,          // Loop counter
                  uDimCopy;         // # dimensions to copy
    APLINT        iInnMax;          // Loop counter
    LPPL_YYSTYPE  lpYYRes = NULL;   // Ptr to the result
    LPPLLOCALVARS lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;     // Ptr to Ctrl-Break flag
    APLRAT        aplRatLft = {0},  // Left arg as a RAT
                  aplRatRht = {0},  // Right ...
                  aplRatAcc = {0},  // Accum ...
                  aplRatVal = {0};  // Value ...
    APLVFP        aplVfpLft = {0},  // Left arg as a VFP
                  aplVfpRht = {0},  // Right ...
                  aplVfpAcc = {0},  // Accum ...
                  aplVfpVal = {0};  // Value ...

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the attributes (Type,NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, &aplColsLft);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Get left & right arg global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Calculate length of right arg first dimension
    if (hGlbRht && !IsScalar (aplRankRht))
        aplFrstRht = *VarArrayBaseToDim (lpMemRht);
    else
        aplFrstRht = 1;

    // Check for LENGTH ERROR
    if (aplColsLft NE aplFrstRht
     && !IsUnitDim (aplColsLft)
     && !IsUnitDim (aplFrstRht))
        goto LENGTH_EXIT;

    // Check for LEFT DOMAIN ERROR
    if (!IsSimpleGlbNum (aplTypeLft))
        goto LEFT_DOMAIN_EXIT;

    // If the left arg is not numeric, ...
    if (!IsNumeric (aplTypeLft))
    {
        // If it's empty and simple, ...
        if (IsEmpty (aplNELMLft)
         && IsSimple (aplTypeLft))
            // Convert to Boolean
            aplTypeLft = ARRAY_BOOL;
        else
            goto LEFT_DOMAIN_EXIT;
    } // End IF

    // Check for RIGHT DOMAIN ERROR
    if (!IsSimpleGlbNum (aplTypeRht))
        goto RIGHT_DOMAIN_EXIT;

    // If the right arg is not numeric, ...
    if (!IsNumeric (aplTypeRht))
    {
        // If it's empty and simple, ...
        if (IsEmpty (aplNELMRht)
         && IsSimple (aplTypeRht))
            // Convert to Boolean
            aplTypeRht = ARRAY_BOOL;
        else
            goto RIGHT_DOMAIN_EXIT;
    } // End IF

    // If both are scalars, and one is not numeric, ...
    if (IsScalar (aplRankLft)
     && IsScalar (aplRankRht))
    {
        if (!IsNumeric (aplTypeLft))
            goto LEFT_DOMAIN_EXIT;
        if (!IsNumeric (aplTypeRht))
            goto RIGHT_DOMAIN_EXIT;
    } // End IF

    // Re-calculate the inner dimensions based upon
    //   scalar/one-element vector extension for empty args
    if ((IsScalar (aplRankLft)
      || IsVectorSing (aplNELMLft, aplRankLft))
     && IsEmpty (aplFrstRht))
        aplColsLft = aplFrstRht;
    else
    if ((IsScalar (aplRankRht)
      || IsVectorSing (aplNELMRht, aplRankRht))
     && IsEmpty (aplColsLft))
        aplFrstRht = aplColsLft;

    // Calc larger of inner dimensions (in case of scalar extension)
    aplInnrMax = max (aplColsLft, aplFrstRht);

    // Calc product of the remaining dimensions in left arg
    if (aplColsLft)
        aplRestLft = aplNELMLft / aplColsLft;
    else
    if (hGlbLft)
        for (aplRestLft = 1, uOutLft = 0; uOutLft < (aplRankLft - 1); uOutLft++)
            aplRestLft *= (VarArrayBaseToDim (lpMemLft))[uOutLft];
    else
        aplRestLft = 1;

    // Calc product of the remaining dimensions in right arg
    if (aplFrstRht)
        aplRestRht = aplNELMRht / aplFrstRht;
    else
    if (hGlbRht)
        for (aplRestRht = 1, uOutRht = 1; uOutRht < aplRankRht; uOutRht++)
            aplRestRht *= (VarArrayBaseToDim (lpMemRht))[uOutRht];
    else
        aplRestRht = 1;

    // Calc result rank
    aplRankRes = max (aplRankLft, 1) + max (aplRankRht, 1) - 2;

    // Calc result NELM
    aplNELMRes = aplRestLft * aplRestRht;

    // Calc result Type
    aplTypeRes = aTypePromote[aplTypeLft][aplTypeRht];

    // Check for empty and non-global numeric result
    if (IsEmpty (aplNELMRes)
     && !IsGlbNum (aplTypeRes))
        // Calc result Type
        aplTypeRes = ARRAY_BOOL;
    else
    {
        // Promote Boolean to integer
        if (IsSimpleBool (aplTypeRes))
            aplTypeRes = ARRAY_INT;
    } // End IF/ELSE

    // If the result is global numeric, ...
    if (IsGlbNum (aplTypeRes))
    {
        // Initialize the temps
        mpq_init (&aplRatLft);
        mpq_init (&aplRatRht);
        mpq_init (&aplRatAcc);
        mpq_init (&aplRatVal);

        mpfr_init0 (&aplVfpLft);
        mpfr_init0 (&aplVfpRht);
        mpfr_init0 (&aplVfpAcc);
        mpfr_init0 (&aplVfpVal);
    } // End IF
RESTART_EXCEPTION:
    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    //***************************************************************
    // Now we can allocate the storage for the result
    //***************************************************************
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock000 (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Skip over the header to the dimensions
    if (hGlbLft)
        lpMemDimLft = VarArrayBaseToDim (lpMemLft);
    if (hGlbRht)
        lpMemDimRht = VarArrayBaseToDim (lpMemRht);
    lpMemDimRes = VarArrayBaseToDim (lpMemRes);

    // Fill in the result's dimension
    //   by copying the left arg dimensions (except for the last)
    //   and then the right arg dimensions  (except for the first)

    // If the left arg is an array, ...
    if (hGlbLft)
    {
        // Calc # dimensions to copy
        uDimCopy = max (aplRankLft, 1) - 1;

        // Copy the dimensions
        CopyMemory (lpMemDimRes, lpMemDimLft, (APLU3264) uDimCopy * sizeof (APLDIM));

        // Skip over the copied dimensions
        lpMemDimRes += uDimCopy;
    } // End IF

    // If the right arg is an array, ...
    if (hGlbRht)
    {
        // Calc # dimensions to copy
        uDimCopy = max (aplRankRht, 1) - 1;

        // Copy the dimensions
        CopyMemory (lpMemDimRes, &lpMemDimRht[1], (APLU3264) uDimCopy * sizeof (APLDIM));

        // Skip over the copied dimensions
        lpMemDimRes += uDimCopy;
    } // End IF

    // Check for empty result
    if (IsEmpty (aplNELMRes))
        goto YYALLOC_EXIT;

    // Skip over the dimensions to the data
    lpMemRes = lpMemDimRes;

    // The left arg is treated as a two-dimensional array of shape
    //   aplRestLft aplColsLft

    // The right arg is treated as a two-dimensional array of shape
    //   aplFrstRht aplRestRht

    // Trundle through the left & right arg remaining dimensions
    for (uOutLft = 0; uOutLft < aplRestLft; uOutLft++)
    for (uOutRht = 0; uOutRht < aplRestRht; uOutRht++)
    {
        APLINT   aplIntAcc,
                 InnValInt;
        APLFLOAT aplFloatAcc,
                 InnValFlt;
        APLFLOAT aplFloatLft,       // Temporary floats
                 aplFloatRht;       // ...

        // Calc result index
        uRes = 1 * uOutRht + aplRestRht * uOutLft;

        // Split cases based upon the result storage type
        switch (aplTypeRes)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
                // Initialize accumulator and weighting value
                aplIntAcc = 0;
                InnValInt = 1;

                break;

            case ARRAY_FLOAT:
                // Initialize accumulator and weighting value
                aplFloatAcc = 0.0;
                InnValFlt   = 1.0;

                break;

            case ARRAY_RAT:
                // Initialize accumulator and weighting value
                mpq_set_ui (&aplRatAcc, 0, 1);
                mpq_set_ui (&aplRatVal, 1, 1);

                break;

            case ARRAY_VFP:
                // Initialize accumulator and weighting value
                mpfr_set_ui (&aplVfpAcc, 0, MPFR_RNDN);
                mpfr_set_ui (&aplVfpVal, 1, MPFR_RNDN);

                break;

            case ARRAY_CHAR:            // Can't happen w/UpTack
            case ARRAY_APA:             // ...
            case ARRAY_HETERO:          // ...
            case ARRAY_NESTED:          // ...
            defstop
                break;
        } // End SWITCH

        // Trundle through the inner dimensions, back to front
        for (iInnMax = aplInnrMax - 1; iInnMax >= 0; iInnMax--)
        {
            APLUINT uInnLft,            // Index into left arg
                    uInnRht;            // ...        right ...

            // Check for Ctrl-Break
            if (CheckCtrlBreak (*lpbCtrlBreak))
                goto ERROR_EXIT;

            // Calc left inner index, taking into account scalar extension
            if (IsUnitDim (aplColsLft))
                uInnLft = 1 * 0       + aplColsLft * uOutLft;
            else
                uInnLft = 1 * iInnMax + aplColsLft * uOutLft;

            // Calc right inner index, taking into account scalar extension
            if (IsUnitDim (aplFrstRht))
                uInnRht = 1 * uOutRht + aplRestRht * 0      ;
            else
                uInnRht = 1 * uOutRht + aplRestRht * iInnMax;

            // Split cases based upon the result storage type
            switch (aplTypeRes)
            {
                case ARRAY_BOOL:
                case ARRAY_INT:
                    // If the right arg is an array, ...
                    if (hGlbRht)
                    {
                        // If the right arg is non-empty, ...
                        if (aplNELMRht)
                            // Get the next right arg value
                            GetNextValueGlb (hGlbRht,           // The global memory handle
                                             uInnRht,           // Index into item
                                             NULL,              // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                                            &aplLongestRht,     // Ptr to result immediate value (may be NULL)
                                             NULL);             // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                        else
                            aplLongestRht = 0;
                    } // End IF

                    __try
                    {
                        // Add into accumulator
                        aplIntAcc = iadd64_RE (aplIntAcc, imul64_RE (InnValInt, aplLongestRht));

                        // Get the next left arg value
                        if (hGlbLft)
                        {
                            // If the left arg is non-empty, ...
                            if (aplNELMLft)
                                GetNextValueGlb (hGlbLft,       // The global memory handle
                                                 uInnLft,       // Index into item
                                                 NULL,          // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                                                &aplLongestLft, // Ptr to result immediate value (may be NULL)
                                                 NULL);         // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                            else
                                aplLongestLft = 0;
                        } // End IF

                        // Multiply into the weighting value
                        InnValInt = imul64_RE (InnValInt, aplLongestLft);
                    } __except (CheckException (GetExceptionInformation (), L"PrimFnDydUpTack_EM_YY"))
                    {
                        switch (MyGetExceptionCode ())
                        {
                            case EXCEPTION_RESULT_FLOAT:
                                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                                if (!IsSimpleFlt (aplTypeRes))
                                {
                                    aplTypeRes = ARRAY_FLOAT;

                                    dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L": %2d (%S#%d)", MyGetExceptionCode (), FNLN);

                                    // We no longer need these ptrs
                                    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

                                    // We no longer need this storage
                                    FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;

                                    goto RESTART_EXCEPTION;
                                } // End IF

                                // Fall through to never-never-land

                            default:
                                // Display message for unhandled exception
                                DisplayException ();

                                break;
                        } // End SWITCH
                    } // End __try/__except

                    break;

                case ARRAY_FLOAT:
                    // If the right arg is an array, ...
                    if (hGlbRht)
                    {
                        // If the right arg is non-empty, ...
                        if (aplNELMRht)
                            // Get the next right arg value
                            GetNextValueGlb (hGlbRht,           // The global memory handle
                                             uInnRht,           // Index into item
                                             NULL,              // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                                            &aplLongestRht,     // Ptr to result immediate value (may be NULL)
                                             NULL);             // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                        else
                            aplLongestRht = 0;
                    } // End IF

                    // If the right arg is int, convert it to float
                    if (IsSimpleInt (aplTypeRht))
                        aplFloatRht = (APLFLOAT) (APLINT) aplLongestRht;
                    else
                        aplFloatRht = *(LPAPLFLOAT) &aplLongestRht;

                    // Add into accumulator
                    aplFloatAcc += InnValFlt * aplFloatRht;

                    // If the left arg is an array, ...
                    if (hGlbLft)
                    {
                        // If the left arg is non-empty, ...
                        if (aplNELMLft)
                            // Get the next left arg value
                            GetNextValueGlb (hGlbLft,           // The global memory handle
                                             uInnLft,           // Index into item
                                             NULL,              // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                                            &aplLongestLft,     // Ptr to result immediate value (may be NULL)
                                             NULL);             // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                        else
                            aplLongestLft = 0;
                    } // End IF

                    // If the left arg is int, convert it to float
                    if (IsSimpleInt (aplTypeLft))
                        aplFloatLft = (APLFLOAT) (APLINT) aplLongestLft;
                    else
                        aplFloatLft = *(LPAPLFLOAT) &aplLongestLft;

                    // Multiply into the weighting value
                    InnValFlt *= aplFloatLft;

                    break;

                case ARRAY_RAT:
                    // If the right arg is an array, ...
                    if (hGlbRht)
                    {
                        // If the right arg is non-empty, ...
                        if (aplNELMRht)
                            // Get the next right arg value
                            GetNextValueGlb (hGlbRht,           // The global memory handle
                                             uInnRht,           // Index into item
                                            &lpSymGlbRht,       // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                                            &aplLongestRht,     // Ptr to result immediate value (may be NULL)
                                             NULL);             // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                        else
                            aplLongestRht = 0;
                    } // End IF

                    // Split cases based upon the right arg storage type
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:
                        case ARRAY_INT:
                        case ARRAY_APA:
                            mpq_set_sx (&aplRatRht, aplLongestRht, 1);

                            break;

                        case ARRAY_RAT:
                            mpq_set    (&aplRatRht, (LPAPLRAT) lpSymGlbRht);

                            break;

                        case ARRAY_FLOAT:       // Can't happen w/Res = RAT
                        case ARRAY_CHAR:        // ...
                        case ARRAY_HETERO:      // ...
                        case ARRAY_NESTED:      // ...
                        case ARRAY_VFP:         // ...
                        defstop
                            break;
                    } // End SWITCH

                    // Add into accumulator
////////////////////aplFloatAcc += InnValFlt * aplFloatRht;
                    mpq_mul (&aplRatRht, &aplRatVal, &aplRatRht);
                    mpq_add (&aplRatAcc, &aplRatAcc, &aplRatRht);

                    // If the left arg is an array, ...
                    if (hGlbLft)
                    {
                        // If the left arg is non-empty, ...
                        if (aplNELMLft)
                            // Get the next left arg value
                            GetNextValueGlb (hGlbLft,       // The global memory handle
                                             uInnLft,       // Index into item
                                            &lpSymGlbLft,   // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                                            &aplLongestLft, // Ptr to result immediate value (may be NULL)
                                             NULL);         // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                        else
                            aplLongestLft = 0;
                    } // End IF

                    // Split cases based upon the left arg storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:
                        case ARRAY_INT:
                        case ARRAY_APA:
                            mpq_set_sx (&aplRatLft, aplLongestLft, 1);

                            break;

                        case ARRAY_RAT:
                            mpq_set    (&aplRatLft, (LPAPLRAT) lpSymGlbLft);

                            break;

                        case ARRAY_FLOAT:       // Can't happen w/Res = RAT
                        case ARRAY_CHAR:        // ...
                        case ARRAY_HETERO:      // ...
                        case ARRAY_NESTED:      // ...
                        case ARRAY_VFP:         // ...
                        defstop
                            break;
                    } // End SWITCH

                    // Multiply into the weighting value
////////////////////InnValFlt *= aplFloatLft;
                    mpq_mul (&aplRatVal, &aplRatVal, &aplRatLft);

                    break;

                case ARRAY_VFP:
                    // If the right arg is an array, ...
                    if (hGlbRht)
                    {
                        // If the right arg is non-empty, ...
                        if (aplNELMRht)
                            // Get the next right arg value
                            GetNextValueGlb (hGlbRht,           // The global memory handle
                                             uInnRht,           // Index into item
                                            &lpSymGlbRht,       // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                                            &aplLongestRht,     // Ptr to result immediate value (may be NULL)
                                             NULL);             // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                        else
                            aplLongestRht = 0;
                    } // End IF

                    // Split cases based upon the right arg storage type
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:
                        case ARRAY_INT:
                        case ARRAY_APA:
                            mpfr_set_sx (&aplVfpRht, aplLongestRht, MPFR_RNDN);

                            break;

                        case ARRAY_FLOAT:
                            mpfr_set_d  (&aplVfpRht, *(LPAPLFLOAT) &aplLongestRht, MPFR_RNDN);

                            break;

                        case ARRAY_RAT:
                            mpfr_set_q  (&aplVfpRht, (LPAPLRAT) lpSymGlbRht, MPFR_RNDN);

                            break;

                        case ARRAY_VFP:
                            mpfr_copy   (&aplVfpRht, (LPAPLVFP) lpSymGlbRht);

                            break;

                        case ARRAY_CHAR:        // Can't happen w/Res=VFP
                        case ARRAY_HETERO:      // ...
                        case ARRAY_NESTED:      // ...
                        defstop
                            break;
                    } // End SWITCH

                    // Add into accumulator
////////////////////aplFloatAcc += InnValFlt * aplFloatRht;
                    mpfr_mul (&aplVfpRht, &aplVfpVal, &aplVfpRht, MPFR_RNDN);
                    mpfr_add (&aplVfpAcc, &aplVfpAcc, &aplVfpRht, MPFR_RNDN);

                    // If the left arg is an array, ...
                    if (hGlbLft)
                    {
                        // If the left arg is non-empty, ...
                        if (aplNELMLft)
                            // Get the next left arg value
                            GetNextValueGlb (hGlbLft,       // The global memory handle
                                             uInnLft,       // Index into item
                                            &lpSymGlbLft,   // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                                            &aplLongestLft, // Ptr to result immediate value (may be NULL)
                                             NULL);         // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                        else
                            aplLongestLft = 0;
                    } // End IF

                    // Split cases based upon the left arg storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:
                        case ARRAY_INT:
                        case ARRAY_APA:
                            mpfr_set_sx (&aplVfpLft, aplLongestLft, MPFR_RNDN);

                            break;

                        case ARRAY_FLOAT:
                            mpfr_set_d  (&aplVfpLft, *(LPAPLFLOAT) &aplLongestLft, MPFR_RNDN);

                            break;

                        case ARRAY_RAT:
                            mpfr_set_q  (&aplVfpLft, (LPAPLRAT) lpSymGlbLft, MPFR_RNDN);

                            break;

                        case ARRAY_VFP:
                            mpfr_copy   (&aplVfpLft, (LPAPLVFP) lpSymGlbLft);

                            break;

                        case ARRAY_CHAR:        // Can't happen w/Res = VFP
                        case ARRAY_HETERO:      // ...
                        case ARRAY_NESTED:      // ...
                        defstop
                            break;
                    } // End SWITCH

                    // Multiply into the weighting value
////////////////////InnValFlt *= aplFloatLft;
                    mpfr_mul (&aplVfpVal, &aplVfpVal, &aplVfpLft, MPFR_RNDN);

                    break;

                case ARRAY_APA:
                case ARRAY_CHAR:
                case ARRAY_HETERO:
                case ARRAY_NESTED:
                defstop
                    break;
            } // End SWITCH
        } // End FOR

        // Split cases based upon the result storage type
        switch (aplTypeRes)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
                // Save in the result
                ((LPAPLINT)   lpMemRes)[uRes] = aplIntAcc;

                break;

            case ARRAY_FLOAT:
                // Save in the result
                ((LPAPLFLOAT) lpMemRes)[uRes] = aplFloatAcc;

                break;

            case ARRAY_RAT:
                // Save in the result
                mpq_init_set (&((LPAPLRAT) lpMemRes)[uRes], &aplRatAcc);

                break;

            case ARRAY_VFP:
                // Save in the result
                mpfr_init_copy (&((LPAPLVFP) lpMemRes)[uRes], &aplVfpAcc);

                break;

            case ARRAY_CHAR:            // Can't happen w/UpTack
            case ARRAY_APA:             // ...
            case ARRAY_HETERO:          // ...
            case ARRAY_NESTED:          // ...
            defstop
                break;
        } // End SWITCH
    } // End FOR/FOR
YYALLOC_EXIT:
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

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkFunc);
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
    // If the result is global numeric, ...
    if (IsGlbNum (aplTypeRes))
    {
        // We no longer need this storage
        Myf_clear (&aplVfpVal);
        Myf_clear (&aplVfpAcc);
        Myf_clear (&aplVfpRht);
        Myf_clear (&aplVfpLft);

        Myq_clear (&aplRatVal);
        Myq_clear (&aplRatAcc);
        Myq_clear (&aplRatRht);
        Myq_clear (&aplRatLft);
    } // End IF

    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
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

    return lpYYRes;
} // End PrimFnDydUpTack_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_utack.c
//***************************************************************************
