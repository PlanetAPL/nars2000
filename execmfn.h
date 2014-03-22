//***************************************************************************
//  NARS2000 -- Magic Function/Operator Execution Header
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

// Magic Function/Operator Enum
typedef enum tagMFO
{
    MFOE_MonIota         ,      // 00:  Extended Monadic Iota
    MFOE_DydIota         ,      // 01:  Extended Dyadic Iota
    MFOE_MonDnShoe       ,      // 02:  Monadic Down Shoe
    MFOE_MonRank         ,      // 03:  Monadic Derived Function from Rank Operator
    MFOE_DydRank         ,      // 04:  Dyadic  ...
    MFOE_Conform         ,      // 05:  Conform  (for Rank Operator)
    MFOE_MonFMT          ,      // 06:  Monadic []FMT
    MFOE_BoxFMT          ,      // 07:  Box (for monadic []FMT)
    MFOE_MonVR           ,      // 08:  Monadic []VR
    MFOE_IdnDot          ,      // 09:  Identity function for Inner Product
    MFOE_IdnJotDot       ,      // 0A:  Identity function for Outer Product
    MFOE_MAD             ,      // 0B:  Multiset Asymmetric Difference
    MFOE_MSD             ,      // 0C:  Multiset Symmetric Difference
    MFOE_MU              ,      // 0D:  Multiset Union
    MFOE_MI              ,      // 0E:  Multiset Intersection
    MFOE_MIO             ,      // 0F:  Multiset Index Of
    MFOE_MEO             ,      // 10:  Multiset Element Of
    MFOE_MM              ,      // 11:  Multiset Match
    MFOE_MLRS            ,      // 12:  Multiset LeftShoe
    MFOE_MLRSU           ,      // 13:  Multiset LeftShoeUnderbar
    MFOE_MMUL            ,      // 14:  Multiset Multiplicities
    MFOE_DydDnShoe       ,      // 15:  Dyadic Down Shoe
    MFOE_DydUpShoe       ,      // 16:  Dyadic Up Shoe
    MFOE_DydLRShoeUnd    ,      // 17:  Dyadic Left/Right Shoe Underbar
    MFOE_SD              ,      // 18:  Symmetric Difference
    MFOE_MonDomino       ,      // 19:  Monadic Domino for global numerics
    MFOE_DydDomino       ,      // 1A:  Dyadic  ...
    MFOE_DydDotDot       ,      // 1B:  Dyadic DotDot
    MFOE_DydIotaUnderbar ,      // 1C:  Dyadic Iota Underbar
    MFOE_MonDot          ,      // 1D:  Monadic derived function from f.g for determinant
    MFOE_MonDotInit      ,      // 1E:  Initialization for #MonDot
    MFOE_MonDotAll       ,      // 1F:  Subroutine to ...
    MFOE_MonDotPerm      ,      // 20:  ...
    MFOE_MonDotMf        ,      // 21:  ...
    MFOE_MonDotCr        ,      // 22:  ...
    MFOE_DydEpsUnderbar  ,      // 23:  Dyadic Epsilon Underbar on empty args
    MFOE_DydConv         ,      // 24:  Dyadic derived function from Convolution
    MFOE_IdnConv         ,      // 25:  Identity function for Convolution
    MFOE_LENGTH                 // 26:  # entries in this enum
} MFO, *LPMFO;

// Magic Function/Operator Names
#define MFON_MonIota            L"#MonIota"
#define MFON_DydIota            L"#DydIota"
#define MFON_MonDnShoe          L"#MonDnShoe"
#define MFON_MonRank            L"#MonRank"
#define MFON_DydRank            L"#DydRank"
#define MFON_Conform            L"#Conform"
#define MFON_MonFMT             L"#MonFMT"
#define MFON_BoxFMT             L"#BoxFMT"
#define MFON_MonVR              L"#MonVR"
#define MFON_IdnDot             L"#IdnDot"
#define MFON_IdnJotDot          L"#IdnJotDot"
#define MFON_MAD                L"#MAD"
#define MFON_MSD                L"#MSD"
#define MFON_MU                 L"#MU"
#define MFON_MI                 L"#MI"
#define MFON_MIO                L"#MIO"
#define MFON_MEO                L"#MEO"
#define MFON_MM                 L"#MM"
#define MFON_MLRS               L"#MLRS"
#define MFON_MLRSU              L"#MLRSU"
#define MFON_MMUL               L"#MMUL"
#define MFON_DydDnShoe          L"#DydDnShoe"
#define MFON_DydUpShoe          L"#DydUpShoe"
#define MFON_DydLRShoeUnd       L"#DydLRShoeUnd"
#define MFON_SD                 L"#SD"
#define MFON_MonDomino          L"#MonDomino"
#define MFON_DydDomino          L"#DydDomino"
#define MFON_DydDotDot          L"#DydDotDot"
#define MFON_DydIotaUnderbar    L"#DydIotaUnderbar"
#define MFON_MonDot             L"#MonDot"
#define MFON_MonDotInit         L"#MonDotInit"
#define MFON_MonDotAll          L"#MonDotAll"
#define MFON_MonDotPerm         L"#MonDotPerm"
#define MFON_MonDotMf           L"#MonDotMf"
#define MFON_MonDotCr           L"#MonDotCr"
#define MFON_DydEpsUnderbar     L"#DydEpsUnderbar"
#define MFON_DydConv            L"#DydConv"
#define MFON_IdnConv            L"#IdnConv"

