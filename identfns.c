//***************************************************************************
//  NARS2000 -- Identity Functions
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
//  $GetIdentityElement_EM
//
//  Return the identity element for a given function and array
//
//  The idea for Identity Functions was taken from Brown/Jenkins
//    "APL Identity Crisis"
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- GetIdentityElement_EM"
#else
#define APPEND_NAME
#endif

GLBSYM GetIdentityElement_EM
    (LPPL_YYSTYPE lpYYFcnStrLft,    // Ptr to left operand function strand
     LPTOKEN      lptkLftArg,       // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrRht,    // Ptr to right operand function strand
     LPTOKEN      lptkRhtArg)       // Ptr to right arg token

{
    LPPL_YYSTYPE lpYYRes = NULL,    // Ptr to intermediate result
                 lpYYRes2 = NULL,   // Ptr to secondary result
                 lpYYResL = NULL,   // Ptr to left result
                 lpYYResL2 = NULL,  // Ptr to secondary left result
                 lpYYResR = NULL,   // Ptr to right result
                 lpYYResR2 = NULL;  // Ptr to secondary right result
    GLBSYM       hGlbSym = {NULL};  // Result
    LPPRIMFLAGS  lpPrimFlagsLft;    // Ptr to Left operand primitive flags
    LPTOKEN      lptkAxisLft,       // Ptr to left operand axis token
                 lptkAxisRht;       // ...    right ...
    TOKEN        tkFcn = {0};       // Function token

    Assert (lpYYFcnStrLft NE NULL);
    Assert (lptkRhtArg    NE NULL);

    // If the right operand is defined, ...
    if (lpYYFcnStrRht)
    {
        LPPRIMFNS lpPrimProtoRht;               // Ptr to right operand prototype function
        TOKEN     tkLft = tkZero;               // Boolean zero token

        // Setup a token with the Boolean scalar zero
////////tkLft.tkFlags.TknType   = TKT_VARIMMED;     // Already set from tkZero
////////tkLft.tkFlags.ImmType   = IMMTYPE_BOOL;     // Already set from tkZero
////////tkLft.tkFlags.NoDisplay = FALSE;            // Already set from tkZero
////////tkLft.tkData.tkBoolean  = 0;                // Already set from tkZero
        tkLft.tkCharIndex       = lpYYFcnStrRht->tkToken.tkCharIndex;

        // Setup a token with the {rho} function
        tkFcn.tkFlags.TknType   = TKT_FCNIMMED;
        tkFcn.tkFlags.ImmType   = IMMTYPE_PRIMFCN;
////////tkFcn.tkFlags.NoDisplay = FALSE;            // Already zero from tkZero
        tkFcn.tkData.tkIndex    = UTF16_RHO;
        tkFcn.tkCharIndex       = lpYYFcnStrRht->tkToken.tkCharIndex;

        // Check for right operand axis operator
        lptkAxisRht = CheckAxisOper (lpYYFcnStrRht);

        // Execute the right operand between the left and right args
        //   as a prototype function if it's a UDFO

        // Get the appropriate prototype function ptr
        lpPrimProtoRht = GetPrototypeFcnPtr (&lpYYFcnStrRht->tkToken);

        // Check for error
        if (!lpPrimProtoRht)
            goto RIGHT_NONCE_EXIT;

        // Reshape the left arg item to an empty vector
        lpYYResL =
          PrimFnDydRho_EM_YY (&tkLft,           // Ptr to left arg token
                              &tkFcn,           // Ptr to function token
                               lptkLftArg,      // Ptr to right arg token
                               NULL);           // Ptr to axis token (may be NULL)
        // Check for error
        if (lpYYResL EQ NULL)
            goto ERROR_EXIT;

        // Reshape the right arg item to an empty vector
        lpYYResR =
          PrimFnDydRho_EM_YY (&tkLft,           // Ptr to left arg token
                              &tkFcn,           // Ptr to function token
                               lptkRhtArg,      // Ptr to right arg token
                               NULL);           // Ptr to axis token (may be NULL)
        // Check for error
        if (lpYYResR EQ NULL)
            goto ERROR_EXIT;

        // Setup a token with the {take} function
////////tkFcn.tkFlags.TknType   = TKT_FCNIMMED;         // Already set above
////////tkFcn.tkFlags.ImmType   = IMMTYPE_PRIMFCN;      // Already set above
////////tkFcn.tkFlags.NoDisplay = FALSE;                // Already zero from = {0}
        tkFcn.tkData.tkIndex    = UTF16_UPARROW;
////////tkFcn.tkCharIndex       = lpYYFcnStrLft->tkToken.tkCharIndex;

        // Execute monadic UpArrow on the last two results to extract the first item
        lpYYResL2 =
          PrimFnMonUpArrow_EM_YY (&tkFcn,               // Ptr to function token
                                  &lpYYResL->tkToken,   // Ptr to right arg token
                                   NULL);               // Ptr to axis token (may be NULL)
        // Check for error
        if (lpYYResL2 EQ NULL)
            goto ERROR_EXIT;

        // Execute monadic UpArrow on the last two results to extract the first item
        lpYYResR2 =
          PrimFnMonUpArrow_EM_YY (&tkFcn,               // Ptr to function token
                                  &lpYYResR->tkToken,   // Ptr to right arg token
                                   NULL);               // Ptr to axis token (may be NULL)
        // Check for error
        if (lpYYResR2 EQ NULL)
            goto ERROR_EXIT;

        // Execute the right operand between the left and right args

        // Note that we cast the function strand to LPTOKEN
        //   to bridge the two types of calls -- one to a primitive
        //   function which takes a function token, and one to a
        //   primitive operator which takes a function strand
        lpYYRes2 =
          (*lpPrimProtoRht) (&lpYYResL2->tkToken,   // Ptr to left arg token (may be NULL)
                    (LPTOKEN) lpYYFcnStrRht,        // Ptr to left operand function strand
                             &lpYYResR2->tkToken,   // Ptr to right arg token
                              lptkAxisRht);         // Ptr to axis token (may be NULL)
    } else
    {
        // Setup a token with the {take} function
        tkFcn.tkFlags.TknType   = TKT_FCNIMMED;         // Already set above
        tkFcn.tkFlags.ImmType   = IMMTYPE_PRIMFCN;      // Already set above
////////tkFcn.tkFlags.NoDisplay = FALSE;                // Already zero from = {0}
        tkFcn.tkData.tkIndex    = UTF16_UPARROW;
        tkFcn.tkCharIndex       = lpYYFcnStrLft->tkToken.tkCharIndex;

        // Execute monadic UpArrow on the right arg to extract the first item
        lpYYRes2 =
          PrimFnMonUpArrow_EM_YY (&tkFcn,               // Ptr to function token
                                   lptkRhtArg,          // Ptr to right arg token
                                   NULL);               // Ptr to axis token (may be NULL)
    } // End IF/ELSE

    // Check for error
    if (lpYYRes2 EQ NULL)
        goto ERROR_EXIT;

    // Copy the result as the new right arg token
    lptkRhtArg = &lpYYRes2->tkToken;

    // Execute the left operand identity function on the right arg

    // Check for left operand axis operator
    lptkAxisLft = CheckAxisOper (lpYYFcnStrLft);

    // Get the appropriate identity function ptr
    lpPrimFlagsLft = GetPrimFlagsPtr (&lpYYFcnStrLft->tkToken);

    // Check for error
    if (!lpPrimFlagsLft || !lpPrimFlagsLft->lpPrimOps)
        goto LEFT_NONCE_EXIT;

    lpYYRes =
      (*lpPrimFlagsLft->lpPrimOps)
                        (lptkRhtArg,            // Ptr to original right arg token
                         lpYYFcnStrLft,         // Ptr to operator or function strand
                         lptkRhtArg,            // Ptr to right arg token
                         lptkAxisLft);          // Ptr to axis token (may be NULL)
    // Check for error
    if (lpYYRes EQ NULL)
        goto ERROR_EXIT;

    // Is the token immediate?
    if (IsTknImmed (&lpYYRes->tkToken))
    {
        // Copy the LPSYMENTRY as the result
        hGlbSym.lpSym =
          MakeSymEntry_EM (lpYYRes->tkToken.tkFlags.ImmType,    // ImmType to use (see IMM_TYPES)
                          &lpYYRes->tkToken.tkData.tkLongest,   // Ptr to value to use
                          &lpYYFcnStrLft->tkToken);             // Ptr to token to use in case of error
        // Check for error
        if (hGlbSym.lpSym EQ NULL)
            goto ERROR_EXIT;
    } else
        // Copy the HGLOBAL as the result
        hGlbSym.hGlb = CopySymGlbDir_PTB (lpYYRes->tkToken.tkData.tkGlbData);

    goto NORMAL_EXIT;

LEFT_NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

RIGHT_NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (lpYYRes)
    {
        FreeResult (lpYYRes);  YYFree (lpYYRes);  lpYYRes  = NULL;
    } // End IF

    if (lpYYRes2)
    {
        FreeResult (lpYYRes2); YYFree (lpYYRes2); lpYYRes2 = NULL;
    } // End IF

    if (lpYYResL)
    {
        FreeResult (lpYYResL); YYFree (lpYYResL); lpYYResL = NULL;
    } // End IF

    if (lpYYResL2)
    {
        FreeResult (lpYYResL2); YYFree (lpYYResL2); lpYYResL2 = NULL;
    } // End IF

    if (lpYYResR)
    {
        FreeResult (lpYYResR); YYFree (lpYYResR); lpYYResR = NULL;
    } // End IF

    if (lpYYResR2)
    {
        FreeResult (lpYYResR2); YYFree (lpYYResR2); lpYYResR2 = NULL;
    } // End IF

    return hGlbSym;
} // End GetIdentityElement_EM
#undef  APPEND_NAME


