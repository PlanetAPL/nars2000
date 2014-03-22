//***************************************************************************
//  NARS2000 -- Primitive Function -- Epsilon
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


// Define the following var in order to grade the secondary
//   argument so as to catch duplicate values.
//   It's probably not worth the effort, so this option is
//     not recommended.
////#define GRADE2ND


//***************************************************************************
//  $PrimFnEpsilon_EM_YY
//
//  Primitive function for monadic and dyadic Epsilon ("enlist" and "member of")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnEpsilon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnEpsilon_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_EPSILON);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc APPEND_NAME_ARG);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonEpsilon_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydEpsilon_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnEpsilon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnEpsilon_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic Epsilon
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnEpsilon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnEpsilon_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnEpsilon_EM_YY,// Ptr to primitive function routine
                                    lptkLftArg,         // Ptr to left arg token
                                    lptkFunc,           // Ptr to function token
                                    lptkRhtArg,         // Ptr to right arg token
                                    lptkAxis);          // Ptr to axis token (may be NULL)
} // End PrimProtoFnEpsilon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonEpsilon_EM_YY
//
//  Primitive function for monadic Epsilon ("enlist")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonEpsilon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonEpsilon_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    //***************************************************************
    // Loop through the right arg counting all of the simple scalars
    //   and saving their storage type.  The result will be either
    //   simple homogeneous or simple heterogeneous.
    //***************************************************************

    // Split cases based upon the right arg's token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must traverse the array
            if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir_PTB (lptkRhtArg->tkData.tkSym->stData.stGlbData));

                return PrimFnMonEpsilonGlb_EM_YY
                       (lptkRhtArg->tkData.tkSym->stData.stGlbData, // HGLOBAL
                        lptkFunc);                                  // Ptr to function token
            } // End IF

            // Handle the immediate case
            return PrimFnMonEpsilonImm_EM_YY
                   (TranslateImmTypeToArrayType (lptkRhtArg->tkData.tkSym->stFlags.ImmType),    // Immediate type
                    lptkRhtArg->tkData.tkSym->stData.stLongest,                                 // Immediate value
                    lptkFunc);                                                                  // Ptr to function token
        case TKT_VARIMMED:
            return PrimFnMonEpsilonImm_EM_YY
                   (TranslateImmTypeToArrayType (lptkRhtArg->tkFlags.ImmType),  // Immediate type
                    lptkRhtArg->tkData.tkLongest,                               // Immediate value
                    lptkFunc);                                                  // Ptr to function token
        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (lptkRhtArg->tkData.tkGlbData));

            return PrimFnMonEpsilonGlb_EM_YY
                   (lptkRhtArg->tkData.tkGlbData,                   // HGLOBAL
                    lptkFunc);                                      // Ptr to function token
        defstop
            return NULL;
    } // End SWITCH
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimFnMonEpsilon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonEpsilonImm_EM_YY
//
//  Monadic Epsilon on an immediate value
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonEpsilonImm_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonEpsilonImm_EM_YY
    (ARRAY_TYPES aplTypeRes,    // Right arg storage type
     APLLONGEST  aplLongest,    // Right arg immediate value
     LPTOKEN     lptkFunc)      // Ptr to function token

{
    APLUINT      ByteRes;       // # bytes in the result
    HGLOBAL      hGlbRes;       // Result global memory handle
    LPVOID       lpMemRes;      // Ptr to result global memory
    LPPL_YYSTYPE lpYYRes;       // Ptr to the result

    // Calculate space needed for the result (a one-element vector)
    ByteRes = CalcArraySize (aplTypeRes, 1, 1);

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
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = 1;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Fill in the dimension
    *VarArrayBaseToDim (lpMemRes) = 1;

    // Skip over the header and dimension to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, 1);

    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:
            // Save the Boolean value
            *((LPAPLBOOL)  lpMemRes) = (APLBOOL)  aplLongest;

            break;

        case ARRAY_INT:
            // Save the Integer value
            *((LPAPLINT)   lpMemRes) = (APLINT)   aplLongest;

            break;

        case ARRAY_FLOAT:
            // Save the Float value
            *((LPAPLFLOAT) lpMemRes) = *(LPAPLFLOAT) &aplLongest;

            break;

        case ARRAY_CHAR:
            // Save the Char value
            *((LPAPLCHAR)  lpMemRes) = (APLCHAR)  aplLongest;

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

    return lpYYRes;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFnMonEpsilonImm_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonEpsilonGlb_EM_YY
//
//  Monadic Epsilon on a global memory object:
//     Count the # scalars and the common storage type
//     Allocate storage
//     Copy the data to the result
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonEpsilonGlb_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonEpsilonGlb_EM_YY
    (HGLOBAL hGlbRht,                       // Right arg global memory handle
     LPTOKEN lptkFunc)                      // Ptr to function token

{
    APLSTYPE          aplTypeRes,           // Result storage type
                      aplTypePro;           // Prototype ...
    APLNELM           aplNELMRes;           // # elements in the result
    HGLOBAL           hGlbRes = NULL;       // Result global memory handle
    LPVOID            lpMemRes = NULL;      // Ptr to result global memory
    APLUINT           ByteRes;              // # bytes in the result
    UINT              uBitMask = 0x01,      // Bit mask for marching through Booleans
                      uBitIndex = 0;        // Bit index ...
    UBOOL             bRet = TRUE;          // TRUE iff result is valid
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    LPVARARRAY_HEADER lpMemHdrRht;          // Ptr to right arg header

    // Traverse the array counting the # simple scalars
    //   and keeping track of the common storage type --
    //   it'll be simple homogeneous or heterogeneous.
    aplTypeRes = ARRAY_INIT;    // Initialize storage type
    aplNELMRes = 0;             // ...        count
    PrimFnMonEpsilonGlbCount (hGlbRht,      // Right arg global memory handle
                             &aplTypeRes,   // Ptr to result storage type
                             &aplNELMRes,   // Ptr to result NELM
                             &aplTypePro);  // Ptr to prototype storage type
    // Handle empty result
    if (IsEmpty (aplNELMRes))
        aplTypeRes = aplTypePro;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, 1);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

    // Lock the memory to get a ptr to it
    lpMemHdrRht = MyGlobalLock (hGlbRht);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->bSelSpec   = lpMemHdrRht->bSelSpec;
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

    // Save the dimension in the result
    *VarArrayBaseToDim (lpMemRes) = aplNELMRes;

    // Skip over the header and dimension to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, 1);

    // Copy the data from the right arg to the result
    if (!PrimFnMonEpsilonGlbCopy_EM (aplTypeRes,
                                    &lpMemRes,
                                    &uBitIndex,
                                     hGlbRht,
                                     lptkFunc))
        goto ERROR_EXIT;
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

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken);

    return lpYYRes;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
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

    return NULL;
} // End PrimFnMonEpsilonGlb_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonEpsilonGlbCount
//
//  Count the # simple scalars in the global memory object
//***************************************************************************

void PrimFnMonEpsilonGlbCount
    (HGLOBAL    hGlbRht,            // Right arg global memory handle
     LPAPLSTYPE lpaplTypeRes,       // Ptr to result storage type
     LPAPLNELM  lpaplNELMRes,       // Ptr to result NELM
     LPAPLSTYPE lpaplTypePro)                       // Ptr to prototype storage type

{
    LPVOID   lpMemRht;
    APLSTYPE aplTypeRht;
    APLNELM  aplNELMRht;
    APLRANK  aplRankRht;
    APLUINT  uRht;

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRht)
    // Get the Array Type, NELM, and Rank
    aplTypeRht = lpHeader->ArrType;
    aplNELMRht = lpHeader->NELM;
    aplRankRht = lpHeader->Rank;
#undef  lpHeader

    // Split cases based upon the right arg's storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_CHAR:
        case ARRAY_APA:
        case ARRAY_HETERO:
        case ARRAY_RAT:
        case ARRAY_VFP:
            // Empty args don't contribute to the result,
            //   so we don't want to risk changing the storage
            //   type to HETERO by running through aTypePromote.
            if (!IsEmpty (aplNELMRht))
            {
                *lpaplTypeRes = aTypePromote[*lpaplTypeRes][aplTypeRht];
                (*lpaplNELMRes) += aplNELMRht;
            } else
                *lpaplTypePro = aplTypeRht;
            break;

        case ARRAY_NESTED:
            // Skip over the header and dimensions to the data
            lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

            // Take prototypes into account
            aplNELMRht = max (aplNELMRht, 1);

            // Loop through the elements
            for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLNESTED) lpMemRht)++)
            // Split cases based upon the ptr type
            switch (GetPtrTypeInd (lpMemRht))
            {
                case PTRTYPE_STCONST:
                    *lpaplTypeRes = aTypePromote[*lpaplTypeRes][TranslateImmTypeToArrayType ((*(LPAPLHETERO) lpMemRht)->stFlags.ImmType)];
                    (*lpaplNELMRes)++;

                    break;

                case PTRTYPE_HGLOBAL:
                    // It's a valid HGLOBAL variable array
                    Assert (IsGlbTypeVarInd_PTB (lpMemRht));

                    PrimFnMonEpsilonGlbCount (ClrPtrTypeInd (lpMemRht), // Right arg global memory handle
                                              lpaplTypeRes,             // Ptr to result storage type
                                              lpaplNELMRes,             // Ptr to result NELM
                                              lpaplTypePro);            // Ptr to prototype storage type
                    break;

                defstop
                    break;
            } // End FOR/SWITCH

            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
} // End PrimFnMonEpsilonGlbCount


//***************************************************************************
//  $PrimFnMonEpsilonGlbCopy_EM
//
//  Copy the data from the right arg to the result
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonEpsilonGlbCopy_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnMonEpsilonGlbCopy_EM
    (APLSTYPE aplTypeRes,           // Result type
     LPVOID  *lplpMemRes,           // Ptr to ptr to result memory
     LPUINT   lpuBitIndex,          // Ptr to uBitIndex
     HGLOBAL  hGlbRht,              // Handle to right arg
     LPTOKEN  lptkFunc)             // Ptr to function token

