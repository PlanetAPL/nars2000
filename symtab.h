//***************************************************************************
//  NARS2000 -- Symbol Table
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

/*

Hash Table

The hash table consists of a series of HSHENTRY entries
separated into initially DEF_HSHTAB_NBLKS number of blocks
each of DEF_HSHTAB_EPB entries per block.


Symbol Table

The symbol table consists of a series of SYMENTRY entries.

References:

Witold Litwin, Linear hashing: A new tool for file and table addressing,
Proc. 6th Conference on Very Large Databases, pages 212-223, 1980.

Per-Åke Larson, Dynamic Hash Tables, CACM 31(4):446-457, April 1988.

http://swig.stanford.edu/pub/summaries/database/linhash.html
http://www.cs.wisc.edu/~dbbook/openAccess/thirdEdition/slides/slides3ed-english/Ch11_Hash_Index.pdf#search=%22%22linear%20hashing%22%22
http://portal.acm.org/citation.cfm?id=3324


 */

//********************* HASH TABLE ******************************************

// Starting # blocks in hash table -- must be a power of two
//   so the hash mask is a contiguous series of 1s
//   (one less than a power of two).
#define DEF_HSHTAB_NBLKS       4096
#if (DEF_HSHTAB_NBLKS & (DEF_HSHTAB_NBLKS - 1)) != 0
  #error DEF_HSHTAB_NBLKS is not a power of two.
#endif

#define DEF_AFO_HSHTAB_NBLKS        128
#if (DEF_AFO_HSHTAB_NBLKS & (DEF_AFO_HSHTAB_NBLKS - 1)) != 0
  #error DEF_AFO_HSHTAB_NBLKS is not a power of two.
#endif

// # entries in each block -- can be any integer > 1
// It can't be 1 as there needs to be at least one
//   overflow entry in each block so we never assign
//   a value to the PrinHash entry which doesn't have
//   the same hash value.
#define DEF_HSHTAB_EPB         8

// Maximum hash table size (# entries)
#ifdef _WIN64
  #define DEF_HSHTAB_MAXNELM  ( 256 * 1024 * DEF_HSHTAB_EPB)
#else
  #define DEF_HSHTAB_MAXNELM  (  64 * 1024 * DEF_HSHTAB_EPB)
#endif

#define DEF_AFO_HSHTAB_MAXNELM      (256 * DEF_HSHTAB_EPB)

// Starting hash table size (# entries)
#define DEF_HSHTAB_INITNELM      (DEF_HSHTAB_NBLKS     * DEF_HSHTAB_EPB)
#define DEF_AFO_HSHTAB_INITNELM  (DEF_AFO_HSHTAB_NBLKS * DEF_HSHTAB_EPB)

// Amount to resize -- this value must be a divisor of DEF_HSHTAB_INITNELM
#define DEF_HSHTAB_INCRNELM      DEF_HSHTAB_INITNELM
#define DEF_AFO_HSHTAB_INCRNELM  DEF_AFO_HSHTAB_INITNELM

#if ((DEF_HSHTAB_INITNELM / DEF_HSHTAB_INCRNELM) * DEF_HSHTAB_INCRNELM) != \
      DEF_HSHTAB_INITNELM
  #error DEF_HSHTAB_INCRNELM not an integral divisor of DEF_HSHTAB_INITNELM.
#endif

#if ((DEF_AFO_HSHTAB_INITNELM / DEF_AFO_HSHTAB_INCRNELM) * DEF_AFO_HSHTAB_INCRNELM) != \
      DEF_AFO_HSHTAB_INITNELM
  #error DEF_AFO_HSHTAB_INCRNELM not an integral divisor of DEF_AFO_HSHTAB_INITNELM.
#endif

// Starting hash mask
#define DEF_HSHTAB_HASHMASK (DEF_HSHTAB_NBLKS - 1)

// The increment used when looking for the next available HSHENTRY
// This number must be relatively prime to each of
//   DEF_HSHTAB_INITNELM + DEF_HSHTAB_INCRNELM * {iota} DEF_HSHTAB_LARGEST / DEF_HSHTAB_INCRNELM
// An easy way to make this happen is to use a prime guaranteed to be
//   larger than the largest symbol table size, and then use as an increment
//   DEF_HSHTAB_PRIME % iHshTabTotalNelm.
#define DEF_HSHTAB_PRIME    ((UINT) 0x7FFFFFFF)  // (2^31 - 1)
#define DEF_HSHTAB_INCRFREE (DEF_HSHTAB_PRIME % DEF_HSHTAB_INITNELM)


