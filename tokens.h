//***************************************************************************
//  NARS2000 -- Tokens
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

/*

Tokens are containers  for all sorts of objects such as operators,
functions, variables, grouping symbols, lists, list separators, etc.

Most tokens are simple in that they have type and data only.e,g,
TKT_LEFTPAREN and '('.  Some tokens are complex in that their data
points to other data.

The following tokens are complex:

TKT_VARNAMED This container's tkData is an LPSYMENTRY which names
            a variable.  The LPSYMENTRY may be immediate or not.
            If the latter, stData contains an HGLOBAL which points
            to a simple or nested or hetero array.

TKT_VARIMMED This container's tkData is one of Boolean, Integer,
            Floating Point, or Character as distinguished by
            the value in tkFlags.ImmType.

TKT_LSTIMMED This container's tkData is a single element which is
            an immediate token.

TKT_LSTARRAY This container's tkData is a single element which is
            a single element array.

TKT_LSTMULT This container's tkData is multiple TOKENs, one per
            each separate list element.

TKT_VARARRAY The main workhorse.  Its tkData is an HGLOBAL which
            contains an array which is either simple, nested, or
            hetero.

TKT_STRAND  Valid only during strand processing after which it
            is converted to TKT_VARARRAY.

TKT_STRING  Valid only during strand processing after which it
            is converted to TKT_VARARRAY.

Outside strand processing, only the tokens TKT_VARNAMED, TKT_VARIMMED,
TKT_LIST, and TKT_VARARRAY are present in a token which points to a
value.

 */

