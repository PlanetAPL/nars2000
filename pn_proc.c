//***************************************************************************
//  NARS2000 -- Parser Grammar Functions XXX Point Notation
//***************************************************************************

//***************************************************************************
//  Parse a line of XXX Point Notation characters.
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
#define _USE_MATH_DEFINES
#include <math.h>
#include "headers.h"


//***************************************************************************
//  $PN_actIDENT
//***************************************************************************

void PN_actIDENT
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
} // End PN_actIDENT


//***************************************************************************
//  $PN_actUNK
//***************************************************************************

void PN_actUNK
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    DbgStop ();
} // End PN_actUNK


//***************************************************************************
//  $PN_actBOOL_FLT
//***************************************************************************

void PN_actBOOL_FLT
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from BOOL to FLT
    lpSrc->at.aplFloat = (APLFLOAT) lpSrc->at.aplInteger;

    lpSrc->chType = PN_NUMTYPE_FLT;
} // End PN_actBOOL_FLT


//***************************************************************************
//  $PN_actBOOL_RAT
//***************************************************************************

void PN_actBOOL_RAT
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from BOOL to RAT
    mpq_init_set_sx (&lpSrc->at.aplRat, lpSrc->at.aplInteger, 1);

    lpSrc->chType = PN_NUMTYPE_RAT;
} // End PN_actBOOL_RAT


//***************************************************************************
//  $PN_actBOOL_VFP
//***************************************************************************

void PN_actBOOL_VFP
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from BOOL to VFP
    mpfr_init_set_sx (&lpSrc->at.aplVfp, lpSrc->at.aplInteger, MPFR_RNDN);

    lpSrc->chType = PN_NUMTYPE_VFP;
} // End PN_actBOOL_VFP


//***************************************************************************
//  $PN_actINT_FLT
//***************************************************************************

void PN_actINT_FLT
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from INT to FLT
    lpSrc->at.aplFloat = (APLFLOAT) lpSrc->at.aplInteger;

    lpSrc->chType = PN_NUMTYPE_FLT;
} // End PN_actINT_FLT


//***************************************************************************
//  $PN_actINT_RAT
//***************************************************************************

void PN_actINT_RAT
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from INT to RAT
    mpq_init_set_sx (&lpSrc->at.aplRat, lpSrc->at.aplInteger, 1);

    lpSrc->chType = PN_NUMTYPE_RAT;
} // End PN_actINT_RAT


//***************************************************************************
//  $PN_actINT_VFP
//***************************************************************************

void PN_actINT_VFP
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from INT to VFP
    mpfr_init_set_sx (&lpSrc->at.aplVfp, lpSrc->at.aplInteger, MPFR_RNDN);

    lpSrc->chType = PN_NUMTYPE_VFP;
} // End PN_actINT_VFP


//***************************************************************************
//  $PN_actFLT_VFP
//***************************************************************************

void PN_actFLT_VFP
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Initialize to 0
    mpfr_init0 (&lpSrc->at.aplVfp);

    // In order to preserve the precision of the floating point number,
    //   we rescan it as a VFP number
    // Convert the string to a VFP number
    mpfr_strtofr (&lpSrc->at.aplVfp, &lppnLocalVars->lpszStart[lpSrc->uNumStart], NULL, 10, MPFR_RNDN);

    lpSrc->chType = PN_NUMTYPE_VFP;
} // End PN_actFLT_VFP


//***************************************************************************
//  $PN_actRAT_VFP
//***************************************************************************

void PN_actRAT_VFP
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLVFP aplVfp = {0};

    // Convert the value from RAT to VFP
    mpfr_init_set_q  (&aplVfp, &lpSrc->at.aplRat, MPFR_RNDN);

    Myq_clear (&lpSrc->at.aplRat);

    lpSrc->at.aplVfp = aplVfp;

    lpSrc->chType = PN_NUMTYPE_VFP;
} // End PN_actRAT_VFP


#define PN_MAT                                                                                                                           \
{/*     BOOL          INT          FLT          HC2          HC4          HC8          RAT          VFP         INIT     */              \
   {M(BOOL,BOOL),M(BOOL,INT ),M(BOOL,FLT ),M(BOOL,HC2 ),M(BOOL,HC4 ),M(BOOL,HC8 ),M(BOOL,RAT ),M(BOOL,VFP ),M(BOOL,BOOL)},   /* BOOL */  \
   {M(INT ,INT ),M(INT ,INT ),M(INT ,FLT ),M(INT ,HC2 ),M(INT ,HC4 ),M(INT ,HC8 ),M(INT ,RAT ),M(INT ,VFP ),M(INT ,INT )},   /* INT  */  \
   {M(FLT ,FLT ),M(FLT ,FLT ),M(FLT ,FLT ),M(FLT ,HC2 ),M(FLT ,HC4 ),M(FLT ,HC8 ),M(FLT ,VFP ),M(FLT ,VFP ),M(FLT ,FLT )},   /* FLT  */  \
   {M(HC2 ,HC2 ),M(HC2 ,HC2 ),M(HC2 ,HC2 ),M(HC2 ,HC2 ),M(HC2 ,HC4 ),M(HC2 ,HC8 ),M(HC2 ,HC2 ),M(HC2 ,HC2 ),M(HC2 ,HC2 )},   /* HC2  */  \
   {M(HC4 ,HC4 ),M(HC4 ,HC4 ),M(HC4 ,HC4 ),M(HC4 ,HC4 ),M(HC4 ,HC4 ),M(HC4 ,HC8 ),M(HC4 ,HC4 ),M(HC4 ,HC4 ),M(HC4 ,HC4 )},   /* HC4  */  \
   {M(HC8 ,HC8 ),M(HC8 ,HC8 ),M(HC8 ,HC8 ),M(HC8 ,HC8 ),M(HC8 ,HC8 ),M(HC8 ,HC8 ),M(HC8 ,HC8 ),M(HC8 ,HC8 ),M(HC8 ,HC8 )},   /* HC8  */  \
   {M(RAT ,RAT ),M(RAT ,RAT ),M(RAT ,VFP ),M(RAT ,HC2 ),M(RAT ,HC4 ),M(RAT ,HC8 ),M(RAT ,RAT ),M(RAT ,VFP ),M(RAT ,RAT )},   /* RAT  */  \
   {M(VFP ,VFP ),M(VFP ,VFP ),M(VFP ,VFP ),M(VFP ,HC2 ),M(VFP ,HC4 ),M(VFP ,HC8 ),M(VFP ,VFP ),M(VFP ,VFP ),M(VFP ,VFP )},   /* VFP  */  \
   {M(INIT,BOOL),M(INIT,INT ),M(INIT,FLT ),M(INIT,HC2 ),M(INIT,HC4 ),M(INIT,HC8 ),M(INIT,RAT ),M(INIT,VFP ),M(INIT,INIT)},   /* INIT */  \
};
// PN_NUMTYPE_xxx promotion result matrix
PNNUMTYPE aNumTypePromote[PN_NUMTYPE_LENGTH][PN_NUMTYPE_LENGTH] =
#define M(a,b)  PN_NUMTYPE_##b
PN_MAT;
#undef  M

#define PN_actBOOL_BOOL     PN_actIDENT
#define PN_actBOOL_INT      PN_actIDENT
////ine PN_actBOOL_FLT      NULL
#define PN_actBOOL_HC2      PN_actUNK
#define PN_actBOOL_HC4      PN_actUNK
#define PN_actBOOL_HC8      PN_actUNK

#define PN_actINT_INT       PN_actIDENT
////ine PN_actINT_FLT       NULL
#define PN_actINT_HC2       PN_actUNK
#define PN_actINT_HC4       PN_actUNK
#define PN_actINT_HC8       PN_actUNK

#define PN_actFLT_FLT       PN_actIDENT
#define PN_actFLT_HC2       PN_actUNK
#define PN_actFLT_HC4       PN_actUNK
#define PN_actFLT_HC8       PN_actUNK

#define PN_actHC2_HC2       PN_actIDENT
#define PN_actHC2_HC4       PN_actUNK
#define PN_actHC2_HC8       PN_actUNK

#define PN_actHC4_HC4       PN_actIDENT
#define PN_actHC4_HC8       PN_actUNK

#define PN_actHC8_HC8       PN_actIDENT

#define PN_actRAT_RAT       PN_actIDENT
#define PN_actRAT_HC2       PN_actUNK
#define PN_actRAT_HC4       PN_actUNK
#define PN_actRAT_HC8       PN_actUNK

#define PN_actVFP_VFP       PN_actIDENT
#define PN_actVFP_HC2       PN_actUNK
#define PN_actVFP_HC4       PN_actUNK
#define PN_actVFP_HC8       PN_actUNK

#define PN_actINIT_BOOL     PN_actUNK
#define PN_actINIT_INT      PN_actUNK
#define PN_actINIT_FLT      PN_actUNK
#define PN_actINIT_HC2      PN_actUNK
#define PN_actINIT_HC4      PN_actUNK
#define PN_actINIT_HC8      PN_actUNK
#define PN_actINIT_RAT      PN_actUNK
#define PN_actINIT_VFP      PN_actUNK
#define PN_actINIT_INIT     PN_actUNK

