//***************************************************************************
//  NARS2000 -- Type Demote/Promote Functions
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


//***************************************************************************
//  $TypeDemote
//
//  Attempt to demote the type of a global memory handle
//
//  Various primitives (e.g. Match) rely upon type demotion to
//  return a correct result.
//
//  Consequently, it must not be possible to store an all simple
//  homogeneous array in nested array format.  In other words,
//  structural functions such as
//
//    partitioned enclose
//    etc.
//
//  The following functions have been changed to use TypeDemote:
//    primitive scalar dyadic functions, simple vs. simple
//    enclose with axis
//    reshape
//    collapsing dyadic transpose
//    enlist
//    compression/replicate
//    each
//    first
//    outer product
//    reduction w/ or w/o axis
//    N-wise reduction
//    monadic & dyadic each
//    squad
//    scan
//    disclose
//    user-defined function/operator with multiple result names
//    pick
//    take
//    drop
//    bracket indexing
//    without
//    bracket indexed assignment
//
//  must call this function to check their result to see if it
//  can be stored more simply.  Note that more simply does not
//  mean in fewer bytes as 32-bit ptrs are four bytes while integers are eight.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- TypeDemote"
#else
#define APPEND_NAME
#endif

void TypeDemote
    (LPTOKEN lptkRhtArg)

{
    HGLOBAL           hGlbRht = NULL,       // Right arg global memory handle
                      hGlbSub,              // Temp      ...
                      hGlbRes = NULL;       // Result    ...
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg header
                      lpMemHdrRes = NULL,   // ...    result    ...
                      lpMemHdrSub = NULL;   // ...    subitem   ...
    LPVOID            lpMemRht,             // Ptr to right arg global memory
                      lpMemRes,             // Ptr to result    ...
                      lpMemSub;             // Ptr to temp      ...
    APLNELM           aplNELMRht,           // Right arg NELM
                      aplNELMNstRht;        // Right arg NELM in case empty nested
    APLRANK           aplRankRht,           // Right arg rank
                      aplRankSub;           // Temp      ...
    APLUINT           uRht,                 // Right arg loop counter
                      ByteRes;              // # bytes in the result
    APLINT            aplInteger;           // Temporary integer
    APLFLOAT          aplFloat;             // ...       float
    APLSTYPE          aplTypeRes,           // Result storage type
                      aplTypeRht,           // Right arg ...
                      aplTypeSub;           // Right arg item ...
    LPSYMENTRY        lpSymEntry;           // Ptr to Hetero item
    SIZE_T            dwSize;               // GlobalSize of right arg

    // Split cases based upon the arg token type
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

                break;          // Join common global code
            } // End IF

            // Handle the immediate case
        case TKT_VARIMMED:
            // if it's integer-like and Boolean valued, ...
            if (IsImmInt (lptkRhtArg->tkFlags.ImmType)
             && IsBooleanValue (*GetPtrTknLongest (lptkRhtArg)))
                lptkRhtArg->tkFlags.ImmType = IMMTYPE_BOOL;
            else
            // If it's an INT masquerading as a FLT, ...
            if (IsImmFlt (lptkRhtArg->tkFlags.ImmType)
            // Check for PoM infinity and numbers whose
            //   absolute value is >= 2*53
             && !(IsFltInfinity (lptkRhtArg->tkData.tkFloat)
               || fabs (lptkRhtArg->tkData.tkFloat) >= Float2Pow53))
            {
                APLFLOAT aplFlt = lptkRhtArg->tkData.tkFloat,
                         aplFlr = floor (aplFlt);

                // If this FLT is an INT,
                //   and if -0 is allowed, the value is not -0, ...
                if (aplFlt EQ aplFlr
                 && !(gAllowNeg0 && aplFlt EQ 0 && SIGN_APLFLOAT (aplFlt)))
                {
                    // Convert it
                    lptkRhtArg->tkFlags.ImmType  = IMMTYPE_INT;
                    lptkRhtArg->tkData.tkInteger = (APLINT) aplFlr;;
                } // End IF
            } // End IF/ELSE/...

            return;

        case TKT_VARARRAY:
            hGlbRht = lptkRhtArg->tkData.tkGlbData;

            break;

        defstop
            break;
    } // End SWITCH

    // Copy to the result in case we don't demote
    hGlbRes = hGlbRht;

    // Lock the memory to get a ptr to it
    lpMemHdrRht = MyGlobalLockVar (hGlbRht);

    // Get the Type, NELM, and Rank
    aplTypeRht = lpMemHdrRht->ArrType;
    aplNELMRht = lpMemHdrRht->NELM;
    aplRankRht = lpMemHdrRht->Rank;

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

    // If the right arg is a simple scalar,
    //   convert it to an immediate
    if (IsSimple (aplTypeRht)
     && IsScalar (aplRankRht))
    {
        APLLONGEST aplLongestRht;
        IMM_TYPES  immTypeRht;

        // Get the immediate value, making sure we don't
        //   overextend beyond the bounds of the right arg
        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
                aplLongestRht = BIT0 & *(LPAPLBOOL) lpMemRht;
                immTypeRht    = IMMTYPE_BOOL;

                break;

            case ARRAY_INT:
                aplLongestRht = *(LPAPLINT)     lpMemRht;
                immTypeRht    = IMMTYPE_INT;

                break;

            case ARRAY_FLOAT:
                aplLongestRht = *(LPAPLLONGEST) lpMemRht;
                immTypeRht    = IMMTYPE_FLOAT;

                break;

            case ARRAY_CHAR:
                aplLongestRht = *(LPAPLCHAR)    lpMemRht;
                immTypeRht    = IMMTYPE_CHAR;

                break;

            case ARRAY_APA:
#define lpAPA           ((LPAPLAPA) lpMemRht)
                aplLongestRht = lpAPA->Off;
                immTypeRht    = IMMTYPE_INT;
#undef  lpAPA
                break;

            case ARRAY_HETERO:
                // Split cases based upon the item's ptr type
                switch (GetPtrTypeInd (lpMemRht))
                {
                    case PTRTYPE_STCONST:
                        switch ((*(LPSYMENTRY *) lpMemRht)->stFlags.ImmType)
                        {
                            case IMMTYPE_BOOL:
                                aplLongestRht = BIT0 & (*(LPSYMENTRY *) lpMemRht)->stData.stBoolean;
                                immTypeRht    = IMMTYPE_BOOL;

                                break;

                            case IMMTYPE_INT:
                                aplLongestRht = (*(LPSYMENTRY *) lpMemRht)->stData.stInteger;
                                immTypeRht    = IMMTYPE_INT;

                                break;

                            case IMMTYPE_FLOAT:
                                aplLongestRht = *(LPAPLLONGEST) &(*(LPSYMENTRY *) lpMemRht)->stData.stFloat;
                                immTypeRht    = IMMTYPE_FLOAT;

                                break;

                            case IMMTYPE_CHAR:
                                aplLongestRht = (*(LPSYMENTRY *) lpMemRht)->stData.stChar;
                                immTypeRht    = IMMTYPE_CHAR;

                                break;

                            case IMMTYPE_RAT:
                                // Get the global memory handle
                                hGlbSub = ClrPtrTypeInd (lpMemRht);

                                // Lock the memory to get a handle on it
                                lpMemHdrSub = MyGlobalLockVar (hGlbSub);

                                // Skip over the header and dimensions to the data
                                lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                                // Copy the data
                                lptkRhtArg->tkFlags.TknType  = TKT_VARARRAY;
                                lptkRhtArg->tkFlags.ImmType  = IMMTYPE_RAT;
                                lptkRhtArg->tkData.tkGlbData =
                                  MakeGlbEntry_EM (ARRAY_RAT,               // Entry type
                                                   (LPAPLRAT) lpMemSub,     // Ptr to the value
                                                   TRUE,                    // TRUE iff we should initialize the target first
                                                   lptkRhtArg);             // Ptr to function token
                                // We no longer need this ptr
                                MyGlobalUnlock (hGlbSub); lpMemHdrSub = NULL;

                                // Check for error
                                if (lptkRhtArg->tkData.tkGlbData EQ NULL)
                                    goto WSFULL_EXIT;
                                goto UNLOCK_EXIT;

                            case IMMTYPE_VFP:
                                // Get the global memory handle
                                hGlbSub = ClrPtrTypeInd (lpMemRht);

                                // Lock the memory to get a handle on it
                                lpMemHdrSub = MyGlobalLockVar (hGlbSub);

                                // Skip over the header and dimensions to the data
                                lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                                // Copy the data
                                lptkRhtArg->tkFlags.TknType  = TKT_VARARRAY;
                                lptkRhtArg->tkFlags.ImmType  = IMMTYPE_VFP;
                                lptkRhtArg->tkData.tkGlbData =
                                  MakeGlbEntry_EM (ARRAY_VFP,               // Entry type
                                                   (LPAPLVFP) lpMemSub,     // Ptr to the value
                                                   TRUE,                    // TRUE iff we should initialize the target first
                                                   lptkRhtArg);             // Ptr to function token
                                // We no longer need this ptr
                                MyGlobalUnlock (hGlbSub); lpMemHdrSub = NULL;

                                // Check for error
                                if (lptkRhtArg->tkData.tkGlbData EQ NULL)
                                    goto WSFULL_EXIT;
                                goto UNLOCK_EXIT;

                            defstop
                                break;
                        } // End SWITCH

                        break;

                    case PTRTYPE_HGLOBAL:
                        // Get the global memory handle
                        hGlbSub = ClrPtrTypeInd (lpMemRht);

                        // Lock the memory to get a handle on it
                        lpMemHdrSub = MyGlobalLockVar (hGlbSub);

                        Assert (IsScalar (lpMemHdrSub->Rank));

                        // Get the array storage type
                        aplTypeSub = lpMemHdrSub->ArrType;

                        // Skip over the header and dimensions to the data
                        lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                        // Split cases based upon the array storage type
                        switch (aplTypeSub)
                        {
                            case ARRAY_RAT:
                                // Copy the data
                                lptkRhtArg->tkFlags.TknType  = TKT_VARARRAY;
                                lptkRhtArg->tkFlags.ImmType  = IMMTYPE_RAT;
                                lptkRhtArg->tkData.tkGlbData =
                                  MakeGlbEntry_EM (ARRAY_RAT,               // Entry type
                                                   (LPAPLRAT) lpMemSub,     // Ptr to the value
                                                   TRUE,                    // TRUE iff we should initialize the target first
                                                   lptkRhtArg);             // Ptr to function token
                                break;

                            case ARRAY_VFP:
                                // Copy the data
                                lptkRhtArg->tkFlags.TknType  = TKT_VARARRAY;
                                lptkRhtArg->tkFlags.ImmType  = IMMTYPE_VFP;
                                lptkRhtArg->tkData.tkGlbData =
                                  MakeGlbEntry_EM (ARRAY_VFP,               // Entry type
                                                   (LPAPLVFP) lpMemSub,     // Ptr to the value
                                                   TRUE,                    // TRUE iff we should initialize the target first
                                                   lptkRhtArg);             // Ptr to function token
                                break;

                            defstop
                                break;
                        } // End SWITCH

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbSub); lpMemHdrSub = NULL;

                        // Check for error
                        if (lptkRhtArg->tkData.tkGlbData EQ NULL)
                            goto WSFULL_EXIT;
                        goto UNLOCK_EXIT;

                        break;

                    defstop
                        break;
                } // End SWITCH

                break;

            defstop
                break;
        } // End SWITCH

        // Split cases based upon the right arg token type
        switch (lptkRhtArg->tkFlags.TknType)
        {
            case TKT_VARNAMED:
                // Fill in the result token
                lptkRhtArg->tkData.tkSym->stFlags.Imm      = TRUE;
                lptkRhtArg->tkData.tkSym->stFlags.ImmType  = immTypeRht;
                lptkRhtArg->tkData.tkSym->stData.stLongest = aplLongestRht;

                break;

            case TKT_VARARRAY:
                // Fill in the result token
                lptkRhtArg->tkFlags.TknType  = TKT_VARIMMED;
                lptkRhtArg->tkFlags.ImmType  = immTypeRht;
                lptkRhtArg->tkData.tkLongest = aplLongestRht;

                break;

            defstop
                break;
        } // End SWITCH
