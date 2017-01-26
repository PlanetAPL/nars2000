//***************************************************************************
//  NARS2000 -- Parser Grammar for function headers
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

%{
#define STRICT
#include <windows.h>
#include "headers.h"


////#define YYLEX_DEBUG
////#define YYFPRINTF_DEBUG

#ifdef DEBUG
#define YYERROR_VERBOSE
#define YYDEBUG 1
#define YYFPRINTF               fh_yyfprintf
#define fprintf                 fh_yyfprintf
////#define YYPRINT                 fh_yyprint
void fh_yyprint     (FILE *yyoutput, unsigned short int yytoknum, FH_YYSTYPE const yyvaluep);
#endif

#define YYMALLOC    malloc
#define YYFREE      free

#include "fh_parse.proto"

#define    YYSTYPE     FH_YYSTYPE
#define  LPYYSTYPE   LPFH_YYSTYPE
#define tagYYSTYPE  tagFH_YYSTYPE

// The following #defines are needed to allow multiple parses
//   to coexist in the same file
#define yy_symbol_print         fh_yy_symbol_print
#define yy_symbol_value_print   fh_yy_symbol_value_print
#define yy_reduce_print         fh_yy_reduce_print
#define yydestruct              fh_yydestruct

////#define DbgMsgWP(a)         DbgMsgW(a)
////#define DbgMsgWP(a)         DbgMsgW(a); DbgBrk ()
////#define DbgMsgWP(a)         DbgMsgW(a)
    #define DbgMsgWP(a)

%}

%pure-parser
%name-prefix "fh_yy"
%parse-param {LPFHLOCALVARS lpfhLocalVars}
%lex-param   {LPFHLOCALVARS lpfhLocalVars}
%token NAMEUNK NAMEOPR NAMESYS ASSIGN LINECONT UNK SOS NOMORE

%start HeaderComm

%%

/*

The set of cases we must handle is as follows:

Niladic functions
-----------------

Result
------
    1.  **Empty**
    2.  Z{is}
    3.  (Z1 Z2 ...){is}
    4.  {Z}{is}
    5.  {(Z1 Z2 ...)}{is}
    6.  ({Z1 Z2 ...}){is}

Left Arg
--------
    1.  **Empty**

Fcn
---
    1.  FOO

Right Arg
---------
    1.  **Empty**


Monadic/Dyadic Functions/Operators
----------------------------------

Result
------
    1.  **Empty**
    2.  Z{is}
    3.  (Z1 Z2 ...){is}
    4.  {Z}{is}
    5.  {(Z1 Z2 ...)}{is}
    6.  ({Z1 Z2 ...}){is}


Left Arg
--------
    1.  **Empty**
    2.  L
    3.  (L)
    4.  (L1 L2 ...)
    5.  {L}
    6.  {L1 L2 ...}
    7.  {(L)}
    8.  {(L1 L2 ...)}
    9.  ({L})
   10.  ({L1 L2 ...})

Fcn
---
    1.  FOO
    2.  FOO[X]

Opr
---
    1.  (FOO)
    2.  (LO FOO)
    3.  (LO FOO RO)
    4.  (FOO[X])
    5.  (LO FOO[X])
    6.  (LO FOO[X] RO)

Right Arg
---------
    1.  R
    2.  (R)
    3.  (R1 R2 ...)
                   Z    L   F   R
This yields  360 (=6 x 10 x 2 x 3) distinct Monadic/Dyadic Function headers
and         1080 (=6 x 10 x 6 x 3) distinct Monadic/Dyadic Derived Function headers from Monadic/Dyadic Operators
and            6 (=6 x  1 x 1 x 1) distinct Niladic Function headers
and           36 (=6 x  1 x 6 x 1) distinct Niladic Derived Function headers from Monadic/Dyadic Operators
Total       1482                   user-defined function/operator headers,
not counting the presence/absence of locals and presence/absence of a comment.

 */


AxisOpr:
      NAMEOPR '[' NAMEUNK ']'   {DbgMsgWP (L"%%AxisOpr:  NAMEOPR[NAMEUNK]");
                                 InitHdrStrand (&$1);
                                 PushHdrStrand_YY (&$1);
                                 PushHdrStrand_YY (&$3);
                                 MakeHdrStrand_YY (&$1);
                                 lpfhLocalVars->DfnAxis  = TRUE;
                                 lpfhLocalVars->lpYYAxisOpr = $3.lpYYStrandIndirect;
                                 $$ = $1;
                                }
    ;

OpenList:
               NAMEUNK          {DbgMsgWP (L"%%OpenList:  NAMEUNK");
                                 InitHdrStrand (&$1);
                                 $$ = *PushHdrStrand_YY (&$1);
                                }
    | OpenList NAMEUNK          {DbgMsgWP (L"%%OpenList:  OpenList NAMEUNK");
                                 $$ = *PushHdrStrand_YY (&$2);
                                }
    ;

