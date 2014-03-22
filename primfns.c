//***************************************************************************
//  NARS2000 -- Primitive Functions
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
#include <math.h>
#include "headers.h"

static APLINT mod64 (APLINT, APLINT);


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
            if (lpSISCur)
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
            return '?';

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
    LPVOID  lpMem;
    APLRANK aplRank;

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlb);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)
    // Get the Rank
    aplRank = lpHeader->Rank;
#undef  lpHeader

    // We no longer need this ptr
    MyGlobalUnlock (hGlb); lpMem = NULL;

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

            if (lpaplType)
                *lpaplType = TranslateImmTypeToArrayType (lpToken->tkData.tkSym->stFlags.ImmType);
            if (lpaplNELM)
                *lpaplNELM = 1;
            if (lpaplRank)
                *lpaplRank = 0;
            if (lpaplCols)
                *lpaplCols = 1;
            return;

        case TKT_VARIMMED:
        case TKT_LSTIMMED:
        case TKT_AXISIMMED:
            if (lpaplType)
                *lpaplType = TranslateImmTypeToArrayType (lpToken->tkFlags.ImmType);
            if (lpaplNELM)
                *lpaplNELM = 1;
            if (lpaplRank)
                *lpaplRank = 0;
            if (lpaplCols)
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
    lpMemData = MyGlobalLock (hGlbData);

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
            if (lpaplType)
                *lpaplType = ARRAY_LIST;
            if (lpaplNELM)
                *lpaplNELM = lpHeader->NELM;
            if (lpaplRank)
                *lpaplRank = 1;
            if (lpaplCols)
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
            if (lpaplType)
                *lpaplType = lpHeader->ArrType;
            if (lpaplNELM)
                *lpaplNELM = lpHeader->NELM;
            if (lpaplRank)
                *lpaplRank = lpHeader->Rank;
            if (lpaplCols)
            {
                if (!IsScalar (lpHeader->Rank))
                    // Skip over the header to the dimensions
                    *lpaplCols = (VarArrayBaseToDim (lpMemData))[lpHeader->Rank - 1];
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
    if (lpMemAxisHead)
    {
        for (iOdo = aplRank - 1; iOdo >= 0; iOdo--)
        if (++lpMemOdo[iOdo] EQ lpMemDim[lpMemAxisHead[iOdo]])
            lpMemOdo[iOdo] = 0;
        else
            return FALSE;
    } else
    {
        for (iOdo = aplRank - 1; iOdo >= 0; iOdo--)
        if (++lpMemOdo[iOdo] EQ lpMemDim[iOdo])
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
    (LPTOKEN    lptkFunc,       // Ptr to function token

     HGLOBAL   *lphGlbRes,      // Return HGLOBAL of the result

     LPVOID     lpMemLft,       // Ptr to left arg memory (may be NULL if monadic)
     LPVOID     lpMemRht,       // ...    right ...

     APLRANK    aplRankLft,     // Left arg rank
     APLRANK    aplRankRht,     // Right ...
     LPAPLRANK  lpaplRankRes,   // Ptr to result rank

     APLSTYPE   aplTypeRes,     // Result type

     UBOOL      bLftIdent,      // TRUE iff the function has a left identity element and the Axis tail is valid
     UBOOL      bRhtIdent,      // ...                         right ...

     APLNELM    aplNELMLft,     // Left arg NELM
     APLNELM    aplNELMRht,     // Right ...
     APLNELM    aplNELMRes)     // Result ...

{
    APLUINT  ByteRes;           // # bytes in the result
    LPVOID   lpMemRes;          // Ptr to locked memory in result
    LPAPLDIM lpMemDimArg;       // Ptr to the arg dimensions
    APLUINT  uRes;              // Loop counter

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
    if (!*lphGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (*lphGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
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
            (VarArrayBaseToDim (lpMemRes))[uRes] = 1;
    } else
    if (IsSingleton (aplNELMLft)
     || IsSingleton (aplNELMRht))
    {
        // Copy the ptr of the non-singleton argument
        if (!IsSingleton (aplNELMLft))
            lpMemDimArg = lpMemLft;
        else
            lpMemDimArg = lpMemRht;

        // Copy the dimensions from the non-singleton arg
        //   to the result
        for (uRes = 0; uRes < *lpaplRankRes; uRes++)
            (VarArrayBaseToDim (lpMemRes))[uRes] = (VarArrayBaseToDim (lpMemDimArg))[uRes];
    } else
    {
        // Get the higher rank's pointer
        //   to copy its dimensions to the result
        if (aplRankLft > aplRankRht)
            lpMemDimArg = lpMemLft;
        else
        if (aplRankLft < aplRankRht)
            lpMemDimArg = lpMemRht;
        else
        {
            // If there's right identity element only, ...
            if (!bLftIdent && bRhtIdent)
                // Copy the left arg dimensions
                lpMemDimArg = lpMemLft;
            else
                // Otherwise, copy the right arg dimensions
                lpMemDimArg = lpMemRht;
        } // End IF/ELSE/...

        // Copy the dimensions to the result
        for (uRes = 0; uRes < *lpaplRankRes; uRes++)
            (VarArrayBaseToDim (lpMemRes))[uRes] = (VarArrayBaseToDim (lpMemDimArg))[uRes];
    } // End IF/ELSE/...

    // We no longer need this ptr
    MyGlobalUnlock (*lphGlbRes); lpMemRes = NULL;

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
    HGLOBAL      hGlbTmp,           // Temporary global memory handle
                 hGlbRes,           // Result    ...
                 hSymGlbProto;      // Prototype ...
    LPVOID       lpMemArr = NULL,   // Ptr to array global memory
                 lpMemRes,          // ...
                 lpMemTmp = NULL;   // Ptr to tmp global memory
    LPVARARRAY_HEADER lpHeader;
    APLSTYPE     aplType;
    APLNELM      aplNELM;
    APLRANK      aplRank;
    UINT         u;
    APLNELM      uLen;
    UBOOL        bRet = TRUE;       // TRUE iff result is valid
    APLUINT      ByteRes;           // # bytes in the result
    LPSYMENTRY   lpSymArr,
                 lpSymRes;
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

    DBGENTER;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Split cases based upon the ptr type
    switch (GetPtrTypeDir (hGlbArr))
    {
        case PTRTYPE_STCONST:
            // If it's numeric, ...
            if (IsImmNum (((LPSYMENTRY) hGlbArr)->stFlags.ImmType))
                return lpMemPTD->lphtsPTD->steZero;
            else
                return lpMemPTD->lphtsPTD->steBlank;

        case PTRTYPE_HGLOBAL:
            break;

        defstop
            break;
    } // End SWITCH

    // Make a copy of the array as we're changing it
    hGlbArr = CopyArray_EM (hGlbArr,
                            lptkFunc);
    if (!hGlbArr)
        return NULL;

    // Lock the memory to get a ptr to it
    lpMemArr = lpHeader = MyGlobalLock (hGlbArr);

    aplType = lpHeader->ArrType;
    aplNELM = lpHeader->NELM;
    aplRank = lpHeader->Rank;

    // Point to the data
    lpMemArr = VarArrayBaseToData (lpMemArr, aplRank);

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
                    lpHeader->ArrType = ARRAY_BOOL;

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
                    if (!hGlbTmp)
                        goto WSFULL_EXIT;

                    // Lock the memory to get a ptr to it
                    lpMemTmp = MyGlobalLock (hGlbTmp);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemTmp)
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
                    CopyMemory (VarArrayBaseToDim (lpMemTmp),
                                VarArrayBaseToDim (lpHeader),
                                (APLU3264) aplRank * sizeof (APLDIM));

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbTmp); lpMemTmp = NULL;

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbArr); lpMemArr = NULL;

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
                            lpSymRes = lpMemPTD->lphtsPTD->steZero;

                            break;

                        case IMMTYPE_CHAR:
                            // Split cases based upon mpEnum
                            switch (mpEnum)
                            {
                                case MP_CHARS:
                                    lpSymRes = lpMemPTD->lphtsPTD->steBlank;

                                    break;

                                case MP_NUMONLY:
                                    goto DOMAIN_ERROR_EXIT;

                                    break;

                                case MP_NUMCONV:
                                    lpSymRes = lpMemPTD->lphtsPTD->steZero;

                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        defstop
                            goto ERROR_EXIT;
                    } // End SWITCH

                    if (lpSymRes)
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
                    if (hSymGlbProto)
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
            ByteRes = CalcArraySize (ARRAY_BOOL, aplNELM, aplRank);

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
            lpHeader->ArrType    = ARRAY_BOOL;
////////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
            lpHeader->RefCnt     = 1;
            lpHeader->NELM       = aplNELM;
            lpHeader->Rank       = aplRank;
#undef  lpHeader
            // Skip over the header to the dimensions
            lpMemRes = VarArrayBaseToDim (lpMemRes);

            // Copy the dimensions
            CopyMemory (lpMemRes, VarArrayBaseToDim (lpHeader), (APLU3264) aplRank * sizeof (APLRANK));

            // The data is already zero from GHND

            // We no longer need these ptrs
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
            MyGlobalUnlock (hGlbArr); lpMemArr = NULL;

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
    if (hGlbArr && lpMemArr)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbArr); lpMemArr = NULL;
    } // End IF

    if (!bRet)
    {
        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbArr); hGlbArr = NULL;
    } // End IF

    DBGLEAVE;

    if (hGlbArr)
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
    APLSTYPE    aplTypeLft,             // Left arg storage type
                aplTypeRht,             // Right ...
                aplTypeRes;             // Result ...
    APLNELM     aplNELMLft,             // Left arg NELM
                aplNELMRht,             // Right ...
                aplNELMRes,             // Result   ...
                aplNELMAxis;            // Axis value ...
    APLRANK     aplRankLft,             // Left arg rank
                aplRankRht,             // Right ...
                aplRankRes;             // Result ...
    HGLOBAL     hGlbRes = NULL,         // Result global memory handle
                hGlbAxis = NULL,        // Axis value ...
                hGlbWVec = NULL,        // Weighting vector ...
                hGlbOdo = NULL,         // Odometer ...
                hGlbSub;                // Subarray ...
    LPAPLNESTED lpMemLft = NULL,        // Ptr to left arg global memory
                lpMemRht = NULL,        // Ptr to right ...
                lpMemRes = NULL;        // Ptr to result   ...
    LPAPLDIM    lpMemDimRes;            // Ptr to result dimensions
    APLUINT     ByteRes,                // # bytes in the result
                uLft,                   // Loop counter
                uRht,                   // Loop counter
                uRes;                   // Loop counter
    APLINT      iDim;                   // Dimension loop counter
    UBOOL       bBoolFn,                // TRUE iff the function is equal or not-equal
                bLftIdent,              // TRUE iff the function has a left identity element and the Axis tail is valid
                bRhtIdent;              // ...                         right ...
    LPPRIMSPEC  lpPrimSpec;             // Ptr to local PRIMSPEC
    LPAPLUINT   lpMemAxisHead = NULL,   // Ptr to axis values, fleshed out by CheckAxis_EM
                lpMemAxisTail = NULL,   // Ptr to grade up of AxisHead
                lpMemOdo = NULL,        // Ptr to odometer global memory
                lpMemWVec = NULL;       // Ptr to weighting vector ...
    LPPRIMFLAGS lpPrimFlags;            // Ptr to function PrimFlags entry

    // Get the attributes (Type, NELM, and Rank)
    //   of the left arg
    if (hGlbLft)        // If not immediate, ...
        AttrsOfGlb (hGlbLft, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    else
    {
        aplTypeLft = TranslateImmTypeToArrayType (immTypeLft);
        aplNELMLft = 1;
        aplRankLft = 0;
    } // End IF

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    if (hGlbRht)        // If not immediate, ...
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
        lpMemAxisHead = MyGlobalLock (hGlbAxis);

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
    if (hGlbLft)
        lpMemLft = MyGlobalLock (hGlbLft);

    // Lock the memory to get a ptr to it
    if (hGlbRht)
        lpMemRht = MyGlobalLock (hGlbRht);

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
                                  lpMemLft,         // Ptr to left arg global memory
                                  aplRankRht,       // Right arg rank
                                  aplNELMRht,       // Right arg NELM
                                  lpMemRht,         // Ptr to right arg global memory
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
        if (!hGlbRes)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
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
            CopyMemory (VarArrayBaseToDim (lpMemRes),
                        VarArrayBaseToDim (lpMemRht),
                        (APLU3264) aplNELMRes * sizeof (APLDIM));
        else
        if (aplRankLft > aplRankRht)
            // Copy from the left
            CopyMemory (VarArrayBaseToDim (lpMemRes),
                        VarArrayBaseToDim (lpMemLft),
                        (APLU3264) aplNELMRes * sizeof (APLDIM));
        else
        {
            // If the function has a right-hand identity element only, ...
            if (bRhtIdent && !bLftIdent)
                // Copy from the left
                CopyMemory (VarArrayBaseToDim (lpMemRes),
                            VarArrayBaseToDim (lpMemLft),
                            (APLU3264) aplNELMRes * sizeof (APLDIM));
            else
                // Copy from the right
                CopyMemory (VarArrayBaseToDim (lpMemRes),
                            VarArrayBaseToDim (lpMemRht),
                            (APLU3264) aplNELMRes * sizeof (APLDIM));
        } // End IF/ELSE/...

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

#define lpAPA       ((LPAPLAPA) lpMemRes)
        // To make a prototype APA, set its
        //   offset and multiplier to zero
        lpAPA->Off =
        lpAPA->Mul = 0;
#undef  lpAPA

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

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
                                         lpMemLft,
                                         lpMemRht,
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
        lpMemRes = MyGlobalLock (hGlbRes);      // ***FIXME*** -- Why???

        // Skip over the header to the dimensions
        lpMemDimRes = VarArrayBaseToDim (lpMemRes);

        // Skip over the header and dimensions to the data
        lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);
        lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

        // If either arg is not simple, loop through the result
        // Otherwise, the result is all zero (already filled in by GHND).
        if (!IsSimple (aplTypeLft)
         || !IsSimple (aplTypeRht))
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
                if (!hGlbWVec)
                    goto WSFULL_EXIT;

                // Lock the memory to get a ptr to it
                lpMemWVec = MyGlobalLock (hGlbWVec);

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
                if (!hGlbOdo)
                    goto WSFULL_EXIT;

                // Lock the memory to get a ptr to it
                lpMemOdo = MyGlobalLock (hGlbOdo);
            } // End IF

            // Loop through the elements of the result
            for (uRes = 0; uRes < aplNELMRes; uRes++)
            {
                // If there's an axis, ...
                if (lptkAxis
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
                    if (!hGlbSub)
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
                    if (!hGlbSub)
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
                    if (!hGlbSub)
                        goto ERROR_EXIT;
                    *lpMemRes++ = hGlbSub;
                } // End IF/ELSE/...
            } // End FOR
        } // End IF

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

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
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    if (hGlbLft && lpMemLft)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

    if (hGlbWVec)
    {
        if (lpMemWVec)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbWVec); lpMemWVec = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbWVec); hGlbWVec = NULL;
    } // End IF

    if (hGlbOdo)
    {
        if (lpMemOdo)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbOdo); lpMemOdo = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbOdo); hGlbOdo = NULL;
    } // End IF

    if (hGlbAxis)
    {
        if (lpMemAxisHead)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbAxis); lpMemAxisHead = lpMemAxisTail = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbAxis); hGlbAxis = NULL;
    } // End IF

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
    LPVOID     lpMemRht;
    APLSTYPE   aplTypeRht;
    APLRANK    aplRankRht;
    UBOOL      bRet = TRUE;
    LPSYMENTRY lpSym;
    HGLOBAL    hGlbFirst;

    // It's a valid HGLOBAL array
    Assert (IsGlbTypeVarDir_PTB (*lphGlbRht));

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (*lphGlbRht);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRht)
    // Get the Array Type and Rank
    aplTypeRht = lpHeader->ArrType;
    aplRankRht = lpHeader->Rank;
