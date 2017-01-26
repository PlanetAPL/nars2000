//***************************************************************************
//  NARS2000 -- Primitive Operator -- Jot
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
//  $PrimOpJot_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    dyadic operator Jot ("compose" and "compose")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpJot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpJot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_JOT);

    return
      PrimOpJotCommon_EM_YY (lptkLftArg,    // Ptr to left arg token (may be NULL if niladic/monadic)
                             lpYYFcnStrOpr, // Ptr to operator function strand
                             lptkRhtArg,    // Ptr to right arg token (may be NULL if niladic)
                             FALSE);        // TRUE iff prototyping
} // End PrimOpJot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoOpJot_EM_YY
//
//  Generate a prototype for the derived functions from Jot ("compose")
//***************************************************************************

LPPL_YYSTYPE PrimProtoOpJot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStr,           // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxis)             // Ptr to axis token always NULL)

{
    Assert (lptkAxis EQ NULL);

    return
      PrimOpJotCommon_EM_YY (lptkLftArg,    // Ptr to left arg token
                             lpYYFcnStr,    // Ptr to operator function strand
                             lptkRhtArg,    // Ptr to right arg token
                             TRUE);         // TRUE iff prototyping
} // End PrimProtoOpJot_EM_YY


//***************************************************************************
//  $PrimOpJotCommon_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    dyadic operator Jot ("compose" and "compose")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpJotCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpJotCommon_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if niladic/monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token (may be NULL if niladic)
     UBOOL        bPrototyping)         // TRUE iff prototyping

