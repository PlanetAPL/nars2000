//***************************************************************************
//  NARS2000 -- Parser Grammar for executable lines
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

/****************************************************************************

Parse a line of pre-tokenized symbols.

Based upon "The Syntax of APL:  An Old Approach Revisited" by
Jean Jacques Giardot & Florence Rollin, ACM SIGAPL Quote-Quad APL 1987
heavily modified to work as an LALR grammar with the lookahead embedded
in the lexical analyser (pl_yylex).

****************************************************************************/

%{
#define STRICT
#include <windows.h>
#include "headers.h"


#define   DIRECT    FALSE           // Flags for PushFcnStrand_YY
#define INDIRECT    TRUE            // ...

////#define YYLEX_DEBUG
////#define YYFPRINTF_DEBUG

#ifdef DEBUG
#define YYERROR_VERBOSE
#define YYDEBUG     1
#define YYFPRINTF   pl_yyfprintf
#define fprintf     pl_yyfprintf
////#define YYPRINT                 pl_yyprint
void pl_yyprint     (FILE *yyoutput, unsigned short int yytoknum, PL_YYSTYPE const yyvaluep);
#endif

#define YYMALLOC    malloc
#define YYFREE      free

#include "pl_parse.proto"

#define    YYSTYPE     PL_YYSTYPE
#define  LPYYSTYPE   LPPL_YYSTYPE
#define tagYYSTYPE  tagPL_YYSTYPE

// The following #defines are needed to allow multiple parsers
//   to coexist in the same file
#define yy_symbol_print         pl_yy_symbol_print
#define yy_symbol_value_print   pl_yy_symbol_value_print
#define yy_reduce_print         pl_yy_reduce_print
#define yydestruct              pl_yydestruct

#define YYERROR2        {lpplLocalVars->bYYERROR = TRUE; YYERROR;}
#define YYERROR3        {if (lpplLocalVars->ExitType NE EXITTYPE_STACK_FULL) lpplLocalVars->ExitType = EXITTYPE_ERROR; YYERROR2;}

////#define DbgMsgWP(a)         DbgMsgW2 (lpplLocalVars->bLookAhead ? L"==" a : a)
////#define DbgMsgWP(a)         DbgMsgW  (lpplLocalVars->bLookAhead ? L"==" a : a); DbgBrk ()
////#define DbgMsgWP(a)         DbgMsgW  (lpplLocalVars->bLookAhead ? L"==" a : a)
    #define DbgMsgWP(a)

#ifdef DEBUG
#define APPEND_NAME     L" -- pl_yyparse"
#else
#define APPEND_NAME
#endif

%}

%pure-parser
%name-prefix="pl_yy"
%parse-param {LPPLLOCALVARS lpplLocalVars}
%lex-param   {LPPLLOCALVARS lpplLocalVars}

%token NAMEVAR NAMEUNK CONSTANT CHRSTRAND NUMSTRAND USRFN0 SYSFN0 QUAD QUOTEQUAD SYSLBL
%token LBRACE RBRACE_FCN_FN0 RBRACE_FCN_FN12 RBRACE_OP1_FN12 RBRACE_OP2_FN12
%token UNK EOL
%token CS_ANDIF
%token CS_ASSERT
%token CS_CASE
%token CS_CASELIST
%token CS_CONTINUE
%token CS_CONTINUEIF
%token CS_ELSE
%token CS_ELSEIF
%token CS_ENDFOR
%token CS_ENDFORLCL
%token CS_ENDREPEAT
%token CS_ENDWHILE
%token CS_FOR
%token CS_FORLCL
%token CS_IF
%token CS_IN
%token CS_LEAVE
%token CS_LEAVEIF
%token CS_ORIF
%token CS_SELECT
%token CS_SKIPCASE
%token CS_SKIPEND
%token CS_UNTIL
%token CS_WHILE
%token FILLJOT

/*  Note that as we parse APL from right to left, these rules
    are all reversed as well as token associativity (long scope).
    That is, functions in APL have long right scope, so they are
    declared here in the %left list.  Similarly, operators in APL
    have long left scope, so they are declared in the %right list.
 */
%right DIAMOND
%left  ASSIGN PRIMFCN NAMEFCN NAMETRN SYSFN12 GOTO SYSNS AFOFCN AFOOP1 AFOOP2 AFOGUARD AFORETURN
%right NAMEOP1 OP1 NAMEOP2 OP2 NAMEOP3 OP3 JOTDOT OP3ASSIGN NAMEOP3ASSIGN
%right OP2CONSTANT OP2NAMEVAR OP2CHRSTRAND OP2NUMSTRAND

%start StmtMult

%%

/*  ToDo
    ----
    * Allow f{is}MonOp MonOp
    * ...   f{is}      DydOp FcnOrVar

    Notes
    -----
    * A monadic operator with a var left arg must be
      enclosed in parens, such as (2 op1) because
      1 2 op1 can be interpreted as 1 (2 op1) as well
      as (1 2) op1.
    * A dyadic operator with either argument as a var
      must be enclosed in parens for the same reasons
      as above, otherwise 1 2 op2 3 4 is way too ambiguous.
 */

// One or more statements
StmtMult:
      // All single stmt errors propagate up to this point where we ABORT -- this ensures
      //   that the call to pl_yyparse terminates wth a non-zero error code.
                       error            {DbgMsgWP (L"%%StmtMult:  error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

                                             Assert (YYResIsEmpty ());

                                             // Get ptr to PerTabData global memory
                                             lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

                                             // If we're resetting via {goto}, ...
                                             if (lpMemPTD->lpSISCur->ResetFlag EQ RESETFLAG_ONE
                                              || lpMemPTD->lpSISCur->ResetFlag EQ RESETFLAG_ONE_INIT)
                                             {
                                                // Mark as not resetting
                                                 lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_NONE;

                                                 // Mark as a SYNTAX ERROR
                                                 if (!lpplLocalVars->bYYERROR)
                                                     ErrorMessageIndirect (ERRMSG_SYNTAX_ERROR);
                                             } // End IF

                                             // If we're resetting, ...
                                             if (lpMemPTD->lpSISCur->ResetFlag NE RESETFLAG_NONE)
                                                 lpplLocalVars->ExitType = TranslateResetFlagToExitType (lpMemPTD->lpSISCur->ResetFlag);

                                             // Set the exit type to error unless already set
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_NONE)
                                                 lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYABORT;
                                         } else
                                             YYABORT;
                                        }
    |                  StmtSing         {DbgMsgWP (L"%%StmtMult:  StmtSing");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             Assert (YYResIsEmpty ());
                                         } // End IF
                                        }
    | StmtMult DIAMOND StmtSing         {DbgMsgWP (L"%%StmtMult:  StmtSing " WS_UTF16_DIAMOND L" StmtMult");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             Assert (YYResIsEmpty ());

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 YYERROR3
                                         } // End IF
                                        }
      // All multiple stmt errors propagate up to this point where we ABORT -- this ensures
      //   that the call to pl_yyparse terminates wth a non-zero error code.
    | StmtMult DIAMOND error            {DbgMsgWP (L"%%StmtMult:  error " WS_UTF16_DIAMOND L" StmtMult");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

                                             Assert (YYResIsEmpty ());

                                             // Get ptr to PerTabData global memory
                                             lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

                                             // If we're resetting via {goto}, ...
                                             if (lpMemPTD->lpSISCur->ResetFlag EQ RESETFLAG_ONE
                                              || lpMemPTD->lpSISCur->ResetFlag EQ RESETFLAG_ONE_INIT)
                                             {
                                                // Mark as not resetting
                                                 lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_NONE;

                                                 // Mark as a SYNTAX ERROR
                                                 if (!lpplLocalVars->bYYERROR)
                                                     ErrorMessageIndirect (ERRMSG_SYNTAX_ERROR);
                                             } // End IF

                                             // If we're resetting, ...
                                             if (lpMemPTD->lpSISCur->ResetFlag NE RESETFLAG_NONE)
                                                 lpplLocalVars->ExitType = TranslateResetFlagToExitType (lpMemPTD->lpSISCur->ResetFlag);

                                             // Set the exit type to error unless already set
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_NONE)
                                                 lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYABORT;
                                         } else
                                             YYABORT;
                                        }
    ;