UNLOCK_EXIT:
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

        // We no longer need this storage
        FreeResultGlobalVar (hGlbRht); hGlbRht = NULL;

        goto IMMED_EXIT;
    } // End IF

    // Split cases based upon the right arg's storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:        // No place to go
        case ARRAY_APA:         // ...
        case ARRAY_CHAR:        // ...
        case ARRAY_RAT:         // ...
        case ARRAY_VFP:         // ...
            goto IMMED_EXIT;

        case ARRAY_INT:         // Demote to Boolean
            // Start with lowest type
            aplTypeRes = ARRAY_BOOL;

            // Loop through the elements
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                aplInteger = *((LPAPLINT) lpMemRht)++;

                // Check for Boolean value
                if (!IsBooleanValue (aplInteger))
                {
                    aplTypeRes = ARRAY_INT;

                    break;
                } // End IF
            } // End FOR

            break;

        case ARRAY_FLOAT:       // Demote to Boolean/Integer
            // Start with lowest type
            aplTypeRes = ARRAY_BOOL;

            // Loop through the elements
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                aplFloat = *((LPAPLFLOAT) lpMemRht)++;

                // If it's -0, ...
                if (gAllowNeg0
                 && SIGN_APLFLOAT (aplFloat) && aplFloat EQ 0)
                    aplTypeSub = ARRAY_FLOAT;
                else
                // Check for Boolean value
                if (IsBooleanValue (aplFloat))
                    aplTypeSub = ARRAY_BOOL;
                else
                // Check for Integer value
                if (aplFloat EQ floor (aplFloat)
                 && fabs (aplFloat) < Float2Pow53)
                    aplTypeSub = ARRAY_INT;
                else
                    aplTypeSub = ARRAY_FLOAT;

                // Check storage type
                aplTypeRes = aTypePromote[aplTypeRes][aplTypeSub];

                // If we're back to the same type, quit
                if (aplTypeRes EQ aplTypeRht)
                    break;
            } // End FOR

            break;

        case ARRAY_HETERO:      // Demote to simple homogeneous
        case ARRAY_NESTED:      // Demote to simple homogeneous/heterogeneous
            // Initialize the type
            aplTypeRes = ARRAY_INIT;

            // Take into account nested prototypes
            if (IsNested (aplTypeRht))
                aplNELMNstRht = max (aplNELMRht, 1);
            else
                aplNELMNstRht = aplNELMRht;

            // Loop through the elements
            for (uRht = 0; uRht < aplNELMNstRht; uRht++, ((LPAPLHETERO) lpMemRht)++)
            // Split cases based upon the ptr type of the element
            switch (GetPtrTypeInd (lpMemRht))
            {
                case PTRTYPE_STCONST:
                    lpSymEntry = *(LPAPLHETERO) lpMemRht;

                    // stData is an immediate
                    Assert (lpSymEntry->stFlags.Imm);

                    // Get the immediate type (translated to an ARRAY_TYPE)
                    aplTypeSub = TranslateImmTypeToArrayType (lpSymEntry->stFlags.ImmType);

                    // Check for various possible type demotions
                    switch (aplTypeSub)
                    {
                        case ARRAY_FLOAT:   // Check for demotion from FLOAT to INT/BOOL
                            aplFloat = lpSymEntry->stData.stFloat;

                            // If it's -0, ...
                            if (gAllowNeg0
                             && SIGN_APLFLOAT (aplFloat) && aplFloat EQ 0)
                                aplTypeSub = ARRAY_FLOAT;
                            else
                            if (IsBooleanValue (aplFloat))
                                aplTypeSub = ARRAY_BOOL;
                            else
                            if (aplFloat EQ floor (aplFloat)
                             && fabs (aplFloat) < Float2Pow53)
                                aplTypeSub = ARRAY_INT;
                            break;

                        case ARRAY_INT:     // Check for demotion from INT to BOOL
                            aplInteger = lpSymEntry->stData.stInteger;

                            if (IsBooleanValue (aplInteger))
                                aplTypeSub = ARRAY_BOOL;
                            break;

                        case ARRAY_BOOL:    // No type demotion
                        case ARRAY_CHAR:    // No type demotion
                        default:
                            break;
                    } // End SWITCH

                    // Check storage type
                    aplTypeRes = aTypePromote[aplTypeRes][aplTypeSub];

                    // Check for no demotion
                    if (IsSimpleHet (aplTypeRes)
                     && IsSimpleHet (aplTypeRht))
                        goto NORMAL_EXIT;
                    break;

                case PTRTYPE_HGLOBAL:
                    // Get the global memory handle
                    hGlbSub = ClrPtrTypeInd (lpMemRht);

                    // Get the global's Type
                    AttrsOfGlb (hGlbSub, &aplTypeSub, NULL, &aplRankSub, NULL);

                    // If it's not a scalar, ...
                    if (!IsScalar (aplRankSub))
                        // It's nested, so there's no demotion
                        goto NORMAL_EXIT;

                    // Split cases based upon the item storage type
                    switch (aplTypeSub)
                    {
                        case ARRAY_BOOL:
                        case ARRAY_INT:
                        case ARRAY_FLOAT:
                        case ARRAY_NESTED:
                            // Check storage type
                            aplTypeRes = aTypePromote[aplTypeRes][aplTypeSub];

                            break;

                        case ARRAY_RAT:
                        case ARRAY_VFP:
                            // Check storage type
                            aplTypeRes = aTypePromote[aplTypeRes][aplTypeSub];

                            // Check for no demotion
                            if (!IsRat (aplTypeRes)
                             && !IsVfp (aplTypeRes))
                                goto NORMAL_EXIT;
                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                defstop
                    break;
            } // End FOR/SWITCH

            break;

        defstop
            break;
    } // End SWITCH

    // If the result type is the same as the right arg type,
    //   there's no demotion
    if (aplTypeRes EQ aplTypeRht)
        goto NORMAL_EXIT;

    // Skip over header and dimensions to the data
    //   as the above SWITCH stmt may have modified lpMemRht
    lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

    // Check for demotion from Nested to Hetero
    if (IsSimpleHet (aplTypeRes)
     && IsNested (aplTypeRht))
    {
        // If the reference count of this array is one, just
        //   change the array type from ARRAY_NESTED to ARRAY_HETERO.
        if (lpMemHdrRht->RefCnt EQ 1)
            lpMemHdrRht->ArrType = ARRAY_HETERO;
        else
        {
            // Copy this array and change the type from ARRAY_NESTED to ARRAY_HETERO
            dwSize = MyGlobalSize (hGlbRht);
            hGlbRes = DbgGlobalAlloc (GHND, dwSize);
            if (hGlbRes NE NULL)
            {
                // Lock the memory to get a ptr to it
                lpMemHdrRes = MyGlobalLock000 (hGlbRes);

                // Copy source to destin
                CopyMemory (lpMemHdrRes, lpMemHdrRht, dwSize);

#ifdef DEBUG_REFCNT
                dprintfWL0 (L"##RefCnt=1 in " APPEND_NAME L": %p(res=1) (%S#%d)", hGlbRes, FNLN);
#endif
#define lpHeader    lpMemHdrRes
                // Set the reference count and array type
                lpHeader->RefCnt  = 1;
                lpHeader->ArrType = ARRAY_HETERO;
#undef  lpHeader
                // We no longer need these ptrs
                MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
                MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

                // Free the old array
                FreeResultGlobalVar (hGlbRht); hGlbRht = NULL;
            } else
                // WS FULL, so no demotion
                hGlbRes = hGlbRht;
        } // End IF/ELSE
    } else
    // Check for demotion to simple homogeneous
    if (uTypeMap[aplTypeRes] < min (uTypeMap[ARRAY_HETERO], uTypeMap[aplTypeRht]))
    {
        // Handle simple scalars as immediates
        if (IsScalar (aplRankRht)
         && IsSimple (aplTypeRes))
        {
            LPAPLLONGEST lpaplLongestRht;

            // Split cases based upon the right arg token type
            switch (lptkRhtArg->tkFlags.TknType)
            {
                case TKT_VARNAMED:
                    // Fill in the result token
                    lptkRhtArg->tkData.tkSym->stFlags.Imm      = TRUE;
                    lptkRhtArg->tkData.tkSym->stFlags.ImmType  = TranslateArrayTypeToImmType (aplTypeRes);
                    lptkRhtArg->tkData.tkSym->stData.stLongest = 0;
                    lpaplLongestRht = &lptkRhtArg->tkData.tkSym->stData.stLongest;

                    break;

                case TKT_VARARRAY:
                    // Fill in the result token
                    lptkRhtArg->tkFlags.TknType  = TKT_VARIMMED;
                    lptkRhtArg->tkFlags.ImmType  = TranslateArrayTypeToImmType (aplTypeRes);
                    lptkRhtArg->tkData.tkLongest = 0;
                    lpaplLongestRht = GetPtrTknLongest (lptkRhtArg);

                    break;

                defstop
                    break;
            } // End SWITCH

            // Skip over header and dimensions to the data
            lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

            // Demote the data in the right arg, copying it to the result
            DemoteData (aplTypeRes,
                        lpaplLongestRht,
                        aplTypeRht,
                        aplNELMRht,
                        lpMemRht);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

            // Free the old array
            FreeResultGlobalVar (hGlbRht); hGlbRht = NULL;

            goto IMMED_EXIT;
        } // End IF

        // Calculate space needed for the result
        ByteRes = CalcArraySize (aplTypeRes, aplNELMRht, aplRankRht);

        // Check for overflow
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;

        // Allocate space for a new array
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
        lpHeader->NELM       = aplNELMRht;
        lpHeader->Rank       = aplRankRht;
#undef  lpHeader

        // Skip over the header to the dimensions
        lpMemRht = VarArrayBaseToDim (lpMemHdrRht);
        lpMemRes = VarArrayBaseToDim (lpMemHdrRes);

        // Copy the dimensions to the result
        for (uRht = 0; uRht < aplRankRht; uRht++)
            *((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemRht)++;

        // Now, lpMemRes and lpMemRht both point to their
        //   respective data

        // Demote the data in the right arg, copying it to the result
        DemoteData (aplTypeRes,
                    lpMemRes,
                    aplTypeRht,
                    aplNELMRht,
                    lpMemRht);
        // We no longer need these ptrs
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

        // Free the old array
        FreeResultGlobalVar (hGlbRht); hGlbRht = NULL;
    } else
    // Check for demotion from Nested or Hetero to Rational
    if ((IsSimpleHet (aplTypeRht)
      || IsNested (aplTypeRht))
     && (IsRat (aplTypeRes)
      || IsVfp (aplTypeRes)))
    {
        // Calculate space needed for the result
        ByteRes = CalcArraySize (aplTypeRes, aplNELMRht, aplRankRht);

        // Check for overflow
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;

        // Allocate space for a new array
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
        lpHeader->NELM       = aplNELMRht;
        lpHeader->Rank       = aplRankRht;
#undef  lpHeader

        // Skip over the header to the dimensions
        lpMemRht = VarArrayBaseToDim (lpMemHdrRht);
        lpMemRes = VarArrayBaseToDim (lpMemHdrRes);

        // Copy the dimensions to the result
        for (uRht = 0; uRht < aplRankRht; uRht++)
            *((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemRht)++;

        // Now, lpMemRes and lpMemRht both point to their
        //   respective data

        // Demote the data in the right arg, copying it to the result
        DemoteData (aplTypeRes,
                    lpMemRes,
                    aplTypeRht,
                    aplNELMRht,
                    lpMemRht);
        // We no longer need these ptrs
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

        // Free the old array
        FreeResultGlobalVar (hGlbRht); hGlbRht = NULL;
    } // End IF/ELSE

    goto NORMAL_EXIT;

WSFULL_EXIT:
    // WS FULL, so no demotion
    hGlbRes = hGlbRht;
NORMAL_EXIT:
    // Split cases based upon the right arg token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // Save the result in the right arg token
            lptkRhtArg->tkData.tkSym->stData.stGlbData = MakePtrTypeGlb (hGlbRes);

            break;

        case TKT_VARARRAY:
            // Save the result in the right arg token
            lptkRhtArg->tkData.tkGlbData = MakePtrTypeGlb (hGlbRes);

            break;

        case TKT_VARIMMED:      // Handled above
            break;

        defstop
            break;
    } // End SWITCH
IMMED_EXIT:
    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF
} // End TypeDemote
#undef  APPEND_NAME


