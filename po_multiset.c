//***************************************************************************
//  NARS2000 -- Primitive Operator -- Multiset
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

// Define the following var in order to grade the secondary
//   argument so as to catch duplicate values.
//   It's probably not worth the effort, so this option is
//     not recommended.
////#define GRADE2ND


//***************************************************************************
//  $PrimOpDownShoeStile_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    monadic operator DownShoeStile ("multiset" and "multiset")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDownShoeStile_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpDownShoeStile_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_DOWNSHOESTILE);

    return
      PrimOpDownShoeStileCommon_EM_YY (lptkLftArg,      // Ptr to left arg token (may be NULL if niladic/monadic)
                                       lpYYFcnStrOpr,   // Ptr to operator function strand
                                       lptkRhtArg,      // Ptr to right arg token (may be NULL if niladic)
                                       FALSE);          // TRUE iff prototyping
} // End PrimOpDownShoeStile_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoOpDownShoeStile_EM_YY
//
//  Generate a prototype for the derived functions from
//    monadic operator DownShoeStile ("multiset" and "multiset")
//***************************************************************************

LPPL_YYSTYPE PrimProtoOpDownShoeStile_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxis)             // Ptr to axis token always NULL)

{
    Assert (lptkAxis EQ NULL);

    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    return
      PrimOpDownShoeStileCommon_EM_YY (lptkLftArg,      // Ptr to left arg token
                                       lpYYFcnStrOpr,   // Ptr to operator function strand
                                       lptkRhtArg,      // Ptr to right arg token
                                       TRUE);           // TRUE iff prototyping
} // End PrimProtoOpDownShoeStile_EM_YY


//***************************************************************************
//  $PrimOpDownShoeStileCommon_EM_YY
//
//  Common subroutine for primitive operator for monadic/dyadic derived functions
//    from Multiset ("multiset")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDownShoeStileCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpDownShoeStileCommon_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token (may be NULL if niladic)
     UBOOL        bPrototyping)         // TRUE iff prototyping

{
    APLSTYPE     aplTypeLft,            // Left arg storage type
                 aplTypeRht;            // Right ...
    APLRANK      aplRankLft,            // Left arg rank
                 aplRankRht;            // Right ...
    HGLOBAL      hGlbMFO;               // Magic function/operator global memory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYFcnStrLft,         // Ptr to left operand function strand
                 lpYYRes;               // Ptr to result
    LPTOKEN      lptkAxisOpr,           // Ptr to operator axis token (may be NULL)
                 lptkTmpArg;            // Ptr to temp token
    APLFLOAT     fQuadCT;               // Current value for QuadCT
    UBOOL        bNotEqual = FALSE;     // TRUE iff not NotEqual

    // Check for axis operator
    lptkAxisOpr = CheckAxisOper (lpYYFcnStrOpr);

    //***************************************************************
    // The derived functions from this operator are not sensitive
    //   to the axis operator, so signal a syntax error if present
    //***************************************************************

    if (lptkAxisOpr NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = GetMonLftOper (lpYYFcnStrOpr, lptkAxisOpr);

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken))
        goto LEFT_SYNTAX_EXIT;

    // Check for a multiset-sensitive function
    if (lpYYFcnStrLft->TknCount EQ 1)
    {
        // Get the attributes (Type, NELM, and Rank) of the left & right args
        if (lptkLftArg NE NULL)
            AttrsOfToken (lptkLftArg, &aplTypeLft, NULL, &aplRankLft, NULL);
        else
        {
            aplTypeLft = ARRAY_BOOL;
            aplRankLft = 1;
        } // End IF/ELSE

        if (lptkRhtArg NE NULL)
            AttrsOfToken (lptkRhtArg, &aplTypeRht, NULL, &aplRankRht, NULL);

        // If the function is not {iotaunderbar}, ...
        if (lpYYFcnStrLft->tkToken.tkData.tkChar NE UTF16_IOTAUNDERBAR
         && lptkLftArg NE NULL
         && lptkRhtArg NE NULL)
        {
            // Check for LEFT/RIGHT RANK ERRORs
            if (IsMultiRank (aplRankLft))
                goto LEFT_RANK_EXIT;

            if (IsMultiRank (aplRankRht))
                goto RIGHT_RANK_EXIT;
        } // End IF

        // Get ptr to PerTabData global memory
        lpMemPTD = GetMemPTD ();

        // Get the current value of []CT
        fQuadCT = GetQuadCT ();

        // Split cases based upon the function symbol
        switch (lpYYFcnStrLft->tkToken.tkData.tkChar)
        {
            case UTF16_EPSILON:
                // Ensure called dyadically
                if (lptkLftArg EQ NULL)
                    goto VALENCE_EXIT;

                // If both args are simple non-heteros, ...
                if (IsSimpleNH (aplTypeLft) && IsSimpleNH (aplTypeRht))
                {
                    // Zero QuadCT
                    SetQuadCT (0);

                    lpYYRes =
                      PrimFnDydMEO_EM_YY (lptkLftArg,               // Ptr to left arg token
                                          aplTypeLft,               // Left arg storage type
                               (LPTOKEN) &lpYYFcnStrLft->tkToken,   // Ptr to function token
                                          lptkRhtArg,               // Ptr to right arg token
                                          aplTypeRht,               // Right arg storage type
                                          lptkAxisOpr);             // Ptr to operator axis token (may be NULL)
                    goto NORMAL_EXIT;
                } // End IF

                // Get the magic function/operator global memory handle
                hGlbMFO = lpMemPTD->hGlbMFO[MFOE_MEO];

                break;

            case UTF16_IOTA:
                // Ensure called dyadically
                if (lptkLftArg EQ NULL)
                    goto VALENCE_EXIT;

                // If both args are simple non-heteros, ...
                if (IsSimpleNH (aplTypeLft) && IsSimpleNH (aplTypeRht))
                {
                    // Zero QuadCT
                    SetQuadCT (0);

                    lpYYRes =
                      PrimFnDydMIO_EM_YY (lptkLftArg,               // Ptr to left arg token
                                          aplTypeLft,               // Left arg storage type
                               (LPTOKEN) &lpYYFcnStrLft->tkToken,   // Ptr to function token
                                          lptkRhtArg,               // Ptr to right arg token
                                          aplTypeRht,               // Right arg storage type
                                          lptkAxisOpr);             // Ptr to operator axis token (may be NULL)
                    goto NORMAL_EXIT;
                } // End IF

                // Get the magic function/operator global memory handle
                hGlbMFO = lpMemPTD->hGlbMFO[MFOE_MIO];

                break;

            case UTF16_TILDE:
            case UTF16_TILDE2:
                // Ensure called dyadically
                if (lptkLftArg EQ NULL)
                    goto VALENCE_EXIT;

                // Get the magic function/operator global memory handle
                hGlbMFO = lpMemPTD->hGlbMFO[MFOE_MAD];

                break;

            case UTF16_SECTION:
                // Ensure called dyadically
                if (lptkLftArg EQ NULL)
                    goto VALENCE_EXIT;

                // Get the magic function/operator global memory handle
                hGlbMFO = lpMemPTD->hGlbMFO[MFOE_MSD];

                break;

            case UTF16_DOWNSHOE:
                // Check for monadic DownShoe (Multiset Multiplicities)
                if (lptkLftArg EQ NULL)
                    // Get the magic function/operator global memory handle
                    hGlbMFO = lpMemPTD->hGlbMFO[MFOE_MMUL];
                else
                    // Get the magic function/operator global memory handle
                    hGlbMFO = lpMemPTD->hGlbMFO[MFOE_MU];
                break;

            case UTF16_UPSHOE:
                // Ensure called dyadically
                if (lptkLftArg EQ NULL)
                    goto VALENCE_EXIT;

                // Get the magic function/operator global memory handle
                hGlbMFO = lpMemPTD->hGlbMFO[MFOE_MI];

                break;

            case UTF16_RIGHTSHOE:
                // Ensure called dyadically
                if (lptkLftArg EQ NULL)
                    goto VALENCE_EXIT;

                // Swap the args so we can use the LeftShoe magic function
                lptkTmpArg = lptkLftArg;
                lptkLftArg = lptkRhtArg;
                lptkRhtArg = lptkTmpArg;

                // Fall through to common code

            case UTF16_LEFTSHOE:
                // Ensure called dyadically
                if (lptkLftArg EQ NULL)
                    goto VALENCE_EXIT;

                // Get the magic function/operator global memory handle
                hGlbMFO = lpMemPTD->hGlbMFO[MFOE_MLRS];

                break;

            case UTF16_RIGHTSHOEUNDERBAR:
                // Ensure called dyadically
                if (lptkLftArg EQ NULL)
                    goto VALENCE_EXIT;

                // Swap the args so we can use the LeftShoeUnderbar magic function
                lptkTmpArg = lptkLftArg;
                lptkLftArg = lptkRhtArg;
                lptkRhtArg = lptkTmpArg;

                // Fall through to common code

            case UTF16_LEFTSHOEUNDERBAR:
                // Ensure called dyadically
                if (lptkLftArg EQ NULL)
                    goto VALENCE_EXIT;

                // Get the magic function/operator global memory handle
                hGlbMFO = lpMemPTD->hGlbMFO[MFOE_MLRSU];

                break;

            case UTF16_NOTEQUALUNDERBAR:
                bNotEqual = TRUE;

                // Fall through to Match case

            case UTF16_EQUALUNDERBAR:
                // Ensure called dyadically
                if (lptkLftArg EQ NULL)
                    goto VALENCE_EXIT;

                // If both args are simple non-heteros, ...
                if (IsSimpleNH (aplTypeLft) && IsSimpleNH (aplTypeRht))
                {
                    // Zero QuadCT
                    SetQuadCT (0);

                    lpYYRes =
                      PrimFnDydMM_EM_YY (lptkLftArg,                // Ptr to left arg token
                                         aplTypeLft,                // Left arg storage type
                              (LPTOKEN) &lpYYFcnStrLft->tkToken,    // Ptr to function token
                                         lptkRhtArg,                // Ptr to right arg token
                                         aplTypeRht,                // Right arg storage type
                                         lptkAxisOpr);              // Ptr to operator axis token (may be NULL)
                    // Check for valid result and NotEqual
                    if (lpYYRes && bNotEqual)
                    {
                        Assert (lpYYRes->tkToken.tkFlags.TknType EQ TKT_VARIMMED);

                        // Complement the result
                        lpYYRes->tkToken.tkData.tkBoolean ^= TRUE;
                    } // End IF

                    goto NORMAL_EXIT;
                } // End IF

                // Get the magic function/operator global memory handle
                hGlbMFO = lpMemPTD->hGlbMFO[MFOE_MM];

                break;

            case UTF16_IOTAUNDERBAR:
                // Ensure called dyadically
                if (lptkLftArg EQ NULL)
                    goto VALENCE_EXIT;

                // Get the magic function/operator global memory handle
                hGlbMFO = lpMemPTD->hGlbMFO[MFOE_MDIU];

                break;

            default:
                // Check for user-defined function/operator
                if (IsTknUsrDfn (&lpYYFcnStrLft->tkToken))
                {
                    // If we're also prototyping, ...
                    if (bPrototyping)
                        goto NONCE_EXIT;

                    // Execute the UDFO
                    return
                      ExecFuncStrLine_EM_YY (lptkLftArg,        // Ptr to left arg token (may be NULL if niladic/monadic)
                                             lpYYFcnStrLft,     // Ptr to left operand function strand
                                             lptkRhtArg,        // Ptr to right arg token (may be NULL if niladic)
                                             lptkAxisOpr,       // Ptr to operator axis token (may be NULL)
                                             LINENUM_MS);       // Starting line # type (see LINE_NUMS)
                } else
                    goto NONCE_EXIT;
        } // End SWITCH
    } else
        goto NONCE_EXIT;

    // Zero QuadCT
    SetQuadCT (0);

    //  Use an internal magic function.
    lpYYRes =
      ExecuteMagicFunction_EM_YY (lptkLftArg,           // Ptr to left arg token (may be NULL if niladic/monadic)
                        (LPTOKEN) lpYYFcnStrLft,        // Ptr to left operand function token
                                  NULL,                 // Ptr to function strand
                                  lptkRhtArg,           // Ptr to right arg token (may be NULL if niladic)
                                  lptkAxisOpr,          // Ptr to operator axis token (may be NULL)
                                  hGlbMFO,              // Magic function/operator global memory handle
                                  NULL,                 // Ptr to HSHTAB struc (may be NULL)
                   bPrototyping ? LINENUM_PRO
                                : LINENUM_MS);          // Starting line # type (see LINE_NUMS)