// Data Token Types
typedef enum tagTOKEN_TYPES
{TKT_UNUSED        ,        // 00: Unused token type -- an error if encountered
 TKT_VARNAMED      ,        // 01: Symbol table entry for a named var      (data is LPSYMENTRY)
 TKT_CHRSTRAND     ,        // 02: Character strand                        (data is HGLOBAL)
 TKT_NUMSTRAND     ,        // 03: Numeric   ...                           (data is HGLOBAL)
 TKT_NUMSCALAR     ,        // 04: Numeric   scalar                        (data is HGLOBAL)
 TKT_VARIMMED      ,        // 05: An immediate constant (Boolean, Integer, Character, or Floating point,
                            //     distinguished by the value in tkFlags.ImmType)
 TKT_ASSIGN        ,        // 06: Assignment symbol                       (data is UTF16_LEFTARROW)
 TKT_LISTSEP       ,        // 07: List separator                          (data is ';')
 TKT_LABELSEP      ,        // 08: Label separator                         (data is ':')
 TKT_COLON         ,        // 09: Colon                                   (data is ':')
 TKT_FCNIMMED      ,        // 0A: An immediate primitive function         (data is UTF16_***)
 TKT_OP1IMMED      ,        // 0B: An immediate monadic primitive operator (data is UTF16_***)
 TKT_OP2IMMED      ,        // 0C: ...          dyadic  ...
 TKT_OP3IMMED      ,        // 0D: ...          ambiguous  ...
 TKT_OPJOTDOT      ,        // 0E: Outer product monadic operator (with right scope) (data is NULL)
 TKT_LEFTPAREN     ,        // 0F: Left paren                              (data is TKT_LEFTPAREN)
 TKT_RIGHTPAREN    ,        // 10: Right ...                                ...         RIGHTPAREN
 TKT_LEFTBRACKET   ,        // 11: Left bracket                             ...        LEFTBRACKET
 TKT_RIGHTBRACKET  ,        // 12: Right ...                                ...         RIGHTBRACKET
 TKT_LEFTBRACE     ,        // 13: Left brace                               ...         LEFTBRACE
 TKT_RIGHTBRACE    ,        // 14: Right ...                                ...         RIGHTBRACE
 TKT_EOS           ,        // 15: End-of-Stmt (data is length of stmt including this token)
 TKT_EOL           ,        // 16: End-of-Line  ...
 TKT_SOS           ,        // 17: Start-of-Stmt                           (data is NULL)
 TKT_LINECONT      ,        // 18: Line continuation                       (data is NULL)
 TKT_INPOUT        ,        // 19: Input/Output                            (data is UTF16_QUAD or UTf16_QUAD2 or UTF16_QUOTEQUAD symbol)
 TKT_VARARRAY      ,        // 1A: Array of data                           (data is HGLOBAL)
 TKT_CS_ANDIF      ,        // 1B: Control Structure:  ANDIF               (data is Line/Stmt/Token #)
 TKT_CS_ASSERT     ,        // 1C: ...                 ASSERT     ...
 TKT_CS_CASE       ,        // 1D: ...                 CASE       ...
 TKT_CS_CASELIST   ,        // 1E: ...                 CASELIST   ...
 TKT_CS_CONTINUE   ,        // 1F: ...                 CONTINUE   ...
 TKT_CS_CONTINUEIF ,        // 20: ...                 CONTINUEIF ...
 TKT_CS_ELSE       ,        // 21: ...                 ELSE       ...
 TKT_CS_ELSEIF     ,        // 22: ...                 ELSEIF     ...
 TKT_CS_END        ,        // 23: ...                 END        ...
 TKT_CS_ENDFOR     ,        // 24: ...                 ENDFOR     ...
 TKT_CS_ENDFORLCL  ,        // 25: ...                 ENDFORLCL  ...
 TKT_CS_ENDIF      ,        // 26: ...                 ENDIF      ...
 TKT_CS_ENDREPEAT  ,        // 27: ...                 ENDREPEAT  ...
 TKT_CS_ENDSELECT  ,        // 28: ...                 ENDSELECT  ...
 TKT_CS_ENDWHILE   ,        // 29: ...                 ENDWHILE   ...
 TKT_CS_FOR        ,        // 2A: ...                 FOR        ...
 TKT_CS_FOR2       ,        // 2B: ...                 FOR2       ...
 TKT_CS_FORLCL     ,        // 2C: ...                 FORLCL     ...
 TKT_CS_GOTO       ,        // 2D: ...                 GOTO       ...
 TKT_CS_IF         ,        // 2E: ...                 IF         ...
 TKT_CS_IF2        ,        // 2F: ...                 IF2        ...
 TKT_CS_IN         ,        // 30: ...                 IN         ...
 TKT_CS_LEAVE      ,        // 31: ...                 LEAVE      ...
 TKT_CS_LEAVEIF    ,        // 32: ...                 LEAVEIF    ...
 TKT_CS_ORIF       ,        // 33: ...                 ORIF       ...
 TKT_CS_REPEAT     ,        // 34: ...                 REPEAT     ...
 TKT_CS_REPEAT2    ,        // 35: ...                 REPEAT2    ...
 TKT_CS_RETURN     ,        // 36: ...                 RETURN     ...
 TKT_CS_SELECT     ,        // 37: ...                 SELECT     ...
 TKT_CS_SELECT2    ,        // 38: ...                 SELECT2    ...
 TKT_CS_UNTIL      ,        // 39: ...                 UNTIL      ...
 TKT_CS_WHILE      ,        // 3A: ...                 WHILE      ...
 TKT_CS_WHILE2     ,        // 3B: ...                 WHILE2     ...
 TKT_CS_SKIPCASE   ,        // 3C: ...                 Special token
 TKT_CS_SKIPEND    ,        // 3D: ...                 Special token
 TKT_SYS_NS        ,        // 3E: System namespace                        (namespace level, 0 = none, 1 = top)
 TKT_SYNTERR       ,        // 3F: Syntax Error                            (data is symbol)
 TKT_SETALPHA      ,        // 40: Assignment into {alpha} only if No Value
 TKT_DEL           ,        // 41: Del -- always a function
 TKT_DELDEL        ,        // 42: Del Del -- either a monadic or dyadic operator
 TKT_DELAFO        ,        // 43: Anonymous monadic/dyadic operator       (data is ptr to function header)
 TKT_FCNAFO        ,        // 44: Anonymous function                      (data is ptr to function header)
 TKT_OP1AFO        ,        // 45: ...       monadic operator              (data is ptr to function header)
 TKT_OP2AFO        ,        // 46: ...       dyadic  ...                   (data is ptr to function header)
 TKT_GLBDFN        ,        // 47: Placeholder for hGlbDfnHdr              (data is ptr to function header)
 TKT_NOP           ,        // 48: NOP used in AFOs
 TKT_AFOGUARD      ,        // 49: AFO guard
 TKT_AFORETURN     ,        // 4A: AFO return

 // These token types are not generated by <Tokenize_EM>
 //   hence, they never appear in a saved token stream,
 //   only as a parse token created during <ParseLine>.
 TKT_STRAND        ,        // 4B: Strand accumulating                     (data is LPTOKEN)
 TKT_LISTINT       ,        // 4C: List intermediate                       (data is HGLOBAL)
 TKT_LISTPAR       ,        // 4D: List in parens                          (data is HGLOBAL)
 TKT_LSTIMMED      ,        // 4E: List in brackets, single element, immed (data is immediate)
 TKT_LSTARRAY      ,        // 4F: List in brackets, single element, array (data is HGLOBAL)
 TKT_LSTMULT       ,        // 50: List in brackets, multiple elements     (data is HGLOBAL)
 TKT_FCNARRAY      ,        // 51: Array of functions and/or operators     (data is HGLOBAL)
 TKT_FCNNAMED      ,        // 52: A named function                        (data is LPSYMENTRY)
 TKT_AXISIMMED     ,        // 53: An immediate axis specification         (data is immediate)
 TKT_AXISARRAY     ,        // 54: An array of  ...                        (data is HGLOBAL)
 TKT_OP1NAMED      ,        // 55: A named monadic primitive operator      (data is LPSYMENTRY)
 TKT_OP2NAMED      ,        // 56: ...     dyadic  ...
 TKT_OP3NAMED      ,        // 57: ...     ambiguous ...
 TKT_STRNAMED      ,        // 58: A named strand
 TKT_CS_NEC        ,        // 59: Control structure:  Special token (cs_yyparse only)
 TKT_CS_EOL        ,        // 5A: Control structure:  Special token (cs_yyparse only)
 TKT_CS_ENS        ,        // 5B: Control structure:  Special token (cs_yyparse only)
 TKT_FILLJOT       ,        // 5C: Fill jot
                            // 5D-7F:  Available entries (7 bits)
} TOKEN_TYPES;