// Hash table flags
typedef struct tagHTFLAGS
{
    UINT Inuse:1,           // 00000001:  Inuse entry
         PrinHash:1,        // 00000002:  Entry with principal hash
         CharIsValid:1,     // 00000004:  htChar is valid (and lpCharName is a
                            //            ptr to the name)
#ifdef DEBUG
         Temp:1,            // 00000008:  Temporary flag used for debugging
         :11,               // 0000FFF0:  Available bits
#else
         :12,               // 0000FFF8:  Available bits
#endif
         htChar:16;         // FFFF0000:  Char if CharIsValid is TRUE
} HTFLAGS, *LPHTFLAGS;

// N.B.:  Whenever changing the above struct (HTFLAGS),
//   be sure to make a corresponding change to
//   <ahtFlagNames> in <display.c>.


#ifdef DEBUG
typedef struct tagAPLCHAR6
{
    APLCHAR aplChar[6];
} APLCHAR6, *LPAPLCHAR6;
#endif


// Hash table entry
typedef struct tagHSHENTRY
{
    struct tagHSHENTRY
            *NextSameHash,  // 00:  Next entry with same hash
            *PrevSameHash;  // 04:  Prev ...
    HTFLAGS htFlags;        // 08:  Flags
    UINT    uHash,          // 0C:  The hash value for this entry
            uHashAndMask;   // 10:  uHash & the current mask
    union                   // 14:  Handle of the entry's name (NULL if none)
    {
        HGLOBAL htGlbName;          // Use only if CharIsValid EQ FALSE
        LPWCHAR lpwCharName;        // Use only if CharIsValid EQ TRUE
#ifdef DEBUG
        LPAPLCHAR6 *lplpaplChar6;   // For debugging only
#endif
    };
    struct tagSYMENTRY
            *htSymEntry;    // 18:  Ptr to the matching SYMENTRY
                            // 1C:  Length
} HSHENTRY, *LPHSHENTRY;

#define LPHSHENTRY_NONE     ((LPHSHENTRY) -1)

typedef struct tagHSHTABSTR
{
    struct tagHSHTABSTR
              *lphtsPrvSrch,            // 00:  Ptr to previous HSHTABSTR
                                        //        for the purposes of searching (NULL = none)
              *lphtsPrvMFO;             // 04:  Ptr to previous HSHTABSTR
                                        //        for the purposes of append new/lookup old MFO (NULL = none)
    LPHSHENTRY lpHshTab,                // 08:  Ptr to start of HshTab
               lpHshTabSplitNext;       // 0C:  ...    next HTE to split (incremented by DEF_HSHTAB_NBLKS)
    int        iHshTabBaseNelm,         // 10:  Base size of hash table
               iHshTabTotalNelm,        // 14:  # HTEs, currently, including EPBs
               iHshTabIncrFree,         // 18:  Increment when looping through HshTab looking for free entry
               iHshTabIncrNelm,         // 1C:  Incremental size
               iHshTabEPB;              // 20:  # entries per block
    UINT       uHashMask,               // 24:  Mask for all HshTab lookups
               uHshTabNBlks;            // 28:  # blocks in this HshTab
    UINT       bGlbHshTab:1,            // 2C:  00000001:  This HTS is global
               bSysNames:1,             //      00000002:  TRUE iff system names have been appended
               bGlbHshSymTabs:1,        //      00000004:  TRUE iff the Sym & Hsh tabs are allocated from global (not virtual) memory
               :29;                     //      FFFFFFF8:  Available bits
    struct tagSYMENTRY
              *lpSymTab,                // 30:  Ptr to start of Symtab
              *lpSymTabNext,            // 34:  Ptr to next available STE
              *lpSymQuad[SYSVAR_LENGTH];// 38:  Ptr to array of system var STEs (15*4 bytes)
    UINT       uSymTabIncrNelm;         // 74:  # STEs by which to resize when low
    int        iSymTabTotalNelm;        // 78:  # STEs, currently
    struct tagSYMENTRY
              *steZero,                 // 7C:  Ptr to STE for constant zero
              *steOne,                  // 80:  ...                     one
              *steBlank,                // 84:  ...                     blank
              *steAlpha,                // 88:  ...            Alpha
              *steDel,                  // 8C:  ...            Del
              *steOmega,                // 90:  ...            Omega
              *steLftOper,              // 94:  ...            Alpha Alpha
              *steDelDel,               // 98:  ...            Del Del
              *steRhtOper,              // 9C:  ...            Omega Omega
              *steNoValue;              // A0:  ...            no-value result
} HSHTABSTR, *LPHSHTABSTR;              // A4:  Length

//********************* SYMBOL TABLE ****************************************

// Maximum symbol table size (# entries)
#ifdef _WIN64
  #define DEF_SYMTAB_MAXNELM  ( 256*1024)
