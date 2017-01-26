//***************************************************************************
//  NARS2000 -- Defined Variables
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

#if _WIN32_WINNT < 0x0600   // From BASETSD.H

#define MAXUINT64   ((UINT64)~((UINT64)0))
#define MAXINT64    ((INT64)(MAXUINT64 >> 1))
#define MININT64    ((INT64)~MAXINT64)

#endif      // _WIN32_WINNT < 0x0600

// Wide char __FILE__ and __LINE__ macros
#define WFILE       WIDEN(__FILE__)
#define WIDEN(x)    SMASH(L,x)
#define SMASH(x,y)  x##y

#define WLINE       TOWL1 (__LINE__)
#define TOWL1(p)    TOWL2(p)
#define TOWL2(p)    L#p

#define WFNLN       WFILE L" " WLINE


#ifdef DEBUG
  ////#define DEBUG_REFCNT
  #define    UTF16_REFCNT_NE1      UTF16_REPLACEMENTCHAR
  #define WS_UTF16_REFCNT_NE1   WS_UTF16_REPLACEMENTCHAR
#endif

// Default definitions
#define DEF_APLFONT_INTNAME L"SImPL Medium"     // The default font internal name for NARS2000
#define DEF_APLFONT_EXTNAME L"SImPL-Medium"     // ...              external ...
#define DEF_APLFONT_EXT     L".ttf"             // ...                       extension
#define DEF_APLFONT_FILE    DEF_APLFONT_EXTNAME DEF_APLFONT_EXT // ... name w/ext
#define DEF_TCFONTNAME      L"Code2000"         // Default TabCtrl font
#ifndef UNISCRIBE
  #define DEF_ASFONTNAME    L"Code2000"         // Default Alternate SM font
#endif
#define DEF_SMFONTNAME      DEF_APLFONT_INTNAME
#define DEF_FBFONTNAME      L"Fallback00-1F"
#define DEF_LWFONTNAME      DEF_APLFONT_INTNAME
#define DEF_PRFONTNAME      DEF_APLFONT_INTNAME
#define DEF_CCFONTNAME      DEF_APLFONT_INTNAME
#define DEF_FEFONTNAME      DEF_APLFONT_INTNAME
#define DEF_MEFONTNAME      DEF_APLFONT_INTNAME
#define DEF_VEFONTNAME      DEF_APLFONT_INTNAME

#define DEF_TCLOGFONT       0,0,0,0,FW_BOLD  ,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,VARIABLE_PITCH | FF_ROMAN ,DEF_TCFONTNAME
#define DEF_SMLOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_SMFONTNAME
#define DEF_FBLOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_FBFONTNAME
#define DEF_LWLOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_LWFONTNAME
#define DEF_PRLOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_PRFONTNAME
#define DEF_CCLOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_CCFONTNAME
#define DEF_FELOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_FEFONTNAME
#define DEF_MELOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_MEFONTNAME
#define DEF_VELOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_VEFONTNAME

#define DEF_KBFONTNAME      DEF_APLFONT_INTNAME
#define DEF_KBLOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_KBFONTNAME

#define DEF_TCPTSIZE        9           // Point size for TC font
#define DEF_SMPTSIZE       13           // ...            SM ...
#define DEF_KBPTSIZE       13           // ...            KB ...
#define DEF_FBPTSIZE       13           // ...            FB ...
#define DEF_LWPTSIZE       13           // ...            LW ...
#define DEF_PRPTSIZE       13           // ...            PR ...
#define DEF_CCPTSIZE       13           // ...            CC ...
#define DEF_FEPTSIZE       13           // ...            FE ...
#define DEF_MEPTSIZE       13           // ...            ME ...
#define DEF_VEPTSIZE       13           // ...            VE ...

#define SampleText      L"(" WS_UTF16_IOTA L"V)" WS_UTF16_EQUALUNDERBAR WS_UTF16_RIGHTSHOE WS_UTF16_JOT L".,/" WS_UTF16_IOTA WS_UTF16_DIERESIS L"V"

#define DEF_INDENT          6           // Prompt indent
#define DEF_TABS            4           // Tab insertion
#define DEF_RECENTFILES    15           // # entries in Recent Files
#define DEF_RATSEP         'r'          // Default rational # separator
#define DEF_RATSEP_WS     L"r"          // ...
#define DEF_VFPSEP         'v'          // ...     VFP        ...
#define DEF_VFPSEP_WS     L"v"          // ...

