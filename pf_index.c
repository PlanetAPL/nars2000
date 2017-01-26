//***************************************************************************
//  NARS2000 -- Primitive Function -- Bracket Indexed Reference & Assignment
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
//  $ArrayIndexRef_EM_YY
//
//  Index reference of an array, e.g., A[L]  or  A[L1;...;Ln]
//  The form A[L] can use either (or both) Reach and Scatter index reference,
//    whereas the form A[L1;...;Ln] can use Rectangular index reference only.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexRef_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ArrayIndexRef_EM_YY
    (LPTOKEN lptkNamArg,            // Ptr to name arg token
     LPTOKEN lptkLstArg)            // Ptr to list arg token

{
    APLSTYPE          aplTypeNam,               // Name arg storage type
                      aplTypeRes,               // Result   ...
                      aplTypeLst,               // List ...
                      aplTypeSub;               // List arg item ...
    APLNELM           aplNELMNam,               // Name arg NELM
                      aplNELMLst,               // List ...
                      aplNELMNestSub,           // List arg item ...
                      aplNELMSub;               // List arg item ...
    APLRANK           aplRankNam,               // Name arg rank
                      aplRankLst,               // List ...
                      aplRankSub;               // List arg item ...
    HGLOBAL           hGlbNam = NULL,           // Name arg global memory handle
                      hGlbLst = NULL,           // List ...
                      hGlbSub = NULL,           // List arg item ...
                      hGlbRes = NULL,           // Result         ...
                      hGlbItm;                  // List subitem  ...
    LPVARARRAY_HEADER lpMemHdrNam = NULL,       // Ptr to name arg header
                      lpMemHdrRes = NULL,       // ...    result ...
                      lpMemHdrSub = NULL;       // ...    subitem  ...
    LPLSTARRAY_HEADER lpMemHdrLst = NULL;       // ...    list ...
    LPVOID            lpMemNam,                 // Ptr to name arg global memory
                      lpMemSub,                 // Ptr to list item ...
                      lpMemRes;                 // Ptr to result     ...
    APLUINT           ByteRes,                  // # bytes in the result
                      uSub;                     // Loop counter
    UBOOL             bRet = TRUE,              // TRUE iff result is valid
                      bQuadIO;                  // []IO
    LPPL_YYSTYPE      lpYYRes = NULL,           // Ptr to the result
                      lpYYItm;                  // Ptr to temporary result
    LPTOKEN           lptkFunc = lptkLstArg;    // Ptr to function token
    APLLONGEST        aplLongestNam,            // Name arg immediate value
                      aplLongestSub,            // List arg item ...
                      aplLongestItm;            // List arg subitem ...
    IMM_TYPES         immTypeItm;               // List arg subitem immediate type
    UINT              uBitMask,                 // Bit mask for when looping through Booleans
                      uBitIndex;                // Bit index ...
    LPSYMENTRY        lpSymTmp;                 // Ptr to temporary LPSYMENTRY
    TOKEN_TYPES       TknTypeLst;               // Token type of the list arg
    LPPERTABDATA      lpMemPTD;                 // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the attributes (Type, NELM, and Rank) of the name & list args
    AttrsOfToken (lptkNamArg, &aplTypeNam, &aplNELMNam, &aplRankNam, NULL);
    AttrsOfToken (lptkLstArg, &aplTypeLst, &aplNELMLst, &aplRankLst, NULL);

    // Get the token type of the list arg
    TknTypeLst = lptkLstArg->tkFlags.TknType;

    // Map APA name arg to INT result
    if (IsSimpleAPA (aplTypeNam))
        aplTypeRes = ARRAY_INT;
    else
        aplTypeRes = aplTypeNam;

    // Get name and list arg's global ptrs
    aplLongestNam = GetGlbPtrs_LOCK (lptkNamArg, &hGlbNam, &lpMemHdrNam);
                    GetGlbPtrs_LOCK (lptkLstArg, &hGlbLst, &lpMemHdrLst);

    //***************************************************************
    // If the list arg is empty, ...
    //***************************************************************
    if (IsEmpty (aplNELMLst) && TknTypeLst EQ TKT_LSTMULT)
    {
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // If the name arg is immediate, ...
        if (hGlbNam EQ NULL)
        {
            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
            lpYYRes->tkToken.tkFlags.ImmType   = TranslateArrayTypeToImmType (aplTypeNam);
////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkLongest  = aplLongestNam;
            lpYYRes->tkToken.tkCharIndex       = lptkLstArg->tkCharIndex;
        } else
        {
            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkGlbData  = CopySymGlbDirAsGlb (hGlbNam);
            lpYYRes->tkToken.tkCharIndex       = lptkLstArg->tkCharIndex;
        } // End IF/ELSE
    } else
    //***************************************************************
    // If the list arg is a singleton (no semicolons), ...
    //***************************************************************
    if (IsSingleton (aplNELMLst) || TknTypeLst NE TKT_LSTMULT)
    {
        APLSTYPE aplTypeItm;            // List arg subitem storage type
        APLNELM  aplNELMItm;            // List arg subitem NELM
        APLRANK  aplRankItm;            // List arg subitem rank

        // If it's an immediate, ...
        if (hGlbLst EQ NULL)
        {
            Assert (lptkLstArg->tkFlags.TknType EQ TKT_LSTIMMED
                 || lptkLstArg->tkFlags.TknType EQ TKT_VARIMMED
                 || lptkLstArg->tkFlags.TknType EQ TKT_AXISIMMED);

            // Set the vars for an immediate
            hGlbSub       = NULL;
            aplTypeSub    = aplTypeLst;
            aplNELMSub    = 1;
            aplRankSub    = 0;
            aplLongestSub = *GetPtrTknLongest (lptkLstArg);
            lpMemSub      = &aplLongestSub;
        } else
        {
            Assert (lptkLstArg->tkFlags.TknType EQ TKT_LSTARRAY
                 || lptkLstArg->tkFlags.TknType EQ TKT_VARARRAY
                 || lptkLstArg->tkFlags.TknType EQ TKT_AXISARRAY);

            // Set the vars for an HGLOBAL
            aplTypeSub    = aplTypeLst;
            aplNELMSub    = aplNELMLst;
            aplRankSub    = aplRankLst;

            hGlbSub       = hGlbLst;

            // Split cases based upon the token type
            switch (lptkLstArg->tkFlags.TknType)
            {
                case TKT_LSTARRAY:
                    // Lock the memory to get a ptr to it
                    lpMemHdrSub = MyGlobalLockLst (hGlbSub);

                    break;

                case TKT_VARARRAY:
                case TKT_AXISARRAY:
                    // Lock the memory to get a ptr to it
                    lpMemHdrSub = MyGlobalLockVar (hGlbSub);

                    break;

                defstop
                    break;
            } // End SWITCH
        } // End IF/ELSE

        // Handle obvious DOMAIN ERRORs
        if (IsSimpleCH (aplTypeSub)
          && !IsEmpty (aplNELMSub))
            goto DOMAIN_EXIT;

        //***************************************************************
        // Split off cases
        //***************************************************************

        //***************************************************************
        // If the list arg item is immediate, do rectangular indexing.
        // The name arg must be a vector.
        //***************************************************************
        if (hGlbSub EQ NULL)
            lpYYRes =
              ArrayIndexRefLstImm_EM_YY (hGlbNam,           // Name arg global memory handle
                                         aplTypeNam,        // Name arg storage type
                                         aplNELMNam,        // Name arg NELM
                                         aplRankNam,        // Name arg rank
                                         aplTypeSub,        // List arg storage type
                                         aplLongestSub,     // List arg immediate value
                                         aplTypeRes,        // Result storage type
                                         lpMemPTD,          // Ptr to PerTabData global memory
                                         lptkFunc);         // Ptr to function token
        else
        //***************************************************************
        // From here on the singleton list arg is a global.
        //***************************************************************

        //***************************************************************
        // If the singleton list arg item is simple numeric, do rectangular indexing.
        // The name arg must be a vector.
        //***************************************************************
        if (IsNumeric (aplTypeSub))
            lpYYRes =
              ArrayIndexRefLstSimpGlb_EM_YY (hGlbNam,       // Name arg global memory handle
                                             aplTypeNam,    // Name arg storage type
                                             aplNELMNam,    // Name arg NELM
                                             aplRankNam,    // Name arg rank
                                             hGlbSub,       // List arg global memory handle
                                             aplTypeSub,    // List arg storage type
                                             aplNELMSub,    // List arg NELM
                                             aplRankSub,    // List arg rank
                                             aplTypeRes,    // Result storage type
                                             lpMemPTD,      // Ptr to PerTabData global memory
                                             lptkFunc);     // Ptr to function token
        else
        //***************************************************************
        // If the name arg is immediate, do reach indexing (special case).
        // The list arg item must be a nested array of simple empty numeric
        //   or char vectors.
        //***************************************************************
        if (hGlbNam EQ NULL)
        {
            Assert (lptkLstArg->tkFlags.TknType EQ TKT_VARARRAY);

            lpYYRes =
              ArrayIndexRefNamImmed_EM_YY (hGlbNam,         // Name arg global memory handle
                                           lpMemHdrNam,     // Ptr to name arg global memory header
                                           aplTypeNam,      // Name arg storage type
                                           aplLongestNam,   // Name arg immediate value
                                           hGlbLst,         // List arg global memory handle
                                           lpMemHdrSub,     // Ptr to list arg item global memory header
                                           aplTypeSub,      // List arg item storage type
                                           aplNELMSub,      // List arg item NELM
                                           aplRankSub,      // List arg item rank
                                           lptkFunc);       // Ptr to function token
        } else
        {
            //***************************************************************
            // From here on the name arg is a global.
            //***************************************************************

            //***************************************************************
            // If the list arg item is a nested array of equal length simple
            //   numeric vectors, do scatter indexing, in which case
            //   the name arg rank must be the same as the length of the
            //   list arg item sub items.
            // If the list arg item is a nested array of nested vectors,
            //   do reach indexing, in which case
            //   the name arg rank must be the same as the length of the
            //   first element of each of the list arg sub items.
            //***************************************************************

            // Calculate space needed for the result
            ByteRes = CalcArraySize (aplTypeRes, aplNELMSub, aplRankSub);

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
////////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
            lpHeader->RefCnt     = 1;
            lpHeader->NELM       = aplNELMSub;
            lpHeader->Rank       = aplRankSub;
#undef  lpHeader

            Assert (lptkLstArg->tkFlags.TknType EQ TKT_VARARRAY
                 || lptkLstArg->tkFlags.TknType EQ TKT_AXISARRAY);

            // Skip over the header to the dimensions
            lpMemRes = VarArrayBaseToDim (lpMemHdrRes);
            lpMemSub = VarArrayBaseToDim (lpMemHdrSub);

            // Fill in the result dimensions
            CopyMemory (lpMemRes, lpMemSub, (APLU3264) aplRankSub * sizeof (APLDIM));

            // Skip over the dimensions to the data
            lpMemRes = VarArrayDataFmBase (lpMemHdrRes);
            lpMemSub = VarArrayDataFmBase (lpMemHdrSub);
            lpMemNam = VarArrayDataFmBase (lpMemHdrNam);

            // Validate each element of the list arg items as either reach (pick)
            //   or scatter (squad) index reference

            // Get the current value of []IO
            bQuadIO = GetQuadIO ();

            // Initialize bit index when looping through Booleans
            uBitIndex = 0;

            // Handle nested prototypes
            aplNELMNestSub = max (aplNELMSub, 1);

            // If the list is an empty char array, ...
            if (IsCharEmpty (aplTypeSub, aplNELMSub))
                goto YYALLOC_EXIT;

            // If the list is a global numeric array, ...
            if (IsGlbNum (aplTypeSub))
            {
                // Loop through the elements of the list arg
                for (uSub = 0; uSub < aplNELMSub; uSub++)
                {
                    // Get the next index as an integer using System []CT
                    aplLongestItm = ConvertToInteger_SCT (aplTypeSub, lpMemSub, uSub, &bRet);

                    // Convert to origin-0
                    aplLongestItm -= bQuadIO;

                    // Check for negative indices [-aplNELMNam, -1]
                    if (SIGN_APLLONGEST (aplLongestItm)
                     && lpMemPTD->aplCurrentFEATURE[FEATURENDX_NEGINDICES])
                        aplLongestItm += aplNELMNam;

                    // Check for INDEX ERROR
                    if (aplLongestItm >= aplNELMNam)
                        goto INDEX_EXIT;

                    // Extract the <aplLongestItm> from the (vector) name arg
                    // Split cases based upon the result storage type
                    switch (aplTypeRes)
                    {
                        case ARRAY_BOOL:
                            uBitMask = BIT0 << (MASKLOG2NBIB & (UINT) aplLongestItm);

                            *((LPAPLBOOL)   lpMemRes) |= ((uBitMask & ((LPAPLBOOL) lpMemNam)[aplLongestItm >> LOG2NBIB]) ? TRUE : FALSE) << uBitIndex;

                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;              // Start over
                                ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                            } // End IF

                            break;

                        case ARRAY_INT:
                            // Save the value in the result
                            *((LPAPLINT)    lpMemRes)++ = ((LPAPLINT)    lpMemNam)[aplLongestItm];

                            break;

                        case ARRAY_FLOAT:
                            // Save the value in the result
                            *((LPAPLFLOAT)  lpMemRes)++ = ((LPAPLFLOAT)  lpMemNam)[aplLongestItm];

                            break;

                        case ARRAY_CHAR:
                            // Save the value in the result
                            *((LPAPLCHAR)   lpMemRes)++ = ((LPAPLCHAR)   lpMemNam)[aplLongestItm];

                            break;

                        case ARRAY_HETERO:
                            // Save the value in the result
                            *((LPAPLHETERO) lpMemRes)++ = ((LPAPLHETERO) lpMemNam)[aplLongestItm];

                            break;

                        case ARRAY_NESTED:
                            // Save the value in the result
                            *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (((LPAPLNESTED) lpMemNam)[aplLongestItm]);

                            break;

                        defstop
                            break;
                    } // End SWITCH
                } // End FOR
            } else
            // Loop through the elements of the list arg
            for (uSub = 0; uSub < aplNELMNestSub; uSub++)
            // Split cases based upon the list arg item ptr type
            switch (GetPtrTypeDir (((LPAPLNESTED) lpMemSub)[uSub]))
            {
                case PTRTYPE_STCONST:       // Immediates are NELM 1, Rank 0
                    // Check for RANK ERROR
                    if (!IsVector (aplRankNam)) // Check Name rank vs. list item NELM
                        goto RANK_EXIT;

                    // Get the value & type
                    aplLongestItm = (((LPAPLHETERO) lpMemSub)[uSub])->stData.stLongest;
                    immTypeItm    = (((LPAPLHETERO) lpMemSub)[uSub])->stFlags.ImmType;

                    // Check for DOMAIN ERROR
                    if (IsImmChr (immTypeItm))
                        goto DOMAIN_EXIT;

                    // If the immediate is a float, attempt to convert it
                    if (IsImmFlt (immTypeItm))
                    {
                        // Attempt to convert the float to an integer using System []CT
                        aplLongestItm = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestItm, &bRet);
                        if (!bRet)
                            goto DOMAIN_EXIT;
                    } // End IF

                    // Convert to origin-0
                    aplLongestItm -= bQuadIO;

                    // Check for negative indices [-aplNELMNam, -1]
                    if (SIGN_APLLONGEST (aplLongestItm)
                     && lpMemPTD->aplCurrentFEATURE[FEATURENDX_NEGINDICES])
                        aplLongestItm += aplNELMNam;

                    // Check for INDEX ERROR
                    if (aplLongestItm >= aplNELMNam)
                        goto INDEX_EXIT;

                    // Extract the <aplLongestItm> from the (vector) name arg
                    // Split cases based upon the result storage type
                    switch (aplTypeRes)
                    {
                        case ARRAY_BOOL:
                            uBitMask = BIT0 << (MASKLOG2NBIB & (UINT) aplLongestItm);

                            *((LPAPLBOOL)   lpMemRes) |= ((uBitMask & ((LPAPLBOOL) lpMemNam)[aplLongestItm >> LOG2NBIB]) ? TRUE : FALSE) << uBitIndex;

                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;              // Start over
                                ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                            } // End IF

                            break;

                        case ARRAY_INT:
                            // Save the value in the result
                            *((LPAPLINT)    lpMemRes)++ = ((LPAPLINT)    lpMemNam)[aplLongestItm];

                            break;

                        case ARRAY_FLOAT:
                            // Save the value in the result
                            *((LPAPLFLOAT)  lpMemRes)++ = ((LPAPLFLOAT)  lpMemNam)[aplLongestItm];

                            break;

                        case ARRAY_CHAR:
                            // Save the value in the result
                            *((LPAPLCHAR)   lpMemRes)++ = ((LPAPLCHAR)   lpMemNam)[aplLongestItm];

                            break;

                        case ARRAY_HETERO:
                            // Save the value in the result
                            *((LPAPLHETERO) lpMemRes)++ = ((LPAPLHETERO) lpMemNam)[aplLongestItm];

                            break;

                        case ARRAY_NESTED:
                            // Save the value in the result
                            *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (((LPAPLNESTED) lpMemNam)[aplLongestItm]);

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case PTRTYPE_HGLOBAL:
                    // Get the item global memory handle
                    hGlbItm = ((LPAPLNESTED) lpMemSub)[uSub];

                    // Get the attributes (Type, NELM, and Rank) of the list arg item
                    AttrsOfGlb (hGlbItm, &aplTypeItm, &aplNELMItm, &aplRankItm, NULL);

                    // Check for RANK ERROR
                    if (IsMultiRank (aplRankItm))
                        goto RANK_EXIT;

                    // If the item is simple,
                    //   and empty, ...
                    if (IsSimple (aplTypeItm)
                     && IsEmpty (aplNELMItm))
                    {
                        // If the list arg is not a singleton, ...
                        if (!IsSingleton (aplNELMNestSub))
                            goto INDEX_EXIT;

                        // Unlock and free (and set to NULL) a global name and ptr
                        UnlFreeGlbName (hGlbRes, lpMemHdrRes);

                        Assert (hGlbNam NE NULL);

                        // The result is the name arg
                        hGlbRes = hGlbNam;

                        // Increment the refcnt
                        DbgIncrRefCntDir_PTB (hGlbNam); // EXAMPLE:  A[{enclose}{zilde}]

                        goto YYALLOC_EXIT;
                    } else
                    // If it's simple numeric and the name arg rank
                    //   matches the list arg subitem NELM,
                    //   use Scatter Index Reference; otherwise,
                    //   use Reach Index Reference
                    if (IsNumeric (aplTypeItm)
                     && aplRankNam EQ aplNELMItm)
                    {
                        TOKEN tkLstArg = {0};

                        // If the result is empty, we've validated all we need
                        if (IsEmpty (aplNELMSub))
                            goto YYALLOC_EXIT;

                        // Fill in the list arg token
                        tkLstArg.tkFlags.TknType   = TKT_VARARRAY;
////////////////////////tkLstArg.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from = {0}
////////////////////////tkLstArg.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
                        tkLstArg.tkData.tkGlbData  = MakePtrTypeGlb (hGlbItm);
                        tkLstArg.tkCharIndex       = lptkFunc->tkCharIndex;

                        // Note the args get switched between indexing and squad
                        lpYYItm =
                          PrimFnDydSquadGlb_EM_YY (&tkLstArg,           // Ptr to left arg token
                                                   &hGlbNam,            // Right arg global memory handle
                                                    NULL,               // Ptr to axis token (may be NULL)
                                                    lptkFunc,           // Ptr to function token
                                                    TRUE,               // TRUE iff we came from indexing
                                                    NULL,               // Ptr to result global memory handle
                                                    NULL,               // Ptr to set arg token
                                                    FALSE);             // TRUE iff protoyping
                        Assert (!IsPtrSuccess (lpYYItm));

                        // If valid, disclose the item as squad returns an enclosed item
                        if (lpYYItm NE NULL)
                        {
                            LPPL_YYSTYPE lpYYItm2;
                            APLSTYPE     aplTypeItm;
                            APLRANK      aplRankItm;

                            // Get the attributes (Type, NELM, and Rank) of the item
                            AttrsOfToken (&lpYYItm->tkToken, &aplTypeItm, NULL, &aplRankItm, NULL);

                            // If the item is a scalar global numeric, don't disclose as that can increment the
                            //   refcnt on a global numerc
                            if (!IsScalar (aplRankItm)
                             || !IsGlbNum (aplTypeItm))
                            {
                                lpYYItm2 = PrimFnMonRightShoe_EM_YY (lptkFunc,
                                                                    &lpYYItm->tkToken,
                                                                     NULL);
                                if (lpYYItm2 NE NULL)
                                {
                                    FreeResult (lpYYItm); YYFree (lpYYItm); lpYYItm = NULL;
                                    lpYYItm = lpYYItm2;
                                } // End IF
                            } // End IF
                        } // End IF
                    } else
                    {
                        // Note the args get switched between indexing and pick
                        lpYYItm =
                          PrimFnDydRightShoeGlbGlb_EM_YY (hGlbItm,      // Left arg global memory handle
                                                          hGlbNam,      // Right arg global memory handle
                                                          lptkFunc,     // Ptr to function token
                                                          FALSE,        // TRUE iff array assignment
                                                          ARRAY_ERROR,  // Set arg storage type
                                                          NULL,         // Set arg global memory handle/LPSYMENTRY (NULL if immediate)
                                                          0,            // Set arg immediate value
                                                          lpMemPTD);    // Ptr to PerTabData global memory
                        Assert (!IsPtrSuccess (lpYYItm));
                    } // End IF/ELSE

                    if (lpYYItm NE NULL)
                    {
                        LPVARARRAY_HEADER lpSymHdrTmp = NULL;

                        // Split cases based upon the result storage type
                        switch (aplTypeRes)
                        {
                            case ARRAY_BOOL:
                                *((LPAPLBOOL)  lpMemRes) |= lpYYItm->tkToken.tkData.tkBoolean << uBitIndex;

                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;              // Start over
                                    ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                                } // End IF

                                break;

                            case ARRAY_INT:
                                // Save the value in the result
                                *((LPAPLINT)   lpMemRes)++ = lpYYItm->tkToken.tkData.tkInteger;

                                break;

                            case ARRAY_FLOAT:
                                // Save the value in the result
                                *((LPAPLFLOAT) lpMemRes)++ = lpYYItm->tkToken.tkData.tkFloat;

                                break;

                            case ARRAY_CHAR:
                                // Save the value in the result
                                *((LPAPLCHAR)  lpMemRes)++ = lpYYItm->tkToken.tkData.tkChar;

                                break;

                            case ARRAY_HETERO:
                            case ARRAY_NESTED:
                                // Copy the squad/pick value to the result
                                if (lpYYItm->tkToken.tkFlags.TknType EQ TKT_VARIMMED)
                                {
                                    *((LPAPLNESTED) lpMemRes)++ =
                                    lpSymTmp =
                                      MakeSymEntry_EM (lpYYItm->tkToken.tkFlags.ImmType,    // Immediate type
                                                      &lpYYItm->tkToken.tkData.tkLongest,   // Ptr to immediate value
                                                       lptkFunc);                           // Ptr to function token
                                    // Check for error
                                    if (lpSymTmp EQ NULL)
                                        goto ERROR_EXIT;
                                } else
                                    *((LPAPLNESTED) lpMemRes)++ =
                                      lpYYItm->tkToken.tkData.tkGlbData;
                                break;

                            case ARRAY_RAT:
                                // Lock the memory to get a ptr to it
                                lpSymHdrTmp = MyGlobalLockVar (lpYYItm->tkToken.tkData.tkGlbData);

                                // Save the value in the result
                                *((LPAPLRAT) lpMemRes)++ = *(LPAPLRAT) VarArrayDataFmBase (lpSymHdrTmp);

                                // We no longer need this ptr
                                MyGlobalUnlock (lpYYItm->tkToken.tkData.tkGlbData); lpSymHdrTmp = NULL;

                                // We no longer need this storage
                                DbgGlobalFree (lpYYItm->tkToken.tkData.tkGlbData); lpYYItm->tkToken.tkData.tkGlbData = NULL;

                                break;

                            case ARRAY_VFP:
                                // Lock the memory to get a ptr to it
                                lpSymHdrTmp = MyGlobalLockVar (lpYYItm->tkToken.tkData.tkGlbData);

                                // Save the value in the result
                                *((LPAPLVFP) lpMemRes)++ = *(LPAPLVFP) VarArrayDataFmBase (lpSymHdrTmp);

                                // We no longer need this ptr
                                MyGlobalUnlock (lpYYItm->tkToken.tkData.tkGlbData); lpSymHdrTmp = NULL;

                                // We no longer need this storage
                                DbgGlobalFree (lpYYItm->tkToken.tkData.tkGlbData); lpYYItm->tkToken.tkData.tkGlbData = NULL;

                                break;

                            case ARRAY_APA:
                            defstop
                                break;
                        } // End SWITCH

                        YYFree (lpYYItm); lpYYItm = NULL;
                    } else
                        goto ERROR_EXIT;
                    break;

                defstop
                    break;
            } // End FOR/SWITCH
