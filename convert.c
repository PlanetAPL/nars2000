//***************************************************************************
//  NARS2000 -- Conversion Routines
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

#define STRICT
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "headers.h"


//***************************************************************************
//  $flt_cmp_ct
//
//  Compare two floating point values with a Comparison Tolerance
//    returning -1, 0, 1
//***************************************************************************

int flt_cmp_ct
    (APLFLOAT   aplFloatLft,        // Left arg float
     APLFLOAT   aplFloatRht,        // Right ...
     APLFLOAT   fCompTol,           // Comparison tolerance
     UBOOL      bIntegerTest)       // TRUE iff this is an integer test

{
    APLFLOAT aplLftAbs,
             aplRhtAbs,
             aplHoodLo;

    // If Lft EQ Rht (absolutely), return 0 (equal)
    if (aplFloatLft EQ aplFloatRht)
        return 0;

    // If the comparison tolerance is zero, return signum of the difference
    if (fCompTol EQ 0)
        return signumflt (aplFloatLft - aplFloatRht);

    // Get the absolute values
    aplLftAbs = fabs (aplFloatLft);
    aplRhtAbs = fabs (aplFloatRht);

    // If this is an integer test, allow comparisons with zero
    if (bIntegerTest)
    {
        if (aplFloatLft EQ 0
         && aplRhtAbs <= fCompTol)
            return 0;

        if (aplFloatRht EQ 0
         && aplLftAbs <= fCompTol)
            return 0;
    } // End IF

    // If the signs differ, return signum of the difference
    if (signumflt (aplFloatLft)
     NE signumflt (aplFloatRht))
        return signumflt (aplFloatLft - aplFloatRht);

    // Calculate the low end of the left neighborhood of (|Rht)
    // ***FIXME*** -- Handle exponent underflow in the
    //   following multiplication
    aplHoodLo = aplRhtAbs - aplRhtAbs * fCompTol;

    // If (|Rht) is greater than (|Lft),
    // and (|Lft) is in the
    //    left-neighborhood of (|Rht) with CT, return 0 (equal)
    if (aplHoodLo <= aplLftAbs
     &&              aplLftAbs < aplRhtAbs)
        return 0;

    // Calculate the low end of the left neighborhood of (|Lft)
    // ***FIXME*** -- Handle exponent underflow in the
    //   following multiplication
    aplHoodLo = aplLftAbs - aplLftAbs * fCompTol;

    // If (|Lft) is greater than (|Rht),
    // and (|Rht) is in the
    //    left-neighborhood of (|Lft) with CT, return 0 (equal)
    if (aplHoodLo <= aplRhtAbs
     &&              aplRhtAbs < aplLftAbs)
        return 0;

    // Otherwise, return the signum of the difference
    return signumflt (aplFloatLft - aplFloatRht);
} // End flt_cmp_ct


//***************************************************************************
//  $hcXY_cmp
//
//  Compare two like items
//    returning -1, 0, 1
//***************************************************************************

