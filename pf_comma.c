//***************************************************************************
//  NARS2000 -- Primitive Function -- Comma
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
//  $PrimFnComma_EM_YY
//
//  Primitive function for monadic and dyadic Comma ("ravel/table" and "catenate/laminate")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnComma_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnComma_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_COMMA
         || lptkFunc->tkData.tkChar EQ UTF16_COMMABAR);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc APPEND_NAME_ARG);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonComma_EM_YY             (lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydComma_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnComma_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnComma_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic Comma
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnComma_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnComma_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnComma_EM_YY,  // Ptr to primitive function routine
                                    lptkLftArg,         // Ptr to left arg token
                                    lptkFunc,           // Ptr to function token
                                    lptkRhtArg,         // Ptr to right arg token
                                    lptkAxis);          // Ptr to axis token (may be NULL)
} // End PrimProtoFnComma_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimIdentFnComma_EM_YY
//
//  Generate an identity element for the primitive function dyadic Comma
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimIdentFnComma_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimIdentFnComma_EM_YY
    (LPTOKEN lptkRhtOrig,           // Ptr to original right arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    TOKEN        tkLft = tkZero,    // Token for temporary left arg
                 tkFcn = {0},       // Ptr to function token
                 tkAxis = {0};      // Ptr to axis token
    LPPL_YYSTYPE lpYYIdent = NULL;  // Ptr to identity element result
    APLRANK      aplRankRht;        // Right arg rank
    APLBOOL      bQuadIO;           // []IO

    // The right arg is the prototype item from
    //   the original empty arg.

    Assert (lptkRhtOrig NE NULL);
    Assert (lptkFunc    NE NULL);
    Assert (lptkRhtArg  NE NULL);

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, NULL, NULL, &aplRankRht, NULL);

    // Check for axis present
    if (lptkAxis)
    {
        // Catentate allows integer axis values only
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           aplRankRht,      // All values less than this
                           TRUE,            // TRUE iff scalar or one-element vector only
                           FALSE,           // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // Return TRUE iff fractional values present
                           NULL,            // Return last axis value
                           NULL,            // Return # elements in axis vector
                           NULL))           // Return HGLOBAL with APLINT axis values
            goto ERROR_EXIT;
    } // End IF

    // Disallow scalars as they do not have a catenate identity function
    if (IsScalar (aplRankRht))
        goto DOMAIN_EXIT;

    // The (left/right) identity function for dyadic comma
    //   (L,[X] R) ("catenate") is
    //   0/[X] R.

    // Setup the left arg token
////tkLft.tkFlags.TknType   = TKT_VARIMMED;     // Already set from tkZero
////tkLft.tkFlags.ImmType   = IMMTYPE_BOOL;     // Already set from tkZero
////tkLft.tkFlags.NoDisplay = FALSE;            // Already set from tkZero
////tkLft.tkData.tkLongest  = 0;                // Already set ftom tkZero
    tkLft.tkCharIndex       = lptkFunc->tkCharIndex;

    // Setup the function token
    tkFcn.tkFlags.TknType   = TKT_FCNIMMED;
    tkFcn.tkFlags.ImmType   = IMMTYPE_PRIMFCN;
////tkFcn.tkFlags.NoDisplay = FALSE;            // Already zero from = {0}
    tkFcn.tkData.tkChar     = (lptkFunc->tkData.tkChar EQ UTF16_COMMABAR) ? UTF16_SLASHBAR
                                                                          : UTF16_SLASH;
    tkFcn.tkCharIndex       = lptkFunc->tkCharIndex;

    // Compute 0/[aplAxis] R
    lpYYIdent =
      PrimFnDydSlash_EM_YY (&tkLft,             // Ptr to left arg token
                            &tkFcn,             // Ptr to function token
                             lptkRhtArg,        // Ptr to right arg token
                             lptkAxis);         // Ptr to left operand axis token (may be NULL)
    return lpYYIdent;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    return NULL;
} // End PrimIdentFnComma_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonComma_EM_YY
//
//  Primitive function for monadic Comma ("ravel/table")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonComma_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonComma_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE aplTypeRht;            // Right arg storage type
    APLRANK  aplRankRht;            // Right arg rank
    HGLOBAL  hGlbRht;               // Right arg global memory handle

    //***************************************************************
    // Comma-bar is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************

    if (lptkFunc->tkData.tkChar EQ UTF16_COMMABAR
     && lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, NULL, &aplRankRht, NULL);

    // Split cases based upon the right arg's token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
            {
                // Get the global memory handle
                hGlbRht = lptkRhtArg->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir_PTB (hGlbRht));
            } else
                hGlbRht = NULL;

            // If it's a scalar, ...
            if (IsScalar (aplRankRht))
                // Handle the scalar case
                return PrimFnMonCommaScalar_EM_YY
                       (aplTypeRht,                                     // Array storage type
                        lptkRhtArg->tkData.tkSym->stData.stLongest,     // Immediate value if simple scalar
                        hGlbRht,                                        // Right arg global memory handle (or NULL if simple scalar)
                        lptkAxis,                                       // Ptr to axis token (may be NULL)
                        lptkFunc);                                      // Ptr to function token
            break;          // Join common global code

        case TKT_VARIMMED:
            // Handle the scalar case
            return PrimFnMonCommaScalar_EM_YY
                   (aplTypeRht,                                     // Array storage type
                    lptkRhtArg->tkData.tkLongest,                   // Immediate value if simple scalar
                    NULL,                                           // Right arg global memory handle (or NULL if simple scalar)
                    lptkAxis,                                       // Ptr to axis token (may be NULL)
                    lptkFunc);                                      // Ptr to function token
        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (lptkRhtArg->tkData.tkGlbData));

            // Get the global memory handle
            hGlbRht = lptkRhtArg->tkData.tkGlbData;

            // If it's a scalar, ...
            if (IsScalar (aplRankRht))
                // Handle the scalar case
                return PrimFnMonCommaScalar_EM_YY
                       (aplTypeRht,                                 // Array storage type
                        0,                                          // Immediate value if simple scalar
                        hGlbRht,                                    // Right arg global memory handle (or NULL if simple scalar)
                        lptkAxis,                                   // Ptr to axis token (may be NULL)
                        lptkFunc);                                  // Ptr to function token
            break;          // Join common global code

        defstop
            return NULL;
    } // End SWITCH

    return PrimFnMonCommaGlb_EM_YY (hGlbRht,                    // HGLOBAL
                                    lptkAxis,                   // Ptr to axis token (may be NULL)
                                    lptkFunc);                  // Ptr to function token
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFnMonComma_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonCommaScalar_EM_YY
//
//  Monadic Comma ("ravel/table") on a scalar value.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonCommaScalar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonCommaScalar_EM_YY
    (ARRAY_TYPES   aplTypeRht,      // Right arg storage type (see ARRAY_TYPES)
     APLLONGEST    aplLongest,      // Right arg immediate value if simple scalar
     HGLOBAL       hGlbRht,         // Right arg global memory handle (or NULL if simple scalar)
     LPTOKEN       lptkAxis,        // Ptr to axis token (may be NULL)
     LPTOKEN       lptkFunc)        // Ptr to function token

