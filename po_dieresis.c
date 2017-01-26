//***************************************************************************
//  NARS2000 -- Primitive Operator -- Dieresis
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
//  $PrimOpDieresis_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    monadic operator Dieresis ("each" and "each")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDieresis_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpDieresis_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if niladic/monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token (may be NULL if niladic)

{
    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_DIERESIS);

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return
          PrimOpMonDieresis_EM_YY (lpYYFcnStrOpr,   // Ptr to operator function strand
                                   lptkRhtArg);     // Ptr to right arg token (may be NULL if niladic)
    else
        return
          PrimOpDydDieresis_EM_YY (lptkLftArg,      // Ptr to left arg token
                                   lpYYFcnStrOpr,   // Ptr to operator function strand
                                   lptkRhtArg);     // Ptr to right arg token
} // End PrimOpDieresis_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoOpDieresis_EM_YY
//
//  Generate a prototype for the derived functions from
//    monadic operator Dieresis ("each" and "each")
//***************************************************************************

LPPL_YYSTYPE PrimProtoOpDieresis_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxis)             // Ptr to axis token always NULL)

{
    Assert (lptkAxis EQ NULL);

    // If left arg is not present, ...
    if (lptkLftArg EQ NULL)
        //***************************************************************
        // Called monadically
        //***************************************************************
        return
          PrimOpMonDieresisCommon_EM_YY (lpYYFcnStrOpr,         // Ptr to operator function strand
                                         lptkRhtArg,            // Ptr to right arg token
                                         TRUE);                 // TRUE iff prototyping
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************
        return
          PrimOpDydDieresisCommon_EM_YY (lptkLftArg,            // Ptr to left arg token
                                         lpYYFcnStrOpr,         // Ptr to operator function strand
                                         lptkRhtArg,            // Ptr to right arg token
                                         TRUE);                 // TRUE iff prototyping
} // End PrimProtoOpDieresis_EM_YY


//***************************************************************************
//  $PrimIdentOpDieresis_EM_YY
//
//  Generate an identity element for the primitive operator dyadic Dieresis
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimIdentOpDieresis_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimIdentOpDieresis_EM_YY
    (LPTOKEN      lptkRhtOrig,      // Ptr to original right arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,    // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,       // Ptr to right arg token
     LPTOKEN      lptkAxisOpr)      // Ptr to axis token (may be NULL)

{
    APLNELM           aplNELMRht,           // Right arg NELM
                      aplNELMRes;           // Result    ...
    APLRANK           aplRankRht;           // Right arg rank
    APLSTYPE          aplTypeRht;           // Right arg storage type
    APLUINT           ByteRes,              // # bytes in the result
                      uRes;                 // Loop counter
    HGLOBAL           hGlbRht = NULL,       // Right arg global memory handle
                      hGlbRes = NULL;       // Result    ...
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg header
                      lpMemHdrRes = NULL;   // ...    result    ...
    LPAPLNESTED       lpMemRes;             // Ptr to result global memory
    LPPL_YYSTYPE      lpYYFcnStrLft,        // Ptr to left operand function strand
                      lpYYRes = NULL,       // Ptr to result
                      lpYYRes2 = NULL;      // Ptr to secondary result
    LPPRIMFLAGS       lpPrimFlagsLft;       // Ptr to left operand primitive flags
    LPTOKEN           lptkAxisLft;          // Ptr to axis operator token (if any)
    TOKEN             tkItem;               // Item token

    // The right arg is the prototype item from
    //   the original empty arg.

    Assert (lptkRhtOrig   NE NULL);
    Assert (lpYYFcnStrOpr NE NULL);
    Assert (lptkRhtArg    NE NULL);

    // The (left) identity function for dyadic Dieresis
    //   (L f {each} R) ("each") is
    //   ({primops} f) {each} R.

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = GetMonLftOper (lpYYFcnStrOpr, lptkAxisOpr);

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken)
     || IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_OPERAND_SYNTAX_EXIT;

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for left operand axis operator
    lptkAxisLft = CheckAxisOper (lpYYFcnStrLft);

    // Get the appropriate primitive flags ptr
    lpPrimFlagsLft = GetPrimFlagsPtr (&lpYYFcnStrLft->tkToken);

    // Check for error
    if (lpPrimFlagsLft EQ NULL || lpPrimFlagsLft->lpPrimOps EQ NULL)
        goto LEFT_OPERAND_DOMAIN_EXIT;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_NESTED, aplNELMRht, aplRankRht);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader    lpMemHdrRes
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_NESTED;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRht;
    lpHeader->Rank       = aplRankRht;
#undef  lpHeader

    // Get right arg's global ptr
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // Copy the dimensions from the right arg to the result
    CopyMemory (VarArrayBaseToDim (lpMemHdrRes),
                VarArrayBaseToDim (lpMemHdrRht),
     (APLU3264) aplRankRht * sizeof (APLDIM));

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // In case the result is empty, we need to process
    //   its prototype
    aplNELMRes = max (aplNELMRht, 1);

    // Loop through the items of the right arg
    //   passing them each to the right operand's
    //   identity function
    for (uRes = 0; uRes < aplNELMRes; uRes++)
    {
        if (aplNELMRht EQ 0
         && IsSimple (aplTypeRht))
        {
            if (IsSimpleChar (aplTypeRht))
                tkItem = tkBlank;
            else
                tkItem = tkZero;
        } else
            // Get the next item from the right arg into a token
            GetNextValueTokenIntoToken (lptkRhtArg,     // Ptr to the arg token
                                         uRes,          // Index to use
                                        &tkItem);       // Ptr to the result token
        // Execute the right operand identity function on each item of the right arg
        lpYYRes2 =
          (*lpPrimFlagsLft->lpPrimOps)
                            (lptkRhtArg,            // Ptr to original right arg token
                             lpYYFcnStrLft,         // Ptr to function strand
                            &tkItem,                // Ptr to right arg token
                             lptkAxisLft);          // Ptr to axis token (may be NULL)
        // Check for error
        if (lpYYRes2 EQ NULL)
            goto ERROR_EXIT;

        // Is the token immediate?
        if (IsTknImmed (&lpYYRes2->tkToken))
        {
            LPSYMENTRY lpSymEntry;

            // Copy the LPSYMENTRY as the result
            lpSymEntry =
              MakeSymEntry_EM (lpYYRes2->tkToken.tkFlags.ImmType,   // ImmType to use (see IMM_TYPES)
                              &lpYYRes2->tkToken.tkData.tkLongest,  // Ptr to value to use
                              &lpYYFcnStrLft->tkToken);             // Ptr to token to use in case of error
            // Check for error
            if (lpSymEntry EQ NULL)
                goto ERROR_EXIT;

            // Save in the result
            *lpMemRes++ = lpSymEntry;
        } else
            // Copy the HGLOBAL as the result
            *lpMemRes++ = CopySymGlbDir_PTB (lpYYRes2->tkToken.tkData.tkGlbData);

        // Free the YYRes
        FreeResult (lpYYRes2); YYFree (lpYYRes2); lpYYRes2 = NULL;
    } // End FOR

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken);

    goto NORMAL_EXIT;

