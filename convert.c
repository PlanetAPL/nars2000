//***************************************************************************
//  NARS2000 -- Conversion Routines
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2014 Sudley Place Software

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

#define STRICT
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "headers.h"


//***************************************************************************
//  $FloatToAplint_CT
//
//  Attempt to convert a Floating Point number to an APLINT
//    using Comparison Tolerance
//***************************************************************************

APLINT FloatToAplint_CT
    (APLFLOAT fFloat,       // The number to convert
     APLFLOAT fQuadCT,      // Comparison tolerance to use
     LPUBOOL  lpbRet)       // TRUE iff successful conversion
                            // (may be NULL if the caller isn't interested)
{
    return _FloatToAplint_CT (fFloat, fQuadCT, lpbRet, FALSE);
} // End FloatToAplint_CT


//***************************************************************************
//  $_FloatToAplint_CT
//
//  Attempt to convert a Floating Point number to an APLINT
//    using Comparison Tolerance
//***************************************************************************

APLINT _FloatToAplint_CT
    (APLFLOAT fFloat,       // The number to convert
     APLFLOAT fQuadCT,      // Comparison tolerance to use
     LPUBOOL  lpbRet,       // TRUE iff successful conversion
                            // (may be NULL if the caller isn't interested)
     UBOOL    bIntegerTest) // TRUE iff this is an integer test
{
    APLINT aplInteger;
    UBOOL  bRet;

    if (lpbRet EQ NULL)
        lpbRet = &bRet;

    // Convert to an integer (rounding down)
    aplInteger = (APLINT) floor (fFloat);

    // See how the number and its tolerant floor compare
    if (_CompareCT (fFloat, (APLFLOAT) aplInteger, fQuadCT, NULL, bIntegerTest))
    {
        *lpbRet = TRUE;

        return aplInteger;
    } // End IF

    // Convert to an integer (rounding up)
    aplInteger = (APLINT) ceil (fFloat);

    // See how the number and its tolerant ceiling compare
    if (_CompareCT (fFloat, (APLFLOAT) aplInteger, fQuadCT, NULL, bIntegerTest))
    {
        *lpbRet = TRUE;

        return aplInteger;
    } // End IF

    // It's not close enough, so we failed
    *lpbRet = FALSE;

    // Return the ceiling of the fractional value
    // The ceiling is important in CheckAxis for laminate
    return aplInteger;
} // End _FloatToAplint_CT


//***************************************************************************
//  $FloatToAplint_SCT
//
//  Attempt to convert a Floating Point number to an APLINT
//    using System Comparison Tolerance
//***************************************************************************

APLINT FloatToAplint_SCT
    (APLFLOAT fFloat,       // The number to convert
     LPUBOOL  lpbRet)       // TRUE iff successful conversion
                            // (may be NULL if the caller isn't interested)
{
    // Floats at or above 2*53 are by definition non-integral
    if (fabs (fFloat) >= Float2Pow53)
    {
        if (lpbRet)
            // Mark as non-integral
            *lpbRet = FALSE;

        return (APLINT) fFloat;
    } else
        return _FloatToAplint_CT (fFloat, SYS_CT, lpbRet, TRUE);
} // End FloatToAplint_SCT


//***************************************************************************
//  $CompareCT
//
//  Compare two floating point values with a Comparison Tolerance
//***************************************************************************

APLBOOL CompareCT
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     APLFLOAT   fCompTol,
     LPPRIMSPEC lpPrimSpec)

{
    return _CompareCT (aplFloatLft,
                       aplFloatRht,
                       fCompTol,
                       lpPrimSpec,
                       FALSE);
} // End CompareCT


//***************************************************************************
//  $_CompareCT
//
//  Compare two floating point values with a Comparison Tolerance
//***************************************************************************

APLBOOL _CompareCT
    (APLFLOAT   aplFloatLft,        // Left arg float
     APLFLOAT   aplFloatRht,        // Right ...
     APLFLOAT   fCompTol,           // Comparison tolerance
     LPPRIMSPEC lpPrimSpec,         // Ptr to local PRIMSPEC
     UBOOL      bIntegerTest)       // TRUE iff this is an integer test