// PN_NUMTYPE_xxx promotion action matrix
PN_ACTION aNumTypeAction [PN_NUMTYPE_LENGTH][PN_NUMTYPE_LENGTH] =
#define M(a,b)  PN_act##a##_##b
PN_MAT;
#undef  M


//***************************************************************************
//  $PN_NumAcc
//
//  Accumulate another digit into the number
//***************************************************************************

void PN_NumAcc
    (LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     char          chCur)               // The next digit

{
    // Save the digit
    lppnLocalVars->lpszNumAccum[lppnLocalVars->uNumAcc++] = chCur;

    // Ensure properly terminated
    lppnLocalVars->lpszNumAccum[lppnLocalVars->uNumAcc  ] = '\0';
} // End PN_NumAcc


//***************************************************************************
//  $PN_NumCalc
//
//  Calculate the value of a number
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PN_NumCalc"
#else
#define APPEND_NAME
#endif

void PN_NumCalc
    (LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     LPPN_YYSTYPE  lpYYArg,             // Ptr to incoming YYSTYPE
     UBOOL         bUseRat)             // TRUE iff this routine may blowup to Rational

{
    UBOOL     bRet = TRUE,              // TRUE iff the result is valid
              bSigned;                  // TRUE iff the number is negative
    APLINT    aplInteger;               // Temporary integer
    UINT      uLen,                     // Loop length
              uNumAcc,                  // Starting offset
              uAcc;                     // Loop counter
    PNNUMTYPE chType;                   // The numeric type (see PNNUMTYPE)

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return;

    // Initialize the starting offset
    uNumAcc = lpYYArg->uNumAcc;

    // Get the numeric type
    chType = (lppnLocalVars->chComType EQ PN_NUMTYPE_INIT) ? lpYYArg->chType : lppnLocalVars->chComType;

    // Handle negative sign
    bSigned = (lppnLocalVars->lpszNumAccum[uNumAcc] EQ OVERBAR1);

    // Split cases based upon the current numeric type
    switch (chType)
    {
        case PN_NUMTYPE_BOOL:
        case PN_NUMTYPE_INT:
            // If we're forming the numerator of a Rational number, ...
            if (bUseRat)
            {
                // Convert the numerator to a GMP integer
                mpz_init_set_str (mpq_numref (&lpYYArg->at.aplRat), &lppnLocalVars->lpszNumAccum[uNumAcc], 10);

                break;
            } else
            {
                // Initialize the accumulator
                aplInteger = 0;

                // Initialize the loop length
                uLen = lstrlen (&lppnLocalVars->lpszNumAccum[uNumAcc]);

                // If the number is negative, ...
                if (bSigned)
                    // Loop through the digits
                    for (uAcc = 1; bRet && uAcc < uLen; uAcc++)
                    {
                        aplInteger = imul64 (aplInteger, 10, &bRet);
                        if (bRet)
                            aplInteger = isub64 (aplInteger, lppnLocalVars->lpszNumAccum[uNumAcc + uAcc] - '0', &bRet);
                    } // End FOR
                else
                    // Loop through the digits
                    for (uAcc = 0; bRet && uAcc < uLen; uAcc++)
                    {
                        aplInteger = imul64 (aplInteger, 10, &bRet);
                        if (bRet)
                            aplInteger = iadd64 (aplInteger, lppnLocalVars->lpszNumAccum[uNumAcc + uAcc] - '0', &bRet);
                    } // End FOR
                if (bRet)
                {
                    // If it's -0, ...
                    if (gAllowNeg0
                     && bSigned
                     && aplInteger EQ 0)
                    {
                        // Save -0 as a float
                        lpYYArg->at.aplFloat = -0.0;

                        // Change the type to float
                        lpYYArg->chType = PN_NUMTYPE_FLT;

                        break;
                    } // End IF

                    // Save in the result
                    lpYYArg->at.aplInteger = aplInteger;

                    if (IsBooleanValue (aplInteger))
                        lpYYArg->chType = PN_NUMTYPE_BOOL;
                    break;
                } // End IF
            } // End IF

            // Fall through to the float case

        case PN_NUMTYPE_FLT:
            Assert (!bUseRat);

            // Use David Gay's routines
            lpYYArg->at.aplFloat = MyStrtod (&lppnLocalVars->lpszNumAccum[uNumAcc], NULL);

            // Change the type to float
            lpYYArg->chType = PN_NUMTYPE_FLT;

            break;

        case PN_NUMTYPE_RAT:
            // If we're in the first pass, ...
            if (lppnLocalVars->chComType EQ PN_NUMTYPE_INIT)
            {
                // This type is used after the numerator has been calculated
                //   and is in mpq_numref (&lpYYArg->at.aplRat).  The denominator is
                //   in lppnLocalVars->NumAccum[uNumAcc] and must be converted.

                // Convert the denominator to a GMP integer
                mpz_init_set_str (mpq_denref (&lpYYArg->at.aplRat), &lppnLocalVars->lpszNumAccum[uNumAcc], 10);

                // Save the sign of the result
                bSigned = (lpYYArg->bSigned NE bSigned);

                // If the denominator is zero, ...
                if (IsMpz0 (mpq_denref (&lpYYArg->at.aplRat)))
                {
                    // If the numerator is zero, ...
                    if (IsMpz0 (mpq_numref (&lpYYArg->at.aplRat)))
                    {
                        APLRAT mpqRes = {0};

                        // See what the []IC oracle has to say
                        mpq_QuadICValue (&lpYYArg->at.aplRat,
                                          ICNDX_0DIV0,
                                         &lpYYArg->at.aplRat,
                                         &mpqRes,
                                          (mpz_sgn (mpq_numref (&lpYYArg->at.aplRat)) EQ -1) NE
                                          (mpz_sgn (mpq_denref (&lpYYArg->at.aplRat)) EQ -1));
                        // We no longer need this storage
                        Myq_clear (&lpYYArg->at.aplRat);

                        // If the result is -0, ...
                        if (gAllowNeg0
                         && bSigned
                         && IsMpq0 (&mpqRes))
                        {
                            // Change the type to VFP
                            lpYYArg->chType = PN_NUMTYPE_VFP;

                            // Initialize the result and set to -0
                            mpfr_init_set_str (&lpYYArg->at.aplVfp, "-0", 10, MPFR_RNDN);
                        } else
                        {
                            // If the result is negative, ...
                            if (bSigned)
                                // Negate it
                                mpq_neg (&mpqRes, &mpqRes);

                            // Copy result to ALLTYPES
                            lpYYArg->at.aplRat = mpqRes;
                        } // End IF/ELSE
                    } else
                    {
                        // We no longer need this storage
                        Myq_clear (&lpYYArg->at.aplRat);

                        // Set to the appropriate-signed infinity
                        mpq_set_infsub (&lpYYArg->at.aplRat, bSigned ? -1 : 1);
                    } // End IF/ELSE
                } else
                // If the result is -0, ...
                if (gAllowNeg0
                 && bSigned
                 && IsMpz0 (mpq_numref (&lpYYArg->at.aplRat)))
                {
                    // We no longer need this storage
                    Myq_clear (&lpYYArg->at.aplRat);

                    // Change the type to VFP
                    lpYYArg->chType = PN_NUMTYPE_VFP;

                    // Initialize the result and set to -0
                    mpfr_init_set_str (&lpYYArg->at.aplVfp, "-0", 10, MPFR_RNDN);
                } else
                    // Canonicalize the arg
                    mpq_canonicalize (&lpYYArg->at.aplRat);
            } else
            {

                // Not formatting from a Rational number
                // Must be one that is expressible as an integer
                Assert (!bUseRat);

                // If it's a FLT, ...
                if (IsPnNumTypeFlt (lpYYArg->chType))
                {
                    CR_RETCODES crRetCode;              // Return code

                    // Convert to a RAT
                    crRetCode =
                      mpq_init_set_str2 (&lpYYArg->at.aplRat,                   // Ptr to result
                                         &lppnLocalVars->lpszNumAccum[uNumAcc], // Ptr to incoming line
                                          10);                                  // Base of number system
                    // Split cases based upon the return code
                    switch (crRetCode)
                    {
                        case CR_SUCCESS:
                            // Change the type to Rational
                            lpYYArg->chType = PN_NUMTYPE_RAT;

                            break;

                        case CR_SYNTAX_ERROR:
                            // Save the error message
                            ErrorMessageIndirect (ERRMSG_SYNTAX_ERROR APPEND_NAME);

                            // Mark as in error
                            lpYYArg = NULL;

                            break;

                        case CR_DOMAIN_ERROR:
                            // Save the error message
                            ErrorMessageIndirect (ERRMSG_DOMAIN_ERROR APPEND_NAME);

                            // Mark as in error
                            lpYYArg = NULL;

                            break;

                        case CR_RESULT_NEG0:
                            // Clear the previous memory
                            Myq_clear (&lpYYArg->at.aplRat);

                            // Zap it
                            ZeroMemory (&lpYYArg->at.aplRat, sizeof (lpYYArg->at.aplRat));

                            // Set the result to -0
                            mpfr_init_set_str (&lpYYArg->at.aplVfp, "-0", 10, MPFR_RNDN);

                            // Set the result type
                            lpYYArg->chType = PN_NUMTYPE_VFP;

                            break;

                        defstop
                            break;
                    } // End SWITCH

                } else
                {
                    // Convert the string to a rational number
                    mpz_init_set_str (mpq_numref (&lpYYArg->at.aplRat), &lppnLocalVars->lpszNumAccum[uNumAcc], 10);
                    mpz_init_set_str (mpq_denref (&lpYYArg->at.aplRat), "1"                                  , 10);

                    // Change the type to Rational
                    lpYYArg->chType = PN_NUMTYPE_RAT;
                } // End IF/ELSE
            } // End IF/ELSE

            break;

        case PN_NUMTYPE_VFP:
            // Not formatting from a Rational number
            Assert (!bUseRat);

            // Convert the string to a VFP number
            mpfr_init_set_str (&lpYYArg->at.aplVfp, &lppnLocalVars->lpszNumAccum[uNumAcc], 10, MPFR_RNDN);

            // Change the type to VFP
            lpYYArg->chType = PN_NUMTYPE_VFP;

            break;

        case PN_NUMTYPE_HC2:
        case PN_NUMTYPE_HC4:
        case PN_NUMTYPE_HC8:
            break;

        defstop
            break;
    } // End SWITCH

    // If it's valid, ...
    if (lpYYArg NE NULL)
        // Save the sign
        lpYYArg->bSigned = bSigned;
} // End PN_NumCalc
#undef  APPEND_NAME


