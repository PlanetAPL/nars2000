//***************************************************************************
//  NARS2000 -- Primitive Functions
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
#include <math.h>
#include "headers.h"

static APLINT mod64 (APLINT, APLINT);

/// // These macros were taken (and adapted) from
/// //   http://www.fefe.de/intof.html
///
/// #define __HALF_MAX_SIGNED_APLINT ((APLINT)1 << (sizeof(APLINT)*8-2))
/// #define __MAX_SIGNED_APLINT (__HALF_MAX_SIGNED_APLINT - 1 + __HALF_MAX_SIGNED_APLINT)
/// #define __MIN_SIGNED_APLINT (-1 - __MAX_SIGNED_APLINT)
///
/// #define __MIN_APLINT ((APLINT)-1 < 1?__MIN_SIGNED_APLINT:(APLINT)0)
/// #define __MAX_APLINT ((APLINT)~__MIN_APLINT)
///
/// #define add_of(c,__a,__b)  \
///   ((__b) < 1) ? \
///     ((__MIN_APLINT - (__b)) <= (__a) ? c = __a + __b, 1 : 0) : \
///     ((__MAX_APLINT - (__b)) >= (__a) ? c = __a + __b, 1 : 0);
///
///
/// #define sub_of(c,__a,__b)  \
///   ((__b) < 1) ? \
///     ((__MAX_APLINT + (__b)) >= (__a) ? c = __a - __b, 1 : 0) : \
///     ((__MIN_APLINT + (__b)) <= (__a) ? c = __a - __b, 1 : 0);


//***************************************************************************
//  $IsTknOp1
//
//  Is the token a monadic operator?
//***************************************************************************

UBOOL IsTknOp1
    (LPTOKEN lptkFcnOpr)            // Ptr to token

{
    return GetTknOpType (lptkFcnOpr, IMMTYPE_PRIMOP1, DFNTYPE_OP1);
} // End IsTknOp1


//***************************************************************************
//  $IsTknOp2
//
//  Is the token a dyadic operator?
//***************************************************************************

UBOOL IsTknOp2
    (LPTOKEN lptkFcnOpr)            // Ptr to token

{
    return GetTknOpType (lptkFcnOpr, IMMTYPE_PRIMOP2, DFNTYPE_OP2);
} // End IsTknOp2


//***************************************************************************
//  $GetTknOpType
//
//  Is the token a monadic operator?
//***************************************************************************

UBOOL GetTknOpType
    (LPTOKEN   lptkFcnOpr,          // Ptr to token
     IMM_TYPES immType,             // Immediate type
     DFN_TYPES dfnType)             // UDFO type

{
    UBOOL         bRet = FALSE;     // TRUE iff the result is valid

    // If the token is a function/operator, ...
    if (IsTknFcnOpr (lptkFcnOpr))
    {
        // If the token is immediate, ...
        if (IsTknImmed (lptkFcnOpr))
            // Check the immediate type
            bRet = (GetImmedType (lptkFcnOpr) EQ immType);
        else
        {
            HGLOBAL hGlbData;

            // Get the global handle
            hGlbData = GetGlbHandle (lptkFcnOpr);

            // Check the signature for UDFO
            if (GetSignatureGlb_PTB (hGlbData) EQ DFN_HEADER_SIGNATURE)
            {
                LPDFN_HEADER  lpMemDfnHdr;      // Ptr to user-defined function/operator header ...

                // Lock the memory to get a ptr to it
                lpMemDfnHdr = MyGlobalLockDfn (hGlbData);

                // Get the DFNTYPE_xxx
                bRet = (lpMemDfnHdr->DfnType EQ dfnType);

                // We no longer need this ptr
                MyGlobalUnlock (hGlbData); lpMemDfnHdr = NULL;
            } // End IF/ELSE
        } // End IF/ELSE
    } // End IF

    return bRet;
} // End GetTknOpType


//***************************************************************************
//  $IsTknFcnOpr
//
//  Return TRUE iff the given token is a function or operator
//***************************************************************************

UBOOL IsTknFcnOpr
    (LPTOKEN lptk)              // Ptr to token

{
    switch (TokenTypeFV (lptk))
    {
        case 'F':
        case '1':
        case '2':
        case '3':
            return TRUE;

        case 'V':
            return FALSE;

        defstop
        case '?':
            return 0;
    } // End SWITCH
} // End IsTknFcnOpr


//***************************************************************************
//  $TokenTypeFV
//
//  Return the type of a token
//
//  'F' = Function
//  'V' = Variable
//  '1' = Monadic operator
//  '2' = Dyadic operator
//  '3' = Ambiguous operator
//  '?' = None of the above
//
//***************************************************************************

char TokenTypeFV
    (LPTOKEN lptk)

{
    LPSIS_HEADER lpSISCur;          // Ptr to current SIS

    // Split cases based upon the token type
    switch (lptk->tkFlags.TknType)
    {
        case TKT_VARNAMED:
        case TKT_CHRSTRAND:
        case TKT_NUMSTRAND:
        case TKT_NUMSCALAR:
        case TKT_VARIMMED:
        case TKT_VARARRAY:
        case TKT_AXISIMMED:
        case TKT_AXISARRAY:
        case TKT_LSTIMMED:
        case TKT_LSTARRAY:
        case TKT_LISTINT:
        case TKT_STRAND:
            return 'V';

        case TKT_FCNIMMED:
        case TKT_FCNARRAY:
        case TKT_FCNNAMED:
        case TKT_FILLJOT:
        case TKT_FCNAFO:
            return 'F';

        case TKT_OP1IMMED:
        case TKT_OP1NAMED:
        case TKT_OPJOTDOT:
        case TKT_OP1AFO:
            return '1';

        case TKT_OP2IMMED:
        case TKT_OP2NAMED:
        case TKT_OP2AFO:
            return '2';

        case TKT_OP3IMMED:
        case TKT_OP3NAMED:
            return '3';

        case TKT_DEL:       // Del -- always a function
        case TKT_DELAFO:    // Del -- monadic/dyadic operator, bound to its operands
            return 'F';

        case TKT_DELDEL:    // Del Del -- either a monadic or dyadic function
            // Search up the SIS chain to see what this is
            lpSISCur = SrchSISForDfn (GetMemPTD ());

            // If the ptr is valid, ...
            if (lpSISCur NE NULL)
            {
                // Split case based upon the function type
                switch (lpSISCur->DfnType)
                {
////////////////////case DFNTYPE_FCN:
////////////////////    return 'F';

                    case DFNTYPE_OP1:
                        return '1';

                    case DFNTYPE_OP2:
                        return '2';

                    default:
                        return '?';
                } // End SWITCH
            } // End IF

            return '?';

        case TKT_SYNTERR:
        case TKT_SETALPHA:
        case TKT_GLBDFN:
        case TKT_ASSIGN:
        case TKT_LISTSEP:
        case TKT_LABELSEP:
        case TKT_COLON:
        case TKT_LEFTPAREN:
        case TKT_RIGHTPAREN:
        case TKT_LEFTBRACKET:
        case TKT_RIGHTBRACKET:
        case TKT_LEFTBRACE:
        case TKT_RIGHTBRACE:
        case TKT_EOS:
        case TKT_EOL:
        case TKT_SOS:
        case TKT_LINECONT:
        case TKT_INPOUT:
        case TKT_LISTPAR:
        case TKT_LSTMULT:
        case TKT_STRNAMED:
        case TKT_CS_ANDIF:                  // Control structure:  ANDIF     (Data is Line/Stmt #)
        case TKT_CS_ASSERT:                 // ...                 ASSERT
        case TKT_CS_CASE:                   // ...                 CASE
        case TKT_CS_CASELIST:               // ...                 CASELIST
        case TKT_CS_CONTINUE:               // ...                 CONTINUE
        case TKT_CS_ELSE:                   // ...                 ELSE
        case TKT_CS_ELSEIF:                 // ...                 ELSEIF
        case TKT_CS_END:                    // ...                 END
        case TKT_CS_ENDFOR:                 // ...                 ENDFOR
        case TKT_CS_ENDFORLCL:              // ...                 ENDFORLCL
        case TKT_CS_ENDIF:                  // ...                 ENDIF
        case TKT_CS_ENDREPEAT:              // ...                 ENDREPEAT
        case TKT_CS_ENDSELECT:              // ...                 ENDSELECT
        case TKT_CS_ENDWHILE:               // ...                 ENDWHILE
        case TKT_CS_FOR:                    // ...                 FOR
        case TKT_CS_FOR2:                   // ...                 FOR2
        case TKT_CS_FORLCL:                 // ...                 FORLCL
        case TKT_CS_GOTO:                   // ...                 GOTO
        case TKT_CS_IF:                     // ...                 IF
        case TKT_CS_IF2:                    // ...                 IF2
        case TKT_CS_IN:                     // ...                 IN
        case TKT_CS_LEAVE:                  // ...                 LEAVE
        case TKT_CS_ORIF:                   // ...                 ORIF
        case TKT_CS_REPEAT:                 // ...                 REPEAT
        case TKT_CS_REPEAT2:                // ...                 REPEAT2
        case TKT_CS_RETURN:                 // ...                 RETURN
        case TKT_CS_SELECT:                 // ...                 SELECT
        case TKT_CS_SELECT2:                // ...                 SELECT2
        case TKT_CS_UNTIL:                  // ...                 UNTIL
        case TKT_CS_WHILE:                  // ...                 WHILE
        case TKT_CS_WHILE2:                 // ...                 WHILE2
        case TKT_CS_SKIPCASE:               // ...                 Special token
        case TKT_CS_SKIPEND:                // ...                 Special token
        case TKT_CS_NEC:                    // ...                 Special token
        case TKT_CS_EOL:                    // ...                 Special token
        case TKT_NOP:                       // NOP
        case TKT_AFOGUARD:                  // AFO guard
        case TKT_AFORETURN:                 // AFO return
            return '?';

        defstop
            return '?';
    } // End SWITCH
} // End TokenTypeFV


//***************************************************************************
//  $RankOfGlb
//
//  Get the rank of a global memory object
//***************************************************************************

APLRANK RankOfGlb
    (HGLOBAL hGlb)

{
    LPVARARRAY_HEADER lpMemHdr;
    APLRANK           aplRank;

    // Lock the memory to get a ptr to it
    lpMemHdr = MyGlobalLockVar (hGlb);

    // Get the Rank
    aplRank = lpMemHdr->Rank;

    // We no longer need this ptr
    MyGlobalUnlock (hGlb); lpMemHdr = NULL;

    return aplRank;
} // End RankOfGlb


//***************************************************************************
//  $AttrsOfToken
//
//  Return the attributes (Type, NELM, and Rank) of a token.
//***************************************************************************

void AttrsOfToken
    (LPTOKEN   lpToken,         // Ptr to token
     APLSTYPE *lpaplType,       // Ptr to token storage type (may be NULL)
     LPAPLNELM lpaplNELM,       // ...          NELM (may be NULL)
     LPAPLRANK lpaplRank,       // ...          rank (may be NULL)
     LPAPLDIM  lpaplCols)       // ...          # cols in the array (scalar = 1) (may be NULL)

{
    HGLOBAL hGlbData;           // Global memory handle

    // Split case based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must traverse the array
            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                // Get the global handle
                hGlbData = lpToken->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir_PTB (hGlbData));

                break;      // Continue with HGLOBAL case
            } // End IF

            if (lpaplType NE NULL)
                *lpaplType = TranslateImmTypeToArrayType (lpToken->tkData.tkSym->stFlags.ImmType);
            if (lpaplNELM NE NULL)
                *lpaplNELM = 1;
            if (lpaplRank NE NULL)
                *lpaplRank = 0;
            if (lpaplCols NE NULL)
                *lpaplCols = 1;
            return;

        case TKT_VARIMMED:
        case TKT_LSTIMMED:
        case TKT_AXISIMMED:
            if (lpaplType NE NULL)
                *lpaplType = TranslateImmTypeToArrayType (lpToken->tkFlags.ImmType);
            if (lpaplNELM NE NULL)
                *lpaplNELM = 1;
            if (lpaplRank NE NULL)
                *lpaplRank = 0;
            if (lpaplCols NE NULL)
                *lpaplCols = 1;
            return;

        case TKT_CHRSTRAND:
        case TKT_NUMSTRAND:
        case TKT_NUMSCALAR:
        case TKT_VARARRAY:
        case TKT_LISTPAR:
        case TKT_LSTARRAY:
        case TKT_AXISARRAY:
            // Get the global handle
            hGlbData = lpToken->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (hGlbData));

            break;      // Continue with HGLOBAL case

        case TKT_LSTMULT:
            // Get the global handle
            hGlbData = lpToken->tkData.tkGlbData;

            // tkData is a valid HGLOBAL list array
            Assert (IsGlbTypeLstDir_PTB (hGlbData));

            break;      // Continue with HGLOBAL case

        defstop
            return;
    } // End SWITCH

    // Get the attributes (Type, NELM, and Rank) of the global
    AttrsOfGlb (hGlbData, lpaplType, lpaplNELM, lpaplRank, lpaplCols);
} // End AttrsOfToken


//***************************************************************************
//  $AttrsOfGlb
//
//  Return the attributes (Type, NELM, and Rank) of a global memory handle.
//***************************************************************************

void AttrsOfGlb
    (HGLOBAL   hGlbData,        // Memory handle
     APLSTYPE *lpaplType,       // Ptr to token storage type (may be NULL)
     LPAPLNELM lpaplNELM,       // ...          NELM (may be NULL)
     LPAPLRANK lpaplRank,       // ...          rank (may be NULL)
     LPAPLUINT lpaplCols)       // ...          # cols in the array (scalar = 1) (may be NULL)

