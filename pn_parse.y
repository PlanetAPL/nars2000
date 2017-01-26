//***************************************************************************
//  NARS2000 -- Parser Grammar for XXX Point Notation
//***************************************************************************

//***************************************************************************
//  Parse a line of XXX Point Notation characters.
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

/***************************************************************************

Build 1227 accumulates a strand of numeric constants all at once by calling
<ParsePointNotation> once on the entire scalar/vector (with intervening spaces),
then looping through the individual elements to determine the common storage
type taking into account that some FLTs might be expressible either as rational
integers or as more precise VFP numbers, and then calling <ParsePointNotation>
again on only those elements that need to be promoted to a higher storage type.
This allows (say) 1111111111111111111111111111111111111 1r2 to be parsed as FLT
RAT on the first pass, and then re-parse the FLT text as RAT so that the final
result is a RAT that displays identically to the format in which it was
entered.  Without this change, the FLT RAT pair would be promoted to VFP and
the FLT value would be converted to a VFP with imprecise trailing digits as in
1111111111111111174642258481095114752 0.5

***************************************************************************/

%{
#define STRICT
#include <windows.h>
#include <math.h>
#include "headers.h"

////#define YYLEX_DEBUG
////#define YYFPRINTF_DEBUG

#ifdef DEBUG
#define YYERROR_VERBOSE
#define YYDEBUG 1
#define YYFPRINTF               pn_yyfprintf
#define fprintf                 pn_yyfprintf
////#define YYPRINT                 pn_yyprint
void pn_yyprint     (FILE *yyoutput, unsigned short int yytoknum, PN_YYSTYPE const yyvaluep);
#endif

#define YYMALLOC    malloc
#define YYFREE      free

#include "pn_parse.proto"

#define    YYSTYPE     PN_YYSTYPE
#define  LPYYSTYPE   LPPN_YYSTYPE
#define tagYYSTYPE  tagPN_YYSTYPE

// The following #defines are needed to allow multiple parses
//   to coexist in the same file
#define yy_symbol_print         pn_yy_symbol_print
#define yy_symbol_value_print   pn_yy_symbol_value_print
#define yy_reduce_print         pn_yy_reduce_print
#define yydestruct              pn_yydestruct

#define YYERROR2        {lppnLocalVars->bYYERROR = TRUE; YYERROR;}

////#define DbgMsgWP(a)         DbgMsgW(a)
////#define DbgMsgWP(a)         DbgMsgW(a); DbgBrk ()
////#define DbgMsgWP(a)         DbgMsgW(a)
    #define DbgMsgWP(a)

#define WSFULL          "memory exhausted"

// Define macro for Boolean or Integer type
#define IsIntegerType(a)        ((a) EQ PN_NUMTYPE_BOOL || (a) EQ PN_NUMTYPE_INT)
#define IsRatType(a)            ((a) EQ PN_NUMTYPE_RAT)
#define IsVfpType(a)            ((a) EQ PN_NUMTYPE_VFP)

%}

%pure-parser
%name-prefix "pn_yy"
%parse-param {LPPNLOCALVARS lppnLocalVars}
%lex-param   {LPPNLOCALVARS lppnLocalVars}
%token EXT INF OVR

%start VectorRes

%%

Digit:
      '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'
    ;

LowerAlphabet:
      'a' | 'b' | 'c' | 'd' | 'e' | 'f' | 'g' | 'h' | 'i' | 'j' | 'k' | 'l' | 'm'
    | 'n' | 'o' | 'p' | 'q' | 'r' | 's' | 't' | 'u' | 'v' | 'w' | 'x' | 'y' | 'z'
    ;

UpperAlphabet:
      'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' | 'J' | 'K' | 'L' | 'M'
    | 'N' | 'O' | 'P' | 'Q' | 'R' | 'S' | 'T' | 'U' | 'V' | 'W' | 'X' | 'Y' | 'Z'
    ;

DEF_RATSEP:     'r'
DEF_VFPSEP:     'v'

Alphabet:
      LowerAlphabet
    | UpperAlphabet
    ;

