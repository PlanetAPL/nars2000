//***************************************************************************
//  NARS2000 -- Enums
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


enum tagMENUPOS_SM              // Positions of items in SM menu
{
    IDMPOS_SM_FILE = 0,         // 00:  File menu item
    IDMPOS_SM_EDIT    ,         // 01:  Edit ...
    IDMPOS_SM_VIEW    ,         // 02:  View ...
    IDMPOS_SM_WINDOW  ,         // 03:  Window ...
    IDMPOS_SM_HELP    ,         // 04:  Help ...
};

enum tagMENUPOS_FE              // Positions of items in FE menu
{
    IDMPOS_FE_FILE = 0,         // 00:  File menu item
    IDMPOS_FE_EDIT    ,         // 01:  Edit ...
    IDMPOS_FE_VIEW    ,         // 02:  View ...
    IDMPOS_FE_WINDOW  ,         // 03:  Window ...
    IDMPOS_FE_OBJECTS ,         // 04:  Objects ...
    IDMPOS_FE_HELP    ,         // 05:  Help ...
};

// N.B.:  Whenever changing either of the above tagMENUPOSxx enums,
//   be sure to make a corresponding change to
//   the one below so that it always has an entry for
//   each of the menu names across all such enums, and be sure to
//   change all <GetIDMPOS_xx> functions.

typedef enum tagALLMENUPOS      // All Menu Positions (the union of the above menu positions)
{
    IDMPOSNAME_FILE = 0,        // 00:  File menu name
    IDMPOSNAME_EDIT    ,        // 01:  Edit ...
    IDMPOSNAME_VIEW    ,        // 02:  View ...
    IDMPOSNAME_WINDOW  ,        // 03:  Window ...
    IDMPOSNAME_OBJECTS ,        // 04:  Objects ...
    IDMPOSNAME_HELP    ,        // 05:  Help ...
} ALLMENUPOS, *LPALLMENUPOS;

typedef UINT (*GETIDMPOS_XX) (ALLMENUPOS);


enum tagWINDOWIDS               // Window IDs
{
    IDWC_DB_LB = 1,             // 01:  Window ID for ListBox       in DB window
    IDWC_FE_EC    ,             // 02:  ...           EditCtrl      in FE ...
    IDWC_SM_EC    ,             // 03:  ...           EditCtrl      in SM ...
    IDWC_SM_DB    ,             // 04:  ...           DebugCtrl     in SM ...
    IDWC_TC_MC    ,             // 05:  ...           MDI Client    in TC ...
    IDWC_CC_LB    ,             // 06:  ...           ListBox       in CC ...
    IDWC_PM_LB    ,             // 07:  ...           ListBox       in PM ...
    IDWC_MF_ST    ,             // 08:  ...           Static Wnd    in MF ...
    IDWC_TC       ,             // 09:  ...                            TC ...
    IDWC_RB       ,             // 0A:  ...                            RB ...
    IDWC_WS_RB    ,             // 0B:  ...           WS Window     in RB control
    IDWC_ED_RB    ,             // 0C:  ...           ED ...        in RB control
    IDWC_OW_RB    ,             // 0D:  ...           OW ...        in RB control
    IDWC_FW_RB    ,             // 0E:  ...           FW ...        in RB control
    IDWC_CBFN_FW  ,             // 0F:  ...           CB Font Name  in FW window
    IDWC_CBFS_FW  ,             // 10:  ...           CB Font Style in ...
    IDWC_EC_FW    ,             // 11:  ...           EC Font Size  in ...
    IDWC_UD_FW    ,             // 12:  ...           UpDown Ctrl   in ...
    IDWC_LW_RB    ,             // 13:  ...           LW Window     in RB control
    IDWC_TB_LW    ,             // 14:  ...           Toolbar       in LW window
};


typedef enum tagWINDOWCLASS     // Window Classes
{
    WINDOWCLASS_MF = 0  ,       // 00:  Master Frame
    WINDOWCLASS_RB      ,       // 01:  Rebar
    WINDOWCLASS_SM      ,       // 02:  Session Manager
    WINDOWCLASS_FE      ,       // 03:  Function Editor
} WINDOWCLASS, *LPWINDOWCLASS;


