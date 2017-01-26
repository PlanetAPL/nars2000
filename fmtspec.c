//***************************************************************************
//  NARS2000 -- Format Specification FSA Routines
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
#include <tchar.h>
#include <math.h>
#include "headers.h"

// The following struc should have as many rows as TKROW_LENGTH
//   and as many columns as TKCOL_LENGTH.
FSACTSTR fsaActTableFS [][FSCOL_LENGTH]
#ifndef PROTO
 =
{   // FSROW_INIT       Initial state
 {{FSROW_INIT    , fnRepAcc  },         // 00:  Digit
  {FSROW_CHKEOS  , fnSetQual },         // 01:  Capital letter A
  {FSROW_SPEC    , fnSetQual },         // 02:  ...            B
  {FSROW_SPEC    , fnSetQual },         // 03:  ...            C
  {FSROW_CHKEOS  , fnSetQual },         // 04:  ...            E
  {FSROW_CHKEOS  , fnSetQual },         // 05:  ...            F
  {FSROW_CHKEOS  , fnSetQual },         // 06:  ...            G
  {FSROW_CHKEOS  , fnSetQual },         // 07:  ...            I
  {FSROW_SPEC    , fnSetQual },         // 08:  ...            K
  {FSROW_SPEC    , fnSetQual },         // 09:  ...            L
  {FSROW_SPEC    , fnSetText },         // 0A:  ...            M
  {FSROW_SPEC    , fnSetText },         // 0B:  ...            N
  {FSROW_SPEC    , fnSetModO },         // 0C:  ...            O
  {FSROW_SPEC    , fnSetText },         // 0D:  ...            P
  {FSROW_SPEC    , fnSetText },         // 0E:  ...            Q
  {FSROW_SPEC    , fnSetR    },         // 0F:  ...            R
  {FSROW_SPEC    , fnSetText },         // 10:  ...            S
  {FSROW_CHKEOS  , fnSetQual },         // 11:  ...            T
  {FSROW_CHKEOS  , fnSetQual },         // 12:  ...            X
  {FSROW_SPEC    , fnSetQual },         // 13:  ...            Z
  {FSROW_CHKEOS  , fnSetQual },         // 14:  Text leading delimiter
  {FSROW_ERROR   , fnError   },         // 15:  Comma
  {FSROW_INIT    , fnDnLvl   },         // 16:  Left paren
  {FSROW_ERROR   , fnError   },         // 17:  Right ...
  {FSROW_INIT    , NULL      },         // 18:  White space
  {FSROW_ERROR   , fnError   },         // 19:  All other chars
  {FSROW_EXIT    , fnFmtExit },         // 1A:  End-Of-Line
 },
    // FSROW_SPEC       Within a format specification
 {{FSROW_ERROR   , fnError   },         // 00:  Digit
  {FSROW_CHKEOS  , fnSetQual },         // 01:  Capital letter A
  {FSROW_SPEC    , fnSetQual },         // 02:  ...            B
  {FSROW_SPEC    , fnSetQual },         // 03:  ...            C
  {FSROW_CHKEOS  , fnSetQual },         // 04:  ...            E
  {FSROW_CHKEOS  , fnSetQual },         // 05:  ...            F
  {FSROW_CHKEOS  , fnSetQual },         // 06:  ...            G
  {FSROW_CHKEOS  , fnSetQual },         // 07:  ...            I
  {FSROW_SPEC    , fnSetQual },         // 08:  ...            K
  {FSROW_SPEC    , fnSetQual },         // 09:  ...            L
  {FSROW_SPEC    , fnSetText },         // 0A:  ...            M
  {FSROW_SPEC    , fnSetText },         // 0B:  ...            N
  {FSROW_SPEC    , fnSetModO },         // 0C:  ...            O
  {FSROW_SPEC    , fnSetText },         // 0D:  ...            P
  {FSROW_SPEC    , fnSetText },         // 0E:  ...            Q
  {FSROW_SPEC    , fnSetR    },         // 0F:  ...            R
  {FSROW_SPEC    , fnSetText },         // 10:  ...            S
  {FSROW_CHKEOS  , fnSetQual },         // 11:  ...            T
  {FSROW_CHKEOS  , fnSetQual },         // 12:  ...            X
  {FSROW_SPEC    , fnSetQual },         // 13:  ...            Z
  {FSROW_CHKEOS  , fnSetQual },         // 14:  Text leading delmiter
  {FSROW_ERROR   , fnError   },         // 15:  Comma
  {FSROW_ERROR   , fnError   },         // 16:  Left paren
  {FSROW_ERROR   , fnError   },         // 17:  Right ...
  {FSROW_SPEC    , NULL      },         // 18:  White space
  {FSROW_ERROR   , fnError   },         // 19:  All other chars
  {FSROW_ERROR   , fnError   },         // 1A:  End-Of-Line
 },
    // FSROW_CHKEOS     Finished with this spec
 {{FSROW_ERROR   , fnError   },         // 00:  Digit
  {FSROW_ERROR   , fnError   },         // 01:  Capital letter A
  {FSROW_ERROR   , fnError   },         // 02:  ...            B
  {FSROW_ERROR   , fnError   },         // 03:  ...            C
  {FSROW_ERROR   , fnError   },         // 04:  ...            E
  {FSROW_ERROR   , fnError   },         // 05:  ...            F
  {FSROW_ERROR   , fnError   },         // 06:  ...            G
  {FSROW_ERROR   , fnError   },         // 07:  ...            I
  {FSROW_ERROR   , fnError   },         // 08:  ...            K
  {FSROW_ERROR   , fnError   },         // 09:  ...            L
  {FSROW_ERROR   , fnError   },         // 0A:  ...            M
  {FSROW_ERROR   , fnError   },         // 0B:  ...            N
  {FSROW_ERROR   , fnError   },         // 0C:  ...            O
  {FSROW_ERROR   , fnError   },         // 0D:  ...            P
  {FSROW_ERROR   , fnError   },         // 0E:  ...            Q
  {FSROW_ERROR   , fnError   },         // 0F:  ...            R
  {FSROW_ERROR   , fnError   },         // 10:  ...            S
  {FSROW_ERROR   , fnError   },         // 11:  ...            T
  {FSROW_ERROR   , fnError   },         // 12:  ...            X
  {FSROW_ERROR   , fnError   },         // 13:  ...            Z
  {FSROW_ERROR   , fnError   },         // 14:  Text leading delimiter
  {FSROW_INIT    , fnFmtDone },         // 15:  Comma
  {FSROW_ERROR   , fnError   },         // 16:  Left paren
  {FSROW_CHKEOS  , fnUpLvl   },         // 17:  Right ...
  {FSROW_CHKEOS  , NULL      },         // 18:  White space
  {FSROW_ERROR   , fnError   },         // 19:  All other chars
  {FSROW_EXIT    , fnFmtExit },         // 1A:  End-Of-Line
 },
}
#endif
;