{
    LPVOID  lpMemData;          // Ptr to global memory

    // Lock the memory to get a ptr to it
    lpMemData = MyGlobalLock (hGlbData);    // LST, VAR

    // Split cases based upon the array signature
    switch (GetSignatureMem (lpMemData))
    {
        case LSTARRAY_HEADER_SIGNATURE:
        {
#ifdef DEBUG
            LPLSTARRAY_HEADER lpHeader = (LPLSTARRAY_HEADER) lpMemData;
#else
  #define lpHeader    ((LPLSTARRAY_HEADER) lpMemData)
#endif
            if (lpaplType NE NULL)
                *lpaplType = ARRAY_LIST;
            if (lpaplNELM NE NULL)
                *lpaplNELM = lpHeader->NELM;
            if (lpaplRank NE NULL)
                *lpaplRank = 1;
            if (lpaplCols NE NULL)
                *lpaplCols = lpHeader->NELM;

            break;
        } // End LSTARRAY_HEADER_SIGNATURE
#ifndef DEBUG
  #undef  lpHeader
#endif

        case VARARRAY_HEADER_SIGNATURE:
        {
#ifdef DEBUG
            LPVARARRAY_HEADER lpHeader = (LPVARARRAY_HEADER) lpMemData;
#else
  #define lpHeader    ((LPVARARRAY_HEADER) lpMemData)
#endif
            if (lpaplType NE NULL)
                *lpaplType = lpHeader->ArrType;
            if (lpaplNELM NE NULL)
                *lpaplNELM = lpHeader->NELM;
            if (lpaplRank NE NULL)
                *lpaplRank = lpHeader->Rank;
            if (lpaplCols NE NULL)
            {
                if (!IsScalar (lpHeader->Rank))
                    // Skip over the header to the dimensions
                    *lpaplCols = (VarArrayBaseToDim (lpHeader))[lpHeader->Rank - 1];
                else
                    *lpaplCols = 1;
            } // End IF

            break;
        } // End VARARRAY_HEADER_SIGNATURE
#ifndef DEBUG
  #undef  lpHeader
#endif

        case DFN_HEADER_SIGNATURE:
        case FCNARRAY_HEADER_SIGNATURE:
        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMemData = NULL;
} // End AttrsOfGlb


//***************************************************************************
//  $StorageType
//
//  Return the storage type for a dyadic scalar function
//    given the left and right storage types and the function.
//***************************************************************************

APLSTYPE StorageType
    (APLSTYPE aplTypeLft,   // Storage type of left arg
     LPTOKEN  lptkFunc,     // Immediate function
     APLSTYPE aplTypeRht)   // Storage type of right arg

{
    APLSTYPE aplTypeRes;

    // Both arguments are within bounds
    Assert (aplTypeLft < ARRAY_LENGTH);
    Assert (aplTypeRht < ARRAY_LENGTH);

    // tkData is an immediate function
    Assert (lptkFunc->tkFlags.TknType EQ TKT_FCNIMMED);

    // Get the common storage type
    aplTypeRes = aTypePromote[aplTypeLft][aplTypeRht];

    // Split cases based upon the result type
    switch (aplTypeRes)
    {
        case ARRAY_ERROR:
        case ARRAY_RAT:
        case ARRAY_VFP:
        case ARRAY_NESTED:
            return aplTypeRes;

        case ARRAY_HETERO:
            // A mixed result is from CHAR or HETERO vs. numeric (BOOL, INT, APA, FLOAT) or NESTED
            // The result is BOOL or NESTED if the function is equal or not-equal, otherwise
            //   the result is ERROR.
            if (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
             || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL)
            {
                if (IsNested (aplTypeLft)
                 || IsNested (aplTypeRht))
                    return ARRAY_NESTED;
                else
                    return ARRAY_BOOL;
            } else
                return ARRAY_ERROR;

        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
            if (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
             || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL)
                return ARRAY_BOOL;
            else
                return aplTypeRes;

        case ARRAY_CHAR:
            if (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
             || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL)
                return ARRAY_BOOL;
            else
                return ARRAY_ERROR;

        case ARRAY_APA:
        defstop
            return ARRAY_ERROR;
    } // End SWITCH
} // StorageType


//***************************************************************************
//  $IncrOdometer
//
//  Increment the odometer in lpMemOdo subject to the
//    limits in lpMemDim[lpMemAxisHead] or lpMemDim, all
//    of length <aplRank>.
//  Return TRUE iff we wrapped back to all 0s.
//***************************************************************************

UBOOL IncrOdometer
    (LPAPLUINT lpMemOdo,        // Ptr to the values to increment
     LPAPLDIM  lpMemDim,        // Ptr to the limits on each dimension
     LPAPLUINT lpMemAxisHead,   // Ptr to axis values, fleshed out by CheckAxis_EM (may be NULL)
     APLRANK   aplRank)         // # elements to which each of the
                                //   ptrs point

{
    APLINT iOdo;

    // Loop through the odometer values from right to left
    // Note we use a signed index variable because we're
    //   walking backwards and the test against zero must be
    //   made as a signed variable.
    if (lpMemAxisHead NE NULL)
    {
        for (iOdo = aplRank - 1; iOdo >= 0; iOdo--)
        if (++lpMemOdo[iOdo] >= lpMemDim[lpMemAxisHead[iOdo]])
            lpMemOdo[iOdo] = 0;
        else
            return FALSE;
    } else
    {
        for (iOdo = aplRank - 1; iOdo >= 0; iOdo--)
        if (++lpMemOdo[iOdo] >= lpMemDim[iOdo])
            lpMemOdo[iOdo] = 0;
        else
            return FALSE;
    } // End IF/ELSE

    return (iOdo < 0);
} // End IncrOdometer


//***************************************************************************
//  $PrimScalarFnDydAllocate_EM
//
//  Allocate space for result of a primitive scalar dyadic function
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimScalarFnDydAllocate_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimScalarFnDydAllocate_EM
    (LPTOKEN           lptkFunc,        // Ptr to function token

     HGLOBAL          *lphGlbRes,       // Return HGLOBAL of the result

     LPVARARRAY_HEADER lpMemHdrLft,     // Ptr to left arg memory (may be NULL if monadic)
     LPVARARRAY_HEADER lpMemHdrRht,     // ...    right ...

     APLRANK           aplRankLft,      // Left arg rank
     APLRANK           aplRankRht,      // Right ...
     LPAPLRANK         lpaplRankRes,    // Ptr to result rank

     APLSTYPE          aplTypeRes,      // Result type

     UBOOL             bLftIdent,       // TRUE iff the function has a left identity element and the Axis tail is valid
     UBOOL             bRhtIdent,       // ...                         right ...

     APLNELM           aplNELMLft,      // Left arg NELM
     APLNELM           aplNELMRht,      // Right ...
     APLNELM           aplNELMRes)      // Result ...

{
    APLUINT           ByteRes;          // # bytes in the result
    LPVARARRAY_HEADER lpMemHdrRes,      // Ptr to result header
                      lpMemHdrCom;      // Ptr to common header
    APLUINT           uRes;             // Loop counter

    // Initialize the result
    *lphGlbRes = NULL;

    // Split cases based upon the singleton status
    //   of the left & right args
    if (IsSingleton (aplNELMLft)
     && IsSingleton (aplNELMRht))
    {
        //***************************************************************
        // Both args are singletons
        //***************************************************************

        // The rank of the result is the larger of the two args
        *lpaplRankRes = max (aplRankLft, aplRankRht);

        // If it's a simple numeric scalar, it'll
        //   be stored as an immediate token,
        //   not global memory.
        if (IsScalar (*lpaplRankRes)
         && IsSimpleNum (aplTypeRes))
            return TRUE;
    } else
    if (IsSingleton (aplNELMLft)
     || IsSingleton (aplNELMRht))
    {
        //***************************************************************
        // One of the args is a singleton, the other not
        //***************************************************************

        // The rank of the result is the rank of the non-singleton
        if (!IsSingleton (aplNELMLft))
            *lpaplRankRes = aplRankLft;
        else
            *lpaplRankRes = aplRankRht;
    } else
    {
        //***************************************************************
        // Neither arg is a singleton
        //***************************************************************

        // The rank of the result is the larger of the two args
        *lpaplRankRes = max (aplRankLft, aplRankRht);
    } // End IF/ELSE/...

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, *lpaplRankRes);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result.
    *lphGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (*lphGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLock000 (*lphGlbRes);

#define lpHeader    lpMemHdrRes
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = *lpaplRankRes;
#undef  lpHeader

    // Fill in the dimensions
    if (IsSingleton (aplNELMLft)
     && IsSingleton (aplNELMRht))
    {
        // Fill in the dimensions (all 1)
        for (uRes = 0; uRes < *lpaplRankRes; uRes++)
            (VarArrayBaseToDim (lpMemHdrRes))[uRes] = 1;
    } else
    if (IsSingleton (aplNELMLft)
     || IsSingleton (aplNELMRht))
    {
        // Copy the ptr of the non-singleton argument
        if (!IsSingleton (aplNELMLft))
            lpMemHdrCom = lpMemHdrLft;
        else
            lpMemHdrCom = lpMemHdrRht;

        // Copy the dimensions from the non-singleton arg
        //   to the result
        for (uRes = 0; uRes < *lpaplRankRes; uRes++)
            (VarArrayBaseToDim (lpMemHdrRes))[uRes] = (VarArrayBaseToDim (lpMemHdrCom))[uRes];
    } else
    {
        // Get the higher rank's pointer
        //   to copy its dimensions to the result
        if (aplRankLft > aplRankRht)
            lpMemHdrCom = lpMemHdrLft;
        else
        if (aplRankLft < aplRankRht)
            lpMemHdrCom = lpMemHdrRht;
        else
        {
            // If there's right identity element only, ...
            if (!bLftIdent && bRhtIdent)
                // Copy the left arg dimensions
                lpMemHdrCom = lpMemHdrLft;
            else
                // Otherwise, copy the right arg dimensions
                lpMemHdrCom = lpMemHdrRht;
        } // End IF/ELSE/...

        // Copy the dimensions to the result
        for (uRes = 0; uRes < *lpaplRankRes; uRes++)
            (VarArrayBaseToDim (lpMemHdrRes))[uRes] = (VarArrayBaseToDim (lpMemHdrCom))[uRes];
    } // End IF/ELSE/...

    // We no longer need this ptr
    MyGlobalUnlock (*lphGlbRes); lpMemHdrRes =  NULL;

    return TRUE;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    return FALSE;
} // End PrimScalarFnDydAllocate_EM
#undef  APPEND_NAME


//***************************************************************************
//  $MakeMonPrototype_EM_PTB
//
//  Make a prototype copy of a single global memory object
//    whose value is sensitive to Ptr Type Bits.
//  The result might be a LPSYMENTRY or HGLOBAL
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeMonPrototype_EM_PTB"
#else
#define APPEND_NAME
#endif

HGLOBAL MakeMonPrototype_EM_PTB
    (HGLOBAL    hGlbArr,            // Incoming array handle
     LPTOKEN    lptkFunc,           // Ptr to function token
     MAKE_PROTO mpEnum)             // See MAKE_PROTO

{
    HGLOBAL           hGlbTmp,              // Temporary global memory handle
                      hGlbRes,              // Result    ...
                      hSymGlbProto;         // Prototype ...
    LPVARARRAY_HEADER lpMemHdrArr = NULL,   // Ptr to array header
                      lpMemHdrRes = NULL,   // ...    result ...
                      lpMemHdrTmp = NULL;   // ...    temp   ...
    LPVOID            lpMemArr,             // Ptr to array global memory
                      lpMemRes;             // ...
    APLSTYPE          aplType;
    APLNELM           aplNELM;
    APLRANK           aplRank;
    UINT              u;
    APLNELM           uLen;
    UBOOL             bRet = TRUE;          // TRUE iff result is valid
    APLUINT           ByteRes;              // # bytes in the result
    LPSYMENTRY        lpSymArr,
                      lpSymRes;
    LPPERTABDATA      lpMemPTD;             // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Split cases based upon the ptr type
    switch (GetPtrTypeDir (hGlbArr))
    {
        case PTRTYPE_STCONST:
            // If it's numeric, ...
            if (IsImmNum (((LPSYMENTRY) hGlbArr)->stFlags.ImmType))
                return lpMemPTD->lphtsGLB->steZero;
            else
                return lpMemPTD->lphtsGLB->steBlank;

        case PTRTYPE_HGLOBAL:
            break;

        defstop
            break;
    } // End SWITCH

    // Make a copy of the array as we're changing it
    hGlbArr = CopyArray_EM (hGlbArr,
                            lptkFunc);
    if (hGlbArr EQ NULL)
        return NULL;

    // Lock the memory to get a ptr to it
    lpMemHdrArr = MyGlobalLockVar (hGlbArr);

#define lpHeader        lpMemHdrArr
    aplType = lpHeader->ArrType;
    aplNELM = lpHeader->NELM;
    aplRank = lpHeader->Rank;
#undef  lpHeader

    // Point to the data
    lpMemArr = VarArrayDataFmBase (lpMemHdrArr);

    // Split cases based upon the array type
    switch (aplType)
    {
        case ARRAY_BOOL:
            // Calculate # bytes of Boolean data
            uLen = RoundUpBitsToBytes (aplNELM);

            // Zero the memory
            ZeroMemory (lpMemArr, (APLU3264) uLen);

            break;

        case ARRAY_INT:
            // Zero the memory
            ZeroMemory (lpMemArr, (APLU3264) aplNELM * sizeof (APLINT));

            break;

        case ARRAY_FLOAT:
            // Zero the memory
            ZeroMemory (lpMemArr, (APLU3264) aplNELM * sizeof (APLFLOAT));

            break;

        case ARRAY_CHAR:
            // Split cases based upon mpEnum
            switch (mpEnum)
            {
                case MP_CHARS:
                    break;

                case MP_NUMONLY:
                    // If the arg is non-empty, that's an error
                    if (!IsEmpty (aplNELM))
                        goto DOMAIN_ERROR_EXIT;

                    // Change the storage type to Boolean
                    lpMemHdrArr->ArrType = ARRAY_BOOL;

                    break;

                case MP_NUMCONV:
                    // Convert the chars to numerics by allocating
                    //   new (Boolean) storage and copying over
                    //   the dimensions

                    // Calculate space needed for the result
                    ByteRes = CalcArraySize (ARRAY_BOOL, aplNELM, aplRank);

                    // Check for overflow
                    if (ByteRes NE (APLU3264) ByteRes)
                        goto WSFULL_EXIT;

                    // Allocate space for the result.
                    hGlbTmp = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
                    if (hGlbTmp EQ NULL)
                        goto WSFULL_EXIT;

                    // Lock the memory to get a ptr to it
                    lpMemHdrTmp = MyGlobalLock000 (hGlbTmp);

#define lpHeader    lpMemHdrTmp
                    // Fill in the header
                    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
                    lpHeader->ArrType    = ARRAY_BOOL;
////////////////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////////////////lpHeader->SysVar     = FALSE;       // Already zero from GHND
                    lpHeader->RefCnt     = 1;
                    lpHeader->NELM       = aplNELM;
                    lpHeader->Rank       = aplRank;
#undef  lpHeader

                    // Copy the dimensions to the result
                    CopyMemory (VarArrayBaseToDim (lpMemHdrTmp),
                                VarArrayBaseToDim (lpMemHdrArr),
                                (APLU3264) aplRank * sizeof (APLDIM));

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbTmp); lpMemHdrTmp = NULL;

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbArr); lpMemHdrArr = NULL;

                    // We no longer need this storage
                    FreeResultGlobalVar (hGlbArr); hGlbArr = NULL;

                    // Copy the global handle
                    hGlbArr = hGlbTmp;

                    goto NORMAL_EXIT;

                defstop
                    break;
            } // End SWITCH

            // Check for overflow
            if (aplNELM NE (APLU3264) aplNELM)
                goto WSFULL_EXIT;

            // Convert the chars to blanks
            FillMemoryW (lpMemArr, (APLU3264) aplNELM, L' ');

            break;

        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemArr)
            // To make a prototype APA, set its
            //   offset and multiplier to zero
            lpAPA->Off =
            lpAPA->Mul = 0;
