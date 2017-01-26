//***************************************************************************
//  NARS2000 -- Parser Grammar for Control Structures
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

%{
#define STRICT
#include <windows.h>
#include "headers.h"

////#define YYLEX_DEBUG
////#define YYFPRINTF_DEBUG

#ifdef DEBUG
#define YYERROR_VERBOSE
#define YYDEBUG 1
#define YYFPRINTF               cs_yyfprintf
#define fprintf                 cs_yyfprintf
////#define YYPRINT                 cs_yyprint
void cs_yyprint     (FILE *yyoutput, unsigned short int yytoknum, CS_YYSTYPE const yyvaluep);
#endif

#define YYMALLOC    malloc
#define YYFREE      free

#include "cs_parse.proto"

#define    YYSTYPE     CS_YYSTYPE
#define  LPYYSTYPE   LPCS_YYSTYPE
#define tagYYSTYPE  tagCS_YYSTYPE

// The following #defines are needed to allow multiple parses
//   to coexist in the same file
#define yy_symbol_print         cs_yy_symbol_print
#define yy_symbol_value_print   cs_yy_symbol_value_print
#define yy_reduce_print         cs_yy_reduce_print
#define yydestruct              cs_yydestruct

////#define DbgMsgWP(a)         DbgMsgW(a)
////#define DbgMsgWP(a)         DbgMsgW(a); DbgBrk ()
////#define DbgMsgWP(a)         DbgMsgW(a)
    #define DbgMsgWP(a)

LPTOKEN     lptk1st, lptkPrv;
TOKEN_TYPES TknType;
TOKEN       tkTmp;

%}

%pure-parser
%name-prefix "cs_yy"
%parse-param {LPCSLOCALVARS lpcsLocalVars}
%lex-param   {LPCSLOCALVARS lpcsLocalVars}
%token  ANDIF
%token  ASSERT
%token  CASE
%token  CASELIST
%token  CONTINUE
%token  CONTINUEIF
%token  ELSE
%token  ELSEIF
%token  END
%token  ENDFOR
%token  ENDFORLCL
%token  ENDIF
%token  ENDREPEAT
%token  ENDSELECT
%token  ENDWHILE
%token  ENS
%token  EOS
%token  FOR
%token  FORLCL
%token  GOTO
%token  IF
%token  INFOR
%token  LEAVE
%token  LEAVEIF
%token  NEC
%token  ORIF
%token  REPEAT
%token  RETURN
%token  SELECT
%token  SKIPCASE
%token  SKIPEND
%token  SOS
%token  UNK
%token  UNTIL
%token  WHILE

%start CtrlStruc

%%

