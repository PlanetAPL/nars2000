//***************************************************************************
//  NARS2000 -- Parser Grammar Functions for executable lines
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2014 Sudley Place Software

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
//  $SymbTypeVFO
//
//  Get the type ('V', 'F', '1', '2', '3', or '?') of a symbol table name token.
//  Used for TKT_VARNAMEDs.
//***************************************************************************

char SymbTypeVFO
    (LPTOKEN lptkNext)

{
    STFLAGS stFlags;

    // Get the symbol table flags
    stFlags = lptkNext->tkData.tkSym->stFlags;

    if (stFlags.Imm)        // IMMTYPE_BOOL, IMMTYPE_INT, IMMTYPE_CHAR, IMMTYPE_FLOAT, IMMTYPE_PRIMFCN, ...
    switch (stFlags.ImmType)
    {
        case IMMTYPE_BOOL:
        case IMMTYPE_INT:
        case IMMTYPE_CHAR:
        case IMMTYPE_FLOAT:
            return 'V';

        case IMMTYPE_PRIMFCN:
            return 'F';

        case IMMTYPE_PRIMOP1:
            return '1';

        case IMMTYPE_PRIMOP2:
            return '2';

        case IMMTYPE_PRIMOP3:
            return '3';

        defstop
            return '?';
    } // End IF/SWITCH

    // Split cases based upon the NAMETYPE_xxx
    switch (stFlags.stNameType)
    {
        case NAMETYPE_UNK:
            // If it's a NAMETYPE_UNK, then it's really a VALUE ERROR, but this
            //   isn't the time to signal that.
            return 'V';

        case NAMETYPE_VAR:
        case NAMETYPE_FN0:
            return 'V';

        case NAMETYPE_FN12:
        case NAMETYPE_TRN:
            return 'F';

        case NAMETYPE_OP1:
            return '1';

        case NAMETYPE_OP2:
            return '2';

        case NAMETYPE_OP3:
            return '3';

        case NAMETYPE_FILL1:
        case NAMETYPE_LST:
        case NAMETYPE_FILL2:
        default:
            return '?';             // SYNTAX ERROR
    } // End SWITCH
} // SymbTypeVFO


//***************************************************************************
//  $LookaheadSurround
//
//  Lookahead (backwards) in the token stream to see if
//    the next non-grouping symbol is a function or a var.
//  Used for TKT_RIGHTPARENs to see if it and the matching
//    TKT_LEFTPAREN surrounds a function, variable, monadic or dyadic
//    operator, or for / & \ to see if the next non-grouping
//    symbol is a function, variable, or dyadic operator.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- LookaheadSurround"
#else
#define APPEND_NAME
#endif

char LookaheadSurround
    (LPPLLOCALVARS lpplLocalVars)   // Ptr to local plLocalVars

{
    PLLOCALVARS plLocalVars;        // Local copy of outer PLLOCALVARS
    char        cRes;               // The result char
    UINT        uRet;               // The result from pl_yyparse

#ifdef DEBUG
    DbgMsgW2 (L"==Entering LookaheadSurround");
#endif
    // Copy outer lpplLocalVars
    plLocalVars = *lpplLocalVars;

    // Mark as looking ahead for token type inside surrounding parens
    plLocalVars.bLookAhead = TRUE;

    // Save the address of the stop token
    // Each right grouping symbol has the index of the matching
    //   left grouping symbol in its tkData.tkIndex
    plLocalVars.lptkStop = &plLocalVars.lptkStart[plLocalVars.lptkNext->tkData.tkIndex];

    // Parse the file, check for errors
    //   0 = success
    //   1 = YYABORT or APL error
    //   2 = memory xhausted
    uRet = pl_yyparse (&plLocalVars);

    // If the LookAhead char index is filled in, pass it on up
    if (plLocalVars.tkLACharIndex NE NEG1U)
        lpplLocalVars->tkLACharIndex = plLocalVars.tkLACharIndex;

    // Check for errors
    if (uRet)
        cRes = '?';
    else
        cRes = NAMETYPE_STRING[plLocalVars.plNameType];
#ifdef DEBUG
    dprintfWL9 (L"==Exiting  LookaheadSurround:  %c", cRes);
#endif
    return cRes;
} // End LookaheadSurround
#undef  APPEND_NAME


//***************************************************************************
//  $LookaheadAdjacent
//
//  Lookahead (backwards) in the token stream to see if
//    the matching grouping symbol is adjacent to a function, variable,
//    monadic or dyadic operator.
//  Used for TKT_RIGHTBRACKETs to see if this is indexing or axis.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- LookaheadAdjacent"
#else
#define APPEND_NAME
#endif

char LookaheadAdjacent
    (LPPLLOCALVARS lpplLocalVars,   // Ptr to local plLocalVars
     UBOOL         bSkipBrackets,   // TRUE iff we're to skip over left/right brackets first
     UBOOL         bCheckLeft,      // TRUE iff we're to check to the left of the current symbol
     UBOOL         bJotOpr)         // TRUE iff Jot is an operator