// Size of storage areas
#define DEF_NUM_INITNELM        (     1024)                 // Initial NELM for lpszNum
#define DEF_NUM_INCRNELM        (     1024)                 // Incremental ...
////ine DEF_NUM_MAXNELM         (   4*1024)                 // Maximum ...
#define DEF_STR_INITNELM        (     1024)                 // Initial NELM for lpwszStr
#define DEF_STR_INCRNELM        (     1024)                 // Incremental ...
////ine DEF_STR_MAXNELM         ( 128*1024)                 // Maximum ...
////ine DEF_TOKENSTACK_INITNELM (   4*1024)                 // Initial NELM of token stack
////ine DEF_TOKENSTACK_INCRNELM (   4*1024)                 // Incremental ...
////ine DEF_TOKENSTACK_MAXNELM  (  64*1024)                 // Maximum ...
#define DEF_SIS_INITNELM        (   1*1024)                 // Initial NELM for State Indicator Stack
#define DEF_SIS_INCRNELM        (   1*1024)                 // Incremental ..
#define DEF_SIS_MAXNELM         (   4*1024)                 // Maximum ...
#define DEF_YYRES_INITNELM      (   1*1024)                 // Initial NELM of YYRes buffer
#define DEF_YYRES_INCRNELM      (   1*1024)                 // Incremental ...
#define DEF_YYRES_MAXNELM       (  16*1024)                 // Maximum ...
#define DEF_WPTDTEMP_INITNELM   (   4*1024)                 // Initial NELM of WCHAR PTD ...
#define DEF_WPTDTEMP_INCRNELM   (   1*1024)                 // Incremental ...
#define DEF_WGLBTEMP_INITNELM   (   4*1024)                 // Initial NELM of WCHAR GLB ...
#define DEF_WGLBTEMP_INCRNELM   (   1*1024)                 // Incremental ...
#define DEF_WGLBTEMP_MAXNELM    (  16*1024)                 // Maximum ...
#define DEF_DEBUG_INITNELM      (   1*1024)                 // Initial NELM of debug ...
#define DEF_DEBUG_INCRNELM      (   1*1024)                 // Incremental ...
#define DEF_DEBUG_MAXNELM       (  16*1024)                 // Maximum ...
#define DEF_WFORMAT_INITNELM    (   4*1024)                 // Initial NELM of WCHAR Formatting storage
#define DEF_WFORMAT_INCRNELM    (   4*1024)                 // Incremental ...
#define DEF_UNDOBUF_INITNELM    (   4*1024)                 // Initial NELM of Undo buffer
#define DEF_UNDOBUF_INCRNELM    (   4*1024)                 // Incremental ...
#define DEF_UNDOBUF_MAXNELM     ( 128*1024)                 // Maximum ...
#define DEF_QUADERROR_INITNELM  (   1*1024)                 // Initial NELM of []ERROR/[]ES buffer
#define DEF_QUADERROR_INCRNELM  (   1*1024)                 // Incremental ...
#define DEF_QUADERROR_MAXNELM   (  16*1024)                 // Maximum ...
#define DEF_STRAND_INITNELM     (   1*1024)                 // Initial NELM in tokens of the strand stack
#define DEF_STRAND_INCRNELM     (   1*1024)                 // Incremental ...
#define DEF_STRAND_MAXNELM      (   4*1024)                 // Maximum ...
#define DEF_DISPLAY_INITNELM    (   4*1024)                 // Initial NELM of WCHARs for Array Display
#define DEF_DISPLAY_INCRNELM    (   4*1024)                 // Incremental ...
#ifdef _WIN64
  #define DEF_WPTDTEMP_MAXNELM (100*1024*1024)              // Maximum ...
  #define DEF_WFORMAT_MAXNELM  (100*1024*1024)              // Maximum ...
  #define DEF_DISPLAY_MAXNELM  (100*1024*1024)              // Maximum ...
#elif defined (_WIN32)
  #define DEF_WPTDTEMP_MAXNELM  (2048*1024)                 // Maximum ...
  #define DEF_WFORMAT_MAXNELM   (4096*1024)                 // Maximum ...
  #define DEF_DISPLAY_MAXNELM   ( 256*1024)                 // Maximum ...
#else
  #error Need code for this architecture.
