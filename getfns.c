//***************************************************************************
//  NARS2000 -- Getxxx Functions
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2014 Sudley Place Software

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
//  $ValidateFirstItemToken
//
//  Get and validate the first item from the token as an integer
//***************************************************************************

APLLONGEST ValidateFirstItemToken
    (APLSTYPE aplTypeRht,               // Right arg storage type
     LPTOKEN  lpToken,                  // Ptr to the token
     LPUBOOL  lpbRet)                   // Ptr to TRUE iff the result is valid

{
    APLLONGEST aplLongestRht;           // Right arg longest if immediate
    HGLOBAL    hGlbSubRht;              // Right arg item global memory handle

    GetFirstItemToken (lpToken,         // Ptr to the token
                      &aplLongestRht,   // Ptr to the longest (may be NULL)
                       NULL,            // ...        immediate type (see IMM_TYPES) (may be NULL)
                      &hGlbSubRht);     // ...        LPSYMENTRY or HGLOBAL (may be NULL)
    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            *lpbRet = TRUE;

            break;

        case ARRAY_FLOAT:
            // Attempt to convert the float to an integer using System []CT
            aplLongestRht = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestRht, lpbRet);

            break;

        case ARRAY_RAT:
            Assert (GetPtrTypeDir (hGlbSubRht) NE PTRTYPE_HGLOBAL);

            // Attempt to convert the RAT to an integer using System []CT
            aplLongestRht = mpq_get_sctsx ((LPAPLRAT) hGlbSubRht, lpbRet);

            break;

        case ARRAY_VFP:
            Assert (GetPtrTypeDir (hGlbSubRht) NE PTRTYPE_HGLOBAL);

            // Attempt to convert the VFP to an integer using System []CT
            aplLongestRht = mpfr_get_sctsx ((LPAPLVFP) hGlbSubRht, lpbRet);

            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            *lpbRet = FALSE;

            break;

        defstop
            *lpbRet = FALSE;

            break;
    } // End SWITCH

    return aplLongestRht;
} // End ValidateFirstItemToken


//***************************************************************************
//  $GetNextIntegerToken
//
//  Return the next value from a token as an integer.
//***************************************************************************

APLINT GetNextIntegerToken
    (LPTOKEN  lptkArg,                  // Ptr to arg token
     APLINT   uIndex,                   // Index
     APLSTYPE aplTypeArg,               // Arg storage type
     LPUBOOL  lpbRet)                   // Ptr to TRUE iff the result is valid

{
    APLLONGEST aplLongestArg;           // Immediate value
    HGLOBAL    hGlbArg = NULL;          // Right arg global memory handle
    LPVOID     lpMemArg;                // Ptr to right arg global memory
    APLINT     aplIntegerRes;           // The result
    APLSTYPE   aplTypeItm;              // Item storage type
    APLFLOAT   aplFloatItm;             // Immediate item as a float
    APLCHAR    aplCharItm;              // Immediate item as a char
    HGLOBAL    hGlbItm;                 // ...                 HGLOBAL

    // Assume it'll be successful
    *lpbRet = TRUE;

    // If the arg is simple non-hetero, ...
    if (IsSimpleNH (aplTypeArg))
        // Get the next value from the right operand (index origin)
        GetNextValueToken (lptkArg,                     // Ptr to the token
                           uIndex,                      // Index to use
                           NULL,                        // Ptr to the integer (or Boolean) (may be NULL)
                           NULL,                        // ...        float (may be NULL)
                           NULL,                        // ...        char (may be NULL)
                          &aplLongestArg,               // ...        longest (may be NULL)
                           NULL,                        // ...        LPSYMENTRY or HGLOBAL (may be NULL)
                           NULL,                        // ...        immediate type (see IMM_TYPES) (may be NULL)
                           NULL);                       // ...        array type:  ARRAY_TYPES (may be NULL)
    else
    // Otherwise it's global numeric
    {
        // Get the global ptrs
        GetGlbPtrs_LOCK (lptkArg, &hGlbArg, &lpMemArg);

        // If it's not an immediate, ...
        if (hGlbArg)
            lpMemArg = VarArrayDataFmBase (lpMemArg);
    } // End IF/ELSE

    // Split cases based upon the storage type
    switch (aplTypeArg)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            aplIntegerRes = aplLongestArg;

            break;

        case ARRAY_FLOAT:
            // Attempt to convert the float to an integer using System []CT
            aplIntegerRes = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestArg, lpbRet);

            break;

        case ARRAY_RAT:
            aplIntegerRes = GetNextRatIntMem (lpMemArg, uIndex, lpbRet);

            break;

        case ARRAY_VFP:
            aplIntegerRes = GetNextVfpIntMem (lpMemArg, uIndex, lpbRet);

            break;

        case ARRAY_HETERO:
            // Get the next values and type
            aplTypeItm = GetNextHetero (lpMemArg, uIndex, &aplIntegerRes, &aplFloatItm, &aplCharItm, &hGlbItm);

            // Split cases based upon the storage type
            switch (aplTypeItm)
            {
                case ARRAY_BOOL:
                case ARRAY_INT:
                    goto NORMAL_EXIT;

                case ARRAY_FLOAT:
                    // Attempt to convert the float to an integer using System []CT
                    aplIntegerRes = FloatToAplint_SCT (aplFloatItm, lpbRet);

                    goto NORMAL_EXIT;

                case ARRAY_RAT:
                    Assert (GetPtrTypeDir (hGlbItm) NE PTRTYPE_HGLOBAL);

                    // Attempt to convert the RAT to an integer using System []CT
                    aplIntegerRes = mpq_get_sctsx ((LPAPLRAT) hGlbItm, lpbRet);

                    goto NORMAL_EXIT;

                case ARRAY_VFP:
                    Assert (GetPtrTypeDir (hGlbItm) NE PTRTYPE_HGLOBAL);

                    // Attempt to convert the VFP to an integer using System []CT
                    aplIntegerRes = mpfr_get_sctsx ((LPAPLVFP) hGlbItm, lpbRet);

                    goto NORMAL_EXIT;

                case ARRAY_CHAR:
                case ARRAY_NESTED:
                    break;

                case ARRAY_APA:             // Can't happen
                case ARRAY_HETERO:          // ...
                defstop
                    break;
            } // End SWITCH

            // Fall through to common error code

        case ARRAY_CHAR:
        case ARRAY_NESTED:
            // Mark as in error
            *lpbRet = FALSE;

            break;
    } // End SWITCH
NORMAL_EXIT:
    // If it's not an immediate, ...
    if (hGlbArg)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbArg); lpMemArg = NULL;
    } // End IF

    return aplIntegerRes;
} // End GetNextIntegerToken


//***************************************************************************
//  $GetNextFloatToken
//
//  Return the next value from a token as a float.
//***************************************************************************

APLFLOAT GetNextFloatToken
    (LPTOKEN  lptkArg,                  // Ptr to arg token
     APLINT   uIndex,                   // Index
     APLSTYPE aplTypeArg,               // Arg storage type
     LPUBOOL  lpbRet)                   // Ptr to TRUE iff the result is valid

{
    APLLONGEST aplLongestArg;           // Immediate value
    HGLOBAL    hGlbArg = NULL;          // Right arg global memory handle
    LPVOID     lpMemArg;                // Ptr to right arg global memory
    APLINT     aplIntegerItm;           // Immediate value as integer
    APLSTYPE   aplTypeItm;              // Item storage type
    APLFLOAT   aplFloatRes;             // The result
    APLCHAR    aplCharItm;              // Immediate item as a char
    HGLOBAL    hGlbItm;                 // ...                 HGLOBAL

    // Assume it'll be successful
    *lpbRet = TRUE;

    // If the arg is simple non-hetero, ...
    if (IsSimpleNH (aplTypeArg))
        // Get the next value from the right operand (index origin)
        GetNextValueToken (lptkArg,                     // Ptr to the token
                           uIndex,                      // Index to use
                           NULL,                        // Ptr to the integer (or Boolean) (may be NULL)
                           NULL,                        // ...        float (may be NULL)
                           NULL,                        // ...        char (may be NULL)
                          &aplLongestArg,               // ...        longest (may be NULL)
                           NULL,                        // ...        LPSYMENTRY or HGLOBAL (may be NULL)
                           NULL,                        // ...        immediate type (see IMM_TYPES) (may be NULL)
                           NULL);                       // ...        array type:  ARRAY_TYPES (may be NULL)
    else
    // Otherwise it's global numeric
    {
        // Get the global ptrs
        GetGlbPtrs_LOCK (lptkArg, &hGlbArg, &lpMemArg);

        // If it's not an immediate, ...
        if (hGlbArg)
            lpMemArg = VarArrayDataFmBase (lpMemArg);
    } // End IF/ELSE

    // Split cases based upon the storage type
    switch (aplTypeArg)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            aplFloatRes = (APLFLOAT) aplLongestArg;

            break;

        case ARRAY_FLOAT:
            aplFloatRes = *(LPAPLFLOAT) &aplLongestArg;

            break;

        case ARRAY_RAT:
            aplFloatRes = GetNextRatFltMem (lpMemArg, uIndex, lpbRet);

            break;

        case ARRAY_VFP:
            aplFloatRes = GetNextVfpFltMem (lpMemArg, uIndex, lpbRet);

            break;

        case ARRAY_HETERO:
            // Get the next values and type
            aplTypeItm = GetNextHetero (lpMemArg, uIndex, &aplIntegerItm, &aplFloatRes, &aplCharItm, &hGlbItm);

            // Split cases based upon the storage type
            switch (aplTypeItm)
            {
                case ARRAY_BOOL:            // GetNextHetero already converts INT to FLOAT
                case ARRAY_INT:             // ...
                case ARRAY_FLOAT:
                    goto NORMAL_EXIT;

                case ARRAY_RAT:
                    Assert (GetPtrTypeDir (hGlbItm) NE PTRTYPE_HGLOBAL);

                    // Attempt to convert the RAT to a float
                    aplFloatRes = mpq_get_d ((LPAPLRAT) hGlbItm);

                    goto NORMAL_EXIT;

                case ARRAY_VFP:
                    Assert (GetPtrTypeDir (hGlbItm) NE PTRTYPE_HGLOBAL);

                    // Attempt to convert the VFP to a float
                    aplFloatRes = mpfr_get_d ((LPAPLVFP) hGlbItm, MPFR_RNDN);

                    goto NORMAL_EXIT;

                case ARRAY_CHAR:
                case ARRAY_NESTED:
                    break;

                case ARRAY_APA:             // Can't happen
                case ARRAY_HETERO:          // ...
                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_CHAR:
        case ARRAY_NESTED:
            // Mark as in error
            *lpbRet = FALSE;

            break;
    } // End SWITCH
NORMAL_EXIT:
    // If it's not an immediate, ...
    if (hGlbArg)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbArg); lpMemArg = NULL;
    } // End IF

    return aplFloatRes;
} // End GetNextFloatToken


//***************************************************************************
//  $GetFirstItemToken
//
//  Return the first item from the token
//***************************************************************************

void GetFirstItemToken
    (LPTOKEN      lpToken,          // Ptr to the token
     LPAPLLONGEST lpaplLongest,     // Ptr to the longest (may be NULL)
     LPIMM_TYPES  lpImmType,        // ...        immediate type (see IMM_TYPES) (may be NULL)
     LPVOID      *lpSymGlb)         // ...        LPSYMENTRY or HGLOBAL (may be NULL)

