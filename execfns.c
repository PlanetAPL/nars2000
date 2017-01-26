//***************************************************************************
//  NARS2000 -- Execution Functions
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
//  $ExecuteFn0
//
//  Execute a niladic function
//***************************************************************************

LPPL_YYSTYPE ExecuteFn0
    (LPPL_YYSTYPE lpYYFcn0)     // Ptr to function PL_YYSTYPE

{
    LPPRIMFNS lpNameFcn;

    // Split cases based upon the ptr type bits
    switch (GetPtrTypeDir (lpYYFcn0->tkToken.tkData.tkVoid))
    {
        case PTRTYPE_STCONST:
            // tkData is an LPSYMENTRY
            lpNameFcn = lpYYFcn0->tkToken.tkData.tkSym->stData.stNameFcn;

            if (lpYYFcn0->tkToken.tkData.tkSym->stFlags.FcnDir)
                // Call the execution routine
                return (*lpNameFcn) (NULL,                      // Ptr to left arg token (may be NULL if monadic)
                                    &lpYYFcn0->tkToken,         // Ptr to function token
                                     NULL,                      // Ptr to right arg token
                                     NULL);                     // Ptr to axis token (may be NULL)
            break;

        case PTRTYPE_HGLOBAL:
            // tkData is an HGLOBAL
            lpNameFcn = lpYYFcn0->tkToken.tkData.tkGlbData;

            break;

        defstop
            break;
    } // End SWITCH

    // tkData is a valid HGLOBAL function array or user-defined function/operator
    Assert (IsGlbTypeFcnDir_PTB (lpNameFcn)
         || IsGlbTypeDfnDir_PTB (lpNameFcn));

    // Split cases based upon the array signature
    switch (GetSignatureGlb_PTB (lpNameFcn))
    {
        case FCNARRAY_HEADER_SIGNATURE:
            // Execute a function array global memory handle
            return ExecFcnGlb_EM_YY (NULL,          // Ptr to left arg token (may be NULL if niladic/monadic)
                                     lpNameFcn,     // Function array global memory handle
                                     NULL,          // Ptr to right arg token (may be NULL if niladic)
                                     NULL);         // Ptr to axis token (may be NULL)
        case DFN_HEADER_SIGNATURE:
            // Execute a user-defined function/operator global memory handle
            return ExecDfnGlb_EM_YY (lpNameFcn,     // User-defined function/operator global memory handle
                                     NULL,          // Ptr to left arg token (may be NULL if monadic)
                                     lpYYFcn0,      // Ptr to function strand
                                     NULL,          // Ptr to axis token (may be NULL -- used only if function strand is NULL)
                                     NULL,          // Ptr to right arg token
                                     LINENUM_ONE);  // Starting line # (see LINE_NUMS)
        defstop
            return NULL;
    } // End SWITCH
} // End ExecuteFn0


