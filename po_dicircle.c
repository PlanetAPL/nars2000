//***************************************************************************
//  NARS2000 -- Primitive Operator -- DieresisCircle
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2011 Sudley Place Software

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
//  $PrimOpDieresisCircle_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    dyadic operator DieresisCircle (ERROR and "composition")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDieresisCircle_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpDieresisCircle_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_DIERESISCIRCLE);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (&lpYYFcnStrOpr->tkToken APPEND_NAME_ARG);

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return
          PrimOpMonDieresisCircle_EM_YY (lpYYFcnStrOpr, // Ptr to operator function strand
                                         lptkRhtArg,    // Ptr to right arg token
                                         FALSE);        // TRUE iff prototyping
    else
        return
          PrimOpDydDieresisCircle_EM_YY (lptkLftArg,    // Ptr to left arg token
                                         lpYYFcnStrOpr, // Ptr to operator function strand
                                         lptkRhtArg,    // Ptr to right arg token
                                         FALSE);        // TRUE iff prototyping
} // End PrimOpDieresisCircle_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoOpDieresisCircle_EM_YY
//
//  Generate a prototype for the derived functions from DieresisCircle ("composition")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoOpDieresisCircle_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoOpDieresisCircle_EM_YY
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
        return
          PrimOpMonDieresisCircle_EM_YY (lpYYFcnStrOpr, // Ptr to operator function strand
                                         lptkRhtArg,    // Ptr to right arg token
                                         TRUE);         // TRUE iff prototyping
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************
        return
          PrimOpDydDieresisCircle_EM_YY (lptkLftArg,    // Ptr to left arg token
                                         lpYYFcnStrOpr, // Ptr to operator function strand
                                         lptkRhtArg,    // Ptr to right arg token
                                         TRUE);         // TRUE iff prototyping
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimProtoOpDieresisCircle_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpMonDieresisCircle_EM_YY
//
//  Primitive operator for monadic derived function from DieresisCircle (ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpMonDieresisCircle_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpMonDieresisCircle_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     UBOOL        bPrototyping)         // TRUE iff prototyping

{
    return PrimFnValenceError_EM (&lpYYFcnStrOpr->tkToken APPEND_NAME_ARG);
} // End PrimOpDieresisCircle_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpDydDieresisCircle_EM_YY
//
//  Primitive operator for dyadic derived function from DieresisCircle ("composition")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydDieresisCircle_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpDydDieresisCircle_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     UBOOL        bPrototyping)         // TRUE iff prototyping

