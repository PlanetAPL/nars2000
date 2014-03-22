//***************************************************************************
//  NARS2000 -- Fast Boolean Reduction and Scan Functions
//
//  The design of this implementation was taken from
//    "The Caret Functions", by E. E. McDonnell,
//    Proceedings of the sixth international conference on APL,
//    1974, pp. 316-323.
//
//  Many thanks Gene!
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2009 Sudley Place Software

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


// Define common macro
#define CHECK_NEXT_RESULT_BYTE(lpmemRes,uDimRes)                                \
            /* Check to see if we need to skip to the next result byte */       \
            if (uDimRes EQ NBIB)                                                \
            {                                                                   \
                uDimRes = 0;                /* Start over again               */\
                ((LPAPLBOOL) lpMemRes)++;   /* Skip to next byte in the result*/\
            } // End IF


//***************************************************************************
//  $FastBoolScan
//
//  Fast Boolean Scan for the eight Boolean functions
//***************************************************************************

void FastBoolScan
    (APLSTYPE     aplTypeRht,       // Right arg storage type
     APLNELM      aplNELMRht,       // Right arg NELM
     LPVOID       lpMemRht,         // Ptr to right arg memory
     LPVOID       lpMemRes,         // Ptr to result    memory
     APLUINT      uDimLo,           // Product of dimensions below axis
     APLUINT      uDimAxRht,        // Length of right arg axis dimension
     FBFN_INDS    uFBFNIndex,       // FBFN_INDS value (e.g., index into FastBoolFns[])
     LPPL_YYSTYPE lpYYFcnStrOpr)    // Ptr to operator function strand

