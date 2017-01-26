//***************************************************************************
//  NARS2000 -- Multi Precision Functions
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
#include <math.h>               // For fabs
#include "headers.h"

#define int32   int


//***************************************************************************
//  $mp_alloc
//
//  Multi Precision Allocation function
//***************************************************************************

LPVOID mp_alloc
    (size_t size)           // Allocation size

{
    LPVOID lpMemRes;

    lpMemRes =
#ifdef DEBUG
      MyHeapAlloc (GetProcessHeap (),
                   0
                 | HEAP_ZERO_MEMORY
                 | HEAP_GENERATE_EXCEPTIONS
                   ,
                   size);
#else
      dlmalloc (size);
#endif
    if (lpMemRes EQ NULL)
    {
#ifdef DEBUG
        APLINT aplSize = size;

        dprintfWL0 (L"###Heap allocation failure:  size = %I64u", aplSize);
#endif
        longjmp (heapFull, 1);
    } else
    {
#ifdef DEBUG
        if (gDbgLvl >= gVfpLvl)
        {
            WCHAR wszTemp[256];

            MySprintfW (wszTemp,
                        sizeof (wszTemp),
                       L"MPFNS(Alloc):    %p (%u)",
                        lpMemRes,
                        size);
            DbgMsgW (wszTemp);
        } // End IF
#endif
    } // End IF/ELSE

    return lpMemRes;
} // End mp_alloc


//***************************************************************************
//  $mp_realloc
//
//  Multi Precision Reallocation function
//***************************************************************************

LPVOID mp_realloc
    (LPVOID lpMem,          // Ptr to start of memory block
     size_t old_size,       // Old size
     size_t new_size)       // New size

{
    LPVOID lpMemRes;

    lpMemRes =
#ifdef DEBUG
      MyHeapReAlloc (GetProcessHeap (),
                     0
                   | HEAP_ZERO_MEMORY
                   | HEAP_GENERATE_EXCEPTIONS
                     ,
                     lpMem,
                     new_size);
#else
      dlrealloc (lpMem, new_size);
#endif
    if (lpMemRes EQ NULL)
    {
#ifdef DEBUG
        APLINT aplOldSize = old_size,
               aplUseSize = dlmalloc_usable_size (lpMem),
               aplNewSize = new_size;

        dprintfWL0 (L"###Heap re-allocation failure:  OldSize = %I64u, UseSize = %I64u, NewSize = %I64u", aplOldSize, aplUseSize, aplNewSize);
#endif
        longjmp (heapFull, 2);
    } else
    {
#ifdef DEBUG
        if (gDbgLvl >= gVfpLvl)
        {
            WCHAR wszTemp[256];

            MySprintfW (wszTemp,
                        sizeof (wszTemp),
                       L"MPFNS(ReAlloc):  %p (%u) to %p (%u)",
                        lpMem,
                        old_size,
                        lpMemRes,
                        new_size);
            DbgMsgW (wszTemp);
        } // End IF
#endif
    } // End IF/ELSE

    return lpMemRes;
} // End mp_realloc


//***************************************************************************
//  $mp_free
//
//  Multi Precision Free function
//***************************************************************************

void mp_free
    (LPVOID lpMem,          // Ptr to start of memory block
     size_t size)           // Size         ...

{
#ifdef DEBUG
    MyHeapFree (GetProcessHeap (),
                0,
                lpMem);
#else
    dlfree (lpMem);
#endif
#ifdef DEBUG
    if (gDbgLvl >= gVfpLvl)
    {
        WCHAR wszTemp[256];

        MySprintfW (wszTemp,
                    sizeof (wszTemp),
                   L"MPFNS(Free):     %p (%u)",
                    lpMem,
                    size);
        DbgMsgW (wszTemp);
    } // End IF
#endif
} // End mp_free


//***************************************************************************
//  $mpz_invalid
//***************************************************************************

mpir_ui mpz_invalid
    (enum MP_ENUM mp_enum,
     mpz_t   rop,
     mpz_t   op1,
     mpz_t   op2,
     mpz_t   op3,
     mpir_ui a,
     mpir_ui b)

{
    UBOOL bSameSign;            // TRUE iff the infinities are the same sign

    // Split cases based upon the MP_ENUM value
    switch (mp_enum)
    {
        case MP_ADD:        // op1 and op2 are infinity of opposite signs
        case MP_SUB:        // op1 and op2 are infinite of the same sign
            mpz_QuadICValue (op1,
                             ICNDX_InfSUBInf,
                             op2,
                             rop,
                             FALSE);
            break;

        case MP_DIV:        // op1 and op2 are infinite
            bSameSign = (mpz_sgn (op1) EQ mpz_sgn (op2));

            mpz_QuadICValue (op1,
                             bSameSign ? ICNDX_PiDIVPi
                                       : ICNDX_NiDIVPi,
                             op2,
                             rop,
                             FALSE);
            break;

        case MP_MUL             :
        case MP_CDIV_Q          :
        case MP_CDIV_R          :
        case MP_CDIV_QR         :
        case MP_CDIV_R_UI       :
        case MP_CDIV_QR_UI      :
        case MP_CDIV_R_2EXP     :
        case MP_FDIV_Q          :
        case MP_FDIV_R          :
        case MP_FDIV_QR         :
        case MP_FDIV_R_UI       :
        case MP_FDIV_QR_UI      :
        case MP_FDIV_R_2EXP     :
        case MP_TDIV_Q          :
        case MP_TDIV_R          :
        case MP_TDIV_QR         :
        case MP_TDIV_R_UI       :
        case MP_TDIV_QR_UI      :
        case MP_TDIV_R_2EXP     :
        case MP_SQRT            :
        case MP_RELDIFF         :
        case MP_AND             :
        case MP_IOR             :
        case MP_XOR             :
        case MP_COM             :
        case MP_GCD             :
        case MP_LCM             :
        case MP_POW_UI          :
        case MP_UI_POW_UI       :
        case MP_PosMODNi        :
        case MP_NegMODNi        :
        case MP_PosMODPi        :
        case MP_NegMODPi        :
        case MP_NiMODPos        :
        case MP_NiMODNeg        :
        case MP_PiMODPos        :
        case MP_PiMODNeg        :
        case MP_MODUINi         :
        case MP_MODUIPi         :
        case MP_DIVISIBLE_P     :
        case MP_DIVISIBLE_UI_P  :
        case MP_DIVISIBLE_2EXP_P:
            DbgStop ();             // ***FIXME***

            break;

        defstop
            break;
    } // End SWITCH

    return 0;       // To keep the compiler happy
} // End mpz_invalid