//***************************************************************************
//  $TypeDemoteGlb
//
//  Attempt to type demote the data in a global memory handle
//***************************************************************************

HGLOBAL TypeDemoteGlb
    (HGLOBAL hGlbRht)               // Right arg global memory handle

{
    TOKEN      tkRhtArg = {0};      // Right arg token
    LPSYMENTRY lpSymEntry;          // Result if immediate

    tkRhtArg.tkFlags.TknType   = TKT_VARARRAY;
////tkRhtArg.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from = {0}
////tkRhtArg.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
    tkRhtArg.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRht);
////tkRhtArg.tkCharIndex       =                // Unused

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&tkRhtArg);

    // Split cases based upon the token type
    switch (tkRhtArg.tkFlags.TknType)
    {
        case TKT_VARIMMED:
            lpSymEntry =
              MakeSymEntry_EM (tkRhtArg.tkFlags.ImmType,
                               GetPtrTknLongest (&tkRhtArg),
                               NULL);
            if (lpSymEntry EQ NULL)
                return hGlbRht;
            else
                return lpSymEntry;

        case TKT_VARARRAY:
            return tkRhtArg.tkData.tkGlbData;

        defstop
            return NULL;
    } // End SWITCH
} // End TypeDemoteGlb


//***************************************************************************
//  $DemoteData
//
//  Demote the data in the right arg, copying it to the result
//***************************************************************************

void DemoteData
    (APLSTYPE aplTypeRes,                   // Result storage type
     LPVOID   lpMemRes,                     // Ptr to result global memory
     APLSTYPE aplTypeRht,                   // Right arg storage type
     APLNELM  aplNELMRht,                   // Right arg NELM
     LPVOID   lpMemRht)                     // Ptr to right arg global memory