{
    APLRANK      aplRankRes;        // Result rank
    HGLOBAL      hGlbRes;           // Result global memory handle
    LPVOID       lpMemRes,          // Ptr to result global memory
                 lpMemRht;          // Ptr to right arg global memory
    UBOOL        bFract = FALSE,    // TRUE iff axis has fractional values
                 bTableRes;         // TRUE iff function is UTF16_COMMABAR
    APLUINT      ByteRes;           // # bytes in the result
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    // Check for axis present
    while (lptkAxis NE NULL)
    {
        // Ravel with axis on a simple scalar requires
        //   that the axis be an empty vector
        if (CheckAxis_EM (lptkAxis,         // The axis token
                          0,                // All values less than this
                          FALSE,            // TRUE iff scalar or one-element vector only
                          FALSE,            // TRUE iff want sorted axes
                          TRUE,             // TRUE iff axes must be contiguous
                          FALSE,            // TRUE iff duplicate axes are allowed
                          NULL,             // Return TRUE iff fractional values present
                          NULL,             // Return last axis value
                          NULL,             // Return # elements in axis vector
                          NULL))            // Return HGLOBAL with APLINT axis values
            break;
        //   or a singleton fractional value
        if (CheckAxis_EM (lptkAxis,         // The axis token
                          1,                // All values less than this
                          TRUE,             // TRUE iff scalar or one-element vector only
                          FALSE,            // TRUE iff want sorted axes
                          TRUE,             // TRUE iff axes must be contiguous
                          FALSE,            // TRUE iff duplicate axes are allowed
                         &bFract,           // Return TRUE iff fractional values present
                          NULL,             // Return last axis value
                          NULL,             // Return # elements in axis vector
                          NULL)             // Return HGLOBAL with APLINT axis values
         && bFract)
            break;
        // Otherwise, it's an AXIS ERROR
        return NULL;
    } // End WHILE

    //***************************************************************
    // If the function is Comma-bar (table), the result is
    //   1 1 {rho} R
    //***************************************************************
    bTableRes = (lptkFunc->tkData.tkChar EQ UTF16_COMMABAR);
    aplRankRes = 1 + bTableRes;

    //***************************************************************
    // Calculate space needed for the result
    //***************************************************************
    ByteRes = CalcArraySize (aplTypeRht, 1, aplRankRes);

    //***************************************************************
    // Check for overflow
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
    lpHeader->ArrType    = aplTypeRht;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = 1;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Fill in the result's dimension
    *VarArrayBaseToDim (lpMemRes) = 1;
    if (bTableRes)
        *(1 + VarArrayBaseToDim (lpMemRes)) = 1;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemRes);

    // If there's a global memory handle, ...
    if (hGlbRht)
    {
        // Lock the memory to get a handle to it
        lpMemRht = MyGlobalLockVar (hGlbRht);

        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemRht);
    } // End IF

    // Copy the immediate value to the result
    // Split cases based upon the result storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            *((LPAPLBOOL)  lpMemRes) = (APLBOOL)  aplLongest;

            break;

        case ARRAY_INT:
            *((LPAPLINT)   lpMemRes) = (APLINT)   aplLongest;

            break;

        case ARRAY_FLOAT:
            *((LPAPLFLOAT) lpMemRes) = *(LPAPLFLOAT) &aplLongest;

            break;

        case ARRAY_CHAR:
            *((LPAPLCHAR)  lpMemRes) = (APLCHAR)  aplLongest;

            break;

        case ARRAY_NESTED:
            *((LPAPLNESTED) lpMemRes) = CopySymGlbInd_PTB (lpMemRht);

            break;

        case ARRAY_RAT:
            mpq_init_set  ((LPAPLRAT) lpMemRes, (LPAPLRAT) lpMemRht);

            break;

        case ARRAY_VFP:
            mpfr_init_copy ((LPAPLVFP) lpMemRes, (LPAPLVFP) lpMemRht);

            break;

        defstop
            break;
    } // End SWITCH

    // If there's a global memory handle, ...
    if (hGlbRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

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

    return lpYYRes;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFnMonCommaScalar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonCommaGlb_EM_YY
//
//  Monadic Comma ("ravel/table") on a global memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonCommaGlb_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonCommaGlb_EM_YY
    (HGLOBAL       hGlbRht,             // Handle to right arg
     LPTOKEN       lptkAxis,            // Ptr to axis token (may be NULL)
     LPTOKEN       lptkFunc)            // Ptr to function token

{
    HGLOBAL       hGlbRes = NULL,       // Result global memory handle
                  hGlbAxis = NULL,      // Axis ...
                  hGlbOdo = NULL,       // Odometer ...
                  hGlbWVec = NULL;      // Weighting vector ...
    LPVOID        lpMemRht = NULL,      // Ptr to right arg global memory
                  lpMemRes = NULL;      // Ptr to result    ...
    LPAPLDIM      lpMemDimRht = NULL;   // Ptr to right arg dimensions
    LPAPLUINT     lpMemAxisHead = NULL, // Ptr to axis values, fleshed out by CheckAxis_EM
                  lpMemAxisTail = NULL, // Ptr to grade up of AxisHead
                  lpMemOdo = NULL,      // Ptr to odometer ...
                  lpMemWVec = NULL;     // Ptr to weighting vector ...
    APLUINT       ByteRes;              // # bytes in the result
    APLNELM       aplNELMRht,           // Right arg NELM
                  aplNELMAxis;          // Axis      ...
    APLRANK       aplRankRht,           // Right arg rank
                  aplRankRes;           // Result    ...
    APLNELM       uRht,                 // Right arg loop counter
                  uRes,                 // Result    ...
                  uOdo;                 // Odometer  ...
    APLDIM        aplDimNew;            //
    APLSTYPE      aplTypeRht,           // Right arg storage type
                  aplTypeRes;           // Result    ...
    APLNELMSIGN   iRht;                 // Right arg loop counter
    APLINT        apaOffRht,            // Right arg APA offset
                  apaMulRht;            // ...           multiplier
    APLUINT       aplFirstAxis,         // First axis value (if contiguous, then lowest)
                  aplLastAxis;          // Last ...                              highest
    UBOOL         bFract = FALSE,       // TRUE iff axis has fractional values
                  bTableRes,            // TRUE iff function is UTF16_COMMABAR
                  bReorder = FALSE;     // TRUE iff result values are reordered
                                        //   from those in the right arg
    LPPL_YYSTYPE  lpYYRes = NULL;       // Ptr to the result
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    LPVARARRAY_HEADER lpMemHdrRht;      // Ptr to right arg header

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the rank of the right arg
    aplRankRht = RankOfGlb (hGlbRht);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        while (TRUE)
        {
            // Ravel/table with axis requires
            //   that the axis be a scalar or vector subset of
            //   {iota}aplRankRht
            if (CheckAxis_EM (lptkAxis,         // The axis token
                              aplRankRht,       // All values less than this
                              FALSE,            // TRUE iff scalar or one-element vector only
                              FALSE,            // TRUE iff want sorted axes
                              TRUE,             // TRUE iff axes must be contiguous
                              FALSE,            // TRUE iff duplicate axes are allowed
                              NULL,             // Return TRUE iff fractional values present
                             &aplLastAxis,      // Return last axis value
                             &aplNELMAxis,      // Return # elements in axis vector
                             &hGlbAxis))        // Return HGLOBAL with APLINT axis values
                break;
            //   or a singleton fractional value
            if (CheckAxis_EM (lptkAxis,         // The axis token
                              aplRankRht + 1,   // All values less than this
                              TRUE,             // TRUE iff scalar or one-element vector only
                              FALSE,            // TRUE iff want sorted axes
                              TRUE,             // TRUE iff axes must be contiguous
                              FALSE,            // TRUE iff duplicate axes are allowed
                             &bFract,           // Return TRUE iff fractional values present
                             &aplLastAxis,      // Return last axis value
                             &aplNELMAxis,      // Return # elements in axis vector
                             &hGlbAxis)         // Return HGLOBAL with APLINT axis values
             && bFract)
                break;
            // Otherwise, it's an AXIS ERROR
            return NULL;
        } // End WHILE
    } else
    {
        // No axis means ravel all dimensions or table all but the first
        aplNELMAxis = aplRankRht;
        aplLastAxis = aplRankRht - 1;
    } // End IF/ELSE

    //***************************************************************
    // If the function is Comma-bar (table), the result is a
    //   matrix equivalent to
    //     ,[1{drop}{iota}{rho}{rho}R] R
    //***************************************************************
    bTableRes = (lptkFunc->tkData.tkChar EQ UTF16_COMMABAR);
    if (bTableRes)
    {
        // If the right arg is already a matrix, ...
        // Note that the axis operator does not apply to the table function
        //   so there can be no reordering.
        if (IsMatrix (aplRankRht))
        {
            // Copy the right arg
            hGlbRes = CopySymGlbDir_PTB (hGlbRht);

            goto YYALLOC_EXIT;
        } // End IF

        aplLastAxis = aplRankRht - 1;
        aplNELMAxis = aplRankRht - 1;
    } else
    // Empty axis means insert new last unit coordinate
    if (IsEmpty (aplNELMAxis))
        aplLastAxis = aplRankRht;

    // Set the value of the first axis
    // Note that for contiguous axes, <aplLastAxis> is
    //   the highest axis value, and <aplFirstAxis> is
    //   the lowest axis value, inclusive.
    // For example, for ,[1 3 2] (in origin-0)
    //   aplFirstAxis is 1, and
    //   aplLastAxis  is 3
    aplFirstAxis = 1 + aplLastAxis - aplNELMAxis;

    //***************************************************************
    // hGlbAxis is not-NULL iff there is an axis
    // aplNELMAxis contains the # elements in the axis.
    //***************************************************************

    // Calculate the rank of the result
    if (bFract)
        aplRankRes = aplRankRht + 1;
    else
        aplRankRes = aplRankRht + 1 - aplNELMAxis;

    //***************************************************************
    // Optimize ravel of a vector
    //***************************************************************

    if (IsVector (aplRankRht)       // If the right arg is a vector
     && IsVector (aplRankRes))      //   and the result is a vector, ...
    {
        // Increment the right arg reference count
        hGlbRes = CopySymGlbDirAsGlb (hGlbRht);

        goto YYALLOC_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRht = lpMemHdrRht = MyGlobalLockVar (hGlbRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRht)
    // Get the Array Type and NELM
    aplTypeRht = lpHeader->ArrType;
    aplNELMRht = lpHeader->NELM;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemDimRht = VarArrayBaseToDim (lpMemRht);

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayDataFmBase (lpMemRht);

    // Check to see if there is reordering
    if (hGlbAxis)
    {
        // Lock the memory to get a ptr to it
        lpMemAxisHead = MyGlobalLockInt (hGlbAxis);

        // Point to the grade-up of the first
        //   <aplRankRht> values in lpMemAxis
        lpMemAxisTail = &lpMemAxisHead[aplRankRht];

        // Get the first axis element + 1
        uRht = lpMemAxisHead[aplRankRht - aplNELMAxis] + 1;

        for (uRes = aplRankRht + 1 - aplNELMAxis; (!bReorder) && uRes < aplRankRht; uRes++, uRht++)
            bReorder = (uRht NE lpMemAxisHead[uRes]);
    } // End IF

    //***************************************************************
    // The NELM of the result is the same as that of the right arg
    //   as we're only re-arranging the right arg.
    //***************************************************************

    //***************************************************************
    // The three cases to consider are as follows:
    //  1.  X is fractional
    //      Insert a unit coordinate to the left of aplLastAxis.
    //  2.  X is empty
    //      Append a unit coordinate as the new last axis.
    //  3.  X is a scalar or vector of integers
    //      Transpose R according to X, and collapse the
    //      X coordinates to a single coordinate.
    //***************************************************************

    // If we're reordering and the right arg is ARRAY_APA,
    //   calculate the array size based upon APLINTs
    if (bReorder && IsSimpleAPA (aplTypeRht))
        // Set the array storage type for the result
        aplTypeRes = ARRAY_INT;
    else
        // Set the array storage type for the result
        aplTypeRes = aplTypeRht;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRht, aplRankRes);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    //***************************************************************
    // Now we can allocate the storage for the result.
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
    lpHeader->bSelSpec   = lpMemHdrRht->bSelSpec;
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRht;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Calculate the length of the raveled dimension
    if (bFract)
        aplDimNew = 1;
    else
    for (uRes = aplFirstAxis, aplDimNew = 1; uRes <= aplLastAxis; uRes++)
        aplDimNew *= lpMemDimRht[uRes];

    // Point to the result's dimension
    lpMemRes = VarArrayBaseToDim (lpMemRes);

    //***************************************************************
    // Copy the appropriate dimensions from the right arg
    //   to the result's dimension
    //***************************************************************
    if (bTableRes)
    {
        // Save the dimensions
        ((LPAPLDIM) lpMemRes)[0] = IsScalar (aplRankRht) ? 1 : lpMemDimRht[0];
        ((LPAPLDIM) lpMemRes)[1] = aplDimNew;
    } else
    // If we're collapsing dimensions, ...
    if (aplRankRes < aplRankRht)
    {
        for (uRes = uRht = 0; uRht < aplRankRht; uRht++)
        {
            // If it's not an axis dimension, ...
            if (uRht < aplFirstAxis
             || uRht > aplLastAxis)
                ((LPAPLDIM) lpMemRes)[uRes++] = lpMemDimRht[uRht];
            else
            // If, it's the last (highest) axis dimension, ...
            if (uRht EQ aplLastAxis)
                ((LPAPLDIM) lpMemRes)[uRes++] = aplDimNew;
        } // End FOR

        // Otherwise, it's the last (highest) axis dimension, ...
        if (uRht EQ aplLastAxis)
            ((LPAPLDIM) lpMemRes)[uRes++] = aplDimNew;
    } else
    // If we're inserting a unit coordinate, ...
    if (aplRankRes > aplRankRht)
    {
        // Insert the unit coordinate
        ((LPAPLDIM) lpMemRes)[aplLastAxis] = 1;

        for (uRes = uRht = 0; uRht < aplRankRht; uRes++)
        {
            // If it's not the last axis value, ...
            if (uRes NE aplLastAxis)
                ((LPAPLDIM) lpMemRes)[uRes] = lpMemDimRht[uRht++];
        } // End FOR
    } else
    {
        for (uRes = uRht = 0; uRht < aplRankRht; uRes++)
            ((LPAPLDIM) lpMemRes)[uRes] = lpMemDimRht[uRht++];
    } // End IF/ELSE

    // Point to the result's data
    lpMemRes = &((LPAPLDIM) lpMemRes)[aplRankRes];

    // As a shortcut, if there's no reordering we can copy the
    //   data from the right arg to the result
    if (!bReorder)
    {
        APLNELM aplNELM;

        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_FLOAT:
            case ARRAY_APA:
            case ARRAY_CHAR:
                // Account for the header and dimensions
                ByteRes -= sizeof (VARARRAY_HEADER)
                         + sizeof (APLDIM) * aplRankRes;
                CopyMemory (lpMemRes, lpMemRht, (APLU3264) ByteRes);

                break;

            case ARRAY_NESTED:
                // We're about to copy the entries from the right arg
                //   into the result.  If the right arg is ARRAY_NESTED,
                //   we need to increment each HGLOBAL's reference count.

                // In case the right arg is empty, include its prototype
                aplNELM = max (aplNELMRht, 1);

                // Loop through the right arg
                for (uRht = 0; uRht < aplNELM; uRht++)
                    // Increment the refcnt
                    DbgIncrRefCntDir_PTB (((LPAPLNESTED) lpMemRht)[uRht]);  // EXAMPLE:  ,NestedMarix

                // Account for the header and dimensions
                ByteRes -= sizeof (VARARRAY_HEADER)
                         + sizeof (APLDIM) * aplRankRes;
                CopyMemory (lpMemRes, lpMemRht, (APLU3264) ByteRes);

                break;

            case ARRAY_HETERO:
                // Make a copy of each RAT/VFP item
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                // Split cases based upon the item's storage type
                switch (GetPtrTypeDir (((LPAPLHETERO) lpMemRht)[uRes]))
                {
                    case PTRTYPE_STCONST:
                        // Copy the ptr
                        ((LPAPLHETERO *) lpMemRes)[uRes] = ((LPAPLHETERO *) lpMemRht)[uRes];

                        break;

                    case PTRTYPE_HGLOBAL:
                        // Increment the reference count
                        DbgIncrRefCntDir_PTB (((LPAPLNESTED) lpMemRht)[uRes]);  // EXAMPLE:  ,1 2{rho}'a' 1x

                        // Copy the ptr
                        ((LPAPLHETERO *) lpMemRes)[uRes] = ((LPAPLHETERO *) lpMemRht)[uRes];

                        break;

                    defstop
                        break;
                } // End FOR/SWITCH

                break;

            case ARRAY_RAT:
                // Make a copy of each RAT item
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                    mpq_init_set (&((LPAPLRAT) lpMemRes)[uRes], &((LPAPLRAT) lpMemRht)[uRes]);
                break;

            case ARRAY_VFP:
                // Make a copy of each RAT item
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                    mpfr_init_copy (&((LPAPLVFP) lpMemRes)[uRes], &((LPAPLVFP) lpMemRht)[uRes]);
                break;

            defstop
                break;
        } // End SWITCH
    } else
    // Reorder the right arg into the result
    {
        // If the right arg is an APA, ...
        if (IsSimpleAPA (aplTypeRht))
        {
#define lpAPA       ((LPAPLAPA) lpMemRht)
            // Get the APA parameters
            apaOffRht = lpAPA->Off;
            apaMulRht = lpAPA->Mul;
#undef  lpAPA
        } // End IF

        // Calculate space needed for the result
        ByteRes = aplRankRht * sizeof (APLUINT);

        // Check for overflow
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;

        //***************************************************************
        // Allocate space for the weighting vector which is
        //   {times}{backscan}1{drop}({rho}R),1
        //***************************************************************
        hGlbWVec = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (hGlbWVec EQ NULL)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemWVec = MyGlobalLock000 (hGlbWVec);

        // Loop through the dimensions of the right arg in reverse
        //   order {backscan} and compute the cumulative product
        //   (weighting vector).
        // Note we use a signed index variable because we're
        //   walking backwards and the test against zero must be
        //   made as a signed variable.
        for (uRes = 1, iRht = aplRankRht - 1; iRht >= 0; iRht--)
        {
            lpMemWVec[iRht] = uRes;
            uRes *= lpMemDimRht[iRht];
        } // End FOR

        // Calculate space needed for the result
        ByteRes = aplRankRht * sizeof (APLUINT);

        // Check for overflow
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;

        //***************************************************************
        // Allocate space for the odometer array, one value per dimension
        //   in the right arg, with values initially all zero (thanks to GHND).
        //***************************************************************
        hGlbOdo = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (hGlbOdo EQ NULL)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemOdo = MyGlobalLock000 (hGlbOdo);

        // Split cases based upon the storage type of the right arg/result
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                {
                    UINT uBitMask;

                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemAxisTail[uOdo]] * lpMemWVec[uOdo];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxisHead]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxisHead, aplRankRht);

                    uBitMask = BIT0 << (MASKLOG2NBIB & (UINT) uRht);

                    // Copy element # uRht from the right arg to lpMemRes[uRes]
                    ((LPAPLBOOL) lpMemRes)[uRes >> LOG2NBIB] |=
                      ((uBitMask & ((LPAPLBOOL) lpMemRht)[uRht >> LOG2NBIB]) ? TRUE : FALSE) << (MASKLOG2NBIB & (UINT) uRes);
                } // End FOR

                break;

            case ARRAY_INT:
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemAxisTail[uOdo]] * lpMemWVec[uOdo];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxishead]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxisHead, aplRankRht);

                    // Copy element # uRht from the right arg to lpMemRes[uRes]
                    ((LPAPLINT) lpMemRes)[uRes] = ((LPAPLINT) lpMemRht)[uRht];
                } // End FOR

                break;

            case ARRAY_FLOAT:
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemAxisTail[uOdo]] * lpMemWVec[uOdo];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxisHead]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxisHead, aplRankRht);

                    // Copy element # uRht from the right arg to lpMemRes[uRes]
                    ((LPAPLFLOAT) lpMemRes)[uRes] = ((LPAPLFLOAT) lpMemRht)[uRht];
                } // End FOR

                break;

            case ARRAY_CHAR:
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemAxisTail[uOdo]] * lpMemWVec[uOdo];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxisHead]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxisHead, aplRankRht);

                    // Copy element # uRht from the right arg to lpMemRes[uRes]
                    ((LPAPLCHAR) lpMemRes)[uRes] = ((LPAPLCHAR) lpMemRht)[uRht];
                } // End FOR

                break;

            case ARRAY_HETERO:
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemAxisTail[uOdo]] * lpMemWVec[uOdo];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxisHead]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxisHead, aplRankRht);

                    // Copy element # uRht from the right arg to lpMemRes[uRes]
                    // Note that APLHETERO elements are LPSYMENTRYs, so there's no
                    //   reference count to increment, or other special handling.
                    ((LPAPLHETERO) lpMemRes)[uRes] = ((LPAPLHETERO) lpMemRht)[uRht];
                } // End FOR

                break;

            case ARRAY_NESTED:
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemAxisTail[uOdo]] * lpMemWVec[uOdo];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxisHead]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxisHead, aplRankRht);

                    // Copy element # uRht from the right arg to lpMemRes[uRes]
                    // Note that APLNESTED elements are a mixture of LPSYMENTRYs
                    //   and HGLOBALs, so we need to run the HGLOBALs through
                    //   CopySymGlbDir_PTB so as to increment the reference count.
                    ((LPAPLNESTED) lpMemRes)[uRes] = CopySymGlbDir_PTB (((LPAPLNESTED) lpMemRht)[uRht]);
                } // End FOR

                break;

            case ARRAY_APA:
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemAxisTail[uOdo]] * lpMemWVec[uOdo];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxisHead]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxisHead, aplRankRht);

                    // Copy element # uRht from the right arg to lpMemRes[uRes]
                    ((LPAPLINT) lpMemRes)[uRes] = apaOffRht + apaMulRht * uRht;
                } // End FOR

                break;

            case ARRAY_RAT:
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemAxisTail[uOdo]] * lpMemWVec[uOdo];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxisHead]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxisHead, aplRankRht);

                    // Copy element # uRht from the right arg to lpMemRes[uRes]
                    mpq_init_set (&((LPAPLRAT) lpMemRes)[uRes], &((LPAPLRAT) lpMemRht)[uRht]);
                } // End FOR

                break;

            case ARRAY_VFP:
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemAxisTail[uOdo]] * lpMemWVec[uOdo];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxisHead]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxisHead, aplRankRht);

                    // Copy element # uRht from the right arg to lpMemRes[uRes]
                    mpfr_init_copy (&((LPAPLVFP) lpMemRes)[uRes], &((LPAPLVFP) lpMemRht)[uRht]);
                } // End FOR

                break;

            defstop
                break;
        } // End SWITCH
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
        MyGlobalUnlock (hGlbRht);  lpMemRht  = NULL;
    } // End IF

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbWVec, lpMemWVec);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbOdo, lpMemOdo);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbAxis, lpMemAxisHead);

    return lpYYRes;
} // End PrimFnMonCommaGlb_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydComma_EM_YY
//
//  Primitive function for dyadic Comma ("catenate/laminate")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydComma_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydComma_EM_YY
    (LPTOKEN       lptkLftArg,      // Ptr to left arg token
     LPTOKEN       lptkFunc,        // Ptr to function token
     LPTOKEN       lptkRhtArg,      // Ptr to right arg token
     LPTOKEN       lptkAxis)        // Ptr to axis token (may be NULL)

