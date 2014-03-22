//***************************************************************************
//  NARS2000 -- Primitive Operator -- JotDot
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
//  $PrimOpJotDot_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    monadic operator JotDot (ERROR and "outer product")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpJotDot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpJotDot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ INDEX_JOTDOT);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (&lpYYFcnStrOpr->tkToken APPEND_NAME_ARG);

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return PrimOpMonJotDot_EM_YY (lpYYFcnStrOpr,    // Ptr to operator function strand
                                      lptkRhtArg);      // Ptr to right arg token
    else
        return PrimOpDydJotDot_EM_YY (lptkLftArg,       // Ptr to left arg token
                                      lpYYFcnStrOpr,    // Ptr to operator function strand
                                      lptkRhtArg);      // Ptr to right arg token
} // End PrimOpJotDot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoOpJotDot_EM_YY
//
//  Generate a prototype for the derived functions from
//    monadic operator JotDot (ERROR and "outer product")
//***************************************************************************

LPPL_YYSTYPE PrimProtoOpJotDot_EM_YY
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
        return PrimOpMonJotDotCommon_EM_YY (lpYYFcnStrOpr,      // Ptr to operator function strand
                                            lptkRhtArg,         // Ptr to right arg token
                                            TRUE);              // TRUE iff prototyping
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************
        return PrimOpDydJotDotCommon_EM_YY (lptkLftArg,         // Ptr to left arg token
                                            lpYYFcnStrOpr,      // Ptr to operator function strand
                                            lptkRhtArg,         // Ptr to right arg token
                                            TRUE);              // TRUE iff prototyping
} // End PrimProtoOpJotDot_EM_YY


//***************************************************************************
//  $PrimIdentOpJotDot_EM_YY
//
//  Generate an identity element for the primitive operator dyadic JotDot
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimIdentOpJotDot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimIdentOpJotDot_EM_YY
    (LPTOKEN      lptkRhtOrig,      // Ptr to original right arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,    // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,       // Ptr to right arg token
     LPTOKEN      lptkAxisOpr)      // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYFcnStrRht;     // Ptr to right operand function strand
    HGLOBAL      hGlbMFO;               // Magic function/operator global memory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory

    // The right arg is the prototype item from
    //   the original empty arg.

    Assert (lptkRhtOrig   NE NULL);
    Assert (lpYYFcnStrOpr NE NULL);
    Assert (lptkRhtArg    NE NULL);

    //***************************************************************
    // This operator is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxisOpr NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // The (left/right -- depends upon f) identity function for dyadic JotDot
    //   (L {jot}.f R) ("outer product") is
    //   f/0{rho} R.

    // Set ptr to right operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrRht = &lpYYFcnStrOpr[1 + (lptkAxisOpr NE NULL)];

    // Ensure the right operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrRht->tkToken)
     || IsTknFillJot (&lpYYFcnStrRht->tkToken))
        goto RIGHT_OPERAND_SYNTAX_EXIT;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the magic function/operator global memory handles
    hGlbMFO = lpMemPTD->hGlbMFO[MFOE_IdnJotDot];

    return
      ExecuteMagicOperator_EM_YY (NULL,                     // Ptr to left arg token
                                 &lpYYFcnStrOpr->tkToken,   // Ptr to function token
                                  lpYYFcnStrRht,            // Ptr to left operand function strand
                                  lpYYFcnStrOpr,            // Ptr to function strand
                                  NULL,                     // Ptr to right operand function strand (may be NULL)
                                  lptkRhtArg,               // Ptr to right arg token
                                  lptkAxisOpr,              // Ptr to axis token
                                  hGlbMFO,                  // Magic function/operator global memory handle
                                  NULL,                     // Ptr to HSHTAB struc (may be NULL)
                                  LINENUM_ID);              // Starting line # type (see LINE_NUMS)
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxisOpr);
    return NULL;

RIGHT_OPERAND_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    return NULL;
} // End PrimIdentOpJotDot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  Magic function/operator for identity function from the
//    outer product operator
//   f/0{rho} R.
//***************************************************************************

#include "mf_jotdot.h"


//***************************************************************************
//  $PrimOpMonJotDot_EM_YY
//
//  Primitive operator for monadic derived function from JotDot (ERROR)
//***************************************************************************