typedef enum tagSYSCMDS_ENUM    // System Commands
{
    SYSCMD_None = 0 ,           // 00:  <invalid result>
    SYSCMD_CLEAR    ,           // 01:  )CLEAR
    SYSCMD_CLOSE    ,           // 02:  )CLOSE
    SYSCMD_COPY     ,           // 03:  )COPY
    SYSCMD_DROP     ,           // 04:  )DROP
    SYSCMD_EDIT     ,           // 05:  )EDIT
    SYSCMD_ERASE    ,           // 06:  )ERASE
    SYSCMD_EXIT     ,           // 07:  )EXIT
    SYSCMD_FNS      ,           // 08:  )FNS
    SYSCMD_FOPS     ,           // 09:  )FOPS
    SYSCMD_IN       ,           // 0A:  )IN
    SYSCMD_INASCII  ,           // 0B:  )INASCII
    SYSCMD_LIB      ,           // 0C:  )LIB
    SYSCMD_LOAD     ,           // 0D:  )LOAD
    SYSCMD_NEWTAB   ,           // 0E:  )NEWTAB
    SYSCMD_NMS      ,           // 0F:  )NMS
    SYSCMD_OPS      ,           // 10:  )OPS
    SYSCMD_OUT      ,           // 11:  )OUT
    SYSCMD_RESET    ,           // 12:  )RESET
    SYSCMD_SAVE     ,           // 13:  )SAVE
    SYSCMD_SI       ,           // 14:  )SI
    SYSCMD_SINL     ,           // 15:  )SINL
    SYSCMD_ULIB     ,           // 16:  )ULIB
    SYSCMD_VARS     ,           // 17:  )VARS
    SYSCMD_XLOAD    ,           // 18:  )XLOAD
    SYSCMD_WSID     ,           // 19:  )WSID
} SYSCMDS_ENUM, *LPSYSCMDS_ENUM;


typedef enum tagEXCEPTION_CODES // Exception Codes
{
    EXCEPTION_SUCCESS = 0 ,     // 00:  All OK
    EXCEPTION_RESULT_FLOAT ,    // 01:  Result should be Float
    EXCEPTION_RESULT_RAT   ,    // 02:  Result should be RAT
    EXCEPTION_RESULT_VFP   ,    // 03:  Result should be VFP
    EXCEPTION_DOMAIN_ERROR ,    // 04:  Signal a DOMAIN ERROR
    EXCEPTION_LIMIT_ERROR  ,    // 05:  Signal a LIMIT ERROR
    EXCEPTION_NONCE_ERROR  ,    // 06:  Signal a NONCE ERROR
    EXCEPTION_WS_FULL      ,    // 07:  Signal a WS FULL
    EXCEPTION_CTRL_BREAK   ,    // 08:  Ctrl-Break pressed
} EXCEPTION_CODES;

// N.B.:  Whenever changing the above tagEXCEPTION_CODES enum,
//   be sure to make a corresponding change to
//   <CheckException> in <except.c>,
//   <MyGetExceptionStr> in <except.c>


typedef enum tagMAKE_PROTO
{
    MP_CHARS,                   // 00:  Chars allowed in MakeMonPrototype arg
    MP_NUMONLY,                 // 01:  Numerics only ...
    MP_NUMCONV                  // 02:  Convert chars to numerics ...
} MAKE_PROTO;