{
    APLUINT       uLo,              // Loop counter for uDimLo
                  uAx,              // Loop counter for uDimAxRht
                  uDimRht,          // Right arg starting index
                  uDimRes = 0,      // Result    ...
                  uIndex,           // Index of marker bit in vector
                  uBitCountTotal;   // Count of bits processed overall
    UCHAR         uByteMask,        // Byte mask
                  uByteMaskIni,     // Initial byte mask based on uDimAxRht
                  uBitsInMask,      // # bits in the mask
                  uInfix,           // Infix bit value
                  uSuffix,          // Suffix equivalence value
                  uIdentElem,       // Identity element (if it exists)
                  uNotMarker;       // Complement of Marker
    FASTBOOLTRANS fbtIndex;         // FBT_FIRST1 or 0
    LPAPLBOOL     lpaplBool;        // Ptr to Booleans to reduce

    // Get the parameters specific to this function
    uNotMarker = FastBoolFns[uFBFNIndex].NotMarker;     // J[0]
    uIdentElem = FastBoolFns[uFBFNIndex].IdentElem;     // J[1]
    uSuffix    = FastBoolFns[uFBFNIndex].Suffix;        // J[2]
    fbtIndex   = fbtFirst[!uNotMarker];                 // FBT_FIRST1 or 0

    // If the right arg is an APA, handle it specially
    if (IsSimpleAPA (aplTypeRht))
    {
        // The possibilities are as follows (where V can be any non-negative integer vector):
        //   1.  V   {rho}0   or 1
        //   2.  V 2 {rho}0 1 or 1 0 as an APA
        //       (presumably coming from V 2{rho}{iota}2 in origin-0
        //        and which was type demoted to Boolean -- as far as
        //        I know, this doesn't happen, but you never know)
        // The difference between the above two cases is in the APA
        //   multiplier -- in the first case it's zero, and in the
        //   second case it's either 1 or -1.

#define lpAPA       ((LPAPLAPA) lpMemRht)

        Assert (IsMultiNELM (aplNELMRht));
        Assert (lpAPA->Mul EQ 0);

        // Check the APA multiplier
        if (lpAPA->Mul EQ 0)
        {
#define uDimHi      1       // This is a requirement for all Fast Boolean operations
#define uHi         0       // A consequence of removing the above loop
            // Because the APA has all the same element (lpAPA->Off),
            //   we find it either in the first position or not at all
            if (lpAPA->Off EQ !uNotMarker)  // If equal, we find it in the first position (uIndex EQ 0)
            {
                // Set the infix bit
                uInfix = !uNotMarker;       // Because the index is in position 0

                // The marker is found in the first position:  index is 0
                // The first bit is uInfix, and the remainder of the bits are all uInfix EQ uSuffix
                if (uInfix || (uInfix EQ uSuffix))  // Same as uInfix >= uSuffix
                for (uLo = 0; uLo < uDimLo; uLo++)
                {
                    // Calculate the starting index in the right arg/result of this vector
                    uDimRht = uLo * uDimHi * uDimAxRht + uHi;

                    *((LPAPLBOOL) lpMemRes) |= uInfix << uDimRes++;
                    uIndex = (uInfix EQ uSuffix);
                    for (uAx = 1; uAx < uDimAxRht; uAx++)
                    {
                        *((LPAPLBOOL) lpMemRes) |= uIndex << uDimRes++;

                        // Check to see if we need to skip to the next result byte
                        CHECK_NEXT_RESULT_BYTE (lpMemRes, uDimRes);
                    } // End FOR
                } // End FOR
            } else
            {
                // The marker is not found:  index is uDimAxRht
                // Fill with the pair of bits (uNotMarker, uIdentElem)
                if (uNotMarker || uIdentElem)
                for (uLo = 0; uLo < uDimLo; uLo++)
                {
                    // Calculate the starting index in the right arg/result of this vector
                    uDimRht = uLo * uDimHi * uDimAxRht + uHi;

                    for (uAx = 0; uAx < uDimAxRht;)
                    {
                        if (uAx++ < uDimAxRht)
                            *((LPAPLBOOL) lpMemRes) |= uNotMarker << uDimRes++;

                        // Check to see if we need to skip to the next result byte
                        CHECK_NEXT_RESULT_BYTE (lpMemRes, uDimRes);

                        if (uAx++ < uDimAxRht)
                            *((LPAPLBOOL) lpMemRes) |= uIdentElem << uDimRes++;

                        // Check to see if we need to skip to the next result byte
                        CHECK_NEXT_RESULT_BYTE (lpMemRes, uDimRes);
                    } // End FOR
                } // End FOR
            } // End IF/ELSE
#undef  uHi
#undef  uDimHi
#undef  lpAPA
        } else
            DbgStop ();         // We should never get here
        return;         // We're done
    } // End IF

    // Calculate the mask for the first byte
    uByteMaskIni = (UCHAR) ((BIT0 << min (uDimAxRht, NBIB)) - 1);

    // Get ptr to first byte
    lpaplBool = (LPAPLBOOL) lpMemRht;

    // Loop through the right arg looking for the first marker bit in each vector
    for (uBitCountTotal = uLo = 0;
         uLo < uDimLo;
         uLo++, uBitCountTotal = uLo * uDimAxRht)
#define uDimHi      1       // This is a requirement for all Fast Boolean operations
////for (uHi = 0; uHi < uDimHi; uHi++)      // uDimHi EQ 1, so this loop is unnecessary
#define uHi         0       // A consequence of removing the above loop
    {
        UCHAR   uByte,                  // The current byte (from lpaplBool)
                uFirstBitIndexInByte;   // Index of first marker bit in the byte
        APLUINT uBitCountVector = 0;    // Count of # bits processed so far in this vector

        // Calculate the starting index in the right arg of this vector
        uDimRht = uLo * uDimHi * uDimAxRht + uHi;

        // Calculate the byte mask
        uByteMask = (UCHAR) (uByteMaskIni << (MASKLOG2NBIB & uDimRht));
        uBitsInMask = FastBoolTrans[uByteMask][FBT_BITSUM];

        // Get the byte in question, shifted to the low-order
        uByte = (UCHAR) ((uByteMask & lpaplBool[uBitCountTotal >> LOG2NBIB]) >> (MASKLOG2NBIB & uDimRht));

        // Find the first marker bit in uByte
        uFirstBitIndexInByte = FastBoolTrans[uByte][fbtIndex];
        uFirstBitIndexInByte = min (uFirstBitIndexInByte, uBitsInMask);

        // Update the # bits processed so far
        uBitCountTotal += uBitsInMask;

        // If not found so far, ...
        if (uFirstBitIndexInByte EQ uBitsInMask)
        {
            // So far, we've processed this many bits w/o a hit
            uBitCountVector = uBitsInMask;
            uFirstBitIndexInByte = 0;

            // If there are more bits in this vector, ...
            while (uDimAxRht > uBitCountVector)
            {
                uByteMask = (UCHAR) ((BIT0 << (min (uDimAxRht - uBitCountVector, NBIB))) - 1);
                uBitsInMask = FastBoolTrans[uByteMask][FBT_BITSUM];

                // Get the byte in question
                uByte = uByteMask & lpaplBool[uBitCountTotal >> LOG2NBIB];

                // Find the first marker bit in uByte
                uFirstBitIndexInByte = FastBoolTrans[uByte][fbtIndex];
                uFirstBitIndexInByte = min (uFirstBitIndexInByte, uBitsInMask);

                // Update the # bits processed so far
                uBitCountTotal += uBitsInMask;

                // If not found, ...
                if (uFirstBitIndexInByte EQ uBitsInMask)
                {
                    uBitCountVector += uBitsInMask;
                    uFirstBitIndexInByte = 0;
                } else
                    break;
            } // End IF
        } // End IF

        // Get the index of the first marker bit in the vector (uDimAxRht if not found)
        uIndex = uBitCountVector + uFirstBitIndexInByte;

        // Set the infix bit
        uInfix = !((BIT0 & uIndex) ? uIdentElem : uNotMarker);

        // Ensure within range
        Assert (uIndex <= uDimAxRht);

        // Save in the result
        // Fill with a PREFIX of uIndex bits alternating between uNotMarker and uIdentElem
        uAx = 0;
        while (TRUE)
        {
            // If more bits to fill, ...
            if (uAx < uIndex)
            {
                uAx++;      // Count in another bit filled
                *((LPAPLBOOL) lpMemRes) |= uNotMarker << uDimRes++;

                // Check to see if we need to skip to the next result byte
                CHECK_NEXT_RESULT_BYTE (lpMemRes, uDimRes);
            } else
                break;

            // If more bits to fill, ...
            if (uAx < uIndex)
            {
                uAx++;      // Count in another bit filled
                *((LPAPLBOOL) lpMemRes) |= uIdentElem << uDimRes++;

                // Check to see if we need to skip to the next result byte
                CHECK_NEXT_RESULT_BYTE (lpMemRes, uDimRes);
            } else
                break;
        } // End WHILE

        Assert (uAx EQ uIndex);

        // If more bits to fill, ...
        if (uAx < uDimAxRht)
        {
            uAx++;      // Count in another bit filled
            // Fill with an INFIX of a single bit of uInfix
            *((LPAPLBOOL) lpMemRes) |= uInfix << uDimRes++;

            // Check to see if we need to skip to the next result byte
            CHECK_NEXT_RESULT_BYTE (lpMemRes, uDimRes);
        } // End IF

        // If more bits to fill, ...
        if (uAx < uDimAxRht)
        {
            // Fill with a SUFFIX of (uDimAxRht - uAx) bits of (uInfix EQ uSuffix)
            uDimRht = uDimAxRht - uAx;
            uIndex = (uInfix EQ uSuffix);
            for (uAx = 0; uAx < uDimRht; uAx++)
            {
                *((LPAPLBOOL) lpMemRes) |= uIndex << uDimRes++;

                // Check to see if we need to skip to the next result byte
                CHECK_NEXT_RESULT_BYTE (lpMemRes, uDimRes);
            } // End FOR
        } // End IF
    } // End FOR
} // End FastBoolScan