{
    APLUINT           uRht;                 // Loop counter
    UINT              uBitIndex;            // Bit index for looping through Booleans
    LPSYMENTRY        lpSymGlbSub;          // Ptr to temp SYMENTRY/HGLOBAL
    LPVOID            lpMemSub;             // Ptr to temp global memory
    LPVARARRAY_HEADER lpMemHdrSub = NULL;   // Ptr to temp memory header

    // Split cases based upon the result's storage type
    // Note that the result is always of lower type than
    //   the right arg.
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:            // Res = BOOL, Rht = INT/FLOAT/HETERO/NESTED
            uBitIndex = 0;

            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_INT:     // Res = BOOL, Rht = INT
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        Assert (IsBooleanValue (*(LPAPLINT) lpMemRht));

                        *((LPAPLBOOL) lpMemRes) |= (*((LPAPLINT) lpMemRht)++) << uBitIndex;

                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;              // Start over
                            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_FLOAT:   // Res = BOOL, Rht = FLOAT
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        Assert (IsBooleanValue (*(LPAPLFLOAT) lpMemRht));

                        *((LPAPLBOOL) lpMemRes) |= ((APLBOOL) *((LPAPLFLOAT) lpMemRht)++) << uBitIndex;

                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;              // Start over
                            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_HETERO:  // Res = BOOL, Rht = HETERO
                case ARRAY_NESTED:  // Res = BOOL, Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Get the SymGlb ptr
                        lpSymGlbSub = *((LPAPLHETERO) lpMemRht)++;

                        // Split cases based upon the ptr type bits
                        switch (GetPtrTypeDir (lpSymGlbSub))
                        {
                            case PTRTYPE_STCONST:
                                // Split cases based upon the immediate type
                                switch (lpSymGlbSub->stFlags.ImmType)
                                {
                                    case IMMTYPE_BOOL:
                                        Assert (IsBooleanValue (lpSymGlbSub->stData.stBoolean));

                                        *((LPAPLBOOL) lpMemRes) |= (lpSymGlbSub->stData.stBoolean) << uBitIndex;

                                        break;

                                    case IMMTYPE_INT:
                                        Assert (IsBooleanValue (lpSymGlbSub->stData.stInteger));

                                        *((LPAPLBOOL) lpMemRes) |= (lpSymGlbSub->stData.stInteger) << uBitIndex;

                                        break;

                                    case IMMTYPE_FLOAT:
                                        Assert (IsBooleanValue (lpSymGlbSub->stData.stFloat));

                                        *((LPAPLBOOL) lpMemRes) |= ((APLINT) (lpSymGlbSub->stData.stFloat)) << uBitIndex;

                                        break;

                                    case IMMTYPE_CHAR:
                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            case PTRTYPE_HGLOBAL:
                                // Lock the memory to get a ptr to it
                                lpMemHdrSub = MyGlobalLockVar (lpSymGlbSub);

                                Assert (IsScalar (lpMemHdrSub->Rank));

                                // Skip over the header and dimensions to the data
                                lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                                // Split cases based upon the storage type
                                switch (lpMemHdrSub->ArrType)
                                {
                                    case ARRAY_BOOL:    // Res = BOOL, Rht = BOOL
                                    case ARRAY_INT:     // Res = BOOL, Rht = INT
                                    case ARRAY_APA:     // Res = BOOL, Rht = APA
                                    case ARRAY_FLOAT:   // Res = BOOL, Rht = FLT
                                        *((LPAPLBOOL) lpMemRes) |= GetNextInteger (lpMemSub, lpMemHdrSub->ArrType, uRht) << uBitIndex;

                                        break;

                                    case ARRAY_RAT:     // Res = BOOL, Rht = RAT
                                    case ARRAY_VFP:     // Res = BOOL, Rht = VFP
                                                        // We don't demote RAT/VFP to BOOL
                                    defstop
                                        break;
                                } // End SWITCH

                                // We no longer need this ptr
                                MyGlobalUnlock (lpSymGlbSub); lpMemHdrSub = NULL;

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

                    break;

                case ARRAY_RAT:     // Res = BOOL, Rht = RAT
                case ARRAY_VFP:     // Res = BOOL, Rht = VFP
                                    // We don't demote RAT/VFP to BOOL
                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_INT:         // Res = INT, Rht = FLOAT/HETERO/NESTED
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_FLOAT:   // Res = INT , Rht = FLOAT
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                        *((LPAPLINT) lpMemRes)++ = (APLINT) *((LPAPLFLOAT) lpMemRht)++;
                    break;

                case ARRAY_HETERO:  // Res = INT , Rht = HETERO
                case ARRAY_NESTED:  // Res = INT , Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Get the SymGlb ptr
                        lpSymGlbSub = *((LPAPLHETERO) lpMemRht)++;

                        // Split cases based upon the ptr type bits
                        switch (GetPtrTypeDir (lpSymGlbSub))
                        {
                            case PTRTYPE_STCONST:
                                // Split cases based upon the immediate type
                                switch (lpSymGlbSub->stFlags.ImmType)
                                {
                                    case IMMTYPE_BOOL:  // Res = INT , Rht = BOOL
                                        *((LPAPLINT) lpMemRes)++ = lpSymGlbSub->stData.stBoolean;

                                        break;

                                    case IMMTYPE_INT:   // Res = INT , Rht = INT
                                        *((LPAPLINT) lpMemRes)++ = lpSymGlbSub->stData.stInteger;

                                        break;

                                    case IMMTYPE_FLOAT: // Res = INT , Rht = FLT
                                        *((LPAPLINT) lpMemRes)++ = (APLINT) lpSymGlbSub->stData.stFloat;

                                        break;

                                    case IMMTYPE_CHAR:  // Res = INT , Rht = CHAR
                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            case PTRTYPE_HGLOBAL:
                                // Lock the memory to get a ptr to it
                                lpMemHdrSub = MyGlobalLockVar (lpSymGlbSub);

                                Assert (IsScalar (lpMemHdrSub->Rank));

                                // Skip over the header and dimensions to the data
                                lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                                // Split cases based upon the storage type
                                switch (lpMemHdrSub->ArrType)
                                {
                                    case ARRAY_BOOL:    // Res = INT , Rht = BOOL
                                    case ARRAY_INT:     // Res = INT , Rht = INT
                                    case ARRAY_APA:     // Res = INT , Rht = APA
                                    case ARRAY_FLOAT:   // Res = INT , Rht = FLT
                                        *((LPAPLINT) lpMemRes)++ = GetNextInteger (lpMemSub, lpMemHdrSub->ArrType, uRht);

                                        break;

                                    case ARRAY_RAT:     // Res = INT , Rht = RAT
                                    case ARRAY_VFP:     // Res = INT , Rht = VFP
                                                        // We don't demote RAT/VFP to INT
                                    defstop
                                        break;
                                } // End SWITCH

                                // We no longer need this ptr
                                MyGlobalUnlock (lpSymGlbSub); lpMemHdrSub = NULL;

                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End FOR

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_FLOAT:       // Res = FLOAT, Rht = HETERO/NESTED
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_HETERO:  // Res = FLOAT, Rht = HETERO
                case ARRAY_NESTED:  // Res = FLOAT, Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Get the SymGlb ptr
                        lpSymGlbSub = *((LPAPLHETERO) lpMemRht)++;

                        // Split cases based upon the ptr type bits
                        switch (GetPtrTypeDir (lpSymGlbSub))
                        {
                            case PTRTYPE_STCONST:
                                // Split cases based upon the immediate type
                                switch (lpSymGlbSub->stFlags.ImmType)
                                {
                                    case IMMTYPE_BOOL:
                                        *((LPAPLFLOAT) lpMemRes)++ = (APLFLOAT) lpSymGlbSub->stData.stBoolean;

                                        break;

                                    case IMMTYPE_INT:
                                        *((LPAPLFLOAT) lpMemRes)++ = (APLFLOAT) lpSymGlbSub->stData.stInteger;

                                        break;

                                    case IMMTYPE_FLOAT:
                                        *((LPAPLFLOAT) lpMemRes)++ = lpSymGlbSub->stData.stFloat;

                                        break;

                                    case IMMTYPE_CHAR:
                                    defstop
                                        break;
                                } // End FOR/SWITCH

                                break;

                            case PTRTYPE_HGLOBAL:
                                // Lock the memory to get a ptr to it
                                lpMemHdrSub = MyGlobalLockVar (lpSymGlbSub);

                                Assert (IsScalar (lpMemHdrSub->Rank));

                                // Skip over the header and dimensions to the data
                                lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                                // Split cases based upon the storage type
                                switch (lpMemHdrSub->ArrType)
                                {
                                    case ARRAY_BOOL:    // Res = FLT , Rht = BOOL
                                    case ARRAY_INT:     // Res = FLT , Rht = INT
                                    case ARRAY_APA:     // Res = FLT , Rht = APA
                                    case ARRAY_FLOAT:   // Res = FLT , Rht = FLT
                                        *((LPAPLFLOAT) lpMemRes)++ = GetNextFloat (lpMemSub, lpMemHdrSub->ArrType, uRht);

                                        break;

                                    case ARRAY_RAT:     // Res = FLT , Rht = RAT
                                    case ARRAY_VFP:     // Res = FLT , Rht = VFP
                                                        // We don't demote RAT/VFP to FLT
                                    defstop
                                        break;
                                } // End SWITCH

                                // We no longer need this ptr
                                MyGlobalUnlock (lpSymGlbSub); lpMemHdrSub = NULL;

                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End FOR

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_CHAR:        // Res = CHAR, Rht = HETERO/NESTED
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_HETERO:  // Res = CHAR, Rht = HETERO
                case ARRAY_NESTED:  // Res = CHAR, Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                        *((LPAPLCHAR) lpMemRes)++ = (*((LPAPLHETERO) lpMemRht)++)->stData.stChar;
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_RAT:
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_HETERO:  // Res = RAT, Rht = HETERO
                case ARRAY_NESTED:  // Res = RAT, Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Get the SymGlb ptr
                        lpSymGlbSub = *((LPAPLNESTED) lpMemRht)++;

                        // Split cases based upon the ptr type bits
                        switch (GetPtrTypeDir (lpSymGlbSub))
                        {
                            case PTRTYPE_STCONST:
                                // Split cases based upon the immediate storage type
                                switch (lpSymGlbSub->stFlags.ImmType)
                                {
                                    case IMMTYPE_BOOL:
                                        // Convert the data to RAT
                                        mpq_init_set_sx (((LPAPLRAT) lpMemRes)++, BIT0 & lpSymGlbSub->stData.stInteger, 1);

                                        break;

                                    case IMMTYPE_INT:
                                        // Convert the data to RAT
                                        mpq_init_set_sx (((LPAPLRAT) lpMemRes)++, lpSymGlbSub->stData.stInteger, 1);

                                        break;

                                    case IMMTYPE_FLOAT:
                                        // Convert the data to RAT
                                        mpq_init_set_d  (((LPAPLRAT) lpMemRes)++, lpSymGlbSub->stData.stFloat);

                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            case PTRTYPE_HGLOBAL:
                                // Lock the memory to get a ptr to it
                                lpMemHdrSub = MyGlobalLockVar (lpSymGlbSub);

                                Assert (IsScalar (lpMemHdrSub->Rank));

                                // Skip over the header and dimensions to the data
                                lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                                // Copy the data
                                mpq_init_set (((LPAPLRAT) lpMemRes)++, (LPAPLRAT) lpMemSub);

                                // We no longer need this ptr
                                MyGlobalUnlock (lpSymGlbSub); lpMemHdrSub = NULL;

                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End FOR

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_VFP:
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_HETERO:  // Res = VFP, Rht = HETERO
                case ARRAY_NESTED:  // Res = VFP, Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Get the SymGlb ptr
                        lpSymGlbSub = *((LPAPLNESTED) lpMemRht)++;

                        // Split cases based upon the ptr type bits
                        switch (GetPtrTypeDir (lpSymGlbSub))
                        {
                            case PTRTYPE_STCONST:
                                // Split cases based upon the immediate storage type
                                switch (lpSymGlbSub->stFlags.ImmType)
                                {
                                    case IMMTYPE_BOOL:
                                        // Convert the data to VFP
                                        mpfr_init_set_sx (((LPAPLVFP) lpMemRes)++, BIT0 & lpSymGlbSub->stData.stInteger, MPFR_RNDN);

                                        break;

                                    case IMMTYPE_INT:
                                        // Convert the data to VFP
                                        mpfr_init_set_sx (((LPAPLVFP) lpMemRes)++, lpSymGlbSub->stData.stInteger, MPFR_RNDN);

                                        break;

                                    case IMMTYPE_FLOAT:
                                        // Convert the data to VFP
                                        mpfr_init_set_d  (((LPAPLVFP) lpMemRes)++, lpSymGlbSub->stData.stFloat, MPFR_RNDN);

                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            case PTRTYPE_HGLOBAL:
                                // Lock the memory to get a ptr to it
                                lpMemHdrSub = MyGlobalLockVar (lpSymGlbSub);

                                Assert (IsScalar (lpMemHdrSub->Rank));

                                // Skip over the header and dimensions to the data
                                lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                                // Copy the data
                                mpfr_init_copy (((LPAPLVFP) lpMemRes)++, (LPAPLVFP) lpMemSub);

                                // We no longer need this ptr
                                MyGlobalUnlock (lpSymGlbSub); lpMemHdrSub = NULL;

                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End FOR

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        defstop
            break;
    } // End SWITCH
} // End DemoteData


//***************************************************************************
//  $TypePromote_EM
//
//  Promote the type of the token to a given storage type
//***************************************************************************

UBOOL TypePromote_EM
    (LPTOKEN  lpToken,              // Ptr to the token
     APLSTYPE aplTypeRes,           // The result storage type
     LPTOKEN  lptkFunc)             // Ptr to function token

{
    HGLOBAL *lphGlbArg;             // Ptr to where the HGLOBAL came from

    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                // Point to the HGLOBAL
                lphGlbArg = &lpToken->tkData.tkSym->stData.stGlbData;

                break;      // Continue below with global case
            } // End IF

            // Split cases based upon the result storage type
            switch (aplTypeRes)
            {
                case ARRAY_INT:         // B -> I
                    lpToken->tkData.tkInteger = lpToken->tkData.tkSym->stData.stBoolean;

                    break;

                case ARRAY_FLOAT:       // B/I -> F
                    lpToken->tkData.tkSym->stData.stFloat = (APLFLOAT) lpToken->tkData.tkSym->stData.stInteger;

                    break;

                defstop
                    break;
            } // End SWITCH

            // Change the immediate type in the token
            lpToken->tkData.tkSym->stFlags.ImmType = TranslateArrayTypeToImmType (aplTypeRes);

            return TRUE;

        case TKT_VARIMMED:
            // Split cases based upon the result storage type
            switch (aplTypeRes)
            {
                case ARRAY_INT:         // B -> I
                    lpToken->tkData.tkInteger = lpToken->tkData.tkBoolean;

                    break;

                case ARRAY_FLOAT:       // B/I -> F
                    lpToken->tkData.tkFloat = (APLFLOAT) lpToken->tkData.tkInteger;

                    break;

                defstop
                    break;
            } // End SWITCH

            // Change the immediate type in the token
            lpToken->tkFlags.ImmType = TranslateArrayTypeToImmType (aplTypeRes);

            return TRUE;

        case TKT_VARARRAY:
            // Point to the HGLOBAL
            lphGlbArg = &lpToken->tkData.tkGlbData;

            break;      // Continue below with global case

        defstop
            break;
    } // End SWITCH

    // tk/stData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir_PTB (*lphGlbArg));

    // Handle the HGLOBAL case
    return TypePromoteGlb_EM (lphGlbArg, aplTypeRes, lptkFunc);
} // End TypePromote_EM


//***************************************************************************
//  $TypePromoteGlb_EM
//
//  Promote the type of the global memory handle to a given storage type
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- TypePromoteGlb_EM"
#else
#define APPEND_NAME
#endif

UBOOL TypePromoteGlb_EM
    (HGLOBAL *lphGlbArg,            // Ptr to global memory handle
     APLSTYPE aplTypeRes,           // The result storage type
     LPTOKEN  lptkFunc)             // Ptr to function token

{
    HGLOBAL           hGlbArg,              // Arg    ...
                      hGlbRes = NULL;       // Result global memory handle
    UBOOL             bRet = FALSE;         // TRUE iff the result is valid
    LPVARARRAY_HEADER lpMemHdrArg = NULL,   // Ptr to arg header
                      lpMemHdrRes = NULL;   // ...    result ...
    LPAPLDIM          lpMemDimArg,          // ...    arg dimensions
                      lpMemDimRes;          // ...    result ...
    LPVOID            lpMemArg,             // ...    arg global memory
                      lpMemRes;             // ...    result ...
    APLSTYPE          aplTypeArg;           // Arg storage type of HGLOBAL
    APLNELM           aplNELMArg;           // Arg NELM         ...
    APLRANK           aplRankArg;           // Arg Rank         ...
    APLUINT           ByteRes;              // # bytes in the result
    LPSYMENTRY        lpSymTmp;             // Ptr to temporary LPSYMENTRY

    // Copy the HGLOBAL
    hGlbArg = *lphGlbArg;

    // Lock the memory to get a ptr to it
    lpMemHdrArg = MyGlobalLockVar (hGlbArg);

#define lpHeader    lpMemHdrArg
    // Get the Array Type and NELM
    aplTypeArg = lpHeader->ArrType;
    aplNELMArg = lpHeader->NELM;
    aplRankArg = lpHeader->Rank;
#undef  lpHeader

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMArg, aplRankArg);

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        // WS FULL, so no promotion
        goto NORMAL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader    lpMemHdrRes
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE     // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMArg;
    lpHeader->Rank       = aplRankArg;
#undef  lpHeader

    // Skip over the dimensions to the data
    lpMemDimArg = VarArrayBaseToDim (lpMemHdrArg);
    lpMemDimRes = VarArrayBaseToDim (lpMemHdrRes);

    // Copy the arg dimensions to the result
    CopyMemory (lpMemDimRes, lpMemDimArg, (APLU3264) BytesIn (ARRAY_INT, aplRankArg));

    // Skip over the header and dimensions to the data
    lpMemArg = VarArrayDataFmBase (lpMemHdrArg);
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        APLUINT   uRes;                 // Loop counter
        APLINT    apaOff,               // APA offset
                  apaMul;               // APA multiplier
        UINT      uBitMask;             // Bit mask for looping through Booleans
        APLHETERO lpSym0,               // LPSYMENTRY for 0
                  lpSym1;               // ...            1

        case ARRAY_BOOL:                // A -> B
            Assert (IsSimpleAPA (aplTypeArg));

            // Split cases based upon the arg storage type
            switch (aplTypeArg)
            {
                case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemArg)
                    // Get the APA parameters
                    apaOff = lpAPA->Off;
                    apaMul = lpAPA->Mul;
#undef  lpAPA
                    Assert (apaMul EQ 0 && IsBooleanValue (apaOff));

                    // Do something only for 1s
                    if (apaOff EQ 1)
                        FillBitMemory (lpMemRes, aplNELMArg);
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_INT:                 // B/A -> I
            Assert (IsSimpleBool (aplTypeArg)
                 || IsSimpleAPA (aplTypeArg));
            // Split cases based upon the arg storage type
            switch (aplTypeArg)
            {
                case ARRAY_BOOL:
                    uBitMask = BIT0;

                    // Loop through the arg converting values to the result
                    for (uRes = 0; uRes < aplNELMArg; uRes++)
                    {
                        if (uBitMask & *(LPAPLBOOL) lpMemArg)
                            *((LPAPLINT) lpMemRes) = TRUE;
                        ((LPAPLINT) lpMemRes)++;

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

                case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemArg)
                    // Get the APA parameters
                    apaOff = lpAPA->Off;
                    apaMul = lpAPA->Mul;
#undef  lpAPA
                    // Loop through the arg converting values to the result
                    for (uRes = 0; uRes < aplNELMArg; uRes++)
                        *((LPAPLINT) lpMemRes)++ =
                          apaOff + apaMul * uRes;
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_FLOAT:               // B/I/A/F -> F
            Assert (IsSimpleNum (aplTypeArg));

            // Split cases based upon the arg storage type
            switch (aplTypeArg)
            {
                case ARRAY_BOOL:
                    uBitMask = BIT0;

                    // Loop through the arg converting values to the result
                    for (uRes = 0; uRes < aplNELMArg; uRes++)
                    {
                        if (uBitMask & *(LPAPLBOOL) lpMemArg)
                            *((LPAPLFLOAT) lpMemRes) = TRUE;
                        ((LPAPLFLOAT) lpMemRes)++;

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

                case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemArg)
                    // Get the APA parameters
                    apaOff = lpAPA->Off;
                    apaMul = lpAPA->Mul;
#undef  lpAPA
                    // Loop through the arg converting values to the result
                    for (uRes = 0; uRes < aplNELMArg; uRes++)
                        *((LPAPLFLOAT) lpMemRes)++ =
                          (APLFLOAT) (APLINT) (apaOff + apaMul * uRes);
                    break;

                case ARRAY_INT:
                    // Loop through the arg converting values to the result
                    for (uRes = 0; uRes < aplNELMArg; uRes++)
                        *((LPAPLFLOAT) lpMemRes)++ =
                          (APLFLOAT) *((LPAPLINT) lpMemArg)++;
                    break;

                case ARRAY_FLOAT:
                    // Loop through the arg converting values to the result
                    for (uRes = 0; uRes < aplNELMArg; uRes++)
                        *((LPAPLFLOAT) lpMemRes)++ =
                          *((LPAPLFLOAT) lpMemArg)++;
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_HETERO:              // B/I/F/C/A -> H
            Assert (IsSimpleNH (aplTypeArg));
        case ARRAY_NESTED:              // B/I/F/C/A/H -> N
            Assert (IsSimple   (aplTypeArg));

            // Check for empty arg
            if (IsEmpty (aplNELMArg))
            {
                // Split cases based upon the arg storage type
                switch (aplTypeArg)
                {
                    case ARRAY_BOOL:
                    case ARRAY_INT:
                    case ARRAY_FLOAT:
                    case ARRAY_APA:
                        // Save a zero STE as the prototype
                        *((LPAPLHETERO) lpMemRes)++ = GetSteZero ();

                        break;

                    case ARRAY_CHAR:
                        // Save a zero STE as the prototype
                        *((LPAPLHETERO) lpMemRes)++ = GetSteBlank ();

                        break;

                    defstop
                        break;
                } // End SWITCH
            } else
            // Split cases based upon the arg storage type
            switch (aplTypeArg)
            {
                case ARRAY_BOOL:
                    uBitMask = BIT0;
                    lpSym0 = GetSteZero ();
                    lpSym1 = GetSteOne ();

                    // Loop through the arg converting values to the result
                    for (uRes = 0; uRes < aplNELMArg; uRes++)
                    {
                        *((LPAPLHETERO) lpMemRes)++ =
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
                    // Loop through the arg converting values to the result
                    for (uRes = 0; uRes < aplNELMArg; uRes++)
                    {
                        *((LPAPLHETERO) lpMemRes)++ =
                        lpSymTmp =
                          SymTabAppendInteger_EM (*((LPAPLINT) lpMemArg)++, TRUE);
                        if (lpSymTmp EQ NULL)
                            goto ERROR_EXIT;
                    } // End FOR

                    break;

                case ARRAY_FLOAT:
                    // Loop through the arg converting values to the result
                    for (uRes = 0; uRes < aplNELMArg; uRes++)
                    {
                        *((LPAPLHETERO) lpMemRes)++ =
                        lpSymTmp =
                          SymTabAppendFloat_EM (*((LPAPLFLOAT) lpMemArg)++);
                        if (lpSymTmp EQ NULL)
                            goto ERROR_EXIT;
                    } // End FOR

                    break;

                case ARRAY_CHAR:
                    // Loop through the arg converting values to the result
                    for (uRes = 0; uRes < aplNELMArg; uRes++)
                    {
                        *((LPAPLHETERO) lpMemRes)++ =
                        lpSymTmp =
                          SymTabAppendChar_EM (*((LPAPLCHAR) lpMemArg)++, TRUE);
                        if (lpSymTmp EQ NULL)
                            goto ERROR_EXIT;
                    } // End FOR

                    break;

                case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemArg)
                    // Get the APA parameters
                    apaOff = lpAPA->Off;
                    apaMul = lpAPA->Mul;
#undef  lpAPA
                    // Loop through the arg converting values to the result
                    for (uRes = 0; uRes < aplNELMArg; uRes++)
                    {
                        *((LPAPLHETERO) lpMemRes)++ =
                        lpSymTmp =
                          SymTabAppendInteger_EM (apaOff + apaMul * uRes, TRUE);
                        if (lpSymTmp EQ NULL)
                            goto ERROR_EXIT;
                    } // End FOR

                    break;

                case ARRAY_HETERO:
                    // Loop through the arg converting values to the result
                    for (uRes = 0; uRes < aplNELMArg; uRes++)
                        *((LPAPLHETERO) lpMemRes)++ =
                          *((LPAPLHETERO) lpMemArg)++;
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_RAT:
            // Loop through the arg converting values to the result
            for (uRes = 0; uRes < aplNELMArg; uRes++)
                (*aTypeActPromote[aplTypeArg][aplTypeRes]) (lpMemArg, uRes, (LPALLTYPES) ((LPAPLRAT) lpMemRes)++);
            break;

        case ARRAY_VFP:
            // Loop through the arg converting values to the result
            for (uRes = 0; uRes < aplNELMArg; uRes++)
                (*aTypeActPromote[aplTypeArg][aplTypeRes]) (lpMemArg, uRes, (LPALLTYPES) ((LPAPLVFP) lpMemRes)++);
            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need thess ptrs
    MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    MyGlobalUnlock (hGlbArg); lpMemHdrArg = NULL;

    // Free the old HGLOBAL
    FreeResultGlobalVar (hGlbArg); hGlbArg = NULL;

    // Save the new HGLOBAL
    *lphGlbArg = hGlbRes;
NORMAL_EXIT:
    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
    if (hGlbArg NE NULL  && lpMemHdrArg NE NULL )
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbArg); lpMemHdrArg = NULL;
    } // End IF

    if (hGlbRes NE NULL  && lpMemHdrRes NE NULL )
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    return bRet;
} // End TypePromoteGlb_EM
#undef  APPEND_NAME


//***************************************************************************
//  $QueryPromote
//
//  Determine if an arg should be promoted
//***************************************************************************

UBOOL QueryPromote
    (APLSTYPE  aplTypeNam,          // Name arg storage type
     APLSTYPE  aplTypeSet,          // Set  ...
     APLSTYPE *lpaplTypeRes)        // Result   ...

{
    // Calculate the result storage type
    *lpaplTypeRes = aTypePromote[aplTypeNam][aplTypeSet];

    // Compare the storage type of the result with the existing storage type
    return (*lpaplTypeRes NE aplTypeNam);
} // End QueryPromote


//***************************************************************************
//      TPF Routines
//***************************************************************************

//***************************************************************************
//  $TPF_IDENT
//***************************************************************************

void TPF_IDENT
    (LPVOID  lpArg,
     APLNELM aplIndex)
{
    // No global storage to free
} // End TPF_IDENT


//***************************************************************************
//  $TPF_HETERO
//  $TPF_NESTED
//***************************************************************************

void TPF_HETERO
    (LPALLTYPES lpatArg,
     APLNELM    aplIndex)
{
    Assert (aplIndex EQ 0);

    // If it's valid, ...
    if (lpatArg->aplHetero NE NULL)
    // Split cases based upon the ptr type bits
    switch (GetPtrTypeDir (lpatArg->aplHetero))
    {
        case PTRTYPE_STCONST:
            break;

        case PTRTYPE_HGLOBAL:
            // Free the global var
            FreeResultGlobalVar (lpatArg->aplHetero);

            // Zap the save area
            lpatArg->aplHetero = NULL;

            break;

        defstop
            break;
    } // End IF/SWITCH
} // End TPF_HETERO


//***************************************************************************
//  $TPF_RAT
//***************************************************************************

void TPF_RAT
    (LPAPLRAT lpArg,
     APLNELM  aplIndex)

{
    Myq_clear (&lpArg[aplIndex]);
} // End TPF_RAT


//***************************************************************************
//  $TPF_VFP
//***************************************************************************

void TPF_VFP
    (LPAPLVFP lpArg,
     APLNELM   aplIndex)

{
    Myf_clear (&lpArg[aplIndex]);
} // End TPF_VFP


//***************************************************************************
//      TPT Routines
//***************************************************************************

//***************************************************************************
//  $TPT_ERROR
//***************************************************************************

void TPT_ERROR
    (LPTOKEN lptkArg)

{
    DbgStop ();
} // TPT_ERROR


//***************************************************************************
//  $TPT_IDENT
//***************************************************************************

void TPT_IDENT
    (LPTOKEN lptkArg)

{
} // TPT_IDENT


//***************************************************************************
//  $TPT_BOOL2INT
//***************************************************************************

void TPT_BOOL2INT
    (LPTOKEN lptkArg)

{
    lptkArg->tkData.tkInteger = BIT0 & lptkArg->tkData.tkInteger;
    lptkArg->tkFlags.ImmType  = IMMTYPE_INT;
} // TPT_BOOL2INT


//***************************************************************************
//  $TPT_BOOL2FLT
//***************************************************************************

void TPT_BOOL2FLT
    (LPTOKEN lptkArg)

{
    lptkArg->tkData.tkFloat  = (APLFLOAT) lptkArg->tkData.tkInteger;
    lptkArg->tkFlags.ImmType = IMMTYPE_FLOAT;
} // TPT_BOOL2FLT


//***************************************************************************
//  $TPT_BOOL2RAT
//***************************************************************************

void TPT_BOOL2RAT
    (LPTOKEN lptkArg)

{
    APLRAT aplRat;

    // Convert the BOOL to a RAT
    mpq_init_set_sx (&aplRat, BIT0 & lptkArg->tkData.tkInteger, 1);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_RAT,           // Array Type to use (see ARRAY_TYPES)
                      &aplRat,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_RAT;
} // TPT_BOOL2RAT


//***************************************************************************
//  $TPT_BOOL2VFP
//***************************************************************************

void TPT_BOOL2VFP
    (LPTOKEN lptkArg)

{
    APLVFP aplVfp;

    // Convert the BOOL to a VFP
    mpfr_init_set_sx (&aplVfp, BIT0 & lptkArg->tkData.tkInteger, MPFR_RNDN);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_VFP,           // Array Type to use (see ARRAY_TYPES)
                      &aplVfp,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_VFP;
} // TPT_BOOL2VFP


//***************************************************************************
//  $TPT_INT2FLT
//***************************************************************************

void TPT_INT2FLT
    (LPTOKEN lptkArg)

{
    lptkArg->tkData.tkFloat  = (APLFLOAT) lptkArg->tkData.tkInteger;
    lptkArg->tkFlags.ImmType = IMMTYPE_FLOAT;
} // TPT_INT2FLT


//***************************************************************************
//  $TPT_INT2RAT
//***************************************************************************

void TPT_INT2RAT
    (LPTOKEN lptkArg)

{
    APLRAT aplRat;

    // Convert the INT to a RAT
    mpq_init_set_sx (&aplRat, lptkArg->tkData.tkInteger, 1);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_RAT,           // Array Type to use (see ARRAY_TYPES)
                      &aplRat,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_RAT;
} // TPT_INT2RAT


//***************************************************************************
//  $TPT_INT2VFP
//***************************************************************************

void TPT_INT2VFP
    (LPTOKEN lptkArg)

{
    APLVFP aplVfp;

    // Convert the INT to a VFP
    mpfr_init_set_sx (&aplVfp, lptkArg->tkData.tkInteger, MPFR_RNDN);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_VFP,           // Array Type to use (see ARRAY_TYPES)
                      &aplVfp,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_VFP;
} // TPT_INT2VFP


//***************************************************************************
//  $TPT_FLT2VFP
//***************************************************************************

void TPT_FLT2VFP
    (LPTOKEN lptkArg)

{
    APLVFP aplVfp;

    // Convert the FLT to a VFP
    mpfr_init_set_d  (&aplVfp, lptkArg->tkData.tkFloat, MPFR_RNDN);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_VFP,           // Array Type to use (see ARRAY_TYPES)
                      &aplVfp,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_VFP;
} // TPT_FLT2VFP


//***************************************************************************
//  $TPT_APA2FLT
//***************************************************************************

void TPT_APA2FLT
    (LPTOKEN lptkArg)

{
    lptkArg->tkData.tkFloat  = (APLFLOAT) lptkArg->tkData.tkInteger;
    lptkArg->tkFlags.ImmType = IMMTYPE_FLOAT;
} // TPT_APA2FLT


//***************************************************************************
//  $TPT_APA2RAT
//***************************************************************************

void TPT_APA2RAT
    (LPTOKEN lptkArg)

{
    APLRAT aplRat;

    // Convert the APA to a RAT
    mpq_init_set_sx (&aplRat, lptkArg->tkData.tkInteger, 1);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_RAT,           // Array Type to use (see ARRAY_TYPES)
                      &aplRat,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_RAT;
} // TPT_APA2RAT


//***************************************************************************
//  $TPT_APA2VFP
//***************************************************************************

void TPT_APA2VFP
    (LPTOKEN lptkArg)

{
    APLVFP aplVfp;

    // Convert the APA to a VFP
    mpfr_init_set_sx (&aplVfp, lptkArg->tkData.tkInteger, MPFR_RNDN);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_VFP,           // Array Type to use (see ARRAY_TYPES)
                      &aplVfp,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_VFP;
} // TPT_APA2VFP


//***************************************************************************
//  $TPT_RAT2VFP
//***************************************************************************

void TPT_RAT2VFP
    (LPTOKEN lptkArg)

{
    HGLOBAL           hGlbRat;
    LPVARARRAY_HEADER lpMemHdrRat = NULL;
    LPAPLRAT          lpMemRat;
    APLVFP            aplVfp;

    Assert (lptkArg->tkFlags.TknType EQ TKT_VARARRAY);
    Assert (GetPtrTypeDir (lptkArg->tkData.tkGlbData) EQ PTRTYPE_HGLOBAL);

    // Get the global memory handle
    hGlbRat = lptkArg->tkData.tkGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrRat = MyGlobalLockVar (hGlbRat);

    Assert (IsScalar (lpMemHdrRat->Rank));

    // Skip over the header and dimensions to the data
    lpMemRat = VarArrayDataFmBase (lpMemHdrRat);

    // Convert the RAT to a VFP
    mpfr_init_set_q (&aplVfp, lpMemRat, MPFR_RNDN);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRat); lpMemHdrRat = NULL;

    // We no longer need this storage
    FreeResultGlobalVar (hGlbRat); hGlbRat = NULL;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_VFP,           // Array Type to use (see ARRAY_TYPES)
                      &aplVfp,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    lptkArg->tkFlags.ImmType = IMMTYPE_VFP;
} // TPT_RAT2VFP


//***************************************************************************
//  $TPA_ERROR
//***************************************************************************

void TPA_ERROR
    (LPVOID      lpaplAny,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    DbgStop ();
} // TPA_ERROR


//***************************************************************************
//  $TPA_BOOL2BOOL
//***************************************************************************

void TPA_BOOL2BOOL
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplInteger = GetNextInteger (lpaplBoolean, ARRAY_BOOL, uInt);
} // TPA_BOOL2BOOL


//***************************************************************************
//  $TPA_BOOL2INT
//***************************************************************************

void TPA_BOOL2INT
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplInteger = GetNextInteger (lpaplBoolean, ARRAY_BOOL, uInt);
} // TPA_BOOL2INT


//***************************************************************************
//  $TPA_BOOL2FLT
//***************************************************************************

void TPA_BOOL2FLT
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplFloat = GetNextFloat (lpaplBoolean, ARRAY_BOOL, uInt);
} // TPA_BOOL2FLT


//***************************************************************************
//  $TPA_BOOL2HETE
//***************************************************************************

void TPA_BOOL2HETE
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextInteger (lpaplBoolean, ARRAY_BOOL, uInt);

    lpAllTypes->aplHetero =
      MakeSymEntry_EM (IMMTYPE_BOOL,
       (LPAPLLONGEST) &aplInteger,
                       NULL);
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_BOOL2HETE


//***************************************************************************
//  $TPA_BOOL2NEST
//***************************************************************************

void TPA_BOOL2NEST
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextInteger (lpaplBoolean, ARRAY_BOOL, uInt);

    lpAllTypes->aplNested =
      MakeSymEntry_EM (IMMTYPE_BOOL,
       (LPAPLLONGEST) &aplInteger,
                       NULL);
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_BOOL2NEST


//***************************************************************************
//  $TPA_BOOL2RAT
//***************************************************************************

void TPA_BOOL2RAT
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextInteger (lpaplBoolean, ARRAY_BOOL, uInt);

    // Initialize the result
    Myq_init (&lpAllTypes->aplRat);

    mpq_set_sx (&lpAllTypes->aplRat, aplInteger, 1);
} // TPA_BOOL2RAT


//***************************************************************************
//  $TPA_BOOL2VFP
//***************************************************************************

void TPA_BOOL2VFP
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextInteger (lpaplBoolean, ARRAY_BOOL, uInt);

    // Initialize the result
    Myf_init (&lpAllTypes->aplVfp);

    mpfr_set_sx (&lpAllTypes->aplVfp, aplInteger, MPFR_RNDN);
} // TPA_BOOL2VFP


