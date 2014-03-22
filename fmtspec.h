//***************************************************************************
//  NARS2000 -- Format Specification FSA Header
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2013 Sudley Place Software

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
//   column order in <fsaActTableFS>.
typedef enum tagFSCOL_INDICES       // FSA column indices for Format Specification
{FSCOL_DIGIT = 0   ,                // 00:  Digit
 FSCOL_A           ,                // 01:  Capital letter A
 FSCOL_B           ,                // 02:  ...            B
 FSCOL_C           ,                // 03:  ...            C
 FSCOL_E           ,                // 04:  ...            E
 FSCOL_F           ,                // 05:  ...            F
 FSCOL_G           ,                // 06:  ...            G
 FSCOL_I           ,                // 07:  ...            I
 FSCOL_K           ,                // 08:  ...            K
 FSCOL_L           ,                // 09:  ...            L
 FSCOL_M           ,                // 0A:  ...            M
 FSCOL_N           ,                // 0B:  ...            N
 FSCOL_O           ,                // 0C:  ...            O
 FSCOL_P           ,                // 0D:  ...            P
 FSCOL_Q           ,                // 0E:  ...            Q
 FSCOL_R           ,                // 0F:  ...            R
 FSCOL_S           ,                // 10:  ...            S
 FSCOL_T           ,                // 11:  ...            T
 FSCOL_X           ,                // 12:  ...            X
 FSCOL_Z           ,                // 13:  ...            Z
 FSCOL_TXTDELIM    ,                // 14:  Text leading delimiter
                                    //      (SingleQuote, DoubleQuote, Quad, Quote-Quad, Dieresis, LeftCaret, LeftShoe)
 FSCOL_COMMA       ,                // 15:  Comma
 FSCOL_LEFTPAREN   ,                // 16:  Left paren
 FSCOL_RIGHTPAREN  ,                // 17:  Right ...
 FSCOL_SPACE       ,                // 18:  White space
 FSCOL_ALPHA       ,                // 19:  All other chars
 FSCOL_EOL         ,                // 1A:  End-Of-Line

 FSCOL_LENGTH      ,                // 1B:  # column indices (cols in fsaActTableFS)
                                    //      Because this enum is origin-0, this value is the # valid columns.
} FSCOLINDICES, *LPFSCOLINDICES;

// N.B.:  Whenever changing the above enum (FSCOLINDICES),
//   be sure to make a corresponding change to
//   <fsaActTableFS> and <CharTransFS> in <fmtspec.c>

// The available letters are DHJUVWY.

// The order of the values of these constants *MUST* match the
//   row order in <fsaActTableFS>.
typedef enum tagFSROW_INDICES       // FSA row indices for Format Specification
{
 FSROW_INIT = 0   ,                 // 00:  Initial state
 FSROW_SPEC       ,                 // 01:  Within a format specification
 FSROW_CHKEOS     ,                 // 02:  Check for EOS (comma or WC_EOS)
 FSROW_LENGTH     ,                 // 03:  # FSA terminal states (rows in fsaActTableFS)
                                    //      Because this enum is origin-0, this value is the # valid rows.
 FSROW_EXIT  = -1 ,                 // FSA is done
 FSROW_ERROR = -2 ,                 // Error encountered
 FSROW_NONCE = -3 ,                 // State not specified as yet
} FSROWINDICES, *LPFSROWINDICES;


typedef struct tagFSLOCALVARS
{
    FSROWINDICES State;             // 00:  Current state (FSROW_xxx)
    LPWCHAR      lpwszCur;          // 04:  ...    current WCHAR in ...
    struct tagFMT_SPEC_STR
                *lpfsCur,           // 08:  Ptr to current FMTSPECSTR
                *lpfsNestCur;       // 0C:  ...            nesting level (NULL = none)
    UINT         uFsNxt,            // 10:  Offset from lpfsCur to next available byte
                 uChar,             // 14:  Current index into lpwszLine
                 nestLvl,           // 18:  Current nesting level (0 = none)
                 uErrIndex;         // 1C:  Offset from start of fmt str to error
    LPTOKEN      lptkFunc;          // 20:  Ptr to function token
                                    // 24:  Length
} FSLOCALVARS, *LPFSLOCALVARS;


typedef UBOOL (*FS_ACTION) (LPFSLOCALVARS);

typedef struct tagFS_ACTSTR
{
    FSROWINDICES iNewState;         // 00:  New state (see FSROW_INDICES)
    FS_ACTION    fnAction;          // 04:  Action
                                    // 08:  Length
} FSACTSTR;


// Format specification values
typedef enum tagFMT_SPEC_VAL
{
    FMTSPECVAL_A = 0,               // 00:  A-format spec
    FMTSPECVAL_E,                   // 01:  E-...
    FMTSPECVAL_F,                   // 02:  F-...
    FMTSPECVAL_G,                   // 03:  G-...
    FMTSPECVAL_I,                   // 04:  I-...
    FMTSPECVAL_R,                   // 05:  R-...
    FMTSPECVAL_T,                   // 06:  T-...
    FMTSPECVAL_X,                   // 07:  X-...
    FMTSPECVAL_TXT,                 // 08:  Text-...
    FMTSPECVAL_ERR,                 // 09:  Error
} FMTSPECVAL, *LPFMTSPECVAL;