{
    LPPL_YYSTYPE lpYYFcnStrLft,         // Ptr to left operand function strand
                 lpYYFcnStrRht,         // Ptr to right ...
                 lpYYRes,               // Ptr to the result
                 lpYYResL = NULL,       // Ptr to left temporary result
                 lpYYResR = NULL;       // Ptr to right ...
    LPPRIMFNS    lpPrimProtoLft;        // Ptr to left operand prototype function
    LPPRIMFNS    lpPrimProtoRht;        // Ptr to right ...
    LPTOKEN      lptkAxis,              // Ptr to axis token (may be NULL)
                 lptkAxisLft,           // Ptr to left operand axis token (may be NULL)
                 lptkAxisRht;           // Ptr to right ...

    // Check for axis operator
    lptkAxis = CheckAxisOper (lpYYFcnStrOpr);

    //***************************************************************
    // The derived functions from this operator are not sensitive to
    //   the axis operator, so signal a syntax error if present
    //***************************************************************

    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Set ptr to left & right operands,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStrOpr[1 + (lptkAxis NE NULL)];
    lpYYFcnStrRht = &lpYYFcnStrLft[lpYYFcnStrLft->TknCount];

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken)
     || IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_OPERAND_SYNTAX_EXIT;

    // Ensure the right operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrRht->tkToken)
     || IsTknFillJot (&lpYYFcnStrRht->tkToken))
        goto RIGHT_OPERAND_SYNTAX_EXIT;

    // Get a ptr to the left & right prototype function
    if (bPrototyping)
    {
       // Get the appropriate prototype function ptrs
       lpPrimProtoLft = GetPrototypeFcnPtr (&lpYYFcnStrLft->tkToken);
       lpPrimProtoRht = GetPrototypeFcnPtr (&lpYYFcnStrRht->tkToken);

        if (!lpPrimProtoLft)
            goto LEFT_OPERAND_NONCE_EXIT;

        if (!lpPrimProtoRht)
            goto RIGHT_OPERAND_NONCE_EXIT;
    } // End IF

    // Check for axis operator in the left operand
    if (lpYYFcnStrLft->TknCount > 1
     && (lpYYFcnStrLft[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
      || lpYYFcnStrLft[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
        lptkAxisLft = &lpYYFcnStrLft[1].tkToken;
    else
        lptkAxisLft = NULL;

    // Check for axis operator in the right operand
    if (lpYYFcnStrRht->TknCount > 1
     && (lpYYFcnStrRht[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
      || lpYYFcnStrRht[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
        lptkAxisRht = &lpYYFcnStrRht[1].tkToken;
    else
        lptkAxisRht = NULL;

    // The definition of this operator is
    //   (g L) f g R

    // Execute the right operand monadically
    //   on the right arg (g R)
    if (bPrototyping)
        // Note that we cast the function strand to LPTOKEN
        //   to bridge the two types of calls -- one to a primitive
        //   function which takes a function token, and one to a
        //   primitive operator which takes a function strand
        lpYYResR = (*lpPrimProtoRht) (NULL,                     // Ptr to left arg token
                            (LPTOKEN) lpYYFcnStrRht,            // Ptr to right operand function strand
                                      lptkRhtArg,               // Ptr to right arg token
                                      lptkAxisRht);             // Ptr to right operand axis token (may be NULL)
    else
        lpYYResR = ExecFuncStr_EM_YY (NULL,                     // Ptr to left arg token
                                      lpYYFcnStrRht,            // Ptr to right operand function strand
                                      lptkRhtArg,               // Ptr to right arg token
                                      lptkAxisRht);             // Ptr to right operand axis token (may be NULL)
    // Check for error
    if (!lpYYResR)
        goto RIGHT_ERROR_EXIT;

    // Execute the right operand monadically
    //   on the left arg (g L)
    if (bPrototyping)
        // Note that we cast the function strand to LPTOKEN
        //   to bridge the two types of calls -- one to a primitive
        //   function which takes a function token, and one to a
        //   primitive operator which takes a function strand
        lpYYResL = (*lpPrimProtoRht) (NULL,                 // Ptr to left arg token
                            (LPTOKEN) lpYYFcnStrRht,        // Ptr to right operand function strand
                                      lptkLftArg,           // Ptr to right arg token
                                      lptkAxisRht);         // Ptr to right operand axis token (may be NULL)
    else
        lpYYResL = ExecFuncStr_EM_YY (NULL,                 // Ptr to left arg token
                                      lpYYFcnStrRht,        // Ptr to right operand function strand
                                      lptkLftArg,           // Ptr to right arg token
                                      lptkAxisRht);         // Ptr to right operand axis token (may be NULL)
    // Check for error
    if (!lpYYResL)
        goto LEFT_ERROR_EXIT;

    // Execute the left operand dyadically
    //   between the above two results (g L) f g R
    if (bPrototyping)
        // Note that we cast the function strand to LPTOKEN
        //   to bridge the two types of calls -- one to a primitive
        //   function which takes a function token, and one to a
        //   primitive operator which takes a function strand
        lpYYRes = (*lpPrimProtoLft) (&lpYYResL->tkToken,    // Ptr to left arg token
                            (LPTOKEN) lpYYFcnStrLft,        // Ptr to left operand function strand
                                     &lpYYResR->tkToken,    // Ptr to right arg token
                                      lptkAxisLft);         // Ptr to left operand axis token (may be NULL)
    else
        lpYYRes = ExecFuncStr_EM_YY (&lpYYResL->tkToken,    // Ptr to left arg token
                                      lpYYFcnStrLft,        // Ptr to left operand function strand
                                     &lpYYResR->tkToken,    // Ptr to right arg token
                                      lptkAxisLft);         // Ptr to left operand axis token (may be NULL)
    // Free the left & right YYRes
    FreeResult (lpYYResL); YYFree (lpYYResL); lpYYResL = NULL;
    FreeResult (lpYYResR); YYFree (lpYYResR); lpYYResR = NULL;

    return lpYYRes;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;

LEFT_OPERAND_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    return NULL;

RIGHT_OPERAND_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
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

LEFT_ERROR_EXIT:
    // Free the right YYRes
    FreeResult (lpYYResR); YYFree (lpYYResR); lpYYResR = NULL;
RIGHT_ERROR_EXIT:
    return NULL;
} // End PrimOpDydDieresisCircle_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: po_dicircle.c
//***************************************************************************