#undef  lpHeader

    // It's a nested element
    Assert (IsNested (aplTypeRht));

    // Point to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

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
    MyGlobalUnlock (*lphGlbRht); lpMemRht = NULL;

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
            if (lpSymGlb->stHshEntry->htGlbName)
                // Copy it to an unnamed value-specific entry
                lpSymGlb =
                  CopyImmSymEntry_EM (lpSymGlb,
                                      IMMTYPE_SAME,
                                      NULL);
            return lpSymGlb;

        case PTRTYPE_HGLOBAL:
            // Increment the reference count in global memory
            DbgIncrRefCntDir_PTB ((HGLOBAL) lpSymGlb);

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
                if (lpSymGlb->stHshEntry->htGlbName)
                    // Copy it to an unnamed value-specific entry
                    lpSymGlb =
                      CopyImmSymEntry_EM (lpSymGlb,
                                          IMMTYPE_SAME,
                                          lptkFunc);
            } // End IF/ELSE

            return lpSymGlb;

        case PTRTYPE_HGLOBAL:
            // Increment the reference count in global memory
            DbgIncrRefCntDir_PTB ((HGLOBAL) lpSymGlb);

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
    LPVOID       lpMemSrc, lpMemSrcBase,
                 lpMemDst, lpMemDstBase;
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
    if (hGlbDst)
    {
#ifdef DEBUG
        VFOHDRPTRS vfoHdrPtrs;
#endif
#ifdef DEBUG
        vfoHdrPtrs.lpMemVar =
#endif
        // Lock both memory blocks
        lpMemDst = MyGlobalLock (hGlbDst);
        lpMemSrc = MyGlobalLock (hGlbSrc);

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
                // Clear the SkipRefCnt flag
                lpHeader->SkipRefCntIncr = FALSE;

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
                lpMemDstBase = lpMemDst = VarArrayBaseToData (lpMemDst, aplRank);
                lpMemSrcBase = lpMemSrc = VarArrayBaseToData (lpMemSrc, aplRank);

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

                        // Fill nested result with PTR_REUSED
                        //   in case we fail part way through
                        for (u = 0; u < aplNELM; u++)
                            *((LPAPLNESTED) lpMemDst)++ = PTR_REUSED;

                        // Start the destin ptr over again
                        lpMemDst = lpMemDstBase;

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
                                if (lpSymDst)
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
                                if (hGlbItm)
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
                if (lpHeader->hGlbTxtLine)
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
                            // No need to copy the UDFO body, just increment the RefCnt
                            DbgIncrRefCntDir_PTB (hGlbItm);
                        else
                            // Copy the array
                            hGlbItm = CopyArray_EM (hGlbItm,
                                                    lptkFunc);
                        if (hGlbItm)
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

                        // If the named var is not immediate, ...
                        if (!lpMemFcn->tkToken.tkData.tkSym->stFlags.Imm)
                        {
                            // Get the item global memory handle
                            hGlbItm = lpMemFcn->tkToken.tkData.tkSym->stData.stGlbData;

                            // It's a var, so just increment the RefCnt
                            DbgIncrRefCntDir_PTB (hGlbItm);
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
                            // No need to copy the UDFO body, just increment the RefCnt
                            DbgIncrRefCntDir_PTB (hGlbItm);
                        else
                            // Copy the array
                            hGlbItm = CopyArray_EM (hGlbItm,
                                                    lptkFunc);
                        if (hGlbItm)
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
                // Increment the reference count
                DbgIncrRefCntDir_PTB (MakePtrTypeGlb (hGlbDst));

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
    (HGLOBAL  hGlbArg,                  // Arg global memory handle
     APLSTYPE aplTypeRes,               // Result storage type
     LPTOKEN  lptkFunc)                 // Ptr to function token

{
    APLSTYPE     aplTypeArg;            // Arg storage type
    APLNELM      aplNELMArg;            // Arg/result NELM
    APLRANK      aplRankArg;            // Arg/result rank
    APLUINT      ByteRes;               // # bytes in the result
    HGLOBAL      hGlbRes = NULL;        // Result global memory handle
    LPVOID       lpMemArg = NULL,       // Ptr to arg global memory
                 lpMemRes = NULL;       // Ptr to result ...
    APLUINT      uArg;                  // Loop counter
    UINT         uBitMask;              // Bit mask for looping through Booleans
    APLINT       apaOffArg,             // Arg APA offset
                 apaMulArg,             // ...     multiplier
                 aplInteger;            // Temporary integer
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPSYMENTRY   lpSym0,                // LPSYMENTRY for constant zero
                 lpSym1,                // ...                     one
                 lpSymTmp;              // Ptr to temporary LPSYMENTRY

    // Get the attributes (Type, NELM, and Rank) of the arg
    AttrsOfGlb (hGlbArg, &aplTypeArg, &aplNELMArg, &aplRankArg, NULL);

    // Lock the memory to get a ptr to it
    lpMemArg = MyGlobalLock (hGlbArg);

    // Calculate space for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMArg, aplRankArg);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
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
    CopyMemory (VarArrayBaseToDim (lpMemRes),
                VarArrayBaseToDim (lpMemArg),
                (APLU3264) aplRankArg * sizeof (APLDIM));
    // Skip over the header to the data
    lpMemArg = VarArrayBaseToData (lpMemArg, aplRankArg);
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankArg);

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
                    if (apaOffArg)
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

                    lpSym0 = lpMemPTD->lphtsPTD->steZero;
                    lpSym1 = lpMemPTD->lphtsPTD->steOne;

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
                        if (!lpSymTmp)
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
                        if (!lpSymTmp)
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
                        if (!lpSymTmp)
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
                        if (!lpSymTmp)
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
                            DbgIncrRefCntDir_PTB (((LPAPLNESTED) lpMemArg)[uArg]);

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
    if (hGlbRes)
    {
        if (lpMemRes)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
        } // End IF

        // We no longer need this resource
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbArg && lpMemArg)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbArg); lpMemArg = NULL;
    } // End IF

    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
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
    UBOOL  bRet = TRUE;

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
    bRet = bRet && IsValidHandle (hGlb);

    if (bRet)
    {
        // Lock the memory to get a ptr to it
        lpMem = MyGlobalLock (hGlb); Assert (lpMem NE NULL);

        // Split cases based upon the signature
        switch (GetSignatureMem (lpMem))
        {
            case DFN_HEADER_SIGNATURE:
#define lpHeader    ((LPDFN_HEADER) lpMem)
                // Ensure it has the correct signature
                bRet = (lpHeader->Sig.nature EQ Signature
                     && lpHeader->RefCnt > 0);
#undef  lpHeader
                break;

            case FCNARRAY_HEADER_SIGNATURE:
#define lpHeader    ((LPFCNARRAY_HEADER) lpMem)
                // Ensure it has the correct signature
                bRet = (lpHeader->Sig.nature EQ Signature
                     && lpHeader->RefCnt > 0);
#undef  lpHeader
                break;

            case LSTARRAY_HEADER_SIGNATURE:
#define lpHeader    ((LPLSTARRAY_HEADER) lpMem)
                // Ensure it has the correct signature
                bRet = (lpHeader->Sig.nature EQ Signature);
#undef  lpHeader
                break;

            case VARARRAY_HEADER_SIGNATURE:
#define lpHeader    ((LPVARARRAY_HEADER) lpMem)
                // Ensure it has the correct signature
                bRet = (lpHeader->Sig.nature EQ Signature)
                    && ((lpHeader->PermNdx NE PERMNDX_NONE) || (lpHeader->RefCnt > 0));
#undef  lpHeader
                break;

            case VARNAMED_HEADER_SIGNATURE:
#define lpHeader    ((LPVARNAMED_HEADER) lpMem)
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
    (APLRANK   aplRankRes,          // Result rank
     APLRANK   aplRankLft,          // Left arg ...
     APLNELM   aplNELMLft,          // ...      NELM
     LPVOID    lpMemLft,            // Ptr to left arg memory
     APLRANK   aplRankRht,          // Right arg rank
     APLNELM   aplNELMRht,          // ...       NELM
     LPVOID    lpMemRht,            // Ptr to right arg memory
     APLNELM   aplNELMAxis,         // Axis NELM
     LPAPLUINT lpMemAxisTail,       // Ptr to grade up of AxisHead
     UBOOL     bLftIdent,           // TRUE iff the function has a left identity element and the Axis tail is valid
     UBOOL     bRhtIdent,           // ...                         right ...
     LPTOKEN   lptkFunc)            // Ptr to function token

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
                if ((VarArrayBaseToDim (lpMemLft))[uRes] !=
                    (VarArrayBaseToDim (lpMemRht))[lpMemAxisTail[uRes]])
                {
                    uRes = (APLINT) -1; // Mark as in error

                    break;
                } // End FOR/IF
            } else
            if (aplRankLft > aplRankRht)
            {
                // Check for OUTER LENGTH ERROR
                for (uRes = 0; uRes < (APLRANKSIGN) aplRankRht; uRes++)
                if ((VarArrayBaseToDim (lpMemLft))[lpMemAxisTail[uRes]] !=
                    (VarArrayBaseToDim (lpMemRht))[uRes])
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
                if ((VarArrayBaseToDim (lpMemLft))[uRes] !=
                    (VarArrayBaseToDim (lpMemRht))[uRes])
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
                if ((VarArrayBaseToDim (lpMemLft))[lpMemAxisTail[uRes]] !=
                    (VarArrayBaseToDim (lpMemRht))[uRes])
                {
                    uRes = (APLINT) -1; // Mark as in error

                    break;
                } // End FOR/IF


            } else
            // If the function has a left- or right-hand or both identity elements
                // Check for OUTER LENGTH ERROR
                for (uRes = 0; uRes < (APLRANKSIGN) aplRankRes; uRes++)
                if ((VarArrayBaseToDim (lpMemLft))[uRes] !=
                    (VarArrayBaseToDim (lpMemRht))[lpMemAxisTail[uRes]])
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
    return (aplInt >= 0) ? aplInt : -aplInt;
} // End abs64


