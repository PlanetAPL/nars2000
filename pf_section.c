//***************************************************************************
//  NARS2000 -- Primitive Function -- Section
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
//  $PrimFnSection_EM_YY
//
//  Primitive function for monadic and dyadic Section (ERROR and "Symmetric Difference")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnSection_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnSection_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_SECTION);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonSection_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydSection_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnSection_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnSection_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic Section
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnSection_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnSection_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnSection_EM_YY,    // Ptr to primitive function routine
                                    lptkLftArg,             // Ptr to left arg token
                                    lptkFunc,               // Ptr to function token
                                    lptkRhtArg,             // Ptr to right arg token
                                    lptkAxis);              // Ptr to axis token (may be NULL)
} // End PrimProtoFnSection_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimIdentFnSection_EM_YY
//
//  Generate an identity element for the primitive function dyadic Section
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimIdentFnSection_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimIdentFnSection_EM_YY
    (LPTOKEN lptkRhtOrig,           // Ptr to original right arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLRANK      aplRankRht;        // Right arg rank
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

    // The (right) identity function for dyadic Section
    //   (L {section} R) ("synmetric difference") is
    //   {zilde}.

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, NULL, NULL, &aplRankRht, NULL);

    // Check for RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto DOMAIN_EXIT;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbZilde);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimIdentFnSection_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonSection_EM_YY
//
//  Primitive function for monadic Section (ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonSection_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonSection_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End PrimFnMonSection_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydSection_EM_YY
//
//  Primitive function for dyadic Section ("Symmetric Difference")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSection_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydSection_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLRANK      aplRankLft,            // Left arg rank
                 aplRankRht;            // Right ...
    HGLOBAL      hGlbMFO;               // Magic function/operator global memory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory

    //***************************************************************
    // The derived functions from this operator are not sensitive
    //   to the axis operator, so signal a syntax error if present
    //***************************************************************

    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, NULL, NULL, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, NULL, NULL, &aplRankRht, NULL);

    // Check for LEFT/RIGHT RANK ERRORs
    if (IsMultiRank (aplRankLft))
        goto LEFT_RANK_EXIT;

    if (IsMultiRank (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the magic function/operator global memory handle
    hGlbMFO = lpMemPTD->hGlbMFO[MFOE_SD];

    //  Use an internal magic function.
    return
      ExecuteMagicFunction_EM_YY (lptkLftArg,           // Ptr to left arg token
                                  lptkFunc,             // Ptr to left operand function token
                                  NULL,                 // Ptr to function strand
                                  lptkRhtArg,           // Ptr to right arg token
                                  lptkAxis,             // Ptr to axis token
                                  hGlbMFO,              // Magic function/operator global memory handle
                                  NULL,                 // Ptr to HSHTAB struc (may be NULL)
                                  LINENUM_ONE);         // Starting line # type (see LINE_NUMS)
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    return NULL;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    return NULL;
} // End PrimFnDydSection_EM_YY


//***************************************************************************
//  Magic function for dyadic Section
//
//  Dyadic Section -- Symmetric Difference
//
//  Return the elements in (L~R),R~L.
//***************************************************************************

#include "mf_section.h"


//***************************************************************************
//  End of File: pf_section.c
//***************************************************************************