NORMAL_EXIT:
    // Restore original value of QuadCT
    SetQuadCT (fQuadCT);

    return lpYYRes;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxisOpr);
    return NULL;

LEFT_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    return NULL;

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    return NULL;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    return NULL;

VALENCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALENCE_ERROR APPEND_NAME,
                               lptkLftArg);
    return NULL;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    return NULL;
} // End PrimOpDownShoeStileCommon_EM_YY
#undef  APPEND_NAME



//***************************************************************************
//  $PrimFnDydMEO_EM_YY
//
//  Execute Multiset Element Of between simple non-heteros
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydMEO_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydMEO_EM_YY
    (LPTOKEN  lptkLftArg,               // Ptr to left arg token
     APLSTYPE aplTypeLft,               // Left arg storage type
     LPTOKEN  lptkFunc,                 // Ptr to function token
     LPTOKEN  lptkRhtArg,               // Ptr to right arg token
     APLSTYPE aplTypeRht,               // Right arg storage type
     LPTOKEN  lptkAxis)                 // Ptr to axis token (may be NULL)

{
    APLNELM       aplNELMLft,           // Left arg NELM
                  aplNELMRht;           // Right ...
    APLRANK       aplRankLft,           // Left arg rank
                  aplRankRht;           // Right ...
    HGLOBAL       hGlbLft = NULL,       // Left arg global memory handle
                  hGlbRht = NULL,       // Right ...
                  hGlbRes = NULL;       // Result   ...
    LPVOID        lpMemLft = NULL,      // Ptr to left arg global memory
                  lpMemRht = NULL;      // Ptr to right ...
    LPAPLBOOL     lpMemRes = NULL;      // Ptr to result   ...
    TOKEN         tkFunc = {0};         // Grade-up function token
#ifdef GRADE2ND
    HGLOBAL       hGlbGupLft = NULL;    // Left arg grade-up global memory handle
    LPPDIE        lpMemGupLft = NULL;   // Ptr to left arg grade-up global memory
#endif
    HGLOBAL       hGlbGupRht = NULL;    // Right arg grade-up glolbal memory handle
    LPPDIE        lpMemGupRht = NULL;   // Ptr to right arg grade-up global memory
    PDIE          aplIntegerZero = {0}; // A zero in case the right arg is a scalar
    APLUINT       ByteRes;              // # bytes in the result
    APLINT        iLft;                 // Loop counter
    APLLONGEST    aplLongestLft,        // Left arg immediate value
                  aplLongestRht;        // Right ...
    LPPL_YYSTYPE  lpYYRes;              // Ptr to grade-up result
    UBOOL         bQuadIO;              // []IO
    APLFLOAT      fQuadCT;              // []CT
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    LPMEOCOM      lpPrimFnDydMEO_Com;   // Ptr to common routine

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the current value of []IO & []CT
    bQuadIO = GetQuadIO ();
    fQuadCT = GetQuadCT ();

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for LEFT RANK ERROR
    if (IsMultiRank (aplRankLft))
        goto LEFT_RANK_EXIT;

    // Check for RIGHT RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Get left and right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Setup the grade-up function token
    tkFunc.tkFlags.TknType   = TKT_FCNIMMED;
    tkFunc.tkFlags.ImmType   = IMMTYPE_PRIMFCN;
////tkFunc.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
    tkFunc.tkData.tkChar     = UTF16_DELTASTILE;
    tkFunc.tkCharIndex       = lptkFunc->tkCharIndex;
#ifdef GRADE2ND
    // If the left arg is not a scalar, ...
    if (!IsScalar (aplRankLft))
    {
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
        lpMemGupLft = MyGlobalLockVar (hGlbGupLft);

        // Skip over the header and dimensions to the data
        lpMemGupLft = VarArrayDataFmBase (lpMemGupLft);
    } else
        // Handle the scalar case
        lpMemGupLft = &aplIntegerZero;
#endif
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
        lpMemGupRht = MyGlobalLockVar (hGlbGupRht);

        // Skip over the header and dimensions to the data
        lpMemGupRht = VarArrayDataFmBase (lpMemGupRht);
    } else
        // Handle the scalar case
        lpMemGupRht = &aplIntegerZero;

    //***************************************************************
    // Calculate space needed for the result
    //***************************************************************
    ByteRes = CalcArraySize (ARRAY_BOOL, aplNELMLft, 1);

    //***************************************************************
    //  Check for overflow
    //***************************************************************
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
    lpHeader->ArrType    = ARRAY_BOOL;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMLft;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = (LPAPLBOOL) VarArrayBaseToDim (lpMemRes);

    // Fill in the result's dimension
    if (lpMemLft NE NULL)
    {
        // Skip over the header to the dimensions
        lpMemLft = VarArrayBaseToDim (lpMemLft);

        // Copy the left arg dimensions to the result
        CopyMemory (lpMemRes, lpMemLft, (APLU3264) aplRankLft * sizeof (APLDIM));

        // Skip over the dimensions to the data
        lpMemRes = VarArrayDimToData (lpMemRes, 1         );
        lpMemLft = VarArrayDimToData (lpMemLft, aplRankLft);
    } else
    {
        // Fill in the result's dimension
        *((LPAPLDIM) lpMemRes)++ = 1;

        // Point to the left arg immediate value
        lpMemLft = &aplLongestLft;
    } // End IF/ELSE

    if (lpMemRht NE NULL)
        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemRht);
    else
        // Point to the right arg immediate value
        lpMemRht = &aplLongestRht;

    // Split cases based upon the left and right arg storage types
    if (IsSimpleInt (aplTypeLft) && IsSimpleInt (aplTypeRht))
        lpPrimFnDydMEO_Com = PrimFnDydMEO_IvI;
    else
    if (IsSimpleNum (aplTypeLft) && IsSimpleNum (aplTypeRht))
        lpPrimFnDydMEO_Com = PrimFnDydMEO_NvN;
    else
    if (IsSimpleChar (aplTypeLft) && IsSimpleChar (aplTypeRht))
        lpPrimFnDydMEO_Com = PrimFnDydMEO_CvC;
    else
    if ((IsSimpleChar (aplTypeLft) && IsNumeric (aplTypeRht))
     || (IsSimpleChar (aplTypeRht) && IsNumeric (aplTypeLft)))
        goto YYALLOC_EXIT;
    else
        DbgStop ();             // We should never get here

    // Loop through the left arg values and look'em up
    //   in the right arg
    for (iLft = 0; iLft < (APLINT) aplNELMLft; iLft++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Call common routine
        (*lpPrimFnDydMEO_Com) (aplTypeLft,      // Left arg storage type
                               lpMemLft,        // Ptr to left arg global memory
#ifdef GRADE2ND
                               lpMemGupLft,     // Ptr to left arg grade-up global memory
#else
                               NULL,            // Ptr to left arg grade-up global memory
#endif
                               iLft,            // Loop counter
                               aplTypeRht,      // Right arg storage type
                               aplNELMRht,      // Right arg NELM
                               lpMemRht,        // Ptr to right arg global memory
                               lpMemGupRht,     // Ptr to right arg grade-up global memory
                               fQuadCT,         // []CT
                               lpMemRes);       // Ptr to result global memory
    } // End FOR
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

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes)
    {
        if (lpMemRes NE NULL)
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
#ifdef GRADE2ND
    if (hGlbGupLft && lpMemGupLft)
    {
        if (lpMemGupLft NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbGupLft); lpMemGupLft = NULL;
        } // End IF

        // We no longer need this resource
        FreeResultGlobalVar (hGlbGupLft); hGlbGupLft = NULL;
    } // End IF