//***************************************************************************
//  $mpq_invalid
//***************************************************************************

mpir_ui mpq_invalid
    (enum MP_ENUM mp_enum,
     mpq_t   rop,
     mpq_t   op1,
     mpq_t   op2,
     mpq_t   op3,
     mpir_ui d)

{
    UBOOL bSameSign;            // TRUE iff the infinities are the same sign

    // Split cases based upon the MP_ENUM value
    switch (mp_enum)
    {
        case MP_ADD:        // op1 and op2 are infinity of opposite signs
        case MP_SUB:        // op1 and op2 are infinite of the same sign
            mpq_QuadICValue (op1,
                             ICNDX_InfSUBInf,
                             op2,
                             rop,
                             FALSE);
            break;

        case MP_DIV:        // op1 and op2 are infinite
            bSameSign = (mpq_sgn (op1) EQ mpq_sgn (op2));

            mpq_QuadICValue (op1,
                             bSameSign ? ICNDX_PiDIVPi
                                       : ICNDX_NiDIVPi,
                             op2,
                             rop,
                             FALSE);
            break;

        defstop
            break;
    } // End SWITCH

    return 0;       // To keep the compiler happy
} // End mpq_invalid


//***************************************************************************
//  $mpfr_invalid
//***************************************************************************

mpir_ui mpfr_invalid
    (enum MP_ENUM mp_enum,
     mpfr_t  rop,
     mpfr_t  op1,
     mpfr_t  op2,
     mpfr_t  op3,
     mpir_ui d)

{
    UBOOL bSameSign;            // TRUE iff the infinities are the same sign

    // Split cases based upon the MP_ENUM value
    switch (mp_enum)
    {
        case MP_ADD:        // op1 and op2 are infinity of opposite signs
        case MP_SUB:        // op1 and op2 are infinite of the same sign
            mpfr_QuadICValue (op1,
                              ICNDX_InfSUBInf,
                              op2,
                              rop,
                              FALSE);
            break;

        case MP_DIV:        // op1 and op2 are infinite
            bSameSign = (mpfr_sgn (op1) EQ mpfr_sgn (op2));

            mpfr_QuadICValue (op1,
                              bSameSign ? ICNDX_PiDIVPi
                                        : ICNDX_NiDIVPi,
                              op2,
                              rop,
                              FALSE);
            break;

        case MP_RELDIFF:    // op1 is infinite and op2 might be, too
                            // Returns (abs (op1 - op2)) / op1
            mpfr_sub (rop, op1, op2, MPFR_RNDN);
            mpfr_abs (rop, rop,      MPFR_RNDN);
            mpfr_div (rop, rop, op1, MPFR_RNDN);

            break;

        case MP_SQRT:       // op1 is negative infinity
                            // Handled in <PrimFnMonRootVisV> and
                            //            <PrimFnMonRootVisVvV>
            Myf_clear (op1);

            RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

            break;

        defstop
            break;
    } // End SWITCH

    return 0;       // To keep the compiler happy
} // End mpfr_invalid


//***************************************************************************
//  MPZ Functions
//***************************************************************************

//***************************************************************************
//  $mpz_QuadICValue
//
//  Return the appropriate []IC value
//***************************************************************************

LPAPLMPI mpz_QuadICValue
    (LPAPLMPI   aplMpiLft,          // Left arg
     IC_INDICES icIndex,            // []IC index
     LPAPLMPI   aplMpiRht,          // Right arg
     LPAPLMPI   mpzRes,             // Result
     UBOOL      bNegate)            // TRUE iff we should negate result

{
    switch (GetQuadICValue (icIndex))
    {
        case ICVAL_NEG1:
            // Initialize the result to -1
            mpz_init_set_si (mpzRes, -1);

            break;

        case ICVAL_ZERO:
            // Initialize the result to 0
            mpz_init (mpzRes);

            break;

        case ICVAL_ONE:
            // Initialize the result to 1
            mpz_init_set_si (mpzRes,  1);

            break;

        case ICVAL_DOMAIN_ERROR:
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

        case ICVAL_POS_INFINITY:
            mpz_init_set (mpzRes, &mpzPosInfinity);

            break;

        case ICVAL_NEG_INFINITY:
            mpz_init_set (mpzRes, &mpzNegInfinity);

            break;

        case ICVAL_LEFT:
            mpz_init_set (mpzRes, aplMpiLft);

            break;

        case ICVAL_RIGHT:
            mpz_init_set (mpzRes, aplMpiRht);

            break;

        defstop
            break;
    } // End SWITCH

    // If we should negate, ...
    if (bNegate)
        mpz_neg (mpzRes, mpzRes);

    return mpzRes;
} // End mpz_QuadICValue


//***************************************************************************
//  $mpz_init_set_fr
//
//  Save an APLVFP value as an MP Integer
//***************************************************************************

void mpz_init_set_fr
    (mpz_ptr  dest,
     LPAPLVFP val)

{
    mpz_init (dest);
    mpz_set_fr (dest, val, MPFR_RNDN);
} // End mpz_init_set_fr


#if FALSE
//***************************************************************************
//  $mpz_get_sa
//
//  Convert an APLMPI to an APLINT
//***************************************************************************

APLINT mpz_get_sa
    (mpz_ptr src,               // Ptr to source value
     LPUBOOL lpbRet)            // Ptr to TRUE iff result is valid (may be NULL)

{
    UBOOL        bRet;
    APLUINTUNION dwSplit;

    if (lpbRet EQ NULL)
        lpbRet = &bRet;
    // Check the range
    *lpbRet = (0 <= mpz_cmp (src, &mpzMinInt)
            &&      mpz_cmp (src, &mpzMaxInt) <= 0);
#if GMP_LIMB_BITS == 32
    dwSplit.dwords[0] = mpz_getlimbn (src, 0);
    dwSplit.dwords[1] = mpz_getlimbn (src, 1);
#else
    dwSplit.aplInt    = mpz_getlimbn (src, 0);
#endif
    if (mpz_sgn (src) < 0)
        return -dwSplit.aplInt;
    else
        return  dwSplit.aplInt;
} // End mpz_get_sa
#endif


