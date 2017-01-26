//***************************************************************************
//  NARS2000 -- Primitive Function -- DownTack
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
//  $PrimFnDownTack_EM_YY
//
//  Primitive function for monadic and dyadic DownTack ("type" and "encode/representation")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDownTack_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDownTack_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_DOWNTACK);

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
        return PrimFnMonDownTack_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydDownTack_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimFnDownTack_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnDownTack_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic DownTack
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnDownTack_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnDownTack_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnDownTack_EM_YY,   // Ptr to primitive function routine
                                    lptkLftArg,             // Ptr to left arg token
                                    lptkFunc,               // Ptr to function token
                                    lptkRhtArg,             // Ptr to right arg token
                                    lptkAxis);              // Ptr to axis token (may be NULL)
} // End PrimProtoFnDownTack_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimIdentFnDownTack_EM_YY
//
//  Generate an identity element for the primitive function dyadic DownTack
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimIdentFnDownTack_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimIdentFnDownTack_EM_YY
    (LPTOKEN lptkRhtOrig,           // Ptr to original right arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYRes;           // Ptr to result

    // The right arg is the prototype item from
    //   the original empty arg.

    Assert (lptkRhtOrig NE NULL);
    Assert (lptkFunc    NE NULL);
    Assert (lptkRhtArg  NE NULL);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // The (left) identity element for dyadic DownTack
    //   (L {downtack} R) ("encode/representation") is
    //   0.

    // Fill in the result token
    lpYYRes->tkToken                   = tkZero;
////lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;  // Already set by tkZero
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_BOOL;  // Already set by tkZero
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already set by tkZero
////lpYYRes->tkToken.tkData.tkBoolean  = FALSE;         // Already set by tkZero
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimIdentFnDownTack_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonDownTack_EM_YY
//
//  Primitive function for monadic DownTack ("type")
//
//  This idea was taken from the APL2 IUP as shown in Brown/Jenkins
//    "APL Identity Crisis".
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonDownTack_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonDownTack_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    HGLOBAL      hGlbRht,           // Right arg global memory handle
                 hSymGlbRes;        // Result    ...
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Split cases based upon the token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, it's an HGLOBAL
            if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
            {
                hGlbRht = lptkRhtArg->tkData.tkSym->stData.stGlbData;

                break;      // Continue with HGLOBAL case
            } // End IF

            // Handle the immediate case

            lpYYRes->tkToken.tkFlags.TknType = TKT_VARIMMED;
            lpYYRes->tkToken.tkFlags.ImmType = lptkRhtArg->tkData.tkSym->stFlags.ImmType;

            // Split cases based upon the token's immediate type
            switch (lptkRhtArg->tkData.tkSym->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    lpYYRes->tkToken.tkData.tkBoolean = FALSE;

                    break;

                case IMMTYPE_INT:
                    lpYYRes->tkToken.tkData.tkInteger = 0;

                    break;

                case IMMTYPE_FLOAT:
                    lpYYRes->tkToken.tkData.tkFloat   = 0;

                    break;

                case IMMTYPE_CHAR:
                    lpYYRes->tkToken.tkData.tkChar    = L' ';

                    break;

                defstop
                    YYFree (lpYYRes); lpYYRes = NULL;

                    break;
            } // End SWITCH

            return lpYYRes;

        case TKT_VARIMMED:
            lpYYRes->tkToken.tkFlags.TknType = TKT_VARIMMED;
            lpYYRes->tkToken.tkFlags.ImmType = lptkRhtArg->tkFlags.ImmType;

            // Split cases based upon the token's immediate type
            switch (lptkRhtArg->tkFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    lpYYRes->tkToken.tkData.tkBoolean = FALSE;

                    break;

                case IMMTYPE_INT:
                    lpYYRes->tkToken.tkData.tkInteger = 0;

                    break;

                case IMMTYPE_FLOAT:
                    lpYYRes->tkToken.tkData.tkFloat   = 0;

                    break;

                case IMMTYPE_CHAR:
                    lpYYRes->tkToken.tkData.tkChar    = L' ';

                    break;

                defstop
                    YYFree (lpYYRes); lpYYRes = NULL;

                    break;
            } // End SWITCH

            return lpYYRes;

        case TKT_VARARRAY:
            hGlbRht = lptkRhtArg->tkData.tkGlbData;

            break;      // Continue with HGLOBAL case

        defstop
            break;
    } // End SWITCH

    // HGLOBAL case

    // tk/stData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir_PTB (hGlbRht));

    // Make the prototype
    hSymGlbRes =
      MakeMonPrototype_EM_PTB (hGlbRht,     // Proto arg handle
                               lptkFunc,    // Ptr to function token
                               MP_CHARS);   // CHARs allowed
    if (!hSymGlbRes)
        return NULL;

    Assert (GetPtrTypeDir (hSymGlbRes) EQ PTRTYPE_HGLOBAL);

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = hSymGlbRes;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken);

    return lpYYRes;
} // End PrimFnMonDownTack_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydDownTack_EM_YY
//
//  Primitive function for dyadic DownTack ("encode/representation")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydDownTack_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydDownTack_EM_YY
    (LPTOKEN lptkLftArg,                // Ptr to left arg token
     LPTOKEN lptkFunc,                  // Ptr to function token
     LPTOKEN lptkRhtArg,                // Ptr to right arg token
     LPTOKEN lptkAxis)                  // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeLft,           // Left arg storage type
                      aplTypeRht,           // Right ...
                      aplTypeRes;           // Result   ...
    APLNELM           aplNELMLft,           // Left arg NELM
                      aplNELMRht,           // Right ...
                      aplNELMRes;           // Result   ...
    APLRANK           aplRankLft,           // Left arg rank
                      aplRankRht,           // Right ...
                      aplRankRes;           // Result   ...
    APLDIM            aplColsLft,           // Left arg # cols
                      aplColsLft2,          // ...             , saved value
                      aplRowsLft,           // Left arg # rows
                      aplRestLft;           // Left arg product of remaining dimensions
    APLLONGEST        aplLongestLft,        // Left arg immediate value
                      aplLongestRht;        // Right ...
    HGLOBAL           hGlbLft = NULL,       // Left arg global memory handle
                      hGlbRht = NULL,       // Right ...
                      hGlbRes = NULL;       // Result   ...
    LPVARARRAY_HEADER lpMemHdrLft = NULL,   // Ptr to left arg var header
                      lpMemHdrRht = NULL,   // ...    right ...
                      lpMemHdrRes = NULL;   // ...    result       ...
    LPVOID            lpMemLft,             // Ptr to left arg global memory
                      lpMemRht,             // Ptr to right ...
                      lpMemRes;             // Ptr to result   ...
    LPAPLDIM          lpMemDimLft,          // Ptr to left arg dimensions
                      lpMemDimRht,          // Ptr to right ...
                      lpMemDimRes;          // Ptr to result   ...
    APLUINT           ByteRes,              // # bytes in the result
                      uLftCol,              // Loop counter
                      uLftRst,              // Loop counter
                      uRht;                 // Loop counter
    APLINT            iLftRow,              // Loop counter
                      uLft,                 // ...
                      uRes,                 // ...
                      aplIntRht;            // Temporary integer
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    APLRAT            aplRatRes = {0};      // Result item as RAT
    APLVFP            aplVfpRes = {0};      // ...            VFP
    ALLTYPES          atLft = {0},          // Left arg as ALLTYPES
                      atRht = {0};          // Right ...

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the attributes (Type,NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, &aplColsLft2);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for LEFT & RIGHT DOMAIN ERRORs
    if (!IsNumeric (aplTypeLft)
     || !IsNumeric (aplTypeRht))
        goto DOMAIN_EXIT;

    // Get left & right arg global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // Calc result NELM, Rank, & Type
    aplNELMRes = aplNELMLft * aplNELMRht;
    aplRankRes = aplRankLft + aplRankRht;

    // Calculate the result storage type
    aplTypeRes = aTypePromote[aplTypeLft][aplTypeRht];

    if (IsEmpty (aplNELMRes)
     && !IsGlbNum (aplTypeRes))
        aplTypeRes = ARRAY_BOOL;
    else
    {
        // Check for exceptions
        if ((IsAll2s (lpMemHdrLft) && IsSimpleInt (aplTypeRht))
         || (lpMemHdrLft EQ NULL && aplLongestLft EQ 2 && !IsGlbNum (aplTypeRht)))
            aplTypeRes = ARRAY_BOOL;
    } // End IF/ELSE