//***************************************************************************
//  $FastBoolScanQual
//
//  Fast Boolean "Equal" and "Notequal" Scan
//***************************************************************************

void FastBoolScanQual
    (APLSTYPE     aplTypeRht,       // Right arg storage type
     APLNELM      aplNELMRht,       // Right arg NELM
     LPVOID       lpMemRht,         // Ptr to right arg memory
     LPVOID       lpMemRes,         // Ptr to result    memory
     APLUINT      uDimLo,           // Product of dimensions below axis
     APLUINT      uDimAxRht,        // Length of right arg axis dimension
     FBFN_INDS    uFBFNIndex,       // FBFN_INDS value (e.g., index into FastBoolFns[])
     LPPL_YYSTYPE lpYYFcnStrOpr)    // Ptr to operator function strand

{
    APLUINT       uLo,              // Loop counter for uDimLo
                  uAx,              // Loop counter for uDimAxRht
                  uDimRht,          // Right arg starting index
                  uDimRes = 0,      // Result    ...
                  uBitCountTotal;   // Count of bits processed overall
    UCHAR         uByteMask,        // Byte mask
                  uByteMaskIni,     // Initial byte mask based on uDimAxRht
                  uBitsInMask,      // # bits in the mask
                  apaByte;          // If right arg is an APA, then 0xFF if lpAPA->Off, else 0x00
    LPAPLBOOL     lpaplBool;        // Ptr to Booleans to reduce
    UBOOL         bAPA;             // TRUE iff right arg is an APA
    FASTBOOLTRANS fbtIndex;         // Index into FastBoolTrans (either FBT_EQSCAN or FBT_NESCAN)

    // If the right arg is an APA, handle it specially
    bAPA = IsSimpleAPA (aplTypeRht);
    if (bAPA)
    {
        // The possibilities are as follows (where V can be any non-negative integer vector):
        //   1.  V   {rho}0   or 1
        //   2.  V 2 {rho}0 1 or 1 0 as an APA
        //       (presumably coming from V 2{rho}{iota}2 in origin-0
        //        and which was type demoted to Boolean -- as far as
        //        I know, this doesn't happen, but you never know)
        // The difference between the above two cases is in the APA
        //   multiplier -- in the first case it's zero, and in the
        //   second case it's either 1 or -1.

#define lpAPA       ((LPAPLAPA) lpMemRht)

        Assert (IsMultiNELM (aplNELMRht));
        Assert (lpAPA->Mul EQ 0);

        // Check the APA multiplier
        if (lpAPA->Mul EQ 0)
        {
            // If the offset is 0 and the function is NotEqual, or
            //   ...            1 ...                 Equal,
            //   then the result is all 0s (for the first case), or
            //   all 1s (for the second case)
            if (lpAPA->Off EQ (uFBFNIndex EQ PF_INDEX_EQUAL))
            {
                if (uFBFNIndex EQ PF_INDEX_EQUAL)
                // Fill with constant byte of 1s (0s already done by GHND)
                    FillBitMemory (lpMemRes, uDimLo * uDimAxRht);
                return;
            } else
                apaByte = (lpAPA->Off) ? 0xFF : 0x00;
        } else
            DbgStop ();         // We should never get here
    } // End IF

    // Calculate the Index into FastBoolTrans
    fbtIndex = (uFBFNIndex EQ PF_INDEX_EQUAL) ? FBT_EQSCAN : FBT_NESCAN;

    // Calculate the mask for the first byte
    uByteMaskIni = (UCHAR) ((BIT0 << min (uDimAxRht, NBIB)) - 1);

    // Get ptr to first byte
    lpaplBool = (LPAPLBOOL) lpMemRht;

    // Loop through the right arg translating the bytes in each vector
    for (uBitCountTotal = uLo = 0;
         uLo < uDimLo;
         uLo++, uBitCountTotal = uLo * uDimAxRht)
#define uDimHi      1       // This is a requirement for all Fast Boolean operations
////for (uHi = 0; uHi < uDimHi; uHi++)      // uDimHi EQ 1, so this loop is unnecessary
#define uHi         0       // A consequence of removing the above loop
    {
        UCHAR   uByte,                  // The current byte (from lpaplBool)
                uLastBit;               // Value of bit in BIT7 position of the last byte
        APLUINT uBitCountVector = 0;    // Count of # bits processed so far in this vector

        // Calculate the starting index in the right arg of this vector
        uDimRht = uLo * uDimHi * uDimAxRht + uHi;

        // Calculate the byte mask
        uByteMask = (UCHAR) (uByteMaskIni << (MASKLOG2NBIB & uDimRht));
        uBitsInMask = FastBoolTrans[uByteMask][FBT_BITSUM];

        // Get the byte in question, shifted to the low-order
        if (bAPA)
            uByte = apaByte;
        else
            uByte = (UCHAR) ((uByteMask & lpaplBool[uBitCountTotal >> LOG2NBIB]) >> (MASKLOG2NBIB & uDimRht));

        // Translate this byte into its NotEqualScan or EqualScan
        uByte = FastBoolTrans[uByte][fbtIndex];
        uLastBit = ((uByte & (BIT0 << (uBitsInMask - 1))) ? TRUE : FALSE);

        // Save in the result
        for (uAx = 0; uAx < uBitsInMask; uAx++)
        {
            *((LPAPLBOOL) lpMemRes) |= (BIT0 & uByte) << uDimRes++;
            uByte >>= 1;        // Shift out the previous bit

            // Check to see if we need to skip to the next result byte
            CHECK_NEXT_RESULT_BYTE (lpMemRes, uDimRes);
        } // End FOR

        // Update the # bits processed so far
        uBitCountTotal += uBitsInMask;
        uBitCountVector = uBitsInMask;

        // If there are more bits in this vector, ...
        while (uDimAxRht > uBitCountVector)
        {
            uByteMask = (UCHAR) ((BIT0 << (min (uDimAxRht - uBitCountVector, NBIB))) - 1);
            uBitsInMask = FastBoolTrans[uByteMask][FBT_BITSUM];

            // Get the byte in question
            if (bAPA)
                uByte = apaByte;
            else
                uByte = uByteMask & lpaplBool[uBitCountTotal >> LOG2NBIB];

            // Translate this byte into its NotEqualScan or EqualScan
            uByte = FastBoolTrans[uByte][fbtIndex];
            if (uLastBit ^ (uFBFNIndex EQ PF_INDEX_EQUAL))
                uByte = ~uByte;
            uLastBit = ((uByte & (BIT0 << (uBitsInMask - 1))) ? TRUE : FALSE);

            // Save in the result
            for (uAx = 0; uAx < uBitsInMask; uAx++)
            {
                *((LPAPLBOOL) lpMemRes) |= (BIT0 & uByte) << uDimRes++;
                uByte >>= 1;        // Shift out the previous bit

                // Check to see if we need to skip to the next result byte
                CHECK_NEXT_RESULT_BYTE (lpMemRes, uDimRes);
            } // End FOR

            // Update the # bits processed so far
            uBitCountTotal  += uBitsInMask;
            uBitCountVector += uBitsInMask;
        } // End IF
    } // End FOR
} // End FastBoolScanQual