//***************************************************************************
//  $ExecFunc_EM_YY
//
//  Execute a user-defined, system, or primitive function
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecFunc_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecFunc_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStr,           // Ptr to function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    LPPRIMFNS    lpPrimFn;              // Ptr to direct primitive or system function
    LPTOKEN      lptkAxis;              // Ptr to axis token (may be NULL)
    HGLOBAL      hGlbFcn,               // Function array global memory handle
                 hGlbLft,               // Left arg global memory handle (may be NULL)
                 hGlbRht;               // Right ...
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPHSHTABSTR  lphtsPTD = NULL,       // Ptr to HshTab struc about to be activated
                 lphtsOld;              // ...    old HTS
    LPPL_YYSTYPE lpYYRes = NULL;        // Ptr to the result
    UINT         uSysNSLvl;             // System namespace level

    // Get the left and right arg global memory handle
    if (lptkLftArg NE NULL)
        hGlbLft = GetGlbHandle (lptkLftArg);
    else
        hGlbLft = NULL;
    if (lptkRhtArg NE NULL)
        hGlbRht = GetGlbHandle (lptkRhtArg);
    else
        hGlbRht = NULL;

    // Check for NoValue
    if (IsTokenNoValue (lptkLftArg)
     || IsTokenNoValue (lptkRhtArg))
        goto VALUE_EXIT;

    // Check for axis operator
    lptkAxis = CheckAxisOper (lpYYFcnStr);

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Split cases based upon the function token type
    switch (lpYYFcnStr->tkToken.tkFlags.TknType)
    {
        case TKT_FCNIMMED:
            // Get the address of the execution routine
            lpPrimFn = PrimFnsTab[SymTrans (&lpYYFcnStr->tkToken)];
            if (lpPrimFn EQ NULL)
                goto SYNTAX_EXIT;

            // Get the system namespace level
            uSysNSLvl = lpYYFcnStr->tkToken.tkFlags.SysNSLvl;

            // Check for system namespace
            if (uSysNSLvl > 0)
            {
                // Save the old HTS
                lphtsPTD =
                lphtsOld = lpMemPTD->lphtsPTD;

                // Peel back HshTabStrs
                while (uSysNSLvl-- > 1 && lphtsPTD->lphtsPrvSrch)
                    lphtsPTD = lphtsPTD->lphtsPrvSrch;

                // If we found a new HTS, ...
                if (lphtsPTD NE lphtsOld)
                    // Point to it
                    lpMemPTD->lphtsPTD = lphtsPTD;
            } // End IF

            __try
            {
                // Execute the primitive function
                lpYYRes =
                  (*lpPrimFn) (lptkLftArg,                      // Ptr to left arg token (may be NULL if monadic)
                              &lpYYFcnStr->tkToken,             // Ptr to function token
                               lptkRhtArg,                      // Ptr to right arg token
                               lptkAxis);                       // Ptr to axis token (may be NULL)
            } __except (CheckException (GetExceptionInformation (), L"ExecFunc_EM_YY #1"))
            {
                // Set the error message text
                ErrorMessageIndirectToken (ERRMSG_LIMIT_ERROR APPEND_NAME,
                                          &lpYYFcnStr->tkToken);
                // Mark as an error
                lpYYRes = NULL;
            } // End __try/__except

            // Get the system namespace level
            uSysNSLvl = lpYYFcnStr->tkToken.tkFlags.SysNSLvl;

            // Check for system namespace,
            //   and past HshTabStr struc
            if (uSysNSLvl > 0
             && lphtsPTD NE lphtsOld)
                // Restore the old HTS
                lpMemPTD->lphtsPTD = lphtsOld;
            break;

        case TKT_FCNNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpYYFcnStr->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If the LPSYMENTRY is not immediate, it must be an HGLOBAL
            if (!lpYYFcnStr->tkToken.tkData.tkSym->stFlags.Imm)
            {
                STFLAGS stFlags;

                // Get the STE flags of the first token
                stFlags = lpYYFcnStr->tkToken.tkData.tkSym->stFlags;

                // If it's an internal function, execute it directly
                if (stFlags.FcnDir)
                {
                    LPPRIMFNS lpNameFcn;            // Ptr to the internal routine

                    // Get the address of the execution routine
                    lpNameFcn = lpYYFcnStr->tkToken.tkData.tkSym->stData.stNameFcn;

                    // Get the system namespace level
                    uSysNSLvl = lpYYFcnStr->tkToken.tkFlags.SysNSLvl;

                    // Check for system namespace
                    if (uSysNSLvl > 0)
                    {
                        // Save the old HTS
                        lphtsPTD =
                        lphtsOld = lpMemPTD->lphtsPTD;

                        // Peel back HshTabStrs
                        while (uSysNSLvl-- > 1 && lphtsPTD->lphtsPrvSrch)
                            lphtsPTD = lphtsPTD->lphtsPrvSrch;

                        // If we found a new HTS, ...
                        if (lphtsPTD NE lphtsOld)
                            // Point to it
                            lpMemPTD->lphtsPTD = lphtsPTD;
                    } // End IF

                    __try
                    {
                        // If it's an internal function, ...
                        lpYYRes =
                          (*lpNameFcn) (lptkLftArg,             // Ptr to left arg token (may be NULL if monadic)
                                       &lpYYFcnStr->tkToken,    // Ptr to function token
                                        lptkRhtArg,             // Ptr to right arg token
                                        lptkAxis);              // Ptr to axis token (may be NULL)
                    } __except (CheckException (GetExceptionInformation (), L"ExecFunc_EM_YY #2"))
                    {
                        // Set the error message text
                        ErrorMessageIndirectToken (ERRMSG_LIMIT_ERROR APPEND_NAME,
                                                  &lpYYFcnStr->tkToken);
                        // Mark as an error
                        lpYYRes = NULL;
                    } // End __try/__except

                    // Get the system namespace level
                    uSysNSLvl = lpYYFcnStr->tkToken.tkFlags.SysNSLvl;

                    // Check for system namespace,
                    //   and past HshTabStr struc
                    if (uSysNSLvl > 0
                     && lphtsPTD NE lphtsOld)
                        // Restore the old HTS
                        lpMemPTD->lphtsPTD = lphtsOld;
                    break;
                } // End IF

                // Use the HGLOBAL
                hGlbFcn = lpYYFcnStr->tkToken.tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL function array
                //   or user-defined function/operator
                Assert (IsGlbTypeFcnDir_PTB (hGlbFcn)
                     || IsGlbTypeDfnDir_PTB (hGlbFcn));

                __try
                {
                    // If it's a user-defined function/operator, ...
                    if (stFlags.UsrDfn)
                        lpYYRes =
                          ExecDfnGlb_EM_YY (hGlbFcn,        // User-defined function/operator global memory handle
                                            lptkLftArg,     // Ptr to left arg token (may be NULL if monadic)
                                            lpYYFcnStr,     // Ptr to function strand
                                            lptkAxis,       // Ptr to axis token (may be NULL -- used only if function strand is NULL)
                                            lptkRhtArg,     // Ptr to right arg token
                                            LINENUM_ONE);   // Starting line # (see LINE_NUMS)
                    else
                        // Execute a function array global memory handle
                        lpYYRes =
                          ExecFcnGlb_EM_YY (lptkLftArg,     // Ptr to left arg token (may be NULL if niladic/monadic)
                                            hGlbFcn,        // Function array global memory handle
                                            lptkRhtArg,     // Ptr to right arg token (may be NULL if niladic)
                                            lptkAxis);      // Ptr to axis token (may be NULL)
                } __except (CheckException (GetExceptionInformation (), L"ExecFunc_EM_YY #3"))
                {
                    // Set the error message text
                    ErrorMessageIndirectToken (ERRMSG_LIMIT_ERROR APPEND_NAME,
                                              &lpYYFcnStr->tkToken);
                    // Mark as an error
                    lpYYRes = NULL;
                } // End __try/__except

                break;
            } // End IF

            // Handle the immediate case

            // Split cases based upon the immediate type
            switch (lpYYFcnStr->tkToken.tkData.tkSym->stFlags.ImmType)
            {
                case IMMTYPE_PRIMFCN:
                {
                    TOKEN tkFn = {0};

                    lpPrimFn = PrimFnsTab[FcnTrans (lpYYFcnStr->tkToken.tkData.tkSym->stData.stChar)];
                    if (lpPrimFn EQ NULL)
                        goto SYNTAX_EXIT;

                    // Fill in for PrimFn*** test
                    tkFn.tkFlags.TknType   = TKT_FCNIMMED;
                    tkFn.tkFlags.ImmType   = GetImmTypeFcn (lpYYFcnStr->tkToken.tkData.tkSym->stData.stChar);
////////////////////tkFn.tkFlags.NoDisplay = FALSE; // Already zero from {0}
                    tkFn.tkData.tkChar     = lpYYFcnStr->tkToken.tkData.tkSym->stData.stChar;
                    tkFn.tkCharIndex       = lpYYFcnStr->tkToken.tkCharIndex;

                    __try
                    {
                        lpYYRes =
                          (*lpPrimFn) (lptkLftArg,      // Ptr to left arg token (may be NULL if monadic)
                                      &tkFn,            // Ptr to function token
                                       lptkRhtArg,      // Ptr to right arg token
                                       lptkAxis);       // Ptr to axis token (may be NULL)
                    } __except (CheckException (GetExceptionInformation (), L"ExecFunc_EM_YY #4"))
                    {
                        // Set the error message text
                        ErrorMessageIndirectToken (ERRMSG_LIMIT_ERROR APPEND_NAME,
                                                  &lpYYFcnStr->tkToken);
                        // Mark as an error
                        lpYYRes = NULL;
                    } // End __try/__except

                    break;
                } // End IMMTYPE_PRIMFCN

                defstop
                    break;
            } // End SWITCH

            break;

        case TKT_FCNARRAY:
        case TKT_FCNAFO:
            // Get the global memory handle
            hGlbFcn = GetGlbDataToken (&lpYYFcnStr->tkToken);

            // tkData is a valid HGLOBAL function array
            //   or user-defined function/operator
            Assert (IsGlbTypeFcnDir_PTB (hGlbFcn)
                 || IsGlbTypeDfnDir_PTB (hGlbFcn));

            __try
            {
                // If it's a user-defined function/operator, ...
                switch (GetSignatureGlb_PTB (hGlbFcn))
                {
                    case FCNARRAY_HEADER_SIGNATURE:
                        // Execute a function array global memory handle
                        lpYYRes =
                          ExecFcnGlb_EM_YY (lptkLftArg,     // Ptr to left arg token (may be NULL if niladic/monadic)
                                            hGlbFcn,        // Function array global memory handle
                                            lptkRhtArg,     // Ptr to right arg token
                                            lptkAxis);      // Ptr to axis token (may be NULL)
                        break;

                    case DFN_HEADER_SIGNATURE:
                        // Execute a user-defined function/operator global memory handle
                        lpYYRes =
                          ExecDfnGlb_EM_YY (hGlbFcn,        // User-defined function/operator global memory handle
                                            lptkLftArg,     // Ptr to left arg token (may be NULL if monadic)
                                            lpYYFcnStr,     // Ptr to function strand
                                            lptkAxis,       // Ptr to axis token (may be NULL -- used only if function strand is NULL)
                                            lptkRhtArg,     // Ptr to right arg token
                                            LINENUM_ONE);   // Starting line # (see LINE_NUMS)
                        break;

                    defstop
                        break;
                } // End SWITCH
            } __except (CheckException (GetExceptionInformation (), L"ExecFunc_EM_YY #5"))
            {
                // Set the error message text
                ErrorMessageIndirectToken (ERRMSG_LIMIT_ERROR APPEND_NAME,
                                          &lpYYFcnStr->tkToken);
                // Mark as an error
                lpYYRes = NULL;
            } // End __try/__except

            break;

        case TKT_DELAFO:
        {
            HGLOBAL      hGlbFcn;           // Function global memory handle
            LPPL_YYSTYPE lpYYFcnStrLft,     // Ptr to left  operand (may be NULL if function                    )
                         lpYYFcnStrRht;     // ...    right ...     (...         if function or monadic operator)

            // Get the global memory handle
            hGlbFcn = lpYYFcnStr->tkToken.tkData.tkGlbData;

            // Setup left and right operands (if present)
            GetOperands (hGlbFcn, &lpYYFcnStrLft, &lpYYFcnStrRht);

            __try
            {
                // Call common routine
                lpYYRes =
                  ExecDfnOprGlb_EM_YY (hGlbFcn,         // User-defined function/operator global memory handle
                                       lptkLftArg,      // Ptr to left arg token (may be NULL if monadic)
                                       lpYYFcnStrLft,   // Ptr to left operand function strand (may be NULL if not an operator and no axis)
                                       lpYYFcnStr,      // Ptr to function strand (may be NULL if not an operator and no axis)
                                       lpYYFcnStrRht,   // Ptr to right operand function strand (may be NULL if not an operator and no axis)
                                       lptkAxis,        // Ptr to axis token (may be NULL -- used only if function strand is NULL)
                                       lptkRhtArg,      // Ptr to right arg token
                                       LINENUM_ONE);    // Starting line # (see LINE_NUMS)
            } __except (CheckException (GetExceptionInformation (), L"ExecFunc_EM_YY #6"))
            {
                // Set the error message text
                ErrorMessageIndirectToken (ERRMSG_LIMIT_ERROR APPEND_NAME,
                                          &lpYYFcnStr->tkToken);
                // Mark as an error
                lpYYRes = NULL;
            } // End __try/__except

            // If we allocated it, ...
            if (lpYYFcnStrLft NE NULL)
                // Free the left operand YYRes
                YYFree (lpYYFcnStrLft);

            // If we allocated it, ...
            if (lpYYFcnStrRht NE NULL)
                // Free the right operand YYRes
                YYFree (lpYYFcnStrRht);

            break;
        } // End TKT_DELAFO

        defstop
            break;
    } // End SWITCH

    // If the result is valid, ...
    if (lpYYRes NE NULL
     && !IsTokenNoValue (&lpYYRes->tkToken))
        // Change the tkSynObj
        lpYYRes->tkToken.tkSynObj = soA;

    goto NORMAL_EXIT;

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStr->tkToken);
    goto NORMAL_EXIT;