{
    HGLOBAL hGlbData;               // Global memory handle of TKT_VARNAMED or TKT_VARARRAY

    // Fill in default values
    if (lpSymGlb)
        *lpSymGlb = NULL;

    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                hGlbData = lpToken->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir_PTB (hGlbData));

                break;      // Continue below with global case
            } // End IF

            // Handle the immediate case
            GetFirstValueImm (lpToken->tkData.tkSym->stFlags.ImmType,
                              lpToken->tkData.tkSym->stData.stLongest,
                              NULL,
                              NULL,
                              NULL,
                              lpaplLongest,
                              lpImmType,
                              NULL);
            return;

        case TKT_VARIMMED:
            // Handle the immediate case
            GetFirstValueImm (lpToken->tkFlags.ImmType,
                              lpToken->tkData.tkLongest,
                              NULL,
                              NULL,
                              NULL,
                              lpaplLongest,
                              lpImmType,
                              NULL);
            return;

        case TKT_VARARRAY:
            hGlbData = lpToken->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (hGlbData));

            break;      // Continue below with global case

        defstop
            return;
    } // End SWITCH

    // Handle the HGLOBAL case
    GetFirstValueGlb (hGlbData,         // The global memory handle
                      NULL,             // Ptr to integer (or Boolean) (may be NULL)
                      NULL,             // ...    float (may be NULL)
                      NULL,             // ...    char (may be NULL)
                      lpaplLongest,     // ...    longest (may be NULL)
                      lpSymGlb,         // ...    LPSYMENTRY or HGLOBAL (may be NULL)
                      lpImmType,        // ...    immediate type (see IMM_TYPES) (may be NULL)
                      NULL,             // ...    array type -- ARRAY_TYPES (may be NULL)
                      FALSE);           // TRUE iff we should expand LPSYMENTRY into immediate value
} // End GetFirstItemToken


//***************************************************************************
//  $GetNextValueTokenIntoToken
//
//  Return the next value from a token as either
//    a VARIMMED or VARARRAY.
//  The token may be an empty array in which case the
//    value of the prototype is returned.
//***************************************************************************

void GetNextValueTokenIntoToken
    (LPTOKEN      lptkArg,      // Ptr to the arg token
     APLUINT      uIndex,       // Index to use
     LPTOKEN      lptkRes)      // Ptr to the result token

{
    HGLOBAL    hGlbData,
               hGlbSub;
    IMM_TYPES  immType;

    // Fill in common values
    lptkRes->tkFlags.NoDisplay = FALSE;
    lptkRes->tkCharIndex       = lptkArg->tkCharIndex;

    // Split cases based upon the token type
    switch (lptkArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lptkArg->tkData.tkSym->stFlags.Imm)
            {
                hGlbData = lptkArg->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir_PTB (hGlbData));

                break;      // Continue below with global case
            } // End IF

            Assert (uIndex EQ 0);

            // Handle the immediate case
            GetFirstValueImm (lptkArg->tkData.tkSym->stFlags.ImmType,
                              lptkArg->tkData.tkSym->stData.stLongest,
                              NULL,
                              NULL,
                              NULL,
                             &lptkRes->tkData.tkLongest,
                             &immType,
                              NULL);
            // Fill in the result token
            lptkRes->tkFlags.TknType   = TKT_VARIMMED;
            lptkRes->tkFlags.ImmType   = immType;
////////////lptkRes->tkFlags.NoDisplay =                    // Filled in above
////////////lptkRes->tkData.tkLongest  =                    // Filled in above
////////////lptkRes->tkCharIndex       =                    // Filled in above

            return;

        case TKT_VARIMMED:
            Assert (uIndex EQ 0);

            // Handle the immediate case
            GetFirstValueImm (lptkArg->tkFlags.ImmType,
                              lptkArg->tkData.tkLongest,
                              NULL,
                              NULL,
                              NULL,
                             &lptkRes->tkData.tkLongest,
                             &immType,
                              NULL);
            // Fill in the result token
            lptkRes->tkFlags.TknType   = TKT_VARIMMED;
            lptkRes->tkFlags.ImmType   = immType;
////////////lptkRes->tkFlags.NoDisplay =                    // Filled in above
////////////lptkRes->tkData.tkLongest  =                    // Filled in above
////////////lptkRes->tkCharIndex       =                    // Filled in above

            return;

        case TKT_VARARRAY:
            hGlbData = lptkArg->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (hGlbData));

            break;      // Continue below with global case

        defstop
            return;
    } // End SWITCH

    // Handle the HGLOBAL case
    GetNextValueGlb (hGlbData,                          // The global memory handle
                     uIndex,                            // Index into item
                    &hGlbSub,                           // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                    &lptkRes->tkData.tkLongest,         // Ptr to result immediate value (may be NULL)
                    &immType);                          // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
    // If the return value is immediate, ...
    if (hGlbSub EQ NULL)
    {
        // Fill in the result token
        lptkRes->tkFlags.TknType   = TKT_VARIMMED;
        lptkRes->tkFlags.ImmType   = immType;
////////lptkRes->tkFlags.NoDisplay =                    // Filled in above
////////lptkRes->tkData.tkLongest  =                    // Filled in above
////////lptkRes->tkCharIndex       =                    // Filled in above
    } else
    {
        // Fill in the result token
        lptkRes->tkFlags.TknType   = TKT_VARARRAY;
        lptkRes->tkFlags.ImmType   = immType;
////////lptkRes->tkFlags.NoDisplay =                    // Filled in above
        lptkRes->tkData.tkGlbData  = hGlbSub;
////////lptkRes->tkCharIndex       =                    // Filled in above
    } // End IF/ELSE
} // End GetNextValueTokenIntoToken


//***************************************************************************
//  $GetNextValueTokenIntoNamedVarToken_EM
//
//  Return the next value from a token as a VARNAMED.
//  The token may be an empty array in which case the
//    value of the prototype is returned.
//  Note that the nested global case increments the reference count
//***************************************************************************

UBOOL GetNextValueTokenIntoNamedVarToken_EM
    (LPTOKEN      lptkArg,      // Ptr to the arg token
     APLUINT      uIndex,       // Index to use
     LPTOKEN      lptkRes)      // Ptr to the result token

{
    HGLOBAL           hGlbData,         // HGLOBAL of data in the arg
                      hGlbSub;          // ...        indexed item in the arg
    IMM_TYPES         immType;          // Immediate type
    UBOOL             bRet = TRUE;      // TRUE iff the result is valid

    // tkData is an LPSYMENTRY
    Assert (GetPtrTypeDir (lptkRes->tkData.tkVoid) EQ PTRTYPE_STCONST);

    // Free the previous value (if any)
    FreeResultName (lptkRes);

    // Fill in common values
    lptkRes->tkFlags.ImmType   = IMMTYPE_ERROR;
    lptkRes->tkFlags.NoDisplay = FALSE;
    lptkRes->tkCharIndex       = lptkArg->tkCharIndex;

    // Split cases based upon the token type
    switch (lptkArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lptkArg->tkData.tkSym->stFlags.Imm)
            {
                hGlbData = lptkArg->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir_PTB (hGlbData));

                break;      // Continue below with global case
            } // End IF

            Assert (uIndex EQ 0);

            // Handle the immediate case
            GetFirstValueImm (lptkArg->tkData.tkSym->stFlags.ImmType,
                              lptkArg->tkData.tkSym->stData.stLongest,
                              NULL,
                              NULL,
                              NULL,
                             &lptkRes->tkData.tkSym->stData.stLongest,
                             &immType,
                              NULL);
            // Fill in the result token
            lptkRes->tkFlags.TknType                  = TKT_VARNAMED;
////////////lptkRes->tkFlags.ImmType                  =     // Filled in above
////////////lptkRes->tkFlags.NoDisplay                =     // Filled in above
            lptkRes->tkData.tkSym->stFlags.Imm        = TRUE;
            lptkRes->tkData.tkSym->stFlags.ImmType    = immType;
            lptkRes->tkData.tkSym->stFlags.Value      = TRUE;
            lptkRes->tkData.tkSym->stFlags.ObjName    = OBJNAME_USR;
            lptkRes->tkData.tkSym->stFlags.stNameType = NAMETYPE_VAR;
////////////lptkRes->tkData.tkSym->stData.stLongest   =     // Filled in above
////////////lptkRes->tkCharIndex                      =     // Filled in above

            goto NORMAL_EXIT;

        case TKT_VARIMMED:
            Assert (uIndex EQ 0);

            // Handle the immediate case
            GetFirstValueImm (lptkArg->tkFlags.ImmType,
                              lptkArg->tkData.tkLongest,
                              NULL,
                              NULL,
                              NULL,
                             &lptkRes->tkData.tkSym->stData.stLongest,
                             &immType,
                              NULL);
            // Fill in the result token
            lptkRes->tkFlags.TknType                  = TKT_VARNAMED;
////////////lptkRes->tkFlags.ImmType                  =     // Filled in above
////////////lptkRes->tkFlags.NoDisplay                =     // Filled in above
            lptkRes->tkData.tkSym->stFlags.Imm        = TRUE;
            lptkRes->tkData.tkSym->stFlags.ImmType    = immType;
            lptkRes->tkData.tkSym->stFlags.Value      = TRUE;
            lptkRes->tkData.tkSym->stFlags.ObjName    = OBJNAME_USR;
            lptkRes->tkData.tkSym->stFlags.stNameType = NAMETYPE_VAR;
////////////lptkRes->tkData.tkSym->stData.stLongest   =     // Filled in above
////////////lptkRes->tkCharIndex                      =     // Filled in above

            goto NORMAL_EXIT;

        case TKT_VARARRAY:
            hGlbData = lptkArg->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (hGlbData));

            break;      // Continue below with global case

        defstop
            goto NORMAL_EXIT;
    } // End SWITCH

    // Handle the HGLOBAL case
    GetNextValueGlb (hGlbData,                              // The global memory handle
                     uIndex,                                // Index into item
                    &hGlbSub,                               // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                    &lptkRes->tkData.tkSym->stData.stLongest,// Ptr to result immediate value (may be NULL)
                    &immType);                              // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
    // If the return value is immediate, ...
    if (hGlbSub EQ NULL)
    {
        // Fill in the result token
        lptkRes->tkFlags.TknType                  = TKT_VARNAMED;
////////lptkRes->tkFlags.ImmType                  =     // Filled in above
////////lptkRes->tkFlags.NoDisplay                =     // Filled in above
        lptkRes->tkData.tkSym->stFlags.Imm        = TRUE;
        lptkRes->tkData.tkSym->stFlags.ImmType    = immType;
        lptkRes->tkData.tkSym->stFlags.Value      = TRUE;
        lptkRes->tkData.tkSym->stFlags.ObjName    = OBJNAME_USR;
        lptkRes->tkData.tkSym->stFlags.stNameType = NAMETYPE_VAR;
////////lptkRes->tkData.tkSym->stData.stLongest   =     // Filled in above
////////lptkRes->tkCharIndex                      =     // Filled in above
    } else
    {
        // Fill in the result token
        lptkRes->tkFlags.TknType                  = TKT_VARNAMED;
////////lptkRes->tkFlags.ImmType                  =     // Filled in above
////////lptkRes->tkFlags.NoDisplay                =     // Filled in above
        lptkRes->tkData.tkSym->stFlags.Imm        = FALSE;
        lptkRes->tkData.tkSym->stFlags.ImmType    = immType;
        lptkRes->tkData.tkSym->stFlags.Value      = TRUE;
        lptkRes->tkData.tkSym->stFlags.ObjName    = OBJNAME_USR;
        lptkRes->tkData.tkSym->stFlags.stNameType = NAMETYPE_VAR;
////////lptkRes->tkData.tkSym->stData.stGlbData   =     // Filled in below
////////lptkRes->tkCharIndex                      =     // Filled in above

        // Split cases based upon the immediate type
        switch (immType)
        {
            case IMMTYPE_BOOL:
            case IMMTYPE_INT:
            case IMMTYPE_FLOAT:
            case IMMTYPE_CHAR:
            case IMMTYPE_ERROR:         // For ARRAY_NESTED
                lptkRes->tkData.tkSym->stData.stGlbData   = CopySymGlbDir_PTB (hGlbSub);

                break;

            case IMMTYPE_RAT:
            case IMMTYPE_VFP:
                // Split cases based upon the ptr type
                switch (GetPtrTypeDir (hGlbSub))
                {
                    case PTRTYPE_STCONST:
                        lptkRes->tkData.tkSym->stData.stGlbData =
                          MakeGlbEntry_EM (TranslateImmTypeToArrayType (immType),   // Entry type
                                           hGlbSub,                                 // Ptr to the value
                                           TRUE,                                    // TRUE iff we should initialize the target first
                                           lptkArg);                                // Ptr to function token
                        break;

                    case PTRTYPE_HGLOBAL:
                        lptkRes->tkData.tkSym->stData.stGlbData = CopySymGlbDir_PTB (hGlbSub);

                        break;

                    defstop
                        break;
                } // End SWITCH

                break;

            defstop
                break;
        } // End SWITCH

        // Check for error
        bRet = (lptkRes->tkData.tkSym->stData.stGlbData NE NULL);
    } // End IF/ELSE