LEFT_OPERAND_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

LEFT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
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

    if (lpYYRes2 NE NULL)
    {
        // Free the YYRes
        FreeResult (lpYYRes2); YYFree (lpYYRes2); lpYYRes2 = NULL;
    } // End IF
NORMAL_EXIT:
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
} // End PrimIdentOpDieresis_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpMonDieresis_EM_YY
//
//  Primitive operator for monadic derived function from Dieresis ("each")
//***************************************************************************

LPPL_YYSTYPE PrimOpMonDieresis_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return
      PrimOpMonDieresisCommon_EM_YY (lpYYFcnStrOpr,         // Ptr to operator function strand
                                     lptkRhtArg,            // Ptr to right arg token
                                     FALSE);                // TRUE iff prototyping
} // End PrimOpDieresis_EM_YY


//***************************************************************************
//  $PrimOpMonDieresisCommon_EM_YY
//
//  Primitive operator for monadic derived function from Dieresis ("each")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpMonDieresisCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpMonDieresisCommon_EM_YY
    (LPPL_YYSTYPE     lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN          lptkRhtArg,           // Ptr to right arg token (may be NULL if niladic)
     UBOOL            bPrototyping)         // TRUE iff protoyping

{
    APLUINT           ByteRes;              // # bytes in the result
    APLSTYPE          aplTypeRht,           // Right arg storage type
                      aplTypeRes;           // Result    ...
    APLNELM           aplNELMRht;           // Right arg NELM
    APLRANK           aplRankRht;           // Right arg rank
    HGLOBAL           hGlbRht = NULL,       // Right arg global memory handle
                      hGlbRes = NULL,       // Result    ...
                      lpSymGlb;             // Ptr to global numeric
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg header
                      lpMemHdrRes = NULL;   // ...    result    ...
    LPVOID            lpMemRht,             // Ptr to right arg global memory
                      lpMemRes;             // Ptr to result    ...
    APLUINT           uRht,                 // Right arg loop counter
                      uValErrCnt = 0;       // VALUE ERROR counter
    LPPL_YYSTYPE      lpYYRes = NULL,       // Ptr to the result
                      lpYYRes2;             // Ptr to secondary result
    APLINT            apaOff,               // Right arg APA offset
                      apaMul;               // ...           multiplier
    TOKEN             tkRhtArg = {0};       // Right arg token
    UINT              uBitMask;             // Bit mask for marching through Booleans
    UBOOL             bRet = TRUE;          // TRUE iff result is valid
    LPPL_YYSTYPE      lpYYFcnStrLft;        // Ptr to left operand function strand
    LPTOKEN           lptkAxisOpr,          // Ptr to operator axis token (may be NULL)
                      lptkAxisLft;          // Ptr to left operand axis token (may be NULL)
    LPPRIMFNS         lpPrimProtoLft;       // Ptr to left operand prototype function (may be NULL if not prototyping)
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Check for axis operator token
    lptkAxisOpr = CheckAxisOper (lpYYFcnStrOpr);

    //***************************************************************
    // The derived function from this operator is not sensitive to
    //   the axis operator, so signal a syntax error if present
    //***************************************************************
    if (lptkAxisOpr NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = GetMonLftOper (lpYYFcnStrOpr, lptkAxisOpr);

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken)
     || IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_OPERAND_SYNTAX_EXIT;

    // Check for left operand axis token
    lptkAxisLft = CheckAxisOper (lpYYFcnStrLft);

    // If the right arg is present, ...
    if (lptkRhtArg NE NULL)
        // Get the attributes (Type, NELM, and Rank) of the right arg
        AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // The result storage type is assumed to be NESTED,
    //   but we'll call TypeDemote at the end just in case.
    aplTypeRes = ARRAY_NESTED;

    //***************************************************************
    //  Handle prototypes separately
    //***************************************************************
    // If the derived function is not niladic,
    //   and the right arg is ether niladic or we're prototyping, ...
    if (lptkRhtArg NE NULL
     && (IsEmpty (aplNELMRht)
      || bPrototyping))
    {
        // Get the appropriate prototype function ptr
        lpPrimProtoLft = GetPrototypeFcnPtr (&lpYYFcnStrLft->tkToken);
        if (lpPrimProtoLft EQ NULL)
            goto LEFT_OPERAND_NONCE_EXIT;

////////// Make sure the result is marked as Nested
////////aplTypeRes = ARRAY_NESTED;      // Set previously
    } else
        lpPrimProtoLft = NULL;

    // If the derived function is not niladic, ...
    if (lptkRhtArg NE NULL)
        // Get right arg's global ptrs
        GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // Handle immediate right arg (lpMemHdrRht is NULL) or niladic derived function
    if (lpMemHdrRht EQ NULL)
    {
        if (lpPrimProtoLft NE NULL)
            lpYYRes2 = (*lpPrimProtoLft) (NULL,             // Ptr to left arg token
                                (LPTOKEN) lpYYFcnStrLft,    // Ptr to left operand function strand
                                          lptkRhtArg,       // Ptr to right arg token (may be NULL if niladic)
                                          lptkAxisOpr);     // Ptr to axis token
        else
            lpYYRes2 = ExecFuncStr_EM_YY (NULL,             // No left arg token
                                          lpYYFcnStrLft,    // Ptr to left operand function strand
                                          lptkRhtArg,       // Ptr to right arg token (may be NULL if niladic)
                                          lptkAxisOpr);     // Ptr to axis token
        if (lpYYRes2 NE NULL)
        {
            // Check for NoValue
            if (!IsTokenNoValue (&lpYYRes2->tkToken))
            {
                // Enclose the item
                lpYYRes = PrimFnMonLeftShoe_EM_YY (&lpYYFcnStrLft->tkToken,     // Ptr to function token
                                                   &lpYYRes2->tkToken,          // Ptr to right arg token
                                                    NULL);                      // Ptr to LPPRIMSPEC
                // We no longer need this storage
                FreeResult (lpYYRes2); YYFree (lpYYRes2); lpYYRes2 = NULL;
            } else
                lpYYRes = lpYYRes2;
        } else
            lpYYRes = NULL;

        goto NORMAL_EXIT;
    } // End IF

    //***************************************************************
    // From here on, the right arg is an HGLOBAL
    //***************************************************************

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRht, aplRankRht);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader    lpMemHdrRes
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRht;
    lpHeader->Rank       = aplRankRht;