VALUE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
                               (lptkLftArg NE NULL) ? lptkLftArg
                                                    : lptkRhtArg);
    goto NORMAL_EXIT;

NORMAL_EXIT:
    return lpYYRes;
} // End ExecFunc_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $GetOperands
//
//  Setup left & right operands if present
//***************************************************************************

void GetOperands
    (HGLOBAL       hGlbFcn,                 // Function global memory handle
     LPPL_YYSTYPE *lplpYYFcnStrLft,         // Ptr to ptr to left operand
     LPPL_YYSTYPE *lplpYYFcnStrRht)         // ...           right ...

{
    LPDFN_HEADER lpMemDfnHdr;       // Ptr to user-defined function/operator header

    // Lock the memory to get a ptr to it
    lpMemDfnHdr = MyGlobalLockDfn (hGlbFcn);

    // If there's a left operand, ...
    if (lpMemDfnHdr->steLftOpr NE NULL)
    {
        // Allocate a new YYRes for the left operand
        *lplpYYFcnStrLft = YYAlloc ();

        // Split cases based upon the type of the left operand
        switch (lpMemDfnHdr->steLftOpr->stFlags.stNameType)
        {
            case NAMETYPE_UNK:
                (*lplpYYFcnStrLft)->tkToken.tkFlags.TknType       = TKT_FILLJOT;
////////////////(*lplpYYFcnStrLft)->tkToken.tkFlags.ImmType       = IMMTYPE_ERROR;      // Already zero from YYAlloc
////////////////(*lplpYYFcnStrLft)->tkToken.tkFlags.NoDisplay     = FALSE;              // Already zero from YYAlloc
                (*lplpYYFcnStrLft)->tkToken.tkData.tkChar         = UTF16_JOT;
////////////////(*lplpYYFcnStrLft)->tkToken.tkCharIndex           =     // Ignored

                break;

            case NAMETYPE_VAR:
                // If the var is immediate, ...
                if (lpMemDfnHdr->steLftOpr->stFlags.Imm)
                {
                    // Fill in the result token
                    (*lplpYYFcnStrLft)->tkToken.tkFlags.TknType   = TKT_VARIMMED;
                    (*lplpYYFcnStrLft)->tkToken.tkFlags.ImmType   = lpMemDfnHdr->steLftOpr->stFlags.ImmType;
////////////////////(*lplpYYFcnStrLft)->tkToken.tkFlags.NoDisplay =     // Filled in by YYAlloc
                    (*lplpYYFcnStrLft)->tkToken.tkData.tkLongest  = lpMemDfnHdr->steLftOpr->stData.stLongest;
////////////////////(*lplpYYFcnStrLft)->tkToken.tkCharIndex       =     // Ignored
                } else
                {
                    // Fill in the result token
                    (*lplpYYFcnStrLft)->tkToken.tkFlags.TknType   = TKT_VARNAMED;
////////////////////(*lplpYYFcnStrLft)->tkToken.tkFlags.ImmType   =     // Filled in by YYAlloc
////////////////////(*lplpYYFcnStrLft)->tkToken.tkFlags.NoDisplay =     // Filled in by YYAlloc
                    (*lplpYYFcnStrLft)->tkToken.tkData.tkSym      = lpMemDfnHdr->steLftOpr;
////////////////////(*lplpYYFcnStrLft)->tkToken.tkCharIndex       =     // Ignored
                } // End IF/ELSE

                break;

            case NAMETYPE_FN12:
                // If the fcn is immediate, ...
                if (lpMemDfnHdr->steLftOpr->stFlags.Imm)
                {
                    // Fill in the result token
                    (*lplpYYFcnStrLft)->tkToken.tkFlags.TknType   = TKT_FCNIMMED;
                    (*lplpYYFcnStrLft)->tkToken.tkFlags.ImmType   = lpMemDfnHdr->steLftOpr->stFlags.ImmType;
////////////////////(*lplpYYFcnStrLft)->tkToken.tkFlags.NoDisplay =     // Filled in by YYAlloc
                    (*lplpYYFcnStrLft)->tkToken.tkData.tkLongest  = lpMemDfnHdr->steLftOpr->stData.stLongest;
////////////////////(*lplpYYFcnStrLft)->tkToken.tkCharIndex       =     // Ignored
                } else
                {
                    // Fill in the result token
                    (*lplpYYFcnStrLft)->tkToken.tkFlags.TknType   = TKT_FCNNAMED;
////////////////////(*lplpYYFcnStrLft)->tkToken.tkFlags.ImmType   =     // Filled in by YYAlloc
////////////////////(*lplpYYFcnStrLft)->tkToken.tkFlags.NoDisplay =     // Filled in by YYAlloc
                    (*lplpYYFcnStrLft)->tkToken.tkData.tkSym      = lpMemDfnHdr->steLftOpr;
////////////////////(*lplpYYFcnStrLft)->tkToken.tkCharIndex       =     // Ignored
                } // End IF/ELSE

                break;

            case NAMETYPE_OP1:
                // If the op1 is immediate, ...
                if (lpMemDfnHdr->steLftOpr->stFlags.Imm)
                {
                    // Fill in the result token
                    (*lplpYYFcnStrLft)->tkToken.tkFlags.TknType   = TKT_OP1IMMED;
                    (*lplpYYFcnStrLft)->tkToken.tkFlags.ImmType   = lpMemDfnHdr->steLftOpr->stFlags.ImmType;
////////////////////(*lplpYYFcnStrLft)->tkToken.tkFlags.NoDisplay =     // Filled in by YYAlloc
                    (*lplpYYFcnStrLft)->tkToken.tkData.tkLongest  = lpMemDfnHdr->steLftOpr->stData.stLongest;
////////////////////(*lplpYYFcnStrLft)->tkToken.tkCharIndex       =     // Ignored
                } else
                {
                    // Fill in the result token
                    (*lplpYYFcnStrLft)->tkToken.tkFlags.TknType   = TKT_OP1NAMED;
////////////////////(*lplpYYFcnStrLft)->tkToken.tkFlags.ImmType   =     // Filled in by YYAlloc
////////////////////(*lplpYYFcnStrLft)->tkToken.tkFlags.NoDisplay =     // Filled in by YYAlloc
                    (*lplpYYFcnStrLft)->tkToken.tkData.tkSym      = lpMemDfnHdr->steLftOpr;
////////////////////(*lplpYYFcnStrLft)->tkToken.tkCharIndex       =     // Ignored
                } // End IF/ELSE

                break;

            case NAMETYPE_OP2:
                // If the op2 is immediate, ...
                if (lpMemDfnHdr->steLftOpr->stFlags.Imm)
                {
                    // Fill in the result token
                    (*lplpYYFcnStrLft)->tkToken.tkFlags.TknType   = TKT_OP2IMMED;
                    (*lplpYYFcnStrLft)->tkToken.tkFlags.ImmType   = lpMemDfnHdr->steLftOpr->stFlags.ImmType;
////////////////////(*lplpYYFcnStrLft)->tkToken.tkFlags.NoDisplay =     // Filled in by YYAlloc
                    (*lplpYYFcnStrLft)->tkToken.tkData.tkLongest  = lpMemDfnHdr->steLftOpr->stData.stLongest;
////////////////////(*lplpYYFcnStrLft)->tkToken.tkCharIndex       =     // Ignored
                } else
                {
                    // Fill in the result token
                    (*lplpYYFcnStrLft)->tkToken.tkFlags.TknType   = TKT_OP2NAMED;
////////////////////(*lplpYYFcnStrLft)->tkToken.tkFlags.ImmType   =     // Filled in by YYAlloc
////////////////////(*lplpYYFcnStrLft)->tkToken.tkFlags.NoDisplay =     // Filled in by YYAlloc
                    (*lplpYYFcnStrLft)->tkToken.tkData.tkSym      = lpMemDfnHdr->steLftOpr;
////////////////////(*lplpYYFcnStrLft)->tkToken.tkCharIndex       =     // Ignored
                } // End IF/ELSE

                break;

            defstop
                break;
        } // End SWITCH

        // Set the token count
        (*lplpYYFcnStrLft)->TknCount = 1;
    } else
        // Mark as unallocated
        *lplpYYFcnStrLft = NULL;

    // If there's a right operand, ...
    if (lpMemDfnHdr->steRhtOpr NE NULL)
    {
        // Allocate a new YYRes for it
        *lplpYYFcnStrRht = YYAlloc ();

        // Split cases based upon the type of the right operand
        switch (lpMemDfnHdr->steRhtOpr->stFlags.stNameType)
        {
            case NAMETYPE_UNK:
                (*lplpYYFcnStrRht)->tkToken.tkFlags.TknType       = TKT_FILLJOT;
////////////////(*lplpYYFcnStrRht)->tkToken.tkFlags.ImmType       = IMMTYPE_ERROR;      // Already zero from YYAlloc
////////////////(*lplpYYFcnStrRht)->tkToken.tkFlags.NoDisplay     = FALSE;              // Already zero from YYAlloc
                (*lplpYYFcnStrRht)->tkToken.tkData.tkChar         = UTF16_JOT;
////////////////(*lplpYYFcnStrRht)->tkToken.tkCharIndex           =     // Ignored

                break;

            case NAMETYPE_VAR:
                // If the var is immediate, ...
                if (lpMemDfnHdr->steRhtOpr->stFlags.Imm)
                {
                    // Fill in the result token
                    (*lplpYYFcnStrRht)->tkToken.tkFlags.TknType   = TKT_VARIMMED;
                    (*lplpYYFcnStrRht)->tkToken.tkFlags.ImmType   = lpMemDfnHdr->steRhtOpr->stFlags.ImmType;
////////////////////(*lplpYYFcnStrRht)->tkToken.tkFlags.NoDisplay =     // Filled in by YYAlloc
                    (*lplpYYFcnStrRht)->tkToken.tkData.tkLongest  = lpMemDfnHdr->steRhtOpr->stData.stLongest;
////////////////////(*lplpYYFcnStrRht)->tkToken.tkCharIndex       =     // Ignored
                } else
                {
                    // Fill in the result token
                    (*lplpYYFcnStrRht)->tkToken.tkFlags.TknType   = TKT_VARNAMED;
////////////////////(*lplpYYFcnStrRht)->tkToken.tkFlags.ImmType   =     // Filled in by YYAlloc
////////////////////(*lplpYYFcnStrRht)->tkToken.tkFlags.NoDisplay =     // Filled in by YYAlloc
                    (*lplpYYFcnStrRht)->tkToken.tkData.tkSym      = lpMemDfnHdr->steRhtOpr;
////////////////////(*lplpYYFcnStrRht)->tkToken.tkCharIndex       =     // Ignored
                } // End IF/ELSE

                break;

            case NAMETYPE_FN12:
                // If the fcn is immediate, ...
                if (lpMemDfnHdr->steRhtOpr->stFlags.Imm)
                {
                    // Fill in the result token
                    (*lplpYYFcnStrRht)->tkToken.tkFlags.TknType   = TKT_FCNIMMED;
                    (*lplpYYFcnStrRht)->tkToken.tkFlags.ImmType   = lpMemDfnHdr->steRhtOpr->stFlags.ImmType;
////////////////////(*lplpYYFcnStrRht)->tkToken.tkFlags.NoDisplay =     // Filled in by YYAlloc
                    (*lplpYYFcnStrRht)->tkToken.tkData.tkLongest  = lpMemDfnHdr->steRhtOpr->stData.stLongest;
////////////////////(*lplpYYFcnStrRht)->tkToken.tkCharIndex       =     // Ignored
                } else
                {
                    // Fill in the result token
                    (*lplpYYFcnStrRht)->tkToken.tkFlags.TknType   = TKT_FCNNAMED;
////////////////////(*lplpYYFcnStrRht)->tkToken.tkFlags.ImmType   =     // Filled in by YYAlloc
////////////////////(*lplpYYFcnStrRht)->tkToken.tkFlags.NoDisplay =     // Filled in by YYAlloc
                    (*lplpYYFcnStrRht)->tkToken.tkData.tkSym      = lpMemDfnHdr->steRhtOpr;
////////////////////(*lplpYYFcnStrRht)->tkToken.tkCharIndex       =     // Ignored
                } // End IF/ELSE

                break;

            case NAMETYPE_OP1:
                // If the op1 is immediate, ...
                if (lpMemDfnHdr->steRhtOpr->stFlags.Imm)
                {
                    // Fill in the result token
                    (*lplpYYFcnStrRht)->tkToken.tkFlags.TknType   = TKT_OP1IMMED;
                    (*lplpYYFcnStrRht)->tkToken.tkFlags.ImmType   = lpMemDfnHdr->steRhtOpr->stFlags.ImmType;
////////////////////(*lplpYYFcnStrRht)->tkToken.tkFlags.NoDisplay =     // Filled in by YYAlloc
                    (*lplpYYFcnStrRht)->tkToken.tkData.tkLongest  = lpMemDfnHdr->steRhtOpr->stData.stLongest;
////////////////////(*lplpYYFcnStrRht)->tkToken.tkCharIndex       =     // Ignored
                } else
                {
                    // Fill in the result token
                    (*lplpYYFcnStrRht)->tkToken.tkFlags.TknType   = TKT_OP1NAMED;
////////////////////(*lplpYYFcnStrRht)->tkToken.tkFlags.ImmType   =     // Filled in by YYAlloc
////////////////////(*lplpYYFcnStrRht)->tkToken.tkFlags.NoDisplay =     // Filled in by YYAlloc
                    (*lplpYYFcnStrRht)->tkToken.tkData.tkSym      = lpMemDfnHdr->steRhtOpr;
////////////////////(*lplpYYFcnStrRht)->tkToken.tkCharIndex       =     // Ignored
                } // End IF/ELSE

                break;

            case NAMETYPE_OP2:
                // If the op2 is immediate, ...
                if (lpMemDfnHdr->steRhtOpr->stFlags.Imm)
                {
                    // Fill in the result token
                    (*lplpYYFcnStrRht)->tkToken.tkFlags.TknType   = TKT_OP2IMMED;
                    (*lplpYYFcnStrRht)->tkToken.tkFlags.ImmType   = lpMemDfnHdr->steRhtOpr->stFlags.ImmType;
////////////////////(*lplpYYFcnStrRht)->tkToken.tkFlags.NoDisplay =     // Filled in by YYAlloc
                    (*lplpYYFcnStrRht)->tkToken.tkData.tkLongest  = lpMemDfnHdr->steRhtOpr->stData.stLongest;
////////////////////(*lplpYYFcnStrRht)->tkToken.tkCharIndex       =     // Ignored
                } else
                {
                    // Fill in the result token
                    (*lplpYYFcnStrRht)->tkToken.tkFlags.TknType   = TKT_OP2NAMED;
////////////////////(*lplpYYFcnStrRht)->tkToken.tkFlags.ImmType   =     // Filled in by YYAlloc
////////////////////(*lplpYYFcnStrRht)->tkToken.tkFlags.NoDisplay =     // Filled in by YYAlloc
                    (*lplpYYFcnStrRht)->tkToken.tkData.tkSym      = lpMemDfnHdr->steRhtOpr;
////////////////////(*lplpYYFcnStrRht)->tkToken.tkCharIndex       =     // Ignored
                } // End IF/ELSE

                break;

            defstop
                break;
        } // End SWITCH

        // Set the token count
        (*lplpYYFcnStrRht)->TknCount = 1;
    } else
        // Mark as unallocated
        *lplpYYFcnStrRht = NULL;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbFcn); lpMemDfnHdr = NULL;
} // End GetOperands


