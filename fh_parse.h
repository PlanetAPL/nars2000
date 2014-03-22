//***************************************************************************
//  NARS2000 -- User-Defined Function/Operator Header
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

typedef struct tagINTMONINFO            // Internal function line monitoring info
{
    APLINT  IncSubFns,                  // 00:  Total time including subfunctions (8 bytes)
            ExcSubFns;                  // 08:  ...        excluding ...          (8 bytes)
    UINT    Count:30,                   // 10:  3FFFFFFF:  # times executed
            IncActive:1,                //      40000000:  IncSubFns active
            ExcActive:1;                //      80000000:  ExcSubFns active
                                        // 14:  Length
} INTMONINFO, *LPINTMONINFO;

typedef struct tagEXTMONINFO_INT        // External function line monitoring info as integers
{
    APLINT   IncSubFns,                 // 00:  Total time including subfunctions (8 bytes)
             ExcSubFns,                 // 08:  ...        excluding ...          (8 bytes)
             Count;                     // 10:  # times executed                  (8 bytes)
                                        // 18:  Length
} EXTMONINFO_INT, *LPEXTMONINFO_INT;

typedef struct tagEXTMONINFO_FLT        // External function line monitoring info as floats
{
    APLFLOAT IncSubFns,                 // 00:  Total time including subfunctions (8 bytes)
             ExcSubFns,                 // 08:  ...        excluding ...          (8 bytes)
             Count;                     // 10:  # times executed                  (8 bytes)
                                        // 18:  Length
} EXTMONINFO_FLT, *LPEXTMONINFO_FLT;

typedef struct tagFCNLINE               // Function line structure, one per function line
{
    HGLOBAL hGlbTxtLine;                // 00:  Text of the line (APLCHAR) global memory handle
    UINT    offTknLine,                 // 04:  Offset to tokenized line (TOKEN_HEADER)
            numNxtLabel1;               // 08:  Line # of next labeled line (origin-1) (0 = none)
    UINT    bStop:1,                    // 0C:  00000001:  Stop on this line
            bTrace:1,                   //      00000002:  Trace this line
            bEmpty:1,                   //      00000004:  Empty line
            bLabel:1,                   //      00000008:  Labeled line
            :28;                        //      FFFFFFF0:  Available bits
                                        // 10:  Length
} FCNLINE, *LPFCNLINE;

typedef struct tagLBLENTRY              // Line label entries
{
    LPSYMENTRY lpSymEntry;              // 00:  Ptr to line label SYMENTRY
    UINT       uLineNum1;               // 04:  Line # (origin-1)
} LBLENTRY, *LPLBLENTRY;

#ifdef DEBUG
  // N.B.:  Whenever changing the above enum
  //   be sure to make a corresponding change to
  //   <cDfnTypeStr> below.

  #define cDfnTypeStr     L"?12FIE!@%"
#endif

typedef enum tagFCN_VALENCES            // User-Defined Function/Operator Valence
{
    FCNVALENCE_NIL = 0,                 // 00:  Niladic function
    FCNVALENCE_MON,                     // 01:  Monadic function/derived function
    FCNVALENCE_DYD,                     // 02:  Dyadic  ...
    FCNVALENCE_AMB,                     // 03:  Ambivalent ...
    FCNVALENCE_IMM,                     // 04:  Immediate execution
                                        // 05-07:  Available entries (3 bits)
} FCN_VALENCES;

typedef enum tagSYSLBL_CON
{
    SYSLBL_CON_ID = 0,                  // 00:  System Label Context:  []ID
    SYSLBL_CON_PRO,                     // 01:  ...                    []PRO
    SYSLBL_CON_SGL,                     // 02:  ...                    []SGL
    SYSLBL_CON_LENGTH                   // 03:  # entries in this enum
} SYSLBL_CON, *LPSYSLBL_CON;

typedef enum tagSYSLBL_VAR              // System Label Variant
{
    SYSLBL_VAR_INV,                     // 00:  System Label Variant:  []INV
    SYSLBL_VAR_MS,                      // 01:  ...                    []MS
    SYSLBL_VAR_LENGTH                   // 02:  # entries in this enum
} SYSLBL_VAR, *LPSYSLBL_VAR;

// User-defined function/operator header signature
#define DFN_HEADER_SIGNATURE   'SNFD'