#undef  lpAPA
            break;

        case ARRAY_NESTED:
            // Empty nested arrays have one element (the prototype)
            aplNELM = max (aplNELM, 1);

            // Fall through to common HETERO/NESTED code

        case ARRAY_HETERO:
            for (u = 0; bRet && u < aplNELM; u++)
            // Split cases based upon the element's ptr type
            switch (GetPtrTypeInd (lpMemArr))
            {
                case PTRTYPE_STCONST:
                    lpSymArr = *(LPAPLHETERO) lpMemArr;

                    // It's an immediate
                    Assert (lpSymArr->stFlags.Imm);

                    // Split cases based upon the immediate type
                    switch (lpSymArr->stFlags.ImmType)
                    {
                        case IMMTYPE_BOOL:
                        case IMMTYPE_INT:
                        case IMMTYPE_FLOAT:
                            lpSymRes = lpMemPTD->lphtsGLB->steZero;

                            break;

                        case IMMTYPE_CHAR:
                            // Split cases based upon mpEnum
                            switch (mpEnum)
                            {
                                case MP_CHARS:
                                    lpSymRes = lpMemPTD->lphtsGLB->steBlank;

                                    break;

                                case MP_NUMONLY:
                                    goto DOMAIN_ERROR_EXIT;

                                    break;

                                case MP_NUMCONV:
                                    lpSymRes = lpMemPTD->lphtsGLB->steZero;

                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        defstop
                            goto ERROR_EXIT;
                    } // End SWITCH

                    if (lpSymRes NE NULL)
                        // Save back into array
                        *((LPAPLHETERO) lpMemArr)++ = MakePtrTypeSym (lpSymRes);
                    else
                        goto SYMTAB_ERROR_EXIT;
                    break;

                case PTRTYPE_HGLOBAL:
                    Assert (IsPtrArray (aplType));

                    // It's a valid HGLOBAL array
                    Assert (IsGlbTypeVarInd_PTB (lpMemArr));

                    hSymGlbProto =
                      MakeMonPrototype_EM_PTB (*(LPAPLNESTED) lpMemArr, // Proto arg handle
                                               lptkFunc,                // Ptr to function token
                                               mpEnum);                 // Pass flag through
                    if (hSymGlbProto NE NULL)
                    {
                        // We no longer need this storage
                        FreeResultGlobalIncompleteVar (ClrPtrTypeInd (lpMemArr)); *((LPAPLNESTED) lpMemArr) = NULL;

                        *((LPAPLNESTED) lpMemArr)++ = hSymGlbProto;
                    } else
                        bRet = FALSE;
                    break;

                defstop
                    break;
            } // End FOR/SWITCH

            break;

        case ARRAY_RAT:
        case ARRAY_VFP:
            // Calculate space needed for the result
            ByteRes = CalcArraySize (aplType, aplNELM, aplRank);

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
            lpHeader->ArrType    = aplType;
////////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
            lpHeader->RefCnt     = 1;
            lpHeader->NELM       = aplNELM;
            lpHeader->Rank       = aplRank;
#undef  lpHeader
            // Skip over the header to the dimensions
            lpMemRes = VarArrayBaseToDim (lpMemHdrRes);

            // Copy the dimensions
            CopyMemory (lpMemRes, VarArrayBaseToDim (lpMemHdrArr), (APLU3264) aplRank * sizeof (APLRANK));

            // Skip over the dimensions to the data
            lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

            // Loop through the result setting it to zero
            for (u = 0; u < aplNELM; u++)
            // Split cases based upon the storage type
            switch (aplType)
            {
                case ARRAY_RAT:
                    // Initialize to 0/1
                    mpq_init   (((LPAPLRAT) lpMemRes)++);

                    break;

                case ARRAY_VFP:
                    // Initialize to 0
                    mpfr_init0 (((LPAPLVFP) lpMemRes)++);

                    break;

                defstop
                    break;
            } // End FOR/SWITCH

            // We no longer need these ptrs
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
            MyGlobalUnlock (hGlbArr); lpMemHdrArr = NULL;

            // We no longer need this storage
            FreeResultGlobalVar (hGlbArr); hGlbArr = NULL;

            // Copy to result var
            hGlbArr = hGlbRes;

            break;

        defstop
            break;
    } // End SWITCH

    goto NORMAL_EXIT;

DOMAIN_ERROR_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

SYMTAB_ERROR_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYMBOL_TABLE_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    bRet = FALSE;
NORMAL_EXIT:
    if (hGlbArr NE NULL && lpMemHdrArr NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbArr); lpMemHdrArr = NULL;
    } // End IF

    if (!bRet)
    {
        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbArr); hGlbArr = NULL;
    } // End IF

    if (hGlbArr NE NULL)
        return MakePtrTypeGlb (hGlbArr);
    else
        return hGlbArr;
} // End MakeMonPrototype_EM_PTB
#undef  APPEND_NAME


//***************************************************************************
//  $MakeDydPrototype_EM_PTB
//
//  Make a prototype from a dyadic scalar function between
//    two global memory objects whose values are sensitive
//    to Ptr Type Bits.
//
//  Instead of returning one of the args as the result,
//    we create a result by trundling through the args
//    using scalar extension as well as checking for
//    RANK, LENGTH, and DOMAIN ERRORs between the args.
//
//  For example, adding empty vectors whose prototypes are
//    0 (0 0) and (0 0) 0 should produce an empty vector
//    whose prototype is (0 0) (0 0) by scalar extension.
//  Also, adding empty vectors whose prototypes are
//    (0 0) and (0 0 0) should produce a LENGTH ERROR.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeDydPrototype_EM_PTB"
#else
#define APPEND_NAME
#endif

