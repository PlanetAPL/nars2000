//***************************************************************************
//  NARS2000 -- Primitive Operator -- Slash
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
#include "mf_ros.h"


//***************************************************************************
//  $PrimOpSlash_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    monadic operator Slash ("reduction" and "N-wise reduction")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpSlash_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpSlash_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ INDEX_OPSLASH
         || lpYYFcnStrOpr->tkToken.tkData.tkChar EQ INDEX_OPSLASHBAR
         || lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_SLASH         // For when we come in via TKT_OP3NAMED
         || lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_SLASHBAR);    // ...

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return PrimOpMonSlash_EM_YY (lpYYFcnStrOpr, // Ptr to operator function strand
                                     lptkRhtArg);   // Ptr to right arg
    else
        return PrimOpDydSlash_EM_YY (lptkLftArg,    // Ptr to left arg token
                                     lpYYFcnStrOpr, // Ptr to operator function strand
                                     lptkRhtArg);   // Ptr to right arg token
} // End PrimOpSlash_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoOpSlash_EM_YY
//
//  Generate a prototype for the derived functions from
//    monadic operator Slash ("reduction" and "N-wise reduction")
//***************************************************************************

LPPL_YYSTYPE PrimProtoOpSlash_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxisOpr)          // Ptr to axis token always NULL)

{
    Assert (lptkAxisOpr EQ NULL);

    // If left arg is not present, ...
    if (lptkLftArg EQ NULL)
        //***************************************************************
        // Called monadically
        //***************************************************************
        return PrimOpMonSlashCommon_EM_YY (lpYYFcnStrOpr,   // Ptr to operator function strand
                                           lptkRhtArg,      // Ptr to right arg token
                                           TRUE);           // TRUE iff prototyping
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************
        return PrimOpDydSlashCommon_EM_YY (lptkLftArg,      // Ptr to left arg token (may be NULL if monadic)
                                           lpYYFcnStrOpr,   // Ptr to operator function strand
                                           lptkRhtArg,      // Ptr to right arg token
                                           TRUE);           // TRUE iff prototyping
} // End PrimProtoOpSlash_EM_YY


//***************************************************************************
//  $PrimOpMonSlash_EM_YY
//
//  Primitive operator for monadic derived function from Slash ("reduction")
//***************************************************************************

LPPL_YYSTYPE PrimOpMonSlash_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpMonSlashCommon_EM_YY (lpYYFcnStrOpr,       // Ptr to operator function strand
                                       lptkRhtArg,          // Ptr to right arg token
                                       FALSE);
} // End PrimOpMonSlash_EM_YY


//***************************************************************************
//  $PrimOpMonSlashCommon_EM_YY
//
//  Primitive operator for monadic derived function from Slash ("reduction")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpMonSlashCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpMonSlashCommon_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,            // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,               // Ptr to right arg token
     UBOOL        bPrototyping)             // TRUE iff prototyping

{
    APLSTYPE          aplTypeRht,           // Right arg storage type
                      aplTypeRht2,          // Right arg secondary storage type
                      aplTypeTmp,           // Temp right arg ...
                      aplTypeNew,           // New result
                      aplTypeRes;           // Result    ...
    APLNELM           aplNELMRht,           // Right arg NELM
                      aplNELMRes;           // Result    ...
    APLRANK           aplRankRht,           // Right arg rank
                      aplRankRes;           // Result    ...
    APLUINT           aplAxis,              // The (one and only) axis value
                      uLo,                  // uDimLo loop counter
                      uHi,                  // uDimHi ...
                      uDim,                 // Loop counter
                      uDimLo,               // Product of dimensions below axis
                      uDimHi,               // ...                   above ...
                      uDimRht,              // Starting index in right arg of current vector
                      uDimAxRht,            // Right arg axis dimension
                      uRht,                 // Right arg loop counter
                      uPrv,                 // Previous right arg loop counter
                      uRes,                 // Result loop counter
                      ByteRes;              // # bytes in the result
    APLINT            iDim,                 // Integer dimension loop counter
                      apaOffRht,            // Right arg APA offset
                      apaMulRht;            // ...           multiplier
    HGLOBAL           hGlbRht = NULL,       // Right arg global memory handle
                      hGlbRes = NULL;       // Result    ...
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg header
                      lpMemHdrRes = NULL;   // ...    result    ...
    LPVOID            lpMemRht,             // Ptr to right arg global memory
                      lpMemRes;             // Ptr to result    ...
    LPAPLDIM          lpMemDimRht,          // Ptr to right arg dimensions
                      lpMemDimRes;          // Ptr to result    ...
    HGLOBAL           hGlbPro = NULL;       // Prototype global memory handle
    APLFLOAT          aplFloatIdent;        // Identity element
    UBOOL             bRet = TRUE,          // TRUE iff result is valid
                      bPrimDydScal = FALSE, // TRUE iff the left operand is a Primitive Dyadic Scalar function
                      bNrmIdent = FALSE,    // TRUE iff reducing an empty array with a primitive scalar dyadic function
                      bPrimIdent = FALSE,   // TRUE iff reducing an empty array with a primitive function/operator
                      bFastBool = FALSE;    // TRUE iff this is a Fast Boolean operation
    LPPRIMFNS         lpPrimProtoLft;       // Ptr to left operand prototype function
    LPPRIMSPEC        lpPrimSpecLft;        // Ptr to left operand PRIMSPEC
    LPPRIMFLAGS       lpPrimFlagsLft;       // Ptr to left operand PrimFlags entry
    LPPRIMIDENT       lpPrimIdentLft;       // Ptr to left operand PrimIdent entry
    LPPL_YYSTYPE      lpYYRes = NULL,       // Ptr to the result
                      lpYYFcnStrLft;        // Ptr to left operand function strand
    TOKEN             tkLftArg = {0},       // Left arg token
                      tkRhtArg = {0};       // Right ...
    LPTOKEN           lptkAxisOpr,          // Ptr to operator axis token (may be NULL)
                      lptkAxisLft;          // ...    left operand axis token (may be NULL)
    LPSYMENTRY        lpSymTmp;             // Ptr to temporary LPSYMENTRY
    UINT              uBitIndex;            // Bit index for looping through Booleans
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    APLLONGEST        aplLongestRht;        // Right arg immediate value
    HGLOBAL           lpSymGlbLft,          // Ptr to RAT/VFP left arg
                      lpSymGlbRht,          // ...            right ...
                      hGlbTmp;              // Temporary global memory handle
    APLRAT            aplRatRht = {0};      // Right arg as Rational
    APLVFP            aplVfpRht = {0};      // ...          VFP
    ALLTYPES          atTmp = {0};          // Temporary ALLTYPES

    // Set the result array storage type for the ending code
    aplTypeRes = ARRAY_BOOL;

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

    // Check for left operand axis operator
    lptkAxisLft = CheckAxisOper (lpYYFcnStrLft);

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for axis present
    if (lptkAxisOpr NE NULL)
    {
        // Reduction allows a single integer axis value only
        if (!CheckAxis_EM (lptkAxisOpr,     // The derived function axis token
                           aplRankRht,      // All values less than this
                           TRUE,            // TRUE iff scalar or one-element vector only
                           FALSE,           // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // Return TRUE iff fractional values present
                          &aplAxis,         // Return last axis value
                           NULL,            // Return # elements in axis vector
                           NULL))           // Return HGLOBAL with APLINT axis values
            return NULL;
    } else
    {
        // No axis specified:
        // if Slash, use last dimension
        if (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ INDEX_OPSLASH
         || lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_SLASH)
            aplAxis = max (aplRankRht, 1) - 1;
        else
        {
            Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ INDEX_OPSLASHBAR
                 || lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_SLASHBAR);

            // Otherwise, it's SlashBar on the first dimension
            aplAxis = 0;
        } // End IF/ELSE
    } // End IF/ELSE

    // Get right arg's global ptr
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // If the argument is not a scalar, ...
    if (!IsScalar (aplRankRht))
    {
        // The rank of the result is one less than that of the right arg
        aplRankRes = aplRankRht - 1;

        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);

        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

        //***************************************************************
        // Calculate product of dimensions before, at, and after the axis dimension
        //***************************************************************

        // Calculate the product of the right arg's dimensions below the axis dimension
        uDimLo = 1;
        for (uDim = 0; uDim < aplAxis; uDim++)
            uDimLo *= lpMemDimRht[uDim];

        // Get the length of the axis dimension
        uDimAxRht = lpMemDimRht[uDim++];

        // Calculate the product of the right arg's dimensions above the axis dimension
        uDimHi = 1;
        for (; uDim < aplRankRht; uDim++)
            uDimHi *= lpMemDimRht[uDim];

        // Calculate the result NELM
        aplNELMRes = imul64 (uDimLo, uDimHi, &bRet);
        if (!bRet)
            goto WSFULL_EXIT;
    } else
        uDimAxRht = 1;

    // If the reduction axis length is 1, ...
    if (IsUnitDim (uDimAxRht))
    {
        // Reduce it
        lpYYRes =
          PrimOpRedOfSing_EM_YY (lptkRhtArg,        // Ptr to right arg token
                                 lpYYFcnStrOpr,     // Ptr to operator function strand
                                 lpYYFcnStrLft,     // Ptr to left operand
                                &aplAxis,           // Ptr to the reduction axis value (may be NULL if scan)
                                 FALSE,             // TRUE iff we should treat as Scan
                                 FALSE,             // TRUE iff the item must be enclosed before reducing it
                                 bPrototyping);     // TRUE iff prototyping
        goto NORMAL_EXIT;
    } // End IF

    //***************************************************************
    // From here on, the right arg is a vector or higher rank array
    //***************************************************************

    // If the right arg is an APA, ...
    if (IsSimpleAPA (aplTypeRht))
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        // Get the APA parameters
        apaOffRht = lpAPA->Off;
        apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // Handle prototypes specially
    if (IsEmpty (aplNELMRes)
     || bPrototyping)
    {
        // Get the appropriate prototype function ptr
        lpPrimProtoLft = GetPrototypeFcnPtr (&lpYYFcnStrLft->tkToken);
        if (lpPrimProtoLft EQ NULL)
            goto LEFT_NONCE_EXIT;
    } else
        lpPrimProtoLft = NULL;

    // Get a ptr to the Primitive Function Flags
    lpPrimFlagsLft = GetPrimFlagsPtr (&lpYYFcnStrLft->tkToken);
    if (lpPrimFlagsLft NE NULL)
        lpPrimIdentLft = &PrimIdent[lpPrimFlagsLft->Index];
    else
        lpPrimIdentLft = NULL;

    // If the result is empty or the axis dimension is zero, ...
    if (IsEmpty (aplNELMRes)
     || IsZeroDim (uDimAxRht))
    {
        // If we're also prototyping,
        //   and it's a UDFO, ...
        if (bPrototyping
         && IsTknUsrDfn (&lpYYFcnStrLft->tkToken))
            goto NONCE_EXIT;

        // If the reduction function is primitive scalar dyadic,
        //   and the right arg is not nested, ...
        if (lpPrimFlagsLft->DydScalar
         && !IsNested (aplTypeRht))
        {
            // If there's no identity element, ...
            if (!lpPrimFlagsLft->IdentElem)
                goto DOMAIN_EXIT;

            // Get the identity element
            aplFloatIdent = lpPrimIdentLft->fIdentElem;

            // If the identity element is Boolean or we're prototyping,
            //   the result is too
            if (lpPrimIdentLft->IsBool
             || lpPrimProtoLft)
                aplTypeRes = ARRAY_BOOL;
            else
                aplTypeRes = ARRAY_FLOAT;

            // Mark as reducing empty array with a primitive scalar dyadic function
            //   to produce its identity element
            bNrmIdent = TRUE;
        } else
        {
            // The result is nested
            aplTypeRes = ARRAY_NESTED;

            // Mark as reducing empty array with a primitive or
            //   user-defined function/operator
            //   to produce its identity element
            bPrimIdent = TRUE;
        } // End IF
    } else
    // If the axis dimension is zero and the array is simple
    //   get the identity element for the left operand or
    //   signal a DOMAIN ERROR
    if (IsZeroDim (uDimAxRht)
     && IsSimple (aplTypeRht))
    {
        // If it's not an immediate primitive function,
        //   or it is, but is without an identity element,
        //   signal a DOMAIN ERROR
        if (lpYYFcnStrLft->tkToken.tkFlags.TknType NE TKT_FCNIMMED
         || !lpPrimFlagsLft->IdentElem)
            goto DOMAIN_EXIT;

        // Get the identity element
        aplFloatIdent = lpPrimIdentLft->fIdentElem;

        // If the identity element is Boolean or we're prototyping,
        //   the result is too
        if (lpPrimIdentLft->IsBool
         || lpPrimProtoLft NE NULL)
            aplTypeRes = ARRAY_BOOL;
        else
            aplTypeRes = ARRAY_FLOAT;

        // Mark as reducing empty array with a primitive scalar dyadic function
        //   to produce its identity element
        bNrmIdent = TRUE;
    } else
    // If the product of the dimensions above
    //   the axis dimension is one, and
    //   this is a primitive function, and
    //   there's no left operand axis token, and
    //   the right arg is Boolean or APA Boolean, and
    //   the axis dimension is > 1, and
    //   we're not doing prototypes, then
    //   check for the possibility of doing a
    //   Fast Boolean Reduction
    if (IsUnitDim (uDimHi)
     && lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
     && lptkAxisLft EQ NULL
     && (IsSimpleBool (aplTypeRht)
      || (IsSimpleAPA (aplTypeRht)
       && IsBooleanValue (apaOffRht)
       && apaMulRht EQ 0 ))
     && IsMultiDim (uDimAxRht)
     && lpPrimProtoLft EQ NULL
     && (lpPrimFlagsLft->FastBool || lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_PLUS))
    {
        // Mark as a Fast Boolean operation
        bFastBool = TRUE;

        // If this is "plus", then the storage type is Integer
        if (lpPrimFlagsLft->Index EQ PF_INDEX_PLUS)
            aplTypeRes = ARRAY_INT;
        else
        // Otherwise, it's Boolean
            aplTypeRes = ARRAY_BOOL;
    } else
    // If the operand is a primitive scalar dyadic function,
    //   there's no left operand axis token, and
    //   and the right arg is simple NH or numeric,
    //   calculate the storage type of the result,
    //   otherwise, assume it's ARRAY_NESTED
    if (lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
     && lptkAxisLft EQ NULL
     && lpPrimFlagsLft->DydScalar
     && (IsSimpleNH (aplTypeRht)
      || IsNumeric (aplTypeRht)))
    {
        // If the function is equal or not-equal, and the right
        //   arg is not Boolean, make the result storage type
        //   nested and let TypeDemote figure it out in the end.
        // This avoids complications with =/3 1{rho}'abc' which
        //   is typed as Boolean but blows up to CHAR.
        if ((lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_EQUAL
          || lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_NOTEQUAL)
         && !IsSimpleBool (aplTypeRht))
            aplTypeRes = ARRAY_NESTED;
        else
    {
            // Get the corresponding lpPrimSpecLft
            lpPrimSpecLft = PrimSpecTab[SymTrans (&lpYYFcnStrLft->tkToken)];

            // Calculate the storage type of the result
            aplTypeRes =
              (*lpPrimSpecLft->StorageTypeDyd) (aplNELMRht,
                                               &aplTypeRht,
                                               &lpYYFcnStrLft->tkToken,
                                                aplNELMRht,
                                               &aplTypeRht);
            if (IsErrorType (aplTypeRes))
                goto DOMAIN_EXIT;

            // Mark as a primitive scalar dyadic function
            bPrimDydScal = TRUE;
        } // End IF/ELSE
    } else
    // If the function is left or right tack, ...
    if (lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_LEFTTACK
     || lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_RIGHTTACK)
    {
        // If the right arg is APA, ...
        if (IsSimpleAPA (aplTypeRht))
        {
            // If the APA is Boolean, ...
            if (IsBooleanValue (apaOffRht)
             && apaMulRht EQ 0)
                aplTypeRes = ARRAY_BOOL;
            else
                aplTypeRes = ARRAY_INT;
        } else
            aplTypeRes = aplTypeRht;
    } else
        aplTypeRes = ARRAY_NESTED;
