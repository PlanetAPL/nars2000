//***************************************************************************
//  NARS2000 -- Axis Functions
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

#define STRICT
#include <windows.h>
#include "headers.h"


//***************************************************************************
//  $CheckAxisOper
//
//  Check for axis operator
//***************************************************************************

LPTOKEN CheckAxisOper
    (LPPL_YYSTYPE lpYYFcnStr)       // Ptr to function strand

{
    if (lpYYFcnStr
     && lpYYFcnStr->TknCount > 1
     && (lpYYFcnStr[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
      || lpYYFcnStr[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
        return &lpYYFcnStr[1].tkToken;
    else
        return NULL;
} // End CheckAxisOper


//***************************************************************************
//  $CheckAxisImm
//
//  Subroutine to CheckAxis_EM for immediate values
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CheckAxisImm"
#else
#define APPEND_NAME
#endif

UBOOL CheckAxisImm
    (IMM_TYPES    immType,          // Type of the immediate value
     APLLONGEST   aplLongest,       // The immediate value
     LPTOKEN      lptkAxis,         // The Axis values
     APLRANK      aplRankCmp,       // Comparison rank
     LPUBOOL      lpbFract,         // Return TRUE iff fractional values are present,
                                    //   (may be NULL if fractional values not allowed)
     LPAPLINT     lpaplLastAxis,    // Return last axis value or ceiling if fractional
                                    //   (may be NULL if caller is not interested)
     LPAPLNELM    lpaplNELMAxis,    // Return # elements in axis
                                    //   (may be NULL if caller is not interested)
     HGLOBAL      *lphGlbAxis,      // Ptr to HGLOBAL where the cleaned up axis
                                    //   is to be stored.  If the return is FALSE,
                                    //   this ptr must be set to NULL.
                                    //   (may be NULL if caller is not interested)
     LPAPLNELM    lpaplNELM,        // Local var w/NELM
     LPAPLINT    *lplpAxisStart,    // Ptr to ptr to start of Axis values in *lphGlbAxis
     LPAPLINT    *lplpAxisHead,     // ...                    user axis values in *lphGlbAxis
     LPAPLUINT    lpaplAxisContLo,  // Contiguous low axis (not NULL)
     LPPERTABDATA lpMemPTD)         // Ptr to PertabData global memory

{
    APLUINT  ByteAxis;              // # bytes for the axis global
    APLRANK  aplRank;               // Maximum rank for comparison
    UBOOL    bRet = TRUE;           // TRUE iff result is valid
    UINT     uCnt;                  // Loop counter
    LPAPLINT lpAxisTail;            // Ptr to grade up of AxisHead
    APLBOOL  bQuadIO;               // []IO

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // The immediate value minus []IO
    //   must be an integer in the range
    //   [0, aplRankCmp - 1], inclusive.

    // Set # elements
    *lpaplNELM = 1;

    // Return the # elements
    if (lpaplNELMAxis NE NULL)
        *lpaplNELMAxis = *lpaplNELM;

    if (lphGlbAxis)
    {
        // Calculate space needed for axis
        // If the comparison rank is zero, the allocation
        //   size is zero, and the lock ptr is zero
        //   which GlobalLock will treat as an error,
        //   returning a zero ptr, so we max aplRankCmp with 1.
        ByteAxis = sizeof (APLUINT) * 2 * max (aplRankCmp, 1);

        // Check for overflow
        if (ByteAxis NE (APLU3264) ByteAxis)
            goto ERROR_EXIT;

        *lphGlbAxis = DbgGlobalAlloc (GHND, (APLU3264) ByteAxis);
        // Allocate storage for the axis vector
        if (!*lphGlbAxis)
            goto ERROR_EXIT;

        // Lock the memory to get a ptr it
        *lplpAxisStart = *lplpAxisHead = MyGlobalLock (*lphGlbAxis);

        // Point to the start of the trailing axes
        lpAxisTail = &(*lplpAxisHead)[aplRankCmp - *lpaplNELM];
    } // End IF

    // Split cases based upon the immediate axis type
    switch (lptkAxis->tkFlags.ImmType)
    {
        case IMMTYPE_BOOL:
        case IMMTYPE_INT:
            // Get the Boolean or Integer value,
            //   less the current index origin
            aplRank = aplLongest - bQuadIO;

            // Check for negative indices [-aplRankCmp, -1]
            if (SIGN_APLRANK (aplRank)
             && lpMemPTD->aplCurrentFEATURE[FEATURENDX_NEGINDICES])
                aplRank += aplRankCmp;

            // Ensure it's within range
            // Note that because aplRank and aplRankCmp
            //   are unsigned, we don't need to check
            //   for below zero
            bRet = (aplRank < aplRankCmp);

            break;

        case IMMTYPE_FLOAT:     // Ensure it's close enough
            // Attempt to convert the float to an integer using System []CT
            aplRank = FloatToAplint_SCT (lptkAxis->tkData.tkFloat, &bRet);
            aplRank -= bQuadIO; // Less the index origin

            // Check for negative indices [-aplRankCmp, -1]
            if (SIGN_APLRANK (aplRank)
             && lpMemPTD->aplCurrentFEATURE[FEATURENDX_NEGINDICES])
                aplRank += aplRankCmp;

            // If fractional values are allowed,
            //   return whether or not they are present
            if (lpbFract)
                *lpbFract = !bRet;

            // If fractional values allowed and are present, ...
            if (lpbFract && !bRet)
                bRet = TRUE;

            // Ensure it's within range
            // Note that because aplRank and aplRankCmp
            //   are unsigned, we don't need to check
            //   for below zero
            bRet = bRet && (aplRank < aplRankCmp);

            break;

        case IMMTYPE_CHAR:      // No chars allowed
            goto ERROR_EXIT;

        defstop
            goto ERROR_EXIT;
    } // End SWITCH

    if (bRet && lphGlbAxis)
    {
        // Save the remaining values
        for (uCnt = 0; uCnt < aplRankCmp; uCnt++)
        if (uCnt NE aplRank)
            *(*lplpAxisHead)++ = uCnt;

        // Save the trailing value
        *lpAxisTail++ = aplRank;
    } // End IF

    // Save the contiguous low axis
    *lpaplAxisContLo = aplRank;

    // Save the last (and only) value
    if (lpaplLastAxis)
        *lpaplLastAxis = aplRank;

    goto NORMAL_EXIT;

ERROR_EXIT:
    // Mark as in error
    bRet = FALSE;
NORMAL_EXIT:
    return bRet;
} // End CheckAxisImm
#undef  APPEND_NAME


//***************************************************************************
//  $CheckAxisGlb
//
//  Subroutine to CheckAxis_EM for global values
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CheckAxisGlb"
#else
#define APPEND_NAME
#endif

UBOOL CheckAxisGlb
    (HGLOBAL      hGlbData,         // The global handle to check
     LPTOKEN      lptkAxis,         // The Axis values
     APLRANK      aplRankCmp,       // Comparison rank
     UBOOL        bSingleton,       // TRUE iff scalar or one-element vector only
                                    //   is allowed
     UBOOL        bSortAxes,        // TRUE iff the axes should be sorted
                                    //   (i.e., the order of the axes is unimportant)
     UBOOL        bContiguous,      // TRUE iff the axes must be contiguous
     UBOOL        bAllowDups,       // TRUE iff duplicate axes are allowed
     LPUBOOL      lpbFract,         // Return TRUE iff fractional values are present,
                                    //   (may be NULL if fractional values not allowed)
     LPAPLINT     lpaplLastAxis,    // Return last axis value or ceiling if fractional
                                    //   (may be NULL if caller is not interested)
     LPAPLNELM    lpaplNELMAxis,    // Return # elements in axis
                                    //   (may be NULL if caller is not interested)
     HGLOBAL     *lphGlbAxis,       // Ptr to HGLOBAL where the cleaned up axis
                                    //   is to be stored.  If the return is FALSE,
                                    //   this ptr must be set to NULL.
                                    //   (may be NULL if caller is not interested)
     LPAPLNELM    lpaplNELM,        // Local var for NELM
     LPAPLINT    *lplpAxisStart,    // Ptr to ptr to start of Axis values in *lphGlbAxis
     LPAPLINT    *lplpAxisHead,     // ...                    user axis values in *lphGlbAxis
     LPAPLUINT    lpaplAxisContLo,  // Contiguous low axis (not NULL)
     LPPERTABDATA lpMemPTD)         // Ptr to PerTabData global memory

{
    UBOOL     bRet = TRUE;          // TRUE iff the result is valid
    LPVOID    lpMem;                // Ptr to incoming data global memory
    LPAPLBOOL lpDup = NULL;         // Ptr to duplciate axes global memory
    HGLOBAL   hGlbDup = NULL;       // Duplicate axes global memory handle
    UINT      uBitMask;             // Bit mask for looping through Booleans
    APLUINT   ByteDup,              // # bytes for the duplicate axis test
              ByteAxis,             // # bytes for the axis vector
              uCnt;                 // Loop counter
    APLSTYPE  aplTypeLcl;           // Incoming data storage type
    APLRANK   aplRankLcl;           // Incoming data rank
    LPAPLINT  lpAxisTail;           // Ptr to grade up of AxisHead
    APLBOOL   bQuadIO;              // []IO

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // st/tkData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir_PTB (hGlbData));

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbData);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)
    // Get the Array Type, NELM, and Rank
    aplTypeLcl = lpHeader->ArrType;
   *lpaplNELM  = lpHeader->NELM;
    aplRankLcl = lpHeader->Rank;
#undef  lpHeader

    // Check the axis rank and the NELM (if singletons only)
    if (IsMultiRank (aplRankLcl)
     || (bSingleton && !IsSingleton (*lpaplNELM)))
        goto ERROR_EXIT;

    // Return the # elements
    if (lpaplNELMAxis NE NULL)
        *lpaplNELMAxis = *lpaplNELM;

    if (lphGlbAxis)
    {
        // Calculate space needed for axis
        // If the comparison rank is zero, the allocation
        //   size is zero, and the lock ptr is zero
        //   which GlobalLock will treat as an error,
        //   returning a zero ptr, so we max aplRankCmp with 1.
        ByteAxis = sizeof (APLUINT) * 2 * max (aplRankCmp, 1);

        // Check for overflow
        if (ByteAxis NE (APLU3264) ByteAxis)
            goto WSFULL_EXIT;

        // Allocate storage for the axis vector
        *lphGlbAxis = DbgGlobalAlloc (GHND, (APLU3264) ByteAxis);
        if (!*lphGlbAxis)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        *lplpAxisStart = *lplpAxisHead = MyGlobalLock (*lphGlbAxis);

        // Point to the start of the trailing axes
        lpAxisTail = &(*lplpAxisHead)[aplRankCmp - *lpaplNELM];
    } // End IF

    // If the comparison rank is zero, the allocation
    //   size is zero, and the lock ptr is zero
    //   which GlobalLock will treat as an error,
    //   returning a zero ptr, so we max aplRankCmp with 1.
    ByteDup = RoundUpBitsToBytes (max (aplRankCmp, 1));

    // Check for overflow
    if (ByteDup NE (APLU3264) ByteDup)
        goto WSFULL_EXIT;

    // Allocate global memory bit vector to test for duplicates
    hGlbDup = DbgGlobalAlloc (GHND, (APLU3264) ByteDup);
    if (!hGlbDup)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to the
    //   duplicate indices testing area
    lpDup = MyGlobalLock (hGlbDup);

    // Skip over the header and dimensions to the data
    lpMem = VarArrayBaseToData (lpMem, aplRankLcl);

    // Split cases based upon the array type
    switch (aplTypeLcl)
    {
        case ARRAY_BOOL:
            uBitMask = BIT0;

            // Loop through the elements
            for (uCnt = 0; bRet && uCnt < *lpaplNELM; uCnt++)
            {
                // Get the next bit value
                aplRankLcl = (uBitMask & *(LPAPLBOOL) lpMem) ? TRUE : FALSE;
                aplRankLcl -= bQuadIO; // Less the index origin

                // Check for negative indices [-aplRankCmp, -1]
                if (SIGN_APLRANK (aplRankLcl)
                 && lpMemPTD->aplCurrentFEATURE[FEATURENDX_NEGINDICES])
                    aplRankLcl += aplRankCmp;

                // Test against the comparison rank
                // Note that because aplRankLcl and aplRankCmp
                //   are unsigned, we don't need to check
                //   for below zero
                bRet = (aplRankLcl < aplRankCmp);

                // Save the next trailing value
                //   if asked to and not sorting
                //   the axes.
                if (bRet && lphGlbAxis && !bSortAxes)
                    *lpAxisTail++ = aplRankLcl;

                // Test for duplicates
                if (bRet)
                    bRet = TestDupAxis (lpDup, aplRankLcl, bAllowDups);

                if (bRet)
                {
                    // Shift over the bit mask
                    uBitMask <<= 1;

                    // Check for end-of-byte
                    if (uBitMask EQ END_OF_BYTE)
                    {
                        uBitMask = BIT0;        // Start over
                        ((LPAPLBOOL) lpMem)++;  // Skip to next byte
                    } // End IF
                } // End IF
            } // End FOR

            break;

        case ARRAY_INT:

#define lpaplInteger    ((LPAPLINT) lpMem)

            // Loop through the elements
            for (uCnt = 0; bRet && uCnt < *lpaplNELM; uCnt++)
            {
                aplRankLcl = *lpaplInteger++ - bQuadIO;

                // Check for negative indices [-aplRankCmp, -1]
                if (SIGN_APLRANK (aplRankLcl)
                 && lpMemPTD->aplCurrentFEATURE[FEATURENDX_NEGINDICES])
                    aplRankLcl += aplRankCmp;

                // Ensure it's within range
                // Note that because aplRankLcl and aplRankCmp
                //   are unsigned, we don't need to check
                //   for below zero
                bRet = (aplRankLcl < aplRankCmp);

                // Save the next trailing value
                //   if asked to and not sorting
                //   the axes.
                if (bRet && lphGlbAxis && !bSortAxes)
                    *lpAxisTail++ = aplRankLcl;

                // Test for duplicates
                if (bRet)
                    bRet = TestDupAxis (lpDup, aplRankLcl, bAllowDups);
            } // End FOR

#undef  lpaplInteger

            break;

        case ARRAY_FLOAT:

#define lpaplFloat      ((LPAPLFLOAT) lpMem)

            // Loop through the elements
            for (uCnt = 0; bRet && uCnt < *lpaplNELM; uCnt++)
            {
                // Attempt to convert the float to an integer using System []CT
                aplRankLcl = FloatToAplint_SCT (*lpaplFloat++, &bRet);
                aplRankLcl -= bQuadIO; // Less the index origin

                // Check for negative indices [-aplRankCmp, -1]
                if (SIGN_APLRANK (aplRankLcl)
                 && lpMemPTD->aplCurrentFEATURE[FEATURENDX_NEGINDICES])
                    aplRankLcl += aplRankCmp;

                // If fractional values are allowed,
                //   return whether or not they are present
                if (lpbFract)
                    *lpbFract |= !bRet;

                // If fractional values allowed and are present, ...
                if (lpbFract && !bRet)
                    bRet = TRUE;

                // Ensure it's within range
                // Note that because aplRankLcl and aplRankCmp
                //   are unsigned, we don't need to check
                //   for below zero
                bRet = bRet && (aplRankLcl < aplRankCmp);

                // Save the next trailing value
                //   if asked to and not sorting
                //   the axes.
                if (bRet && lphGlbAxis && !bSortAxes)
                    *lpAxisTail++ = aplRankLcl;

                // Test for duplicates
                if (bRet)
                    bRet = TestDupAxis (lpDup, aplRankLcl, bAllowDups);
            } // End FOR

#undef  lpaplFloat

            break;

        case ARRAY_APA:
        {
            APLINT apaOff,
                   apaMul,
                   apaLen;

#define lpAPA       ((LPAPLAPA) lpMem)

            // Get the APA parameters
            apaOff = lpAPA->Off;
            apaMul = lpAPA->Mul;
            apaLen = *lpaplNELM;

#undef  lpAPA

            // Convert to origin-0
            apaOff -= bQuadIO;

            // It's sufficient to check the length and
            //   the first and last values for validity.
            bRet = (apaLen EQ 0)
                || (((apaOff + apaMul *           0 ) < (APLRANKSIGN) aplRankCmp)
                 && ((apaOff + apaMul * (apaLen - 1)) < (APLRANKSIGN) aplRankCmp));

            // Save the trailing axis values
            for (uCnt = 0; bRet && uCnt < (APLUINT) apaLen; uCnt++)
            {
                // Get the next value
                aplRankLcl = apaOff + apaMul * uCnt;

                // Check for negative indices [-aplRankCmp, -1]
                if (SIGN_APLRANK (aplRankLcl)
                 && lpMemPTD->aplCurrentFEATURE[FEATURENDX_NEGINDICES])
                    aplRankLcl += aplRankCmp;

                // Ensure it's within range
                // Note that because aplRankLcl and aplRankCmp
                //   are unsigned, we don't need to check
                //   for below zero
                bRet = (aplRankLcl < aplRankCmp);

                // Save the next trailing value
                //   if asked to and not sorting
                //   the axes.
                if (bRet && lphGlbAxis && !bSortAxes)
                    *lpAxisTail++ = aplRankLcl;

                // Test for duplicates
                if (bRet)
                    bRet = TestDupAxis (lpDup, aplRankLcl, bAllowDups);
            } // End FOR

            break;
        } // End ARRAY_APA

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
        case ARRAY_LIST:
            goto ERROR_EXIT;

            break;

        case ARRAY_RAT:

#define lpaplRat        ((LPAPLRAT) lpMem)

            // Loop through the elements
            for (uCnt = 0; bRet && uCnt < *lpaplNELM; uCnt++)
            {
                // Attempt to fit the RAT into an APLINT
                aplRankLcl = mpq_get_sx (lpaplRat++, &bRet);
                aplRankLcl -= bQuadIO; // Less the index origin

                // Check for negative indices [-aplRankCmp, -1]
                if (SIGN_APLRANK (aplRankLcl)
                 && lpMemPTD->aplCurrentFEATURE[FEATURENDX_NEGINDICES])
                    aplRankLcl += aplRankCmp;

                // If fractional values are allowed,
                //   return whether or not they are present
                if (lpbFract)
                    *lpbFract |= !bRet;

                // If fractional values allowed and are present, ...
                if (lpbFract && !bRet)
                    bRet = TRUE;

                // Ensure it's within range
                // Note that because aplRankLcl and aplRankCmp
                //   are unsigned, we don't need to check
                //   for below zero
                bRet = bRet && (aplRankLcl < aplRankCmp);

                // Save the next trailing value
                //   if asked to and not sorting
                //   the axes.
                if (bRet && lphGlbAxis && !bSortAxes)
                    *lpAxisTail++ = aplRankLcl;

                // Test for duplicates
                if (bRet)
                    bRet = TestDupAxis (lpDup, aplRankLcl, bAllowDups);
            } // End FOR

#undef  lpaplRat

            break;

        case ARRAY_VFP:

#define lpaplVfp        ((LPAPLVFP) lpMem)

            // Loop through the elements
            for (uCnt = 0; bRet && uCnt < *lpaplNELM; uCnt++)
            {
                // Attempt to fit the VFP into an APLINT
                aplRankLcl = mpfr_get_sx (lpaplVfp++, &bRet);
                aplRankLcl -= bQuadIO; // Less the index origin

                // Check for negative indices [-aplRankCmp, -1]
                if (SIGN_APLRANK (aplRankLcl)
                 && lpMemPTD->aplCurrentFEATURE[FEATURENDX_NEGINDICES])
                    aplRankLcl += aplRankCmp;

                // If fractional values are allowed,
                //   return whether or not they are present
                if (lpbFract)
                    *lpbFract |= !bRet;

                // If fractional values allowed and are present, ...
                if (lpbFract && !bRet)
                    bRet = TRUE;

                // Ensure it's within range
                // Note that because aplRankLcl and aplRankCmp
                //   are unsigned, we don't need to check
                //   for below zero
                bRet = bRet && (aplRankLcl < aplRankCmp);

                // Save the next trailing value
                //   if asked to and not sorting
                //   the axes.
                if (bRet && lphGlbAxis && !bSortAxes)
                    *lpAxisTail++ = aplRankLcl;

                // Test for duplicates
                if (bRet)
                    bRet = TestDupAxis (lpDup, aplRankLcl, bAllowDups);
            } // End FOR

#undef  lpaplVfp

            break;

        defstop
            break;
    } // End SWITCH

    // Save the last value
    if (lpaplLastAxis)
        *lpaplLastAxis = aplRankLcl;

    // Fill in the leading axis values
    if (bRet && lphGlbAxis && !bAllowDups)
    {
        uBitMask = BIT0;

        // Loop through the lpDup values looking for zeros
        for (uCnt = 0; uCnt < aplRankCmp; uCnt++)
        {
            // If the bit is a zero, ...
            if (!(uBitMask & *lpDup))
                // Save the next value
                *(*lplpAxisHead)++ = uCnt;
            else            // the bit is a one
            if (bSortAxes)  // and we're sorting axes
                // Fill in the trailing axis values in order
                *lpAxisTail++ = uCnt;

            // Shift over the bit mask
            uBitMask <<= 1;

            // Check for end-of-byte
            if (uBitMask EQ END_OF_BYTE)
            {
                uBitMask = BIT0;        // Start over
                lpDup++;                // Skip to next byte
            } // End IF
        } // End FOR
    } // End IF

    // If the axes must be contiguous, check that
    if (bRet && (bContiguous || bAllowDups))
    {
        // Unlock and lock the memory to reset the
        //   ptr to the start
        MyGlobalUnlock (hGlbDup); lpDup = NULL;
        lpDup = MyGlobalLock (hGlbDup);

        uBitMask = BIT0;

        // Look for the first 1
        for (uCnt = 0; uCnt < aplRankCmp; uCnt++)
        {
            // If it's a 1, break
            if (uBitMask & *lpDup)
                break;

            // Shift over the bit mask
            uBitMask <<= 1;

            // Check for end-of-byte
            if (uBitMask EQ END_OF_BYTE)
            {
                uBitMask = BIT0;        // Start over
                lpDup++;                // Skip to next byte
            } // End IF
        } // End FOR

        // Save as contiguous low axis
        *lpaplAxisContLo = uCnt;

        // Look for consecutive 1s
        for (; uCnt < aplRankCmp; uCnt++)
        {
            // If it's a 0, break
            if (!(uBitMask & *lpDup))
                break;

            // Shift over the bit mask
            uBitMask <<= 1;

            // Check for end-of-byte
            if (uBitMask EQ END_OF_BYTE)
            {
                uBitMask = BIT0;        // Start over
                lpDup++;                // Skip to next byte
            } // End IF
        } // End FOR

        // Save as highest contiguous axis
        if (lpaplLastAxis)
            *lpaplLastAxis = uCnt - 1;

        // Look for consecutive 0s
        for (; uCnt < aplRankCmp; uCnt++)
        {
            // If it's a 1, break
            if (uBitMask & *lpDup)
                break;

            // Shift over the bit mask
            uBitMask <<= 1;

            // Check for end-of-byte
            if (uBitMask EQ END_OF_BYTE)
            {
                uBitMask = BIT0;        // Start over
                lpDup++;                // Skip to next byte
            } // End IF
        } // End FOR

        // If we're not at the end, the axes
        //   are not contiguous
        bRet = (uCnt EQ aplRankCmp);
    } // End IF

    // If we allow duplicates, ...
    if (bRet && bAllowDups && aplRankCmp NE 0)
        // If so (it's slicing dyadic transpose), so the axes
        //   must be contiguous starting at []IO.
        bRet = (*lpDup & BIT0);

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

ERROR_EXIT:
    // Mark as in error
    bRet = FALSE;
NORMAL_EXIT:
    if (hGlbDup)
    {
        if (lpDup)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbDup); lpDup = NULL;
        } // End IF

        // We no longer need this HGLOBAL
        DbgGlobalFree (hGlbDup); hGlbDup = NULL;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMem = NULL;

    return bRet;
} // End CheckAxisGlb
#undef  APPEND_NAME


//***************************************************************************
//  $CheckAxis_EM
//
//  Check an axis value against a given rank
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CheckAxis_EM"
#else
#define APPEND_NAME
#endif

UBOOL CheckAxis_EM
    (LPTOKEN    lptkAxis,           // The Axis values
     APLRANK    aplRankCmp,         // Comparison rank
     UBOOL      bSingleton,         // TRUE iff scalar or one-element vector only
                                    //   is allowed
     UBOOL      bSortAxes,          // TRUE iff the axes should be sorted
                                    //   (i.e., the order of the axes is unimportant)
     UBOOL      bContiguous,        // TRUE iff the axes must be contiguous
     UBOOL      bAllowDups,         // TRUE iff duplicate axes are allowed
     LPUBOOL    lpbFract,           // Return TRUE iff fractional values are present,
                                    //   (may be NULL if fractional values not allowed)
     LPAPLINT   lpaplLastAxis,      // Return last axis value or ceiling if fractional
                                    //   (may be NULL if caller is not interested)
     LPAPLNELM  lpaplNELMAxis,      // Return # elements in axis
                                    //   (may be NULL if caller is not interested)
     HGLOBAL    *lphGlbAxis)        // Ptr to HGLOBAL where the cleaned up axis
                                    //   is to be stored.  If the return is FALSE,
                                    //   this ptr must be set to NULL.
                                    //   (may be NULL if caller is not interested)
{
    UBOOL        bRet = TRUE;       // TRUE iff the result is valid
    APLNELM      aplNELM;           //
    LPAPLINT     lpAxisStart,       // Ptr to start of Axis values in *lphGlbAxis
                 lpAxisHead;        // ...             user axis values in *lphGlbAxis
    UINT         uCnt;                 // Loop counter
    APLUINT      aplAxisContLo;     // Contiguous low axis
    HGLOBAL      hGlbData = NULL;   //
    IMM_TYPES    immType;           //
    APLLONGEST   aplLongest;        //
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // If the caller requests, an axis vector is allocated
    //   of length <2 * aplRankCmp>.  In the first <aplRankCmp>
    //   values, the trailing <*lpaplNELMAxis> values are
    //   the actual values in the axis (converted to origin-0
    //   APLINTs), and the leading values are all the other
    //   values in {iota}aplRankCmp.
    // The second <aplRankCmp> values are the grade-up of
    //   the first <aplRankCmp> values.

    // Mark as no fractional values
    if (lpbFract)
        *lpbFract = FALSE;

    // If there's no axis token, allocate storage anyway
    if (lptkAxis EQ NULL)
    {
        APLUINT   ByteAxis;         // # bytes for the axis vector
        LPAPLUINT lpMemAxis;        // Ptr to axis values, fleshed out by CheckAxis_EM

        Assert (lphGlbAxis NE NULL);

        //***************************************************************
        // Calculate space needed for axis
        //
        // If the comparison rank is zero, the allocation
        //   size is zero, and the lock ptr is zero
        //   which GlobalLock will treat as an error,
        //   returning a zero ptr, so we max aplRankCmp with 1.
        //***************************************************************
        ByteAxis = sizeof (APLUINT) * 2 * max (aplRankCmp, 1);

        //***************************************************************
        // Check for overflow
        //***************************************************************
        if (ByteAxis NE (APLU3264) ByteAxis)
            goto WSFULL_EXIT;

        //***************************************************************
        // Allocate storage for the axis vector
        //***************************************************************
        *lphGlbAxis = DbgGlobalAlloc (GHND, (APLU3264) ByteAxis);
        if (!*lphGlbAxis)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemAxis = MyGlobalLock (*lphGlbAxis);

        // Fill the memory with [0, alpRankCmp-1]
        for (uCnt = 0; uCnt < aplRankCmp; uCnt++)
            *lpMemAxis++ = uCnt;

        // Second verse same as the first
        for (uCnt = 0; uCnt < aplRankCmp; uCnt++)
            *lpMemAxis++ = uCnt;

        // We no longer need this ptr
        MyGlobalUnlock (*lphGlbAxis); lpMemAxis = NULL;

        // Save the last value
        if (lpaplLastAxis)
            *lpaplLastAxis = uCnt;

        // Return the # elements
        if (lpaplNELMAxis NE NULL)
            *lpaplNELMAxis = aplRankCmp;

        goto NORMAL_EXIT;
    } // End IF

    // Split cases based upon the axis token type
    switch (lptkAxis->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkAxis->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, it's an HGLOBAL
            if (!lptkAxis->tkData.tkSym->stFlags.Imm)
                // Get the global handle
                hGlbData = lptkAxis->tkData.tkSym->stData.stGlbData;
            else
            {
                // Get the immediate type and value
                immType    = lptkAxis->tkData.tkSym->stFlags.ImmType;
                aplLongest = lptkAxis->tkData.tkSym->stData.stLongest;
            } // End IF/ELSE

            break;

        case TKT_VARIMMED:
        case TKT_AXISIMMED:
            // Get the immediate type and value
            immType    = lptkAxis->tkFlags.ImmType;
            aplLongest = lptkAxis->tkData.tkLongest;

            break;

        case TKT_VARARRAY:
        case TKT_AXISARRAY:
            // Get the global handle
            hGlbData = lptkAxis->tkData.tkGlbData;

            break;

        defstop
            break;
    } // End SWITCH

    if (hGlbData)
        // Handle the global case
        bRet = CheckAxisGlb (hGlbData,      // The global handl;e to check
                             lptkAxis,      // The axis values
                             aplRankCmp,    // Comparison rank
                             bSingleton,    // TRUE iff scalar or one-element vector only
                                            //   is allowed
                             bSortAxes,     // TRUE iff the axes should be sorted
                                            //   (i.e., the order of the axes is unimportant)
                             bContiguous,   // TRUE iff the axes must be contiguous
                             bAllowDups,    // TRUE iff duplicate axes are allowed
                             lpbFract,      // Return TRUE iff fractional values are present,
                                            //   (may be NULL if fractional values not allowed)
                             lpaplLastAxis, // Return last axis value or ceiling if fractional
                                            //   (may be NULL if caller is not interested)
                             lpaplNELMAxis, // Return # elements in axis
                                            //   (may be NULL if caller is not interested)
                             lphGlbAxis,    // Ptr to HGLOBAL where the cleaned up axis
                                            //   is to be stored.  If the return is FALSE,
                                            //   this ptr must be set to NULL.
                                            //   (may be NULL if caller is not interested)
                            &aplNELM,       // Local var for NELM
                            &lpAxisStart,   // Ptr to ptr to start of Axis values in *lphGlbAxis
                            &lpAxisHead,    // ...                    user axis values in *lphGlbAxis
                            &aplAxisContLo, // Contiguous low axis
                             lpMemPTD);     // Ptr to PerTabData global memory
    else
        // Handle the immediate case
        bRet = CheckAxisImm (immType,
                             aplLongest,
                             lptkAxis,
                             aplRankCmp,    // Comparison rank
                             lpbFract,      // Return TRUE iff fractional values are present,
                                            //   (may be NULL if fractional values not allowed)
                             lpaplLastAxis, // Return last axis value or ceiling if fractional
                                            //   (may be NULL if caller is not interested)
                             lpaplNELMAxis, // Return # elements in axis
                                            //   (may be NULL if caller is not interested)
                             lphGlbAxis,    // Ptr to HGLOBAL where the cleaned up axis
                                            //   is to be stored.  If the return is FALSE,
                                            //   this ptr must be set to NULL.
                                            //   (may be NULL if caller is not interested)
                            &aplNELM,       // Local var for NELM
                            &lpAxisStart,   // Ptr to ptr to start of Axis values in *lphGlbAxis
                            &lpAxisHead,    // ...                    user axis values in *lphGlbAxis
                            &aplAxisContLo, // Contiguous low axis
                             lpMemPTD);     // Ptr to PerTabData global memory
    // If bad values, it's an AXIS ERROR
    if (!bRet)
    {
        if (lphGlbAxis && *lphGlbAxis)
        {
            // We no longer need this ptr
            MyGlobalUnlock (*lphGlbAxis); lpAxisStart = lpAxisHead = NULL;

            // We no longer need this storage
            DbgGlobalFree (*lphGlbAxis); *lphGlbAxis = NULL;
        } // End IF

        goto AXIS_EXIT;
    } else
    if (lphGlbAxis && *lphGlbAxis)
    {
        if (bContiguous && !bAllowDups)
        {
            // If the axes are to be contiguous and no duplicates,
            //   this must be ravel (contiguous and duplicates is transpose)
            //   in which case the axes must be represented as
            //   A{is}{iota}aplRankCmp {diamond} A[X[{gradeup}X]]{is}X
            //   not as they are above for the dyadic scalar functions.

            // We accomplish this by forming the above result in the
            //   second half of lpAxisStart, and copying it to the
            //   first half.

            // A{is}{iota}aplRankCmp
            for (uCnt = 0; uCnt < aplRankCmp; uCnt++)
                lpAxisStart[aplRankCmp + uCnt] = uCnt;

            // A[X[{gradeup}X]]{is}X
            for (uCnt = 0; uCnt < aplNELM; uCnt++)
                lpAxisStart[aplRankCmp + aplAxisContLo + uCnt] = lpAxisHead[uCnt];

            // Copy back to the first half
            CopyMemory (lpAxisStart, &lpAxisStart[aplRankCmp], (APLU3264) (aplRankCmp * sizeof (lpAxisStart[0])));
        } // End IF

        // Place the grade-up of lpAxisStart into &lpAxisStart[aplRankCmp]
        if (bAllowDups)
            GradeUp (lpAxisStart, &lpAxisStart[aplRankCmp], aplRankCmp);
        else
            // Because these values are guaranteed to be a permutation
            //   vector, we can use an address sort.
            for (uCnt = 0; uCnt < aplRankCmp; uCnt++)
                lpAxisStart[aplRankCmp + lpAxisStart[uCnt]] = uCnt;

        if (lphGlbAxis)
        {
            // We no longer need this ptr
            MyGlobalUnlock (*lphGlbAxis); lpAxisStart = lpAxisHead = NULL;
        } // End IF
    } // End IF/ELSE

    goto NORMAL_EXIT;

AXIS_EXIT:
    ErrorMessageIndirectToken (ERRMSG_AXIS_ERROR APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

ERROR_EXIT:
    // Mark as in error
    bRet = FALSE;
NORMAL_EXIT:
    return bRet;
} // End CheckAxis_EM
#undef  APPEND_NAME


//***************************************************************************
//  $TestDupAxis
//
//  Test an axis value for duplicates
//***************************************************************************

UBOOL TestDupAxis
    (LPAPLBOOL lpDup,
     APLRANK   aplRank,
     UBOOL     bAllowDups)

{
    UBOOL bRet = TRUE;
    UINT uBitMask;

    // Calculate the bit mask
    uBitMask = (BIT0 << (UINT) (aplRank % NBIB));

    // See if this value has already been seen
    if (!bAllowDups)
        bRet = !(uBitMask & lpDup[aplRank >> LOG2NBIB]);

    // Set this value for the next time if necessary
    if (bRet || bAllowDups)
        lpDup[aplRank >> LOG2NBIB] |= uBitMask;

    return bRet;
} // End TestDupAxis


//***************************************************************************
//  End of File: axisfns.c
//***************************************************************************