//***************************************************************************
//  $FastBoolRed
//
//  Fast Boolean Reduction for the eight Boolean functions
//***************************************************************************

void FastBoolRed
    (APLSTYPE     aplTypeRht,       // Right arg storage type
     APLNELM      aplNELMRht,       // Right arg NELM
     LPVOID       lpMemRht,         // Ptr to right arg memory
     LPVOID       lpMemRes,         // Ptr to result    memory
     APLUINT      uDimLo,           // Product of dimensions below axis
     APLUINT      uDimAxRht,        // Length of right arg axis dimension
     FBFN_INDS    uFBFNIndex,       // FBFN_INDS value (e.g., index into FastBoolFns[])
     LPPL_YYSTYPE lpYYFcnStrOpr)    // Ptr to operator function strand

{
    APLUINT       uLo,              // Loop counter for uDimLo
                  uDimRht,          // Right arg starting index
                  uDimRes = 0,      // Result    ...
                  uIndex,           // Index of marker bit in vector
                  uBitCountTotal;   // Count of bits processed overall
    UCHAR         uByteMask,        // Byte mask
                  uByteMaskIni,     // Initial byte mask based on uDimAxRht
                  uBitsInMask,      // # bits in the mask
                  uInfix,           // Infix bit value
                  uSuffix,          // Suffix equivalence value
                  uIdentElem,       // Identity element (if it exists)
                  uNotMarker;       // Complement of Marker
    FASTBOOLTRANS fbtIndex;         // FBT_FIRST1 or 0
    LPAPLBOOL     lpaplBool;        // Ptr to Booleans to reduce

    // Get the parameters specific to this function
    uNotMarker = FastBoolFns[uFBFNIndex].NotMarker;     // J[0]
    uIdentElem = FastBoolFns[uFBFNIndex].IdentElem;     // J[1]
    uSuffix    = FastBoolFns[uFBFNIndex].Suffix;        // J[2]
    fbtIndex   = fbtFirst[!uNotMarker];                 // FBT_FIRST1 or 0

    // If the right arg is an APA, handle it specially
    if (IsSimpleAPA (aplTypeRht))
    {
        // The possibilities are as follows (where V can be any non-negative integer vector):
        //   1.  V   {rho}0   or 1
        //   2.  V 2 {rho}0 1 or 1 0 as an APA
        //       (presumably coming from V 2{rho}{iota}2 in origin-0
        //        and which was type demoted to Boolean -- as far as
        //        I know, this doesn't happen, but you never know)
        // The difference between the above two cases is in the APA
        //   multiplier -- in the first case it's zero, and in the
        //   second case it's either 1 or -1.

#define lpAPA       ((LPAPLAPA) lpMemRht)

        Assert (IsMultiNELM (aplNELMRht));
        Assert (lpAPA->Mul EQ 0);

        // Check the APA multiplier
        if (lpAPA->Mul EQ 0)
        {
            // Set the infix bit
            uInfix = !((BIT0 & aplNELMRht) ? uIdentElem : uNotMarker);

            // Because the APA has all the same element (lpAPA->Off),
            //   we find it either in the first position or not at all
            if (lpAPA->Off EQ !uNotMarker)
            {
                // Marker bit found in the first position:  the index is 0
                // Fill with uInfix EQ uSuffix
                if (uInfix EQ uSuffix)
                    FillBitMemory (lpMemRes, uDimLo);
            } else
            {
                // Marker bit not found:  the index is uDimAxRht
                // If the index is odd, use uNotMarker
                //   otherwise, use uIdentElem
                if ((BIT0 & uDimAxRht) ? uNotMarker : uIdentElem)
                    FillBitMemory (lpMemRes, uDimLo);
            } // End IF/ELSE
#undef  lpAPA
        } else
            DbgStop ();         // We should never get here
        return;         // We're done
    } // End IF

    // Calculate the mask for the first byte
    uByteMaskIni = (UCHAR) ((BIT0 << min (uDimAxRht, NBIB)) - 1);

    // Get ptr to first byte
    lpaplBool = (LPAPLBOOL) lpMemRht;

    // Loop through the right arg looking for the first marker bit in each vector
    for (uBitCountTotal = uLo = 0;
         uLo < uDimLo;
         uLo++, uBitCountTotal = uLo * uDimAxRht)
#define uDimHi      1       // This is a requirement for all Fast Boolean operations
////for (uHi = 0; uHi < uDimHi; uHi++)      // uDimHi EQ 1, so this loop is unnecessary
#define uHi         0       // A consequence of removing the above loop
    {
        UCHAR   uByte,                  // The current byte (from lpaplBool)
                uFirstBitIndexInByte;   // Index of first marker bit in the byte
        APLUINT uBitCountVector = 0;    // Count of # bits processed so far in this vector

        // Calculate the starting index in the right arg of this vector
        uDimRht = uLo * uDimHi * uDimAxRht + uHi;

        // Calculate the byte mask
        uByteMask = (UCHAR) (uByteMaskIni << (MASKLOG2NBIB & uDimRht));
        uBitsInMask = FastBoolTrans[uByteMask][FBT_BITSUM];

        // Get the byte in question, shifted to the low-order
        uByte = (UCHAR) ((uByteMask & lpaplBool[uBitCountTotal >> LOG2NBIB]) >> (MASKLOG2NBIB & uDimRht));

        // Find the first marker bit in uByte
        uFirstBitIndexInByte = FastBoolTrans[uByte][fbtIndex];
        uFirstBitIndexInByte = min (uFirstBitIndexInByte, uBitsInMask);

        // Update the # bits processed so far
        uBitCountTotal += uBitsInMask;

        // If not found so far, ...
        if (uFirstBitIndexInByte EQ uBitsInMask)
        {
            // So far, we've processed this many bits w/o a hit
            uBitCountVector = uBitsInMask;
            uFirstBitIndexInByte = 0;

            // If there are more bits in this vector, ...
            while (uDimAxRht > uBitCountVector)
            {
                uByteMask = (UCHAR) ((BIT0 << (min (uDimAxRht - uBitCountVector, NBIB))) - 1);
                uBitsInMask = FastBoolTrans[uByteMask][FBT_BITSUM];

                // Get the byte in question
                uByte = uByteMask & lpaplBool[uBitCountTotal >> LOG2NBIB];

                // Find the first marker bit in uByte
                uFirstBitIndexInByte = FastBoolTrans[uByte][fbtIndex];
                uFirstBitIndexInByte = min (uFirstBitIndexInByte, uBitsInMask);

                // Update the # bits processed so far
                uBitCountTotal += uBitsInMask;

                // If not found, ...
                if (uFirstBitIndexInByte EQ uBitsInMask)
                {
                    uBitCountVector += uBitsInMask;
                    uFirstBitIndexInByte = 0;
                } else
                    break;
            } // End WHILE
        } // End IF

        // Get the index of the first marker bit in the vector (uDimAxRht if not found)
        uIndex = uBitCountVector + uFirstBitIndexInByte;

        // Set the infix bit
        uInfix = !((BIT0 & uIndex) ? uIdentElem: uNotMarker);

        // Ensure within range
        Assert (uIndex <= uDimAxRht);

        // Save in the result
        // If not found, ...
        if (uIndex EQ uDimAxRht)
            // If the index is odd, use uNotMarker
            //   otherwise, use uIdentElem
            *((LPAPLBOOL) lpMemRes) |= ((BIT0 & uIndex) ? uNotMarker
                                                        : uIdentElem) << uDimRes++;
        else
        // If found in last position
        if (uIndex EQ (uDimAxRht - 1))
            // Use uInfix
            *((LPAPLBOOL) lpMemRes) |= uInfix << uDimRes++;
        else
            // Use uInfix EQ uSuffix
            *((LPAPLBOOL) lpMemRes) |= (uInfix EQ uSuffix) << uDimRes++;
        // Check to see if we need to skip to the next result byte
        CHECK_NEXT_RESULT_BYTE (lpMemRes, uDimRes);
    } // End FOR
#undef  uHi
#undef  uDimHi
} // End FastBoolRed