//***************************************************************************
//  $PN_ChrAcc
//
//  Append a new char to the internal buffer
//***************************************************************************

void PN_ChrAcc
    (LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     char          chCur)               // The next char

{
    // Accumulate this char
    lppnLocalVars->lpszAlphaInt[lppnLocalVars->uAlpAcc++] = chCur;
} // End PN_ChrAcc


//***************************************************************************
//  $PN_MakeRatPoint
//
//  Merge the numerator and denominator to form a number
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PN_MakeRatPoint"
#else
#define APPEND_NAME
#endif

LPPN_YYSTYPE PN_MakeRatPoint
    (LPPN_YYSTYPE  lpYYNum,             // Ptr to The numerator part
     LPPN_YYSTYPE  lpYYDen,             // Ptr to The denominator part (may be NULL)
     LPPNLOCALVARS lppnLocalVars)       // Ptr to local pnLocalVars

{
    size_t      numLen;                 // # chars in number
    PNNUMTYPE   chType;                 // The numeric type (see PNNUMTYPE)
    char        chZap;                  // Zapped char
    CR_RETCODES crRetCode;              // Return code

    // Get the numeric type
    chType = (lppnLocalVars->chComType EQ PN_NUMTYPE_INIT) ? PN_NUMTYPE_RAT : lppnLocalVars->chComType;

    // Get the length of the number
    numLen = strspn (&lppnLocalVars->lpszStart[lpYYNum->uNumStart], OVERBAR1_STR INFINITY1_STR ".0123456789eEr/");

    // Initialize to 0/1
    mpq_init (&lpYYNum->at.aplRat);

    // Save and zap the next char
    chZap = lppnLocalVars->lpszStart[numLen + lpYYNum->uNumStart];
            lppnLocalVars->lpszStart[numLen + lpYYNum->uNumStart] = AC_EOS;

    crRetCode =
      mpq_set_str2 (&lpYYNum->at.aplRat,
                    &lppnLocalVars->lpszStart[lpYYNum->uNumStart],
                     10);
    // Restore the zapped char
    lppnLocalVars->lpszStart[numLen + lpYYNum->uNumStart] = chZap;

    // Change the type to RAT
    lpYYNum->chType = PN_NUMTYPE_RAT;

    // Split cases based upon the return code
    switch (crRetCode)
    {
        case CR_SUCCESS:
            break;

        case CR_SYNTAX_ERROR:
            // Save the error message
            ErrorMessageIndirect (ERRMSG_SYNTAX_ERROR APPEND_NAME);

            // Mark as in error
            lpYYNum = NULL;

            break;

        case CR_DOMAIN_ERROR:
            // Save the error message
            ErrorMessageIndirect (ERRMSG_DOMAIN_ERROR APPEND_NAME);

            // Mark as in error
            lpYYNum = NULL;

            break;

        case CR_RESULT_NEG0:
            // Clear the previous memory
            Myq_clear (&lpYYNum->at.aplRat);

            // Zap it
            ZeroMemory (&lpYYNum->at.aplRat, sizeof (lpYYNum->at.aplRat));

            // Set the result to -0
            mpfr_init_set_str (&lpYYNum->at.aplVfp, "-0", 10, MPFR_RNDN);

            // Set the result type
            lpYYNum->chType = PN_NUMTYPE_VFP;

            break;

        defstop
            break;
    } // End SWITCH

    return lpYYNum;
} // End PN_MakeRatPoint
#undef  APPEND_NAME


//***************************************************************************
//  $PN_MakeBasePoint
//
//  Merge the base and alphaint part to form a number
//***************************************************************************

LPPN_YYSTYPE PN_MakeBasePoint
    (LPPN_YYSTYPE  lpYYBase,            // The base part
     LPPN_YYSTYPE  lpYYAlphaInt,        // The AlphaInt part
     LPPNLOCALVARS lppnLocalVars)       // Ptr to local pnLocalVars