RESTART_ALLOC:
    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

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
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = lpMemDimRes = VarArrayBaseToDim (lpMemHdrRes);

    // Copy the dimensions from the right arg to the result
    //   except for the axis dimension
    for (uDim = 0; uDim < aplRankRht; uDim++)
    if (uDim NE aplAxis)
        *((LPAPLDIM) lpMemRes)++ = lpMemDimRht[uDim];

    // lpMemRes now points to its data

    // If this is primitive or user-defined function/operator identity element, ...
    if (bPrimIdent)
    {
        if (!FillIdentityElement_EM (lpMemRes,              // Ptr to result global memory
                                     aplNELMRes,            // Result NELM
                                     aplTypeRes,            // Result storage type
                                     lpYYFcnStrLft,         // Ptr to left operand function strand
                                     NULL,                  // Ptr to left arg token
                                     NULL,                  // Ptr to right operand function strand (may be NULL if Scan)
                                     lptkRhtArg))           // Ptr to right arg token
            goto ERROR_EXIT;
    } else
    // If this is primitive scalar dyadic function identity element, ...
    if (bNrmIdent)
    {
        // The zero case is done (GHND)

        // If we're not doing prototypes, ...
        if (lpPrimProtoLft EQ NULL)
        {
            // Check for Boolean identity element
            if (lpPrimIdentLft->IsBool)
            {
                // Check for identity element 1
                if (lpPrimIdentLft->bIdentElem)
                {
                    APLNELM uNELMRes;

                    // Calculate the # bytes in the result, rounding up
                    uNELMRes = (aplNELMRes + (NBIB - 1)) >> LOG2NBIB;

                    for (uRes = 0; uRes < uNELMRes; uRes++)
                        *((LPAPLBOOL) lpMemRes)++ = 0xFF;
                } // End IF
            } else
            for (uRes = 0; uRes < aplNELMRes; uRes++)
                *((LPAPLFLOAT) lpMemRes)++ = aplFloatIdent;
        } // End IF
    } else
    // If this is a fast Boolean operation, ...
    if (bFastBool)
    {
        LPFASTBOOLFCN lpFastBool;               // Ptr to Fast Boolean reduction routine

        // Get a ptr to the appropriate Fast Boolean routine
        lpFastBool = FastBoolFns[lpPrimFlagsLft->Index].lpReduction;

        // Call it
        (*lpFastBool) (aplTypeRht,              // Right arg storage type
                       aplNELMRht,              // Right arg NELM
                       lpMemRht,                // Ptr to right arg memory
                       lpMemRes,                // Ptr to result    memory
                       uDimLo,                  // Product of dimensions below axis
                       uDimAxRht,               // Length of right arg axis dimension
                       lpPrimFlagsLft->Index,   // FBFN_INDS value (e.g., index into FastBoolFns[])
                       lpYYFcnStrOpr);          // Ptr to operator function strand
    } else
    // If this is a nested result from an empty right arg, ...
    if (IsEmpty (aplNELMRht)
     && IsNested (aplTypeRes))
    {
        APLNELM aplNELMNst;

        // Calculate the # elements in the result
        aplNELMNst = max (aplNELMRes, 1);

        // If the right arg is nested or simple hetero, ...
        if (IsPtrArray (aplTypeRht))
            // Loop through the right arg/result
            for (uRes = 0; uRes < aplNELMNst; uRes++)
                // Copy the right arg's prototype
                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbInd_PTB (lpMemRht);
        else
        {
            LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

            // Get ptr to PerTabData global memory
            lpMemPTD = GetMemPTD ();

            // Loop through the right arg/result
            for (uRes = 0; uRes < aplNELMNst; uRes++)
                // Make & save a prototype of the right arg
                *((LPAPLNESTED) lpMemRes)++ =
                  (IsSimpleNum (aplTypeRht)) ? lpMemPTD->lphtsGLB->steZero
                                             : lpMemPTD->lphtsGLB->steBlank;
        } // End IF
    } else
    // If this is +/APA or max/APA or min/APA
    if (IsSimpleAPA (aplTypeRht)
     && lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
     && (lpYYFcnStrLft->tkToken.tkData.tkChar  EQ L'+'
      || lpYYFcnStrLft->tkToken.tkData.tkChar  EQ UTF16_UPSTILE
      || lpYYFcnStrLft->tkToken.tkData.tkChar  EQ UTF16_DOWNSTILE))
    {
RESTART_EXCEPTION_APA:
        // If the result is integer
        if (IsSimpleInt (aplTypeRes))
        {
            // Loop through the right arg calling the
            //   function strand between data, storing in the
            //   result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                // Calculate the starting index in the right arg of this vector
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;

                // The indices of the right arg are
                //   uDimRht + uDimHi * {iota} uDimAxRht

                // Split cases based upon the function symbol
                switch (lpYYFcnStrLft->tkToken.tkData.tkChar)
                {
                    case UTF16_PLUS:
                        // The result is (in origin-0)
                        //   +/apaOffRht + apaMulRht * (uDimRht + uDimHi * {iota} uDimAxRht)
                        // = +/apaOffRht + apaMulRht * uDimRht + apalMulRht * uDimHi * {iota} uDimAxRht
                        // = uDimAxRht * (apaOffRht + apaMulRht * uDumRht) + +/apaMulRht * uDimHi *   {iota} uDimAxRht
                        // = uDimAxRht * (apaOffRht + apaMulRht * uDumRht) +   apaMulRht * uDimHi * +/{iota} uDimAxRht
                        // = uDimAxRht * (apaOffRht + apaMulRht * uDumRht) +   apaMulRht * uDimHi * (uDimAxRht * (uDimAxRht - 1)) / 2
                        __try
                        {
        ////////////////////*((LPAPLINT) lpMemRes)++ = uDimAxRht * (apaOffRht + apaMulRht * uDimRht)
        ////////////////////                         + apaMulRht * uDimHi * (uDimAxRht * (uDimAxRht - 1)) / 2;
                            *((LPAPLINT) lpMemRes)++ = iadd64_RE (imul64_RE (uDimAxRht, iadd64_RE (apaOffRht, imul64_RE (apaMulRht, uDimRht))),
                                                                  imul64_RE (apaMulRht, imul64_RE (uDimHi,    imul64_RE (uDimAxRht, isub64_RE (uDimAxRht, 1)) / 2)));
                        } __except (CheckException (GetExceptionInformation (), L"PrimFnMon_EM_YY #1"))
                        {
                            dprintfWL9 (L"!!Initiating Exception in " APPEND_NAME L" #1: %2d (%S#%d)", MyGetExceptionCode (), FNLN);

                            // Split cases based upon the ExceptionCode
                            switch (MyGetExceptionCode ())
                            {
                                case EXCEPTION_RESULT_FLOAT:
                                    MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                                    if (IsSimpleNum (aplTypeRes)
                                     && !IsSimpleFlt (aplTypeRes))
                                    {
                                        // It's now a FLOAT result
                                        aplTypeRes = ARRAY_FLOAT;

                                        // Tell the header about it
                                        lpMemHdrRes->ArrType = aplTypeRes;

                                        // Restart the pointer
                                        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

                                        dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #1: %2d (%S#%d)", MyGetExceptionCode (), FNLN);

                                        goto RESTART_EXCEPTION_APA;
                                    } // End IF

                                    // Fall through to never-never-land

                                default:
                                    // Display message for unhandled exception
                                    DisplayException ();

                                    break;
                            } // End SWITCH
                        } // End __try/__except

                        break;

                    case UTF16_UPSTILE:
                        // The result is (in origin-0)
                        //   max/apaOffRht + apaMulRht * (uDimRht + uDimHi * {iota} uDimAxRht)
                        // = max/apaOffRht + apaMulRht * uDimRht + apalMulRht * uDimHi * {iota} uDimAxRht
                        // If     apaMulRht == 0
                        // =     apaOffRht
                        // ElseIf apaMulRht  > 0
                        // =     apaOffRht + apaMulRht * uDimRht + apalMulRht * uDimHi * (uDimAxRht - 1)
                        // =     apaOffRht + apaMulRht * (uDimRht + uDimHi * (uDimAxRht - 1)))
                        // ElseIf apaMulRht  < 0
                        // =     apaOffRht + apaMulRht * uDimRht + apalMulRht * uDimHi * 0
                        // =     apaOffRht + apaMulRht * uDimRht

                        __try
                        {
                            // Split cases based upon the sign of the multiplier
                            switch (signumint (apaMulRht))
                            {
                                case   0:
                                    *((LPAPLINT) lpMemRes)++ = apaOffRht;

                                    break;

                                case   1:
                                    *((LPAPLINT) lpMemRes)++ = iadd64_RE (apaOffRht, imul64_RE (apaMulRht,
                                                                                                iadd64_RE (uDimRht,
                                                                                                           imul64_RE (uDimHi,
                                                                                                                      isub64_RE (uDimAxRht, 1)))));
                                    break;

                                case  -1:
                                    *((LPAPLINT) lpMemRes)++ = iadd64_RE (apaOffRht, imul64_RE (apaMulRht, uDimRht));

                                    break;

                                defstop
                                    break;
                            } // End SWITCH
                        } __except (CheckException (GetExceptionInformation (), L"PrimFnMon_EM_YY #1"))
                        {
                            dprintfWL9 (L"!!Initiating Exception in " APPEND_NAME L" #2: %2d (%S#%d)", MyGetExceptionCode (), FNLN);

                            // Split cases based upon the ExceptionCode
                            switch (MyGetExceptionCode ())
                            {
                                case EXCEPTION_RESULT_FLOAT:
                                    MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                                    if (IsSimpleNum (aplTypeRes)
                                     && !IsSimpleFlt (aplTypeRes))
                                    {
                                        // It's now a FLOAT result
                                        aplTypeRes = ARRAY_FLOAT;

                                        // Tell the header about it
                                        lpMemHdrRes->ArrType = aplTypeRes;

                                        // Restart the pointer
                                        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

                                        dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #1: %2d (%S#%d)", MyGetExceptionCode (), FNLN);

                                        goto RESTART_EXCEPTION_APA;
                                    } // End IF

                                    // Fall through to never-never-land

                                default:
                                    // Display message for unhandled exception
                                    DisplayException ();

                                    break;
                            } // End SWITCH
                        } // End __try/__except

                        break;

                    case UTF16_DOWNSTILE:
                        // The result is (in origin-0)
                        //   min/apaOffRht + apaMulRht * (uDimRht + uDimHi * {iota} uDimAxRht)
                        // = min/apaOffRht + apaMulRht * uDimRht + apalMulRht * uDimHi * {iota} uDimAxRht
                        // If     apaMulRht == 0
                        // =     apaOffRht
                        // ElseIf apaMulRht  > 0
                        // =     apaOffRht + apaMulRht * uDimRht + apalMulRht * uDimHi * 0
                        // =     apaOffRht + apaMulRht * (uDimRht + uDimHi * uDimAxRht))
                        // ElseIf apaMulRht  < 0
                        // =     apaOffRht + apaMulRht * uDimRht + apalMulRht * uDimHi * (uDimAxRht - 1)
                        // =     apaOffRht + apaMulRht * (uDimRht + uDimHi))

                        __try
                        {
                            // Split cases based upon the sign of the multiplier
                            switch (signumint (apaMulRht))
                            {
                                case   0:
                                    *((LPAPLINT) lpMemRes)++ = apaOffRht;

                                    break;

                                case  -1:
                                    *((LPAPLINT) lpMemRes)++ = iadd64_RE (apaOffRht, imul64_RE (apaMulRht,
                                                                                                iadd64_RE (uDimRht,
                                                                                                           imul64_RE (uDimHi,
                                                                                                                      isub64_RE (uDimAxRht, 1)))));
                                    break;

                                case   1:
                                    *((LPAPLINT) lpMemRes)++ = iadd64_RE (apaOffRht, imul64_RE (apaMulRht, uDimRht));

                                    break;

                                defstop
                                    break;
                            } // End SWITCH
                        } __except (CheckException (GetExceptionInformation (), L"PrimFnMon_EM_YY #1"))
                        {
                            dprintfWL9 (L"!!Initiating Exception in " APPEND_NAME L" #3: %2d (%S#%d)", MyGetExceptionCode (), FNLN);

                            // Split cases based upon the ExceptionCode
                            switch (MyGetExceptionCode ())
                            {
                                case EXCEPTION_RESULT_FLOAT:
                                    MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                                    if (IsSimpleNum (aplTypeRes)
                                     && !IsSimpleFlt (aplTypeRes))
                                    {
                                        // It's now a FLOAT result
                                        aplTypeRes = ARRAY_FLOAT;

                                        // Tell the header about it
                                        lpMemHdrRes->ArrType = aplTypeRes;

                                        // Restart the pointer
                                        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

                                        dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #1: %2d (%S#%d)", MyGetExceptionCode (), FNLN);

                                        goto RESTART_EXCEPTION_APA;
                                    } // End IF

                                    // Fall through to never-never-land

                                default:
                                    // Display message for unhandled exception
                                    DisplayException ();

                                    break;
                            } // End SWITCH
                        } // End __try/__except

                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR/FOR
        } else
        // The result is float
        {
            APLFLOAT uFltDimRht,
                     apaFltOffRht,
                     apaFltMulRht,
                     uFltDimAxRht,
                     uFltDimHi;

            // Convert vars to float
            apaFltOffRht = (APLFLOAT) apaOffRht;
            apaFltMulRht = (APLFLOAT) apaMulRht;
            uFltDimAxRht = (APLFLOAT) (APLINT) uDimAxRht;
            uFltDimHi    = (APLFLOAT) (APLINT) uDimHi;

            // Loop through the right arg calling the
            //   function strand between data, storing in the
            //   result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                // Calculate the starting index in the right arg of this vector
                uFltDimRht = (APLFLOAT) (APLINT) (uLo * uDimHi * uDimAxRht + uHi);

                // The indices of the right arg are
                //   uDimRht + uDimHi * {iota} uDimAxRht

                // The result is
                //   +/apaOffRht + apaMulRht * (uDimRht + uDimHi * {iota} uDimAxRht)
                // = +/apaOffRht + apaMulRht * uDimRht + apalMulRht * uDimHi * {iota} uDimAxRht
                // = uDimAxRht * (apaOffRht + apaMulRht * uDumRht) + +/apaMulRht * uDimHi *   {iota} uDimAxRht
                // = uDimAxRht * (apaOffRht + apaMulRht * uDumRht) +   apaMulRht * uDimHi * +/{iota} uDimAxRht
                // = uDimAxRht * (apaOffRht + apaMulRht * uDumRht) +   apaMulRht * uDimHi * (uDimAxRht * (uDimAxRht - 1)) / 2
////////////////*((LPAPLINT) lpMemRes)++ = uDimAxRht * (apaOffRht + apaMulRht * uDimRht)
////////////////                         + apaMulRht * uDimHi * (uDimAxRht * (uDimAxRht - 1)) / 2;
                *((LPAPLFLOAT) lpMemRes)++ = uFltDimAxRht * (apaFltOffRht + apaFltMulRht * uFltDimRht)
                                           + apaFltMulRht * uFltDimHi * (uFltDimAxRht * (uFltDimAxRht - 1)) / 2;
            } // End FOR/FOR
        } // End IF/ELSE
    } else
    // If the function is left or right tack, ...
    if (lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_LEFTTACK
     || lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_RIGHTTACK)
    {
        // Initialize in case Boolean result
        uBitIndex = 0;

        // Loop through the right arg
        for (uLo = 0; uLo < uDimLo; uLo++)
        for (uHi = 0; uHi < uDimHi; uHi++)
        {
            // Calculate the starting index in the right arg of this vector
            uDimRht = uLo * uDimHi * uDimAxRht + uHi;

            // If the function is left tack, ...
            if (lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_LEFTTACK)
                // Calculate the index of first element in this vector
                uRht = uDimRht;
            else
                // Calculate the index of last element in this vector
                uRht = uDimRht + (uDimAxRht - 1) * uDimHi;

            // Get the first/last value in the vector from the right arg token
            GetNextValueToken (lptkRhtArg,      // Ptr to the token
                               uRht,            // Index to use
                               NULL,            // Ptr to the integer (or Boolean) (may be NULL)
                               NULL,            // ...        float (may be NULL)
                               NULL,            // ...        char (may be NULL)
                              &aplLongestRht,   // ...        longest (may be NULL)
                              &lpSymGlbRht,     // ...        LPSYMENTRY or HGLOBAL (may be NULL)
                               NULL,            // ...        immediate type (see IMM_TYPES) (may be NULL)
                               NULL);           // ...        array type:  ARRAY_TYPES (may be NULL)
            // If the right arg is immediate, ...
            if (lpSymGlbRht EQ NULL)
                // Point to the data
                lpSymGlbRht = &aplLongestRht;

            // Split cases based upon the result type
            switch (aplTypeRes)
            {
                case ARRAY_BOOL:
                    // If the leading/trailing bit is a 1, ...
                    if (aplLongestRht & BIT0)
                        // Save in the result
                        *((LPAPLBOOL) lpMemRes) |= BIT0 << uBitIndex;

                    // Check for end-of-byte
                    if (++uBitIndex EQ NBIB)
                    {
                        uBitIndex = 0;              // Start over
                        ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                    } // End IF

                    break;

                case ARRAY_INT:
                    // Save in the result
                    *((LPAPLINT) lpMemRes)++ = (APLINT) aplLongestRht;

                    break;

                case ARRAY_FLOAT:
                    // Save in the result
                    *((LPAPLFLOAT) lpMemRes)++ = *(LPAPLFLOAT) &aplLongestRht;

                    break;

                case ARRAY_CHAR:
                    // Save in the result
                    *((LPAPLCHAR) lpMemRes)++ = (APLCHAR) aplLongestRht;

                    break;

                case ARRAY_HETERO:
                case ARRAY_NESTED:
                    // Save in the result
                    *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (lpSymGlbRht);

                    break;

                case ARRAY_RAT:
                    // Save in the result
                    mpq_init_set (((LPAPLRAT) lpMemRes)++, (LPAPLRAT) lpSymGlbRht);

                    break;

                case ARRAY_VFP:
                    // Save in the result
                    mpfr_init_set (((LPAPLVFP) lpMemRes)++, (LPAPLVFP) lpSymGlbRht, MPFR_RNDN);

                    break;

                defstop
                    break;
            } // End SWITCH
        } // End FOR/FOR
    } else
    {
        // Fill in the right arg token
////////tkRhtArg.tkFlags.TknType   =            // To be filled in below
////////tkRhtArg.tkFlags.ImmType   =            // To be filled in below
////////tkRhtArg.tkFlags.NoDisplay = FALSE;     // Already zero from {0}
////////tkRhtArg.tkData.tkGlbData  =            // To be filled in below
        tkRhtArg.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

        // Fill in the left arg token
////////tkLftArg.tkFlags.TknType   =            // To be filled in below
////////tkLftArg.tkFlags.ImmType   =            // To be filled in below
////////tkLftArg.tkFlags.NoDisplay = FALSE;     // Already zero from {0}
////////tkLftArg.tkData.tkGlbData  =            // To be filled in below
        tkLftArg.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

        // Initialize the secondary right arg storage type
        aplTypeRht2 = aplTypeRht;

        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
            case ARRAY_FLOAT:
            case ARRAY_CHAR:
            case ARRAY_HETERO:
            case ARRAY_NESTED:
                break;

            case ARRAY_RAT:
                // Initialize the temp
                mpq_init (&aplRatRht);

                break;

            case ARRAY_VFP:
                // Initialize the temp to 0
                mpfr_init0 (&aplVfpRht);

                break;

            defstop
                break;
        } // End SWITCH

        // if the result and right arg are of different types, ...
        if (aplTypeRht NE aplTypeRes)
        // Split cases based upon the result storage type
        switch (aplTypeRes)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
            case ARRAY_FLOAT:
            case ARRAY_HETERO:
            case ARRAY_NESTED:
                break;

            case ARRAY_RAT:
                // Initialize the temp
                mpq_init (&aplRatRht);

                break;

            case ARRAY_VFP:
                // Initialize the temp to 0
                mpfr_init0 (&aplVfpRht);

                break;

            defstop
                break;
        } // End IF/SWITCH
