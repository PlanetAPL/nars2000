//***************************************************************************
//  NARS2000 -- Types
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2012 Sudley Place Software

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


typedef unsigned int   UBOOL, *LPUBOOL;
typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;

typedef UINT8  *LPUINT8 ;
typedef UINT16 *LPUINT16;
typedef UINT32 *LPUINT32;
typedef UINT64 *LPUINT64;

typedef  INT8  *LPINT8 ;
typedef  INT16 *LPINT16;
typedef  INT32 *LPINT32;
typedef  INT64 *LPINT64;

typedef unsigned __int8  uint8;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;

typedef ULONGLONG   APLRANK;            // The type of the rank element in an array
typedef  LONGLONG   APLRANKSIGN;        // ...             rank element ... (signed)
typedef ULONGLONG   APLNELM;            // ...             # elements   ...
typedef  LONGLONG   APLNELMSIGN;        // ...             # elements   ... (signed)
typedef ULONGLONG   APLDIM;             // ...             dimensions   ...
typedef UCHAR       APLBOOL;            // ...         an element in a Boolean array (unsigned)
typedef  LONGLONG   APLINT;             // ...         ...           an integer array
typedef ULONGLONG   APLUINT;            // ...         ...           an integer array (unsigned)
typedef WCHAR       APLCHAR;            // ...         ...           a character array
typedef double      APLFLOAT;           // ...         ...           a Floating Point array
typedef HGLOBAL     APLNESTED;          // ...         ...           a nested array
typedef struct tagSYMENTRY *APLHETERO;  // ...         ...           a HETERO array
typedef struct tagTOKEN APLLIST;        // ...                       a list
typedef MP_INT      APLMPI;             // ...         ...           an Arbitrary Precision Integer array
typedef MP_RAT      APLRAT;             // ...         ...           a Rational Number array
typedef __mpfr_struct APLVFP;           // ...         ...           a Variable-precision Floating Point array
typedef enum tagARRAY_TYPES APLSTYPE;   // Storage type (see ARRAY_TYPES)

typedef ULONGLONG   APLB64;             // Boolean 64 bits
typedef uint32_t    APLB32;             // ...     32 ...
typedef uint16_t    APLB16;             // ...     16 ...
typedef uint8_t     APLB08;             // ...      8 ...

#if defined (_WIN64)
  typedef          __int64 APLI3264;    // Widest native signed value
  typedef unsigned __int64 APLU3264;    // ...           unsigned ...
#elif defined (_WIN32)
  typedef          __int32 APLI3264;    // Widest native signed value
  typedef unsigned __int32 APLU3264;    // ...           unsigned ...
#else
  #error Need code for this architecture.
#endif

// APLNESTED and APLHETERO may be either an LPSYMENTRY or
//   an HGLOBAL.

typedef ULONGLONG   APLLONGEST;         // Longest datatype in TOKEN_DATA & SYMTAB_DATA

// Define ptrs
typedef char      * LPCHAR;
typedef WCHAR     * LPWCHAR;
typedef HGLOBAL   * LPLIST;
typedef UCHAR     * LPUCHAR;
typedef APLSTYPE  * LPAPLSTYPE;

typedef APLRANK   * LPAPLRANK;
typedef APLNELM   * LPAPLNELM;
typedef APLDIM    * LPAPLDIM;
typedef APLBOOL   * LPAPLBOOL;
typedef APLINT    * LPAPLINT;
typedef APLUINT   * LPAPLUINT;
typedef APLCHAR   * LPAPLCHAR;
typedef APLNESTED * LPAPLNESTED;
typedef APLLIST   * LPAPLLIST;
typedef APLFLOAT  * LPAPLFLOAT;
typedef APLHETERO * LPAPLHETERO;
typedef APLMPI    * LPAPLMPI;
typedef APLRAT    * LPAPLRAT;
typedef APLVFP    * LPAPLVFP;

typedef APLB64    * LPAPLB64;
typedef APLB32    * LPAPLB32;
typedef APLB16    * LPAPLB16;
typedef APLB08    * LPAPLB08;

typedef APLLONGEST* LPAPLLONGEST;

#pragma pack(push,4)
typedef struct tagAPLI_SPLIT
{
    UINT lo;
     int hi;
} APLI_SPLIT, *LPAPLI_SPLIT;

typedef struct tagAPLU_SPLIT
{
    UINT lo,
         hi;
} APLU_SPLIT, *LPAPLU_SPLIT;

typedef union tagISPLIT
{
    APLINT  aplInt;
    struct {UINT lo;
             int hi;};
} ISPLIT, *LPISPLIT;

typedef union tagUSPLIT
{
    APLUINT aplInt;
    struct {UINT lo;
            UINT hi;};
} USPLIT, *LPUSPLIT;
#pragma pack(pop)


//***************************************************************************
//  End of File: types.h
//***************************************************************************