{
    PLLOCALVARS  plLocalVars;       // Local copy of outer PLLOCALVARS
    char         cRes;              // The result char
    LPSIS_HEADER lpSISCur;          // Ptr to current SIS

    if (bSkipBrackets)
    {
        DbgMsgW2 (L"==Entering LookaheadAdjacent:  TRUE");
    } else
    {
        DbgMsgW2 (L"==Entering LookaheadAdjacent:  FALSE");
    } // End IF/ELSE

    // Copy outer lpplLocalVars
    plLocalVars = *lpplLocalVars;

    // Each right grouping symbol has the index of the matching
    //   left grouping symbol in its tkData.tkIndex.
    // Get a ptr to the token adjacent to ("- 1") the matching left
    //   grouping symbol.
    if (bSkipBrackets)
        plLocalVars.lptkNext = &plLocalVars.lptkStart[plLocalVars.lptkNext->tkData.tkIndex - 1];
    else
        plLocalVars.lptkNext--; // Skip over the function/operator symbol
    while (TRUE)
    // Split cases based upon the token type
    switch (plLocalVars.lptkNext->tkFlags.TknType)
    {
        case TKT_FILLJOT:
            // If the caller doesn't want us to check to the left of the jot, ...
            if (!bCheckLeft)
            {
                cRes = 'J';             // Jot

                goto NORMAL_EXIT;
            } // End IF

            // Otherwise, fall through to check to the left

        case TKT_CHRSTRAND:
        case TKT_NUMSTRAND:
        case TKT_NUMSCALAR:
        case TKT_VARIMMED:
        case TKT_VARARRAY:
        case TKT_INPOUT:
            // Lookahead to see if the next token is a dyadic operator:
            //   if so, return 'F'; otherwise return 'V'.

            // If the next token is a dyadic op, ...
            if (LookaheadDyadicOp (&plLocalVars, &plLocalVars.lptkNext[-1]))
                cRes = 'F';             // Function
            else
                cRes = 'V';             // Variable

            goto NORMAL_EXIT;

        case TKT_DEL:                   // Del -- always a function
        case TKT_DELAFO:                // Del Anon -- either a monadic or dyadic operator, bound to its operands
            cRes = 'F';                 // Function

            goto NORMAL_EXIT;

        case TKT_DELDEL:                // Del Del  -- either a monadic or dyadic operator
            // Search up the SIS chain to see what this is
            lpSISCur = SrchSISForDfn (lpplLocalVars->lpMemPTD);

            // If the ptr is valid, ...
            if (lpSISCur)
            {
                // Split case based upon the function type
                switch (lpSISCur->DfnType)
                {
////////////////////case DFNTYPE_FCN:
////////////////////    cRes = 'F';     // Function
////////////////////
////////////////////    break;

                    case DFNTYPE_OP1:
                        cRes = '1';     // Monadic operator

                        break;

                    case DFNTYPE_OP2:
                        cRes = '2';     // Dyadic operator

                        break;

                    default:
                        cRes = '?';     // SYNTAX ERROR

                        break;
                } // End SWITCH
            } else
                cRes = '?';             // SYNTAX ERROR

            goto NORMAL_EXIT;

        case TKT_OP1IMMED:
        case TKT_OP1NAMED:
        case TKT_OPJOTDOT:
        case TKT_OP1AFO:
            cRes = '1';                 // Monadic operator

            goto NORMAL_EXIT;

        case TKT_OP2AFO:
            cRes = '2';                 // Dyadic operator

            goto NORMAL_EXIT;

        case TKT_OP2IMMED:
            // If the OP2 is JOT, ...
            if (IsAPLCharJot (plLocalVars.lptkNext->tkData.tkChar)
             && !bJotOpr)
            {
                // If the caller wants us to check to the left of the jot, ...
                if (bCheckLeft)
                {
                    // Lookahead to see if the next token is a dyadic operator:
                    //   if so, return 'F'; otherwise return 'V'.

                    // If the next token is a dyadic op, ...
                    if (LookaheadDyadicOp (&plLocalVars, &plLocalVars.lptkNext[-1]))
                        cRes = 'F';     // Function
                    else
                        cRes = 'V';     // Variable
                } else
                    cRes = 'J';         // Jot
            } else
                cRes = '2';             // Dyadic operator

            goto NORMAL_EXIT;

        case TKT_OP2NAMED:
            // If the OP2 is JOT, ...
            if (IsAPLCharJot (plLocalVars.lptkNext->tkData.tkSym->stData.stChar))
            {
                // If the caller wants us to check to the left of the jot, ...
                if (bCheckLeft)
                {
                    // Lookahead to see if the next token is a dyadic operator:
                    //   if so, return 'F'; otherwise return 'V'.

                    // If the next token is a dyadic op, ...
                    if (LookaheadDyadicOp (&plLocalVars, &plLocalVars.lptkNext[-1]))
                        cRes = 'F';     // Function
                    else
                        cRes = 'V';     // Variable
                } else
                    cRes = 'J';         // Jot
            } else
                cRes = '2';             // Dyadic operator

            goto NORMAL_EXIT;

        case TKT_OP3IMMED:
        case TKT_OP3NAMED:
            cRes = '3';                 // Ambiguous operator

            goto NORMAL_EXIT;

        case TKT_FCNIMMED:
        case TKT_FCNARRAY:
        case TKT_FCNNAMED:
        case TKT_FCNAFO:
            cRes = 'F';                 // Function

            goto NORMAL_EXIT;

        case TKT_RIGHTBRACE:
            // Check to see if it's a function or operator
            switch (plLocalVars.lptkNext->tkData.tkDfnType)
            {
                case DFNTYPE_FCN:
                    cRes = 'F';         // Function

                    break;

                case DFNTYPE_OP1:
                    cRes = '1';         // Monadic operator

                    break;

                case DFNTYPE_OP2:
                    cRes = '2';         // Dyadic operator

                    break;

                defstop
                    break;
            } // End SWITCH

            goto NORMAL_EXIT;

        case TKT_VARNAMED:
            // Get the token type of the symbol table name
            cRes = SymbTypeVFO (plLocalVars.lptkNext);

            goto NORMAL_EXIT;

        case TKT_RIGHTBRACKET:
            // Each right grouping symbol has the index of the matching
            //   left grouping symbol in its tkData.tkIndex
            // Get a ptr to the token adjacent to ("- 1") the matching left
            //   grouping symbol.
            plLocalVars.lptkNext = &plLocalVars.lptkStart[plLocalVars.lptkNext->tkData.tkIndex - 1];

            break;                      // Go around again

        case TKT_RIGHTPAREN:
            cRes = LookaheadSurround (&plLocalVars);

            goto NORMAL_EXIT;

        case TKT_LINECONT:              // Line continuation marker
        case TKT_SYS_NS:                // System namespace
        case TKT_SETALPHA:              // Set {alpha}
        case TKT_GLBDFN:                // Placeholder for hGlbDfnHdr
        case TKT_NOP:                   // NOP
        case TKT_AFOGUARD:              // AFO guard
        case TKT_AFORETURN:             // AFO return
            plLocalVars.lptkNext--;     // Ignore these tokens

            break;                      // Go around again

        case TKT_EOS:
        case TKT_EOL:
            cRes = 'E';                 // EOS/EOL

            goto NORMAL_EXIT;

        case TKT_LEFTPAREN:             // To allow (//R)
            if (!bSkipBrackets)
            {
                cRes = '(';             // Left paren

                goto NORMAL_EXIT;
            } // End IF

            cRes = '?';                 // SYNTAX ERROR

            goto NORMAL_EXIT;

        case TKT_ASSIGN:                // To allow f{is}/[1]
            cRes = 'A';                 // Assignment arrow

            goto NORMAL_EXIT;

        case TKT_LISTSEP:
        case TKT_LABELSEP:
        case TKT_COLON:
        case TKT_LEFTBRACKET:
        case TKT_STRAND:
        case TKT_AXISIMMED:
        case TKT_AXISARRAY:
        case TKT_LISTINT:
        case TKT_LISTPAR:
        case TKT_LSTIMMED:
        case TKT_LSTARRAY:
        case TKT_LSTMULT:
        case TKT_LEFTBRACE:
        case TKT_SOS:
        case TKT_CS_ANDIF:              // Control structure:  ANDIF     (Data is Line/Stmt #)
        case TKT_CS_ASSERT:             // ...                 ASSERT
        case TKT_CS_CASE:               // ...                 CASE
        case TKT_CS_CASELIST:           // ...                 CASELIST
        case TKT_CS_CONTINUE:           // ...                 CONTINUE
        case TKT_CS_CONTINUEIF:         // ...                 CONTINUEIF
        case TKT_CS_ELSE:               // ...                 ELSE
        case TKT_CS_ELSEIF:             // ...                 ELSEIF
        case TKT_CS_END:                // ...                 END
        case TKT_CS_ENDFOR:             // ...                 ENDFOR
        case TKT_CS_ENDFORLCL:          // ...                 ENDFORLCL
        case TKT_CS_ENDIF:              // ...                 ENDIF
        case TKT_CS_ENDREPEAT:          // ...                 ENDREPEAT
        case TKT_CS_ENDSELECT:          // ...                 ENDSELECT
        case TKT_CS_ENDWHILE:           // ...                 ENDWHILE
        case TKT_CS_FOR:                // ...                 FOR
        case TKT_CS_FOR2:               // ...                 FOR2
        case TKT_CS_FORLCL:             // ...                 FORLCL
        case TKT_CS_GOTO:               // ...                 GOTO
        case TKT_CS_IF:                 // ...                 IF
        case TKT_CS_IF2:                // ...                 IF2
        case TKT_CS_IN:                 // ...                 IN
        case TKT_CS_LEAVE:              // ...                 LEAVE
        case TKT_CS_LEAVEIF:            // ...                 LEAVEIF
        case TKT_CS_ORIF:               // ...                 ORIF
        case TKT_CS_REPEAT:             // ...                 REPEAT
        case TKT_CS_REPEAT2:            // ...                 REPEAT2
        case TKT_CS_RETURN:             // ...                 RETURN
        case TKT_CS_SELECT:             // ...                 SELECT
        case TKT_CS_SELECT2:            // ...                 SELECT2
        case TKT_CS_UNTIL:              // ...                 UNTIL
        case TKT_CS_WHILE:              // ...                 WHILE
        case TKT_CS_WHILE2:             // ...                 WHILE2
        case TKT_CS_SKIPCASE:           // ...                 Special token
        case TKT_CS_SKIPEND:            // ...                 Special token
        case TKT_CS_NEC:                // ...                 Special token
        case TKT_CS_EOL:                // ...                 Special token
        case TKT_SYNTERR:               // Syntax Error
            cRes = '?';                 // SYNTAX ERROR

            goto NORMAL_EXIT;

        defstop
            cRes = '?';                 // SYNTAX ERROR

            goto NORMAL_EXIT;
    } // End WHILE/SWITCH
NORMAL_EXIT:
#ifdef DEBUG
    dprintfWL9 (L"==Exiting  LookaheadAdjacent:  %c", cRes);
#endif
    return cRes;
} // LookaheadAdjacent
#undef  APPEND_NAME


