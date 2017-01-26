//***************************************************************************
//  NARS2000 -- Primitive Function Special
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
//  $PrimFnSyntaxError_EM
//
//  Primitive function SYNTAX ERROR
//***************************************************************************

LPPL_YYSTYPE PrimFnSyntaxError_EM
    (LPTOKEN lptkFunc
#ifdef DEBUG
, LPWCHAR APPEND_NAME
#endif
    )

{
#ifdef DEBUG
    static WCHAR wszTemp[1024];

    // Copy the error message to the temp
    MyStrcpyW (wszTemp, sizeof (wszTemp), ERRMSG_SYNTAX_ERROR);

    // Append the suffix to the temp
    MyStrcatW (wszTemp, sizeof (wszTemp), APPEND_NAME);

    ErrorMessageIndirectToken (wszTemp, lptkFunc);
#else
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR,
                               lptkFunc);
#endif
    return NULL;
} // End PrimFnSyntaxError_EM


//***************************************************************************
//  $PrimFnValenceError_EM
//
//  Primitive function VALENCE ERROR
//***************************************************************************

LPPL_YYSTYPE PrimFnValenceError_EM
    (LPTOKEN lptkFunc
#ifdef DEBUG
, LPWCHAR APPEND_NAME
#endif
    )

{
#ifdef DEBUG
    static WCHAR wszTemp[1024];

    // Copy the error message to the temp
    MyStrcpyW (wszTemp, sizeof (wszTemp), ERRMSG_VALENCE_ERROR);

    // Append the suffix to the temp
    MyStrcatW (wszTemp, sizeof (wszTemp), APPEND_NAME);

    ErrorMessageIndirectToken (wszTemp, lptkFunc);
#else
    ErrorMessageIndirectToken (ERRMSG_VALENCE_ERROR,
                               lptkFunc);
#endif
    return NULL;
} // End PrimFnValenceError_EM


//***************************************************************************
//  $PrimFnNonceError_EM
//
//  Primitive function NONCE ERROR
//***************************************************************************

LPPL_YYSTYPE PrimFnNonceError_EM
    (LPTOKEN lptkFunc
#ifdef DEBUG
, LPWCHAR APPEND_NAME
#endif
    )

{
#ifdef DEBUG
    static WCHAR wszTemp[1024];

    // Copy the error message to the temp
    MyStrcpyW (wszTemp, sizeof (wszTemp), ERRMSG_NONCE_ERROR);

    // Append the suffix to the temp
    MyStrcatW (wszTemp, sizeof (wszTemp), APPEND_NAME);

    ErrorMessageIndirectToken (wszTemp, lptkFunc);
#else
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR,
                               lptkFunc);
#endif
    return NULL;
} // End PrimFnNonceError_EM


//***************************************************************************
//  $PrimFnDomainError_EM
//
//  Primitive function DOMAIN ERROR
//***************************************************************************

LPPL_YYSTYPE PrimFnDomainError_EM
    (LPTOKEN lptkFunc
#ifdef DEBUG
, LPWCHAR APPEND_NAME
#endif
    )

{
#ifdef DEBUG
    static WCHAR wszTemp[1024];

    // Copy the error message to the temp
    MyStrcpyW (wszTemp, sizeof (wszTemp), ERRMSG_DOMAIN_ERROR);

    // Append the suffix to the temp
    MyStrcatW (wszTemp, sizeof (wszTemp), APPEND_NAME);

    ErrorMessageIndirectToken (wszTemp, lptkFunc);
#else
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR,
                               lptkFunc);
#endif
    return NULL;
} // End PrimFnDomainError_EM


//***************************************************************************
//  $PrimFnValueError_EM
//
//  Primitive function VALUE ERROR
//***************************************************************************

LPPL_YYSTYPE PrimFnValueError_EM
    (LPTOKEN lptkFunc
#ifdef DEBUG
, LPWCHAR APPEND_NAME
#endif
    )

{
#ifdef DEBUG
    static WCHAR wszTemp[1024];

    // Copy the error message to the temp
    MyStrcpyW (wszTemp, sizeof (wszTemp), ERRMSG_VALUE_ERROR);

    // Append the suffix to the temp
    MyStrcatW (wszTemp, sizeof (wszTemp), APPEND_NAME);

    // If there's a name attached to this error, ...
    if ((IsTknNamed   (lptkFunc)
      || IsTknAFO     (lptkFunc))
     && GetPtrTypeDir (lptkFunc->tkData.tkVoid) EQ PTRTYPE_STCONST
     && lptkFunc->tkData.tkSym->stHshEntry)
    {
        // Leading separator
        MyStrcatW (wszTemp, sizeof (wszTemp), L" (");

        // Append the name
        MyStrcatW (wszTemp, sizeof (wszTemp), lptkFunc->tkData.tkSym->stHshEntry->lpwCharName);

        // Trailing separator
        MyStrcatW (wszTemp, sizeof (wszTemp), L")");
    } // End IF

    ErrorMessageIndirectToken (wszTemp, lptkFunc);
#else
    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR,
                               lptkFunc);
#endif
    return NULL;
} // End PrimFnValueError_EM


//***************************************************************************
//  $PrimFnMonValenceError_EM
//
//  Primitive scalar monadic function VALENCE ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonValenceError_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonValenceError_EM
    (LPTOKEN    lptkFunc,           // Ptr to function token
     LPTOKEN    lptkRhtArg,         // Ptr to right arg token
     LPTOKEN    lptkAxis,           // Ptr to axis token (may be NULL)
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End PrimFnMonValenceError_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnMixed_EM_YY
//
//  Generate a prototype result for the monadic & dyadic primitive mixed functions
//***************************************************************************

LPPL_YYSTYPE PrimProtoFnMixed_EM_YY
    (LPPRIMFNS  lpPrimFn,           // Ptr to primitive function routine
     LPTOKEN    lptkLftArg,         // Ptr to left arg token
     LPTOKEN    lptkFunc,           // Ptr to function token
     LPTOKEN    lptkRhtArg,         // Ptr to right arg token
     LPTOKEN    lptkAxis)           // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result
    HGLOBAL      hGlbTmp,           // Temporary global memory handle
                 hSymGlbProto;      // Prototype ...

    // Call the original function
    lpYYRes = (*lpPrimFn) (lptkLftArg,      // Ptr to left arg token
                           lptkFunc,        // Ptr to function token
                           lptkRhtArg,      // Ptr to right arg token
                           lptkAxis);       // Ptr to axis token (may be NULL)
    if (lpYYRes NE NULL)
    // As this is a prototype function, convert
    //   the result to a prototype
    // Split cases based upon the result token type
    switch (lpYYRes->tkToken.tkFlags.TknType)
    {
        case TKT_VARIMMED:
            if (IsImmChr (lpYYRes->tkToken.tkFlags.ImmType))
                lpYYRes->tkToken.tkData.tkChar = L' ';
            else
            {
                lpYYRes->tkToken.tkFlags.ImmType  = IMMTYPE_BOOL;
                lpYYRes->tkToken.tkData.tkBoolean = FALSE;
            } // End IF/ELSE

            break;

        case TKT_VARARRAY:
            // Get the global memory handle
            hGlbTmp = lpYYRes->tkToken.tkData.tkGlbData;

            // Make the prototype
            hSymGlbProto =
              MakeMonPrototype_EM_PTB (hGlbTmp,     // Proto arg handle
                                       lptkFunc,    // Ptr to function token
                                       MP_CHARS);   // CHARs allowed
            if (!hSymGlbProto)
            {
                YYFree (lpYYRes); lpYYRes = NULL;
            } else
            {
                // Save back into the result
                lpYYRes->tkToken.tkData.tkGlbData = hSymGlbProto;

                // We no longer need this storage
                FreeResultGlobalVar (hGlbTmp); hGlbTmp = NULL;
            } // End IF/ELSE

            break;

        defstop
            break;
    } // End IF/SWITCH

    return lpYYRes;
} // End PrimProtoFnMixed_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnScalar_EM_YY
//
//  Generate a prototype result for the monadic & dyadic primitive scalar functions
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnScalar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnScalar_EM_YY
    (LPTOKEN    lptkLftArg,         // Ptr to left arg token
     LPTOKEN    lptkFunc,           // Ptr to function token
     LPTOKEN    lptkRhtArg,         // Ptr to right arg token
     LPTOKEN    lptkAxis)           // Ptr to axis token (may be NULL)

{
    HGLOBAL      hGlbLft,           // Left arg global memory handle
                 hGlbRht,           // Right ...
                 hSymGlbRes;        // Result   ...
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result

    // Get right arg's global memory handle
    hGlbRht = GetGlbHandle (lptkRhtArg);

    // If left arg is not present, ...
    if (lptkLftArg EQ NULL)
    {
        //***************************************************************
        // Scalar monadic primitive functions are not sensitive
        //   to the axis operator, so signal a syntax error if present
        //***************************************************************
        if (lptkAxis NE NULL)
            goto AXIS_SYNTAX_EXIT;

        //***************************************************************
        // Called monadically
        //***************************************************************

        // Make the prototype
        hSymGlbRes =
          MakeMonPrototype_EM_PTB (hGlbRht,     // Proto arg handle
                                   lptkFunc,    // Ptr to function token
                                   MP_NUMONLY); // Numerics only
    } else
    {
        //***************************************************************
        // Called dyadically
        //***************************************************************

        // Get left arg's global memory handle
        hGlbLft = GetGlbHandle (lptkLftArg);

        // Check for both args immediate
        if (hGlbLft EQ NULL
         && hGlbRht EQ NULL)
        {
            // Allocate a new YYRes
            lpYYRes = YYAlloc ();

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
            lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_BOOL;
////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkInteger  = 0;
            lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

            goto NORMAL_EXIT;
        } // End IF

        // If the left arg is global, ...
        if (hGlbLft NE NULL)
            // Set the ptr type bits
            hGlbLft = MakePtrTypeGlb (hGlbLft);
        // If the right arg is global, ...
        if (hGlbRht NE NULL)
            // Set the ptr type bits
            hGlbRht = MakePtrTypeGlb (hGlbRht);

        // Handle as dyadic prototype
        hSymGlbRes =
          MakeDydPrototype_EM_PTB (hGlbLft,                     // Left arg global memory handle (may be NULL if immediate)
                                   GetImmedType (lptkLftArg),   // Left arg immediate type
                                   lptkFunc,                    // Ptr to function token
                                   hGlbRht,                     // Right arg global memory handle
                                   GetImmedType (lptkRhtArg),   // Left arg immediate type
                                   lptkAxis);                   // Ptr to axis value token (may be NULL)
    } // End IF

    if (hSymGlbRes EQ NULL)
        goto ERROR_EXIT;

    Assert (GetPtrTypeDir (hSymGlbRes) EQ PTRTYPE_HGLOBAL);

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = hSymGlbRes;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    return lpYYRes;
} // End PrimProtoFnScalar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimIdentFnScalar_EM_YY
//
//  Generate an identity element for a primitive scalar dyadic function
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimIdentFnScalar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimIdentFnScalar_EM_YY
    (LPTOKEN lptkRhtOrig,               // Ptr to original right arg token
     LPTOKEN lptkFunc,                  // Ptr to function token
     LPTOKEN lptkRhtArg,                // Ptr to right arg token
     LPTOKEN lptkAxis)                  // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to identity element result
    APLSTYPE          aplTypeRht,           // Right arg storage type
                      aplTypeRes;           // Result    ...
    APLNELM           aplNELMRht,           // Right arg NELM
                      aplNELMRes,           // Result    ...
                      aplNELMAxis;          // Axis      ...
    APLRANK           aplRankRht,           // Right arg rank
                      aplRankRes;           // Result    ...
    HGLOBAL           hGlbRht = NULL,       // Right arg global memory handle
                      hGlbRes = NULL,       // Result    ...
                      hGlbAxis = NULL;      // Axis      ...
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg header
                      lpMemHdrRes = NULL;   // ...    result    ...
    LPVOID            lpMemRht,             // Ptr to right arg global memory
                      lpMemRes;             // Ptr to result    ...
    LPAPLDIM          lpMemDimRht,          // Ptr to right arg dimensions
                      lpMemDimRes;          // Ptr to result    ...
    APLLONGEST        aplLongestRht;        // Right arg immediate value
    APLUINT           ByteRes;              // # bytes in the result
    LPAPLUINT         lpMemAxisHead = NULL, // Ptr to axis values, fleshed out by CheckAxis_EM
                      lpMemAxisTail = NULL; // Ptr to grade up of AxisHead
    LPPRIMSPEC        lpPrimSpec;           // Ptr to function PRIMSPEC
    LPPRIMFLAGS       lpPrimFlags;          // Ptr to function PrimFlags entry
    LPPRIMIDENT       lpPrimIdent;          // Ptr to function PrimIdent entry
    APLUINT           uRht;                 // Loop counter

    // The right arg is the prototype item from
    //   the original empty arg.

    Assert (lptkRhtOrig NE NULL);
    Assert (lptkFunc    NE NULL);
    Assert (lptkRhtArg  NE NULL);

    // Get the corresponding lpPrimSpec
    lpPrimSpec = PrimSpecTab[SymTrans (lptkFunc)];

    // Get a ptr to the Primitive Function Flags
    lpPrimFlags = GetPrimFlagsPtr (lptkFunc);

    // If there's an identity element, ...
    if (lpPrimFlags NE NULL && lpPrimFlags->IdentElem)
        lpPrimIdent = &PrimIdent[lpPrimFlags->Index];
    else
        goto DOMAIN_EXIT;

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Check the axis values, fill in # elements in axis
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           aplRankRht,      // All values less than this
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
        lpMemAxisTail = &lpMemAxisHead[aplRankRht - aplNELMAxis];
    } else
        // No axis is the same as all axes
        aplNELMAxis = aplRankRht;

    // The result rank is the NELM axis
    aplRankRes = aplNELMAxis;

    // Calculate the storage type of the result
    aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (aplNELMRht,
                                               &aplTypeRht,
                                                lptkFunc,
                                                aplNELMRht,
                                               &aplTypeRht);
    if (IsErrorType (aplTypeRes))
        goto DOMAIN_EXIT;

    Assert (IsSimpleGlbNum (aplTypeRes)
         || IsNested (aplTypeRes));

    // If the result is simple numeric, ...
    if (IsSimpleNum (aplTypeRes))
    {
        // If the identity element is simple Boolean, ...
        if (lpPrimIdent->IsBool)
            // Make it Boolean
            aplTypeRes = ARRAY_BOOL;
        else
            // Make it float
            aplTypeRes = ARRAY_FLOAT;
    } // End IF

    // Get right arg global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // If the right arg is a simple scalar, ...
    if (hGlbRht EQ NULL)
    {
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = lpPrimIdent->IsBool ? IMMTYPE_BOOL
                                                                 : IMMTYPE_FLOAT;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        if (lpPrimIdent->IsBool)
            lpYYRes->tkToken.tkData.tkBoolean = lpPrimIdent->bIdentElem;
        else
            lpYYRes->tkToken.tkData.tkFloat   = lpPrimIdent->fIdentElem;
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

        goto NORMAL_EXIT;
    } // End IF

    // Skip over the header to the dimensions
    lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);

    //***************************************************************
    //  Code from here on down to the call to <PrimFnScalarCommon_EM>
    //    can be incorporated into that routine as an optimization.
    //***************************************************************

    //***************************************************************
    //  Calculate result NELM
    //***************************************************************
    if (lpMemAxisTail NE NULL)
        for (aplNELMRes = 1, uRht = 0; uRht < aplRankRes; uRht++)
            aplNELMRes *= lpMemDimRht[lpMemAxisTail[uRht]];
    else
        aplNELMRes = aplNELMRht;

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
    lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader        lpMemHdrRes
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
    lpMemDimRes = VarArrayBaseToDim (lpMemHdrRes);

    //***************************************************************
    // Fill in the result's dimension
    //***************************************************************
    if (lpMemAxisTail NE NULL)
        for (uRht = 0; uRht < aplRankRes; uRht++)
            *lpMemDimRes++ = lpMemDimRht[lpMemAxisTail[uRht]];
    else
        CopyMemory (lpMemDimRes, lpMemDimRht, (APLU3264) aplRankRes * sizeof (APLDIM));

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // If the right arg (and result) is simple or Global (numeric), ...
    if (IsSimpleGlbNum (aplTypeRht))
    {
        // Fill in the result data
        if (lpPrimIdent->IsBool)
        {
            // Split cases based upon the storage type
            switch (aplTypeRes)
            {
                case ARRAY_BOOL:
                    // If the identity element is 1, ...
                    if (lpPrimIdent->bIdentElem)
                        FillMemory (lpMemRes, (APLU3264) RoundUpBitsToBytes (aplNELMRes), 0xFF);
                    break;

                case ARRAY_RAT:
                    for (uRht = 0; uRht < aplNELMRes; uRht++)
                        mpq_init_set_sx (((LPAPLRAT) lpMemRes)++, lpPrimIdent->bIdentElem, 1);
                    break;

                case ARRAY_VFP:
                    for (uRht = 0; uRht < aplNELMRes; uRht++)
                        mpfr_init_set_si (((LPAPLVFP) lpMemRes)++, lpPrimIdent->bIdentElem, MPFR_RNDN);
                    break;

                defstop
                    break;
            } // End SWITCH
        } else
        {
            // Split cases based upon the storage type
            switch (aplTypeRes)
            {
                case ARRAY_FLOAT:
                    for (uRht = 0; uRht < aplNELMRes; uRht++)
                        *((LPAPLFLOAT) lpMemRes)++ = lpPrimIdent->fIdentElem;
                    break;

                case ARRAY_RAT:
                    for (uRht = 0; uRht < aplNELMRes; uRht++)
                        mpq_init_set_d  (((LPAPLRAT) lpMemRes)++, lpPrimIdent->fIdentElem);
                    break;

                case ARRAY_VFP:
                    for (uRht = 0; uRht < aplNELMRes; uRht++)
                        mpfr_init_set_d (((LPAPLVFP) lpMemRes)++, lpPrimIdent->fIdentElem, MPFR_RNDN);
                    break;

                defstop
                    break;
            } // End SWITCH
        } // End IF/ELSE
    } else
    {
        // The result is nested
        Assert (IsNested (aplTypeRes));

        // Handle nested prototypes
        aplNELMRht = max (aplNELMRht, 1);

        // Recurse into the array
        if (!PrimIdentFnScalarCommon_EM (lpMemRht,          // Ptr to right arg global memory data
                                         lpMemRes,          // ...    result ...
                                         aplNELMRht,        // Right arg NELM
                                         lpPrimIdent,       // Ptr to function PrimIdent entry
                                         lpPrimSpec,        // Ptr to function PRIMSPEC
                                         lptkAxis,          // Ptr to axis token
                                         lptkFunc))         // Ptr to function token
            goto ERROR_EXIT;
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
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken);

    goto NORMAL_EXIT;

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
        if (lpMemHdrRes NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    if (hGlbAxis NE NULL && lpMemAxisHead NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbAxis); lpMemAxisHead = lpMemAxisTail = NULL;
    } // End IF

    return lpYYRes;
} // End PrimIdentFnScalar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimIdentFnScalarCommon_EM
//
//  Common (recursive) routine to PrimIdentScalar_EM_YY
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimIdentFnScalarCommon_EM_YY"
#else
#define APPEND_NAME
#endif

UBOOL PrimIdentFnScalarCommon_EM
    (LPAPLHETERO lpMemRht,                  // Ptr to right arg global memory data
     LPAPLHETERO lpMemRes,                  // ...    result ...
     APLNELM     aplNELMRht,                // Right arg NELM
     LPPRIMIDENT lpPrimIdent,               // Ptr to function PrimIdent entry
     LPPRIMSPEC  lpPrimSpec,                // Ptr to function PRIMSPEC
     LPTOKEN     lptkAxis,                  // Ptr to axis token
     LPTOKEN     lptkFunc)                  // Ptr to function token

{
    APLUINT           uRht;                 // Loop counter
    HGLOBAL           hGlbItm = NULL,       // Item global memory handle
                      hGlbRes2 = NULL,      // Result2 ...
                      hGlbAxis = NULL;      // Axis ...
    APLSTYPE          aplTypeItm,           // Item storage type
                      aplTypeRes2;          // Result2 ...
    APLNELM           aplNELMItm,           // Item NELM
                      aplNELMRes2,          // Result2 ...
                      aplNELMAxis;          // Axis ...
    APLRANK           aplRankItm,           // Item rank
                      aplRankRes2;          // Result2 rank
    LPVARARRAY_HEADER lpMemHdrItm = NULL,   // Ptr to item header
                      lpMemHdrRes2 = NULL;  // ...    result2 ...
    LPVOID            lpMemItm,             // Ptr to item global memory
                      lpMemRes2;            // ...    result2 ...
    UBOOL             bRet = FALSE;         // TRUE iff the result is valid
    APLUINT           ByteRes2;             // # bytes in the result2
    LPAPLDIM          lpMemDimItm,          // Ptr to item dimensions
                      lpMemDimRes2;         // Ptr to result2    ...
    LPAPLUINT         lpMemAxisHead = NULL, // Ptr to axis values, fleshed out by CheckAxis_EM
                      lpMemAxisTail = NULL; // Ptr to grade up of AxisHead

    // Loop through the right arg
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    {
        // Split cases based upon the ptr type bits
        switch (GetPtrTypeDir (lpMemRht[uRht]))
        {
            case PTRTYPE_STCONST:
                // Save the identity element
                *lpMemRes++ =
                  MakeSymEntry_EM (lpMemRht[uRht]->stFlags.ImmType,     // Immediate type
                                  &lpMemRht[uRht]->stData.stLongest,    // Ptr to immediate value
                                   lptkFunc);                           // Ptr to function token
                break;

            case PTRTYPE_HGLOBAL:
                // Get the item's global memory handle
                hGlbItm = lpMemRht[uRht];

                // Lock the memory to get a ptr to it
                lpMemHdrItm = MyGlobalLockVar (hGlbItm);

                // Get the item's attributes
#define lpHeader        lpMemHdrItm
                aplTypeItm = lpHeader->ArrType;
                aplNELMItm = lpHeader->NELM;
                aplRankItm = lpHeader->Rank;
#undef  lpHeader
                // Check for axis present
                if (lptkAxis NE NULL)
                {
                    // Check the axis values, fill in # elements in axis
                    if (!CheckAxis_EM (lptkAxis,        // The axis token
                                       aplRankItm,      // All values less than this
                                       FALSE,           // TRUE iff scalar or one-element vector only
                                       FALSE,           // TRUE iff want sorted axes
                                       FALSE,           // TRUE iff axes must be contiguous
                                       FALSE,           // TRUE iff duplicate axes are allowed
                                       NULL,            // TRUE iff fractional values allowed
                                       NULL,            // Return last axis value
                                      &aplNELMAxis,     // Return # elements in axis vector
                                      &hGlbAxis))       // Return HGLOBAL with APLINT axis values
                        goto ERROR_EXIT;

                    // Lock the memory to get a ptr to it
                    lpMemAxisHead = MyGlobalLockInt (hGlbAxis);

                    // Get pointer to the axis tail (where the [X] values are)
                    lpMemAxisTail = &lpMemAxisHead[aplRankItm - aplNELMAxis];
                } else
                    // No axis is the same as all axes
                    aplNELMAxis = aplRankItm;

                // The result2 rank is the NELM axis
                aplRankRes2 = aplNELMAxis;

                // Calculate the storage type of the result2
                aplTypeRes2 = (*lpPrimSpec->StorageTypeDyd) (aplNELMItm,
                                                            &aplTypeItm,
                                                             lptkFunc,
                                                             aplNELMItm,
                                                            &aplTypeItm);
                if (IsErrorType (aplTypeRes2))
                    goto DOMAIN_EXIT;

                Assert (IsSimpleNum (aplTypeRes2)
                     || IsNested (aplTypeRes2));

                // If the result2 is simple numeric, ...
                if (IsSimpleNum (aplTypeRes2))
                {
                    // If the identity element is simple Boolean, ...
                    if (lpPrimIdent->IsBool)
                        // Make it Boolean
                        aplTypeRes2 = ARRAY_BOOL;
                    else
                        // Make it float
                        aplTypeRes2 = ARRAY_FLOAT;
                } // End IF

                // Skip over the header to the dimensions
                lpMemDimItm = VarArrayBaseToDim (lpMemHdrItm);

                //***************************************************************
                //  Calculate result2 NELM
                //***************************************************************
                if (lpMemAxisTail NE NULL)
                    for (aplNELMRes2 = 1, uRht = 0; uRht < aplRankRes2; uRht++)
                        aplNELMRes2 *= lpMemDimItm[lpMemAxisTail[uRht]];
                else
                    aplNELMRes2 = aplNELMItm;

                //***************************************************************
                // Calculate space needed for the result2
                //***************************************************************
                ByteRes2 = CalcArraySize (aplTypeRes2, aplNELMRes2, aplRankRes2);

                //***************************************************************
                // Check for overflow
                //***************************************************************
                if (ByteRes2 NE (APLU3264) ByteRes2)
                    goto WSFULL_EXIT;

                //***************************************************************
                // Now we can allocate the storage for the result
                //***************************************************************
                hGlbRes2 = DbgGlobalAlloc (GHND, (APLU3264) ByteRes2);
                if (hGlbRes2 EQ NULL)
                    goto WSFULL_EXIT;

                // Lock the memory to get a ptr to it
                lpMemHdrRes2 = MyGlobalLock000 (hGlbRes2);

#define lpHeader        lpMemHdrRes2
                // Fill in the header
                lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
                lpHeader->ArrType    = aplTypeRes2;
////////////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
                lpHeader->RefCnt     = 1;
                lpHeader->NELM       = aplNELMRes2;
                lpHeader->Rank       = aplRankRes2;
#undef  lpHeader

                // Skip over the header to the dimensions
                lpMemDimRes2 = VarArrayBaseToDim (lpMemHdrRes2);

                //***************************************************************
                // Fill in the result's dimension
                //***************************************************************
                if (lpMemAxisTail NE NULL)
                    for (uRht = 0; uRht < aplRankRes2; uRht++)
                        *lpMemDimRes2++ = lpMemDimItm[lpMemAxisTail[uRht]];
                else
                    CopyMemory (lpMemDimRes2, lpMemDimItm, (APLU3264) aplRankRes2 * sizeof (APLDIM));

                // Skip over the header and dimensions to the data
                lpMemItm  = VarArrayDataFmBase (lpMemHdrItm );
                lpMemRes2 = VarArrayDataFmBase (lpMemHdrRes2);

                // If the item (and result2) is simple (numeric), ...
                if (IsSimpleNum (aplTypeItm))
                {
                    // Fill in the result data
                    if (lpPrimIdent->IsBool)
                    {
                        Assert (IsSimpleBool (aplTypeRes2));

                        if (lpPrimIdent->bIdentElem)
                            FillMemory (lpMemRes2, (APLU3264) RoundUpBitsToBytes (aplNELMRes2), 0xFF);
                    } else
                    {
                        Assert (IsSimpleFlt (aplTypeRes2));

                        for (uRht = 0; uRht < aplNELMRes2; uRht++)
                            *((LPAPLFLOAT) lpMemRes2)++ = lpPrimIdent->fIdentElem;
                    } // End IF/ELSE
                } else
                {
                    // The result is nested
                    Assert (IsNested (aplTypeRes2));

                    // Handle nested prototypes
                    aplNELMItm = max (aplNELMItm, 1);

                    // Recurse into the array
                    if (!PrimIdentFnScalarCommon_EM (lpMemItm,          // Ptr to item global memory data
                                                     lpMemRes2,         // ...    result2 ...
                                                     aplNELMItm,        // Item NELM
                                                     lpPrimIdent,       // Ptr to function PrimIdent entry
                                                     lpPrimSpec,        // Ptr to function PRIMSPEC
                                                     lptkAxis,          // Ptr to axis token
                                                     lptkFunc))         // Ptr to function token
                        goto ERROR_EXIT;
                } // End IF/ELSE

                // We no longer need these ptrs
                MyGlobalUnlock (hGlbItm);  lpMemHdrItm  = NULL;
                MyGlobalUnlock (hGlbRes2); lpMemHdrRes2 = NULL;

                // Save in the result
                *lpMemRes++ = MakePtrTypeGlb (hGlbRes2);

                break;

            defstop
                break;
        } // End SWITCH
    } // End FOR

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes2 NE NULL)
    {
        if (lpMemHdrRes2 NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes2); lpMemHdrRes2 = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes2); hGlbRes2 = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbRes2 NE NULL && lpMemHdrRes2 NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes2); lpMemHdrRes2 = NULL;
    } // End IF

    if (hGlbItm NE NULL && lpMemHdrItm NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbItm); lpMemHdrItm = NULL;
    } // End IF

    if (hGlbAxis NE NULL && lpMemAxisTail NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbAxis); lpMemAxisHead = lpMemAxisTail = NULL;
    } // End IF

    return bRet;
} // End PrimIdentFnScalarCommon_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMon_EM_YY
//
//  Primitive scalar monadic function
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMon_EM_YY
    (LPTOKEN    lptkFunc,           // Ptr to function token
     LPTOKEN    lptkRhtArg,         // Ptr to right arg token
     LPTOKEN    lptkAxis,           // Ptr to axis token (may be NULL)
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    HGLOBAL      hGlbRes;           // Result global memory handle
    APLSTYPE     aplTypeRes,        // Result storage type
                 aplTypeRht;        // Right arg storage type
    APLRANK      aplRankRht;        // Right arg rank
    APLNELM      aplNELMRht;        // Right arg NELM
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result
    PRIMSPEC     LclPrimSpec;       // Writable copy of PRIMSPEC
    APLVFP       mpfArg = {0},      // VFP arg
                 mpfRes = {0};      // VFP result

    // Check for axis present
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Save a writable copy of PRIMSPEC
    LclPrimSpec = *lpPrimSpec;

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Handle prototypes separately
    if (IsEmpty (aplNELMRht))
        return PrimProtoFnScalar_EM_YY (NULL,       // Ptr to left arg token
                                        lptkFunc,   // Ptr to function token
                                        lptkRhtArg, // Ptr to right arg token
                                        lptkAxis);  // Ptr to axis token (may be NULL)
    // Get the storage type of the result
    aplTypeRes = (*lpPrimSpec->StorageTypeMon) (aplNELMRht,
                                               &aplTypeRht,
                                                lptkFunc);
    // Check for DOMAIN ERROR
    if (IsErrorType (aplTypeRes))
        goto DOMAIN_EXIT;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Split cases based upon the right arg's token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must trundle through the array
            if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
            {
                // Get the global memory handle
                hGlbRes = lptkRhtArg->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir_PTB (hGlbRes));

                // In order to make roll atomic, save the current []RL into LclPrimSpec
                SavePrimSpecRL (&LclPrimSpec);

                // Handle via subroutine
                hGlbRes = PrimFnMonGlb_EM (hGlbRes,
                                           lptkFunc,
                                          &LclPrimSpec);
                if (hGlbRes EQ NULL)
                {
                    YYFree (lpYYRes); lpYYRes = NULL;

                    return NULL;
                } // End IF

                // Restore the value of []RL from LclPrimSpec
                RestPrimSpecRL (&LclPrimSpec);

                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkGlbData  = hGlbRes;
                lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

                return lpYYRes;
            } // End IF
RESTART_EXCEPTION_VARNAMED:
            // Handle the immediate case

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
            lpYYRes->tkToken.tkFlags.ImmType   = TranslateArrayTypeToImmType (aplTypeRes);
////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
////////////lpYYRes->tkToken.tkData            =        // Filled in below
            lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

            // In order to make roll atomic, save the current []RL into LclPrimSpec
            SavePrimSpecRL (&LclPrimSpec);

            __try
            {
                // Split cases based upon the result storage type
                switch (aplTypeRes)
                {
                    case ARRAY_BOOL:            // Res = BOOL
                        // Split cases based upon the right arg's storage type
                        switch (lptkRhtArg->tkData.tkSym->stFlags.ImmType)
                        {
                            case IMMTYPE_BOOL:  // Res = BOOL, Rht = BOOL
                                lpYYRes->tkToken.tkData.tkBoolean  =
                                  (*lpPrimSpec->BisB) (lptkRhtArg->tkData.tkSym->stData.stBoolean & BIT0,
                                                      &LclPrimSpec);
                                break;

                            case IMMTYPE_INT:   // Res = BOOL, Rht = INT
                                lpYYRes->tkToken.tkData.tkBoolean  =
                                  (*lpPrimSpec->BisI) (lptkRhtArg->tkData.tkSym->stData.stInteger,
                                                      &LclPrimSpec);
                                break;

                            case IMMTYPE_FLOAT: // Res = BOOL, Rht = FLOAT
                                lpYYRes->tkToken.tkData.tkBoolean  =
                                  (*lpPrimSpec->BisF) (lptkRhtArg->tkData.tkSym->stData.stFloat,
                                                      &LclPrimSpec);
                                break;

                            defstop
                                return NULL;
                        } // End SWITCH

                        break;

                    case ARRAY_INT:             // Res = INT
                        // Split cases based upon the right arg's storage type
                        switch (lptkRhtArg->tkData.tkSym->stFlags.ImmType)
                        {
                            case IMMTYPE_BOOL:  // Res = INT, Rht = BOOL
                                lpYYRes->tkToken.tkData.tkInteger  =
                                  (*lpPrimSpec->IisI) (lptkRhtArg->tkData.tkSym->stData.stBoolean & BIT0,
                                                      &LclPrimSpec);
                                break;

                            case IMMTYPE_INT:   // Res = INT, Rht = INT
                                lpYYRes->tkToken.tkData.tkInteger  =
                                  (*lpPrimSpec->IisI) (lptkRhtArg->tkData.tkSym->stData.stInteger,
                                                      &LclPrimSpec);
                                break;

                            case IMMTYPE_FLOAT: // Res = INT, Rht = FLOAT
                                lpYYRes->tkToken.tkData.tkInteger  =
                                  (*lpPrimSpec->IisF) (lptkRhtArg->tkData.tkSym->stData.stFloat,
                                                      &LclPrimSpec);
                                break;

                            defstop
                                return NULL;
                        } // End SWITCH

                        break;

                    case ARRAY_FLOAT:           // Res = FLOAT
                        // Split cases based upon the right arg's storage type
                        switch (lptkRhtArg->tkData.tkSym->stFlags.ImmType)
                        {
                            case IMMTYPE_BOOL:  // Res = FLOAT, Rht = BOOL
                                lpYYRes->tkToken.tkData.tkFloat  =
                                  (*lpPrimSpec->FisI) (lptkRhtArg->tkData.tkSym->stData.stBoolean & BIT0,
                                                      &LclPrimSpec);
                                break;

                            case IMMTYPE_INT:   // Res = FLOAT, Rht = INT
                                lpYYRes->tkToken.tkData.tkFloat  =
                                  (*lpPrimSpec->FisI) (lptkRhtArg->tkData.tkSym->stData.stInteger,
                                                      &LclPrimSpec);
                                break;

                            case IMMTYPE_FLOAT: // Res = FLOAT, Rht = FLOAT
                                lpYYRes->tkToken.tkData.tkFloat  =
                                  (*lpPrimSpec->FisF) (lptkRhtArg->tkData.tkSym->stData.stFloat,
                                                      &LclPrimSpec);
                                break;

                            defstop
                                return NULL;
                        } // End SWITCH

                        break;

                    defstop
                        return NULL;
                } // SWITCH
            } __except (CheckException (GetExceptionInformation (), L"PrimFnMon_EM_YY #1"))
            {
                EXCEPTION_CODES ExceptionCode = MyGetExceptionCode ();  // The exception code

                dprintfWL9 (L"!!Initiating Exception in " APPEND_NAME L" #1: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                // Split cases based upon the ExceptionCode
                switch (ExceptionCode)
                {
                    case EXCEPTION_DOMAIN_ERROR:
                    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                    case EXCEPTION_INT_DIVIDE_BY_ZERO:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        YYFree (lpYYRes); lpYYRes = NULL;

                        goto DOMAIN_EXIT;

                    case EXCEPTION_WS_FULL:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        goto WSFULL_EXIT;

                    case EXCEPTION_NONCE_ERROR:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        YYFree (lpYYRes); lpYYRes = NULL;

                        goto NONCE_EXIT;

                    case EXCEPTION_RESULT_RAT:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        if (IsNumeric (aplTypeRes)
                         && !IsRat    (aplTypeRes))
                        {
                            // It's now a RAT result
                            aplTypeRes = ARRAY_RAT;

                            dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #1: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                            goto RESTART_EXCEPTION_VARNAMED;
                        } // End IF

                        // Display message for unhandled exception
                        DisplayException ();

                        break;

                    case EXCEPTION_RESULT_VFP:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        if (IsNumeric (aplTypeRes)
                         && !IsVfp    (aplTypeRes))
                        {
                            // It's now a VFP result
                            aplTypeRes = ARRAY_VFP;

                            dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #1: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                            goto RESTART_EXCEPTION_VARNAMED;
                        } // End IF

                        // Display message for unhandled exception
                        DisplayException ();

                        break;

                    case EXCEPTION_RESULT_FLOAT:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        // Because the right arg is immediate, it can't be global numeric
                        Assert (!IsGlbNum (aplTypeRes));

                        if (IsSimpleNum (aplTypeRes)
                         && !IsSimpleFlt (aplTypeRes))
                        {
                            // It's now a FLOAT result
                            aplTypeRes = ARRAY_FLOAT;

                            dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #1: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                            goto RESTART_EXCEPTION_VARNAMED;
                        } // End IF

                        // Fall through to never-never-land

                    default:
                        // Display message for unhandled exception
                        DisplayException ();

                        break;
                } // End SWITCH
            } // End __try/__except

            // Restore the value of []RL from LclPrimSpec
            RestPrimSpecRL (&LclPrimSpec);

            return lpYYRes;

        case TKT_VARIMMED:
RESTART_EXCEPTION_VARIMMED:
            // In order to make roll atomic, save the current []RL into LclPrimSpec
            SavePrimSpecRL (&LclPrimSpec);

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
            lpYYRes->tkToken.tkFlags.ImmType   = TranslateArrayTypeToImmType (aplTypeRes);
////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
////////////lpYYRes->tkToken.tkData            = (Filled in above)
            lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

            __try
            {
                // Split cases based upon the result storage type
                switch (aplTypeRes)
                {
                    case ARRAY_BOOL:            // Res = BOOL
                        // Split cases based upon the right arg's storage type
                        switch (lptkRhtArg->tkFlags.ImmType)
                        {
                            case IMMTYPE_BOOL:  // Res = BOOL, Rht = BOOL
                                lpYYRes->tkToken.tkData.tkBoolean =
                                  (*lpPrimSpec->BisB) (lptkRhtArg->tkData.tkBoolean & BIT0,
                                                      &LclPrimSpec);
                                break;

                            case IMMTYPE_INT:   // Res = BOOL, Rht = INT
                                lpYYRes->tkToken.tkData.tkBoolean =
                                  (*lpPrimSpec->BisI) (lptkRhtArg->tkData.tkInteger,
                                                      &LclPrimSpec);
                                break;

                            case IMMTYPE_FLOAT: // Res = BOOL, Rht = FLOAT
                                lpYYRes->tkToken.tkData.tkBoolean =
                                  (*lpPrimSpec->BisF) (lptkRhtArg->tkData.tkFloat,
                                                      &LclPrimSpec);
                                break;

                            defstop
                                return NULL;
                        } // End SWITCH

                        break;

                    case ARRAY_INT:             // Res = INT
                        // Split cases based upon the right arg's storage type
                        switch (lptkRhtArg->tkFlags.ImmType)
                        {
                            case IMMTYPE_BOOL:  // Res = INT, Rht = BOOL
                                lpYYRes->tkToken.tkData.tkInteger =
                                  (*lpPrimSpec->IisI) (lptkRhtArg->tkData.tkBoolean & BIT0,
                                                      &LclPrimSpec);
                                break;

                            case IMMTYPE_INT:   // Res = INT, Rht = INT
                                lpYYRes->tkToken.tkData.tkInteger =
                                  (*lpPrimSpec->IisI) (lptkRhtArg->tkData.tkInteger,
                                                      &LclPrimSpec);
                                break;

                            case IMMTYPE_FLOAT: // Res = INT, Rht = FLOAT
                                lpYYRes->tkToken.tkData.tkInteger =
                                  (*lpPrimSpec->IisF) (lptkRhtArg->tkData.tkFloat,
                                                      &LclPrimSpec);
                                break;

                            defstop
                                return NULL;
                        } // End SWITCH

                        break;

                    case ARRAY_FLOAT:           // Res = FLOAT
                        // Split cases based upon the right arg's storage type
                        switch (lptkRhtArg->tkFlags.ImmType)
                        {
                            case IMMTYPE_BOOL:  // Res = FLOAT, Rht = BOOL
                                lpYYRes->tkToken.tkData.tkFloat   =
                                  (*lpPrimSpec->FisI) (lptkRhtArg->tkData.tkBoolean & BIT0,
                                                      &LclPrimSpec);
                                break;

                            case IMMTYPE_INT:   // Res = FLOAT, Rht = INT
                                lpYYRes->tkToken.tkData.tkFloat   =
                                  (*lpPrimSpec->FisI) (lptkRhtArg->tkData.tkInteger,
                                                      &LclPrimSpec);
                                break;

                            case IMMTYPE_FLOAT: // Res = FLOAT, Rht = FLOAT
                                lpYYRes->tkToken.tkData.tkFloat   =
                                  (*lpPrimSpec->FisF) (lptkRhtArg->tkData.tkFloat,
                                                      &LclPrimSpec);
                                break;

                            defstop
                                return NULL;
                        } // End SWITCH

                        break;

                case ARRAY_VFP:                 // Res = VFP
                        // Initialize the arg
                        mpfr_init0 (&mpfArg);

                        // Split cases based upon the right arg's storage type
                        switch (lptkRhtArg->tkFlags.ImmType)
                        {
                            case IMMTYPE_BOOL:  // Res = VFP, Rht = BOOL
                                mpfr_set_sx (&mpfArg, lptkRhtArg->tkData.tkBoolean & BIT0, MPFR_RNDN);

                                break;

                            case IMMTYPE_INT:   // Res = VFP, Rht = INT
                                mpfr_set_sx (&mpfArg, lptkRhtArg->tkData.tkBoolean, MPFR_RNDN);

                                break;

                            case IMMTYPE_FLOAT: // Res = VFP, Rht = FLOAT
                                mpfr_set_d  (&mpfArg, lptkRhtArg->tkData.tkFloat, MPFR_RNDN);

                                break;

                            defstop
                                return NULL;
                        } // End SWITCH

                        // Calculate the result
                        mpfRes =  (*lpPrimSpec->VisV) (mpfArg,
                                                      &LclPrimSpec);
                        // Save in the result
                        lpYYRes->tkToken.tkFlags.TknType  = TKT_VARARRAY;
                        lpYYRes->tkToken.tkFlags.ImmType  = IMMTYPE_VFP;
                        lpYYRes->tkToken.tkData.tkGlbData =
                          MakeGlbEntry_EM (ARRAY_VFP,   // Entry type
                                          &mpfRes,      // Ptr to the value
                                           FALSE,       // TRUE iff we should initialize the target first
                                           lptkFunc);   // Ptr to function token
                        // We no longer need this storage
                        Myf_clear (&mpfArg);

                        // Check the result
                        if (lpYYRes->tkToken.tkData.tkGlbData EQ NULL)
                            RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
                        break;

                    defstop
                        return NULL;
                } // End SWITCH
            } __except (CheckException (GetExceptionInformation (), L"PrimFnMon_EM_YY #2"))
            {
                EXCEPTION_CODES ExceptionCode = MyGetExceptionCode ();  // The exception code

                dprintfWL9 (L"!!Initiating Exception in " APPEND_NAME L" #2: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                // Split cases based upon the ExceptionCode
                switch (ExceptionCode)
                {
                    case EXCEPTION_DOMAIN_ERROR:
                    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                    case EXCEPTION_INT_DIVIDE_BY_ZERO:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        YYFree (lpYYRes); lpYYRes = NULL;

                        goto DOMAIN_EXIT;

                    case EXCEPTION_WS_FULL:
                        goto WSFULL_EXIT;

                    case EXCEPTION_NONCE_ERROR:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        YYFree (lpYYRes); lpYYRes = NULL;

                        goto NONCE_EXIT;

                    case EXCEPTION_RESULT_RAT:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        if (IsNumeric (aplTypeRes)
                         && !IsRat    (aplTypeRes))
                        {
                            // It's now a RAT result
                            aplTypeRes = ARRAY_RAT;

                            dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #2: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                            goto RESTART_EXCEPTION_VARIMMED;
                        } // End IF

                        // Display message for unhandled exception
                        DisplayException ();

                        break;

                    case EXCEPTION_RESULT_VFP:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        if (IsNumeric (aplTypeRes)
                         && !IsVfp    (aplTypeRes))
                        {
                            // It's now a VFP result
                            aplTypeRes = ARRAY_VFP;

                            dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #2: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                            goto RESTART_EXCEPTION_VARIMMED;
                        } // End IF

                        // Display message for unhandled exception
                        DisplayException ();

                        break;

                    case EXCEPTION_RESULT_FLOAT:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        // Because the right arg is immediate, it can't be global numeric
                        Assert (!IsGlbNum (aplTypeRes));

                        if (IsSimpleNum (aplTypeRes)
                         && !IsSimpleFlt (aplTypeRes))
                        {
                            // It's now a FLOAT result
                            aplTypeRes = ARRAY_FLOAT;

                            dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #2: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                            goto RESTART_EXCEPTION_VARIMMED;
                        } // End IF

                        // Fall through to never-never-land

                    default:
                        // Display message for unhandled exception
                        DisplayException ();

                        break;
                } // End SWITCH
            } // End __try/__except

            // Restore the value of []RL from LclPrimSpec
            RestPrimSpecRL (&LclPrimSpec);

            return lpYYRes;

        case TKT_VARARRAY:
            // Get the global memory handle
            hGlbRes = lptkRhtArg->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (hGlbRes));

            // In order to make roll atomic, save the current []RL into LclPrimSpec
            SavePrimSpecRL (&LclPrimSpec);

            // Handle via subroutine
            hGlbRes = PrimFnMonGlb_EM (hGlbRes,
                                       lptkFunc,
                                      &LclPrimSpec);
            if (hGlbRes EQ NULL)
            {
                YYFree (lpYYRes); lpYYRes = NULL;

                return NULL;
            } // End IF

            // Restore the value of []RL from LclPrimSpec
            RestPrimSpecRL (&LclPrimSpec);

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkGlbData  = hGlbRes;
            lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

            return lpYYRes;

        defstop
            break;
    } // End SWITCH

    DbgStop ();         // We should never get here

    YYFree (lpYYRes); lpYYRes = NULL;

    return NULL;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    return NULL;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFnMon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonGlb_EM
//
//  Primitive scalar monadic function on a global memory object
//  Returning an HGLOBAL with the ptr type bits significant
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonGlb_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL PrimFnMonGlb_EM
    (HGLOBAL    hGlbRht,                    // Right arg handle
     LPTOKEN    lptkFunc,                   // Ptr to function token
     LPPRIMSPEC lpPrimSpec)                 // Ptr to local PRIMSPEC

{
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg header
                      lpMemHdrRes = NULL;   // ...    result    ...
    LPVOID            lpMemRht,             // Ptr to right arg global memory
                      lpMemRes;             // Ptr to result    ...
    HGLOBAL           hGlbRes = NULL,       // Result global memory handle
                      hGlbSub;              // Subarray ...
    APLSTYPE          aplTypeRht,           // Right arg storage type
                      aplTypeRes;           // Result    ...
    APLNELM           aplNELMRht,           // # elements in the array
                      aplNELMTmp,           // Temporary NELM
                      aplNELMRem;           // Remaining NELM
    APLRANK           aplRankRht;           // The rank of the array
    APLINT            uRes,                 // Result loop counter
                      apaOffRht,            // Right arg APA offset
                      apaMulRht;            // ...           multiplier
    APLUINT           ByteRes;              // # bytes in the result
    UBOOL             bRet = TRUE;          // TRUE iff result is valid
    UINT              uBitIndex;            // Bit index when marching through Booleans
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    APLVFP            mpfRes = {0};         // VFP result

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Lock the memory to get a ptr to it
    lpMemHdrRht = MyGlobalLockVar (hGlbRht);

#define lpHeader    lpMemHdrRht
    // Get the Array Type, NELM, and Rank
    aplTypeRht = lpHeader->ArrType;
    aplNELMRht = lpHeader->NELM;
    aplRankRht = lpHeader->Rank;

    // Get the storage type of the result
    aplTypeRes = (*lpPrimSpec->StorageTypeMon) (aplNELMRht,
                                               &aplTypeRht,
                                                lptkFunc);
    // In case StorageTypeMon changed the value of aplTypeRht,
    //   save it back into the array
    lpHeader->ArrType = aplTypeRht;
#undef  lpHeader

    // Check for DOMAIN ERROR
    if (IsErrorType (aplTypeRes))
        goto DOMAIN_EXIT;

    // In case the result is APA
    if (IsSimpleAPA (aplTypeRes))
    {
        __try
        {
            if (!(*lpPrimSpec->ApaResultMon_EM) (NULL,
                                                 lptkFunc,
                                                 hGlbRht,
                                                &hGlbRes,
                                                 aplRankRht,
                                                 lpPrimSpec))
                goto ERROR_EXIT;
            else
                goto NORMAL_EXIT;
        } __except (CheckException (GetExceptionInformation (), L"PrimFnMonGlb_EM"))
        {
            EXCEPTION_CODES ExceptionCode = MyGetExceptionCode ();  // The exception code

            dprintfWL9 (L"!!Initiating Exception in " APPEND_NAME L" #3: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

            // Split cases based upon the ExceptionCode
            switch (ExceptionCode)
            {
                case EXCEPTION_DOMAIN_ERROR:
                case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                case EXCEPTION_INT_DIVIDE_BY_ZERO:
                    MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                    goto DOMAIN_EXIT;

                case EXCEPTION_WS_FULL:
                    goto WSFULL_EXIT;

                case EXCEPTION_NONCE_ERROR:
                    MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                    goto NONCE_EXIT;

                case EXCEPTION_RESULT_RAT:
                    MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                    if (IsNumeric (aplTypeRes)
                     && !IsRat    (aplTypeRes))
                    {
                        // It's now a RAT result
                        aplTypeRes = ARRAY_RAT;

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

                        // Lock the memory to get a ptr to it
                        lpMemHdrRht = MyGlobalLockVar (hGlbRht);

                        if (hGlbRes NE NULL)
                        {
                            // We no longer need this ptr
                            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

                            // We no longer need this storage
                            FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
                        } // End IF

                        dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #3: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                        goto RESTART_EXCEPTION;
                    } // End IF

                    // Display message for unhandled exception
                    DisplayException ();

                    break;

                case EXCEPTION_RESULT_VFP:
                    MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                    if (IsNumeric (aplTypeRes)
                     && !IsVfp    (aplTypeRes))
                    {
                        // It's now a VFP result
                        aplTypeRes = ARRAY_VFP;

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

                        // Lock the memory to get a ptr to it
                        lpMemHdrRht = MyGlobalLockVar (hGlbRht);

                        if (hGlbRes NE NULL)
                        {
                            // We no longer need this ptr
                            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

                            // We no longer need this storage
                            FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
                        } // End IF

                        dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #3: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                        goto RESTART_EXCEPTION;
                    } // End IF

                    // Display message for unhandled exception
                    DisplayException ();

                    break;

                case EXCEPTION_RESULT_FLOAT:
                    MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                    // Because the right arg is immediate, it can't be global numeric
                    Assert (!IsGlbNum (aplTypeRes));

                    if (IsSimpleNum (aplTypeRes)
                     && !IsSimpleFlt (aplTypeRes))
                    {
                        // It's now a FLOAT result
                        aplTypeRes = ARRAY_FLOAT;

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

                        // Lock the memory to get a ptr to it
                        lpMemHdrRht = MyGlobalLockVar (hGlbRht);

                        if (hGlbRes NE NULL)
                        {
                            // We no longer need this ptr
                            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

                            // We no longer need this storage
                            FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
                        } // End IF

                        dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #3: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                        goto RESTART_EXCEPTION;
                    } // End IF

                    // Fall through to never-never-land

                default:
                    // Display message for unhandled exception
                    DisplayException ();

                    break;
            } // End SWITCH
        } // End __try/__except
    } // End IF
RESTART_EXCEPTION:
    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRht, aplRankRht);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result.
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader    lpMemHdrRes
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRht;
    lpHeader->Rank       = aplRankRht;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemHdrRes);
    lpMemRht = VarArrayBaseToDim (lpMemHdrRht);

    //***************************************************************
    // Copy the dimensions from the right arg
    //   to the result's dimension
    //***************************************************************
    for (uRes = 0; uRes < (APLRANKSIGN) aplRankRht; uRes++)
        *((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemRht)++;

    // lpMemRes now points to the result's data
    // lpMemRht now points to the right arg's data

    // If the right arg is an APA, ...
    if (IsSimpleAPA (aplTypeRht))
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        // Get the APA parameters
        apaOffRht = lpAPA->Off;
        apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // In case the right arg is Boolean
    uBitIndex = 0;

    __try
    {
        // Split cases based upon the storage type of the result
        switch (aplTypeRes)
        {
            case ARRAY_BOOL:            // Res = BOOL
                // Split cases based upon the right arg's storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:    // Res = BOOL, Rht = BOOL
                        // Initialize # remaining NELM
                        aplNELMRem = aplNELMRht;

                        // Check for optimized chunking
                        if (lpPrimSpec->B64isB64 NE NULL)
                        {
                            // Calculate the # 64-bit chunks
                            aplNELMTmp  = aplNELMRem / 64;
                            aplNELMRem -= aplNELMTmp * 64;

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB64) lpMemRes)++ =
                                  (*lpPrimSpec->B64isB64) (*((LPAPLB64) lpMemRht)++, lpPrimSpec);
                            } // End FOR

                            // Calculate the # remaining 32-bit chunks
                            aplNELMTmp  = aplNELMRem / 32;
                            aplNELMRem -= aplNELMTmp * 32;

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB32) lpMemRes)++ =
                                  (*lpPrimSpec->B32isB32) (*((LPAPLB32) lpMemRht)++, lpPrimSpec);
                            } // End FOR

                            // Calculate the # remaining 16-bit chunks
                            aplNELMTmp  = aplNELMRem / 16;
                            aplNELMRem -= aplNELMTmp * 16;

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB16) lpMemRes)++ =
                                  (*lpPrimSpec->B16isB16) (*((LPAPLB16) lpMemRht)++, lpPrimSpec);
                            } // End FOR

                            // Calculate the # remaining  8-bit chunks
                            aplNELMTmp  = aplNELMRem /  8;
                            aplNELMRem -= aplNELMTmp *  8;

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB08) lpMemRes)++ =
                                  (*lpPrimSpec->B08isB08) (*((LPAPLB08) lpMemRht)++, lpPrimSpec);
                            } // End FOR
                        } // End IF

                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRem; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLBOOL) lpMemRes) |=
                              (*lpPrimSpec->BisB) ((APLBOOL) (BIT0 & ((*(LPAPLBOOL) lpMemRht) >> uBitIndex)),
                                                   lpPrimSpec) << uBitIndex;
                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;              // Start over
                                ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                                ((LPAPLBOOL) lpMemRes)++;   // ...
                            } // End IF
                        } // End FOR

                        break;

                    case ARRAY_INT:     // Res = BOOL, Rht = INT
                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLBOOL) lpMemRes) |=
                              (*lpPrimSpec->BisI) (*((LPAPLINT) lpMemRht)++,
                                                   lpPrimSpec) << uBitIndex;

                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;              // Start over
                                ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                            } // End IF
                        } // End FOR

                        break;

                    case ARRAY_APA:     // Res = BOOL, Rht = APA
                        // Initialize # remaining NELM
                        aplNELMRem = aplNELMRht;

                        // Check for Boolean APA and optimized chunking
#define lpAPA       ((LPAPLAPA) lpMemRht)
                        if (IsBooleanAPA (lpAPA)
                         && lpPrimSpec->B64isB64)
                        {
                            APLB64 aplB64APA;

                            if (lpAPA->Off NE 0)
                                aplB64APA = 0xFFFFFFFFFFFFFFFF;
                            else
                                aplB64APA = 0x0000000000000000;
#undef  lpAPA
                            // Calculate the # 64-bit chunks
                            aplNELMTmp  = aplNELMRem / 64;
                            aplNELMRem -= aplNELMTmp * 64;

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB64) lpMemRes)++ =
                                  (*lpPrimSpec->B64isB64) ((APLB64) aplB64APA, lpPrimSpec);
                            } // End FOR

                            // Calculate the # remaining 32-bit chunks
                            aplNELMTmp  = aplNELMRem / 32;
                            aplNELMRem -= aplNELMTmp * 32;

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB32) lpMemRes)++ =
                                  (*lpPrimSpec->B32isB32) ((APLB32) aplB64APA, lpPrimSpec);
                            } // End FOR

                            // Calculate the # remaining 16-bit chunks
                            aplNELMTmp  = aplNELMRem / 16;
                            aplNELMRem -= aplNELMTmp * 16;

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB16) lpMemRes)++ =
                                  (*lpPrimSpec->B16isB16) ((APLB16) aplB64APA, lpPrimSpec);
                            } // End FOR

                            // Calculate the # remaining  8-bit chunks
                            aplNELMTmp  = aplNELMRem /  8;
                            aplNELMRem -= aplNELMTmp *  8;

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB08) lpMemRes)++ =
                                  (*lpPrimSpec->B08isB08) ((APLB08) aplB64APA, lpPrimSpec);
                            } // End FOR
                        } // End IF

                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRem; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLBOOL) lpMemRes) |=
                              (*lpPrimSpec->BisI) (apaOffRht + apaMulRht * uRes,
                                                   lpPrimSpec) << uBitIndex;

                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;              // Start over
                                ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                            } // End IF
                        } // End FOR

                        break;

                    case ARRAY_FLOAT:   // Res = BOOL, Rht = FLOAT
                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLBOOL) lpMemRes) |=
                              (*lpPrimSpec->BisF) (*((LPAPLFLOAT) lpMemRht)++,
                                                   lpPrimSpec) << uBitIndex;

                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;              // Start over
                                ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                            } // End IF
                        } // End FOR

                        break;

                    defstop
                        break;
                } // End FOR/SWITCH

                break;

            case ARRAY_INT:             // Res = INT
                // Split cases based upon the right arg's storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:    // Res = INT, Rht = BOOL
                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLINT) lpMemRes)++ =
                              (*lpPrimSpec->IisI) (BIT0 & ((*(LPAPLBOOL) lpMemRht) >> uBitIndex),
                                                   lpPrimSpec);

                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;              // Start over
                                ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                            } // End IF
                        } // End FOR

                        break;

                    case ARRAY_INT:     // Res = INT, Rht = INT
                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLINT) lpMemRes)++ =
                              (*lpPrimSpec->IisI) (*((LPAPLINT) lpMemRht)++,
                                                   lpPrimSpec);
                        } // End FOR

                        break;

                    case ARRAY_FLOAT:   // Res = INT, Rht = FLOAT
                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLINT) lpMemRes)++ =
                              (*lpPrimSpec->IisF) (*((LPAPLFLOAT) lpMemRht)++,
                                                   lpPrimSpec);
                        } // End FOR

                        break;

                    case ARRAY_APA:     // Res = INT, Rht = APA
                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLINT) lpMemRes)++ =
                              (*lpPrimSpec->IisI) (apaOffRht + apaMulRht * uRes,
                                                   lpPrimSpec);
                        } // End FOR

                        break;

                    defstop
                        break;
                } // End FOR/SWITCH

                break;

            case ARRAY_FLOAT:           // Res = FLOAT
                // Split cases based upon the right arg's storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:    // Res = FLOAT, Rht = BOOL
                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLFLOAT) lpMemRes)++ =
                              (*lpPrimSpec->FisI) (BIT0 & ((*(LPAPLBOOL) lpMemRht) >> uBitIndex),
                                                   lpPrimSpec);
                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;              // Start over
                                ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                            } // End IF
                        } // End FOR

                        break;

                    case ARRAY_INT:     // Res = FLOAT, Rht = INT
                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLFLOAT) lpMemRes)++ =
                              (*lpPrimSpec->FisI) (*((LPAPLINT) lpMemRht)++,
                                                   lpPrimSpec);
                        } // End FOR

                        break;

                    case ARRAY_APA:     // Res = FLOAT, Rht = APA
                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLFLOAT) lpMemRes)++ =
                              (*lpPrimSpec->FisI) (apaOffRht + apaMulRht * uRes,
                                                   lpPrimSpec);
                        } // End FOR

                        break;

                    case ARRAY_FLOAT:   // Res = FLOAT, Rht = FLOAT
                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLFLOAT) lpMemRes)++ =
                              (*lpPrimSpec->FisF) (*((LPAPLFLOAT) lpMemRht)++,
                                                   lpPrimSpec);
                        } // End FOR

                        break;

                    defstop
                        break;
                } // End FOR/SWITCH

                break;

            case ARRAY_NESTED:          // Res = NESTED
                // Loop through the right arg/result
                for (uRes = 0; bRet && uRes < (APLNELMSIGN) aplNELMRht; uRes++, ((APLNESTED *) lpMemRht)++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    switch (GetPtrTypeInd (lpMemRht))
                    {
                        case PTRTYPE_STCONST:
                        {
                            APLSTYPE   aplTypeRes2,
                                       aplTypeRht2;
                            LPSYMENTRY lpSymSrc,
                                       lpSymDst;

                            // Get the type of the SYMENTRY
                            aplTypeRht2 = TranslateImmTypeToArrayType (((LPSYMENTRY) ClrPtrTypeInd (lpMemRht))->stFlags.ImmType);

                            // Get the storage type of the result
                            aplTypeRes2 = (*lpPrimSpec->StorageTypeMon) (1,
                                                                        &aplTypeRht2,
                                                                         lptkFunc);
                            // Check for DOMAIN ERROR
                            if (IsErrorType (aplTypeRes2))
                                goto DOMAIN_EXIT;

                            // Copy the SYMENTRY as the same type as the result
                            lpSymSrc = ClrPtrTypeInd (lpMemRht);
                            lpSymDst = CopyImmSymEntry_EM (lpSymSrc,
                                                           TranslateArrayTypeToImmType (aplTypeRes2),
                                                           lptkFunc);
                            if (lpSymDst NE NULL)
                            {
                                // Split cases based upon the result storage type
                                switch (aplTypeRes2)
                                {
                                    case ARRAY_BOOL:            // Res = BOOL
                                        // Split cases based upon the right arg's storage type
                                        switch (lpSymSrc->stFlags.ImmType)
                                        {
                                            case IMMTYPE_BOOL:  // Res = BOOL, Rht = BOOL
                                                lpSymDst->stData.stBoolean =
                                                  (*lpPrimSpec->BisB) (lpSymSrc->stData.stBoolean & BIT0,
                                                                       lpPrimSpec);
                                                break;

                                            case IMMTYPE_INT:   // Res = BOOL, Rht = INT
                                                lpSymDst->stData.stBoolean =
                                                  (*lpPrimSpec->BisI) (lpSymSrc->stData.stInteger,
                                                                       lpPrimSpec);
                                                break;

                                            case IMMTYPE_FLOAT: // Res = BOOL, Rht = FLOAT
                                                lpSymDst->stData.stBoolean =
                                                  (*lpPrimSpec->BisF) (lpSymSrc->stData.stFloat,
                                                                       lpPrimSpec);
                                                break;

                                            defstop
                                                break;
                                        } // End SWITCH

                                        break;

                                    case ARRAY_INT:
                                        // Split cases based upon the right arg's storage type
                                        switch (lpSymSrc->stFlags.ImmType)
                                        {
                                            case IMMTYPE_BOOL:  // Res = INT, Rht = BOOL
                                                lpSymDst->stData.stInteger =
                                                  (*lpPrimSpec->IisI) (lpSymSrc->stData.stBoolean & BIT0,
                                                                       lpPrimSpec);
                                                break;

                                            case IMMTYPE_INT:   // Res = INT, Rht = INT
                                                lpSymDst->stData.stInteger =
                                                  (*lpPrimSpec->IisI) (lpSymSrc->stData.stInteger,
                                                                       lpPrimSpec);
                                                break;

                                            case IMMTYPE_FLOAT: // Res = INT, Rht = FLOAT
                                                lpSymDst->stData.stInteger =
                                                  (*lpPrimSpec->IisF) (lpSymSrc->stData.stFloat,
                                                                       lpPrimSpec);
                                                break;

                                            defstop
                                                break;
                                        } // End SWITCH

                                        break;

                                    case ARRAY_FLOAT:
                                        // Split cases based upon the right arg's storage type
                                        switch (lpSymSrc->stFlags.ImmType)
                                        {
                                            case IMMTYPE_BOOL:  // Res = FLOAT, Rht = BOOL
                                                lpSymDst->stData.stFloat =
                                                  (*lpPrimSpec->FisI) (lpSymSrc->stData.stBoolean & BIT0,
                                                                       lpPrimSpec);
                                                break;

                                            case IMMTYPE_INT:   // Res = FLOAT, Rht = INT
                                                lpSymDst->stData.stFloat =
                                                  (*lpPrimSpec->FisI) (lpSymSrc->stData.stInteger,
                                                                       lpPrimSpec);
                                                break;

                                            case IMMTYPE_FLOAT: // Res = FLOAT, Rht = FLOAT
                                                lpSymDst->stData.stFloat =
                                                  (*lpPrimSpec->FisF) (lpSymSrc->stData.stFloat,
                                                                       lpPrimSpec);
                                                break;

                                            defstop
                                                break;
                                        } // End SWITCH

                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                // Save in the result
                                *((LPAPLNESTED) lpMemRes)++ = MakePtrTypeSym (lpSymDst);
                            } else
                                bRet = FALSE;
                            break;
                        } // End PTRTYPE_STCONST

                        case PTRTYPE_HGLOBAL:
                            // Handle via subroutine
                            hGlbSub = PrimFnMonGlb_EM (ClrPtrTypeInd (lpMemRht),
                                                       lptkFunc,
                                                       lpPrimSpec);
                            if (hGlbSub NE NULL)
                                // Save in the result
                                *((LPAPLNESTED) lpMemRes)++ = hGlbSub;
                            else
                                bRet = FALSE;
                            break;

                        defstop
                            break;
                    } // End SWITCH
                } // End FOR

                break;

            case ARRAY_RAT:             // Res = RAT
                Assert (IsRat (aplTypeRht));

                // Loop through the right arg/result
                for (uRes = 0; bRet && uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Save in the result
                    *((LPAPLRAT) lpMemRes)++ =
                      (*lpPrimSpec->RisR) (*((LPAPLRAT) lpMemRht)++,
                                           lpPrimSpec);
                } // End FOR

                break;

            case ARRAY_VFP:             // Res = VFP
                // Initialize the temp
                mpfr_init0 (&mpfRes);

                // Split cases based upon the right arg's storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:    // Res = VFP, Rht = BOOL
                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Convert the BOOL to a VFP
                            mpfr_set_si (&mpfRes, BIT0 & ((*(LPAPLBOOL) lpMemRht) >> uBitIndex), MPFR_RNDN);

                            // Save in the result
                            *((LPAPLVFP) lpMemRes)++ =
                              (*lpPrimSpec->VisV) (mpfRes,
                                                   lpPrimSpec);
                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;              // Start over
                                ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                            } // End IF
                        } // End FOR

                        break;

                    case ARRAY_INT:     // Res = VFP, Rht = INT
                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Convert the INT to a VFP
                            mpfr_set_sx (&mpfRes, *((LPAPLINT) lpMemRht)++, MPFR_RNDN);

                            // Save in the result
                            *((LPAPLVFP) lpMemRes)++ =
                              (*lpPrimSpec->VisV) (mpfRes,
                                                   lpPrimSpec);
                        } // End FOR

                        break;

                    case ARRAY_APA:     // Res = VFP, Rht = APA
                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Convert the APA to a VFP
                            mpfr_set_sx (&mpfRes, apaOffRht + apaMulRht * uRes, MPFR_RNDN);

                            // Save in the result
                            *((LPAPLVFP) lpMemRes)++ =
                              (*lpPrimSpec->VisV) (mpfRes,
                                                   lpPrimSpec);
                        } // End FOR

                        break;

                    case ARRAY_FLOAT:   // Res = VFP, Rht = FLOAT
                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Convert the FLOAT to a VFP
                            mpfr_set_d (&mpfRes, *((LPAPLFLOAT) lpMemRht)++, MPFR_RNDN);

                            // Save in the result
                            *((LPAPLVFP) lpMemRes)++ =
                              (*lpPrimSpec->VisV) (mpfRes,
                                                   lpPrimSpec);
                        } // End FOR

                        break;

                    case ARRAY_RAT:     // Res = VFP, Rht = RAT
                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Convert the RAT to a VFP
                            mpfr_set_q (&mpfRes, ((LPAPLRAT) lpMemRht)++, MPFR_RNDN);

                            // Save in the result
                            *((LPAPLVFP) lpMemRes)++ =
                              (*lpPrimSpec->VisV) (mpfRes,
                                                   lpPrimSpec);
                        } // End FOR

                        break;

                    case ARRAY_VFP:     // Res = VFP, Rht = VFP
                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLVFP) lpMemRes)++ =
                              (*lpPrimSpec->VisV) (*((LPAPLVFP) lpMemRht)++,
                                                   lpPrimSpec);
                        } // End FOR

                        break;

                    defstop
                        break;
                } // End FOR/SWITCH

                // We no longer need this storage
                Myf_clear (&mpfRes);

                break;

            defstop
                break;
        } // End SWITCH
    } __except (CheckException (GetExceptionInformation (), L"PrimFnMonGlb_EM"))
    {
        EXCEPTION_CODES ExceptionCode = MyGetExceptionCode ();  // The exception code

        dprintfWL9 (L"!!Initiating Exception in " APPEND_NAME L" #4: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

        // Split cases based upon the ExceptionCode
        switch (ExceptionCode)
        {
            case EXCEPTION_DOMAIN_ERROR:
            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            case EXCEPTION_INT_DIVIDE_BY_ZERO:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                goto DOMAIN_EXIT;

            case EXCEPTION_WS_FULL:
                goto WSFULL_EXIT;

            case EXCEPTION_NONCE_ERROR:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                goto NONCE_EXIT;

            case EXCEPTION_RESULT_RAT:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                if (IsNumeric (aplTypeRes)
                 && !IsRat    (aplTypeRes))
                {
                    // It's now a RAT result
                    aplTypeRes = ARRAY_RAT;

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

                    // Lock the memory to get a ptr to it
                    lpMemHdrRht = MyGlobalLockVar (hGlbRht);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

                    // We no longer need this storage
                    FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;

                    dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #4: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                    goto RESTART_EXCEPTION;
                } // End IF

                // Display message for unhandled exception
                DisplayException ();

                break;

            case EXCEPTION_RESULT_VFP:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                if (IsNumeric (aplTypeRes)
                 && !IsVfp    (aplTypeRes))
                {
                    // It's now a VFP result
                    aplTypeRes = ARRAY_VFP;

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

                    // Lock the memory to get a ptr to it
                    lpMemHdrRht = MyGlobalLockVar (hGlbRht);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

                    // We no longer need this storage
                    FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;

                    dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #4: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                    goto RESTART_EXCEPTION;
                } // End IF

                // Display message for unhandled exception
                DisplayException ();

                break;

            case EXCEPTION_RESULT_FLOAT:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                // Because the right arg is immediate, it can't be global numeric
                Assert (!IsGlbNum (aplTypeRes));

                if (IsNumeric (aplTypeRes)
                 && !IsSimpleFlt (aplTypeRes))
                {
                    // It's now a FLOAT result
                    aplTypeRes = ARRAY_FLOAT;

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

                    // Lock the memory to get a ptr to it
                    lpMemHdrRht = MyGlobalLockVar (hGlbRht);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

                    // We no longer need this storage
                    FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;

                    dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #4: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                    goto RESTART_EXCEPTION;
                } // End IF

                // Fall through to never-never-land

            default:
                // Display message for unhandled exception
                DisplayException ();

                break;
        } // End SWITCH
    } // End __try/__except

    if (bRet)
        goto NORMAL_EXIT;
    else
        goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
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
    // If the result global memory handle is valid, ...
    if (hGlbRes NE NULL)
        // Make it into a ptr type
        hGlbRes = MakePtrTypeGlb (hGlbRes);

    // We no longer need this storage
    Myf_clear (&mpfRes);

    if (lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    if (lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    return hGlbRes;
} // End PrimFnMonGlb_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDyd_EM_YY
//
//  Primitive scalar function for dyadic fns
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDyd_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDyd_EM_YY
    (LPTOKEN    lptkLftArg,         // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN    lptkFunc,           // Ptr to function token
     LPTOKEN    lptkRhtArg,         // Ptr to right arg token
     LPTOKEN    lptkAxis,           // Ptr to axis token (may be NULL)
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLRANK            aplRankLft,              // Left arg rank
                       aplRankRht,              // Right ...
                       aplRankRes;              // Result   ...
    APLNELM            aplNELMLft,              // Left arg NELM
                       aplNELMRht,              // Right ...
                       aplNELMRes,              // Result   ...
                       aplNELMAxis;             // Axis     ...
    APLLONGEST         aplLongestLft,           // Left arg longest value
                       aplLongestRht;           // Right ...
    HGLOBAL            hGlbLft = NULL,          // Left arg global memory handle
                       hGlbRht = NULL,          // Right ...
                       hGlbRes = NULL,          // Result   ...
                       hGlbAxis = NULL;         // Axis     ...
    APLSTYPE           aplTypeLft,              // Left arg storage type
                       aplTypeRht,              // Right ...
                       aplTypeRes;              // Result   ...
    LPAPLUINT          lpMemAxisHead = NULL,    // Ptr to axis values, fleshed out by CheckAxis_EM
                       lpMemAxisTail = NULL;    // Ptr to grade up of AxisHead
    LPVARARRAY_HEADER  lpMemHdrLft = NULL,      // Ptr to left arg header
                       lpMemHdrRht = NULL;      // ...    right    ...
    APLINT             aplInteger;              // Temporary integer value
    UBOOL              bRet = TRUE,             // TRUE iff result is valid
                       bLftIdent,               // TRUE iff the function has a left identity element and the Axis tail is valid
                       bRhtIdent;               // ...                         right ...
    LPPRIMFN_DYD_SNvSN lpPrimFn;                // Ptr to dyadic scalar Simp/Nest vs. Simp/Nest function
    LPPL_YYSTYPE       lpYYRes = NULL;          // Ptr to the result
    LPPRIMFLAGS        lpPrimFlags;             // Ptr to function PrimFlags entry
    PRIMSPEC           LclPrimSpec;             // Writable copy of PRIMSPEC

    // Save a writable copy of PRIMSPEC
    LclPrimSpec = *lpPrimSpec;

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Handle prototypes separately
    if (IsEmpty (aplNELMLft)
     || IsEmpty (aplNELMRht))
    {
        lpYYRes = PrimProtoFnScalar_EM_YY (lptkLftArg,  // Ptr to left arg token
                                           lptkFunc,    // Ptr to function token
                                           lptkRhtArg,  // Ptr to right arg token
                                           lptkAxis);   // Ptr to axis token (may be NULL)
        bRet = (lpYYRes NE NULL);

        goto NORMAL_EXIT;
    } // End IF

    // The rank of the result is the larger of the two args
    aplRankRes = max (aplRankLft, aplRankRht);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Check the axis values, fill in # elements in axis
        if (!CheckAxis_EM (lptkAxis,        // The axis token
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

    // Calculate the storage type of the result
    aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (aplNELMLft,
                                               &aplTypeLft,
                                                lptkFunc,
                                                aplNELMRht,
                                               &aplTypeRht);
    if (IsErrorType (aplTypeRes))
        goto DOMAIN_EXIT;

    Assert (IsNumeric (aplTypeRes)
         || IsNested  (aplTypeRes));

    // Get left and right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // Get a ptr to the Primitive Function Flags
    lpPrimFlags = GetPrimFlagsPtr (lptkFunc);

    // Set the identity element bits
    bLftIdent = lpPrimFlags->bLftIdent
             && (lpMemAxisTail NE NULL);
    bRhtIdent = lpPrimFlags->bRhtIdent
             && (lpMemAxisTail NE NULL);

    // Check for RANK and LENGTH ERRORs
    if (!CheckRankLengthError_EM (aplRankRes,
                                  aplRankLft,
                                  aplNELMLft,
                                  lpMemHdrLft,
                                  aplRankRht,
                                  aplNELMRht,
                                  lpMemHdrRht,
                                  aplNELMAxis,
                                  lpMemAxisTail,
                                  bLftIdent,
                                  bRhtIdent,
                                  lptkFunc))
        goto ERROR_EXIT;

    // The NELM of the result is the larger of the two args
    //   unless one is empty
    if (IsEmpty (aplNELMLft) || IsEmpty (aplNELMRht))
        aplNELMRes = 0;
    else
        aplNELMRes = max (aplNELMLft, aplNELMRht);

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Handle APA result separately
    if (IsSimpleAPA (aplTypeRes))
    {
        if (IsSimpleAPA (aplTypeLft))
            aplInteger = aplLongestRht;
        else
            aplInteger = aplLongestLft;
        __try
        {
            if (!(*lpPrimSpec->ApaResultDyd_EM) (lpYYRes,
                                                 lptkFunc,
                                                 hGlbLft,
                                                 hGlbRht,
                                                &hGlbRes,
                                                 aplRankLft,
                                                 aplRankRht,
                                                 aplNELMLft,
                                                 aplNELMRht,
                                                 aplInteger,
                                                &LclPrimSpec))
                goto ERROR_EXIT;
            else
                goto NORMAL_EXIT;
        } __except (CheckException (GetExceptionInformation (), L"PrimFnDyd_EM_YY #1"))
        {
            EXCEPTION_CODES ExceptionCode = MyGetExceptionCode ();  // The exception code

            dprintfWL9 (L"!!Initiating Exception in " APPEND_NAME L" #11: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

            // Split cases based upon the ExceptionCode
            switch (ExceptionCode)
            {
                case EXCEPTION_RESULT_FLOAT:
                    MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                    // It's now a FLOAT result
                    aplTypeRes = ARRAY_FLOAT;

                    if (hGlbRes NE NULL)
                    {
                        // We need to start over with the result
                        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
                    } // End IF

                    dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #11: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                    break;

                defstop
                    RaiseException (ExceptionCode, 0, 0, NULL);

                    break;
            } // End SWITCH
        } // End __try/__except
    } // End IF

    // Allocate space for result
    if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                    &hGlbRes,
                                     lpMemHdrLft,
                                     lpMemHdrRht,
                                     aplRankLft,
                                     aplRankRht,
                                    &aplRankRes,
                                     aplTypeRes,
                                     bLftIdent,
                                     bRhtIdent,
                                     aplNELMLft,
                                     aplNELMRht,
                                     aplNELMRes))
        goto ERROR_EXIT;

    // Four cases:
    //   Result     Left       Right
    //   ----------------------------
    //   Simple     Simple     Simple
    //   Nested     Simple     Nested
    //   Nested     Nested     Simple
    //   Nested     Nested     Nested

    // Split cases based upon the combined left vs. right types
    if ( IsNested (aplTypeLft)
     &&  IsNested (aplTypeRht))
        // Left arg is NESTED, right arg is NESTED
        lpPrimFn = &PrimFnDydNestNest_EM;
    else
    if (!IsNested (aplTypeLft)
     &&  IsNested (aplTypeRht))
        // Left arg is SIMPLE, right arg is NESTED
        lpPrimFn = &PrimFnDydSimpNest_EM;
    else
    if ( IsNested (aplTypeLft)
     && !IsNested (aplTypeRht))
        // Left arg is NESTED, right arg is SIMPLE
        lpPrimFn = &PrimFnDydNestSimp_EM;
    else
    if (!IsNested (aplTypeLft)
     && !IsNested (aplTypeRht))
        // Left arg is SIMPLE, right arg is SIMPLE
        lpPrimFn = &PrimFnDydSimpSimp_EM;
    else
    {
        DbgStop ();     // We should never get here

        return NULL;
    } // Endf IF/ELSE/...

    // Call the appropriate function
    if (!(*lpPrimFn) (lpYYRes,
                      lptkLftArg,
                      lptkFunc,
                      lptkRhtArg,
                      hGlbLft,
                      hGlbRht,
                     &hGlbRes,
                      lpMemHdrLft,      // Ptr to left arg header
                      lpMemHdrRht,      // ...    right ...
                      lpMemAxisHead,    // Ptr to axis values, fleshed out by CheckAxis_EM
                      lpMemAxisTail,    // Ptr to grade up of AxisHead
                      aplRankLft,
                      aplRankRht,
                      aplRankRes,
                      aplTypeLft,
                      aplTypeRht,
                      aplTypeRes,
                      aplNELMLft,
                      aplNELMRht,
                      aplNELMRes,
                      aplNELMAxis,
                      bLftIdent,
                      bRhtIdent,
                     &LclPrimSpec))
        goto ERROR_EXIT;
    else
        goto NORMAL_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    bRet = FALSE;

    if (hGlbRes NE NULL)
    {
        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    // If the result is valid, ...
    if (lpYYRes NE NULL)
        // Fill in the character index
        lpYYRes->tkToken.tkCharIndex = lptkFunc->tkCharIndex;

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

    if (hGlbAxis NE NULL && lpMemAxisHead NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbAxis); lpMemAxisHead = lpMemAxisTail = NULL;
    } // End IF

    // If we failed and there's a result, free it
    if (!bRet && lpYYRes NE NULL)
    {
        YYFree (lpYYRes); lpYYRes = NULL;
    } // End IF/ELSE

    return lpYYRes;
} // End PrimFnDyd_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydSimpNest_EM
//
//  Dyadic primitive scalar function, left simple, right nested
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSimpNest_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydSimpNest_EM
    (LPPL_YYSTYPE      lpYYRes,         // Ptr to the result

     LPTOKEN           lptkLftArg,      // Ptr to left arg token
     LPTOKEN           lptkFunc,        // ...    function ...
     LPTOKEN           lptkRhtArg,      // ...    right arg ...

     HGLOBAL           hGlbLft,         // Left arg handle
     HGLOBAL           hGlbRht,         // Right ...
     HGLOBAL          *lphGlbRes,       // Ptr to result handle

     LPVARARRAY_HEADER lpMemHdrLft,     // Ptr to left arg header
     LPVARARRAY_HEADER lpMemHdrRht,     // ...    right    ...

     LPAPLUINT         lpMemAxisHead,   // Ptr to axis values, fleshed out by CheckAxis_EM
     LPAPLUINT         lpMemAxisTail,   // Ptr to grade up of AxisHead
                                        //
     APLRANK           aplRankLft,      // Left arg rank
     APLRANK           aplRankRht,      // Right ...
     APLRANK           aplRankRes,      // Result ...
                                        //
     APLSTYPE          aplTypeLft,      // Left arg type
     APLSTYPE          aplTypeRht,      // Right ...
     APLSTYPE          aplTypeRes,      // Result ...

     APLNELM           aplNELMLft,      // Left arg NELM
     APLNELM           aplNELMRht,      // Right ...
     APLNELM           aplNELMRes,      // Result ...
     APLNELM           aplNELMAxis,     // Axis ...

     UBOOL             bLftIdent,       // TRUE iff the function has a left identity element and the Axis tail is valid
     UBOOL             bRhtIdent,       // ...                         right ...

     LPPRIMSPEC        lpPrimSpec)      // Ptr to local PRIMSPEC

{
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to result header
    LPVOID            lpMemLft,             // Ptr to left arg global memory
                      lpMemRht,             // ...    right    ...
                      lpMemRes;             // ...    result   ...
    UBOOL             bRet = TRUE;          // TRUE iff result is valid
    APLINT            uRes;                 // Loop counter
    APLINT            aplIntegerLft,
                      aplIntegerRht,
                      apaOffLft,
                      apaMulLft,
                      iDim;
    APLUINT           ByteAlloc;
    APLFLOAT          aplFloatLft,
                      aplFloatRht;
    HGLOBAL           hGlbWVec = NULL,
                      hGlbOdo = NULL,
                      lpSymGlbLft,
                      hGlbSub;
    LPAPLUINT         lpMemWVec = NULL,
                      lpMemDimLft,
                      lpMemDimRht,
                      lpMemDimRes,
                      lpMemOdo = NULL;
    APLCHAR           aplCharLft,
                      aplCharRht;
    LPPLLOCALVARS     lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;     // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // If the left arg is immediate, get the one and only value
    if (lpMemHdrLft EQ NULL)
        GetFirstValueToken (lptkLftArg,     // Ptr to left arg token
                           &aplIntegerLft,  // Ptr to integer result
                           &aplFloatLft,    // Ptr to float ...
                           &aplCharLft,     // Ptr to WCHAR ...
                            NULL,           // Ptr to longest ...
                            NULL,           // Ptr to lpSym/Glb ...
                            NULL,           // Ptr to ...immediate type ...
                            NULL);          // Ptr to array type ...
    else
    {
        // Skip over the header to the dimensions
        lpMemDimLft = VarArrayBaseToDim (lpMemHdrLft);

        // Skip over the header and dimensions to the data
        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
    } // End IF/ELSE

    // If the left arg is APA, ...
    if (IsSimpleAPA (aplTypeLft))
    {
#define lpAPA       ((LPAPLAPA) lpMemLft)
        // Get the APA parameters
        apaOffLft = lpAPA->Off;
        apaMulLft = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);

    // Skip over the header to the dimensions
    lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);
    lpMemDimRes = VarArrayBaseToDim (lpMemHdrRes);

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
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
            uRes *= lpMemDimRes[iDim];
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

    // Loop through the result
    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
    {
        APLINT   uLft, uRht, uArg;
        APLSTYPE aplTypeHetLft,
                 aplTypeHetRht;

        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Copy in case we are heterogeneous
        aplTypeHetLft = aplTypeLft;

        // If the left arg is not immediate, get the next value
        if (lpMemHdrLft NE NULL)
        {
            if (aplNELMAxis NE aplRankRes)
            {
                // Loop through the odometer values accumulating
                //   the weighted sum
                for (uArg = 0, uRht = aplRankRes - aplNELMAxis; uRht < (APLRANKSIGN) aplRankRes; uRht++)
                    uArg += lpMemOdo[lpMemAxisHead[uRht]] * lpMemWVec[uRht];

                // Increment the odometer in lpMemOdo subject to
                //   the values in lpMemDimRes
                IncrOdometer (lpMemOdo, lpMemDimRes, NULL, aplRankRes);

                // Use the just computed index for the argument
                //   with the smaller rank
                if (aplRankLft < aplRankRht)
                {
                    uLft = uArg * !IsScalar (aplRankLft);
                    uRht = uRes;
                } else
                {
                    uRht = uArg * !IsScalar (aplRankRht);
                    uLft = uRes;
                } // End IF/ELSE
            } else
            {
                uLft = uRes % aplNELMLft;
                uRht = uRes % aplNELMRht;
            } // End IF/ELSE

            // Split cases based upon the left arg's storage type
            switch (aplTypeLft)
            {
                case ARRAY_BOOL:
                case ARRAY_INT:
                case ARRAY_APA:
                    aplIntegerLft = GetNextInteger (lpMemLft, aplTypeLft, uLft);
                    aplFloatLft   = (APLFLOAT) aplIntegerLft;   // In case of type promotion

                    break;

                case ARRAY_FLOAT:
                    aplFloatLft   = GetNextFloat   (lpMemLft, aplTypeLft, uLft);

                    break;

                case ARRAY_CHAR:
                    aplCharLft    = ((LPAPLCHAR) lpMemLft)[uLft];

                    break;

                case ARRAY_HETERO:
                    aplTypeHetLft = GetNextHetero (lpMemLft, uLft, &aplIntegerLft, &aplFloatLft, &aplCharLft, &lpSymGlbLft);

                    break;

                case ARRAY_RAT:
                    lpSymGlbLft   = &((LPAPLRAT) lpMemLft)[uLft];

                    break;

                case ARRAY_VFP:
                    lpSymGlbLft   = &((LPAPLVFP) lpMemLft)[uLft];

                    break;

                defstop
                    break;
            } // End SWITCH
        } else
            uRht = uRes;

        // Get the right arg element
        hGlbSub = ((LPAPLNESTED) lpMemRht)[uRht];

        // Split cases based upon the ptr type of the nested right arg
        switch (GetPtrTypeDir (hGlbSub))
        {
            case PTRTYPE_STCONST:
                GetFirstValueImm (((LPSYMENTRY) hGlbSub)->stFlags.ImmType,
                                  ((LPSYMENTRY) hGlbSub)->stData.stLongest,
                                 &aplIntegerRht,
                                 &aplFloatRht,
                                 &aplCharRht,
                                  NULL,
                                  NULL,
                                 &aplTypeHetRht);
                hGlbSub = PrimFnDydSiScSiSc_EM (lptkFunc,
                                               *lphGlbRes,
                                                aplTypeHetLft,
                                                aplIntegerLft,
                                                aplFloatLft,
                                                aplCharLft,
                                                lpSymGlbLft,
                                                aplTypeHetRht,
                                                aplIntegerRht,
                                                aplFloatRht,
                                                aplCharRht,
                                                NULL,
                                                lpPrimSpec);
                if (hGlbSub EQ NULL)
                    goto ERROR_EXIT;
                else
                // If the result is not already filled in, ...
                if (((LPAPLNESTED) lpMemRes)[0] EQ NULL)
                    // Save in the result
                    ((LPAPLNESTED) lpMemRes)[0] = hGlbSub;
                // Skip to the next result
                ((LPAPLNESTED) lpMemRes)++;

                break;

            case PTRTYPE_HGLOBAL:
                // 2 4-({enclose}0 1)(0 1 2)
                hGlbSub = PrimFnDydSiScNest_EM (lptkFunc,
                                                aplTypeHetLft,
                                                aplIntegerLft,
                                                aplFloatLft,
                                                aplCharLft,
                                                lpSymGlbLft,
                                                hGlbSub,
                                                bLftIdent,
                                                bRhtIdent,
                                                lpPrimSpec);
                if (hGlbSub EQ NULL)
                    goto ERROR_EXIT;
                else
                    // Save in the result
                    *((LPAPLNESTED) lpMemRes)++ = MakePtrTypeGlb (hGlbSub);
                break;

            defstop
                break;
        } // End SWITCH
    } // End FOR

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (*lphGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    bRet = FALSE;

    if (*lphGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (*lphGlbRes); *lphGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (*lphGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
    } // End IF

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbWVec, lpMemWVec);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbOdo, lpMemOdo);

    return bRet;
} // End PrimFnDydSimpNest_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydNestSimp_EM
//
//  Dyadic primitive scalar function, left nested, right simple
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydNestSimp_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydNestSimp_EM
    (LPPL_YYSTYPE  lpYYRes,         // Ptr to the result

     LPTOKEN       lptkLftArg,      // Ptr to left arg token
     LPTOKEN       lptkFunc,        // ...    function ...
     LPTOKEN       lptkRhtArg,      // ...    right arg ...

     HGLOBAL       hGlbLft,         // Left arg handle
     HGLOBAL       hGlbRht,         // Right ...
     HGLOBAL      *lphGlbRes,       // Ptr to result handle

     LPVARARRAY_HEADER lpMemHdrLft, // Ptr to left arg header
     LPVARARRAY_HEADER lpMemHdrRht, // Ptr to right ...

     LPAPLUINT     lpMemAxisHead,   // Ptr to axis values, fleshed out by CheckAxis_EM
     LPAPLUINT     lpMemAxisTail,   // Ptr to grade up of AxisHead
                                    //
     APLRANK       aplRankLft,      // Left arg rank
     APLRANK       aplRankRht,      // Right ...
     APLRANK       aplRankRes,      // Result ...

     APLSTYPE      aplTypeLft,      // Left arg type
     APLSTYPE      aplTypeRht,      // Right ...
     APLSTYPE      aplTypeRes,      // Result ...

     APLNELM       aplNELMLft,      // Left arg NELM
     APLNELM       aplNELMRht,      // Right ...
     APLNELM       aplNELMRes,      // Result ...
     APLNELM       aplNELMAxis,     // Axis ...

     UBOOL         bLftIdent,       // TRUE iff the function has a left identity element and the Axis tail is valid
     UBOOL         bRhtIdent,       // ...                         right ...

     LPPRIMSPEC    lpPrimSpec)      // Ptr to local PRIMSPEC

{
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to result header
    LPVOID            lpMemLft,             // Ptr to left arg global memory
                      lpMemRht,             // ...    right    ...
                      lpMemRes;             // ...    result   ...
    UBOOL             bRet = TRUE;          // TRUE iff result is valid
    APLINT            uRes;                 // Loop counter
    APLINT            aplIntegerLft,
                      aplIntegerRht,
                      apaOffRht,
                      apaMulRht,
                      ByteAlloc,
                      iDim;
    APLFLOAT          aplFloatLft,
                      aplFloatRht;
    HGLOBAL           hGlbWVec = NULL,
                      hGlbOdo = NULL,
                      lpSymGlbRht,
                      hGlbSub;
    LPAPLUINT         lpMemWVec = NULL,
                      lpMemDimLft,
                      lpMemDimRht,
                      lpMemDimRes,
                      lpMemOdo = NULL;
    APLCHAR           aplCharLft,
                      aplCharRht;
    LPPLLOCALVARS     lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;     // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // If the right arg is immediate, get the one and only value
    if (lpMemHdrRht EQ NULL)
       GetFirstValueToken (lptkRhtArg,      // Ptr to right arg token
                          &aplIntegerRht,   // Ptr to integer result
                          &aplFloatRht,     // Ptr to float ...
                          &aplCharRht,      // Ptr to WCHAR ...
                           NULL,            // Ptr to longest ...
                           NULL,            // Ptr to lpSym/Glb ...
                           NULL,            // Ptr to ...immediate type ...
                           NULL);           // Ptr to array type ...
    else
    {
        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);

        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    } // End IF/ELSE

    // If the right arg is APA, ...
    if (IsSimpleAPA (aplTypeRht))
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        // Get the APA parameters
        apaOffRht = lpAPA->Off;
        apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);

    // Skip over the header to the dimensions
    lpMemDimLft = VarArrayBaseToDim (lpMemHdrLft);
    lpMemDimRes = VarArrayBaseToDim (lpMemHdrRes);

    // Skip over the header and dimensions to the data
    lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
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
            uRes *= lpMemDimRes[iDim];
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

    // Loop through the result
    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
    {
        APLINT   uLft, uRht, uArg;
        APLSTYPE aplTypeHetLft,
                 aplTypeHetRht;

        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Copy in case we are heterogeneous
        aplTypeHetRht = aplTypeRht;

        // If the right arg is not immediate, get the next value
        if (lpMemHdrRht NE NULL)
        {
            if (aplNELMAxis NE aplRankRes)
            {
                // Loop through the odometer values accumulating
                //   the weighted sum
                for (uArg = 0, uLft = aplRankRes - aplNELMAxis; uLft < (APLRANKSIGN) aplRankRes; uLft++)
                    uArg += lpMemOdo[lpMemAxisHead[uLft]] * lpMemWVec[uLft];

                // Increment the odometer in lpMemOdo subject to
                //   the values in lpMemDimRes
                IncrOdometer (lpMemOdo, lpMemDimRes, NULL, aplRankRes);

                // Use the just computed index for the argument
                //   with the smaller rank
                if (aplRankLft < aplRankRht)
                {
                    uLft = uArg * !IsScalar (aplRankLft);
                    uRht = uRes;
                } else
                {
                    uRht = uArg * !IsScalar (aplRankRht);
                    uLft = uRes;
                } // End IF/ELSE
            } else
            {
                uLft = uRes % aplNELMLft;
                uRht = uRes % aplNELMRht;
            } // End IF/ELSE

            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:
                case ARRAY_INT:
                case ARRAY_APA:
                    aplIntegerRht = GetNextInteger (lpMemRht, aplTypeRht, uRht);
                    aplFloatRht   = (APLFLOAT) aplIntegerRht;   // In case of type promotion

                    break;

                case ARRAY_FLOAT:
                    aplFloatRht   = GetNextFloat   (lpMemRht, aplTypeRht, uRht);

                    break;

                case ARRAY_CHAR:
                    aplCharRht    = ((LPAPLCHAR) lpMemRht)[uRht];

                    break;

                case ARRAY_HETERO:
                    aplTypeHetRht = GetNextHetero (lpMemRht, uRht, &aplIntegerRht, &aplFloatRht, &aplCharRht, &lpSymGlbRht);

                    break;

                case ARRAY_RAT:
                    lpSymGlbRht   = &((LPAPLRAT) lpMemRht)[uRht];

                    break;

                case ARRAY_VFP:
                    lpSymGlbRht   = &((LPAPLVFP) lpMemRht)[uRht];

                    break;

                defstop
                    break;
            } // End SWITCH
        } else
            uLft = uRes;

        // Get the left arg element
        hGlbSub = ((LPAPLNESTED) lpMemLft)[uLft];

        // Split cases based upon the ptr type of the nested left arg
        switch (GetPtrTypeDir (hGlbSub))
        {
            case PTRTYPE_STCONST:
                GetFirstValueImm (((LPSYMENTRY) hGlbSub)->stFlags.ImmType,
                                  ((LPSYMENTRY) hGlbSub)->stData.stLongest,
                                 &aplIntegerLft,
                                 &aplFloatLft,
                                 &aplCharLft,
                                  NULL,
                                  NULL,
                                 &aplTypeHetLft);
                hGlbSub = PrimFnDydSiScSiSc_EM (lptkFunc,
                                               *lphGlbRes,
                                                aplTypeHetLft,
                                                aplIntegerLft,
                                                aplFloatLft,
                                                aplCharLft,
                                                NULL,
                                                aplTypeHetRht,
                                                aplIntegerRht,
                                                aplFloatRht,
                                                aplCharRht,
                                                lpSymGlbRht,
                                                lpPrimSpec);
                if (hGlbSub EQ NULL)
                    goto ERROR_EXIT;
                else
                // If the result is not already filled in, ...
                if (((LPAPLNESTED) lpMemRes)[0] EQ NULL)
                    // Save in the result
                    ((LPAPLNESTED) lpMemRes)[0] = hGlbSub;
                // Skip to the next result
                ((LPAPLNESTED) lpMemRes)++;

                break;

            case PTRTYPE_HGLOBAL:
                // ({enclose}0 1)(0 1 2)-2 4
                hGlbSub = PrimFnDydNestSiSc_EM (lptkFunc,
                                                aplTypeHetRht,
                                                aplIntegerRht,
                                                aplFloatRht,
                                                aplCharRht,
                                                lpSymGlbRht,
                                                hGlbSub,
                                                bLftIdent,
                                                bRhtIdent,
                                                lpPrimSpec);
                if (hGlbSub EQ NULL)
                    goto ERROR_EXIT;
                else
                    // Save in the result
                    *((LPAPLNESTED) lpMemRes)++ = MakePtrTypeGlb (hGlbSub);
                break;

            defstop
                break;
        } // End SWITCH
    } // End FOR

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (*lphGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    bRet = FALSE;

    if (*lphGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (*lphGlbRes); *lphGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (*lphGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
    } // End IF

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbWVec, lpMemWVec);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbOdo, lpMemOdo);

    return bRet;
} // End PrimFnDydNestSimp_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydNestSiSc_EM
//
//  Subroutine to PrimFnDydNestSimp_EM to handle right arg simple scalars
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydNestSiSc_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL PrimFnDydNestSiSc_EM
    (LPTOKEN    lptkFunc,           // Ptr to function token
     APLSTYPE   aplTypeRht,         // Right arg type
     APLINT     aplIntegerRht,      // ...       integer value
     APLFLOAT   aplFloatRht,        // ...       float   ...
     APLCHAR    aplCharRht,         // ...       char    ...
     HGLOBAL    lpSymGlbRht,        // ...       lpSymGlb
     APLNESTED  aplNestedLft,       // Left arg nested value
     UBOOL      bLftIdent,          // TRUE iff the function has a left identity element and the Axis tail is valid
     UBOOL      bRhtIdent,          // ...                         right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    UBOOL             bRet = TRUE;
    HGLOBAL           hGlbLft = NULL,
                      hGlbRes = NULL,
                      hGlbSub;
    LPVARARRAY_HEADER lpMemHdrRes = NULL,
                      lpMemHdrLft = NULL;
    LPVOID            lpMemLft,
                      lpMemRes;
    APLSTYPE          aplTypeLft,
                      aplTypeRes;
    APLNELM           aplNELMLft,
                      aplNELMRht = 1,
                      aplNELMRes;
    APLRANK           aplRankLft,
                      aplRankRht = 0,
                      aplRankRes;
    APLINT            aplIntegerLft,
                      uLft,
                      apaOffLft,
                      apaMulLft;
    APLFLOAT          aplFloatLft;
    APLCHAR           aplCharLft;
    UINT              uBitIndex = 0;


    // The left arg data is a valid HGLOBAL array
    Assert (IsGlbTypeVarDir_PTB (aplNestedLft));

    // Clear the identifying bits
    hGlbLft = aplNestedLft;

    // Lock the memory to get a ptr to it
    lpMemHdrLft = MyGlobalLockVar (hGlbLft);

#define lpHeader    lpMemHdrLft
    // Get the Array Type, NELM, and Rank
    aplTypeLft = lpHeader->ArrType;
    aplNELMLft = lpHeader->NELM;
    aplRankLft = lpHeader->Rank;
#undef  lpHeader

    // The NELM of the result is NELM of the non-scalar
    aplNELMRes = aplNELMLft;

    // Calculate the storage type of the result
    aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (aplNELMLft,
                                               &aplTypeLft,
                                                lptkFunc,
                                                aplNELMRht,
                                               &aplTypeRht);
    if (IsErrorType (aplTypeRes))
        goto DOMAIN_EXIT;

    // Special case APA result
    if (IsSimpleAPA (aplTypeRes))
    {
        if (!(*lpPrimSpec->ApaResultDyd_EM) (NULL,
                                             lptkFunc,
                                             hGlbLft,
                                             NULL,
                                            &hGlbRes,
                                             aplRankLft,
                                             aplRankRht,
                                             aplNELMLft,
                                             aplNELMRht,
                                             aplIntegerRht,
                                             lpPrimSpec))
            goto ERROR_EXIT;
        else
            goto NORMAL_EXIT;
    } // End IF

    Assert (IsSimpleGlbNum (aplTypeRes)
         || IsNested (aplTypeRes));

    // Allocate space for result
    if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                    &hGlbRes,
                                     NULL,
                                     lpMemHdrLft,
                                     aplRankRht,
                                     aplRankLft,
                                    &aplRankRes,
                                     aplTypeRes,
                                     bLftIdent,
                                     bRhtIdent,
                                     aplNELMRht,
                                     aplNELMLft,
                                     aplNELMRes))
        goto ERROR_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (hGlbRes);

    // Skip over the header and dimensions to the data
    lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // If the left arg is an APA, ...
    if (IsSimpleAPA (aplTypeLft))
    {
#define lpAPA       ((LPAPLAPA) lpMemLft)
        // Get the APA parameters
        apaOffLft = lpAPA->Off;
        apaMulLft = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // If simple result, ...
    if (IsSimpleGlbNum (aplTypeRes))
        bRet = PrimFnDydMultSing_EM (&hGlbRes,
                                     aplTypeRes,
                                     lpMemHdrRes,
                                    &lpMemRes,
                                     aplNELMRes,
                                     aplTypeLft,
                                     apaOffLft,
                                     apaMulLft,
                                     lpMemHdrLft,
                                     lpMemLft,
                                     aplTypeRht,
                                     aplIntegerRht,
                                     aplFloatRht,
                                     aplCharRht,
                                     lpSymGlbRht,
                                     bLftIdent,
                                     bRhtIdent,
                                     lptkFunc,
                                     lpPrimSpec);
    else
    // If nested result, ...
    if (IsNested (aplTypeRes))
    {
        // Loop through the left arg/result
        for (uLft = 0; uLft < (APLNELMSIGN) aplNELMLft; uLft++)
        {
            APLSTYPE aplTypeHetLft;

            // Copy in case we are heterogeneous
            aplTypeHetLft = aplTypeLft;

            // Get the left arg element
            hGlbSub = ((LPAPLNESTED) lpMemLft)[uLft];

            // Split cases based upon the ptr type of the nested left arg
            switch (GetPtrTypeDir (hGlbSub))
            {
                case PTRTYPE_STCONST:
                    GetFirstValueImm (((LPSYMENTRY) hGlbSub)->stFlags.ImmType,
                                      ((LPSYMENTRY) hGlbSub)->stData.stLongest,
                                     &aplIntegerLft,
                                     &aplFloatLft,
                                     &aplCharLft,
                                      NULL,
                                      NULL,
                                     &aplTypeHetLft);
                    hGlbSub = PrimFnDydSiScSiSc_EM (lptkFunc,
                                                    hGlbRes,
                                                    aplTypeHetLft,
                                                    aplIntegerLft,
                                                    aplFloatLft,
                                                    aplCharLft,
                                                    NULL,
                                                    aplTypeRht,
                                                    aplIntegerRht,
                                                    aplFloatRht,
                                                    aplCharRht,
                                                    lpSymGlbRht,
                                                    lpPrimSpec);
                    if (hGlbSub EQ NULL)
                        goto ERROR_EXIT;
                    else
                    // If the result is not already filled in, ...
                    if (((LPAPLNESTED) lpMemRes)[0] EQ NULL)
                        // Save in the result
                        ((LPAPLNESTED) lpMemRes)[0] = hGlbSub;
                    // Skip to the next result
                    ((LPAPLNESTED) lpMemRes)++;

                    break;

                case PTRTYPE_HGLOBAL:
                    // ({enclose}0 1)(0 1 2)-2 4
                    hGlbSub = PrimFnDydNestSiSc_EM (lptkFunc,
                                                    aplTypeRht,
                                                    aplIntegerRht,
                                                    aplFloatRht,
                                                    aplCharRht,
                                                    lpSymGlbRht,
                                                    hGlbSub,
                                                    bLftIdent,
                                                    bRhtIdent,
                                                    lpPrimSpec);
                    if (hGlbSub EQ NULL)
                        goto ERROR_EXIT;
                    else
                        // Save in the result
                        *((LPAPLNESTED) lpMemRes)++ = MakePtrTypeGlb (hGlbSub);
                    break;

                defstop
                    break;
            } // End SWITCH
        } // End FOR
    } else
        DbgStop ();         // We should never get here

    if (bRet)
        goto NORMAL_EXIT;
    else
        goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
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
    if (lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;

    if (bRet)
        return hGlbRes;
    else
        return NULL;
} // End PrimFnDydNestSiSc_EM
#undef  APPEND_NAME


//***************************************************************************
//  $FillToken_PTB
//
//  Fill in a token from a Sym/Glb whose value is sensitive to Ptr Type Bits.
//***************************************************************************

void FillToken_PTB
    (LPTOKEN lptkArg,           // Ptr to arg token
     LPVOID  lpSymGlb,          // Ptr to either HGLOBAL or LPSYMENTRY
     UINT    tkCharIndex)       // Character index in case of error

{
    // Split cases based upon the ptr type
    switch (GetPtrTypeDir (lpSymGlb))
    {
        case PTRTYPE_STCONST:
            lptkArg->tkFlags.TknType   = TKT_VARIMMED;
            lptkArg->tkFlags.ImmType   = ((LPSYMENTRY) lpSymGlb)->stFlags.ImmType;
////////////lptkArg->tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
            lptkArg->tkData.tkLongest  = ((LPSYMENTRY) lpSymGlb)->stData.stLongest;
            lptkArg->tkCharIndex       = tkCharIndex;

            break;

        case PTRTYPE_HGLOBAL:
            lptkArg->tkFlags.TknType   = TKT_VARARRAY;
////////////lptkArg->tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////////lptkArg->tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
            lptkArg->tkData.tkGlbData  = lpSymGlb;
            lptkArg->tkCharIndex       = tkCharIndex;

            break;

        defstop
            break;
    } // End SWITCH
} // End FillToken_PTB


//***************************************************************************
//  $PrimFnDydNestNest_EM
//
//  Dyadic primitive scalar function, left nested, right nested
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydNestNest_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydNestNest_EM
    (LPPL_YYSTYPE      lpYYRes,         // Ptr to the result

     LPTOKEN           lptkLftArg,      // Ptr to left arg token
     LPTOKEN           lptkFunc,        // ...    function ...
     LPTOKEN           lptkRhtArg,      // ...    right arg ...

     HGLOBAL           hGlbLft,         // Left arg handle
     HGLOBAL           hGlbRht,         // Right ...
     HGLOBAL          *lphGlbRes,       // Ptr to result handle

     LPVARARRAY_HEADER lpMemHdrLft,     // Ptr to left arg header
     LPVARARRAY_HEADER lpMemHdrRht,     // ...    right ...

     LPAPLUINT         lpMemAxisHead,   // Ptr to axis values, fleshed out by CheckAxis_EM
     LPAPLUINT         lpMemAxisTail,   // Ptr to grade up of AxisHead

     APLRANK           aplRankLft,      // Left arg rank
     APLRANK           aplRankRht,      // Right ...
     APLRANK           aplRankRes,      // Result ...

     APLSTYPE          aplTypeLft,      // Left arg type
     APLSTYPE          aplTypeRht,      // Right ...
     APLSTYPE          aplTypeRes,      // Result ...

     APLNELM           aplNELMLft,      // Left arg NELM
     APLNELM           aplNELMRht,      // Right ...
     APLNELM           aplNELMRes,      // Result ...
     APLNELM           aplNELMAxis,     // Axis ...

     UBOOL             bLftIdent,       // TRUE iff the function has a left identity element and the Axis tail is valid
     UBOOL             bRhtIdent,       // ...                         right ...

     LPPRIMSPEC        lpPrimSpec)      // Ptr to local PRIMSPEC

{
    UBOOL             bRet = TRUE;          // TRUE iff result is valid
    APLINT            uRes;                 // Loop counter
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to result header
    LPVOID            lpMemLft,             // Ptr to left arg global memory
                      lpMemRht,             // ...    right ...
                      lpMemRes;             // ...    result   ...
    LPPL_YYSTYPE      lpYYRes2;             // Ptr to the secondary result

    // Check for INNER RANK and LENGTH ERRORs
    if (!CheckRankLengthError_EM (aplRankRes,
                                  aplRankLft,
                                  aplNELMLft,
                                  lpMemHdrLft,
                                  aplRankRht,
                                  aplNELMRht,
                                  lpMemHdrRht,
                                  aplNELMAxis,
                                  lpMemAxisTail,
                                  bLftIdent,
                                  bRhtIdent,
                                  lptkFunc))
        goto ERROR_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);

    // Skip over the headers and dimensions to the data
    lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
    lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Loop through the left and right args
    for (uRes = 0; bRet && uRes < (APLNELMSIGN) aplNELMRes; uRes++)
    {
        TOKEN     tkLft = {0},
                  tkRht = {0};

        // Fill in the left arg token
        FillToken_PTB (&tkLft,
                        ((LPAPLNESTED) lpMemLft)[uRes % aplNELMLft],
                        lptkLftArg->tkCharIndex);
        // Fill in the right arg token
        FillToken_PTB (&tkRht,
                        ((LPAPLNESTED) lpMemRht)[uRes % aplNELMRht],
                        lptkRhtArg->tkCharIndex);
        // Call as dyadic function
        lpYYRes2 =
          (*lpPrimSpec->PrimFnDyd_EM_YY) (&tkLft,           // Ptr to left arg token
                                           lptkFunc,        // Ptr to function token
                                          &tkRht,           // Ptr to right arg token
                                           NULL,            // Ptr to axis token
                                           lpPrimSpec);     // Ptr to local PRIMSPEC
        if (lpYYRes2 NE NULL)
        {
            // If the result is immediate, make it into a SYMENTRY
            // Split cases based upon the result token type
            switch (lpYYRes2->tkToken.tkFlags.TknType)
            {
                case TKT_VARIMMED:
                    // Save in the result
                    ((LPAPLNESTED) lpMemRes)[uRes] =
                      MakeSymEntry_EM (lpYYRes2->tkToken.tkFlags.ImmType,   // Immediate type
                                      &lpYYRes2->tkToken.tkData.tkLongest,  // Ptr to immediate value
                                       lptkFunc);                           // Ptr to function token
                    break;

                case TKT_VARARRAY:
                    // Save in the result
                    ((LPAPLNESTED) lpMemRes)[uRes] =
                      lpYYRes2->tkToken.tkData.tkGlbData;
                    break;

                defstop
                    break;
            } // End SWITCH

            // Free the YYRes
            YYFree (lpYYRes2); lpYYRes2 = NULL;
        } else
            bRet = FALSE;
    } // End FOR

    // We no longer need this ptr
    MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;

    if (bRet)
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (*lphGlbRes);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } // End IF
ERROR_EXIT:
    return bRet;
} // End PrimFnDydNestNest_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydSingMult_EM
//
//  Primitive scalar dyadic function,
//    left simple singleton, right simple multipleton
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSingMult_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydSingMult_EM
    (HGLOBAL          *lphGlbRes,           // Ptr to result handle
     APLSTYPE          aplTypeRes,          // Result type
     LPVARARRAY_HEADER lpMemHdrRes,         // Ptr to result header (in case we blow up)
     LPVOID           *lplpMemRes,          // Ptr to ptr to result memory (Points to the data)
     APLNELM           aplNELMRes,          // Result NELM
     APLSTYPE          aplTypeLft,          // Left arg type
     APLINT            aplIntegerLft,       // ...      integer value
     APLFLOAT          aplFloatLft,         // ...      float   ...
     APLCHAR           aplCharLft,          // ...      char    ...
     HGLOBAL           lpSymGlbLft,         // ...      lpSymGlb
     APLSTYPE          aplTypeRht,          // Right arg type
     APLINT            apaOffRht,           // ...       APA offset
     APLINT            apaMulRht,           // ...       ... multiplier
     LPVARARRAY_HEADER lpMemHdrRht,         // Ptr to left/right arg header (in case we blow up)
     LPVOID            lpMemRht,            // Points to the data
     UBOOL             bLftIdent,           // TRUE iff the function has a left identity element and the Axis tail is valid
     UBOOL             bRhtIdent,           // ...                         right ...
     LPTOKEN           lptkFunc,            // Ptr to function token
     LPPRIMSPEC        lpPrimSpec)          // Ptr to local PRIMSPEC

{
    APLINT        uRes;
    UBOOL         bRet = FALSE;     // TRUE iff the result is valid
    APLRANK       aplRankRes;       // Temp var for DydAllocate
    APLNELM       aplNELMTmp,       // Temporary NELM
                  aplNELMRem;       // Remaining NELM
    UINT          uBitIndex = 0;
    LPVOID        lpMemRhtStart,
                  lpMemRes;
    APLSTYPE      aplTypeHetRht;
    APLINT        aplIntegerRht;
    APLFLOAT      aplFloatRht;
    APLCHAR       aplCharRht;
    LPPLLOCALVARS lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;     // Ptr to Ctrl-Break flag
    APLRAT        aplRatLft = {0},  // Left arg as RAT
                  aplRatRht = {0};  // Right ...
    APLVFP        aplVfpLft = {0},  // Left arg as VFP
                  aplVfpRht = {0};  // Right ...
    HGLOBAL       lpSymGlbRht;

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the memory ptr
    lpMemRes = *lplpMemRes;

    // If the singleton is integer,
    //   attempt to demote it to Boolean
    if (aplTypeLft EQ ARRAY_INT
     && IsBooleanValue (aplIntegerLft))
        aplTypeLft = ARRAY_BOOL;

    // Save the starting values in case we need
    //   to restart from an exception
    lpMemRhtStart = lpMemRht;
RESTART_EXCEPTION:
    // Skip over the header to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    __try
    {
    // Split cases based upon the storage type of the result
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:                    // Res = BOOL
            // Split off character or heterogeneous (CH) arguments
            if (IsSimpleCH (aplTypeLft)
             || IsSimpleCH (aplTypeRht))
            {
                APLBOOL bCvN;

                Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                     || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);

                // Calculate the result of CvN
                bCvN = (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);

                // If both arguments are CHAR,
                //   use BisCvC
                if (IsSimpleChar (aplTypeLft)
                 && IsSimpleChar (aplTypeRht))
                {
                    // Loop through the result
                    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Save in the result
                        *((LPAPLBOOL)  lpMemRes) |=
                          (*lpPrimSpec->BisCvC) (aplCharLft,
                                                 *((LPAPLCHAR) lpMemRht)++,
                                                 lpPrimSpec) << uBitIndex;
                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;                  // Start over
                            ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                        } // End IF
                    } // End FOR
                } else
                // If the other argument is simple or global numeric,
                //   use bCvN
                if ((IsNumeric (aplTypeLft) && IsSimpleChar (aplTypeRht))
                 || (IsNumeric (aplTypeRht) && IsSimpleChar (aplTypeLft)))
                {
                    // Loop through the result
                    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Save in the result
                        *((LPAPLBOOL)  lpMemRes) |= (bCvN << uBitIndex);

                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;                  // Start over
                            ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                        } // End IF
                    } // End FOR
                } else
                // If the multipleton argument is HETERO,
                //   look at each LPSYMENTRY
                if (IsSimpleHet (aplTypeRht))
                {
                    // Split cases based upon the left arg's storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:    // Res = BOOL, Lft = BOOL (S)
                        case ARRAY_INT:     // Res = BOOL, Lft = INT  (S)
                        case ARRAY_APA:     // Res = BOOL, Lft = APA  (S)
                            // Initialize the temps
                            mpq_init (&aplRatLft);
                            mpfr_init0 (&aplVfpLft);

                            // Loop through the right arg
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                aplTypeHetRht = GetNextHetero (lpMemRht, uRes, &aplIntegerRht, &aplFloatRht, &aplCharRht, &lpSymGlbRht);

                                // Split cases based upon the right arg's item's storage type
                                switch (aplTypeHetRht)
                                {
                                    case ARRAY_BOOL:    // Res = BOOL, Lft = BOOL/INT (S), Rht = HETERO:BOOL (M)
                                    case ARRAY_INT:     // Res = BOOL, Lft = BOOL/INT (S), Rht = HETERO:INT  (M)
                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisIvI) (aplIntegerLft,
                                                                 aplIntegerRht,
                                                                 lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_FLOAT:   // Res = BOOL, Lft = BOOL/INT (S), Rht = HETERO:FLOAT(M)
                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                                 aplFloatRht,
                                                                 lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_CHAR:    // Res = BOOL, Lft = BOOL/INT (S), Rht = HETERO:CHAR (M)
                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |= (bCvN << uBitIndex);

                                        break;

                                    case ARRAY_RAT:     // Res = BOOL, Lft = BOOL/INT (S), Rht = HETERO:RAT  (M)
                                        // Convert the BOOL/INT to a RAT
                                        mpq_set_sx (&aplRatLft, aplIntegerLft, 1);

                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisRvR) (aplRatLft,
                                                                *(LPAPLRAT) lpSymGlbRht,
                                                                 lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_VFP:     // Res = BOOL, Lft = BOOL/INT (S), Rht = HETERO:VFP  (M)
                                        // Convert the BOOL/INT to a VFP
                                        mpfr_set_sx (&aplVfpLft, aplIntegerLft, MPFR_RNDN);

                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                                *(LPAPLVFP) lpSymGlbRht,
                                                                 lpPrimSpec) << uBitIndex;
                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            // We no longer need this storage
                            Myf_clear (&aplVfpLft);
                            Myq_clear (&aplRatLft);

                            break;

                        case ARRAY_FLOAT:   // Res = BOOL, Lft = FLOAT(S)
                            // Initialize the temps
                            mpq_init (&aplRatLft);
                            mpfr_init0 (&aplVfpLft);
                            mpfr_init0 (&aplVfpRht);

                            // Loop through the right arg
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                aplTypeHetRht = GetNextHetero (lpMemRht, uRes, &aplIntegerRht, &aplFloatRht, &aplCharRht, &lpSymGlbRht);

                                // Split cases based upon the right arg's item's storage type
                                switch (aplTypeHetRht)
                                {
                                    case ARRAY_BOOL:    // Res = BOOL, Lft = FLOAT (S), Rht = HETERO:BOOL (M)
                                    case ARRAY_INT:     // Res = BOOL, Lft = FLOAT (S), Rht = HETERO:INT  (M)
                                    case ARRAY_FLOAT:   // Res = BOOL, Lft = FLOAT (S), Rht = HETERO:FLOAT(M)
                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                                 aplFloatRht,
                                                                 lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_CHAR:    // Res = BOOL, Lft = FLOAT (S), Rht = HETERO:CHAR (M)
                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |= (bCvN << uBitIndex);

                                        break;

                                    case ARRAY_RAT:     // Res = BOOL, Lft = FLOAT (S), Rht = HETERO:RAT  (M)
                                        // Convert the FLOAT to a VFP
                                        mpfr_set_d (&aplVfpLft, aplFloatLft, MPFR_RNDN);

                                        // Convert the RAT to a VFP
                                        mpfr_set_q (&aplVfpRht, (LPAPLRAT) lpSymGlbRht, MPFR_RNDN);

                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                                 aplVfpRht,
                                                                 lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_VFP:     // Res = BOOL, Lft = FLOAT (S), Rht = HETERO:VFP  (M)
                                        // Convert the FLOAT to a VFP
                                        mpfr_set_d (&aplVfpLft, aplFloatLft, MPFR_RNDN);

                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                                *(LPAPLVFP) lpSymGlbRht,
                                                                 lpPrimSpec) << uBitIndex;
                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            // We no longer need this storage
                            Myf_clear (&aplVfpRht);
                            Myf_clear (&aplVfpLft);
                            Myq_clear (&aplRatLft);

                            break;

                        case ARRAY_CHAR:    // Res = BOOL, Lft = CHAR (S)
                            // Loop through the right arg
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                aplTypeHetRht = GetNextHetero (lpMemRht, uRes, &aplIntegerRht, &aplFloatRht, &aplCharRht, NULL);

                                // Split cases based upon the right arg's item's storage type
                                switch (aplTypeHetRht)
                                {
                                    case ARRAY_BOOL:    // Res = BOOL, Lft = CHAR  (S), Rht = HETERO:BOOL (M)
                                    case ARRAY_INT:     // Res = BOOL, Lft = CHAR  (S), Rht = HETERO:INT  (M)
                                    case ARRAY_FLOAT:   // Res = BOOL, Lft = CHAR  (S), Rht = HETERO:FLOAT(M)
                                    case ARRAY_RAT:     // Res = BOOL, Lft = CHAR  (S), Rht = HETERO:RAT  (M)
                                    case ARRAY_VFP:     // Res = BOOL, Lft = CHAR  (S), Rht = HETERO:VFP  (M)
                                        *((LPAPLBOOL)  lpMemRes) |= (bCvN << uBitIndex);

                                        break;

                                    case ARRAY_CHAR:    // Res = BOOL, Lft = CHAR  (S), Rht = HETERO:CHAR (M)
                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisCvC) (aplCharLft,
                                                                 aplCharRht,
                                                                 lpPrimSpec) << uBitIndex;
                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_RAT:     // Res = BOOL, Lft = RAT  (S)
                            // Initialize the temps
                            mpq_init (&aplRatRht);
                            mpfr_init0 (&aplVfpLft);
                            mpfr_init0 (&aplVfpRht);

                            // Loop through the right arg
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                aplTypeHetRht = GetNextHetero (lpMemRht, uRes, &aplIntegerRht, &aplFloatRht, &aplCharRht, &lpSymGlbRht);

                                // Split cases based upon the right arg's item's storage type
                                switch (aplTypeHetRht)
                                {
                                    case ARRAY_BOOL:    // Res = BOOL, Lft = RAT (S), Rht = HETERO:BOOL (M)
                                    case ARRAY_INT:     // Res = BOOL, Lft = RAT (S), Rht = HETERO:INT  (M)
                                        // Convert the BOOL/INT to a RAT
                                        mpq_set_sx (&aplRatRht, aplIntegerRht, 1);

                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisRvR) (*(LPAPLRAT) lpSymGlbLft,
                                                                  aplRatRht,
                                                                  lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_FLOAT:   // Res = BOOL, Lft = RAT (S), Rht = HETERO:FLOAT(M)
                                        // Convert the RAT to a VFP
                                        mpfr_set_q (&aplVfpLft, (LPAPLRAT) lpSymGlbLft, MPFR_RNDN);

                                        // Convert the FLOAT to a VFP
                                        mpfr_set_d (&aplVfpRht, aplFloatRht, MPFR_RNDN);

                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                                 aplVfpRht,
                                                                 lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_CHAR:    // Res = BOOL, Lft = RAT (S), Rht = HETERO:CHAR (M)
                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |= (bCvN << uBitIndex);

                                        break;

                                    case ARRAY_RAT:     // Res = BOOL, Lft = RAT (S), Rht = HETERO:RAT  (M)
                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisRvR) (*(LPAPLRAT) lpSymGlbLft,
                                                                 *(LPAPLRAT) lpSymGlbRht,
                                                                  lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_VFP:     // Res = BOOL, Lft = RAT (S), Rht = HETERO:VFP  (M)
                                        // Convert the RAT to a VFP
                                        mpfr_set_q (&aplVfpLft, (LPAPLRAT) lpSymGlbLft, MPFR_RNDN);

                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                                *(LPAPLVFP) lpSymGlbRht,
                                                                 lpPrimSpec) << uBitIndex;
                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            // We no longer need this storage
                            Myf_clear (&aplVfpRht);
                            Myf_clear (&aplVfpLft);
                            Myq_clear (&aplRatRht);

                            break;

                        case ARRAY_VFP:     // Res = BOOL, Lft = VFP  (S)
                            // Initialize the temps
                            mpfr_init0 (&aplVfpLft);
                            mpfr_init0 (&aplVfpRht);

                            // Loop through the right arg
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                aplTypeHetRht = GetNextHetero (lpMemRht, uRes, &aplIntegerRht, &aplFloatRht, &aplCharRht, &lpSymGlbRht);

                                // Split cases based upon the right arg's item's storage type
                                switch (aplTypeHetRht)
                                {
                                    case ARRAY_BOOL:    // Res = BOOL, Lft = VFP (S), Rht = HETERO:BOOL (M)
                                    case ARRAY_INT:     // Res = BOOL, Lft = VFP (S), Rht = HETERO:INT  (M)
                                        // Convert the BOOL/INT to a VFP
                                        mpfr_set_sx (&aplVfpRht, aplIntegerRht, MPFR_RNDN);

                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisVvV) (*(LPAPLVFP) lpSymGlbLft,
                                                                  aplVfpRht,
                                                                  lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_FLOAT:   // Res = BOOL, Lft = VFP (S), Rht = HETERO:FLOAT(M)
                                        // Convert the FLOAT to a VFP
                                        mpfr_set_d (&aplVfpRht, aplFloatRht, MPFR_RNDN);

                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisVvV) (*(LPAPLVFP) lpSymGlbLft,
                                                                  aplVfpRht,
                                                                  lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_CHAR:    // Res = BOOL, Lft = VFP (S), Rht = HETERO:CHAR (M)
                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |= (bCvN << uBitIndex);

                                        break;

                                    case ARRAY_RAT:     // Res = BOOL, Lft = VFP (S), Rht = HETERO:RAT  (M)
                                        // Convert the RAT to a VFP
                                        mpfr_set_q (&aplVfpRht, (LPAPLRAT) lpSymGlbRht, MPFR_RNDN);

                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisVvV) (*(LPAPLVFP) lpSymGlbLft,
                                                                  aplVfpRht,
                                                                  lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_VFP:     // Res = BOOL, Lft = VFP (S), Rht = HETERO:VFP  (M)
                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisVvV) (*(LPAPLVFP) lpSymGlbLft,
                                                                 *(LPAPLVFP) lpSymGlbRht,
                                                                  lpPrimSpec) << uBitIndex;
                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            // We no longer need this storage
                            Myf_clear (&aplVfpRht);
                            Myf_clear (&aplVfpLft);

                            break;

                        defstop
                            break;
                    } // End SWITCH
                } else
                    DbgStop ();         // We should never get here
            } else
            // Split cases based upon the storage type of the left arg
            switch (aplTypeLft)
            {
                case ARRAY_BOOL:            // Res = BOOL, Lft = BOOL(S)
                    if (IsSimpleBool (aplTypeRht))
                    {
                        // Initialize # remaining NELM
                        aplNELMRem = aplNELMRes;

                        // Check for optimized chunking
                        if (lpPrimSpec->B64isB64vB64 NE NULL)
                        {
                            APLB64 aplB64Lft;

                            if (aplIntegerLft)
                                aplB64Lft = 0xFFFFFFFFFFFFFFFF;
                            else
                                aplB64Lft = 0x0000000000000000;

                            // Calculate the # 64-bit chunks
                            aplNELMTmp  = aplNELMRem / 64;
                            aplNELMRem -= aplNELMTmp * 64;

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB64) lpMemRes)++ =
                                  (*lpPrimSpec->B64isB64vB64) ((APLB64) aplB64Lft, *((LPAPLB64) lpMemRht)++, lpPrimSpec);
                            } // End FOR

                            // Calculate the # remaining 32-bit chunks
                            aplNELMTmp  = aplNELMRem / 32;
                            aplNELMRem -= aplNELMTmp * 32;

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB32) lpMemRes)++ =
                                  (*lpPrimSpec->B32isB32vB32) ((APLB32) aplB64Lft, *((LPAPLB32) lpMemRht)++, lpPrimSpec);
                            } // End FOR

                            // Calculate the # remaining 16-bit chunks
                            aplNELMTmp  = aplNELMRem / 16;
                            aplNELMRem -= aplNELMTmp * 16;

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB16) lpMemRes)++ =
                                  (*lpPrimSpec->B16isB16vB16) ((APLB16) aplB64Lft, *((LPAPLB16) lpMemRht)++, lpPrimSpec);
                            } // End FOR

                            // Calculate the # remaining  8-bit chunks
                            aplNELMTmp  = aplNELMRem /  8;
                            aplNELMRem -= aplNELMTmp *  8;

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB08) lpMemRes)++ =
                                  (*lpPrimSpec->B08isB08vB08) ((APLB08) aplB64Lft, *((LPAPLB08) lpMemRht)++, lpPrimSpec);
                            } // End FOR
                        } // End IF

                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRem; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLBOOL)  lpMemRes) |=
                              (*lpPrimSpec->BisBvB) ((APLBOOL) aplIntegerLft,
                                                     (APLBOOL) (BIT0 & ((*(LPAPLBOOL) lpMemRht) >> uBitIndex)),
                                                     lpPrimSpec) << uBitIndex;
                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;                  // Start over
                                ((LPAPLBOOL) lpMemRht)++;       // Skip to next byte
                                ((LPAPLBOOL) lpMemRes)++;       // ...
                            } // End IF
                        } // End FOR

                        break;
                    } else
#define lpAPA       ((LPAPLAPA) lpMemRht)
                    // Check for right arg Boolean APA and optimized chunking
                    if (IsSimpleAPA (aplTypeRht)
                     && IsBooleanAPA (lpAPA)
                     && lpPrimSpec->B64isB64vB64)
                    {
                        APLB64 aplB64Lft,
                               aplB64APA;
                        APLB08 aplB08Res;

                        if (lpAPA->Off NE 0)
                            aplB64APA = 0xFFFFFFFFFFFFFFFF;
                        else
                            aplB64APA = 0x0000000000000000;
#undef  lpAPA
                        if (aplIntegerLft NE 0)
                            aplB64Lft = 0xFFFFFFFFFFFFFFFF;
                        else
                            aplB64Lft = 0x0000000000000000;

                        // Calculate one byte of the result
                        aplB08Res = (*lpPrimSpec->B08isB08vB08) ((APLB08) aplB64Lft, (APLB08) aplB64APA, lpPrimSpec);

                        // If it's not all zero, ...
                        if (aplB08Res NE 0)
                            FillMemory (lpMemRes, (APLU3264) RoundUpBitsToBytes (aplNELMRes), aplB08Res);
                        break;
                    } // End IF/ELSE

                    // Fall through to common code

                case ARRAY_INT:             // Res = BOOL, Lft = INT(S)
                case ARRAY_APA:             // Res = BOOL, Lft = APA(S)
                    // Split cases based upon the storage type of the right arg
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:    // Res = BOOL, Lft = INT/APA(S), Rht = BOOL(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisIvI) (aplIntegerLft,
                                                         BIT0 & ((*(LPAPLBOOL) lpMemRht) >> uBitIndex),
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRht)++;       // Skip to next byte
                                    ((LPAPLBOOL) lpMemRes)++;       // ...
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = BOOL, Lft = INT/APA(S), Rht = INT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisIvI) (aplIntegerLft,
                                                         *((LPAPLINT) lpMemRht)++,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_APA:     // Res = BOOL, Lft = INT/APA(S), Rht = APA(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisIvI) (aplIntegerLft,
                                                         apaOffRht + apaMulRht * uRes,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = BOOL, Lft = INT/APA(S), Rht = FLOAT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                         *((LPAPLFLOAT) lpMemRht)++,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_RAT:     // Res = BOOL, Lft = INT/APA(S), Rht = RAT(M)
                            // Initialize the temp
                            mpq_init (&aplRatLft);

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the INT to a RAT
                                mpq_set_sx (&aplRatLft, aplIntegerLft, 1);

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisRvR) (aplRatLft,
                                                        *((LPAPLRAT) lpMemRht)++,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            // We no longer need this storage
                            Myq_clear (&aplRatLft);

                            break;

                        case ARRAY_VFP:     // Res = BOOL, Lft = INT/APA(S), Rht = VFP(M)
                            // Initialize the temp
                            mpfr_init0 (&aplVfpLft);

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the INT to a VFP
                                mpfr_set_sx (&aplVfpLft, aplIntegerLft, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                        *((LPAPLVFP) lpMemRht)++,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            // We no longer need this storage
                            Myf_clear (&aplVfpLft);

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case ARRAY_FLOAT:           // Res = BOOL, Lft = FLOAT(S)
                    // Split cases based upon the storage type of the right arg
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:    // Res = BOOL, Lft = FLOAT(S), Rht = BOOL(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                         BIT0 & ((*(LPAPLBOOL) lpMemRht) >> uBitIndex),
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRht)++;       // Skip to next byte
                                    ((LPAPLBOOL) lpMemRes)++;       // ...
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = BOOL, Lft = FLOAT(S), Rht = INT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                         (APLFLOAT) *((LPAPLINT) lpMemRht)++,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_APA:     // Res = BOOL, Lft = FLOAT(S), Rht = APA(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                         (APLFLOAT) (apaOffRht + apaMulRht * uRes),
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = BOOL, Lft = FLOAT(S), Rht = FLOAT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                         *((LPAPLFLOAT) lpMemRht)++,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_RAT:     // Res = BOOL, Lft = FLOAT(S), Rht = RAT(M)
                            // Initialize the temp
                            mpfr_init0 (&aplVfpLft);
                            mpfr_init0 (&aplVfpRht);

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the FLOAT to a VFP
                                mpfr_set_d (&aplVfpLft, aplFloatLft, MPFR_RNDN);

                                // Convert the RAT to a VFP
                                mpfr_set_q (&aplVfpRht, ((LPAPLRAT) lpMemRht)++, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                         aplVfpRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            // We no longer need this storage
                            Myf_clear (&aplVfpRht);
                            Myf_clear (&aplVfpLft);

                            break;

                        case ARRAY_VFP:     // Res = BOOL, Lft = FLOAT(S), Rht = VFP(M)
                            // Initialize the temp
                            mpfr_init0 (&aplVfpLft);

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the FLOAT to a VFP
                                mpfr_set_d (&aplVfpLft, aplFloatLft, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                        *((LPAPLVFP) lpMemRht)++,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            // We no longer need this storage
                            Myf_clear (&aplVfpLft);

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case ARRAY_RAT:             // Res = BOOL, Lft = RAT  (S)
                    // Initialize the temps
                    mpq_init (&aplRatRht);
                    mpfr_init0 (&aplVfpLft);
                    mpfr_init0 (&aplVfpRht);

                    // Split cases based upon the storage type of the right arg
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:    // Res = BOOL, Lft = RAT(S), Rht = BOOL(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the BOOL to a RAT
                                mpq_set_sx (&aplRatRht, BIT0 & ((*(LPAPLBOOL) lpMemRht) >> uBitIndex), 1);

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisRvR) (*(LPAPLRAT) lpSymGlbLft,
                                                          aplRatRht,
                                                          lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRht)++;       // Skip to next byte
                                    ((LPAPLBOOL) lpMemRes)++;       // ...
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = BOOL, Lft = RAT(S), Rht = INT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the INT to a RAT
                                mpq_set_sx (&aplRatRht, *((LPAPLINT) lpMemRht)++, 1);

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisRvR) (*(LPAPLRAT) lpSymGlbLft,
                                                          aplRatRht,
                                                          lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_APA:     // Res = BOOL, Lft = RAT(S), Rht = APA(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the APA to a RAT
                                mpq_set_sx (&aplRatRht, apaOffRht + apaMulRht * uRes, 1);

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisRvR) (*(LPAPLRAT) lpSymGlbLft,
                                                          aplRatRht,
                                                          lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = BOOL, Lft = RAT(S), Rht = FLOAT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the RAT to a VFP
                                mpfr_set_q (&aplVfpLft, (LPAPLRAT) lpSymGlbLft, MPFR_RNDN);

                                // Convert the FLOAT to a VFP
                                mpfr_set_d (&aplVfpRht, *((LPAPLFLOAT) lpMemRht)++, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                         aplVfpRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_RAT:     // Res = BOOL, Lft = RAT(S), Rht = RAT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisRvR) (*(LPAPLRAT) lpSymGlbLft,
                                                         *((LPAPLRAT) lpMemRht)++,
                                                          lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_VFP:     // Res = BOOL, Lft = RAT(S), Rht = VFP(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the RAT to a VFP
                                mpfr_set_q (&aplVfpLft, (LPAPLRAT) lpSymGlbLft, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                        *((LPAPLVFP) lpMemRht)++,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    // We no longer need this storage
                    Myf_clear (&aplVfpRht);
                    Myf_clear (&aplVfpLft);
                    Myq_clear (&aplRatRht);

                    break;

                case ARRAY_VFP:             // Res = BOOL, Lft = VFP  (S)
                    // Initialize the temps
                    mpfr_init0 (&aplVfpLft);
                    mpfr_init0 (&aplVfpRht);

                    // Split cases based upon the storage type of the right arg
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:    // Res = BOOL, Lft = VFP(S), Rht = BOOL(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the BOOL to a VFP
                                mpfr_set_sx (&aplVfpRht, BIT0 & ((*(LPAPLBOOL) lpMemRht) >> uBitIndex), MPFR_RNDN);

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisVvV) (*(LPAPLVFP) lpSymGlbLft,
                                                          aplVfpRht,
                                                          lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRht)++;       // Skip to next byte
                                    ((LPAPLBOOL) lpMemRes)++;       // ...
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = BOOL, Lft = VFP(S), Rht = INT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the INT to a VFP
                                mpfr_set_sx (&aplVfpRht, *((LPAPLINT) lpMemRht)++, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisVvV) (*(LPAPLVFP) lpSymGlbLft,
                                                          aplVfpRht,
                                                          lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_APA:     // Res = BOOL, Lft = VFP(S), Rht = APA(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the APA to a VFP
                                mpfr_set_sx (&aplVfpRht, apaOffRht + apaMulRht * uRes, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisVvV) (*(LPAPLVFP) lpSymGlbLft,
                                                          aplVfpRht,
                                                          lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = BOOL, Lft = VFP(S), Rht = FLOAT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the FLOAT to a VFP
                                mpfr_set_d (&aplVfpRht, *((LPAPLFLOAT) lpMemRht)++, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisVvV) (*(LPAPLVFP) lpSymGlbLft,
                                                          aplVfpRht,
                                                          lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_RAT:     // Res = BOOL, Lft = VFP(S), Rht = RAT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the RAT to a VFP
                                mpfr_set_q (&aplVfpRht, ((LPAPLRAT) lpMemRht)++, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisVvV) (*(LPAPLVFP) lpSymGlbLft,
                                                          aplVfpRht,
                                                          lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_VFP:     // Res = BOOL, Lft = VFP(S), Rht = VFP(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisVvV) (*(LPAPLVFP) lpSymGlbLft,
                                                         *((LPAPLVFP) lpMemRht)++,
                                                          lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    // We no longer need this storage
                    Myf_clear (&aplVfpRht);
                    Myf_clear (&aplVfpLft);

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_INT:                     // Res = INT
            // Split cases based upon the storage type of the left arg
            switch (aplTypeLft)
            {
                case ARRAY_BOOL:            // Res = INT, Lft = BOOL(S)
                case ARRAY_INT:             // Res = INT, Lft = INT(S)
                case ARRAY_APA:             // Res = INT, Lft = APA(S)
                    // Split cases based upon the storage type of the right arg
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:    // Res = INT, Lft = BOOL/INT(S), Rht = BOOL(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisIvI) (aplIntegerLft,
                                                         BIT0 & ((*(LPAPLBOOL) lpMemRht) >> uBitIndex),
                                                         lpPrimSpec);
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRht)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = INT, Lft = BOOL/INT(S), Rht = INT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisIvI) (aplIntegerLft,
                                                         *((LPAPLINT) lpMemRht)++,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_APA:     // Res = INT, Lft = BOOL/INT(S), Rht = APA(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisIvI) (aplIntegerLft,
                                                         apaOffRht + apaMulRht * uRes,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = INT, Lft = BOOL/INT(S), Rht = FLOAT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLINT) lpMemRes)++ =
                                  (*lpPrimSpec->IisFvF) (aplFloatLft,
                                                         *((LPAPLFLOAT) lpMemRht)++,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case ARRAY_FLOAT:           // Res = INT, Lft = FLOAT(S)
                    // Split cases based upon the storage type of the right arg
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:    // Res = INT, Lft = FLOAT(S), Rht = BOOL(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLINT) lpMemRes)++ =
                                  (*lpPrimSpec->IisFvF) (aplFloatLft,
                                                         BIT0 & ((*(LPAPLBOOL) lpMemRht) >> uBitIndex),
                                                         lpPrimSpec);
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRht)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = INT, Lft = FLOAT(S), Rht = INT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisFvF) (aplFloatLft,
                                                         (APLFLOAT) *((LPAPLINT) lpMemRht)++,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_APA:     // Res = INT, Lft = FLOAT(S), Rht = APA(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisFvF) (aplFloatLft,
                                                         (APLFLOAT) (apaOffRht + apaMulRht * uRes),
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = INT, Lft = FLOAT(S), Rht = FLOAT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisFvF) (aplFloatLft,
                                                         *((LPAPLFLOAT) lpMemRht)++,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_FLOAT:                   // Res = FLOAT
            // Split cases based upon the storage type of the left arg
            switch (aplTypeLft)
            {
                case ARRAY_BOOL:            // Res = FLOAT, Lft = BOOL(S)
                case ARRAY_INT:             // Res = FLOAT, Lft = INT(S)
                case ARRAY_APA:             // Res = FLOAT, Lft = APA(S)
                    // Split cases based upon the storage type of the right arg
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:    // Res = FLOAT, Lft = BOOL/INT/APA(S), Rht = BOOL(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisIvI) (aplIntegerLft,
                                                         BIT0 & ((*(LPAPLBOOL) lpMemRht) >> uBitIndex),
                                                         lpPrimSpec);
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRht)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = FLOAT, Lft = BOOL/INT/APA(S), Rht = INT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisIvI) (aplIntegerLft,
                                                         *((LPAPLINT) lpMemRht)++,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_APA:     // Res = FLOAT, Lft = BOOL/INT/APA(S), Rht = APA(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisIvI) (aplIntegerLft,
                                                         apaOffRht + apaMulRht * uRes,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = FLOAT, Lft = BOOL/INT/APA(S), Rht = FLOAT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisFvF) (aplFloatLft,
                                                         *((LPAPLFLOAT) lpMemRht)++,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case ARRAY_FLOAT:           // Res = FLOAT, Lft = FLOAT(S)
                    // Split cases based upon the storage type of the right arg
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:    // Res = FLOAT, Lft = FLOAT(S), Rht = BOOL(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisFvF) (aplFloatLft,
                                                         BIT0 & ((*(LPAPLBOOL) lpMemRht) >> uBitIndex),
                                                         lpPrimSpec);
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRht)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = FLOAT, Lft = FLOAT(S), Rht = INT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisFvF) (aplFloatLft,
                                                         (APLFLOAT) *((LPAPLINT) lpMemRht)++,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_APA:     // Res = FLOAT, Lft = FLOAT(S), Rht = APA(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisFvF) (aplFloatLft,
                                                         (APLFLOAT) (apaOffRht + apaMulRht * uRes),
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = FLOAT, Lft = FLOAT(S), Rht = FLOAT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisFvF) (aplFloatLft,
                                                         *((LPAPLFLOAT) lpMemRht)++,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_RAT:     // Res = FLOAT, Lft = FLOAT(S), Rht = RAT  (M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisFvF) (aplFloatLft,
                                                         mpq_get_d (((LPAPLRAT) lpMemRht)++),
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_VFP:     // Res = FLOAT, Lft = FLOAT(S), Rht = VFP  (M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisFvF) (aplFloatLft,
                                                         mpfr_get_d (((LPAPLVFP) lpMemRht)++, MPFR_RNDN),
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case ARRAY_RAT:             // Res = FLOAT, Lft = RAT  (S)
                    // Split cases based upon the storage type of the right arg
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:    // Res = FLOAT, Lft = RAT  (S), Rht = BOOL(M)
                            DbgBrk ();      // Can't happen with any known primitive

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisFvF) (mpq_get_d ((LPAPLRAT) lpSymGlbLft),
                                                         BIT0 & ((*(LPAPLBOOL) lpMemRht) >> uBitIndex),
                                                         lpPrimSpec);
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRht)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = FLOAT, Lft = RAT  (S), Rht = INT(M)
                            DbgBrk ();      // Can't happen with any known primitive

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisFvF) (mpq_get_d ((LPAPLRAT) lpSymGlbLft),
                                                         (APLFLOAT) *((LPAPLINT) lpMemRht)++,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = FLOAT, Lft = RAT  (S), Rht = FLOAT(M)
                            DbgBrk ();      // Can't happen with any known primitive

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisFvF) (mpq_get_d ((LPAPLRAT) lpSymGlbLft),
                                                        *((LPAPLFLOAT) lpMemRht)++,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_RAT:     // Res = FLOAT, Lft = RAT  (S), Rht = RAT  (M)
                            DbgBrk ();      // Can't happen with any known primitive

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisFvF) (mpq_get_d (( LPAPLRAT) lpSymGlbLft),
                                                         mpq_get_d (((LPAPLRAT) lpMemRht)++),
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_VFP:     // Res = FLOAT, Lft = RAT  (S), Rht = VFP  (M)
                            DbgBrk ();      // Can't happen with any known primitive

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisFvF) (mpq_get_d (( LPAPLRAT) lpSymGlbLft),
                                                         mpfr_get_d (((LPAPLVFP) lpMemRht)++, MPFR_RNDN),
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_RAT:                     // Res = RAT
            // Split cases based upon the storage type of the left arg
            switch (aplTypeLft)
            {
                case ARRAY_BOOL:            // Res = RAT, Lft = BOOL(S)
                case ARRAY_INT:             // Res = RAT, Lft = INT(S)
                case ARRAY_APA:             // Res = RAT, Lft = APA(S)
                    // Convert the INT to a RAT
                    mpq_init_set_sx (&aplRatLft, aplIntegerLft, 1);

                    // Split cases based upon the storage type of the right arg
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:    // Res = RAT, Lft = BOOL/INT/APA(S), Rht = BOOL(M)
                        case ARRAY_INT:     // Res = RAT, Lft = BOOL/INT/APA(S), Rht = INT (M)
                        case ARRAY_APA:     // Res = RAT, Lft = BOOL/INT/APA(S), Rht = APA (M)
                            // Initialize the temp
                            mpq_init (&aplRatRht);

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the INT to a RAT
                                mpq_set_sx (&aplRatRht,
                                             GetNextInteger (lpMemRht, aplTypeRht, uRes),
                                             1);
                                // Save in the result
                                *((LPAPLRAT) lpMemRes)++ =
                                  (*lpPrimSpec->RisRvR) (aplRatLft,
                                                         aplRatRht,
                                                         lpPrimSpec);
                            } // End FOR

                            // We no longer need this storage
                            Myq_clear (&aplRatRht);

                            break;

                        case ARRAY_FLOAT:   // Res = RAT, Lft = BOOL/INT/APA(S), Rht = FLOAT(M)
                            // Initialize the temp
                            mpq_init (&aplRatRht);

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the FLOAT to a RAT
                                mpq_set_d  (&aplRatRht,
                                             GetNextFloat   (lpMemRht, aplTypeRht, uRes));
                                // Save in the result
                                *((LPAPLRAT) lpMemRes)++ =
                                  (*lpPrimSpec->RisRvR) (aplRatLft,
                                                         aplRatRht,
                                                         lpPrimSpec);
                            } // End FOR

                            // We no longer need this storage
                            Myq_clear (&aplRatRht);

                            break;

                        case ARRAY_RAT:     // Res = RAT, Lft = BOOL/INT/APA(S), Rht = RAT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLRAT) lpMemRes)++ =
                                  (*lpPrimSpec->RisRvR) (aplRatLft,
                                                         *((LPAPLRAT) lpMemRht)++,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    // We no longer need this storage
                    Myq_clear (&aplRatLft);

                    break;

                case ARRAY_FLOAT:           // Res = RAT, Lft = FLOAT(S)
                    // Convert the FLOAT to a RAT
                    mpq_init_set_d  (&aplRatLft, aplFloatLft);

                    // Split cases based upon the storage type of the right arg
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:    // Res = RAT, Lft = FLOAT(S), Rht = BOOL(M)
                        case ARRAY_INT:     // Res = RAT, Lft = FLOAT(S), Rht = INT (M)
                        case ARRAY_APA:     // Res = RAT, Lft = FLOAT(S), Rht = APA (M)
                            // Initialize the temp
                            mpq_init (&aplRatRht);

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the INT to a RAT
                                mpq_set_sx (&aplRatRht,
                                             GetNextInteger (lpMemRht, aplTypeRht, uRes),
                                             1);
                                // Save in the result
                                *((LPAPLRAT) lpMemRes)++ =
                                  (*lpPrimSpec->RisRvR) (aplRatLft,
                                                         aplRatRht,
                                                         lpPrimSpec);
                            } // End FOR

                            // We no longer need this storage
                            Myq_clear (&aplRatRht);

                            break;

                        case ARRAY_FLOAT:   // Res = RAT, Lft = FLOAT(S), Rht = FLOAT(M)
                            // Initialize the temp
                            mpq_init (&aplRatRht);

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the FLOAT to a RAT
                                mpq_set_d  (&aplRatRht,
                                             GetNextFloat   (lpMemRht, aplTypeRht, uRes));
                                // Save in the result
                                *((LPAPLRAT) lpMemRes)++ =
                                  (*lpPrimSpec->RisRvR) (aplRatLft,
                                                         aplRatRht,
                                                         lpPrimSpec);
                            } // End FOR

                            // We no longer need this storage
                            Myq_clear (&aplRatRht);

                            break;

                        case ARRAY_RAT:     // Res = RAT, Lft = FLOAT(S), Rht = RAT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLRAT) lpMemRes)++ =
                                  (*lpPrimSpec->RisRvR) (aplRatLft,
                                                         *((LPAPLRAT) lpMemRht)++,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    // We no longer need this storage
                    Myq_clear (&aplRatLft);

                    break;

                case ARRAY_RAT:             // Res = RAT
                    // Split cases based upon the storage type of the right arg
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:    // Res = RAT, Lft = RAT(S), Rht = BOOL(M)
                        case ARRAY_INT:     // Res = RAT, Lft = RAT(S), Rht = INT (M)
                        case ARRAY_APA:     // Res = RAT, Lft = RAT(S), Rht = APA (M)
                            // Initialize the temp
                            mpq_init (&aplRatRht);

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the INT to a RAT
                                mpq_set_sx (&aplRatRht,
                                             GetNextInteger (lpMemRht, aplTypeRht, uRes),
                                             1);
                                // Save in the result
                                *((LPAPLRAT) lpMemRes)++ =
                                  (*lpPrimSpec->RisRvR) (*(LPAPLRAT) lpSymGlbLft,
                                                         aplRatRht,
                                                         lpPrimSpec);
                            } // End FOR

                            // We no longer need this storage
                            Myq_clear (&aplRatRht);

                            break;

                        case ARRAY_RAT:     // Res = RAT, Lft = RAT(S), Rht = RAT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLRAT) lpMemRes)++ =
                                  (*lpPrimSpec->RisRvR) (*(LPAPLRAT) lpSymGlbLft,
                                                         *((LPAPLRAT) lpMemRht)++,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_VFP:                     // Res = VFP
            // Initialize the temps
            mpfr_init0 (&aplVfpLft);
            mpfr_init0 (&aplVfpRht);

            // Split cases based upon the storage type of the left arg
            switch (aplTypeLft)
            {
                case ARRAY_BOOL:            // Res = VFP, Lft = BOOL(S)
                case ARRAY_INT:             // Res = VFP, Lft = INT(S)
                case ARRAY_APA:             // Res = VFP, Lft = APA(S)
                    // Convert the INT to a VFP
                    mpfr_set_sx (&aplVfpLft, aplIntegerLft, MPFR_RNDN);

                    // Split cases based upon the storage type of the right arg
                    switch (aplTypeRht)
                    {
                        case ARRAY_RAT:     // Res = VFP, Lft = BOOL/INT/APA(S), Rht = RAT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the RAT to a VFP
                                mpfr_set_q (&aplVfpRht, ((LPAPLRAT) lpMemRht)++, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLVFP) lpMemRes)++ =
                                  (*lpPrimSpec->VisVvV) (aplVfpLft,
                                                         aplVfpRht,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_VFP:     // Res = VFP, Lft = BOOL/INT/APA(S), Rht = VFP(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLVFP) lpMemRes)++ =
                                  (*lpPrimSpec->VisVvV) (aplVfpLft,
                                                        *((LPAPLVFP) lpMemRht)++,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case ARRAY_FLOAT:           // Res = VFP, Lft = FLOAT(S)
                    // Convert the FLOAT to a VFP
                    mpfr_set_d (&aplVfpLft, aplFloatLft, MPFR_RNDN);

                    // Split cases based upon the storage type of the right arg
                    switch (aplTypeRht)
                    {
                        case ARRAY_RAT:     // Res = VFP, Lft = FLOAT(S), Rht = RAT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the RAT to a VFP
                                mpfr_set_q (&aplVfpRht, ((LPAPLRAT) lpMemRht)++, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLVFP) lpMemRes)++ =
                                  (*lpPrimSpec->VisVvV) (aplVfpLft,
                                                         aplVfpRht,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_VFP:     // Res = VFP, Lft = FLOAT(S), Rht = VFP(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLVFP) lpMemRes)++ =
                                  (*lpPrimSpec->VisVvV) (aplVfpLft,
                                                        *((LPAPLVFP) lpMemRht)++,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case ARRAY_RAT:             // Res = VFP, Lft = RAT(S)
                    // Split cases based upon the storage type of the right arg
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:    // Res = VFP, Lft = RAT(S), Rht = BOOL(M)
                        case ARRAY_INT:     // Res = VFP, Lft = RAT(S), Rht = INT (M)
                        case ARRAY_APA:     // Res = VFP, Lft = RAT(S), Rht = APA (M)
                            // Convert the RAT to a VFP
                            mpfr_set_q (&aplVfpLft, (LPAPLRAT) lpSymGlbLft, MPFR_RNDN);

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the INT to a VFP
                                mpfr_set_sx (&aplVfpRht,
                                              GetNextInteger (lpMemRht, aplTypeRht, uRes),
                                              MPFR_RNDN);
                                // Save in the result
                                *((LPAPLVFP) lpMemRes)++ =
                                  (*lpPrimSpec->VisVvV) (aplVfpLft,
                                                         aplVfpRht,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = VFP, Lft = RAT(S), Rht = FLOAT(M)
                            // Convert the RAT to a VFP
                            mpfr_set_q (&aplVfpLft, (LPAPLRAT) lpSymGlbLft, MPFR_RNDN);

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the FLOAT to a VFP
                                mpfr_set_d (&aplVfpRht,
                                             *((LPAPLFLOAT) lpMemRht)++,
                                             MPFR_RNDN);
                                // Save in the result
                                *((LPAPLVFP) lpMemRes)++ =
                                  (*lpPrimSpec->VisVvV) (aplVfpLft,
                                                         aplVfpRht,
                                                         lpPrimSpec);
                            } // End FOR


                            break;

                        case ARRAY_RAT:     // Res = VFP, Lft = RAT(S), Rht = RAT(M)
                            // Convert the RAT to a VFP
                            mpfr_set_q (&aplVfpLft, (LPAPLRAT) lpSymGlbLft, MPFR_RNDN);

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the RAT to a VFP
                                mpfr_set_q (&aplVfpRht, ((LPAPLRAT) lpMemRht)++, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLVFP) lpMemRes)++ =
                                  (*lpPrimSpec->VisVvV) (aplVfpLft,
                                                         aplVfpRht,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_VFP:     // Res = VFP, Lft = RAT(S), Rht = VFP(M)
                            // Convert the RAT to a VFP
                            mpfr_set_q (&aplVfpLft, (LPAPLRAT) lpSymGlbLft, MPFR_RNDN);

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLVFP) lpMemRes)++ =
                                  (*lpPrimSpec->VisVvV) (aplVfpLft,
                                                        *((LPAPLVFP) lpMemRht)++,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case ARRAY_VFP:             // Res = VFP, Lft = VFP(S)
                    // Split cases based upon the storage type of the right arg
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:    // Res = VFP, Lft = VFP(S), Rht = BOOL(M)
                        case ARRAY_INT:     // Res = VFP, Lft = VFP(S), Rht = INT (M)
                        case ARRAY_APA:     // Res = VFP, Lft = VFP(S), Rht = APA (M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the INT to a VFP
                                mpfr_set_sx (&aplVfpRht,
                                              GetNextInteger (lpMemRht, aplTypeRht, uRes),
                                              MPFR_RNDN);
                                // Save in the result
                                *((LPAPLVFP) lpMemRes)++ =
                                  (*lpPrimSpec->VisVvV) (*(LPAPLVFP) lpSymGlbLft,
                                                          aplVfpRht,
                                                          lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = VFP, Lft = VFP(S), Rht = FLOAT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the FLOAT to a VFP
                                mpfr_set_d (&aplVfpRht,
                                            *((LPAPLFLOAT) lpMemRht)++,
                                            MPFR_RNDN);
                                // Save in the result
                                *((LPAPLVFP) lpMemRes)++ =
                                  (*lpPrimSpec->VisVvV) (*(LPAPLVFP) lpSymGlbLft,
                                                         aplVfpRht,
                                                         lpPrimSpec);
                            } // End FOR


                            break;

                        case ARRAY_RAT:     // Res = VFP, Lft = VFP(S), Rht = RAT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the RAT to a VFP
                                mpfr_set_q (&aplVfpRht, ((LPAPLRAT) lpMemRht)++, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLVFP) lpMemRes)++ =
                                  (*lpPrimSpec->VisVvV) (*(LPAPLVFP) lpSymGlbLft,
                                                         aplVfpRht,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_VFP:     // Res = VFP, Lft = VFP(S), Rht = VFP(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLVFP) lpMemRes)++ =
                                  (*lpPrimSpec->VisVvV) (*(LPAPLVFP) lpSymGlbLft,
                                                        *((LPAPLVFP) lpMemRht)++,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                defstop
                    break;
            } // End SWITCH

            // We no longer need this storage
            Myf_clear (&aplVfpRht);
            Myf_clear (&aplVfpLft);

            break;

        defstop
            break;
    } // End SWITCH
    } __except (CheckException (GetExceptionInformation (), L"PrimFnDydSingMult_EM"))
    {
        EXCEPTION_CODES ExceptionCode = MyGetExceptionCode ();  // The exception code

        dprintfWL9 (L"!!Initiating Exception in " APPEND_NAME L" #5: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

        // Split cases based upon the ExceptionCode
        switch (ExceptionCode)
        {
            case EXCEPTION_DOMAIN_ERROR:
            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            case EXCEPTION_INT_DIVIDE_BY_ZERO:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                goto DOMAIN_EXIT;

            case EXCEPTION_WS_FULL:
                goto WSFULL_EXIT;

            case EXCEPTION_NONCE_ERROR:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                goto NONCE_EXIT;

            case EXCEPTION_RESULT_RAT:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                if (IsNumeric (aplTypeRes)
                 && !IsRat    (aplTypeRes))
                {
                    // It's now a RAT result
                    aplTypeRes = ARRAY_RAT;

                    // We need to start over with the result
                    MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
                    FreeResultGlobalVar (*lphGlbRes); *lphGlbRes = NULL;

                    if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                                     lphGlbRes,
                                                     NULL,
                                                     lpMemHdrRht,
                                                     0,
                                                     lpMemHdrRht->Rank,
                                                    &aplRankRes,
                                                     aplTypeRes,
                                                     bLftIdent,
                                                     bRhtIdent,
                                                     1,
                                                     lpMemHdrRht->NELM,
                                                     aplNELMRes))
                        goto ERROR_EXIT;

                    // Lock the memory to get a ptr to it
                    lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);

                    // Restart the pointer
                    lpMemRht = lpMemRhtStart;

                    dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #5: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                    goto RESTART_EXCEPTION;
                } // End IF

                // Display message for unhandled exception
                DisplayException ();

                break;

            case EXCEPTION_RESULT_VFP:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                if (IsNumeric (aplTypeRes)
                 && !IsVfp    (aplTypeRes))
                {
                    // It's now a VFP result
                    aplTypeRes = ARRAY_VFP;

                    // We need to start over with the result
                    MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
                    FreeResultGlobalVar (*lphGlbRes); *lphGlbRes = NULL;

                    if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                                     lphGlbRes,
                                                     NULL,
                                                     lpMemHdrRht,
                                                     0,
                                                     lpMemHdrRht->Rank,
                                                    &aplRankRes,
                                                     aplTypeRes,
                                                     bLftIdent,
                                                     bRhtIdent,
                                                     1,
                                                     lpMemHdrRht->NELM,
                                                     aplNELMRes))
                        goto ERROR_EXIT;

                    // Lock the memory to get a ptr to it
                    lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);

                    // Restart the pointer
                    lpMemRht = lpMemRhtStart;

                    dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #5: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                    goto RESTART_EXCEPTION;
                } // End IF

                // Display message for unhandled exception
                DisplayException ();

                break;

            case EXCEPTION_RESULT_FLOAT:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                if (IsNumeric (aplTypeRes)
                 && !IsSimpleFlt (aplTypeRes))
                {
                    // If the previous result is Boolean, we need to
                    //   unlock, free, and allocate the result anew
                    if (IsSimpleBool (aplTypeRes))
                    {
                        // It's now a FLOAT result
                        aplTypeRes = ARRAY_FLOAT;

                        // We need to start over with the result
                        MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
                        FreeResultGlobalVar (*lphGlbRes); *lphGlbRes = NULL;

                        if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                                         lphGlbRes,
                                                         NULL,
                                                         lpMemHdrRht,
                                                         0,
                                                         lpMemHdrRht->Rank,
                                                        &aplRankRes,
                                                         aplTypeRes,
                                                         bLftIdent,
                                                         bRhtIdent,
                                                         1,
                                                         lpMemHdrRht->NELM,
                                                         aplNELMRes))
                            goto ERROR_EXIT;

                        // Lock the memory to get a ptr to it
                        lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);
                    } else
                    // The previous result must have been INT which is
                    //   the same size as FLOAT, so there's no need to
                    //   change storage.
                    {
                        // It's now a FLOAT result
                        aplTypeRes = ARRAY_FLOAT;

                        // If the result is not nested, ...
                        if (!IsNested (lpMemHdrRes->ArrType))
                            // Tell the header about it
                            lpMemHdrRes->ArrType = aplTypeRes;

                        // Restart the pointer
                        lpMemRes = lpMemHdrRes;
                    } // End IF/ELSE

                    // Restart the pointer
                    lpMemRht = lpMemRhtStart;

                    dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #5: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                    goto RESTART_EXCEPTION;
                } // End IF

                // Fall through to never-never-land

            default:
                // Display message for unhandled exception
                DisplayException ();

                break;
        } // End SWITCH
    } // End __try/__except

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    // We no longer need this storage
    Myq_clear (&aplRatRht);
    Myq_clear (&aplRatLft);
    Myf_clear (&aplVfpRht);
    Myf_clear (&aplVfpLft);

    // Restore the memory ptr
    *lplpMemRes = lpMemRes;

    return bRet;
} // End PrimFnDydSingMult_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydMultSing_EM
//
//  Primitive scalar dyadic function,
//    left simple multipleton, right simple singleton
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydMultSing_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydMultSing_EM
    (HGLOBAL          *lphGlbRes,           // Ptr to result handle
     APLSTYPE          aplTypeRes,          // Result type
     LPVARARRAY_HEADER lpMemHdrRes,         // Ptr to result header (in case we blow up)
     LPVOID           *lplpMemRes,          // Ptr to ptr to result memory (Points to the data)
     APLNELM           aplNELMRes,          // Result NELM
     APLSTYPE          aplTypeLft,          // Left arg type
     APLINT            apaOffLft,           // ...      APA offset
     APLINT            apaMulLft,           // ...      ... multiplier
     LPVARARRAY_HEADER lpMemHdrLft,         // Ptr to left arg header (in case we blow up)
     LPVOID            lpMemLft,            // Points to the data
     APLSTYPE          aplTypeRht,          // Right arg type
     APLINT            aplIntegerRht,       // ...       integer value
     APLFLOAT          aplFloatRht,         // ...       float   ...
     APLCHAR           aplCharRht,          // ...       char    ...
     HGLOBAL           lpSymGlbRht,         // ...       lpSymGlb
     UBOOL             bLftIdent,           // TRUE iff the function has a left identity element and the Axis tail is valid
     UBOOL             bRhtIdent,           // ...                         right ...
     LPTOKEN           lptkFunc,            // Ptr to function token
     LPPRIMSPEC        lpPrimSpec)          // Ptr to local PRIMSPEC

{
    APLINT        uRes;                     // Loop counter
    UBOOL         bRet = FALSE;             // TRUE iff the result is valid
    APLRANK       aplRankRes;               // Temp var for DydAllocate
    APLNELM       aplNELMTmp,               // Temporary NELM
                  aplNELMRem;               // Remaining NELM
    UINT          uBitIndex = 0;
    LPVOID        lpMemLftStart,
                  lpMemRes;
    APLSTYPE      aplTypeHetLft;
    APLINT        aplIntegerLft;
    APLFLOAT      aplFloatLft;
    APLCHAR       aplCharLft;
    LPPLLOCALVARS lpplLocalVars;            // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;             // Ptr to Ctrl-Break flag
    APLRAT        aplRatLft = {0},          // Left arg as Rational
                  aplRatRht = {0};          // Right ...
    APLVFP        aplVfpLft = {0},          // Left arg as VFP
                  aplVfpRht = {0};          // Right ...
    HGLOBAL       lpSymGlbLft;              // Left arg global numeric memory handle

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the memory ptr
    lpMemRes = *lplpMemRes;

    // If the singleton is integer,
    //   attempt to demote it to Boolean
    if (aplTypeRht EQ ARRAY_INT
     && IsBooleanValue (aplIntegerRht))
        aplTypeRht = ARRAY_BOOL;

    // Save the starting values in case we need
    //   to restart from an exception
    lpMemLftStart = lpMemLft;
RESTART_EXCEPTION:
    // Skip over the header to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    __try
    {
    // Split cases based upon the result's storage type
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:                    // Res = BOOL
            // Split off character or heterogeneous (CH) arguments
            if (IsSimpleCH (aplTypeLft)
             || IsSimpleCH (aplTypeRht))
            {
                APLBOOL bCvN;

                Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                     || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);

                // Calculate the result of CvN
                bCvN = (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);

                // If both arguments are CHAR,
                //   use BisCvC
                if (IsSimpleChar (aplTypeLft)
                 && IsSimpleChar (aplTypeRht))
                {
                    // Loop through the result
                    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Save in the result
                        *((LPAPLBOOL)  lpMemRes) |=
                          (*lpPrimSpec->BisCvC) (*((LPAPLCHAR) lpMemLft)++,
                                                 aplCharRht,
                                                 lpPrimSpec) << uBitIndex;
                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;                  // Start over
                            ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                        } // End IF
                    } // End FOR
                } else
                // If the other argument is simple or global numeric,
                //   use BisCvN
                if ((IsNumeric (aplTypeLft) && IsSimpleChar (aplTypeRht))
                 || (IsNumeric (aplTypeRht) && IsSimpleChar (aplTypeLft)))
                {
                    // Loop through the result
                    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Save in the result
                        *((LPAPLBOOL)  lpMemRes) |= (bCvN << uBitIndex);

                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;                  // Start over
                            ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                        } // End IF
                    } // End FOR
                } else
                // If the multipleton argument is HETERO,
                //   look at each LPSYMENTRY
                if (IsSimpleHet (aplTypeLft))
                {
                    // Split cases based upon the right arg's storage type
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:    // Res = BOOL,                  Rht = BOOL (S)
                        case ARRAY_INT:     // Res = BOOL,                  Rht = INT  (S)
                        case ARRAY_APA:     // Res = BOOL,                  Rht = APA  (S)
                            // Convert the BOOL/INT to a RAT
                            mpq_init_set_sx (&aplRatRht, aplIntegerRht, 1);
                            mpfr_init_set_sx (&aplVfpRht, aplIntegerRht, MPFR_RNDN);

                            // Loop through the left argument
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                aplTypeHetLft = GetNextHetero (lpMemLft, uRes, &aplIntegerLft, &aplFloatLft, &aplCharLft, &lpSymGlbLft);

                                // Split cases based upon the left arg's item's storage type
                                switch (aplTypeHetLft)
                                {
                                    case ARRAY_BOOL:    // Res = BOOL, Lft = HETERO:BOOL (M), Rht = BOOL/INT/APA (S)
                                    case ARRAY_INT:     // Res = BOOL, Lft = HETERO:INT  (M), Rht = BOOL/INT/APA (S)
                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisIvI) (aplIntegerLft,
                                                                 aplIntegerRht,
                                                                 lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_FLOAT:   // Res = BOOL, Lft = HETERO:FLOAT(M), Rht = BOOL/INT/APA (S)
                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                                 aplFloatRht,
                                                                 lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_CHAR:    // Res = BOOL, Lft = HETERO:CHAR (M), Rht = BOOL/INT/APA (S)
                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |= (bCvN << uBitIndex);

                                        break;

                                    case ARRAY_RAT:     // Res = BOOL, Lft = HETERO:RAT  (M), Rht = BOOL/INT/APA (S)
                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisRvR) (*(LPAPLRAT) lpSymGlbLft,
                                                                  aplRatRht,
                                                                  lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_VFP:     // Res = BOOL, Lft = HETERO:VFP  (M), Rht = BOOL/INT/APA (S)
                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisVvV) (*(LPAPLVFP) lpSymGlbLft,
                                                                  aplVfpRht,
                                                                  lpPrimSpec) << uBitIndex;
                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            // We no longer need this storage
                            Myf_clear (&aplVfpRht);
                            Myq_clear (&aplRatRht);

                            break;

                        case ARRAY_FLOAT:   // Res = BOOL,                  Rht = FLOAT(S)
                            // Convert the FLOAT to a RAT
                            mpq_init_set_d (&aplRatRht, aplFloatRht);
                            mpfr_init_set_d (&aplVfpRht, aplFloatRht, MPFR_RNDN);

                            // Loop through the left argument
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                aplTypeHetLft = GetNextHetero (lpMemLft, uRes, &aplIntegerLft, &aplFloatLft, &aplCharLft, &lpSymGlbLft);

                                // Split cases based upon the left arg's item's storage type
                                switch (aplTypeHetLft)
                                {
                                    case ARRAY_BOOL:    // Res = BOOL, Lft = HETERO:BOOL (M), Rht = FLOAT (S)
                                    case ARRAY_INT:     // Res = BOOL, Lft = HETERO:INT  (M), Rht = FLOAT (S)
                                    case ARRAY_FLOAT:   // Res = BOOL, Lft = HETERO:FLOAT(M), Rht = FLOAT (S)
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                                 aplFloatRht,
                                                                 lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_CHAR:    // Res = BOOL, Lft = HETERO:CHAR (M), Rht = FLOAT (S)
                                        *((LPAPLBOOL)  lpMemRes) |= (bCvN << uBitIndex);

                                        break;

                                    case ARRAY_RAT:     // Res = BOOL, Lft = HETERO:RAT  (M), Rht = FLOAT (S)
                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisRvR) (*(LPAPLRAT) lpSymGlbLft,
                                                                  aplRatRht,
                                                                  lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_VFP:     // Res = BOOL, Lft = HETERO:VFP  (M), Rht = FLOAT (S)
                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisVvV) (*(LPAPLVFP) lpSymGlbLft,
                                                                  aplVfpRht,
                                                                  lpPrimSpec) << uBitIndex;
                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            // We no longer need this storage
                            Myf_clear (&aplVfpRht);
                            Myq_clear (&aplRatRht);

                            break;

                        case ARRAY_CHAR:    // Res = BOOL,                  Rht = CHAR (S)
                            // Loop through the left argument
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                aplTypeHetLft = GetNextHetero (lpMemLft, uRes, &aplIntegerLft, &aplFloatLft, &aplCharLft, NULL);

                                // Split cases based upon the left arg's item's storage type
                                switch (aplTypeHetLft)
                                {
                                    case ARRAY_BOOL:    // Res = BOOL, Lft = HETERO:BOOL (M), Rht = CHAR  (S)
                                    case ARRAY_INT:     // Res = BOOL, Lft = HETERO:INT  (M), Rht = CHAR  (S)
                                    case ARRAY_FLOAT:   // Res = BOOL, Lft = HETERO:FLOAT(M), Rht = CHAR  (S)
                                    case ARRAY_RAT:     // Res = BOOL, Lft = HETERO:RAT  (M), Rht = CHAR  (S)
                                    case ARRAY_VFP:     // Res = BOOL, Lft = HETERO:VFP  (M), Rht = CHAR  (S)
                                        *((LPAPLBOOL)  lpMemRes) |= (bCvN << uBitIndex);

                                        break;

                                    case ARRAY_CHAR:    // Res = BOOL, Lft = HETERO:CHAR (M), Rht = CHAR  (S)
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisCvC) (aplCharLft,
                                                                 aplCharRht,
                                                                 lpPrimSpec) << uBitIndex;
                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_RAT:     // Res = BOOL,                  Rht = RAT  (S)
                            // Initialize the temps
                            mpq_init (&aplRatLft);
                            mpfr_init0 (&aplVfpLft);
                            mpfr_init0 (&aplVfpRht);

                            // Loop through the left argument
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                aplTypeHetLft = GetNextHetero (lpMemLft, uRes, &aplIntegerLft, &aplFloatLft, &aplCharLft, &lpSymGlbLft);

                                // Split cases based upon the left arg's item's storage type
                                switch (aplTypeHetLft)
                                {
                                    case ARRAY_BOOL:    // Res = BOOL, Lft = HETERO:BOOL (M), Rht = RAT (S)
                                    case ARRAY_INT:     // Res = BOOL, Lft = HETERO:INT  (M), Rht = RAT (S)
                                        mpq_set_sx (&aplRatLft, aplIntegerLft, 1);

                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisRvR) (aplRatLft,
                                                                *(LPAPLRAT) lpSymGlbRht,
                                                                 lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_CHAR:    // Res = BOOL, Lft = HETERO:CHAR (M), Rht = RAT (S)
                                        *((LPAPLBOOL)  lpMemRes) |= (bCvN << uBitIndex);

                                        break;

                                    case ARRAY_FLOAT:   // Res = BOOL, Lft = HETERO:FLT  (M), Rht = RAT (S)
                                        // Convert the FLOAT to a VFP
                                        mpfr_set_d (&aplVfpLft, aplFloatLft, MPFR_RNDN);

                                        // Convert the RAT to a VFP
                                        mpfr_set_q (&aplVfpRht, (LPAPLRAT) lpSymGlbRht, MPFR_RNDN);

                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                                 aplVfpRht,
                                                                 lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_RAT:     // Res = BOOL, Lft = HETERO:RAT  (M), Rht = RAT (S)
                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisRvR) (*(LPAPLRAT) lpSymGlbLft,
                                                                 *(LPAPLRAT) lpSymGlbRht,
                                                                  lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_VFP:     // Res = BOOL, Lft = HETERO:VFP  (M), Rht = RAT (S)
                                        // Convert the RAT to a VFP
                                        mpfr_set_q (&aplVfpRht, (LPAPLRAT) lpSymGlbRht, MPFR_RNDN);

                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisVvV) (*(LPAPLVFP) lpSymGlbLft,
                                                                  aplVfpRht,
                                                                  lpPrimSpec) << uBitIndex;
                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            // We no longer need this storage
                            Myf_clear (&aplVfpRht);
                            Myf_clear (&aplVfpLft);
                            Myq_clear (&aplRatLft);

                            break;

                        case ARRAY_VFP:     // Res = BOOL,                  Rht = VFP  (S)
                            // Initialize the temp
                            mpfr_init0 (&aplVfpLft);

                            // Loop through the left argument
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                aplTypeHetLft = GetNextHetero (lpMemLft, uRes, &aplIntegerLft, &aplFloatLft, &aplCharLft, &lpSymGlbLft);

                                // Split cases based upon the left arg's item's storage type
                                switch (aplTypeHetLft)
                                {
                                    case ARRAY_BOOL:    // Res = BOOL, Lft = HETERO:BOOL (M), Rht = VFP (S)
                                    case ARRAY_INT:     // Res = BOOL, Lft = HETERO:INT  (M), Rht = VFP (S)
                                        mpfr_set_sx (&aplVfpLft, aplIntegerLft, MPFR_RNDN);

                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                                *(LPAPLVFP) lpSymGlbRht,
                                                                 lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_CHAR:    // Res = BOOL, Lft = HETERO:CHAR (M), Rht = VFP (S)
                                        *((LPAPLBOOL)  lpMemRes) |= (bCvN << uBitIndex);

                                        break;

                                    case ARRAY_FLOAT:   // Res = BOOL, Lft = HETERO:FLOAT(M), Rht = VFP (S)
                                        // Convert the FLOAT to a VFP
                                        mpfr_set_d (&aplVfpLft, aplFloatLft, MPFR_RNDN);

                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                                *(LPAPLVFP) lpSymGlbRht,
                                                                 lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_RAT:     // Res = BOOL, Lft = HETERO:RAT  (M), Rht = VFP (S)
                                        // Convert the RAT to a VFP
                                        mpfr_set_q (&aplVfpLft, (LPAPLRAT) lpSymGlbLft, MPFR_RNDN);

                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                                *(LPAPLVFP) lpSymGlbRht,
                                                                 lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_VFP:     // Res = BOOL, Lft = HETERO:VFP  (M), Rht = VFP (S)
                                        // Save in the result
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisVvV) (*(LPAPLVFP) lpSymGlbLft,
                                                                 *(LPAPLVFP) lpSymGlbRht,
                                                                  lpPrimSpec) << uBitIndex;
                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            // We no longer need this storage
                            Myf_clear (&aplVfpLft);

                            break;

                        defstop
                            break;
                    } // End SWITCH
                } else
                    DbgStop ();         // We should never get here
            } else
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:            // Res = BOOL,                  Rht = BOOL(S)
                    if (IsSimpleBool (aplTypeLft))
                    {
                        // Initialize # remaining NELM
                        aplNELMRem = aplNELMRes;

                        // Check for optimized chunking
                        if (lpPrimSpec->B64isB64vB64 NE NULL)
                        {
                            APLB64 aplB64Rht;

                            if (aplIntegerRht NE 0)
                                aplB64Rht = 0xFFFFFFFFFFFFFFFF;
                            else
                                aplB64Rht = 0x0000000000000000;

                            // Calculate the # 64-bit chunks
                            aplNELMTmp  = aplNELMRem / 64;
                            aplNELMRem -= aplNELMTmp * 64;

                            // Loop through the left arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB64) lpMemRes)++ =
                                  (*lpPrimSpec->B64isB64vB64) (*((LPAPLB64) lpMemLft)++, (APLB64) aplB64Rht, lpPrimSpec);
                            } // End FOR

                            // Calculate the # remaining 32-bit chunks
                            aplNELMTmp  = aplNELMRem / 32;
                            aplNELMRem -= aplNELMTmp * 32;

                            // Loop through the left arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB32) lpMemRes)++ =
                                  (*lpPrimSpec->B32isB32vB32) (*((LPAPLB32) lpMemLft)++, (APLB32) aplB64Rht, lpPrimSpec);
                            } // End FOR

                            // Calculate the # remaining 16-bit chunks
                            aplNELMTmp  = aplNELMRem / 16;
                            aplNELMRem -= aplNELMTmp * 16;

                            // Loop through the left arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB16) lpMemRes)++ =
                                  (*lpPrimSpec->B16isB16vB16) (*((LPAPLB16) lpMemLft)++, (APLB16) aplB64Rht, lpPrimSpec);
                            } // End FOR

                            // Calculate the # remaining  8-bit chunks
                            aplNELMTmp  = aplNELMRem /  8;
                            aplNELMRem -= aplNELMTmp *  8;

                            // Loop through the left arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB08) lpMemRes)++ =
                                  (*lpPrimSpec->B08isB08vB08) (*((LPAPLB08) lpMemLft)++, (APLB08) aplB64Rht, lpPrimSpec);
                            } // End FOR
                        } // End IF

                        // Loop through the left arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRem; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLBOOL)  lpMemRes) |=
                              (*lpPrimSpec->BisBvB) ((APLBOOL) (BIT0 & ((*(LPAPLBOOL) lpMemLft) >> uBitIndex)),
                                                     (APLBOOL) aplIntegerRht,
                                                     lpPrimSpec) << uBitIndex;
                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;                  // Start over
                                ((LPAPLBOOL) lpMemLft)++;       // Skip to next byte
                                ((LPAPLBOOL) lpMemRes)++;       // ...
                            } // End IF
                        } // End FOR

                        break;
                    } else
#define lpAPA       ((LPAPLAPA) lpMemLft)
                    // Check for left arg Boolean APA and optimized chunking
                    if (IsSimpleAPA (aplTypeLft)
                     && IsBooleanAPA (lpAPA)
                     && lpPrimSpec->B64isB64vB64)
                    {
                        APLB64 aplB64Rht,
                               aplB64APA;
                        APLB08 aplB08Res;

                        if (lpAPA->Off NE 0)
                            aplB64APA = 0xFFFFFFFFFFFFFFFF;
                        else
                            aplB64APA = 0x0000000000000000;
#undef  lpAPA
                        if (aplIntegerRht NE 0)
                            aplB64Rht = 0xFFFFFFFFFFFFFFFF;
                        else
                            aplB64Rht = 0x0000000000000000;

                        // Calculate one byte of the result
                        aplB08Res = (*lpPrimSpec->B08isB08vB08) ((APLB08) aplB64APA, (APLB08) aplB64Rht, lpPrimSpec);

                        // If it's not all zero, ...
                        if (aplB08Res NE 0)
                            FillMemory (lpMemRes, (APLU3264) RoundUpBitsToBytes (aplNELMRes), aplB08Res);
                        break;
                    } // End IF/ELSE

                    // Fall through to common code

                case ARRAY_INT:             // Res = BOOL,                  Rht = INT(S)
                    // Split cases based upon the left arg's storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:    // Res = BOOL, Lft = BOOL(M),   Rht = INT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisIvI) (BIT0 & ((*(LPAPLBOOL) lpMemLft) >> uBitIndex),
                                                         aplIntegerRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemLft)++;       // Skip to next byte
                                    ((LPAPLBOOL) lpMemRes)++;       // ...
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = BOOL, Lft = INT (M),   Rht = BOOL/INT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisIvI) (*((LPAPLINT) lpMemLft)++,
                                                         aplIntegerRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_APA:     // Res = BOOL, Lft = APA (M),   Rht = BOOL/INT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisIvI) (apaOffLft + apaMulLft * uRes,
                                                         aplIntegerRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = BOOL, Lft = FLOAT(M),  Rht = BOOL/INT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisFvF) (*((LPAPLFLOAT) lpMemLft)++,
                                                         aplFloatRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_RAT:     // Res = BOOL, Lft = RAT  (M), Rht = BOOL/INT(S)
                            // Initialize the temp
                            mpq_init_set_sx (&aplRatRht, aplIntegerRht, 1);

                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisRvR) (*((LPAPLRAT) lpMemLft)++,
                                                          aplRatRht,
                                                          lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            // We no longer need this storage
                            Myq_clear (&aplRatRht);

                            break;

                        case ARRAY_VFP:     // Res = BOOL, Lft = VFP  (M), Rht = BOOL/INT(S)
                            // Initialize the temp
                            mpfr_init_set_sx (&aplVfpRht, aplIntegerRht, MPFR_RNDN);

                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisVvV) (*((LPAPLVFP) lpMemLft)++,
                                                          aplVfpRht,
                                                          lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            // We no longer need this storage
                            Myf_clear (&aplVfpRht);

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case ARRAY_APA:             // Res = BOOL,                  Rht = APA(S)
                    // Split cases based upon the left arg's storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:    // Res = BOOL, Lft = BOOL(M),   Rht = APA(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisIvI) (BIT0 & ((*(LPAPLBOOL) lpMemLft) >> uBitIndex),
                                                         aplIntegerRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemLft)++;       // Skip to next byte
                                    ((LPAPLBOOL) lpMemRes)++;       // ...
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = BOOL, Lft = INT (M),   Rht = APA(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisIvI) (*((LPAPLINT) lpMemLft)++,
                                                         aplIntegerRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_APA:     // Res = BOOL, Lft = APA (M),   Rht = APA(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisIvI) (apaOffLft + apaMulLft * uRes,
                                                         aplIntegerRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = BOOL, Lft = FLOAT(M),  Rht = APA(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisFvF) (*((LPAPLFLOAT) lpMemLft)++,
                                                         aplFloatRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_RAT:     // Res = BOOL, Lft = RAT (M),   Rht = APA(S)
                            // Initialize temp
                            mpq_init_set_sx (&aplRatRht, aplIntegerRht, 1);

                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisRvR) (*((LPAPLRAT) lpMemLft)++,
                                                          aplRatRht,
                                                          lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            // We no longer need this storage
                            Myq_clear (&aplRatRht);

                            break;

                        case ARRAY_VFP:     // Res = BOOL, Lft = VFP (M),   Rht = APA(S)
                            // Initialize temp
                            mpfr_init_set_sx (&aplVfpRht, aplIntegerRht, MPFR_RNDN);

                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisVvV) (*((LPAPLVFP) lpMemLft)++,
                                                          aplVfpRht,
                                                          lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            // We no longer need this storage
                            Myf_clear (&aplVfpRht);

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case ARRAY_FLOAT:           // Res = BOOL,                  Rht = FLOAT(S)
                    // Split cases based upon the left arg's storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:    // Res = BOOL, Lft = BOOL (M),  Rht = FLOAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisFvF) (BIT0 & ((*(LPAPLBOOL) lpMemLft) >> uBitIndex),
                                                         aplFloatRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemLft)++;       // Skip to next byte
                                    ((LPAPLBOOL) lpMemRes)++;       // ...
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = BOOL, Lft = INT  (M),  Rht = FLOAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisFvF) ((APLFLOAT) *((LPAPLINT) lpMemLft)++,
                                                         aplFloatRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_APA:     // Res = BOOL, Lft = APA  (M),  Rht = FLOAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisFvF) ((APLFLOAT) (apaOffLft + apaMulLft * uRes),
                                                         aplFloatRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = BOOL, Lft = FLOAT(M),  Rht = FLOAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisFvF) (*((LPAPLFLOAT) lpMemLft)++,
                                                         aplFloatRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_RAT:     // Res = BOOL, Lft = RAT(M), Rht = FLOAT(S)
                            // Initialize the temps
                            mpfr_init0 (&aplVfpLft);
                            mpfr_init0 (&aplVfpRht);

                            // Convert the FLOAT to a VFP
                            mpfr_set_d (&aplVfpRht, aplFloatRht, MPFR_RNDN);

                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the RAT to a VFP
                                mpfr_set_q (&aplVfpLft, ((LPAPLRAT) lpMemLft)++, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                         aplVfpRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            // We no longer need this storage
                            Myf_clear (&aplVfpRht);
                            Myf_clear (&aplVfpLft);

                            break;

                        case ARRAY_VFP:     // Res = BOOL, Lft = VFP(M), Rht = FLOAT(S)
                            // Initialize the temp
                            mpfr_init0 (&aplVfpRht);

                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the FLOAT to a RAT
                                mpfr_set_d (&aplVfpRht, aplFloatRht, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisVvV) (*((LPAPLVFP) lpMemLft)++,
                                                         aplVfpRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            // We no longer need this storage
                            Myf_clear (&aplVfpRht);

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case ARRAY_RAT:             // Res = BOOL,                  Rht = RAT(S)
                    // Initialize the temps
                    mpq_init (&aplRatLft);
                    mpfr_init0 (&aplVfpLft);
                    mpfr_init0 (&aplVfpRht);

                    // Split cases based upon the left arg's storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:    // Res = BOOL, Lft = BOOL(M),   Rht = RAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the BOOL to a RAT
                                mpq_set_sx (&aplRatLft, BIT0 & (*(LPAPLBOOL) lpMemLft) >> uBitIndex, 1);

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisRvR) (aplRatLft,
                                                        *(LPAPLRAT) lpSymGlbRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemLft)++;       // Skip to next byte
                                    ((LPAPLBOOL) lpMemRes)++;       // ...
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = BOOL, Lft = INT (M),   Rht = RAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the INT to a RAT
                                mpq_set_sx (&aplRatLft, *((LPAPLINT) lpMemLft)++, 1);

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisRvR) (aplRatLft,
                                                        *(LPAPLRAT) lpSymGlbRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_APA:     // Res = BOOL, Lft = APA (M),   Rht = RAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the APA to a RAT
                                mpq_set_sx (&aplRatLft, apaOffLft + apaMulLft * uRes, 1);

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisRvR) (aplRatLft,
                                                        *(LPAPLRAT) lpSymGlbRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = BOOL, Lft = FLOAT(M),  Rht = RAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the FLOAT to a VFP
                                mpfr_set_d (&aplVfpLft, *((LPAPLFLOAT) lpMemLft)++, MPFR_RNDN);

                                // Convert the RAT to a VFP
                                mpfr_set_q (&aplVfpRht, (LPAPLRAT) lpSymGlbRht, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                         aplVfpRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_RAT:     // Res = BOOL, Lft = RAT (M),   Rht = RAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisRvR) (*((LPAPLRAT) lpMemLft)++,
                                                         *(LPAPLRAT) lpSymGlbRht,
                                                          lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            // We no longer need this storage
                            Myq_clear (&aplRatRht);

                            break;

                        case ARRAY_VFP:     // Res = BOOL, Lft = VFP (M),   Rht = RAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the RAT to a VFP
                                mpfr_set_q (&aplVfpRht, (LPAPLRAT) lpSymGlbRht, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisVvV) (*((LPAPLVFP) lpMemLft)++,
                                                          aplVfpRht,
                                                          lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    // We no longer need this storage
                    Myf_clear (&aplVfpRht);
                    Myf_clear (&aplVfpLft);
                    Myq_clear (&aplRatLft);

                    break;

                case ARRAY_VFP:             // Res = BOOL,                  Rht = VFP(S)
                    // Initialize the temps
                    mpfr_init0 (&aplVfpLft);
                    mpfr_init0 (&aplVfpRht);

                    // Split cases based upon the left arg's storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:    // Res = BOOL, Lft = BOOL(M),   Rht = VFP(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the BOOL to a VFP
                                mpfr_set_sx (&aplVfpLft, BIT0 & (*(LPAPLBOOL) lpMemLft) >> uBitIndex, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                        *(LPAPLVFP) lpSymGlbRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemLft)++;       // Skip to next byte
                                    ((LPAPLBOOL) lpMemRes)++;       // ...
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = BOOL, Lft = INT (M),   Rht = VFP(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the INT to a VFP
                                mpfr_set_sx (&aplVfpLft, *((LPAPLINT) lpMemLft)++, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                        *(LPAPLVFP) lpSymGlbRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_APA:     // Res = BOOL, Lft = APA (M),   Rht = VFP(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the APA to a VFP
                                mpfr_set_sx (&aplVfpLft, apaOffLft + apaMulLft * uRes, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                        *(LPAPLVFP) lpSymGlbRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = BOOL, Lft = FLOAT(M),  Rht = VFP(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the FLOAT to a VFP
                                mpfr_set_d (&aplVfpLft, *((LPAPLFLOAT) lpMemLft)++, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                        *(LPAPLVFP) lpSymGlbRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_RAT:     // Res = BOOL, Lft = RAT (M),   Rht = VFP(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the RAT to a VFP
                                mpfr_set_q (&aplVfpLft, ((LPAPLRAT) lpMemLft)++, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                        *(LPAPLVFP) lpSymGlbRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            // We no longer need this storage
                            Myq_clear (&aplRatRht);

                            break;

                        case ARRAY_VFP:     // Res = BOOL, Lft = VFP (M),   Rht = VFP(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisVvV) (*((LPAPLVFP) lpMemLft)++,
                                                         *(LPAPLVFP) lpSymGlbRht,
                                                          lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    // We no longer need this storage
                    Myf_clear (&aplVfpRht);
                    Myf_clear (&aplVfpLft);

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_INT:                     // Res = INT
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:            // Res = INT,                   Rht = BOOL(S)
                case ARRAY_INT:             // Res = INT,                   Rht = INT(S)
                case ARRAY_APA:             // Res = INT,                   Rht = APA(S)
                    // Split cases based upon the left arg's storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:    // Res = INT, Lft = BOOL(M),    Rht = BOOL/INT/APA(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisIvI) (BIT0 & ((*(LPAPLBOOL) lpMemLft) >> uBitIndex),
                                                         aplIntegerRht,
                                                         lpPrimSpec);
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemLft)++;    // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = INT, Lft = INT (M),    Rht = BOOL/INT/APA(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisIvI) (*((LPAPLINT) lpMemLft)++,
                                                         aplIntegerRht,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_APA:     // Res = INT, Lft = APA (M),    Rht = BOOL/INT/APA(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisIvI) (apaOffLft + apaMulLft * uRes,
                                                         aplIntegerRht,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = INT, Lft = FLOAT(M),   Rht = BOOL/INT/APA(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisFvF) (*((LPAPLFLOAT) lpMemLft)++,
                                                         aplFloatRht,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case ARRAY_FLOAT:           // Res = INT,                   Rht = FLOAT(S)
                    // Split cases based upon the left arg's storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:    // Res = INT, Lft = BOOL (M),   Rht = FLOAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisFvF) (BIT0 & ((*(LPAPLBOOL) lpMemLft) >> uBitIndex),
                                                         aplFloatRht,
                                                         lpPrimSpec);
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemLft)++;    // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = INT, Lft = INT  (M),   Rht = FLOAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisFvF) ((APLFLOAT) *((LPAPLINT) lpMemLft)++,
                                                         aplFloatRht,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_APA:     // Res = INT, Lft = APA  (M),   Rht = FLOAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisFvF) ((APLFLOAT) (apaOffLft + apaMulLft * uRes),
                                                         aplFloatRht,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = INT, Lft = FLOAT(M),   Rht = FLOAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisFvF) (*((LPAPLFLOAT) lpMemLft)++,
                                                         aplFloatRht,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_FLOAT:                   // Res = FLOAT
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:            // Res = FLOAT,                 Rht = BOOL(S)
                case ARRAY_INT:             // Res = FLOAT,                 Rht = INT (S)
                case ARRAY_APA:             // Res = FLOAT,                 Rht = APA (S)
                    // Split cases based upon the left arg's storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:    // Res = FLOAT, Lft = BOOL (M), Rht = BOOL/INT/APA(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisIvI) (BIT0 & ((*(LPAPLBOOL) lpMemLft) >> uBitIndex),
                                                         aplIntegerRht,
                                                         lpPrimSpec);
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemLft)++;    // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = FLOAT, Lft = INT  (M), Rht = BOOL/INT/APA(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisIvI) (*((LPAPLINT) lpMemLft)++,
                                                         aplIntegerRht,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_APA:     // Res = FLOAT, Lft = APA  (M), Rht = BOOL/INT/APA(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisIvI) (apaOffLft + apaMulLft * uRes,
                                                         aplIntegerRht,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = FLOAT, Lft = FLOAT(M), Rht = BOOL/INT/APA(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisFvF) (*((LPAPLFLOAT) lpMemLft)++,
                                                         aplFloatRht,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case ARRAY_FLOAT:           // Res = FLOAT,                 Rht = FLOAT(S)
                    // Split cases based upon the left arg's storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:    // Res = FLOAT, Lft = BOOL (M), Rht = FLOAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisFvF) (BIT0 & ((*(LPAPLBOOL) lpMemLft) >> uBitIndex),
                                                         aplFloatRht,
                                                         lpPrimSpec);
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemLft)++;    // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = FLOAT, Lft = INT  (M), Rht = FLOAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisFvF) ((APLFLOAT) *((LPAPLINT) lpMemLft)++,
                                                         aplFloatRht,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_APA:     // Res = FLOAT, Lft = APA  (M), Rht = FLOAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisFvF) ((APLFLOAT) (apaOffLft + apaMulLft * uRes),
                                                         aplFloatRht,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = FLOAT, Lft = FLOAT(M), Rht = FLOAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisFvF) (*((LPAPLFLOAT) lpMemLft)++,
                                                         aplFloatRht,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_RAT:                     // Res = RAT
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:            // Res = RAT,                   Rht = BOOL(S)
                case ARRAY_INT:             // Res = RAT,                   Rht = INT(S)
                case ARRAY_APA:             // Res = RAT,                   Rht = APA(S)
                    // Convert the INT to a RAT
                    mpq_init_set_sx (&aplRatRht, aplIntegerRht, 1);

                    // Split cases based upon the left arg's storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:    // Res = RAT, Lft = BOOL(M),    Rht = BOOL/INT/APA(S)
                        case ARRAY_INT:     // Res = RAT, Lft = INT (M),    Rht = BOOL/INT/APA(S)
                        case ARRAY_APA:     // Res = RAT, Lft = APA (M),    Rht = BOOL/INT/APA(S)
                            // Initialize the temp
                            mpq_init (&aplRatLft);

                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the INT to a RAT
                                mpq_set_sx (&aplRatLft, GetNextInteger (lpMemLft, aplTypeLft, uRes), 1);

                                // Save in the result
                                *((LPAPLRAT)   lpMemRes)++ =
                                  (*lpPrimSpec->RisRvR) (aplRatLft,
                                                         aplRatRht,
                                                         lpPrimSpec);
                            } // End FOR

                            // We no longer need this storage
                            Myq_clear (&aplRatLft);

                            break;

                        case ARRAY_FLOAT:   // Res = RAT, Lft = FLOAT(M),   Rht = BOOL/INT/APA(S)
                            // Initialize the temp
                            mpq_init (&aplRatLft);

                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the FLOAT to a RAT
                                mpq_set_d (&aplRatLft, GetNextFloat (lpMemLft, aplTypeLft, uRes));

                                // Save in the result
                                *((LPAPLRAT)   lpMemRes)++ =
                                  (*lpPrimSpec->RisRvR) (aplRatLft,
                                                         aplRatRht,
                                                         lpPrimSpec);
                            } // End FOR

                            // We no longer need this storage
                            Myq_clear (&aplRatLft);

                            break;

                        case ARRAY_RAT:     // Res = RAT, Lft = RAT(M),     Rht = BOOL/INT/APA(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLRAT)   lpMemRes)++ =
                                  (*lpPrimSpec->RisRvR) (*((LPAPLRAT) lpMemLft)++,
                                                          aplRatRht,
                                                          lpPrimSpec);
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    // We no longer need this storage
                    Myq_clear (&aplRatRht);

                    break;

                case ARRAY_FLOAT:           // Res = RAT,                   Rht = FLOAT(S)
                    // Convert the FLOAT to a RAT
                    mpq_init_set_d (&aplRatRht, aplFloatRht);

                    // Split cases based upon the left arg's storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:    // Res = RAT, Lft = BOOL(M),    Rht = FLOAT(S)
                        case ARRAY_INT:     // Res = RAT, Lft = INT (M),    Rht = FLOAT(S)
                        case ARRAY_APA:     // Res = RAT, Lft = APA (M),    Rht = FLOAT(S)
                            // Initialize the temp
                            mpq_init (&aplRatLft);

                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the INT to a RAT
                                mpq_set_sx (&aplRatLft, GetNextInteger (lpMemLft, aplTypeLft, uRes), 1);

                                // Save in the result
                                *((LPAPLRAT)   lpMemRes)++ =
                                  (*lpPrimSpec->RisRvR) (aplRatLft,
                                                         aplRatRht,
                                                         lpPrimSpec);
                            } // End FOR

                            // We no longer need this storage
                            Myq_clear (&aplRatLft);

                            break;

                        case ARRAY_FLOAT:   // Res = RAT, Lft = FLOAT(M),   Rht = FLOAT(S)
                            // Initialize the temp
                            mpq_init (&aplRatLft);

                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the FLOAT to a RAT
                                mpq_set_d (&aplRatLft, GetNextFloat (lpMemLft, aplTypeLft, uRes));

                                // Save in the result
                                *((LPAPLRAT)   lpMemRes)++ =
                                  (*lpPrimSpec->RisRvR) (aplRatLft,
                                                         aplRatRht,
                                                         lpPrimSpec);
                            } // End FOR

                            // We no longer need this storage
                            Myq_clear (&aplRatLft);

                            break;

                        case ARRAY_RAT:     // Res = RAT, Lft = RAT(M),     Rht = FLOAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLRAT)   lpMemRes)++ =
                                  (*lpPrimSpec->RisRvR) (*((LPAPLRAT) lpMemLft)++,
                                                          aplRatRht,
                                                          lpPrimSpec);
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    // We no longer need this storage
                    Myq_clear (&aplRatRht);

                    break;

                case ARRAY_RAT:             // Res = RAT,                   Rht = RAT(S)
                    // Split cases based upon the left arg's storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:    // Res = RAT, Lft = BOOL(M),    Rht = RAT(S)
                        case ARRAY_INT:     // Res = RAT, Lft = INT(M),     Rht = RAT(S)
                        case ARRAY_APA:     // Res = RAT, Lft = APA(M),     Rht = RAT(S)
                            // Initialize the temp
                            mpq_init (&aplRatLft);

                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the INT to a RAT
                                mpq_set_sx (&aplRatLft, GetNextInteger (lpMemLft, aplTypeLft, uRes), 1);

                                // Save in the result
                                *((LPAPLRAT)   lpMemRes)++ =
                                  (*lpPrimSpec->RisRvR) (aplRatLft,
                                                        *(LPAPLRAT) lpSymGlbRht,
                                                         lpPrimSpec);
                            } // End FOR

                            // We no longer need this storage
                            Myq_clear (&aplRatLft);

                            break;

                        case ARRAY_RAT:     // Res = RAT, Lft = RAT(M),     Rht = RAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLRAT)   lpMemRes)++ =
                                  (*lpPrimSpec->RisRvR) (*((LPAPLRAT) lpMemLft)++,
                                                         *(LPAPLRAT) lpSymGlbRht,
                                                          lpPrimSpec);
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_VFP:                     // Res = VFP
            // Initialize the temps
            mpfr_init0 (&aplVfpLft);
            mpfr_init0 (&aplVfpRht);

            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:            // Res = VFP,                   Rht = BOOL(S)
                case ARRAY_INT:             // Res = VFP,                   Rht = INT(S)
                case ARRAY_APA:             // Res = VFP,                   Rht = APA(S)
                    // Convert the INT to a VFP
                    mpfr_set_sx (&aplVfpRht, aplIntegerRht, MPFR_RNDN);

                    // Split cases based upon the left arg's storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_RAT:     // Res = VFP, Lft = RAT(M),     Rht = BOOL/INT/APA(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the RAT to a VFP
                                mpfr_set_q (&aplVfpLft, ((LPAPLRAT) lpMemLft)++, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLVFP)   lpMemRes)++ =
                                  (*lpPrimSpec->VisVvV) (aplVfpLft,
                                                         aplVfpRht,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_VFP:     // Res = VFP, Lft = VFP(M),     Rht = BOOL/INT/APA(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLVFP)   lpMemRes)++ =
                                  (*lpPrimSpec->VisVvV) (*((LPAPLVFP) lpMemLft)++,
                                                          aplVfpRht,
                                                          lpPrimSpec);
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case ARRAY_FLOAT:           // Res = VFP,                   Rht = FLOAT(S)
                    // Convert the FLOAT to a VFP
                    mpfr_set_d (&aplVfpRht, aplFloatRht, MPFR_RNDN);

                    // Split cases based upon the left arg's storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_RAT:     // Res = VFP, Lft = RAT(M),     Rht = BOOL/INT/APA(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the RAT to a VFP
                                mpfr_set_q (&aplVfpLft, ((LPAPLRAT) lpMemLft)++, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLVFP)   lpMemRes)++ =
                                  (*lpPrimSpec->VisVvV) (aplVfpLft,
                                                         aplVfpRht,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_VFP:     // Res = VFP, Lft = VFP(M),     Rht = BOOL/INT/APA(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLVFP)   lpMemRes)++ =
                                  (*lpPrimSpec->VisVvV) (*((LPAPLVFP) lpMemLft)++,
                                                          aplVfpRht,
                                                          lpPrimSpec);
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case ARRAY_RAT:             // Res = VFP,                   Rht = RAT(S)
                    // Convert the RAT to a VFP
                    mpfr_set_q (&aplVfpRht, (LPAPLRAT) lpSymGlbRht, MPFR_RNDN);

                    // Split cases based upon the left arg's storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:    // Res = VFP, Lft = BOOL(M),    Rht = RAT(S)
                        case ARRAY_INT:     // Res = VFP, Lft = INT(M),     Rht = RAT(S)
                        case ARRAY_APA:     // Res = VFP, Lft = APA(M),     Rht = RAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the INT to a VFP
                                mpfr_set_sx (&aplVfpLft, GetNextInteger (lpMemLft, aplTypeLft, uRes), MPFR_RNDN);

                                // Save in the result
                                *((LPAPLVFP)   lpMemRes)++ =
                                  (*lpPrimSpec->VisVvV) (aplVfpLft,
                                                         aplVfpRht,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = VFP, Lft = FLOAT(M),   Rht = RAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the FLOAT to a VFP
                                mpfr_set_d (&aplVfpLft, *((LPAPLFLOAT) lpMemLft)++, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLVFP)   lpMemRes)++ =
                                  (*lpPrimSpec->VisVvV) (aplVfpLft,
                                                         aplVfpRht,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_RAT:     // Res = VFP, Lft = RAT(M),     Rht = RAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the RAT to a VFP
                                mpfr_set_q (&aplVfpLft, ((LPAPLRAT) lpMemLft)++, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLVFP)   lpMemRes)++ =
                                  (*lpPrimSpec->VisVvV) (aplVfpLft,
                                                         aplVfpRht,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_VFP:     // Res = VFP, Lft = VFP(M),     Rht = RAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLVFP)   lpMemRes)++ =
                                  (*lpPrimSpec->VisVvV) (*((LPAPLVFP) lpMemLft)++,
                                                          aplVfpRht,
                                                          lpPrimSpec);
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case ARRAY_VFP:             // Res = VFP
                    // Split cases based upon the left arg's storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:    // Res = VFP, Lft = BOOL(M),    Rht = VFP(S)
                        case ARRAY_INT:     // Res = VFP, Lft = INT(M),     Rht = VFP(S)
                        case ARRAY_APA:     // Res = VFP, Lft = APA(M),     Rht = VFP(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the INT to a VFP
                                mpfr_set_sx (&aplVfpLft, GetNextInteger (lpMemLft, aplTypeLft, uRes), MPFR_RNDN);

                                // Save in the result
                                *((LPAPLVFP)   lpMemRes)++ =
                                  (*lpPrimSpec->VisVvV) (aplVfpLft,
                                                        *(LPAPLVFP) lpSymGlbRht,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = VFP, Lft = FLOAT(M),   Rht = VFP(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the FLOAT to a VFP
                                mpfr_set_d (&aplVfpLft, *((LPAPLFLOAT) lpMemLft)++, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLVFP)   lpMemRes)++ =
                                  (*lpPrimSpec->VisVvV) (aplVfpLft,
                                                        *(LPAPLVFP) lpSymGlbRht,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_RAT:     // Res = VFP, Lft = RAT(M),     Rht = VFP(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert the RAT to a VFP
                                mpfr_set_q (&aplVfpLft, ((LPAPLRAT) lpMemLft)++, MPFR_RNDN);

                                // Save in the result
                                *((LPAPLVFP)   lpMemRes)++ =
                                  (*lpPrimSpec->VisVvV) (aplVfpLft,
                                                        *(LPAPLVFP) lpSymGlbRht,
                                                         lpPrimSpec);
                            } // End FOR

                            break;

                        case ARRAY_VFP:     // Res = VFP, Lft = VFP(M),     Rht = VFP(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLVFP)   lpMemRes)++ =
                                  (*lpPrimSpec->VisVvV) (*((LPAPLVFP) lpMemLft)++,
                                                         *(LPAPLVFP) lpSymGlbRht,
                                                          lpPrimSpec);
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                defstop
                    break;
            } // End SWITCH

            // We no longer need this storage
            Myf_clear (&aplVfpRht);
            Myf_clear (&aplVfpLft);

            break;

        defstop
            break;
    } // End SWITCH
    } __except (CheckException (GetExceptionInformation (), L"PrimFnDydMultSing_EM"))
    {
        EXCEPTION_CODES ExceptionCode = MyGetExceptionCode ();  // The exception code

        dprintfWL9 (L"!!Initiating Exception in " APPEND_NAME L" #6: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

        // Split cases based upon the ExceptionCode
        switch (ExceptionCode)
        {
            case EXCEPTION_DOMAIN_ERROR:
            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            case EXCEPTION_INT_DIVIDE_BY_ZERO:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                goto DOMAIN_EXIT;

            case EXCEPTION_WS_FULL:
                goto WSFULL_EXIT;

            case EXCEPTION_NONCE_ERROR:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                goto NONCE_EXIT;

            case EXCEPTION_RESULT_RAT:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                if (IsNumeric (aplTypeRes)
                 && !IsRat    (aplTypeRes))
                {
                    // It's now a RAT result
                    aplTypeRes = ARRAY_RAT;

                    // We need to start over with the result
                    MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
                    FreeResultGlobalVar (*lphGlbRes); *lphGlbRes = NULL;

                    if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                                     lphGlbRes,
                                                     lpMemHdrLft,
                                                     NULL,
                                                     lpMemHdrLft->Rank,
                                                     0,
                                                    &aplRankRes,
                                                     aplTypeRes,
                                                     bLftIdent,
                                                     bRhtIdent,
                                                     lpMemHdrLft->NELM,
                                                     1,
                                                     aplNELMRes))
                        goto ERROR_EXIT;

                    // Lock the memory to get a ptr to it
                    lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);

                    // Restart the pointer
                    lpMemLft = lpMemLftStart;

                    dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #6: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                    goto RESTART_EXCEPTION;
                } // End IF

                // Display message for unhandled exception
                DisplayException ();

                break;

            case EXCEPTION_RESULT_VFP:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                if (IsNumeric (aplTypeRes)
                 && !IsVfp    (aplTypeRes))
                {
                    // It's now a VFP result
                    aplTypeRes = ARRAY_VFP;

                    // We need to start over with the result
                    MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
                    FreeResultGlobalVar (*lphGlbRes); *lphGlbRes = NULL;

                    if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                                     lphGlbRes,
                                                     lpMemHdrLft,
                                                     NULL,
                                                     lpMemHdrLft->Rank,
                                                     0,
                                                    &aplRankRes,
                                                     aplTypeRes,
                                                     bLftIdent,
                                                     bRhtIdent,
                                                     lpMemHdrLft->NELM,
                                                     1,
                                                     aplNELMRes))
                        goto ERROR_EXIT;

                    // Lock the memory to get a ptr to it
                    lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);

                    // Restart the pointer
                    lpMemLft = lpMemLftStart;

                    dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #6: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                    goto RESTART_EXCEPTION;
                } // End IF

                // Display message for unhandled exception
                DisplayException ();

                break;

            case EXCEPTION_RESULT_FLOAT:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                if (IsNumeric (aplTypeRes)
                 && !IsSimpleFlt (aplTypeRes))
                {
                    // If the previous result is Boolean, we need to
                    //   unlock, free, and allocate the result anew
                    if (IsSimpleBool (aplTypeRes))
                    {
                        // It's now a FLOAT result
                        aplTypeRes = ARRAY_FLOAT;

                        // We need to start over with the result
                        MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
                        FreeResultGlobalVar (*lphGlbRes); *lphGlbRes = NULL;

                        if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                                         lphGlbRes,
                                                         lpMemHdrLft,
                                                         NULL,
                                                         lpMemHdrLft->Rank,
                                                         0,
                                                        &aplRankRes,
                                                         aplTypeRes,
                                                         bLftIdent,
                                                         bRhtIdent,
                                                         lpMemHdrLft->NELM,
                                                         1,
                                                         aplNELMRes))
                            goto ERROR_EXIT;

                        // Lock the memory to get a ptr to it
                        lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);
                    } else
                    // The previous result must have been INT which is
                    //   the same size as FLOAT, so there's no need to
                    //   change storage.
                    {
                        // It's now a FLOAT result
                        aplTypeRes = ARRAY_FLOAT;

                        // If the result is not nested, ...
                        if (!IsNested (lpMemHdrRes->ArrType))
                            // Tell the header about it
                            lpMemHdrRes->ArrType = aplTypeRes;

                        // Restart the pointers
                        lpMemRes = lpMemHdrRes;
                    } // End IF/ELSE

                    // Restart the pointer
                    lpMemLft = lpMemLftStart;

                    dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #6: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                    goto RESTART_EXCEPTION;
                } // End IF

                // Fall through to never-never-land

            default:
                // Display message for unhandled exception
                DisplayException ();

                break;
        } // End SWITCH
    } // End __try/__except

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    // We no longer need this storage
    Myq_clear (&aplRatRht);
    Myq_clear (&aplRatLft);
    Myf_clear (&aplVfpRht);
    Myf_clear (&aplVfpLft);

    // Restore the memory ptr
    *lplpMemRes = lpMemRes;

    return bRet;
} // End PrimFnDydMultSing_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydSiScNest_EM
//
//  Subroutine to PrimFnDydSimpNest_EM to handle left arg simple scalars
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSiScNest_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL PrimFnDydSiScNest_EM
    (LPTOKEN    lptkFunc,           // Ptr to function token
     APLSTYPE   aplTypeLft,         // Left arg type
     APLINT     aplIntegerLft,      // ...      integer value
     APLFLOAT   aplFloatLft,        // ...      float   ...
     APLCHAR    aplCharLft,         // ...      char    ...
     HGLOBAL    lpSymGlbLft,        // ...      lpSymGlb
     APLNESTED  aplNestedRht,       // Right arg nested value
     UBOOL      bLftIdent,          // TRUE iff the function has a left identity element and the Axis tail is valid
     UBOOL      bRhtIdent,          // ...                         right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    UBOOL             bRet = TRUE;
    HGLOBAL           hGlbRht = NULL,
                      hGlbRes = NULL,
                      hGlbSub;
    LPVOID            lpMemRht,
                      lpMemRes;
    LPVARARRAY_HEADER lpMemHdrRes = NULL,
                      lpMemHdrRht = NULL;
    APLSTYPE          aplTypeRht,
                      aplTypeRes;
    APLNELM           aplNELMLft = 1,
                      aplNELMRht,
                      aplNELMRes;
    APLRANK           aplRankLft = 0,
                      aplRankRht,
                      aplRankRes;
    APLINT            aplIntegerRht,
                      uRht,
                      apaOffRht,
                      apaMulRht;
    APLFLOAT          aplFloatRht;
    APLCHAR           aplCharRht;
    UINT              uBitIndex = 0;

    // The right arg data is a valid HGLOBAL array
    Assert (IsGlbTypeVarDir_PTB (aplNestedRht));

    // Clear the identifying bits
    hGlbRht = aplNestedRht;

    // Lock the memory to get a ptr to it
    lpMemHdrRht = MyGlobalLockVar (hGlbRht);

#define lpHeader    lpMemHdrRht
    // Get the Array Type, NELM, and Rank
    aplTypeRht = lpHeader->ArrType;
    aplNELMRht = lpHeader->NELM;
    aplRankRht = lpHeader->Rank;
#undef  lpHeader

    // The NELM of the result is NELM of the non-scalar
    aplNELMRes = aplNELMRht;

    // Calculate the storage type of the result
    aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (aplNELMLft,
                                               &aplTypeLft,
                                                lptkFunc,
                                                aplNELMRht,
                                               &aplTypeRht);
    if (IsErrorType (aplTypeRes))
        goto DOMAIN_EXIT;

    // Special case APA result
    if (IsSimpleAPA (aplTypeRes))
    {
        if (!(*lpPrimSpec->ApaResultDyd_EM) (NULL,
                                             lptkFunc,
                                             NULL,
                                             hGlbRht,
                                            &hGlbRes,
                                             aplRankLft,
                                             aplRankRht,
                                             aplNELMLft,
                                             aplNELMRht,
                                             aplIntegerLft,
                                             lpPrimSpec))
            goto ERROR_EXIT;
        else
            goto NORMAL_EXIT;
    } // End IF

    Assert (IsSimpleGlbNum (aplTypeRes)
         || IsNested (aplTypeRes));

    // Allocate space for result
    if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                    &hGlbRes,
                                     NULL,
                                     lpMemHdrRht,
                                     aplRankLft,
                                     aplRankRht,
                                    &aplRankRes,
                                     aplTypeRes,
                                     bLftIdent,
                                     bRhtIdent,
                                     aplNELMLft,
                                     aplNELMRht,
                                     aplNELMRes))
        goto ERROR_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (hGlbRes);

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // If the right arg is an APA, ...
    if (IsSimpleAPA (aplTypeRht))
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        // Get the APA parameters
        apaOffRht = lpAPA->Off;
        apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // If simple result, ...
    if (IsSimpleGlbNum (aplTypeRes))
        bRet = PrimFnDydSingMult_EM (&hGlbRes,
                                      aplTypeRes,
                                      lpMemHdrRes,
                                     &lpMemRes,
                                      aplNELMRes,
                                      aplTypeLft,
                                      aplIntegerLft,
                                      aplFloatLft,
                                      aplCharLft,
                                      lpSymGlbLft,
                                      aplTypeRht,
                                      apaOffRht,
                                      apaMulRht,
                                      lpMemHdrRht,
                                      lpMemRht,
                                      bLftIdent,
                                      bRhtIdent,
                                      lptkFunc,
                                      lpPrimSpec);
    else
    // If nested result, ...
    if (IsNested (aplTypeRes))
    {
        // Loop through the right arg/result
        for (uRht = 0; uRht < (APLNELMSIGN) aplNELMRht; uRht++)
        {
            APLSTYPE aplTypeHetRht;

            // Copy in case we are heterogeneous
            aplTypeHetRht = aplTypeRht;

            // Get the right arg element
            hGlbSub = ((LPAPLNESTED) lpMemRht)[uRht];

            // Split cases based upon the ptr type of the nested right arg
            switch (GetPtrTypeDir (hGlbSub))
            {
                case PTRTYPE_STCONST:
                    GetFirstValueImm (((LPSYMENTRY) hGlbSub)->stFlags.ImmType,
                                      ((LPSYMENTRY) hGlbSub)->stData.stLongest,
                                     &aplIntegerRht,
                                     &aplFloatRht,
                                     &aplCharRht,
                                      NULL,
                                      NULL,
                                     &aplTypeHetRht);
                    hGlbSub = PrimFnDydSiScSiSc_EM (lptkFunc,
                                                    hGlbRes,
                                                    aplTypeLft,
                                                    aplIntegerLft,
                                                    aplFloatLft,
                                                    aplCharLft,
                                                    lpSymGlbLft,
                                                    aplTypeHetRht,
                                                    aplIntegerRht,
                                                    aplFloatRht,
                                                    aplCharRht,
                                                    NULL,
                                                    lpPrimSpec);
                    if (hGlbSub EQ NULL)
                        goto ERROR_EXIT;
                    else
                    // If the result is not already filled in, ...
                    if (((LPAPLNESTED) lpMemRes)[0] EQ NULL)
                        // Save in the result
                        ((LPAPLNESTED) lpMemRes)[0] = hGlbSub;
                    // Skip to the next result
                    ((LPAPLNESTED) lpMemRes)++;

                    break;

                case PTRTYPE_HGLOBAL:
                    // 2 4-({enclose}0 1)(0 1 2)
                    hGlbSub = PrimFnDydSiScNest_EM (lptkFunc,
                                                    aplTypeLft,
                                                    aplIntegerLft,
                                                    aplFloatLft,
                                                    aplCharLft,
                                                    lpSymGlbLft,
                                                    hGlbSub,
                                                    bLftIdent,
                                                    bRhtIdent,
                                                    lpPrimSpec);
                    if (hGlbSub EQ NULL)
                        goto ERROR_EXIT;
                    else
                        // Save in the result
                        *((LPAPLNESTED) lpMemRes)++ = MakePtrTypeGlb (hGlbSub);
                    break;

                defstop
                    break;
            } // End SWITCH
        } // End FOR
    } else
        DbgStop ();         // We should never get here

    if (bRet)
        goto NORMAL_EXIT;
    else
        goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
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
    if (lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

    if (bRet)
        return hGlbRes;
    else
        return NULL;
} // End PrimFnDydSiScNest_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydSiScSiSc_EM
//
//  Dyadic primitive scalar function, left simple scalar, right simple scalar
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSiScSiSc_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL PrimFnDydSiScSiSc_EM
    (LPTOKEN    lptkFunc,           // Ptr to function token
     HGLOBAL    hGlbRes,            // Result global memory handle
     APLSTYPE   aplTypeLft,         // Left arg storage type
     APLINT     aplIntegerLft,      // ...      as integer
     APLFLOAT   aplFloatLft,        // ...         float
     APLCHAR    aplCharLft,         // ...         char
     HGLOBAL    lpSymGlbLft,        // ...         Sym/Glb (may be NULL)
     APLSTYPE   aplTypeRht,         // Right arg storage type
     APLINT     aplIntegerRht,      // ...       as integer
     APLFLOAT   aplFloatRht,        // ...          float
     APLCHAR    aplCharRht,         // ...          char
     HGLOBAL    lpSymGlbRht,        // ...          Sym/Glb (may be NULL)
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    TOKEN    tkRes = {0};   // Result token
    APLSTYPE aplTypeRes;    // Result storage type

    // Calculate the storage type of the result
    aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (1,
                                               &aplTypeLft,
                                                lptkFunc,
                                                1,
                                               &aplTypeRht);
    if (IsErrorType (aplTypeRes))
        goto DOMAIN_EXIT;

    if (PrimFnDydSiScSiScSub_EM (&tkRes,
                                  lptkFunc,
                                 &hGlbRes,
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
                                 &aplTypeRes,
                                  lpPrimSpec))
    {
        // If the result is an immediate, ...
        if (IsTknImmed (&tkRes))
            // Convert the immediate type and value in tkRes
            //   into an LPSYMENTRY
            return MakeSymEntry_EM (tkRes.tkFlags.ImmType,      // Immediate type
                                   &tkRes.tkData.tkLongest,     // Ptr to immediate value
                                    lptkFunc);                  // Ptr to function token
        else
            return tkRes.tkData.tkGlbData;
    } else
        return NULL;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFnDydSiScSiSc_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydSiScSiScSub_EM
//
//  Subroutine to PrimFnDydSiScSiSc_EM
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSiScSiScSub_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydSiScSiScSub_EM
    (LPTOKEN    lptkRes,            // Ptr to result token
     LPTOKEN    lptkFunc,           // Ptr to function token
     HGLOBAL   *lphGlbRes,          // Ptr to result global memory handle (may be NULL)
     APLSTYPE   aplTypeRes,         // Result storage type
     APLSTYPE   aplTypeLft,         // Left arg storage type
     APLINT     aplIntegerLft,      // ...      as an integer
     APLFLOAT   aplFloatLft,        // ...            float
     APLCHAR    aplCharLft,         // ...            char
     HGLOBAL    lpSymGlbLft,        // ...            lpSym/Glb
     APLSTYPE   aplTypeRht,         // Right arg storage type
     APLINT     aplIntegerRht,      // ...       as an integer
     APLFLOAT   aplFloatRht,        // ...             float
     APLCHAR    aplCharRht,         // ...             char
     HGLOBAL    lpSymGlbRht,        // ...             lpSym/Glb
     APLSTYPE  *lpaplTypeRes,       // Ptr to result storage type
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    IMM_TYPES         immType;              // Result immediate type
    UBOOL             bRet = TRUE;          // TRUE iff result is valid
    APLUINT           ByteRes;              // # bytes in the result
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to result header
    LPVOID            lpMemRes;             // Ptr to result global memory
    APLRAT            aplRatLft = {0},      // Left arg as Rational
                      aplRatRht = {0},      // Right ...
                      aplRatRes = {0};      // Result ...
    APLVFP            aplVfpLft = {0},      // Left arg as VFP
                      aplVfpRht = {0},      // Right ...
                      aplVfpRes = {0};      // Result ...
    APLSTYPE          aplTypeCom;           // Common storage type
    HGLOBAL           hGlbTmp = NULL;       // Temporary hGlbRes

RESTART_EXCEPTION_IMMED:
    // In case we restart, ...
    if (lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbTmp); lpMemHdrRes = NULL;
    } // End IF

    // If the result is simple, ...
    if (IsSimple (aplTypeRes))
    {
        // Get the immediate type for the token
        immType = TranslateArrayTypeToImmType (aplTypeRes);

        Assert (IsImmNum (immType));

        // Fill in the result token
        lptkRes->tkFlags.TknType   = TKT_VARIMMED;
        lptkRes->tkFlags.ImmType   = immType;
////////lptkRes->tkFlags.NoDisplay = FALSE;     // Already zero from YYAlloc
        lptkRes->tkCharIndex       = lptkFunc->tkCharIndex;
    } else
    {
        // Fill in the result token
        lptkRes->tkFlags.TknType   = TKT_VARARRAY;
////////lptkRes->tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lptkRes->tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lptkRes->tkCharIndex       = lptkFunc->tkCharIndex;
    } // End IF/ELSE

    __try
    {
        // Split cases based upon the result's storage type
        switch (aplTypeRes)
        {
            case ARRAY_BOOL:                    // Res = BOOL
                // If both arguments are BOOL,
                //   use BisBvB
                if (IsSimpleBool (aplTypeLft)
                 && IsSimpleBool (aplTypeRht))  // Res = BOOL, Lft = BOOL(S), Rht = BOOL(S)
                    lptkRes->tkData.tkBoolean  =
                      (*lpPrimSpec->BisBvB) ((APLBOOL) aplIntegerLft,
                                             (APLBOOL) aplIntegerRht,
                                             lpPrimSpec);
                else
                // If both arguments are integer-like (BOOL, INT, or APA),
                //   use BisIvI
                if (IsSimpleInt (aplTypeLft)
                 && IsSimpleInt (aplTypeRht))   // Res = BOOL, Lft = BOOL/INT/APA(S), Rht = BOOL/INT/APA(S)
                    lptkRes->tkData.tkBoolean  =
                      (*lpPrimSpec->BisIvI) (aplIntegerLft,
                                             aplIntegerRht,
                                             lpPrimSpec);
                else
                // If both arguments are CHAR,
                //   use BisCvC
                if (IsSimpleChar (aplTypeLft)
                 && IsSimpleChar (aplTypeRht))  // Res = BOOL, Lft = CHAR(S), Rht = CHAR(S)
                    lptkRes->tkData.tkBoolean  =
                      (*lpPrimSpec->BisCvC) (aplCharLft,
                                             aplCharRht,
                                             lpPrimSpec);
                else
                // If either argument is FLOAT and the other is simple numeric (BOOL, INT, APA, or FLOAT),
                //   use BisFvF
                if ((IsSimpleFlt (aplTypeLft) && IsSimpleNum (aplTypeRht))   // Res = BOOL, Lft = FLOAT(S), Rht = BOOL/INT/APA/FLOAT(S)
                 || (IsSimpleFlt (aplTypeRht) && IsSimpleNum (aplTypeLft)))  // Res = BOOL, Lft = BOOL/INT/APA/FLOAT(S), Rht = FLOAT(S)
                    lptkRes->tkData.tkBoolean  =
                      (*lpPrimSpec->BisFvF) (aplFloatLft,
                                             aplFloatRht,
                                             lpPrimSpec);
                else
                // If one arg is numeric, the other char
                if ((IsNumeric (aplTypeLft) && IsSimpleChar (aplTypeRht))   // Res = BOOL, Lft = NUMB(S), Rht = CHAR(s)
                 || (IsNumeric (aplTypeRht) && IsSimpleChar (aplTypeLft)))  // Res = BOOL, Lft = CHAR(S), Rht = CHAR(S)
                {
                    Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                         || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);

                    // If the function is UTF16_NOTEQUAL, the result is one
                    lptkRes->tkData.tkBoolean  = (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);
                } else
                // If either argument is global numeric, ...
                if (IsGlbNum (aplTypeLft)
                 || IsGlbNum (aplTypeRht))
                {
                    // Calculate the common storage type
                    aplTypeCom = aTypePromote[aplTypeLft][aplTypeRht];

                    // Initialize the temps
                    mpq_init (&aplRatLft);
                    mpq_init (&aplRatRht);
                    mpfr_init0 (&aplVfpLft);
                    mpfr_init0 (&aplVfpRht);

                    // Promote the left arg to the common type
                    switch (aplTypeCom)
                    {
                        case ARRAY_RAT:                 // Res = BOOL
                            // Split cases based upon the left arg storage type
                            switch (aplTypeLft)
                            {
                                case ARRAY_BOOL:        // Res = BOOL, Lft = BOOL
                                case ARRAY_INT:         // Res = BOOL, Lft = INT
                                case ARRAY_APA:         // Res = BOOL, Lft = APA
                                    // Convert the BOOL/INT/APA to a RAT
                                    mpq_set_sx (&aplRatLft, aplIntegerLft, 1);

                                    break;

                                case ARRAY_FLOAT:       // Res = BOOL, Lft = FLOAT
                                    // Convert the FLOAT to a RAT
                                    mpq_set_d  (&aplRatLft, aplFloatLft);

                                    break;

                                case ARRAY_RAT:         // Res = BOOL, :ft = RAT
                                    // Copy the RAT to a RAT
                                    mpq_set (&aplRatLft, (LPAPLRAT) lpSymGlbLft);

                                    break;

                                case ARRAY_CHAR:        // Can't happen
                                case ARRAY_VFP:         // Can't happen
                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        case ARRAY_VFP:                 // Res = BOOL
                            // Split cases based upon the left arg storage type
                            switch (aplTypeLft)
                            {
                                case ARRAY_BOOL:        // Res = BOOL, Lft = BOOL
                                case ARRAY_INT:         // Res = BOOL, Lft = INT
                                case ARRAY_APA:         // Res = BOOL, Lft = APA
                                    // Convert the BOOL/INT/APA to a VFP
                                    mpfr_set_sx (&aplVfpLft, aplIntegerLft, MPFR_RNDN);

                                    break;

                                case ARRAY_FLOAT:       // Res = BOOL, Lft = FLOAT
                                    // Convert the FLOAT to a VFP
                                    mpfr_set_d (&aplVfpLft, aplFloatLft, MPFR_RNDN);

                                    break;

                                case ARRAY_RAT:         // Res = BOOL, Lft = RAT
                                    // Convert the RAT to a VFP
                                    mpfr_set_q (&aplVfpLft, (LPAPLRAT) lpSymGlbLft, MPFR_RNDN);

                                    break;

                                case ARRAY_VFP:         // Res = BOOL, Lft = VFP
                                    // Copy the VFP to a VFP
                                    mpfr_copy (&aplVfpLft, (LPAPLVFP) lpSymGlbLft);

                                    break;

                                case ARRAY_CHAR:        // Can't happen
                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    // Promote the right arg to the common type
                    switch (aplTypeCom)
                    {
                        case ARRAY_RAT:                 // Res = BOOL
                            // Split cases based upon the right arg storage type
                            switch (aplTypeRht)
                            {
                                case ARRAY_BOOL:        // Res = BOOL, Rht = BOOL
                                case ARRAY_INT:         // Res = BOOL, Rht = INT
                                case ARRAY_APA:         // Res = BOOL, Rht = APA
                                    // Convert the BOOL/INT/APA to a RAT
                                    mpq_set_sx (&aplRatRht, aplIntegerRht, 1);

                                    break;

                                case ARRAY_FLOAT:       // Res = BOOL, Rht = FLOAT
                                    // Convert the FLOAT to a RAT
                                    mpq_set_d  (&aplRatRht, aplFloatRht);

                                    break;

                                case ARRAY_RAT:         // Res = BOOL, Rht = RAT
                                    // Copy the RAT to a RAT
                                    mpq_set (&aplRatRht, (LPAPLRAT) lpSymGlbRht);

                                    break;

                                case ARRAY_CHAR:        // Can't happen
                                case ARRAY_VFP:         // Can't happen
                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        case ARRAY_VFP:                 // Res = BOOL
                            // Split cases based upon the right arg storage type
                            switch (aplTypeRht)
                            {
                                case ARRAY_BOOL:        // Res = BOOL, Rht = BOOL
                                case ARRAY_INT:         // Res = BOOL, Rht = INT
                                case ARRAY_APA:         // Res = BOOL, Rht = APA
                                    // Convert the BOOL/INT/APA to a VFP
                                    mpfr_set_sx (&aplVfpRht, aplIntegerRht, MPFR_RNDN);

                                    break;

                                case ARRAY_FLOAT:       // Res = BOOL, Rht = FLOAT
                                    // Convert the FLOAT to a VFP
                                    mpfr_set_d (&aplVfpRht, aplFloatRht, MPFR_RNDN);

                                    break;

                                case ARRAY_RAT:         // Res = BOOL, Rht = RAT
                                    // Convert the RAT to a VFP
                                    mpfr_set_q (&aplVfpRht, (LPAPLRAT) lpSymGlbRht, MPFR_RNDN);

                                    break;

                                case ARRAY_VFP:         // Res = BOOL, Rht = VFP
                                    // Copy the VFP to a VFP
                                    mpfr_copy (&aplVfpRht, (LPAPLVFP) lpSymGlbRht);

                                    break;

                                case ARRAY_CHAR:        // Can't happen
                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    switch (aplTypeCom)
                    {
                        case ARRAY_RAT:                 // Res = BOOL, Lft = RAT, Rht = RAT
                            // Save the result
                            lptkRes->tkData.tkBoolean  =
                              (*lpPrimSpec->BisRvR) (aplRatLft,
                                                     aplRatRht,
                                                     lpPrimSpec);
                            break;

                        case ARRAY_VFP:                 // Res = BOOL, Lft = VFP, Rht = VFP
                            // Save the result
                            lptkRes->tkData.tkBoolean  =
                              (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                     aplVfpRht,
                                                     lpPrimSpec);
                            break;

                        defstop
                            break;
                    } // End SWITCH

                    // We no longer need this storage
                    Myf_clear (&aplVfpRht);
                    Myf_clear (&aplVfpLft);
                    Myq_clear (&aplRatRht);
                    Myq_clear (&aplRatLft);
                } else
                    DbgStop ();         // We should never get here

                break;

            case ARRAY_INT:                     // Res = INT
                // If both left and right arguments are integer-like (BOOL, INT, or APA),
                //   use IisIvI
                if (IsSimpleInt (aplTypeLft)
                 && IsSimpleInt (aplTypeRht))  // Res = INT, Lft = BOOL/INT/APA(S), Rht = BOOL/INT/APA(S)
                    lptkRes->tkData.tkInteger  =
                      (*lpPrimSpec->IisIvI) (aplIntegerLft,
                                             aplIntegerRht,
                                             lpPrimSpec);
                else
                // If either argument is FLOAT and the other is simple numeric (BOOL, INT, APA, or FLOAT),
                //   use IisFvF
                if ((IsSimpleFlt (aplTypeLft) && IsSimpleNum (aplTypeRht))   // Res = INT, Lft = FLOAT(S), Rht = BOOL/INT/APA/FLOAT(S)
                 || (IsSimpleFlt (aplTypeRht) && IsSimpleNum (aplTypeLft)))  // Res = INT, Lft = BOOL/INT/APA/FLOAT(S), Rht = FLOAT(S)
                    lptkRes->tkData.tkInteger  =
                      (*lpPrimSpec->IisFvF) (aplFloatLft,
                                             aplFloatRht,
                                             lpPrimSpec);
                else
                    DbgStop ();         // We should never get here
                break;

            case ARRAY_FLOAT:                   // Res = FLOAT
                // If both arguments are simple integer (BOOL, INT, APA),
                //   use FisIvI
                if (IsSimpleInt (aplTypeLft)
                 && IsSimpleInt (aplTypeRht))   // Res = FLOAT, Lft = BOOL/INT/APA(S), Rht = BOOL/INT/APA(S)
                    lptkRes->tkData.tkFloat    =
                      (*lpPrimSpec->FisIvI) (aplIntegerLft,
                                             aplIntegerRht,
                                             lpPrimSpec);
                else
                // If both arguments are simple numeric (BOOL, INT, APA, FLOAT),
                //   use FisFvF
                if (IsSimpleNum (aplTypeLft)
                 && IsSimpleNum (aplTypeRht))   // Res = FLOAT, Lft = BOOL/INT/APA/FLOAT(S), Rht = BOOL/INT/APA/FLOAT(S)
                    lptkRes->tkData.tkFloat    =
                      (*lpPrimSpec->FisFvF) (aplFloatLft,
                                             aplFloatRht,
                                             lpPrimSpec);
                else
                // If both arguments are numeric (RAT, VFP, ...)
                if (IsNumeric (aplTypeLft)
                 && IsNumeric (aplTypeRht))     // Res = FLOAT, Lft = RAT/VFP/...(S), Rht = RAT/VFP/...(S)
                {
                    DbgBrk ();          // Can't happen with any known primitive






                } else
                    DbgStop ();         // We should never get here
                break;

            case ARRAY_RAT:                     // Res = RAT
                // If the result global memory handle is NULL, ...
                if (lphGlbRes EQ NULL || *lphGlbRes EQ NULL)
                {
                    // Allocate our own global memory and return it in lptkRes

                    //***************************************************************
                    // Calculate space needed for the result
                    //***************************************************************
                    ByteRes = CalcArraySize (aplTypeRes, 1, 0);

                    //***************************************************************
                    // Check for overflow
                    //***************************************************************
                    if (ByteRes NE (APLU3264) ByteRes)
                        goto WSFULL_EXIT;

                    //***************************************************************
                    // Now we can allocate the storage for the result
                    //***************************************************************
                    hGlbTmp = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
                    if (lphGlbRes NE NULL)
                       *lphGlbRes = hGlbTmp;
                    if (hGlbTmp EQ NULL)
                        goto WSFULL_EXIT;
                    // Lock the memory to get a ptr to it
                    lpMemHdrRes = MyGlobalLock000 (hGlbTmp);

#define lpHeader        lpMemHdrRes
                    // Fill in the header
                    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
                    lpHeader->ArrType    = aplTypeRes;
////////////////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////////////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
                    lpHeader->RefCnt     = 1;
                    lpHeader->NELM       = 1;
                    lpHeader->Rank       = 0;
#undef  lpHeader
                } else
                {
                    hGlbTmp = *lphGlbRes;

                    // Lock the memory to get a ptr to it
                    lpMemHdrRes = MyGlobalLockVar (hGlbTmp);

                    // If the result is not nested, ...
                    if (!IsNested (lpMemHdrRes->ArrType))
                        // In case we promoted the result, ...
                        lpMemHdrRes->ArrType = aplTypeRes;
                } // End IF/ELSE

                // Skip over the header and dimensions to the data
                lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

                // Promote the left arg to Rational
                // Split cases based upon the left arg type
                switch (aplTypeLft)
                {
                    case ARRAY_BOOL:            // Res = RAT, Lft = BOOL
                    case ARRAY_INT:             // Res = RAT, Lft = INT
                    case ARRAY_APA:             // Res = RAT, Lft = APA
                        mpq_init_set_sx (&aplRatLft, aplIntegerLft, 1);

                        break;

                    case ARRAY_RAT:             // Res = RAT, Lft = RAT
                        mpq_init_set    (&aplRatLft, (LPAPLRAT) lpSymGlbLft);

                        break;

                    case ARRAY_FLOAT:           // Res = RAT, Lft = FLOAT
                        mpq_init_set_d  (&aplRatLft, aplFloatLft);

                        break;

                    case ARRAY_CHAR:            // Can't happen
                    case ARRAY_VFP:             // Can't happen
                    defstop
                        break;
                } // End SWITCH

                // Promote the right arg to Rational
                // Split cases based upon the right arg type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:            // Res = RAT, Rht = BOOL
                    case ARRAY_INT:             // Res = RAT, Rht = INT
                    case ARRAY_APA:             // Res = RAT, Rht = APA
                        mpq_init_set_sx (&aplRatRht, aplIntegerRht, 1);

                        break;

                    case ARRAY_RAT:             // Res = RAT, Rht = RAT
                        mpq_init_set    (&aplRatRht, (LPAPLRAT) lpSymGlbRht);

                        break;

                    case ARRAY_FLOAT:           // Res = RAT, Rht = FLOAT
                        mpq_init_set_d  (&aplRatRht, aplFloatRht);

                        break;

                    case ARRAY_CHAR:            // Can't happen
                    case ARRAY_VFP:             // Can't happen
                    defstop
                        break;
                } // End SWITCH

                // Save the result
                aplRatRes =
                  (*lpPrimSpec->RisRvR) (aplRatLft,
                                         aplRatRht,
                                         lpPrimSpec);
                // If the result is nested, ...
                if (IsNested (lpMemHdrRes->ArrType))
                    // Make a global entry and save in the nested result
                    ((LPAPLNESTED) lpMemRes)[0] =
                      MakeGlbEntry_EM (ARRAY_RAT,   // Entry type
                                      &aplRatRes,   // Ptr to the value
                                       FALSE,       // TRUE iff we should initialize the target first
                                       lptkFunc);   // Ptr to function token
                else
                    // Save in the result
                    ((LPAPLRAT) lpMemRes)[0] = aplRatRes;

                // We no longer need this ptr
                MyGlobalUnlock (hGlbTmp); lpMemHdrRes = NULL;

                // Save in the result
                lptkRes->tkData.tkGlbData = MakePtrTypeGlb (hGlbTmp);

                // We no longer need this storage
                Myq_clear (&aplRatRht);
                Myq_clear (&aplRatLft);

                break;

            case ARRAY_VFP:                     // Res = VFP
                // If the result global memory handle is NULL, ...
                if (lphGlbRes EQ NULL || *lphGlbRes EQ NULL)
                {
                    // Allocate our own global memory and return it in lptkRes

                    //***************************************************************
                    // Calculate space needed for the result
                    //***************************************************************
                    ByteRes = CalcArraySize (aplTypeRes, 1, 0);

                    //***************************************************************
                    // Check for overflow
                    //***************************************************************
                    if (ByteRes NE (APLU3264) ByteRes)
                        goto WSFULL_EXIT;

                    //***************************************************************
                    // Now we can allocate the storage for the result
                    //***************************************************************
                    hGlbTmp = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
                    if (lphGlbRes NE NULL)
                       *lphGlbRes = hGlbTmp;
                    if (hGlbTmp EQ NULL)
                        goto WSFULL_EXIT;
                    // Lock the memory to get a ptr to it
                    lpMemHdrRes = MyGlobalLock000 (hGlbTmp);

#define lpHeader        lpMemHdrRes
                    // Fill in the header
                    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
                    lpHeader->ArrType    = aplTypeRes;
////////////////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////////////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
                    lpHeader->RefCnt     = 1;
                    lpHeader->NELM       = 1;
                    lpHeader->Rank       = 0;
#undef  lpHeader
                } else
                {
                    hGlbTmp = *lphGlbRes;

                    // Lock the memory to get a ptr to it
                    lpMemHdrRes = MyGlobalLockVar (hGlbTmp);

                    // If the result is not nested, ...
                    if (!IsNested (lpMemHdrRes->ArrType))
                        // In case we promoted the result, ...
                        lpMemHdrRes->ArrType = aplTypeRes;
                } // End IF/ELSE

                // Skip over the header and dimensions to the data
                lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

                // Promote the left arg to VFP
                // Split cases based upon the left arg type
                switch (aplTypeLft)
                {
                    case ARRAY_BOOL:            // Res = VFP, Lft = BOOL
                    case ARRAY_INT:             // Res = VFP, Lft = INT
                    case ARRAY_APA:             // Res = VFP, Lft = APA
                        mpfr_init_set_sx (&aplVfpLft, aplIntegerLft, MPFR_RNDN);

                        break;

                    case ARRAY_FLOAT:           // Res = VFP, Lft = FLOAT
                        mpfr_init_set_d  (&aplVfpLft, aplFloatLft, MPFR_RNDN);

                        break;

                    case ARRAY_RAT:             // Res = VFP, Lft = RAT
                        mpfr_init_set_q  (&aplVfpLft, (LPAPLRAT) lpSymGlbLft, MPFR_RNDN);

                        break;

                    case ARRAY_VFP:             // Res = VFP, Lft = VFP
                        mpfr_init_copy   (&aplVfpLft, (LPAPLVFP) lpSymGlbLft);

                        break;

                    case ARRAY_CHAR:            // Can't happen
                    defstop
                        break;
                } // End SWITCH

                // Promote the right arg to VFP
                // Split cases based upon the right arg type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:            // Res = VFP, Rht = BOOL
                    case ARRAY_INT:             // Res = VFP, Rht = INT
                    case ARRAY_APA:             // Res = VFP, Rht = APA
                        mpfr_init_set_sx (&aplVfpRht, aplIntegerRht, MPFR_RNDN);

                        break;

                    case ARRAY_FLOAT:           // Res = VFP, Rht = FLOAT
                        mpfr_init_set_d  (&aplVfpRht, aplFloatRht, MPFR_RNDN);

                        break;

                    case ARRAY_RAT:             // Res = VFP, Rht = RAT
                        mpfr_init_set_q  (&aplVfpRht, (LPAPLRAT) lpSymGlbRht, MPFR_RNDN);

                        break;

                    case ARRAY_VFP:             // Res = VFP, Rht = VFP
                        mpfr_init_copy   (&aplVfpRht, (LPAPLVFP) lpSymGlbRht);

                        break;

                    case ARRAY_CHAR:
                    defstop
                        break;
                } // End SWITCH

                // Save the result
                aplVfpRes =
                  (*lpPrimSpec->VisVvV) (aplVfpLft,
                                         aplVfpRht,
                                         lpPrimSpec);
                // If the result is nested, ...
                if (IsNested (lpMemHdrRes->ArrType))
                    // Make a global entry and save in the nested result
                    ((LPAPLNESTED) lpMemRes)[0] =
                      MakeGlbEntry_EM (ARRAY_VFP,   // Entry type
                                      &aplVfpRes,   // Ptr to the value
                                       FALSE,       // TRUE iff we should initialize the target first
                                       lptkFunc);   // Ptr to function token
                else
                    // Save in the result
                    ((LPAPLVFP) lpMemRes)[0] = aplVfpRes;

                // We no longer need this storage
                Myf_clear (&aplVfpRht);
                Myf_clear (&aplVfpLft);

                // We no longer need this ptr
                MyGlobalUnlock (hGlbTmp); lpMemHdrRes = NULL;

                // Save in the result
                lptkRes->tkData.tkGlbData = MakePtrTypeGlb (hGlbTmp);

                break;

            defstop
                break;
        } // End SWITCH
    } __except (CheckException (GetExceptionInformation (), L"PrimFnDydSiScSiScSub_EM"))
    {
        EXCEPTION_CODES ExceptionCode = MyGetExceptionCode ();  // The exception code

        dprintfWL9 (L"!!Initiating Exception in " APPEND_NAME L" #7: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

        // Split cases based upon the ExceptionCode
        switch (ExceptionCode)
        {
            case EXCEPTION_DOMAIN_ERROR:
            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            case EXCEPTION_INT_DIVIDE_BY_ZERO:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                goto DOMAIN_EXIT;

            case EXCEPTION_WS_FULL:
                goto WSFULL_EXIT;

            case EXCEPTION_NONCE_ERROR:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                goto NONCE_EXIT;

            case EXCEPTION_RESULT_RAT:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                if (IsNumeric (aplTypeRes)
                 && !IsRat    (aplTypeRes))
                {
                    if (lphGlbRes NE NULL && *lphGlbRes NE NULL)
                    {
                        if (lpMemHdrRes NE NULL)
                        {
                            // We need to start over with the result
                            MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
                        } // End IF

                        // We no longer need this storage
                        FreeResultGlobalVar (*lphGlbRes); *lphGlbRes = NULL;
                    } else
                    if (hGlbTmp NE NULL)
                    {
                        if (lpMemHdrRes NE NULL)
                        {
                            // We need to start over with the result
                            MyGlobalUnlock (hGlbTmp); lpMemHdrRes = NULL;
                        } // End IF

                        // We no longer need this storage
                        FreeResultGlobalVar (hGlbTmp); hGlbTmp = NULL;
                    } // End IF/ELSE

                    // It's now a RAT result
                    aplTypeRes = ARRAY_RAT;

                    dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #7: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                    goto RESTART_EXCEPTION_IMMED;
                } // End IF

                // Display message for unhandled exception
                DisplayException ();

                break;

            case EXCEPTION_RESULT_VFP:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                if (IsNumeric (aplTypeRes)
                 && !IsVfp    (aplTypeRes))
                {
                    if (lphGlbRes NE NULL && *lphGlbRes NE NULL)
                    {
                        if (lpMemHdrRes NE NULL)
                        {
                            // We need to start over with the result
                            MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
                        } // End IF

                        // We no longer need this storage
                        FreeResultGlobalVar (*lphGlbRes); *lphGlbRes = NULL;
                    } else
                    if (hGlbTmp NE NULL)
                    {
                        if (lpMemHdrRes NE NULL)
                        {
                            // We need to start over with the result
                            MyGlobalUnlock (hGlbTmp); lpMemHdrRes = NULL;
                        } // End IF

                        // We no longer need this storage
                        FreeResultGlobalVar (hGlbTmp); hGlbTmp = NULL;
                    } // End IF/ELSE

                    // It's now a VFP result
                    aplTypeRes = ARRAY_VFP;

                    dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #7: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                    goto RESTART_EXCEPTION_IMMED;
                } // End IF

                // Display message for unhandled exception
                DisplayException ();

                break;

            case EXCEPTION_RESULT_FLOAT:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                if (IsNumeric (aplTypeRes)
                 && !IsSimpleFlt (aplTypeRes))
                {
                    if (lphGlbRes NE NULL && *lphGlbRes NE NULL)
                    {
                        if (lpMemHdrRes NE NULL)
                        {
                            // We need to start over with the result
                            MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
                        } // End IF

                        // We no longer need this storage
                        FreeResultGlobalVar (*lphGlbRes); *lphGlbRes = NULL;
                    } else
                    if (hGlbTmp NE NULL)
                    {
                        if (lpMemHdrRes NE NULL)
                        {
                            // We need to start over with the result
                            MyGlobalUnlock (hGlbTmp); lpMemHdrRes = NULL;
                        } // End IF

                        // We no longer need this storage
                        FreeResultGlobalVar (hGlbTmp); hGlbTmp = NULL;
                    } // End IF/ELSE

                    // It's now a FLOAT result
                    aplTypeRes = ARRAY_FLOAT;

                    dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #7: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                    goto RESTART_EXCEPTION_IMMED;
                } // End IF

                // Fall through to never-never-land

            default:
                // Display message for unhandled exception
                DisplayException ();

                break;
        } // End SWITCH
    } // End __try/__except

    goto NORMAL_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbTmp NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbTmp); lpMemHdrRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalVar (hGlbTmp); hGlbTmp = NULL;

        // If the caller passed it in, ...
        if (lphGlbRes NE NULL)
            *lphGlbRes = NULL;
    } // End IF

    bRet = FALSE;
NORMAL_EXIT:
    // We no longer need this storage
    Myq_clear (&aplRatRht);
    Myq_clear (&aplRatLft);
    Myf_clear (&aplVfpRht);
    Myf_clear (&aplVfpLft);

    // Return as (possibly new) storage type
    *lpaplTypeRes = aplTypeRes;

    return bRet;
} // End PrimFnDydSiScSiScSub_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydSimpSimp_EM
//
//  Dyadic primitive scalar function,
//    left simple or global numeric, right simple or global numeric
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSimpSimp_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydSimpSimp_EM
    (LPPL_YYSTYPE      lpYYRes,         // Ptr to the result

     LPTOKEN           lptkLftArg,      // Ptr to left arg token
     LPTOKEN           lptkFunc,        // ...    function ...
     LPTOKEN           lptkRhtArg,      // ...    right arg ...

     HGLOBAL           hGlbLft,         // Left arg handle
     HGLOBAL           hGlbRht,         // Right ...
     HGLOBAL          *lphGlbRes,       // Ptr to result handle

     LPVARARRAY_HEADER lpMemHdrLft,     // Ptr to left arg header
     LPVARARRAY_HEADER lpMemHdrRht,     // ...    right ...

     LPAPLUINT         lpMemAxisHead,   // Ptr to axis values, fleshed out by CheckAxis_EM
     LPAPLUINT         lpMemAxisTail,   // Ptr to grade up of AxisHead

     APLRANK           aplRankLft,      // Left arg rank
     APLRANK           aplRankRht,      // Right ...
     APLRANK           aplRankRes,      // Result ...

     APLSTYPE          aplTypeLft,      // Left arg type
     APLSTYPE          aplTypeRht,      // Right ...
     APLSTYPE          aplTypeRes,      // Result ...

     APLNELM           aplNELMLft,      // Left arg NELM
     APLNELM           aplNELMRht,      // Right ...
     APLNELM           aplNELMRes,      // Result ...
     APLNELM           aplNELMAxis,     // Axis ...

     UBOOL             bLftIdent,       // TRUE iff the function has a left identity element and the Axis tail is valid
     UBOOL             bRhtIdent,       // ...                         right ...

     LPPRIMSPEC        lpPrimSpec)      // Ptr to local PRIMSPEC

{
    LPVOID            lpMemLft,             // Ptr to left arg global memory
                      lpMemRht,             // Ptr to right ...
                      lpMemRes;             // Ptr to result   ...
    LPVARARRAY_HEADER lpMemHdrRes = NULL,   // Ptr to result header (in case we blow up)
                      lpMemHdrArg = NULL;   // Ptr to left/right ...
    UBOOL             bRet = TRUE;          // TRUE iff result is valid
    APLSTYPE          aplTypeArg;
    APLINT            aplIntegerLft,
                      aplIntegerRht;
    APLFLOAT          aplFloatLft,
                      aplFloatRht;
    LPAPLUINT         lpMemWVec = NULL,
                      lpMemOdo = NULL;
    HGLOBAL           hGlbWVec = NULL,
                      hGlbOdo = NULL;
    APLINT            uLft,
                      uRht,
                      uRes;
    LPAPLDIM          lpMemDimArg,
                      lpMemDimRes;
    APLINT            apaOffLft,
                      apaMulLft,
                      apaOffRht,
                      apaMulRht,
                      iRht;
    UINT              uBitIndex = 0;
    APLUINT           ByteRes;          // # bytes in the result
    APLCHAR           aplCharLft,
                      aplCharRht;
    LPPLLOCALVARS     lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;     // Ptr to Ctrl-Break flag
    APLNELM           aplNELMTmp,       // Temporary NELM
                      aplNELMRem;       // Remaining NELM
    HGLOBAL           lpSymGlbLft,      // Left arg Sym/Glb
                      lpSymGlbRht;      // Right ...
    APLRAT            aplRatLft = {0},  // Left arg as Rational
                      aplRatRht = {0};  // Right ...
    APLVFP            aplVfpLft = {0},  // Left arg as VFP
                      aplVfpRht = {0};  // Right ...
    APLSTYPE          aplTypeHetLft,    // Left hetero types
                      aplTypeHetRht,    // Right ...
                      aplTypeNew,       // New result
                      aplTypeCom;       // Common storage type

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    //***************************************************************
    // Both arguments are simple
    //***************************************************************

    // Lock the memory to get a ptr to it
    if (*lphGlbRes NE NULL)
        lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);

    if (IsSingleton (aplNELMLft)
     && IsSingleton (aplNELMRht))
    {
        //***************************************************************
        // Both args are singletons
        //***************************************************************

        // Axis may be anything

        // If the result is a scalar, ...
        if (IsScalar (aplRankRes))
        {
            // Get the respective first values
            GetFirstValueToken (lptkLftArg,         // Ptr to left arg token
                               &aplIntegerLft,      // Ptr to integer result
                               &aplFloatLft,        // Ptr to float ...
                               &aplCharLft,         // Ptr to WCHAR ...
                                NULL,               // Ptr to longest ...
                               &lpSymGlbLft,        // Ptr to lpSym/Glb ...
                                NULL,               // Ptr to ...immediate type ...
                                NULL);              // Ptr to array type ...
            GetFirstValueToken (lptkRhtArg,         // Ptr to right arg token
                               &aplIntegerRht,      // Ptr to integer result
                               &aplFloatRht,        // Ptr to float ...
                               &aplCharRht,         // Ptr to WCHAR ...
                                NULL,               // Ptr to longest ...
                               &lpSymGlbRht,        // Ptr to lpSym/Glb ...
                                NULL,               // Ptr to ...immediate type ...
                                NULL);              // Ptr to array type ...
            bRet =
              PrimFnDydSiScSiScSub_EM (&lpYYRes->tkToken,
                                        lptkFunc,
                                        lphGlbRes,
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
                                       &aplTypeNew,
                                        lpPrimSpec);
            // Because the result is contained entirely within lpYYRes->tkToken, we don't
            //   need to do anything special in the case where aplTypeRes NE aplTypeNew.
        } else
        // It's a singleton array
        {
            // Get the respective values
            GetFirstValueToken (lptkLftArg,     // Ptr to left arg token
                               &aplIntegerLft,  // Ptr to integer result
                               &aplFloatLft,    // Ptr to float ...
                               &aplCharLft,     // Ptr to WCHAR ...
                                NULL,           // Ptr to longest ...
                               &lpSymGlbLft,    // Ptr to lpSym/Glb ...
                                NULL,           // Ptr to ...immediate type ...
                                NULL);          // Ptr to array type ...
            GetFirstValueToken (lptkRhtArg,     // Ptr to right arg token
                               &aplIntegerRht,  // Ptr to integer result
                               &aplFloatRht,    // Ptr to float ...
                               &aplCharRht,     // Ptr to WCHAR ...
                                NULL,           // Ptr to longest ...
                               &lpSymGlbRht,    // Ptr to lpSym/Glb ...
                                NULL,           // Ptr to ...immediate type ...
                                NULL);          // Ptr to array type ...
RESTART_EXCEPTION_SINGLETON:
            // Skip over the header and dimensions to the data
            lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

            // Skip over the header to the dimensions
            lpMemDimRes = VarArrayBaseToDim (lpMemHdrRes);

            __try
            {
            // Split cases based upon the result's storage type
            switch (aplTypeRes)
            {
                case ARRAY_BOOL:                    // Res = BOOL
                    // If both arguments are BOOL,
                    //   use BisBvB
                    if (IsSimpleBool (aplTypeLft)
                     && IsSimpleBool (aplTypeRht))  // Res = BOOL, Lft = BOOL(S), Rht = BOOL(S)
                    {
                        *((LPAPLBOOL)  lpMemRes) =
                          (*lpPrimSpec->BisBvB) ((APLBOOL) aplIntegerLft,
                                                 (APLBOOL) aplIntegerRht,
                                                 lpPrimSpec);
                    } else
                    // If both arguments are integer-like (BOOL, INT, or APA),
                    //   use BisIvI
                    if (IsSimpleInt (aplTypeLft)
                     && IsSimpleInt (aplTypeRht))  // Res = BOOL, Lft = BOOL/INT/APA(S), Rht = BOOL/INT/APA(S)
                    {
                        *((LPAPLBOOL)  lpMemRes) =
                          (*lpPrimSpec->BisIvI) (aplIntegerLft,
                                                 aplIntegerRht,
                                                 lpPrimSpec);
                    } else
                    // If both arguments are CHAR,
                    //   use BisCvC
                    if (IsSimpleChar (aplTypeLft)
                     && IsSimpleChar (aplTypeRht))
                    {
                        *((LPAPLBOOL)  lpMemRes) =
                          (*lpPrimSpec->BisCvC) (aplCharLft,
                                                 aplCharRht,
                                                 lpPrimSpec);
                    } else
                    // If either argument is FLOAT and the other is simple numeric (BOOL, INT, APA, or FLOAT),
                    //   use BisFvF
                    if ((IsSimpleFlt (aplTypeLft) && IsSimpleNum (aplTypeRht))   // Res = BOOL, Lft = FLOAT(S), Rht = BOOL/INT/APA/FLOAT(S)
                     || (IsSimpleFlt (aplTypeRht) && IsSimpleNum (aplTypeLft)))  // Res = BOOL, Lft = BOOL/INT/APA/FLOAT(S), Rht = FLOAT(S)
                    {
                        *((LPAPLBOOL)  lpMemRes) =
                          (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                 aplFloatRht,
                                                 lpPrimSpec);
                    } else
                    // If one arg is numeric and the other char, ...
                    if ((IsNumeric (aplTypeLft) && IsSimpleChar (aplTypeRht))     // Res = BOOL, Lft = BOOL/INT/APA/FLOAT(S), Rht = CHAR(S)
                     || (IsNumeric (aplTypeRht) && IsSimpleChar (aplTypeLft)))    // Res = BOOL, Lft = CHAR(S)              , Rht = BOOL/INT/APA/FLOAT(S)
                    {
                        // One arg is numeric, the other char
                        Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                             || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);

                        // If the function is not-equal, the result is 1
                        if (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL)
                            *((LPAPLBOOL)  lpMemRes) = 1;
                    } else
                    // If either argument is global numeric, ...
                    if (IsGlbNum (aplTypeLft)
                     || IsGlbNum (aplTypeRht))
                    {
                        // Calculate the common storage type
                        aplTypeCom = aTypePromote[aplTypeLft][aplTypeRht];

                        // Initialize the temps
                        mpq_init (&aplRatLft);
                        mpq_init (&aplRatRht);
                        mpfr_init0 (&aplVfpLft);
                        mpfr_init0 (&aplVfpRht);

                        // Promote the left arg to the common type
                        switch (aplTypeCom)
                        {
                            case ARRAY_RAT:                 // Res = BOOL
                                // Split cases based upon the left arg storage type
                                switch (aplTypeLft)
                                {
                                    case ARRAY_BOOL:        // Res = BOOL, Lft = BOOL
                                    case ARRAY_INT:         // Res = BOOL, Lft = INT
                                    case ARRAY_APA:         // Res = BOOL, Lft = APA
                                        // Convert the BOOL/INT/APA to a RAT
                                        mpq_set_sx (&aplRatLft, aplIntegerLft, 1);

                                        break;

                                    case ARRAY_FLOAT:       // Res = BOOL, Lft = FLOAT
                                        // Convert the FLOAT to a RAT
                                        mpq_set_d  (&aplRatLft, aplFloatLft);

                                        break;

                                    case ARRAY_RAT:         // Res = BOOL, Lft = RAT
                                        // Copy the RAT to a RAT
                                        mpq_set (&aplRatLft, (LPAPLRAT) lpSymGlbLft);

                                        break;

                                    case ARRAY_CHAR:        // Can't happen
                                    case ARRAY_VFP:         // Can't happen
                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            case ARRAY_VFP:                 // Res = BOOL
                                // Split cases based upon the left arg storage type
                                switch (aplTypeLft)
                                {
                                    case ARRAY_BOOL:        // Res = BOOL, Rht = BOOL
                                    case ARRAY_INT:         // Res = BOOL, Rht = INT
                                    case ARRAY_APA:         // Res = BOOL, Rht = APA
                                        // Convert the BOOL/INT/APA to a VFP
                                        mpfr_set_sx (&aplVfpLft, aplIntegerLft, MPFR_RNDN);

                                        break;

                                    case ARRAY_FLOAT:       // Res = BOOL, Rht = FLOAT
                                        // Convert the FLOAT to a VFP
                                        mpfr_set_d (&aplVfpLft, aplFloatLft, MPFR_RNDN);

                                        break;

                                    case ARRAY_RAT:         // Res = BOOL, Rht = RAT
                                        // Convert the RAT to a VFP
                                        mpfr_set_q (&aplVfpLft, (LPAPLRAT) lpSymGlbLft, MPFR_RNDN);

                                        break;

                                    case ARRAY_VFP:         // Res = BOOL, Rht = VFP
                                        // Copy the VFP to a VFP
                                        mpfr_copy (&aplVfpLft, (LPAPLVFP) lpSymGlbLft);

                                        break;

                                    case ARRAY_CHAR:        // Can't happen
                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            defstop
                                break;
                        } // End SWITCH

                        // Promote the right arg to the common type
                        switch (aplTypeCom)
                        {
                            case ARRAY_RAT:                 // Res = BOOL
                                // Split cases based upon the right arg storage type
                                switch (aplTypeRht)
                                {
                                    case ARRAY_BOOL:        // Res = BOOL, Rht = BOOL
                                    case ARRAY_INT:         // Res = BOOL, Rht = INT
                                    case ARRAY_APA:         // Res = BOOL, Rht = APA
                                        // Convert the BOOL/INT/APA to a RAT
                                        mpq_set_sx (&aplRatRht, aplIntegerRht, 1);

                                        break;

                                    case ARRAY_FLOAT:       // Res = BOOL, Rht = FLOAT
                                        // Convert the FLOAT to a RAT
                                        mpq_set_d  (&aplRatRht, aplFloatRht);

                                        break;

                                    case ARRAY_RAT:         // Res = BOOL, Rht = RAT
                                        // Copy the RAT to a RAT
                                        mpq_set (&aplRatRht, (LPAPLRAT) lpSymGlbRht);

                                        break;

                                    case ARRAY_CHAR:        // Can't happen
                                    case ARRAY_VFP:         // Can't happen
                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            case ARRAY_VFP:                 // Res = BOOL
                                // Split cases based upon the right arg storage type
                                switch (aplTypeRht)
                                {
                                    case ARRAY_BOOL:        // Res = BOOL, Rht = BOOL
                                    case ARRAY_INT:         // Res = BOOL, Rht = INT
                                    case ARRAY_APA:         // Res = BOOL, Rht = APA
                                        // Convert the BOOL/INT/APA to a VFP
                                        mpfr_set_sx (&aplVfpRht, aplIntegerRht, MPFR_RNDN);

                                        break;

                                    case ARRAY_FLOAT:       // Res = BOOL, Rht = FLOAT
                                        // Convert the FLOAT to a VFP
                                        mpfr_set_d (&aplVfpRht, aplFloatRht, MPFR_RNDN);

                                        break;

                                    case ARRAY_RAT:         // Res = BOOL, Rht = RAT
                                        // Convert the RAT to a VFP
                                        mpfr_set_q (&aplVfpRht, (LPAPLRAT) lpSymGlbRht, MPFR_RNDN);

                                        break;

                                    case ARRAY_VFP:         // Res = BOOL, Rht = VFP
                                        // Copy the VFP to a VFP
                                        mpfr_copy (&aplVfpRht, (LPAPLVFP) lpSymGlbRht);

                                        break;

                                    case ARRAY_CHAR:        // Can't happen
                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            defstop
                                break;
                        } // End SWITCH

                        // Split cases based upon the common type
                        switch (aplTypeCom)
                        {
                            case ARRAY_RAT:                 // Res = BOOL, Lft = RAT, Rht = RAT
                                *((LPAPLBOOL)  lpMemRes) =
                                  (*lpPrimSpec->BisRvR) (aplRatLft,
                                                         aplRatRht,
                                                         lpPrimSpec) << uBitIndex;
                                break;

                            case ARRAY_VFP:                 // Res = BOOL, Lft = VFP, Rht = VFP
                                *((LPAPLBOOL)  lpMemRes) =
                                  (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                         aplVfpRht,
                                                         lpPrimSpec) << uBitIndex;
                                break;

                            defstop
                                break;
                        } // End SWITCH

                        // We no longer need this storage
                        Myf_clear (&aplVfpRht);
                        Myf_clear (&aplVfpLft);
                        Myq_clear (&aplRatRht);
                        Myq_clear (&aplRatLft);
                    } else
                        DbgStop ();         // We should never get here
                    break;

                case ARRAY_INT:                     // Res = INT
                    // If both left and right arguments are integer-like (BOOL, INT, or APA),
                    //   use IisIvI
                    if (IsSimpleInt (aplTypeLft)
                     && IsSimpleInt (aplTypeRht))  // Res = INT, Lft = BOOL/INT/APA(S), Rht = BOOL/INT/APA(S)
                    {
                        *((LPAPLINT)   lpMemRes) =
                          (*lpPrimSpec->IisIvI) (aplIntegerLft,
                                                 aplIntegerRht,
                                                 lpPrimSpec);
                    } else
                    // If either argument is FLOAT and the other is simple numeric (BOOL, INT, APA, or FLOAT),
                    //   use IisFvF
                    if ((IsSimpleFlt (aplTypeLft) && IsSimpleNum (aplTypeRht))   // Res = INT, Lft = FLOAT(S), Rht = BOOL/INT/APA/FLOAT(S)
                     || (IsSimpleFlt (aplTypeRht) && IsSimpleNum (aplTypeLft)))  // Res = INT, Lft = BOOL/INT/APA/FLOAT(S), Rht = FLOAT(S)
                    {
                        *((LPAPLINT)   lpMemRes) =
                          (*lpPrimSpec->IisFvF) (aplFloatLft,
                                                 aplFloatRht,
                                                 lpPrimSpec);
                    } else
                        DbgStop ();         // We should never get here
                    break;

                case ARRAY_FLOAT:                   // Res = FLOAT
                    // If both arguments are simple integer (BOOL, INT, APA),
                    //   use FisIvI
                    if (IsSimpleInt (aplTypeLft)
                     && IsSimpleInt (aplTypeRht))  // Res = FLOAT, Lft = BOOL/INT/APA(S), Rht = BOOL/INT/APA(S)
                    {
                        // Save in the result
                        *((LPAPLFLOAT) lpMemRes) =
                          (*lpPrimSpec->FisIvI) (aplIntegerLft,
                                                 aplIntegerRht,
                                                 lpPrimSpec);
                    } else
                    // If both arguments are simple numeric (BOOL, INT, APA, FLOAT),
                    //   use FisFvF
                    if (IsSimpleNum (aplTypeLft)
                     && IsSimpleNum (aplTypeRht))  // Res = FLOAT, Lft = BOOL/INT/APA/FLOAT(S), Rht = BOOL/INT/APA/FLOAT(S)
                    {
                        // Save in the result
                        *((LPAPLFLOAT) lpMemRes) =
                          (*lpPrimSpec->FisFvF) (aplFloatLft,
                                                 aplFloatRht,
                                                 lpPrimSpec);
                    } else
                    // If both arguments are numeric (RAT, VFP, ...)
                    if (IsNumeric (aplTypeLft)
                     && IsNumeric (aplTypeRht))    // Res = FLOAT, Lft = RAT/VFP/...(S), Rht = RAT/VFP/...(S)
                    {
                        DbgBrk ();          // Can't happen with any known primitive






                    } else
                        DbgStop ();         // We should never get here
                    break;

                case ARRAY_RAT:                     // Res = RAT
                    // Promote the left arg to Rational
                    // Split cases based upon the left arg type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:            // Res = RAT, Lft = BOOL
                        case ARRAY_INT:             // Res = RAT, Lft = INT
                        case ARRAY_APA:             // Res = RAT, Lft = APA
                            mpq_init_set_sx (&aplRatLft, aplIntegerLft, 1);

                            break;

                        case ARRAY_FLOAT:           // Res = RAT, Lft = FLOAT
                            mpq_init_set_d  (&aplRatLft, aplFloatLft);

                            break;

                        case ARRAY_RAT:             // Res = RAT, Lft = RAT
                            mpq_init_set    (&aplRatLft, (LPAPLRAT) lpSymGlbLft);

                            break;

                        case ARRAY_CHAR:            // Can't happen
                        case ARRAY_VFP:             // Can't happen
                        defstop
                            break;
                    } // End SWITCH

                    // Promote the right arg to Rational
                    // Split cases based upon the right arg type
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:            // Res = RAT, Rht = BOOL
                        case ARRAY_INT:             // Res = RAT, Rht = INT
                        case ARRAY_APA:             // Res = RAT, Rht = APA
                            mpq_init_set_sx (&aplRatRht, aplIntegerRht, 1);

                            break;

                        case ARRAY_FLOAT:           // Res = RAT, Rht = FLOAT
                            mpq_init_set_d  (&aplRatRht, aplFloatRht);

                            break;

                        case ARRAY_RAT:             // Res = RAT, Rht = RAT
                            mpq_init_set    (&aplRatRht, (LPAPLRAT) lpSymGlbRht);

                            break;

                        case ARRAY_CHAR:            // Can't happen
                        case ARRAY_VFP:             // Can't happen
                        defstop
                            break;
                    } // End SWITCH

                    // Save in the result
                    *((LPAPLRAT)   lpMemRes) =
                      (*lpPrimSpec->RisRvR) (aplRatLft,
                                             aplRatRht,
                                             lpPrimSpec);
                    // We no longer need this storage
                    Myq_clear (&aplRatRht);
                    Myq_clear (&aplRatLft);

                    break;

                case ARRAY_VFP:                     // Res = VFP
                    // Promote the left arg to VFP
                    // Split cases based upon the left arg type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:            // Res = VFP, Lft = BOOL
                        case ARRAY_INT:             // Res = VFP, Lft = INT
                        case ARRAY_APA:             // Res = VFP, Lft = APA
                            mpfr_init_set_sx (&aplVfpLft, aplIntegerLft, MPFR_RNDN);

                            break;

                        case ARRAY_FLOAT:           // Res = VFP, Lft = FLOAT
                            mpfr_init_set_d  (&aplVfpLft, aplFloatLft, MPFR_RNDN);

                            break;

                        case ARRAY_RAT:             // Res = VFP, Lft = RAT
                            mpfr_init_set_q  (&aplVfpLft, (LPAPLRAT) lpSymGlbLft, MPFR_RNDN);

                            break;

                        case ARRAY_VFP:             // Res = VFP, Lft = VFP
                            mpfr_init_copy   (&aplVfpLft, (LPAPLVFP) lpSymGlbLft);

                            break;

                        case ARRAY_CHAR:            // Can't happen
                        defstop
                            break;
                    } // End SWITCH

                    // Promote the right arg to VFP
                    // Split cases based upon the right arg type
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:            // Res = VFP, Rht = BOOL
                        case ARRAY_INT:             // Res = VFP, Rht = INT
                        case ARRAY_APA:             // Res = VFP, Rht = APA
                            mpfr_init_set_sx (&aplVfpRht, aplIntegerRht, MPFR_RNDN);

                            break;

                        case ARRAY_FLOAT:           // Res = VFP, Rht = FLOAT
                            mpfr_init_set_d  (&aplVfpRht, aplFloatRht, MPFR_RNDN);

                            break;

                        case ARRAY_RAT:             // Res = VFP, Rht = RAT
                            mpfr_init_set_q  (&aplVfpRht, (LPAPLRAT) lpSymGlbRht, MPFR_RNDN);

                            break;

                        case ARRAY_VFP:             // Res = VFP, Rht = VFP
                            mpfr_init_copy   (&aplVfpRht, (LPAPLVFP) lpSymGlbRht);

                            break;

                        case ARRAY_CHAR:            // Can't happen
                        defstop
                            break;
                    } // End SWITCH

                    // Save in the result
                    *((LPAPLVFP)   lpMemRes) =
                      (*lpPrimSpec->VisVvV) (aplVfpLft,
                                             aplVfpRht,
                                             lpPrimSpec);
                    // We no longer need this storage
                    Myf_clear (&aplVfpRht);
                    Myf_clear (&aplVfpLft);

                    break;

                defstop
                    break;
            } // End SWITCH
            } __except (CheckException (GetExceptionInformation (), L"PrimFnDydSimpSimp_EM #1"))
            {
                EXCEPTION_CODES ExceptionCode = MyGetExceptionCode ();  // The exception code

                dprintfWL9 (L"!!Initiating Exception in " APPEND_NAME L" #8: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                // Split cases based upon the ExceptionCode
                switch (ExceptionCode)
                {
                    case EXCEPTION_DOMAIN_ERROR:
                    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                    case EXCEPTION_INT_DIVIDE_BY_ZERO:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        goto DOMAIN_EXIT;

                    case EXCEPTION_NONCE_ERROR:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        goto NONCE_EXIT;

                    case EXCEPTION_RESULT_RAT:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        if (IsNumeric (aplTypeRes)
                         && !IsRat    (aplTypeRes))
                        {
                            // It's now a RAT result
                            aplTypeRes = ARRAY_RAT;

                            // We need to start over with the result
                            MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
                            FreeResultGlobalVar (*lphGlbRes); *lphGlbRes = NULL;

                            if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                                             lphGlbRes,
                                                             lpMemHdrLft,
                                                             lpMemHdrRht,
                                                             aplRankLft,
                                                             aplRankRht,
                                                            &aplRankRes,
                                                             aplTypeRes,
                                                             bLftIdent,
                                                             bRhtIdent,
                                                             aplNELMLft,
                                                             aplNELMRht,
                                                             aplNELMRes))
                                goto ERROR_EXIT;

                            // Lock the memory to get a ptr to it
                            lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);

                            // If the result is not nested, ...
                            if (!IsNested (lpMemHdrRes->ArrType))
                                // Tell the header about it
                                lpMemHdrRes->ArrType = aplTypeRes;

                            dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #8: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                            goto RESTART_EXCEPTION_SINGLETON;
                        } // End IF

                        // Display message for unhandled exception
                        DisplayException ();

                        break;

                    case EXCEPTION_RESULT_VFP:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        if (IsNumeric (aplTypeRes)
                         && !IsVfp    (aplTypeRes))
                        {
                            // It's now a VFP result
                            aplTypeRes = ARRAY_VFP;

                            // We need to start over with the result
                            MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
                            FreeResultGlobalVar (*lphGlbRes); *lphGlbRes = NULL;

                            if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                                             lphGlbRes,
                                                             lpMemHdrLft,
                                                             lpMemHdrRht,
                                                             aplRankLft,
                                                             aplRankRht,
                                                            &aplRankRes,
                                                             aplTypeRes,
                                                             bLftIdent,
                                                             bRhtIdent,
                                                             aplNELMLft,
                                                             aplNELMRht,
                                                             aplNELMRes))
                                goto ERROR_EXIT;

                            // Lock the memory to get a ptr to it
                            lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);

                            // If the result is not nested, ...
                            if (!IsNested (lpMemHdrRes->ArrType))
                                // Tell the header about it
                                lpMemHdrRes->ArrType = aplTypeRes;

                            dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #8: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                            goto RESTART_EXCEPTION_SINGLETON;
                        } // End IF

                        // Display message for unhandled exception
                        DisplayException ();

                        break;

                    case EXCEPTION_RESULT_FLOAT:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        if (IsNumeric (aplTypeRes)
                         && !IsSimpleFlt (aplTypeRes))
                        {
                            // If the previous result is Boolean, we need to
                            //   unlock, free, and allocate the result anew
                            if (IsSimpleBool (aplTypeRes))
                            {
                                // It's now a FLOAT result
                                aplTypeRes = ARRAY_FLOAT;

                                // We need to start over with the result
                                MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
                                FreeResultGlobalVar (*lphGlbRes); *lphGlbRes = NULL;

                                if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                                                 lphGlbRes,
                                                                 lpMemHdrLft,
                                                                 lpMemHdrRht,
                                                                 aplRankLft,
                                                                 aplRankRht,
                                                                &aplRankRes,
                                                                 aplTypeRes,
                                                                 bLftIdent,
                                                                 bRhtIdent,
                                                                 aplNELMLft,
                                                                 aplNELMRht,
                                                                 aplNELMRes))
                                    goto ERROR_EXIT;

                                // Lock the memory to get a ptr to it
                                lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);
                            } else
                            // The previous result must have been INT which is
                            //   the same size as FLOAT, so there's no need to
                            //   change storage.
                            {
                                // It's now a FLOAT result
                                aplTypeRes = ARRAY_FLOAT;

                                // Restart the pointers
                                lpMemRes = lpMemHdrRes;
                            } // End IF/ELSE

                            // If the result is not nested, ...
                            if (!IsNested (lpMemHdrRes->ArrType))
                                // Tell the header about it
                                lpMemHdrRes->ArrType = aplTypeRes;

                            dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #8: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                            goto RESTART_EXCEPTION_SINGLETON;
                        } // End IF

                        // Fall through to never-never-land

                    default:
                        // Display message for unhandled exception
                        DisplayException ();

                        break;
                } // End SWITCH
            } // End __try/__except

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (*lphGlbRes);
        } // End IF/ELSE

        // Finish with common code
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } else
    if (IsSingleton (aplNELMLft)
     || IsSingleton (aplNELMRht))
    {
        //***************************************************************
        // One of the args is a singleton, the other not
        //***************************************************************

        // Axis may be anything

        // Copy the ptr of the non-singleton argument
        if (!IsSingleton (aplNELMLft))
        {
            lpMemHdrArg = lpMemHdrLft;
            aplTypeArg  = aplTypeLft;
        } else
        {
            lpMemHdrArg = lpMemHdrRht;
            aplTypeArg  = aplTypeRht;
        } // End IF/ELSE

        // Skip over the header and dimensions to the data
        lpMemRes    = VarArrayDataFmBase (lpMemHdrRes);
        lpMemDimArg = VarArrayDataFmBase (lpMemHdrArg);

        // If the non-singleton arg is an APA, ...
        if (IsSimpleAPA (aplTypeArg))
        {
#define lpAPA       ((LPAPLAPA) lpMemDimArg)
            // Get the APA parameters
            apaOffLft = apaOffRht = lpAPA->Off;
            apaMulLft = apaMulRht = lpAPA->Mul;
#undef  lpAPA
        } // End IF

        // Get the value of the singleton
        if (IsSingleton (aplNELMLft))
            GetFirstValueToken (lptkLftArg,     // Ptr to left arg token
                               &aplIntegerLft,  // Ptr to integer result
                               &aplFloatLft,    // Ptr to float ...
                               &aplCharLft,     // Ptr to WCHAR ...
                                NULL,           // Ptr to longest ...
                               &lpSymGlbLft,    // Ptr to lpSym/Glb ...
                                NULL,           // Ptr to ...immediate type ...
                                NULL);          // Ptr to array type ...
        else
            GetFirstValueToken (lptkRhtArg,     // Ptr to right arg token
                               &aplIntegerRht,  // Ptr to integer result
                               &aplFloatRht,    // Ptr to float ...
                               &aplCharRht,     // Ptr to WCHAR ...
                                NULL,           // Ptr to longest ...
                               &lpSymGlbRht,    // Ptr to lpSym/Glb ...
                                NULL,           // Ptr to ...immediate type ...
                                NULL);          // Ptr to array type ...
        // Split cases based upon which argument is the singleton
        if (!IsSingleton (aplNELMLft))  // Lft = Multipleton, Rht = Singleton
            bRet = PrimFnDydMultSing_EM (lphGlbRes,
                                         aplTypeRes,
                                         lpMemHdrRes,
                                        &lpMemRes,
                                         aplNELMRes,
                                         aplTypeLft,
                                         apaOffLft,
                                         apaMulLft,
                                         lpMemHdrArg,
                                         lpMemDimArg,
                                         aplTypeRht,
                                         aplIntegerRht,
                                         aplFloatRht,
                                         aplCharRht,
                                         lpSymGlbRht,
                                         bLftIdent,
                                         bRhtIdent,
                                         lptkFunc,
                                         lpPrimSpec);
        else                            // Lft = Singleton, Rht = Multipleton
            bRet = PrimFnDydSingMult_EM (lphGlbRes,
                                         aplTypeRes,
                                         lpMemHdrRes,
                                        &lpMemRes,
                                         aplNELMRes,
                                         aplTypeLft,
                                         aplIntegerLft,
                                         aplFloatLft,
                                         aplCharLft,
                                         lpSymGlbLft,
                                         aplTypeRht,
                                         apaOffRht,
                                         apaMulRht,
                                         lpMemHdrArg,
                                         lpMemDimArg,
                                         bLftIdent,
                                         bRhtIdent,
                                         lptkFunc,
                                         lpPrimSpec);
        // Check for error
        if (!bRet)
            goto ERROR_EXIT;

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (*lphGlbRes);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } else
    {
        //***************************************************************
        // Neither arg is a singleton, but both are simple
        //***************************************************************

        // Axis is significant (if present)

        // Skip over the header to the dimensions
        lpMemDimRes = VarArrayBaseToDim (lpMemHdrRes);

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);
        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

        // If the left arg is an APA, ...
        if (IsSimpleAPA (aplTypeLft))
        {
#define lpAPA       ((LPAPLAPA) lpMemLft)
            // Get the APA parameters
            apaOffLft = lpAPA->Off;
            apaMulLft = lpAPA->Mul;
#undef  lpAPA
        } // End IF

        // If the right arg is an APA, ...
        if (IsSimpleAPA (aplTypeRht))
        {
#define lpAPA       ((LPAPLAPA) lpMemRht)
            // Get the APA parameters
            apaOffRht = lpAPA->Off;
            apaMulRht = lpAPA->Mul;
#undef  lpAPA
        } // End IF

        // If the axis is significant, ...
        if (lpMemAxisHead NE NULL)
        {
            // Calculate space needed for the weighting vector
            ByteRes = aplRankRes * sizeof (APLUINT);

            // Check for overflow
            if (ByteRes NE (APLU3264) ByteRes)
                goto WSFULL_EXIT;

            //***************************************************************
            // Allocate space for the weighting vector which is
            //
            //   {times}{backscan}1{drop}({rho}R)[Cx,Ax],1
            //
            //   where Ax contains the specified axes, and
            //   Cx contains the remaining axes.
            //***************************************************************
            hGlbWVec = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
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
            for (uRes = 1, iRht = aplRankRes - 1; iRht >= 0; iRht--)
            {
                lpMemWVec[iRht] = uRes;
                uRes *= lpMemDimRes[lpMemAxisHead[iRht]];
            } // End FOR

            // Calculate space needed for the odometer array
            ByteRes = aplRankRes * sizeof (APLUINT);

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
RESTART_EXCEPTION_AXIS:
            // Skip over the header and dimensions to the data
            lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

            // Skip over the header to the dimensions
            lpMemDimRes = VarArrayBaseToDim (lpMemHdrRes);

            __try
            {
            // Split cases based upon the result's storage type
            switch (aplTypeRes)
            {
                case ARRAY_BOOL:                    // Res = BOOL(Axis)
                    // If both arguments are BOOL,
                    //   use BisBvB
                    if (IsSimpleBool (aplTypeLft)
                     && IsSimpleBool (aplTypeRht))  // Res = BOOL(Axis), Lft = BOOL, Rht = BOOL
                    {
                        // ***FIXME*** -- Optimize by chunking

                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Calculate the left and right argument indices
                            CalcLftRhtArgIndices (uRes,
                                                  aplRankRes,
                                                 &uLft,
                                                  aplRankLft,
                                                 &uRht,
                                                  aplRankRht,
                                                  bLftIdent,
                                                  bRhtIdent,
                                                  aplNELMAxis,
                                                  lpMemAxisHead,
                                                  lpMemOdo,
                                                  lpMemWVec,
                                                  lpMemDimRes);
                            // Save in the result
                            *((LPAPLBOOL)  lpMemRes) |=
                              (*lpPrimSpec->BisBvB) ((APLBOOL) GetNextInteger (lpMemLft, aplTypeLft, uLft),
                                                     (APLBOOL) GetNextInteger (lpMemRht, aplTypeRht, uRht),
                                                     lpPrimSpec) << uBitIndex;
                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;                  // Start over
                                ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                            } // End IF
                        } // End FOR
                    } else
                    // If both arguments are integer-like (BOOL, INT, or APA),
                    //   use BisIvI
                    if (IsSimpleInt (aplTypeLft)
                     && IsSimpleInt (aplTypeRht))   // Res = BOOL(Axis), Lft = BOOL/INT/APA, Rht = BOOL/INT/APA
                    {
                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Calculate the left and right argument indices
                            CalcLftRhtArgIndices (uRes,
                                                  aplRankRes,
                                                 &uLft,
                                                  aplRankLft,
                                                 &uRht,
                                                  aplRankRht,
                                                  bLftIdent,
                                                  bRhtIdent,
                                                  aplNELMAxis,
                                                  lpMemAxisHead,
                                                  lpMemOdo,
                                                  lpMemWVec,
                                                  lpMemDimRes);
                            // Save in the result
                            *((LPAPLBOOL)  lpMemRes) |=
                              (*lpPrimSpec->BisIvI) (GetNextInteger (lpMemLft, aplTypeLft, uLft),
                                                     GetNextInteger (lpMemRht, aplTypeRht, uRht),
                                                     lpPrimSpec) << uBitIndex;
                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;                  // Start over
                                ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                            } // End IF
                        } // End FOR
                    } else
                    // If both arguments are CHAR,
                    //   use BisCvC
                    if (IsSimpleChar (aplTypeLft)
                     && IsSimpleChar (aplTypeRht))  // Res = BOOL(Axis), Lft = CHAR, Rht = CHAR
                    {
                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Calculate the left and right argument indices
                            CalcLftRhtArgIndices (uRes,
                                                  aplRankRes,
                                                 &uLft,
                                                  aplRankLft,
                                                 &uRht,
                                                  aplRankRht,
                                                  bLftIdent,
                                                  bRhtIdent,
                                                  aplNELMAxis,
                                                  lpMemAxisHead,
                                                  lpMemOdo,
                                                  lpMemWVec,
                                                  lpMemDimRes);
                            // Save in the result
                            *((LPAPLBOOL)  lpMemRes) |=
                              (*lpPrimSpec->BisCvC) (((LPAPLCHAR) lpMemLft)[uLft],
                                                     ((LPAPLCHAR) lpMemRht)[uRht],
                                                     lpPrimSpec) << uBitIndex;
                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;                  // Start over
                                ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                            } // End IF
                        } // End FOR
                    } else
                    // If either argument is FLOAT and the other is simple numeric (BOOL, INT, APA, or FLOAT),
                    //   use BisFvF
                    if ((IsSimpleFlt (aplTypeLft) && IsSimpleNum (aplTypeRht))   // Res = BOOL(Axis), Lft = FLOAT, Rht = BOOL/INT/APA/FLOAT
                     || (IsSimpleFlt (aplTypeRht) && IsSimpleNum (aplTypeLft)))  // Res = BOOL(Axis), Lft = BOOL/INT/APA/FLOAT, Rht = FLOAT
                    {
                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Calculate the left and right argument indices
                            CalcLftRhtArgIndices (uRes,
                                                  aplRankRes,
                                                 &uLft,
                                                  aplRankLft,
                                                 &uRht,
                                                  aplRankRht,
                                                  bLftIdent,
                                                  bRhtIdent,
                                                  aplNELMAxis,
                                                  lpMemAxisHead,
                                                  lpMemOdo,
                                                  lpMemWVec,
                                                  lpMemDimRes);
                            // Save in the result
                            *((LPAPLBOOL)  lpMemRes) |=
                              (*lpPrimSpec->BisFvF) (GetNextFloat (lpMemLft, aplTypeLft, uLft),
                                                     GetNextFloat (lpMemRht, aplTypeRht, uRht),
                                                     lpPrimSpec) << uBitIndex;
                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;                  // Start over
                                ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                            } // End IF
                        } // End FOR
                    } else
                    // If either arg is hetero and the other is simple, ...
                    if ((IsSimpleHet (aplTypeLft) && IsSimpleGlbNum (aplTypeRht))   // Res = BOOL(Axis), Lft = HETERO, Rht = BOOL/INT/APA/FLOAT/CHAR/HETERO/RAT/VFP
                     || (IsSimpleHet (aplTypeRht) && IsSimpleGlbNum (aplTypeLft)))  // Res = BOOL(Axis), Lft = BOOL/INT/APA/FLOAT/CHAR/HETERO/RAT/VFP, Rht = HETERO
                    {
                        // Initialize the temps
                        mpq_init (&aplRatLft);
                        mpq_init (&aplRatRht);
                        mpfr_init0 (&aplVfpLft);
                        mpfr_init0 (&aplVfpRht);

                        // Copy the storage types
                        aplTypeHetLft = aplTypeLft;
                        aplTypeHetRht = aplTypeRht;

                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Calculate the left and right argument indices
                            CalcLftRhtArgIndices (uRes,
                                                  aplRankRes,
                                                 &uLft,
                                                  aplRankLft,
                                                 &uRht,
                                                  aplRankRht,
                                                  bLftIdent,
                                                  bRhtIdent,
                                                  aplNELMAxis,
                                                  lpMemAxisHead,
                                                  lpMemOdo,
                                                  lpMemWVec,
                                                  lpMemDimRes);
                            // Get the next values and type
                            if (IsSimpleHet (aplTypeLft))
                                aplTypeHetLft = GetNextHetero (lpMemLft, uLft, &aplIntegerLft, &aplFloatLft, &aplCharLft, &lpSymGlbLft);
                            else
                                GetNextSimple (lpMemLft,        // Ptr to item global memory data
                                               aplTypeLft,      // Item storage type
                                               aplNELMLft,      // Item NELM
                                               uLft,            // Index into item
                                              &aplIntegerLft,   // Ptr to Boolean/Integer result
                                              &aplFloatLft,     // Ptr to Float result
                                              &aplCharLft,      // Ptr to Char result
                                              &lpSymGlbLft);    // Ptr to global memory handle (may be NULL)
                            if (IsSimpleHet (aplTypeRht))
                                aplTypeHetRht = GetNextHetero (lpMemRht, uRht, &aplIntegerRht, &aplFloatRht, &aplCharRht, &lpSymGlbRht);
                            else
                                GetNextSimple (lpMemRht,        // Ptr to item global memory data
                                               aplTypeRht,      // Item storage type
                                               aplNELMRht,      // Item NELM
                                               uRht,            // Index into item
                                              &aplIntegerRht,   // Ptr to Boolean/Integer result
                                              &aplFloatRht,     // Ptr to Float result
                                              &aplCharRht,      // Ptr to Char result
                                              &lpSymGlbRht);    // Ptr to global memory handle (may be NULL)
                            // Split cases based upon the left hetero's storage type
                            switch (aplTypeHetLft)
                            {
                                case ARRAY_BOOL:            // Res = BOOL(Axis), Lft = BOOL,  Rht = BOOL/INT/FLOAT/CHAR/RAT/VFP (one hetero, one simple)
                                    // Split cases based upon the right hetero's storage type
                                    switch (aplTypeHetRht)
                                    {
                                        case ARRAY_BOOL:    // Res = BOOL(Axis), Lft = BOOL, Rht = BOOL  (one hetero, one simple)
                                            // Save in the result
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisBvB) ((APLBOOL) aplIntegerLft,
                                                                     (APLBOOL) aplIntegerRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_INT:     // Res = BOOL(Axis), Lft = BOOL, Rht = INT   (one hetero, one simple)
                                            // Save in the result
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisIvI) ((APLBOOL) aplIntegerLft,
                                                                               aplIntegerRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_FLOAT:   // Res = BOOL(Axis), Lft = BOOL, Rht = FLOAT (one hetero, one simple)
                                            // Save in the result
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisFvF) ((APLFLOAT) aplIntegerLft,
                                                                                aplFloatRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_CHAR:    // Res = BOOL(Axis), Lft = BOOL, Rht = CHAR   (one hetero, one simple)
                                            // One arg is numeric, the other char
                                            Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                                                 || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);
                                            // If the function is UTF16_NOTEQUAL, the result is one
                                            // Save in the result
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL) << uBitIndex;

                                            break;

                                        case ARRAY_RAT:     // Res = BOOL(Axis), Lft = BOOL, Rht = RAT    (one hetero, one simple)
                                            // Convert the Boolean to a RAT
                                            mpq_set_sx (&aplRatLft, aplIntegerLft, 1);

                                            // Save in the result
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisRvR) (aplRatLft,
                                                                    *(LPAPLRAT) lpSymGlbRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_VFP:     // Res = BOOL(Axis), Lft = BOOL, Rht = VFP    (one hetero, one simple)
                                            // Convert the Boolean to a VFP
                                            mpfr_set_sx (&aplVfpLft, aplIntegerLft, MPFR_RNDN);

                                            // Save in the result
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                                    *(LPAPLVFP) lpSymGlbRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        defstop
                                            break;
                                    } // End SWITCH

                                    break;

                                case ARRAY_INT:             // Res = BOOL(Axis), Lft = INT,   Rht = BOOL/INT/FLOAT/CHAR/RAT/VFP (one hetero, one simple)
                                    // Split cases based upon the right hetero's storage type
                                    switch (aplTypeHetRht)
                                    {
                                        case ARRAY_BOOL:    // Res = BOOL(Axis), Lft = INT,   Rht = BOOL  (one hetero, one simple)
                                        case ARRAY_INT:     // Res = BOOL(Axis), Lft = INT,   Rht = INT   (one hetero, one simple)
                                            // Save in the result
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisIvI) (aplIntegerLft,
                                                                     aplIntegerRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_FLOAT:   // Res = BOOL(Axis), Lft = INT,   Rht = FLOAT (one hetero, one simple)
                                            // Save in the result
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                                     aplFloatRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_CHAR:    // Res = BOOL(Axis), Lft = INT,   Rht = CHAR  (one hetero, one simple)
                                            // One arg is numeric, the other char
                                            Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                                                 || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);
                                            // If the function is UTF16_NOTEQUAL, the result is one
                                            // Save in the result
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL) << uBitIndex;

                                            break;

                                        case ARRAY_RAT:     // Res = BOOL(Axis), Lft = INT,   Rht = RAT   (one hetero, one simple)
                                            // Convert the integer to a RAT
                                            mpq_set_sx (&aplRatLft, aplIntegerLft, 1);

                                            // Save in the result
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisRvR) (aplRatLft,
                                                                    *(LPAPLRAT) lpSymGlbRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_VFP:     // Res = BOOL(Axis), Lft = INT,   Rht = VFP   (one hetero, one simple)
                                            // Convert the integer to a VFP
                                            mpfr_set_sx (&aplVfpLft, aplIntegerLft, MPFR_RNDN);

                                            // Save in the result
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                                    *(LPAPLVFP) lpSymGlbRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        defstop
                                            break;
                                    } // End SWITCH

                                    break;

                                case ARRAY_FLOAT:           // Res = BOOL(Axis), Lft = FLOAT, Rht = BOOL/INT/FLOAT/CHAR/RAT/VFP (one hetero, one simple)
                                    // Split cases based upon the right hetero's storage type
                                    switch (aplTypeHetRht)
                                    {
                                        case ARRAY_BOOL:    // Res = BOOL(Axis), Lft = FLOAT, Rht = BOOL (one hetero, one simple)
                                        case ARRAY_INT:     // Res = BOOL(Axis), Lft = FLOAT, Rht = INT  (one hetero, one simple)
                                        case ARRAY_FLOAT:   // Res = BOOL(Axis), Lft = FLOAT, Rht = FLOAT(one hetero, one simple)
                                            // Save in the result
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                                     aplFloatRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_CHAR:    // Res = BOOL(Axis), Lft = FLOAT, Rht = CHAR (one hetero, one simple)
                                            // One arg is numeric, the other char
                                            Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                                                 || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);
                                            // If the function is UTF16_NOTEQUAL, the result is one
                                            // Save in the result
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL) << uBitIndex;

                                            break;

                                        case ARRAY_RAT:     // Res = BOOL(Axis), Lft = FLOAT, Rht = RAT  (one hetero, one simple)
                                            // Convert the FLOAT and RAT to a VFP
                                            mpfr_set_d (&aplVfpLft, aplFloatLft, MPFR_RNDN);
                                            mpfr_set_q (&aplVfpRht, (LPAPLRAT) lpSymGlbRht, MPFR_RNDN);

                                            // Save in the result
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                                     aplVfpRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_VFP:     // Res = BOOL(Axis), Lft = FLOAT, Rht = VFP  (one hetero, one simple)
                                            // Convert the FLOAT to a VFP
                                            mpfr_set_d (&aplVfpLft, aplFloatLft, MPFR_RNDN);

                                            // Save in the result
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                                    *(LPAPLVFP) lpSymGlbRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        defstop
                                            break;
                                    } // End SWITCH

                                    break;

                                case ARRAY_CHAR:            // Res = BOOL(Axis), Lft = CHAR,  Rht = BOOL/INT/FLOAT/CHAR/RAT/VFP (one hetero, one simple)
                                    // Split cases based upon the right hetero's storage type
                                    switch (aplTypeHetRht)
                                    {
                                        case ARRAY_BOOL:    // Res = BOOL(Axis), Lft = CHAR,  Rht = BOOL  (one hetero, one simple)
                                        case ARRAY_INT:     // Res = BOOL(Axis), Lft = CHAR,  Rht = INT   (one hetero, one simple)
                                        case ARRAY_FLOAT:   // Res = BOOL(Axis), Lft = CHAR,  Rht = FLOAT (one hetero, one simple)
                                        case ARRAY_RAT:     // Res = BOOL(Axis), Lft = CHAR,  Rht = RAT   (one hetero, one simple)
                                        case ARRAY_VFP:     // Res = BOOL(Axis), Lft = CHAR,  Rht = VFP   (one hetero, one simple)
                                            // One arg is numeric, the other char
                                            Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                                                 || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);
                                            // If the function is UTF16_NOTEQUAL, the result is one
                                            // Save in the result
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL) << uBitIndex;

                                            break;

                                        case ARRAY_CHAR:    // Res = BOOL(Axis), Lft = CHAR,  Rht = CHAR  (one hetero, one simple)
                                            // Save in the result
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisCvC) (aplCharLft,
                                                                     aplCharRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        defstop
                                            break;
                                    } // End SWITCH

                                    break;

                                case ARRAY_RAT:             // Res = BOOL(Axis), Lft = RAT ,  Rht = BOOL/INT/FLOAT/CHAR/RAT/VFP (one hetero, one simple)
                                    // Split cases based upon the right hetero's storage type
                                    switch (aplTypeHetRht)
                                    {
                                        case ARRAY_BOOL:    // Res = BOOL(Axis), Lft = RAT, Rht = BOOL (one hetero, one simple)
                                        case ARRAY_INT:     // Res = BOOL(Axis), Lft = RAT, Rht = INT  (one hetero, one simple)
                                            // Convert the INT to a RAT
                                            mpq_set_sx (&aplRatRht, aplIntegerRht, 1);

                                            // Save in the result
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisRvR) (*(LPAPLRAT) lpSymGlbLft,
                                                                     aplRatRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_FLOAT:   // Res = BOOL(Axis), Lft = RAT, Rht = FLOAT(one hetero, one simple)
                                            // Convert the RAT and FLOAT to a VFP
                                            mpfr_set_q (&aplVfpLft, (LPAPLRAT) lpSymGlbLft, MPFR_RNDN);
                                            mpfr_set_d (&aplVfpRht, aplFloatRht, MPFR_RNDN);

                                            // Save in the result
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                                     aplVfpRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_CHAR:    // Res = BOOL(Axis), Lft = RAT, Rht = CHAR (one hetero, one simple)
                                            // One arg is numeric, the other char
                                            Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                                                 || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);
                                            // If the function is UTF16_NOTEQUAL, the result is one
                                            // Save in the result
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL) << uBitIndex;

                                            break;

                                        case ARRAY_RAT:     // Res = BOOL(Axis), Lft = RAT, Rht = RAT  (one hetero, one simple)
                                            // Save in the result
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisRvR) (*(LPAPLRAT) lpSymGlbLft,
                                                                     *(LPAPLRAT) lpSymGlbRht,
                                                                      lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_VFP:     // Res = BOOL(Axis), Lft = RAT, Rht = VFP  (one hetero, one simple)
                                            // Convert the RAT to a VFP
                                            mpfr_set_q (&aplVfpLft, (LPAPLRAT) lpSymGlbLft, MPFR_RNDN);

                                            // Save in the result
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                                    *(LPAPLVFP) lpSymGlbRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        defstop
                                            break;
                                    } // End SWITCH

                                    break;

                                case ARRAY_VFP:             // Res = BOOL(Axis), Lft = VFP ,  Rht = BOOL/INT/FLOAT/CHAR/RAT/VFP (one hetero, one simple)
                                    // Split cases based upon the right hetero's storage type
                                    switch (aplTypeHetRht)
                                    {
                                        case ARRAY_BOOL:    // Res = BOOL(Axis), Lft = VFP, Rht = BOOL (one hetero, one simple)
                                        case ARRAY_INT:     // Res = BOOL(Axis), Lft = VFP, Rht = INT  (one hetero, one simple)
                                            // Convert the INT to a VFP
                                            mpfr_set_sx (&aplVfpRht, aplIntegerRht, MPFR_RNDN);

                                            // Save in the result
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisVvV) (*(LPAPLVFP) lpSymGlbLft,
                                                                     aplVfpRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_FLOAT:   // Res = BOOL(Axis), Lft = VFP, Rht = FLOAT(one hetero, one simple)
                                            // Convert the FLOAT to a VFP
                                            mpfr_set_d (&aplVfpRht, aplFloatRht, MPFR_RNDN);

                                            // Save in the result
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisVvV) (*(LPAPLVFP) lpSymGlbLft,
                                                                     aplVfpRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_CHAR:    // Res = BOOL(Axis), Lft = VFP, Rht = CHAR (one hetero, one simple)
                                            // One arg is numeric, the other char
                                            Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                                                 || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);
                                            // If the function is UTF16_NOTEQUAL, the result is one
                                            // Save in the result
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL) << uBitIndex;

                                            break;

                                        case ARRAY_RAT:     // Res = BOOL(Axis), Lft = VFP, Rht = RAT  (one hetero, one simple)
                                            // Convert the RAT to a VFP
                                            mpfr_set_q (&aplVfpRht, (LPAPLRAT) lpSymGlbRht, MPFR_RNDN);

                                            // Save in the result
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisVvV) (*(LPAPLVFP) lpSymGlbLft,
                                                                      aplVfpRht,
                                                                      lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_VFP:     // Res = BOOL(Axis), Lft = VFP, Rht = VFP  (one hetero, one simple)
                                            // Save in the result
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisVvV) (*(LPAPLVFP) lpSymGlbLft,
                                                                     *(LPAPLVFP) lpSymGlbRht,
                                                                      lpPrimSpec) << uBitIndex;
                                            break;

                                        defstop
                                            break;
                                    } // End SWITCH

                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;                  // Start over
                                ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                            } // End IF
                        } // End FOR

                        // We no longer need this storage
                        Myf_clear (&aplVfpRht);
                        Myf_clear (&aplVfpLft);
                        Myq_clear (&aplRatRht);
                        Myq_clear (&aplRatLft);
                    } else
                    // If one arg is numeric and the other char, ...
                    if ((IsNumeric (aplTypeLft) && IsSimpleChar (aplTypeRht))     // Res = BOOL(Axis), Lft = BOOL/INT/APA/FLOAT, Rht = CHAR
                     || (IsNumeric (aplTypeRht) && IsSimpleChar (aplTypeLft)))    // Res = BOOL(Axis), Lft = CHAR              , Rht = BOOL/INT/APA/FLOAT
                    {
                        // One arg is numeric, the other char
                        Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                             || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);

                        // If the function is not-equal, the result is all 1s
                        if (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL)
                            // Fill the result with all 1s
                            FillMemory (lpMemRes, (APLU3264) RoundUpBitsToBytes (aplNELMRes), 0xFF);
                    } else
                    // If either argument is global numeric, ...
                    if (IsGlbNum (aplTypeLft)
                     || IsGlbNum (aplTypeRht))
                    {
                        // Calculate the common storage type
                        aplTypeCom = aTypePromote[aplTypeLft][aplTypeRht];

                        // Initialize the temps
                        mpq_init (&aplRatLft);
                        mpq_init (&aplRatRht);
                        mpfr_init0 (&aplVfpLft);
                        mpfr_init0 (&aplVfpRht);

                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Calculate the left and right argument indices
                            CalcLftRhtArgIndices (uRes,
                                                  aplRankRes,
                                                 &uLft,
                                                  aplRankLft,
                                                 &uRht,
                                                  aplRankRht,
                                                  bLftIdent,
                                                  bRhtIdent,
                                                  aplNELMAxis,
                                                  lpMemAxisHead,
                                                  lpMemOdo,
                                                  lpMemWVec,
                                                  lpMemDimRes);
                            // Promote the left arg to the common type
                            switch (aplTypeCom)
                            {
                                case ARRAY_RAT:                 // Res = BOOL(Axis)
                                    // Split cases based upon the left arg storage type
                                    switch (aplTypeLft)
                                    {
                                        case ARRAY_BOOL:        // Res = BOOL(Axis), Lft = BOOL
                                        case ARRAY_INT:         // Res = BOOL(Axis), Lft = INT
                                        case ARRAY_APA:         // Res = BOOL(Axis), Lft = APA
                                            // Convert the BOOL/INT/APA to a RAT
                                            mpq_set_sx (&aplRatLft, GetNextInteger (lpMemLft, aplTypeLft, uLft), 1);

                                            break;

                                        case ARRAY_FLOAT:       // Res = BOOL(Axis), Lft = FLOAT
                                            // Convert the FLOAT to a RAT
                                            mpq_set_d  (&aplRatLft, GetNextFloat   (lpMemLft, aplTypeLft, uLft));

                                            break;

                                        case ARRAY_RAT:         // Res = BOOL(Axis), Lft = RAT
                                            // Copy the RAT to a RAT
                                            mpq_set (&aplRatLft, &((LPAPLRAT) lpMemLft)[uLft]);

                                            break;

                                        case ARRAY_CHAR:        // Can't happen
                                        case ARRAY_VFP:         // Can't happen
                                        defstop
                                            break;
                                    } // End SWITCH

                                    break;

                                case ARRAY_VFP:                 // Res = BOOL(Axis)
                                    // Split cases based upon the left arg storage type
                                    switch (aplTypeLft)
                                    {
                                        case ARRAY_BOOL:        // Res = BOOL(Axis), Lft = BOOL
                                        case ARRAY_INT:         // Res = BOOL(Axis), Lft = INT
                                        case ARRAY_APA:         // Res = BOOL(Axis), Lft = APA
                                            // Convert the BOOL/INT/APA to a VFP
                                            mpfr_set_sx (&aplVfpLft, GetNextInteger (lpMemLft, aplTypeLft, uLft), MPFR_RNDN);

                                            break;

                                        case ARRAY_FLOAT:       // Res = BOOL(Axis), Lft = FLOAT
                                            // Convert the FLOAT to a VFP
                                            mpfr_set_d (&aplVfpLft, ((LPAPLFLOAT) lpMemLft)[uLft], MPFR_RNDN);

                                            break;

                                        case ARRAY_RAT:         // Res = BOOL(Axis), Lft = RAT
                                            // Convert the RAT to a VFP
                                            mpfr_set_q (&aplVfpLft, &((LPAPLRAT) lpMemLft)[uLft], MPFR_RNDN);

                                            break;

                                        case ARRAY_VFP:         // Res = BOOL(Axis), Lft = VFP
                                            // Copy the VFP to a VFP
                                            mpfr_copy (&aplVfpLft, &((LPAPLVFP) lpMemLft)[uLft]);

                                            break;

                                        case ARRAY_CHAR:        // Can't happen
                                        defstop
                                            break;
                                    } // End SWITCH

                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            // Promote the right arg to the common type
                            switch (aplTypeCom)
                            {
                                case ARRAY_RAT:                 // Res = BOOL(Axis)
                                    // Split cases based upon the right arg storage type
                                    switch (aplTypeRht)
                                    {
                                        case ARRAY_BOOL:        // Res = BOOL(Axis), Rht = BOOL
                                        case ARRAY_INT:         // Res = BOOL(Axis), Rht = INT
                                        case ARRAY_APA:         // Res = BOOL(Axis), Rht = APA
                                            // Convert the BOOL/INT/APA to a RAT
                                            mpq_set_sx (&aplRatRht, GetNextInteger (lpMemRht, aplTypeRht, uRht), 1);

                                            break;

                                        case ARRAY_FLOAT:       // Res = BOOL(Axis), Rht = FLOAT
                                            // Convert the FLOAT to a RAT
                                            mpq_set_d  (&aplRatRht, GetNextFloat   (lpMemRht, aplTypeRht, uRht));

                                            break;

                                        case ARRAY_RAT:         // Res = BOOL(Axis), Rht = RAT
                                            // Copy the RAT to a RAT
                                            mpq_set (&aplRatRht, &((LPAPLRAT) lpMemRht)[uRht]);

                                            break;

                                        case ARRAY_CHAR:        // Can't happen
                                        case ARRAY_VFP:         // Can't happen
                                        defstop
                                            break;
                                    } // End SWITCH

                                    break;

                                case ARRAY_VFP:                 // Res = BOOL(Axis)
                                    // Split cases based upon the right arg storage type
                                    switch (aplTypeRht)
                                    {
                                        case ARRAY_BOOL:        // Res = BOOL(Axis), Rht = BOOL
                                        case ARRAY_INT:         // Res = BOOL(Axis), Rht = INT
                                        case ARRAY_APA:         // Res = BOOL(Axis), Rht = APA
                                            // Convert the BOOL/INT/APA to a VFP
                                            mpfr_set_sx (&aplVfpRht, GetNextInteger (lpMemRht, aplTypeRht, uRht), MPFR_RNDN);

                                            break;

                                        case ARRAY_FLOAT:       // Res = BOOL(Axis), Rht = FLOAT
                                            // Convert the FLOAT to a VFP
                                            mpfr_set_d (&aplVfpRht, ((LPAPLFLOAT) lpMemRht)[uRht], MPFR_RNDN);

                                            break;

                                        case ARRAY_RAT:         // Res = BOOL(Axis), Rht = RAT
                                            // Convert the RAT to a VFP
                                            mpfr_set_q (&aplVfpRht, &((LPAPLRAT) lpMemRht)[uRht], MPFR_RNDN);

                                            break;

                                        case ARRAY_VFP:         // Res = BOOL(Axis), Rht = VFP
                                            // Copy the VFP to a VFP
                                            mpfr_copy (&aplVfpRht, &((LPAPLVFP) lpMemRht)[uRht]);

                                            break;

                                        case ARRAY_CHAR:        // Can't happen
                                        defstop
                                            break;
                                    } // End SWITCH

                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            // Split cases based upon the common type
                            switch (aplTypeCom)
                            {
                                case ARRAY_RAT:                 // Res = BOOL(Axis), Lft = RAT, Rht = RAT
                                    *((LPAPLBOOL)  lpMemRes) |=
                                      (*lpPrimSpec->BisRvR) (aplRatLft,
                                                             aplRatRht,
                                                             lpPrimSpec) << uBitIndex;
                                    break;

                                case ARRAY_VFP:                 // Res = BOOL(Axis), Lft = VFP, Rht = VFP
                                    *((LPAPLBOOL)  lpMemRes) |=
                                      (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                             aplVfpRht,
                                                             lpPrimSpec) << uBitIndex;
                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;                  // Start over
                                ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                            } // End IF
                        } // End FOR

                        // We no longer need this storage
                        Myf_clear (&aplVfpRht);
                        Myf_clear (&aplVfpLft);
                        Myq_clear (&aplRatRht);
                        Myq_clear (&aplRatLft);
                    } else
                        DbgStop ();     // We should never get here
                    break;

                case ARRAY_INT:                     // Res = INT(Axis)
                    // If both left and right arguments are integer-like (BOOL, INT, or APA),
                    //   use IisIvI
                    if (IsSimpleInt (aplTypeLft)
                     && IsSimpleInt (aplTypeRht))   // Res = INT(Axis), Lft = BOOL/INT/APA, Rht = BOOL/INT/APA
                    {
                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Calculate the left and right argument indices
                            CalcLftRhtArgIndices (uRes,
                                                  aplRankRes,
                                                 &uLft,
                                                  aplRankLft,
                                                 &uRht,
                                                  aplRankRht,
                                                  bLftIdent,
                                                  bRhtIdent,
                                                  aplNELMAxis,
                                                  lpMemAxisHead,
                                                  lpMemOdo,
                                                  lpMemWVec,
                                                  lpMemDimRes);
                            // Save in the result
                            *((LPAPLINT)   lpMemRes)++ =
                              (*lpPrimSpec->IisIvI) (GetNextInteger (lpMemLft, aplTypeLft, uLft),
                                                     GetNextInteger (lpMemRht, aplTypeRht, uRht),
                                                     lpPrimSpec);
                        } // End FOR
                    } else
                    // If either argument is FLOAT and the other is simple numeric (BOOL, INT, APA, or FLOAT),
                    //   use IisFvF
                    if ((IsSimpleFlt (aplTypeLft) && IsSimpleNum (aplTypeRht))   // Res = INT(Axis), Lft = FLOAT, Rht = BOOL/INT/APA/FLOAT
                     || (IsSimpleFlt (aplTypeRht) && IsSimpleNum (aplTypeLft)))  // Res = INT(Axis), Lft = BOOL/INT/APA/FLOAT, Rht = FLOAT
                    {
                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Calculate the left and right argument indices
                            CalcLftRhtArgIndices (uRes,
                                                  aplRankRes,
                                                 &uLft,
                                                  aplRankLft,
                                                 &uRht,
                                                  aplRankRht,
                                                  bLftIdent,
                                                  bRhtIdent,
                                                  aplNELMAxis,
                                                  lpMemAxisHead,
                                                  lpMemOdo,
                                                  lpMemWVec,
                                                  lpMemDimRes);
                            // Save in the result
                            *((LPAPLINT)   lpMemRes)++ =
                              (*lpPrimSpec->IisFvF) (GetNextFloat (lpMemLft, aplTypeLft, uLft),
                                                     GetNextFloat (lpMemRht, aplTypeRht, uRht),
                                                     lpPrimSpec);
                        } // End FOR
                    } else
                    // If both arguments are numeric
                    if (IsNumeric (aplTypeLft)
                     && IsNumeric (aplTypeRht))                                  // Res = INT(Axis), Lft = RAT/VFP/..., Rht = RAT/VFP/...
                    {
                        DbgBrk ();              // Can't happen with any known primitive




                    } else
                        DbgStop ();     // We should never get here
                    break;

                case ARRAY_FLOAT:                   // Res = FLOAT(Axis)
                    // If both arguments are simple integer (BOOL, INT, APA),
                    //   use FisIvI
                    if (IsSimpleInt (aplTypeLft)
                     && IsSimpleInt (aplTypeRht))   // Res = FLOAT(Axis), Lft = BOOL/INT/APA, Rht = BOOL/INT/APA
                    {
                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Calculate the left and right argument indices
                            CalcLftRhtArgIndices (uRes,
                                                  aplRankRes,
                                                 &uLft,
                                                  aplRankLft,
                                                 &uRht,
                                                  aplRankRht,
                                                  bLftIdent,
                                                  bRhtIdent,
                                                  aplNELMAxis,
                                                  lpMemAxisHead,
                                                  lpMemOdo,
                                                  lpMemWVec,
                                                  lpMemDimRes);
                            // Save in the result
                            *((LPAPLFLOAT) lpMemRes)++ =
                              (*lpPrimSpec->FisIvI) (GetNextInteger (lpMemLft, aplTypeLft, uLft),
                                                     GetNextInteger (lpMemRht, aplTypeRht, uRht),
                                                     lpPrimSpec);
                        } // End FOR
                    } else
                    // If both arguments are simple numeric (BOOL, INT, APA, FLOAT),
                    //   use FisFvF
                    if (IsSimpleNum (aplTypeLft)
                     && IsSimpleNum (aplTypeRht))   // Res = FLOAT(Axis), Lft = BOOL/INT/APA/FLOAT, Rht = BOOL/INT/APA/FLOAT
                    {
                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Calculate the left and right argument indices
                            CalcLftRhtArgIndices (uRes,
                                                  aplRankRes,
                                                 &uLft,
                                                  aplRankLft,
                                                 &uRht,
                                                  aplRankRht,
                                                  bLftIdent,
                                                  bRhtIdent,
                                                  aplNELMAxis,
                                                  lpMemAxisHead,
                                                  lpMemOdo,
                                                  lpMemWVec,
                                                  lpMemDimRes);
                            // Save in the result
                            *((LPAPLFLOAT) lpMemRes)++ =
                              (*lpPrimSpec->FisFvF) (GetNextFloat (lpMemLft, aplTypeLft, uLft),
                                                     GetNextFloat (lpMemRht, aplTypeRht, uRht),
                                                     lpPrimSpec);
                        } // End FOR
                    } else
                    // If both arguments are numeric
                    if (IsNumeric (aplTypeLft)
                     && IsNumeric (aplTypeRht))     // Res = FLOAT(Axis), Lft = RAT/VFP/..., Rht = RAT/VFP/...
                    {
                        DbgBrk ();              // Can't happen with any known primitive




                    } else
                        DbgStop ();     // We should never get here
                    break;

                case ARRAY_RAT:                     // Res = RAT(Axis)
                    // Initialize the temps
                    mpq_init (&aplRatLft);
                    mpq_init (&aplRatRht);

                    // Loop through the left/right args/result
                    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Calculate the left and right argument indices
                        CalcLftRhtArgIndices (uRes,
                                              aplRankRes,
                                             &uLft,
                                              aplRankLft,
                                             &uRht,
                                              aplRankRht,
                                              bLftIdent,
                                              bRhtIdent,
                                              aplNELMAxis,
                                              lpMemAxisHead,
                                              lpMemOdo,
                                              lpMemWVec,
                                              lpMemDimRes);
                        // Promote the left arg to Rational
                        // Split cases based upon the left arg type
                        switch (aplTypeLft)
                        {
                            case ARRAY_BOOL:            // Res = RAT(Axis), Lft = BOOL
                            case ARRAY_INT:             // Res = RAT(Axis), Lft = INT
                            case ARRAY_APA:             // Res = RAT(Axis), Lft = APA
                                // Convert the BOOL/INT/APA to a RAT
                                mpq_set_sx (&aplRatLft, GetNextInteger (lpMemLft, aplTypeLft, uLft), 1);

                                break;

                            case ARRAY_FLOAT:           // Res = RAT(Axis), Lft = FLOAT
                                // Convert the FLOAT to a RAT
                                mpq_set_d  (&aplRatLft, GetNextFloat   (lpMemLft, aplTypeLft, uLft));

                                break;

                            case ARRAY_RAT:             // Res = RAT(Axis), Lft = RAT
                                // Copy the RAT to a RAT
                                mpq_set    (&aplRatLft, &((LPAPLRAT) lpMemLft)[uLft]);

                                break;

                            case ARRAY_CHAR:            // Can't happen
                            case ARRAY_VFP:             // Can't happen
                            defstop
                                break;
                        } // End SWITCH

                        // Promote the right arg to Rational
                        // Split cases based upon the right arg type
                        switch (aplTypeRht)
                        {
                            case ARRAY_BOOL:            // Res = RAT(Axis), Rht = BOOL
                            case ARRAY_INT:             // Res = RAT(Axis), Rht = INT
                            case ARRAY_APA:             // Res = RAT(Axis), Rht = APA
                                // Convert the BOOL/INT/APA to a RAT
                                mpq_set_sx (&aplRatRht, aplIntegerRht, 1);

                                break;

                            case ARRAY_FLOAT:           // Res = RAT(Axis), Rht = FLOAT
                                // Convert the FLOAT to a RAT
                                mpq_set_d  (&aplRatRht, aplFloatRht);

                                break;

                            case ARRAY_RAT:             // Res = RAT(Axis), Rht = RAT
                                // Copy the RAT to a RAT
                                mpq_set    (&aplRatRht, &((LPAPLRAT) lpMemRht)[uRht]);

                                break;

                            case ARRAY_CHAR:            // Can't happen
                            case ARRAY_VFP:             // Can't happen
                            defstop
                                break;
                        } // End SWITCH

                        // Save in the result
                        *((LPAPLRAT  ) lpMemRes)++ =
                          (*lpPrimSpec->RisRvR) (aplRatLft,
                                                 aplRatRht,
                                                 lpPrimSpec);
                    } // End FOR

                    // We no longer need this storage
                    Myq_clear (&aplRatRht);
                    Myq_clear (&aplRatLft);

                    break;

                case ARRAY_VFP:                     // Res = VFP(Axis)
                    // Initialize the temps
                    mpfr_init0 (&aplVfpLft);
                    mpfr_init0 (&aplVfpRht);

                    // Loop through the left/right args/result
                    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Calculate the left and right argument indices
                        CalcLftRhtArgIndices (uRes,
                                              aplRankRes,
                                             &uLft,
                                              aplRankLft,
                                             &uRht,
                                              aplRankRht,
                                              bLftIdent,
                                              bRhtIdent,
                                              aplNELMAxis,
                                              lpMemAxisHead,
                                              lpMemOdo,
                                              lpMemWVec,
                                              lpMemDimRes);
                        // Promote the left arg to VFP
                        // Split cases based upon the left arg type
                        switch (aplTypeLft)
                        {
                            case ARRAY_BOOL:            // Res = VFP(Axis), Lft = BOOL
                            case ARRAY_INT:             // Res = VFP(Axis), Lft = INT
                            case ARRAY_APA:             // Res = VFP(Axis), Lft = APA
                                // Convert the BOOL/INT/APA to a VFP
                                mpfr_set_sx (&aplVfpLft, GetNextInteger (lpMemLft, aplTypeLft, uLft), MPFR_RNDN);

                                break;

                            case ARRAY_FLOAT:           // Res = VFP(Axis), Lft = FLOAT
                                // Convert the FLOAT to a VFP
                                mpfr_set_d  (&aplVfpLft,  ((LPAPLFLOAT) lpMemLft)[uLft], MPFR_RNDN);

                                break;

                            case ARRAY_RAT:             // Res = VFP(Axis), Lft = RAT
                                // Convert the RAT to a VFP
                                mpfr_set_q  (&aplVfpLft, &((LPAPLRAT) lpMemLft)[uLft], MPFR_RNDN);

                                break;

                            case ARRAY_VFP:             // Res = VFP(Axis), Lft = VFP
                                // Copy the VFP to a VFP
                                mpfr_copy   (&aplVfpLft, &((LPAPLVFP) lpMemLft)[uLft]);

                                break;

                            case ARRAY_CHAR:            // Can't happen
                            defstop
                                break;
                        } // End SWITCH

                        // Promote the right arg to VFP
                        // Split cases based upon the right arg type
                        switch (aplTypeRht)
                        {
                            case ARRAY_BOOL:            // Res = VFP(Axis), Rht = BOOL
                            case ARRAY_INT:             // Res = VFP(Axis), Rht = INT
                            case ARRAY_APA:             // Res = VFP(Axis), Rht = APA
                                // Convert the BOOL/INT/APA to a VFP
                                mpfr_set_sx (&aplVfpRht, aplIntegerRht, MPFR_RNDN);

                                break;

                            case ARRAY_FLOAT:           // Res = VFP(Axis), Rht = FLOAT
                                // Convert the FLOAT to a VFP
                                mpfr_set_d  (&aplVfpRht,  ((LPAPLFLOAT) lpMemRht)[uLft], MPFR_RNDN);

                                break;

                            case ARRAY_RAT:             // Res = VFP(Axis), Rht = RAT
                                // Convert the RAT to a VFP
                                mpfr_set_q  (&aplVfpRht, &((LPAPLRAT) lpMemRht)[uLft], MPFR_RNDN);

                                break;

                            case ARRAY_VFP:             // Res = VFP(Axis), Rht = VFP
                                // Copy the VFP to a VFP
                                mpfr_copy   (&aplVfpRht, &((LPAPLVFP) lpMemRht)[uRht]);

                                break;

                            case ARRAY_CHAR:            // Can't happen
                            defstop
                                break;
                        } // End SWITCH

                        // Save in the result
                        *((LPAPLVFP  ) lpMemRes)++ =
                          (*lpPrimSpec->VisVvV) (aplVfpLft,
                                                 aplVfpRht,
                                                 lpPrimSpec);
                    } // End FOR

                    // We no longer need this storage
                    Myf_clear (&aplVfpRht);
                    Myf_clear (&aplVfpLft);

                    break;

                defstop
                    break;
            } // End SWITCH
            } __except (CheckException (GetExceptionInformation (), L"PrimFnDydSimpSimp_EM #2"))
            {
                EXCEPTION_CODES ExceptionCode = MyGetExceptionCode ();  // The exception code

                dprintfWL9 (L"!!Initiating Exception in " APPEND_NAME L" #9: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                // Split cases based upon the ExceptionCode
                switch (ExceptionCode)
                {
                    case EXCEPTION_DOMAIN_ERROR:
                    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                    case EXCEPTION_INT_DIVIDE_BY_ZERO:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        goto DOMAIN_EXIT;

                    case EXCEPTION_NONCE_ERROR:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        goto NONCE_EXIT;

                    case EXCEPTION_RESULT_RAT:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        if (IsNumeric (aplTypeRes)
                         && !IsRat    (aplTypeRes))
                        {
                            // It's now a RAT result
                            aplTypeRes = ARRAY_RAT;

                            // We need to start over with the result
                            MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
                            FreeResultGlobalVar (*lphGlbRes); *lphGlbRes = NULL;

                            if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                                             lphGlbRes,
                                                             lpMemHdrLft,
                                                             lpMemHdrRht,
                                                             aplRankLft,
                                                             aplRankRht,
                                                            &aplRankRes,
                                                             aplTypeRes,
                                                             bLftIdent,
                                                             bRhtIdent,
                                                             aplNELMLft,
                                                             aplNELMRht,
                                                             aplNELMRes))
                                goto ERROR_EXIT;

                            // Lock the memory to get a ptr to it
                            lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);

                            // If the result is not nested, ...
                            if (!IsNested (lpMemHdrRes->ArrType))
                                // Tell the header about it
                                lpMemHdrRes->ArrType = aplTypeRes;

                            // Re-initialize lpMemOdo
                            for (uRes = 0 ; uRes < (APLRANKSIGN) aplRankRes; uRes++)
                                lpMemOdo[uRes] = 0;

                            dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #9: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                            goto RESTART_EXCEPTION_AXIS;
                        } // End IF

                        // Display message for unhandled exception
                        DisplayException ();

                        break;

                    case EXCEPTION_RESULT_VFP:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        if (IsNumeric (aplTypeRes)
                         && !IsVfp    (aplTypeRes))
                        {
                            // It's now a VFP result
                            aplTypeRes = ARRAY_VFP;

                            // We need to start over with the result
                            MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
                            FreeResultGlobalVar (*lphGlbRes); *lphGlbRes = NULL;

                            if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                                             lphGlbRes,
                                                             lpMemHdrLft,
                                                             lpMemHdrRht,
                                                             aplRankLft,
                                                             aplRankRht,
                                                            &aplRankRes,
                                                             aplTypeRes,
                                                             bLftIdent,
                                                             bRhtIdent,
                                                             aplNELMLft,
                                                             aplNELMRht,
                                                             aplNELMRes))
                                goto ERROR_EXIT;

                            // Lock the memory to get a ptr to it
                            lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);

                            // If the result is not nested, ...
                            if (!IsNested (lpMemHdrRes->ArrType))
                                // Tell the header about it
                                lpMemHdrRes->ArrType = aplTypeRes;

                            // Re-initialize lpMemOdo
                            for (uRes = 0 ; uRes < (APLRANKSIGN) aplRankRes; uRes++)
                                lpMemOdo[uRes] = 0;

                            dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #9: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                            goto RESTART_EXCEPTION_AXIS;
                        } // End IF

                        // Display message for unhandled exception
                        DisplayException ();

                        break;

                    case EXCEPTION_RESULT_FLOAT:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        if (IsNumeric (aplTypeRes)
                         && !IsSimpleFlt (aplTypeRes))
                        {
                            // If the previous result is Boolean, we need to
                            //   unlock, free, and allocate the result anew
                            if (IsSimpleBool (aplTypeRes))
                            {
                                // It's now a FLOAT result
                                aplTypeRes = ARRAY_FLOAT;

                                // We need to start over with the result
                                MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
                                FreeResultGlobalVar (*lphGlbRes); *lphGlbRes = NULL;

                                if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                                                 lphGlbRes,
                                                                 lpMemHdrLft,
                                                                 lpMemHdrRht,
                                                                 aplRankLft,
                                                                 aplRankRht,
                                                                &aplRankRes,
                                                                 aplTypeRes,
                                                                 bLftIdent,
                                                                 bRhtIdent,
                                                                 aplNELMLft,
                                                                 aplNELMRht,
                                                                 aplNELMRes))
                                    goto ERROR_EXIT;

                                // Lock the memory to get a ptr to it
                                lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);
                            } else
                            // The previous result must have been INT which is
                            //   the same size as FLOAT, so there's no need to
                            //   change storage.
                            {
                                // It's now a FLOAT result
                                aplTypeRes = ARRAY_FLOAT;

                                // Restart the pointers
                                lpMemRes = lpMemHdrRes;
                            } // End IF/ELSE

                            // If the result is not nested, ...
                            if (!IsNested (lpMemHdrRes->ArrType))
                                // Tell the header about it
                                lpMemHdrRes->ArrType = aplTypeRes;

                            // Re-initialize lpMemOdo
                            for (uRes = 0 ; uRes < (APLRANKSIGN) aplRankRes; uRes++)
                                lpMemOdo[uRes] = 0;

                            dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #9: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                            goto RESTART_EXCEPTION_AXIS;
                        } // End IF

                        // Fall through to never-never-land

                    default:
                        // Display message for unhandled exception
                        DisplayException ();

                        break;
                } // End SWITCH
            } // End __try/__except
        } else      // Axis is not significant, neither argument is singleton
        {
RESTART_EXCEPTION_NOAXIS:
            // Skip over the header and dimensions to the data
            lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

            // Skip over the header to the dimensions
            lpMemDimRes = VarArrayBaseToDim (lpMemHdrRes);

            __try
            {
            // Split cases based upon the result's storage type
            switch (aplTypeRes)
            {
                case ARRAY_BOOL:                    // Res = BOOL(No Axis)
                    // If both arguments are BOOL,
                    //   use BisBvB
                    if (IsSimpleBool (aplTypeLft)
                     && IsSimpleBool (aplTypeRht))  // Res = BOOL(No Axis), Lft = BOOL, Rht = BOOL
                    {
                        // Initialize # remaining NELM
                        aplNELMRem = aplNELMRes;

                        // Check for optimized chunking
                        if (lpPrimSpec->B64isB64vB64 NE NULL)
                        {
                            // Calculate the # 64-bit chunks
                            aplNELMTmp  = aplNELMRem / 64;
                            aplNELMRem -= aplNELMTmp * 64;

                            // Loop through the left/right args/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB64) lpMemRes)++ =
                                  (*lpPrimSpec->B64isB64vB64) (*((LPAPLB64) lpMemLft)++, *((LPAPLB64) lpMemRht)++, lpPrimSpec);
                            } // End FOR

                            // Calculate the # remaining 32-bit chunks
                            aplNELMTmp  = aplNELMRem / 32;
                            aplNELMRem -= aplNELMTmp * 32;

                            // Loop through the left/right args/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB32) lpMemRes)++ =
                                  (*lpPrimSpec->B32isB32vB32) (*((LPAPLB32) lpMemLft)++, *((LPAPLB32) lpMemRht)++, lpPrimSpec);
                            } // End FOR

                            // Calculate the # remaining 16-bit chunks
                            aplNELMTmp  = aplNELMRem / 16;
                            aplNELMRem -= aplNELMTmp * 16;

                            // Loop through the left/right args/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB16) lpMemRes)++ =
                                  (*lpPrimSpec->B16isB16vB16) (*((LPAPLB16) lpMemLft)++, *((LPAPLB16) lpMemRht)++, lpPrimSpec);
                            } // End FOR

                            // Calculate the # remaining  8-bit chunks
                            aplNELMTmp  = aplNELMRem /  8;
                            aplNELMRem -= aplNELMTmp *  8;

                            // Loop through the left/right args/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB08) lpMemRes)++ =
                                  (*lpPrimSpec->B08isB08vB08) (*((LPAPLB08) lpMemLft)++, *((LPAPLB08) lpMemRht)++, lpPrimSpec);
                            } // End FOR
                        } // End IF

                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRem; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLBOOL)  lpMemRes) |=
                              (*lpPrimSpec->BisBvB) ((APLBOOL) GetNextInteger (lpMemLft, aplTypeLft, uRes),
                                                     (APLBOOL) GetNextInteger (lpMemRht, aplTypeRht, uRes),
                                                     lpPrimSpec) << uBitIndex;
                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;                  // Start over
                                ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                            } // End IF
                        } // End FOR
                    } else
                    // If the left arg is Boolean APA and the right arg is BOOL, ...
#define lpAPA       ((LPAPLAPA) lpMemLft)
                    if (IsSimpleAPA  (aplTypeLft)
                     && IsBooleanAPA (lpAPA)
                     && lpPrimSpec->B64isB64vB64
                     && IsSimpleBool (aplTypeRht))  // Res = BOOL(No Axis), Lft = APA , Rht = BOOL
                    {
                        APLB64 aplB64APA;

                        if (lpAPA->Off NE 0)
                            aplB64APA = 0xFFFFFFFFFFFFFFFF;
                        else
                            aplB64APA = 0x0000000000000000;
#undef  lpAPA
                        // Initialize # remaining NELM
                        aplNELMRem = aplNELMRht;

                        // Calculate the # 64-bit chunks
                        aplNELMTmp  = aplNELMRem / 64;
                        aplNELMRem -= aplNELMTmp * 64;

                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLB64) lpMemRes)++ =
                              (*lpPrimSpec->B64isB64vB64) ((APLB64) aplB64APA, *((LPAPLB64) lpMemRht)++, lpPrimSpec);
                        } // End FOR

                        // Calculate the # remaining 32-bit chunks
                        aplNELMTmp  = aplNELMRem / 32;
                        aplNELMRem -= aplNELMTmp * 32;

                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLB32) lpMemRes)++ =
                              (*lpPrimSpec->B32isB32vB32) ((APLB32) aplB64APA, *((LPAPLB32) lpMemRht)++, lpPrimSpec);
                        } // End FOR

                        // Calculate the # remaining 16-bit chunks
                        aplNELMTmp  = aplNELMRem / 16;
                        aplNELMRem -= aplNELMTmp * 16;

                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLB16) lpMemRes)++ =
                              (*lpPrimSpec->B16isB16vB16) ((APLB16) aplB64APA, *((LPAPLB16) lpMemRht)++, lpPrimSpec);
                        } // End FOR

                        // Calculate the # remaining  8-bit chunks
                        aplNELMTmp  = aplNELMRem /  8;
                        aplNELMRem -= aplNELMTmp *  8;

                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLB08) lpMemRes)++ =
                              (*lpPrimSpec->B08isB08vB08) ((APLB08) aplB64APA, *((LPAPLB08) lpMemRht)++, lpPrimSpec);
                        } // End FOR

                        // If any bits remain, ...
                        if (!IsEmpty (aplNELMRem))
                            *((LPAPLB08) lpMemRes) =
                              (*lpPrimSpec->B08isB08vB08) ((APLB08) aplB64APA, *(LPAPLB08) lpMemRht, lpPrimSpec)
                            & (BIT0 << (UINT) aplNELMRem) - 1;
                    } else
                    // If the left arg is BOOL and the right arg is Boolean APA, ...
#define lpAPA       ((LPAPLAPA) lpMemRht)
                    if (IsSimpleAPA  (aplTypeRht)
                     && IsBooleanAPA (lpAPA)
                     && lpPrimSpec->B64isB64vB64
                     && IsSimpleBool (aplTypeLft))  // Res = BOOL(No Axis), Lft = BOOL, Rht = APA
                    {
                        APLB64 aplB64APA;

                        if (lpAPA->Off NE 0)
                            aplB64APA = 0xFFFFFFFFFFFFFFFF;
                        else
                            aplB64APA = 0x0000000000000000;
#undef  lpAPA
                        // Initialize # remaining NELM
                        aplNELMRem = aplNELMRht;

                        // Calculate the # 64-bit chunks
                        aplNELMTmp  = aplNELMRem / 64;
                        aplNELMRem -= aplNELMTmp * 64;

                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLB64) lpMemRes)++ =
                              (*lpPrimSpec->B64isB64vB64) (*((LPAPLB64) lpMemLft)++, (APLB64) aplB64APA, lpPrimSpec);
                        } // End FOR

                        // Calculate the # remaining 32-bit chunks
                        aplNELMTmp  = aplNELMRem / 32;
                        aplNELMRem -= aplNELMTmp * 32;

                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLB32) lpMemRes)++ =
                              (*lpPrimSpec->B32isB32vB32) (*((LPAPLB32) lpMemLft)++, (APLB32) aplB64APA, lpPrimSpec);
                        } // End FOR

                        // Calculate the # remaining 16-bit chunks
                        aplNELMTmp  = aplNELMRem / 16;
                        aplNELMRem -= aplNELMTmp * 16;

                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLB16) lpMemRes)++ =
                              (*lpPrimSpec->B16isB16vB16) (*((LPAPLB16) lpMemLft)++, (APLB16) aplB64APA, lpPrimSpec);
                        } // End FOR

                        // Calculate the # remaining  8-bit chunks
                        aplNELMTmp  = aplNELMRem /  8;
                        aplNELMRem -= aplNELMTmp *  8;

                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLB08) lpMemRes)++ =
                              (*lpPrimSpec->B08isB08vB08) (*((LPAPLB08) lpMemLft)++, (APLB08) aplB64APA, lpPrimSpec);
                        } // End FOR

                        // If any bits remain, ...
                        if (!IsEmpty (aplNELMRem))
                            *((LPAPLB08) lpMemRes) =
                              (*lpPrimSpec->B08isB08vB08) (*(LPAPLB08) lpMemLft, (APLB08) aplB64APA, lpPrimSpec)
                            & (BIT0 << (UINT) aplNELMRem) - 1;
                    } else
                    // If both arguments are integer-like (BOOL, INT, or APA),
                    //   use BisIvI
                    if (IsSimpleInt (aplTypeLft)
                     && IsSimpleInt (aplTypeRht))   // Res = BOOL(No Axis), Lft = BOOL/INT/APA, Rht = BOOL/INT/APA
                    {
                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLBOOL)  lpMemRes) |=
                              (*lpPrimSpec->BisIvI) (GetNextInteger (lpMemLft, aplTypeLft, uRes),
                                                     GetNextInteger (lpMemRht, aplTypeRht, uRes),
                                                     lpPrimSpec) << uBitIndex;
                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;                  // Start over
                                ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                            } // End IF
                        } // End FOR
                    } else
                    // If both arguments are CHAR,
                    //   use BisCvC
                    if (IsSimpleChar (aplTypeLft)
                     && IsSimpleChar (aplTypeRht))  // Res = BOOL(No Axis), Lft = CHAR, Rht = CHAR
                    {
                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLBOOL)  lpMemRes) |=
                              (*lpPrimSpec->BisCvC) (((LPAPLCHAR) lpMemLft)[uRes],
                                                     ((LPAPLCHAR) lpMemRht)[uRes],
                                                     lpPrimSpec) << uBitIndex;
                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;                  // Start over
                                ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                            } // End IF
                        } // End FOR
                    } else
                    // If either argument is FLOAT and the other is simple numeric (BOOL, INT, APA, or FLOAT),
                    //   use BisFvF
                    if ((IsSimpleFlt (aplTypeLft) && IsSimpleNum (aplTypeRht))  // Res = BOOL(No Axis), Lft = FLOAT, Rht = BOOL/INT/APA/FLOAT
                     || (IsSimpleFlt (aplTypeRht) && IsSimpleNum (aplTypeLft))) // Res = BOOL(No Axis), Lft = BOOL/INT/APA/FLOAT, Rht = FLOAT
                    {
                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLBOOL)  lpMemRes) |=
                              (*lpPrimSpec->BisFvF) (GetNextFloat (lpMemLft, aplTypeLft, uRes),
                                                     GetNextFloat (lpMemRht, aplTypeRht, uRes),
                                                     lpPrimSpec) << uBitIndex;
                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;                  // Start over
                                ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                            } // End IF
                        } // End FOR
                    } else
                    // If either arg is hetero and the other is numeric or char, ...
                    if ((IsSimpleHet (aplTypeLft) && IsSimpleGlbNum (aplTypeRht))    // Res = BOOL(No Axis), Lft = HETERO, Rht = BOOL/INT/APA/FLOAT/CHAR/HETERO (one hetero, one simple)
                     || (IsSimpleHet (aplTypeRht) && IsSimpleGlbNum (aplTypeLft)))   // Res = BOOL(No Axis), Lft = BOOL/INT/APA/FLOAT/CHAR/HETERO, Rht = HETERO (one hetero, one simple)
                    {
                        // Initialize the temps
                        mpq_init (&aplRatLft);
                        mpq_init (&aplRatRht);
                        mpfr_init0 (&aplVfpLft);
                        mpfr_init0 (&aplVfpRht);

                        // Copy the storage types
                        aplTypeHetLft = aplTypeLft;
                        aplTypeHetRht = aplTypeRht;

                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Get the next values and type
                            if (IsSimpleHet (aplTypeLft))
                                aplTypeHetLft = GetNextHetero (lpMemLft, uRes, &aplIntegerLft, &aplFloatLft, &aplCharLft, &lpSymGlbLft);
                            else
                                GetNextSimple (lpMemLft,        // Ptr to item global memory data
                                               aplTypeLft,      // Item storage type
                                               aplNELMLft,      // Item NELM
                                               uRes,            // Index into item
                                              &aplIntegerLft,   // Ptr to Boolean/Integer result
                                              &aplFloatLft,     // Ptr to Float result
                                              &aplCharLft,      // Ptr to Char result
                                              &lpSymGlbLft);    // Ptr to global memory handle (may be NULL)
                            if (IsSimpleHet (aplTypeRht))
                                aplTypeHetRht = GetNextHetero (lpMemRht, uRes, &aplIntegerRht, &aplFloatRht, &aplCharRht, &lpSymGlbRht);
                            else
                                GetNextSimple (lpMemRht,        // Ptr to item global memory data
                                               aplTypeRht,      // Item storage type
                                               aplNELMRht,      // Item NELM
                                               uRes,            // Index into item
                                              &aplIntegerRht,   // Ptr to Boolean/Integer result
                                              &aplFloatRht,     // Ptr to Float result
                                              &aplCharRht,      // Ptr to Char result
                                              &lpSymGlbRht);    // Ptr to global memory handle (may be NULL)
                            // Split cases based upon the left hetero's storage type
                            switch (aplTypeHetLft)
                            {
                                case ARRAY_BOOL:            // Res = BOOL(No Axis), Lft = BOOL,  Rht = BOOL/INT/FLOAT/CHAR/RAT/VFP (one hetero, one simple)
                                    // Split cases based upon the right hetero's storage type
                                    switch (aplTypeHetRht)
                                    {
                                        case ARRAY_BOOL:    // Res = BOOL(No Axis), Lft = BOOL,  Rht = BOOL  (one hetero, one simple)
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisBvB) ((APLBOOL) aplIntegerLft,
                                                                     (APLBOOL) aplIntegerRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_INT:     // Res = BOOL(No Axis), Lft = BOOL, Rht = INT    (one hetero, one simple)
                                        case ARRAY_APA:     // Res = BOOL(No Axis), Lft = BOOL, Rht = APA    (one hetero, one simple)
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisIvI) ((APLBOOL) aplIntegerLft,
                                                                               aplIntegerRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_FLOAT:   // Res = BOOL(No Axis), Lft = BOOL, Rht = FLOAT  (one hetero, one simple)
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisFvF) ((APLFLOAT) aplIntegerLft,
                                                                                aplFloatRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_CHAR:    // Res = BOOL(No Axis), Lft = BOOL, Rht = CHAR   (one hetero, one simple)
                                            // One arg is numeric, the other char
                                            Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                                                 || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);
                                            // If the function is UTF16_NOTEQUAL, the result is one
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL) << uBitIndex;

                                            break;

                                        case ARRAY_RAT:     // Res = BOOL(No Axis), Lft = BOOL, Rht = RAT    (one hetero, one simple)
                                            mpq_set_sx (&aplRatLft, aplIntegerLft, 1);

                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisRvR) (aplRatLft,
                                                                    *(LPAPLRAT) lpSymGlbRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_VFP:     // Res = BOOL(No Axis), Lft = BOOL, Rht = VFP    (one hetero, one simple)
                                            mpfr_set_sx (&aplVfpLft, aplIntegerLft, MPFR_RNDN);

                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                                    *(LPAPLVFP) lpSymGlbRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        defstop
                                            break;
                                    } // End SWITCH

                                    break;

                                case ARRAY_INT:             // Res = BOOL(No Axis), Lft = INT,   Rht = BOOL/INT/FLOAT/CHAR/RAT/VFP
                                case ARRAY_APA:             // Res = BOOL(No Axis), Lft = APA,   Rht = BOOL/INT/FLOAT/CHAR/RAT/VFP
                                    // Split cases based upon the right hetero's storage type
                                    switch (aplTypeHetRht)
                                    {
                                        case ARRAY_BOOL:    // Res = BOOL(No Axis), Lft = INT,  Rht = BOOL  (one hetero, one simple)
                                        case ARRAY_INT:     // Res = BOOL(No Axis), Lft = INT,  Rht = INT   (one hetero, one simple)
                                        case ARRAY_APA:     // Res = BOOL(No Axis), Lft = INT,  Rht = APA   (one hetero, one simple)
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisIvI) (aplIntegerLft,
                                                                     aplIntegerRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_FLOAT:   // Res = BOOL(No Axis), Lft = INT,  Rht = FLOAT (one hetero, one simple)
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                                     aplFloatRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_CHAR:    // Res = BOOL(No Axis), Lft = INT,  Rht = CHAR  (one hetero, one simple)
                                            // One arg is numeric, the other char
                                            Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                                                 || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);
                                            // If the function is UTF16_NOTEQUAL, the result is one
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL) << uBitIndex;

                                            break;

                                        case ARRAY_RAT:     // Res = BOOL(No Axis), Lft = INT , Rht = RAT    (one hetero, one simple)
                                            mpq_set_sx (&aplRatLft, aplIntegerLft, 1);

                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisRvR) (aplRatLft,
                                                                    *(LPAPLRAT) lpSymGlbRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_VFP:     // Res = BOOL(No Axis), Lft = INT , Rht = VFP    (one hetero, one simple)
                                            mpfr_set_sx (&aplVfpLft, aplIntegerLft, MPFR_RNDN);

                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                                    *(LPAPLVFP) lpSymGlbRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        defstop
                                            break;
                                    } // End SWITCH

                                    break;

                                case ARRAY_FLOAT:           // Res = BOOL(No Axis), Lft = FLOAT, Rht = BOOL/INT/FLOAT/CHAR/RAT/VFP (one hetero, one simple)
                                    // Split cases based upon the right hetero's storage type
                                    switch (aplTypeHetRht)
                                    {
                                        case ARRAY_BOOL:    // Res = BOOL(No Axis), Lft = FLOAT, Rht = BOOL  (one hetero, one simple)
                                        case ARRAY_INT:     // Res = BOOL(No Axis), Lft = FLOAT, Rht = INT   (one hetero, one simple)
                                        case ARRAY_APA:     // Res = BOOL(No Axis), Lft = FLOAT, Rht = APA   (one hetero, one simple)
                                        case ARRAY_FLOAT:   // Res = BOOL(No Axis), Lft = FLOAT, Rht = FLOAT (one hetero, one simple)
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                                     aplFloatRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_CHAR:    // Res = BOOL(No Axis), Lft = FLOAT, Rht = CHAR  (one hetero, one simple)
                                            // One arg is numeric, the other char
                                            Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                                                 || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);
                                            // If the function is UTF16_NOTEQUAL, the result is one
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL) << uBitIndex;

                                            break;

                                        case ARRAY_RAT:     // Res = BOOL(No Axis), Lft = FLOAT, Rht = RAT    (one hetero, one simple)
                                            // Convert the FLOAT to a VFP
                                            mpfr_set_d (&aplVfpLft, aplFloatLft, MPFR_RNDN);

                                            // Convert the RAT to a VFP
                                            mpfr_set_q (&aplVfpRht, (LPAPLRAT) lpSymGlbRht, MPFR_RNDN);

                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                                     aplVfpRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_VFP:     // Res = BOOL(No Axis), Lft = FLOAT, Rht = VFP    (one hetero, one simple)
                                            mpfr_set_d (&aplVfpLft, aplFloatLft, MPFR_RNDN);

                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                                    *(LPAPLVFP) lpSymGlbRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        defstop
                                            break;
                                    } // End SWITCH

                                    break;

                                case ARRAY_CHAR:            // Res = BOOL(No Axis), Lft = CHAR,  Rht = BOOL/INT/FLOAT/CHAR/RAT/VFP (one hetero, one simple)
                                    // Split cases based upon the right hetero's storage type
                                    switch (aplTypeHetRht)
                                    {
                                        case ARRAY_BOOL:    // Res = BOOL(No Axis), Lft = CHAR,  Rht = BOOL  (one hetero, one simple)
                                        case ARRAY_INT:     // Res = BOOL(No Axis), Lft = CHAR,  Rht = INT   (one hetero, one simple)
                                        case ARRAY_APA:     // Res = BOOL(No Axis), Lft = CHAR,  Rht = APA   (one hetero, one simple)
                                        case ARRAY_FLOAT:   // Res = BOOL(No Axis), Lft = CHAR,  Rht = FLOAT (one hetero, one simple)
                                        case ARRAY_RAT:     // Res = BOOL(No Axis), Lft = CHAR,  Rht = RAT   (one hetero, one simple)
                                        case ARRAY_VFP:     // Res = BOOL(No Axis), Lft = CHAR,  Rht = VFP   (one hetero, one simple)
                                            // One arg is numeric, the other char
                                            Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                                                 || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);
                                            // If the function is UTF16_NOTEQUAL, the result is one
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL) << uBitIndex;

                                            break;

                                        case ARRAY_CHAR:    // Res = BOOL(No Axis), Lft = CHAR,  Rht = CHAR  (one hetero, one simple)
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisCvC) (aplCharLft,
                                                                     aplCharRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        defstop
                                            break;
                                    } // End SWITCH

                                    break;

                                case ARRAY_RAT:             // Res = BOOL(No Axis), Lft = RAT  , Rht = BOOL/INT/FLOAT/CHAR/RAT/VFP (one hetero, one simple)
                                    // Split cases based upon the right hetero's storage type
                                    switch (aplTypeHetRht)
                                    {
                                        case ARRAY_BOOL:    // Res = BOOL(No Axis), Lft = RAT  , Rht = BOOL  (one hetero, one simple)
                                        case ARRAY_INT:     // Res = BOOL(No Axis), Lft = RAT  , Rht = INT   (one hetero, one simple)
                                        case ARRAY_APA:     // Res = BOOL(No Axis), Lft = RAT  , Rht = APA   (one hetero, one simple)
                                            // Convert the BOOL/INT to a RAT
                                            mpq_set_sx (&aplRatRht, aplIntegerRht, 1);

                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisRvR) (*(LPAPLRAT) lpSymGlbLft,
                                                                      aplRatRht,
                                                                      lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_FLOAT:   // Res = BOOL(No Axis), Lft = RAT  , Rht = FLOAT (one hetero, one simple)
                                            // Convert the RAT to a VFP
                                            mpfr_set_q (&aplVfpLft, (LPAPLRAT) lpSymGlbLft, MPFR_RNDN);

                                            // Convert the FLOAT to a VFP
                                            mpfr_set_d (&aplVfpRht, aplFloatRht, MPFR_RNDN);

                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                                     aplVfpRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_CHAR:    // Res = BOOL(No Axis), Lft = RAT  , Rht = CHAR  (one hetero, one simple)
                                            // One arg is numeric, the other char
                                            Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                                                 || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);
                                            // If the function is UTF16_NOTEQUAL, the result is one
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL) << uBitIndex;

                                            break;

                                        case ARRAY_RAT:     // Res = BOOL(No Axis), Lft = RAT  , Rht = RAT    (one hetero, one simple)
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisRvR) (*(LPAPLRAT) lpSymGlbLft,
                                                                     *(LPAPLRAT) lpSymGlbRht,
                                                                      lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_VFP:     // Res = BOOL(No Axis), Lft = RAT  , Rht = VFP    (one hetero, one simple)
                                            // Convert the RAT to a VFP
                                            mpfr_set_q (&aplVfpLft, (LPAPLRAT) lpSymGlbLft, MPFR_RNDN);

                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                                    *(LPAPLVFP) lpSymGlbRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        defstop
                                            break;
                                    } // End SWITCH

                                    break;

                                case ARRAY_VFP:             // Res = BOOL(No Axis), Lft = VFP  , Rht = BOOL/INT/FLOAT/CHAR/RAT/VFP (one hetero, one simple)
                                    // Split cases based upon the right hetero's storage type
                                    switch (aplTypeHetRht)
                                    {
                                        case ARRAY_BOOL:    // Res = BOOL(No Axis), Lft = VFP  , Rht = BOOL  (one hetero, one simple)
                                        case ARRAY_INT:     // Res = BOOL(No Axis), Lft = VFP  , Rht = INT   (one hetero, one simple)
                                        case ARRAY_APA:     // Res = BOOL(No Axis), Lft = VFP  , Rht = APA   (one hetero, one simple)
                                            // Convert the BOOL/INT to a VFP
                                            mpfr_set_sx (&aplVfpRht, aplIntegerRht, MPFR_RNDN);

                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisVvV) (*(LPAPLVFP) lpSymGlbLft,
                                                                      aplVfpRht,
                                                                      lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_FLOAT:   // Res = BOOL(No Axis), Lft = VFP  , Rht = FLOAT (one hetero, one simple)
                                            // Convert the FLOAT to a VFP
                                            mpfr_set_d (&aplVfpRht, aplFloatRht, MPFR_RNDN);

                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisVvV) (*(LPAPLVFP) lpSymGlbLft,
                                                                      aplVfpRht,
                                                                      lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_CHAR:    // Res = BOOL(No Axis), Lft = VFP  , Rht = CHAR  (one hetero, one simple)
                                            // One arg is numeric, the other char
                                            Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                                                 || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);
                                            // If the function is UTF16_NOTEQUAL, the result is one
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL) << uBitIndex;

                                            break;

                                        case ARRAY_RAT:     // Res = BOOL(No Axis), Lft = VFP  , Rht = RAT    (one hetero, one simple)
                                            // Convert the RAT to a VFP
                                            mpfr_set_q (&aplVfpRht, (LPAPLRAT) lpSymGlbRht, MPFR_RNDN);

                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisVvV) (*(LPAPLVFP) lpSymGlbLft,
                                                                      aplVfpRht,
                                                                      lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_VFP:     // Res = BOOL(No Axis), Lft = VFP  , Rht = VFP    (one hetero, one simple)
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisVvV) (*(LPAPLVFP) lpSymGlbLft,
                                                                     *(LPAPLVFP) lpSymGlbRht,
                                                                      lpPrimSpec) << uBitIndex;
                                            break;

                                        defstop
                                            break;
                                    } // End SWITCH

                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;                  // Start over
                                ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                            } // End IF
                        } // End FOR

                        // We no longer need this storage
                        Myf_clear (&aplVfpRht);
                        Myf_clear (&aplVfpLft);
                        Myq_clear (&aplRatRht);
                        Myq_clear (&aplRatLft);
                    } else
                    // If one arg is numeric and the other char, ...
                    if ((IsNumeric (aplTypeLft) && IsSimpleChar (aplTypeRht))     // Res = BOOL(No Axis), Lft = BOOL/INT/APA/FLOAT/RAT/VFP, Rht = CHAR
                     || (IsNumeric (aplTypeRht) && IsSimpleChar (aplTypeLft)))    // Res = BOOL(No Axis), Lft = CHAR, Rht = BOOL/INT/APA/FLOAT/RAT/VFP
                    {
                        // One arg is numeric, the other char
                        Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                             || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);

                        // If the function is not-equal, the result is all 1s
                        if (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL)
                            // Fill the result with all 1s
                            FillMemory (lpMemRes, (APLU3264) RoundUpBitsToBytes (aplNELMRes), 0xFF);
                    } else
                    // If either argument is global numeric, ...
                    if (IsGlbNum (aplTypeLft)
                     || IsGlbNum (aplTypeRht))
                    {
                        // Calculate the common storage type
                        aplTypeCom = aTypePromote[aplTypeLft][aplTypeRht];

                        // Initialize the temps
                        mpq_init (&aplRatLft);
                        mpq_init (&aplRatRht);
                        mpfr_init0 (&aplVfpLft);
                        mpfr_init0 (&aplVfpRht);

                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Promote the left arg to the common type
                            switch (aplTypeCom)
                            {
                                case ARRAY_RAT:                 // Res = BOOL(No Axis)
                                    // Split cases based upon the left arg storage type
                                    switch (aplTypeLft)
                                    {
                                        case ARRAY_BOOL:        // Res = BOOL(No Axis), Lft = BOOL
                                        case ARRAY_INT:         // Res = BOOL(No Axis), Lft = INT
                                        case ARRAY_APA:         // Res = BOOL(No Axis), Lft = APA
                                            // Convert the BOOL/INT/APA to a RAT
                                            mpq_set_sx (&aplRatLft, GetNextInteger (lpMemLft, aplTypeLft, uRes), 1);

                                            break;

                                        case ARRAY_FLOAT:       // Res = BOOL(No Axis), Lft = FLOAT
                                            // Convert the FLOAT to a RAT
                                            mpq_set_d  (&aplRatLft, GetNextFloat   (lpMemLft, aplTypeLft, uRes));

                                            break;

                                        case ARRAY_RAT:         // Res = BOOL(No Axis), Lft = RAT
                                            // Copy the RAT to a RAT
                                            mpq_set (&aplRatLft, &((LPAPLRAT) lpMemLft)[uRes]);

                                            break;

                                        case ARRAY_CHAR:        // Can't happen
                                        case ARRAY_VFP:         // Can't happen
                                        defstop
                                            break;
                                    } // End SWITCH

                                    break;

                                case ARRAY_VFP:                 // Res = BOOL(No Axis)
                                    // Split cases based upon the left arg storage type
                                    switch (aplTypeLft)
                                    {
                                        case ARRAY_BOOL:        // Res = BOOL(No Axis), Lft = BOOL
                                        case ARRAY_INT:         // Res = BOOL(No Axis), Lft = INT
                                        case ARRAY_APA:         // Res = BOOL(No Axis), Lft = APA
                                            // Convert the BOOL/INT/APA to a VFP
                                            mpfr_set_sx (&aplVfpLft, GetNextInteger (lpMemLft, aplTypeLft, uRes), MPFR_RNDN);

                                            break;

                                        case ARRAY_FLOAT:       // Res = BOOL(No Axis), Lft = FLOAT
                                            // Convert the FLOAT to a VFP
                                            mpfr_set_d (&aplVfpLft, ((LPAPLFLOAT) lpMemLft)[uRes], MPFR_RNDN);

                                            break;

                                        case ARRAY_RAT:         // Res = BOOL(No Axis), Lft = RAT
                                            // Convert the RAT to a VFP
                                            mpfr_set_q (&aplVfpLft, &((LPAPLRAT) lpMemLft)[uRes], MPFR_RNDN);

                                            break;

                                        case ARRAY_VFP:         // Res = BOOL(No Axis), Lft = VFP
                                            // Copy the VFP to a VFP
                                            mpfr_copy (&aplVfpLft, &((LPAPLVFP) lpMemLft)[uRes]);

                                            break;

                                        case ARRAY_CHAR:        // Can't happen
                                        defstop
                                            break;
                                    } // End SWITCH

                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            // Promote the right arg to the common type
                            switch (aplTypeCom)
                            {
                                case ARRAY_RAT:                 // Res = BOOL(No Axis)
                                    // Split cases based upon the right arg storage type
                                    switch (aplTypeRht)
                                    {
                                        case ARRAY_BOOL:        // Res = BOOL(No Axis), Rht = BOOL
                                        case ARRAY_INT:         // Res = BOOL(No Axis), Rht = INT
                                        case ARRAY_APA:         // Res = BOOL(No Axis), Rht = APA
                                            // Convert the BOOL/INT/APA to a RAT
                                            mpq_set_sx (&aplRatRht, GetNextInteger (lpMemRht, aplTypeRht, uRes), 1);

                                            break;

                                        case ARRAY_FLOAT:       // Res = BOOL(No Axis), Rht = FLOAT
                                            // Convert the FLOAT to a RAT
                                            mpq_set_d  (&aplRatRht, GetNextFloat   (lpMemRht, aplTypeRht, uRes));

                                            break;

                                        case ARRAY_RAT:         // Res = BOOL(No Axis), Rht = RAT
                                            // Copy the RAT to a RAT
                                            mpq_set (&aplRatRht, &((LPAPLRAT) lpMemRht)[uRes]);

                                            break;

                                        case ARRAY_CHAR:        // Can't happen
                                        case ARRAY_VFP:         // Can't happen
                                        defstop
                                            break;
                                    } // End SWITCH

                                    break;

                                case ARRAY_VFP:                 // Res = BOOL(No Axis)
                                    // Split cases based upon the right arg storage type
                                    switch (aplTypeRht)
                                    {
                                        case ARRAY_BOOL:        // Res = BOOL(No Axis), Rht = BOOL
                                        case ARRAY_INT:         // Res = BOOL(No Axis), Rht = INT
                                        case ARRAY_APA:         // Res = BOOL(No Axis), Rht = APA
                                            // Convert the BOOL/INT/APA to a VFP
                                            mpfr_set_sx (&aplVfpRht, GetNextInteger (lpMemRht, aplTypeRht, uRes), MPFR_RNDN);

                                            break;

                                        case ARRAY_FLOAT:       // Res = BOOL(No Axis), Rht = FLOAT
                                            // Convert the FLOAT to a VFP
                                            mpfr_set_d (&aplVfpRht, ((LPAPLFLOAT) lpMemRht)[uRes], MPFR_RNDN);

                                            break;

                                        case ARRAY_RAT:         // Res = BOOL(No Axis), Rht = RAT
                                            // Convert the RAT to a VFP
                                            mpfr_set_q (&aplVfpRht, &((LPAPLRAT) lpMemRht)[uRes], MPFR_RNDN);

                                            break;

                                        case ARRAY_VFP:         // Res = BOOL(No Axis), Rht = VFP
                                            // Copy the VFP to a VFP
                                            mpfr_copy (&aplVfpRht, &((LPAPLVFP) lpMemRht)[uRes]);

                                            break;

                                        case ARRAY_CHAR:        // Can't happen
                                        defstop
                                            break;
                                    } // End SWITCH

                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            // Split cases based upon the common type
                            switch (aplTypeCom)
                            {
                                case ARRAY_RAT:                 // Res = Bool(No Axis), Lft = RAT, Rht = RAT
                                    *((LPAPLBOOL)  lpMemRes) |=
                                      (*lpPrimSpec->BisRvR) (aplRatLft,
                                                             aplRatRht,
                                                             lpPrimSpec) << uBitIndex;
                                    break;

                                case ARRAY_VFP:                 // Res = Bool(No Axis), Lft = VFP, Rht = VFP
                                    *((LPAPLBOOL)  lpMemRes) |=
                                      (*lpPrimSpec->BisVvV) (aplVfpLft,
                                                             aplVfpRht,
                                                             lpPrimSpec) << uBitIndex;
                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;                  // Start over
                                ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                            } // End IF
                        } // End FOR

                        // We no longer need this storage
                        Myf_clear (&aplVfpRht);
                        Myf_clear (&aplVfpLft);
                        Myq_clear (&aplRatRht);
                        Myq_clear (&aplRatLft);
                    } else
                        DbgStop ();         // We should never get here
                    break;

                case ARRAY_INT:                     // Res = INT(No Axis)
                    // If both left and right arguments are integer-like (BOOL, INT, or APA),
                    //   use IisIvI
                    if (IsSimpleInt (aplTypeLft)
                     && IsSimpleInt (aplTypeRht))   // Res = INT(No Axis), Lft = BOOL/INT/APA, Rht = BOOL/INT/APA
                    {
                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLINT)   lpMemRes)++ =
                              (*lpPrimSpec->IisIvI) (GetNextInteger (lpMemLft, aplTypeLft, uRes),
                                                     GetNextInteger (lpMemRht, aplTypeRht, uRes),
                                                     lpPrimSpec);
                        } // End FOR
                    } else
                    // If either argument is FLOAT and the other is simple numeric (BOOL, INT, APA, or FLOAT),
                    //   use IisFvF
                    if ((IsSimpleFlt (aplTypeLft) && IsSimpleNum (aplTypeRht))   // Res = INT(No Axis), Lft = FLOAT, Rht = BOOL/INT/APA/FLOAT
                     || (IsSimpleFlt (aplTypeRht) && IsSimpleNum (aplTypeLft)))  // Res = INT(No Axis), Lft = BOOL/INT/APA/FLOAT, Rht = FLOAT
                    {
                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLINT)   lpMemRes)++ =
                              (*lpPrimSpec->IisFvF) (GetNextFloat (lpMemLft, aplTypeLft, uRes),
                                                     GetNextFloat (lpMemRht, aplTypeRht, uRes),
                                                     lpPrimSpec);
                        } // End FOR
                    } else
                        DbgStop ();         // We should never get here
                    break;

                case ARRAY_FLOAT:                   // Res = FLOAT(No Axis)
                    // If both arguments are simple integer (BOOL, INT, APA),
                    //   use FisIvI
                    if (IsSimpleInt (aplTypeLft)
                     && IsSimpleInt (aplTypeRht))   // Res = FLOAT(No Axis), Lft = BOOL/INT/APA      , Rht = BOOL/INT/APA
                    {
                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLFLOAT) lpMemRes)++ =
                              (*lpPrimSpec->FisIvI) (GetNextInteger (lpMemLft, aplTypeLft, uRes),
                                                     GetNextInteger (lpMemRht, aplTypeRht, uRes),
                                                     lpPrimSpec);
                        } // End FOR
                    } else
                    // If both arguments are simple numeric (BOOL, INT, APA, FLOAT),
                    //   use FisFvF
                    if (IsSimpleNum (aplTypeLft)
                     && IsSimpleNum (aplTypeRht))   // Res = FLOAT(No Axis), Lft = BOOL/INT/APA/FLOAT, Rht = BOOL/INT/APA/FLOAT
                    {
                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLFLOAT) lpMemRes)++ =
                              (*lpPrimSpec->FisFvF) (GetNextFloat (lpMemLft, aplTypeLft, uRes),
                                                     GetNextFloat (lpMemRht, aplTypeRht, uRes),
                                                     lpPrimSpec);
                        } // End FOR
                    } else
                    // If both args are numeric
                    if (IsNumeric (aplTypeLft)      // Res = FLOAT(No Axis), Lft = RAT/VFP/..., Rht = RAT/VFP/...
                     && IsNumeric (aplTypeRht))
                    {
                        DbgBrk ();              // Can't happen with any known primitive




                    } else
                        DbgStop ();         // We should never get here
                    break;

                case ARRAY_RAT:                     // Res = RAT(No Axis)
                    // Initialize the temps
                    mpq_init (&aplRatLft);
                    mpq_init (&aplRatRht);

                    // Loop through the left/right args/result
                    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Promote the left arg to Rational
                        // Split cases based upon the left arg type
                        switch (aplTypeLft)
                        {
                            case ARRAY_BOOL:        // Res = RAT(No Axis), Lft = BOOL
                            case ARRAY_INT:         // Res = RAT(No Axis), Lft = INT
                            case ARRAY_APA:         // Res = RAT(No Axis), Lft = APA
                                // Convert the BOOL/INT/APA to a RAT
                                mpq_set_sx (&aplRatLft, GetNextInteger (lpMemLft, aplTypeLft, uRes), 1);

                                break;

                            case ARRAY_FLOAT:       // Res = RAT(No Axis), Lft = FLOAT
                                // Convert the FLOAT to a RAT
                                mpq_set_d  (&aplRatLft, GetNextFloat   (lpMemLft, aplTypeLft, uRes));

                                break;

                            case ARRAY_RAT:         // Res = RAT(No Axis), Lft = RAT
                                // Copy the RAT to a RAT
                                mpq_set    (&aplRatLft, ((LPAPLRAT) lpMemLft)++);

                                break;

                            case ARRAY_CHAR:        // Can't happen
                            case ARRAY_VFP:         // Can't happen
                            defstop
                                break;
                        } // End SWITCH

                        // Promote the right arg to Rational
                        // Split cases based upon the right arg type
                        switch (aplTypeRht)
                        {
                            case ARRAY_BOOL:        // Res = RAT(No Axis), Rht = BOOL
                            case ARRAY_INT:         // Res = RAT(No Axis), Rht = INT
                            case ARRAY_APA:         // Res = RAT(No Axis), Rht = APA
                                // Convert the BOOL/INT/APA to a RAT
                                mpq_set_sx (&aplRatRht, GetNextInteger (lpMemRht, aplTypeRht, uRes), 1);

                                break;

                            case ARRAY_FLOAT:       // Res = RAT(No Axis), Rht = FLOAT
                                // Convert the FLOAT to a RAT
                                mpq_set_d  (&aplRatRht, GetNextFloat   (lpMemRht, aplTypeRht, uRes));

                                break;

                            case ARRAY_RAT:         // Res = RAT(No Axis), Rht = RAT
                                // Copy the RAT to a RAT
                                mpq_set    (&aplRatRht, ((LPAPLRAT) lpMemRht)++);

                                break;

                            case ARRAY_CHAR:        // Can't happen
                            case ARRAY_VFP:         // Can't happen
                            defstop
                                break;
                        } // End SWITCH

                        // Save the result
                        ((LPAPLRAT) lpMemRes)[uRes] =
                          (*lpPrimSpec->RisRvR) (aplRatLft,
                                                 aplRatRht,
                                                 lpPrimSpec);
                    } // End FOR

                    // We no longer need this storage
                    Myq_clear (&aplRatRht);
                    Myq_clear (&aplRatLft);

                    break;

                case ARRAY_VFP:                     // Res = VFP(No Axis)
                    // Initialize the temps
                    mpfr_init0 (&aplVfpLft);
                    mpfr_init0 (&aplVfpRht);

                    // Loop through the left/right args/result
                    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Promote the left arg to VFP
                        // Split cases based upon the left arg type
                        switch (aplTypeLft)
                        {
                            case ARRAY_BOOL:        // Res = VFP(No Axis), Lft = BOOL
                            case ARRAY_INT:         // Res = VFP(No Axis), Lft = INT
                            case ARRAY_APA:         // Res = VFP(No Axis), Lft = APA
                                // Convert the BOOL/INT/APA to a VFP
                                mpfr_set_sx (&aplVfpLft, GetNextInteger (lpMemLft, aplTypeLft, uRes), MPFR_RNDN);

                                break;

                            case ARRAY_FLOAT:       // Res = VFP(No Axis), Lft = FLOAT
                                // Convert the FLOAT to a VFP
                                mpfr_set_d  (&aplVfpLft, *((LPAPLFLOAT) lpMemLft)++, MPFR_RNDN);

                                break;

                            case ARRAY_RAT:         // Res = VFP(No Axis), Lft = RAT
                                // Convert the RAT to a VFP
                                mpfr_set_q  (&aplVfpLft,  ((LPAPLRAT)   lpMemLft)++, MPFR_RNDN);

                                break;

                            case ARRAY_VFP:         // Res = VFP(No Axis), Lft = VFP
                                // Copy the VFP to a VFP
                                mpfr_copy   (&aplVfpLft,  ((LPAPLVFP)   lpMemLft)++);

                                break;

                            case ARRAY_CHAR:        // Can't happen
                            defstop
                                break;
                        } // End SWITCH

                        // Promote the right arg to VFP
                        // Split cases based upon the right arg type
                        switch (aplTypeRht)
                        {
                            case ARRAY_BOOL:        // Res = VFP(No Axis), Rht = BOOL
                            case ARRAY_INT:         // Res = VFP(No Axis), Rht = INT
                            case ARRAY_APA:         // Res = VFP(No Axis), Rht = APA
                                // Convert the BOOL/INT/APA to a VFP
                                mpfr_set_sx (&aplVfpRht, GetNextInteger (lpMemRht, aplTypeRht, uRes), MPFR_RNDN);

                                break;

                            case ARRAY_FLOAT:       // Res = VFP(No Axis), Rht = FLOAT
                                // Convert the FLOAT to a VFP
                                mpfr_set_d  (&aplVfpRht, *((LPAPLFLOAT) lpMemRht)++, MPFR_RNDN);

                                break;

                            case ARRAY_RAT:         // Res = VFP(No Axis), Rht = RAT
                                // Convert the RAT to a VFP
                                mpfr_set_q  (&aplVfpRht,  ((LPAPLRAT)   lpMemRht)++, MPFR_RNDN);

                                break;

                            case ARRAY_VFP:         // Res = VFP(No Axis), Rht = VFP
                                // Copy the VFP to a VFP
                                mpfr_copy   (&aplVfpRht,  ((LPAPLVFP)   lpMemRht)++);

                                break;

                            case ARRAY_CHAR:        // Can't happen
                            defstop
                                break;
                        } // End SWITCH

                        // Save the result
                        ((LPAPLVFP) lpMemRes)[uRes] =
                          (*lpPrimSpec->VisVvV) (aplVfpLft,
                                                 aplVfpRht,
                                                 lpPrimSpec);
                    } // End FOR

                    // We no longer need this storage
                    Myf_clear (&aplVfpRht);
                    Myf_clear (&aplVfpLft);

                    break;

                defstop
                    break;
            } // End SWITCH
            } __except (CheckException (GetExceptionInformation (), L"PrimFnDydSimpSimp_EM #3"))
            {
                EXCEPTION_CODES ExceptionCode = MyGetExceptionCode ();  // The exception code

                dprintfWL9 (L"!!Initiating Exception in " APPEND_NAME L" #10: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                // Split cases based upon the ExceptionCode
                switch (ExceptionCode)
                {
                    case EXCEPTION_DOMAIN_ERROR:
                    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                    case EXCEPTION_INT_DIVIDE_BY_ZERO:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        goto DOMAIN_EXIT;

                    case EXCEPTION_NONCE_ERROR:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        goto NONCE_EXIT;

                    case EXCEPTION_RESULT_RAT:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        if (IsNumeric (aplTypeRes)
                         && !IsRat    (aplTypeRes))
                        {
                            // It's now a RAT result
                            aplTypeRes = ARRAY_RAT;

                            // If the old result is not immediate, ...
                            if (lpMemHdrRes NE NULL)
                            {
                                // We need to start over with the result
                                MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
                                FreeResultGlobalVar (*lphGlbRes); *lphGlbRes = NULL;
                            } // End IF

                            if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                                             lphGlbRes,
                                                             lpMemHdrLft,
                                                             lpMemHdrRht,
                                                             aplRankLft,
                                                             aplRankRht,
                                                            &aplRankRes,
                                                             aplTypeRes,
                                                             bLftIdent,
                                                             bRhtIdent,
                                                             aplNELMLft,
                                                             aplNELMRht,
                                                             aplNELMRes))
                                goto ERROR_EXIT;

                            // Lock the memory to get a ptr to it
                            lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);

                            // Skip over the header and dimension to the data
                            lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
                            lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

                            // If the result is not nested, ...
                            if (!IsNested (lpMemHdrRes->ArrType))
                                // Tell the header about it
                                lpMemHdrRes->ArrType = aplTypeRes;

                            dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #10: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                            goto RESTART_EXCEPTION_NOAXIS;
                        } // End IF

                        // Display message for unhandled exception
                        DisplayException ();

                        break;

                    case EXCEPTION_RESULT_VFP:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        if (IsNumeric (aplTypeRes)
                         && !IsVfp    (aplTypeRes))
                        {
                            // It's now a VFP result
                            aplTypeRes = ARRAY_VFP;

                            // We need to start over with the result
                            MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
                            FreeResultGlobalVar (*lphGlbRes); *lphGlbRes = NULL;

                            if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                                             lphGlbRes,
                                                             lpMemHdrLft,
                                                             lpMemHdrRht,
                                                             aplRankLft,
                                                             aplRankRht,
                                                            &aplRankRes,
                                                             aplTypeRes,
                                                             bLftIdent,
                                                             bRhtIdent,
                                                             aplNELMLft,
                                                             aplNELMRht,
                                                             aplNELMRes))
                                goto ERROR_EXIT;

                            // Lock the memory to get a ptr to it
                            lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);

                            // Skip over the header and dimension to the data
                            lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
                            lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

                            // If the result is not nested, ...
                            if (!IsNested (lpMemHdrRes->ArrType))
                                // Tell the header about it
                                lpMemHdrRes->ArrType = aplTypeRes;

                            dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #10: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                            goto RESTART_EXCEPTION_NOAXIS;
                        } // End IF

                        // Display message for unhandled exception
                        DisplayException ();

                        break;

                    case EXCEPTION_RESULT_FLOAT:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        if (IsNumeric (aplTypeRes)
                         && !IsSimpleFlt (aplTypeRes))
                        {
                            // If the previous result is Boolean, we need to
                            //   unlock, free, and allocate the result anew
                            if (IsSimpleBool (aplTypeRes))
                            {
                                // It's now a FLOAT result
                                aplTypeRes = ARRAY_FLOAT;

                                // We need to start over with the result
                                MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
                                FreeResultGlobalVar (*lphGlbRes); *lphGlbRes = NULL;

                                if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                                                 lphGlbRes,
                                                                 lpMemHdrLft,
                                                                 lpMemHdrRht,
                                                                 aplRankLft,
                                                                 aplRankRht,
                                                                &aplRankRes,
                                                                 aplTypeRes,
                                                                 bLftIdent,
                                                                 bRhtIdent,
                                                                 aplNELMLft,
                                                                 aplNELMRht,
                                                                 aplNELMRes))
                                    goto ERROR_EXIT;

                                // Lock the memory to get a ptr to it
                                lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);
                            } else
                            // The previous result must have been INT which is
                            //   the same size as FLOAT, so there's no need to
                            //   change storage.
                            {
                                // It's now a FLOAT result
                                aplTypeRes = ARRAY_FLOAT;
                            } // End IF/ELSE

                            // If the result is not nested, ...
                            if (!IsNested (lpMemHdrRes->ArrType))
                                // Tell the header about it
                                lpMemHdrRes->ArrType = aplTypeRes;

                            // Skip over the header and dimension to the data
                            lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
                            lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

                            dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #10: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                            goto RESTART_EXCEPTION_NOAXIS;
                        } // End IF

                        // Fall through to never-never-land

                    default:
                        // Display message for unhandled exception
                        DisplayException ();

                        break;
                } // End SWITCH
            } // End __try/__except
        } // End IF/ELSE

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (*lphGlbRes);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } // End IF/ELSE/...

    if (!bRet)
        goto ERROR_EXIT;

    // Unlock the result global memory in case TypeDemote actually demotes
    if (*lphGlbRes NE NULL && lpMemRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
    } // End IF

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken);

    goto NORMAL_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    // Mark as in error
    bRet = FALSE;

    if (*lphGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (*lphGlbRes); *lphGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    // We no longer need this storage
    Myq_clear (&aplRatRht);
    Myq_clear (&aplRatLft);
    Myf_clear (&aplVfpRht);
    Myf_clear (&aplVfpLft);

    if (*lphGlbRes NE NULL && lpMemRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
    } // End IF

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbWVec, lpMemWVec);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbOdo, lpMemOdo);

    return bRet;
} // End PrimFnDydSimpSimp_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CalcLftRhtArgIndices
//
//  Calculate the left and right argument indices
//***************************************************************************

void CalcLftRhtArgIndices
    (APLINT    uRes,
     APLRANK   aplRankRes,
     LPAPLINT  lpuLft,
     APLRANK   aplRankLft,
     LPAPLINT  lpuRht,
     APLRANK   aplRankRht,
     UBOOL     bLftIdent,           // TRUE iff the function has a left identity element and the Axis tail is valid
     UBOOL     bRhtIdent,           // ...                         right ...
     APLNELM   aplNELMAxis,
     LPAPLUINT lpMemAxisHead,       // Ptr to axis values, fleshed out by CheckAxis_EM
     LPAPLUINT lpMemOdo,
     LPAPLUINT lpMemWVec,
     LPAPLDIM  lpMemDimRes)

{
    APLINT uArg, uInd;

    // Loop through the odometer values accumulating
    //   the weighted sum
    // Note that the contents of lpMemOdo change each time this function is called
    for (uArg = 0, uInd = aplRankRes - aplNELMAxis; uInd < (APLRANKSIGN) aplRankRes; uInd++)
        uArg += lpMemOdo[lpMemAxisHead[uInd]] * lpMemWVec[uInd];

    // Increment the odometer in lpMemOdo subject to
    //   the values in lpMemDimRes
    IncrOdometer (lpMemOdo, lpMemDimRes, NULL, aplRankRes);

    // Use the just computed index for the argument
    //   with the smaller rank
    if (aplRankLft > aplRankRht)
    {
        *lpuRht = uArg * !IsScalar (aplRankRht);
        *lpuLft = uRes;
    } else
    if (aplRankLft < aplRankRht)
    {
        *lpuLft = uArg * !IsScalar (aplRankLft);
        *lpuRht = uRes;
    } else
    {
        // If there's right identity element only, ...
        if (!bLftIdent && bRhtIdent)
        {
            *lpuRht = uArg * !IsScalar (aplRankRht);
            *lpuLft = uRes;
        } else
        {
            *lpuLft = uArg * !IsScalar (aplRankLft);
            *lpuRht = uRes;
        } // End IF/ELSE
    } // End IF/ELSE
} // End CalcLftRhtArgIndices


//***************************************************************************
//  $IsTknBooleanAPA
//
//  Return TRUE iff the token is a Boolean APA
//***************************************************************************

UBOOL IsTknBooleanAPA
    (LPTOKEN lptkArg)               // Ptr to token arg

{
    UBOOL             bRet = FALSE;         // TRUE iff the result is valid
    APLSTYPE          aplTypeArg;           // Arg storage type
    APLRANK           aplRankArg;           // Arg rank
    HGLOBAL           hGlbArg = NULL;       // Arg global memory handle
    LPVARARRAY_HEADER lpMemHdrArg = NULL;   // Ptr to arg header
    LPAPLAPA          lpMemArg;             // Ptr to arg global memory

    // Get the attributes (Type, NELM, and Rank)
    //   of the arg
    AttrsOfToken (lptkArg, &aplTypeArg, NULL, &aplRankArg, NULL);

    // Check for APA
    if (!IsSimpleAPA (aplTypeArg))
        goto NORMAL_EXIT;

    // Get arg global ptrs
    GetGlbPtrs_LOCK (lptkArg, &hGlbArg, &lpMemHdrArg);

    // Skip over the header to the data
    lpMemArg = VarArrayDataFmBase (lpMemHdrArg);

    // Check for Boolean APA
    bRet = IsBooleanAPA (lpMemArg);
NORMAL_EXIT:
    if (hGlbArg NE NULL && lpMemHdrArg NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbArg); lpMemHdrArg = NULL;
    } // End IF

    return bRet;
} // End IsTknBooleanAPA


//***************************************************************************
//  End of File: primspec.c
//***************************************************************************
