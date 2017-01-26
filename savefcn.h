//***************************************************************************
//  NARS2000 -- Save Function Header
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

typedef enum tagSF_TYPES
{
    SFTYPES_UNK = 0 ,               // 00:  Unknown type
    SFTYPES_FX      ,               // 01:  Called from []FX
    SFTYPES_AFO     ,               // 02:  ...         AFO
    SFTYPES_TF      ,               // 03:  ...         []TF
    SFTYPES_AA      ,               // 04:  ...         )INASCII
    SFTYPES_LOAD    ,               // 05:  ...         )LOAD/...
    SFTYPES_FE      ,               // 06:  ...         Function Editor
} SF_TYPES, *LPSF_TYPES;

typedef struct tagSF_FCNS
{
    UINT        bDisplayErr:1,      // 00:  00000001:  TRUE iff we should display errors
                bRet:1,             //      00000002:  TRUE iff result is valid
                bAFO:1,             //      00000004:  TRUE iff we're parsing an AFO
                bMakeAFO:1,         //      00000008:  TRUE iff we're called from <MakeAFO>
                bSetAlpha:1,        //      00000010:  TRUE iff some stmt sets {alpha}
                bRefAlpha:1,        //      00000020:  TRUE iff some stmt references {alpha}
                bRefLftOper:1,      //      00000040:  TRUE iff ...                  {alpha}{alpha}
                bRefOmega:1,        //      00000080:  TRUE iff ...                  {omega}
                bRefRhtOper:1,      //      00000100:  TRUE iff ...                  {omega}{omega
                bMFO:1,             //      00000200:  TRUE iff this is an MFO
                bLclRL:1,           //      00000400:  TRUE iff []RL is localized in this function
                bMakeAFE:1,         //      000008000  TRUE iff we're called from <SaveFunction> as an AFO
                bMakeAFX:1,         //      000010000  TRUE iff we're called from <SysFnMonFX_EM> as an AFO
                :19;                //      FFFFE000:  Available bits
    UINT        uErrLine;           // 04:  If (!bRet), the line in error (origin-0) (NEG1U = Error Message valid)
    LPSYMENTRY  lpSymName;          // 08:  If (bRet), ptr to SYMENTRY of the function name
    LPTOKEN     lptkFunc;           // 0C:  Ptr to function token
    LPVOID      LclParams;          // 10:  Ptr to local parameters
    UINT      (*SF_LineLen)         // 14:  Ptr to get line length function
                (HWND, struct tagSF_FCNS *, UINT);
    void      (*SF_ReadLine)        // 18:  Ptr to read line function
                (HWND, struct tagSF_FCNS *, UINT, LPAPLCHAR);
    UBOOL     (*SF_IsLineCont)      // 1C:  Ptr to Is Line Continued function
                (HWND, struct tagSF_FCNS *, UINT);
    UINT      (*SF_NumPhyLines)     // 20:  Ptr to get # physical lines function
                (HWND, struct tagSF_FCNS *);
    UINT      (*SF_NumLogLines)     // 24:  Ptr to get # logical  ...
                (HWND, struct tagSF_FCNS *);
    void      (*SF_CreationTime)    // 28:  Ptr to get function creation time
                (struct tagSF_FCNS *, SYSTEMTIME *, FILETIME *);
    void      (*SF_LastModTime)     // 2C:  Ptr to get function last mod time
                (struct tagSF_FCNS *, SYSTEMTIME *, FILETIME *);
    HGLOBAL   (*SF_UndoBuffer)      // 30:  Ptr to get function Undo Buffer global memory handle
                (HWND, struct tagSF_FCNS *);
    HGLOBAL     hGlbDfnHdr;         // 34:  User-defined function/operator global memory handle
    UINT        numLocalsSTE;       // 38:  # locals in AFO
    LPSYMENTRY *lplpLocalSTEs;      // 3C:  Ptr to save area for local STEs (may be NULL during sizing)
    LPHSHTABSTR lpHTS;              // 40:  Ptr to HshTabStr (may be NULL)
    HSHTABSTR   htsDFN;             // 44:  Local HshTab struc for this AFO (124 bytes)
    SF_TYPES    sfTypes;            // 48:  Caller type (see SF_TYPES above)
    WCHAR       wszErrMsg[256];     // 4C:  Save area for error message
                                    //24C:  Length of struc
} SF_FCNS, *LPSF_FCNS;


