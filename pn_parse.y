//***************************************************************************
//  NARS2000 -- Parser Grammar for XXX Point Notation
//***************************************************************************

//***************************************************************************
//  Parse a line of XXX Point Notation characters.
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
%name-prefix="pn_yy"
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

AlphaInt:
               Alphabet             {DbgMsgWP (L"%%AlphaInt:  Alphabet");
                                     PN_ChrAcc (lppnLocalVars, $1.chCur);
                                    }
    |          Digit                {DbgMsgWP (L"%%AlphaInt:  Digit");
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

                                     // Mark the result as integer
                                     $1.chType = PN_NUMTYPE_INT;

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
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_FLT,  1);
                                    }
    | OVR INF                       {DbgMsgWP (L"%%DecConstants:  OVR INF");
                                     // Set constant infinity
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_FLT, -1);
                                    }
    ;

VfpConstants:
          INF DEF_VFPSEP            {DbgMsgWP (L"%%VfpConstants:  INF 'v'");
                                     // Set constant infinity
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_VFP,  1);
                                    }
    | OVR INF DEF_VFPSEP            {DbgMsgWP (L"%%VfpConstants:  OVR INF 'v'");
                                     // Set constant infinity
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_VFP, -1);
                                    }
    ;

// Complex Point
/// Hc2Point:
///       DecPoint 'i' DecPoint         {DbgMsgWP (L"%%Hc2Point:  DecPoint 'i' DecPoint");
///                                      // If the real part is integer, ...
///                                      if (IsIntegerType ($1.chType))
///                                          // Convert it to float
///                                          $1.at.aplFloat = (APLFLOAT) $1.at.aplInteger;
///                                      // If the imaginary part is integer, ...
///                                      if (IsIntegerType ($3.chType))
///                                          // Convert it to float
///                                          $3.at.aplFloat = (APLFLOAT) $3.at.aplInteger;
///
///                                      // Save the real & imaginary parts
///                                      $1.at.aplHC2.Real = $1.at.aplFloat;
///                                      $1.at.aplHC2.Imag = $3.at.aplFloat;
///
///                                      // Mark the result as Complex
///                                      $1.chType = PN_NUMTYPE_HC2;
///
///                                      $$ = $1;
///                                     }
///     | DecPoint 'J' DecPoint         {DbgMsgWP (L"%%Hc2Point:  DecPoint 'J' DecPoint");
///                                      // If the real part is integer, ...
///                                      if (IsIntegerType ($1.chType))
///                                          // Convert it to float
///                                          $1.at.aplFloat = (APLFLOAT) $1.at.aplInteger;
///                                      // If the imaginary part is integer, ...
///                                      if (IsIntegerType ($3.chType))
///                                          // Convert it to float
///                                          $3.at.aplFloat = (APLFLOAT) $3.at.aplInteger;
///
///                                      // Save the real & imaginary parts
///                                      $1.at.aplHC2.Real = $1.at.aplFloat;
///                                      $1.at.aplHC2.Imag = $3.at.aplFloat;
///
///                                      // Mark the result as Complex
///                                      $1.chType = PN_NUMTYPE_HC2;
///
///                                      $$ = $1;
///                                     }
///     | DecPoint 'a' 'd' DecPoint     {DbgMsgWP (L"%%Hc2Point:  DecPoint 'a' 'd' DecPoint");
///                                      // If the real part is integer, ...
///                                      if (IsIntegerType ($1.chType))
///                                          // Convert it to float
///                                          $1.at.aplFloat = (APLFLOAT) $1.at.aplInteger;
///                                      // If the imaginary part is integer, ...
///                                      if (IsIntegerType ($4.chType))
///                                          // Convert it to float
///                                          $4.at.aplFloat = (APLFLOAT) $4.at.aplInteger;
///
///                                      // Convert from degrees to radians
///                                      $4.at.aplFloat = FloatPi * $4.at.aplFloat / 180;
///
///                                      // Save the real & imaginary parts
///                                      // ***FIXME*** -- Do we need to reduce the radians modulo Pi/2 ??
///                                      $1.at.aplHC2.Real = $1.at.aplFloat * cos ($4.at.aplFloat);
///                                      $1.at.aplHC2.Imag = $1.at.aplFloat * sin ($4.at.aplFloat);
///
///                                      // Mark the result as Complex
///                                      $1.chType = PN_NUMTYPE_HC2;
///
///                                      $$ = $1;
///                                     }
///     | DecPoint 'a' 'r' DecPoint     {DbgMsgWP (L"%%Hc2Point:  DecPoint 'a' 'r' DecPoint");
///                                      // If the real part is integer, ...
///                                      if (IsIntegerType ($1.chType))
///                                          // Convert it to float
///                                          $1.at.aplFloat = (APLFLOAT) $1.at.aplInteger;
///                                      // If the imaginary part is integer, ...
///                                      if (IsIntegerType ($4.chType))
///                                          // Convert it to float
///                                          $4.at.aplFloat = (APLFLOAT) $4.at.aplInteger;
///
///                                      // Save the real & imaginary parts
///                                      // ***FIXME*** -- Do we need to reduce the radians modulo Pi/2 ??
///                                      $1.at.aplHC2.Real = $1.at.aplFloat * cos ($4.at.aplFloat);
///                                      $1.at.aplHC2.Imag = $1.at.aplFloat * sin ($4.at.aplFloat);
///
///                                      // Mark the result as Complex
///                                      $1.chType = PN_NUMTYPE_HC2;
///
///                                      $$ = $1;
///                                     }
///     ;
///
/// // Quaternion Point
/// Hc4Point:
///       DecPoint 'i' DecPoint 'j' DecPoint 'i' 'j' DecPoint
///                                     {DbgMsgWP (L"%%Hc4Point:  DecPoint 'i' DecPoint 'j' DecPoint 'i' 'j' DecPoint");
///                                      // If the real part is integer, ...
///                                      if (IsIntegerType ($1.chType))
///                                          // Convert it to float
///                                          $1.at.aplFloat = (APLFLOAT) $1.at.aplInteger;
///                                      // If the imaginary part #1 is integer, ...
///                                      if (IsIntegerType ($3.chType))
///                                          // Convert it to float
///                                          $3.at.aplFloat = (APLFLOAT) $3.at.aplInteger;
///                                      // If the imaginary part #2 is integer, ...
///                                      if (IsIntegerType ($5.chType))
///                                          // Convert it to float
///                                          $5.at.aplFloat = (APLFLOAT) $5.at.aplInteger;
///                                      // If the imaginary part #3 is integer, ...
///                                      if (IsIntegerType ($8.chType))
///                                          // Convert it to float
///                                          $8.at.aplFloat = (APLFLOAT) $8.at.aplInteger;
///
///                                      // Save the real & imaginary parts
///                                      $1.at.aplHC4.Real  = $1.at.aplFloat;
///                                      $1.at.aplHC4.Imag1 = $3.at.aplFloat;
///                                      $1.at.aplHC4.Imag2 = $5.at.aplFloat;
///                                      $1.at.aplHC4.Imag3 = $8.at.aplFloat;
///
///                                      // Mark the result as Quaternion
///                                      $1.chType = PN_NUMTYPE_HC4;
///
///                                      $$ = $1;
///                                     }
///     | DecPoint 'i' DecPoint 'j' DecPoint 'k' DecPoint
///                                     {DbgMsgWP (L"%%Hc4Point:  DecPoint 'i' DecPoint 'j' DecPoint 'k' DecPoint");
///                                      // If the real part is integer, ...
///                                      if (IsIntegerType ($1.chType))
///                                          // Convert it to float
///                                          $1.at.aplFloat = (APLFLOAT) $1.at.aplInteger;
///                                      // If the imaginary part #1 is integer, ...
///                                      if (IsIntegerType ($3.chType))
///                                          // Convert it to float
///                                          $3.at.aplFloat = (APLFLOAT) $3.at.aplInteger;
///                                      // If the imaginary part #2 is integer, ...
///                                      if (IsIntegerType ($5.chType))
///                                          // Convert it to float
///                                          $5.at.aplFloat = (APLFLOAT) $5.at.aplInteger;
///                                      // If the imaginary part #3 is integer, ...
///                                      if (IsIntegerType ($7.chType))
///                                          // Convert it to float
///                                          $7.at.aplFloat = (APLFLOAT) $7.at.aplInteger;
///
///                                      // Save the real & imaginary parts
///                                      $1.at.aplHC4.Real  = $1.at.aplFloat;
///                                      $1.at.aplHC4.Imag1 = $3.at.aplFloat;
///                                      $1.at.aplHC4.Imag2 = $5.at.aplFloat;
///                                      $1.at.aplHC4.Imag3 = $7.at.aplFloat;
///
///                                      // Mark the result as Quaternion
///                                      $1.chType = PN_NUMTYPE_HC4;
///
///                                      $$ = $1;
///                                     }
///     ;
///
/// // Octonion Point
/// Hc8Point:
///       DecPoint 'i' DecPoint 'j' DecPoint 'k' DecPoint 'l' DecPoint 'i' 'j' DecPoint 'i' 'k' DecPoint 'i' 'l' DecPoint
///                                     {DbgMsgWP (L"%%Hc4Point:  DecPoint 'i' DecPoint 'j' DecPoint 'k' DecPoint 'l' DecPoint 'i' 'j' DecPoint 'i' 'k' DecPoint 'i' 'l' DecPoint");
///                                      // If the real part is integer, ...
///                                      if (IsIntegerType ($1.chType))
///                                          // Convert it to float
///                                          $1.at.aplFloat = (APLFLOAT) $1.at.aplInteger;
///                                      // If the imaginary part #1 is integer, ...
///                                      if (IsIntegerType ($3.chType))
///                                          // Convert it to float
///                                          $3.at.aplFloat = (APLFLOAT) $3.at.aplInteger;
///                                      // If the imaginary part #2 is integer, ...
///                                      if (IsIntegerType ($5.chType))
///                                          // Convert it to float
///                                          $5.at.aplFloat = (APLFLOAT) $5.at.aplInteger;
///                                      // If the imaginary part #3 is integer, ...
///                                      if (IsIntegerType ($7.chType))
///                                          // Convert it to float
///                                          $7.at.aplFloat = (APLFLOAT) $7.at.aplInteger;
///                                      // If the imaginary part #4 is integer, ...
///                                      if (IsIntegerType ($9.chType))
///                                          // Convert it to float
///                                          $9.at.aplFloat = (APLFLOAT) $9.at.aplInteger;
///                                      // If the imaginary part #5 is integer, ...
///                                      if (IsIntegerType ($12.chType))
///                                          // Convert it to float
///                                          $12.at.aplFloat = (APLFLOAT) $12.at.aplInteger;
///                                      // If the imaginary part #6 is integer, ...
///                                      if (IsIntegerType ($15.chType))
///                                          // Convert it to float
///                                          $15.at.aplFloat = (APLFLOAT) $15.at.aplInteger;
///                                      // If the imaginary part #7 is integer, ...
///                                      if (IsIntegerType ($18.chType))
///                                          // Convert it to float
///                                          $18.at.aplFloat = (APLFLOAT) $18.at.aplInteger;
///
///                                      // Save the real & imaginary parts
///                                      $1.at.aplHC8.Real  = $1.at.aplFloat;
///                                      $1.at.aplHC8.Imag1 = $3.at.aplFloat;
///                                      $1.at.aplHC8.Imag2 = $5.at.aplFloat;
///                                      $1.at.aplHC8.Imag3 = $7.at.aplFloat;
///                                      $1.at.aplHC8.Imag4 = $9.at.aplFloat;
///                                      $1.at.aplHC8.Imag5 = $12.at.aplFloat;
///                                      $1.at.aplHC8.Imag6 = $15.at.aplFloat;
///                                      $1.at.aplHC8.Imag7 = $18.at.aplFloat;
///
///                                      // Mark the result as Octonion
///                                      $1.chType = PN_NUMTYPE_HC8;
///
///                                      $$ = $1;
///                                     }
///     ;