#undef  lpHeader

    // Copy the dimensions from the right arg to the result
    CopyMemory (VarArrayBaseToDim (lpMemHdrRes),
                VarArrayBaseToDim (lpMemHdrRht),
                (APLU3264) aplRankRht * sizeof (APLDIM));

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);
    lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

    // Fill in the right arg token
    tkRhtArg.tkFlags.TknType   = TKT_VARIMMED;
////tkRhtArg.tkFlags.ImmType   =       // To be filled in below
////tkRhtArg.tkFlags.NoDisplay = FALSE; // Already zero from = {0}
////tkRhtArg.tkData.tkLongest  =       // To be filled in below
    tkRhtArg.tkCharIndex       = lpYYFcnStrLft->tkToken.tkCharIndex;

    // Handle prototypes separately
    if (IsEmpty (aplNELMRht))
    {
        // Split cases based upon the storage type of the right arg
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_FLOAT:
            case ARRAY_APA:
                tkRhtArg.tkFlags.ImmType = IMMTYPE_BOOL;
                tkRhtArg.tkData.tkBoolean = 0;

                // Execute the function on the arg token
                if (!ExecFuncOnToken_EM (&lpMemRes,             // Ptr to output storage
                                          NULL,                 // Ptr to left arg token
                                          lpYYFcnStrLft,        // Ptr to function strand
                                         &tkRhtArg,             // Ptr to right arg token
                                          lptkAxisLft,          // Ptr to left operand axis token
                                         &uValErrCnt,           // Ptr to VALUE ERROR counter
                                          lpPrimProtoLft))      // Ptr to left operand prototype function (may be NULL)
                    goto ERROR_EXIT;
                break;

            case ARRAY_CHAR:
                tkRhtArg.tkFlags.ImmType = IMMTYPE_CHAR;
                tkRhtArg.tkData.tkBoolean = L' ';

                // Execute the function on the arg token
                if (!ExecFuncOnToken_EM (&lpMemRes,             // Ptr to output storage
                                          NULL,                 // Ptr to left arg token
                                          lpYYFcnStrLft,        // Ptr to function strand
                                         &tkRhtArg,             // Ptr to right arg token
                                          lptkAxisLft,          // Ptr to left operand axis token
                                         &uValErrCnt,           // Ptr to VALUE ERROR counter
                                          lpPrimProtoLft))      // Ptr to left operand prototype function (may be NULL)
                    goto ERROR_EXIT;
                break;

            case ARRAY_NESTED:
            case ARRAY_HETERO:
                // Split cases based upon the ptr type
                switch (GetPtrTypeInd (lpMemRht))
                {
                    case PTRTYPE_STCONST:
                        // Set the token type
                        tkRhtArg.tkFlags.TknType = TKT_VARIMMED;

                        // Get the immediate type from the STE
                        tkRhtArg.tkFlags.ImmType = (*(LPAPLHETERO) lpMemRht)->stFlags.ImmType;

                        // Copy the value to the arg token
                        tkRhtArg.tkData.tkLongest = (*(LPAPLHETERO) lpMemRht)->stData.stLongest;

                        break;

                    case PTRTYPE_HGLOBAL:
                        // Set the token & immediate type
                        tkRhtArg.tkFlags.TknType = TKT_VARARRAY;
                        tkRhtArg.tkFlags.ImmType = IMMTYPE_ERROR;

                        // Copy the value to the arg token
                        tkRhtArg.tkData.tkGlbData = CopySymGlbInd_PTB ((LPAPLNESTED) lpMemRht);

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Execute the function on the arg token
                bRet =
                  ExecFuncOnToken_EM (&lpMemRes,            // Ptr to output storage
                                       NULL,                // Ptr to left arg token
                                       lpYYFcnStrLft,       // Ptr to function strand
                                      &tkRhtArg,            // Ptr to right arg token
                                       lptkAxisLft,         // Ptr to left operand axis token
                                      &uValErrCnt,          // Ptr to VALUE ERROR counter
                                       lpPrimProtoLft);     // Ptr to left operand prototype function (may be NULL)
                // Free the arg token
                FreeResultTkn (&tkRhtArg);

                if (!bRet)
                    goto ERROR_EXIT;
                break;

            defstop
                break;
        } // End SWITCH
    } else
    {
        // Translate ARRAY_APA args to ARRAY_INT
        if (IsSimpleAPA (aplTypeRht))
            tkRhtArg.tkFlags.ImmType = IMMTYPE_INT;
        else
            tkRhtArg.tkFlags.ImmType = TranslateArrayTypeToImmType (aplTypeRht);

        // Split cases based upon the storage type of the right arg
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
                // Initialize the bit mask
                uBitMask = BIT0;

                // Loop through the right arg
                for (uRht = 0; uRht < aplNELMRht; uRht++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Copy the value to the arg token
                    tkRhtArg.tkData.tkBoolean = (uBitMask & *(LPAPLBOOL) lpMemRht) ? TRUE : FALSE;

                    // Shift over the bit mask
                    uBitMask <<= 1;

                    // Check for end-of-byte
                    if (uBitMask EQ END_OF_BYTE)
                    {
                        uBitMask = BIT0;            // Start over
                        ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                    } // End IF

                    // Execute the function on the arg token
                    if (!ExecFuncOnToken_EM (&lpMemRes,             // Ptr to output storage
                                              NULL,                 // Ptr to left arg token
                                              lpYYFcnStrLft,        // Ptr to function strand
                                             &tkRhtArg,             // Ptr to right arg token
                                              lptkAxisLft,          // Ptr to left operand axis token
                                             &uValErrCnt,           // Ptr to VALUE ERROR counter
                                              lpPrimProtoLft))      // Ptr to left operand prototype function (may be NULL)
                        goto ERROR_EXIT;
                } // End FOR

                break;

            case ARRAY_INT:
                // Loop through the right arg
                for (uRht = 0; uRht < aplNELMRht; uRht++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Copy the value to the arg token
                    tkRhtArg.tkData.tkInteger = *((LPAPLINT) lpMemRht)++;

                    // Execute the function on the arg token
                    if (!ExecFuncOnToken_EM (&lpMemRes,             // Ptr to output storage
                                              NULL,                 // Ptr to left arg token
                                              lpYYFcnStrLft,        // Ptr to function strand
                                             &tkRhtArg,             // Ptr to right arg token
                                              lptkAxisLft,          // Ptr to left operand axis token
                                             &uValErrCnt,           // Ptr to VALUE ERROR counter
                                              lpPrimProtoLft))      // Ptr to left operand prototype function (may be NULL)
                        goto ERROR_EXIT;
                } // End FOR

                break;

            case ARRAY_FLOAT:
                // Loop through the right arg
                for (uRht = 0; uRht < aplNELMRht; uRht++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Copy the value to the arg token
                    tkRhtArg.tkData.tkFloat = *((LPAPLFLOAT) lpMemRht)++;

                    // Execute the function on the arg token
                    if (!ExecFuncOnToken_EM (&lpMemRes,             // Ptr to output storage
                                              NULL,                 // Ptr to left arg token
                                              lpYYFcnStrLft,        // Ptr to function strand
                                             &tkRhtArg,             // Ptr to right arg token
                                              lptkAxisLft,          // Ptr to left operand axis token
                                             &uValErrCnt,           // Ptr to VALUE ERROR counter
                                              lpPrimProtoLft))      // Ptr to left operand prototype function (may be NULL)
                        goto ERROR_EXIT;
                } // End FOR

                break;

            case ARRAY_CHAR:
                // Loop through the right arg
                for (uRht = 0; uRht < aplNELMRht; uRht++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Copy the value to the arg token
                    tkRhtArg.tkData.tkChar = *((LPAPLCHAR) lpMemRht)++;

                    // Execute the function on the arg token
                    if (!ExecFuncOnToken_EM (&lpMemRes,             // Ptr to output storage
                                              NULL,                 // Ptr to left arg token
                                              lpYYFcnStrLft,        // Ptr to function strand
                                             &tkRhtArg,             // Ptr to right arg token
                                              lptkAxisLft,          // Ptr to left operand axis token
                                             &uValErrCnt,           // Ptr to VALUE ERROR counter
                                              lpPrimProtoLft))      // Ptr to left operand prototype function (may be NULL)
                        goto ERROR_EXIT;
                } // End FOR

                break;

            case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemRht)
                // Get the APA parameters
                apaOff = lpAPA->Off;
                apaMul = lpAPA->Mul;