// N.B.:  Whenever changing the above enum (TOKEN_TYPES),
//   be sure to make a corresponding change to
//   <Untokenize> and <AppendNewToken_EM> in <tokenize.c>,
//   <pl_yylex> in <pl_parse.y>,
//   <MakeVarStrand_EM_YY> and <CopyToken_EM> in <strand.c>,
//   <GetTokenTypeName> in <dispdbg.c>,
//   <GetNameType> in <assign.c>,
//   <tokenSo> in <parseline.c>,
//   and <TokenTypeFV>, <IsTknNamed>, <IsTknNamedVar>, and <IsTknImmed> in <primfns.c>.


typedef struct tagTKFLAGS
{
    UINT TknType:7,         // 0000007F:  Data token type (see TOKEN_TYPES)
         bSetAlpha:1,       // 00000080:  TRUE iff this stmt sets {alpha}
         ImmType:4,         // 00000F00:  Type of immediate data (see IMM_TYPES) (if .TknType is TKT_xxxIMMED)
         NoDisplay:1,       // 00001000:  Do not display this token
         SysNSLvl:3,        // 0000E000:  System namespace level (0 = none, 1 = top)
         bSyntErr:1,        // 00010000:  TRUE iff this stmt is a SYNTAX ERROR
         bAfoArgs:1,        // 00020000:  TRUE iff this stmt references {alpha} or {omega}
         bGuardStmt:1,      // 00040000:  TRUE iff this stmt is an AFO Guard
         bAssignName:1,     // 00080000:  TRUE iff this token is a name that is the target of an assignment
         bTempAPV:1,        // 00100000:  TRUE iff this is a temporary APV used in place of an elided index
         :11;               // FFE00000:  Available bits
} TKFLAGS, *LPTKFLAGS;

