//***************************************************************************
//  NARS2000 -- Primitive Operator -- Slope
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2012 Sudley Place Software

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
//  $PrimOpSlope_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    monadic operator Slope ("scan" and ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpSlope_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpSlope_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ INDEX_OPSLOPE
         || lpYYFcnStrOpr->tkToken.tkData.tkChar EQ INDEX_OPSLOPEBAR
         || lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_SLOPE         // For when we come in via TKT_OP3NAMED
         || lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_SLOPEBAR);    // ...

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (&lpYYFcnStrOpr->tkToken APPEND_NAME_ARG);

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return PrimOpMonSlope_EM_YY (lpYYFcnStrOpr, // Ptr to operator function strand
                                     lptkRhtArg);   // Ptr to right arg
    else
        return PrimOpDydSlope_EM_YY (lptkLftArg,    // Ptr to left arg token
                                     lpYYFcnStrOpr, // Ptr to operator function strand
                                     lptkRhtArg);   // Ptr to right arg token
} // End PrimOpSlope_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoOpSlope_EM_YY
//
//  Generate a prototype for the derived functions from
//    monadic operator Slope ("scan" and ERROR)
//***************************************************************************

LPPL_YYSTYPE PrimProtoOpSlope_EM_YY
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
        return PrimOpMonSlopeCommon_EM_YY (lpYYFcnStrOpr,   // Ptr to operator function strand
                                           lptkRhtArg,      // Ptr to right arg token
                                           TRUE);           // TRUE iff prototyping
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************
        return PrimOpDydSlopeCommon_EM_YY (lptkLftArg,      // Ptr to left arg token
                                           lpYYFcnStrOpr,   // Ptr to operator function strand
                                           lptkRhtArg,      // Ptr to right arg token
                                           TRUE);           // TRUE iff prototyping
} // End PrimProtoOpSlope_EM_YY


//***************************************************************************
//  $PrimOpMonSlope_EM_YY
//
//  Primitive operator for monadic derived function from Slope ("scan")
//***************************************************************************

LPPL_YYSTYPE PrimOpMonSlope_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpMonSlopeCommon_EM_YY (lpYYFcnStrOpr,       // Ptr to operator function strand
                                       lptkRhtArg,          // Ptr to right arg token
                                       FALSE);              // TRUE iff prototyping
} // End PrimOpMonSlope_EM_YY


//***************************************************************************
//  $PrimOpMonSlopeCommon_EM_YY
//
//  Primitive operator for monadic derived function from Slope ("scan")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpMonSlopeCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpMonSlopeCommon_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,            // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,               // Ptr to right arg token
     UBOOL        bPrototyping)             // TRUE if prototyping