Rational:
      Integer  DEF_RATSEP Digit     {DbgMsgWP (L"%%Rational:  Integer 'r' Digit");
                                     // Terminate the argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     // Mark the result as integer
                                     $1.chType = PN_NUMTYPE_INT;

                                     // Calculate the number
                                     PN_NumCalc (lppnLocalVars, &$1, TRUE);

                                     // Mark starting offset
                                     $1.uNumAcc = lppnLocalVars->uNumAcc;

                                     // Accumulate the digit
                                     PN_NumAcc (lppnLocalVars, $3.chCur);

                                     $$ = $1;
                                    }
    | Integer  DEF_RATSEP OVR Digit {DbgMsgWP (L"%%Rational:  Integer 'r' '" WS_UTF16_OVERBAR L"' Digit");
                                     // Terminate the argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     // Mark the result as integer
                                     $1.chType = PN_NUMTYPE_INT;

                                     // Calculate the number
                                     PN_NumCalc (lppnLocalVars, &$1, TRUE);

                                     // Mark starting offset
                                     $1.uNumAcc = lppnLocalVars->uNumAcc;

                                     // Accumulate the negative sign
                                     PN_NumAcc (lppnLocalVars, OVERBAR1);

                                     // Accumulate the digit
                                     PN_NumAcc (lppnLocalVars, $4.chCur);

                                     $$ = $1;
                                    }
    | Rational         Digit        {DbgMsgWP (L"%%Rational:  Rational Digit");
                                     // Accumulate the digit
                                     PN_NumAcc (lppnLocalVars, $2.chCur);
                                    }
    ;