//***************************************************************************
//  $_iadd64
//
//  Add two 64-bit integers retaining maximum precision
//    while handling overflow.
//***************************************************************************

APLINT _iadd64
    (APLINT  aplLft,            // Left arg
     APLINT  aplRht,            // Right ...
     LPUBOOL lpbRet)            // Is the result valid?? (may be NULL)

{
    APLINT aplRes;              // The result
    UBOOL  bRet;                // TRUE iff the result is valid

    // Call an assembler routine
    bRet = iAsmAdd64 (&aplRes, aplLft, aplRht);

    // If the caller handles the overflow, tell 'em whether or not it did
    if (lpbRet)
        *lpbRet = bRet;
    else
    // Otherwise, if it overflowed, ...
    if (!bRet)
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

    return aplRes;
} // End _iadd64


//***************************************************************************
//  $_isub64
//
//  Subtract two 64-bit integers retaining maximum precision
//    while handling overflow.
//***************************************************************************

APLINT _isub64
    (APLINT  aplLft,            // Left arg
     APLINT  aplRht,            // Right ...
     LPUBOOL lpbRet)            // Is the result valid?? (may be NULL)

{
    APLINT aplRes;              // The result
    UBOOL  bRet;                // TRUE iff the result is valid

    // Call an assembler routine
    bRet = iAsmSub64 (&aplRes, aplLft, aplRht);

    // If the caller handles the overflow, tell 'em whether or not it did
    if (lpbRet)
        *lpbRet = bRet;
    else
    // Otherwise, if it overflowed, ...
    if (!bRet)
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

    return aplRes;
} // End _isub64