NORMAL_EXIT:
    return bRet;
} // End GetNextValueTokenIntoNamedVarToken_EM


//***************************************************************************
//  $GetNextValueToken
//
//  Return the next value from a token as either
//    both an integer and a float, or as a character,
//    or as an LPSYMENTRY/HGLOBAL.  The token may be
//    an empty array in which case the value of the
//    prototype is returned.
//***************************************************************************

void GetNextValueToken
    (LPTOKEN      lpToken,      // Ptr to the token
     APLUINT      uIndex,       // Index to use
     LPAPLINT     lpaplInteger, // Ptr to the integer (or Boolean) (may be NULL)
     LPAPLFLOAT   lpaplFloat,   // ...        float (may be NULL)
     LPAPLCHAR    lpaplChar,    // ...        char (may be NULL)
     LPAPLLONGEST lpaplLongest, // ...        longest (may be NULL)
     LPVOID      *lpSymGlb,     // ...        LPSYMENTRY or HGLOBAL (may be NULL)
     LPIMM_TYPES  lpImmType,    // ...        immediate type (see IMM_TYPES) (may be NULL)
     LPAPLSTYPE   lpArrType)    // ...        array type:  ARRAY_TYPES (may be NULL)

{
    HGLOBAL    hGlbData,
               hGlbSub;
    APLLONGEST aplLongest;
    IMM_TYPES  immType;

    // Fill in default values
    if (lpSymGlb)
        *lpSymGlb = NULL;

    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                hGlbData = lpToken->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir_PTB (hGlbData));

                break;      // Continue below with global case
            } // End IF

            Assert (uIndex EQ 0);

            // Handle the immediate case
            GetFirstValueImm (lpToken->tkData.tkSym->stFlags.ImmType,
                              lpToken->tkData.tkSym->stData.stLongest,
                              lpaplInteger,
                              lpaplFloat,
                              lpaplChar,
                              lpaplLongest,
                              lpImmType,
                              lpArrType);
            return;

        case TKT_VARIMMED:
            Assert (uIndex EQ 0);

            // Handle the immediate case
            GetFirstValueImm (lpToken->tkFlags.ImmType,
                              lpToken->tkData.tkLongest,
                              lpaplInteger,
                              lpaplFloat,
                              lpaplChar,
                              lpaplLongest,
                              lpImmType,
                              lpArrType);
            return;

        case TKT_CHRSTRAND:
        case TKT_NUMSTRAND:
        case TKT_NUMSCALAR:
        case TKT_VARARRAY:
            hGlbData = lpToken->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (hGlbData));

            break;      // Continue below with global case

        defstop
            return;
    } // End SWITCH

    // Handle the HGLOBAL case
    GetNextValueGlb (hGlbData,      // The global memory handle
                     uIndex,        // Index into item
                    &hGlbSub,       // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                    &aplLongest,    // Ptr to result immediate value (may be NULL)
                    &immType);      // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
    // Fill in various result values
    if (lpaplLongest)
       *lpaplLongest = aplLongest;
    if (lpSymGlb)
       *lpSymGlb     = hGlbSub;
    if (lpImmType)
       *lpImmType    = immType;
    if (lpArrType)
       *lpArrType    = TranslateImmTypeToArrayType (immType);;

    // If the return value is immediate, ...
    if (hGlbSub EQ NULL)
    {
        if (lpaplInteger)
           *lpaplInteger = (APLINT) aplLongest;
        if (lpaplFloat)
           *lpaplFloat   = *(LPAPLFLOAT) &aplLongest;
        if (lpaplChar)
           *lpaplChar    = (APLCHAR) aplLongest;
    } // End IF
} // End GetNextValueToken


//***************************************************************************
//  $GetFirstValueToken
//
//  Return the first value from a token as either
//    both an integer and a float, or as a character,
//    or as an LPSYMENTRY/HGLOBAL.  The token may be
//    an empty array in which case the value of the
//    prototype is returned.
//***************************************************************************

void GetFirstValueToken
    (LPTOKEN      lpToken,      // Ptr to the token
     LPAPLINT     lpaplInteger, // Ptr to the integer (or Boolean) (may be NULL)
     LPAPLFLOAT   lpaplFloat,   // ...        float (may be NULL)
     LPAPLCHAR    lpaplChar,    // ...        char (may be NULL)
     LPAPLLONGEST lpaplLongest, // ...        longest (may be NULL)
     LPVOID      *lpSymGlb,     // ...        LPSYMENTRY or HGLOBAL (may be NULL)
     LPIMM_TYPES  lpImmType,    // ...        immediate type (see IMM_TYPES) (may be NULL)
     LPAPLSTYPE   lpArrType)    // ...        array type:  ARRAY_TYPES (may be NULL)

{
    HGLOBAL hGlbData;

    // Fill in default values
    if (lpSymGlb)
        *lpSymGlb = NULL;

    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                hGlbData = lpToken->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir_PTB (hGlbData));

                break;      // Continue below with global case
            } // End IF

            // Handle the immediate case
            GetFirstValueImm (lpToken->tkData.tkSym->stFlags.ImmType,
                              lpToken->tkData.tkSym->stData.stLongest,
                              lpaplInteger,
                              lpaplFloat,
                              lpaplChar,
                              lpaplLongest,
                              lpImmType,
                              lpArrType);
            return;

        case TKT_VARIMMED:
            // Handle the immediate case
            GetFirstValueImm (lpToken->tkFlags.ImmType,
                              lpToken->tkData.tkLongest,
                              lpaplInteger,
                              lpaplFloat,
                              lpaplChar,
                              lpaplLongest,
                              lpImmType,
                              lpArrType);
            return;

        case TKT_VARARRAY:
            hGlbData = lpToken->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (hGlbData));

            break;      // Continue below with global case

        defstop
            return;
    } // End SWITCH

    // Handle the HGLOBAL case
    GetFirstValueGlb (hGlbData,         // The global memory handle
                      lpaplInteger,     // Ptr to integer (or Boolean) (may be NULL)
                      lpaplFloat,       // ...    float (may be NULL)
                      lpaplChar,        // ...    char (may be NULL)
                      lpaplLongest,     // ...    longest (may be NULL)
                      lpSymGlb,         // ...    LPSYMENTRY or HGLOBAL (may be NULL)
                      lpImmType,        // ...    immediate type (see IMM_TYPES) (may be NULL)
                      lpArrType,        // ...    array type -- ARRAY_TYPES (may be NULL)
                      TRUE);            // TRUE iff we should expand LPSYMENTRY into immediate value
} // End GetFirstValueToken


//***************************************************************************
//  $GetFirstValueImm
//
//  Return the first value of an immediate
//***************************************************************************

void GetFirstValueImm
    (IMM_TYPES    immType,      // The token's immediate type (see IMM_TYPES)
     APLLONGEST   aplLongest,   // The longest immediate value
     LPAPLINT     lpaplInteger, // Return the integer (or Boolean) (may be NULL)
     LPAPLFLOAT   lpaplFloat,   // ...        float (may be NULL)
     LPAPLCHAR    lpaplChar,    // ...        char (may be NULL)
     LPAPLLONGEST lpaplLongest, // ...        longest (may be NULL)
     LPIMM_TYPES  lpImmType,    // ...        immediate type (see IMM_TYPES) (may be NULL)
     LPAPLSTYPE   lpArrType)    // ...        array type:  ARRAY_TYPES (may be NULL)
{
    if (lpImmType)
        *lpImmType    = immType;
    if (lpArrType)
        *lpArrType    = TranslateImmTypeToArrayType (immType);
    if (lpaplLongest)
        *lpaplLongest = aplLongest;

    // Split cases based upon the immediate type
    switch (immType)
    {
        case IMMTYPE_BOOL:
            if (lpaplInteger)
                *lpaplInteger = (APLBOOL) aplLongest;
            if (lpaplFloat)
                *lpaplFloat   = (APLFLOAT) (APLBOOL) aplLongest;
            if (lpaplChar)
                *lpaplChar    = WC_EOS;
            break;

        case IMMTYPE_INT:
            if (lpaplInteger)
                *lpaplInteger = (APLINT) aplLongest;
            if (lpaplFloat)
                // ***FIXME*** -- Possible loss of precision
                *lpaplFloat   = (APLFLOAT) (APLINT) aplLongest;
            if (lpaplChar)
                *lpaplChar    = WC_EOS;
            break;

        case IMMTYPE_FLOAT:
            if (lpaplFloat)
                *lpaplFloat   = *(LPAPLFLOAT) &aplLongest;
            if (lpaplInteger)
                *lpaplInteger = (APLINT) *(LPAPLFLOAT) &aplLongest;
            if (lpaplChar)
                *lpaplChar    = WC_EOS;
            break;

        case IMMTYPE_CHAR:
            if (lpaplInteger)
                *lpaplInteger = 0;
            if (lpaplFloat)
                *lpaplFloat   = 0;
            if (lpaplChar)
                *lpaplChar    = (APLCHAR) aplLongest;
            break;

        defstop
            break;
    } // End SWITCH
} // End GetFirstValueImm


//***************************************************************************
//  $GetFirstValueGlb
//
//  Return the first value from an HGLOBAL as either
//    both an integer and a float, or as a character,
//    or as an HGLOBAL.  The HGLOBAL may be
//    an empty array in which case the value of the
//    prototype is returned.
//***************************************************************************

void GetFirstValueGlb
    (HGLOBAL      hGlbData,         // The global memory handle
     LPAPLINT     lpaplInteger,     // Return the integer (or Boolean) (may be NULL)
     LPAPLFLOAT   lpaplFloat,       // ...        float (may be NULL)
     LPAPLCHAR    lpaplChar,        // ...        char (may be NULL)
     LPAPLLONGEST lpaplLongest,     // ...        longest (may be NULL)
     LPVOID      *lpSymGlb,         // ...        LPSYMENTRY or HGLOBAL (may be NULL)
     LPIMM_TYPES  lpImmType,        // ...        immediate type (see IMM_TYPES) (may be NULL)
     LPAPLSTYPE   lpArrType,        // ...        array type -- ARRAY_TYPES (may be NULL)
     UBOOL        bExpandSym)       // TRUE iff we should expand LPSYMENTRY into immediate value