RESTART_EXCEPTION:
        // Initialize in case Boolean
        uBitIndex = 0;

        // Loop through the right arg calling the
        //   function strand between data, storing in the
        //   result
        for (uLo = 0; uLo < uDimLo; uLo++)
        for (uHi = 0; uHi < uDimHi; uHi++)
        {
            // Calculate the starting index in the right arg of this vector
            uDimRht = uLo * uDimHi * uDimAxRht + uHi;

            // Calculate the index of last element in this vector
            uRht = uDimRht + (uDimAxRht - 1) * uDimHi;

            // Copy the right arg type to a temp so we can respecify
            //   it in case we blow up
            aplTypeTmp = aplTypeRht;

            // If the left operand is a Primitive Dyadic Scalar function, ...
            if (bPrimDydScal)
            {
                APLINT   aplIntegerLft,             // Left arg as integer
                         aplIntegerRht;             // Right ...
                APLFLOAT aplFloatLft,               // Left arg as float
                         aplFloatRht;               // Right ...
                APLCHAR  aplCharLft,                // Left arg as char
                         aplCharRht;                // Right ...

                // Split cases based upon the right arg storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:
                        tkRhtArg.tkFlags.ImmType  = IMMTYPE_BOOL;
                        tkRhtArg.tkData.tkInteger =
                        aplIntegerRht             = GetNextInteger (lpMemRht, aplTypeRht, uRht);
                        aplFloatRht               = (APLFLOAT) aplIntegerRht;   // In case of type promotion

                        break;

                    case ARRAY_INT:
                        tkRhtArg.tkFlags.ImmType  = IMMTYPE_INT;
                        tkRhtArg.tkData.tkInteger =
                        aplIntegerRht             = ((LPAPLINT)   lpMemRht)[uRht];
                        aplFloatRht               = (APLFLOAT) aplIntegerRht;   // In case of type promotion

                        break;

                    case ARRAY_APA:
                        tkRhtArg.tkFlags.ImmType  = IMMTYPE_INT;
                        tkRhtArg.tkData.tkInteger =
                        aplIntegerRht             = apaOffRht + apaMulRht * uRht;
                        aplFloatRht               = (APLFLOAT) aplIntegerRht;   // In case of type promotion

                        break;

                    case ARRAY_FLOAT:
                        tkRhtArg.tkFlags.ImmType  = IMMTYPE_FLOAT;
                        tkRhtArg.tkData.tkFloat   =
                        aplFloatRht               = ((LPAPLFLOAT) lpMemRht)[uRht];

                        break;

                    case ARRAY_CHAR:
                        tkRhtArg.tkFlags.ImmType  = IMMTYPE_CHAR;
                        tkRhtArg.tkData.tkChar    =
                        aplCharRht                = ((LPAPLCHAR)  lpMemRht)[uRht];

                        break;

                    case ARRAY_RAT:
                        tkRhtArg.tkFlags.ImmType  = IMMTYPE_RAT;
                        lpSymGlbRht               = &((LPAPLRAT)  lpMemRht)[uRht];
                        mpq_set (&aplRatRht, (LPAPLRAT) lpSymGlbRht);

                        break;

                    case ARRAY_VFP:
                        tkRhtArg.tkFlags.ImmType  = IMMTYPE_VFP;
                        lpSymGlbRht               = &((LPAPLVFP)  lpMemRht)[uRht];
                        mpfr_set (&aplVfpRht, (LPAPLVFP) lpSymGlbRht, MPFR_RNDN);

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Split cases based upon the result storage type
                switch (aplTypeRes)
                {
                    case ARRAY_BOOL:
                    case ARRAY_INT:
                    case ARRAY_FLOAT:
                        // Set the token & immediate types in case uDimAxRht EQ 1
                        tkRhtArg.tkFlags.TknType = TKT_VARIMMED;

                        break;

                    case ARRAY_RAT:
                    case ARRAY_VFP:
                        // Set the token & immediate types in case uDimAxRht EQ 1
                        tkRhtArg.tkFlags.TknType = TKT_VARARRAY;

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Loop backwards through the elements along the specified axis
                for (iDim = uDimAxRht - 2, uPrv = uDimRht + iDim * uDimHi; iDim >= 0; iDim--, uPrv -= uDimHi)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

////////////////////// Calculate the index of the previous element in this vector (now done in the FOR stmt)
////////////////////uPrv = uDimRht + iDim * uDimHi;

                    // Split cases based upon the left (actually right) arg storage type
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:
                            aplIntegerLft = GetNextInteger (lpMemRht, aplTypeRht, uPrv);
                            aplFloatLft   = (APLFLOAT) aplIntegerLft;   // In case of type promotion

                            break;

                        case ARRAY_INT:
                            aplIntegerLft = ((LPAPLINT)   lpMemRht)[uPrv];
                            aplFloatLft   = (APLFLOAT) aplIntegerLft;   // In case of type promotion

                            break;

                        case ARRAY_APA:
                            aplIntegerLft = apaOffRht + apaMulRht * uPrv;
                            aplFloatLft   = (APLFLOAT) aplIntegerLft;   // In case of type promotion

                            break;

                        case ARRAY_FLOAT:
                            aplFloatLft   = ((LPAPLFLOAT) lpMemRht)[uPrv];

                            break;

                        case ARRAY_CHAR:
                            aplCharLft    = ((LPAPLCHAR)  lpMemRht)[uPrv];

                            break;

                        case ARRAY_RAT:
                            lpSymGlbLft   = &((LPAPLRAT)  lpMemRht)[uPrv];

                            break;

                        case ARRAY_VFP:
                            lpSymGlbLft   = &((LPAPLVFP)  lpMemRht)[uPrv];

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    // Execute the Primitive Dyadic Scalar function
                    //   between the args
////#define FAST_TEST
#ifdef FAST_TEST
                    aplLongestRht += aplLongestLft;
#else
                    if (!PrimFnDydSiScSiScSub_EM (&tkRhtArg,
                                                  &lpYYFcnStrLft->tkToken,
                                                   NULL,
                                                   aplTypeRes,
                                                   aplTypeRht2,
                                                   aplIntegerLft,
                                                   aplFloatLft,
                                                   aplCharLft,
                                                   lpSymGlbLft,
                                                   aplTypeTmp,
                                                   aplIntegerRht,
                                                   aplFloatRht,
                                                   aplCharRht,
                                                   lpSymGlbRht,
                                                  &aplTypeNew,               // New storage type
                                                   lpPrimSpecLft))
                        goto ERROR_EXIT;

                    Assert (tkRhtArg.tkFlags.TknType EQ TKT_VARIMMED
                         || IsGlbNum (aplTypeRes));

                    // If we blew up from RAT to VFP, ...
                    if (aplTypeRes NE aTypePromote[aplTypeRes][aplTypeNew])
                    {
                        // Initialize the temp to 0
                        mpfr_init0 (&aplVfpRht);

                        // Copy the RAT value
                        mpfr_set_q (&aplVfpRht, &aplRatRht, MPFR_RNDN);

                        // Free the temp
                        Myq_clear (&aplRatRht);

                        // Tell the header about it
                        lpMemHdrRes->ArrType = aplTypeRes = aplTypeNew;
                    } // End IF

                    // Copy the result type as the temporary right arg type
                    aplTypeTmp = aplTypeRes;

                    // Split cases based upon the result storage type
                    switch (aplTypeRes)
                    {
                        case ARRAY_BOOL:
                        case ARRAY_INT:
                        case ARRAY_FLOAT:
                            // Split cases based upon the result immediate type
                            switch (tkRhtArg.tkFlags.ImmType)
                            {
                                case IMMTYPE_BOOL:
                                    aplIntegerRht = (BIT0 &tkRhtArg.tkData.tkBoolean);
                                    aplFloatRht   = (APLFLOAT) aplIntegerRht;   // In case of type promotion

                                    break;

                                case IMMTYPE_INT:
                                    aplIntegerRht = tkRhtArg.tkData.tkInteger;
                                    aplFloatRht   = (APLFLOAT) aplIntegerRht;   // In case of type promotion

                                    break;

                                case IMMTYPE_FLOAT:
                                    aplFloatRht   = tkRhtArg.tkData.tkFloat;
                                    aplTypeRht2   =
                                    aplTypeRes    =
                                    lpMemHdrRes->ArrType = ARRAY_FLOAT;

                                    break;

                                case IMMTYPE_CHAR:                          // Can't happen
                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        case ARRAY_RAT:
                            // Get the global memory handle
                            hGlbTmp = tkRhtArg.tkData.tkGlbData;

                            // Lock the memory to get a ptr to it
                            lpSymGlbRht = MyGlobalLockVar (hGlbTmp);

                            // Skip over the header and dimensions to the data
                            lpSymGlbRht = VarArrayDataFmBase (lpSymGlbRht);

                            // Copy the data
                            mpq_set (&aplRatRht, (LPAPLRAT) lpSymGlbRht);

                            // We no longer need this storage
                            Myq_clear ((LPAPLRAT) lpSymGlbRht);

                            // We no longer need this ptr
                            MyGlobalUnlock (hGlbTmp); lpSymGlbRht = NULL;

                            // We no longer need this storage
                            DbgGlobalFree (hGlbTmp); tkRhtArg.tkData.tkGlbData = hGlbTmp = NULL;

                            // Point to the data
                            lpSymGlbRht = &aplRatRht;

                            break;

                        case ARRAY_VFP:
                            // Get the global memory handle
                            hGlbTmp = tkRhtArg.tkData.tkGlbData;

                            // Lock the memory to get a ptr to it
                            lpSymGlbRht = MyGlobalLockVar (hGlbTmp);

                            // Skip over the header and dimensions to the data
                            lpSymGlbRht = VarArrayDataFmBase (lpSymGlbRht);

                            // Copy the data
                            mpfr_copy (&aplVfpRht, (LPAPLVFP) lpSymGlbRht);

                            // We no longer need this storage
                            Myf_clear ((LPAPLVFP) lpSymGlbRht);

                            // We no longer need this ptr
                            MyGlobalUnlock (hGlbTmp); lpSymGlbRht = NULL;

                            // We no longer need this storage
                            DbgGlobalFree (hGlbTmp); tkRhtArg.tkData.tkGlbData = hGlbTmp = NULL;

                            // Point to the data
                            lpSymGlbRht = &aplVfpRht;

                            break;

                        defstop
                            break;
                    } // End SWITCH
#endif
                } // End FOR

#ifdef FAST_TEST
                // Fill in the result token
                tkRhtArg.tkFlags.TknType  = TKT_VARIMMED;
                tkRhtArg.tkFlags.ImmType  = IMMTYPE_INT;
                tkRhtArg.tkData.tkLongest = aplLongestRht;
#endif
            } else
            {
                // Get the last element as the right arg
                GetNextValueMemIntoToken (uRht,             // Index to use
                                          lpMemRht,         // Ptr to global memory object to index
                                          aplTypeRht,       // Storage type of the arg
                                          aplNELMRht,       // NELM         ...
                                          apaOffRht,        // APA offset (if needed)
                                          apaMulRht,        // APA multiplier (if needed)
                                         &tkRhtArg);        // Ptr to token in which to place the value
                // In case we blew up, check to see if we must blow up tkRhtArg
                if (IsSimpleFlt (aplTypeRes)
                 && IsSimpleInt (aplTypeRht))
                {
                    // Change the immediate type & value
                    tkRhtArg.tkFlags.ImmType = IMMTYPE_FLOAT;
                    tkRhtArg.tkData.tkFloat  = (APLFLOAT) tkRhtArg.tkData.tkInteger;
                } // End IF

                // Loop backwards through the elements along the specified axis
                for (iDim = uDimAxRht - 2; iDim >= 0; iDim--)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Calculate the index of the previous element in this vector
                    uPrv = uDimRht + iDim * uDimHi;

                    // Get the previous element as the left arg
                    GetNextValueMemIntoToken (uPrv,         // Index to use
                                              lpMemRht,     // Ptr to global memory object to index
                                              aplTypeRht,   // Storage type of the arg
                                              aplNELMRht,   // NELM         ...
                                              apaOffRht,    // APA offset (if needed)
                                              apaMulRht,    // APA multiplier (if needed)
                                             &tkLftArg);    // Ptr to token in which to place the value
                    // Execute the left operand between the left & right args
                    if (lpPrimProtoLft NE NULL)
                        // Note that we cast the function strand to LPTOKEN
                        //   to bridge the two types of calls -- one to a primitive
                        //   function which takes a function token, and one to a
                        //   primitive operator which takes a function strand
                        lpYYRes = (*lpPrimProtoLft) (&tkLftArg,     // Ptr to left arg token
                                            (LPTOKEN) lpYYFcnStrLft,// Ptr to left operand function strand
                                                     &tkRhtArg,     // Ptr to right arg token
                                                      NULL);        // Ptr to axis token (may be NULL)
                    else
                        lpYYRes = ExecFuncStr_EM_YY (&tkLftArg,     // Ptr to left arg token
                                                      lpYYFcnStrLft,// Ptr to left operand function strand
                                                     &tkRhtArg,     // Ptr to right arg token
                                                      NULL);        // Ptr to axis token (may be NULL)
                    // Free the left & right arg tokens
                    FreeResultTkn (&tkLftArg);
                    FreeResultTkn (&tkRhtArg);

                    // If it succeeded, ...
                    if (lpYYRes NE NULL)
                    {
                        // Check for NoValue
                        if (IsTokenNoValue (&lpYYRes->tkToken))
                        {
                            // Free the YYRes (but not the storage)
                            YYFree (lpYYRes); lpYYRes = NULL;

                            goto VALUE_EXIT;
                        } // End IF

                        // Copy the result to the right arg token
                        tkRhtArg = lpYYRes->tkToken;

                        // Free the YYRes (but not the storage)
                        YYFree (lpYYRes); lpYYRes = NULL;
                    } else
                        goto ERROR_EXIT;
                } // End FOR
            } // End IF/ELSE

            // Split cases based upon the token type of the right arg (result)
            switch (tkRhtArg.tkFlags.TknType)
            {
                case TKT_VARIMMED:
                    // Split cases based upon the result storage type
                    switch (aplTypeRes)
                    {
                        case ARRAY_BOOL:
                            // Check for blow up (to INT or FLOAT -- can this ever happen??)
                            if (IsImmBool (tkRhtArg.tkFlags.ImmType))
                            {
                                // Save in the result as a BOOL
                                *((LPAPLBOOL) lpMemRes) |= (BIT0 & tkRhtArg.tkData.tkBoolean) << uBitIndex;

                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;              // Start over
                                    ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                                } // End IF
                            } else
                            {
                                // It's now an INT result
                                aplTypeRes = ARRAY_INT;

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

                                goto RESTART_ALLOC;
                            } // End IF/ELSE

                            break;

                        case ARRAY_INT:
                            // Check for blow up (to FLOAT)
                            if (IsImmInt (tkRhtArg.tkFlags.ImmType))
                                // Save in the result as an INT
                                *((LPAPLINT) lpMemRes)++ = tkRhtArg.tkData.tkInteger;
                            else
                            {
                                // It's now a FLOAT result
                                aplTypeRes = ARRAY_FLOAT;

                                // Tell the header about it
                                lpMemHdrRes->ArrType = aplTypeRes;

                                // Restart the pointer
                                lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

                                goto RESTART_EXCEPTION;
                            } // End IF/ELSE

                            break;

                        case ARRAY_FLOAT:
                            // Extract the immediate type as an array type
                            aplTypeTmp = TranslateImmTypeToArrayType (tkRhtArg.tkFlags.ImmType);

                            // In case the current item was demoted in type, we blow it up again to the result
                            (*aTypeActPromote[aplTypeTmp][aplTypeRes]) (&tkRhtArg.tkData.tkLongest, 0, &atTmp);

                            // Save in the result as a FLOAT
                            *((LPAPLFLOAT) lpMemRes)++ = atTmp.aplFloat;

                            break;

                        case ARRAY_NESTED:
                            // Save in the result as an LPSYMENTRY
                            *((LPAPLNESTED) lpMemRes)++ =
                            lpSymTmp =
                              MakeSymEntry_EM (tkRhtArg.tkFlags.ImmType,    // Immediate type
                                              &tkRhtArg.tkData.tkLongest,   // Ptr to immediate value
                                              &lpYYFcnStrOpr->tkToken);     // Ptr to function token
                            if (lpSymTmp EQ NULL)
                                goto ERROR_EXIT;
                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case TKT_VARARRAY:
                    // Split cases based upon the result storage type
                    switch (aplTypeRes)
                    {
                        case ARRAY_BOOL:
                        case ARRAY_INT:
                        case ARRAY_FLOAT:
                        case ARRAY_HETERO:
                        case ARRAY_NESTED:
                            // Save in the result as an HGLOBAL
                            *((LPAPLNESTED) lpMemRes)++ = tkRhtArg.tkData.tkGlbData;

                            break;

                        case ARRAY_RAT:
                            // Copy the data to the result
                            mpq_init_set (((LPAPLRAT) lpMemRes)++, &aplRatRht);

                            break;

                        case ARRAY_VFP:
                            // Copy the data to the result
                            mpfr_init_copy (((LPAPLVFP) lpMemRes)++, &aplVfpRht);

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                defstop
                    break;
            } // End SWITCH

            // Because we didn't CopyArray_EM on the tkGlbData,
            //   we don't need to free tkRhtArg.
        } // End FOR/FOR
    } // End IF/ELSE

    // Unlock the result global memory in case TypeDemote actually demotes
    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
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

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    goto ERROR_EXIT;