{
    LPPL_YYSTYPE lpYYFcnStrLft,         // Ptr to left operand function strand
                 lpYYFcnStrRht,         // Ptr to right ...
                 lpYYRes,               // Ptr to the result
                 lpYYRes2;              // Ptr to secondary result
    UBOOL        bLftOpr,               // TRUE iff left operand is a function/operator
                 bRhtOpr;               //          right ...
    LPPRIMFNS    lpPrimProtoLft;        // Ptr to left operand prototype function
    LPPRIMFNS    lpPrimProtoRht;        // Ptr to right ...
    LPTOKEN      lptkAxisOpr,           // Ptr to axis token (may be NULL)
                 lptkAxisLft,           // Ptr to left operand axis token (may be NULL)
                 lptkAxisRht;           // Ptr to right ...

    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_JOT);

    // Check for axis operator
    lptkAxisOpr = CheckAxisOper (lpYYFcnStrOpr);

    //***************************************************************
    // The derived functions from this operator are not sensitive to
    //   the axis operator, so signal a syntax error if present
    //***************************************************************

    if (lptkAxisOpr NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Set ptr to left & right operands,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrRht = GetDydRhtOper (lpYYFcnStrOpr, lptkAxisOpr);
    lpYYFcnStrLft = GetDydLftOper (lpYYFcnStrRht);

    if (IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_OPERAND_DOMAIN_EXIT;

    if (IsTknFillJot (&lpYYFcnStrRht->tkToken))
        goto RIGHT_OPERAND_DOMAIN_EXIT;

    // Test for fcn/opr vs. var
    bLftOpr = IsTknFcnOpr (&lpYYFcnStrLft->tkToken);
    bRhtOpr = IsTknFcnOpr (&lpYYFcnStrRht->tkToken);

    // Get a ptr to the prototype function for the left operand
    if (bPrototyping && bLftOpr)
    {
        // Get the appropriate prototype function ptr
        lpPrimProtoLft = GetPrototypeFcnPtr (&lpYYFcnStrLft->tkToken);
        if (lpPrimProtoLft EQ NULL)
            goto LEFT_OPERAND_NONCE_EXIT;
    } else
        lpPrimProtoLft = NULL;

    // Get a ptr to the prototype function for the right operand
    if (bPrototyping && bRhtOpr)
    {
        // Get the appropriate prototype function ptr
        lpPrimProtoRht = GetPrototypeFcnPtr (&lpYYFcnStrRht->tkToken);
        if (lpPrimProtoRht EQ NULL)
            goto RIGHT_OPERAND_NONCE_EXIT;
    } else
        lpPrimProtoRht = NULL;

    // Handle both monadic & dyadic derived functions together

    // Determine if the left & right arg tokens are functions/operators
    // Check for axis operator in the left operand
    if (bLftOpr
     && lpYYFcnStrLft->TknCount > 1
     && IsTknTypeAxis (lpYYFcnStrLft[1].tkToken.tkFlags.TknType))
        lptkAxisLft = &lpYYFcnStrLft[1].tkToken;
    else
        lptkAxisLft = NULL;

    // Check for axis operator in the right operand
    if (bRhtOpr
     && lpYYFcnStrRht->TknCount > 1
     && IsTknTypeAxis (lpYYFcnStrRht[1].tkToken.tkFlags.TknType))
        lptkAxisRht = &lpYYFcnStrRht[1].tkToken;
    else
        lptkAxisRht = NULL;

    // If we're called niladically, ...
    if (lptkRhtArg EQ NULL)
        goto VALENCE_EXIT;

    // Split cases based upon the type (V or F) of
    //   the left and right operands
    switch (bLftOpr * 2 + bRhtOpr * 1)
    {
        case 1 * 2 + 1 * 1:     // F1 Jot F2 -> F1 F2 R or L F1 F2 R
            // Execute the right operand monadically
            //   on the right arg
            if (bPrototyping)
                // Note that we cast the function strand to LPTOKEN
                //   to bridge the two types of calls -- one to a primitive
                //   function which takes a function token, and one to a
                //   primitive operator which takes a function strand
                lpYYRes2 = (*lpPrimProtoRht) (NULL,                     // Ptr to left arg token (may be NULL if niladic/monadic)
                                    (LPTOKEN) lpYYFcnStrRht,            // Ptr to right operand function strand
                                              lptkRhtArg,               // Ptr to right arg token (may be NULL if niladic)
                                              lptkAxisRht);             // Ptr to right operand axis token (may be NULL)
            else
                lpYYRes2 = ExecFuncStr_EM_YY (NULL,                     // Ptr to left arg token (may be NULL if niladic/monadic)
                                              lpYYFcnStrRht,            // Ptr to right operand function strand
                                              lptkRhtArg,               // Ptr to right arg token (may be NULL if niladic)
                                              lptkAxisRht);             // Ptr to right operand axis token (may be NULL)
            if (lpYYRes2)
            {
                // Execute the left operand dyadically
                //   between the (optional) left arg and the
                //   above result from the right operand.
                if (bPrototyping)
                    // Note that we cast the function strand to LPTOKEN
                    //   to bridge the two types of calls -- one to a primitive
                    //   function which takes a function token, and one to a
                    //   primitive operator which takes a function strand
                    lpYYRes = (*lpPrimProtoLft) (lptkLftArg,            // Ptr to left arg token (may be NULL if niladic/monadic)
                                       (LPTOKEN) lpYYFcnStrLft,         // Ptr to left operand function strand
                                                &lpYYRes2->tkToken,     // Ptr to right arg token (may be NULL if niladic)
                                                 lptkAxisLft);          // Ptr to left operand axis token (may be NULL)
                else
                    lpYYRes = ExecFuncStr_EM_YY (lptkLftArg,            // Ptr to left arg token (may be NULL if niladic/monadic)
                                                 lpYYFcnStrLft,         // Ptr to left operand function strand
                                                &lpYYRes2->tkToken,     // Ptr to right arg token (may be NULL if niladic)
                                                 lptkAxisLft);          // Ptr to left operand axis token (may be NULL)
                FreeResult (lpYYRes2); YYFree (lpYYRes2); lpYYRes2 = NULL;
            } else
                lpYYRes = NULL;

            break;

        case 0 * 2 + 1 * 1:     // V Jot F -> V F R or SYNTAX ERROR
            // If there's a left arg, signal a SYNTAX ERROR
            if (lptkLftArg)
                goto LEFT_SYNTAX_EXIT;

            // Execute the right operand dyadically
            //   between the left operand and the right arg.
            if (bPrototyping)
                // Note that we cast the function strand to LPTOKEN
                //   to bridge the two types of calls -- one to a primitive
                //   function which takes a function token, and one to a
                //   primitive operator which takes a function strand
                lpYYRes = (*lpPrimProtoRht) (&lpYYFcnStrLft->tkToken,   // Ptr to left arg token (may be NULL if niladic/monadic)
                                    (LPTOKEN) lpYYFcnStrRht,            // Ptr to right operand function strand
                                              lptkRhtArg,               // Ptr to right arg token (may be NULL if niladic)
                                              lptkAxisRht);             // Ptr to right operand axis token (may be NULL)
            else
                lpYYRes = ExecFuncStr_EM_YY (&lpYYFcnStrLft->tkToken,   // Ptr to left arg token (may be NULL if niladic/monadic)
                                              lpYYFcnStrRht,            // Ptr to right operand function strand
                                              lptkRhtArg,               // Ptr to right arg token (may be NULL if niladic)
                                              lptkAxisRht);             // Ptr to right operand axis token (may be NULL)
            break;

        case 1 * 2 + 0 * 1:     // F Jot V -> R F V
            // If there's a left arg, signal a SYNTAX ERROR
            if (lptkLftArg)
                goto LEFT_SYNTAX_EXIT;

            // Execute the left operand dyadically
            //   between the right arg and the right operand.
            if (bPrototyping)
                // Note that we cast the function strand to LPTOKEN
                //   to bridge the two types of calls -- one to a primitive
                //   function which takes a function token, and one to a
                //   primitive operator which takes a function strand
                lpYYRes = (*lpPrimProtoLft) (lptkRhtArg,                // Ptr to left arg token (may be NULL if niladic/monadic)
                                   (LPTOKEN) lpYYFcnStrLft,             // Ptr to left operand function strand
                                            &lpYYFcnStrRht->tkToken,    // Ptr to right arg token (may be NULL if niladic)
                                             lptkAxisLft);              // Ptr to left operand axis token (may be NULL)
            else
                lpYYRes = ExecFuncStr_EM_YY (lptkRhtArg,                // Ptr to left arg token (may be NULL if niladic/monadic)
                                             lpYYFcnStrLft,             // Ptr to left operand function strand
                                            &lpYYFcnStrRht->tkToken,    // Ptr to right arg token (may be NULL if niladic)
                                             lptkAxisLft);              // Ptr to left operand axis token (may be NULL)
            break;

        case 0 * 2 + 0 * 1:     // V op2 V -> SYNTAX ERROR
            goto LEFT_SYNTAX_EXIT;

        defstop
            break;
    } // End SWITCH

    return lpYYRes;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxisOpr);
    return NULL;

LEFT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    return NULL;

RIGHT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    return NULL;

LEFT_OPERAND_NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    return NULL;

RIGHT_OPERAND_NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    return NULL;

VALENCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALENCE_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    return NULL;

LEFT_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkLftArg);
    return NULL;
} // End PrimOpJotCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: po_jot.c
//***************************************************************************