//***************************************************************************
//  $ExecFcnGlb_EM_YY
//
//  Execute a function in global memory
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecFcnGlb_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecFcnGlb_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     HGLOBAL hGlbFcn,               // Handle to function strand
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    LPFCNARRAY_HEADER lpMemHdrFcn = NULL;   // Ptr to function array header
    NAME_TYPES        fnNameType;           // Function array NAMETYPE
    UINT              tknNELM;              // # tokens in the function array
    LPPL_YYSTYPE      lpYYFcnStr,           // Ptr to function strand
                      lpYYRes;              // Ptr to the result
    LPTOKEN           lptkAxis2;            // Ptr to secondary axis token (may be NULL)

    // Lock the memory to get a ptr to it
    lpMemHdrFcn = MyGlobalLockFcn (hGlbFcn);

#define lpHeader            lpMemHdrFcn
    // Save the NAMETYPE_xxx and # tokens
    fnNameType = lpHeader->fnNameType;
    tknNELM    = lpHeader->tknNELM;
#undef  lpHeader

    // Skip over the header to the data (PL_YYSTYPEs)
    lpYYFcnStr = FcnArrayBaseToData (lpMemHdrFcn);

    // Check for axis operator
    lptkAxis2 = CheckAxisOper (lpYYFcnStr);

    // If two axis tokens, that's an error
    if (lptkAxis NE NULL && lptkAxis2 NE NULL)
        goto AXIS_SYNTAX_EXIT;
    else
    // If the secondary only, use it
    if (lptkAxis2 NE NULL)
        lptkAxis = lptkAxis2;

    // If it's a Train, ...
    if (fnNameType EQ NAMETYPE_TRN)
    {
        if (lptkAxis NE NULL)
            lpYYRes =
              PrimFnSyntaxError_EM (&lpYYFcnStr->tkToken APPEND_NAME_ARG);
        else
            // Execute as Train, skipping over the monadic operator
            lpYYRes =
              ExecTrain_EM_YY (lptkLftArg,      // Ptr to left arg token
                              &lpYYFcnStr[1],   // Ptr to function strand
                               lptkRhtArg,      // Ptr to right arg token
                               tknNELM - 1);    // # elements in the train
    } else
        // The contents of the global memory object consist of
        //   a series of PL_YYSTYPEs in RPN order.
        lpYYRes =
          ExecFuncStr_EM_YY (lptkLftArg,    // Ptr to left arg token
                             lpYYFcnStr,    // Ptr to function strand
                             lptkRhtArg,    // Ptr to right arg token
                             lptkAxis);     // Ptr to axis token
    // We no longer need this ptr
    MyGlobalUnlock (hGlbFcn); lpMemHdrFcn = NULL;

    // If the result is valid, ...
    if (lpYYRes NE NULL
     && !IsTokenNoValue (&lpYYRes->tkToken))
        // Change the SynObj
        lpYYRes->tkToken.tkSynObj = soA;

    return lpYYRes;
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStr->tkToken);
    return NULL;
} // End ExecFcnGlb_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ExecTrain_EM_YY
//
//  Execute a Train
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecTrain_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecTrain_EM_YY
    (LPTOKEN      lptkLftArg,       // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStr,       // Ptr to function strand
     LPTOKEN      lptkRhtArg,       // Ptr to right arg token
     UINT         tknNELM)          // # elements in the Train