#endif
#define DEF_GLBHSHTAB_NBLKS            128                  // Starting # blocks
#define DEF_GLBHSHTAB_EPB                2                  // # entries in each block
#define DEF_GLBHSHTAB_INITNELM  (DEF_GLBHSHTAB_NBLKS * DEF_GLBHSHTAB_EPB)   // Initial NELM of global HshTab for {symbol} names & values
#define DEF_GLBHSHTAB_INCRNELM  (DEF_GLBHSHTAB_INITNELM)    // Incremental ...
#define DEF_GLBHSHTAB_MAXNELM   (1024 * DEF_GLBHSHTAB_EPB)  // Maximum ...
#define DEF_GLBHSHTAB_HASHMASK  (DEF_GLBHSHTAB_NBLKS - 1)   // Starting hash mask
#define DEF_GLBHSHTAB_INCRFREE  (DEF_HSHTAB_PRIME % DEF_GLBHSHTAB_INITNELM)
#define DEF_CS_INITNELM         (     4*1024)               // Initial NELM of Ctrl Struc token buffer
#define DEF_CS_INCRNELM         (     4*1024)               // Incremental ...
#define DEF_CS_MAXNELM          (    64*1024)               // Maximum ...
#define DEF_FORSTMT_INITNELM    (     1*1024)               // Initial NELM of FORSTMT stack
#define DEF_FORSTMT_INCRNELM    (     1*1024)               // Incremental ...
#define DEF_FORSTMT_MAXNELM     (     1*1024)               // Maximum ...
#define DEF_LFTSTK_INITNELM     (     4*1024)               // Initial NELM of 2by2 left stack
#define DEF_LFTSTK_INCRNELM     (     4*1024)               // incremental ...
#define DEF_LFTSTK_MAXNELM      (   128*1024)               // Maximum ...
#define DEF_RHTSTK_INITNELM     DEF_LFTSTK_INITNELM         // Initial NELM of 2by2 left stack
#define DEF_RHTSTK_INCRNELM     DEF_LFTSTK_INCRNELM         // incremental ...
#define DEF_RHTSTK_MAXNELM      DEF_LFTSTK_MAXNELM          // Maximum ...


// Global Options for User Preferences
#define DEF_ADJUSTPW                TRUE
#define DEF_UNDERBARTOLOWERCASE     FALSE
#define DEF_NEWTABONCLEAR           TRUE
#define DEF_NEWTABONLOAD            TRUE
#define DEF_USELOCALTIME            TRUE
#define DEF_BACKUPONLOAD            TRUE
#define DEF_BACKUPONSAVE            TRUE
#define DEF_NOCOPYRIGHTMSG          FALSE
#define DEF_DEFAULTPASTE            UNITRANS_NORMAL
#define DEF_DEFAULTCOPY             UNITRANS_NARS
#define DEF_SYNTCLRFCNS             TRUE
#define DEF_SYNTCLRSESS             TRUE
#define DEF_SYNTCLRPRNT             FALSE
#define DEF_CHECKGROUP              TRUE
#define DEF_INSSTATE                TRUE
#define DEF_REVDBLCLK               FALSE
#define DEF_VIEWSTATUSBAR           TRUE
#define DEF_DISPFCNLINENUMS         TRUE
#define DEF_DISPMPSUF               FALSE
#define DEF_OUTPUTDEBUG             FALSE


// Range limits for []vars
#define DEF_RANGELIMIT_CT           TRUE
#define DEF_RANGELIMIT_FEATURE      TRUE
#define DEF_RANGELIMIT_FPC          TRUE
#define DEF_RANGELIMIT_IC           TRUE
#define DEF_RANGELIMIT_IO           TRUE
#define DEF_RANGELIMIT_PP           TRUE
#define DEF_RANGELIMIT_PW           TRUE
#define DEF_RANGELIMIT_RL           TRUE


// Empty assignment to []vars as Fixed System value (TRUE) or .ini file value (FALSE)
#define DEF_RESETVARS_CT            FALSE
#define DEF_RESETVARS_DT            FALSE
#define DEF_RESETVARS_FC            FALSE
#define DEF_RESETVARS_FEATURE       FALSE
#define DEF_RESETVARS_FPC           FALSE
#define DEF_RESETVARS_IC            FALSE
#define DEF_RESETVARS_IO            FALSE
#define DEF_RESETVARS_PP            FALSE
#define DEF_RESETVARS_PW            FALSE
#define DEF_RESETVARS_RL            FALSE