//***************************************************************************
//  $FillIdentityElement_EM
//
//  Fill in the result with the identity element for a give function and array
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FillIdentityElement_EM"
#else
#define APPEND_NAME
#endif

UBOOL FillIdentityElement_EM
    (LPVOID       lpMemRes,                     // Ptr to result global memory
     APLNELM      aplNELMRes,                   // Result NELM
     APLSTYPE     aplTypeRes,                   // Result storage type
     LPPL_YYSTYPE lpYYFcnStrLft,                // Ptr to left operand function strand
     LPTOKEN      lptkLftArg,                   // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrRht,                // Ptr to right operand function strand
     LPTOKEN      lptkRhtArg)                   // Ptr to right arg token

{
    GLBSYM  hGlbSym;                            // Result
    APLUINT uRes;                               // Loop counter

    // Get the identity element
    hGlbSym =
      GetIdentityElement_EM (lpYYFcnStrLft,     // Ptr to left operand function strand
                             lptkLftArg,        // Ptr to left arg token
                             lpYYFcnStrRht,     // Ptr to right operand function strand
                             lptkRhtArg);       // Ptr to right arg token
    // Check for errors
    if (hGlbSym.hGlb EQ NULL)
        return FALSE;

    // Split cases based upon the ptr type bits
    switch (GetPtrTypeDir (hGlbSym.hGlb))
    {
        case PTRTYPE_STCONST:
            // The identity element is a simple scalar

            // Split cases based upon the result storage type
            switch (aplTypeRes)
            {
                case ARRAY_BOOL:
                case ARRAY_INT:
                case ARRAY_CHAR:
                case ARRAY_FLOAT:
                    // Split cases based upon the identity element immediate type
                    switch (hGlbSym.lpSym->stFlags.ImmType)
                    {
                        case IMMTYPE_BOOL:
                            // If it's a Boolean != 0, save in the result
                            if (hGlbSym.lpSym->stData.stBoolean)
                                // Fill the result with all 1s
                                FillMemory (lpMemRes, (APLU3264) RoundUpBitsToBytes (aplNELMRes), 0xFF);
                            break;

                        case IMMTYPE_INT:
                            // If it's an integer != 0, save in the result
                            if (hGlbSym.lpSym->stData.stInteger)
                            while (aplNELMRes--)
                                *((LPAPLINT) lpMemRes)++ = hGlbSym.lpSym->stData.stInteger;
                            break;

                        case IMMTYPE_FLOAT:
                            // If it's a float != 0, save in the result
                            if (hGlbSym.lpSym->stData.stFloat)
                            while (aplNELMRes--)
                                *((LPAPLFLOAT) lpMemRes)++ = hGlbSym.lpSym->stData.stFloat;
                            break;

                        case IMMTYPE_CHAR:
                            while (aplNELMRes--)
                                *((LPAPLCHAR) lpMemRes)++ = hGlbSym.lpSym->stData.stChar;
                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case ARRAY_NESTED:
                    // If the result is empty, ...
                    if (IsEmpty (aplNELMRes))
                    {
                        LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

                        // Get ptr to PerTabData global memory
                        lpMemPTD = GetMemPTD ();

                        // If the identity element is numeric, ...
                        if (IsImmNum (hGlbSym.lpSym->stFlags.ImmType))
                            // Save the prototype of the identity element in the result
                            *((LPAPLNESTED) lpMemRes)++ = lpMemPTD->lphtsPTD->steZero;
                        else
                            // Save the prototype of the identity element in the result
                            *((LPAPLNESTED) lpMemRes)++ = lpMemPTD->lphtsPTD->steBlank;
                    } else
                    {
                        // Save the identity element in the result
                        *((LPAPLNESTED) lpMemRes)++ = hGlbSym.hGlb;
                        for (uRes = 1; uRes < aplNELMRes; uRes++)
                            *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (hGlbSym.hGlb);
                    } // End IF/ELSE

                    break;

                case ARRAY_APA:
                case ARRAY_HETERO:
                defstop
                    break;
            } // End SWITCH

            break;

        case PTRTYPE_HGLOBAL:
            // The identity element is a global

            Assert (IsNested (aplTypeRes));

            // If the result is empty, ...
            if (IsEmpty (aplNELMRes))
            {
                HGLOBAL hGlbPro;        // Prototype global memory handle

                // Make the prototype
                hGlbPro =
                  MakeMonPrototype_EM_PTB (hGlbSym.hGlb,            // Proto arg handle
                                          &lpYYFcnStrLft->tkToken,  // Ptr to function token
                                           MP_CHARS);               // CHARs allowed
                // We no longer need this storage
                FreeResultGlobalVar (hGlbSym.hGlb); hGlbSym.hGlb = NULL;

                // Check for error
                if (hGlbPro EQ NULL)
                    return FALSE;

                // Save the prototype of the identity element in the result
                *((LPAPLNESTED) lpMemRes)++ = hGlbPro;
            } else
            {
                *((LPAPLNESTED) lpMemRes)++ = hGlbSym.hGlb;
                for (uRes = 1; uRes < aplNELMRes; uRes++)
                    *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (hGlbSym.hGlb);
            } // End IF/ELSE

            break;

        defstop
            break;
    } // End IF/SWITCH

    return TRUE;
} // End FillIdentityElement_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: identfns.c
//***************************************************************************