{
    LPVOID     lpMem;               // Ptr to global memory
    APLSTYPE   aplType;             // Storage type
    APLNELM    aplNELM;             // NELM
    APLINT     aplInteger;          // Temp integer
    APLFLOAT   aplFloat;            // ...  float
    APLCHAR    aplChar;             // ...  char
    LPSYMENTRY lpSym;               // Ptr to SYMENTRY in HETERO or NESTED cases

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbData);

    // Split cases based upon the signature
    switch (GetSignatureMem (lpMem))
    {
        case LSTARRAY_HEADER_SIGNATURE:
#define lpHeader    ((LPLSTARRAY_HEADER) lpMem)
            // Get the Array Type and NELM
            aplType = ARRAY_LIST;
            aplNELM = lpHeader->NELM;

            // Skip over the header to the data
            lpMem = LstArrayBaseToData (lpMem);
#undef  lpHeader
            break;

        case VARARRAY_HEADER_SIGNATURE:
#define lpHeader    ((LPVARARRAY_HEADER) lpMem)
            // Get the Array Type and NELM
            aplType = lpHeader->ArrType;
            aplNELM = lpHeader->NELM;

            // Skip over the header and dimensions to the data
            lpMem = VarArrayBaseToData (lpMem, lpHeader->Rank);
#undef  lpHeader
            break;

        case DFN_HEADER_SIGNATURE:
        case FCNARRAY_HEADER_SIGNATURE:
        defstop
    } // End SWITCH

    if (lpImmType)
        *lpImmType = TranslateArrayTypeToImmType (aplType);
    if (lpArrType)
        *lpArrType = aplType;

    // Split cases based upon the storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
            // If the array is empty and numeric, the value must be Boolean or APA
            aplInteger = aplNELM ? (BIT0 & *(LPAPLBOOL) lpMem) : 0;

            if (lpaplInteger)
                *lpaplInteger = aplInteger;
            if (lpaplFloat)
                *lpaplFloat   = (APLFLOAT) aplInteger;
            if (lpaplChar)
                *lpaplChar    = WC_EOS;
            if (lpaplLongest)
                *lpaplLongest = aplInteger;
            if (lpSymGlb)
                *lpSymGlb     = NULL;
            break;

        case ARRAY_INT:
            aplInteger = aplNELM ? *(LPAPLINT) lpMem : 0;

            if (lpaplInteger)
                *lpaplInteger = aplInteger;
            if (lpaplFloat)
                *lpaplFloat   = (APLFLOAT) aplInteger;  // ***FIXME*** -- Possible loss of precision
            if (lpaplChar)
                *lpaplChar    = WC_EOS;
            if (lpaplLongest)
                *lpaplLongest = aplInteger;
            if (lpSymGlb)
                *lpSymGlb     = NULL;
            break;

        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMem)
            // If the array is empty and numeric, the value must be Boolean or APA
            aplInteger = aplNELM ? lpAPA->Off : 0;
#undef  lpAPA
            if (lpaplInteger)
                *lpaplInteger = aplInteger;
            if (lpaplFloat)
                *lpaplFloat   = (APLFLOAT) aplInteger;  // ***FIXME*** -- Possible loss of precision
            if (lpaplChar)
                *lpaplChar    = WC_EOS;
            if (lpaplLongest)
                *lpaplLongest = aplInteger;
            if (lpSymGlb)
                *lpSymGlb     = NULL;
            break;

        case ARRAY_FLOAT:
            aplFloat = aplNELM ? *(LPAPLFLOAT) lpMem : 0;

            if (lpaplFloat)
                *lpaplFloat   = aplFloat;
            if (lpaplInteger)
                *lpaplInteger = (APLINT) aplFloat;
            if (lpaplChar)
                *lpaplChar    = WC_EOS;
            if (lpaplLongest)
                *lpaplLongest = *(LPAPLLONGEST) &aplFloat;
            if (lpSymGlb)
                *lpSymGlb     = NULL;
            break;

        case ARRAY_CHAR:
            // If the array is empty and char, the value must be blank
            aplChar = aplNELM ? (*(LPAPLCHAR) lpMem) : L' ';

            if (lpaplInteger)
                *lpaplInteger = 0;
            if (lpaplFloat)
                *lpaplFloat   = 0;
            if (lpaplChar)
                *lpaplChar    = aplChar;
            if (lpaplLongest)
                *lpaplLongest = aplChar;
            if (lpSymGlb)
                *lpSymGlb     = NULL;
            break;

        case ARRAY_LIST:
#define lptkList        ((LPAPLLIST) lpMem)
            // Split cases based upon the token type
            switch (lptkList->tkFlags.TknType)
            {
                case TKT_VARIMMED:
                    // Fill in default values
                    if (lpSymGlb)
                        *lpSymGlb = NULL;

                    // Handle the immediate case
                    GetFirstValueImm (lptkList->tkFlags.ImmType,
                                      lptkList->tkData.tkLongest,
                                      lpaplInteger,
                                      lpaplFloat,
                                      lpaplChar,
                                      lpaplLongest,
                                      lpImmType,
                                      lpArrType);
                    break;

                case TKT_VARARRAY:
                    // Handle the global case
                    GetFirstValueGlb (lptkList->tkData.tkGlbData,   // The global memory handle
                                      lpaplInteger,                 // Ptr to integer (or Boolean) (may be NULL)
                                      lpaplFloat,                   // ...    float (may be NULL)
                                      lpaplChar,                    // ...    char (may be NULL)
                                      lpaplLongest,                 // ...    longest (may be NULL)
                                      lpSymGlb,                     // ...    LPSYMENTRY or HGLOBAL (may be NULL)
                                      lpImmType,                    // ...    immediate type (see IMM_TYPES) (may be NULL)
                                      lpArrType,                    // ...    array type -- ARRAY_TYPES (may be NULL)
                                      FALSE);                       // TRUE iff we should expand LPSYMENTRY into immediate value
                    break;

                case TKT_LISTSEP:
                    if (lpaplInteger)
                        *lpaplInteger = 0;
                    if (lpaplFloat)
                        *lpaplFloat   = 0;
                    if (lpaplChar)
                        *lpaplChar    = WC_EOS;
                    if (lpaplLongest)
                        *lpaplLongest = WC_EOS;
                    if (lpSymGlb)
                        *lpSymGlb     = NULL;
                    break;

                defstop
                    break;
            } // End SWITCH
#undef  lptkList
            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            lpSym = *(LPAPLHETERO) lpMem;

            // Split cases based upon the ptr type
            switch (GetPtrTypeDir (lpSym))
            {
                case PTRTYPE_STCONST:
                    // stData is immediate
                    Assert (lpSym->stFlags.Imm);

                    if (!bExpandSym
                     && lpSymGlb)
                    {
                        *lpSymGlb = lpSym;

                        break;
                    } // End IF

                    Assert (IsNested (aplType) || !IsEmpty (aplNELM));

                    // Fill in default values
                    if (lpSymGlb)
                        *lpSymGlb = NULL;

                    // Handle the immediate case
                    GetFirstValueImm (lpSym->stFlags.ImmType,
                                      lpSym->stData.stLongest,
                                      lpaplInteger,
                                      lpaplFloat,
                                      lpaplChar,
                                      lpaplLongest,
                                      lpImmType,
                                      lpArrType);
                    break;

                case PTRTYPE_HGLOBAL:
                    if (lpaplInteger)
                        *lpaplInteger = 0;
                    if (lpaplFloat)
                        *lpaplFloat   = 0;
                    if (lpaplChar)
                        *lpaplChar    = WC_EOS;
                    if (lpaplLongest)
                        *lpaplLongest = 0;
                    if (lpSymGlb)
                        *lpSymGlb     = *(LPAPLNESTED) lpMem;
                    if (lpImmType)
                    {
                        AttrsOfGlb (*(LPAPLNESTED) lpMem, &aplType, NULL, NULL, NULL);
                        *lpImmType    = TranslateArrayTypeToImmType (aplType);
                    } // End IF

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_RAT:
        case ARRAY_VFP:
            if (lpaplInteger)
                *lpaplInteger = 0;
            if (lpaplFloat)
                *lpaplFloat   = 0;
            if (lpaplChar)
                *lpaplChar    = WC_EOS;
            if (lpaplLongest)
                *lpaplLongest = 0;
            // If the array is empty, ...
            if (IsEmpty (aplNELM))
            {
                if (lpSymGlb)
                    *lpSymGlb  = NULL;
                if (lpImmType)
                    *lpImmType = IMMTYPE_BOOL;
                if (lpArrType)
                    *lpArrType = ARRAY_BOOL;
            } else
                if (lpSymGlb)
                    *lpSymGlb  = lpMem;
            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMem = NULL;
} // End GetFirstValueGlb


//***************************************************************************
//  $GetNextValueMemIntoToken
//
//  Get the next value from global memory into a token
//  Note that the nested global case increments the reference count
//***************************************************************************

UBOOL GetNextValueMemIntoToken
    (APLUINT  uArg,                         // Index to use
     LPVOID   lpMemArg,                     // Ptr to global memory object to index
     APLSTYPE aplTypeArg,                   // Storage type of the arg
     APLINT   apaOff,                       // APA offset (if needed)
     APLINT   apaMul,                       // APA multiplier (if needed)
     LPTOKEN  lptkArg)                      // Ptr to token in which to place the value

{
    // Clear the NoDisplay flag
    lptkArg->tkFlags.NoDisplay = FALSE;

    // Split cases based upon the arg storage type
    switch (aplTypeArg)
    {
        case ARRAY_BOOL:
            lptkArg->tkFlags.TknType  = TKT_VARIMMED;
            lptkArg->tkFlags.ImmType  = IMMTYPE_BOOL;
            lptkArg->tkData.tkInteger = BIT0 & (UCHAR) (((LPAPLBOOL) lpMemArg)[uArg >> LOG2NBIB] >> (MASKLOG2NBIB & uArg));

            break;

        case ARRAY_INT:
            lptkArg->tkFlags.TknType  = TKT_VARIMMED;
            lptkArg->tkFlags.ImmType  = IMMTYPE_INT;
            lptkArg->tkData.tkInteger = ((LPAPLINT) lpMemArg)[uArg];

            break;

        case ARRAY_FLOAT:
            lptkArg->tkFlags.TknType  = TKT_VARIMMED;
            lptkArg->tkFlags.ImmType  = IMMTYPE_FLOAT;
            lptkArg->tkData.tkFloat   = ((LPAPLFLOAT) lpMemArg)[uArg];

            break;

        case ARRAY_CHAR:
            lptkArg->tkFlags.TknType  = TKT_VARIMMED;
            lptkArg->tkFlags.ImmType  = IMMTYPE_CHAR;
            lptkArg->tkData.tkChar    = ((LPAPLCHAR) lpMemArg)[uArg];

            break;

        case ARRAY_APA:
            lptkArg->tkFlags.TknType  = TKT_VARIMMED;
            lptkArg->tkFlags.ImmType  = IMMTYPE_INT;
            lptkArg->tkData.tkInteger = (apaOff + apaMul * uArg);

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Split cases based upon the ptr type
            switch (GetPtrTypeDir (((LPAPLNESTED) lpMemArg)[uArg]))
            {
                case PTRTYPE_STCONST:
                    lptkArg->tkFlags.TknType  = TKT_VARIMMED;
                    lptkArg->tkFlags.ImmType  = ((LPAPLHETERO) lpMemArg)[uArg]->stFlags.ImmType;
                    lptkArg->tkData.tkLongest = ((LPAPLHETERO) lpMemArg)[uArg]->stData.stLongest;

                    break;

                case PTRTYPE_HGLOBAL:
                    lptkArg->tkFlags.TknType  = TKT_VARARRAY;
                    lptkArg->tkFlags.ImmType  = IMMTYPE_ERROR;
                    lptkArg->tkData.tkGlbData = CopySymGlbDir_PTB (((LPAPLNESTED) lpMemArg)[uArg]);

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_RAT:
            lptkArg->tkFlags.TknType  = TKT_VARARRAY;
            lptkArg->tkFlags.ImmType  = IMMTYPE_RAT;
            lptkArg->tkData.tkGlbData =
              MakeGlbEntry_EM (aplTypeArg,                      // Entry type
                              &((LPAPLRAT) lpMemArg)[uArg],     // Ptr to the value
                               TRUE,                            // TRUE iff we should initialize the target first
                               lptkArg);                        // Ptr to function token
            if (!lptkArg->tkData.tkGlbData)
                goto ERROR_EXIT;
            break;

        case ARRAY_VFP:
            lptkArg->tkFlags.TknType  = TKT_VARARRAY;
            lptkArg->tkFlags.ImmType  = IMMTYPE_VFP;
            lptkArg->tkData.tkGlbData =
              MakeGlbEntry_EM (aplTypeArg,                      // Entry type
                              &((LPAPLVFP) lpMemArg)[uArg],     // Ptr to the value
                               TRUE,                            // TRUE iff we should initialize the target first
                               lptkArg);                        // Ptr to function token
            if (!lptkArg->tkData.tkGlbData)
                goto ERROR_EXIT;
            break;

        defstop
            break;
    } // End SWITCH

    return TRUE;

ERROR_EXIT:
    // Set the token type to something FreeResultTkn will ignore
    lptkArg->tkFlags.TknType  = TKT_VARIMMED;

    return FALSE;
} // End GetNextValueMemIntoToken


//***************************************************************************
//  $GetNextRatIntGlb
//
//  Get the next value from a rational array global memory handle
//    as an integer using System []CT
//***************************************************************************

APLINT GetNextRatIntGlb
    (HGLOBAL hGlbRat,                       // Global memory handle
     APLINT  uRes,                          // Index
     LPUBOOL lpbRet)                        // Ptr to TRUE iff the result is valid

{
    LPAPLRAT lpMemRat;
    APLINT   aplInteger;

    Assert (IsGlbTypeVarDir_PTB (hGlbRat));

    // Lock the memory to get a ptr to it
    lpMemRat = MyGlobalLock (hGlbRat);

    // Skip over the header and dimensions to the data
    lpMemRat = VarArrayBaseToData (lpMemRat, ((LPVARARRAY_HEADER) lpMemRat)->Rank);

    // Convert the next RAT to an INT using System []CT
    aplInteger = mpq_get_sctsx (&lpMemRat[uRes], lpbRet);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRat); lpMemRat = NULL;

    return aplInteger;
} // End GetNextRatIntGlb