//***************************************************************************
//  $LookaheadDyadicOp
//
//  The current token is a variable, but it could be part of a function
//    if the next non-grouping symbol is a dyadic operator.
//
//  Lookahead (to the left) in the token stream to see if
//    the next non-right grouping symbol is a dyadic operator or not.
//***************************************************************************

UBOOL LookaheadDyadicOp
    (LPPLLOCALVARS lpplLocalVars,
     LPTOKEN       lptkNext)

{
    UBOOL        bRet;          // The result
    LPSIS_HEADER lpSISCur;      // Ptr to current SIS

    DbgMsgW2 (L"==Entering LookaheadDyadicOp");

    while (TRUE)
    // Split cases based upon the token type
    switch (lptkNext->tkFlags.TknType)
    {
        case TKT_CHRSTRAND:
        case TKT_NUMSTRAND:
        case TKT_NUMSCALAR:
        case TKT_VARIMMED:
        case TKT_LEFTPAREN:
        case TKT_FCNIMMED:
        case TKT_EOS:
        case TKT_EOL:
        case TKT_SOS:
        case TKT_ASSIGN:
        case TKT_LISTSEP:
        case TKT_LISTINT:
        case TKT_LISTPAR:
        case TKT_LSTIMMED:
        case TKT_LSTARRAY:
        case TKT_LSTMULT:
        case TKT_LABELSEP:
        case TKT_COLON:
        case TKT_FCNNAMED:
        case TKT_OP1IMMED:
        case TKT_OP1NAMED:
        case TKT_OP3IMMED:
        case TKT_OP3NAMED:
        case TKT_LEFTBRACKET:
        case TKT_LEFTBRACE:
        case TKT_RIGHTBRACE:
        case TKT_VARARRAY:
        case TKT_INPOUT:
        case TKT_CS_ANDIF:          // Control structure:  ANDIF     (Data is Line/Stmt #)
        case TKT_CS_ASSERT:         // ...                 ASSERT
        case TKT_CS_CASE:           // ...                 CASE
        case TKT_CS_CASELIST:       // ...                 CASELIST
        case TKT_CS_CONTINUE:       // ...                 CONTINUE
        case TKT_CS_CONTINUEIF:     // ...                 CONTINUEIF
        case TKT_CS_ELSE:           // ...                 ELSE
        case TKT_CS_ELSEIF:         // ...                 ELSEIF
        case TKT_CS_END:            // ...                 END
        case TKT_CS_ENDFOR:         // ...                 ENDFOR
        case TKT_CS_ENDFORLCL:      // ...                 ENDFORLCL
        case TKT_CS_ENDIF:          // ...                 ENDIF
        case TKT_CS_ENDREPEAT:      // ...                 ENDREPEAT
        case TKT_CS_ENDSELECT:      // ...                 ENDSELECT
        case TKT_CS_ENDWHILE:       // ...                 ENDWHILE
        case TKT_CS_FOR:            // ...                 FOR
        case TKT_CS_FOR2:           // ...                 FOR2
        case TKT_CS_FORLCL:         // ...                 FORLCL
        case TKT_CS_GOTO:           // ...                 GOTO
        case TKT_CS_IF:             // ...                 IF
        case TKT_CS_IF2:            // ...                 IF2
        case TKT_CS_IN:             // ...                 IN
        case TKT_CS_LEAVE:          // ...                 LEAVE
        case TKT_CS_LEAVEIF:        // ...                 LEAVEIF
        case TKT_CS_ORIF:           // ...                 ORIF
        case TKT_CS_REPEAT:         // ...                 REPEAT
        case TKT_CS_REPEAT2:        // ...                 REPEAT2
        case TKT_CS_RETURN:         // ...                 RETURN
        case TKT_CS_SELECT:         // ...                 SELECT
        case TKT_CS_SELECT2:        // ...                 SELECT2
        case TKT_CS_UNTIL:          // ...                 UNTIL
        case TKT_CS_WHILE:          // ...                 WHILE
        case TKT_CS_WHILE2:         // ...                 WHILE2
        case TKT_CS_SKIPCASE:       // ...                 Special token
        case TKT_CS_SKIPEND:        // ...                 Special token
        case TKT_CS_NEC:            // ...                 Special token
        case TKT_CS_EOL:            // ...                 Special token
        case TKT_SYNTERR:           // Syntax Error
        case TKT_SYS_NS:            // System namespace
        case TKT_FILLJOT:           // Fill jot
        case TKT_DEL:               // Del -- always a function
        case TKT_DELAFO:                // Del Anon -- either a monadic or dyadic operator, bound to its operands
        case TKT_FCNAFO:            // Anonymous function
        case TKT_OP1AFO:            // Anonymous monadic operator
        case TKT_GLBDFN:            // Placeholder for hGlbDfnHdr
        case TKT_NOP:               // NOP
        case TKT_AFOGUARD:          // AFO guard
        case TKT_AFORETURN:         // AFO return
            bRet = FALSE;

            goto NORMAL_EXIT;

        case TKT_DELDEL:            // Del Del  -- either a monadic or dyadic operator
            // Search up the SIS chain to see what this is
            lpSISCur = SrchSISForDfn (lpplLocalVars->lpMemPTD);

            // If the ptr is valid, ...
            if (lpSISCur)
            {
                // Split case based upon the function type
                switch (lpSISCur->DfnType)
                {
////////////////////case DFNTYPE_FCN:
                    case DFNTYPE_OP1:
                        bRet = FALSE;

                        break;

                    case DFNTYPE_OP2:
                        bRet = TRUE;

                        break;

                    default:
                        bRet = FALSE;

                        break;
                } // End SWITCH
            } else
                bRet = FALSE;

            goto NORMAL_EXIT;

        case TKT_OP2IMMED:
            if (IsAPLCharJot (lptkNext->tkData.tkChar))
                bRet = !LookaheadDyadicOp (lpplLocalVars, lptkNext - 1);
            else
                bRet = TRUE;

            goto NORMAL_EXIT;

        case TKT_OP2NAMED:
            if (IsAPLCharJot (lptkNext->tkData.tkSym->stData.stChar))
                bRet = !LookaheadDyadicOp (lpplLocalVars, lptkNext - 1);
            else
                bRet = TRUE;

            goto NORMAL_EXIT;

        case TKT_OP2AFO:
        case TKT_OPJOTDOT:
            bRet = TRUE;

            goto NORMAL_EXIT;

        case TKT_VARNAMED:
            // Look inside the symbol table entry
            bRet = (lptkNext->tkData.tkSym->stFlags.stNameType EQ NAMETYPE_OP2);

            goto NORMAL_EXIT;

        case TKT_RIGHTPAREN:
        case TKT_LINECONT:
        case TKT_SETALPHA:
            lptkNext--;             // Ignore this token

            break;                  // Go around again

        case TKT_RIGHTBRACKET:
            // Each right grouping symbol has the index of the matching
            //   left grouping symbol in its tkData.tkIndex
            // Get a ptr to the token adjacent to ("- 1") the matching left
            //   grouping symbol.
            lptkNext = &lpplLocalVars->lptkStart[lptkNext->tkData.tkIndex - 1];

            break;                  // Go around again

        case TKT_STRAND:
        defstop
            break;
    } // End WHILE/SWITCH
NORMAL_EXIT:
#ifdef DEBUG
    dprintfWL9 (L"==Exiting  LookaheadDyadicOp:  %d", bRet);
#endif
    return bRet;
} // End LookaheadDyadicOp