//***************************************************************************
//  $mpz_cmp_sx
//
//  Compare an MP Integer with an APLINT value
//***************************************************************************

int mpz_cmp_sx
    (mpz_ptr dest,
     APLINT  val)

{
    MP_INT aplTmp = {0};
    int    iRes;

    mpz_init_set_sx (&aplTmp, val);
    iRes = mpz_cmp (dest, &aplTmp);
    mpz_clear (&aplTmp);

    return iRes;
} // End mpz_cmp_sx


//***************************************************************************
//  $mpz_fits_sx_p
//
//  Does the arg fit in a 64-bit signed integer?
//***************************************************************************

UBOOL mpz_fits_sx_p
    (LPAPLMPI lpaplMPI)             // The number to check

{
    return (mpz_cmp_sx (lpaplMPI, MAX_APLINT) <= 0
         && mpz_cmp_sx (lpaplMPI, MIN_APLINT) >= 0);
} // End mpz_fits_sx_p


//***************************************************************************
//  $mpz_next_prime
//
//  Return the value of the next likely prime
//***************************************************************************

UBOOL mpz_next_prime
    (LPAPLMPI      mpzRes,          // The result
     LPAPLMPI      mpzArg,          // The starting number
     LPUBOOL       lpbCtrlBreak,    // Ptr to Ctrl-Break flag
     LPPERTABDATA  lpMemPTD)        // Ptr to PerTabData global memory

{
    // Copy the arg
    mpz_set (mpzRes, mpzArg);

    // Increment to the previous odd number
    mpz_add_ui (mpzRes, mpzRes, mpz_odd_p (mpzRes) ? 2 : 1);

    while (!mpz_likely_prime_p (mpzRes, lpMemPTD->randState, 0))
    {
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        mpz_add_ui (mpzRes, mpzRes, 2);
    } // End WHILE

    return TRUE;
ERROR_EXIT:
    return FALSE;
} // End mpz_next_prime


//***************************************************************************
//  $mpz_prev_prime
//
//  Return the value of the previous likely prime
//***************************************************************************

UBOOL mpz_prev_prime
    (LPAPLMPI      mpzRes,          // The result
     LPAPLMPI      mpzArg,          // The starting number
     LPUBOOL       lpbCtrlBreak,    // Ptr to Ctrl-Break flag
     LPPERTABDATA  lpMemPTD)        // Ptr to PerTabData global memory

{
    // Copy the arg
    mpz_set (mpzRes, mpzArg);

    // Decrement to the previous odd number
    mpz_sub_ui (mpzRes, mpzRes, mpz_odd_p (mpzRes) ? 2 : 1);

    while (!mpz_likely_prime_p (mpzRes, lpMemPTD->randState, 0))
    {
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        mpz_sub_ui (mpzRes, mpzRes, 2);
    } // End WHILE

    return TRUE;
ERROR_EXIT:
    return FALSE;
} // End mpz_prev_prime


//***************************************************************************
//  $Myz_init
//***************************************************************************

void Myz_init
    (LPAPLMPI mpzVal)

{
#ifdef DEBUG
    if (mpzVal->_mp_d NE NULL)
        DbgStop ();
#endif
    mpz_init (mpzVal);
} // End Myz_init


//***************************************************************************
//  $Myz_clear
//***************************************************************************

void Myz_clear
    (MP_INT *mpzVal)

{
    if (mpzVal->_mp_d NE NULL)
    {
        mpz_clear (mpzVal);
        mpzVal->_mp_d = NULL;
    } // End IF
} // End Myz_clear


//***************************************************************************
//  MPQ Functions
//***************************************************************************

//***************************************************************************
//  $mpq_QuadICValue
//
//  Return the appropriate []IC value
//***************************************************************************

LPAPLRAT mpq_QuadICValue
    (LPAPLRAT   aplRatLft,          // Left arg
     IC_INDICES icIndex,            // []IC index
     LPAPLRAT   aplRatRht,          // Right arg
     LPAPLRAT   mpqRes,             // Result
     UBOOL      bNegate)            // TRUE iff we should negate result

{
    switch (GetQuadICValue (icIndex))
    {
        case ICVAL_NEG1:
            // Initialize the result to -1
            mpq_init_set_si (mpqRes, -1, 1);

            break;

        case ICVAL_ZERO:
            // Initialize the result to 0
            mpq_init (mpqRes);

            break;

        case ICVAL_ONE:
            // Initialize the result to 1
            mpq_init_set_si (mpqRes,  1, 1);

            break;

        case ICVAL_DOMAIN_ERROR:
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

        case ICVAL_POS_INFINITY:
            mpq_init_set (mpqRes, &mpqPosInfinity);

            break;

        case ICVAL_NEG_INFINITY:
            mpq_init_set (mpqRes, &mpqNegInfinity);

            break;

        case ICVAL_LEFT:
            mpq_init_set (mpqRes, aplRatLft);

            break;

        case ICVAL_RIGHT:
            mpq_init_set (mpqRes, aplRatRht);

            break;

        defstop
            break;
    } // End SWITCH

    // If we should negate, ...
    if (bNegate)
        mpq_neg (mpqRes, mpqRes);

    return mpqRes;
} // End mpq_QuadICValue


//***************************************************************************
//  $mpq_init_set
//
//  Save a Rational value as Rational
//***************************************************************************

void mpq_init_set
    (mpq_ptr dest,
     mpq_ptr val)

{
    mpq_init (dest);
    mpq_set (dest, val);
} // End mpq_init_set


//***************************************************************************
//  $mpq_init_set_sx
//
//  Save an APLINT value as a Rational
//***************************************************************************

void mpq_init_set_sx
    (mpq_ptr dest,
     APLINT  num,
     APLUINT den)

{
    mpq_init (dest);
    mpq_set_sx (dest, num, den);
} // End mpq_init_set_sx


//***************************************************************************
//  $mpq_init_set_si
//
//  Save an int value as a Rational
//***************************************************************************

void mpq_init_set_si
    (mpq_ptr dest,
     int     num,
     UINT    den)

{
    mpq_init (dest);
    mpq_set_si (dest, num, den);
} // End mpq_init_set_si


//***************************************************************************
//  $mpq_init_set_ui
//
//  Save an unsigned int value as a Rational
//***************************************************************************

void mpq_init_set_ui
    (mpq_ptr dest,
     mpir_ui num,
     mpir_ui den)

{
    mpq_init (dest);
    mpq_set_ui (dest, num, den);
} // End mpq_init_set_si


