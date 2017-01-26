//***************************************************************************
//  NARS2000 -- System Function -- Quad NC
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
#include "nameclass.h"


//***************************************************************************
//  $SysFnNC_EM_YY
//
//  System function:  []NC -- Name Class
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnNC_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnNC_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc APPEND_NAME_ARG);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonNC_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydNC_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnNC_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonNC_EM_YY
//
//  Monadic []NC -- Name Class
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonNC_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonNC_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeRht;           // Right arg storage type
    APLNELM           aplNELMRht,           // Right arg NELM
                      aplNELMRes,           // Result NELM
                      aplNELMCol;           // Result column NELM
    APLRANK           aplRankRht;           // Right arg Rank
    APLLONGEST        aplLongestRht;        // Right arg longest if immediate
    HGLOBAL           hGlbRht = NULL,       // Right arg global memory handle
                      hGlbRes = NULL;       // Result    ...
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg header
                      lpMemHdrRes = NULL;   // ...    result    ...
    LPAPLCHAR         lpMemDataRht,         // Ptr to right arg char data
                      lpMemDataStart;       // Ptr to start of identifier
    LPAPLINT          lpMemDataRes;         // Ptr to result integer data
    APLUINT           uRht,                 // Loop counter
                      uCol,                 // ...
                      ByteRes;              // # bytes in the result
    LPSYMENTRY        lpSymEntry;           // Ptr to SYMENTRY
    STFLAGS           stFlags;              // STE flags
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    UBOOL             bRet = TRUE;          // TRUE iff result is valid

    // The right arg may be of three forms:
    //   1.  a scalar    name  as in 'a'
    //   2.  a vector of names as in 'a' or 'a b c'
    //   3.  a matrix of names as in 3 1{rho}'abc'

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RANK ERROR
    if (IsRank3P (aplRankRht))
        goto RANK_EXIT;

    // Check for DOMAIN ERROR
    if (!IsSimple (aplTypeRht)
     || ((!IsSimpleChar (aplTypeRht))
      && !IsEmpty (aplNELMRht)))
        goto DOMAIN_EXIT;

    // Get right arg's global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // Calculate the # identifiers in the argument
    //   allowing for vector and matrix with multiple names
    bRet =
      CalcNumIDs (aplNELMRht,       // Right arg NELM
                  aplRankRht,       // Right arg rank
                  aplLongestRht,    // Right arg longest
                  TRUE,             // TRUE iff we allow multiple names in a vector
                  lpMemHdrRht,      // Ptr to right arg global memory header
                 &aplNELMRes,       // Ptr to # right arg IDs
                 &aplNELMCol);      // Ptr to # right arg cols (matrix only)
    // Note that if bRet EQ FALSE, aplNELMRes EQ 1

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_INT, aplNELMRes, 1);

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
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_INT;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Fill in the dimension
    *VarArrayBaseToDim (lpMemHdrRes) = aplNELMRes;

    // Skip over the header and dimensions to the data
    lpMemDataRes = VarArrayDataFmBase (lpMemHdrRes);

    // If we failed in CalcNumIDs, quit now
    if (!bRet)
    {
        *lpMemDataRes++ = NAMECLASS_INV;

        goto YYALLOC_EXIT;
    } // End IF

    // Calculate the name class of each element

    // Split cases based upon the right arg rank
    switch (aplRankRht)
    {
        case 0:
            // Lookup the name in the symbol table
            // SymTabLookupName sets the .ObjName enum,
            //   and the .Inuse flag
            ZeroMemory (&stFlags, sizeof (stFlags));
            lpSymEntry = SymTabLookupNameLength ((LPAPLCHAR) &aplLongestRht,
                                                 1,
                                                &stFlags);
            // If not found, return NAMECLASS_INV or NAMECLASS_AVL
            if (lpSymEntry EQ NULL)
                *lpMemDataRes++ = IsValidName ((LPAPLCHAR) &aplLongestRht, 1)
                                ? NAMECLASS_AVL : NAMECLASS_INV;
            else
                *lpMemDataRes++ = CalcNameClass (lpSymEntry);
            break;

        case 1:
            // Skip over the header and dimensions to the data
            lpMemDataRht = VarArrayDataFmBase (lpMemHdrRht);

            // Loop through the right arg looking for identifiers
            uRht = 0;
            while (TRUE)
            {
                // Skip over white space
                while (uRht < aplNELMRht && lpMemDataRht[uRht] EQ L' ')
                    uRht++;
                if (uRht < aplNELMRht)
                {
                    // Save the starting ptr
                    lpMemDataStart = &lpMemDataRht[uRht];

                    // Skip over black space
                    while (uRht < aplNELMRht && lpMemDataRht[uRht] NE L' ')
                        uRht++;
                    // Lookup the name in the symbol table
                    // SymTabLookupName sets the .ObjName enum,
                    //   and the .Inuse flag
                    ZeroMemory (&stFlags, sizeof (stFlags));
                    lpSymEntry = SymTabLookupNameLength (lpMemDataStart,
                                                        (APLU3264) (&lpMemDataRht[uRht] - lpMemDataStart),
                                                        &stFlags);
                    // If not found, return NAMECLASS_INV or NAMECLASS_AVL
                    if (lpSymEntry EQ NULL)
                        *lpMemDataRes++ = IsValidName (lpMemDataStart, (APLU3264) (&lpMemDataRht[uRht] - lpMemDataStart))
                                        ? NAMECLASS_AVL : NAMECLASS_INV;
                    else
                        *lpMemDataRes++ = CalcNameClass (lpSymEntry);
                } else
                    break;
            } // End WHILE

            break;

        case 2:
            // Skip over the header and dimensions to the data
            lpMemDataRht = VarArrayDataFmBase (lpMemHdrRht);

            for (uRht = 0; uRht < aplNELMRes; uRht++)
            {
                // Point to the start of the data
                lpMemDataStart = &lpMemDataRht[aplNELMCol * uRht];

                // Skip over leading white space
                uCol = 0;
                while (uCol < aplNELMCol && lpMemDataStart[uCol] EQ L' ')
                    uCol++;

                // Lookup the name in the symbol table
                // SymTabLookupName sets the .ObjName enum,
                //   and the .Inuse flag
                ZeroMemory (&stFlags, sizeof (stFlags));
                lpSymEntry = SymTabLookupNameLength (&lpMemDataStart[uCol],
                                                      (APLU3264) (aplNELMCol - uCol),
                                                     &stFlags);
                // If not found, return NAMECLASS_INV or NAMECLASS_AVL
                if (lpSymEntry EQ NULL)
                    *lpMemDataRes++ = IsValidName (&lpMemDataStart[uCol], (APLU3264) (aplNELMCol - uCol))
                                    ? NAMECLASS_AVL : NAMECLASS_INV;
                else
                    *lpMemDataRes++ = CalcNameClass (lpSymEntry);
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH
YYALLOC_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

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

    // We no longer need this ptr
    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnMonNC_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $IsValidName
//
//  Determine if a name is validly constructed
//
//  A name is validly constructed if
//
//   first:  QUAD | QUOTEQUAD | ALPHABETIC | UNDERBAR;
//   second: ALPHABETIC | NUMERIC | UNDERBAR | OVERBAR;
//   name:   ALPHA | OMEGA | first second;
//
//***************************************************************************

UBOOL IsValidName
    (LPAPLCHAR lpaplChar,
     APLU3264  uLen)

{
    UINT uNam;

    // Ensure non-empty
    if (uLen EQ 0)
        return FALSE;

    // Check for direct names
    if (uLen EQ 1 && IsDirectName (lpaplChar[0]))
        return TRUE;

    // Check the first char
    if (IsSysName (lpaplChar)
     || IsValid1stCharInName (lpaplChar[0]))
    {
        // Loop through the rest of the chars
        for (uNam = 1; uNam < uLen; uNam++)
        if (!IsValid2ndCharInName (lpaplChar[uNam]))
            return FALSE;
        return TRUE;
    } // End IF

    return FALSE;
} // End IsValidName


//***************************************************************************
//  $IsValid1stCharInName
//
//  Return TRUE iff the given char is valid as a first char in a name
//***************************************************************************

UBOOL IsValid1stCharInName
    (WCHAR wch)

{
    return (wch EQ UTF16_DELTA
         || wch EQ UTF16_DELTAUNDERBAR
         || wch EQ UTF16_UNDERBAR
         || (L'a' <= wch
          &&         wch <= L'z')
         || (L'A' <= wch
          &&         wch <= L'Z'));
} // End IsValid1stCharInName


//***************************************************************************
//  $IsValid2ndCharInName
//
//  Return TRUE iff the given char is valid as a second char in a name
//***************************************************************************

UBOOL IsValid2ndCharInName
    (WCHAR wch)

{
    return (IsValid1stCharInName (wch)
         || wch EQ UTF16_OVERBAR
         || (L'0' <= wch
          &&         wch <= L'9'));
} // End IsValid2ndCharInName


//***************************************************************************
//  $CalcNameClass
//
//  Calculate the name class of each element
//
//   -1 = Invalid name or unknown sysname
//    0 = Available name
//    1 = User label
//    2 = User variable
//    3 = User-defined function (any valence:  0, 1, or 2)
//    4 = User-defined operator (either valence:  1 or 2)
//    5 = System variable
//    6 = System function       (any valence:  0, 1, or 2)
// 7-20 = (Unused)
//   21 = System label
//   22 = (Unused)
//   23 = Magic function        (any valence:  0, 1, or 2)
//   24 = Magic operator        (either valence:  1 or 2)
//
//  Note that the left shifts (BIT0 <<) in <SysFnDydNL_EM_YY>
//    assume that the name class values are limited to 63.  If
//    you add nameclasses to invalidate that assumption, be sure
//    to make <SysFnDydNL_EM_YY> work, too.
//***************************************************************************

APLINT CalcNameClass
    (LPSYMENTRY lpSymEntry)

{
    // Split cases based upon the Name Type
    switch (lpSymEntry->stFlags.stNameType)
    {
        case NAMETYPE_UNK:
            return NAMECLASS_AVL;

        case NAMETYPE_VAR:
            if (lpSymEntry->stFlags.DfnLabel)
                return NAMECLASS_USRLBL;
            else
            if (lpSymEntry->stFlags.DfnSysLabel)
                return NAMECLASS_SYSLBL;
            else
            if (lpSymEntry->stFlags.ObjName EQ OBJNAME_USR)
                return NAMECLASS_USRVAR;
            else
                return NAMECLASS_SYSVAR;

        case NAMETYPE_FN0:
        case NAMETYPE_FN12:
        case NAMETYPE_TRN:
            // Check for user-defined functions
            if (lpSymEntry->stFlags.ObjName EQ OBJNAME_USR)
                return NAMECLASS_USRFCN;
            else
            // Check for Magic Functions
            if (lpSymEntry->stFlags.ObjName EQ OBJNAME_MFO)
                return NAMECLASS_MAGFCN;
            else
                return NAMECLASS_SYSFCN;

        case NAMETYPE_OP1:
        case NAMETYPE_OP2:
        case NAMETYPE_OP3:
            // Check for Magic Operators
            if (lpSymEntry->stFlags.ObjName EQ OBJNAME_MFO)
                return NAMECLASS_MAGOPR;
            else
                return NAMECLASS_USROPR;

        case NAMETYPE_LST:
        defstop
            return NAMECLASS_INV;
    } // End SWITCH
} // End CalcNameClass


//***************************************************************************
//  $SysFnDydNC_EM_YY
//
//  Dyadic []NC -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydNC_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydNC_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End SysFnDydNC_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $CalcNumIDs
//
//  Calculate the # identifiers in an arg
//***************************************************************************

UBOOL CalcNumIDs
    (APLNELM           aplNELMRht,      // Right arg NELM
     APLRANK           aplRankRht,      // Right arg rank
     APLLONGEST        aplLongestRht,   // Right arg longest
     UBOOL             bMultipleNames,  // TRUE iff we allow multiple names in
                                        //   a vector (e.g., 'a b c') or a matrix
     LPVARARRAY_HEADER lpMemHdrRht,     // Ptr to right arg global memory
     LPAPLNELM         lpaplNELMRes,    // Ptr to # right arg IDs
     LPAPLNELM         lpaplNELMCol)    // Ptr to # right arg cols (matrix only)

{
    APLUINT   uRht;                     // Loop counter
    LPAPLCHAR lpMemRht;

    // Split cases based upon the right arg rank
    switch (aplRankRht)
    {
        case 0:
            *lpaplNELMRes = (L' ' NE (APLCHAR) aplLongestRht);

            break;

        case 1:
            *lpaplNELMRes = 0;      // Initialize

            // Skip over the header and dimensions to the data
            lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

            // Loop through the right arg looking for identifiers
            uRht = 0;
            while (TRUE)
            {
                // Skip over leading white space
                while (uRht < aplNELMRht && lpMemRht[uRht] EQ L' ')
                    uRht++;
                if (uRht < aplNELMRht)
                {
                    // If multiple names in a vector are not allowed
                    //   and this is the second name, ...
                    if (!bMultipleNames
                     && IsSingleton (*lpaplNELMRes))
                        return FALSE;

                    // Count in another element in the result
                    (*lpaplNELMRes)++;

                    // Skip over black space
                    while (uRht < aplNELMRht && lpMemRht[uRht] NE L' ')
                        uRht++;
                } else
                    break;
            } // End FOR

            break;

        case 2:
            if (!bMultipleNames)
                return FALSE;
            *lpaplNELMRes = (VarArrayBaseToDim (lpMemHdrRht))[0];
            *lpaplNELMCol = (VarArrayBaseToDim (lpMemHdrRht))[1];

            break;

        defstop
            return FALSE;
    } // End IF

    return TRUE;
} // End CalcNumIDs


//***************************************************************************
//  End of File: qf_nc.c
//***************************************************************************