AxisList:
      '('          AxisOpr          ')'
                                {DbgMsgWP (L"%%AxisList:  (AxisOpr)");
                                 InitHdrStrand (&$2);
                                 $2.Indirect = TRUE;
                                 PushHdrStrand_YY (&$2);
                                 $$ = *MakeHdrStrand_YY (&$2);
                                }
    | '(' NAMEUNK  AxisOpr          ')'
                                {DbgMsgWP (L"%%AxisList:  (NAMEUNK AxisOpr)");
                                 PushHdrStrand_YY (&$2);
                                 $3.Indirect = TRUE;
                                 PushHdrStrand_YY (&$3);
                                 $$ = *MakeHdrStrand_YY (&$2);
                                }
    | '(' NAMEUNK  AxisOpr NAMEUNK ')'
                                {DbgMsgWP (L"%%AxisList:  (NAMEUNK AxisOpr NAMEUNK)");
                                 PushHdrStrand_YY (&$2);
                                 $3.Indirect = TRUE;
                                 PushHdrStrand_YY (&$3);
                                 PushHdrStrand_YY (&$4);
                                 $$ = *MakeHdrStrand_YY (&$2);
                                }
    ;

List:
      '(' OpenList ')'          {DbgMsgWP (L"%%List:  '(' OpenList ')'");
                                 $$ = *MakeHdrStrand_YY (&$2);
                                 $$.List = TRUE;                                    // Set the List bit
                                }
    ;

Result:
      List    ASSIGN            {DbgMsgWP (L"%%Result:  List" WS_UTF16_LEFTARROW);
                                 if ($1.lpYYStrandBase->uStrandLen EQ 1
                                  && $1.List
                                  && !lpfhLocalVars->ParseFcnName)
                                 {
                                     fh_yyerror (lpfhLocalVars, "length error");
                                     YYERROR;
                                 } // End IF

                                 lpfhLocalVars->lpYYResult = $1.lpYYStrandBase;
                                 lpfhLocalVars->ListRes    = $1.List;               // Copy the List bit
                                 $$ = $1;
                                }
    | OptArg  ASSIGN            {DbgMsgWP (L"%%Result:  OptArg" WS_UTF16_LEFTARROW);
                                 if ($1.lpYYStrandBase->uStrandLen EQ 1
                                  && $1.List
                                  && !lpfhLocalVars->ParseFcnName)
                                 {
                                     fh_yyerror (lpfhLocalVars, "length error");
                                     YYERROR;
                                 } // End IF

                                 lpfhLocalVars->lpYYResult = $1.lpYYStrandBase;
                                 lpfhLocalVars->ListRes    = $1.List;               // Copy the List bit
                                 lpfhLocalVars->NoDispRes  = TRUE;
                                 $$ = $1;
                                }
    | NAMEUNK ASSIGN            {DbgMsgWP (L"%%Result:  NAMEUNK" WS_UTF16_LEFTARROW);
                                 InitHdrStrand (&$1);
                                 PushHdrStrand_YY (&$1);
                                 $$ = *MakeHdrStrand_YY (&$1);
                                 lpfhLocalVars->lpYYResult = $1.lpYYStrandBase;
                                }
    ;

OptArg:
      '{' List     '}'          {DbgMsgWP (L"%%OptArg:  {List}");
                                 $$ = $2;
                                }
    | '{' OpenList '}'          {DbgMsgWP (L"%%OptArg:  {OpenList}");
                                 $$ = *MakeHdrStrand_YY (&$2);
                                 // This is the only odd case where we can't use the List bit
                                 //   because there are no parens, so we use the strand length
                                 $$.List = $$.lpYYStrandBase->uStrandLen > 1;       // Set the List bit
                                }
    | '(' '{' OpenList '}' ')'  {DbgMsgWP (L"%%OptArg:  ({OpenList})");
                                 $$ = *MakeHdrStrand_YY (&$3);
                                 $$.List = TRUE;                                    // Set the List bit
                                }
    ;

RhtArg:
      List                      {DbgMsgWP (L"%%RhtArg:  List");
                                 lpfhLocalVars->ListRht = $1.List;                  // Copy the List bit
                                 $$ = $1;
                                }
    | NAMEUNK                   {DbgMsgWP (L"%%RhtArg:  NAMEUNK");
                                 InitHdrStrand (&$1);
                                 PushHdrStrand_YY (&$1);
                                 $$ = *MakeHdrStrand_YY (&$1);
                                }
    ;