{
    APLSTYPE      aplTypeRht;       // Right arg storage type
    APLNELM       aplNELMRht;       // Right arg NELM
    APLRANK       aplRankRht;       // Right arg rank
    LPVOID        lpMemRht;         // Ptr to right arg global memory
    UINT          uBitMask = BIT0;  // Bit mask for marching through Booleans
    APLUINT       uRht;             // Right arg loop counter
    APLUINT       ByteRes;          // # bytes in the result
    APLINT        apaOffRht,        // Right arg APA offset
                  apaMulRht;        // ...           multiplier
    APLLONGEST    aplVal;           // Temporary value
    UBOOL         bRet = FALSE;     // TRUE iff the result is valid
    LPSYMENTRY    lpSymTmp;         // Ptr to temporary LPSYMENTRY
    LPPLLOCALVARS lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;     // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRht)
    // Get the Array Type, NELM, and Rank
    aplTypeRht = lpHeader->ArrType;
    aplNELMRht = lpHeader->NELM;
    aplRankRht = lpHeader->Rank;
#undef  lpHeader

    // Skip past the header and dimensions to the data
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

    // If the item is non-empty, ...
    if (aplNELMRht)
    // Split cases based upon the result's storage type
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:                            // Res = BOOL  , Rht = BOOL/NESTED
            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:                    // Res = BOOL  , Rht = BOOL
                    // The result's and right arg's storage type are the same,
                    //   so just copy the data if it's aligned
                    if (*lpuBitIndex EQ 0)
                    {
                        // Calculate space needd for the result
                        ByteRes = sizeof (APLBOOL) * RoundUpBitsToBytes (aplNELMRht);

                        // Check for overflow
                        if (ByteRes NE (APLU3264) ByteRes)
                            goto WSFULL_EXIT;

                        CopyMemory (*lplpMemRes, lpMemRht, (APLU3264) ByteRes);

                        // Unless the right arg is a multiple of NBIB bits,
                        //   the following += is one too large
                        ((LPAPLBOOL) *lplpMemRes) += RoundUpBitsToBytes (aplNELMRht);

                        // Calculate the next bit index
                        *lpuBitIndex = (UINT) (aplNELMRht % NBIB);

                        // Account for non-NBIB # bits in right arg
                        if (*lpuBitIndex)
                            ((LPAPLBOOL) *lplpMemRes)--;
                    } else
                    {
                        // Loop through the elements
                        for (uRht = 0; uRht < aplNELMRht; uRht++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            *(*(LPAPLBOOL *) lplpMemRes) |= ((uBitMask & *((LPAPLBOOL) lpMemRht)) ? TRUE : FALSE) << *lpuBitIndex;

                            // Check for end-of-byte
                            if (++*lpuBitIndex EQ NBIB)
                            {
                                *lpuBitIndex = 0;               // Start over
                                (*(LPAPLBOOL *) lplpMemRes)++;  // Skip to next byte
                            } // End IF

                            // Shift over the bit mask
                            uBitMask <<= 1;

                            // Check for end-of-byte
                            if (uBitMask EQ END_OF_BYTE)
                            {
                                uBitMask = BIT0;                // Start over
                                ((LPAPLBOOL) lpMemRht)++;       // Skip to next byte
                            } // End IF
                        } // End FOR
                    } // End IF

                    break;

                case ARRAY_NESTED:                  // Res = BOOL  , Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLNESTED) lpMemRht)++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Split cases based upon the ptr type
                        switch (GetPtrTypeInd (lpMemRht))
                        {
                            case PTRTYPE_STCONST:       // Res = BOOL  , Rht = NESTED:BOOL/INT/FLOAT/CHAR
                                // Split cases based upon the right arg's immediate type
                                switch ((*(LPAPLHETERO) lpMemRht)->stFlags.ImmType)
                                {
                                    case IMMTYPE_BOOL:  // Res = BOOL  , Rht = NESTED:BOOL
                                        *(*(LPAPLBOOL *) lplpMemRes) |= ((*(LPAPLHETERO) lpMemRht)->stData.stBoolean) << *lpuBitIndex;

                                        // Check for end-of-byte
                                        if (++*lpuBitIndex EQ NBIB)
                                        {
                                            *lpuBitIndex = 0;               // Start over
                                            (*(LPAPLBOOL *) lplpMemRes)++;  // Skip to next byte
                                        } // End IF

                                        break;

                                    case IMMTYPE_INT:   // Res = BOOL  , Rht = NESTED:INT    (Can't happen w/Boolean result)
                                    case IMMTYPE_FLOAT: // Res = BOOL  , Rht = NESTED:FLOAT  (...)
                                    case IMMTYPE_CHAR:  // Res = BOOL  , Rht = NESTED:CHAR   (...)
                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            case PTRTYPE_HGLOBAL:       // Res = BOOL  , Rht = NESTED:NESTED
                                // It's a valid HGLOBAL variable array
                                Assert (IsGlbTypeVarInd_PTB (lpMemRht));

                                // Copy the data to the result
                                if (!PrimFnMonEpsilonGlbCopy_EM (aplTypeRes,
                                                                 lplpMemRes,
                                                                 lpuBitIndex,
                                                                 ClrPtrTypeInd (lpMemRht),
                                                                 lptkFunc))
                                    goto ERROR_EXIT;
                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End FOR

                    break;

                case ARRAY_INT:                     // Res = BOOL  , Rht = INT    (Can't happen w/Boolean result)
                case ARRAY_FLOAT:                   // Res = BOOL  , Rht = FLOAT  (...)
                case ARRAY_CHAR:                    // Res = BOOL  , Rht = CHAR   (...)
                case ARRAY_APA:                     // Res = BOOL  , Rht = APA    (...)
                case ARRAY_HETERO:                  // Res = BOOL  , Rht = HETERO (...)
                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_INT:                             // Res = INT   , Rht = BOOL/INT/APA/NESTED
            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:                    // Res = INT   , Rht = BOOL
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        *(*(LPAPLINT *) lplpMemRes)++ = (uBitMask & *((LPAPLBOOL) lpMemRht)) ? TRUE : FALSE;

                        // Shift over the bit mask
                        uBitMask <<= 1;

                        // Check for end-of-byte
                        if (uBitMask EQ END_OF_BYTE)
                        {
                            uBitMask = BIT0;                // Start over
                            ((LPAPLBOOL) lpMemRht)++;       // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_INT:                     // Res = INT   , Rht = INT
                    // The result's and right arg's storage type are the same,
                    //   so just copy the data
                    // Calculate space needed for the result
                    ByteRes = sizeof (APLINT) * aplNELMRht;

                    // Check for overflow
                    if (ByteRes NE (APLU3264) ByteRes)
                        goto WSFULL_EXIT;

                    CopyMemory (*lplpMemRes, lpMemRht, (APLU3264) ByteRes);
                    ((LPAPLINT) *lplpMemRes) += aplNELMRht;

                    break;

                case ARRAY_APA:                     // Res = INT   , Rht = APA
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        *(*(LPAPLINT *) lplpMemRes)++ = apaOffRht + apaMulRht * uRht;
                    } // End FOR

                    break;

                case ARRAY_NESTED:                  // Res = INT   , Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLNESTED) lpMemRht)++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Split cases based upon the ptr type
                        switch (GetPtrTypeInd (lpMemRht))
                        {
                            case PTRTYPE_STCONST:       // Res = INT   , Rht = NESTED:BOOL/INT/FLOAT/CHAR
                                // Split cases based upon the right arg's immediate type
                                switch ((*(LPAPLHETERO) lpMemRht)->stFlags.ImmType)
                                {
                                    case IMMTYPE_BOOL:  // Res = INT   , Rht = NESTED:BOOL
                                        *(*(LPAPLINT *) lplpMemRes)++ = (*(LPAPLHETERO) lpMemRht)->stData.stBoolean;

                                        break;

                                    case IMMTYPE_INT:   // Res = INT   , Rht = NESTED:INT
                                        *(*(LPAPLINT *) lplpMemRes)++ = (*(LPAPLHETERO) lpMemRht)->stData.stInteger;

                                        break;

                                    case IMMTYPE_FLOAT: // Res = INT   , Rht = NESTED:FLOAT  (Can't happen w/Integer result)
                                    case IMMTYPE_CHAR:  // Res = INT   , Rht = NESTED:CHAR   (...)
                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            case PTRTYPE_HGLOBAL:       // Res = INT   , Rht = NESTED:NESTED
                                // It's a valid HGLOBAL variable array
                                Assert (IsGlbTypeVarInd_PTB (lpMemRht));

                                // Copy the data to the result
                                if (!PrimFnMonEpsilonGlbCopy_EM (aplTypeRes,
                                                                 lplpMemRes,
                                                                 lpuBitIndex,
                                                                 ClrPtrTypeInd (lpMemRht),
                                                                 lptkFunc))
                                    goto ERROR_EXIT;
                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End FOR

                    break;

                case ARRAY_FLOAT:                   // Res = INT   , Rht = FLOAT  (Can't happen w/Integer result)
                case ARRAY_CHAR:                    // Res = INT   , Rht = CHAR   (...)
                case ARRAY_HETERO:                  // Res = INT   , Rht = HETERO (...)
                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_FLOAT:                           // Res = FLOAT , Rht = BOOL/INT/FLOAT/APA/NESTED
            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:                    // Res = FLOAT , Rht = BOOL
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        *(*(LPAPLFLOAT *) lplpMemRes)++ = (uBitMask & *((LPAPLBOOL) lpMemRht)) ? TRUE : FALSE;

                        // Shift over the bit mask
                        uBitMask <<= 1;

                        // Check for end-of-byte
                        if (uBitMask EQ END_OF_BYTE)
                        {
                            uBitMask = BIT0;                // Start over
                            ((LPAPLBOOL) lpMemRht)++;       // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_INT:                     // Res = FLOAT , Rht = INT
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        *(*(LPAPLFLOAT *) lplpMemRes)++ = (APLFLOAT) *((LPAPLINT) lpMemRht)++;
                    } // End FOR

                    break;

                case ARRAY_FLOAT:                   // Res = FLOAT , Rht = FLOAT
                    // The result's and right arg's storage type are the same,
                    //   so just copy the data
                    // Calculate space needed for the result
                    ByteRes = sizeof (APLFLOAT) * aplNELMRht;

                    // Check for overflow
                    if (ByteRes NE (APLU3264) ByteRes)
                        goto WSFULL_EXIT;

                    CopyMemory (*lplpMemRes, lpMemRht, (APLU3264) ByteRes);
                    ((LPAPLFLOAT) *lplpMemRes) += aplNELMRht;

                    break;

                case ARRAY_APA:                     // Res = FLOAT , Rht = APA
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        *(*(LPAPLFLOAT *) lplpMemRes)++ = (APLFLOAT) (APLINT) (apaOffRht + apaMulRht * uRht);
                    } // End FOR

                    break;

                case ARRAY_NESTED:                  // Res = FLOAT , Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLNESTED) lpMemRht)++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Split cases based upon the ptr type
                        switch (GetPtrTypeInd (lpMemRht))
                        {
                            case PTRTYPE_STCONST:       // Res = FLOAT , Rht = NESTED:BOOL/INT/FLOAT
                                // Split cases based upon the right arg's immediate type
                                switch ((*(LPAPLHETERO) lpMemRht)->stFlags.ImmType)
                                {
                                    case IMMTYPE_BOOL:  // Res = FLOAT , Rht = NESTED:BOOL
                                        *(*(LPAPLFLOAT *) lplpMemRes)++ = (*(LPAPLHETERO) lpMemRht)->stData.stBoolean;

                                        break;

                                    case IMMTYPE_INT:   // Res = FLOAT , Rht = NESTED:INT
                                        *(*(LPAPLFLOAT *) lplpMemRes)++ = (APLFLOAT) (*(LPAPLHETERO) lpMemRht)->stData.stInteger;

                                        break;

                                    case IMMTYPE_FLOAT: // Res = FLOAT , Rht = NESTED:FLOAT
                                        *(*(LPAPLFLOAT *) lplpMemRes)++ = (*(LPAPLHETERO) lpMemRht)->stData.stFloat;

                                        break;

                                    case IMMTYPE_CHAR:  // Res = FLOAT , Rht = NESTED:CHAR   (Can't happen w/Float result)
                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            case PTRTYPE_HGLOBAL:       // Res = FLOAT , Rht = NESTED:NESTED
                                // It's a valid HGLOBAL variable array
                                Assert (IsGlbTypeVarInd_PTB (lpMemRht));

                                // Copy the data to the result
                                if (!PrimFnMonEpsilonGlbCopy_EM (aplTypeRes,
                                                                 lplpMemRes,
                                                                 lpuBitIndex,
                                                                 ClrPtrTypeInd (lpMemRht),
                                                                 lptkFunc))
                                    goto ERROR_EXIT;
                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End FOR

                    break;

                case ARRAY_CHAR:                    // Res = FLOAT , Rht = CHAR   (Can't happen w/Float result)
                case ARRAY_HETERO:                  // Res = FLOAT , Rht = HETERO (...)
                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_CHAR:                            // Res = CHAR  , Rht = CHAR/NESTED
            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_CHAR:                    // Res = CHAR  , Rht = CHAR
                    // The result's and right arg's storage type are the same,
                    //   so just copy the data
                    // Calculate space needed for the result
                    ByteRes = sizeof (APLCHAR) * aplNELMRht;

                    // Check for overflow
                    if (ByteRes NE (APLU3264) ByteRes)
                        goto WSFULL_EXIT;

                    CopyMemory (*lplpMemRes, lpMemRht, (APLU3264) ByteRes);
                    ((LPAPLCHAR) *lplpMemRes) += aplNELMRht;

                    break;

                case ARRAY_NESTED:                  // Res = CHAR  , Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLNESTED) lpMemRht)++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Split cases based upon the ptr type
                        switch (GetPtrTypeInd (lpMemRht))
                        {
                            case PTRTYPE_STCONST:       // Res = CHAR  , Rht = NESTED:CHAR
                                // Split cases based upon the right arg's immediate type
                                switch ((*(LPAPLHETERO) lpMemRht)->stFlags.ImmType)
                                {
                                    case IMMTYPE_CHAR:  // Res = CHAR  , Rht = NESTED:CHAR
                                        *(*(LPAPLCHAR *) lplpMemRes)++ = (*(LPAPLHETERO) lpMemRht)->stData.stChar;

                                        break;

                                    case IMMTYPE_BOOL:  // Res = CHAR  , Rht = NESTED:BOOL    (Can't happen w/Char result)
                                    case IMMTYPE_INT:   // Res = CHAR  , Rht = NESTED:INT     (...)
                                    case IMMTYPE_FLOAT: // Res = CHAR  , Rht = NESTED:FLOAT   (...)
                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            case PTRTYPE_HGLOBAL:       // Res = CHAR  , Rht = NESTED:NESTED
                                // It's a valid HGLOBAL variable array
                                Assert (IsGlbTypeVarInd_PTB (lpMemRht));

                                // Copy the data to the result
                                if (!PrimFnMonEpsilonGlbCopy_EM (aplTypeRes,
                                                                 lplpMemRes,
                                                                 lpuBitIndex,
                                                                 ClrPtrTypeInd (lpMemRht),
                                                                 lptkFunc))
                                    goto ERROR_EXIT;
                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End FOR

                    break;

                case ARRAY_BOOL:                    // Res = CHAR  , Rht = BOOL   (Can't happen w/Char result)
                case ARRAY_INT:                     // Res = CHAR  , Rht = INT    (...)
                case ARRAY_APA:                     // Res = CHAR  , Rht = APA    (...)
                case ARRAY_HETERO:                  // Res = CHAR  , Rht = HETERO (...)
                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_HETERO:                          // Res = HETERO, Rht = BOOL/INT/FLOAT/APA/CHAR/HETERO/NESTED/RAT/VFP
            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:                    // Res = HETERO, Rht = BOOL
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        aplVal = (uBitMask & *((LPAPLBOOL) lpMemRht)) ? TRUE : FALSE;
                        *(*(LPAPLHETERO *) lplpMemRes)++ =
                        lpSymTmp =
                          MakeSymEntry_EM (IMMTYPE_BOOL,            // Immediate type
                                          &aplVal,                  // Ptr to immediate value
                                           lptkFunc);               // Ptr to function token
                        if (!lpSymTmp)
                            goto ERROR_EXIT;
                        // Shift over the bit mask
                        uBitMask <<= 1;

                        // Check for end-of-byte
                        if (uBitMask EQ END_OF_BYTE)
                        {
                            uBitMask = BIT0;         // Start over
                            ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_INT:                     // Res = HETERO, Rht = INT
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        aplVal = *((LPAPLINT) lpMemRht)++;
                        *(*(LPAPLHETERO *) lplpMemRes)++ =
                        lpSymTmp =
                          MakeSymEntry_EM (IMMTYPE_INT,             // Immediate type
                                          &aplVal,                  // Ptr to immediate value
                                           lptkFunc);               // Ptr to function token
                        if (!lpSymTmp)
                            goto ERROR_EXIT;
                    } // End FOR

                    break;

                case ARRAY_FLOAT:                   // Res = HETERO, Rht = FLOAT
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        aplVal = *(LPAPLLONGEST) ((LPAPLFLOAT) lpMemRht)++;
                        *(*(LPAPLHETERO *) lplpMemRes)++ =
                        lpSymTmp =
                          MakeSymEntry_EM (IMMTYPE_FLOAT,           // Immediate type
                                          &aplVal,                  // Ptr to immediate value
                                           lptkFunc);               // Ptr to function token
                        if (!lpSymTmp)
                            goto ERROR_EXIT;
                    } // End FOR

                    break;

                case ARRAY_APA:                     // Res = HETERO, Rht = APA
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        aplVal = apaOffRht + apaMulRht * uRht;
                        *(*(LPAPLHETERO *) lplpMemRes)++ =
                        lpSymTmp =
                          MakeSymEntry_EM (IMMTYPE_INT,             // Immediate type
                                          &aplVal,                  // Ptr to immediate value
                                           lptkFunc);               // Ptr to function token
                        if (!lpSymTmp)
                            goto ERROR_EXIT;
                    } // End FOR

                    break;

                case ARRAY_CHAR:                    // Res = HETERO, Rht = CHAR
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        aplVal = *((LPAPLCHAR) lpMemRht)++;
                        *(*(LPAPLHETERO *) lplpMemRes)++ =
                        lpSymTmp =
                          MakeSymEntry_EM (IMMTYPE_CHAR,            // Immediate type
                                          &aplVal,                  // Ptr to immediate value
                                           lptkFunc);               // Ptr to function token
                        if (!lpSymTmp)
                            goto ERROR_EXIT;
                    } // End FOR

                    break;

                case ARRAY_HETERO:                  // Res = HETERO, Rht = HETERO
                    // The result's and right arg's storage type are the same,
                    //   so just copy the data
                    // Calculate space needed for the result
                    ByteRes = sizeof (APLHETERO) * aplNELMRht;

                    // Check for overflow
                    if (ByteRes NE (APLU3264) ByteRes)
                        goto WSFULL_EXIT;

                    CopyMemory (*lplpMemRes, lpMemRht, (APLU3264) ByteRes);
                    ((LPAPLHETERO) *lplpMemRes) += aplNELMRht;

                    break;

                case ARRAY_NESTED:                  // Res = HETERO, Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLNESTED) lpMemRht)++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Split cases based upon the ptr type
                        switch (GetPtrTypeInd (lpMemRht))
                        {
                            case PTRTYPE_STCONST:       // Res = HETERO, Rht = NESTED:BOOL/INT/FLOAT/CHAR
                                // Split cases based upon the right arg's immediate type
                                switch ((*(LPAPLHETERO) lpMemRht)->stFlags.ImmType)
                                {
                                    case IMMTYPE_BOOL:  // Res = HETERO, Rht = NESTED:BOOL
                                        aplVal = (*(LPAPLHETERO) lpMemRht)->stData.stBoolean;
                                        *(*(LPAPLNESTED *) lplpMemRes)++ =
                                        lpSymTmp =
                                          MakeSymEntry_EM (IMMTYPE_BOOL,    // Immediate type
                                                          &aplVal,          // Ptr to immediate value
                                                           lptkFunc);       // Ptr to function token
                                        if (!lpSymTmp)
                                            goto ERROR_EXIT;
                                        break;

                                    case IMMTYPE_INT:   // Res = HETERO, Rht = NESTED:INT
                                        aplVal = (*(LPAPLHETERO) lpMemRht)->stData.stInteger;
                                        *(*(LPAPLNESTED *) lplpMemRes)++ =
                                        lpSymTmp =
                                          MakeSymEntry_EM (IMMTYPE_INT,     // Immediate type
                                                          &aplVal,          // Ptr to immediate value
                                                           lptkFunc);       // Ptr to function token
                                        if (!lpSymTmp)
                                            goto ERROR_EXIT;
                                        break;

                                    case IMMTYPE_FLOAT: // Res = HETERO, Rht = NESTED:FLOAT
                                        aplVal = *(LPAPLLONGEST) &(*(LPAPLHETERO) lpMemRht)->stData.stFloat;
                                        *(*(LPAPLNESTED *) lplpMemRes)++ =
                                        lpSymTmp =
                                          MakeSymEntry_EM (IMMTYPE_FLOAT,   // Immediate type
                                                          &aplVal,          // Ptr to immediate value
                                                           lptkFunc);       // Ptr to function token
                                        if (!lpSymTmp)
                                            goto ERROR_EXIT;
                                        break;


                                    case IMMTYPE_CHAR:  // Res = HETERO, Rht = NESTED:CHAR
                                        aplVal = (*(LPAPLHETERO) lpMemRht)->stData.stChar;
                                        *(*(LPAPLNESTED *) lplpMemRes)++ =
                                        lpSymTmp =
                                          MakeSymEntry_EM (IMMTYPE_CHAR,    // Immediate type
                                                          &aplVal,          // Ptr to immediate value
                                                          lptkFunc);        // Ptr to function token
                                        if (!lpSymTmp)
                                            goto ERROR_EXIT;
                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            case PTRTYPE_HGLOBAL:       // Res = HETERO, Rht = NESTED:NESTED
                                // It's a valid HGLOBAL variable array
                                Assert (IsGlbTypeVarInd_PTB (lpMemRht));

                                // Copy the data to the result
                                if (!PrimFnMonEpsilonGlbCopy_EM (aplTypeRes,
                                                                 lplpMemRes,
                                                                 lpuBitIndex,
                                                                 ClrPtrTypeInd (lpMemRht),
                                                                 lptkFunc))
                                    goto ERROR_EXIT;
                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End FOR

                    break;

                case ARRAY_RAT:                     // Res = HETERO, Rht = RAT
                case ARRAY_VFP:                     // Res = HETERO, Rht = VFP
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLRAT) lpMemRht)++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        *(*(LPAPLHETERO *) lplpMemRes)++ =
                        lpSymTmp =
                          MakeGlbEntry_EM (aplTypeRht,              // Entry type
                                           lpMemRht,                // Ptr to the value
                                           TRUE,                    // TRUE iff we should initialize the target first
                                           lptkFunc);               // Ptr to function token
                        if (!lpSymTmp)
                            goto ERROR_EXIT;
                    } // End FOR

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_RAT:                             // Res = RAT   , Rht = BOOL/INT/APA/NESTED/RAT
            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:                    // Res = RAT   , Rht = BOOL
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        mpq_init_set_sx ((*(LPAPLRAT *) lplpMemRes)++, (uBitMask & *((LPAPLBOOL) lpMemRht)) ? TRUE : FALSE, 1);

                        // Shift over the bit mask
                        uBitMask <<= 1;

                        // Check for end-of-byte
                        if (uBitMask EQ END_OF_BYTE)
                        {
                            uBitMask = BIT0;                // Start over
                            ((LPAPLBOOL) lpMemRht)++;       // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_INT:                     // Res = RAT   , Rht = INT
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        mpq_init_set_sx ((*(LPAPLRAT *) lplpMemRes)++, *((LPAPLINT) lpMemRht)++, 1);
                    } // End FOR

                    break;

                case ARRAY_APA:                     // Res = RAT   , Rht = APA
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        mpq_init_set_sx ((*(LPAPLRAT *) lplpMemRes)++, apaOffRht + apaMulRht * uRht, 1);
                    } // End FOR

                    break;

                case ARRAY_NESTED:                  // Res = RAT   , Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLNESTED) lpMemRht)++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Split cases based upon the ptr type
                        switch (GetPtrTypeInd (lpMemRht))
                        {
                            case PTRTYPE_STCONST:       // Res = RAT   , Rht = NESTED:BOOL/INT
                                // Split cases based upon the right arg's immediate type
                                switch ((*(LPAPLHETERO) lpMemRht)->stFlags.ImmType)
                                {
                                    case IMMTYPE_BOOL:  // Res = RAT   , Rht = NESTED:BOOL
                                        mpq_init_set_sx ((*(LPAPLRAT *) lplpMemRes)++, (*(LPAPLHETERO) lpMemRht)->stData.stBoolean, 1);

                                        break;

                                    case IMMTYPE_INT:   // Res = RAT   , Rht = NESTED:INT
                                        mpq_init_set_sx ((*(LPAPLRAT *) lplpMemRes)++, (*(LPAPLHETERO) lpMemRht)->stData.stInteger, 1);

                                        break;

                                    case IMMTYPE_FLOAT: // Res = RAT   , Rht = NESTED:FLOAT  (Can't happen w/Rational result)
                                    case IMMTYPE_CHAR:  // Res = RAT   , Rht = NESTED:CHAR   (...)
                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            case PTRTYPE_HGLOBAL:       // Res = RAT   , Rht = NESTED:NESTED
                                // It's a valid HGLOBAL variable array
                                Assert (IsGlbTypeVarInd_PTB (lpMemRht));

                                // Copy the data to the result
                                if (!PrimFnMonEpsilonGlbCopy_EM (aplTypeRes,
                                                                 lplpMemRes,
                                                                 lpuBitIndex,
                                                                 ClrPtrTypeInd (lpMemRht),
                                                                 lptkFunc))
                                    goto ERROR_EXIT;
                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End FOR

                    break;

                case ARRAY_RAT:                     // Res = RAT   , Rht = RAT
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        mpq_init_set    ((*(LPAPLRAT *) lplpMemRes)++, ((LPAPLRAT) lpMemRht)++);
                    } // End FOR

                    break;

                case ARRAY_FLOAT:                   // Res = RAT   , Rht = FLOAT  (Can't happen w/Rational result)
                case ARRAY_CHAR:                    // Res = RAT   , Rht = CHAR   (...)
                case ARRAY_HETERO:                  // Res = RAT   , Rht = HETERO (...)
                case ARRAY_VFP:                     // Res = RAT   , Rht = VFP    (...)
                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_VFP:                             // Res = VFP   , Rht = BOOL/INT/FLOAT/APA/NESTED/RAT/VFP
            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:                    // Res = VFP   , Rht = BOOL
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        mpfr_init_set_sx ((*(LPAPLVFP *) lplpMemRes)++, (uBitMask & *((LPAPLBOOL) lpMemRht)) ? TRUE : FALSE, MPFR_RNDN);

                        // Shift over the bit mask
                        uBitMask <<= 1;

                        // Check for end-of-byte
                        if (uBitMask EQ END_OF_BYTE)
                        {
                            uBitMask = BIT0;                // Start over
                            ((LPAPLBOOL) lpMemRht)++;       // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_INT:                     // Res = VFP   , Rht = INT
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        mpfr_init_set_sx ((*(LPAPLVFP *) lplpMemRes)++, *((LPAPLINT) lpMemRht)++, MPFR_RNDN);
                    } // End FOR

                    break;

                case ARRAY_FLOAT:                   // Res = VFP   , Rht = FLOAT
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        mpfr_init_set_d  ((*(LPAPLVFP *) lplpMemRes)++, *((LPAPLFLOAT) lpMemRht)++, MPFR_RNDN);
                    } // End FOR

                    break;

                case ARRAY_APA:                     // Res = VFP   , Rht = APA
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        mpfr_init_set_sx ((*(LPAPLVFP *) lplpMemRes)++, apaOffRht + apaMulRht * uRht, MPFR_RNDN);
                    } // End FOR

                    break;

                case ARRAY_NESTED:                  // Res = VFP   , Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLNESTED) lpMemRht)++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Split cases based upon the ptr type
                        switch (GetPtrTypeInd (lpMemRht))
                        {
                            case PTRTYPE_STCONST:       // Res = VFP   , Rht = NESTED:BOOL/INT/FLOAT
                                // Split cases based upon the right arg's immediate type
                                switch ((*(LPAPLHETERO) lpMemRht)->stFlags.ImmType)
                                {
                                    case IMMTYPE_BOOL:  // Res = VFP   , Rht = NESTED:BOOL
                                        mpfr_init_set_sx ((*(LPAPLVFP *) lplpMemRes)++, (*(LPAPLHETERO) lpMemRht)->stData.stBoolean, MPFR_RNDN);

                                        break;

                                    case IMMTYPE_INT:   // Res = VFP   , Rht = NESTED:INT
                                        mpfr_init_set_sx ((*(LPAPLVFP *) lplpMemRes)++, (*(LPAPLHETERO) lpMemRht)->stData.stInteger, MPFR_RNDN);

                                        break;

                                    case IMMTYPE_FLOAT: // Res = VFP   , Rht = NESTED:FLOAT
                                        mpfr_init_set_d  ((*(LPAPLVFP *) lplpMemRes)++, (*(LPAPLHETERO) lpMemRht)->stData.stFloat  , MPFR_RNDN);

                                        break;

                                    case IMMTYPE_CHAR:  // Res = VFP   , Rht = NESTED:CHAR   (Can't happen w/VFP result)
                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            case PTRTYPE_HGLOBAL:       // Res = VFP   , Rht = NESTED:NESTED
                                // It's a valid HGLOBAL variable array
                                Assert (IsGlbTypeVarInd_PTB (lpMemRht));

                                // Copy the data to the result
                                if (!PrimFnMonEpsilonGlbCopy_EM (aplTypeRes,
                                                                 lplpMemRes,
                                                                 lpuBitIndex,
                                                                 ClrPtrTypeInd (lpMemRht),
                                                                 lptkFunc))
                                    goto ERROR_EXIT;
                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End FOR

                    break;

                case ARRAY_RAT:                     // Res = VFP   , Rht = RAT
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        mpfr_init_set_q  ((*(LPAPLVFP *) lplpMemRes)++, ((LPAPLRAT) lpMemRht)++, MPFR_RNDN);
                    } // End FOR

                    break;

                case ARRAY_VFP:                     // Res = VFP   , Rht = VFP
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        mpfr_init_copy   ((*(LPAPLVFP *) lplpMemRes)++, ((LPAPLVFP) lpMemRht)++);
                    } // End FOR

                    break;

                case ARRAY_CHAR:                    // Res = VFP   , Rht = CHAR   (Can't happen w/VFP result)
                case ARRAY_HETERO:                  // Res = VFP   , Rht = HETERO (...)
                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_APA:                             // Res = APA    (Can't happen w/Enlist)
        case ARRAY_NESTED:                          // Res = NESTED (...)
        defstop
            break;
    } // End SWITCH

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbRht); lpMemRht = NULL;

    return bRet;
} // End PrimFnMonEpsilonGlbCopy_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydEpsilon_EM_YY
//
//  Primitive function for dyadic Epsilon ("member of")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydEpsilon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydEpsilon_EM_YY
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
    LPVARARRAY_HEADER lpHeaderRht;  // Ptr to right arg header
    LPVOID            lpMemLft = NULL,  // Ptr to left arg global memory
                      lpMemRht = NULL;  // Ptr to right ...
    LPAPLBOOL         lpMemRes = NULL;  // Ptr to result   ...
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

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Get left and right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    //***************************************************************
    // Calculate space needed for the result
    //***************************************************************
    ByteRes = CalcArraySize (ARRAY_BOOL, aplNELMLft, aplRankLft);

    //***************************************************************
    // Check for overflow
    //***************************************************************
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

    // Save ptr to right arg header
    lpHeaderRht = lpMemRht;

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_BOOL;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMLft;
    lpHeader->Rank       = aplRankLft;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = (LPAPLBOOL) VarArrayBaseToDim (lpMemRes);

    // Fill in the result's dimension
    if (lpMemLft)
    {
        // Skip over the header to the dimensions
        lpMemLft = VarArrayBaseToDim (lpMemLft);

        // Copy the left arg dimensions to the result
        CopyMemory (lpMemRes, lpMemLft, (APLU3264) aplRankLft * sizeof (APLDIM));

        // Skip over the dimensions to the data
        lpMemRes = VarArrayDimToData (lpMemRes, aplRankLft);
        lpMemLft = VarArrayDimToData (lpMemLft, aplRankLft);
    } else
        // Point to the left arg immediate value
        lpMemLft = &aplLongestLft;

    if (lpMemRht)
        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);
    else
        // Point to the right arg immediate value
        lpMemRht = &aplLongestRht;

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
            if (!PrimFnDydEpsilonBvB (lpMemRes,         // Ptr to result global memory data
                                      aplNELMLft,       // Left arg NELM
                                      lpMemLft,         // Ptr to left arg global memory data
                                      aplNELMRht,       // Right arg NELM
                                      lpMemRht,         // Ptr to right arg global memory data
                                      lpbCtrlBreak))    // Ptr to Ctrl-Break flag
                goto ERROR_EXIT;
        } else
        if (IsNumeric (aplTypeLft) && IsSimpleBool (aplTypeRht))
        {
            // Handle APLINT/APLAPA/APLFLOAT/APLRAT/APLVFP vs. APLBOOL
            if (!PrimFnDydEpsilonNvB (lpMemRes,         // Ptr to result global memory data
                                      aplTypeLft,       // Left arg storage type
                                      aplNELMLft,       // Left arg NELM
                                      lpMemLft,         // Ptr to left arg global memory data
                                      aplNELMRht,       // Right arg NELM
                                      lpMemRht,         // Ptr to right arg global memory data
                                      lpbCtrlBreak))    // Ptr to Ctrl-Break flag
                goto ERROR_EXIT;
        } else
        if (IsNumeric (aplTypeLft) && IsPermVector (lpHeaderRht))
        {
            // Handle APLBOOL/APLINT/APLAPA/APLFLOAT vs. PV
            if (!PrimFnDydEpsilonNvP_EM (lpMemRes,      // Ptr to result global memory data
                                         aplTypeLft,    // Left arg storage type
                                         aplNELMLft,    // Left arg NELM
                                         lpMemLft,      // Ptr to left arg global memory data
                                         lpHeaderRht,   // Ptr to right arg header
                                         aplTypeRht,    // Right arg storage type
                                         aplNELMRht,    // Right arg NELM
                                         lpbCtrlBreak,  // Ptr to Ctrl-Break flag
                                         lptkFunc))     // Ptr to function token
                goto ERROR_EXIT;
        } else
        if (IsNumeric (aplTypeLft) && IsSimpleAPA (aplTypeRht))
        {
            // Handle APLBOOL/APLINT/APLAPA/APLFLOAT vs. APLAPA
            if (!PrimFnDydEpsilonNvA_EM (lpMemRes,      // Ptr to result global memory data
                                         aplTypeLft,    // Left arg storage type
                                         aplNELMLft,    // Left arg NELM
                                         lpMemLft,      // Ptr to left arg global memory data
                                         aplNELMRht,    // Right arg NELM
                                         lpMemRht,      // Ptr to right arg global memory data
                                         lpbCtrlBreak,  // Ptr to Ctrl-Break flag
                                         lptkFunc))     // Ptr to function token
                goto ERROR_EXIT;
        } else
        if (IsSimpleInt (aplTypeLft) && IsSimpleInt (aplTypeRht))
        {
            // Handle APLBOOL/APLINT/APLAPA vs. APLINT
            if (!PrimFnDydEpsilonIvI_EM (lpMemRes,      // Ptr to result global memory data
                                         lptkLftArg,    // Ptr to left arg token
                                         aplTypeLft,    // Left arg storage type
                                         aplNELMLft,    // Left arg NELM
                                         aplRankLft,    // Left arg rank
                                         lpMemLft,      // Ptr to left arg global memory data
                                         lptkRhtArg,    // Ptr to right arg token
                                         aplTypeRht,    // Right arg storage type
                                         aplNELMRht,    // Right arg NELM
                                         aplRankRht,    // Right arg rank
                                         lpMemRht,      // Ptr to right arg global memory data
                                         lpbCtrlBreak,  // Ptr to Ctrl-Break flag
                                         lptkFunc))     // Ptr to function token
                goto ERROR_EXIT;
        } else
        if (IsNumeric (aplTypeLft) && IsSimpleNum (aplTypeRht))
        {
            // Handle APLBOOL/APLINT/APLAPA/APLFLOAT/APLRAT/APLVFP vs. APLFLOAT
            // Handle APLFLOAT vs. APLINT
            if (!PrimFnDydEpsilonNvN_EM (lpMemRes,      // Ptr to result global memory data
                                         lptkLftArg,    // Ptr to left arg token
                                         aplTypeLft,    // Left arg storage type
                                         aplNELMLft,    // Left arg NELM
                                         aplRankLft,    // Left arg rank
                                         lpMemLft,      // Ptr to left arg global memory data
                                         lptkRhtArg,    // Ptr to right arg token
                                         aplTypeRht,    // Right arg storage type
                                         aplNELMRht,    // Right arg NELM
                                         aplRankRht,    // Right arg rank
                                         lpMemRht,      // Ptr to right arg global memory data
                                         lpbCtrlBreak,  // Ptr to Ctrl-Break flag
                                         lptkFunc))     // Ptr to function token
                goto ERROR_EXIT;
        } else
        if (IsNumeric (aplTypeLft) && IsRat (aplTypeRht))
        {
            // Handle APLBOOL/APLINT/APLAPA/APLFLOAT/APLRAT/APLVFP vs. APLRAT
            if (!PrimFnDydEpsilonNvR_EM (lpMemRes,      // Ptr to result global memory data
                                         lptkLftArg,    // Ptr to left arg token
                                         aplTypeLft,    // Left arg storage type
                                         aplNELMLft,    // Left arg NELM
                                         aplRankLft,    // Left arg rank
                                         lpMemLft,      // Ptr to left arg global memory data
                                         lptkRhtArg,    // Ptr to right arg token
                                         aplTypeRht,    // Right arg storage type
                                         aplNELMRht,    // Right arg NELM
                                         aplRankRht,    // Right arg rank
                                         lpMemRht,      // Ptr to right arg global memory data
                                         lpbCtrlBreak,  // Ptr to Ctrl-Break flag
                                         lptkFunc))     // Ptr to function token
                goto ERROR_EXIT;
        } else
        if (IsNumeric (aplTypeLft) && IsVfp (aplTypeRht))
        {
            // Handle APLBOOL/APLINT/APLAPA/APLFLOAT/APLRAT/APLVFP vs. APLVFP
            if (!PrimFnDydEpsilonNvV_EM (lpMemRes,      // Ptr to result global memory data
                                         lptkLftArg,    // Ptr to left arg token
                                         aplTypeLft,    // Left arg storage type
                                         aplNELMLft,    // Left arg NELM
                                         aplRankLft,    // Left arg rank
                                         lpMemLft,      // Ptr to left arg global memory data
                                         lptkRhtArg,    // Ptr to right arg token
                                         aplTypeRht,    // Right arg storage type
                                         aplNELMRht,    // Right arg NELM
                                         aplRankRht,    // Right arg rank
                                         lpMemRht,      // Ptr to right arg global memory data
                                         lpbCtrlBreak,  // Ptr to Ctrl-Break flag
                                         lptkFunc))     // Ptr to function token
                goto ERROR_EXIT;
        } else
        if (IsSimpleChar (aplTypeLft) && IsSimpleChar (aplTypeRht))
        {
            // Handle APLCHAR vs. APLCHAR
            if (!PrimFnDydEpsilonCvC_EM (lpMemRes,      // Ptr to result global memory data
                                         aplNELMLft,    // Left arg NELM
                                         lpMemLft,      // Ptr to left arg global memory data
                                         aplNELMRht,    // Right arg NELM
                                         lpMemRht,      // Ptr to right arg global memory data
                                         lpbCtrlBreak,  // Ptr to Ctrl-Break flag
                                         lptkFunc))     // Ptr to function token
                goto ERROR_EXIT;
        } else
        {
            // Handle all other combinations
            // APLHETERO/APLNESTED vs. anything
            // anything            vs. APLHETERO/APLNESTED
            if (!PrimFnDydEpsilonOther (lpMemRes,       // Ptr to result global memory data
                                        aplTypeLft,     // Left arg storage type
                                        aplNELMLft,     // Left arg NELM
                                        lpMemLft,       // Ptr to left arg global memory data
                                        aplTypeRht,     // Right arg storage type
                                        aplNELMRht,     // Right arg NELM
                                        lpMemRht,       // Ptr to right arg global memory data
                                        lpbCtrlBreak,   // Ptr to Ctrl-Break flag
                                        lptkFunc))      // Ptr to function token
                goto ERROR_EXIT;
        } // End IF/ELSE/...
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

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken);

    goto NORMAL_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
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
} // End PrimFnDydEpsilon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydEpsilonBvB
//
//  Dyadic epsilon of APLBOOL vs. APLBOOL
//***************************************************************************