//***************************************************************************
//  $GetNextRatIntMem
//
//  Get the next value from a rational array global memory handle
//    as an integer using System []CT
//***************************************************************************

APLINT GetNextRatIntMem
    (LPAPLRAT lpMemRat,                     // Ptr to global memory
     APLINT   uRes,                         // Index
     LPUBOOL  lpbRet)                       // Ptr to TRUE iff the result is valid

{
    // Convert the next RAT to an INT using System []CT
    return mpq_get_sctsx (&lpMemRat[uRes], lpbRet);
} // End GetNextRatIntMem


//***************************************************************************
//  $GetNextRatFltMem
//
//  Get the next value from a rational array global memory ptr
//***************************************************************************

APLFLOAT GetNextRatFltMem
    (LPAPLRAT lpMemRat,                     // Ptr to global memory
     APLINT   uRes,                         // Index
     LPUBOOL  lpbRet)                       // Ptr to TRUE iff the result is valid

{
    APLVFP mpfTmp = {0};

    // Initialize the temp
    mpfr_init (&mpfTmp);

    // Copy the RAT to a VFP
    mpfr_set_q (&mpfTmp, &lpMemRat[uRes], MPFR_RNDN);

    // Zero the sign
    mpfr_abs (&mpfTmp, &mpfTmp, MPFR_RNDN);

    // Compare against the maximum float
    if (mpfr_cmp_d (&mpfTmp, DBL_MAX) > 0
     || (mpfr_cmp_d (&mpfTmp, DBL_MIN) < 0
      && !mpfr_zero_p (&mpfTmp)))
        *lpbRet = FALSE;
    else
        *lpbRet = TRUE;
    // We no longer need this storage
    Myf_clear (&mpfTmp);

    // If the value is in range, ...
    if (*lpbRet)
        // Convert the next RAT to an FLOAT
        return mpq_get_d (&lpMemRat[uRes]);
    else
        return 0.0;
} // End GetNextRatFltMem


//***************************************************************************
//  $GetNextVfpIntGlb
//
//  Get the next value from a VFP array global memory handle
//    as an integer using System []CT
//***************************************************************************

APLINT GetNextVfpIntGlb
    (HGLOBAL hGlbVfp,                       // Global memory handle
     APLINT  uRes,                          // Index
     LPUBOOL lpbRet)                        // Ptr to TRUE iff the result is valid

{
    LPAPLVFP lpMemVfp;
    APLINT   aplInteger;

    Assert (IsGlbTypeVarDir_PTB (hGlbVfp));

    // Lock the memory to get a ptr to it
    lpMemVfp = MyGlobalLock (hGlbVfp);

    // Skip over the header and dimensions to the data
    lpMemVfp = VarArrayBaseToData (lpMemVfp, ((LPVARARRAY_HEADER) lpMemVfp)->Rank);

    // Convert the next VFP to an INT using System []CT
    aplInteger = mpfr_get_sctsx (&lpMemVfp[uRes], lpbRet);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbVfp); lpMemVfp = NULL;

    return aplInteger;
} // End GetNextVfpIntGlb


//***************************************************************************
//  $GetNextVfpIntMem
//
//  Get the next value from a VFP array global memory ptr
//    as an integer using System []CT
//***************************************************************************

APLINT GetNextVfpIntMem
    (LPAPLVFP lpMemVfp,                     // Ptr to global memory
     APLINT   uRes,                         // Index
     LPUBOOL  lpbRet)                       // Ptr to TRUE iff the result is valid

{
    // Convert the next VFP to an INT using System []CT
    return mpfr_get_sctsx (&lpMemVfp[uRes], lpbRet);
} // End GetNextVfpIntMem


//***************************************************************************
//  $GetNextVfpFltMem
//
//  Get the next value from a VFP array global memory ptr
//***************************************************************************

APLFLOAT GetNextVfpFltMem
    (LPAPLVFP lpMemVfp,                     // Ptr to global memory
     APLINT   uRes,                         // Index
     LPUBOOL  lpbRet)                       // Ptr to TRUE iff the result is valid

{
    APLVFP mpfTmp = {0};

    // Initialize the temp
    mpfr_init (&mpfTmp);

    // Copy the RAT to a VFP
    mpfr_set (&mpfTmp, &lpMemVfp[uRes], MPFR_RNDN);

    // Zero the sign
    mpfr_abs (&mpfTmp, &mpfTmp, MPFR_RNDN);

    // Compare against the maximum float
    if (mpfr_cmp_d (&mpfTmp, DBL_MAX) > 0
     || (mpfr_cmp_d (&mpfTmp, DBL_MIN) < 0
      && !mpfr_zero_p (&mpfTmp)))
        *lpbRet = FALSE;
    else
        *lpbRet = TRUE;
    // We no longer need this storage
    Myf_clear (&mpfTmp);

    // If the value is in range, ...
    if (*lpbRet)
        // Convert the next VFP to an FLOAT
        return mpfr_get_d (&lpMemVfp[uRes], MPFR_RNDN);
    else
        return 0.0;
} // End GetNextVfpFltMem


//***************************************************************************
//  $GetNextInteger
//
//  Return the next value as an integer
//***************************************************************************

APLINT GetNextInteger
    (LPVOID   lpMem,                        // Ptr to global memory
     APLSTYPE aplType,                      // Storage type
     APLINT   uRes)                         // Index

{
    // Split cases based upon the storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
            return BIT0 & (((LPAPLBOOL) lpMem)[uRes >> LOG2NBIB] >> (MASKLOG2NBIB & uRes));

        case ARRAY_INT:
            return ((LPAPLINT) lpMem)[uRes];

        case ARRAY_FLOAT:
            return (APLINT) ((LPAPLFLOAT) lpMem)[uRes];

        case ARRAY_APA:
            return ((LPAPLAPA) lpMem)->Off + ((LPAPLAPA) lpMem)->Mul * uRes;

        defstop
            return 0;
    } // End SWITCH
} // End GetNextInteger


//***************************************************************************
//  $GetNextFloat
//
//  Return the next value as a float
//***************************************************************************

APLFLOAT GetNextFloat
    (LPVOID   lpMem,                        // Ptr to global memory
     APLSTYPE aplType,                      // Storage type
     APLINT   uRes)                         // Index

{
    // Split cases based upon the storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
            return (APLFLOAT) (BIT0 & (((LPAPLBOOL) lpMem)[uRes >> LOG2NBIB] >> (MASKLOG2NBIB & uRes)));

        case ARRAY_INT:
            // ***FIXME*** -- Possible loss of precision
            return (APLFLOAT) ((LPAPLINT) lpMem)[uRes];

        case ARRAY_FLOAT:
            return ((LPAPLFLOAT) lpMem)[uRes];

        case ARRAY_APA:
            // ***FIXME*** -- Possible loss of precision
            return (APLFLOAT) (((LPAPLAPA) lpMem)->Off + ((LPAPLAPA) lpMem)->Mul * uRes);

        defstop
            return 0;
    } // End SWITCH
} // End GetNextFloat


//***************************************************************************
//  $GetNextHetero
//
//  Return the next value as Boolean/Integer, Float, Char, Rat, or Vfp
//***************************************************************************

APLSTYPE GetNextHetero
    (LPAPLHETERO lpMem,                     // Ptr to global memory
     APLINT      uRes,                      // Index
     LPAPLINT    lpaplInteger,              // Ptr to Boolean/Integer result
     LPAPLFLOAT  lpaplFloat,                // Ptr to Float result
     LPAPLCHAR   lpaplChar,                 // Ptr to Char result
     HGLOBAL    *lplpSymGlb)                // Ptr to global memory handle (may be NULL)

{
    LPSYMENTRY lpSymEntry;
    HGLOBAL    lpSymGlb;
    APLSTYPE   aplType;

    // Get the next LPSYMENTRY
    lpSymEntry = lpMem[uRes];

    // Split cases based upon the ptr type bits
    switch (GetPtrTypeDir (lpSymEntry))
    {
        case PTRTYPE_STCONST:
            // stData is immediate
            Assert (lpSymEntry->stFlags.Imm);

            // Split cases based upon the immediate storage type
            switch (lpSymEntry->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    *lpaplInteger = lpSymEntry->stData.stBoolean;
                    *lpaplFloat   = (APLFLOAT) *lpaplInteger;  // ***FIXME*** -- Possible loss of precision

                    return ARRAY_BOOL;

                case IMMTYPE_INT:
                    *lpaplInteger = lpSymEntry->stData.stInteger;
                    *lpaplFloat   = (APLFLOAT) *lpaplInteger;  // ***FIXME*** -- Possible loss of precision

                    return ARRAY_INT;

                case IMMTYPE_FLOAT:
                    *lpaplFloat   = lpSymEntry->stData.stFloat;

                    return ARRAY_FLOAT;

                case IMMTYPE_CHAR:
                    *lpaplChar    = lpSymEntry->stData.stChar;

                    return ARRAY_CHAR;

                defstop
                    return ARRAY_ERROR;
            } // End SWITCH

            break;

        case PTRTYPE_HGLOBAL:
            // Copy the LPSYMENTRY
            lpSymGlb = lpSymEntry;

            // Get the attributes (Type, NELM, and Rank) of the global
            AttrsOfGlb (lpSymGlb, &aplType, NULL, NULL, NULL);

            // If the arg is global numeric, ...
            if (IsGlbNum (aplType))
            {
                Assert (IsGlbTypeVarDir_PTB (lpSymGlb));

                // Clear the ptr type bits
                lpSymGlb = ClrPtrTypeDir (lpSymGlb);

                // Skip over the header to the data
                lpSymGlb = VarArrayBaseToData (lpSymGlb, 0);
            } // End IF

            if (lplpSymGlb)
                *lplpSymGlb = lpSymGlb;

            return aplType;

        defstop
            return ARRAY_ERROR;
    } // End SWITCH
} // End GetNextHetero