//***************************************************************************
//  $fnFmtExit
//
//  We're done with the entire format string.
//  Ensure we've matched all nesting levels.
//***************************************************************************

UBOOL fnFmtExit
    (LPFSLOCALVARS lpfsLocalVars)           // Ptr to FmtSpecStr local vars

{
     // Save the offset to the next FMTSPECSTR
     lpfsLocalVars->lpfsCur->offNext = lpfsLocalVars->uFsNxt;

    // Ensure we're at nesting level 0
    if (lpfsLocalVars->nestLvl EQ 0)
        return TRUE;
    else
        return fnError (lpfsLocalVars);
} // End fnFmtExit


//***************************************************************************
//  $fnFmtDone
//
//  We're done with this format specification
//***************************************************************************

UBOOL fnFmtDone
    (LPFSLOCALVARS lpfsLocalVars)           // Ptr to FmtSpecStr local vars

{
    Assert (lpfsLocalVars->lpwszCur[0] EQ L',');

    // Ensure there's another WCHAR in the format string
    if (lpfsLocalVars->lpwszCur[1] EQ WC_EOS)
        goto FORMAT_EXIT;

    // If this entry is inuse, ...
    if (lpfsLocalVars->lpfsCur->bInuse)
    {
        // Save the offset to the next FMTSPECSTR
        lpfsLocalVars->lpfsCur->offNext = lpfsLocalVars->uFsNxt;

        // Skip over it
        lpfsLocalVars->lpfsCur =
          (LPFMTSPECSTR) ByteAddr (lpfsLocalVars->lpfsCur, lpfsLocalVars->uFsNxt);

        // Initialize the FmtSpecStr output area
        InitFmtSpecStr (lpfsLocalVars);
    } // End IF

    return TRUE;

FORMAT_EXIT:
    return fnError (lpfsLocalVars);
} // End fnFmtDone


//***************************************************************************
//  $fnDnLvl
//
//  We encountered a left paren.
//  Start nesting.
//***************************************************************************

UBOOL fnDnLvl
    (LPFSLOCALVARS lpfsLocalVars)           // Ptr to FmtSpecStr local vars

{
    LPFMTSPECSTR lpfsCur;                   // Ptr to current FMTSPECSTR

    // Get a ptr to the current FMTSPECSTR
    lpfsCur = lpfsLocalVars->lpfsCur;

    // If there's a repetition factor, ...
    if (lpfsCur->bRep)
    {
        lpfsCur->nestRep = lpfsCur->fsRep;
        lpfsCur->fsRep = 0;
        lpfsCur->bRep = FALSE;
    } else
        lpfsCur->nestRep = 1;

    // Increment the nesting level
    lpfsLocalVars->nestLvl++;

    Assert (lpfsLocalVars->uFsNxt EQ sizeof (FMTSPECSTR));

    // Link this entry into the chain
    lpfsCur->lpfsNestPrv       = lpfsLocalVars->lpfsNestCur;
    lpfsLocalVars->lpfsNestCur = lpfsCur;

    // Get ptr to next FMTSPECSTR
    lpfsLocalVars->lpfsCur = &lpfsLocalVars->lpfsCur[1];

    // Initialize the FmtSpecStr output area
    InitFmtSpecStr (lpfsLocalVars);

    return TRUE;
} // End fnDnLvl


//***************************************************************************
//  $InitFmtSpecStr
//
//  Initialize the FmtSpecStr output area
//***************************************************************************

void InitFmtSpecStr
    (LPFSLOCALVARS lpfsLocalVars)           // Ptr to FmtSpecStr local vars

{
    // Zero the memory
    ZeroMemory (lpfsLocalVars->lpfsCur, sizeof (FMTSPECSTR));

    // Initialize the repetition factor
    lpfsLocalVars->lpfsCur->fsRep = 1;

    // Initialize local vars
    lpfsLocalVars->uFsNxt = sizeof (FMTSPECSTR);
} // End InitFmtSpecStr


//***************************************************************************
//  $fnUpLvl
//
//  We encountered a right paren.
//  Close off this nesting by copying the intermediate
//    format specs to FMTSPECSTR memory
//***************************************************************************

UBOOL fnUpLvl
    (LPFSLOCALVARS lpfsLocalVars)           // Ptr to FmtSpecStr local vars

{
    LPFMTSPECSTR lpfsNestCur,               // Ptr to left paren FMTSPECSTR
                 lpfsNxt,                   // ...    next available ...
                 lpfsCur;                   // ...    loop counter
    UINT         rep,                       // Nesting repetition factor
                 uLen,                      // Byte length of fmt specs to be repeated
                 uCnt;                      // Loop counter

    // Save the offset to the next FMTSPECSTR
    lpfsLocalVars->lpfsCur->offNext = lpfsLocalVars->uFsNxt;

    // Get a ptr to the matching left paren FMTSPECSTR
    lpfsNestCur = lpfsLocalVars->lpfsNestCur;

    // If it's not valid, ...
    if (lpfsNestCur EQ NULL)
        goto FORMAT_EXIT;

    // Get the repetition factor
    rep = lpfsNestCur->nestRep;

    // Unlink this entry from the chain
    lpfsLocalVars->lpfsNestCur = lpfsNestCur->lpfsNestPrv;

    // If the repetition factor is non-zero
    if (rep)
    {
        // Get a ptr to the next available output save area byte
        lpfsNxt = (LPFMTSPECSTR) ByteAddr (lpfsLocalVars->lpfsCur, lpfsLocalVars->uFsNxt);

        // Get the byte length of the fmt specs to be repeated
        uLen = (UINT) ByteDiff (lpfsNxt, &lpfsNestCur[1]);

        // Calculate the # format specs to be repeated
        for (uCnt = 0, lpfsCur = &lpfsNestCur[1];
             lpfsCur < lpfsNxt;
             uCnt++)
            lpfsCur = (LPFMTSPECSTR) ByteAddr (lpfsCur, lpfsCur->offNext);

        // Move the FMTSPECSTRs down over the left paren FMTSPECSTR
        MoveMemory (lpfsNestCur, &lpfsNestCur[1], uLen);

        // Back up the next available ptr, too
        lpfsNxt--;

        // Copy the FMTSPECSTRs
        while (--rep)
        {
            // Copy the FMTSPECSTRs
            CopyMemory (lpfsNxt, lpfsNestCur, uLen);

            // Skip to next available byte
            lpfsNxt = (LPFMTSPECSTR) ByteAddr (lpfsNxt, uLen);
        } // End WHILE

        // Save as next FMTSPECSTR entry
        lpfsNestCur = lpfsNxt;
    } // End IF

    // Decrement the nesting level
    lpfsLocalVars->nestLvl--;

    // Save as the current FMTSPECSTR
    lpfsLocalVars->lpfsCur = lpfsNestCur;

    // Initialize the FmtSpecStr output area
    InitFmtSpecStr (lpfsLocalVars);

    return TRUE;

FORMAT_EXIT:
    return fnError (lpfsLocalVars);
} // End fnUpLvl