//***************************************************************************
//  $FastBoolRedQual
//
//  Fast Boolean "Equal" and "Notequal" Reduction
//***************************************************************************

void FastBoolRedQual
    (APLSTYPE     aplTypeRht,       // Right arg storage type
     APLNELM      aplNELMRht,       // Right arg NELM
     LPVOID       lpMemRht,         // Ptr to right arg memory
     LPVOID       lpMemRes,         // Ptr to result    memory
     APLUINT      uDimLo,           // Product of dimensions below axis
     APLUINT      uDimAxRht,        // Length of right arg axis dimension
     FBFN_INDS    uFBFNIndex,       // FBFN_INDS value (e.g., index into FastBoolFns[])
     LPPL_YYSTYPE lpYYFcnStrOpr)    // Ptr to operator function strand

{
    APLUINT   uLo,                  // Loop counter for uDimLo
              uDimRht,              // Right arg starting index
              uDimRes,              // Result    ...
              uBitCountTotal,       // Count of bits processed overall
              uAccum;               // Accumulator for the result
    UCHAR     uByteMask,            // Byte mask
              uByteMaskIni,         // Initial byte mask based on uDimAxRht
              uBitsInMask;          // # bits in the mask
    LPAPLBOOL lpaplBool;            // Ptr to Booleans to reduce
    UBOOL     bComp;                // TRUE iff (uFBFNIndex EQ PF_INDEX_NOTEQUAL) EQ (BIT0 & uDimAxRht)

    // Calculate comparison bit to distinguish EQ from NE
    bComp = (uFBFNIndex EQ PF_INDEX_NOTEQUAL);

    // If the right arg is an APA, handle it specially
    if (IsSimpleAPA (aplTypeRht))
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)

        Assert (IsMultiNELM (aplNELMRht));
        Assert (lpAPA->Mul EQ 0);

        // The possibilities are as follows (where V can be any non-negative integer vector):
        //   1.  V   {rho}0   or 1
        //   2.  V 2 {rho}0 1 or 1 0 as an APA
        //       (presumably coming from V 2{rho}{iota}2 in origin-0
        //        and which was type demoted to Boolean -- as far as
        //        I know, this doesn't happen, but you never know)
        // The difference between the above two cases is in the APA
        //   multiplier -- in the first case it's zero, and in the
        //   second case it's either 1 or -1.

        // Check the APA multiplier
        if (lpAPA->Mul EQ 0)
        {
            if (bComp EQ (BIT0 & uDimAxRht))
                FillBitMemory (lpMemRes, uDimLo);
        } else
            DbgStop ();         // We should never get here