// Aliases for common functions
#define SF_IsLineContM      SF_IsLineContCom
#define SF_IsLineContN      SF_IsLineContCom
#define SF_IsLineContAA     SF_IsLineContCom
#define SF_IsLineContAN     SF_IsLineContCom
#define SF_IsLineContLW     SF_IsLineContCom
#define SF_IsLineContTF1    SF_IsLineContCom

#define SF_NumPhyLinesM     SF_NumLinesM
#define SF_NumLogLinesM     SF_NumLinesM
#define SF_NumPhyLinesN     SF_NumLinesCom
#define SF_NumLogLinesN     SF_NumLinesCom
#define SF_NumPhyLinesAA    SF_NumLinesAA
#define SF_NumLogLinesAA    SF_NumLinesAA
#define SF_NumPhyLinesAN    SF_NumLinesAN
#define SF_NumLogLinesAN    SF_NumLinesAN
#define SF_NumPhyLinesLW    SF_NumLinesLW
#define SF_NumLogLinesLW    SF_NumLinesLW
#define SF_NumPhyLinesTF1   SF_NumLinesTF1
#define SF_NumLogLinesTF1   SF_NumLinesTF1

#define SF_CreationTimeM    SF_CreationTimeCom
#define SF_CreationTimeN    SF_CreationTimeCom
#define SF_CreationTimeSV   SF_CreationTimeCom
#define SF_CreationTimeTF1  SF_CreationTimeCom
#define SF_CreationTimeAA   SF_CreationTimeCom

#define SF_LastModTimeM     SF_LastModTimeCom
#define SF_LastModTimeN     SF_LastModTimeCom
#define SF_LastModTimeSV    SF_LastModTimeCom
#define SF_LastModTimeTF1   SF_LastModTimeCom
#define SF_LastModTimeAA    SF_LastModTimeCom

#define SF_UndoBufferM      SF_UndoBufferCom
#define SF_UndoBufferN      SF_UndoBufferCom
#define SF_UndoBufferSV     SF_UndoBufferCom
#define SF_UndoBufferTF1    SF_UndoBufferCom
#define SF_UndoBufferAA     SF_UndoBufferCom


typedef struct tagFX_PARAMS
{
    HGLOBAL         hGlbRht;        // 00:  Right arg global memory handle
    APLRANK         aplRankRht;     // 04:  Right arg rank
    APLLONGEST      aplLongestRht;  // 0C:  Right arg immediate value
    APLDIM          aplRowsRht,     // 14:  Right arg # rows (8 bytes)
                    aplColsRht;     // 1C:  ...         cols  ...
    struct tagAFODETECT_STR *
                    lpafoDetectStr; // 20:  Ptr to AFO Detect struc (24 bytes)
                                    // 38:  Length of struc
} FX_PARAMS, *LPFX_PARAMS;


typedef struct tagLW_PARAMS         // LoadWorkspace & AFO struc
{
    LPWCHAR       lpwSectName,      // 00:  Ptr to section name [nnn.Name]
                  lpwBuffer,        // 04:  Ptr to temporary buffer
                  lpMemUndoTxt;     // 08:  Ptr to Undo Buffer in text format
    LPDICTIONARY  lpDict;           // 0C:  Ptr to workspace dictionary
    APLI3264      iMaxSize;         // 10:  Maximum size of lpwBuffer
    FILETIME      ftCreation,       // 14:  Function Creation Time
                  ftLastMod;        // 18:  Function Last Modification Time
    LPWCHAR       lpwszVersion;     // 1C:  Ptr to workspace version text
    LPPL_YYSTYPE  lpYYRht;          // 20:  Ptr to right brace YYSTYPE
    LPPLLOCALVARS lpplLocalVars;    // 24:  Ptr to LocalVars
                                    // 28:  Length
} LW_PARAMS, *LPLW_PARAMS;


typedef struct tagTF1_PARAMS
{
    LPAPLCHAR lpMemRht;             // 00:  Ptr to right arg global memory
    APLDIM    aplRowsRht,           // 04:  Right arg # rows
              aplColsRht;           // 08:  Right arg # cols
} TF1_PARAMS, *LPTF1_PARAMS;


typedef struct tagAA_PARAMS
{
    UINT    NumLines;               // 00:  # lines in the function
    LPWCHAR lpwStart;               // 04:  Ptr to start of function
} AA_PARAMS, *LPAA_PARAMS;


//***************************************************************************
//  End of File: savefcn.h
//***************************************************************************