//***************************************************************************
//  $TPA_INT2INT
//***************************************************************************

void TPA_INT2INT
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplInteger = lpaplInteger[uInt];
} // TPA_INT2INT


//***************************************************************************
//  $TPA_INT2FLT
//***************************************************************************

void TPA_INT2FLT
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    // No need to copy the input as it is the same size as the output

    lpAllTypes->aplFloat = (APLFLOAT) lpaplInteger[uInt];
} // TPA_INT2FLT


//***************************************************************************
//  $TPA_INT2HETE
//***************************************************************************

void TPA_INT2HETE
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextInteger (lpaplInteger, ARRAY_INT, uInt);

    lpAllTypes->aplHetero =
      MakeSymEntry_EM (IMMTYPE_INT,
       (LPAPLLONGEST) &aplInteger,
                       NULL);
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_INT2HETE


//***************************************************************************
//  $TPA_INT2NEST
//***************************************************************************

void TPA_INT2NEST
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = lpaplInteger[uInt];

    lpAllTypes->aplNested =
      MakeSymEntry_EM (IMMTYPE_INT,
       (LPAPLLONGEST) &aplInteger,
                       NULL);
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_INT2NEST


//***************************************************************************
//  $TPA_INT2RAT
//***************************************************************************

void TPA_INT2RAT
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = lpaplInteger[uInt];

    // Initialize the result
    Myq_init (&lpAllTypes->aplRat);

    mpq_set_sx (&lpAllTypes->aplRat, aplInteger, 1);
} // TPA_INT2RAT


