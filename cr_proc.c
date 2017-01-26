//***************************************************************************
//  NARS2000 -- Conversion to Rational FSA Routines
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
#include "headers.h"

#define mpq_zero_p(a)   (mpq_cmp_si ((a), 0, 1) EQ 0)

#define crIniNeg        crIniSeg
#define crIniDig        crIniSeg
#define crIniDec        NULL
#define crIniInf        crIniSeg
#define crAccDig        crAccSeg
#define crAccDec        crAccSeg
#define crAccExp        crAccSeg
#define crAccInf        crAccSeg
#define crEndInt        crEndSeg
#define crEndDec        crEndSeg
#define crEndInf        crEndSeg

CRACTSTR fsaActTableCR [][CRCOL_LENGTH]
#ifndef PROTO
 =
// New State       Action #1  Action #2
{   // CRROW_INIT       Initial state ('')
 {{CRROW_NEG     , crIniNeg , crAccDig  },      // 00:  Overbar or minus sign
  {CRROW_INT1    , crIniDig , crAccDig  },      // 01:  Digit
  {CRROW_DEC     , crIniDec , crAccDec  },      // 02:  Decimal point
  {CRROW_ERROR   , NULL     , crError   },      // 03:  E
  {CRROW_ERROR   , NULL     , crError   },      // 04:  /
  {CRROW_ERROR   , NULL     , crError   },      // 05:  x
  {CRROW_INF     , crIniInf , crAccInf  },      // 06:  Infinity
  {CRROW_ERROR   , NULL     , crError   },      // 07:  All other chars
 },
    // CRROW_NEG        Negative Sign ('-')
 {{CRROW_ERROR   , NULL     , crError   },      // 00:  Overbar or minus sign
  {CRROW_INT1    , crAccDig , NULL      },      // 01:  Digit
  {CRROW_DEC     , crAccDig , crIniDec  },      // 02:  Decimal point
  {CRROW_ERROR   , NULL     , crError   },      // 03:  E
  {CRROW_INIT    , NULL     , crError   },      // 04:  /
  {CRROW_EXIT    , NULL     , crError   },      // 05:  x
  {CRROW_INF     , crAccInf , NULL      },      // 06:  Infinity
  {CRROW_EXIT    , crEndInt , crExit    },      // 07:  All other chars
 },
    // CRROW_INT1       Next of Integer State ('-0123456789')
 {{CRROW_ERROR   , NULL     , crError   },      // 00:  Overbar or minus sign
  {CRROW_INT1    , crAccDig , NULL      },      // 01:  Digit
  {CRROW_DEC     , crAccDig , crIniDec  },      // 02:  Decimal point
  {CRROW_EXP0    , crEndInt , crIniExp  },      // 03:  E
  {CRROW_INIT    , crEndInt , crIniRat  },      // 04:  /
  {CRROW_EXIT    , crEndInt , crIniExt  },      // 05:  x
  {CRROW_ERROR   , NULL     , crError   },      // 06:  Infinity
  {CRROW_EXIT    , crEndInt , crExit    },      // 07:  All other chars
 },
    // CRROW_DEC        Decimal point ('.')
 {{CRROW_ERROR   , NULL     , crError   },      // 00:  Overbar or minus sign
  {CRROW_DEC     , crAccDec , NULL      },      // 01:  Digit
  {CRROW_ERROR   , NULL     , crError   },      // 02:  Decimal point
  {CRROW_EXP0    , crEndDec , crIniExp  },      // 03:  E
  {CRROW_INIT    , crEndDec , crIniRat  },      // 04:  /
  {CRROW_EXIT    , crEndDec , crIniExt  },      // 05:  x
  {CRROW_ERROR   , NULL     , crError   },      // 06:  Infinity
  {CRROW_EXIT    , crEndDec , crExit    },      // 07:  All other chars
 },
    // CRROW_EXP0       Start of Exponent State ('E')
 {{CRROW_EXP1    , NULL     , crAccExp  },      // 00:  Overbar or minus sign
  {CRROW_EXP1    , NULL     , crAccExp  },      // 01:  Digit
  {CRROW_ERROR   , NULL     , crError   },      // 02:  Decimal point
  {CRROW_ERROR   , NULL     , crError   },      // 03:  E
  {CRROW_ERROR   , NULL     , crError   },      // 04:  /
  {CRROW_ERROR   , NULL     , crError   },      // 05:  x
  {CRROW_ERROR   , NULL     , crError   },      // 06:  Infinity
  {CRROW_ERROR   , NULL     , crError   },      // 07:  All other chars
 },
    // CRROW_EXP1       Next of Exponent State ('-0123456789')
 {{CRROW_ERROR   , NULL     , crError   },      // 00:  Overbar or minus sign
  {CRROW_EXP1    , crAccExp , NULL      },      // 01:  Digit
  {CRROW_ERROR   , NULL     , crError   },      // 02:  Decimal point
  {CRROW_ERROR   , NULL     , crError   },      // 03:  E
  {CRROW_INIT    , crEndExp , crIniRat  },      // 04:  /
  {CRROW_EXIT    , crEndExp , crIniExt  },      // 05:  x
  {CRROW_ERROR   , NULL     , crError   },      // 06:  Infinity
  {CRROW_EXIT    , crEndExp , crExit    },      // 07:  All other chars
 },
    // CRROW_INF        Infinity ('!')
 {{CRROW_ERROR   , NULL     , crError   },      // 00:  Overbar or minus sign
  {CRROW_ERROR   , NULL     , crError   },      // 01:  Digit
  {CRROW_ERROR   , NULL     , crError   },      // 02:  Decimal point
  {CRROW_ERROR   , NULL     , crError   },      // 03:  E
  {CRROW_INIT    , crEndInf , crIniRat  },      // 04:  /
  {CRROW_EXIT    , crEndInf , crIniExt  },      // 05:  x
  {CRROW_ERROR   , NULL     , crError   },      // 06:  Infinity
  {CRROW_EXIT    , crEndInf , crExit    },      // 07:  All other chars
 },
}
#endif
;