//***************************************************************************
//  $fnSetR
//
//  Handle either Rational format (Rnn.nn) or Background Qualifier (R<...>)
//***************************************************************************

UBOOL fnSetR
    (LPFSLOCALVARS lpfsLocalVars)           // Ptr to FmtSpecStr local vars

{
    // Izit Rational format?
    if (isdigit (lpfsLocalVars->lpwszCur[1]))
    {
        UBOOL bRet;

        // Handle as R-format spec
        bRet = fnSetQual (lpfsLocalVars);

        // If all went well, ...
        if (bRet)
            lpfsLocalVars->State = FSROW_CHKEOS;

        return bRet;
    } else
        return fnSetText (lpfsLocalVars);
} // End fnSetR


//***************************************************************************
//  $fnSetText
//
//  Save the text of a qualifier
//
//  M<text>
//  N<text>
//  P<text>
//  Q<text>
//  R<text>
//  S<text>
//***************************************************************************

UBOOL fnSetText
    (LPFSLOCALVARS lpfsLocalVars)           // Ptr to FmtSpecStr local vars

{
    WCHAR   wchQual;                        // Qualifier, e.g. M N P Q R S
    UINT    offTxt;                         // Offset to saved text
    LPWCHAR lpwCur;                         // Ptr to current text

    // The current char is the modifier
    wchQual = *lpfsLocalVars->lpwszCur;

    // Skip over it
    lpfsLocalVars->lpwszCur++;
    lpfsLocalVars->uChar++;

    Assert (wchQual EQ L'M'
         || wchQual EQ L'N'
         || wchQual EQ L'P'
         || wchQual EQ L'Q'
         || wchQual EQ L'R'
         || wchQual EQ L'S');

    // Save ptr to text to scan
    lpwCur = lpfsLocalVars->lpwszCur;

    // If it's not Symbol Substitution, ...
    // Save the text to the FMTSPECSTR save area
    // On return:
    //    lpwCur points to the char after the trailing delimiter
    offTxt = SaveText (lpfsLocalVars, &lpwCur, wchQual NE L'S');

    // If it's in error, ...
    if (offTxt EQ 0)
        goto FORMAT_EXIT;

    // Mark the qualifier as present
    //   and save the offset
    switch (wchQual)
    {
        case L'M':
            // Check to see if already present
            if (lpfsLocalVars->lpfsCur->bM)
                goto FORMAT_EXIT;

            // Mark as present
            lpfsLocalVars->lpfsCur->bM = TRUE;

            // Save the offset
            lpfsLocalVars->lpfsCur->offMtxt = offTxt;

            break;

        case L'N':
            // Check to see if already present
            if (lpfsLocalVars->lpfsCur->bN)
                goto FORMAT_EXIT;

            // Mark as present
            lpfsLocalVars->lpfsCur->bN = TRUE;

            // Save the offset
            lpfsLocalVars->lpfsCur->offNtxt = offTxt;

            break;

        case L'P':
            // Check to see if already present
            if (lpfsLocalVars->lpfsCur->bP)
                goto FORMAT_EXIT;

            // Mark as present
            lpfsLocalVars->lpfsCur->bP = TRUE;

            // Save the offset
            lpfsLocalVars->lpfsCur->offPtxt = offTxt;

            break;

        case L'Q':
            // Check to see if already present
            if (lpfsLocalVars->lpfsCur->bQ)
                goto FORMAT_EXIT;

            // Mark as present
            lpfsLocalVars->lpfsCur->bQ = TRUE;

            // Save the offset
            lpfsLocalVars->lpfsCur->offQtxt = offTxt;

            break;

        case L'R':
            // Check to see if already present
            if (lpfsLocalVars->lpfsCur->bR)
                goto FORMAT_EXIT;

            // Mark as present
            lpfsLocalVars->lpfsCur->bR = TRUE;

            // Save the offset
            lpfsLocalVars->lpfsCur->offRtxt = offTxt;

            break;

        case L'S':
            // Check to see if already present
            if (lpfsLocalVars->lpfsCur->bS)
                goto FORMAT_EXIT;

            // Mark as present
            lpfsLocalVars->lpfsCur->bS = TRUE;

            // Save the offset
            lpfsLocalVars->lpfsCur->offStxt = offTxt;

            break;

        defstop
            break;
    } // End SWITCH

    // Mark as inuse
    lpfsLocalVars->lpfsCur->bInuse = TRUE;

    return TRUE;

FORMAT_EXIT:
    return fnError (lpfsLocalVars);
} // End fnSetText


//***************************************************************************
//  $SaveText
//
//  Save text to the FMTSPECSTR output save area
//
//  On return:
//      *lplpwText points the char after the trailing delimiter
//      Offset from lpfsLocalVars->lpfsCur to saved text
//***************************************************************************

UINT SaveText
    (LPFSLOCALVARS lpfsLocalVars,           // Ptr to FmtSpecStr local vars
     LPWCHAR      *lplpwText,               // Ptr to ptr to leading text delimiter
     UBOOL         bSave)                   // TRUE iff we should actually save