#endif
    if (hGlbGupRht && lpMemGupRht)
    {
        if (lpMemGupRht NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbGupRht); lpMemGupRht = NULL;
        } // End IF

        // We no longer need this resource
        FreeResultGlobalVar (hGlbGupRht); hGlbGupRht = NULL;
    } // End IF

    return lpYYRes;

} // End PrimFnDydMEO_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydMEO_IvI
//
//  Subroutine to PrimFnDydMEO_Com for simple ints
//***************************************************************************

void PrimFnDydMEO_IvI
    (APLSTYPE   aplTypeLft,     // Left arg storage type
     LPVOID     lpMemLft,       // Ptr to left arg global memory
     LPPDIE     lpMemGupLft,    // Ptr to left arg grade-up global memory
     APLINT     iLft,           // Loop counter
     APLSTYPE   aplTypeRht,     // Right arg storage type
     APLNELM    aplNELMRht,     // Right arg NELM
     LPVOID     lpMemRht,       // Ptr to right arg global memory
     LPPDIE     lpMemGupRht,    // Ptr to right arg grade-up global memory
     APLFLOAT   fQuadCT,        // []CT
     LPAPLBOOL  lpMemRes)       // Ptr to result global memory

{
#ifdef GRADE2ND
    APLINT iRes;                // Loop counter
#endif
    APLINT aplIntegerLft,       // Left arg integer
           aplIntegerRht,       // Right arg integer
           iRht,                // Loop counter
           iMin,                // Minimum index
           iMax;                // Maximum ...

#ifdef GRADE2ND
        // Get the result index
        iRes = lpMemGupLft[iLft].Index;

        // Get the next integer from the left arg
        aplIntegerLft =
          GetNextInteger (lpMemLft,                     // Ptr to global memory
                          aplTypeLft,                   // Storage type
                          iRes);                        // Index
#else
        // Get the next integer from the left arg
        aplIntegerLft =
          GetNextInteger (lpMemLft,                     // Ptr to global memory
                          aplTypeLft,                   // Storage type
                          iLft);                        // Index
#endif
        // Initialize the right arg minimum and maximum indices
        iMin = 0;
        iMax = aplNELMRht - 1;

        // Lookup this value in the right arg (binary search)
        while (iMin <= iMax)
        {
            // Set the current index
            iRht = (iMin + iMax) / 2;

            // Get the next integer from the right arg
            aplIntegerRht =
              GetNextInteger (lpMemRht,                 // Ptr to global memory
                              aplTypeRht,               // Storage type
                              lpMemGupRht[iRht].Index); // Index
            // Check for a match
            if (aplIntegerRht > aplIntegerLft)
                iMax = iRht - 1;
            else
            if (aplIntegerRht < aplIntegerLft
             || (aplIntegerRht EQ aplIntegerLft
              && lpMemGupRht[iRht].Used))
                iMin = iRht + 1;
            else
            {
                // We found a match -- check earlier indices for a match
                //   so we always return the lowest index match.
                for (iRht = iRht - 1;
                     iRht >= 0 && !lpMemGupRht[iRht].Used;
                     iRht--)
                {
                    // Get the next integer from the right arg
                    aplIntegerRht =
                      GetNextInteger (lpMemRht,                 // Ptr to global memory
                                      aplTypeRht,               // Storage type
                                      lpMemGupRht[iRht].Index); // Index
                    if (aplIntegerLft NE aplIntegerRht)
                        break;
                } // End FOR
#ifdef GRADE2ND
                // Save in the result
                lpMemRes[iRes >> LOG2NBIB] |=
                  BIT0 << (MASKLOG2NBIB & (UINT) iRes);
#else
                // Save in the result
                lpMemRes[iLft >> LOG2NBIB] |=
                  BIT0 << (MASKLOG2NBIB & (UINT) iLft);
#endif
                // Mark as used
                lpMemGupRht[iRht + 1].Used = TRUE;

                break;
            } // End IF/ELSE/...
        } // End WHILE
#ifdef GRADE2ND
////////// If we didn't find a match, ...
////////if (iMin > iMax)
////////{
////////    // Save in the result
////////    lpMemRes[uRes >> LOG2NBIB] |=
////////      0 << (MASKLOG2NBIB & (UINT) uRes);
////////} // End IF
#endif
} // End PrimFnDydMEO_IvI