//***************************************************************************
//  $mpq_init_set_d
//
//  Save an APLFLOAT value as a Rational
//***************************************************************************

void mpq_init_set_d
    (mpq_ptr  dest,
     APLFLOAT val)

{
    mpq_init (dest);
    mpq_set_d (dest, val);
} // End mpq_init_set_d


//***************************************************************************
//  $mpq_init_set_fr
//
//  Save a VFP value as a Rational
//***************************************************************************

void mpq_init_set_fr
    (mpq_ptr  dest,
     mpfr_ptr src)

{
    mpf_t mpfTmp = {0};

    // Initialize the dest & temp
    mpq_init (dest);
    mpf_init (mpfTmp);

    // Convert the MPFR to an MPF
    mpfr_get_f (mpfTmp, src, MPFR_RNDN);

    // Convert the MPF to a RAT
    mpq_set_f (dest, mpfTmp);

    // We no longer need this storage
    mpf_clear (mpfTmp);
} // End mpq_init_set_fr


//***************************************************************************
//  $mpq_init_set_z
//
//  Save a MPI value as a Rational
//***************************************************************************

void mpq_init_set_z
    (mpq_ptr dest,
     mpz_ptr src)

{
    mpq_init (dest);
    mpq_set_z (dest, src);
} // End mpq_init_set_z


//***************************************************************************
//  $mpq_init_set_str
//
//  Save a string value as a Rational
//***************************************************************************

void mpq_init_set_str
    (mpq_ptr dest,
     char   *str,
     int     base)

{
    mpq_init (dest);
    mpq_set_str (dest, str, base);
} // End mpq_init_set_str


//***************************************************************************
//  $mpq_get_sx
//
//  Convert an APLRAT to an APLINT
//***************************************************************************

APLINT mpq_get_sx
    (mpq_ptr src,           // Ptr to source value
     LPUBOOL lpbRet)        // TRUE iff the result is valid (may be NULL)

{
    APLMPI mpzDiv = {0};
    APLINT aplInteger;
    UBOOL  bRet;

    if (lpbRet EQ NULL)
        lpbRet = &bRet;
    // Check the range
    *lpbRet = (0 <= mpz_cmp (mpq_numref (src), &mpzMinInt)
            &&      mpz_cmp (mpq_numref (src), &mpzMaxInt) <= 0)
            && mpq_integer_p (src);
    // Initialize the quotient
    mpz_init (&mpzDiv);

    // Divide the numerator by the denominator
    mpz_fdiv_q (&mpzDiv, mpq_numref (src), mpq_denref (src));

    // Convert the MPI to an integer
    aplInteger = mpz_get_sx (&mpzDiv);

    // We no longer need this storage
    Myz_clear (&mpzDiv);

    return aplInteger;
} // End mpq_get_sx


//***************************************************************************
//  $mpq_get_ctsx
//
//  Convert an APLRAT to an APLINT within []CT
//***************************************************************************

APLINT _mpq_get_ctsx
    (mpq_ptr  src,          // Ptr to source value
     APLFLOAT fQuadCT,      // []CT
     LPUBOOL  lpbRet,       // TRUE iff the result is valid (may be NULL)
     UBOOL    bIntegerTest) // TRUE iff this is an integer test

{
    APLVFP mpfSrc  = {0};
    APLINT aplInt;

    // Initialize and convert the RAT to a VFP
    mpfr_init_set_q (&mpfSrc, src, MPFR_RNDN);

    // Use the MPFR routine
    aplInt = _mpfr_get_ctsx (&mpfSrc, fQuadCT, lpbRet, bIntegerTest);

    // We no longer need this storage
    Myf_clear (&mpfSrc);

    return aplInt;
} // End _mpq_get_ctsx


//***************************************************************************
//  $mpq_set_sx
//
//  Save an APLINT value as a Rational
//***************************************************************************

void mpq_set_sx
    (mpq_ptr dest,
     APLINT  num,
     APLUINT den)

{
    MP_INT mpzNum = {0},
           mpzDen = {0};

    Assert (den NE 0);

    // If the numerator is zero, ...
    if (num EQ 0)
        mpq_set_ui (dest, 0, 1);
    else
    {
        // Initialize the numerator and denominator
        mpz_set_sx (mpq_numref (dest), num);
        mpz_set_sx (mpq_denref (dest), den);

        // Only if we need to canonicalize, ...
        if (den NE 1)
            // Canonicalize the Rational
            mpq_canonicalize (dest);
    } // End IF
} // End mpq_set_sx


//***************************************************************************
//  $mpq_sub_ui
//***************************************************************************

void mpq_sub_ui
    (mpq_ptr dest,
     mpq_ptr src,
     mpir_ui num,
     mpir_ui den)

{
    APLRAT mpqTmp = {0};

    // Initialize the temp
    mpq_init (&mpqTmp);

    // Save the UINT as a RAT
    mpq_set_ui (&mpqTmp, num, den);

    // Subtract the UINT
    mpq_sub (dest, src, &mpqTmp);

    // We no longer need this storage
    Myq_clear (&mpqTmp);
} // End mpq_sub_ui


//***************************************************************************
//  $mpq_floor
//***************************************************************************

void mpq_floor
    (mpq_ptr dest,
     mpq_ptr src)

{
    // Divide the numerator by the denominator
    mpz_fdiv_q (mpq_numref (dest), mpq_numref (src), mpq_denref (src));

    // Set the denominator to 1
    mpz_set_ui (mpq_denref (dest), 1);
} // End mpq_floor


//***************************************************************************
//  $mpq_ceil
//***************************************************************************

void mpq_ceil
    (mpq_ptr dest,
     mpq_ptr src)

{
    // Divide the numerator by the denominator
    mpz_cdiv_q (mpq_numref (dest), mpq_numref (src), mpq_denref (src));

    // Set the denominator to 1
    mpz_set_ui (mpq_denref (dest), 1);
} // End mpq_ceil


//***************************************************************************
//  $mpq_mod
//
//  Calculate aplOpr mod (aplMod)
//***************************************************************************

void mpq_mod
    (mpq_ptr dest,          // Destination
     mpq_ptr aplOpr,        // Operand
     mpq_ptr aplMod)        // Modulus