typedef enum tagEXIT_TYPES
{
    EXITTYPE_NONE = 0,          // 00:  Undefined
    EXITTYPE_GOTO_ZILDE,        // 01:  {goto} {zilde}
    EXITTYPE_GOTO_LINE,         // 02:  {goto} LineNum
    EXITTYPE_RESET_ONE,         // 03:  {goto}
    EXITTYPE_RESET_ONE_INIT,    // 04:  {goto}  (first time)
    EXITTYPE_RESET_ALL,         // 05:  )RESET
    EXITTYPE_QUADERROR_INIT,    // 06:  []ERROR/[]ES -- initialization
    EXITTYPE_QUADERROR_EXEC,    // 07:  []ERROR/[]ES -- execute []ELX
    EXITTYPE_ERROR,             // 08:  ERROR
    EXITTYPE_STOP,              // 09:  Stop processing, cannot continue
    EXITTYPE_DISPLAY,           // 0A:  Value not already displayed
    EXITTYPE_NODISPLAY,         // 0B:  Value already displayed
    EXITTYPE_NOVALUE,           // 0C:  No value returned
    EXITTYPE_RETURNxLX,         // 0D:  Return from []ELX/[]ALX
    EXITTYPE_STACK_FULL,        // 0E:  ERROR -> STACK FULL
                                // 0F-0F:  Available entries (4 bits)
} EXIT_TYPES, *LPEXITTYPES;


typedef enum tagERROR_CODES     // Error codes
{
    ERRORCODE_NONE = 0,         // 00:  No error
    ERRORCODE_ALX     ,         // 01:  Signal []ALX
    ERRORCODE_ELX     ,         // 02:  Signal []ELX
    ERRORCODE_DM      ,         // 03:  Display []DM
} ERROR_CODES, *LPERROR_CODES;

// N.B.:  Whenever changing the above tagERROR_CODES enum,
//   be sure to make a corresponding change to
//   SWITCH stmts in <ParseLine> in <pl_parse.y>


typedef enum tagLINE_NUMS       // Starting line #s
{
    LINENUM_ONE = 0,            // 00:  Line #1
    LINENUM_ID ,                // 01:  Line []ID
    LINENUM_INV,                // 02:  Line []INV
    LINENUM_MS ,                // 03:  Line []MS
    LINENUM_PRO,                // 04:  Line []PRO
} LINE_NUMS, *LPLINE_NUMS;


typedef enum tagFLT_DISP_FMT    // Floating Point Display Formats
{
    FLTDISPFMT_E = 0,           // 00:  E-format to nDigits significant digits
    FLTDISPFMT_F,               // 01:  F-format with nDigits to the right of the decimal point
    FLTDISPFMT_RAWINT,          // 02:  Raw integer format to nDigits significant digits
    FLTDISPFMT_RAWFLT,          // 03:  Raw float format to nDigits significant digits -- switch
                                //      to E-format if more than []PP digits required
    FLTDISPFMT_LENGTH,          // 04:  # entries in this enum
} FLTDISPFMT, *LPFLTDISPFMT;

// N.B.:  Whenever changing the above enum (FLT_DISP_FMT),
//   be sure to make a corresponding change to
//   <gDTOA_Mode> in <display.c>.


typedef enum tagTIMER_SOURCE
{
    TIMERSOURCE_MS = 1,         // 01:  Milliseconds
    TIMERSOURCE_PC    ,         // 02:  Performance Counters
    TIMERSOURCE_PC2MS ,         // 03:  Performance Counters scaled to Milliseconds
} TIMER_SOURCE, *LPTIMER_SOURCE;


typedef enum tagTABSTATE        // Table State for Nth and Number of Primes
{
    TABSTATE_NTHPRIMETAB = 0,   // 0:  Use NthPrimeTab ...
    TABSTATE_NTHPOWERTAB    ,   // 1:  Use NthPowerTab table
    TABSTATE_LASTVALUE      ,   // 2:  Use last value (NTHPRIMESTR)
} TABSTATE, *LPTABSTATE;


typedef enum tagNUMTHEORY       // Number theory values (left arg to dyadic Pi)
{
    NUMTHEORY_ISPRIME   =  0  ,     // 00:  TRUE iff the given # is prime
    NUMTHEORY_NEXTPRIME =  1  ,     // 01:  Next prime after a given #
    NUMTHEORY_PREVPRIME = -1  ,     //-01:  Prev prime before a given #
    NUMTHEORY_NUMPRIMES =  2  ,     // 02:  # primes <= a given #
    NUMTHEORY_NTHPRIME  = -2  ,     //-02:  Nth prime

    NUMTHEORY_DIVCNT    = 10  ,     // 0A:  Divisor count function
    NUMTHEORY_DIVSUM    = 11  ,     // 0B:  Divisor sum function
    NUMTHEORY_MOBIUS    = 12  ,     // 0C:  Mobius function
    NUMTHEORY_TOTIENT   = 13  ,     // 0D:  Totient function

    NUMTHEORY_FACTOR    = 20  ,     // 14:  Factor function (the # is arbitrary and may be changed)
} NUMTHEORY, *LPNUMTHEORY;