//***************************************************************************
//  $LookbehindOp
//
//  The current token is a variable, but it could be part of a function
//    if the prev non-grouping symbol is a monadic or dyadic operator.
//
//  Lookbehind (to the right) in the token stream to see if
//    the next non-right grouping symbol is a monadic or dyadic operator or not.
//***************************************************************************

UBOOL LookbehindOp
    (LPPLLOCALVARS lpplLocalVars,
     LPTOKEN       lptkPrev)

{
    UBOOL        bRet;              // The result
    LPSIS_HEADER lpSISCur;          // Ptr to current SIS

    DbgMsgW2 (L"==Entering LookbehindOp");

    while (TRUE)
    // Split cases based upon the token type
    switch (lptkPrev->tkFlags.TknType)
    {
        case TKT_CHRSTRAND:
        case TKT_NUMSTRAND:
        case TKT_NUMSCALAR:
        case TKT_VARIMMED:
        case TKT_LEFTPAREN:
        case TKT_RIGHTPAREN:
        case TKT_FCNIMMED:
        case TKT_OPJOTDOT:
        case TKT_EOS:
        case TKT_EOL:
        case TKT_SOS:
        case TKT_ASSIGN:
        case TKT_LISTSEP:
        case TKT_LISTINT:
        case TKT_LISTPAR:
        case TKT_LSTIMMED:
        case TKT_LSTARRAY:
        case TKT_LSTMULT:
        case TKT_LABELSEP:
        case TKT_COLON:
        case TKT_FCNNAMED:
        case TKT_LEFTBRACKET:
        case TKT_RIGHTBRACKET:
        case TKT_LEFTBRACE:
        case TKT_RIGHTBRACE:
        case TKT_VARARRAY:
        case TKT_INPOUT:
        case TKT_CS_ANDIF:          // Control structure:  ANDIF     (Data is Line/Stmt #)
        case TKT_CS_ASSERT:         // ...                 ASSERT
        case TKT_CS_CASE:           // ...                 CASE
        case TKT_CS_CASELIST:       // ...                 CASELIST
        case TKT_CS_CONTINUE:       // ...                 CONTINUE
        case TKT_CS_CONTINUEIF:     // ...                 CONTINUEIF
        case TKT_CS_ELSE:           // ...                 ELSE
        case TKT_CS_ELSEIF:         // ...                 ELSEIF
        case TKT_CS_END:            // ...                 END
        case TKT_CS_ENDFOR:         // ...                 ENDFOR
        case TKT_CS_ENDFORLCL:      // ...                 ENDFORLCL
        case TKT_CS_ENDIF:          // ...                 ENDIF
        case TKT_CS_ENDREPEAT:      // ...                 ENDREPEAT
        case TKT_CS_ENDSELECT:      // ...                 ENDSELECT
        case TKT_CS_ENDWHILE:       // ...                 ENDWHILE
        case TKT_CS_FOR:            // ...                 FOR
        case TKT_CS_FOR2:           // ...                 FOR2
        case TKT_CS_FORLCL:         // ...                 FORLCL
        case TKT_CS_GOTO:           // ...                 GOTO
        case TKT_CS_IF:             // ...                 IF
        case TKT_CS_IF2:            // ...                 IF2
        case TKT_CS_IN:             // ...                 IN
        case TKT_CS_LEAVE:          // ...                 LEAVE
        case TKT_CS_LEAVEIF:        // ...                 LEAVEIF
        case TKT_CS_ORIF:           // ...                 ORIF
        case TKT_CS_REPEAT:         // ...                 REPEAT
        case TKT_CS_REPEAT2:        // ...                 REPEAT2
        case TKT_CS_RETURN:         // ...                 RETURN
        case TKT_CS_SELECT:         // ...                 SELECT
        case TKT_CS_SELECT2:        // ...                 SELECT2
        case TKT_CS_UNTIL:          // ...                 UNTIL
        case TKT_CS_WHILE:          // ...                 WHILE
        case TKT_CS_WHILE2:         // ...                 WHILE2
        case TKT_CS_SKIPCASE:       // ...                 Special token
        case TKT_CS_SKIPEND:        // ...                 Special token
        case TKT_CS_NEC:            // ...                 Special token
        case TKT_CS_EOL:            // ...                 Special token
        case TKT_SYNTERR:           // Syntax Error
        case TKT_SYS_NS:            // System namespace
        case TKT_FILLJOT:           // Fill jot
        case TKT_FCNAFO:            // Anonymous function
        case TKT_GLBDFN:            // Placeholder for hGlbDfnHdr
        case TKT_NOP:               // NOP
        case TKT_AFOGUARD:          // AFO guard
        case TKT_AFORETURN:         // AFO return
            bRet = FALSE;

            goto NORMAL_EXIT;

        case TKT_DEL:               // Del -- always a function
        case TKT_DELAFO:            // Del Anon -- either a monadic or dyadic operator, bound to its operands
////////////// Search up the SIS chain to see what this is
////////////lpSISCur = SrchSISForDfn (lpplLocalVars->lpMemPTD);
////////////
////////////// If the ptr is valid, ...
////////////if (lpSISCur)
                bRet = FALSE;
////////////else
////////////    bRet = FALSE;

            goto NORMAL_EXIT;

        case TKT_DELDEL:            // Del Del  -- either a monadic or dyadic operator
            // Search up the SIS chain to see what this is
            lpSISCur = SrchSISForDfn (lpplLocalVars->lpMemPTD);

            // If the ptr is valid, ...
            if (lpSISCur)
            {
                // Split case based upon the function type
                switch (lpSISCur->DfnType)
                {
////////////////////case DFNTYPE_FCN:
////////////////////    bRet = FALSE;
////////////////////
////////////////////    break;

                    case DFNTYPE_OP1:
                    case DFNTYPE_OP2:
                        bRet = TRUE;

                        break;

                    defstop
                        break;
                } // End SWITCH
            } else
                bRet = FALSE;

            goto NORMAL_EXIT;

        case TKT_OP1IMMED:
        case TKT_OP1NAMED:
        case TKT_OP1AFO:            // Anonymous monadic operator
        case TKT_OP2IMMED:
        case TKT_OP2NAMED:
        case TKT_OP2AFO:            // Anonymous dyadic operator
            bRet = TRUE;

            goto NORMAL_EXIT;

        case TKT_OP3IMMED:
        case TKT_OP3NAMED:
            bRet = FALSE;

            goto NORMAL_EXIT;

        case TKT_VARNAMED:
            // Look inside the symbol table entry
            bRet = (lptkPrev->tkData.tkSym->stFlags.stNameType EQ NAMETYPE_OP1)
                || (lptkPrev->tkData.tkSym->stFlags.stNameType EQ NAMETYPE_OP2);

            goto NORMAL_EXIT;

        case TKT_LINECONT:
        case TKT_SETALPHA:
            lptkPrev++;             // Ignore these tokens

            break;                  // Go around again

        case TKT_STRAND:
        defstop
            break;
    } // End WHILE/SWITCH
NORMAL_EXIT:
#ifdef DEBUG
    dprintfWL9 (L"==Exiting  LookbehindOp:  %d", bRet);
#endif
    return bRet;
} // End LookbehindOp


