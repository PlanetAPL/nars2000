//***************************************************************************
//  NARS2000 -- Primitive Operator -- DieresisJot
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
#include "headers.h"


//***************************************************************************
//  $PrimOpDieresisJot_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    dyadic operator DieresisJot ("rank" and "rank")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDieresisJot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpDieresisJot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_DIERESISJOT);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (&lpYYFcnStrOpr->tkToken APPEND_NAME_ARG);

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return PrimOpMonDieresisJot_EM_YY (lpYYFcnStrOpr,   // Ptr to operator function strand
                                           lptkRhtArg);     // Ptr to right arg token
    else
        return PrimOpDydDieresisJot_EM_YY (lptkLftArg,      // Ptr to left arg token
                                           lpYYFcnStrOpr,   // Ptr to operator function strand
                                           lptkRhtArg);     // Ptr to right arg token
} // End PrimOpDieresisJot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoOpDieresisJot_EM_YY
//
//  Generate a prototype for the derived functions from
//    dyadic operator DieresisJot ("rank" and "rank")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoOpDieresisJot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoOpDieresisJot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxis)             // Ptr to axis token always NULL)

{
    if (lptkAxis EQ NULL)
        // Check for axis operator
        lptkAxis = CheckAxisOper (lpYYFcnStrOpr);

    //***************************************************************
    // The derived functions from this operator are not sensitive
    //   to the axis operator, so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // If left arg is not present, ...
    if (lptkLftArg EQ NULL)
        //***************************************************************
        // Called monadically
        //***************************************************************
        return PrimOpMonDieresisJotCommon_EM_YY (lpYYFcnStrOpr,     // Ptr to operator function strand
                                                 lptkRhtArg,        // Ptr to right arg token
                                                 TRUE);             // TRUE iff prototyping
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************
        return PrimOpDydDieresisJotCommon_EM_YY (lptkLftArg,        // Ptr to left arg token
                                                 lpYYFcnStrOpr,     // Ptr to operator function strand
                                                 lptkRhtArg,        // Ptr to right arg token
                                                 TRUE);             // TRUE iff prototyping
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimProtoOpDieresisJot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpMonDieresisJot_EM_YY
//
//  Primitive operator for monadic derived function from DieresisJot ("rank")
//***************************************************************************

LPPL_YYSTYPE PrimOpMonDieresisJot_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpMonDieresisJotCommon_EM_YY (lpYYFcnStrOpr,     // Ptr to operator function strand
                                             lptkRhtArg,        // Ptr to right arg token
                                             FALSE);            // TRUE iff prototyping
} // End PrimOpMonDieresisJot_EM_YY


//***************************************************************************
//  $PrimOpMonDieresisJotCommon_EM_YY
//
//  Primitive operator for monadic derived function from DieresisJot ("rank")
//***************************************************************************

LPPL_YYSTYPE PrimOpMonDieresisJotCommon_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     UBOOL        bPrototyping)         // TRUE iff protoyping

{
    LPPL_YYSTYPE lpYYFcnStrLft,         // Ptr to left operand function strand
                 lpYYFcnStrRht;         // Ptr to right ...

    // Set ptr to left & right operands,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStrOpr[1 + (NULL NE CheckAxisOper (lpYYFcnStrOpr))];
    lpYYFcnStrRht = &lpYYFcnStrLft[lpYYFcnStrLft->TknCount];

    return
      PrimOpDieresisJotCommon_EM_YY (NULL,              // Ptr to left arg token (may be NULL if monadic derived function)
                                     lpYYFcnStrLft,     // Ptr to left operand function strand
                                     lpYYFcnStrOpr,     // Ptr to operator function strand
                                     lpYYFcnStrRht,     // Ptr to right operand function strand
                                     lptkRhtArg,        // Ptr to right arg token
                                     bPrototyping);     // TRUE iff protoyping
} // End PrimOpMonDieresisJotCommon_EM_YY