//***************************************************************************
//  $GetNextItemGlb
//
//  Get next item from a global memory handle
//  If *lphGlbRes is NULL on exit, the result is an immediate.
//***************************************************************************

void GetNextItemGlb
    (HGLOBAL     hGlbSub,                   // Item global memory handle
     APLUINT     uSub,                      // Index into item
     HGLOBAL    *lphGlbRes,                 // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
     APLLONGEST *lpaplLongestRes)           // Ptr to result immediate value (may be NULL)

{
    APLSTYPE  aplTypeSub;                   // Item storage type
#ifdef DEBUG
    APLNELM   aplNELMSub;                   // Item NELM
#endif
    APLRANK   aplRankSub;                   // Item rank
    LPVOID    lpMemSub;                     // Ptr to item global memory

    // Lock the memory to get a ptr to it
    lpMemSub = MyGlobalLock (hGlbSub);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemSub)
    // Get the Array Type and Rank
    aplTypeSub = lpHeader->ArrType;
#ifdef DEBUG
    aplNELMSub = lpHeader->NELM;
#endif
    aplRankSub = lpHeader->Rank;
#undef  lpHeader

    Assert (uSub < aplNELMSub);

    // Skip over the header and dimensions to the data
    lpMemSub = VarArrayBaseToData (lpMemSub, aplRankSub);

    // Get next item from global memory
    GetNextItemMem (lpMemSub,               // Ptr to item global memory data
                    aplTypeSub,             // Item storage type
                    uSub,                   // Index into item
                    lphGlbRes,              // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                    lpaplLongestRes);       // Ptr to result immediate value (may be NULL)
    // We no longer need this ptr
    MyGlobalUnlock (hGlbSub); lpMemSub = NULL;
} // End GetNextItemGlb


//***************************************************************************
//  $GetNextValueGlb
//
//  Get next value from a global memory handle
//  If *lphGlbRes is NULL on exit, the result is an immediate.
//***************************************************************************

void GetNextValueGlb
    (HGLOBAL     hGlbSub,                   // Item global memory handle
     APLUINT     uSub,                      // Index into item
     HGLOBAL    *lphGlbRes,                 // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
     APLLONGEST *lpaplLongestRes,           // Ptr to result immediate value (may be NULL)
     IMM_TYPES  *lpimmTypeRes)              // Ptr to result immediate type (see IMM_TYPES) (may be NULL)

{
    APLSTYPE  aplTypeSub;                   // Item storage type
#ifdef DEBUG
    APLNELM   aplNELMSub;                   // Item NELM
#endif
    APLRANK   aplRankSub;                   // Item rank
    LPVOID    lpMemSub;                     // Ptr to item global memory

    // Lock the memory to get a ptr to it
    lpMemSub = MyGlobalLock (hGlbSub);

    // Split cases based upon the signature
    switch (GetSignatureMem (lpMemSub))
    {
        case LSTARRAY_HEADER_SIGNATURE:
#define lpHeader        ((LPLSTARRAY_HEADER) lpMemSub)
            // Get the Array Type and Rank
            aplTypeSub = ARRAY_LIST;
#ifdef DEBUG
            aplNELMSub = lpHeader->NELM;
#endif
            aplRankSub = 1;
#undef  lpHeader
            // Skip over the header and dimensions to the data
            lpMemSub = LstArrayBaseToData (lpMemSub);

            break;

        case VARARRAY_HEADER_SIGNATURE:
#define lpHeader        ((LPVARARRAY_HEADER) lpMemSub)
            // Get the Array Type and Rank
            aplTypeSub = lpHeader->ArrType;
#ifdef DEBUG
            aplNELMSub = lpHeader->NELM;
#endif
            aplRankSub = lpHeader->Rank;
#undef  lpHeader
            // Skip over the header and dimensions to the data
            lpMemSub = VarArrayBaseToData (lpMemSub, aplRankSub);

            break;

        case DFN_HEADER_SIGNATURE:
        case FCNARRAY_HEADER_SIGNATURE:
        defstop
            break;
    } // End SWITCH

    Assert ((uSub < aplNELMSub)
         || ((uSub EQ 0) && IsNested (aplTypeSub)));

    // Get next value from global memory
    GetNextValueMem (lpMemSub,              // Ptr to item global memory data
                     aplTypeSub,            // Item storage type
                     uSub,                  // Index into item
                     lphGlbRes,             // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                     lpaplLongestRes,       // Ptr to result immediate value (may be NULL)
                     lpimmTypeRes);         // Ptr to result immediate type (may be NULL)
    // We no longer need this ptr
    MyGlobalUnlock (hGlbSub); lpMemSub = NULL;
} // End GetNextValueGlb


//***************************************************************************
//  $GetNextValueMem
//
//  Get next value from global memory
//  If *lphGlbRes is NULL on exit, the result is an immediate.
//***************************************************************************

void GetNextValueMem
    (LPVOID      lpMemSub,                  // Ptr to item global memory data
     APLSTYPE    aplTypeSub,                // Item storage type
     APLUINT     uSub,                      // Index into item
     HGLOBAL    *lphGlbRes,                 // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
     APLLONGEST *lpaplLongestRes,           // Ptr to result immediate value (may be NULL)
     IMM_TYPES  *lpimmTypeRes)              // Ptr to result immediate type (see IMM_TYPES) (may be NULL)

{
    GetNextValueMemSub (lpMemSub,           // Ptr to item global memory data
                        aplTypeSub,         // Item storage type
                        uSub,               // Index into item
                        lphGlbRes,          // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                        lpaplLongestRes,    // Ptr to result immediate value (may be NULL)
                        lpimmTypeRes);      // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
} // End GetNextValueMem


//***************************************************************************
//  $GetNextItemMem
//
//  Get next item from global memory
//  If *lphGlbRes is NULL on exit, the result is an immediate.
//***************************************************************************

void GetNextItemMem
    (LPVOID      lpMemSub,              // Ptr to item global memory data
     APLSTYPE    aplTypeSub,            // Item storage type
     APLUINT     uSub,                  // Index into item
     HGLOBAL    *lphGlbRes,             // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
     APLLONGEST *lpaplLongestRes)       // Ptr to result immediate value (may be NULL)

{
    GetNextValueMemSub (lpMemSub,           // Ptr to item global memory data
                        aplTypeSub,         // Item storage type
                        uSub,               // Index into item
                        lphGlbRes,          // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                        lpaplLongestRes,    // Ptr to result immediate value (may be NULL)
                        NULL);              // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
} // End GetNextItemMem


//***************************************************************************
//  $GetNextSimple
//
//  Get next simple (or GlbNum) item from global memory
//***************************************************************************

void GetNextSimple
    (LPVOID      lpMemSub,                  // Ptr to item global memory data
     APLSTYPE    aplTypeSub,                // Item storage type
     APLUINT     uSub,                      // Index into item
     LPAPLINT    lpaplInteger,              // Ptr to Boolean/Integer result
     LPAPLFLOAT  lpaplFloat,                // Ptr to Float result
     LPAPLCHAR   lpaplChar,                 // Ptr to Char result
     HGLOBAL    *lplpSymGlb)                // Ptr to global memory handle (may be NULL)

{
    APLLONGEST aplLongest;                  // Ptr to result immediate value (may be NULL)

    GetNextValueMemSub (lpMemSub,           // Ptr to item global memory data
                        aplTypeSub,         // Item storage type
                        uSub,               // Index into item
                        lplpSymGlb,         // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                       &aplLongest,         // Ptr to result immediate value (may be NULL)
                        NULL);              // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
    if (lpaplInteger)
       *lpaplInteger = aplLongest;
    if (lpaplFloat)
       *lpaplFloat   = *(LPAPLFLOAT) &aplLongest;
    if (lpaplChar)
       *lpaplChar    = (APLCHAR) aplLongest;
} // End GetNextSimple


//***************************************************************************
//  $GetNextValueMemSub
//
//  Get next value from global memory
//  If *lphGlbRes is NULL on exit, the result is an immediate.
//  If both lphGlbRes and lpimmTypeRes are not NULL, *lphGlbRes
//    is never an LPSYMENTRY; instead *lpimmTypeRes is filled in
//    with the immediate type.
//***************************************************************************

void GetNextValueMemSub
    (LPVOID      lpMemSub,              // Ptr to item global memory data
     APLSTYPE    aplTypeSub,            // Item storage type
     APLUINT     uSub,                  // Index into item
     HGLOBAL    *lphGlbRes,             // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
     APLLONGEST *lpaplLongestRes,       // Ptr to result immediate value (may be NULL)
     IMM_TYPES  *lpimmTypeRes)          // Ptr to result immediate type (see IMM_TYPES) (may be NULL)