//***************************************************************************
//  $CheckNullOp3
//
//  Check for NULLOP followed by an ambiguous operator possible with axis
//***************************************************************************

UBOOL CheckNullOp3
    (LPPL_YYSTYPE  lpYYLval,        // Ptr to lval
     LPPLLOCALVARS lpplLocalVars)   // Ptr to local plLocalVars

{
    LPTOKEN lptkNext;               // Ptr to next token to check

    // Split cases based upon the current token type
    switch (lpYYLval->tkToken.tkFlags.TknType)
    {
        case TKT_OP1IMMED:
            // First of all, the current token must be NULLOP
            if (lpYYLval->tkToken.tkData.tkChar EQ UTF16_CIRCLEMIDDLEDOT)
                break;
            return FALSE;

        case TKT_OP1NAMED:
            // First of all, the current token must be NULLOP
            if (lpYYLval->tkToken.tkData.tkSym->stFlags.Imm
             && lpYYLval->tkToken.tkData.tkSym->stData.stChar EQ UTF16_CIRCLEMIDDLEDOT)
                break;
            return FALSE;

        defstop
            break;
    } // End SWITCH

    // Then, either the next token is OP3, ...
    if (lpplLocalVars->lptkNext[-1].tkFlags.TknType EQ TKT_OP3IMMED)
        lptkNext = &lpplLocalVars->lptkNext[-1];        // Point to the OP3IMMED token
    else
    // or the next four tokens are /[I] or /[A]
    if (              lpplLocalVars->lptkNext[-1].tkFlags.TknType EQ TKT_RIGHTBRACKET
     && IsTknTypeVar (lpplLocalVars->lptkNext[-2].tkFlags.TknType)
     && lpplLocalVars->lptkNext[-3].tkFlags.TknType EQ TKT_LEFTBRACKET
     && lpplLocalVars->lptkNext[-4].tkFlags.TknType EQ TKT_OP3IMMED)
        lptkNext = &lpplLocalVars->lptkNext[-4];        // Point to the OP3IMMED token
    else
        return FALSE;

    // Change the "next" token from ambiguous operator to a function
    lpYYLval->tkToken.tkFlags.TknType =
    lptkNext[0]      .tkFlags.TknType = TKT_FCNIMMED;
    lpYYLval->tkToken.tkFlags.ImmType =
    lptkNext[0]      .tkFlags.ImmType = IMMTYPE_PRIMFCN;

    return TRUE;
} // End CheckNullOp3


//***************************************************************************
//  $WaitForInput
//
//  Wait for either Quad or Quote-quad input
//***************************************************************************

LPPL_YYSTYPE WaitForInput
    (HWND    hWndSM,                // Window handle to Session Manager
     UBOOL   bQuoteQuad,            // TRUE iff Quote-Quad input (FALSE if Quad input)
     LPTOKEN lptkFunc)              // Ptr to function token

{
    HANDLE         hSemaphore;      // Semaphore handle
    LPPL_YYSTYPE   lpYYRes;         // Ptr to the result
    LPPERTABDATA   lpMemPTD;        // Ptr to PerTabData global memory
    UINT           uLinePos,        // Char position of start of line
                   uCharPos;        // Current char position
    HWND           hWndEC;          // Edit Ctrl window handle

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Create a semaphore
    hSemaphore =
      MyCreateSemaphoreW (NULL,         // No security attrs
                          0,            // Initial count (non-signalled)
                          64*1024,      // Maximum count
                          NULL);        // No name
    // Fill in the SIS header for Quote-Quad/Quad Input Mode
    FillSISNxt (lpMemPTD,               // Ptr to PerTabData global memory
                hSemaphore,             // Semaphore handle
                bQuoteQuad ? DFNTYPE_QQUAD: DFNTYPE_QUAD, // DfnType
                FCNVALENCE_NIL,         // FcnValence
                TRUE,                   // Suspended
                TRUE,                   // Restartable
                TRUE);                  // LinkIntoChain
    // Save a ptr to the function token
    lpMemPTD->lpSISCur->lptkFunc = lptkFunc;

    // Get the Edit Ctrl window handle
    (HANDLE_PTR) hWndEC = GetWindowLongPtrW (hWndSM, GWLSF_HWNDEC);

    // Get the char position of the caret
    uCharPos = GetCurCharPos (hWndEC);

    // Get the position of the start of the line
    uLinePos = (UINT) SendMessageW (hWndEC, EM_LINEINDEX, NEG1U, 0);

    // Save the Quote-Quad input prompt length
    lpMemPTD->lpSISCur->QQPromptLen = uCharPos - uLinePos;

    // Tell the Session Manager to display the appropriate prompt
    PostMessageW (hWndSM, MYWM_QUOTEQUAD, bQuoteQuad, 14);
#ifdef DEBUG
    dprintfWL9 (L"~~WaitForSingleObject (ENTRY):  %s (%S#%d)", L"WaitForInput", FNLN);
#endif
    // Wait for the semaphore to trigger
    WaitForSingleObject (hSemaphore,    // Handle to wait for
                         INFINITE);     // Timeout value in milliseconds
#ifdef DEBUG
    dprintfWL9 (L"~~WaitForSingleObject (EXIT):   %s (%S#%d)", L"WaitForInput", FNLN);
#endif
    // Close the semaphore handle as it is no longer needed
    MyCloseSemaphore (hSemaphore); hSemaphore = NULL;

    // Unlocalize the STEs on the innermost level
    //   and strip off one level
    UnlocalizeSTEs ();

    // If we're resetting, ...
    if (lpMemPTD->lpSISCur
     && lpMemPTD->lpSISCur->ResetFlag NE RESETFLAG_NONE)
        lpYYRes = NULL;
    else
    // If there's no result from the expression, ...
    if (lpMemPTD->YYResExec.tkToken.tkFlags.TknType EQ 0)
        // Make a PL_YYSTYPE NoValue entry
        lpYYRes = MakeNoValue_YY (lptkFunc);
    else
    {
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Copy the result
        *lpYYRes = lpMemPTD->YYResExec;
    } // End IF/ELSE

    // We no longer need these values
    ZeroMemory (&lpMemPTD->YYResExec, sizeof (lpMemPTD->YYResExec));

    return lpYYRes;
} // End WaitForInput