{
    APLRAT aplTmp = {0};

    // Initialize the temp
    mpq_init (&aplTmp);

    mpq_div   (&aplTmp,  aplOpr,  aplMod);      // T = L / R
    mpq_floor (&aplTmp, &aplTmp);               // T = floor (T)
    mpq_mul   (&aplTmp,  aplMod, &aplTmp);      // T = R * T
    mpq_sub   (dest   ,  aplOpr, &aplTmp);      // Z = L - T

    // We no longer need this storage
    Myq_clear (&aplTmp);
} // End mpq_mod


//***************************************************************************
//  $mpq_cmp_ct
//
//  Compare two RATs relative to a given comparison tolerance
//
//  Return +1 if Lft >  Rht
//          0 if Lft EQ Rht
//         -1 if Lft <  Rht
//***************************************************************************

int mpq_cmp_ct
    (APLRAT   aplRatLft,
     APLRAT   aplRatRht,
     APLFLOAT fQuadCT)

{
    APLVFP mpfLft = {0},
           mpfRht = {0};
    UBOOL  bRet;

    // So as to avoid dividing by zero, if neither arg is zero, ...
    if (!IsMpq0 (&aplRatLft)
     && !IsMpq0 (&aplRatRht))
    {
        // Initialize the temps to 0
        mpfr_init0 (&mpfLft);
        mpfr_init0 (&mpfRht);

        // Copy the RAT as a VFP
        mpfr_set_q (&mpfLft, &aplRatLft, MPFR_RNDN);
        mpfr_set_q (&mpfRht, &aplRatRht, MPFR_RNDN);

        // Compare the two VFPs relative to []CT
        bRet = mpfr_cmp_ct (mpfLft, mpfRht, fQuadCT) EQ 0;

        // We no longer need this storage
        Myf_clear (&mpfRht);
        Myf_clear (&mpfLft);

        if (bRet)
            return 0;
    } // End IF

    return mpq_cmp (&aplRatLft, &aplRatRht);
} // End mpq_cmp_ct


//***************************************************************************
//  $Myq_init
//***************************************************************************

void Myq_init
    (LPAPLRAT mpqVal)

{
#ifdef DEBUG
    if (mpqVal->_mp_num._mp_d NE NULL)
        DbgStop ();
#endif
    mpq_init (mpqVal);
} // End Myq_init


//***************************************************************************
//  $Myq_clear
//
//  Free a RAT unless already clear
//***************************************************************************

void Myq_clear
    (LPAPLRAT mpqVal)

{
    if (mpq_numref (mpqVal)->_mp_d NE NULL)
    {
        mpz_clear (mpq_numref (mpqVal));
        mpqVal->_mp_num._mp_d = NULL;
    } // End IF

    if (mpq_denref (mpqVal)->_mp_d NE NULL)
    {
        mpz_clear (mpq_denref (mpqVal));
        mpqVal->_mp_den._mp_d = NULL;
    } // End IF
} // End Myq_clear


//***************************************************************************
//  $mpq_integer_p
//
//  Return TRUE iff the numerator of op is evenly divisible by its denominator
//***************************************************************************

int mpq_integer_p
    (mpq_t op)                  // Source

{
    if (mpq_inf_p (op))
        return TRUE;
    else
        return mpz_divisible_p (mpq_numref (op), mpq_denref (op));
} // mpq_integer_p


//***************************************************************************
//  MPFR Functions
//***************************************************************************

//***************************************************************************
//  $mpfr_QuadICValue
//
//  Return the appropriate []IC value
//***************************************************************************

LPAPLVFP mpfr_QuadICValue
    (LPAPLVFP   aplVfpLft,          // Left arg
     IC_INDICES icIndex,            // []IC index
     LPAPLVFP   aplVfpRht,          // Right arg
     LPAPLVFP   mpfRes,             // Result
     UBOOL      bNegate)            // TRUE iff we should negate result

{
    switch (GetQuadICValue (icIndex))
    {
        case ICVAL_NEG1:
            // Initialize the result to -1
            mpfr_init_set_si (mpfRes, -1, MPFR_RNDN);

            break;

        case ICVAL_ZERO:
            // Initialize the result to 0
            mpfr_init0 (mpfRes);

            break;

        case ICVAL_ONE:
            // Initialize the result to 1
            mpfr_init_set_si (mpfRes,  1, MPFR_RNDN);

            break;

        case ICVAL_DOMAIN_ERROR:
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

        case ICVAL_POS_INFINITY:
            mpfr_init_set (mpfRes, &mpfPosInfinity, MPFR_RNDN);

            break;

        case ICVAL_NEG_INFINITY:
            mpfr_init_set (mpfRes, &mpfNegInfinity, MPFR_RNDN);

            break;

        case ICVAL_LEFT:
            mpfr_init_copy (mpfRes, aplVfpLft);

            break;

        case ICVAL_RIGHT:
            mpfr_init_copy (mpfRes, aplVfpRht);

            break;

        defstop
            break;
    } // End SWITCH

    // If we should negate, ...
    if (bNegate)
        mpfr_neg (mpfRes, mpfRes, MPFR_RNDN);

    return mpfRes;
} // End mpfr_QuadICValue


//***************************************************************************
//  $mpfr_copy
//
//  Save an MPFR value as a Variable FP with the same precision
//***************************************************************************

void mpfr_copy
    (mpfr_ptr dest,
     mpfr_ptr val)

{
    mpfr_set_prec (dest, mpfr_get_prec (val));
    mpfr_set      (dest, val, MPFR_RNDN);
} // End mpfr_copy


//***************************************************************************
//  $mpfr_init_copy
//
//  Save an MPFR value as a Variable FP with the same precision
//***************************************************************************

void mpfr_init_copy
    (mpfr_ptr dest,
     mpfr_ptr val)

{
    mpfr_init2 (dest, mpfr_get_prec (val));
    mpfr_set   (dest, val, MPFR_RNDN);
} // End mpfr_init_copy


//***************************************************************************
//  $mpfr_init_set
//
//  Save a VFP value as a Variable FP
//***************************************************************************

void mpfr_init_set
    (mpfr_ptr   dest,
     mpfr_ptr   src,
     mpfr_rnd_t rnd)

{
    mpfr_init (dest);       // OK not to be mpfr_init0
    mpfr_set (dest, src, rnd);
} // End mpfr_init_set


//***************************************************************************
//  $mpfr_init_set_sx
//
//  Save an APLINT value as a Variable FP
//***************************************************************************