//***************************************************************************
//  $_imul64
//
//  Multiply two 64-bit integers retaining maximum precision
//    while handling overflow.
//***************************************************************************

APLINT _imul64
    (APLINT  aplLft,            // Left arg
     APLINT  aplRht,            // Right ...
     LPUBOOL lpbRet)            // Is the result valid?? (may be NULL)

{
    APLINT aplRes;              // The result
    UBOOL  bRet;                // TRUE iff the result is valid

    // Call an assembler routine
    bRet = iAsmMul64 (&aplRes, aplLft, aplRht);

    // If the caller handles the overflow, tell 'em whether or not it did
    if (lpbRet)
        *lpbRet = bRet;
    else
    // Otherwise, if it overflowed, ...
    if (!bRet)
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    return aplRes;
} // End _imul64


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
            return _imul64 (sizeof (APLBOOL)   , RoundUpBitsInArray (aplNELM), lpbRet);

        case ARRAY_INT:
            return _imul64 (sizeof (APLINT)    , aplNELM                     , lpbRet);

        case ARRAY_FLOAT:
            return _imul64 (sizeof (APLFLOAT)  , aplNELM                     , lpbRet);

        case ARRAY_CHAR:
            // Add in one element so we always have
            //   a zero-terminated string
            return _imul64 (sizeof (APLCHAR)   , aplNELM + 1                 , lpbRet);

        case ARRAY_APA:
            return _imul64 (sizeof (APLAPA)    , 1                           , lpbRet);

        case ARRAY_HETERO:
            return _imul64 (sizeof (APLHETERO) , aplNELM                     , lpbRet);

        case ARRAY_LIST:
            ByteRes = _imul64 (sizeof (APLLIST)   , aplNELM                  , lpbRet);
            if (*lpbRet)
                ByteRes = _iadd64 (ByteRes, sizeof (LSTARRAY_HEADER)         , lpbRet);
            return ByteRes;

        case ARRAY_NESTED:
            // Make room for the prototype
            return _imul64 (sizeof (APLNESTED) , max (aplNELM, 1)            , lpbRet);

        case ARRAY_RAT:
            return _imul64 (sizeof (APLRAT)    , aplNELM                     , lpbRet);

        case ARRAY_VFP:
            return _imul64 (sizeof (APLVFP)    , aplNELM                     , lpbRet);

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
        ByteRes = _iadd64 (ByteRes, CalcHeaderSize (aplRank), &bRet);

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
    (LPTOKEN lptkFunc)

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
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

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
     UBOOL        Suspended,            // TRUE iff starts Suspended
     UBOOL        Restartable,          // TRUE iff restartable
     UBOOL        LinkIntoChain)        // TRUE iff we should link this entry into the SIS chain

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
    lpMemPTD->lpSISNxt->DfnType       = DfnType;
    lpMemPTD->lpSISNxt->FcnValence    = FcnValence;