NoResHdr:                       // N.B. that this production does not need to return a result (in $$)
                                //   because it calculates all the information we need into
                                //   lpfhLocalVars->lpYY... and lpfhLocalVars->u???Valence
              NAMEUNK           {DbgMsgWP (L"%%NoResHdr:  NAMEUNK");                // Niladic function
                                 InitHdrStrand (&$1);
                                 PushHdrStrand_YY (&$1);
                                 MakeHdrStrand_YY (&$1);

                                 lpfhLocalVars->lpYYFcnName = $1.lpYYStrandBase;
                                 lpfhLocalVars->offFcnName  = $1.lpYYStrandBase->offTknIndex;
                                 lpfhLocalVars->DfnType     = DFNTYPE_FCN;          // Mark as a function
                                 lpfhLocalVars->fhNameType  = NAMETYPE_FN0;         // Mark as a niladic function
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_NIL;       // Mark as niladic
                                }

    |         AxisOpr           {DbgMsgWP (L"%%NoResHdr:  AxisOpr");                // Niladic function w/axis operator
                                 InitHdrStrand (&$1);
                                 PushHdrStrand_YY (&$1);
                                 MakeHdrStrand_YY (&$1);

                                 lpfhLocalVars->lpYYFcnName = $1.lpYYStrandBase;
                                 lpfhLocalVars->offFcnName  = $1.lpYYStrandBase->offTknIndex;
                                 lpfhLocalVars->DfnType     = DFNTYPE_FCN;          // Mark as a function
                                 lpfhLocalVars->fhNameType  = NAMETYPE_FN0;         // Mark as a niladic function
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_NIL;       // Mark as niladic

                                 // Check for Axis Operator on niladic function
                                 if (lpfhLocalVars->DfnAxis)
                                 {
                                     fh_yyerror (lpfhLocalVars, "syntax error");
                                     YYERROR;
                                 } // End IF
                                }
    |         AxisOpr  RhtArg   {DbgMsgWP (L"%%NoResHdr:  AxisOpr RhtArg");         // Monadic function w/axis operator
                                 InitHdrStrand (&$1);
                                 PushHdrStrand_YY (&$1);
                                 MakeHdrStrand_YY (&$1);

                                 lpfhLocalVars->lpYYFcnName = $1.lpYYStrandBase;
                                 lpfhLocalVars->offFcnName  = $1.lpYYStrandBase->offTknIndex;
                                 lpfhLocalVars->lpYYRhtArg  = $2.lpYYStrandBase;
                                 lpfhLocalVars->DfnType     = DFNTYPE_FCN;          // Mark as a function
                                 lpfhLocalVars->fhNameType  = NAMETYPE_FN12;        // Mark as a monadic/dyadic function
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_MON;       // Mark as monadic
                                }

    |         NAMEUNK  RhtArg   {DbgMsgWP (L"%%NoResHdr:  NAMEUNK RhtArg");         // Monadic function
                                 InitHdrStrand (&$1);
                                 PushHdrStrand_YY (&$1);
                                 MakeHdrStrand_YY (&$1);

                                 lpfhLocalVars->lpYYFcnName = $1.lpYYStrandBase;
                                 lpfhLocalVars->offFcnName  = $1.lpYYStrandBase->offTknIndex;
                                 lpfhLocalVars->lpYYRhtArg  = $2.lpYYStrandBase;
                                 lpfhLocalVars->DfnType     = DFNTYPE_FCN;          // Mark as a function
                                 lpfhLocalVars->fhNameType  = NAMETYPE_FN12;        // Mark as a monadic/dyadic function
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_MON;       // Mark as monadic
                                }

    | NAMEUNK AxisOpr  RhtArg   {DbgMsgWP (L"%%NoResHdr:  NAMEUNK AxisOpr RhtArg"); // Dyadic function w/axis operator
                                 InitHdrStrand (&$1);
                                 PushHdrStrand_YY (&$1);
                                 MakeHdrStrand_YY (&$1);

                                 InitHdrStrand (&$2);
                                 PushHdrStrand_YY (&$2);
                                 MakeHdrStrand_YY (&$2);

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYFcnName = $2.lpYYStrandBase;
                                 lpfhLocalVars->offFcnName  = $2.lpYYStrandBase->offTknIndex;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->DfnType     = DFNTYPE_FCN;          // Mark as a function
                                 lpfhLocalVars->fhNameType  = NAMETYPE_FN12;        // Mark as a monadic/dyadic function
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_DYD;       // Mark as dyadic
                                }
    | NAMEUNK NAMEUNK  RhtArg   {DbgMsgWP (L"%%NoResHdr:  NAMEUNK NAMEUNK RhtArg"); // Dyadic function
                                 InitHdrStrand (&$1);
                                 PushHdrStrand_YY (&$1);
                                 MakeHdrStrand_YY (&$1);

                                 InitHdrStrand (&$2);
                                 PushHdrStrand_YY (&$2);
                                 MakeHdrStrand_YY (&$2);

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYFcnName = $2.lpYYStrandBase;
                                 lpfhLocalVars->offFcnName  = $2.lpYYStrandBase->offTknIndex;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->DfnType     = DFNTYPE_FCN;          // Mark as a function
                                 lpfhLocalVars->fhNameType  = NAMETYPE_FN12;        // Mark as a monadic/dyadic function
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_DYD;       // Mark as dyadic
                                }
    | List    AxisOpr  RhtArg   {DbgMsgWP (L"%%NoResHdr:  List AxisOpr RhtArg");    // Dyadic function w/axis operator
                                 InitHdrStrand (&$2);
                                 PushHdrStrand_YY (&$2);
                                 MakeHdrStrand_YY (&$2);

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYFcnName = $2.lpYYStrandBase;
                                 lpfhLocalVars->offFcnName  = $2.lpYYStrandBase->offTknIndex;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->DfnType     = DFNTYPE_FCN;          // Mark as a function
                                 lpfhLocalVars->fhNameType  = NAMETYPE_FN12;        // Mark as a monadic/dyadic function
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_DYD;       // Mark as dyadic
                                 lpfhLocalVars->ListLft     = $1.List;              // Copy the List bit
                                }
    | List    NAMEUNK  RhtArg   {DbgMsgWP (L"%%NoResHdr:  List NAMEUNK RhtArg");    // Dyadic function
                                 InitHdrStrand (&$2);
                                 PushHdrStrand_YY (&$2);
                                 MakeHdrStrand_YY (&$2);

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYFcnName = $2.lpYYStrandBase;
                                 lpfhLocalVars->offFcnName  = $2.lpYYStrandBase->offTknIndex;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->DfnType     = DFNTYPE_FCN;          // Mark as a function
                                 lpfhLocalVars->fhNameType  = NAMETYPE_FN12;        // Mark as a monadic/dyadic function
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_DYD;       // Mark as dyadic
                                 lpfhLocalVars->ListLft     = $1.List;              // Copy the List bit
                                }
    | OptArg  AxisOpr  RhtArg   {DbgMsgWP (L"%%NoResHdr:  OptArg AxisOpr RhtArg");  // Bivalent function w/axis operator
                                 InitHdrStrand (&$2);
                                 PushHdrStrand_YY (&$2);
                                 MakeHdrStrand_YY (&$2);

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYFcnName = $2.lpYYStrandBase;
                                 lpfhLocalVars->offFcnName  = $2.lpYYStrandBase->offTknIndex;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->DfnType     = DFNTYPE_FCN;          // Mark as a function
                                 lpfhLocalVars->fhNameType  = NAMETYPE_FN12;        // Mark as a monadic/dyadic function
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_AMB;       // Mark as ambivalent
                                 lpfhLocalVars->ListLft     = $1.List;              // Copy the List bit
                                }
    | OptArg  NAMEUNK  RhtArg   {DbgMsgWP (L"%%NoResHdr:  OptArg NAMEUNK RhtArg");  // Bivalent function
                                 InitHdrStrand (&$2);
                                 PushHdrStrand_YY (&$2);
                                 MakeHdrStrand_YY (&$2);

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYFcnName = $2.lpYYStrandBase;
                                 lpfhLocalVars->offFcnName  = $2.lpYYStrandBase->offTknIndex;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->DfnType     = DFNTYPE_FCN;          // Mark as a function
                                 lpfhLocalVars->fhNameType  = NAMETYPE_FN12;        // Mark as a monadic/dyadic function
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_AMB;       // Mark as ambivalent
                                 lpfhLocalVars->ListLft     = $1.List;              // Copy the List bit
                                }
    |        AxisList           {DbgMsgWP (L"%%NoResHdr:  AxisList");               // Mon/Dyd operator, niladic derived function w/axis operator
                                 if (!GetOprName_EM (&$1))
                                     YYERROR;

                                 lpfhLocalVars->FcnValence  = FCNVALENCE_NIL;       // Mark as niladic
                                 lpfhLocalVars->fhNameType  = NAMETYPE_FN0;         // ...
                                }
    |        AxisList  RhtArg   {DbgMsgWP (L"%%NoResHdr:  AxisList RhtArg");        // Mon/Dyd operator, monadic derived function w/axis operator
                                 if (!GetOprName_EM (&$1))
                                     YYERROR;

                                 lpfhLocalVars->lpYYRhtArg  = $2.lpYYStrandBase;
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_MON;       // Mark as monadic
                                }
    |         List              {DbgMsgWP (L"%%NoResHdr:  List");                   // Mon/Dyd operator, niladic derived function
                                 if (!GetOprName_EM (&$1))
                                     YYERROR;

                                 lpfhLocalVars->FcnValence  = FCNVALENCE_NIL;       // Mark as niladic
                                 lpfhLocalVars->fhNameType  = NAMETYPE_FN0;         // ...
                                }
    |         List     RhtArg   {DbgMsgWP (L"%%NoResHdr:  List RhtArg");            // Mon/Dyd operator, monadic derived function
                                 if (!GetOprName_EM (&$1))
                                     YYERROR;

                                 lpfhLocalVars->lpYYRhtArg  = $2.lpYYStrandBase;
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_MON;       // Mark as monadic
                                }
    | NAMEUNK AxisList RhtArg   {DbgMsgWP (L"%%NoResHdr:  NAMEUNK AxisList RhtArg");// Mon/Dyd operator, dyadic derived function w/axis operator
                                 if (!GetOprName_EM (&$2))
                                     YYERROR;

                                 InitHdrStrand (&$1);
                                 PushHdrStrand_YY (&$1);
                                 MakeHdrStrand_YY (&$1);

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_DYD;       // Mark as dyadic
                                }
    | NAMEUNK List     RhtArg   {DbgMsgWP (L"%%NoResHdr:  NAMEUNK List RhtArg");    // Mon/Dyd operator, dyadic derived function
                                 if (!GetOprName_EM (&$2))
                                     YYERROR;

                                 InitHdrStrand (&$1);
                                 PushHdrStrand_YY (&$1);
                                 MakeHdrStrand_YY (&$1);

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_DYD;       // Mark as dyadic
                                }
    | List    AxisList RhtArg   {DbgMsgWP (L"%%NoResHdr:  List AxisList RhtArg");   // Mon/Dyd operator, dyadic derived function w/axis operator
                                 if (!GetOprName_EM (&$2))
                                     YYERROR;

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_DYD;       // Mark as dyadic
                                 lpfhLocalVars->ListLft     = $1.List;              // Copy the List bit
                                }
    | List    List     RhtArg   {DbgMsgWP (L"%%NoResHdr:  List List RhtArg");       // Mon/Dyd operator, dyadic derived function
                                 if (!GetOprName_EM (&$2))
                                     YYERROR;

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_DYD;       // Mark as dyadic
                                 lpfhLocalVars->ListLft     = $1.List;              // Copy the List bit
                                }
    | OptArg  AxisList RhtArg   {DbgMsgWP (L"%%NoResHdr:  OptArg AxisList RhtArg"); // Mon/Dyd operator, ambivalent derived function w/axis operator
                                 if (!GetOprName_EM (&$2))
                                     YYERROR;

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_AMB;       // Mark as ambivalent
                                 lpfhLocalVars->ListLft     = $1.List;              // Copy the List bit
                                }
    | OptArg  List     RhtArg   {DbgMsgWP (L"%%NoResHdr:  OptArg List RhtArg");     // Mon/Dyd operator, ambivalent derived function
                                 if (!GetOprName_EM (&$2))
                                     YYERROR;

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_AMB;       // Mark as ambivalent
                                 lpfhLocalVars->ListLft     = $1.List;              // Copy the List bit
                                }
    ;