LPPL_YYSTYPE PrimOpMonJotDot_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpMonJotDotCommon_EM_YY (lpYYFcnStrOpr,      // Ptr to operator function strand
                                        lptkRhtArg,         // Ptr to right arg token
                                        FALSE);             // TRUE iff prototyping
} // End PrimOpMonJotDot_EM_YY


//***************************************************************************
//  $PrimOpMonJotDotCommon_EM_YY
//
//  Primitive operator for monadic derived function from JotDot (ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpMonJotDotCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpMonJotDotCommon_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     UBOOL        bPrototyping)         // TRUE iff prototyping

{
    return PrimFnValenceError_EM (&lpYYFcnStrOpr->tkToken APPEND_NAME_ARG);
} // End PrimOpJotDotCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpDydJotDot_EM_YY
//
//  Primitive operator for dyadic derived function from JotDot ("outer product")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydJotDot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStr,           // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpDydJotDotCommon_EM_YY (lptkLftArg,     // Ptr to left arg token
                                        lpYYFcnStr,     // Ptr to operator function strand
                                        lptkRhtArg,     // Ptr to right arg token
                                        FALSE);         // TRUE iff prototoyping
} // End PrimOpDydJotDot_EM_YY


//***************************************************************************
//  $PrimOpDydJotDotCommon_EM_YY
//
//  Primitive operator for dyadic derived function from JotDot ("outer product")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydJotDotCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpDydJotDotCommon_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     UBOOL        bPrototyping)         // TRUE iff prototyping