#undef  lpAPA
                // Loop through the right arg
                for (uRht = 0; uRht < aplNELMRht; uRht++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Copy the value to the arg token
                    tkRhtArg.tkData.tkInteger = apaOff + apaMul * uRht;

                    // Execute the function on the arg token
                    if (!ExecFuncOnToken_EM (&lpMemRes,             // Ptr to output storage
                                              NULL,                 // Ptr to left arg token
                                              lpYYFcnStrLft,        // Ptr to function strand
                                             &tkRhtArg,             // Ptr to right arg token
                                              lptkAxisLft,          // Ptr to left operand axis token
                                             &uValErrCnt,           // Ptr to VALUE ERROR counter
                                              lpPrimProtoLft))      // Ptr to left operand prototype function (may be NULL)
                        goto ERROR_EXIT;
                } // End FOR

                break;

            case ARRAY_NESTED:
                // Take into account nested prototypes
                if (IsNested (aplTypeRht))
                    aplNELMRht = max (aplNELMRht, 1);
            case ARRAY_HETERO:
                // Loop through the right arg
                for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLHETERO) lpMemRht)++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Split cases based upon the ptr type
                    switch (GetPtrTypeInd (lpMemRht))
                    {
                        case PTRTYPE_STCONST:
                            // Set the token type
                            tkRhtArg.tkFlags.TknType = TKT_VARIMMED;

                            // Get the immediate type from the STE
                            tkRhtArg.tkFlags.ImmType = (*(LPAPLHETERO) lpMemRht)->stFlags.ImmType;

                            // Copy the value to the arg token
                            tkRhtArg.tkData.tkLongest = (*(LPAPLHETERO) lpMemRht)->stData.stLongest;

                            break;

                        case PTRTYPE_HGLOBAL:
                            // Set the token & immediate type
                            tkRhtArg.tkFlags.TknType = TKT_VARARRAY;
                            tkRhtArg.tkFlags.ImmType = IMMTYPE_ERROR;

                            // Copy the value to the arg token
                            tkRhtArg.tkData.tkGlbData = CopySymGlbInd_PTB ((LPAPLNESTED) lpMemRht);

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    // Execute the function on the arg token
                    bRet =
                      ExecFuncOnToken_EM (&lpMemRes,            // Ptr to output storage
                                           NULL,                // Ptr to left arg token
                                           lpYYFcnStrLft,       // Ptr to function strand
                                          &tkRhtArg,            // Ptr to right arg token
                                           lptkAxisLft,         // Ptr to left operand axis token
                                          &uValErrCnt,          // Ptr to VALUE ERROR counter
                                           lpPrimProtoLft);     // Ptr to left operand prototype function (may be NULL)
                    // Free the arg token
                    FreeResultTkn (&tkRhtArg); tkRhtArg.tkData.tkGlbData = NULL;

                    if (!bRet)
                        goto ERROR_EXIT;
                } // End FOR

                break;

            case ARRAY_RAT:
                // Loop through the right arg
                for (uRht = 0; uRht < aplNELMRht; uRht++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Set the token & immediate type
                    tkRhtArg.tkFlags.TknType = TKT_VARARRAY;
                    tkRhtArg.tkFlags.ImmType = IMMTYPE_RAT;

                    // Copy the value to the arg token
                    tkRhtArg.tkData.tkGlbData =
                    lpSymGlb =
                      MakeGlbEntry_EM (ARRAY_RAT,                   // Entry type
                                       ((LPAPLRAT) lpMemRht)++,     // Ptr to the value
                                       TRUE,                        // TRUE iff we should initialize the target first
                                      &lpYYFcnStrOpr->tkToken);     // Ptr to function token
                    if (lpSymGlb EQ NULL)
                        goto ERROR_EXIT;
                    // Execute the function on the arg token
                    bRet =
                      ExecFuncOnToken_EM (&lpMemRes,                // Ptr to output storage
                                           NULL,                    // Ptr to left arg token
                                           lpYYFcnStrLft,           // Ptr to function strand
                                          &tkRhtArg,                // Ptr to right arg token
                                           lptkAxisLft,             // Ptr to left operand axis token
                                          &uValErrCnt,              // Ptr to VALUE ERROR counter
                                           lpPrimProtoLft);         // Ptr to left operand prototype function (may be NULL)
                    // Free the arg token
                    FreeResultTkn (&tkRhtArg); tkRhtArg.tkData.tkGlbData = NULL;

                    if (!bRet)
                        goto ERROR_EXIT;
                } // End FOR

                break;

            case ARRAY_VFP:
                // Loop through the right arg
                for (uRht = 0; uRht < aplNELMRht; uRht++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Set the token & immediate type
                    tkRhtArg.tkFlags.TknType = TKT_VARARRAY;
                    tkRhtArg.tkFlags.ImmType = IMMTYPE_VFP;

                    // Copy the value to the arg token
                    tkRhtArg.tkData.tkGlbData =
                    lpSymGlb =
                      MakeGlbEntry_EM (ARRAY_VFP,                   // Entry type
                                       ((LPAPLVFP) lpMemRht)++,     // Ptr to the value
                                       TRUE,                        // TRUE iff we should initialize the target first
                                      &lpYYFcnStrOpr->tkToken);     // Ptr to function token
                    if (lpSymGlb EQ NULL)
                        goto ERROR_EXIT;
                    // Execute the function on the arg token
                    bRet =
                      ExecFuncOnToken_EM (&lpMemRes,                // Ptr to output storage
                                           NULL,                    // Ptr to left arg token
                                           lpYYFcnStrLft,           // Ptr to function strand
                                          &tkRhtArg,                // Ptr to right arg token
                                           lptkAxisLft,             // Ptr to left operand axis token
                                          &uValErrCnt,              // Ptr to VALUE ERROR counter
                                           lpPrimProtoLft);         // Ptr to left operand prototype function (may be NULL)
                    // Free the arg token
                    FreeResultTkn (&tkRhtArg); tkRhtArg.tkData.tkGlbData = NULL;

                    if (!bRet)
                        goto ERROR_EXIT;
                } // End FOR

                break;

            defstop
                break;
        } // End SWITCH
    } // End IF/ELSE

    // Unlock the result global memory in case TypeDemote actually demotes
    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    // Check for VALUE ERROR
    if (uValErrCnt)
    {
        // Check for all VALUE ERRORs
        if (uValErrCnt EQ aplNELMRht)
        {
            // We no longer need this storage
            FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;

            // Make a PL_YYSTYPE NoValue entry
            lpYYRes = MakeNoValue_YY (&lpYYFcnStrOpr->tkToken);

            goto NORMAL_EXIT;
        } else
            goto VALUE_EXIT;
    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrLft->tkToken.tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken);

    goto NORMAL_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxisOpr);
    goto ERROR_EXIT;