//***************************************************************************
//  $PrimFnDydMEO_NvN
//
//  Subroutine to PrimFnDydMEO_Com for simple nums one of which is a float
//***************************************************************************

void PrimFnDydMEO_NvN
    (APLSTYPE   aplTypeLft,     // Left arg storage type
     LPVOID     lpMemLft,       // Ptr to left arg global memory
     LPPDIE     lpMemGupLft,    // Ptr to left arg grade-up global memory
     APLINT     iLft,           // Loop counter
     APLSTYPE   aplTypeRht,     // Right arg storage type
     APLNELM    aplNELMRht,     // Right arg NELM
     LPVOID     lpMemRht,       // Ptr to right arg global memory
     LPPDIE     lpMemGupRht,    // Ptr to right arg grade-up global memory
     APLFLOAT   fQuadCT,        // []CT
     LPAPLBOOL  lpMemRes)       // Ptr to result global memory

{
#ifdef GRADE2ND
    APLINT   iRes;              // Loop counter
#endif
    APLFLOAT aplFloatLft,       // Left arg float
             aplFloatRht;       // Right arg float
    APLINT   iRht,              // Loop counter
             iMin,              // Minimum index
             iMax;              // Maximum ...
    UBOOL    bComp;             // TRUE iff the left and right floats are equal within []CT

#ifdef GRADE2ND
        // Get the result index
        iRes = lpMemGupLft[iLft].Index;

        // Get the next float from the left arg
        aplFloatLft =
          GetNextFloat (lpMemLft,                       // Ptr to global memory
                        aplTypeLft,                     // Storage type
                        iRes);                          // Index
#else
        // Get the next float from the left arg
        aplFloatLft =
          GetNextFloat (lpMemLft,                       // Ptr to global memory
                        aplTypeLft,                     // Storage type
                        iLft);                          // Index
#endif
        // Initialize the right arg minimum and maximum indices
        iMin = 0;
        iMax = aplNELMRht - 1;

        // Lookup this value in the right arg (binary search)
        while (iMin <= iMax)
        {
            // Set the current index
            iRht = (iMin + iMax) / 2;

            // Get the next float from the right arg
            aplFloatRht =
              GetNextFloat (lpMemRht,                   // Ptr to global memory
                            aplTypeRht,                 // Storage type
                            lpMemGupRht[iRht].Index);   // Index
            // Compare 'em
            bComp = CompareCT (aplFloatLft, aplFloatRht, fQuadCT, NULL);

            // Check for a match
            if (bComp)
            {
                // We found a match

                // If it's already used, ...
                if (lpMemGupRht[iRht].Used)
                    iMin = iRht + 1;
                else
                {
                    // We found a match -- check earlier indices for a match
                    //   so we always return the lowest index match.
                    for (iRht = iRht - 1;
                         iRht >= 0 && !lpMemGupRht[iRht].Used;
                         iRht--)
                    {
                        // Get the next float from the right arg
                        aplFloatRht =
                          GetNextFloat (lpMemRht,                   // Ptr to global memory
                                        aplTypeRht,                 // Storage type
                                        lpMemGupRht[iRht].Index);   // Index
                        if (aplFloatLft NE aplFloatRht)
                            break;
                    } // End FOR
#ifdef GRADE2ND
                    // Save in the result
                    lpMemRes[iRes >> LOG2NBIB] |=
                      BIT0 << (MASKLOG2NBIB & (UINT) iRes);
#else
                    // Save in the result
                    lpMemRes[iLft >> LOG2NBIB] |=
                      BIT0 << (MASKLOG2NBIB & (UINT) iLft);
#endif
                    // Mark as used
                    lpMemGupRht[iRht + 1].Used = TRUE;

                    break;
                } // IF/ELSE
            } else
            if (aplFloatRht > aplFloatLft)
                iMax = iRht - 1;
            else
                iMin = iRht + 1;
        } // End WHILE
#ifdef GRADE2ND
////////// If we didn't find a match, ...
////////if (iMin > iMax)
////////{
////////    // Save in the result
////////    lpMemRes[uRes >> LOG2NBIB] |=
////////      0 << (MASKLOG2NBIB & (UINT) uRes);
////////} // End IF
#endif
} // End PrimFnDydMEO_NvN


//***************************************************************************
//  $PrimFnDydMEO_CvC
//
//  Subroutine to PrimFnDydMEO_Com for simple chars
//***************************************************************************

void PrimFnDydMEO_CvC
    (APLSTYPE   aplTypeLft,     // Left arg storage type
     LPVOID     lpMemLft,       // Ptr to left arg global memory
     LPPDIE     lpMemGupLft,    // Ptr to left arg grade-up global memory
     APLINT     iLft,           // Loop counter
     APLSTYPE   aplTypeRht,     // Right arg storage type
     APLNELM    aplNELMRht,     // Right arg NELM
     LPVOID     lpMemRht,       // Ptr to right arg global memory
     LPPDIE     lpMemGupRht,    // Ptr to right arg grade-up global memory
     APLFLOAT   fQuadCT,        // []CT
     LPAPLBOOL  lpMemRes)       // Ptr to result global memory

{
#ifdef GRADE2ND
    APLINT  iRes;               // Loop counter
#endif
    APLCHAR aplCharLft,         // Left arg char
            aplCharRht;         // Right arg char
    APLINT  iRht,               // Loop counter
            iMin,               // Minimum index
            iMax;               // Maximum ...

#ifdef GRADE2ND
        // Get the result index
        iRes = lpMemGupLft[iLft].Index;

        // Get the next char from the left arg
        aplCharLft = ((LPAPLCHAR) lpMemLft)[iRes];
#else
        // Get the next char from the left arg
        aplCharLft = ((LPAPLCHAR) lpMemLft)[iLft];
#endif
        // Initialize the right arg minimum and maximum indices
        iMin = 0;
        iMax = aplNELMRht - 1;

        // Lookup this value in the right arg (binary search)
        while (iMin <= iMax)
        {
            // Set the current index
            iRht = (iMin + iMax) / 2;

            // Get the next char from the right arg
            aplCharRht = ((LPAPLCHAR) lpMemRht)[lpMemGupRht[iRht].Index];

            // Check for a match
            if (aplCharRht > aplCharLft)
                iMax = iRht - 1;
            else
            if (aplCharRht < aplCharLft
             || (aplCharRht EQ aplCharLft
              && lpMemGupRht[iRht].Used))
                iMin = iRht + 1;
            else
            {
                // We found a match -- check earlier indices for a match
                //   so we always return the lowest index match.
                for (iRht = iRht - 1;
                     iRht >= 0 && !lpMemGupRht[iRht].Used;
                     iRht--)
                {
                    // Get the next char from the right arg
                    aplCharRht = ((LPAPLCHAR) lpMemRht)[lpMemGupRht[iRht].Index];

                    if (aplCharLft NE aplCharRht)
                        break;
                } // End IF/FOR
#ifdef GRADE2ND
                // Save in the result
                lpMemRes[iRes >> LOG2NBIB] |=
                  BIT0 << (MASKLOG2NBIB & (UINT) iRes);
#else
                // Save in the result
                lpMemRes[iLft >> LOG2NBIB] |=
                  BIT0 << (MASKLOG2NBIB & (UINT) iLft);
#endif
                // Mark as used
                lpMemGupRht[iRht + 1].Used = TRUE;

                break;
            } // End IF/ELSE/...
        } // End WHILE
#ifdef GRADE2ND
////////// If we didn't find a match, ...
////////if (iMin > iMax)
////////{
////////    // Save in the result
////////    lpMemRes[uRes >> LOG2NBIB] |=
////////      0 << (MASKLOG2NBIB & (UINT) uRes);
////////} // End IF
#endif
} // End PrimFnDydMEO_CvC