LEFT_NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    goto ERROR_EXIT;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    goto ERROR_EXIT;

VALUE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
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

    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            break;

        case ARRAY_RAT:
            // Free the temp
            Myq_clear (&aplRatRht);

            break;

        case ARRAY_VFP:
            // Free the temp
            Myf_clear (&aplVfpRht);

            break;

        case ARRAY_ERROR:
            Assert (lpYYRes EQ NULL);

            break;

        defstop
            break;
    } // End SWITCH

    return lpYYRes;
} // End PrimOpMonSlashCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpRedOfSing_EM_YY
//
//  Handle reduction of a singleton
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpRedOfSing_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpRedOfSing_EM_YY
    (LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPPL_YYSTYPE lpYYFcnStrLft,        // Ptr to left operand
     LPAPLUINT    lpaplAxis,            // Ptr to reduction axis value (may be NULL if scan)
     UBOOL        bScan,                // TRUE iff we should treat as Scan
     UBOOL        bEnclose1st,          // TRUE iff the item must be enclosed before reducing it
     UBOOL        bPrototyping)         // TRUE iff prototyping

{
    LPPRIMFLAGS  lpPrimFlagsLft;        // Ptr to left operand primitive flags
    TOKEN        tkLft = {0},           // Left arg token
                 tkAxis = {0};          // Axis token
    LPTOKEN      lptkLft,               // Ptr to left arg token
                 lptkAxis;              // Ptr to Axis token
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    HGLOBAL      hGlbMFO;               // Magic function/operator global memory handle

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // If we should treat as Scan, ...
    if (bScan)
    {
        // Get the magic function/operator global memory handle
        hGlbMFO = lpMemPTD->hGlbMFO[MFOE_RoS2];

        // Zap the left arg and axis token ptrs
        lptkLft = lptkAxis = NULL;
    } else
    {
        // Get a ptr to the Primitive Function Flags
        lpPrimFlagsLft = GetPrimFlagsPtr (&lpYYFcnStrLft->tkToken);

        // Fill in the left arg token
        tkLft.tkFlags.TknType   = TKT_VARIMMED;
        tkLft.tkFlags.ImmType   = IMMTYPE_BOOL;
////////tkLft.tkFlags.NoDisplay = FALSE;            // Already zero from = {0}
        tkLft.tkData.tkLongest  = bEnclose1st;
////////tkLft.tkCharIndex       =                   // Ignored

        // Fill in the axis token
        tkAxis.tkFlags.TknType   = TKT_VARIMMED;
        tkAxis.tkFlags.ImmType   = IMMTYPE_INT;
////////tkAxis.tkFlags.NoDisplay = FALSE;            // Already zero from = {0}
////////tkAxis.tkData.tkLongest  =                   // Filled in below
////////tkAxis.tkCharIndex       =                   // Ignored

        // If there's an axis value, ...
        if (lpaplAxis NE NULL)
            tkAxis.tkData.tkInteger = GetQuadIO () + *lpaplAxis;
        else
            tkAxis.tkData.tkInteger = -1;

        // Get the magic function/operator global memory handle
        hGlbMFO = lpMemPTD->hGlbMFO[lpPrimFlagsLft->bLftIdent ? MFOE_RoS1L : MFOE_RoS1R];

        // Set the left arg and axis token ptrs
        lptkLft  = &tkLft;
        lptkAxis = &tkAxis;
    } // End IF/ELSE

    return
      ExecuteMagicOperator_EM_YY (lptkLft,                  // Ptr to left arg token
                                 &lpYYFcnStrOpr->tkToken,   // Ptr to operator token
                                  lpYYFcnStrLft,            // Ptr to left operand function strand
                                  lpYYFcnStrOpr,            // Ptr to function strand
                                  NULL,                     // Ptr to right operand function strand (may be NULL)
                                  lptkRhtArg,               // Ptr to right arg token
                                  lptkAxis,                 // Ptr to axis token (may be NULL)
                                  hGlbMFO,                  // Magic function/operator global memory handle
                                  NULL,                     // Ptr to HSHTAB struc (may be NULL)
                                  bPrototyping
                                ? LINENUM_PRO
                                : LINENUM_ONE);             // Starting line # type (see LINE_NUMS)
} // End PrimOpRedOfSing_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpDydSlash_EM_YY
//
//  Primitive operator for dyadic derived function from Slash ("N-wise reduction")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydSlash_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpDydSlashCommon_EM_YY (lptkLftArg,      // Ptr to left arg token (may be NULL if monadic)
                                       lpYYFcnStrOpr,   // Ptr to operator function strand
                                       lptkRhtArg,      // Ptr to right arg token
                                       FALSE);          // TRUE iff prototyping
} // End PrimOpDydSlash_EM_YY