Locals:
              LINECONT          {DbgMsgWP (L"%%Locals:  LINECONT");
                                }
    |         ';'      NOMORE   {DbgMsgWP (L"%%Locals:  ';' NOMORE");
                                }
    |         ';'      NAMEUNK  {DbgMsgWP (L"%%Locals:  ';' NAMEUNK");
                                 InitHdrStrand (&$2);
                                 $$ = *PushHdrStrand_YY (&$2);
                                }
    |         ';'      NAMESYS  {DbgMsgWP (L"%%Locals:  ';' NAMESYS");
                                 if (!$2.tkToken.tkData.tkSym->stFlags.Value
                                  && !lpfhLocalVars->ParseFcnName)
                                 {
                                     fh_yyerror (lpfhLocalVars, "value error");
                                     YYERROR;
                                 } // End IF

                                 InitHdrStrand (&$2);
                                 $$ = *PushHdrStrand_YY (&$2);
                                }
    | Locals  LINECONT          {DbgMsgWP (L"%%Locals:  Locals LINECONT");
                                }
    | Locals  ';'      NOMORE   {DbgMsgWP (L"%%Locals:  Locals ';' NOMORE");
                                }
    | Locals  ';'      NAMEUNK  {DbgMsgWP (L"%%Locals:  Locals ';' NAMEUNK");
                                 $$ = *PushHdrStrand_YY (&$3);
                                }
    | Locals  ';'      NAMESYS  {DbgMsgWP (L"%%Locals:  Locals ';' NAMESYS");
                                 if (!$3.tkToken.tkData.tkSym->stFlags.Value
                                  && !lpfhLocalVars->ParseFcnName)
                                 {
                                     fh_yyerror (lpfhLocalVars, "value error");
                                     YYERROR;
                                 } // End IF

                                 $$ = *PushHdrStrand_YY (&$3);
                                }
    | Locals           NAMEUNK  {DbgMsgWP (L"%%Locals:  Locals ';' NAMEUNK");
                                 $$ = *PushHdrStrand_YY (&$2);
                                }
    | Locals           NAMESYS  {DbgMsgWP (L"%%Locals:  Locals ';' NAMESYS");
                                 if (!$2.tkToken.tkData.tkSym->stFlags.Value
                                  && !lpfhLocalVars->ParseFcnName)
                                 {
                                     fh_yyerror (lpfhLocalVars, "value error");
                                     YYERROR;
                                 } // End IF

                                 $$ = *PushHdrStrand_YY (&$2);
                                }
    ;