//***************************************************************************
//  $TPA_INT2VFP
//***************************************************************************

void TPA_INT2VFP
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = lpaplInteger[uInt];

    // Initialize the result to NaN
    Myf_init (&lpAllTypes->aplVfp);

    mpfr_set_sx (&lpAllTypes->aplVfp, aplInteger, MPFR_RNDN);
} // TPA_INT2VFP


//***************************************************************************
//  $TPA_FLT2FLT
//***************************************************************************

void TPA_FLT2FLT
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    // No need to copy the input as it is the same size as the output

    lpAllTypes->aplFloat = lpaplFloat[uInt];
} // TPA_FLT2FLT


//***************************************************************************
//  $TPA_FLT2HETE
//***************************************************************************

void TPA_FLT2HETE
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLFLOAT aplFloat;

    // Copy the input value in case it overlaps with the output
    aplFloat = lpaplFloat[uInt];

    lpAllTypes->aplHetero =
      MakeSymEntry_EM (IMMTYPE_FLOAT,
       (LPAPLLONGEST) &aplFloat,
                       NULL);
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_FLT2HETE


//***************************************************************************
//  $TPA_FLT2NEST
//***************************************************************************

void TPA_FLT2NEST
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLFLOAT aplFloat;

    // Copy the input value in case it overlaps with the output
    aplFloat = lpaplFloat[uInt];

    lpAllTypes->aplNested =
      MakeSymEntry_EM (IMMTYPE_FLOAT,
       (LPAPLLONGEST) &aplFloat,
                       NULL);
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_FLT2NEST