YYALLOC_EXIT:
            // Allocate a new YYRes
            lpYYRes = YYAlloc ();

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
            lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

            // Unlock the result global memory in case TypeDemote actually demotes
            if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
            {
                // We no longer need this ptr
                MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
            } // End IF
        } // End IF/ELSE/...

        // Check for error
        if (lpYYRes EQ NULL)
            goto ERROR_EXIT;

        // See if it fits into a lower (but not necessarily smaller) datatype
        TypeDemote (&lpYYRes->tkToken);
    } else
    {
        //***************************************************************
        // The list arg list has multiple elements (semicolons):  do
        //   rectangular indexing.
        // The name arg must be a matching rank array.
        //***************************************************************

        Assert (lptkLstArg->tkFlags.TknType EQ TKT_LSTARRAY
             || lptkLstArg->tkFlags.TknType EQ TKT_LSTMULT);

        lpYYRes =
          ArrayIndexRefRect_EM_YY (lptkLstArg,      // Ptr to list arg token
                                   hGlbNam,         // Name arg global memory handle
                                   aplRankNam,      // Name arg rank
                                   lpMemHdrLst,     // Ptr to list arg global memory (points to list header)
                                   aplNELMLst,      // List arg NELM
                                   lptkFunc);       // Ptr to function token
    } // End IF/ELSE/...

    goto NORMAL_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

INDEX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
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
    if (hGlbSub NE NULL && lpMemHdrSub NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbSub); lpMemHdrSub = NULL;
    } // End IF

    if (hGlbLst NE NULL && lpMemHdrLst NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLst); lpMemHdrLst = NULL;
    } // End IF

    if (hGlbNam NE NULL && lpMemHdrNam NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbNam); lpMemHdrNam = NULL;
    } // End IF

    // If the result is valid, ...
    if (lpYYRes NE NULL)
        // Change the tkSynObj
        lpYYRes->tkToken.tkSynObj = soA;

    return lpYYRes;
} // End ArrayIndexRef_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexRefLstImm_EM_YY
//
//  Array index reference for immediate list arg
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexRefLstImm_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ArrayIndexRefLstImm_EM_YY
    (HGLOBAL      hGlbNam,          // Name arg global memory handle
     APLSTYPE     aplTypeNam,       // Name arg storage type
     APLNELM      aplNELMNam,       // Name arg NELM
     APLRANK      aplRankNam,       // Name arg rank
     APLSTYPE     aplTypeLst,       // List arg storage type
     APLLONGEST   aplLongestLst,    // List arg immediate value
     APLSTYPE     aplTypeRes,       // Result storage type
     LPPERTABDATA lpMemPTD,         // Ptr to PerTabData global memory
     LPTOKEN      lptkFunc)         // Ptr to function token

{
    APLRANK           aplRankRes = 0;       // Result rank
    HGLOBAL           lpSymGlbSub,          // Name arg item global memory handle
                      hGlbRes,              // Result global memory handle
                      lpSymGlbNam;          // Ptr to name arg global numeric
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to result header
    LPVOID            lpMemRes,             // Ptr to result global memory
                      lpMemSub;             // ...    item   ....
    IMM_TYPES         immTypeNam;           // Name arg immediate type
    UBOOL             bRet,                 // TRUE iff result is valid
                      bQuadIO;              // []IO
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    APLUINT           ByteRes;              // # bytes in the result
    APLLONGEST        aplLongestNam,        // Name arg immediate value
                      aplLongestSub;        // Item ...
    IMM_TYPES         immTypeSub;           // Name arg item immediate type

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Check for RANK ERROR
    if (!IsVector (aplRankNam))
        goto RANK_EXIT;

    // Split cases based upon the list storage type
    switch (aplTypeLst)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            break;

        case ARRAY_FLOAT:
            // Attempt to convert the float to an integer using System []CT
            aplLongestLst = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestLst, &bRet);
            if (!bRet)
                goto DOMAIN_EXIT;
            break;

        case ARRAY_RAT:                 // No such thing as an immediate RAT
        case ARRAY_VFP:                 // ...                           VFP
        case ARRAY_HETERO:              // ...                           hetero
        case ARRAY_NESTED:              // ...                           nested
        defstop
            break;
    } // End SWITCH

    // Convert the index to origin-0
    aplLongestLst -= bQuadIO;

    // Check for negative indices [-aplNELMNam, -1]
    if (SIGN_APLLONGEST (aplLongestLst)
     && lpMemPTD->aplCurrentFEATURE[FEATURENDX_NEGINDICES])
        aplLongestLst += aplNELMNam;

    // Check for within range
    if (aplLongestLst >= aplNELMNam)
        goto INDEX_EXIT;

    // If the name arg is simple or global numeric, the result is immediate or a scalar global numeric
    if (IsSimpleGlbNum (aplTypeNam))
    {
        // Get the indexed value
        GetNextValueGlb (hGlbNam,           // Item global memory handle
                         aplLongestLst,     // Index into item
                        &lpSymGlbNam,       // Ptr to result global memory handle (may be NULL)
                        &aplLongestNam,     // Ptr to result immediate value (may be NULL)
                        &immTypeNam);       // Ptr to result immediate type (may be NULL)
        if (lpSymGlbNam EQ NULL)
        {
            // Allocate a new YYRes
            lpYYRes = YYAlloc ();

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
            lpYYRes->tkToken.tkFlags.ImmType   = immTypeNam;
////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkLongest  = aplLongestNam;
            lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

            goto NORMAL_EXIT;
        } // End IF

////    // Get the arg's type
////    AttrsOfGlb (lpSymGlbNam, &aplTypeRes, NULL, NULL, NULL);
    } // End IF

    // The name arg is a global

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, 1, aplRankRes);

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
    lpHeader->NELM       = 1;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // No need to fill in the result's dimension as it's a scalar

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Get the indexed value
    GetNextValueGlb (hGlbNam,           // Item global memory handle
                     aplLongestLst,     // Index into item
                    &lpSymGlbSub,       // Ptr to result global memory handle (may be NULL)
                    &aplLongestSub,     // Ptr to result immediate value (may be NULL)
                    &immTypeSub);       // Ptr to result immediate type (may be NULL)
    // Split cases based upon the immediate type
    switch (aplTypeRes)
    {
        LPVARARRAY_HEADER lpMemHdrSub = NULL;

        case ARRAY_NESTED:
        case ARRAY_HETERO:
            // If the item is an immediate as in the 2nd item in (1r2 2r3) 1, ...
            if (lpSymGlbSub EQ NULL)
            {
                *((LPAPLNESTED) lpMemRes) =
                  MakeSymEntry_EM (immTypeSub,
                                  &aplLongestSub,
                                   lptkFunc);
                // Check on error
                if (*((LPAPLNESTED) lpMemRes) EQ NULL)
                    goto WSFULL_EXIT;
            } else
                // Copy the global memory handle to the result
                *((LPAPLNESTED) lpMemRes) = CopySymGlbDir_PTB (lpSymGlbSub);
            break;

        case ARRAY_RAT:
            // Split cases based upon the ptr type of the item
            switch (GetPtrTypeDir (lpSymGlbSub))
            {
                case PTRTYPE_STCONST:
                    // Copy the global numeric value to the result
                    mpq_init_set ((LPAPLRAT) lpMemRes, (LPAPLRAT) lpSymGlbSub);

                    break;

                case PTRTYPE_HGLOBAL:
                    // Lock the memory to get a ptr to it
                    lpMemHdrSub = MyGlobalLockVar (lpSymGlbSub);

                    // Skip over the header and dimensions to the data
                    lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                    // Copy the global numeric value to the result
                    mpq_init_set ((LPAPLRAT) lpMemRes, (LPAPLRAT) lpMemSub);

                    // We no longer need this ptr
                    MyGlobalUnlock (lpSymGlbSub); lpMemHdrSub = NULL;

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_VFP:
            // Split cases based upon the ptr type of the item
            switch (GetPtrTypeDir (lpSymGlbSub))
            {
                case PTRTYPE_STCONST:
                    // Copy the global numeric value to the result
                    mpfr_init_copy ((LPAPLVFP) lpMemRes, (LPAPLVFP) lpSymGlbSub);

                    break;

                case PTRTYPE_HGLOBAL:
                    // Lock the memory to get a ptr to it
                    lpMemHdrSub = MyGlobalLockVar (lpSymGlbSub);

                    // Skip over the header and dimensions to the data
                    lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                    // Copy the global numeric value to the result
                    mpfr_init_copy ((LPAPLVFP) lpMemRes, (LPAPLVFP) lpMemSub);

                    // We no longer need this ptr
                    MyGlobalUnlock (lpSymGlbSub); lpMemHdrSub = NULL;

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        defstop
            break;
    } // End SWITCH

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

INDEX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    return lpYYRes;
} // End ArrayIndexRefLstImm_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexRefLstSimpGlb_EM_YY
//
//  Array index reference for simple global list arg
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexRefLstSimpGlb_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ArrayIndexRefLstSimpGlb_EM_YY
    (HGLOBAL      hGlbNam,                  // Name arg global memory handle
     APLSTYPE     aplTypeNam,               // Name arg storage type
     APLNELM      aplNELMNam,               // Name arg NELM
     APLRANK      aplRankNam,               // Name arg rank
     HGLOBAL      hGlbLst,                  // List arg global memory handle
     APLSTYPE     aplTypeLst,               // List arg storage type
     APLNELM      aplNELMLst,               // List arg NELM
     APLRANK      aplRankLst,               // List arg rank
     APLSTYPE     aplTypeRes,               // Result storage type
     LPPERTABDATA lpMemPTD,                 // Ptr to PerTabData global memory
     LPTOKEN      lptkFunc)                 // Ptr to function token

{
    LPSYMENTRY        lpSymGlbSub;          // Ptr to item
    HGLOBAL           hGlbRes = NULL;       // Result global memory handle
    LPVARARRAY_HEADER lpMemHdrLst = NULL,   // Ptr to list arg header
                      lpMemHdrRes = NULL,   // ...    result   ...
                      lpMemHdrNam = NULL;   // ...    name ...
    LPVOID            lpMemNam ,            // Ptr to name arg global memory
                      lpMemRes ;            // Ptr to result    ...
    LPAPLDIM          lpMemDimRes,          // Ptr to result dimensions
                      lpMemDimLst;          // ...    list   ...
    UBOOL             bRet,                 // TRUE iff result is valid
                      bQuadIO;              // []IO
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    APLUINT           ByteRes,              // # bytes in the result
                      uLst;                 // Loop counter
    UINT              uBitIndex;            // Bit index when looping through Booleans
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Check for RANK ERROR
    if (!IsVector (aplRankNam))
        goto RANK_EXIT;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMLst, aplRankLst);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLock000 (hGlbRes);
    lpMemHdrLst = MyGlobalLockVar (hGlbLst);
    lpMemHdrNam = MyGlobalLockVar (hGlbNam);

#define lpHeader        lpMemHdrRes
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND

    // If both the name and list args are the same length
    //   PVs in the same origin, ...
    if ((aplNELMNam EQ aplNELMLst)
     && (lpMemHdrNam->PV0 && lpMemHdrLst->PV0 && (bQuadIO EQ 0)
      || lpMemHdrNam->PV1 && lpMemHdrLst->PV1 && (bQuadIO EQ 1)))
    {
        // Then so is the result
        lpHeader->PV0    = lpMemHdrNam->PV0;
        lpHeader->PV1    = lpMemHdrNam->PV1;
    } // End IF

    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMLst;
    lpHeader->Rank       = aplRankLst;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemDimRes = VarArrayBaseToDim (lpMemHdrRes);
    lpMemDimLst = VarArrayBaseToDim (lpMemHdrLst);

    // Copy the dimensions to the result
    CopyMemory (lpMemDimRes, lpMemDimLst, (APLU3264) aplRankLst * sizeof (APLDIM));

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);
////lpMemLst = VarArrayDataFmBase (lpMemHdrLst);
    lpMemNam = VarArrayDataFmBase (lpMemHdrNam);

    // Handle prototypes
    if (IsEmpty (aplNELMLst)
     && IsNested (aplTypeRes))
    {
        HGLOBAL     hSymGlbProto;       // Prototype global memory handle

        // Make a prototype for the result
        hSymGlbProto =
          MakeMonPrototype_EM_PTB (*(LPAPLNESTED) lpMemNam, // Proto arg handle
                                   lptkFunc,                // Ptr to function token
                                   MP_CHARS);               // CHARs allowed
        if (hSymGlbProto EQ NULL)
            goto WSFULL_EXIT;

        // Save the value in the result
        *((LPAPLNESTED) lpMemRes) = hSymGlbProto;
    } else
    // Loop through the list arg elements
    for (uLst = uBitIndex = 0; uLst < aplNELMLst; uLst++)
    {
        APLLONGEST aplLongestNam,           // Name arg immediate value
                   aplLongestLst;           // List ...
        HGLOBAL    lpSymGlbLst;             // Ptr to global numeric

        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Get the next index from the list arg
        GetNextValueGlb (hGlbLst,           // List arg global memory handle
                         uLst,              // Index into list arg
                        &lpSymGlbLst,       // Ptr to list global memory handle (may be NULL)
                        &aplLongestLst,     // Ptr to list immediate value (may be NULL)
                         NULL);             // Ptr to list immediate type (may be NULL)
        // If the list arg item is immediate, ...
        if (lpSymGlbLst EQ NULL)
            // Point to the data
            lpSymGlbLst = &aplLongestLst;

        // Split cases based upon the list storage type
        switch (aplTypeLst)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
                bRet = TRUE;

                break;

            case ARRAY_FLOAT:
            case ARRAY_RAT:
            case ARRAY_VFP:
                // Attempt to convert the item to an integer using System []CT
                aplLongestLst = ConvertToInteger_SCT (aplTypeLst, lpSymGlbLst, 0, &bRet);

                break;

            defstop
                break;
        } // End SWITCH

        if (!bRet)
            goto DOMAIN_EXIT;

        // Convet the index to origin-0
        aplLongestLst -= bQuadIO;

        // Check for negative indices [-aplNELMNam, -1]
        if (SIGN_APLLONGEST (aplLongestLst)
         && lpMemPTD->aplCurrentFEATURE[FEATURENDX_NEGINDICES])
            aplLongestLst += aplNELMNam;

        // Check for INDEX ERROR
        if (aplLongestLst >= aplNELMNam)
            goto INDEX_EXIT;

        // Get the indexed item from the name arg
        GetNextItemGlb (hGlbNam,            // Item global memory handle
                        aplLongestLst,      // Index into item
                       &lpSymGlbSub,        // Ptr to item (may be NULL)
                       &aplLongestNam);     // Ptr to result immediate value (may be NULL)
        // Split cases based upon the result storage type
        switch (aplTypeRes)
        {
            case ARRAY_BOOL:
                *((LPAPLBOOL)  lpMemRes) |= aplLongestNam << uBitIndex;

                // Check for end-of-byte
                if (++uBitIndex EQ NBIB)
                {
                    uBitIndex = 0;              // Start over
                    ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                } // End IF

                break;

            case ARRAY_INT:
                // Save the value in the result
                *((LPAPLINT)    lpMemRes)++ = (APLINT) aplLongestNam;

                break;

            case ARRAY_FLOAT:
                // Save the value in the result
                *((LPAPLFLOAT)  lpMemRes)++ = *(LPAPLFLOAT) &aplLongestNam;

                break;

            case ARRAY_CHAR:
                // Save the value in the result
                *((LPAPLCHAR)   lpMemRes)++ = (APLCHAR) aplLongestNam;

                break;

            case ARRAY_HETERO:
            case ARRAY_NESTED:
                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (((LPAPLNESTED) lpMemNam)[aplLongestLst]);

                break;

            case ARRAY_RAT:
                mpq_init_set (((LPAPLRAT) lpMemRes)++, (LPAPLRAT) lpSymGlbSub);

                break;

            case ARRAY_VFP:
                mpfr_init_copy (((LPAPLVFP) lpMemRes)++, (LPAPLVFP) lpSymGlbSub);

                break;

            case ARRAY_APA:
            defstop
                break;
        } // End SWITCH
    } // End FOR

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

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken);

    goto NORMAL_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