{
    LPPL_YYSTYPE lpYYRes  = NULL,   // Ptr to the result
                 lpYYRes1 = NULL,   // Ptr to temporary result #1
                 lpYYRes2 = NULL;   // ...                      2

    // Note that the rightmost function is at the start of Train (lpYYFcnStr[0])
    //   and the leftmost function is at the end of the Train (lpYYFcnStr[tknNELM - 1]).

    Assert (tknNELM > 1);

    // Split cases based upon the # elements in the Train
    switch (tknNELM)
    {               //         Monadic                  Dyadic
                    //   (g h)R       (A h)R     L(g h)R      L(A h)R
                    //  -------------------------------------------------
////////case 2:     //  R g h R        A         L g h R
        case 2:     //    g h R        A         g L h R        A
            // If the lefthand token (A or g) is an array, ...
            if (IsTknTypeVar (lpYYFcnStr[1].tkToken.tkFlags.TknType))
            {
                // Allocate a new YYRes
                lpYYRes = YYAlloc ();

                // Copy the PL_YYSTYPE
                YYCopy (lpYYRes, &lpYYFcnStr[1]);

                // Increment the RefCnt
                DbgIncrRefCntTkn (&lpYYRes->tkToken);
            } else
            {
                // Execute the righthand function (h)
                //   between the (optional) left arg (L) and the right arg (R)
                //   to yield   h R   or   L h R
                lpYYRes1 = ExecFuncStr_EM_YY (lptkLftArg,       // Ptr to left arg token
                                             &lpYYFcnStr[0],    // Ptr to function strand
                                              lptkRhtArg,       // Ptr to right arg token
                                              NULL);            // Ptr to axis token
                if (lpYYRes1 NE NULL)
                {
                    // Check for NoValue
                    if (IsTokenNoValue (&lpYYRes1->tkToken))
                    {
                        // Free the YYRes (but not the storage)
                        YYFree (lpYYRes1); lpYYRes1 = NULL;

                        goto VALUE_EXIT;
                    } // End IF
#ifdef DEBUG
                    // Decrement the SI level of lpYYRes1
                    //   so YYResIsEmpty won't complain
                    lpYYRes1->SILevel--;
#endif
                    // If this derived function is monadic, ...
                    if (lptkLftArg EQ NULL)
                        lptkLftArg = lptkRhtArg;

                    // Execute the lefthand function (g)
                    //   on the previous result
                    lpYYRes = ExecFuncStr_EM_YY (NULL,              // Ptr to left arg token
                                                &lpYYFcnStr[1],     // Ptr to function strand
                                                &lpYYRes1->tkToken, // Ptr to right arg token
                                                 NULL);             // Ptr to axis token
#ifdef DEBUG
                    // Restore the SI level of lpYYRes1
                    lpYYRes1->SILevel++;
#endif
                } // End IF
            } // End IF/ELSE

            break;

                    //           Monadic                        Dyadic
                    //   (f g h)R       (A g h)R     L(f g h)R          L(A g h)R
                    //  ---------------------------------------------------------
        case 3:     //  (f R) g (h R)    A g h R    (L f R) g (L h R)   A g L h R
            // If the lefthand token (A or f) is an array, ...
            if (IsTknTypeVar (lpYYFcnStr[2].tkToken.tkFlags.TknType))
            {
                // Execute the righthand function (h)
                //   between the (optional) left arg (L) and the right arg (R)
                //   to yield   h R   or   L h R
                lpYYRes1 = ExecFuncStr_EM_YY (lptkLftArg,       // Ptr to left arg token
                                             &lpYYFcnStr[0],    // Ptr to function strand
                                              lptkRhtArg,       // Ptr to right arg token
                                              NULL);            // Ptr to axis token
                if (lpYYRes1 NE NULL)
                {
                    // Check for NoValue
                    if (IsTokenNoValue (&lpYYRes1->tkToken))
                    {
                        // Free the YYRes (but not the storage)
                        YYFree (lpYYRes1); lpYYRes1 = NULL;

                        goto VALUE_EXIT;
                    } // End IF

#ifdef DEBUG
                    // Decrement the SI level of lpYYRes1
                    //   so YYResIsEmpty won't complain
                    lpYYRes1->SILevel--;
#endif
                    // Execute the lefthand function (g)
                    //   between the array (A) and the previous result
                    //   to yield   A g h R   or   A g L h R
                    lpYYRes = ExecFuncStr_EM_YY (&lpYYFcnStr[2].tkToken,    // Ptr to left arg token
                                                 &lpYYFcnStr[1],            // Ptr to function strand
                                                 &lpYYRes1->tkToken,        // Ptr to right arg token
                                                  NULL);                    // Ptr to axis token
#ifdef DEBUG
                    // Restore the SI level of lpYYRes1
                    lpYYRes1->SILevel++;
#endif
                } // End IF
            } else
            {
                // Execute the righthand function (h) between
                //   the (optional) left (L) and right (R) args
                //   to yield   h R   or   L h R
                lpYYRes1 = ExecFuncStr_EM_YY (lptkLftArg,       // Ptr to left arg token
                                             &lpYYFcnStr[0],    // Ptr to function strand
                                              lptkRhtArg,       // Ptr to right arg token
                                              NULL);            // Ptr to axis token
                if (lpYYRes1 NE NULL)
                {
                    // Check for NoValue
                    if (IsTokenNoValue (&lpYYRes1->tkToken))
                    {
                        // Free the YYRes (but not the storage)
                        YYFree (lpYYRes1); lpYYRes1 = NULL;

                        goto VALUE_EXIT;
                    } // End IF
#ifdef DEBUG
                    // Decrement the SI level of lpYYRes1
                    //   so YYResIsEmpty won't complain
                    lpYYRes1->SILevel--;
#endif
                    // Execute the lefthand function (f) between
                    //   the (optional) left (L) and right (R) args
                    //   to yield   f R   or   L f R
                    lpYYRes2 = ExecFuncStr_EM_YY (lptkLftArg,       // Ptr to left arg token
                                                 &lpYYFcnStr[2],    // Ptr to function strand
                                                  lptkRhtArg,       // Ptr to right arg token
                                                  NULL);            // Ptr to axis token
                    if (lpYYRes2 NE NULL)
                    {
                        // Check for NoValue
                        if (IsTokenNoValue (&lpYYRes2->tkToken))
                        {
                            // Free the YYRes (but not the storage)
                            YYFree (lpYYRes2); lpYYRes2 = NULL;

                            goto VALUE_EXIT;
                        } // End IF
#ifdef DEBUG
                        // Decrement the SI level of lpYYRes2
                        //   so YYResIsEmpty won't complain
                        lpYYRes2->SILevel--;
#endif
                        // Execute the middle function (g) between
                        //   the two previous results
                        //   to yield   (f R) g (h R)   or   (L f R) g (L h R)
                        lpYYRes = ExecFuncStr_EM_YY (&lpYYRes2->tkToken,    // Ptr to left arg token
                                                     &lpYYFcnStr[1],        // Ptr to function strand
                                                     &lpYYRes1->tkToken,    // Ptr to right arg token
                                                      NULL);                // Ptr to axis token
#ifdef DEBUG
                        // Restore the SI level of lpYYRes2
                        lpYYRes2->SILevel++;
#endif
                    } // End IF
#ifdef DEBUG
                    // Restore the SI level of lpYYRes1
                    lpYYRes1->SILevel++;
#endif
                } // End IF
            } // End IF/ELSE

            break;

        default:
            // If the # elements in the Train is Odd, ...
            if (BIT0 & tknNELM)
            {
                // Execute all but the two leftmost functions as a Train
                //   between the left and right args
                lpYYRes1 = ExecTrain_EM_YY (lptkLftArg,     // Ptr to left arg token
                                           &lpYYFcnStr[0],  // Ptr to function strand
                                            lptkRhtArg,     // Ptr to right arg token
                                            tknNELM - 2);   // # elements in the Train
                if (lpYYRes1 NE NULL)
                {
                    // Check for NoValue
                    if (IsTokenNoValue (&lpYYRes1->tkToken))
                    {
                        // Free the YYRes (but not the storage)
                        YYFree (lpYYRes1); lpYYRes1 = NULL;

                        goto VALUE_EXIT;
                    } // End IF
#ifdef DEBUG
                    // Decrement the SI level of lpYYRes1
                    //   so YYResIsEmpty won't complain
                    lpYYRes1->SILevel--;
#endif
                    // If the leftmost token is an array, ...
                    if (IsTknTypeVar (lpYYFcnStr[tknNELM - 1].tkToken.tkFlags.TknType))
                    {
                        // Allocate a new YYRes
                        lpYYRes2 = YYAlloc ();

                        // Copy the PL_YYSTYPE
                        YYCopy (lpYYRes2, &lpYYFcnStr[tknNELM - 1]);

                        // Increment the RefCnt
                        DbgIncrRefCntTkn (&lpYYRes2->tkToken);
                    } else
                        // Execute the leftmost function
                        //   between the left and right args
                        lpYYRes2 = ExecFuncStr_EM_YY (lptkLftArg,               // Ptr to left arg token
                                                     &lpYYFcnStr[tknNELM - 1],  // Ptr to function strand
                                                      lptkRhtArg,               // Ptr to right arg token
                                                      NULL);                    // Ptr to axis token
                    if (lpYYRes2 NE NULL)
                    {
                        // Check for NoValue
                        if (IsTokenNoValue (&lpYYRes2->tkToken))
                        {
                            // Free the YYRes (but not the storage)
                            YYFree (lpYYRes2); lpYYRes2 = NULL;

                            goto VALUE_EXIT;
                        } // End IF
#ifdef DEBUG
                        // Decrement the SI level of lpYYRes2
                        //   so YYResIsEmpty won't complain
                        lpYYRes2->SILevel--;
#endif
                        // Execute the next to the leftmost function
                        //   between the two previous results
                        lpYYRes = ExecFuncStr_EM_YY (&lpYYRes2->tkToken,        // Ptr to left arg token
                                                     &lpYYFcnStr[tknNELM - 2],  // Ptr to function strand
                                                     &lpYYRes1->tkToken,        // Ptr to right argtoken
                                                      NULL);                    // Ptr to axis token
#ifdef DEBUG
                        // Restore the SI level of lpYYRes2
                        lpYYRes2->SILevel++;
#endif
                    } // End IF
#ifdef DEBUG
                    // Restore the SI level of lpYYRes1
                    lpYYRes1->SILevel++;
#endif
                } // End IF
            } else
            // If the # elements in the Train is Even, ...
            {
                // If the lefthand token (A or g) is an array, ...
                if (IsTknTypeVar (lpYYFcnStr[tknNELM - 1].tkToken.tkFlags.TknType))
                {
                    // Allocate a new YYRes
                    lpYYRes = YYAlloc ();

                    // Copy the PL_YYSTYPE
                    YYCopy (lpYYRes, &lpYYFcnStr[tknNELM - 1]);

                    // Increment the RefCnt
                    DbgIncrRefCntTkn (&lpYYRes->tkToken);
                } else
                {
                    // Execute all but the leftmost function as a Train
                    //   between the (optional) left (L) and right (R) args
                    lpYYRes1 = ExecTrain_EM_YY (lptkLftArg,     // Ptr to left arg token
                                               &lpYYFcnStr[0],  // Ptr to function strand
                                                lptkRhtArg,     // Ptr to right arg token
                                                tknNELM - 1);   // # elements in the Train
                    if (lpYYRes1 NE NULL)
                    {
                        // Check for NoValue
                        if (IsTokenNoValue (&lpYYRes1->tkToken))
                        {
                            // Free the YYRes (but not the storage)
                            YYFree (lpYYRes1); lpYYRes1 = NULL;

                            goto VALUE_EXIT;
                        } // End IF
#ifdef DEBUG
                        // Decrement the SI level of lpYYRes1
                        //   so YYResIsEmpty won't complain
                        lpYYRes1->SILevel--;
#endif
                        // Execute the leftmost function between
                        //   the left arg and the previous result
                        lpYYRes = ExecFuncStr_EM_YY (lptkLftArg,                // Ptr to left arg token
                                                    &lpYYFcnStr[tknNELM - 1],   // Ptr to function strand
                                                    &lpYYRes1->tkToken,         // Ptr to right arg token
                                                     NULL);                     // Ptr to axis token
#ifdef DEBUG
                        // Restore the SI level of lpYYRes1
                        lpYYRes1->SILevel++;
#endif
                    } // End IF
                } // End IF/ELSE
            } // End IF/ELSE

            break;
    } // End SWITCH

    goto NORMAL_EXIT;

VALUE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
                              &lpYYFcnStr->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    // If a previous result is active, ...
    if (lpYYRes1 NE NULL)
    {
        // Free the previous result
        FreeResult (lpYYRes1); YYFree (lpYYRes1); lpYYRes1 = NULL;
    } // End IF

    if (lpYYRes2 NE NULL)
    {
        // Free the previous result
        FreeResult (lpYYRes2); YYFree (lpYYRes2); lpYYRes2 = NULL;
    } // End IF

    return lpYYRes;
} // End ExecTrain_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ExecFuncStr_EM_YY
//
//  Execute a function strand
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecFuncStr_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecFuncStr_EM_YY
    (LPTOKEN      lptkLftArg,       // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStr,       // Ptr to function strand
     LPTOKEN      lptkRhtArg,       // Ptr to right arg token
     LPTOKEN      lptkAxis)         // Ptr to axis token (may be NULL)