UBOOL PrimFnDydEpsilonBvB
    (LPAPLBOOL lpMemRes,            // Ptr to result global memory data
     APLNELM   aplNELMLft,          // Left arg (and result) NELM
     LPAPLBOOL lpMemLft,            // Ptr to left arg global memory data
     APLNELM   aplNELMRht,          // Right arg NELM
     LPAPLBOOL lpMemRht,            // Ptr to right arg global memory data
     LPUBOOL   lpbCtrlBreak)        // Ptr to Ctrl-Break flag

{
    UBOOL   Found[2];               // TRUE iff there is at least one [0,1] in the right arg
    APLUINT BytesInLftBits,         // # bytes in the left arg data
            BytesInRhtBits,         // ...            right ...
            uBit,                   // The bit we found or are searching for
            uValid,                 // # valid bits in the arg
            uLft,                   // Loop counter
            uRht;                   // Loop counter

    // The result is either all 0s              (if right arg is empty -- already ruled out), or
    //                      same as Lft         (if right arg is all 1s), or
    //                      complement of Lft   (if right arg is all 0s), or
    //                      all 1s              (if right arg has at least one 0 and one 1)

    // Calculate the # bytes in the left arg (result) data
    BytesInLftBits = RoundUpBitsToBytes (aplNELMLft);

    // Calculate the # bytes in the right arg data
    BytesInRhtBits = RoundUpBitsToBytes (aplNELMRht);

    // Get the first bit from the right arg
    uBit = BIT0 & *(LPAPLBOOL) lpMemRht;

    // Save that information
    Found[uBit] = TRUE;

    // Complement the value to get the bit we're searching for
    uBit = !uBit;

    // Search the right arg for uBit
    for (Found[uBit] = FALSE, uRht = 0; uRht < (BytesInRhtBits - 1); uRht++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        if (FastBoolTrans[lpMemRht[uRht]][fbtFirst[uBit]] < NBIB)
        {
            Found[uBit] = TRUE;

            break;
        } // End IF
    } // End FOR

    // If we didn't find uBit, check the last byte (may be short)
    if (!Found[uBit])
    {
        // Calculate the # valid bits in the last byte in the right arg
        uValid = (MASKLOG2NBIB & (aplNELMRht - 1)) + 1;

        // Handle the last byte specially
        Found[uBit] = (FastBoolTrans[lpMemRht[uRht] & ((BIT0 << uValid) - 1)][fbtFirst[uBit]] < uValid);
    } // End IF

    // If we found both a 0 and a 1, the result is all 1s
    if (Found[0] && Found[1])
        FillBitMemory (lpMemRes, aplNELMLft);
    else
    // If we found only 0s, the result is the complement of the left arg
    if (Found[0])
    {
        // Copy all but the last byte
        for (uLft = 0; uLft < (BytesInLftBits - 1); uLft++)
        {
            // Check for Ctrl-Break
            if (CheckCtrlBreak (*lpbCtrlBreak))
                goto ERROR_EXIT;

            *lpMemRes++ = ~*lpMemLft++;
        } // End FOR

        // Calculate the # valid bits in the last byte in the left arg
        uValid = (MASKLOG2NBIB & (aplNELMLft - 1)) + 1;

        // Handle the last byte specially
        *lpMemRes |= ((BIT0 << uValid) - 1) & ~*lpMemLft;
    } else
    // If we found only 1s, the result is the same as the left arg
    if (Found[1])
        CopyMemory (lpMemRes, lpMemLft, (APLU3264) BytesInLftBits);

    return TRUE;
ERROR_EXIT:
    return FALSE;
} // End PrimFnDydEpsilonBvB