INDEX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
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
    if (hGlbNam NE NULL && lpMemHdrNam NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbNam); lpMemHdrNam = NULL;
    } // End IF

    if (hGlbLst NE NULL && lpMemHdrLst NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLst); lpMemHdrLst= NULL;
    } // End IF

    return lpYYRes;
} // End ArrayIndexRefLstSimpGlb_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexRefNamImmed_EM_YY
//
//  Array index reference for immediate name arg
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexRefNamImmed_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ArrayIndexRefNamImmed_EM_YY
    (HGLOBAL           hGlbNam,             // Name arg global memory handle
     LPVARARRAY_HEADER lpMemHdrNam,         // Ptr to name arg global memory header
     APLSTYPE          aplTypeNam,          // Name arg storage type
     APLLONGEST        aplLongestNam,       // Name arg immediate value
     HGLOBAL           hGlbSub,             // Item arg global memory handle
     LPVARARRAY_HEADER lpMemHdrSub,         // Ptr to item global memory header
     APLSTYPE          aplTypeSub,          // Item arg storage type
     APLNELM           aplNELMSub,          // Item arg NELM
     APLRANK           aplRankSub,          // Item arg rank
     LPTOKEN           lptkFunc)            // Ptr to function token

{
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    APLNELM           aplNELMRes;           // Result NELM
    LPAPLDIM          lpMemDimSub,          // Ptr to item arg dimensions
                      lpMemDimRes;          // ...    result   ...
    APLUINT           ByteRes;              // # bytes in the result
    HGLOBAL           hGlbRes = NULL;       // Result global memory handle
    LPVARARRAY_HEADER lpMemHdrRes;          // Ptr to result global memory header
    LPVOID            lpMemRes,             // Ptr to result global memory
                      lpMemNam;             // ...    name arg ...
    APLUINT           uRes;                 // Loop counter
    LPAPLLIST         lpMemSub;             // Ptr to item global memory

    // Check for RANK ERROR
    if (IsSimpleGlbNum (aplTypeSub))
        goto RANK_EXIT;

    // Copy as result NELM
    aplNELMRes = aplNELMSub;

    // Handle prototypes
    aplNELMSub = max (aplNELMSub, 1);

    // Skip over the dimensions
    lpMemDimSub = VarArrayBaseToDim (lpMemHdrSub);

    // Skip over the header and dimensions to the data
    lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeNam, aplNELMRes, aplRankSub);

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
    lpHeader->ArrType    = aplTypeNam;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankSub;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemDimRes = VarArrayBaseToDim (lpMemHdrRes);

    // Copy the item arg dimensions to the result
    CopyMemory (lpMemDimRes, lpMemDimSub, (APLU3264) aplRankSub * sizeof (APLDIM));

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Loop through the items
    for (uRes = 0; uRes < aplNELMRes; uRes++)
    {
        if (!ArrayIndexValidZilde_EM (lpMemSub, uRes, NULL, lptkFunc))
            goto ERROR_EXIT;
    } // End FOR

    // Repeat the (scalar) name arg in the result

    // Split cases based upon the name arg storage type
    switch (aplTypeNam)
    {
        case ARRAY_BOOL:
            if ((APLBOOL) aplLongestNam)
                FillBitMemory (lpMemRes, aplNELMSub);
            break;

        case ARRAY_INT:
            for (uRes = 0; uRes < aplNELMRes; uRes++)
                *((LPAPLINT) lpMemRes)++ = (APLINT) aplLongestNam;
            break;

        case ARRAY_FLOAT:
            for (uRes = 0; uRes < aplNELMRes; uRes++)
                *((LPAPLFLOAT) lpMemRes)++ = *(LPAPLFLOAT) &aplLongestNam;
            break;

        case ARRAY_CHAR:
            for (uRes = 0; uRes < aplNELMRes; uRes++)
                *((LPAPLCHAR) lpMemRes)++ = (APLCHAR) aplLongestNam;
            break;

        case ARRAY_NESTED:
            // Skip over the header and dimensions to the data
            lpMemNam = VarArrayDataFmBase (lpMemHdrNam);

            // If the list arg is empty, copy the name arg prototype to the result
            if (IsEmpty (aplNELMRes))
            {
                HGLOBAL hSymGlbProto;       // Prototype global memory handle

                hSymGlbProto =
                  MakeMonPrototype_EM_PTB (*(LPAPLNESTED) lpMemNam, // Proto arg handle
                                           lptkFunc,                // Ptr to function token
                                           MP_CHARS);               // CHARs allowed
                // Check for error
                if (hSymGlbProto EQ NULL)
                    goto ERROR_EXIT;
                *((LPAPLNESTED) lpMemRes) = hSymGlbProto;
            } else
            for (uRes = 0; uRes < aplNELMSub; uRes++)
                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbInd_PTB (lpMemNam);
            break;

        case ARRAY_HETERO:
        case ARRAY_APA:
        defstop
            break;
    } // End SWITCH

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

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken);

    goto NORMAL_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
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
    return lpYYRes;
} // End ArrayIndexRefNamImmed_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexValidZilde_EM
//
//  Loop through the index arg looking for simple empty vectors
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexValidZilde_EM"
#else
#define APPEND_NAME
#endif

UBOOL ArrayIndexValidZilde_EM
    (LPVOID    lpMemLst,                    // Ptr to index list global memory
     APLUINT   uLst,                        // Index list index
     LPAPLNELM lpaplNELMZilde,              // Ptr to zilde count (may be NULL)
     LPTOKEN   lptkFunc)                    // Ptr to function token

{
    HGLOBAL           hGlbSub = NULL;       // Index arg item global memory handle
    LPVARARRAY_HEADER lpMemHdrSub = NULL;   // Ptr to list arg global memory header
    LPVOID            lpMemSub;             // Ptr to index arg item global memory
    APLSTYPE          aplTypeSub;           // Index arg item storage type
    APLNELM           aplNELMSub;           // ...            NELM
    APLRANK           aplRankSub;           // ...            rank

    // If valid, ...
    if (lpaplNELMZilde NE NULL)
        // Initialize the zilde count
        *lpaplNELMZilde = 0;

    // Check for immediate list
    if (lpMemLst EQ NULL)
        goto NORMAL_EXIT;

    // Split cases based upon the list arg item ptr type
    switch (GetPtrTypeDir (((LPAPLNESTED) lpMemLst)[uLst]))
    {
        case PTRTYPE_STCONST:       // Immediates are NELM 1, Rank 0
            // It's not a simple empty vector, so signal a RANK ERROR
            goto RANK_EXIT;

        case PTRTYPE_HGLOBAL:
            // Get the index arg item global memory handle
            hGlbSub = ((LPAPLNESTED) lpMemLst)[uLst];

            // Get the attributes (Type, NELM, and Rank) of the list arg item
            AttrsOfGlb (hGlbSub, &aplTypeSub, &aplNELMSub, &aplRankSub, NULL);

            // If valid, ...
            if (lpaplNELMZilde NE NULL)
                // Save the zilde count
                *lpaplNELMZilde = max (*lpaplNELMZilde, aplNELMSub);

            // Check for RANK ERROR
            if (IsMultiRank (aplRankSub))
                goto RANK_EXIT;

            // If it's nested, ...
            if (IsNested (aplTypeSub))
            {
                // The current object must be a vector of zildes

                // If it's empty, ...
                if (IsEmpty (aplNELMSub))
                {
                    // It must be simple
                    if (!IsSimple (aplTypeSub))
                        goto DOMAIN_EXIT;
                } else
                {
                    // Lock the memory to get a ptr to it
                    lpMemHdrSub = MyGlobalLockVar (hGlbSub);

                    // Skip over the header and dimensions to the data
                    lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                    // Ensure the left arg items are zildes
                    if (!PrimFnDydRightShoeGlbImm_EM (aplNELMSub,           // Left arg NELM
                                                      lpMemSub,             // Ptr to left arg global memory
                                                      0,                    // Left arg starting index
                                                      lptkFunc))            // Ptr to function token
                        goto ERROR_EXIT;

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbSub); lpMemHdrSub = NULL;
                } // End IF/ELSE
            } else
            {
                // Check for LENGTH ERROR
                if (!IsEmpty (aplNELMSub))
                    goto LENGTH_EXIT;

                // Check for DOMAIN ERROR
                if (!IsNumeric (aplTypeSub))
                    goto DOMAIN_EXIT;
            } // End IF/ELSE

            break;

        defstop
            break;
    } // End SWITCH
NORMAL_EXIT:
    return TRUE;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbSub NE NULL && lpMemHdrSub NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbSub); lpMemHdrSub= NULL;
    } // End IF

    return FALSE;
} // End ArrayIndexValidZilde_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexRefRect_EM_YY
//
//  Array index reference for multiple list elements (rectangular indexing)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexRefRect_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ArrayIndexRefRect_EM_YY
    (LPTOKEN           lptkLstArg,          // Ptr to list arg token
     HGLOBAL           hGlbNam,             // Name arg gobal memory handle
     APLRANK           aplRankNam,          // Name arg rank
     LPLSTARRAY_HEADER lpMemHdrLst,         // Ptr to list arg global memory header
     APLNELM           aplNELMLst,          // List arg NELM
     LPTOKEN           lptkFunc)            // Ptr to function token

{
    HGLOBAL           hGlbLstNew = NULL,    // New list arg global memory handle
                      hGlbAxis = NULL;      // Axis operator global memory handle
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    TOKEN             tkLstArg = {0},       // New list arg token
                      tkAxis = {0};         // Axis token
    LPAPLLIST         lpMemLst;             // ptr to list arg global memory data

    // Check for RANK ERROR
    if (aplRankNam NE aplNELMLst
     && !IsSingleton (aplNELMLst))
        goto RANK_EXIT;

    // Skip over the header and dimension
    lpMemLst = LstArrayBaseToData (lpMemHdrLst);

    // Fill in the temporary APV values (if any)
    FillTempApv (hGlbNam, lpMemLst, aplNELMLst);

    // Note the args get switched between indexing and squad
    lpYYRes =
      PrimFnDydSquadGlb_EM_YY (lptkLstArg,  // Ptr to Left arg token
                              &hGlbNam,     // Right arg global memory handle
                               NULL,        // Ptr to axis token (may be NULL)
                               lptkFunc,    // Ptr to function token
                               TRUE,        // TRUE iff we came from indexing
                               NULL,        // Ptr to result global memory handle
                               NULL,        // Ptr to set arg token
                               FALSE);      // TRUE iff protoyping
    Assert (!IsPtrSuccess (lpYYRes));

    goto NORMAL_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    return lpYYRes;
} // End ArrayIndexRefRect_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexSet_EM
//
//  Array index assignment, e.g., A[L]{is}R
//                            or  A[L1;...;Ln]{is}R
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexSet_EM"
#else
#define APPEND_NAME
#endif

UBOOL ArrayIndexSet_EM
    (LPTOKEN lptkNamArg,            // Ptr to name arg token
     LPTOKEN lptkLstArg,            // Ptr to list arg token
     LPTOKEN lptkRhtArg)            // Ptr to right arg token