//***************************************************************************
//  $AmbOpSwap_EM
//
// Change the first token from ambiguous operator to a function
//   and swap the first two tokens
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- AmbOpSwap_EM"
#else
#define APPEND_NAME
#endif

UBOOL AmbOpSwap_EM
    (LPPL_YYSTYPE lpYYFcn)              // Ptr to function strand

{
    DbgMsgW2 (L"==Entering AmbOpSwap");

    // Change the first token in the function strand
    //   from ambiguous operator to a function
    AmbOpToFcn (lpYYFcn);

    // Split cases based upon the token count
    switch (YYCountFcnStr (lpYYFcn->lpYYFcnBase))
    {
        case 1:                             // Single token already converted to a function
            break;

        case 2:
            // If there's an axis operator in the function strand, ...
            if (YYIsFcnStrAxis (lpYYFcn->lpYYFcnBase))
                break;                      //   we're done

            // Fall through to multiple element case

        case 3:
            // ***FIXME*** -- Multiple element function strand -- modify internal RPN structure

            ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                                      &lpYYFcn->tkToken);
            return FALSE;

        defstop
            break;
    } // End SWITCH

    return TRUE;
} // End AmbOpSwap_EM
#undef  APPEND_NAME


//***************************************************************************
//  $AmbOpToFcn
//
// Change the first token in the function strand
//   from ambiguous operator to a function
//***************************************************************************

void AmbOpToFcn
    (LPPL_YYSTYPE lpYYFcn)              // Ptr to function strand

{
    HGLOBAL      hGlbFcn;               // Function array global memory handle
    LPPL_YYSTYPE lpMemFcn;              // Ptr to function array global memory

    DbgMsgW2 (L"==Entering AmbOpToFcn");

    // Split cases based upon token type
    switch (lpYYFcn->tkToken.tkFlags.TknType)
    {
        case TKT_OP3IMMED:
            // Change the first token from ambiguous operator to an immediate function
            lpYYFcn->tkToken.tkFlags.TknType = TKT_FCNIMMED;
            lpYYFcn->tkToken.tkFlags.ImmType = IMMTYPE_PRIMFCN;
            if (lpYYFcn->lpYYFcnBase)
            {
                lpYYFcn->lpYYFcnBase->tkToken.tkFlags.TknType = TKT_FCNIMMED;
                lpYYFcn->lpYYFcnBase->tkToken.tkFlags.ImmType = IMMTYPE_PRIMFCN;
            } // End IF

            break;

        case TKT_OP3NAMED:
            // Change the first token from ambiguous operator to a named function
            lpYYFcn->tkToken.tkFlags.TknType = TKT_FCNNAMED;
            if (lpYYFcn->lpYYFcnBase)
                lpYYFcn->lpYYFcnBase->tkToken.tkFlags.TknType = TKT_FCNNAMED;

            // If the STE is an immediate, ...
            if (lpYYFcn->tkToken.tkData.tkSym->stFlags.Imm)
            {
                // Change the STE from ambiguous operator to an immediate function
                lpYYFcn->tkToken.tkData.tkSym->stFlags.ImmType = IMMTYPE_PRIMFCN;
                if (lpYYFcn->lpYYFcnBase)
                    lpYYFcn->lpYYFcnBase->tkToken.tkData.tkSym->stFlags.ImmType = IMMTYPE_PRIMFCN;
            } // End IF

            break;

        case TKT_FCNNAMED:
            // If it's a global, ...
            if (!lpYYFcn->tkToken.tkData.tkSym->stFlags.Imm)
            {
                DbgBrk ();          // ***FIXME*** -- Can this ever occur?






            } // End IF

            break;

        case TKT_FCNARRAY:
            // Get the global
            hGlbFcn = lpYYFcn->tkToken.tkData.tkGlbData;

            // Lock the memory to get a ptr to it
            lpMemFcn = MyGlobalLock (hGlbFcn);

            // Skip over the function header to the data
            lpMemFcn = FcnArrayBaseToData (lpMemFcn);

            // If it's an immediate ambiguous operator, ...
            if (lpMemFcn->tkToken.tkFlags.TknType EQ TKT_OP3IMMED)
                // Change it to be an immediate function
                lpMemFcn->tkToken.tkFlags.TknType = TKT_FCNIMMED;

            // We no longer need this ptr
            MyGlobalUnlock (hGlbFcn); lpMemFcn = NULL;

            break;

        case TKT_FCNIMMED:
            break;

        defstop
            break;
    } // End SWITCH
} // End AmbOpToFcn


//***************************************************************************
//  $AmbOpToOp1
//
// Change the first token in the function strand
//   from ambiguous operator to a monadic operator
//***************************************************************************

void AmbOpToOp1
    (LPPL_YYSTYPE lpYYFcn)              // Ptr to function strand

{
    Assert (lpYYFcn->tkToken.tkFlags.TknType EQ TKT_OP3IMMED
         || lpYYFcn->tkToken.tkFlags.TknType EQ TKT_OP3NAMED);

    if (lpYYFcn->tkToken.tkFlags.TknType EQ TKT_OP3IMMED)
    {
        // Change the first token from ambiguous operator to an immediate monadic operator
        lpYYFcn->tkToken.tkFlags.TknType = TKT_OP1IMMED;
        lpYYFcn->tkToken.tkFlags.ImmType = IMMTYPE_PRIMOP1;
        if (lpYYFcn->lpYYFcnBase)
        {
            lpYYFcn->lpYYFcnBase->tkToken.tkFlags.TknType = TKT_OP1IMMED;
            lpYYFcn->lpYYFcnBase->tkToken.tkFlags.ImmType = IMMTYPE_PRIMOP1;
        } // End IF
    } else
    {
        // Change the first token from ambiguous operator to a named monadic operator
        lpYYFcn->tkToken.tkFlags.TknType = TKT_OP1NAMED;
        if (lpYYFcn->lpYYFcnBase)
            lpYYFcn->lpYYFcnBase->tkToken.tkFlags.TknType = TKT_OP1NAMED;

        // If the STE is an immediate, ...
        if (lpYYFcn->tkToken.tkData.tkSym->stFlags.Imm)
        {
            // Change the STE from ambiguous operator to an immediate monadic operator
            lpYYFcn->tkToken.tkData.tkSym->stFlags.ImmType = IMMTYPE_PRIMOP1;
            if (lpYYFcn->lpYYFcnBase)
                lpYYFcn->lpYYFcnBase->tkToken.tkData.tkSym->stFlags.ImmType = IMMTYPE_PRIMOP1;
        } // End IF
    } // End IF/ELSE
} // End AmbOpToOp1


