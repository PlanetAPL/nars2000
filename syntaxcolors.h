//***************************************************************************
//  NARS2000 -- Syntax Coloring Header
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

#define DEF_SCN_TRANSPARENT    DEF_SCN_WHITE

#define DEF_SC_GLBNAME         DEF_SCN_RED          , DEF_SCN_TRANSPARENT   // 00
#define DEF_SC_LCLNAME         DEF_SCN_GRAY         , DEF_SCN_TRANSPARENT   // 01
#define DEF_SC_LABEL           DEF_SCN_MAROON       , DEF_SCN_TRANSPARENT   // 02
#define DEF_SC_PRIMFCN         DEF_SCN_NAVY         , DEF_SCN_TRANSPARENT   // 03
#define DEF_SC_PRIMOP1         DEF_SCN_ROYALBLUE    , DEF_SCN_TRANSPARENT   // 04
#define DEF_SC_PRIMOP2         DEF_SCN_LIME         , DEF_SCN_TRANSPARENT   // 05
#define DEF_SC_SYSFCN          DEF_SCN_NAVY         , DEF_SCN_TRANSPARENT   // 06
#define DEF_SC_GLBSYSVAR       DEF_SCN_PURPLE       , DEF_SCN_TRANSPARENT   // 07
#define DEF_SC_LCLSYSVAR       DEF_SCN_PURPLE       , DEF_SCN_TRANSPARENT   // 08
#define DEF_SC_CTRLSTRUC       DEF_SCN_MAROON       , DEF_SCN_TRANSPARENT   // 09
#define DEF_SC_NUMCONST        DEF_SCN_GRAY         , DEF_SCN_TRANSPARENT   // 0A
#define DEF_SC_CHRCONST        DEF_SCN_TEAL         , DEF_SCN_TRANSPARENT   // 0B
#define DEF_SC_PNSEP           DEF_SCN_CRIMSON      , DEF_SCN_TRANSPARENT   // 0C
#define DEF_SC_COMMENT         DEF_SCN_GREEN        , DEF_SCN_TRANSPARENT   // 0D
#define DEF_SC_LINEDRAWING     DEF_SCN_TEAL         , DEF_SCN_TRANSPARENT   // 0E
#define DEF_SC_FCNLINENUMS     DEF_SCN_DARKSEAGREEN , DEF_SCN_TRANSPARENT   // 0F
#define DEF_SC_MATCHGRP1       DEF_SCN_BLUE         , DEF_SCN_TRANSPARENT   // 10
#define DEF_SC_MATCHGRP2       DEF_SCN_DARKGREEN    , DEF_SCN_TRANSPARENT   // 11
#define DEF_SC_MATCHGRP3       DEF_SCN_DARKCYAN     , DEF_SCN_TRANSPARENT   // 12
#define DEF_SC_MATCHGRP4       DEF_SCN_DARKMAGENTA  , DEF_SCN_TRANSPARENT   // 13
#define DEF_SC_UNMATCHGRP      DEF_SCN_BLUE         , DEF_SCN_LIGHTCYAN     // 14
#define DEF_SC_UNNESTED        DEF_SCN_BLUE         , DEF_SCN_LIGHTPINK     // 15
#define DEF_SC_UNK             DEF_SCN_BLUE         , DEF_SCN_VIOLET        // 16
#define DEF_SC_LINECONT        DEF_SCN_DODGERBLUE   , DEF_SCN_TRANSPARENT   // 17
#define DEF_SC_WINTEXT         DEF_SCN_BLACK        , DEF_SCN_WHITE         // 18

// Syntax Coloring categories
typedef enum tagSC_TYPE
{
    SC_GLBNAME = 0,     // 00:  Global Name
    SC_LCLNAME,         // 01:  Local  ...
    SC_LABEL,           // 02:  Label (including trailing colon)
    SC_PRIMFCN,         // 03:  Primitive Function (including quad, quote-quad, left/right arrows,
                        //      del, semicolon, and diamond)
    SC_PRIMOP1,         // 04:  Primitive Monadic Operator
    SC_PRIMOP2,         // 05:  Primitive Dyadic Operator
    SC_SYSFCN,          // 06:  System Function (including leading quad)
    SC_GLBSYSVAR,       // 07:  Global System Variable (including leading quad)
    SC_LCLSYSVAR,       // 08:  Local  ...
    SC_CTRLSTRUC,       // 09:  Control Structure (including leading colon)
    SC_NUMCONST,        // 0A:  Numeric constant (including leading overbar)
    SC_CHRCONST,        // 0B:  Character constant (including single- or double-quote marks)
    SC_PNSEP,           // 0C:  Point notation separator (beEprvx)
    SC_COMMENT,         // 0D:  Comment (including comment symbol)
    SC_LINEDRAWING,     // 0E:  Line drawing chars
    SC_FCNLINENUMS,     // 0F:  Function Line Numbers
    SC_MATCHGRP1,       // 10:  Matched Grouping Symbols [] () {}
    SC_MATCHGRP2,       // 11:  Matched Grouping Symbols [] () {}
    SC_MATCHGRP3,       // 12:  Matched Grouping Symbols [] () {}
    SC_MATCHGRP4,       // 13:  Matched Grouping Symbols [] () {}
    SC_UNMATCHGRP,      // 14:  Unmatched Grouping Symbols [] () {} ' "
    SC_UNNESTED,        // 15:  Improperly Nested Grouping Symbols [] () {}
    SC_UNK,             // 16:  Unknown symbol
    SC_LINECONT,        // 17:  Line Continuation
    SC_WINTEXT,         // 18:  Window text
    SC_LENGTH           // 19:  # entries in this enum
                        //      Because this enum is origin-0, this value is the # valid columns.
} SCTYPE, *LPSCTYPE;

#define NUM_MATCHGRPS       4

// N.B.:  Whenever changing the above enum (SC_TYPE),
//   be sure to make a corresponding change to
//   <DEF_SC_xxx> in <syntaxcolors.h> (above),
//   <gSyntaxColorName> and <gSyntClrBGTrans> in <externs.h>,
//   <KEYNAME_SC_xxx> and <aColorKeyNames> in <inifile.c>,
//   <#define IDC_SYNTCLR_XB_TRANSxx>,
//   <#define IDC_SYNTCLR_BN_FGCLRxx>,
//   <#define IDC_SYNTCLR_BN_BGCLRxx>,
//   <#define IDC_SYNTCLR_LT_FGMRKxx>,
//   <#define IDC_SYNTCLR_LT_BGMRKxx> in <resource.h>, and
//   <SYNTAX COLORING -- WM_COMMAND> in <CustomizeDlgProc> in <customize.c>.
// If changing the # SC_MATCHGRPn,
//   be sure to make a corresponding change to
//   <aMatchGrp> in <GroupDoneCom> in <tokenize.c>.

typedef struct tagSYNTAX_COLORS
{
    COLORREF crFore,        // 00:  Foreground color
             crBack;        // 04:  Background color (-1 = transparent)
                            // 08:  Length
} SYNTAXCOLORS, *LPSYNTAXCOLORS;


//***************************************************************************
//  End of File: syntaxcolors.h
//***************************************************************************