Header:
        /* Empty */             {DbgMsgWP (L"%%Header:  <empty>");
                                }
    |         error             {DbgMsgWP (L"%%Header:  error");
                                 YYABORT;
                                }
    |         NoResHdr          {DbgMsgWP (L"%%Header:  NoResHdr");
                                }
    |         NoResHdr error    {DbgMsgWP (L"%%Header:  NoResHdr error");
                                 if (!lpfhLocalVars->ParseFcnName)
                                     YYABORT;
                                }
    |         NoResHdr Locals   {DbgMsgWP (L"%%Header:  NoResHdr Locals");
                                 lpfhLocalVars->lpYYLocals = MakeHdrStrand_YY (&$2);
                                }
    | error   NoResHdr          {DbgMsgWP (L"%%Header:  error NoResHdr");
                                 YYABORT;
                                }
    | Result  NoResHdr          {DbgMsgWP (L"%%Header:  Result NoResHdr");
                                }
    | error   NoResHdr Locals   {DbgMsgWP (L"%%Header:  error  NoResHdr Locals");
                                 YYABORT;
                                }
    | Result  NoResHdr error    {DbgMsgWP (L"%%Header:  Result NoResHdr error");
                                 if (!lpfhLocalVars->ParseFcnName)
                                     YYABORT;
                                }
    | Result  NoResHdr Locals   {DbgMsgWP (L"%%Header:  Result NoResHdr Locals");
                                 lpfhLocalVars->lpYYLocals = MakeHdrStrand_YY (&$3);
                                }
    ;