HGLOBAL MakeDydPrototype_EM_PTB
    (HGLOBAL   hGlbLft,                 // Left arg global memory handle (may be NULL if immediate)
     IMM_TYPES immTypeLft,              // Left arg storage type if immediate (see IMM_TYPES)
     LPTOKEN   lptkFunc,                // Ptr to function token
     HGLOBAL   hGlbRht,                 // Right arg global memory handle (may be NULL if immediate)
     IMM_TYPES immTypeRht,              // Right arg storage type if immediate (see IMM_TYPES)
     LPTOKEN   lptkAxis)                // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeLft,           // Left arg storage type
                      aplTypeRht,           // Right ...
                      aplTypeRes;           // Result ...
    APLNELM           aplNELMLft,           // Left arg NELM
                      aplNELMRht,           // Right ...
                      aplNELMRes,           // Result   ...
                      aplNELMAxis;          // Axis value ...
    APLRANK           aplRankLft,           // Left arg rank
                      aplRankRht,           // Right ...
                      aplRankRes;           // Result ...
    HGLOBAL           hGlbRes = NULL,       // Result global memory handle
                      hGlbAxis = NULL,      // Axis value ...
                      hGlbWVec = NULL,      // Weighting vector ...
                      hGlbOdo = NULL,       // Odometer ...
                      hGlbSub;              // Subarray ...
    LPVARARRAY_HEADER lpMemHdrLft = NULL,   // Ptr to left arg header
                      lpMemHdrRht = NULL,   // ...    right ...
                      lpMemHdrRes = NULL;   // ...    result ...
    LPAPLNESTED       lpMemLft,             // Ptr to left arg global memory
                      lpMemRht,             // Ptr to right ...
                      lpMemRes;             // Ptr to result   ...
    LPAPLDIM          lpMemDimRes;          // Ptr to result dimensions
    APLUINT           ByteRes,              // # bytes in the result
                      uLft,                 // Loop counter
                      uRht,                 // Loop counter
                      uRes;                 // Loop counter
    APLINT            iDim;                 // Dimension loop counter
    UBOOL             bBoolFn,              // TRUE iff the function is equal or not-equal
                      bLftIdent,            // TRUE iff the function has a left identity element and the Axis tail is valid
                      bRhtIdent;            // ...                         right ...
    LPPRIMSPEC        lpPrimSpec;           // Ptr to local PRIMSPEC
    LPAPLUINT         lpMemAxisHead = NULL, // Ptr to axis values, fleshed out by CheckAxis_EM
                      lpMemAxisTail = NULL, // Ptr to grade up of AxisHead
                      lpMemOdo = NULL,      // Ptr to odometer global memory
                      lpMemWVec = NULL;     // Ptr to weighting vector ...
    LPPRIMFLAGS       lpPrimFlags;          // Ptr to function PrimFlags entry

    // Get the attributes (Type, NELM, and Rank)
    //   of the left arg
    if (hGlbLft NE NULL)        // If not immediate, ...
        AttrsOfGlb (hGlbLft, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    else
    {
        aplTypeLft = TranslateImmTypeToArrayType (immTypeLft);
        aplNELMLft = 1;
        aplRankLft = 0;
    } // End IF

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    if (hGlbRht NE NULL)        // If not immediate, ...
        AttrsOfGlb (hGlbRht, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);
    else
    {
        aplTypeRht = TranslateImmTypeToArrayType (immTypeRht);
        aplNELMRht = 1;
        aplRankRht = 0;
    } // End IF

    // Handle scalar extension with empty other arg
    if (IsEmpty (aplNELMLft)
     && hGlbRht EQ NULL)
        aplTypeRht = ARRAY_BOOL;
    if (IsEmpty (aplNELMRht)
     && hGlbLft EQ NULL)
        aplTypeLft = ARRAY_BOOL;

    // The rank of the result is the larger of the two args
    //   unless one is a singleton.
    if ( IsSingleton (aplNELMLft) && !IsSingleton (aplNELMRht) && aplRankLft > aplRankRht)
        aplRankRes = aplRankRht;
    else
    if (!IsSingleton (aplNELMLft) &&  IsSingleton (aplNELMRht) && aplRankLft < aplRankRht)
        aplRankRes = aplRankLft;
    else
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

    // Get the corresponding lpPrimSpec
    lpPrimSpec = PrimSpecTab[SymTrans (lptkFunc)];

    // Calculate the storage type of the result
    aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (aplNELMLft,
                                               &aplTypeLft,
                                                lptkFunc,
                                                aplNELMRht,
                                               &aplTypeRht);
    if (IsErrorType (aplTypeRes))
        goto DOMAIN_EXIT;

    Assert (IsSimpleGlbNum (aplTypeRes)
         || IsNested (aplTypeRes));

    // Lock the memory to get a ptr to it
    if (hGlbLft NE NULL)
        lpMemHdrLft = MyGlobalLockVar (hGlbLft);

    // Lock the memory to get a ptr to it
    if (hGlbRht NE NULL)
        lpMemHdrRht = MyGlobalLockVar (hGlbRht);

    // Get a ptr to the Primitive Function Flags
    lpPrimFlags = GetPrimFlagsPtr (lptkFunc);

    // Set the identity element bits
    bLftIdent = lpPrimFlags->bLftIdent
             && (lpMemAxisTail NE NULL);
    bRhtIdent = lpPrimFlags->bRhtIdent
             && (lpMemAxisTail NE NULL);

    // Check for RANK and LENGTH ERRORs
    if (!CheckRankLengthError_EM (aplRankRes,       // Result rank
                                  aplRankLft,       // Left arg rank
                                  aplNELMLft,       // Left arg NELM
                                  lpMemHdrLft,      // Ptr to left arg global memory
                                  aplRankRht,       // Right arg rank
                                  aplNELMRht,       // Right arg NELM
                                  lpMemHdrRht,      // Ptr to right arg global memory
                                  aplNELMAxis,      // Axis NELM
                                  lpMemAxisTail,    // Ptr to grade up of AxisHead
                                  bLftIdent,        // TRUE iff the function has a left identity element
                                  bRhtIdent,        // ...                         right ...
                                  lptkFunc))        // Ptr to the function token
        goto ERROR_EXIT;

    // The NELM of the result is the larger of the two args
    //   unless one is empty
    if (IsEmpty (aplNELMLft) || IsEmpty (aplNELMRht))
        aplNELMRes = 0;
    else
        aplNELMRes = max (aplNELMLft, aplNELMRht);

    // Handle APA result separately
    if (IsSimpleAPA (aplTypeRes))
    {
        // Calculate space needed for the result
        ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

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
////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = aplNELMRes;
        lpHeader->Rank       = aplRankRes;
#undef  lpHeader

        // Copy the dimensions to the result
        if (aplRankLft < aplRankRht)
            // Copy from the right
            CopyMemory (VarArrayBaseToDim (lpMemHdrRes),
                        VarArrayBaseToDim (lpMemHdrRht),
                        (APLU3264) aplNELMRes * sizeof (APLDIM));
        else
        if (aplRankLft > aplRankRht)
            // Copy from the left
            CopyMemory (VarArrayBaseToDim (lpMemHdrRes),
                        VarArrayBaseToDim (lpMemHdrLft),
                        (APLU3264) aplNELMRes * sizeof (APLDIM));
        else
        {
            // If the function has a right-hand identity element only, ...
            if (bRhtIdent && !bLftIdent)
                // Copy from the left
                CopyMemory (VarArrayBaseToDim (lpMemHdrRes),
                            VarArrayBaseToDim (lpMemHdrLft),
                            (APLU3264) aplNELMRes * sizeof (APLDIM));
            else
                // Copy from the right
                CopyMemory (VarArrayBaseToDim (lpMemHdrRes),
                            VarArrayBaseToDim (lpMemHdrRht),
                            (APLU3264) aplNELMRes * sizeof (APLDIM));
        } // End IF/ELSE/...

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

#define lpAPA       ((LPAPLAPA) lpMemRes)
        // To make a prototype APA, set its
        //   offset and multiplier to zero
        lpAPA->Off =
        lpAPA->Mul = 0;
#undef  lpAPA

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = (LPVOID) lpMemRes = NULL;

        // Set the ptr type bits
        hGlbRes = MakePtrTypeGlb (hGlbRes);

        goto NORMAL_EXIT;
    } // End IF

    // If the result is simple numeric,
    //   convert it to Boolean
    if (IsSimpleNum (aplTypeRes))
        aplTypeRes = ARRAY_BOOL;

    // Get the function type
    bBoolFn = lptkFunc->tkData.tkChar EQ UTF16_EQUAL
           || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL;

    // If the left arg is immediate, the result is the prototype of the other arg
    if (hGlbLft EQ NULL)
        hGlbRes =
          MakeMonPrototype_EM_PTB (hGlbRht,     // Proto arg handle
                                   lptkFunc,    // Ptr to function token
                                   bBoolFn ? MP_NUMCONV : MP_NUMONLY);
    else
    if (hGlbRht EQ NULL)
        hGlbRes =
          MakeMonPrototype_EM_PTB (hGlbLft,     // Proto arg handle
                                   lptkFunc,    // Ptr to function token
                                   bBoolFn ? MP_NUMCONV : MP_NUMONLY);
    else
    {
        // Neither arg is immediate

        // Allocate space for result
        if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                        &hGlbRes,
                                         lpMemHdrLft,
                                         lpMemHdrRht,
                                         aplRankLft,
                                         aplRankRht,
                                        &aplRankRes,
                                         aplTypeRes,
                                         bLftIdent,     // TRUE iff the function has a left identity element and the Axis tail is valid
                                         bRhtIdent,     // ...                         right ...
                                         aplNELMLft,
                                         aplNELMRht,
                                         aplNELMRes))
            goto ERROR_EXIT;

        // Take into account nested prototypes
        if (IsNested (aplTypeLft))
            aplNELMLft = max (aplNELMLft, 1);
        if (IsNested (aplTypeRht))
            aplNELMRht = max (aplNELMRht, 1);
        if (IsNested (aplTypeRes))
            aplNELMRes = max (aplNELMRes, 1);

        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLockVar (hGlbRes);

        // Skip over the header to the dimensions
        lpMemDimRes = VarArrayBaseToDim (lpMemHdrRes);

        // Skip over the header and dimensions to the data
        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

        // If either arg is neither simple nor global numeric, loop through the result
        // Otherwise, the result is all zero (already filled in by GHND).
        if (!IsSimpleGlbNum (aplTypeLft)
         || !IsSimpleGlbNum (aplTypeRht))
        {
            // Handle axis if present
            if (aplNELMAxis NE aplRankRes)
            {
                // Calculate space needed for the weighting vector
                ByteRes = aplRankRes * sizeof (APLUINT);

                // Check for overflow
                if (ByteRes NE (APLU3264) ByteRes)
                    goto WSFULL_EXIT;

                //***************************************************************
                // Allocate space for the weighting vector which is
                //   {times}{backscan}1{drop}({rho}Z),1
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
                for (uRes = 1, iDim = aplRankRes - 1; iDim >= 0; iDim--)
                {
                    lpMemWVec[iDim] = uRes;
                    uRes *= lpMemDimRes[iDim];
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
            } // End IF

            // Loop through the elements of the result
            for (uRes = 0; uRes < aplNELMRes; uRes++)
            {
                // If there's an axis, ...
                if (lptkAxis NE NULL
                 && aplNELMAxis NE aplRankRes)
                {
                    APLUINT uArg;       // Loop counter

                    // Loop through the odometer values accumulating
                    //   the weighted sum
                    for (uArg = 0, uRht = aplRankRes - aplNELMAxis; uRht < aplRankRes; uRht++)
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
                    // Handle 0 modulus as C fails miserably
                    uLft = mod64 (uRes, aplNELMLft);
                    uRht = mod64 (uRes, aplNELMRht);
                } // End IF/ELSE

                // If the left arg element is simple or an STE,
                //   the result element is the prototype
                //   of the right arg element
                if (IsSimple (aplTypeLft)
                 || GetPtrTypeDir (lpMemLft[uLft]) EQ PTRTYPE_STCONST)
                {
                    hGlbSub =
                      MakeMonPrototype_EM_PTB (lpMemRht[uRht],                  // Proto arg handle
                                               lptkFunc,                        // Ptr to function token
                                               bBoolFn ? MP_NUMCONV : MP_NUMONLY);
                    if (hGlbSub EQ NULL)
                        goto ERROR_EXIT;
                    *lpMemRes++ = hGlbSub;
                } else
                // If the right arg element is simple or an STE,
                //   the result element is the prototype
                //   of the left arg element
                if (IsSimple (aplTypeRht)
                 || GetPtrTypeDir (lpMemRht[uRht]) EQ PTRTYPE_STCONST)
                {
                    hGlbSub =
                      MakeMonPrototype_EM_PTB (lpMemLft[uLft],                  // Proto arg handle
                                               lptkFunc,                        // Ptr to function token
                                               bBoolFn ? MP_NUMCONV : MP_NUMONLY);
                    if (hGlbSub EQ NULL)
                        goto ERROR_EXIT;
                    *lpMemRes++ = hGlbSub;
                } else
                {
                    // Both args are nested HGLOBALs
                    hGlbSub =
                      MakeDydPrototype_EM_PTB (lpMemLft[uLft],  // Left arg proto handle
                                               0,               // Left arg immediate type (irrelevant as it's an HGLOBAL)
                                               lptkFunc,        // Ptr to function token
                                               lpMemRht[uRht],  // Right arg proto handle
                                               0,               // Right arg immediate type (irrelevant as it's an HGLOBAL)
                                               NULL);           // Ptr to axis token (may be NULL)
                    if (hGlbSub EQ NULL)
                        goto ERROR_EXIT;
                    *lpMemRes++ = hGlbSub;
                } // End IF/ELSE/...
            } // End FOR
        } // End IF

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

        // Set the ptr type bits
        hGlbRes = MakePtrTypeGlb (hGlbRes);
    } // End IF/ELSE/...

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

    if (hGlbLft NE NULL && lpMemHdrLft NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;
    } // End IF

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbWVec, lpMemWVec);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbOdo, lpMemOdo);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbAxis, lpMemAxisHead);

    return hGlbRes;
} // End MakeDydPrototype_EM_PTB
#undef  APPEND_NAME


//***************************************************************************
//  $IsFirstSimpleGlb
//
//  Is the first element of a global simple?
//***************************************************************************

UBOOL IsFirstSimpleGlb
    (HGLOBAL   *lphGlbRht,
     LPAPLSTYPE lpaplTypeRes)