{
    return
      ExecFuncStrLine_EM_YY (lptkLftArg,    // Ptr to left arg token (may be NULL if niladic/monadic)
                             lpYYFcnStr,    // Ptr to function strand
                             lptkRhtArg,    // Ptr to right arg token (may be NULL if niladic)
                             lptkAxis,      // Ptr to axis token (may be NULL)
                             LINENUM_ONE);  // Starting line # type (see LINE_NUMS)
} // End ExecFuncStr_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ExecFuncStrLine_EM_YY
//
//  Execute a function strand at a given line # type
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecFuncStrLine_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecFuncStrLine_EM_YY
    (LPTOKEN      lptkLftArg,       // Ptr to left arg token (may be NULL if niladic/monadic)
     LPPL_YYSTYPE lpYYFcnStr,       // Ptr to function strand
     LPTOKEN      lptkRhtArg,       // Ptr to right arg token (may be NULL if niladic)
     LPTOKEN      lptkAxis,         // Ptr to axis token (may be NULL)
     LINE_NUMS    startLineType)    // Starting line type (see LINE_NUMS)

{
    LPPRIMFNS    lpPrimFn;          // Ptr to function address
    HGLOBAL      hGlbFcn;           // Function strand or user-defined function/operator global memory handle
    LPPL_YYSTYPE lpYYRes,           // Ptr to the result
                 lpYYFcnStrLft,     // Ptr to left operand function strand (may be NULL if not an operator)
                 lpYYFcnStrRht;     // Ptr to right operand function strand (may be NULL if monadic operator or not an operator)

    // Note that lptkAxis is not NULL for (say) L {take}[1]{each} R

    // Split cases based upon the type of the first token
    switch (lpYYFcnStr->tkToken.tkFlags.TknType)
    {
        case TKT_OP1IMMED:
        case TKT_OP3IMMED:
            return ExecOp1_EM_YY
                   (lptkLftArg,     // Ptr to left arg token
                    lpYYFcnStr,     // Ptr to operator function strand
                    lptkRhtArg);    // Ptr to right arg token

        case TKT_OP2IMMED:
        case TKT_OPJOTDOT:
            return ExecOp2_EM_YY
                   (lptkLftArg,     // Ptr to left arg token
                    lpYYFcnStr,     // Ptr to operator function strand
                    lptkRhtArg);    // Ptr to right arg token

        case TKT_FCNIMMED:  // Either F or F[X]
            Assert (lpYYFcnStr->TknCount EQ 1
                 || lpYYFcnStr->TknCount EQ 2);

            // Check for axis operator
            lptkAxis = CheckAxisOper (lpYYFcnStr);

            lpPrimFn = PrimFnsTab[SymTrans (&lpYYFcnStr->tkToken)];
            if (lpPrimFn EQ NULL)
                goto SYNTAX_EXIT;

            return (*lpPrimFn) (lptkLftArg,                 // Ptr to left arg token (may be NULL if monadic)
                               &lpYYFcnStr->tkToken,        // Ptr to function token
                                lptkRhtArg,                 // Ptr to right arg token
                                lptkAxis);                  // Ptr to axis token (may be NULL)
        case TKT_FCNARRAY:
        case TKT_FCNAFO:
        case TKT_OP1AFO:
        case TKT_OP2AFO:
                                // 1.  User-defined operator
                                //   e.g., Z{is}L (F FOO G) R
                                //         +foo- 1 2
                                // 2.  User-defined function
                                //   e.g., Z{is}FOO R
                                //         3{jot}foo 1 2
                                // 3.  User-defined function/operator w/axis operator
                                //   e.g., Z{is}FOO[X] R
                                //         foo[1] 1 2
                                // 4.  Monadic operator derived function
                                //   e.g., f{is}/[1]
                                //         +f 2 3{rho}{iota}6
            // Get the global memory handle
            hGlbFcn = GetGlbDataToken (&lpYYFcnStr->tkToken);

            // tkData is a valid HGLOBAL function array
            //   or user-defined function/operator
            Assert (IsGlbTypeFcnDir_PTB (hGlbFcn)
                 || IsGlbTypeDfnDir_PTB (hGlbFcn));

            // Split cases based upon the array signature
            switch (GetSignatureGlb_PTB (hGlbFcn))
            {
                case FCNARRAY_HEADER_SIGNATURE:
                    // Check for axis operator
                    lptkAxis = CheckAxisOper (lpYYFcnStr);

                    // Execute a function array global memory handle
                    return ExecFcnGlb_EM_YY (lptkLftArg,    // Ptr to left arg token (may be NULL if niladic/monadic)
                                             hGlbFcn,       // Function array global memory handle
                                             lptkRhtArg,    // Ptr to right arg token
                                             lptkAxis);     // Ptr to axis token (may be NULL)
                case DFN_HEADER_SIGNATURE:
                    // Execute a user-defined function/operator global memory handle
                    return ExecDfnGlb_EM_YY (hGlbFcn,       // User-defined function/operator global memory handle
                                             lptkLftArg,    // Ptr to left arg token (may be NULL if monadic)
                                             lpYYFcnStr,    // Ptr to function strand
                                             lptkAxis,      // Ptr to axis token (may be NULL -- used only if function strand is NULL)
                                             lptkRhtArg,    // Ptr to right arg token
                                             startLineType); // Starting line type (see LINE_NUMS)
                defstop
                    break;
            } // End SWITCH

            DbgStop ();             // We should never get here

        case TKT_FCNNAMED:
        case TKT_OP1NAMED:
        case TKT_OP2NAMED:
        case TKT_OP3NAMED:
            // Check for axis operator
            lptkAxis = CheckAxisOper (lpYYFcnStr);

            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpYYFcnStr->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If the SYMENTRY is an immediate, ...
            if (lpYYFcnStr->tkToken.tkData.tkSym->stFlags.Imm)
            {
                // Check for axis operator
                lptkAxis = CheckAxisOper (lpYYFcnStr);

                lpPrimFn = PrimFnsTab[SymTrans (&lpYYFcnStr->tkToken)];
                if (lpPrimFn EQ NULL)
                    goto SYNTAX_EXIT;

                return (*lpPrimFn) (lptkLftArg,                 // Ptr to left arg token (may be NULL if niladic/monadic)
                                   &lpYYFcnStr->tkToken,        // Ptr to function token
                                    lptkRhtArg,                 // Ptr to right arg token (may be NULL if niladic)
                                    lptkAxis);                  // Ptr to axis token (may be NULL)
            } else
            {
                // Get the global memory handle or function address if direct
                hGlbFcn  =
                lpPrimFn = lpYYFcnStr->tkToken.tkData.tkSym->stData.stNameFcn;

                // Check for internal functions
                if (lpYYFcnStr->tkToken.tkData.tkSym->stFlags.FcnDir)
                    return (*lpPrimFn) (lptkLftArg,             // Ptr to left arg token (may be NULL if monadic)
                                       &lpYYFcnStr->tkToken,    // Ptr to function token
                                        lptkRhtArg,             // Ptr to right arg token (may be NULL if niladic)
                                        lptkAxis);              // Ptr to axis token (may be NULL)
                // tkData is a valid HGLOBAL function array
                //   or user-defined function/operator
                Assert (IsGlbTypeFcnDir_PTB (hGlbFcn)
                     || IsGlbTypeDfnDir_PTB (hGlbFcn));

                // Split cases based upon the array signature
                switch (GetSignatureGlb_PTB (hGlbFcn))
                {
                    case FCNARRAY_HEADER_SIGNATURE:
                        // Execute a function array global memory handle
                        return ExecFcnGlb_EM_YY (lptkLftArg,    // Ptr to left arg token (may be NULL if niladic/monadic)
                                                 hGlbFcn,       // Function array global memory handle
                                                 lptkRhtArg,    // Ptr to right arg token (may be NULL if niladic)
                                                 lptkAxis);     // Ptr to axis token (may be NULL)
                    case DFN_HEADER_SIGNATURE:
                        // Execute a user-defined function/operator global memory handle
                        return ExecDfnGlb_EM_YY (hGlbFcn,       // User-defined function/operator global memory handle
                                                 lptkLftArg,    // Ptr to left arg token (may be NULL if niladic/monadic)
                                                 lpYYFcnStr,    // Ptr to function strand
                                                 lptkAxis,      // Ptr to axis token (may be NULL -- used only if function strand is NULL)
                                                 lptkRhtArg,    // Ptr to right arg token (may be NULL if niladic)
                                                 startLineType); // Starting line type (see LINE_NUMS)
                    defstop
                        return NULL;
                } // End SWITCH
            } // End IF/ELSE

        case TKT_DELAFO:
            // Get the global memory handle
            hGlbFcn = lpYYFcnStr->tkToken.tkData.tkGlbData;

            // Setup left and right operands (if present)
            GetOperands (hGlbFcn, &lpYYFcnStrLft, &lpYYFcnStrRht);

            // Call common routine
            lpYYRes =
              ExecDfnOprGlb_EM_YY (hGlbFcn,         // User-defined function/operator global memory handle
                                   lptkLftArg,      // Ptr to left arg token (may be NULL if monadic)
                                   lpYYFcnStrLft,   // Ptr to left operand function strand (may be NULL if not an operator and no axis)
                                   lpYYFcnStr,      // Ptr to function strand (may be NULL if not an operator and no axis)
                                   lpYYFcnStrRht,   // Ptr to right operand function strand (may be NULL if not an operator and no axis)
                                   lptkAxis,        // Ptr to axis token (may be NULL -- used only if function strand is NULL)
                                   lptkRhtArg,      // Ptr to right arg token
                                   startLineType);  // Starting line type (see LINE_NUMS)
            // If we allocated it, ...
            if (lpYYFcnStrLft NE NULL)
                // Free the left operand YYRes
                YYFree (lpYYFcnStrLft);

            // If we allocated it, ...
            if (lpYYFcnStrRht NE NULL)
                // Free the right operand YYRes
                YYFree (lpYYFcnStrRht);

            return lpYYRes;

        defstop
            goto ERROR_EXIT;
    } // End SWITCH

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStr->tkToken);
ERROR_EXIT:
    return NULL;
} // End ExecFuncStrLine_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ExecOp1_EM_YY
//
//  Execute an immediate monadic operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecOp1_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecOp1_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    // Split cases based upon the type of the monadic operator
    switch (lpYYFcnStrOpr->tkToken.tkData.tkChar)
    {
        case UTF16_SLASH:           // Reduction along the last coordinate
        case UTF16_SLASHBAR:        // Reduction along the first coordinate
        case INDEX_OPSLASH:         // Reduction along the last coordinate
        case INDEX_OPSLASHBAR:      // Reduction along the first coordinate
            return PrimOpSlash_EM_YY (lptkLftArg,   // Ptr to left arg token (may be NULL if monadic)
                                      lpYYFcnStrOpr,// Ptr to operator function strand
                                      lptkRhtArg);  // Ptr to right arg token
        case UTF16_SLOPE:           // Scan along the last coordinate
        case UTF16_SLOPEBAR:        // Scan along the first coordinate
        case INDEX_OPSLOPE:         // Scan along the last coordinate
        case INDEX_OPSLOPEBAR:      // Scan along the first coordinate
            return PrimOpSlope_EM_YY (lptkLftArg,               // Ptr to left arg token (may be NULL if monadic)
                                      lpYYFcnStrOpr,            // Ptr to operator function strand
                                      lptkRhtArg);              // Ptr to right arg token
        case UTF16_STILETILDE:      // Partition
            return PrimFnNonceError_EM (&lpYYFcnStrOpr->tkToken APPEND_NAME_ARG);

            DbgBrk ();              // ***FINISHME*** -- UTF16_STILETILDE






            return NULL;

        case UTF16_DOWNSHOESTILE:   // Multiset
            return PrimOpDownShoeStile_EM_YY (lptkLftArg,       // Ptr to left arg token (may be NULL if monadic)
                                              lpYYFcnStrOpr,    // Ptr to operator function strand
                                              lptkRhtArg);      // Ptr to right arg token
        case UTF16_DIERESIS:        // Each
            return PrimOpDieresis_EM_YY (lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
                                         lpYYFcnStrOpr,         // Ptr to operator function strand
                                         lptkRhtArg);           // Ptr to right arg token
        case UTF16_DIERESISTILDE:   // Commute/Duplicate
            return PrimOpDieresisTilde_EM_YY (lptkLftArg,       // Ptr to left arg token (may be NULL if monadic)
                                              lpYYFcnStrOpr,    // Ptr to operator function strand
                                              lptkRhtArg);      // Ptr to right arg token
        case UTF16_CIRCLEMIDDLEDOT: // Null Op
            return PrimOpCircleMiddleDot_EM_YY (lptkLftArg,     // Ptr to left arg token (may be NULL if monadic)
                                                lpYYFcnStrOpr,  // Ptr to operator function strand
                                                lptkRhtArg);    // Ptr to right arg token
        case INDEX_OPTRAIN:         // Train
            // Execute as Train, skipping over the monadic operator
            return ExecTrain_EM_YY (lptkLftArg,                     // Ptr to left arg token
                                   &lpYYFcnStrOpr[1],               // Ptr to function strand
                                    lptkRhtArg,                     // Ptr to right arg token
                                    lpYYFcnStrOpr->TknCount - 1);   // # elements in the train
        defstop
            return NULL;
    } // End SWITCH
} // End ExecOp1_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ExecOp2_EM_YY
//
//  Execute an immediate dyadic operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecOp2_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecOp2_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    LPTOKEN lptkAxis;                   // Ptr to axis token (may be NULL)

    // Check for axis operator
    lptkAxis = CheckAxisOper (lpYYFcnStrOpr);

    // Split cases based upon the type of the dyadic operator
    switch (lpYYFcnStrOpr->tkToken.tkData.tkChar)
    {
        case UTF16_JOTDOT:          // Outer product
            return PrimOpJotDot_EM_YY (lptkLftArg,          // Ptr to left arg token (may be NULL if monadic)
                                       lpYYFcnStrOpr,       // Ptr to operator function strand
                                       lptkRhtArg);         // Ptr to right arg token
        case UTF16_DOT:             // Inner product
            return PrimOpDot_EM_YY    (lptkLftArg,          // Ptr to left arg token (may be NULL if monadic)
                                       lpYYFcnStrOpr,       // Ptr to operator function strand
                                       lptkRhtArg);         // Ptr to right arg token
        case UTF16_JOT:             // Compose
        case UTF16_JOT2:            // Compose
            return PrimOpJot_EM_YY    (lptkLftArg,          // Ptr to left arg token (may be NULL if monadic)
                                       lpYYFcnStrOpr,       // Ptr to operator function strand
                                       lptkRhtArg);         // Ptr to right arg token
        case UTF16_DIERESISCIRCLE:  // Composition
            return PrimOpDieresisCircle_EM_YY (lptkLftArg,      // Ptr to left arg token (may be NULL if monadic)
                                               lpYYFcnStrOpr,   // Ptr to operator function strand
                                               lptkRhtArg);     // Ptr to right arg token
        case UTF16_DIERESISDEL:     // Dual
            return PrimFnNonceError_EM (&lpYYFcnStrOpr->tkToken APPEND_NAME_ARG);

            DbgBrk ();              // ***FINISHME*** -- UTF16_DIERESISDEL






            return NULL;

        case UTF16_DIERESISJOT:     // Rank (as dyadic op)
            return PrimOpDieresisJot_EM_YY (lptkLftArg,     // Ptr to left arg token (may be NULL if monadic)
                                            lpYYFcnStrOpr,  // Ptr to operator function strand
                                            lptkRhtArg);    // Ptr to right arg token
        case UTF16_DIERESISDOWNTACK:// Convolution
            return PrimOpDieresisDownTack_EM_YY (lptkLftArg,    // Ptr to left arg token (may be NULL if monadic)
                                                 lpYYFcnStrOpr, // Ptr to operator function strand
                                                 lptkRhtArg);   // Ptr to right arg token
        case UTF16_DIERESISSTAR:    // Power
            return PrimFnNonceError_EM (&lpYYFcnStrOpr->tkToken APPEND_NAME_ARG);

            DbgBrk ();              // ***FINISHME*** -- UTF16_DIERESISSTAR






            return NULL;

        case UTF16_VARIANT:         // Variant
            return PrimOpVariant_EM_YY (lptkLftArg,         // Ptr to left arg token (may be NULL if monadic)
                                        lpYYFcnStrOpr,      // Ptr to operator function strand
                                        lptkRhtArg);        // Ptr to right arg token
        defstop
            return NULL;
    } // End SWITCH
} // End ExecOp2_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: execfns.c
//***************************************************************************