//***************************************************************************
//  $PrimOpDydSlashCommon_EM_YY
//
//  Primitive operator for dyadic derived function from Slash ("N-wise reduction")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydSlashCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpDydSlashCommon_EM_YY
    (LPTOKEN      lptkLftArg,               // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,            // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,               // Ptr to right arg token
     UBOOL        bPrototyping)             // TRUE iff prototyping

{
    APLSTYPE          aplTypeLft,           // Left arg storage type
                      aplTypeRht,           // Right ...
                      aplTypeTmp,           // Temp right arg ...
                      aplTypeRes;           // Result   ...
    APLNELM           aplNELMLft,           // Left arg NELM
                      aplNELMRht,           // Right ...
                      aplNELMRes;           // Result   ...
    APLRANK           aplRankLft,           // Left arg rank
                      aplRankRht,           // Right ...
                      aplRankRes;           // Result   ...
    APLINT            aplIntegerLft,        // Left arg integer
                      aplIntegerLftAbs,     // ...              absolute value
                      apaOffRht,            // Right arg APA offset
                      apaMulRht;            // ...           multiplier
    APLFLOAT          aplFloatLft,          // Left arg float
                      aplFloatIdent;        // Identity element
    APLUINT           aplAxis,              // The (one and only) axis value
                      uLo,                  // uDimLo loop counter
                      uHi,                  // uDimHi ...
                      uDim,                 // Loop counter
                      uDimLo,               // Product of dimensions below axis
                      uDimHi,               // ...                   above ...
                      uDimRht,              // Starting index in right arg of current vector
                      uDimAxRht,            // Right arg axis dimension length
                      uDimAxRes,            // Result    ...
                      uRht,                 // Right arg loop counter
                      uRes;                 // Result    ...
    HGLOBAL           hGlbRht = NULL,       // Right arg global memory handle
                      hGlbRes = NULL,       // Result    ...
                      lpSymGlbLft;          // Ptr to left arg as global numeric
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg header
                      lpMemHdrRes = NULL;   // ...    result    ...
    LPVOID            lpMemRht,             // Ptr to right arg global memory
                      lpMemRes;             // Ptr to result    ...
    LPAPLDIM          lpMemDimRht;          // Ptr to right arg dimensions
    UBOOL             bRet = TRUE,          // TRUE iff result is valid
                      bNstIdent = FALSE,    // TRUE iff reducing an empty nested array with a primitive scalar dyadic fcn
                      bPrimIdent = FALSE;   // TRUE iff reducing an empty array with a primitive
                                            //   or user-defined function/operator
    LPPL_YYSTYPE      lpYYRes = NULL,       // Ptr to the result
                      lpYYRes2,             // Ptr to secondary result
                      lpYYFcnStrLft;        // Ptr to the left operand strand
    TOKEN             tkLftArg = {0},       // Left arg token
                      tkRhtArg = {0},       // Right ...
                      tkFcn = {0};          // The function token
    LPTOKEN           lptkAxisOpr,          // Ptr to operator axis token (may be NULL)
                      lptkAxisLft;          // ...    left operand axis token (may be NULL)
    LPPRIMFNS         lpPrimProtoLft;       // Ptr to left operand prototype function
    LPPRIMSPEC        lpPrimSpecLft;        // Ptr to left operand PRIMSPEC
    LPPRIMFLAGS       lpPrimFlagsLft;       // Ptr to left operand PrimFlags entry
    LPPRIMIDENT       lpPrimIdentLft;       // Ptr to left operand PrimIdent entry
    LPSYMENTRY        lpSymTmp;             // Ptr to temporary LPSYMENTRY
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    ALLTYPES          atTmp = {0};          // Temporary ALLTYPES

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

    // Check for left operand axis operator
    lptkAxisLft = CheckAxisOper (lpYYFcnStrLft);

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Handle prototypes specially
    // Get a ptr to the prototype function for the first symbol (a function or operator)
    if (bPrototyping
     || IsEmpty (aplNELMLft)
     || IsEmpty (aplNELMRht))
    {
        // Get the appropriate prototype function ptr
        lpPrimProtoLft = GetPrototypeFcnPtr (&lpYYFcnStrLft->tkToken);
        if (lpPrimProtoLft EQ NULL)
            goto LEFT_OPERAND_NONCE_EXIT;
    } else
        lpPrimProtoLft = NULL;

    // Check for LEFT RANK ERROR
    if (IsMultiRank (aplRankLft))
        goto LEFT_RANK_EXIT;

    // Check for LEFT LENGTH ERROR
    if (!IsSingleton (aplNELMLft))
        goto LEFT_LENGTH_EXIT;

    // Check for LEFT DOMAIN ERROR
    if (!IsNumeric (aplTypeLft))
        goto LEFT_DOMAIN_EXIT;

    // Get the one (and only) value from the left arg
    GetFirstValueToken (lptkLftArg,         // Ptr to left arg token
                       &aplIntegerLft,      // Ptr to integer result
                       &aplFloatLft,        // Ptr to float ...
                        NULL,               // Ptr to WCHAR ...
                        NULL,               // Ptr to longest ...
                       &lpSymGlbLft,        // Ptr to lpSym/Glb ...
                        NULL,               // Ptr to ...immediate type ...
                        NULL);              // Ptr to array type ...
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
            aplIntegerLft = FloatToAplint_SCT (aplFloatLft, &bRet);

            break;

        case ARRAY_RAT:
            // Attempt to convert the RAT to an integer using System []CT
            aplIntegerLft = mpq_get_sctsx ((LPAPLRAT) lpSymGlbLft, &bRet);

            break;

        case ARRAY_VFP:
            // Attempt to convert the VFP to an integer using System []CT
            aplIntegerLft = mpfr_get_sctsx ((LPAPLVFP) lpSymGlbLft, &bRet);

            break;

        defstop
            break;
    } // End SWITCH

    if (!bRet)
        goto LEFT_DOMAIN_EXIT;

    // Calculate the absolute value of aplIntegerLft
    aplIntegerLftAbs = abs64 (aplIntegerLft);

    // Check for axis present
    if (lptkAxisOpr NE NULL)
    {
        // Reverse allows a single integer axis value only
        if (!CheckAxis_EM (lptkAxisOpr,     // The operator axis token
                           aplRankRht,      // All values less than this
                           TRUE,            // TRUE iff scalar or one-element vector only
                           FALSE,           // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // Return TRUE iff fractional values present
                          &aplAxis,         // Return last axis value
                           NULL,            // Return # elements in axis vector
                           NULL))           // Return HGLOBAL with APLINT axis values
            return NULL;
    } else
    {
        // No axis specified:
        // if Slash, use last dimension
        if (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ INDEX_OPSLASH
         || lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_SLASH)
            aplAxis = max (aplRankRht, 1) - 1;
        else
        {
            Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ INDEX_OPSLASHBAR
                 || lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_SLASHBAR);

            // Otherwise, it's SlashBar on the first dimension
            aplAxis = 0;
        } // End IF/ELSE
    } // End IF/ELSE

    //***************************************************************
    // Calculate product of dimensions before, at, and after the axis dimension
    //***************************************************************

    // Get right arg's global ptr
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // If the right arg is not an immediate, ...
    if (lpMemHdrRht NE NULL)
    {
        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);

        // Calculate the product of the right arg's dimensions below the axis dimension
        uDimLo = 1;
        for (uDim = 0; uDim < aplAxis; uDim++)
            uDimLo *= lpMemDimRht[uDim];

        // Get the length of the axis dimension
        if (IsScalar (aplRankRht))
            uDimAxRht = 1;
        else
            uDimAxRht = lpMemDimRht[uDim];

        // Calculate the product of the right arg's dimensions above the axis dimension
        uDimHi = 1;
        for (uDim++; uDim < aplRankRht; uDim++)
            uDimHi *= lpMemDimRht[uDim];
    } else
        uDimLo = uDimAxRht = uDimHi = 1;

    // Check for more LEFT DOMAIN ERRORs
    if (aplIntegerLftAbs > (APLINT) (1 + uDimAxRht))
        goto LEFT_DOMAIN_EXIT;

    // Calculate the result axis dimension
    uDimAxRes = 1 + uDimAxRht - aplIntegerLftAbs;

    // The rank of the result is the same as that of the right arg
    //   or 1 whichever is higher
    aplRankRes = max (aplRankRht, 1);

    // Get a ptr to the Primitive Function Flags
    lpPrimFlagsLft = GetPrimFlagsPtr (&lpYYFcnStrLft->tkToken);
    if (lpPrimFlagsLft NE NULL)
        lpPrimIdentLft = &PrimIdent[lpPrimFlagsLft->Index];
    else
        lpPrimIdentLft = NULL;

    // If the left arg is zero, or
    //    the absolute value of the left arg is (uDimAxRht + 1),
    //    the result is a reshape of LeftOperandIdentityElement,
    //    so we need that value
    if (aplIntegerLft EQ 0
     || aplIntegerLftAbs EQ (APLINT) (uDimAxRht + 1))
    {
        // If it's an immediate primitive function,
        //   and has an identity element, ...
        if (lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
         && lpPrimFlagsLft->IdentElem)
        {
            // If the right arg is nested, ...
            if (IsNested (aplTypeRht))
            {
                // The result is nested
                aplTypeRes = ARRAY_NESTED;

                // Mark as reducing empty nested array
                //   to produce its identity element
                bNstIdent = TRUE;
            } else
            {
                // Get the identity element
                aplFloatIdent = lpPrimIdentLft->fIdentElem;

                // If the identity element is Boolean or we're prototyping,
                //   the result is, too
                if (lpPrimIdentLft->IsBool
                 || lpPrimProtoLft NE NULL)
                    aplTypeRes = ARRAY_BOOL;
                else
                    aplTypeRes = ARRAY_FLOAT;
            } // End IF/ELSE
        } else
        {
            // The result is nested
            aplTypeRes = ARRAY_NESTED;

            // Mark as reducing empty array with a primitive or
            //   user-defined function/operator
            //   to produce its identity element
            bPrimIdent = TRUE;
        } // End IF/ELSE
    } else
    // If the operand is a primitive scalar dyadic function, and
    //   there's no left operand axis token,
    //   calculate the storage type of the result,
    //   otherwise, assume it's ARRAY_NESTED
    if (lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
     && lptkAxisLft EQ NULL
     && lpPrimFlagsLft->DydScalar)
    {
        // If the function is equal or not-equal, and the right
        //   arg is not Boolean, make the result storage type
        //   nested and let TypeDemote figure it out in the end.
        // This avoids complications with =/3 1{rho}'abc' which
        //   is typed as Boolean but blows up to CHAR.
        if ((lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_EQUAL
          || lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_NOTEQUAL)
         && !IsSimpleBool (aplTypeRht))
            aplTypeRes = ARRAY_NESTED;
        else
        {
            // Get the corresponding lpPrimSpecLft
            lpPrimSpecLft = PrimSpecTab[SymTrans (&lpYYFcnStrLft->tkToken)];

            // Calculate the storage type of the result
            aplTypeRes =
              (*lpPrimSpecLft->StorageTypeDyd) (1,
                                               &aplTypeRht,
                                               &lpYYFcnStrLft->tkToken,
                                                1,
                                               &aplTypeRht);
            if (IsErrorType (aplTypeRes))
                goto DOMAIN_EXIT;
        } // End IF/ELSE
    } else
        // Assume that the result storage type is nested
        //   but we'll call TypeDemote at the end just in case
        aplTypeRes = ARRAY_NESTED;

    // Calculate the result NELM
    aplNELMRes = imul64 (uDimLo, uDimHi, &bRet);
    if (bRet || IsZeroDim (uDimAxRes))
        aplNELMRes = imul64 (aplNELMRes, uDimAxRes, &bRet);
    if (!bRet)
        goto WSFULL_EXIT;

    //***************************************************************
    // Pick off special cases of the left arg
    //***************************************************************

    // If the left arg is zero, or
    //    the absolute value of the left arg is (uDimAxRht + 1),
    //    the result is a reshape of LeftOperandIdentityElement
    if (aplIntegerLft EQ 0
     || aplIntegerLftAbs EQ (APLINT) (uDimAxRht + 1))
    {
        // Allocate storage for the result
        if (!PrimOpDydSlashAllocate_EM
             (aplTypeRht,           // Right arg storage type
              max (aplRankRht, 1),  // Right arg rank
              aplTypeRes,           // Result storage type
              aplNELMRes,           // Result NELM
              aplRankRes,           // Result rank
              lpMemHdrRht,          // Ptr to right arg global memory
              lpMemDimRht,          // Ptr to right arg dimensions
              aplAxis,              // The (one and only) axis value
              uDimAxRes,            // Result axis value dimension length

             &hGlbRes,              // Ptr to result global memory handle
             &lpMemRes,             // Ptr to ptr to result global memory
             &apaOffRht,            // Ptr to right arg APA offset
             &apaMulRht,            // ...                  multiplier
              lpYYFcnStrOpr))       // Ptr to operator function strand
            goto ERROR_EXIT;

        // Fill in the data values (i.e., replicate the identity element)

        // If the function is a primitive or user-defined function/operator, ...
        if (bPrimIdent)
        {
            if (!FillIdentityElement_EM (lpMemRes,              // Ptr to result global memory
                                         aplNELMRes,            // Result NELM
                                         aplTypeRes,            // Result storage type
                                         lpYYFcnStrLft,         // Ptr to left operand function strand
                                         lptkLftArg,            // Ptr to left arg token
                                         NULL,                  // Ptr to right operand function strand (may be NULL if Scan)
                                         lptkRhtArg))           // Ptr to right arg token
                goto ERROR_EXIT;
        } else
        // If the right arg is nested, ...
        if (bNstIdent)
        {
            APLLONGEST aplLongestIdn;       // Identity immediate value
            HGLOBAL    hGlbIdn;             // Identity element global memory handle
            LPVOID     hSymGlbIdn;          // ...               LPSYMENTRY or HGLOBAL
            APLSTYPE   aplTypeIdn;          // ...               storage type

            // The result common element is {first} f/[X] 0/[X] R

            // Fill in the left arg token
            tkLftArg.tkFlags.TknType   = TKT_VARIMMED;
            tkLftArg.tkFlags.ImmType   = IMMTYPE_BOOL;
////////////tkLftArg.tkFlags.NoDisplay = FALSE;     // Already zero from {0}
////////////tkLftArg.tkData.tkBoolean  = 0;         // Already zero from {0}
            tkLftArg.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

            // Setup the function token
            tkFcn.tkFlags.TknType   = TKT_FCNIMMED;
            tkFcn.tkFlags.ImmType   = IMMTYPE_PRIMFCN;
////////////tkFcn.tkFlags.NoDisplay = FALSE;            // Already zero from = {0}
            tkFcn.tkData.tkChar     = (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ INDEX_OPSLASHBAR) ? UTF16_SLASHBAR
                                                                                                 : UTF16_SLASH;
            tkFcn.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

            // Compress the right arg
            lpYYRes =
              PrimFnDydSlash_EM_YY(&tkLftArg,
                                   &tkFcn,
                                    lptkRhtArg,
                                    lptkAxisOpr);
            if (lpYYRes EQ NULL)
                goto ERROR_EXIT;
#ifdef DEBUG
            // Decrement the SI level of lpYYRes so YYResIsEmpty won't complain
            lpYYRes->SILevel--;
#endif
            // Reduce that result by the left operand
            lpYYRes2 =
              PrimOpMonSlashCommon_EM_YY (lpYYFcnStrOpr,
                                         &lpYYRes->tkToken,
                                          TRUE);
#ifdef DEBUG
            // Restore the SI level of lpYYRes
            lpYYRes->SILevel++;
#endif
            // Free the YYRes (and the storage)
            FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;

            if (lpYYRes2 EQ NULL)
                goto ERROR_EXIT;
#ifdef DEBUG
            // Decrement the SI level of lpYYRes so YYResIsEmpty won't complain
            lpYYRes2->SILevel--;
#endif
            // Extract the identity element
            lpYYRes =
              PrimFnMonUpArrow_EM_YY (&lpYYFcnStrLft->tkToken,
                                      &lpYYRes2->tkToken,
                                       NULL);
#ifdef DEBUG
            // Restore the SI level of lpYYRes
            lpYYRes2->SILevel++;
#endif
            // Free the YYRes (and the storage)
            FreeResult (lpYYRes2); YYFree (lpYYRes2); lpYYRes2 = NULL;

            if (lpYYRes EQ NULL)
                goto ERROR_EXIT;

            // Get identity element's global ptr (if any)
            aplLongestIdn = GetGlbPtrs (&lpYYRes->tkToken, &hGlbIdn);

            // If the result is empty, ...
            if (IsEmpty (aplNELMRes))
            {
                // If the result is global, ...
                if (hGlbIdn NE NULL)
                    // Make the prototype
                    hSymGlbIdn =
                      MakeMonPrototype_EM_PTB (MakePtrTypeGlb (hGlbIdn),    // Proto arg handle
                                              &lpYYFcnStrLft->tkToken,      // Ptr to function token
                                               MP_CHARS);                   // CHARs allowed
                else
                {
                    // Get the attributes (Type, NELM, and Rank) of the identity element
                    AttrsOfToken (&lpYYRes2->tkToken, &aplTypeIdn, NULL, NULL, NULL);

                    // If the identity element is numeric
                    if (IsImmNum (aplTypeIdn))
                    {
                        // Use Boolean zero
                        aplTypeIdn = ARRAY_BOOL;
                        aplLongestIdn = 0;
                    } else
                    {
                        // Use character blank
                        aplTypeIdn = ARRAY_CHAR;
                        aplLongestIdn = L' ';
                    } // End IF/ELSE

                    // Make the prototype
                    hSymGlbIdn =
                      MakeSymEntry_EM (TranslateArrayTypeToImmType (aplTypeIdn),
                                      &aplLongestIdn,
                                      &lpYYFcnStrLft->tkToken);
                } // End IF/ELSE
            } else
            // If the result is global, ...
            if (hGlbIdn NE NULL)
                hSymGlbIdn = CopySymGlbDirAsGlb (hGlbIdn);
            else
            {
                // Get the attributes (Type, NELM, and Rank) of the identity element
                AttrsOfToken (&lpYYRes2->tkToken, &aplTypeIdn, NULL, NULL, NULL);

                // Convert the immediate value to a SYMENTRY
                hSymGlbIdn =
                  MakeSymEntry_EM (TranslateArrayTypeToImmType (aplTypeIdn),
                                  &aplLongestIdn,
                                  &lpYYFcnStrLft->tkToken);
            } // End IF/ELSE/...

            // Free the YYRes (and the storage)
            FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;

            if (hSymGlbIdn EQ NULL)
                goto ERROR_EXIT;

            // Save the identity element in the result
            *((LPAPLNESTED) lpMemRes)++ = hSymGlbIdn;

            for (uRes = 1; uRes < aplNELMRes; uRes++)
                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (hSymGlbIdn);
        } else
        {
            // The zero case is done (already zero from GHND)

            // Check for Boolean identity element
            if (lpPrimIdentLft->IsBool)
            {
                // Check for identity element 1
                if (lpPrimIdentLft->bIdentElem)
                {
                    APLNELM uNELMRes;

                    // Calculate the # bytes in the result, rounding up
                    uNELMRes = (aplNELMRes + (NBIB - 1)) >> LOG2NBIB;

                    for (uRes = 0; uRes < uNELMRes; uRes++)
                        *((LPAPLBOOL) lpMemRes)++ = 0xFF;
                } // End IF
            } else
            for (uRes = 0; uRes < aplNELMRes; uRes++)
                *((LPAPLFLOAT) lpMemRes)++ = aplFloatIdent;
        } // End IF/ELSE
    } else
    // If the absolute value of the left arg is one, ...
    if (aplIntegerLftAbs EQ 1 && hGlbRht)
    {
        LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
        HGLOBAL      hGlbMFO;               // Magic function/operator global memory handle

        // Get ptr to PerTabData global memory
        lpMemPTD = GetMemPTD ();

        // Get the magic function/operator global memory handle
        hGlbMFO = lpMemPTD->hGlbMFO[MFOE_RoS3];

        // The result is {disclose} {each} LftOpr / {each}                  1/R  (R simple)
        // The result is {disclose} {each} LftOpr / {each} {enclose} {each} 1/R  (R nested)
        lpYYRes =
          ExecuteMagicOperator_EM_YY (NULL,                     // Ptr to left arg token
                                     &lpYYFcnStrOpr->tkToken,   // Ptr to function token
                                      lpYYFcnStrLft,            // Ptr to left operand function strand
                                      lpYYFcnStrOpr,            // Ptr to function strand
                                      NULL,                     // Ptr to right operand function strand (may be NULL)
                                      lptkRhtArg,               // Ptr to right arg token
                                      NULL,                     // Ptr to axis token
                                      hGlbMFO,                  // Magic function/operator global memory handle
                                      NULL,                     // Ptr to HSHTAB struc (may be NULL)
                                      LINENUM_ONE);             // Starting line # type (see LINE_NUMS)
        if (lpYYRes EQ NULL)
            goto ERROR_EXIT;
        else
            goto NORMAL_EXIT;
    } else
    // If the left arg is uDimAxRht, the result is
    //   ({rho} Result) {rho} LeftOperand /[X] RightArg
    //   where ({rho} Result) is ({rho} RightArg) with
    //   ({rho} RightArg})[X] set to one.
    if (aplIntegerLft EQ (APLINT) uDimAxRht)
    {
        // Reduce the right arg
        lpYYRes = PrimOpMonSlashCommon_EM_YY (lpYYFcnStrOpr,    // Ptr to operator function strand
                                              lptkRhtArg,       // Ptr to right arg
                                              bPrototyping);    // TRUE iff prototyping
        // Insert a unit dimension into the result
        if (!PrimOpDydSlashInsertDim_EM (lpYYRes,       // Ptr to the result
                                         aplAxis,       // The (one and only) axis value
                                         uDimAxRes,     // Result axis dimension length
                                        &hGlbRes,       // Ptr to the result global memory handle
                                         lpYYFcnStrOpr))// Ptr to operator function strand
            goto ERROR_EXIT;
        else
            goto NORMAL_EXIT;
    } else
    // If the left arg is -uDimAxRht, the result is
    //   ({rho} Result) {rho} LeftOperand /[X] {reverse}[X] RightArg
    //   where ({rho} Result) is ({rho} RightArg) with
    //   ({rho} RightArg})[X] set to one
    if (aplIntegerLft EQ -(APLINT) uDimAxRht)
    {
        LPPL_YYSTYPE lpYYRes2;

        // Setup the function token
        tkFcn.tkFlags.TknType   = TKT_FCNIMMED;
        tkFcn.tkFlags.ImmType   = IMMTYPE_PRIMFCN;
////////tkFcn.tkFlags.NoDisplay = FALSE;            // Already zero from = {0}
        tkFcn.tkData.tkChar     = (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ INDEX_OPSLASHBAR) ? UTF16_CIRCLEBAR
                                                                                             : UTF16_CIRCLESTILE;
        tkFcn.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

        // Reverse the right arg along the specified axis
        lpYYRes2 = PrimFnMonCircleStile_EM_YY (&tkFcn,                  // Ptr to function token
                                                lptkRhtArg,             // Ptr to right arg token
                                                lptkAxisOpr);           // Ptr to operator axis token (may be NULL)
        // If it failed, ...
        if (lpYYRes2 EQ NULL)
            goto ERROR_EXIT;

        // Reduce the reversed right arg along the specified axis
        lpYYRes = PrimOpMonSlashCommon_EM_YY (lpYYFcnStrOpr,        // Ptr to operator function strand
                                             &lpYYRes2->tkToken,    // Ptr to right arg
                                              bPrototyping);        // TRUE iff prototyping
        // Free the result of the function execution
        FreeResult (lpYYRes2); YYFree (lpYYRes2); lpYYRes2 = NULL;

        // Insert a unit dimension into the result
        if (!PrimOpDydSlashInsertDim_EM (lpYYRes,       // Ptr to the result
                                         aplAxis,       // The (one and only) axis value
                                         uDimAxRes,     // Result axis dimension length
                                        &hGlbRes,       // Ptr to the result global memory handle
                                         lpYYFcnStrOpr))// Ptr to operator function strand
            goto ERROR_EXIT;
        else
            goto NORMAL_EXIT;
    } else
    {
        // Otherwise, we're out of special cases
RESTART_ALLOC:
        // Allocate storage for the result
        if (!PrimOpDydSlashAllocate_EM
             (aplTypeRht,           // Right arg storage type
              max (aplRankRht, 1),  // Right arg rank
              aplTypeRes,           // Result storage type
              aplNELMRes,           // Result NELM
              aplRankRes,           // Result rank
              lpMemHdrRht,          // Ptr to right arg global memory
              lpMemDimRht,          // Ptr to right arg dimensions
              aplAxis,              // The (one and only) axis value
              uDimAxRes,            // Result axis value dimension length

             &hGlbRes,              // Ptr to result global memory handle
             &lpMemRes,             // Ptr to ptr to result global memory
             &apaOffRht,            // Ptr to right arg APA offset
             &apaMulRht,            // ...                  multiplier
              lpYYFcnStrOpr))       // Ptr to operator function strand
            goto ERROR_EXIT;

        // Save a ptr to the array header
        // Note that PrimOpDydSlashAllocate returns a ptr to the data
        //   so we have to relock/unlock
        lpMemHdrRes = MyGlobalLockVar (hGlbRes); MyGlobalUnlock (hGlbRes);

        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

        // Fill in the right arg token
////////tkRhtArg.tkFlags.TknType   =            // To be filled in below
////////tkRhtArg.tkFlags.ImmType   =            // To be filled in below
////////tkRhtArg.tkFlags.NoDisplay = FALSE;     // Already zero from {0}
////////tkRhtArg.tkData.tkGlbData  =            // To be filled in below
        tkRhtArg.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

        // Fill in the left arg token
////////tkLftArg.tkFlags.TknType   =            // To be filled in below
////////tkLftArg.tkFlags.ImmType   =            // To be filled in below
////////tkLftArg.tkFlags.NoDisplay = FALSE;     // Already zero from {0}
////////tkLftArg.tkData.tkGlbData  =            // To be filled in below
        tkLftArg.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;
RESTART_EXCEPTION:
        // Loop through the right arg calling the
        //   function strand between data, storing in the
        //   result.
        for (uLo = 0; uLo < uDimLo; uLo++)
        for (uHi = 0; uHi < uDimHi; uHi++)
        {
            APLUINT uAx;            // Loop counter for uDimAxRes
            APLINT  iDim;           // Loop counter for aplIntegerLftAbs

            // Calculate the starting index in the right arg of this vector
            uDimRht = uLo * uDimHi * uDimAxRht + uHi;

            // There are uDimAxRes elements in the result to be generated now
            for (uAx = 0; uAx < uDimAxRes; uAx++)
            {
                // Reduce the aplIntegerLftAbs values starting at
                //   uRht = uDimRht + uAx * uDimHi;
                // through
                //   uRht = uDimRht + (uAx + aplIntegerLftAbs - 1) * uDimHi;

                // Split cases based upon the sign of the left arg
                if (aplIntegerLft > 0)
                {
                    // Calculate the index of last element in this vector
                    uRht = uDimRht + (uAx + aplIntegerLftAbs - 1) * uDimHi;

                    // Get the last element as the right arg
                    GetNextValueMemIntoToken (uRht,             // Index to use
                                              lpMemRht,         // Ptr to global memory object to index
                                              aplTypeRht,       // Storage type of the arg
                                              aplNELMRht,       // NELM         ...
                                              apaOffRht,        // APA offset (if needed)
                                              apaMulRht,        // APA multiplier (if needed)
                                             &tkRhtArg);        // Ptr to token in which to place the value
                    // In case we blew up, check to see if we must blow up tkRhtArg
                    if (aplTypeRes NE aplTypeRht)
                        (*aTypeTknPromote[aplTypeRht][aplTypeRes]) (&tkRhtArg);

                    // Loop backwards through the elements along the specified axis
                    for (iDim = aplIntegerLftAbs - 2; iDim >= 0; iDim--)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Calculate the index of the previous element in this vector
                        uRht = uDimRht + (uAx + iDim) * uDimHi;

                        // Get the previous element as the left arg
                        GetNextValueMemIntoToken (uRht,         // Index to use
                                                  lpMemRht,     // Ptr to global memory object to index
                                                  aplTypeRht,   // Storage type of the arg
                                                  aplNELMRht,   // NELM         ...
                                                  apaOffRht,    // APA offset (if needed)
                                                  apaMulRht,    // APA multiplier (if needed)
                                                 &tkLftArg);    // Ptr to token in which to place the value
                        // Execute the left operand between the left & right args
                        if (lpPrimProtoLft NE NULL)
                            // Note that we cast the function strand to LPTOKEN
                            //   to bridge the two types of calls -- one to a primitive
                            //   function which takes a function token, and one to a
                            //   primitive operator which takes a function strand
                            lpYYRes = (*lpPrimProtoLft) (&tkLftArg,         // Ptr to left arg token
                                                (LPTOKEN) lpYYFcnStrLft,    // Ptr to left operand function strand
                                                         &tkRhtArg,         // Ptr to right arg token
                                                          lptkAxisOpr);     // Ptr to operator axis token (may be NULL)
                        else
                            lpYYRes = ExecFuncStr_EM_YY (&tkLftArg,         // Ptr to left arg token
                                                          lpYYFcnStrLft,    // Ptr to function strand
                                                         &tkRhtArg,         // Ptr to right arg token
                                                          lptkAxisOpr);     // Ptr to operator axis token (may be NULL)
                        // Free the left & right arg tokens
                        FreeResultTkn (&tkRhtArg);
                        FreeResultTkn (&tkLftArg);

                        // If it succeeded, ...
                        if (lpYYRes NE NULL)
                        {
                            // Copy the result to the right arg token
                            tkRhtArg = lpYYRes->tkToken;

                            // Free the YYRes (but not the storage)
                            YYFree (lpYYRes); lpYYRes = NULL;
                        } else
                            goto ERROR_EXIT;
                    } // End FOR
                } else
                {
                    // Calculate the index of first element in this vector
                    uRht = uDimRht + uAx * uDimHi;

                    // Get the first element as the right arg
                    GetNextValueMemIntoToken (uRht,             // Index to use
                                              lpMemRht,         // Ptr to global memory object to index
                                              aplTypeRht,       // Storage type of the arg
                                              aplNELMRht,       // NELM         ...
                                              apaOffRht,        // APA offset (if needed)
                                              apaMulRht,        // APA multiplier (if needed)
                                             &tkRhtArg);        // Ptr to token in which to place the value
                    // In case we blew up, check to see if we must blow up tkRhtArg
                    if (IsSimpleFlt (aplTypeRes)
                     && IsSimpleInt (aplTypeRht))
                    {
                        // Change the immediate type & value
                        tkRhtArg.tkFlags.ImmType = IMMTYPE_FLOAT;
                        tkRhtArg.tkData.tkFloat  = (APLFLOAT) tkRhtArg.tkData.tkInteger;
                    } // End IF

                    // Loop forwards through the elements along the specified axis
                    for (iDim = 1; iDim < aplIntegerLftAbs; iDim++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Calculate the index of the next element in this vector
                        uRht = uDimRht + (uAx + iDim) * uDimHi;

                        // Get the next element as the left arg
                        GetNextValueMemIntoToken (uRht,         // Index to use
                                                  lpMemRht,     // Ptr to global memory object to index
                                                  aplTypeRht,   // Storage type of the arg
                                                  aplNELMRht,   // NELM         ...
                                                  apaOffRht,    // APA offset (if needed)
                                                  apaMulRht,    // APA multiplier (if needed)
                                                 &tkLftArg);    // Ptr to token in which to place the value
                        // Execute the left operand between the left & right args
                        if (lpPrimProtoLft NE NULL)
                            // Note that we cast the function strand to LPTOKEN
                            //   to bridge the two types of calls -- one to a primitive
                            //   function which takes a function token, and one to a
                            //   primitive operator which takes a function strand
                            lpYYRes = (*lpPrimProtoLft) (&tkLftArg,         // Ptr to left arg token
                                                (LPTOKEN) lpYYFcnStrLft,    // Ptr to left operand function strand
                                                         &tkRhtArg,         // Ptr to right arg token
                                                          lptkAxisOpr);     // Ptr to operator axis token (may be NULL)
                        else
                            lpYYRes = ExecFuncStr_EM_YY (&tkLftArg,         // Ptr to left arg token
                                                          lpYYFcnStrLft,    // Ptr to function strand
                                                         &tkRhtArg,         // Ptr to right arg token
                                                          lptkAxisOpr);     // Ptr to operator axis token (may be NULL)
                        // Free the left & right arg tokens
                        FreeResultTkn (&tkRhtArg);
                        FreeResultTkn (&tkLftArg);

                        // If it succeeded, ...
                        if (lpYYRes NE NULL)
                        {
                            // Copy the result to the right arg token
                            tkRhtArg = lpYYRes->tkToken;

                            // Free the YYRes (but not the storage)
                            YYFree (lpYYRes); lpYYRes = NULL;
                        } else
                            goto ERROR_EXIT;
                    } // End FOR
                } // End IF/ELSE

                // Initialize index into the result
                uRes = uHi + uDimHi * (uAx + uDimAxRes * uLo);;

                // Split cases based upon the token type of the right arg (result)
                switch (tkRhtArg.tkFlags.TknType)
                {
                    case TKT_VARIMMED:
                        // Split cases based upon the result storage type
                        switch (aplTypeRes)
                        {
                            case ARRAY_BOOL:
                                // Check for blow up (to INT or FLOAT -- can this ever happen??)
                                if (IsImmBool (tkRhtArg.tkFlags.ImmType))
                                    // Save in the result as a BOOL
                                    ((LPAPLBOOL) lpMemRes)[uRes >> LOG2NBIB] |=
                                      (BIT0 & tkRhtArg.tkData.tkBoolean) << (MASKLOG2NBIB & (UINT) uRes);
                                else
                                {
                                    // It's now an INT result
                                    aplTypeRes = ARRAY_INT;

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

                                    goto RESTART_ALLOC;
                                } // End IF/ELSE

                                break;

                            case ARRAY_INT:
                                // Check for blow up (to FLOAT)
                                if (IsImmInt (tkRhtArg.tkFlags.ImmType))
                                    // Save in the result as an INT
                                    ((LPAPLINT) lpMemRes)[uRes] = tkRhtArg.tkData.tkInteger;
                                else
                                {
                                    // It's now a FLOAT result
                                    aplTypeRes = ARRAY_FLOAT;

                                    // Tell the header about it
                                    lpMemHdrRes->ArrType = aplTypeRes;

                                    // Restart the pointer
                                    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

                                    goto RESTART_EXCEPTION;
                                } // End IF/ELSE

                                break;

                            case ARRAY_FLOAT:
                                // Extract the immediate type as an array type
                                aplTypeTmp = TranslateImmTypeToArrayType (tkRhtArg.tkFlags.ImmType);

                                // In case the current item was demoted in type, we blow it up again to the result
                                (*aTypeActPromote[aplTypeTmp][aplTypeRes]) (&tkRhtArg.tkData.tkLongest, 0, &atTmp);

                                // Save in the result as a FLOAT
                                ((LPAPLFLOAT) lpMemRes)[uRht] = atTmp.aplFloat;

                                break;

                            case ARRAY_NESTED:
                                // Save in the result as an LPSYMENTRY
                                ((LPAPLNESTED) lpMemRes)[uRes] =
                                lpSymTmp =
                                  MakeSymEntry_EM (tkRhtArg.tkFlags.ImmType,    // Immediate type
                                                  &tkRhtArg.tkData.tkLongest,   // Ptr to immediate value
                                                  &lpYYFcnStrOpr->tkToken);     // Ptr to function token
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                                break;

                            defstop
                                break;
                        } // End SWITCH

                        break;

                    case TKT_VARARRAY:
                        // Split cases based upon the result storage type
                        switch (aplTypeRes)
                        {
                            LPAPLRAT lpMemRat;
                            LPAPLVFP lpMemVfp;

                            case ARRAY_RAT:
                                // Lock the memory to get a ptr to it
                                lpMemRat = MyGlobalLockVar (tkRhtArg.tkData.tkGlbData);

                                // Skip over the header and dimensions to the data
                                lpMemRat = VarArrayDataFmBase (lpMemRat);

                                ((LPAPLRAT) lpMemRes)[uRes] = *lpMemRat;

                                // We no longer need this ptr
                                MyGlobalUnlock (tkRhtArg.tkData.tkGlbData); lpMemRat = NULL;

                                // We no longer need this storage
                                DbgGlobalFree (tkRhtArg.tkData.tkGlbData); tkRhtArg.tkData.tkGlbData = NULL;

                                break;

                            case ARRAY_VFP:
                                // Lock the memory to get a ptr to it
                                lpMemVfp = MyGlobalLockVar (tkRhtArg.tkData.tkGlbData);

                                // Skip over the header and dimensions to the data
                                lpMemVfp = VarArrayDataFmBase (lpMemVfp);

                                ((LPAPLVFP) lpMemRes)[uRes] = *lpMemVfp;

                                // We no longer need this ptr
                                MyGlobalUnlock (tkRhtArg.tkData.tkGlbData); lpMemVfp = NULL;

                                // We no longer need this storage
                                DbgGlobalFree (tkRhtArg.tkData.tkGlbData); tkRhtArg.tkData.tkGlbData = NULL;

                                break;

                            case ARRAY_NESTED:
                                // Save in the result as an HGLOBAL
                                ((LPAPLNESTED) lpMemRes)[uRes] = tkRhtArg.tkData.tkGlbData;

                                break;

                            defstop
                                break;
                        } // End SWITCH

                        break;

                    case TKT_VARNAMED:
                        // Check for NoValue
                        Assert (IsTokenNoValue (&tkRhtArg));

                        goto VALUE_EXIT;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR
        } // End FOR/FOR
    } // End IF/ELSE/.../FOR/FOR

    // Unlock the result global memory in case TypeDemote actually demotes
    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
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

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

LEFT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    goto ERROR_EXIT;

VALUE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (lpYYRes NE NULL)
    {
        YYFree (lpYYRes); lpYYRes = NULL;
    } // End IF

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
} // End PrimOpDydSlashCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpDydSlashInsertDim_EM
//
//  Insert a unit dimension into a result
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydSlashInsertDim_EM_YY"
#else
#define APPEND_NAME
#endif