{
    APLFLOAT aplLftAbs,
             aplRhtAbs,
             aplHoodLo;

    // If Lft EQ Rht (absolutely), return TRUE
    if (aplFloatLft EQ aplFloatRht)
        return TRUE;

    // If the comparison tolerance is zero, return FALSE
    if (fCompTol EQ 0)
        return FALSE;

    // Get the absolute values
    aplLftAbs = PrimFnMonStileFisF (aplFloatLft, lpPrimSpec);
    aplRhtAbs = PrimFnMonStileFisF (aplFloatRht, lpPrimSpec);

    // If this is an integer test, allow comparisons with zero
    if (bIntegerTest)
    {
        if (aplFloatLft EQ 0
         && aplRhtAbs <= fCompTol)
            return TRUE;

        if (aplFloatRht EQ 0
         && aplLftAbs <= fCompTol)
            return TRUE;
    } // End IF

    // If the signs differ, return FALSE
    if (signumf (aplFloatLft)
     NE signumf (aplFloatRht))
        return FALSE;

    // Calculate the low end of the left neighborhood of (|Rht)
    // ***FIXME*** -- Handle exponent underflow in the
    //   following multiplication
    aplHoodLo = aplRhtAbs - aplRhtAbs * fCompTol;

    // If (|Rht) is greater than (|Lft),
    // and (|Lft) is in the
    //    left-neighborhood of (|Rht) with CT, return 1
    if (aplHoodLo <= aplLftAbs
     &&              aplLftAbs < aplRhtAbs)
        return TRUE;

    // Calculate the low end of the left neighborhood of (|Lft)
    // ***FIXME*** -- Handle exponent underflow in the
    //   following multiplication
    aplHoodLo = aplLftAbs - aplLftAbs * fCompTol;

    // If (|Lft) is greater than (|Rht),
    // and (|Rht) is in the
    //    left-neighborhood of (|Lft) with CT, return 1
    if (aplHoodLo <= aplRhtAbs
     &&              aplRhtAbs < aplLftAbs)
        return TRUE;

    return FALSE;
} // End _CompareCT


//***************************************************************************
//  $IntFloatToAplchar
//
//  Convert an integer/floating point number to APLCHARs
//***************************************************************************

void IntFloatToAplchar
    (LPAPLCHAR    lpMemRes,         // Ptr to output save area
     LPAPLLONGEST lpaplLongest)     // Ptr to value to convert

{
    int i;

#define ifta        ((LPCHAR) lpaplLongest)

    for (i = 7; i >= 0; i--)
    {
        *lpMemRes++ = L"0123456789ABCDEF"[(ifta[i] & 0xF0) >> 4];
        *lpMemRes++ = L"0123456789ABCDEF"[(ifta[i] & 0x0F) >> 0];
    } // End FOR
#undef  ifta
} // End IntFloatToAplchar


//// //***************************************************************************
//// //  $sprintfW
//// //
//// //  sprintf for wide chars
//// //***************************************************************************
////
//// int sprintfW
////     (LPAPLCHAR lpwszOut,
////      LPAPLCHAR lpwszFmt,
////      ...)
////
//// {
////     va_list vl;
////     int     i1, i2, i3, i4,
////             iLen;
////     char    szFmt[128],
////             szTemp[1024];
////
////     // We hope that no one calls us with more than
////     //   four arguments.
////     // Note we must grab them separately this way
////     //   as using va_arg in the argument list to
////     //   wsprintf pushes the arguments in reverse
////     //   order.
////     va_start (vl, lpwszFmt);
////
////     i1 = va_arg (vl, int);
////     i2 = va_arg (vl, int);
////     i3 = va_arg (vl, int);
////     i4 = va_arg (vl, int);
////
////     va_end (vl);
////
////     // Convert the format string to ASCII
////     W2A (szFmt, lpwszFmt, sizeof (szFmt) - 1);
////
////     // Format the data
////     iLen = sprintf (szTemp,
////                     szFmt,
////                     i1, i2, i3, i4);
////
////     // Convert the result back to wide chars
////     // The destin buffer length is a guess
////     A2W (lpwszOut, szTemp, 4096);
////
////     return iLen;
//// } // End sprintfW


//***************************************************************************
//  $ConvertWideToName
//
//  Convert wide chars to ASCII or {name} or {\xXXXX}
//***************************************************************************

