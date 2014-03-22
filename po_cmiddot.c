//***************************************************************************
//  NARS2000 -- Primitive Operator -- CircleMiddleDot
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
//  $PrimOpCircleMiddleDot_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    monadic operator CircleMiddleDot ("null op" and "null op")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpCircleMiddleDot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpCircleMiddleDot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_CIRCLEMIDDLEDOT);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (&lpYYFcnStrOpr->tkToken APPEND_NAME_ARG);

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return PrimOpMonCircleMiddleDot_EM_YY (lpYYFcnStrOpr,   // Ptr to operator function strand
                                               lptkRhtArg);     // Ptr to right arg
    else
        return PrimOpDydCircleMiddleDot_EM_YY (lptkLftArg,      // Ptr to left arg token
                                               lpYYFcnStrOpr,   // Ptr to operator function strand
                                               lptkRhtArg);     // Ptr to right arg token
} // End PrimOpCircleMiddleDot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoOpCircleMiddleDot_EM_YY
//
//  Generate a prototype for the derived functions from
//    monadic operator CircleMiddleDot ("duplicate" and "commute")
//***************************************************************************

LPPL_YYSTYPE PrimProtoOpCircleMiddleDot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxis)             // Ptr to axis token always NULL)

{
    Assert (lptkAxis EQ NULL);

    // If left arg is not present, ...
    if (lptkLftArg EQ NULL)
        //***************************************************************
        // Called monadically
        //***************************************************************
        return PrimOpMonCircleMiddleDotCommon_EM_YY (lpYYFcnStrOpr, // Ptr to operator function strand
                                                     lptkRhtArg,    // Ptr to right arg token
                                                     TRUE);         // TRUE iff prototyping
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************
        return PrimOpDydCircleMiddleDotCommon_EM_YY (lptkLftArg,    // Ptr to left arg token
                                                     lpYYFcnStrOpr, // Ptr to operator function strand
                                                     lptkRhtArg,    // Ptr to right arg token
                                                     TRUE);         // TRUE iff prototyping
} // End PrimProtoOpCircleMiddleDot_EM_YY


//***************************************************************************
//  $PrimOpMonCircleMiddleDot_EM_YY
//
//  Primitive operator for monadic derived function from CircleMiddleDot ("duplicate")
//***************************************************************************

LPPL_YYSTYPE PrimOpMonCircleMiddleDot_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpMonCircleMiddleDotCommon_EM_YY (lpYYFcnStrOpr, // Ptr to operator function strand
                                                 lptkRhtArg,    // Ptr to right arg token
                                                 FALSE);        // TRUE iff prototyping
} // End PrimOpMonCircleMiddleDot_EM_YY


//***************************************************************************
//  $PrimOpMonCircleMiddleDot_EM_YY
//
//  Primitive operator for monadic derived function from CircleMiddleDot ("duplicate")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpMonCircleMiddleDotCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpMonCircleMiddleDotCommon_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     UBOOL        bPrototyping)         // TRUE if prototyping

