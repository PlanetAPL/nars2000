//***************************************************************************
//  NARS2000 -- Conversion To Rational FSA Header
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


// The order of the values of these constants *MUST* match the
//   column order in <fsaActTableCR>.
typedef enum tagCRCOL_INDICES       // FSA column indices for <mpq_set_str>
{
    CRCOL_NEG   = 0     ,           // 00:  Overbar or Minus sign
    CRCOL_DIGIT         ,           // 01:  Digit
    CRCOL_DEC           ,           // 02:  Decimal point
    CRCOL_EXP           ,           // 03:  e or E
    CRCOL_RAT           ,           // 04:  r or /
    CRCOL_EXT           ,           // 05:  x
    CRCOL_INF           ,           // 06:  Infinity
    CRCOL_EON           ,           // 07:  All other chars (End-Of-Number)

    CRCOL_LENGTH        ,           // 08:  # column indices (cols in fsaActTableCR)
                                    //      Because this enum is origin-0, this value is the # valid columns.
} CRCOLINDICES, *LPCRCOLINDICES;

// N.B.:  Whenever changing the above enum (CRCOLINDICES),
//   be sure to make a corresponding change to
//   <fsaActTableCR> in <cr_proc.c>

// The order of the values of these constants *MUST* match the
//   row order in <fsaActTableCR>.
typedef enum tagCRROW_INDICES       // FSA row indices for Format Specification
{
    CRROW_INIT = 0   ,              // 00:  Initial state
    CRROW_NEG        ,              // 01:  Negative sign
    CRROW_INT1       ,              // 02:  Next of integer state
    CRROW_DEC        ,              // 03:  Decimal point of integer state
    CRROW_EXP0       ,              // 04:  Start of exponent state
    CRROW_EXP1       ,              // 05:  Next of exponent state
    CRROW_INF        ,              // 06:  Infinity
    CRROW_LENGTH     ,              // 07:  # FSA terminal states (rows in fsaActTableCR)
                                    //      Because this enum is origin-0, this value is the # valid rows.

    CRROW_EXIT  = -1 ,              // FSA is done
    CRROW_ERROR = -2 ,              // Error encountered
    CRROW_NONCE = -3 ,              // State not specified as yet
} CRROWINDICES, *LPCRROWINDICES;

// N.B.:  Whenever changing the above enum (CRROWINDICES),
//   be sure to make a corresponding change to
//   <fsaActTableCR> in <cr_proc.c>

typedef enum tagCR_RETCODES         // FSA return codes
{
    CR_SUCCESS      = 0 ,           // 00:  Success
    CR_SYNTAX_ERROR     ,           // 01:  Syntax error
    CR_DOMAIN_ERROR     ,           // 02:  Domain error (0/0)
    CR_RESULT_NEG0      ,           // 03:  Result is -0
} CR_RETCODES, *LPCR_RETCODES;

typedef struct tagCRLOCALVARS
{
    CRROWINDICES iNewState;         // 00:  New state (see CRROW_INDICES)
    LPCHAR       lpszIni,           // 04:  Ptr to incoming text
                 lpszCur,           // 08:  Ptr to current char in lpszIni
                 lpszStart;         // 0C:  Ptr to start of current segment
    mpq_t        mpqRes,            // 10:  Result
                 mpqMul,            // 14:  Numerator Multiplier
                 mpqExp;            // 18:  ...       Exponent
    int          base;              // 1C:  Base for exponent
    UBOOL        bRatSep;           // 20:  TRUE iff there's a RAT separator ('r/')
    CR_RETCODES  crRetCode;         // 24:  Return code
                                    // 28:  Length of struct
} CRLOCALVARS, *LPCRLOCALVARS;

typedef UBOOL (*CR_ACTION) (LPCRLOCALVARS);

typedef struct tagCR_ACTSTR
{
    CRROWINDICES iNewState;         // 00:  New state (see CRROW_INDICES)
    CR_ACTION    crAction1,         // 04:  Primary action
                 crAction2;         // 08:  Secondary action
                                    // 0C:  Length
} CRACTSTR, *LPCRACTSTR;


//***************************************************************************
//  End of File: cr_proc.h
//***************************************************************************