{
    HGLOBAL      hGlbNam,           // Name arg global memory handle
                 hGlbRes = NULL;    // Result   ...
    APLNELM      aplNELMLst;        // List NELM
    APLRANK      aplRankNam;        // Name arg rank
    UBOOL        bRet = TRUE,       // TRUE iff the result is valid
                 bSysVar;           // TRUE iff indexed assignment into a SysVar
    TOKEN_TYPES  TknTypeLst;        // Token type of the list arg
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

#define lptkFunc        lptkNamArg

    // Get the attributes (Type, NELM, and Rank) of the name & list args
    AttrsOfToken (lptkNamArg, NULL,  NULL,       &aplRankNam, NULL);
    AttrsOfToken (lptkLstArg, NULL, &aplNELMLst,  NULL,       NULL);

    // Get the token type of the list arg
    TknTypeLst = lptkLstArg->tkFlags.TknType;

    // If the name arg is a system var, check for function/operator system labels
    bSysVar = (IsNameTypeVar (lptkNamArg->tkData.tkSym->stFlags.stNameType)
            && lptkNamArg->tkData.tkSym->stFlags.ObjName EQ OBJNAME_SYS);
    // If the target is a user-defined function/operator system label, ...
    if (bSysVar
     && lptkNamArg->tkData.tkSym->stFlags.DfnSysLabel)
        // Signal a SYNTAX ERROR
        goto SYNTAX_EXIT;

    // Get name arg global ptr
    hGlbNam = GetGlbHandle (lptkNamArg);

    // Split cases based upon whether or not the name
    //   var is immediate or scalar
    if (hGlbNam EQ NULL)
    {
        //***************************************************************
        // The name arg value is an immediate
        //***************************************************************
        if (!ArrayIndexSetNamImmed_EM (lptkNamArg,          // Ptr to name arg token
                                       lptkLstArg,          // Ptr to list ...
                                       lptkRhtArg,          // Ptr to right ...
                                       bSysVar,             // TRUE iff indexed assignment into a SysVar
                                       lptkFunc))           // Ptr to function ...
            goto ERROR_EXIT;
    } else
    {
        //***************************************************************
        // The name arg value is a global handle
        //***************************************************************

        //***************************************************************
        // If the list arg is empty, ...
        //***************************************************************
        if (IsEmpty (aplNELMLst) && TknTypeLst EQ TKT_LSTMULT)  // A[]{is}R
        {
            hGlbRes = ArrayIndexSetNoLst_EM (lptkNamArg,    // Ptr to name arg token
                                             lptkRhtArg,    // Ptr to right ...
                                             bSysVar,       // TRUE iff indexed assignment into a SysVar
                                             lptkFunc);     // Ptr to function ...
            // Check for error
            if (hGlbRes EQ NULL)
                goto ERROR_EXIT;
        } else
        {
            // Trundle through the index list and the right arg
            //   validating the indices and replacing the
            //   corresponding item in the name arg

            //***************************************************************
            // If the list arg is a singleton, ...
            //***************************************************************
            if (IsSingleton (aplNELMLst) || TknTypeLst NE TKT_LSTMULT)
            {
                bRet = ArrayIndexSetSingLst_EM (lptkNamArg,     // Ptr to name arg token
                                                lptkLstArg,     // Ptr to list ...
                                                lptkRhtArg,     // Ptr to right ...
                                               &hGlbRes,        // Ptr to result global memory handle
                                                bSysVar,        // TRUE iff indexed assignment into a SysVar
                                                lpMemPTD,       // Ptr to PerTabData global memory
                                                lptkFunc);      // Ptr to function token
                // Check for error
                if (!bRet)
                    goto ERROR_EXIT;
                if (hGlbRes EQ NULL)
                    goto DEMOTE_EXIT;
            } else
            //***************************************************************
            // The list arg list has multiple elements (semicolons):  do
            //   rectangular indexing.
            // The name arg must be a matching rank array.
            //***************************************************************
            {
                // Check for RANK ERROR between the name and list args
                if (aplRankNam NE aplNELMLst)
                    goto RANK_EXIT;

                if (ArrayIndexSetRect_EM (lptkNamArg,           // Ptr to name arg token
                                          lptkLstArg,           // Ptr to list ...
                                          lptkRhtArg,           // Ptr to right ...
                                          bSysVar,              // TRUE iff indexed assignment into a SysVar
                                          lptkFunc))            // Ptr to function ...
                    goto DEMOTE_EXIT;
                else
                    goto ERROR_EXIT;
            } // End IF/ELSE/...
        } // End IF/ELSE/...

        // If the handles are different, ...
        if (hGlbRes NE hGlbNam)
        {
            // Free the original global memory handle
            FreeResultGlobalVar (hGlbNam); hGlbNam = NULL;

            // Save the new global memory handle in the STE
            lptkNamArg->tkData.tkSym->stData.stGlbData = MakePtrTypeGlb (hGlbRes);
        } // End IF
    } // End IF/ELSE
DEMOTE_EXIT:
    // See if it fits into a lower (but not necessarily smaller) datatype
    if (!bSysVar)
        TypeDemote (lptkNamArg);

    goto NORMAL_EXIT;

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
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
    return bRet;
} // End ArrayIndexSet_EM
#undef  lptkFunc
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexSetNamImmed_EM
//
//  Array index assignment where the name arg is an immediate
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexSetNamImmed_EM"
#else
#define APPEND_NAME
#endif

UBOOL ArrayIndexSetNamImmed_EM
    (LPTOKEN    lptkNamArg,         // Ptr to name arg token
     LPTOKEN    lptkLstArg,         // Ptr to list ...
     LPTOKEN    lptkRhtArg,         // Ptr to right ...
     UBOOL      bSysVar,            // TRUE iff indexed assignment into a SysVar
     LPTOKEN    lptkFunc)           // Ptr to function token

{
    HGLOBAL           hGlbNam = NULL,       // Name arg global memory handle
                      hGlbLst = NULL,       // List arg global memory handle
                      hGlbSubLst = NULL,    // List arg item ...
                      hGlbRht = NULL,       // Right arg global memory handle
                      hGlbSubRht = NULL,    // Right arg item ...
                      hGlbRes = NULL;       // Result        ...
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg header
                      lpMemHdrRes = NULL,   // ...    result    ...
                      lpMemHdrLst = NULL;   // ...    list arg  ...
    LPVOID            lpMemLst,             // Ptr to list arg global memory
                      lpMemRht;             // Ptr to right arg global memory
    LPAPLNESTED       lpMemRes;             // Ptr to result        ...
    UBOOL             bRet = TRUE;          // TRUE iff the result is valid
    APLSTYPE          aplTypeRht,           // Right arg storage type
                      aplTypeLst;           // List  ...
    APLNELM           aplNELMLst,           // List arg NELM
                      aplNELMRht,           // Right arg NELM
                      aplNELMZilde;         // Zilde count for enclosing
    APLRANK           aplRankLst,           // List arg rank
                      aplRankRht;           // Right arg rank
    APLLONGEST        aplLongestRht;        // Right arg immediate value
    APLUINT           ByteRes,              // # bytes in the result
                      uRht;                 // Loop counter
    IMM_TYPES         immTypeRht;           // Right arg item immediate type
    TOKEN             tkFunc;               // Save area for function token when calling {enclose}
    LPPERTABDATA      lpMemPTD;             // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    //***************************************************************
    // The name arg value is a scalar
    //***************************************************************

    // Get the attributes (Type, NELM, and Rank) of the name, list, and right args
    AttrsOfToken (lptkNamArg,  NULL,        NULL      ,  NULL      , NULL);
    AttrsOfToken (lptkLstArg,  NULL,       &aplNELMLst, &aplRankLst, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Get list and right arg global ptrs
                    GetGlbPtrs_LOCK (lptkLstArg, &hGlbLst, &lpMemHdrLst);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // Get the name arg global handle
    hGlbNam = GetGlbHandle (lptkNamArg);

    // If the list is non-empty, ...
    if (aplNELMLst)
    {
        // Check for RANK ERROR
        if (hGlbLst EQ NULL)
            goto RANK_EXIT;

        // Get the storage type
        aplTypeLst = lpMemHdrLst->ArrType;

        // If the left arg is not nested, ...
        if (!IsNested (aplTypeLst))
            goto DOMAIN_EXIT;

        // Split cases based upon the token type
        switch (lptkLstArg->tkFlags.TknType)
        {
            case TKT_LSTIMMED:
            case TKT_LSTMULT:
                goto RANK_EXIT;

            case TKT_LSTARRAY:
            case TKT_VARARRAY:      // To handle Selective Specification
                break;

            defstop
                break;
        } // End SWITCH

        // Check for LENGTH ERROR
        switch (2 * IsSingleton (aplNELMLst) + 1 * IsSingleton (aplNELMRht))
        {
            case 2 * 0 + 1 * 0:         // Lst multiple, Rht multiple
                // Check for RANK ERROR
                if (aplRankLst NE aplRankRht)
                    goto RANK_EXIT;

                // Skip over the header and dimensions to the data
                lpMemLst = VarArrayDataFmBase (lpMemHdrLst);
                lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

                break;

            case 2 * 0 + 1 * 1:         // Lst multiple, Rht single
            case 2 * 1 + 1 * 1:         // Lst single, Rht single
                // Skip over the header and dimensions to the data
                lpMemLst = VarArrayDataFmBase (lpMemHdrLst);

                // If the right arg is not immediate, ...
                if (hGlbRht NE NULL)
                    // Skip over the header and dimensions to the data
                    lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
                else
                    lpMemRht = &aplLongestRht;

                break;

            case 2 * 1 + 1 * 0:         // Lst single, Rht multiple
                goto LENGTH_EXIT;

            defstop
                break;
        } // End SWITCH
    } else      // NAM[]{is}R
    {
        // Check for RANK ERROR between the name and right args
        if (!IsSingleton (aplNELMRht)
         && !IsScalar    (aplRankRht))
            goto LENGTH_EXIT;

        // If the right arg is not immediate, ...
        if (hGlbRht NE NULL)
            // Skip over the header and dimensions to the data
            lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
        else
            lpMemRht = &aplLongestRht;
        // An empty list is equivalent to {enclose}{zilde} which has a single {zilde} in it
        aplNELMZilde = 1;
    } // End IF/ELSE

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_NESTED, 1, 0);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Wrap the global in a scalar
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader        lpMemHdrRes
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_NESTED;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = 1;
    lpHeader->Rank       = 0;
#undef  lpHeader

    // Skip over the header to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Copy the function token in case we call {enclose}
    tkFunc = *lptkFunc;
    tkFunc.tkFlags.TknType = TKT_FCNIMMED;
    tkFunc.tkFlags.ImmType = IMMTYPE_PRIMFCN;
    tkFunc.tkData.tkChar   = UTF16_LEFTSHOE;
    tkFunc.tkSynObj        = soF;

    // Loop through the list and right arg
    for (uRht = 0; uRht < max (aplNELMLst, 1); uRht++)
    {
        APLSTYPE     aplTypeSubRht,
                     aplTypeSet;
        APLRANK      aplRankSubRht;
        APLNELM      uCnt;
        LPPL_YYSTYPE lpYYItm;

        // Get the next item of the right arg
        GetNextValueMem (lpMemRht,              // Ptr to item global memory data
                         aplTypeRht,            // Item storage type
                         aplNELMRht,            // Item NELM
                         (IsSingleton (aplNELMRht)) // If it's a singleton, ...
                         ? 0
                         : uRht,                // Index into item
                        &hGlbSubRht,            // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                        &aplLongestRht,         // Ptr to result immediate value (may be NULL)
                        &immTypeRht);           // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
        // Convert the right arg item immediate value
        aplTypeSet = TranslateImmTypeToArrayType (immTypeRht);

        // If the current item is an immediate,
        //   or a global numeric, ...
        if (hGlbSubRht EQ NULL
         || IsGlbNum (aplTypeRht))
        {
            LPSYMENTRY lpSymTmp;

            Assert (!IsImmErr (immTypeRht));

            // If it's a GlbNum, ..
            if (hGlbSubRht NE NULL)
                // Save an LPSYMENTRY in the result
                lpSymTmp =
                  MakeGlbEntry_EM (aplTypeRht,  // Entry type
                                   hGlbSubRht,  // Ptr to the value
                                   TRUE,        // TRUE iff we should initialize the target first
                                   lptkFunc);   // Ptr to function token
            else    // Rht is an immediate
                // Save an LPSYMENTRY in the result
                lpSymTmp =
                  MakeSymEntry_EM (immTypeRht,                  // Immediate type
                                  &aplLongestRht,               // Ptr to immediate value
                                   lptkFunc);                   // Ptr to function token
            // Check for error
            if (lpSymTmp EQ NULL)
                goto ERROR_EXIT;

            // Copy as right arg item
            hGlbSubRht = lpSymTmp;

            // Set the attributes
            aplTypeSubRht = aplTypeSet;
            aplRankSubRht = 0;
        } else
        {
            // Get the attributes (Type, NELM, and Rank) of the right arg item
            AttrsOfGlb (hGlbSubRht, &aplTypeSubRht, NULL, &aplRankSubRht, NULL);

            // Copy the current item
            hGlbSubRht = CopySymGlbDir_PTB (hGlbSubRht);
        } // End IF/ELSE

        // If the result is present, ...
        //   and neither a simple nor global numeric, ...
        if (lpMemRes[0] NE NULL
         && !IsSimpleGlbNum (aplTypeRht))
        {
#define lpMemLst2   ((LPAPLNESTED) lpMemLst)
////////////DisplayGlbVar (L"Name = ", MakePtrTypeGlb (hGlbRes));
////////////DisplayGlbVar (L"List = ", lpMemLst2[uRht]);
////////////DisplayGlbVar (L"Set  = ", hGlbSubRht     );

            // Set the corresponding element of the result
            // Note the args get switched between indexing and pick
            lpYYItm =
              PrimFnDydRightShoeGlbGlb_EM_YY (lpMemLst2[uRht],  // Left arg global memory handle
                                              hGlbRes,          // Right arg global memory handle
                                              lptkFunc,         // Ptr to function token
                                              TRUE,             // TRUE iff array assignment
                                              aplTypeSet,       // Set arg storage type
                                              hGlbSubRht,       // Set arg global memory handle/LPSYMENTRY (NULL if immediate)
                                              aplLongestRht,    // Set arg immediate value
                                              lpMemPTD);        // Ptr to PerTabData global memory
#undef  lpMemLst2
            if (!IsPtrSuccess (lpYYItm))
            {
                // We no longer need this storage
                FreeResultGlobalVar (lpMemRes[0]); lpMemRes[0] = NULL;
            } // End IF

            // Check for error
            if (lpYYItm EQ NULL)
                goto ERROR_EXIT;

            if (!IsPtrSuccess (lpYYItm))
                // Save the global memory handle in the result
                lpMemRes[0] = lpYYItm->tkToken.tkData.tkGlbData;
        } else
        {
            // If the list is non-empty, ...
            // Confirm that the current item in the list of indices is a vector of {zilde}s
            if (aplNELMLst
             && !ArrayIndexValidZilde_EM (lpMemLst, uRht, &aplNELMZilde, lptkFunc))
                goto ERROR_EXIT;

            // If the current right arg item isn't a simple or global numeric salar, ...
            if (!(IsSimpleGlbNum (aplTypeSubRht)
               && IsScalar (aplRankSubRht))
             && aplNELMZilde-- NE 0)        // Less one level as we enclose the right arg item already
            {
                // Enclose the last item as many times as there are {zilde}s in the index
                for (uCnt = 0; uCnt < aplNELMZilde; uCnt++)
                {
                    LPPL_YYSTYPE lpYYSub;

                    // Enclose the item
                    lpYYSub =
                      PrimFnMonLeftShoeGlb_EM_YY (hGlbSubRht,   // Right arg global memory handle
                                                  NULL,         // Ptr to axis token (may be NULL)
                                                 &tkFunc);      // Ptr to function token
                    // Free the old global memory handle
                    FreeResultGlobalVar (hGlbSubRht); hGlbSubRht = NULL;

                    // Check for error
                    if (lpYYSub EQ NULL)
                        goto ERROR_EXIT;

                    // Copy the {enclose}d item
                    hGlbSubRht = lpYYSub->tkToken.tkData.tkGlbData;

                    // YYFree the allocation
                    YYFree (lpYYSub); lpYYSub = NULL;
                } // End FOR
            } // End IF

            // If the existing global memory handle is valid, ...
            if (lpMemRes[0] NE NULL
             && GetPtrTypeDir (lpMemRes[0]) NE PTRTYPE_STCONST)
            {
                FreeResultGlobalVar (lpMemRes[0]); lpMemRes[0] = NULL;
            } // End IF

            // Save the global memory handle in the result
            lpMemRes[0] = hGlbSubRht;
        } // End IF/ELSE
    } // End FOR

    // If the result is still locked, ...
    if (lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    // If the name arg is not immediate, free it first
    if (!lptkNamArg->tkData.tkSym->stFlags.Imm)
    {
        // We no longer need this storage
        FreeResultGlobalVar (hGlbNam); hGlbNam = NULL;
    } // End IF

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_APA:
        case ARRAY_CHAR:
            lpMemRht = &aplLongestRht;

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // If the right arg is not a singleton, ...
            if (!IsSingleton (aplNELMRht))
                // Point to the last item in the right arg
                lpMemRht = &((LPAPLNESTED) lpMemRht)[aplNELMRht - 1];
            break;

        case ARRAY_RAT:
            // If the right arg is not a singleton, ...
            if (!IsSingleton (aplNELMRht))
                // Point to the last item in the right arg
                lpMemRht = &((LPAPLRAT   ) lpMemRht)[aplNELMRht - 1];
            break;

        case ARRAY_VFP:
            // If the right arg is not a singleton, ...
            if (!IsSingleton (aplNELMRht))
                // Point to the last item in the right arg
                lpMemRht = &((LPAPLVFP   ) lpMemRht)[aplNELMRht - 1];
            break;

        defstop
            break;
    } // End SWITCH

    // If the right arg is not an immediate, ...
    if (hGlbRht NE NULL)
    {
        // If this is indexed assignment into a SysVar, ...
        if (bSysVar)
        {
            ASYSVARVALIDNDX SysVarValidNdx; // Ptr to the SysVar validation routine

            // Save the address of the validation function for later use
            SysVarValidNdx = aSysVarValidNdx[lptkNamArg->tkData.tkSym->stFlags.SysVarValid];

            // Validate the one (and only) element in the right arg
            if (!(*SysVarValidNdx) (0,                  // The origin-0 index value (in case the position is important)
                                    aplTypeRht,         // Right arg storage type
                                   &aplLongestRht,      // Ptr to the right arg value
                                   &immTypeRht,         // Ptr to right arg immediate type (may be NULL)
                                    lpMemRht,           // Ptr to right arg global value
                                    lptkLstArg))        // Ptr to function token
                goto DOMAIN_EXIT;
        } // End IF

        // In case the <SysVarValidNdx> routine demoted the datatype, ...
        if (bSysVar
         && IsImmNum (immTypeRht))
        {
            // Save the new immediate type & value
            lptkNamArg->tkData.tkSym->stFlags.Imm      = TRUE;
            lptkNamArg->tkData.tkSym->stFlags.ImmType  = immTypeRht;
            lptkNamArg->tkData.tkSym->stData.stLongest = aplLongestRht;

            // We no longer need this storage
            FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;
        } else
        {
            // Mark as no longer immediate
            lptkNamArg->tkData.tkSym->stFlags.Imm      = FALSE;
            lptkNamArg->tkData.tkSym->stFlags.ImmType  = IMMTYPE_ERROR;

            // Save the new global memory handle in the STE
            lptkNamArg->tkData.tkSym->stData.stGlbData = MakePtrTypeGlb (hGlbRes);
        } // End IF/ELSE
    } else
    {
        // If this is indexed assignment into a SysVar, ...
        if (bSysVar)
        {
            ASYSVARVALIDNDX SysVarValidNdx; // Ptr to the SysVar validation routine

            // Save the address of the validation function for later use
            SysVarValidNdx = aSysVarValidNdx[lptkNamArg->tkData.tkSym->stFlags.SysVarValid];

            // Validate the one (and only) element in the right arg
            if (!(*SysVarValidNdx) (0,                  // The origin-0 index value (in case the position is important)
                                    aplTypeRht,         // Right arg storage type
                                   &aplLongestRht,      // Ptr to the right arg value
                                   &immTypeRht,         // Ptr to right arg immediate type (may be NULL)
                                    lpMemRht,           // Ptr to right arg global value
                                    lptkLstArg))        // Ptr to function token
                goto DOMAIN_EXIT;
        } // End IF

        // Save the new immediate type & value
        lptkNamArg->tkData.tkSym->stFlags.Imm      = TRUE;
        lptkNamArg->tkData.tkSym->stFlags.ImmType  = immTypeRht;
        lptkNamArg->tkData.tkSym->stData.stLongest = aplLongestRht;

        // If this is indexed assignment into a SysVar, ...
        if (bSysVar)
            // Execute the post-validation function
            (*aSysVarValidPost[lptkNamArg->tkData.tkSym->stFlags.SysVarValid]) (lptkNamArg);
    } // End IF/ELSE

    // In case we overstepped, ...
    TypeDemote (lptkNamArg);

    goto NORMAL_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
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

    if (hGlbSubRht NE NULL
     && GetPtrTypeDir (hGlbSubRht) EQ PTRTYPE_HGLOBAL)
    {
        // We no longer need this storage
        FreeResultGlobalVar (hGlbSubRht); hGlbSubRht = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    if (hGlbLst NE NULL && lpMemHdrLst NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLst); lpMemHdrLst = NULL;
    } // End IF

    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    return bRet;
} // End ArrayIndexSetNamImmed_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexSetNoLst_EM
//
//  Array assignment with an empty list arg
//  The name arg scalar case has already been taken care of
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexSetNoLst_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL ArrayIndexSetNoLst_EM
    (LPTOKEN    lptkNamArg,         // Ptr to name arg token
     LPTOKEN    lptkRhtArg,         // Ptr to right ...
     UBOOL      bSysVar,            // TRUE iff indexed assignment into a SysVar
     LPTOKEN    lptkFunc)           // Ptr to function token