typedef struct tagDFN_HEADER            // Function header structure
{
    HEADER_SIGNATURE Sig;               // 00:  User-defined function/operator header signature
    UINT             Version;           // 04:  Version # of this header
    UINT             DfnType:4,         // 08:  0000000F:  User-defined function/operator type (see DFN_TYPES)
                     FcnValence:3,      //      00000070:  User-defined function/operator valence (see FCN_VALENCES)
                     DfnAxis:1,         //      00000080:  User-defined function/operator accepts axis value
                     PermFn:1,          //      00000100:  Permanent function (i.e. Magic Function/Operator)
                     NoDispRes:1,       //      00000200:  TRUE iff the result is non-displayable
                     ListRes:1,         //      00000400:  TRUE iff the result is a list (unused so far)
                     ListLft:1,         //      00000800:  TRUE iff the left arg is a list
                     ListRht:1,         //      00001000:  TRUE iff the right arg is a list
                     MonOn:1,           //      00002000:  TRUE iff function line monitoring is on for this function
                     SaveSTEFlags:1,    //      00004000:  TRUE iff on free we are to save the function name STE flags
                     SkipRefCntIncr:1,  //      00008000:  Skip the next RefCnt increment
                     bAFO:1,            //      00010000:  TRUE iff this is an AFO
                     bAfoCtrlStruc:1,   //      00020000:  TRUE iff Ctrl Strucs in AFO
                     bAfoNoDispRes:1,   //      00040000:  TRUE iff the AFO result is non-displayable
                     bMFO:1,            //      00080000:  TRUE iff this is an MFO
                     :12;               //      FFF00000:  Available bits
    UINT             RefCnt,            // 0C:  Reference count
                     nSysLblInv,        // 10:  Line # of the []ID  label (0 if not present)
                     nSysLblId,         // 14:  Line # of the []INV label (0 if not present)
                     nSysLblMs,         // 18:  Line # of the []MS  label (0 if not present)
                     nSysLblPro,        // 1C:  Line # of the []PRO label (0 if not present)
                     nSysLblSgl,        // 20:  Line # of the []SGL label (0 if not present)
                     nSysLblPair        // 24:  Line # of the System Label pairs (Context vs. Variant) (12 bytes)
                       [SYSLBL_CON_LENGTH][SYSLBL_VAR_LENGTH],
                     numResultSTE,      // 30:  # result STEs (may be zero if no result)
                     offResultSTE,      // 34:  Offset to result STEs (ignored if numResultSTE is zero)
                     numLftArgSTE,      // 38:  # left arg STEs (may be zero if niladic/monadic)
                     offLftArgSTE,      // 3C:  Offset to left arg STEs (ignored if numLftArgSTE is zero)
                     numRhtArgSTE,      // 40:  # right arg STEs (may be zero if niladic)
                     offRhtArgSTE,      // 44:  Offset to right arg STEs (ignored if numRhtArgSTE is zero)
                     numLocalsSTE,      // 48:  # right arg STEs (may be zero if niladic)
                     offLocalsSTE,      // 4C:  Offset to start of function lines (FCNLINE[nLines])
                     numFcnLines,       // 50:  # lines in the function (not counting the header)
                     offFcnLines,       // 54:  Offset to start of function lines (FCNLINE[nLines])
                     offTknLines,       // 58:  Offset to start of tokenized function lines
                     numLblLines,       // 5C:  # labeled lines in the function
                     num1stLabel1;      // 60:  Line # of 1st labeled line (origin-1)
    LPSYMENTRY       steLftOpr,         // 64:  Left operand STE (may be NULL if not an operator)
                     steFcnName,        // 68:  Function name STE
                     steAxisOpr,        // 6C:  Axis operator STE
                     steRhtOpr;         // 70:  Right operand STE (may be NULL if monadic operator or not an operator)
    HGLOBAL          hGlbTxtHdr,        // 74:  Text of function header (APLCHAR) global memory handle
                     hGlbTknHdr,        // 78:  Tokenized function header (TOKEN) ...
                     hGlbUndoBuff,      // 7C:  Undo buffer (UNDO_BUF)            ... (may be NULL)
                     hGlbMonInfo;       // 80:  Function line monitor info (MONINFO)
    FILETIME         ftCreation,        // 84:  Time of creation (8 bytes)
                     ftLastMod;         // 8C:  Time of last modification (8 bytes)
    HSHTABSTR        htsDFN;            // 94:  Local HTS (htsDFN.lpHshTab may be NULL if none) (124 bytes)
                                        //110:  Length
                                        //110:  Array of function line structures (FCNLINE[nLines])
} DFN_HEADER, *LPDFN_HEADER;