#else
  #define DEF_SYMTAB_MAXNELM  (  64*1024)
#endif

#define DEF_AFO_SYMTAB_MAXNELM          256

// Starting symbol table size (# entries)
#define DEF_SYMTAB_INITNELM      (   4*1024)
#define DEF_AFO_SYMTAB_INITNELM DEF_AFO_SYMTAB_MAXNELM

// Amount to resize -- this value must be a divisor of DEF_SYMTAB_INITNELM
#define DEF_SYMTAB_INCRNELM      DEF_SYMTAB_INITNELM
#define DEF_AFO_SYMTAB_INCRNELM DEF_AFO_SYMTAB_INITNELM

typedef enum tagIMM_TYPES
{
    IMMTYPE_SAME = -1 ,     // -1:  Use same type as source
    IMMTYPE_ERROR = 0 ,     // 00:  Error (not an immediate type)
    IMMTYPE_BOOL      ,     // 01:  Boolean
    IMMTYPE_INT       ,     // 02:  Integer
    IMMTYPE_FLOAT     ,     // 03:  Floating point
    IMMTYPE_CHAR      ,     // 04:  Character
    IMMTYPE_PRIMFCN   ,     // 05:  Primitive monadic/dyadic function
    IMMTYPE_PRIMOP1   ,     // 06:  Primitive monadic operator
    IMMTYPE_PRIMOP2   ,     // 07:  ...       dyadic  ...
    IMMTYPE_PRIMOP3   ,     // 08:  ...       ambiguous ...
    IMMTYPE_RAT       ,     // 09:  Rational number
    IMMTYPE_VFP       ,     // 0A:  Variable-precision floating-point number
                            // 0B-0F:  Available entries (4 bits)
} IMM_TYPES, *LPIMM_TYPES;

// N.B.:  Whenever changing the above enum (IMM_TYPES),
//   be sure to make a corresponding change to
//   <TranslateImmTypeToChar> in <translate.c>,
//   the macros <IsImmXXX> in <macros.h>, the
//   #define ImmTypeAsChar just below here.

// Translate an immediate type to a char
// Note that the order of the chars in this #define
//   depends upon the ordering of the above enum
#define ImmTypeAsChar   L"!BIFC?123RV"

// Name types
typedef enum tagNAME_TYPES
{
    NAMETYPE_UNK = 0,       // 00 (0000):  Name is unknown
    NAMETYPE_VAR,           // 01 (0001):  ...     V variable
    NAMETYPE_FN0,           // 02 (0010):  ...     F niladic function
    NAMETYPE_FN12,          // 03 (0011):  ...     F monadic/dyadic/ambivalent function
    NAMETYPE_OP1,           // 04 (0100):  ...     O monadic operator
    NAMETYPE_OP2,           // 05 (0101):  ...     O dyadic operator
    NAMETYPE_OP3,           // 06 (0110):  ...     F ambiguous function/operator
    NAMETYPE_FILL1,         // 07 (0111):  ...       filler
    NAMETYPE_LST,           // 08 (1000):  ...       list
    NAMETYPE_FILL2,         // 09 (1001):  ...       filler
    NAMETYPE_TRN,           // 0A (1010):  ...     F train
                            // 09-0F:  Available entries (4 bits)
} NAME_TYPES;

// N.B.:  Whenever changing the above enum (NAME_TYPES),
//   be sure to make a corresponding change to
//   NAMETYPEMASK_FN and NAMETYPEMASK_OP, and
//   the macros <IsNameTypeXXX> in <macros.h>.

#define NAMETYPE_STRING     "?VNF123?L?T"
//                              0       1       2       3       4       5       6       7       8       9       A
#define NAMETYPE_STRPTR     { "Unk",  "Var",  "Nil",  "Fcn",  "Op1",  "Op2",  "Op3",  "???",  "Lst",  "???",  "Trn"}
#define NAMETYPE_WSTRPTR    {L"Unk", L"Var", L"Nil", L"Fcn", L"Op1", L"Op2", L"Op3", L"???", L"Lst", L"???", L"Trn"}

// The above enum is constructed so as to allow the following masks to be used:
#define NAMETYPEMASK_FN     0x02    // Name is a function
#define NAMETYPEMASK_OP     0x04    // Name is an operator

typedef enum tagOBJ_NAMES
{
    OBJNAME_NONE = 0,       // 00:  Unnamed
    OBJNAME_USR,            // 01:  User name
    OBJNAME_SYS,            // 02:  System name (starts with a Quad or Quote-quad)
    OBJNAME_MFO,            // 03:  Magic function/operator
    OBJNAME_LOD,            // 04:  )LOAD and )COPY HGLOBAL
    OBJNAME_NOVALUE,        // 05:  NoValue object (result from, say, {execute}'')
                            // 06-07:  Available entries (3 bits)
} OBJ_NAMES;

