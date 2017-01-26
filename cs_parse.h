//***************************************************************************
//  NARS2000 -- Control Structures Header
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


typedef struct tagFORSTMT           // FOR/FORLCL stmt struc
{
    APLUINT    uIndex;              // 00:  Index into the array in tkForArr (8 bytes)
    APLNELM    uNELM;               // 08:  NELM of    ...
    TOKEN      tkForI,              // 10:  Token for I in ":FOR I :IN Arr" token (16 bytes)
               tkForArr;            // 20:  Token for Arr ...
    UINT       uForStmtID;          // 30:  FOR/FORLCL stmt identifier
    SYMENTRY   symForI;             // 34:  SYMENTRY for I in ":FOR I :IN Arr" token (28 bytes)
                                    // 50:  Length
} FORSTMT, *LPFORSTMT;


typedef struct tagCS_YYSTYPE        // YYSTYPE for Control Structure parser
{
    LPTOKEN lptkCur,                // 00:  Ptr to current token on CS stack
            lptk1st,                // 04:  Ptr to 1st token in a sequence
            lptkNxt,                // 08:  Ptr to next token in a sequence
            lptkIF1st,              // 0C:  Ptr to 1st token in a sequence of IF/ELSE/.../ENDIF
            lptkCL1st;              // 10:  Ptr to 1st unmatched ContinueLeave in this group
    UINT    uCLIndex;               // 14:  ContinueLeave index common to this group
                                    // 18:  Length
} CS_YYSTYPE, *LPCS_YYSTYPE;        // Data type of yacc stack

#define YYSTYPE_IS_DECLARED 1


typedef struct tagCSLOCALVARS       // Control Structure Local Vars
{
    HWND         hWndEC;            // 00:  Window handle for Edit Ctrl
    LPPERTABDATA lpMemPTD;          // 04:  Ptr to PerTabData global memory
    LPTOKEN      lptkCSBeg,         // 08:  First available entry after the header
                 lptkCSNxt,         // 0C:  Next  ...
                 lptkCSLink;        // 10:  Ptr to CS stack Stmt link (NULL = none)
    TOKEN        tkCSErr;           // 14:  Error token (28 bytes)
    UINT         bDisplayErr:1,     // 30:  00000001:  TRUE iff we should display error messages
                 bMainStmt:1,       //      00000002:  TRUE iff we encountered a CS main stmt (FOR, FORLCL, IF, REPEAT, SELECT, WHILE)
                 :30;               //      FFFFFFFC:  Available bits
    HGLOBAL      hGlbDfnHdr,        // 34:  Defined function/operator global memory handle
                                    //      (NULL = hGlbImmExec is valid)
                 hGlbImmExec;       // 38:  Immediate execution tokenized line global memory habndle
                                    //      (NULL = hGlbDfnHdr is valid)
    LPWCHAR      lpwszErrMsg;       // 3C:  Ptr to error message (e.g. L"CS_SYNTAX ERROR")
                                    // 40:  Length
} CSLOCALVARS, *LPCSLOCALVARS;



//***************************************************************************
//  End of File: cs_parse.h
//***************************************************************************