//***************************************************************************
//  $TPA_FLT2VFP
//***************************************************************************

void TPA_FLT2VFP
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLFLOAT aplFloat;

    // Copy the input value in case it overlaps with the output
    aplFloat = lpaplFloat[uInt];

    // Initialize the result
    Myf_init (&lpAllTypes->aplVfp);

    mpfr_set_d  (&lpAllTypes->aplVfp, aplFloat, MPFR_RNDN);
} // TPA_FLT2VFP


//***************************************************************************
//  $TPA_CHAR2CHAR
//***************************************************************************

void TPA_CHAR2CHAR
    (LPAPLCHAR   lpaplChar,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    // No need to copy the input as it is the same size as the output

    lpAllTypes->aplChar = lpaplChar[uInt];
} // TPA_CHAR2CHAR


//***************************************************************************
//  $TPA_CHAR2HETE
//***************************************************************************

void TPA_CHAR2HETE
    (LPAPLCHAR   lpaplChar,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLCHAR aplChar;

    // Copy the input value in case it overlaps with the output
    aplChar = lpaplChar[uInt];

    lpAllTypes->aplHetero =
      MakeSymEntry_EM (IMMTYPE_CHAR,
       (LPAPLLONGEST) &aplChar,
                       NULL);
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_CHAR2HETE


//***************************************************************************
//  $TPA_CHAR2NEST
//***************************************************************************

void TPA_CHAR2NEST
    (LPAPLCHAR   lpaplChar,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLCHAR aplChar;

    // Copy the input value in case it overlaps with the output
    aplChar = lpaplChar[uInt];

    lpAllTypes->aplNested =
      MakeSymEntry_EM (IMMTYPE_CHAR,
       (LPAPLLONGEST) &aplChar,
                       NULL);
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_CHAR2NEST


//***************************************************************************
//  $TPA_HETE2NEST
//***************************************************************************

void TPA_HETE2NEST
    (LPAPLHETERO lpaplHetero,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    // Copy the HETERO to NESTED
    lpAllTypes->aplNested = lpaplHetero[uInt];
} // TPA_HETE2NEST


//***************************************************************************
//  $TPA_APA2INT
//***************************************************************************

void TPA_APA2INT
    (LPAPLAPA    lpaplAPA,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    // No need to copy the input as it is larger than the output

    lpAllTypes->aplInteger = GetNextInteger (lpaplAPA, ARRAY_APA, uInt);
} // TPA_APA2INT


//***************************************************************************
//  $TPA_APA2FLT
//***************************************************************************

void TPA_APA2FLT
    (LPAPLAPA    lpaplAPA,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    // No need to copy the input as it is larger than the output

    lpAllTypes->aplFloat = GetNextFloat (lpaplAPA, ARRAY_APA, uInt);
} // TPA_APA2FLT


//***************************************************************************
//  $TPA_APA2HETE
//***************************************************************************

void TPA_APA2HETE
    (LPAPLAPA    lpaplAPA,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextInteger (lpaplAPA, ARRAY_APA, uInt);

    lpAllTypes->aplHetero =
      MakeSymEntry_EM (IMMTYPE_INT,
       (LPAPLLONGEST) &aplInteger,
                       NULL);
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_APA2HETE


//***************************************************************************
//  $TPA_APA2NEST
//***************************************************************************

void TPA_APA2NEST
    (LPAPLAPA    lpaplAPA,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextInteger (lpaplAPA, ARRAY_APA, uInt);

    lpAllTypes->aplNested =
      MakeSymEntry_EM (IMMTYPE_INT,
       (LPAPLLONGEST) &aplInteger,
                       NULL);
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_APA2NEST


//***************************************************************************
//  $TPA_APA2RAT
//***************************************************************************

void TPA_APA2RAT
    (LPAPLAPA    lpaplAPA,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextInteger (lpaplAPA, ARRAY_APA, uInt);

    // Initialize the result
    Myq_init (&lpAllTypes->aplRat);

    mpq_set_sx (&lpAllTypes->aplRat, aplInteger, 1);
} // TPA_APA2RAT


//***************************************************************************
//  $TPA_APA2VFP
//***************************************************************************

void TPA_APA2VFP
    (LPAPLAPA    lpaplAPA,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextInteger (lpaplAPA, ARRAY_APA, uInt);

    // Initialize the result
    Myf_init (&lpAllTypes->aplVfp);

    mpfr_set_sx (&lpAllTypes->aplVfp, aplInteger, MPFR_RNDN);
} // TPA_APA2VFP


//***************************************************************************
//  $TPA_RAT2VFP
//***************************************************************************

void TPA_RAT2VFP
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLRAT aplRat;

    // Copy the input value in case it overlaps with the output
    aplRat = lpaplRat[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplRat)
        DbgStop ();
#endif

    // Initialize the result
    Myf_init (&lpAllTypes->aplVfp);

    mpfr_set_q  (&lpAllTypes->aplVfp, &aplRat, MPFR_RNDN);
} // TPA_RAT2VFP


//***************************************************************************
//  $TPA_HETE2HETE
//***************************************************************************

void TPA_HETE2HETE
    (LPAPLHETERO lpaplHetero,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    // Copy the HETERO to HETERO
    lpAllTypes->aplHetero = lpaplHetero[uInt];
} // TPA_HETE2HETE


//***************************************************************************
//  $TPA_NEST2NEST
//***************************************************************************

void TPA_NEST2NEST
    (LPAPLNESTED lpaplNested,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    // Copy the NESTED to NESTED
    lpAllTypes->aplNested = CopySymGlbDir_PTB (((LPAPLNESTED) ClrPtrTypeDir (lpaplNested))[uInt]);
} // TPA_NEST2NEST


//***************************************************************************
//  $TPA_RAT2HETE
//***************************************************************************

void TPA_RAT2HETE
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplHetero =
      MakeGlbEntry_EM (ARRAY_RAT,       // Entry type
                      &lpaplRat[uInt],  // Ptr to the value
                       TRUE,            // TRUE iff we should initialize the target first
                       NULL);           // Ptr to function token
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_RAT2HETE


//***************************************************************************
//  $TPA_RAT2NEST
//***************************************************************************

void TPA_RAT2NEST
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplNested =
      MakeGlbEntry_EM (ARRAY_RAT,       // Entry type
                      &lpaplRat[uInt],  // Ptr to the value
                       TRUE,            // TRUE iff we should initialize the target first
                       NULL);           // Ptr to function token
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_RAT2NEST


//***************************************************************************
//  $TPA_RAT2RAT
//***************************************************************************

void TPA_RAT2RAT
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLRAT aplRat;

    // Copy the input value in case it overlaps with the output
    aplRat = lpaplRat[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplRat)
        DbgStop ();
#endif

    // Initialize the result
    Myq_init (&lpAllTypes->aplRat);

    mpq_set (&lpAllTypes->aplRat, &aplRat);
} // TPA_RAT2RAT


//***************************************************************************
//  $TPA_VFP2HETE
//***************************************************************************

void TPA_VFP2HETE
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplHetero =
      MakeGlbEntry_EM (ARRAY_VFP,       // Entry type
                      &lpaplVfp[uInt],  // Ptr to the value
                       TRUE,            // TRUE iff we should initialize the target first
                       NULL);           // Ptr to function token
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_VFP2HETE


//***************************************************************************
//  $TPA_VFP2NEST
//***************************************************************************

void TPA_VFP2NEST
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplNested =
      MakeGlbEntry_EM (ARRAY_VFP,       // Entry type
                      &lpaplVfp[uInt],  // Ptr to the value
                       TRUE,            // TRUE iff we should initialize the target first
                       NULL);           // Ptr to function token
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_VFP2NEST


//***************************************************************************
//  $TPA_VFP2VFP
//***************************************************************************

void TPA_VFP2VFP
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLVFP aplVfp;

    // Copy the input value in case it overlaps with the output
    aplVfp = lpaplVfp[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplVfp)
        DbgStop ();
#endif

    // Initialize the result
    Myf_init (&lpAllTypes->aplVfp);

    mpfr_copy (&lpAllTypes->aplVfp, &aplVfp);
} // TPA_VFP2VFP


//***************************************************************************
//  $TCA_FLT2INT
//***************************************************************************

void TCA_FLT2INT
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplInteger = GetNextInteger (lpaplFloat, ARRAY_FLOAT, uInt);
} // TCA_FLT2INT


