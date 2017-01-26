//***************************************************************************
//  NARS2000 -- Tokenizer Header
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


// Defined constant for ...
#define NO_PREVIOUS_GROUPING_SYMBOL     NEG1U
#define NO_PREVIOUS_NAME                NEG1U


// The order of the values of these constants *MUST* match the
//   column order in fsaActTableTK.

typedef enum tagTKCOL_INDICES       // FSA column indices for Tokenize
{TKCOL_DIGIT = 0    ,               // 00:  Digit
 TKCOL_DOT          ,               // 01:  Decimal number, inner & outer product separator
 TKCOL_ALPHA        ,               // 02:  Alphabetic
 TKCOL_OVERBAR      ,               // 03:  Overbar
 TKCOL_DIRIDENT     ,               // 04:  Alpha or Omega
 TKCOL_Q_QQ         ,               // 05:  Quad
 TKCOL_UNDERBAR     ,               // 06:  Underbar
 TKCOL_INFINITY     ,               // 07:  Infinity
 TKCOL_ASSIGN       ,               // 08:  Assignment symbol
 TKCOL_SEMICOLON    ,               // 09:  Semicolon symbol
 TKCOL_COLON        ,               // 0A:  Colon symbol
 TKCOL_CTRLSTRUC    ,               // 0B:  Control Structure
 TKCOL_PRIM_FN      ,               // 0C:  Primitive monadic or dyadic function
 TKCOL_PRIM_FN0     ,               // 0D:  ...       niladic function
 TKCOL_PRIM_OP1     ,               // 0E:  ...       monadic/ambiguous operator
 TKCOL_PRIM_OP2     ,               // 0F:  ...       dyadic  ...
 TKCOL_JOT          ,               // 10:  Jot symbol
 TKCOL_LEFTPAREN    ,               // 11:  Left paren
 TKCOL_RIGHTPAREN   ,               // 12:  Right ...
 TKCOL_LEFTBRACKET  ,               // 13:  Left bracket
 TKCOL_RIGHTBRACKET ,               // 14:  Right ...
 TKCOL_LEFTBRACE    ,               // 15:  Left brace
 TKCOL_RIGHTBRACE   ,               // 16:  Right ...
 TKCOL_SPACE        ,               // 17:  White space (' ' or '\t')
 TKCOL_QUOTE1       ,               // 18:  Single quote symbol
 TKCOL_QUOTE2       ,               // 19:  Double ...
 TKCOL_DIAMOND      ,               // 1A:  Diamond symbol
 TKCOL_LAMP         ,               // 1B:  Comment symbol
 TKCOL_SYS_NS       ,               // 1C:  System namespace
 TKCOL_DEL          ,               // 1D:  Del
 TKCOL_EOL          ,               // 1E:  End-Of-Line
 TKCOL_UNK          ,               // 1F:  Unknown symbols

 TKCOL_LENGTH       ,               // 20:  # column indices (cols in fsaActTableTK)
                                    //      Because this enum is origin-0, this value is the # valid columns.
} TKCOLINDICES, *LPTKCOLINDICES;

// Whenever you add a new TKCOL_*** entry,
//   be sure to put code into <CharTransTK> in <tokenize.c>
//   to return the newly defined value,
//   and insert a new entry into <GetColName> in <tokenize.c>.


// The order of the values of these constants *MUST* match the
//   row order in fsaActTableTK.
typedef enum tagTKROW_INDICES       // FSA row indices for Tokenize
{TKROW_SOS = 0    ,                 // 00:  Start of stmt
 TKROW_INIT       ,                 // 01:  Initial state
 TKROW_POINTNOT0  ,                 // 02:  Point Notation, Initial State after white space
 TKROW_POINTNOT1  ,                 // 03:  Point Notation, Initial State after first char
 TKROW_ALPHA      ,                 // 04:  Alphabetic char
 TKROW_SYSNAME    ,                 // 05:  System name
 TKROW_QUOTE1A    ,                 // 06:  Start of or within single quoted char or char vector
 TKROW_QUOTE1Z    ,                 // 07:  End of   ...
 TKROW_QUOTE2A    ,                 // 08:  Start of or within double quoted char or char vector
 TKROW_QUOTE2Z    ,                 // 09:  End of   ...
 TKROW_DOTAMBIG   ,                 // 0A:  Ambiguous dot:  either TKROW_POINTNOT or TKROW_INIT w/fnOp2Done
 TKROW_JOTAMBIG   ,                 // 0B:  Ambiguous jot:  either TKROW_INIT w/fnOp2Done or TKROW_OUTAMBIG
 TKROW_OUTAMBIG   ,                 // 0C:  Ambiguous outer product:  either TKROW_INIT w/fnOutDone or TKROW_POINTNOT w/fnOp2Done
 TKROW_SYS_NS     ,                 // 0D:  System namespace
 TKROW_LBR_INIT   ,                 // 0E:  Inside braces
 TKROW_LBR_Q1     ,                 // 0F:  Inside braces, single quotes
 TKROW_LBR_Q2     ,                 // 10:  Inside braces, double quotes
 TKROW_LENGTH     ,                 // 11:  # FSA terminal states (rows in fsaActTableTK)
                                    //      Because this enum is origin-0, this value is the # valid rows.
 TKROW_EXIT  = -1 ,                 // FSA is done
 TKROW_NONCE = -2 ,                 // State not specified as yet

} TKROWINDICES, *LPTKROWINDICES;