{
    APLHETERO lpSymSub;                 // Item as APLHETERO
    LPAPLLIST lptkList;                 // Item as APLLIST

    // Assume the result is an immediate
    if (lphGlbRes)
        *lphGlbRes = NULL;

    // Split cases based upon the right arg storage type
    switch (aplTypeSub)
    {
        case ARRAY_BOOL:
            if (lpaplLongestRes)
                *lpaplLongestRes = BIT0 & (((LPAPLBOOL) lpMemSub)[uSub >> LOG2NBIB] >> (MASKLOG2NBIB & uSub));
            if (lpimmTypeRes)
                *lpimmTypeRes    = IMMTYPE_BOOL;
            break;

        case ARRAY_INT:
            if (lpaplLongestRes)
                *lpaplLongestRes = ((LPAPLINT) lpMemSub)[uSub];
            if (lpimmTypeRes)
                *lpimmTypeRes    = IMMTYPE_INT;
            break;

        case ARRAY_FLOAT:
            if (lpaplLongestRes)
                *lpaplLongestRes = *(LPAPLLONGEST) &((LPAPLFLOAT) lpMemSub)[uSub];
            if (lpimmTypeRes)
                *lpimmTypeRes    = IMMTYPE_FLOAT;
            break;

        case ARRAY_CHAR:
            if (lpaplLongestRes)
                *lpaplLongestRes = ((LPAPLCHAR) lpMemSub)[uSub];
            if (lpimmTypeRes)
                *lpimmTypeRes    = IMMTYPE_CHAR;
            break;

        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemSub)
            if (lpaplLongestRes)
                *lpaplLongestRes = lpAPA->Off + lpAPA->Mul * uSub;
            if (lpimmTypeRes)
                *lpimmTypeRes    = IMMTYPE_INT;
#undef  lpAPA
            break;

        case ARRAY_LIST:
            lptkList = &((LPAPLLIST) lpMemSub)[uSub];

            // Split cases based upon the token type
            switch (lptkList->tkFlags.TknType)
            {
                case TKT_VARIMMED:
                    // Extract the immediate type & value
                    if (lpaplLongestRes)
                        *lpaplLongestRes = lptkList->tkData.tkLongest;
                    if (lpimmTypeRes)
                        *lpimmTypeRes    = lptkList->tkFlags.ImmType;
                    break;

                case TKT_VARARRAY:
                    if (lpaplLongestRes)
                        *lpaplLongestRes = 0;
                    if (lpimmTypeRes)
                    {
                        APLSTYPE aplTypeSub;

                        // Get the global attrs
                        AttrsOfGlb (lptkList->tkData.tkGlbData, &aplTypeSub, NULL, NULL, NULL);

                        *lpimmTypeRes    = TranslateArrayTypeToImmType (aplTypeSub);;
                    } // End IF

                    if (lphGlbRes)
                        *lphGlbRes       = lptkList->tkData.tkGlbData;
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Get the item
            lpSymSub = ((LPAPLNESTED) lpMemSub)[uSub];

            // Split cases based upon the ptr type
            switch (GetPtrTypeDir (lpSymSub))
            {
                case PTRTYPE_STCONST:
                    // Extract the immediate type & value
                    if (lpaplLongestRes)
                        *lpaplLongestRes = lpSymSub->stData.stLongest;
                    if (lpimmTypeRes)
                        *lpimmTypeRes    = lpSymSub->stFlags.ImmType;
                    break;

                case PTRTYPE_HGLOBAL:
                    if (lphGlbRes)
                        *lphGlbRes       = lpSymSub;
                    if (lpimmTypeRes)
                        *lpimmTypeRes    = GetImmTypeGlb (lpSymSub);
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

      case ARRAY_RAT:
            if (lpaplLongestRes)
                *lpaplLongestRes = 0;
            if (lpimmTypeRes)
                *lpimmTypeRes    = IMMTYPE_RAT;
            if (lphGlbRes)
                *lphGlbRes       = &((LPAPLRAT) lpMemSub)[uSub];
            break;

      case ARRAY_VFP:
            if (lpaplLongestRes)
                *lpaplLongestRes = 0;
            if (lpimmTypeRes)
                *lpimmTypeRes    = IMMTYPE_VFP;
            if (lphGlbRes)
                *lphGlbRes       = &((LPAPLVFP) lpMemSub)[uSub];
            break;

        defstop
            break;
    } // End SWITCH
} // End GetNextValueMemSub


//***************************************************************************
//  $GetGlbHandle
//
//  Return the HGLOBAL from a token if it's an HGLOBAL
//    NULL otherwise.
//***************************************************************************

HGLOBAL GetGlbHandle
    (LPTOKEN lpToken)           // Ptr to token

{
    // If it's an immediate object, ...
    if (IsTknImmed (lpToken))
        return NULL;
    else
    // If it's a named object, ...
    if (IsTknNamed (lpToken))
        return lpToken->tkData.tkSym->stData.stGlbData;
    else
        return lpToken->tkData.tkGlbData;
} // End GetGlbHandle


//***************************************************************************
//  $GetGlbPtrs
//
//  Return the HGLOBAL and LPVOID from a token if it's an HGLOBAL,
//    NULL otherwise.
//  If the arg is immediate, return the APLLONGEST value.
//***************************************************************************

APLLONGEST GetGlbPtrs
    (LPTOKEN  lpToken,          // Ptr to token
     HGLOBAL *lphGlb)           // Ptr to ptr to HGLOBAL

{
    // Get the global handle & longest w/o locking it
    return GetGlbPtrs_LOCK (lpToken, lphGlb, NULL);
} // End GetGlbPtrs


//***************************************************************************
//  $GetGlbPtrs_LOCK
//
//  Return the HGLOBAL and LPVOID from a token if it's an HGLOBAL,
//    NULL otherwise.
//  If the arg is immediate, return the APLLONGEST value.
//
//  The "_LOCK" suffix is a reminder that this function locks
//    a global memory handle and the caller must unlock it.
//***************************************************************************

APLLONGEST GetGlbPtrs_LOCK
    (LPTOKEN  lpToken,          // Ptr to token
     HGLOBAL *lphGlb,           // Ptr to ptr to HGLOBAL
     LPVOID  *lplpMem)          // Ptr to ptr to memory (may be NULL)

{
    LPVOID     lpMem;           // Ptr to locked memory header
    APLLONGEST aplLongest;      // First value in the data (unless empty)
    APLSTYPE   aplTypeMem;      // Memory storage type
    APLNELM    aplNELMMem;      // Memory NELM

    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must traverse the array
            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                *lphGlb = lpToken->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir_PTB (*lphGlb));

                // Lock the memory to get a ptr to it
                lpMem = MyGlobalLock (*lphGlb);

#define lpHeader        ((LPVARARRAY_HEADER) lpMem)
                // Get the type & NELM
                aplTypeMem = lpHeader->ArrType;
                aplNELMMem = lpHeader->NELM;
#undef  lpHeader
                break;      // Continue with common HGLOBAL code
            } // End IF

            // Handle the immediate case
            *lphGlb  = NULL;

            if (lplpMem)
                *lplpMem = NULL;

            return lpToken->tkData.tkSym->stData.stLongest;

        case TKT_FCNNAMED:
        case TKT_OP1NAMED:
        case TKT_OP2NAMED:
        case TKT_OP3NAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must traverse the array
            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                *lphGlb = lpToken->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL function array
                Assert (IsGlbTypeFcnDir_PTB (*lphGlb)
                     || IsGlbTypeDfnDir_PTB (*lphGlb));

                // Lock the memory to get a ptr to it
                lpMem = MyGlobalLock (*lphGlb);

                // Get the pseudo-type & NELM
                aplTypeMem = ARRAY_LIST;
                aplNELMMem = 0;

                break;      // Continue with common HGLOBAL code
            } // End IF

            // Handle the immediate case
            *lphGlb  = NULL;

            if (lplpMem)
                *lplpMem = NULL;

            return lpToken->tkData.tkSym->stData.stLongest;

        case TKT_FCNAFO:
        case TKT_DELAFO:
        case TKT_DELDEL:
        case TKT_OP1AFO:
        case TKT_OP2AFO:
            *lphGlb = lpToken->tkData.tkGlbData;

            // stData is a valid HGLOBAL function array
            Assert (IsGlbTypeDfnDir_PTB (*lphGlb));

            // Lock the memory to get a ptr to it
            lpMem = MyGlobalLock (*lphGlb);

            // Get the pseudo-type & NELM
            aplTypeMem = ARRAY_LIST;
            aplNELMMem = 0;

            break;      // Continue with common HGLOBAL code

        case TKT_VARIMMED:
        case TKT_AXISIMMED:
        case TKT_LSTIMMED:
        case TKT_FCNIMMED:
            *lphGlb  = NULL;

            if (lplpMem)
                *lplpMem = NULL;

            return lpToken->tkData.tkLongest;

        case TKT_FCNARRAY:
            *lphGlb = lpToken->tkData.tkGlbData;

            // Lock the memory to get a ptr to it
            lpMem = MyGlobalLock (*lphGlb);

            // Get the pseudo-type & NELM
            aplTypeMem = ARRAY_LIST;
            aplNELMMem = 0;

            break;      // Continue with common HGLOBAL code

        case TKT_CHRSTRAND:
        case TKT_NUMSTRAND:
        case TKT_NUMSCALAR:
        case TKT_VARARRAY:
        case TKT_AXISARRAY:
        case TKT_LSTARRAY:
            *lphGlb = lpToken->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (*lphGlb));

            // Lock the memory to get a ptr to it
            lpMem = MyGlobalLock (*lphGlb);

#define lpHeader        ((LPVARARRAY_HEADER) lpMem)
            // Get the type & NELM
            aplTypeMem = lpHeader->ArrType;
            aplNELMMem = lpHeader->NELM;
#undef  lpHeader
            break;      // Continue with common HGLOBAL code

        case TKT_LSTMULT:
            *lphGlb = lpToken->tkData.tkGlbData;

            // tkData is a valid HGLOBAL list array
            Assert (IsGlbTypeLstDir_PTB (*lphGlb));

            // Lock the memory to get a ptr to it
            lpMem = MyGlobalLock (*lphGlb);

#define lpHeader        ((LPLSTARRAY_HEADER) lpMem)
            // Get the type & NELM
            aplTypeMem = ARRAY_LIST;
            aplNELMMem = lpHeader->NELM;
#undef  lpHeader
            break;      // Continue with common HGLOBAL code

        defstop
            break;
    } // End SWITCH

    // If the arg is non-empty & not a list, ...
    if (!IsEmpty (aplNELMMem)
     && !IsList (aplTypeMem))
        GetFirstValueGlb (*lphGlb,          // The global memory handle
                          NULL,             // Ptr to integer (or Boolean) (may be NULL)
                          NULL,             // ...    float (may be NULL)
                          NULL,             // ...    char (may be NULL)
                         &aplLongest,       // ...    longest (may be NULL)
                          NULL,             // ...    LPSYMENTRY or HGLOBAL (may be NULL)
                          NULL,             // ...    immediate type (see IMM_TYPES) (may be NULL)
                          NULL,             // ...    array type -- ARRAY_TYPES (may be NULL)
                          TRUE);            // TRUE iff we should expand LPSYMENTRY into immediate value
    else
        aplLongest = 0;

    // If the caller wants the locked memory ptr, ...
    if (lplpMem)
        // Lock the memory to get a ptr to it
        *lplpMem = lpMem;
    else
    {
        MyGlobalUnlock (*lphGlb); lpMem = NULL;
    } // End IF/ELSE

    return aplLongest;
} // End GetGlbPtrs_LOCK


//***************************************************************************
//  $GetSteZero
//
//  Return the LPSYMENTRY corresponding to the constant zero
//***************************************************************************

LPSYMENTRY GetSteZero
    (void)

{
    // Get the STE
    return GetMemPTD ()->lphtsPTD->steZero;
} // End GetSteZero


//***************************************************************************
//  $GetSteOne
//
//  Return the LPSYMENTRY corresponding to the constant one
//***************************************************************************

LPSYMENTRY GetSteOne
    (void)

{
    // Get the STE
    return GetMemPTD ()->lphtsPTD->steOne;
} // End GetSteOne


//***************************************************************************
//  $GetSteBlank
//
//  Return the LPSYMENTRY corresponding to the constant blank
//***************************************************************************

LPSYMENTRY GetSteBlank
    (void)

{
    // Get the STE
    return GetMemPTD ()->lphtsPTD->steBlank;
} // End GetSteBlank


//***************************************************************************
//  $GetQuadCT
//
//  Get the current value of []CT
//***************************************************************************

APLFLOAT GetQuadCT
    (void)

{
    return GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_CT]->stData.stFloat;
} // End GetQuadCT


//***************************************************************************
//  $SetQuadCT
//
//  Set the current value of []CT
//***************************************************************************

void SetQuadCT
    (APLFLOAT fQuadCT)

{
    GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_CT]->stData.stFloat = fQuadCT;
} // End SetQuadCT


//***************************************************************************
//  $GetQuadDT
//
//  Get the current value of []DT
//***************************************************************************

APLCHAR GetQuadDT
    (void)

{
    return GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_DT]->stData.stChar;
} // End GetQuadDT


//***************************************************************************
//  $SetQuadDT
//
//  Set the current value of []DT
//***************************************************************************

void SetQuadDT
    (APLCHAR cQuadDT)

{
    GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_DT]->stData.stChar = cQuadDT;
} // End SetQuadDT


//***************************************************************************
//  $GetQuadIO
//
//  Get the current value of []IO
//***************************************************************************

APLBOOL GetQuadIO
    (void)

{
    return GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_IO]->stData.stBoolean;
} // End GetQuadIO


//***************************************************************************
//  $SetQuadIO
//
//  Set the current value of []IO
//***************************************************************************

void SetQuadIO
    (APLBOOL bQuadIO)

{
    GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_IO]->stData.stBoolean = bQuadIO;
} // End SetQuadIO


//***************************************************************************
//  $GetQuadPP
//
//  Get the current value of []PP
//***************************************************************************

APLUINT GetQuadPP
    (void)