int hcXY_cmp
    (APLSTYPE   aplTypeCom,         // Common storage type
     LPALLTYPES lpatLft,            // Ptr to left arg as ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     APLFLOAT   fQuadCT)            // []CT (0 = Exact comparison)

{
    // Split cases based upon the common storage type
    switch (aplTypeCom)
    {
        case ARRAY_BOOL:
            return signumint (lpatLft->aplBoolean - lpatRht->aplBoolean);

        case ARRAY_INT:
            return signumint (lpatLft->aplInteger - lpatRht->aplInteger);

        case ARRAY_FLOAT:
            if (fQuadCT EQ 0.0)
                return signumflt (lpatLft->aplFloat - lpatRht->aplFloat);
            else
                return flt_cmp_ct (lpatLft->aplFloat, lpatRht->aplFloat, fQuadCT, FALSE);

        case ARRAY_CHAR:
            return signumint (lpatLft->aplChar - lpatRht->aplChar);

        case ARRAY_RAT:
            if (fQuadCT EQ 0.0)
                return signumint (mpq_cmp (&lpatLft->aplRat, &lpatRht->aplRat));
            else
                return signumint (mpq_cmp_ct (lpatLft->aplRat, lpatRht->aplRat, fQuadCT));

        case ARRAY_VFP:
            if (fQuadCT EQ 0.0)
                return signumint (mpfr_cmp (&lpatLft->aplVfp, &lpatRht->aplVfp));
            else
                return signumint (mpfr_cmp_ct (lpatLft->aplVfp, lpatRht->aplVfp, fQuadCT));

        defstop
            return 0;
    } // End SWITCH
} // End hcXY_cmp


//***************************************************************************
//  $HeNe_cmp
//
//  Compare HETERO/NESTED items
//    returning -1 (L < R)
//               0 (L == R)
//               1 (L > R)
//***************************************************************************

int HeNe_cmp
    (APLHETERO lpSymGlbLft,             // Left arg
     APLHETERO lpSymGlbRht,             // Right ...
     APLFLOAT  fQuadCT)                 // []CT (0 = Exact comparison)

{
    APLSTYPE          aplTypeLft,
                      aplTypeRht,
                      aplTypeCom;
    APLRANK           aplRankLft,
                      aplRankRht;
    APLNELM           aplNELMLft,
                      aplNELMRht;
    LPAPLDIM          lpMemDimLft,
                      lpMemDimRht;
    LPVARARRAY_HEADER lpMemHdrLft = NULL,
                      lpMemHdrRht = NULL,
                      lpMemHdrSub = NULL;
    LPVOID            lpMemSubLft,
                      lpMemSubRht;
    ALLTYPES          atLft = {0},
                      atRht = {0};
    int               iDiff = 0,
                      iDim;

    // Split cases based upon the ptr type bits
    switch (GetPtrTypeDir (lpSymGlbLft))
    {
        case PTRTYPE_STCONST:
            // Zap ptr to array header
            lpMemHdrLft = NULL;

            // Get the storage type
            aplTypeLft = TranslateImmTypeToArrayType (lpSymGlbLft->stFlags.ImmType);

            // Get the rank
            aplRankLft = 0;

            // Get the NELM
            aplNELMLft = 1;

            // Get a ptr to the dimensions
            lpMemDimLft = NULL;

            // Get a ptr to the data
            lpMemSubLft = &lpSymGlbLft->stData.stLongest;

            break;

        case PTRTYPE_HGLOBAL:
            // Get a ptr to the array header
            lpMemHdrLft = MyGlobalLockVar (lpSymGlbLft);

            // Get the storage type
            aplTypeLft = lpMemHdrLft->ArrType;

            // Get the rank
            aplRankLft = lpMemHdrLft->Rank;

            // Get the NELM
            aplNELMLft = lpMemHdrLft->NELM;

            // Get a ptr to the dimensions
            lpMemDimLft = VarArrayBaseToDim (lpMemHdrLft);

            // Get a ptr to the data
            lpMemSubLft = VarArrayDataFmBase (lpMemHdrLft);

            break;

        defstop
            break;
    } // End SWITCH

    // Split cases based upon the ptr type bits
    switch (GetPtrTypeDir (lpSymGlbRht))
    {
        case PTRTYPE_STCONST:
            // Zap ptr to array header
            lpMemHdrRht = NULL;

            // Get the storage type
            aplTypeRht = TranslateImmTypeToArrayType (lpSymGlbRht->stFlags.ImmType);

            // Get the rank
            aplRankRht = 0;

            // Get the NELM
            aplNELMRht = 1;

            // Get a ptr to the dimensions
            lpMemDimRht = NULL;

            // Get a ptr to the data
            lpMemSubRht = &lpSymGlbRht->stData.stLongest;

            break;

        case PTRTYPE_HGLOBAL:
            // Get a ptr to the array header
            lpMemHdrRht = MyGlobalLockVar (lpSymGlbRht);

            // Get the storage type
            aplTypeRht = lpMemHdrRht->ArrType;

            // Get the rank
            aplRankRht = lpMemHdrRht->Rank;

            // Get the NELM
            aplNELMRht = lpMemHdrRht->NELM;

            // Get a ptr to the dimensions
            lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);

            // Get a ptr to the data
            lpMemSubRht = VarArrayDataFmBase (lpMemHdrRht);

            break;

        defstop
            break;
    } // End SWITCH

    // If the ranks are different, ...
    if (aplRankLft NE aplRankRht)
    {
        iDiff = signumint (aplRankLft - aplRankRht);

        goto NORMAL_EXIT;
    } else
    {
        // If the leading dimensions differ, ...
        for (iDim = 0; iDim < aplRankLft; iDim++)
        if (lpMemDimLft[iDim] NE lpMemDimRht[iDim])
        {
            iDiff = signumint (lpMemDimLft[iDim] - lpMemDimRht[iDim]);

            goto NORMAL_EXIT;
        } // End FOR/IF
    } // End IF/ELSE

    // If the storage types differ (Numeric vs. Char), ...
    if (IsNumeric (aplTypeLft) && IsSimpleChar (aplTypeRht))
        // Num < Char
        iDiff = -1;
    else
    // If the storage types differ (Char vs. Numeric), ...
    if (IsSimpleChar (aplTypeLft) && IsNumeric (aplTypeRht))
        // Char > Num
        iDiff =  1;
    else
    if ((IsNumeric    (aplTypeLft) && IsNumeric    (aplTypeRht))
     || (IsSimpleChar (aplTypeLft) && IsSimpleChar (aplTypeRht)))
    {
        Assert (aplNELMLft EQ aplNELMRht);

        // Calculate the common var storage type
        aplTypeCom = aTypePromote[aplTypeLft][aplTypeRht];

        // Loop through the elements
        for (iDim = 0; iDim < aplNELMLft; iDim++)
        {
            // Convert the left arg item to a common storage type
            (*aTypeActPromote[aplTypeLft][aplTypeCom]) (lpMemSubLft, iDim, &atLft);

            // Convert the right arg item to a common storage type
            (*aTypeActPromote[aplTypeRht][aplTypeCom]) (lpMemSubRht, iDim, &atRht);

            // Compare the two items
            iDiff = hcXY_cmp (aplTypeCom, &atLft, &atRht, fQuadCT);

            // Free the old atLft and atRht
            (*aTypeFree[aplTypeCom]) (&atLft, 0);
            (*aTypeFree[aplTypeCom]) (&atRht, 0);

            if (iDiff NE 0)
                break;
        } // End FOR
    } else
    // One or both items are HETERO/NESTED
    {
        // Sp[lit cases based upon the immediate status
        switch (2 * (lpMemHdrLft EQ NULL)
              + 1 * (lpMemHdrRht EQ NULL))
        {
            case 2 * 0 + 1 * 0:         // Neither is STCONST
                // Loop through the elements
                for (iDim = 0; iDim < aplNELMLft; iDim++)
                {
                    // Call recursively
                    iDiff = HeNe_cmp (((LPAPLNESTED) lpMemSubLft)[iDim],
                                      ((LPAPLNESTED) lpMemSubRht)[iDim], fQuadCT);
                    if (iDiff NE 0)
                        break;
                } // End FOR

                break;

            case 2 * 0 + 1 * 1:         // Right   is STCONST
                // Save stLongest
                atRht.aplLongest = *(LPAPLLONGEST) lpMemSubRht;

                // Call common routine, negate result
                iDiff = -HeNe_cmpsub (&atRht,       // Ptr to STE's ALLTYPES (filled in with value)
                                       aplTypeRht,  // STE's array storage type
                                      &atLft,       // Ptr to GLB's ALLTYPES (empty)
                        *(LPAPLNESTED) lpMemSubLft, // GLB's global memory handle
                                       fQuadCT);    // []CT
                break;

            case 2 * 1 + 1 * 0:         // Left    is STCONST
                // Save stLongest
                atLft.aplLongest = *(LPAPLLONGEST) lpMemSubLft;

                // Call common routine
                iDiff =  HeNe_cmpsub (&atLft,       // Ptr to STE's ALLTYPES (filled in with value)
                                       aplTypeLft,  // STE's array storage type
                                      &atRht,       // Ptr to GLB's ALLTYPES (empty)
                        *(LPAPLNESTED) lpMemSubRht, // GLB's global memory handle
                                       fQuadCT);    // []CT
                break;
            case 2 * 1 + 1 * 1:         // Both   are STCONST
            {
                IMM_TYPES immTypeLft = lpSymGlbLft->stFlags.ImmType,
                          immTypeRht = lpSymGlbRht->stFlags.ImmType;

                // If the storage types differ (Numeric vs. Char), ...
                if (IsImmNum (immTypeLft) && IsImmChr (immTypeRht))
                    // Num < Char
                    iDiff = -1;
                else
                // If the storage types differ (Char vs. Numeric), ...
                if (IsImmChr (immTypeLft) && IsImmNum (immTypeRht))
                    // Char > Num
                    iDiff =  1;
                else
                // If the storage types are both char, ...
                if (IsImmChr (immTypeLft) && IsImmChr (immTypeRht))
                    iDiff = lpSymGlbLft->stData.stChar -
                            lpSymGlbRht->stData.stChar;
                else
                {
                    // Translate to array storage type
                    aplTypeLft = TranslateImmTypeToArrayType (immTypeLft);
                    aplTypeRht = TranslateImmTypeToArrayType (immTypeRht);

                    // Calculate the common var storage type
                    aplTypeCom = aTypePromote[aplTypeLft][aplTypeRht];

                    // Promote to common storage type
                    (*aTypeActPromote[aplTypeLft][aplTypeCom]) (&lpSymGlbLft->stData.stLongest, 0, &atLft);
                    (*aTypeActPromote[aplTypeRht][aplTypeCom]) (&lpSymGlbRht->stData.stLongest, 0, &atRht);

                    // Compare the two items
                    iDiff = hcXY_cmp (aplTypeCom, &atLft, &atRht, fQuadCT);

                    // Free the old atLft and atRht
                    (*aTypeFree[aplTypeCom]) (&atLft, 0);
                    (*aTypeFree[aplTypeCom]) (&atRht, 0);
                } // End IF/ELSE/...

                break;
            } // End

            defstop
                break;
        } // End SWITCH
    } // End IF/ELSE/...
NORMAL_EXIT:
    // If the left item is locked, ...
    if (lpMemHdrLft NE NULL)
    {
        MyGlobalUnlock (lpSymGlbLft); lpSymGlbLft = NULL;
    } // End IF

    // If the right item is locked, ...
    if (lpMemHdrRht NE NULL)
    {
        MyGlobalUnlock (lpSymGlbRht); lpSymGlbRht = NULL;
    } // End IF

    return iDiff;
} // End HeNe_cmp