{
    APLSTYPE          aplTypeNam,           // Name arg storage type
                      aplTypeRht;           // Right ...
    APLNELM           aplNELMNam,           // Name arg NELM
                      aplNELMRht;           // Right ...
    APLRANK           aplRankNam,           // Name arg rank
                      aplRankRht;           // Right ...
    HGLOBAL           hGlbNam = NULL,       // Name arg global memory handle
                      hGlbRht = NULL,       // Right arg ...
                      hGlbRes = NULL;       // Result    ...
    LPVARARRAY_HEADER lpMemHdrNam = NULL,   // Ptr to name arg header
                      lpMemHdrRht = NULL,   // ...    right arg ...
                      lpMemHdrRes = NULL;   // ...    result    ...
    LPVOID            lpMemNam,             // Ptr to name arg global memory
                      lpMemRht,             // Ptr to right arg global memory
                      lpMemRes;             // Ptr to result   ...
    APLUINT           uRht;                 // Loop counter
    APLLONGEST        aplLongestRht;        // Right arg immediate value
    HGLOBAL           hGlbSubRht;           // ...       global value
    LPPL_YYSTYPE      lpYYRes1,             // Ptr to primary result
                      lpYYRes2;             // Ptr to secondary result

    // Get the attributes (Type, NELM, and Rank) of the name and right args
    AttrsOfToken (lptkNamArg,  &aplTypeNam, &aplNELMNam, &aplRankNam, NULL);
    AttrsOfToken (lptkRhtArg,  &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Get name and right arg global ptrs
                    GetGlbPtrs_LOCK (lptkNamArg, &hGlbNam, &lpMemHdrNam);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // Singleton right args match all lists
    if (!IsSingleton (aplNELMRht))
    {
        // Check for RANK ERROR between the list and right args
        if (aplRankNam NE aplRankRht)
            goto RANK_EXIT;

        // Skip over the header to the dimensions
        lpMemNam = VarArrayBaseToDim (lpMemHdrNam);
        lpMemRht = VarArrayBaseToDim (lpMemHdrRht);

        // Check for LENGTH ERROR between the list and right args
        for (uRht = 0; uRht < aplRankRht; uRht++)
        if (*((LPAPLDIM) lpMemNam)++ NE *((LPAPLDIM) lpMemRht)++)
            goto LENGTH_EXIT;
    } else
    if (hGlbRht NE NULL)
        // Skip over the header to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    else
        lpMemRht = &aplLongestRht;

    // If this is indexed assignment into a SysVar, ...
    if (bSysVar)
    {
        ASYSVARVALIDNDX SysVarValidNdx;     // Ptr to the SysVar validation routine
        UINT            uBitMask;           // Mask for looping through Booleans

        // Save the address of the validation function for later use
        SysVarValidNdx = aSysVarValidNdx[lptkNamArg->tkData.tkSym->stFlags.SysVarValid];

        // Copy the name arg
        hGlbRes = CopyGlbAsType_EM (hGlbNam, aplTypeNam, lptkNamArg);

        // Check for error
        if (hGlbRes EQ NULL)
            goto ERROR_EXIT;

        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLockVar (hGlbRes);

        // Skip over the header to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

        // Loop through the elements in the right arg
        for (uRht = 0; uRht < aplNELMNam; uRht++)
        {
            // If there are more right arg elements, ...
            if (uRht < aplNELMRht)
                // Get the next right arg value
                GetNextValueMem (lpMemRht,          // Ptr to item global memory data
                                 aplTypeRht,        // Item storage type
                                 aplNELMRht,        // Item NELM
                                 uRht,              // Index into item
                                &hGlbSubRht,        // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                                &aplLongestRht,     // Ptr to result immediate value (may be NULL)
                                 NULL);             // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
            // Validate all elements in the right arg
            if (!(*SysVarValidNdx) (uRht,               // The origin-0 index value (in case the position is important)
                                    aplTypeRht,         // Right arg storage type
                                   &aplLongestRht,      // Ptr to the right arg value
                                    NULL,               // Ptr to right arg immediate type (may be NULL)
                                    hGlbSubRht,         // Ptr to right arg global value
                                    lptkFunc))          // Ptr to function token
                goto DOMAIN_EXIT;
            // Split cases based upon the name arg storage type
            switch (aplTypeNam)
            {
                case ARRAY_BOOL:
                    // Calculate the bit mask
                    uBitMask = BIT0 << (MASKLOG2NBIB & (UINT) uRht);

                    // Save the value in the result
                    if (aplLongestRht)
                        ((LPAPLBOOL) lpMemRes)[uRht >> LOG2NBIB] |=  uBitMask;
                    else
                        ((LPAPLBOOL) lpMemRes)[uRht >> LOG2NBIB] &= ~uBitMask;
                    break;

                case ARRAY_INT:
                    // Save the value in the result
                    *((LPAPLINT) lpMemRes)++ = (APLINT) aplLongestRht;

                    break;

                case ARRAY_FLOAT:
                    // Save the value in the result
                    *((LPAPLFLOAT) lpMemRes)++ = (APLFLOAT) (APLINT) aplLongestRht;

                    break;

                case ARRAY_CHAR:
                    // Save the value in the result
                    *((LPAPLCHAR) lpMemRes)++ = (APLCHAR) aplLongestRht;

                    break;

                defstop
                    break;
            } // End SWITCH
        } // End FOR

        // Execute the post-validation function
        (*aSysVarValidPost[lptkNamArg->tkData.tkSym->stFlags.SysVarValid]) (lptkNamArg);

        goto NORMAL_EXIT;
    } // End IF

    // Replace all elements of the name arg with those of the right arg

    // Find the shape of the name arg
    lpYYRes1 = PrimFnMonRho_EM_YY (lptkFunc,                // Ptr to function token
                                   lptkNamArg,              // Ptr to right arg token
                                   NULL);                   // Ptr to axis token
    // Check for error
    if (lpYYRes1 EQ NULL)
        goto ERROR_EXIT;

    // Reshape the right arg to the shape of the name arg
    lpYYRes2 = PrimFnDydRho_EM_YY (&lpYYRes1->tkToken,      // Ptr to left arg token
                                    lptkFunc,               // Ptr to function token
                                    lptkRhtArg,             // Ptr to right arg token
                                    NULL);                  // Ptr to axis token
    // Free the first result
    FreeResult (lpYYRes1); YYFree (lpYYRes1); lpYYRes1 = NULL;

    // Check for error
    if (lpYYRes2 EQ NULL)
        goto ERROR_EXIT;

    // Get the global memory handle
    hGlbRes = lpYYRes2->tkToken.tkData.tkGlbData;

    // Free the second result
    YYFree (lpYYRes2); lpYYRes2 = NULL;

    goto NORMAL_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
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

    if (hGlbNam NE NULL && lpMemHdrNam NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbNam); lpMemHdrNam = NULL;
    } // End IF

    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    return hGlbRes;
} // End ArrayIndexSetNoLst_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexSetSingLst_EM
//
//  Array assignment with a singleton list arg
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexSetSingLst_EM"
#else
#define APPEND_NAME
#endif

UBOOL ArrayIndexSetSingLst_EM
    (LPTOKEN      lptkNamArg,                   // Ptr to name arg token
     LPTOKEN      lptkLstArg,                   // Ptr to list ...
     LPTOKEN      lptkRhtArg,                   // Ptr to right ...
     HGLOBAL     *lphGlbRes,                    // Ptr to result global memory handle
     UBOOL        bSysVar,                      // TRUE iff indexed assignment into a SysVar
     LPPERTABDATA lpMemPTD,                     // Ptr to PerTabData global memory
     LPTOKEN      lptkFunc)                     // Ptr to function token