// Date/time formats
#define DATETIME_OUT        L"dd/mm/yyyy hh:mm:ss"
#define DATETIME_FMT        L"%02d/%02d/%04d %2d:%02d:%02d"
#define DATETIME_LEN        strcountof (DATETIME_OUT)


#ifdef DEBUG
  #define    APPEND_DEBUG  " (DEBUG)"
  #define WS_APPEND_DEBUG L" (DEBUG)"
#else
  #define    APPEND_DEBUG
  #define WS_APPEND_DEBUG
#endif

#define    APPNAME      "NARS2000"
#define WS_APPNAME     L"NARS2000"

#define WS_WKSNAME  L"workspaces"       // Name of Workspaces subdirectory under main dir

#define LVER_LEGALCOPYRIGHT_YEARS  L"2006-2013"
#define LVER_COMPANYNAME_STR       L"Sudley Place Software"
#define LVER_COPYRIGHT1_STR        L"Copyright " WS_UTF16_COPYRIGHT L" " LVER_LEGALCOPYRIGHT_YEARS L" " LVER_COMPANYNAME_STR

#define VER_LEGALCOPYRIGHT_YEARS    "2006-2013"
#define VER_COMPANYNAME_STR         "Sudley Place Software"
#define VER_COPYRIGHT1_STR          "Copyright "    UTF16_COPYRIGHT  " "  VER_LEGALCOPYRIGHT_YEARS  " "  VER_COMPANYNAME_STR

#define PROP_STATUSMSG             L"StatusMsg"


//***************************************************************************
//  Floating point constants
//***************************************************************************

#define POS_INFINITY            (0x7FF0000000000000)
#define NEG_INFINITY            (0xFFF0000000000000)
#define QUIET_NAN               (0xFFF8000000000000)
#define FLOAT2POW53             (0x4340000000000000)
#define FLOATPI                 (0x400921FB54442D18)
#define FLOATGAMMA              (0x3FE2788CFC6FB619)
#define FLOATE                  (0x4005BF0A8B145769)


//***************************************************************************
//  Window Classes
//***************************************************************************

#define  MFWNDCLASS     "MFClass"       // Master Frame Window class
#define LMFWNDCLASS    L"MFClass"       // ...
#define  SMWNDCLASS     "SMClass"       // Session Manager ...
#define LSMWNDCLASS    L"SMClass"       // ...
#ifdef DEBUG
  #define  DBWNDCLASS   "DBClass"       // Debugger     ...
  #define LDBWNDCLASS  L"DBClass"       // ...
#endif
#ifdef PERFMONON
  #define  PMWNDCLASS   "PMClass"       // Performance Monitoring ...
  #define LPMWNDCLASS  L"PMClass"       // ...
#endif
#define  FEWNDCLASS     "FEClass"       // Function Editor ...
#define LFEWNDCLASS    L"FEClass"       // ...
#define  ECWNDCLASS     "ECClass"       // Edit Ctrl ...
#define LECWNDCLASS    L"ECClass"       // ...
#define  CCWNDCLASS     "CCClass"       // Crash Control ...
#define LCCWNDCLASS    L"CCClass"       // ...
#define  PBWNDCLASS     "PBClass"       // Progress Bar Window class
#define LPBWNDCLASS    L"PBClass"       // ...
#define  FW_RBWNDCLASS  "FW_RBClass"    // Font Window in Rebar Ctrl ...
#define LFW_RBWNDCLASS L"FW_RBClass"    // ...
#define  LW_RBWNDCLASS  "LW_RBClass"    // Language Window in Rebar Ctrl ...
#define LLW_RBWNDCLASS L"LW_RBClass"    // ...

#define LCL_WC_STATUSBARA    "LclStatusBar"
#define LCL_WC_STATUSBARW   L"LclStatusBar"

#define CQUADPR_MT      L'\xFFFF'       // cQuadPR value indicating it's empty


//***************************************************************************
//  Threads
//***************************************************************************

#define TLSTYPE_DB      ((LPVOID) 'DB')     // Debugger
#define TLSTYPE_IE      ((LPVOID) 'IE')     // Immediate Execution
#define TLSTYPE_MF      ((LPVOID) 'MF')     // Master Frame
#define TLSTYPE_PL      ((LPVOID) 'PL')     // ParseLine
#define TLSTYPE_TC      ((LPVOID) 'TC')     // Tab Control


//***************************************************************************
//  Miscellaneous constants
//***************************************************************************

