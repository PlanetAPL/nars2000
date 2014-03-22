//***************************************************************************
//  NARS2000 -- Primitive Function -- RightTack
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
//  $PrimFnRightTack_EM_YY
//
//  Primitive function for monadic and dyadic RightTack ("right" and "right")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnRightTack_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnRightTack_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_RIGHTTACK);

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
        return PrimFnMonRightTack_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydRightTack_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimFnRightTack_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnRightTack_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic RightTack
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnRightTack_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnRightTack_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnRightTack_EM_YY,  // Ptr to primitive function routine
                                    lptkLftArg,             // Ptr to left arg token
                                    lptkFunc,               // Ptr to function token
                                    lptkRhtArg,             // Ptr to right arg token
                                    lptkAxis);              // Ptr to axis token (may be NULL)
} // End PrimProtoFnRightTack_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonRightTack_EM_YY
//
//  Primitive function for monadic RightTack ("right")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonRightTack_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonRightTack_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnDydTackCommon_YY (lptkRhtArg, lptkFunc);
} // End PrimFnMonRightTack_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydRightTack_EM_YY
//
//  Primitive function for dyadic RightTack ("right")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydRightTack_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydRightTack_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnDydTackCommon_YY (lptkRhtArg, lptkFunc);
} // End PrimFnRightTack_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydTackCommon_YY
//
//  Common subroutine to PrimFnDydUpTack_EM_YY & PrimFnDydDownTack_EM_YY
//***************************************************************************

LPPL_YYSTYPE PrimFnDydTackCommon_YY
    (LPTOKEN lptkArg,               // Ptr to arg token
     LPTOKEN lptkFunc)              // Ptr to function token

{
    APLSTYPE     aplType;           // Arg storage type
    APLLONGEST   aplLongest;        // Arg immediate value
    HGLOBAL      hGlbArg;           // Arg global memory handle
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    // Get the attributes (Type)
    //   of the arg
    AttrsOfToken (lptkArg, &aplType, NULL, NULL, NULL);

    // Get arg global ptr
    aplLongest = GetGlbPtrs (lptkArg, &hGlbArg);

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();
    lpYYRes->tkToken.tkCharIndex           = lptkFunc->tkCharIndex;

    // If the arg is a global, ...
    if (hGlbArg)
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = CopySymGlbDirAsGlb (hGlbArg);
////////lpYYRes->tkToken.tkCharIndex       =                // Already set
    } else
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = TranslateArrayTypeToImmType (aplType);
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkLongest  = aplLongest;
////////lpYYRes->tkToken.tkCharIndex       =                // Already set
    } // End IF/ELSE

    return lpYYRes;
} // End PrimFnDydTackCommon_YY


//***************************************************************************
//  End of File: pf_rtack.c
//***************************************************************************