{
    HGLOBAL           hGlbNam = NULL,           // Name arg global memory handle
                      hGlbLst = NULL,           // List ...
                      hGlbSubLst = NULL,        // List arg item global memory handle
                      hGlbRht = NULL,           // Right arg ...
                      hGlbSubRht = NULL;        // Right arg item...
    LPVARARRAY_HEADER lpMemHdrRes = NULL,       // Ptr to result header
                      lpMemHdrRht = NULL,       // ...    right ...
                      lpMemHdrNam = NULL,       // ...    name  ...
                      lpMemHdrSubLst = NULL;    // ...    sublist ...
    LPLSTARRAY_HEADER lpMemHdrLst = NULL;       // ...    list  ...
    LPVOID            lpMemNam,                 // Ptr to name arg global memory
                      lpMemSubLst,              // Ptr to list arg item global memory
                      lpMemRht,                 // Ptr to right arg global memory
                      lpMemRes;                 // Ptr to result   ...
    LPAPLDIM          lpMemDimRes,              // Ptr to result dimensions
                      lpMemDimNam;              // ...    name arg ...
    APLSTYPE          aplTypeNam,               // Name arg storage type
                      aplTypeSubLst,            // List arg subitem ...
                      aplTypeRht,               // Right ...
                      aplTypeRes;               // Result    ...
    APLNELM           aplNELMNam,               // Name arg NELM
                      aplNELMLst,               // List ...
                      aplNELMSubLst,            // List arg subitem ...
                      aplNELMRht;               // Right ...
    APLRANK           aplRankNam,               // Name arg rank
                      aplRankSubLst,            // List arg subitem ...
                      aplRankRht;               // Right ...
    APLLONGEST        aplLongestSubLst,         // List arg subitem immediate value
                      aplLongestRht;            // Right arg item   ...
    IMM_TYPES         immTypeRht;               // Right ...
    APLUINT           uRht,                     // Loop counter
                      uRes;                     // Loop counter
    APLINT            apaOffNam,                // Name arg APA offset
                      apaMulNam;                // ...          multiplier
    UINT              uBitMask,                 // Bit mask for when looping through Booleans
                      bRet = TRUE;              // TRUE iff result is valid
    UBOOL             bQuadIO;                  // []IO
    ASYSVARVALIDNDX   SysVarValidNdx;           // Ptr to the SysVar validation routine
    ASYSVARVALIDPOST  SysVarValidPost;          // Ptr to the SysVar post-validation routine
    LPSYMENTRY        lpSymTmp;                 // Ptr to temporary LPSYMENTRY
    ALLTYPES          atRht = {0};              // Set arg as ALLTYPES

    // If this is indexed assignment into a SysVar, ...
    if (bSysVar)
    {
        // Save the address of the validation functions for later use
        SysVarValidNdx  = aSysVarValidNdx [lptkNamArg->tkData.tkSym->stFlags.SysVarValid];
        SysVarValidPost = aSysVarValidPost[lptkNamArg->tkData.tkSym->stFlags.SysVarValid];
    } // End IF

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Get the attributes (Type, NELM, and Rank) of the name, list, and right args
    AttrsOfToken (lptkNamArg, &aplTypeNam, &aplNELMNam, &aplRankNam, NULL);
    AttrsOfToken (lptkLstArg,  NULL,       &aplNELMLst,  NULL,       NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Get name, list, and right arg global ptrs
                    GetGlbPtrs_LOCK (lptkNamArg, &hGlbNam, &lpMemHdrNam);
                    GetGlbPtrs_LOCK (lptkLstArg, &hGlbLst, &lpMemHdrLst);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

////DisplayGlbVar (L"hGlbNam = ", hGlbNam);
////DisplayGlbVar (L"hGlbLst = ", hGlbLst);
////DisplayGlbVar (L"hGlbRht = ", hGlbRht);

    // Split cases based upon the list token type
    switch (lptkLstArg->tkFlags.TknType)
    {
        case TKT_LSTIMMED:
        case TKT_VARIMMED:
            // Set the vars for an immediate
            hGlbSubLst = NULL;
            aplTypeSubLst = TranslateImmTypeToArrayType (lptkLstArg->tkFlags.ImmType);
            aplNELMSubLst = 1;
            aplRankSubLst = 0;
            aplLongestSubLst = *GetPtrTknLongest (lptkLstArg);

            break;

        case TKT_LSTARRAY:
        case TKT_VARARRAY:              // To handle Selective Specification
            // Set the vars for an HGLOBAL
            hGlbSubLst = lptkLstArg->tkData.tkGlbData;
            AttrsOfGlb (hGlbSubLst, &aplTypeSubLst, &aplNELMSubLst, &aplRankSubLst, NULL);
            lpMemHdrSubLst = MyGlobalLockVar (hGlbSubLst);

            break;

        case TKT_LSTMULT:
        defstop
            break;
    } // End SWITCH

    // Assume no result
    *lphGlbRes = NULL;

    // Check for RANK ERROR between the list and right args
    if (!IsSingleton (aplNELMRht)
     && aplRankSubLst NE aplRankRht
     && aplNELMSubLst NE aplNELMRht)
        goto RANK_EXIT;

    // If the list is not an immediate, ...
    if (lpMemHdrSubLst NE NULL)
        // Skip over the header to the dimensions
        lpMemSubLst = VarArrayBaseToDim (lpMemHdrSubLst);
    else
        // Point to the data
        lpMemSubLst = &aplLongestSubLst;

    // Check for LENGTH ERROR between the list and right args
    if (!IsSingleton (aplNELMRht))
    {
        APLRANK aplRankMax;

        // Use the larger rank
        aplRankMax = max (aplRankSubLst, aplRankRht);

        // Skip over the header to the dimensions
        lpMemRht = VarArrayBaseToDim (lpMemHdrRht);

        for (uRht = 0; uRht < aplRankMax; uRht++)
        {
            APLDIM aplDimSubLst,        // Temporary dimension value
                   aplDimRht;           // ...

            // Get the respective dimensions
            if (uRht < aplRankSubLst)
                aplDimSubLst = *((LPAPLDIM) lpMemSubLst)++;
            else
                aplDimSubLst = 1;
            if (uRht < aplRankRht)
                aplDimRht    = *((LPAPLDIM) lpMemRht)++;
            else
                aplDimRht    = 1;

            // Skip over equal or unit dimensions
            if (aplDimSubLst EQ aplDimRht
             ||  IsUnitDim (aplDimSubLst) && !IsUnitDim (aplDimRht)
             || !IsUnitDim (aplDimSubLst) &&  IsUnitDim (aplDimRht))
                continue;

            // If the ranks differ, it's a RANK ERROR
            //   otherwise, it's a LENGTH ERROR
            if (aplRankSubLst NE aplRankRht)
                goto RANK_EXIT;
            else
                goto LENGTH_EXIT;
        } // End FOR
    } else
    {
        // If the list is not an immediate, ...
        if (lpMemHdrSubLst NE NULL)
            // Skip over the dimensions to the data
            lpMemSubLst = VarArrayDataFmBase (lpMemHdrSubLst);
        else
            // Point to the data
            lpMemSubLst = &aplLongestSubLst;

        if (hGlbRht NE NULL)
            // Skip over the header and dimensions to the data
            lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
        else
            lpMemRht = &aplLongestRht;
    } // End IF/ELSE

    //***************************************************************
    // From here on, lpMemSubLst points to the data, and lpMemRht
    //   points to its data
    //***************************************************************

    // Handle obvious DOMAIN ERRORs
    if (IsSimpleHet (aplTypeSubLst)
     || (IsSimpleChar (aplTypeSubLst)
      && !IsEmpty (aplNELMSubLst)))
        goto DOMAIN_EXIT;

    // If the list is empty, there's no assignment
    if (IsEmpty (aplNELMSubLst))
        goto NORMAL_EXIT;

    // If the name arg is an APA, ...
    if (IsSimpleAPA (aplTypeNam))
    {
#define lpAPA       ((LPAPLAPA) (VarArrayDataFmBase (lpMemHdrNam)))
        // Get APA parameters
        apaOffNam = lpAPA->Off;
        apaMulNam = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // Handle Boolean-valued APA vs. Boolean
    if (IsSimpleAPA (aplTypeNam)
     && IsSimpleBool (aplTypeRht)
     && apaMulNam EQ 0
     && IsBooleanValue (apaOffNam))
        // The result is also Boolean
        aplTypeRes = ARRAY_BOOL;
    else
        // Calculate the storage type of the result
        aplTypeRes = aTypePromote[aplTypeNam][aplTypeRht];

    //***************************************************************
    // Split off cases
    //***************************************************************

    //***************************************************************
    // If the list arg item is immediate, do rectangular indexing.
    // The name arg must be a vector.
    //***************************************************************
    if (hGlbSubLst EQ NULL)
    {
        // If the name arg is a scalar, ...
        if (IsScalar (aplRankNam))
            goto INDEX_EXIT;

        // Skip over the header to the dimensions
        lpMemDimNam = VarArrayBaseToDim (lpMemHdrNam);

        // Convert to origin-0
        aplLongestSubLst -= bQuadIO;

        // Check for negative indices [-*lpMemDimNam, -1]
        if (SIGN_APLLONGEST (aplLongestSubLst)
         && lpMemPTD->aplCurrentFEATURE[FEATURENDX_NEGINDICES])
            aplLongestSubLst += *lpMemDimNam;

        // Check for INDEX ERROR (skipping over the single dimension)
        if (aplLongestSubLst >= *((LPAPLDIM) lpMemDimNam)++)
            goto INDEX_EXIT;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbNam); lpMemHdrNam = NULL;

        // Get the one (and only) item from the right arg
        GetFirstItemToken (lptkRhtArg,      // Ptr to the token
                          &aplLongestRht,   // Ptr to the longest (may be NULL)
                          &immTypeRht,      // ...        immediate type (see IMM_TYPES) (may be NULL)
                          &hGlbSubRht);     // ...        LPSYMENTRY or HGLOBAL (may be NULL)
        // If this is indexed assignment into a SysVar, ...
        if (bSysVar)
        {
            // Validate the one (and only) item from the right arg
            if (!(*SysVarValidNdx) (aplLongestSubLst,   // The origin-0 index value (in case the position is important)
                                    aplTypeRht,         // Right arg storage type
                                    &aplLongestRht,     // Ptr to the right arg value
                                     NULL,              // Ptr to right arg immediate type (may be NULL)
                                     hGlbSubRht,        // Ptr to right arg global value
                                     lptkLstArg))       // Ptr to function token
                goto DOMAIN_EXIT;

            // As we don't promote sysvars, ...
            aplTypeRes = aplTypeNam;

            *lphGlbRes = CopyGlbAsType_EM (hGlbNam, aplTypeNam, lptkNamArg);
        } else
        {
            // If the result and name arg storage types are the same, ...
            if (aplTypeRes EQ aplTypeNam)
            {
                // If the reference count for this array is one, there's
                //   no need to make a copy of the array as we can change
                //   it in place
                if (GetRefCntGlb (hGlbNam) EQ 1)
                    *lphGlbRes = hGlbNam;
                else
                    // Because this operation changes the named array,
                    //   we need to copy the entire array first.  The
                    //   caller of this code deletes the old array.
                    *lphGlbRes = CopyArray_EM (hGlbNam, lptkNamArg);
            } else
            {
                *lphGlbRes = CopyGlbAsType_EM (hGlbNam, aplTypeRes, lptkNamArg);

                // Name arg is now same type as result
                aplTypeNam = aplTypeRes;
            } // End IF/ELSE
        } // End IF/ELSE

        // Check for error
        if (*lphGlbRes EQ NULL)
            goto ERROR_EXIT;

        // Copy as new name arg global memory handle
        hGlbNam = *lphGlbRes;

        // Lock the memory to get a ptr to it
        lpMemHdrNam = MyGlobalLockVar (hGlbNam);

        // Skip over the header to the data
        lpMemNam = VarArrayDataFmBase (lpMemHdrNam);

        // If the types are different, we need to type promote the name arg
        if (aplTypeNam NE aTypePromote[aplTypeNam][aplTypeRes])
        {
            Assert (!bSysVar);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbNam); lpMemHdrNam = NULL;

            if (!TypePromote_EM (lptkNamArg, aplTypeRes, lptkFunc))
                goto ERROR_EXIT;

            // Get name arg's global ptrs
            GetGlbPtrs_LOCK (lptkNamArg, &hGlbNam, &lpMemHdrNam);

            // Save back in case it changed
            *lphGlbRes = hGlbNam;

            // Skip over the header and dimensions to the data
            lpMemNam = VarArrayDataFmBase (lpMemHdrNam);
        } // End IF

        // Promote the right arg to the result type
        (*aTypeActPromote[aplTypeRht][aplTypeRes]) (lpMemRht, 0, &atRht);

        // Split cases based upon the common storage type
        switch (aplTypeRes)
        {
            case ARRAY_BOOL:
                uBitMask = BIT0 << (MASKLOG2NBIB & (UINT) aplLongestSubLst);
                if (aplLongestRht)
                    ((LPAPLBOOL)   lpMemNam)[aplLongestSubLst >> LOG2NBIB] |= uBitMask;
                else
                    ((LPAPLBOOL)   lpMemNam)[aplLongestSubLst >> LOG2NBIB] &= ~uBitMask;
                break;

            case ARRAY_INT:
                ((LPAPLINT)    lpMemNam)[aplLongestSubLst] = atRht.aplInteger;

                break;

            case ARRAY_FLOAT:
                ((LPAPLFLOAT)  lpMemNam)[aplLongestSubLst] = atRht.aplFloat;

                break;

            case ARRAY_CHAR:
                ((LPAPLCHAR)   lpMemNam)[aplLongestSubLst] = atRht.aplChar;

                break;

            case ARRAY_HETERO:
            case ARRAY_NESTED:
                // Free the old entry
                // Split cases based upon the ptr type
                switch (GetPtrTypeDir (((LPAPLNESTED) lpMemNam)[aplLongestSubLst]))
                {
                    case PTRTYPE_STCONST:
                        break;

                    case PTRTYPE_HGLOBAL:
                        FreeResultGlobalVar (((LPAPLNESTED) lpMemNam)[aplLongestSubLst]);
                        ((LPAPLNESTED) lpMemNam)[aplLongestSubLst] = NULL;

                        break;

                    defstop
                        break;
                } // End SWITCH

                if (hGlbSubRht NE NULL)
                    ((LPAPLNESTED) lpMemNam)[aplLongestSubLst] = CopySymGlbDir_PTB (atRht.aplNested);
                else
                {
                    ((LPAPLHETERO) lpMemNam)[aplLongestSubLst] =
                    lpSymTmp =
                      MakeSymEntry_EM (immTypeRht,      // Immediate type
                                      &aplLongestRht,   // Ptr to immediate value
                                       lptkFunc);       // Ptr to function token
                    // Check for error
                    if (lpSymTmp EQ NULL)
                        goto ERROR_EXIT;
                } // End IF/ELSE

                break;

            case ARRAY_RAT:
                mpq_set (&((LPAPLRAT) lpMemNam)[aplLongestSubLst], &atRht.aplRat);

                break;

            case ARRAY_VFP:
                mpfr_set (&((LPAPLVFP) lpMemNam)[aplLongestSubLst], &atRht.aplVfp, MPFR_RNDN);

                break;

            defstop
                break;
        } // End SWITCH

        // Free the old atRht
        (*aTypeFree[aplTypeRes]) (&atRht, 0);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbNam); lpMemHdrNam = NULL;

        // If this is indexed assignment into a SysVar, ...
        if (bSysVar)
            // Execute the post-validation function
            (*aSysVarValidPost[lptkNamArg->tkData.tkSym->stFlags.SysVarValid]) (lptkNamArg);

        goto NORMAL_EXIT;
    } // End IF

    //***************************************************************
    // From here on the singleton list arg is a global.
    //***************************************************************

    // We no longer need this ptr
    MyGlobalUnlock (hGlbNam); lpMemHdrNam = NULL;

    // Because this operation changes the named array,
    //   we need to copy the entire array first.  The
    //   caller of this code deletes the old array.
    *lphGlbRes = CopyArray_EM (hGlbNam, lptkNamArg);

    // Check for error
    if (*lphGlbRes EQ NULL)
        goto ERROR_EXIT;

    // If the types are different, we need to type promote the name
    if (aplTypeNam NE aTypePromote[aplTypeNam][aplTypeRes]  // Types are different,
      && (!(bSysVar && IsEmpty (aplNELMSubLst))             // and not an empty []var
      && !TypePromoteGlb_EM (lphGlbRes, aplTypeRes, lptkFunc)))
          goto ERROR_EXIT;

    // If it's not an empty []var, ...
    if (!(bSysVar && IsEmpty (aplNELMSubLst)))
        // Respecify the new result storage type
        aplTypeNam = aTypePromote[aplTypeNam][aplTypeRes];

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);

    // Skip over the header to the dimensions
    lpMemDimRes = VarArrayBaseToDim (lpMemHdrRes);

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    //***************************************************************
    // If the singleton list arg item is simple or global numeric,
    //   do rectangular indexing.
    // The name arg must be a vector.
    //***************************************************************
    if (IsSimpleGlbNum (aplTypeSubLst))
    {
        if (IsSingleton (aplNELMRht))
            // Get the one (and only) item from the right arg
            GetFirstItemToken (lptkRhtArg,      // Ptr to the token
                              &aplLongestRht,   // Ptr to the longest (may be NULL)
                              &immTypeRht,      // ...        immediate type (see IMM_TYPES) (may be NULL)
                              &hGlbSubRht);     // ...        LPSYMENTRY or HGLOBAL (may be NULL)
        // Loop through the list indices and the right arg
        //   changing the result as appropriate
        for (uRes = 0; uRes < aplNELMSubLst; uRes++)
        {
            // Get the next item from the right arg
            if (!IsSingleton (aplNELMRht))
                GetNextValueMem (lpMemRht,                  // Ptr to right arg global memory data
                                 aplTypeRht,                // Right arg storage type
                                 aplNELMRht,                // Right arg NELM
                                 uRes,                      // Index into right arg
                                &hGlbSubRht,                // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                                &aplLongestRht,             // Ptr to result immediate value (may be NULL)
                                &immTypeRht);               // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
            // Index assignment into the top level of a vector
            bRet = ArrayIndexSetVector_EM (aplNELMNam,      // Name arg NELM
                                           lpMemSubLst,     // Ptr to list arg subitem
                                           aplTypeSubLst,   // List arg subitem storage type
                                           aplNELMSubLst,   // List arg subitem NELM
                                           aplTypeRes,      // Result storage type
                                           lpMemRes,        // Ptr to result global memory
                                           uRes,            // Index into the list arg subitem
                                           lpMemRht,        // Ptr to right arg global memory
            IsSingleton (aplNELMRht) ? 0 : uRes,            // Index into the right arg
                                           hGlbSubRht,      // Right arg item global memory handle
                                           aplTypeRht,      // Right arg storage type
                                           aplLongestRht,   // Right arg immediate value
                                           immTypeRht,      // Right arg item immediate type
                                           bQuadIO,         // []IO
                                           bSysVar,         // TRUE iff indexed assignment into a SysVar
                                           SysVarValidNdx,  // Ptr to the SysVar validation routine
                                           SysVarValidPost, // Ptr to the SysVar post-validation routine
                                           lptkNamArg,      // Ptr to name arg token
                                           lpMemPTD,        // Ptr to PerTabData global memory
                                           lptkFunc);       // Ptr to function token
            // Check for error
            if (!bRet)
                goto ERROR_EXIT;
        } // End FOR
    } else
    {
        APLSTYPE  aplTypeSet,
                  aplTypeSet2;
        IMM_TYPES immTypeSet;

        //***************************************************************
        // From here on the singleton list arg is nested
        //***************************************************************

        //***************************************************************
        // If the list arg item is a nested array of equal length simple
        //   numeric vectors, do scatter indexing, in which case
        //   the name arg rank must be the same as the length of the
        //   list arg item sub items.
        //
        // If the list item arg is a nested array of nested vectors,
        //   do reach indexing, in which case
        //   the name arg rank must be the same as the length of the
        //   first element of each of the list arg sub items.
        //***************************************************************

        // If the right arg is an APA, set the set type to INT
        if (IsSimpleAPA (aplTypeRht))
            aplTypeSet = ARRAY_INT;
        else
            aplTypeSet = aplTypeRht;

        if (IsSingleton (aplNELMRht))
            // Get the one (and only) item from the right arg
            GetFirstItemToken (lptkRhtArg,      // Ptr to the token
                              &aplLongestRht,   // Ptr to the longest (may be NULL)
                              &immTypeRht,      // ...        immediate type (see IMM_TYPES) (may be NULL)
                              &hGlbSubRht);     // ...        LPSYMENTRY or HGLOBAL (may be NULL)
        // Loop through the list indices and the right arg
        //   changing the result as appropriate
        for (uRes = 0; uRes < aplNELMSubLst; uRes++)
        {
            // Get the next value from the right arg
            if (!IsSingleton (aplNELMRht))
            {
                GetNextValueMem (lpMemRht,                          // Ptr to right arg global memory data
                                 aplTypeRht,                        // Right arg storage type
                                 aplNELMRht,                        // Right arg NELM
                                 uRes,                              // Index into right arg
                                &hGlbSubRht,                        // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                                &aplLongestRht,                     // Ptr to result immediate value (may be NULL)
                                &immTypeSet);                       // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                immTypeRht = immTypeSet;
            } // End IF

            // Split cases based upon the list arg subitem ptr type
            switch (GetPtrTypeDir (((LPAPLHETERO) lpMemSubLst)[uRes]))
            {
                LPPL_YYSTYPE lpYYItm;
                HGLOBAL      hGlbSubLst2;
                APLSTYPE     aplTypeSubLst2;
                APLNELM      aplNELMSubLst2;
                APLRANK      aplRankSubLst2;

                case PTRTYPE_STCONST:
                    // Check for RANK ERROR
                    if (!IsVector (aplRankNam))
                        goto RANK_EXIT;

                    // Index assignment into the top level of a vector
                    bRet = ArrayIndexSetVector_EM (aplNELMNam,      // Name arg NELM
                                                   lpMemSubLst,     // Ptr to list arg subitem
                                                   aplTypeSubLst,   // List arg subitem storage type
                                                   aplNELMSubLst,   // List arg subitem NELM
                                                   aplTypeRes,      // Result storage type
                                                   lpMemRes,        // Ptr to result global memory
                                                   uRes,            // Index into the list arg subitem
                                                   lpMemRht,        // Ptr to right arg global memory
                    IsSingleton (aplNELMRht) ? 0 : uRes,            // Index into the right arg
                                                   hGlbSubRht,      // Right arg item global memory handle
                                                   aplTypeRht,      // Right arg storage type
                                                   aplLongestRht,   // Right arg immediate value
                                                   immTypeRht,      // Right arg item immediate type
                                                   bQuadIO,         // []IO
                                                   bSysVar,         // TRUE iff indexed assignment into a SysVar
                                                   SysVarValidNdx,  // Ptr to the SysVar validation routine
                                                   SysVarValidPost, // Ptr to the SysVar post-validation routine
                                                   lptkNamArg,      // Ptr to name arg token
                                                   lpMemPTD,        // Ptr to PerTabData global memory
                                                   lptkFunc);       // Ptr to function token
                    // Check for error
                    if (!bRet)
                        goto ERROR_EXIT;
                    break;

                case PTRTYPE_HGLOBAL:
                    // Get the global memory handle
                    hGlbSubLst2 = ((LPAPLHETERO) lpMemSubLst)[uRes];

                    AttrsOfGlb (hGlbSubLst2, &aplTypeSubLst2, &aplNELMSubLst2, &aplRankSubLst2, NULL);

                    // If the global is an empty vector, ...
                    if (IsVector (aplRankSubLst2)
                     && IsEmpty  (aplNELMSubLst2))
                    {
                        // We no longer need this ptr
                        MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;

                        // We no longer need this storage
                        FreeResultGlobalIncompleteVar (*lphGlbRes); *lphGlbRes = NULL;

                        // If the list is a singleton, ...
                        if (IsSingleton (aplNELMSubLst))
                        {
                            // Assign the right arg to the name
                            // Note that <AssignName_EM> sets the <NoDisplay> flag in the source token
                            if (AssignName_EM (lptkNamArg,
                                               lptkRhtArg))
                                goto NORMAL_EXIT;
                            else
                                goto ERROR_EXIT;
                        } else
                            goto INDEX_EXIT;
                    } // End IF

                    // Because of the way RightShoe works, if this global is simple
                    //   or global numeric and its NELM matches the rank of the name arg,
                    //   we need to enclose it first
                    if (IsSimpleGlbNum (aplTypeSubLst2)
                     && aplNELMSubLst2 EQ aplRankNam)
                    {
                        lpYYItm =
                          PrimFnMonLeftShoeGlb_EM_YY (hGlbSubLst2,          // Right arg global memory handle
                                                      NULL,                 // Ptr to axis token (may be NULL)
                                                      lptkFunc);            // Ptr to function token
                        // Save just the global memory handle (which we need to free later)
                        hGlbSubLst2 = lpYYItm->tkToken.tkData.tkGlbData;

                        YYFree (lpYYItm); lpYYItm = NULL;
                    } // End IF

                    // If the item is immediate,
                    //   and the parent is hetero, ...
                    if (hGlbSubRht EQ NULL
                     && IsSimpleHet (aplTypeSet))
                        // Use the immediate type
                        aplTypeSet2 = TranslateImmTypeToArrayType (immTypeSet);
                    else
                        aplTypeSet2 = aplTypeSet;

                    // Set the corresponding element of the result
                    // Note the args get switched between indexing and pick
                    lpYYItm =
                      PrimFnDydRightShoeGlbGlb_EM_YY (hGlbSubLst2,      // Left arg global memory handle
                                                     *lphGlbRes,        // Right arg global memory handle
                                                      lptkFunc,         // Ptr to function token
                                                      TRUE,             // TRUE iff array assignment
                                                      aplTypeSet2,      // Set arg storage type
                                                      hGlbSubRht,       // Set arg global memory handle/LPSYMENTRY (NULL if immediate)
                                                      aplLongestRht,    // Set arg immediate value
                                                      lpMemPTD);        // Ptr to PerTabData global memory
                    // If we allocated it above, free it now
                    if (IsSimpleGlbNum (aplTypeSubLst2)
                     && aplNELMSubLst2 EQ aplRankNam)
                    {
                        // We no longer need this storage
                        FreeResultGlobalVar (hGlbSubLst2); hGlbSubLst2 = NULL;
                    } // End IF

                    // Check for error
                    if (lpYYItm EQ NULL)
                        goto ERROR_EXIT;
                    break;

                defstop
                    break;
            } // End SWITCH
        } // End FOR
    } // End IF/ELSE/...

    goto NORMAL_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