CtrlStruc:
    /* Empty */                                                 {DbgMsgWP (L"%%CtrlStruc:  <empty>");
                                                                }
  | Stmts                                                       {DbgMsgWP (L"%%CtrlStruc:  Stmts");
                                                                    $$ = $1;
                                                                }
  | Stmts  CSRec                                                {DbgMsgWP (L"%%CtrlStruc:  Stmts CSRec");
                                                                    // Ensure there is no unmatched ContinueLeave in $2
                                                                    if ($2.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $2;
                                                                }
  |        CSRec                                                {DbgMsgWP (L"%%CtrlStruc:  CSRec");
                                                                    // Ensure there is no unmatched ContinueLeave in $1
                                                                    if ($1.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  ;

CSRec:
          AssertStmt                                            {DbgMsgWP (L"%%CSRec:  AssertStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $1
                                                                    if ($1.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  |       ForStmt                                               {DbgMsgWP (L"%%CSRec:  ForStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $1
                                                                    if ($1.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  |       ForLclStmt                                            {DbgMsgWP (L"%%CSRec:  ForLclStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $1
                                                                    if ($1.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  |       GotoStmt                                              {DbgMsgWP (L"%%CSRec:  GotoStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $1
                                                                    if ($1.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  |       IfStmt                                                {DbgMsgWP (L"%%CSRec:  IfStmt");
                                                                    // IfStmt is the only stmt allowed to have an unmatched ContinueLeave
                                                                    $$ = $1;
                                                                }
  |       RepeatStmt                                            {DbgMsgWP (L"%%CSRec:  RepeatStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $1
                                                                    if ($1.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  |       ReturnStmt                                            {DbgMsgWP (L"%%CSRec:  ReturnStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $1
                                                                    if ($1.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  |       SelectStmt                                            {DbgMsgWP (L"%%CSRec:  SelectStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $1
                                                                    if ($1.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  |       WhileStmt                                             {DbgMsgWP (L"%%CSRec:  WhileStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $1
                                                                    if ($1.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  | CSRec AssertStmt                                            {DbgMsgWP (L"%%CSRec:  CSRec AssertStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $2
                                                                    if ($2.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // There can't be an unmatched CONTINUE/LEAVE in $2, so ignore it
                                                                    $$ = $1;
                                                                }
  | CSRec ForStmt                                               {DbgMsgWP (L"%%CSRec:  CSRec ForStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $2
                                                                    if ($2.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // There can't be an unmatched CONTINUE/LEAVE in $2, so ignore it
                                                                    $$ = $1;
                                                                }
  | CSRec ForLclStmt                                            {DbgMsgWP (L"%%CSRec:  CSRec ForLclStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $2
                                                                    if ($2.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // There can't be an unmatched CONTINUE/LEAVE in $2, so ignore it
                                                                    $$ = $1;
                                                                }
  | CSRec GotoStmt                                              {DbgMsgWP (L"%%CSRec:  CSRec GotoStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $2
                                                                    if ($2.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // There can't be an unmatched CONTINUE/LEAVE in $2, so ignore it
                                                                    $$ = $1;
                                                                }
  | CSRec IfStmt                                                {DbgMsgWP (L"%%CSRec:  CSRec IfStmt");
                                                                    // If $1 has an unmatched ContinueLeave, ...
                                                                    if ($1.lptkCL1st)
                                                                    {
                                                                        // If $2 has an unmatched ContinueLeave, ...
                                                                        if ($2.lptkCL1st)
                                                                        // Loop through $2's unmatched ContinueLeave
                                                                        for (lptk1st = $2.lptkCL1st; lptk1st <= $2.lptkCur; lptk1st++)
                                                                        // If it's a ContinueLeave, ...
                                                                        if (lptk1st->tkData.uCLIndex EQ $2.uCLIndex)
                                                                        {
                                                                            Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUEIF
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                            // If it's not ENDIF, ...
                                                                            if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                                // Convert to $1's CLIndex so they are all the same
                                                                                lptk1st->tkData.uCLIndex = $1.uCLIndex;
                                                                        } // End IF/FOR/IF

                                                                        // Copy the ContinueLeave index
                                                                        $2.uCLIndex                 =
                                                                        $2.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                        // Move the ptr to encompass $1's ContinueLeave
                                                                        $2.lptkCL1st                =
                                                                        $2.lptk1st                  = $1.lptkCL1st;
                                                                    } // End IF

                                                                    $$ = $2;
                                                                }
  | CSRec RepeatStmt                                            {DbgMsgWP (L"%%CSRec:  CSRec RepeatStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $2
                                                                    if ($2.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // There can't be an unmatched CONTINUE/LEAVE in $2, so ignore it
                                                                    $$ = $1;
                                                                }
  | CSRec ReturnStmt                                            {DbgMsgWP (L"%%CSRec:  CSRec ReturnStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $2
                                                                    if ($2.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // There can't be an unmatched CONTINUE/LEAVE in $2, so ignore it
                                                                    $$ = $1;
                                                                }
  | CSRec SelectStmt                                            {DbgMsgWP (L"%%CSRec:  CSRec SelectStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $2
                                                                    if ($2.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // There can't be an unmatched CONTINUE/LEAVE in $2, so ignore it
                                                                    $$ = $1;
                                                                }
  | CSRec WhileStmt                                             {DbgMsgWP (L"%%CSRec:  CSRec WhileStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $2
                                                                    if ($2.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // There can't be an unmatched CONTINUE/LEAVE in $2, so ignore it
                                                                    $$ = $1;
                                                                }
  ;

CSCLCSRec:
    /* Empty */                                                 {DbgMsgWP (L"%%CSCLCSRec:  <empty>");
                                                                    ZeroMemory (&$$, sizeof ($$));
                                                                }
  |             CSRec                                           {DbgMsgWP (L"%%CSCLCSRec:  CSRec");
                                                                    $$ = $1;
                                                                }
  |       CLRec                                                 {DbgMsgWP (L"%%CSCLCSRec:  CLRec");
                                                                    $$ = $1;
                                                                }
  |       CLRec CSRec                                           {DbgMsgWP (L"%%CSCLCSRec:  CLRec CSRec");
                                                                    // Note that righthand CSRec is never executed

                                                                    // If $2 has an unmatched ContinueLeave, ...
                                                                    if ($2.lptkCL1st)
                                                                    // Loop through $2's unmatched ContinueLeave
                                                                    for (lptk1st = $2.lptkCL1st; lptk1st <= $2.lptkCur; lptk1st++)
                                                                    // If it's a ContinueLeave, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $2.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                        // If it's not ENDIF, ...
                                                                        if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                            // Convert to $1's CLIndex so they are all the same
                                                                            lptk1st->tkData.uCLIndex = $1.uCLIndex;
                                                                    } // End IF/FOR/IF

                                                                    // Copy the ContinueLeave index
                                                                    $2.uCLIndex                 =
                                                                    $2.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    // Move the ptr to encompass $1's ContinueLeave
                                                                    $2.lptkCL1st                =
                                                                    $2.lptk1st                  = $1.lptkCL1st;

                                                                    $$ = $2;
                                                                }
  | CSRec CLRec                                                 {DbgMsgWP (L"%%CSCLCSRec:  CSRec CLRec");
                                                                    // If $1 has an unmatched ContinueLeave, ...
                                                                    if ($1.lptkCL1st)
                                                                    // Loop through $1's unmatched ContinueLeave
                                                                    for (lptk1st = $1.lptkCL1st; lptk1st <= $1.lptkCur; lptk1st++)
                                                                    // If it's a ContinueLeave, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $1.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                        // If it's not ENDIF, ...
                                                                        if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                            // Convert to $2's CLIndex so they are all the same
                                                                            lptk1st->tkData.uCLIndex = $2.uCLIndex;
                                                                    } // End IF/FOR/IF

                                                                    $$ = $2;
                                                                }
  | CSRec CLRec CSRec                                           {DbgMsgWP (L"%%CSCLCSRec:  CSRec CLRec CSRec");
                                                                    // Note that righthand CSRec is never executed

                                                                    // If $1 has an unmatched ContinueLeave, ...
                                                                    if ($1.lptkCL1st)
                                                                    // Loop through $1's unmatched ContinueLeave
                                                                    for (lptk1st = $1.lptkCL1st; lptk1st <= $1.lptkCur; lptk1st++)
                                                                    // If it's a ContinueLeave, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $1.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                        // If it's not ENDIF, ...
                                                                        if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                            // Convert to $2's CLIndex so they are all the same
                                                                            lptk1st->tkData.uCLIndex = $2.uCLIndex;
                                                                    } // End IF/FOR/IF

                                                                    // If $3 has an unmatched ContinueLeave, ...
                                                                    if ($3.lptkCL1st)
                                                                    // Loop through $3's unmatched ContinueLeave
                                                                    for (lptk1st = $3.lptkCL1st; lptk1st <= $3.lptkCur; lptk1st++)
                                                                    // If it's a ContinueLeave, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $3.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                        // If it's not ENDIF, ...
                                                                        if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                            // Convert to $2's CLIndex so they are all the same
                                                                            lptk1st->tkData.uCLIndex = $2.uCLIndex;
                                                                    } // End IF/FOR/IF

                                                                    // Copy the ContinueLeave index
                                                                    $3.uCLIndex                 =
                                                                    $3.lptkCur->tkData.uCLIndex = $2.uCLIndex;

                                                                    // If $1 has an unmatched ContinueLeave, ...
                                                                    if ($1.lptkCL1st)
                                                                        // Move the ptr to encompass $1's ContinueLeave
                                                                        $3.lptkCL1st            =
                                                                        $3.lptk1st              = $1.lptkCL1st;
                                                                    else
                                                                        // Move the ptr to encompass $2's ContinueLeave
                                                                        $3.lptkCL1st            =
                                                                        $3.lptk1st              = $2.lptkCL1st;

                                                                    $$ = $3;
                                                                }
  ;

NS:
    NEC SOS                                                     {DbgMsgWP (L"%%NS:  NEC SOS");
                                                                    // No return value needed
                                                                    $$ = $1;
                                                                }
  ;

NSS:
    NEC SOS                                                     {DbgMsgWP (L"%%NSS:  NEC SOS");
                                                                    // No return value needed
                                                                    $$ = $1;
                                                                }
  | NEC SOS Stmts                                               {DbgMsgWP (L"%%NSS:  NEC SOS Stmts");
                                                                    // No return value needed
                                                                    $$ = $1;
                                                                }
  ;

Stmts:
          ENS                                                   {DbgMsgWP (L"%%Stmts:  ENS");
                                                                    // No return value needed
                                                                    $$ = $1;
                                                                }
  | Stmts ENS                                                   {DbgMsgWP (L"%%Stmts:  Stmts ENS");
                                                                    // No return value needed
                                                                    $$ = $1;
                                                                }
  ;

SOSStmts:
    SOS                                                         {DbgMsgWP (L"%%SOSStmts:  SOS");
                                                                    // No return value needed
                                                                    $$ = $1;
                                                                }
  | SOS Stmts                                                   {DbgMsgWP (L"%%SOSStmts:  SOS Stmts");
                                                                    // No return value needed
                                                                    $$ = $1;
                                                                }
  ;

EndFor:
    ENDFOR    SOSStmts                                          {DbgMsgWP (L"%%EndFor:  ENDFOR SOSStmts");
                                                                    // Ensure the ENDFOR token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  | END       SOSStmts                                          {DbgMsgWP (L"%%EndFor:  END SOSStmts");
                                                                    // Ensure the END token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Change token END to ENDFOR to provide a consistent ending
                                                                    CS_ChangeTokenType (lpcsLocalVars, $1.lptkCur, TKT_CS_ENDFOR);

                                                                    $$ = $1;
                                                                }
  ;

EndForLcl:
    ENDFORLCL SOSStmts                                          {DbgMsgWP (L"%%EndForLcl:  ENDFORLCL SOSStmts");
                                                                    // Ensure the ENDFORLCL token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  | END       SOSStmts                                          {DbgMsgWP (L"%%EndForLcl:  END SOSStmts");
                                                                    // Ensure the END token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Change token END to ENDFORLCL to provide a consistent ending
                                                                    CS_ChangeTokenType (lpcsLocalVars, $1.lptkCur, TKT_CS_ENDFORLCL);

                                                                    $$ = $1;
                                                                }
  ;

EndIf:
    ENDIF     SOSStmts                                          {DbgMsgWP (L"%%EndIf:  ENDIF SOSStmts");
                                                                    // Ensure the ENDIF token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  | END       SOSStmts                                          {DbgMsgWP (L"%%EndIf:  END SOSStmts");
                                                                    // Ensure the END token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Change token END to ENDIF to provide a consistent ending
                                                                    CS_ChangeTokenType (lpcsLocalVars, $1.lptkCur, TKT_CS_ENDIF);

                                                                    $$ = $1;
                                                                }
  ;

EndRepeat:
    ENDREPEAT SOSStmts                                          {DbgMsgWP (L"%%EndRepeat:  ENDREPEAT SOSStmts");
                                                                    // Ensure the ENDREPEAT token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  | END       SOSStmts                                          {DbgMsgWP (L"%%EndRepeat:  END SOSStmts");
                                                                    // Ensure the END token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Change token END to ENDREPEAT to provide a consistent ending
                                                                    CS_ChangeTokenType (lpcsLocalVars, $1.lptkCur, TKT_CS_ENDREPEAT);

                                                                    $$ = $1;
                                                                }
  ;

EndSelect:
    ENDSELECT SOSStmts                                          {DbgMsgWP (L"%%EndSelect:  ENDSELECT SOSStmts");
                                                                    // Ensure the ENDSELECT token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  | END       SOSStmts                                          {DbgMsgWP (L"%%EndSelect:  END SOSStmts");
                                                                    // Ensure the END token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Change token END to ENDSELECT to provide a consistent ending
                                                                    CS_ChangeTokenType (lpcsLocalVars, $1.lptkCur, TKT_CS_ENDSELECT);

                                                                    $$ = $1;
                                                                }
  ;

EndWhile:
    ENDWHILE  SOSStmts                                          {DbgMsgWP (L"%%EndWhile:  ENDWHILE SOSStmts");
                                                                    // Ensure the ENDWHILE token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  | END       SOSStmts                                          {DbgMsgWP (L"%%EndWhile:  END SOSStmts");
                                                                    // Ensure the END token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Change token END to ENDWHILE to provide a consistent ending
                                                                    CS_ChangeTokenType (lpcsLocalVars, $1.lptkCur, TKT_CS_ENDWHILE);

                                                                    $$ = $1;
                                                                }
  ;

ContinueAndIf:
                CONTINUE SOSStmts                               {DbgMsgWP (L"%%ContinueAndIf:  CONTINUE SOSStmts");
                                                                    $$ = $1;
                                                                }
  |             CONTINUEIF NSS                                  {DbgMsgWP (L"%%ContinueAndIf:  CONTINUEIF NSS");
                                                                    $$ = $1;
                                                                }
  ;

LeaveAndIf:
                LEAVE   SOSStmts                                {DbgMsgWP (L"%%LeaveAndIf:  LEAVE SOSStmts");
                                                                    $$ = $1;
                                                                }
  |             LEAVEIF NSS                                     {DbgMsgWP (L"%%LeaveAndIf:  LEAVEIF NSS");
                                                                    $$ = $1;
                                                                }
  ;

CLRec:
                ContinueAndIf                                   {DbgMsgWP (L"%%CLRec:  ContinueAndIf");
                                                                    // Ensure the CONTINUE/CONTINUEIF token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Save the starting index common to this stmt
                                                                    $1.uCLIndex                 =
                                                                    $1.lptkCur->tkData.uCLIndex = lpcsLocalVars->lptkCSLink->tkData.uCLIndex;
                                                                    lpcsLocalVars->lptkCSLink->tkData.uCLIndex = 0;
                                                                    $1.lptkCL1st = $1.lptk1st = $1.lptkCur;

                                                                    $$ = $1;
                                                                }
  |             LeaveAndIf                                      {DbgMsgWP (L"%%CLRec:  LeaveAndIf");
                                                                    // Ensure the LEAVE/LEAVEIF token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Save the starting index common to this stmt
                                                                    $1.uCLIndex                 =
                                                                    $1.lptkCur->tkData.uCLIndex = lpcsLocalVars->lptkCSLink->tkData.uCLIndex;
                                                                    lpcsLocalVars->lptkCSLink->tkData.uCLIndex = 0;
                                                                    $1.lptkCL1st = $1.lptk1st = $1.lptkCur;

                                                                    $$ = $1;
                                                                }
  | CLRec       ContinueAndIf                                   {DbgMsgWP (L"%%CLRec:  CLRec ContinueAndIf");
                                                                    // Ensure the CONTINUE/CONTINUEIF token is SOS
                                                                    if (!$2.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    //   and the index common to this group
                                                                    $2.lptkCL1st                =
                                                                    $2.lptk1st                  = $1.lptkCL1st;
                                                                    $2.uCLIndex                 =
                                                                    $2.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $2;
                                                                }
  | CLRec CSRec ContinueAndIf                                   {DbgMsgWP (L"%%CLRec:  CLRec CSRec ContinueAndIf");
                                                                    // Note that righthand CSRec is never executed

                                                                    // Ensure the CONTINUE/CONTINUEIF token is SOS
                                                                    if (!$3.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$3.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // If $2 has unmatched ContinueLeave, ...
                                                                    if ($2.lptkCL1st)
                                                                    {
                                                                        Assert ($2.lptkCur->tkFlags.TknType EQ TKT_CS_IF);
                                                                    } // End IF

                                                                    // If $2 has unmatched ContinueLeave, ...
                                                                    if ($2.lptkCL1st)
                                                                    // Loop through $2's unmatched ContinueLeave
                                                                    for (lptk1st = $2.lptkCL1st; lptk1st <= $2.lptkCur; lptk1st++)
                                                                    // If it's a ContinueLeave, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $2.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                        // If it's not ENDIF, ...
                                                                        if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                            // Convert to $1's CLIndex so they are all the same
                                                                            lptk1st->tkData.uCLIndex = $1.uCLIndex;
                                                                    } // End IF/FOR/IF

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    //   and the index common to this group
                                                                    $3.lptkCL1st                =
                                                                    $3.lptk1st                  = $1.lptkCL1st;
                                                                    $3.uCLIndex                 =
                                                                    $3.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $3;
                                                                }
  | CLRec       LeaveAndIf                                      {DbgMsgWP (L"%%CLRec:  CLRec LeaveAndIf");
                                                                    // Ensure the LEAVE/LEAVEIF token is SOS
                                                                    if (!$2.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    //   and the index common to this group
                                                                    $2.lptkCL1st                =
                                                                    $2.lptk1st                  = $1.lptkCL1st;
                                                                    $2.uCLIndex                 =
                                                                    $2.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $2;
                                                                }
  | CLRec CSRec LeaveAndIf                                      {DbgMsgWP (L"%%CLRec:  CLRec CSRec LeaveAndIf");
                                                                    // Note that righthand CSRec is never executed

                                                                    // Ensure the LEAVE/LEAVEIF token is SOS
                                                                    if (!$3.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$3.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // If $2 has unmatched ContinueLeave, ...
                                                                    if ($2.lptkCL1st)
                                                                    {
                                                                        Assert ($2.lptkCur->tkFlags.TknType EQ TKT_CS_IF);
                                                                    } // End IF

                                                                    // If $2 has unmatched ContinueLeave, ...
                                                                    if ($2.lptkCL1st)
                                                                    // Loop through $2's unmatched ContinueLeave
                                                                    for (lptk1st = $2.lptkCL1st; lptk1st <= $2.lptkCur; lptk1st++)
                                                                    // If it's a ContinueLeave, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $2.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                        // If it's not ENDIF, ...
                                                                        if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                            // Convert to $1's CLIndex so they are all the same
                                                                            lptk1st->tkData.uCLIndex = $1.uCLIndex;
                                                                    } // End IF/FOR/IF

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    //   and the index common to this group
                                                                    $3.lptkCL1st                =
                                                                    $3.lptk1st                  = $1.lptkCL1st;
                                                                    $3.uCLIndex                 =
                                                                    $3.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $3;
                                                                }
  ;

AssertStmt:
    ASSERT NSS                                                  {DbgMsgWP (L"%%AssertStmt:  ASSERT NSS");
                                                                    // Ensure the ASSERT token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  ;

ForStmt:
    FOR NEC INFOR NSS CSCLCSRec EndFor                          {DbgMsgWP (L"%%ForStmt:  FOR NEC INFOR NSS CSCLCSRec EndFor");
                                                                    // Ensure the FOR token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Ensure the INFOR token is not SOS
                                                                    if ($3.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$3.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // N.B.:  $5 could be empty

                                                                    // If $5 has an unmatched ContinueLeave, ...
                                                                    if ($5.lptkCL1st)
                                                                    // Loop through $5's unmatched ContinueLeave
                                                                    for (lptk1st = $5.lptkCL1st; lptk1st <= $5.lptkCur; lptk1st++)
                                                                    // If it's a ContinueLeave, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $5.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                        // If it's not ENDIF, ...
                                                                        if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                            // Chain together the LEAVE/CONTINUE token and the EndFor token
                                                                            CS_ChainTokens (lpcsLocalVars, &lptk1st->tkData, $6.lptk1st);
                                                                    } // End IF/FOR/IF

                                                                    // Save the same unique identifier in the FOR and EndFor
                                                                    //   tokens so we may tie together the two and thus allow
                                                                    //   a branch to an EndFor stmt
                                                                    CS_SetTokenCLIndex (lpcsLocalVars, &$6.lptkCur->tkData, $1.uCLIndex);
                                                                    CS_SetTokenCLIndex (lpcsLocalVars, &$1.lptkCur->tkData, $1.uCLIndex);

                                                                    // Copy the FOR token as the next call to CS_ChainTokens clobbers it
                                                                    tkTmp = *$1.lptkCur;

                                                                    // Chain together the FOR token and the EndFor token
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $6.lptk1st);

                                                                    // Chain together the EndFor token and the FOR token
                                                                    CS_ChainTokens (lpcsLocalVars, &$6.lptkCur->tkData, &tkTmp);

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $6;
                                                                }
  ;

ForLclStmt:
    FORLCL NEC INFOR NSS CSCLCSRec EndForLcl                   {DbgMsgWP (L"%%ForStmt:  FORLCL NEC INFOR NSS CSCLCSRec EndForLcl");
                                                                    // Ensure the FORLCL token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Ensure the INFOR token is not SOS
                                                                    if ($3.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$3.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // N.B.:  $5 could be empty

                                                                    // If $5 has an unmatched ContinueLeave, ...
                                                                    if ($5.lptkCL1st)
                                                                    // Loop through $5's unmatched ContinueLeave
                                                                    for (lptk1st = $5.lptkCL1st; lptk1st <= $5.lptkCur; lptk1st++)
                                                                    // If it's a ContinueLeave, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $5.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                        // If it's not ENDIF, ...
                                                                        if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                            // Chain together the LEAVE/CONTINUE token and the EndForLcl token
                                                                            CS_ChainTokens (lpcsLocalVars, &lptk1st->tkData, $6.lptk1st);
                                                                    } // End IF/FOR/IF

                                                                    // Save the same unique identifier in the FORLCL and EndForLcl
                                                                    //   tokens so we may tie together the two and thus allow
                                                                    //   a branch to an EndForLcl stmt
                                                                    CS_SetTokenCLIndex (lpcsLocalVars, &$6.lptkCur->tkData, $1.uCLIndex);
                                                                    CS_SetTokenCLIndex (lpcsLocalVars, &$1.lptkCur->tkData, $1.uCLIndex);

                                                                    // Copy the FORLCL token as the next call to CS_ChainTokens clobbers it
                                                                    tkTmp = *$1.lptkCur;

                                                                    // Chain together the FORLCL token and the EndForLcl token
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $6.lptk1st);

                                                                    // Chain together the EndForLcl token and the FORLCL token
                                                                    CS_ChainTokens (lpcsLocalVars, &$6.lptkCur->tkData, &tkTmp);

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $6;
                                                                }
  ;

GotoStmt:
    GOTO NSS                                                    {DbgMsgWP (L"%%GotoStmt:  GOTO NSS");
                                                                    // Ensure the GOTO token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  ;

OrIfRec:
                 ORIF                                           {DbgMsgWP (L"%%OrIfRec:  ORIF");
                                                                    // Ensure the ORIF token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  | OrIfRec  NS  ORIF                                           {DbgMsgWP (L"%%OrIfRec:  OrIfRec NS ORIF");
                                                                    // Ensure the ORIF token is SOS
                                                                    if (!$3.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$3.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Chain together the last token in OrIfRec and the ORIF token
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $3.lptkIF1st);

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    $3.lptkIF1st = $1.lptkIF1st;

                                                                    $$ = $3;
                                                                }
  ;

AndIfRec:
                 ANDIF                                          {DbgMsgWP (L"%%AndIfRec:  ANDIF");
                                                                    // Ensure the ANDIF token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  | AndIfRec NS  ANDIF                                          {DbgMsgWP (L"%%AndIfRec:  AndIfRec NS ANDIF");
                                                                    // Ensure the ANDIF token is SOS
                                                                    if (!$3.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$3.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Chain together the last token in AndIfRec and the ANDIF token
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $3.lptkIF1st);

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    $3.lptkIF1st = $1.lptkIF1st;

                                                                    $$ = $3;
                                                                }
  ;

OrIfAndIf:
    OrIfRec  NSS                                                {DbgMsgWP (L"%%OrIfAndIf: OrIfRec NSS");
                                                                    $$ = $1;
                                                                }
  | AndIfRec NSS                                                {DbgMsgWP (L"%%OrIfAndIf: AndIfRec NSS");
                                                                    $$ = $1;
                                                                }
  ;


SkipEnd:
    SKIPEND SOS                                                 {DbgMsgWP (L"%%SkipEnd:  SKIPEND SOS");
                                                                    $$ = $1;
                                                                }
  ;

ElseIf:
            SkipEnd ELSEIF NSS                                  {DbgMsgWP (L"%%ElseIf:  SkipEnd ELSEIF NSS");
                                                                    // Ensure the ELSEIF token is SOS
                                                                    if (!$2.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Set the next .uCLIndex value
                                                                    CS_SetCLIndex (&$2);

                                                                    // Chain together the SkipEnd token and the ELSEIF token
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $2.lptkIF1st);

                                                                    $$ = $2;
                                                                }
  |         SkipEnd ELSEIF NS  OrIfAndIf                        {DbgMsgWP (L"%%ElseIf:  SkipEnd ELSEIF NS OrIfAndIf");
                                                                    // Ensure the ELSEIF token is SOS
                                                                    if (!$2.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Set the next .uCLIndex value
                                                                    CS_SetCLIndex (&$2);

                                                                    // If $4 has an unmatched Continue, ...
                                                                    if ($4.lptk1st)
                                                                    // Loop through $4's tokens setting that token's .uCLIndex to $2's
                                                                    for (lptk1st = $4.lptk1st; lptk1st <= $4.lptkCur; lptk1st++)
                                                                    // If it's an ANDIF or ORIF, ...
                                                                    if (lptk1st->tkFlags.TknType EQ TKT_CS_ANDIF
                                                                     || lptk1st->tkFlags.TknType EQ TKT_CS_ORIF)
                                                                        lptk1st->tkData.uCLIndex = $2.uCLIndex;

                                                                    // Chain together the SkipEnd token and the ELSEIF token
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $2.lptkIF1st);

                                                                    // Chain together the ELSEIF token and the first token in OrIfAndIf
                                                                    CS_ChainTokens (lpcsLocalVars, &$2.lptkCur->tkData, $4.lptkIF1st);

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    $4.lptkIF1st                = $2.lptkIF1st;
                                                                    $4.lptk1st                  = $2.lptkCur;
                                                                    $4.uCLIndex                 =
                                                                    $4.lptkCur->tkData.uCLIndex = $2.uCLIndex;

                                                                    $$ = $4;
                                                                }
  ;

ElseIfRec:
    ElseIf                                                      {DbgMsgWP (L"%%ElseIfRec:  ElseIf");
                                                                    $$ = $1;
                                                                }
  | ElseIfRec       ElseIf                                      {DbgMsgWP (L"%%ElseIfRec:  ElseIfRec ElseIf");
                                                                    // Chain together the last token in ElseIfRec and the first token in ElseIf
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $2.lptkIF1st);

                                                                    // If $2 has an unmatched Continue, ...
                                                                    if ($2.lptk1st)
                                                                    // Loop through $2's tokens converting that token's .uCLIndex to $1's
                                                                    for (lptk1st = $2.lptk1st; lptk1st <= $2.lptkCur; lptk1st++)
                                                                    // If it's the same .uCLIndex
                                                                    if (lptk1st->tkData.uCLIndex EQ $2.uCLIndex)
                                                                        lptk1st->tkData.uCLIndex = $1.uCLIndex;

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    $2.lptkIF1st                = $1.lptkIF1st;
                                                                    $2.lptkCL1st                =
                                                                    $2.lptk1st                  = $1.lptkCL1st;
                                                                    $2.uCLIndex                 =
                                                                    $2.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $2;
                                                                }
  | ElseIfRec CSRec ElseIf                                      {DbgMsgWP (L"%%ElseIfRec:  ElseIfRec CSRec ElseIf");
                                                                    // If $1 has an unmatched ContinueLeave, ...
                                                                    if ($1.lptkCL1st)
                                                                    {
                                                                        // If $2 has an unmatched ContinueLeave, ...
                                                                        if ($2.lptkCL1st)
                                                                        // Loop through $2's unmatched ContinueLeave
                                                                        for (lptk1st = $2.lptkCL1st; lptk1st <= $2.lptkCur; lptk1st++)
                                                                        // If it's a ContinueLeave, ...
                                                                        if (lptk1st->tkData.uCLIndex EQ $2.uCLIndex)
                                                                        {
                                                                            Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUEIF
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                            // If it's not ENDIF, ...
                                                                            if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                                // Convert to $1's CLIndex so they are all the same
                                                                                lptk1st->tkData.uCLIndex = $1.uCLIndex;
                                                                        } // End IF/FOR/IF

                                                                        // Copy the ContinueLeave index
                                                                        $2.uCLIndex                 =
                                                                        $2.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                        // Move the ptr to encompass $1's ContinueLeave
                                                                        $2.lptkCL1st                =
                                                                        $2.lptk1st                  = $1.lptkCL1st;
                                                                    } // End IF

                                                                    // Chain together the last token in ElseIfRec and the first token in ElseIf
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $3.lptkIF1st);

                                                                    // If $3 has an unmatched Continue, ...
                                                                    if ($3.lptk1st)
                                                                    // Loop through $3's tokens converting that token's .uCLIndex to $1's
                                                                    for (lptk1st = $3.lptk1st; lptk1st <= $3.lptkCur; lptk1st++)
                                                                    // If it's the same .uCLIndex
                                                                    if (lptk1st->tkData.uCLIndex EQ $3.uCLIndex)
                                                                        lptk1st->tkData.uCLIndex = $1.uCLIndex;

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    $3.lptkIF1st                = $1.lptkIF1st;
                                                                    $3.lptkCL1st                =
                                                                    $3.lptk1st                  = $1.lptkCL1st;
                                                                    $3.uCLIndex                 =
                                                                    $3.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $3;
                                                                }
  ;

ElseIfElse:
                    ELSE   SOSStmts                             {DbgMsgWP (L"%%ElseIfElse:  ELSE SOSStmts");
                                                                    // Ensure the ELSE token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Set the next .uCLIndex value
                                                                    CS_SetCLIndex (&$1);

                                                                    // Save as 1st token in sequence
                                                                    $1.lptkIF1st = $1.lptkCur;

                                                                    $$ = $1;
                                                                }
  | ElseIfRec       ELSE   SOSStmts                             {DbgMsgWP (L"%%ElseIfElse:  ElseIfRec ELSE SOSStmts");
                                                                    // Ensure the ELSE token is SOS
                                                                    if (!$2.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Chain together the last token in ElseIfRec and the ELSE token
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $2.lptkIF1st);

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    $2.lptkIF1st                = $1.lptkIF1st;
                                                                    $2.lptkCL1st                =
                                                                    $2.lptk1st                  = $1.lptkCL1st;
                                                                    $2.uCLIndex                 =
                                                                    $2.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $2;
                                                                }
  | ElseIfRec CSRec ELSE   SOSStmts                             {DbgMsgWP (L"%%ElseIfElse:  ElseIfRec CSRec ELSE SOSStmts");
                                                                    // Ensure the ELSE token is SOS
                                                                    if (!$3.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$3.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // If $1 has an unmatched ContinueLeave, ...
                                                                    if ($1.lptkCL1st)
                                                                    {
                                                                        // If $2 has an unmatched ContinueLeave, ...
                                                                        if ($2.lptkCL1st)
                                                                        // Loop through $2's unmatched ContinueLeave
                                                                        for (lptk1st = $2.lptkCL1st; lptk1st <= $2.lptkCur; lptk1st++)
                                                                        // If it's a ContinueLeave, ...
                                                                        if (lptk1st->tkData.uCLIndex EQ $2.uCLIndex)
                                                                        {
                                                                            Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUEIF
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                            // If it's not ENDIF, ...
                                                                            if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                                // Convert to $1's CLIndex so they are all the same
                                                                                lptk1st->tkData.uCLIndex = $1.uCLIndex;
                                                                        } // End IF/FOR/IF

                                                                        // Copy the ContinueLeave index
                                                                        $2.uCLIndex                 =
                                                                        $2.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                        // Move the ptr to encompass $1's ContinueLeave
                                                                        $2.lptkCL1st                =
                                                                        $2.lptk1st                  = $1.lptkCL1st;
                                                                    } // End IF

                                                                    // Chain together the last token in ElseIfRec and the ELSE token
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $3.lptk1st);

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    $3.lptkIF1st                = $1.lptkIF1st;
                                                                    $3.lptkCL1st                =
                                                                    $3.lptk1st                  = $1.lptkCL1st;
                                                                    $3.uCLIndex                 =
                                                                    $3.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $3;
                                                                }
  ;

IfStmt:
    IF NSS                                CSCLCSRec EndIf       {DbgMsgWP (L"%%IfStmt:  IF NSS CSCLCSRec EndIf");
                                                                    // Ensure the IF token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // N.B.:  $3 could be empty

                                                                    // Chain together the IF token and the EndIf token
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $4.lptkIF1st);

                                                                    // In case there's an unmatched ContinueLeave in the IfBody,
                                                                    //   pass on the ptr and index
                                                                    $4.lptkIF1st                = $1.lptkIF1st;
                                                                    $4.lptkCL1st                =
                                                                    $4.lptk1st                  = $3.lptkCL1st;
                                                                    $4.uCLIndex                 =
                                                                    $4.lptkCur->tkData.uCLIndex = $3.uCLIndex;

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $4;
                                                                }
  | IF NSS           CSCLCSRec ElseIfRec  CSCLCSRec EndIf       {DbgMsgWP (L"%%IfStmt:  IF NSS CSCLCSRec ElseIfRec CSCLCSRec EndIf");
                                                                    // Ensure the IF token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // N.B.:  $3 and $5 could be empty

                                                                    // If $3 has an unmatched ContinueLeave, ...
                                                                    if ($3.lptkCL1st)
                                                                    {
                                                                        // If $5 has an unmatched ContinueLeave, ...
                                                                        if ($5.lptkCL1st)
                                                                        // Loop through $5's unmatched ContinueLeave
                                                                        for (lptk1st = $5.lptkCL1st; lptk1st <= $5.lptkCur; lptk1st++)
                                                                        // If it's a ContinueLeave, ...
                                                                        if (lptk1st->tkData.uCLIndex EQ $5.uCLIndex)
                                                                        {
                                                                            Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUEIF
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                            // If it's not ENDIF, ...
                                                                            if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                                // Convert to $3's CLIndex so they are all the same
                                                                                lptk1st->tkData.uCLIndex = $3.uCLIndex;
                                                                        } // End IF/FOR/IF

                                                                        // Copy the ContinueLeave index
                                                                        $5.uCLIndex                 = $3.uCLIndex;
                                                                        if ($5.lptkCur)
                                                                            $5.lptkCur->tkData.uCLIndex = $3.uCLIndex;

                                                                        // Move the ptr to encompass $1's ContinueLeave
                                                                        $5.lptkCL1st                =
                                                                        $5.lptk1st                  = $3.lptkCL1st;
                                                                    } // End IF

                                                                    // Chain together the IF token and the first token in ElseIfRec
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $4.lptkIF1st);

                                                                    // Chain together the last token in ElseIfRec and the EndIf token
                                                                    CS_ChainTokens (lpcsLocalVars, &$4.lptkCur->tkData, $6.lptkIF1st);

                                                                    // In case there's an unmatched ContinueLeave in the IfBody,
                                                                    //   pass on the ptr and index
                                                                    $6.lptkIF1st                = $1.lptkIF1st;
                                                                    $6.lptkCL1st                =
                                                                    $6.lptk1st                  = $5.lptkCL1st;
                                                                    $6.uCLIndex                 =
                                                                    $6.lptkCur->tkData.uCLIndex = $5.uCLIndex;

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $6;
                                                                }
  | IF NSS           CSCLCSRec ElseIfElse CSCLCSRec EndIf       {DbgMsgWP (L"%%IfStmt:  IF NSS CSCLCSRec ElseIfElse CSCLCSRec EndIf");
                                                                    // Ensure the IF token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // N.B.:  $3 and $5 could be empty

                                                                    // If $3 has an unmatched ContinueLeave, ...
                                                                    if ($3.lptkCL1st)
                                                                    {
                                                                        // If $5 has an unmatched ContinueLeave, ...
                                                                        if ($5.lptkCL1st)
                                                                        // Loop through $5's unmatched ContinueLeave
                                                                        for (lptk1st = $5.lptkCL1st; lptk1st <= $5.lptkCur; lptk1st++)
                                                                        // If it's a ContinueLeave, ...
                                                                        if (lptk1st->tkData.uCLIndex EQ $5.uCLIndex)
                                                                        {
                                                                            Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUEIF
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                            // If it's not ENDIF, ...
                                                                            if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                                // Convert to $3's CLIndex so they are all the same
                                                                                lptk1st->tkData.uCLIndex = $3.uCLIndex;
                                                                        } // End IF/FOR/IF

                                                                        // Copy the ContinueLeave index
                                                                        $5.uCLIndex                 = $3.uCLIndex;
                                                                        if ($5.lptkCur)
                                                                            $5.lptkCur->tkData.uCLIndex = $3.uCLIndex;

                                                                        // Move the ptr to encompass $1's ContinueLeave
                                                                        $5.lptkCL1st                =
                                                                        $5.lptk1st                  = $3.lptkCL1st;
                                                                    } // End IF

                                                                    // Chain together the IF token and the first token in ElseIfElse
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $4.lptkIF1st);

                                                                    // Chain together the last token in ElseIfElse and the EndIf token
                                                                    CS_ChainTokens (lpcsLocalVars, &$4.lptkCur->tkData, $6.lptkIF1st);

                                                                    // In case there's an unmatched ContinueLeave in the IfBody,
                                                                    //   pass on the ptr and index
                                                                    $6.lptkIF1st                = $1.lptkIF1st;
                                                                    $6.lptkCL1st                =
                                                                    $6.lptk1st                  = $5.lptkCL1st;
                                                                    $6.uCLIndex                 =
                                                                    $6.lptkCur->tkData.uCLIndex = $5.uCLIndex;

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $6;
                                                                }
  | IF NS  OrIfAndIf                      CSCLCSRec EndIf       {DbgMsgWP (L"%%IfStmt:  IF NS OrIfAndIf CSCLCSRec EndIf");
                                                                    // Ensure the IF token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // N.B.:  $4 could be empty

                                                                    // Chain together the IF token and the first token in OrIfAndIf
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $3.lptkIF1st);

                                                                    // Chain together the last token in OrIfAndIf and the EndIf token
                                                                    CS_ChainTokens (lpcsLocalVars, &$3.lptkCur->tkData, $5.lptkIF1st);

                                                                    // In case there's an unmatched ContinueLeave in the IfBody,
                                                                    //   pass on the ptr and index
                                                                    $5.lptkIF1st                = $1.lptkIF1st;
                                                                    $5.lptkCL1st                =
                                                                    $5.lptk1st                  = $4.lptkCL1st;
                                                                    $5.uCLIndex                 =
                                                                    $5.lptkCur->tkData.uCLIndex = $4.uCLIndex;

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $5;
                                                                }
  | IF NS  OrIfAndIf CSCLCSRec ElseIfRec  CSCLCSRec EndIf       {DbgMsgWP (L"%%IfStmt:  IF NS OrIfAndIf CSCLCSRec ElseIfRec CSCLCSRec EndIf");
                                                                    // Ensure the IF token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // N.B.:  $4 and $6 could be empty

                                                                    // If $4 has an unmatched ContinueLeave, ...
                                                                    if ($4.lptkCL1st)
                                                                    {
                                                                        // If $6 has an unmatched ContinueLeave, ...
                                                                        if ($6.lptkCL1st)
                                                                        // Loop through $6's unmatched ContinueLeave
                                                                        for (lptk1st = $6.lptkCL1st; lptk1st <= $6.lptkCur; lptk1st++)
                                                                        // If it's a ContinueLeave, ...
                                                                        if (lptk1st->tkData.uCLIndex EQ $6.uCLIndex)
                                                                        {
                                                                            Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUEIF
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                            // If it's not ENDIF, ...
                                                                            if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                                // Convert to $4's CLIndex so they are all the same
                                                                                lptk1st->tkData.uCLIndex = $4.uCLIndex;
                                                                        } // End IF/FOR/IF

                                                                        // Copy the ContinueLeave index
                                                                        $6.uCLIndex                 = $4.uCLIndex;
                                                                        if ($6.lptkCur)
                                                                            $6.lptkCur->tkData.uCLIndex = $4.uCLIndex;

                                                                        // Move the ptr to encompass $1's ContinueLeave
                                                                        $6.lptkCL1st                =
                                                                        $6.lptk1st                  = $4.lptkCL1st;
                                                                    } // End IF

                                                                    // Chain together the IF token and the first token in OrIfAndIf
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $3.lptkIF1st);

                                                                    // Chain together the last token in OrIfAndIf and the first token in ElseIfRec
                                                                    CS_ChainTokens (lpcsLocalVars, &$3.lptkCur->tkData, $5.lptkIF1st);

                                                                    // Chain together the last token in ElseIfRec and the EndIf token
                                                                    CS_ChainTokens (lpcsLocalVars, &$5.lptkCur->tkData, $7.lptkIF1st);

                                                                    // In case there's an unmatched ContinueLeave in the IfBody,
                                                                    //   pass on the ptr and index
                                                                    $7.lptkIF1st                = $1.lptkIF1st;
                                                                    $7.lptkCL1st                =
                                                                    $7.lptk1st                  = $6.lptkCL1st;
                                                                    $7.uCLIndex                 =
                                                                    $7.lptkCur->tkData.uCLIndex = $6.uCLIndex;

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $7;
                                                                }
  | IF NS  OrIfAndIf CSCLCSRec ElseIfElse CSCLCSRec EndIf       {DbgMsgWP (L"%%IfStmt:  IF NS OrIfAndIf CSCLCSRec ElseIfElse CSCLCSRec EndIf");
                                                                    // Ensure the IF token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // N.B.:  $4 and $6 could be empty

                                                                    // If $4 has an unmatched ContinueLeave, ...
                                                                    if ($4.lptkCL1st)
                                                                    {
                                                                        // If $6 has an unmatched ContinueLeave, ...
                                                                        if ($6.lptkCL1st)
                                                                        // Loop through $6's unmatched ContinueLeave
                                                                        for (lptk1st = $6.lptkCL1st; lptk1st <= $6.lptkCur; lptk1st++)
                                                                        // If it's a ContinueLeave, ...
                                                                        if (lptk1st->tkData.uCLIndex EQ $6.uCLIndex)
                                                                        {
                                                                            Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUEIF
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                            // If it's not ENDIF, ...
                                                                            if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                                // Convert to $4's CLIndex so they are all the same
                                                                                lptk1st->tkData.uCLIndex = $4.uCLIndex;
                                                                        } // End IF/FOR/IF

                                                                        // Copy the ContinueLeave index
                                                                        $6.uCLIndex                 = $4.uCLIndex;
                                                                        if ($6.lptkCur)
                                                                            $6.lptkCur->tkData.uCLIndex = $4.uCLIndex;

                                                                        // Move the ptr to encompass $1's ContinueLeave
                                                                        $6.lptkCL1st                =
                                                                        $6.lptk1st                  = $4.lptkCL1st;
                                                                    } // End IF

                                                                    // Chain together the IF token and the first token in OrIfAndIf
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $3.lptkIF1st);

                                                                    // Chain together the last token in OrIfAndIf and the first token in ElseIfElse
                                                                    CS_ChainTokens (lpcsLocalVars, &$3.lptkCur->tkData, $5.lptkIF1st);

                                                                    // Chain together the last token in ElseIfElse and the EndIf token
                                                                    CS_ChainTokens (lpcsLocalVars, &$5.lptkCur->tkData, $7.lptkIF1st);

                                                                    // In case there's an unmatched ContinueLeave in the IfBody,
                                                                    //   pass on the ptr and index
                                                                    $7.lptkIF1st                = $1.lptkIF1st;
                                                                    $7.lptkCL1st                =
                                                                    $7.lptk1st                  = $6.lptkCL1st;
                                                                    $7.uCLIndex                 =
                                                                    $7.lptkCur->tkData.uCLIndex = $6.uCLIndex;

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $7;
                                                                }
  ;

Until:
    UNTIL NSS                                                   {DbgMsgWP (L"%%Until:  UNTIL NSS");
                                                                    // Ensure the UNTIL token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  | UNTIL NS  OrIfAndIf                                         {DbgMsgWP (L"%%Until:  UNTIL NS OrIfAndIf");
                                                                    // Ensure the UNTIL token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Chain together the UNTIL token and the first token in OrIfAndIf
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $3.lptkIF1st);

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    $3.lptkIF1st = $1.lptkIF1st;

                                                                    $$ = $3;
                                                                }
  ;

RepeatStmt:
    REPEAT SOSStmts CSCLCSRec EndRepeat                         {DbgMsgWP (L"%%RepeatStmt:  REPEAT SOSStmts CSCLCSRec EndRepeat");
                                                                    // Ensure the REPEAT token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // N.B.:  $3 could be empty

                                                                    // If $3 has an unmatched ContinueLeave, ...
                                                                    if ($3.lptkCL1st)
                                                                    // Loop through the ContinueLeave tokens
                                                                    for (lptk1st = $3.lptk1st; lptk1st <= $3.lptkCur; lptk1st++)
                                                                    // If it's a ContinueLeave, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $3.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                        // If it's not ENDIF, ...
                                                                        if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                            // Chain together the LEAVE/CONTINUE token and the EndRepeat token
                                                                            CS_ChainTokens (lpcsLocalVars, &lptk1st->tkData, $4.lptk1st);
                                                                    } // End IF/FOR/IF

                                                                    // Chain together the EndRepeat token and the REPEAT token
                                                                    CS_ChainTokens (lpcsLocalVars, &$4.lptkCur->tkData, $1.lptk1st);

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $4;
                                                                }
  | REPEAT SOSStmts CSCLCSRec Until                             {DbgMsgWP (L"%%RepeatStmt:  REPEAT SOSStmts CSCLCSRec Until");
                                                                    // Ensure the REPEAT token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // N.B.:  $3 could be empty

                                                                    // If $3 has an unmatched ContinueLeave, ...
                                                                    if ($3.lptkCL1st)
                                                                    // Loop through $3's unmatched ContinueLeave
                                                                    for (lptk1st = $3.lptkCL1st; lptk1st <= $3.lptkCur; lptk1st++)
                                                                    // If it's a ContinueLeave, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $3.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                        // If it's not ENDIF, ...
                                                                        if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                            // Chain together the LEAVE/CONTINUE token and the 1st token in Until
                                                                            CS_ChainTokens (lpcsLocalVars, &lptk1st->tkData, $4.lptk1st);
                                                                    } // End IF/FOR/IF

                                                                    // Chain together the last token in Until and the REPEAT token
                                                                    CS_ChainTokens (lpcsLocalVars, &$4.lptkCur->tkData, $1.lptk1st);

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $4;
                                                                }
  ;

ReturnStmt:
    RETURN SOSStmts                                             {DbgMsgWP (L"%%ReturnStmt:  RETURN SOSStmts");
                                                                    // Ensure the RETURN token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  ;

SkipCase:
    SKIPCASE SOS                                                {DbgMsgWP (L"%%SkipCase:  SKIPCASE SOS");
                                                                    $$ = $1;
                                                                }
  ;

CCListRec:
                    SkipCase CASE     NSS                       {DbgMsgWP (L"%%CCListRec:  SkipCase CASE NSS");
                                                                    // Ensure the CASE token is SOS
                                                                    if (!$2.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Set the next .uCLIndex value
                                                                    CS_SetCLIndex (&$2);

                                                                    // Chain together the SkipCase token and the CASE token
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $2.lptk1st);

                                                                    $$ = $2;
                                                                }
  |                 SkipCase CASELIST NSS                       {DbgMsgWP (L"%%CCListRec:  SkipCase CASELIST NSS");
                                                                    // Ensure the CASELIST token is SOS
                                                                    if (!$2.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Set the next .uCLIndex value
                                                                    CS_SetCLIndex (&$2);

                                                                    // Chain together the SkipCase token and the CASELIST token
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $2.lptk1st);

                                                                    $$ = $2;
                                                                }
  | CCListRec       SkipCase CASE     NSS                       {DbgMsgWP (L"%%CCListRec:  CCListRec SkipCase CASE NSS");
                                                                    // Ensure the CASE token is SOS
                                                                    if (!$3.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$3.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Chain together the SkipCase token and the CASE token
                                                                    CS_ChainTokens (lpcsLocalVars, &$2.lptkCur->tkData, $3.lptk1st);

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    $3.lptk1st                  = $1.lptk1st;
                                                                    $3.uCLIndex                 =
                                                                    $3.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $3;
                                                                }
  | CCListRec CSRec SkipCase CASE     NSS                       {DbgMsgWP (L"%%CCListRec:  CCListRec CSRec SkipCase CASE NSS");
                                                                    // Ensure the CASE token is SOS
                                                                    if (!$4.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$4.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // If $2 has an unmatched CONTINUE, ...
                                                                    if ($2.lptkCL1st)
                                                                    // Loop through $2's unmatched Continue
                                                                    for (lptk1st = $2.lptk1st; lptk1st <= $2.lptkCur; lptk1st++)
                                                                    // If it's in the same sequence, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $2.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUEIF);

                                                                        // If it's CONTINUE or CONTINUEIF, ...
                                                                        if (lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                         || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUEIF)
                                                                        {
                                                                            lpcsLocalVars->tkCSErr = *lptk1st;
                                                                            YYERROR;
                                                                        } // End IF

                                                                        // Mark with same index as $1's
                                                                        lptk1st->tkData.uCLIndex = $1.uCLIndex;
                                                                    } // End IF/FOR/IF

                                                                    // Chain together the SkipCase token and the CASE token
                                                                    CS_ChainTokens (lpcsLocalVars, &$3.lptkCur->tkData, $4.lptk1st);

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    $4.lptk1st                  = $1.lptk1st;
                                                                    $4.uCLIndex                 =
                                                                    $4.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $4;
                                                                }
  | CCListRec       SkipCase CASELIST NSS                       {DbgMsgWP (L"%%CCListRec:  CCListRec SkipCase CASELIST NSS");
                                                                    // Ensure the CASELIST token is SOS
                                                                    if (!$3.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$3.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Chain together the SkipCase token and the CASELIST token
                                                                    CS_ChainTokens (lpcsLocalVars, &$2.lptkCur->tkData, $3.lptk1st);

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    $3.lptk1st                  = $1.lptk1st;
                                                                    $3.uCLIndex                 =
                                                                    $3.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $3;
                                                                }
  | CCListRec CSRec SkipCase CASELIST NSS                       {DbgMsgWP (L"%%CCListRec:  CCListRec CSRec SkipCase CASELIST NSS");
                                                                    // Ensure the CASELIST token is SOS
                                                                    if (!$4.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$4.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // If $2 has an unmatched CONTINUE, ...
                                                                    if ($2.lptk1st)
                                                                    // Loop through $2's unmatched Continue
                                                                    for (lptk1st = $2.lptk1st; lptk1st <= $2.lptkCur; lptk1st++)
                                                                    // If it's in the same sequence, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $2.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUEIF);

                                                                        // If it's CONTINUE or CONTINUEIF, ...
                                                                        if (lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                         || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUEIF)
                                                                        {
                                                                            lpcsLocalVars->tkCSErr = *lptk1st;
                                                                            YYERROR;
                                                                        } // End IF

                                                                        // Mark with same index as $1's
                                                                        lptk1st->tkData.uCLIndex = $1.uCLIndex;
                                                                    } // End IF/FOR/IF

                                                                    // Chain together the SkipCase token and the CASELIST token
                                                                    CS_ChainTokens (lpcsLocalVars, &$3.lptkCur->tkData, $4.lptk1st);

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    $4.lptk1st                  = $1.lptk1st;
                                                                    $4.uCLIndex                 =
                                                                    $4.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $4;
                                                                }
  ;

CCListCS:
    /* Empty */                                                 {DbgMsgWP (L"%%CCListCS:  <empty>");
                                                                    ZeroMemory (&$$, sizeof ($$));
                                                                }
  | CCListRec CSCLCSRec                                         {DbgMsgWP (L"%%CCListCS:  CCListRec CSCLCSRec");
                                                                    // N.B.:  $2 could be empty

                                                                    // If $2 has an unmatched CONTINUE, ...
                                                                    if ($2.lptk1st)
                                                                    // Loop through $2's unmatched Continue
                                                                    for (lptk1st = $2.lptk1st; lptk1st <= $2.lptkCur; lptk1st++)
                                                                    // If it's in the same sequence, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $2.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUEIF);

                                                                        // If it's CONTINUE or CONTINUEIF, ...
                                                                        if (lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                         || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUEIF)
                                                                        {
                                                                            lpcsLocalVars->tkCSErr = *lptk1st;
                                                                            YYERROR;
                                                                        } // End IF

                                                                        // Mark with same index as $1's
                                                                        lptk1st->tkData.uCLIndex = $1.uCLIndex;
                                                                    } // End IF/FOR/IF

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    if ($2.lptkCur)
                                                                    {
                                                                        $2.lptk1st                  = $1.lptk1st;
                                                                        $2.uCLIndex                 =
                                                                        $2.lptkCur->tkData.uCLIndex = $1.uCLIndex;
                                                                        $$ = $2;
                                                                    } else
                                                                        $$ = $1;
                                                                }
  ;

SelectStmt:
    SELECT NS CCListCS                     EndSelect            {DbgMsgWP (L"%%SelectStmt:  SELECT NS CCListCS EndSelect");
                                                                    // Ensure the SELECT token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Save as the ptr to the previous token
                                                                    lptkPrv = $1.lptk1st;

                                                                    // If $3 has an unmatched CONTINUE, ...
                                                                    if ($3.lptk1st)
                                                                    // Loop through the tokens in CCListCS
                                                                    for (lptk1st = $3.lptk1st; lptk1st <= $3.lptkCur; lptk1st++)
                                                                    // If it's in the same sequence, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $3.uCLIndex)
                                                                    {
                                                                        // The following commented out code is a bandaid covering the
                                                                        //   fact that the CS parsing code doesn't handle certain cases
                                                                        //   such as an :if ... :end stmt in a :case stmt.
////////////////////////////////////////////////////////////////////////Assert (lptk1st->tkFlags.TknType EQ TKT_CS_CASE
////////////////////////////////////////////////////////////////////////     || lptk1st->tkFlags.TknType EQ TKT_CS_CASELIST
////////////////////////////////////////////////////////////////////////     || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
////////////////////////////////////////////////////////////////////////     || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF);
////////////////////////////////////////////////////////////////////////
                                                                        // If it's LEAVE or LEAVEIF, ...
                                                                        if (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                         || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF)
                                                                            // Chain together lptk1st and the EndSelect token
                                                                            CS_ChainTokens (lpcsLocalVars, &lptk1st->tkData, $4.lptk1st);
                                                                        else
                                                                        // It's CASE or CASELIST
                                                                        {
                                                                            // Chain together lptkPrv and lptk1st
                                                                            CS_ChainTokens (lpcsLocalVars, &lptkPrv->tkData, lptk1st);

                                                                            // Save as the ptr to the previous token
                                                                            lptkPrv = lptk1st;
                                                                        } // End IF
                                                                    } // End IF/FOR/IF

                                                                    // Chain together the last token in CCListCS and the EndSelect token
                                                                    CS_ChainTokens (lpcsLocalVars, &$3.lptkCur->tkData, $4.lptk1st);

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $4;
                                                                }
  | SELECT NS CCListCS ELSE SOSStmts       EndSelect            {DbgMsgWP (L"%%SelectStmt:  SELECT NS CCListCS ELSE SOSStmts EndSelect");
                                                                    // Ensure the SELECT token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Ensure the ELSE token is SOS
                                                                    if (!$4.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$4.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Save as the ptr to the previous token
                                                                    lptkPrv = $1.lptk1st;

                                                                    // Loop through the tokens in CCListCS
                                                                    if ($3.lptk1st)
                                                                    // Loop through $3's unmatched Continue
                                                                    for (lptk1st = $3.lptk1st; lptk1st <= $3.lptkCur; lptk1st++)
                                                                    // If it's in the same sequence, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $3.uCLIndex)
                                                                    {
                                                                        // The following commented out code is a bandaid covering the
                                                                        //   fact that the CS parsing code doesn't handle certain cases
                                                                        //   such as an :if ... :end stmt in a :case stmt.
////////////////////////////////////////////////////////////////////////Assert (lptk1st->tkFlags.TknType EQ TKT_CS_CASE
////////////////////////////////////////////////////////////////////////     || lptk1st->tkFlags.TknType EQ TKT_CS_CASELIST
////////////////////////////////////////////////////////////////////////     || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
////////////////////////////////////////////////////////////////////////     || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF);
////////////////////////////////////////////////////////////////////////
                                                                        // If it's LEAVE or LEAVEIF, ...
                                                                        if (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                         || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF)
                                                                            // Chain together lptk1st and the EndSelect token
                                                                            CS_ChainTokens (lpcsLocalVars, &lptk1st->tkData, $6.lptk1st);
                                                                        else
                                                                        // It's CASE or CASELIST
                                                                        {
                                                                            // Chain together lptkPrv and lptk1st
                                                                            CS_ChainTokens (lpcsLocalVars, &lptkPrv->tkData, lptk1st);

                                                                            // Save as the ptr to the previous token
                                                                            lptkPrv = lptk1st;
                                                                        } // End IF
                                                                    } // End IF/FOR/IF

                                                                    // Chain together the last token in CCListCS and the ELSE token
                                                                    CS_ChainTokens (lpcsLocalVars, &$3.lptkCur->tkData, $4.lptk1st);

                                                                    // Chain together the ELSE token and the EndSelect token
                                                                    CS_ChainTokens (lpcsLocalVars, &$4.lptkCur->tkData, $6.lptk1st);

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $6;
                                                                }
  | SELECT NS CCListCS ELSE SOSStmts CSRec EndSelect            {DbgMsgWP (L"%%SelectStmt:  SELECT NS CCListCS ELSE SOSStmts CSRec EndSelect");
                                                                    // Ensure the SELECT token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Ensure the ELSE token is SOS
                                                                    if (!$4.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$4.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // If $5 has an unmatched ContinueLeave, ...
                                                                    if ($5.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$5.lptkCL1st;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Save as the ptr to the previous token
                                                                    lptkPrv = $1.lptk1st;

                                                                    // If $3 has an unmatched CONTINUE, ...
                                                                    if ($3.lptk1st)
                                                                    // Loop through the tokens in CCListCS
                                                                    for (lptk1st = $3.lptk1st; lptk1st <= $3.lptkCur; lptk1st++)
                                                                    // If it's in the same sequence, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $3.uCLIndex)
                                                                    {
                                                                        // The following commented out code is a bandaid covering the
                                                                        //   fact that the CS parsing code doesn't handle certain cases
                                                                        //   such as an :if ... :end stmt in a :case stmt.
////////////////////////////////////////////////////////////////////////Assert (lptk1st->tkFlags.TknType EQ TKT_CS_CASE
////////////////////////////////////////////////////////////////////////     || lptk1st->tkFlags.TknType EQ TKT_CS_CASELIST
////////////////////////////////////////////////////////////////////////     || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
////////////////////////////////////////////////////////////////////////     || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF);
////////////////////////////////////////////////////////////////////////
                                                                        // If it's LEAVE or LEAVEIF, ...
                                                                        if (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                         || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF)
                                                                            // Chain together lptk1st and the EndSelect token
                                                                            CS_ChainTokens (lpcsLocalVars, &lptk1st->tkData, $7.lptk1st);
                                                                        else
                                                                        // It's CASE or CASELIST
                                                                        {
                                                                            // Chain together lptkPrv and lptk1st
                                                                            CS_ChainTokens (lpcsLocalVars, &lptkPrv->tkData, lptk1st);

                                                                            // Save as the ptr to the previous token
                                                                            lptkPrv = lptk1st;
                                                                        } // End IF
                                                                    } // End IF/FOR/IF

                                                                    // Chain together the last token in CCListCS and the ELSE token
                                                                    CS_ChainTokens (lpcsLocalVars, &$3.lptkCur->tkData, $4.lptk1st);

                                                                    // Chain together the ELSE token and the EndSelect token
                                                                    CS_ChainTokens (lpcsLocalVars, &$4.lptkCur->tkData, $7.lptk1st);

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $7;
                                                                }
  ;

WhileStmt:
    WHILE NSS           CSCLCSRec EndWhile                      {DbgMsgWP (L"%%WhileStmt:  WHILE NSS CSCLCSRec EndWhile");
                                                                    // Ensure the WHILE token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // N.B.:  $3 could be empty

                                                                    // If $3 has an unmatched ContinueLeave, ...
                                                                    if ($3.lptkCL1st)
                                                                    // Loop through the ContinueLeave tokens
                                                                    for (lptk1st = $3.lptk1st; lptk1st <= $3.lptkCur; lptk1st++)
                                                                    // If it's a ContinueLeave, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $3.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                        // If it's not ENDIF, ...
                                                                        if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                            // Chain together the LEAVE/CONTINUE token and the EndWhile token
                                                                            CS_ChainTokens (lpcsLocalVars, &lptk1st->tkData, $4.lptk1st);
                                                                    } // End IF/FOR/IF

                                                                    // Copy the WHILE token as the next call to CS_ChainTokens clobbers it
                                                                    tkTmp = *$1.lptkIF1st;

                                                                    // Chain together the WHILE token and the EndWhile token
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $4.lptkIF1st);

                                                                    // Chain together the EndWhile token and the WHILE token
                                                                    CS_ChainTokens (lpcsLocalVars, &$4.lptkCur->tkData, &tkTmp);

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $4;
                                                                }
  | WHILE NS  OrIfAndIf CSCLCSRec EndWhile                      {DbgMsgWP (L"%%WhileStmt:  WHILE NS OrIfAndIf CSCLCSRec EndWhile");
                                                                    // Ensure the WHILE token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // N.B.:  $4 could be empty

                                                                    // If $4 has an unmatched ContinueLeave, ...
                                                                    if ($4.lptkCL1st)
                                                                    // Loop through $4's unmatched ContinueLeave
                                                                    for (lptk1st = $4.lptkCL1st; lptk1st <= $4.lptkCur; lptk1st++)
                                                                    // If it's a ContinueLeave, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $4.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                        // If it's not ENDIF, ...
                                                                        if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                            // Chain together the LEAVE/CONTINUE token and the EndWhile token
                                                                            CS_ChainTokens (lpcsLocalVars, &lptk1st->tkData, $5.lptk1st);
                                                                    } // End IF/FOR/IF

                                                                    // Copy the WHILE token as the next call to CS_ChainTokens clobbers it
                                                                    tkTmp = *$1.lptkIF1st;

                                                                    // Save as the ptr to the previous token
                                                                    lptkPrv = $1.lptk1st;

                                                                    // If $3 has an unmatched CONTINUE, ...
                                                                    if ($3.lptk1st)
                                                                    // Loop through the tokens in OrIfAndIf
                                                                    for (lptk1st = $3.lptk1st; lptk1st <= $3.lptkCur; lptk1st++)
                                                                    if (lptk1st->tkFlags.TknType EQ TKT_CS_ANDIF
                                                                     || lptk1st->tkFlags.TknType EQ TKT_CS_ORIF)
                                                                    {
                                                                        // Chain together lptkPrv and lptk1st
                                                                        CS_ChainTokens (lpcsLocalVars, &lptkPrv->tkData, lptk1st);

                                                                        // Save as the ptr to the previous token
                                                                        lptkPrv = lptk1st;
                                                                    } // End IF/FOR/IF

                                                                    // Chain together the last token in OrIfAndIf and the EndWhile token
                                                                    CS_ChainTokens (lpcsLocalVars, &$3.lptkCur->tkData, $5.lptkIF1st);

                                                                    // Chain together the EndWhile token and the WHILE token
                                                                    CS_ChainTokens (lpcsLocalVars, &$5.lptkCur->tkData, &tkTmp);

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $5;
                                                                }
  | WHILE NSS           CSCLCSRec Until                         {DbgMsgWP (L"%%WhileStmt:  WHILE NSS CSCLCSRec Until");
                                                                    // Ensure the WHILE token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // N.B.:  $3 could be empty

                                                                    // If $3 has an unmatched ContinueLeave, ...
                                                                    if ($3.lptkCL1st)
                                                                    // Loop through $3's unmatched ContinueLeave
                                                                    for (lptk1st = $3.lptkCL1st; lptk1st <= $3.lptkCur; lptk1st++)
                                                                    // If it's a ContinueLeave, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $3.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                        // If it's not ENDIF, ...
                                                                        if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                            // Chain together the LEAVE/CONTINUE token and the 1st token in Until
                                                                            CS_ChainTokens (lpcsLocalVars, &lptk1st->tkData, $4.lptk1st);
                                                                    } // End IF/FOR/IF

                                                                    // Copy the WHILE token as the next call to CS_ChainTokens clobbers it
                                                                    tkTmp = *$1.lptkIF1st;

                                                                    // Chain together the WHILE token and the 1st token in Until
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $4.lptkIF1st);

                                                                    // Chain together the last token in Until and the WHILE token
                                                                    CS_ChainTokens (lpcsLocalVars, &$4.lptkCur->tkData, &tkTmp);

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $4;
                                                                }
  | WHILE NS  OrIfAndIf CSCLCSRec Until                         {DbgMsgWP (L"%%WhileStmt:  WHILE NS OrIfAndIf CSCLCSRec Until");
                                                                    // Ensure the WHILE token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // N.B.:  $4 could be empty

                                                                    // If $4 has an unmatched ContinueLeave, ...
                                                                    if ($4.lptkCL1st)
                                                                    // Loop through $4's unmatched ContinueLeave
                                                                    for (lptk1st = $4.lptkCL1st; lptk1st <= $4.lptkCur; lptk1st++)
                                                                    // If it's a ContinueLeave, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $4.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_LEAVEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                        // If it's not ENDIF, ...
                                                                        if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                            // Chain together the LEAVE/CONTINUE token and the 1st token in Until
                                                                            CS_ChainTokens (lpcsLocalVars, &lptk1st->tkData, $5.lptk1st);
                                                                    } // End IF/FOR/IF

                                                                    // Copy the WHILE token as the next call to CS_ChainTokens clobbers it
                                                                    tkTmp = *$1.lptkIF1st;

                                                                    // Save as the ptr to the previous token
                                                                    lptkPrv = $1.lptkCur;

                                                                    // If $3 has an unmatched CONTINUE, ...
                                                                    if ($3.lptk1st)
                                                                    // Loop through the tokens in OrIfAndIf
                                                                    for (lptk1st = $3.lptk1st; lptk1st <= $3.lptkCur; lptk1st++)
                                                                    if (lptk1st->tkFlags.TknType EQ TKT_CS_ANDIF
                                                                     || lptk1st->tkFlags.TknType EQ TKT_CS_ORIF)
                                                                    {
                                                                        // Chain together lptkPrv and lptk1st
                                                                        CS_ChainTokens (lpcsLocalVars, &lptkPrv->tkData, lptk1st);

                                                                        // Save as the ptr to the previous token
                                                                        lptkPrv = lptk1st;
                                                                    } // End IF/FOR/IF

                                                                    // Chain together the last token in OrIfAndIf and the first token in Until
                                                                    CS_ChainTokens (lpcsLocalVars, &$3.lptkCur->tkData, $5.lptkIF1st);

                                                                    // Chain together the last token in Until and the WHILE token
                                                                    CS_ChainTokens (lpcsLocalVars, &$5.lptkCur->tkData, &tkTmp);

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $5;
                                                                }
  ;

%%

//***************************************************************************
//  Start of C program
//***************************************************************************

//***************************************************************************
//  $ParseCtrlStruc_EM
//
//  Parse function Control Structures
//***************************************************************************

UBOOL ParseCtrlStruc_EM
    (LPCSLOCALVARS lpcsLocalVars)       // Ptr to Ctrl Struc Local vars

{
    UBOOL bRet;                         // TRUE iff result is valid

    __try
    {
        // Clear the error token
        ZeroMemory (&lpcsLocalVars->tkCSErr, sizeof (lpcsLocalVars->tkCSErr));

        // Append the CS_EOL token to the CS stack
        //   to allow for later parsing for SYNTAX ERRORs
        AppendNewCSToken_EM (TKT_CS_EOL,
                             lpcsLocalVars->lpMemPTD,
                             0,
                             0,
                             0,
                             TRUE,
                             NEG1U);
        // ***FIXME*** -- Use VirtualAlloc for the parser stack














#if YYDEBUG
        // Enable debugging
        yydebug = TRUE;
#endif

        // Parse the CS stack, check for errors
        //   0 = success
        //   1 = YYABORT or APL error
        //   2 = memory exhausted
        switch (cs_yyparse (lpcsLocalVars))
        {
            case 0:
                lpcsLocalVars->lpwszErrMsg = NULL;
                bRet = TRUE;

                break;

            case 1:
                lpcsLocalVars->lpwszErrMsg = L"CS " ERRMSG_SYNTAX_ERROR;
                bRet = FALSE;

                break;

            case 2:
                lpcsLocalVars->lpwszErrMsg = L"CS " ERRMSG_WS_FULL;
                bRet = FALSE;

                break;

            defstop
                break;
        } // End SWITCH
#if YYDEBUG
        // Disable debugging
        yydebug = FALSE;
#endif
    } __except (CheckException (GetExceptionInformation (), L"ParseCtrlStruc"))
    {
        // Split cases based upon the ExceptionCode
        switch (MyGetExceptionCode ())
        {
            case EXCEPTION_STACK_OVERFLOW:
                // Set the error message
                lpcsLocalVars->lpwszErrMsg = L"CS " ERRMSG_STACK_FULL;

                break;

            default:
                // Display message for unhandled exception
                DisplayException ();

                break;
        } // End SWITCH

        // Mark as in error
        bRet = FALSE;
    } // End __try/__except

    return bRet;
} // End ParseCtrlStruc_EM


//***************************************************************************
//  $cs_yylex
//
//  Lexical analyzer for Bison
//***************************************************************************

int cs_yylex
    (LPCS_YYSTYPE  lpYYLval,            // Ptr to yylval
     LPCSLOCALVARS lpcsLocalVars)       // Ptr to Ctrl Struc Local vars

{
    LPTOKEN     lptkCSCur;              // Ptr to current token (the one we return)
    TOKEN_TYPES TknType;                // Token type of current token

CS_YYLEX_START:
    // Save the ptr to the current CS_YYSTYPE and skip over it
    lptkCSCur = lpcsLocalVars->lptkCSNxt++;

    // Return the current token
    lpYYLval->lptkCur   =               // Mark as the current token
    lpYYLval->lptkIF1st =               // Mark as the first in a sequence of IF/ELSE/.../ENDIF
    lpYYLval->lptk1st   =               // Mark as the first in a sequence
    lpYYLval->lptkNxt   = lptkCSCur;    // ...         next  ...
    lpYYLval->lptkCL1st = NULL;         // No ContinueLeave ptr
    lpYYLval->uCLIndex  = 0;            // No ContinueLeave index

    // Split cases based upon the token type
    switch (lptkCSCur->tkFlags.TknType)
    {
        case TKT_CS_EOL:
            return '\0';

        case TKT_CS_ENS:
            return ENS;

        case TKT_EOS:
        case TKT_EOL:
            // If the next token is LABELSEP, check the token after that
            if (lpcsLocalVars->lptkCSNxt->tkFlags.TknType EQ TKT_LABELSEP)
                TknType = lpcsLocalVars->lptkCSNxt[1].tkFlags.TknType;
            else
                TknType = lpcsLocalVars->lptkCSNxt[0].tkFlags.TknType;

            // If the next token of interest is not EOS, INFOR, NEC, or SOS,
            //   then skip this token and return the next token.
            // This avoids having to insert an EOS before almost every token.
            if (TknType EQ TKT_CS_ANDIF
             || TknType EQ TKT_CS_ASSERT
             || TknType EQ TKT_CS_CASE
             || TknType EQ TKT_CS_CASELIST
             || TknType EQ TKT_CS_CONTINUE
             || TknType EQ TKT_CS_CONTINUEIF
             || TknType EQ TKT_CS_ELSE
             || TknType EQ TKT_CS_ELSEIF
             || TknType EQ TKT_CS_END
             || TknType EQ TKT_CS_ENDFOR
             || TknType EQ TKT_CS_ENDFORLCL
             || TknType EQ TKT_CS_ENDIF
             || TknType EQ TKT_CS_ENDREPEAT
             || TknType EQ TKT_CS_ENDSELECT
             || TknType EQ TKT_CS_ENDWHILE
             || TknType EQ TKT_CS_FOR
             || TknType EQ TKT_CS_FORLCL
             || TknType EQ TKT_CS_GOTO
             || TknType EQ TKT_CS_IF
             || TknType EQ TKT_CS_LEAVE
             || TknType EQ TKT_CS_LEAVEIF
             || TknType EQ TKT_CS_ORIF
             || TknType EQ TKT_CS_REPEAT
             || TknType EQ TKT_CS_RETURN
             || TknType EQ TKT_CS_SELECT
             || TknType EQ TKT_CS_SKIPCASE
             || TknType EQ TKT_CS_SKIPEND
             || TknType EQ TKT_CS_UNTIL
             || TknType EQ TKT_CS_WHILE)
                goto CS_YYLEX_START;

            return EOS;

        case TKT_SOS:
            return SOS;

        case TKT_CS_ANDIF:                  // Control structure:  ANDIF     (Data is Line/Stmt #)
            return ANDIF;

        case TKT_CS_ASSERT:                 // ...                 ASSERT
            return ASSERT;

        case TKT_CS_CASE:                   // ...                 CASE
            return CASE;

        case TKT_CS_CASELIST:               // ...                 CASELIST
            return CASELIST;

        case TKT_CS_CONTINUE:               // ...                 CONTINUE
            return CONTINUE;

        case TKT_CS_CONTINUEIF:             // ...                 CONTINUEIF
            return CONTINUEIF;

        case TKT_CS_ELSE:                   // ...                 ELSE
            return ELSE;

        case TKT_CS_ELSEIF:                 // ...                 ELSEIF
            return ELSEIF;

        case TKT_CS_END:                    // ...                 END
            return END;

        case TKT_CS_ENDFOR:                 // ...                 ENDFOR
            return ENDFOR;

        case TKT_CS_ENDFORLCL:              // ...                 ENDFORLCL
            return ENDFORLCL;

        case TKT_CS_ENDIF:                  // ...                 ENDIF
            return ENDIF;

        case TKT_CS_ENDREPEAT:              // ...                 ENDREPEAT
            return ENDREPEAT;

        case TKT_CS_ENDSELECT:              // ...                 ENDSELECT
            return ENDSELECT;

        case TKT_CS_ENDWHILE:               // ...                 ENDWHILE
            return ENDWHILE;

        case TKT_CS_FOR:                    // ...                 FOR
            // Mark as a main CS stmt for AFO
            lpcsLocalVars->bMainStmt = TRUE;

            // Set start of stmt
            CS_LinkStmt (lpcsLocalVars, lpYYLval);

            return FOR;

        case TKT_CS_FORLCL:                 // ...                 FORLCL
            // Mark as a main CS stmt for AFO
            lpcsLocalVars->bMainStmt = TRUE;

            // Set start of stmt
            CS_LinkStmt (lpcsLocalVars, lpYYLval);

            return FORLCL;

        case TKT_CS_GOTO:                   // ...                 GOTO
            // Mark as a main CS stmt for AFO
            lpcsLocalVars->bMainStmt = TRUE;

            return GOTO;

        case TKT_CS_IF:                     // ...                 IF
            // Mark as a main CS stmt for AFO
            lpcsLocalVars->bMainStmt = TRUE;

            // Set start of stmt
            CS_LinkStmt (lpcsLocalVars, lpYYLval);

            return IF;

        case TKT_CS_IN:                     // ...                 IN
            return INFOR;

        case TKT_CS_LEAVE:                  // ...                 LEAVE
            return LEAVE;

        case TKT_CS_LEAVEIF:                // ...                 LEAVEIF
            return LEAVEIF;

        case TKT_CS_ORIF:                   // ...                 ORIF
            return ORIF;

        case TKT_CS_NEC:                    // ...                 NEC
            return NEC;

        case TKT_CS_REPEAT:                 // ...                 REPEAT
            // Mark as a main CS stmt for AFO
            lpcsLocalVars->bMainStmt = TRUE;

            // Set start of stmt
            CS_LinkStmt (lpcsLocalVars, lpYYLval);

            return REPEAT;

        case TKT_CS_RETURN:                 // ...                 RETURN
            // Mark as a main CS stmt for AFO
            lpcsLocalVars->bMainStmt = TRUE;

            return RETURN;

        case TKT_CS_SELECT:                 // ...                 SELECT
            // Mark as a main CS stmt for AFO
            lpcsLocalVars->bMainStmt = TRUE;

            // Set start of stmt
            CS_LinkStmt (lpcsLocalVars, lpYYLval);

            return SELECT;

        case TKT_CS_SKIPCASE:               // ...                 Special token
            return SKIPCASE;

        case TKT_CS_SKIPEND:                // ...                 Special token
            return SKIPEND;

        case TKT_CS_UNTIL:                  // ...                 UNTIL
            return UNTIL;

        case TKT_CS_WHILE:                  // ...                 WHILE
            // Mark as a main CS stmt for AFO
            lpcsLocalVars->bMainStmt = TRUE;

            // Set start of stmt
            CS_LinkStmt (lpcsLocalVars, lpYYLval);

            return WHILE;

        case TKT_LABELSEP:
            // If the preceding token is NEC, delete it and ignore this one
            if (lpcsLocalVars->lptkCSNxt[-1].tkFlags.TknType EQ TKT_CS_NEC)
                // Delete the NEC token
                lpcsLocalVars->lptkCSNxt--;

            // Fall through to common code

        case TKT_CS_FOR2:
        case TKT_CS_IF2:
        case TKT_CS_REPEAT2:
        case TKT_CS_SELECT2:
        case TKT_CS_WHILE2:
            goto CS_YYLEX_START;            // Ignore these tokens

        defstop
            return UNK;
    } // End SWITCH
} // End cs_yylex


//***************************************************************************
//  $cs_yyerror
//
//  Error callback from Bison
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- cs_yyerror"
#else
#define APPEND_NAME
#endif

void cs_yyerror                         // Called for Bison syntax error
    (LPCSLOCALVARS lpcsLocalVars,       // Ptr to Ctrl Struc Local vars
     const char   *s)                   // Ptr to error msg

{
    char szTemp[1024], *p;
    UINT uLineNum,
         uLinePos;

#ifdef DEBUG
    DbgMsg ((char *) s);
#endif

    // If the error token is not already set, ...
    if (lpcsLocalVars->tkCSErr.tkFlags.TknType EQ 0)
        lpcsLocalVars->tkCSErr = lpcsLocalVars->lptkCSNxt[-1];

    // Get and save the character index position
    uLinePos = lpcsLocalVars->tkCSErr.tkCharIndex;
    uLineNum = lpcsLocalVars->tkCSErr.tkData.Orig.c.uLineNum;

    // If the caller wants error messages displayed, ...
    if (lpcsLocalVars->bDisplayErr)
    {
        // Check for SYNTAX ERROR
#define ERR     "syntax error"
        strcpyn (szTemp, s, sizeof (ERR));      // Note: Terminates the string, too
        if (lstrcmp (szTemp, ERR) EQ 0)
        {
            wsprintf (szTemp,
                      "SYNTAX ERROR in line %d, position %d -- function NOT saved.",
                      uLineNum,
                      uLinePos);
            p = szTemp;

            goto DISPLAYCAT;
#undef  ERR
        } // End IF

        // Check for VALUE ERROR
#define ERR     "value error"
        strcpyn (szTemp, s, sizeof (ERR));      // Note: Terminates the string, too
        if (lstrcmp (szTemp, ERR) EQ 0)
        {
            wsprintf (szTemp,
                      "VALUE ERROR in line %d, position %d -- function NOT saved.",
                      uLineNum,
                      uLinePos);
            p = szTemp;

            goto DISPLAYCAT;
#undef  ERR
        } // End IF

        // Check for LENGTH ERROR
#define ERR     "length error"
        strcpyn (szTemp, s, sizeof (ERR));      // Note: Terminates the string, too
        if (lstrcmp (szTemp, ERR) EQ 0)
        {
            wsprintf (szTemp,
                      "LENGTH ERROR in line %d, position %d -- function NOT saved.",
                      uLineNum,
                      uLinePos);
            p = szTemp;

            goto DISPLAYCAT;
#undef  ERR
        } // End IF
    } else
        return;

#define ERR     "memory exhausted"
    strcpyn (szTemp, s, sizeof (ERR));      // Note: Terminates the string, too
    if (lstrcmp (szTemp, ERR) EQ 0)
    {
        wsprintf (szTemp,
                  "Insufficient memory -- function NOT saved.");
        p = szTemp;

        goto DISPLAYCAT;
#undef  ERR
    } // End IF

    // Use the error message as given
    p = (char *) s;

    goto DISPLAY;

DISPLAYCAT:
#ifdef DEBUG
    MyStrcat (szTemp, sizeof (szTemp), "(");
    MyStrcat (szTemp, sizeof (szTemp),  s );
    MyStrcat (szTemp, sizeof (szTemp), ")");
#endif
DISPLAY:
    // Display a message box
    MessageBox (hWndMF,
                p,
                lpszAppName,
                MB_OK | MB_ICONWARNING | MB_APPLMODAL);
} // End cs_yyerror
#undef  APPEND_NAME


//***************************************************************************
//  $cs_yyfprintf
//
//  Debugger output
//***************************************************************************

void cs_yyfprintf
    (FILE  *hfile,                      // Ignore this
     LPCHAR lpszFmt,                    // Format string
     ...)                               // Zero or more arguments

{
#if (defined (DEBUG)) && (defined (YYFPRINTF_DEBUG))
    HRESULT  hResult;       // The result of <StringCbVPrintf>
    va_list  vl;
    APLU3264 i1;
    static   char szTemp[256] = {'\0'};

    // Initialize the variable list
    va_start (vl, lpszFmt);

    // Accumulate into local buffer because
    //   Bison calls this function multiple
    //   times for the same line, terminating
    //   the last call for the line with a LF.
    hResult = StringCbVPrintf (&szTemp[lstrlen (szTemp)],
                                sizeof (szTemp),
                                lpszFmt,
                                vl);
    // End the variable list
    va_end (vl);

    // If it failed, ...
    if (FAILED (hResult))
        DbgBrk ();                  // #ifdef DEBUG

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
    } // End IF/ELSE
#endif
} // End cs_yyfprintf


//***************************************************************************
//  End of File: cs_parse.y
//***************************************************************************