////lpMemPTD->lpSISNxt->DfnAxis       = FALSE;          // Already zero from ZeroMemory
    lpMemPTD->lpSISNxt->Suspended     = Suspended;
////lpMemPTD->lpSISNxt->ResetFlag     = RESETFLAG_NONE; // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->PermFn        = FALSE;          // Already zero from ZeroMemory
    lpMemPTD->lpSISNxt->Restartable   = Restartable;
////lpMemPTD->lpSISNxt->Unwind        = FALSE;          // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->Avail         = 0;              // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->EventType     = MAKE_ET (0, 0); // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->CurLineNum    = 0;              // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->NxtLineNum    = 0;              // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->NxtTknNum     = 0;              // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->numLabels     = 0;              // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->numFcnLines   = 0;              // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->QQPromptLen   = 0;              // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->ErrorCode     = ERRORCODE_NONE; // Already zero from ZeroMemory
    lpMemPTD->lpSISNxt->lpSISPrv      = lpMemPTD->lpSISCur;
    lpMemPTD->lpSISNxt->lpSISNxt      = (LPSIS_HEADER) ByteAddr (lpMemPTD->lpSISNxt, sizeof (SIS_HEADER));
////lpMemPTD->lpSISNxt->lptkFunc      = NULL;           // Already zero from ZeroMemory

    // If the outgoing header is from []EA/[]EC,
    //   and this header is not []EA/[]EC, ...
    if (lpMemPTD->lpSISCur
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
    if (lpMemPTD->lpSISCur)
        lpMemPTD->lpSISNxt->lpForStmtBase =
        lpMemPTD->lpSISNxt->lpForStmtNext = lpMemPTD->lpSISCur->lpForStmtNext;
    else
        lpMemPTD->lpSISNxt->lpForStmtBase =
        lpMemPTD->lpSISNxt->lpForStmtNext = lpMemPTD->lpForStmtBase;

    dprintfWL9 (L"~~Localize:    %p (%s)", lpMemPTD->lpSISNxt, L"FillSISNxt");

    // Link this SIS into the chain, if requested
    if (LinkIntoChain)
    {
        if (lpMemPTD->lpSISCur)
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
    if (hSigaphore
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
//  $IsTknTypeAFO
//
//  Return TRUE iff the given token type is an AFO
//***************************************************************************

UBOOL IsTknTypeAFO
    (TOKEN_TYPES tknType)

{
    // Split cases based upon the token type
    switch (tknType)
    {
        case TKT_FCNAFO:
        case TKT_OP1AFO:
        case TKT_OP2AFO:
            return TRUE;

        default:
            return FALSE;
    } // End SWITCH
} // End IsTknTypeAFO


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
    (LPTOKEN lptkVar)                       // Ptr to var token

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
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkVar->tkData.tkVoid) EQ PTRTYPE_STCONST);

            return lptkVar->tkData.tkSym->stFlags.Imm;

        default:
            return FALSE;
    } // End SWITCH
} // End IsTknImmed