{
    UINT      uLen,                     // Length of lpszAlphaInt
              uAcc;                     // Loop counter
    LPCHAR    lpszAlphaInt;             // Ptr to next char in lpszAlphaInt
    APLINT    aplIntBase,               // Base as an integer
              aplIntPowBase,            // Base as successive powers
              aplIntTmp,                // Temporary integer
              aplIntAcc;                // Temporary accumulator
    APLFLOAT  aplFltBase,               // Base as a  float
              aplFltPowBase,            // Base as successive powers
              aplFltAcc,                // Temporary accumulator
              aplAccHC2Real,            // Accumulator real part
              aplAccHC2Imag,            // ...         imag ...
              aplBaseHC2Real,           // Base real part
              aplBaseHC2Imag,           // ...  imag ...
              aplPowBaseHC2Real,        // Base as successive powers real part
              aplPowBaseHC2Imag;        // ...                       imag ...
    UBOOL     bRet = TRUE;              // TRUE iff the result is valid
    UCHAR     chCur;                    // Temporary character
    PNNUMTYPE chType;                   // The numeric type (see PNNUMTYPE)

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return NULL;

    // Get the string length
    uLen = lppnLocalVars->uAlpAcc;

    // Get the ptr to the end of the AlphaInt string
    lpszAlphaInt = &lppnLocalVars->lpszAlphaInt[uLen - 1];

    // Get the numeric type
    chType = (lppnLocalVars->chComType EQ PN_NUMTYPE_INIT) ? lpYYBase->chType : lppnLocalVars->chComType;

    // Split cases based upon the numeric type of the base
    switch (chType)
    {
        case PN_NUMTYPE_BOOL:
        case PN_NUMTYPE_INT:
            // Get the base value
            aplIntBase = lpYYBase->at.aplInteger;

            // Initialize the accumulator and base power
            aplIntAcc = 0;
            aplIntPowBase = 1;

            // Loop through the AlphaInt arg
            for (uAcc = 0; bRet && uAcc < uLen; uAcc++)
            {
                // Get the next char as lowercase
                chCur = (UCHAR) CharLower ((LPCHAR) (*lpszAlphaInt--));

                // Convert to a number
                if ('0' <= chCur
                 &&        chCur <= '9')
                    chCur -= '0';
                else
                if ('a' <= chCur
                 &&        chCur <= 'z')
                    chCur -= 'a' - 10;

                // Times the power base
                aplIntTmp = imul64 (chCur, aplIntPowBase, &bRet);
                if (!bRet)
                    break;

                // Accumulate
                aplIntAcc = iadd64 (aplIntAcc, aplIntTmp, &bRet);
                if (!bRet)
                    break;

                // Shift over the power base
                aplIntPowBase = imul64 (aplIntPowBase, aplIntBase, &bRet);

                // Ignore spurious overflow on last digit
                bRet |= (uAcc EQ (uLen - 1));

////////////////if (!bRet)          // Not needed as we're at the end of the FOR loop
////////////////    break;
            } // End FOR

            if (bRet)
            {
                // Save the result
                lpYYBase->at.aplInteger = aplIntAcc;

                break;
            } // End IF

            // Convert the base value to Rational
            mpq_init_set_sx (&lpYYBase->at.aplRat, lpYYBase->at.aplInteger, 1);

            // Get the ptr to the end of the AlphaInt string
            lpszAlphaInt = &lppnLocalVars->lpszAlphaInt[uLen - 1];

            // Change the type to RAT
            lpYYBase->chType = PN_NUMTYPE_RAT;

            // Fall through to the rational case

        case PN_NUMTYPE_RAT:
        {
            APLRAT aplRatBase    = {0},
                   aplRatPowBase = {0},
                   aplRatTmp     = {0};

            // Initialize and set the base value
            mpq_init_set (&aplRatBase, &lpYYBase->at.aplRat);

            // Initialize the accumulator and base power
            mpq_set_ui (&lpYYBase->at.aplRat, 0, 1);
            mpq_init_set_ui (&aplRatPowBase, 1, 1);
            mpq_init (&aplRatTmp);

            // Loop through the AlphaInt arg
            for (uAcc = 0; uAcc < uLen; uAcc++)
            {
                // Get the next char as lowercase
                chCur = (UCHAR) CharLower ((LPCHAR) (*lpszAlphaInt--));

                // Convert to a number
                if ('0' <= chCur
                 &&        chCur <= '9')
                    chCur -= '0';
                else
                if ('a' <= chCur
                 &&        chCur <= 'z')
                    chCur -= 'a' - 10;

                // Times the power base and accumulate
                mpq_set_ui (&aplRatTmp, chCur, 1);
                mpq_mul (&aplRatTmp, &aplRatPowBase, &aplRatTmp);
                mpq_add (&lpYYBase->at.aplRat, &lpYYBase->at.aplRat, &aplRatTmp);
                mpq_canonicalize (&lpYYBase->at.aplRat);

                // Shift over the power base
                mpq_mul (&aplRatPowBase, &aplRatPowBase, &aplRatBase);
            } // End FOR

            // We no longer need this storage
            Myq_clear (&aplRatTmp);
            Myq_clear (&aplRatPowBase);
            Myq_clear (&aplRatBase);

            break;
        } // End PN_NUMTYPE_RAT

        case PN_NUMTYPE_VFP:
        {
            APLVFP aplVfpBase    = {0},
                   aplVfpPowBase = {0},
                   aplVfpTmp     = {0};

            // Initialize and set the base value
            mpfr_init_copy (&aplVfpBase, &lpYYBase->at.aplVfp);

            // Initialize the accumulator and base power
            mpfr_set_ui (&lpYYBase->at.aplVfp, 0, MPFR_RNDN);
            mpfr_init_set_ui (&aplVfpPowBase, 1, MPFR_RNDN);
            mpfr_init0 (&aplVfpTmp);

            // Loop through the AlphaInt arg
            for (uAcc = 0; uAcc < uLen; uAcc++)
            {
                // Get the next char as lowercase
                chCur = (UCHAR) CharLower ((LPCHAR) (*lpszAlphaInt--));

                // Convert to a number
                if ('0' <= chCur
                 &&        chCur <= '9')
                    chCur -= '0';
                else
                if ('a' <= chCur
                 &&        chCur <= 'z')
                    chCur -= 'a' - 10;

                // Times the power base and accumulate
                mpfr_set_ui (&aplVfpTmp, chCur, MPFR_RNDN);
                mpfr_mul (&aplVfpTmp, &aplVfpPowBase, &aplVfpTmp, MPFR_RNDN);
                mpfr_add (&lpYYBase->at.aplVfp, &lpYYBase->at.aplVfp, &aplVfpTmp, MPFR_RNDN);

                // Shift over the power base
                mpfr_mul (&aplVfpPowBase, &aplVfpPowBase, &aplVfpBase, MPFR_RNDN);
            } // End FOR

            // We no longer need this storage
            Myf_clear (&aplVfpTmp);
            Myf_clear (&aplVfpPowBase);
            Myf_clear (&aplVfpBase);

            break;
        } // End PN_NUMTYPE_VFP

        case PN_NUMTYPE_FLT:
            // Get the base value
            aplFltBase = lpYYBase->at.aplFloat;

            // Initialize the accumulator and base power
            aplFltAcc = 0;
            aplFltPowBase = 1;

            // Loop through the AlphaInt arg
            for (uAcc = 0; uAcc < uLen; uAcc++)
            {
                // Get the next char as lowercase
                chCur = (UCHAR) CharLower ((LPCHAR) (*lpszAlphaInt--));

                // Convert to a number
                if ('0' <= chCur
                 &&        chCur <= '9')
                    chCur -= '0';
                else
                if ('a' <= chCur
                 &&        chCur <= 'z')
                    chCur -= 'a' - 10;

                // Times the power base and accumulate
                aplFltAcc += chCur * aplFltPowBase;

                // Shift over the power base
                aplFltPowBase *= aplFltBase;
            } // End FOR

            // Save the result
            lpYYBase->at.aplFloat = aplFltAcc;

            break;

        case PN_NUMTYPE_HC2:
            // Get the base value
            aplBaseHC2Real = lpYYBase->at.aplHC2.Real;
            aplBaseHC2Imag = lpYYBase->at.aplHC2.Imag;

            // Initialize the accumulator and base power
            aplAccHC2Real =
            aplAccHC2Imag = 0;
            aplPowBaseHC2Real = 1;
            aplPowBaseHC2Imag = 0;

            // Loop through the AlphaInt arg
            for (uAcc = 0; uAcc < uLen; uAcc++)
            {
                // Get the next char as lowercase
                chCur = (UCHAR) CharLower ((LPCHAR) (*lpszAlphaInt--));

                // Convert to a number
                if ('0' <= chCur
                 &&        chCur <= '9')
                    chCur -= '0';
                else
                if ('a' <= chCur
                 &&        chCur <= 'z')
                    chCur -= 'a' - 10;

                // Times the power base and accumulate
                aplAccHC2Real += chCur * aplPowBaseHC2Real;
                aplAccHC2Imag += chCur * aplPowBaseHC2Imag;

                // Shift over the power base
                aplPowBaseHC2Real = aplBaseHC2Real * aplPowBaseHC2Real - aplBaseHC2Imag * aplPowBaseHC2Imag;
                aplPowBaseHC2Imag = aplBaseHC2Imag * aplPowBaseHC2Real + aplBaseHC2Real * aplPowBaseHC2Imag;
            } // End FOR

            // Save the result
            lpYYBase->at.aplHC2.Real = aplAccHC2Real;
            lpYYBase->at.aplHC2.Imag = aplAccHC2Imag;

            break;

        case PN_NUMTYPE_HC4:
        case PN_NUMTYPE_HC8:
            // ***FINISHME***

            break;

        defstop
            break;
    } // End SWITCH

    return lpYYBase;
} // End PN_MakeBasePoint


//***************************************************************************
//  $PN_MakeEulerPoint
//
//  Merge the multiplier and exponential part to form a number
//***************************************************************************

LPPN_YYSTYPE PN_MakeEulerPoint
    (LPPN_YYSTYPE  lpYYMultiplier,      // The multiplier part
     LPPN_YYSTYPE  lpYYExponent,        // The exponent part
     LPPNLOCALVARS lppnLocalVars)       // Ptr to local pnLocalVars