#define NUMTHEORY_MIN       -2      // Minimum valid index
#define NUMTHEORY_MAX       13      // Maximum ...


typedef enum tagNFNS_NERR       // Common code for []NERASE, []NRENAME, and []NRESIZE
{
    COM_NERASE = 0,             // 0:  []NERASE
    COM_NRENAME,                // 1:  []NRENAME
    COM_NRESIZE                 // 2:  []NRESIZE
} NFNS_NERR;


typedef enum tagDFN_TYPES               // User-Defined Function/Operator Types
{
    DFNTYPE_UNK = 0,                    // 00:  Unknown
    DFNTYPE_OP1,                        // 01:  Monadic operator
    DFNTYPE_OP2,                        // 02:  Dyadic operator
    DFNTYPE_FCN,                        // 03:  Niladic/monadic/dyadic/ambivalent function
    DFNTYPE_IMM,                        // 04:  Immediate execution
    DFNTYPE_EXEC,                       // 05:  Execute primitive
    DFNTYPE_QUAD,                       // 06:  Quad input
    DFNTYPE_QQUAD,                      // 07:  Quote-Quad input
    DFNTYPE_ERRCTRL,                    // 08:  Error control via Quad-EA/-EC
                                        // 09-0F:  Available entries (4 bits)
} DFN_TYPES;

#ifdef DEBUG
  // N.B.:  Whenever changing the above enum
  //   be sure to make a corresponding change to
  //   <cDfnTypeStr> below.

  #define cDfnTypeStr     L"?12FIE!@%"
#endif


typedef enum tagPTDMEMVIRTENUM
{
    PTDMEMVIRT_QUADERROR = 0,           // 00:  lpszQuadErrorMsg
    PTDMEMVIRT_UNDOBEG,                 // 01:  lpUndoBeg
    PTDMEMVIRT_HTSPTD,                  // 02:  lphtsPTD
    PTDMEMVIRT_HSHTAB,                  // 03:  lphtsPTD->lpHshTab
    PTDMEMVIRT_SYMTAB,                  // 04:  lphtsPTD->lpSymTab
    PTDMEMVIRT_SIS,                     // 05:  lpSISBeg
    PTDMEMVIRT_CS,                      // 06:  lptkCSIni
    PTDMEMVIRT_YYRES,                   // 07:  lpYYRes
    PTDMEMVIRT_STRAND_VAR,              // 08:  lpStrand[STRAND_VAR]
    PTDMEMVIRT_STRAND_FCN,              // 09:  lpStrand[STRAND_FCN]
    PTDMEMVIRT_STRAND_LST,              // 0A:  lpStrand[STRAND_LST]
    PTDMEMVIRT_STRAND_NAM,              // 0B:  lpStrand[STRAND_NAM]
    PTDMEMVIRT_WSZFORMAT,               // 0C:  Temporary formatting
    PTDMEMVIRT_WSZTEMP,                 // 0D:  Temporary save area
    PTDMEMVIRT_FORSTMT,                 // 0E:  FOR ... IN stmts
    PTDMEMVIRT_MFO1,                    // 0F:  Magic functions/operators
    PTDMEMVIRT_MFO2,                    // 10:  ...
    PTDMEMVIRT_LFTSTK,                  // 11:  2by2 left stack
    PTDMEMVIRT_RHTSTK,                  // 12:  ...  right ...
    PTDMEMVIRT_LENGTH                   // 13:  # entries
} PTDMEMVIRTENUM;