//***************************************************************************
//  $PrimFnDydEpsilonNvB
//
//  Dyadic epsilon of APLINT/APLAPA/APLFLOAT vs. APLBOOL
//***************************************************************************

UBOOL PrimFnDydEpsilonNvB
    (LPAPLBOOL lpMemRes,            // Ptr to result global memory data
     APLSTYPE  aplTypeLft,          // Left arg storage type
     APLNELM   aplNELMLft,          // Left arg (and result) NELM
     LPAPLBOOL lpMemLft,            // Ptr to left arg global memory data
     APLNELM   aplNELMRht,          // Right arg NELM
     LPAPLBOOL lpMemRht,            // Ptr to right arg global memory data
     LPUBOOL   lpbCtrlBreak)        // Ptr to Ctrl-Break flag

{
    UBOOL    Found[2];              // TRUE iff there is at least one [0,1] in the right arg
    APLUINT  BytesInRhtBits,        // # bytes in the right arg data
             uBit,                  // The bit we found or are searching for
             uValid,                // # valid bits in the arg
             uLft,                  // Loop counter
             uRht,                  // Loop counter
             uTmp;                  // Temporary
    UINT     uBitIndex;             // Bit index when marching through Booleans
    APLINT   apaOff,                // APA offfset
             apaMul;                // ... multiplier
    APLFLOAT uFlt,                  // Temporary float
             fQuadCT;               // []CT
    UBOOL    bRet;                  // TRUE iff the result is valid

    // Get the current value of []CT
    fQuadCT = GetQuadCT ();

    // Calculate the # bytes in the right arg data
    BytesInRhtBits = RoundUpBitsToBytes (aplNELMRht);

    // Get the first bit from the right arg
    uBit = BIT0 & *(LPAPLBOOL) lpMemRht;

    // Save that information
    Found[uBit] = TRUE;

    // Complement the value to get the bit we're searching for
    uBit = !uBit;

    // Search the right arg for uBit
    for (Found[uBit] = FALSE, uRht = 0; uRht < (BytesInRhtBits - 1); uRht++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        if (FastBoolTrans[lpMemRht[uRht]][fbtFirst[uBit]] < NBIB)
        {
            Found[uBit] = TRUE;

            break;
        } // End IF
    } // End FOR

    // If we didn't find uBit, check the last byte (may be short)
    if (!Found[uBit])
    {
        // Calculate the # valid bits in the last byte in the right arg
        uValid = (MASKLOG2NBIB & (aplNELMRht - 1)) + 1;

        // Handle the last byte specially
        Found[uBit] = (FastBoolTrans[lpMemRht[uRht] & ((BIT0 << uValid) - 1)][fbtFirst[uBit]] < uValid);
    } // End IF

    // If the left arg is APA, get its parameters
    if (IsSimpleAPA (aplTypeLft))
    {
#define lpaplAPA        ((LPAPLAPA) lpMemLft)
        apaOff = lpaplAPA->Off;
        apaMul = lpaplAPA->Mul;
#undef  lpaplAPA
    } // End IF

    // Initialize the Boolean bit index
    uBitIndex = 0;

    // Loop through the left arg,
    //   saving in the result Found[0] for each 0 in the left arg, and
    //                        Found[1] for each 1 in the left arg, and
    //                        0        for each non-Boolean ...
    for (uLft = 0; uLft < aplNELMLft; uLft++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Split cases based upon the left arg storage type
        switch (aplTypeLft)
        {
            case ARRAY_INT:
                // Get the next left arg value
                uTmp = *((LPAPLINT) lpMemLft)++;

                // Split cases based upon the left arg value
                if (IsBooleanValue (uTmp))
                    // Save the appropriate value in the result
                    *lpMemRes |= Found[uTmp] << uBitIndex;
////////////////else
////////////////    // Save the appropriate value in the result
////////////////    *lpMemRes |= 0;
                break;

            case ARRAY_APA:
                // Get the next left arg value
                uTmp = apaOff + apaMul * uLft;

                // Split cases based upon the left arg value
                if (IsBooleanValue (uTmp))
                    // Save the appropriate value in the result
                    *lpMemRes |= Found[uTmp] << uBitIndex;
////////////////else
////////////////    // Save the appropriate value in the result
////////////////    *lpMemRes |= 0;
                break;

            case ARRAY_FLOAT:
                // Get the next left arg value
                uFlt = *((LPAPLFLOAT) lpMemLft)++;

                if (uFlt EQ 0)
                    // Save the appropriate value in the result
                    *lpMemRes |= Found[0] << uBitIndex;
                else
                if (CompareCT (uFlt, 1.0, fQuadCT, NULL))
                    // Save the appropriate value in the result
                    *lpMemRes |= Found[1] << uBitIndex;
////////////////else
////////////////    // Save the appropriate value in the result
////////////////    *lpMemRes |= 0;
                break;

            case ARRAY_RAT:
                // Attempt to convert the RAT to an APLINT
                uTmp = mpq_get_sx (&((LPAPLRAT) lpMemLft)[uLft], &bRet);
                if (bRet && IsBooleanValue (uTmp))
                    // Save the appropriate value in the result
                    *lpMemRes |= Found[uTmp] << uBitIndex;
////////////////else
////////////////    // Save the appropriate value in the result
////////////////    *lpMemRes |= 0;
                break;

            case ARRAY_VFP:
                // Attempt to convert the VFP to an APLINT
                uTmp = mpfr_get_sx (&((LPAPLVFP) lpMemLft)[uLft], &bRet);
                if (bRet && IsBooleanValue (uTmp))
                    // Save the appropriate value in the result
                    *lpMemRes |= Found[uTmp] << uBitIndex;
////////////////else
////////////////    // Save the appropriate value in the result
////////////////    *lpMemRes |= 0;
                break;

            defstop
                break;
        } // End SWITCH

        // Check for end-of-byte
        if (++uBitIndex EQ NBIB)
        {
            uBitIndex = 0;              // Start over
            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
        } // End IF
    } // End FOR

    return TRUE;
ERROR_EXIT:
    return FALSE;
} // End PrimFnDydEpsilonNvB