HeaderComm:
      Header SOS                {DbgMsgWP (L"%%HeaderComm:  Header SOS");
#ifdef DEBUG
                                 DisplayFcnHdr (lpfhLocalVars);
#endif
                                }
    | Header SOS NOMORE         {DbgMsgWP (L"%%HeaderComm:  Header SOS NOMORE");
#ifdef DEBUG
                                 DisplayFcnHdr (lpfhLocalVars);
#endif
                                }
    ;

%%

//***************************************************************************
//  Start of C program
//***************************************************************************

//***************************************************************************
//  $ParseFcnHeader
//
//  Parse a function header
//***************************************************************************

UBOOL ParseFcnHeader
    (HWND          hWndEC,          // Window handle of Edit Ctrl
     HGLOBAL       hGlbTknHdr,      // Tokenized header global memory handle
     LPFHLOCALVARS lpfhLocalVars,   // Ptr to Function Header local vars
     UBOOL         bDisplayErr)     // TRUE iff want error messages displayed

{
    UBOOL bRet = FALSE,             // TRUE iff result is valid
          OldDisplayErr;            // Save area for old DisplayErr

    __try
    {
        // Save the window handle
        lpfhLocalVars->hWndEC = hWndEC;

        // Save the error display flag
        OldDisplayErr = lpfhLocalVars->DisplayErr;
        lpfhLocalVars->DisplayErr = bDisplayErr;

        // Save ptr to Function Header local vars
        TlsSetValue (dwTlsFhLocalVars, (LPVOID) lpfhLocalVars);

        // Save the token header global memory handle
        lpfhLocalVars->hGlbTknHdr = hGlbTknHdr;

        // Lock the memory to get a ptr to it
        lpfhLocalVars->lpHeader  = MyGlobalLockTkn (lpfhLocalVars->hGlbTknHdr);

        // Initialize the base & next strand ptrs
        lpfhLocalVars->lpYYStrandBase =
        lpfhLocalVars->lpYYStrandNext = lpfhLocalVars->lpYYStrandStart;

        // Skip over TOKEN_HEADER
        lpfhLocalVars->lptkStart = TokenBaseToStart (lpfhLocalVars->lpHeader);

        // Skip over the starting EOL
        lpfhLocalVars->lptkNext  = &lpfhLocalVars->lptkStart[1];

        // Account for the above skip
        lpfhLocalVars->offTknBase++;

        // Mark the stopping point
        lpfhLocalVars->lptkStop  = &lpfhLocalVars->lptkStart[lpfhLocalVars->lptkStart->tkData.tkChar];

        // Start off with no error
        lpfhLocalVars->tkErrorCharIndex = NEG1U;

        // ***FIXME*** -- Use VirtualAlloc for the parser stack














#if YYDEBUG
        // Enable debugging
        yydebug = TRUE;
#endif

        // Parse the header, check for errors
        //   0 = success
        //   1 = YYABORT or APL error
        //   2 = memory exhausted
        bRet = fh_yyparse (lpfhLocalVars) EQ 0;

#if YYDEBUG
        // Disable debugging
        yydebug = FALSE;
#endif
    } __except (CheckException (GetExceptionInformation (), L"ParseFcnHeader"))
    {
        // Display message for unhandled exception
        DisplayException ();

        // Mark as in error
        bRet = FALSE;
    } // End __try/__except

    // Restore the error display flag
    lpfhLocalVars->DisplayErr = OldDisplayErr;

    // We no longer need this ptr
    MyGlobalUnlock (lpfhLocalVars->hGlbTknHdr); lpfhLocalVars->lpHeader = NULL;

    return bRet;
} // End ParseFcnHeader