typedef struct tagSC_INDICES
{
    UINT        PrevGroup,          // 00:  Index into lpGrpSeqIni of previous grouping symbol
                                    //      (NO_PREVIOUS_GROUPING_SYMBOL = none)
                clrIndex;           // 04:  Matching index into lpMemClrIni
    TOKEN_TYPES TknType;            // 08:  Token type of this grouping symbol
} SCINDICES, *LPSCINDICES;


typedef enum tagSC_NAMETYPE
{
    SC_NAMETYPE_GLBNAME = 0,        // 00:  Name is global
    SC_NAMETYPE_LCLNAME,            // 01:  ...     local to a fcn suspended or being edited
    SC_NAMETYPE_SYSFCN,             // 02:  ...     system function
    SC_NAMETYPE_GLBSYSVAR,          // 03:  ...     global system variable
    SC_NAMETYPE_LCLSYSVAR,          // 04:  ...     local  ...    variable
    SC_NAMETYPE_SYSUNK,             // 05:  ...     ...    unknown
    SC_NAMETYPE_PRIMITIVE,          // 06:  ...     a primitive (Quad/QuoteQuad)
    SC_NAMETYPE_UNK,                // 07:  ...     unknown
                                    // 08:  # entries in this enum
} SCNAMETYPE, *LPSCNAMETYPE;

typedef struct tagCLR_COL
{
    SYNTAXCOLORS syntClr;           // 00:  Foreground and background colors (8 bytes)
    TKCOLINDICES colIndex;          // 08:  Column index (see TKCOL_INDICES)
                                    // 0C:  Length
} CLRCOL, *LPCLRCOL;

typedef struct tagTKLOCALVARS
{
    HGLOBAL        hGlbToken;           // 00:  Global memory handle
    LPTOKEN_HEADER lpHeader;            // 04:  Ptr to tokenized header in global memory
    LPTOKEN        lptkStart,           // 08:  First available entry after the header
                   lptkNext,            // 0C:  Next  ...
                   lptkLastEOS;         // 10:  Ptr to last EOS token
    TKROWINDICES   State[3];            // 14:  Current state (TKROW__xxx) (12 bytes)
    UINT           uChar,               // 20:  ...     index into lpwszLine
                   uCharStart,          // 24:  Initial ...                  (static)
                   uCharIni,            // 28:  ...                          (dynamic)
                   uActLen;             // 2C:  Actual length of lpwszLine (may be shorter than lstrlenW)
    LPWCHAR        lpwszOrig,           // 30:  Ptr to original lpwszLine
                   lpwszCur;            // 34:  ...    current WCHAR in ...
    TOKEN_TYPES    CtrlStrucTknType;    // 38:  Control Structure token type
    UINT           CtrlStrucStrLen;     // 3C:  ...               string length
    ANON_CTRL_STRUC;                    // 40:  Ctrl Struc data (8 bytes)
    LPCLRCOL       lpMemClrIni,         // 48:  Ptr to initial array of Syntax Colors (NULL = not coloring) (static)
                   lpMemClrNxt;         // 4C:  Ptr to next    ...                                          (dynamic)
    LPSCINDICES    lpGrpSeqIni,         // 50:  Ptr to initial syntax coloring grouping sequence (static)
                   lpGrpSeqNxt;         // 54:  Ptr to next    ...                               (dynamic)
    UINT           PrevGroup,           // 58:  Current index in lpGrpSeq of the previous grouping symbol
                                        //      (NO_PREVIOUS_GROUPING_SYMBOL = none)
                   NameInit;            // 5C:  Index in lpMemClrIni of Start of name (including sysnames)
                                        //      (NO_PREVIOUS_NAME = none)
    SCNAMETYPE     scNameType;          // 60:  Type of name starting at NameInit
    HWND           hWndEC;              // 64:  Window handle of Edit Ctrl (parent is SM or FE)
    TKCOLINDICES   colIndex;            // 68:  Current TKCOL_xxx value
    UINT           uSyntClrLen;         // 6C:  # Syntax Color entries
    HGLOBAL        hGlbNum,             // 70:  NumAlp global memory handle
                   hGlbStr;             // 74:  NumAlp global memory handle
    int            iNumLen,             // 78:  # chars in lpszNumAlp
                   iStrLen,             // 7C:  ...        lpwszString
                   iNumLim,             // 80:  Current limit for lpszNumAlp
                   iStrLim,             // 84:  ...               lpwszString
                   lbrCount;            // 88:  Count of accumulated left braces
    UINT           bMFO:1,              // 8C:  00000001:  TRUE iff this is a Magic Function/Operator
                   bAfoArgs:1,          //      00000002:  TRUE iff some stmt references {alpha} or {omega}
                   :30;                 //      FFFFFFFC:  Available bits
    struct tagPERTABDATA
                  *lpMemPTD;            // 90:  Ptr to PerTabData global memory
    LPSF_FCNS      lpSF_Fcns;           // 94:  Ptr to common struc
    DFN_TYPES      AfoDfnType;          // 98:  Top level AFO defined function type
    LPHSHTABSTR    lpHTS;               // 9C:  Ptr to local HshTabStr (may *NOT* be NULL)
                                        // A0:  Length
} TKLOCALVARS, *LPTKLOCALVARS;

typedef UBOOL (*TK_ACTION) (LPTKLOCALVARS);

typedef struct tagTK_ACTSTR
{
    TKROWINDICES iNewState;         // 00:  New state (see TKROW_INDICES)
    TK_ACTION    fnAction1,         // 04:  Primary action
                 fnAction2,         // 08:  Secondary action
                 scAction1,         // 0C:  Primary action
                 scAction2;         // 10:  Secondary action
                                    // 14:  Length
} TKACTSTR;


//***************************************************************************
//  End of File: tokenize.h
//***************************************************************************