{
    LPVARARRAY_HEADER lpMemHdrRht = NULL;
    LPVOID            lpMemRht;
    APLSTYPE          aplTypeRht;
    APLRANK           aplRankRht;
    UBOOL             bRet = TRUE;
    LPSYMENTRY        lpSym;
    HGLOBAL           hGlbFirst;

    // It's a valid HGLOBAL array
    Assert (IsGlbTypeVarDir_PTB (*lphGlbRht));

    // Lock the memory to get a ptr to it
    lpMemHdrRht = MyGlobalLockVar (*lphGlbRht);

#define lpHeader        lpMemHdrRht
    // Get the Array Type and Rank
    aplTypeRht = lpHeader->ArrType;
    aplRankRht = lpHeader->Rank;
#undef  lpHeader

    // It's a nested element
    Assert (IsNested (aplTypeRht));

    // Point to the data
    lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

    // Split cases based upon the element's ptr type
    switch (GetPtrTypeInd (lpMemRht))
    {
        case PTRTYPE_STCONST:       // a {is} 1 {diamond} 0{rho}a (2 3)
            lpSym = *(LPAPLHETERO) lpMemRht;

            // It's an immediate
            Assert (lpSym->stFlags.Imm);
            *lpaplTypeRes = TranslateImmTypeToArrayType (lpSym->stFlags.ImmType);

            break;

        case PTRTYPE_HGLOBAL:
            // Get the HGLOBAL
            hGlbFirst = *(LPAPLNESTED) lpMemRht;

            bRet = FALSE;

            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (*lphGlbRht); lpMemHdrRht = NULL;

    // If not simple, return the HGLOBAL of the first element
    if (!bRet)
        *lphGlbRht = hGlbFirst;

    return bRet;
} // End IsFirstSimpleGlb


//***************************************************************************
//  $CopySymGlbDir_PTB
//
//  Copy a direct LPSYMENTRY or HGLOBAL incrementing the reference count
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CopySymGlbDir_PTB"
#else
#define APPEND_NAME
#endif

HGLOBAL CopySymGlbDir_PTB
    (LPSYMENTRY lpSymGlb)

{
    // Split cases based upon the ptr type
    switch (GetPtrTypeDir (lpSymGlb))
    {
        case PTRTYPE_STCONST:
            Assert (lpSymGlb->Sig.nature EQ SYM_HEADER_SIGNATURE);

            // If the SYMENTRY is an internal function, ...
            if (lpSymGlb->stFlags.FcnDir)
                return lpSymGlb->stData.stGlbData;

            // If the SYMENTRY is named, ...
            if (lpSymGlb->stHshEntry->htGlbName NE NULL)
                // Copy it to an unnamed value-specific entry
                lpSymGlb =
                  CopyImmSymEntry_EM (lpSymGlb,
                                      IMMTYPE_SAME,
                                      NULL);
            return lpSymGlb;

        case PTRTYPE_HGLOBAL:
            // Increment the reference count in global memory
            DbgIncrRefCntDir_PTB ((HGLOBAL) lpSymGlb);  // EXAMPLE:  {iota}2 3

            return lpSymGlb;

        defstop
            return NULL;
    } // End SWITCH
} // End CopySymGlbDir_PTB
#undef  APPEND_NAME


//***************************************************************************
//  $CopySymGlbNumDir_PTB
//
//  Copy a direct LPSYMENTRY or HGLOBAL or Global Numeric
//    incrementing the reference count or making a copy of the global numeric
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CopySymGlbNumDir_PTB"
#else
#define APPEND_NAME
#endif

HGLOBAL CopySymGlbNumDir_PTB
    (LPSYMENTRY lpSymGlb,               // Ptr to LPSYMENTRY, HGLOBAL, or Global Numeric value
     APLSTYPE   aplType,                // Argument storage type
     LPTOKEN    lptkFunc)               // Ptr to function token

{
    // Split cases based upon the ptr type
    switch (GetPtrTypeDir (lpSymGlb))
    {
        case PTRTYPE_STCONST:
            // If it's a global numeric, ...
            if (IsGlbNum (aplType))
                lpSymGlb =
                  MakeGlbEntry_EM (aplType,     // Entry type
                                   lpSymGlb,    // Ptr to the value
                                   TRUE,        // TRUE iff we should initialize the target first
                                   lptkFunc);   // Ptr to function token
            else
            {
                Assert (lpSymGlb->Sig.nature EQ SYM_HEADER_SIGNATURE);

                // If the SYMENTRY is an internal function, ...
                if (lpSymGlb->stFlags.FcnDir)
                    return lpSymGlb->stData.stGlbData;

                // If the SYMENTRY is named, ...
                if (lpSymGlb->stHshEntry->htGlbName NE NULL)
                    // Copy it to an unnamed value-specific entry
                    lpSymGlb =
                      CopyImmSymEntry_EM (lpSymGlb,
                                          IMMTYPE_SAME,
                                          lptkFunc);
            } // End IF/ELSE

            return lpSymGlb;

        case PTRTYPE_HGLOBAL:
            // Increment the reference count in global memory
            DbgIncrRefCntDir_PTB ((HGLOBAL) lpSymGlb);  // EXAMPLE:  {disclose} NestedArray

            return MakePtrTypeGlb (lpSymGlb);

        defstop
            return NULL;
    } // End SWITCH
} // End CopySymGlbNumDir_PTB
#undef  APPEND_NAME


//***************************************************************************
//  $CopyArray_EM
//
//  Make a copy of a global memory ptr array
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CopyArray_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL CopyArray_EM
    (HGLOBAL hGlbSrc,                       // Source handle
     LPTOKEN lptkFunc)                      // Ptr to function token

{
    SIZE_T       dwSize;
    LPVOID       lpMemSrc,
                 lpMemDst;
    HGLOBAL      hGlbDst,                   // Dest global memory handle
                 hGlbItm;                   // Item ...
    APLSTYPE     aplType;
    APLNELM      aplNELM;
    APLRANK      aplRank;
    LPSYMENTRY   lpSymSrc,
                 lpSymDst;
    LPPL_YYSTYPE lpMemFcn;                  // Ptr to function array data
    APLNELM      u;
    UBOOL        bRet = TRUE;

    // Get the size of the global memory object
    dwSize = MyGlobalSize (hGlbSrc);
    Assert (dwSize NE 0);

    // Allocate storage for the copy of the array
    hGlbDst = DbgGlobalAlloc (GHND, dwSize);
    if (hGlbDst NE NULL)
    {
#ifdef DEBUG
        VFOHDRPTRS vfoHdrPtrs;
#endif
#ifdef DEBUG
        vfoHdrPtrs.lpMemVFO =
#endif
        // Lock both memory blocks
        lpMemDst = MyGlobalLock000 (hGlbDst);
        lpMemSrc = MyGlobalLock    (hGlbSrc);   // VAR, FCN, DFN

        // Copy source to destin
        CopyMemory (lpMemDst, lpMemSrc, dwSize);

        // Split cases based upon the array type
        switch (GetSignatureMem (lpMemDst))
        {
            case VARARRAY_HEADER_SIGNATURE:
                // Set the reference count in case it was a TKT_VARNAMED
#ifdef DEBUG_REFCNT
                dprintfWL0 (L"##RefCnt=1 in " APPEND_NAME L":        %p(res=1) (%S#%d)", hGlbDst, FNLN);
#endif
#ifdef DEBUG
  #define lpHeader vfoHdrPtrs.lpMemVar
#else
  #define lpHeader    ((LPVARARRAY_HEADER) lpMemDst)
#endif
                // Clear the PermNdx flags
                lpHeader->PermNdx = PERMNDX_NONE;

                // Set the RefCnt
                lpHeader->RefCnt = 1;

                // Clear all array properties
                lpHeader->PV0   =
                lpHeader->PV1   =
                lpHeader->All2s = FALSE;

                // Recurse through the array, copying all the global ptrs
                aplType = lpHeader->ArrType;
                aplNELM = lpHeader->NELM;
                aplRank = lpHeader->Rank;
#undef  lpHeader
                lpMemDst = VarArrayDataFmBase (lpMemDst);
                lpMemSrc = VarArrayDataFmBase (lpMemSrc);

                // Split cases based upon the array type
                switch (aplType)
                {
                    case ARRAY_BOOL:
                    case ARRAY_INT:
                    case ARRAY_FLOAT:
                    case ARRAY_CHAR:
                    case ARRAY_APA:
                        break;

                    case ARRAY_HETERO:
                    case ARRAY_NESTED:
                        // Handle the empty case
                        aplNELM = max (aplNELM, 1);

                        // Loop through the source and destin arrays
                        for (u = 0;
                             u < aplNELM;
                             u++, ((LPAPLNESTED) lpMemDst)++, ((LPAPLNESTED) lpMemSrc)++)
                        // Split cases based upon the ptr type
                        switch (GetPtrTypeInd (lpMemSrc))
                        {
                            case PTRTYPE_STCONST:
                                lpSymSrc = *(LPAPLHETERO) lpMemSrc;

                                // It's an immediate
                                Assert (lpSymSrc->stFlags.Imm);

                                // Copy it
                                lpSymDst =
                                  CopyImmSymEntry_EM (lpSymSrc,
                                                      IMMTYPE_SAME,
                                                      lptkFunc);
                                if (lpSymDst NE NULL)
                                    // Save into the destin
                                    *((LPAPLHETERO) lpMemDst) = MakePtrTypeSym (lpSymDst);
                                else
                                    bRet = FALSE;
                                break;

                            case PTRTYPE_HGLOBAL:
                                Assert (IsPtrArray (aplType));

                                // It's a valid HGLOBAL array
                                Assert (IsGlbTypeVarInd_PTB (lpMemSrc));

                                // Copy the array
                                hGlbItm = CopyArray_EM (*(LPAPLNESTED) lpMemSrc,
                                                        lptkFunc);
                                if (hGlbItm NE NULL)
                                    // Save into the destin
                                    *((LPAPLNESTED) lpMemDst) = MakePtrTypeGlb (hGlbItm);
                                else
                                    bRet = FALSE;
                                break;

                            defstop
                                break;
                        } // End FOR/SWITCH

                        break;

                    case ARRAY_RAT:
                        // Loop through the source and destin arrays
                        for (u = 0;
                             u < aplNELM;
                             u++, ((LPAPLRAT) lpMemDst)++, ((LPAPLRAT) lpMemSrc)++)
                        {
                            // Zap the copied values
                            ZeroMemory (lpMemDst, sizeof (APLRAT));

                            // Copy the source value to the destin
                            mpq_init_set ((LPAPLRAT) lpMemDst, (LPAPLRAT) lpMemSrc);
                        } // End FOR

                        break;

                    case ARRAY_VFP:
                        // Loop through the source and destin arrays
                        for (u = 0;
                             u < aplNELM;
                             u++, ((LPAPLVFP) lpMemDst)++, ((LPAPLVFP) lpMemSrc)++)
                        {
                            // Zap the copied values
                            ZeroMemory (lpMemDst, sizeof (APLVFP));

                            // Copy the source value to the destin
                            mpfr_init_copy ((LPAPLVFP) lpMemDst, (LPAPLVFP) lpMemSrc);
                        } // End FOR

                        break;

                    defstop
                        break;
                } // End SWITCH

                break;

            case FCNARRAY_HEADER_SIGNATURE:
#ifdef DEBUG
  #define lpHeader vfoHdrPtrs.lpMemFcn
#else
  #define lpHeader    ((LPFCNARRAY_HEADER) lpMemDst)
#endif
                // Set the RefCnt
                lpHeader->RefCnt = 1;

                // Get the NELM
                aplNELM = lpHeader->tknNELM;

                // If there is a line text global memory handle, ...
                if (lpHeader->hGlbTxtLine NE NULL)
                    lpHeader->hGlbTxtLine =
                      // Copy the memory to a new handle, ignoring failure
                      CopyGlbMemory (lpHeader->hGlbTxtLine, TRUE);
#undef  lpHeader
                // Skip over the header to the data
                lpMemFcn = FcnArrayBaseToData (lpMemDst);

                // Loop through the items
                for (u = 0; bRet && u < aplNELM; u++, lpMemFcn++)
                // Split cases based upon the token type
                switch (lpMemFcn->tkToken.tkFlags.TknType)
                {
                    case TKT_FCNARRAY:
                    case TKT_VARARRAY:
                    case TKT_CHRSTRAND:
                    case TKT_NUMSTRAND:
                    case TKT_NUMSCALAR:
                    case TKT_AXISARRAY:
                    case TKT_FCNAFO:
                    case TKT_OP1AFO:
                    case TKT_OP2AFO:
                        // Get the item global memory handle
                        hGlbItm = lpMemFcn->tkToken.tkData.tkGlbData;

                        // If it's a UDFO, ...
                        if (GetSignatureGlb (hGlbItm) EQ DFN_HEADER_SIGNATURE)
                        {
                            // This DEBUG stmt probably never is triggered because
                            //    CopyArray_EM is never used on any form of function.
#ifdef DEBUG
                            DbgStop ();         // ***Probably never executed***
#endif
                            // No need to copy the UDFO body, just increment the RefCnt
                            DbgIncrRefCntDir_PTB (hGlbItm); // EXAMPLE:  ***Probably never executed***
                        } else
                            // Copy the array
                            hGlbItm = CopyArray_EM (hGlbItm,
                                                    lptkFunc);
                        if (hGlbItm NE NULL)
                            // Save back into the destin
                            lpMemFcn->tkToken.tkData.tkGlbData = MakePtrTypeGlb (hGlbItm);
                        else
                            bRet = FALSE;
                        break;

                    case TKT_AXISIMMED:
                    case TKT_VARIMMED:
                    case TKT_FCNIMMED:
                    case TKT_OP1IMMED:
                    case TKT_OP2IMMED:
                    case TKT_OP3IMMED:
                    case TKT_OPJOTDOT:
                    case TKT_FILLJOT:
                        break;

                    case TKT_VARNAMED:
                        // tkData is a LPSYMENTRY
                        Assert (GetPtrTypeDir (lpMemFcn->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

                        // This DEBUG stmt probably never is triggered because
                        //    pl_yylex converts all unassigned named vars to temps
#ifdef DEBUG
                        DbgStop ();             // ***Probably never executed***
#endif
                        // If the named var is not immediate, ...
                        if (!lpMemFcn->tkToken.tkData.tkSym->stFlags.Imm)
                        {
                            // Get the item global memory handle
                            hGlbItm = lpMemFcn->tkToken.tkData.tkSym->stData.stGlbData;

                            // It's a var, so just increment the RefCnt
                            DbgIncrRefCntDir_PTB (hGlbItm); // EXAMPLE:  ***Probably never executed***
                        } // End IF/ELSE

                        break;

                    case TKT_FCNNAMED:
                    case TKT_OP1NAMED:
                    case TKT_OP2NAMED:
                    case TKT_OP3NAMED:
                        // Get the item global memory handle
                        hGlbItm = lpMemFcn->tkToken.tkData.tkSym->stData.stGlbData;

                        // If it's a UDFO, ...
                        if (lpMemFcn->tkToken.tkData.tkSym->stFlags.UsrDfn)
                        {
                            // This DEBUG stmt probably never is triggered because
                            //    CopyArray_EM is never used on any form of function.
#ifdef DEBUG
                            DbgStop ();         // ***Probably never executed***
#endif
                            // No need to copy the UDFO body, just increment the RefCnt
                            DbgIncrRefCntDir_PTB (hGlbItm); // EXAMPLE:  ***Probably never executed***
                        } else
                            // Copy the array
                            hGlbItm = CopyArray_EM (hGlbItm,
                                                    lptkFunc);
                        if (hGlbItm NE NULL)
                        {
                            // Save back into the destin
                            lpMemFcn->tkToken.tkData.tkGlbData = MakePtrTypeGlb (hGlbItm);

                            // Change the token type and data from a named function array
                            //   to an unnamed function array
                            lpMemFcn->tkToken.tkFlags.TknType  = TKT_FCNARRAY;
                        } else
                            bRet = FALSE;
                        break;

                    defstop
                        break;
                } // End FOR/SWITCH

                break;

            case DFN_HEADER_SIGNATURE:
                // This DEBUG stmt probably never is triggered because
                //    CopyArray_EM is never used on any form of function.
#ifdef DEBUG
                DbgStop ();         // ***Probably never executed***
#endif
                // Increment the reference count
                DbgIncrRefCntDir_PTB (MakePtrTypeGlb (hGlbDst));    // EXAMPLE:  ***Probably never executed***

                break;

            case LSTARRAY_HEADER_SIGNATURE:     // No call for these as yet
            case VARNAMED_HEADER_SIGNATURE:     // ...
            defstop
                break;
        } // End SWITCH

        // We no longer need these ptrs
        MyGlobalUnlock (hGlbDst); lpMemDst = NULL;
        MyGlobalUnlock (hGlbSrc); lpMemSrc = NULL;

        if (!bRet)
        {
            FreeResultGlobalIncompleteVar (hGlbDst); hGlbDst = NULL;
        } // End IF
    } else
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
    return hGlbDst;
} // End CopyArray_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CopyGlbAsType_EM
//
//  Copy a simple global as a given (simple and possibly wider
//    but never narrower) storage type
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CopyGlbAsType_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL CopyGlbAsType_EM
    (HGLOBAL  hGlbArg,                      // Arg global memory handle
     APLSTYPE aplTypeRes,                   // Result storage type
     LPTOKEN  lptkFunc)                     // Ptr to function token

{
    APLSTYPE          aplTypeArg;           // Arg storage type
    APLNELM           aplNELMArg;           // Arg/result NELM
    APLRANK           aplRankArg;           // Arg/result rank
    APLUINT           ByteRes;              // # bytes in the result
    HGLOBAL           hGlbRes = NULL;       // Result global memory handle
    LPVARARRAY_HEADER lpMemHdrArg = NULL,   // Ptr to arg header
                      lpMemHdrRes = NULL;   // ...    result ...
    LPVOID            lpMemArg,             // Ptr to arg global memory
                      lpMemRes;             // Ptr to result ...
    APLUINT           uArg;                 // Loop counter
    UINT              uBitMask;             // Bit mask for looping through Booleans
    APLINT            apaOffArg,            // Arg APA offset
                      apaMulArg,            // ...     multiplier
                      aplInteger;           // Temporary integer
    LPPERTABDATA      lpMemPTD;             // Ptr to PerTabData global memory
    LPSYMENTRY        lpSym0,               // LPSYMENTRY for constant zero
                      lpSym1,               // ...                     one
                      lpSymTmp;             // Ptr to temporary LPSYMENTRY

    // Get the attributes (Type, NELM, and Rank) of the arg
    AttrsOfGlb (hGlbArg, &aplTypeArg, &aplNELMArg, &aplRankArg, NULL);

    // Lock the memory to get a ptr to it
    lpMemHdrArg = MyGlobalLockVar (hGlbArg);

    // Calculate space for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMArg, aplRankArg);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
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
    lpHeader->NELM       = aplNELMArg;
    lpHeader->Rank       = aplRankArg;
#undef  lpHeader

    // Copy the dimensions
    CopyMemory (VarArrayBaseToDim (lpMemHdrRes),
                VarArrayBaseToDim (lpMemHdrArg),
                (APLU3264) aplRankArg * sizeof (APLDIM));
    // Skip over the header to the data
    lpMemArg = VarArrayDataFmBase (lpMemHdrArg);
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Copy the existing named values to the result
    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:                    // Res = BOOL, Arg = BOOL/APA
            // Split cases based upon the arg storage type
            switch (aplTypeArg)
            {
                case ARRAY_BOOL:            // Res = BOOL, Arg = BOOL
                    // Copy the arg elements to the result
                    CopyMemory (lpMemRes, lpMemArg, (APLU3264) RoundUpBitsToBytes (aplNELMArg));

                    break;

                case ARRAY_APA:             // Res = BOOL, Arg = APA
#define lpAPA       ((LPAPLAPA) lpMemArg)
                    // Get the APA parameters
                    apaOffArg = lpAPA->Off;
                    apaMulArg = lpAPA->Mul;
#undef  lpAPA
                    if (apaMulArg NE 0 || !IsBooleanValue (apaOffArg))
                        goto DOMAIN_EXIT;

                    // Do something only for 1s
                    if (apaOffArg NE 0)
                        FillBitMemory (lpMemRes, aplNELMArg);
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_INT:                     // Res = INT, Arg = BOOL/INT/APA
            // Loop through the arg elements
            for (uArg = 0; uArg < aplNELMArg; uArg++)
                (*aTypeActPromote[aplTypeArg][aplTypeRes]) (lpMemArg, uArg, (LPALLTYPES) ((LPAPLINT  ) lpMemRes)++);
            break;

        case ARRAY_FLOAT:                   // Res = FLOAT, Arg = BOOL/INT/APA/FLOAT
            // Loop through the arg elements
            for (uArg = 0; uArg < aplNELMArg; uArg++)
                (*aTypeActPromote[aplTypeArg][aplTypeRes]) (lpMemArg, uArg, (LPALLTYPES) ((LPAPLFLOAT) lpMemRes)++);
            break;

        case ARRAY_CHAR:
            // Copy the memory to the result
            CopyMemory (lpMemRes, lpMemArg, (APLU3264) aplNELMArg * sizeof (APLCHAR));

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Copy the arg to the result

            // Split cases based upon the arg storage type
            switch (aplTypeArg)
            {
                case ARRAY_BOOL:
                    // Get ptr to PerTabData global memory
                    lpMemPTD = GetMemPTD ();

                    lpSym0 = lpMemPTD->lphtsGLB->steZero;
                    lpSym1 = lpMemPTD->lphtsGLB->steOne;

                    uBitMask = BIT0;

                    // Loop through the arg elements
                    for (uArg = 0; uArg < aplNELMArg; uArg++)
                    {
                        // Save an LPSYMENTRY in the result
                        *((LPAPLNESTED) lpMemRes)++ =
                          (uBitMask & *(LPAPLBOOL) lpMemArg) ? lpSym1 : lpSym0;

                        // Shift over the bit mask
                        uBitMask <<= 1;

                        // Check for end-of-byte
                        if (uBitMask EQ END_OF_BYTE)
                        {
                            uBitMask = BIT0;            // Start over
                            ((LPAPLBOOL) lpMemArg)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_INT:
                    // Loop through the arg elements
                    for (uArg = 0; uArg < aplNELMArg; uArg++)
                    {
                        // Save an LPSYMENTRY in the result
                        *((LPAPLNESTED) lpMemRes)++ =
                        lpSymTmp =
                          MakeSymEntry_EM (IMMTYPE_INT,                 // Immediate type
                                           ((LPAPLINT) lpMemArg)++,     // Ptr to immediate value
                                           lptkFunc);                   // Ptr to function token
                        if (lpSymTmp EQ NULL)
                            goto ERROR_EXIT;
                    } // End IF

                    break;

                case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemArg)
                    // Get the APA parameters
                    apaOffArg = lpAPA->Off;
                    apaMulArg = lpAPA->Mul;
#undef  lpAPA
                    // Loop through the arg elements
                    for (uArg = 0; uArg < aplNELMArg; uArg++)
                    {
                        // Calculate the value
                        aplInteger = apaOffArg + apaMulArg * uArg;

                        // Save an LPSYMENTRY in the result
                        *((LPAPLNESTED) lpMemRes)++ =
                        lpSymTmp =
                          MakeSymEntry_EM (IMMTYPE_INT,                 // Immediate type
                                          &aplInteger,                  // Ptr to immediate value
                                           lptkFunc);                   // Ptr to function token
                        if (lpSymTmp EQ NULL)
                            goto ERROR_EXIT;
                    } // End FOR

                    break;

                case ARRAY_FLOAT:
                    // Loop through the arg elements
                    for (uArg = 0; uArg < aplNELMArg; uArg++)
                    {
                        // Save an LPSYMENTRY in the result
                        *((LPAPLNESTED) lpMemRes)++ =
                        lpSymTmp =
                          MakeSymEntry_EM (IMMTYPE_FLOAT,               // Immediate type
                            (LPAPLLONGEST) ((LPAPLFLOAT) lpMemArg)++,   // Ptr to immediate value
                                           lptkFunc);                   // Ptr to function token
                        if (lpSymTmp EQ NULL)
                            goto ERROR_EXIT;
                    } // End FOR

                    break;

                case ARRAY_CHAR:
                    // Loop through the arg elements
                    for (uArg = 0; uArg < aplNELMArg; uArg++)
                    {
                        // Save an LPSYMENTRY in the result
                        *((LPAPLNESTED) lpMemRes)++ =
                        lpSymTmp =
                          MakeSymEntry_EM (IMMTYPE_CHAR,                // Immediate type
                            (LPAPLLONGEST) ((LPAPLCHAR) lpMemArg)++,    // Ptr to immediate value
                                           lptkFunc);                   // Ptr to function token
                        if (lpSymTmp EQ NULL)
                            goto ERROR_EXIT;
                    } // End FOR

                    break;

                case ARRAY_HETERO:
                case ARRAY_NESTED:
                    // Loop through the arg elements
                    for (uArg = 0; uArg < aplNELMArg; uArg++)
                    // Split cases based upon the ptr type bits
                    switch (GetPtrTypeDir (((LPAPLNESTED) lpMemArg)[uArg]))
                    {
                        case PTRTYPE_STCONST:
                            // Copy over the LPSYMENTRY
                            *((LPAPLNESTED) lpMemRes)++ = ((LPAPLNESTED) lpMemArg)[uArg];

                            break;

                        case PTRTYPE_HGLOBAL:
                            // Copy over the HGLOBAL
                            *((LPAPLNESTED) lpMemRes)++ = ((LPAPLNESTED) lpMemArg)[uArg];

                            // Because we just made a copy, we need to increment the RefCnt
                            // Increment the reference count in global memory
                            DbgIncrRefCntDir_PTB (((LPAPLNESTED) lpMemArg)[uArg]);  // EXAMPLE a{is}1 2x 'a' {diamond} a[1]{is}{enclose}'abc;

                            break;

                        defstop
                            break;
                    } // End FOR/SWITCH

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_RAT:
            // Loop through the arg elements
            for (uArg = 0; uArg < aplNELMArg; uArg++)
                (*aTypeActPromote[aplTypeArg][aplTypeRes]) (lpMemArg, uArg, (LPALLTYPES) ((LPAPLRAT  ) lpMemRes)++);
            break;

        case ARRAY_VFP:
            // Loop through the arg elements
            for (uArg = 0; uArg < aplNELMArg; uArg++)
                (*aTypeActPromote[aplTypeArg][aplTypeRes]) (lpMemArg, uArg, (LPALLTYPES) ((LPAPLVFP  ) lpMemRes)++);
            break;

        defstop
            break;
    } // End SWITCH

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
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

        // We no longer need this resource
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbArg NE NULL && lpMemHdrArg NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbArg); lpMemHdrArg = NULL;
    } // End IF

    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    return hGlbRes;
} // End CopyGlbAsType_EM
#undef  APPEND_NAME


//***************************************************************************
//  $IsGlobalTypeArray_PTB
//
//  Confirm that an HGLOBAL is a valid variable or function array, or a
//    user-defined function/operator.
//***************************************************************************

UBOOL IsGlobalTypeArray_PTB
    (HGLOBAL hGlb,
     UINT    Signature)

{
    LPVOID lpMem;
    UBOOL  bRet;

    // It's an HGLOBAL
    switch (GetPtrTypeDir (hGlb))
    {
        case PTRTYPE_STCONST:
            return FALSE;

        case PTRTYPE_HGLOBAL:
            break;

        defstop
            break;
    } // End SWITCH

    // It's a valid handle
    bRet = IsValidHandle (hGlb);

    if (bRet)
    {
#ifdef DEBUG
        VFOHDRPTRS vfoHdrPtrs;
#endif
#ifdef DEBUG
        vfoHdrPtrs.lpMemVFO =
#endif
        // Lock the memory to get a ptr to it
        lpMem = MyGlobalLock (hGlb); Assert (lpMem NE NULL);    // DFN, FCN, LST, VAR, VNM

        // Split cases based upon the signature
        switch (GetSignatureMem (lpMem))
        {
            case DFN_HEADER_SIGNATURE:
#ifdef DEBUG
  #define lpHeader vfoHdrPtrs.lpMemDfn
#else
  #define lpHeader    ((LPDFN_HEADER) lpMem)
#endif
                // Ensure it has the correct signature
                bRet = (lpHeader->Sig.nature EQ Signature
                     && lpHeader->RefCnt > 0);
#undef  lpHeader
                break;

            case FCNARRAY_HEADER_SIGNATURE:
#ifdef DEBUG
  #define lpHeader vfoHdrPtrs.lpMemFcn
#else
  #define lpHeader    ((LPFCNARRAY_HEADER) lpMem)
#endif
                // Ensure it has the correct signature
                bRet = (lpHeader->Sig.nature EQ Signature
                     && lpHeader->RefCnt > 0);
#undef  lpHeader
                break;

            case LSTARRAY_HEADER_SIGNATURE:
#ifdef DEBUG
  #define lpHeader vfoHdrPtrs.lpMemLst
#else
  #define lpHeader    ((LPLSTARRAY_HEADER) lpMem)
#endif
                // Ensure it has the correct signature
                bRet = (lpHeader->Sig.nature EQ Signature);
#undef  lpHeader
                break;

            case VARARRAY_HEADER_SIGNATURE:
#ifdef DEBUG
  #define lpHeader vfoHdrPtrs.lpMemVar
#else
  #define lpHeader    ((LPVARARRAY_HEADER) lpMem)
#endif
                // Ensure it has the correct signature
                bRet = (lpHeader->Sig.nature EQ Signature)
                    && ((lpHeader->PermNdx NE PERMNDX_NONE) || (lpHeader->RefCnt > 0));
#undef  lpHeader
                break;

            case VARNAMED_HEADER_SIGNATURE:
#ifdef DEBUG
  #define lpHeader vfoHdrPtrs.lpMemNam
#else
  #define lpHeader    ((LPVARNAMED_HEADER) lpMem)
#endif
                // Ensure it has the correct signature
                bRet = (lpHeader->Sig.nature EQ Signature);
#undef  lpHeader
                break;

            defstop
                break;
        } // End SWITCH

        // We no longer need this ptr
        MyGlobalUnlock (hGlb); lpMem = NULL;
    } // End IF

    return bRet;
} // End IsGlobalTypeArray_PTB


//***************************************************************************
//  $IsGlbFcnArray
//
//  Determine if a global memory handle is that of a function array
//***************************************************************************

UBOOL IsGlbFcnArray
    (HGLOBAL hGlbFcn)

{
    return (GetSignatureGlb (hGlbFcn) EQ FCNARRAY_HEADER_SIGNATURE);
} // End IsGlbFcnArray


//***************************************************************************
//  $CheckRankLengthError_EM
//
//  Check a dyadic scalar function for RANK and LENGTH ERRORs
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CheckRankLengthError_EM"
#else
#define APPEND_NAME
#endif

UBOOL CheckRankLengthError_EM
    (APLRANK           aplRankRes,      // Result rank
     APLRANK           aplRankLft,      // Left arg ...
     APLNELM           aplNELMLft,      // ...      NELM
     LPVARARRAY_HEADER lpMemHdrLft,     // Ptr to left arg header
     APLRANK           aplRankRht,      // Right arg rank
     APLNELM           aplNELMRht,      // ...       NELM
     LPVARARRAY_HEADER lpMemHdrRht,     // Ptr to right arg header
     APLNELM           aplNELMAxis,     // Axis NELM
     LPAPLUINT         lpMemAxisTail,   // Ptr to grade up of AxisHead
     UBOOL             bLftIdent,       // TRUE iff the function has a left identity element and the Axis tail is valid
     UBOOL             bRhtIdent,       // ...                         right ...
     LPTOKEN           lptkFunc)        // Ptr to function token

{
    APLINT uRes;

    //***************************************************************
    // If the ranks don't match, then it gets complicated:
    //
    //   If both arguments are singletons, the result rank
    //     (and shape) is the same rank (and shape) as the
    //     argument with the larger rank.
    //   If only one argument is a singleton, the result rank
    //     (and shape) is the same as the rank (and shape) of the
    //     non-singleton argument.
    //   If either argument is a singleton, any valid axis values
    //     are allowed.  That is, any scalar or vector X such that
    //     {and}/X{epsilon}{iota}{rho}{rho}Z.
    //   If no axis is specified, it is equivalent to the axis being
    //     {iota}{rho}{rho}Z.
    //   If neither argument is a singleton and axis is specified,
    //     then it must be true that
    //       ({rho}L)[X]{match}{rho}R
    //     assuming that L is the argument of lower rank.  If not,
    //     switch L and R in the above expression.
    //***************************************************************

    // OUTER RANK and LENGTH ERRORs are possible only if
    //   neither argument is a singleton
    if (!IsSingleton (aplNELMLft) && !IsSingleton (aplNELMRht))
    {
        // Check for OUTER RANK ERROR
        if ((aplRankLft <  aplRankRht && (aplRankLft NE aplNELMAxis))
         || (aplRankLft >  aplRankRht && (aplRankRht NE aplNELMAxis)))
            goto RANK_EXIT;

        // If axis not full, ...
        if (aplNELMAxis NE aplRankRes)
        {
            if (aplRankLft < aplRankRht)
            {
                // Check for OUTER LENGTH ERROR
                for (uRes = 0; uRes < (APLRANKSIGN) aplRankLft; uRes++)
                if ((VarArrayBaseToDim (lpMemHdrLft))[uRes] !=
                    (VarArrayBaseToDim (lpMemHdrRht))[lpMemAxisTail[uRes]])
                {
                    uRes = (APLINT) -1; // Mark as in error

                    break;
                } // End FOR/IF
            } else
            if (aplRankLft > aplRankRht)
            {
                // Check for OUTER LENGTH ERROR
                for (uRes = 0; uRes < (APLRANKSIGN) aplRankRht; uRes++)
                if ((VarArrayBaseToDim (lpMemHdrLft))[lpMemAxisTail[uRes]] !=
                    (VarArrayBaseToDim (lpMemHdrRht))[uRes])
                {
                    uRes = (APLINT) -1; // Mark as in error

                    break;
                } // End FOR/IF
            } else
                uRes = (APLINT) -1; // Mark as in error (equal ranks with axis)
        } else
        // If axis full (or no axis) and ranks the same, ...
        if (aplRankLft EQ aplRankRht)
        {
            // If the function has no identity element, ...
            if (!bLftIdent && !bRhtIdent)
            {
                // Check for OUTER LENGTH ERROR
                for (uRes = 0; uRes < (APLRANKSIGN) aplRankRes; uRes++)
                if ((VarArrayBaseToDim (lpMemHdrLft))[uRes] !=
                    (VarArrayBaseToDim (lpMemHdrRht))[uRes])
                {
                    uRes = (APLINT) -1; // Mark as in error

                    break;
                } // End FOR/IF
            } else
            // If the function has a right-hand identity element only, ...
            if (bRhtIdent && !bLftIdent)
            {
                // Check for OUTER LENGTH ERROR
                for (uRes = 0; uRes < (APLRANKSIGN) aplRankRes; uRes++)
                if ((VarArrayBaseToDim (lpMemHdrLft))[lpMemAxisTail[uRes]] !=
                    (VarArrayBaseToDim (lpMemHdrRht))[uRes])
                {
                    uRes = (APLINT) -1; // Mark as in error

                    break;
                } // End FOR/IF


            } else
            // If the function has a left- or right-hand or both identity elements
                // Check for OUTER LENGTH ERROR
                for (uRes = 0; uRes < (APLRANKSIGN) aplRankRes; uRes++)
                if ((VarArrayBaseToDim (lpMemHdrLft))[uRes] !=
                    (VarArrayBaseToDim (lpMemHdrRht))[lpMemAxisTail[uRes]])
                {
                    uRes = (APLINT) -1; // Mark as in error

                    break;
                } // End FOR/IF
        } else
            uRes = (APLINT) -1; // Mark as in error (unequal ranks w/o axis)

        // Check for error
        if (uRes EQ (APLINT) -1)
            goto LENGTH_EXIT;
    } // End IF

    return TRUE;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    return FALSE;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkFunc);
    return FALSE;
} // End CheckRankLengthError_EM
#undef  APPEND_NAME


//***************************************************************************
//  $RoundUpBitsToBytes
//
//  Round up bits to a byte (8 bits/byte) and convert to # bytes
//***************************************************************************

APLINT RoundUpBitsToBytes
    (APLNELM aplNELM)           // NELM to convert

{
    return ((aplNELM + (NBIB - 1)) & ~MASKLOG2NBIB) >> LOG2NBIB;
} // End RoundUpBitsToBytes


//***************************************************************************
//  $RoundUpBitsInArray
//
//  Round up bits to a dword (32 bits/dword) and convert to # bytes
//***************************************************************************

APLINT RoundUpBitsInArray
    (APLNELM aplNELM)           // NELM to convert

{
    return ((aplNELM + (NBID - 1)) & ~MASKLOG2NBID) >> LOG2NBIB;
} // End RoundUpBitsInArray


//***************************************************************************
//  $BytesIn
//
//  Return the # bytes in an array of a given type and NELM
//***************************************************************************

APLUINT BytesIn
    (APLSTYPE aplTypeRht,               // Right arg storage type
     APLNELM  aplNELMRht)               // Right arg NELM

{
    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:                // 1 bit per element
            return RoundUpBitsToBytes (aplNELMRht);

        case ARRAY_INT:                 // 8 bytes per element
        case ARRAY_FLOAT:
            return aplNELMRht * sizeof (APLINT);

        case ARRAY_CHAR:                // 2 byte per element
            return aplNELMRht * sizeof (APLCHAR);

        case ARRAY_APA:                 // 2 8-byte elements
            return sizeof (APLAPA);

        case ARRAY_HETERO:              // 4 bytes per element
            return aplNELMRht * sizeof (APLHETERO);

        case ARRAY_NESTED:              // 4 bytes per element
            return aplNELMRht * sizeof (APLNESTED);

        defstop
            return 0;
    } // End SWITCH
} // End BytesIn


//***************************************************************************
//  $abs64
//
//  Return the absolute value of a 64-bit integer
//
//  Originally, this function was a macro as in
//
//    #define abs64(a)    (((a)>=0)?a:-(a))
//
//  however, the first time I tried
//
//    abs64 (*((LPAPLINT) lpMemLft)++)
//
//  I decided it was better off as a function.
//***************************************************************************

APLINT abs64
    (APLINT aplInt)

{
    // If the arg is the smallest (signed) APLINT, ...
    if (aplInt EQ MIN_APLINT)
        // Raise an exception because we can't represent the absolute value as an integer
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
////else
        return (aplInt >= 0) ? aplInt : -aplInt;
} // End abs64


//***************************************************************************
//  $iadd64
//
//  Add two 64-bit integers retaining maximum precision
//    while handling overflow.
//***************************************************************************

APLINT iadd64
    (APLINT  aplLft,            // Left arg
     APLINT  aplRht,            // Right ...
     LPUBOOL lpbRet)            // Is the result valid?? (may be NULL)

{
    APLINT aplRes;              // The result
    UBOOL  bRet;                // TRUE iff the result is valid

////// Call a routine to detect inexact precision result
////bRet = add_of (aplRes, aplLft, aplRht);
    // Call an assembler routine
    bRet = iAsmAdd64 (&aplRes, aplLft, aplRht);

    // If the caller handles the overflow, tell 'em whether or not it did
    if (lpbRet NE NULL)
        *lpbRet = bRet;
    else
    // Otherwise, if it overflowed, ...
    if (!bRet)
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

    return aplRes;
} // End iadd64


//***************************************************************************
//  $isub64
//
//  Subtract two 64-bit integers retaining maximum precision
//    while handling overflow.
//***************************************************************************

APLINT isub64
    (APLINT  aplLft,            // Left arg
     APLINT  aplRht,            // Right ...
     LPUBOOL lpbRet)            // Is the result valid?? (may be NULL)

{
    APLINT aplRes;              // The result
    UBOOL  bRet;                // TRUE iff the result is valid

////// Call a routine to detect inexact precision result
////bRet = sub_of (aplRes, aplLft, aplRht);
    // Call an assembler routine
    bRet = iAsmSub64 (&aplRes, aplLft, aplRht);

    // If the caller handles the overflow, tell 'em whether or not it did
    if (lpbRet NE NULL)
        *lpbRet = bRet;
    else
    // Otherwise, if it overflowed, ...
    if (!bRet)
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

    return aplRes;
} // End isub64


//***************************************************************************
//  $imul64
//
//  Multiply two 64-bit integers retaining maximum precision
//    while handling overflow.
//***************************************************************************

APLINT imul64
    (APLINT  aplLft,            // Left arg
     APLINT  aplRht,            // Right ...
     LPUBOOL lpbRet)            // Is the result valid?? (may be NULL)

{
    APLINT aplRes;              // The result
    UBOOL  bRet;                // TRUE iff the result is valid

    // Call an assembler routine
    bRet = iAsmMul64 (&aplRes, aplLft, aplRht);

    // If the caller handles the overflow, tell 'em whether or not it did
    if (lpbRet NE NULL)
        *lpbRet = bRet;
    else
    // Otherwise, if it overflowed, ...
    if (!bRet)
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    return aplRes;
} // End imul64


//***************************************************************************
//  $CalcDataSize
//
//  Calculate the size in bytes of the data portion of an array
//***************************************************************************

APLUINT CalcDataSize
    (ARRAY_TYPES aplType,       // Result storage type
     APLNELM     aplNELM,       // ...    NELM
     LPUBOOL     lpbRet)        // Ptr to TRUE iff the result is valid

{
    APLUINT ByteRes;            // # bytes in the result

    // Split cases based upon the result storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
            return imul64 (sizeof (APLBOOL)   , RoundUpBitsInArray (aplNELM), lpbRet);

        case ARRAY_INT:
            return imul64 (sizeof (APLINT)    , aplNELM                     , lpbRet);

        case ARRAY_FLOAT:
            return imul64 (sizeof (APLFLOAT)  , aplNELM                     , lpbRet);

        case ARRAY_CHAR:
            // Add in one element so we always have
            //   a zero-terminated string
            return imul64 (sizeof (APLCHAR)   , aplNELM + 1                 , lpbRet);

        case ARRAY_APA:
            return imul64 (sizeof (APLAPA)    , 1                           , lpbRet);

        case ARRAY_HETERO:
            return imul64 (sizeof (APLHETERO) , aplNELM                     , lpbRet);

        case ARRAY_LIST:
            ByteRes = imul64 (sizeof (APLLIST)   , aplNELM                  , lpbRet);
            if (*lpbRet)
                ByteRes = iadd64 (ByteRes, sizeof (LSTARRAY_HEADER)         , lpbRet);
            return ByteRes;

        case ARRAY_NESTED:
            // Make room for the prototype
            return imul64 (sizeof (APLNESTED) , max (aplNELM, 1)            , lpbRet);

        case ARRAY_RAT:
            return imul64 (sizeof (APLRAT)    , aplNELM                     , lpbRet);

        case ARRAY_VFP:
            return imul64 (sizeof (APLVFP)    , aplNELM                     , lpbRet);

        defstop
            return MAX_APLINT;
    } // End SWITCH
} // End CalcDataSize