RatPoint:
      Rational                      {DbgMsgWP (L"%%RatPoint:  Rational");
                                     // Terminate the argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     // Mark the result as rational
                                     $1.chType = PN_NUMTYPE_RAT;

                                     // Calculate the number
                                     PN_NumCalc (lppnLocalVars, &$1, TRUE);

                                     $$ = $1;
                                    }
    ;

RatConstants:
          INF EXT                   {DbgMsgWP (L"%%RatConstants:  INF 'x'");
                                     // Set constant infinity
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_RAT,  1);
                                    }
    |     INF DEF_RATSEP Integer    {DbgMsgWP (L"%%RatConstants:  INF 'r' Integer");
                                     // Set constant infinity
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_RAT,  1);
                                    }
    | OVR INF EXT                   {DbgMsgWP (L"%%RatConstants:  OVR INF 'x'");
                                     // Set constant infinity
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_RAT, -1);
                                    }
    | OVR INF DEF_RATSEP Integer    {DbgMsgWP (L"%%RatConstants:  OVR INF 'r' Integer");
                                     // Set constant infinity
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_RAT, -1);
                                    }
    | Integer DEF_RATSEP INF        {DbgMsgWP (L"%%RatConstants:  Integer 'r' INF");
                                     // Initialize to 0/1
                                     mpq_init (&$1.at.aplRat);

                                     // Mark the result as RAT
                                     $1.chType = PN_NUMTYPE_RAT;

                                     $$ = $1;
                                    }
    | Integer DEF_RATSEP OVR INF    {DbgMsgWP (L"%%RatConstants:  Integer 'r' OVR INF");
                                     // Initialize to 0/1
                                     mpq_init (&$1.at.aplRat);

                                     // Mark the result as RAT
                                     $1.chType = PN_NUMTYPE_RAT;

                                     $$ = $1;
                                    }
    ;