{
    APLSTYPE      aplTypeLft,           // Left arg storage type
                  aplTypeRht,           // Right ...
                  aplTypeNew,           // New result   ...
                  aplTypeRes;           // Result   ...
    APLNELM       aplNELMLft,           // Left arg NELM
                  aplNELMRht,           // Right ...
                  aplNELMRes;           // Result   ...
    APLRANK       aplRankLft,           // Left arg rank
                  aplRankRht,           // Right ...
                  aplRankRes;           // Result   ...
    APLUINT       ByteRes,              // # bytes in the result
                  uLft,                 // Left arg loop counter
                  uRht,                 // Right ...
                  uRes,                 // Result   ...
                  uValErrCnt = 0;       // VALUE ERROR counter
    HGLOBAL       hGlbLft = NULL,       // Left arg global memory handle
                  hGlbRht = NULL,       // Right ...
                  hGlbRes = NULL;       // Result   ...
    LPVOID        lpMemLft = NULL,      // Ptr to left arg global memory
                  lpMemRht = NULL,      // Ptr to right ...
                  lpMemRes = NULL;      // Ptr to result   ...
    UBOOL         bRet = TRUE;          // TRUE iff result is valid
    TOKEN         tkLftArg = {0},       // Left arg token
                  tkRhtArg = {0};       // Right ...
    IMM_TYPES     immType,              // Immediate type
                  immTypeRes;           // Result immediate type
    APLINT        apaOffLft,            // Left arg APA offset
                  apaMulLft,            // ...          multiplier
                  apaOffRht,            // Right arg APA offset
                  apaMulRht;            // ...           multiplier
    LPPL_YYSTYPE  lpYYFcnStrRht,        // Ptr to right operand function strand
                  lpYYRes = NULL;       // Ptr to the result
    LPPRIMFNS     lpPrimProtoRht;       // Ptr to right operand prototype function
    LPTOKEN       lptkAxisOpr,          // Ptr to operator axis token (may be NULL)
                  lptkAxisRht;          // Ptr to right operand axis token (may be NULL)
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    LPPRIMSPEC    lpPrimSpec;           // Ptr to local PRIMSPEC
    LPPRIMFLAGS   lpPrimFlagsRht;       // Ptr to right operand PrimFlags entry

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for operator axis token
    lptkAxisOpr = CheckAxisOper (lpYYFcnStrOpr);

    //***************************************************************
    // The derived functions from this operator are not sensitive to
    //   the axis operator, so signal a syntax error if present
    //***************************************************************
    if (lptkAxisOpr NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Set ptr to right operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrRht = &lpYYFcnStrOpr[1 + (lptkAxisOpr NE NULL)];

    // Ensure the right operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrRht->tkToken)
     || IsTknFillJot (&lpYYFcnStrRht->tkToken))
        goto RIGHT_OPERAND_SYNTAX_EXIT;

    // The result NELM is the product of the left & right NELMs
    aplNELMRes = _imul64 (aplNELMLft, aplNELMRht, &bRet);
    if (!bRet)
        goto WSFULL_EXIT;

    // Check for right operand axis token
    lptkAxisRht = CheckAxisOper (lpYYFcnStrRht);

    // Handle prototypes separately
    if (IsEmpty (aplNELMRes)
     || bPrototyping)
    {
        // Get the appropriate prototype function ptr
        lpPrimProtoRht = GetPrototypeFcnPtr (&lpYYFcnStrRht->tkToken);
        if (!lpPrimProtoRht)
            goto RIGHT_OPERAND_NONCE_EXIT;
    } else
        lpPrimProtoRht = NULL;

    // Get a ptr to the Primitive Function Flags
    lpPrimFlagsRht = GetPrimFlagsPtr (&lpYYFcnStrRht->tkToken);

    // The rank of the result is the sum of the left & right ranks
    aplRankRes = aplRankLft + aplRankRht;

    // If the function is scalar dyadic,
    //   and there's no right operand axis,
    //   and both args are simple non-hetero, ...
    if (lpPrimFlagsRht->DydScalar
     && lptkAxisRht EQ NULL
     && IsSimpleNHGlbNum (aplTypeLft)
     && IsSimpleNHGlbNum (aplTypeRht))
    {
        // Get the corresponding lpPrimSpec
        lpPrimSpec = PrimSpecTab[SymTrans (&lpYYFcnStrRht->tkToken)];

        // Calculate the storage type of the result
        aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (aplNELMLft,
                                                   &aplTypeLft,
                                                   &lpYYFcnStrRht->tkToken,
                                                    aplNELMRht,
                                                   &aplTypeRht);
        if (IsErrorType (aplTypeRes))
            goto DOMAIN_EXIT;
        // For the moment, APA is treated as INT
        if (IsSimpleAPA (aplTypeRes))
            aplTypeRes = ARRAY_INT;
    } else
        // The result storage type is assumed to be NESTED,
        //   but we'll call TypeDemote at the end just in case.
        aplTypeRes = ARRAY_NESTED;

    // Save as immediate type
    immTypeRes = TranslateArrayTypeToImmType (aplTypeRes);
RESTART_JOTDOT:
    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result.
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Get left and right arg's global ptrs
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemRes);
    if (lpMemLft)
        lpMemLft = VarArrayBaseToDim (lpMemLft);
    if (lpMemRht)
        lpMemRht = VarArrayBaseToDim (lpMemRht);

    //***************************************************************
    // Copy the dimensions from the left arg
    //   to the result's dimension
    //***************************************************************
    if (lpMemLft)
    for (uRes = 0; uRes < aplRankLft; uRes++)
        *((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemLft)++;

    //***************************************************************
    // Copy the dimensions from the right arg
    //   to the result's dimension
    //***************************************************************
    if (lpMemRht)
    for (uRes = 0; uRes < aplRankRht; uRes++)
        *((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemRht)++;

    // lpMemRes now points to the result's data
    // lpMemLft now points to the left arg's data
    // lpMemRht now points to the right arg's data

    // Fill in the arg tokens
    tkLftArg.tkCharIndex =
    tkRhtArg.tkCharIndex = lpYYFcnStrOpr->tkToken.tkCharIndex;

    // If the left arg is immediate, fill in the token
    if (lpMemLft EQ NULL)
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
        lpMemLft = &tkLftArg.tkData.tkLongest;
    } // End IF

    // If the right arg is immediate, fill in the token
    if (lpMemRht EQ NULL)
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
        lpMemRht = &tkRhtArg.tkData.tkLongest;
    } // End IF

    // If the left arg is APA, fill in the offset and multiplier
    if (IsSimpleAPA (aplTypeLft))
    {
#define lpAPA       ((LPAPLAPA) lpMemLft)
        // Get the APA parameters
        apaOffLft = lpAPA->Off;
        apaMulLft = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // If the right arg is APA, fill in the offset and multiplier
    if (IsSimpleAPA (aplTypeRht))
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        // Get the APA parameters
        apaOffRht = lpAPA->Off;
        apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // Take into account nested prototypes
    if (IsNested (aplTypeRht))
        aplNELMRht = max (aplNELMRht, 1);
    if (IsNested (aplTypeLft))
        aplNELMLft = max (aplNELMLft, 1);

    // Handle prototypes
    if (IsEmpty (aplNELMRes)
     && IsNested (aplTypeRes))
    {
        if (!PrimOpDydJotDotProto_EM (&lpMemRes,        // Ptr to ptr to result global memory
                                       lptkLftArg,      // Ptr to left arg tokwn
                                       lptkRhtArg,      // Ptr to right arg token
                                       lpYYFcnStrRht,   // Ptr to right operand function strand
                                       lpPrimProtoRht,  // Ptr to right operand prototype function
                                       lpbCtrlBreak))   // Ptr to Ctrl-Break flag
            goto ERROR_EXIT;
    } else
    // If the function is scalar dyadic,
    //   and there's no right operand axis,
    //   and both args are simple non-hetero or global numeric, ...
    if (lpPrimFlagsRht->DydScalar
     && lptkAxisRht EQ NULL
     && IsSimpleNHGlbNum (aplTypeLft)
     && IsSimpleNHGlbNum (aplTypeRht))
    {
        TOKEN    tkRes = {0};
        APLINT   aplIntegerLft,
                 aplIntegerRht;
        APLFLOAT aplFloatLft,
                 aplFloatRht;
        APLCHAR  aplCharLft,
                 aplCharRht;
        UINT     uBitIndex;             // Bit index for looping through Boolean result
        HGLOBAL  lpSymGlbLft = NULL,    // Ptr to left arg global memory
                 lpSymGlbRht = NULL;    // ...    right ...
        LPVOID   lpMemSub;              // Ptr to temp global memory

        // Initialize the bit index
        uBitIndex = 0;

        // Loop through the left arg
        for (uLft = 0; uLft < aplNELMLft; uLft++)
        {
            // Split cases based upon the left arg's storage type
            switch (aplTypeLft)
            {
                case ARRAY_BOOL:
                    aplIntegerLft = GetNextInteger (lpMemLft, aplTypeLft, uLft);
                    aplFloatLft   = (APLFLOAT) aplIntegerLft;   // In case of type promotion

                    break;

                case ARRAY_INT:
                    aplIntegerLft = ((LPAPLINT)   lpMemLft)[uLft];
                    aplFloatLft   = (APLFLOAT) aplIntegerLft;   // In case of type promotion

                    break;

                case ARRAY_APA:
                    aplIntegerLft = apaOffLft + apaMulLft * uLft;
                    aplFloatLft   = (APLFLOAT) aplIntegerLft;   // In case of type promotion

                    break;

                case ARRAY_FLOAT:
                    aplFloatLft   = ((LPAPLFLOAT) lpMemLft)[uLft];

                    break;

                case ARRAY_CHAR:
                    aplCharLft    = ((LPAPLCHAR)  lpMemLft)[uLft];

                    break;

                case ARRAY_RAT:
                    lpSymGlbLft   = &((LPAPLRAT)  lpMemLft)[uLft];

                    break;

                case ARRAY_VFP:
                    lpSymGlbLft   = &((LPAPLVFP)  lpMemLft)[uLft];

                    break;

                defstop
                    break;
            } // End SWITCH

            // Loop through the right arg
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (*lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Split cases based upon the right arg's storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:
                        aplIntegerRht = GetNextInteger (lpMemRht, aplTypeRht, uRht);
                        aplFloatRht   = (APLFLOAT) aplIntegerRht;   // In case of type promotion

                        break;

                    case ARRAY_INT:
                        aplIntegerRht = ((LPAPLINT)   lpMemRht)[uRht];
                        aplFloatRht   = (APLFLOAT) aplIntegerRht;   // In case of type promotion

                        break;

                    case ARRAY_APA:
                        aplIntegerRht = apaOffRht + apaMulRht * uRht;
                        aplFloatRht   = (APLFLOAT) aplIntegerRht;   // In case of type promotion

                        break;

                    case ARRAY_FLOAT:
                        aplFloatRht   = ((LPAPLFLOAT) lpMemRht)[uRht];

                        break;

                    case ARRAY_CHAR:
                        aplCharRht    = ((LPAPLCHAR)  lpMemRht)[uRht];

                        break;

                    case ARRAY_RAT:
                        lpSymGlbRht   = &((LPAPLRAT)  lpMemRht)[uRht];

                        break;

                    case ARRAY_VFP:
                        lpSymGlbRht   = &((LPAPLVFP)  lpMemRht)[uRht];

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Compare the two items
                if (PrimFnDydSiScSiScSub_EM (&tkRes,
                                             &lpYYFcnStrRht->tkToken,
                                              NULL,                     // Result global memory handle (may be NULL)
                                              aplTypeRes,
                                              aplTypeLft,
                                              aplIntegerLft,
                                              aplFloatLft,
                                              aplCharLft,
                                              lpSymGlbLft,
                                              aplTypeRht,
                                              aplIntegerRht,
                                              aplFloatRht,
                                              aplCharRht,
                                              lpSymGlbRht,
                                             &aplTypeNew,               // New storage type
                                              lpPrimSpec))
                {
                    // Check for type promotion
                    if (aplTypeRes NE aplTypeNew)
                    {
                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

                        // We no longer need this resource
                        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;

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

                        // Save as the new immediate & storage types
                        immTypeRes = (IMM_TYPES) tkRes.tkFlags.ImmType;
                        aplTypeRes = aplTypeNew;

                        goto RESTART_JOTDOT;
                    } // End IF

                    // Split cases based upon the result storage type
                    switch (aplTypeRes)
                    {
                        case ARRAY_BOOL:
                            // Save in the result
                            *((LPAPLBOOL) lpMemRes) |= (BIT0 &tkRes.tkData.tkBoolean) << uBitIndex;

                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;              // Start over
                                ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                            } // End IF

                            break;

                        case ARRAY_INT:
                            // Save in the result
                            *((LPAPLINT)   lpMemRes)++ = tkRes.tkData.tkInteger;

                            break;

                        case ARRAY_FLOAT:
                            // Save in the result
                            *((LPAPLFLOAT) lpMemRes)++ = tkRes.tkData.tkFloat;

                            break;

                        case ARRAY_CHAR:
                            // Can this ever happen???

                            // Save in the result
                            *((LPAPLCHAR)  lpMemRes)++ = tkRes.tkData.tkChar;

                            break;

                        case ARRAY_RAT:
                            // Lock the memory to get a ptr to it
                            lpMemSub = MyGlobalLock (tkRes.tkData.tkGlbData);

                            // Skip over the header and dimensions to the data
                            lpMemSub = VarArrayBaseToData (lpMemSub, 0);

                            // Save in the result
                            *((LPAPLRAT)   lpMemRes)++ = *(LPAPLRAT) lpMemSub;

                            // We no longer need this ptr
                            MyGlobalUnlock (tkRes.tkData.tkGlbData); lpMemSub = NULL;

                            // We no longer need this storage
                            // Note that we reused the global numeric, so we don't
                            //   need to use FreeResultGlobalVar here
                            MyGlobalFree (tkRes.tkData.tkGlbData); tkRes.tkData.tkGlbData = NULL;

                            break;

                        case ARRAY_VFP:
                            // Lock the memory to get a ptr to it
                            lpMemSub = MyGlobalLock (tkRes.tkData.tkGlbData);

                            // Skip over the header and dimensions to the data
                            lpMemSub = VarArrayBaseToData (lpMemSub, 0);

                            // Save in the result
                            *((LPAPLVFP)   lpMemRes)++ = *(LPAPLVFP) lpMemSub;

                            // We no longer need this ptr
                            MyGlobalUnlock (tkRes.tkData.tkGlbData); lpMemSub = NULL;

                            // We no longer need this storage
                            // Note that we reused the global numeric, so we don't
                            //   need to use FreeResultGlobalVar here
                            MyGlobalFree (tkRes.tkData.tkGlbData); tkRes.tkData.tkGlbData = NULL;

                            break;

                        defstop
                            break;
                    } // End SWITCH
                } else
                    goto ERROR_EXIT;
            } // End FOR
        } // End FOR
    } else
    // Loop through the left & right args
    for (uLft = 0; uLft < aplNELMLft; uLft++)
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // If the left arg is not immediate, get the next value
        if (lpMemLft)
            // Get the next value from the left arg
            GetNextValueMemIntoToken (uLft,         // Index to use
                                      lpMemLft,     // Ptr to global memory object to index
                                      aplTypeLft,   // Storage type of the arg
                                      apaOffLft,    // APA offset (if needed)
                                      apaMulLft,    // APA multiplier (if needed)
                                     &tkLftArg);    // Ptr to token in which to place the result
        // If the right arg is not immediate, get the next value
        if (lpMemRht)
            // Get the next value from the right arg
            GetNextValueMemIntoToken (uRht,         // Index to use
                                      lpMemRht,     // Ptr to global memory object to index
                                      aplTypeRht,   // Storage type of the arg
                                      apaOffRht,    // APA offset (if needed)
                                      apaMulRht,    // APA multiplier (if needed)
                                     &tkRhtArg);    // Ptr to token in which to place the result
        bRet =
          ExecFuncOnToken_EM (&lpMemRes,            // Ptr to ptr to result global memory
                              &tkLftArg,            // Ptr to left arg token
                               lpYYFcnStrRht,       // Ptr to function strand
                              &tkRhtArg,            // Ptr to right arg token
                               lptkAxisRht,         // Ptr to right operand axis token (may be NULL)
                              &uValErrCnt,          // Ptr to VALUE ERROR counter
                               lpPrimProtoRht);     // Ptr to right operand prototype function
        // Free the left & right arg tokens
        if (lpMemLft)
            FreeResultTkn (&tkRhtArg);
        if (lpMemRht)
            FreeResultTkn (&tkLftArg);
        // If it failed, ...
        if (!bRet)
            goto ERROR_EXIT;
    } // End FOR/FOR

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Check for VALUE ERROR
    if (uValErrCnt)
    {
        // Check for all VALUE ERRORs
        if (uValErrCnt EQ aplNELMRes)
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

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxisOpr);
    goto ERROR_EXIT;