//***************************************************************************
//  $CalcArraySize
//
//  Calculate the size in bytes of an array
//***************************************************************************

APLUINT CalcArraySize
    (ARRAY_TYPES aplType,       // Result storage type
     APLNELM     aplNELM,       // ...    NELM
     APLRANK     aplRank)       // ...    rank

{
    APLUINT ByteRes;            // # bytes in the result
    UBOOL   bRet = FALSE;       // TRUE iff the result is valid

    // Calculate the size of the data portion (excluding the header)
    ByteRes = CalcDataSize (aplType, aplNELM, &bRet);

    if (bRet && aplType NE ARRAY_LIST)
        // Add in the size of the header and dimension
        ByteRes = iadd64 (ByteRes, CalcHeaderSize (aplRank), &bRet);

    return bRet ? ByteRes : MAX_APLINT;
} // End CalcArraySize


//***************************************************************************
//  $CalcHeaderSize
//
//  Calculate the size in bytes of the header of an array
//***************************************************************************

APLUINT CalcHeaderSize
    (APLRANK aplRank)

{
    return sizeof (VARARRAY_HEADER)
         + sizeof (APLDIM) * aplRank;
} // End CalcHeaderSize


//***************************************************************************
//  $CalcFcnSize
//
//  Calculate the size in bytes of a function array
//***************************************************************************