//***************************************************************************
//  $fh_yylex
//
//  Lexical analyzer for Bison
//***************************************************************************

int fh_yylex
    (LPFH_YYSTYPE  lpYYLval,            // Ptr to lval
     LPFHLOCALVARS lpfhLocalVars)       // Ptr to Function Header local vars

{
FH_YYLEX_START:
    // Save the token base as this item's index
    //   and post-increment it
    lpYYLval->offTknIndex = lpfhLocalVars->offTknBase++;

    // Check for stopping point
    if (lpfhLocalVars->lptkStop EQ lpfhLocalVars->lptkNext)
    {
        // If called from ParseFunctionName, ...
        if (lpfhLocalVars->ParseFcnName)
        {
            // Reset so we return NOMORE only once
            lpfhLocalVars->ParseFcnName = FALSE;

            return NOMORE;
        } else
            return '\0';
    } // End IF

#ifdef YYLEX_DEBUG
    dprintfWL9 (L"==fh_yylex:  TknType = %S, CharIndex = %d",
                GetTokenTypeName (lpfhLocalVars->lptkNext->tkFlags.TknType),
                lpfhLocalVars->lptkNext->tkCharIndex);
#endif

    // Return the current token
    lpYYLval->tkToken            = *lpfhLocalVars->lptkNext;
    lpYYLval->uStrandLen         =
    lpYYLval->Indirect           = FALSE;
    lpYYLval->List               = FALSE;
    lpYYLval->lpYYStrandIndirect = NULL;
    lpYYLval->lpYYStrandBase     = lpfhLocalVars->lpYYStrandBase;

    // Split cases based upon the token type
    switch (lpfhLocalVars->lptkNext++->tkFlags.TknType)
    {
        case TKT_DEL:           // Del      -- always a function
        case TKT_DELDEL:        // Del Del  -- either a monadic or dyadic operator
        case TKT_DELAFO:        // Del Anon -- either a monadic or dyadic operator, bound to its operands
            // If we're not processing an AFO
            if (!lpfhLocalVars->bAFO)
                return UNK;

            // If the next token is a left bracket, return NAMEOPR
            if (lpfhLocalVars->lptkNext->tkFlags.TknType EQ TKT_LEFTBRACKET)
                return NAMEOPR;
            else
                return NAMEUNK;

        case TKT_VARNAMED:
            // If the token is a sysname, return NAMESYS
            if (lpfhLocalVars->lptkNext[-1].tkData.tkSym->stFlags.ObjName EQ OBJNAME_SYS)
            {
                // If this local is []RL, ...
                if (IsSymSysName (lpfhLocalVars->lptkNext[-1].tkData.tkSym, $QUAD_RL))
                   // Set a flag
                   lpfhLocalVars->bLclRL = TRUE;

                return NAMESYS;
            } else
            {
                // If the next token is a left bracket, return NAMEOPR
                if (lpfhLocalVars->lptkNext->tkFlags.TknType EQ TKT_LEFTBRACKET)
                    return NAMEOPR;
                else
                    return NAMEUNK;
            } // End IF

        case TKT_ASSIGN:
            return ASSIGN;

        case TKT_LISTSEP:
            return ';';

        case TKT_LEFTPAREN:
            return '(';

        case TKT_RIGHTPAREN:
            return ')';

        case TKT_LEFTBRACKET:
            return '[';

        case TKT_RIGHTBRACKET:
            return ']';

        case TKT_LEFTBRACE:
            return '{';

        case TKT_RIGHTBRACE:
            return '}';

        case TKT_SOS:
            return SOS;

        case TKT_GLBDFN:
        case TKT_LINECONT:
            goto FH_YYLEX_START;    // Ignore these tokens

        default:
            return UNK;
    } // End SWITCH
} // End fh_yylex