Integer:
              Digit                 {DbgMsgWP (L"%%Integer:  Digit");
                                     // Mark starting offset
                                     $1.uNumAcc = lppnLocalVars->uNumAcc;

                                     // Accumulate the digit
                                     PN_NumAcc (lppnLocalVars, $1.chCur);

                                     $$ = $1;
                                    }
    | OVR     Digit                 {DbgMsgWP (L"%%Integer:  " WS_UTF16_OVERBAR L" Digit");
                                     // Mark starting offset
                                     $1.uNumAcc = lppnLocalVars->uNumAcc;

                                     // Accumulate the negative sign
                                     PN_NumAcc (lppnLocalVars, OVERBAR1);

                                     // Accumulate the digit
                                     PN_NumAcc (lppnLocalVars, $2.chCur);

                                     $$ = $1;
                                    }
    | Integer Digit                 {DbgMsgWP (L"%%Integer:  Integer Digit");
                                     // Accumulate the digit
                                     PN_NumAcc (lppnLocalVars, $2.chCur);
                                    }
    ;

// Unsigned integer
UnInteger:
              Digit                 {DbgMsgWP (L"%%UnInteger:  Digit");
                                     // Mark starting offset
                                     $1.uNumAcc = lppnLocalVars->uNumAcc;

                                     // Accumulate the digit
                                     PN_NumAcc (lppnLocalVars, $1.chCur);

                                     $$ = $1;
                                    }
    | Integer Digit                 {DbgMsgWP (L"%%UnInteger:  Integer Digit");
                                     // Accumulate the digit
                                     PN_NumAcc (lppnLocalVars, $2.chCur);
                                    }
    ;

AlphaInt:
               Alphabet             {DbgMsgWP (L"%%AlphaInt:  Alphabet");
                                     lppnLocalVars->uAlpAcc = 0;
                                     PN_ChrAcc (lppnLocalVars, $1.chCur);
                                    }
    |          Digit                {DbgMsgWP (L"%%AlphaInt:  Digit");
                                     lppnLocalVars->uAlpAcc = 0;
                                     PN_ChrAcc (lppnLocalVars, $1.chCur);
                                    }
    | AlphaInt Alphabet             {DbgMsgWP (L"%%AlphaInt:  Alphaint Alphabet");
                                     PN_ChrAcc (lppnLocalVars, $2.chCur);
                                    }
    | AlphaInt Digit                {DbgMsgWP (L"%%AlphaInt:  AlphaInt Digit");
                                     PN_ChrAcc (lppnLocalVars, $2.chCur);
                                    }
    ;

Decimal:
              '.' Digit             {DbgMsgWP (L"%%Decimal:  '.' Digit");
                                     // Mark starting offset
                                     $1.uNumAcc = lppnLocalVars->uNumAcc;

                                     // Accumulate the decimal point
                                     PN_NumAcc (lppnLocalVars, '.');

                                     // Accumulate the digit
                                     PN_NumAcc (lppnLocalVars, $2.chCur);

                                     $$ = $1;
                                    }
    | OVR     '.' Digit             {DbgMsgWP (L"%%Decimal:  '" WS_UTF16_OVERBAR L"' '.' Digit");
                                     // Mark starting offset
                                     $1.uNumAcc = lppnLocalVars->uNumAcc;

                                     // Accumulate the negative sign
                                     PN_NumAcc (lppnLocalVars, OVERBAR1);

                                     // Accumulate the decimal point
                                     PN_NumAcc (lppnLocalVars, '.');

                                     // Accumulate the digit
                                     PN_NumAcc (lppnLocalVars, $3.chCur);

                                     $$ = $1;
                                    }
    | Integer '.' Digit             {DbgMsgWP (L"%%Decimal:  Integer '.' Digit");
                                     // Accumulate the decimal point
                                     PN_NumAcc (lppnLocalVars, '.');

                                     // Accumulate the digit
                                     PN_NumAcc (lppnLocalVars, $3.chCur);
                                    }
    | Decimal     Digit             {DbgMsgWP (L"%%Decimal:  Decimal Digit");
                                     // Accumulate the digit
                                     PN_NumAcc (lppnLocalVars, $2.chCur);
                                    }
    ;

UnIntPoint:
      UnInteger                     {DbgMsgWP (L"%%UnIntPoint:  UnInteger");
                                     // Calculate the number
                                     PN_NumCalc (lppnLocalVars, &$1, FALSE);

                                     $$ = $1;
                                    }
    ;