APLUINT CalcFcnSize
    (UINT fcnCount)             // # fcns in the array

{
    return sizeof (FCNARRAY_HEADER) + fcnCount * sizeof (PL_YYSTYPE);
} // End CalcFcnSize


//***************************************************************************
//  $MakeNoValue_YY
//
//  Return a NoValue PL_YYSTYPE entry
//***************************************************************************

LPPL_YYSTYPE MakeNoValue_YY
    (LPTOKEN lptkFunc)          // Ptr to function token (may be NULL)

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYRes;       // Ptr to the result

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARNAMED;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
    lpYYRes->tkToken.tkFlags.NoDisplay = TRUE;
    lpYYRes->tkToken.tkData.tkSym      = lpMemPTD->lphtsPTD->steNoValue;
    lpYYRes->tkToken.tkCharIndex       = (lptkFunc NE NULL) ? lptkFunc->tkCharIndex
                                                            : -1;
    lpYYRes->tkToken.tkSynObj          = soNVAL;

    return lpYYRes;
} // End MakeNoValue_YY


//***************************************************************************
//  $GetSISLayer
//
//  Peel back to the first non-Imm/Exec layer
//***************************************************************************

LPSIS_HEADER GetSISLayer
    (LPSIS_HEADER lpSISCur)             // Ptr to current SIS layer

{
    while (lpSISCur
        && (lpSISCur->DfnType EQ DFNTYPE_IMM
         || lpSISCur->DfnType EQ DFNTYPE_EXEC))
        lpSISCur = lpSISCur->lpSISPrv;

    return lpSISCur;
} // End GetSISLayer


//***************************************************************************
//  $FillSISNxt
//
//  Fill in the SIS Header for lpSISNxt
//***************************************************************************

void FillSISNxt
    (LPPERTABDATA lpMemPTD,             // Ptr to PerTabData global memory
     HANDLE       hSemaphore,           // Semaphore handle
     DFN_TYPES    DfnType,              // DFNTYPE_xxx
     FCN_VALENCES FcnValence,           // FCNVALENCE_xxx
     UBOOL        bSuspended,           // TRUE iff starts Suspended
     UBOOL        bRestartable,         // TRUE iff restartable
     UBOOL        bLinkIntoChain)       // TRUE iff we should link this entry into the SIS chain