void mpfr_init_set_sx
    (mpfr_ptr   dest,
     APLINT     val,
     mpfr_rnd_t rnd)

{
    mpfr_init (dest);       // OK not to be mpfr_init0
    mpfr_set_sx (dest, val, rnd);
} // End mpfr_init_set_sx


//***************************************************************************
//  $mpfr_init_set_ui
//
//  Save a UINT value as a Variable FP
//***************************************************************************

void mpfr_init_set_ui
    (mpfr_ptr          dest,
     unsigned long int val,
     mpfr_rnd_t        rnd)

{
    mpfr_init (dest);       // OK not to be mpfr_init0 to avoid recursion
    mpfr_set_ui (dest, val, rnd);
} // End mpfr_init_set_ui


//***************************************************************************
//  $mpfr_init_set_si
//
//  Save an INT value as a Variable FP
//***************************************************************************

void mpfr_init_set_si
    (mpfr_ptr   dest,
     long int   val,
     mpfr_rnd_t rnd)

{
    mpfr_init (dest);       // OK not to be mpfr_init0
    mpfr_set_si (dest, val, rnd);
} // End mpfr_init_set_si


//***************************************************************************
//  $mpfr_init_set_d
//
//  Save a FLT value as a Variable FP
//***************************************************************************

void mpfr_init_set_d
    (mpfr_ptr   dest,
     APLFLOAT   val,
     mpfr_rnd_t rnd)

{
    mpfr_init (dest);       // OK not to be mpfr_init0
    mpfr_set_d (dest, val, rnd);
} // End mpfr_init_set_d


//***************************************************************************
//  $mpfr_init_set_z
//
//  Save a MPZ value as a Variable FP
//***************************************************************************

void mpfr_init_set_z
    (mpfr_ptr   dest,
     LPAPLMPI   val,
     mpfr_rnd_t rnd)

{
    mpfr_init (dest);       // OK not to be mpfr_init0
    mpfr_set_z (dest, val, rnd);
} // End mpfr_init_set_z


//***************************************************************************
//  $mpfr_init_set_q
//
//  Save a RAT value as a Variable FP
//***************************************************************************

void mpfr_init_set_q
    (mpfr_ptr   dest,
     LPAPLRAT   val,
     mpfr_rnd_t rnd)

{
    mpfr_init (dest);       // OK not to be mpfr_init0
    mpfr_set_q (dest, val, rnd);
} // End mpfr_init_set_q


//***************************************************************************
//  $mpfr_get_sx
//
//  Convert a VFP to an APLINT
//***************************************************************************

APLINT mpfr_get_sx
    (mpfr_ptr src,          // Ptr to source value
     LPUBOOL lpbRet)        // TRUE iff the result is valid (may be NULL)

{
     UBOOL bRet;

     if (lpbRet EQ NULL)
         lpbRet = &bRet;
     // See if it fits
     *lpbRet = mpfr_fits_intmax_p (src, MPFR_RNDN);

     // If the fractional part is zero and the integer part is in range, ...
     if (*lpbRet)
         return mpfr_get_sj (src, MPFR_RNDN);
     else
         // Return a known value
         return 0;
} // End mpfr_get_sx


//***************************************************************************
//  $mpfr_get_sctsx
//
//  Convert an APLVFP to an APLINT within system []CT
//***************************************************************************

APLINT mpfr_get_sctsx
    (mpfr_ptr src,          // Ptr to source value
     LPUBOOL  lpbRet)       // TRUE iff the result is valid (may be NULL)

{
    return _mpfr_get_ctsx (src, SYS_CT, lpbRet, TRUE);
} // End mpfr_get_sctsx


//***************************************************************************
//  $mpfr_get_ctsx
//
//  Convert an APLVFP to an APLINT within []CT
//***************************************************************************

APLINT _mpfr_get_ctsx
    (mpfr_ptr src,          // Ptr to source value
     APLFLOAT fQuadCT,      // []CT
     LPUBOOL  lpbRet,       // TRUE iff the result is valid (may be NULL)
     UBOOL    bIntegerTest) // TRUE iff this is an integer test

{
    APLVFP mpfTmp1 = {0},
           mpfTmp2 = {0},
           mpfSrc  = {0};
    APLINT aplInt = 0;
    UBOOL  bRet = TRUE;

    if (lpbRet EQ NULL)
        lpbRet = &bRet;

    // Handle special case of 0 as <mpfr_reldiff> returns 1 (= 0{div}0)
    if (mpfr_zero_p (src))
        *lpbRet = TRUE;
    else
    // Handle special case of infinity as <mpfr_get_sx> fails
    if (mpfr_inf_p (src))
        longjmp (heapFull, 3);
    else
    {
        // Initialize the temps to 0
        mpfr_init0 (&mpfTmp1);
        mpfr_init0 (&mpfTmp2);
        mpfr_init0 (&mpfSrc);

        // Copy the source
        mpfr_copy (&mpfSrc, src);

        // Get the floor
        mpfr_floor (&mpfTmp1, src);

        // Get the ceil
        mpfr_ceil  (&mpfTmp2, src);

        // Compare the number and its floor
        if (_mpfr_cmp_ct (&mpfSrc, &mpfTmp1, fQuadCT, bIntegerTest) EQ 0)
            // Return the floor
            aplInt = mpfr_get_sx (&mpfTmp1, lpbRet);
        else
        // Compare the number and its ceiling
        if (_mpfr_cmp_ct (&mpfSrc, &mpfTmp2, fQuadCT, bIntegerTest) EQ 0)
            // Return the ceiling
            aplInt = mpfr_get_sx (&mpfTmp2, lpbRet);
        else
        {
            // Return the floor even though it isn't within []CT
            aplInt = mpfr_get_sx (&mpfTmp1, lpbRet);

            // Mark as not within []CT
            *lpbRet = FALSE;
        } // End IF/ELSE/...

        // We no longer need this storage
        Myf_clear (&mpfSrc);
        Myf_clear (&mpfTmp2);
        Myf_clear (&mpfTmp1);
    } // Endf IF/ELSE

    return aplInt;
} // End _mpfr_get_ctsx