//***************************************************************************
//  $TCA_FLT2RAT
//***************************************************************************

void TCA_FLT2RAT
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    // Initialize the result
    Myq_init (&lpAllTypes->aplRat);

    mpq_set_d (&lpAllTypes->aplRat, lpaplFloat[uInt]);
} // TCA_FLT2RAT


//***************************************************************************
//  $TCA_RAT2INT
//***************************************************************************

void TCA_RAT2INT
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplInteger = mpq_get_sctsx (&lpaplRat[uInt], NULL);
} // TCA_RAT2INT


//***************************************************************************
//  $TCA_RAT2FLT
//***************************************************************************

void TCA_RAT2FLT
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplFloat = mpq_get_d (&lpaplRat[uInt]);
} // TCA_RAT2FLT


//***************************************************************************
//  $TCA_VFP2INT
//***************************************************************************

void TCA_VFP2INT
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplInteger = mpfr_get_sctsx (&lpaplVfp[uInt], NULL);
} // TCA_VFP2INT


//***************************************************************************
//  $TCA_VFP2FLT
//***************************************************************************

void TCA_VFP2FLT
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplFloat = mpfr_get_d (&lpaplVfp[uInt], MPFR_RNDN);
} // TCA_VFP2FLT


//***************************************************************************
//  $TCA_VFP2RAT
//***************************************************************************

void TCA_VFP2RAT
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    // Initialize the result
    Myq_init   (&lpAllTypes->aplRat);

    mpq_set_fr (&lpAllTypes->aplRat, &lpaplVfp[uInt]);
} // TCA_VFP2RAT


//***************************************************************************
//  End of File: typemote.c
//***************************************************************************