RESTART_EXCEPTION:
    // Clear atLft and atRht in case aplTypeRes changed
    ZeroMemory (&atLft, sizeof (atLft));
    ZeroMemory (&atRht, sizeof (atRht));

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
    if (hGlbLft NE NULL)
        lpMemDimLft = VarArrayBaseToDim (lpMemHdrLft);
    if (hGlbRht NE NULL)
        lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);
    lpMemDimRes = VarArrayBaseToDim (lpMemHdrRes);

    // Fill in the result's dimension
    //   by copying the left arg dimensions
    //   and then the right arg dimensions
    if (hGlbLft NE NULL)
    {
        CopyMemory (lpMemDimRes, lpMemDimLft, (APLU3264) aplRankLft * sizeof (APLDIM));

        // Skip over the copied dimensions
        lpMemDimRes += aplRankLft;

        // Calc the # rows in the left arg
        if (IsMultiRank (aplRankLft))
        {
            aplRowsLft = (VarArrayBaseToDim (lpMemHdrLft))[aplRankLft - 2];
            aplColsLft = aplColsLft2;
        } else
        {
            aplRowsLft = aplColsLft2;
            aplColsLft = 1;
        } // End IF/ELSE

        // Calc the product of remaining dimensions
        if (!IsEmpty (aplRowsLft)
         && !IsEmpty (aplColsLft))
            aplRestLft = aplNELMLft / (aplRowsLft * aplColsLft);

        // Skip over the header and dimensions to the data
        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
    } else
    {
        aplRowsLft = aplRestLft = 1;
        aplColsLft = aplColsLft2;
        lpMemLft   = &aplLongestLft;
    } // End IF/ELSE

    if (hGlbRht NE NULL)
    {
        CopyMemory (lpMemDimRes, lpMemDimRht, (APLU3264) aplRankRht * sizeof (APLDIM));

        // Skip over the copied dimensions
        lpMemDimRes += aplRankRht;

        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    } else
        lpMemRht = &aplLongestRht;

    // Check for empty result
    if (IsEmpty (aplNELMRes))
        goto YYALLOC_EXIT;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // If the result is Boolean, ...
    if (IsSimpleBool (aplTypeRes))
    {
        // Trundle through the right arg
        for (uRht = 0; uRht < aplNELMRht; uRht++)
        {
            // Get the next right arg value
            if (hGlbRht NE NULL)
                aplLongestRht = GetNextInteger (lpMemRht, aplTypeRht, uRht);

            // The left arg is treated as a three-dimensional array of shape
            //   aplRestLft aplRowsLft aplColsLft

            // Trundle through the left arg remaining dimensions & cols
            for (uLftRst = 0; uLftRst < aplRestLft; uLftRst++)
            for (uLftCol = 0; uLftCol < aplColsLft; uLftCol++)
            {
                // Initialize the right arg value
                aplIntRht = aplLongestRht;

                // Trundle through the left arg rows from back to front
                for (iLftRow = aplRowsLft - 1; iLftRow >= 0; iLftRow--)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Get left arg index
                    uLft = 1 * uLftCol + aplColsLft * (1 * iLftRow + aplRowsLft * uLftRst);

                    // Get result index
                    uRes = 1 * uRht + aplNELMRht * uLft;

                    // Calculate the result item
                    if (aplIntRht & BIT0)
                        // Save in the result
                        ((LPAPLBOOL) lpMemRes)[uRes >> LOG2NBIB] |= BIT0 << (MASKLOG2NBIB & (UINT) uRes);

                    // Subtract from the right arg item and shift right
                    aplIntRht >>= 1;

                    // If the right arg is zero, we're finished with this row
                    if (aplIntRht EQ 0)
                        break;
                } // End FOR
            } // End FOR/FOR
        } //End FOR
    } else
    // Trundle through the right arg
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    {
        // The left arg is treated as a three-dimensional array of shape
        //   aplRestLft aplRowsLft aplColsLft

        // Trundle through the left arg remaining dimensions & cols
        for (uLftRst = 0; uLftRst < aplRestLft; uLftRst++)
        for (uLftCol = 0; uLftCol < aplColsLft; uLftCol++)
        {
            APLFLOAT aplFloatRes;
            APLINT   aplIntRes;
            UBOOL    bDoneFOR = FALSE;

            // Promote the right arg to the result type
            (*aTypeActPromote[aplTypeRht][aplTypeRes])(lpMemRht, (hGlbRht EQ NULL) ? 0 : uRht, &atRht);

            // Trundle through the left arg rows from back to front
            for (iLftRow = aplRowsLft - 1; !bDoneFOR && iLftRow >= 0; iLftRow--)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (*lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Get left arg index
                uLft = 1 * uLftCol + aplColsLft * (1 * iLftRow + aplRowsLft * uLftRst);

                // Get result index
                uRes = 1 * uRht + aplNELMRht * uLft;

                // Promote the left arg to the result type
                (*aTypeActPromote[aplTypeLft][aplTypeRes])(lpMemLft, (hGlbLft EQ NULL) ? 0 : uLft, &atLft);

                __try
                {
                    // Split cases based upon the result arg storage type
                    switch (aplTypeRes)
                    {
////////////////////////case ARRAY_BOOL:                // Res = BOOL   Can't happen w/DownTack
                        case ARRAY_INT:                 // Res = INT
////////////////////////case ARRAY_APA:                 // Res = APA    Can't happen w/DownTack
                            // Calculate the result item
                            aplIntRes = AplModI (atLft.aplInteger, atRht.aplInteger);

                            // Save in the result
                            ((LPAPLINT)   lpMemRes)[uRes] = aplIntRes;

                            // If the modulus is zero, we're finished with this row
                            if (atLft.aplInteger EQ 0)
                            {
                                bDoneFOR = TRUE;        // Quit the FOR stmt

                                break;
                            } // End IF

                            // Subtract from the right arg item and shift right
                            atRht.aplInteger = (atRht.aplInteger - aplIntRes) / atLft.aplInteger;

                            break;

                        case ARRAY_FLOAT:               // Res = FLOAT
                            // Calculate the result item
                            aplFloatRes = AplModF (atLft.aplFloat, atRht.aplFloat);

                            // Save in the result
                            ((LPAPLFLOAT) lpMemRes)[uRes] = aplFloatRes;

                            // If the modulus is zero, we're finished with this row
                            if (atLft.aplFloat EQ 0)
                            {
                                bDoneFOR = TRUE;        // Quit the FOR stmt

                                break;
                            } // End IF

                            // Subtract from the right arg item
                            atRht.aplFloat = (atRht.aplFloat - aplFloatRes) / atLft.aplFloat;

                            break;

                        case ARRAY_RAT:                 // Res = RAT
                            // Calculate the result item
                            aplRatRes = AplModR (atLft.aplRat, atRht.aplRat);

                            // Save in the result
                            ((LPAPLRAT) lpMemRes)[uRes] = aplRatRes;

                            // If the modulus is zero, we're finished with this row
                            if (IsMpq0 (&atLft.aplRat))
                            {
                                bDoneFOR = TRUE;        // Quit the FOR stmt

                                break;
                            } // End IF

                            // Subtract from the right arg item
////////////////////////////atRht.aplRat = (atRht.aplRat - aplRatRes) / atLft.aplRat;
                            mpq_sub (&atRht.aplRat, &atRht.aplRat, &aplRatRes);
                            mpq_div (&atRht.aplRat, &atRht.aplRat, &atLft.aplRat);

                            break;

                        case ARRAY_VFP:                 // Res = VFP
                            // Calculate the result item
                            aplVfpRes = AplModV (atLft.aplVfp, atRht.aplVfp);

                            // Save in the result
                            ((LPAPLVFP) lpMemRes)[uRes] = aplVfpRes;

                            // If the modulus is zero, we're finished with this row
                            if (IsMpf0 (&atLft.aplVfp))
                            {
                                bDoneFOR = TRUE;        // Quit the FOR stmt

                                break;
                            } // End IF

                            // Subtract from the right arg item
////////////////////////////atRht.aplVfp = (atRht.aplVfp - aplVfpRes) / atLft.aplVfp;
                            mpfr_sub (&atRht.aplVfp, &atRht.aplVfp, &aplVfpRes   , MPFR_RNDN);
                            mpfr_div (&atRht.aplVfp, &atRht.aplVfp, &atLft.aplVfp, MPFR_RNDN);

                            break;

                        case ARRAY_CHAR:
                        case ARRAY_HETERO:
                        case ARRAY_NESTED:
                        defstop
                            break;
                    } // End SWITCH

                    // Free the old atLft
                    (*aTypeFree[aplTypeRes]) (&atLft, 0);
                } __except (CheckException (GetExceptionInformation (), L"PrimFnDydUpTack_EM_YY"))
                {
                    // Free the old atLft and atRht
                    (*aTypeFree[aplTypeRes]) (&atLft, 0);
                    (*aTypeFree[aplTypeRes]) (&atRht, 0);

                    switch (MyGetExceptionCode ())
                    {
                        case EXCEPTION_RESULT_FLOAT:
                            MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                            if (!IsSimpleFlt (aplTypeRes))
                            {
                                aplTypeRes = ARRAY_FLOAT;

                                dprintfWL0 (L"!!Restarting Exception in " APPEND_NAME L": %2d (%S#%d)", MyGetExceptionCode (), FNLN);

                                // We no longer need these ptrs
                                MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

                                // We no longer need this storage
                                FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;

                                goto RESTART_EXCEPTION;
                            } // End IF

                            // Display message for unhandled exception
                            DisplayException ();

                            break;

                        case EXCEPTION_RESULT_VFP:
                            if (!IsVfp (aplTypeRes))
                            {
                                aplTypeRes = ARRAY_VFP;

                                dprintfWL0 (L"!!Restarting Exception in " APPEND_NAME L": %2d (%S#%d)", MyGetExceptionCode (), FNLN);

                                // We no longer need these ptrs
                                MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

                                // We no longer need this storage
                                FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;

                                goto RESTART_EXCEPTION;
                            } // End IF

                            // Display message for unhandled exception
                            DisplayException ();

                            break;

                        default:
                            // Display message for unhandled exception
                            DisplayException ();

                            break;
                    } // End SWITCH
                } // End __try/__except
            } // End FOR

            // Free the old atRht
            (*aTypeFree[aplTypeRes]) (&atRht, 0);
        } // End FOR/FOR
    } // End FOR
YYALLOC_EXIT:
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

        // We no longer need this resource
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

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

    return lpYYRes;
} // End PrimFnDydDownTack_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_dtack.c
//***************************************************************************