//// //***************************************************************************
//// //  $mpfr_get_ux
//// //
//// //  Convert a VFP to an APLUINT
//// //***************************************************************************
////
//// APLUINT mpfr_get_ux
////     (mpfr_ptr src,             // Ptr to source value
////      LPUBOOL lpbRet)           // TRUE iff the result is valid (may be NULL)
////
//// {
////     UBOOL bRet;
////
////     if (lpbRet EQ NULL)
////         lpbRet = &bRet;
////     // See if it fits
////     *lpbRet = mpfr_fits_uintmax_p (src, MPFR_RNDN);
////
////     // If the fractional part is zero and the integer part is in range, ...
////     if (*lpbRet)
////         return mpfr_get_uj (src, MPFR_RNDN);
////     else
////         // Return a known value
////         return 0;
//// } // End mpfr_get_ux


//***************************************************************************
//  $mpfr_mod
//
//  Calculate aplLft % aplRht
//***************************************************************************

void mpfr_mod
    (mpfr_ptr dest,         // Destination
     mpfr_ptr aplOpr,       // Operand
     mpfr_ptr aplMod)       // Modulus

{
#if FALSE                   // ***FIXME*** -- Check rounding
    mpfr_t mpfrOpr = {0},
           mpfrMod = {0};

    // Copy the Opr & Mod as non-negative numbers
    mpfr_init_copy (mpfrOpr, aplOpr);
    mpfr_init_copy (mpfrMod, aplMod);

    if (mpfr_sgn (mpfrOpr) < 0)
        mpfr_neg (mpfrOpr, mpfrOpr, MPFR_RNDZ);
    if (mpfr_sgn (mpfrMod) < 0)
        mpfr_neg (mpfrMod, mpfrMod, MPFR_RNDZ);

    // Compute the modulus
    mpfr_fmod (dest, mpfrOpr, mpfrMod, MPFR_RNDZ);

    // If the arguments are of opposite sign
    //   and the result so far is non-zero,
    //   replace the result with its complement
    //   in the modulus.
    if ((mpfr_sgn (aplMod) > 0) NE (mpfr_sgn (aplOpr) > 0)
     && mpfr_sgn (dest) NE 0)
        mpfr_sub (dest, aplMod, dest, MPFR_RNDZ);

    // The sign of the result is the sign of the left arg
    if (mpfr_sgn (aplMod) < 0)
        mpfr_neg (dest, dest, MPFR_RNDZ);
#else
    // Due to precision limitations, the result of <mpfr_mod_sub>
    //   might not be less than the modulus, so we iterate until it is
    mpfr_mod_sub (dest, aplOpr, aplMod);
    while (mpfr_cmp_abs (dest, aplMod) > 0)
        mpfr_mod_sub (dest, dest  , aplMod);
#endif
} // End mpfr_mod


//***************************************************************************
//  $mpfr_mod_sub
//
//  Calculate aplLft % aplRht
//***************************************************************************

void mpfr_mod_sub
    (mpfr_ptr dest,         // Destination
     mpfr_ptr aplOpr,       // Operand
     mpfr_ptr aplMod)       // Modulus

{
////#define MOD_DEBUG

    APLVFP aplTmp = {0};
#if defined (DEBUG) && defined (MOD_DEBUG)
    char szTemp1[1024],
         szTemp2[1024];
    int  expptr1,
         expptr2;
#endif

    // Initialize the temp to 0
    mpfr_init0 (&aplTmp);

#if defined (DEBUG) && defined (MOD_DEBUG)
    mpfr_get_str (szTemp1, &expptr1, 10, 200, aplOpr, MPFR_RNDN);
    mpfr_get_str (szTemp2, &expptr2, 10, 200, aplMod, MPFR_RNDN);
#endif
    mpfr_div   (&aplTmp,  aplOpr,  aplMod, MPFR_RNDN);
#if defined (DEBUG) && defined (MOD_DEBUG)
    mpfr_get_str (szTemp1, &expptr1, 10, 200, &aplTmp, MPFR_RNDN);
#endif
    mpfr_floor (&aplTmp, &aplTmp);
#if defined (DEBUG) && defined (MOD_DEBUG)
    mpfr_get_str (szTemp1, &expptr1, 10, 200, &aplTmp, MPFR_RNDN);
#endif
    mpfr_mul   (&aplTmp,  aplMod, &aplTmp, MPFR_RNDN);
#if defined (DEBUG) && defined (MOD_DEBUG)
    mpfr_get_str (szTemp1, &expptr1, 10, 200, &aplTmp, MPFR_RNDN);
#endif
    mpfr_sub   (dest   ,  aplOpr, &aplTmp, MPFR_RNDN);
#if defined (DEBUG) && defined (MOD_DEBUG)
    mpfr_get_str (szTemp1, &expptr1, 10, 200, dest, MPFR_RNDN);
#endif

    // We no longer need this storage
    Myf_clear (&aplTmp);
} // End mpfr_mod_sub


//***************************************************************************
//  $mpfr_cmp_ct
//
//  Compare two VFPs relative to a given comparison tolerance
//
//  Return +1 if Lft >  Rht
//          0 if Lft EQ Rht
//         -1 if Lft <  Rht
//***************************************************************************

int _mpfr_cmp_ct
    (LPAPLVFP lpaplVfpLft,          // Ptr to left arg
     LPAPLVFP lpaplVfpRht,          // ...    right ...
     APLFLOAT fQuadCT,              // []CT
     UBOOL    bIntegerTest)         // TRUE iff this is an integer test

{
    int iRet;                       // Result of mpfr_cmp

////#define CT_DEBUG

#if defined (DEBUG) && defined (CT_DEBUG)
    WCHAR  wszTemp[1024];
    APLVFP mpfFmt = {0};

    strcpyW (wszTemp, L"Lft: "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], *lpaplVfpLft, 0) = WC_EOS; DbgMsgW (wszTemp);
    strcpyW (wszTemp, L"Rht: "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], *lpaplVfpRht, 0) = WC_EOS; DbgMsgW (wszTemp);