//***************************************************************************
//  $SetVFOArraySRCIFlag
//
//  Set SkipRefCntIncr flag in a variable/function/operator array.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SetVFOArraySRCIFlag"
#else
#define APPEND_NAME
#endif

void SetVFOArraySRCIFlag
    (LPTOKEN lptkVFO)                   // Ptr to var/fcn/opr token

{
    HGLOBAL    hGlbVFO;
    VFOHDRPTRS vfoHdrPtrs;

    // If the token is named and has no value, ...
    if (IsTknNamed   (lptkVFO)
     && IsSymNoValue (lptkVFO->tkData.tkSym))
        return;

    // If the token is immediate, ...
    if (IsTknImmed (lptkVFO))
        return;

    // Get the array's global ptrs (if any)
    //   and lock it
    GetGlbPtrs_LOCK (lptkVFO, &hGlbVFO, &vfoHdrPtrs.lpMemVFO);

    // Is the array global?
    if (hGlbVFO)
    {
        // If the array is a UDFO, ...
        if (IsTknUsrDfn (lptkVFO))
            // Set the UDFO flag which says to skip the next
            //   IncrRefCnt
            vfoHdrPtrs.lpMemDfn->SkipRefCntIncr = TRUE;
        else
        // If the array is a fcn/opr, ...
        if (IsTknTypeFcnOpr (lptkVFO->tkFlags.TknType))
            // Set the Function Array flag which says to skip the next
            //   IncrRefCnt
            vfoHdrPtrs.lpMemFcn->SkipRefCntIncr = TRUE;
        else
        // If the array is a var?
        if (IsTknTypeVar (lptkVFO->tkFlags.TknType))
        {
            // Set the Variable flag which says to skip the next
            //   IncrRefCnt
            vfoHdrPtrs.lpMemVar->SkipRefCntIncr = TRUE;
#ifdef DEBUG_REFCNT
            dprintfWL0 (L"  RefCnt&& in " APPEND_NAME L":     %p(res=%d) (%S#%d)", hGlbVFO, vfoHdrPtrs.lpMemVar->RefCnt, FNLN);
#endif
        } else
            DbgStop ();

        // We no longer need this ptr
        MyGlobalUnlock (hGlbVFO); vfoHdrPtrs.lpMemVFO = NULL;
    } // End IF
} // End SetVFOArraySRCIFlag
#undef  APPEND_NAME