UBOOL PrimOpDydSlashInsertDim_EM
    (LPPL_YYSTYPE lpYYRes,          // Ptr to the result
     APLUINT      aplAxis,          // The (one and only) axis value
     APLUINT      uDimAxRes,        // Result axis dimension length
     HGLOBAL     *lphGlbRes,        // Ptr to result global memory handle
     LPPL_YYSTYPE lpYYFcnStrOpr)    // Ptr to operator function strand

{
    HGLOBAL           hGlbTmp;              // Temporary global memory handle
    APLUINT           ByteRes;              // # bytes in the result
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to result header
    LPVOID            lpMemRes;             // Ptr to result global memory

    // If it failed, ...
    if (lpYYRes EQ NULL)
        return FALSE;

    // Check for immediate result
    if (lpYYRes->tkToken.tkFlags.TknType EQ TKT_VARIMMED)
    {
        IMM_TYPES  immType;         // Datatype of immediate result (see IMM_TYPES)
        APLLONGEST aplLongest;      // Value of immediate result
        APLUINT    ByteRes;         // # bytes in the result
        APLSTYPE   aplTypeRes;      // Result storage type

        // Save the immediate type
        immType = lpYYRes->tkToken.tkFlags.ImmType;

        // Save the value
        aplLongest = lpYYRes->tkToken.tkData.tkLongest;

        // Calculate the result storage type
        aplTypeRes = TranslateImmTypeToArrayType (immType);

        // Calculate space needed for the result
        ByteRes = CalcArraySize (aplTypeRes, 1, 1); // One-element vector

        // Check for overflow
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;

        // Allocate space for the result
        hGlbTmp = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (hGlbTmp EQ NULL)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLock000 (hGlbTmp);

#define lpHeader    lpMemHdrRes
        // Fill in the header values
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = aplTypeRes;
////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////lpHeader->SysVar     = FALSE;       // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = 1;
        lpHeader->Rank       = 1;
#undef  lpHeader

        // Fill in the dimension
        *VarArrayBaseToDim (lpMemHdrRes) = 1;

        // Skip over the header and dimension
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

        // Fill in the value
        // Split cases based upon the immediate type
        switch (immType)
        {
            case IMMTYPE_BOOL:
                *((LPAPLBOOL)  lpMemRes) = (APLBOOL) aplLongest;

                break;

            case IMMTYPE_INT:
                *((LPAPLINT)   lpMemRes) = (APLINT) aplLongest;

                break;

            case IMMTYPE_FLOAT:
                *((LPAPLFLOAT) lpMemRes) = *(LPAPLFLOAT) &aplLongest;

                break;

            case IMMTYPE_CHAR:
                *((LPAPLCHAR)  lpMemRes) = (APLCHAR) aplLongest;

                break;

            defstop
                break;
        } // End SWITCH

        // We no longer need this ptr
        MyGlobalUnlock (hGlbTmp); lpMemHdrRes = NULL;

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
        lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbTmp);
////////lpYYRes->tkToken.tkCharIndex       =        // Already filled in by caller

        return TRUE;
    } // End IF

    // Get the global memory handle
    hGlbTmp = lpYYRes->tkToken.tkData.tkGlbData;

    Assert (IsGlbTypeVarDir_PTB (hGlbTmp));

    // The result is the same as lpYYRes except we need
    //   to insert a unit dimension between aplAxis and
    //   aplAxis + 1.

    // Get the size of the temporary global memory
    //   plus a new dimension
    ByteRes = MyGlobalSize (hGlbTmp) + sizeof (APLDIM);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Resize the global memory to include a new dimension
    //   moving the old data to the new location, and
    //   freeing the old global memory
    *lphGlbRes =
      MyGlobalReAlloc (hGlbTmp,
            (APLU3264) ByteRes,
                       GHND);
    // Check for errors
    if (*lphGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Make it a global ptr type
    *lphGlbRes = MakePtrTypeGlb (*lphGlbRes);

    // Check to see if GlobalReAlloc returns the same handle
    if (ClrPtrTypeDir (hGlbTmp) NE *lphGlbRes)
        // Save back into the result
        lpYYRes->tkToken.tkData.tkGlbData = *lphGlbRes;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);

    // Move the memory upwards to make room for a new dimension
    // Use MoveMemory as the source and destin blocks overlap
    // Note that ByteRes already has one sizeof (APLDIM) included
    //   in it, so the third arg below needs to subtract it out
    //   which is why the trailing "* (aplAxis + 1)" is used rather
    //   than the more logical "* aplAxis".
    MoveMemory (ByteAddr (lpMemHdrRes,  sizeof (VARARRAY_HEADER) + sizeof (APLDIM) * (aplAxis + 1)),
                ByteAddr (lpMemHdrRes,  sizeof (VARARRAY_HEADER) + sizeof (APLDIM) *  aplAxis     ),
                (APLU3264) (ByteRes  - (sizeof (VARARRAY_HEADER) + sizeof (APLDIM) * (aplAxis + 1))));
    // Increase the rank by one
    lpMemHdrRes->Rank++;

    // Insert the new dimension
    (VarArrayBaseToDim (lpMemHdrRes))[aplAxis] = uDimAxRes;

    // We no longer need this ptr
    MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;

    return TRUE;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    return FALSE;
} // End PrimOpDydSlashInsertDim_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpDydSlashAllocate_EM
//
//  Allocate storage for the result
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydSlashAllocate_EM_YY"
#else
#define APPEND_NAME
#endif

