//***************************************************************************
//  NARS2000 -- System Function -- Quad ERROR
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
//  $SysFnERROR_EM_YY
//
//  System function:  []ERROR -- Error Signal
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnERROR_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnERROR_EM_YY
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
        return SysFnMonERROR_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydERROR_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnERROR_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonERROR_EM_YY
//
//  Monadic []ERROR -- Error Signal
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonERROR_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonERROR_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeRht;        // Right arg storage type
    APLNELM      aplNELMRht;        // Right arg NELM
    APLRANK      aplRankRht;        // Right arg Rank
    APLLONGEST   aplLongestRht;     // Right arg longest if immediate
    HGLOBAL      hGlbRht = NULL;    // Right arg global memory handle
    LPVOID       lpMemRht = NULL;   // Ptr to right arg global memory
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to result

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RIGHT RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RANK_EXIT;

    // Check for DOMAIN ERROR
    if (!IsSimpleNH (aplTypeRht)
     || ((!IsSimpleChar (aplTypeRht))
      && !IsEmpty (aplNELMRht)))
        goto DOMAIN_EXIT;

    // If the right arg is empty, return NoValue
    if (IsEmpty (aplNELMRht))
        lpYYRes = MakeNoValue_YY (lptkFunc);
    else
    {
        // Get ptr to PerTabData global memory
        lpMemPTD = GetMemPTD ();

        // Get right arg's global ptrs
        aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

        // If the message is a global, ...
        if (hGlbRht)
        {
            // Skip over the header and dimensions to the data
            lpMemRht = VarArrayDataFmBase (lpMemRht);

            // Copy the error message to temporary storage
            CopyMemoryW (lpMemPTD->lpwszQuadErrorMsg, lpMemRht, (APLU3264) aplNELMRht);
        } else
            lpMemPTD->lpwszQuadErrorMsg[0] = (APLCHAR) aplLongestRht;

        // Ensure properly terminated
        lpMemPTD->lpwszQuadErrorMsg[aplNELMRht] = WC_EOS;

        // Save in PTD -- note that the tkCharIndex in the
        //   function token passed here isn't used unless this is
        //   immediate execution mode; normally, the tkCharIndex of the
        //   caller's is used.
        ErrorMessageIndirectToken (lpMemPTD->lpwszQuadErrorMsg, lptkFunc);
        lpMemPTD->tkErrorCharIndex = lptkFunc->tkCharIndex;

        // Set the reset flag
        lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_QUADERROR_INIT;
    } // End IF/ELSE

    goto NORMAL_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    // We no longer need this ptr
    if (hGlbRht && lpMemRht)
    {
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnMonERROR_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydERROR_EM_YY
//
//  Dyadic []ERROR -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydERROR_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydERROR_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End SysFnDydERROR_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_error.c
//***************************************************************************