#ifdef _WIN64
  #define WS_WINSTR     L"Win64"
#elif defined (_WIN32)
  #define WS_WINSTR     L"Win32"
#else
  #error Need code for this architecture.
#endif

#define EQ ==
#define NE !=

#define TRUE    1
#define FALSE   0
#define NEG1U   (   (UINT) -1)
#define NEG1A   ((APLUINT) -1)

#define LOPART_DWORDLONG    ((DWORDLONG) 0x00000000FFFFFFFF)
#define HIPART_DWORDLONG    ((DWORDLONG) 0xFFFFFFFF00000000)

#define LOPART_LONGLONG     ((LONGLONG)  0x00000000FFFFFFFF)
#define HIPART_LONGLONG     ((LONGLONG)  0xFFFFFFFF00000000)

#define QWORD   ULONGLONG

#define PAGESIZE        (4 * 1024)

#define LIBCMD_SWITCH   L'?'

#ifdef _WIN64
#define WS_BITSIZE      L".64"
#else
#define WS_BITSIZE      L".32"
#endif


//***************************************************************************
//  Debugging
//***************************************************************************

#define DbgBrk          __debugbreak
#define DbgStop         __debugbreak

#define defstop \
default:        \
    DbgStop();

#define FNLN    FileNameOnly (__FILE__), __LINE__

#ifdef DEBUG
  #define LCLODS        ODS
  #define LCLODSDD      ODSDD
  #define LCLODSRECT    ODSRECT
  #define LCLODSSIZE    ODSSIZE
  #define LCLODSAPI     ODSAPI

  #define DBGENTER
  #define DBGLEAVE

  #define APPEND_NAME_ARG , APPEND_NAME
#else
  #define LCLODS
  #define LCLODSDD
  #define LCLODSRECT
  #define LCLODSSIZE
  #define LCLODSAPI

  #define DBGENTER
  #define DBGLEAVE

  #define APPEND_NAME_ARG
  #define oprintfW
#endif


//***************************************************************************
//  Window extra byte offsets
//***************************************************************************

// Define offsets in DBWNDCLASS window extra bytes
#define GWLDB_HWNDLB    0                                           // Window handle of Listbox
#define GWLDB_EXTRA     GWLDB_HWNDLB   + 1 * sizeof (HANDLE_PTR)    // Total # extra bytes

// Define common offset between the Session Manager and Function Editor
#define GWLSF_PERTAB     0                                           // Ptr to PerTabData global memory
#define GWLSF_HWNDEC     GWLSF_PERTAB     + 1 * sizeof (HANDLE_PTR)  // ...   Edit Control window
#define GWLSF_HGLBDFNHDR GWLSF_HWNDEC     + 1 * sizeof (HANDLE_PTR)  // ...   pre-existing function global memory handle (may be NULL)
#define GWLSF_UNDO_BEG   GWLSF_HGLBDFNHDR + 1 * sizeof (HANDLE_PTR)  // ...                beginning
#define GWLSF_UNDO_NXT   GWLSF_UNDO_BEG   + 1 * sizeof (HANDLE_PTR)  // ...                next
#define GWLSF_UNDO_LST   GWLSF_UNDO_NXT   + 1 * sizeof (HANDLE_PTR)  // ...                last
#define GWLSF_UNDO_GRP   GWLSF_UNDO_LST   + 1 * sizeof (HANDLE_PTR)  // Value of next Undo group index
#define GWLSF_LASTKEY    GWLSF_UNDO_GRP   + 1 * sizeof (long)        // Value of last WM_KEYDOWN key
#define GWLSF_CHANGED    GWLSF_LASTKEY    + 1 * sizeof (long)        // Boolean of whether or not the text has changed
#define GWLSF_FLN        GWLSF_CHANGED    + 1 * sizeof (long)        // Boolean of whether or not function line #s are to be displayed
#define GWLSF_LPMVS      GWLSF_FLN        + 1 * sizeof (long)        // Ptr to LPMEMVIRTSTR

// Define offsets in SMWNDCLASS window extra bytes
#define GWLSM_EXTRA     GWLSF_LPMVS    + 1 * sizeof (HANDLE_PTR)    // Total # extra bytes

