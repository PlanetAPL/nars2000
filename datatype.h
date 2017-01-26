//***************************************************************************
//  NARS2000 -- Datatype Header File
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


#define APL_EXPONENT_LEN    11          // APLFLOAT exponent length (# bits)
#define APL_MANTISSA_LEN    52          // ...      mantissa ...
#define APL_EXP_BIAS        1023        // ...      exponent bias

typedef struct tagAPLFLOAT_STR
{
    APLUINT uMantissa:APL_MANTISSA_LEN, // 00:  000FFFFFFFFFFFFF:  The mantissa
            uExp     :APL_EXPONENT_LEN, //      7FF0000000000000:  The exponent + bias
            bSign    :1;                //      8000000000000000:  The sign
} APLFLOATSTR, *LPAPLFLOATSTR;

typedef union tagAPLFLOAT_UNION
{
    APLFLOAT    aplFloat;               // 00:  The floating point number
    APLUINT     aplUInt;                // 00:  ... as an unsigned int
    APLFLOATSTR aplFloatStr;            // 00:  The struct to pick it apart
} APLFLOATUNION, *LPAPLFLOATUNION;

typedef union tagAPLUINT_UNION
{
    APLINT  aplInt;
    APLUINT aplUInt;
    DWORD   dwords[2];
} APLUINTUNION;

#define MAX_APLUINT     0xFFFFFFFFFFFFFFFF                      // Largest  APLINT
#define MAX_APLINT      0x7FFFFFFFFFFFFFFF                      // ...      APLINT
#define MAX_APLNELM     MAX_APLINT                              // ...      APLNELM
#define MAX_APLRANK     MAX_APLINT                              // ...      APLRANK
#define MAX_APLDIM      MAX_APLINT                              // ...      APLDIM
#define MIN_APLINT      0x8000000000000000                      // Smallest APLINT
#define MIN_APLINT_STR  WS_UTF16_OVERBAR L"9223372036854775808" // ...      ...    as a string
#define MAX_UNICODE     0x10FFFF                                // Largest allowable Unicode code point

// # bits in an APLxxx
#define BITS_IN_APLCHAR (NBIB * (sizeof (APLCHAR)))
#define BITS_IN_APLINT  (NBIB * (sizeof (APLINT )))
#define BITS_IN_APLUINT (NBIB * (sizeof (APLUINT)))

// # integers in an APLCHAR
#define APLCHAR_SIZE    (BIT0 << BITS_IN_APLCHAR)

// Strand types -- used during strand constuction
typedef enum tagSTRAND_TYPES
{
 STRAND_INIT = 0,                       // 00:  Initial state
 STRAND_BOOL,                           // 01:  Boolean
 STRAND_INT,                            // 02:  Integer
 STRAND_FLOAT,                          // 03:  Floating point
 STRAND_CHAR,                           // 04:  Character
 STRAND_CHARST,                         // 05:  Character strand     (two or more character scalars in a row)
 STRAND_STRING,                         // 06:  Character vector
 STRAND_HETERO,                         // 07:  Simple heterogeneous (mixed numeric and character scalar)
 STRAND_NESTED,                         // 08:  Nested
 STRAND_RAT,                            // 09:  Multiprecision Rational Number
 STRAND_VFP,                            // 0A:  Variable-precision Float

 STRAND_LENGTH                          // 0B:  # elements in this enum
                                        //      *MUST* be the last entry
} STRAND_TYPES;

typedef struct tagDTHC2
{
    APLFLOAT Real,                  // 00:  Complex number,    real      part    (8 bytes)
             Imag;                  // 08:  ...                imaginary part    (8 bytes)
} DTHC2, *LPDTHC2;

typedef struct tagDTHC4
{
    APLFLOAT  Real,                 // 00:  Quaternion number, real      part    (8 bytes)
              Imag1,                // 08:  ...                imaginary part #1 (8 bytes)
              Imag2,                // 10:  ...                imaginary part #2 (8 bytes)
              Imag3;                // 18:  ...                imaginary part #3 (8 bytes)
} DTHC4, *LPDTHC4;