{
    WCHAR   wchDelim;                       // Text delmiter char
    LPWCHAR lpwNext;                        // Ptr to matching delimiter
    UINT    offTxt,                         // Offset to saved text
            uCnt,                           // Loop counter
            uScanTxtLenChars,               // Scanned text length in WCHARs
            uCopyTxtLenChars;               // Copied  ...

    // The first char is the text delmiter --
    //   save and skip over it
    wchDelim = *(*lplpwText)++;

    // Find matching delimiter
    // Split cases based upon the left hand delimiter
    switch (wchDelim)
    {
        // These delimiters are all their own
        //   matching righthand delimiter
        case WC_SQ:
        case WC_DQ:
        case UTF16_QUAD:
        case UTF16_QUAD2:
        case UTF16_QUOTEQUAD:
        case UTF16_DIERESIS:
            break;

        case UTF16_LEFTCARET:
            wchDelim = UTF16_RIGHTCARET;

            break;

        case UTF16_LEFTSHOE:
            wchDelim = UTF16_RIGHTSHOE;

            break;

        default:
            return 0;
    } // End SWITCH

    // Search for the matching delimiter
    lpwNext = strchrW (*lplpwText, wchDelim);
    if (lpwNext EQ NULL)
        goto FORMAT_EXIT;

    // Calculate the scanned text length in chars
    uScanTxtLenChars = (UINT) (lpwNext - *lplpwText);

    // Save the offset to the text
    offTxt = lpfsLocalVars->uFsNxt;

    if (bSave)
    {
        // Save as copied text length in chars
        uCopyTxtLenChars = uScanTxtLenChars;

        // Copy the text to local memory
        CopyMemoryW (ByteAddr (lpfsLocalVars->lpfsCur, offTxt),
                    *lplpwText,
                     uCopyTxtLenChars);
    } else
    {
        WCHAR wSymSub[SYMSUB_LENGTH + 1] = STDSYMSUB,   // Symbol Substitutions
              wStdSym[SYMSUB_LENGTH + 1] = STDSYMSUB;   // Standard Symbols

        // Validate the symbol substitution text

        // Ensure even # values
        if (uScanTxtLenChars & BIT0)
            goto FORMAT_EXIT;

        // Fill in the default values for Symbol Substitution
////////wSymSub[SYMSUB_OVERFLOW_FILL]  = L'*';              // Already set by = STDSYMSUB
        wSymSub[SYMSUB_DECIMAL_SEP]    = GetQuadFCValue (FCNDX_DECIMAL_SEP);
        wSymSub[SYMSUB_THOUSANDS_SEP]  = GetQuadFCValue (FCNDX_THOUSANDS_SEP);
////////wSymSub[SYMSUB_ZERO_FILL]      = L'0';              // Already set by = STDSYMSUB
////////wSymSub[SYMSUB_PRECISION_LOSS] = DEF_UNDERFLOW      // Already set by = STDSYMSUB
////////wSymSub[SYMSUB_Z_CHAR]         = L'Z';              // Already set by = STDSYMSUB
////////wSymSub[SYMSUB_9_CHAR]         = L'9';              // Already set by = STDSYMSUB
////////wSymSub[SYMSUB_EXPONENT_CHAR]  = DEF_EXPONENT_UC    // Already set by = STDSYMSUB
////////wSymSub[SYMSUB_RATIONAL_CHAR]  = DEF_RATSEP;        // Already set by = STDSYMSUB

        // Loop through the symbol substitutions
        for (uCnt = 0; uCnt < uScanTxtLenChars; uCnt += 2)
        {
            LPWCHAR lpw;

            // Ensure the 1st char is a Standard Symbol
            lpw = strchrW (wStdSym, *(*lplpwText)++);

            // If it is, ...
            if (lpw)
                wSymSub[lpw - wStdSym] = *(*lplpwText)++;
            else
                goto FORMAT_EXIT;
        } // End FOR

        // Save as the text length (including trailing zero)
        uCopyTxtLenChars = countof (STDSYMSUB);

        // Copy the text to local memory
        CopyMemoryW (ByteAddr (lpfsLocalVars->lpfsCur, offTxt),
                     wSymSub,
                     uCopyTxtLenChars);
    } // End IF/ELSE

    // Skip over it
    lpfsLocalVars->uFsNxt += uCopyTxtLenChars * sizeof (WCHAR);

    // Ensure properly terminated
    *(LPWCHAR) ByteAddr (lpfsLocalVars->lpfsCur, lpfsLocalVars->uFsNxt) = WC_EOS;

    //   and skip over it
    lpfsLocalVars->uFsNxt += sizeof (WCHAR);

    // Point to the char after the trailing delimiter
    *lplpwText = &lpwNext[1];

    // Save the copied text length
    lpfsLocalVars->lpfsCur->fsWid = uCopyTxtLenChars;

    // Skip over the chars scanned
    //   including the leading "1 +" and trailing "+ 1" delimiters
    // ("- 1" to account for the ++ at the end of the FOR stmt)
    lpfsLocalVars->uChar   += 1 + uScanTxtLenChars + 1 - 1;
    lpfsLocalVars->lpwszCur = &lpwNext[1];

    return offTxt;

FORMAT_EXIT:
    return 0;
} // End SaveText


//***************************************************************************
//  $fnRepAcc
//
//  Accumulate a repetition factor
//***************************************************************************

UBOOL fnRepAcc
    (LPFSLOCALVARS lpfsLocalVars)           // Ptr to FmtSpecStr local vars

{
    // Ensure we haven't seen this before
    if (lpfsLocalVars->lpfsCur->bRep)
        goto FORMAT_EXIT;

    // Mark as present
    lpfsLocalVars->lpfsCur->bRep = TRUE;

    // Scan the text for the repetition count
    // On return:
    //   lpfsLocalVars->uChar and lpfsLocalVars->lpwszCur are incremented.
    if (1 EQ ScanNumberFS (lpfsLocalVars, L"%u", &lpfsLocalVars->lpfsCur->fsRep, FALSE, 1))
        return TRUE;

    // Fall through to error code

FORMAT_EXIT:
    return fnError (lpfsLocalVars);
} // End fnRepAcc


//***************************************************************************
//  $ScanNumberFS
//
//  Scan for a number in the format string
//
//  Returns the # fields scanned.
//  lpfsLocalVars->uChar and lpfsLocalVars->lpwszCur are incremented.
//***************************************************************************

UINT ScanNumberFS
    (LPFSLOCALVARS lpfsLocalVars,           // Ptr to FmtSpecStr local vars
     LPWCHAR       lpwFmtFmt,               // Ptr to format str (e.g., L"%u")
     LPUINT        lpuVal,                  // Ptr to output save area
     UBOOL         bAllowNeg,               // TRUE iff negative #s allowed
     int           iMinCnt)                 // Minimum # of values allowed

{
    WCHAR wch;                              // Temporary char for sign (if any)
    UBOOL bNeg;                             // TRUE iff the number is negative
    UINT  uTxtLenChars;                     // Text length of number in WCHARs

    // Skip over any leading blanks
    while (IsWhiteW (*lpfsLocalVars->lpwszCur))
    {
        // Skip over it
        lpfsLocalVars->lpwszCur++;
        lpfsLocalVars->uChar   ++;
    } // End WHILE

    // If we allow negatives, ...
    if (bAllowNeg)
    {
        // Handle negative sign
        wch = *lpfsLocalVars->lpwszCur;
        bNeg = (wch EQ UTF16_OVERBAR || wch EQ L'-');
    } else
        bNeg = FALSE;
    // Scan for the number
    if (iMinCnt > sscanfW (&lpfsLocalVars->lpwszCur[bNeg], lpwFmtFmt, lpuVal))
        return 0;

    if (bNeg)
        *lpuVal = -(int) *lpuVal;

    // Find the length of the scanned numbers
    uTxtLenChars = bNeg + (UINT) strspnW (&lpfsLocalVars->lpwszCur[bNeg], L"0123456789");

    // Skip over the # chars we scanned
    // ("- 1" to account for the ++ at the end of the FOR stmt)
    lpfsLocalVars->uChar    += uTxtLenChars - 1;
    lpfsLocalVars->lpwszCur += uTxtLenChars;

    return 1;
} // End ScanNumberFS