// Define offsets in FEWNDCLASS window extra bytes
#define GWLFE_HWNDPRV   GWLSF_LPMVS    + 1 * sizeof (HANDLE_PTR)    // Next window handle in linked list (NULL = none)
#define GWLFE_HWNDNXT   GWLFE_HWNDPRV  + 1 * sizeof (HANDLE_PTR)    // Previous ...
#define GWLFE_EXTRA     GWLFE_HWNDNXT  + 1 * sizeof (HANDLE_PTR)    // Total # extra bytes

// Define offsets in PMWNDCLASS window extra bytes
#define GWLPM_HWNDLB    0                                           // Window handle of Listbox
#define GWLPM_EXTRA     GWLPM_HWNDLB   + 1 * sizeof (HANDLE_PTR)    // Total # extra bytes

// Define offsets in PBWNDCLASS window extra bytes
#define GWLPB_HGLB      0                                           // Window data global memory handle
#define GWLPB_EXTRA     GWLPB_HGLB     + 1 * sizeof (HANDLE_PTR)    // Total # extra bytes

// Define offsets in FW_RBWNDCLASS window extra bytes
#define GWLFW_RB_EXTRA  0                                           // Total # extra bytes

// Define offsets in LW_RBWNDCLASS window extra bytes
#define GWLLW_RB_EXTRA  0                                           // Total # extra bytes

// Define offsets in CCWNDCLASS window extra bytes
#define GWLCC_EXTRA     0                                           // Total # extra bytes

// Define offsets in WCWNDCLASS window extra bytes
#define GWLWC_EXTRA     0                                           // Total # extra bytes


//***************************************************************************
//  Local window messages
//***************************************************************************

// Define local window messages
#define MYWM_MOVE           (WM_APP + 0)    // MF
#define MYWM_SETFOCUS       (WM_APP + 1)    // SM (SetFocus)
#define MYWM_IZITNAME       (WM_APP + 2)    // FE (Izit A Name)
#define MYWM_SAVE_FN        (WM_APP + 3)    // FE (SaveFunction)
#define MYWM_SAVECLOSE_FN   (WM_APP + 4)    // FE (SaveFunction/CloseFunction)
#define MYWM_SAVE_AS_FN     (WM_APP + 5)    // FE (SaveAsFunction)
#define MYWM_CLOSE_FN       (WM_APP + 6)    // FE (CloseFunction)
#define MYWM_QUOTEQUAD      (WM_APP + 7)    // PL (Quote-Quad/Quad Input)
#define MYWM_INIT_SMDB      (WM_APP + 8)    // SM (Initialize SM/DB windows)
#define MYWM_ERRMSG         (WM_APP + 9)    // SM (Display error message)
#define MYWM_SAVE_WS        (WM_APP +10)    // SM (Save workspace)
#define MYWM_DISPMB         (WM_APP +11)    // CC (Display MessageBox)
#define MYWM_RESIZE         (WM_APP +12)    // MF (Resize to display Status Bar)
#define MYWM_NOTIFY         (WM_APP +13)    // EC (Pass on of WM_NOTIFY from EC)
#define MYWM_CMPNAME        (WM_APP +14)    // FE (Compare function names)
#define MYWM_RBUTTONDBLCLK  (WM_APP +15)    // FE/SM (Right-double-click)
#define MYWM_NEWFONT        (WM_APP +16)    // WS_FW (Set a new font and point size)
#define MYWM_GETCLIENTSIZE  (WM_APP +17)    // LW_FW (Get size of client area before the window is created)
#define MYWM_CLEARCHAR      (WM_APP +18)    // LW_FW (Clear highlight around any previously outlined char)
#define MYWM_CREATEFCN      (WM_APP +19)    // SM (Create a new function)
#define MYWM_LPMEMPTD       (WM_APP +20)    // MF (Pass lpMemPTD for TLS)
#define MYWM_PROMPT         (WM_APP +21)    // MF (Display the prompt)
#define MYWM_INIT_EC        (WM_APP +22)    // EC (Initialize local Edit Ctrl)
#define MYWM_DNL_SUCCESS    (WM_APP +23)    // DL (TRUE iff the download succeeded)
#define MYWM_MOUSELEAVE     (WM_APP+ 24)    // RB (Local version of WM_MOUSELEAVE)

// Define Debug window messages
#define MYWM_INIT_DB        (WM_APP +50)    // DB
#define MYWM_DBGMSGA        (WM_APP +51)    // DB
#define MYWM_DBGMSGW        (WM_APP +52)    // DB
#define MYWM_DBGMSG_CLR     (WM_APP +53)    // DB
#define MYWM_DBGMSG_SCROLL  (WM_APP +54)    // DB
#define MYWM_UNHOOK         (WM_APP +55)    // DB