//***************************************************************************
//  $PrimFnDydMIO_EM_YY
//
//  Execute Multiset Index Of between simple non-heteros
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydMIO_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydMIO_EM_YY
    (LPTOKEN  lptkLftArg,               // Ptr to left arg token
     APLSTYPE aplTypeLft,               // Left arg storage type
     LPTOKEN  lptkFunc,                 // Ptr to function token
     LPTOKEN  lptkRhtArg,               // Ptr to right arg token
     APLSTYPE aplTypeRht,               // Right ...
     LPTOKEN  lptkAxis)                 // Ptr to axis token (may be NULL)

{
    APLNELM       aplNELMLft,           // Left arg NELM
                  aplNELMRht;           // Right ...
    APLRANK       aplRankLft,           // Left arg rank
                  aplRankRht;           // Right ...
    HGLOBAL       hGlbLft = NULL,       // Left arg global memory handle
                  hGlbRht = NULL,       // Right ...
                  hGlbRes = NULL;       // Result   ...
    LPVOID        lpMemLft = NULL,      // Ptr to left arg global memory
                  lpMemRht = NULL;      // Ptr to right ...
    LPAPLUINT     lpMemRes = NULL;      // Ptr to result   ...
    TOKEN         tkFunc = {0};         // Grade-up function token
#ifdef GRADE2ND
    HGLOBAL       hGlbGupRht = NULL;    // Right arg grade-up global memory handle
    LPPDIE        lpMemGupRht = NULL;   // Ptr to right arg grade-up global memory
#endif
    HGLOBAL       hGlbGupLft = NULL;    // Left arg grade-up global memory handle
    LPPDIE        lpMemGupLft = NULL;   // Ptr to left arg grade-up global memory
    PDIE          aplIntegerZero = {0}; // A zero in case the right arg is a scalar
    APLUINT       NotFound,             // Not found value
                  ByteRes;              // # bytes in the result
    APLINT        iRht;                 // Loop counter
    APLLONGEST    aplLongestLft,        // Left arg immediate value
                  aplLongestRht;        // Right ...
    LPPL_YYSTYPE  lpYYRes;              // Ptr to grade-up result
    UBOOL         bQuadIO;              // []IO
    APLFLOAT      fQuadCT;              // []CT
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    LPMIOCOM      lpPrimFnDydMIO_Com;   // Ptr to common routine

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the current value of []IO & []CT
    bQuadIO = GetQuadIO ();
    fQuadCT = GetQuadCT ();

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, NULL, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, NULL, &aplNELMRht, &aplRankRht, NULL);

    // Check for LEFT RANK ERROR
    if (IsMultiRank (aplRankLft))
        goto LEFT_RANK_EXIT;

    // Check for RIGHT RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Get left and right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Setup the grade-up function token
    tkFunc.tkFlags.TknType   = TKT_FCNIMMED;
    tkFunc.tkFlags.ImmType   = IMMTYPE_PRIMFCN;
////tkFunc.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
    tkFunc.tkData.tkChar     = UTF16_DELTASTILE;
    tkFunc.tkCharIndex       = lptkFunc->tkCharIndex;

    // If the left arg is not a scalar, ...
    if (!IsScalar (aplRankLft))
    {
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
        lpMemGupLft = MyGlobalLockVar (hGlbGupLft);

        // Skip over the header and dimensions to the data
        lpMemGupLft = VarArrayDataFmBase (lpMemGupLft);
    } else
        // Handle the scalar case
        lpMemGupLft = &aplIntegerZero;
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
        lpMemGupRht = MyGlobalLockVar (hGlbGupRht);

        // Skip over the header and dimensions to the data
        lpMemGupRht = VarArrayDataFmBase (lpMemGupRht);
    } else
        // Handle the scalar case
        lpMemGupRht = &aplIntegerZero;
#endif
    //***************************************************************
    // Calculate space needed for the result
    //***************************************************************
    ByteRes = CalcArraySize (ARRAY_INT, aplNELMRht, 1);

    //***************************************************************
    // Now we can allocate the storage for the result
    // N.B.:  Conversion from APLUINT to UINT.
    //***************************************************************
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock000 (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_INT;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRht;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = (LPAPLUINT) VarArrayBaseToDim (lpMemRes);

    // Fill in the result's dimension
    if (lpMemRht NE NULL)
    {
        // Skip over the header to the dimensions
        lpMemRht = VarArrayBaseToDim (lpMemRht);

        // Copy the left arg dimensions to the result
        CopyMemory (lpMemRes, lpMemRht, (APLU3264) aplRankRht * sizeof (APLDIM));

        // Skip over the dimensions to the data
        lpMemRes = VarArrayDimToData (lpMemRes, 1         );
        lpMemRht = VarArrayDimToData (lpMemRht, aplRankRht);
    } else
    {
        // Fill in the result's dimension
        *((LPAPLDIM) lpMemRes)++ = 1;

        // Point to the right arg immediate value
        lpMemRht = &aplLongestRht;
    } // End IF/ELSE

    if (lpMemLft NE NULL)
        // Skip over the header and dimensions to the data
        lpMemLft = VarArrayDataFmBase (lpMemLft);
    else
        // Point to the left arg immediate value
        lpMemLft = &aplLongestLft;

    // Calculate the NotFound value
    NotFound = bQuadIO + aplNELMLft;

    // Split cases based upon the left and right arg storage types
    if (IsSimpleInt (aplTypeLft) && IsSimpleInt (aplTypeRht))
        lpPrimFnDydMIO_Com = PrimFnDydMIO_IvI;
    else
    if (IsSimpleNum (aplTypeLft) && IsSimpleNum (aplTypeRht))
        lpPrimFnDydMIO_Com = PrimFnDydMIO_NvN;
    else
    if (IsSimpleChar (aplTypeLft) && IsSimpleChar (aplTypeRht))
        lpPrimFnDydMIO_Com = PrimFnDydMIO_CvC;
    else
    if ((IsSimpleChar (aplTypeLft) && IsNumeric (aplTypeRht))
     || (IsSimpleChar (aplTypeRht) && IsNumeric (aplTypeLft)))
    {
        // Loop through the right arg values
        for (iRht = 0; iRht < (APLINT) aplNELMRht; iRht++)
        {
            // Check for Ctrl-Break
            if (CheckCtrlBreak (*lpbCtrlBreak))
                goto ERROR_EXIT;

            // Fill the result with NotFound
            *lpMemRes++ = NotFound;
        } // End FOR

        goto YYALLOC_EXIT;
    } else
        DbgStop ();             // We should never get here

    // Loop through the right arg values and look'em up
    //   in the left arg
    for (iRht = 0; iRht < (APLINT) aplNELMRht; iRht++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Call common routine
        (*lpPrimFnDydMIO_Com) (aplTypeLft,      // Left arg storage type
                               aplNELMLft,      // Left arg NELM
                               lpMemLft,        // Ptr to left arg global memory
                               lpMemGupLft,     // Ptr to left arg grade-up global memory
                               aplTypeRht,      // Right arg storage type
                               lpMemRht,        // Ptr to right arg global memory
#ifdef GRADE2ND
                               lpMemGupRht,     // Ptr to right arg grade-up global memory
#else
                               NULL,            // Ptr to left arg grade-up global memory
#endif
                               iRht,            // Loop counter
                               bQuadIO,         // []IO
                               fQuadCT,         // []CT
                               NotFound,        // Not found value
                               lpMemRes);       // Ptr to result global memory
    } // End FOR
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

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes)
    {
        if (lpMemRes NE NULL)
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

    if (hGlbGupLft && lpMemGupLft)
    {
        if (lpMemGupLft NE NULL)
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
        if (lpMemGupRht NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbGupRht); lpMemGupRht = NULL;
        } // End IF

        // We no longer need this resource
        FreeResultGlobalVar (hGlbGupRht); hGlbGupRht = NULL;
    } // End IF