//***************************************************************************
//  $PrimFnDydEpsilonNvA_EM
//
//  Dyadic epsilon of APLBOOL/APLINT/APLAPA/APLFLOAT vs. APLAPA
//***************************************************************************

UBOOL PrimFnDydEpsilonNvA_EM
    (LPAPLBOOL lpMemRes,                // Ptr to result global memory data
     APLSTYPE  aplTypeLft,              // Left arg storage type
     APLNELM   aplNELMLft,              // Left arg NELM
     LPVOID    lpMemLft,                // Ptr to left arg global memory data
     APLNELM   aplNELMRht,              // Right arg NELM
     LPAPLAPA  lpMemRht,                // Ptr to right arg global memory data
     LPUBOOL   lpbCtrlBreak,            // Ptr to Ctrl-Break flag
     LPTOKEN   lptkFunc)                // Ptr to function token

{
    APLINT       iLft,                  // Loop counter
                 apaOffRht,             // Right arg APA offset
                 apaMulRht,             // ...           multiplier
                 apaMinRht,             // ...           minimum value
                 apaMaxRht,             // ...           maximum ...
                 aplIntegerLft;         // Left arg integer
    APLFLOAT     fQuadCT;               // []CT
    UBOOL        bRet = FALSE;          // TRUE iff the result is valid

    // Get the current value of []CT
    fQuadCT = GetQuadCT ();

    // Get the right arg APA parameters
    apaOffRht = lpMemRht->Off;
    apaMulRht = lpMemRht->Mul;

    if (apaMulRht >= 0)
    {
        apaMinRht = apaOffRht;
        apaMaxRht = apaOffRht + (aplNELMRht - 1) * apaMulRht;
    } else
    {
        apaMinRht = apaOffRht + (aplNELMRht - 1) * apaMulRht;
        apaMaxRht = apaOffRht;
    } // End IF/ELSE

    // Loop through the left arg values and look'em up
    //   in the right arg
    for (iLft = 0; iLft < (APLINT) aplNELMLft; iLft++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Split cases based upon the left arg storage type
        switch (aplTypeLft)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
                // Get the next integer from the left arg
                aplIntegerLft =
                  GetNextInteger (lpMemLft,     // Ptr to global memory
                                  aplTypeLft,   // Storage type
                                  iLft);        // Index
                // Mark as an integer
                bRet = TRUE;

                break;

            case ARRAY_FLOAT:
                // Attempt to convert the float to an integer using []CT
                aplIntegerLft =
                  FloatToAplint_CT (*((LPAPLFLOAT) lpMemLft)++,
                                    fQuadCT,
                                   &bRet);
                break;

            case ARRAY_RAT:
                // Attempt to convert the RAT to an APLINT
                aplIntegerLft = mpq_get_sx (((LPAPLRAT) lpMemLft)++, &bRet);

                break;

            case ARRAY_VFP:
                // Attempt to convert the VFP to an APLINT
                aplIntegerLft = mpfr_get_sx (((LPAPLVFP) lpMemLft)++, &bRet);

                break;

            defstop
                break;
        } // End SWITCH

        // Determine if the left arg value is in the right arg
        if (bRet                                            //     L is an integer
         && apaMinRht <= aplIntegerLft                      // AND L is at or above the minimum
         &&              aplIntegerLft <= apaMaxRht         // AND L is at or below the maximum
         && (0 EQ apaMulRht                                 // AND (R is a constant
          || 0 EQ (aplIntegerLft - apaOffRht) % apaMulRht)) //   OR L is in the APA)
            // Save in the result
            lpMemRes[iLft >> LOG2NBIB] |=
              1 << (MASKLOG2NBIB & (UINT) iLft);
////////else
////////    // Save in the result
////////    lpMemRes[iLft >> LOG2NBIB] |=
////////      0 << (MASKLOG2NBIB & (UINT) iLft);
    } // End FOR

    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
    return bRet;
} // End PrimFnDydEpsilonNvA_EM