{
    LPPL_YYSTYPE lpYYFcnStrLft;         // Ptr to function strand
    LPPRIMFNS    lpPrimProtoLft;        // Ptr to left operand prototype function
    LPTOKEN      lptkAxisOpr;           // Ptr to operator axis token (may be NULL)

    // Check for axis operator
    lptkAxisOpr = CheckAxisOper (lpYYFcnStrOpr);

    //***************************************************************
    // The derived functions from this operator are not sensitive
    //   to the axis operator, so signal a syntax error if present
    //***************************************************************

    if (lptkAxisOpr NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStrOpr[1 + (lptkAxisOpr NE NULL)];

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken)
     || IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_OPERAND_SYNTAX_EXIT;

    // Split cases depending on whether or not we're prototyping
    if (bPrototyping)
    {
        // Get the appropriate prototype function ptr
        lpPrimProtoLft = GetPrototypeFcnPtr (&lpYYFcnStrLft->tkToken);
        if (!lpPrimProtoLft)
            goto LEFT_OPERAND_NONCE_EXIT;

        // Execute the function monadically on the right arg
        // Note that we cast the function strand to LPTOKEN
        //   to bridge the two types of calls -- one to a primitive
        //   function which takes a function token, and one to a
        //   primitive operator which takes a function strand
        return (*lpPrimProtoLft) (NULL,             // Ptr to left arg token
                        (LPTOKEN) lpYYFcnStrLft,    // Ptr to left operand fnuction strand
                                  lptkRhtArg,       // Ptr to right arg token
                                  lptkAxisOpr);     // Ptr to operator axis token
    } else
        // Execute the function monadically on the right arg
        return ExecFuncStr_EM_YY (NULL,             // Ptr to left arg token
                                  lpYYFcnStrLft,    // Ptr to left operand function strand
                                  lptkRhtArg,       // Ptr to right arg token
                                  lptkAxisOpr);     // Ptr to operator axis token
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxisOpr);
    return NULL;

LEFT_OPERAND_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    return NULL;

LEFT_OPERAND_NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    return NULL;
} // End PrimOpMonCircleMiddleDotCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpDydCircleMiddleDot_EM_YY
//
//  Primitive operator for dyadic derived function from CircleMiddleDot ("commute")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydCircleMiddleDot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpDydCircleMiddleDotCommon_EM_YY (lptkLftArg,    // Ptr to left arg token
                                                 lpYYFcnStrOpr, // Ptr to operator function strand
                                                 lptkRhtArg,    // Ptr to right arg token
                                                 FALSE);        // TRUE iff prototyping
} // End PrimOpDydCircleMiddleDot_EM_YY


//***************************************************************************
//  $PrimOpDydCircleMiddleDotCommon_EM_YY
//
//  Primitive operator for dyadic derived function from CircleMiddleDot ("commute")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydCircleMiddleDotCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpDydCircleMiddleDotCommon_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     UBOOL        bPrototyping)         // TRUE iff prototyping

{
    LPPL_YYSTYPE lpYYFcnStrLft;         // Ptr to left operand function strand
    LPPRIMFNS    lpPrimProtoLft;        // Ptr to left operand prototype function
    LPTOKEN      lptkAxisOpr;           // Ptr to operator axis token (may be NULL)

    // Check for axis operator
    lptkAxisOpr = CheckAxisOper (lpYYFcnStrOpr);

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStrOpr[1 + (lptkAxisOpr NE NULL)];

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken)
     || IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_OPERAND_SYNTAX_EXIT;

    // Split cases depending on whether or not we're prototyping
    if (bPrototyping)
    {
        // Get the appropriate prototype function ptr
        lpPrimProtoLft = GetPrototypeFcnPtr (&lpYYFcnStrLft->tkToken);
        if (!lpPrimProtoLft)
            goto LEFT_OPERAND_NONCE_EXIT;

        // Execute the function dyadically between the two args
        // Note that we cast the function strand to LPTOKEN
        //   to bridge the two types of calls -- one to a primitive
        //   function which takes a function token, and one to a
        //   primitive operator which takes a function strand
        return (*lpPrimProtoLft) (lptkLftArg,       // Ptr to left arg token
                        (LPTOKEN) lpYYFcnStrLft,    // Ptr to left operand function strand
                                  lptkRhtArg,       // Ptr to right arg token
                                  lptkAxisOpr);     // Ptr to operator axis token
    } else
        // Execute the function dyadically between the two args
        return ExecFuncStr_EM_YY (lptkLftArg,   // Ptr to left arg token
                                  lpYYFcnStrLft,// Ptr to left operand function strand
                                  lptkRhtArg,   // Ptr to right arg token
                                  lptkAxisOpr);     // Ptr to operator axis token
LEFT_OPERAND_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    return NULL;

LEFT_OPERAND_NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    return NULL;
} // End PrimOpDydCircleMiddleDotCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: po_cmiddot.c
//***************************************************************************