//***************************************************************************
//  $crError
//
//  Signal an error
//***************************************************************************

UBOOL crError
    (LPCRLOCALVARS lpcrLocalVars)       // Ptr to <mpq_set_str2> local vars

{
    // Mark as a syntax error
    lpcrLocalVars->crRetCode = CR_SYNTAX_ERROR;

    return FALSE;
} // End crError


//***************************************************************************
//  $crIniSeg
//
//  Initialize a segment character
//***************************************************************************

UBOOL crIniSeg
    (LPCRLOCALVARS lpcrLocalVars)       // Ptr to <mpq_set_str2> local vars

{
    // Initialize the ptr to the current segment
    lpcrLocalVars->lpszStart = lpcrLocalVars->lpszCur;

    return TRUE;
} // End crIniSeg


//***************************************************************************
//  $crIniExp
//
//  Initialize an exponent
//***************************************************************************

UBOOL crIniExp
    (LPCRLOCALVARS lpcrLocalVars)       // Ptr to <mpq_set_str2> local vars

{
    Assert (lpcrLocalVars->lpszCur[0] EQ 'e'
         || lpcrLocalVars->lpszCur[0] EQ 'E');

    // Skip over the exponent separator and
    //   initialize the ptr to the current segment
    lpcrLocalVars->lpszStart = ++lpcrLocalVars->lpszCur;

    return TRUE;
} // End crIniExp


//***************************************************************************
//  $crIniRat
//
//  Initialize a new segment from numerator to denominator
//***************************************************************************

UBOOL crIniRat
    (LPCRLOCALVARS lpcrLocalVars)       // Ptr to <mpq_set_str2> local vars

{
    Assert (lpcrLocalVars->lpszCur[0] EQ 'r'
         || lpcrLocalVars->lpszCur[0] EQ '/');

    // Ensure this is the first
    if (lpcrLocalVars->bRatSep)
        return crError (lpcrLocalVars);

    // Skip over the rational separator and
    //   initialize the ptr to the current segment
    lpcrLocalVars->lpszStart = ++lpcrLocalVars->lpszCur;

    // Save the current mpqMul value into mpqRes
    mpq_set (lpcrLocalVars->mpqRes, lpcrLocalVars->mpqMul);

    // Mark as present
    lpcrLocalVars->bRatSep = TRUE;

    return TRUE;
} // End crIniRat


//***************************************************************************
//  $crIniExt
//
//  End a new segment as an Extended Integer
//***************************************************************************

UBOOL crIniExt
    (LPCRLOCALVARS lpcrLocalVars)       // Ptr to <mpq_set_str2> local vars