#endif
    return lpYYRes;
} // End PrimFnDydMIO_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydMIO_IvI
//
//  Subroutine to PrimFnDydMIO_Com for simple ints
//***************************************************************************

void PrimFnDydMIO_IvI
    (APLSTYPE   aplTypeLft,     // Left arg storage type
     APLNELM    aplNELMLft,     // Left arg NELM
     LPVOID     lpMemLft,       // Ptr to left arg global memory
     LPPDIE     lpMemGupLft,    // Ptr to left arg grade-up global memory
     APLSTYPE   aplTypeRht,     // Right arg storage type
     LPVOID     lpMemRht,       // Ptr to right arg global memory
     LPPDIE     lpMemGupRht,    // Ptr to right arg grade-up global memory
     APLINT     iRht,           // Loop counter
     UBOOL      bQuadIO,        // []IO
     APLFLOAT   fQuadCT,        // []CT
     APLUINT    NotFound,       // Not found value
     LPAPLUINT  lpMemRes)       // Ptr to result global memory

{
#ifdef GRADE2ND
    APLINT  iRes;               // Loop counter
    APLUINT uLastVal;           // Last value saved in the result
#endif
    APLINT  aplIntegerLft,      // Left arg integer
            aplIntegerRht,      // Right arg integer
            iLft,               // Loop counter
            iMin,               // Minimum index
            iMax;               // Maximum ...

#ifdef GRADE2ND
        // Get the result index
        iRes = lpMemGupRht[iRht].Index;

        // Get the next integer from the right arg
        aplIntegerRht =
          GetNextInteger (lpMemRht,                     // Ptr to global memory
                          aplTypeRht,                   // Storage type
                          iRes);                        // Index
#else
        // Get the next integer from the right arg
        aplIntegerRht =
          GetNextInteger (lpMemRht,                     // Ptr to global memory
                          aplTypeRht,                   // Storage type
                          iRht);                        // Index
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
              GetNextInteger (lpMemLft,                 // Ptr to global memory
                              aplTypeLft,               // Storage type
                              lpMemGupLft[iLft].Index); // Index
            // Check for a match
            if (aplIntegerLft > aplIntegerRht)
                iMax = iLft - 1;
            else
            if (aplIntegerLft < aplIntegerRht
             || (aplIntegerLft EQ aplIntegerRht
              && lpMemGupLft[iLft].Used))
                iMin = iLft + 1;
            else
            {
                // We found a match -- check earlier indices for a match
                //   so we always return the lowest index match.
                for (iLft = iLft - 1;
                     iLft >= 0 && !lpMemGupLft[iLft].Used;
                     iLft--)
                {
                    // Get the next integer from the left arg
                    aplIntegerLft =
                      GetNextInteger (lpMemLft,                 // Ptr to global memory
                                      aplTypeLft,               // Storage type
                                      lpMemGupLft[iLft].Index); // Index
                    if (aplIntegerLft NE aplIntegerRht)
                        break;
                } // End FOR
#ifdef GRADE2ND
                // Save in the result
                lpMemRes[iRes] =
                  uLastVal     = bQuadIO + lpMemGupLft[iLft + 1].Index;
#else
                // Save in the result
                lpMemRes[iRht] = bQuadIO + lpMemGupLft[iLft + 1].Index;
#endif
                // Mark as used
                lpMemGupLft[iLft + 1].Used = TRUE;

                break;
            } // End IF/ELSE/...
        } // End WHILE

        // If we didn't find a match, ...
        if (iMin > iMax)
#ifdef GRADE2ND
            // Save in the result
            lpMemRes[lpMemGupRht[iRht].Index] =
              uLastVal                        = NotFound;
#else
            // Save in the result
            lpMemRes[iRht] = NotFound;
#endif
} // End PrimFnDydMIO_IvI


//***************************************************************************
//  $PrimFnDydMIO_NvN
//
//  Subroutine to PrimFnDydMIO_Com for simple nums one of which is a float
//***************************************************************************

void PrimFnDydMIO_NvN
    (APLSTYPE   aplTypeLft,     // Left arg storage type
     APLNELM    aplNELMLft,     // Left arg NELM
     LPVOID     lpMemLft,       // Ptr to left arg global memory
     LPPDIE     lpMemGupLft,    // Ptr to left arg grade-up global memory
     APLSTYPE   aplTypeRht,     // Right arg storage type
     LPVOID     lpMemRht,       // Ptr to right arg global memory
     LPPDIE     lpMemGupRht,    // Ptr to right arg grade-up global memory
     APLINT     iRht,           // Loop counter
     UBOOL      bQuadIO,        // []IO
     APLFLOAT   fQuadCT,        // []CT
     APLUINT    NotFound,       // Not found value
     LPAPLUINT  lpMemRes)       // Ptr to result global memory

{
#ifdef GRADE2ND
    APLINT   iRes;              // Loop counter
    APLUINT  uLastVal;          // Last value saved in the result
#endif
    APLFLOAT aplFloatLft,       // Left arg float
             aplFloatRht;       // Right ...
    APLINT   iLft,              // Loop counter
             iMin,              // Minimum index
             iMax;              // Maximum ...
    UBOOL    bComp;             // TRUE iff the left and right floats are equal within []CT

#ifdef GRADE2ND
        // Get the result index
        iRes = lpMemGupRht[iRht].Index;

        // Get the next float from the right arg
        aplFloatRht =
          GetNextFloat (lpMemRht,                       // Ptr to global memory
                        aplTypeRht,                     // Storage type
                        iRes);                          // Index
#else
        // Get the next float from the right arg
        aplFloatRht =
          GetNextFloat (lpMemRht,                       // Ptr to global memory
                        aplTypeRht,                     // Storage type
                        iRht);                          // Index
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
              GetNextFloat (lpMemLft,                   // Ptr to global memory
                            aplTypeLft,                 // Storage type
                            lpMemGupLft[iLft].Index);   // Index
            // Compare 'em
            bComp = CompareCT (aplFloatLft, aplFloatRht, fQuadCT, NULL);

            // Check for a match
            if (bComp)
            {
                // We found a match

                // If it's already used, ...
                if (lpMemGupLft[iLft].Used)
                    iMin = iLft + 1;
                else
                {
                    // We found a match -- check earlier indices for a match
                    //   so we always return the lowest index match.
                    for (iLft = iLft - 1;
                         iLft >= 0 && !lpMemGupLft[iLft].Used;
                         iLft--)
                    {
                        // Get the next float from the left arg
                        aplFloatLft =
                          GetNextFloat (lpMemLft,                   // Ptr to global memory
                                        aplTypeLft,                 // Storage type
                                        lpMemGupLft[iLft].Index);   // Index
                        if (!CompareCT (aplFloatLft, aplFloatRht, fQuadCT, NULL))
                            break;
                    } // End FOR
#ifdef GRADE2ND
                    // Save in the result
                    lpMemRes[iRes] =
                      uLastVal     = bQuadIO + lpMemGupLft[iLft + 1].Index;
#else
                    // Save in the result
                    lpMemRes[iRht] = bQuadIO + lpMemGupLft[iLft + 1].Index;
#endif
                    // Mark as used
                    lpMemGupLft[iLft + 1].Used = TRUE;

                    break;
                } // End IF/ELSE
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
            lpMemRes[lpMemGupRht[iRht].Index] =
              uLastVal                        = NotFound;
#else
            // Save in the result
            lpMemRes[iRht] = NotFound;
#endif
} // End PrimFnDydMIO_NvN


//***************************************************************************
//  $PrimFnDydMIO_CvC
//
//  Subroutine to PrimFnDydMIO_Com for simple chars
//***************************************************************************

void PrimFnDydMIO_CvC
    (APLSTYPE   aplTypeLft,     // Left arg storage type
     APLNELM    aplNELMLft,     // Left arg NELM
     LPVOID     lpMemLft,       // Ptr to left arg global memory
     LPPDIE     lpMemGupLft,    // Ptr to left arg grade-up global memory
     APLSTYPE   aplTypeRht,     // Right arg storage type
     LPVOID     lpMemRht,       // Ptr to right arg global memory
     LPPDIE     lpMemGupRht,    // Ptr to right arg grade-up global memory
     APLINT     iRht,           // Loop counter
     UBOOL      bQuadIO,        // []IO
     APLFLOAT   fQuadCT,        // []CT
     APLUINT    NotFound,       // Not found value
     LPAPLUINT  lpMemRes)       // Ptr to result global memory

{
#ifdef GRADE2ND
    APLINT  iRes;               // Loop counter
    APLUINT uLastVal;           // Last value saved in the result
#endif
    APLCHAR aplCharLft,         // Left arg char
            aplCharRht;         // Right arg char
    APLINT  iLft,               // Loop counter
            iMin,               // Minimum index
            iMax;               // Maximum ...

#ifdef GRADE2ND
        // Get the result index
        iRes = lpMemGupRht[iRht].Index;

        // Get the next char from the right arg
        aplCharRht = ((LPAPLCHAR) lpMemRht)[iRes];
#else
        // Get the next char from the right arg
        aplCharRht = ((LPAPLCHAR) lpMemRht)[iRht];
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
            aplCharLft = ((LPAPLCHAR) lpMemLft)[lpMemGupLft[iLft].Index];

            // Check for a match
            if (aplCharLft > aplCharRht)
                iMax = iLft - 1;
            else
            if (aplCharLft < aplCharRht
             || (aplCharLft EQ aplCharRht
              && lpMemGupLft[iLft].Used))
                iMin = iLft + 1;
            else
            {
                // We found a match -- check earlier indices for a match
                //   so we always return the lowest index match.
                for (iLft = iLft - 1;
                     iLft >= 0 && !lpMemGupLft[iLft].Used;
                     iLft--)
                {
                    // Get the next char from the left arg
                    aplCharLft = ((LPAPLCHAR) lpMemLft)[lpMemGupLft[iLft].Index];

                    if (aplCharLft NE aplCharRht)
                        break;
                } // End FOR
#ifdef GRADE2ND
                // Save in the result
                lpMemRes[iRes] =
                  uLastVal     = bQuadIO + lpMemGupLft[iLft + 1].Index;
#else
                // Save in the result
                lpMemRes[iRht] = bQuadIO + lpMemGupLft[iLft + 1].Index;
#endif
                // Mark as used
                lpMemGupLft[iLft + 1].Used = TRUE;

                break;
            } // End IF/ELSE/...
        } // End WHILE

        // If we didn't find a match, ...
        if (iMin > iMax)
#ifdef GRADE2ND
            // Save in the result
            lpMemRes[lpMemGupRht[iRht].Index] =
              uLastVal                        = NotFound;
#else
            // Save in the result
            lpMemRes[iRht] = NotFound;
#endif
} // End PrimFnDydMIO_CvC