typedef struct tagDTHC8
{
    APLFLOAT Real,                  // 00:  Octonion number,   real      part    (8 bytes)
             Imag1,                 // 08:  ...                imaginary part #1 (8 bytes)
             Imag2,                 // 10:  ...                imaginary part #2 (8 bytes)
             Imag3,                 // 18:  ...                imaginary part #3 (8 bytes)
             Imag4,                 // 20:  ...                imaginary part #4 (8 bytes)
             Imag5,                 // 28:  ...                imaginary part #5 (8 bytes)
             Imag6,                 // 30:  ...                imaginary part #6 (8 bytes)
             Imag7;                 // 38:  ...                imaginary part #7 (8 bytes)
} DTHC8, *LPDTHC8;

typedef union tagALLTYPES
{
    APLBOOL    aplBoolean;          // 00:  Boolean
    APLINT     aplInteger;          // 00:  Integer or real number as an integer ( 8 bytes)
    APLFLOAT   aplFloat;            // 00:  Real number as a float               ( 8 bytes)
    APLCHAR    aplChar;             // 00:  Character
    DTHC2      aplHC2;              // 00:  Complex number                       (16 bytes)
    DTHC4      aplHC4;              // 00:  Quaternion number                    (32 bytes)
    DTHC8      aplHC8;              // 00:  Octonion number                      (64 bytes)
    APLHETERO  aplHetero;           // 00:  Heterogeneous array                  ( 4 bytes)
    APLNESTED  aplNested;           // 00:  Nested array                         ( 4 bytes)
    APLRAT     aplRat;              // 00:  RAT number, num and denom parts      (24 bytes)
    APLVFP     aplVfp;              // 00:  VFP number                           (16 bytes)
    APLLONGEST aplLongest;          // 00:  tkLongest ot stLongest
} ALLTYPES, *LPALLTYPES;

// Array types -- used to identify array storage type in memory
typedef enum tagARRAY_TYPES
{
 ARRAY_BOOL = 0,                        // 00:  Boolean
 ARRAY_INT,                             // 01:  Integer
 ARRAY_FLOAT,                           // 02:  Floating point
 ARRAY_CHAR,                            // 03:  Character
 ARRAY_HETERO,                          // 04:  Simple heterogeneous (mixed numeric and character scalars)
 ARRAY_NESTED,                          // 05:  Nested
 ARRAY_LIST,                            // 06:  List
 ARRAY_APA,                             // 07:  Arithmetic Progression Array
 ARRAY_RAT,                             // 08:  Multiprecision Rational Number
 ARRAY_VFP,                             // 09:  Variable-precision Float

 ARRAY_LENGTH,                          // 0A:  # elements in this enum
                                        //      *MUST* be the last non-error entry
                                        // 0A-0F:  Available entries (4 bits)
 ARRAY_INIT  = ARRAY_LENGTH,
 ARRAY_ERROR = (APLSTYPE) -1,
} ARRAY_TYPES;

// Whenever changing this <enum>, be sure to make a
//   corresponding change to <StorageType>  in <primfns.c>,
//   <TypeDemote> in <typemote.c>
//   <aTypePromote> in <typemote.h>,
//   <uTypeMap> in <externs.h>,
//   <aTypeFree> in <typemote.h>,
//   <IsSimpleNH> and <IsSimpleNum> macros in <macros.h>,
//   ArrayTypeAsChar and BPE_VEC in <datatype.h>.


// Translate an array type to a char
// Note that the order of the chars in this #define
//   depends upon the ordering of the above enum
#define ArrayTypeAsChar     L"BIFCHNLARV"

// Bites per element vector
// N.B. the order of elements in this vector matches
//   the order of elements in the above ARRAY_TYPES enum.
//                  B   I   F   C  H  N  L   A   R   V
#define BPE_VEC     1, 64, 64, 16, 0, 0, 0, 64,  0,  0

// Define APA structure
typedef struct tagAPLAPA                // Offset + Multiplier {times} {iota} NELM (origin-0)
{
    APLINT  Off,                        // 00:  Offset
            Mul;                        // 04:  Multiplier
                                        // 08:  Length
} APLAPA, * LPAPLAPA;