ExtPoint:
      Integer EXT                   {DbgMsgWP (L"%%ExtPoint:  Integer 'x'");
                                     // Terminate the argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     // Mark the result as integer
                                     $1.chType = PN_NUMTYPE_INT;

                                     // Calculate the number
                                     PN_NumCalc (lppnLocalVars, &$1, TRUE);

                                     // Set the denominator to 1
                                     mpz_init_set_ui (mpq_denref (&$1.at.aplRat), 1);

                                     // Mark the result as rational
                                     $1.chType = PN_NUMTYPE_RAT;

                                     $$ = $1;
                                    }
    ;

// Euler/Pi left and right arguments
EPArgs:
      DecPoint                      {DbgMsgWP (L"%%EPArgs:  DecPoint");
                                    }
    | ExpPoint                      {DbgMsgWP (L"%%EPArgs:  ExpPoint");
                                    }
////| Hc2Point                      {DbgMsgWP (L"%%EPArgs:  Hc2Point");
////                                }
////| Hc4Point                      {DbgMsgWP (L"%%EPArgs:  Hc4Point");
////                                }
////| Hc8Point                      {DbgMsgWP (L"%%EPArgs:  Hc8Point");
////                                }
    | RatPoint                      {DbgMsgWP (L"%%EPArgs:  RatPoint");
                                    }
    | VfpPoint                      {DbgMsgWP (L"%%EPArgs:  VfpPoint");
                                    }
    ;

PiPoint:
      EPArgs    'p' EPArgs          {DbgMsgWP (L"%%PiPoint:  EPArgs 'p' EPArgs");
                                     $$ = *PN_MakePiPoint (lppnLocalVars, &$1, &$3);
                                    }
    ;