//***************************************************************************
//  $PrimOpDieresisJotCommon_EM_YY
//
//  Primitive operator for monadic & dyadic derived function
//    from DieresisJot ("rank")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDieresisJotCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpDieresisJotCommon_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic derived function)
     LPPL_YYSTYPE lpYYFcnStrLft,        // Ptr to left operand function strand
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPPL_YYSTYPE lpYYFcnStrRht,        // Ptr to right operand function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     UBOOL        bPrototyping)         // TRUE iff protoyping

{
    HGLOBAL       hGlbMFO1,             // Magic function/operator #1 global memory handle
                  hGlbMFO2,             // Magic function/operator #2 global memory handle
                  hGlbOprRht;           // Right operand global memory handle
    LPPERTABDATA  lpMemPTD;             // Ptr to PerTabData global memory
    LPPL_YYSTYPE  lpYYRes = NULL;       // Ptr to result
    LPTOKEN       lptkAxis;             // Ptr to axis token
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Check for axis operator
    lptkAxis = CheckAxisOper (lpYYFcnStrOpr);

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken)
     || IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_OPERAND_SYNTAX_EXIT;

    // Ensure the right operand is a variable
    if (IsTknFcnOpr (&lpYYFcnStrRht->tkToken)
     || IsTknFillJot (&lpYYFcnStrRht->tkToken))
        goto RIGHT_OPERAND_SYNTAX_EXIT;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the magic function/operator global memory handles
    hGlbMFO1 = (lptkLftArg EQ NULL) ? lpMemPTD->hGlbMFO[MFOE_MonRank]
                                    : lpMemPTD->hGlbMFO[MFOE_DydRank];
    hGlbMFO2 = lpMemPTD->hGlbMFO[MFOE_Conform];

    // Get right operand global ptrs
    hGlbOprRht = GetGlbHandle (&lpYYFcnStrRht->tkToken);

    // If the right operand is not immediate, ...
    if (hGlbOprRht NE NULL)
    {
        APLSTYPE aplTypeOprRht;             // Right operand storage type
        APLNELM  aplNELMOprRht;             // Right operand NELM
        APLRANK  aplRankOprRht;             // Right operand rank

        // Get the attributes (Type, NELM, and Rank)
        //   of the right arg token
        AttrsOfGlb (hGlbOprRht, &aplTypeOprRht, &aplNELMOprRht, &aplRankOprRht, NULL);

        // Check for RIGHT OPERAND RANK ERROR
        if (1 < aplRankOprRht)
            goto RIGHT_OPERAND_RANK_EXIT;

        // Check for RIGHT OPERAND LENGTH ERROR
        if (1 > aplNELMOprRht
         ||     aplNELMOprRht > 3)
            goto RIGHT_OPERAND_LENGTH_EXIT;

        // Check for RIGHT OPERAND DOMAIN ERROR
        if (!IsNumeric (aplTypeOprRht))
            goto RIGHT_OPERAND_DOMAIN_EXIT;
    } else
    // Check for RIGHT OPERAND DOMAIN ERROR
    if (!IsImmNum (lpYYFcnStrRht->tkToken.tkFlags.ImmType))
        goto RIGHT_OPERAND_DOMAIN_EXIT;

    lpYYRes =
      ExecuteMagicOperator_EM_YY (lptkLftArg,               // Ptr to left arg token
                                 &lpYYFcnStrOpr->tkToken,   // Ptr to function token
                                  lpYYFcnStrLft,            // Ptr to left operand function strand
                                  lpYYFcnStrOpr,            // Ptr to function strand
                                  lpYYFcnStrRht,            // Ptr to right operand function strand (may be NULL)
                                  lptkRhtArg,               // Ptr to right arg token
                                  NULL,                     // Ptr to axis token
                                  hGlbMFO1,                 // Magic function/operator global memory handle
                                  NULL,                     // Ptr to HSHTAB struc (may be NULL)
                                  bPrototyping
                                ? LINENUM_PRO
                                : LINENUM_ONE);             // Starting line # type (see LINE_NUMS)
    // If the result is valid, finish the process with Conforming Disclose
    if (lpYYRes && !IsTknImmed (&lpYYRes->tkToken))
    {
        HGLOBAL     hGlbRes;                // Result global memory handle
        LPAPLNESTED lpMemRes;               // Ptr to result global memory
        APLSTYPE    aplTypeRes;             // Result storage type
        APLNELM     aplNELMRes;             // Result NELM
        APLRANK     aplRankRes;             // Result rank

        // Get result global ptrs
        hGlbRes = GetGlbHandle (&lpYYRes->tkToken);

        // Get the attributes (Type, NELM, and Rank)
        //   of the result global
        AttrsOfGlb (hGlbRes, &aplTypeRes, &aplNELMRes, &aplRankRes, NULL);

        // If the result is not simple, ...
        if (!IsSimple (aplTypeRes))
        {
            APLRANK      uMinRank,          // Minimum rank across all items
                         uMaxRank;          // maximum ...
            APLUINT      uRes;              // Loop counter
            APLRANK      aplRankItm;        // Result item rank
            LPPL_YYSTYPE lpYYRes2;          // Secondary result

            // Lock the memory to get a ptr to it
            lpMemRes = MyGlobalLock (hGlbRes);

            // Skip over the header to the data
            lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

            // Initialize minimum & maximum ranks
            uMinRank = NEG1A;
            uMaxRank = 0;

            // In case it's empty, we need to process the prototype
            aplNELMRes = max (aplNELMRes, 1);

            // Loop through the result looking for the minimum/maximum rank items
            for (uRes = 0; uRes < aplNELMRes; uRes++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (*lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Split cases based the item ptr type
                switch (GetPtrTypeDir (lpMemRes[uRes]))
                {
                    case PTRTYPE_STCONST:
                        // Get the minimum rank
                        uMinRank = min (uMinRank, 0);

                        break;

                    case PTRTYPE_HGLOBAL:
                        // Get the attributes (Type, NELM, and Rank)
                        //   of the result global item
                        AttrsOfGlb (lpMemRes[uRes], NULL, NULL, &aplRankItm, NULL);

                        // Get the minimum rank
                        uMinRank = min (uMinRank, aplRankItm);

                        // Get the maximum rank
                        uMaxRank = max (uMaxRank, aplRankItm);

                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR

            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

            // If the two ranks are unequal, ...
            if (uMinRank NE uMaxRank)
            {
                TOKEN tkLftArg = {0};   // Left arg token

                // Fill in the left arg token
                tkLftArg.tkFlags.TknType   = TKT_VARIMMED;
                tkLftArg.tkFlags.ImmType   = IMMTYPE_INT;
////////////////tkLftArg.tkFlags.NoDisplay = FALSE; // Already zero from = {0}
                tkLftArg.tkData.tkInteger  = uMaxRank;
////////////////tkLftArg.tkCharIndex       = 0;     // Already zero from = {0}
#ifdef DEBUG
                // Decrement the SI level of lpYYRes so YYResIsEmpty won't complain
                lpYYRes->SILevel--;
#endif
                // Finish it off with another magic function/operator
                lpYYRes2 =
                  ExecuteMagicFunction_EM_YY (&tkLftArg,                // Ptr to left arg token
                                              &lpYYFcnStrOpr->tkToken,  // Ptr to function token
                                               NULL,                    // Ptr to function strand
                                              &lpYYRes->tkToken,        // Ptr to right arg token
                                               NULL,                    // Ptr to axis token
                                               hGlbMFO2,                // Magic function/operator global memory handle
                                               NULL,                    // Ptr to HSHTAB struc (may be NULL)
                                               LINENUM_ONE);            // Starting line # type (see LINE_NUMS)
#ifdef DEBUG
                // Restore the SI level of lpYYRes
                lpYYRes->SILevel++;
#endif
                // Free the first YYRes
                FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;

                // Copy secondary result as primary result
                lpYYRes = lpYYRes2; lpYYRes2 = NULL;
            } // End IF

            // Just disclose the argument
            lpYYRes2 =
              PrimFnMonRightShoe_EM_YY (&lpYYFcnStrOpr->tkToken,    // Ptr to function token
                                        &lpYYRes->tkToken,          // Ptr to right arg token
                                         lptkAxis);                 // Ptr to axis token
            // Free the first YYRes
            FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;

            // Copy secondary result as primary result
            lpYYRes = lpYYRes2;
        } // End IF
    } // End IF

    goto NORMAL_EXIT;

LEFT_OPERAND_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

RIGHT_OPERAND_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

RIGHT_OPERAND_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

RIGHT_OPERAND_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

RIGHT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (lpYYRes)
    {
        // Free the first YYRes
        FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;
    } // End IF
NORMAL_EXIT:
    return lpYYRes;
} // End PrimOpDieresisJotCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  Magic function/operator for monadic derived function from the rank dyadic operator
//
//  This operator was originally based upon code found in the paper by J. Philip Benkhard,
//    "Extending structure, type, and expression in APL2", ACM SIGAPL APL Quote Quad,
//    v.21 n.4, p.20-38, Aug. 1991, but later was modified to reflect the
//    definition of the Rank operator in the ISO-IEC 13751 Extended APL Standard.
//***************************************************************************

//***************************************************************************
//  Magic function/operator for the Conform step for monadic/dyadic derived
//    function from the rank dyadic operator
//
//  This function is based upon code found in the paper by J. Philip Benkhard,
//    "Extending structure, type, and expression in APL2", ACM SIGAPL APL Quote Quad,
//    v.21 n.4, p.20-38, Aug. 1991.
//***************************************************************************

//***************************************************************************
//  Magic function/operator for dyadic derived function from the rank dyadic operator
//
//  This operator was originally based upon code found in the paper by J. Philip Benkhard,
//    "Extending structure, type, and expression in APL2", ACM SIGAPL APL Quote Quad,
//    v.21 n.4, p.20-38, Aug. 1991, but later was modified to reflect the
//    definition of the Rank operator in the ISO-IEC 13751 Extended APL Standard.
//***************************************************************************

#include "mf_dijot.h"



//***************************************************************************
//  $PrimOpDydDieresisJot_EM_YY
//
//  Primitive operator for dyadic derived function from DieresisJot ("rank")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydDieresisJot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpDydDieresisJotCommon_EM_YY (lptkLftArg,    // Ptr to left arg token
                                             lpYYFcnStrOpr, // Ptr to operator function strand
                                             lptkRhtArg,    // Ptr to right arg token
                                             FALSE);        // TRUE iff prototyping
} // End PrimOpDydDieresisJot_EM_YY