enum tagMP_ENUM         // Multi-precision mpX_invalid arguments
{
    MP_ADD              ,   // 00:  rop = op1 + op2
    MP_SUB              ,   // 01:  rop = op1 - op2
    MP_MUL              ,   // 02:  rop = op1 * op2
    MP_CDIV_Q           ,   // 03:  rop = quotient  (op1 / op2)
    MP_CDIV_R           ,   // 04:  rop = remainder (op1 / op2)
    MP_CDIV_QR          ,   // 05:  rop = quotient  (op1 / op2), op3 = remainder (op1/op2)
    MP_CDIV_R_UI        ,   // 06:  rop = remainder (op1 / op2)
    MP_CDIV_QR_UI       ,   // 07:  rop = quotient  (op1 / op2), op3 = remainder (op1/op2)
    MP_CDIV_R_2EXP      ,   // 08:  rop = remainder (op1 / op2)
    MP_FDIV_Q           ,   // 09:  rop = quotient  (op1 / op2)
    MP_FDIV_R           ,   // 0A:  rop = remainder (op1 / op2)
    MP_FDIV_QR          ,   // 0B:  rop = quotient  (op1 / op2), op3 = remainder (op1/op2)
    MP_FDIV_R_UI        ,   // 0C:  rop = remainder (op1 / op2)
    MP_FDIV_QR_UI       ,   // 0D:  rop = quotient  (op1 / op2), op3 = remainder (op1/op2)
    MP_FDIV_R_2EXP      ,   // 0E:  rop = remainder (op1 / op2)
    MP_TDIV_Q           ,   // 0F:  rop = quotient  (op1 / op2)
    MP_TDIV_R           ,   // 10:  rop = remainder (op1 / op2)
    MP_TDIV_QR          ,   // 11:  rop = quotient  (op1 / op2), op3 = remainder (op1/op2)
    MP_TDIV_R_UI        ,   // 12:  rop = remainder (op1 / op2)
    MP_TDIV_QR_UI       ,   // 13:  rop = quotient  (op1 / op2), op3 = remainder (op1/op2)
    MP_TDIV_R_2EXP      ,   // 14:  rop = remainder (op1 / op2)
    MP_DIV              ,   // 15:  rop = op1 / op2
    MP_SQRT             ,   // 16:  rop = sqrt (op)
    MP_RELDIFF          ,   // 17:  rop = (abs (op1 - op2)) / op1
    MP_AND              ,   // 18:  rop = op1 & op2
    MP_IOR              ,   // 19:  rop = op1 inclusive-or op2
    MP_XOR              ,   // 1A:  rop = op1 exclusive-or op2
    MP_COM              ,   // 1B:  rop = one's complement of op1
    MP_GCD              ,   // 1C:  rop = gcd (op1, op2)
    MP_LCM              ,   // 1D:  rop = lcm (op1, op2)
    MP_POW_UI           ,   // 1E:  rop = base ^ exp
    MP_UI_POW_UI        ,   // 1F:  rop = base ^ exp
    MP_PosMODNi         ,   // 20:  rop = N mod D
    MP_NegMODNi         ,   // 21:  rop = N mod D
    MP_PosMODPi         ,   // 22:  rop = N mod D
    MP_NegMODPi         ,   // 23:  rop = N mod D
    MP_NiMODPos         ,   // 24:  rop = N mod D
    MP_NiMODNeg         ,   // 25:  rop = N mod D
    MP_PiMODPos         ,   // 26:  rop = N mod D
    MP_PiMODNeg         ,   // 27:  rop = N mod D
    MP_MODUINi          ,   // 28:  rop = N mod D
    MP_MODUIPi          ,   // 29:  rop = N mod D
    MP_DIVISIBLE_P      ,   // 2A:  rop = non-zero if N is evenly divisible by D
    MP_DIVISIBLE_UI_P   ,   // 2B:  rop = non-zero if N is evenly divisible by D
    MP_DIVISIBLE_2EXP_P ,   // 2C:  rop = non-zero if N is evenly divisible by 2^D
} MP_ENUM;


//***************************************************************************
//  End of File: enums.h
//***************************************************************************
