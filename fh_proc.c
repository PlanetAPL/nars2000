//***************************************************************************
//  NARS2000 -- Parser Grammar Functions for function headers
//***************************************************************************

//***************************************************************************
//  Parse a line of pre-tokenized tokens.
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
//  $ValidSysName
//
//  Validate the name as a system variable name
//***************************************************************************

BOOL ValidSysName
    (LPTOKEN lptkName)      // Ptr to name token

{
    return (lptkName->tkData.tkSym->stFlags.ObjName EQ OBJNAME_SYS);
} // End ValidSysName


//***************************************************************************
//  $InitHdrStrand
//
//  Initialize the header strand stack.
//***************************************************************************

void InitHdrStrand
    (LPFH_YYSTYPE lpYYArg)              // Ptr to the incoming argument

{
    LPFHLOCALVARS lpfhLocalVars;        // Ptr to local fhLocalVars

    // Get this thread's LocalVars ptr
    lpfhLocalVars = (LPFHLOCALVARS) TlsGetValue (dwTlsFhLocalVars);

    // Set the base of this strand to the next available location
    lpYYArg->lpYYStrandBase       =
    lpfhLocalVars->lpYYStrandBase = lpfhLocalVars->lpYYStrandNext;
} // End InitHdrStrand


//***************************************************************************
//  $PushHdrStrand_YY
//
//  Push a header token onto the strand stack.
//***************************************************************************

LPFH_YYSTYPE PushHdrStrand_YY
    (LPFH_YYSTYPE lpYYArg)              // Ptr to the incoming argument

{
    LPFHLOCALVARS lpfhLocalVars;        // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpfhLocalVars = (LPFHLOCALVARS) TlsGetValue (dwTlsFhLocalVars);

    // Copy the strand base to the result
    lpYYArg->lpYYStrandBase = lpfhLocalVars->lpYYStrandBase;

    // Return our own position so the next user
    //   of this token can refer to it.
    if (!lpYYArg->lpYYStrandIndirect)
        lpYYArg->lpYYStrandIndirect = lpfhLocalVars->lpYYStrandNext;

    // Save this token on the strand stack
    //   and skip over it
    *lpfhLocalVars->lpYYStrandNext++ = *lpYYArg;

    return lpYYArg;
} // End PushHdrStrand_YY


//***************************************************************************
//  $MakeHdrStrand_YY
//
//  Make a header strand
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeHdrStrand_YY"
#else
#define APPEND_NAME
#endif

LPFH_YYSTYPE MakeHdrStrand_YY
    (LPFH_YYSTYPE lpYYArg)              // Ptr to incoming token

{
    LPFHLOCALVARS lpfhLocalVars;        // Ptr to local fhLocalVars

    // Get this thread's LocalVars ptr
    lpfhLocalVars = (LPFHLOCALVARS) TlsGetValue (dwTlsFhLocalVars);

    // Save the # elements in the strand
    lpYYArg->lpYYStrandBase->uStrandLen =
      (UINT) (lpfhLocalVars->lpYYStrandNext - lpYYArg->lpYYStrandBase);

    // Save the next position as the new base
    lpfhLocalVars->lpYYStrandBase = lpfhLocalVars->lpYYStrandNext;

    // Return the base of the current strand
    return lpYYArg->lpYYStrandBase;
} // End MakeHdrStrand_YY
#undef  APPEND_NAME


//***************************************************************************
//  $GetOprName_EM
//
//  Extract the function/operator/operand names from a list
//***************************************************************************

BOOL GetOprName_EM
    (LPFH_YYSTYPE lpYYArg)

{
    LPFHLOCALVARS lpfhLocalVars;        // Ptr to Function Header local vars

    // Get this thread's LocalVars ptr
    lpfhLocalVars = (LPFHLOCALVARS) TlsGetValue (dwTlsFhLocalVars);

    // Split cases based upon the strand length
    switch (lpYYArg->uStrandLen)
    {
        case 1:         // Function
            lpfhLocalVars->DfnType     = DFNTYPE_FCN;
            lpfhLocalVars->fhNameType  = NAMETYPE_FN12;
            lpfhLocalVars->lpYYFcnName = &lpYYArg->lpYYStrandBase[0];
            lpfhLocalVars->offFcnName  =  lpYYArg->lpYYStrandBase[0].offTknIndex;

            return TRUE;

        case 2:         // Monadic operator
            lpfhLocalVars->DfnType     = DFNTYPE_OP1;
            lpfhLocalVars->fhNameType  = NAMETYPE_OP1;
            lpfhLocalVars->lpYYLftOpr  = &lpYYArg->lpYYStrandBase[0];
            lpfhLocalVars->lpYYFcnName = &lpYYArg->lpYYStrandBase[1];
            lpfhLocalVars->offFcnName  =  lpYYArg->lpYYStrandBase[1].offTknIndex;

            return TRUE;

        case 3:         // Dyadic operator
            lpfhLocalVars->DfnType     = DFNTYPE_OP2;
            lpfhLocalVars->fhNameType  = NAMETYPE_OP2;
            lpfhLocalVars->lpYYLftOpr  = &lpYYArg->lpYYStrandBase[0];
            lpfhLocalVars->lpYYFcnName = &lpYYArg->lpYYStrandBase[1];
            lpfhLocalVars->offFcnName  =  lpYYArg->lpYYStrandBase[1].offTknIndex;
            lpfhLocalVars->lpYYRhtOpr  = &lpYYArg->lpYYStrandBase[2];

            return TRUE;

        default:
            if (lpfhLocalVars->DisplayErr)
            {
                lpfhLocalVars->lptkNext->tkCharIndex = lpYYArg->tkToken.tkCharIndex;
                fh_yyerror (lpfhLocalVars, "syntax error");
            } // End IF

            return FALSE;
    } // End SWITCH
} // End GetOprName_EM


//***************************************************************************
//  End of File: fh_proc.c
//***************************************************************************