{
    // Skip over the character
    lpcrLocalVars->lpszCur++;

    // Ensure this is the ending char
    //   and there's no preceding rational separator, ...
    if ((lstrlen (lpcrLocalVars->lpszStart) NE
        (lpcrLocalVars->lpszCur - lpcrLocalVars->lpszStart))
     || lpcrLocalVars->bRatSep)
        return crError (lpcrLocalVars);

    // Save the current mpqMul value into mpqRes
    mpq_set (lpcrLocalVars->mpqRes, lpcrLocalVars->mpqMul);

    return TRUE;
} // End crIniExt


//***************************************************************************
//  $crExit
//
//  Exit the FSA
//***************************************************************************

UBOOL crExit
    (LPCRLOCALVARS lpcrLocalVars)       // Ptr to <mpq_set_str2> local vars

{
    // If there's a preceding rational separator, ...
    if (lpcrLocalVars->bRatSep)
    {
        _try
        {
            // Check for indeterminates:  0 {div} 0
            if (mpq_zero_p (lpcrLocalVars->mpqRes)
             && mpq_zero_p (lpcrLocalVars->mpqMul))
                // Save in the result
                lpcrLocalVars->mpqRes[0] =
                  *mpq_QuadICValue (lpcrLocalVars->mpqRes,
                                    ICNDX_0DIV0,
                                    lpcrLocalVars->mpqMul,
                                    lpcrLocalVars->mpqRes,
                                    FALSE);
            else
            // Check for indeterminates:  L {div} 0
            if (mpq_zero_p (lpcrLocalVars->mpqMul))
                // Save in the result
                lpcrLocalVars->mpqRes[0] =
                  *mpq_QuadICValue (lpcrLocalVars->mpqRes,
                                    ICNDX_DIV0,
                                    lpcrLocalVars->mpqMul,
                                    lpcrLocalVars->mpqRes,
                                    FALSE);
            else
            // Check for indeterminates:  _ {div} _ (same or different signs)
            if (mpq_inf_p (lpcrLocalVars->mpqRes)
             && mpq_inf_p (lpcrLocalVars->mpqMul))
            {
                if (mpq_sgn (lpcrLocalVars->mpqRes) EQ mpq_sgn (lpcrLocalVars->mpqMul))
                    lpcrLocalVars->mpqRes[0] =
                      *mpq_QuadICValue (lpcrLocalVars->mpqRes,
                                        ICNDX_PiDIVPi,
                                        lpcrLocalVars->mpqMul,
                                        lpcrLocalVars->mpqRes,
                                        FALSE);
                else
                    lpcrLocalVars->mpqRes[0] =
                      *mpq_QuadICValue (lpcrLocalVars->mpqRes,
                                        ICNDX_NiDIVPi,
                                        lpcrLocalVars->mpqMul,
                                        lpcrLocalVars->mpqRes,
                                        FALSE);
            } else
            // Check for -0
            if (lpcrLocalVars->lpszStart[0] EQ OVERBAR1
             && IsMpq0  (lpcrLocalVars->mpqRes)
             && mpq_sgn (lpcrLocalVars->mpqMul) > 0
             && gAllowNeg0)
            {
                // Mark as exiting with result == -0
                lpcrLocalVars->iNewState = CRROW_EXIT;
                lpcrLocalVars->crRetCode = CR_RESULT_NEG0;

                return FALSE;
            } else
            {
                // Divide the denominator into the numerator
                mpq_div (lpcrLocalVars->mpqRes,
                         lpcrLocalVars->mpqRes,
                         lpcrLocalVars->mpqMul);
            } // End IF/ELSE
        } __except (CheckException (GetExceptionInformation (), L"crExit"))
        {
            lpcrLocalVars->crRetCode = CR_DOMAIN_ERROR;

            return FALSE;
        } // __try/__except
    } else
        // Save the current mpqMul value into mpqRes
        mpq_set (lpcrLocalVars->mpqRes, lpcrLocalVars->mpqMul);

    return TRUE;
} // End crExit


//***************************************************************************
//  $crAccSeg
//
//  Accumulate a segment character
//***************************************************************************

UBOOL crAccSeg
    (LPCRLOCALVARS lpcrLocalVars)       // Ptr to <mpq_set_str2> local vars

{
    // Skip over the character
    lpcrLocalVars->lpszCur++;

    return TRUE;
} // End crAccSeg