//***************************************************************************
//  $ArrExprCheckCaller
//
//  Tuck away the result from Execute/Quad.
//***************************************************************************

void ArrExprCheckCaller
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to local vars
     LPSIS_HEADER  lpSISCur,            // Ptr to current SIS header
     LPPL_YYSTYPE  lpYYArg,             // Ptr to ArrExpr
     UBOOL         bNoDisplay)          // TRUE iff value is not to be displayed

{
    LPPERTABDATA  lpMemPTD;             // Ptr to PerTabData global memory

    // Mark as not in error
    lpplLocalVars->bRet = TRUE;

    // Get ptr to PerTabData global memory
    lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // If it's NoValue, ...
    if (IsTokenNoValue (&lpYYArg->tkToken))
        // Copy the result
        lpplLocalVars->lpYYRes = lpYYArg;
    else
        // Copy the result
        lpplLocalVars->lpYYRes = CopyPL_YYSTYPE_EM_YY (lpYYArg, FALSE);

    // Set/clear the NoDisplay flag
    lpplLocalVars->lpYYRes->tkToken.tkFlags.NoDisplay = bNoDisplay;

    // If the Execute/Quad result is present,
    //   and the token is not named or it has a value, ...
    if (lpMemPTD->YYResExec.tkToken.tkFlags.TknType
     && (!IsTknNamed (&lpMemPTD->YYResExec.tkToken)
      || !IsSymNoValue (lpMemPTD->YYResExec.tkToken.tkData.tkSym)))
    {
        HGLOBAL hGlbDfnHdr;

        // If we're parsing an AFO, ...
        if (hGlbDfnHdr = SISAfo (lpMemPTD))
            lpplLocalVars->bStopExec =
            lpplLocalVars->bRet =
              AfoDisplay_EM (&lpMemPTD->YYResExec.tkToken, bNoDisplay, lpplLocalVars, hGlbDfnHdr);
        else
            lpplLocalVars->bRet =
              ArrayDisplay_EM (&lpMemPTD->YYResExec.tkToken, TRUE, &lpplLocalVars->bCtrlBreak);
    } // End IF

    // Save the Execute/Quad result
    //   unless the current line starts with a "sink"
    //   or there's a error control parent active
    if (lpplLocalVars->lptkEOS EQ NULL
     || lpplLocalVars->lptkEOS[1].tkFlags.TknType NE TKT_ASSIGN
     || lpMemPTD->lpSISCur->lpSISErrCtrl NE NULL)
        lpMemPTD->YYResExec = *lpplLocalVars->lpYYRes;

    // If it's not NoValue, ...
    if (!IsTokenNoValue (&lpYYArg->tkToken))
    {
        // Free the YYRes (but not the storage)
        YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
    } // End IF
} // End ArrExprCheckCaller


//***************************************************************************
//  $CheckSelSpec_EM
//
//  Check on Selective Specification of the first (and only) name
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CheckSelSpec_EM"
#else
#define APPEND_NAME
#endif

UBOOL CheckSelSpec_EM
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to local vars
     LPTOKEN       lptkNameVars)        // Ptr to NameVars token

{
    HGLOBAL hGlbNameVars;               // NameVars global memory handle
    LPTOKEN lpMemNameVars;              // Ptr to NameVars global memory
    UBOOL   bRet;                       // TRUE iff the result is valid
    UBOOL   bSelSpec = FALSE;           // TRUE iff Selective Specification is in error ***DEBUG***
    UINT    uCharIndex;                 // Char index

    // If it's Selective Specification, and
    //   we have yet to see the first name, ...
    if (lpplLocalVars->bSelSpec && lpplLocalVars->bIniSpec)
    {
        // Clear the flag
        lpplLocalVars->bIniSpec = FALSE;

        // Get the global memory handle
        hGlbNameVars = lptkNameVars->tkData.tkGlbData;

        Assert (IsGlbTypeNamDir_PTB (hGlbNameVars));

        // Get the global memory handle
        hGlbNameVars = lptkNameVars->tkData.tkGlbData;

        // Lock the memory to get a ptr to it
        lpMemNameVars = MyGlobalLock (hGlbNameVars);

        // Ensure there is only one name
        bRet = (((LPVARNAMED_HEADER) lpMemNameVars)->NELM EQ 1);
        if (bRet)
        {
            LPPL_YYSTYPE lpYYRes,       // Ptr to result
                         lpYYRes2;      // ...

            // Ensure the SelectSpec is properly marked
            bRet = IsSelectSpec (lptkNameVars);
            bSelSpec = !bRet;
            if (bRet)
            {
                // Skip over the header to the data
                lpMemNameVars = VarNamedBaseToData (lpMemNameVars);

                // Save the token
                lpplLocalVars->tkSelSpec = *lpMemNameVars;

                // Change the one (and only) token to an APA
                //   which maps the indices of the array
                lpYYRes2 =
                  PrimFnMonRho_EM_YY (&lpplLocalVars->tkSelSpec,
                                      &lpplLocalVars->tkSelSpec,
                                       NULL);
                if (lpYYRes2)
                {
                    lpYYRes =
                      PrimFnMonIotaVector_EM_YY (&lpplLocalVars->tkSelSpec, // Ptr to function token
                                                 &lpYYRes2->tkToken,        // Ptr to right arg token
                                                  NULL);                    // Ptr to axis token (may be NULL)
                    FreeYYFcn1 (lpYYRes2); lpYYRes2 = NULL;

                    if (lpYYRes)
                    {
                        LPVARARRAY_HEADER lpMemData;

                        // Save the char index
                        uCharIndex = lptkNameVars->tkCharIndex;

                        // Lock the memory to get a ptr to it
                        lpMemData = MyGlobalLock (lpYYRes->tkToken.tkData.tkGlbData);

                        // Mark as a Selective Specification array
                        lpMemData->bSelSpec = TRUE;

                        // We no longer need this ptr
                        MyGlobalUnlock (lpYYRes->tkToken.tkData.tkGlbData); lpMemData = NULL;

                        // Fill in the result token
                        ZeroMemory (lptkNameVars, sizeof (lptkNameVars[0]));
                        lptkNameVars->tkFlags.TknType   = TKT_VARARRAY;
////////////////////////lptkNameVars->tkFlags.ImmType   = IMMTYPE_ERROR;    // Already zero from ZeroMemory
////////////////////////lptkNameVars->tkFlags.NoDisplay = FALSE;            // Already zero from ZeroMemory
                        lptkNameVars->tkData.tkGlbData  = lpYYRes->tkToken.tkData.tkGlbData;
                        lptkNameVars->tkCharIndex       = uCharIndex;

                        YYFree (lpYYRes); lpYYRes = NULL;
                    } else
                        // Mark as unsuccessful
                        bRet = FALSE;
                } else
                    // Mark as unsuccessful
                    bRet = FALSE;
            } // End IF
        } // End IF

        if (!bRet)
        {
            if (!(CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR))
                ErrorMessageIndirectToken (bSelSpec ? L"SelSpec ERROR" APPEND_NAME          // ***DEBUG***
                                                    : ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                           lptkNameVars);
            lpplLocalVars->ExitType = EXITTYPE_ERROR;
        } // End IF

        // We no longer need this ptr
        MyGlobalUnlock (hGlbNameVars); lpMemNameVars = NULL;

        return bRet;
    } else
        return TRUE;

    return FALSE;
} // End CheckSelSpec_EM
#undef  APPEND_NAME