//***************************************************************************
//  $fnSetModO
//
//  Save the value and text of an O-qualifier in a chain
//    because there can be more than one O-qualifier.
//
//  Ov<text>    where v is a signed integer or decimal #.
//  O<text>     is shorthand for O0<text>
//***************************************************************************

UBOOL fnSetModO
    (LPFSLOCALVARS lpfsLocalVars)           // Ptr to FmtSpecStr local vars

{
    UINT     offTxt,                        // Offset to saved text
             uScanTxtLenChars;              // Scanned text length in WCHARs
    LPWCHAR  lpwCur,                        // Ptr to current text
             lpwDec;                        // Ptr to decimal point (if any)
    LPOCHAIN lpOChain;                      // Ptr to this O-chain
    WCHAR    wch;                           // Temporary char for sign (if any)
    UBOOL    bNeg;                          // TRUE iff the number is negative

    Assert (lpfsLocalVars->lpwszCur[0] EQ L'O');

    // ***FIXME*** -- Allow RAT (and VFP?) numbers

    // Skip over it
    lpfsLocalVars->lpwszCur++;
    lpfsLocalVars->uChar++;

    // Save ptr to text to scan
    lpwCur = lpfsLocalVars->lpwszCur;

    // Save ptr to the new O-chain
    lpOChain = (LPOCHAIN) ByteAddr (lpfsLocalVars->lpfsCur, lpfsLocalVars->uFsNxt);

    // Initialize the O-chain entry
    ZeroMemory (lpOChain, sizeof (OCHAIN));

    // Skip over it
    lpfsLocalVars->uFsNxt += sizeof (OCHAIN);

    // Link it into the O-chain
    lpOChain->offPrvChain             = lpfsLocalVars->lpfsCur->offOchain;
    lpfsLocalVars->lpfsCur->offOchain = (UINT) ByteDiff (lpOChain, lpfsLocalVars->lpfsCur);

    // Mark as present
    lpfsLocalVars->lpfsCur->bO = TRUE;

    // Handle negative sign
    wch = lpwCur[0];
    bNeg = (wch EQ UTF16_OVERBAR || wch EQ L'-');

    // Get position of decimal point (if any)
    lpwDec = strchrW (&lpwCur[bNeg], L'.');

    // If there's a decimal point, but it's beyond the consecutive sequence
    //   of digits and a decimal point, there's no decimal point
    //   (e.g., 'O1<one>F10.2')
    if (lpwDec NE NULL
     && (UINT) (lpwDec - &lpwCur[bNeg]) > strspnW (&lpwCur[bNeg], L"0123456789."))
        lpwDec = NULL;

    // Scan for the number
    // Split cases based upon the # args
    switch (sscanfW (&lpwCur[bNeg], L"%lf", &lpOChain->aplFltVal))
    {
        case 0:             // No fields scanned
            lpOChain->aplIntVal = 0; // Default value for no fields
            lpOChain->aplFltVal = 0; // ...

            break;

        case 1:             // One field scanned
            if (bNeg)
                lpOChain->aplFltVal = -lpOChain->aplFltVal;

            // Save as integer, too
            lpOChain->aplIntVal = (APLINT) lpOChain->aplFltVal;

            // Find the length of the integer part of the scanned number in chars
            uScanTxtLenChars = bNeg + (UINT) strspnW (&lpwCur[bNeg], L"0123456789");

            // If there's a decimal point
            if (lpwDec)
            {
                // Count in the decimal point
                uScanTxtLenChars++;
                lpwDec++;

                // Add in the length of the fractional part
                uScanTxtLenChars += (UINT) strspnW (lpwDec, L"0123456789");
            } // End IF

            // Skip over it
            lpfsLocalVars->lpwszCur += uScanTxtLenChars;
            lpfsLocalVars->uChar    += uScanTxtLenChars;

            // Save ptr to text to scan
            lpwCur = lpfsLocalVars->lpwszCur;

            break;

        default:
            goto FORMAT_EXIT;
    } // End SWITCH

    // Save the text to the FMTSPECSTR save area
    // On return:
    //    lpwCur points to the char after the trailing delimiter
    offTxt = SaveText (lpfsLocalVars, &lpwCur, TRUE);

    // If it's in error, ...
    if (offTxt EQ 0)
        goto FORMAT_EXIT;

    // Mark as inuse
    lpfsLocalVars->lpfsCur->bInuse = TRUE;

    return TRUE;

FORMAT_EXIT:
    return fnError (lpfsLocalVars);
} // End fnSetModO


//***************************************************************************
//  $fnSetQual
//
//  Save a qualifier or format specification
//***************************************************************************

UBOOL fnSetQual
    (LPFSLOCALVARS lpfsLocalVars)           // Ptr to FmtSpecStr local vars