UBOOL PrimOpDydSlashAllocate_EM
    (APLRANK           aplTypeRht,          // Right arg storage type
     APLRANK           aplRankRht,          // Right arg rank
     APLSTYPE          aplTypeRes,          // Result storage type
     APLNELM           aplNELMRes,          // Result NELM
     APLRANK           aplRankRes,          // Result rank
     LPVARARRAY_HEADER lpMemHdrRht,         // Ptr to right arg header
     LPAPLDIM          lpMemDimRht,         // Ptr to right arg dimensions
     APLUINT           aplAxis,             // Axis value
     APLUINT           uDimAxRes,           // Result axis value
     HGLOBAL          *lphGlbRes,           // Ptr to result global memory handle
     LPVOID           *lplpMemRes,          // Ptr to ptr to result global memory
     LPAPLINT          lpapaOffRht,         // Ptr to right arg APA offset
     LPAPLINT          lpapaMulRht,         // ...                  multiplier
     LPPL_YYSTYPE      lpYYFcnStrOpr)       // Ptr to operator function strand

{
    APLUINT  ByteRes,           // # bytes in the result
             uDim;              // Dimension loop counter
    LPAPLAPA lpMemRht;          // Ptr to APA data

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    *lphGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (*lphGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    *lplpMemRes = MyGlobalLock000 (*lphGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) *lplpMemRes)
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Skip over the header to the dimensions
    *lplpMemRes = VarArrayBaseToDim (*lplpMemRes);

    // Copy the dimensions from the right arg to the result
    //   except for the axis dimension
    for (uDim = 0; uDim < aplRankRht; uDim++)
    if (uDim EQ aplAxis)
        *((LPAPLDIM) *lplpMemRes)++ = uDimAxRes;
    else
        *((LPAPLDIM) *lplpMemRes)++ = lpMemDimRht[uDim];

    // lpMemRes now points to its data

    // If the right arg is an APA, ...
    if (IsSimpleAPA (aplTypeRht))
    {
        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

#define lpAPA       lpMemRht
        // Get the APA parameters
        *lpapaOffRht = lpAPA->Off;
        *lpapaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    return TRUE;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    return FALSE;
} // End PrimOpDydSlashAllocate_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: po_slash.c
//***************************************************************************