ExpPoint:
      DecPoint 'e' Integer          {DbgMsgWP (L"%%ExpPoint:  DecPoint 'e' Integer");
                                     // Terminate the (Exponent) argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     lppnLocalVars->lpYYRes = PN_MakeExpPoint (lppnLocalVars, &$1, &$3);
                                     if (!lppnLocalVars->lpYYRes)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    | DecPoint 'E' Integer          {DbgMsgWP (L"%%ExpPoint:  DecPoint 'E' Integer");
                                     // Terminate the (Exponent) argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     lppnLocalVars->lpYYRes = PN_MakeExpPoint (lppnLocalVars, &$1, &$3);
                                     if (!lppnLocalVars->lpYYRes)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    ;

VfpPoint:
      DecPoint DEF_VFPSEP           {DbgMsgWP (L"%%VfpPoint:  DecPoint 'v'");
                                     $$ = *PN_MakeVfpPoint (lppnLocalVars, &$1, NULL);
                                    }
    | DecPoint 'e' Integer DEF_VFPSEP
                                    {DbgMsgWP (L"%%VfpPoint:  DecPoint 'e' Integer 'v'");
                                     // Terminate the (Exponent) argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     $$ = *PN_MakeVfpPoint (lppnLocalVars, &$1, &$3);
                                    }
    | DecPoint 'E' Integer DEF_VFPSEP
                                    {DbgMsgWP (L"%%VfpPoint:  DecPoint 'E' Integer 'v'");
                                     // Terminate the (Exponent) argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     $$ = *PN_MakeVfpPoint (lppnLocalVars, &$1, &$3);
                                    }
    ;

EulerPoint:
      EPArgs   'x' EPArgs           {DbgMsgWP (L"%%EulerPoint:  EPArgs 'x' EPArgs");
                                     $$ = *PN_MakeEulerPoint (lppnLocalVars, &$1, &$3);
                                    }
    ;

BasePoint:
      DecPoint   'b' AlphaInt       {DbgMsgWP (L"%%BasePoint:  DecPoint 'b' AlphaInt");
                                     $$ = *PN_MakeBasePoint (lppnLocalVars, &$1, &$3);
                                    }
    | EulerPoint 'b' AlphaInt       {DbgMsgWP (L"%%BasePoint:  EulerPoint 'b' AlphaInt");
                                     $$ = *PN_MakeBasePoint (lppnLocalVars, &$1, &$3);
                                    }
    | ExpPoint   'b' AlphaInt       {DbgMsgWP (L"%%BasePoint:  ExpPoint 'b' AlphaInt");
                                     $$ = *PN_MakeBasePoint (lppnLocalVars, &$1, &$3);
                                    }
    | PiPoint    'b' AlphaInt       {DbgMsgWP (L"%%BasePoint:  PiPoint 'b' AlphaInt");
                                     $$ = *PN_MakeBasePoint (lppnLocalVars, &$1, &$3);
                                    }
    | RatPoint   'b' AlphaInt       {DbgMsgWP (L"%%BasePoint:  RatPoint 'b' AlphaInt");
                                     $$ = *PN_MakeBasePoint (lppnLocalVars, &$1, &$3);
                                    }
////| Hc2Point   'b' AlphaInt       {DbgMsgWP (L"%%BasePoint:  Hc2Point 'b' AlphaInt");
////                                 $$ = *PN_MakeBasePoint (lppnLocalVars, &$1, &$3);
////                                }
////| Hc4Point   'b' AlphaInt       {DbgMsgWP (L"%%BasePoint:  Hc4Point 'b' AlphaInt");
////                                 $$ = *PN_MakeBasePoint (lppnLocalVars, &$1, &$3);
////                                }
////| Hc8Point   'b' AlphaInt       {DbgMsgWP (L"%%BasePoint:  Hc8Point 'b' AlphaInt");
////                                 $$ = *PN_MakeBasePoint (lppnLocalVars, &$1, &$3);
////                                }
    | VfpPoint   'b' AlphaInt       {DbgMsgWP (L"%%BasePoint:  VfpPoint 'b' AlphaInt");
                                     $$ = *PN_MakeBasePoint (lppnLocalVars, &$1, &$3);
                                    }
    ;

Number:
      BasePoint                     {DbgMsgWP (L"%%Number:  BasePoint");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | DecPoint                      {DbgMsgWP (L"%%Number:  DecPoint");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | EulerPoint                    {DbgMsgWP (L"%%Number:  EulerPoint");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | ExpPoint                      {DbgMsgWP (L"%%Number:  ExpPoint");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | ExtPoint                      {DbgMsgWP (L"%%Number:  ExtPoint");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | PiPoint                       {DbgMsgWP (L"%%Number:  PiPoint");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | RatPoint                      {DbgMsgWP (L"%%Number:  RatPoint");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
////| Hc2Point                      {DbgMsgWP (L"%%Number:  Hc2Point");
////                                 lppnLocalVars->at             = $1.at;
////                                 lppnLocalVars->chType         = $1.chType;
////                                }
////| Hc4Point                      {DbgMsgWP (L"%%Number:  Hc4Point");
////                                 lppnLocalVars->at             = $1.at;
////                                 lppnLocalVars->chType         = $1.chType;
////                                }
////| Hc8Point                      {DbgMsgWP (L"%%Number:  Hc8Point");
////                                 lppnLocalVars->at             = $1.at;
////                                 lppnLocalVars->chType         = $1.chType;
////                                }
    | VfpPoint                      {DbgMsgWP (L"%%Number:  VfpPoint");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | DecConstants                  {DbgMsgWP (L"%%Number:  DecConstants");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | RatConstants                  {DbgMsgWP (L"%%Number:  RatConstants");
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
                                     if (!PN_VectorAcc (lppnLocalVars))
                                         YYERROR2;
                                    }
    | VectorAcc Space Number        {DbgMsgWP (L"%%VectorAcc:  VectorAcc Space Number");
                                     // Accumulate the current value into the vector
                                     if (!PN_VectorAcc (lppnLocalVars))
                                         YYERROR2;
                                    }
    ;

Space:
            ' '                     {DbgMsgWP (L"%%Space:  ' '");
                                    }
    | Space ' '                     {DbgMsgWP (L"%%Space:  Space ' '");
////                                 Assert (IsWhite (lppnLocalVars->lpszStart[lppnLocalVars->uNumCur]));
////                                 lppnLocalVars->uNumCur++;
////                                 lppnLocalVars->uNumIni = lppnLocalVars->uNumCur;
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
//  $PN_actIDENT
//***************************************************************************

void PN_actIDENT
    (LPPN_YYSTYPE lpSrc)

{
} // End PN_actIDENT


//***************************************************************************
//  $PN_actUNK
//***************************************************************************

void PN_actUNK
    (LPPN_YYSTYPE lpSrc)

{
    DbgStop ();
} // End PN_actUNK


//***************************************************************************
//  $PN_actBOOL_FLT
//***************************************************************************

void PN_actBOOL_FLT
    (LPPN_YYSTYPE lpSrc)

{
    // Convert the value from BOOL to FLT
    lpSrc->at.aplFloat = (APLFLOAT) lpSrc->at.aplInteger;

    lpSrc->chType = PN_NUMTYPE_FLT;
} // End PN_actBOOL_FLT


//***************************************************************************
//  $PN_actBOOL_RAT
//***************************************************************************

void PN_actBOOL_RAT
    (LPPN_YYSTYPE lpSrc)

{
    // Convert the value from BOOL to RAT
    mpq_init_set_sx (&lpSrc->at.aplRat, lpSrc->at.aplInteger, 1);

    lpSrc->chType = PN_NUMTYPE_RAT;
} // End PN_actBOOL_RAT


//***************************************************************************
//  $PN_actBOOL_VFP
//***************************************************************************

void PN_actBOOL_VFP
    (LPPN_YYSTYPE lpSrc)

{
    // Convert the value from BOOL to VFP
    mpfr_init_set_sx (&lpSrc->at.aplVfp, lpSrc->at.aplInteger, MPFR_RNDN);

    lpSrc->chType = PN_NUMTYPE_VFP;
} // End PN_actBOOL_VFP


//***************************************************************************
//  $PN_actINT_FLT
//***************************************************************************

void PN_actINT_FLT
    (LPPN_YYSTYPE lpSrc)

{
    // Convert the value from INT to FLT
    lpSrc->at.aplFloat = (APLFLOAT) lpSrc->at.aplInteger;

    lpSrc->chType = PN_NUMTYPE_FLT;
} // End PN_actINT_FLT


//***************************************************************************
//  $PN_actINT_RAT
//***************************************************************************

void PN_actINT_RAT
    (LPPN_YYSTYPE lpSrc)

{
    // Convert the value from INT to RAT
    mpq_init_set_sx (&lpSrc->at.aplRat, lpSrc->at.aplInteger, 1);

    lpSrc->chType = PN_NUMTYPE_RAT;
} // End PN_actINT_RAT


//***************************************************************************
//  $PN_actINT_VFP
//***************************************************************************

void PN_actINT_VFP
    (LPPN_YYSTYPE lpSrc)

{
    // Convert the value from INT to VFP
    mpfr_init_set_sx (&lpSrc->at.aplVfp, lpSrc->at.aplInteger, MPFR_RNDN);

    lpSrc->chType = PN_NUMTYPE_VFP;
} // End PN_actINT_VFP


//***************************************************************************
//  $PN_actFLT_VFP
//***************************************************************************

void PN_actFLT_VFP
    (LPPN_YYSTYPE lpSrc)

{
    // Convert the value from FLT to VFP
    mpfr_init_set_d  (&lpSrc->at.aplVfp, lpSrc->at.aplFloat  , MPFR_RNDN);

    lpSrc->chType = PN_NUMTYPE_VFP;
} // End PN_actFLT_VFP


//***************************************************************************
//  $PN_actRAT_VFP
//***************************************************************************

void PN_actRAT_VFP
    (LPPN_YYSTYPE lpSrc)

{
    APLVFP aplVfp = {0};

    // Convert the value from RAT to VFP
    mpfr_init_set_q  (&aplVfp, &lpSrc->at.aplRat, MPFR_RNDN);

    Myq_clear (&lpSrc->at.aplRat);

    lpSrc->at.aplVfp = aplVfp;

    lpSrc->chType = PN_NUMTYPE_VFP;
} // End PN_actRAT_VFP


#define PN_MAT                                                                                                                           \
{/*     BOOL          INT          FLT          HC2          HC4          HC8          RAT          VFP         INIT     */              \
   {M(BOOL,BOOL),M(BOOL,INT ),M(BOOL,FLT ),M(BOOL,HC2 ),M(BOOL,HC4 ),M(BOOL,HC8 ),M(BOOL,RAT ),M(BOOL,VFP ),M(BOOL,BOOL)},   /* BOOL */  \
   {M(INT ,INT ),M(INT ,INT ),M(INT ,FLT ),M(INT ,HC2 ),M(INT ,HC4 ),M(INT ,HC8 ),M(INT ,RAT ),M(INT ,VFP ),M(INT ,INT )},   /* INT  */  \
   {M(FLT ,FLT ),M(FLT ,FLT ),M(FLT ,FLT ),M(FLT ,HC2 ),M(FLT ,HC4 ),M(FLT ,HC8 ),M(FLT ,VFP ),M(FLT ,VFP ),M(FLT ,FLT )},   /* FLT  */  \
   {M(HC2 ,HC2 ),M(HC2 ,HC2 ),M(HC2 ,HC2 ),M(HC2 ,HC2 ),M(HC2 ,HC4 ),M(HC2 ,HC8 ),M(HC2 ,HC2 ),M(HC2 ,HC2 ),M(HC2 ,HC2 )},   /* HC2  */  \
   {M(HC4 ,HC4 ),M(HC4 ,HC4 ),M(HC4 ,HC4 ),M(HC4 ,HC4 ),M(HC4 ,HC4 ),M(HC4 ,HC8 ),M(HC4 ,HC4 ),M(HC4 ,HC4 ),M(HC4 ,HC4 )},   /* HC4  */  \
   {M(HC8 ,HC8 ),M(HC8 ,HC8 ),M(HC8 ,HC8 ),M(HC8 ,HC8 ),M(HC8 ,HC8 ),M(HC8 ,HC8 ),M(HC8 ,HC8 ),M(HC8 ,HC8 ),M(HC8 ,HC8 )},   /* HC8  */  \
   {M(RAT ,RAT ),M(RAT ,RAT ),M(RAT ,VFP ),M(RAT ,HC2 ),M(RAT ,HC4 ),M(RAT ,HC8 ),M(RAT ,RAT ),M(RAT ,VFP ),M(RAT ,RAT )},   /* RAT  */  \
   {M(VFP ,VFP ),M(VFP ,VFP ),M(VFP ,VFP ),M(VFP ,HC2 ),M(VFP ,HC4 ),M(VFP ,HC8 ),M(VFP ,VFP ),M(VFP ,VFP ),M(VFP ,VFP )},   /* VFP  */  \
   {M(INIT,BOOL),M(INIT,INT ),M(INIT,FLT ),M(INIT,HC2 ),M(INIT,HC4 ),M(INIT,HC8 ),M(INIT,RAT ),M(INIT,VFP ),M(INIT,INIT)},   /* INIT */  \
};
// PN_NUMTYPE_xxx promotion result matrix
PNNUMTYPE aNumTypePromote[PN_NUMTYPE_LENGTH][PN_NUMTYPE_LENGTH] =
#define M(a,b)  PN_NUMTYPE_##b
PN_MAT
#undef  M

typedef void (*PN_ACTION)(LPPN_YYSTYPE);

#define PN_actBOOL_BOOL     PN_actIDENT
#define PN_actBOOL_INT      PN_actIDENT
////ine PN_actBOOL_FLT      NULL
#define PN_actBOOL_HC2      PN_actUNK
#define PN_actBOOL_HC4      PN_actUNK
#define PN_actBOOL_HC8      PN_actUNK

#define PN_actINT_INT       PN_actIDENT
////ine PN_actINT_FLT       NULL
#define PN_actINT_HC2       PN_actUNK
#define PN_actINT_HC4       PN_actUNK
#define PN_actINT_HC8       PN_actUNK

#define PN_actFLT_FLT       PN_actIDENT
#define PN_actFLT_HC2       PN_actUNK
#define PN_actFLT_HC4       PN_actUNK
#define PN_actFLT_HC8       PN_actUNK

#define PN_actHC2_HC2       PN_actIDENT
#define PN_actHC2_HC4       PN_actUNK
#define PN_actHC2_HC8       PN_actUNK

#define PN_actHC4_HC4       PN_actIDENT
#define PN_actHC4_HC8       PN_actUNK

#define PN_actHC8_HC8       PN_actIDENT

#define PN_actRAT_RAT       PN_actIDENT
#define PN_actRAT_HC2       PN_actUNK
#define PN_actRAT_HC4       PN_actUNK
#define PN_actRAT_HC8       PN_actUNK

#define PN_actVFP_VFP       PN_actIDENT
#define PN_actVFP_HC2       PN_actUNK
#define PN_actVFP_HC4       PN_actUNK
#define PN_actVFP_HC8       PN_actUNK

#define PN_actINIT_BOOL     PN_actUNK
#define PN_actINIT_INT      PN_actUNK
#define PN_actINIT_FLT      PN_actUNK
#define PN_actINIT_HC2      PN_actUNK
#define PN_actINIT_HC4      PN_actUNK
#define PN_actINIT_HC8      PN_actUNK
#define PN_actINIT_RAT      PN_actUNK
#define PN_actINIT_VFP      PN_actUNK
#define PN_actINIT_INIT     PN_actUNK

// PN_NUMTYPE_xxx promotion action matrix
PN_ACTION aNumTypeAction [PN_NUMTYPE_LENGTH][PN_NUMTYPE_LENGTH] =
#define M(a,b)  PN_act##a##_##b
PN_MAT
#undef  M


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
    // Check for EOL
    if (lppnLocalVars->uNumCur EQ lppnLocalVars->uNumLen)
        lpYYLval->chCur = '\0';
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
     LPCHAR        s)               // Ptr to error msg

{
    char    szTemp[1024];
    LPWCHAR lpwszTemp;

    DbgMsg (s);

    // Check for SYNTAX ERROR
#define ERR     "syntax error"
    if (lstrcpyn (szTemp, s, sizeof (ERR)),     // Note: Terminates the string, too
        lstrcmp (szTemp, ERR) EQ 0)
#undef  ERR
        lpwszTemp = ERRMSG_SYNTAX_ERROR APPEND_NAME;
    else
    // Check for NONCE ERROR
#define ERR     "nonce error"
    if (lstrcpyn (szTemp, s, sizeof (ERR)),     // Note: Terminates the string, too
        lstrcmp (szTemp, ERR) EQ 0)
#undef  ERR
    {
        lpwszTemp = ERRMSG_NONCE_ERROR APPEND_NAME;

        // Back off by one so the caret points to the symbol in error
        lppnLocalVars->uNumCur--;
    } else
    // Check for WS FULL
#define ERR     WSFULL
    if (lstrcpyn (szTemp, s, sizeof (ERR)),     // Note: Terminates the string, too
        lstrcmp (szTemp, ERR) EQ 0)
#undef  ERR
        lpwszTemp = ERRMSG_WS_FULL APPEND_NAME;
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
    va_list  vl;
    APLU3264 i1,
             i2,
             i3;
    static  char szTemp[256] = {'\0'};

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
    } // End IF/ELSE
#endif
} // End pn_yyfprintf


//***************************************************************************
//  End of File: pn_parse.y
//***************************************************************************