// Format specification struc
typedef struct tagFMT_SPEC_STR
{
                                    //      Modifier present flags
    UINT bB:1,                      // 00:  00000001:  TRUE iff B (<= 1 per fmt spec)
         bC:1,                      //      00000002:  ...      C (<= 1 per fmt spec)
         bK:1,                      //      00000004:  ...      K (<= 1 per fmt spec)
         bL:1,                      //      00000008:  ...      L (<= 1 per fmt spec)
         bM:1,                      //      00000010:  ...      M (<= 1 per fmt spec)
         bN:1,                      //      00000020:  ...      N (<= 1 per fmt spec)
         bO:1,                      //      00000040:  ...      O (>= 0 per fmt spec)
         bP:1,                      //      00000080:  ...      P (<= 1 per fmt spec)
         bQ:1,                      //      00000100:  ...      Q (<= 1 per fmt spec)
         bR:1,                      //      00000200:  ...      R (<= 1 per fmt spec)
         bS:1,                      //      00000400:  ...      S (<= 1 per fmt spec)
         bZ:1,                      //      00000800:  ...      Z (<= 1 per fmt spec)
         bRep:1,                    //      00001000:  ...      Repetition factor present
         bInuse:1,                  //      00002000:  ...      Entry is inuse
         :18;                       //      FFFFC000:  Available bits
    UINT fsRep,                     // 04:  Repetition factor for this fmt spec (valid iff bRep EQ TRUE)
         fsDig,                     // 08:  Precision for F- and R-format, significant digits for E-format
         nestRep,                   // 0C:  Nesting repetition factor
         fmtSpecVal;                // 10:  Format specification value:  FMTSPECVAL_xx (A E F G I R T X TXT)
    int  fsWid,                     // 14:  Format width or (signed) count for T/X
         fsScl;                     // 18:  Signed scale factor (valid iff bK EQ TRUE)
                                    //      OffsetS are all measured from the start of the individual fmtspec
    UINT offGtxt,                   // 1C:  to the G-pattern
         offText,                   // 20:  ...    text for Text Insertion (FMTSPEC_TXT)
         offMtxt,                   // 24:  ...    M-modifier text  (valid iff bM EQ TRUE)
         offNtxt,                   // 28:  ...    N-modifier text  (valid iff bN EQ TRUE)
         offOchain,                 // 2C:  ...    O-modifier chain (valid iff bO EQ TRUE)
         offPtxt,                   // 30:  ...    P-modifier text  (valid iff bP EQ TRUE)
         offQtxt,                   // 34:  ...    Q-modifier text  (valid iff bQ EQ TRUE)
         offRtxt,                   // 38:  ...    R-modifier text  (valid iff bR EQ TRUE)
         offStxt,                   // 3C:  ...    S-modifier text  (valid iff bS EQ TRUE)
         offNext;                   // 40:  Offset to next FMTSPEC_STR (0 = none)
    struct tagFMT_SPEC_STR
        *lpfsNestPrv;               // 44:  Ptr to previous left paren (NULL = none)
                                    // 48:  Length
} FMTSPECSTR, *LPFMTSPECSTR;


typedef struct tagO_CHAIN           // Struct for an entry in the O-modifier chain
{
    UINT     offPrvChain;           // 00:  Offset from base of this FMTSPEC_STR to the previous O-chain (0 = none)
    APLINT   aplIntVal;             // 04:  The integer representation of the value to check
    APLFLOAT aplFltVal;             // 0C:  The float   ...
                                    // 14:  Length
                                    // 14:  Start of the substitution text
} OCHAIN, *LPOCHAIN;


// Standard symbols (1st char in each S-qualifier pair)
// These chars must be in the same order as the folowing enum
#define STDSYMSUB           L"*.,0" DEF_UNDERFLOW_WS L"Z9" DEF_EXPONENT_UC_WS DEF_RATSEP_WS

typedef enum tagSYM_SUB             // Symbol Substitution enum
{
    SYMSUB_OVERFLOW_FILL = 0,       // 00:  * (overflow fill)
    SYMSUB_DECIMAL_SEP,             // 01:  . (decimal point)
    SYMSUB_THOUSANDS_SEP,           // 02:  , (thousands separator)
    SYMSUB_ZERO_FILL,               // 03:  0 (fill for Z qualifier)
    SYMSUB_PRECISION_LOSS,          // 04:  _ (loss of precision)
    SYMSUB_Z_CHAR,                  // 05:  Z (G-format pattern char)
    SYMSUB_9_CHAR,                  // 06:  9 (G-format pattern char)
    SYMSUB_EXPONENT_SEP,            // 07:  E (Exponent separator)
    SYMSUB_RATIONAL_SEP,            // 08:  r (Rational number separator)
    SYMSUB_LENGTH,                  // 09:  # entries in this enum
} SYMSUB, *LPSYMSUB;


typedef struct tagSURR_TEXT         // Surrounding text info
{
    UINT    uHead,                  // 00:  Length of leading text
            uTail;                  // 04:  ...       trailing ...
    LPWCHAR lpHead,                 // 08:  Ptr to leading text
            lpTail;                 // 0C:  ...    trailing ...
                                    // 10:  Length
} SURRTEXT, *LPSURRTEXT;


//***************************************************************************
//  End of File: fmtspec.h
//***************************************************************************