//***************************************************************************
//  $PrimFnDydEpsilonIvI_EM
//
//  Dyadic epsilon of APLBOOL/APLINT/APLAPA vs. APLINT
//***************************************************************************

UBOOL PrimFnDydEpsilonIvI_EM
    (LPAPLBOOL lpMemRes,                // Ptr to result global memory data
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
     LPUBOOL   lpbCtrlBreak,            // Ptr to Ctrl-Break flag
     LPTOKEN   lptkFunc)                // Ptr to function token

{
    TOKEN        tkFunc = {0};          // Grade-up function token
    APLBOOL      bQuadIO;               // []IO
    HGLOBAL      hGlbGupRht = NULL;     // Right arg grade-up global memory handle
    LPAPLINT     lpMemGupRht = NULL;    // Ptr to right arg grade-up global memory
#ifdef GRADE2ND
    HGLOBAL      hGlbGupLft = NULL;     // Left arg grade-up global memory handle
    LPAPLINT     lpMemGupLft = NULL;    // Ptr to left arg grade-up global memory
    UBOOL        bLastVal;              // Last value saved in the result
    APLINT       aplIntegerLst,         // Last left arg integer
                 iRes;                  // Loop counter
#endif
    APLINT       iLft,                  // Loop counter
                 iRht,                  // Loop counter
                 iMin,                  // Minimum index
                 iMax;                  // Maximum ...
    APLINT       aplIntegerLft = 0,     // Left arg integer
                 aplIntegerRht,         // Right arg integer
                 aplIntegerZero = 0;    // A zero in case the right arg is a scalar
    LPPL_YYSTYPE lpYYRes;               // Ptr to grade-up result
    UBOOL        bRet = FALSE;          // TRUE iff the result is valid

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

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
        lpMemGupLft = MyGlobalLock (hGlbGupLft);

        // Skip over the header and dimensions to the data
        lpMemGupLft = VarArrayBaseToData (lpMemGupLft, 1);
    } else
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
        lpMemGupRht = MyGlobalLock (hGlbGupRht);

        // Skip over the header and dimensions to the data
        lpMemGupRht = VarArrayBaseToData (lpMemGupRht, 1);
    } else
        lpMemGupRht = &aplIntegerZero;

    // Loop through the left arg values and look'em up
    //   in the right arg
    for (iLft = 0; iLft < (APLINT) aplNELMLft; iLft++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;
#ifdef GRADE2ND
        // Save the left arg value as the last one
        aplIntegerLst = aplIntegerLft;

        // Get the result index
        iRes = lpMemGupLft[iLft];

        // Get the next integer from the left arg
        aplIntegerLft =
          GetNextInteger (lpMemLft,                 // Ptr to global memory
                          aplTypeLft,               // Storage type
                          iRes);                    // Index
        // Check for duplicate value from the left arg
        if (iLft
         && aplIntegerLft EQ aplIntegerLst)
        {
            // Save in the result
            lpMemRes[iRes >> LOG2NBIB] |=
              bLastVal << (MASKLOG2NBIB & (UINT) iRes);

            continue;
        } // End IF
#else
        // Get the next integer from the left arg
        aplIntegerLft =
          GetNextInteger (lpMemLft,                 // Ptr to global memory
                          aplTypeLft,               // Storage type
                          iLft);                    // Index
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
              GetNextInteger (lpMemRht,             // Ptr to global memory
                              aplTypeRht,           // Storage type
                              lpMemGupRht[iRht]);   // Index
            // Check for a match
            if (aplIntegerRht > aplIntegerLft)
                iMax = iRht - 1;
            else
            if (aplIntegerRht < aplIntegerLft)
                iMin = iRht + 1;
            else
            {
                // We found a match
#ifdef GRADE2ND
                // Save as last value
                bLastVal = 1;

                // Save in the result
                lpMemRes[iRes >> LOG2NBIB] |=
                  1 << (MASKLOG2NBIB & (UINT) iRes);
#else
                // Save in the result
                lpMemRes[iLft >> LOG2NBIB] |=
                  1 << (MASKLOG2NBIB & (UINT) iLft);
#endif
                break;
            } // End IF/ELSE/...
        } // End WHILE
#ifdef GRADE2ND
        // If we didn't find a match, ...
        if (iMin > iMax)
        {
            // Save as last value
            bLastVal = 0;

////////////// Save in the result
////////////lpMemRes[iRes >> LOG2NBIB] |=
////////////  0 << (MASKLOG2NBIB & (UINT) iRes);
        } // End IF
#endif
    } // End FOR

    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
#ifdef GRADE2ND
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
#endif
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

    return bRet;
} // End PrimFnDydEpsilonIvI_EM


//***************************************************************************
//  $PrimFnDydEpsilonNvN_EM
//
//  Dyadic epsilon of APLBOOL/APLINT/APLAPA/APLFLOAT vs. APLFLOAT and
//                    APLFLOAT vs. APLINT
//***************************************************************************

UBOOL PrimFnDydEpsilonNvN_EM
    (LPAPLBOOL lpMemRes,                // Ptr to result global memory data
     LPTOKEN   lptkLftArg,              // Ptr to left arg token
     APLSTYPE  aplTypeLft,              // Left arg storage type
     APLNELM   aplNELMLft,              // Left arg NELM
     APLRANK   aplRankLft,              // Left arg rank
     LPVOID    lpMemLft,                // Ptr to left arg global memory data
     LPTOKEN   lptkRhtArg,              // Ptr to right arg token
     APLSTYPE  aplTypeRht,              // Ptr to right arg storage type
     APLNELM   aplNELMRht,              // Right arg NELM
     APLRANK   aplRankRht,              // Right arg rank
     LPVOID    lpMemRht,                // Ptr to right arg global memory data
     LPUBOOL   lpbCtrlBreak,            // Ptr to Ctrl-Break flag
     LPTOKEN   lptkFunc)                // Ptr to function token

{
    TOKEN        tkFunc = {0};          // Grade-up function token
    APLBOOL      bQuadIO;               // []IO
    HGLOBAL      hGlbGupRht = NULL;     // Right arg grade-up global memory handle
    LPAPLINT     lpMemGupRht = NULL;    // Ptr to right arg grade-up global memory
#ifdef GRADE2ND
    HGLOBAL      hGlbGupLft = NULL;     // Left arg grade-up global memory handle
    LPAPLINT     lpMemGupLft = NULL;    // Ptr to left arg grade-up global memory
    UBOOL        bLastVal;              // Last value saved in the result
    APLFLOAT     aplFloatLst;           // Last left arg float
    APLINT       iRes;                  // Loop counter
#endif
    APLINT       iLft,                  // Loop counter
                 iRht,                  // Loop counter
                 iMin,                  // Minimum index
                 iMax;                  // Maximum ...
    APLFLOAT     aplFloatLft = 0,       // Left arg float
                 aplFloatRht,           // Right arg float
                 fQuadCT;               // []CT
    APLINT       aplIntegerZero = 0;    // A zero in case the right arg is a scalar
    LPPL_YYSTYPE lpYYRes;               // Ptr to grade-up result
    UBOOL        bRet = FALSE,          // TRUE iff the result is valid
                 bComp;                 // TRUE iff the left and right floats are equal within []CT

    // Get the current value of []CT
    fQuadCT = GetQuadCT ();

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

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
        lpMemGupLft = MyGlobalLock (hGlbGupLft);

        // Skip over the header and dimensions to the data
        lpMemGupLft = VarArrayBaseToData (lpMemGupLft, 1);
    } else
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
        lpMemGupRht = MyGlobalLock (hGlbGupRht);

        // Skip over the header and dimensions to the data
        lpMemGupRht = VarArrayBaseToData (lpMemGupRht, 1);
    } else
        lpMemGupRht = &aplIntegerZero;

    // Loop through the left arg values and look'em up
    //   in the right arg
    for (iLft = 0; iLft < (APLINT) aplNELMLft; iLft++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;
#ifdef GRADE2ND
        // Save the left arg value as the last one
        aplFloatLst = aplFloatLft;

        // Get the result index
        iRes = lpMemGupLft[iLft];

        // Get the next float from the left arg
        aplFloatLft =
          GetNextFloat (lpMemLft,                   // Ptr to global memory
                        aplTypeLft,                 // Storage type
                        iRes);                      // Index
        // Check for duplicate value from the left arg
        if (iLft
         && aplFloatLft EQ aplFloatLst)
        {
            // Save in the result
            lpMemRes[iRes >> LOG2NBIB] |=
              bLastVal << (MASKLOG2NBIB & (UINT) iRes);

            continue;
        } // End IF
#else
        // Split cases based upon the left arg storage type
        switch (aplTypeLft)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
            case ARRAY_FLOAT:
                // Get the next float from the left arg
                aplFloatLft =
                  GetNextFloat (lpMemLft,                   // Ptr to global memory
                                aplTypeLft,                 // Storage type
                                iLft);                      // Index
                break;

            case ARRAY_RAT:
                // Attempt to convert the RAT to an APLFLOAT
                aplFloatLft = mpq_get_d (&((LPAPLRAT) lpMemLft)[iLft]);

                break;

            case ARRAY_VFP:
                // Attempt to convert the VFP to an APLFLOAT
                aplFloatLft = mpfr_get_d (&((LPAPLVFP) lpMemLft)[iLft], MPFR_RNDN);

                break;

            defstop
                break;
        } // End SWITCH
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
              GetNextFloat (lpMemRht,               // Ptr to global memory
                            aplTypeRht,             // Storage type
                            lpMemGupRht[iRht]);     // Index
            // Compare 'em
            bComp = CompareCT (aplFloatLft, aplFloatRht, fQuadCT, NULL);

            // Check for a match
            if (bComp)
            {
                // We found a match

#ifdef GRADE2ND
                // Save as last value
                bLastVal = 1;

                // Save in the result
                lpMemRes[iRes >> LOG2NBIB] |=
                  1 << (MASKLOG2NBIB & (UINT) iRes);
#else
                // Save in the result
                lpMemRes[iLft >> LOG2NBIB] |=
                  1 << (MASKLOG2NBIB & (UINT) iLft);
#endif
                break;
            } else
            if (aplFloatRht > aplFloatLft)
                iMax = iRht - 1;
            else
                iMin = iRht + 1;
        } // End WHILE
#ifdef GRADE2ND
        // If we didn't find a match, ...
        if (iMin > iMax)
        {
            // Save as last value
            bLastVal = 0;

////////////// Save in the result
////////////lpMemRes[iRes >> LOG2NBIB] |=
////////////  0 << (MASKLOG2NBIB & (UINT) iRes);
        } // End IF
#endif
    } // End FOR

    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
#ifdef GRADE2ND
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
#endif
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

    return bRet;
} // End PrimFnDydEpsilonNvN_EM


//***************************************************************************
//  $PrimFnDydEpsilonNvR_EM
//
//  Dyadic epsilon of APLBOOL/APLINT/APLAPA/APLFLOAT/APLRAT/APLVFP vs. APLRAT
//***************************************************************************

UBOOL PrimFnDydEpsilonNvR_EM
    (LPAPLBOOL lpMemRes,                // Ptr to result global memory data
     LPTOKEN   lptkLftArg,              // Ptr to left arg token
     APLSTYPE  aplTypeLft,              // Left arg storage type
     APLNELM   aplNELMLft,              // Left arg NELM
     APLRANK   aplRankLft,              // Left arg rank
     LPVOID    lpMemLft,                // Ptr to left arg global memory data
     LPTOKEN   lptkRhtArg,              // Ptr to right arg token
     APLSTYPE  aplTypeRht,              // Ptr to right arg storage type
     APLNELM   aplNELMRht,              // Right arg NELM
     APLRANK   aplRankRht,              // Right arg rank
     LPAPLRAT  lpMemRht,                // Ptr to right arg global memory data
     LPUBOOL   lpbCtrlBreak,            // Ptr to Ctrl-Break flag
     LPTOKEN   lptkFunc)                // Ptr to function token