{
    LPFMTSPECSTR lpfsCur;                   // Ptr to current FMTSPECSTR
    UINT         offTxt;                    // Offset to saved text
    LPWCHAR      lpwCur;                    // Ptr to current text

    // Get a ptr to the current FMTSPECSTR
    lpfsCur = lpfsLocalVars->lpfsCur;

    // Split cases based upon the letter
    switch (*lpfsLocalVars->lpwszCur)
    {
        case L'A':
            // Save as the format specification
            lpfsCur->fmtSpecVal = FMTSPECVAL_A;

            // Skip over the A-fmtspec
            lpfsLocalVars->lpwszCur++;
            lpfsLocalVars->uChar++;

            // Scan the text for the width
            // On return:
            //   lpfsLocalVars->uChar and lpfsLocalVars->lpwszCur are incremented.
            if (1 NE ScanNumberFS (lpfsLocalVars, L"%u", &lpfsCur->fsWid, FALSE, 1))
                goto FORMAT_EXIT;

            // Modifiers allowed for A-format:        L           R S   Rep
            // ...       not ...             :  B C K   M N O P Q     Z

            // Check for disallowed modifiers
            if (0
             || lpfsCur->bB
             || lpfsCur->bC
             || lpfsCur->bK
//////////// || lpfsCur->bL
             || lpfsCur->bM
             || lpfsCur->bN
             || lpfsCur->bO
             || lpfsCur->bP
             || lpfsCur->bQ
//////////// || lpfsCur->bR
//////////// || lpfsCur->bS
             || lpfsCur->bZ
//////////// || lpfsCur->bRep
               )
                goto FORMAT_EXIT;
            break;

        case L'B':
            // Check to see if already present
            if (lpfsCur->bB)
                goto FORMAT_EXIT;

            // Mark as present
            lpfsCur->bB = TRUE;

            break;

        case L'C':
            // Check to see if already present
            if (lpfsCur->bC)
                goto FORMAT_EXIT;

            // Mark as present
            lpfsCur->bC = TRUE;

            break;

        case L'E':
            // Save as the format specification
            lpfsCur->fmtSpecVal = FMTSPECVAL_E;

            // Skip over the E-fmtspec
            lpfsLocalVars->lpwszCur++;
            lpfsLocalVars->uChar++;

            // Scan the text for the width
            // On return:
            //   lpfsLocalVars->uChar and lpfsLocalVars->lpwszCur are incremented.
            if (1 NE ScanNumberFS (lpfsLocalVars, L"%u", &lpfsCur->fsWid, FALSE, 1))
                goto FORMAT_EXIT;

            if (*lpfsLocalVars->lpwszCur++ NE L'.')
                goto FORMAT_EXIT;

            // Skip over the decimal separator and the "- 1" for the FOR stmt
            lpfsLocalVars->uChar += 1 + 1;

            // Scan the text for the precision
            // On return:
            //   lpfsLocalVars->uChar and lpfsLocalVars->lpwszCur are incremented.
            if (1 NE ScanNumberFS (lpfsLocalVars, L"%u", &lpfsCur->fsDig, FALSE, 1))
                goto FORMAT_EXIT;

            // Disallow 0 significant digits
            if (lpfsCur->fsDig EQ 0)
                goto FORMAT_EXIT;

            // Modifiers allowed for E-format:  B   K L M N O P Q R S Z Rep
            // ...       not ...             :    C

            // Check for disallowed modifiers
            if (0
//////////// || lpfsCur->bB
             || lpfsCur->bC
//////////// || lpfsCur->bK
//////////// || lpfsCur->bL
//////////// || lpfsCur->bM
//////////// || lpfsCur->bN
//////////// || lpfsCur->bO
//////////// || lpfsCur->bP
//////////// || lpfsCur->bQ
//////////// || lpfsCur->bR
//////////// || lpfsCur->bS
//////////// || lpfsCur->bZ
//////////// || lpfsCur->bRep
               )
                goto FORMAT_EXIT;
            break;

        case L'F':
            // Save as the format specification
            lpfsCur->fmtSpecVal = FMTSPECVAL_F;

            // Skip over the F-fmtspec
            lpfsLocalVars->lpwszCur++;
            lpfsLocalVars->uChar++;

            // Scan the text for the width
            // On return:
            //   lpfsLocalVars->uChar and lpfsLocalVars->lpwszCur are incremented.
            if (1 NE ScanNumberFS (lpfsLocalVars, L"%u", &lpfsCur->fsWid, FALSE, 1))
                goto FORMAT_EXIT;

            if (*lpfsLocalVars->lpwszCur++ NE L'.')
                goto FORMAT_EXIT;

            // Skip over the decimal separator and the "- 1" for the FOR stmt
            lpfsLocalVars->uChar += 1 + 1;

            // Scan the text for the precision
            // On return:
            //   lpfsLocalVars->uChar and lpfsLocalVars->lpwszCur are incremented.
            if (1 NE ScanNumberFS (lpfsLocalVars, L"%u", &lpfsCur->fsDig, FALSE, 1))
                goto FORMAT_EXIT;

            // Modifiers allowed for F-format:  all
            // ...       not ...             :  none

////////////// Check for disallowed modifiers
////////////if (0
//////////// || lpfsCur->bB
//////////// || lpfsCur->bC
//////////// || lpfsCur->bK
//////////// || lpfsCur->bL
//////////// || lpfsCur->bM
//////////// || lpfsCur->bN
//////////// || lpfsCur->bO
//////////// || lpfsCur->bP
//////////// || lpfsCur->bQ
//////////// || lpfsCur->bR
//////////// || lpfsCur->bS
//////////// || lpfsCur->bZ
//////////// || lpfsCur->bRep
////////////   )
////////////    goto FORMAT_EXIT;
            break;

        case L'R':
            // Save as the format specification
            lpfsCur->fmtSpecVal = FMTSPECVAL_R;

            // Skip over the R-fmtspec
            lpfsLocalVars->lpwszCur++;
            lpfsLocalVars->uChar++;

            // Scan the text for the width
            // On return:
            //   lpfsLocalVars->uChar and lpfsLocalVars->lpwszCur are incremented.
            if (1 NE ScanNumberFS (lpfsLocalVars, L"%u", &lpfsCur->fsWid, FALSE, 1))
                goto FORMAT_EXIT;

            if (*lpfsLocalVars->lpwszCur++ NE L'.')
                goto FORMAT_EXIT;

            // Skip over the decimal separator and the "- 1" for the FOR stmt
            lpfsLocalVars->uChar += 1 + 1;

            // Scan the text for the precision
            // On return:
            //   lpfsLocalVars->uChar and lpfsLocalVars->lpwszCur are incremented.
            if (1 NE ScanNumberFS (lpfsLocalVars, L"%u", &lpfsCur->fsDig, FALSE, 1))
                goto FORMAT_EXIT;

            // Modifiers allowed for R-format:  all
            // ...       not ...             :  none

////////////// Check for disallowed modifiers
////////////if (0
//////////// || lpfsCur->bB
//////////// || lpfsCur->bC
//////////// || lpfsCur->bK
//////////// || lpfsCur->bL
//////////// || lpfsCur->bM
//////////// || lpfsCur->bN
//////////// || lpfsCur->bO
//////////// || lpfsCur->bP
//////////// || lpfsCur->bQ
//////////// || lpfsCur->bR
//////////// || lpfsCur->bS
//////////// || lpfsCur->bZ
//////////// || lpfsCur->bRep
////////////   )
////////////    goto FORMAT_EXIT;
            break;

        case L'G':
            // Save as the format specification
            lpfsCur->fmtSpecVal = FMTSPECVAL_G;

            // Skip over the G-fmtspec
            lpfsLocalVars->lpwszCur++;
            lpfsLocalVars->uChar++;

            // Save ptr to text to scan
            lpwCur = lpfsLocalVars->lpwszCur;

            // Scan the text for the pattern
            // On return:
            //    lpwCur points to the char after the trailing delimiter
            offTxt = SaveText (lpfsLocalVars, &lpwCur, TRUE);

            // If it's in error, ...
            if (offTxt EQ 0)
                goto FORMAT_EXIT;

            // Save the offset and length
            lpfsCur->offGtxt = offTxt;
            lpfsCur->fsWid   = lstrlenW ((LPWCHAR) ByteAddr (lpfsCur, offTxt));

            // Modifiers allowed for G-format:  B   K   M   O P   R S   Rep
            // ...       not                 :    C   L   N     Q     Z

            // Check for disallowed modifiers
            if (0
//////////// || lpfsCur->bB
             || lpfsCur->bC
//////////// || lpfsCur->bK
             || lpfsCur->bL
//////////// || lpfsCur->bM
             || lpfsCur->bN
//////////// || lpfsCur->bO
//////////// || lpfsCur->bP
             || lpfsCur->bQ
//////////// || lpfsCur->bR
//////////// || lpfsCur->bS
             || lpfsCur->bZ
//////////// || lpfsCur->bRep
               )
                goto FORMAT_EXIT;
            break;

        case L'I':
            // Save as the format specification
            lpfsCur->fmtSpecVal = FMTSPECVAL_I;

            // Skip over the I-fmtspec
            lpfsLocalVars->lpwszCur++;
            lpfsLocalVars->uChar++;

            // Scan the text for the width
            // On return:
            //   lpfsLocalVars->uChar and lpfsLocalVars->lpwszCur are incremented.
            if (1 NE ScanNumberFS (lpfsLocalVars, L"%u", &lpfsCur->fsWid, FALSE, 1))
                goto FORMAT_EXIT;

            // Modifiers allowed for I-format:  all
            // ...       not ...             :  none

////////////// Check for disallowed modifiers
////////////if (0
//////////// || lpfsCur->bB
//////////// || lpfsCur->bC
//////////// || lpfsCur->bK
//////////// || lpfsCur->bL
//////////// || lpfsCur->bM
//////////// || lpfsCur->bN
//////////// || lpfsCur->bO
//////////// || lpfsCur->bP
//////////// || lpfsCur->bQ
//////////// || lpfsCur->bR
//////////// || lpfsCur->bS
//////////// || lpfsCur->bZ
//////////// || lpfsCur->bRep
////////////   )
////////////    goto FORMAT_EXIT;
            break;

        case L'K':
            // Check to see if already present
            if (lpfsCur->bK)
                goto FORMAT_EXIT;

            // Mark as present
            lpfsCur->bK = TRUE;

            // Skip over the K-modifier
            lpfsLocalVars->lpwszCur++;
            lpfsLocalVars->uChar++;

            // Scan the text for the (signed) scale
            // On return:
            //   lpfsLocalVars->uChar and lpfsLocalVars->lpwszCur are incremented.
            if (1 NE ScanNumberFS (lpfsLocalVars, L"%u", &lpfsCur->fsScl, TRUE, 1))
                goto FORMAT_EXIT;
            break;

        case L'L':
            // Check to see if already present
            if (lpfsCur->bL)
                goto FORMAT_EXIT;

            // Mark as present
            lpfsCur->bL = TRUE;

            break;

        case L'T':
            // Save as the format specification
            lpfsCur->fmtSpecVal = FMTSPECVAL_T;

            // Skip over the T-fmtspec
            lpfsLocalVars->lpwszCur++;
            lpfsLocalVars->uChar++;

            // Scan the text for the width (if present)
            // On return:
            //   lpfsLocalVars->uChar and lpfsLocalVars->lpwszCur are incremented.
            offTxt = ScanNumberFS (lpfsLocalVars, L"%u", &lpfsCur->fsWid, FALSE, 0);
            if (1 < offTxt)
                goto FORMAT_EXIT;

            // If no numbers scanned, back off uChar by 1
            //   because of the ++ in the outer FOR stmt
            if (offTxt EQ 0)
                lpfsLocalVars->uChar--;

            // Modifiers allowed for T-format:  none
            // ...       not ...             :  all

            // Check for disallowed modifiers
            if (0
             || lpfsCur->bB
             || lpfsCur->bC
             || lpfsCur->bK
             || lpfsCur->bL
             || lpfsCur->bM
             || lpfsCur->bN
             || lpfsCur->bO
             || lpfsCur->bP
             || lpfsCur->bQ
             || lpfsCur->bR
             || lpfsCur->bS
             || lpfsCur->bZ
             || lpfsCur->bRep
               )
                goto FORMAT_EXIT;
            break;

        case L'X':
            // Save as the format specification
            lpfsCur->fmtSpecVal = FMTSPECVAL_X;

            // Skip over the X-fmtspec
            lpfsLocalVars->lpwszCur++;
            lpfsLocalVars->uChar++;

            // Scan the text for the (signed) width
            // On return:
            //   lpfsLocalVars->uChar and lpfsLocalVars->lpwszCur are incremented.
            if (1 NE ScanNumberFS (lpfsLocalVars, L"%u", &lpfsCur->fsWid, TRUE, 1))
                goto FORMAT_EXIT;

            // Modifiers allowed for X-format:                          Rep
            // ...       not ...             :  B C K L M N O P Q R S Z

            // Check for disallowed modifiers
            if (0
             || lpfsCur->bB
             || lpfsCur->bC
             || lpfsCur->bK
             || lpfsCur->bL
             || lpfsCur->bM
             || lpfsCur->bN
             || lpfsCur->bO
             || lpfsCur->bP
             || lpfsCur->bQ
             || lpfsCur->bR
             || lpfsCur->bS
             || lpfsCur->bZ
//////////// || lpfsCur->bRep
               )
                goto FORMAT_EXIT;
            break;

        case L'Z':
            // Check to see if already present
            if (lpfsCur->bZ)
                goto FORMAT_EXIT;

            // Mark as present
            lpfsCur->bZ = TRUE;

            break;

        case WC_SQ:
        case WC_DQ:
        case UTF16_QUAD:
        case UTF16_QUAD2:
        case UTF16_QUOTEQUAD:
        case UTF16_DIERESIS:
        case UTF16_LEFTCARET:
        case UTF16_LEFTSHOE:
            // Save as the format specification
            lpfsCur->fmtSpecVal = FMTSPECVAL_TXT;

            // Save ptr to text to scan
            lpwCur = lpfsLocalVars->lpwszCur;

            // Save the text
            // On return:
            //    lpwCur points to the char after the trailing delimiter
            offTxt = SaveText (lpfsLocalVars, &lpwCur, TRUE);

            // If it's in error, ...
            if (offTxt EQ 0)
                goto FORMAT_EXIT;

            // Save the offset
            lpfsLocalVars->lpfsCur->offText = offTxt;

            // Modifiers allowed for Text Insertion:  none
            // ...       not                       :  all

            // Check for disallowed modifiers
            if (0
             || lpfsCur->bB
             || lpfsCur->bC
             || lpfsCur->bK
             || lpfsCur->bL
             || lpfsCur->bM
             || lpfsCur->bN
             || lpfsCur->bO
             || lpfsCur->bP
             || lpfsCur->bQ
             || lpfsCur->bR
             || lpfsCur->bS
             || lpfsCur->bZ
             || lpfsCur->bRep
               )
                goto FORMAT_EXIT;
            break;

        defstop
            break;
    } // End SWITCH

    // Mark as inuse
    lpfsLocalVars->lpfsCur->bInuse = TRUE;

    return TRUE;

FORMAT_EXIT:
    return fnError (lpfsLocalVars);
} // End fnSetQual