INDEX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
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

    if (hGlbSubLst NE NULL && lpMemHdrSubLst NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbSubLst); lpMemHdrSubLst = NULL;
    } // End IF

    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    if (hGlbLst NE NULL && lpMemHdrLst NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLst); lpMemHdrLst = NULL;
    } // End IF

    if (hGlbNam NE NULL && lpMemHdrNam NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbNam); lpMemHdrNam = NULL;
    } // End IF

    return bRet;
} // End ArrayIndexSetSingLst_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexSetVector_EM
//
//  Array assignment into a vector
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexSetVector_EM"
#else
#define APPEND_NAME
#endif

UBOOL ArrayIndexSetVector_EM
    (APLNELM          aplNELMNam,       // Name arg NELM
     LPVOID           lpMemSubLst,      // Ptr to list arg subitem
     APLSTYPE         aplTypeSubLst,    // List arg subitem storage type
     APLNELM          aplNELMSubLst,    // List arg subitem NELM
     APLSTYPE         aplTypeRes,       // Result storage type
     LPVOID           lpMemRes,         // Ptr to result global memory
     APLUINT          uRes,             // Index into the list arg subitem
     LPVOID           lpMemRht,         // Ptr to right arg global memory
     APLUINT          uRht,             // Index into the right arg
     HGLOBAL          hGlbSubRht,       // Right arg item global memory handle
     APLSTYPE         aplTypeRht,       // Right arg storage type
     APLLONGEST       aplLongestRht,    // Right arg immediate value
     IMM_TYPES        immTypeRht,       // Right arg item immediate type
     UBOOL            bQuadIO,          // []IO
     UBOOL            bSysVar,          // TRUE iff indexed assignment into a SysVar
     ASYSVARVALIDNDX  SysVarValidNdx,   // Ptr to the SysVar validation routine
     ASYSVARVALIDPOST SysVarValidPost,  // Ptr to the SysVar post-validation routine
     LPTOKEN          lptkNamArg,       // Ptr to name arg token
     LPPERTABDATA     lpMemPTD,         // Ptr to PerTabData global memory
     LPTOKEN          lptkFunc)         // Ptr to function token

{
    UBOOL      bRet = TRUE;             // TRUE iff the result is valid
    APLLONGEST aplLongestSubLst;        // List arg subitem immediate value
    LPSYMENTRY lpSymGlbSubLst;          // Ptr to list arg subitem
    IMM_TYPES  immTypeSubLst;           // List arg subitem immediate type
    LPSYMENTRY lpSymTmp;                // Ptr to temporary LPSYMENTRY

    // Get next index from global memory
    GetNextValueMem (lpMemSubLst,       // Ptr to item global memory data
                     aplTypeSubLst,     // Item storage type
                     aplNELMSubLst,     // Item NELM
                     uRes,              // Index into item
                    &lpSymGlbSubLst,    // Ptr to result (may be NULL)
                    &aplLongestSubLst,  // Ptr to result immediate value (may be NULL)
                    &immTypeSubLst);    // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
    // Check for NESTED/HETERO
    if (!IsImmErr (immTypeSubLst))
    {
        // Split cases based upon the list arg subitem immediate type
        switch (immTypeSubLst)
        {
            case IMMTYPE_BOOL:
                aplLongestSubLst &= BIT0;

                break;

            case IMMTYPE_INT:
                break;

            case IMMTYPE_FLOAT:
            case IMMTYPE_RAT:
            case IMMTYPE_VFP:
                // Attempt to convert the item to an integer using System []CT
                aplLongestSubLst =
                  ConvertToInteger_SCT (TranslateImmTypeToArrayType (immTypeSubLst),
                                        lpSymGlbSubLst,
                                        0,
                                       &bRet);
                if (bRet)
                    break;

                // Fall through to common DOMAIN ERROR code

            case IMMTYPE_CHAR:
                goto DOMAIN_EXIT;

            defstop
                break;
        } // End SWITCH
    } else
    // Split cases based upon the list arg subitem storage type
    switch (aplTypeSubLst)
    {
        case ARRAY_BOOL:
            aplLongestSubLst &= BIT0;

            break;

        case ARRAY_INT:
        case ARRAY_APA:
            break;

        case ARRAY_NESTED:
        {
            APLSTYPE          aplTypeSubLst2;
            APLNELM           aplNELMSubLst2;
            APLRANK           aplRankSubLst2;
            LPVARARRAY_HEADER lpMemHdrSubLst2 = NULL;   // Ptr to sublist header
            LPAPLLONGEST      lpMemSubLst2;             // Ptr to data

            // Test for a simple scalar

            // Split cases based upon the ptr type
            switch (GetPtrTypeDir (lpSymGlbSubLst))
            {
                case PTRTYPE_STCONST:
                    aplTypeSubLst2 = TranslateImmTypeToArrayType (lpSymGlbSubLst->stFlags.ImmType);
                    aplNELMSubLst2 = 1;
                    aplRankSubLst2 = 0;

                    // Point to the data
                    lpMemSubLst2 = &lpSymGlbSubLst->stData.stLongest;

                    break;

                case PTRTYPE_HGLOBAL:
                    // Lock the memory to get a ptr to it
                    lpMemHdrSubLst2 = MyGlobalLockVar (lpSymGlbSubLst);

                    // Get the type and rank
                    aplTypeSubLst2 = lpMemHdrSubLst2->ArrType;
                    aplNELMSubLst2 = lpMemHdrSubLst2->NELM;
                    aplRankSubLst2 = lpMemHdrSubLst2->Rank;

                    // Skip over the header and dimensions
                    lpMemSubLst2 = VarArrayDataFmBase (lpMemHdrSubLst2);

                    break;

                defstop
                    break;
            } // End SWITCH

            // If the array is a simple scalar, ...
            if (IsSimpleNHGlbNum (aplTypeSubLst2)
             && IsScalar   (aplRankSubLst2))
            {
                // Get the value
                // Split cases based upon the storage type
                switch (aplTypeSubLst2)
                {
                    case ARRAY_BOOL:
                        // Get the value
                        aplLongestSubLst = BIT0 & *(LPAPLBOOL) lpMemSubLst2;

                        break;

                    case ARRAY_INT:
                        // Get the value
                        aplLongestSubLst = *(LPAPLINT) lpMemSubLst2;

                        break;

                    case ARRAY_APA:
                        // Get the value
                        // In case the APA has a zero multiplier, we can't necessarily trust the
                        //   offset to be zero, too.
                        aplLongestSubLst = ((LPAPLAPA) lpMemSubLst2)->Off * ((LPAPLAPA) lpMemSubLst2)->Mul;

                        break;

                    case ARRAY_FLOAT:
                    case ARRAY_RAT:
                    case ARRAY_VFP:
                        // Attempt to convert the item to an integer using System []CT
                        aplLongestSubLst = ConvertToInteger_SCT (aplTypeSubLst2, lpMemSubLst2, 0, &bRet);
                        if (bRet)
                            break;

                        // Fall through to common DOMAIN ERROR code

                    case ARRAY_CHAR:
                        goto DOMAIN_EXIT;

                    defstop
                        break;
                } // End SWITCH
            } else
                bRet = FALSE;

            if (lpSymGlbSubLst NE NULL
             && lpMemHdrSubLst2 NE NULL)
            {
                // We no longer need this ptr
                MyGlobalUnlock (lpSymGlbSubLst); lpMemHdrSubLst2 = NULL;
            } // End IF

            if (!bRet)
                goto DOMAIN_EXIT;

            break;
        } // End ARRAY_NESTED

        case ARRAY_FLOAT:
        case ARRAY_RAT:
        case ARRAY_VFP:
            // Attempt to convert the item to an integer using System []CT
            aplLongestSubLst = ConvertToInteger_SCT (aplTypeSubLst, lpSymGlbSubLst, 0, &bRet);

            if (bRet)
                break;

            // Fall through to common DOMAIN ERROR code

        case ARRAY_CHAR:
        case ARRAY_HETERO:
            goto DOMAIN_EXIT;

        defstop
            break;
    } // End SWITCH

    // Convert to origin-0
    aplLongestSubLst -= bQuadIO;

    // Check for negative indices [-aplNELMNam, -1]
    if (SIGN_APLLONGEST (aplLongestSubLst)
     && lpMemPTD->aplCurrentFEATURE[FEATURENDX_NEGINDICES])
        aplLongestSubLst += aplNELMNam;

    // Check for INDEX ERROR
    if (aplLongestSubLst >= aplNELMNam)
        goto INDEX_EXIT;

    // If this is indexed assignment into a SysVar, ...
    if (bSysVar)
    {
        // Validate the one (and only) item from the right arg
        if (!(*SysVarValidNdx) (aplLongestSubLst,       // The origin-0 index value (in case the position is important)
                                aplTypeRht,             // Right arg storage type
                               &aplLongestRht,          // Ptr to the right arg value
                               &immTypeRht,             // Ptr to right arg immediate type (may be NULL)
                                hGlbSubRht,             // Ptr to right arg global value
                                lptkFunc))              // Ptr to function token
            goto DOMAIN_EXIT;
    } // End IF

    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        UINT uBitMask;          // Mask for working with Booleans

        case ARRAY_BOOL:
            // Replace the corresponding item in the result
            uBitMask = (BIT0 << (MASKLOG2NBIB & (UINT) aplLongestSubLst));
            if (BIT0 & aplLongestRht)
                ((LPAPLBOOL)    lpMemRes)[aplLongestSubLst >> LOG2NBIB] |= uBitMask;
            else
                ((LPAPLBOOL)    lpMemRes)[aplLongestSubLst >> LOG2NBIB] &= ~uBitMask;
            break;

        case ARRAY_INT:
            // Replace the corresponding item in the result
            ((LPAPLINT)    lpMemRes)[aplLongestSubLst] = (APLINT) aplLongestRht;

            break;

        case ARRAY_FLOAT:
            // Replace the corresponding item in the result
            // If the right arg is simple integer, convert it to float
            if (IsSimpleInt (aplTypeRht))
                ((LPAPLFLOAT)  lpMemRes)[aplLongestSubLst] = (APLFLOAT) (APLINT) aplLongestRht;
            else
                ((LPAPLFLOAT)  lpMemRes)[aplLongestSubLst] = *(LPAPLFLOAT) &aplLongestRht;
            break;

        case ARRAY_CHAR:
            // Replace the corresponding item in the result
            ((LPAPLCHAR)   lpMemRes)[aplLongestSubLst] = (APLCHAR) aplLongestRht;

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Free the old value
            // Split cases based upon the ptr type
            switch (GetPtrTypeDir (((LPAPLNESTED) lpMemRes)[aplLongestSubLst]))
            {
                case PTRTYPE_STCONST:
                    break;

                case PTRTYPE_HGLOBAL:
                    FreeResultGlobalVar (((LPAPLNESTED) lpMemRes)[aplLongestSubLst]);
                    ((LPAPLNESTED) lpMemRes)[aplLongestSubLst] = NULL;

                    break;

                defstop
                    break;
            } // End IF/SWITCH

            // If the right arg item is an HGLOBAL, ...
            if (hGlbSubRht NE NULL)
                // Replace the corresponding item in the result
                ((LPAPLNESTED) lpMemRes)[aplLongestSubLst] =
                  CopySymGlbDir_PTB (hGlbSubRht);
            else
            {
                // Replace the corresponding item in the result
                ((LPAPLNESTED) lpMemRes)[aplLongestSubLst] =
                lpSymTmp =
                  MakeSymEntry_EM (immTypeRht,      // Immediate type
                                  &aplLongestRht,   // Ptr to immediate value
                                   lptkFunc);       // Ptr to function token
                // Check for error
                if (lpSymTmp EQ NULL)
                    goto ERROR_EXIT;
            } // End IF/ELSE

            break;

        case ARRAY_RAT:
            // Clear out the old
            Myq_clear (&((LPAPLRAT) lpMemRes)[aplLongestSubLst]);

            // Promote the right arg to the result type
            (*aTypeActPromote[aplTypeRht][aplTypeRes])(lpMemRht, uRht, (LPALLTYPES) &((LPAPLRAT) lpMemRes)[aplLongestSubLst]);

            break;

        case ARRAY_VFP:
            // Clear out the old
            Myf_clear (&((LPAPLVFP) lpMemRes)[aplLongestSubLst]);

            // Promote the right arg to the result type
            (*aTypeActPromote[aplTypeRht][aplTypeRes])(lpMemRht, uRht, (LPALLTYPES) &((LPAPLVFP) lpMemRes)[aplLongestSubLst]);

            break;

        defstop
            break;
    } // End SWITCH

    // If this is indexed assignment into a SysVar, ...
    if (bSysVar)
        // Execute the post-validation function
        (*SysVarValidPost) (lptkNamArg);

    goto NORMAL_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

INDEX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    bRet = FALSE;
NORMAL_EXIT:
    return bRet;
} // End ArrayIndexSetVector_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexSetRect_EM
//
//  Array assignment with a multiple list arg
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexSetRect_EM"
#else
#define APPEND_NAME
#endif

UBOOL ArrayIndexSetRect_EM
    (LPTOKEN    lptkNamArg,                     // Ptr to name arg token
     LPTOKEN    lptkLstArg,                     // Ptr to list ...
     LPTOKEN    lptkRhtArg,                     // Ptr to right ...
     UBOOL      bSysVar,                        // TRUE iff indexed assignment into a SysVar
     LPTOKEN    lptkFunc)                       // Ptr to function token

{
    APLNELM           aplNELMLst;               // List arg NELM
    APLRANK           aplRankNam;               // Name arg rank
    HGLOBAL           hGlbNam = NULL,           // Name arg global memory handle
                      hGlbNam2 = NULL,          // Secondary name ...
                      hGlbRes = NULL,           // Result   ...
                      hGlbLst = NULL;           // List ...
    LPVARARRAY_HEADER lpMemHdrAxis = NULL;      // Ptr to axis header
    LPLSTARRAY_HEADER lpMemHdrLst = NULL,       // Ptr to list arg header
                      lpMemHdrLstNew = NULL;    // Ptr to new list ...
    LPAPLLIST         lpMemLst;                 // Ptr to list ...
    UBOOL             bRet = FALSE;             // TRUE iff result is valid
    LPPL_YYSTYPE      lpYYRes = NULL;           // Ptr to the result
    TOKEN             tkLstArg = {0},           // New list arg token
                      tkAxis = {0};             // Axis token

    Assert (!bSysVar);

    // Get the attributes (Type, NELM, and Rank) of the name and list args
    AttrsOfToken (lptkNamArg, NULL, NULL,        &aplRankNam, NULL);
    AttrsOfToken (lptkLstArg, NULL, &aplNELMLst, NULL,        NULL);

    // Check for RANK ERROR
    if (aplRankNam NE aplNELMLst
     && !IsSingleton (aplNELMLst))
        goto RANK_EXIT;

    // Get name and list arg's global ptrs
    hGlbNam = GetGlbHandle (lptkNamArg);
    GetGlbPtrs_LOCK (lptkLstArg, &hGlbLst, &lpMemHdrLst);

    // Skip over the header
    lpMemLst = LstArrayBaseToData (lpMemHdrLst);

    // Fill in the temporary APV values (if any)
    FillTempApv (hGlbNam, lpMemLst, aplNELMLst);

    // Save original name arg global memory handle
    hGlbNam2 = hGlbNam;

    // Note the args get switched between indexing and squad
    lpYYRes =
      PrimFnDydSquadGlb_EM_YY (lptkLstArg,      // Ptr to Left arg token
                              &hGlbNam2,        // Ptr to right arg global memory handle
                               NULL,            // Ptr to axis token (may be NULL)
                               lptkFunc,        // Ptr to function token
                               TRUE,            // TRUE iff we came from indexing
                              &hGlbRes,         // Ptr to result global memory handle
                               lptkRhtArg,      // Ptr to set arg token
                               FALSE);          // TRUE iff protoyping
    // If the result is valid, ...
    if (lpYYRes NE NULL)
    {
        // If the name global memory handle changed, ...
        if (hGlbNam NE hGlbNam2)
        {
            // Free the old one
            FreeResultGlobalVar (hGlbNam); hGlbNam = NULL;
        } // End IF

        // Save the global
        lptkNamArg->tkData.tkSym->stData.stGlbData = MakePtrTypeGlb (hGlbRes);
    } // End IF

    // The result is dependent upon the state of lpYYRes
    //   (it's either NULL or PTR_SUCCESS)
    Assert (IsPtrSuccess (lpYYRes) || IsPtrNullDir (lpYYRes));
    bRet = IsPtrSuccess (lpYYRes);
    if (bRet)
        goto NORMAL_EXIT;
    else
        goto ERROR_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes NE NULL)
    {
        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbLst NE NULL && lpMemHdrLst NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLst); lpMemHdrLst = NULL;
    } // End IF

    return bRet;
} // End ArrayIndexSetRect_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexReplace_EM
//
//  Replace a top level value in an array
//***************************************************************************

UBOOL ArrayIndexReplace_EM
    (APLSTYPE   aplTypeRht,             // Right arg storage type
     LPVOID     lpMemRht,               // Ptr to right arg global memory data
     APLUINT    aplIndex,               // Index into right arg
     APLSTYPE   aplTypeSet,             // Set arg storage type
     APLLONGEST aplLongestSet,          // Set arg immediate value
     HGLOBAL    hGlbSet,                // Set arg global memory handle
     LPTOKEN    lptkFunc)               // Ptr to function token

