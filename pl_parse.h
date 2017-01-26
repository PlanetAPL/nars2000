//***************************************************************************
//  NARS2000 -- ParseLine Header
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

typedef struct tagPL_YYSTYPE        // YYSTYPE for ParseLine
{
    // N.B.:  The first item in this struct *MUST* be the TOKEN
    //   because when we pass it as an argument to an LPPRIMFNS
    //   we might need it to be an LPPL_YYSTYPE (for a function strand)
    //   or an LPTOKEN (single function).
    TOKEN   tkToken;                // 00:  Token info (28 bytes)
    UINT    TknCount;               // 1C:  Count of adjacent tokens including this one
    UINT    YYInuse:1,              // 20:  00000001:  This entry is in use
            YYIndirect:1,           //      00000002:  Arg is indirect
            YYPerm:1,               //      00000004:  Item is permanent, don't free
            YYStranding:1,          //      00000008:  Item is in the process of being stranded
#ifdef DEBUG
            :3,                     //      00000070:  Available bits
            YYIndex:25;             //      FFFFFF80:  Index #
#else
            :28;                    //      FFFFFFF0:  Available bits
#endif
    struct tagPL_YYSTYPE
           *lpYYFcnBase,            // 24:  Ptr to base function/operator
                                    //      Not valid outside the enclosing
                                    //        invocation of ParseLine
           *lpYYStrandBase,         // 28:  Ptr to this token's strand base
           *lpplYYArgCurry,         // 2C:  Ptr to left argument curry
           *lpplYYIdxCurry,         // 30:  Ptr to axis curry
           *lpplYYFcnCurry,         // 34:  ...         function ...
           *lpplYYOpRCurry;         // 38:  ...    right operand ...
    LPTOKEN lptkLftBrace,           // 3C:  Ptr to the AFO's left brace token
            lptkRhtBrace;           // 40:  ...              right ...
#ifdef DEBUG
    UINT    SILevel;                // 44:  SI Level (needed for YYResIsEmpty)
    LPCHAR  lpFileName;             // 48:  Ptr to filename where allocated
    UINT    uLineNum;               // 4C:  Line # where allocated
                                    // 50:  Length
#else
                                    // 44:  Length
#endif
} PL_YYSTYPE, *LPPL_YYSTYPE;        // Data type of yacc stack

// N.B.:  Whenever changing the above struct (PL_YYSTYPE),
//   be sure to make a corresponding change to
//   <plYYEOS> in <parseline.c>.

#define YYSTYPE_IS_DECLARED 1

// Define indices into lpYYStrandStart/Base/Nest[STRAND_LEN];
typedef enum tagSTRAND_INDS
{
    STRAND_VAR = 0,                 // 00:  Index for var strands
    STRAND_FCN,                     // 01:  Index for function strands
    STRAND_LST,                     // 02:  Index for list strands
    STRAND_NAM,                     // 03:  Index for name strands
    STRAND_LEN                      // 04:  # elements in this enum
} STRAND_INDS;

typedef struct tagPLLOCALVARS       // ParseLine Local Vars
{
    LPTOKEN_HEADER lpMemTknHdr;         // 00:  Ptr to tokenized line header global memory
    HGLOBAL        hGlbTxtLine;         // 04:  Text of line global memory handle
    UINT           Filler1;             // 08:  (Filler)
    LPTOKEN        lptkStart,           // 0C:  Ptr to first available entry after the header
                   lptkNext,            // 10:  Ptr to next  ...
                   lptkEnd,             // 14:  Ptr to end of token stream
                   lptkEOS;             // 18:  ...    EOS ...
    LPAPLCHAR      lpwszLine;           // 1C:  Ptr to line text (zero-terminated)
    UINT           tkErrorCharIndex;    // 20:  Error char index
    UINT           plNameType:4,        // 24:  0000000F:  Object name type (see NAME_TYPES)
                   ExitType:4,          //      000000F0:  Exit Type (see EXIT_TYPES)
                   bRet:1,              //      00000100   Various function results
                   bStopExec:1,         //      00000200   TRUE iff we're to stop executing this line
                   bRestart:1,          //      00000400   TRUE iff we're to restart from a Control Structure
                   bExec1Stmt:1,        //      00000800   TRUE iff we're to execute one stmt only
                   bReturn:1,           //      00001000   TRUE iff we're faking a :return as {goto} 0
                   bYYERROR:1,          //      00002000   TRUE iff there's been a YYERROR
                   bSelSpec:1,          //      00004000   TRUE iff we're inside Selective Specification
                   bIniSpec:1,          //      00008000   TRUE iff we have yet to see the first name inside Selective Specification
                   bAFO:1,              //      00010000   TRUE iff we're parsing an AFO
                   bAfoCtrlStruc:1,     //      00020000:  TRUE iff Ctrl Strucs in AFO
                   bMFO:1,              //      00040000:  TRUE iff this is an MFO
                   bTraceLine:1,        //      00080000:  TRUE iff we're tracing this line
                   bTraceFree:1,        //      00100000:  TRUE iff we're to free the var after tracing
                   bAfoValue:1,         //      00200000:  TRUE iff an AFO returned a value
                   :10;                 //      FFC00000:  Available bits
    UBOOL          bCtrlBreak;          // 28:  TRUE iff Ctrl-Break pressed
    LPPL_YYSTYPE   lpYYStrArrStart[STRAND_LEN],   // 2C:  Strand stack start (static) (16 bytes)
                   lpYYStrArrBase [STRAND_LEN],   // 3C:  ...          base (dynamic) ...
                   lpYYStrArrNext [STRAND_LEN];   // 4C:  ...          next token (dynamic)
    HWND           hWndSM;              // 5C:  Window handle to Session Manager
    struct tagPLLOCALVARS
                  *lpPLPrev;            // 60:  Ptr to previous PLLOCALVARS struct
                                        //      in thread creation order (NULL = none)
    UINT           uLineNum,            // 64:  Function line # (1 for execute or immexec)
                   uStmtNum,            // 68:  Function line stmt # (origin-1 if after PARSELINE_DONE)
                   uTokenCnt;           // 6C:  # tokens in the function line
    HGLOBAL        hGlbDfnHdr;          // 70:  User-defined function/operator global memory handle (NULL = execute/immexec)
    TOKEN          tkSelSpec;           // 74:  TOKEN for Selective Specification (28 bytes)
    struct tagPERTABDATA *lpMemPTD;     // 90:  Ptr to PerTabData global memory
                                        // 94:  Length
} PLLOCALVARS, *LPPLLOCALVARS;


//***************************************************************************
//  End of File: pl_parse.h
//***************************************************************************