//***************************************************************************
//  $fnError
//
//  Mark as in error
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- fnError"
#else
#define APPEND_NAME
#endif

UBOOL fnError
    (LPFSLOCALVARS lpfsLocalVars)           // Ptr to FmtSpecStr local vars

{
    // Save the error index
    lpfsLocalVars->uErrIndex = lpfsLocalVars->uChar;

    // Set the error message
    ErrorMessageIndirectToken (ERRMSG_FORMAT_ERROR APPEND_NAME,
                               lpfsLocalVars->lptkFunc);
    return FALSE;
} // End fnError
#undef  APPEND_NAME


//***************************************************************************
//  $FmtSpec_EM
//
//  Format Specification FSA
//***************************************************************************

UBOOL FmtSpec_EM
    (LPWCHAR       lpwszLine,               // Ptr to fmt spec string
     UINT          uActLen,                 // Actual length of lpwszLine
     LPFMTSPECSTR  lpwszSpec,               // Ptr to save area for FmtSpecStr
     LPFMTSPECSTR *lplpfsEnd,               // Ptr to ptr to end of the FMTSPECSTRs
     LPTOKEN       lptkFunc)                // Ptr to function token

{
    UINT         uChar;                     // Loop counter
    WCHAR        wchOrig;                   // The original char
    FSCOLINDICES colIndex;                  // The translated char for tokenization as a FSCOL_*** value
    FS_ACTION    fnAction_EM;               // Ptr to action routine
    FSLOCALVARS  fsLocalVars = {0};         // Local vars
    UBOOL        bRet = FALSE;              // TRUE iff the result is valid

    // Save local vars in struct which we pass to each FSA action routine
    fsLocalVars.State     = FSROW_INIT;     // Initial states
    fsLocalVars.lpwszCur  = lpwszLine;      // Just so it has a known value
    fsLocalVars.lptkFunc  = lptkFunc;       // Ptr to function token in case of error
    fsLocalVars.lpfsCur   = lpwszSpec;      // Ptr to FMTSPECSTR area

    // Initialize the FmtSpecStr output area
    InitFmtSpecStr (&fsLocalVars);

    // Loop through the chars in the format string
    for (uChar = 0; uChar <= uActLen; uChar++)
    {
        // Use a FSA to tokenize the line

        // Save current index (may be modified by an action)
        fsLocalVars.uChar = uChar;

        // Save pointer to current wch
        fsLocalVars.lpwszCur = &lpwszLine[uChar];

        if (uChar EQ uActLen)
            wchOrig = WC_EOS;
        else
            wchOrig = lpwszLine[uChar];

        // Strip out EOL check so we don't confuse a zero-value char with EOL
        if (uChar EQ uActLen)
            colIndex = FSCOL_EOL;
        else
            colIndex = CharTransFS (wchOrig, &fsLocalVars);

        // Get the action and new state
        fnAction_EM = fsaActTableFS[fsLocalVars.State][colIndex].fnAction;
        fsLocalVars.State = fsaActTableFS[fsLocalVars.State][colIndex].iNewState;

        // Check for primary action
        if (fnAction_EM
         && !(*fnAction_EM) (&fsLocalVars))
            goto ERROR_EXIT;

        // Split cases based upon the return code
        switch (fsLocalVars.State)
        {
            case FSROW_EXIT:
                // Mark as successful
                bRet = TRUE;

                goto NORMAL_EXIT;
        } // End SWITCH

        // Get next index (may have been modified by an action)
        uChar = fsLocalVars.uChar;
    } // End FOR

    // We should never get here as we process the
    //   trailing zero in the input line which should
    //   exit from one of the actions with FSROW_EXIT.
    DbgStop ();         // We should never get here

ERROR_EXIT:
NORMAL_EXIT:
    // Return the address of the end of the FMTSPECSTRs
    *lplpfsEnd = (LPFMTSPECSTR) ByteAddr (fsLocalVars.lpfsCur, fsLocalVars.uFsNxt);

    return bRet;
} // End FmtSpec_EM