DecPoint:
      Integer                       {DbgMsgWP (L"%%DecPoint:  Integer");
                                     // Terminate the argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     // Mark the result as integer
                                     $1.chType = PN_NUMTYPE_INT;

                                     // Calculate the number
                                     PN_NumCalc (lppnLocalVars, &$1, FALSE);

                                     $$ = $1;
                                    }
    | Integer '.'                   {DbgMsgWP (L"%%DecPoint:  Integer '.'");
                                     // Terminate the argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     // Mark the result as float
                                     $1.chType = PN_NUMTYPE_FLT;

                                     // Calculate the number
                                     PN_NumCalc (lppnLocalVars, &$1, FALSE);

                                     $$ = $1;
                                    }
    | Decimal                       {DbgMsgWP (L"%%DecPoint:  Decimal");
                                     // Terminate the argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     // Mark the result as float
                                     $1.chType = PN_NUMTYPE_FLT;

                                     // Calculate the number
                                     PN_NumCalc (lppnLocalVars, &$1, FALSE);

                                     $$ = $1;
                                    }
    ;

DecConstants:
          INF                       {DbgMsgWP (L"%%DecConstants:  INF");
                                     // Set constant infinity
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_FLT, $1.uNumStart,  1, NULL);
                                    }
    | OVR INF                       {DbgMsgWP (L"%%DecConstants:  OVR INF");
                                     // Set constant infinity
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_FLT, $1.uNumStart, -1, NULL);
                                    }
    ;

VfpConstants:
          INF DEF_VFPSEP            {DbgMsgWP (L"%%VfpConstants:  INF 'v'");
                                     // Set constant infinity
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_VFP, $1.uNumStart,  1, NULL);
                                    }
    |     INF DEF_VFPSEP UnIntPoint {DbgMsgWP (L"%%VfpConstants:  INF 'v' UnIntPoint");
                                     // Set constant infinity
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_VFP, $1.uNumStart,  1, &$3.at.aplInteger);
                                    }
    | OVR INF DEF_VFPSEP            {DbgMsgWP (L"%%VfpConstants:  OVR INF 'v'");
                                     // Set constant infinity
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_VFP, $1.uNumStart, -1, NULL);
                                     if (lppnLocalVars->bYYERROR)
                                        YYERROR;
                                    }
    | OVR INF DEF_VFPSEP UnIntPoint {DbgMsgWP (L"%%VfpConstants:  OVR INF 'v' UnIntPoint");
                                     // Set constant infinity
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_VFP, $1.uNumStart, -1, &$4.at.aplInteger);
                                     if (lppnLocalVars->bYYERROR)
                                        YYERROR;
                                    }
    ;

// RatPoint left and right arguments
// ExtPoint left arg
RatArgs:
      DecPoint
    | ExpPoint
    ;

RatPoint:
      RatArgs  DEF_RATSEP RatArgs   {DbgMsgWP (L"%%RatPoint:  RatArgs 'r' RatArgs");
                                     // Make it into a RatPoint number
                                     lppnLocalVars->lpYYRes = PN_MakeRatPoint   (&$1, &$3,  lppnLocalVars);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    ;

RatConstantsInt:
          INF  DEF_RATSEP RatArgs   {DbgMsgWP (L"%%RatConstantsInt:  INF 'r' RatArgs");
                                     // Set constant infinity
                                     //   taking into account negative integer and -0
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_RAT, $1.uNumStart,  1 - 2 * (lppnLocalVars->lpszNumAccum[0] EQ OVERBAR1), NULL);
                                    }
    | OVR INF  DEF_RATSEP RatArgs   {DbgMsgWP (L"%%RatConstantsInt:  OVR INF 'r' RatArgs");
                                     // Set constant infinity
                                     //   taking into account negative integer and -0
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_RAT, $1.uNumStart,  (2 * (lppnLocalVars->lpszNumAccum[0] EQ OVERBAR1)) - 1, NULL);
                                    }
    | RatArgs  DEF_RATSEP INF       {DbgMsgWP (L"%%RatConstantsInt:  RatArgs 'r' INF");
                                     // If the integer is signed, ...
                                     if (gAllowNeg0
                                      && lppnLocalVars->lpszNumAccum[0] EQ OVERBAR1)
                                     {
                                         // Initialize the result and set to -0
                                         mpfr_init_set_str (&$$.at.aplVfp, "-0", 10, MPFR_RNDN);

                                         // Mark the result as VFP
                                         $$.chType = PN_NUMTYPE_VFP;
                                     } else
                                     {
                                         // Initialize the result to 0/1
                                         mpq_init (&$$.at.aplRat);

                                         // Mark the result as RAT
                                         $$.chType = PN_NUMTYPE_RAT;
                                     } // End IF/ELSE
                                    }
    | RatArgs  DEF_RATSEP OVR INF   {DbgMsgWP (L"%%RatConstantsInt:  RatArgs 'r' OVR INF");
                                     // If the integer is NOT signed, ...
                                     if (gAllowNeg0
                                      && lppnLocalVars->lpszNumAccum[0] NE OVERBAR1)
                                     {
                                         // Initialize the result and set to -0
                                         mpfr_init_set_str (&$$.at.aplVfp, "-0", 10, MPFR_RNDN);

                                         // Mark the result as VFP
                                         $$.chType = PN_NUMTYPE_VFP;
                                     } else
                                     {
                                         // Initialize the result to 0/1
                                         mpq_init (&$$.at.aplRat);

                                         // Mark the result as RAT
                                         $$.chType = PN_NUMTYPE_RAT;
                                     } // End IF/ELSE
                                    }
    ;