#ifdef DEBUG
//***************************************************************************
//  $GetVFOArraySRCIFlag
//
//  Get the SkipRefCntIncr flag in a variable/function/operator array.
//***************************************************************************

UBOOL GetVFOArraySRCIFlag
    (LPTOKEN lptkVFO)                   // Ptr to var/fcn/opr token

{
    HGLOBAL    hGlbVFO;
    VFOHDRPTRS vfoHdrPtrs;
    UBOOL      bRet = FALSE;        // Return value

    // If the token is named and has no value, ...
    if (IsTknNamed   (lptkVFO)
     && IsSymNoValue (lptkVFO->tkData.tkSym))
        return FALSE;

    // If the token is immediate, ...
    if (IsTknImmed (lptkVFO))
        return FALSE;

    // Get the array's global ptrs (if any)
    //   and lock it
    GetGlbPtrs_LOCK (lptkVFO, &hGlbVFO, &vfoHdrPtrs.lpMemVFO);

    // Is the array global?
    if (hGlbVFO)
    {
        // If the array is a UDFO, ...
        if (IsTknUsrDfn (lptkVFO))
            // Set the UDFO flag which says to skip the next
            //   IncrRefCnt
            bRet = vfoHdrPtrs.lpMemDfn->SkipRefCntIncr;
        else
        // If the array is a fcn/opr, ...
        if (IsTknTypeFcnOpr (lptkVFO->tkFlags.TknType))
            // Set the Function Array flag which says to skip the next
            //   IncrRefCnt
            bRet = vfoHdrPtrs.lpMemFcn->SkipRefCntIncr;
        else
        // If the array is a var?
        if (IsTknTypeVar (lptkVFO->tkFlags.TknType))
            // Set the Variable flag which says to skip the next
            //   IncrRefCnt
            bRet = vfoHdrPtrs.lpMemVar->SkipRefCntIncr;
        else
            DbgStop ();

        // We no longer need this ptr
        MyGlobalUnlock (hGlbVFO); vfoHdrPtrs.lpMemVFO = NULL;
    } // End IF

    return bRet;
} // End GetVFOArraySRCIFlag
#endif