// Magic Function/Operator Variables
#define $ALPHA          WS_UTF16_ALPHA
#define $AFORESULT      WS_UTF16_QUAD L"Z"
#define $AND            WS_UTF16_UPCARET
#define $CEILING        WS_UTF16_UPSTILE
#define $COMMABAR       WS_UTF16_COMMABAR
#define $COMMENT        WS_UTF16_LAMP
#define $COMMUTE        WS_UTF16_DIERESISTILDE
#define $DECODE         WS_UTF16_UPTACK
#define $DIAMOND        WS_UTF16_DIAMOND
#define $DROP           WS_UTF16_DOWNARROW
#define $DEL            WS_UTF16_DEL
#define $DELDEL         WS_UTF16_DELDEL
#define $DISCLOSE       WS_UTF16_RIGHTSHOE
#define $DIVIDE         WS_UTF16_COLONBAR
#define $DOMINO         WS_UTF16_DOMINO
#define $DUPLICATE      WS_UTF16_DIERESISTILDE
#define $EACH           WS_UTF16_DIERESIS
#define $ENCLOSE        WS_UTF16_LEFTSHOE
#define $ENCODE         WS_UTF16_DOWNTACK
#define $EPSILON        WS_UTF16_EPSILON
#define $EXECUTE        WS_UTF16_UPTACKJOT
#define $FIRST          WS_UTF16_UPARROW
#define $FLOOR          WS_UTF16_DOWNSTILE
#define $FORMAT         WS_UTF16_DOWNTACKJOT
#define $GE             WS_UTF16_RIGHTCARETUNDERBAR
#define $GOTO           WS_UTF16_RIGHTARROW
#define $GRADEDN        WS_UTF16_DELSTILE
#define $GRADEUP        WS_UTF16_DELTASTILE
#define $GDN            WS_UTF16_DELSTILE
#define $GUP            WS_UTF16_DELTASTILE
#define $HASH           WS_UTF16_NUMBER
#define $IOTA           WS_UTF16_IOTA
#define $IOTAUND        WS_UTF16_IOTAUNDERBAR
#define $IS             WS_UTF16_LEFTARROW
#define $JOT            WS_UTF16_JOT
#define $LDC_LT_HORZ    WS_UTF16_LDC_LT_HORZ
#define $LDC_LT_VERT    WS_UTF16_LDC_LT_VERT
#define $LDC_LT_UL      WS_UTF16_LDC_LT_UL
#define $LDC_LT_UR      WS_UTF16_LDC_LT_UR
#define $LDC_LT_LL      WS_UTF16_LDC_LT_LL
#define $LDC_LT_LR      WS_UTF16_LDC_LT_LR
#define $LDC_LT_VERT_R  WS_UTF16_LDC_LT_VERT_R
#define $LDC_LT_VERT_L  WS_UTF16_LDC_LT_VERT_L
#define $LDC_LT_HORZ_D  WS_UTF16_LDC_LT_HORZ_D
#define $LDC_LT_HORZ_U  WS_UTF16_LDC_LT_HORZ_U
#define $LOPER          WS_UTF16_LFTOPER
#define $MATCH          WS_UTF16_EQUALUNDERBAR
#define $MAX            WS_UTF16_UPSTILE
#define $MIN            WS_UTF16_DOWNSTILE
#define $MISMATCH       WS_UTF16_NOTEQUALUNDERBAR
#define $MULTISET       WS_UTF16_DOWNSHOESTILE
#define $NE             WS_UTF16_NOTEQUAL
#define $NEG            WS_UTF16_OVERBAR
#define $NOT            WS_UTF16_TILDE
#define $NOTEQUAL       WS_UTF16_NOTEQUAL
#define $NUMBER         WS_UTF16_NUMBER
#define $OMEGA          WS_UTF16_OMEGA
#define $OR             WS_UTF16_DOWNCARET
#define $PARENT         WS_UTF16_NUMBER WS_UTF16_NUMBER
#define $PICK           WS_UTF16_RIGHTSHOE
#define $PSUBSET        WS_UTF16_LEFTSHOE
#define $PSUPERSET      WS_UTF16_RIGHTSHOE
#define $QUAD           WS_UTF16_QUAD
#define $QUAD_ALX       WS_UTF16_QUAD L"ALX"
#define $QUAD_AT        WS_UTF16_QUAD L"AT"
#define $QUAD_CT        WS_UTF16_QUAD L"CT"
#define $QUAD_CR        WS_UTF16_QUAD L"CR"
#define $QUAD_DM        WS_UTF16_QUAD L"DM"
#define $QUAD_ELX       WS_UTF16_QUAD L"ELX"
#define $QUAD_EM        WS_UTF16_QUAD L"EM"
#define $QUAD_ERROR     WS_UTF16_QUAD L"ERROR"
#define $QUAD_FEATURE   WS_UTF16_QUAD L"FEATURE"
#define $QUAD_FMT       WS_UTF16_QUAD L"FMT"
#define $QUAD_FPC       WS_UTF16_QUAD L"FPC"
#define $QUAD_FX        WS_UTF16_QUAD L"FX"
#define $QUAD_ID        WS_UTF16_QUAD SYSLBL_ID
#define $QUAD_INV       WS_UTF16_QUAD SYSLBL_INV
#define $QUAD_IO        WS_UTF16_QUAD L"IO"
#define $QUAD_LX        WS_UTF16_QUAD L"LX"
#define $QUAD_MS        WS_UTF16_QUAD SYSLBL_MS
#define $QUAD_NC        WS_UTF16_QUAD L"NC"
#define $QUAD_PRO       WS_UTF16_QUAD SYSLBL_PRO
#define $QUAD_SGL       WS_UTF16_QUAD SYSLBL_SGL
#define $QUAD_TCNL      WS_UTF16_QUAD L"TCNL"
#define $QUAD_WSID      WS_UTF16_QUAD L"WSID"
#define $RANK           WS_UTF16_DIERESISJOT
#define $REVERSE        WS_UTF16_CIRCLESTILE
#define $RHO            WS_UTF16_RHO
#define $ROPER          WS_UTF16_RHTOPER
#define $ROTATE         WS_UTF16_CIRCLESTILE
#define $SQUAD          WS_UTF16_SQUAD
#define $SUBSET         WS_UTF16_LEFTSHOEUNDERBAR
#define $SUPERSET       WS_UTF16_RIGHTSHOEUNDERBAR
#define $TAKE           WS_UTF16_UPARROW
#define $TILDE          WS_UTF16_TILDE
#define $TIMES          WS_UTF16_TIMES
#define $TRANSPOSE      WS_UTF16_CIRCLESLOPE
#define $TYPE           WS_UTF16_DOWNTACK
#define $ZILDE          WS_UTF16_ZILDE


typedef struct tagMAGIC_FCNOPR
{
    LPAPLCHAR  Header;              // 00:  Ptr to function/operator header
    LPAPLCHAR *Body;                // 04:  Ptr to array of function lines
    UINT       numFcnLines;         // 08:  # function lines in the body
                                    // 0C:  Length
} MAGIC_FCNOPR, *LPMAGIC_FCNOPR;

typedef struct tagINIT_MFO
{
    struct tagHSHTABSTR
                *lphtsMFO;          // 00:  Ptr to HSHTABSTR struc
    struct tagMEMVIRTSTR
                *lpLclMemVirtStr;   // 04:  Ptr to local MemVirtStr
    UINT         uPtdMemVirtStart,  // 08:  Starting offset into lpLclMemVirtStr
                 uPtdMemVirtEnd;    // 0C:  Ending   ...
                                    // 10:  Length
} INIT_MFO, *LPINIT_MFO;


//***************************************************************************
//  End of File: execmfn.h
//***************************************************************************