{
    TOKEN        tkFunc = {0};          // Grade-up function token
    APLBOOL      bQuadIO;               // []IO
    HGLOBAL      hGlbGupRht = NULL;     // Right arg grade-up global memory handle
    LPAPLINT     lpMemGupRht = NULL;    // Ptr to right arg grade-up global memory
#ifdef GRADE2ND
    HGLOBAL      hGlbGupLft = NULL;     // Left arg grade-up global memory handle
    LPAPLINT     lpMemGupLft = NULL;    // Ptr to left arg grade-up global memory
    UBOOL        bLastVal;              // Last value saved in the result
    APLFLOAT     aplFloatLst;           // Last left arg float
    APLINT       iRes;                  // Loop counter
#endif
    APLINT       iLft,                  // Loop counter
                 iRht,                  // Loop counter
                 iMin,                  // Minimum index
                 iMax;                  // Maximum ...
    APLINT       aplIntegerZero = 0;    // A zero in case the right arg is a scalar
    APLFLOAT     fQuadCT;               // []CT
    LPPL_YYSTYPE lpYYRes;               // Ptr to grade-up result
    UBOOL        bRet = FALSE,          // TRUE iff the result is valid
                 bCompCT = FALSE;       // TRUE iff we're comparing w.r.t. []CT
    APLINT       iComp;                 // TRUE iff the left and right floats are equal within []CT
    APLRAT       aplRatLft = {0},       // Left arg item as rational
                 aplRatRht = {0};       // Right ...
    APLVFP       aplVfpLft = {0},       // Left arg as VFP
                 aplVfpRht = {0};       // Right ...

    // Initialize the temps
    mpq_init (&aplRatLft);
    mpq_init (&aplRatRht);
    mpfr_init0 (&aplVfpLft);
    mpfr_init0 (&aplVfpRht);

    // Set the compare w.r.t. []CT flag
    bCompCT = IsVfp (aplTypeLft);

    // Get the current value of []CT
    fQuadCT = GetQuadCT ();

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

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
        lpMemGupLft = MyGlobalLock (hGlbGupLft);

        // Skip over the header and dimensions to the data
        lpMemGupLft = VarArrayBaseToData (lpMemGupLft, 1);
    } else
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
        lpMemGupRht = MyGlobalLock (hGlbGupRht);

        // Skip over the header and dimensions to the data
        lpMemGupRht = VarArrayBaseToData (lpMemGupRht, 1);
    } else
        lpMemGupRht = &aplIntegerZero;

    // Loop through the left arg values and look'em up
    //   in the right arg
    for (iLft = 0; iLft < (APLINT) aplNELMLft; iLft++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;
#ifdef GRADE2ND
        // Save the left arg value as the last one
        aplFloatLst = aplFloatLft;

        // Get the result index
        iRes = lpMemGupLft[iLft];

        // Get the next float from the left arg
        aplFloatLft =
          GetNextFloat (lpMemLft,                   // Ptr to global memory
                        aplTypeLft,                 // Storage type
                        iRes);                      // Index
        // Check for duplicate value from the left arg
        if (iLft
         && aplFloatLft EQ aplFloatLst)
        {
            // Save in the result
            lpMemRes[iRes >> LOG2NBIB] |=
              bLastVal << (MASKLOG2NBIB & (UINT) iRes);

            continue;
        } // End IF
#else
        // Split cases based upon the left arg storage type
        switch (aplTypeLft)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
                // Convert the BOOL/INT/APA to a RAT
                mpq_set_sx (&aplRatLft,
                             GetNextInteger (lpMemLft,      // Ptr to global memory
                                             aplTypeLft,    // Storage type
                                             iLft),         // Index
                             1);
                break;

            case ARRAY_FLOAT:
                // Convert the FLOAT to a RAT
                mpq_set_d (&aplRatLft,
                           ((LPAPLFLOAT) lpMemLft)[iLft]);
                break;

            case ARRAY_RAT:
                // Copy the RAT to a RAT
                mpq_set   (&aplRatLft,
                           &((LPAPLRAT)   lpMemLft)[iLft]);
                break;

            case ARRAY_VFP:
                // Copy the VFP to a VFP
                mpfr_copy  (&aplVfpLft,
                           &((LPAPLVFP)   lpMemLft)[iLft]);
                break;

            defstop
                break;
        } // End SWITCH
#endif
        // Initialize the right arg minimum and maximum indices
        iMin = 0;
        iMax = aplNELMRht - 1;

        // Lookup this value in the right arg (binary search)
        while (iMin <= iMax)
        {
            // Set the current index
            iRht = (iMin + iMax) / 2;

            // If we're comparing w.r.t. []CT, ...
            if (bCompCT)
            {
                // Get the next rational from the right arg
                mpfr_set_q (&aplVfpRht, &lpMemRht[lpMemGupRht[iRht]], MPFR_RNDN);

                // Compare 'em
                iComp = mpfr_cmp_ct (aplVfpLft, aplVfpRht, fQuadCT);
            } else
            {
                // Get the next rational from the right arg
                mpq_set   (&aplRatRht, &lpMemRht[lpMemGupRht[iRht]]);

                // Compare 'em
                iComp = mpq_cmp    (&aplRatLft, &aplRatRht);
            } // End IF/ELSE

            // Check for a match
            if (iComp EQ 0)
            {
                // We found a match

#ifdef GRADE2ND
                // Save as last value
                bLastVal = 1;

                // Save in the result
                lpMemRes[iRes >> LOG2NBIB] |=
                  1 << (MASKLOG2NBIB & (UINT) iRes);
#else
                // Save in the result
                lpMemRes[iLft >> LOG2NBIB] |=
                  1 << (MASKLOG2NBIB & (UINT) iLft);
#endif
                break;
            } else
            if (iComp < 0)
                iMax = iRht - 1;
            else
                iMin = iRht + 1;
        } // End WHILE
#ifdef GRADE2ND
        // If we didn't find a match, ...
        if (iMin > iMax)
        {
            // Save as last value
            bLastVal = 0;

////////////// Save in the result
////////////lpMemRes[iRes >> LOG2NBIB] |=
////////////  0 << (MASKLOG2NBIB & (UINT) iRes);
        } // End IF
#endif
    } // End FOR

    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
#ifdef GRADE2ND
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
#endif
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

    // We no longer need this storage
    Myf_clear (&aplVfpRht);
    Myf_clear (&aplVfpLft);
    Myq_clear (&aplRatRht);
    Myq_clear (&aplRatLft);

    return bRet;
} // End PrimFnDydEpsilonNvR_EM


//***************************************************************************
//  $PrimFnDydEpsilonNvV_EM
//
//  Dyadic epsilon of APLBOOL/APLINT/APLAPA/APLFLOAT/APLRAT/APLVFP vs. APLVFP
//***************************************************************************

UBOOL PrimFnDydEpsilonNvV_EM
    (LPAPLBOOL lpMemRes,                // Ptr to result global memory data
     LPTOKEN   lptkLftArg,              // Ptr to left arg token
     APLSTYPE  aplTypeLft,              // Left arg storage type
     APLNELM   aplNELMLft,              // Left arg NELM
     APLRANK   aplRankLft,              // Left arg rank
     LPVOID    lpMemLft,                // Ptr to left arg global memory data
     LPTOKEN   lptkRhtArg,              // Ptr to right arg token
     APLSTYPE  aplTypeRht,              // Ptr to right arg storage type
     APLNELM   aplNELMRht,              // Right arg NELM
     APLRANK   aplRankRht,              // Right arg rank
     LPAPLVFP  lpMemRht,                // Ptr to right arg global memory data
     LPUBOOL   lpbCtrlBreak,            // Ptr to Ctrl-Break flag
     LPTOKEN   lptkFunc)                // Ptr to function token

{
    TOKEN        tkFunc = {0};          // Grade-up function token
    APLBOOL      bQuadIO;               // []IO
    HGLOBAL      hGlbGupRht = NULL;     // Right arg grade-up global memory handle
    LPAPLINT     lpMemGupRht = NULL;    // Ptr to right arg grade-up global memory
#ifdef GRADE2ND
    HGLOBAL      hGlbGupLft = NULL;     // Left arg grade-up global memory handle
    LPAPLINT     lpMemGupLft = NULL;    // Ptr to left arg grade-up global memory
    UBOOL        bLastVal;              // Last value saved in the result
    APLFLOAT     aplFloatLst;           // Last left arg float
    APLINT       iRes;                  // Loop counter
#endif
    APLINT       iLft,                  // Loop counter
                 iRht,                  // Loop counter
                 iMin,                  // Minimum index
                 iMax;                  // Maximum ...
    APLINT       aplIntegerZero = 0;    // A zero in case the right arg is a scalar
    APLFLOAT     fQuadCT;               // []CT
    LPPL_YYSTYPE lpYYRes;               // Ptr to grade-up result
    UBOOL        bRet = FALSE;          // TRUE iff the result is valid
    APLINT       iComp;                 // TRUE iff the left and right floats are equal within []CT
    APLVFP       aplVfpLft = {0},       // Left arg item as VFP
                 aplVfpRht = {0};       // Right ...

    // Initialize the temps
    mpfr_init0 (&aplVfpLft);
    mpfr_init0 (&aplVfpRht);

    // Get the current value of []CT
    fQuadCT = GetQuadCT ();

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

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
        lpMemGupLft = MyGlobalLock (hGlbGupLft);

        // Skip over the header and dimensions to the data
        lpMemGupLft = VarArrayBaseToData (lpMemGupLft, 1);
    } else
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
        lpMemGupRht = MyGlobalLock (hGlbGupRht);

        // Skip over the header and dimensions to the data
        lpMemGupRht = VarArrayBaseToData (lpMemGupRht, 1);
    } else
        lpMemGupRht = &aplIntegerZero;

    // Loop through the left arg values and look'em up
    //   in the right arg
    for (iLft = 0; iLft < (APLINT) aplNELMLft; iLft++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;
#ifdef GRADE2ND
        // Save the left arg value as the last one
        aplFloatLst = aplFloatLft;

        // Get the result index
        iRes = lpMemGupLft[iLft];

        // Get the next float from the left arg
        aplFloatLft =
          GetNextFloat (lpMemLft,                   // Ptr to global memory
                        aplTypeLft,                 // Storage type
                        iRes);                      // Index
        // Check for duplicate value from the left arg
        if (iLft
         && aplFloatLft EQ aplFloatLst)
        {
            // Save in the result
            lpMemRes[iRes >> LOG2NBIB] |=
              bLastVal << (MASKLOG2NBIB & (UINT) iRes);

            continue;
        } // End IF
#else
        // Split cases based upon the left arg storage type
        switch (aplTypeLft)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
                // Convert the BOOL/INT/APA to a VFP
                mpfr_set_sx (&aplVfpLft,
                              GetNextInteger (lpMemLft,     // Ptr to global memory
                                              aplTypeLft,   // Storage type
                                              iLft),        // Index
                              MPFR_RNDN);                   // Rounding mode
                break;

            case ARRAY_FLOAT:
                // Convert the FLOAT to a VFP
                mpfr_set_d (&aplVfpLft,
                           ((LPAPLFLOAT) lpMemLft)[iLft],
                           MPFR_RNDN);
                break;

            case ARRAY_RAT:
                // Convert the RAT to a VFP
                mpfr_set_q (&aplVfpLft,
                           &((LPAPLRAT)   lpMemLft)[iLft],
                           MPFR_RNDN);
                break;

            case ARRAY_VFP:
                // Copy the VFP to a VFP
                mpfr_copy  (&aplVfpLft,
                           &((LPAPLVFP)   lpMemLft)[iLft]);
                break;

            defstop
                break;
        } // End SWITCH
#endif
        // Initialize the right arg minimum and maximum indices
        iMin = 0;
        iMax = aplNELMRht - 1;

        // Lookup this value in the right arg (binary search)
        while (iMin <= iMax)
        {
            // Set the current index
            iRht = (iMin + iMax) / 2;

            // Get the next VFP from the right arg
            mpfr_copy (&aplVfpRht, &lpMemRht[lpMemGupRht[iRht]]);

            // Compare 'em
            iComp = mpfr_cmp_ct (aplVfpLft, aplVfpRht, fQuadCT);

            // Check for a match
            if (iComp EQ 0)
            {
                // We found a match

#ifdef GRADE2ND
                // Save as last value
                bLastVal = 1;

                // Save in the result
                lpMemRes[iRes >> LOG2NBIB] |=
                  1 << (MASKLOG2NBIB & (UINT) iRes);
#else
                // Save in the result
                lpMemRes[iLft >> LOG2NBIB] |=
                  1 << (MASKLOG2NBIB & (UINT) iLft);
#endif
                break;
            } else
            if (iComp < 0)
                iMax = iRht - 1;
            else
                iMin = iRht + 1;
        } // End WHILE
#ifdef GRADE2ND
        // If we didn't find a match, ...
        if (iMin > iMax)
        {
            // Save as last value
            bLastVal = 0;

////////////// Save in the result
////////////lpMemRes[iRes >> LOG2NBIB] |=
////////////  0 << (MASKLOG2NBIB & (UINT) iRes);
        } // End IF
#endif
    } // End FOR

    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
#ifdef GRADE2ND
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
#endif
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

    // We no longer need this storage
    Myf_clear (&aplVfpRht);
    Myf_clear (&aplVfpLft);

    return bRet;
} // End PrimFnDydEpsilonNvV_EM


//***************************************************************************
//  $PrimFnDydEpsilonNvP_EM
//
//  Dyadic epsilon of APLBOOL/APLINT/APLAPA/APLFLOAT vs. PV
//***************************************************************************

UBOOL PrimFnDydEpsilonNvP_EM
    (LPAPLBOOL         lpMemRes,        // Ptr to result global memory data
     APLSTYPE          aplTypeLft,      // Left arg storage type
     APLNELM           aplNELMLft,      // Left arg NELM
     LPVOID            lpMemLft,        // Ptr to left arg global memory data
     LPVARARRAY_HEADER lpHeaderRht,     // Ptr to right arg header
     APLSTYPE          aplTypeRht,      // Right arg storage type
     APLNELM           aplNELMRht,      // Right arg NELM
     LPUBOOL           lpbCtrlBreak,    // Ptr to Ctrl-Break flag
     LPTOKEN           lptkFunc)        // Ptr to function token