//***************************************************************************
//  $crEndSeg
//
//  End a segment (either _INT or _DEC)
//***************************************************************************

UBOOL crEndSeg
    (LPCRLOCALVARS lpcrLocalVars)       // Ptr to <mpq_set_str2> local vars

{
    LPCHAR lpDec = NULL;                // Ptr to decimal point (NULL if none)
    int    res = 0;                     // mpq_set_str result
    char   chZap,                       // Zapped char
           chZapRat;                    // ...    rational sep

    // Convert the segment from lpcrLocalVars.lpszStart into a rational number
    // This segment may include a decimal point

    // If the next char is a rational separator, ...
    if (CharTransCR (lpcrLocalVars->lpszCur[0]) EQ CRCOL_RAT)
    {
        // Save and zap the rational sep so as to terminate the integer part of the string
        chZapRat = lpcrLocalVars->lpszCur[0];
                   lpcrLocalVars->lpszCur[0] = AC_EOS;
    } else
       chZapRat = AC_EOS;

    // Check for a decimal point
    lpDec = strchr (lpcrLocalVars->lpszStart, '.');
    if (lpDec NE NULL)
    {
        size_t frcLen;                  // Length (in chars) of the fractional part
        mpq_t  mpqt_frc = {0},          // Temp vars
               mpqt_div = {0};          // ...

        // Initialize to 0/1
        mpq_init (mpqt_frc);
        mpq_init (mpqt_div);

        // If the integer part is non-empty, ...
        if (lpDec > lpcrLocalVars->lpszStart)
        {
            // Save and zap the decimal point so as to terminate the integer part of the string
            chZap = lpDec[0];
                    lpDec[0] = AC_EOS;
            // Convert the integer part of the string to a multiple precision integer
            res = mpq_set_str (lpcrLocalVars->mpqMul,
                               lpcrLocalVars->lpszStart,
                               lpcrLocalVars->base);
            // Restore the zapped char
            lpDec[0] = chZap;

            if (res NE 0)
                goto ERROR_EXIT;
        } // End IF

        // Get the length of the fractional part
        frcLen = strspn (&lpDec[1], "0123456789");

        // If the fractional part is non-empty, ...
        if (frcLen NE 0)
        {
            // Save and zap the ending char
            chZap = lpDec[1 + frcLen];
                    lpDec[1 + frcLen] = AC_EOS;

            // Convert the fractional part of the string to a multiple precision integer
            res = mpz_set_str (mpq_numref (mpqt_frc),
                              &lpDec[1],
                               lpcrLocalVars->base);
            // Restore the zapped char
            lpDec[1 + frcLen] = chZap;

            if (res NE 0)
                goto ERROR_EXIT;
        } // End IF

        // Calculate the fractional part's divisor as base^frcLen
        mpz_ui_pow_ui (mpq_numref (mpqt_div),
                       lpcrLocalVars->base,
                       frcLen);
        // Divide the fractional part by base^frcLen
        mpq_div (mpqt_frc,
                 mpqt_frc,
                 mpqt_div);
        // If the integer part is non-negative, ...
        if (mpq_sgn (lpcrLocalVars->mpqMul) >= 0)
            // Add the fractional part to the integer part
            mpq_add (lpcrLocalVars->mpqMul,
                     lpcrLocalVars->mpqMul,
                     mpqt_frc);
        else
            // Subtract the fractional part from the integer part
            mpq_sub (lpcrLocalVars->mpqMul,
                     lpcrLocalVars->mpqMul,
                     mpqt_frc);
ERROR_EXIT:
        mpq_clear (mpqt_div);
        mpq_clear (mpqt_frc);
    } else
    {
        size_t intLen;                  // Length (in chars) of the integer part

        // Get the length of the integer part
        intLen = strspn (lpcrLocalVars->lpszStart, OVERBAR1_STR INFINITY1_STR "0123456789");

        // Save and zap the ending char
        chZap = lpcrLocalVars->lpszStart[intLen];
                lpcrLocalVars->lpszStart[intLen] = AC_EOS;

        // Convert the integer part of the string to a multiple precision integer
        res = mpq_set_str (lpcrLocalVars->mpqMul,
                           lpcrLocalVars->lpszStart,
                           lpcrLocalVars->base);
        // Restore the zapped char
        lpcrLocalVars->lpszStart[intLen] = chZap;
    } // End IF/ELSE

    // If we zapped the rational sep, ...
    if (chZapRat NE AC_EOS)
        // Restore it
        lpcrLocalVars->lpszCur[0] = chZapRat;

    if (res EQ 0)
    {
        // Check for -0
        if (lpcrLocalVars->lpszStart[0] EQ OVERBAR1
         && IsMpq0 (lpcrLocalVars->mpqMul)
         && gAllowNeg0)
        {
            // Mark as exiting with result == -0
            lpcrLocalVars->iNewState = CRROW_EXIT;
            lpcrLocalVars->crRetCode = CR_RESULT_NEG0;

            return FALSE;
        } else
            return TRUE;
    } else
        return crError (lpcrLocalVars);
} // End crEndSeg