//***************************************************************************
//  $ClrVFOArraySRCIFlag
//
//  Clear SkipRefCntIncr flag in a variable/function/operator array.
//***************************************************************************

void ClrVFOArraySRCIFlag
    (LPTOKEN lptkVFO)                   // Ptr to var/fcn/opr token

{
    HGLOBAL    hGlbVFO;
    VFOHDRPTRS vfoHdrPtrs;

    // If the token is named and has no value, ...
    if (IsTknNamed   (lptkVFO)
     && IsSymNoValue (lptkVFO->tkData.tkSym))
        return;

    // If the token is immediate, ...
    if (IsTknImmed (lptkVFO))
        return;

    // Get the array's global ptrs (if any)
    //   and lock it
    GetGlbPtrs_LOCK (lptkVFO, &hGlbVFO, &vfoHdrPtrs.lpMemVFO);

    // Is the array global?
    if (hGlbVFO)
    {
        // If the array is a UDFO, ...
        if (IsTknUsrDfn (lptkVFO))
            // Set the UDFO flag which says to skip the next
            //   IncrRefCnt
            vfoHdrPtrs.lpMemDfn->SkipRefCntIncr = FALSE;
        else
        // If the array is a fcn/opr, ...
        if (IsTknTypeFcnOpr (lptkVFO->tkFlags.TknType))
            // Set the Function Array flag which says to skip the next
            //   IncrRefCnt
            vfoHdrPtrs.lpMemFcn->SkipRefCntIncr = FALSE;
        else
        // If the array is a var?
        if (IsTknTypeVar (lptkVFO->tkFlags.TknType))
            // Set the Variable flag which says to skip the next
            //   IncrRefCnt
            vfoHdrPtrs.lpMemVar->SkipRefCntIncr = FALSE;
        else
            DbgStop ();

        // We no longer need this ptr
        MyGlobalUnlock (hGlbVFO); vfoHdrPtrs.lpMemVFO = NULL;
    } // End IF
} // End ClrVFOArraySRCIFlag


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
    if (aplRht)
        return aplLft % aplRht;
    else
        return aplLft;
} // End mod64


//***************************************************************************
//  End of File: primfns.c
//***************************************************************************