{
    PNNUMTYPE pnTypeRes;

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return NULL;

    // If this is the first time through, ...
    if (lppnLocalVars->chComType EQ PN_NUMTYPE_INIT)
    {
        // Promote the multiplier and exponent to a common format
        pnTypeRes = aNumTypePromote[lpYYMultiplier->chType][lpYYExponent->chType];

        // If the result is BOOL/INT, ...
        if (pnTypeRes EQ PN_NUMTYPE_BOOL
         || pnTypeRes EQ PN_NUMTYPE_INT)
            // Make it FLT
            pnTypeRes = PN_NUMTYPE_FLT;

        // If the result is RAT, ...
        if (pnTypeRes EQ PN_NUMTYPE_RAT)
            // Make it VFP
            pnTypeRes = PN_NUMTYPE_VFP;
    } else
        // Use common type
        pnTypeRes = lppnLocalVars->chComType;

    // If the multiplier must be promoted, ...
    if (pnTypeRes NE lpYYMultiplier->chType
     && aNumTypeAction[lpYYMultiplier->chType][pnTypeRes] NE NULL)
        (*aNumTypeAction[lpYYMultiplier->chType][pnTypeRes]) (lpYYMultiplier, lppnLocalVars);

    // If the exponent must be promoted, ...
    if (pnTypeRes NE lpYYExponent->chType
     && aNumTypeAction[lpYYExponent->chType  ][pnTypeRes] NE NULL)
        (*aNumTypeAction[lpYYExponent->chType  ][pnTypeRes]) (lpYYExponent, lppnLocalVars);

    // Set the result type
    lpYYMultiplier->chType =
    lpYYExponent->chType   = pnTypeRes;

    // Split cases based upon the numeric type of the result
    switch (pnTypeRes)
    {
        case PN_NUMTYPE_BOOL:
        case PN_NUMTYPE_INT:
            // The result is Multiplier x (*1) * Exponent
            lpYYMultiplier->at.aplFloat *= pow (FloatE, (APLFLOAT) lpYYExponent->at.aplInteger);

            break;

        case PN_NUMTYPE_FLT:
            // The result is Multiplier x (*1) * Exponent
            lpYYMultiplier->at.aplFloat *= pow (FloatE, lpYYExponent->at.aplFloat);

            break;

        case PN_NUMTYPE_VFP:
        {
            APLVFP aplVfpTmp = {0};
#ifdef DEBUG_FMT
            WCHAR wszTemp[512];
#endif
            // Initialize the temp array
            mpfr_init0 (&aplVfpTmp);
#ifdef DEBUG_FMT
            strcpyW (wszTemp, L"Mul:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], lpYYMultiplier->at.aplVfp, 0) = WC_EOS; DbgMsgW (wszTemp);
            strcpyW (wszTemp, L"Exp:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], lpYYExponent->at.aplVfp, 0) = WC_EOS; DbgMsgW (wszTemp);
#endif
            // The result is Multiplier x (*1) * Exponent
            mpfr_pow (&aplVfpTmp, &GetMemPTD ()->mpfrE, &lpYYExponent->at.aplVfp, MPFR_RNDN);
#ifdef DEBUG_FMT
            strcpyW (wszTemp, L"e *:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], aplVfpTmp, 0) = WC_EOS; DbgMsgW (wszTemp);
#endif
            // Accumulate in the multiplier
            mpfr_mul (&lpYYMultiplier->at.aplVfp, &lpYYMultiplier->at.aplVfp, &aplVfpTmp, MPFR_RNDN);
#ifdef DEBUG_FMT
            strcpyW (wszTemp, L"Res:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], lpYYMultiplier->at.aplVfp, 0) = WC_EOS; DbgMsgW (wszTemp);
#endif
            // We no longer need this storage
            Myf_clear (&aplVfpTmp);
            Myf_clear (&lpYYExponent->at.aplVfp);

            break;
        } // End PN_NUMTYPE_VFP

        defstop
            break;
    } // End SWITCH

    return lpYYMultiplier;
} // End PN_MakeEulerPoint


//***************************************************************************
//  $PN_MakeExpPoint
//
//  Merge the multiplier and exponential part to form a number
//***************************************************************************

LPPN_YYSTYPE PN_MakeExpPoint
    (LPPN_YYSTYPE  lpYYArg,             // The mantissa part
     LPPN_YYSTYPE  lpYYExponent,        // The exponent part
     LPPNLOCALVARS lppnLocalVars)       // Ptr to local pnLocalVars

{
    UINT    uNumAcc;                    // Starting offset
    APLMPI  aplMpiMult = {0},
            aplMpiExp  = {0};
#ifdef DEBUG_FMT
    WCHAR   wszTemp[512];
#endif
    LPCHAR  lpszNumAccum;               // Ptr to next byte

    // Initialize the starting offset
    uNumAcc = lpYYArg->uNumAcc;
    lpszNumAccum = &lppnLocalVars->lpszNumAccum[0];

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return NULL;

    // Terminate the (Exponent) argument
    PN_NumAcc (lppnLocalVars, '\0');

    // If this is not a RAT, ...
    if (lppnLocalVars->chComType NE PN_NUMTYPE_RAT)
        // Insert the exponent separator
        lppnLocalVars->lpszNumAccum[lpYYExponent->uNumAcc - 1] = 'e';

    // Split cases based upon the common type
    switch (lppnLocalVars->chComType)
    {
        int iExpSgn;                    // The signum of the exponent

        case PN_NUMTYPE_RAT:
            // Convert the exponent to a rational number
            mpz_init_set_str (&aplMpiExp , &lpszNumAccum[lpYYExponent->uNumAcc], 10);

            // Check the exponent for sign
            iExpSgn = mpz_sgn (&aplMpiExp);

            // Set exponent to absolute value
            mpz_abs (&aplMpiExp, &aplMpiExp);

            // If it doesn't fit in a UINT, ...
            if (mpz_fits_uint_p (&aplMpiExp) EQ 0)
            {
                // We no longer need this storage
                Myz_clear (&aplMpiExp);

                // Set to +/- infinity
                mpq_set_inf (&lpYYArg->at.aplRat, (lpszNumAccum[uNumAcc] EQ '-') ? -1 : 1);
            } else
            {
                // Create 10 * exp
                mpz_ui_pow_ui (&aplMpiExp, 10, mpz_get_ui (&aplMpiExp));

                // Convert the multiplier to a rational number
                mpz_init_set_str (&aplMpiMult, &lpszNumAccum[uNumAcc], 10);

                // If the exponent sign is negative, ...
                if (iExpSgn < 0)
                {
                    mpz_set_ui (mpq_numref (&lpYYArg->at.aplRat), 1);
                               *mpq_denref (&lpYYArg->at.aplRat) = aplMpiExp;
                } else
                {
                    mpz_set_ui (mpq_denref (&lpYYArg->at.aplRat), 1);
                               *mpq_numref (&lpYYArg->at.aplRat) = aplMpiExp;
                } // End IF/ELSE

#ifdef DEBUG_FMT
                strcpyW (wszTemp, L"Res1: "); *FormatAplRat (&wszTemp[lstrlenW (wszTemp)], lpYYArg->at.aplRat) = WC_EOS; DbgMsgW (wszTemp);
#endif
                // Multiply the multiplier by the 10 * exp
                mpz_mul (mpq_numref (&lpYYArg->at.aplRat),
                         mpq_numref (&lpYYArg->at.aplRat),
                        &aplMpiMult);
#ifdef DEBUG_FMT
                strcpyW (wszTemp, L"Res2: "); *FormatAplRat (&wszTemp[lstrlenW (wszTemp)], lpYYArg->at.aplRat) = WC_EOS; DbgMsgW (wszTemp);
#endif
                mpq_canonicalize (&lpYYArg->at.aplRat);

                // We no longer need this storage
                Myz_clear (&aplMpiMult);

#ifdef DEBUG_FMT
                strcpyW (wszTemp, L"Res3: "); *FormatAplRat (&wszTemp[lstrlenW (wszTemp)], lpYYArg->at.aplRat) = WC_EOS; DbgMsgW (wszTemp);
#endif
            } // End IF/ELSE

            // Change the type to RAT
            lpYYArg->chType = PN_NUMTYPE_RAT;

            break;

        case PN_NUMTYPE_VFP:
            // Convert the string to a VFP number
            mpfr_init_set_str (&lpYYArg->at.aplVfp, &lppnLocalVars->lpszStart[lpYYArg->uNumAcc], 10, MPFR_RNDN);

            // Change the type to VFP
            lpYYArg->chType = PN_NUMTYPE_VFP;

            break;

        case PN_NUMTYPE_FLT:
        case PN_NUMTYPE_INIT:
            // Use David Gay's routines
            lpYYArg->at.aplFloat = MyStrtod (&lpszNumAccum[uNumAcc], NULL);

            // Change the type to float
            lpYYArg->chType = PN_NUMTYPE_FLT;

            break;

        case PN_NUMTYPE_BOOL:       // Can't happen
        case PN_NUMTYPE_INT:        // ...
        defstop
            break;
    } // End SWITCH

    return lpYYArg;
} // End PN_MakeExpPoint


//***************************************************************************
//  $PN_MakeGammaPoint
//
//  Merge the multiplier and exponential part to form a number
//***************************************************************************

LPPN_YYSTYPE PN_MakeGammaPoint
    (LPPN_YYSTYPE  lpYYMultiplier,      // The multiplier part
     LPPN_YYSTYPE  lpYYExponent,        // The exponent part
     LPPNLOCALVARS lppnLocalVars)       // Ptr to local pnLocalVars

{
    PNNUMTYPE pnTypeRes;

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return NULL;

    // If this is the first time through, ...
    if (lppnLocalVars->chComType EQ PN_NUMTYPE_INIT)
    {
        // Promote the multiplier and exponent to a common format
        pnTypeRes = aNumTypePromote[lpYYMultiplier->chType][lpYYExponent->chType];

        // If the result is BOOL, ...
        if (IsPnNumTypeBool (pnTypeRes))
            // Make it FLT
            pnTypeRes = PN_NUMTYPE_FLT;
        else
        // If the result is INT, ...
        if (IsPnNumTypeInt (pnTypeRes))
            // Make it FLT
            pnTypeRes++;
        else
        // If the result is RAT, ...
        if (IsPnNumTypeRat (pnTypeRes))
            // Make it VFP
            pnTypeRes++;
    } else
        // Use common type
        pnTypeRes = lppnLocalVars->chComType;

    // If the multiplier must be promoted, ...
    if (pnTypeRes NE lpYYMultiplier->chType
     && aNumTypeAction[lpYYMultiplier->chType][pnTypeRes] NE NULL)
        (*aNumTypeAction[lpYYMultiplier->chType][pnTypeRes]) (lpYYMultiplier, lppnLocalVars);

    // If the exponent must be promoted, ...
    if (pnTypeRes NE lpYYExponent->chType
     && aNumTypeAction[lpYYExponent->chType  ][pnTypeRes] NE NULL)
        (*aNumTypeAction[lpYYExponent->chType  ][pnTypeRes]) (lpYYExponent, lppnLocalVars);

    // Set the result type
    lpYYMultiplier->chType =
    lpYYExponent->chType   = pnTypeRes;

    // Split cases based upon the numeric type
    switch (pnTypeRes)
    {
////////case PN_NUMTYPE_BOOL:
////////case PN_NUMTYPE_INT:
////////    // The result is Multiplier x (o1) * Exponent
////////    lpYYMultiplier->at.aplFloat *= pow (FloatGamma, (APLFLOAT) lpYYExponent->at.aplInteger);
////////
////////    break;
////////
        case PN_NUMTYPE_FLT:
            // The result is Multiplier x (o1) * Exponent
            lpYYMultiplier->at.aplFloat *= pow (FloatGamma, lpYYExponent->at.aplFloat);

            break;

        case PN_NUMTYPE_VFP:
        {
            APLVFP aplVfpTmp = {0};

            // Initialize the temp array
            mpfr_init0 (&aplVfpTmp);

            // The result is Multiplier x (o1) * Exponent
            mpfr_pow (&aplVfpTmp, &GetMemPTD ()->mpfrGamma, &lpYYExponent->at.aplVfp, MPFR_RNDN);

            // Accumulate in the multiplier
            mpfr_mul (&lpYYMultiplier->at.aplVfp, &lpYYMultiplier->at.aplVfp, &aplVfpTmp, MPFR_RNDN);

            // We no longer need this storage
            Myf_clear (&aplVfpTmp);
            Myf_clear (&lpYYExponent->at.aplVfp);

            break;
        } // End PN_NUMTYPE_VFP

        defstop
            break;
    } // End SWITCH

    return lpYYMultiplier;
} // End PN_MakeGammaPoint


//***************************************************************************
//  $PN_MakePiPoint
//
//  Merge the multiplier and exponential part to form a number
//***************************************************************************

LPPN_YYSTYPE PN_MakePiPoint
    (LPPN_YYSTYPE  lpYYMultiplier,      // The multiplier part
     LPPN_YYSTYPE  lpYYExponent,        // The exponent part
     LPPNLOCALVARS lppnLocalVars)       // Ptr to local pnLocalVars

{
    PNNUMTYPE pnTypeRes;

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return NULL;

    // If this is the first time through, ...
    if (lppnLocalVars->chComType EQ PN_NUMTYPE_INIT)
    {
        // Promote the multiplier and exponent to a common format
        pnTypeRes = aNumTypePromote[lpYYMultiplier->chType][lpYYExponent->chType];

        // If the result is BOOL, ...
        if (IsPnNumTypeBool (pnTypeRes))
            // Make it FLT
            pnTypeRes = PN_NUMTYPE_FLT;
        else
        // If the result is INT, ...
        if (IsPnNumTypeInt (pnTypeRes))
            // Make it FLT
            pnTypeRes++;
        else
        // If the result is RAT, ...
        if (IsPnNumTypeRat (pnTypeRes))
            // Make it VFP
            pnTypeRes++;
    } else
        // Use common type
        pnTypeRes = lppnLocalVars->chComType;

    // If the multiplier must be promoted, ...
    if (pnTypeRes NE lpYYMultiplier->chType
     && aNumTypeAction[lpYYMultiplier->chType][pnTypeRes] NE NULL)
        (*aNumTypeAction[lpYYMultiplier->chType][pnTypeRes]) (lpYYMultiplier, lppnLocalVars);

    // If the exponent must be promoted, ...
    if (pnTypeRes NE lpYYExponent->chType
     && aNumTypeAction[lpYYExponent->chType  ][pnTypeRes] NE NULL)
        (*aNumTypeAction[lpYYExponent->chType  ][pnTypeRes]) (lpYYExponent, lppnLocalVars);

    // Set the result type
    lpYYMultiplier->chType =
    lpYYExponent->chType   = pnTypeRes;

    // Split cases based upon the numeric type
    switch (pnTypeRes)
    {
////////case PN_NUMTYPE_BOOL:
////////case PN_NUMTYPE_INT:
////////    // The result is Multiplier x (o1) * Exponent
////////    lpYYMultiplier->at.aplFloat *= pow (FloatPi, (APLFLOAT) lpYYExponent->at.aplInteger);
////////
////////    break;
////////
        case PN_NUMTYPE_FLT:
            // The result is Multiplier x (o1) * Exponent
            lpYYMultiplier->at.aplFloat *= pow (FloatPi, lpYYExponent->at.aplFloat);

            break;

        case PN_NUMTYPE_VFP:
        {
            APLVFP aplVfpTmp = {0};

            // Initialize the temp array
            mpfr_init0 (&aplVfpTmp);

            // The result is Multiplier x (o1) * Exponent
            mpfr_pow (&aplVfpTmp, &GetMemPTD ()->mpfrPi, &lpYYExponent->at.aplVfp, MPFR_RNDN);

            // Accumulate in the multiplier
            mpfr_mul (&lpYYMultiplier->at.aplVfp, &lpYYMultiplier->at.aplVfp, &aplVfpTmp, MPFR_RNDN);

            // We no longer need this storage
            Myf_clear (&aplVfpTmp);
            Myf_clear (&lpYYExponent->at.aplVfp);

            break;
        } // End PN_NUMTYPE_VFP

        defstop
            break;
    } // End SWITCH

    return lpYYMultiplier;
} // End PN_MakePiPoint


//***************************************************************************
//  $PN_MakeVfpPoint
//
//  Merge the multiplier and exponential part to form a number
//***************************************************************************

LPPN_YYSTYPE PN_MakeVfpPoint
    (LPPN_YYSTYPE  lpYYArg,             // The mantissa part
     LPPN_YYSTYPE  lpYYExponent,        // The exponent part (may be NULL)
     LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     LPAPLINT      lpiVfpPrec)          // Ptr to VFP precision (may be NULL)

{
    UINT      uNumAcc,                  // Starting offset
              uDig,                     // # significant digits
              uArg,                     // Loop counter
              uLen;                     // # accumulated chars
    mp_prec_t uOldPrec,                 // Old precision
              uNewPrec;                 // # significant bits
    LPCHAR    lpszNumAccum;             // Ptr to next byte

    // Initialize the starting offset
    uNumAcc = lpYYArg->uNumAcc;
    lpszNumAccum = &lppnLocalVars->lpszNumAccum[0];

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return NULL;

    Assert (lppnLocalVars->chComType EQ PN_NUMTYPE_INIT
         || lppnLocalVars->chComType EQ PN_NUMTYPE_VFP);

    // Get # accumulated chars
    uLen = lstrlen (&lpszNumAccum[uNumAcc]);

    // Calculate the # significant digits in the number
    for (uDig = uArg = 0; uArg < uLen; uArg++)
    if (isdigit (lpszNumAccum[uNumAcc + uArg]))
        uDig++;

    // Convert the # significant digits to # significant bits
    //   via the formula 1 + floor (log2 (10^N))
    //                 = 1 + floor (N x log2 (10))
    //   where log2 (10) = (ln (10)) / (ln (2))
    //                   = M_LN10 / M_LN2
    uNewPrec = 1 + (mp_prec_t) floor (uDig * M_LN10 / M_LN2);

    // If present, ...
    if (lpYYExponent)
        // Insert the exponent separator
        lpszNumAccum[lpYYExponent->uNumAcc - 1] = 'e';

    // Get and save the current precision
    uOldPrec = mpfr_get_default_prec ();

    if (lpiVfpPrec NE NULL && *lpiVfpPrec NE 0)
    {
        // Validate the desired precision
        if (!ValidateIntegerTest (lpiVfpPrec,           // Ptr to the integer to test
                                  DEF_MIN_QUADFPC,      // Low range value (inclusive)
                                  DEF_MAX_QUADFPC,      // High ...
                                  TRUE))                // TRUE iff we're range limiting
        {
            // Mark as invalid result
            lppnLocalVars->bYYERROR = TRUE;

            return NULL;
        } else
            uNewPrec = (UINT) *lpiVfpPrec;
    } else
        // Set the precision to the default
        uNewPrec = uOldPrec;

    // Set the default precision
    mpfr_set_default_prec (uNewPrec);

    // Use MPFR routine
    mpfr_init_set_str (&lpYYArg->at.aplVfp, &lpszNumAccum[uNumAcc], 10, MPFR_RNDN);

    // Restore the default precision
    mpfr_set_default_prec (uOldPrec);

    // Change the type to VFP
    lpYYArg->chType = PN_NUMTYPE_VFP;

    // Return as the result
    return lpYYArg;
} // End PN_MakeVfpPoint


//***************************************************************************
//  $PN_VectorAcc
//
//  Accumulate the current value into the vector
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PN_VectorAcc"
#else
#define APPEND_NAME
#endif

UBOOL PN_VectorAcc
    (LPPN_YYSTYPE  lpYYArg,             // Number to accumulate
     LPPNLOCALVARS lppnLocalVars)       // Ptr to local pnLocalVars

{
    LPPN_VECTOR lppnVector;
    LPCHAR      lpStart;
    UINT        uNumLen;
    UBOOL       bFltN0,                 // TRUE iff the arg is FLT and -0
                bVfpN0,                 // ...                 VFP ...
                bNeg0;                  // ...      bFltN0 || bVfpN0

    // Accumulate the number

    // If the global handle has not been allocated as yet, ...
    if (lppnLocalVars->hGlbVector EQ NULL)
    {
        lppnLocalVars->hGlbVector =
          DbgGlobalAlloc (GHND, PNVECTOR_INIT * sizeof (PN_VECTOR));
        if (lppnLocalVars->hGlbVector EQ NULL)
            goto WSFULL_EXIT;
        lppnLocalVars->uGlbVectorMaxLen = PNVECTOR_INIT;
        lppnLocalVars->uGlbVectorCurLen = 0;
    } // End IF

    // Check to see if we need to reallocate
    if (lppnLocalVars->uGlbVectorCurLen EQ lppnLocalVars->uGlbVectorMaxLen)
    {
        UINT uMaxLen;

        // Calculate the new maximum length
        uMaxLen = lppnLocalVars->uGlbVectorMaxLen + PNVECTOR_INCR;

        // Attempt to reallocate the storage
        //   moving the old data to the new location, and
        //   freeing the old global memory
        lppnLocalVars->hGlbVector =
          MyGlobalReAlloc (lppnLocalVars->hGlbVector,
                           uMaxLen * sizeof (PN_VECTOR),
                           GMEM_MOVEABLE);
        // If that failed, ...
        if (lppnLocalVars->hGlbVector EQ NULL)
        {
            HGLOBAL hGlbVector;
            LPVOID  lpMemVectorOld,
                    lpMemVectorNew;

            // Allocate a new global memory object
            hGlbVector =
               DbgGlobalAlloc (GHND,
                              uMaxLen * sizeof (PN_VECTOR));
            if (hGlbVector EQ NULL)    // ***FIXME*** do we need to free the hGlbVector storage???
                goto WSFULL_EXIT;
            // Lock the two global memory areas and copy old to new
            lpMemVectorOld = MyGlobalLock    (lppnLocalVars->hGlbVector);
            lpMemVectorNew = MyGlobalLock000 (               hGlbVector);
            CopyMemory (lpMemVectorNew,
                        lpMemVectorOld,
                        lppnLocalVars->uGlbVectorMaxLen * sizeof (PN_VECTOR));
            // We no longer need these ptrs
            MyGlobalUnlock (lppnLocalVars->hGlbVector); lpMemVectorNew = NULL;
            MyGlobalUnlock (               hGlbVector); lpMemVectorOld = NULL;

            // Free the old storage
            DbgGlobalFree (lppnLocalVars->hGlbVector); lppnLocalVars->hGlbVector = NULL;

            // Copy to save area
            lppnLocalVars->hGlbVector = hGlbVector; hGlbVector = NULL;
        } // End IF

        // Save as new maximum length
        lppnLocalVars->uGlbVectorMaxLen = uMaxLen;
    } // End IF

    // Lock the memory to get a ptr to it
    lppnVector = MyGlobalLock (lppnLocalVars->hGlbVector);

    lppnVector[lppnLocalVars->uGlbVectorCurLen].at       = lppnLocalVars->at;
    ZeroMemory (&lppnLocalVars->at, sizeof (lppnLocalVars->at));

    lppnVector[lppnLocalVars->uGlbVectorCurLen].chType   = lppnLocalVars->chType;
    lppnLocalVars->chType = PN_NUMTYPE_INIT;

    lpStart = &lppnLocalVars->lpszStart[lpYYArg->uNumStart];
    uNumLen =  lppnLocalVars->uNumCur - lpYYArg->uNumStart;     // ***CHECKME***

    // Delete trailing blanks
    // While the last char is white, ...
    while (uNumLen && IsWhite (lpStart[uNumLen - 1]))
        // Skip to the previous char
        uNumLen--;

    // Delete leading blanks
    // While the first char is white, ...
    while (uNumLen && IsWhite (lpStart[0]))
    {
        // Skip over it
        lpStart++; uNumLen--;
    } // End WHILE

    lppnVector[lppnLocalVars->uGlbVectorCurLen].lpStart = lpStart;
    lppnVector[lppnLocalVars->uGlbVectorCurLen].uNumLen = uNumLen;

    bFltN0 = (IsPnNumTypeFlt (lppnVector[lppnLocalVars->uGlbVectorCurLen].chType)           // Is -0.0
           && IsFltN0        (lppnVector[lppnLocalVars->uGlbVectorCurLen].at.aplFloat));    // ...
    bVfpN0 = (IsPnNumTypeVfp (lppnVector[lppnLocalVars->uGlbVectorCurLen].chType)           // Is -0.0v
           && IsVfpN0        (lppnVector[lppnLocalVars->uGlbVectorCurLen].at.aplVfp  ));    // ...
    bNeg0  = bFltN0 || bVfpN0;

    // Izit expressible as a rational?
    lppnVector[lppnLocalVars->uGlbVectorCurLen].bRatExp =
        lppnLocalVars->bRatSep
     && !(gAllowNeg0 ? bNeg0
                     : FALSE
         );
    // Set the new initial point
    lppnLocalVars->uNumIni += uNumLen;

    // Count in another item
    lppnLocalVars->uGlbVectorCurLen++;

    // We no longer need this resource
    MyGlobalUnlock (lppnLocalVars->hGlbVector); lppnVector = NULL;

    // Mark as successful
    return TRUE;

WSFULL_EXIT:
    // Save the error message
    ErrorMessageIndirect (ERRMSG_WS_FULL APPEND_NAME);

    return FALSE;
} // End PN_VectorAcc
#undef  APPEND_NAME


//***************************************************************************
//  $PN_VectorRes
//
//  Create the scalar or vector result
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PN_VectorRes"
#else
#define APPEND_NAME
#endif

UBOOL PN_VectorRes
    (LPPNLOCALVARS lppnLocalVars)           // Ptr to local pnLocalVars

{
    UINT              uCnt,                 // Loop counter
                      uRes;                 // ...
    UBOOL             bRet = FALSE;         // TRUE iff the result is valid
    LPPN_VECTOR       lppnVector;           // Ptr to accumulated vector
    APLSTYPE          aplTypeRes;           // Result storage type
    APLNELM           aplNELMRes;           // ...    NELM
    APLRANK           aplRankRes;           // ...    rank
    APLUINT           ByteRes;              // # bytes in the string vector
    LPVOID            lpMemRes;             // Ptr to result global memory
    PNLOCALVARS       pnLocalVars = {0};    // PN Local vars
    PNNUMTYPE         chComType;            // Common numeric type (see PNNUMTYPE)
    PN_YYSTYPE        pnType = {0};         // PN_YYSTYPE stack element
    UBOOL             bPass1;               // TRUE iff pass 1

    // Lock the memory to get a ptr to it
    lppnVector = MyGlobalLock (lppnLocalVars->hGlbVector);

    // Get the # items
    aplNELMRes = lppnLocalVars->uGlbVectorCurLen;

    Assert (aplNELMRes NE 0);

    // Mark as pass 1 or not
    bPass1 = (lppnLocalVars->chComType EQ PN_NUMTYPE_INIT);

    // If this is the first pass, ...
    if (bPass1)
    {
        // Initialize the starting datatype
        chComType                =
        lppnLocalVars->chComType = PN_NUMTYPE_INIT;

        // Scan all items to determine a common datatype
        for (uCnt = 0; uCnt < aplNELMRes; uCnt++)
        {
            UBOOL bFltN0 = (IsPnNumTypeFlt (lppnVector[uCnt].chType)        // Is -0.0
                         && IsFltN0        (lppnVector[uCnt].at.aplFloat)), // ...
                  bVfpN0 = (IsPnNumTypeVfp (lppnVector[uCnt].chType)        // Is -0.0v
                         && IsVfpN0        (lppnVector[uCnt].at.aplVfp  )), // ...
                  bNeg0  = bFltN0 || bVfpN0,
                  bFlt   =  IsPnNumTypeFlt (lppnVector[uCnt].chType)        // Is FLT
                         || IsPnNumTypeVfp (lppnVector[uCnt].chType);       // Is VFP

            // If there's a Rat but no Vfp separator, and
            //    the item is a form of FLT, and
            //    the item is not -0, ...
            if (lppnLocalVars->bRatSep
             && !(gAllowNeg0 ? bNeg0
                             : FALSE
                 )
                )
                // ***ASSUME***:  chtype - 1 is INT- or RAT-like
                lppnLocalVars->chComType = aNumTypePromote[lppnLocalVars->chComType][lppnVector[uCnt].chType - bFlt];
            else
                lppnLocalVars->chComType = aNumTypePromote[lppnLocalVars->chComType][lppnVector[uCnt].chType       ];
        } // End FOR

        // Translate the datatype to an array storage type
        aplTypeRes = TranslatePnTypeToArrayType (lppnLocalVars->chComType);

        // Calculate the result rank
        aplRankRes = aplNELMRes > 1;        // 0 if singleton, 1 if not

        // Calculate space needed for the numeric scalar/vector
        ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

        // Check for overflow
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;

        // Allocate global memory for the array
        lppnLocalVars->hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (lppnLocalVars->hGlbRes EQ NULL)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lppnLocalVars->lpMemHdrRes = MyGlobalLock000 (lppnLocalVars->hGlbRes);

#define lpHeader        lppnLocalVars->lpMemHdrRes
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = aplTypeRes;
////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
#ifdef DEBUG
        lpHeader->bMFOvar    = lppnLocalVars->lptkLocalVars->bMFO;
#endif
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = aplNELMRes;
        lpHeader->Rank       = aplRankRes;
#undef  lpHeader

        // If it's not a scalar, ...
        if (!IsScalar (aplRankRes))
            // Save the dimension
            *VarArrayBaseToDim (lppnLocalVars->lpMemHdrRes) = aplNELMRes;

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lppnLocalVars->lpMemHdrRes);

        // Save common values
        pnLocalVars.lpszAlphaInt  = lppnLocalVars->lpszAlphaInt;
        pnLocalVars.lpszNumAccum  = lppnLocalVars->lpszNumAccum;
        pnLocalVars.uCharIndex    = lppnLocalVars->uCharIndex;
        pnLocalVars.lptkLocalVars = lppnLocalVars->lptkLocalVars;
        pnLocalVars.bRatSep       = lppnLocalVars->bRatSep;

        // Convert all items to a common datatype (lppnLocalVars->chComType)
        for (uCnt = uRes = 0; uCnt < aplNELMRes; uCnt++)
        {
            // We're trying to avioid anomalies such as having (say)
            // 1:   1111111111111111111111111111111111111 1r2
            //   return a VFP because the string of 1s is converted to FLT
            //   and FLT RAT -> VFP where we would prefer RAT RAT -> RAT
            // 2:   1111111111111111111111111111111111111 0i1
            //   return a HC2F because the string of 1s is converted to FLT
            //   and FLT HC2I -> HC2F where we would prefer RAT HC2I -> HC2R.
            // 3:   0.1 5v
            //   convert 0.1 as FLT and then to VFP
            //   instead of convert 0.1 to VFP directly and not introduce spurious trailing digits.

            // If the item needs promoting, ...
            if (lppnLocalVars->chComType NE lppnVector[uCnt].chType)
            {
                UBOOL bRet;                 // TRUE iff result is valid
                char  cZap;                 // Temporary char

                // Save the starting point and length of the character stream
                pnLocalVars.lpszStart    = lppnVector[uCnt].lpStart;
                pnLocalVars.uNumLen      = lppnVector[uCnt].uNumLen;
                pnLocalVars.uNumIni      = 0;
                pnLocalVars.chComType    = lppnLocalVars->chComType;

                // Ensure properly terminated
                cZap = pnLocalVars.lpszStart[pnLocalVars.uNumLen];
                       pnLocalVars.lpszStart[pnLocalVars.uNumLen] = AC_EOS;

                // Call the parser to convert the PN to a number
                bRet = ParsePointNotation (&pnLocalVars);

                // Restore zapped char
                pnLocalVars.lpszStart[pnLocalVars.uNumLen] = cZap;

                if (bRet)
                {
                    LPPN_VECTOR lpMemVector;            // Ptr to hGlbVector global memory

                    // Lock the memory to get a ptr to it
                    lpMemVector = MyGlobalLock (pnLocalVars.hGlbVector);

                    // Copy the new value to temp storage
                    pnType.at = lpMemVector->at;

                    // Promote to the common type and free old storage
                    (*aNumTypeAction[lpMemVector->chType][lppnLocalVars->chComType]) (&pnType, lppnLocalVars);

                    // We no longer need this ptr
                    MyGlobalUnlock (pnLocalVars.hGlbVector); lpMemVector = NULL;

                    // We no longer need this storage
                    DbgGlobalFree (pnLocalVars.hGlbVector); pnLocalVars.hGlbVector = NULL;
                } else
                    goto ERROR_EXIT;
            } else
            {
                // Copy the new value to temp storage
                pnType.at = lppnVector[uCnt].at;

                // Promote to the common type and free old storage
                (*aNumTypeAction[lppnVector[uCnt].chType][lppnLocalVars->chComType]) (&pnType, lppnLocalVars);
            } // End IF/ELSE

            // Split cases based upon the common type
            switch (aplTypeRes)
            {
                case ARRAY_BOOL:
                    // Save the promoted type in the result
                    *((LPAPLBOOL ) lpMemRes)  |= (pnType.at.aplInteger) << uRes;

                    // Check for end-of-byte
                    if (++uRes EQ NBIB)
                    {
                        uRes = 0;                   // Start over
                        ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                    } // End IF

                    break;

                case ARRAY_INT:
                    // Save the promoted type in the result
                    *((LPAPLINT  ) lpMemRes)++ = pnType.at.aplInteger;

                    break;

                case ARRAY_FLOAT:
                    // Save the promoted type in the result
                    *((LPAPLFLOAT) lpMemRes)++ = pnType.at.aplFloat;

                    break;

                case ARRAY_RAT:
                    // Save the promoted type in the result
                    *((LPAPLRAT  ) lpMemRes)++ = pnType.at.aplRat;

                    break;

                case ARRAY_VFP:
                    // Save the promoted type in the result
                    *((LPAPLVFP  ) lpMemRes)++ = pnType.at.aplVfp;

                    break;

                case ARRAY_CHAR:            // Can't happen as this code is for numbers only
                case ARRAY_HETERO:          // ...
                case ARRAY_NESTED:          // ...
                defstop
                    break;
            } // End SWITCH
        } // End FOR
    } else
    {
        Assert (IsSingleton (aplNELMRes));

        // Save the global memory handle
        lppnLocalVars->hGlbRes = lppnLocalVars->hGlbVector;
    } // End IF/ELSE

    // Copy the last result back to <lppnLocalVars->at> in case it's a scalar
    //   and we need its value for :CONSTANT on )LOAD or )COPY
    lppnLocalVars->at = pnType.at;

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    // Save the error message
    ErrorMessageIndirect (ERRMSG_WS_FULL APPEND_NAME);

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (lppnLocalVars->hGlbRes NE NULL && lppnLocalVars->lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lppnLocalVars->hGlbRes); lppnLocalVars->lpMemHdrRes = NULL;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (lppnLocalVars->hGlbVector); lppnVector = NULL;

    if (bPass1)
    {
        // We no longer need this storage
        DbgGlobalFree (lppnLocalVars->hGlbVector); lppnLocalVars->hGlbVector = NULL;
    } // End IF

    return bRet;
} // End PN_VectorRes
#undef  APPEND_NAME


//***************************************************************************
//  $PN_SetInfinity
//
//  Set a constant infinity
//***************************************************************************

PN_YYSTYPE PN_SetInfinity
    (LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     PNNUMTYPE     pnNumType,           // The suggested PN_NUMTYPE_xx
     int           uNumStart,           // The starting offset in lpszStart
     int           iInfSgn,             // The sign of infinity (1 for positive, -1 for negative)
     LPAPLINT      lpiVfpPrec)          // Ptr to VFP Precision (NULL = none, ptr to 0 = default)

{
    PN_YYSTYPE  pnYYRes = {0};          // The result
    mpfr_prec_t uDefPrec;               // Default VFP precision

    Assert (iInfSgn EQ 1 || iInfSgn EQ -1);

    // Save the starting offset
    pnYYRes.uNumStart = uNumStart;

    // Get the numeric type
    pnYYRes.chType = (lppnLocalVars->chComType EQ PN_NUMTYPE_INIT) ? pnNumType : lppnLocalVars->chComType;

    // Split cases based upon the current numeric type
    switch (pnYYRes.chType)
    {
        case PN_NUMTYPE_FLT:
            // Mark as +/- infinity
            pnYYRes.at.aplFloat = (iInfSgn EQ 1) ? fltPosInfinity : fltNegInfinity;

            break;

        case PN_NUMTYPE_RAT:
            // Mark as _/- infinity
            mpq_set_infsub (&pnYYRes.at.aplRat, iInfSgn);

            break;

        case PN_NUMTYPE_VFP:
            if (lpiVfpPrec NE NULL && *lpiVfpPrec NE 0)
            {
                // Validate the desired precision
                if (!ValidateIntegerTest (lpiVfpPrec,           // Ptr to the integer to test
                                          DEF_MIN_QUADFPC,      // Low range value (inclusive)
                                          DEF_MIN_QUADFPC,      // High ...
                                          TRUE))                // TRUE iff we're range limiting
                {
                    // Mark as invalid result
                    lppnLocalVars->bYYERROR = TRUE;

                    break;
                } else
                    uDefPrec = (UINT) *lpiVfpPrec;
            } else
                // Use the default precision
                uDefPrec = mpfr_get_default_prec ();

            // Initialize the VFP
            mpfr_init2 (&pnYYRes.at.aplVfp, uDefPrec);

            // Mark as +/- infinity
            mpfr_set_inf (&pnYYRes.at.aplVfp, iInfSgn);

            break;

        case PN_NUMTYPE_HC2:            // Not implemented as yet
        case PN_NUMTYPE_HC4:            // ...
        case PN_NUMTYPE_HC8:            // ...
        case PN_NUMTYPE_BOOL:           // Can't happen -- no such type
        case PN_NUMTYPE_INT:            // ...
        defstop
            break;
    } // End SWITCH

    return pnYYRes;
} // End PN_SetInfinity


//***************************************************************************
//  End of File: pn_proc.c
//***************************************************************************