/*

Array Storage
-------------

Arrays are stored either in memory or, if it's a simple scalar,
in the symbol table.  If it's in the symbol table, it is identified
by its symbol table index.

All variable arrays in memory have a common header (VARARRAY_HEADER).
All function ...                                   (FCNARRAY_HEADER).
All list     ...                                   (LSTARRAY_HEADER).


Array Contents
--------------

The contents of a nested or a heterogeneous array may be
an LPSYMENTRY or an HGLOBAL.  These two types are
distinguished by their low-order two bits.  Because we know that
LPSYMENTRY and HGLOBAL are pointers and that these pointers are
aligned on a dword boundary, their two low-order bits are zero,
and thus available.


Array Types
-----------

ARRAY_BOOL      One value per bit, stored sequentially with
                the 1st value in bit position 0.

ARRAY_INT       One value per APLINT, stored sequentially.

ARRAY_FLOAT     One value per APLFLOAT, stored sequentially.

ARRAY_CHAR      One value per APLCHAR, stored sequentially.

ARRAY_HETERO    One value per APLHETERO, stored sequentially.
                The array contents are all LPSYMENTRYs.

ARRAY_NESTED    One value per APLNESTED, stored sequentially.
                The array contents may be a mixture of LPSYMENTRYs and
                HGLOBALs (which point to one of the other Array Types),
                all of which are distinguished by the low-order two bits.

ARRAY_LIST      One value per APLLIST, stored sequentially.
                Values are either VARIMMED, VARARRAY, or LISTSEP tokens.

ARRAY_APA       An APA is a representation of Off + Mul {times} {iota} aplNELM
                in origin-0.

                These two elements are stored in the array in succession:

                1.  Off = Offset
                2.  Mul = Multiplier

                The NELM comes from the array header.

ARRAY_RAT       A rational or extended precision (denom EQ 1) scalar number.
                The value is an HGLOBAL of a GMP value (see <gmp.h> for details).

ARRAY_VFP       A variable-precision floating point scalar number.
                The value is an HGLOBAL of a GMP value (see <gmp.h> for details).
 */

typedef struct tagHEADER_SIGNATURE
{
    UINT             nature;            // 00:  Array header signature (common to all types of arrays)
                                        // 04:  Length
} HEADER_SIGNATURE, *LPHEADER_SIGNATURE;

typedef enum tagPERM_NDX
{
    PERMNDX_NONE = 0    ,               // 00:  Not a permanent array
    PERMNDX_QUADA       ,               // 01:  []A
    PERMNDX_QUADAV      ,               // 02:  []AV
    PERMNDX_QUADD       ,               // 03:  []D
    PERMNDX_QUADxLX     ,               // 04:  []xLX default
    PERMNDX_QUADFC      ,               // 05:  []FC default
    PERMNDX_QUADFEATURE ,               // 06:  []FEATURE default
    PERMNDX_QUADIC      ,               // 07:  []IC default
    PERMNDX_SACLEAR     ,               // 08:  'CLEAR'
    PERMNDX_SAERROR     ,               // 09:  'ERROR'
    PERMNDX_SAEXIT      ,               // 0A:  'EXIT'
    PERMNDX_SAOFF       ,               // 0B:  'OFF'
    PERMNDX_V0CHAR      ,               // 0C:  ''
    PERMNDX_ZILDE       ,               // 0D:  {zilde}
    PERMNDX_0BY0        ,               // 0E:  0 0 {rho} 0 -- []EC[2] default
    PERMNDX_3BY0        ,               // 0F:  3 0 {rho}'' -- []EM default
                                        // ??-??:  Available entries (4 bits)
} PERM_NDX;

// Variable array header
#define VARARRAY_HEADER_SIGNATURE   'SRAV'