#undef  lpAPA
        return;         // We're done
    } // End IF

    // Calculate the mask for the first byte
    uByteMaskIni = (UCHAR) ((BIT0 << min (uDimAxRht, NBIB)) - 1);

    // Get ptr to first byte
    lpaplBool = (LPAPLBOOL) lpMemRht;

    // Loop through the right arg summing the bits in each vector
    for (uBitCountTotal = uDimRes = uLo = 0;
         uLo < uDimLo;
         uLo++, uBitCountTotal = uLo * uDimAxRht)
#define uDimHi      1       // This is a requirement for all Fast Boolean operations
////for (uHi = 0; uHi < uDimHi; uHi++)      // uDimHi EQ 1, so this loop is unnecessary
#define uHi         0       // A consequence of removing the above loop
    {
        APLUINT uBitCountVector = 0;    // Count of # bits processed so far in this vector

        // Calculate the starting index in the right arg/result of this vector
        uDimRht = uLo * uDimHi * uDimAxRht + uHi;

        // Calculate the byte mask
        uByteMask = (UCHAR) (uByteMaskIni << (MASKLOG2NBIB & uDimRht));
        uBitsInMask = FastBoolTrans[uByteMask][FBT_BITSUM];

        // Get +/ on first byte or tail thereof
        uAccum = FastBoolTrans[uByteMask & lpaplBool[uBitCountTotal >> LOG2NBIB]][FBT_BITSUM];

        // Update the # bits processed so far (modulo NBIB)
        uBitCountTotal += uBitsInMask;

        // So far, we've processed this many bits
        uBitCountVector = uBitsInMask;

        // If there are more bits in this vector, ...
        while (uDimAxRht > uBitCountVector)
        {
            uByteMask = (UCHAR) ((BIT0 << (min (uDimAxRht - uBitCountVector, NBIB))) - 1);
            uBitsInMask = FastBoolTrans[uByteMask][FBT_BITSUM];

            uAccum += FastBoolTrans[uByteMask & lpaplBool[uBitCountTotal >> LOG2NBIB]][FBT_BITSUM];

            // Update the # bits processed so far (modulo NBIB)
            uBitCountTotal  += uBitsInMask;
            uBitCountVector += uBitsInMask;
        } // End IF

        // Save in the result
        *((LPAPLBOOL) lpMemRes) |=
          (bComp ?  (BIT0 &              uAccum)
                 : !(BIT0 & (uDimAxRht - uAccum))) << uDimRes++;

        // Check to see if we need to skip to the next result byte
        CHECK_NEXT_RESULT_BYTE (lpMemRes, uDimRes);
    } // End FOR