APLU3264 ConvertWideToName
    (LPWCHAR lpwszOut,              // Ptr to output save buffer
     LPWCHAR lpwszInp)              // Ptr to incoming chars

{
    return ConvertWideToNameLength (lpwszOut,               // Ptr to output save buffer
                                    lpwszInp,               // Ptr to incoming chars
                                    lstrlenW (lpwszInp));   // # chars to convert
} // End ConvertWideToName


//***************************************************************************
//  $ConvertWideToNameLength
//
//  Convert wide chars of a given length to ASCII or {name} or {\xXXXX}
//***************************************************************************

APLU3264 ConvertWideToNameLength
    (LPWCHAR  lpwszOut,             // Ptr to output save buffer
     LPWCHAR  lpwszInp,             // Ptr to incoming chars
     APLU3264 uLen)                 // # chars to convert

{
    WCHAR   wc;                     // A wide char
    LPWCHAR lpwCharName,            // Ptr to char name
            lpwsz;                  // Ptr to output save area

    // Initialize the ptr to the output save area
    lpwsz = lpwszOut;

    // Loop through the wide chars
    while (uLen--)
    {
        // Get the next char
        wc = *lpwszInp++;

        if (32 < wc && wc <= 0x7E
         && wc NE WC_SQ         // Used to surround 'a'
         && wc NE L'#'
         && wc NE L'{'          // Used to surround {symbols}
         && wc NE L'}'          // ...
         && wc NE WC_SLOPE)     // Used in <iniparser_load> for multiline support
            *lpwsz++ = wc;
        else
        // Check for name in hash table
        if (wc NE WC_EOS
         && (lpwCharName = CharToSymbolName (wc)) NE NULL)
            lpwsz += wsprintfW (lpwsz,
                                L"%s",
                                lpwCharName);
        else
            lpwsz += wsprintfW (lpwsz,
                                L"{\\x%04X}",
                                wc);
    } // End WHILE

    // Ensure properly terminated
    *lpwsz = WC_EOS;

    return (APLU3264) (lpwsz - lpwszOut);
} // End ConvertWideToNameLength


//***************************************************************************
//  $FormatCurDateTime
//
//  Format the current date & time as "dd/mm/yyyy hh:mm:ss"
//***************************************************************************

void FormatCurDateTime
    (LPAPLCHAR wszTemp)

{
    SYSTEMTIME SystemTime;

    // Get the current date & time
    if (OptionFlags.bUseLocalTime)
        GetLocalTime  (&SystemTime);
    else
        GetSystemTime (&SystemTime);

    // Format it
    wsprintfW (wszTemp,
               DATETIME_FMT,
               SystemTime.wMonth,
               SystemTime.wDay,
               SystemTime.wYear,
               SystemTime.wHour,
               SystemTime.wMinute,
               SystemTime.wSecond);
} // End FormatCurDateTime


//***************************************************************************
//  $ConvertNameInPlace
//
//  Convert the {name}s and other chars to UTF16_xxx equivalents
//***************************************************************************

LPWCHAR ConvertNameInPlace
    (LPWCHAR lpwSrc)            // Ptr to string to convert

{
    LPWCHAR lpwDst = lpwSrc;
    WCHAR   wcTmp;

    while (*lpwSrc)
    if (*lpwSrc EQ L'{')
    {
        // Get the next char
        wcTmp = SymbolNameToChar (lpwSrc);

        // If there's a matching UTF16_xxx equivalent, ...
        if (wcTmp)
        {
            // Replace {name} with UTF16_xxx equivalent
            *lpwDst++ = wcTmp;

            // Skip over the {name}
            lpwSrc = SkipPastCharW (lpwSrc, L'}');
        } else
        {
            // Copy source to the destin up to and including the matching '}'
            while (*lpwSrc NE L'}')
                *lpwDst++ = *lpwSrc++;

            // Copy the '}'
            *lpwDst++ = *lpwSrc++;
        } // End IF/ELSE
    } else
        *lpwDst++ = *lpwSrc++;

    // Ensure properly terminated
    *lpwDst = WC_EOS;

    return lpwSrc;
} // End ConvertNameInPlace


//***************************************************************************
//  End of File: convert.c
//***************************************************************************