//***************************************************************************
//  $crEndExp
//
//  End an exponent
//***************************************************************************

UBOOL crEndExp
    (LPCRLOCALVARS lpcrLocalVars)       // Ptr to <mpq_set_str2> local vars

{
    int     res;                        // mpq_set_str result
    size_t  expLen;                     // # chars in exponent
    mpir_si expVal;                     // (Signed) value of the exponent
    char    chZap;                      // Zapped char

    Assert (lpcrLocalVars->lpszStart[-1] EQ 'e'
         || lpcrLocalVars->lpszStart[-1] EQ 'E');

    // Get the length of the exponent part
    expLen = strspn (lpcrLocalVars->lpszStart, OVERBAR1_STR "0123456789");

    // Save and zap the next char
    chZap = lpcrLocalVars->lpszStart[expLen];
            lpcrLocalVars->lpszStart[expLen] = AC_EOS;

    // Convert the exponent at lpcrLocalVars.lpszStart into a rational number
    res = mpq_set_str (lpcrLocalVars->mpqExp,
                       lpcrLocalVars->lpszStart,
                       lpcrLocalVars->base);
    // Restore the zapped char
    lpcrLocalVars->lpszStart[expLen] = chZap;

    if (res NE 0)
        goto ERROR_EXIT;

    // Get the value of the exponent part
    expVal = mpz_get_si (mpq_numref (lpcrLocalVars->mpqExp));

    // Calculate the exponent part's multiplier as base^abs (expVal)
    mpz_ui_pow_ui (mpq_numref (lpcrLocalVars->mpqExp),
                   lpcrLocalVars->base,
                   (mpir_ui) (expVal > 0 ? expVal : -expVal));
    // If the exponent is negative, ...
    if (expVal < 0)
        // Divide the mutiplier by the exponent
        mpq_div (lpcrLocalVars->mpqMul,
                 lpcrLocalVars->mpqMul,
                 lpcrLocalVars->mpqExp);
    else
        // Multiply the mutiplier by the exponent
        mpq_mul (lpcrLocalVars->mpqMul,
                 lpcrLocalVars->mpqMul,
                 lpcrLocalVars->mpqExp);
ERROR_EXIT:
    if (res EQ 0)
        return TRUE;

    return crError (lpcrLocalVars);
} // End crEndExp


//***************************************************************************
//  $mpq_init_set_str2
//
//  Convert a string to rational allowing
//    -12.345E-56/78.90E-123
//***************************************************************************

CR_RETCODES mpq_init_set_str2
    (mpq_t  mpqRes,                         // Ptr to result
     LPCHAR lpszLine,                       // Ptr to incoming line
     int    base)                           // Base of number system

{
    // Initialize to 0/1
    mpq_init (mpqRes);

    return mpq_set_str2 (mpqRes,            // Ptr to result
                         lpszLine,          // Ptr to incoming line
                         base);             // Base of number system
} // End mpq_init_set_str2


//***************************************************************************
//  $mpq_set_str2
//
//  Convert a string to rational allowing
//    -12.345E-56/78.90E-123
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- mpq_set_str2"
#else
#define APPEND_NAME
#endif

CR_RETCODES mpq_set_str2
    (mpq_t  mpqRes,                         // Ptr to result
     LPCHAR lpszLine,                       // Ptr to incoming line
     int    base)                           // Base of number system