//***************************************************************************
//  $CharTransFS
//
//  Translate a character in preparation for format spec
//***************************************************************************

FSCOLINDICES CharTransFS
    (WCHAR         wchOrig,             // Char to translate into a FSCOL_xxx index
     LPFSLOCALVARS lpfsLocalVars)       // Ptr to FmtSpec_EM local vars

{
    // Split cases based upon the incoming character
    switch (wchOrig)
    {
        case L'A':
            return FSCOL_A;

        case L'B':
            return FSCOL_B;

        case L'C':
            return FSCOL_C;

        case L'E':
            return FSCOL_E;

        case L'F':
            return FSCOL_F;

        case L'G':
            return FSCOL_G;

        case L'I':
            return FSCOL_I;

        case L'K':
            return FSCOL_K;

        case L'L':
            return FSCOL_L;

        case L'M':
            return FSCOL_M;

        case L'N':
            return FSCOL_N;

        case L'O':
            return FSCOL_O;

        case L'P':
            return FSCOL_P;

        case L'Q':
            return FSCOL_Q;

        case L'R':
            return FSCOL_R;

        case L'S':
            return FSCOL_S;

        case L'T':
            return FSCOL_T;

        case L'X':
            return FSCOL_X;

        case L'Z':
            return FSCOL_Z;

        case L'0':
        case L'1':
        case L'2':
        case L'3':
        case L'4':
        case L'5':
        case L'6':
        case L'7':
        case L'8':
        case L'9':
            return FSCOL_DIGIT;

        case L' ':
            return FSCOL_SPACE;

        case WC_SQ:
        case WC_DQ:
        case UTF16_QUAD:
        case UTF16_QUAD2:
        case UTF16_QUOTEQUAD:
        case UTF16_DIERESIS:
        case UTF16_LEFTCARET:
        case UTF16_LEFTSHOE:
            return FSCOL_TXTDELIM;

        case UTF16_COMMA:
            return FSCOL_COMMA;

        case UTF16_LEFTPAREN:
            return FSCOL_LEFTPAREN;

        case UTF16_RIGHTPAREN:
            return FSCOL_RIGHTPAREN;

        default:
            return FSCOL_ALPHA;
    } // End SWITCH
} // End CharTransFS


#ifdef DEBUG
//***************************************************************************
//  $InitFsaTabFS
//
//  Ensure the FS FSA table has been properly setup
//***************************************************************************

void InitFsaTabFS
    (void)

{
    // Ensure we calculated the lengths properly
    Assert (sizeof (fsaActTableFS) EQ (FSCOL_LENGTH * sizeof (FSACTSTR) * FSROW_LENGTH));
} // End InitFsaTabFS
#endif


//***************************************************************************
//  End of File: fmtspec.c
//***************************************************************************