//***************************************************************************
//  $fh_yyerror
//
//  Error callback from Bison
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- fh_yyerror"
#else
#define APPEND_NAME
#endif

void fh_yyerror                         // Called for Bison syntax error
    (LPFHLOCALVARS lpfhLocalVars,       // Ptr to Function Header local vars
     const char   *s)                   // Ptr to error msg

{
    char  szTemp[1024];
    WCHAR wszTemp[1024], *wp;
    UINT  uCharIndex;

#ifdef DEBUG
    DbgMsg ((char *) s);
#endif
    // Check for stopping point
    if (lpfhLocalVars->lptkStop EQ lpfhLocalVars->lptkNext)
        // Get the character index position
        uCharIndex = lpfhLocalVars->lptkNext[-1].tkCharIndex + 1;
    else
        // Get the character index position
        uCharIndex = lpfhLocalVars->lptkNext[ 0].tkCharIndex;

    // Save the character index position
    lpfhLocalVars->tkErrorCharIndex = uCharIndex;

    // Check for SYNTAX ERROR
#define ERR     "syntax error"
    MyStrcpyn (szTemp, sizeof (szTemp), s, strsizeof (ERR));    // Note: Terminates the string, too
    if (lstrcmp (szTemp, ERR) EQ 0)
    {
        MySprintfW (wszTemp,
                    sizeof (wszTemp),
                   L"SYNTAX ERROR in header position %d -- function NOT saved.",
                    uCharIndex);
        wp = wszTemp;

        goto DISPLAYCAT;
#undef  ERR
    } // End IF

    // Check for VALUE ERROR
#define ERR     "value error"
    MyStrcpyn (szTemp, sizeof (szTemp), s, strsizeof (ERR));    // Note: Terminates the string, too
    if (lstrcmp (szTemp, ERR) EQ 0)
    {
        MySprintfW (wszTemp,
                    sizeof (wszTemp),
                   L"VALUE ERROR in header position %d -- function NOT saved.",
                    uCharIndex);
        wp = wszTemp;

        goto DISPLAYCAT;
#undef  ERR
    } // End IF

    // Check for LENGTH ERROR
#define ERR     "length error"
    MyStrcpyn (szTemp, sizeof (szTemp), s, strsizeof (ERR));    // Note: Terminates the string, too
    if (lstrcmp (szTemp, ERR) EQ 0)
    {
        MySprintfW (wszTemp,
                    sizeof (wszTemp),
                   L"LENGTH ERROR in header position %d -- function NOT saved.",
                    uCharIndex);
        wp = wszTemp;

        goto DISPLAYCAT;
#undef  ERR
    } // End IF

#define ERR     "memory exhausted"
    MyStrcpyn (szTemp, sizeof (szTemp), s, strsizeof (ERR));    // Note: Terminates the string, too
    if (lstrcmp (szTemp, ERR) EQ 0)
    {
        MySprintfW (wszTemp,
                    sizeof (wszTemp),
                   L"Insufficient memory to parse header -- function NOT saved.");
        wp = wszTemp;

        goto DISPLAYCAT;
#undef  ERR
    } // End IF

    // Use the error message as given
    MySprintfW (wszTemp,
                sizeof (wszTemp),
               L"%S",
                s);
    wp = wszTemp;

    goto DISPLAY;

DISPLAYCAT:
    // If the caller doesn't wants error messages displayed, ...
    if (!lpfhLocalVars->DisplayErr)
    {
        // Pass the error message back to the caller
        MyStrcpyW (lpfhLocalVars->wszErrMsg, sizeof (lpfhLocalVars->wszErrMsg), wszTemp);

        return;
    } // End IF

#ifdef DEBUG
    // Append the original error message
    MySprintfW (&wszTemp[lstrlenW (wszTemp)],
                 sizeof (wszTemp) - (lstrlenW (wszTemp) * sizeof (wszTemp[0])),
                L"(%S)",
                 s);
#endif
DISPLAY:
    // Display a message box
    MessageBoxW (hWndMF,
                 wp,
                 lpwszAppName,
                 MB_OK | MB_ICONWARNING | MB_APPLMODAL);
} // End fh_yyerror
#undef  APPEND_NAME


//***************************************************************************
//  $fh_yyfprintf
//
//  Debugger output
//***************************************************************************

void fh_yyfprintf
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
} // End fh_yyfprintf


//***************************************************************************
//  End of File: fh_parse.y
//***************************************************************************