{
    CRCOLINDICES colIndex;                  // The translated char for tokenization as a CRCOL_*** value
    CR_ACTION    crAction1_EM,              // Ptr to 1st action
                 crAction2_EM;              // ...    2nd ...
    CRLOCALVARS  crLocalVars = {0};         // Local vars
    int          iLen,                      // Length of incoming line
                 iCnt;                      // Loop counter
    UBOOL        bRet = FALSE;              // TRUE iff the result is valid

    // Calculate the line length of the incoming string
    iLen = lstrlen (lpszLine);

    // Initialize the LocalVars struc
    crLocalVars.iNewState  = CRROW_INIT;
    crLocalVars.lpszIni   =
    crLocalVars.lpszCur   =
    crLocalVars.lpszStart = lpszLine;
    mpq_init (crLocalVars.mpqRes);
    mpq_init (crLocalVars.mpqMul);
    mpq_init (crLocalVars.mpqExp);
////crLocalVars.bRatSep   = FALSE;          // Already zero from "= {0}"
    crLocalVars.base      = base;

    // Loop through the incoming line
    for (iCnt = 0; iCnt <= iLen; iCnt++)
    {
        // Use a FSA to parse the line

        // Calculate the class of the current char
        colIndex = CharTransCR (crLocalVars.lpszCur[0]);

        // Get primary action and new state
        crAction1_EM = fsaActTableCR[crLocalVars.iNewState][colIndex].crAction1;
        crAction2_EM = fsaActTableCR[crLocalVars.iNewState][colIndex].crAction2;
        crLocalVars.iNewState = fsaActTableCR[crLocalVars.iNewState][colIndex].iNewState;

        // Check for primary action
        if (crAction1_EM
         && !(*crAction1_EM) (&crLocalVars))
            goto ERROR_EXIT;

        // Check for secondary action
        if (crAction2_EM
         && !(*crAction2_EM) (&crLocalVars))
            goto ERROR_EXIT;

        // Split cases based upon the return code
        switch (crLocalVars.iNewState)
        {
            case CRROW_ERROR:
                goto ERROR_EXIT;

            case CRROW_EXIT:
                // Mark as successful
                crLocalVars.crRetCode = CR_SUCCESS;
                bRet = TRUE;

                goto NORMAL_EXIT;

            default:
                break;
        } // End SWITCH
    } // End FOR

    DbgStop ();     // We should never get here

ERROR_EXIT:
    // Set the caret position
    ErrorMessageSetCharIndex ((int) (crLocalVars.lpszCur - crLocalVars.lpszIni));

    goto NORMAL_EXIT;

NORMAL_EXIT:
    // Save the result
    mpq_set (mpqRes, crLocalVars.mpqRes);

    // We no longer need this storage
    mpq_clear (crLocalVars.mpqExp);
    mpq_clear (crLocalVars.mpqMul);
    mpq_clear (crLocalVars.mpqRes);
#ifdef DEBUG
    if (bRet)
        Assert (crLocalVars.crRetCode EQ CR_SUCCESS);
    else
        Assert (crLocalVars.crRetCode NE CR_SUCCESS);
#endif
    return crLocalVars.crRetCode;
} // End mpq_set_str2
#undef  APPEND_NAME


//***************************************************************************
//  $CharTransCR
//
//  Translate a character in preparation for conversion to rational
//***************************************************************************

CRCOLINDICES CharTransCR
    (WCHAR wcCur)                       //  Char to translate

{
    // Split cases based upon the incoming character
    switch (wcCur)
    {
        case OVERBAR1:
            return CRCOL_NEG;

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return CRCOL_DIGIT;

        case '.':
            return CRCOL_DEC;

        case 'e':
        case 'E':
            return CRCOL_EXP;

        case 'r':
        case '/':
            return CRCOL_RAT;

        case 'x':
            return CRCOL_EXT;

        case INFINITY1:
            return CRCOL_INF;

        default:
            return CRCOL_EON;
    } // End SWITCH
} // End CharTransCR


#ifdef DEBUG
//***************************************************************************
//  $InitFsaTabCR
//
//  Ensure the CR FSA table has been properly setup
//***************************************************************************

void InitFsaTabCR
    (void)

{
    // Ensure we calculated the lengths properly
    Assert (sizeof (fsaActTableCR) EQ (CRCOL_LENGTH * sizeof (CRACTSTR) * CRROW_LENGTH));
} // End InitFsaTabCR
#endif


//***************************************************************************
//  End of File: cr_proc.c
//***************************************************************************