{
    return min (DEF_MAX_QUADPP64, GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_PP]->stData.stInteger);
} // End GetQuadPP


//***************************************************************************
//  $SetQuadPPV
//
//  Set the current value of []PP for VFPs
//***************************************************************************

void SetQuadPPV
    (APLINT uQuadPPV)

{
    GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_PP]->stData.stInteger=
      min (DEF_MAX_QUADPPVFP, uQuadPPV);
} // End SetQuadPPV


//***************************************************************************
//  $GetQuadPPV
//
//  Get the current value of []PP for VFP
//***************************************************************************

APLUINT GetQuadPPV
    (void)

{
    return GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_PP]->stData.stInteger;
} // End GetQuadPPV


//***************************************************************************
//  $GetQuadPW
//
//  Get the current value of []PW
//***************************************************************************

APLUINT GetQuadPW
    (void)

{
    return GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_PW]->stData.stInteger;
} // End GetQuadPW


//***************************************************************************
//  $GetQuadRL
//
//  Get the current value of []RL
//***************************************************************************

APLUINT GetQuadRL
    (void)

{
    // Get the current value
    return GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_RL]->stData.stInteger;
} // End GetQuadRL


//***************************************************************************
//  $SetQuadRL
//
//  Set the current value of []RL
//***************************************************************************

void SetQuadRL
    (APLUINT uQuadRL)           // []RL

{
    // Set the new value
    GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_RL]->stData.stInteger = uQuadRL;
} // End SetQuadRL


//***************************************************************************
//  $SavePrimSpecRL
//
//  Save []RL in lpPrimSpec
//***************************************************************************

void SavePrimSpecRL
    (LPPRIMSPEC lpPrimSpec)

{
    // Save it
    lpPrimSpec->QuadRL = GetQuadRL ();
} // End SavePrimSpecRL


//***************************************************************************
//  $RestPrimSpecRL
//
//  Restore []RL from lpPrimSpec
//***************************************************************************

void RestPrimSpecRL
    (LPPRIMSPEC lpPrimSpec)

{
    // Restore it
    SetQuadRL (lpPrimSpec->QuadRL);
} // End RestPrimSpecRL


//***************************************************************************
//  $GetPrototypeFcnPtr
//
//  Return a ptr to the appropriate prototype function
//    corresponding to a give function token type.
//***************************************************************************

LPPRIMFNS GetPrototypeFcnPtr
    (LPTOKEN lptkFunc)

{
    HGLOBAL      hGlbFcn;                   // Function array global memory handle
    LPPL_YYSTYPE lpMemFcn;                  // Ptr to function array global memory
    LPPRIMFNS    lpPrimFns;                 // Ptr to result

    // Split cases based upon the token type of the function strand's first item
    switch (lptkFunc->tkFlags.TknType)
    {
        case TKT_FCNIMMED:
            // Get a ptr to the prototype function for the first symbol (a function)
            return PrimProtoFnsTab[SymTrans (lptkFunc)];

        case TKT_OP1IMMED:
        case TKT_OP2IMMED:
        case TKT_OP3IMMED:
        case TKT_OPJOTDOT:
            // Note the white lie we tell here (LPPRIMFNS) instead of
            //   (LPPRIMOPS) so we can use one function (GetPrototypeFcnPtr)
            //   for both primitive functions and operators.  When the
            //   prototype function for a primitive function is called,
            //   lptkFunc is a ptr to the primitive function token.
            //   When the prototype function for a primitive operator
            //   is called, the same parameter is a ptr to the function strand.
            // Get a ptr to the prototype function for the first symbol (an operator)
            return (LPPRIMFNS) PrimProtoOpsTab[SymTrans (lptkFunc)];

        case TKT_FCNNAMED:      // e.g. []VR{each}0{rho}{enclose}'abc'
            return NULL;

        case TKT_FCNARRAY:
            // Split cases based upon the function array signature
            switch (GetSignatureGlb (lptkFunc->tkData.tkGlbData))
            {
                case FCNARRAY_HEADER_SIGNATURE:
                    // Get the function global memory handle
                    hGlbFcn = lptkFunc->tkData.tkGlbData;

                    // Lock the memory to get a ptr to it
                    lpMemFcn = MyGlobalLock (hGlbFcn);

                    // Skip over the header to the data
                    lpMemFcn = FcnArrayBaseToData (lpMemFcn);

                    // Recurse to get the result
                    lpPrimFns = GetPrototypeFcnPtr (&lpMemFcn->tkToken);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbFcn); lpMemFcn = NULL;

                    return lpPrimFns;

                case DFN_HEADER_SIGNATURE:
                    // Get a ptr to the prototype function for the user-defined function/operator
                    return &ExecDfnGlbProto_EM_YY;

                defstop
                    return NULL;
            } // End SWITCH

        defstop
            return NULL;
    } // End SWITCH
} // End GetPrototypeFcnPtr


//***************************************************************************
//  $GetPrimFlagsPtr
//
//  Return a ptr to the PrimFlags entry
//    corresponding to a given function token type.
//***************************************************************************

LPPRIMFLAGS GetPrimFlagsPtr
    (LPTOKEN lptkFunc)

{
    HGLOBAL      hGlbFcn;                   // Function array global memory handle
    LPPL_YYSTYPE lpMemFcn;                  // Ptr to function array global memory
    LPPRIMFLAGS  lpPrimFlags;               // Ptr to result
    static PRIMFLAGS
      DfnIdentFns =
    {
        FALSE,                              // Index
        FALSE,                              // IdentElem
        FALSE,                              // bLftIdent
        FALSE,                              // bRhtIdent
        FALSE,                              // DydScalar
        FALSE,                              // MonScalar
        FALSE,                              // Alter
        FALSE,                              // AssocBool
        FALSE,                              // AssocNumb
        FALSE,                              // FastBool
        (LPPRIMOPS) ExecDfnGlbIdent_EM_YY,  // Ptr to PRIMOPS
    },
      PrimFlagsNone = {0};

    // Split cases based upon the token type of the function strand's first item
    switch (lptkFunc->tkFlags.TknType)
    {
        case TKT_FCNIMMED:
        case TKT_OP1IMMED:
        case TKT_OP2IMMED:
        case TKT_OP3IMMED:
        case TKT_OPJOTDOT:
            return &PrimFlags[SymTrans (lptkFunc)];

        case TKT_FCNNAMED:
            // If this is a direct function, ...
            if (lptkFunc->tkData.tkSym->stFlags.FcnDir)
                return &PrimFlagsNone;
            else
                DbgStop ();

        case TKT_FCNARRAY:
            // Split cases based upon the function array signature
            switch (GetSignatureGlb (lptkFunc->tkData.tkGlbData))
            {
                case FCNARRAY_HEADER_SIGNATURE:
                    // Get the function global memory handle
                    hGlbFcn = lptkFunc->tkData.tkGlbData;

                    // Lock the memory to get a ptr to it
                    lpMemFcn = MyGlobalLock (hGlbFcn);

                    // Skip over the header to the data
                    lpMemFcn = FcnArrayBaseToData (lpMemFcn);

                    // Recurse to get the result
                    lpPrimFlags = GetPrimFlagsPtr (&lpMemFcn->tkToken);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbFcn); lpMemFcn = NULL;

                    return lpPrimFlags;

                case DFN_HEADER_SIGNATURE:
                    // Get a ptr to the prototype function for the user-defined function/operator
                    return &DfnIdentFns;

                defstop
                    return NULL;
            } // End SWITCH

            break;

        case TKT_DELAFO:
        case TKT_FCNAFO:
        case TKT_OP1AFO:
        case TKT_OP2AFO:
            // Get a ptr to the prototype function for the user-defined function/operator
            return &DfnIdentFns;

        defstop
            return NULL;
    } // End SWITCH
} // End GetPrimFlagsPtr


//***************************************************************************
//  $GetImmedType
//
//  Get the immediate type from a token
//***************************************************************************

IMM_TYPES GetImmedType
    (LPTOKEN lptkArg)               // Ptr to token

{
    if (IsTknNamed (lptkArg))
        return lptkArg->tkData.tkSym->stFlags.ImmType;
    else
        return lptkArg->tkFlags.ImmType;
} // End GetImmedType


//***************************************************************************
//  $GetImmTypeGlb
//
//  Get the immediate type from a global
//***************************************************************************

IMM_TYPES GetImmTypeGlb
    (HGLOBAL hGlbArg)               // Ptr to token

{
    LPVARARRAY_HEADER lpMemArg;
    IMM_TYPES         immTypeArg;

    // Lock the memory to get a ptr to it
    lpMemArg = MyGlobalLock (hGlbArg);

    // Get the immediate type
    immTypeArg = TranslateArrayTypeToImmType (lpMemArg->ArrType);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbArg); lpMemArg = NULL;

    return immTypeArg;
} // End GetImmTypeGlb


//***************************************************************************
//  $GetSignatureGlb
//
//  Get the signature of a global object
//***************************************************************************

UINT GetSignatureGlb
    (HGLOBAL hGlbLcl)

{
    LPHEADER_SIGNATURE lpMemLcl;            // Ptr to signature global memory
    UINT               Sig;                 // The signature

    // Lock the memory to get a ptr to it
    lpMemLcl = MyGlobalLock (hGlbLcl);

    Assert (IsValidPtr (lpMemLcl, sizeof (lpMemLcl->nature)));

    // Get the signature
    Sig = lpMemLcl->nature;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbLcl); lpMemLcl = NULL;

    return Sig;
} // End GetSignatureGlb


//***************************************************************************
//  $GetSignatureGlb_PTB
//
//  Get the signature of a global object whose value is sensitive to Ptr Type Bits
//***************************************************************************

UINT GetSignatureGlb_PTB
    (LPVOID lpSymGlbLcl)

{
    LPHEADER_SIGNATURE lpMemLcl;            // Ptr to signature global memory
    UINT               Sig;                 // The signature
    HGLOBAL            hGlbLcl;             // Global memory handle
#ifdef DEBUG
    LPSYMENTRY         lpSymEntry = lpSymGlbLcl;
#else
  #define lpSymEntry    ((LPSYMENTRY) lpSymGlbLcl)
#endif

    // Split cases based upon the ptr bits
    switch (GetPtrTypeDir (lpSymGlbLcl))
    {
        case PTRTYPE_STCONST:
            // If it's a direct function, ...
            if (lpSymEntry->stFlags.FcnDir)
                // Return a pseudo-signature
                return SYSFN_HEADER_SIGNATURE;

            Assert (lpSymEntry->stFlags.Imm EQ FALSE);

            // Clear the ptr type bits
            hGlbLcl = lpSymEntry->stData.stGlbData;

            break;

        case PTRTYPE_HGLOBAL:
            // Copy the HGLOBAL
            hGlbLcl = lpSymGlbLcl;

            break;

        defstop
            break;
    } // End SWITCH

    // Lock the memory to get a ptr to it
    lpMemLcl = MyGlobalLock (hGlbLcl);

    // Get the signature
    Sig = lpMemLcl->nature;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbLcl); lpMemLcl = NULL;

    return Sig;
#ifndef DEBUG
  #undef  lpSymEntry
#endif
} // End GetSignatureGlb_PTB


//***************************************************************************
//  $GetMemPTD
//
//  Return the current lpMemPTD
//***************************************************************************

LPPERTABDATA GetMemPTD
    (void)

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory handle

    lpMemPTD = TlsGetValue (dwTlsPerTabData); // Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));
    if (!IsValidPtr (lpMemPTD, sizeof (lpMemPTD)))
        lpMemPTD = GetPerTabPtr (TabCtrl_GetCurSel (hWndTC));
    Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    return lpMemPTD;
} // End GetMemPTD


//***************************************************************************
//  End of File: getfns.c
//***************************************************************************