LEFT_OPERAND_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

LEFT_OPERAND_NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

VALUE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
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
} // End PrimOpMonDieresisCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ExecFuncOnToken_EM
//
//  Execute a monadic or dyadic derived function strand
//    on a token or between tokens
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecFuncOnToken_EM"
#else
#define APPEND_NAME
#endif

UBOOL ExecFuncOnToken_EM
    (LPVOID      *lplpMemRes,           // Ptr to ptr to result memory
     LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStr,           // Ptr to function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxis,             // Ptr to function strand axis token (may be NULL)
     LPAPLUINT    lpuValErrCnt,         // Ptr to VALUE ERROR counter
     LPPRIMFNS    lpPrimProto)          // Ptr to left operand prototype function (may be NULL)

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result
    LPSYMENTRY   lpSymTmp;              // Ptr to temporary LPSYMENTRY

    // Execute the function on the arg token
    if (lpPrimProto NE NULL)
        // Note that we cast the function strand to LPTOKEN
        //   to bridge the two types of calls -- one to a primitive
        //   function which takes a function token, and one to a
        //   primitive operator which takes a function strand
        lpYYRes = (*lpPrimProto) (lptkLftArg,       // Ptr to left arg token
                        (LPTOKEN) lpYYFcnStr,       // Ptr to function strand
                                  lptkRhtArg,       // Ptr to right arg token
                                  lptkAxis);        // Ptr to axis token (may be NULL)
    else
        lpYYRes = ExecFuncStr_EM_YY (lptkLftArg,    // Ptr to left arg token
                                     lpYYFcnStr,    // Ptr to function strand
                                     lptkRhtArg,    // Ptr to right arg token
                                     lptkAxis);     // Ptr to axis token (may be NULL)
    // If it succeeded, ...
    if (lpYYRes NE NULL)
    {
        // Split cases based upon the result token type
        switch (lpYYRes->tkToken.tkFlags.TknType)
        {
            case TKT_VARIMMED:
                // Convert the immediate value into a symbol table constant,
                //   and save into the result
                *((LPAPLNESTED) *lplpMemRes)++ =
                lpSymTmp =
                  MakeSymEntry_EM (lpYYRes->tkToken.tkFlags.ImmType,    // Immediate type
                                  &lpYYRes->tkToken.tkData.tkLongest,   // Ptr to immediate value
                                  &lpYYFcnStr->tkToken);                // Ptr to function token
                if (lpSymTmp EQ NULL)
                    goto ERROR_EXIT;
                break;

            case TKT_VARARRAY:
                // Copy (increment the reference count of) the global object,
                //   and save into the result
                *((LPAPLNESTED) *lplpMemRes)++ =
                  CopySymGlbDir_PTB (lpYYRes->tkToken.tkData.tkGlbData);
                break;

            case TKT_VARNAMED:
                // Check for NoValue
                Assert (IsTokenNoValue (&lpYYRes->tkToken));

                // Free the YYRes (but not the storage)
                YYFree (lpYYRes); lpYYRes = NULL;

                // Increment the VALUE ERROR counter
                (*lpuValErrCnt)++;

                return TRUE;

            defstop
                break;
        } // End SWITCH

        // Free the result of the function execution
        FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;

        return TRUE;
    } // End IF

    // Fall through to error handling code

