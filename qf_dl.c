//***************************************************************************
//  NARS2000 -- System Function -- Quad DL
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
//  $SysFnDL_EM_YY
//
//  System function:  []DL -- Delay
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDL_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDL_EM_YY
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
        return SysFnMonDL_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydDL_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnDL_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonDL_EM_YY
//
//  Monadic []DL -- Delay
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonDL_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonDL_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeRht;        // Right arg storage type
    APLNELM      aplNELMRht;        // Right arg NELM
    APLRANK      aplRankRht;        // Right arg rank
    APLFLOAT     aplFloatRht;       // Right arg float value
    DWORD        dwTickCount;       // The current tick count (time in millseconds since W was started)
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    HGLOBAL      lpSymGlbRht;       // Ptr to global numeric

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RANK_EXIT;

    // Check for LENGTH ERROR
    if (!IsSingleton (aplNELMRht))
        goto LENGTH_EXIT;

    // Check for DOMAIN ERROR
    if (!IsNumeric (aplTypeRht))
        goto DOMAIN_EXIT;

    // Get the one (and only) value from the right arg
    GetFirstValueToken (lptkRhtArg,         // Ptr to right arg token
                        NULL,               // Ptr to integer result
                       &aplFloatRht,        // Ptr to float ...
                        NULL,               // Ptr to WCHAR ...
                        NULL,               // Ptr to longest ...
                       &lpSymGlbRht,        // Ptr to lpSym/Glb ...
                        NULL,               // Ptr to ...immediate type ...
                        NULL);              // Ptr to array type ...
    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
        case ARRAY_FLOAT:
            break;

        case ARRAY_RAT:
            aplFloatRht = mpq_get_d ((LPAPLRAT) lpSymGlbRht);

            break;

        case ARRAY_VFP:
            aplFloatRht = mpfr_get_d ((LPAPLVFP) lpSymGlbRht, MPFR_RNDN);

            break;

        defstop
            break;
    } // End SWITCH

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Create a semaphore so we can be interrupted
    lpMemPTD->hSemaDelay =
      MyCreateSemaphoreW (NULL,         // No security attrs
                          0,            // Initial count (non-signalled)
                          64*1024,      // Maximum count
                          NULL);        // No name
    // Get the current tick count so we can subtract it from the next tick count
    dwTickCount = GetTickCount ();

    // Scale the value from float seconds to DWORD milliseconds
    //   and wait for that long
    MyWaitForSemaphore (lpMemPTD->hSemaDelay,
                (DWORD) (aplFloatRht * 1000),
                       L"SysFnMonDL_EM_YY");
    // Determine how long we waited
    dwTickCount = GetTickCount () - dwTickCount;

    // Close the semaphore handle as it is no longer needed
    MyCloseSemaphore (lpMemPTD->hSemaDelay); lpMemPTD->hSemaDelay = NULL;

    // Convert from DWORD milliseconds to float seconds
    aplFloatRht = ((APLFLOAT) dwTickCount) / 1000;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_FLOAT;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkFloat    = aplFloatRht;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End SysFnMonDL_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydDL_EM_YY
//
//  Dyadic []DL -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydDL_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydDL_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End SysFnDydDL_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_dl.c
//***************************************************************************