//***************************************************************************
//  Keyboard State Bits
//***************************************************************************

#define KS_SHIFT        4
#define KS_CTRL         2
#define KS_ALT          1
#define KS_NONE         0


//***************************************************************************
//  Character constants -- ASCII and Wide characters and strings
//***************************************************************************

#define AC_EOS          '\0'        // 00:  End-of-string
#define AC_BS           '\b'        // 08:  Backspace
#define AC_HT           '\t'        // 09:  Horizontal Tab
#define AC_LF           '\n'        // 0A:  Linefeed
#define AC_FF           '\f'        // 0C:  FormFeed
#define AC_CR           '\r'        // 0D:  Carriage Return
#define AC_BLANK        ' '         // 20:  Space
#define AC_LEFTPAREN    '('         // 28:  Left Paren
#define AC_STAR         '*'         // 2A:  Asterisk
#define AC_X            'X'         // 58:  Capital X
#define AC_SLOPE        '\\'        // 5C:  Slope
#define AC_CRLF         '\r\n'      // 0D0A:  Carriage Return Linefeed

#define AS_LF           "\n"        // 0A:  Linefeed
#define AS_CR           "\r"        // 0D:  Carriage Return
#define AS_CRLF         "\r\n"      // 0D0A:  Carriage Return Linefeed
#define EOL_LEN         2           // Length of EOL ("\r\n")

#define WC_EOS          L'\0'       // 00:  End-of-string
#define WC_BEL          L'\x0007'   // 07:  Bell
#define WC_BS           L'\b'       // 08:  Backspace
#define WC_HT           L'\t'       // 09:  Horizontal Tab
#define WC_LF           L'\n'       // 0A:  Linefeed
#define WC_FF           L'\f'       // 0C:  FormFeed
#define WC_CR           L'\r'       // 0D:  Carriage Return
#define WC_ESC          L'\x001B'   // 1B:  Escape
#define WC_DQ           L'\"'       // 22:  Double Quote
#define WC_SQ           L'\''       // 27:  Single Quote
#define WC_SLOPE        L'\\'       // 5C:  Slope
#define WC_LC           L'\x27A5'   // 27A5:  Line Continuation marker

#define WS_BS           L"\b"       // 08:  Backspace
#define WS_HT           L"\t"       // 09:  Horizontal Tab
#define WS_LF           L"\n"       // 0A:  Linefeed
#define WS_FF           L"\f"       // 0C:  FormFeed
#define WS_CR           L"\r"       // 0D:  Carriage Return
#define WS_DQ           L"\""       // 22:  Double Quote
#define WS_SQ           L"\'"       // 27:  Single Quote
#define WS_SLOPE        L"\\"       // 5C:  Slope
#define WS_LC           L"\x27A5"   // 27A5:  Line Continuation marker

#define WS_CRLF         L"\r\n"     // 0D0A:    CR/LF    (hard line-break)
#define WS_CRCRLF       L"\r\r\n"   // 0D0D0A:  CR/CR/LF (soft line-break)

#define DEF_UNDERFLOW       L'_'    // Default underflow char
#define DEF_UNDERFLOW_WS    L"_"    // ...
#define DEF_EXPONENT_UC     L'E'    // ...     exponent  ...
#define DEF_EXPONENT_LC     L'e'    // ...
#define DEF_EXPONENT_UC_WS  L"E"    // ...
#define DEF_EXPONENT_LC_WS  L"e"    // ...

// Define text for infinity
#define  TEXT_INFINITY       "{infinity}"
#define LTEXT_INFINITY      L"{infinity}"

#define DEF_POSINFINITY_CHAR     '!'
#define DEF_POSINFINITY_STR      "!"
#define DEF_NEGINFINITY_STR     "-!"

// Use when translating to non-WCHAR code in <pn_parse.y>
#define INFINITY1       DEF_POSINFINITY_CHAR
#define INFINITY1_STR   DEF_POSINFINITY_STR
#define OVERBAR1        '-'
#define OVERBAR1_STR    "-"


//***************************************************************************
// Wide-char routines From <string.h>
//***************************************************************************