#endif
    // Compare 'em without tolerance
    iRet = mpfr_cmp (lpaplVfpLft, lpaplVfpRht);

    // If args are unequal,
    //   and []CT is non-zero,
    //   and neither arg is either infinity, ...
    if (iRet NE 0                               // Lft NE Rht
     && fQuadCT NE 0                            // []CT NE 0
     && !IsMpfInfinity (lpaplVfpLft)
     && !IsMpfInfinity (lpaplVfpRht))
    {
        APLVFP mpfLftAbs = {0},     // Absolute value of left arg
               mpfRhtAbs = {0};     // ...               right ...
        UINT   sgnLft,              // Left arg sign
               sgnRht;              // Right ...

        // Use an algorithm similar to the one in _CompareCT

        // Initialize the temps to 0
        mpfr_init0 (&mpfLftAbs);
        mpfr_init0 (&mpfRhtAbs);

        // Get the signs
        sgnLft = signumvfp (lpaplVfpLft);
        sgnRht = signumvfp (lpaplVfpRht);

        // Get the absolute values
        mpfr_abs (&mpfLftAbs, lpaplVfpLft, MPFR_RNDN);
        mpfr_abs (&mpfRhtAbs, lpaplVfpRht, MPFR_RNDN);

        // If this is an integer test, allow comparison with zero
        if (bIntegerTest
         && sgnLft EQ 0
         && mpfr_cmp_d (&mpfRhtAbs, fQuadCT) <= 0)
            iRet = 0;
        else
        if (bIntegerTest
         && sgnRht EQ 0
         && mpfr_cmp_d (&mpfLftAbs, fQuadCT) <= 0)
            iRet = 0;
        else
        if (!IsMpf0 (lpaplVfpLft)       // Lft NE 0
         && !IsMpf0 (lpaplVfpRht)       // Rht NE 0
         && sgnLft EQ sgnRht)           // Signs are the same, ...
        {
            APLVFP mpfCT     = {0},
                   mpfHoodLo = {0};

            // Initialize the temps to 0
            mpfr_init0 (&mpfCT);
            mpfr_init0 (&mpfHoodLo);

            // Convert the comparison tolerance
            mpfr_set_d (&mpfCT, fQuadCT, MPFR_RNDN);
#if defined (DEBUG) && defined (CT_DEBUG)
            strcpyW (wszTemp, L"CT:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], mpfCT, 0) = WC_EOS; DbgMsgW (wszTemp);
#endif
            // Calculate the low end of the left neighborhood of (|Rht)
////////////aplHoodLo = aplRhtAbs - aplRhtAbs * fQuadCT;
            mpfr_mul (&mpfHoodLo, &mpfRhtAbs, &mpfCT    , MPFR_RNDN);
            mpfr_sub (&mpfHoodLo, &mpfRhtAbs, &mpfHoodLo, MPFR_RNDN);
#if defined (DEBUG) && defined (CT_DEBUG)
            strcpyW (wszTemp, L"Lo1: "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], mpfHoodLo, 0) = WC_EOS; DbgMsgW (wszTemp);
            strcpyW (wszTemp, L"L1 : "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], mpfLftAbs, 0) = WC_EOS; DbgMsgW (wszTemp);
            strcpyW (wszTemp, L"R1 : "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], mpfRhtAbs, 0) = WC_EOS; DbgMsgW (wszTemp);
#endif
            // If (|Rht) is greater than (|Lft),
            // and (|Lft) is in the
            //    left-neighborhood of (|Rht) with CT, return 1
////////////if (aplHoodLo <= aplLftAbs
//////////// &&              aplLftAbs < aplRhtAbs)
////////////    return TRUE;
            if (mpfr_cmp (&mpfHoodLo, &mpfLftAbs)             <= 0
             && mpfr_cmp (            &mpfLftAbs, &mpfRhtAbs) <  0)
                iRet = 0;
            else
            {
                // Calculate the low end of the left neighborhood of (|Lft)
////////////////aplHoodLo = aplLftAbs - aplLftAbs * fQuadCT;
                mpfr_mul (&mpfHoodLo, &mpfLftAbs, &mpfCT    , MPFR_RNDN);
                mpfr_sub (&mpfHoodLo, &mpfLftAbs, &mpfHoodLo, MPFR_RNDN);
#if defined (DEBUG) && defined (CT_DEBUG)
            strcpyW (wszTemp, L"Lo2: "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], mpfHoodLo, 0) = WC_EOS; DbgMsgW (wszTemp);
            strcpyW (wszTemp, L"R2 : "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], mpfRhtAbs, 0) = WC_EOS; DbgMsgW (wszTemp);
            strcpyW (wszTemp, L"L2 : "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], mpfLftAbs, 0) = WC_EOS; DbgMsgW (wszTemp);
#endif
                // If (|Lft) is greater than (|Rht),
                // and (|Rht) is in the
                //    left-neighborhood of (|Lft) with CT, return 1
////////////////if (aplHoodLo <= aplRhtAbs
//////////////// &&              aplRhtAbs < aplLftAbs)
////////////////    return TRUE;
                if (mpfr_cmp (&mpfHoodLo, &mpfRhtAbs)             <= 0
                 && mpfr_cmp (            &mpfRhtAbs, &mpfLftAbs) <  0)
                    iRet = 0;
            } // End IF/ELSE

            // We no longer need this storage
            Myf_clear (&mpfHoodLo);
            Myf_clear (&mpfCT    );
        } // End IF/ELSE

        Myf_clear (&mpfRhtAbs);
        Myf_clear (&mpfLftAbs);
    } // End IF

    return iRet;
} // End _mpfr_cmp_ct


//***************************************************************************
//  $mpfr_ui_cmp
//
//  Compare an unsigned long int and a VFP
//***************************************************************************

int mpfr_ui_cmp
    (unsigned long int op1,     // Left arg
     mpfr_t             op2)     // Right arg

{
    return -mpfr_cmp_ui (op2, op1);
} // End mpfr_ui_cmp


//***************************************************************************
//  $mpfr_si_cmp
//
//  Compare a signed long int and a VFP
//***************************************************************************

int mpfr_si_cmp
    (signed long int op1,       // Left arg
     mpfr_t           op2)       // Right arg

{
    return -mpfr_cmp_si (op2, op1);
} // End mpfr_si_cmp


//***************************************************************************
//  $Myf_init
//***************************************************************************

void Myf_init
    (LPAPLVFP mpfVal)

{
#ifdef DEBUG
    if (mpfVal->_mpfr_d NE NULL)
        DbgStop ();
#endif
    mpfr_init0 (mpfVal);
} // End Myf_init


//***************************************************************************
//  $Myf_clear
//
//  Free a VFP unless already clear
//***************************************************************************

void Myf_clear
    (LPAPLVFP mpfVal)

{
    if (mpfVal->_mpfr_d NE NULL)
    {
        mpfr_clear (mpfVal);
        mpfVal->_mpfr_d = NULL;
    } // End IF
} // End Myf_clear


//***************************************************************************
//  End of File: mpfns.c
//***************************************************************************