RIGHT_OPERAND_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

RIGHT_OPERAND_NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    return NULL;

VALUE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    return NULL;

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
} // End PrimOpDydJotDotCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpDydJotDotProto_EM
//
//  Handle prototypes in dyadic jotdot
//***************************************************************************

UBOOL PrimOpDydJotDotProto_EM
    (LPVOID      *lplpMemRes,                   // Ptr to ptr to result global memory
     LPTOKEN      lptkLftArg,                   // Ptr to left arg token
     LPTOKEN      lptkRhtArg,                   // Ptr to right arg token
     LPPL_YYSTYPE lpYYFcnStrRht,                // Ptr to function strand
     LPPRIMFNS    lpPrimProtoRht,               // Ptr to right operand prototype function
     LPUBOOL      lpbCtrlBreak)                 // Ptr to Ctrl-Break flag

{
    LPPL_YYSTYPE lpYYRes = NULL;                // Ptr to the result
    HGLOBAL      hGlbPro;                       // Prototype result global memory handle
    UBOOL        bRet;                          // TRUE iff the result is valid

    // Check for Ctrl-Break
    if (CheckCtrlBreak (*lpbCtrlBreak))
        return FALSE;

    // Execute the right operand between the left & right prototypes
    if (!ExecDydProto_EM (lptkLftArg,           // Ptr to left arg token
           (LPPL_YYSTYPE) lptkLftArg,           // Ptr to left operand function strand (for tkCharIndex only)
                          lptkRhtArg,           // Ptr to right arg token
                          lpYYFcnStrRht,        // Ptr to right operand function strand
                          lpPrimProtoRht,       // Ptr to right operand prototype function
                         &lpYYRes))             // Ptr ptr to to result
        goto ERROR_EXIT;

    // Get the global handle (if any) of the last calc
    hGlbPro = GetGlbHandle (&lpYYRes->tkToken);

    // If the result of the last calc is immediate, ...
    if (!hGlbPro)
    {
        // Convert the immediate type and value in lpYYRes->tkToken
        //   into an LPSYMENTRY
        *(*(LPAPLNESTED *) lplpMemRes) =
          MakeSymEntry_EM (lpYYRes->tkToken.tkFlags.ImmType,
                          &lpYYRes->tkToken.tkData.tkLongest,
                           lptkRhtArg);
        if (*lplpMemRes EQ NULL)
            goto ERROR_EXIT;
    } else
        // Save the result into global memory
        *(*(LPAPLNESTED *) lplpMemRes) =
          MakePtrTypeGlb (hGlbPro);

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

ERROR_EXIT:
    if (lpYYRes)
        FreeResult (lpYYRes);

    // Mark as in error
    bRet = FALSE;
NORMAL_EXIT:
    // Free the result item (but not the storage)
    YYFree (lpYYRes); lpYYRes = NULL;

    return bRet;
} // End PrimOpDydJotDotProto_EM


//***************************************************************************
//  End of File: po_jotdot.c
//***************************************************************************
