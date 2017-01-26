/*
 * Compilers that uses ILP32, LP64 or P64 type models
 * for both Win32 and Win64 are supported by this file.
 *
 * Copyright (C) 1999 Patrik Stridvall
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef __WINE_BASETSD_H
#define __WINE_BASETSD_H

#ifdef __cplusplus
extern "C" {
#endif /* defined(__cplusplus) */

/*
 * Win32 was easy to implement under Unix since most (all?) 32-bit
 * Unices uses the same type model (ILP32) as Win32, where int, long
 * and pointer are 32-bit.
 *
 * Win64, however, will cause some problems when implemented under Unix.
 * Linux/{Alpha, Sparc64} and most (all?) other 64-bit Unices uses
 * the LP64 type model where int is 32-bit and long and pointer are
 * 64-bit. Win64 on the other hand uses the P64 (sometimes called LLP64)
 * type model where int and long are 32 bit and pointer is 64-bit.
 */

#if defined(__x86_64__) && !defined(_WIN64)
#define _WIN64
#endif

/* Type model independent typedefs */
/* The __intXX types are native types defined by the MS C compiler.
 * Apps that make use of them before they get defined here, can
 * simply add to the command line:
 *    -D__int8=char -D__int16=short -D__int32=int "-D__int64=long long"
 */
#if !defined(_MSC_VER) && !defined(__WIDL__)
#  ifndef __int8
#    define __int8  char
#  endif
#  ifndef __int16
#    define __int16 short
#  endif
#  ifndef __int32
#    define __int32 int
#  endif
#  ifndef __int64
#    ifdef _WIN64
#      define __int64 long
#    else
#      define __int64 long long
#    endif
#  endif
#endif /* !defined(_MSC_VER) */

/* FIXME: DECLSPEC_ALIGN should be declared only in winnt.h, but we need it here too */
#ifndef DECLSPEC_ALIGN
# if defined(_MSC_VER) && (_MSC_VER >= 1300) && !defined(MIDL_PASS)
#  define DECLSPEC_ALIGN(x) __declspec(align(x))
# elif defined(__GNUC__)
#  define DECLSPEC_ALIGN(x) __attribute__((aligned(x)))
# else
#  define DECLSPEC_ALIGN(x)
# endif
#endif

typedef signed char      INT8, *PINT8;
typedef signed short     INT16, *PINT16;
typedef signed int       INT32, *PINT32;
typedef unsigned char    UINT8, *PUINT8;
typedef unsigned short   UINT16, *PUINT16;
typedef unsigned int     UINT32, *PUINT32;
typedef signed int       LONG32, *PLONG32;
typedef unsigned int     ULONG32, *PULONG32;
typedef unsigned int     DWORD32, *PDWORD32;

#ifdef _MSC_VER
typedef signed __int64   INT64, *PINT64;
typedef unsigned __int64 UINT64, *PUINT64;
typedef signed __int64   LONG64, *PLONG64;
typedef unsigned __int64 ULONG64, *PULONG64;
typedef unsigned __int64 DWORD64, *PDWORD64;
#else
typedef signed __int64   DECLSPEC_ALIGN(8) INT64, *PINT64;
typedef unsigned __int64 DECLSPEC_ALIGN(8) UINT64, *PUINT64;
typedef signed __int64   DECLSPEC_ALIGN(8) LONG64, *PLONG64;
typedef unsigned __int64 DECLSPEC_ALIGN(8) ULONG64, *PULONG64;
typedef unsigned __int64 DECLSPEC_ALIGN(8) DWORD64, *PDWORD64;
#endif

#if !defined(_W64)
#if !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && _MSC_VER >= 1300
#define _W64 __w64
#else
#define _W64
#endif
#endif

/* Win32 or Win64 dependent typedef/defines. */

#ifdef _WIN64

typedef unsigned __int64 HANDLE_PTR;
#define __int3264   __int64
typedef signed __int64   INT_PTR, *PINT_PTR;
typedef signed __int64   LONG_PTR, *PLONG_PTR;
typedef unsigned __int64 UINT_PTR, *PUINT_PTR;
typedef unsigned __int64 ULONG_PTR, *PULONG_PTR;
typedef unsigned __int64 DWORD_PTR, *PDWORD_PTR;

#define MAXINT_PTR 0x7fffffffffffffff
#define MININT_PTR 0x8000000000000000
#define MAXUINT_PTR 0xffffffffffffffff

typedef int HALF_PTR, *PHALF_PTR;
typedef unsigned int UHALF_PTR, *PUHALF_PTR;

#define MAXHALF_PTR 0x7fffffff
#define MINHALF_PTR 0x80000000
#define MAXUHALF_PTR 0xffffffff

// __inline
// unsigned long
// HandleToUlong(
//     const void *h
//     )
// {
//     return((unsigned long) h );
// }

// __inline
// long
// HandleToLong(
//     const void *h
//     )
// {
//     return((long) h );
// }

