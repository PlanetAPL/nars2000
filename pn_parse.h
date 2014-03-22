//***************************************************************************
//  NARS2000 -- Point Notation Parser Header
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

typedef enum tagPN_NUMTYPE
{
    PN_NUMTYPE_BOOL,                // 00:  Boolean type
    PN_NUMTYPE_INT,                 // 01:  Integer ...
    PN_NUMTYPE_FLT,                 // 02:  Float   ...
    PN_NUMTYPE_HC2,                 // 03:  Complex ...
    PN_NUMTYPE_HC4,                 // 04:  Quaternion ...
    PN_NUMTYPE_HC8,                 // 05:  Octonion ...
    PN_NUMTYPE_RAT,                 // 06:  RAT ...
    PN_NUMTYPE_VFP,                 // 07:  VFP ...
    PN_NUMTYPE_INIT,                // 08:  Initial value (used in <aNumTypePromote>)
    PN_NUMTYPE_LENGTH               // 09:  Length
} PNNUMTYPE, *LPPNNUMTYPE;

#define IsPnNumTypeFlt(a)           \
    ((a) EQ PN_NUMTYPE_FLT          \
/*|| (a) EQ PN_NUMTYPE_HC2F */      \
/*|| (a) EQ PN_NUMTYPE_HC4F */      \
/*|| (a) EQ PN_NUMTYPE_HC8F */      \
    )

#define IsPnNumTypeRat(a)           \
    ((a) EQ PN_NUMTYPE_RAT          \
/*|| (a) EQ PN_NUMTYPE_HC2R */      \
/*|| (a) EQ PN_NUMTYPE_HC4R */      \
/*|| (a) EQ PN_NUMTYPE_HC8R */      \
    )

typedef struct tagPN_YYSTYPE        // YYSTYPE for Point Notation parser
{
    ALLTYPES  at;                   // 00:  All datatypes as a union (64 bytes)
    UINT      uNumAcc;              // 40:  Starting offset into lpszNumAccum
    PNNUMTYPE chType;               // 44:  The numeric type (see PNNUMTYPE)
    UCHAR     chCur,                // 48:  A char for the accumulator
              chAlign[3];           // 49:  For alignment
                                    // 4C:  Length
} PN_YYSTYPE, *LPPN_YYSTYPE;        // Data type of yacc stack

#define YYSTYPE_IS_DECLARED 1


typedef struct tagPN_VECTOR
{
    ALLTYPES  at;                   // 00:  All datatypes as a union (64 bytes)
    PNNUMTYPE chType;               // 44:  The numeric type (see PNNUMTYPE)
    LPCHAR    lpStart;              // 48:  Ptr to start of char stream
    UINT      uNumLen;              // 4C:  # chars in lpStart
    UINT      bInteger:1;           // 50:  000000001:  TRUE iff the number is integral (no decimal point, no 'e' 'E')
                                    // 54:  Length
} PN_VECTOR, *LPPN_VECTOR;

#define PNVECTOR_INIT       10      // Initial allocation count of PN_VECTOR
#define PNVECTOR_INCR        5      // Incremental ...


typedef struct tagPNLOCALVARS       // Point Notation Local Vars
{
    LPCHAR        lpszStart;        // 00:  Ptr to start of char stream
    UINT          uNumLen,          // 04:  # chars in lpszStart
                  uNumCur,          // 08:  Current index into lpszStart
                  uNumIni,          // 0C:  Initial index into lpszStart
                  uAlpAcc,          // 10:  Current index into lpszAlphaInt
                  uNumAcc,          // 14:  Current index into lpszNumAccum
                  uCharIndex;       // 18:  Starting character index
    LPCHAR        lpszAlphaInt,     // 1C:  Ptr to AlphaInt accumulator
                  lpszNumAccum;     // 20:  Ptr to numeric accumulator (in case of overflow)
    ALLTYPES      at;               // 24:  All datatypes as a union (64 bytes)
    PNNUMTYPE     chType,           // 64:  The numeric type (see PNNUMTYPE)
                  chComType;        // 68:  The common numeric type ...
    UBOOL         bYYERROR;         // 6C:  TRUE iff there's been a YYERROR
    HGLOBAL       hGlbRes,          // 70:  Result global memory handle
                  hGlbVector;       // 74:  PN_VECTOR global memory handle (NULL = none)
    UINT          uGlbVectorMaxLen, // 78:  Maximum length of memory in hGlbVector (units = PN_VECTOR)
                  uGlbVectorCurLen; // 7C:  Current ...
    LPPN_YYSTYPE  lpYYRes;          // 80:  Temp ptr
    struct tagTKLOCALVARS *
                  lptkLocalVars;    // 84:  Ptr to Tokenize_EM local vars
                                    // 88:  Length
} PNLOCALVARS, *LPPNLOCALVARS;


//***************************************************************************
//  End of File: pn_parse.h
//***************************************************************************