{
    APLUINT   uLft;                     // Loop counter
    APLINT    aplIntegerRhtMin,         // Right arg minimum value
              aplIntegerRhtMax,         // Right arg maximum ...
              aplIntegerLft;            // Left  ...
    APLFLOAT  aplFloatLft,              // Left arg float
              fQuadCT;                  // []CT
    UINT      uBitMask;                 // Bit mask for looping through Booleans
    UBOOL     bRet = FALSE;             // TRUE iff the result is valid

    // Get the current value of []CT
    fQuadCT = GetQuadCT ();

    // Get the minimim and maximum values in the PV
    aplIntegerRhtMin = lpHeaderRht->PV1;
    aplIntegerRhtMax = aplNELMRht - lpHeaderRht->PV0;

    // Initialize the bit mask
    uBitMask = BIT0;

    // Loop through the left arg comparing each value
    //   against the minimum and maximum values in the PV,
    //   saving the answer in the result
    for (uLft = 0; uLft < aplNELMLft; uLft++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Split cases based upon the left arg storage type
        switch (aplTypeLft)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
                // Get the next integer
                aplIntegerLft =
                  GetNextInteger (lpMemLft,
                                  aplTypeLft,
                                  uLft);
                // Mark as an integer
                bRet = TRUE;

                break;

            case ARRAY_FLOAT:
                // Get the next float
                aplFloatLft = *((LPAPLFLOAT) lpMemLft)++;

                // Attempt to convert the float to an integer using []CT
                aplIntegerLft =
                  FloatToAplint_CT (aplFloatLft,
                                    fQuadCT,
                                   &bRet);
                break;

            case ARRAY_RAT:
                // Attempt to convert the RAT to an APLINT
                aplIntegerLft = mpq_get_sx (((LPAPLRAT) lpMemLft)++, &bRet);

                break;

            case ARRAY_VFP:
                // Attempt to convert the VFP to an APLINT
                aplIntegerLft = mpfr_get_sx (((LPAPLVFP) lpMemLft)++, &bRet);

                break;

            defstop
                break;
        } // End SWITCH

        // If it's an integer, and it's within range, ...
        if (bRet
         && aplIntegerRhtMin <= aplIntegerLft
         &&                     aplIntegerLft <= aplIntegerRhtMax)
            // Save a one in the result
            *lpMemRes |= uBitMask;
////////else
////////    // Save a zero in the result
////////    *lpMemRes |= 0;

        // Shift over the bit mask
        uBitMask <<= 1;

        // Check for end-of-byte
        if (uBitMask EQ END_OF_BYTE)
        {
            uBitMask = BIT0;            // Start over
            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
        } // End IF
    } // End FOR

    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
    return bRet;
} // End PrimFnDydEpsilonNvP_EM


//***************************************************************************
//  $PrimFnDydEpsilonCvC_EM
//
//  Dyadic epsilon of APLCHAR vs. APLCHAR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydEpsilonCvC_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydEpsilonCvC_EM
    (LPAPLBOOL lpMemRes,            // Ptr to result global memory data
     APLNELM   aplNELMLft,          // Left arg (and result) NELM
     LPAPLCHAR lpMemLft,            // Ptr to left arg global memory data
     APLNELM   aplNELMRht,          // Right arg NELM
     LPAPLCHAR lpMemRht,            // Ptr to right arg global memory data
     LPUBOOL   lpbCtrlBreak,        // Ptr to Ctrl-Break flag
     LPTOKEN   lptkFunc)            // Ptr to function token

{
    LPAPLBOOL lpMemTT;              // Translate table global memory handle
    APLUINT   ByteTT;               // # bytes in the Translate Table
    APLUINT   uLft,                 // Loop counter
              uRht;                 // Loop counter
    APLCHAR   aplChar;              // Temporary char
    UINT      uBitIndex;            // Bit index for marching through Booleans

    // Calculate # bytes in the TT at one bit per 16-bit index (APLCHAR)
    ByteTT = RoundUpBitsToBytes (APLCHAR_SIZE);

    // Check for overflow
    if (ByteTT NE (APLU3264) ByteTT)
        goto WSFULL_EXIT;

    // Allocate space for a ByteTT Translate Table
    // Note that this allocation is GPTR (GMEM_FIXED | GMEM_ZEROINIT)
    //   because we'll use it quickly and then free it.
    lpMemTT = MyGlobalAlloc (GPTR, (APLU3264) ByteTT);
    if (!lpMemTT)
        goto WSFULL_EXIT;

    // Trundle through the right arg marking the TT
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Get the APLCHAR from the right arg
        aplChar = *lpMemRht++;

        lpMemTT[aplChar >> LOG2NBIB] |= BIT0 << (MASKLOG2NBIB & aplChar);
    } // End FOR

    // Trundle through the left arg looking the chars in the TT
    //   and setting the result bit
    uBitIndex = 0;
    for (uLft = 0; uLft < aplNELMLft; uLft++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Get the APLCHAR from the left arg
        aplChar = *lpMemLft++;

        // If the char is in the TT, ...
        if (lpMemTT[aplChar >> LOG2NBIB] & (BIT0 << (MASKLOG2NBIB & aplChar)))
            *lpMemRes |= (BIT0 << uBitIndex);
        // Check for end-of-byte
        if (++uBitIndex EQ NBIB)
        {
            uBitIndex = 0;              // Start over
            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
        } // End IF
    } // End FOR

    // We no longer need this storage
    MyGlobalFree (lpMemTT); lpMemTT = NULL;

    return TRUE;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    return FALSE;
} // End PrimFnDydEpsilonCvC_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydEpsilonOther
//
//  Dyadic epsilon between all other arg combinations
//***************************************************************************

UBOOL PrimFnDydEpsilonOther
    (LPAPLBOOL lpMemRes,            // Ptr to result global memory data
     APLSTYPE  aplTypeLft,          // Left arg storage type
     APLNELM   aplNELMLft,          // Left arg NELM
     LPVOID    lpMemLft,            // Ptr to left arg global memory data
     APLSTYPE  aplTypeRht,          // Right arg storage type
     APLNELM   aplNELMRht,          // Right arg NELM
     LPVOID    lpMemRht,            // Ptr to right arg global memory data
     LPUBOOL   lpbCtrlBreak,        // Ptr to Ctrl-Break flag
     LPTOKEN   lptkFunc)            // Ptr to function token

{
    HGLOBAL      hGlbSubLft,        // Left arg item global memory handle
                 hGlbSubRht;        // Right ...
    APLLONGEST   aplLongestSubLft,  // Left arg item immediate value
                 aplLongestSubRht;  // Right ...
    IMM_TYPES    immTypeSubLft,     // Left arg item immediate type
                 immTypeSubRht;     // Right ...
    UINT         uBitIndex;         // Bit index for marching through Booleans
    APLFLOAT     fQuadCT;           // []CT
    APLUINT      uLft,              // Loop counter
                 uRht;              // ...
    LPPL_YYSTYPE lpYYTmp;           // Ptr to the temporary result
    UBOOL        bCmp;              // TRUE iff the comparison is TRUE

    // This leaves:  Left vs.  Right
    //               BIAF vs.     HN
    //               HN   vs. BIAFHN

    uBitIndex = 0;

    // Get the current value of []CT
    fQuadCT = GetQuadCT ();

    // Loop through the left arg
    for (uLft = 0; uLft < aplNELMLft; uLft++)
    {
        // Get the next value from the left arg
        GetNextValueMem (lpMemLft,              // Ptr to left arg global memory
                         aplTypeLft,            // Left arg storage type
                         uLft,                  // Left arg index
                        &hGlbSubLft,            // Left arg item LPSYMENTRY or HGLOBAL (may be NULL)
                        &aplLongestSubLft,      // Ptr to left arg immediate value
                        &immTypeSubLft);        // Ptr to left arg immediate type
        // Loop through the right arg
        for (uRht = 0; uRht < aplNELMRht; uRht++)
        {
            // Check for Ctrl-Break
            if (CheckCtrlBreak (*lpbCtrlBreak))
                goto ERROR_EXIT;

            // Get the next value from the right arg
            GetNextValueMem (lpMemRht,              // Ptr to right arg global memory
                             aplTypeRht,            // Right arg storage type
                             uRht,                  // Right arg index
                            &hGlbSubRht,            // Right arg item LPSYMENTRY or HGLOBAL (may be NULL)
                            &aplLongestSubRht,      // Ptr to right arg immediate value
                            &immTypeSubRht);        // Ptr to right arg immediate type
            // If both items are globals, ...
            if ((hGlbSubLft NE NULL) && (hGlbSubRht NE NULL))
            {
                TOKEN tkSubLft = {0},       // Left arg item token
                      tkSubRht = {0};       // Right ...

                // Fill in the left arg item token
                tkSubLft.tkFlags.TknType   = TKT_VARARRAY;
////////////////tkSubLft.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from = {0}
////////////////tkSubLft.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
                tkSubLft.tkData.tkGlbData  = hGlbSubLft;
                tkSubLft.tkCharIndex       = lptkFunc->tkCharIndex;

                // Fill in the right arg item token
                tkSubRht.tkFlags.TknType   = TKT_VARARRAY;
////////////////tkSubRht.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from = {0}
////////////////tkSubRht.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
                tkSubRht.tkData.tkGlbData  = MakePtrTypeGlb (hGlbSubRht);
                tkSubRht.tkCharIndex       = lptkFunc->tkCharIndex;

                // Use match to determine equality
                lpYYTmp =
                  PrimFnDydEqualUnderbar_EM_YY (&tkSubLft,      // Ptr to left arg token
                                                 lptkFunc,      // Ptr to function token
                                                &tkSubRht,      // Ptr to right arg token
                                                 NULL);         // Ptr to axis token (may be NULL)
                // Save the result of the comparison
                bCmp = lpYYTmp->tkToken.tkData.tkBoolean;

                // Free the temporary result
                YYFree (lpYYTmp); lpYYTmp = NULL;

                if (bCmp)
                    goto SET_RESULT_BIT;
            } else
            // If both items are simple, ...
            if ((hGlbSubLft EQ NULL) && (hGlbSubRht EQ NULL))
            {
                // If both items are APLCHARs, ...
                if (IsImmChr (immTypeSubLft) && IsImmChr (immTypeSubRht))
                {
                    // Compare the APLCHARs
                    if (((APLCHAR) aplLongestSubLft) EQ (APLCHAR) aplLongestSubRht)
                        goto SET_RESULT_BIT;
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
                                        goto SET_RESULT_BIT;
                                    break;

                                case IMMTYPE_INT:       // Lft = BOOL, Rht = INT
                                    if (((APLBOOL) aplLongestSubLft) EQ (APLINT)  aplLongestSubRht)
                                        goto SET_RESULT_BIT;
                                    break;

                                case IMMTYPE_FLOAT:     // Lft = BOOL, Rht = FLOAT
                                    if (CompareCT ((APLBOOL) aplLongestSubLft, *(LPAPLFLOAT) &aplLongestSubRht, fQuadCT, NULL))
                                        goto SET_RESULT_BIT;
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
                                        goto SET_RESULT_BIT;
                                    break;

                                case IMMTYPE_INT:       // Lft = INT, Rht = INT
                                    if (((APLINT)  aplLongestSubLft) EQ (APLINT)  aplLongestSubRht)
                                        goto SET_RESULT_BIT;
                                    break;

                                case IMMTYPE_FLOAT:     // Lft = INT, Rht = FLOAT
                                    if (CompareCT ((APLFLOAT) (APLINT)  aplLongestSubLft, *(LPAPLFLOAT) &aplLongestSubRht, fQuadCT, NULL))
                                        goto SET_RESULT_BIT;
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
                                        goto SET_RESULT_BIT;
                                    break;

                                case IMMTYPE_INT:       // Lft = FLOAT, Rht = INT
                                    if (CompareCT (*(LPAPLFLOAT) &aplLongestSubLft, (APLFLOAT) (APLINT) aplLongestSubRht, fQuadCT, NULL))
                                        goto SET_RESULT_BIT;
                                    break;

                                case IMMTYPE_FLOAT:     // Lft = FLOAT, Rht = FLOAT
                                    if (CompareCT (*(LPAPLFLOAT) &aplLongestSubLft, *(LPAPLFLOAT) &aplLongestSubRht, fQuadCT, NULL))
                                        goto SET_RESULT_BIT;
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

            continue;
SET_RESULT_BIT:
            // Set the result bit
            *lpMemRes |= (BIT0 << uBitIndex);

            break;
        } // End FOR

        // Check for end-of-byte
        if (++uBitIndex EQ NBIB)
        {
            uBitIndex = 0;      // Start over
            lpMemRes++;         // Skip to next byte
        } // End IF
    } // End FOR

    return TRUE;
ERROR_EXIT:
    return FALSE;
} // End PrimFnDydEpsilonOther


//***************************************************************************
//  End of File: pf_epsilon.c
//***************************************************************************