__inline
void *
LongToHandle(
    const long h
    )
{
    return((void *) (INT_PTR) h );
}


// __inline
// unsigned long
// PtrToUlong(
//     const void  *p
//     )
// {
//     return((unsigned long) p );
// }

// __inline
// unsigned int
// PtrToUint(
//     const void  *p
//     )
// {
//     return((unsigned int) p );
// }

__inline
unsigned short
PtrToUshort(
    const void  *p
    )
{
    return((unsigned short) p );
}

// __inline
// long
// PtrToLong(
//     const void  *p
//     )
// {
//     return((long) p );
// }

// __inline
// int
// PtrToInt(
//     const void  *p
//     )
// {
//     return((int) p );
// }

__inline
short
PtrToShort(
    const void  *p
    )
{
    return((short) p );
}

__inline
void *
IntToPtr(
    const int i
    )
// Caution: IntToPtr() sign-extends the int value.
{
    return( (void *)(INT_PTR)i );
}

__inline
void *
UIntToPtr(
    const unsigned int ui
    )
// Caution: UIntToPtr() zero-extends the unsigned int value.
{
    return( (void *)(UINT_PTR)ui );
}

__inline
void *
LongToPtr(
    const long l
    )
// Caution: LongToPtr() sign-extends the long value.
{
    return( (void *)(LONG_PTR)l );
}

__inline
void *
ULongToPtr(
    const unsigned long ul
    )
// Caution: ULongToPtr() zero-extends the unsigned long value.
{
    return( (void *)(ULONG_PTR)ul );
}

#else /* FIXME: defined(_WIN32) */

typedef _W64 unsigned long HANDLE_PTR;
#define __int3264   __int32
typedef int INT_PTR, *PINT_PTR;
typedef long LONG_PTR, *PLONG_PTR;
typedef unsigned int UINT_PTR, *PUINT_PTR;
typedef unsigned long ULONG_PTR, *PULONG_PTR;
typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;

#define MAXINT_PTR 0x7fffffff
#define MININT_PTR 0x80000000
#define MAXUINT_PTR 0xffffffff

typedef signed short HALF_PTR, *PHALF_PTR;
typedef unsigned short UHALF_PTR, *PUHALF_PTR;

#define MAXUHALF_PTR 0xffff
#define MAXHALF_PTR 0x7fff
#define MINHALF_PTR 0x8000

#define HandleToUlong( h ) ((ULONG)(ULONG_PTR)(h) )
#define HandleToLong( h ) ((LONG)(LONG_PTR) (h) )
#define LongToHandle( h) ((HANDLE)(LONG_PTR) (h))
#define PtrToUlong( p ) ((ULONG)(ULONG_PTR) (p) )
#define PtrToLong( p ) ((LONG)(LONG_PTR) (p) )
#define PtrToUint( p ) ((UINT)(UINT_PTR) (p) )
#define PtrToInt( p ) ((INT)(INT_PTR) (p) )
#define PtrToUshort( p ) ((unsigned short)(ULONG_PTR)(p) )
#define PtrToShort( p ) ((short)(LONG_PTR)(p) )
#define IntToPtr( i )    ((VOID *)(INT_PTR)((int)i))
#define UIntToPtr( ui )  ((VOID *)(UINT_PTR)((unsigned int)ui))
#define LongToPtr( l )   ((VOID *)(LONG_PTR)((long)l))
#define ULongToPtr( ul )  ((VOID *)(ULONG_PTR)((unsigned long)ul))

#endif /* defined(_WIN64) || defined(_WIN32) */

typedef LONG_PTR SSIZE_T, *PSSIZE_T;
typedef ULONG_PTR SIZE_T, *PSIZE_T;

typedef ULONG_PTR KAFFINITY, *PKAFFINITY;

/* Some Wine-specific definitions */

/* Architecture dependent settings. */
/* These are hardcoded to avoid dependencies on config.h in Winelib apps. */
#if defined(__i386__)
# undef  WORDS_BIGENDIAN
# undef  BITFIELDS_BIGENDIAN
# define ALLOW_UNALIGNED_ACCESS
#elif defined(__x86_64__)
# undef  WORDS_BIGENDIAN
# undef  BITFIELDS_BIGENDIAN
# define ALLOW_UNALIGNED_ACCESS
#elif defined(__sparc__)
# define WORDS_BIGENDIAN
# define BITFIELDS_BIGENDIAN
# undef  ALLOW_UNALIGNED_ACCESS
#elif defined(__powerpc__)
# define WORDS_BIGENDIAN
# define BITFIELDS_BIGENDIAN
# undef  ALLOW_UNALIGNED_ACCESS
#elif defined(__ALPHA__)
# undef  WORDS_BIGENDIAN
# undef  BITFIELDS_BIGENDIAN
# undef  ALLOW_UNALIGNED_ACCESS
#elif !defined(RC_INVOKED) && !defined(__WIDL__)
# error Unknown CPU architecture!
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif /* defined(__cplusplus) */

#endif /* !defined(__WINE_BASETSD_H) */