//***************************************************************************
//  $PrimFnDydMM_EM_YY
//
//  Execute Multiset Match between sinple non-heteros
//***************************************************************************

LPPL_YYSTYPE PrimFnDydMM_EM_YY
    (LPTOKEN  lptkLftArg,               // Ptr to left arg token
     APLSTYPE aplTypeLft,               // Left arg storage type
     LPTOKEN  lptkFunc,                 // Ptr to function token
     LPTOKEN  lptkRhtArg,               // Ptr to right arg token
     APLSTYPE aplTypeRht,               // Right arg storage type
     LPTOKEN  lptkAxis)                 // Ptr to axis token (may be NULL)

{
    APLNELM       aplNELMLft,           // Left arg NELM
                  aplNELMRht;           // Right ...
    APLRANK       aplRankLft,           // Left arg rank
                  aplRankRht;           // Right ...
    HGLOBAL       hGlbLft = NULL,       // Left arg global memory handle
                  hGlbRht = NULL;       // Right ...
    LPVOID        lpMemLft = NULL,      // Ptr to left arg global memory
                  lpMemRht = NULL;      // Ptr to right ...
    TOKEN         tkFunc = {0};         // Grade-up function token
    HGLOBAL       hGlbGupLft = NULL,    // Left arg grade-up global memory handle
                  hGlbGupRht = NULL;    // Right ...
    LPPDIE        lpMemGupLft = NULL,   // Ptr to left arg grade-up global memory
                  lpMemGupRht = NULL;   // ...    right ...
    APLLONGEST    aplLongestLft,        // Left arg immediate value
                  aplLongestRht;        // Right ...
    LPPL_YYSTYPE  lpYYRes = NULL;       // Ptr to grade-up result
    UBOOL         bQuadIO;              // []IO
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    UBOOL         aplBoolRes;           // The result
    LPMMCOM       lpPrimFnDydMM_Com;    // Ptr to common routine

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, NULL, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, NULL, &aplNELMRht, &aplRankRht, NULL);

    // Get left and right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Check for mismatched lengths (RANK ERRORS are handled by the caller)
    //   or mismatched types
    if (aplNELMLft NE aplNELMRht
     || (IsSimpleChar (aplTypeLft) && IsNumeric (aplTypeRht))
     || (IsSimpleChar (aplTypeRht) && IsNumeric (aplTypeLft)))
        aplBoolRes = FALSE;
    else
    // If the args are singletons, ...
    //   (this covers the scalar vs. one-element vector case)
    if (IsSingleton (aplNELMLft))
        aplBoolRes = (aplLongestLft EQ aplLongestRht);
    else
    {
        // Setup the grade-up function token
        tkFunc.tkFlags.TknType   = TKT_FCNIMMED;
        tkFunc.tkFlags.ImmType   = IMMTYPE_PRIMFCN;
////////tkFunc.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
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

        // Check for error
        if (lpYYRes EQ NULL)
            goto ERROR_EXIT;
        // Get the grade-up global memory handle
        hGlbGupLft = lpYYRes->tkToken.tkData.tkGlbData;

        // Free the YYRes
        YYFree (lpYYRes); lpYYRes = NULL;

        // Lock the memory to get a ptr to it
        lpMemGupLft = MyGlobalLockVar (hGlbGupLft);

        // Skip over the header and dimensions to the data
        lpMemGupLft = VarArrayDataFmBase (lpMemGupLft);

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

        // Check for error
        if (lpYYRes EQ NULL)
            goto ERROR_EXIT;
        // Get the grade-up global memory handle
        hGlbGupRht = lpYYRes->tkToken.tkData.tkGlbData;

        // Free the YYRes
        YYFree (lpYYRes); lpYYRes = NULL;

        // Lock the memory to get a ptr to it
        lpMemGupRht = MyGlobalLockVar (hGlbGupRht);

        // Skip over the header and dimensions to the data
        lpMemGupRht = VarArrayDataFmBase (lpMemGupRht);

        // Skip over the header and dimensions to the data
        lpMemLft = VarArrayDataFmBase (lpMemLft);
        lpMemRht = VarArrayDataFmBase (lpMemRht);

        // Split cases based upon the left and right arg storage types
        if (IsSimpleInt (aplTypeLft) && IsSimpleInt (aplTypeRht))
            lpPrimFnDydMM_Com = PrimFnDydMM_IvI;
        else
        if (IsSimpleNum (aplTypeLft) && IsSimpleNum (aplTypeRht))
            lpPrimFnDydMM_Com = PrimFnDydMM_NvN;
        else
        if (IsSimpleChar (aplTypeLft) && IsSimpleChar (aplTypeRht))
            lpPrimFnDydMM_Com = PrimFnDydMM_CvC;
        else
            DbgStop ();             // We should never get here

        // Call common routine
        aplBoolRes =
          (*lpPrimFnDydMM_Com) (aplTypeLft,     // Left arg storage type
                                aplNELMLft,     // Left arg NELM
                                lpMemLft,       // Ptr to left arg global memory
                                lpMemGupLft,    // Ptr to left arg grade-up global memory
                                aplTypeRht,     // Right arg storage type
                                lpMemRht,       // Ptr to right arg global memory
                                lpMemGupRht,    // Ptr to right arg grade-up global memory
                                lpbCtrlBreak);  // Ptr to Ctrl-Break flag
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;
    } // End IF/ELSE

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_BOOL;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkBoolean  = aplBoolRes;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