//***************************************************************************
//  $PrimOpDydDieresisJotCommon_EM_YY
//
//  Primitive operator for dyadic derived function from DieresisJot ("rank")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydDieresisJotCommon_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     UBOOL        bPrototyping)         // TRUE iff protoyping

{
    LPPL_YYSTYPE lpYYFcnStrLft,         // Ptr to left operand function strand
                 lpYYFcnStrRht;         // Ptr to right ...

    // Set ptr to left & right operands,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStrOpr[1 + (NULL NE CheckAxisOper (lpYYFcnStrOpr))];
    lpYYFcnStrRht = &lpYYFcnStrLft[lpYYFcnStrLft->TknCount];

    return
      PrimOpDieresisJotCommon_EM_YY (lptkLftArg,        // Ptr to left arg token (may be NULL if monadic derived function)
                                     lpYYFcnStrLft,     // Ptr to left operand function strand
                                     lpYYFcnStrOpr,     // Ptr to operator function strand
                                     lpYYFcnStrRht,     // Ptr to right operand function strand
                                     lptkRhtArg,        // Ptr to right arg token
                                     bPrototyping);     // TRUE iff protoyping
} // End PrimOpDydDieresisJotCommon_EM_YY


//***************************************************************************
//  End of File: po_dieresisjot.c
//***************************************************************************