#define strchrW         wcschr
#define strncmpW        wcsncmp
#define strncmpi        _strnicmp
#define strncmpiW       _wcsnicmp
#define strpbrkW        wcspbrk
#define strspnW         wcsspn
#define strcspnW        wcscspn
#define strtolW         wcstol
#define isspaceW        iswspace
#define atofW           _wtof
#define sscanfW         swscanf
#define fgetsW          fgetws
#define fopenW          _wfopen
#define strlwrW         _wcslwr
#define strdupW         _wcsdup
#define tolowerW        towlower
#define IsDigit         isdigit
#define IsDigitW        iswdigit


//***************************************************************************
//  Bit, byte, etc. constants
//***************************************************************************

// Define bit masks
#define BIT0    0x0001
#define BIT1    0x0002
#define BIT2    0x0004
#define BIT3    0x0008
#define BIT4    0x0010
#define BIT5    0x0020
#define BIT6    0x0040
#define BIT7    0x0080
#define BIT8    0x0100
#define BIT9    0x0200
#define BIT10   0x0400
#define BIT11   0x0800
#define BIT12   0x1000
#define BIT13   0x2000
#define BIT14   0x4000
#define BIT15   0x8000

// # bits in a byte
#define NBIB            8

// Log base 2 of NBIB
#define LOG2NBIB        3

// Mask for LOG2NBIB bits
#define MASKLOG2NBIB    ((BIT0 << LOG2NBIB) - 1)    // a.k.a. (NBIB - 1)

// # bits in a word
#define NBIW           16

// Log base 2 of NBIW
#define LOG2NBIW        4

// Mask for LOG2NBIW bits
#define MASKLOG2NBIW    ((BIT0 << LOG2NBIW) - 1)    // a.k.a. (NBIW - 1)

// # bits in a dword
#define NBID           32

// Log base 2 of NBID
#define LOG2NBID        5

// Mask for LOG2NBID bits
#define MASKLOG2NBID    ((BIT0 << LOG2NBID) - 1)    // a.k.a. (NBID - 1)

// # bits in a qword
#define NBIQ           64

// Log base 2 of NBIQ
#define LOG2NBIQ        6

// Mask for LOG2NBIQ bits
#define MASKLOG2NBIQ    ((BIT0 << LOG2NBIQ) - 1)    // a.k.a. (NBIQ - 1)

// End value for shift mask
#define END_OF_BYTE     (BIT0 << NBIB)


//***************************************************************************
//  WM_PRINTCLIENT flags
//***************************************************************************

// Extra flags for WM_PRINTCLIENT to indicate ...
#define PRF_PRINTCLIENT 0x80000000L     // called from WM_PRINTCLIENT
#define PRF_SELECTION   0x40000000L     // to print the selection
#define PRF_CURRENTPAGE 0x20000000L     // to print the current page


//***************************************************************************
// Resource debugging size
//***************************************************************************

#define MAXOBJ  256000


//***************************************************************************
//  Window Properties
//***************************************************************************

#define PROP_IDMPOSFN   L"PROP_IDMPOSFN"
#define PROP_NTHREADS   L"PROP_NTHREADS"

//***************************************************************************
//  RAT & VFP
//***************************************************************************

// Define the following symbol iff comparison of two RATs is meant to be exact
//   as opposed to sensitive to []CT.
////#define RAT_EXACT

//***************************************************************************
//  Timers
//***************************************************************************

#define ID_TIMER_UPDCHK        1
#define ID_TIMER_EDIT       1729
#define ID_TIMER_EXEC       1730

//***************************************************************************
//  Progress Bar
//***************************************************************************

typedef struct tagCOLORBLEND *LPCOLORBLEND;     // Dummy entry for .pro files only

#if _WIN32_WINNT < 0x0600

#define PBM_GETSTEP             (WM_USER+13)
#define PBM_GETBKCOLOR          (WM_USER+14)
#define PBM_GETBARCOLOR         (WM_USER+15)
#define PBM_SETSTATE            (WM_USER+16) // wParam = PBST_[State] (NORMAL, ERROR, PAUSED)
#define PBM_GETSTATE            (WM_USER+17)

#define PBST_NORMAL             0x0001
#define PBST_ERROR              0x0002
#define PBST_PAUSED             0x0003
#endif      // _WIN32_WINNT < 0x0600

#define PBM_GETBUDDY            (WM_USER+18)
#define PBM_SETBUDDY            (WM_USER+19)


//***************************************************************************
//  End of File: defines.h
//***************************************************************************