typedef struct tagVARARRAY_HEADER
{
    HEADER_SIGNATURE Sig;               // 00:  Array header signature
    UINT             ArrType:4,         // 04:  0000000F:  The type of the array (see ARRAY_TYPES)
                     PermNdx:4,         //      000000F0:  Permanent array index (e.g., PERMNDX_ZILDE for {zilde})
                     SysVar:1,          //      00000100:  Izit for a Sysvar (***DEBUG*** only)?
                     PV0:1,             //      00000200:  Permutation Vector in origin-0
                     PV1:1,             //      00000400:  ...                          1
                     bSelSpec:1,        //      00000800:  Select Specification array
                     All2s:1,           //      00001000:  Values are all 2s
#ifdef DEBUG
                     bMFOvar:1,         //      00002000:  Magic Function/Operator var -- do not display
                     :18;               //      FFFFC000:  Available bits
#else
                     :19;               //      FFFFE000:  Available bits
#endif
    UINT             RefCnt;            // 08:  Reference count
    APLNELM          NELM;              // 0C:  # elements in the array (8 bytes)
    APLRANK          Rank;              // 14:  The rank of the array (8 bytes)
                                        //      followed by the dimensions
                                        // 1C:  Length
} VARARRAY_HEADER, *LPVARARRAY_HEADER;

// List array header
#define LSTARRAY_HEADER_SIGNATURE   'TSIL'

typedef struct tagLSTARRAY_HEADER
{
    HEADER_SIGNATURE Sig;               // 00:  Array header signature
    APLNELM          NELM;              // 04:  # elements in the array (8 bytes)
                                        // 0C:  Length
} LSTARRAY_HEADER, *LPLSTARRAY_HEADER;

// Function array header signature
#define FCNARRAY_HEADER_SIGNATURE   'SNCF'

// Function array header
typedef struct tagFCNARRAY_HEADER
{
    HEADER_SIGNATURE Sig;               // 00:  Array header signature
    UINT             fnNameType:4,      // 04:  0000000F:  The type of the array (see NAME_TYPES)
                     :28;               //      FFFFFFF0:  Available bits
    UINT             RefCnt,            // 08:  Reference count
                     tknNELM;           // 0C:  # tokens in the array (each of which may point to additional arrays)
    HGLOBAL          hGlbTxtLine;       // 10:  Line text global memory handle (may be NULL)
    FILETIME         ftCreation,        // 14:  Time of last creation (8 bytes)
                     ftLastMod;         // 1C:  Time of last modification (8 bytes)
                                        // 24:  Length
} FCNARRAY_HEADER, *LPFCNARRAY_HEADER;

// Named strand header
#define VARNAMED_HEADER_SIGNATURE   ' MNV'

typedef struct tagVARNAMED_HEADER
{
    HEADER_SIGNATURE Sig;               // 00:  Array header signature
    APLNELM          NELM;              // 04:  # elements in the array (8 bytes)
                                        // 0C:  Length
} VARNAMED_HEADER, *LPVARNAMED_HEADER;

// Variable/Function/Operator Header ptrs union
typedef union tagVFOHDRPTRS
{
    LPVOID            lpMemVFO;         // 00:  Generic ptr
    LPVARARRAY_HEADER lpMemVar;         // 00:  Ptr to a variable
    LPFCNARRAY_HEADER lpMemFcn;         // 00:  ...      function array
    struct tagDFN_HEADER *
                      lpMemDfn;         // 00:  ...      UDFO
    LPLSTARRAY_HEADER lpMemLst;         // 00:  ...      list
    LPVARNAMED_HEADER lpMemNam;         // 00:  ...      named strand
} VFOHDRPTRS, *LPVFOHDRPTRS;

#define SYSFN_HEADER_SIGNATURE      'FSYS'      // Pseudo-signature

// Distinguish between immediate LPSYMENTRY and HGLOBAL in an array
typedef enum tagPTR_TYPES
{
 PTRTYPE_STCONST = 0,                   // 00:  *MUST* be first (and thus zero) as we can't afford to clear all the time
 PTRTYPE_HGLOBAL,                       // 01:  This ptr is an HGLOBAL
 PTRTYPE_LENGTH                         // 02:  # entries in this enum (1 bit)
                                        // No available entries (1 bit)
} PTR_TYPES;

#define PTRTYPE_MASK    BIT0            // This masks the one low-order bit
#define PTR_SUCCESS     ((LPVOID) (HANDLE_PTR) 0x0FFFFFFF)  // Used to indicate success on ArrayIndexSet only
#define IsPtrSuccess(a) ((a) EQ PTR_SUCCESS)


//***************************************************************************
//  End of File: datatype.h
//***************************************************************************