{
    lpMemPTD->SILevel++;

    // Clear to default value
    ZeroMemory (lpMemPTD->lpSISNxt, sizeof (*lpMemPTD->lpSISNxt));

    // Create another level on the SI stack
    lpMemPTD->lpSISNxt->Sig.nature    = SIS_HEADER_SIGNATURE;
    lpMemPTD->lpSISNxt->hSemaphore    = hSemaphore;
////lpMemPTD->lpSISNxt->hSigaphore    = NULL;           // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->hGlbDfnHdr    = NULL;           // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->hGlbFcnName   = NULL;           // Already zero from ZeroMemory
    lpMemPTD->lpSISNxt->hGlbQuadEM    = hGlbQuadEM_DEF;
////lpMemPTD->lpSISNxt->lpSymGlbGoto  = NULL;           // ALready zero from ZeroMemory
    lpMemPTD->lpSISNxt->DfnType       = DfnType;
    lpMemPTD->lpSISNxt->FcnValence    = FcnValence;
////lpMemPTD->lpSISNxt->DfnAxis       = FALSE;          // Already zero from ZeroMemory
    lpMemPTD->lpSISNxt->bSuspended    = bSuspended;
////lpMemPTD->lpSISNxt->ResetFlag     = RESETFLAG_NONE; // Already zero from ZeroMemory
    lpMemPTD->lpSISNxt->bRestartable  = bRestartable;
////lpMemPTD->lpSISNxt->bUnwind       = FALSE;          // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->bItsEC        = FALSE;          // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->bAFO          = FALSE;          // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->bMFO          = FALSE;          // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->EventType     = MAKE_ET (0, 0); // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->CurLineNum    = 0;              // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->NxtLineNum    = 0;              // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->NxtTknNum     = 0;              // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->numLabels     = 0;              // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->numFcnLines   = 0;              // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->numSymEntries = 0;              // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->QQPromptLen   = 0;              // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->ErrorCode     = ERRORCODE_NONE; // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->lpSISErrCtrl  =                 // Set in the code below
    lpMemPTD->lpSISNxt->lpSISPrv      = lpMemPTD->lpSISCur;
    lpMemPTD->lpSISNxt->lpSISNxt      = (LPSIS_HEADER) ByteAddr (lpMemPTD->lpSISNxt, sizeof (SIS_HEADER));
////lpMemPTD->lpSISNxt->lptkFunc      = NULL;           // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->lpForStmtBase =                 // Set in the code below
////lpMemPTD->lpSISNxt->lpForStmtNext =                 // Set in the code below

    // If the outgoing header is from []EA/[]EC,
    //   and this header is not []EA/[]EC, ...
    if (lpMemPTD->lpSISCur NE NULL
     && lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_ERRCTRL
     && DfnType NE DFNTYPE_ERRCTRL)
        lpMemPTD->lpSISNxt->lpSISErrCtrl = lpMemPTD->lpSISCur;
    else
    // If this header is from []EA/[]EC
    //   or there's no previous header, ...
    if (DfnType EQ DFNTYPE_ERRCTRL
     || lpMemPTD->lpSISCur EQ NULL)
        lpMemPTD->lpSISNxt->lpSISErrCtrl = NULL;
    else
        lpMemPTD->lpSISNxt->lpSISErrCtrl = lpMemPTD->lpSISCur->lpSISErrCtrl;

    // Fill in the FORSTMT ptr
    if (lpMemPTD->lpSISCur NE NULL)
        lpMemPTD->lpSISNxt->lpForStmtBase =
        lpMemPTD->lpSISNxt->lpForStmtNext = lpMemPTD->lpSISCur->lpForStmtNext;
    else
        lpMemPTD->lpSISNxt->lpForStmtBase =
        lpMemPTD->lpSISNxt->lpForStmtNext = lpMemPTD->lpForStmtBase;

    dprintfWL9 (L"~~Localize:    %p (%s)", lpMemPTD->lpSISNxt, L"FillSISNxt");

    // Link this SIS into the chain, if requested
    if (bLinkIntoChain)
    {
        if (lpMemPTD->lpSISCur NE NULL)
            lpMemPTD->lpSISCur->lpSISNxt = lpMemPTD->lpSISNxt;
        lpMemPTD->lpSISCur = lpMemPTD->lpSISNxt;
        lpMemPTD->lpSISNxt = lpMemPTD->lpSISNxt->lpSISNxt;
    } // End IF
} // End FillSISNxt


//***************************************************************************
//  $PassSigaphore
//
// If this hSigaphore is not for this level, pass it on up the line
//***************************************************************************

LPSIS_HEADER PassSigaphore
    (LPPERTABDATA lpMemPTD,
     HANDLE       hSigaphore)
{
    if (hSigaphore NE NULL
     && lpMemPTD->lpSISCur
     && hSigaphore NE lpMemPTD->lpSISCur->hSemaphore)
    {
        Assert (lpMemPTD->lpSISCur->hSigaphore EQ NULL);

        // Pass it on up the line
        lpMemPTD->lpSISCur->hSigaphore = hSigaphore;
        hSigaphore = NULL;
    } // End IF

    return hSigaphore;
} // End PassSigaphore


//***************************************************************************
//  $IsTknNamed
//
//  Return TRUE iff the given token is named
//***************************************************************************

UBOOL IsTknNamed
    (LPTOKEN lpToken)

{
    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:
        case TKT_FCNNAMED:
        case TKT_OP1NAMED:
        case TKT_OP2NAMED:
        case TKT_OP3NAMED:
        case TKT_DEL:
        case TKT_DELDEL:
            return TRUE;

        case TKT_FCNAFO:
        case TKT_OP1AFO:
        case TKT_OP2AFO:
            // Split cases based upon the ptr type bits
            switch (GetPtrTypeDir (lpToken->tkData.tkVoid))
            {
                case PTRTYPE_STCONST:
                    return TRUE;

                case PTRTYPE_HGLOBAL:
                    return FALSE;

                defstop
                    break;
            } // End SWITCH

            return FALSE;

        default:
            return FALSE;
    } // End SWITCH
} // End IsTknNamed


//***************************************************************************
//  $IsTknNamedFcnOpr
//
//  Return TRUE iff the given token is a named fcn/opr
//***************************************************************************

UBOOL IsTknNamedFcnOpr
    (LPTOKEN lpToken)

{
    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_FCNNAMED:
        case TKT_OP1NAMED:
        case TKT_OP2NAMED:
        case TKT_OP3NAMED:
            return TRUE;

        case TKT_FCNAFO:
        case TKT_OP1AFO:
        case TKT_OP2AFO:
            // Split cases based upon the ptr type bits
            switch (GetPtrTypeDir (lpToken->tkData.tkVoid))
            {
                case PTRTYPE_STCONST:
                    return TRUE;

                case PTRTYPE_HGLOBAL:
                    return FALSE;

                defstop
                    break;
            } // End SWITCH

            return FALSE;

        default:
            return FALSE;
    } // End SWITCH
} // End IsTknNamedFcnOpr


//***************************************************************************
//  $IsTknTypeAxis
//
//  Return TRUE iff the given token type is an Axis operand
//***************************************************************************

UBOOL IsTknTypeAxis
    (TOKEN_TYPES tknType)

{
    // Split cases based upon the token type
    switch (tknType)
    {
        case TKT_AXISIMMED:
        case TKT_AXISARRAY:
            return TRUE;

        default:
            return FALSE;
    } // End SWITCH
} // End IsTknTypeAxis


//***************************************************************************
//  $IsTknAFO
//
//  Return TRUE iff the given token is an AFO
//***************************************************************************

UBOOL IsTknAFO
    (LPTOKEN lpToken)           // Ptr to the token to test

{
    HGLOBAL      hGlbFcn;       // Global memory handle
    VFOHDRPTRS   vfoHdrPtrs;    // Ptr to global memory
    UBOOL        bRet;          // TRUE iff the function is an AFO

    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_DELAFO:
        case TKT_FCNAFO:
        case TKT_OP1AFO:
        case TKT_OP2AFO:
            return TRUE;

        case TKT_OP1NAMED:
        case TKT_OP2NAMED:
            // Get the global memory handle
            hGlbFcn = GetGlbHandle (lpToken);

            Assert (hGlbFcn NE NULL);

            // Lock the memory to get a ptr to it
            vfoHdrPtrs.lpMemVFO = MyGlobalLock (hGlbFcn);

            switch (GetSignatureMem (vfoHdrPtrs.lpMemVFO))
            {
                case DFN_HEADER_SIGNATURE:
                    // Copy the AFO flag
                    bRet = vfoHdrPtrs.lpMemDfn->bAFO;

                    break;

                case FCNARRAY_HEADER_SIGNATURE:
                    bRet = FALSE;

                    break;

                defstop
                    break;
            } // End SWITCH

            // We no longer need this ptr
            MyGlobalUnlock (hGlbFcn); vfoHdrPtrs.lpMemVFO = NULL;

            return bRet;

        default:
            return FALSE;
    } // End SWITCH
} // End IsTknAFO


//***************************************************************************
//  $IsTknTrain
//
//  Return TRUE iff the given token is a Train
//***************************************************************************

UBOOL IsTknTrain
    (LPTOKEN lpToken)           // Ptr to the token to test

{
    HGLOBAL           hGlbFcn;      // Global memory handle
    LPFCNARRAY_HEADER lpMemHdrFcn;  // Ptr to global memory header
    UBOOL             bRet;         // TRUE iff the token is a Train

    // Get the global memory handle
    hGlbFcn = GetGlbHandle (lpToken);

    // If there is a global handle,
    //   and it's a function array, ...
    if (hGlbFcn NE NULL
     && IsTknFcnArray (lpToken))
    {
        // Lock the memory to get a ptr to it
        lpMemHdrFcn = MyGlobalLockFcn (hGlbFcn);

        // Is the token a Train?
        bRet = (lpMemHdrFcn->fnNameType EQ NAMETYPE_TRN);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbFcn); lpMemHdrFcn = NULL;
    } else
        // Mark as not a Train
        bRet = FALSE;

    return bRet;
} // End IsTknTrain


//***************************************************************************
//  $IsTknTypeFcnOpr
//
//  Return TRUE iff the given token type is a fcn/opr
//***************************************************************************

UBOOL IsTknTypeFcnOpr
    (TOKEN_TYPES tknType)

{
    // Split cases based upon the token type
    switch (tknType)
    {
        case TKT_FCNNAMED:
        case TKT_OP1NAMED:
        case TKT_OP2NAMED:
        case TKT_OP3NAMED:
        case TKT_FCNIMMED:
        case TKT_OP1IMMED:
        case TKT_OP2IMMED:
        case TKT_OP3IMMED:
        case TKT_OPJOTDOT:
        case TKT_FCNARRAY:
        case TKT_FCNAFO:
        case TKT_OP1AFO:
        case TKT_OP2AFO:
            return TRUE;

        default:
            return FALSE;
    } // End SWITCH
} // End IsTknTypeFcnOpr


//***************************************************************************
//  $IsTknTypeVar
//
//  Return TRUE iff the given token type is a var
//***************************************************************************

UBOOL IsTknTypeVar
    (TOKEN_TYPES tknType)

{
    // Split cases based upon the token type
    switch (tknType)
    {
        case TKT_VARNAMED:
        case TKT_CHRSTRAND:
        case TKT_NUMSTRAND:
        case TKT_NUMSCALAR:
        case TKT_VARIMMED:
        case TKT_VARARRAY:
        case TKT_AXISARRAY:
        case TKT_STRAND:
            return TRUE;

        default:
            return FALSE;
    } // End SWITCH
} // End IsTknTypeVar


//***************************************************************************
//  $IsTknNamedVar
//
//  Return TRUE iff the given token is a named var
//***************************************************************************

UBOOL IsTknNamedVar
    (LPTOKEN lpToken)

{
    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            return TRUE;

        default:
            return FALSE;
    } // End SWITCH
} // End IsTknNamedVar


//***************************************************************************
//  $IsTknUsrDfn
//
//  Return TRUE iff the given token is a user-defined function
//***************************************************************************

UBOOL IsTknUsrDfn
    (LPTOKEN lptkVar)

{
    HGLOBAL hGlb;

    // If the token is immediate, it can't be a user fn
    if (IsTknImmed (lptkVar))
        return FALSE;

    // Get the global memory handle (if any)
    hGlb = GetGlbHandle (lptkVar);

    if (hGlb EQ NULL)
        return FALSE;

    return (IsGlbTypeDfnDir_PTB (MakePtrTypeGlb (hGlb)));
} // End IsTknUsrDfn


//***************************************************************************
//  $IsTknImmed
//
//  Return TRUE iff the given token is immediate
//***************************************************************************

UBOOL IsTknImmed
    (LPTOKEN lptkVar)                       // Ptr to token

{
    // Split cases based upon the token type
    switch (lptkVar->tkFlags.TknType)
    {
        case TKT_VARIMMED:
        case TKT_FCNIMMED:
        case TKT_OP1IMMED:
        case TKT_OP2IMMED:
        case TKT_OP3IMMED:
        case TKT_AXISIMMED:
        case TKT_LSTIMMED:
        case TKT_OPJOTDOT:
        case TKT_FILLJOT:
            return TRUE;

        case TKT_VARNAMED:
        case TKT_FCNNAMED:
        case TKT_OP1NAMED:
        case TKT_OP2NAMED:
        case TKT_OP3NAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkVar->tkData.tkVoid) EQ PTRTYPE_STCONST);

            return lptkVar->tkData.tkSym->stFlags.Imm;

        default:
            return FALSE;
    } // End SWITCH
} // End IsTknImmed


//***************************************************************************
//  $IsTknHybrid
//
//  Return TRUE iff the given token is a hybrid
//***************************************************************************

UBOOL IsTknHybrid
    (LPTOKEN lptkVar)                       // Ptr to token

{
    // Split cases based upon the token type
    switch (lptkVar->tkFlags.TknType)
    {
        case TKT_OP3IMMED:
        case TKT_OP3NAMED:
            return TRUE;

        default:
            return FALSE;
    } // End SWITCH
} // End IsTknHybrid


//***************************************************************************
//  $IsTknSysName
//
//  Return TRUE iff the given token is a []name, except possibly for []Z
//***************************************************************************

UBOOL IsTknSysName
    (LPTOKEN lptkVar,                       // Ptr to token
     UBOOL   bExcludeQuadZ)                 // TRUE iff we should exclude []Z as a SysName

{
    HGLOBAL   htGlbName;                    // SYMENTRY name global memory handle
    LPAPLCHAR lpMemName;                    // Ptr to var name
    UBOOL     bRet;                         // TRUE iff the name starts with a Quad or Quote-quad

    // If it's not named, ...
    if (!IsTknNamed (lptkVar))
        return FALSE;

    // Get the HshEntry name's global memory handle
    htGlbName = lptkVar->tkData.tkSym->stHshEntry->htGlbName;

    // Lock the memory to get a ptr to it
    lpMemName = MyGlobalLockWsz (htGlbName);

    // Izit a sys name?
    bRet = IsSysName (lpMemName);

    // If we should exclude []Z as a SysName, ...
    if (bExcludeQuadZ
     && lstrcmpiW (lpMemName, $AFORESULT) EQ 0)
        bRet = FALSE;

    // We no longer need this ptr
    MyGlobalUnlock (htGlbName); lpMemName = NULL;

    return bRet;
} // End IsTknSysName


//***************************************************************************
//  $mod64
//
//  Return the modulus of a 64-bit integer by a 64-bit integer
//
//  I'd like to use C's bulltin function a % b, but it signals a
//    Divide By Zero error if b EQ 0.
//
//  This routine is marked "static" because mpir defines the same routine
//    in "likely_prime_p.obj".
//***************************************************************************

static APLINT mod64
    (APLINT aplLft,             // Left arg (the value -- moduland?)
     APLINT aplRht)             // Right arg (the modulus)

{
    if (aplRht NE 0)
        return aplLft % aplRht;
    else
        return aplLft;
} // End mod64


//***************************************************************************
//  End of File: primfns.c
//***************************************************************************