// N.B.:  Whenever changing the above enum (TKFLAGS),
//   be sure to make a corresponding change to
//   the values assigned to <tkZero> and <tkBlank> in <externs.h>, and
//   <plYYEOS>/<plYYSOS> in <parseline.c>.


typedef struct tagLOCATION
{
    USHORT uLineNum,            // 00:  Line #      (origin-1)
           uStmtNum,            // 02:  Statement # (origin-0)
           uTknNum;             // 04:  Token #     (origin-0)
                                // 06:  Length
} LOCATION, *LPLOCATION;

typedef const LOCATION CLOCATION, *LPCLOCATION;

typedef union tagULOCATION
{
    const LOCATION c;           // 00:  Constant LOCATION
          LOCATION d;           // 00:  Dynamic  ...
                                // 06:  Length
} ULOCATION, *LPULOCATION;

#define ANON_CTRL_STRUC             \
  struct {ULOCATION Orig;           \
           LOCATION Next;           \
           USHORT   uCLIndex;       \
           USHORT   bSOS:1,         \
                    :15;            \
         }

typedef union tagTOKEN_DATA
{
    struct tagSYMENTRY
              *tkSym;               // 00:  Data is an LPSYMENTRY
    HGLOBAL    tkGlbData;           // 00:  ...     an HGLOBAL
    struct {
      UINT         tkIndex;         // 00:  ...     an index
      DFN_TYPES    tkDfnType;       // 04:  ...     a DFNTYPE_xxx
           };
    APLBOOL    tkBoolean;           // 00:  ...     an APLBOOL
    APLINT     tkInteger;           // 00:  ...     an APLINT
    APLAPA     tkAPA;               // 00:  ...     an APLAPA
    ANON_CTRL_STRUC;                // 00:  ...     Ctrl Struc data (8 bytes)
    struct tagTOKEN
              *lptkCSLink;          // 00:  ...     Ptr to previous token at start of stmt
                                    //              (found in TKT_CS_xxx2 only)
    APLFLOAT   tkFloat;             // 00:  ...     a floating point number
    APLCHAR    tkChar;              // 00:  ...     an APLCHAR
    LPVOID     tkVoid;              // 00:  ...     an abritrary ptr
    APLLONGEST tkLongest;           // 00:  Longest datatype (8 bytes) (so we can copy the entire data)
    ANON_CTRL_STRUC tkCtrlStruc;    // 00:  Longest Ctrl Struc (16 bytes)
                                    // 10:  Length
} TOKEN_DATA, *LPTOKEN_DATA;

// N.B.:  Whenever changing the above enum (TOKEN_DATA),
//   be sure to make a corresponding change to
//   <plYYEOS>/<plYYSOS> in <parseline.c>.


typedef struct tagTOKEN
{
    TKFLAGS          tkFlags;       // 00:  The flags part
    SO_ENUM          tkSynObj;      // 04:  The Syntax Object for this token
    TOKEN_DATA       tkData;        // 08:  The data part (16 bytes)
    int              tkCharIndex;   // 18:  Index into the input line of this token
                                    // 1C:  Length
} TOKEN, *LPTOKEN;

// N.B.:  Whenever changing the above struct (TOKEN),
//   be sure to make a corresponding change to
//   <tkZero> and <tkBlank> in <externs.h>,
//   <plYYEOS>/<plYYSOS> in <parseline.c>,
//   <tkMinMaxAfo> in <po_dot.c>.

#define TOKEN_HEADER_SIGNATURE      'NKOT'

typedef struct tagTOKEN_HEADER
{
    HEADER_SIGNATURE Sig;           // 00:  Token header signature
    UINT             Version,       // 04:  Version # of this header
                     TokenCnt,      // 08:  # tokens in lpToken
                     PrevGroup;     // 0C:  Index of the previous (to the left) grouping symbol
                                    //      (L/R paren, L/R bracket) where the index is relative
                                    //      to the first token after this header.
                                    // 10:  Length
                                    // 10:  Followed by a <TokenCnt> array of TOKENs
} TOKEN_HEADER, *LPTOKEN_HEADER;


//***************************************************************************
//  End of File: tokens.h
//***************************************************************************