//***************************************************************************
//  $IsSelectSpec
//
//  Determine whether or not the arg is a Select Specification array
//***************************************************************************

UBOOL IsSelectSpec
    (LPTOKEN lptkArg)               // Ptr to arg token

{
    UBOOL bRet = FALSE;             // The result

    // If the arg is not immediate, ...
    if (lptkArg->tkFlags.TknType EQ TKT_VARARRAY)
    {
        HGLOBAL           hGlbArg;
        LPVARARRAY_HEADER lpMemHdr;

        // Get the global memory handle
        hGlbArg = lptkArg->tkData.tkGlbData;

        // Lock the memory to get a ptr to it
        lpMemHdr = MyGlobalLock (hGlbArg);

        // Get the Selective Specification array bit
        bRet = lpMemHdr->bSelSpec;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbArg); lpMemHdr = NULL;
    } // End IF

    return bRet;
} // End IsSelectSpec


//***************************************************************************
//  $MakeTempCopy
//
//  Make a temporary copy of a named var
//***************************************************************************

PL_YYSTYPE MakeTempCopy
    (LPPL_YYSTYPE lpYYTmp)              // Ptr to incoming var

{
    static PL_YYSTYPE YYTmp;

    // If the token is not a named var, pass it on as the result
    if (lpYYTmp->tkToken.tkFlags.TknType NE TKT_VARNAMED)
        return *lpYYTmp;

    // Copy the non-TOKEN fields
    YYTmp = *lpYYTmp;

    // If the named var is immediate, ...
    if (lpYYTmp->tkToken.tkData.tkSym->stFlags.Imm)
    {
        // Fill in the result token
        YYTmp.tkToken.tkFlags.TknType   = TKT_VARIMMED;
        YYTmp.tkToken.tkFlags.ImmType   = lpYYTmp->tkToken.tkData.tkSym->stFlags.ImmType;
        YYTmp.tkToken.tkFlags.NoDisplay = FALSE;
        YYTmp.tkToken.tkData.tkLongest  = lpYYTmp->tkToken.tkData.tkSym->stData.stLongest;
////////YYTmp.tkToken.tkCharIndex       =                   // Set by above YYTmp =
    } else
    {
        // Fill in the result token
        YYTmp.tkToken.tkFlags.TknType   = TKT_VARARRAY;
        YYTmp.tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
        YYTmp.tkToken.tkFlags.NoDisplay = FALSE;
        YYTmp.tkToken.tkData.tkGlbData  = lpYYTmp->tkToken.tkData.tkSym->stData.stGlbData;
////////YYTmp.tkToken.tkCharIndex       =                   // Set by above YYTmp =
    } // End IF/ELSE

    return YYTmp;
} // End MakeTempCopy


//***************************************************************************
//  $GetEOSIndex
//
//  Return the index of the EOS stmt for a char index
//***************************************************************************

UINT GetEOSIndex
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to local vars
     int           tkCharIndex)         // Character index of array to display/free

{
    UINT uCnt,                          // Loop counter
         uLen;                          // # tokens in the stmt

    // Loop through the tokens looking for this stmt's char index
    for (uCnt = 0; uCnt < lpplLocalVars->uTokenCnt; uCnt += uLen)
    {
        Assert (lpplLocalVars->lptkStart[uCnt].tkFlags.TknType EQ TKT_EOL
             || lpplLocalVars->lptkStart[uCnt].tkFlags.TknType EQ TKT_EOS);

        // Get the # tokens in this stmt
        uLen = lpplLocalVars->lptkStart[uCnt].tkData.tkIndex;

        Assert (lpplLocalVars->lptkStart[uCnt + uLen - 1].tkFlags.TknType EQ TKT_SOS);

        if (lpplLocalVars->lptkStart[uCnt].tkCharIndex <= tkCharIndex
         &&                                               tkCharIndex < lpplLocalVars->lptkStart[uCnt + uLen - 1].tkCharIndex)
            break;
    } // End FOR

    // If we're still within the line, ...
    if (uCnt < lpplLocalVars->uTokenCnt)
        return uCnt;
    else
        return NEG1U;
} // End GetEOSIndex


//***************************************************************************
//  $IsLastStmt
//
//  Return TRUE iff the current stmt is the last one on the line
//  We need this to determine whether or not to print or return a result
//    from within an Execute or Quad stmt.
//***************************************************************************

UBOOL IsLastStmt
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to local vars
     int           tkCharIndex)         // Character index of array to display/free

{
    UINT uIndex;                        // Index of the corresponding EOS stmt

    // Get the index of the EOS stmt
    uIndex = GetEOSIndex (lpplLocalVars, tkCharIndex);

    // If we're still within the line, ...
    if (uIndex NE NEG1U)
        return (lpplLocalVars->uTokenCnt EQ (uIndex + lpplLocalVars->lptkStart[uIndex].tkData.tkIndex));
    else
        return TRUE;
} // End IsLastStmt


//***************************************************************************
//  $SrchSISForDfn
//
//  Search up the SIS chain looking for a UDFO
//***************************************************************************

LPSIS_HEADER SrchSISForDfn
    (LPPERTABDATA lpMemPTD)         // Ptr to PerTabData global memory

{
    LPSIS_HEADER lpSISCur;          // Ptr to current SIS layer

    // Search up the SIS chain to see what this is
    for (lpSISCur = lpMemPTD->lpSISCur;
         lpSISCur;
         lpSISCur = lpSISCur->lpSISPrv)
    // Split case based upon the function type
    switch (lpSISCur->DfnType)
    {
        case DFNTYPE_FCN:
        case DFNTYPE_OP1:
        case DFNTYPE_OP2:
            return lpSISCur;

        case DFNTYPE_IMM:
        case DFNTYPE_EXEC:
        case DFNTYPE_QUAD:
        case DFNTYPE_QQUAD:
        case DFNTYPE_ERRCTRL:
            break;

        defstop
            break;
    } // End FOR/SWITCH

    return NULL;
} // End SrchSISForDfn


//***************************************************************************
//  End of File: pl_proc.c
//***************************************************************************
