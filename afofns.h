//***************************************************************************
//  NARS2000 -- AFO Detector Header
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
//   column order in fsaActTableAFO.

typedef enum tagAFOCOL_INDICES      // FSA column indices for AfoDetect
{AFOCOL_DIGIT = 0   ,               // 00:  Digit
 AFOCOL_ALPHA       ,               // 01:  Alphabetic
 AFOCOL_OVERBAR     ,               // 02:  Overbar
 AFOCOL_ASSIGN      ,               // 03:  Assignment symbol
 AFOCOL_LEFTBRACE   ,               // 04:  Left brace
 AFOCOL_RIGHTBRACE  ,               // 05:  Right ...
 AFOCOL_SPACE       ,               // 06:  White space (' ' or '\t')
 AFOCOL_QUOTE1      ,               // 07:  Single quote symbol
 AFOCOL_QUOTE2      ,               // 08:  Double ...
 AFOCOL_LAMP        ,               // 09:  Comment symbol
 AFOCOL_EOL         ,               // 0A:  End-Of-Line
 AFOCOL_UNK         ,               // 0B:  Unknown symbols
 AFOCOL_CR          ,               // 0C:  CR

 AFOCOL_LENGTH      ,               // 0D:  # column indices (cols in fsaActTableAFO)
                                    //      Because this enum is origin-0, this value is the # valid columns.
} AFOCOLINDICES, *LPAFOCOLINDICES;


// Whenever you add a new AFOCOL_*** entry,
//   be sure to put code into <CharTransAFO> in <afofns.c>
//   to return the newly defined value.


// The order of the values of these constants *MUST* match the
//   row order in fsaActTableAFO.
typedef enum tagAFOROW_INDICES      // FSA row indices for AfoDetect
{AFOROW_SOS = 0     ,               // 00:  Start of stmt
 AFOROW_ALPHA       ,               // 01:  Alphabetic char
 AFOROW_SPACE       ,               // 01:  White space after Alpha
 AFOROW_ASSIGN      ,               // 01:  Assignment arrow
 AFOROW_LBR_INIT    ,               // 06:  Inside braces
 AFOROW_LBR_EXIT    ,               // 06:  Outside braces
 AFOROW_LBR_Q1      ,               // 07:  Inside braces, single quotes
 AFOROW_LBR_Q2      ,               // 08:  Inside braces, double quotes
 AFOROW_LENGTH      ,               // 09:  # FSA terminal states (rows in fsaActTableAFO)
                                    //      Because this enum is origin-0, this value is the # valid rows.
 AFOROW_EXIT   = -1 ,               // FSA is done
 AFOROW_WSFULL = -2 ,               // FSA is done, WS FULL

} AFOROWINDICES, *LPAFOROWINDICES;

typedef struct tagAFOLOCALVARS
{
    AFOROWINDICES  State;           // 00:  Current state (see AFOROW_INDICES)
    UINT           uChar;           // 04:  Current index into lpwszOrig
    LPWCHAR        lpwszOrig;       // 08:  Ptr to original lpwszOrig
    int            lbrCount;        // 0C:  Count of accumulated left braces
    UINT           bLbrAlpha:1,     // 10:  00000001:  TRUE iff we encountered an alphabetic char
                   bLbrAssign:1,    //      00000002:  TRUE iff we encountered an assignment symbol
                   bLbrInit:1;      //      00000004:  TRUE iff we encountered a left brace
    struct tagAFODETECT_STR *
                   lpafoDetectStr;  // 14:  Ptr to AFO Detect struc (24 bytes)
                                    // 18:  Length
} AFOLOCALVARS, *LPAFOLOCALVARS;

typedef UBOOL (*AFO_ACTION) (LPAFOLOCALVARS);

typedef struct tagAFO_ACTSTR
{
    AFOROWINDICES iNewState;        // 00:  New state (see AFOROW_INDICES)
    AFO_ACTION    afoAction1,       // 04:  Action #1
                  afoAction2;       // 08:  ...    #2
                                    // 0C:  Length
} AFOACTSTR, *LPAFOACTSTR;

typedef struct tagAFOLINE_STR
{
    UINT uLineOff,                  // 00:  Line offset of a physical line
         uLineLen;                  // 04:  ...  length ...
    UINT bLineCont:1,               // 08:  00000001:  TRUE iff this physical line is continued
                                    //                 onto the next physical line
         :31;
} AFOLINE_STR, *LPAFOLINE_STR;

typedef struct tagAFODETECT_STR
{
    HGLOBAL       hGlbLineStr;      // 00:  Global memory handle for lpafoLineStr
    UINT          numPhyLines,      // 04:  # physical lines (excluding the header)
                  numLogLines,      // 08:  # logical  ...    ...
                  uLastLineOff,     // 0C:  Last line offset
                  uLineStrCnt,      // 10:  Total # entries in lpafoLineStr
                  uLineStrNxt;      // 14:  Next available index into lpafoLineStr
    LPAFOLINE_STR lpafoLineStr;     // 18:  Ptr to AFO line struc (AFOLINE_STR[1 + numPhyLines])
                                    // 1C:  Length of struc
} AFODETECT_STR, *LPAFODETECT_STR;

#define AFOLINESTR_INIT     20      // Initial allocation for AFOLINE_STR
#define AFOLINESTR_INCR     20      // Incremental ...


//***************************************************************************
//  End of File: afofns.h
//***************************************************************************