RatConstantsExt:
          INF EXT                   {DbgMsgWP (L"%%RatConstantsExt:  INF 'x'");
                                     // Set constant infinity
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_RAT, $1.uNumStart,  1, NULL);
                                    }
    | OVR INF EXT                   {DbgMsgWP (L"%%RatConstantsExt:  OVR INF 'x'");
                                     // Set constant infinity
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_RAT, $1.uNumStart, -1, NULL);
                                    }
    ;

ExtPoint:
      RatArgs  EXT                  {DbgMsgWP (L"%%ExtPoint:  RatArgs 'x'");
                                     // Make it into a RatPoint number
                                     lppnLocalVars->lpYYRes = PN_MakeRatPoint   (&$1, NULL, lppnLocalVars);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    ;

// Euler/Pi/Gamma left and right arguments
EPGArgs:
      DecPoint
    | ExpPoint
    | RatPoint
    | VfpPoint
    ;

GammaPoint:
      EPGArgs   'g' EPGArgs         {DbgMsgWP (L"%%GammaPoint:  EPGArgs 'g' EPGArgs");
                                     // Make it into a GammaPoint number
                                     lppnLocalVars->lpYYRes = PN_MakeGammaPoint (&$1, &$3,  lppnLocalVars);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    | EPGArgs   'g' ExtPoint        {DbgMsgWP (L"%%GammaPoint:  EPGArgs 'g' ExtPoint");
                                     // Make it into a GammaPoint number
                                     lppnLocalVars->lpYYRes = PN_MakeGammaPoint (&$1, &$3,  lppnLocalVars);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    ;

PiPoint:
      EPGArgs   'p' EPGArgs         {DbgMsgWP (L"%%PiPoint:  EPGArgs 'p' EPGArgs");
                                     // Make it into a PiPoint number
                                     lppnLocalVars->lpYYRes = PN_MakePiPoint    (&$1, &$3,  lppnLocalVars);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    | EPGArgs   'p' ExtPoint        {DbgMsgWP (L"%%PiPoint:  EPGArgs 'p' ExtPoint");
                                     // Make it into a PiPoint number
                                     lppnLocalVars->lpYYRes = PN_MakePiPoint    (&$1, &$3,  lppnLocalVars);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    ;

ExpPoint:
      DecPoint 'e' Integer          {DbgMsgWP (L"%%ExpPoint:  DecPoint 'e' Integer");
                                     // Terminate the (Exponent) argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     // Make it into a ExpPoint number
                                     lppnLocalVars->lpYYRes = PN_MakeExpPoint   (&$1, &$3,  lppnLocalVars);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    | DecPoint 'E' Integer          {DbgMsgWP (L"%%ExpPoint:  DecPoint 'E' Integer");
                                     // Terminate the (Exponent) argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     // Make it into a ExpPoint number
                                     lppnLocalVars->lpYYRes = PN_MakeExpPoint   (&$1, &$3,  lppnLocalVars);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    ;

VfpPoint:
      DecPoint DEF_VFPSEP           {DbgMsgWP (L"%%VfpPoint:  DecPoint 'v'");
                                     // Make it into a VfpPoint number
                                     lppnLocalVars->lpYYRes = PN_MakeVfpPoint   (&$1, NULL, lppnLocalVars, NULL);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    | DecPoint DEF_VFPSEP UnIntPoint{DbgMsgWP (L"%%VfpPoint:  DecPoint 'v' UnIntPoint");
                                     // Make it into a VfpPoint number
                                     lppnLocalVars->lpYYRes = PN_MakeVfpPoint   (&$1, NULL, lppnLocalVars, &$3.at.aplInteger);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    | DecPoint 'e' Integer DEF_VFPSEP
                                    {DbgMsgWP (L"%%VfpPoint:  DecPoint 'e' Integer 'v'");
                                     // Terminate the (Exponent) argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     // Make it into a VfpPoint number
                                     lppnLocalVars->lpYYRes = PN_MakeVfpPoint   (&$1, &$3,  lppnLocalVars, NULL);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    | DecPoint 'e' Integer DEF_VFPSEP UnIntPoint
                                    {DbgMsgWP (L"%%VfpPoint:  DecPoint 'e' Integer 'v'");
                                     // Terminate the (Exponent) argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     // Make it into a VfpPoint number
                                     lppnLocalVars->lpYYRes = PN_MakeVfpPoint   (&$1, &$3,  lppnLocalVars, &$5.at.aplInteger);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    | DecPoint 'E' Integer DEF_VFPSEP
                                    {DbgMsgWP (L"%%VfpPoint:  DecPoint 'E' Integer 'v'");
                                     // Terminate the (Exponent) argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     // Make it into a VfpPoint number
                                     lppnLocalVars->lpYYRes = PN_MakeVfpPoint   (&$1, &$3,  lppnLocalVars, NULL);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    | DecPoint 'E' Integer DEF_VFPSEP UnIntPoint
                                    {DbgMsgWP (L"%%VfpPoint:  DecPoint 'E' Integer 'v'");
                                     // Terminate the (Exponent) argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     // Make it into a VfpPoint number
                                     lppnLocalVars->lpYYRes = PN_MakeVfpPoint   (&$1, &$3,  lppnLocalVars, &$5.at.aplInteger);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    ;

EulerPoint:
      EPGArgs  'x' EPGArgs          {DbgMsgWP (L"%%EulerPoint:  EPGArgs 'x' EPGArgs");
                                     // Make it into a EulerPoint number
                                     lppnLocalVars->lpYYRes = PN_MakeEulerPoint (&$1, &$3,  lppnLocalVars);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    | EPGArgs  'x' ExtPoint         {DbgMsgWP (L"%%EulerPoint:  EPGArgs 'x' ExtPoint");
                                     // Make it into a EulerPoint number
                                     lppnLocalVars->lpYYRes = PN_MakeEulerPoint (&$1, &$3,  lppnLocalVars);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    ;

// BasePoint left args
BaseArgs:
      EPGArgs
    | EulerPoint
    | GammaPoint
    | PiPoint
    ;

BasePoint:
      BaseArgs   'b' AlphaInt       {DbgMsgWP (L"%%BasePoint:  BaseArgs 'b' AlphaInt");
                                     // Make it into a BasePoint number
                                     lppnLocalVars->lpYYRes = PN_MakeBasePoint  (&$1, &$3,  lppnLocalVars);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    ;

Number:
      BasePoint                     {DbgMsgWP (L"%%Number:  BasePoint");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | BaseArgs                      {DbgMsgWP (L"%%Number:  BaseArgs");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | ExtPoint                      {DbgMsgWP (L"%%Number:  ExtPoint");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | DecConstants                  {DbgMsgWP (L"%%Number:  DecConstants");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | RatConstantsExt               {DbgMsgWP (L"%%Number:  RatConstantsExt");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | RatConstantsInt               {DbgMsgWP (L"%%Number:  RatConstantsInt");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | VfpConstants                  {DbgMsgWP (L"%%Number:  VfpConstants");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    ;

// Accumulate into a vector
VectorAcc:
      Number                        {DbgMsgWP (L"%%VectorAcc:  Number");
                                     // Accumulate the current value into the vector
                                     if (!PN_VectorAcc (&$1, lppnLocalVars))
                                         YYERROR2;
                                    }
    | VectorAcc Space Number        {DbgMsgWP (L"%%VectorAcc:  VectorAcc Space Number");
                                     // Accumulate the current value into the vector
                                     if (!PN_VectorAcc (&$3, lppnLocalVars))
                                         YYERROR2;
                                    }
    ;

Space:
            ' '                     {DbgMsgWP (L"%%Space:  ' '");
                                     // Skip over the space
                                     lppnLocalVars->uNumIni++;
                                    }
    | Space ' '                     {DbgMsgWP (L"%%Space:  Space ' '");
                                     // Skip over the space
                                     lppnLocalVars->uNumIni++;
                                    }
    ;

// The resulting vector
VectorRes:
      VectorAcc                     {DbgMsgWP (L"%%VectorRes:  VectorAcc");
                                     // Create the scalar or vector result
                                     if (!PN_VectorRes (lppnLocalVars))
                                        YYERROR2;
                                    }
    ;

%%

//***************************************************************************
//  Start of C program
//***************************************************************************

//***************************************************************************
//  $ParsePointNotation
//
//  Parse a sequence of Point Notation characters
//
//  Thanks to the designers of J for these clever ideas.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ParsePointNotation"
#else
#define APPEND_NAME
#endif

UBOOL ParsePointNotation
    (LPPNLOCALVARS lppnLocalVars)           // Ptr to PN Local vars

{
    UBOOL bRet;                             // TRUE iff result is valid

    // Initialize the starting indices
    lppnLocalVars->uNumCur =
    lppnLocalVars->uNumIni =
    lppnLocalVars->uAlpAcc =
    lppnLocalVars->uNumAcc = 0;

    // While the last char is a blank, ...
    while (lppnLocalVars->uNumLen && IsWhite (lppnLocalVars->lpszStart[lppnLocalVars->uNumLen - 1]))
        // Delete it
        lppnLocalVars->uNumLen--;

    // Check for Rat but not Vfp separator
    if (strchr (lppnLocalVars->lpszStart, 'v' ) EQ NULL
     && (strchr (lppnLocalVars->lpszStart, 'r' ) NE NULL
      || strstr (lppnLocalVars->lpszStart, "x ") NE NULL
      || lppnLocalVars->lpszStart[lppnLocalVars->uNumLen - 1] EQ 'x'))
    {
        // Weed out non-RAT expressions such as 2g3 and 2p3
        if (strchr (lppnLocalVars->lpszStart, 'g' ) EQ NULL
         && strchr (lppnLocalVars->lpszStart, 'p' ) EQ NULL)
        {
            LPCHAR p;

            // Weed out non-RAT expressions such as 2x3
            p = strchr (lppnLocalVars->lpszStart, 'x');
            while (p NE NULL && (p[1] EQ ' ' || p[1] EQ AC_EOS))
                p = strchr (p + 1, 'x');
            if (p EQ NULL)
                // Save for later use
                lppnLocalVars->bRatSep = TRUE;
        } // End IF
    } // End IF

#if YYDEBUG
    // Enable debugging
    yydebug = TRUE;
#endif
    __try
    {
        // Parse the header, check for errors
        //   0 = success
        //   1 = YYABORT or APL error
        //   2 = memory exhausted
        bRet = pn_yyparse (lppnLocalVars) EQ 0;
    } __except (CheckException (GetExceptionInformation (), L"ParsePointNotation"))
    {
        // Split cases based upon the ExceptionCode
        switch (MyGetExceptionCode ())
        {
            case EXCEPTION_NONCE_ERROR:
                // Mark as in error
                bRet = FALSE;
                ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                                           NULL);
                break;

            default:
                // Display message for unhandled exception
                DisplayException ();

                // Mark as in error
                bRet = FALSE;

                break;
        } // End SWITCH
    } // End __try/__except

#if YYDEBUG
    // Disable debugging
    yydebug = FALSE;
#endif
    return bRet;
} // End ParsePointNotation
#undef  APPEND_NAME


//***************************************************************************
//  $pn_yylex
//
//  Lexical analyzer for Bison
//***************************************************************************

int pn_yylex
    (LPPN_YYSTYPE  lpYYLval,            // Ptr to lval
     LPPNLOCALVARS lppnLocalVars)       // Ptr to local pnLocalVars

{
    // Save the index position in lpszStart
    lpYYLval->uNumStart = lppnLocalVars->uNumCur;

    // Check for EOL
    if (lppnLocalVars->uNumCur EQ lppnLocalVars->uNumLen)
        lpYYLval->chCur = AC_EOS;
    else
    {
        // Get the next char
        lpYYLval->chCur = lppnLocalVars->lpszStart[lppnLocalVars->uNumCur++];

        // If the character is 'x' and
        //   is the last in the sequence, ...
        if (lpYYLval->chCur EQ 'x'
         && (lppnLocalVars->uNumCur EQ lppnLocalVars->uNumLen
          || IsWhite (lppnLocalVars->lpszStart[lppnLocalVars->uNumCur])))
            return EXT;

        // If the character is overbar, ...
        if (lpYYLval->chCur EQ OVERBAR1)
            return OVR;

        // If the character is infinity, ...
        if (lpYYLval->chCur EQ INFINITY1)
            return INF;
    } // End IF/ELSE

    // Return it
    return lpYYLval->chCur;
} // End pn_yylex


//***************************************************************************
//  $pn_yyerror
//
//  Error callback from Bison
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- pn_yyerror"
#else
#define APPEND_NAME
#endif

void pn_yyerror                     // Called for Bison syntax error
    (LPPNLOCALVARS lppnLocalVars,   // Ptr to local pnLocalVars
     const char   *s)               // Ptr to error msg

{
    char    szTemp[1024];
    LPWCHAR lpwszTemp;

    DbgMsg ((char *) s);

    // Check for SYNTAX ERROR
#define ERR     PN_SYNTAX
    if (MyStrcpyn (szTemp, sizeof (szTemp), s, strsizeof (ERR)),    // Note: Terminates the string, too
        lstrcmp (szTemp, ERR) EQ 0)
#undef  ERR
        lpwszTemp = ERRMSG_SYNTAX_ERROR APPEND_NAME;
    else
    // Check for NONCE ERROR
#define ERR     PN_NONCE
    if (MyStrcpyn (szTemp, sizeof (szTemp), s, strsizeof (ERR)),    // Note: Terminates the string, too
        lstrcmp (szTemp, ERR) EQ 0)
#undef  ERR
    {
        lpwszTemp = ERRMSG_NONCE_ERROR APPEND_NAME;

        // Back off by one so the caret points to the symbol in error
        lppnLocalVars->uNumCur--;
    } else
    // Check for WS FULL
#define ERR     PN_WSFULL
    if (MyStrcpyn (szTemp, sizeof (szTemp), s, strsizeof (ERR)),    // Note: Terminates the string, too
        lstrcmp (szTemp, ERR) EQ 0)
#undef  ERR
        lpwszTemp = ERRMSG_WS_FULL APPEND_NAME;
    else
    // Check for DOMAIN ERROR
#define ERR     PN_DOMAIN
    if (MyStrcpyn (szTemp, sizeof (szTemp), s, strsizeof (ERR)),    // Note: Terminates the string, too
        lstrcmp (szTemp, ERR) EQ 0)
#undef  ERR
        lpwszTemp = ERRMSG_DOMAIN_ERROR APPEND_NAME;
    else
        lpwszTemp = ERRMSG_PN_PARSE APPEND_NAME;

    // Save the error message
    ErrorMessageIndirect (lpwszTemp);

    // Set the error char index
    lppnLocalVars->uCharIndex += lppnLocalVars->uNumCur;

    // Set the error flag
    lppnLocalVars->bYYERROR = TRUE;

    // If we're at the end, ...
    if (lppnLocalVars->lpszStart[lppnLocalVars->uNumCur] EQ WC_EOS)
        // Back off to the last char
        lppnLocalVars->uCharIndex--;

    if (lppnLocalVars->hGlbRes NE NULL)
    {
        if (lppnLocalVars->lpMemHdrRes NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (lppnLocalVars->hGlbRes); lppnLocalVars->lpMemHdrRes = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (lppnLocalVars->hGlbRes); lppnLocalVars->hGlbRes = NULL;
    } // End IF
} // End pn_yyerror
#undef  APPEND_NAME


//***************************************************************************
//  $pn_yyfprintf
//
//  Debugger output
//***************************************************************************

void pn_yyfprintf
    (FILE  *hfile,          // Ignore this
     LPCHAR lpszFmt,        // Format string
     ...)                   // Zero or more arguments

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
} // End pn_yyfprintf


//***************************************************************************
//  End of File: pn_parse.y
//***************************************************************************