#define OBJNAME_WSTRPTR     {L"None", L"USR", L"SYS", L"MFO", L"LOD", L"NoV"}

// Symbol table flags
typedef struct tagSTFLAGS
{
    UINT Imm:1,             // 00000001:  The data in .stData is Immediate simple numeric or character scalar
         ImmType:4,         // 0000001E:  ...                    Immediate Boolean, Integer, Character, or Float (see IMM_TYPES)
         Inuse:1,           // 00000020:  Inuse entry
         Value:1,           // 00000040:  Entry has a value
         ObjName:3,         // 00000380:  The data in .stData is NULL if .stNameType is NAMETYPE_UNK; value, address, or HGLOBAL otherwise
                            //            (see OBJ_NAMES)
         stNameType:4,      // 00003C00:  The data in .stdata is value (if .Imm), address (if .FcnDir), or HGLOBAL (otherwise)
                            //            (see NAME_TYPES)
         SysVarValid:5,     // 0007C000:  Index to validation routine for System Vars (see SYS_VARS)
         UsrDfn:1,          // 00080000:  User-defined function/operator
         DfnLabel:1,        // 00100000:  User-defined function/operator label        (valid only if .Value is set)
         DfnSysLabel:1,     // 00200000:  User-defined function/operator system label (valid only if .Value is set)
         DfnAxis:1,         // 00400000:  User-defined function/operator accepts axis value
         FcnDir:1,          // 00800000:  Direct function/operator               (stNameFcn is valid)
         StdSysName:1,      // 01000000:  Is a standard System Name
         bIsAlpha:1,        // 02000000:  Is Alpha
         bIsOmega:1,        // 04000000:  Is Omega
         :5;                // F8000000:  Available bits
} STFLAGS, *LPSTFLAGS;

// N.B.:  Whenever changing the above struct (STFLAGS),
//   be sure to make a corresponding change to
//   <astFlagNames> in <dispdbg.c>.

// .Inuse and .PrinHash are valid for all entries.
// .Inuse   = 0 implies that all but .PrinHash are zero.
// .Imm     implies one and only one of the IMMTYPE_***s
// .Imm     = 1 implies that one and only one of aplBoolean, aplInteger, aplChar, or aplFloat is valid.
// .Imm     = 0 implies that stGlbData is valid.
// .Value   is valid for NAMETYPE_VAR only, however .stNameType EQ NAMETYPE_VAR
//          should never be without a value.
// .UsrDfn  is set when the function is user-defined.
// .FcnDir  may be set for any function/operator; it is a
//          direct pointer to the code.
// htGlbName in HSHENTRY is set when .Imm and .FcnDir are clear.

// Immediate data or a handle to global data
typedef union tagSYMTAB_DATA
{
    APLBOOL    stBoolean;       // 00:  A number (Boolean)
    APLINT     stInteger;       // 00:  A number (Integer)
    APLFLOAT   stFloat;         // 00:  A floating point number
    APLCHAR    stChar;          // 00:  A character
    HGLOBAL    stGlbData;       // 00:  Handle of the entry's data
    LPVOID     stVoid;          // 00:  An abritrary ptr
    LPPRIMFNS  stNameFcn;       // 00:  Ptr to a named function
    APLLONGEST stLongest;       // 00:  Longest datatype (so we can copy the entire data)
                                // 08:  Length
} SYMTAB_DATA, *LPSYMTAB_DATA;

#define SYM_HEADER_SIGNATURE    'EMYS'

// Symbol table entry
typedef struct tagSYMENTRY
{
    STFLAGS     stFlags;        // 00:  Flags
    SYMTAB_DATA stData;         // 04:  For immediates, the data value;
                                //        for others, the HGLOBAL (8 bytes)
    LPHSHENTRY  stHshEntry;     // 0C:  Ptr to the matching HSHENTRY
    struct tagSYMENTRY
               *stPrvEntry,     // 10:  Ptr to previous (shadowed) STE (NULL = none)
               *stSymLink;      // 14:  Ptr to next entry in linked list of
                                //        similarly grouped entries (NULL = none)
    UINT        stSILevel;      // 18:  State Indicator Level for this STE
    HEADER_SIGNATURE Sig;       // 1C:  STE header signature
                                // 20:  Length
} SYMENTRY, *LPSYMENTRY;

#define LPSYMENTRY_NONE     ((LPSYMENTRY) -1)


//***************************************************************************
//  End of File: symtab.h
//***************************************************************************