#undef  uHi
#undef  uDimHi
} // End FastBoolRedQual


//***************************************************************************
//  $FastBoolRedPlus
//
//  Fast Boolean "Plus" Reduction
//***************************************************************************

void FastBoolRedPlus
    (APLSTYPE     aplTypeRht,       // Right arg storage type
     APLNELM      aplNELMRht,       // Right arg NELM
     LPVOID       lpMemRht,         // Ptr to right arg memory
     LPVOID       lpMemRes,         // Ptr to result    memory
     APLUINT      uDimLo,           // Product of dimensions below axis
     APLUINT      uDimAxRht,        // Length of right arg axis dimension
     FBFN_INDS    uFBFNIndex,       // FBFN_INDS value (e.g., index into FastBoolFns[])
     LPPL_YYSTYPE lpYYFcnStrOpr)    // Ptr to operator function strand

{
    APLUINT   uLo,                  // Loop counter for uDimLo
              uDimRht,              // Right arg starting index
              uBitCountTotal,       // Count of bits processed overall
              uAccum;               // Accumulator for the result
    UCHAR     uByteMask,            // Byte mask
              uByteMaskIni,         // Initial byte mask based on uDimAxRht
              uBitsInMask;          // # bits in the mask
    LPAPLBOOL lpaplBool;            // Ptr to Booleans to reduce

    // If the right arg is an APA, handle it specially
    if (IsSimpleAPA (aplTypeRht))
    {
        // The possibilities are as follows (where V can be any non-negative integer vector):
        //   1.  V   {rho}0   or 1
        //   2.  V 2 {rho}0 1 or 1 0 as an APA
        //       (presumably coming from V 2{rho}{iota}2 in origin-0
        //        and which was type demoted to Boolean -- as far as
        //        I know, this doesn't happen, but you never know)
        // The difference between the above two cases is in the APA
        //   multiplier -- in the first case it's zero, and in the
        //   second case it's either 1 or -1.

        // Check the APA multiplier
#define lpAPA       ((LPAPLAPA) lpMemRht)
        if (lpAPA->Mul NE 0)
            *((LPAPLINT) lpMemRes) = 1;             // That's +/0 1 or +/1 0
        else
        if (lpAPA->Off)                             // If it's non-zero (i.e., 1), ...
        for (uLo = 0; uLo < uDimLo; uLo++)
            *((LPAPLINT) lpMemRes)++ = uDimAxRht;   // That's +/V{rho}1
#undef  lpAPA
        return;         // We're done
    } // End IF

    // Calculate the mask for the first byte
    uByteMaskIni = (UCHAR) ((BIT0 << min (uDimAxRht, NBIB)) - 1);

    // Get ptr to first byte
    lpaplBool = (LPAPLBOOL) lpMemRht;

    // Loop through the right arg summing the bits in each vector
    for (uBitCountTotal = uLo = 0;
         uLo < uDimLo;
         uLo++, uBitCountTotal = uLo * uDimAxRht)
#define uDimHi      1       // This is a requirement for all Fast Boolean operations
////for (uHi = 0; uHi < uDimHi; uHi++)      // uDimHi EQ 1, so this loop is unnecessary
#define uHi         0       // A consequence of removing the above loop
    {
        APLUINT uBitCountVector = 0;    // Count of # bits processed so far in this vector

        // Calculate the starting index in the right arg/result of this vector
        uDimRht = uLo * uDimHi * uDimAxRht + uHi;

        // Calculate the byte mask
        uByteMask = (UCHAR) (uByteMaskIni << (MASKLOG2NBIB & uDimRht));
        uBitsInMask = FastBoolTrans[uByteMask][FBT_BITSUM];

        // Get +/ on first byte or tail thereof
        uAccum = FastBoolTrans[uByteMask & lpaplBool[uBitCountTotal >> LOG2NBIB]][FBT_BITSUM];

        // Update the # bits processed so far (modulo NBIB)
        uBitCountTotal += uBitsInMask;

        // So far, we've processed this many bits
        uBitCountVector = uBitsInMask;

        // If there are more bits in this vector, ...
        while (uDimAxRht > uBitCountVector)
        {
            uByteMask = (UCHAR) ((BIT0 << (min (uDimAxRht - uBitCountVector, NBIB))) - 1);
            uBitsInMask = FastBoolTrans[uByteMask][FBT_BITSUM];

            uAccum += FastBoolTrans[uByteMask & lpaplBool[uBitCountTotal >> LOG2NBIB]][FBT_BITSUM];

            // Update the # bits processed so far (modulo NBIB)
            uBitCountTotal  += uBitsInMask;
            uBitCountVector += uBitsInMask;
        } // End IF

        // Save in the result
        *((LPAPLINT) lpMemRes)++ = uAccum;
    } // End FOR
#undef  uHi
#undef  uDimHi
} // End FastBoolRedPlus


//***************************************************************************
//  End of File: fastbool.c
//***************************************************************************