ERROR_EXIT:
    if (lpYYRes NE NULL)
    {
        // Free the result of the function execution
        FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;
    } // End IF

    return FALSE;
} // End ExecFuncOnToken_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpDydDieresis_EM_YY
//
//  Primitive operator for dyadic derived function from Dieresis ("each")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydDieresis_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpDydDieresisCommon_EM_YY (lptkLftArg,           // Ptr to left arg token
                                          lpYYFcnStrOpr,        // Ptr to operator function strand
                                          lptkRhtArg,           // Ptr to right arg token
                                          FALSE);               // TRUE iff prototyping
} // End PrimOpDydDieresis_EM_YY


//***************************************************************************
//  $PrimOpDydDieresisCommon_EM_YY
//
//  Primitive operator for dyadic derived function from Dieresis ("each")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydDieresisCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpDydDieresisCommon_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     UBOOL        bPrototyping)         // TRUE iff prototyping

{
    APLSTYPE          aplTypeLft,           // Left arg storage type
                      aplTypeRht,           // Right ...
                      aplTypeRes;           // Result ...
    APLNELM           aplNELMLft,           // Left arg NELM
                      aplNELMRht,           // Right ...
                      aplNELMRes,           // Result ...
                      aplNELMAxis;          // Axis ...
    APLRANK           aplRankLft,           // Left arg rank
                      aplRankRht,           // Right ...
                      aplRankRes;           // Result ...
    HGLOBAL           hGlbLft = NULL,       // Left arg global memory handle
                      hGlbRht = NULL,       // Right ...
                      hGlbRes = NULL,       // Result   ...
                      hGlbAxis = NULL,      // Axis     ...
                      hGlbWVec = NULL,      // Weighting vector ...
                      hGlbOdo = NULL;       // Odometer ...
    LPAPLUINT         lpMemAxisHead = NULL, // Ptr to axis values, fleshed out by CheckAxis_EM
                      lpMemAxisTail = NULL, // Ptr to grade up of AxisHead
                      lpMemOdo = NULL,      // Ptr to odometer global memory
                      lpMemWVec = NULL;     // Ptr to weighting vector ...
    LPVARARRAY_HEADER lpMemHdrLft = NULL,   // Ptr to left arg header
                      lpMemHdrRht = NULL,   // ...    right ...
                      lpMemHdrRes = NULL;   // ...    result   ...
    LPVOID            lpMemLft,             // Ptr to left arg global memory
                      lpMemRht,             // Ptr to right ...
                      lpMemRes;             // Ptr to result   ...
    LPAPLDIM          lpMemDimRes;          // Ptr to result dimensions
    UBOOL             bRet = TRUE,          // TRUE iff result is valid
                      bLftIdent,            // TRUE iff the function has a left identity element and the Axis tail is valid
                      bRhtIdent;            // ...                         right ...
    APLUINT           uLft,                 // Left arg loop counter
                      uRht,                 // Right ...
                      uRes,                 // Result   ...
                      uValErrCnt = 0,       // VALUE ERROR counter
                      ByteAlloc;            // # bytes to allocate
    APLINT            apaOffLft,            // Left arg APA offset
                      apaMulLft,            // ...          multiplier
                      apaOffRht,            // Right arg APA offset
                      apaMulRht,            // ...           multiplier
                      iDim;                 // Dimension loop counter
    IMM_TYPES         immType;              // Immediate type
    LPPL_YYSTYPE      lpYYRes = NULL,       // Ptr to the result
                      lpYYFcnStrLft;        // Ptr to left operand function strand
    TOKEN             tkLftArg = {0},       // Left arg token
                      tkRhtArg = {0};       // Right ...
    LPTOKEN           lptkAxisOpr,          // Ptr to operator axis token (may be NULL)
                      lptkAxisLft;          // Ptr to left operand axis token (may be NULL)
    LPPRIMFNS         lpPrimProtoLft;       // Ptr to left operand function strand (may be NULL if not prototyping)
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    LPPRIMFLAGS       lpPrimFlagsLft;       // Ptr to left operand PrimFlags entry

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Check for operator axis token
    lptkAxisOpr = CheckAxisOper (lpYYFcnStrOpr);

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = GetMonLftOper (lpYYFcnStrOpr, lptkAxisOpr);

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken)
     || IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_OPERAND_SYNTAX_EXIT;

    // Check for left operand axis token
    lptkAxisLft = CheckAxisOper (lpYYFcnStrLft);

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // In case we're doing (1 1 1{rho}1)f{each}{iota}4
    //   set the rank of the result to the rank of
    //   the non-singleton argument.

    // Split cases based upon the arg's NELM
    if (!IsSingleton (aplNELMLft) && IsSingleton (aplNELMRht))
        aplRankRes = aplRankLft;
    else
    if (!IsSingleton (aplNELMRht) && IsSingleton (aplNELMLft))
        aplRankRes = aplRankRht;
    else
        // The rank of the result is the larger of the two args
        aplRankRes = max (aplRankLft, aplRankRht);

    // Check for operator axis present
    if (lptkAxisOpr NE NULL)
    {
        // Check the axis values, fill in # elements in axis
        if (!CheckAxis_EM (lptkAxisOpr,     // The operator axis token
                           aplRankRes,      // All values less than this
                           FALSE,           // TRUE iff scalar or one-element vector only
                           FALSE,           // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // TRUE iff fractional values allowed
                           NULL,            // Return last axis value
                          &aplNELMAxis,     // Return # elements in axis vector
                          &hGlbAxis))       // Return HGLOBAL with APLINT axis values
            return NULL;
        // Lock the memory to get a ptr to it
        lpMemAxisHead = MyGlobalLockInt (hGlbAxis);

        // Get pointer to the axis tail (where the [X] values are)
        lpMemAxisTail = &lpMemAxisHead[aplRankRes - aplNELMAxis];
    } else
        // No axis is the same as all axes
        aplNELMAxis = aplRankRes;

    // The result storage type is assumed to be NESTED,
    //   but we'll call TypeDemote at the end just in case.
    aplTypeRes = ARRAY_NESTED;

    // Get left and right arg's global ptrs
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // Get a ptr to the Primitive Function Flags
    lpPrimFlagsLft = GetPrimFlagsPtr (&lpYYFcnStrLft->tkToken);

    // Set the identity element bits
    bLftIdent = lpPrimFlagsLft->bLftIdent
             && (lpMemAxisTail NE NULL);
    bRhtIdent = lpPrimFlagsLft->bRhtIdent
             && (lpMemAxisTail NE NULL);

    // Check for RANK and LENGTH ERRORs
    if (!CheckRankLengthError_EM (aplRankRes,       // Result rank
                                  aplRankLft,       // Left arg rank
                                  aplNELMLft,       // Left arg NELM
                                  lpMemHdrLft,      // Ptr to left arg memory Sig.nature
                                  aplRankRht,       // Right arg rank
                                  aplNELMRht,       // Rigth arg NELM
                                  lpMemHdrRht,      // Ptr to right arg memory Sig.nature
                                  aplNELMAxis,      // Axis NELM
                                  lpMemAxisTail,    // Ptr to grade up of AxisHead
                                  bLftIdent,        // TRUE iff the function has a left identity element and the Axis tail is valid
                                  bRhtIdent,        // ...                         right ...
                                 &lpYYFcnStrOpr->tkToken))  // Ptr to function token
        goto ERROR_EXIT;

    // The NELM of the result is the larger of the two args
    //   unless one is empty
    if (IsEmpty (aplNELMLft) || IsEmpty (aplNELMRht))
        aplNELMRes = 0;
    else
        aplNELMRes = max (aplNELMLft, aplNELMRht);

    //***************************************************************
    //  Handle prototypes separately
    //***************************************************************
    if (IsEmpty (aplNELMRes)
     || bPrototyping)
    {
        // Get the appropriate prototype function ptr
        lpPrimProtoLft = GetPrototypeFcnPtr (&lpYYFcnStrLft->tkToken);
        if (lpPrimProtoLft EQ NULL)
            goto LEFT_OPERAND_NONCE_EXIT;

////////// Make sure the result is marked as Nested
////////aplTypeRes = ARRAY_NESTED;      // Set previously
    } else
        lpPrimProtoLft = NULL;

    // Allocate space for result
    if (!PrimScalarFnDydAllocate_EM (&lpYYFcnStrLft->tkToken,
                                     &hGlbRes,
                                      lpMemHdrLft,  // Ptr to left arg memory Sig.nature
                                      lpMemHdrRht,  // ...    right ...
                                      aplRankLft,
                                      aplRankRht,
                                     &aplRankRes,
                                      aplTypeRes,
                                      bLftIdent,    // TRUE iff the function has a left identity element and the Axis tail is valid
                                      bRhtIdent,    // ...                         right ...
                                      aplNELMLft,
                                      aplNELMRht,
                                      aplNELMRes))
        goto ERROR_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (hGlbRes);

    // Fill in the arg tokens
    tkLftArg.tkCharIndex =
    tkRhtArg.tkCharIndex = lpYYFcnStrLft->tkToken.tkCharIndex;

    // If the left arg is immediate, fill in the token
    if (lpMemHdrLft EQ NULL)
    {
        tkLftArg.tkFlags.TknType = TKT_VARIMMED;
        GetFirstValueToken (lptkLftArg,                 // Ptr to left arg token
                            NULL,                       // Ptr to integer result
                            NULL,                       // Ptr to float ...
                            NULL,                       // Ptr to WCHAR ...
                           &tkLftArg.tkData.tkLongest,  // Ptr to longest ...
                            NULL,                       // Ptr to lpSym/Glb ...
                           &immType,                    // Ptr to ...immediate type ...
                            NULL);                      // Ptr to array type ...
        tkLftArg.tkFlags.ImmType = immType;
    } else
    // Otherwise, skip over the header and dimensions to the data
        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);

    // If the right arg is immediate, fill in the token
    if (lpMemHdrRht EQ NULL)
    {
        tkRhtArg.tkFlags.TknType = TKT_VARIMMED;
        GetFirstValueToken (lptkRhtArg,                 // Ptr to right arg token
                            NULL,                       // Ptr to integer result
                            NULL,                       // Ptr to float ...
                            NULL,                       // Ptr to WCHAR ...
                           &tkRhtArg.tkData.tkLongest,  // Ptr to longest ...
                            NULL,                       // Ptr to lpSym/Glb ...
                           &immType,                    // Ptr to ...immediate type ...
                            NULL);                      // Ptr to array type ...
        tkRhtArg.tkFlags.ImmType = immType;
    } else
    // Otherwise, skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

    // If the left arg is APA, ...
    if (IsSimpleAPA (aplTypeLft))
    {
#define lpAPA       ((LPAPLAPA) lpMemLft)
        // Get the APA parameters
        apaOffLft = lpAPA->Off;
        apaMulLft = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // If the right arg is APA, ...
    if (IsSimpleAPA (aplTypeRht))
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        // Get the APA parameters
        apaOffRht = lpAPA->Off;
        apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // Skip over the header to the dimensions
    lpMemDimRes = VarArrayBaseToDim (lpMemHdrRes);

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Handle axis if present
    if (aplNELMAxis NE aplRankRes)
    {
        // Calculate space needed for the weighting vector
        ByteAlloc = aplRankRes * sizeof (APLUINT);

        // Check for overflow
        if (ByteAlloc NE (APLU3264) ByteAlloc)
            goto WSFULL_EXIT;

        //***************************************************************
        // Allocate space for the weighting vector which is
        //   {times}{backscan}1{drop}({rho}Z),1
        //***************************************************************
        hGlbWVec = DbgGlobalAlloc (GHND, (APLU3264) ByteAlloc);
        if (hGlbWVec EQ NULL)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemWVec = MyGlobalLock000 (hGlbWVec);

        // Loop through the dimensions of the result in reverse
        //   order {backscan} and compute the cumulative product
        //   (weighting vector).
        // Note we use a signed index variable because we're
        //   walking backwards and the test against zero must be
        //   made as a signed variable.
        for (uRes = 1, iDim = aplRankRes - 1; iDim >= 0; iDim--)
        {
            lpMemWVec[iDim] = uRes;
            uRes *= lpMemDimRes[lpMemAxisHead[iDim]];
        } // End FOR

        // Calculate space needed for the odometer array
        ByteAlloc = aplRankRes * sizeof (APLUINT);

        // Check for overflow
        if (ByteAlloc NE (APLU3264) ByteAlloc)
            goto WSFULL_EXIT;

        //***************************************************************
        // Allocate space for the odometer array, one value per dimension
        //   in the right arg, with values initially all zero (thanks to GHND).
        //***************************************************************
        hGlbOdo = DbgGlobalAlloc (GHND, (APLU3264) ByteAlloc);
        if (hGlbOdo EQ NULL)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemOdo = MyGlobalLock000 (hGlbOdo);
    } // End IF

    // Take into account nested prototypes
    if (IsNested (aplTypeLft))
        aplNELMLft = max (aplNELMLft, 1);
    if (IsNested (aplTypeRht))
        aplNELMRht = max (aplNELMRht, 1);
    if (IsNested (aplTypeRes))
        aplNELMRes = max (aplNELMRes, 1);

    // Loop through the result
    for (uRes = 0; uRes < aplNELMRes; uRes++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // If there's an axis, ...
        if (lptkAxisOpr NE NULL
         && aplNELMAxis NE aplRankRes)
            // Calculate the left and right argument indices
            CalcLftRhtArgIndices (uRes,
                                  aplRankRes,
                                 &uLft,
                                  aplRankLft,
                                 &uRht,
                                  aplRankRht,
                                  bLftIdent,            // TRUE iff the function has a left identity element and the Axis tail is valid
                                  bRhtIdent,            // ...                         right ...
                                  aplNELMAxis,
                                  lpMemAxisHead,        // Ptr to axis values, fleshed out by CheckAxis_EM
                                  lpMemOdo,
                                  lpMemWVec,
                                  lpMemDimRes);
        else
        {
            uLft = IsEmpty (aplNELMLft) ? uRes : uRes % aplNELMLft;
            uRht = IsEmpty (aplNELMRht) ? uRes : uRes % aplNELMRht;
        } // End IF/ELSE

        // Initialize
        bRet = TRUE;

        // If the left arg is not immediate, get the next value
        if (lpMemHdrLft NE NULL)
            // Get the next value from the left arg
            bRet &=
              GetNextValueMemIntoToken (uLft,       // Index to use
                                        lpMemLft,   // Ptr to global memory object to index
                                        aplTypeLft, // Storage type of the arg
                                        aplNELMLft, // NELM         ...
                                        apaOffLft,  // APA offset (if needed)
                                        apaMulLft,  // APA multiplier (if needed)
                                       &tkLftArg);  // Ptr to token in which to place the value
        // If the right arg is not immediate, get the next value
        if (lpMemHdrRht NE NULL)
            // Get the next value from the right arg
            bRet &=
              GetNextValueMemIntoToken (uRht,       // Index to use
                                        lpMemRht,   // Ptr to global memory object to index
                                        aplTypeRht, // Storage type of the arg
                                        aplNELMRht, // NELM         ...
                                        apaOffRht,  // APA offset (if needed)
                                        apaMulRht,  // APA multiplier (if needed)
                                       &tkRhtArg);  // Ptr to token in which to place the value
        // If there's been no error so far, ...
        if (bRet)
            // Execute the function strand between the left & right arg tokens
            bRet =
              ExecFuncOnToken_EM (&lpMemRes,        // Ptr to output storage
                                  &tkLftArg,        // Ptr to left arg token
                                   lpYYFcnStrLft,   // Ptr to function strand
                                  &tkRhtArg,        // Ptr to right arg token
                                   lptkAxisLft,     // Ptr to left operand axis token
                                  &uValErrCnt,      // Ptr to VALUE ERROR counter
                                   lpPrimProtoLft); // Ptr to left operand prototype function
        // Free the left & right arg tokens
        if (lpMemHdrLft NE NULL)
            FreeResultTkn (&tkLftArg);
        if (lpMemHdrRht NE NULL)
            FreeResultTkn (&tkRhtArg);
        if (!bRet)
            goto ERROR_EXIT;
    } // End FOR

    // Unlock the result global memory in case TypeDemote actually demotes
    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    // Check for VALUE ERROR
    if (uValErrCnt)
    {
        // Check for all VALUE ERRORs
        if (uValErrCnt EQ aplNELMRht)
        {
            // We no longer need this storage
            FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;

            // Make a PL_YYSTYPE NoValue entry
            lpYYRes = MakeNoValue_YY (&lpYYFcnStrOpr->tkToken);

            goto NORMAL_EXIT;
        } else
            goto VALUE_EXIT;
    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken);

    goto NORMAL_EXIT;

LEFT_OPERAND_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

LEFT_OPERAND_NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

VALUE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    bRet = FALSE;

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
    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbWVec, lpMemWVec);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbOdo, lpMemOdo);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbAxis, lpMemAxisHead);

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

    // If we failed and there's a result, free it
    if (!bRet && lpYYRes)
    {
        YYFree (lpYYRes); lpYYRes = NULL;
    } // End IF/ELSE

    return lpYYRes;
} // End PrimOpDydDieresis_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: po_dieresis.c
//***************************************************************************