//***************************************************************************
//  $HeNe_cmpsub
//
//  Comparison subroutine to HeNe_cmp
//
//  Note that it returns the difference of STE-GLB, so if you need
//    GLB-STE, negate the result.
//***************************************************************************

int HeNe_cmpsub
    (LPALLTYPES lpatSte,                // Ptr to STE's ALLTYPES (filled in with value)
     APLSTYPE   aplTypeSte,             // STE's array storage type
     LPALLTYPES lpatGlb,                // Ptr to GLB's ALLTYPES (empty)
     HGLOBAL    hGlb,                   // GLB's global memory handle
     APLFLOAT   fQuadCT)                // []CT

{
    APLSTYPE          aplTypeCom;       // Common array storage type
    LPVARARRAY_HEADER lpMemHdr = NULL;  // Ptr to item header
    LPVOID            lpMem;            // Ptr to item global memory
    int               iDiff;            // The result

    // Lock the memory to get a ptr to it
    lpMemHdr = MyGlobalLockVar (hGlb);

    // Skip over the headers and dimensions to the data
    lpMem = VarArrayDataFmBase (lpMemHdr);

    // Calculate the common array storage type
    aplTypeCom = aTypePromote[lpMemHdr->ArrType][aplTypeSte];

    // Promote the STE and GLB to common array storage type
    (*aTypeActPromote[aplTypeSte       ][aplTypeCom]) (lpatSte, 0, lpatSte);
    (*aTypeActPromote[lpMemHdr->ArrType][aplTypeCom]) (lpMem  , 0, lpatGlb);

    // Compare the two items
    iDiff = hcXY_cmp (aplTypeCom, lpatSte, lpatGlb, fQuadCT);

    // Free the old atGlb
    (*aTypeFree[aplTypeCom]) (lpatGlb, 0);

    // We no longer need this ptr
    MyGlobalUnlock (hGlb); lpMemHdr = NULL;

    return iDiff;
} // End HeNe_cmpsub