// Whenever changing the above struct, be sure to make a
//   corresponding change to
//   <CalcSymentrySize> in <qf_at.c>,
//   <SaveFunction> in <savefcn.c>,
//   <FEWndProc:WM_CREATE> in <editfcn.c>, and
//   <FreeResultGlobalDfn> in <free.c>.


typedef struct tagFH_YYSTYPE        // YYSTYPE for Function Header parser
{
    TOKEN  tkToken;                 // 00:  Token info (24 bytes)
    UINT   uStrandLen:30,           // 18:  3FFFFFFF:  # elements in this strand
           Indirect:1,              //      40000000:  Indirect entry
           List:1;                  //      80000000:  Itsa list
                                    //      00000000:  No available bits
    struct tagFH_YYSTYPE *
           lpYYStrandIndirect;      // 1C:  Ptr to the indirect strand if .Indirect is set
    struct tagFH_YYSTYPE *
           lpYYStrandBase;          // 20:  Ptr to this token's strand base
                                    // 24:  Length
} FH_YYSTYPE, *LPFH_YYSTYPE;        // Data type of yacc stack

#define YYSTYPE_IS_DECLARED 1


typedef struct tagFHLOCALVARS       // Function Header Local Vars
{
    HWND           hWndEC;              // 00:  Window handle for Edit Ctrl
    HGLOBAL        hGlbTknHdr,          // 04:  Tokenized header global memory handle
                   hGlbUndoBuff;        // 08:  Undo buffer      ...
    LPTOKEN_HEADER lpHeader;            // 0C:  Ptr to tokenized line header in global memory
    LPTOKEN        lptkStart,           // 10:  First available entry after the header
                   lptkNext,            // 14:  Next  ...
                   lptkStop;            // 18:  Stopping token
    UINT           tkErrorCharIndex;    // 1C:  Error char index
    UINT           DfnType:4,           // 20:  0000000F:  User-defined function/operator type (see DFN_TYPES)
                   FcnValence:3,        //      00000070:  User-defined function/operator valence (see FCN_VALENCES)
                   DfnAxis:1,           //      00000080:  User-defined function/operator accepts axis value
                   DisplayErr:1,        //      00000100:  TRUE iff we should display error messages
                   NoDispRes:1,         //      00000200:  TRUE iff the result is non-displayable
                   ListRes:1,           //      00000400:  TRUE iff the result is a list
                   ListLft:1,           //      00000800:  TRUE iff the left arg ...
                   ListRht:1,           //      00001000:  TRUE iff the right arg ...
                   ParseFcnName:1,      //      00002000:  TRUE iff we're parsing the function name
                   fhNameType:4,        //      0003C000:  Function name type (see NAME_TYPES)
                   bAFO:1,              //      00040000   TRUE iff we're parsing an AFO
                   :13;                 //      FFF80000:  Available bits
    LPFH_YYSTYPE   lpYYStrandStart,     // 24:  Strand stack start (static)
                   lpYYStrandBase,      // 28:  ...          base (dynamic)
                   lpYYStrandNext,      // 2C:  ...          next token (dynamic)
                   lpYYResult,          // 30:  Ptr to result name or list
                   lpYYLftArg,          // 34:  ...    left arg name or list
                   lpYYLftOpr,          // 38:  ...    left operand name
                   lpYYFcnName,         // 3C:  ...    function/operator name
                   lpYYAxisOpr,         // 40:  ...    axis operator name
                   lpYYRhtOpr,          // 44:  ...    right operand name
                   lpYYRhtArg,          // 48:  ...    right arg name or list
                   lpYYLocals;          // 4C:  ...    locals name or list
    WCHAR          wszErrMsg[256];      // 50:  Save area for error message
                                        //250:  Length
} FHLOCALVARS, *LPFHLOCALVARS;


//***************************************************************************
//  End of File: fh_parse.h
//***************************************************************************