{
    APLUINT       aplAxis,          // The (one and only) axis value
                  uLft,             // Loop counter
                  uRht,             // ...
                  uBeg,             // ...
                  uEnd,             // ...
                  uEndLft,          // ...
                  uEndRht,          // ...
                  ByteRes;          // # bytes in the result
    APLRANK       aplRankLft,       // The rank of the left arg
                  aplRankRht,       // ...             right ...
                  aplRankRes,       // ...             result
                  aplRankLo,        // ...             smaller rank arg
                  aplRankHi;        // ...             larger  ...
    APLSTYPE      aplTypeLft,       // Left arg storage type
                  aplTypeRht,       // Right ...
                  aplTypeRes;       // Result   ...
    APLNELM       aplNELMLft,       // Left arg NELM
                  aplNELMRht,       // Right ...
                  aplNELMRes;       // Result   ...
    HGLOBAL       hGlbLft = NULL,   // Left arg global memory handle
                  hGlbRht = NULL,   // Right ...
                  hGlbRes = NULL,   // Result   ...
                  hGlbNMT;          // Non-empty ...
    LPVOID        lpMemLft = NULL,  // Ptr to left arg global memory
                  lpMemRht = NULL,  // ...    right ...
                  lpMemRes = NULL,  // ...    result   ...
                  lpSymGlbLft,      // ...    left arg LPSYMENTRY/HGLOBAL
                  lpSymGlbRht;      // ...    right ...
    LPAPLDIM      lpMemDimLft,      // Ptr to left arg dimensions
                  lpMemDimRht,      // ...    right ...
                  lpMemDimRes,      // ...    result   ...
                  lpMemDimLo,       // ...    smaller rank dimensions
                  lpMemDimHi;       // ...    larger  ...
    APLDIM        aplDimTmp,
                  aplDimBeg,
                  aplDimLftEnd,
                  aplDimRhtEnd,
                  aplDim1 = 1;
    UBOOL         bFract = FALSE;   // TRUE iff the axis value is fractional (laminate)
    UINT          uBitMaskLft,      // Left arg bit mask for trundling through Booleans
                  uBitMaskRht,      // Right ...
                  uBitIndexRes;     // Result   ...
    APLINT        aplIntegerLft,    // Left arg temporary integer value
                  aplIntegerRht,    // Right ...
                  apaOffLft,        // Left arg APA offset
                  apaOffRht,        // Right ...
                  apaMulLft,        // Left arg APA multiplier
                  apaMulRht;        // Right ...
    APLFLOAT      aplFloatLft,      // Left arg float temporary value
                  aplFloatRht;      // Right ...
    APLCHAR       aplCharLft,       // Left arg temporary character value
                  aplCharRht;       // Right ...
    APLLONGEST    aplVal;           // Temporary value
    LPPL_YYSTYPE  lpYYRes = NULL;   // Ptr to the result
    LPSYMENTRY    lpSymTmp;         // Ptr to temporary LPSYMENTRY
    LPPLLOCALVARS lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;     // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Both arguments are within bounds
    Assert (aplTypeLft < ARRAY_LENGTH);
    Assert (aplTypeRht < ARRAY_LENGTH);

    // Get the respective first values
    // Left arg
    if (IsScalar (aplRankLft)                   // Scalar
     && !IsEmpty (aplNELMLft)                   // and non-empty
     && !IsNested (aplTypeLft))                 // and non-nested
        GetFirstValueToken (lptkLftArg,         // Ptr to left arg token
                           &aplIntegerLft,      // Ptr to integer result
                           &aplFloatLft,        // Ptr to float ...
                           &aplCharLft,         // Ptr to WCHAR ...
                            NULL,               // Ptr to longest ...
                           &lpSymGlbLft,        // Ptr to lpSym/Glb ...
                            NULL,               // Ptr to ...immediate type ...
                            NULL);              // Ptr to array type ...
    else                                        // otherwise,
    if ((IsScalar (aplRankLft)                  // Scalar
      || IsEmpty (aplNELMLft))                  //   or empty
     && IsNested (aplTypeLft))                  // and nested
        GetFirstValueToken (lptkLftArg,         // Ptr to left arg token
                            NULL,               // Ptr to integer result
                            NULL,               // Ptr to float ...
                            NULL,               // Ptr to WCHAR ...
                            NULL,               // Ptr to longest ...
                           &lpSymGlbLft,        // Ptr to lpSym/Glb ...
                            NULL,               // Ptr to ...immediate type ...
                            NULL);              // Ptr to array type ...
    // Right arg
    if (IsScalar (aplRankRht)                   // Scalar
     && !IsEmpty (aplNELMRht)                   // and non-empty
     && !IsNested (aplTypeRht))                 // and non-nested
        GetFirstValueToken (lptkRhtArg,         // Ptr to right arg token
                           &aplIntegerRht,      // Ptr to integer result
                           &aplFloatRht,        // Ptr to float ...
                           &aplCharRht,         // Ptr to WCHAR ...
                            NULL,               // Ptr to longest ...
                           &lpSymGlbRht,        // Ptr to lpSym/Glb ...
                            NULL,               // Ptr to ...immediate type ...
                            NULL);              // Ptr to array type ...
    else                                        // otherwise,
    if ((IsScalar (aplRankRht) || IsEmpty (aplNELMRht)) // Scalar or empty
     && IsNested (aplTypeRht))                  // and nested
        GetFirstValueToken (lptkRhtArg,         // Ptr to right arg token
                            NULL,               // Ptr to integer result
                            NULL,               // Ptr to float ...
                            NULL,               // Ptr to WCHAR ...
                            NULL,               // Ptr to longest ...
                           &lpSymGlbRht,        // Ptr to lpSym/Glb ...
                            NULL,               // Ptr to ...immediate type ...
                            NULL);              // Ptr to array type ...
    // The rank of the result is the larger of the two args
    aplRankRes = max (aplRankLft, aplRankRht);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        while (TRUE)
        {
            // Catentate allows integer axis values only
            if (CheckAxis_EM (lptkAxis,         // The axis token
                              aplRankRes,       // All values less than this
                              TRUE,             // TRUE iff scalar or one-element vector only
                              FALSE,            // TRUE iff want sorted axes
                              FALSE,            // TRUE iff axes must be contiguous
                              FALSE,            // TRUE iff duplicate axes are allowed
                              NULL,             // Return TRUE iff fractional values present
                             &aplAxis,          // Return last axis value
                              NULL,             // Return # elements in axis vector
                              NULL))            // Return HGLOBAL with APLINT axis values
                break;
            // Laminate allows fractional values
            if (CheckAxis_EM (lptkAxis,         // The axis token
                              aplRankRes + 1,   // All values less than this
                              TRUE,             // TRUE iff scalar or one-element vector only
                              FALSE,            // TRUE iff want sorted axes
                              FALSE,            // TRUE iff axes must be contiguous
                              FALSE,            // TRUE iff duplicate axes are allowed
                             &bFract,           // Return TRUE iff fractional values present
                             &aplAxis,          // Return last axis value
                              NULL,             // Return # elements in axis vector
                              NULL)             // Return HGLOBAL with APLINT axis values
             && bFract)
                break;

            // Otherwise, it's an AXIS ERROR
            goto ERROR_EXIT;
        } // End WHILE
    } else
    {
        // No axis specified:
        //   if comma, use last dimension
        if (lptkFunc->tkData.tkChar EQ UTF16_COMMA)
            aplAxis = max (0, (APLRANKSIGN) aplRankRes - 1);
        else
        {
            Assert (lptkFunc->tkData.tkChar EQ UTF16_COMMABAR);

            // Otherwise, it's CommaBar on the first dimension
            aplAxis = 0;
        } // End IF/ELSE
    } // End IF/ELSE

    // Left and right scalar args are laminated
    if (IsScalar (aplRankLft)
     && IsScalar (aplRankRht))
    {
        bFract = TRUE;
        aplAxis = 0;
    } // End IF

    // If it's laminate, add one dimension
    aplRankRes += bFract;

    //***************************************************************
    // Check for RANK & LENGTH ERRORs.
    // If either argument is a scalar, no error.
    // Catenate:
    //      The arguments may differ in rank by 0 or 1 only.
    //      If the arguments differ in rank by 1, then the shape of the
    //        arg of larger rank without the axis dimension must be the
    //        same as that of shape of the arg of the smaller rank.
    // Laminate:
    //      The arguments must be of the same rank and shape.
    //***************************************************************

    // Get left and right arg's global ptrs
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // If neither arg is a scalar, ...
    if (!IsScalar (aplRankLft)
     && !IsScalar (aplRankRht))
    {
        // Skip over the headers to the dimensions
        lpMemDimLft = VarArrayBaseToDim (lpMemLft);
        lpMemDimRht = VarArrayBaseToDim (lpMemRht);

        // If the ranks differ, ...
        if (aplRankLft NE aplRankRht)
        {
            // If laminate or the ranks differ by more than 1, ...
            if (bFract || abs64 ((APLRANKSIGN) (aplRankLft - aplRankRht)) NE 1)
                goto RANK_EXIT;

            // The shapes must be equal except for aplAxis in the larger rank arg
            for (uRht = uLft = 0; uRht < aplRankRht && uLft < aplRankLft; uLft++, uRht++)
            {
                // If the right arg is the larger rank, ...
                if (aplRankLft < aplRankRht)
                    // If this is the axis dimension, skip it
                    uRht += (uRht EQ aplAxis);
                else
                    // If this is the axis dimension, skip it
                    uLft += (uLft EQ aplAxis);

                // Compare the dimensions
                if (lpMemDimLft[uLft] NE lpMemDimRht[uRht])
                    goto LENGTH_EXIT;
            } // End FOR
        } else
        // The ranks are the same
        {
            // For catenate, the shapes must be equal except for aplAxis.
            // For laminate, the shapes must be equal.
            for (uRht = 0; uRht < aplRankRht; uRht++)
            if ((bFract || uRht NE aplAxis)             // Laminate or not aplAxis
             && lpMemDimLft[uRht] NE lpMemDimRht[uRht]) // Compare the dimensions
                goto LENGTH_EXIT;
        } // End IF/ELSE
    } // End IF

    // If left arg is scalar, set dimension to 1
    if (IsScalar (aplRankLft))
        lpMemDimLft = &aplDim1;
    else
        // Skip over the header to the dimensions
        lpMemDimLft = VarArrayBaseToDim (lpMemLft);

    // If right arg is scalar, set dimension to 1
    if (IsScalar (aplRankRht))
        lpMemDimRht = &aplDim1;
    else
        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemRht);

    // Get a ptr to the dimensions of the larger (or equal) rank arg
    if (aplRankLft < aplRankRht)
    {
        lpMemDimHi  = lpMemDimRht;
        lpMemDimLo  = lpMemDimLft;
        aplRankHi   = aplRankRht;
        aplRankLo   = aplRankLft;
    } else
    {
        lpMemDimHi  = lpMemDimLft;
        lpMemDimLo  = lpMemDimRht;
        aplRankHi   = aplRankLft;
        aplRankLo   = aplRankRht;
    } // End IF/ELSE

    // Calculate the product of the non-axis dimensions
    for (uRht = 0, aplDimTmp = 1; uRht < aplRankHi; uRht++)
    if (bFract || uRht NE aplAxis)
        aplDimTmp *= lpMemDimHi[uRht];

    // Calculate the NELM of the result
    if (bFract)
        aplNELMRes = aplDimTmp * 2;
    else
    if (aplRankLft NE aplRankRht)
        aplNELMRes = aplDimTmp * (lpMemDimHi [aplAxis] + 1);
    else
        aplNELMRes = aplDimTmp * (lpMemDimLft[aplAxis] + lpMemDimRht[aplAxis]);

    // Determine the storage type of the result based upon
    //   the storage types of the left and right args
    //   as well as the NELMs
    if (IsEmpty (aplNELMLft))
    {
        aplTypeRes = aplTypeLft = aplTypeRht;
        hGlbNMT    = hGlbRht;
    } else
    if (IsEmpty (aplNELMRht))
    {
        aplTypeRes = aplTypeRht = aplTypeLft;
        hGlbNMT    = hGlbLft;
    } else
        aplTypeRes = aTypePromote[aplTypeLft][aplTypeRht];

    // Check for Boolean result
    if ((IsSimpleBool (aplTypeLft)
      && IsTknBooleanAPA (lptkRhtArg))
     || (IsSimpleBool (aplTypeRht)
      && IsTknBooleanAPA (lptkLftArg)))
        aplTypeRes = ARRAY_BOOL;

    // Check for APA result
    if (IsSimpleAPA (aplTypeRes))
    {
        // If the ranks differ, ...
        if (aplRankLft NE aplRankRht)
        {
            LPAPLAPA lpaplAPARes,
                     lpaplAPALo;

            // Allocate a new APA

            //***************************************************************
            // Calculate space needed for the result
            //***************************************************************
            ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

            //***************************************************************
            // Check for overflow
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
            lpHeader->ArrType    = aplTypeRes;
////////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
            lpHeader->RefCnt     = 1;
            lpHeader->NELM       = aplNELMRes;
            lpHeader->Rank       = aplRankRes;
#undef  lpHeader
            // Skip over the header to the dimensions
            lpMemDimRes = VarArrayBaseToDim (lpMemRes);

            // Copy the dimensions from the higher rank arg
            CopyMemory (lpMemDimRes, lpMemDimHi, (APLU3264) (aplRankRes * sizeof (APLDIM)));

            // Change the empty dimension
            lpMemDimRes[aplAxis]++;

            // Point to the APA data
            lpaplAPARes = VarArrayDimToData (lpMemDimRes, aplRankRes);
            lpaplAPALo  = VarArrayDimToData (lpMemDimLo , aplRankLo );

            // Copy the APA data
            lpaplAPARes->Off = lpaplAPALo->Off;
            lpaplAPARes->Mul = lpaplAPALo->Mul;

            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
        } else
            // Copy the non-empty arg
            hGlbRes = CopySymGlbDirAsGlb (hGlbNMT);

        goto YYALLOC_EXIT;
    } // End IF

    // If the result is empty, use the prototype of the right arg
    if (IsEmpty (aplNELMRes))
    {
        // Split cases based upon the right arg's storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_FLOAT:
                aplTypeRes = ARRAY_BOOL;

                break;

            case ARRAY_CHAR:
                aplTypeRes = ARRAY_CHAR;

                break;

            case ARRAY_RAT:
            case ARRAY_VFP:
            case ARRAY_NESTED:
                break;

            case ARRAY_APA:         // Can't happen
            case ARRAY_HETERO:      // Can't happen
            defstop
                break;
        } // End SWITCH
    } // End IF

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

    // Skip over the result header to the dimensions
    lpMemDimRes = VarArrayBaseToDim (lpMemRes);

    // Skip over the result header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemRes);

    // Copy the dimensions of the larger rank arg
    for (uRht = 0; uRht < aplRankHi; uRht++)
        lpMemDimRes[uRht + (bFract && (aplAxis <= uRht))] = lpMemDimHi[uRht];

    // Add in the axis dimension
    if (bFract)
        lpMemDimRes[aplAxis] = 2;
    else
    if (aplRankLft NE aplRankRht)
        lpMemDimRes[aplAxis]++;
    else
        lpMemDimRes[aplAxis] = lpMemDimLft[aplAxis] + lpMemDimRht[aplAxis];

    // Calculate the product of the left/right arg dimensions
    //   before the axis
    for (uBeg = 0,
           aplDimBeg = 1;
         uBeg < aplAxis;
         uBeg++)
        aplDimBeg *= lpMemDimRes[uBeg];

    // Calculate the product of the left arg dimensions
    //   at and beyond the axis taking into account
    //   laminate, scalar extension, and rank differing by one
    aplDimLftEnd = 1;
    if (IsScalar (aplRankLft))
        for (uEnd = aplAxis + !bFract;
             uEnd < aplRankRht;
             uEnd++)
            aplDimLftEnd *= lpMemDimRht[uEnd];
    else
        for (uEnd = aplAxis;
             uEnd < aplRankLft;
             uEnd++)
            aplDimLftEnd *= lpMemDimLft[uEnd];

    // Calculate the product of the right arg dimensions
    //   at and beyond the axis taking into account
    //   laminate, scalar extension, and rank differing by one
    aplDimRhtEnd = 1;
    if (IsScalar (aplRankRht))
        for (uEnd = aplAxis + !bFract;
             uEnd < aplRankLft;
             uEnd++)
            aplDimRhtEnd *= lpMemDimLft[uEnd];
    else
        for (uEnd = aplAxis;
             uEnd < aplRankRht;
             uEnd++)
            aplDimRhtEnd *= lpMemDimRht[uEnd];

    // Initialize the bit masks and index for Boolean arguments
    uBitMaskLft = uBitMaskRht = BIT0;
    uBitIndexRes = 0;

    // If the left arg is not a scalar or is a global numeric,
    //   skip over its header and dimensions to the data
    //   and get the APA values if appropriate
    if (!IsScalar (aplRankLft)
     || IsGlbNum (aplTypeLft))
    {
        // Skip over the header and dimensions to the data
        lpMemLft = VarArrayDataFmBase (lpMemLft);

        // If the left arg is an APA, ...
        if (IsSimpleAPA (aplTypeLft))
        {
#define lpAPA       ((LPAPLAPA) lpMemLft)
            // Get the APA parameters
            apaOffLft = lpAPA->Off;
            apaMulLft = lpAPA->Mul;
#undef  lpAPA
            uEndLft   = 0;
        } // End IF
    } // End IF

    // If the right arg is not a scalar or is a global numeric,
    //   skip over its header and dimensions to the data
    //   and get the APA values if appropriate
    if (!IsScalar (aplRankRht)
     || IsGlbNum (aplTypeRht))
    {
        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemRht);

        // If the right arg is an APA, ...
        if (IsSimpleAPA (aplTypeRht))
        {
#define lpAPA       ((LPAPLAPA) lpMemRht)
            // Get the APA parameters
            apaOffRht = lpAPA->Off;
            apaMulRht = lpAPA->Mul;
#undef  lpAPA
            uEndRht   = 0;
        } // End IF
    } // End IF

    // Split cases based upon the result's storage type
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:                // Res = BOOL, Lft/Rht = BOOL
            // If the result is Boolean, then both args are Boolean-valued
            //   (either or both could be a Boolean-valued APA)

            Assert (!IsSimpleAPA (aplTypeLft) || (aplNELMLft EQ 0) || (apaMulLft EQ 0 && IsBooleanValue (apaOffLft)));
            Assert (!IsSimpleAPA (aplTypeRht) || (aplNELMRht EQ 0) || (apaMulRht EQ 0 && IsBooleanValue (apaOffRht)));

            // Loop through the leading dimensions
            for (uBeg = 0; uBeg < aplDimBeg; uBeg++)
            {
                // If the left arg is a scalar, ...
                if (IsScalar (aplRankLft))
                    // Loop through the left arg's trailing dimensions
                    for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        *((LPAPLBOOL) lpMemRes) |= (APLBOOL) (aplIntegerLft << uBitIndexRes);

                        // Increment the result bit index
                        uBitIndexRes++;

                        // Check for end-of-byte
                        if (uBitIndexRes EQ NBIB)
                        {
                            uBitIndexRes = 0;           // Start over
                            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                        } // End IF
                    } // End FOR
                else
                    // Loop through the left arg's trailing dimensions
                    for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        if (IsSimpleAPA (aplTypeLft))
                            *((LPAPLBOOL) lpMemRes) |= apaOffLft << uBitIndexRes;
                        else
                            *((LPAPLBOOL) lpMemRes) |= ((uBitMaskLft & *((LPAPLBOOL) lpMemLft)) ? TRUE : FALSE) << uBitIndexRes;

                        // Increment the result bit index
                        uBitIndexRes++;

                        // Check for end-of-byte
                        if (uBitIndexRes EQ NBIB)
                        {
                            uBitIndexRes = 0;           // Start over
                            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                        } // End IF

                        // Shift over the left bit mask
                        uBitMaskLft <<= 1;

                        // Check for end-of-byte
                        if (uBitMaskLft EQ END_OF_BYTE)
                        {
                            uBitMaskLft = BIT0;         // Start over
                            ((LPAPLBOOL) lpMemLft)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                // If the right arg is a scalar, ...
                if (IsScalar (aplRankRht))
                    // Loop through the right arg's trailing dimensions
                    for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        *((LPAPLBOOL) lpMemRes) |= (APLBOOL) (aplIntegerRht << uBitIndexRes);

                        // Increment the result bit index
                        uBitIndexRes++;

                        // Check for end-of-byte
                        if (uBitIndexRes EQ NBIB)
                        {
                            uBitIndexRes = 0;           // Start over
                            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                        } // End IF
                    } // End FOR
                else
                    // Loop through the right arg's trailing dimensions
                    for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        if (IsSimpleAPA (aplTypeRht))
                            *((LPAPLBOOL) lpMemRes) |= apaOffRht << uBitIndexRes;
                        else
                            *((LPAPLBOOL) lpMemRes) |= ((uBitMaskRht & *((LPAPLBOOL) lpMemRht)) ? TRUE : FALSE) << uBitIndexRes;

                        // Increment the result bit index
                        uBitIndexRes++;

                        // Check for end-of-byte
                        if (uBitIndexRes EQ NBIB)
                        {
                            uBitIndexRes = 0;           // Start over
                            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                        } // End IF

                        // Shift over the right bit mask
                        uBitMaskRht <<= 1;

                        // Check for end-of-byte
                        if (uBitMaskRht EQ END_OF_BYTE)
                        {
                            uBitMaskRht = BIT0;         // Start over
                            ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                        } // End IF
                } // End FOR
            } // End FOR

            break;

        case ARRAY_INT:                 // Res = INT, Lft/Rht = BOOL/INT/APA
            // Loop through the leading dimensions
            for (uBeg = 0; uBeg < aplDimBeg; uBeg++)
            {
                // Split cases based upon the left arg's storage type
                switch (aplTypeLft)
                {
                    case ARRAY_BOOL:    // Res = INT, Lft = BOOL
                        // If the left arg is a scalar, ...
                        if (IsScalar (aplRankLft))
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLINT) lpMemRes)++ = aplIntegerLft;
                            } // End FOR
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLINT) lpMemRes)++ = (uBitMaskLft & *((LPAPLBOOL) lpMemLft)) ? TRUE : FALSE;

                                // Shift over the bit mask
                                uBitMaskLft <<= 1;

                                // Check for end-of-byte
                                if (uBitMaskLft EQ END_OF_BYTE)
                                {
                                    uBitMaskLft = BIT0;         // Start over
                                    ((LPAPLBOOL) lpMemLft)++;   // Skip to next byte
                                } // End IF
                            } // End FOR
                        break;

                    case ARRAY_INT:     // Res = INT, Lft = INT
                        // If the left arg is a scalar, ...
                        if (IsScalar (aplRankLft))
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLINT) lpMemRes)++ = aplIntegerLft;
                            } // End FOR
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLINT) lpMemRes)++ = *((LPAPLINT) lpMemLft)++;
                            } // End FOR
                        break;

                    case ARRAY_APA:     // Res = INT, Lft = APA
                        // Loop through the right arg's trailing dimensions
                        for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            *((LPAPLINT) lpMemRes)++ = apaOffLft + apaMulLft * uEndLft++;
                        } // End FOR

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Split cases based upon the right arg's storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:    // Res = INT, Rht = BOOL
                        // If the right arg is a scalar, ...
                        if (IsScalar (aplRankRht))
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLINT) lpMemRes)++ = aplIntegerRht;
                            } // End FOR
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLINT) lpMemRes)++ = (uBitMaskRht & *((LPAPLBOOL) lpMemRht)) ? TRUE : FALSE;

                                // Shift over the bit mask
                                uBitMaskRht <<= 1;

                                // Check for end-of-byte
                                if (uBitMaskRht EQ END_OF_BYTE)
                                {
                                    uBitMaskRht = BIT0;         // Start over
                                    ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                                } // End IF
                            } // End FOR
                        break;

                    case ARRAY_INT:     // Res = INT, Rht = INT
                        // If the right arg is a scalar, ...
                        if (IsScalar (aplRankRht))
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLINT) lpMemRes)++ = aplIntegerRht;
                            } // End FOR
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLINT) lpMemRes)++ = *((LPAPLINT) lpMemRht)++;
                            } // End FOR
                        break;

                    case ARRAY_APA:     // Res = INT, Rht = APA
                        // Loop through the right arg's trailing dimensions
                        for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            *((LPAPLINT) lpMemRes)++ = apaOffRht + apaMulRht * uEndRht++;
                        } // End FOR

                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR

            break;

        case ARRAY_FLOAT:               // Res = FLOAT, Lft/Rht = BOOL/INT/APA/FLOAT
            // Loop through the leading dimensions
            for (uBeg = 0; uBeg < aplDimBeg; uBeg++)
            {
                // Split cases based upon the left arg's storage type
                switch (aplTypeLft)
                {
                    case ARRAY_BOOL:    // Res = FLOAT, Lft = BOOL
                        // If the left arg is a scalar, ...
                        if (IsScalar (aplRankLft))
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLFLOAT) lpMemRes)++ = aplFloatLft;
                            } // End FOR
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLFLOAT) lpMemRes)++ = (uBitMaskLft & *((LPAPLBOOL) lpMemLft)) ? TRUE : FALSE;

                                // Shift over the bit mask
                                uBitMaskLft <<= 1;

                                // Check for end-of-byte
                                if (uBitMaskLft EQ END_OF_BYTE)
                                {
                                    uBitMaskLft = BIT0;         // Start over
                                    ((LPAPLBOOL) lpMemLft)++;   // Skip to next byte
                                } // End IF
                            } // End FOR
                        break;

                    case ARRAY_INT:     // Res = FLOAT, Lft = INT
                        // If the left arg is a scalar, ...
                        if (IsScalar (aplRankLft))
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLFLOAT) lpMemRes)++ = aplFloatLft;
                            } // End FOR
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLFLOAT) lpMemRes)++ = (APLFLOAT) *((LPAPLINT) lpMemLft)++;
                            } // End FOR
                        break;

                    case ARRAY_FLOAT:   // Res = FLOAT, Lft = FLOAT
                        // If the left arg is a scalar, ...
                        if (IsScalar (aplRankLft))
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLFLOAT) lpMemRes)++ = aplFloatLft;
                            } // End FOR
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLFLOAT) lpMemRes)++ = *((LPAPLFLOAT) lpMemLft)++;
                            } // End FOR
                        break;

                    case ARRAY_APA:     // Res = FLOAT, Lft = APA
                        // Loop through the left arg's trailing dimensions
                        for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            *((LPAPLFLOAT) lpMemRes)++ = (APLFLOAT) (APLINT) (apaOffLft + apaMulLft * uEndLft++);
                        } // End FOR

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Split cases based upon the right arg's storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:    // Res = FLOAT, Rht = BOOL
                        // If the right arg is a scalar, ...
                        if (IsScalar (aplRankRht))
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLFLOAT) lpMemRes)++ = aplFloatRht;
                            } // End FOR
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLFLOAT) lpMemRes)++ = (uBitMaskRht & *((LPAPLBOOL) lpMemRht)) ? TRUE : FALSE;

                                // Shift over the bit mask
                                uBitMaskRht <<= 1;

                                // Check for end-of-byte
                                if (uBitMaskRht EQ END_OF_BYTE)
                                {
                                    uBitMaskRht = BIT0;         // Start over
                                    ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                                } // End IF
                            } // End FOR
                        break;

                    case ARRAY_INT:     // Res = FLOAT, Rht = INT
                        // If the right arg is a scalar, ...
                        if (IsScalar (aplRankRht))
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLFLOAT) lpMemRes)++ = aplFloatRht;
                            } // End FOR
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLFLOAT) lpMemRes)++ = (APLFLOAT) *((LPAPLINT) lpMemRht)++;
                            } // End FOR
                        break;

                    case ARRAY_FLOAT:   // Res = FLOAT, Rht = FLOAT
                        // If the right arg is a scalar, ...
                        if (IsScalar (aplRankRht))
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLFLOAT) lpMemRes)++ = aplFloatRht;
                            } // End FOR
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLFLOAT) lpMemRes)++ = *((LPAPLFLOAT) lpMemRht)++;
                            } // End FOR
                        break;

                    case ARRAY_APA:     // Res = FLOAT, Rht = APA
                        // Loop through the right arg's trailing dimensions
                        for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            *((LPAPLFLOAT) lpMemRes)++ = (APLFLOAT) (APLINT) (apaOffRht + apaMulRht * uEndRht++);
                        } // End FOR

                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR

            break;

        case ARRAY_CHAR:                // Res = CHAR, Lft/Rht = CHAR
            // Loop through the leading dimensions
            for (uBeg = 0; uBeg < aplDimBeg; uBeg++)
            {
                // If the left arg is a scalar, ...
                if (IsScalar (aplRankLft))
                    // Loop through the left arg's trailing dimensions
                    for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        *((LPAPLCHAR) lpMemRes)++ = aplCharLft;
                    } // End FOR
                else
                    // Loop through the left arg's trailing dimensions
                    for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        *((LPAPLCHAR) lpMemRes)++ = *((LPAPLCHAR) lpMemLft)++;
                    } // End FOR

                // If the right arg is a scalar, ...
                if (IsScalar (aplRankRht))
                    // Loop through the right arg's trailing dimensions
                    for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        *((LPAPLCHAR) lpMemRes)++ = aplCharRht;
                    } // End FOR
                else
                    // Loop through the right arg's trailing dimensions
                    for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        *((LPAPLCHAR) lpMemRes)++ = *((LPAPLCHAR) lpMemRht)++;
                    } // End FOR
            } // End FOR

            break;

        case ARRAY_HETERO:              // Res = HETERO, Lft/Rht = HETERO/Num/CHAR
        case ARRAY_NESTED:              // Res = NESTED, Lft/Rht = NESTED/HETERO/Num/CHAR
            // If the left arg is a simple scalar, ...
            if (IsScalar (aplRankLft)
             && IsSimpleNH (aplTypeLft))
            {
                // Split cases based upon the left arg storage type
                switch (aplTypeLft)
                {
                    case ARRAY_BOOL:
                    case ARRAY_INT:
                        aplVal = aplIntegerLft;

                        break;

                    case ARRAY_FLOAT:
                        aplVal = *(LPAPLLONGEST) &aplFloatLft;

                        break;

                    case ARRAY_CHAR:
                        aplVal = aplCharLft;

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Make an LPSYMENTRY out of it
                lpSymGlbLft =
                  MakeSymEntry_EM (TranslateArrayTypeToImmType (aplTypeLft),    // Immediate type
                                  &aplVal,                                      // Ptr to immediate value
                                   lptkFunc);                                   // Ptr to function token
                if (lpSymGlbLft EQ NULL)
                    goto ERROR_EXIT;
            } // End IF

            // If the right arg is a simple scalar, ...
            if (IsScalar (aplRankRht)
             && IsSimpleNH (aplTypeRht))
            {
                // Split cases based upon the right arg storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:
                    case ARRAY_INT:
                        aplVal = aplIntegerRht;

                        break;

                    case ARRAY_FLOAT:
                        aplVal = *(LPAPLLONGEST) &aplFloatRht;

                        break;

                    case ARRAY_CHAR:
                        aplVal = aplCharRht;

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Make an LPSYMENTRY out of it
                lpSymGlbRht =
                  MakeSymEntry_EM (TranslateArrayTypeToImmType (aplTypeRht),    // Immediate type
                                  &aplVal,                                      // Ptr to immediate value
                                   lptkFunc);                                   // Ptr to function token
                if (lpSymGlbRht EQ NULL)
                    goto ERROR_EXIT;
            } // End IF

            // If the result is empty, use the prototype of the right arg
            if (IsEmpty (aplNELMRes))
                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (lpSymGlbRht);
            else
            // Loop through the leading dimensions
            for (uBeg = 0; uBeg < aplDimBeg; uBeg++)
            {
                // Split cases based upon the left arg's storage type
                switch (aplTypeLft)
                {
                    case ARRAY_BOOL:    // Res = NESTED, Lft = BOOL
                        // If the left arg is a scalar, ...
                        if (IsScalar (aplRankLft))
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (lpSymGlbLft);
                            } // End FOR
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                aplVal = (uBitMaskLft & *((LPAPLBOOL) lpMemLft)) ? TRUE : FALSE;
                                *((LPAPLNESTED) lpMemRes)++ =
                                lpSymTmp =
                                  MakeSymEntry_EM (IMMTYPE_BOOL,    // Immediate type
                                                  &aplVal,          // Ptr to immediate value
                                                   lptkFunc);       // Ptr to function token
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                                // Shift over the bit mask
                                uBitMaskLft <<= 1;

                                // Check for end-of-byte
                                if (uBitMaskLft EQ END_OF_BYTE)
                                {
                                    uBitMaskLft = BIT0;         // Start over
                                    ((LPAPLBOOL) lpMemLft)++;   // Skip to next byte
                                } // End IF
                            } // End FOR
                        break;

                    case ARRAY_INT:     // Res = NESTED, Lft = INT
                        // If the left arg is a scalar, ...
                        if (IsScalar (aplRankLft))
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (lpSymGlbLft);
                            } // End FOR
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                aplVal = *((LPAPLINT) lpMemLft)++;
                                *((LPAPLNESTED) lpMemRes)++ =
                                lpSymTmp =
                                  MakeSymEntry_EM (IMMTYPE_INT,     // Immediate type
                                                  &aplVal,          // Ptr to immediate value
                                                  lptkFunc);        // Ptr to function token
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                            } // End FOR
                        break;

                    case ARRAY_FLOAT:   // Res = NESTED, Lft = FLOAT
                        // If the left arg is a scalar, ...
                        if (IsScalar (aplRankLft))
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (lpSymGlbLft);
                            } // End FOR
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                aplVal = *((LPAPLINT) lpMemLft)++;
                                *((LPAPLNESTED) lpMemRes)++ =
                                lpSymTmp =
                                  MakeSymEntry_EM (IMMTYPE_FLOAT,   // Immediate type
                                                  &aplVal,          // Ptr to immediate value
                                                  lptkFunc);        // Ptr to function token
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                            } // End FOR
                        break;

                    case ARRAY_CHAR:    // Res = NESTED, Lft = CHAR
                        // If the left arg is a scalar, ...
                        if (IsScalar (aplRankLft))
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (lpSymGlbLft);
                            } // End FOR
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                aplVal = *((LPAPLCHAR) lpMemLft)++;
                                *((LPAPLNESTED) lpMemRes)++ =
                                lpSymTmp =
                                  MakeSymEntry_EM (IMMTYPE_CHAR,    // Immediate type
                                                  &aplVal,          // Ptr to immediate value
                                                   lptkFunc);       // Ptr to function token
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                            } // End FOR
                        break;

                    case ARRAY_APA:     // Res = NESTED, Lft = APA
                        // Loop through the left arg's trailing dimensions
                        for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            aplVal = apaOffLft + apaMulLft * uEndLft++;
                            *((LPAPLNESTED) lpMemRes)++ =
                            lpSymTmp =
                              MakeSymEntry_EM (IMMTYPE_INT,         // Immediate type
                                              &aplVal,              // Ptr to immediate value
                                               lptkFunc);           // Ptr to function token
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                        } // End FOR

                        break;

                    case ARRAY_HETERO:  // Res = NESTED, Lft = HETERO
                    case ARRAY_NESTED:  // Res = NESTED, Lft = NESTED
                        // If the left arg is a scalar, ...
                        if (IsScalar (aplRankLft))
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (lpSymGlbLft);
                            } // End FOR
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (*((LPAPLNESTED) lpMemLft)++);
                            } // End FOR
                        break;

                    case ARRAY_RAT:     // Res = NESTED, Lft = RAT
                        // Loop through the left arg's trailing dimensions
                        for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            *((LPAPLNESTED) lpMemRes)++ =
                            lpSymTmp =
                              MakeGlbEntry_EM (ARRAY_RAT,       // Entry type
                                   ((LPAPLRAT) lpMemLft)++,     // Ptr to the value
                                               TRUE,            // TRUE iff we should initialize the target first
                                               lptkFunc);       // Ptr to function token
                            if (lpSymTmp EQ NULL)
                                goto ERROR_EXIT;
                        } // End FOR

                        break;

                    case ARRAY_VFP:     // Res = NESTED, Lft = VFP
                        // Loop through the left arg's trailing dimensions
                        for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            *((LPAPLNESTED) lpMemRes)++ =
                            lpSymTmp =
                              MakeGlbEntry_EM (ARRAY_VFP,       // Entry type
                                   ((LPAPLVFP) lpMemLft)++,     // Ptr to the value
                                               TRUE,            // TRUE iff we should initialize the target first
                                               lptkFunc);       // Ptr to function token
                            if (lpSymTmp EQ NULL)
                                goto ERROR_EXIT;
                        } // End FOR

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Split cases based upon the right arg's storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:    // Res = NESTED, Rht = BOOL
                        // If the right arg is a scalar, ...
                        if (IsScalar (aplRankRht))
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (lpSymGlbRht);
                            } // End FOR
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                aplVal = (uBitMaskRht & *((LPAPLBOOL) lpMemRht)) ? TRUE : FALSE;
                                *((LPAPLNESTED) lpMemRes)++ =
                                lpSymTmp =
                                  MakeSymEntry_EM (IMMTYPE_BOOL,    // Immediate type
                                                  &aplVal,          // Ptr to immediate value
                                                   lptkFunc);       // Ptr to function token
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;

                                // Shift over the bit mask
                                uBitMaskRht <<= 1;

                                // Check for end-of-byte
                                if (uBitMaskRht EQ END_OF_BYTE)
                                {
                                    uBitMaskRht = BIT0;         // Start over
                                    ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                                } // End IF
                            } // End FOR
                        break;

                    case ARRAY_INT:     // Res = NESTED, Rht = INT
                        // If the right arg is a scalar, ...
                        if (IsScalar (aplRankRht))
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (lpSymGlbRht);
                            } // End FOR
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                aplVal = *((LPAPLINT) lpMemRht)++;
                                *((LPAPLNESTED) lpMemRes)++ =
                                lpSymTmp =
                                  MakeSymEntry_EM (IMMTYPE_INT,     // Immediate type
                                                  &aplVal,          // Ptr to immediate value
                                                   lptkFunc);       // Ptr to function token
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                            } // End FOR
                        break;

                    case ARRAY_FLOAT:   // Res = NESTED, Rht = FLOAT
                        // If the right arg is a scalar, ...
                        if (IsScalar (aplRankRht))
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (lpSymGlbRht);
                            } // End FOR
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                aplVal = *((LPAPLINT) lpMemRht)++;
                                *((LPAPLNESTED) lpMemRes)++ =
                                lpSymTmp =
                                  MakeSymEntry_EM (IMMTYPE_FLOAT,   // Immediate type
                                                  &aplVal,          // Ptr to immediate value
                                                   lptkFunc);       // Ptr to function token
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                            } // End FOR
                        break;

                    case ARRAY_CHAR:    // Res = NESTED, Rht = CHAR
                        // If the right arg is a scalar, ...
                        if (IsScalar (aplRankRht))
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (lpSymGlbRht);
                            } // End FOR
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                aplVal = *((LPAPLCHAR) lpMemRht)++;
                                *((LPAPLNESTED) lpMemRes)++ =
                                lpSymTmp =
                                  MakeSymEntry_EM (IMMTYPE_CHAR,    // Immediate type
                                                  &aplVal,          // Ptr to immediate value
                                                   lptkFunc);       // Ptr to function token
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                            } // End FOR
                        break;

                    case ARRAY_APA:     // Res = NESTED, Rht = APA
                        // Loop through the right arg's trailing dimensions
                        for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            aplVal = apaOffRht + apaMulRht * uEndRht++;
                            *((LPAPLNESTED) lpMemRes)++ =
                            lpSymTmp =
                              MakeSymEntry_EM (IMMTYPE_INT,         // Immediate type
                                              &aplVal,              // Ptr to immediate value
                                               lptkFunc);           // Ptr to function token
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                        } // End FOR

                        break;

                    case ARRAY_HETERO:  // Res = NESTED, Rht = HETERO
                    case ARRAY_NESTED:  // Res = NESTED, Rht = NESTED
                        // If the right arg is a scalar, ...
                        if (IsScalar (aplRankRht))
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (lpSymGlbRht);
                            } // End FOR
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (*((LPAPLNESTED) lpMemRht)++);
                            } // End FOR
                        break;

                    case ARRAY_RAT:     // Res = NESTED, Rht = RAT
                        // Loop through the right arg's trailing dimensions
                        for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            *((LPAPLNESTED) lpMemRes)++ =
                            lpSymTmp =
                              MakeGlbEntry_EM (ARRAY_RAT,       // Entry type
                                   ((LPAPLRAT) lpMemRht)++,     // Ptr to the value
                                               TRUE,            // TRUE iff we should initialize the target first
                                               lptkFunc);       // Ptr to function token
                            if (lpSymTmp EQ NULL)
                                goto ERROR_EXIT;
                        } // End FOR

                        break;

                    case ARRAY_VFP:     // Res = NESTED, Rht = VFP
                        // Loop through the right arg's trailing dimensions
                        for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            *((LPAPLNESTED) lpMemRes)++ =
                            lpSymTmp =
                              MakeGlbEntry_EM (ARRAY_VFP,       // Entry type
                                   ((LPAPLVFP) lpMemRht)++,     // Ptr to the value
                                               TRUE,            // TRUE iff we should initialize the target first
                                               lptkFunc);       // Ptr to function token
                            if (lpSymTmp EQ NULL)
                                goto ERROR_EXIT;
                        } // End FOR

                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR

            break;

        case ARRAY_RAT:                 // Res = RAT
            // Loop through the leading dimensions
            for (uBeg = 0; uBeg < aplDimBeg; uBeg++)
            {
                // Split cases based upon the left arg's storage type
                switch (aplTypeLft)
                {
                    case ARRAY_BOOL:    // Res = RAT, Lft = BOOL
                        // If the left arg is a scalar, ...
                        if (IsScalar (aplRankLft))
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the BOOL to a RAT
                                mpq_init_set_sx (((LPAPLRAT) lpMemRes)++, aplIntegerLft, 1);
                            } // End FOR
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the BOOL to a RAT
                                mpq_init_set_sx (((LPAPLRAT) lpMemRes)++, (uBitMaskLft & *((LPAPLBOOL) lpMemLft)) ? TRUE : FALSE, 1);

                                // Shift over the bit mask
                                uBitMaskLft <<= 1;

                                // Check for end-of-byte
                                if (uBitMaskLft EQ END_OF_BYTE)
                                {
                                    uBitMaskLft = BIT0;         // Start over
                                    ((LPAPLBOOL) lpMemLft)++;   // Skip to next byte
                                } // End IF
                            } // End FOR
                        break;

                    case ARRAY_INT:     // Res = RAT, Lft = INT
                        // If the left arg is a scalar, ...
                        if (IsScalar (aplRankLft))
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the INT to a RAT
                                mpq_init_set_sx (((LPAPLRAT) lpMemRes)++, aplIntegerLft, 1);
                            } // End FOR
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the INT to a RAT
                                mpq_init_set_sx (((LPAPLRAT) lpMemRes)++, *((LPAPLINT) lpMemLft)++, 1);
                            } // End FOR
                        break;

                    case ARRAY_APA:     // Res = RAT, Lft = APA
                        // Loop through the left arg's trailing dimensions
                        for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Convert the APA to a RAT
                            mpq_init_set_sx (((LPAPLRAT) lpMemRes)++, apaOffLft + apaMulLft * uEndLft++, 1);
                        } // End FOR

                        break;

                    case ARRAY_RAT:     // Res = RAT, Lft = RAT
                        // If the left arg is a scalar, ...
                        if (IsScalar (aplRankLft))
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Copy the RAT to a RAT
                                mpq_init_set (((LPAPLRAT) lpMemRes)++, (LPAPLRAT) lpSymGlbLft);
                            } // End FOR
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Copy the RAT to a RAT
                                mpq_init_set (((LPAPLRAT) lpMemRes)++, ((LPAPLRAT) lpMemLft)++);
                            } // End FOR
                        break;

                    defstop
                        break;
                } // End SWITCH

                // Split cases based upon the right arg's storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:    // Res = RAT, Rht = BOOL
                        // If the right arg is a scalar, ...
                        if (IsScalar (aplRankRht))
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the BOOL to a RAT
                                mpq_init_set_sx (((LPAPLRAT) lpMemRes)++, aplIntegerRht, 1);
                            } // End FOR
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the BOOL to a RAT
                                mpq_init_set_sx (((LPAPLRAT) lpMemRes)++, (uBitMaskRht & *((LPAPLBOOL) lpMemRht)) ? TRUE : FALSE, 1);

                                // Shift over the bit mask
                                uBitMaskRht <<= 1;

                                // Check for end-of-byte
                                if (uBitMaskRht EQ END_OF_BYTE)
                                {
                                    uBitMaskRht = BIT0;         // Start over
                                    ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                                } // End IF
                            } // End FOR
                        break;

                    case ARRAY_INT:     // Res = RAT, Rht = INT
                        // If the right arg is a scalar, ...
                        if (IsScalar (aplRankRht))
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the INT to a RAT
                                mpq_init_set_sx (((LPAPLRAT) lpMemRes)++, aplIntegerRht, 1);
                            } // End FOR
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the INT to a RAT
                                mpq_init_set_sx (((LPAPLRAT) lpMemRes)++, *((LPAPLINT) lpMemRht)++, 1);
                            } // End FOR
                        break;

                    case ARRAY_APA:     // Res = RAT, Rht = APA
                        // Loop through the right arg's trailing dimensions
                        for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Convert the APA to a RAT
                            mpq_init_set_sx (((LPAPLRAT) lpMemRes)++, apaOffRht + apaMulRht * uEndRht++, 1);
                        } // End FOR

                        break;

                    case ARRAY_RAT:     // Res = RAT, Rht = RAT
                        // If the right arg is a scalar, ...
                        if (IsScalar (aplRankRht))
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Copy the RAT to a RAT
                                mpq_init_set (((LPAPLRAT) lpMemRes)++, (LPAPLRAT) lpSymGlbRht);
                            } // End FOR
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Copy the RAT to a RAT
                                mpq_init_set (((LPAPLRAT) lpMemRes)++, ((LPAPLRAT) lpMemRht)++);
                            } // End FOR
                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR

            break;

        case ARRAY_VFP:                 // Res = VFP
            // Loop through the leading dimensions
            for (uBeg = 0; uBeg < aplDimBeg; uBeg++)
            {
                // Split cases based upon the left arg's storage type
                switch (aplTypeLft)
                {
                    case ARRAY_BOOL:    // Res = VFP, Lft = BOOL
                        // If the left arg is a scalar, ...
                        if (IsScalar (aplRankLft))
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the BOOL to a VFP
                                mpfr_init_set_sx (((LPAPLVFP) lpMemRes)++, aplIntegerLft, MPFR_RNDN);
                            } // End FOR
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the BOOL to a VFP
                                mpfr_init_set_sx (((LPAPLVFP) lpMemRes)++, (uBitMaskLft & *((LPAPLBOOL) lpMemLft)) ? TRUE : FALSE, MPFR_RNDN);

                                // Shift over the bit mask
                                uBitMaskLft <<= 1;

                                // Check for end-of-byte
                                if (uBitMaskLft EQ END_OF_BYTE)
                                {
                                    uBitMaskLft = BIT0;         // Start over
                                    ((LPAPLBOOL) lpMemLft)++;   // Skip to next byte
                                } // End IF
                            } // End FOR
                        break;

                    case ARRAY_INT:     // Res = VFP, Lft = INT
                        // If the left arg is a scalar, ...
                        if (IsScalar (aplRankLft))
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the INT to a VFP
                                mpfr_init_set_sx (((LPAPLVFP) lpMemRes)++, aplIntegerLft, MPFR_RNDN);
                            } // End FOR
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the INT to a VFP
                                mpfr_init_set_sx (((LPAPLVFP) lpMemRes)++, *((LPAPLINT) lpMemLft)++, MPFR_RNDN);
                            } // End FOR
                        break;

                    case ARRAY_APA:     // Res = VFP, Lft = APA
                        // Loop through the left arg's trailing dimensions
                        for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Convert the APA to a VFP
                            mpfr_init_set_sx (((LPAPLVFP) lpMemRes)++, apaOffLft + apaMulLft * uEndLft++, MPFR_RNDN);
                        } // End FOR

                        break;

                    case ARRAY_FLOAT:
                        // If the left arg is a scalar, ...
                        if (IsScalar (aplRankLft))
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the FLT to a VFP
                                mpfr_init_set_d  (((LPAPLVFP) lpMemRes)++, aplFloatLft, MPFR_RNDN);
                            } // End FOR
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the FLT to a VFP
                                mpfr_init_set_d  (((LPAPLVFP) lpMemRes)++, *((LPAPLFLOAT) lpMemLft)++, MPFR_RNDN);
                            } // End FOR
                        break;

                    case ARRAY_RAT:     // Res = VFP, Lft = RAT
                        // If the left arg is a scalar, ...
                        if (IsScalar (aplRankLft))
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the RAT to a VFP
                                mpfr_init_set_q (((LPAPLVFP) lpMemRes)++, (LPAPLRAT) lpSymGlbLft, MPFR_RNDN);
                            } // End FOR
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the RAT to a VFP
                                mpfr_init_set_q (((LPAPLVFP) lpMemRes)++, ((LPAPLRAT) lpMemLft)++, MPFR_RNDN);
                            } // End FOR
                        break;

                    case ARRAY_VFP:     // Res = VFP, Lft = VFP
                        // If the left arg is a scalar, ...
                        if (IsScalar (aplRankLft))
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Copy the VFP to a VFP
                                mpfr_init_copy  (((LPAPLVFP) lpMemRes)++, (LPAPLVFP) lpSymGlbLft);
                            } // End FOR
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Copy the VFP to a VFP
                                mpfr_init_copy  (((LPAPLVFP) lpMemRes)++, ((LPAPLVFP) lpMemLft)++);
                            } // End FOR
                        break;

                    defstop
                        break;
                } // End SWITCH

                // Split cases based upon the right arg's storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:    // Res = VFP, Rht = BOOL
                        // If the right arg is a scalar, ...
                        if (IsScalar (aplRankRht))
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the BOOL to a VFP
                                mpfr_init_set_sx (((LPAPLVFP) lpMemRes)++, aplIntegerRht, MPFR_RNDN);
                            } // End FOR
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the BOOL to a VFP
                                mpfr_init_set_sx (((LPAPLVFP) lpMemRes)++, (uBitMaskRht & *((LPAPLBOOL) lpMemRht)) ? TRUE : FALSE, MPFR_RNDN);

                                // Shift over the bit mask
                                uBitMaskRht <<= 1;

                                // Check for end-of-byte
                                if (uBitMaskRht EQ END_OF_BYTE)
                                {
                                    uBitMaskRht = BIT0;         // Start over
                                    ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                                } // End IF
                            } // End FOR
                        break;

                    case ARRAY_INT:     // Res = VFP, Rht = INT
                        // If the right arg is a scalar, ...
                        if (IsScalar (aplRankRht))
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the INT to a VFP
                                mpfr_init_set_sx (((LPAPLVFP) lpMemRes)++, aplIntegerRht, MPFR_RNDN);
                            } // End FOR
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the INT to a VFP
                                mpfr_init_set_sx (((LPAPLVFP) lpMemRes)++, *((LPAPLINT) lpMemRht)++, MPFR_RNDN);
                            } // End FOR
                        break;

                    case ARRAY_APA:     // Res = VFP, Rht = APA
                        // Loop through the right arg's trailing dimensions
                        for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Convert the APA to a VFP
                            mpfr_init_set_sx (((LPAPLVFP) lpMemRes)++, apaOffRht + apaMulRht * uEndRht++, MPFR_RNDN);
                        } // End FOR

                        break;

                    case ARRAY_FLOAT:   // Res = VFP, Rht = FLT
                        // If the right arg is a scalar, ...
                        if (IsScalar (aplRankRht))
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the FLT to a VFP
                                mpfr_init_set_d (((LPAPLVFP) lpMemRes)++, aplFloatRht, MPFR_RNDN);
                            } // End FOR
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the FLT to a VFP
                                mpfr_init_set_d (((LPAPLVFP) lpMemRes)++, *((LPAPLFLOAT) lpMemRht)++, MPFR_RNDN);
                            } // End FOR
                        break;

                    case ARRAY_RAT:     // Res = VFP, Rht = RAT
                        // If the right arg is a scalar, ...
                        if (IsScalar (aplRankRht))
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the RAT to a VFP
                                mpfr_init_set_q (((LPAPLVFP) lpMemRes)++, (LPAPLRAT) lpSymGlbRht, MPFR_RNDN);
                            } // End FOR
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the RAT to a VFP
                                mpfr_init_set_q (((LPAPLVFP) lpMemRes)++, ((LPAPLRAT) lpMemRht)++, MPFR_RNDN);
                            } // End FOR
                        break;

                    case ARRAY_VFP:     // Res = VFP, Rht = VFP
                        // If the right arg is a scalar, ...
                        if (IsScalar (aplRankRht))
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Copy the VFP to a VFP
                                mpfr_init_copy (((LPAPLVFP) lpMemRes)++, (LPAPLVFP) lpSymGlbRht);
                            } // End FOR
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Copy the VFP to a VFP
                                mpfr_init_copy (((LPAPLVFP) lpMemRes)++, ((LPAPLVFP) lpMemRht)++);
                            } // End FOR
                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR

            break;

        case ARRAY_APA:
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
    if (lpMemLft)
    {
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

    if (lpMemRht)
    {
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    if (lpMemRes)
    {
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    return lpYYRes;
} // End PrimFnDydComma_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_comma.c
//***************************************************************************