//***************************************************************************
//  $FloatToAplint_CT
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
    if (signumflt (aplFloatLft)
     NE signumflt (aplFloatRht))
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
//  $ConvertToInteger_SCT
//
//  Convert a value to an integer (if possible) using System []CT
//***************************************************************************

APLINT ConvertToInteger_SCT
    (APLSTYPE   aplTypeArg,             // Argument storage type
     LPVOID     lpMemArg,               // ...      global memory ptr
     APLUINT    uArg,                   // Index into <lpMemArg>
     LPUBOOL    lpbRet)                 // Ptr to TRUE iff the result is valid

{
    LPSYMENTRY lpSymEntry;              // Ptr to STE
//  ALLTYPES atArg = {0};

    // Mark as using SYS_CT
//  atArg.enumCT = ENUMCT_SYS;

    // Attempt to convert the value in <lpSymGlbArg> to an INT using System []CT
//  (*aTypeActConvert[aplTypeArg][ARRAY_INT]) (lpSymGlbArg, uArg, &atArg, lpbRet);

    // Split cases based upon the arg storage type
    switch (aplTypeArg)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            *lpbRet = TRUE;

            return GetNextInteger (lpMemArg, aplTypeArg, uArg);

        case ARRAY_FLOAT:
            return FloatToAplint_SCT (((LPAPLFLOAT) lpMemArg)[uArg], lpbRet);

        case ARRAY_RAT:
            // Attempt to convert the RAT to an integer using System []CT
            return mpq_get_sctsx  (&((LPAPLRAT) lpMemArg)[uArg], lpbRet);

        case ARRAY_VFP:
            // Attempt to convert the VFP to an integer using System []CT
            return mpfr_get_sctsx (&((LPAPLVFP) lpMemArg)[uArg], lpbRet);

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Get the element in question
            lpSymEntry = ((LPAPLHETERO) lpMemArg)[uArg];

            // Split cases based upon the ptr type bits
            switch (GetPtrTypeDir (lpSymEntry))
            {
                case PTRTYPE_STCONST:
                    return
                      ConvertToInteger_SCT (TranslateImmTypeToArrayType (lpSymEntry->stFlags.ImmType),
                                           &lpSymEntry->stData.stLongest,
                                            0,
                                            lpbRet);
                case PTRTYPE_HGLOBAL:
                    break;

                defstop
                    break;
            } // End SWITCH

            *lpbRet = FALSE;

            return 0;

        defstop
            return 0;
    } // End SWITCH
} // End ConvertToInteger_SCT


//***************************************************************************
//  End of File: convert.c
//***************************************************************************