{
    HGLOBAL    hGlbSubRht;
    UINT       uBitMask;
    UBOOL      bRet = FALSE;            // TRUE iff the result is valid
    LPSYMENTRY lpSymTmp;                // Ptr to temporary LPSYMENTRY
    ALLTYPES   atSet = {0};             // Set arg as ALLTYPES
    LPVOID     lpMemSet;                // Ptr to set arg global memory

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            uBitMask = BIT0 << (MASKLOG2NBIB & (UINT) aplIndex);

            if (aplLongestSet & BIT0)
                ((LPAPLBOOL) lpMemRht)[aplIndex >> LOG2NBIB] |= uBitMask;
            else
                ((LPAPLBOOL) lpMemRht)[aplIndex >> LOG2NBIB] &= ~uBitMask;
            break;

        case ARRAY_INT:
            ((LPAPLINT) lpMemRht)[aplIndex] = (APLINT) aplLongestSet;

            break;

        case ARRAY_FLOAT:
            if (IsSimpleInt (aplTypeSet))
                // Convert the integer to a float
                ((LPAPLFLOAT) lpMemRht)[aplIndex] = (APLFLOAT) (APLINT) aplLongestSet;
            else
                ((LPAPLFLOAT) lpMemRht)[aplIndex] = *(LPAPLFLOAT) &aplLongestSet;

            break;

        case ARRAY_CHAR:
            ((LPAPLCHAR) lpMemRht)[aplIndex] = (APLCHAR) aplLongestSet;

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Get the previous value
            hGlbSubRht = ((LPAPLNESTED) lpMemRht)[aplIndex];

            // If the set arg is simple, ...
            if (IsSimple (aplTypeSet))
            {
                ((LPAPLNESTED) lpMemRht)[aplIndex] =
                lpSymTmp =
                  MakeSymEntry_EM (TranslateArrayTypeToImmType (aplTypeSet),    // Immediate type
                                  &aplLongestSet,                               // Ptr to immediate value
                                   lptkFunc);                                   // Ptr to function token
                // Check for error
                if (lpSymTmp EQ NULL)
                    goto ERROR_EXIT;
            } else
            if (IsGlbNum (aplTypeSet))
            {
                ((LPAPLNESTED) lpMemRht)[aplIndex] =
                lpSymTmp =
                  MakeGlbEntry_EM (aplTypeSet,          // Entry type
                                   hGlbSet,             // Ptr to the value
                                   TRUE,                // TRUE iff we should initialize the target first
                                   lptkFunc);           // Ptr to function token
                // Check for error
                if (lpSymTmp EQ NULL)
                    goto ERROR_EXIT;
            } else
                ((LPAPLNESTED) lpMemRht)[aplIndex] =
                  CopySymGlbDirAsGlb (hGlbSet);

            // Free the old value if it's a global
            if (GetPtrTypeDir (hGlbSubRht) EQ PTRTYPE_HGLOBAL)
            {
                FreeResultGlobalVar (hGlbSubRht); hGlbSubRht = NULL;
            } // End IF

            break;

        case ARRAY_RAT:
            // Because we have already handled type promotion,
            //   aplTypeRht is "wider" or equal to aplTypeSet

            // If the set arg is simple, ...
            if (hGlbSet EQ NULL)
                lpMemSet = &aplLongestSet;
            else
                lpMemSet = hGlbSet;

            // Promote the set arg to the right arg type
            (*aTypeActConvert[aplTypeSet][aplTypeRht])(lpMemSet, 0, &atSet);

            // Save the result in the right arg
            ((LPAPLRAT) lpMemRht)[aplIndex] = atSet.aplRat;

            break;

        case ARRAY_VFP:
            // Because we have already handled type promotion,
            //   aplTypeRht is "wider" or equal to aplTypeSet

            // If the set arg is simple, ...
            if (hGlbSet EQ NULL)
                lpMemSet = &aplLongestSet;
            else
                lpMemSet = hGlbSet;

            // Promote the set arg to the right arg type
            (*aTypeActConvert[aplTypeSet][aplTypeRht])(lpMemSet, 0, &atSet);

            // Save the result in the right arg
            ((LPAPLVFP) lpMemRht)[aplIndex] = atSet.aplVfp;

            break;

        defstop
            break;
    } // End SWITCH

    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
    return bRet;
} // End ArrayIndexReplace_EM


//***************************************************************************
//  $ArrayIndexFcnSet_EM
//
//  Modified index assignment of an array, e.g., A[L]fcn{is}R
//                                           or  A[L1;...;Ln]fcn{is}R
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexFcnSet_EM"
#else
#define APPEND_NAME
#endif

UBOOL ArrayIndexFcnSet_EM
    (LPTOKEN      lptkNamArg,           // Ptr to name arg token
     LPTOKEN      lptkLstArg,           // Ptr to list arg token
     LPPL_YYSTYPE lpYYFcnStr,           // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    LPPL_YYSTYPE lpYYRes1 = NULL,       // Ptr to temporary resuilt
                 lpYYRes2 = NULL,       // ...
                 lpYYRes3 = NULL,       // ...
                 lpYYResL = NULL,       // ...
                 lpYYResR = NULL,       // ...
                 lpYYResInd = NULL,     // ...                      for Ind
                 lpYYResSub = NULL;     // ...                      for Ind[I]
    UBOOL        bRet = TRUE;           // TRUE iff result is valid
    TOKEN        tkFunc = {0},          // Temporary token for a function
                 tkList = {0};          // ...                   list
    APLNELM      aplNELMInd,            // The NELM of the indices
                 aplNELMArg,            // ...             right arg
                 uInd;                  // Loop counter
    APLBOOL      bQuadIO;               // []IO

    // Get []IO
    bQuadIO = GetQuadIO ();

    //***************************************************************************
    // Compute ,({iota}{rho} NAM)[L]
    //  so we can loop through the indices one by one
    //  in case there are duplicate indices
    //***************************************************************************

    // Setup a token with the {rho} function
    tkFunc.tkFlags.TknType   = TKT_FCNIMMED;
    tkFunc.tkFlags.ImmType   = IMMTYPE_PRIMFCN;
////tkFunc.tkFlags.NoDisplay = FALSE;           // Already zero from tkZero
    tkFunc.tkData.tkChar     = UTF16_RHO;
    tkFunc.tkCharIndex       = lpYYFcnStr->tkToken.tkCharIndex;

    // Compute {rho}NAM
    lpYYRes1 =
      PrimFnMonRho_EM_YY (&tkFunc,              // Ptr to function token
                           lptkNamArg,          // Ptr to right arg token
                           NULL);               // Ptr to axis token (may be NULL)
    // Check for error
    if (lpYYRes1 EQ NULL)
        goto ERROR_EXIT;

    // Setup a token with the {iota} function
////tkFunc.tkFlags.TknType   = TKT_FCNIMMED;        // Already set above
////tkFunc.tkFlags.ImmType   = IMMTYPE_PRIMFCN;     // Already set above
////tkFunc.tkFlags.NoDisplay = FALSE;               // Already zero from = {0}
    tkFunc.tkData.tkChar     = UTF16_IOTA;
////tkFunc.tkCharIndex       = lpYYFcnStrLft->tkToken.tkCharIndex;

    // Compute {iota}{rho}NAM
    lpYYRes2 =
      PrimFnMonIota_EM_YY (&tkFunc,                 // Ptr to function token
                           &lpYYRes1->tkToken,      // Ptr to right arg token
                            NULL);                  // Ptr to axis token (may be NULL)
    FreeResult (lpYYRes1); YYFree (lpYYRes1); lpYYRes1 = NULL;

    // Check for error
    if (lpYYRes2 EQ NULL)
        goto ERROR_EXIT;

    // Compute ({iota}{rho} NAM)[L]
    lpYYRes1 =
      ArrayIndexRef_EM_YY (&lpYYRes2->tkToken,      // Ptr to indices token
                            lptkLstArg);            // Ptr to right arg token
    FreeResult (lpYYRes2); YYFree (lpYYRes2); lpYYRes2 = NULL;

    // Check for error
    if (lpYYRes1 EQ NULL)
        goto ERROR_EXIT;

    // Setup a token with the {comma} function
////tkFunc.tkFlags.TknType   = TKT_FCNIMMED;        // Already set above
////tkFunc.tkFlags.ImmType   = IMMTYPE_PRIMFCN;     // Already set above
////tkFunc.tkFlags.NoDisplay = FALSE;               // Already zero from = {0}
    tkFunc.tkData.tkChar     = UTF16_COMMA;
////tkFunc.tkCharIndex       = lpYYFcnStrLft->tkToken.tkCharIndex;

    // Compute ,({iota}{rho} NAM)[L]
    lpYYResInd =
      PrimFnMonComma_EM_YY (&tkFunc,                // Ptr to function token
                            &lpYYRes1->tkToken,     // Ptr to right arg token
                             NULL);                 // Ptr to axis token (may be NULL)
    FreeResult (lpYYRes1); YYFree (lpYYRes1); lpYYRes1 = NULL;

    // Check for error
    if (lpYYResInd EQ NULL)
        goto ERROR_EXIT;

    // Get the NELM of the indices & right arg
    AttrsOfToken (&lpYYResInd->tkToken, NULL, &aplNELMInd, NULL, NULL);
    AttrsOfToken ( lptkRhtArg         , NULL, &aplNELMArg, NULL, NULL);

    // Loop through the values in Ind
    for (uInd = 0; bRet && uInd < aplNELMInd; uInd++)
    {
        //***************************************************************************
        // Compute {enclose}({first}NAM[Ind[I]]) fcn {first}R[I]
        //  so we can assign it into NAM[Ind[I]]
        //***************************************************************************

        // Form I as a list arg
        tkList.tkFlags.TknType   = TKT_VARIMMED;
        tkList.tkFlags.ImmType   = IMMTYPE_INT;
////////tkList.tkFlags.NoDisplay = FALSE;           // Already zero from tkZero
        tkList.tkData.tkInteger  = uInd + bQuadIO;
        tkList.tkCharIndex       = lpYYFcnStr->tkToken.tkCharIndex;

        // Compute Ind[I]
        lpYYResSub =
          ArrayIndexRef_EM_YY (&lpYYResInd->tkToken,
                               &tkList);
        if (lpYYResSub EQ NULL)
            goto ERROR_EXIT;
        // If the right arg is not a singleton, ...
        if (!IsSingleton (aplNELMArg))
        {
            // Compute R[I]
            lpYYRes1 =
              ArrayIndexRef_EM_YY (lptkRhtArg,
                                  &tkList);
            // Check for error
            if (lpYYRes1 EQ NULL)
                goto ERROR_EXIT;
        } else
        {
            // Allocate a new YYRes
            lpYYRes1 = YYAlloc ();

            // Copy right arg
            lpYYRes1->tkToken = *lptkRhtArg;
        } // End IF/ELSE

        // Setup a token with the {first} function
////////tkFunc.tkFlags.TknType   = TKT_FCNIMMED;        // Already set above
////////tkFunc.tkFlags.ImmType   = IMMTYPE_PRIMFCN;     // Already set above
////////tkFunc.tkFlags.NoDisplay = FALSE;               // Already zero from = {0}
        tkFunc.tkData.tkChar     = UTF16_UPARROW;
////////tkFunc.tkCharIndex       = lpYYFcnStrLft->tkToken.tkCharIndex;

        // Compute {first}R[I]
        lpYYResR =
          PrimFnMonUpArrow_EM_YY (&tkFunc,              // Ptr to function token
                                  &lpYYRes1->tkToken,   // Ptr to right arg token
                                   NULL);               // Ptr to axis token (may be NULL)
        // If the right arg is not a singleton, ...
        if (!IsSingleton (aplNELMArg))
            FreeResult (lpYYRes1);
        // Clear it
        YYFree (lpYYRes1); lpYYRes1 = NULL;

        // Check for error
        if (lpYYResR EQ NULL)
            goto ERROR_EXIT;

        // Compute NAM[Ind[I]]
        lpYYRes1 =
          ArrayIndexRef_EM_YY (lptkNamArg,
                              &lpYYResSub->tkToken);
        // Check for error
        if (lpYYRes1 EQ NULL)
            goto ERROR_EXIT;

        // Setup a token with the {first} function
////////tkFunc.tkFlags.TknType   = TKT_FCNIMMED;        // Already set above
////////tkFunc.tkFlags.ImmType   = IMMTYPE_PRIMFCN;     // Already set above
////////tkFunc.tkFlags.NoDisplay = FALSE;               // Already zero from = {0}
////////tkFunc.tkData.tkChar     = UTF16_UPARROW;       // Already set above
////////tkFunc.tkCharIndex       = lpYYFcnStrLft->tkToken.tkCharIndex;

        // Compute {first}NAM[Ind[I]]
        lpYYResL =
          PrimFnMonUpArrow_EM_YY (&tkFunc,              // Ptr to function token
                                  &lpYYRes1->tkToken,   // Ptr to right arg token
                                   NULL);               // Ptr to axis token (may be NULL)
        FreeResult (lpYYRes1); YYFree (lpYYRes1); lpYYRes1 = NULL;

        // Check for error
        if (lpYYResL EQ NULL)
            goto ERROR_EXIT;

        // Compute ({first}NAM[Ind[I]]) fcn {first}R[I]
        lpYYRes1 =
          ExecFuncStr_EM_YY (&lpYYResL->tkToken,    // Ptr to left arg token (may be NULL if monadic)
                              lpYYFcnStr,           // Ptr to function strand
                             &lpYYResR->tkToken,    // Ptr to right arg token
                              NULL);                // Ptr to axis token (may be NULL)
        FreeResult (lpYYResL); YYFree (lpYYResL); lpYYResL = NULL;
        FreeResult (lpYYResR); YYFree (lpYYResR); lpYYResR = NULL;

        // Check for error
        if (lpYYRes1 EQ NULL)
            goto ERROR_EXIT;

        // Setup a token with the {enclose} function
////////tkFunc.tkFlags.TknType   = TKT_FCNIMMED;        // Already set above
////////tkFunc.tkFlags.ImmType   = IMMTYPE_PRIMFCN;     // Already set above
////////tkFunc.tkFlags.NoDisplay = FALSE;               // Already zero from = {0}
        tkFunc.tkData.tkChar     = UTF16_LEFTSHOE;
////////tkFunc.tkCharIndex       = lpYYFcnStrLft->tkToken.tkCharIndex;

        // Compute {enclose}({first}NAM[Ind[I]]) fcn {first}R[I]
        lpYYRes2 =
          PrimFnMonLeftShoe_EM_YY (&tkFunc,             // Ptr to function token
                                   &lpYYRes1->tkToken,  // Ptr to right arg token
                                    NULL);              // Ptr to axis token (may be NULL)
        FreeResult (lpYYRes1); YYFree (lpYYRes1); lpYYRes1 = NULL;

        // Check for error
        if (lpYYRes2 EQ NULL)
            goto ERROR_EXIT;

        // Assign the resulting values into NAM[Ind[I]]
        bRet =
          ArrayIndexSet_EM (lptkNamArg,
                           &lpYYResSub->tkToken,
                           &lpYYRes2->tkToken);
        FreeResult (lpYYRes2  ); YYFree (lpYYRes2  ); lpYYRes2   = NULL;
        FreeResult (lpYYResSub); YYFree (lpYYResSub); lpYYResSub = NULL;
    } // End FOR

    goto NORMAL_EXIT;

ERROR_EXIT:
    // Mark as in error
    bRet = FALSE;
NORMAL_EXIT:
    FreeYYRes (lpYYRes1  );
    FreeYYRes (lpYYRes2  );
    FreeYYRes (lpYYRes3  );
    FreeYYRes (lpYYResL  );
    FreeYYRes (lpYYResR  );
    FreeYYRes (lpYYResInd);
    FreeYYRes (lpYYResSub);

    return bRet;
} // End ArrayIndexFcnSet_EM
#undef  APPEND_NAME


//***************************************************************************
//  $FillTempApv
//
//  Fill in the missing values in the temporary APV
//***************************************************************************

void FillTempApv
    (HGLOBAL   hGlbNam,             // Name arg gobal memory handle
     LPAPLLIST lpMemLst,            // Name arg global memory handle
     APLNELM   aplNELMLst)          // Length of the list

{
    LPVARARRAY_HEADER lpMemHdrNam = NULL;   // Ptr to name header
    LPAPLDIM          lpMemDimNam;          // Ptr to named var dimensions
    LPVARARRAY_HEADER lpMemHdrTmp = NULL;
    LPAPLAPA          lpMemApvTmp;
    HGLOBAL           hGlbTmp;              // Temporary APV global memory handle
    UBOOL             bQuadIO;              // []IO
    APLUINT           uLst;                 // Loop counter

    // Lock the memory to get a ptr to it
    lpMemHdrNam = MyGlobalLockVar (hGlbNam);

    // Skip to the dimensions
    lpMemDimNam = VarArrayBaseToDim (lpMemHdrNam);

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    Assert (aplNELMLst EQ lpMemHdrNam->Rank);

    // Loop through the elements of the list arg
    for (uLst = 0; uLst < aplNELMLst; uLst++)
    {
        Assert (lpMemLst[uLst].tkFlags.TknType NE TKT_LISTSEP);

        // If this token is a temporary APV, ...
        if (lpMemLst[uLst].tkFlags.bTempAPV)
        {
            // Fill in the missing values

            // Get the global handle
            hGlbTmp = lpMemLst[uLst].tkData.tkGlbData;

            // Lock the memory to get a ptr to it
            lpMemHdrTmp = MyGlobalLockVar (hGlbTmp);

            // Skip over the header and dimensions to the data
            lpMemApvTmp = VarArrayDataFmBase (lpMemHdrTmp);

            // Fill in the NELM, dimension, Offset, and Multiplier
            lpMemHdrTmp->NELM                =
            *VarArrayBaseToDim (lpMemHdrTmp) = lpMemDimNam[uLst];
            lpMemApvTmp->Off                 = bQuadIO;
            lpMemApvTmp->Mul                 = 1;

            // We no longer need this ptr
            MyGlobalLock (hGlbTmp); lpMemHdrTmp = NULL;
        } // End IF
    } // End FOR

    // We no longer need this ptr
    MyGlobalUnlock (hGlbNam); lpMemHdrNam = NULL;
} // End FillTempApv


//***************************************************************************
//  End of File: pf_index.c
//***************************************************************************