ERROR_EXIT:
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

    if (hGlbGupLft)
    {
        if (lpMemGupLft NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbGupLft); lpMemGupLft = NULL;
        } // End IF

        // We no longer need this resource
        FreeResultGlobalVar (hGlbGupLft); hGlbGupLft = NULL;
    } // End IF

    if (hGlbGupRht)
    {
        if (lpMemGupRht NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbGupRht); lpMemGupRht = NULL;
        } // End IF

        // We no longer need this resource
        FreeResultGlobalVar (hGlbGupRht); hGlbGupRht = NULL;
    } // End IF

    return lpYYRes;
} // End PrimFnDydMM_EM_YY


//***************************************************************************
//  $PrimFnDydMM_IvI
//
//  Subroutine to PrimFnDydMM_Com for simple ints
//***************************************************************************

APLBOOL PrimFnDydMM_IvI
    (APLSTYPE   aplTypeLft,     // Left arg storage type
     APLNELM    aplNELMLft,     // Left arg NELM
     LPAPLCHAR  lpMemLft,       // Ptr to left arg global memory
     LPPDIE     lpMemGupLft,    // Ptr to left arg grade-up global memory
     APLSTYPE   aplTypeRht,     // Right arg storage type
     LPAPLCHAR  lpMemRht,       // Ptr to right arg global memory
     LPPDIE     lpMemGupRht,    // Ptr to right arg grade-up global memory
     LPUBOOL    lpbCtrlBreak)   // Ptr to Ctrl-Break flag

{
    APLUINT  uRes;              // Loop counter
    APLINT   aplIntegerLft,     // Left arg integer
             aplIntegerRht;     // Right ...

    // Compare the left & right arg values
    for (uRes = 0; uRes < aplNELMLft; uRes++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Get the next integer from the left arg
        aplIntegerLft =
          GetNextInteger (lpMemLft,                     // Ptr to global memory
                          aplTypeLft,                   // Storage type
                          lpMemGupLft[uRes].Index);     // Index
        // Get the next integer from the right arg
        aplIntegerRht =
          GetNextInteger (lpMemRht,                     // Ptr to global memory
                          aplTypeRht,                   // Storage type
                          lpMemGupRht[uRes].Index);     // Index
        if (aplIntegerLft NE aplIntegerRht)
            return FALSE;
    } // End FOR
ERROR_EXIT:
    return TRUE;
} // End PrimFnDydMM_IvI


//***************************************************************************
//  $PrimFnDydMM_NvN
//
//  Subroutine to PrimFnDydMM_Com for simple nums one of which is a float
//***************************************************************************

APLBOOL PrimFnDydMM_NvN
    (APLSTYPE   aplTypeLft,     // Left arg storage type
     APLNELM    aplNELMLft,     // Left arg NELM
     LPAPLCHAR  lpMemLft,       // Ptr to left arg global memory
     LPPDIE     lpMemGupLft,    // Ptr to left arg grade-up global memory
     APLSTYPE   aplTypeRht,     // Right arg storage type
     LPAPLCHAR  lpMemRht,       // Ptr to right arg global memory
     LPPDIE     lpMemGupRht,    // Ptr to right arg grade-up global memory
     LPUBOOL    lpbCtrlBreak)   // Ptr to Ctrl-Break flag

{
    APLUINT  uRes;              // Loop counter
    APLFLOAT aplFloatLft,       // Left arg float
             aplFloatRht;       // Right ...

    // Compare the left & right arg values
    for (uRes = 0; uRes < aplNELMLft; uRes++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Get the next float from the left arg
        aplFloatLft =
          GetNextFloat (lpMemLft,                       // Ptr to global memory
                        aplTypeLft,                     // Storage type
                        lpMemGupLft[uRes].Index);       // Index
        // Get the next float from the right arg
        aplFloatRht =
          GetNextFloat (lpMemRht,                       // Ptr to global memory
                        aplTypeRht,                     // Storage type
                        lpMemGupRht[uRes].Index);       // Index
        if (aplFloatLft NE aplFloatRht)
            return FALSE;
    } // End FOR
ERROR_EXIT:
    return TRUE;
} // End PrimFnDydMM_NvN


//***************************************************************************
//  $PrimFnDydMM_CvC
//
//  Subroutine to PrimFnDydMM_Com for simple chars
//***************************************************************************

APLBOOL PrimFnDydMM_CvC
    (APLSTYPE   aplTypeLft,     // Left arg storage type
     APLNELM    aplNELMLft,     // Left arg NELM
     LPAPLCHAR  lpMemLft,       // Ptr to left arg global memory
     LPPDIE     lpMemGupLft,    // Ptr to left arg grade-up global memory
     APLSTYPE   aplTypeRht,     // Right arg storage type
     LPAPLCHAR  lpMemRht,       // Ptr to right arg global memory
     LPPDIE     lpMemGupRht,    // Ptr to right arg grade-up global memory
     LPUBOOL    lpbCtrlBreak)   // Ptr to Ctrl-Break flag

{
    APLUINT uRes;               // Loop counter

    // Compare the left & right arg values
    for (uRes = 0; uRes < aplNELMLft; uRes++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        if (lpMemLft[lpMemGupLft[uRes].Index] NE lpMemRht[lpMemGupRht[uRes].Index])
            return FALSE;
    } // End FOR
ERROR_EXIT:
    return TRUE;
} // End PrimFnDydMM_CvC


//***************************************************************************
//  Magic function for Multiset Asymmetric Difference
//
//  Dyadic Tilde -- Multiset Asymmetric Difference
//
//  On scalar or vector args, return the elements in L~R.
//***************************************************************************

//***************************************************************************
//  Magic function for Multiset Symmetric Difference
//
//  Dyadic Section -- Multiset Symmetric Difference
//
//  On scalar or vector args, return the elements in (L~$R),R~$L.
//***************************************************************************

//***************************************************************************
//  Magic function for Multiset Union
//
//  Dyadic DownShoe -- Multiset Union
//
//  On scalar or vector args, return the multiset union of the two args
//***************************************************************************

//***************************************************************************
//  Magic function for Multiset Intersection
//
//  Dyadic UpShoe -- Intersection
//
//  On scalar or vector args, return the multiset intersection of the two args
//***************************************************************************

//***************************************************************************
//  Magic function for Multiset Proper Subset
//
//  Dyadic LeftShoe -- Multiset Proper Subset
//
//  On scalar or vector args, return TRUE iff L is a proper multisubset of R.
//***************************************************************************

//***************************************************************************
//  Magic function for Multiset Subset
//
//  Dyadic LeftShoeUnderbar -- Multiset Subset
//
//  On scalar or vector args, return TRUE iff L is a multisubset of R.
//***************************************************************************

//***************************************************************************
//  Magic function for Multiset Index Of
//
//  Dyadic Iota -- Multiset Index Of
//
//  On scalar or vector args, return the elements in L{iota}$R.
//***************************************************************************

//***************************************************************************
//  Magic function for Multiset Element Of
//
//  Dyadic Epsilon -- Multiset Element Of
//
//  On scalar or vector args, return the elements in L{epsilon}$R.
//***************************************************************************

//***************************************************************************
//  Magic function for Multiset Match
//
//  Dyadic Match -- Multiset Match
//
//  On scalar or vector args, return the elements in L{match}$R.
//***************************************************************************

//***************************************************************************
//  Magic function for Multiset Multiplicities
//
//  Monadic DownShoe -- Multiset Multiplicities
//
//  On scalar or vector args, return the multiplicities of the unique elements
//    in the same order as the unique elements.
//***************************************************************************

#include "mf_multiset.h"


//***************************************************************************
//  End of File: po_multiset.c
//***************************************************************************