// Single statement
StmtSing:
      /* Empty */                       {DbgMsgWP (L"%%StmtSing:  <empty>");
                                         if (!lpplLocalVars->bLookAhead)
                                             lpplLocalVars->ExitType = EXITTYPE_NOVALUE;
                                        }
    | error   CS_ANDIF                  {DbgMsgWP (L"%%StmtSing:  CS_ANDIF error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrValu CS_ANDIF                  {DbgMsgWP (L"%%StmtSing:  CS_ANDIF ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle ANDIF statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_IF_Stmt_EM (lpplLocalVars, &$2, &$1);
                                             FreeResult (&$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | error   CS_ASSERT                 {DbgMsgWP (L"%%StmtSing:  CS_ASSERT error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrValu CS_ASSERT                 {DbgMsgWP (L"%%StmtSing:  CS_ASSERT ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle ASSERT statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_ASSERT_Stmt_EM (lpplLocalVars, &$1);
                                             FreeResult (&$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | error   CS_CASE                   {DbgMsgWP (L"%%StmtSing:  CS_CASE error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrValu CS_CASE                   {DbgMsgWP (L"%%StmtSing:  CS_CASE ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle CASE/CASELIST statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_CASE_Stmt (lpplLocalVars, &$1);
                                             FreeResult (&$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | error   CS_CASELIST               {DbgMsgWP (L"%%StmtSing:  CS_CASELIST error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrValu CS_CASELIST               {DbgMsgWP (L"%%StmtSing:  CS_CASELIST ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle CASE/CASELIST statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_CASE_Stmt (lpplLocalVars, &$1);
                                             FreeResult (&$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | CS_CONTINUE                       {DbgMsgWP (L"%%StmtSing:  CS_CONTINUE");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle CONTINUE statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_CONTINUE_Stmt (lpplLocalVars, &$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | ArrValu CS_CONTINUEIF             {DbgMsgWP (L"%%StmtSing:  CS_CONTINUEIF ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle CONTINUEIF statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_CONTINUEIF_Stmt_EM (lpplLocalVars, &$2, &$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | CS_ENDREPEAT                      {DbgMsgWP (L"%%StmtSing:  CS_ENDREPEAT");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle ENDREPEAT statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_ENDREPEAT_Stmt (lpplLocalVars, &$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | CS_ENDWHILE                       {DbgMsgWP (L"%%StmtSing:  CS_ENDWHILE");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle ENDWHILE statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_ENDWHILE_Stmt (lpplLocalVars, &$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | CS_ELSE                           {DbgMsgWP (L"%%StmtSing:  CS_ELSE");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle ELSE statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_ELSE_Stmt (lpplLocalVars, &$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | error   CS_ELSEIF                 {DbgMsgWP (L"%%StmtSing:  CS_ELSEIF error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrValu CS_ELSEIF                 {DbgMsgWP (L"%%StmtSing:  CS_ELSEIF ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle ELSEIF statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_IF_Stmt_EM (lpplLocalVars, &$2, &$1);
                                             FreeResult (&$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | CS_ENDFOR                         {DbgMsgWP (L"%%StmtSing:  CS_ENDFOR");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle ENDFOR statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_ENDFOR_Stmt_EM (lpplLocalVars, &$1, FALSE);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | CS_ENDFORLCL                      {DbgMsgWP (L"%%StmtSing:  CS_ENDFORLCL");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle ENDFORLCL statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_ENDFOR_Stmt_EM (lpplLocalVars, &$1, TRUE);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | ArrValu CS_IN NameAnyVar CS_FOR   {DbgMsgWP (L"%%StmtSing:  CS_FOR NameAnyVar CS_IN ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle FOR statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_FOR_Stmt_EM (lpplLocalVars, &$4, &$3, &$2, &$1, FALSE);
                                             FreeResult (&$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | ArrValu CS_IN NameAnyVar CS_FORLCL{DbgMsgWP (L"%%StmtSing:  CS_FORLCL NameAnyVar CS_IN ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle FORLCL statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_FOR_Stmt_EM (lpplLocalVars, &$4, &$3, &$2, &$1, TRUE);
                                             FreeResult (&$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | error   CS_IF                     {DbgMsgWP (L"%%StmtSing:  CS_IF error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrValu CS_IF                     {DbgMsgWP (L"%%StmtSing:  CS_IF ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle IF statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_IF_Stmt_EM (lpplLocalVars, &$2, &$1);
                                             FreeResult (&$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | CS_LEAVE                          {DbgMsgWP (L"%%StmtSing:  CS_LEAVE");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle LEAVE statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_LEAVE_Stmt (lpplLocalVars, &$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | ArrValu CS_LEAVEIF                {DbgMsgWP (L"%%StmtSing:  CS_LEAVEIF ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle LEAVEIF statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_LEAVEIF_Stmt_EM (lpplLocalVars, &$2, &$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | error   CS_ORIF                   {DbgMsgWP (L"%%StmtSing:  CS_ORIF error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrValu CS_ORIF                   {DbgMsgWP (L"%%StmtSing:  CS_ORIF ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle ORIF statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_IF_Stmt_EM (lpplLocalVars, &$2, &$1);
                                             FreeResult (&$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | error   CS_SELECT                 {DbgMsgWP (L"%%StmtSing:  CS_SELECT error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrValu CS_SELECT                 {DbgMsgWP (L"%%StmtSing:  CS_SELECT ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle SELECT statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_SELECT_Stmt_EM (lpplLocalVars, &$2, &$1);
                                             FreeResult (&$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | CS_SKIPCASE                       {DbgMsgWP (L"%%StmtSing:  CS_SKIPCASE");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle SKIPCASE statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_SKIPCASE_Stmt (lpplLocalVars, &$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | CS_SKIPEND                        {DbgMsgWP (L"%%StmtSing:  CS_SKIPEND");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle SKIPEND statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_SKIPEND_Stmt (lpplLocalVars, &$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | error   CS_UNTIL                  {DbgMsgWP (L"%%StmtSing:  CS_UNTIL error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrValu CS_UNTIL                  {DbgMsgWP (L"%%StmtSing:  CS_UNTIL ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle UNTIL statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_IF_Stmt_EM (lpplLocalVars, &$2, &$1);
                                             FreeResult (&$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | error   CS_WHILE                  {DbgMsgWP (L"%%StmtSing:  CS_WHILE error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrValu CS_WHILE                  {DbgMsgWP (L"%%StmtSing:  CS_WHILE ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle WHILE statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_IF_Stmt_EM (lpplLocalVars, &$2, &$1);
                                             FreeResult (&$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | LeftOper                          {DbgMsgWP (L"%%StmtSing:  LeftOper");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (!(CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR))
                                                 // Mark as a SYNTAX ERROR
                                                 PrimFnSyntaxError_EM (&$1.tkToken APPEND_NAME_ARG);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
////| error                             //--Conflicts
    | ArrExpr                           {DbgMsgWP (L"%%StmtSing:  ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
                                             LPSIS_HEADER lpSISCur;          // Ptr to current SIS header

                                             if ((CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR) && !$1.tkToken.tkFlags.NoDisplay)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                             {
                                                 HGLOBAL hGlbDfnHdr;

                                                 // Get ptr to PerTabData global memory
                                                 lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

                                                 // Get a ptr to the current SIS header
                                                 lpSISCur = lpMemPTD->lpSISCur;

                                                 // Do not display if caller is Execute or Quad
                                                 //   and the current statement is the last one on the line
                                                 if (IsLastStmt (lpplLocalVars, $1.tkToken.tkCharIndex)
                                                  && lpSISCur
                                                  && (lpSISCur->DfnType EQ DFNTYPE_EXEC
                                                   || (lpSISCur->DfnType EQ DFNTYPE_IMM
                                                    && lpSISCur->lpSISPrv NE NULL
                                                    && (lpSISCur->lpSISPrv->DfnType EQ DFNTYPE_EXEC
                                                     || lpSISCur->lpSISPrv->DfnType EQ DFNTYPE_QUAD))))
                                                     // Handle ArrExpr if caller is Execute or quad
                                                     ArrExprCheckCaller (lpplLocalVars, lpSISCur, &$1, FALSE);
                                                 else
                                                 // Do not display if we're parsing an AFO, ...
                                                 if (hGlbDfnHdr = SISAfo (lpMemPTD))
                                                     lpplLocalVars->bRet =
                                                       AfoDisplay_EM (&$1.tkToken, FALSE, lpplLocalVars, hGlbDfnHdr);
                                                 else
                                                     lpplLocalVars->bRet =
                                                       ArrayDisplay_EM (&$1.tkToken, TRUE, &lpplLocalVars->bCtrlBreak);
                                             } // End IF/ELSE

                                             FreeResult (&$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;           // Stop executing this line

                                             // If the exit type is RESET_ONE_INIT, ...
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_RESET_ONE_INIT)
                                             {
                                                 LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
                                                 LPSIS_HEADER lpSISCur;          // Ptr to current SIS layer

                                                 // Get ptr to PerTabData global memory
                                                 lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

                                                 // Get a ptr to the current SIS layer
                                                 lpSISCur = lpMemPTD->lpSISCur;

                                                 // Peel back to the first non-Exec layer
                                                 while (lpSISCur->DfnType EQ DFNTYPE_EXEC)
                                                    lpSISCur = lpSISCur->lpSISPrv;

                                                 // Set the reset flag to ONE if user-defined function/operator
                                                 //   as we'll stop at the next ImmExec
                                                 if (lpSISCur->DfnType EQ DFNTYPE_OP1
                                                  || lpSISCur->DfnType EQ DFNTYPE_OP2
                                                  || lpSISCur->DfnType EQ DFNTYPE_FCN)
                                                 {
                                                     lpSISCur->ResetFlag = RESETFLAG_ONE;
                                                     lpplLocalVars->ExitType = EXITTYPE_RESET_ONE;
                                                 } else
                                                 {
                                                     if (!lpSISCur->ItsEC)
                                                         lpSISCur->ResetFlag = RESETFLAG_ONE_INIT;
                                                     lpplLocalVars->ExitType = EXITTYPE_RESET_ONE_INIT;
                                                 } // End IF/ELSE

                                                 YYACCEPT;              // Stop executing this line
                                             } // End IF

                                             // If we're resetting all levels, ...
                                             if (lpSISCur
                                              && lpSISCur->ResetFlag EQ RESETFLAG_ALL)
                                                 lpplLocalVars->ExitType = EXITTYPE_RESET_ALL;
                                             else
                                             // If the result is NoValue, mark the exit type as such
                                             if (IsTokenNoValue (&$1.tkToken))
                                                 lpplLocalVars->ExitType = EXITTYPE_NOVALUE;
                                             else
                                             // If the exit type isn't GOTO_LINE, mark it displayable or not
                                             if (lpplLocalVars->ExitType NE EXITTYPE_GOTO_LINE)
                                                 lpplLocalVars->ExitType =
                                                   $1.tkToken.tkFlags.NoDisplay ? EXITTYPE_NODISPLAY
                                                                                : EXITTYPE_DISPLAY;
                                         } // End IF
                                        }
    | error   AFOGUARD                  {DbgMsgWP (L"%%StmtSing:  AFOGUARD error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrValu AFOGUARD                  {DbgMsgWP (L"%%StmtSing:  AFOGUARD ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (!AfoGuard (lpplLocalVars, &$1.tkToken))
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             FreeResult (&$1);

                                             // No return value needed
                                         } // End IF
                                        }
    | error   AFORETURN                 {DbgMsgWP (L"%%StmtSing:  AFORETURN error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrExpr AFORETURN                 {DbgMsgWP (L"%%StmtSing:  AFORETURN ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             AfoReturn (lpplLocalVars, (&$1));

                                             YYACCEPT;              // Stop executing this line
                                         } // End IF
                                        }
    | error   ASSIGN AFORETURN          {DbgMsgWP (L"%%StmtSing:  AFORETURN " WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrValu ASSIGN AFORETURN          {DbgMsgWP (L"%%StmtSing:  AFORETURN " WS_UTF16_LEFTARROW L"ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $1.tkToken.tkFlags.NoDisplay = TRUE;
                                             AfoReturn (lpplLocalVars, (&$1));

                                             YYACCEPT;              // Stop executing this line
                                         } // End IF
                                        }
    | error   GOTO                      {DbgMsgWP (L"%%StmtSing:  " WS_UTF16_RIGHTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrValu GOTO                      {DbgMsgWP (L"%%StmtSing:  " WS_UTF16_RIGHTARROW L"ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->ExitType = GotoLine_EM (&$1.tkToken, &$2.tkToken);
                                             FreeResult (&$1);

                                             // Get ptr to PerTabData global memory
                                             lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

                                             // Make a PL_YYSTYPE NoValue entry
                                             lpplLocalVars->lpYYMak =
                                               MakeNoValue_YY (&$2.tkToken);
                                             lpMemPTD->YYResExec = *lpplLocalVars->lpYYMak;
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             // Split cases based upon the Exit Type
                                             switch (lpplLocalVars->ExitType)
                                             {
                                                 case EXITTYPE_GOTO_ZILDE:  // {zilde}
                                                     break;

                                                 case EXITTYPE_GOTO_LINE:   // Valid line #
                                                     // If we're not at a LABELSEP, EOS or EOL, YYERROR
                                                     if (lpplLocalVars->lptkNext[-1].tkFlags.TknType NE TKT_LABELSEP
                                                      && lpplLocalVars->lptkNext[-1].tkFlags.TknType NE TKT_EOS
                                                      && lpplLocalVars->lptkNext[-1].tkFlags.TknType NE TKT_EOL)
                                                     {
                                                         PrimFnSyntaxError_EM (&$2.tkToken APPEND_NAME_ARG);
                                                         YYERROR3
                                                     } else
                                                         YYACCEPT;          // Stop executing this line

                                                 case EXITTYPE_ERROR:       // Error
                                                 case EXITTYPE_STACK_FULL:  // ...
                                                     YYERROR3               // Stop on error

                                                 defstop
                                                     break;
                                             } // End SWITCH
                                         } // End IF
                                        }
    |         GOTO                      {DbgMsgWP (L"%%StmtSing:  " WS_UTF16_RIGHTARROW);
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
                                             LPSIS_HEADER lpSISCur;          // Ptr to current SIS layer

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 YYERROR3

                                             // If we're not at the EOS or EOL, YYERROR
                                             if (lpplLocalVars->lptkNext[-1].tkFlags.TknType NE TKT_EOL
                                              && lpplLocalVars->lptkNext[-1].tkFlags.TknType NE TKT_EOS)
                                             {
                                                 if (!lpplLocalVars->bYYERROR)
                                                     PrimFnSyntaxError_EM (&$1.tkToken APPEND_NAME_ARG);
                                                 YYERROR3
                                             } // End IF

                                             // Get ptr to PerTabData global memory
                                             lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

                                             // Get a ptr to the current SIS layer
                                             lpSISCur = lpMemPTD->lpSISCur;

                                             // Peel back to the first non-Exec layer
                                             while (lpSISCur->DfnType EQ DFNTYPE_EXEC)
                                                lpSISCur = lpSISCur->lpSISPrv;

                                             // Set the reset flag to ONE if user-defined function/operator
                                             //   as we'll stop at the next ImmExec
                                             if (lpSISCur->DfnType EQ DFNTYPE_OP1
                                              || lpSISCur->DfnType EQ DFNTYPE_OP2
                                              || lpSISCur->DfnType EQ DFNTYPE_FCN)
                                             {
                                                 lpSISCur->ResetFlag = RESETFLAG_ONE;
                                                 lpplLocalVars->ExitType = EXITTYPE_RESET_ONE;
                                             } else
                                             {
                                                 if (!lpSISCur->ItsEC)
                                                     lpSISCur->ResetFlag = RESETFLAG_ONE_INIT;
                                                 lpplLocalVars->ExitType = EXITTYPE_RESET_ONE_INIT;
                                             } // End IF/ELSE

                                             YYACCEPT;              // Stop executing this line
                                         } // End IF
                                        }
    | error   ASSIGN                    {DbgMsgWP (L"%%StmtSing:  " WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrValu ASSIGN                    {DbgMsgWP (L"%%StmtSing:  " WS_UTF16_LEFTARROW L"ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
                                             LPSIS_HEADER lpSISCur;          // Ptr to current SIS header
                                             HGLOBAL      hGlbDfnHdr;        // AFO DfnHdr global memory handle

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             // Get ptr to PerTabData global memory
                                             lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

                                             // Get a ptr to the current SIS header
                                             lpSISCur = lpMemPTD->lpSISCur;

                                             // Do not free if caller is Execute or Quad
                                             //   and the current statement is the last one on the line
                                             if (IsLastStmt (lpplLocalVars, $1.tkToken.tkCharIndex)
                                              && lpSISCur
                                              && (lpSISCur->DfnType EQ DFNTYPE_EXEC
                                               || (lpSISCur->DfnType EQ DFNTYPE_IMM
                                                && lpSISCur->lpSISPrv NE NULL
                                                && (lpSISCur->lpSISPrv->DfnType EQ DFNTYPE_EXEC
                                                 || lpSISCur->lpSISPrv->DfnType EQ DFNTYPE_QUAD))))
                                             {
                                                 // Handle ArrExpr if caller is Execute or quad
                                                 ArrExprCheckCaller (lpplLocalVars, lpSISCur, &$1, TRUE);

                                                 // Mark as already displayed
                                                 $1.tkToken.tkFlags.NoDisplay = TRUE;

                                                 if (lpplLocalVars->bStopExec)
                                                     YYACCEPT;           // Stop executing this line

                                                 // Pass on as result
                                                 $$ = $1;
                                             } else
                                             // Do not free if we're parsing an AFO, ...
                                             //   and the current statement is the last one on the line
                                             if (IsLastStmt (lpplLocalVars, $1.tkToken.tkCharIndex)
                                              && (hGlbDfnHdr = SISAfo (lpMemPTD)))
                                                 lpplLocalVars->bRet =
                                                   AfoDisplay_EM (&$1.tkToken, TRUE, lpplLocalVars, hGlbDfnHdr);
                                             else
                                                 FreeResult (&$1);

                                             // Mark the result as already displayed
                                             lpplLocalVars->ExitType = EXITTYPE_NODISPLAY;
                                         } // End IF
                                        }
////     | ArrExpr NAMEOP3                   {DbgMsgWP (L"%%StmtSing:  NAMEOP3 ArrExpr");
////                                              DbgBrk ();
////
////                                              lpplLocalVars->lpYYMak =
////                                                MakeNameFcnOpr_YY (&$2, TRUE);
////
////                                              if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
////                                              {
////                                                  FreeResult (&$1);
////                                                  YYERROR3
////                                              } // End IF
////
////                                              // Change the first token in the function strand
////                                              //   from ambiguous operator to a function
////                                              AmbOpToFcn (lpplLocalVars->lpYYMak);
////
////                                              lpplLocalVars->lpYYOp3 =
////                                                PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Ambiguous operator (Direct)
////                                              YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;
////
////                                              if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
////                                              {
////                                                  FreeResult (&$1);
////                                                  YYERROR3
////                                              } // End IF
////
////                                              lpplLocalVars->lpYYFcn =
////                                                MakeFcnStrand_EM_YY (lpplLocalVars->lpYYOp3, NAMETYPE_FN12, FALSE);
////
////                                              if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
////                                              {
////                                                  FreeResult (&$1);
////                                                  YYERROR3
////                                              } // End IF
////
////                                              lpplLocalVars->lpYYRes =
////                                                ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken, FALSE, TRUE);
////                                              YYFreeArray (lpplLocalVars->lpYYFcn);
////                                              FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
////
////                                              if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
////                                                  YYERROR3
////
////                                              $$ = *lpplLocalVars->lpYYRes;
////                                              YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
////                                              YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
////                                         }
    | FcnSpec                           {DbgMsgWP (L"%%StmtSing:  FcnSpec");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

                                             // Free the result as the arg to the name was passed on
                                             FreeResult (&$1);

                                             // Get ptr to PerTabData global memory
                                             lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

                                             if (lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_IMM
                                              || lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_EXEC
                                              || lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QUAD)
                                                 lpplLocalVars->ExitType = EXITTYPE_NOVALUE;

                                             $$ = $1;
                                         } // End IF
                                        }
    | Op1Spec                           {DbgMsgWP (L"%%StmtSing:  Op1Spec");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

                                             // Free the result as the arg to the name was passed on
                                             FreeResult (&$1);

                                             // Get ptr to PerTabData global memory
                                             lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

                                             if (lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_IMM
                                              || lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_EXEC
                                              || lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QUAD)
                                                 lpplLocalVars->ExitType = EXITTYPE_NOVALUE;

                                             $$ = $1;
                                         } // End IF
                                        }
    | Op2Spec                           {DbgMsgWP (L"%%StmtSing:  Op2Spec");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

                                             // Free the result as the arg to the name was passed on
                                             FreeResult (&$1);

                                             // Get ptr to PerTabData global memory
                                             lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

                                             if (lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_IMM
                                              || lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_EXEC
                                              || lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QUAD)
                                                 lpplLocalVars->ExitType = EXITTYPE_NOVALUE;

                                             $$ = $1;
                                         } // End IF
                                        }
    | Op3Spec                           {DbgMsgWP (L"%%StmtSing:  Op3Spec");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

                                             // Free the result as the arg to the name was passed on
                                             FreeResult (&$1);

                                             // Get ptr to PerTabData global memory
                                             lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

                                             if (lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_IMM
                                              || lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_EXEC
                                              || lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QUAD)
                                                 lpplLocalVars->ExitType = EXITTYPE_NOVALUE;

                                             $$ = $1;
                                         } // End IF
                                        }
// The following productions ending in EOL are for bLookAhead only
    |     error   EOL                   {DbgMsgWP (L"%%StmtSing:  EOL error");
                                         if (lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
////|     IndexListWE EOL               {DbgMsgWP (L"%%StmtSing:  EOL IndexListWE");
////                                     if (lpplLocalVars->bLookAhead)
////                                     {
////                                         lpplLocalVars->plNameType = NAMETYPE_LST;
////                                         YYACCEPT;              // Stop executing this line
////                                     } else
////                                         YYERROR2
////                                    }
    |     ArrExpr EOL                   {DbgMsgWP (L"%%StmtSing:  EOL ArrExpr");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_VAR;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     FcnSpec EOL                   {DbgMsgWP (L"%%StmtSing:  EOL FcnSpec");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_FN12;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     Op1Spec EOL                   {DbgMsgWP (L"%%StmtSing:  EOL Op1Spec");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_OP1;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     Op1Spec LeftOper EOL          {DbgMsgWP (L"%%StmtSing:  EOL LeftOper Op1Spec");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_FN12;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     Op2Spec EOL                   {DbgMsgWP (L"%%StmtSing:  EOL Op2Spec");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_OP2;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     Op3Spec EOL                   {DbgMsgWP (L"%%StmtSing:  EOL Op3Spec");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_OP3;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     MonOp   EOL                   {DbgMsgWP (L"%%StmtSing:  EOL MonOp");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_OP1;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     DydOp   EOL                   {DbgMsgWP (L"%%StmtSing:  EOL DydOp");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_OP2;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     AmbOpX  EOL                   {DbgMsgWP (L"%%StmtSing:  EOL AmbOpX");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_OP3;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     LeftOper EOL                  {DbgMsgWP (L"%%StmtSing:  EOL LeftOper");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_FN12;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     AxisFunc EOL                  {DbgMsgWP (L"%%StmtSing:  EOL AxisFunc");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_FN12;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     Drv1Func EOL                  {DbgMsgWP (L"%%StmtSing:  EOL Drv1Func");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_FN12;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     Drv2Func EOL                  {DbgMsgWP (L"%%StmtSing:  EOL Drv2Func");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_FN12;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     Drv4Func EOL                  {DbgMsgWP (L"%%StmtSing:  EOL Drv4Func");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_FN12;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     Drv5Func EOL                  {DbgMsgWP (L"%%StmtSing:  EOL Drv5Func");    // Includes Drv3Func
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_FN12;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     Train EOL                     {DbgMsgWP (L"%%StmtSing:  EOL Train");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_TRN;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    ;

NameAnyVar:
      NAMEUNK                           {DbgMsgWP (L"%%NameAnyVar:  NAMEUNK");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | NAMEVAR                           {DbgMsgWP (L"%%NameAnyVar:  NAMEVAR");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    ;

NameAnyOpN:
      NAMEUNK                           {DbgMsgWP (L"%%NameAnyOpN:  NAMEUNK");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | NAMEVAR                           {DbgMsgWP (L"%%NameAnyOpN:  NAMEVAR");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | NAMEFCN                           {DbgMsgWP (L"%%NameAnyOpN:  NAMEFCN");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | NAMETRN                           {DbgMsgWP (L"%%NameAnyOpN:  NAMETRN");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | NAMEOP1                           {DbgMsgWP (L"%%NameAnyOpN:  NAMEOP1");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | NAMEOP2                           {DbgMsgWP (L"%%NameAnyOpN:  NAMEOP2");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | NAMEOP3                           {DbgMsgWP (L"%%NameAnyOpN:  NAMEOP3");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    ;

// Function specification
FcnSpec:
////  error    ASSIGN NameAnyOpN        //--Conflicts
      LeftOper ASSIGN NameAnyOpN        {DbgMsgWP (L"%%FcnSpec:  NameAnyOpN" WS_UTF16_LEFTARROW L"LeftOper");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
/////////////////////////////////////////////////FreeResult (&$3);                   // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
/////////////////////////////////////////////YYFreeArray (lpplLocalVars->lpYYFcn);
/////////////////////////////////////////////FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;     // DO NOT FREE:  Passed on as result
/////////////////////////////////////////////FreeResult (&$3);                       // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 YYFreeArray (lpplLocalVars->lpYYFcn);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
////| error    ASSIGN NameAnyOpN        //--Conflicts
    | Drv1Func ASSIGN NameAnyOpN        {DbgMsgWP (L"%%FcnSpec:  NameAnyOpN" WS_UTF16_LEFTARROW L"Drv1Func");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
/////////////////////////////////////////////////FreeResult (&$3);                   // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
/////////////////////////////////////////////YYFreeArray (lpplLocalVars->lpYYFcn);
/////////////////////////////////////////////FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;     // DO NOT FREE:  Passed on as result
/////////////////////////////////////////////FreeResult (&$3);                       // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 YYFreeArray (lpplLocalVars->lpYYFcn);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
////| error    ASSIGN NameAnyOpN        //--Conflicts
    | Drv2Func ASSIGN NameAnyOpN        {DbgMsgWP (L"%%FcnSpec:  NameAnyOpN" WS_UTF16_LEFTARROW L"Drv2Func");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
/////////////////////////////////////////////////FreeResult (&$3);                   // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
/////////////////////////////////////////////YYFreeArray (lpplLocalVars->lpYYFcn);
/////////////////////////////////////////////FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;     // DO NOT FREE:  Passed on as result
/////////////////////////////////////////////FreeResult (&$3);                       // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 YYFreeArray (lpplLocalVars->lpYYFcn);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | Drv3Func ASSIGN NameAnyOpN        {DbgMsgWP (L"%%FcnSpec:  NameAnyOpN" WS_UTF16_LEFTARROW L"Drv3Func");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
/////////////////////////////////////////////////FreeResult (&$3);                   // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
/////////////////////////////////////////////YYFreeArray (lpplLocalVars->lpYYFcn);
/////////////////////////////////////////////FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;     // DO NOT FREE:  Passed on as result
/////////////////////////////////////////////FreeResult (&$3);                       // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 YYFreeArray (lpplLocalVars->lpYYFcn);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | Drv4Func ASSIGN NameAnyOpN        {DbgMsgWP (L"%%FcnSpec:  NameAnyOpN" WS_UTF16_LEFTARROW L"Drv4Func");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
/////////////////////////////////////////////////FreeResult (&$3);                   // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
/////////////////////////////////////////////YYFreeArray (lpplLocalVars->lpYYFcn);
/////////////////////////////////////////////FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;     // DO NOT FREE:  Passed on as result
/////////////////////////////////////////////FreeResult (&$3);                       // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 YYFreeArray (lpplLocalVars->lpYYFcn);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
////| error    ASSIGN NameAnyOpN        //--Conflicts
    | AxisFunc ASSIGN NameAnyOpN        {DbgMsgWP (L"%%FcnSpec:  NameAnyOpN" WS_UTF16_LEFTARROW L"AxisFunc");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
/////////////////////////////////////////////////FreeResult (&$3);                   // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
/////////////////////////////////////////////YYFreeArray (lpplLocalVars->lpYYFcn);
/////////////////////////////////////////////FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;     // DO NOT FREE:  Passed on as result
/////////////////////////////////////////////FreeResult (&$3);                       // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 YYFreeArray (lpplLocalVars->lpYYFcn);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | Drv6Func ASSIGN NameAnyOpN        {DbgMsgWP (L"%%FcnSpec:  NameAnyOpN" WS_UTF16_LEFTARROW L"Drv6Func");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
/////////////////////////////////////////////////FreeResult (&$3);                   // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
/////////////////////////////////////////////YYFreeArray (lpplLocalVars->lpYYFcn);
/////////////////////////////////////////////FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;     // DO NOT FREE:  Passed on as result
/////////////////////////////////////////////FreeResult (&$3);                       // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 YYFreeArray (lpplLocalVars->lpYYFcn);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    ;

Op1Spec:
      MonOp    ASSIGN NameAnyOpN        {DbgMsgWP (L"%%Op1Spec:  NameAnyOpN" WS_UTF16_LEFTARROW L"MonOp");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_OP1, TRUE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
/////////////////////////////////////////////////FreeResult (&$3);                   // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
/////////////////////////////////////////////YYFreeArray (lpplLocalVars->lpYYFcn);
/////////////////////////////////////////////FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;     // DO NOT FREE:  Passed on as result
/////////////////////////////////////////////FreeResult (&$3);                       // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1);
                                                 YYFreeArray (lpplLocalVars->lpYYFcn);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = $1;

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;

                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
// The following assignment can be made to work if we switch the order of the arguments
//   to a dyadic operator in a function strand from D L R to D R L and make all corresponding
//   changes elsewhere such as whenever parsing a function strand.
////| RightOper DydOp ASSIGN NameAnyOpN {DbgMsgWP (L"%%Op1Spec:  NameAnyOpN" WS_UTF16_LEFTARROW L"DydOp RightOper");
////                                     // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
////                                     if (!lpplLocalVars->bLookAhead)
////                                     {
////                                         lpplLocalVars->lpYYOp2 =
////                                           PushFcnStrand_YY (&$2, 2, INDIRECT);  // Dyadic operator (Indirect)
////
////                                         lpplLocalVars->lpYYRht =
////                                           PushFcnStrand_YY (&$1, 1, INDIRECT);  // Right operand (Indirect)
////
////                                         if (!lpplLocalVars->lpYYOp2             // If not defined, free args and YYERROR
////                                          || !lpplLocalVars->lpYYRht)
////                                         {
////                                             if (lpplLocalVars->lpYYOp2)         // If defined, free it
////                                             {
////                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
////                                             } // End IF
////
////                                             if (lpplLocalVars->lpYYRht)         // If defined, free it
////                                             {
////                                                 FreeYYFcn1 (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
////                                             } // End IF
////
//// ////////////////////////////////////////////FreeResult (&$4);                   // Validation only
////                                             YYERROR3
////                                         } // End IF
////
////                                         lpplLocalVars->lpYYRes =
////                                           MakeFcnStrand_EM_YY (lpplLocalVars->lpYYOp2, NAMETYPE_OP1, TRUE);
////
////                                         if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
////                                         {
////                                             FreeYYFcn1 (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
////                                             FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
//// ////////////////////////////////////////////FreeResult (&$4);                   // Validation only
////                                             YYERROR3
////                                         } // End IF
////
////                                         if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
////                                             lpplLocalVars->bRet = FALSE;
////                                         else
////                                             lpplLocalVars->bRet =
////                                               AssignName_EM (&$4.tkToken, &lpplLocalVars->lpYYRes->tkToken);
//// ////////////////////////////////////////YYFreeArray (lpplLocalVars->lpYYRes);
//// ////////////////////////////////////////FreeYYFcn1 (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;     // DO NOT FREE:  Passed on as result
//// ////////////////////////////////////////FreeResult (&$4);                       // Validation only
////
////                                         if (!lpplLocalVars->bRet)
////                                             YYERROR3
////
////                                         // The result is always the root of the function tree
////                                         $$ = *lpplLocalVars->YYRes;
////
////                                         // Mark as already displayed
////                                         $$.tkToken.tkFlags.NoDisplay = TRUE;
////
////                                         YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
////                                         YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
////                                         YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
////                                     } // End IF
////                                    }
    ;

// Dyadic operator specification
Op2Spec:
      DydOp    ASSIGN NameAnyOpN        {DbgMsgWP (L"%%Op2Spec:  NameAnyOpN" WS_UTF16_LEFTARROW L"DydOp");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_OP2, TRUE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
/////////////////////////////////////////////////FreeResult (&$3);                   // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
/////////////////////////////////////////////YYFreeArray (lpplLocalVars->lpYYFcn);
/////////////////////////////////////////////FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;     // DO NOT FREE:  Passed on as result
/////////////////////////////////////////////FreeResult (&$3);                       // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 YYFreeArray (lpplLocalVars->lpYYFcn);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    ;


// Ambiguous operator assignment
AmbOpAssign:
      OP3ASSIGN                         {DbgMsgWP (L"%%AmbOpAssign:  OP3ASSIGN");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimOp3_YY (&$1);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Ambiguous operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp3;
                                             YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                         } // End IF
                                        }
    | NAMEOP3ASSIGN                     {DbgMsgWP (L"%%AmbOpAssign:  NAMEOP3ASSIGN");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, TRUE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Ambiguous operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp3;
                                             YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                         } // End IF
                                        }
    | AmbOpX                            {DbgMsgWP (L"%%AmbOpAssign:  AmbOpX");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | AmbOpAssign OP3ASSIGN             {DbgMsgWP (L"%%AmbOpAssign:  OP3ASSIGN AmbOpAssign");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimOp3_YY (&$2);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Ambiguous operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp3;

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Ambiguous operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                         } // End IF
                                        }
    | AmbOpAssign NAMEOP3ASSIGN         {DbgMsgWP (L"%%AmbOpAssign:  NAMEOP3ASSIGN AmbOpAssign");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$2, TRUE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Ambiguous operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp3;

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Ambiguous operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                         } // End IF
                                        }
    ;


// Ambiguous operator specification
Op3Spec:
      AmbOpAssign   ASSIGN NameAnyOpN   {DbgMsgWP (L"%%Op3Spec:  NameAnyOpN" WS_UTF16_LEFTARROW L"AmbOp");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_OP3, TRUE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
/////////////////////////////////////////////////FreeResult (&$3);                   // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
/////////////////////////////////////////////YYFreeArray (lpplLocalVars->lpYYFcn);
/////////////////////////////////////////////FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;     // DO NOT FREE:  Passed on as result
/////////////////////////////////////////////FreeResult (&$3);                       // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1);
                                                 YYFreeArray (lpplLocalVars->lpYYFcn);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = $1;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    ;

// Array expression w/value
ArrValu:
      ArrExpr                           {DbgMsgWP (L"%%ArrValu:  ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (IsTokenNoValue (&$1.tkToken))
                                             {
                                                 PrimFnValueError_EM (&$1.tkToken APPEND_NAME_ARG);
                                                 YYERROR3
                                             } // End IF

                                             $$ = $1;
                                         } // End IF
                                        }
    ;

// Array expression
ArrExpr:
//////error                             //--Conflicts
      SimpExpr                          {DbgMsgWP (L"%%ArrExpr:  SimpExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | StrandInst                        {DbgMsgWP (L"%%ArrExpr:  StrandInst");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | error   LeftMonOper               {DbgMsgWP (L"%%ArrExpr:  LeftMonOper error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResNNU (&$2);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
////| ArrExpr error                     //--Conflicts
    | StrandOp2 LeftMonOper             {DbgMsgWP (L"%%ArrExpr:  LeftMonOper StrandOp2");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Increment the RefCnt because StrandOp2 doesn't
                                             //   go through MakeFcnStrand_EM_YY as this is a
                                             //   variable not an operand.
                                             DbgIncrRefCntTkn (&$1.tkToken);

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken, FALSE, TRUE);
                                             YYFreeArray (lpplLocalVars->lpYYFcn);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
////| ArrValu error                     {DbgMsgWP (L"%%ArrExpr:  error ArrValu");
////                                     if (!lpplLocalVars->bLookAhead)
////                                     {
////                                         FreeResult (&$1);
////                                         YYERROR3
////                                     } else
////                                         YYERROR2
////                                    }
    | ArrValu LeftMonOper               {DbgMsgWP (L"%%ArrExpr:  LeftMonOper ArrValu");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken, FALSE, TRUE);
                                             YYFreeArray (lpplLocalVars->lpYYFcn);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 if (lpplLocalVars->ExitType EQ EXITTYPE_RETURNxLX)
                                                     YYABORT;
                                                 YYERROR3
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | ArrValu LeftOper                  {DbgMsgWP (L"%%ArrExpr:  LeftOper ArrValu");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken, FALSE, TRUE);
                                             YYFreeArray (lpplLocalVars->lpYYFcn);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 if (lpplLocalVars->ExitType EQ EXITTYPE_RETURNxLX)
                                                     YYABORT;
                                                 YYERROR3
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | error   AxisFunc                  {DbgMsgWP (L"%%ArrExpr:  AxisFunc error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$2);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
////| ArrExpr error                     //--Conflicts
    | ArrValu AxisFunc                  {DbgMsgWP (L"%%ArrExpr:  AxisFunc ArrValu");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken, FALSE, TRUE);
                                             YYFreeArray (lpplLocalVars->lpYYFcn);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | ArrValu LeftOper error            {DbgMsgWP (L"%%ArrExpr:  error LeftOper ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1);
                                             FreeResNNU (&$2);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | error   LeftOper StrandInst       {DbgMsgWP (L"%%ArrExpr:  StrandInst LeftOper error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResNNU (&$2);
                                             FreeResult (&$3);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
////| ArrValu LeftOper error            //--Conflicts
////| ArrValu error    StrandInst       //--Conflicts
    | ArrValu LeftOper StrandInst       {DbgMsgWP (L"%%ArrExpr:  StrandInst LeftOper ArrValu");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (&$3.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken, TRUE, TRUE);
                                             YYFreeArray (lpplLocalVars->lpYYFcn);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | ArrValu AmbOpX   error            {DbgMsgWP (L"%%ArrExpr:  error AmbOpX ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1);
                                             FreeResult (&$2);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | error   AmbOpX StrandInst         {DbgMsgWP (L"%%ArrExpr:  StrandInst AmbOpX error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$2);
                                             FreeResult (&$3);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrValu AmbOpX NAMEOP3ASSIGN      {DbgMsgWP (L"%%ArrExpr:  OP3ASSIGN AmbOpX ArrValu");
                                         DbgBrk ();     // ***FIXME*** -- Can we ever get here??



                                        }
    | error   AmbOpX NAMEOP3ASSIGN      {DbgMsgWP (L"%%ArrExpr:  OP3ASSIGN AmbOpX error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$2);
                                             FreeResult (&$3);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrValu AmbOpX OP3ASSIGN          {DbgMsgWP (L"%%ArrExpr:  OP3ASSIGN AmbOpX ArrValu");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimOp3_YY (&$3);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             // Change the first token in the function strand
                                             //   from ambiguous operator to a function
                                             AmbOpToFcn (lpplLocalVars->lpYYMak);

                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (&$2, 2, INDIRECT);  // Ambiguous operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Ambiguous operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (lpplLocalVars->lpYYOp3, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken, FALSE, TRUE);
                                             YYFreeArray (lpplLocalVars->lpYYFcn);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes;

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                             YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }

    | ArrValu AmbOpX StrandInst         {DbgMsgWP (L"%%ArrExpr:  StrandInst AmbOpX ArrValu");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Change the first token from ambiguous operator to a function
                                             //   and swap the first two tokens
                                             if (!AmbOpSwap_EM (&$2))
                                             {
                                                 FreeResult (&$1);
                                                 FreeResult (&$2);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             // Because MakeFcnStrand might have brought in a function strand from
                                             //   a global which still has TKT_OP3IMMED, change the token type to
                                             //   TKT_FCNIMMED.
                                             // Change the first token in the function strand
                                             //   from ambiguous operator to a function
                                             AmbOpToFcn (lpplLocalVars->lpYYFcn);

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (&$3.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken, TRUE, TRUE);
                                             YYFreeArray (lpplLocalVars->lpYYFcn);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | error   Drv5Func    StrandInst    {DbgMsgWP (L"%%ArrExpr:  StrandInst Drv5Func error");    // Also Drv3Func
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$2);
                                             FreeResult (&$3);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrValu Drv5Func    error         {DbgMsgWP (L"%%ArrExpr:  error Drv5Func ArrValu");       // Also Drv3Func
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1);
                                             FreeResult (&$2);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrValu Drv5Func    StrandInst    {DbgMsgWP (L"%%ArrExpr:  StrandInst Drv5Func ArrValu");  // Also Drv3Func
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, TRUE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (&$3.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken, TRUE, TRUE);
                                             YYFreeArray (lpplLocalVars->lpYYFcn);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | ArrValu AxisFunc error            {DbgMsgWP (L"%%ArrExpr:  error AxisFunc ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1);
                                             FreeResult (&$2);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | error   AxisFunc StrandInst       {DbgMsgWP (L"%%ArrExpr:  StrandInst AxisFunc error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$2);
                                             FreeResult (&$3);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
////| ArrValu AxisFunc error            //--Conflicts
////| ArrValu error    StrandInst       //--Conflicts
    | ArrValu AxisFunc StrandInst       {DbgMsgWP (L"%%ArrExpr:  StrandInst AxisFunc ArrValu");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (&$3.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken, TRUE, TRUE);
                                             YYFreeArray (lpplLocalVars->lpYYFcn);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Strand instance to the right of a dyadic op
StrandOp2:
          OP2CONSTANT                   {DbgMsgWP (L"%%StrandOp2:  OP2CONSTANT");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    |     OP2NAMEVAR                    {DbgMsgWP (L"%%StrandOp2:  OP2NAMEVAR");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    |     OP2CHRSTRAND                  {DbgMsgWP (L"%%StrandOp2:  OP2CHRSTRAND");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    |     OP2NUMSTRAND                  {DbgMsgWP (L"%%StrandOp2:  OP2NUMSTRAND");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    ;

// Single var (including single names)
SingVar:
          NAMEUNK                       {DbgMsgWP (L"%%SingVar:  NAMEUNK");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (!(CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR))
                                                 PrimFnValueError_EM (&$1.tkToken APPEND_NAME_ARG);
                                             YYERROR3
                                         } // End IF
                                        }
    |     QUAD                          {DbgMsgWP (L"%%SingVar:  QUAD");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
                                             RESET_FLAGS  resetFlag;             // The current Reset Flag

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 YYERROR3

                                             lpplLocalVars->lpYYRes =
                                               WaitForInput (lpplLocalVars->hWndSM, FALSE, &$1.tkToken);
                                             FreeResult (&$1);

                                             // Get ptr to PerTabData global memory
                                             lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

                                             // Get the Reset Flag
                                             resetFlag = lpMemPTD->lpSISCur->ResetFlag;

                                             // If we're resetting, ...
                                             if (resetFlag NE RESETFLAG_NONE)
                                                 lpplLocalVars->ExitType = TranslateResetFlagToExitType (resetFlag);

                                             // If we're resetting, ...
                                             if (resetFlag NE RESETFLAG_NONE)
                                                 YYACCEPT;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |     QUOTEQUAD                     {DbgMsgWP (L"%%SingVar:  QUOTEQUAD");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 YYERROR3

                                             lpplLocalVars->lpYYRes =
                                               WaitForInput (lpplLocalVars->hWndSM, TRUE, &$1.tkToken);
                                             FreeResult (&$1);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |     CONSTANT                      {DbgMsgWP (L"%%SingVar:  CONSTANT");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    |     NAMEVAR                       {DbgMsgWP (L"%%SingVar:  NAMEVAR");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    |     SYSLBL                        {DbgMsgWP (L"%%SingVar:  SYSLBL");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    |     USRFN0                        {DbgMsgWP (L"%%SingVar:  USRFN0");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecuteFn0 (&$1);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             // Because there's no matching DecrRefCnt to the IncrRefCnt
                                             //   in MakeVarStrand_EM_YY, we mark this array as skipping
                                             //   the next IncrRefCnt.
                                             SetVFOArraySRCIFlag (&$$.tkToken);
                                         } // End IF
                                        }
    |     SYSFN0                        {DbgMsgWP (L"%%SingVar:  SYSFN0");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecuteFn0 (&$1);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             // Because there's no matching DecrRefCnt to the IncrRefCnt
                                             //   in MakeVarStrand_EM_YY, we mark this array as skipping
                                             //   the next IncrRefCnt.
                                             SetVFOArraySRCIFlag (&$$.tkToken);
                                         } // End IF
                                        }
    | RBRACE_FCN_FN0 StmtMult LBRACE    {DbgMsgWP (L"%%SingVar:  { StmtMult }");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Gather together the tokens between the braces into an AFO
                                             lpplLocalVars->lpYYFcn =
                                               MakeAfo_EM_YY (&$3, &$1, lpplLocalVars);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecuteFn0 (lpplLocalVars->lpYYFcn);

                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |     CHRSTRAND                     {DbgMsgWP (L"%%SingVar:  CHRSTRAND");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    |     NUMSTRAND                     {DbgMsgWP (L"%%SingVar:  NUMSTRAND");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    | ')' error   '('                   {DbgMsgWP (L"%%SingVar:  error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ')' ArrExpr '('                   {DbgMsgWP (L"%%SingVar:  (ArrExpr)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $$ = $2;

                                             // Because there's no matching DecrRefCnt to the IncrRefCnt
                                             //   in MakeVarStrand_EM_YY, we mark this array as skipping
                                             //   the next IncrRefCnt.
                                             SetVFOArraySRCIFlag (&$$.tkToken);
                                         } // End IF
                                        }
    ;

// Index var with brackets
IndexVarBR:
      IndexListBR     error             {DbgMsgWP (L"%%IndexVarBR:  error IndexListBR");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | IndexListBR     SingVar           {DbgMsgWP (L"%%IndexVarBR:  SingVar IndexListBR");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (IsTokenNoValue (&$2.tkToken))
                                             {
                                                 PrimFnValueError_EM (&$2.tkToken APPEND_NAME_ARG);
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             InitVarStrand (&$2);

                                             lpplLocalVars->lpYYRes =
                                               PushVarStrand_YY (&$2);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYStr =
                                               MakeVarStrand_EM_YY (lpplLocalVars->lpYYRes);
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             if (!lpplLocalVars->lpYYStr)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ArrayIndexRef_EM_YY (&lpplLocalVars->lpYYStr->tkToken, &$1.tkToken);
                                             FreeResult (&$1);
                                             FreeYYFcn1 (lpplLocalVars->lpYYStr); lpplLocalVars->lpYYStr = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Strand Recursion
StrandRec:
////  error                             //--Conflicts
      SingVar                           {DbgMsgWP (L"%%StrandRec:  SingVar");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             InitVarStrand (&$1);

                                             lpplLocalVars->lpYYRes =
                                               PushVarStrand_YY (&$1);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | IndexVarBR                        {DbgMsgWP (L"%%StrandRec:  IndexVarBR");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Because there's no matching DecrRefCnt to the IncrRefCnt
                                             //   in MakeVarStrand_EM_YY, we mark this array as skipping
                                             //   the next IncrRefCnt.
                                             SetVFOArraySRCIFlag (&$1.tkToken);

                                             InitVarStrand (&$1);

                                             lpplLocalVars->lpYYRes =
                                               PushVarStrand_YY (&$1);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | StrandRec   error                 {DbgMsgWP (L"%%StrandRec:  error StrandRec");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYStr =
                                               MakeVarStrand_EM_YY (&$1);

                                             if (!lpplLocalVars->lpYYStr)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             FreeYYFcn1 (lpplLocalVars->lpYYStr); lpplLocalVars->lpYYStr = NULL;

                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | StrandRec   SingVar               {DbgMsgWP (L"%%StrandRec:  SingVar StrandRec");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                               PushVarStrand_YY (&$2);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | StrandRec IndexVarBR              {DbgMsgWP (L"%%StrandRec:  IndexVarBR StrandRec");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                               PushVarStrand_YY (&$2);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Strand instance
StrandInst:
////  error                             //--Conflicts
      StrandRec                         {DbgMsgWP (L"%%StrandInst:  StrandRec");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYStr =
                                               MakeVarStrand_EM_YY (&$1);

                                             if (!lpplLocalVars->lpYYStr)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYStr;
                                             YYFree (lpplLocalVars->lpYYStr); lpplLocalVars->lpYYStr = NULL;
                                         } // End IF
                                        }
    ;

// Simple array expression
SimpExpr:
      error   ASSIGN       QUAD         {DbgMsgWP (L"%%SimpExpr:  " WS_UTF16_QUAD WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrValu ASSIGN       QUAD         {DbgMsgWP (L"%%SimpExpr:  " WS_UTF16_QUAD WS_UTF16_LEFTARROW L"ArrValu");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
/////////////////////////////////////////////LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
/////////////////////////////////////////////HGLOBAL      hGlbDfnHdr;        // AFO DfnHdr global memory handle
/////////////////////////////////////////////
/////////////////////////////////////////////// Get ptr to PerTabData global memory
/////////////////////////////////////////////lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));
/////////////////////////////////////////////
                                             // Mark as NOT already displayed
                                             $1.tkToken.tkFlags.NoDisplay = FALSE;
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
/////////////////////////////////////////////// If we're parsing an AFO, ... (NOT WITH ASSIGN QUAD)
/////////////////////////////////////////////if (hGlbDfnHdr = SISAfo (lpMemPTD))
/////////////////////////////////////////////{
/////////////////////////////////////////////    lpplLocalVars->bRet =
/////////////////////////////////////////////      AfoDisplay_EM (&$1.tkToken, FALSE, lpplLocalVars, hGlbDfnHdr);
/////////////////////////////////////////////
/////////////////////////////////////////////    YYACCEPT;           // Stop executing this line
/////////////////////////////////////////////} else
                                                 lpplLocalVars->bRet =
                                                   ArrayDisplay_EM (&$1.tkToken, TRUE, &lpplLocalVars->bCtrlBreak);
/////////////////////////////////////////////FreeResult (&$1);                       // DO NOT FREE:  Passed on as result

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             // Mark as already displayed
                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN       QUOTEQUAD    {DbgMsgWP (L"%%SimpExpr:  " WS_UTF16_QUOTEQUAD WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrValu ASSIGN       QUOTEQUAD    {DbgMsgWP (L"%%SimpExpr:  " WS_UTF16_QUOTEQUAD WS_UTF16_LEFTARROW L"ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
/////////////////////////////////////////////LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
/////////////////////////////////////////////HGLOBAL      hGlbDfnHdr;        // AFO DfnHdr global memory handle
/////////////////////////////////////////////
/////////////////////////////////////////////// Get ptr to PerTabData global memory
/////////////////////////////////////////////lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));
/////////////////////////////////////////////
                                             // Mark as NOT already displayed
                                             $1.tkToken.tkFlags.NoDisplay = FALSE;
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
/////////////////////////////////////////////// If we're parsing an AFO, ... (NOT WITH ASSIGN QUOTEQUAD)
/////////////////////////////////////////////if (hGlbDfnHdr = SISAfo (lpMemPTD))
/////////////////////////////////////////////{
/////////////////////////////////////////////    lpplLocalVars->bRet =
/////////////////////////////////////////////      AfoDisplay_EM (&$1.tkToken, FALSE, lpplLocalVars, hGlbDfnHdr);
/////////////////////////////////////////////
/////////////////////////////////////////////    YYACCEPT;           // Stop executing this line
/////////////////////////////////////////////} else
                                                 lpplLocalVars->bRet =
                                                   ArrayDisplay_EM (&$1.tkToken, FALSE, &lpplLocalVars->bCtrlBreak);
/////////////////////////////////////////////FreeResult (&$1);                       // DO NOT FREE:  Passed on as result

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             // Mark as already displayed
                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN       NameAnyVar   {DbgMsgWP (L"%%SimpExpr:  NameAnyVar" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
/////////////////////////////////////////////FreeResult (&$3);                       // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrValu ASSIGN       NameAnyVar   {DbgMsgWP (L"%%SimpExpr:  NameAnyVar" WS_UTF16_LEFTARROW L"ArrValu");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Copy as temporary var in case it's named
                                             $$ = MakeTempCopy (&$1);

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   AssignName_EM (&$3.tkToken, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1);                       // DO NOT FREE:  Passed on as result
/////////////////////////////////////////////FreeResult (&$3);                       // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN IndexListBR  NAMEVAR
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEVAR IndexListBR" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3);
/////////////////////////////////////////////FreeResult (&$4);                       // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrValu ASSIGN IndexListBR NAMEVAR
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEVAR IndexListBR" WS_UTF16_LEFTARROW L"ArrValu");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $4.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                             // Copy as temporary var in case it's named
                                             $$ = MakeTempCopy (&$1);

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   ArrayIndexSet_EM (&$4.tkToken, &$3.tkToken, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1);                       // DO NOT FREE:  Passed on as result
                                             FreeResult (&$3);
/////////////////////////////////////////////FreeResult (&$4);                       // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | ArrValu ASSIGN error        NAMEVAR
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEVAR error" WS_UTF16_LEFTARROW L"ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1);
/////////////////////////////////////////////FreeResult (&$4);                       // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | error   ASSIGN IndexListBR  NAMEUNK
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEUNK IndexListBR" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3);
/////////////////////////////////////////////FreeResult (&$4);                       // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrValu ASSIGN IndexListBR  NAMEUNK
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEUNK IndexListBR" WS_UTF16_LEFTARROW L"ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (!(CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR))
                                                 PrimFnValueError_EM (&$4.tkToken APPEND_NAME_ARG);

                                             FreeResult (&$1);
                                             FreeResult (&$3);
/////////////////////////////////////////////FreeResult (&$4);                       // Validation only
                                             YYERROR3
                                         } // End IF
                                        }
    | ArrValu ASSIGN error      NAMEUNK {DbgMsgWP (L"%%SimpExpr:  NAMEUNK error" WS_UTF16_LEFTARROW L"ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1);
/////////////////////////////////////////////FreeResult (&$4);                       // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | error   ASSIGN   ')' SelectSpec '('
                                        {DbgMsgWP (L"%%SimpExpr:  (SelectSpec)" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Mark as no longer doing Selective Specification
                                             lpplLocalVars->bSelSpec = FALSE;

                                             FreeResult (&$4);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrValu ASSIGN   ')' error    '(' {DbgMsgWP (L"%%SimpExpr:  (error)" WS_UTF16_LEFTARROW L"ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Mark as no longer doing Selective Specification
                                             lpplLocalVars->bSelSpec = FALSE;

                                             FreeResult (&$1);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrValu ASSIGN   ')' SelectSpec '('
                                        {DbgMsgWP (L"%%SimpExpr:  (SelectSpec)" WS_UTF16_LEFTARROW L"ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Copy as temporary var in case it's named
                                             $$ = MakeTempCopy (&$1);

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                             // If it's Selective Specification, ...
                                             if (lpplLocalVars->bSelSpec && !lpplLocalVars->bIniSpec)
                                             {
                                                 // Mark as no longer doing Selective Specification
                                                 lpplLocalVars->bSelSpec = FALSE;

                                                 // Remove Selective Specification until I figure it out
                                                 #if FALSE
                                                     lpplLocalVars->bRet =
                                                       ArrayIndexSet_EM (&lpplLocalVars->tkSelSpec, &$4.tkToken, &$1.tkToken);
                                                 #else
                                                     lpplLocalVars->bRet = FALSE;
                                                     ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                                                                               &$3.tkToken);
                                                 #endif
                                             } else
                                             {
                                                 // Mark as no longer doing Selective Specification
                                                 lpplLocalVars->bSelSpec =
                                                 lpplLocalVars->bIniSpec = FALSE;

                                                 lpplLocalVars->bRet =
                                                   AssignNamedVars_EM (&$4.tkToken, &$1.tkToken);
                                             } // End IF/ELSE/...

/////////////////////////////////////////////FreeResult (&$1);                       // DO NOT FREE:  Passed on as result
                                             FreeResult (&$4);

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN   ')' IndexListBR NAMEVAR '('
                                        {DbgMsgWP (L"%%SimpExpr:  (NAMEVAR IndexListBR)" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Mark as no longer doing Selective Specification
                                             lpplLocalVars->bSelSpec = FALSE;

                                             FreeResult (&$4);
/////////////////////////////////////////////FreeResult (&$5);                       // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrValu ASSIGN   ')' IndexListBR NAMEVAR '('
                                        {DbgMsgWP (L"%%SimpExpr:  (NAMEVAR IndexListBR)" WS_UTF16_LEFTARROW L"ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Copy as temporary var in case it's named
                                             $$ = MakeTempCopy (&$1);

                                             // Mark as no longer doing Selective Specification
                                             lpplLocalVars->bSelSpec = FALSE;

                                             $5.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   ArrayIndexSet_EM (&$5.tkToken, &$4.tkToken, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1);                       // DO NOT FREE:  Passed on as result
                                             FreeResult (&$4);
/////////////////////////////////////////////FreeResult (&$5);                       // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | ArrValu ASSIGN   ')' error NAMEVAR '('
                                        {DbgMsgWP (L"%%SimpExpr:  (NAMEVAR error)" WS_UTF16_LEFTARROW L"ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Mark as no longer doing Selective Specification
                                             lpplLocalVars->bSelSpec = FALSE;

                                             FreeResult (&$1);
/////////////////////////////////////////////FreeResult (&$5);                       // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | error   ASSIGN   ')' IndexListBR NAMEUNK '('
                                        {DbgMsgWP (L"%%SimpExpr:  (NAMEUNK IndexListBR)" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Mark as no longer doing Selective Specification
                                             lpplLocalVars->bSelSpec = FALSE;

                                             FreeResult (&$4);
/////////////////////////////////////////////FreeResult (&$5);                       // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrValu ASSIGN   ')' IndexListBR NAMEUNK '('
                                        {DbgMsgWP (L"%%SimpExpr:  (NAMEUNK IndexListBR)" WS_UTF16_LEFTARROW L"ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Mark as no longer doing Selective Specification
                                             lpplLocalVars->bSelSpec = FALSE;

                                             if (!(CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR))
                                                 PrimFnValueError_EM (&$5.tkToken APPEND_NAME_ARG);

                                             FreeResult (&$1);
                                             FreeResult (&$4);
/////////////////////////////////////////////FreeResult (&$5);                       // Validation only
                                             YYERROR3
                                         } // End IF
                                        }
    | ArrValu ASSIGN   ')' error       NAMEUNK '('
                                        {DbgMsgWP (L"%%SimpExpr:  (NAMEUNK error)" WS_UTF16_LEFTARROW L"ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Mark as no longer doing Selective Specification
                                             lpplLocalVars->bSelSpec = FALSE;

                                             FreeResult (&$1);
/////////////////////////////////////////////FreeResult (&$5);                       // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | error   ASSIGN LeftOper  NAMEVAR  {DbgMsgWP (L"%%SimpExpr:  NAMEVAR LeftOper" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResNNU (&$3);
/////////////////////////////////////////////FreeResult (&$4);                       // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
////| ArrValu ASSIGN error     NAMEVAR  //--Conflicts
    | ArrValu ASSIGN LeftOper  NAMEVAR  {DbgMsgWP (L"%%SimpExpr:  NAMEVAR LeftOper" WS_UTF16_LEFTARROW L"ArrValu");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $4.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
/////////////////////////////////////////////////FreeResult (&$4);                   // Validation only
                                                 YYERROR3
                                             } // End IF

                                             // Copy as temporary var in case it's named
                                             $$ = MakeTempCopy (&$1);

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (&$4.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken, FALSE, FALSE);
                                             YYFreeArray (lpplLocalVars->lpYYFcn);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
/////////////////////////////////////////////////FreeResult (&$4);                   // Validation only
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->bRet =
                                               AssignName_EM (&$4.tkToken, &lpplLocalVars->lpYYRes->tkToken);
                                             FreeResult (lpplLocalVars->lpYYRes); YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
/////////////////////////////////////////////FreeResult (&$4);                       // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN AxisFunc  NAMEVAR  {DbgMsgWP (L"%%SimpExpr:  NAMEVAR AxisFunc" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3);
/////////////////////////////////////////////FreeResult (&$4);                       // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
////| ArrValu ASSIGN error     NAMEVAR  //--Conflicts
    | ArrValu ASSIGN AxisFunc  NAMEVAR  {DbgMsgWP (L"%%SimpExpr:  NAMEVAR AxisFunc" WS_UTF16_LEFTARROW L"ArrValu");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $4.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                             // Copy as temporary var in case it's named
                                             $$ = MakeTempCopy (&$1);

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
/////////////////////////////////////////////////FreeResult (&$4);                   // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (&$4.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken, FALSE, FALSE);
                                             YYFreeArray (lpplLocalVars->lpYYFcn);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
/////////////////////////////////////////////////FreeResult (&$4);                   // Validation only
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->bRet =
                                               AssignName_EM (&$4.tkToken, &lpplLocalVars->lpYYRes->tkToken);
                                             FreeYYFcn1 (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
/////////////////////////////////////////////FreeResult (&$4);                       // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN LeftOper ')' NameVals '('
                                        {DbgMsgWP (L"%%SimpExpr:  (NameVals) LeftOper" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResNNU (&$3);
                                             FreeResult (&$5);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrValu ASSIGN LeftOper ')' error '('
                                        {DbgMsgWP (L"%%SimpExpr:  (error) LeftOper" WS_UTF16_LEFTARROW L"ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResNNU (&$1);
                                             FreeResult (&$3);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrValu ASSIGN error    ')' NameVals '('
                                        {DbgMsgWP (L"%%SimpExpr:  (NameVals) error" WS_UTF16_LEFTARROW L"ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1);
                                             FreeResult (&$5);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrValu ASSIGN LeftOper ')' NameVals '('
                                        {DbgMsgWP (L"%%SimpExpr:  (NameVals) LeftOper" WS_UTF16_LEFTARROW L"ArrValu");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYStrN =
                                               MakeNameStrand_EM_YY (&$5);
/////////////////////////////////////////////FreeResult (&$5);                       // DO NOT FREE:  RefCnt not incremented by MakeNameStrand_EM_YY

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);

                                             // Copy as temporary var in case it's named
                                             $$ = MakeTempCopy (&$1);

                                             if (!lpplLocalVars->lpYYStrN            // If not defined, free args and YYERROR
                                              || !lpplLocalVars->lpYYFcn)
                                             {
                                                 if (lpplLocalVars->lpYYStrN)        // If defined, free it
                                                 {
                                                     FreeYYFcn1 (lpplLocalVars->lpYYStrN); lpplLocalVars->lpYYStrN = NULL;
                                                 } // End IF

                                                 if (lpplLocalVars->lpYYFcn)         // If defined, free it
                                                 {
                                                     YYFreeArray (lpplLocalVars->lpYYFcn);
                                                     FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 } // End IF

                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   ModifyAssignNameVals_EM (&lpplLocalVars->lpYYStrN->tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1);                       // DO NOT FREE:  Passed on as result
                                             YYFreeArray (lpplLocalVars->lpYYFcn);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn);  lpplLocalVars->lpYYFcn = NULL;
                                             FreeYYFcn1 (lpplLocalVars->lpYYStrN); lpplLocalVars->lpYYStrN = NULL;

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN AxisFunc ')' NameVals '('
                                        {DbgMsgWP (L"%%SimpExpr:  (NameVals) AxisFunc" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3);
                                             FreeResult (&$5);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrValu ASSIGN AxisFunc ')' error '('
                                        {DbgMsgWP (L"%%SimpExpr:  (error) AxisFunc" WS_UTF16_LEFTARROW L"ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1);
                                             FreeResult (&$3);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
////| ArrValu ASSIGN error    ')' NameVals '(' //--Conflicts
    | ArrValu ASSIGN AxisFunc ')' NameVals '('
                                        {DbgMsgWP (L"%%SimpExpr:  (NameVals) AxisFunc" WS_UTF16_LEFTARROW L"ArrValu");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Copy as temporary var in case it's named
                                             $$ = MakeTempCopy (&$1);

                                             lpplLocalVars->lpYYStrN =
                                               MakeNameStrand_EM_YY (&$5);
/////////////////////////////////////////////FreeResult (&$5);                       // DO NOT FREE:  RefCnt not incremented by MakeNameStrand_EM_YY

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYStrN            // If not defined, free args and YYERROR
                                              || !lpplLocalVars->lpYYFcn)
                                             {
                                                 if (lpplLocalVars->lpYYStrN)        // If defined, free it
                                                 {
                                                     FreeYYFcn1 (lpplLocalVars->lpYYStrN); lpplLocalVars->lpYYStrN = NULL;
                                                 } // End IF

                                                 if (lpplLocalVars->lpYYFcn)         // If defined, free it
                                                 {
                                                     YYFreeArray (lpplLocalVars->lpYYFcn);
                                                     FreeYYFcn1 (lpplLocalVars->lpYYFcn);  lpplLocalVars->lpYYFcn = NULL;
                                                 } // End IF

                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   ModifyAssignNameVals_EM (&lpplLocalVars->lpYYStrN->tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1);                       // DO NOT FREE:  Passed on as result
                                             YYFreeArray (lpplLocalVars->lpYYFcn);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn);  lpplLocalVars->lpYYFcn = NULL;
                                             FreeYYFcn1 (lpplLocalVars->lpYYStrN); lpplLocalVars->lpYYStrN = NULL;

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN LeftOper IndexListBR NAMEVAR
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEVAR IndexListBR LeftOper" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResNNU (&$3);
                                             FreeResult (&$4);
/////////////////////////////////////////////FreeResult (&$5);                       // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrValu ASSIGN error    IndexListBR NAMEVAR
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEVAR IndexListBR error" WS_UTF16_LEFTARROW L"ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1);
                                             FreeResult (&$4);
/////////////////////////////////////////////FreeResult (&$5);                       // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrValu ASSIGN LeftOper IndexListBR NAMEVAR
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEVAR IndexListBR LeftOper" WS_UTF16_LEFTARROW L"ArrValu");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $5.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                             // Copy as temporary var in case it's named
                                             $$ = MakeTempCopy (&$1);

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeResult (&$4);
/////////////////////////////////////////////////FreeResult (&$5);                   // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   ArrayIndexFcnSet_EM (&$5.tkToken, &$4.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1);                       // DO NOT FREE:  Passed on as result
                                             YYFreeArray (lpplLocalVars->lpYYFcn);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             FreeResult (&$4);
/////////////////////////////////////////////FreeResult (&$5);                       // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | ArrValu ASSIGN LeftOper error NAMEVAR
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEVAR error LeftOper" WS_UTF16_LEFTARROW L"ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1);
                                             FreeResNNU (&$3);
/////////////////////////////////////////////FreeResult (&$5);                       // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrValu ASSIGN LeftOper error NAMEUNK
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEUNK error LeftOper" WS_UTF16_LEFTARROW L"ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1);
                                             FreeResNNU (&$3);
/////////////////////////////////////////////FreeResult (&$5);                       // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | error   ASSIGN AxisFunc IndexListBR NAMEVAR
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEVAR IndexListBR AxisFunc" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3);
                                             FreeResult (&$4);
/////////////////////////////////////////////FreeResult (&$5);                       // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
////| ArrValu ASSIGN error    IndexListBR NAMEVAR //-- Conflicts:  redundant
    | ArrValu ASSIGN AxisFunc IndexListBR NAMEVAR
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEVAR IndexListBR AxisFunc" WS_UTF16_LEFTARROW L"ArrValu");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $5.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                             // Copy as temporary var in case it's named
                                             $$ = MakeTempCopy (&$1);

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeResult (&$4);
/////////////////////////////////////////////////FreeResult (&$5);                   // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   ArrayIndexFcnSet_EM (&$5.tkToken, &$4.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1);                       // DO NOT FREE:  Passed on as result
                                             YYFreeArray (lpplLocalVars->lpYYFcn);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             FreeResult (&$4);
/////////////////////////////////////////////FreeResult (&$5);                       // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | ArrValu ASSIGN AxisFunc error NAMEVAR
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEVAR error AxisFunc" WS_UTF16_LEFTARROW L"ArrValu");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1);
                                             FreeResult (&$3);
/////////////////////////////////////////////FreeResult (&$5);                       // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    ;

// Selective specification
SelectSpec:
      NameVars                          {DbgMsgWP (L"%%SelectSpec:  NameVars");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYStrN =
                                               MakeNameStrand_EM_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1);                       // DO NOT FREE:  RefCnt not incremented by MakeNameStrand_EM_YY

                                             if (!lpplLocalVars->lpYYStrN)           // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYStrN;
                                             YYFree (lpplLocalVars->lpYYStrN); lpplLocalVars->lpYYStrN = NULL;
                                         } // End IF
                                        }
////| SelectSpec error                  //--Conflicts
    | SelectSpec LeftOper               {DbgMsgWP (L"%%SelectSpec:  LeftOper SelectSpec");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             // Check on Selective Specification first (and only) name
                                             if (!CheckSelSpec_EM (lpplLocalVars, &$1.tkToken))
                                             {
                                                 YYFreeArray (lpplLocalVars->lpYYFcn);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken, FALSE, TRUE);
                                             YYFreeArray (lpplLocalVars->lpYYFcn);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
////| SelectSpec error                  //--Conflicts
    | SelectSpec Drv1Func               {DbgMsgWP (L"%%SelectSpec:  Drv1Func SelectSpec");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             // Check on Selective Specification first (and only) name
                                             if (!CheckSelSpec_EM (lpplLocalVars, &$1.tkToken))
                                             {
                                                 YYFreeArray (lpplLocalVars->lpYYFcn);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken, FALSE, TRUE);
                                             YYFreeArray (lpplLocalVars->lpYYFcn);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | SelectSpec Drv3Func               {DbgMsgWP (L"%%SelectSpec:  Drv3Func SelectSpec");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             // Check on Selective Specification first (and only) name
                                             if (!CheckSelSpec_EM (lpplLocalVars, &$1.tkToken))
                                             {
                                                 YYFreeArray (lpplLocalVars->lpYYFcn);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken, FALSE, TRUE);
                                             YYFreeArray (lpplLocalVars->lpYYFcn);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | SelectSpec AxisFunc               {DbgMsgWP (L"%%SelectSpec:  AxisFunc SelectSpec");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             // Check on Selective Specification first (and only) name
                                             if (!CheckSelSpec_EM (lpplLocalVars, &$1.tkToken))
                                             {
                                                 YYFreeArray (lpplLocalVars->lpYYFcn);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken, FALSE, TRUE);
                                             YYFreeArray (lpplLocalVars->lpYYFcn);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | SelectSpec LeftOper error         {DbgMsgWP (L"%%SelectSpec:  error LeftOper SelectSpec");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1);
                                             FreeResNNU (&$2);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
////| SelectSpec error    StrandInst    //--Conflicts
    | SelectSpec LeftOper StrandInst    {DbgMsgWP (L"%%SelectSpec:  StrandInst LeftOper SelectSpec");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             // Check on Selective Specification first (and only) name
                                             if (!CheckSelSpec_EM (lpplLocalVars, &$1.tkToken))
                                             {
                                                 YYFreeArray (lpplLocalVars->lpYYFcn);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 FreeResult (&$1);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (&$3.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken, TRUE, TRUE);
                                             YYFreeArray (lpplLocalVars->lpYYFcn);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | SelectSpec AxisFunc error         {DbgMsgWP (L"%%SelectSpec:  error AxisFunc SelectSpec");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1);
                                             FreeResult (&$2);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
////| SelectSpec error    StrandInst    //--Conflicts
    | SelectSpec AxisFunc StrandInst    {DbgMsgWP (L"%%SelectSpec:  StrandInst AxisFunc SelectSpec");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             // Check on Selective Specification first (and only) name
                                             if (!CheckSelSpec_EM (lpplLocalVars, &$1.tkToken))
                                             {
                                                 YYFreeArray (lpplLocalVars->lpYYFcn);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 FreeResult (&$1);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (&$3.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken, TRUE, TRUE);
                                             YYFreeArray (lpplLocalVars->lpYYFcn);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Named Variables Strand w/Value
// Skip Ctrl-Break checking here so the Name Strand processing isn't interrupted
NameVals:
      NAMEVAR                           {DbgMsgWP (L"%%NameVals:  NAMEVAR");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $1.tkToken.tkFlags.TknType = TKT_VARNAMED;
                                             InitNameStrand (&$1);

                                             lpplLocalVars->lpYYRes =
                                               PushNameStrand_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1);                       // Validation only

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | NAMEUNK                           {DbgMsgWP (L"%%NameVals:  NAMEUNK");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (!(CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR))
                                                 PrimFnValueError_EM (&$1.tkToken APPEND_NAME_ARG);
/////////////////////////////////////////////FreeResult (&$1);                       // Validation only
                                             YYERROR3
                                         } // End IF
                                        }
    | NameVals       NAMEVAR            {DbgMsgWP (L"%%NameVals:  NAMEVAR NameVals");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $2.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                             lpplLocalVars->lpYYRes =
                                               PushNameStrand_YY (&$2);
/////////////////////////////////////////////FreeResult (&$2);                       // Validation only

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | NameVals       NAMEUNK            {DbgMsgWP (L"%%NameVals:  NAMEUNK NameVals");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (!(CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR))
                                                 PrimFnValueError_EM (&$2.tkToken APPEND_NAME_ARG);
                                             FreeResult (&$1);
/////////////////////////////////////////////FreeResult (&$2);                       // Validation only
                                             YYERROR3
                                         } // End IF
                                        }
    |       ')' IndexListBR NAMEVAR '(' {DbgMsgWP (L"%%NameVals:  (NAMEVAR IndexListBR)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $3.tkToken.tkFlags.TknType = TKT_VARNAMED;




                                             if (!(CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR))
                                                 PrimFnNonceError_EM (&$2.tkToken APPEND_NAME_ARG);
                                             FreeResult (&$2);
/////////////////////////////////////////////FreeResult (&$3);                       // Validation only
                                             YYERROR3
                                         } // End IF
                                        }
    |       ')' error       NAMEVAR '(' {DbgMsgWP (L"%%NameVals:  (NAMEVAR error)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
/////////////////////////////////////////////FreeResult (&$3);                       // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    |       ')' IndexListBR NAMEUNK '('
                                        {DbgMsgWP (L"%%NameVals:  (NAMEUNK IndexListBR)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (!(CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR))
                                                 PrimFnValueError_EM (&$2.tkToken APPEND_NAME_ARG);
                                             FreeResult (&$2);
/////////////////////////////////////////////FreeResult (&$3);                       // Validation only
                                             YYERROR3
                                         } // End IF
                                        }
    |       ')' error       NAMEUNK '(' {DbgMsgWP (L"%%NameVals:  (NAMEUNK error)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
/////////////////////////////////////////////FreeResult (&$3);                       // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | NameVals ')' IndexListBR NAMEVAR '('
                                        {DbgMsgWP (L"%%NameVals:  (NAMEVAR IndexListBR) NameVals");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $4.tkToken.tkFlags.TknType = TKT_VARNAMED;




                                             if (!(CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR))
                                                 PrimFnNonceError_EM (&$3.tkToken APPEND_NAME_ARG);
                                             FreeResult (&$1);
                                             FreeResult (&$3);
/////////////////////////////////////////////FreeResult (&$4);                       // Validation only
                                             YYERROR3
                                         } // End IF
                                        }
    | NameVals ')' error    NAMEVAR '(' {DbgMsgWP (L"%%NameVals:  (NAMEVAR error) NameVals");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1);
/////////////////////////////////////////////FreeResult (&$4);                       // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | NameVals ')' IndexListBR NAMEUNK '('
                                        {DbgMsgWP (L"%%NameVals:  (NAMEUNK IndexListBR) NameVals");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (!(CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR))
                                                 PrimFnValueError_EM (&$4.tkToken APPEND_NAME_ARG);
                                             FreeResult (&$1);
                                             FreeResult (&$3);
/////////////////////////////////////////////FreeResult (&$4);                       // Validation only
                                             YYERROR3
                                         } // End IF
                                        }
    | NameVals ')' error    NAMEUNK '(' {DbgMsgWP (L"%%NameVals:  (NAMEUNK error) NameVals");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1);
/////////////////////////////////////////////FreeResult (&$4);                       // Validation only
                                             YYERROR3
                                         } // End IF
                                        }
    ;

// Named Variables Strand with or w/o Value
// Skip Ctrl-Break checking here so the Name Strand processing isn't interrupted
NameVars:
      NAMEVAR                           {DbgMsgWP (L"%%NameVars:  NAMEVAR");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $1.tkToken.tkFlags.TknType = TKT_VARNAMED;
                                             InitNameStrand (&$1);

                                             lpplLocalVars->lpYYRes =
                                               PushNameStrand_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1);                       // Validation only

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | NAMEUNK                           {DbgMsgWP (L"%%NameVars:  NAMEUNK");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             InitNameStrand (&$1);

                                             lpplLocalVars->lpYYRes =
                                               PushNameStrand_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1);                       // Validation only

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | ')' IndexListBR NAMEVAR '('       {DbgMsgWP (L"%%NameVars:  (NAMEVAR IndexListBR)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $3.tkToken.tkFlags.TknType = TKT_VARNAMED;
                                             if (!(CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR))
                                                 PrimFnNonceError_EM (&$3.tkToken APPEND_NAME_ARG);


                                             FreeResult (&$2);
/////////////////////////////////////////////FreeResult (&$3);                       // Validation only
                                             YYERROR3
                                         } // End IF
                                        }
    | ')' error       NAMEVAR '('       {DbgMsgWP (L"%%NameVars:  (NAMEVAR error)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
/////////////////////////////////////////////FreeResult (&$3);                       // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ')' IndexListBR NAMEUNK '('       {DbgMsgWP (L"%%NameVars:  (NAMEUNK IndexListBR)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (!(CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR))
                                                 PrimFnValueError_EM (&$3.tkToken APPEND_NAME_ARG);
                                             FreeResult (&$2);
/////////////////////////////////////////////FreeResult (&$3);                       // Validation only
                                             YYERROR3
                                         } // End IF
                                        }
    | ')' error       NAMEUNK '('       {DbgMsgWP (L"%%NameVars:  (NAMEUNK error)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
/////////////////////////////////////////////FreeResult (&$3);                       // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | NameVars       NAMEVAR            {DbgMsgWP (L"%%NameVars:  NameVars NAMEVAR");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $2.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                             lpplLocalVars->lpYYRes =
                                               PushNameStrand_YY (&$2);
/////////////////////////////////////////////FreeResult (&$2);                       // Validation only

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | NameVars       NAMEUNK            {DbgMsgWP (L"%%NameVars:  NameVars NAMEUNK");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                               PushNameStrand_YY (&$2);
/////////////////////////////////////////////FreeResult (&$2);                       // Validation only

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Derived left function expression, Type 1
//   valid in ArrExpr:      ArrValu Drv1Func
//            FcnSpec:      ...
//            ParenFunc:    (Drv1Func)
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
Drv1Func:
      RightOper DydOp error             {DbgMsgWP (L"%%Drv1Func:  error DydOp RightOper");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResNNU (&$1);
/////////////////////////////////////////////FreeResult (&$2);               // Don't free named fcn/opr on error
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | FILLJOT   DydOp StrandInst        {DbgMsgWP (L"%%Drv1Func:  StrandInst DydOp FILLJOT");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, DIRECT);    // Left operand (Direct)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYMak =
                                               MakeFillJot_YY (&$1);
                                             FreeResult (&$1);

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Right operand (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    | RightOper DydOp StrandInst        {DbgMsgWP (L"%%Drv1Func:  StrandInst DydOp RightOper");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResNNU (&$1);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, DIRECT);    // Left operand (Direct)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResNNU (&$1);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, INDIRECT);  // Right operand (Indirect)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    | FILLJOT   DydOp Drv1Func          {DbgMsgWP (L"%%Drv1Func:  Drv1Func DydOp FILLJOT");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYMak =
                                               MakeFillJot_YY (&$1);
                                             FreeResult (&$1);

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Right operand (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    | RightOper DydOp Drv1Func          {DbgMsgWP (L"%%Drv1Func:  Drv1Func DydOp RightOper");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResNNU (&$1);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResNNU (&$1);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, INDIRECT);  // Right operand (Indirect)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    |           MonOp error             {DbgMsgWP (L"%%Drv1Func:  error MonOp");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
/////////////////////////////////////////////FreeResult (&$1);               // Don't free named fcn/opr on error
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    |           MonOp StrandInst        {DbgMsgWP (L"%%Drv1Func:  StrandInst MonOp");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, DIRECT);    // Left operand (Direct)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    ;

// Derived left function expression, Type 2
//   valid in FcnSpec:      ...
//   but not ArrExpr: because of Strand on the right
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
Drv2Func:
      StrandInst DydOp error            {DbgMsgWP (L"%%Drv2Func:  error DydOp StrandInst");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1);
/////////////////////////////////////////////FreeResult (&$2);               // Don't free named fcn/opr on error
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | error      DydOp LeftOper         {DbgMsgWP (L"%%Drv2Func:  LeftOper DydOp error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
/////////////////////////////////////////////FreeResult (&$2);               // Don't free named fcn/opr on error
                                             FreeResNNU (&$3);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | StrandInst DydOp FILLJOT          {DbgMsgWP (L"%%Drv2Func:  FILLJOT DydOp StrandInst");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYMak =
                                               MakeFillJot_YY (&$3);
                                             FreeResult (&$3);

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Left operand (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, DIRECT);    // Right operand (Direct)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    | StrandInst DydOp LeftOper         {DbgMsgWP (L"%%Drv2Func:  LeftOper DydOp StrandInst");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeResNNU (&$3);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, DIRECT);    // Right operand (Direct)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    | error      DydOp AxisFunc         {DbgMsgWP (L"%%Drv2Func:  AxisFunc DydOp error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
/////////////////////////////////////////////FreeResult (&$2);               // Don't free named fcn/opr on error
                                             FreeResult (&$3);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | FILLJOT    DydOp AxisFunc         {DbgMsgWP (L"%%Drv2Func:  AxisFunc DydOp StrandInst");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYMak =
                                               MakeFillJot_YY (&$1);
                                             FreeResult (&$1);

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT);    // Right operand (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    | StrandInst DydOp AxisFunc         {DbgMsgWP (L"%%Drv2Func:  AxisFunc DydOp StrandInst");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, DIRECT);    // Right operand (Direct)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    ;

// Derived left function expression, Type 3
//   valid in FcnSpec:      ...
//            ArrExpr:      ArrValu Drv3Func
//   because the Strand on the right is bound to the DydOp)
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
Drv3Func:
      RhtOpVal   DydOp error            {DbgMsgWP (L"%%Drv3Func:  error DydOp RhtOpVal");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1);
/////////////////////////////////////////////FreeResult (&$2);               // Don't free named fcn/opr on error
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | RhtOpVal   DydOp FILLJOT          {DbgMsgWP (L"%%Drv3Func:  FILLJOT DydOp RhtOpVal");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYMak =
                                               MakeFillJot_YY (&$3);
                                             FreeResult (&$3);

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Left operand (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, DIRECT);    // Right operand (Direct)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    | RhtOpVal   DydOp LeftOper         {DbgMsgWP (L"%%Drv3Func:  LeftOper DydOp RhtOpVal");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeResNNU (&$3);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, DIRECT);    // Right operand (Direct)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    | RhtOpVal   DydOp AxisFunc         {DbgMsgWP (L"%%Drv3Func:  AxisFunc DydOp RhtOpVal");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, DIRECT);    // Right operand (Direct)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    | RhtOpVal   DydOp Drv3Func         {DbgMsgWP (L"%%Drv3Func:  Drv3Func DydOp RhtOpVal");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, DIRECT);    // Right operand (Direct)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    ;

// Derived left function expression, Type 4
//    valid in FcnSpec:     ...
//             ArrExpr:     ArrValu Drv4Func
//             LeftOper:    MonOp Drv4Func
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
Drv4Func:
////| StrandInst DydOp error            --See above
      RhtOpVal   DydOp Drv1Func         {DbgMsgWP (L"%%Drv4Func:  Drv1Func DydOp RhtOpVal");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, DIRECT);    // Right operand (Direct)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    | RhtOpVal   DydOp Drv4Func         {DbgMsgWP (L"%%Drv4Func:  Drv4Func DydOp RhtOpVal");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, DIRECT);    // Right operand (Direct)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    | RhtOpVal   DydOp StrandInst       {DbgMsgWP (L"%%Drv4Func:  StrandInst DydOp RhtOpVal");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, DIRECT);    // Left operand (Direct)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, DIRECT);    // Right operand (Direct)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    ;

// Right operand values
RhtOpVal:
      StrandOp2                         {DbgMsgWP (L"%%RhtOpVal:  StrandOp2");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Check for error
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_ERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    | '^' ArrValu '('                   {DbgMsgWP (L"%%RhtOpVal:  (ArrValu)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Check for error
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_ERROR)
                                                 YYERROR3
                                             $$ = $2;
                                         } // End IF

/////////////////////////////////////////////// Because there's no matching DecrRefCnt to the IncrRefCnt
///////////////////////////////////////////////   in MakeVarStrand_EM_YY, we mark this array as skipping
///////////////////////////////////////////////   the next IncrRefCnt.
/////////////////////////////////////////////SetVFOArraySRCIFlag (&$$.tkToken);
                                           // The above lines were removed so that +op2 (1,2,3) 'a' would work
                                        }
    ;

// Derived left function expression, Type 5
//    valid in ArrExpr:     ArrValu Drv5Func StrandInst
//             ParenFunc:   (Drv5Func)
Drv5Func:
      MonOp AmbOpX                      {DbgMsgWP (L"%%Drv5Func:  AmbOpX MonOp");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Change the first token in the function strand
                                             //   from ambiguous operator to a function
                                             AmbOpToFcn (&$2);

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Function (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    | Drv3Func                          {DbgMsgWP (L"%%Drv5Func:  Drv3Func");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Check for error
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_ERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    | MonOp Drv5Func                    {DbgMsgWP (L"%%Drv5Func:  Drv5Func MonOp");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Function (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    | AmbOpX Drv5Func                   {DbgMsgWP (L"%%Drv5Func:  Drv5Func AmbOpX");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Change the first token in the function strand
                                             //   from ambiguous operator to a monadic operator
                                             AmbOpToOp1 (&$1);

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Function (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    ;

// Derived left function expression, Type 6
//    valid in TrainFunc:  Drv6Func
//             FcnSpec:    Drv6Func ASSIGN NameAnyOpN
Drv6Func:
      MonOp  Drv1Func                   {DbgMsgWP (L"%%Drv6Func:  Drv1Func MonOp");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Function (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    | MonOp  Drv3Func                   {DbgMsgWP (L"%%Drv6Func:  Drv3Func MonOp");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Function (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    | AmbOpX Drv1Func                   {DbgMsgWP (L"%%Drv6Func:  Drv1Func AmbOpX");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Function (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    | AmbOpX Drv2Func                   {DbgMsgWP (L"%%Drv6Func:  Drv2Func AmbOpX");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Function (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    | AmbOpX Drv3Func                   {DbgMsgWP (L"%%Drv6Func:  Drv3Func AmbOpX");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Function (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    | AmbOpX Drv4Func                   {DbgMsgWP (L"%%Drv6Func:  Drv4Func AmbOpX");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Function (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    ;

// Parenthesized function expression
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
ParenFunc:
      '>' error            '('          {DbgMsgWP (L"%%ParenFunc:  (error)");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | '>' Drv1Func         '('          {DbgMsgWP (L"%%ParenFunc:  (Drv1Func)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $2;
                                        }
    | '>' Drv2Func         '('          {DbgMsgWP (L"%%ParenFunc:  (Drv2Func)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $2;
                                        }
    | '>' Drv4Func         '('          {DbgMsgWP (L"%%ParenFunc:  (Drv4Func)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $2;
                                        }
    | '>' Drv5Func         '('          {DbgMsgWP (L"%%ParenFunc:  (Drv5Func)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $2;
                                        }
    | '>' LeftOper         '('          {DbgMsgWP (L"%%ParenFunc:  (LeftOper)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $2;
                                        }
    | '>' AxisFunc         '('          {DbgMsgWP (L"%%ParenFunc:  (AxisFunc)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $2;
                                        }
    | '>' FcnSpec          '('          {DbgMsgWP (L"%%ParenFunc:  (FcnSpec)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (&$2, 1, DIRECT);    // Function (Direct)

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    | '>' Op1Spec error    '('          {DbgMsgWP (L"%%ParenFunc:  (error Op1Spec)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$2);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | '>' Op1Spec LeftOper '('          {DbgMsgWP (L"%%ParenFunc:  (LeftOper Op1Spec)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$2, 2, DIRECT);    // Monadic operator (Direct)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResNNU (&$3);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    ;

// Left monadic operand function
LeftMonOper:
      Drv1Func                          {DbgMsgWP (L"%%LeftMonOper:  Drv1Func");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Check for error
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_ERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    | Drv3Func                          {DbgMsgWP (L"%%LeftMonOper:  Drv3Func");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Check for error
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_ERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    | Drv4Func                          {DbgMsgWP (L"%%LeftMonOper:  Drv4Func");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Check for error
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_ERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    | MonOp LeftMonOper                 {DbgMsgWP (L"%%LeftMonOper:  LeftMonOper MonOp");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResNNU (&$2);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    | AmbOpX Drv3Func                   {DbgMsgWP (L"%%LeftMonOper:  Drv3Func AmbOpX");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Change the first token in the function strand
                                             //   from ambiguous operator to a monadic operator
                                             AmbOpToOp1 (&$1);

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResNNU (&$2);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    ;

AfoFcn:
      AFOFCN                            {DbgMsgWP (L"%%AfoFcn:  AFOFCN");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Check for error
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_ERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    | RBRACE_FCN_FN12 StmtMult LBRACE   {DbgMsgWP (L"%%AfoFcn:  { StmtMult }");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Gather together the tokens between the braces into an AFO
                                             lpplLocalVars->lpYYFcn =
                                               MakeAfo_EM_YY (&$3, &$1, lpplLocalVars);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    ;

// Left operand function
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
LeftOper:
          PRIMFCN  SYSNS                {DbgMsgWP (L"%%LeftOper:  SYSNS PRIMFCN");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimFcn_YY (&$1);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Copy the system namespace level
                                             $$.tkToken.tkFlags.SysNSLvl = $2.tkToken.tkFlags.SysNSLvl;
                                         } // End IF
                                        }
        | PRIMFCN                       {DbgMsgWP (L"%%LeftOper:  PRIMFCN");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimFcn_YY (&$1);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    |     NAMEFCN  SYSNS                {DbgMsgWP (L"%%LeftOper:  SYSNS NAMEFCN");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, TRUE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResNNU (&$1);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Copy the system namespace level
                                             $$.tkToken.tkFlags.SysNSLvl = $2.tkToken.tkFlags.SysNSLvl;
                                         } // End IF
                                        }
    |     AfoFcn                        {DbgMsgWP (L"%%LeftOper:  AfoFcn");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (&$1, 1, DIRECT);    // Function (Direct)

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResNNU (&$1);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    |     NAMEFCN                       {DbgMsgWP (L"%%LeftOper:  NAMEFCN");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, TRUE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResNNU (&$1);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    |     NAMETRN                       {DbgMsgWP (L"%%LeftOper:  NAMETRN");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, TRUE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    |     SYSFN12  SYSNS                {DbgMsgWP (L"%%LeftOper:  SYSNS SYSFN12");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, FALSE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Copy the system namespace level
                                             $$.tkToken.tkFlags.SysNSLvl = $2.tkToken.tkFlags.SysNSLvl;
                                         } // End IF
                                        }
    |     SYSFN12                       {DbgMsgWP (L"%%LeftOper:  SYSFN12");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, FALSE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    |     ParenFunc                     {DbgMsgWP (L"%%LeftOper:  ParenFunc");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Check for error
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_ERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    |     RightOper    JOTDOT           {DbgMsgWP (L"%%LeftOper:  " WS_UTF16_JOT L". RightOper");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$2, 2, DIRECT);    // Monadic operator (Direct)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResNNU (&$1);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, INDIRECT);  // Right operand (Indirect)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
////|                  MonOp error      //--Conflicts
    |                  MonOp FILLJOT    {DbgMsgWP (L"%%LeftOper:  FILLJOT MonOp");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYMak =
                                               MakeFillJot_YY (&$2);
                                             FreeResult (&$2);

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Left operand (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    |                  MonOp Drv2Func   {DbgMsgWP (L"%%LeftOper:  Drv2Func MonOp");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    |                  MonOp LeftOper   {DbgMsgWP (L"%%LeftOper:  LeftOper MonOp");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResNNU (&$2);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
////|                  MonOp error      //--Conflicts
    |                  MonOp AxisFunc   {DbgMsgWP (L"%%LeftOper:  AxisFunc MonOp");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
////|                  AmbOpX error     //--Conflicts
    |                  AmbOpX LeftOper  {DbgMsgWP (L"%%LeftOper:  LeftOper AmbOpX"); //***FIXME***
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Change the first token in the function strand
                                             //   from ambiguous operator to a monadic operator
                                             AmbOpToOp1 (&$1);

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResNNU (&$2);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
////|     RightOper DydOp error         //--Conflicts
    |     RightOper DydOp FILLJOT       {DbgMsgWP (L"%%LeftOper:  FILLJOT DydOp RightOper");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResNNU (&$1);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYMak =
                                               MakeFillJot_YY (&$3);
                                             FreeResult (&$3);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResNNU (&$1);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT);     // Left operand (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResNNU (&$1);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, INDIRECT);  // Right operand (Indirect)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    |     FILLJOT   DydOp LeftOper      {DbgMsgWP (L"%%LeftOper:  LeftOper DydOp FILLJOT");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeResNNU (&$3);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYMak =
                                               MakeFillJot_YY (&$1);
                                             FreeResult (&$1);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Right operand (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    |     FILLJOT   DydOp FILLJOT       {DbgMsgWP (L"%%LeftOper:  FILLJOT DydOp FILLJOT");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYMak =
                                               MakeFillJot_YY (&$3);
                                             FreeResult (&$3);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Left operand (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             lpplLocalVars->lpYYMak =
                                               MakeFillJot_YY (&$1);
                                             FreeResult (&$1);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Right operand (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    |     RightOper DydOp LeftOper      {DbgMsgWP (L"%%LeftOper:  LeftOper DydOp RightOper");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResNNU (&$1);
                                                 FreeResNNU (&$3);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResNNU (&$1);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, INDIRECT);  // Right operand (Indirect)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
////|     RightOper DydOp error         //--Conflicts
    |     RightOper DydOp AxisFunc      {DbgMsgWP (L"%%LeftOper:  AxisFunc DydOp RightOper");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResNNU (&$1);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResNNU (&$1);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, INDIRECT);  // Right operand (Indirect)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    |     RightOper DydOp Drv3Func      {DbgMsgWP (L"%%LeftOper:  Drv3Func DydOp RightOper");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResNNU (&$1);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResNNU (&$1);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, INDIRECT);  // Right operand (Indirect)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    | '%' Train '('                     {DbgMsgWP (L"%%LeftOper:  (Train)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Initialize the function strand (Train) base
                                             $2.lpYYStrandBase = $2.lpYYFcnBase;

                                             lpplLocalVars->lpYYLft =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_TRN, TRUE);

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYStrL =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYLft, 1, DIRECT); // Lefthand function (Direct)
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYStrL;
                                             YYFree (lpplLocalVars->lpYYStrL); lpplLocalVars->lpYYStrL = NULL;

                                             // Because there's no matching DecrRefCnt to the IncrRefCnt
                                             //   in MakeFcnStrand_EM_YY, we mark this array as skipping
                                             //   the next IncrRefCnt.
                                             SetVFOArraySRCIFlag (&$$.tkToken);
#ifdef DEBUG
                                             // Display the strand stack
                                             DisplayStrand (STRAND_FCN);
#endif
                                         } // End IF
                                        }
    ;

TrainFuncL:
      LeftOper                          {DbgMsgWP (L"%%TrainFuncL:  LeftOper");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Check for error
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_ERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    | AxisFunc                          {DbgMsgWP (L"%%TrainFuncL:  AxisFunc");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Check for error
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_ERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    ;

TrainFuncB:
      TrainFuncL                        {DbgMsgWP (L"%%TrainFuncB:  TrainFuncL");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Check for error
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_ERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    | Drv1Func                          {DbgMsgWP (L"%%TrainFuncB:  Drv1Func");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Check for error
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_ERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    | Drv6Func                          {DbgMsgWP (L"%%TrainFuncB:  Drv6Func");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Check for error
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_ERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    ;

Train:
      TrainFuncL error                  {DbgMsgWP (L"%%Train:  error TrainFuncL");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Initialize the function strand (Train) base
                                             $1.lpYYStrandBase = $1.lpYYFcnBase;

/////////////////////////////////////////////FreeResult (&$1);  // DO NOT FREE as we haven't incremented (MakeFcnStrand) as yet
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | TrainFuncB TrainFuncL             {DbgMsgWP (L"%%Train:  TrainFuncL TrainFuncB");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Initialize the function strand (Train) base
                                             $2.lpYYStrandBase = $2.lpYYFcnBase;

                                             lpplLocalVars->lpYYLft =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, TRUE);

                                             // Initialize the function strand (Train) base
                                             $1.lpYYStrandBase = $1.lpYYFcnBase;

                                             lpplLocalVars->lpYYRht =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);

                                             lpplLocalVars->lpYYMak =
                                               MakeTrainOp_YY (&$1);

                                             if (!lpplLocalVars->lpYYLft             // If not defined, free args and YYERROR
                                              || !lpplLocalVars->lpYYRht
                                              || !lpplLocalVars->lpYYMak)
                                             {
                                                 FreeResult (&$1);
                                                 FreeResult (&$2);

                                                 if (lpplLocalVars->lpYYLft)
                                                 {
                                                     YYFreeArray (lpplLocalVars->lpYYLft);
                                                     FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 } // End IF

                                                 if (lpplLocalVars->lpYYRht)
                                                 {
                                                     YYFreeArray (lpplLocalVars->lpYYRht);
                                                     FreeYYFcn1 (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                                 } // End IF

                                                 if (lpplLocalVars->lpYYMak)
                                                 {
                                                     FreeYYFcn1 (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;
                                                 } // End IF

                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 3, DIRECT); // Monadic operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             lpplLocalVars->lpYYStrR =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYRht, 1, DIRECT); // Righthand function (Direct)
                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;

                                             lpplLocalVars->lpYYStrL =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYLft, 1, DIRECT); // Lefthand function (Direct)
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;

                                             // Because there's no matching DecrRefCnt to the IncrRefCnt
                                             //   in MakeFcnStrand_EM_YY, we mark this array as skipping
                                             //   the next IncrRefCnt.
                                             SetVFOArraySRCIFlag (&lpplLocalVars->lpYYStrL->tkToken);

                                             // Because there's no matching DecrRefCnt to the IncrRefCnt
                                             //   in MakeFcnStrand_EM_YY, we mark this array as skipping
                                             //   the next IncrRefCnt.
                                             SetVFOArraySRCIFlag (&lpplLocalVars->lpYYStrR->tkToken);

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             YYFree (lpplLocalVars->lpYYStrL); lpplLocalVars->lpYYStrL = NULL;
                                             YYFree (lpplLocalVars->lpYYStrR); lpplLocalVars->lpYYStrR = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1 ); lpplLocalVars->lpYYOp1  = NULL;
#ifdef DEBUG
                                             // Display the strand stack
                                             DisplayStrand (STRAND_FCN);
#endif
                                         } // End IF
                                        }
    | Train    error                    {DbgMsgWP (L"%%Train:  error Train");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Initialize the function strand (Train) base
                                             $1.lpYYStrandBase = $1.lpYYFcnBase;

                                             FreeResult (&$1);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | Train    TrainFuncL               {DbgMsgWP (L"%%Train:  TrainFuncL Train");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Increment the token count
                                             $1.TknCount++; $1.lpYYFcnBase->TknCount++;

                                             // Initialize the function strand (Train) base
                                             $2.lpYYStrandBase = $2.lpYYFcnBase;

                                             lpplLocalVars->lpYYLft =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, TRUE);

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 // Initialize the function strand (Train) base
                                                 $1.lpYYStrandBase = $1.lpYYFcnBase;

                                                 lpplLocalVars->lpYYRht =
                                                   MakeFcnStrand_EM_YY (&$1, NAMETYPE_TRN, TRUE);

                                                 if (lpplLocalVars->lpYYRht)
                                                 {
                                                     YYFreeArray (lpplLocalVars->lpYYRht);
                                                     FreeYYFcn1 (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                                 } // End IF

                                                 FreeResult (&$1);
                                                 FreeResNNU (&$2);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYStrL =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYLft, 1, DIRECT); // Lefthand function (Direct)
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;

                                             if (!lpplLocalVars->lpYYStrL)           // If not defined, free args and YYERROR
                                             {
                                                 // Initialize the function strand (Train) base
                                                 $1.lpYYStrandBase = $1.lpYYFcnBase;

                                                 lpplLocalVars->lpYYRht =
                                                   MakeFcnStrand_EM_YY (&$1, NAMETYPE_TRN, TRUE);

                                                 if (lpplLocalVars->lpYYRht)
                                                 {
                                                     YYFreeArray (lpplLocalVars->lpYYRht);
                                                     FreeYYFcn1 (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                                 } // End IF

                                                 FreeResult (&$1);
                                                 FreeResNNU (&$2);
                                                 YYERROR3
                                             } // End IF

                                             // Because there's no matching DecrRefCnt to the IncrRefCnt
                                             //   in MakeFcnStrand_EM_YY, we mark this array as skipping
                                             //   the next IncrRefCnt.
                                             SetVFOArraySRCIFlag (&lpplLocalVars->lpYYStrL->tkToken);

                                             YYFree (lpplLocalVars->lpYYStrL); lpplLocalVars->lpYYStrL = NULL;
#ifdef DEBUG
                                             // Display the strand stack
                                             DisplayStrand (STRAND_FCN);
#endif
                                             // The result is always the root of the function tree
                                             $$ = $1;
                                         } // End IF
                                        }
    ;

// Right operand function (short right scope)
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
RightOper:
          PRIMFCN SYSNS                 {DbgMsgWP (L"%%RightOper:  SYSNS PRIMFCN");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimFcn_YY (&$1);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Copy the system namespace level
                                             $$.tkToken.tkFlags.SysNSLvl = $2.tkToken.tkFlags.SysNSLvl;
                                         } // End IF
                                        }
        | PRIMFCN                       {DbgMsgWP (L"%%RightOper:  PRIMFCN");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimFcn_YY (&$1);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    |     NAMEFCN SYSNS                 {DbgMsgWP (L"%%RightOper:  SYSNS NAMEFCN");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, TRUE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResNNU (&$1);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Copy the system namespace level
                                             $$.tkToken.tkFlags.SysNSLvl = $2.tkToken.tkFlags.SysNSLvl;
                                         } // End IF
                                        }
    |     NAMEFCN                       {DbgMsgWP (L"%%RightOper:  NAMEFCN");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, TRUE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResNNU (&$1);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    |     NAMETRN                       {DbgMsgWP (L"%%RightOper:  NAMETRN");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, TRUE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    |     SYSFN12 SYSNS                 {DbgMsgWP (L"%%RightOper:  SYSNS SYSFN12");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, FALSE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Copy the system namespace level
                                             $$.tkToken.tkFlags.SysNSLvl = $2.tkToken.tkFlags.SysNSLvl;
                                         } // End IF
                                        }
    |     SYSFN12                       {DbgMsgWP (L"%%RightOper:  SYSFN12");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, FALSE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    |     ParenFunc                     {DbgMsgWP (L"%%RightOper:  ParenFunc");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    ;

// Axis function expression
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
AxisFunc:
      '}' error   '['  PRIMFCN SYSNS    {DbgMsgWP (L"%%AxisFunc:  SYSNS PRIMFCN[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$4);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | '}' error   '['  PRIMFCN          {DbgMsgWP (L"%%AxisFunc:  PRIMFCN[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$4);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | '}' ArrValu '['  PRIMFCN SYSNS    {DbgMsgWP (L"%%AxisFunc:  SYSNS PRIMFCN[ArrValu]");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimFcn_YY (&$4);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 2, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;

                                             lpplLocalVars->lpYYMak =
                                               MakeAxis_YY (&$2);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYAxis =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Axis value (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYAxis)           // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYAxis); lpplLocalVars->lpYYAxis = NULL;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Copy the system namespace level
                                             $$.tkToken.tkFlags.SysNSLvl = $5.tkToken.tkFlags.SysNSLvl;
                                         } // End IF
                                        }
    | '}' ArrValu '['  PRIMFCN          {DbgMsgWP (L"%%AxisFunc:  PRIMFCN[ArrValu]");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimFcn_YY (&$4);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 2, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;

                                             lpplLocalVars->lpYYMak =
                                               MakeAxis_YY (&$2);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYAxis =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Axis value (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYAxis)           // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYAxis); lpplLocalVars->lpYYAxis = NULL;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    | '}' error   '['  NAMEFCN SYSNS    {DbgMsgWP (L"%%AxisFunc:  SYSNS NAMEFCN[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
/////////////////////////////////////////////FreeResult (&$4);                       // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | '}' error   '['  NAMEFCN          {DbgMsgWP (L"%%AxisFunc:  NAMEFCN[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
/////////////////////////////////////////////FreeResult (&$4);                       // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | '}' ArrValu '['  NAMEFCN SYSNS    {DbgMsgWP (L"%%AxisFunc:  SYSNS NAMEFCN[ArrValu]");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$4, TRUE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 2, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;

                                             lpplLocalVars->lpYYMak =
                                               MakeAxis_YY (&$2);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYAxis =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Axis value (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYAxis)           // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYAxis); lpplLocalVars->lpYYAxis = NULL;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Copy the system namespace level
                                             $$.tkToken.tkFlags.SysNSLvl = $5.tkToken.tkFlags.SysNSLvl;
                                         } // End IF
                                        }
    | '}' ArrValu '['  NAMEFCN          {DbgMsgWP (L"%%AxisFunc:  NAMEFCN[ArrValu]");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$4, TRUE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 2, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;

                                             lpplLocalVars->lpYYMak =
                                               MakeAxis_YY (&$2);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYAxis =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Axis value (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYAxis)           // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYAxis); lpplLocalVars->lpYYAxis = NULL;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    | '}' error   '['  SYSFN12 SYSNS    {DbgMsgWP (L"%%AxisFunc:  SYSNS SYSFN12[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
/////////////////////////////////////////////FreeResult (&$4);                       // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | '}' error   '['  SYSFN12          {DbgMsgWP (L"%%AxisFunc:  SYSFN12[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
/////////////////////////////////////////////FreeResult (&$4);                       // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | '}' ArrValu '['  SYSFN12 SYSNS    {DbgMsgWP (L"%%AxisFunc:  SYSNS SYSFN12[ArrValu]");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$4, FALSE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 2, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;

                                             lpplLocalVars->lpYYMak =
                                               MakeAxis_YY (&$2);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYAxis =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Axis value (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYAxis)           // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYAxis); lpplLocalVars->lpYYAxis = NULL;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Copy the system namespace level
                                             $$.tkToken.tkFlags.SysNSLvl = $5.tkToken.tkFlags.SysNSLvl;
                                         } // End IF
                                        }
    | '}' ArrValu '['  SYSFN12          {DbgMsgWP (L"%%AxisFunc:  SYSFN12[ArrValu]");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$4, FALSE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 2, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;

                                             lpplLocalVars->lpYYMak =
                                               MakeAxis_YY (&$2);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYAxis =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Axis value (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYAxis)           // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYAxis); lpplLocalVars->lpYYAxis = NULL;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    | '}' error   '['  ParenFunc        {DbgMsgWP (L"%%AxisFunc:  ParenFunc[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$4);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | '}' ArrValu '['  ParenFunc        {DbgMsgWP (L"%%AxisFunc:  ParenFunc[ArrValu]");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (&$4, 2, INDIRECT);  // Function (Indirect)

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;

                                             lpplLocalVars->lpYYMak =
                                               MakeAxis_YY (&$2);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYAxis =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Axis value (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYAxis)           // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYAxis); lpplLocalVars->lpYYAxis = NULL;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    ;

// Ambiguous operator
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
AmbOp:
          OP3                           {DbgMsgWP (L"%%AmbOp:  OP3");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimOp3_YY (&$1);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Ambiguous operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp3;
                                             YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                         } // End IF
                                        }
    |     NAMEOP3                       {DbgMsgWP (L"%%AmbOp:  NAMEOP3");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, TRUE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Ambiguous operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp3;
                                             YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                         } // End IF
                                        }
////|     error                         //--Conflicts
    | '>' AmbOpX '('                    {DbgMsgWP (L"%%AmbOp:  (AmbOpX)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $2;
                                        }
    | '>' Op3Spec '('                   {DbgMsgWP (L"%%AmbOp:  (Op3Spec)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                               PushFcnStrand_YY (&$2, 1, DIRECT);    // Ambiguous operator (Direct)

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Ambiguous operator w/axis
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
AmbOpAxis:
      '}' error   '['  AmbOp            {DbgMsgWP (L"%%AmbOpAxis:  AmbOp[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$4);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | '}' ArrValu '['  AmbOp            {DbgMsgWP (L"%%AmbOpAxis:  AmbOp[ArrValu]");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (&$4, 2, INDIRECT);  // Ambiguous operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp3;

                                             lpplLocalVars->lpYYMak =
                                               MakeAxis_YY (&$2);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYAxis =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Axis value (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYAxis)           // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYAxis); lpplLocalVars->lpYYAxis = NULL;
                                             YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                         } // End IF
                                        }
    ;

// Ambiguous operator with or w/o axis
AmbOpX:
      AmbOp                             {DbgMsgWP (L"%%AmbX:  AmbOp");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | AmbOpAxis                         {DbgMsgWP (L"%%AmbX:  AmbOpAxis");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    ;

// Anonymous monadic operator
AfoOp1:
      AFOOP1                            {DbgMsgWP (L"%%AfoOp1:  AFOOP1");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Check for error
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_ERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    | RBRACE_OP1_FN12 StmtMult LBRACE   {DbgMsgWP (L"%%AfoOp1:  { StmtMult }");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Gather together the tokens between the braces into an anonymous monadic operator
                                             lpplLocalVars->lpYYOp1 =
                                               MakeAfo_EM_YY (&$3, &$1, lpplLocalVars);

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    ;

// Monadic operator
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
MonOp:
                       OP1              {DbgMsgWP (L"%%MonOp:  OP1");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimOp1_YY (&$1);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Monadic operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    |                  AfoOp1           {DbgMsgWP (L"%%MonOp:  AfoOp1");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 1, DIRECT);    // Monadic operator (Direct)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    |                  NAMEOP1          {DbgMsgWP (L"%%MonOp:  NAMEOP1");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, TRUE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Monadic operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
////|              error                //--Conflicts
    |              MonOpAxis            {DbgMsgWP (L"%%MonOp:  MonOpAxis");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Check for error
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_ERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    |              '>' MonOp '('        {DbgMsgWP (L"%%MonOp:  (MonOp)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Check for error
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_ERROR)
                                                 YYERROR3
                                             $$ = $2;
                                         } // End IF
                                        }
    |              '>' Op1Spec '('      {DbgMsgWP (L"%%MonOp:  (Op1Spec)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$2, 1, DIRECT);    // Monadic operator (Direct)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    ;

// Monadic operator w/axis
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
MonOpAxis:
      '}' error   '['  MonOp            {DbgMsgWP (L"%%MonOpAxis:  MonOp[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$4);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | '}' ArrValu '['  MonOp            {DbgMsgWP (L"%%MonOpAxis:  MonOp[ArrValu]");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$4, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYMak =
                                               MakeAxis_YY (&$2);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRes =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Axis value (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    ;

// Anonymous dyadic operator
AfoOp2:
      AFOOP2                            {DbgMsgWP (L"%%AfoOp2:  AFOOP2");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Check for error
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_ERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    | RBRACE_OP2_FN12 StmtMult LBRACE   {DbgMsgWP (L"%%AfoOp2:  { StmtMult }");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Gather together the tokens between the braces into an anonymous dyadic operator
                                             lpplLocalVars->lpYYOp2 =
                                               MakeAfo_EM_YY (&$3, &$1, lpplLocalVars);

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    ;

// Dyadic operator
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
DydOp:
                       OP2              {DbgMsgWP (L"%%DydOp:  OP2");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimOp2_YY (&$1);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Dyadic operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    |                  AfoOp2           {DbgMsgWP (L"%%DydOp:  AfoOp2");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$1, 1, DIRECT);    // Dyadic operator (Direct)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    |                  NAMEOP2          {DbgMsgWP (L"%%DydOp:  NAMEOP2");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, TRUE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Dyadic operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
////|                  error            //--Conflicts
    |                  DydOpAxis        {DbgMsgWP (L"%%DydOp:  DydOpAxis");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Check for error
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_ERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    |              '#' DydOp '('        {DbgMsgWP (L"%%DydOp:  (DydOp)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Check for error
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_ERROR)
                                                 YYERROR3
                                             $$ = $2;
                                         } // End IF
                                        }
    |              '#' Op2Spec '('      {DbgMsgWP (L"%%DydOp:  (Op2Spec)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 1, DIRECT);    // Dyadic operator (Direct)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    ;

// Dyadic operator w/axis
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
DydOpAxis:
      '?' error   '['  DydOp            {DbgMsgWP (L"%%DydOpAxis:  DydOp[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$4);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | '?' ArrValu '['  DydOp            {DbgMsgWP (L"%%DydOpAxis:  DydOp[ArrValu]");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$4, 2, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYMak =
                                               MakeAxis_YY (&$2);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRes =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Axis value (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    ;

// Index list w/brackets, allowing for A[A][A]...
IndexListBR:
                  ']'             '['   {DbgMsgWP (L"%%IndexListBR:  []");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYLst =
                                               InitList0_YY (&$1);
                                             if (!lpplLocalVars->lpYYLst)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYRes =
                                               MakeList_EM_YY (lpplLocalVars->lpYYLst, TRUE);
                                             YYFree (lpplLocalVars->lpYYLst); lpplLocalVars->lpYYLst = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |             ']' error       '['   {DbgMsgWP (L"%%IndexListBR:  [error]");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    |             ']' IndexListWE '['   {DbgMsgWP (L"%%IndexListBR:  [IndexListWE]");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYLst =
                                               MakeList_EM_YY (&$2, TRUE);
                                             FreeResult (&$2);

                                             if (!lpplLocalVars->lpYYLst)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYLst;
                                             YYFree (lpplLocalVars->lpYYLst); lpplLocalVars->lpYYLst = NULL;
                                         } // End IF
                                        }
    | IndexListBR ']'             '['   {DbgMsgWP (L"%%IndexListBR:  [] IndexListBR ");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Check for error
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_ERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    | IndexListBR ']' error       '['   {DbgMsgWP (L"%%IndexListBR:  [error] IndexListBR ");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | IndexListBR ']' IndexListWE '['   {DbgMsgWP (L"%%IndexListBR:  [IndexListWE] IndexListBR ");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYLst =
                                               MakeList_EM_YY (&$3, TRUE);
                                             FreeResult (&$3);

                                             if (!lpplLocalVars->lpYYLst)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ListIndexRef_EM_YY (&lpplLocalVars->lpYYLst->tkToken, &$1.tkToken);
                                             FreeResult (&$1);
                                             FreeYYFcn1 (lpplLocalVars->lpYYLst); lpplLocalVars->lpYYLst = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Index list, with empties (meaning no ArrValu between semicolons)
// Skip Ctrl-Break checking here so the List processing isn't interrupted
IndexListWE:
      IndexListWE1                      {DbgMsgWP (L"%%IndexListWE:  IndexListWE1");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Check for error
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_ERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    | IndexListWE2                      {DbgMsgWP (L"%%IndexListWE:  IndexListWE2");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Check for error
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_ERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    ;

// Index list, with empties, starting with a semicolon
IndexListWE1:
                   ';'                  {DbgMsgWP (L"%%IndexListWE1:  ;");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Initialize the list with an empty item
                                             lpplLocalVars->lpYYLst =
                                               InitList1_YY (NULL, &$1);

                                             if (!lpplLocalVars->lpYYLst)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // Push an empty item onto the list
                                             lpplLocalVars->lpYYRes =
                                               PushList_YY (lpplLocalVars->lpYYLst, NULL, &$1);
                                             FreeYYFcn1 (lpplLocalVars->lpYYLst); lpplLocalVars->lpYYLst = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |              ';' error            {DbgMsgWP (L"%%IndexListWE1:  error;");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    |              ';' ArrValu          {DbgMsgWP (L"%%IndexListWE1:  ArrValu;");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Initialize the list with an empty item
                                             lpplLocalVars->lpYYLst =
                                               InitList1_YY (NULL, &$2);

                                             if (!lpplLocalVars->lpYYLst)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2);
                                                 YYERROR3
                                             } // End IF

                                             // Push an item onto the list
                                             lpplLocalVars->lpYYRes =
                                               PushList_YY (lpplLocalVars->lpYYLst, &$2, &$2);
/////////////////////////////////////////////FreeResult (&$2);                       // Copied w/o IncrRefCnt in PushList_YY
                                             FreeYYFcn1 (lpplLocalVars->lpYYLst); lpplLocalVars->lpYYLst = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | IndexListWE1 ';'                  {DbgMsgWP (L"%%IndexListWE1:  ;IndexListWE1");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Push an empty item onto the list
                                             lpplLocalVars->lpYYRes =
                                               PushList_YY (&$1, NULL, &$2);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | IndexListWE1 ';' error            {DbgMsgWP (L"%%IndexListWE1:  error;IndexListWE1");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | IndexListWE1 ';' ArrValu          {DbgMsgWP (L"%%IndexListWE1:  ArrValu;IndexListWE1");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                               PushList_YY (&$1, &$3, &$3);
/////////////////////////////////////////////FreeResult (&$3);                       // Copied w/o IncrRefCnt in PushList_YY

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Index list, with empties, starting with an array expr
IndexListWE2:
                       ArrValu          {DbgMsgWP (L"%%IndexListWE2:  ArrValu");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Initialize the list with the arg
                                             lpplLocalVars->lpYYRes =
                                               InitList1_YY (&$1, &$1);
/////////////////////////////////////////////FreeResult (&$1);                       // Copied w/o IncrRefCnt in PushList_YY

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | IndexListWE2 ';'                  {DbgMsgWP (L"%%IndexListWE2:  ;IndexListWE2");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Push an empty item onto the list
                                             lpplLocalVars->lpYYRes =
                                               PushList_YY (&$1, NULL, &$2);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 YYERROR3
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | IndexListWE2 ';' error            {DbgMsgWP (L"%%IndexListWE2:  error;IndexListWE2");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | IndexListWE2 ';' ArrValu          {DbgMsgWP (L"%%IndexListWE2:  ArrValu;IndexListWE2");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Push an item onto the list
                                             lpplLocalVars->lpYYRes =
                                               PushList_YY (&$1, &$3, &$3);
/////////////////////////////////////////////FreeResult (&$3);                       // Copied w/o IncrRefCnt in PushList_YY

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1);
                                                 FreeResult (&$3);
                                                 YYERROR3
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

%%
#undef  APPEND_NAME

//***************************************************************************
//  Start of C program
//***************************************************************************


//***************************************************************************
//  $ParseLine
//
//  Parse a line
//  The result of parsing the line is in the return value (plLocalVars.ExitType)
//    as well as in lpMemPTD->YYResExec if there is a value (EXITTYPE_DISPLAY
//    or EXITTYPE_NODISPLAY).
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ParseLine"
#else
#define APPEND_NAME
#endif

EXIT_TYPES ParseLine
    (HWND           hWndSM,                 // Session Manager window handle
     LPTOKEN_HEADER lpMemTknHdr,            // Ptr to tokenized line global memory header
     HGLOBAL        hGlbTxtLine,            // Text of tokenized line global memory handle
     LPWCHAR        lpwszLine,              // Ptr to the line text (may be NULL)
     LPPERTABDATA   lpMemPTD,               // Ptr to PerTabData global memory
     UINT           uLineNum,               // Function line #
     UINT           uTknNum,                // Starting token # in the above function line
     HGLOBAL        hGlbDfnHdr,             // User-defined function/operator global memory handle (NULL = execute/immexec)
     UBOOL          bActOnErrors,           // TRUE iff errors are acted upon
     UBOOL          bExec1Stmt,             // TRUE iff executing only one stmt
     UBOOL          bNoDepthCheck)          // TRUE iff we're to skip the depth check

{
    PLLOCALVARS   plLocalVars = {0};        // ParseLine local vars
    LPPLLOCALVARS oldTlsPlLocalVars;        // Ptr to previous value of dwTlsPlLocalVars
    UINT          oldTlsType,               // Previous value of dwTlsType
                  uRet;                     // The result from pl_yyparse
    ERROR_CODES   uError = ERRORCODE_NONE;  // Error code
    UBOOL         bOldExecuting;            // Old value of bExecuting
    HWND          hWndEC;                   // Edit Ctrl window handle
    LPSIS_HEADER  lpSISCur,                 // Ptr to current SI Stack Header
                  lpSISPrv;                 // Ptr to previous ...
    LPDFN_HEADER  lpMemDfnHdr;              // Ptr to user-defined function/operator header ...

    // Save the previous value of dwTlsType
    oldTlsType = PtrToUlong (TlsGetValue (dwTlsType));

    // Save the thread type ('PL')
    TlsSetValue (dwTlsType, TLSTYPE_PL);

    // Save the previous value of dwTlsPlLocalVars
    oldTlsPlLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Save ptr to ParseLine local vars
    TlsSetValue (dwTlsPlLocalVars, (LPVOID) &plLocalVars);

    // Save ptr to PerTabData global memory
    TlsSetValue (dwTlsPerTabData, (LPVOID) lpMemPTD);

    DBGENTER;           // Must be placed after the TlsSetValue for lpMemPTD

    // Get the Edit Ctrl window handle
    (HANDLE_PTR) hWndEC = GetWindowLongPtrW (hWndSM, GWLSF_HWNDEC);

    // Indicate that we're executing
    bOldExecuting = lpMemPTD->bExecuting; SetExecuting (lpMemPTD, TRUE);

    EnterCriticalSection (&CSOPL);

    // Increment the depth counter
    lpMemPTD->uExecDepth++;

    // Link this plLocalVars into the chain of such objects
    plLocalVars.lpPLPrev = lpMemPTD->lpPLCur;

    // If there's a previous ptr, transfer its Ctrl-Break flag
    if (lpMemPTD->lpPLCur)
        plLocalVars.bCtrlBreak = lpMemPTD->lpPLCur->bCtrlBreak;

    // Save as new current ptr
    lpMemPTD->lpPLCur = &plLocalVars;

    LeaveCriticalSection (&CSOPL);

    // Initialize the error code
    uError = ERRORCODE_NONE;

    // If we don't have a valid ptr, ...
    if (!lpMemTknHdr)
    {
        plLocalVars.ExitType = EXITTYPE_ERROR;

        goto NORMAL_EXIT;
    } // End IF
#ifdef DEBUG
    // Display the tokens so far
    DisplayTokens (lpMemTknHdr);
#endif

    // If there's a UDFO global memory handle, ...
    if (hGlbDfnHdr)
    {
        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLock (hGlbDfnHdr);

        // Save value in LocalVars
        plLocalVars.bAFO          = lpMemDfnHdr->bAFO;
        plLocalVars.bAfoCtrlStruc = lpMemDfnHdr->bAfoCtrlStruc;
        plLocalVars.bMFO          = lpMemDfnHdr->bMFO;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
    } // End IF

    // Save values in the LocalVars
    plLocalVars.lpMemPTD       = lpMemPTD;
    plLocalVars.hWndSM         = hWndSM;
    plLocalVars.hGlbTxtLine    = hGlbTxtLine;
    plLocalVars.lpMemTknHdr    = lpMemTknHdr;
    plLocalVars.lpwszLine      = lpwszLine;
    plLocalVars.bLookAhead     = FALSE;
    plLocalVars.ExitType       = EXITTYPE_NONE;
    plLocalVars.uLineNum       = uLineNum;
    plLocalVars.hGlbDfnHdr     = hGlbDfnHdr;
    plLocalVars.bExec1Stmt     = bExec1Stmt;

    // Get # tokens in the line
    plLocalVars.uTokenCnt = plLocalVars.lpMemTknHdr->TokenCnt;

    // If the starting token # is outside the token count, ...
    if (uTknNum >= plLocalVars.uTokenCnt)
    {
        // Set the exit type to exit normally
        plLocalVars.ExitType = EXITTYPE_GOTO_ZILDE;

        goto NORMAL_EXIT;
    } // End IF

    // Skip over TOKEN_HEADER
    plLocalVars.lptkStart = TokenBaseToStart (plLocalVars.lpMemTknHdr);
    plLocalVars.lptkEnd   = &plLocalVars.lptkStart[plLocalVars.uTokenCnt];

    Assert (plLocalVars.lptkStart->tkFlags.TknType EQ TKT_EOL
         || plLocalVars.lptkStart->tkFlags.TknType EQ TKT_EOS);

    // If we're not starting at the first token, ...
    if (uTknNum)
    {
        // Skip to the starting token
        plLocalVars.lptkNext  = &plLocalVars.lptkStart[uTknNum];

        // If this token is an EOS/EOL, skip to the end of the stmt
        //   and start executing there
        if (plLocalVars.lptkNext->tkFlags.TknType EQ TKT_EOS
         || plLocalVars.lptkNext->tkFlags.TknType EQ TKT_EOL)
            plLocalVars.lptkNext = &plLocalVars.lptkNext[plLocalVars.lptkNext->tkData.tkIndex - 1];
    } else
        // Skip to end of 1st stmt
        plLocalVars.lptkNext  = &plLocalVars.lptkStart[plLocalVars.lptkStart->tkData.tkIndex];

    // Start off with no error
    plLocalVars.tkErrorCharIndex =
    plLocalVars.tkLACharIndex    = NEG1U;

    __try
    {
        __try
        {
            // Initialize the strand starts
            if (oldTlsPlLocalVars)
            {
                plLocalVars.lpYYStrArrStart[STRAND_VAR] = oldTlsPlLocalVars->lpYYStrArrNext[STRAND_VAR];
                plLocalVars.lpYYStrArrStart[STRAND_FCN] = oldTlsPlLocalVars->lpYYStrArrNext[STRAND_FCN];
                plLocalVars.lpYYStrArrStart[STRAND_LST] = oldTlsPlLocalVars->lpYYStrArrNext[STRAND_LST];
                plLocalVars.lpYYStrArrStart[STRAND_NAM] = oldTlsPlLocalVars->lpYYStrArrNext[STRAND_NAM];
            } else
            {
                plLocalVars.lpYYStrArrStart[STRAND_VAR] = lpMemPTD->lpStrand[STRAND_VAR];
                plLocalVars.lpYYStrArrStart[STRAND_FCN] = lpMemPTD->lpStrand[STRAND_FCN];
                plLocalVars.lpYYStrArrStart[STRAND_LST] = lpMemPTD->lpStrand[STRAND_LST];
                plLocalVars.lpYYStrArrStart[STRAND_NAM] = lpMemPTD->lpStrand[STRAND_NAM];
            } // End IF/ELSE

            // Initialize the base & next strand ptrs
            plLocalVars.lpYYStrArrStart[STRAND_VAR]->lpYYStrandBase =
            plLocalVars.lpYYStrArrBase [STRAND_VAR]                 =
            plLocalVars.lpYYStrArrNext [STRAND_VAR]                 = plLocalVars.lpYYStrArrStart[STRAND_VAR];
            plLocalVars.lpYYStrArrStart[STRAND_FCN]->lpYYStrandBase =
            plLocalVars.lpYYStrArrBase [STRAND_FCN]                 =
            plLocalVars.lpYYStrArrNext [STRAND_FCN]                 = plLocalVars.lpYYStrArrStart[STRAND_FCN];
            plLocalVars.lpYYStrArrStart[STRAND_LST]->lpYYStrandBase =
            plLocalVars.lpYYStrArrBase [STRAND_LST]                 =
            plLocalVars.lpYYStrArrNext [STRAND_LST]                 = plLocalVars.lpYYStrArrStart[STRAND_LST];
            plLocalVars.lpYYStrArrStart[STRAND_NAM]->lpYYStrandBase =
            plLocalVars.lpYYStrArrBase [STRAND_NAM]                 =
            plLocalVars.lpYYStrArrNext [STRAND_NAM]                 = plLocalVars.lpYYStrArrStart[STRAND_NAM];

#if YYDEBUG
            // Enable debugging
            yydebug = TRUE;
#endif

            // Parse the line, check for errors
            //   0 = success
            //   1 = YYABORT or APL error
            //   2 = memory exhausted
            uRet = pl_yyparse (&plLocalVars);
        } __except (CheckException (GetExceptionInformation (), L"ParseLine"))
        {
            // Pass the exception on
            RaiseException (MyGetExceptionCode (),
                            0,
                            0,
                            NULL);
        } // End __try/__except
    } __except (CheckException (GetExceptionInformation (), L"ParseLine"))
    {
        // Split cases based upon the ExceptionCode
        switch (MyGetExceptionCode ())
        {
            case EXCEPTION_STACK_OVERFLOW:
                // Set the exit type
                plLocalVars.ExitType = EXITTYPE_STACK_FULL;

                break;

            case EXCEPTION_WS_FULL:
                // Set the exit type
                plLocalVars.ExitType = EXITTYPE_ERROR;

                // Mark as in error
                uRet = 1;
                uError = ERRORCODE_ELX;
                ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                           NULL);
                break;

            case EXCEPTION_LIMIT_ERROR:
                // Set the exit type
                plLocalVars.ExitType = EXITTYPE_ERROR;

                // Mark as in error
                uRet = 1;
                uError = ERRORCODE_ELX;
                ErrorMessageIndirectToken (ERRMSG_LIMIT_ERROR APPEND_NAME,
                                           NULL);
                break;

            default:
                // Display message for unhandled exception
                DisplayException ();

                // Mark as in error
                uRet = 1;
                uError = ERRORCODE_ELX;

                goto NORMAL_EXIT;
        } // End SWITCH
    } // End __try/__except

#if YYDEBUG
    // Disable debugging
    yydebug = FALSE;
#endif

    if (!plLocalVars.bCtrlBreak
     && !plLocalVars.bLookAhead)
    // Split cases based upon the exit type
    switch (plLocalVars.ExitType)
    {
        case EXITTYPE_QUADERROR_INIT:
            // Set the reset flag
            lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_QUADERROR_INIT;

            break;

        case EXITTYPE_RESET_ALL:
            // Set the reset flag
            lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_ALL;

            break;

        case EXITTYPE_RESET_ONE:
            // Set the reset flag
            lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_ONE;

            break;

        case EXITTYPE_RESET_ONE_INIT:
            // Get a ptr to the current SIS header
            lpSISCur = lpMemPTD->lpSISCur;

            // Peel back to the first non-Exec layer
            while (lpSISCur->DfnType EQ DFNTYPE_EXEC)
               lpSISCur = lpSISCur->lpSISPrv;

            if (!lpSISCur->ItsEC)
                // Set the reset flag
                lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_ONE_INIT;

            break;

        case EXITTYPE_QUADERROR_EXEC:
            // Set the exit type
            plLocalVars.ExitType = EXITTYPE_ERROR;

            // Set the reset flag
            lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_NONE;

            // Fall through to common code

        case EXITTYPE_ERROR:        // Mark user-defined function/operator as suspended
        case EXITTYPE_STACK_FULL:   // ...
        case EXITTYPE_STOP:         // ...
            // If it's STACK FULL, ...
            if (plLocalVars.ExitType EQ EXITTYPE_STACK_FULL)
            {
                // Mark as in error
                uRet = 1;
                uError = ERRORCODE_NONE;
                ErrorMessageIndirectToken (ERRMSG_STACK_FULL APPEND_NAME,
                                           NULL);
                // Set the reset flag
                lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_STOP;
            } // End IF

            // Get a ptr to the current SIS header
            lpSISCur = lpMemPTD->lpSISCur;

            // If it's a permanent function (i.e. Magic Function/Operator),
            //   or an AFO,
            // but not STACK FULL, ...
            if ((lpSISCur->PermFn
              || plLocalVars.bAFO)
             && plLocalVars.ExitType NE EXITTYPE_STACK_FULL)
            {
                // Don't suspend at this level

                // Set the exit type
                plLocalVars.ExitType = EXITTYPE_QUADERROR_INIT;

                // Set the reset flag
                lpSISCur->ResetFlag  = RESETFLAG_QUADERROR_INIT;

                break;
            } // End IF

            // If this level or an adjacent preceding level is from
            //   Execute, immediate execution mode, or an AFO,
            //   peel back to the preceding level
            while (lpSISCur
                && (lpSISCur->DfnType EQ DFNTYPE_EXEC
                 || lpSISCur->DfnType EQ DFNTYPE_IMM
                 || lpSISCur->bAFO))
                lpSISCur = lpSISCur->lpSISPrv;

            // If this level is a user-defined function/operator,
            //   mark it as suspended
            if (lpSISCur
             && (lpSISCur->DfnType EQ DFNTYPE_OP1
              || lpSISCur->DfnType EQ DFNTYPE_OP2
              || lpSISCur->DfnType EQ DFNTYPE_FCN))
                lpSISCur->Suspended = TRUE;
            break;

        case EXITTYPE_DISPLAY:      // Nothing more to do with these types
        case EXITTYPE_NODISPLAY:    // ...
        case EXITTYPE_NOVALUE:      // ...
        case EXITTYPE_GOTO_ZILDE:   // ...
        case EXITTYPE_GOTO_LINE:    // ...
        case EXITTYPE_NONE:         // ...
            // Check on user-defined function/operator exit error
            if (CheckDfnExitError_EM (lpMemPTD))
            {
                // Mark as an APL error
                plLocalVars.ExitType = EXITTYPE_ERROR;
                uRet = 1;
            } // End IF

            break;

        case EXITTYPE_RETURNxLX:
            uRet = 0;

            break;

        defstop
            break;
    } // End IF/SWITCH

    // If Ctrl-Break was pressed, ...
    if (plLocalVars.bCtrlBreak)
    {
        // Mark as in error
        uError = ERRORCODE_ALX;

        // Reset the flag
        plLocalVars.bCtrlBreak = FALSE;

        // Peel back to first user-defined function/operator
        for (lpSISCur = lpMemPTD->lpSISCur;
             lpSISCur && lpSISCur NE lpMemPTD->lpSISNxt;
             lpSISCur = lpSISCur->lpSISPrv)
        if (!lpSISCur->PermFn
         && !lpSISCur->bAFO
         && (lpSISCur->DfnType EQ DFNTYPE_OP1
          || lpSISCur->DfnType EQ DFNTYPE_OP2
          || lpSISCur->DfnType EQ DFNTYPE_FCN))
            break;

        // Signal an error
        BreakMessage (hWndSM,
                      (lpSISCur && lpSISCur NE lpMemPTD->lpSISNxt) ? lpSISCur
                                                                   : NULL);
    } // End IF

    if (uRet EQ 0 || uError EQ ERRORCODE_ALX)
        goto NORMAL_EXIT;

    // If we're not resetting, ...
    if (lpMemPTD->lpSISCur->ResetFlag EQ RESETFLAG_NONE)
    {
        // If called from Immediate Execution/Execute, ...
        if (lpwszLine)
            // Create []DM & []EM
            ErrorMessageDirect (lpMemPTD->lpwszErrorMessage,    // Ptr to error message text
                                lpwszLine,                      // Ptr to the line which generated the error
                                plLocalVars.tkErrorCharIndex);  // Position of caret (origin-0)
        else
        {
            LPMEMTXT_UNION lpMemTxtLine;

            // Lock the memory to get a ptr to it
            lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

            // Create []DM & []EM
            ErrorMessageDirect (lpMemPTD->lpwszErrorMessage,    // Ptr to error message text
                               &lpMemTxtLine->C,                // Ptr to the line which generated the error
                                plLocalVars.tkErrorCharIndex);  // Position of caret (origin-0)
            // We no longer need this ptr
            MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;
        } // End IF/ELSE

        // If not already set, ...
        if (uError EQ ERRORCODE_NONE)
            // Mark as in error
            uError = ERRORCODE_ELX;
    } // End IF
NORMAL_EXIT:
    EnterCriticalSection (&CSOPL);

    // Unlink this plLocalVars from the chain of such objects
    lpMemPTD->lpPLCur = plLocalVars.lpPLPrev;

    // Transfer the Ctrl-Break flag in case it hasn't been acted upon
    if (lpMemPTD->lpPLCur)
        lpMemPTD->lpPLCur->bCtrlBreak = plLocalVars.bCtrlBreak;

    LeaveCriticalSection (&CSOPL);

    // Restore the previous value of dwTlsType
    TlsSetValue (dwTlsType, ULongToPtr (oldTlsType));

    // If there's an error to be signalled, ...
    if (uError NE ERRORCODE_NONE
     && bActOnErrors
     && lpMemPTD->lpSISCur->lpSISErrCtrl EQ NULL)
    {
        EXIT_TYPES exitType;        // Return code from PrimFnMonUpTackJotCSPLParse

#ifdef DEBUG
        // Split cases based upon the error code
        switch (uError)
        {
            case ERRORCODE_ALX:
                DbgMsgW2 (L"~~Start []ALX on");

                break;

            case ERRORCODE_ELX:
                DbgMsgW2 (L"~~Start []ELX on");

                break;

            case ERRORCODE_DM:
                DbgMsgW2 (L"~~Display []DM on");

                break;

            defstop
                break;
        } // End SWITCH

        if (lpwszLine)
        {
            DbgMsgW2 (lpwszLine);
        } else
        {
            LPMEMTXT_UNION lpMemTxtLine;

            // Lock the memory to get a ptr to it
            lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

            // Display the function line
            DbgMsgW2 (&lpMemTxtLine->C);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;
        } // End IF/ELSE
#endif
        // Set the text for the line
        // Split cases based upon the error code
        switch (uError)
        {
            case ERRORCODE_ALX:
                lpwszLine = WS_UTF16_UPTACKJOT $QUAD_ALX;
                bNoDepthCheck = FALSE;

                break;

            case ERRORCODE_ELX:
                lpwszLine = WS_UTF16_UPTACKJOT $QUAD_ELX;
                bNoDepthCheck = FALSE;

                break;

            case ERRORCODE_DM:
                lpwszLine = $QUAD_DM;
                bNoDepthCheck = TRUE;

                break;

            defstop
                break;
        } // End SWITCH

        // Split cases based upon the error code
        switch (uError)
        {
            case ERRORCODE_ALX:
            case ERRORCODE_ELX:
                __try
                {
                    // Execute the statement
                    exitType =
                      PrimFnMonUpTackJotCSPLParse (hWndEC,          // Edit Ctrl window handle
                                                   lpMemPTD,        // Ptr to PerTabData global memory
                                                   lpwszLine,       // Ptr to text of line to execute
                                                   lstrlenW (lpwszLine), // Length of the line to execute
                                                   TRUE,            // TRUE iff we should act on errors
                                                   bNoDepthCheck,   // TRUE iff we're to skip the depth check
                                                   NULL);           // Ptr to function token
                } __except (CheckException (GetExceptionInformation (), L"ParseLine"))
                {
                    // Split cases based upon the ExceptionCode
                    switch (MyGetExceptionCode ())
                    {
                        case EXCEPTION_STACK_OVERFLOW:
                            // Set the error message
                            ErrorMessageIndirectToken (ERRMSG_STACK_FULL APPEND_NAME,
                                                       NULL);
                            break;

                        case EXCEPTION_LIMIT_ERROR:
                            // Set the error message
                            ErrorMessageIndirectToken (ERRMSG_LIMIT_ERROR APPEND_NAME,
                                                       NULL);
                            break;

                        default:
                            // Display message for unhandled exception
                            DisplayException ();

                            // Mark as in error
                            uError = ERRORCODE_DM;

                            break;
                    } // End SWITCH

                    // Create []DM & []EM
                    ErrorMessageDirect (lpMemPTD->lpwszErrorMessage,    // Ptr to error message text
                                        lpwszLine,                      // Ptr to the line which generated the error
                                        plLocalVars.tkErrorCharIndex);  // Position of caret (origin-0)
                    // Set the exit type
                    plLocalVars.ExitType = EXITTYPE_STACK_FULL;

                    // Set the reset flag
                    lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_STOP;

                    // Mark as in error
                    uError = ERRORCODE_DM;

                    goto DISPLAYDM_EXIT;
                } // End __try/__except

                break;

            case ERRORCODE_DM:
DISPLAYDM_EXIT:
                // Display []DM
                DisplayGlbArr_EM (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_DM]->stData.stGlbData,   // Global memory handle to display
                                  TRUE,                                                         // TRUE iff last line has CR
                                 &plLocalVars.bCtrlBreak,                                       // Ptr to Ctrl-Break flag
                                  NULL);                                                        // Ptr to function token
                // Return code as already displayed
                exitType = EXITTYPE_NODISPLAY;

                break;

            defstop
                break;
        } // End SWITCH

        // Split cases based upon the error code
        switch (uError)
        {
            case ERRORCODE_ALX:
                // If we exited normally, ...
                if (exitType EQ EXITTYPE_DISPLAY
                 || exitType EQ EXITTYPE_NODISPLAY
                 || exitType EQ EXITTYPE_NOVALUE)
                {
                    // Stop execution so we can display the break message
                    plLocalVars.ExitType =
                    exitType             = EXITTYPE_STOP;

                    // Set the reset flag
                    lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_STOP;

                    // If the Execute/Quad result is present, clear it
                    if (lpMemPTD->YYResExec.tkToken.tkFlags.TknType)
                    {
                        // Free the result
                        FreeResult (&lpMemPTD->YYResExec);

                        // We no longer need these values
                        ZeroMemory (&lpMemPTD->YYResExec, sizeof (lpMemPTD->YYResExec));
                    } // End IF
                } // End IF

                break;

            case ERRORCODE_ELX:
            case ERRORCODE_DM:
                break;

            defstop
                break;
        } // End SWITCH

        // Split cases based upon the exit type
        switch (exitType)
        {
            case EXITTYPE_GOTO_LINE:
            case EXITTYPE_RESET_ALL:
            case EXITTYPE_RESET_ONE:
            case EXITTYPE_RESET_ONE_INIT:
            case EXITTYPE_QUADERROR_INIT:
                // Pass on to caller
                plLocalVars.ExitType = exitType;

                break;

            case EXITTYPE_NODISPLAY:    // Display the result (if any)
            case EXITTYPE_DISPLAY:      // ...
                // If the Execute/Quad result is present, display it
                if (lpMemPTD->YYResExec.tkToken.tkFlags.TknType)
                {
                    HGLOBAL hGlbDfnHdr;                 // AFO DfnHdr global memory handle

                    // If it's not from executing []ALX,
                    //   and we're parsing an AFO, ...
                    if (uError NE ERRORCODE_ALX
                     && (hGlbDfnHdr = SISAfo (lpMemPTD)))
                        AfoDisplay_EM (&lpMemPTD->YYResExec.tkToken, FALSE, &plLocalVars, hGlbDfnHdr);
                    else
                        // Display the array
                        ArrayDisplay_EM (&lpMemPTD->YYResExec.tkToken, TRUE, &plLocalVars.bCtrlBreak);

                    // Free the result
                    FreeResult (&lpMemPTD->YYResExec);

                    // We no longer need these values
                    ZeroMemory (&lpMemPTD->YYResExec, sizeof (lpMemPTD->YYResExec));
                } // End IF

                // Fall through to common code

            case EXITTYPE_ERROR:        // Display the prompt unless called by Quad or User fcn/opr
            case EXITTYPE_STACK_FULL:   // ...
            case EXITTYPE_STOP:         // ...
            case EXITTYPE_NOVALUE:      // ...
            case EXITTYPE_RETURNxLX:
                // Get a ptr to the current SIS header
                lpSISPrv =
                lpSISCur = lpMemPTD->lpSISCur;

                // If this level is an AFO, ...
                while (lpSISPrv
                    && lpSISPrv->bAFO)
                    // Peel back
                    lpSISPrv = lpSISPrv->lpSISPrv;

                // If this level is Immediate Execution Mode, ...
                while (lpSISPrv
                    && lpSISPrv->DfnType EQ DFNTYPE_IMM)
                    // Peel back
                    lpSISPrv = lpSISPrv->lpSISPrv;

                // If this level is Execute, skip the prompt
                //   as Execute will handle that
                if (lpSISPrv
                 && lpSISPrv->DfnType EQ DFNTYPE_EXEC)
                {
                    // If the result of {execute}[]xLX is a normal
                    //   result (EXITTYPE_NODISPLAY),
                    //   return  EXITTYPE_RETURNxLX so the caller
                    //   can avoid displaying a prompt until the
                    //   result of executing []xLX is handled
                    if (exitType EQ EXITTYPE_NODISPLAY)
                        exitType =  EXITTYPE_RETURNxLX;

                    // Pass on this exit type to the caller
                    plLocalVars.ExitType = exitType;
////////////////////
////////////////////break;
                } // End IF

////////////////// If this level or an adjacent preceding level is from
//////////////////   Execute or Immediate Execution Mode,
//////////////////   peel back to the preceding level
////////////////while (lpSISPrv
////////////////    && (lpSISPrv->DfnType EQ DFNTYPE_EXEC
////////////////     || lpSISPrv->DfnType EQ DFNTYPE_IMM))
////////////////    lpSISPrv = lpSISPrv->lpSISPrv;
                break;

            case EXITTYPE_GOTO_ZILDE:   // Nothing more to do with these
            case EXITTYPE_NONE:         // ...
                break;

            defstop
                break;
        } // End SWITCH
    } // End IF

    // Restore the previous value of dwTlsPlLocalVars
    TlsSetValue (dwTlsPlLocalVars, oldTlsPlLocalVars);

    // Restore the previous executing state
    SetExecuting (lpMemPTD, bOldExecuting);

    // Decrement the depth counter
    lpMemPTD->uExecDepth--;

    DBGLEAVE;

    return plLocalVars.ExitType;
} // End ParseLine
#undef  APPEND_NAME


//***************************************************************************
//  $pl_yylex
//
//  Lexical analyzer for Bison
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- pl_yylex"
#else
#define APPEND_NAME
#endif

int pl_yylex
    (LPPL_YYSTYPE  lpYYLval,        // Ptr to lval
     LPPLLOCALVARS lpplLocalVars)   // Ptr to local plLocalVars

{
#ifdef DEBUG
    static UINT YYIndex = 0;        // Unique index for each YYRes
#endif
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPSIS_HEADER lpSISCur;          // Ptr to current SIS layer

    // If we're restarting from a Control Structure, ...
    if (lpplLocalVars->bRestart)
    {
        lpplLocalVars->bRestart = FALSE;

        return DIAMOND;
    } // End IF

PL_YYLEX_START:
    // Clear the fields
    ZeroMemory (lpYYLval, sizeof (lpYYLval[0]));

    // Because we're parsing the stmt from right to left
    lpplLocalVars->lptkNext--;

#if (defined (DEBUG)) && (defined (YYLEX_DEBUG))
    dprintfWL9 (L"==pl_yylex:  TknType = %S, CharIndex = %d",
              GetTokenTypeName (lpplLocalVars->lptkNext->tkFlags.TknType),
              lpplLocalVars->lptkNext->tkCharIndex);
#endif

    // Return the current token
    lpYYLval->tkToken        = *lpplLocalVars->lptkNext;

    // Initialize the rest of the fields
////lpYYLval->TknCount       =              // Already zero from ZeroMemory
////lpYYLval->YYInuse        =              // ...
////lpYYLval->YYIndirect     = 0;           // ...
////lpYYLval->YYCopyArray    = FALSE;       // ...
////lpYYLval->lpYYFcnBase    = NULL;        // ...
////lpYYLval->lpYYStrandBase = NULL;        // ...

    // Get ptr to PerTabData global memory
    lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

#ifdef DEBUG
    lpYYLval->YYIndex        = ++YYIndex;
    lpYYLval->YYFlag         = TRUE;      // Mark as a pl_yylex Index
    lpYYLval->SILevel        = lpMemPTD->SILevel;
    lpYYLval->lpFileName     = FileNameOnly (__FILE__);
    lpYYLval->uLineNum       = __LINE__;
#endif

    // Split cases based upon the token type
    switch (lpplLocalVars->lptkNext->tkFlags.TknType)
    {
        case TKT_VARIMMED:
        case TKT_VARARRAY:
            // If the next token is a dyadic op, ...
            if (LookaheadDyadicOp (lpplLocalVars, &lpplLocalVars->lptkNext[-1]))
                return OP2CONSTANT;
            else
                return CONSTANT;

        case TKT_INPOUT:
            if (IsAPLCharQuad (lpplLocalVars->lptkNext->tkData.tkChar))
                return QUAD;
            else
                return QUOTEQUAD;

        case TKT_FCNNAMED:
            return NAMEFCN;

        case TKT_VARNAMED:
        {
            STFLAGS stFlags;        // STE flags

            // Get the STE flags
            stFlags = lpplLocalVars->lptkNext->tkData.tkSym->stFlags;

            // Split cases based upon the NAMETYPE_xxx
            switch (stFlags.stNameType)
            {
                case NAMETYPE_UNK:
                    if (stFlags.DfnSysLabel)
                        return SYSLBL;
                    else
                        return NAMEUNK;

                case NAMETYPE_VAR:
                    // If the next token is a dyadic op, ...
                    if (LookaheadDyadicOp (lpplLocalVars, &lpplLocalVars->lptkNext[-1]))
                        return OP2NAMEVAR;
////////////////////// Call this one TKT_VARNAMED
////////////////////lpYYLval->tkToken.tkFlags.TknType = TKT_VARNAMED;   // Already set

                    return NAMEVAR;

                case NAMETYPE_FN0:
                    // If the previous token is TKT_ASSIGN, ...
                    //   and the current one is not a system function
                    //   and the next one is not TKT_VARNAMED (NAMETYPE_VAR),
                    if (lpplLocalVars->lptkNext[ 1].tkFlags.TknType EQ TKT_ASSIGN
                     && stFlags.ObjName NE OBJNAME_SYS
                     && (lpplLocalVars->lptkNext[-1].tkFlags.TknType NE TKT_VARNAMED
                      || lpplLocalVars->lptkNext[-1].tkData.tkSym->stFlags.stNameType NE NAMETYPE_VAR))
                    {
////////////////////////// Call this one TKT_VARNAMED
////////////////////////lpYYLval->tkToken.tkFlags.TknType = TKT_VARNAMED;   // Already set

                        return NAMEVAR;
                    } // End IF

                    // Call this one TKT_FCNNAMED
                    lpYYLval->tkToken.tkFlags.TknType = TKT_FCNNAMED;

                    if (stFlags.ObjName EQ OBJNAME_SYS)
                        return SYSFN0;
                    else
                        return USRFN0;

                case NAMETYPE_FN12:
                    // If the previous token is TKT_ASSIGN, ...
                    //   and the current one is not a system function
                    //   and the next one is not TKT_VARNAMED (NAMETYPE_VAR),
                    if (lpplLocalVars->lptkNext[ 1].tkFlags.TknType EQ TKT_ASSIGN
                     && stFlags.ObjName NE OBJNAME_SYS
                     && (lpplLocalVars->lptkNext[-1].tkFlags.TknType NE TKT_VARNAMED
                      || lpplLocalVars->lptkNext[-1].tkData.tkSym->stFlags.stNameType NE NAMETYPE_VAR))
                    {
////////////////////////// Call this one TKT_VARNAMED
////////////////////////lpYYLval->tkToken.tkFlags.TknType = TKT_VARNAMED;   // Already set

                        return NAMEVAR;
                    } // End IF

                    // Call this one TKT_FCNNAMED
                    lpYYLval->tkToken.tkFlags.TknType = TKT_FCNNAMED;

                    if (stFlags.ObjName EQ OBJNAME_SYS)
                        return SYSFN12;
                    else
                        return NAMEFCN;

                case NAMETYPE_OP1:
                    // Call this one TKT_OP1NAMED
                    lpYYLval->tkToken.tkFlags.TknType = TKT_OP1NAMED;

                    goto PL_YYLEX_NAMEOP1;

                case NAMETYPE_OP2:
                    // Call this one TKT_OP2NAMED
                    lpYYLval->tkToken.tkFlags.TknType = TKT_OP2NAMED;

                    goto PL_YYLEX_NAMEOP2;

                case NAMETYPE_OP3:
                    // Call this one TKT_OP3NAMED
                    lpYYLval->tkToken.tkFlags.TknType = TKT_OP3NAMED;

                    goto PL_YYLEX_NAMEOP3;

                case NAMETYPE_TRN:
                    // If the previous token is TKT_ASSIGN, ...
                    //   and the current one is not a system function
                    //   and the next one is not TKT_VARNAMED (NAMETYPE_VAR),
                    if (lpplLocalVars->lptkNext[ 1].tkFlags.TknType EQ TKT_ASSIGN
                     && stFlags.ObjName NE OBJNAME_SYS
                     && (lpplLocalVars->lptkNext[-1].tkFlags.TknType NE TKT_VARNAMED
                      || lpplLocalVars->lptkNext[-1].tkData.tkSym->stFlags.stNameType NE NAMETYPE_VAR))
                    {
////////////////////////// Call this one TKT_VARNAMED
////////////////////////lpYYLval->tkToken.tkFlags.TknType = TKT_VARNAMED;   // Already set

                        return NAMEVAR;
                    } // End IF

                    // Call this one TKT_FCNNAMED
                    lpYYLval->tkToken.tkFlags.TknType = TKT_FCNNAMED;

                    return NAMETRN;

                case NAMETYPE_LST:
                defstop
                    return UNK;
            } // End SWITCH

            return UNK;
        } // End TKT_VARNAMED

        case TKT_ASSIGN:
            // If the next token is a right paren, we're starting Selective Specification
            if (lpplLocalVars->lptkNext[-1].tkFlags.TknType EQ TKT_RIGHTPAREN)
            {
                // If we're already doing Selective Specification, signal an error
                if (lpplLocalVars->bSelSpec)
                    return UNK;
                // Mark as doing Selective Specification
                lpplLocalVars->bSelSpec =
                lpplLocalVars->bIniSpec = TRUE;
            } // End IF

            return ASSIGN;

        case TKT_LISTSEP:           // List separator
            return ';';

        case TKT_FCNAFO:            // Anonymous function
            return AFOFCN;

        case TKT_OP1AFO:            // Anonymous monadic operator
            return AFOOP1;

        case TKT_OP2AFO:            // Anonymous dyadic operator
            return AFOOP2;

        case TKT_AFOGUARD:          // AFO guard
            return AFOGUARD;

        case TKT_AFORETURN:         // AFO return
            return AFORETURN;

        case TKT_DELAFO:            // Del Anon -- either a function, or a monadic or dyadic operator bound to its operands
            return AFOFCN;

        case TKT_DEL:               // Del -- always a function, possibly derived from an operator
            // Search up the SIS chain to see what this is
            lpSISCur = SrchSISForDfn (lpMemPTD);

            // If the ptr is valid, ...
            if (lpSISCur)
            {
                // Fill in the ptr to the function header
                //   in both the return value and the token stream
                       lpYYLval->tkToken.tkData.tkVoid =
                lpplLocalVars->lptkNext->tkData.tkVoid = MakePtrTypeGlb (lpSISCur->hGlbDfnHdr);
                       lpYYLval->tkToken.tkFlags.TknType =
                lpplLocalVars->lptkNext->tkFlags.TknType = TKT_DELAFO;

                return AFOFCN;
            } else
                // Mark it as a SYNTAX ERROR
                return UNK;

        case TKT_DELDEL:            // Del Del -- either a monadic or dyadic operator
            // Search up the SIS chain to see what this is
            lpSISCur = SrchSISForDfn (lpMemPTD);

            // If the ptr is valid, ...
            if (lpSISCur)
            {
                // Split case based upon the function type
                switch (lpSISCur->DfnType)
                {
////////////////////case DFNTYPE_FCN:
////////////////////    // Fill in the ptr to the function header
////////////////////    //   in both the return value and the token stream
////////////////////           lpYYLval->tkToken.tkData.tkVoid =
////////////////////    lpplLocalVars->lptkNext->tkData.tkVoid = MakePtrTypeGlb (lpSISCur->hGlbDfnHdr);
////////////////////
////////////////////    // Change it into an anonymous function
////////////////////    //   in both the return value and the token stream
////////////////////           lpYYLval->tkToken.tkFlags.TknType =
////////////////////    lpplLocalVars->lptkNext->tkFlags.TknType = TKT_FCNAFO;
////////////////////
////////////////////    return AFOFCN;

                    case DFNTYPE_OP1:
                        // Fill in the ptr to the function header
                        //   in both the return value and the token stream
                               lpYYLval->tkToken.tkData.tkVoid =
                        lpplLocalVars->lptkNext->tkData.tkVoid = MakePtrTypeGlb (lpSISCur->hGlbDfnHdr);

                        // Change it into an anonymous monadic operator
                        //   in both the return value and the token stream
                               lpYYLval->tkToken.tkFlags.TknType =
                        lpplLocalVars->lptkNext->tkFlags.TknType = TKT_OP1AFO;

                        return AFOOP1;

                    case DFNTYPE_OP2:
                        // Fill in the ptr to the function header
                        //   in both the return value and the token stream
                               lpYYLval->tkToken.tkData.tkVoid =
                        lpplLocalVars->lptkNext->tkData.tkVoid = MakePtrTypeGlb (lpSISCur->hGlbDfnHdr);

                        // Change it into an anonymous dyadic operator
                        //   in both the return value and the token stream
                               lpYYLval->tkToken.tkFlags.TknType =
                        lpplLocalVars->lptkNext->tkFlags.TknType = TKT_OP2AFO;

                        return AFOOP2;

                    default:
                        // Mark it as a SYNTAX ERROR
                        return UNK;
                } // End SWITCH
            } else
                // Mark it as a SYNTAX ERROR
                return UNK;

        case TKT_SETALPHA:          // Set {alpha}
            Assert (lpplLocalVars->lptkNext[1].tkFlags.TknType EQ TKT_SOS);

            // If {alpha} has a value, ...
            if (lpYYLval->tkToken.tkData.tkSym->stFlags.Value)
                // Back off to the token to the right of the EOS/EOL
                //   so as to skip this stmt entirely
                lpplLocalVars->lptkNext -= (lpplLocalVars->lptkNext[1].tkData.tkIndex - 2);

            goto PL_YYLEX_START;    // Go around again

        case TKT_FCNIMMED:
            if (lpplLocalVars->lptkNext->tkData.tkIndex EQ UTF16_RIGHTARROW)
                return GOTO;
            else
                return PRIMFCN;

        case TKT_LINECONT:
        case TKT_SOS:
        case TKT_NOP:               // NOP
            goto PL_YYLEX_START;    // Ignore these tokens

        case TKT_CHRSTRAND:
            // If the next token is a dyadic op, ...
            if (LookaheadDyadicOp (lpplLocalVars, &lpplLocalVars->lptkNext[-1]))
                return OP2CHRSTRAND;
            else
                return CHRSTRAND;

        case TKT_NUMSTRAND:
        case TKT_NUMSCALAR:
            // If the next token is a dyadic op, ...
            if (LookaheadDyadicOp (lpplLocalVars, &lpplLocalVars->lptkNext[-1]))
                return OP2NUMSTRAND;
            else
                return NUMSTRAND;

        case TKT_OP1NAMED:
PL_YYLEX_NAMEOP1:
            // Check for / or /[I] or /[A]
            if (CheckNullOp3 (lpYYLval, lpplLocalVars))
                goto PL_YYLEX_START;    // Ignore this token
            else
                return NAMEOP1;

        case TKT_OP2NAMED:
PL_YYLEX_NAMEOP2:
            return NAMEOP2;

        case TKT_OP3NAMED:
PL_YYLEX_NAMEOP3:
            // Lookahead to see if this ambiguous symbol is adjacent to
            //   a function or monadic operator (return NAMEOP1), or
            //   a left paren                   (return NAMEFCN), or
            //   a dyadic operator              (return NAMEFCN), or
            //   an ambiguous operator          (return NAMEOP3), or
            //   a variable or niladic function (return NAMEFCN), or
            //   an assignment arrow            (return NAMEOP3ASSIGN)

            Assert (GetPtrTypeDir (lpYYLval->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

            // Convert the named OP3 to an immediate OP3
            lpYYLval->tkToken.tkFlags.TknType = TKT_OP3IMMED;
            lpYYLval->tkToken.tkFlags.ImmType = IMMTYPE_PRIMOP3;
            lpYYLval->tkToken.tkData.tkChar   = lpYYLval->tkToken.tkData.tkSym->stData.stChar;

            goto PL_YYLEX_OP3IMMED;

        case TKT_OP1IMMED:
            // Check for / or /[I] or /[A]
            if (CheckNullOp3 (lpYYLval, lpplLocalVars))
                goto PL_YYLEX_START;    // Ignore this token
            else
                return OP1;

        case TKT_OP2IMMED:
            // Split cases based upon the operator symbol
            switch (lpYYLval->tkToken.tkData.tkChar)
            {
                case UTF16_JOT:
                case UTF16_JOT2:
                    // If either the next or previous token is OP2NAMED,
                    //   or the previous token is OP1NAMED, ...
                    if (lpplLocalVars->lptkNext[-1].tkFlags.TknType EQ TKT_OP2NAMED
                     || lpplLocalVars->lptkNext[ 1].tkFlags.TknType EQ TKT_OP2NAMED
                     || lpplLocalVars->lptkNext[ 1].tkFlags.TknType EQ TKT_OP1NAMED)
                        return FILLJOT;

                    // If the previous token is VARNAMED and the name type is OP1 or OP2, ...
                    if (lpplLocalVars->lptkNext[ 1].tkFlags.TknType EQ TKT_VARNAMED
                     && (lpplLocalVars->lptkNext[ 1].tkData.tkSym->stFlags.stNameType EQ NAMETYPE_OP1
                      || lpplLocalVars->lptkNext[ 1].tkData.tkSym->stFlags.stNameType EQ NAMETYPE_OP2))
                        return FILLJOT;

                    // If the next     token is VARNAMED and the name type is OP2, ...
                    if (lpplLocalVars->lptkNext[-1].tkFlags.TknType EQ TKT_VARNAMED
                     && lpplLocalVars->lptkNext[-1].tkData.tkSym->stFlags.stNameType EQ NAMETYPE_OP2)
                        return FILLJOT;

                    // If the previous token is OP2IMMED and the value is not JOT, ...
                    if (lpplLocalVars->lptkNext[ 1].tkFlags.TknType EQ TKT_OP2IMMED
                     && !IsAPLCharJot (lpplLocalVars->lptkNext[ 1].tkData.tkChar))
                        return FILLJOT;

                    // If the next     token is OP2IMMED and the value is not JOT, ...
                    if (lpplLocalVars->lptkNext[-1].tkFlags.TknType EQ TKT_OP2IMMED
                     && !IsAPLCharJot (lpplLocalVars->lptkNext[-1].tkData.tkChar))
                        return FILLJOT;

                    // If the previous token is OP1IMMED, ...
                    if (lpplLocalVars->lptkNext[ 1].tkFlags.TknType EQ TKT_OP1IMMED)
                        return FILLJOT;
                    break;

                default:
                    break;
            } // End SWITCH

            return OP2;

        case TKT_OP3IMMED:
PL_YYLEX_OP3IMMED:
            // Lookahead to see if this ambiguous symbol is adjacent to
            //   a function or monadic operator (return OP1), or
            //   a left paren                   (return PRIMFCN), or
            //   a dyadic operator              (return PRIMFCN), or
            //   an ambiguous operator          (return OP3), or
            //   a variable or niladic function (return PRIMFCN), or
            //   an assignment arrow            (return OP3ASSIGN)

            // Split cases based upon the lookahead result
            switch (LookaheadAdjacent (lpplLocalVars, FALSE, TRUE, TRUE))
            {
                case '1':               // If the next token is a monadic operator, or
                case 'F':               // If the next token is a function,
                case 'T':               // If the next token is a train,
                                        //   then this token is a monadic operator
                    // Replace the function symbol with the
                    //   corresponding monadic operator symbol/index

                    // Split cases based upon the function symbol
                    switch (lpYYLval->tkToken.tkData.tkChar)
                    {
                        case UTF16_SLASH:
                            lpYYLval->tkToken.tkData.tkChar = INDEX_OPSLASH;

                            break;

                        case UTF16_SLASHBAR:
                            lpYYLval->tkToken.tkData.tkChar = INDEX_OPSLASHBAR;

                            break;

                        case UTF16_SLOPE:
                            lpYYLval->tkToken.tkData.tkChar = INDEX_OPSLOPE;

                            break;

                        case UTF16_SLOPEBAR:
                            lpYYLval->tkToken.tkData.tkChar = INDEX_OPSLOPEBAR;

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    lpYYLval->tkToken.tkFlags.TknType = TKT_OP1IMMED;
                    lpYYLval->tkToken.tkFlags.ImmType = IMMTYPE_PRIMOP1;

                    return OP1;

                case 'E':               // If the next token is EOL/EOS,
                case 'V':               //   a variable, or
                case '2':               //   a dyadic operator, or
                case '(':               //   a left paren, or
                case '0':               //   a niladic function,
                                        //   then this token is a function
                    lpYYLval->tkToken.tkFlags.TknType = TKT_FCNIMMED;
                    lpYYLval->tkToken.tkFlags.ImmType = IMMTYPE_PRIMFCN;

                    return PRIMFCN;

                case '3':               // If the next token is an ambiguous operator,
                    return OP3;         //   then this token is ambiguous

                case 'A':               // If the next token is an assignment arrow
                    return OP3ASSIGN;   //   then this token is an ambiguous assignment

                case '?':               // SYNTAX ERROR
                    return UNK;

                defstop
                    return UNK;
            } // End SWITCH

        case TKT_OPJOTDOT:
            return JOTDOT;

        case TKT_LEFTPAREN:
            // If we're in LookaheadSurround and this is the stop token, mark as EOL
            if (lpplLocalVars->bLookAhead
             && lpplLocalVars->lptkStop EQ lpplLocalVars->lptkNext)
                return EOL;
            else
                return '(';

        case TKT_RIGHTPAREN:
            // Lookahead to see if this right grouping symbol surrounds
            //   a dyadic operator              (return '#'), or
            //   a function or monadic operator (return '>'), or
            //   a train                        (return '%'), or
            //   a variable or niladic function (return ')' or '^')

            // Split cases based upon the lookahead result
            switch (LookaheadSurround (lpplLocalVars))
            {
                case '2':               // Dyadic operator
                    return '#';

                case '1':               // Monadic operator
                case '3':               // Ambiguous ...
                case 'F':               // Function
                    return '>';

                case 'V':               // Variable
                case '0':               // Niladic function
                case 'L':               // List
                    // Lookahead to see if the next token after the matching
                    //   right paren is a dyadic op in which case the object
                    //   which these parens surround is absorbed as the right
                    //   operand of the dyadic op.

                    // Each right grouping symbol has the index of the matching
                    //   left grouping symbol in its tkData.tkIndex
                    // Get a ptr to the token adjacent to ("- 1") the matching left
                    //   grouping symbol.

                    // If the token after the matching left paren is a dyadic op, ...
                    if (LookaheadDyadicOp (lpplLocalVars,
                                          &lpplLocalVars->lptkStart[lpplLocalVars->lptkNext->tkData.tkIndex - 1]))
                        return '^';     // Variable as right operand

                    return ')';

                case 'T':               // Train
                    return '%';

                case '?':               // SYNTAX ERROR
                    return UNK;

                defstop
                    return UNK;
            } // End SWITCH

        case TKT_LEFTBRACKET:
            return '[';

        case TKT_RIGHTBRACKET:
            // Lookahead to see if this right grouping symbol is adjacent to
            //   a dyadic operator              (return '?'), or
            //   a function or monadic operator (return '}'), or
            //   a variable or niladic function (return ']')

            // Split cases based upon the lookahead result
            switch (LookaheadAdjacent (lpplLocalVars, TRUE, FALSE, FALSE))
            {
                case '2':               // If the next token is a dyadic operator,
                    return '?';         //   then this token is an axis operator

                case '1':               // If the next token is a monadic operator, or
                case '3':               //   an ambiguous operator, or
                case 'A':               //   an assignment arrow, or
                case 'F':               //   a function,
                case 'T':               //   a train,
                case 'J':               //   a jot,
                    return '}';         //   then this token is an axis operator

                case 'E':               // If the next token is EOL/EOS, or
                case 'V':               // If the next token is a variable
                case '0':               //   or a niladic function,
                    return ']';         //   then this token is an indexing symbol

                case '?':               // SYNTAX ERROR
                    return UNK;

                defstop
                    return UNK;
            } // End SWITCH

        case TKT_LABELSEP:
            // If we're at the EOL, return '\0'
            if (lpplLocalVars->lptkNext[-2].tkFlags.TknType EQ TKT_EOL)
                return '\0';

            // Fall through to common code

        case TKT_EOS:
            // If we're to execute only one stmt, ...
            if (lpplLocalVars->bExec1Stmt)
                return '\0';

            // Skip to end of the current stmt
            lpplLocalVars->lptkNext = &lpplLocalVars->lptkNext[lpplLocalVars->lptkNext->tkData.tkIndex];

            // Save a ptr to the EOS/EOL token
            lpplLocalVars->lptkEOS = lpplLocalVars->lptkNext;

            // And again to the end of the next stmt
            lpplLocalVars->lptkNext = &lpplLocalVars->lptkNext[lpplLocalVars->lptkNext->tkData.tkIndex];

            return DIAMOND;

        case TKT_EOL:
            return '\0';

        case TKT_COLON:
            return ':';

        case TKT_LEFTBRACE:
            return LBRACE;

        case TKT_RIGHTBRACE:
        {
            DFN_TYPES dfnType;
            UBOOL     bAfoArgs;

            // Point to the right brace token
            lpYYLval->lptkRhtBrace  =  lpplLocalVars->lptkNext;

            // Point to the matching left brace token
            lpYYLval->lptkLftBrace  = &lpplLocalVars->lptkStart[lpYYLval->lptkRhtBrace->tkData.tkIndex];

            // Get the DFNTYPE_xxx
            dfnType = lpplLocalVars->lptkNext->tkData.tkDfnType;

            // Get the AfoArgs flag
            bAfoArgs = lpplLocalVars->lptkNext->tkFlags.bAfoArgs;

            // Point the next token to one to the right of the matching left brace
            //   so that the next token we process is the left brace
            lpplLocalVars->lptkNext = &lpYYLval->lptkLftBrace[1];

            // Split cases based upon the DFNTYPE_xxx
            switch (dfnType)
            {
                case DFNTYPE_FCN:
                    if (bAfoArgs)
                        return RBRACE_FCN_FN12;
                    else
                        return RBRACE_FCN_FN0;

                case DFNTYPE_OP1:
////////////////////if (bAfoArgs)
                        return RBRACE_OP1_FN12;
////////////////////else
////////////////////    return RBRACE_OP1_FN0;

                case DFNTYPE_OP2:
////////////////////if (bAfoArgs)
                        return RBRACE_OP2_FN12;
////////////////////else
////////////////////    return RBRACE_OP2_FN0;

                defstop
                    return UNK;
            } // End SWITCH
        } // End TKT_RIGHTBRACE

        case TKT_SYS_NS:
            return SYSNS;           // System namespace

        case TKT_CS_ANDIF:          // Control structure:  ANDIF
            return CS_ANDIF;

        case TKT_CS_ASSERT:         // Control Structure:  ASSERT
            return CS_ASSERT;

        case TKT_CS_CASE:           // Control Structure:  CASE
            return CS_CASE;

        case TKT_CS_CASELIST:       // Control Structure:  CASELIST
            return CS_CASELIST;

        case TKT_CS_CONTINUE:       // Control Structure:  CONTINUE
            return CS_CONTINUE;

        case TKT_CS_CONTINUEIF:     // Control Structure:  CONTINUEIF
            return CS_CONTINUEIF;

        case TKT_CS_ELSE:           // Control Structure:  ELSE
            return CS_ELSE;

        case TKT_CS_ELSEIF:         // Control Structure:  ELSEIF
            return CS_ELSEIF;

        case TKT_CS_ENDFOR:         // Control Structure:  ENDFOR
            return CS_ENDFOR;

        case TKT_CS_ENDFORLCL:      // Control Structure:  ENDFORLCL
            return CS_ENDFORLCL;

        case TKT_CS_ENDREPEAT:      // Control Structure:  ENDREPEAT
            return CS_ENDREPEAT;

        case TKT_CS_ENDWHILE:       // Control Structure:  ENDWHILE
            return CS_ENDWHILE;

        case TKT_CS_FOR:            // Control Structure:  FOR
            // Check for Ctrl Strucs in AFO
            if (lpplLocalVars->bAfoCtrlStruc)  // FOR
                goto SYNTAX_EXIT;

            return CS_FOR;

        case TKT_CS_FORLCL:         // Control Structure:  FORLCL
            // Check for Ctrl Strucs in AFO
            if (lpplLocalVars->bAfoCtrlStruc)  // FORLCL
                goto SYNTAX_EXIT;

            return CS_FORLCL;

        case TKT_CS_GOTO:           // Control Structure:  GOTO
            // Check for Ctrl Strucs in AFO
            if (lpplLocalVars->bAfoCtrlStruc)  // GOTO
                goto SYNTAX_EXIT;

            return GOTO;

        case TKT_CS_IF:             // Control Structure:  IF
            // Check for Ctrl Strucs in AFO
            if (lpplLocalVars->bAfoCtrlStruc)  // IF
                goto SYNTAX_EXIT;

            return CS_IF;

        case TKT_CS_IN:             // Control Structure:  IN
            return CS_IN;

        case TKT_CS_LEAVE:          // Control Structure:  LEAVE
            return CS_LEAVE;

        case TKT_CS_LEAVEIF:        // Control Structure:  LEAVEIF
            return CS_LEAVEIF;

        case TKT_CS_ORIF:           // Control Structure:  ORIF
            return CS_ORIF;

        case TKT_CS_RETURN:         // Control Structure:  RETURN
            // Check for Ctrl Strucs in AFO
            if (lpplLocalVars->bAfoCtrlStruc)   // RETURN
                goto SYNTAX_EXIT;

            // If this is the first time through for this keyword,
            //   return a CONSTANT
            if (!lpplLocalVars->bReturn)
            {
                // Return a Boolean zero
                lpYYLval->tkToken.tkFlags.TknType = TKT_VARIMMED;
                lpYYLval->tkToken.tkFlags.ImmType = IMMTYPE_BOOL;
                lpYYLval->tkToken.tkData.tkBoolean = 0;

                // Increment so we see this token again
                lpplLocalVars->lptkNext++;

                // Mark for the next time
                lpplLocalVars->bReturn = TRUE;

                return CONSTANT;
            } else
            {
                // Unmark for the next time
                lpplLocalVars->bReturn = FALSE;

                return GOTO;
            } // End IF/ELSE

        case TKT_CS_SELECT:         // Control Structure:  SELECT
            // Check for Ctrl Strucs in AFO
            if (lpplLocalVars->bAfoCtrlStruc)  // SELECT
                goto SYNTAX_EXIT;

            return CS_SELECT;

        case TKT_CS_SKIPCASE:       // Control Structure:  Special token
            return CS_SKIPCASE;

        case TKT_CS_SKIPEND:        // Control Structure:  Special token
            return CS_SKIPEND;

        case TKT_CS_UNTIL:          // Control Structure:  UNTIL
            return CS_UNTIL;

        case TKT_CS_WHILE:          // Control Structure:  WHILE
            // Check for Ctrl Strucs in AFO
            if (lpplLocalVars->bAfoCtrlStruc)  // WHILE
                goto SYNTAX_EXIT;

            return CS_WHILE;

        case TKT_CS_REPEAT:         // Control Structure:  REPEAT
            // Check for Ctrl Strucs in AFO
            if (lpplLocalVars->bAfoCtrlStruc)  // REPEAT
                goto SYNTAX_EXIT;

            // Fall through to common code

        case TKT_CS_ENDIF:          // Control Structure:  ENDIF
        case TKT_CS_ENDSELECT:      // ...                 ENDSELECT
        case TKT_CS_FOR2:           // ...                 FOR2
        case TKT_CS_IF2:            // ...                 IF2
        case TKT_CS_REPEAT2:        // ...                 REPEAT2
        case TKT_CS_SELECT2:        // ...                 SELECT2
        case TKT_CS_WHILE2:         // ...                 WHILE2
        case TKT_GLBDFN:            // Placeholder for hGlbDfnHdr
            goto PL_YYLEX_START;    // Ignore these tokens

        case TKT_SYNTERR:           // Syntax Error
            return UNK;

        case TKT_CS_END:            // Control Structure:  END
        defstop
            return UNK;
    } // End SWITCH
SYNTAX_EXIT:
    // Check on Ctrl Strucs in AFOs
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lpplLocalVars->lptkNext);
    return UNK;
} // End pl_yylex
#undef  APPEND_NAME


//***************************************************************************
//  $pl_yyerror
//
//  Error callback from Bison
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- pl_yyerror"
#else
#define APPEND_NAME
#endif

void pl_yyerror                     // Called for Bison syntax error
    (LPPLLOCALVARS lpplLocalVars,   // Ptr to local plLocalVars
     LPCHAR        s)               // Ptr to error msg

{
    char szTemp[32];

#ifdef DEBUG
    DbgMsg (s);
#endif

    // Skip this if we're in lookahead
    //   but save the char index
    if (lpplLocalVars->bLookAhead)
    {
        if (lpplLocalVars->lptkEnd <= &lpplLocalVars->lptkNext[1])
            lpplLocalVars->tkLACharIndex = NEG1U;
        else
            lpplLocalVars->tkLACharIndex = lpplLocalVars->lptkNext[1].tkCharIndex;
        lpplLocalVars->ExitType = EXITTYPE_ERROR;
    } else
    // If we've not already encountered an error, ...
    if (lpplLocalVars->ExitType NE EXITTYPE_ERROR)
    {
        // Set the caret ptr and exit type
        if (lpplLocalVars->tkLACharIndex NE NEG1U)
            lpplLocalVars->tkErrorCharIndex = lpplLocalVars->tkLACharIndex;
        else
        if (lpplLocalVars->lptkEnd <= &lpplLocalVars->lptkNext[1])
            lpplLocalVars->tkErrorCharIndex = NEG1U;
        else
            lpplLocalVars->tkErrorCharIndex = lpplLocalVars->lptkNext[1].tkCharIndex;

        lpplLocalVars->ExitType = EXITTYPE_ERROR;

        // Check for SYNTAX ERROR
#define ERR     "syntax error"
        lstrcpyn (szTemp, s, sizeof (ERR));     // Note: Terminates the string, too
        if (lstrcmp (szTemp, ERR) EQ 0)
            ErrorMessageIndirect (ERRMSG_SYNTAX_ERROR APPEND_NAME);
#undef  ERR
        else
#define ERR     "memory exhausted"
        lstrcpyn (szTemp, s, sizeof (ERR));     // Note: Terminates the string, too
        if (lstrcmp (szTemp, ERR) EQ 0)
            ErrorMessageIndirect (ERRMSG_WS_FULL APPEND_NAME);
#undef  ERR
    } // End IF/ELSE/IF
} // End pl_yyerror
#undef  APPEND_NAME


//***************************************************************************
//  $pl_yyfprintf
//
//  Debugger output
//***************************************************************************

void pl_yyfprintf
    (FILE  *hfile,          // Ignore this
     LPCHAR lpszFmt,        // Format string
     ...)                   // Zero or more arguments

{
#if (defined (DEBUG)) && (defined (YYFPRINTF_DEBUG))
    va_list  vl;
    APLU3264 i1,
             i2,
             i3;
    static   char szTemp[256] = {'\0'};

    va_start (vl, lpszFmt);

    // Bison uses no more than three arguments.
    // Note we must grab them separately this way
    //   as using va_arg in the argument list to
    //   wsprintf pushes the arguments in reverse
    //   order.
    i1 = va_arg (vl, APLU3264);
    i2 = va_arg (vl, APLU3264);
    i3 = va_arg (vl, APLU3264);

    va_end (vl);

    // Accumulate into local buffer because
    //   Bison calls this function multiple
    //   times for the same line, terminating
    //   the last call for the line with a LF.
    wsprintf (&szTemp[lstrlen (szTemp)],
              lpszFmt,
              i1, i2, i3);
    // Check last character.
    i1 = lstrlen (szTemp);

    // If it's a LF, it's time to flush the buffer.
    if (szTemp[i1 - 1] EQ AC_LF)
    {
        szTemp[i1 - 1] = AC_EOS;    // Remove trailing LF
                                    //   because we're displaying
                                    //   in a GUI.
        DbgMsg (szTemp);            // Display in my debugger window.

        szTemp[0] = AC_EOS;         // Restart the buffer
    } // End IF
#endif
} // End pl_yyfprintf


#if (defined (DEBUG) & defined (YYPRINT))
//***************************************************************************
//  $pl_yyprint
//
//  Debugger output
//***************************************************************************

void pl_yyprint
    (FILE            *yyoutput,
     yytype_uint16    yytoknum,
     PL_YYSTYPE const yyvaluep)

{
    WCHAR wszTemp[32],
         *lpwsz;
    char *lpszFmt1 = "TKT_%s",
         *lpszFmt2 = "TKT_%s:  %S";
    char *lpszFmt;

    // Split cases based upon the token type
    switch (yyvaluep.tkToken.tkFlags.TknType)
    {
        case TKT_VARIMMED:
            lpwsz =
            FormatImmed (wszTemp,
                         yyvaluep.tkToken.tkFlags.ImmType,
         (LPAPLLONGEST) &yyvaluep.tkToken.tkData.tkLongest);
            lpwsz[-1] = WC_EOS;
            lpszFmt = lpszFmt2;

            break;

        default:
            wszTemp[0] = WC_EOS;
            lpszFmt = lpszFmt1;

            break;
    } // End SWITCH

    pl_yyfprintf (NULL,
                  lpszFmt,
                  GetTokenTypeName (yyvaluep.tkToken.tkFlags.TknType),
                  wszTemp);
} // End pl_yyprint
#endif


//***************************************************************************
//  End of File: pl_parse.y
//***************************************************************************