{
    APLSTYPE          aplTypeRht,           // Right arg storage type
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
                      uAx,                  // Loop counter for uDimAxRht
                      uRht,                 // Right arg loop counter
                      ByteRes;              // # bytes in the result
    APLINT            apaOffRht,            // Right arg APA offset
                      apaMulRht;            // ...           multiplier
    HGLOBAL           hGlbRht = NULL,       // Right arg global memory handle
                      hGlbRes = NULL;       // Result    ...
    LPVOID            lpMemRht = NULL,      // Ptr to right arg global memory
                      lpMemRes = NULL;      // Ptr to result    ...
    LPAPLDIM          lpMemDimRht;          // Ptr to right arg dimensions
    UBOOL             bRet = TRUE,          // TRUE iff result is valid
                      bFastBool = FALSE;    // TRUE iff this is a Fast Boolean operation
    LPPRIMSPEC        lpPrimSpec;           // Ptr to local PRIMSPEC
    LPPRIMFLAGS       lpPrimFlagsLft;       // Ptr to left operand PrimFlags entry
    TOKEN             tkLftArg = {0},       // Left arg token
                      tkRhtArg = {0};       // Right ...
    LPTOKEN           lptkAxis;             // Ptr to axis token (may be NULL)
    LPPL_YYSTYPE      lpYYRes = NULL,       // Ptr to the result
                      lpYYFcnStrLft;        // Ptr to left operand function strand
    PL_YYSTYPE        YYFcnStrLft;          // Temporary left operand function strand for alternating scans
    LPPRIMFNS         lpPrimProtoLft;       // Ptr to left operand prototype function
    LPSYMENTRY        lpSymTmp;             // Ptr to temporary LPSYMENTRY
    LPVARARRAY_HEADER lpMemHdrRes;          // Ptr to result header
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    APLLONGEST        aplLongestRht;        // Right arg immediate type
    APLCHAR           alterChar;            // Original function in case alternating

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Check for axis operator
    lptkAxis = CheckAxisOper (lpYYFcnStrOpr);

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStrOpr[1 + (lptkAxis NE NULL)];

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken)
     || IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_OPERAND_SYNTAX_EXIT;

    // Get a ptr to the prototype function for the first symbol (a function or operator)
    if (bPrototyping)
    {
        // Get the appropriate prototype function ptr
        lpPrimProtoLft = GetPrototypeFcnPtr (&lpYYFcnStrLft->tkToken);
        if (!lpPrimProtoLft)
            goto LEFT_OPERAND_NONCE_EXIT;
    } else
        lpPrimProtoLft = NULL;

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Reverse allows a single integer axis value only
        if (!CheckAxis_EM (lptkAxis,        // The axis token
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
        //   if Slope, use last dimension
        if (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ INDEX_OPSLOPE
         || lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_SLOPE)
            aplAxis = aplRankRht - 1;
        else
        {
            Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ INDEX_OPSLOPEBAR
                 || lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_SLOPEBAR);

            // Otherwise, it's SlopeBar on the first dimension
            aplAxis = 0;
        } // End IF/ELSE
    } // End IF/ELSE

    // Get right arg's global ptr
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // If the right arg is a scalar, return it
    if (IsScalar (aplRankRht))
    {
        lpYYRes =
          PrimOpMonSlashScalar_EM_YY (lptkRhtArg,       // Ptr to right arg token
                                      aplTypeRht,       // Right arg storage type
                                      aplLongestRht,    // Right arg immediate value
                                      hGlbRht,          // Right arg global memory handle
                                      lpYYFcnStrOpr,    // Ptr to operator function strand
                                      lpYYFcnStrLft,    // Ptr to left operand
                                      bPrototyping);    // TRUE iff prototyping
        goto NORMAL_EXIT;
    } // End IF

    //***************************************************************
    // From here on, the right arg is a vector or higher rank array
    //***************************************************************

    // If the function is right tack, return the right arg unchanged
    if (lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_RIGHTTACK)
    {
        // Increment the right arg reference count so we can copy it
        DbgIncrRefCntDir_PTB (MakePtrTypeGlb (hGlbRht));

        // Copy the right arg
        hGlbRes = hGlbRht;

        goto YYALLOC_EXIT;
    } // End IF

    // The rank of the result is the same as that of the right arg
    aplRankRes = aplRankRht;

    // Skip over the header to the dimensions
    lpMemDimRht = VarArrayBaseToDim (lpMemRht);

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    // If the right arg is an APA, ...
    if (IsSimpleAPA (aplTypeRht))
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        // Get the APA parameters
        apaOffRht = lpAPA->Off;
        apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

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
    aplNELMRes = _imul64 (uDimLo, uDimHi, &bRet);
    if (bRet || IsZeroDim (uDimAxRht))
        aplNELMRes = _imul64 (aplNELMRes, uDimAxRht, &bRet);
    if (!bRet)
        goto WSFULL_EXIT;

    // If the right arg is empty, return it
    if (IsEmpty (aplNELMRht))
    {
        hGlbRes = CopySymGlbDirAsGlb (MakePtrTypeGlb (hGlbRht));

        goto YYALLOC_EXIT;
    } // End IF

    // Get a ptr to the Primitive Function Flags
    lpPrimFlagsLft = GetPrimFlagsPtr (&lpYYFcnStrLft->tkToken);

    // If the product of the dimensions above
    //   the axis dimension is one, and
    //   this is a primitive function, and
    //   the right arg is Boolean or APA Boolean, and
    //   the axis dimension is > 1, and
    //   we're not doing prototypes, then
    //   check for the possibility of doing a
    //   Fast Boolean Scan
    if (IsUnitDim (uDimHi)
     && lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
     && (IsSimpleBool (aplTypeRht)
      || (IsSimpleAPA (aplTypeRht)
       && IsBooleanValue (apaOffRht)
       && apaMulRht EQ 0 ))
     && IsMultiDim (uDimAxRht)
     && lpPrimProtoLft EQ NULL
     && lpPrimFlagsLft->FastBool)
    {
        // Mark as a Fast Boolean operation
        bFastBool = TRUE;

        // The storage type is Boolean
        aplTypeRes = ARRAY_BOOL;
    } else
    // If the operand is a primitive scalar dyadic function,
    //   and the right arg is simple NH or global numeric,
    //   calculate the storage type of the result,
    //   otherwise, assume it's ARRAY_NESTED
    if (lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
     && lpPrimFlagsLft->DydScalar
     && IsSimpleNHGlbNum (aplTypeRht))
    {
        // If the function is equal or not-equal, and the right
        //   arg is not Boolean, make the result storage type
        //   nested and let TypeDemote figure it out in the end.
        // This avoids complications with =\3 1{rho}'abc' which
        //   is typed as Boolean but blows up to HETERO.
        if ((lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_EQUAL
          || lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_NOTEQUAL)
         && !IsSimpleBool (aplTypeRht))
            aplTypeRes = ARRAY_NESTED;
        else
        {
            // Get the corresponding lpPrimSpec
            lpPrimSpec = PrimSpecTab[SymTrans (&lpYYFcnStrLft->tkToken)];

            // Calculate the storage type of the result
            aplTypeRes =
              (*lpPrimSpec->StorageTypeDyd) (1,
                                            &aplTypeRht,
                                            &lpYYFcnStrLft->tkToken,
                                             1,
                                            &aplTypeRht);
            if (IsErrorType (aplTypeRes))
                goto DOMAIN_EXIT;
        } // End IF/ELSE
    } else
    // If the function is left tack, ...
    if (lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_LEFTTACK)
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
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = lpMemHdrRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
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
    lpMemRes = VarArrayBaseToDim (lpMemRes);

    // Copy the dimensions from the right arg to the result
    for (uDim = 0; uDim < aplRankRht; uDim++)
        *((LPAPLDIM) lpMemRes)++ = lpMemDimRht[uDim];

    // lpMemRes now points to its data

    // If the function is left tack, ...
    if (lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_LEFTTACK)
    {
        HGLOBAL lpSymGlbRht;

        // Loop through the right arg
        for (uLo = 0; uLo < uDimLo; uLo++)
        for (uHi = 0; uHi < uDimHi; uHi++)
        {
            // Calculate the starting index in the right arg of this vector
            uDimRht = uLo * uDimHi * uDimAxRht + uHi;

            // Get the first value in the vector from the right arg token
            GetNextValueToken (lptkRhtArg,      // Ptr to the token
                               uDimRht,         // Index to use
                               NULL,            // Ptr to the integer (or Boolean) (may be NULL)
                               NULL,            // ...        float (may be NULL)
                               NULL,            // ...        char (may be NULL)
                              &aplLongestRht,   // ...        longest (may be NULL)
                              &lpSymGlbRht,     // ...        LPSYMENTRY or HGLOBAL (may be NULL)
                               NULL,            // ...        immediate type (see IMM_TYPES) (may be NULL)
                               NULL);           // ...        array type:  ARRAY_TYPES (may be NULL)
            // Split cases based upon the result type
            switch (aplTypeRes)
            {
                case ARRAY_BOOL:
                    // If the leading bit is a 1, ...
                    if (aplLongestRht & BIT0)
                    // Loop through the elements in the vector along the axis
                    for (uAx = 0; uAx < uDimAxRht; uAx++)
                    {
                        // Calculate the index of the next element in this vector
                        uRht = uDimRht + uAx * uDimHi;

                        // Save in the result
                        ((LPAPLBOOL) lpMemRes)[uRht >> LOG2NBIB] |= BIT0 << (MASKLOG2NBIB & (UINT) uRht);
                    } // End IF/FOR

                    break;

                case ARRAY_INT:
                    // Loop through the elements in the vector along the axis
                    for (uAx = 0; uAx < uDimAxRht; uAx++)
                    {
                        // Calculate the index of the next element in this vector
                        uRht = uDimRht + uAx * uDimHi;

                        // Save in the result
                        ((LPAPLINT) lpMemRes)[uRht] = (APLINT) aplLongestRht;
                    } // End FOR

                    break;

                case ARRAY_FLOAT:
                    // Loop through the elements in the vector along the axis
                    for (uAx = 0; uAx < uDimAxRht; uAx++)
                    {
                        // Calculate the index of the next element in this vector
                        uRht = uDimRht + uAx * uDimHi;

                        // Save in the result
                        ((LPAPLFLOAT) lpMemRes)[uRht] = *(LPAPLFLOAT) &aplLongestRht;
                    } // End FOR

                    break;

                case ARRAY_CHAR:
                    // Loop through the elements in the vector along the axis
                    for (uAx = 0; uAx < uDimAxRht; uAx++)
                    {
                        // Calculate the index of the next element in this vector
                        uRht = uDimRht + uAx * uDimHi;

                        // Save in the result
                        ((LPAPLCHAR) lpMemRes)[uRht] = (APLCHAR) aplLongestRht;
                    } // End FOR

                    break;

                case ARRAY_HETERO:
                case ARRAY_NESTED:
                    // Loop through the elements in the vector along the axis
                    for (uAx = 0; uAx < uDimAxRht; uAx++)
                    {
                        // Calculate the index of the next element in this vector
                        uRht = uDimRht + uAx * uDimHi;

                        // Save in the result
                        ((LPAPLNESTED) lpMemRes)[uRht] = CopySymGlbDir_PTB (lpSymGlbRht);
                    } // End FOR

                    break;

                case ARRAY_RAT:
                    // Loop through the elements in the vector along the axis
                    for (uAx = 0; uAx < uDimAxRht; uAx++)
                    {
                        // Calculate the index of the next element in this vector
                        uRht = uDimRht + uAx * uDimHi;

                        // Save in the result
                        mpq_init_set (&((LPAPLRAT) lpMemRes)[uRht], (LPAPLRAT) lpSymGlbRht);
                    } // End FOR

                    break;

                case ARRAY_VFP:
                    // Loop through the elements in the vector along the axis
                    for (uAx = 0; uAx < uDimAxRht; uAx++)
                    {
                        // Calculate the index of the next element in this vector
                        uRht = uDimRht + uAx * uDimHi;

                        // Save in the result
                        mpfr_init_set (&((LPAPLVFP) lpMemRes)[uRht], (LPAPLVFP) lpSymGlbRht, MPFR_RNDN);
                    } // End FOR

                    break;

                defstop
                    break;
            } // End SWITCH
        } // End FOR/FOR
    } else
    // If this is a fast Boolean operation, ...
    if (bFastBool)
    {
        LPFASTBOOLFCN lpFastBool;           // Ptr to Fast Boolean reduction routine

        // Get a ptr to the appropriate Fast Boolean routine
        lpFastBool = FastBoolFns[lpPrimFlagsLft->Index].lpScan;

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
RESTART_EXCEPTION:
        // Copy the current left operand function strand
        //   to substitute into when we're doing alternating scans
        YYFcnStrLft = *lpYYFcnStrLft;

        // In case we're executing an alternating function,
        //   save the original function
        alterChar = YYFcnStrLft.tkToken.tkData.tkChar;

        // Loop through the right arg calling the
        //   function strand between data, storing in the
        //   result
        for (uLo = 0; uLo < uDimLo; uLo++)
        for (uHi = 0; uHi < uDimHi; uHi++)
        {
            APLINT  iDim;           // Loop counter for aplIntegerLftAbs

            // Calculate the starting index in the right arg of this vector
            uDimRht = uLo * uDimHi * uDimAxRht + uHi;

            // There are uDimAxRht elements in the result to be generated now
            //   each being successive reductions of leading elements in the
            //   vector under consideration.

            // If this function is associative or alternating, speed it up
            if ((lpPrimFlagsLft->AssocBool && IsSimpleBool (aplTypeRht))
             || (lpPrimFlagsLft->AssocNumb && IsSimpleGlbNum (aplTypeRht))
             ||  lpPrimFlagsLft->Alter)
            {
                // Restore the original function in case it's alternating
                YYFcnStrLft.tkToken.tkData.tkChar = alterChar;

                // Calculate the first index in this vector
                uRht = uDimRht + 0 * uDimHi;

                // Get the first element as the left arg
                GetNextValueMemIntoToken (uRht,         // Index to use
                                          lpMemRht,     // Ptr to global memory object to index
                                          aplTypeRht,   // Storage type of the arg
                                          apaOffRht,    // APA offset (if needed)
                                          apaMulRht,    // APA multiplier (if needed)
                                         &tkLftArg);    // Ptr to token in which to place the value
                // In case we blew up, check to see if we must blow up tkLftArg
                //   (whose storage type is in aplTypeRht)
                if (aplTypeRes NE aplTypeRht)
                    (*aTypeTknPromote[aplTypeRht][aplTypeRes]) (&tkLftArg);

                // Split cases based upon the token type of the left arg
                switch (tkLftArg.tkFlags.TknType)
                {
                    case TKT_VARIMMED:
                        // Split cases based upon the result storage type
                        switch (aplTypeRes)
                        {
                            case ARRAY_BOOL:
                                // Check for blow up (to INT or FLOAT -- can this ever happen??)
                                if (IsImmBool (tkLftArg.tkFlags.ImmType))
                                    // Save in the result as a BOOL
                                    ((LPAPLBOOL) lpMemRes)[uRht >> LOG2NBIB] |=
                                      (BIT0 & tkLftArg.tkData.tkBoolean) << (MASKLOG2NBIB & (UINT) uRht);
                                else
                                {
                                    // It's now an INT result
                                    aplTypeRes = ARRAY_INT;

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

                                    goto RESTART_ALLOC;
                                } // End IF/ELSE

                                break;

                            case ARRAY_INT:
                                // Check for blow up (to FLOAT)
                                if (IsImmInt (tkLftArg.tkFlags.ImmType))
                                    // Save in the result as an INT
                                    ((LPAPLINT) lpMemRes)[uRht] = tkLftArg.tkData.tkInteger;
                                else
                                {
                                    // It's now a FLOAT result
                                    aplTypeRes = ARRAY_FLOAT;

                                    // Tell the header about it
                                    lpMemHdrRes->ArrType = aplTypeRes;

                                    // Note that we don't go to RESTART_ALLOC because we know
                                    //   that integer and float are the same size (8 bytes)
                                    goto RESTART_EXCEPTION;
                                } // End IF/ELSE

                                break;

                            case ARRAY_FLOAT:
                                // Save in the result as a FLOAT
                                ((LPAPLFLOAT) lpMemRes)[uRht] = tkLftArg.tkData.tkFloat;

                                break;

                            case ARRAY_NESTED:
                                // Save in the result as an LPSYMENTRY
                                ((LPAPLNESTED) lpMemRes)[uRht] =
                                lpSymTmp =
                                  MakeSymEntry_EM (tkLftArg.tkFlags.ImmType,    // Immediate type
                                                  &tkLftArg.tkData.tkLongest,   // Ptr to immediate value
                                                  &lpYYFcnStrOpr->tkToken);     // Ptr to function token
                                if (!lpSymTmp)
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
                                lpMemRat = MyGlobalLock (tkLftArg.tkData.tkGlbData);

                                // Skip over the header and dimensions to the data
                                lpMemRat = VarArrayBaseToData (lpMemRat, 0);

                                // Copy to the result
                                mpq_init_set (&((LPAPLRAT) lpMemRes)[uRht], lpMemRat);

                                // We no longer need this ptr
                                MyGlobalUnlock (tkLftArg.tkData.tkGlbData); lpMemRat = NULL;

                                break;

                            case ARRAY_VFP:
                                // Lock the memory to get a ptr to it
                                lpMemVfp = MyGlobalLock (tkLftArg.tkData.tkGlbData);

                                // Skip over the header and dimensions to the data
                                lpMemVfp = VarArrayBaseToData (lpMemVfp, 0);

                                // Copy to the result
                                mpfr_init_copy (&((LPAPLVFP) lpMemRes)[uRht], lpMemVfp);

                                // We no longer need this ptr
                                MyGlobalUnlock (tkLftArg.tkData.tkGlbData); lpMemVfp = NULL;

                                break;

                            case ARRAY_NESTED:
                                // Save in the result as an HGLOBAL
                                ((LPAPLNESTED) lpMemRes)[uRht] = CopySymGlbDir_PTB (tkLftArg.tkData.tkGlbData);

                                break;

                            defstop
                                break;
                        } // End SWITCH

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Loop forwards through the elements along the specified axis
                for (uAx = 1; uAx < uDimAxRht; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Calculate the index of the next element in this vector
                    uRht = uDimRht + uAx * uDimHi;

                    // Get the next element as the right arg
                    GetNextValueMemIntoToken (uRht,         // Index to use
                                              lpMemRht,     // Ptr to global memory object to index
                                              aplTypeRht,   // Storage type of the arg
                                              apaOffRht,    // APA offset (if needed)
                                              apaMulRht,    // APA multiplier (if needed)
                                             &tkRhtArg);    // Ptr to token in which to place the value
                    // In case we blew up, check to see if we must blow up tkRhtArg
                    //   (whose storage type is in aplTypeRht)
                    if (aplTypeRes NE aplTypeRht)
                        (*aTypeTknPromote[aplTypeRht][aplTypeRes]) (&tkRhtArg);

                    // Execute the left operand between the left & right args
                    if (lpPrimProtoLft)
                        // Note that we cast the function strand to LPTOKEN
                        //   to bridge the two types of calls -- one to a primitive
                        //   function which takes a function token, and one to a
                        //   primitive operator which takes a function strand
                        lpYYRes = (*lpPrimProtoLft) (&tkLftArg,         // Ptr to left arg token
                                           (LPTOKEN) &YYFcnStrLft,      // Ptr to left operand function strand
                                                     &tkRhtArg,         // Ptr to right arg token
                                                      lptkAxis);        // Ptr to axis token (may be NULL)
                    else
                        lpYYRes = ExecFuncStr_EM_YY (&tkLftArg,         // Ptr to left arg token
                                                     &YYFcnStrLft,      // Ptr to function strand
                                                     &tkRhtArg,         // Ptr to right arg token
                                                      lptkAxis);        // Ptr to axis token (may be NULL)
                    // Free the left & right arg tokens
                    FreeResultTkn (&tkRhtArg);
                    FreeResultTkn (&tkLftArg);

                    // If it succeeded, ...
                    if (lpYYRes)
                    {
                        // Copy the result to the left arg token
                        tkLftArg = lpYYRes->tkToken;

                        // Free the YYRes (but not the storage)
                        YYFree (lpYYRes); lpYYRes = NULL;
                    } else
                        goto ERROR_EXIT;

                    // Split cases based upon the token type of the left arg
                    switch (tkLftArg.tkFlags.TknType)
                    {
                        case TKT_VARIMMED:
                            // Split cases based upon the result storage type
                            switch (aplTypeRes)
                            {
                                case ARRAY_BOOL:
                                    // Check for blow up (to INT or FLOAT -- can this ever happen??)
                                    if (IsImmBool (tkLftArg.tkFlags.ImmType))
                                        // Save in the result as a BOOL
                                        ((LPAPLBOOL) lpMemRes)[uRht >> LOG2NBIB] |=
                                          (BIT0 & tkLftArg.tkData.tkBoolean) << (MASKLOG2NBIB & (UINT) uRht);
                                    else
                                    {
                                        // It's now an INT result
                                        aplTypeRes = ARRAY_INT;

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

                                        goto RESTART_ALLOC;
                                    } // End IF/ELSE

                                    break;

                                case ARRAY_INT:
                                    // Check for blow up (to FLOAT)
                                    if (IsImmInt (tkLftArg.tkFlags.ImmType))
                                        // Save in the result as an INT
                                        ((LPAPLINT) lpMemRes)[uRht] = tkLftArg.tkData.tkInteger;
                                    else
                                    {
                                        // It's now a FLOAT result
                                        aplTypeRes = ARRAY_FLOAT;

                                        // Tell the header about it
                                        lpMemHdrRes->ArrType = aplTypeRes;

                                        // Note that we don't go to RESTART_ALLOC because we kn ow
                                        //   that integer and float are the same size (8 bytes)
                                        goto RESTART_EXCEPTION;
                                    } // End IF/ELSE

                                    break;

                                case ARRAY_FLOAT:
                                    // Save in the result as a FLOAT
                                    ((LPAPLFLOAT) lpMemRes)[uRht] = tkLftArg.tkData.tkFloat;

                                    break;

                                case ARRAY_NESTED:
                                    // Save in the result as an LPSYMENTRY
                                    ((LPAPLNESTED) lpMemRes)[uRht] =
                                    lpSymTmp =
                                      MakeSymEntry_EM (tkLftArg.tkFlags.ImmType,    // Immediate type
                                                      &tkLftArg.tkData.tkLongest,   // Ptr to immediate value
                                                      &lpYYFcnStrOpr->tkToken);     // Ptr to function token
                                    if (!lpSymTmp)
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
                                    lpMemRat = MyGlobalLock (tkLftArg.tkData.tkGlbData);

                                    // Skip over the header and dimensions to the data
                                    lpMemRat = VarArrayBaseToData (lpMemRat, 0);

                                    // Copy to the result
                                    mpq_init_set (&((LPAPLRAT) lpMemRes)[uRht], lpMemRat);

                                    // We no longer need this ptr
                                    MyGlobalUnlock (tkLftArg.tkData.tkGlbData); lpMemRat = NULL;

                                    break;

                                case ARRAY_VFP:
                                    // Lock the memory to get a ptr to it
                                    lpMemVfp = MyGlobalLock (tkLftArg.tkData.tkGlbData);

                                    // Skip over the header and dimensions to the data
                                    lpMemVfp = VarArrayBaseToData (lpMemVfp, 0);

                                    // Copy to the result
                                    mpfr_init_copy (&((LPAPLVFP) lpMemRes)[uRht], lpMemVfp);

                                    // We no longer need this ptr
                                    MyGlobalUnlock (tkLftArg.tkData.tkGlbData); lpMemVfp = NULL;

                                    break;

                                case ARRAY_NESTED:
                                    // Save in the result as an HGLOBAL
                                    ((LPAPLNESTED) lpMemRes)[uRht] = CopySymGlbDir_PTB (tkLftArg.tkData.tkGlbData);

                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    // If the left operand is alternating, ...
                    if (lpPrimFlagsLft->Alter)
                    {
                        // Split cases based upon the current left operand
                        switch (YYFcnStrLft.tkToken.tkData.tkChar)
                        {
                            case UTF16_BAR:
                            case UTF16_BAR2:
                                YYFcnStrLft.tkToken.tkData.tkChar = UTF16_PLUS;

                                break;

                            case UTF16_PLUS:
                                YYFcnStrLft.tkToken.tkData.tkChar = UTF16_BAR;

                                break;

                            case UTF16_COLONBAR:
                                YYFcnStrLft.tkToken.tkData.tkChar = UTF16_TIMES;

                                break;

                            case UTF16_TIMES:
                                YYFcnStrLft.tkToken.tkData.tkChar = UTF16_COLONBAR;

                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End IF
                } // End FOR

                // Free the left arg token
                FreeResultTkn (&tkLftArg);
            } else
            // Loop through the ending indices of the reduction, that is,
            //   each reduction starts at uDimRht and goes through the
            //   successive elements up to and including uDimRht + uAx * uDimHi
            //   for uAx running through 0 to uDimAxRht - 1.
            // At this point, the left arg function is non-associative.
            for (uAx = 0; uAx < uDimAxRht; uAx++)
            {
                // Reduce the uDimAxRht values starting at
                //   uRht = uDimRht +   0 * uDimHi;
                // through
                //   uRht = uDimRht + uAx * uDimHi;

                // Calculate the index of rightmost element in this vector
                uRht = uDimRht + uAx * uDimHi;

                // Get the righmost element as the right arg
                GetNextValueMemIntoToken (uRht,             // Index to use
                                          lpMemRht,         // Ptr to global memory object to index
                                          aplTypeRht,       // Storage type of the arg
                                          apaOffRht,        // APA offset (if needed)
                                          apaMulRht,        // APA multiplier (if needed)
                                         &tkRhtArg);        // Ptr to token in which to place the value
                // In case we blew up, check to see if we must blow up tkRhtArg
                if (aplTypeRes NE aplTypeRht)
                    (*aTypeTknPromote[aplTypeRht][aplTypeRes]) (&tkRhtArg);

                // Loop backwards through the elements along the specified axis
                for (iDim = uAx - 1; iDim >= 0; iDim--)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Calculate the index of the previous element in this vector
                    uRht = uDimRht + iDim * uDimHi;

                    // Get the previous element as the left arg
                    GetNextValueMemIntoToken (uRht,         // Index to use
                                              lpMemRht,     // Ptr to global memory object to index
                                              aplTypeRht,   // Storage type of the arg
                                              apaOffRht,    // APA offset (if needed)
                                              apaMulRht,    // APA multiplier (if needed)
                                             &tkLftArg);    // Ptr to token in which to place the value
                    // Execute the left operand between the left & right args
                    if (lpPrimProtoLft)
                        // Note that we cast the function strand to LPTOKEN
                        //   to bridge the two types of calls -- one to a primitive
                        //   function which takes a function token, and one to a
                        //   primitive operator which takes a function strand
                        lpYYRes = (*lpPrimProtoLft) (&tkLftArg,         // Ptr to left arg token
                                            (LPTOKEN) lpYYFcnStrLft,    // Ptr to left operand function strand
                                                     &tkRhtArg,         // Ptr to right arg token
                                                      lptkAxis);        // Ptr to axis token (may be NULL)
                    else
                        lpYYRes = ExecFuncStr_EM_YY (&tkLftArg,         // Ptr to left arg token
                                                      lpYYFcnStrLft,    // Ptr to function strand
                                                     &tkRhtArg,         // Ptr to right arg token
                                                      lptkAxis);        // Ptr to axis token (may be NULL)
                    // Free the left & right arg tokens
                    FreeResultTkn (&tkRhtArg);
                    FreeResultTkn (&tkLftArg);

                    // If it succeeded, ...
                    if (lpYYRes)
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

                // Get the index into the result
                uRht = uDimRht + uAx * uDimHi;

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
                                    ((LPAPLBOOL) lpMemRes)[uRht >> LOG2NBIB] |=
                                      (BIT0 & tkRhtArg.tkData.tkBoolean) << (MASKLOG2NBIB & (UINT) uRht);
                                else
                                {
                                    // This can occur with {nand}\2 3

                                    // It's now an INT result
                                    aplTypeRes = ARRAY_INT;

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

                                    goto RESTART_ALLOC;
                                } // End IF/ELSE

                                break;

                            case ARRAY_INT:
                                // Check for blow up (to FLOAT)
                                if (IsImmInt (tkRhtArg.tkFlags.ImmType))
                                    // Save in the result as an INT
                                    ((LPAPLINT) lpMemRes)[uRht] = tkRhtArg.tkData.tkInteger;
                                else
                                {
                                    // It's now a FLOAT result
                                    aplTypeRes = ARRAY_FLOAT;

                                    // Tell the header about it
                                    lpMemHdrRes->ArrType = aplTypeRes;

                                    // Note that we don't go to RESTART_ALLOC because we know
                                    //   that integer and float are the same size (8 bytes)
                                    goto RESTART_EXCEPTION;
                                } // End IF/ELSE

                                break;

                            case ARRAY_FLOAT:
                                // Save in the result as a FLOAT
                                ((LPAPLFLOAT) lpMemRes)[uRht] = tkRhtArg.tkData.tkFloat;

                                break;

                            case ARRAY_NESTED:
                                // Save in the result as an LPSYMENTRY
                                ((LPAPLNESTED) lpMemRes)[uRht] =
                                lpSymTmp =
                                  MakeSymEntry_EM (tkRhtArg.tkFlags.ImmType,    // Immediate type
                                                  &tkRhtArg.tkData.tkLongest,   // Ptr to immediate value
                                                  &lpYYFcnStrOpr->tkToken);     // Ptr to function token
                                if (!lpSymTmp)
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
                                lpMemRat = MyGlobalLock (tkRhtArg.tkData.tkGlbData);

                                // Skip over the header and dimensions to the data
                                lpMemRat = VarArrayBaseToData (lpMemRat, 0);

                                // Copy to the result
                                mpq_init_set (&((LPAPLRAT) lpMemRes)[uRht], lpMemRat);

                                // We no longer need this ptr
                                MyGlobalUnlock (tkRhtArg.tkData.tkGlbData); lpMemRat = NULL;

                                break;

                            case ARRAY_VFP:
                                // Lock the memory to get a ptr to it
                                lpMemVfp = MyGlobalLock (tkRhtArg.tkData.tkGlbData);

                                // Skip over the header and dimensions to the data
                                lpMemVfp = VarArrayBaseToData (lpMemVfp, 0);

                                // Copy to the result
                                mpfr_init_copy (&((LPAPLVFP) lpMemRes)[uRht], lpMemVfp);

                                // We no longer need this ptr
                                MyGlobalUnlock (tkRhtArg.tkData.tkGlbData); lpMemVfp = NULL;

                                break;

                            case ARRAY_NESTED:
                                // Save in the result as an HGLOBAL
                                ((LPAPLNESTED) lpMemRes)[uRht] = CopySymGlbDir_PTB (tkRhtArg.tkData.tkGlbData);

                                break;

                            defstop
                                break;
                        } // End SWITCH

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Free the right arg token
                FreeResultTkn (&tkRhtArg);
            } // End FOR
        } // End FOR/FOR
    } // End IF/ELSE
YYALLOC_EXIT:
    // Unlock the result global memory in case TypeDemote actually demotes
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
    lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken);

    goto NORMAL_EXIT;

LEFT_OPERAND_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    goto ERROR_EXIT;

LEFT_OPERAND_NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    goto ERROR_EXIT;

VALUE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
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
} // End PrimOpMonSlopeCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpDydSlope_EM_YY
//
//  Primitive operator for dyadic derived function from Slope (ERROR)
//***************************************************************************

LPPL_YYSTYPE PrimOpDydSlope_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpDydSlopeCommon_EM_YY (lptkLftArg,      // Ptr to left arg token (may be NULL if monadic)
                                       lpYYFcnStrOpr,   // Ptr to operator function strand
                                       lptkRhtArg,      // Ptr to right arg token
                                       FALSE);          // TRUE iff prototyping
} // End PrimOpDydSlope_EM_YY


//***************************************************************************
//  $PrimOpDydSlopeCommon_EM_YY
//
//  Primitive operator for dyadic derived function from Slope (ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydSlopeCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpDydSlopeCommon_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     UBOOL        bPrototyping)         // TRUE if prototyping

{
    return PrimFnValenceError_EM (&lpYYFcnStrOpr->tkToken APPEND_NAME_ARG);
} // End PrimOpDydSlopeCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: po_slope.c
//***************************************************************************
